# Auto-updater called by Halo3.exe before launch.
# Exit 0 to continue launching, or 10 to relaunch after a successful update.
param(
    [Parameter(Mandatory = $true)]
    [string]$InstallDir,

    [string]$CurrentVersion = "unknown",
    [string]$Repo = "summers-aaron/halo-3",
    [switch]$Force
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

function Get-OrderedVersion([string]$Text) {
    $Match = [regex]::Match($Text, '^v?([0-9]+\.[0-9]+\.[0-9]+)(?:-([0-9]+)-g[0-9a-fA-F]+)?(?:-dirty)?\z')
    if ($Match.Success) {
        $Distance = if ($Match.Groups[2].Success) { $Match.Groups[2].Value } else { '0' }
        $Version = $null
        if ([version]::TryParse("$($Match.Groups[1].Value).$Distance", [ref]$Version)) {
            return $Version
        }
    }
}

$LocalVersion = Get-OrderedVersion $CurrentVersion
if ($null -eq $LocalVersion) { exit 0 }
if ($env:HALO3_UPDATE_REPO) { $Repo = $env:HALO3_UPDATE_REPO }
$ApiUrl = if ($env:HALO3_UPDATE_API_URL) {
    $env:HALO3_UPDATE_API_URL
} else {
    "https://api.github.com/repos/$Repo/releases/latest"
}

$ApiUri = $null
if (-not [uri]::TryCreate($ApiUrl, [UriKind]::Absolute, [ref]$ApiUri) -or
    ($ApiUri.Scheme -ne 'https' -and $ApiUri.Scheme -ne 'http')) {
    exit 0
}

$Token = $null
if ($ApiUri.Scheme -eq 'https' -and $ApiUri.Host -eq 'api.github.com' -and $ApiUri.IsDefaultPort) {
    $Token = if ($env:HALO3_UPDATE_TOKEN) { $env:HALO3_UPDATE_TOKEN } elseif ($env:HALO3_GITHUB_TOKEN) { $env:HALO3_GITHUB_TOKEN } else { $env:GITHUB_TOKEN }
}

# Use an explicit wall-clock wait because Windows PowerShell's web cmdlet
# timeout can be exceeded by DNS resolution.
$Client = $null
$Response = $null
try {
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
    Add-Type -AssemblyName System.Net.Http
    $Client = [Net.Http.HttpClient]::new()
    $Client.MaxResponseContentBufferSize = 1048576
    $Client.DefaultRequestHeaders.UserAgent.ParseAdd('halo3-pc-updater')
    $Client.DefaultRequestHeaders.Accept.ParseAdd('application/vnd.github+json')
    if ($Token) {
        $Client.DefaultRequestHeaders.Authorization = [Net.Http.Headers.AuthenticationHeaderValue]::new('Bearer', $Token)
    }

    $Request = $Client.GetAsync($ApiUri)
    if (-not $Request.Wait(3000)) { exit 0 }
    $Response = $Request.Result
    if (-not $Response.IsSuccessStatusCode) { exit 0 }
    $Release = $Response.Content.ReadAsStringAsync().GetAwaiter().GetResult() | ConvertFrom-Json
} catch {
    exit 0
} finally {
    if ($Response) { $Response.Dispose() }
    if ($Client) { $Client.Dispose() }
}

$LatestVersion = Get-OrderedVersion $Release.tag_name
if ($null -eq $LatestVersion -or $LatestVersion -le $LocalVersion) {
    if ($Force -and $LatestVersion -eq $LocalVersion) {
        Write-Host "halo3: already up to date ($CurrentVersion)"
    }
    exit 0
}

function Get-ReleaseAsset([string]$Name) {
    $Release.assets | Where-Object { $_.name -ceq $Name } | Select-Object -First 1
}

# Keep installer and portable installs in their original format.
$Package = $null
if (Test-Path -LiteralPath (Join-Path $InstallDir "Uninstall.exe") -PathType Leaf) {
    $Package = Get-ReleaseAsset "Halo3-Setup.exe"
}
if (-not $Package) { $Package = Get-ReleaseAsset "Halo3-Windows-Portable.zip" }
if (-not $Package) { $Package = Get-ReleaseAsset "Halo3-Windows.zip" }
if (-not $Package) { exit 0 }
$Checksum = Get-ReleaseAsset "$($Package.name).sha256"
if (-not $Checksum) { exit 0 }
$IsInstaller = $Package.name -ceq "Halo3-Setup.exe"

function Save-ReleaseAsset($Asset, [string]$Destination, [int]$Timeout) {
    $Headers = @{ "User-Agent" = "halo3-pc-updater"; "Accept" = "application/octet-stream" }
    $Url = $Asset.browser_download_url
    if ($Token) {
        $Url = $Asset.url
        $Headers["Authorization"] = "Bearer $Token"
    }
    $Uri = $null
    if (-not [uri]::TryCreate($Url, [UriKind]::Absolute, [ref]$Uri) -or
        ($Uri.Scheme -ne 'https' -and ($Uri.Scheme -ne 'http' -or $ApiUri.Scheme -ne 'http')) -or
        ($Token -and ($Uri.Host -ne 'api.github.com' -or -not $Uri.IsDefaultPort))) {
        throw "Invalid release asset URL"
    }
    Invoke-WebRequest -UseBasicParsing -Uri $Uri -Headers $Headers -OutFile $Destination -TimeoutSec $Timeout
}

$Lock = $null
$TempDir = $null
$Backups = [ordered]@{}
$Targets = [Collections.Generic.List[string]]::new()
$RestoreFailed = $false
$VersionPath = Join-Path $InstallDir "VERSION"

try {
    # The install directory also keeps staged moves on the same volume.
    $LockPath = Join-Path $InstallDir ".halo3-update.lock"
    try {
        $Lock = [IO.FileStream]::new($LockPath, [IO.FileMode]::OpenOrCreate, [IO.FileAccess]::ReadWrite,
            [IO.FileShare]::None, 1, [IO.FileOptions]::DeleteOnClose)
    } catch {
        exit 0
    }

    $InstalledVersion = Get-OrderedVersion (Get-Content -LiteralPath $VersionPath -Raw).Trim()
    if ($null -eq $InstalledVersion -or $LatestVersion -le $InstalledVersion) { exit 0 }

    $TempDir = Join-Path $InstallDir (".halo3-update-" + [guid]::NewGuid().ToString("N"))
    [IO.Directory]::CreateDirectory($TempDir) | Out-Null
    $ShaPath = Join-Path $TempDir $Checksum.name
    Save-ReleaseAsset $Checksum $ShaPath 5
    if ((Get-Item -LiteralPath $ShaPath).Length -gt 4096) { exit 0 }
    $Digest = [regex]::Match((Get-Content -LiteralPath $ShaPath -Raw).Trim(), '\A([0-9a-fA-F]{64})(?:[ \t]+[^\r\n]*)?\z')
    if (-not $Digest.Success) { exit 0 }

    $PackagePath = Join-Path $TempDir $Package.name
    Save-ReleaseAsset $Package $PackagePath 120
    $Actual = (Get-FileHash -LiteralPath $PackagePath -Algorithm SHA256).Hash
    if ($Actual -ne $Digest.Groups[1].Value) { exit 0 }

    $Stage = Join-Path $TempDir "stage"
    $BackupDir = Join-Path $TempDir "backup"
    [IO.Directory]::CreateDirectory($Stage) | Out-Null
    [IO.Directory]::CreateDirectory($BackupDir) | Out-Null
    if ($IsInstaller) {
        $Names = @("Halo3.exe", "bin", "SHADER_ABI", "BUILD_MANIFEST.txt", "Uninstall.exe", "VERSION")
        $BinDir = Join-Path $InstallDir "bin"
        if (Test-Path -LiteralPath $BinDir -PathType Container) {
            Copy-Item -LiteralPath $BinDir -Destination $Stage -Recurse -Force
        }
    } else {
        $ExtractDir = Join-Path $TempDir "extracted"
        Expand-Archive -LiteralPath $PackagePath -DestinationPath $ExtractDir
        $SourceRoot = $ExtractDir
        foreach ($Folder in @("Halo3-Windows-Portable", "Halo3-Windows")) {
            $WrappedRoot = Join-Path $ExtractDir $Folder
            if (Test-Path -LiteralPath $WrappedRoot -PathType Container) {
                $SourceRoot = $WrappedRoot
                break
            }
        }
        if (-not (Test-Path -LiteralPath (Join-Path $SourceRoot "Halo3.exe") -PathType Leaf) -or
            -not (Test-Path -LiteralPath (Join-Path $SourceRoot "bin/halo3.exe") -PathType Leaf)) {
            exit 0
        }
        $PackageVersion = Get-OrderedVersion (Get-Content -LiteralPath (Join-Path $SourceRoot "VERSION") -Raw).Trim()
        if ($null -eq $PackageVersion -or $PackageVersion -lt $LatestVersion) { exit 0 }

        $Entries = Get-ChildItem -LiteralPath $SourceRoot | Sort-Object { $_.Name -eq "VERSION" }
        $Names = @($Entries.Name)
        foreach ($Entry in $Entries) {
            $Existing = Join-Path $InstallDir $Entry.Name
            if ($Entry.PSIsContainer -and (Test-Path -LiteralPath $Existing -PathType Container)) {
                Copy-Item -LiteralPath $Existing -Destination $Stage -Recurse -Force
            }
            Copy-Item -LiteralPath $Entry.FullName -Destination $Stage -Recurse -Force
        }
    }

    # Rename the running launcher and back up the other package paths before
    # publishing anything. Keep the old launcher until Windows releases it.
    $OldLauncher = Join-Path $InstallDir "Halo3.exe.old"
    if (Test-Path -LiteralPath $OldLauncher -PathType Leaf) { Remove-Item -LiteralPath $OldLauncher -Force }
    foreach ($Name in $Names) {
        $Path = Join-Path $InstallDir $Name
        if (Test-Path -LiteralPath $Path) {
            $Backup = if ($Name -eq "Halo3.exe") { $OldLauncher } else { Join-Path $BackupDir $Name }
            Move-Item -LiteralPath $Path -Destination $Backup
            $Backups[$Path] = $Backup
        }
        $Targets.Add($Path)
    }

    foreach ($Entry in (Get-ChildItem -LiteralPath $Stage | Sort-Object { $_.Name -eq "VERSION" })) {
        Move-Item -LiteralPath $Entry.FullName -Destination $InstallDir
    }
    if ($IsInstaller) {
        # NSIS requires /D to be the last argument and its path to be unquoted.
        $Setup = Start-Process -FilePath $PackagePath -ArgumentList "/S /D=$InstallDir" -Wait -PassThru
        $InstalledVersion = Get-OrderedVersion (Get-Content -LiteralPath $VersionPath -Raw).Trim()
        if ($Setup.ExitCode -ne 0 -or $null -eq $InstalledVersion -or $InstalledVersion -lt $LatestVersion -or
            -not (Test-Path -LiteralPath (Join-Path $InstallDir "Halo3.exe") -PathType Leaf) -or
            -not (Test-Path -LiteralPath (Join-Path $InstallDir "bin/halo3.exe") -PathType Leaf)) {
            throw "Installer did not complete the update"
        }
    }
    Write-Host "==> Updated to $($Release.tag_name)!"
    exit 10
} catch {
    foreach ($Path in $Targets) {
        try {
            if (Test-Path -LiteralPath $Path) { Remove-Item -LiteralPath $Path -Recurse -Force }
            if ($Backups.Contains($Path)) { Move-Item -LiteralPath $Backups[$Path] -Destination $Path }
        } catch {
            $RestoreFailed = $true
        }
    }
    if ($RestoreFailed) {
        Write-Warning "halo3: could not restore all update files. Backups remain in '$TempDir'."
    } elseif ($Force) {
        Write-Host "halo3: update failed ($($_.Exception.Message))"
    }
    exit 0
} finally {
    if ($TempDir -and -not $RestoreFailed) {
        Remove-Item -LiteralPath $TempDir -Recurse -Force -ErrorAction SilentlyContinue
    }
    if ($Lock) { $Lock.Dispose() }
}

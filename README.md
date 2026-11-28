# Halo 3 PC Port

Native 64-bit (`x86_64`) PC port of retail **Halo 3 (Xbox 360, 2007)** built from a **100% byte-matching decompilation** of the original Xbox 360 retail image (`default.xex`, Title ID `4D5307E6`, SHA-256 `98743335129649210f5ef8b1c11b20e9d68beb441e563b7f7369842a5a140899`) and a clean-room implementation of the Xbox 360 runtime.

No Xbox 360 XDK, MCC files, or redistributed assets are required. Building and running requires only this repository and a retail Halo 3 Xbox 360 disc.

## Overview

- **Runtime:** Native 64-bit (`x86_64`) Linux and Windows executables (0 function stubs), clean-room Xbox 360 API layer (`port/sdk/xbox-api/`), VMX128-to-SSE2 intrinsics, Bink 1 playback, D3DX image encoding, and an in-game Xbox Guide (`Profiles`, `Achievements`, `Friends`, `Messages`, `Lobby Codes`, `Settings`).
- **Graphics:** Vulkan 1.3 renderer with ahead-of-time Xenos microcode-to-SPIR-V translation (`port/tools/xenos_spirv`, `port/tools/shaderpack`), `VK_EXT_graphics_pipeline_library`, persistent pipeline caches, texture memexport (1080p Theater screenshots), high-resolution UI typography (`HD Text`), shadow resolution up to 2048, draw distance up to 4x, and anisotropic filtering up to 16x.
- **Assets & DLC:** Load-time conversion of unmodified retail `.map` files and STFS (`LIVE`/`PIRS`/`CON`) DLC packages (big-endian byte-swapping, vertex/index unpacking, Xenos texture untiling and transcoding).
- **Audio & Voice:** XMA2 software decoder, XAudio voice/submix mixer, I3DL2 reverb, Waves Q10 EQs, L360 master limiter (stereo or 5.1), and XHV microphone voice chat.
- **PC & Multiplayer:** Frame interpolation up to 240+ Hz or uncapped, arbitrary resolutions up to 4K and ultrawide (`2560x1080`, `3440x1440`, `3840x2160`), raw mouse-look and rebindable keyboard/mouse, 1-4P gamepad with rumble, 1-4P local split-screen, 4-player local/LAN Campaign co-op, System Link LAN over UDP, and online multiplayer via **SlopBoxLive** (or a self-hosted Live server) with NAT hole-punching and UDP relay fallback.

## Prebuilt Releases

Packages are published on the [Releases](https://github.com/summers-aaron/halo-3/releases) page.

### Windows (`x86_64`)
- **Installer:** `Halo3-Setup.exe` (per-user install, Start Menu and Desktop shortcuts).
- **Portable:** Extract `Halo3-Windows-Portable.zip` and run `Halo3.exe`.

On first launch, select your retail Halo 3 Xbox 360 `.iso` or extracted disc folder. The launcher extracts the disc, compiles the local Vulkan 1.3 shader pack into `%LOCALAPPDATA%\halo3`, and boots the game.

### Linux (`x86_64`)
Pure 64-bit (`GLIBC_2.31+`). No 32-bit or multilib packages required.

- **AppImage:**
  ```bash
  chmod +x Halo3-x86_64.AppImage
  ./Halo3-x86_64.AppImage [/path/to/Halo3.iso]
  ```
- **Flatpak:**
  ```bash
  flatpak install --user Halo3-x86_64.flatpak
  flatpak run io.github.summers_aaron.Halo3
  ```

First launch extracts the ISO (raw XISO, XGD2/Redump, or XGD3), unpacks `default.xex`, compiles shaders into `~/.local/share/halo3`, and boots the game.

## Building from Source

### Retail Disc Layout
Provide an extracted retail Halo 3 Xbox 360 disc (Title ID `4D5307E6`):

```text
halo3-disc/
├── default.xex
├── maps/
│   ├── mainmenu.map
│   ├── shared.map
│   ├── campaign.map
│   └── *.map
└── bink/
```

No copyrighted assets, maps, shaders, or binaries are included in this repository.

### Container Build (Recommended)
Builds Linux `x86_64`, Windows `x86_64`, and release packages (`build-pc/dist/`) inside a pinned Debian 11 + LLVM 22 + MinGW-w64 + Odin image (`podman` or `docker`):

```bash
HALO3_DISC=/path/to/halo3-disc ./port/build/container/build.sh linux
HALO3_DISC=/path/to/halo3-disc ./port/build/container/build.sh windows
HALO3_DISC=/path/to/halo3-disc ./port/build/container/build.sh release
```

Individual package targets: `appimage`, `flatpak`, `windows-exe`, `windows-zip`.

## Configuration & Launch Options

Settings are persisted in `halo3.ini` and editable in-game via **Xbox Guide -> Settings**. Any option can be overridden per launch via CLI flags or `HALO3_*` environment variables.

| Flag | Environment Variable | Description |
|---|---|---|
| `--iso PATH` | | Retail Halo 3 Xbox 360 `.iso` (XGD2/Redump/raw XISO) |
| `--disc PATH` | `HALO3_DISC` | Extracted Halo 3 disc directory |
| `--dlc PATH` | `HALO3_DLC` | STFS DLC map pack directory (default: `<data>/dlc`) |
| `--data PATH` | `HALO3_DATA` | Saves, profiles, caches, and pipeline cache directory |
| `--config PATH` | `HALO3_CONFIG` | Path to `halo3.ini` |
| `--shader-pack PATH` | `HALO3_SHADER_PACK` | Path to compiled shader pack |
| `--res WxH` | `HALO3_RES` | Output resolution (e.g. `1920x1080`, `3440x1440`, `3840x2160`) |
| `--window-mode MODE` | `HALO3_WINDOW_MODE` | `windowed`, `borderless`, or `fullscreen` |
| `--scale 1\|2\|3` | `HALO3_RES_SCALE` | Internal EDRAM scale when `resolution = native` (`1`=720p, `2`=1440p, `3`=4K) |
| `--fps N` | `HALO3_FPS` | Frame cap (`30`, `60`, `120`, `240`, or `0` uncapped) |
| `--interpolate 0\|1` | `HALO3_INTERPOLATE` | High-Hz frame interpolation (default: `1`) |
| `--vsync 0\|1` | `HALO3_VSYNC` | `auto`, `1`, or `0` |
| `--gamma N` | `HALO3_DISPLAY_GAMMA` | Gamma curve (`1`=TV, `2`=sRGB, `3`=power, `0`=linear) |
| `--gpu N` | `HALO3_VULKAN_DEVICE` | Vulkan GPU device index |
| `--raw-mouse` | `HALO3_RAW_MOUSE=1` | Raw mouse-look |
| `--mouse-capture` | `HALO3_MOUSE_CAPTURE=1` | Lock cursor to window |
| `--mouse-sens F` | `HALO3_MOUSE_SENS` | Mouse sensitivity multiplier (default: `1.0`) |
| `--players 1..4` | `HALO3_LOCAL_PLAYERS` | Local split-screen players (default: `1`) |
| `--guests 0..3` | `HALO3_LOCAL_GUESTS` | Local guest players |
| `--coop-4p` | `HALO3_COOP_4P=1` | Enable 4-player local/LAN Campaign co-op |
| `--gamertag NAME` | `HALO3_GAMERTAG` | Player gamertag (max 15 chars) |
| `--audio-backend NAME` | `HALO3_AUDIO_BACKEND` | `auto`, `pulse`, `alsa`, `pipe`, `winmm`, or `null` |
| `--audio-channels 2\|6` | `HALO3_AUDIO_CHANNELS` | Stereo (`2`) or 5.1 surround (`6`) |
| `--no-audio` | `HALO3_NO_AUDIO=1` | Disable audio output |
| `--voice` / `--no-voice` | `HALO3_VOICE=1\|0` | Toggle XHV microphone voice chat |
| `--lan` | `HALO3_LAN=1` | Enable System Link LAN over UDP |
| `--lan-port N` | `HALO3_LAN_PORT` | System Link UDP port (default: `3074`) |
| `--lan-addr IP` | `HALO3_LAN_ADDRESS` | System Link bind address |
| `--lan-slots N` | `HALO3_LAN_SLOTS` | Multi-instance LAN slot count |
| `--lan-peer IP[:PORT]` | `HALO3_LAN_PEER` | Direct System Link peer(s) |
| `--live URL` | `HALO3_LIVE_SERVER` | Live server URL (e.g. `https://slopboxlive.com`) |
| `--live-account PATH` | `HALO3_LIVE_ACCOUNT` | Live account token JSON path |
| `--map NAME` | `HALO3_PROBE_MAP` | Boot directly into a map (e.g. `010_jungle`) |
| `--frame-stats N` | `HALO3_FRAME_STATS` | Log frame timing every `N` frames |
| `--version` | | Print release version and shader ABI |

## Directory Layout

```text
halo-3/
├── tu/                       # Decompiled Halo 3 C++ translation units
├── include/                  # Game C++ headers
├── metadata/                 # Link-time symbol and data-owner tables
└── port/
    ├── build/                # CMake build, toolchains, link scripts, container build
    ├── probe/                # PC port unit/header overrides, retail data fillers, extras
    ├── sdk/                  # Clean-room Xbox 360 runtime (Vulkan 1.3, audio, networking, Guide)
    ├── data/                 # Load-time .map byte-swapper and Xenos texture converter
    ├── shaders/              # Shader pack loader and clean-room HLSL sources
    ├── packaging/            # Linux/Windows GUI launchers, AppImage, Flatpak, NSIS installer
    └── tools/                # Build/asset tools (xiso, xex2pe, shaderpack, xenos_spirv, stubgen, datagen)
```

## Troubleshooting

- **Vulkan initialization failure on Linux:** Verify 64-bit Vulkan loader and GPU ICD packages (`mesa-vulkan-drivers` or `nvidia-vulkan-icd`) are installed.
- **User data location:** Saves, profiles, `halo3.ini`, `dlc/`, and caches are stored in `%LOCALAPPDATA%\halo3` (Windows) or `$XDG_DATA_HOME/halo3` / `~/.local/share/halo3` (Linux). Override with `--data` or `HALO3_DATA`.
- **Incremental rebuilds:** Re-run `HALO3_DISC=/path/to/halo3-disc ./port/build/container/build.sh release` (or `linux` / `windows`). Pass `CLEAN=1` for a full rebuild.

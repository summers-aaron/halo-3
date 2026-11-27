# port/probe/coff-aliases.awk: port/probe/fill/aliases.ld (`"A" = "B";` linker-script assignments, ELF
# spelling) -> COFF assembly whose .drectve section names each A an alternate name for B (lld-link /alternatename).
function deco(n) {
    if (substr(n, 1, 1) == "?") {
        if (arch64) {
            gsub(/\?global_scenario!!3PAUscenario!!A/, "?global_scenario!!3PEAUscenario!!EA", n)
            gsub(/\?global_tag_instances!!3PBUcache_file_tag_instance!!B/, "?global_tag_instances!!3PEBUcache_file_tag_instance!!EB", n)
        }
        gsub(/!/, "@", n)
        return n
    }
    return arch64 ? n : "_" n
}
BEGIN { print "\t.section .drectve,\"yn\"" }
match($0, /^"[^"]+" *= *"[^"]+";/) {
    split($0, f, "\"")
    printf "\t.ascii \" /alternatename:%s=%s\"\n", deco(f[2]), deco(f[4])
    n++
}
END { printf "# %d alternate names\n", n }

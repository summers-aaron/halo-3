# port/probe/coff-asm.awk: retailfill's i686 ELF assembly -> the same data as COFF (lld-link).
#   - symbol names (quoted): MS-mangled names get their `@` back (retailfill spells them `!`, as elf-names.sh does);
#     C names get COFF's leading underscore;
#   - retailfill's own unquoted globals (retail_fn_*) and port_stub_called: the underscore too;
#   - ELF-only directives (.size, .type) dropped; .rodata is .rdata.
# String literals (.ascii/.asciz) pass through unchanged.
function deco(n) {
    if (substr(n, 1, 1) == "?") { gsub(/!/, "@", n); return n }
    return arch64 ? n : "_" n
}
/^\t\.size / || /^\t\.type / { next }
/^\t\.section \.rodata/ { print "\t.section .rdata,\"dr\""; next }
/\.asci[iz] / { print; next }
{
    line = $0; out = ""
    while (match(line, /"[^"]*"/)) {
        out = out substr(line, 1, RSTART - 1) "\"" deco(substr(line, RSTART + 1, RLENGTH - 2)) "\""
        line = substr(line, RSTART + RLENGTH)
    }
    line = out line
    if (!arch64) {
        gsub(/\<retail_fn_/, "_retail_fn_", line)
        gsub(/\<port_stub_called\>/, "_port_stub_called", line)
    }
    print line
}

#!/usr/bin/env bash
# port/tools/shaderpack/stage.sh [WORK]: stages the writable DX11 HLSL tree the shader pack is built from.
#   WORK (default tmp/shaders-work) gets hlsl/ = Bungie's MCC tree (local/h3hlsl-src/hlsl, read-only) with
#   local/shaders-pc/mcc-tree-patches.diff applied (DXC vs FXC fixes), then the port's own patches
#   port/shaders/patches/*.diff in name order (where the retail Xbox data or EDRAM layout differs from what MCC's PC
#   tree assumes), the case alias DOF_filter.fx, and the port's own additions from port/shaders/hlsl_2007/ (clean-room
#   reconstructions of retail-only explicit shaders) on top.
# Nothing under local/ is modified. Refuses to overwrite an existing WORK/hlsl: move it aside first.
set -euo pipefail
repo=$(cd "$(dirname "$0")/../../.." && pwd)
work=${1:-$repo/tmp/shaders-work}
src=$repo/local/h3hlsl-src/hlsl
patches=$repo/local/shaders-pc/mcc-tree-patches.diff
mkdir -p "$work"
if [ -e "$work/hlsl" ]; then
  echo "stage.sh: $work/hlsl exists; move it aside (or use another WORK)" >&2
  exit 1
fi
cp -r "$src" "$work/hlsl"
chmod -R u+w "$work/hlsl"
(cd "$work" && patch -p0 -s --no-backup-if-mismatch < "$patches")
# each port patch names hlsl/<file> on both sides (-p0 from WORK); a patch that does not apply stops the staging
for p in "$repo"/port/shaders/patches/*.diff; do
  [ -f "$p" ] || continue
  (cd "$work" && patch -p0 -s --forward --no-backup-if-mismatch < "$p") || { echo "stage.sh: $p does not apply" >&2; exit 1; }
done
# the tree includes "DOF_filter.fx" but ships dof_filter.fx (Windows is case-insensitive)
ln -s dof_filter.fx "$work/hlsl/DOF_filter.fx"
cp "$repo"/port/shaders/hlsl_2007/*.hlsl "$work/hlsl/"
echo "stage.sh: $work/hlsl staged ($(ls "$work/hlsl" | wc -l) files)"

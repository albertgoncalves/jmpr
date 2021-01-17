#!/usr/bin/env bash

set -euo pipefail

"$WD/scripts/build.sh"
"$WD/bin/main" "$WD/src/vert.glsl" "$WD/src/frag.glsl" || echo $?

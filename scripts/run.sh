#!/usr/bin/env bash

set -euo pipefail

"$WD/scripts/build.sh"
"$WD/bin/main" || echo $?

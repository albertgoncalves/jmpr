#!/usr/bin/env bash

set -eu

export ASAN_OPTIONS="detect_leaks=0"

"$WD/scripts/build.sh"
"$WD/bin/main" || echo $?

#!/usr/bin/env bash

set -eu

"$WD/scripts/build.sh"
"$WD/bin/main" || echo $?

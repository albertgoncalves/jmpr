#!/usr/bin/env bash

set -eu

"$WD/scripts/build.sh"

export ASAN_OPTIONS="detect_leaks=0"

sudo sh -c "echo 1 > /proc/sys/kernel/perf_event_paranoid"
sudo sh -c "echo 0 > /proc/sys/kernel/kptr_restrict"
perf record \
    --call-graph fp \
    "$WD/bin/main" "$WD/src/vert.glsl" "$WD/src/frag.glsl"
perf report
rm perf.data*

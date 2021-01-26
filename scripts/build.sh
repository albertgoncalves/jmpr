#!/usr/bin/env bash

set -euo pipefail

if [ ! -d "$WD/glfw" ]; then
    git clone https://github.com/glfw/glfw.git
    (
        cd "$WD/glfw" || exit 1
        cmake -DBUILD_SHARED_LIBS=ON .
        make
    )
fi

flags=(
    "-fshort-enums"
    "-fsingle-precision-constant"
    "-g"
    "-march=native"
    "-O1"
    "-Wall"
    "-Wcast-align"
    "-Wcast-align=strict"
    "-Wcast-qual"
    "-Wconversion"
    "-Wdate-time"
    "-Wduplicated-branches"
    "-Wduplicated-cond"
    "-Werror"
    "-Wextra"
    "-Wfatal-errors"
    "-Wfloat-equal"
    "-Wformat=2"
    "-Wformat-signedness"
    "-Winline"
    "-Wlogical-op"
    "-Wmissing-declarations"
    "-Wmissing-include-dirs"
    "-Wmissing-prototypes"
    "-Wnested-externs"
    "-Wno-unused-function"
    "-Wno-unused-parameter"
    "-Wnull-dereference"
    "-Wpacked"
    "-Wpedantic"
    "-Wpointer-arith"
    "-Wredundant-decls"
    "-Wshadow"
    "-Wstack-protector"
    "-Wstrict-prototypes"
    "-Wswitch-enum"
    "-Wtrampolines"
    "-Wundef"
    "-Wunused"
    "-Wunused-macros"
    "-Wwrite-strings"
)
libs=(
    "-lm"
    "-lglfw"
    "-lGL"
    "-lX11"
    "-lXfixes"
)
paths=(
    "-I${WD}/glfw/include"
    "-L${WD}/glfw/src"
)

now () {
    date +%s.%N
}

(
    start=$(now)
    for x in "$WD"/src/*.h; do
        cppcheck "$x" | sed 's/\/.*\/\(.*\) \.\.\./\1/g'
    done
    cppcheck \
        --enable=all \
        --suppress=missingIncludeSystem \
        "$WD/src" \
        | sed 's/\/.*\/\(.*\) \.\.\./\1/g'
    clang-format -i -verbose "$WD/src"/*.glsl 2>&1 | sed 's/\/.*\///g'
    clang-format -i -verbose "$WD/src"/*.c 2>&1 | sed 's/\/.*\///g'
    clang-format -i -verbose "$WD/src"/*.h 2>&1 | sed 's/\/.*\///g'
    gcc \
        "${paths[@]}" \
        "${libs[@]}" \
        "${flags[@]}" \
        -o "$WD/bin/main" \
        "$WD/src/main.c"
    end=$(now)
    python3 -c "print(\"Compiled! ({:.3f}s)\n\".format(${end} - ${start}))"
)

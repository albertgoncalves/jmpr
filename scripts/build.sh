#!/usr/bin/env bash

set -euo pipefail

if [ ! -d "$WD/glfw" ]; then
    (
        cd "$WD"
        git clone https://github.com/glfw/glfw.git
        cd "$WD/glfw"
        cmake -DBUILD_SHARED_LIBS=ON .
        make
    )
fi

flags=(
    "-ferror-limit=1"
    "-fshort-enums"
    "-g"
    "-march=native"
    "-O0"
    "-Werror"
    "-Weverything"
    "-Wno-disabled-macro-expansion"
    "-Wno-documentation"
    "-Wno-documentation-unknown-command"
    "-Wno-error=#warnings"
    "-Wno-extra-semi-stmt"
    "-Wno-missing-noreturn"
    "-Wno-padded"
    "-Wno-reserved-id-macro"
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
    clang-format -i -verbose "$WD/src"/*
    clang \
        "${paths[@]}" \
        "${libs[@]}" \
        "${flags[@]}" \
        -o "$WD/bin/main" \
        "$WD/src/main.c"
    end=$(now)
    python3 -c "print(\"Compiled! ({:.3f}s)\n\".format(${end} - ${start}))"
)

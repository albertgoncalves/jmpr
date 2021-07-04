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
    "-fno-exceptions"
    "-fno-math-errno"
    "-fno-rtti"
    "-fno-unwind-tables"
    "-fshort-enums"
    "-g"
    "-march=native"
    "-std=c++11"
    "-Werror"
    "-Weverything"
    "-Wno-c++98-compat-pedantic"
    "-Wno-c99-extensions"
    "-Wno-disabled-macro-expansion"
    "-Wno-documentation"
    "-Wno-documentation-unknown-command"
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
    "-I$WD/glfw/include"
    "-L$WD/glfw/src"
)

now () {
    date +%s.%N
}

(
    start=$(now)
    clang++ -O1 "${flags[@]}" -o "$WD/bin/codegen" "$WD/src/codegen.cpp"
    "$WD/bin/codegen" > "$WD/src/scene_assets_codegen.hpp"
    clang-format -i -verbose "$WD/src"/*
    clang++ -O3 "${paths[@]}" "${libs[@]}" "${flags[@]}" -o "$WD/bin/main" \
        "$WD/src/main.cpp"
    end=$(now)
    python3 -c "print(\"Compiled! ({:.3f}s)\n\".format($end - $start))"
)

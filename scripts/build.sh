#!/usr/bin/env bash

set -eu

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
    "-ffast-math"
    "-fno-autolink"
    "-fno-exceptions"
    "-fno-math-errno"
    "-fno-rtti"
    "-fno-unwind-tables"
    "-fshort-enums"
    "-g"
    "-march=native"
    "-nostdlib++"
    "-std=c++11"
    "-Werror"
    "-Weverything"
    "-Wno-c++98-compat-pedantic"
    "-Wno-c99-extensions"
    "-Wno-disabled-macro-expansion"
    "-Wno-extra-semi-stmt"
    "-Wno-padded"
    "-Wno-reserved-id-macro"
)
libs=(
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
    "$WD/scripts/codegen.py" > "$WD/src/init_assets_codegen.hpp"
    clang-format -i -verbose "$WD/src"/*
    clang++ -O3 "${paths[@]}" "${libs[@]}" "${flags[@]}" -o "$WD/bin/main" \
        "$WD/src/main.cpp"
    end=$(now)
    python3 -c "print(\"Compiled! ({:.3f}s)\n\".format($end - $start))"
)

#!/usr/bin/env python3

from os import environ
from os.path import join

WD = environ["WD"]


def codegen(var, path):
    with open(path, "r") as file:
        return "static const char {}[] = {{ {}, 0, }};".format(
            var,
            ", ".join(map(str, map(ord, file.read())))
        )


def main():
    print("\n".join([
        "#ifndef __INIT_ASSETS_CODEGEN_H__",
        "#define __INIT_ASSETS_CODEGEN_H__",
        codegen("SHADER_VERT", join(WD, "src", "vert.glsl")),
        codegen("SHADER_FRAG", join(WD, "src", "frag.glsl")),
        "#endif",
    ]))


if __name__ == "__main__":
    main()

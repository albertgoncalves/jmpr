with import <nixpkgs> {};
mkShell {
    buildInputs = [
        clang_10
        cmake
        cppcheck
        doxygen
        libGL
        linuxPackages.perf
        pkg-config
        python3
        shellcheck
        valgrind
        xorg.libX11
        xorg.libXcursor
        xorg.libXext
        xorg.libXfixes
        xorg.libXi
        xorg.libXinerama
        xorg.libXrandr
    ];
    shellHook = ''
        . .shellhook
    '';
}

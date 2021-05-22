with import <nixpkgs> {};
mkShell {
    buildInputs = [
        clang_11
        cmake
        cppcheck
        doxygen
        libGL
        linuxPackages.perf
        pkg-config
        python3
        shellcheck
        simplescreenrecorder
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

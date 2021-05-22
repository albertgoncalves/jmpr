with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_11.stdenv; } {
    buildInputs = [
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

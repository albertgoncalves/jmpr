with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_12.stdenv; } {
    buildInputs = [
        cmake
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

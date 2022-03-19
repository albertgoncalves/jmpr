with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_14.stdenv; } {
    buildInputs = [
        cmake
        doxygen
        libGL
        linuxPackages.perf
        mold
        pkg-config
        python3Packages.flake8
        renderdoc
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
    APPEND_LIBRARY_PATH = lib.makeLibraryPath [
        libGL
        xorg.libX11
        xorg.libXfixes
    ];
    shellHook = ''
        export LD_LIBRARY_PATH="$APPEND_LIBRARY_PATH:$LD_LIBRARY_PATH"
        . .shellhook
    '';
}

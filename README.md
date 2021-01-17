# jmpr

Miniture `fps`/`platformer` engine made with the help of [https://learnopengl.com/](https://learnopengl.com/), [https://www.khronos.org/](https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glDrawElementsInstanced.xhtml), and [https://stackoverflow.com/](https://stackoverflow.com/questions/47620258/how-to-make-low-res-graphics-with-opengl).

Makes use of [instancing](https://learnopengl.com/Advanced-OpenGL/Instancing) to reduce draw calls and [frame buffering](https://stackoverflow.com/questions/7071090/low-resolution-in-opengl-to-mimic-older-games/7071311#7071311) to downsample the pixel resolution.

Limited to drawing simple platforms. Untested outside of `linux`.

<div align="center"><img src="cover.gif"></div>

Needed things
---
*   [Nix](https://nixos.org/download.html)

Quick start
---
```
$ ./shell
[nix-shell:path/to/jmpr]$ ./scripts/build.sh    # build only
[nix-shell:path/to/jmpr]$ ./scripts/run.sh      # build, run
[nix-shell:path/to/jmpr]$ ./scripts/profile.sh  # build, profile via perf, cachegrind
```

Controls
--
* `move` - <kbd>Up</kbd> <kbd>Down</kbd> <kbd>Left</kbd> <kbd>Right</kbd>
* `jump` - <kbd>Space</kbd>
* `look` - <kbd>Mouse</kbd>
* `quit` - <kbd>Escape</kbd>

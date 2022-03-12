Aperture - Game Engine
====

> Currently status: Developing...

Aperture: A simple & tiny game engine based on OpenGL ES,
just for learning OpenGL and render stuff, for fun.

Current status (TODOs)
----

- [x] Texture load
- [x] Model import  (Finished, but unstable)
- [x] Lightning stimulation
- [ ] Cross platform:
    - [ ] Windows
    - [x] Android (see [GameEngine-Android](https://github.com/STARRY-S/GameEngine-Android))
    - [x] Linux
    - [ ] ~~Mac OS~~
    - [ ] ~~IOS~~
- [ ] Music & Sound engine
- [ ] Network (socket)...
- [ ] Physic engine
- [ ] GUI tools (User graphics)
- [ ] Database

Usage
----

### Linux
Install `CMake`, `Assimp`, `GLFW` and other tools required for compilation:
`gcc`, `make`, etc... on your system.

```
# clone this repository
$ git clone https://github.com/STARRY-S/GameEngine.git && cd GameEngine
$ mkdir build && cd build
$ cmake .. && make -j4
```

After that, it will build a dynamically linked library `libgame_engine.so`,
executable files `game_engine_demo` and `test`.

Run `./game_engine_demo -h` for help.

### Android

See [GameEngine-Android](https://github.com/STARRY-S/GameEngine-Android)

Dependencies
----

- OpenGL ES 3.0
- cglm
- Assimp
- stb_image

License
----

> [Apache 2.0](LICENSE)
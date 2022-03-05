# Game Engine

> Still developing...

**Name Needed**: need a name for this game engine project.

A simple & tiny game engine based on OpenGL ES.

This project is currently unstable, and can't work properly.

## Current status (TODOs)

- [x] Texture load
- [x] Model import  (Finished, but unstable)
- [ ] Cross platform support
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

## Usage

Install `CMake`, `Assimp`.

```
# clone this repository
$ git clone https://github.com/STARRY-S/GameEngine.git && cd GameEngine
$ mkdir build && cd build
$ cmake .. && make -j4
```

After that, it will generate a dynamically linked library `libgame_engine.so`.

## Dependencies

- OpenGL ES 3.0
- cglm
- Assimp
- stb_image

## License

> [Apache 2.0](LICENSE)
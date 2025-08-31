# boobcraft (working title!!)

Simple 3D framework written in C + OpenGL for retro singleplayer games.

| Specs      |                                        |
|------------|----------------------------------------|
| Resolution | 400x240                                |
| Framerate  | 30fps                                  |
| Camera     | 40°, 0.01-100units                     |
| Input      | D-pad, 2 action buttons, 1 menu button |
| Color      | 24 bit, full black is transparent      |

*I may remove some of the spec restrictions later to make the framework more generalized.*

## Installation

```
brew install sdl2
brew install glew
git clone https://github.com/dairycultist/boobcraft/
cd boobcraft
make run
```

## TO-DO

- make meshes "singletons," requiring an external transform to be rendered (may be expanded to also externalize the texture, unsure yet)
- switch to opengl 1.1 for minimalism (and comparability to 3DS/HL1)
- make a full-blown engine that recreates the technical and aesthetic aspects of games like the first Doom, Quake, and Half-Life

## References

the game programmer doesn't even have to think about OpenGL and all that; it's all handled by the engine! abstraction!

Minimal and lightweight. Trying to use only snake case and failing.

[Miku model used for testing](https://sketchfab.com/3d-models/hatsune-miku-low-poly-6668784e9dfa46ba92bc28f85d8154e5)

[Skybox texture](https://opengameart.org/content/sky-box-sunny-day)

https://github.com/martincohen/Punity

http://mgarcia.org/Blog/3DS

**OpenGL**

https://open.gl/

https://www.khronos.org/files/opengl-quick-reference-card.pdf

**SDL2**

https://github.com/libsdl-org/SDL

https://github.com/michalzalobny/3d-renderer-in-c

# lucoacraft

A Doom clone written in C focused on level-clearing and light fantasy.

## Technical

A sector is just a **2D poly** with a **floor** and **ceiling** height. Collision is just 1) check if you're inside the sector (2D comparison) and 2) check if you're below the ceiling and above the floor (1D comparison).

- find out how to do input with GLFW
- implement shader stuff below + render a cube + player controls (just flying for now)

```
Shading for 3D
vertex shader does all the transforms related to camera position and perspective divide and whatnot, so we have to pass in the player position/rotation as a uniform
ALSO we can use the post-transform pre-perspective divide Z coordinate to determine how bright something is (closer = brighter) giving us simple lighting for free
we just have to pass the depth alongside the position to the frag shader
also need to remember to discard anything behind the player
```

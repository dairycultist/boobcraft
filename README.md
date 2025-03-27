# lucoacraft

A Doom clone written in C focused on being minimal and lightweight with no external dependencies (it currently uses the Mac-only version of one of the libraries tho I think).

Todo
- find out how to do input with GLFW
- implement shader stuff below + render a cube + player controls (just flying for now)

## Technical

A sector is just a **2D poly** with a **floor** and **ceiling** height. Collision is just 1) check if you're inside the sector (2D comparison) and 2) check if you're below the ceiling and above the floor (1D comparison).

The vertex shader is passed in the camera position/rotation as a uniform. It uses this to transformation meshdata relative to the camera. Then, it performs the perspective divide. The final Z coordinate represents the **depth** of each vertex. Any vertex behind the camera is discarded. Finally, the depth is passed to the fragment shader to determine how bright something is (closer = brighter) giving us simple lighting for free.

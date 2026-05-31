# Ether Blocks

Ether Blocks is a first-person crystalline construction game set in a dark void.
It is not intended to be a Minecraft clone. The goal is a focused, atmospheric
building sandbox with a minimal sci-fi visual language.

## Vision

There is no sky, terrain horizon, or conventional world backdrop. Structures
float in an empty space, with darkness and fog closing in around the edges of
the visible area.

Blocks are geometric and textureless. Their surfaces emit a cold blue or cyan
glow, giving the world the appearance of a digital crystal lattice: a block
builder imagined through a 2077-style minimalist aesthetic.

The design can later grow beyond simple construction:

- block types with distinct colors and emission levels;
- blocks that react to adjacent materials;
- energy propagation through connected structures;
- light, motion, or transformation rules driven by block combinations.

## Current Prototype

The repository currently contains an early interactive prototype:

- first-person camera controls;
- voxel selection with ray casting;
- placing and removing blocks;
- mesh rebuilding after world edits;
- a selection outline and center crosshair;
- a small OpenGL abstraction layer for buffers, meshes, materials, textures,
  shaders, rendering, window management, and input.

The void environment, fog, emissive materials, and textureless crystalline art
direction are the next visual milestones.

## Controls

| Input              | Action                |
|--------------------|-----------------------|
| `W`, `A`, `S`, `D` | Move                  |
| Mouse              | Look around           |
| Left mouse button  | Place a block         |
| Right mouse button | Remove a block        |
| Mouse wheel        | Adjust camera zoom    |
| `Esc`              | Close the application |

## Building

The project uses CMake and vcpkg manifest mode. It requires a C++23 compiler.

```sh
cmake -S . -B build
cmake --build build
```

The executable is generated as `build/app/app.exe` on Windows.

## Status

Ether Blocks is experimental and under active development. The current focus is
building a clean rendering foundation before expanding the world simulation and
visual effects.

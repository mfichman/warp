# WARP
Matt Fichman, Francesco Georg
CS248, Spring 2010

## Premise
Warp is inspired by the tripped out arcade-like shooter REZ for PS2. Like REZ, a significant part of the
game will be about the sound and visual effects, making a game that's as much fun to watch as it is to
play. Unlike REZ we are incorporating a physics engine and limited freedom of movement. Another
element we will add is that there will be no clear direction of “up” (no gravity!) As you flip your ship
upside-down to avoid an obstacle, the world will flip around with you. Our targeting system will be
similar to the REZ targeting system, where the user selects enemies by picking them with a cursor.

[Video Link](http://www.youtube.com/watch?v=wU54dPsoSC4)

## Features
* Rigid body physics and collision detection with triangle meshes (Bullet)
* Destructible enemies and objects
* Particle explosions and flames
* Synchronization between music and game events (ChucK)
* Network-interfaced audio server
* Scripting module with game-specific extensions (Lua)
* Scripted AI tracking for enemies and missiles
* Rigged & skinned animations
* Procedurally-generated buildings
* Tube and level path dynamically generated from custom script file format
* Cooperative script multitasking
* All models, textures and animations are homemade

## Dependencies
* Boost 1.42 (date_time, thread, filesystem, system)
* Bullet 2.75
* Ogre 1.7.0 RC1 (with TerrainSceneManager plugin)
* OIS
* OSCPack
* Lua 5.1
* Visual Studio 2008
* OpenGL 2.0

## Build Instructions
1. Download dependencies from http://www.stanford.edu/~mfichman/deps.zip and copy the
“Deps” folder into the top-level directory for the project
2. Open Build\Warp.sln in Visual Studio
3. Build > Build Solution (F7 key)

# Run instructions
1. Open Warp.exe (it’s in the top-level folder)

# Credits
* GarageBand (for sound clips)
* Quaternion & vector classes in Lua ported from Ogre C++ versions

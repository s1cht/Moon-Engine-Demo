# Moon-Engine-Demo

Welcome to the Moon-Engine-Demo.

<img width="1438" height="999" alt="image" src="https://github.com/user-attachments/assets/f6c8c6db-1f54-4ffd-93d7-aec597d97dd6" />

This repository is a public demonstration of my private development project called **Moon-Engine**. I implemented the engine core from the ground up to gain a deeper understanding of low-level systems and high-performance graphics.

## Project Overview

The engine uses a deferred rendering pipeline. In this approach, scene geometry is first rendered into a geometry buffer, and lighting is computed in a separate pass. This separation makes it possible to handle many light sources without reprocessing geometry for each light.

## Current Implementation

- **Entity Component System**  
  The engine uses a custom Entity Component System to manage game objects and their associated data. This design improves cache locality and keeps data separate from behavior, which helps the engine scale to more complex scenes.

- **Rendering Backend and Shader Language**  
  The rendering backend is implemented using the Vulkan graphics application programming interface. Shaders are written in High Level Shading Language and are compiled to the Vulkan-compatible intermediate representation using the DirectX Shader Compiler.

- **Lighting**  
  The lighting system supports three primary light types: point lights, directional lights, and spot lights.

- **Core Architecture**  
  The engine core is entirely my own implementation and focuses on manual resource management and runtime efficiency. The project uses xxHash for fast identifier lookup.

- **Audio Subsystem**  
  The repository includes a minimal audio engine that handles basic music playback. The current focus of the audio system is performance and simplicity rather than advanced effects.

## Geometry Buffer Structure

The deferred pipeline in this demo relies on a geometry buffer that stores per-pixel surface information. The lighting pass reads this data to compute final pixel colors without re-evaluating geometry for each light. The geometry buffer stores the following data across multiple render targets:

- **Position**  
  World-space or view-space coordinates of each fragment.

- **Depth**  
  The distance from the camera to each fragment, used for occlusion and reconstruction.

- **Normal**  
  Surface normal vectors used to compute how light interacts with surfaces.

- **Base Color**  
  The base surface color or texture data.

## Tools and Libraries

The project integrates several commonly used libraries and tools:

- Vulkan Memory Allocator for graphics processing unit memory management  
- Premake version five for project configuration and build file generation  
- DirectX Shader Compiler for compiling High Level Shading Language code to Vulkan-compatible intermediate representation  
- spdlog logging library for clean and asynchronous logging  
- Dear ImGui immediate mode graphical user interface library for debugging and tooling  
- xxHash for high-speed hashing inside the engine core

## Roadmap

The engine is under active development. Planned improvements include:

- A fully integrated editor for scene composition and live tweaking  
- A shadow mapping system that supports all light types  
- Additional modern rendering techniques and post-processing effects such as screen-space ambient occlusion and bloom  
- A physics system for collision detection and rigid body dynamics  
- Expanded audio features including spatial audio and sound effects  
- A DirectX twelve rendering backend for Microsoft Windows

## How to Build

This project is configured for the Microsoft Windows operating system and uses Premake for project generation.

1. Open the `ProjectGeneration` folder.  
2. Run the `GenerateProjects.bat` batch file.  
3. This will generate a Visual Studio solution file.  
4. Open the solution in the Visual Studio integrated development environment and build the project.

## Note

This repository is a public snapshot of a private repository and represents the state of the engine at the time of export. Some higher-level features and ongoing work remain in the private main branch.

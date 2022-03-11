# d3dTest
Hi! I'm working on a personal, reusable framework for, and messing around with, the Window32 and DirectX11 APIs.

# Notes
Make sure to set the platform to Win32, which is where all of the current configurations are.

# Files
## Framework Files
- `framework/Camera.cpp` and `framework/Camera.h`: implementation for an updatable camera that works with DirectX math structures
- `framework/ConstantBuffers.h`: header file for the constant buffer structures
	- `ConstBuffer`: a basic struct to hold a transformation matrix
	- `TConstBuffer`: a struct to hold a transformation matrix; transposes the input matrix first
	- `VPTConstBuffer`: a struct to hold a transformation matrix; transposes the input matrix and performs View and Perspective transforms as well
- `framework/Cube.h`: class that represents a non-textured cube
- `framework/CubeSkinned.h`: class that represents a textured cube
- `framework/CwfException.cpp` and `framework/CwfException.h`: provides a custom exception class for different types of errors and the associated macros
- `framework/DXDebugInfoManager.cpp` and `framework/DXDebugInfoManager.h`: class that manages the DirectX debug information queue (for error collection purposes)
	- Credit to ChiliTomatoNoodle
- `framework/Graphics.cpp` and `framework/Graphics.h`: class that manages the graphics of a certain window
- `framework/Keyboard.cpp` and `framework/Keyboard.h`: class that manages and provides access to keyboard input
- `framework/Material.h`: class for Materials (see below)
- `framework/Mouse.cpp` and `framework/Mouse.h`: class that manages and provides access to mouse input
- `framework/Orientation.h`: class that maintains an updatable rotational transformation matrix
- `framework/ShaderStage.h`: enum class for different shader stages; right now, it's just vertex and pixel shaders
- `framework/ShapeConcepts.h`: defines the concepts for specific types of vertices; essentially asserts something exists for a type (thank you C++20)
- `framework/Submaterial.h`: class for Submaterials (see below) 
- `framework/Vertices.h`: defines a namespace for types of vertices and several default vertex types (e.g. 3 dimensions + texture coordinates, 4 dimensions)
- `framework/WStringLiteral.h`:	Defines a compile-time wide string literal that allows us to template on, effectively, file names
- `framework/Window.cpp` and `framework/Window.h`: class that manages the actual graphical window for an application
- `framework/WindowBuilder.cpp` and `framework/WindowBuilder.h`: class that allows elegant specification of window styles, options, etc.
- `framework/WindowClass.cpp` and `framework/WindowClass.h`: class that specifies a "window class" to register with Windows
- `framework/lib/`: code necessary to framework, not written by me
- `framework/shaders/`: HLSL source files for shaders

## Example User Defined Files
- `App.cpp` and `App.h`

## Launcher/Entry Point File
- `WinMain.cpp`

## Visual Studio Project Files
- `d3dTest.sln`
- `d3dTest.vcxproj`
- `d3dTest.vcxproj.filters`
- `d3dTest.vcxproj.user`

## Resources
- `copy_resources.ps1`: copies or updates files from one directory to another
	- specifically used to copy or update files in `resources` to the output directory
- `resources/`: the resources the program needs

## Other
- `CubeTestPixelShader.hlsl`: test pixel shader HLSL source
- `CubeTestVertexShader.hlsl`: test vertex shader HLSL source
- files with the same name (minus extension) as `hlsl` files: compiled shaders

# Materials and Submaterials
## Materials
Materials can be thought of as, well, materials. Each Material object represents one material of physical object in a scene, and has one set of shaders and constant buffers, but as many meshes as you want. For example, one Material object might represent objects made out of wood, and would have the associated shaders and buffers, but with different meshes (e.g. a cube and a chair, both would use the shaders and buffers of the Material).

I chose to use Materials for several reasons:
	- it fits the way I think about scenes
	- I thought it would be educational to implement
	- and it means you can draw all objects using the same set of shaders at the same time (and thus don't need to reload the same shaders later)
(However, I do not purport to be very well acquainted with actual graphics optimization, so this could very well be a poor design choice)

## Submaterials
A Submaterial is like a "child" of a Material. It uses the same shaders and general information as its parent Material, but has different constant buffers.

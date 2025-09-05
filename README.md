# A C++ Vulkan engine supporting windows and linux

## How to use:
### WINDOWS: Add mingw64 from MSYS2, and CMake to your system PATH.
### LINUX: *(type 'y' on all promps)*
```bash
sudo apt update
sudo apt install build-essential
sudo apt install cmake
sudo apt install libx11-dev
sudo apt install gdb
```
- Install **@category:debuggers cppdbg** to run the program in VSCode/VSCodium 
- Include the bin folder from VULKAN in you environmental variables *if you want to edit shaders!*

## How do make your own games:

- Make a new folder in src/games named whatever you want to name your game
- Duplicate program.cpp and .hpp from games/flappyBird into your new folder
- Delete the tick() implementation and replace it with your own. 
- If your game uses the keyboard, keep "keyboard.resetKeys();" at the bottom of the file
- Navigate to src/vulkan/app.hpp and update the include path to the path of your file
- Update the constructor and the tick() functions to your game's.
- Navigate to src/vulkan/pipeline.hpp and create a new function to load your game
- Navigate to src/vulkan/pipeline.cpp and use your new function at the bottom of the file
- You may make other files and folders in your game folder, because you are ready to create!

## Third party dependencies:
[GLM (Math)](https://github.com/icaven/glm)  
[GLFW (Window)](https://www.glfw.org/download)  
[VULKAN (Graphics)](https://vulkan.lunarg.com/sdk/home)  
[MINIAUDIO (Audio)](https://miniaud.io/)  
[STB-LIBRARIES (Image loader, font text rasterizer)](https://github.com/nothings/stb)  
## Build commands:
### Windows:
```bash
cmake --preset windows
cmake --build --preset windows -- -j8
```

*or use **windowsrelease** for release builds*

### Linux:
```bash
cmake --preset linux
cmake --build --preset linux -- -j8
```

*or use **linuxrelease** for release builds*

**Any number between 1-100 may be used instead of -j8**
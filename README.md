# A C++ Vulkan engine supporting windows and linux

## How to use:
### WINDOWS:
- Download msys2 from [https://www.msys2.org/](https://www.msys2.org/) **CLOSE THE TERMINAL**
- Launch MSYS2 MSYS (the purple one)
- Run the following commands to download a C++ compiler:
```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
```
- In the windows search bar, look up "Edit environment variables"
- Go to "advanced"
- Add C:\msys64\ucrt64\bin to your user PATH
- Add C:\msys64\mingw64\bin to your user PATH
- Download the **Windows x64 Installer** from [https://cmake.org/download/](https://cmake.org/download/)
### LINUX: *(type 'y' on all promps)*
```bash
sudo apt update
sudo apt install build-essential
sudo apt install cmake
sudo apt install libx11-dev
sudo apt install gdb
```
### GENERAL:
- Install **@category:debuggers cppdbg** to run the program in VSCode/VSCodium 
- If you want to edit shaders, include the bin folder from VULKAN in you environmental variables

## How do make your own games:

- Make a new folder in src/games named whatever you want to name your game
- Duplicate program.cpp and .hpp from games/flappyBird into your new folder
- Delete the tick() implementation and replace it with your own
- Navigate to src/vulkan/app.hpp and update the include path to the path of your file
- Update the constructor and the tick() functions to your game's
- Navigate to src/vulkan/pipeline.hpp and create a new function to load your game
- Navigate to src/vulkan/pipeline.cpp and use your new function at the bottom of the file

## Third party dependencies:
[VULKAN (Graphics) {Modified}](https://vulkan.lunarg.com/sdk/home)  
[GLFW (Window) {Modified}](https://www.glfw.org/download)  
[MINIAUDIO (Audio) {Modified}](https://miniaud.io/)  
[STB-LIBRARIES (Image loader, font text rasterizer) {Modified}](https://github.com/nothings/stb)  
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
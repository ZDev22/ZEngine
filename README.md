A sprite-based C++ Vulkan engine that supports WINDOWS and LINUX || still in development!

**-----------------HOW-TO-USE-----------------**

WINDOWS:
Add mingw64 from MSYS2, and CMake to your system PATH.

LINUX: *(type 'y' on all promps)*
```bash
sudo apt update
sudo apt install build-essential
sudo apt update
sudo apt install cmake
sudo apt update
sudo apt install libx11-dev
sudo apt update
sudo apt install gdb
```

- Include the bin folder from VULKAN in you environmental variables *if you want to edit shaders!*

**-----------THIRD-PARTY-DEPENDENCIES---------**
```bash
GLM - https://github.com/icaven/glm
GLFW - https://www.glfw.org/download
VULKAN - https://vulkan.lunarg.com/sdk/home
STB-LIBRARIES - https://github.com/nothings/stb
```
**-------------------COMMANDS-----------------**

WINDOWS BUILD:
```bash
cmake --preset windows
cmake --build --preset windows -- -j8
```

*or use **windowsrelease** for release builds*

LINUX BUILD:
```bash
cmake --preset linux
cmake --build --preset linux -- -j8
```

*or use **linuxrelease** for release builds*

**Any number between 1-100 may be used instead of -j8**
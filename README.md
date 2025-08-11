A sprite-based C++ Vulkan engine that supports WINDOWS and LINUX || still in development!

__-----------------HOW-TO-USE-----------------__

WINDOWS:
Add mingw64 from MSYS2, and CMake to your system PATH.

LINUX: _(type 'y' on all promps)_
sudo apt update
sudo apt install build-essential
sudo apt update
sudo apt install cmake
sudo apt update
sudo apt install libx11-dev
sudo apt update
sudo apt install gdb


- Include the bin folder from VULKAN in you environmental variables _if you want to edit shaders!_

__-----------THIRD-PARTY-DEPENDENCIES---------__

GLM - https://github.com/icaven/glm
GLFW - https://www.glfw.org/download
VULKAN - https://vulkan.lunarg.com/sdk/home
STB-LIBRARIES - https://github.com/nothings/stb

__-------------------COMMANDS-----------------__

WINDOWS BUILD:
cmake --preset windows
cmake --build --preset windows -- -j16

_or use __windowsrelease__ for release builds_

LINUX BUILD:
cmake --preset linux
cmake --build --preset linux -- -j16

_or use __linuxrelease__ for release builds_
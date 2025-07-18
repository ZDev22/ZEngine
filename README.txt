A sprite-based C++ Vulkan engine - supports WINDOWS, and can run on LINUX.
Still in development! Shared so you guys can look at my code!

-----------------HOW-TO-USE-----------------

WINDOWS:
Requires mingw64 from MSYS2 and CMake in your systems's PATH.

LINUX: (type 'y' on all promps)
sudo apt update
sudo apt install build-essential
sudo apt update
sudo apt install cmake
sudo apt update
sudo apt install libx11-dev
sudo apt update
sudo apt install gdb

- Set up GLSLC in your evironmental variabels (methods may vary, recommended - use the one included with vulkanSDK 1.4.321.0)

-----------THIRD-PARTY-DEPENDENCIES---------

GLM - https://github.com/icaven/glm
GLFW - https://www.glfw.org/download
VULKAN - https://vulkan.lunarg.com/sdk/home
STB-LIBRARIES - https://github.com/nothings/stb
FASTNOISELITE - https://github.com/Auburn/FastNoiseLite

-------------------COMMANDS-----------------

WINDOWS BUILD:
cmake --preset windows
cmake --build --preset windows

or use "windowsrelease" for release builds

LINUX BUILD:
cmake --preset linux
cmake --build --preset linux

or use "linuxrelease" for release builds

Get the latest github changes: git fetch origin
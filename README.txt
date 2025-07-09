A sprite-based C++ Vulkan engine - supports WINDOWS, and can run on LINUX.

Requires mingw64 from MSYS2, VULKAN, and CMake in your systems's PATH to run.

Still in development! Shared so you guys can look at my code!


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
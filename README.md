A sprite-based C++ Vulkan engine that supports WINDOWS and LINUX || still in development!

**-----------------HOW-TO-USE-----------------**

WINDOWS:
Add mingw64 from MSYS2, and CMake to your system PATH.

LINUX: *(type 'y' on all promps), may take a couple of minutes*
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

- Install **@category:debuggers cppdbg** to run the program in VSCode/VSCodium
- Include the bin folder from VULKAN in you environmental variables *if you want to edit shaders!*

**----------HOW-TO-MAKE-YOUR-OWN-GAMES--------**

- Make a new folder in src/games named whatever you want to name your game
- Duplicate program.cpp and .hpp from games/flappyBird into your new folder.
- Update the last include path in src/vulkan/app.hpp to the path to your file 
- Create a new function in pipeline.cpp and .hpp to load your sprites
- Delete the tick() implementation and replace it with your own. *(if your game uses the keyboard, keep "keyboard.resetKeys();" at the bottom of the file)*
- You may make other files and folders in the folder you created! You can now make a game!

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
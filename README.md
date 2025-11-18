# A C++ Vulkan engine supporting windows and linux
## About:
My goal for this engine is to be **THE FASTEST** *2D game engine*. It is very simplistic and a little *hard to work with*, but if you use it properly it can achieve **thousands of FPS**. It has features like *sprite creation* and *texture loading* that run **REALTIME**. *As long as you have a GPU* (even an intigrated one), **it can run on your device**. There are many planned optimizations on the way, (mainly on linux because it's better) *with the goal of **4K FPS** on an **Intel(R) Celeron(R) N4500*** with *1.10GHz* on two cores, and it's intigrated GPU *(on ![FlappyBird](/src/assets/images/flappyBird.png)flappybird![FlappyBird](/src/assets/images/flappyBird.png)). Mac support is planned, but I currently don't have If you want one.* To see the progress on the engine, **check out my YouTube channel: [https://www.youtube.com/@ZDev12345](https://www.youtube.com/@ZDev12345)** and consider leaving a star so more people can view the engine **:D**

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
### LINUX:
#### Ubuntu-based:
```bash
sudo apt update
sudo apt install build-essential cmake gdb -y #C/C++/Cmake
sudo apt install libx11-dev libxrandr-dev libxkbcommon-dev libegl1-mesa-dev libxcursor-dev -y libxi-dev #X11
sudo apt install libwayland-dev wayland-protocols -y #Wayland
sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-tools -y #Vulkan
```
#### Arch-based:
```bash
sudo pacman -Syu --noconfirm
sudo pacman -S gcc cmake gdb --noconfirm #C/C++/Cmake
sudo pacman -S libx11 libxrandr xkbcommon mesa --noconfirm #X11
sudo pacman -S wayland wayland-protocols --noconfirm #Wayland
sudo pacman -S vulkan-icd-loader vulkan-tools --noconfirm #Vulkan
# If you have an Intel or AMD GPU, install Vulkan drivers:
# Intel: sudo pacman -S vulkan-intel --noconfirm
# AMD: sudo pacman -S vulkan-radeon --noconfirm
```
### GENERAL:
- Install **@category:debuggers cppdbg** to run the program in VSCode/VSCodium
- If you want to edit shaders, include the bin folder from VULKAN in you environmental variables
- When cloning the dev branch, clone it from the most recent commit that isn't skool edits or broken

## How do make your own games:

- Make a new folder in src/games named whatever you want to name your game
- Duplicate program.cpp and .hpp from games/flappyBird into your new folder
- Delete the tick() implementation and replace it with your own
- Navigate to src/vulkan/pipeline.hpp and create a new function to load your game
- Navigate to src/vulkan/pipeline.cpp and implement your new function at the bottom of the file
- Navigate to src/vulkan/app.cpp and update the include path to the path of your file
- In app.cpp, update the load, constructor and the tick() functions to your game's

## Third party dependencies:
[VULKAN (Graphics) {Modified}](https://vulkan.lunarg.com/sdk/home)
[RGFW (Window) {Modified}](https://github.com/ColleagueRiley/RGFW)
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
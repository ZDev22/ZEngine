# **A C++ engine supporting Linux, Windows and Mac**
# About:
My goal for this engine is to be **THE FASTEST** *2D game engine*, aiming for **thousands of FPS** on lower-end devices, as well as utilizing GPU accelerated graphics with Vulkan and SPIR-V.

It has features like *sprite creation* and *texture loading* and *(beta) font rendering* that all run **REALTIME**, as well as the **ZDeps libraries** which are lightweight and super usefull for creating 2D games!

There are many planned optimizations on the way, (mainly on linux because it's better) *with the goal of **4K FPS** on an **Intel(R) Celeron(R) N4500*** with *2.70GHz*. And **10k FPS** on modern hardware.

To see the progress on the engine, **check out my YouTube channel: [youtube.com/@ZDev12345](https://www.youtube.com/@ZDev12345)** and consider leaving a star so more people can see the engine **:D**

*Version handling is ZIVH (ZDev's Incremental Version Handling) - a non-destructive version counting system* ex: ```1.4.7 -> 2.4.7``` ```9.12.9.4 -> 9.13.9.4``` *and does none of this nonsense:* ```1.8.0 -> 1.8.5```

*If you find an issue, please submit an issue on the github page. If you also find a fix, make it a pull request!*

# How to use:
## LINUX:
#### Arch:
```bash
sudo pacman -Syu --noconfirm
sudo pacman -S gcc cmake gdb --noconfirm #C/C++/Cmake/Cdbg
sudo pacman -S libx11 libxrandr xkbcommon mesa --noconfirm #X11
sudo pacman -S wayland wayland-protocols --noconfirm #Wayland
sudo pacman -S vulkan-icd-loader vulkan-tools --noconfirm #Vulkan
# Intel: sudo pacman -S vulkan-intel --noconfirm
# AMD: sudo pacman -S vulkan-radeon --noconfirm
```
#### Debian:
```bash
sudo apt update
sudo apt install build-essential cmake gdb -y #C/C++/Cmake/Cdbg
sudo apt install libx11-dev libxrandr-dev libxkbcommon-dev libegl1-mesa-dev libxcursor-dev libxi-dev -y #X11
sudo apt install libwayland-dev wayland-protocols -y #Wayland
sudo apt install libvulkan1 mesa-vulkan-drivers vulkan-tools -y #Vulkan
```
#### Fedora:
```bash
sudo dnf update -y
sudo dnf install gcc gcc-c++ cmake gdb -y #C/C++/CMake/Cdbg
sudo dnf install libX11-devel libXrandr-devel libxkbcommon-devel mesa-libEGL-devel libXi-devel libXcursor-devel -y #X11
sudo dnf install wayland-devel wayland-protocols-devel -y #Wayland
sudo dnf install vulkan vulkan-tools vulkan-loader-devel -y #Vulkan
# Intel: sudo dnf install vulkan-intel -y
# AMD: sudo dnf install vulkan-radeon -y
```
#### Gentoo:
```bash
sudo emerge --sync
sudo emerge --ask sys-devel/gcc sys-devel/cmake sys-devel/gdb -y #C/C++/CMake/Cdbg
sudo emerge --ask x11-libs/libX11 x11-libs/libXrandr x11-libs/libXi x11-libs/libXcursor x11-libs/libxkbcommon mesa-libs/mesa -y #X11
sudo emerge --ask x11-libs/wayland x11-libs/wayland-protocols -y #Wayland
sudo emerge --ask media-libs/vulkan-loader media-libs/vulkan-tools -y #Vulkan
# Intel: sudo emerge --ask media-libs/vulkan-intel
# AMD: sudo emerge --ask media-libs/vulkan-radeon
```
#### Alpine:
```bash
sudo apk update
sudo apk add build-base cmake gdb #C/C++/CMake/Cdbg
sudo apk add libx11-dev libxrandr-dev libxkbcommon-dev libxi-dev libxcursor-dev mesa-egl-dev #X11
sudo apk add wayland wayland-protocols-dev #Wayland
sudo apk add vulkan-tools vulkan-headers vulkan-loader-dev #Vulkan
# Intel: sudo apk add vulkan-intel
# AMD: sudo apk add vulkan-radeon
```
#### Leap:
```bash
sudo zypper refresh
sudo zypper update -y
sudo zypper install -t pattern devel_C_C++ cmake gdb -y #C/C++/CMake/Cdbg
sudo zypper install libX11-devel libXrandr-devel libxkbcommon-devel Mesa-libEGL-devel libXi-devel libXcursor-devel -y #X11
sudo zypper install wayland-devel wayland-protocols-devel -y #Wayland
sudo zypper install vulkan-loader-devel vulkan-tools -y #Vulkan
# Intel: sudo zypper install vulkan-intel -y
# AMD: sudo zypper install vulkan-radeon -y
```
## MAC:
```bash
brew install gcc cmake lldb #C/C++/Cmake/Cppdbg
brew install molten-vk vulkan-headers vulkan-tools #Vulkan
```
Install Vulkan to your home directory from this link: https://vulkan.lunarg.com/sdk/home
Then install **vadimcn.vscode-lldb** to run the program on VSCode (Does not support Zed)
```bash
vim ~/.zshrc #Edit your system's environmental variables
```
Paste these exports into zshrc to link up Vukan with your project:
```bash
export VULKAN_SDK=$HOME/VulkanSDK/1.4.328.1/macOS
export PATH=$VULKAN_SDK/bin:$PATH
export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib
export VK_LAYER_PATH=$VULKAN_SDK/share/vulkan/explicit_layer.d
export VK_ICD_FILENAMES=$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json
```
Exit zshrc then reload:
```bash
source ~/.zshrc
sudo cp ~/VulkanSDK/1.4.328.1/macOS/lib/libMoltenVK.dylib /usr/local/lib/
```
**THIS PROJECT CANNOT RUN ON ZED WITH MACOS**
## WINDOWS:
- Download msys2 from [https://www.msys2.org/](https://www.msys2.org/) **CLOSE THE TERMINAL**
- Launch MSYS2 MSYS (the purple one)
- Run the following commands to download a C++ compiler:
```bash
pacman -Syu
pacman -S base-devel mingw-w64-x86_64-toolchain
```
- In the windows search bar, look up "Edit environment variables"
- Go to "advanced"
- Add C:\msys64\ucrt64\bin to your user (or system) PATH
- Add C:\msys64\mingw64\bin to your user (or system) PATH
- Download the **Windows x64 Installer** from [https://cmake.org/download/](https://cmake.org/download/)
## Extensions (Linux/Windows):
- Install **@category:debuggers cppdbg** to run the program in VSCode
- Install the **cppdbg** extension to run the program in VSCodium
- Include the Vulkan bin folder in your environmental variables to edit shaders

# How do make your own games:

- Make a new folder in src/games named whatever you want to name your game
- Duplicate program.cpp and .hpp from games/flappyBird into your new folder
- Delete the tick() implementation and replace it with your own
- Navigate to src/vulkan/pipeline.hpp and create a new function to load your game
- Navigate to src/vulkan/pipeline.cpp and implement your new function at the bottom of the file
- Navigate to src/vulkan/app.cpp and update the include path to the path of your file
- In app.cpp, update the load, constructor and the tick() functions to your game's

# Dependencies:
[VULKAN (Graphics) {Modified}](https://vulkan.lunarg.com/sdk/home)</br>
[RGFW (Window) {Modified}](https://github.com/ColleagueRiley/RGFW)</br>
[MINIAUDIO (Audio) {Modified}](https://miniaud.io/)</br>
[STB-LIBRARIES (Image loader, font text rasterizer) {Modified}](https://github.com/nothings/stb)
# Build commands:
## Linux:
```bash
cmake --preset linuxdebug
cmake --build --preset linuxdebug -j8
```
## Mac:
```bash
cmake --preset macdebug
cmake --build --preset macdebug -j8
```
## Windows:
```bash
cmake --preset windowsdebug
cmake --build --preset windowsdebug -j8
```
### Availiable presets:
```
linuxdebug    linuxrelease    linuxsmol
macdebug      macrelease      macsmol
windowsdebug  windowsrelease  windowssmol
```
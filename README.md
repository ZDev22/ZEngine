![ZENGINE.H](ZENGINE.png "ZENGINE - THE 2D GAME ENGINE")
# **THE SINGLE-HEADER C 2D VULKAN ENGINE**
# About:
My goal for this engine is to be **THE FASTEST** *2D game engine*, aiming for **thousands of FPS** on lower-end devices, as well as utilizing GPU accelerated graphics with Vulkan and SPIR-V shaders.

It has features like *sprite creation* and *texture loading* and *(beta) font rendering* that all run **REALTIME**, as well as the **ZDep modules** which are lightweight and super usefull for creating 2D games!

Use ffmpeg to convert any .wav into .mp3 files. Also use ffmpeg to compress them: ```ffmpeg -i input.mp3 -c:a libmp3lame -q:a 9 output.mp3```

Version handling is ZIVH (ZDev's Incremental Version Handling) - a *non-destructive* version counting system ex: ```1.4.7 -> 2.4.7``` ```9.12.9.4 -> 9.13.9.4``` *and does none of this nonsense:* ```1.8.0 -> 1.8.5```

Licensed under the GNU GPL v3 license - please see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for more details

*If you find an issue, please submit an issue on the github page. If you also find a fix, make it a pull request!*

# Clone the repo:
```bash
git clone --depth=1 https://github.com/ZDev22/ZEngine
cd ZEngine
```
# Dependencies:
## LINUX:
#### Arch:
```bash
sudo pacman -Syu --noconfirm
sudo pacman -S gcc --noconfirm #C
sudo pacman -S libx11 libxrandr libxcursor xkbcommon mesa --noconfirm #X11
sudo pacman -S vulkan-headers vulkan-tools vulkan-icd-loader --noconfirm #Vulkan
# Intel GPU: sudo pacman -S vulkan-intel --noconfirm
# AMD GPU: sudo pacman -S vulkan-radeon --noconfirm
```
#### Debian:
```bash
sudo apt update
sudo apt install build-essential -y #C
sudo apt install libx11-dev libxrandr-dev libxkbcommon-dev libegl1-mesa-dev libxcursor-dev libxi-dev -y #X11
sudo apt install libvulkan-dev libvulkan1 mesa-vulkan-drivers vulkan-tools -y #Vulkan
```
#### Fedora:
```bash
sudo dnf update -y
sudo dnf install gcc -y #C
sudo dnf install libX11-devel libXrandr-devel libxkbcommon-devel mesa-libEGL-devel libXi-devel libXcursor-devel -y #X11
sudo dnf install vulkan-headers vulkan-tools vulkan-loader-devel -y #Vulkan
# Intel GPU: sudo dnf install vulkan-intel -y
# AMD GPU: sudo dnf install vulkan-radeon -y
```
#### Gentoo:
```bash
root # emerge --sync
sudo emerge --ask sys-devel/gcc -y #C
sudo emerge --ask x11-libs/libX11 x11-libs/libXrandr x11-libs/libXi x11-libs/libXcursor x11-libs/libxkbcommon mesa-libs/mesa -y #X11
sudo emerge --ask media-libs/vulkan-loader dev-util/vulkan-headers media-libs/vulkan-tools -y #Vulkan
# Intel GPU: sudo emerge --ask media-libs/vulkan-intel
# AMD GPU: sudo emerge --ask media-libs/vulkan-radeon
```
#### Leap:
```bash
sudo zypper refresh
sudo zypper update -y
sudo zypper install -t pattern devel_C -y #C
sudo zypper install libX11-devel libXrandr-devel libxkbcommon-devel Mesa-libEGL-devel libXi-devel libXcursor-devel -y #X11
sudo zypper install vulkan-headers vulkan-loader-devel vulkan-tools -y #Vulkan
# Intel GPU: sudo zypper install vulkan-intel -y
# AMD GPU: sudo zypper install vulkan-radeon -y
```
#### Alpine:
```bash
sudo apk update
sudo apk add build-base #C
sudo apk add libx11-dev libxrandr-dev libxkbcommon-dev libxi-dev libxcursor-dev mesa-egl-dev #X11
sudo apk add vulkan-headers vulkan-tools vulkan-loader-dev #Vulkan
# Intel GPU: sudo apk add vulkan-intel
# AMD GPU: sudo apk add vulkan-radeon
```
## MAC (beta):
- Download vulkan from: [https://vulkan.lunarg.com/sdk/home](https://vulkan.lunarg.com/sdk/home)
- Move vulkan into your home folder
- Run the following command to install moltenVK:
```bash
brew install molten-vk
```
## WINDOWS:
- Download vulkan from: [https://vulkan.lunarg.com/sdk/home](https://vulkan.lunarg.com/sdk/home)
- Download msys2 from [https://www.msys2.org/](https://www.msys2.org/)
- Launch MSYS2 UCRT64 (the yellow one)
- Run the following commands to download a C compiler:
```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-make
pacman -S mingw-w64-ucrt-x86_64-vulkan-headers
pacman -S mingw-w64-ucrt-x86_64-vulkan-loader
pacman -S mingw-w64-ucrt-x86_64-shaderc
```
- Make sure to compile through the MSYS2 UCRT64 terminal

# How do make your own games:
- Duplicate flappybird.h as a good base for your game
- Delete the tickGame() implementation and replace it with your own
- In main.c make sure to include your game instead of the example
- ZDeps documentation is inside their respective files

# Included dependencies:
[VULKAN (Graphics)](https://vulkan.lunarg.com/sdk/home)</br>
[RGFW (Window)](https://github.com/ColleagueRiley/RGFW)</br>
[MINIAUDIO (Audio) {Modified}](https://miniaud.io/)</br>
[STB-IMAGE (Image loader) {Modified}](https://github.com/nothings/stb)</br>
[STB-TRUETYPE (font text rasterizer) {Modified}](https://github.com/nothings/stb)

# Build:
```bash
make
```

# Run:
```bash
cd build
./main
```


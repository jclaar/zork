# README

***
This Fork update/changes (org README below)
Support for showing the AI art mentioned below in Ascii art using buildin code or improved gfx with chafa (if installed) was added together with an inital GUI app to show the images properly. Start zork wiht --gui or --ascii-art to use this modes.
A script to generate AI images that also support replacing images and edit the room description if needed to generate better images was added. It needs a huggingface token and you get some images for free, if you need/want to add money you can get a way with a very low amount, I used less then $0.20 (Jan 2026) to generate all images during development.

This might get merged back to the main project and if so this part should be folded into the text below.
***

Main project README:

***
UPDATE: Because I either have too much time on my hands, or an odd (borderline unhealthy) obsession with all of this,
I've been using AI image generators to create pictures of various locations in Zork. In a separate project
I've put together an http server that will display room images as the user moves from place to place. It's
not integrated here yet because I don't really have enough images to make it worthwhile, but it's been a fun thing
to play around with. Images I have so far are saved in the zork_pics folder. Filenames are
the room ID (the first string for each room in roomdefs.h). If I can come up with enough rooms I might add the server to display them as you play. My current
server supports png and jpeg (as you can see by the various generators I've used, which create images in different 
formats). If anyone wants to contribute, add an image and send a pull request! I can promise you neither fame
nor fortune, but you can rest easy knowing you've contributed to a bit of Zork history.
***

This is a C++-20 port of the final 616-point Zork, written at MIT around 1978-1981. The goal was to do a port
as directly as possible, meaning that the original logic, structures, functions, modules, etc. are
ported as-is. Therefore, it is not necessarily the most "C++" way of doing things. The main exceptions 
to the MDL code are saving/restoring, which uses the Boost serialization libraries,
and I/O, which uses C++ cin/cout. Restarting also needs platform-specific code to find the application's
location. There is also no attempt
to obfuscate or encrypt any of the text, rooms, puzzles, and so on. With the number of walkthroughs on the
internet, it seems pointless to take such precautions.

The port has become more of a project for me to play around with new C++ features as they're added, so updates
that are posted recently probably aren't fixing anything or changing any functionality. Hopefully they're not 
breaking things either...

There is a full map of Zork here: https://www.reddit.com/r/zork/comments/10tlxfd/new_zork_map/.

One additional modification: Use the "TERMINAL" command in Zork to simulate the feel of a 1970's-era
terminal output.

Builds are supported for Linux (gcc-12 required), MacOS and Windows (32 and 64 bit, Visual Studio 2022 required).
There is nothing using any non-standard C++ anywhere in the code, so it should be simple to port to other
platforms. It also makes extensive use of C++-20 features, and thus a C++-20-compliant compiler
is required. 

Build requirements:

Boost (www.boost.org) - Requires the filesystem, serialization, and system libraries. This build used
version 1.85.0, though earlier (and later) versions (v1.70 or later) will probably work fine, within reason. See www.boost.org for
installation instructions. A simple method to build the requirements for Boost from source on Linux is:
```
git clone https://github.com/boostorg/boost.git
cd boost
git checkout boost-1.85.0
git submodule update --init --recursive
./bootstrap.sh
./b2 --with-serialization --with-system --with-filesystem -j8
sudo ./b2 --with-serialization --with-system --with-filesystem -j8 install
```

## CMAKE BUILD
Presets have been added for Linux, Mac and Windows. Presets will be created in the "out" subdirectory.
CMake Build Recipe (Tested in Ubuntu 22.04 w/ gcc 13.1.0).  

The presets use Ninja for the build. Other build systems are untested, but would presumably work.

Windows presets:  
x86-debug  
x86-release  
x64-debug  
x64-release  

Linux presets:  
linux-debug  
linux-release

Mac presets:  
macos-debug  
macos-release

Build files are placed in ./out/build/[preset name]  
Final output is ./out/build/[preset name]/zork[.exe]  

There is no installation implemented since everything is in a single file.
Boost libraries are statically-linked so there are no other dependencies. 

### Example
```
cmake --preset linux-debug  
cmake --build out/build/linux-debug  
out/build/linux-debug/zork  
```

## Building with GUI Support

The GUI mode uses raylib to display room images in a graphical window. To enable it:

### 1. Install raylib

**On Ubuntu/Debian:**
```bash
sudo apt install libraylib-dev
```

**On Fedora:**
```bash
sudo dnf install raylib-devel
```

**On macOS (using Homebrew):**
```bash
brew install raylib
```

**Building from source (all platforms):**
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j4
sudo make install
```

### 2. Build with raylib enabled

```bash
# Using presets
cmake --preset linux-debug -DUSE_RAYLIB=ON
cmake --build out/build/linux-debug

# Or using direct cmake
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_RAYLIB=ON ..
make -j4
```

### 3. Run with GUI

```bash
./zork --gui
```

### Troubleshooting

If CMake reports "raylib not found", ensure raylib is installed and CMake can find it. You may need to set:
```bash
export CMAKE_PREFIX_PATH=/usr/local
```

Or specify the path directly:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_RAYLIB=ON -Draylib_ROOT=/path/to/raylib ..
```
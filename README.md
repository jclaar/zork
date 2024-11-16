# README #

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
version 1.85.0, though earlier (and later) versions will probably work fine, within reason. See www.boost.org for
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

### CMAKE BUILD
Presets have been added for Linux, Mac and Windows. Presets will be created in the "out" subdirectory.
CMake Build Recipe (Tested in Ubuntu 22.04 w/ gcc 13.1.0).  

The presets use Ninja for the build. Other build systems are untested, but would presumably work.

Windows:
cmake --preset 
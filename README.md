# README #

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

A map of the 585-point version is [here](https://2warpstoneptune.com/2015/05/13/hand-drawn-dungeon-map-1979-1981/).
It is missing three puzzles, but I could not find a more complete one.
One additional modification: Use the "TERMINAL" command in Zork to simulate the feel of a 1970's-era
terminal output.

Builds are supported for Linux (gcc-13 required), MacOS and Windows (32 and 64 bit, Visual Studio 2022 required).
There is nothing using any non-standard C++ anywhere in the code, so it should be simple to port to other
platforms. It also makes extensive use of C++-20 features, and thus a C++-20-compliant compiler
is required. 

Build requirements:

Boost (www.boost.org) - Requires the filesystem, serialization, and system libraries. This build used
version 1.85.0, though earlier versions will probably work fine, within reason.

### CMAKE BUILD
Presets have been added for Linux, Mac and Windows. Presets will be created in the "out" subdirectory.
CMake Build Recipe (Tested in Ubuntu 22.04 w/ gcc 13.1.0).  

The presets use Ninja for the build. Other build systems are untested, but would presumably work.

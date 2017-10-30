# README #

This is a C++-17 port of the final 616-point Zork, written at MIT around 1978-1981. The goal was to do a port
as directly as possible, meaning that the original logic, structures, functions, modules, etc. are
ported as-is. Therefore, it is not necessarily the most "C++" way of doing things. The main exceptions 
to the MDL code are saving/restoring, which uses the Boost serializaton libraries,
and I/O, which uses C++ cin/cout. Restarting also needs platform-specific code to find the application's
location. There is also no attempt
to obfuscate or encrypt any of the text, rooms, puzzles, and so on. With the number of walkthroughs on the
internet, it seems pointless to take such precautions.

A map of the 585-point version is [here](https://2warpstoneptune.com/2015/05/13/hand-drawn-dungeon-map-1979-1981/).
It is missing three puzzles, but I could not find a more complete one.
One additional modification: Use the "TERMINAL" command in Zork to simulate the feel of a 1970's-era
terminal output.

Builds are supported for Linux (gcc-7 required) and Windows (32 and 64 bit, Visual Studio 2017 required).
There is nothing using any non-standard C++ anywhere in the code, so it should be simple to port to other
platforms. It also makes extensive use of C++-17 features, and thus a C++-17-compliant compiler
is required. 

Build requirements:
Boost (www.boost.org) - Requires the program\_options and the serialization libraries. This build used
version 1.65.1, which added Visual Studio 2017 support. 

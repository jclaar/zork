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
version 1.66.0.

Individual modules:

gobj\_parse: Early on, it became apparent that manually defining rooms, objects and strings was time-consuming
and error prone. gobj_parse automates this by parsing dung.mud (where all items in the dungeon are defined)
and generating various headers and C++ modules. See the post-build step in the project file or the makefile
for more information. Suffice it to say that this executable needs to be built and run first.

func\_dump: Once gobj\_parse has run and generated the header files, func\_dump runs and generates
header files defining the room and object functions.

mdlfun: This is the main game implementation. Modules almost entirely correspond to their respective
MDL sources. Functions and variables are named the same, except they are lowercase and an underscore
replaces the hyphen. For example, the POUR-ON function in act1.mud is equivalent to the pour_on
function in act1.cpp.

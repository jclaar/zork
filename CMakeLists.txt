cmake_minimum_required( VERSION 3.1.0 )

cmake_policy( VERSION 3.14 )

project( zork VERSION 1.0.0 )

set( CMAKE_CXX_STANDARD 20 )

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.65.0 REQUIRED COMPONENTS serialization filesystem system)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Make sure we have at least gcc-13.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13")
        message(FATAL_ERROR "Must use at least gcc-13.")
    endif()
endif()

# Don't give warnings on Win32 Visual Studio builds.
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(zork
	act1.h  
	act3.h
	adv.h
	cevent.h
	defs.h
	funcs.h
	gobject.h
	makstr.h
	memq.h
	object.h
	objser.h
	roomdefs.h
	room.h
	sr.h
	strings.h
	ZorkException.h
	act2.h
	act4.h
	
	clock.h
	dung.h
	globals.h
	info.h
	melee.h
	objdefs.h
	objfns.h
	parser.h
	roomfns.h
	rooms.h
	stdafx.h
	util.h
	zstring.h
	act1.cpp
	act3.cpp
	adv.cpp
	cevent.cpp
	dung.cpp
	globals.cpp
	makstr.cpp
	melee.cpp
	object.cpp
	room.cpp
	rooms.cpp
	stdafx.cpp
	util.cpp
	act2.cpp
	act4.cpp
	
	defs.cpp
	funcs.cpp
	info.cpp
	mdlfun.cpp
	memq.cpp
	parser.cpp
	roomfns.cpp
	sr.cpp
	strings.cpp
	FlagSupport.h
	zstring.cpp )

target_link_libraries(zork ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ldl)

install(TARGETS zork DESTINATION bin)

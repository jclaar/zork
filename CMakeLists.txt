cmake_minimum_required( VERSION 3.15 )

cmake_policy( VERSION 3.15 )
cmake_policy(SET CMP0091 NEW)

project( zork VERSION 1.2.0 )

set( CMAKE_CXX_STANDARD 20 )

if (WIN32)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(Boost_USE_STATIC_RUNTIME ON)
add_compile_definitions(BOOST_ALL_NO_LIB)
endif()

# Suppresses warning if Boost v1.70 or later is installed.
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_USE_STATIC_LIBS ON)
if (CMAKE_VERSION VERSION_LESS "3.30")
    find_package(Boost 1.65.0 REQUIRED COMPONENTS serialization filesystem system)
else()
    find_package(Boost CONFIG REQUIRED COMPONENTS serialization filesystem system)
endif()

# Make sure we have at least gcc-12.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12")
        message(FATAL_ERROR "Must use at least gcc-12.")
    endif()
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_compile_options(-Wno-parentheses)
endif()

# Don't give warnings on Win32 Visual Studio builds.
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(zork
	act1.h act1.cpp
	act2.h act2.cpp
	act3.h act3.cpp
	act4.h act4.cpp
	adv.h adv.cpp
	cevent.h cevent.cpp
	
	defs.h defs.cpp
	dung.h dung.cpp
	FlagSupport.h
	funcs.h funcs.cpp
	globals.h globals.cpp
	gobject.h 
	info.h info.cpp
	makstr.h makstr.cpp
	mdlfun.cpp
	melee.h melee.cpp
	memq.h memq.cpp
	objdefs.h
	object.h object.cpp
	objfns.h objser.h
	parser.h parser.cpp
	room.h room.cpp
	roomdefs.h
	roomfns.h roomfns.cpp
	rooms.h rooms.cpp
	sr.h sr.cpp
	precomp.h
	strings.h strings.cpp
	util.h util.cpp
	version.h
	ZorkException.h
	zstring.h zstring.cpp
)

target_link_libraries(zork ${Boost_LIBRARIES})
target_precompile_headers(zork PRIVATE precomp.h)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
	if (APPLE)
		add_custom_command(TARGET zork 
			POST_BUILD
			COMMAND hdiutil create -volname "Zork" -srcfolder "${CMAKE_CURRENT_BINARY_DIR}/zork" -ov -format UDZO "Zork_mac_${CMAKE_PROJECT_VERSION}.dmg"
		)
	elseif(UNIX)
	    add_custom_command(TARGET zork
		    POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E tar "zcfv" "zork_linux_${CMAKE_PROJECT_VERSION}.gz" "zork"
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/zork"
			COMMENT "Creating archive file for release."
		)
	else()
	    add_custom_command(TARGET zork
		    POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E tar "cfv" "zork_win64_${CMAKE_PROJECT_VERSION}.zip" --format=zip "zork.exe"
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/zork.exe"
			COMMENT "Creating archive file for release."
		)
	endif()
endif()


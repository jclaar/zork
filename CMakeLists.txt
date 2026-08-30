cmake_minimum_required( VERSION 3.30 )

# What a pain this is...
if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.4.0")
  message("Using CMake v4.4.0 or higher")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3.0")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.3")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.0")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "3.31.8")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30.0")
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
endif()

cmake_policy( VERSION 3.15 )
cmake_policy(SET CMP0091 NEW)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.30")
cmake_policy(SET CMP0167 NEW)
endif()

project( zork VERSION 1.2.0 LANGUAGES CXX)

set( CMAKE_CXX_STANDARD 23 )
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if (WIN32)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(Boost_USE_STATIC_RUNTIME ON)
add_compile_definitions(BOOST_ALL_NO_LIB)
add_compile_definitions(WIN32_LEAN_AND_MEAN)
set(NTDLL ntdll.lib)
endif()

set(Boost_USE_STATIC_LIBS ON)
# Boost changed system to a header-only library in version 1.90.0, so only require it for older versions.
find_package(Boost 1.74.0 REQUIRED)
set(Boost_REQUIRED_COMPONENTS filesystem serialization)
if (${Boost_VERSION} VERSION_LESS "1.89.0")
	list(APPEND Boost_REQUIRED_COMPONENTS system)
endif()
if (${Boost_VERSION} VERSION_GREATER_EQUAL "1.81.0")
	list(APPEND Boost_REQUIRED_COMPONENTS process)
endif()
find_package(Boost 1.74.0 REQUIRED COMPONENTS ${Boost_REQUIRED_COMPONENTS})
if(LINUX)
find_package(Threads REQUIRED)
endif()

# Make sure we have at least gcc-11.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11")
        message(FATAL_ERROR "Must use at least gcc-11.")
    endif()
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_compile_options(-Wno-parentheses)
endif()

# Don't give warnings on Win32 Visual Studio builds.
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(BOOST_PROCESS_VERSION=2)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_executable(zork)

target_sources(zork PRIVATE
	act1.h
	act2.h
	act3.h
	act4.h
	adv.h adv.cpp
	cevent.h cevent.cpp
	
	defs.h defs.cpp
	dung.h dung.cpp
	FlagSupport.h
	funcs.h funcs.cpp
	globals.h
	gobject.h 
	makstr.h makstr.cpp
	mdlfun.cpp
	melee.h melee.cpp
	objdefs.h
	object.h object.cpp
	objfns.h objser.h
	parser.h parser.cpp
	room.h room.cpp
	roomdefs.h
	roomfns.h roomfns.cpp
	rooms.h rooms.cpp
	sr.h sr.cpp
	strings.h strings.cpp
	util.h util.cpp
	version.h
	ZorkException.h

	PUBLIC
	FILE_SET CXX_MODULES FILES
	    info.cpp
		strings.cpp
		zork_main.cpp
		memq.cpp
		zstring.cpp
		act1.cpp
        act2.cpp
		act3.cpp
        act4.cpp
        globals.cpp
)

set_target_properties(zork PROPERTIES
    CXX_MODULE_STD ON
    CXX_STANDARD 23
)

target_link_libraries(zork ${Boost_LIBRARIES} ${NTDLL} ${CMAKE_DL_LIBS})
if (LINUX)
target_link_libraries(zork Threads::Threads)
endif()

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
	    target_compile_definitions(zork PRIVATE _WIN32_WINNT=0x0601)
	    add_custom_command(TARGET zork
		    POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E tar "cfv" "zork_win64_${CMAKE_PROJECT_VERSION}.zip" --format=zip "zork.exe"
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/zork.exe"
			COMMENT "Creating archive file for release."
		)
	endif()
endif()


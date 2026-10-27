cmake_minimum_required(VERSION 3.30)

project(zork VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(Boost_USE_STATIC_RUNTIME ON)
    add_compile_definitions(BOOST_ALL_NO_LIB WIN32_LEAN_AND_MEAN)
endif()

set(Boost_USE_STATIC_LIBS ON)

# Resolve Boost version first so we can require the correct components per version.
find_package(Boost 1.74.0 REQUIRED)

set(_boost_components filesystem serialization)
# Boost::system is a library prior to v1.90.
if(Boost_VERSION_STRING VERSION_LESS "1.89.0")
    list(APPEND _boost_components system)
endif()
# Boost::process v2 was added in 1.81.0.
if(Boost_VERSION_STRING VERSION_GREATER_EQUAL "1.81.0")
    list(APPEND _boost_components process)
endif()

find_package(Boost 1.74.0 REQUIRED COMPONENTS ${_boost_components})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Threads REQUIRED)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
    message(FATAL_ERROR "Must use at least GCC 14.")
endif()

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
    info.h
    makstr.h makstr.cpp
    mdlfun.cpp
    melee.h melee.cpp
    memq.h
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
    zstring.h zstring.cpp
)

target_compile_features(zork PRIVATE cxx_std_23)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    #target_compile_options(zork PRIVATE -Wno-parentheses)
endif()

target_compile_definitions(zork PRIVATE
    _CRT_SECURE_NO_WARNINGS
    BOOST_PROCESS_VERSION=2
)

target_include_directories(zork PRIVATE
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(zork PRIVATE
    ${Boost_LIBRARIES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CPACK_PACKAGE_NAME "Zork++")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Zork from MIT 1981")
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

    if(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
        set(CPACK_SYSTEM_NAME "macOS")
        set(CPACK_DMG_VOLUME_NAME "Zork")
        set(CPACK_DMG_FORMAT "UDZO")
        set(CPACK_DMG_APPLICATIONS_SYMLINK ON)
    elseif(UNIX)
        set(CPACK_GENERATOR "TGZ")
    else()
        target_compile_definitions(zork PRIVATE _WIN32_WINNT=0x0601)
        set(CPACK_BINARY_NSIS OFF)
        set(CPACK_GENERATOR "ZIP")
    endif()

    set(CPACK_PACKAGE_FILE_NAME "Zork_${CPACK_SYSTEM_NAME}_${PROJECT_VERSION}")
    install(TARGETS zork DESTINATION .)
endif()

include(CPack)
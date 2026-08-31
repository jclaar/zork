#pragma once

#include <string>
#include <tuple>
#include "defs.h"

enum class SpeechType
{
    kVerb,
    kPrep,
    kAdj,
    kBuzz,
};

// Actions
// Object support flags.
class nrobj {};
class robjs {};
class reach {};
class obj {};
class aobjs {};
class have {};
class no_take {};
class try_ {}; // Added underscore to avoid using "try" keyword
class take {};
class driver {};
class flip {};
typedef std::variant<std::monostate, int, reach, robjs, aobjs, no_take, have, try_, take, Bits, std::list<Bits>> ALType;
typedef std::list<ALType> AL;
class AVSyntax : private std::tuple<std::string_view, rapplic>
{
public:
    AVSyntax(std::string_view name, rapplic fn) : std::tuple<std::string_view, rapplic>(name, fn) {}

    std::string_view verb() const { return std::get<0>(*this); }
    rapplic fn() const { return std::get<1>(*this); }
};
using ParseItem = std::variant<const char *, obj, nrobj, AL, AVSyntax, driver, flip>;
using AnyV = std::vector<ParseItem>;
using ActionVec = std::vector<AnyV>;

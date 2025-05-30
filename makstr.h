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

void add_inqobj(const ObjectP &obj);
WordP make_word(SpeechType st, std::string_view val);


void add_question(const char *str, const std::initializer_list<QuestionValue> &vector);
void add_demon(const HackP &x);

PrepP find_prep(std::string_view prep);
VerbP find_verb(std::string_view verb);
const ActionP &find_action(std::string_view action);
direction find_dir(const std::string &dir);

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
typedef std::variant<const char *, obj, nrobj, AL, AVSyntax, driver, flip> ParseItem;
typedef std::vector<ParseItem> AnyV;
typedef std::vector<AnyV> ActionVec;
void oneadd_action(const char *str1, const char *str2, rapplic atm);
void onenradd_action(const char *str1, const char *str2, rapplic atm);
void add_action(const char *nam, const char *str, const ActionVec &decl);
void add_action(const char* nam, const char* str, const AnyV& av);
void sadd_action(const char *name, rapplic action);

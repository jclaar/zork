export module Zork:Makstr;
import ZDefs;
import :Speech;
import std;

export enum class SpeechType
{
    kVerb,
    kPrep,
    kAdj,
    kBuzz,
};

WordP make_word(SpeechType st, std::string_view val);


export void add_question(const char* str, const std::initializer_list<QuestionValue>& vector);
export void add_demon(const HackP& x);

export PrepP find_prep(std::string_view prep);
export VerbP find_verb(std::string_view verb);
export const ActionP& find_action(std::string_view action);
export direction find_dir(const std::string& dir);

// Actions
// Object support flags.
export class nrobj {};
export class robjs {};
export class reach {};
export class obj {};
export class aobjs {};
export class have {};
export class no_take {};
export class try_ {}; // Added underscore to avoid using "try" keyword
export class take {};
export class driver {};
export class flip {};
export using ALType = std::variant<std::monostate, int, reach, robjs, aobjs, no_take, have, try_, take, Bits, std::list<Bits>>;
export using AL = std::list<ALType>;
export class AVSyntax : private std::tuple<std::string_view, rapplic>
{
public:
    AVSyntax(std::string_view name, rapplic fn) : std::tuple<std::string_view, rapplic>(name, fn) {}
    template <typename Fn>
	AVSyntax(std::string_view name, Fn fn) : std::tuple<std::string_view, rapplic>(name, rapplic(fn)) {}

    std::string_view verb() const { return std::get<0>(*this); }
    rapplic fn() const { return std::get<1>(*this); }
};
export using ParseItem = std::variant<const char*, obj, nrobj, AL, AVSyntax, driver, flip>;
export using AnyV = std::vector<ParseItem>;
export using ActionVec = std::vector<AnyV>;
export void oneadd_action(const char* str1, const char* str2, rapplic atm);
export template <typename Fn>
void oneadd_action(const char* str1, const char* str2, Fn atm)
{
    oneadd_action(str1, str2, rapplic(atm));
}

export void onenradd_action(const char* str1, const char* str2, rapplic atm);
export template <typename Fn>
void onenradd_action(const char* str1, const char* str2, Fn atm)
{
	onenradd_action(str1, str2, rapplic(atm));
}
export void add_action(const char* nam, const char* str, const ActionVec& decl);
export void add_action(const char* nam, const char* str, const AnyV& av);
export void sadd_action(const char* name, rapplic action);
export template <typename Fn>
void sadd_action(const char* name, Fn action)
{
	sadd_action(name, rapplic(action));
}
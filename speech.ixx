module;
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/bitset.hpp>

export module Zork:Speech;
import :fwd;
import ZException;
import ZDefs;
import ZFlagSupport;
import ZFlagSupport;
import std;

// Flags in vword of a varg
enum class vword_flag
{
    vabit,      // Look in AOBJS
    vrbit,      // Look in ROBJS
    vtbit,      // true: Try to take the object
    vcbit,      // true: Care if can't take the object.
    vfbit,      // true: Care if can't reach the object.
    numvbits
};
constexpr size_t numvbits = std::to_underlying(vword_flag::numvbits);

export class word
{
public:
    word(std::string_view s) : _w(s) {}
    virtual ~word() {}

    const std::string& w() const { return _w; }

private:
    std::string _w;
};

export class prep_t : public word
{
public:
    prep_t(std::string_view s) : word(s) {}
};

export using PrepP = std::shared_ptr<prep_t>;

class buzz : public word
{
public:
    buzz(std::string_view s) : word(s) {}
};

class adjective : public word
{
public:
    adjective(std::string_view s) : word(s) {}
};
export using AdjectiveP = std::shared_ptr<adjective>;
inline bool operator==(const AdjectiveP& a, const std::string& s) { return a->w() == s; }
inline bool operator==(const std::string& s, const AdjectiveP& a) { return a == s; }

export using WordP = std::shared_ptr<word>;


struct _varg
{
    Flags<Bits, numbits> vbit;   // acceptable object characteristics (default any)
    Flags<Bits, numbits> vfwim;  // spec for fwimming
    PrepP vprep; // preposition that must precede(?) object
    Flags<vword_flag, numvbits> vword;
};
export using VargP = std::shared_ptr<_varg>;


struct verb : public word
{
public:
    verb(std::string_view w, rapplic vf = nullptr) : word(w), _vfcn(vf) {}

    const rapplic& vfcn() const { return _vfcn; }
    void set_vfcn(const rapplic& fn) { _vfcn = fn; }

private:
    rapplic _vfcn;
};
export using VerbP = std::shared_ptr<verb>;

// Flags for syntax
enum class SyntaxBits
{
    sflip,
    sdriver,
    snumflags
};

struct syntax
{
    VargP syn[2];
    VerbP sfcn;
    Flags<SyntaxBits, std::to_underlying(SyntaxBits::snumflags)> sflags;
};
export using SyntaxP = std::shared_ptr<syntax>;

export using vspec = std::vector<SyntaxP>;

struct Action
{
private:
    std::string vname_;
    vspec vdecl_;
    std::string vstr_;

public:
    Action(std::string_view vn, const vspec& vd, std::string_view vs) :
        vname_(vn),
        vdecl_(vd),
        vstr_(vs)
    {}

    const std::string& vname() const { return vname_; }
    const vspec& vdecl() const { return vdecl_; }
    const std::string& vstr() const { return vstr_; }
};
export using ActionP = std::shared_ptr<Action>;

class phrase
{
public:
    phrase(const WordP& p, const ObjectP& op) : _pprep(p), _pobj(op) {}

    const WordP& prep() const { return _pprep; }
    void prep(const WordP& p) { _pprep = p; }

    const ObjectP& obj() const { return _pobj; }
    void obj(const ObjectP& p) { _pobj = p; }
private:
    WordP _pprep;
    ObjectP _pobj;
};
export using PhraseP = std::shared_ptr<phrase>;
export using PhraseVecV = std::vector<PhraseP>;
PhraseP make_phrase(const WordP& p, const ObjectP& op);

export using QuestionValue = std::variant<std::string_view, ObjectP, ActionP>;

struct question
{
public:
    question(std::string_view question, const std::vector<QuestionValue>& answers) :
        _qstr(question),
        _qans(answers)
    {}

    std::string_view qstr() const { return _qstr; }
    const std::vector<QuestionValue>& qans() const { return _qans; }

private:
    std::string _qstr;
    std::vector<QuestionValue> _qans;
};
using QuestionP = std::shared_ptr<question>;

inline bool vtrnn(const VargP& va, vword_flag bit)
{
    return va->vword[bit];
}

// Check status of specific bit in object or room.
bool strnn(const SyntaxP& syn, SyntaxBits b)
{
    return syn->sflags.test(b);
}

inline PhraseP make_phrase(const WordP& p, const ObjectP& op)
{
    return std::make_shared<phrase>(p, op);
}

std::vector<QuestionP> qvec;


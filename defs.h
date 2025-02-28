#pragma once

#include <algorithm>
#include <functional>
#include <variant>
#include <list>
#include <bitset>
#include "FlagSupport.h"

// For variant stuff.
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

// Hacky method of allowing an additional argument to be passed to 
// apply_random. This is only used in a couple of cases.
enum class ApplyRandomArg
{
    read_out,
    read_in
};
// Defines a functor for an rapplic. The ApplyRandomArg argument is optional.
typedef std::optional<ApplyRandomArg> Rarg;
#define RAPPLIC(x) struct x { \
    bool operator()() const; \
    bool operator()(Rarg ra) const { return (*this)(); } \
    }
#define RAPPLIC_RARG(x) struct x { \
    bool operator()(Rarg rarg = Rarg()) const; \
}
#define RAPPLIC_DEF(x, type, def) struct x { \
    bool operator()(type v = def) const;\
    bool operator()(Rarg arg, type v = def) const { return (*this)(v); } \
    }
#define EX_RAPPLIC(x) struct x { ExitFuncVal operator()() const; }

#define HACKFN(x) struct x { \
    bool operator()(const HackP &dem) const; \
}

inline std::string operator+(std::string_view s1, std::string_view s2)
{
    std::string ss1(s1);
    ss1 += s2;
    return ss1;
}

enum class e_oactor
{
    player,
    master,
    robot,
    none
};

enum class Bits
{
    ovison,
    readbit,
    takebit,
    doorbit,
    transbit,
    foodbit,
    ndescbit,
    drinkbit,
    contbit,
    lightbit,
    vicbit,
    burnbit,
    flamebit,
    toolbit,
    turnbit,
    vehbit,
    findmebit,
    sleepbit,
    searchbit,
    sacredbit,
    tiebit,
    climbbit,
    actorbit,
    weaponbit,
    fightbit,
    villain,
    staggered,
    trytakebit,
    no_check_bit,
    openbit,
    touchbit,
    onbit,
    bunchbit,
    oglobal,
    digbit,
    lastrealbit,
    // Special bits. Anything higher than lastrealbit refers to a GOBJECT
    housebit,
    rgwater,
    treebit,
    dwindow,
    birdbit,
    wellbit,
    wall_eswbit,
    wall_nbit,
    cpladder,
    cpwall,
    slidebit,
    ropebit,
    rosebit,
    chanbit,
    guardbit,
    mirrorbit,
    panelbit,
    masterbit,
    numbits
};
constexpr size_t numbits = static_cast<int>(Bits::numbits);

enum class RoomBit
{
    rseenbit,    // "visited?"
    rlightbit,   // "endogenous light source?"
    rlandbit,    // "on land"
    rwaterbit,   // "water room"
    rairbit,     // "mid-air room"
    rsacredbit,  // "thief not allowed"
    rfillbit,    // "can fill bottle here"
    rmungbit,    // "room has been munged"
    rbuckbit,    // "this room is a bucket"
    rhousebit,   // "This room is part of the house"
    rendgame,    // "This room is in the end game"
    rnwallbit,   // "This room doesn't have walls"
    rnumbits
};
constexpr size_t rnumbits = static_cast<int>(RoomBit::rnumbits);
using RoomBits = Flags<RoomBit, rnumbits>;

using BitsList = std::list<Bits>;

enum class direction
{
    NullExit,
    North,
    South,
    East,
    West,
    Launc,
    Land,
    Se,
    Sw,
    Ne,
    Nw,
    Up,
    Down,
    Enter,
    Exit,
    Leave,
    Out,
    Cross,
    NumDirs
};

class Object;
using ObjectP = std::shared_ptr<Object>;
using ObjList = std::list<ObjectP>;
using ObjVector = std::vector<ObjectP>;
class GObject;
typedef std::shared_ptr<GObject> GObjectPtr;
class Room;
using RoomP = std::shared_ptr<Room>;
using RoomList = std::list<RoomP>;
class CEvent;
typedef std::shared_ptr<CEvent> CEventP;
typedef std::list<CEventP> EventList;
class Adv;
typedef std::unique_ptr<Adv> AdvP;
typedef std::array <AdvP, static_cast<size_t>(e_oactor::none)> AdvArray;

class hack;
typedef std::shared_ptr<hack> HackP;

template <typename T0, typename... Ts>
bool is_empty(const std::variant<T0, Ts...> &v)
{
    return std::holds_alternative<std::monostate>(v);
}

// Values that can be returned from an exit function.
using ExitFuncVal = std::variant<std::monostate, bool, RoomP>;

using rapplic = std::function<bool(Rarg)>;
using ex_rapplic = std::function<ExitFuncVal()>;
using hackfn = std::function<bool(const HackP&)>;

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
constexpr size_t numvbits = static_cast<size_t>(vword_flag::numvbits);

class word
{
public:
    word(std::string_view s) : _w(s) {}
    virtual ~word() {}

    const std::string &w() const { return _w; }

private:
    std::string _w;
};

class prep_t : public word
{
public:
    prep_t(std::string_view s) : word(s) {}
};

typedef std::shared_ptr<prep_t> PrepP;

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
using AdjectiveP = std::shared_ptr<adjective>;
inline bool operator==(const AdjectiveP& a, const std::string& s) { return a->w() == s; }
inline bool operator==(const std::string& s, const AdjectiveP& a) { return a == s; }

typedef std::shared_ptr<word> WordP;


struct _varg
{
    Flags<Bits, numbits> vbit;   // acceptable object characteristics (default any)
    Flags<Bits, numbits> vfwim;  // spec for fwimming
    PrepP vprep; // preposition that must precede(?) object
    Flags<vword_flag, numvbits> vword;
};
typedef std::shared_ptr<_varg> VargP;


struct verb : public word
{
public:
    verb(std::string_view w, rapplic vf = nullptr) : word(w), _vfcn(vf) {}
    
    const rapplic &vfcn() const { return _vfcn; }
    void set_vfcn(const rapplic &fn) { _vfcn = fn; }

private:
    rapplic _vfcn;
};
typedef std::shared_ptr<verb> VerbP;

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
    Flags<SyntaxBits, static_cast<size_t>(SyntaxBits::snumflags)> sflags;
};
typedef std::shared_ptr<syntax> SyntaxP;

typedef std::vector<SyntaxP> vspec;

struct Action
{
private:
    std::string vname_;
    vspec vdecl_;
    std::string vstr_;

public:
    Action(std::string_view vn, const vspec &vd, std::string_view vs) :
        vname_(vn),
        vdecl_(vd),
        vstr_(vs)
    { }

    const std::string &vname() const { return vname_; }
    const vspec &vdecl() const { return vdecl_; }
    const std::string &vstr() const { return vstr_; }
};
typedef std::shared_ptr<Action> ActionP;

class phrase
{
public:
    phrase(const WordP &p, const ObjectP &op) : _pprep(p), _pobj(op) {}

    const WordP &prep() const { return _pprep; }
    void prep(const WordP &p) { _pprep = p; }

    const ObjectP &obj() const { return _pobj; }
    void obj(const ObjectP &p) { _pobj = p; }
private:
    WordP _pprep;
    ObjectP _pobj;
};
typedef std::shared_ptr<phrase> PhraseP;
typedef std::vector<PhraseP> PhraseVecV;
PhraseP make_phrase(const WordP &p, const ObjectP &op);

typedef std::variant<std::string_view, ObjectP, ActionP> QuestionValue;

struct question
{
public:
    question(std::string_view question, const std::vector<QuestionValue> &answers) :
        _qstr(question),
        _qans(answers)
    {}

    std::string_view qstr() const { return _qstr; }
    const std::vector<QuestionValue> &qans() const { return _qans; }

private:
    std::string _qstr;
    std::vector<QuestionValue> _qans;
};
typedef std::shared_ptr<question> QuestionP;

inline bool vtrnn(const VargP &va, vword_flag bit)
{
    return va->vword[bit];
}

// ORPHANS -- mysterious vector of orphan data
typedef std::variant<std::monostate, ObjectP, PhraseP> OrphanSlotType;
class Orphans
{
public:
    Orphans() :
        _oflag(false)
    {
    }

    bool oflag() const { return _oflag; }
    void oflag(bool f) { _oflag = f; }

    const ActionP &overb() const { return _overb; }
    void overb(const ActionP &p) { _overb = p; }

    const PrepP &oprep() const { return _oprep; }
    void oprep(const PrepP &prep) { _oprep = prep; }

    const std::string &oname() const { return _oname; }
    void oname(std::string_view name) { _oname = name; }

    const ObjectP &oslot1() const { return _oslot1; }
    void oslot1(const OrphanSlotType &a) {
        static_assert(std::variant_size<OrphanSlotType>() == 3);
        std::visit(overload{
            [&](const ObjectP& op) { _oslot1 = op; },
            [&](const PhraseP& pp) { _oslot1 = pp->obj(); },
            [&](auto p) { _oslot1.reset(); }
            }, a);
    }

    const OrphanSlotType &oslot2() const { return _oslot2; }
    void oslot2(const OrphanSlotType &a) { _oslot2 = a; }

private:
    bool _oflag;
    ActionP _overb;
    ObjectP _oslot1;
    OrphanSlotType _oslot2;
    PrepP _oprep;
    std::string _oname;
};

bool apply_object(const ObjectP &op);
bool describable(const ObjectP &op);
bool see_inside(const ObjectP &op);

extern int no_tell;
extern int eg_score;

bool apply_random(const rapplic& fcn);
inline bool apply_random(rapplic fcn, ApplyRandomArg arg)
{
    return fcn(arg);
}
ExitFuncVal apply_random(ex_rapplic fcn);
bool apply_random(hackfn fcn, const HackP &demon);

// oflags, rflags testers and setter

// Check status of specific bit in object or room.
bool trnn_bits(const ObjectP& op, const Flags<Bits, numbits>& bits_to_check);
void trc(const ObjectP &op, Bits b);
bool strnn(const SyntaxP &syn, SyntaxBits b);
bool gtrnn(const RoomP &, Bits);
// Set or 0 object bit or bits.
void rtrc(const RoomP &p, RoomBit b);

template <typename T>
int length(const T& c)
{
    return (int) c.size();
}


bool flaming(const ObjectP &obj);


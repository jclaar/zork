#pragma once

#include <vector>
#include <memory>
#include <bitset>
#include <algorithm>
#include <set>
#include <variant>
#include <list>
#include <any>
#include <optional>
#include <sstream>
#include <string_view>

inline std::string operator+(std::string_view s1, std::string_view s2)
{
    std::stringstream ss;
    ss << s1 << s2;
    return ss.str();
}

enum e_oactor
{
    oa_player,
    oa_master,
    oa_robot,
    oa_none
};

enum Bits
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

typedef std::list<Bits> BitsList;

enum direction
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
typedef std::shared_ptr<Object> ObjectP;
typedef std::list<ObjectP> ObjList;
typedef std::vector<ObjectP> ObjVector;
class GObject;
typedef std::shared_ptr<GObject> GObjectPtr;
class Room;
typedef std::shared_ptr<Room> RoomP;
typedef std::list<RoomP> RoomList;
class CEvent;
typedef std::shared_ptr<CEvent> CEventP;
typedef std::list<CEventP> EventList;
class Adv;
typedef std::unique_ptr<Adv> AdvP;

class hack;
typedef std::shared_ptr<hack> HackP;

template <typename T>
bool is_empty(const T &v)
{
    return std::get_if<std::monostate>(&v) != nullptr;
}

// Values that can be returned from an exit function.
typedef std::variant<std::monostate, bool, RoomP> ExitFuncVal;

typedef bool(*rapplic)(); // Action functions
typedef ExitFuncVal(*ex_rapplic)(); // Exit functions
typedef bool(*hackfn)(const HackP &demon);

// Flags in vword of a varg
enum vword_flag
{
    vabit,      // Look in AOBJS
    vrbit,      // Look in ROBJS
    vtbit,      // true: Try to take the object
    vcbit,      // true: Care if can't take the object.
    vfbit,      // true: Care if can't reach the object.
    numvbits
};

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
typedef std::shared_ptr<adjective> AdjectiveP;

typedef std::shared_ptr<word> WordP;


struct _varg
{
    std::bitset<numbits> vbit;   // acceptable object characteristics (default any)
    std::bitset<numbits> vfwim;  // spec for fwimming
    PrepP vprep; // preposition that must precede(?) object
    std::bitset<numvbits> vword;
};
typedef std::shared_ptr<_varg> VargP;


struct verb : public word
{
public:
    verb(std::string_view w, rapplic vf = nullptr) : word(w), _vfcn(vf) {}
    
    rapplic vfcn() const { return _vfcn; }
    void set_vfcn(rapplic fn) { _vfcn = fn; }

private:
    rapplic _vfcn;
};
typedef std::shared_ptr<verb> VerbP;

// Flags for syntax
enum SyntaxBits
{
    sflip,
    sdriver,
    snumflags
};

struct syntax
{
    VargP syn[2];
    VerbP sfcn;
    std::bitset<snumflags> sflags;
};
typedef std::shared_ptr<syntax> SyntaxP;

typedef std::vector<SyntaxP> vspec;

struct action
{
private:
    std::string vname_;
    vspec vdecl_;
    std::string vstr_;

public:
    action(std::string_view vn, const vspec &vd, std::string_view vs) :
        vname_(vn),
        vdecl_(vd),
        vstr_(vs)
    { }

    const std::string &vname() const { return vname_; }
    const vspec &vdecl() const { return vdecl_; }
    const std::string &vstr() const { return vstr_; }
};
typedef std::shared_ptr<action> ActionP;

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

typedef std::variant<std::string, ObjectP, ActionP> QuestionValue;

struct question
{
public:
    question(std::string_view question, const std::vector<QuestionValue> &answers) :
        _qstr(question),
        _qans(answers)
    {}

    const std::string &qstr() const { return _qstr; }
    const std::vector<QuestionValue> &qans() const { return _qans; }

private:
    std::string _qstr;
    std::vector<QuestionValue> _qans;
};
typedef std::shared_ptr<question> QuestionP;

inline bool vtrnn(const VargP &va, vword_flag bit)
{
    return va->vword[bit] == 1;
}

// ORPHANS -- mysterious vector of orphan data
typedef std::variant<std::monostate, ObjectP, PhraseP> OrphanSlotType;
class Orphans
{
public:
    Orphans(bool oflg = false, ActionP vrb = nullptr, OrphanSlotType slot1 = std::monostate(), PrepP prep = nullptr,
        const std::string &nm = std::string()) :
        _oflag(oflg), _overb(vrb), _oprep(prep), _oname(nm)
    {
        if (auto op = std::get_if<ObjectP>(&slot1))
            _oslot1 = *op;
        else if (auto pp = std::get_if<PhraseP>(&slot1))
            _oslot1 = (*pp)->obj();
    }

    bool oflag() const { return _oflag; }
    void oflag(bool f) { _oflag = f; }

    const ActionP &overb() const { return _overb; }
    void overb(const ActionP &p) { _overb = p; }

    const PrepP &oprep() const { return _oprep; }
    void oprep(const PrepP &prep) { _oprep = prep; }

    const std::string &oname() const { return _oname; }
    void oname(const std::string &name) { _oname = name; }

    const ObjectP &oslot1() const { return _oslot1; }
    void oslot1(const OrphanSlotType &a) {
        static_assert(std::variant_size<OrphanSlotType>() == 3);
        if (is_empty(a))
            _oslot1.reset();
        else if (auto op = std::get_if<ObjectP>(&a))
            _oslot1 = *op;
        else
            _oslot1 = std::get<PhraseP>(a)->obj();
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
typedef std::shared_ptr<Orphans> OrphanP;

bool apply_object(const ObjectP &op);
bool describable(const ObjectP &op);
bool see_inside(const ObjectP &op);

extern int no_tell;
extern int eg_score;

// Hacky method of allowing an additional argument to be passed to 
// apply_random. This is only used in a couple of cases.
enum ApplyRandomArg
{
    read_out,
    read_in
};
extern std::optional<ApplyRandomArg> arg;
bool apply_random(rapplic fcn, std::optional<ApplyRandomArg> arg = std::optional<ApplyRandomArg>());
ExitFuncVal apply_random(ex_rapplic fcn);
bool apply_random(hackfn fcn, const HackP &demon);

// oflags, rflags testers and setter

// Check status of specific bit in object or room.
template <Bits b>
bool trnnt(const ObjectP &op);
#define trnn(obj, b) trnnt<b>(obj)

bool trnn_list(const ObjectP &op, const std::initializer_list<Bits> &bits_to_check);
bool trnn_bits(const ObjectP &op, const std::bitset<numbits> &bits_to_check);
void trc(const ObjectP &op, Bits b);
bool strnn(const SyntaxP &syn, SyntaxBits b);
bool gtrnn(const RoomP &, Bits);
// Set or 0 object bit or bits.
void rtrc(const RoomP &p, Bits b);

inline bool openable(const ObjectP &op)
{
    return trnn_list(op, { doorbit, contbit });
}

inline int length(const RoomList &rl) { return (int)rl.size(); }

bool flaming(const ObjectP &obj);

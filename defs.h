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

typedef int pstring;
typedef int noffset;

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
typedef std::shared_ptr<Adv> AdvP;

class hack;
typedef std::shared_ptr<hack> HackP;

// Values that can be returned from an exit function.
typedef std::variant<std::monostate, bool, RoomP> ExitFuncVal;
enum { kefv_none, kefv_bool, kefv_roomp };

typedef bool(*rapplic)(); // Action functions
typedef ExitFuncVal(*ex_rapplic)(); // Exit functions
typedef bool(*hackfn)(HackP demon);

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
    word(const std::string &s) : _w(s) {}
    virtual ~word() {}

    const std::string &w() const { return _w; }

private:
    std::string _w;
};

class prep_t : public word
{
public:
    prep_t(const std::string &s) : word(s) {}
};

typedef std::shared_ptr<prep_t> PrepP;

class buzz : public word
{
public:
    buzz(const std::string &s) : word(s) {}
};

class adjective : public word
{
public:
    adjective(const std::string &s) : word(s) {}
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
    verb(const std::string &w, rapplic vf = nullptr) : word(w), _vfcn(vf) {}
    
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
    action(const std::string &vn, const vspec &vd, const std::string &vs) :
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
    phrase(const WordP &p, ObjectP op) : _pprep(p), _pobj(op) {}

    WordP prep() const { return _pprep; }
    void prep(WordP p) { _pprep = p; }

    ObjectP obj() const { return _pobj; }
    void obj(ObjectP p) { _pobj = p; }
private:
    WordP _pprep;
    ObjectP _pobj;
};
typedef std::shared_ptr<phrase> PhraseP;
typedef std::vector<PhraseP> PhraseVecV;
typedef std::vector<PhraseP> PrepVecV;
PhraseP make_phrase(const WordP &p, ObjectP op);

typedef std::variant<std::string, ObjectP, ActionP> QuestionValue;
enum { kqv_string, kqv_object, kqv_action };

struct question
{
public:
    question(const std::string &question, const std::vector<QuestionValue> &answers) :
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
enum { kos_none, kos_object, kos_phrase };
class Orphans
{
public:
    Orphans(bool oflg = false, ActionP vrb = nullptr, OrphanSlotType slot1 = std::monostate(), PrepP prep = nullptr,
        const std::string &nm = std::string()) :
        _oflag(oflg), _overb(vrb), _oprep(prep), _oname(nm)
    {
        if (slot1.index() == kos_object)
            _oslot1 = std::get<kos_object>(slot1);
        else if (slot1.index() == kos_phrase)
            _oslot1 = std::get<kos_phrase>(slot1)->obj();
    }

    bool oflag() const { return _oflag; }
    void oflag(bool f) { _oflag = f; }

    ActionP overb() const { return _overb; }
    void overb(ActionP p) { _overb = p; }

    PrepP oprep() const { return _oprep; }
    void oprep(PrepP prep) { _oprep = prep; }

    const std::string &oname() const { return _oname; }
    void oname(const std::string &name) { _oname = name; }

    const ObjectP &oslot1() const { return _oslot1; }
    void oslot1(const OrphanSlotType &a) {
        switch (a.index())
        {
        case kos_none:
            _oslot1.reset();
            break;
        case kos_object:
            _oslot1 = std::get<kos_object>(a);
            break;
        case kos_phrase:
            _oslot1 = std::get<kos_phrase>(a)->obj();
            break;
        }
    }

    const OrphanSlotType &oslot2() const { return _oslot2; }
    void oslot2(OrphanSlotType a) { _oslot2 = a; }

private:
    bool _oflag;
    ActionP _overb;
    ObjectP _oslot1;
    OrphanSlotType _oslot2;
    PrepP _oprep;
    std::string _oname;
};
typedef std::shared_ptr<Orphans> OrphanP;

bool apply_object(ObjectP op);
bool describable(ObjectP op);
bool see_inside(ObjectP op);

extern int no_tell;
extern int eg_score;

bool verbq(const char *al);
bool verbq(const std::initializer_list<const char*> &verbs);

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
bool apply_random(hackfn fcn, HackP demon);

// oflags, rflags testers and setter

// Check status of specific bit in object or room.
bool trnn(ObjectP op, Bits b);
bool trnn(ObjectP op, const std::initializer_list<Bits> &bits_to_check);
bool trnn(ObjectP op, const std::bitset<numbits> &bits_to_check);
void trc(ObjectP op, Bits b);
bool strnn(SyntaxP syn, SyntaxBits b);
bool rtrnn(RoomP p, Bits b);
bool rtrnn(RoomP p, const std::initializer_list<Bits> &bits);
bool gtrnn(RoomP, const std::string &);
// Set or 0 room bit
void rtro(RoomP p, Bits b);
bool rtrz(RoomP p, Bits b);
bool rtrz(RoomP p, const std::initializer_list<Bits> &bits);
// Set or 0 object bit or bits.
ObjectP tro(ObjectP op, Bits b);
void tro(ObjectP op, const std::initializer_list<Bits> &b);
int trz(ObjectP op, Bits b);
void trz(ObjectP op, const std::initializer_list<Bits> &b);
void rtrc(RoomP p, Bits b);

inline bool openable(ObjectP op)
{
    return trnn(op, { doorbit, contbit });
}

inline int length(const RoomList &rl) { return (int)rl.size(); }

bool flaming(ObjectP obj);

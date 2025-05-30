#pragma once
#include <map>
#include <vector>
#include <array>
#include <span>
#include <string>
#include "defs.h"
#include "makstr.h"
#include "room.h"
#include "funcs.h"
#include "melee.h"

using WordsPobl = std::map<std::string, WordP, std::less<>>;
using DirectionsPobl = std::map<std::string_view, direction, std::less<>>;
using ActionsPobl = std::map<std::string, ActionP, std::less<>>;

extern WordsPobl words_pobl;
extern DirectionsPobl directions_pobl;
extern ActionsPobl actions_pobl;
extern const ObjList small_papers;
extern const ObjList palobjs;
extern ObjList inqobjs;
extern ObjectP bunch_obj;
extern SIterator indentstr;
extern RoomP mloc;
extern GObjectPtr it_object;
extern int deaths;
extern HackP robber_demon;
extern HackP sword_demon;
extern HackP fight_demon;
extern HackP clocker;
extern VerbP buncher;
extern int cyclowrath;
extern std::vector<VerbP> robot_actions;
extern std::vector<VerbP> master_actions;
extern RoomP bloc;
extern const RoomP startroom;
extern const RoomP &northend;
extern const RoomP &southend;
extern const ObjList cobjs;
extern const ObjList nobjs;
extern const ObjList pobjs;
extern std::array<ObjList, 8> cells;

using NumObjs = std::pair<std::string_view, int>;
constexpr std::array numobjs =
{
    NumObjs{"ONE", 1},
    NumObjs{"TWO", 2},
    NumObjs{"THREE", 3},
    NumObjs{"FOUR", 4},
    NumObjs{"FIVE", 5},
    NumObjs{"SIX", 6},
    NumObjs{"SEVEN", 7},
    NumObjs{"EIGHT", 8}
};

// Direction vector for mirror
using DVPair = std::pair<direction, int>;
inline bool operator==(const DVPair& dp, direction d) { return std::get<0>(dp) == d; }
inline bool operator==(direction d, const DVPair& dp) { return dp == d; }
constexpr std::array dirvec = {
    DVPair(direction::North, 0),
    DVPair(direction::Ne, 45),
    DVPair(direction::East, 90),
    DVPair(direction::Se, 135),
    DVPair(direction::South, 180),
    DVPair(direction::Sw, 225),
    DVPair(direction::West, 270),
    DVPair(direction::Nw, 315),
};

class hack
{
public:
    hack(hackfn ha, const ObjList &ho, const RoomList &hr, const RoomP &rm, const ObjectP &obj) :
        _haction(ha), _room(rm), _hobj(obj), _hobjs_ob(ho), _hrooms(hr), _hflag(false)
    {
    }

    hack(const hack &h)
    {
        copy(h);
    }

    hack &operator=(const hack &h)
    {
        copy(h);
        return *this;
    }

    hackfn haction() const { return _haction; }
    void haction(hackfn fn) { _haction = fn; }

    bool hflag() const { return _hflag; }
    void hflag(bool flg) { _hflag = flg; }
    const ObjectP &hobj() const { return _hobj; }
    const RoomP &hroom() { return _room; }
    void hroom(const RoomP &rm) { _room = rm; }
    const RoomList &hrooms() const { return _hrooms; }
    RoomList &hrooms() { return _hrooms; }

    const ObjList &hobjs_ob() const
    {
        return _hobjs_ob;
    }
    void hobjs(const ObjList &ol)
    {
        _hobjs_ob = ol;
    }

    const EventList &hobjs_ev() const
    {
        return _hobjs_ev;
    }
    void hobjs(const EventList &el)
    {
        _hobjs_ev = el;
    }

    void hobjs_add(const CEventP &ev)
    {
        _hobjs_ev.push_front(ev);
    }

    void hobjs_add(const ObjectP &ob)
    {
        _hobjs_ob.push_front(ob);
    }

private:
    hackfn _haction = nullptr;
    EventList _hobjs_ev;
    ObjList _hobjs_ob;
    RoomList _hrooms;
    RoomP _room;
    ObjectP _hobj;
    bool _hflag = false;

    void copy(const hack &s)
    {
        _haction = s._haction;
        _hobjs_ob = s._hobjs_ob;
        _hobjs_ev = s._hobjs_ev;
        _hrooms = s._hrooms;
        _room = s._room;
        _hobj = s._hobj;
        _hflag = s._hflag;
    }
};

typedef std::shared_ptr<hack> HackP;

// Puzzle room
struct CpExit
{
    direction dir;
    int offset;
    constexpr CpExit(direction d, int o) : dir(d), offset(o) {}
};
inline bool operator==(const CpExit& cp, direction d) { return cp.dir == d; }
inline bool operator==(direction d, const CpExit& cp) { return cp == d; }
constexpr std::array cpexits = {
    CpExit(direction::North, -8),
    CpExit(direction::South, 8),
    CpExit(direction::East, 1),
    CpExit(direction::West, -1),
    CpExit(direction::Ne, -7),
    CpExit(direction::Nw, -9),
    CpExit(direction::Se, 9),
    CpExit(direction::Sw, 7),
};

// Bank puzzle
extern RoomP scol_room;
extern RoomP scol_active;
struct ScolRooms
{
    direction dir;
    const char *rm;
};
inline bool operator==(direction d, const ScolRooms& sr) { return sr.dir == d; }
inline bool operator==(const ScolRooms& sr, direction d) { return d == sr; }

struct ScolWalls
{
    std::string_view rm1;
    std::string_view obj;
    std::string_view rm2;
};

using namespace std::string_view_literals;

constexpr std::array scol_walls =
{
    ScolWalls{ "BKVW"sv, "WEAST"sv, "BKVE"sv },
    ScolWalls{ "BKVE"sv, "WWEST"sv, "BKVW"sv },
    ScolWalls{ "BKTWI"sv, "WSOUT"sv, "BKVAU"sv },
    ScolWalls{ "BKVAU"sv, "WNORT"sv, "BKTWI"sv }
};

constexpr std::array scol_rooms =
{
    ScolRooms{direction::East, "BKVE"},
    ScolRooms{direction::West, "BKVW"},
    ScolRooms{direction::North, "BKTWI"},
    ScolRooms{direction::South, "BKVAU"}
};

extern const ObjList villains;
extern ObjList oppv;
extern std::vector<int> villain_probs;

typedef std::tuple<ObjectP, ObjectP, int> BestWeapons;
typedef std::array<BestWeapons, 2> BestWeaponsList;

// Parse vector is defined in parser.mud. It is a 3-element vector,
// containing various items:
// 0: ActionP or VerbP
// 1: ObjectP or direction
// 2: ObjectP or nothing
typedef std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction> ParseVecVal;
typedef std::array<ParseVecVal, 3> ParseVecA;
typedef std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction, WordP, std::string, ObjList> ParseAval;

inline ParseVecVal as_pvv(const ParseAval &pv)
{
    return std::visit(overload{
            [](const ActionP& ap) { return ParseVecVal(ap); },
            [](const VerbP& vp) { return ParseVecVal(vp); },
            [](const ObjectP& op) { return ParseVecVal(op); },
            [](const PhraseP& pp) { return ParseVecVal(pp); },
            [](direction d) { return ParseVecVal(d); },
            [](auto unused) { return ParseVecVal(); }
        }, pv);
}

inline OrphanSlotType as_ost(ParseVecVal pv)
{
    return std::visit(overload{
        [](const ObjectP& op) { return OrphanSlotType(op); },
        [](const PhraseP& pp) { return OrphanSlotType(pp); },
        [](auto unused) { return OrphanSlotType(); }
        }, pv);
}

inline direction as_dir(const ParseVecVal &a)
{
    return std::get<direction>(a);
}

inline ObjectP as_obj(const ParseVecVal &pvv)
{
    try
    {
        return std::get<ObjectP>(pvv);
    }
    catch (std::bad_variant_access&)
    {
        return ObjectP();
    }
}

inline WordP as_word(const ParseAval &a)
{
    return std::get<WordP>(a);
}

inline VerbP as_verb(const ParseVecVal &a)
{
    try
    {
        return std::get<VerbP>(a);
    }
    catch (std::bad_variant_access&)
    {
        return VerbP();
    }
}

void dir_syns();

void init_dung();

template <typename T>
void synonym(const char *n1, T n2)
{
    const WordP &wp = words_pobl[n1];
    _ASSERT(wp);
    words_pobl[n2] = wp;
}
template <typename T, typename ...Args>
void synonym(const char *n1, T first, Args... args)
{
    synonym(n1, first);
    synonym(n1, args...);
}

inline void dsynonym(const char *dir, const char *syn)
{
    auto iter = directions_pobl.find(dir);
    if (iter == directions_pobl.end())
        error("Invalid direction synonym added");
    directions_pobl[syn] = iter->second;
}

template <typename T>
void vsynonym(const char *verb, T syn)
{
    actions_pobl[syn] = actions_pobl[verb];
}

template <typename T, typename ...Args>
void vsynonym(const char *verb, T first, Args... args)
{
    vsynonym(verb, first);
    vsynonym(verb, args...);
}

template <typename T>
void add_zork(SpeechType st, T wc)
{
    // One hack -- remove LOWER from the adjective list so that
    // it doesn't conflict with the verb LOWER. I don't know why 
    // this isn't a problem in the MDL code? I'm guessing because
    // of the different between a STRING and a PSTRING?
    std::string w = wc;
    if (w != "LOWER")
    {
        words_pobl[w] = make_word(st, wc);
    }
}

template <typename T, typename ...Args>
void add_zork(SpeechType st, T first, Args... args)
{
    add_zork(st, first);
    add_zork(st, args...);
}

template <typename T>
void add_buzz(T w)
{
    add_zork(SpeechType::kBuzz, w);
}

template <typename T, typename ...Args>
void add_buzz(T first, Args... args)
{
    add_buzz(first);
    add_buzz(args...);
}
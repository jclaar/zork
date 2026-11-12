#pragma once
#include <map>
#include <vector>
#include <array>
#include <span>
#include <string>
#include "defs.h"
#include "makstr.h"
#include "funcs.h"
#include "melee.h"
#include <memory>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

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
        _haction(ha), _hroom(rm), _hobj(obj), _hobjs_ob(ho), _hrooms(hr), _hflag(false)
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

    PROP(haction);
    PROP(hflag);
    PROP(hroom);

    const ObjectP &hobj() const { return _hobj; }
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

    template <typename T>
        requires (std::same_as<T, CEventP> || std::same_as<T, ObjectP>)
    void hobjs_add(const T &obj)
    {
        if constexpr (std::same_as<T, CEventP>)
            _hobjs_ev.push_front(obj);
        else
            _hobjs_ob.push_front(obj);
    }

private:
    hackfn _haction = nullptr;
    EventList _hobjs_ev;
    ObjList _hobjs_ob;
    RoomList _hrooms;
    RoomP _hroom;
    ObjectP _hobj;
    bool _hflag = false;

    void copy(const hack &s)
    {
        _haction = s._haction;
        _hobjs_ob = s._hobjs_ob;
        _hobjs_ev = s._hobjs_ev;
        _hrooms = s._hrooms;
        _hroom = s._hroom;
        _hobj = s._hobj;
        _hflag = s._hflag;
    }
};

using HackP = std::shared_ptr<hack>;

// Bank puzzle
extern RoomP scol_room;
extern RoomP scol_active;

extern const ObjList villains;
extern ObjList oppv;
extern std::vector<int> villain_probs;

using BestWeapons = std::tuple<ObjectP, ObjectP, int>;
bool operator==(const ObjectP& villain, const BestWeapons& bw);
inline bool operator==(const BestWeapons& bw, const ObjectP& villain) { return villain == bw; }
using BestWeaponsList = std::array<BestWeapons, 2>;
extern const BestWeaponsList best_weapons;

// Parse vector is defined in parser.mud. It is a 3-element vector,
// containing various items:
// 0: ActionP or VerbP
// 1: ObjectP or direction
// 2: ObjectP or nothing
using ParseVecVal = std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction>;
using ParseVecA = std::array<ParseVecVal, 3>;
using ParseAval = std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction, WordP, std::string, ObjList>;

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


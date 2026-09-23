module;
#include "defs.h"

export module Zork:Dungeon;
import ZException;
import :Speech;
import :Melee;
import :Object;
import :fwd;
import ZFuncs;
import ZDefs;
import :CEvent;
import :Makstr;
import std;

export using WordsPobl = std::map<std::string, WordP, std::less<>>;
export using DirectionsPobl = std::map<std::string_view, direction, std::less<>>;
export using ActionsPobl = std::map<std::string, ActionP, std::less<>>;

export WordsPobl words_pobl;
export int cphere;
namespace
{
    auto mp = [](const char* sd, direction d)
        {
            return std::make_pair<DirectionsPobl::key_type>(sd, d);
        };
}
export DirectionsPobl directions_pobl = {
        mp("#!#!#", direction::NullExit),
        mp("NORTH", direction::North),
        mp("SOUTH", direction::South),
        mp("EAST", direction::East),
        mp("WEST", direction::West),
        mp("LAUNC", direction::Launc),
        mp("LAND", direction::Land),
        mp("SE", direction::Se),
        mp("SW", direction::Sw),
        mp("NE", direction::Ne),
        mp("NW", direction::Nw),
        mp("UP", direction::Up),
        mp("DOWN", direction::Down),
        mp("ENTER", direction::Enter),
        mp("EXIT", direction::Exit),
        mp("CROSS", direction::Cross)
};
export ActionsPobl actions_pobl;
extern const ObjList small_papers;
extern const ObjList palobjs;
export extern ObjList inqobjs;
export extern ObjectP bunch_obj;
export extern SIterator indentstr;
export extern GObjectPtr it_object;
export int deaths = 0;
export extern HackP robber_demon;
export extern HackP sword_demon;
extern HackP fight_demon;
extern HackP clocker;
export extern VerbP buncher;
using ASSpan = std::span<const attack_state>;
extern VerbP buncher;
export using ASSpan = std::span<const attack_state>;
// Attacking things...
namespace {
    constexpr auto def1 = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::unconscious, attack_state::unconscious,
        attack_state::killed, attack_state::killed, attack_state::killed, attack_state::killed, attack_state::killed });
    constexpr auto def2a = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound,
        attack_state::unconscious });
    constexpr auto def2b = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound, attack_state::light_wound,
        attack_state::unconscious,
        attack_state::killed, attack_state::killed, attack_state::killed });
    constexpr auto def3a = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound,
        attack_state::serious_wound, attack_state::serious_wound });
    constexpr auto def3b = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound, attack_state::light_wound,
        attack_state::serious_wound, attack_state::serious_wound, attack_state::serious_wound });
    constexpr auto def3c = std::to_array({ attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound, attack_state::light_wound, attack_state::light_wound,
        attack_state::serious_wound, attack_state::serious_wound, attack_state::serious_wound });
}

constexpr std::array def1_res{
    ASSpan(def1.data(), def1.size()),
    ASSpan(def1.data() + 1, def1.size() - 1),
    ASSpan(def1.data() + 2, def1.size() - 2)
};
constexpr std::array def2_res{
    ASSpan(def2a.data(), def2a.size()),
    ASSpan(def2b.data(), def2b.size()),
    ASSpan(def2b.data() + 1, def2b.size() - 1),
    ASSpan(def2b.data() + 2, def2b.size() - 2)
};
constexpr std::array def3_res{
    ASSpan(def3a.data(), def3a.size()),
    ASSpan(def3a.data() + 1, def3a.size() - 1),
    ASSpan(def3b.data(), def3b.size()),
    ASSpan(def3b.data() + 1, def3b.size() - 1),
    ASSpan(def3c.data(), def3c.size())
};

export int cyclowrath = 0;
extern std::vector<VerbP> robot_actions;
export extern std::vector<VerbP> master_actions;
export extern RoomP bloc;
export extern const RoomP startroom;
export extern const RoomP& northend;
export extern const RoomP& southend;
export extern const ObjList cobjs;
export extern const ObjList nobjs;
export extern const ObjList pobjs;
export extern std::array<ObjList, 8> cells;

using NumObjs = std::pair<std::string_view, int>;
export constexpr std::array numobjs =
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
export bool operator==(const DVPair& dp, direction d) { return std::get<0>(dp) == d; }
export bool operator==(direction d, const DVPair& dp) { return dp == d; }
export constexpr std::array dirvec = {
    DVPair(direction::North, 0),
    DVPair(direction::Ne, 45),
    DVPair(direction::East, 90),
    DVPair(direction::Se, 135),
    DVPair(direction::South, 180),
    DVPair(direction::Sw, 225),
    DVPair(direction::West, 270),
    DVPair(direction::Nw, 315),
};

export class hack
{
public:
    hack(hackfn ha, const ObjList& ho, const RoomList& hr, const RoomP& rm, const ObjectP& obj) :
        _haction(ha), _room(rm), _hobj(obj), _hobjs_ob(ho), _hrooms(hr), _hflag(false)
    {}

    hack(const hack& h)
    {
        copy(h);
    }

    hack& operator=(const hack& h)
    {
        copy(h);
        return *this;
    }

	~hack() = default;

    PROP(haction);

    bool hflag() const { return _hflag; }
    void hflag(bool flg) { _hflag = flg; }
    const ObjectP& hobj() const { return _hobj; }
    const RoomP& hroom() { return _room; }
    void hroom(const RoomP& rm) { _room = rm; }
    const RoomList& hrooms() const { return _hrooms; }
    RoomList& hrooms() { return _hrooms; }

    const ObjList& hobjs_ob() const
    {
        return _hobjs_ob;
    }
    void hobjs(const ObjList& ol)
    {
        _hobjs_ob = ol;
    }

    const EventList& hobjs_ev() const
    {
        return _hobjs_ev;
    }
    void hobjs(const EventList& el)
    {
        _hobjs_ev = el;
    }

    void hobjs_add(const CEventP& ev)
    {
        _hobjs_ev.push_front(ev);
    }

    void hobjs_add(const ObjectP& ob)
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

    void copy(const hack& s)
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

using hackfn = std::function<bool(const HackP&)>;


// Puzzle room
struct CpExit
{
    direction dir;
    int offset;
    constexpr CpExit(direction d, int o) : dir(d), offset(o) {}
};
export bool operator==(const CpExit& cp, direction d) { return cp.dir == d; }
export bool operator==(direction d, const CpExit& cp) { return cp == d; }
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
    const char* rm;
};
export bool operator==(direction d, const ScolRooms& sr) { return sr.dir == d; }
export bool operator==(const ScolRooms& sr, direction d) { return d == sr; }

struct ScolWalls
{
    std::string_view rm1;
    std::string_view obj;
    std::string_view rm2;
};

using namespace std::string_view_literals;

constexpr std::array scol_walls =
{
    ScolWalls{ "BKVW", "WEAST", "BKVE" },
    ScolWalls{ "BKVE", "WWEST", "BKVW" },
    ScolWalls{ "BKTWI", "WSOUT", "BKVAU" },
    ScolWalls{ "BKVAU", "WNORT", "BKTWI" }
};

constexpr std::array scol_rooms =
{
    ScolRooms{direction::East, "BKVE"},
    ScolRooms{direction::West, "BKVW"},
    ScolRooms{direction::North, "BKTWI"},
    ScolRooms{direction::South, "BKVAU"}
};

export extern const ObjList villains;
export extern ObjList oppv;
export extern std::vector<int> villain_probs;

export using BestWeapons = std::tuple<ObjectP, ObjectP, int>;
export bool operator==(const ObjectP& villain, const BestWeapons& bw);
export bool operator==(const BestWeapons& bw, const ObjectP& villain) { return villain == bw; }
export using BestWeaponsList = std::array<BestWeapons, 2>;
export extern const BestWeaponsList best_weapons;

// Parse vector is defined in parser.mud. It is a 3-element vector,
// containing various items:
// 0: ActionP or VerbP
// 1: ObjectP or direction
// 2: ObjectP or nothing
export using ParseVecVal = std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction>;
export using ParseVecA = std::array<ParseVecVal, 3>;
export using ParseAval = std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction, WordP, std::string, ObjList>;

export ParseVecVal as_pvv(const ParseAval& pv)
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

export OrphanSlotType as_ost(ParseVecVal pv)
{
    return std::visit(overload{
        [](const ObjectP& op) { return OrphanSlotType(op); },
        [](const PhraseP& pp) { return OrphanSlotType(pp); },
        [](auto unused) { return OrphanSlotType(); }
        }, pv);
}

export direction as_dir(const ParseVecVal& a)
{
    return std::get<direction>(a);
}

export ObjectP as_obj(const ParseVecVal& pvv)
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

export WordP as_word(const ParseAval& a)
{
    return std::get<WordP>(a);
}

export VerbP as_verb(const ParseVecVal& a)
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

export void init_dung();

export template <typename T>
void synonym(const char* n1, T n2)
{
    const WordP& wp = words_pobl[n1];
    words_pobl[n2] = wp;
}
export template <typename T, typename ...Args>
void synonym(const char* n1, T first, Args... args)
{
    synonym(n1, first);
    synonym(n1, args...);
}

export void dsynonym(const char* dir, const char* syn)
{
    auto iter = directions_pobl.find(dir);
    if (iter == directions_pobl.end())
        error("Invalid direction synonym added");
    directions_pobl[syn] = iter->second;
}

export template <typename T>
void vsynonym(const char* verb, T syn)
{
    actions_pobl[syn] = actions_pobl[verb];
}

export template <typename T, typename ...Args>
void vsynonym(const char* verb, T first, Args... args)
{
    vsynonym(verb, first);
    vsynonym(verb, args...);
}

export template <typename T>
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

export template <typename T, typename ...Args>
void add_zork(SpeechType st, T first, Args... args)
{
    add_zork(st, first);
    add_zork(st, args...);
}

export template <typename T>
void add_buzz(T w)
{
    add_zork(SpeechType::kBuzz, w);
}

export template <typename T, typename ...Args>
void add_buzz(T first, Args... args)
{
    add_buzz(first);
    add_buzz(args...);
}

export bool apply_random(hackfn fcn, const HackP& demon)
{
    return fcn(demon);
}

// Demons
EHACKFN(robber);
EHACKFN(sword_glow);
EHACKFN(fighting);

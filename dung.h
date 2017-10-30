#pragma once
#include <map>
#include <vector>
#include <array>
#include <string>
#include "defs.h"
#include "room.h"
#include "funcs.h"

typedef std::map<std::string, WordP> WordsPobl;
typedef std::pair<std::string, WordP> WordsPoblPair;
typedef std::map<std::string, direction> DirectionsPobl;
typedef std::map<std::string, ActionP> ActionsPobl;

extern int64_t star_bits;
extern int64_t glohi;
extern WordsPobl words_pobl;
extern DirectionsPobl directions_pobl;
extern ActionsPobl actions_pobl;
extern ObjList small_papers;
extern ObjList palobjs;
extern RoomList random_list;
extern ObjList inqobjs;
extern ObjectP bunch_obj;
extern Iterator<std::string> indentstr;
extern RoomP mloc;
extern GObjectPtr it_object;
extern int deaths;
extern HackP robber_demon;
extern HackP sword_demon;
extern HackP fight_demon;
extern HackP clocker;
extern VerbP buncher;
extern std::vector<std::vector<int>> def1_res;
extern std::vector<std::vector<int>> def2_res;
extern std::vector<std::vector<int>> def3_res;
extern int cyclowrath;
extern std::vector<VerbP> robot_actions;
extern std::vector<VerbP> master_actions;
extern RoomP bloc;
extern RoomP startroom;
extern RoomP northend;
extern RoomP southend;
extern ObjList cobjs;
extern ObjList nobjs;
extern ObjList pobjs;
extern std::array<ObjList, 8> cells;
extern int played_time;

typedef std::pair<ObjectP, int> NumObjs;
extern std::vector<NumObjs> numobjs;
inline std::vector<NumObjs>::const_iterator memq(ObjectP o, const std::vector<NumObjs> &nm)
{
    for (auto iter = nm.begin(); iter != nm.end(); ++iter)
    {
        if (iter->first == o)
            return iter;
    }
    return nm.end();
}

// Direction vector for mirror
typedef std::pair<direction, int> DVPair;
typedef std::vector<DVPair> DirVec;
extern DirVec dirvec;
inline const DVPair *memq(direction d, const DirVec &dv)
{
    for (const DVPair &dvp : dv)
    {
        if (dvp.first == d)
            return &dvp;
    }
    return nullptr;
}

class hack
{
    typedef std::variant<CEventP, ObjectP> HobjsValue;
    enum { kho_event, kho_object };
public:
    hack(hackfn ha, const ObjList &ho, const std::list<RoomP> &hr, RoomP rm, ObjectP obj) :
        _haction(ha), _room(rm), _hobj(obj)
    {
        _hobjs_ob = ho;
        _hrooms = hr;
        _hflag = false;
    }

    hack(const hack &h)
    {
        copy(h);
    }

    hack operator=(const hack &h)
    {
        copy(h);
        return *this;
    }

    hackfn haction() const { return _haction; }
    void haction(hackfn fn) { _haction = fn; }

    bool hflag() const { return _hflag; }
    void hflag(bool flg) { _hflag = flg; }
    ObjectP hobj() { return _hobj; }
    RoomP hroom() { return _room; }
    void hroom(RoomP rm) { _room = rm; }
    const std::list<RoomP> &hrooms() const { return _hrooms; }
    std::list<RoomP> &hrooms() { return _hrooms; }

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

    void hobjs_add(CEventP ev)
    {
        _hobjs_ev.push_front(ev);
    }

    void hobjs_add(ObjectP ob)
    {
        _hobjs_ob.push_front(ob);
    }

private:
    hackfn _haction;
    EventList _hobjs_ev;
    ObjList _hobjs_ob;
    std::list<RoomP> _hrooms;
    RoomP _room;
    ObjectP _hobj;
    bool _hflag;

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

inline bool memq(CEventP ev, const EventList &hobjs)
{
    for (auto hobj : hobjs)
    {
        if (ev == hobj)
            return true;
    }
    return false;
}

typedef std::shared_ptr<hack> HackP;

// Puzzle room
struct CpExit
{
    direction dir;
    int offset;
};
typedef std::vector<CpExit> CpExitV;
extern CpExitV cpexits;
inline CpExitV::iterator memq(direction d, CpExitV &v)
{
    return std::find_if(v.begin(), v.end(), [d](const CpExit &e) { return e.dir == d; });
}

// Bank puzzle
extern RoomP scol_room;
extern RoomP scol_active;
struct ScolRooms
{
    direction dir;
    RoomP rm;
};
typedef std::vector<ScolRooms> ScolRoomsV;

struct ScolWalls
{
    RoomP rm1;
    ObjectP obj;
    RoomP rm2;
};
typedef std::vector<ScolWalls> ScolWallsV;

extern ScolRoomsV scol_rooms;
extern ScolWallsV scol_walls;
const ScolRooms &memq(direction dir, const ScolRoomsV &cont);

extern const ObjList villains;
extern ObjList oppv;
extern std::vector<int> villain_probs;

extern const ObjList weapons;
struct BestWeapons
{
    BestWeapons(ObjectP v, ObjectP w, int val) :
        villain(v), weapon(w), value(val) {}
    ObjectP villain;
    ObjectP weapon;
    int value;
};
typedef std::shared_ptr<BestWeapons> BestWeaponsP;
typedef std::vector<BestWeaponsP> BestWeaponsList;
extern const BestWeaponsList best_weapons;
BestWeaponsP memq(ObjectP v, const BestWeaponsList &bwl);

// Parse vector is defined in parser.mud. It is a 3-element vector,
// containing various items:
// 0: ActionP or VerbP
// 1: ObjectP or direction
// 2: ObjectP or nothing
typedef std::variant<std::monostate, ActionP, VerbP, ObjectP, PhraseP, direction> ParseVecVal;
enum { kpv_none, kpv_action, kpv_verb, kpv_object, kpv_phrase, kpv_direction };
typedef std::array<ParseVecVal, 3> ParseVecA;

inline ParseVecVal as_pvv(std::any a)
{
    if (a.type() == typeid(ActionP))
        return std::any_cast<ActionP>(a);
    else if (a.type() == typeid(VerbP))
        return std::any_cast<VerbP>(a);
    else if (a.type() == typeid(ObjectP))
        return std::any_cast<ObjectP>(a);
    else if (a.type() == typeid(PhraseP))
        return std::any_cast<PhraseP>(a);
    else if (a.type() == typeid(direction))
        return std::any_cast<direction>(a);
    _ASSERT(0);
    return ParseVecVal();
}

inline OrphanSlotType as_ost(ParseVecVal pv)
{
    OrphanSlotType ost;
    switch (pv.index())
    {
    case kpv_object:
        ost = std::get<kpv_object>(pv);
        break;
    case kpv_phrase:
        ost = std::get<kpv_phrase>(pv);
        break;
    case kpv_none:
        break;
    default:
        _ASSERT(0);
    }
    return ost;
}

inline direction as_dir(const ParseVecVal &a)
{
    _ASSERT(a.index() == kpv_direction);
    return std::get<kpv_direction>(a);
}

inline ObjectP as_obj(ParseVecVal pvv)
{
    _ASSERT(pvv.index() == kpv_object || pvv.index() == kpv_none);
    return (pvv.index() == kpv_object) ? std::get<kpv_object>(pvv) : ObjectP();
}

inline std::any as_any(ObjectP op)
{
    return op ? std::any(op) : std::any();
}

inline WordP as_word(const std::any &a)
{
    _ASSERT(a.type() == typeid(WordP));
    return std::any_cast<WordP>(a);
}

inline VerbP as_verb(const ParseVecVal &a)
{
    _ASSERT(a.index() == kpv_verb);
    return std::get<kpv_verb>(a);
}

inline std::string as_string(const std::any &a)
{
    _ASSERT(a.type() == typeid(std::string));
    return std::any_cast<std::string>(a);
}

void init_dung();

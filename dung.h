#pragma once
#include <map>
#include <vector>
#include <array>
#include <string>
#include "defs.h"
#include "makstr.h"
#include "room.h"
#include "funcs.h"
#include "melee.h"

typedef std::map<std::string, WordP> WordsPobl;
typedef std::pair<std::string, WordP> WordsPoblPair;
typedef std::map<std::string, direction> DirectionsPobl;
typedef std::map<std::string, ActionP> ActionsPobl;

extern WordsPobl words_pobl;
extern DirectionsPobl directions_pobl;
extern ActionsPobl actions_pobl;
extern const ObjList small_papers;
extern const ObjList palobjs;
extern const RoomList random_list;
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
extern const std::vector<std::vector<attack_state>> def1_res;
extern const std::vector<std::vector<attack_state>> def2_res;
extern const std::vector<std::vector<attack_state>> def3_res;
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

typedef std::pair<ObjectP, int> NumObjs;
extern std::array<NumObjs, 8> numobjs;

// Direction vector for mirror
typedef std::pair<direction, int> DVPair;
typedef std::vector<DVPair> DirVec;
extern const DirVec dirvec;

class hack
{
    typedef std::variant<CEventP, ObjectP> HobjsValue;
public:
    hack(hackfn ha, const ObjList &ho, const std::list<RoomP> &hr, RoomP rm, ObjectP obj) :
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
    std::list<RoomP> _hrooms;
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
typedef std::tuple<direction, int> CpExit;
typedef std::vector<CpExit> CpExitV;
extern const CpExitV cpexits;

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

extern const ObjList villains;
extern ObjList oppv;
extern std::vector<int> villain_probs;

extern const ObjList weapons;
class BestWeapons
{
public:
    BestWeapons(ObjectP v, ObjectP w, int va) :
        vill(v), weap(w), val(va) {}

    const ObjectP &villain() const { return vill; }
    const ObjectP &weapon() const { return weap; }
    int value() const { return val; }
private:
    ObjectP vill;
    ObjectP weap;
    int val;
};
typedef std::shared_ptr<BestWeapons> BestWeaponsP;
typedef std::vector<BestWeaponsP> BestWeaponsList;
extern const BestWeaponsList best_weapons;

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
    ParseVecVal pvv;
    if (auto ap = std::get_if<ActionP>(&pv))
        pvv = *ap;
    else if (auto vp = std::get_if<VerbP>(&pv))
        pvv = *vp;
    else if (auto op = std::get_if<ObjectP>(&pv))
        pvv = *op;
    else if (auto pp = std::get_if<PhraseP>(&pv))
        pvv = *pp;
    else if (auto dp = std::get_if<direction>(&pv))
        pvv = *dp;
    else
        error("Invalid parse vector value");
    return pvv;
}

inline OrphanSlotType as_ost(ParseVecVal pv)
{
    OrphanSlotType ost;
    ObjectP *op;
    PhraseP *pp;
    if (op = std::get_if<ObjectP>(&pv))
        ost = *op;
    else if (pp = std::get_if<PhraseP>(&pv))
        ost = *pp;
    return ost;
}

inline direction as_dir(const ParseVecVal &a)
{
    return std::get<direction>(a);
}

inline ObjectP as_obj(ParseVecVal pvv)
{
    ObjectP *op = std::get_if<ObjectP>(&pvv);
    return op ? *op : ObjectP();
}

inline WordP as_word(const ParseAval &a)
{
    return std::get<WordP>(a);
}

inline VerbP as_verb(const ParseVecVal &a)
{
    const VerbP *p = std::get_if<VerbP>(&a);
    return p ? *p : VerbP();
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

template <typename T>
void dsynonym(const char *dir, T syn)
{
    auto iter = directions_pobl.find(dir);
    if (iter == directions_pobl.end())
        error("Invalid direction synonym added");
    directions_pobl[syn] = iter->second;
}

template <typename T, typename ...Args>
void dsynonym(const char *dir, T first, Args... args)
{
    dsynonym(dir, first);
    dsynonym(dir, args...);
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
    if (wc != "LOWER")
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
    add_zork(kBuzz, w);
}

template <typename T, typename ...Args>
void add_buzz(T first, Args... args)
{
    add_buzz(first);
    add_buzz(args...);
}
#pragma once
#include "room.h"
#include <map>
#include <memory>

enum AdvBits
{
    astaggered,
    anumbits
};

class Adv
{
public:

    Adv(RoomP r, const std::initializer_list<ObjectP> &objs, int score, const ObjectP &vehicle,
        const ObjectP &obj, rapplic action, int strength);

    const ObjectP &aobj() const { return _aobj; }

    int astrength() const { return _astrength; }
    void astrength(int new_s) { _astrength = new_s; }

    int ascore() const { return _ascore; }
    void ascore(int new_score) { _ascore = new_score; }

    rapplic aaction() { return _aaction; }
    void aaction(rapplic new_action) { _aaction = new_action; }

    const RoomP &aroom() const { return _aroom; }
    void aroom(const RoomP &rp) { _aroom = rp; }

    const ObjectP &avehicle() const { return _avehicle; }
    void avehicle(const ObjectP &op) { _avehicle = op; }

    const ObjList &aobjs() const { return _aobjs; }
    ObjList &aobjs() { return _aobjs; }

    std::bitset<anumbits> &flags() { return bits; }
    const std::bitset<anumbits> &flags() const { return bits; }

    void restore(const Adv &a)
    {
        _aroom = a.aroom();
        _ascore = a.ascore();
        _avehicle = a.avehicle();
        _aobj = a.aobj();
        _astrength = a.astrength();
        bits = a.flags();
        _aobjs = a.aobjs();
    }

private:
    Adv() {}
    RoomP _aroom;                    // Where he is
    ObjList _aobjs;                  // What he's carrying
    int _ascore = 0;                 // Score
    ObjectP _avehicle;               // What he's riding in
    ObjectP _aobj;                   // What he is
    rapplic _aaction = nullptr;      // Special action for robot, etc.
    int _astrength = 0;              // Fighting strength
    std::bitset<anumbits> bits;
};

inline bool atrnn(const AdvP &adv, AdvBits b)
{
    return adv->flags()[b] != 0;
}

inline void atrz(const AdvP &adv, AdvBits b)
{
    adv->flags()[b] = 0;
}

inline void atro(const AdvP &adv, AdvBits b)
{
    adv->flags()[b] = 1;
}

void add_actor(e_oactor actor_name, RoomP room, const std::initializer_list<ObjectP> &objs,
    int score, ObjectP vehicle, ObjectP obj, rapplic action, int strength);
std::array<AdvP, oa_none> &actors();

inline const AdvP &player() { return actors()[oa_player]; }

// Actor functions
namespace actor_funcs
{
    bool master_actor();
    bool dead_function();
    bool robot_actor();
}

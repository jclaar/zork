#pragma once
#include <boost/serialization/split_member.hpp>
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
    friend class boost::serialization::access;
    template <class archive>
    void save(archive &ar, const unsigned int version) const
    {
        ar & (_aroom ? _aroom->rid() : std::string());
        ar & _ascore;
        ar & (_avehicle ? _avehicle->oid() : std::string());
        ar & _aobj->oid();
        ar & _astrength;
        ar & bits;
        std::list<std::string> obj_list;
        for (auto o : _aobjs)
        {
            obj_list.push_back(o->oid());
        }
        ar & obj_list;
    }

    template <class archive>
    void load(archive &ar, const unsigned int version)
    {
        std::string temp;
        ar & temp;
        if (!temp.empty())
            _aroom = sfind_room(temp);
        ar & _ascore;
        ar & temp;
        if (!temp.empty())
            _avehicle = sfind_obj(temp);
        ar & temp;
        _aobj = sfind_obj(temp);
        ar & _astrength;
        ar & bits;
        std::list<std::string> obj_list;
        ar & obj_list;
        for (auto s : obj_list)
        {
            _aobjs.push_back(sfind_obj(s));
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

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

void add_actor(e_oactor actor_name, const RoomP &room, const std::initializer_list<ObjectP> &objs,
    int score, const ObjectP &vehicle, const ObjectP &obj, rapplic action, int strength);
std::array<AdvP, oa_none> &actors();

inline const AdvP &player() { return actors()[oa_player]; }

// Actor functions
namespace actor_funcs
{
    bool master_actor();
    bool dead_function();
    bool robot_actor();
}

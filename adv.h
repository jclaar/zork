#pragma once
#include <boost/serialization/split_member.hpp>
#include "room.h"

enum class AdvBits
{
    astaggered,
    anumbits
};
typedef Flags<AdvBits, std::to_underlying(AdvBits::anumbits)> AdvBitset;

class Adv
{
public:

    Adv(RoomP r, const ObjectP &obj, rapplic action, int strength);

    const ObjectP &aobj() const { return _aobj; }

    PROP(astrength);
    PROP(ascore);
    PROP(aaction);
    PROP(aroom);
    PROP(avehicle);
    PROP(aobjs);
    PROP(flags);

    void restore(const Adv& a);

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
        ar & _flags;
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
        ar & _flags;
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
    AdvBitset _flags;
};

bool atrnn(const AdvP& adv, AdvBits b);
void atrz(const AdvP& adv, AdvBits b);
void atro(const AdvP& adv, AdvBits b);

void add_actor(e_oactor actor_name, const RoomP &room, 
    const ObjectP &obj, rapplic action, int strength);
AdvArray &actors();

const AdvP& player();

// Actor functions
namespace actor_funcs
{
    RAPPLIC(master_actor);
    RAPPLIC(dead_function);
    RAPPLIC(robot_actor);
}

#include "stdafx.h"
#include "adv.h"

namespace
{
    std::map<e_oactor, AdvP> actor_list;
}

std::map<e_oactor, AdvP> &actors()
{
    return actor_list;
}

Adv::Adv(RoomP r, const std::initializer_list<ObjectP> &objs, int score, ObjectP vehicle,
    ObjectP actor_obj, rapplic action, int strength) :
    _aroom(r),
    _aobjs(objs),
    _ascore(score),
    _avehicle(vehicle),
    _aobj(actor_obj),
    _aaction(action),
    _astrength(strength)
{

}

AdvP add_actor(e_oactor actor_name, RoomP room, const std::initializer_list<ObjectP> &objs,
    int score, ObjectP vehicle, ObjectP obj, rapplic action, int strength)
{
    AdvP ap = std::make_shared<Adv>(room, objs, score, vehicle, obj, action, strength);
    actor_list[actor_name] = ap;
    return ap;
}

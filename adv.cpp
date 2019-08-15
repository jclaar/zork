#include "stdafx.h"
#include "adv.h"

namespace
{
    std::array<AdvP, oa_none> actor_list;
}

std::array<AdvP, oa_none> &actors()
{
    return actor_list;
}

Adv::Adv(RoomP r, const std::initializer_list<ObjectP> &objs, int score, const ObjectP &vehicle,
    const ObjectP &actor_obj, rapplic action, int strength) :
    _aroom(r),
    _aobjs(objs),
    _ascore(score),
    _avehicle(vehicle),
    _aobj(actor_obj),
    _aaction(action),
    _astrength(strength)
{

}

void add_actor(e_oactor actor_name, const RoomP &room, const std::initializer_list<ObjectP> &objs,
    int score, const ObjectP &vehicle, const ObjectP &obj, rapplic action, int strength)
{
    actor_list[actor_name] = std::make_unique<Adv>(room, objs, score, vehicle, obj, action, strength);
}

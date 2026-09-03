module;

#include "adv.h"

export module ZAdv;
namespace
{
    AdvArray actor_list;
}

export AdvArray &actors()
{
    return actor_list;
}

Adv::Adv(RoomP r, const ObjectP &actor_obj, rapplic action, int strength) :
    _aroom(r),
    _aobj(actor_obj),
    _aaction(action),
    _astrength(strength)
{

}

void Adv::restore(const Adv& a)
{
    _aroom = a.aroom();
    _ascore = a.ascore();
    _avehicle = a.avehicle();
    _aobj = a.aobj();
    _astrength = a.astrength();
    _flags = a.flags();
    _aobjs = a.aobjs();
}

export void add_actor(e_oactor actor_name, const RoomP &room,
    const ObjectP &obj, rapplic action, int strength)
{
    actor_list[std::to_underlying(actor_name)] = std::make_unique<Adv>(room, obj, action, strength);
}

export const AdvP& player()
{ 
    return actors()[std::to_underlying(e_oactor::player)]; 
}

export bool atrnn(const AdvP& adv, AdvBits b)
{
    return adv->flags()[b] != 0;
}

export void atrz(const AdvP& adv, AdvBits b)
{
    adv->flags()[b] = 0;
}

export void atro(const AdvP& adv, AdvBits b)
{
    adv->flags()[b] = 1;
}

#include "precomp.h"
#include "adv.h"

namespace
{
    AdvArray actor_list;
}

AdvArray &actors()
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

void add_actor(e_oactor actor_name, const RoomP &room,
    const ObjectP &obj, rapplic action, int strength)
{
    actor_list[static_cast<size_t>(actor_name)] = std::make_unique<Adv>(room, obj, action, strength);
}

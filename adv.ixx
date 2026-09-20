module;
#include <boost/serialization/split_member.hpp>
#include "defs.h"

export module Zork:Adv;
import :fwd;
import ZDefs;
import ZFlagSupport;
import std;

export enum class AdvBits
{
    astaggered,
    anumbits
};
export using AdvBitset = Flags<AdvBits, std::to_underlying(AdvBits::anumbits)>;

export class Adv
{
public:

    Adv(RoomP r, const ObjectP& obj, rapplic action, int strength);

    const ObjectP& aobj() const { return _aobj; }

    PROP(astrength);

    int ascore() const { return _ascore; }
    void ascore(int new_score) { _ascore = new_score; }

    const rapplic& aaction() const { return _aaction; }
    void aaction(rapplic new_action) { _aaction = new_action; }

    const RoomP& aroom() const { return _aroom; }
    void aroom(const RoomP& rp) { _aroom = rp; }

    const ObjectP& avehicle() const { return _avehicle; }
    void avehicle(const ObjectP& op) { _avehicle = op; }

    const ObjList& aobjs() const { return _aobjs; }
    ObjList& aobjs() { return _aobjs; }

    AdvBitset& flags() { return bits; }
    const AdvBitset& flags() const { return bits; }

    void restore(const Adv& a)
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
    void save(archive& ar, const unsigned int version) const;

    template <class archive>
    void load(archive& ar, const unsigned int version);


    BOOST_SERIALIZATION_SPLIT_MEMBER();

    RoomP _aroom;                    // Where he is
    ObjList _aobjs;                  // What he's carrying
    int _ascore = 0;                 // Score
    ObjectP _avehicle;               // What he's riding in
    ObjectP _aobj;                   // What he is
    rapplic _aaction = nullptr;      // Special action for robot, etc.
    int _astrength = 0;              // Fighting strength
    AdvBitset bits;
};
export using AdvArray = std::array <AdvP, std::to_underlying(e_oactor::none)>;

export bool atrnn(const AdvP& adv, AdvBits b)
{
    return adv->flags()[b] != 0;
}

export void atrz(const AdvP& adv, AdvBits b)
{
    adv->flags()[b] = 0;
}

export inline void atro(const AdvP& adv, AdvBits b)
{
    adv->flags()[b] = 1;
}

export void add_actor(e_oactor actor_name, const RoomP& room,
    const ObjectP& obj, rapplic action, int strength);
export template <typename Fn>
void add_actor(e_oactor actor_name, const RoomP& room,
	const ObjectP& obj, Fn action, int strength)
{
	add_actor(actor_name, room, obj, rapplic(action), strength);
}

export AdvArray& actors();

export const AdvP& player() { return actors()[std::to_underlying(e_oactor::player)]; }

// Actor functions
namespace actor_funcs
{
    RAPPLIC(master_actor);
    RAPPLIC(dead_function);
    RAPPLIC(robot_actor);
}

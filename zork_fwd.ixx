export module Zork:fwd;
import std;

export enum class ObjectSlots
{
    ksl_odesco,
    ksl_odesc1,
    ksl_osize,
    ksl_ofval,
    ksl_otval,
    ksl_ocapac,
    ksl_oread,
    ksl_oglobal,
    ksl_oactor,
    ksl_ovtype,
    ksl_ostrength,
    ksl_ofmsgs,
    ksl_olint,
    ksl_omatch,
    ksl_obverb,
    ksl_rglobal,
    ksl_rval
};

class Adv;
using AdvP = std::shared_ptr<Adv>;
class Object;
using ObjectP = std::shared_ptr<Object>;
class Room;
using RoomP = std::shared_ptr<Room>;



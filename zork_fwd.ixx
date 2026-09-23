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

export class Adv;
export using AdvP = std::shared_ptr<Adv>;

export class Object;
export using ObjectP = std::shared_ptr<Object>;
export using ObjList = std::list<ObjectP>;
export using ObjVector = std::vector<ObjectP>;

export class Room;
export using RoomP = std::shared_ptr<Room>;
export using RoomList = std::list<RoomP>;

export class hack;
export using HackP = std::shared_ptr<hack>;
export using hackfn = std::function<bool(const HackP&)>;

export class GObject;
export using GObjectPtr = std::shared_ptr<GObject>;

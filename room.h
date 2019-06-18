#pragma once

//#include <boost/serialization/bitset.hpp>
#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <variant>
#include <tuple>
#include <any>
#include "defs.h"
#include "globals.h"
#include "object.h"
#include "roomfns.h"

typedef std::variant<int, std::vector<Bits>> RPValue;
typedef std::tuple<ObjectSlots, RPValue> RP;
inline RP rg(const std::initializer_list<Bits> &rb)
{
    std::vector<Bits> bits(rb);
    return RP(ksl_rglobal, bits);
}

// Special message on no exit
class NExit
{
public:
	explicit NExit(const char *desc) : nexit_desc(desc) {}

    const std::string &desc() const { return nexit_desc; }
private:
    std::string nexit_desc;
};

// Special exit conditions
class CExit
{
public:
    typedef std::variant<FlagId, rapplic> FlagVar;

    CExit(FlagVar flag_name, const std::string &rmid, const std::string &desc = "", bool flag = false, ex_rapplic fn = nullptr) :
        _flid(flag_name),
        _rmid(rmid),
        _desc(desc),
        _fn(fn)
    {
        flag; 
    }
    CExit(FlagVar flag_name, const std::string &rmid, const std::string &(*desc)(), bool flag = false, ex_rapplic fn = nullptr) :
        _flid(flag_name),
        _rmid(rmid),
        _desc(desc()),
        _fn(fn)
    {
        flag;
    }

    ex_rapplic cxaction() const { return _fn; }
    const RoomP &cxroom() const;
    const std::string &cxstr() const { return _desc; }

    bool cxflag() const {
        if (auto fid = std::get_if<FlagId>(&_flid))
        {
            return flags[*fid];
        }
        return false;
    }
private:
    FlagVar _flid;
    std::string _rmid;
    std::string _desc;
    ex_rapplic _fn;
};

typedef std::shared_ptr<CExit> CExitPtr;

class DoorExit
{
public:
    DoorExit(const std::string &oid, const std::string &rm1, const std::string &rm2, const std::string &str = std::string(), ex_rapplic fn = nullptr) :
        _oid(oid),
        _rm1(rm1),
        _rm2(rm2),
        _str(str),
        _fn(fn)
    {

    }

    ex_rapplic daction() const {
        return _fn;
    }
    const ObjectP &dobj() const;
    const RoomP &droom1() const;
    const RoomP &droom2() const;
    const std::string &dstr() const { return _str; }
    void dstr(const std::string &s) { _str = s; }

private:
    std::string _oid;
    std::string _rm1;
    std::string _rm2;
    std::string _str;
    ex_rapplic _fn;
};
typedef std::shared_ptr<DoorExit> DoorExitPtr;

class SetgExit
{
public:
    SetgExit(const std::string &name, const CExitPtr &cep) : sname(name), ce(cep) {}

    const std::string &name() const { return sname; }
    CExitPtr cexit() { return ce; }
private:
    std::string sname;
    CExitPtr ce;
};
typedef std::shared_ptr<SetgExit> SetgExitP;

typedef std::variant<std::monostate, NExit, CExitPtr, DoorExitPtr, SetgExitP, std::string, RoomP> ExitType;
typedef std::tuple<direction, ExitType> Ex;

class Room
{
public:
	Room(const std::string &rid, const std::string &, const std::string &d2,
        const std::initializer_list<Ex> &exits,
        const std::initializer_list<const char*> &contents,
        rapplic roomf,
        const std::initializer_list<Bits> &rb,
        const std::initializer_list<RP> &room_slots);

    const std::string &rid() const { return _id; }
    const std::string &rdesc1() const { return _desc1; }
    std::string &rdesc1() { return _desc1; }
    const std::string &rdesc2() const { return _desc2; }
    ObjList &robjs() { return _contents; }
    const ObjList &robjs() const { return _contents; }
    const std::vector<Ex> &rexits() const { return _exits; }
    const std::vector<Bits> &rglobal() const { return _rglobal; }
    void rglobal(Bits new_global) { _rglobal.push_back(new_global); }
    std::bitset<numbits> &rbits() { return _room_bits; }
    rapplic raction() const { return _room_fn; }
    int rval() const;
    void rval(int new_val);

    void restore(const Room &src)
    {
        _room_bits = src._room_bits;
        _rval = src._rval;
        robjs() = src.robjs();
        // Also have to save the room description, since mung_room
        // might change it.
        _desc1 = src.rdesc1();
    }

private:
    Room() {}

    std::string _id;
    std::string _desc1;
    std::string _desc2;
    int _rval = 0;
    std::bitset<numbits> _room_bits;
    std::vector<Bits> _rglobal;
    std::vector<Ex> _exits;
    ObjList _contents;
    rapplic _room_fn = nullptr;
};

void init_rooms();
const RoomP &get_room(const char *rid, RoomP init_val = RoomP());
const RoomP &get_room(const std::string &rid, RoomP init_val = RoomP());
const RoomP &find_room(const std::string &rid);
const RoomP &sfind_room(const std::string &s);
const RoomP &sfind_room(const char *s);
std::list<RoomP> &rooms();
std::map<std::string, RoomP> &room_map();

inline std::list<RoomP>::iterator rest(std::list<RoomP>::iterator i, int count = 1)
{
    std::advance(i, count);
    return i;
}

// Set or 0 room bit
template <Bits b>
bool rtro(const RoomP &p)
{
	p->rbits().set(b);
	return true;
}
template <Bits b>
bool rtrz(const RoomP &p)
{
	p->rbits().reset(b);
	return true;
}

#pragma once

#include <boost/serialization/bitset.hpp>
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

    CExit(FlagVar flag_name, std::string_view rmid, std::string_view desc = "", bool flag = false, ex_rapplic fn = nullptr) :
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
    DoorExit(std::string_view oid, std::string_view rm1, std::string_view rm2, std::string_view str = std::string_view(), ex_rapplic fn = nullptr) :
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
    void dstr(std::string_view s) { _str = s; }

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
    SetgExit(std::string_view name, const CExitPtr &cep) : sname(name), ce(cep) {}

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
	Room(std::string_view rid, std::string_view d1, std::string_view d2,
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
    friend class boost::serialization::access;
    Room() {}
    template <class archive>
    void save(archive &ar, const unsigned int version) const
    {
        ar & _room_bits;
        ar & _rval;
        std::list<std::string> rob;
        std::transform(robjs().begin(), robjs().end(), std::back_inserter(rob), [](ObjectP o)
        {
            return o->oid();
        });
        ar & rob;
        ar & _desc1;
    }

    template <class archive>
    void load(archive &ar, const unsigned int version)
    {
        ar & _room_bits;
        ar & _rval;
        std::list<std::string> rob;
        ar & rob;
        ar & _desc1;
        robjs().clear();
        std::transform(rob.begin(), rob.end(), std::back_inserter(robjs()), [](const std::string &oid)
        {
            return sfind_obj(oid);
        });
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

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
const RoomP &get_room(std::string_view rid, RoomP init_val = RoomP());
const RoomP &find_room(std::string_view rid);
inline const RoomP &sfind_room(std::string_view s) { return find_room(s); }
RoomList &rooms();
typedef std::map<std::string, RoomP, std::less<>> RoomMap;
RoomMap &room_map();

inline RoomList::iterator rest(RoomList::iterator i, int count = 1)
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

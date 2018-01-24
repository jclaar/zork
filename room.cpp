#include "stdafx.h"
#include "room.h"
#include "zstring.h"
#include "globals.h"
#include "object.h"
#include "funcs.h"
#include "objfns.h"
#include "ZorkException.h"

// These are all exit definitions in the original MDL code.
#define notree_FORE1 NExit("There is no tree here suitable for climbing")
#define notree_FORE2 notree_FORE1
#define notree_FORE4 notree_FORE1
#define notree_FORE5 notree_FORE1
#define kitchen_window_EHOUS DoorExitPtr(new DoorExit("WINDO", "KITCH", "EHOUS"))
#define kitchen_window_KITCH kitchen_window_EHOUS
#define magcmach_MAGNE CExitPtr(new CExit(frobozz, "cmach", "", false, exit_funcs::magnet_room_exit))
#define magalice_MAGNE CExitPtr(new CExit(frobozz, "cmach", "", false, exit_funcs::magnet_room_exit))
#define current_RIVR1 NExit("You cannot go upstream due to strong currents.")
#define current_RIVR2 current_RIVR1
#define current_RIVR3 current_RIVR1
#define current_RIVR4 current_RIVR1
#define current_RIVR5 current_RIVR1
#define cliffs_RIVR1 NExit("The White Cliffs prevent your landing here.")
#define cliffs_RIVR2 cliffs_RIVR1
#define crain_POG CExitPtr(new CExit(rainbow, "RAINB"))
#define cxgnome_LEDG2 CExitPtr(new CExit(gnome_door, "VLBOT"))
#define cxgnome_LEDG4 cxgnome_LEDG2
#define palandoor_PALAN DoorExitPtr(new DoorExit("PDOOR", "PALAN", "PRM"))
#define palandoor_PRM palandoor_PALAN
#define palanwind_PALAN DoorExitPtr(new DoorExit("PWIND", "PALAN", "PRM"))
#define palanwind_PRM palanwind_PALAN
#define mg_r CExitPtr(new CExit(frobozz, "MRG", "", false, exit_funcs::mrgo))
#define mr_g_MRD CExitPtr(new CExit(frobozz, "MRG", "", false, exit_funcs::mrgo))
#define mr_g_MRC mr_g_MRD
#define mr_d_MRG CExitPtr(new CExit(frobozz, "MRD", "", false, exit_funcs::mrgo))
#define cd_TOMB DoorExitPtr(new DoorExit("TOMB", "TOMB", "CRYPT"))
#define cd_CRYPT cd_TOMB
#define cd_NCORR DoorExitPtr(new DoorExit("CDOOR", "NCORR", "CELL"))
#define cd_CELL cd_NCORR
#define smdrop_ALISM NExit("There is a chasm too large to jump across.")
#define fout_PCELL NExit("The door is securely fastened.")
#define fout_NCELL fout_PCELL
#define mirex_MRAW CExitPtr(new CExit(mirror_open, "INMIR", "", false, exit_funcs::mirin))
#define mirex_MRBW mirex_MRAW
#define mirex_MRAE mirex_MRAW
#define mirex_MRBE mirex_MRAW
#define mirex_MRCE mirex_MRAW
#define mirex_MRCW mirex_MRAW
#define mirex_MRA  mirex_MRAW
#define mirex_MRB  mirex_MRAW
#define mirex_MRC  mirex_MRAW
#define mout_INMIR CExitPtr(new CExit(frobozz, "MRA", "", false, exit_funcs::mirout))
#define mr_a_MREYE CExitPtr(new CExit(frobozz, "MRA", "", false, exit_funcs::mrgo))
#define mr_a_MPB mr_a_MREYE
#define mr_a_MRB mr_a_MREYE
#define mr_b_MRA CExitPtr(new CExit(frobozz, "MRB", "", false, exit_funcs::mrgo))
#define mr_b_MRC mr_b_MRA
#define mr_c_MRG CExitPtr(new CExit(frobozz, "MRC", "", false, exit_funcs::mrgo))
#define mr_c_MRB mr_c_MRG
#define mr_d_FDOOR CExitPtr(new CExit(frobozz, "MRD", "", false, exit_funcs::mrgo))
#define od_SCORR DoorExitPtr(new DoorExit("ODOOR", "SCORR", "CELL", "", exit_funcs::maybe_door))
#define od_CELL od_SCORR
#define wd_BDOOR DoorExitPtr(new DoorExit("QDOOR", "BDOOR", "FDOOR"))
#define wd_FDOOR wd_BDOOR
#define nd_NCELL DoorExitPtr(new DoorExit("ODOOR", "NCELL", "NIRVA"))

std::list<RoomP> &rooms()
{
    static std::list<RoomP> rps;
    return rps;
}

std::map<std::string, RoomP> &room_map()
{
    static std::map<std::string, RoomP> rm;
    return rm;
}

namespace
{
    RoomP mr(const std::string &id, const std::string &d1, const std::string &d2, const std::initializer_list<Ex> &exits,
        const std::initializer_list<ObjectP> &contents = {}, rapplic roomf = nullptr,
        const std::initializer_list<Bits> &rb = { rlandbit },
        const std::initializer_list<RP> &props = {})
    {
        RoomP rp = RoomP(new Room(id, d1, d2, exits, contents, roomf, rb, props));
        return rp;
    }

    std::tuple<RoomP*, RoomP*> get_rooms()
    {
        static RoomP rooms[] =
        {
#include "roomdefs.h"
        };
        return std::tuple<RoomP*, RoomP*>(rooms, rooms + ARRSIZE(rooms));
    }
}

RoomP CExit::cxroom() const
{
    return sfind_room(_rmid.c_str());
}

ObjectP DoorExit::dobj() const
{
    return find_obj(_oid.c_str());
}

RoomP DoorExit::droom1() const
{
    return find_room(_rm1);
}

RoomP DoorExit::droom2() const
{
    return find_room(_rm2);
}


Room::Room(const std::string &rid, const std::string &d1, const std::string &d2, const std::initializer_list<Ex> &exits,
    const std::initializer_list<ObjectP> &cntnts, rapplic roomf, const std::initializer_list<Bits> &rb,
    const std::initializer_list<RP> &props) :
 _id(rid),
 _desc1(d1),
 _desc2(d2),
 _contents(cntnts),
 _room_fn(roomf),
 _exits(exits)
{
    for (auto b : rb)
    {
        _room_bits[b] = 1;
    }

    for (auto p : props)
    {
        switch (std::get<0>(p))
        {
        case ksl_rglobal:
        {
            if (std::get<1>(p).type() == typeid(std::vector<std::string>))
            {
                const std::vector<std::string> &rg = std::any_cast<std::vector<std::string>>(std::get<1>(p));
                _rglobal = rg;
            }
            else
                throw ZorkException("Bad rglobal flags.");
            break;
        }
        case ksl_rval:
        {
            int val = std::any_cast<int>(std::get<1>(p));
            _rval = val;
            if (_room_bits[rendgame])
            {
                eg_score_max += val;
            }
            else
            {
                inc_score_max(val);
            }
            break;
        }
        }
    }
}

int Room::rval() const
{
    return _rval;
}

void Room::rval(int new_val)
{
    _rval = new_val;
}

RoomP get_room(const char *cid, RoomP init_val)
{
    RoomP p;
    std::string sid = cid;
    auto iter = room_map().find(sid);
    if (iter == room_map().end())
    {
        // Add an empty room to the list, or add the init_val.
        if (!init_val)
        {
            init_val = mr(cid, "", "", {});
        }
        iter = room_map().insert(std::pair<std::string, RoomP>(sid, init_val)).first;
        rooms().push_front(init_val);
    }
    p = iter->second;
    return room_map()[cid];
}

void init_rooms()
{
    auto rooms = get_rooms();
    while (std::get<0>(rooms) != std::get<1>(rooms))
    {
        RoomP cur_room = *std::get<0>(rooms);
        RoomP p = get_room(cur_room->rid().c_str(), cur_room);
        if (p.get() != cur_room.get())
        {
            *(p.get()) = *(cur_room.get());

        }
        // Make sure all objects have their room pointers set.
        for (ObjectP op : p->robjs())
        {
            op->oroom(p);
        }
        std::get<0>(rooms)++;
    }
}

RoomP find_room(const std::string &rid)
{
    return room_map()[rid];
}

RoomP sfind_room(const std::string &rid)
{
    return find_room(rid);
}


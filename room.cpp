#include "stdafx.h"
#include "room.h"
#include "zstring.h"
#include "globals.h"
#include "object.h"
#include "funcs.h"
#include "objfns.h"
#include "ZorkException.h"
#include "act3.h"

// These are all exit definitions in the original MDL code.
#define notree_FORE1 NExit("There is no tree here suitable for climbing.")
#define notree_FORE2 notree_FORE1
#define notree_FORE4 notree_FORE1
#define notree_FORE5 notree_FORE1
#define kitchen_window_EHOUS std::make_shared<DoorExit>("WINDO", "KITCH", "EHOUS")
#define kitchen_window_KITCH kitchen_window_EHOUS
#define magcmach_MAGNE std::make_shared<CExit>(frobozz, "cmach", "", false, exit_funcs::magnet_room_exit)
#define magalice_MAGNE std::make_shared<CExit>(frobozz, "cmach", "", false, exit_funcs::magnet_room_exit)
#define current_RIVR1 NExit("You cannot go upstream due to strong currents.")
#define current_RIVR2 current_RIVR1
#define current_RIVR3 current_RIVR1
#define current_RIVR4 current_RIVR1
#define current_RIVR5 current_RIVR1
#define cliffs_RIVR1 NExit("The White Cliffs prevent your landing here.")
#define cliffs_RIVR2 cliffs_RIVR1
#define crain_POG std::make_shared<CExit>(rainbow, "RAINB")
#define cxgnome_LEDG2 std::make_shared<CExit>(gnome_door, "VLBOT")
#define cxgnome_LEDG4 cxgnome_LEDG2
#define palandoor_PALAN std::make_shared<DoorExit>("PDOOR", "PALAN", "PRM")
#define palandoor_PRM palandoor_PALAN
#define palanwind_PALAN std::make_shared<DoorExit>("PWIND", "PALAN", "PRM")
#define palanwind_PRM palanwind_PALAN
#define mg_r std::make_shared<CExit>(frobozz, "MRG", "", false, exit_funcs::mrgo)
#define mr_g_MRD std::make_shared<CExit>(frobozz, "MRG", "", false, exit_funcs::mrgo)
#define mr_g_MRC mr_g_MRD
#define mr_d_MRG std::make_shared<CExit>(frobozz, "MRD", "", false, exit_funcs::mrgo)
#define cd_TOMB std::make_shared<DoorExit>("TOMB", "TOMB", "CRYPT")
#define cd_CRYPT cd_TOMB
#define cd_NCORR std::make_shared<DoorExit>("CDOOR", "NCORR", "CELL")
#define cd_CELL cd_NCORR
#define smdrop_ALISM NExit("There is a chasm too large to jump across.")
#define fout_PCELL NExit("The door is securely fastened.")
#define fout_NCELL fout_PCELL
#define mirex_MRAW std::make_shared<CExit>(mirror_open, "INMIR", "", false, exit_funcs::mirin)
#define mirex_MRBW mirex_MRAW
#define mirex_MRAE mirex_MRAW
#define mirex_MRBE mirex_MRAW
#define mirex_MRCE mirex_MRAW
#define mirex_MRCW mirex_MRAW
#define mirex_MRA  mirex_MRAW
#define mirex_MRB  mirex_MRAW
#define mirex_MRC  mirex_MRAW
#define mout_INMIR std::make_shared<CExit>(frobozz, "MRA", "", false, exit_funcs::mirout)
#define mr_a_MREYE std::make_shared<CExit>(frobozz, "MRA", "", false, exit_funcs::mrgo)
#define mr_a_MPB mr_a_MREYE
#define mr_a_MRB mr_a_MREYE
#define mr_b_MRA std::make_shared<CExit>(frobozz, "MRB", "", false, exit_funcs::mrgo)
#define mr_b_MRC mr_b_MRA
#define mr_c_MRG std::make_shared<CExit>(frobozz, "MRC", "", false, exit_funcs::mrgo)
#define mr_c_MRB mr_c_MRG
#define mr_d_FDOOR std::make_shared<CExit>(frobozz, "MRD", "", false, exit_funcs::mrgo)
#define od_SCORR std::make_shared<DoorExit>("ODOOR", "SCORR", "CELL", "", exit_funcs::maybe_door)
#define od_CELL od_SCORR
#define wd_BDOOR std::make_shared<DoorExit>("QDOOR", "BDOOR", "FDOOR")
#define wd_FDOOR wd_BDOOR
#define nd_NCELL std::make_shared<DoorExit>("ODOOR", "NCELL", "NIRVA")

RoomList &rooms()
{
    static RoomList rps;
    return rps;
}

RoomMap &room_map()
{
    static RoomMap rm;
    return rm;
}

namespace
{
    RoomP mr(std::string_view id, std::string_view d1, std::string_view d2, const std::initializer_list<Ex> &exits,
        const std::initializer_list<const char*> &contents = {}, rapplic roomf = nullptr,
        const std::initializer_list<Bits> &rb = { rlandbit },
        const std::initializer_list<RP> &props = {})
    {
        return std::make_shared<Room>(id, d1, d2, exits, contents, roomf, rb, props);
    }

	typedef std::vector<RoomP> RoomVector;

#if _MSC_FULL_VER==192227812
#pragma optimize("", off)
#endif
    const RoomVector &get_rooms()
    {
        static const RoomVector rooms =
        {
#include "roomdefs.h"
        };
		return rooms;
    }
#if _MSC_FULL_VER==192227812
#pragma optimize("", on)
#endif
}

const RoomP &CExit::cxroom() const
{
    return sfind_room(_rmid);
}

const ObjectP &DoorExit::dobj() const
{
    return find_obj(_oid);
}

const RoomP &DoorExit::droom1() const
{
    return find_room(_rm1);
}

const RoomP &DoorExit::droom2() const
{
    return find_room(_rm2);
}


Room::Room(std::string_view rid, std::string_view d1, std::string_view d2, const std::initializer_list<Ex> &exits,
    const std::initializer_list<const char*> &cntnts, rapplic roomf, const std::initializer_list<Bits> &rb,
    const std::initializer_list<RP> &props) :
 _id(rid),
 _desc1(d1),
 _desc2(d2),
 _room_fn(roomf),
 _exits(exits)
{
	std::transform(cntnts.begin(), cntnts.end(), std::back_inserter(_contents),
		[](const char *n)
	{
		return get_obj(n);
	});
    for (auto b : rb)
    {
        _room_bits[b] = 1;
    }

    for (auto &p : props)
    {
        switch (std::get<0>(p))
        {
        case ksl_rglobal:
        {
            auto &prop = std::get<1>(p);
            // Must be a vector of bits.
            const std::vector<Bits> &pvb = std::get<std::vector<Bits>>(prop);
            _rglobal = pvb;
            break;
        }
        case ksl_rval:
        {
            int val = std::get<int>(std::get<1>(p));
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
        default:
            error("Bad rglobal flags");
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

const RoomP &get_room(std::string_view sid, RoomP init_val)
{
    auto iter = room_map().find(sid);
    if (iter == room_map().end())
    {
        // Add an empty room to the list, or add the init_val.
        if (!init_val)
        {
            init_val = mr(sid, "", "", {});
        }
        iter = room_map().insert(std::pair(std::string(sid), init_val)).first;
        rooms().push_front(init_val);
    }
    return iter->second;
}

void init_rooms()
{
    auto &rooms = get_rooms();
	auto cur_iter = rooms.begin();
	while (cur_iter != rooms.end())
    {
        RoomP cur_room = *cur_iter++;
        RoomP p = get_room(cur_room->rid(), cur_room);
        if (p.get() != cur_room.get())
        {
            *(p.get()) = *(cur_room.get());

        }
        // Make sure all objects have their room pointers set.
        for (const ObjectP &op : p->robjs())
        {
            op->oroom(p);
        }
    }
}

const RoomP &find_room(std::string_view rid)
{
    return room_map().find(rid)->second;
}


#include "stdafx.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <algorithm>
#include "sr.h"
#include "act1.h"
#include "act2.h"
#include "act3.h"
#include <fstream>
#include <vector>
#include "object.h"
#include "cevent.h"
#include "dung.h"
#include "room.h"
#include "rooms.h"
#include "adv.h"
#include "objser.h"

typedef std::vector<int> SV;
typedef SV::iterator SVI;

namespace
{
    const int save_version = 5;
    RoomP room_from_str(const std::string &s)
    {
        return (s.empty() ? RoomP() : sfind_room(s));
    }

    ObjectP obj_from_str(const std::string &s)
    {
        return (s.empty() ? ObjectP() : sfind_obj(s));
    }
}

template <class archive>
void dump_objects(archive &oa)
{
    oa & object_pobl();
}

template <class archive>
void restore_objects(archive &oa)
{
    ObjectPobl rest_objs;
    oa & rest_objs;
    const ObjectPobl &old_objs = object_pobl();
    for (auto rest : rest_objs)
    {
        auto old = old_objs.find(rest.first);
        _ASSERT(old != old_objs.end());
        _ASSERT(rest.second.size() == old->second.size());
        auto src_iter = rest.second.cbegin();
        auto dst_iter = old->second.begin();
        while (src_iter != rest.second.cend())
        {
            (*dst_iter)->restore(*(src_iter->get()));
            ++src_iter;
            ++dst_iter;
        }
    }
}

template <class archive>
void dump_rooms(archive &oa)
{
    oa & room_map();
}

template <class archive>
void restore_rooms(archive &ia)
{
    RoomMap rm_map;
    ia & rm_map;
    _ASSERT(rm_map.size() == room_map().size());

    for (auto rm : rm_map)
    {
        auto dest = room_map().find(rm.first);
        _ASSERT(dest != room_map().end());
        dest->second->restore(*rm.second.get());
    }
}

template <class archive>
void dump_robber(archive &ar)
{
    HackP robber = robber_demon;
    std::list<std::string> rob_objs;
    std::transform(robber->hobjs_ob().begin(), robber->hobjs_ob().end(),
        std::back_inserter(rob_objs), [](ObjectP o) { return o->oid(); });
    ar & rob_objs;

    // Save the head of the first room he's heading to.
    ar & robber->hrooms().front()->rid();
    ar & robber->hroom()->rid();
    bool temp = robber->hflag();
    ar & temp;
    temp = (robber->haction() ? true : false);
    ar & temp;
}

template <class archive>
void restore_robber(archive &ar)
{
    HackP robber = robber_demon;
    std::list<std::string> rob_objs;
    ar & rob_objs;
    ObjList robber_items;
    std::transform(rob_objs.begin(), rob_objs.end(), std::back_inserter(robber_items),
        [](const std::string &s) { return sfind_obj(s); });
    robber->hobjs(robber_items);

    std::string temp;
    ar & temp;
    RoomP rm = room_from_str(temp);
    RoomList hack_list = rooms();
    auto cur_room = std::find_if(hack_list.begin(), hack_list.end(), [rm](RoomP cur) {return cur->rid() == rm->rid(); });
    _ASSERT(cur_room != hack_list.end());
    hack_list.erase(hack_list.begin(), cur_room);
    robber->hrooms() = hack_list;

    ar & temp;
    robber->hroom(room_from_str(temp));
    bool btemp;
    ar & btemp;
    robber->hflag(btemp);
    ar & btemp;
    robber->haction(btemp ? ::robber : nullptr);
}

template <class archive>
void dump_clockers(archive &oa)
{
    oa & clocker->hobjs_ev();
}

template <class archive>
void restore_clockers(archive &ia)
{
    EventList el;
    ia & el;
    // Iterate through the whole event list, and create a new
    // one with the actual events in it.
    EventList restored_list;
    for (auto e : el)
    {
        auto iter = std::find_if(ev.begin(), ev.end(), [e](CEventP ev)
        {
            return e->cid() == ev->cid();
        });
        // The event better exist...
        _ASSERT(iter != ev.end());
        (*iter)->restore(*e.get());
        restored_list.push_front(*iter);
    }
    clocker->hobjs(restored_list);
}

template <class archive>
void dump_winners(archive &oa)
{
    oa & actors();
}

template <class archive>
void restore_winners(archive &ia)
{
    std::array<AdvP, oa_none> rest_actors;
    ia & rest_actors;
    for (size_t i = 0; i < oa_none; ++i)
    {
        actors()[i]->restore(*rest_actors[i].get());
    }
}

template <class archive>
void save_puzzle(archive &oa)
{
    oa & cpobjs;
    oa & cphere;
    oa & sfind_room("CP")->robjs();
    oa & cpuvec;
}

template <class archive>
void restore_puzzle(archive &ia)
{
    PuzzleContents cpo;
    ia & cpo;
    // Restore all objects into cpobjs.
    _ASSERT(cpo.size() == cpobjs.size());
    auto src_iter = cpo.cbegin();
    auto dest_iter = cpobjs.begin();
    for (; src_iter != cpo.cend(); ++src_iter, ++dest_iter)
    {
        dest_iter->clear();
        for (auto s : *src_iter)
        {
            ObjectP o = sfind_obj(s->oid());
            _ASSERT(o);
            dest_iter->push_back(o);
        }
    }

    ia & cphere;

    ObjList robjs, robjs2;
    ia & robjs;
    for (auto o : robjs)
    {
        ObjectP rest_obj = sfind_obj(o->oid());
        robjs2.push_back(rest_obj);
    }
    sfind_room("CP")->robjs() = robjs2;
    ia & cpuvec;
}

bool save_game(const std::string &f)
{
    auto h = robber_demon;
    std::ofstream sf(f);
    if (sf)
    {
        boost::archive::text_oarchive oa(sf);

        oa.register_type<Object>();
        oa.register_type<GObject>();

        const std::string emp;
        oa << save_version;
        dump_objects(oa);
        dump_rooms(oa);
        dump_robber(oa);
        bool temp = (sword_demon->haction() ? true : false);
        oa & temp;
        dump_clockers(oa);
        dump_winners(oa);
        // Save various globals (from MGVALS in dung.mud)
        oa & flags;
        oa & (binf ? binf->oid() : emp);
        oa & (btie ? btie->oid() : emp);
        oa & light_shaft;
        oa & moves;
        oa & raw_score;
        oa & deaths;
        oa & water_level;
        oa & cyclowrath;
        oa & eg_score;
        oa & beach_dig;
        oa & cphere;

        // Save room globals (RMGVALS)
        oa & (bloc ? bloc->rid() : emp);
        oa & (here ? here->rid() : emp);
        oa & (scol_room ? scol_room->rid() : emp);
        oa & (scol_active ? scol_active->rid() : emp);

        // OBJGVALS
        oa & (matobj ? matobj->oid() : emp);
        oa & (timber_tie ? timber_tie->oid() : emp);

        save_puzzle(oa);

        tell("Done.");
    }
    else
    {
        tell("Unable to open save file " + f);
    }

    return true;
}

bool restore_game(const std::string &f)
{
    bool rv = true;
    try
    {
        std::ifstream sf(f);
        if (!sf.is_open())
        {
            tell("Unable to open save file " + f + ".");
            rv = false;
        }
        else
        {
            boost::archive::text_iarchive ia(sf);
            ia.register_type<Object>();
            ia.register_type<GObject>();

            int sv;
            sf >> sv;
            if (sv != save_version)
            {
                tell("ERROR--Save file is incompatible with this version of Dungeon.");
                rv = false;
            }
            else
            {
                restore_objects(ia);
                restore_rooms(ia);
                restore_robber(ia);
                bool btemp;
                ia & btemp;
                sword_demon->haction(btemp ? sword_glow : nullptr);
                restore_clockers(ia);
                restore_winners(ia);
                // Restore various globals.
                ia & flags;
                std::string temp;
                ia & temp;
                binf = obj_from_str(temp);
                ia & temp;
                btie = obj_from_str(temp);
                ia & light_shaft;
                ia & moves;
                ia & raw_score;
                ia & deaths;
                ia & water_level;
                ia & cyclowrath;
                ia & eg_score;
                ia & beach_dig;
                ia & cphere;

                // RMGVALS
                ia & temp, bloc = room_from_str(temp);
                ia & temp, here = room_from_str(temp);
                ia & temp, scol_room = room_from_str(temp);
                ia & temp, scol_active = room_from_str(temp);

                // OBJGVALS
                ia & temp, matobj = obj_from_str(temp);
                ia & temp, timber_tie = obj_from_str(temp);

                restore_puzzle(ia);
            }
        }
    }
    catch (boost::archive::archive_exception &)
    {
        tell("Unable to process save file. Maybe corrupt or not a Zork save file?");
    }
    return true;
}

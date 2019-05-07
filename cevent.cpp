#include "stdafx.h"
#include <algorithm>
#include "act1.h"
#include "act2.h"
#include "act3.h"
#include "act4.h"
#include "cevent.h"
#include "melee.h"
#include "objfns.h"
#include "roomfns.h"
#include "ZorkException.h"
#include <tuple>

CEventP sphere_clock;

namespace
{
    CEventP mke(int tick, rapplic action, bool flag, const char *id, bool death)
    {
        return std::make_shared<CEvent>(tick, action, flag, id, death);
    }
}

std::array<CEventP, kev_numevs> ev =
{
    mke(0, obj_funcs::brochure, true, "BROIN", false),
    mke(0, obj_funcs::cyclops, true, "CYCIN", true),
    mke(0, obj_funcs::slide_cint, false, "SLDIN", true),
    mke(0, xb_cint, false, "XBIN", true),
    mke(0, xc_cint, false, "XCIN", true),
    mke(0, xbh_cint, false, "XBHIN", true),
    mke(0, room_funcs::forest_room, false, "FORIN", false),
    mke(0, cure_clock, false, "CURIN", false),
    mke(0, room_funcs::maint_room, true, "MNTIN", false),
    mke(0, obj_funcs::lantern, true, "LNTIN", false),
    mke(0, obj_funcs::match_function, true, "MATIN", false),
    mke(0, obj_funcs::candles, true, "CNDIN", false),
    mke(0, obj_funcs::balloon, true, "BINT", false),
    mke(0, burnup, true, "BRNIN", true),
    mke(0, obj_funcs::fuse_function, true, "FUSIN", true),
    mke(0, ledge_mung, true, "LEDIN", true),
    mke(0, safe_mung, true, "SAFIN", true),
    mke(0, volgnome, true, "VLGIN", false),
    mke(0, obj_funcs::gnome_function, true, "GNOIN", false),
    mke(0, obj_funcs::bucket, true, "BCKIN", false),
    mke(0, obj_funcs::sphere_function, true, "SPHIN", false),
    mke(0, scol_clock, true, "SCLIN", false),
    mke(0, end_game_herald, false, "EGHER", false),
    mke(0, zgnome_init, true, "ZGNIN", false),
    mke(0, obj_funcs::zgnome_function, true, "ZGLIN", false),
    mke(0, follow, false, "FOLIN", false),
    mke(0, obj_funcs::mrswitch, false, "MRINT", false),
    mke(0, obj_funcs::mends, false, "PININ", false),
    mke(0, inquisitor, false, "INQIN", false),
    mke(0, start_end, true, "STRTE", false),
};

CEvent::CEvent(int tick, rapplic action, bool flag, std::string_view id, bool death) :
    _ctick(tick),
    _caction(action),
    _cflag(flag),
    _cid(id),
    _cdeath(death)
{

}


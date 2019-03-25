#include "stdafx.h"
#include "b.h"
#include "room.h"
#include "rooms.h"

namespace
{
    const Bits bittys[] =
    {
        rlightbit,
        rairbit,
        rwaterbit,
        rsacredbit,
        rfillbit,
        rmungbit,
        rbuckbit,
        rhousebit,
        rendgame,
    };
    const char *desc[] =
    {
        "Lighted",
        "Mid-air",
        "Watery",
        "Robber-proof",
        "Water-source",
        "Destroyed",
        "Bucket",
        "part of the House",
        "part of the End Game",
    };
    static_assert(sizeof(bittys) / sizeof(bittys[0]) == sizeof(desc) / sizeof(desc[0]), "array sizes do not match");
}

bool bit_info(RoomP r)
{
    bool bb = false;
    if (r->rval() != 0)
    {
        princ("Room is valued at ");
        princ(r->rval());
        bb = true;
    }

    for (size_t i = 0; i < sizeof(bittys) / sizeof(bittys[0]); ++i)
    {
        if (rtrnn(r, bittys[i]))
        {
            if (bb)
            {
                princ(", ");
            }
            else
            {
                princ("Room is ");
            }
            bb = true;
            princ(desc[i]);
        }
    }

    if (bb)
        princ(".\n\n");

    return bb;
}

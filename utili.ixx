export module Zork:UtilI;
export import :Util;

bool in_room(const ObjectP& obj, const RoomP& here)
{
    bool found = false;
    const ObjectP& tobj = obj->ocan();
    if (tobj)
    {
        if (tobj->oroom() == here)
        {
            found = true;
        }
        else if (trnn(tobj, Bits::searchbit))
        {
            found = in_room(tobj, here);
        }
    }
    else
    {
        found = obj->oroom() == here;
    }
    return found;
}


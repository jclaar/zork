#include "util.h"
#include "adv.h"
#include "funcs.h"
#include "rooms.h"
#include "dung.h"
#include "parser.h"
import ZUtil;
import ZMemq;
import ZActors;

const HackP &get_demon(const char *id)
{
    const ObjectP &obj = find_obj(id);
    return *std::find_if(demons.cbegin(), demons.cend(), [&obj](const HackP &h)
    {
        return h->hobj() == obj;
    });
}


module;
#include "object.h"
#include "parser.h"
#include "adv.h"
#include "util.h"

export module ZActors:Impl;
export import :Iface;
import ZTell;
import ZMemq;
import ZGlobals;
import ZUtil;
import ZString;
import ZEvents;
import ZUtilObj;

namespace actor_funcs
{
    bool master_actor::operator()() const
    {
        bool rv = true;
        if (!trnn(sfind_obj("QDOOR"), Bits::openbit))
        {
            tell("There is no reply.");
        }
        else if (verbq("WALK"))
        {
            direction prso = as_dir(prsvec[1]);
            if ((prso == direction::South || prso == direction::Enter) && here == sfind_room("NCORR") ||
                (prso == direction::North || prso == direction::Enter) && here == sfind_room("SCORR"))
            {
                tell("'I am not permitted to enter the prison cell.'");
            }
            else
            {
                tell("'I prefer to stay where I am, thank you.'");
            }
        }
        else if (memq(prsa(), master_actions))
        {
            if (!verbq("STAY", "FOLLO"))
            {
                tell("'If you wish,' he replies.");
            }
            rv = false;
        }
        else
        {
            tell("'I cannot perform that action for you.'");
        }
        return rv;
    }
    bool robot_actor::operator()() const
    {
        const ObjectP& r = sfind_obj("ROBOT");
        const AdvP* ract;
        bool rv = true;

        if (auto& cage = sfind_obj("CAGE"); verbq("RAISE") && prso() == cage)
        {
            tell("The cage shakes and is hurled across the room.");
            clock_disable(sphere_clock);
            winner = &player();
            auto& c = sfind_room("CAGER");
            goto_(c);
            insert_object(cage, c);
            tro(cage, Bits::takebit);
            trz(cage, Bits::ndescbit);
            trz(r, Bits::ndescbit);
            tro(sfind_obj("SPHER"), Bits::takebit);
            remove_object(r);
            insert_object(r, c);
            (*(ract = r->oactor()))->aroom() = c;
            winner = ract;
            flags[FlagId::cage_solve] = true;
        }
        else if (verbq("EAT", "DRINK"))
        {
            tell("\"I am sorry but that action is difficult for a being with no mouth.\"");
        }
        else if (verbq("READ"))
        {
            tell("\"My vision is not sufficiently acute to read such small type.\"");
        }
        else if (memq(prsa(), robot_actions))
        {
            tell("\"Whirr, buzz, click!\"");
            rv = false;
        }
        else
        {
            tell("\"I am only a stupid robot and cannot perform that command.\"");
        }

        return rv;
    }

    bool dead_function::operator()() const
    {
        bool rv = true;
        if (verbq("WALK"))
        {
            rv = false;
            // Special case for dark_room. Kind of kludgy...
            if (auto m = memq(as_dir(prsvec[1]), here->rexits()))
            {
                if (auto* sgp = std::get_if<SetgExitP>(&std::get<1>(**m)))
                {
                    if ((*sgp)->name() == "dark_room")
                    {
                        tell("You cannot enter in your condition.");
                        rv = true;
                    }
                }
            }
        }
        else if (verbq("QUIT", "RESTA"))
        {
            return false;
        }
        else if (verbq("ATTAC", "BLOW", "DESTR", "KILL", "POKE", "STRIK", "SWING", "TAUNT"))
        {
            tell("All such attacks are vain in your condition."sv);
        }
        else if (verbq("OPEN", "CLOSE", "EAT", "DRINK", "INFLA", "DEFLA", "TURN", "BURN", "TIE", "UNTIE", "RUB"))
        {
            tell("Even such a simple action is beyond your capabilities."sv);
        }
        else if (verbq("TRNON"))
        {
            tell("You need no light to guide you."sv);
        }
        else if (verbq("SCORE"))
        {
            tell("How can you think of your score in your condition?"sv);
        }
        else if (verbq("TELL"))
        {
            tell("No one can hear you."sv);
        }
        else if (verbq("TAKE"))
        {
            tell("Your hand passes through its object."sv);
        }
        else if (verbq("DROP", "THROW", "INVEN"))
        {
            tell("You have no possessions."sv);
        }
        else if (verbq("DIAGN"))
        {
            tell("You are dead."sv);
        }
        else if (verbq("LOOK"))
        {
            tell("The room looks strange and unearthly", 1, empty(here->robjs()) ? "." : " and objects appear indistinct.");
            rtrnn(here, RoomBit::rlightbit) || tell("Although there is no light, the room seems dimly illuminated.");
            return false;
        }
        else if (verbq("BUG"))
        {
            rv = bugger()();
        }
        else if (verbq("FEATU"))
        {
            rv = feech()();
        }
        else if (verbq("PRAY"))
        {
            if (here == find_room("TEMP2"))
            {
                tro(find_obj("LAMP"), Bits::ovison);
                goto_(find_room("FORE1"));
                player()->aaction() = nullptr;
                gwim_disable = false;
                always_lit = false;
                flags[FlagId::dead] = false;
                tell(life);
            }
            else
            {
                tell("Your prayers are not heard."sv);
            }
        }
        else
            tell("You can't even do that."sv);
        return rv;
    }
}

namespace
{
    AdvArray actor_list;
}

export AdvArray& actors()
{
    return actor_list;
}

export void add_actor(e_oactor actor_name, const RoomP& room,
    const ObjectP& obj, rapplic action, int strength)
{
    actor_list[std::to_underlying(actor_name)] = std::make_unique<Adv>(room, obj, action, strength);
}

const AdvP& player() 
{ 
    return actors()[std::to_underlying(e_oactor::player)]; 
}
#include "stdafx.h"
#include <map>
#include "dung.h"
#include "memq.h"
#include "funcs.h"
#include "object.h"
#include "rooms.h"
#include "room.h"
#include "act1.h"
#include "act2.h"
#include "act3.h"
#include "act4.h"
#include "melee.h"
#include "adv.h"
#include "parser.h"
#include "ZorkException.h"
#include "b.h"

namespace
{
    void init_actors()
    {
        // Dungeon master
        add_actor(oa_master, get_room("BDOOR"), {}, 0, nullptr, get_obj("MASTE"), actor_funcs::master_actor, 30);
        add_actor(oa_player, get_room("WHOUS"), {}, 0, nullptr, get_obj("#####"), actor_funcs::dead_function, 0);
        add_actor(oa_robot, get_room("MAGNE"), {}, 0, nullptr, get_obj("ROBOT"), actor_funcs::robot_actor, 3);
    }

    std::vector<VerbP> init_actions(const std::initializer_list<const char *> &il)
    {
        std::vector<VerbP> acts;
        acts.reserve(il.size());
        std::transform(std::begin(il), std::end(il), std::back_inserter(acts),
            [](const char *v) { return find_verb(v); });
        return acts;
    }

    void init_robot()
    {
        robot_actions = init_actions({ "WALK", "TAKE", "DROP", "PUT", "JUMP", "PUSH", "THROW", "TURN" });
    }

    void init_master()
    {
        master_actions = init_actions({ "TAKE", "DROP", "PUT", "THROW", "PUSH", "TURN", "TRNTO", "SPIN",
            "FOLLO", "STAY", "OPEN", "CLOSE", "KILL" });
    }

}

// Bunch vector.
ObjVector bunch_cont()
{
    ObjVector ov(8, get_obj("#####"));
    return ov;
}
ObjVector bunuvec_cont = bunch_cont();
Iterator<ObjVector> bunuvec(bunuvec_cont, bunuvec_cont.end());
Iterator<ObjVector> bunch(bunuvec);

WordsPobl words_pobl;

// Globals from dung.mud appear here.
PhraseVecV make_prepvec()
{
    auto with_prep = find_prep("WITH");
    auto cretin = get_obj("#####");
    PhraseVecV pvv;
    std::generate_n(std::back_inserter(pvv), 5, [&with_prep, &cretin]() { return make_phrase(with_prep, cretin); });
    return pvv;
}

PhraseVecV prepvecb = make_prepvec();

PrepVec prepvec(prepvecb);

// Attacking things...
const std::vector<attack_state> def1 = { missed, missed, missed, missed,
    stagger, stagger,
    unconscious, unconscious,
    killed, killed, killed, killed, killed };
const std::vector<attack_state> def2a = { missed, missed, missed, missed, missed,
    stagger, stagger,
    light_wound, light_wound,
    unconscious };
const std::vector<attack_state> def2b = { missed, missed, missed,
    stagger, stagger,
    light_wound, light_wound, light_wound,
    unconscious,
    killed, killed, killed };
const std::vector<attack_state> def3a = { missed, missed, missed, missed, missed,
    stagger, stagger,
    light_wound, light_wound,
    serious_wound, serious_wound };
const std::vector<attack_state> def3b = { missed, missed, missed,
    stagger, stagger,
    light_wound, light_wound, light_wound,
    serious_wound, serious_wound, serious_wound };
const std::vector<attack_state> def3c = { missed,
    stagger, stagger,
    light_wound, light_wound, light_wound, light_wound,
    serious_wound, serious_wound, serious_wound };
const std::vector<std::vector<attack_state>> def1_res = { def1,{ def1.begin() + 1, def1.end() },{ def1.begin() + 2, def1.end() } };
const std::vector<std::vector<attack_state>> def2_res = { def2a, def2b, {def2b.begin() + 1, def2b.end()}, {def2b.begin() + 2, def2b.end()} };
const std::vector<std::vector<attack_state>> def3_res = { def3a, { def3a.begin() + 1, def3a.end()}, def3b, { def3b.begin() + 1, def3b.end() }, def3c };

ActionsPobl actions_pobl;
std::map<std::string, direction> directions_pobl = {
    { "#!#!#", NullExit },
    { "NORTH", North },
    { "SOUTH", South },
    { "EAST", East },
    { "WEST", West },
    { "LAUNC", Launc },
    { "LAND", Land },
    { "SE", Se },
    { "SW", Sw },
    { "NE", Ne },
    { "NW", Nw },
    { "UP", Up },
    { "DOWN", Down },
    { "ENTER", Enter },
    { "EXIT", Exit },
    { "CROSS", Cross }
};
std::string indentstrb = "        ";
SIterator indentstr(indentstrb, indentstrb.end());
ParseVecA prsveca;
ParseVec prsvec(prsveca);

int cyclowrath = 0;

// Cells for endgame
std::array<ObjList, 8> cells;
const ObjList cobjs = { get_obj("CDOOR"), get_obj("ODOOR") };
const ObjList nobjs = { get_obj("MDOOR"), get_obj("ODOOR") };
const ObjList pobjs = { get_obj("LDOOR") };
const ObjList weapons{ get_obj("STICK"), get_obj("KNIFE"), get_obj("SWORD"), get_obj("RKNIF") };
const ObjList villains{ get_obj("TROLL"), get_obj("THIEF"), get_obj("CYCLO") };
ObjList oppv(villains.size());
std::vector<int> villain_probs(villains.size());
const ObjList small_papers = { get_obj("BLABE"), get_obj("LABEL"), get_obj("CARD"), get_obj("WARNI"), get_obj("PAPER"), get_obj("GUIDE") };
const ObjList palobjs = { get_obj("SCREW"), get_obj("KEYS"), get_obj("STICK"), get_obj("PKEY") };
ObjList inqobjs;
const RoomList random_list = { get_room("LROOM"), get_room("KITCH"), get_room("CLEAR"), get_room("FORE3"), get_room("FORE2"),
    get_room("SHOUS"), get_room("FORE2"), get_room("KITCH"), get_room("EHOUS") };
const RoomP &northend = get_room("MRD");
RoomP mloc = get_room("MRB");
const RoomP startroom = mloc;
RoomP bloc = get_room("VLBOT");
const RoomP &southend = get_room("MRA");
VerbP buncher;
ObjectP bunch_obj = get_obj("*BUN*");
GObjectPtr it_object;
int deaths = 0;
HackP robber_demon;
HackP sword_demon;
HackP fight_demon;
const BestWeaponsList best_weapons = {
    std::make_shared<BestWeapons>(get_obj("TROLL"), get_obj("SWORD"), 1),
    std::make_shared<BestWeapons>(get_obj("THIEF"), get_obj("KNIFE"), 1),
};
std::vector<VerbP> robot_actions;
std::vector<VerbP> master_actions;

std::array<NumObjs, 8> numobjs;
const DirVec dirvec = {
    DVPair(North, 0),
    DVPair(Ne, 45),
    DVPair(East, 90),
    DVPair(Se, 135),
    DVPair(South, 180),
    DVPair(Sw, 225),
    DVPair(West, 270),
    DVPair(Nw, 315),
};

RoomP scol_room = get_room("BKVW");
// This gets initialized after directions are set up.
std::vector<ScolRooms> scol_rooms;
RoomP scol_active;
ScolWallsV scol_walls;

int cphere;
PuzzleContents cpobjs;
std::array<int, 64> cpuvec = {
    1,  1,  1,  1,  1,  1,  1,  1,
    1,  0, -1,  0,  0, -1,  0,  1,
    1, -1,  0,  1,  0, -2,  0,  1,
    1,  0,  0,  0,  0,  1,  0,  1,
    1, -3,  0,  0, -1, -1,  0,  1,
    1,  0,  0, -1,  0,  0,  0,  1,
    1,  1,  1,  0,  0,  0,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1 };
const CpExitV cpexits = {
    { North, -8 },
    { South, 8 },
    { East, 1 },
    { West, -1 },
    { Ne, -7 },
    { Nw, -9 },
    { Se, 9 },
    { Sw, 7 },
};

cpwall_vec cpwalls;

namespace
{
    // Bank
    void init_bank()
    {
        std::vector<ScolRooms> sc =
        {
            {East, get_room("BKVE")},
            {West, get_room("BKVW")},
            {North, get_room("BKTWI")},
            {South, get_room("BKVAU")}
        };
        scol_rooms.swap(sc);
        scol_active = find_room("FCHMP");

        scol_walls = {
            { get_room("BKVW"), get_obj("WEAST"), get_room("BKVE") },
            { get_room("BKVE"), get_obj("WWEST"), get_room("BKVW") },
            { get_room("BKTWI"), get_obj("WSOUT"), get_room("BKVAU") },
            { get_room("BKVAU"), get_obj("WNORT"), get_room("BKTWI") }
        };
    }

    void init_endgame()
    {
        static const char *num[] =
        {
            "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT"
        };
        static_assert(sizeof(num) / sizeof(num[0]) == numobjs.size(), "Array sizes do not match");
        int index = 1;
        std::transform(std::begin(num), std::end(num), numobjs.begin(), [&index](const char *n)
            {
                return NumObjs(get_obj(n), index++);
            });
        //for (int i = 0; i < (int) numobjs.size(); ++i)
        //{
        //    numobjs[i] = NumObjs(get_obj(num[i]), i + 1);
        //}
    }

    // Puzzle rooms
    void init_puzzle_rooms()
    {
        // MDL is one-based for arrays, so these need to have one less
        // than the value specified in the MDL code.
        cpobjs[37 - 1] = { get_obj("GCARD") };
        cpobjs[52 - 1] = { get_obj("CPSLT"), get_obj("CPDOR") };
        cpwalls = {
            cpwall_val(get_obj("CPSWL"), 8),
            cpwall_val(get_obj("CPNWL"), -8),
            cpwall_val(get_obj("CPEWL"), 1),
            cpwall_val(get_obj("CPWWL"), -1)
        };
    }

    void init_object_locs()
    {
        // A few object locations from dung.mud.
        const RoomP &cp = get_room("CP");
        for (auto o : { "CPSLT", "CPDOR", "GCARD" })
        {
            sfind_obj(o)->oroom(cp);
        }
    }

    void init_demons()
    {
        std::list<RoomP> empty_rooms;
        add_demon(robber_demon = std::make_shared<hack>(robber, ObjList(), rooms(), *rooms().begin(), get_obj("THIEF")));
        add_demon(sword_demon = std::make_shared<hack>(sword_glow, villains, empty_rooms, *rooms().begin(), get_obj("SWORD")));
        add_demon(fight_demon = std::make_shared<hack>(fighting, villains, empty_rooms, *rooms().begin(), get_obj("TROLL")));
    }

    void init_actions()
    {
        sadd_action("1ST?", room_funcs::time);
        sadd_action("C-INT", room_funcs::time);

        sadd_action("ANSWE", answer);
        vsynonym("ANSWE", "RESPO");

        add_action("ATTAC", "Attack", ActionVec{
            AnyV{
            AL{
            villain,
            robjs(),
            reach()
        },
            "WITH",
            AL{
            weaponbit,
            aobjs(),
            have()
        },
            AVSyntax("ATTAC", attacker)
        }
        });
        vsynonym("ATTAC", "FIGHT", "HURT", "INJUR", "HIT" );

        sadd_action("BACK", backer);
        sadd_action("BLAST", blast);

        add_action("BLOW", "Blow", ActionVec{
            AnyV{ "UP", obj(), "WITH", AL{ toolbit, reach(), robjs(), aobjs() }, AVSyntax("INFLA", inflater), driver() },
            AnyV{ "UP", obj(), AVSyntax("BLAST", blast) },
            AnyV{ "INTO", obj(), AVSyntax("BLOIN", breathe) }
        });

        add_action("BOARD", "Board", ActionVec{ AnyV{ AL{ vehbit, robjs(), reach() }, AVSyntax("BOARD", board) } });

        sadd_action("BRIEF", brief);

        add_action("BRUSH", "Brush", ActionVec{
            AnyV{ AL{ -1, aobjs(), robjs(), reach() }, AVSyntax("BRUSH", brush), driver() },
            AnyV{ AL{ -1, aobjs(), robjs(), reach() }, "WITH", obj(), AVSyntax("BRUSH", brush) }
        });

        sadd_action("BUG", bugger);
        vsynonym("BUG", "GRITC", "COMPL" );

        add_action("BURN", "Burn", ActionVec{ AnyV{ AL{ burnbit, aobjs(), robjs(), reach() }, "WITH", AL{ flamebit, aobjs(), robjs(), have() }, AVSyntax{ "BURN", burner } } });
        vsynonym("BURN", "INCIN", "IGNIT" );

        sadd_action("CHOMP", chomp);
        vsynonym("CHOMP", "LOSE", "BARF" );

        add_action("CLIMB", "Climb", ActionVec{
            AnyV({ "UP", AL{ climbbit, robjs() }, AVSyntax("CLUP", climb_up) }),
            AnyV({ "DOWN", AL{ climbbit, robjs() }, AVSyntax("CLDN", climb_down) }),
            AnyV({ AL{ climbbit, robjs() }, AVSyntax("CLUDG", climb_foo) })
        });

        add_action("CLOSE", "Close", ActionVec{
            AnyV{
            AL{ std::list<Bits>{doorbit, contbit}, reach(), aobjs(), robjs() } , AVSyntax("CLOSE", closer)
        }
        });

        oneadd_action("COUNT", "Count", count);
        vsynonym("COUNT", "MANY" );

        sadd_action("CURSE", curses);
        vsynonym("CURSE", "SHIT", "FUCK", "DAMN" );

        oneadd_action("DEFLA", "Deflate", deflater);

        sadd_action("DEAD!", room_funcs::time);

        add_action("DESTR", "Destroy", ActionVec{
            AnyV{ AL{ -1, reach(), robjs(), aobjs() }, AVSyntax("MUNG", munger), driver() },
            AnyV{ AL{ -1, reach(), robjs(), aobjs() }, "WITH", AL{ -1, aobjs(), take() }, AVSyntax("MUNG", munger) },
        });
        vsynonym("DESTR", "MUNG", "DAMAG");

        sadd_action("DIAGN", diagnose);

        add_action("DIG", "dig", ActionVec{
            AnyV{
            AL{ digbit, robjs() },
            "WITH",
            AL{ toolbit, aobjs(), have() },
            AVSyntax("DIG", digger),
            driver()
        },
            AnyV({
            "INTO",
            AL{ digbit, robjs() },
            "WITH",
            AL{ toolbit, aobjs(), have() },
            AVSyntax("DIG", digger)
        })
        });

        add_action("DISEM", "Disembark from", ActionVec{
            AnyV{ AL{ vehbit, robjs() }, AVSyntax("DISEM", unboard) }
        });

        sadd_action("DOC", doc);

        add_action("DRINK", "Drink", ActionVec{
            AnyV{ AL{ drinkbit, aobjs(), robjs(), reach() }, AVSyntax("DRINK", eat) }
        });
        vsynonym("DRINK", "IMBIB", "SWALL" );

        add_action("DROP", "Drop", ActionVec{
            AnyV{
            AL{ -1, aobjs(), reach() },
            AVSyntax("DROP", dropper),
            driver()
        },
            AnyV{
            AL{ -1, aobjs(), reach() },
            "DOWN",
            obj(),
            AVSyntax("PUT", putter)
        },
            AnyV{
            AL{ -1, aobjs(), reach() },
            "IN",
            obj(),
            AVSyntax("PUT", putter)
        }
        });
        vsynonym("DROP", "RELEA" );

        add_action("EAT", "Eat", ActionVec{
            AnyV{ AL{ foodbit, aobjs(), robjs(), reach(), take() },AVSyntax("EAT", eat) }
        });
        vsynonym("EAT", "CONSU", "GOBBL", "MUNCH", "TASTE" );

        onenradd_action("EXAMI", "Examine", room_info);
        vsynonym("EXAMI", "DESCR", "WHAT", "WHATS", "WHAT'" );

        sadd_action("EXORC", exorcise);
        vsynonym("EXORC", "XORCI");

        add_action("EXTIN", "Turn off", ActionVec{
            AnyV{ AL{ lightbit, reach(), aobjs(), robjs(), take() }, AVSyntax("TRNOF", lamp_off) }
        });
        vsynonym("EXTIN", "DOUSE" );

        add_action("ENTER", "Enter", ActionVec{
            AnyV{ AVSyntax("ENTER", enter) },
            AnyV{ obj(), AVSyntax("GTHRO", through) }
        });

        sadd_action("FEATU", feech);
        vsynonym("FEATU", "COMME", "SUGGE", "IDEA" );

        sadd_action("FGHT?", room_funcs::time);

        add_action("FILL", "Fill", ActionVec{
            AnyV{ AL{ contbit, reach(), aobjs(), robjs() }, "WITH", obj(), AVSyntax("FILL", fill) },
            AnyV{ AL{ contbit, reach(), aobjs(), robjs() }, AVSyntax("FILL", fill) }
        });

        onenradd_action("FIND", "Find", find);
        vsynonym("FIND", "WHERE", "SEEK", "SEE" );

        add_action("FOLLO", "Follow", ActionVec{
            AnyV{ AVSyntax("FOLLO", follow) },
            AnyV{ obj(), AVSyntax("FOLLO", follow) }
        });

        sadd_action("FOO", jargon);
        vsynonym("FOO", "BAR", "BLETC" );

        sadd_action("FROBO", frobozz);
        sadd_action("GERON", geronimo);

        add_action("GIVE", "Give", ActionVec{
            AnyV{ obj(), "TO", AL{ vicbit, reach(), robjs() }, AVSyntax("GIVE", dropper), driver() },
            AnyV{ AL{ vicbit, reach(), robjs() }, obj(), AVSyntax("GIVE", dropper), flip() }
        });

        sadd_action("GO-IN", room_funcs::time);

        oneadd_action("GTHRO", "Go through", through);
        vsynonym("GTHRO", "THROU", "INTO" );

        sadd_action("HACK?", room_funcs::time);
        add_action("HELLO", "Hello", ActionVec{
            AnyV{ AVSyntax("HELLO", hello), driver() },
            AnyV{ obj(), AVSyntax("HELLO", hello) }
        });
        sadd_action("HELP", help);

        sadd_action("IN!", room_funcs::time);
        sadd_action("INCAN", incant);

        add_action("INFLA", "Inflate", ActionVec{
            AnyV{ obj(), "WITH", AL{ toolbit, reach(), robjs(), aobjs() }, AVSyntax("INFLA", inflater) }
        });

        sadd_action("INFO", info);

        sadd_action("INVEN", invent);
        vsynonym("INVEN", "LIST", "I" );

        add_action("JUMP", "jump", ActionVec{
            AnyV{
            AVSyntax("JUMP", leaper)
        },
            AnyV{
            "OVER", obj(), AVSyntax("JUMP", leaper)
        }
        });
        vsynonym("JUMP", "LEAP" );

        oneadd_action("KICK", "Taunt", kicker);
        vsynonym("KICK", "BITE", "TAUNT" );

        add_action("KILL", "Kill", ActionVec{
            AnyV{ AL{ villain, robjs(), reach() }, "WITH", AL{ weaponbit, aobjs(), have() }, AVSyntax("KILL", killer) }
        });

        add_action("KNOCK", "Knock", ActionVec{
            AnyV{ "AT", obj(), AVSyntax("KNOCK", knock), driver() },
            AnyV{ "ON", obj(), AVSyntax("KNOCK", knock) },
            AnyV{ "DOWN", AL{ vicbit, reach(), robjs() }, AVSyntax("ATTAC", attacker) }
        });
        vsynonym("KNOCK", "RAP" );

        add_action("LEAVE", "Enter", ActionVec{
            AnyV{ AVSyntax("LEAVE", leave) },
            AnyV{ obj(), AVSyntax("DROP", dropper) }
        });

        add_action("LIGHT", "Light", ActionVec{
            AnyV{ AL{ lightbit, reach(), aobjs(), robjs(), take() }, AVSyntax("LIGHT", lamp_on), driver() },
            AnyV{ AL{ lightbit, reach(), aobjs(), robjs() }, "WITH", AL{ flamebit, aobjs(), take() }, AVSyntax("BURN", burner) }
        });

        add_action("LOCK", "Lock", ActionVec{
            AnyV{ AL{ -1, robjs() }, "WITH", AL{ toolbit, aobjs(), robjs(), take() }, AVSyntax("LOCK", locker) }
        });

        add_action("LOOK", "Look", ActionVec{
            AnyV{
            AVSyntax("LOOK", room_desc)
        },
            AnyV{
            "AT", nrobj(), AVSyntax("LKAT", room_desc)
        },
            AnyV{
            "THROU", nrobj(), AVSyntax("LKIN", look_inside)
        },
            AnyV{
            "UNDER", nrobj(), AVSyntax("LKUND", look_under)
        },
            AnyV{
            "IN", nrobj(), AVSyntax("LKIN", look_inside)
        },
            AnyV{
            "AT", nrobj(), "WITH", obj(), AVSyntax("READ", reader)
        },
            AnyV{
            "AT", nrobj(), "THROU", obj(), AVSyntax("READ", reader)
        },
        });
        vsynonym("LOOK", "L", "STARE", "GAZE" );

        oneadd_action("LOWER", "Lower", r_l);

        oneadd_action("MAKE", "Make", maker);
        vsynonym("MAKE", "BUILD");

        add_action("LUBRI", "Lubricate", ActionVec{
            AnyV{ obj(), "WITH", AL{ -1, aobjs(), reach() }, AVSyntax("OIL", oil) }
        });
        vsynonym("LUBRI", "OIL", "GREAS" );

        add_action("MELT", "Melt", ActionVec{
            AnyV{ obj(), "WITH", AL{ flamebit, reach(), aobjs(), robjs() }, AVSyntax("MELT", melter) }
        });
        vsynonym("MELT", "LIQUI" );

        add_action("MOVE", "Move", ActionVec{
            AnyV{ AL{ -1, robjs() }, AVSyntax("MOVE", move) }
        });

        sadd_action("MUMBL", mumbler);
        vsynonym("MUMBL", "SIGH");

        sadd_action("NOOBJ", no_obj_hack);

        // Probably not necessary here...
        //sadd_action("NAME", srname);

        sadd_action("OBJEC", room_obj);

        sadd_action("ODYSS", sinbad);
        vsynonym("ODYSS", "ULYSS" );

        oneadd_action("OOPS", "Oops", oops);

        add_action("OPEN", "Open", ActionVec{
            AnyV{AL{ std::list<Bits>{doorbit, contbit}, reach(), aobjs(), robjs() }, AVSyntax("OPEN", opener), driver() },
            AnyV{AL{ std::list<Bits>{doorbit, contbit}, reach(), aobjs(), robjs()}, "WITH", AL{weaponbit, toolbit, robjs(), aobjs(), have()}, AVSyntax("OPEN", opener)}
        });

        sadd_action("OUT!", room_funcs::time);

        add_action("PICK", "Pick", ActionVec{
            AnyV{ "UP", AL{ std::list<Bits>{takebit, trytakebit}, reach(), robjs(), aobjs() }, AVSyntax("TAKE", takefn) }
        });

        add_action("PLAY", "Play", ActionVec{
            AnyV{ obj(), AVSyntax("PLAY", play), driver() },
            AnyV{ obj(), "WITH", AL{ toolbit, aobjs(), robjs(), reach(), take() }, AVSyntax("PLAY", play) }
        });

        add_action("PLUG", "Plug", ActionVec{
            AnyV{ obj(), "WITH", obj(), AVSyntax("PLUG", plugger) }
        });
        vsynonym("PLUG", "GLUE", "PATCH" );

        sadd_action("PLUGH", advent);
        vsynonym("PLUGH", "XYZZY" );

        add_action("POKE", "Poke", ActionVec{
            AnyV{ AL{ villain, reach(), robjs() }, "WITH", AL{ weaponbit, aobjs(), have() }, AVSyntax("POKE", munger) }
        });
        vsynonym("POKE", "JAB", "BREAK", "BLIND" );

        add_action("POUR", "Pour", ActionVec{
            AnyV{ AL{ -1, aobjs(), reach() }, AVSyntax("POUR", dropper), driver() },
            AnyV{ AL{ -1, aobjs(), reach() }, "IN", obj(), AVSyntax("POUR", dropper) },
            AnyV{ AL{ -1, aobjs(), reach() }, "ON", obj(), AVSyntax("PORON", pour_on) }
        });
        vsynonym("POUR", "SPILL" );

        sadd_action("PRAY", prayer);

        add_action("PULL", "Pull", ActionVec{
            AnyV{ AL{ -1, reach(), robjs() }, AVSyntax("MOVE", move), driver() },
            AnyV{ "ON", AL{ -1, reach(), robjs() }, AVSyntax("MOVE", move) }
        });
        vsynonym("PULL", "TUG" );

        add_action("PUMP", "Pump", ActionVec{
            AnyV{ "UP", obj(), AVSyntax("PMPUP", pumper) }
        });

        add_action("PUSH", "Push", ActionVec{
            AnyV{
            obj(), AVSyntax("PUSH", pusher)
        },
            AnyV{
            "ON", obj(), AVSyntax("PUSH", pusher)
        },
            AnyV{
            obj(), "UNDER", obj(), AVSyntax("PTUND", put_under)
        }
        });
        vsynonym("PUSH", "PRESS");

        add_action("PUT", "Put", ActionVec{
            AnyV{
            obj(), "IN", obj(), AVSyntax("PUT", putter), driver()
        },
            AnyV{
            "DOWN", obj(), AVSyntax("DROP", dropper)
        },
            AnyV{
            obj(), "UNDER", obj(), AVSyntax("PTUND", put_under)
        }
        });
        vsynonym("PUT", "STUFF", "PLACE", "INSER" );

        sadd_action("QUIT", finish);
        vsynonym("QUIT", "Q", "GOODB" );

        add_action("RAISE", "Raise", ActionVec{
            AnyV{ obj(), AVSyntax("RAISE", r_l), driver() },
            AnyV{ "UP", obj(), AVSyntax("RAISE", r_l) }
        });
        vsynonym("RAISE", "LIFT" );

        add_action("READ", "Read", ActionVec{
            AnyV{ AL{ readbit, reach(), aobjs(), robjs(), try_() }, AVSyntax("READ", reader), driver() },
            AnyV{ AL{ readbit, reach(), aobjs(), robjs(), try_() }, "WITH", obj(), AVSyntax("READ", reader) },
            AnyV{ AL{ readbit, reach(), aobjs(), robjs(), try_() }, "THROU", obj(), AVSyntax("READ", reader) }
        });

        sadd_action("REPEN", repent);
        sadd_action("RESTA", restart);
        sadd_action("RESTO", do_restore);

        add_action("RING", "Ring", ActionVec{
            AnyV{ obj(), AVSyntax("RING", ring), driver() },
            AnyV{ obj(), "WITH", obj(), AVSyntax("RING", ring) }
        });

        sadd_action("RNAME", room_name);
        sadd_action("ROOM", room_room);
        oneadd_action("RUB", "Rub", rubber);
        vsynonym("RUB", "CARES", "TOUCH", "FEEL", "FONDL" );
        sadd_action("SAVE", do_save);
        sadd_action("SCORE", score);
        sadd_action("SCRIP", do_script);

        add_action("SEND", "Send", ActionVec{
            AnyV{ "FOR", obj(), AVSyntax("SEND", sender) }
        });

        oneadd_action("SHAKE", "Shake", shaker);

        sadd_action("SKIP", skipper);
        vsynonym("SKIP", "HOP" );

        add_action("SLIDE", "Slide", ActionVec{
            AnyV{ obj(), "UNDER", obj(), AVSyntax("PTUND", put_under) }
        });

        oneadd_action("SMELL", "Smell", smeller);
        vsynonym("SMELL", "SNIFF" );

        oneadd_action("SPIN", "Spin", turnto);

        oneadd_action("SQUEE", "Squeeze", squeezer);

        sadd_action("STATS", stats);

        sadd_action("STAY", stay);

        add_action("STRIK", "Strike", ActionVec{
            AnyV{ AL{ vicbit, vicbit, reach(), robjs() }, "WITH", AL{ weaponbit, aobjs(), robjs(), have() }, AVSyntax("ATTAC", attacker) },
            AnyV{ AL{ vicbit, vicbit, reach(), robjs() }, AVSyntax("ATTAC", attacker), driver() },
            AnyV{ AL{ -1, reach(), robjs(), aobjs(), try_() }, AVSyntax("LIGHT", lamp_on) }
        });


        sadd_action("SUPER", superbrief);

        sadd_action("SWIM", swimmer);
        vsynonym("SWIM", "BATHE", "WADE" );

        add_action("SWING", "Swing", ActionVec{
            AnyV{ AL{ weaponbit, aobjs(), have() }, "AT", AL{ villain, reach(), robjs() }, AVSyntax("SWING", swinger) }
        });

        add_action("TAKE", "Take", ActionVec{
            AnyV{ AL{ std::list<Bits>{trytakebit, takebit}, reach(), robjs(), aobjs() }, AVSyntax("TAKE", takefn), driver() },
            AnyV{ "IN", AL{ vehbit, robjs(), reach() }, AVSyntax("BOARD", board) },
            AnyV{ "OUT", AL{ vehbit, robjs(), reach() }, AVSyntax("DISEM", unboard) },
            AnyV{ AL{ std::list<Bits>{takebit, trytakebit}, reach(), robjs(), aobjs() }, "OUT", obj(), AVSyntax("TAKE", takefn) },
            AnyV{ AL{ std::list<Bits>{takebit, trytakebit}, reach(), robjs(), aobjs() }, "FROM", obj(), AVSyntax("TAKE", takefn) },
        });
        vsynonym("TAKE", "REMOV", "GET", "HOLD", "CARRY" );

        add_action("TELL", "Tell", ActionVec{
            AnyV{ AL{ actorbit, robjs() }, AVSyntax("TELL", command) }
        });
        vsynonym("TELL", "COMMA", "REQUE" );

        sadd_action("TEMPL", treas);

        add_action("THROW", "Throw", ActionVec{
            AnyV{ AL{ -1, aobjs(), have() }, "AT", AL{ vicbit, reach(), robjs() }, AVSyntax("THROW", dropper), driver() },
            AnyV{ AL{ -1, aobjs(), have() }, "THROU", AL{ vicbit, reach(), robjs() }, AVSyntax("THROW", dropper) },
            AnyV{ AL{ -1, aobjs(), have() }, "IN", obj(), AVSyntax("PUT", putter) },
        });
        vsynonym("THROW", "HURL", "CHUCK" );

        add_action("TIE", "Tie", ActionVec{
            AnyV{ obj(), "TO", obj(), AVSyntax("TIE", tie) },
            AnyV{ "UP", AL{ vicbit, reach(), robjs() }, "WITH", AL{ toolbit, reach(), robjs(), aobjs(), have() }, AVSyntax("TIEUP", tie_up) }
        });
        vsynonym("TIE", "FASTE" );

        sadd_action("TREAS", treas);

        add_action("TURN", "Turn", ActionVec{
            AnyV{ AL{ turnbit, reach(), aobjs(), robjs() }, "WITH", AL{ toolbit, robjs(), aobjs(), have() }, AVSyntax("TURN", turner), driver() },
            AnyV{ "ON", AL{ lightbit, reach(), aobjs(), robjs(), take() }, AVSyntax("TRNON", lamp_on) },
            AnyV{ "OFF", AL{ lightbit, reach(), aobjs(), robjs(), take() }, AVSyntax("TRNOF", lamp_off) },
            AnyV{ AL{ turnbit, reach(), aobjs(), robjs() }, "TO", AL{ -1, robjs() }, AVSyntax("TRNTO", turnto) }
        });
        vsynonym("TURN", "SET" );

        add_action("UNLOC", "Unlock", ActionVec{
            AnyV{ AL{ -1, reach(), robjs() }, "WITH", AL{ toolbit, aobjs(), robjs(), take() }, AVSyntax("UNLOC", unlocker) }
        });

        add_action("UNTIE", "Untie", ActionVec{
            AnyV{ AL{ tiebit, reach(), robjs(), aobjs() }, AVSyntax("UNTIE", untie), driver() },
            AnyV{ AL{ tiebit, reach(), robjs(), aobjs() }, "FROM", obj(), AVSyntax("UTFRM", untie_from) }
        });
        vsynonym("UNTIE", "RELEA", "FREE" );

        add_action("WAKE", "Wake", ActionVec{
            AnyV{ AL{ vicbit, robjs() }, AVSyntax("WAKE", alarm), driver() },
            AnyV{ "UP", AL{ vicbit, robjs() }, AVSyntax("WAKE", alarm) }
        });
        vsynonym("WAKE", "AWAKE", "SURPR", "START" );

        add_action("WALK", "Walk", ActionVec{
            AnyV{ obj(), AVSyntax("WALK", walk) },
            AnyV{ "IN", obj(), AVSyntax("GTHRO", through) },
            AnyV{ "THROU", obj(), AVSyntax("GTHRO", through) }
        });
        vsynonym("WALK", "GO", "RUN", "PROCE" );

        add_action("WAVE", "Wave", ActionVec{
            AnyV{ AL{ -1, aobjs() }, AVSyntax("WAVE", waver) }
        });
        vsynonym("WAVE", "BRAND" );

        sadd_action("TEMPL", treas);
        sadd_action("TIME", play_time);
        sadd_action("TREAS", treas);
        sadd_action("UNSCR", do_unscript);
        sadd_action("VERBO", verbose);
        sadd_action("VERSI", version);
        sadd_action("WAIT", wait);
        sadd_action("WIN", win);
        vsynonym("WIN", "WINNA" );

        add_action("WIND", "Wind", ActionVec{
            AnyV{ obj(), AVSyntax("WIND", wind) },
            AnyV{ "UP", obj(), AVSyntax("WIND", wind) }
        });

        sadd_action("WISH", wisher);
        sadd_action("YELL", yell);
        sadd_action("ZORK", zork);

        sadd_action("TERMI", terminal);

#ifdef _DEBUG
        // Displays room bits
        sadd_action("RBITS", []
        {
            if (!bit_info(here))
            {
                tell("No bit information available.");
            }
            else
            {
                flags[tell_flag] = true;
            }
            return true;
        });

        // Handy function to locate any object.
        sadd_action("LOCAT", [] {
            std::string obj;
            std::cout << "Locate what? ";
            std::cin >> obj;
            std::transform(obj.begin(), obj.end(), obj.begin(), toupper);
            obj = obj.substr(0, 5);
            if (is_obj(obj))
            {
                ObjectP objp = find_obj(obj);
                if (objp->oroom())
                {
                    tell("The " + objp->odesc2() + " is in " + objp->oroom()->rid());
                }
                else if (memq(objp, player()->aobjs()))
                {
                    tell("You're holding it, dummy.");
                }
                else
                {
                    tell("The " + objp->odesc2() + " is nowhere.");
                }
            }
            else
            {
                tell("Unknown object");
            }

            return true;
        });

        // Function to list all treasures and their locations.
        sadd_action("LISTT", [] {
            auto &op = object_pobl();
			std::set<ObjectP> dups;
            for (auto o : op)
            {
                ObjectP obj = *o.second.begin();
                if (obj->otval() > 0)
                {
					if (dups.find(obj) == dups.end())
					{
						std::string loc;
						if (obj->oroom())
							loc = obj->oroom()->rid();
						else if (obj->ocan())
							loc = obj->ocan()->oid();
						else
							loc = "unknown";
						tell("Obj " + obj->odesc2() + " is in " + loc);
						dups.insert(obj);
					}
                }
            }
            return true;
        });

        sadd_action("RMID", []() { return tell("Room: " + here->rid()); });

        // Function to display where the thief is, and is going to be, for debugging.
        sadd_action("WT", []()
        {
            // Print the room the thief is in, and the next two rooms he will be in.
            RoomList &rl = robber_demon->hrooms();
            auto iter = rl.begin();

            tell("Cur: " + (*iter)->rid());
            for (int i = 0; i < 2; ++i)
            {
                while (iter != rl.end())
                {
                    if (rtrnn(*iter, rsacredbit) || rtrnn(*iter, rendgame) || !rtrnn(*iter, rlandbit))
                    {
                        ++iter;
                    }
                    else
                    {
                        break;
                    }
                }
                if (iter != rl.end())
                {
                    tell("Next: " + (*iter)->rid());
                    ++iter;
                }
            }
            return true;
        });
#endif
    }

    void init_questions()
    {
        add_question("From which room can one enter the robber's hideaway without passing\n"
            "through the cyclops room?", { "TEMPL" });
        add_question("Beside the Temple, to which room is it possible to go from the Altar?", { "FORES" });
        add_question("What is the absolute minimum specified value of the Zork treasures,\n"
            "in Zorkmids?", { "30003" });
        add_question("What object is of use in determining the function of the iced cakes?", { get_obj("FLASK") });
        add_question("What can be done to the Mirror that is useful?", { actions_pobl["RUB"] });
        add_question("The taking of which object offends the ghosts?", { get_obj("BONES") });
        add_question("Which object in the dungeon is haunted?", { get_obj("RKNIF") });
        add_inqobj(get_obj("KNIFE"));
        add_question("In which room is 'Hello Sailor!' useful?", { "NONE", "NOWHE" });
    }

}

void dir_syns()
{
    const std::pair<const char*, const char*> ds[] = {
        std::make_pair("NORTH", "N"),
        std::make_pair("SOUTH", "S"),
        std::make_pair("EAST", "E"),
        std::make_pair("WEST", "W"),
        std::make_pair("UP", "U"),
        std::make_pair("DOWN", "D"),
        std::make_pair("ENTER", "IN"),
        std::make_pair("CROSS", "TRAVE"),
    };
    for (auto &d : ds)
    {
        dsynonym(d.first, d.second);
    }

    dsynonym("EXIT", "OUT", "LEAVE" );
}

void init_dung()
{
    buncher = std::make_shared<verb>("BUNCH", bunchem);
    words_pobl["BUNCH"] = buncher;

    init_gobjects();
    add_demon(clocker = std::make_shared<hack>(clock_demon, ObjList(), std::list<RoomP>(), RoomP(), ObjectP()));

    dir_syns();

	init_objects();
    init_synonyms();
    init_rooms();
    init_actors();
    init_demons();

    init_object_locs();

    init_puzzle_rooms();
    init_bank();
    init_endgame();

    // Add prepositions
    add_buzz( "AND", "BY", "IS", "A", "THE", "AN", "TODAY", "HOW", "CHIMN" );
    add_zork(kPrep, "OVER", "WITH", "AT", "TO", "IN", "FOR", "DOWN", "UP", "UNDER", "OF", "FROM" );
    synonym("WITH", "USING", "THROU" );
    synonym("IN", "INSID", "INTO" );

    init_actions();

    init_robot();
    init_master();

    add_buncher({ "TAKE", "DROP", "PUT", "COUNT" });

    init_questions();
}

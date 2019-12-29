#include "stdafx.h"
#include "strings.h"

#define D "%D%"
#define W "%W%"

const tofmsgs &cyclops_melee()
{
    static const tofmsgs cm =
    {
        {   "The Cyclops misses, but the backwash almost knocks you over.",
            "The Cyclops rushes you, but runs into the wall.",
            "The Cyclops trips over his feet trying to get at you.",
            "The Cyclops unleashes a roundhouse punch, but you have time to dodge."
        },
        {
            "The Cyclops knocks you unconscious.",
            "The Cyclops sends you crashing to the floor, unconscious."
        },
        {
            "The Cyclops raises his arms and crushes your skull.",
            "The Cyclops has just essentially ripped you to shreds.",
            "The Cyclops decks you.  In fact, you are dead.",
            "The Cyclops breaks your neck with a massive smash.",
        },
        {
            "A quick punch, but it was only a glancing blow.",
            "The Cyclops grabs but you twist free, leaving part of your cloak.",
            "A glancing blow from the Cyclops' fist.",
            "The Cyclops chops at you with the side of his hand, and it connects.\nbut not solidly."
        },
        {
            "The Cyclops gets a good grip and breaks your arm.",
            "The monster smashes his huge fist into your chest, breaking several\nribs.",
            "The Cyclops almost knocks the wind out of you with a quick punch.",
            "A flying drop kick breaks your jaw.",
            "The Cyclops breaks your leg with a staggering blow."
        },
        {
            "The Cyclops knocks you silly, and you reel back.",
            "The Cyclops lands a punch that knocks the wind out of you.",
            "Heedless of your weapons, the Cyclops tosses you against the rock\nwall of the room.",
            "The Cyclops grabs you, and almost strangles you before you wiggle\nfree, breathless."
        },
        {
            "The Cyclops grabs you by the arm, and you drop your " W ".",
            "The Cyclops kicks your " W " out of your hand.",
            "The Cyclops grabs your " W ", tastes it, and throws it to the\nground in disgust.",
            "The monster grabs you on the wrist, squeezes, and you drop your\n" W " in pain."
        },
        {
            "The Cyclops is so excited by his success that he neglects to kill\nyou.",
            "The Cyclops, momentarily overcome by remorse, holds back.",
            "The Cyclops seems unable to decide whether to broil or stew his\ndinner."
        },
        {
            "The Cyclops, no sportsman, dispatches his unconscious victim."
        }
    };
    return cm;
};
const tofmsgs &knife_melee()
{
    static const tofmsgs km =
    {
        {
            "Your stab misses the " D " by an inch.",
            "A good slash, but it misses the " D " by a mile.",
            "You charge, but the " D " jumps nimbly aside.",
            "A quick stroke, but the " D " is on guard.",
            "A good stroke, but it's too slow, the " D " dodges."
        },
        {
            "The haft of your knife knocks out the " D ".",
            "The " D " drops to the floor, unconscious.",
            "The " D " is knocked out!",
        },
        {
            "The end for the " D " as your knife severs his jugular.",
            "The fatal thrust strikes the " D " square in the heart:  He dies.",
            "The " D " takes a final blow and slumps to the floor dead."
        },
        {
            "The " D " is slashed on the arm, blood begins to trickle down.",
            "Your knife point pinks the " D " on the wrist, but it's not serious.",
            "Your stroke lands, but it was only the flat of the blade.",
            "The blow lands, making a shallow gash in the " D "'s arm!"
        },
        {
            "The " D " receives a deep gash in his side.",
            "A savage cut on the leg stuns the " D ", but he can still fight!",
            "Slash!  Your stroke connects!  The " D " could be in serious trouble!"
        },
        {
            "The " D " drops to his knees, staggered.",
            "The " D " is confused and can't fight back.",
            "The quickness of your thrust knocks the " D " back, stunned."
        },
        {
            "The " D " is disarmed by a subtle feint past his guard."
        }
    };
    return km;
}

const tofmsgs &sword_melee()
{
    static const tofmsgs sm =
    {
        {
            "Your swing misses the " D " by an inch.",
            "A mighty blow, but it misses the " D " by a mile.",
            "You charge, but the " D " jumps nimbly aside.",
            "Clang! Crash! The " D " parries.",
            "A good stroke, but it's too slow, the " D " dodges."
        },
        {
            "Your sword crashes down, knocking the " D " into dreamland.",
            "The " D " is battered into unconsciousness.",
            "A furious exchange, and the " D " is knocked out!"
        },
        {
            "It's curtains for the " D " as your sword removes his head.",
            "The fatal blow strikes the " D " square in the heart:  He dies.",
            "The " D " takes a final blow and slumps to the floor dead."
        },
        {
            "The " D " is struck on the arm, blood begins to trickle down.",
            "Your sword pinks the " D " on the wrist, but it's not serious.",
            "Your stroke lands, but it was only the flat of the blade.",
            "The blow lands, making a shallow gash in the " D "'s arm!"
        },
        {
            "The " D " receives a deep gash in his side.",
            "A savage blow on the thigh!  The " D " is stunned but can still fight!",
            "Slash!  Your blow lands!  That one hit an artery, it could be serious!"
        },
        {
            "The " D " is staggered, and drops to his knees.",
            "The " D " is momentarily disoriented and can't fight back.",
            "The force of your blow knocks the " D " back, stunned."
        },
        {
            "The " D "'s weapon is knocked to the floor, leaving him unarmed."
        }
    };
    return sm;
}

const tofmsgs &thief_melee()
{
    static const tofmsgs tm =
    {
        {
            "The thief stabs nonchalantly with his stiletto and misses.",
            "You dodge as the thief comes in low.",
            "You parry a lightning thrust, and the thief salutes you with\n"
                "a grim nod.",
            "The thief tries to sneak past your guard, but you twist away."
        },
        {
            "Shifting in the midst of a thrust, the thief knocks you unconscious\n"
                "with the haft of his stiletto.",
            "The thief knocks you out.",
        },
        {
            "Finishing you off, a lightning throw right to the heart.",
            "The stiletto severs your jugular.  It looks like the end.",
            "The thief comes in from the side, feints, and inserts the blade\n"
                "into your ribs.",
            "The thief bows formally, raises his stiletto, and with a wry grin,\n"
                "ends the battle and your life."
        },
        {
            "A quick thrust pinks your left arm, and blood starts to\ntrickle down.",
            "The thief draws blood, raking his stiletto across your arm.",
            "The stiletto flashes faster than you can follow, and blood wells\nfrom your leg.",
            "The thief slowly approaches, strikes like a snake, and leaves\nyou wounded."
        },
        {
            "The thief strikes like a snake!  The resulting wound is serious.",
            "The thief stabs a deep cut in your upper arm.",
            "The stiletto touches your forehead, and the blood obscures your\nvision.",
            "The thief strikes at your wrist, and suddenly your grip is slippery\nwith blood."
        },
        {
            "The butt of his stiletto cracks you on the skull, and you stagger\nback.",
            "You are forced back, and trip over your own feet, falling heavily\nto the floor.",
            "The thief rams the haft of his blade into your stomach, leaving\nyou out of breath.",
            "The thief attacks, and you fall back desperately."
        },
        {
            "A long, theatrical slash.  You catch it on your " W ", but the\nthief twists his knife, and the " W " goes flying.",
            "The thief neatly flips your " W " out of your hands, and it drops\nto the floor.",
            "You parry a low thrust, and your " W " slips out of your hand.",
            "Avoiding the thief's stiletto, you stumble to the floor, dropping\nyour " W "."
        },
        {
            "The thief, a man of good breeding, refrains from attacking a helpless\nopponent.",
            "The thief amuses himself by searching your pockets.",
            "The thief entertains himself by rifling your pack.",
        },
        {
            "The thief, noticing you begin to stir, reluctantly finishes you off.",
            "The thief, forgetting his essentially genteel upbringing, cuts your\nthroat.",
            "The thief, who is essentially a pragmatist, dispatches you as a\nthreat to his livelihood."
        }
    };
    return tm;
}

const tofmsgs &troll_melee()
{
    static const tofmsgs tm =
    {
        {
            "The troll swings his axe, but it misses.",
            "The troll's axe barely misses your ear.",
            "The axe sweeps past as you jump aside.",
            "The axe crashes against the rock, throwing sparks!"
        },
        {
            "The flat of the troll's axe hits you delicately on the head, knocking\nyou out."
        },
        {
            "The troll lands a killing blow.  You are dead.",
            "The troll neatly removes your head.",
            "The troll's axe stroke cleaves you from the nave to the chops.",
            "The troll's axe removes your head."
        },
        {
            "The axe gets you right in the side.  Ouch!",
            "The flat of the troll's axe skins across your forearm.",
            "The troll's swing almost knocks you over as you barely parry\nin time.",
            "The troll swings his axe, and it nicks your arm as you dodge."
        },
        {
            "The troll charges, and his axe slashes you on your " W " arm.",
            "An axe stroke makes a deep wound in your leg.",
            "The troll's axe swings down, gashing your shoulder.",
            "The troll sees a hole in your defense, and a lightning stroke\nopens a wound in your left side."
        },
        {
            "The troll hits you with a glancing blow, and you are momentarily\nstunned.",
            "The troll swings; the blade turns on your armor but crashes\nbroadside into your head.",
            "You stagger back under a hail of axe strokes.",
            "The troll's mighty blow drops you to your knees."
        },
        {
            "The axe hits your " W " and knocks it spinning.",
            "The troll swings, you parry, but the force of his blow disarms you.",
            "The axe knocks your " W " out of your hand.  It falls to the floor.",
            "Your " W " is knocked out of your hands, but you parried the blow."
        },
        {
            "The troll strikes at your unconscious form, but misses in his rage.",
            "The troll hesitates, fingering his axe.",
            "The troll scratches his head ruminatively:  Might you be magically\nprotected, he wonders? ",
            "The troll seems afraid to approach your crumpled form."
        },
        {
            "Conquering his fears, the troll puts you to death."
        }
    };
    return tm;
}

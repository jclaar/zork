#pragma once
#include <array>
#include <string>
#include <string_view>
using namespace std::string_view_literals;
constexpr std::string_view  msg_string = R"~(Muddle 105 Version/Please report strange occurrences.)~";
constexpr auto device_table  = std::to_array({ R"~(A)~"sv, R"~(AI)~"sv, R"~(D)~"sv, R"~(DM)~"sv, R"~(C)~"sv, R"~(ML)~"sv, R"~(H)~"sv, R"~(AI)~"sv, R"~(L)~"sv, R"~(AI)~"sv, R"~(M)~"sv, R"~(ML)~"sv, R"~(N)~"sv, R"~(MC)~"sv, R"~(P)~"sv, R"~(ML)~"sv, R"~(U)~"sv, R"~(MC)~"sv, R"~(Z)~"sv, R"~(ML)~"sv});
constexpr std::string_view  flushstr1 = R"~(There appears before you a threatening figure clad all over in heavy
black armor.  His legs seem like the massive trunk of the oak tree.
His broad shoulders and helmeted head loom high over your own puny
frame and you realize that his powerful arms could easily crush the
very life from your body.  There hangs from his belt a veritable
arsenal of deadly weapons: sword, mace, ball and chain, dagger,
lance, and trident. He speaks with a commanding voice:

		     "YOU SHALL NOT PASS "

As he grabs you by the neck all grows dim about you.)~";
constexpr std::string_view  flushstr2 = R"~(Suddenly, a sinister, wraithlike figure appears before you, seeming
to float in the air.  He glows with an eldritch light.  In a barely
audible voice he says, "Begone, defiler!  Your presence upsets the
very balance of the System itself!"  With a sinister chuckle, he
raises his oaken staff, taps you on the head, and fades into the
gloom.  In his place appears a tastefully lettered sign reading:

			DUNGEON CLOSED

At that instant, you disappear, and all your belongings clatter to
the ground.
)~";
constexpr std::string_view  end_herald_1 = R"~(Suddenly a sinister wraithlike figure, cloaked and hooded, appears
seeming to float in the air before you.  In a low, almost inaudible
voice he says, "I welcome you to the ranks of the chosen of Zork. You
have persisted through many trials and tests, and have overcome them
all, dispelling the darkness of ignorance and danger.  One such as
yourself is fit to join even the Implementers!"  He then raises his
oaken staff, and chuckling, drifts away like a wisp of smoke, his
laughter fading in the distance.)~";
constexpr std::string_view  end_herald_2 = R"~(
Unfortunately, as the wraith fades, in his place appears a tastefully
lettered sign reading:

	      "Soon to be Constructed on this Site
		  A Complete Modern Zork Endgame
		    Designed and Built by the
		  Frobozz Magic Dungeon Company"

)~";
constexpr std::string_view  suicidal = R"~(You clearly are a suicidal maniac.  We don't allow psychotics in the
cave, since they may harm other adventurers.  Your remains will be
installed in the Land of the Living Dead, where your fellow
adventurers may gloat over them.)~";
constexpr std::string_view  no_patch = R"~(What?  You don't trust me?  Why, only last week I patched a running
ITS and it survived for over 30 seconds.  Oh, well.)~";
constexpr std::string_view  patch = R"~(Now, let me see...
Well, we weren't quite able to restore your state.  You can't have
everything.)~";
constexpr std::string_view  death = R"~(As you take your last breath, you feel relieved of your burdens. The
feeling passes as you find yourself before the gates of Hell, where
the spirits jeer at you and deny you entry.  Your senses are
disturbed.  The objects in the dungeon appear indistinct, bleached of
color, even unreal.)~";
constexpr std::string_view  life = R"~(From the distance the sound of a lone trumpet is heard.  The room
becomes very bright and you feel disembodied.  In a moment, the
brightness fades and you find yourself rising as if from a long
sleep, deep in the woods.  In the distance you can faintly hear a
song bird and the sounds of the forest.)~";
constexpr std::string_view  losstr = R"~(I can't do everything, because I ran out of room.)~";
constexpr std::string_view  backstr = R"~(He who puts his hand to the plow and looks back is not fit for the
kingdom of winners.  In any case, "back" doesn't work.)~";
constexpr std::string_view  kitch_desc = R"~(You are in the kitchen of the white house.  A table seems to have
been used recently for the preparation of food.  A passage leads to
the west and a dark staircase can be seen leading upward.  To the
east is a small window which is )~";
constexpr std::string_view  lroom_desc1 = R"~(You are in the living room.  There is a door to the east.  To the
west is a cyclops-shaped hole in an old wooden door, above which is
some strange gothic lettering )~";
constexpr std::string_view  lroom_desc2 = R"~(You are in the living room.  There is a door to the east, a wooden
door with strange gothic lettering to the west, which appears to be
nailed shut, )~";
constexpr std::string_view  ltide_desc = R"~(It appears that the dam has been opened since the water level behind
it is low and the sluice gate has been opened.  Water is rushing
downstream through the gates.)~";
constexpr std::string_view  htide_desc = R"~(The sluice gates on the dam are closed.  Behind the dam, there can be
seen a wide lake.  A small stream is formed by the runoff from the
lake.)~";
constexpr std::string_view  reser_desc = R"~(You are on the reservoir.  Beaches can be seen north and south.
Upstream a small stream enters the reservoir through a narrow cleft
in the rocks.  The dam can be seen downstream.)~";
constexpr std::string_view  dam_desc = R"~(You are standing on the top of the Flood Control Dam #3, which was
quite a tourist attraction in times far distant.  There are paths to
the north, south, east, and down.)~";
constexpr std::string_view  cella_desc = R"~(You are in a dark and damp cellar with a narrow passageway leading
east, and a crawlway to the south.  On the west is the bottom of a
steep metal ramp which is unclimbable.)~";
constexpr std::string_view  mirr_desc = R"~(You are in a large square room with tall ceilings.  On the south wall
is an enormous mirror which fills the entire wall.  There are exits
on the other three sides of the room.)~";
constexpr std::string_view  trolldesc = R"~(A nasty-looking troll, brandishing a bloody axe, blocks all passages
out of the room.)~";
constexpr std::string_view  trollout = R"~(An unconscious troll is sprawled on the floor.  All passages out of
the room are open.)~";
constexpr std::string_view  cyclokill = R"~(The cyclops, tired of all of your games and trickery, eats you.
The cyclops says 'Mmm.  Just like mom used to make 'em.')~";
constexpr std::string_view  cyclofood = R"~(The cyclops says 'Mmm Mmm.  I love hot peppers!  But oh, could I use
a drink.  Perhaps I could drink the blood of that thing'.  From the
gleam in his eye, it could be surmised that you are 'that thing'.)~";
constexpr std::string_view  cyclolook = R"~(A cyclops, who looks prepared to eat horses (much less mere
adventurers), blocks the staircase.  From his state of health, and
the bloodstains on the walls, you gather that he is not very
friendly, though he likes people.)~";
constexpr std::string_view  cycloeye = R"~(The cyclops is standing in the corner, eyeing you closely.  I don't
think he likes you very much.  He looks extremely hungry even for a
cyclops.)~";
constexpr std::string_view  robber_c_desc = R"~(There is a suspicious-looking individual, holding a bag, leaning
against one wall.  He is armed with a vicious-looking stiletto.)~";
constexpr std::string_view  robber_u_desc = R"~(There is a suspicious-looking individual lying unconscious on the
ground.  His bag and stiletto seem to have vanished.)~";
constexpr std::string_view  resdesc = R"~(However, with the water level lowered, there is merely a wide stream
running through the center of the room.)~";
constexpr std::string_view  gladesc = R"~(This is a large room, with giant icicles hanging from the walls
and ceiling.  There are passages to the north and east.)~";
constexpr std::string_view  glacier_win = R"~(The torch hits the glacier and explodes into a great ball of flame,
devouring the glacier.  The water from the melting glacier rushes
downstream, carrying the torch with it.  In the place of the glacier,
there is a passageway leading west.)~";
constexpr auto yuks
       = std::to_array({ R"~(A valiant attempt.)~"sv, R"~(You can't be serious.)~"sv, R"~(Not a prayer.)~"sv, R"~(Not likely.)~"sv, R"~(An interesting idea...)~"sv, R"~(What a concept!)~"sv});
constexpr std::string_view  rusty_knife_str = R"~(As the knife approaches its victim, your mind is submerged by an
overmastering will.  Slowly, your hand turns, until the rusty blade
is an inch from your neck.  The knife seems to sing as it savagely
slits your throat.)~";
constexpr std::string_view  cursestr = R"~(A ghost appears in the room and is appalled at your having
desecrated the remains of a fellow adventurer.  He casts a curse
on all of your valuables and orders them banished to the Land of
the Living Dead.  The ghost leaves, muttering obscenities.)~";
constexpr std::string_view  torch_desc = R"~(This is a large room with a prominent doorway leading to a down
staircase. To the west is a narrow twisting tunnel, covered with
a thin layer of dust.  Above you is a large dome painted with scenes
depicting elfin hacking rites. Up around the edge of the dome
(20 feet up) is a wooden railing. In the center of the room there
is a white marble pedestal.)~";
constexpr std::string_view  dome_desc = R"~(You are at the periphery of a large dome, which forms the ceiling
of another room below.  Protecting you from a precipitous drop is a
wooden railing which circles the dome.)~";
constexpr std::string_view  hellgate = R"~(You are outside a large gateway, on which is inscribed 
	"Abandon every hope, all ye who enter here."  
The gate is open; through it you can see a desolation, with a pile of
mangled corpses in one corner.  Thousands of voices, lamenting some
hideous fate, can be heard.)~";
constexpr std::string_view  exor1 = R"~(The bell suddenly becomes red hot and falls to the ground. The
wraiths, as if paralyzed, stop their jeering and slowly turn to face
you.  On their ashen faces, the expression of a long-forgotten terror
takes shape.)~";
constexpr std::string_view  exor2 = R"~(The flames flicker wildly and appear to dance.  The earth beneath
your feet trembles, and your legs nearly buckle beneath you.
The spirits cower at your unearthly power.)~";
constexpr std::string_view  exor3 = R"~(Each word of the prayer reverberates through the hall in a deafening
confusion.  As the last word fades, a voice, loud and commanding,
speaks: 'Begone, fiends!'.  A heart-stopping scream fills the cavern, 
and the spirits, sensing a greater power, flee through the walls.)~";
constexpr std::string_view  exor4 = R"~(The tension of this ceremony is broken, and the wraiths, amused but
shaken at your clumsy attempt, resume their hideous jeering.)~";
constexpr std::string_view  xorcst2 = R"~(There is a clap of thunder, and a voice echoes through the
cavern: "Begone, chomper!"  Apparently, the voice thinks you
are an evil spirit, and dismisses you from the realm of the living.)~";
constexpr std::string_view  lld_desc = R"~(You have entered the Land of the Living Dead, a large desolate room.
Although it is apparently uninhabited, you can hear the sounds of
thousands of lost souls weeping and moaning.  In the east corner are
stacked the remains of dozens of previous adventurers who were less
fortunate than yourself.  To the east is an ornate passage,
apparently recently constructed.  A passage exits to the west.)~";
constexpr std::string_view  lld_desc1 = R"~(Amid the desolation, you spot what appears to be your head,
at the end of a long pole.)~";
constexpr std::string_view  bro1 = R"~(The mailing label on this glossy brochure from MIT Tech reads:

		)~";
constexpr std::string_view  bro2 = R"~(
		c/o Local Dungeon Master
		White House, GUE

From the Introduction:

The brochure describes, for the edification of the prospective
student, the stringent but wide-ranging curriculum of MIT Tech.
Required courses are offered in Ambition, Distraction, Uglification
and Derision.  The Humanities are not slighted in this institution,
as the student may register for Reeling and Writhing, Mystery
(Ancient and Modern), Seaography, and Drawling (which includes
Stretching and Fainting in Coils).  Advanced students are expected to
learn Laughing and Grief.

				William Barton Flathead, Fovnder

(The brochure continues in this vein for a few hundred more pages.))~";
constexpr auto drownings
       = std::to_array({ R"~(up to your ankles.)~"sv, R"~(up to your shin.)~"sv, R"~(up to your knees.)~"sv, R"~(up to your hips.)~"sv, R"~(up to your waist.)~"sv, R"~(up to your chest.)~"sv, R"~(up to your neck.)~"sv, R"~(over your head.)~"sv, R"~(high in your lungs.)~"sv});
constexpr auto cyclomad
       = std::to_array({ R"~(The cyclops seems somewhat agitated.)~"sv, R"~(The cyclops appears to be getting more agitated.)~"sv, R"~(The cyclops is moving about the room, looking for something.)~"sv, R"~(The cyclops was looking for salt and pepper.  I think he is gathering
condiments for his upcoming snack.)~"sv, R"~(The cyclops is moving toward you in an unfriendly manner.)~"sv, R"~(You have two choices: 1. Leave  2. Become dinner.)~"sv});
constexpr auto hellos
       = std::to_array({ R"~(Hello.)~"sv, R"~(Good day.)~"sv, R"~(Nice weather we've been having lately.)~"sv, R"~(Goodbye.)~"sv});
constexpr auto wheeeee
       = std::to_array({ R"~(Very good.  Now you can go to the second grade.)~"sv, R"~(Have you tried hopping around the dungeon, too?)~"sv, R"~(Are you enjoying yourself?)~"sv, R"~(Wheeeeeeeeee!!!!!)~"sv, R"~(Do you expect me to applaud?)~"sv});
constexpr auto jumploss
       = std::to_array({ R"~(You should have looked before you leaped.)~"sv, R"~(I'm afraid that leap was a bit much for your weak frame.)~"sv, R"~(In the movies, your life would be passing in front of your eyes.)~"sv, R"~(Geronimo.....)~"sv});
constexpr auto dummy
       = std::to_array({ R"~(Look around.)~"sv, R"~(You think it isn't?)~"sv, R"~(I think you've already done that.)~"sv});
constexpr auto offended 
   = std::to_array({ R"~(Such language in a high-class establishment like this!)~"sv, R"~(You ought to be ashamed of yourself.)~"sv, R"~(Its not so bad.  You could have been killed already.)~"sv, R"~(Tough shit, asshole.)~"sv, R"~(Oh, dear.  Such language from a supposed winning adventurer!)~"sv});
constexpr auto doormungs
   = std::to_array({ R"~(The door is invulnerable.)~"sv, R"~(You cannot damage this door.)~"sv, R"~(The door is still under warranty.)~"sv});
constexpr auto ho_hum
  = std::to_array({ R"~( does not seem to do anything.)~"sv, R"~( is not notably useful.)~"sv, R"~( isn't very interesting.)~"sv, R"~( doesn't appear worthwhile.)~"sv, R"~( has no effect.)~"sv, R"~( doesn't do anything.)~"sv});
constexpr auto bat_drops
       = std::to_array({ R"~(MINE1)~"sv, R"~(MINE2)~"sv, R"~(MINE3)~"sv, R"~(MINE4)~"sv, R"~(MINE5)~"sv, R"~(MINE6)~"sv, R"~(MINE7)~"sv, R"~(TLADD)~"sv, R"~(BLADD)~"sv});
constexpr std::string_view  machine_desc = R"~(This is a large room which seems to be air-conditioned.  In one
corner there is a machine (?) which is shaped somewhat like a clothes
dryer.  On the 'panel' there is a switch which is labelled in a
dialect of Swahili.  Fortunately, I know this dialect and the label
translates to START.  The switch does not appear to be manipulable by
any human hand (unless the fingers are about 1/16 by 1/4 inch).  On
the front of the machine is a large lid, which is )~";
constexpr auto cdigs
    = std::to_array({ R"~(You are digging into a pile of bat guano.)~"sv, R"~(You seem to be getting knee deep in guano.)~"sv, R"~(You are covered with bat turds, cretin.)~"sv});
constexpr auto bdigs
    = std::to_array({ R"~(You seem to be digging a hole here.)~"sv, R"~(The hole is getting deeper, but that's about it.)~"sv, R"~(You are surrounded by a wall of sand on all sides.)~"sv});
constexpr std::string_view  over_falls_str = R"~(I didn't think you would REALLY try to go over the falls in a
barrel. It seems that some 450 feet below, you were met by a number
of  unfriendly rocks and boulders, causing your immediate demise.  Is
this what 'over a barrel' means?)~";
constexpr std::string_view  over_falls_str1 = R"~(Unfortunately, a rubber raft doesn't provide much protection from
the unfriendly sorts of rocks and boulders one meets at the bottom of
many waterfalls.  Including this one.)~";
constexpr auto swimyuks
    = std::to_array({ R"~(I don't really see how.)~"sv, R"~(I think that swimming is best performed in water.)~"sv, R"~(Perhaps it is your head that is swimming.)~"sv});
constexpr std::string_view  grue_desc1 = R"~(The grue is a sinister, lurking presence in the dark places of the
earth.  Its favorite diet is adventurers, but its insatiable
appetite is tempered by its fear of light.  No grue has ever been
seen by the light of day, and few have survived its fearsome jaws
to tell the tale.)~";
constexpr std::string_view  grue_desc2 = R"~(There is no grue here, but I'm sure there is at least one lurking
in the darkness nearby.  I wouldn't let my light go out if I were
you!)~";
constexpr std::string_view  brick_boom = R"~(Now you've done it.  It seems that the brick has other properties
than weight, namely the ability to blow you to smithereens.)~";
constexpr std::string_view  hook_desc = R"~(There is a small hook attached to the rock here.)~";
constexpr std::string_view  greek_to_me = R"~(This book is written in a tongue with which I am unfamiliar.)~";
constexpr std::string_view  gnome_desc = R"~(A volcano gnome seems to walk straight out of the wall and says
'I have a very busy appointment schedule and little time to waste on
trespassers, but for a small fee, I'll show you the way out.'  You
notice the gnome nervously glancing at his watch.)~";
constexpr std::string_view  headstr1 = R"~(Although the implementers are dead, they foresaw that some cretin
would tamper with their remains.  Therefore, they took steps to
punish such actions.)~";
constexpr std::string_view  headstr = R"~(Unfortunately, we've run out of poles.  Therefore, in punishment for
your most grievous sin, we shall deprive you of all your valuables,
and of your life.)~";
constexpr std::string_view  cagestr = R"~(As you reach for the sphere, a steel cage falls from the ceiling
to entrap you.  To make matters worse, poisonous gas starts coming
into the room.)~";
constexpr std::string_view  robotdie = R"~(The robot is injured (being of shoddy construction) and falls to the
floor in a pile of garbage, which disintegrates before your eyes.)~";
constexpr std::string_view  vapors = R"~(Just before you pass out, you notice that the vapors from the
flask's contents are fatal.)~";
constexpr std::string_view  crushed = R"~(The room seems to have become too small to hold you.  It seems that
the walls are not as compressible as your body, which is somewhat
demolished.)~";
constexpr std::string_view  icemung = R"~(The door to the room seems to be blocked by sticky orange rubble
from an explosion.  Probably some careless adventurer was playing
with blasting cakes.)~";
constexpr std::string_view  iceblast = R"~(You have been blasted to smithereens (wherever they are).)~";
constexpr std::string_view  cmach_desc = R"~(This is a large room full of assorted heavy machinery.  The room
smells of burned resistors. The room is noisy from the whirring
sounds of the machines. Along one wall of the room are three buttons
which are, respectively, round, triangular, and square.  Naturally,
above these buttons are instructions written in EBCDIC.  A large sign
in English above all the buttons says
		'DANGER -- HIGH VOLTAGE '.
There are exits to the west and the south.)~";
constexpr std::string_view  spindizzy = R"~(According to Prof. TAA of MIT Tech, the rapidly changing magnetic
fields in the room are so intense as to cause you to be electrocuted. 
I really don't know, but in any event, something just killed you.)~";
constexpr std::string_view  spinrobot = R"~(According to Prof. TAA of MIT Tech, the rapidly changing magnetic
fields in the room are so intense as to fry all the delicate innards
of the robot.  I really don't know, but in any event, smoke is coming
out of its ears and it has stopped moving.)~";
constexpr std::string_view  robot_crush = R"~(As the robot reaches for the sphere, a steel cage falls from the
ceiling.  The robot attempts to fend it off, but is trapped below it.
Alas, the robot short-circuits in his vain attempt to escape, and
crushes the sphere beneath him as he falls to the floor.)~";
constexpr std::string_view  poison = R"~(Time passes...and you die from some obscure poisoning.)~";
constexpr std::string_view  alarm_voice = R"~(A metallic voice says 'Hello, Intruder!  Your unauthorized presence
in the vault area of the Bank of Zork has set off all sorts of nasty
surprises, several of which seem to have been fatal.  This message
brought to you by the Frobozz Magic Alarm Company.)~";
constexpr std::string_view  teller_desc = R"~(You are in a small square room, which was used by a bank officer
whose job it was to retrieve safety deposit boxes for the customer.
On the north side of the room is a sign which reads  'Viewing Room'.
On the )~";
constexpr std::string_view  zgnome_desc = R"~(An epicene gnome of Zurich wearing a three-piece suit and carrying a
safety-deposit box materializes in the room.  'You seem to have
forgotten to deposit your valuables,' he says, tapping the lid of the
box impatiently.  'We don't usually allow customers to use the boxes
here, but we can make this ONE exception, I suppose...'  He looks
askance at you over his wire-rimmed bifocals.)~";
constexpr std::string_view  zgnome_pop = R"~('I wouldn't put THAT in a safety deposit box,' remarks the gnome with
disdain, tossing it over his shoulder, where it disappears with an
understated 'pop'.)~";
constexpr std::string_view  zgnome_pop_1 = R"~('Surely you jest,' he says.  He tosses the brick over his shoulder,
and disappears with an understated 'pop'.)~";
constexpr std::string_view  zgnome_bye = R"~(The gnome looks impatient:  'I may have another customer waiting;
you'll just have to fend for yourself, I'm afraid.  He disappears,
leaving you alone in the bare room.)~";
constexpr std::string_view  tree_desc = R"~(You are about 10 feet above the ground nestled among some large
branches.  The nearest branch above you is above your reach.)~";
constexpr std::string_view  opera = R"~(The canary chirps, slightly off-key, an aria from a forgotten opera.
From out of the greenery flies a lovely songbird.  It perches on a
limb just over your head and opens its beak to sing.  As it does so
a beautiful brass bauble drops from its mouth, bounces off the top of
your head, and lands glimmering in the grass.  As the canary winds
down, the songbird flies away.)~";
constexpr std::string_view  complex_desc = R"~(....
The architecture of this region is getting complex, so that further
descriptions will be diagrams of the immediate vicinity in a 3x3
grid. The walls here are rock, but of two different types - sandstone
and marble. The following notations will be used:
			..  = current position (middle of grid)
		        MM  = marble wall
		        SS  = sandstone wall
		        ??  = unknown (blocked by walls))~";
constexpr std::string_view  gigo = R"~(The item vanishes into the slot.  A moment later, a previously 
unseen sign flashes 'Garbage In, Garbage Out' and spews
the )~";
constexpr std::string_view  confiscate = R"~(The card slides easily into the slot and vanishes and the metal door
slides open revealing a passageway to the west.  A moment later, a
previously unseen sign flashes: 
     'Unauthorized/Illegal Use of Pass Card -- Card Confiscated')~";
constexpr std::string_view  pal_room = R"~(This is a small and rather dreary room, which is eerily illuminated
by a red glow emanating from a crack in one of the walls.  The light 
appears to focus on a dusty wooden table in the center of the room.)~";
constexpr std::string_view  pal_door = R"~( side of the room is a massive wooden door, near
the top of which, in the center, is a window barred with iron.
A formidable bolt lock is set within the door frame.  A keyhole)~";
constexpr std::string_view  slide_desc = R"~(This is a small chamber, which appears to have been part of a
coal mine. On the south wall of the chamber the letters "Granite
Wall" are etched in the rock. To the east is a long passage and
there is a steep metal slide twisting downward. To the north is
a small opening.)~";
constexpr std::string_view  slippery = R"~(As you descend, you realize that the rope is slippery from the grime
of the coal chute and that your grasp will not last long.)~";
constexpr std::string_view  tomb_desc1 = R"~(This is the Tomb of the Unknown Implementer.
A hollow voice says:  "That's not a bug, it's a feature!"
In the north wall of the room is the Crypt of the Implementers.  It
is made of the finest marble, and apparently large enough for four
headless corpses.  The crypt is )~";
constexpr std::string_view  tomb_desc2 = R"~( Above the entrance is the
cryptic inscription:

		     "Feel Free."
)~";
constexpr std::string_view  crypt_desc = R"~(Though large and esthetically pleasing the marble crypt is empty; the
sarcophagi, bodies, and rich treasure to be expected in a tomb of
this magnificence are missing. Inscribed on one wall is the motto of
the implementers, "Feel Free".  There is a door leading out of the
crypt to the south.  The door is )~";
constexpr std::string_view  pass_word_inst = R"~(Suddenly, as you wait in the dark, you begin to feel somewhat
disoriented.  The feeling passes, but something seems different.
As you regain your composure, the cloaked figure appears before you,
and says, "You are now ready to face the ultimate challenge of
Zork.  Should you wish to do this somewhat more quickly in the
future, you will be given a magic phrase which will at any time
transport you by magic to this point.  To select the phrase, say
	INCANT "<word>"
and you will be told your own magic phrase to use by saying
	INCANT "... <phrase> ..."
Good luck, and choose wisely!"
)~";
constexpr std::string_view  miropen = R"~(The mirror is mounted on a panel which has been opened outward.)~";
constexpr std::string_view  panopen = R"~(The panel has been opened outward.)~";
constexpr std::string_view  hallway = R"~(This is a part of the long hallway.  The east and west walls are
dressed stone.  In the center of the hall is a shallow stone channel.
In the center of the room the channel widens into a large hole around
which is engraved a compass rose.)~";
constexpr std::string_view  guardkill = R"~(The Guardians awake, and in perfect unison, utterly destroy you with
their stone bludgeons.  Satisfied, they resume their posts.)~";
constexpr std::string_view  guardkill1 = R"~(Suddenly the Guardians realize someone is trying to sneak by them in
the structure.  They awake, and in perfect unison, hammer the box and
its contents (including you) to pulp.  They then resume their posts,
satisfied.)~";
constexpr std::string_view  guard_attack = R"~(Attacking the Guardians is about as useful as attacking a stone wall.
Unfortunately for you, your futile blow attracts their attention, and
they manage to dispatch you effortlessly.)~";
constexpr std::string_view  mirbreak = R"~(The mirror breaks, revealing a wooden panel behind it.)~";
constexpr std::string_view  mirbroke = R"~(The mirror has already been broken.)~";
constexpr std::string_view  panelbreak = R"~(To break the panel you would have to break the mirror first.)~";
constexpr std::string_view  panelbroke = R"~(The panel is not that easily destroyed.)~";
constexpr std::string_view  guardstr = R"~(, identical stone statues face each other from
pedestals on opposite sides of the corridor.  The statues represent
Guardians of Zork, a military order of ancient lineage.  They are
portrayed as heavily armored warriors standing at ease, hands clasped
around formidable bludgeons.)~";
constexpr std::string_view  inside_mirror_1 = R"~(You are inside a rectangular box of wood whose structure is rather
complicated.  Four sides and the roof are filled in, and the floor is
open.
     As you face the side opposite the entrance, two short sides of
carved and polished wood are to your left and right.  The left panel
is mahogany, the right pine.  The wall you face is red on its left
half and black on its right.  On the entrance side, the wall is white
opposite the red part of the wall it faces, and yellow opposite the
black section.  The painted walls are at least twice the length of
the unpainted ones.  The ceiling is painted blue.
     In the floor is a stone channel about six inches wide and a foot
deep.  The channel is oriented in a north-south direction.  In the
exact center of the room the channel widens into a circular
depression perhaps two feet wide.  Incised in the stone around this
area is a compass rose.
     Running from one short wall to the other at about waist height
is a wooden bar, carefully carved and drilled.  This bar is pierced
in two places.  The first hole is in the center of the bar (and thus
the center of the room).  The second is at the left end of the room
(as you face opposite the entrance).  Through each hole runs a wooden
pole.
     The pole at the left end of the bar is short, extending about a foot
above the bar, and ends in a hand grip.  The pole )~";
constexpr std::string_view  mirror_pole_desc = R"~(     The long pole at the center of the bar extends from the ceiling
through the bar to the circular area in the stone channel.  This
bottom end of the pole has a T-bar a bit less than two feet long
attached to it, and on the T-bar is carved an arrow.  The arrow and
T-bar are pointing )~";
constexpr auto longdirs
       = std::to_array({ R"~(north)~"sv, R"~(northeast)~"sv, R"~(east)~"sv, R"~(southeast)~"sv, R"~(south)~"sv, R"~(southwest)~"sv, R"~(west)~"sv, R"~(northwest)~"sv});
constexpr auto nums  = std::to_array({ R"~(one)~"sv, R"~(two)~"sv, R"~(three)~"sv, R"~(four)~"sv, R"~(five)~"sv, R"~(six)~"sv, R"~(seven)~"sv, R"~(eight)~"sv});
constexpr std::string_view  master_attack = R"~(The dungeon master is taken momentarily by surprise.  He dodges your
blow, and then, with a disappointed expression on his face, he raises
his staff, and traces a complicated pattern in the air.  As it
completes you crumble into dust.)~";
constexpr std::string_view  inq_lose = R"~(" The dungeon master,
obviously disappointed in your lack of knowledge, shakes his head and
mumbles "I guess they'll let anyone in the Dungeon these days".  With
that, he departs.)~";
constexpr std::string_view  quiz_win = R"~(The dungeon master, obviously pleased, says "You are indeed a
master of lore. I am proud to be at your service."  The massive
wooden door swings open, and the master motions for you to enter.)~";
constexpr std::string_view  quiz_rules = R"~(The knock reverberates along the hall.  For a time it seems there
will be no answer.  Then you hear someone unlatching the small wooden
panel.  Through the bars of the great door, the wrinkled face of an
old man appears.  He gazes down at you and intones as follows:

     "I am the Master of the Dungeon, whose task it is to insure
that none but the most scholarly and masterful adventurers are
admitted into the secret realms of the Dungeon.  To ascertain whether
you meet the stringent requirements laid down by the Great
Implementers, I will ask three questions which should be easy for one
of your reputed excellence to answer.  You have undoubtedly
discovered their answers during your travels through the Dungeon. 
Should you answer each of these questions correctly within five
attempts, then I am obliged to acknowledge your skill and daring and
admit you to these regions.
     "All answers should be in the form 'ANSWER "<answer>"'")~";
constexpr std::string_view  ewc_desc = R"~(This is a large east-west corridor which opens onto a northern
parapet at its center.  You can see flames and smoke as you peer
towards the parapet.  The corridor turns south at its east and west
ends, and due south is a massive wooden door.  In the door is a small
window barred with iron.  The door is )~";
constexpr std::string_view  parapet_desc = R"~(You are standing behind a stone retaining wall which rims a large
parapet overlooking a fiery pit.  It is difficult to see through the
smoke and flame which fills the pit, but it seems to be more or less
bottomless.  It also extends upward out of sight.  The pit itself is
of roughly dressed stone and circular in shape.  It is about two
hundred feet in diameter.  The flames generate considerable heat, so
it is rather uncomfortable standing here.
There is an object here which looks like a sundial.  On it are an
indicator arrow and (in the center) a large button.  On the face of
the dial are numbers 'one' through 'eight'.  The indicator points to
the number ')~";
constexpr std::string_view  win_totally = R"~(     As you gleefully examine your new-found riches, the Dungeon
Master himself materializes beside you, and says, "Now that you have
solved all the mysteries of the Dungeon, it is time for you to assume
your rightly-earned place in the scheme of things.  Long have I
waited for one capable of releasing me from my burden!"  He taps you
lightly on the head with his staff, mumbling a few well-chosen spells,
and you feel yourself changing, growing older and more stooped.  For
a moment there are two identical mages staring at each other among
the treasure, then you watch as your counterpart dissolves into a
mist and disappears, a sardonic grin on his face.
)~";
constexpr std::string_view  deadend = R"~(Dead End)~";
constexpr std::string_view  sdeadend = R"~(You have come to a dead end in the maze.)~";
constexpr std::string_view  stfore = R"~(This is a forest, with trees in all directions around you.)~";
constexpr std::string_view  forest = R"~(Forest)~";
constexpr std::string_view  fordes = R"~(This is a dimly lit forest, with large trees all around.  To the
east, there appears to be sunlight.)~";
constexpr std::string_view  fortree = R"~(This is a dimly lit forest, with large trees all around.  One
particularly large tree with some low branches stands here.)~";
constexpr std::string_view  tchomp = R"~(The troll fends you off with a menacing gesture.)~";
constexpr std::string_view  mazedesc = R"~(This is part of a maze of twisty little passages, all alike.)~";
constexpr std::string_view  smazedesc = R"~(Maze)~";
constexpr std::string_view  nofit = R"~(You cannot fit through this passage with that load.)~";
constexpr std::string_view  mindesc = R"~(This is a non-descript part of a coal mine.)~";
constexpr std::string_view  smindesc = R"~(Coal mine)~";
constexpr std::string_view  riverdesc = R"~(Frigid River)~";
constexpr std::string_view  narrow = R"~(The path is too narrow.)~";
constexpr std::string_view  view_room = R"~(This is a room used by holders of safety deposit boxes to view
their contents.  On the north side of the room is a sign which says 
	
   REMAIN HERE WHILE THE BANK OFFICER RETRIEVES YOUR DEPOSIT BOX
    WHEN YOU ARE FINISHED, LEAVE THE BOX, AND EXIT TO THE SOUTH  
     AN ADVANCED PROTECTIVE DEVICE PREVENTS ALL CUSTOMERS FROM
      REMOVING ANY SAFETY DEPOSIT BOX FROM THIS VIEWING AREA!
               Thank You for banking at the Zork!
)~";
constexpr std::string_view  bkalarm = R"~(An alarm rings briefly and an invisible force prevents your leaving.)~";
constexpr std::string_view  timber_untied = R"~(There is a wooden timber on the ground here.)~";
constexpr std::string_view  timber_tied = R"~(The coil of rope is tied to the wooden timber.)~";
constexpr std::string_view  coffin_untied = R"~(The solid-gold coffin used for the burial of Ramses II is here.)~";
constexpr std::string_view  coffin_tied = R"~(The coil of rope is tied to Ramses II's gold coffin.)~";

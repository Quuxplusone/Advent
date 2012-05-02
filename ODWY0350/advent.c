#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;
enum { true=1, false=0 };

#ifdef Z_MACHINE
int ran(int range) = "\t@random r0 -> r0;\n";
#define assert(x)
#define tolower(ch) ((ch) | (('A' <= (ch) && (ch) <= 'Z') ? 0x20 : 0x00))
#define isspace(ch) ((ch)==' ' || (ch)=='\n')
#else
int ran(int range) { return rand() % range; }
#include <assert.h>
#include <ctype.h>
#endif

bool pct(int percent) { return (ran(100) < percent); }
bool streq(const char *a, const char *b) { return !strncmp(a, b, 5); }

/*========== Forward declarations. ========================================
 */
void dwarves_upset(void);
void give_up(void);
void quit(void);

/*========== The vocabulary. ==============================================
 * This section corresponds to sections 4--17 in Knuth.
 */

typedef enum {
    WordType_None, WordType_Motion, WordType_Object,
    WordType_Action, WordType_Message
} WordType;

struct HashEntry {
    char text[6];
    WordType word_type;
    char meaning;
};

#define HASH_PRIME 1009
struct HashEntry hash_table[HASH_PRIME];

void new_word(WordType t, const char *w, int m)
{
    int h = 0;
    const char *p;
    for (p = w; *p != '\0'; ++p) {
	h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].word_type != WordType_None)
	h = (h+1) % HASH_PRIME;
    strcpy(hash_table[h].text, w);
    hash_table[h].word_type = t;
    hash_table[h].meaning = m;
}

void new_motion_word(const char *w, int m) { new_word(WordType_Motion, w, m); }
void new_object_word(const char *w, int m) { new_word(WordType_Object, w, m); }
void new_action_word(const char *w, int m) { new_word(WordType_Action, w, m); }
void new_message_word(const char *w, int m) { new_word(WordType_Message, w, m); }

int lookup(const char *w)
{
    int h = 0;
    const char *p;
    for (p = w; *p != '\0' && p != w+5; ++p) {
	h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].word_type != WordType_None) {
	if (streq(w, hash_table[h].text)) return h;
	h = (h+1) % HASH_PRIME;
    }
    return -1;
}

typedef enum {
    N,S,E,W,NE,SE,NW,SW,U,D,L,R,IN,OUT,FORWARD,BACK,
    OVER,ACROSS,UPSTREAM,DOWNSTREAM,
    ENTER,CRAWL,JUMP,CLIMB,LOOK,CROSS,
    ROAD,HILL,WOODS,VALLEY,HOUSE,GULLY,STREAM,DEPRESSION,ENTRANCE,CAVE,
    ROCK,SLAB,BED,PASSAGE,CAVERN,CANYON,AWKWARD,SECRET,BEDQUILT,RESERVOIR,
    GIANT,ORIENTAL,SHELL,BARREN,BROKEN,DEBRIS,VIEW,FORK,
    PIT,SLIT,CRACK,DOME,HOLE,WALL,HALL,ROOM,FLOOR,
    STAIRS,STEPS,COBBLES,SURFACE,DARK,LOW,OUTDOORS,
    Y2,XYZZY,PLUGH,PLOVER,OFFICE,NOWHERE
} MotionWord;

typedef enum {
    NOTHING, KEYS, LAMP, GRATE, GRATE_, CAGE, ROD, ROD2, TREADS, TREADS_,
    BIRD, DOOR, PILLOW, SNAKE, CRYSTAL, CRYSTAL_, TABLET, CLAM, OYSTER,
    MAG, DWARF, KNIFE, FOOD, BOTTLE, WATER, OIL,
    MIRROR, MIRROR_, PLANT, PLANT2, PLANT2_, STALACTITE, SHADOW, SHADOW_,
    AXE, ART, PIRATE, DRAGON, DRAGON_, BRIDGE, BRIDGE_, TROLL, TROLL_, TROLL2, TROLL2_,
    BEAR, MESSAGE, GEYSER, PONY, BATTERIES, MOSS,
    MIN_TREASURE,
    GOLD=MIN_TREASURE, DIAMONDS, SILVER, JEWELS, COINS, CHEST, EGGS, TRIDENT, VASE,
    EMERALD, PYRAMID, PEARL, RUG, RUG_, SPICES, CHAIN,
    MAX_OBJ=CHAIN
} ObjectWord;

#define IS_TREASURE(t) ((t) >= MIN_TREASURE)

typedef enum {
    ABSTAIN, TAKE, DROP, OPEN, CLOSE, ON, OFF, WAVE, CALM, GO, RELAX,
    POUR, EAT, DRINK, RUB, TOSS, WAKE, FEED, FILL, BREAK, BLAST, KILL,
    SAY, READ, FEEFIE, BRIEF, FIND, INVENTORY, SCORE, QUIT
} ActionWord;

const char *default_msg[30];  /* messages for untoward actions */
const char *message[13];  /* messages tied to certain vocabulary words */
const char ok[] = "OK.";  /* Woods' Fortran version didn't include the period, by the way. */
const char pitch_dark_msg[] = "It is now pitch dark.  If you proceed you will most likely fall into a pit.";

void build_vocabulary(void)
{
    new_motion_word("north", N); new_motion_word("n", N);
    new_motion_word("south", S); new_motion_word("s", S);
    new_motion_word("east", E); new_motion_word("e", E);
    new_motion_word("west", W); new_motion_word("w", W);
    new_motion_word("ne", NE);
    new_motion_word("se", SE);
    new_motion_word("nw", NW);
    new_motion_word("sw", SW);
    new_motion_word("upwar", U); new_motion_word("up", U);
    new_motion_word("u", U); new_motion_word("above", U);
    new_motion_word("downw", D); new_motion_word("down", D);
    new_motion_word("d", D); new_motion_word("desce", D);
    new_motion_word("left", L);
    new_motion_word("right", R);
    new_motion_word("inwar", IN); new_motion_word("insid", IN);
    new_motion_word("in", IN);
    new_motion_word("out", OUT); new_motion_word("outsi", OUT);
    new_motion_word("exit", OUT); new_motion_word("leave", OUT);
    new_motion_word("forwa", FORWARD); new_motion_word("conti", FORWARD);
    new_motion_word("onwar", FORWARD);
    new_motion_word("back", BACK); new_motion_word("retur", BACK);
    new_motion_word("retre", BACK);
    new_motion_word("over", OVER);
    new_motion_word("acros", ACROSS);
    new_motion_word("upstr", UPSTREAM);
    new_motion_word("downs", DOWNSTREAM);
    new_motion_word("enter", ENTER);
    new_motion_word("crawl", CRAWL);
    new_motion_word("jump", JUMP);
    new_motion_word("climb", CLIMB);
    new_motion_word("look", LOOK); new_motion_word("exami", LOOK);
    new_motion_word("touch", LOOK); new_motion_word("descr", LOOK);
    new_motion_word("cross", CROSS);
    new_motion_word("road", ROAD);
    new_motion_word("hill", HILL);
    new_motion_word("fores", WOODS);
    new_motion_word("valle", VALLEY);
    new_motion_word("build", HOUSE); new_motion_word("house", HOUSE);
    new_motion_word("gully", GULLY);
    new_motion_word("strea", STREAM);
    new_motion_word("depre", DEPRESSION);
    new_motion_word("entra", ENTRANCE);
    new_motion_word("cave", CAVE);
    new_motion_word("rock", ROCK);
    new_motion_word("slab", SLAB); new_motion_word("slabr", SLAB);
    new_motion_word("bed", BED);
    new_motion_word("passa", PASSAGE); new_motion_word("tunne", PASSAGE);
    new_motion_word("caver", CAVERN);
    new_motion_word("canyo", CANYON);
    new_motion_word("awkwa", AWKWARD);
    new_motion_word("secre", SECRET);
    new_motion_word("bedqu", BEDQUILT);
    new_motion_word("reser", RESERVOIR);
    new_motion_word("giant", GIANT);
    new_motion_word("orien", ORIENTAL);
    new_motion_word("shell", SHELL);
    new_motion_word("barre", BARREN);
    new_motion_word("broke", BROKEN);
    new_motion_word("debri", DEBRIS);
    new_motion_word("view", VIEW);
    new_motion_word("fork", FORK);
    new_motion_word("pit", PIT);
    new_motion_word("slit", SLIT);
    new_motion_word("crack", CRACK);
    new_motion_word("dome", DOME);
    new_motion_word("hole", HOLE);
    new_motion_word("wall", WALL);
    new_motion_word("hall", HALL);
    new_motion_word("room", ROOM);
    new_motion_word("floor", FLOOR);
    new_motion_word("stair", STAIRS);
    new_motion_word("steps", STEPS);
    new_motion_word("cobbl", COBBLES);
    new_motion_word("surfa", SURFACE);
    new_motion_word("dark", DARK);
    new_motion_word("low", LOW);
    new_motion_word("outdo", OUTDOORS);
    new_motion_word("y2", Y2);
    new_motion_word("xyzzy", XYZZY);
    new_motion_word("plugh", PLUGH);
    new_motion_word("plove", PLOVER);
    new_motion_word("main", OFFICE); new_motion_word("offic", OFFICE);
    new_motion_word("null", NOWHERE); new_motion_word("nowhe", NOWHERE);
    
    new_object_word("key", KEYS); new_object_word("keys", KEYS);
    new_object_word("lamp", LAMP); new_object_word("lante", LAMP);
    new_object_word("headl", LAMP);
    new_object_word("grate", GRATE);
    new_object_word("cage", CAGE);
    new_object_word("rod", ROD);
    new_object_word("bird", BIRD);
    new_object_word("door", DOOR);
    new_object_word("pillo", PILLOW);
    new_object_word("snake", SNAKE);
    new_object_word("fissu", CRYSTAL);
    new_object_word("table", TABLET);
    new_object_word("clam", CLAM);
    new_object_word("oyste", OYSTER);
    new_object_word("magaz", MAG); new_object_word("issue", MAG);
    new_object_word("spelu", MAG); new_object_word("\"spel", MAG);
    new_object_word("dwarf", DWARF); new_object_word("dwarv", DWARF);
    new_object_word("knife", KNIFE); new_object_word("knive", KNIFE);
    new_object_word("food", FOOD); new_object_word("ratio", FOOD);
    new_object_word("bottl", BOTTLE); new_object_word("jar", BOTTLE);
    new_object_word("water", WATER); new_object_word("h2o", WATER);
    new_object_word("oil", OIL);
    new_object_word("mirro", MIRROR);
    new_object_word("plant", PLANT); new_object_word("beans", PLANT);
    new_object_word("stala", STALACTITE);
    new_object_word("shado", SHADOW); new_object_word("figur", SHADOW);
    new_object_word("axe", AXE);
    new_object_word("drawi", ART);
    new_object_word("pirat", PIRATE);
    new_object_word("drago", DRAGON);
    new_object_word("chasm", BRIDGE);
    new_object_word("troll", TROLL);
    new_object_word("bear", BEAR);
    new_object_word("messa", MESSAGE);
    new_object_word("volca", GEYSER); new_object_word("geyse", GEYSER);
    new_object_word("vendi", PONY); new_object_word("machi", PONY);
    new_object_word("batte", BATTERIES);
    new_object_word("moss", MOSS); new_object_word("carpe", MOSS); 
    new_object_word("gold", GOLD); new_object_word("nugge", GOLD); 
    new_object_word("diamo", DIAMONDS); 
    new_object_word("silve", SILVER); new_object_word("bars", SILVER);
    new_object_word("jewel", JEWELS);
    new_object_word("coins", COINS);
    new_object_word("chest", CHEST); new_object_word("box", CHEST);
    new_object_word("treas", CHEST);
    new_object_word("eggs", EGGS); new_object_word("egg", EGGS);
    new_object_word("nest", EGGS);
    new_object_word("tride", TRIDENT);
    new_object_word("ming", VASE); new_object_word("vase", VASE);
    new_object_word("shard", VASE);
    new_object_word("emera", EMERALD);
    new_object_word("plati", PYRAMID); new_object_word("pyram", PYRAMID);
    new_object_word("pearl", PEARL);
    new_object_word("persi", RUG); new_object_word("rug", RUG);
    new_object_word("spice", SPICES);
    new_object_word("chain", CHAIN);
    
    new_action_word("take", TAKE); new_action_word("carry", TAKE);
    new_action_word("keep", TAKE); new_action_word("catch", TAKE);
    new_action_word("captu", TAKE); new_action_word("steal", TAKE);
    new_action_word("get", TAKE); new_action_word("tote", TAKE);
    default_msg[TAKE] = "You are already carrying it!";
    new_action_word("drop", DROP); new_action_word("relea", DROP);
    new_action_word("free", DROP); new_action_word("disca", DROP);
    new_action_word("dump", DROP);
    default_msg[DROP] = "You aren't carrying it!";
    new_action_word("open", OPEN); new_action_word("unloc", OPEN);
    default_msg[OPEN] = "I don't know how to lock or unlock such a thing.";
    new_action_word("close", CLOSE); new_action_word("lock", CLOSE);
    default_msg[CLOSE] = default_msg[OPEN];
    new_action_word("light", ON); new_action_word("on", ON);
    default_msg[ON] = "You have no source of light.";
    new_action_word("extin", OFF); new_action_word("off", OFF);
    default_msg[OFF] = default_msg[ON];

    new_action_word("wave", WAVE); new_action_word("shake", WAVE);
    new_action_word("swing", WAVE);
    default_msg[WAVE] = "Nothing happens.";
    new_action_word("calm", CALM); new_action_word("placa", CALM);
    new_action_word("tame", CALM);
    default_msg[CALM] = "I'm game.  Would you care to explain how?";
    new_action_word("walk", GO); new_action_word("run", GO);
    new_action_word("trave", GO); new_action_word("go", GO);
    new_action_word("proce", GO); new_action_word("explo", GO);
    new_action_word("goto", GO); new_action_word("follo", GO);
    new_action_word("turn", GO);
    default_msg[GO] = "Where?";
    new_action_word("nothi", RELAX);
    default_msg[RELAX] = ok;
    new_action_word("pour", POUR);
    default_msg[POUR] = default_msg[DROP];
    new_action_word("eat", EAT); new_action_word("devou", EAT);
    default_msg[EAT] = "Don't be ridiculous!";
    new_action_word("drink", DRINK);
    default_msg[DRINK] =
	"You have taken a drink from the stream.  The water tastes strongly of\n"
	"minerals, but is not unpleasant.  It is extremely cold.";
    new_action_word("rub", RUB);
    default_msg[RUB] =
	"Rubbing the electric lamp is not particularly rewarding. Anyway,\n"
	"nothing exciting happens.";
    new_action_word("throw", TOSS); new_action_word("toss", TOSS);
    default_msg[TOSS] = "Peculiar.  Nothing unexpected happens.";
    new_action_word("wake", WAKE); new_action_word("distu", WAKE);
    default_msg[WAKE] = default_msg[EAT];
    new_action_word("feed", FEED);
    default_msg[FEED] = "There is nothing here to eat.";
    new_action_word("fill", FILL);
    default_msg[FILL] = "You can't fill that.";
    new_action_word("break", BREAK); new_action_word("smash", BREAK);
    new_action_word("shatt", BREAK);
    default_msg[BREAK] = "It is beyond your power to do that.";
    new_action_word("blast", BLAST); new_action_word("deton", BLAST);
    new_action_word("ignit", BLAST); new_action_word("blowu", BLAST);
    default_msg[BLAST] = "Blasting requires dynamite.";
    new_action_word("attac", KILL); new_action_word("kill", KILL);
    new_action_word("fight", KILL); new_action_word("hit", KILL);
    new_action_word("strik", KILL); new_action_word("slay", KILL);
    default_msg[KILL] = default_msg[EAT];
    new_action_word("say", SAY);
    new_action_word("chant", SAY);
    new_action_word("sing", SAY);
    new_action_word("utter", SAY);
    new_action_word("mumbl", SAY);
    /* default_msg[SAY] = NULL; */    
    new_action_word("read", READ); new_action_word("perus", READ);
    default_msg[READ] = "I'm afraid I don't understand.";
    new_action_word("fee", FEEFIE); new_action_word("fie", FEEFIE);
    new_action_word("foe", FEEFIE); new_action_word("foo", FEEFIE);
    new_action_word("fum", FEEFIE);
    default_msg[FEEFIE] = "I don't know how.";
    new_action_word("brief", BRIEF); 
    default_msg[BRIEF] = "On what?";  /* [ajo] Pretty sure this message is unreachable! */
    new_action_word("find", FIND); new_action_word("where", FIND);
    default_msg[FIND] =
	"I can only tell you what you see as you move about and manipulate\n"
	"things.  I cannot tell you where remote things are.";
    new_action_word("inven", INVENTORY);
    default_msg[INVENTORY] = default_msg[FIND];
    new_action_word("score", SCORE);
    default_msg[SCORE] = "Eh?";
    new_action_word("quit", QUIT);
    default_msg[QUIT] = default_msg[SCORE];

    /* Finally, our vocabulary is rounded out by words like HELP, which
     * trigger the printing of fixed messages. */
    new_message_word("abra", 0);
    new_message_word("abrac", 0);
    new_message_word("opens", 0);
    new_message_word("sesam", 0);
    new_message_word("shaza", 0);
    new_message_word("hocus", 0);
    new_message_word("pocus", 0);
    message[0] = "Good try, but that is an old worn-out magic word.";
    
    new_message_word("help", 1);
    new_message_word("?", 1);
    message[1] =
	"I know of places, actions, and things. Most of my vocabulary\n"
	"describes places and is used to move you there. To move, try words\n"
	"like forest, building, downstream, enter, east, west, north, south,\n"
	"up, or down. I know about a few special objects, like a black rod\n"
	"hidden in the cave. These objects can be manipulated using some of\n"
	"the action words that I know. Usually you will need to give both the\n"
	"object and action words (in either order), but sometimes I can infer\n"
	"the object from the verb alone. Some objects also imply verbs; in\n"
	"particular, \"inventory\" implies \"take inventory\", which causes me to\n"
	"give you a list of what you're carrying. The objects have side\n"
	"effects; for instance, the rod scares the bird. Usually people having\n"
	"trouble moving just need to try a few more words. Usually people\n"
	"trying unsuccessfully to manipulate an object are attempting something\n"
	"beyond their (or my!) capabilities and should try a completely\n"
	"different tack. To speed the game you can sometimes move long\n"
	"distances with a single word. For example, \"building\" usually gets\n"
	"you to the building from anywhere above ground except when lost in the\n"
	"forest. Also, note that cave passages turn a lot, and that leaving a\n"
	"room to the north does not guarantee entering the next from the south.\n"
	"Good luck!";
    
    new_message_word("tree", 2);
    new_message_word("trees", 2);
    message[2] =
	"The trees of the forest are large hardwood oak and maple, with an\n"
	"occasional grove of pine or spruce.  There is quite a bit of under-\n"
        "growth, largely birch and ash saplings plus nondescript bushes of\n"
	"various sorts.  This time of year visibility is quite restricted by\n"
        "all the leaves, but travel is quite easy if you detour around the\n"
	"spruce and berry bushes.";

    new_message_word("dig", 3);
    new_message_word("excav", 3);
    message[3] =
	"Digging without a shovel is quite impractical.  Even with a shovel\n"
	"progress is unlikely.";

    new_message_word("lost", 4);
    message[4] = "I'm as confused as you are.";
    message[5] =
	"There is a loud explosion and you are suddenly splashed across the\n"
	"walls of the room.";
    message[6] =
	"There is a loud explosion and a twenty-foot hole appears in the far\n"
	"wall, burying the snakes in the rubble. A river of molten lava pours\n"
	"in through the hole, destroying everything in its path, including you!";

    new_message_word("mist", 7);
    message[7] =
	"Mist is a white vapor, usually water, seen from time to time in\n"
	"caverns.  It can be found anywhere but is frequently a sign of a deep\n"
	"pit leading down to water.";

    new_message_word("fuck", 8);
    message[8] = "Watch it!";
    message[9] =
	"There is a loud explosion and a twenty-foot hole appears in the far\n"
	"wall, burying the dwarves in the rubble.  You march through the hole\n"
	"and find yourself in the main office, where a cheering band of\n"
	"friendly elves carry the conquering adventurer off into the sunset.";

    new_message_word("stop", 10);
    message[10] = "I don't know the word \"stop\".  Use \"quit\" if you want to give up.";
    new_message_word("info", 11);
    new_message_word("infor", 11);
    message[11] =
        "If you want to end your adventure early, say \"quit\". To get full\n"
        "credit for a treasure, you must have left it safely in the building,\n"
	"though you get partial credit just for locating it. You lose points\n"
	"for getting killed, or for quitting, though the former costs you more.\n"
	"There are also points based on how much (if any) of the cave you've\n"
	"managed to explore; in particular, there is a large bonus just for\n"
	"getting in (to distinguish the beginners from the rest of the pack),\n"
	"and there are other ways to determine whether you've been through some\n"
	"of the more harrowing sections. If you think you've found all the\n"
	"treasures, just keep exploring for a while. If nothing interesting\n"
	"happens, you haven't found them all yet. If something interesting\n"
	"DOES happen, it means you're getting a bonus and have an opportunity\n"
	"to garner many more points in the master's section.\n"
	"I may occasionally offer hints if you seem to be having trouble.\n"
	"If I do, I'll warn you in advance how much it will affect your score\n"
	"to accept the hints. Finally, to save paper, you may specify \"brief\",\n"
	"which tells me never to repeat the full description of a place\n"
	"unless you explicitly ask me to.";
    
    new_message_word("swim", 12);
    message[12] = "I don't know how.";
}

/*========== Locations. ===================================================
 * This section corresponds to sections 18--62 in Knuth.
 */

#define FORCED_MOVE(loc) ((loc) >= R_CRACK)

typedef enum {
    R_INHAND = -1, R_LIMBO = 0,
    R_ROAD, R_HILL, R_HOUSE, R_VALLEY, R_FOREST, R_WOODS, R_SLIT, R_OUTSIDE,
    R_INSIDE, MIN_IN_CAVE = R_INSIDE,
    R_COBBLES, R_DEBRIS, R_AWK, R_BIRD, R_SPIT,
    R_EMIST, MIN_LOWER_LOC = R_EMIST,
    R_NUGGET, R_EFISS, R_WFISS, R_WMIST,
    R_LIKE1, R_LIKE2, R_LIKE3, R_LIKE4, R_LIKE5, R_LIKE6, R_LIKE7,
    R_LIKE8, R_LIKE9, R_LIKE10, R_LIKE11, R_LIKE12, R_LIKE13, R_LIKE14,
    R_BRINK, R_ELONG, R_WLONG,
    R_DIFF0, R_DIFF1, R_DIFF2, R_DIFF3, R_DIFF4, R_DIFF5,
    R_DIFF6, R_DIFF7, R_DIFF8, R_DIFF9, R_DIFF10,
    R_PONY, R_CROSS, R_HMK, R_WEST, R_SOUTH, R_NS, R_Y2, R_JUMBLE, R_WINDOE,
    R_DIRTY, R_CLEAN, R_WET, R_DUSTY, R_COMPLEX,
    R_SHELL, R_ARCH, R_RAGGED, R_SAC, R_ANTE, R_WITT,
    R_BEDQUILT, R_CHEESE, R_SOFT,
    R_E2PIT, R_W2PIT, R_EPIT, R_WPIT,
    R_NARROW, R_GIANT, R_BLOCK, R_IMMENSE,
    R_FALLS, R_STEEP, R_ABOVEP, R_SJUNC,
    R_TITE, R_LOW, R_CRAWL, R_WINDOW,
    R_ORIENTAL, R_MISTY, R_ALCOVE, R_PROOM, R_DROOM,
    R_SLAB, R_ABOVER, R_MIRROR, R_RES,
    R_SCAN1, R_SCAN2, R_SCAN3, R_SECRET,
    R_WIDE, R_TIGHT, R_TALL, R_BOULDERS,
    R_SCORR, R_SWSIDE,
    R_DEAD0, R_DEAD1, R_PIRATES_NEST, R_DEAD3, R_DEAD4, R_DEAD5,
    R_DEAD6, R_DEAD7, R_DEAD8, R_DEAD9, R_DEAD10, R_DEAD11,
    R_NESIDE, R_CORR, R_FORK, R_WARM, R_VIEW, R_CHAMBER,
    R_LIME, R_FBARR, R_BARR,
    R_NEEND, R_SWEND,
    R_CRACK, MIN_FORCED_LOC = R_CRACK,
    R_NECK, R_LOSE, R_CANT, R_CLIMB, R_CHECK, R_SNAKED,
    R_THRU, R_DUCK, R_SEWER, R_UPNOUT,
    R_DIDIT, MAX_LOC = R_DIDIT,
    R_PPASS, R_PDROP,
    R_TROLL, MAX_SPEC = R_TROLL
} Location;

typedef struct {
    MotionWord mot;
    int cond;
    Location dest;
} Instruction;

enum flagsBits {
    F_LIGHTED    = 0x001,
    F_OIL        = 0x002,
    F_LIQUID     = 0x004,
    F_CAVE_HINT  = 0x008,
    F_BIRD_HINT  = 0x010,
    F_SNAKE_HINT = 0x020,
    F_TWIST_HINT = 0x040,
    F_DARK_HINT  = 0x080,
    F_WITT_HINT  = 0x100
};

Instruction travels[740];
Instruction *start[MAX_LOC+2];
const char *long_desc[MAX_LOC+1];
const char *short_desc[MAX_LOC+1];
const char *remarks[15];
unsigned int flags[MAX_LOC+1];
int visits[MAX_LOC+1];

void make_loc(Instruction *q, Location x, const char *l, const char *s, unsigned int f)
{
    long_desc[x] = l;
    short_desc[x] = s;
    flags[x] = f;
    start[x] = q;
}

void make_inst(Instruction *q, MotionWord m, int c, Location d)
{
    q->mot = m;
    q->cond = c;
    q->dest = d;
}

#define make_ins(m, d) make_inst(q++, m, 0, d)
#define make_cond_ins(m, c, d) make_inst(q++, m, c, d)
#define ditto(m) make_inst(q, m, q[-1].cond, q[-1].dest); ++q;

void build_travel_table(void)
{
    Instruction *q = travels;
    static const char all_alike[] = "You are in a maze of twisty little passages, all alike.";
    static const char dead_end[] = "Dead end.";
    make_loc(q, R_ROAD,
	     "You are standing at the end of a road before a small brick building.\n"
	     "Around you is a forest.  A small stream flows out of the building and\n"
	     "down a gully.",
	     "You're at end of road again.", F_LIGHTED | F_LIQUID);
    make_ins(W, R_HILL); ditto(U); ditto(ROAD);
    make_ins(E, R_HOUSE); ditto(IN); ditto(HOUSE); ditto(ENTER);
    make_ins(S, R_VALLEY); ditto(D); ditto(GULLY); ditto(STREAM); ditto(DOWNSTREAM);
    make_ins(N, R_FOREST); ditto(WOODS);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_HILL,
	     "You have walked up a hill, still in the forest.  The road slopes back\n"
	     "down the other side of the hill.  There is a building in the distance.",
	     "You're at hill in road.", F_LIGHTED);
    make_ins(ROAD, R_ROAD); ditto(HOUSE); ditto(FORWARD); ditto(E); ditto(D);
    make_ins(WOODS, R_FOREST); ditto(N); ditto(S);
    make_loc(q, R_HOUSE,
	     "You are inside a building, a well house for a large spring.",
	     "You're inside building.", F_LIGHTED | F_LIQUID);
    make_ins(ENTER, R_ROAD); ditto(OUT); ditto(OUTDOORS); ditto(W);
    make_ins(XYZZY, R_DEBRIS);
    make_ins(PLUGH, R_Y2);
    make_ins(DOWNSTREAM, R_SEWER); ditto(STREAM);
    make_loc(q, R_VALLEY,
	     "You are in a valley in the forest beside a stream tumbling along a\n"
	     "rocky bed.",
	     "You're in valley.", F_LIGHTED | F_LIQUID);
    make_ins(UPSTREAM, R_ROAD); ditto(HOUSE); ditto(N);
    make_ins(WOODS, R_FOREST); ditto(E); ditto(W); ditto(U);
    make_ins(DOWNSTREAM, R_SLIT); ditto(S); ditto(D);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_FOREST,
	     "You are in open forest, with a deep valley to one side.",
	     "You're in forest.", F_LIGHTED);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(D);
    make_inst(q++, WOODS, 50, R_FOREST); ditto(FORWARD); ditto(N);
    make_ins(WOODS, R_WOODS);
    make_ins(W, R_FOREST); ditto(S);
    make_loc(q, R_WOODS,
	     "You are in open forest near both a valley and a road.",
	     "You're in forest.", F_LIGHTED);
    make_ins(ROAD, R_ROAD); ditto(N);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(W); ditto(D);
    make_ins(WOODS, R_FOREST); ditto(S);
    make_loc(q, R_SLIT,
	     "At your feet all the water of the stream splashes into a 2-inch slit\n"
	     "in the rock.  Downstream the streambed is bare rock.",
	     "You're at slit in streambed.", F_LIGHTED | F_LIQUID);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_VALLEY); ditto(N);
    make_ins(WOODS, R_FOREST); ditto(E); ditto(W);
    make_ins(DOWNSTREAM, R_OUTSIDE); ditto(ROCK); ditto(BED); ditto(S);
    remarks[0] = "You don't fit through a two-inch slit!";
    make_inst(q++, SLIT, 0, MAX_SPEC+0); ditto(STREAM); ditto(D);
    make_loc(q, R_OUTSIDE,
	     "You are in a 20-foot depression floored with bare dirt.  Set into the\n"
	     "dirt is a strong steel grate mounted in concrete.  A dry streambed\n"
	     "leads into the depression.",
	     "You're outside grate.", F_LIGHTED | F_CAVE_HINT);
    make_ins(WOODS, R_FOREST); ditto(E); ditto(W); ditto(S);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_SLIT); ditto(GULLY); ditto(N);
    make_cond_ins(ENTER, 300+GRATE, R_INSIDE); ditto(ENTER); ditto(IN); ditto(D);
    remarks[1] = "You can't go through a locked steel grate!";
    make_inst(q++, ENTER, 0, MAX_SPEC+1);
    make_loc(q, R_INSIDE,
	     "You are in a small chamber beneath a 3x3 steel grate to the surface.\n"
	     "A low crawl over cobbles leads inwards to the west.",
	     "You're below the grate.", F_LIGHTED);
    make_cond_ins(OUT, 300+GRATE, R_OUTSIDE); ditto(OUT); ditto(U);
    make_cond_ins(OUT, 0, MAX_SPEC+1);
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(IN); ditto(W);
    make_ins(PIT, R_SPIT);
    make_ins(DEBRIS, R_DEBRIS);
    make_loc(q, R_COBBLES,
	     "You are crawling over cobbles in a low passage.  There is a dim light\n"
	     "at the east end of the passage.",
	     "You're in cobble crawl.", F_LIGHTED);
    make_ins(OUT, R_INSIDE); ditto(SURFACE); ditto(NOWHERE); ditto(E);
    make_ins(IN, R_DEBRIS); ditto(DARK); ditto(W); ditto(DEBRIS);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_DEBRIS,
	     "You are in a debris room filled with stuff washed in from the surface.\n"
	     "A low wide passage with cobbles becomes plugged with mud and debris\n"
	     "here, but an awkward canyon leads upward and west.  A note on the wall\n"
	     "says \"MAGIC WORD XYZZY\".",
	     "You're in debris room.", 0);
    make_cond_ins(DEPRESSION, 300+GRATE, R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(PASSAGE); ditto(LOW); ditto(E);
    make_ins(CANYON, R_AWK); ditto(IN); ditto(U); ditto(W);
    make_ins(XYZZY, R_HOUSE);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_AWK,
	     "You are in an awkward sloping east/west canyon.",
	     NULL, 0);
    make_cond_ins(DEPRESSION, 300+GRATE, R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(D, R_DEBRIS); ditto(E); ditto(DEBRIS);
    make_ins(IN, R_BIRD); ditto(U); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_BIRD,
	     "You are in a splendid chamber thirty feet high. The walls are frozen\n"
	     "rivers of orange stone. An awkward canyon and a good passage exit\n"
	     "from east and west sides of the chamber.",
	     "You're in bird chamber.", F_BIRD_HINT);
    make_cond_ins(DEPRESSION, 300+GRATE, R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(DEBRIS, R_DEBRIS);
    make_ins(CANYON, R_AWK); ditto(E);
    make_ins(PASSAGE, R_SPIT); ditto(PIT); ditto(W);
    make_loc(q, R_SPIT,
	     "At your feet is a small pit breathing traces of white mist. An east\n"
	     "passage ends here except for a small crack leading on.",
	     "You're at top of small pit.", 0);
    make_cond_ins(DEPRESSION, 300+GRATE, R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(DEBRIS, R_DEBRIS);
    make_ins(PASSAGE, R_BIRD); ditto(E);
    make_inst(q++, D, 100+GOLD, R_NECK); ditto(PIT); ditto(STEPS);
    /* good thing you weren't loaded down with GOLD */
    make_ins(D, R_EMIST);
    make_ins(CRACK, R_CRACK); ditto(W);
    make_loc(q, R_EMIST,
	     "You are at one end of a vast hall stretching forward out of sight to\n"
	     "the west.  There are openings to either side.  Nearby, a wide stone\n"
	     "staircase leads downward.  The hall is filled with wisps of white mist\n"
	     "swaying to and fro almost as if alive.  A cold wind blows up the\n"
	     "staircase.  There is a passage at the top of a dome behind you.",
	     "You're in Hall of Mists.", 0);
    make_ins(L, R_NUGGET); ditto(S);
    make_ins(FORWARD, R_EFISS); ditto(HALL); ditto(W);
    make_ins(STAIRS, R_HMK); ditto(D); ditto(N);
    make_cond_ins(U, 100+GOLD, R_CANT); ditto(PIT); ditto(STEPS); ditto(DOME); ditto(PASSAGE); ditto(E);
    make_ins(U, R_SPIT);
    make_ins(Y2, R_JUMBLE);
    make_loc(q, R_NUGGET,
	     "This is a low room with a crude note on the wall. The note says,\n"
	     "\"You won't get it up the steps\".",
	     "You're in nugget of gold room.", 0);
    make_ins(HALL, R_EMIST); ditto(OUT); ditto(N);
    make_loc(q, R_EFISS,
	     "You are on the east bank of a fissure slicing clear across the hall.\n"
	     "The mist is quite thick here, and the fissure is too wide to jump.",
	     "You're on east bank of fissure.", 0);
    make_ins(HALL, R_EMIST); ditto(E);
    remarks[2] = "I respectfully suggest you go across the bridge instead of jumping.";
    make_cond_ins(JUMP, 300+CRYSTAL, MAX_SPEC+2);
    make_cond_ins(FORWARD, 300+CRYSTAL+100, R_LOSE);
    remarks[3] = "There is no way across the fissure.";
    make_cond_ins(OVER, 300+CRYSTAL+100, MAX_SPEC+3); ditto(ACROSS); ditto(W); ditto(CROSS);
    make_ins(OVER, R_WFISS);
    make_loc(q, R_WFISS,
	     "You are on the west side of the fissure in the Hall of Mists.",
	     NULL, 0);
    make_cond_ins(JUMP, 300+CRYSTAL, MAX_SPEC+2);
    make_cond_ins(FORWARD, 300+CRYSTAL+100, R_LOSE);
    make_cond_ins(OVER, 300+CRYSTAL+100, MAX_SPEC+3); ditto(ACROSS); ditto(E); ditto(CROSS);
    make_ins(OVER, R_EFISS);
    make_ins(N, R_THRU);
    make_ins(W, R_WMIST);

    make_loc(q, R_WMIST,
	     "You are at the west end of the Hall of Mists. A low wide crawl\n"
	     "continues west and another goes north. To the south is a little\n"
	     "passage 6 feet off the floor.",
	     "You're at west end of Hall of Mists.", 0);
    make_ins(S, R_LIKE1); ditto(U); ditto(PASSAGE); ditto(CLIMB);
    make_ins(E, R_WFISS);
    make_ins(N, R_DUCK);
    make_ins(W, R_ELONG); ditto(CRAWL);

    make_loc(q, R_LIKE1, all_alike, NULL, F_TWIST_HINT);
    make_ins(U, R_WMIST);
    make_ins(N, R_LIKE1);
    make_ins(E, R_LIKE2);
    make_ins(S, R_LIKE4);
    make_ins(W, R_LIKE11);
    make_loc(q, R_LIKE2, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE1);
    make_ins(S, R_LIKE3);
    make_ins(E, R_LIKE4);
    make_loc(q, R_LIKE3, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_LIKE2);
    make_ins(D, R_DEAD5);
    make_ins(S, R_LIKE6);
    make_ins(N, R_DEAD9);
    make_loc(q, R_LIKE4, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE1);
    make_ins(N, R_LIKE2);
    make_ins(E, R_DEAD3);
    make_ins(S, R_DEAD4);
    make_ins(U, R_LIKE14); ditto(D);
    make_loc(q, R_LIKE5, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_LIKE6);
    make_ins(W, R_LIKE7);
    make_loc(q, R_LIKE6, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_LIKE3);
    make_ins(W, R_LIKE5);
    make_ins(D, R_LIKE7);
    make_ins(S, R_LIKE8);
    make_loc(q, R_LIKE7, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE5);
    make_ins(U, R_LIKE6);
    make_ins(E, R_LIKE8);
    make_ins(S, R_LIKE9);
    make_loc(q, R_LIKE8, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE6);
    make_ins(E, R_LIKE7);
    make_ins(S, R_LIKE8);
    make_ins(U, R_LIKE9);
    make_ins(N, R_LIKE10);
    make_ins(D, R_DEAD11);
    make_loc(q, R_LIKE9, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE7);
    make_ins(N, R_LIKE8);
    make_ins(S, R_DEAD6);
    make_loc(q, R_LIKE10, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE8);
    make_ins(N, R_LIKE10);
    make_ins(D, R_DEAD7);
    make_ins(E, R_BRINK);
    make_loc(q, R_LIKE11, all_alike, NULL, F_TWIST_HINT);
    make_ins(N, R_LIKE1);
    make_ins(W, R_LIKE11); ditto(S);
    make_ins(E, R_DEAD1);
    make_loc(q, R_LIKE12, all_alike, NULL, F_TWIST_HINT);
    make_ins(S, R_BRINK);
    make_ins(E, R_LIKE13);
    make_ins(W, R_DEAD10);
    make_loc(q, R_LIKE13 , all_alike, NULL, F_TWIST_HINT);
    make_ins(N, R_BRINK);
    make_ins(W, R_LIKE12);
    make_ins(NW, R_PIRATES_NEST);  /* NW: a dirty trick! */
    make_loc(q, R_LIKE14, all_alike, NULL, F_TWIST_HINT);
    make_ins(U, R_LIKE4); ditto(D);
    make_loc(q, R_BRINK,
	     "You are on the brink of a thirty-foot pit with a massive orange column\n"
	     "down one wall.  You could climb down here but you could not get back\n"
	     "up.  The maze continues at this level.",
	     "You're at brink of pit.", 0);
    make_ins(D, R_BIRD); ditto(CLIMB);
    make_ins(W, R_LIKE10);
    make_ins(S, R_DEAD8);
    make_ins(N, R_LIKE12);
    make_ins(E, R_LIKE13);
    make_loc(q, R_ELONG,
	     "You are at the east end of a very long hall apparently without side\n"
	     "chambers.  To the east a low wide crawl slants up.  To the north a\n"
	     "round two-foot hole slants down.",
	     "You're at east end of long hall.", 0);
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL);
    make_ins(W, R_WLONG);
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE);
    make_loc(q, R_WLONG,
             "You are at the west end of a very long featureless hall.  The hall\n"
	     "joins up with a narrow north/south passage.",
	     "You're at west end of long hall.", 0);
    make_ins(E, R_ELONG);
    make_ins(N, R_CROSS);
    make_cond_ins(S, 100, R_DIFF0);  /* 100: Dwarves Not Permitted. */

#define twist(name,n,s,e,w,ne,se,nw,sw,u,d,m)  \
	make_loc(q, name, m, NULL, 0); \
	make_ins(N,n); make_ins(S,s); make_ins(E,e); make_ins(W,w); \
	make_ins(NE,ne); make_ins(SE,se); make_ins(NW,nw); make_ins(SW,sw); \
	make_ins(U,u); make_ins(D,d);

    twist(R_DIFF0,R_DIFF9,R_DIFF1,R_DIFF7,R_DIFF8,R_DIFF3,R_DIFF4,R_DIFF6,R_DIFF2,R_DIFF5,R_WLONG,
        "You are in a maze of twisty little passages, all different.");
    twist(R_DIFF1,R_DIFF8,R_DIFF9,R_DIFF10,R_DIFF0,R_DIFF5,R_DIFF2,R_DIFF3,R_DIFF4,R_DIFF6,R_DIFF7,
	"You are in a maze of twisting little passages, all different.");
    twist(R_DIFF2,R_DIFF3,R_DIFF4,R_DIFF8,R_DIFF5,R_DIFF7,R_DIFF10,R_DIFF0,R_DIFF6,R_DIFF1,R_DIFF9,
	"You are in a little maze of twisty passages, all different.");
    twist(R_DIFF3,R_DIFF7,R_DIFF10,R_DIFF6,R_DIFF2,R_DIFF4,R_DIFF9,R_DIFF8,R_DIFF5,R_DIFF0,R_DIFF1,
	"You are in a twisting maze of little passages, all different.");
    twist(R_DIFF4,R_DIFF1,R_DIFF7,R_DIFF5,R_DIFF9,R_DIFF0,R_DIFF3,R_DIFF2,R_DIFF10,R_DIFF8,R_DIFF6,
	"You are in a twisting little maze of passages, all different.");
    twist(R_DIFF5,R_DIFF0,R_DIFF3,R_DIFF4,R_DIFF6,R_DIFF8,R_DIFF1,R_DIFF9,R_DIFF7,R_DIFF10,R_DIFF2,
	"You are in a twisty little maze of passages, all different.");
    twist(R_DIFF6,R_DIFF10,R_DIFF5,R_DIFF0,R_DIFF1,R_DIFF9,R_DIFF8,R_DIFF7,R_DIFF3,R_DIFF2,R_DIFF4,
	"You are in a twisty maze of little passages, all different.");
    twist(R_DIFF7,R_DIFF6,R_DIFF2,R_DIFF9,R_DIFF10,R_DIFF1,R_DIFF0,R_DIFF5,R_DIFF8,R_DIFF4,R_DIFF3,
	"You are in a little twisty maze of passages, all different.");
    twist(R_DIFF8,R_DIFF5,R_DIFF6,R_DIFF1,R_DIFF4,R_DIFF2,R_DIFF7,R_DIFF10,R_DIFF9,R_DIFF3,R_DIFF0,
	"You are in a maze of little twisting passages, all different.");
    twist(R_DIFF9,R_DIFF4,R_DIFF8,R_DIFF2,R_DIFF3,R_DIFF10,R_DIFF6,R_DIFF1,R_DIFF0,R_DIFF7,R_DIFF5,
	"You are in a maze of little twisty passages, all different.");
    twist(R_DIFF10,R_DIFF2,R_PONY,R_DIFF3,R_DIFF7,R_DIFF6,R_DIFF5,R_DIFF4,R_DIFF1,R_DIFF9,R_DIFF8,
	"You are in a little maze of twisting passages, all different.");

#undef twist

    make_loc(q, R_PONY, dead_end, NULL, 0);
    make_ins(N, R_DIFF10); ditto(OUT);
    make_loc(q, R_CROSS,
	     "You are at a crossover of a high N/S passage and a low E/W one.",
	     NULL, 0);
    make_ins(W, R_ELONG);
    make_ins(N, R_DEAD0);
    make_ins(E, R_WEST);
    make_ins(S, R_WLONG);
    make_loc(q, R_HMK,
        "You are in the Hall of the Mountain King, with passages off in all\n"
	"directions.",
	"You're in Hall of Mt King.", F_SNAKE_HINT);
    make_ins(STAIRS, R_EMIST); ditto(U); ditto(E);
    make_cond_ins(N, 300+SNAKE, R_NS); ditto(L);
    make_cond_ins(S, 300+SNAKE, R_SOUTH); ditto(R);
    make_cond_ins(W, 300+SNAKE, R_WEST); ditto(FORWARD);
    make_ins(N, R_SNAKED);
    make_cond_ins(SW, 35, R_SECRET);
    make_cond_ins(SW, 200+SNAKE, R_SNAKED);
    make_ins(SECRET, R_SECRET);
    make_loc(q, R_WEST,
	"You are in the west side chamber of the Hall of the Mountain King.\n"
	"A passage continues west and up here.",
	"You're in west side chamber.", 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(E);
    make_ins(W, R_CROSS); ditto(U);
    make_loc(q, R_SOUTH,
	"You are in the south side chamber.",
	NULL, 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(N);
    make_loc(q, R_NS,
	     "You are in a low N/S passage at a hole in the floor.  The hole goes\n"
	     "down to an E/W passage.",
	     "You're in N/S passage.", 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(S);
    make_ins(N, R_Y2); ditto(Y2);
    make_ins(D, R_DIRTY); ditto(HOLE);
    make_loc(q, R_Y2,
	     "You are in a large room, with a passage to the south, a passage to the\n"
	     "west, and a wall of broken rock to the east.  There is a large \"Y2\" on\n"
	     "a rock in the room's center.",
	     "You're at \"Y2\".", 0);
    make_ins(PLUGH, R_HOUSE);
    make_ins(S, R_NS);
    make_ins(E, R_JUMBLE); ditto(WALL); ditto(BROKEN);
    make_ins(W, R_WINDOE);
    make_cond_ins(PLOVER, 100+EMERALD, R_PDROP);
    make_ins(PLOVER, R_PROOM);
    make_loc(q, R_JUMBLE,
	     "You are in a jumble of rock, with cracks everywhere.",
	     NULL, 0);
    make_ins(D, R_Y2); ditto(Y2);
    make_ins(U, R_EMIST);
    make_loc(q, R_WINDOE,
	     "You're at a low window overlooking a huge pit, which extends up out of\n"
	     "sight.  A floor is indistinctly visible over 50 feet below.  Traces of\n"
	     "white mist cover the floor of the pit, becoming thicker to the right.\n"
	     "Marks in the dust around the window would seem to indicate that\n"
	     "someone has been here recently.  Directly across the pit from you and\n"
	     "25 feet away there is a similar window looking into a lighted room.\n"
	     "A shadowy figure can be seen there peering back at you.",
	     "You're at window on pit.", 0);
    make_ins(E, R_Y2); ditto(Y2);
    make_ins(JUMP, R_NECK);
    make_loc(q, R_DIRTY,
	     "You are in a dirty broken passage.  To the east is a crawl.  To the\n"
	     "west is a large passage.  Above you is a hole to another passage.",
	     "You're in dirty passage.", 0);
    make_ins(E, R_CLEAN); ditto(CRAWL);
    make_ins(U, R_NS); ditto(HOLE);
    make_ins(W, R_DUSTY);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_loc(q, R_CLEAN,
	     "You are on the brink of a small clean climbable pit.  A crawl leads\n"
	     "west.",
	     "You're by a clean pit.", 0);
    make_ins(W, R_DIRTY); ditto(CRAWL);
    make_ins(D, R_WET); ditto(PIT); ditto(CLIMB);
    make_loc(q, R_WET,
	     "You are in the bottom of a small pit with a little stream, which\n"
	     "enters and exits through tiny slits.",
	     "You're in pit by stream.", F_LIQUID);
    make_ins(CLIMB, R_CLEAN); ditto(U); ditto(OUT);
    make_ins(SLIT, MAX_SPEC+0); ditto(STREAM); ditto(D); ditto(UPSTREAM); ditto(DOWNSTREAM);
    make_loc(q, R_DUSTY,
	     "You are in a large room full of dusty rocks.  There is a big hole in\n"
	     "the floor.  There are cracks everywhere, and a passage leading east.",
	     "You're in dusty rock room.", 0);
    make_ins(E, R_DIRTY); ditto(PASSAGE);
    make_ins(D, R_COMPLEX); ditto(HOLE); ditto(FLOOR);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_loc(q, R_COMPLEX,
	     "You are at a complex junction.  A low hands-and-knees passage from the\n"
	     "north joins a higher crawl from the east to make a walking passage\n"
	     "going west.  There is also a large room above.  The air is damp here.",
	     "You're at complex junction.", 0);
    make_ins(U, R_DUSTY); ditto(CLIMB); ditto(ROOM);
    make_ins(W, R_BEDQUILT); ditto(BEDQUILT);
    make_ins(N, R_SHELL); ditto(SHELL);
    make_ins(E, R_ANTE);
    make_loc(q, R_SHELL,
	     "You're in a large room carved out of sedimentary rock.  The floor\n"
	     "and walls are littered with bits of shells embedded in the stone.\n"
	     "A shallow passage proceeds downward, and a somewhat steeper one\n"
	     "leads up.  A low hands-and-knees passage enters from the south.",
	     "You're in Shell Room.", 0);
    make_ins(U, R_ARCH); ditto(HALL);
    make_ins(D, R_RAGGED);
    remarks[4] = "You can't fit this five-foot clam through that little passage!";
    remarks[5] = "You can't fit this five-foot oyster through that little passage!";
    make_cond_ins(S, 100+CLAM, MAX_SPEC+4);
    make_cond_ins(S, 100+OYSTER, MAX_SPEC+5);
    make_ins(S, R_COMPLEX);
    make_loc(q, R_ARCH,
	     "You are in an arched hall.  A coral passage once continued up and east\n"
	     "from here, but is now blocked by debris.  The air smells of sea water.",
	     "You're in arched hall.", 0);
    make_ins(D, R_SHELL); ditto(SHELL); ditto(OUT);
    make_loc(q, R_RAGGED,
	     "You are in a long sloping corridor with ragged sharp walls.",
	     NULL, 0);
    make_ins(U, R_SHELL); ditto(SHELL);
    make_ins(D, R_SAC);
    make_loc(q, R_SAC,
	     "You are in a cul-de-sac about eight feet across.",
	     NULL, 0);
    make_ins(U, R_RAGGED); ditto(OUT);
    make_ins(SHELL, R_SHELL);
    make_loc(q, R_ANTE,
	     "You are in an anteroom leading to a large passage to the east.  Small\n"
	     "passages go west and up.  The remnants of recent digging are evident.\n"
	     "A sign in midair here says \"CAVE UNDER CONSTRUCTION BEYOND THIS POINT.\n"
	     "PROCEED AT OWN RISK.  [WITT CONSTRUCTION COMPANY]\"",
	     "You're in anteroom.", 0);
    make_ins(U, R_COMPLEX);
    make_ins(W, R_BEDQUILT);
    make_ins(E, R_WITT);
    make_loc(q, R_WITT,
	     "You are at Witt's End.  Passages lead off in \"all\" directions.",
	     "You're at Witt's End.", F_WITT_HINT);
    remarks[6] = "You have crawled around in some little holes and wound up back in the\n"
		 "main passage.";
    make_cond_ins(E, 95, MAX_SPEC+6); ditto(N); ditto(S);
    ditto(NE); ditto(SE); ditto(SW); ditto(NW); ditto(U); ditto(D);
    make_ins(E, R_ANTE);
    remarks[7] = "You have crawled around in some little holes and found your way\n"
	         "blocked by a recent cave-in.  You are now back in the main passage.";
    make_ins(W, MAX_SPEC+7);

    make_loc(q, R_BEDQUILT,
	     "You are in Bedquilt, a long east/west passage with holes everywhere.\n"
	     "To explore at random select north, south, up, or down.",
	     "You're in Bedquilt.", 0);
    make_ins(E, R_COMPLEX);
    make_ins(W, R_CHEESE);
    make_cond_ins(S, 80, MAX_SPEC+6);
    make_ins(SLAB, R_SLAB);
    make_cond_ins(U, 80, MAX_SPEC+6);
    make_cond_ins(U, 50, R_ABOVEP);
    make_ins(U, R_DUSTY);
    make_cond_ins(N, 60, MAX_SPEC+6);
    make_cond_ins(N, 75, R_LOW);
    make_ins(N, R_SJUNC);
    make_cond_ins(D, 80, MAX_SPEC+6);
    make_ins(D, R_ANTE);

    make_loc(q, R_CHEESE,
	     "You are in a room whose walls resemble Swiss cheese.  Obvious passages\n"
	     "go west, east, NE, and NW.  Part of the room is occupied by a large\n"
	     "bedrock block.",
	     "You're in Swiss cheese room.", 0);
    make_ins(NE, R_BEDQUILT);
    make_ins(W, R_E2PIT);
    make_cond_ins(S, 80, MAX_SPEC+6);
    make_ins(CANYON, R_TALL);
    make_ins(E, R_SOFT);
    make_cond_ins(NW, 50, MAX_SPEC+6);
    make_ins(ORIENTAL, R_ORIENTAL);
    make_loc(q, R_SOFT,
	     "You are in the Soft Room.  The walls are covered with heavy curtains,\n"
	     "the floor with a thick pile carpet.  Moss covers the ceiling.",
	     "You're in Soft Room.", 0);
    make_ins(W, R_CHEESE); ditto(OUT);
    make_loc(q, R_E2PIT,
	     "You are at the east end of the Twopit Room.  The floor here is\n"
	     "littered with thin rock slabs, which make it easy to descend the pits.\n"
	     "There is a path here bypassing the pits to connect passages from east\n"
	     "and west.  There are holes all over, but the only big one is on the\n"
	     "wall directly over the west pit where you can't get to it.",
	     "You're at east end of Twopit Room.", 0);
    make_ins(E, R_CHEESE);
    make_ins(W, R_W2PIT); ditto(ACROSS);
    make_ins(D, R_EPIT); ditto(PIT);
    make_loc(q, R_W2PIT,
	     "You are at the west end of the Twopit Room.  There is a large hole in\n"
	     "the wall above the pit at this end of the room.",
	     "You're at west end of Twopit Room.", 0);
    make_ins(E, R_E2PIT); ditto(ACROSS);
    make_ins(W, R_SLAB); ditto(SLAB);
    make_ins(D, R_WPIT); ditto(PIT);
    remarks[8] = "It is too far up for you to reach.";
    make_ins(HOLE, MAX_SPEC+8);
    make_loc(q, R_EPIT,
	     "You are at the bottom of the eastern pit in the Twopit Room.  There is\n"
	     "a small pool of oil in one corner of the pit.",
	     "You're in east pit.", F_LIQUID | F_OIL);
    make_ins(U, R_E2PIT); ditto(OUT);
    make_loc(q, R_WPIT,
	     "You are at the bottom of the western pit in the Twopit Room.  There is\n"
	     "a large hole in the wall about 25 feet above you.",
	     "You're in west pit.", 0);
    make_ins(U, R_W2PIT); ditto(OUT);
    make_cond_ins(CLIMB, 300+PLANT+400, R_CHECK);
    make_ins(CLIMB, R_CLIMB);
    make_loc(q, R_NARROW,
	     "You are in a long, narrow corridor stretching out of sight to the\n"
	     "west.  At the eastern end is a hole through which you can see a\n"
	     "profusion of leaves.",
	     "You're in narrow corridor.", 0);
    make_ins(D, R_WPIT); ditto(CLIMB); ditto(E);
    make_ins(JUMP, R_NECK);
    make_ins(W, R_GIANT); ditto(GIANT);
    make_loc(q, R_GIANT,
	     "You are in the Giant Room.  The ceiling here is too high up for your\n"
	     "lamp to show it.  Cavernous passages lead east, north, and south.  On\n"
	     "the west wall is scrawled the inscription, \"FEE FIE FOE FOO\" [sic].",
	     "You're in Giant Room.", 0);
    make_ins(S, R_NARROW);
    make_ins(E, R_BLOCK);
    make_ins(N, R_IMMENSE);
    make_loc(q, R_BLOCK,
	     "The passage here is blocked by a recent cave-in.",
	     NULL, 0);
    make_ins(S, R_GIANT); ditto(GIANT); ditto(OUT);
    make_loc(q, R_IMMENSE,
	     "You are at one end of an immense north/south passage.",
	     NULL, 0);
    make_ins(S, R_GIANT); ditto(GIANT); ditto(PASSAGE);
    make_cond_ins(N, 300+DOOR, R_FALLS); ditto(ENTER); ditto(CAVERN);
    remarks[9] = "The door is extremely rusty and refuses to open.";
    make_ins(N, MAX_SPEC+9);
    make_loc(q, R_FALLS,
	     "You are in a magnificent cavern with a rushing stream, which cascades\n"
	     "over a sparkling waterfall into a roaring whirlpool that disappears\n"
	     "through a hole in the floor.  Passages exit to the south and west.",
	     "You're in cavern with waterfall.", F_LIQUID);
    make_ins(S, R_IMMENSE); ditto(OUT);
    make_ins(GIANT, R_GIANT);
    make_ins(W, R_STEEP);
    make_loc(q, R_STEEP,
	     "You are at the top of a steep incline above a large room.  You could\n"
	     "climb down here, but you would not be able to climb up.  There is a\n"
	     "passage leading back to the north.",
	     "You're at steep incline above large room.", 0);
    make_ins(N, R_FALLS); ditto(CAVERN); ditto(PASSAGE);
    make_ins(D, R_LOW); ditto(CLIMB);
    make_loc(q, R_ABOVEP,
	     "You are in a secret N/S canyon above a sizable passage.",
	     NULL, 0);
    make_ins(N, R_SJUNC);
    make_ins(D, R_BEDQUILT); ditto(PASSAGE);
    make_ins(S, R_TITE);
    make_loc(q, R_SJUNC,
	     "You are in a secret canyon at a junction of three canyons, bearing\n"
	     "north, south, and SE.  The north one is as tall as the other two\n"
	     "combined.",
	     "You're at junction of three secret canyons.", 0);
    make_ins(SE, R_BEDQUILT);
    make_ins(S, R_ABOVEP);
    make_ins(N, R_WINDOW);
    make_loc(q, R_TITE,
	     "A large stalactite extends from the roof and almost reaches the floor\n"
	     "below.  You could climb down it, and jump from it to the floor, but\n"
	     "having done so you would be unable to reach it to climb back up.",
	     "You're on top of stalactite.", 0);
    make_ins(N, R_ABOVEP);
    make_cond_ins(D, 40, R_LIKE6); ditto(JUMP); ditto(CLIMB);
    make_cond_ins(D, 50, R_LIKE9);
    make_ins(D, R_LIKE4);
    make_loc(q, R_LOW,
	     "You are in a large low room.  Crawls lead north, SE, and SW.",
	     NULL, 0);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_ins(SW, R_SCORR);
    make_ins(N, R_CRAWL);
    make_ins(SE, R_ORIENTAL); ditto(ORIENTAL);
    make_loc(q, R_CRAWL,
	     "Dead end crawl.",
	     NULL, 0);
    make_ins(S, R_LOW); ditto(CRAWL); ditto(OUT);
    make_loc(q, R_WINDOW,
	     "You're at a low window overlooking a huge pit, which extends up out of\n"
	     "sight.  A floor is indistinctly visible over 50 feet below.  Traces of\n"
	     "white mist cover the floor of the pit, becoming thicker to the left.\n"
	     "Marks in the dust around the window would seem to indicate that\n"
	     "someone has been here recently.  Directly across the pit from you and\n"
	     "25 feet away there is a similar window looking into a lighted room.\n"
	     "A shadowy figure can be seen there peering back at you.",
	     "You're at window on pit.", 0);	     
    make_ins(W, R_SJUNC);
    make_ins(JUMP, R_NECK);
    make_loc(q, R_ORIENTAL,
	     "This is the Oriental Room.  Ancient oriental cave drawings cover the\n"
	     "walls.  A gently sloping passage leads upward to the north, another\n"
	     "passage leads SE, and a hands-and-knees crawl leads west.",
	     "You're in Oriental Room.", 0);
    make_ins(SE, R_CHEESE);
    make_ins(W, R_LOW); ditto(CRAWL);
    make_ins(U, R_MISTY); ditto(N); ditto(CAVERN);
    make_loc(q, R_MISTY,
	     "You are following a wide path around the outer edge of a large cavern.\n"
	     "Far below, through a heavy white mist, strange splashing noises can be\n"
	     "heard.  The mist rises up through a fissure in the ceiling.  The path\n"
	     "exits to the south and west.",
	     "You're in misty cavern.", 0);
    make_ins(S, R_ORIENTAL); ditto(ORIENTAL);
    make_ins(W, R_ALCOVE);
    make_loc(q, R_ALCOVE,
	     "You are in an alcove.  A small NW path seems to widen after a short\n"
	     "distance.  An extremely tight tunnel leads east.  It looks like a very\n"
	     "tight squeeze.  An eerie light can be seen at the other end.",
	     "You're in alcove.", F_DARK_HINT);
    make_ins(NW, R_MISTY); ditto(CAVERN);
    make_ins(E, R_PPASS); ditto(PASSAGE);
    make_ins(E, R_PROOM);  /* never performed, but seen by "BACK" */
    make_loc(q, R_PROOM,
	     "You're in a small chamber lit by an eerie green light.  An extremely\n"
	     "narrow tunnel exits to the west.  A dark corridor leads NE.",
	     "You're in Plover Room.", F_LIGHTED | F_DARK_HINT);
    make_ins(W, R_PPASS); ditto(PASSAGE); ditto(OUT);
    make_ins(W, R_ALCOVE);  /* never performed, but seen by "BACK" */
    make_cond_ins(PLOVER, 100+EMERALD, R_PDROP);
    make_ins(PLOVER, R_Y2);
    make_ins(NE, R_DROOM); ditto(DARK);
    make_loc(q, R_DROOM,
	     "You're in the Dark-Room.  A corridor leading south is the only exit.",
	     "You're in Dark-Room.", F_DARK_HINT);
    make_ins(S, R_PROOM); ditto(PLOVER); ditto(OUT);
    make_loc(q, R_SLAB,
	     "You are in a large low circular chamber whose floor is an immense slab\n"
	     "fallen from the ceiling (Slab Room).  There once were large passages\n"
	     "to the east and west, but they are now filled with boulders.  Low\n"
	     "small passages go north and south, and the south one quickly bends\n"
	     "east around the boulders.",
	     "You're in Slab Room.", 0);
    make_ins(S, R_W2PIT);
    make_ins(U, R_ABOVER); ditto(CLIMB);
    make_ins(N, R_BEDQUILT);
    make_loc(q, R_ABOVER,
	     "You are in a secret N/S canyon above a large room.",
	     NULL, 0);
    make_ins(D, R_SLAB); ditto(SLAB);
    make_cond_ins(S, 300+DRAGON, R_SCAN2);
    make_ins(S, R_SCAN1);
    make_ins(N, R_MIRROR);
    make_ins(RESERVOIR, R_RES);
    make_loc(q, R_MIRROR,
	     "You are in a north/south canyon about 25 feet across.  The floor is\n"
	     "covered by white mist seeping in from the north.  The walls extend\n"
	     "upward for well over 100 feet.  Suspended from some unseen point far\n"
	     "above you, an enormous two-sided mirror is hanging parallel to and\n"
	     "midway between the canyon walls.  (The mirror is obviously provided\n"
	     "for the use of the dwarves, who as you know are extremely vain.)\n"
	     "A small window can be seen in either wall, some fifty feet up.",
	     "You're in mirror canyon.", 0);
    make_ins(S, R_ABOVER);
    make_ins(N, R_RES); ditto(RESERVOIR);
    make_loc(q, R_RES,
	     "You are at the edge of a large underground reservoir.  An opaque cloud\n"
	     "of white mist fills the room and rises rapidly upward.  The lake is\n"
	     "fed by a stream, which tumbles out of a hole in the wall about 10 feet\n"
	     "overhead and splashes noisily into the water somewhere within the\n"
	     "mist.  The only passage goes back toward the south.",
	     "You're at reservoir.", F_LIQUID);
    make_ins(S, R_MIRROR); ditto(OUT);

    /* R_SCAN1 and R_SCAN3 are the rooms the player sees when entering the
     * secret canyon from the north and the east, respectively. The dragon
     * blocks different exits in each room.
     * Once the dragon has been vanquished, R_SCAN2 replaces both rooms.
     */
    make_loc(q, R_SCAN1,
	     "You are in a secret canyon that exits to the north and east.",
	     NULL, 0);
    make_ins(N, R_ABOVER); ditto(OUT);
    remarks[10] = "The dragon looks rather nasty.  You'd best not try to get by.";
    make_ins(E, MAX_SPEC+10); ditto(FORWARD);
    make_loc(q, R_SCAN2,
	     long_desc[R_SCAN1],
	     NULL, 0);
    make_ins(N, R_ABOVER);
    make_ins(E, R_SECRET);
    make_loc(q, R_SCAN3,
	     long_desc[R_SCAN1],
	     NULL, 0);
    make_ins(E, R_SECRET); ditto(OUT);
    make_ins(N, MAX_SPEC+10); ditto(FORWARD);

    make_loc(q, R_SECRET,
	     "You are in a secret canyon, which here runs E/W.  It crosses over a\n"
	     "very tight canyon 15 feet below.  If you go down you may not be able\n"
	     "to get back up.",
	     "You're in secret E/W canyon above tight canyon.", 0);
    make_ins(E, R_HMK);
    make_cond_ins(W, 300+DRAGON, R_SCAN2);
    make_ins(W, R_SCAN3);
    make_ins(D, R_WIDE);
    make_loc(q, R_WIDE,
	     "You are at a wide place in a very tight N/S canyon.",
	     NULL, 0);
    make_ins(S, R_TIGHT);
    make_ins(N, R_TALL);
    make_loc(q, R_TIGHT,
	     "The canyon here becomes too tight to go further south.",
	     NULL, 0);
    make_ins(N, R_WIDE);
    make_loc(q, R_TALL,
	     "You are in a tall E/W canyon.  A low tight crawl goes 3 feet north and\n"
	     "seems to open up.",
	     "You're in tall E/W canyon.", 0);
    make_ins(E, R_WIDE);
    make_ins(W, R_BOULDERS);
    make_ins(N, R_CHEESE); ditto(CRAWL);
    make_loc(q, R_BOULDERS,
	     /* Knuth can't help using "---" for the em dash here,
	      * but Woods' version had only "--". */
	     "The canyon runs into a mass of boulders -- dead end.",
	     NULL, 0);
    make_ins(S, R_TALL);
    make_loc(q, R_SCORR,
	     "You are in a long winding corridor sloping out of sight in both\n"
	     "directions.",
	     "You're in sloping corridor.", 0);
    make_ins(D, R_LOW);
    make_ins(U, R_SWSIDE);
    make_loc(q, R_SWSIDE,
	     "You are on one side of a large, deep chasm.  A heavy white mist rising\n"
	     "up from below obscures all view of the far side.  A SW path leads away\n"
	     "from the chasm into a winding corridor.",
	     "You're on SW side of chasm.", 0);
    make_ins(SW, R_SCORR);
    remarks[11] = "The troll refuses to let you cross.";
    make_cond_ins(OVER, 200+TROLL, MAX_SPEC+11); ditto(ACROSS); ditto(CROSS); ditto(NE);
    remarks[12] = "There is no longer any way across the chasm.";
    make_cond_ins(OVER, 300+BRIDGE, MAX_SPEC+12);
    make_ins(OVER, R_TROLL);
    make_cond_ins(JUMP, 300+BRIDGE, R_LOSE);
    make_ins(JUMP, MAX_SPEC+2);
    make_loc(q, R_DEAD0, dead_end, NULL, 0);
    make_ins(S, R_CROSS); ditto(OUT);
    make_loc(q, R_DEAD1, dead_end, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE11); ditto(OUT);
    make_loc(q, R_PIRATES_NEST, dead_end, NULL, 0);
    make_ins(SE, R_LIKE13);
    make_loc(q, R_DEAD3, dead_end, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE4); ditto(OUT);
    make_loc(q, R_DEAD4, dead_end, 0, F_TWIST_HINT);
    make_ins(E, R_LIKE4); ditto(OUT);
    make_loc(q, R_DEAD5, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_LIKE3); ditto(OUT);
    make_loc(q, R_DEAD6, dead_end, 0, F_TWIST_HINT);
    make_ins(W, R_LIKE9); ditto(OUT);
    make_loc(q, R_DEAD7, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_LIKE10); ditto(OUT);
    make_loc(q, R_DEAD8, dead_end, 0, 0);
    make_ins(E, R_BRINK); ditto(OUT);
    make_loc(q, R_DEAD9, dead_end, 0, F_TWIST_HINT);
    make_ins(S, R_LIKE3); ditto(OUT);
    make_loc(q, R_DEAD10, dead_end, 0, F_TWIST_HINT);
    make_ins(E, R_LIKE12); ditto(OUT);
    make_loc(q, R_DEAD11, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_LIKE8); ditto(OUT);
    make_loc(q, R_NESIDE,
	     "You are on the far side of the chasm.  A NE path leads away from the\n"
	     "chasm on this side.",
	     "You're on NE side of chasm.", 0);
    make_ins(NE, R_CORR);
    make_cond_ins(OVER, 200+TROLL, MAX_SPEC+11); ditto(ACROSS); ditto(CROSS); ditto(SW);
    make_ins(OVER, R_TROLL);
    make_ins(JUMP, MAX_SPEC+2);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_CORR,
	     "You're in a long east/west corridor.  A faint rumbling noise can be\n"
	     "heard in the distance.",
	     "You're in corridor.", 0);
    make_ins(W, R_NESIDE);
    make_ins(E, R_FORK); ditto(FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_FORK,
	     "The path forks here.  The left fork leads northeast.  A dull rumbling\n"
	     "seems to get louder in that direction.  The right fork leads southeast\n"
	     "down a gentle slope.  The main corridor enters from the west.",
	     "You're at fork in path.", 0);
    make_ins(W, R_CORR);
    make_ins(NE, R_WARM); ditto(L);
    make_ins(SE, R_LIME); ditto(R); ditto(D);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_WARM,
	     "The walls are quite warm here.  From the north can be heard a steady\n"
	     "roar, so loud that the entire cave seems to be trembling.  Another\n"
	     "passage leads south, and a low crawl goes east.",
	     "You're at junction with warm walls.", 0);
    make_ins(S, R_FORK); ditto(FORK);
    make_ins(N, R_VIEW); ditto(VIEW);
    make_ins(E, R_CHAMBER); ditto(CRAWL);
    make_loc(q, R_VIEW,
	     "You are on the edge of a breath-taking view.  Far below you is an\n"
	     "active volcano, from which great gouts of molten lava come surging\n"
	     "out, cascading back down into the depths.  The glowing rock fills the\n"
	     "farthest reaches of the cavern with a blood-red glare, giving every-\n"
	     "thing an eerie, macabre appearance.  The air is filled with flickering\n"
	     "sparks of ash and a heavy smell of brimstone.  The walls are hot to\n"
	     "the touch, and the thundering of the volcano drowns out all other\n"
	     "sounds.  Embedded in the jagged roof far overhead are myriad twisted\n"
	     "formations, composed of pure white alabaster, which scatter the murky\n"
	     "light into sinister apparitions upon the walls.  To one side is a deep\n"
	     "gorge, filled with a bizarre chaos of tortured rock that seems to have\n"
	     "been crafted by the Devil himself.  An immense river of fire crashes\n"
	     "out from the depths of the volcano, burns its way through the gorge,\n"
	     "and plummets into a bottomless pit far off to your left.  To the\n"
	     "right, an immense geyser of blistering steam erupts continuously\n"
	     "from a barren island in the center of a sulfurous lake, which bubbles\n"
	     "ominously.  The far right wall is aflame with an incandescence of its\n"
	     "own, which lends an additional infernal splendor to the already\n"
	     "hellish scene.  A dark, foreboding passage exits to the south.",
	     "You're at breath-taking view.", F_LIGHTED);
    make_ins(S, R_WARM); ditto(PASSAGE); ditto(OUT);
    make_ins(FORK, R_FORK);
    remarks[13] = default_msg[EAT];  /* Don't be ridiculous! */
    make_ins(D, MAX_SPEC+13); ditto(JUMP);
    make_loc(q, R_CHAMBER,
	     "You are in a small chamber filled with large boulders.  The walls are\n"
	     "very warm, causing the air in the room to be almost stifling from the\n"
	     "heat.  The only exit is a crawl heading west, through which a low\n"
	     "rumbling noise is coming.",
	     "You're in chamber of boulders.", 0);
    make_ins(W, R_WARM); ditto(OUT); ditto(CRAWL);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_LIME,
	     "You are walking along a gently sloping north/south passage lined with\n"
	     "oddly shaped limestone formations.",
	     "You're in limestone passage.", 0);
    make_ins(N, R_FORK); ditto(U); ditto(FORK);
    make_ins(S, R_FBARR); ditto(D); ditto(BARREN);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_FBARR,
	     "You are standing at the entrance to a large, barren room.  A sign\n"
	     "posted above the entrance reads:  \"CAUTION!  BEAR IN ROOM!\"",
	     "You're in front of barren room.", 0);
    make_ins(W, R_LIME); ditto(U);
    make_ins(FORK, R_FORK);
    make_ins(E, R_BARR); ditto(IN); ditto(BARREN); ditto(ENTER);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_BARR,
	     "You are inside a barren room.  The center of the room is completely\n"
	     "empty except for some dust.  Marks in the dust lead away toward the\n"
	     "far end of the room.  The only exit is the way you came in.",
	     "You're in barren room.", 0);
    make_ins(W, R_FBARR); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    /* The end-game repository. */
    make_loc(q, R_NEEND,
	     "You are at the northeast end of an immense room, even larger than the\n"
	     "Giant Room.  It appears to be a repository for the \"Adventure\"\n"
	     "program.  Massive torches far overhead bathe the room with smoky\n"
	     "yellow light.  Scattered about you can be seen a pile of bottles (all\n"
	     "of them empty), a nursery of young beanstalks murmuring quietly, a bed\n"
	     "of oysters, a bundle of black rods with rusty stars on their ends, and\n"
	     "a collection of brass lanterns.  Off to one side a great many dwarves\n"
	     "are sleeping on the floor, snoring loudly.  A sign nearby reads:  \"DO\n"
	     "NOT DISTURB THE DWARVES!\"  An immense mirror is hanging against one\n"
	     "wall, and stretches to the other end of the room, where various other\n"
	     "sundry objects can be glimpsed dimly in the distance.",
	     "You're at NE end.", F_LIGHTED);
    make_ins(SW, R_SWEND);
    make_loc(q, R_SWEND,
	     "You are at the southwest end of the repository.  To one side is a pit\n"
	     "full of fierce green snakes.  On the other side is a row of small\n"
	     "wicker cages, each of which contains a little sulking bird.  In one\n"
	     "corner is a bundle of black rods with rusty marks on their ends.\n"
	     "A large number of velvet pillows are scattered about on the floor.\n"
	     "A vast mirror stretches off to the northeast.  At your feet is a\n"
	     "large steel grate, next to which is a sign that reads, \"TREASURE\n"
	     "VAULT.  KEYS IN MAIN OFFICE.\"",
	     "You're at SW end.", F_LIGHTED);
    make_ins(NE, R_NEEND);
    make_ins(D, MAX_SPEC+1);  /* You can't go through a locked steel grate! */

    /* When the current location is R_CRACK or higher, it's a pseudo-location.
     * In such cases we don't ask you for input; we assume that you have told
     * us to force another instruction through. For example, if you try to go
     * through the crack by the small pit in the upper cave (R_SPIT), the
     * instruction there sends you to R_CRACK, which immediately sends you
     * back to R_SPIT.
     * [ajo] Couldn't these mostly be replaced with remarks? TODO
     */
    make_loc(q, R_CRACK, "The crack is far too small for you to follow.", NULL, 0);
    make_ins(0, R_SPIT);
    make_loc(q, R_NECK, "You are at the bottom of the pit with a broken neck.", NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_LOSE, "You didn't make it.", NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_CANT, "The dome is unclimbable.", NULL, 0);
    make_ins(0, R_EMIST);
    make_loc(q, R_CLIMB, "You clamber up the plant and scurry through the hole at the top.", NULL, 0);
    make_ins(0, R_NARROW);
    make_loc(q, R_CHECK, "", NULL, 0);
    make_cond_ins(0, 300+PLANT+200, R_UPNOUT);
    make_ins(0, R_DIDIT);
    make_loc(q, R_SNAKED, "You can't get by the snake.", NULL, 0);
    make_ins(0, R_HMK);
    make_loc(q, R_THRU,
	     "You have crawled through a very low wide passage parallel to and north\n"
	     "of the Hall of Mists.",
	     NULL, 0);
    make_ins(0, R_WMIST);
    make_loc(q, R_DUCK, long_desc[R_THRU], NULL, 0);
    make_ins(0, R_WFISS);
    make_loc(q, R_SEWER,
	     "The stream flows out through a pair of 1-foot-diameter sewer pipes.\n"
	     "It would be advisable to use the exit.",
	     NULL, 0);
    make_ins(0, R_HOUSE);
    make_loc(q, R_UPNOUT,
	     "There is nothing here to climb.  Use \"up\" or \"out\" to leave the pit.",
	     NULL, 0);
    make_ins(0, R_WPIT);
    make_loc(q, R_DIDIT, "You have climbed up the plant and out of the pit.", NULL, 0);
    make_ins(0, R_W2PIT);
    
    /* The remaining "locations" R_PPASS, R_PDROP, and R_TROLL are special. */
    start[R_PPASS] = q;
}

/*========== Data structures for objects. =================================
 * This section corresponds to sections 63--70 in Knuth.
 */

struct ObjectData {
    struct ObjectData *link;
    struct ObjectData *base;
    int prop;
    Location place;
    const char *name;
    int offset;
} objs[MAX_OBJ+1];

struct ObjectData *first[MAX_LOC+1];
const char *note[100];
int holding_count;  /* how many objects have prop[t] < 0? */
Location knife_loc;  /* place where knife was mentioned, or -1 */
int tally = 15;  /* treasures awaiting you */
int lost_treasures;  /* treasures that you won't find */

#define toting(t) (objs[t].place < 0)

ObjectWord bottle_contents(void)
{
    switch (objs[BOTTLE].prop) {
	case 0: return WATER;
	case 2: return OIL;
	/* other valid values: 1, -2 (post-closing) */
    }
    return NOTHING;
}

bool here(ObjectWord t, Location loc)
{
    return toting(t) || (objs[t].place == loc);
}

void drop(ObjectWord t, Location l)
{
    if (toting(t)) --holding_count;
    objs[t].place = l;
    if (l < 0) {
	++holding_count;
    } else if (l > 0) {
	objs[t].link = first[l];
	first[l] = &objs[t];
    }
}

#define move(t,l) do { carry(t); drop((t),l); } while (0)
#define destroy(t) move((t), R_LIMBO)

void carry(ObjectWord t)
{
    Location l = objs[t].place;
    if (l >= R_LIMBO) {
	objs[t].place = R_INHAND;
	++holding_count;
	if (l > R_LIMBO) {
	    /* Remove t from l's object-list */
	    struct ObjectData **p = &first[l];
	    while (*p != &objs[t]) p = &(*p)->link;
	    *p = (*p)->link;
	}
    }
}

bool is_at_loc(ObjectWord t, Location loc)
{
    ObjectWord tt;
    if (objs[t].base == NULL)
	return (objs[t].place == loc);
    /* Check the "alternative" objects based on this one. */
    for (tt = t; objs[tt].base == &objs[t]; ++tt) {
	if (objs[tt].place == loc)
	    return true;
    }
    return false;
}

void mobilize(ObjectWord t) { objs[t].base = NULL; }
void immobilize(ObjectWord t) { objs[t].base = &objs[t]; }

void new_obj(int note_ptr, ObjectWord t, const char *n, ObjectWord b, Location l)
{
    objs[t].name = n;
    objs[t].base = (b != 0 ? &objs[b] : NULL);
    objs[t].offset = note_ptr;
    objs[t].prop = (IS_TREASURE(t) ? -1 : 0);
    drop(t, l);
}

void build_object_table(void)
{
    new_obj(0, RUG_, 0, RUG, R_SCAN3);
    new_obj(0, RUG, "Persian rug", RUG, R_SCAN1);
    note[0] = "There is a Persian rug spread out on the floor!";
    note[1] = "The dragon is sprawled out on a Persian rug!!";
    new_obj(2, TROLL2_, 0, TROLL2, R_LIMBO);
    new_obj(2, TROLL2, 0, TROLL2, R_LIMBO);
    note[2] = "The troll is nowhere to be seen.";
    new_obj(3, TROLL_, 0, TROLL, R_NESIDE);
    new_obj(3, TROLL, 0, TROLL, R_SWSIDE);
    note[3] =
	"A burly troll stands by the bridge and insists you throw him a\n"
	"treasure before you may cross.";
    note[4] = "The troll steps out from beneath the bridge and blocks your way.";
    note[5] = NULL;
    new_obj(6, BRIDGE_, 0, BRIDGE, R_NESIDE);
    new_obj(6, BRIDGE, 0, BRIDGE, R_SWSIDE);
    note[6] =
	"A rickety wooden bridge extends across the chasm, vanishing into the\n"
	"mist. A sign posted on the bridge reads, \"STOP! PAY TROLL!\"";
    note[7] =
	"The wreckage of a bridge (and a dead bear) can be seen at the bottom\n"
	"of the chasm.";
    new_obj(8, DRAGON_, 0, DRAGON, R_SCAN3);
    new_obj(8, DRAGON, 0, DRAGON, R_SCAN1);
    note[8] = "A huge green fierce dragon bars the way!";
    note[9] =
	"Congratulations!  You have just vanquished a dragon with your bare\n"
	"hands! (Unbelievable, isn't it?)";
    note[10] = "The body of a huge green dead dragon is lying off to one side.";
    new_obj(11, SHADOW_, 0, SHADOW, R_WINDOW);
    new_obj(11, SHADOW, 0, SHADOW, R_WINDOE);
    note[11] = "The shadowy figure seems to be trying to attract your attention.";
    new_obj(12, PLANT2_, 0, PLANT2, R_E2PIT);
    new_obj(12, PLANT2, 0, PLANT2, R_W2PIT);
    note[12] = NULL;
    note[13] = "The top of a 12-foot-tall beanstalk is poking out of the west pit.";
    note[14] = "There is a huge beanstalk growing out of the west pit up to the hole.";
    new_obj(15, CRYSTAL_, 0, CRYSTAL, R_WFISS);
    new_obj(15, CRYSTAL, 0, CRYSTAL, R_EFISS);
    note[15] = NULL;
    note[16] ="A crystal bridge now spans the fissure.";
    note[17] = "The crystal bridge has vanished!";
    new_obj(18, TREADS_, 0, TREADS, R_EMIST);
    new_obj(18, TREADS, 0, TREADS, R_SPIT);
    note[18] = "Rough stone steps lead down the pit.";
    note[19] = "Rough stone steps lead up the dome.";
    new_obj(20, GRATE_, 0, GRATE, R_INSIDE);
    new_obj(20, GRATE, 0, GRATE, R_OUTSIDE);
    note[20] = "The grate is locked.";
    note[21] = "The grate is open.";
    new_obj(22, MIRROR_, 0, MIRROR, R_LIMBO);  /* joins up with MIRROR later */
    new_obj(22, CHAIN, "Golden chain", CHAIN, R_BARR);
    note[22] = "There is a golden chain lying in a heap on the floor!";
    note[23] = "The bear is locked to the wall with a golden chain!";
    note[24] = "There is a golden chain locked to the wall!";
    new_obj(25, SPICES, "Rare spices", 0, R_CHAMBER);
    note[25] = "There are rare spices here!";
    new_obj(26, PEARL, "Glistening pearl", 0, R_LIMBO);
    note[26] = "Off to one side lies a glistening pearl!";
    new_obj(27, PYRAMID, "Platinum pyramid", 0, R_DROOM);
    note[27] = "There is a platinum pyramid here, 8 inches on a side!";
    new_obj(28, EMERALD, "Egg-sized emerald", 0, R_PROOM);
    note[28] = "There is an emerald here the size of a plover's egg!";
    new_obj(29, VASE, "Ming vase", 0, R_ORIENTAL);
    note[29] = "There is a delicate, precious, Ming vase here!";
    note[30] = "The vase is now resting, delicately, on a velvet pillow.";
    note[31] = "The floor is littered with worthless shards of pottery.";
    note[32] = "The Ming vase drops with a delicate crash.";
    new_obj(33, TRIDENT, "Jeweled trident", 0, R_FALLS);
    note[33] = "There is a jewel-encrusted trident here!";
    new_obj(34, EGGS, "Golden eggs", 0, R_GIANT);
    note[34] = "There is a large nest here, full of golden eggs!";
    note[35] = "The nest of golden eggs has vanished!";
    note[36] = "Done!";
    new_obj(37, CHEST, "Treasure chest", 0, R_LIMBO);
    note[37] = "The pirate's treasure chest is here!";
    new_obj(38, COINS, "Rare coins", 0, R_WEST);
    note[38] = "There are many coins here!";
    new_obj(39, JEWELS, "Precious jewelry", 0, R_SOUTH);
    note[39] = "There is precious jewelry here!";
    new_obj(40, SILVER, "Bars of silver", 0, R_NS);
    note[40] = "There are bars of silver here!";
    new_obj(41, DIAMONDS, "Several diamonds", 0, R_WFISS);
    note[41] = "There are diamonds here!";
    new_obj(42, GOLD, "Large gold nugget", 0, R_NUGGET);
    note[42] = "There is a large sparkling nugget of gold here!";
    new_obj(43, MOSS, 0, MOSS, R_SOFT);
    note[43] = NULL;
    new_obj(44, BATTERIES, "Batteries", 0, R_LIMBO);
    note[44] = "There are fresh batteries here.";
    note[45] = "Some worn-out batteries have been discarded nearby.";
    new_obj(46, PONY, 0, PONY, R_PONY);
    note[46] =
	"There is a massive vending machine here. The instructions on it read:\n"
	"\"Drop coins here to receive fresh batteries.\"";
    new_obj(47, GEYSER, 0, GEYSER, R_VIEW);
    note[47] = NULL;
    new_obj(48, MESSAGE, 0, MESSAGE, R_LIMBO);
    note[48] =
	"There is a message scrawled in the dust in a flowery script, reading:\n"
	"\"This is not the maze where the pirate hides his treasure chest.\"";
    new_obj(49, BEAR, 0, BEAR, R_BARR);
    note[49] = "There is a ferocious cave bear eying you from the far end of the room!";
    note[50] = "There is a gentle cave bear sitting placidly in one corner.";
    note[51] = "There is a contented-looking bear wandering about nearby.";
    note[52] = NULL;
    new_obj(53, PIRATE, 0, PIRATE, R_LIMBO);
    note[53] = NULL;
    new_obj(54, ART, 0, ART, R_ORIENTAL);
    note[54] = NULL;
    new_obj(55, AXE, "Dwarf's axe", 0, R_LIMBO);
    note[55] = "There is a little axe here.";
    note[56] = "There is a little axe lying beside the bear.";
    new_obj(57, STALACTITE, 0, STALACTITE, R_TITE);
    note[57] = NULL;
    new_obj(58, PLANT, 0, PLANT, R_WPIT);
    note[58] = "There is a tiny little plant in the pit, murmuring \"Water, water, ...\"";
    note[59] = "The plant spurts into furious growth for a few seconds.";
    note[60] =
	"There is a 12-foot-tall beanstalk stretching up out of the pit,\n"
	"bellowing \"Water!! Water!!\"";
    note[61] = "The plant grows explosively, almost filling the bottom of the pit.";
    note[62] = "There is a gigantic beanstalk stretching all the way up to the hole.";
    note[63] = "You've over-watered the plant! It's shriveling up! It's, it's...";
    new_obj(64, MIRROR, 0, MIRROR, R_MIRROR);
    note[64] = NULL;
    new_obj(65, OIL, "Oil in the bottle", 0, R_LIMBO);
    new_obj(65, WATER, "Water in the bottle", 0, R_LIMBO);
    new_obj(65, BOTTLE, "Small bottle", 0, R_HOUSE);
    note[65] = "There is a bottle of water here.";
    note[66] = "There is an empty bottle here.";
    note[67] = "There is a bottle of oil here.";
    new_obj(68, FOOD, "Tasty food", 0, R_HOUSE);
    note[68] = "There is food here.";
    new_obj(69, KNIFE, 0, 0, R_LIMBO);
    new_obj(69, DWARF, 0, DWARF, R_LIMBO);
    new_obj(69, MAG, "\"Spelunker Today\"", 0, R_ANTE);
    note[69] = "There are a few recent issues of \"Spelunker Today\" magazine here.";
    new_obj(70, OYSTER, "Giant oyster >GROAN!<", 0, R_LIMBO);
    note[70] = "There is an enormous oyster here with its shell tightly closed.";
    note[71] =
	"Interesting. There seems to be something written on the underside of\n"
	"the oyster.";
    new_obj(72, CLAM, "Giant clam >GRUNT!<", 0, R_SHELL);
    note[72] = "There is an enormous clam here with its shell tightly closed.";
    new_obj(73, TABLET, 0, TABLET, R_DROOM);
    note[73] =
	"A massive stone tablet embedded in the wall reads:\n"
	"\"CONGRATULATIONS ON BRINGING LIGHT INTO THE DARK-ROOM!\"";
    new_obj(74, SNAKE, 0, SNAKE, R_HMK);
    note[74] = "A huge green fierce snake bars the way!";
    note[75] = NULL;
    new_obj(76, PILLOW, "Velvet pillow", 0, R_SOFT);
    note[76] = "A small velvet pillow lies on the floor.";
    new_obj(77, DOOR, 0, DOOR, R_IMMENSE);
    note[77] = "The way north is barred by a massive, rusty, iron door.";
    note[78] = "The way north leads through a massive, rusty, iron door.";
    new_obj(79, BIRD, "Little bird in cage", 0, R_BIRD);
    note[79] = "A cheerful little bird is sitting here singing.";
    note[80] = "There is a little bird in the cage.";
    new_obj(81, ROD2, "Black rod", 0, R_LIMBO);
    note[81] = "A three-foot black rod with a rusty mark on an end lies nearby.";
    new_obj(82, ROD, "Black rod", 0, R_DEBRIS);
    note[82] = "A three-foot black rod with a rusty star on an end lies nearby.";
    new_obj(83, CAGE, "Wicker cage", 0, R_COBBLES);
    note[83] = "There is a small wicker cage discarded nearby.";
    new_obj(84, LAMP, "Brass lantern", 0, R_HOUSE);
    note[84] = "There is a shiny brass lamp nearby.";
    note[85] = "There is a lamp shining nearby.";
    new_obj(86, KEYS, "Set of keys", 0, R_HOUSE);
    note[86] = "There are some keys on the ground here.";
}



/*========== Input routines. ==============================================
 * This section corresponds to sections 71--73 in Knuth.
 */

#define BUF_SIZE 72
char buffer[BUF_SIZE]; /* your input goes here */
char word1[BUF_SIZE], word2[BUF_SIZE]; /* and then we snarf it to here */

bool yes(const char *q, const char *y, const char *n)
{
    while (true) {
	printf("%s\n** ", q); fflush(stdout);
	fgets(buffer, sizeof(buffer), stdin);
	if (tolower(*buffer) == 'y') {
	    if (y) puts(y);
	    return true;
	} else if (tolower(*buffer) == 'n') {
	    if (n) puts(n);
	    return false;
	} else {
	    puts(" Please answer Yes or No.");
	}
    }
}

void listen(void)
{
    char *p, *q;
    while (true) {
	printf("* "); fflush(stdout);
	fgets(buffer, sizeof(buffer), stdin);
	for (p = buffer; isspace(*p); ++p) ;
	if (*p == '\0') {
	    puts(" Tell me to do something."); continue;
	}
	/* Notice that this algorithm depends on the buffer's being
	 * terminated by "\n\0", or at least some whitespace character. */
	for (q = word1; !isspace(*p); ++p, ++q) {
	    *q = tolower(*p);
	}
	*q = '\0';
	for (++p; isspace(*p); ++p) ;
	if (*p == '\0') {
	    *word2 = '\0'; return;
	}
	for (q = word2; !isspace(*p); ++p, ++q) {
	    *q = tolower(*p);
	}
	*q = '\0';
	for (++p; isspace(*p); ++p) ;
	if (*p == '\0') return;
	puts(" Please stick to 1- and 2-word commands.");
    }
}

/*========== Dwarves and pirate. ==========================================
 * This section corresponds to sections ??? in Knuth.
 */

int dflag;  /* how angry are the dwarves? */
int dkill;  /* how many of them have you killed? */
Location dloc[6];
Location odloc[6];
bool dseen[6];

bool dwarf_in(int loc)  /* is a dwarf present? */
{
    int j;
    if (dflag < 2) return false;
    for (j=1; j <= 5; ++j) {
	if (dloc[j] == loc) return true;
    }
    return false;
}

void return_pirate_to_lair(bool with_chest)
{
    if (with_chest) {
	move(CHEST, R_PIRATES_NEST);
        move(MESSAGE, R_PONY);
    }
    dloc[0] = odloc[0] = R_PIRATES_NEST;
    dseen[0] = false;
}

bool too_easy_to_steal(ObjectWord t, Location loc)
{
    return (t == PYRAMID && (loc == R_PROOM || loc == R_DROOM));
}

void steal_all_your_treasure(Location loc)
{
    int i;
    puts("Out from the shadows behind you pounces a bearded pirate!  \"Har, har,\"\n"
	 "he chortles. \"I'll just take all this booty and hide it away with me\n"
	 "chest deep in the maze!\"  He snatches your treasure and vanishes into\n"
	 "the gloom.");
    for (i = MIN_TREASURE; i <= MAX_OBJ; ++i) {
	if (too_easy_to_steal(i, loc)) continue;
	if (objs[i].base == NULL && objs[i].place == loc) carry(i);
	if (toting(i)) drop(i, R_PIRATES_NEST);
    }
}

void pirate_tracks_you(Location loc)
{
    bool chest_needs_placing = (objs[MESSAGE].place == R_LIMBO);
    bool stalking = false;
    int i;
    /* The pirate leaves you alone once you've found the chest. */
    if (loc == R_PIRATES_NEST || objs[CHEST].prop >= 0) return;
    for (i = MIN_TREASURE; i <= MAX_OBJ; ++i) {
	if (too_easy_to_steal(i, loc)) continue;
	if (toting(i)) {
	    steal_all_your_treasure(loc);
	    return_pirate_to_lair(chest_needs_placing);
	    return;
	}
	if (objs[i].place == loc) {
	    /* We're carrying a treasure with base != NOTHING.
	     * [ajo] TODO understand how this can happen.
	     * The pirate can't steal it yet, so he remains quiet. */
	    stalking = true;
	}
    }
    /* tally is the number of treasures we haven't seen; lost_treasures is
     * the number we never will see (due to killing the bird or destroying
     * the troll bridge). */
    if (tally == lost_treasures+1 && !stalking && objs[MESSAGE].place == R_LIMBO &&
	objs[LAMP].prop && here(LAMP, loc)) {
	/* Let the pirate be spotted. We don't spot the pirate unless our lamp
	 * is on. We do spot him even if the lighted lamp is on the ground. */
	puts("There are faint rustling noises from the darkness behind you. As you\n"
	     "turn toward them, the beam of your lamp falls across a bearded pirate.\n"
	     "He is carrying a large chest. \"Shiver me timbers!\" he cries, \"I've\n"
	     "been spotted! I'd best hie meself off to the maze to hide me chest!\"\n"
	     "With that, he vanishes into the gloom.");
	return_pirate_to_lair(true);
	return;
    }
    if (odloc[0] != dloc[0] && pct(20)) {
	puts("There are faint rustling noises from the darkness behind you.");
    }
}

/* Return true if the player got killed by a dwarf this turn. */
bool move_dwarves_and_pirate(Location loc)
{
    if (loc > R_PIRATES_NEST || loc == R_LIMBO) {
	/* Bypass all dwarf motion if you are in a place forbidden to the
	 * pirate, or if your next motion is forced. Besides the cases that
	 * Knuth mentions (dwarves can't meet the bear, dwarves can't enter
	 * most dead ends), this also prevents the axe-toting dwarf from
	 * showing up in the middle of a forced move and dropping the axe
	 * in an inaccessible pseudo-location. */
    } else if (dflag == 0) {
	if (loc >= MIN_LOWER_LOC) dflag = 1;
    } else if (dflag == 1) {
	if (loc >= MIN_LOWER_LOC && pct(5)) {
	    /* When level 2 of the cave is reached, we silently kill 0, 1,
	     * or 2 of the dwarves. Then if any of the survivors is in
	     * the current location, we move him to R_NUGGET; thus no
	     * dwarf is presently tracking you. Another dwarf does,
	     * however, toss an axe and grumpily leave the scene. */
	    int j;
	    dflag = 2;
	    if (pct(50)) dloc[1+ran(5)] = R_LIMBO;
	    if (pct(50)) dloc[1+ran(5)] = R_LIMBO;
	    for (j=1; j <= 5; ++j) {
		if (dloc[j] == loc) dloc[j] = R_NUGGET;
		odloc[j] = dloc[j];
	    }
	    puts("A little dwarf just walked around a corner, saw you, threw a little\n"
		 "axe at you, cursed, and ran away.  (The axe missed.)");
	    drop(AXE, loc);
	}
    } else {
	/* Move dwarves and the pirate. */
	int dtotal = 0;  /* this many dwarves are in the room with you */
	int attack = 0;  /* this many have had time to draw their knives */
	int stick = 0;  /* this many have hurled their knives accurately */
	int j;
	for (j=0; j <= 5; ++j) {
	    if (dloc[j] != R_LIMBO) {
		Location ploc[19];  /* potential locations for the next random step */
		int i = 0;
		/* Make a table of all potential exits.
		 * Dwarves think R_SCAN1, R_SCAN2, R_SCAN3 are three different locations,
	  	 * although you will never have that perception. */
		Instruction *q;
		for (q = start[dloc[j]]; q < start[dloc[j]+1]; ++q) {
		    Location newloc = q->dest;
		    if (i != 0 && newloc == ploc[i-1]) continue;
		    if (newloc < MIN_LOWER_LOC) continue;  /* don't follow above level 2 */
		    if (newloc == odloc[j] || newloc == dloc[j]) continue;  /* don't double back */
		    if (q->cond == 100) continue;
		    if (j == 0 && newloc > R_PIRATES_NEST) continue;
		    if (j != 0 && newloc >= MIN_FORCED_LOC) continue;
		    ploc[i++] = newloc;
		}
		if (i==0) ploc[i++] = odloc[j];
		odloc[j] = dloc[j];
		dloc[j] = ploc[ran(i)];  /* this is the random walk */
		dseen[j] = (dloc[j] == loc ||
			    odloc[j] == loc ||
			    (dseen[j] && loc >= MIN_LOWER_LOC));
		if (dseen[j]) {
		    /* Make dwarf j follow */
		    dloc[j] = loc;
		    if (j == 0) {
			pirate_tracks_you(loc);
		    } else {
			++dtotal;
			if (odloc[j] == dloc[j]) {
			    ++attack;
			    if (knife_loc >= 0) knife_loc = loc;
			    if (pct(95*(dflag-2))) ++stick;
			}
		    }
		}
	    }
	}
	if (dtotal != 0) {
	    /* Make the threatening dwarves attack. */
	    if (dtotal == 1) {
		puts("There is a threatening little dwarf in the room with you!");
	    } else {
		printf("There are %d threatening little dwarves in the room with you!\n", dtotal);
	    }
	    if (attack) {
		if (dflag == 2) dflag = 3;
		if (attack == 1) {
		    printf("One sharp nasty knife is thrown at you --- ");
		    if (stick == 0) puts("it misses!");
		    else puts("it gets you!");
		} else {
		    printf(" %d of them throw knives at you --- ", attack);
		    if (stick == 0) puts("none of them hit you!");
		    else if (stick == 1) puts("one of them gets you!");
		    else printf("%d of them get you!\n", stick);
		}
		if (stick) return true;  /* goto death */
	    }
	}
    }
    return false;  /* the player survived this function */
}


/*========== Closing the cave. ============================================
 * This section corresponds to sections 103, 178--181 in Knuth.
 */

int lamp_limit;  /* countdown till darkness */
int clock1 = 15, clock2 = 30;  /* clocks that govern closing time */
bool panic, closed;  /* various stages of closedness */
int bonus;  /* extra points awarded for exceptional adventuring skills */

bool cave_is_closing(void)
{
    return (clock1 < 0);
}

void close_the_cave(void)
{
    int j;
    puts("The sepulchral voice intones, \"The cave is now closed.\"  As the echoes\n"
	 "fade, there is a blinding flash of light (and a small puff of orange\n"
	 "smoke). . . .    Then your eyes refocus; you look around and find...");
    move(BOTTLE, R_NEEND); objs[BOTTLE].prop = -2;
    move(PLANT, R_NEEND); objs[PLANT].prop = -1;
    move(OYSTER, R_NEEND); objs[OYSTER].prop = -1;
    move(LAMP, R_NEEND); objs[LAMP].prop = -1;
    move(ROD, R_NEEND); objs[ROD].prop = -1;
    move(DWARF, R_NEEND); objs[DWARF].prop = -1;
    move(MIRROR, R_NEEND); objs[MIRROR].prop = -1;
    move(GRATE, R_SWEND); objs[GRATE].prop = 0;
    move(SNAKE, R_SWEND); objs[SNAKE].prop = -2;
    move(BIRD, R_SWEND); objs[BIRD].prop = -1;
    move(CAGE, R_SWEND); objs[CAGE].prop = -1;
    move(ROD2, R_SWEND); objs[ROD2].prop = -1;
    move(PILLOW, R_SWEND); objs[PILLOW].prop = -1;
    move(MIRROR_, R_SWEND);
    for (j = 1; j <= MAX_OBJ; ++j) {
	if (toting(j)) destroy(j);
    }
    closed = true;
    bonus = 10;
}

/* Return true if the cave just closed. */
bool check_clocks_and_lamp(Location loc)
{
    if (tally == 0 && loc >= MIN_LOWER_LOC && loc != R_Y2)
	--clock1;
    if (clock1 == 0) {
	/* At the time of first warning, we lock the grate, destroy the
	 * crystal bridge, kill all the dwarves (and the pirate), and
	 * remove the troll and bear (unless dead).
	 * It's too much trouble to move the dragon, so we leave it.
	 * From now on until clock2 runs out, you cannot unlock the grate,
	 * move to any location outside the cave, or create the bridge.
	 * Nor can you be resurrected if you die. */
	int j;
	puts("A sepulchral voice, reverberating through the cave, says \"Cave\n"
	     "closing soon.  All adventurers exit immediately through main office.\"");
	clock1 = -1;
	objs[GRATE].prop = 0;
	objs[CRYSTAL].prop = 0;
	for (j=0; j <= 5; ++j) {
	    dseen[j] = false;
	    dloc[j] = R_LIMBO;
	}
	destroy(TROLL); destroy(TROLL_);
	move(TROLL2, R_SWSIDE); move(TROLL2_, R_NESIDE);
	move(BRIDGE, R_SWSIDE); move(BRIDGE_, R_NESIDE);
	if (objs[BEAR].prop != 3) destroy(BEAR);
	objs[CHAIN].prop = 0; mobilize(CHAIN);
	objs[AXE].prop = 0; mobilize(AXE);
    } else {
	if (cave_is_closing()) --clock2;
	if (clock2 == 0) {
	    close_the_cave();
	    return true;
	} else {
	    static bool warned = false;
	    /* On every turn (if the cave is not closed), we check to see
	     * if you are in trouble lampwise. */
	    if (objs[LAMP].prop == 1) --lamp_limit;
	    if (lamp_limit <= 30 && here(LAMP, loc) && here(BATTERIES, loc) && objs[BATTERIES].prop == 0) {
		puts("Your lamp is getting dim.  I'm taking the liberty of replacing\n"
		     "the batteries.");
		objs[BATTERIES].prop = 1;
		if (toting(BATTERIES)) drop(BATTERIES, loc);
		lamp_limit = 2500;
	    } else if (lamp_limit == 0) {
		if (here(LAMP, loc)) puts("Your lamp has run out of power.");
		objs[LAMP].prop = 0;
		lamp_limit = -1;
	    } else if (lamp_limit < 0 && loc < MIN_IN_CAVE) {
		puts("There's not much point in wandering around out here, and you can't\n"
		     "explore the cave without a lamp.  So let's just call it a day.");
		give_up();
	    } else if (lamp_limit < 30 && !warned && here(LAMP, loc)) {
		printf("Your lamp is getting dim");
		if (objs[BATTERIES].prop == 1) {
		    puts(", and you're out of spare batteries.  You'd\n"
			 "best start wrapping this up.");
		} else if (objs[BATTERIES].place == R_LIMBO) {
		    puts(".  You'd best start wrapping this up, unless\n"
			 "you can find some fresh batteries.  I seem to recall that there's\n"
			 "a vending machine in the maze.  Bring some coins with you.");
		} else {
		    puts(".  You'd best go back for those batteries.");
		}
		warned = true;
	    }
	}
    }
    return false;
}

void panic_at_closing_time(void)
{
    /* If you try to get out while the cave is closing, we assume that
     * you panic; we give you a few additional turns to get frantic
     * before we close. */
    if (!panic) {
	clock2 = 15;
	panic = true;
    }
    puts("A mysterious recorded voice groans into life and announces:\n"
	 "   \"This exit is closed.  Please leave via main office.\"");
}



/*========== The main loop. ===============================================
 * This section corresponds to sections 74--75 in Knuth.
 */

int turns;  /* how many times we've read your commands */
int verbose_interval = 5;  /* command BRIEF sets this to 10000 */
int foobar;  /* progress in the FEE FIE FOE FOO incantation */

void give_optional_plugh_hint(Location loc)
{
    if (loc == R_Y2 && pct(25) && !cave_is_closing()) {
	puts("A hollow voice says \"PLUGH\".");
    }
}

int look_around(Location loc, bool dark, bool was_dark)
{
    const char *room_description;
    if (dark && !FORCED_MOVE(loc)) {
	if (was_dark && pct(35)) return 'p';  /* goto pitch_dark; */
	room_description = pitch_dark_msg;
    } else if (short_desc[loc] == NULL || visits[loc] % verbose_interval == 0) {
	room_description = long_desc[loc];
    } else {
	room_description = short_desc[loc];
    }
    if (toting(BEAR)) {
	puts("You are being followed by a very large, tame bear.");
    }
    printf("\n%s\n", room_description);
    if (FORCED_MOVE(loc)) return 't';  /* goto try_move; */
    give_optional_plugh_hint(loc);
    if (!dark) {
	struct ObjectData *t;
	visits[loc]++;
	/* Describe the objects at this location. */
	for (t = first[loc]; t != NULL; t = t->link) {
	    struct ObjectData *tt = t->base ? t->base : t;
	    if (t->prop < 0) {  /* you've spotted a treasure */
		if (closed) continue;  /* no automatic prop change after hours */
		t->prop = (tt == &objs[RUG] || tt == &objs[CHAIN]);
		tally--;
		if (tally == lost_treasures && tally > 0 && lamp_limit > 35) {
		    /* Zap the lamp if the remaining treasures are too elusive */
		    lamp_limit = 35;
		}
	    }
	    if (tt == &objs[TREADS] && toting(GOLD)) {
		/* The rough stone steps disappear if we are carrying the nugget. */
	    } else {
		int going_up = (tt == &objs[TREADS] && loc == R_EMIST);
		const char *obj_description = note[tt->prop + tt->offset + going_up];
		if (obj_description != NULL) {
		    puts(obj_description);
		}
	    }
	}
    }
    return 0;  /* just continue normally */
}


/*========== Hints. =======================================================
 * This section corresponds to sections 80 and 193--196 in Knuth.
 */

void advise_about_going_west(const char *word1)
{
    /* Here's a freely offered hint that may save you typing. */
    static int west_count = 0;
    if (streq(word1, "west")) {
	++west_count;
	if (west_count == 10) {
	    puts(" If you prefer, simply type W rather than WEST.");
	}
    }
}

struct {
    int count;
    bool given;
    int thresh;
    int cost;
    const char *prompt;
    const char *hint;
} hints[] = {
    { 0, false, 0, 5, "Welcome to Adventure!!  Would you like instructions?",
    "Somewhere nearby is Colossal Cave, where others have found fortunes in\n"
    "treasure and gold, though it is rumored that some who enter are never\n"
    "seen again. Magic is said to work in the cave. I will be your eyes\n"
    "and hands. Direct me with commands of one or two words. I should\n"
    "warn you that I look at only the first five letters of each word, so\n"
    "you'll have to enter \"NORTHEAST\" as \"NE\" to distinguish it from\n"
    "\"NORTH\". Should you get stuck, type \"HELP\" for some general hints.\n"
    "For information on how to end your adventure, etc., type \"INFO\".\n"
    "                        -  -  -\n"
    "The first adventure program was developed by Willie Crowther.\n"
    "Most of the features of the current program were added by Don Woods.\n"
    "This particular program was translated from FORTRAN to CWEB by Don Knuth,\n"
    "and then from CWEB to ANSI C by Arthur O'Dwyer." },
    { 0, false, 0, 10,
    "Hmmm, this looks like a clue, which means it'll cost you 10 points to\n"
    "read it.  Should I go ahead and read it anyway?",
    "It says, \"There is something strange about this place, such that one\n"
    "of the words I've always known now has a new effect.\"" },
    { 0, false, 4, 2, "Are you trying to get into the cave?",
    "The grate is very solid and has a hardened steel lock.  You cannot\n"
    "enter without a key, and there are no keys in sight.  I would recommend\n"
    "looking elsewhere for the keys." },
    { 0, false, 5, 2, "Are you trying to catch the bird?",
    "Something seems to be frightening the bird just now and you cannot\n"
    "catch it no matter what you try. Perhaps you might try later." },
    { 0, false, 8, 2, "Are you trying to deal somehow with the snake?",
    "You can't kill the snake, or drive it away, or avoid it, or anything\n"
    "like that.  There is a way to get by, but you don't have the necessary\n"
    "resources right now." },
    { 0, false, 75, 4, "Do you need help getting out of the maze?",
    "You can make the passages look less alike by dropping things." },
    { 0, false, 25, 5, "Are you trying to explore beyond the Plover Room?",
    "There is a way to explore that region without having to worry about\n"
    "falling into a pit.  None of the objects available is immediately\n"
    "useful for discovering the secret." },
    { 0, false, 20, 3, "Do you need help getting out of here?",
    "Don't go west." }
};

void offer(int j)
{
    if (j > 1) {
	if (!yes(hints[j].prompt, " I am prepared to give you a hint,", ok)) return;
	printf(" but it will cost you %d points.  ", hints[j].cost);
	hints[j].given = yes("Do you want the hint?", hints[j].hint, ok);
    } else {
	hints[j].given = yes(hints[j].prompt, hints[j].hint, ok);
    }
    if (hints[j].given && lamp_limit > 30) {
	/* Give the lamp some more power. */
	lamp_limit += 30*hints[j].cost;
    }
}

void maybe_give_a_hint(Location loc, Location oldloc, Location oldoldloc, ObjectWord oldobj)
{
    /* Check if a hint applies, and give it if requested. */
    unsigned int k = F_CAVE_HINT;
    int j;
    for (j = 2; j <= 7; ++j, k <<= 1) {
	if (hints[j].given) continue;
	if ((flags[loc] & k) == 0) {
	    /* We've left the map region associated with this hint. */
	    hints[j].count = 0;
	    continue;
	}
	/* Count the number of turns spent here. */
	if (++hints[j].count >= hints[j].thresh) {
	    switch (j) {
		case 2:  /* How to get into the cave. */
		    if (!objs[GRATE].prop && !here(KEYS, loc)) {
			offer(j);
		    }
		    hints[j].count = 0;
		    break;
		case 3:  /* Are you trying to catch the bird? */
		    /* Notice that this hint is offered even when the bird has
		     * already been caught, if you spend enough time in the bird
		     * chamber carrying the rod and experimenting with the bird.
		     * This behavior is in Woods' Fortran original. */
		    if (here(BIRD, loc) && oldobj == BIRD && toting(ROD)) {
			offer(j);
			hints[j].count = 0;
		    }
		    break;
		case 4:  /* How to deal with the snake. */
		    if (here(SNAKE, loc) && !here(BIRD, loc)) {
			offer(j);
		    }
		    hints[j].count = 0;
		    break;
		case 5:  /* How to map the twisty passages all alike. */
		    if (first[loc] == NULL && first[oldloc] == NULL && first[oldoldloc] == NULL && holding_count > 1) {
			offer(j);
		    }
		    hints[j].count = 0;
		    break;
		case 6:  /* How to explore beyond the Plover Room. */
		    if (objs[EMERALD].prop != -1 && objs[PYRAMID].prop == -1) {
			offer(j);
		    }
		    hints[j].count = 0;
		    break;
		case 7:  /* How to get out of Witt's End. */
		    offer(j);
		    hints[j].count = 0;
		    break;
	    }
	}
    }
}

/*========== Scoring. =====================================================
 * This section corresponds to sections 193 and 197 in Knuth.
 */

#define MAX_SCORE 350
#define MAX_DEATHS 3
bool gave_up;
int death_count;

int score(void)
{
    int i;
    int s = 2;
    if (dflag != 0) s += 25;
    for (i = MIN_TREASURE; i <= MAX_OBJ; ++i) {
	if (objs[i].prop >= 0) {
	    s += 2;  /* two points just for seeing a treasure */
	    if (objs[i].place == R_HOUSE && objs[i].prop == 0) {
		if (i < CHEST) {
		    s += 10;
		} else if (i == CHEST) {
		    s += 12;
		} else {
		    s += 14;
		}
	    }
	}
    }
    s += 10 * (MAX_DEATHS - death_count);
    if (!gave_up) s += 4;
    if (objs[MAG].place == R_WITT) s += 1;  /* last lousy point */
    if (cave_is_closing()) s += 25;  /* bonus for making it this far */
    s += bonus;
    for (i = 0; i < 8; ++i) {
	if (hints[i].given) s -= hints[i].cost;
    }
    return s;
}

void say_score(void)
{
    printf("If you were to quit now, you would score %d\n"
	   "out of a possible %d.\n", score()-4, MAX_SCORE);
    if (yes("Do you indeed wish to quit now?", ok, ok)) give_up();
}

#define HIGHEST_CLASS 8
static int class_score[] = { 35, 100, 130, 200, 250, 300, 330, 349, 9999 };
static const char *class_message[] = {
    "You are obviously a rank amateur.  Better luck next time.",
    "Your score qualifies you as a novice class adventurer.",
    "You have achieved the rating \"Experienced Adventurer\".",
    "You may now consider yourself a \"Seasoned Adventurer\".",
    "You have reached \"Junior Master\" status.",
    "Your score puts you in Master Adventurer Class C.",
    "Your score puts you in Master Adventurer Class B.",
    "Your score puts you in Master Adventurer Class A.",
    "All of Adventuredom gives tribute to you, Adventure Grandmaster!"
};

void give_up(void)
{
    gave_up = true;
    quit();
}

void quit(void)
{
    /* Print the score and say adieu. */
    int s = score();
    int rank;
    printf("You scored %d out of a possible %d, using %d turn%s.\n",
	   s, MAX_SCORE, turns, (turns==1 ? "" : "s"));
    for (rank = 0; class_score[rank] <= s; ++rank) ;
    printf("%s\nTo achieve the next higher rating", class_message[rank]);
    if (rank < HIGHEST_CLASS) {
	int delta = class_score[rank] - s;
	printf(", you need %d more point%s.\n",
	       delta, (delta==1 ? "" : "s"));
    } else {
	puts(" would be a neat trick!\nCongratulations!!");
    }
    exit(0);
}

const char *death_wishes[2*MAX_DEATHS] = {
    "Oh dear, you seem to have gotten yourself killed. I might be able to\n"
      "help you out, but I've never really done this before. Do you want me\n"
      "to try to reincarnate you?",
    "All right. But don't blame me if something goes wr......\n"
      "                   --- POOF!! ---\n"
      "You are engulfed in a cloud of orange smoke. Coughing and gasping,\n"
      "you emerge from the smoke and find....",
    "You clumsy oaf, you've done it again! I don't know how long I can\n"
      "keep this up. Do you want me to try reincarnating you again?",
    "Okay, now where did I put my resurrection kit?.... >POOF!<\n"
      "Everything disappears in a dense cloud of orange smoke.",
    "Now you've really done it! I'm out of orange smoke! You don't expect\n"
      "me to do a decent reincarnation without any orange smoke, do you?",
    "Okay, if you're so smart, do it yourself! I'm leaving!"
};

void kill_the_player(Location last_safe_place)
{
    int j;
    death_count++;
    if (cave_is_closing()) {
	puts("It looks as though you're dead.  Well, seeing as how it's"
	     " so close to closing time anyway, let's just call it a day.");
	quit();
    }
    if (!yes(death_wishes[2*death_count-2], death_wishes[2*death_count-1], ok) || death_count == MAX_DEATHS)
	quit();
    /* At this point you are reborn. */
    for (j = MAX_OBJ; j > 0; --j) {
	if (toting(j)) drop(j, (j == LAMP) ? R_ROAD : last_safe_place);
    }
    if (toting(LAMP)) objs[LAMP].prop = 0;
    objs[WATER].place = R_LIMBO;
    objs[OIL].place = R_LIMBO;
}

/*========== Main loop. ===================================================
 * This section corresponds to sections 74--??? in Knuth.
 */

void adjustments_before_listening(Location loc)
{
    (void)ran(1);  /* kick the random number generator */
    if (knife_loc > R_LIMBO && knife_loc != loc) knife_loc = R_LIMBO;
    if (closed) {
	/* After the cave is closed, we look for objects being toted with
	 * prop < 0; their prop value is changed to -1 - prop. This means
         * they won't be described until they've been picked up and put
         * down, separate from their respective piles. */
	if (objs[OYSTER].prop < 0 && toting(OYSTER)) {
	    int j;
	    puts(note[71]);
	    for (j=1; j <= MAX_OBJ; ++j) {
		if (toting(j) && objs[j].prop < 0)
		    objs[j].prop = -1 - objs[j].prop;
	    }
        }
    }
}

Location attempt_plover_passage(Location from)  /* section 149 in Knuth */
{
    if (holding_count == !!toting(EMERALD))
        return R_ALCOVE + R_PROOM - from;
    puts("Something you're carrying won't fit through the tunnel with you.\n"
         "You'd best take inventory and drop something.");
    return from;
}

void attempt_inventory(void)
{
    bool holding_anything = false;
    ObjectWord t;
    for (t = 1; t <= MAX_OBJ; ++t) {
	if (toting(t) && (objs[t].base == NULL || objs[t].base == &objs[t]) && t != BEAR) {
	    if (!holding_anything) {
		holding_anything = true;
		puts("You are currently holding the following:");
	    }
	    printf(" %s\n", objs[t].name);
	}
    }
    if (toting(BEAR)) {
	puts("You are being followed by a very large, tame bear.");
    } else if (!holding_anything) {
	puts("You're not carrying anything.");
    }
}

void attempt_eat(ObjectWord obj)  /* section 98 in Knuth */
{
    switch (obj) {
	case FOOD:
	    destroy(FOOD);
	    puts("Thank you, it was delicious!");
	    break;
	case BIRD: case SNAKE: case CLAM: case OYSTER:
	case DWARF: case DRAGON: case TROLL: case BEAR:
	    puts("I think I just lost my appetite.");
	    break;
	default:
	    puts(default_msg[EAT]);
	    break;
    }
}

void take_something_immobile(ObjectWord obj)
{
    if (obj == CHAIN && objs[BEAR].prop != 0) {
	puts("The chain is still locked.");
    } else if (obj == BEAR && objs[BEAR].prop == 1) {
	puts("The bear is still chained to the wall.");
    } else if (obj == PLANT && objs[PLANT].prop <= 0) {
	puts("The plant has exceptionally deep roots and cannot be pulled free.");
    } else {
	puts("You can't be serious!");
    }
}

/* The verb is TAKE. Returns true if the action is finished. */
bool take_bird_or_cage(ObjectWord obj)
{
    if (obj == BIRD && !objs[BIRD].prop) {
	if (toting(ROD)) {
	    puts("The bird was unafraid when you entered, but as you approach it becomes\n"
		 "disturbed and you cannot catch it.");
	    return true;
	} else if (!toting(CAGE)) {
	    puts("You can catch the bird, but you cannot carry it.");
	    return true;
	} else {
	    objs[BIRD].prop = 1;
	}
    }
    /* At this point the TAKE action is guaranteed to succeed, so
     * let's also deal with the cage. Taking a caged bird means also
     * taking the cage; taking a cage with a bird in it means also
     * taking the bird. */
    if (obj == BIRD) carry(CAGE);
    if (obj == CAGE && objs[BIRD].prop) carry(BIRD);
    return false;
}

/* Return true if the command was TAKE WATER or TAKE OIL,
 * and there's not a bottle of that substance on the ground.
 * We'll redirect it to FILL BOTTLE and try again. */
bool attempt_take(ObjectWord obj, Location loc)
{
    if (toting(obj)) {
	puts(default_msg[TAKE]);
	return false;
    } else if (objs[obj].base != NULL) {
	take_something_immobile(obj);
	return false;
    } else if (obj != NOTHING && here(BOTTLE, loc) && obj == bottle_contents()) {
	obj = BOTTLE;  /* take the bottle of water or oil */
    } else if (obj == WATER || obj == OIL) {
	if (toting(BOTTLE))
	    return true;  /* redirect to FILL BOTTLE */
	puts("You have nothing in which to carry it.");
	return false;
    }
    if (holding_count >= 7) {
	puts("You can't carry anything more.  You'll have to drop something first.");
    } else if (take_bird_or_cage(obj)) {
	/* The bird was uncatchable. */
    } else {
        carry(obj);
        if (obj == BOTTLE && bottle_contents() != NOTHING)
	    objs[bottle_contents()].place = R_INHAND;
	puts(ok);
    }
    return false;
}

void attempt_drop(ObjectWord obj, Location loc)
{
    bool suppress_ok_message = false;
    if (obj == ROD && toting(ROD2) && !toting(ROD))
	obj = ROD2;
    if (!toting(obj)) {
	puts(default_msg[DROP]);
	return;		
    }
    if (obj == COINS && here(PONY, loc)) {
	/* Put coins in the vending machine. */
	destroy(COINS);
	drop(BATTERIES, loc);
	objs[BATTERIES].prop = 0;
	puts(note[44]);
	return;
    } else if (obj == BIRD) {
	if (here(SNAKE, loc)) {
	    puts("The little bird attacks the green snake, and in an astounding flurry\n"
		 "drives the snake away.");
	    suppress_ok_message = true;
	    if (closed) dwarves_upset();  /* Wow, is this reachable? */
	    destroy(SNAKE);
	    objs[SNAKE].prop = 1;  /* used in conditional Instructions */
	} else if (is_at_loc(DRAGON, loc) && objs[DRAGON].prop == 0) {
	    destroy(BIRD);
	    objs[BIRD].prop = 0;
	    /* Now that the bird is dead, you can never get past the snake
	     * into the south side chamber, so the precious jewelry is lost. */
	    if (objs[SNAKE].place == R_HMK) ++lost_treasures;
	    puts("The little bird attacks the green dragon, and in an astounding flurry\n"
		 "gets burnt to a cinder.  The ashes blow away.");
	    return;
	}
    } else if (obj == VASE && loc != R_SOFT) {
	suppress_ok_message = true;
	if (objs[PILLOW].place == loc) {
	    puts(note[30]);
	    objs[VASE].prop = 0;  /* the vase is now broken */
	    immobilize(VASE);
	} else {
	    puts(note[32]);
	    objs[VASE].prop = 2;  /* resting gently on the pillow */
	}
    } else if (obj == BEAR && is_at_loc(TROLL, loc)) {
	/* Chase the troll away. */
	puts("The bear lumbers toward the troll, who lets out a startled shriek and\n"
	     "scurries away.  The bear soon gives up the pursuit and wanders back.");
	suppress_ok_message = true;
	destroy(TROLL); destroy(TROLL_);
	drop(TROLL2, R_SWSIDE); drop(TROLL2_, R_NESIDE);
	objs[TROLL].prop = 2;
	move(BRIDGE, R_SWSIDE); move(BRIDGE_, R_NESIDE);  /* put first in their lists */
    } else {
	/* Special cases for dropping a liquid. */
	if (obj == WATER && objs[BOTTLE].prop == 0) obj = BOTTLE;
	if (obj == OIL && objs[BOTTLE].prop == 2) obj = BOTTLE;
	if (obj == BOTTLE && bottle_contents() != NOTHING)
	    objs[bottle_contents()].place = R_LIMBO;
    }
    if (obj == BIRD) objs[BIRD].prop = 0;  /* no longer caged */
    if (obj == CAGE && objs[BIRD].prop) drop(BIRD, loc);
    drop(obj, loc);
    if (!suppress_ok_message)
	puts(ok);
}

void attempt_wave(ObjectWord obj, Location loc)
{
    if (obj == ROD && (loc == R_EFISS || loc == R_WFISS) &&
	    toting(ROD) && !cave_is_closing()) {
	bool crystal_bridge_exists = objs[CRYSTAL].prop;
	puts(note[16+crystal_bridge_exists]);
	objs[CRYSTAL].prop = !crystal_bridge_exists;
    } else if (toting(obj) || (obj == ROD && toting(ROD2))) {
	puts(default_msg[WAVE]);  /* Nothing happens. */
    } else {
	puts(default_msg[DROP]);  /* You aren't carrying it! */
    }
}

void attempt_blast(Location loc)
{
    if (closed && objs[ROD2].prop >= 0) {
	bonus = (here(ROD2, loc) ? 25 :  /* bonus for blowing self up */
		 (loc == R_NEEND) ? 30 :  /* bonus for drowning in magma */
		 45);  /* bonus for winning the game */
	puts(message[bonus/5]);
	quit();
    } else {
	puts(default_msg[BLAST]);
    }
}

void attempt_rub(ObjectWord obj)
{
    if (obj == LAMP) {
	puts(default_msg[RUB]);  /* Rubbing the electric lamp... */
    } else {
	puts(default_msg[TOSS]);  /* Peculiar. Nothing unexpected happens. */
    }
}

void attempt_find(ObjectWord obj, Location loc)
{
    if (toting(obj)) {
	puts(default_msg[TAKE]);  /* You are already carrying it! */
    } else if (closed) {
	puts("I daresay whatever you want is around here somewhere.");
    } else {
	bool its_right_here = false;
	if (is_at_loc(obj, loc)) {
	    its_right_here = true;
	} else if (obj != NOTHING && obj == bottle_contents() && objs[BOTTLE].place == loc) {
	    its_right_here = true;
	} else if (obj == WATER && (flags[loc] & F_LIQUID) && !(flags[loc] & F_OIL)) {
	    its_right_here = true;
	} else if (obj == OIL && (flags[loc] & F_OIL)) {
	    its_right_here = true;
	} else if (obj == DWARF && dwarf_in(loc)) {
	    its_right_here = true;
	}
	if (its_right_here) {
	    puts("I believe what you want is right here with you.");
	} else {
	    puts(default_msg[FIND]);  /* I can only tell you what you see... */
	}
    }
}

void attempt_break(ObjectWord obj, Location loc)
{
    if (obj == VASE && objs[VASE].prop == 0) {
	if (toting(VASE))
	    drop(VASE, loc);
	puts("You have taken the vase and hurled it delicately to the ground.");
	objs[VASE].prop = 2;  /* worthless shards */
	immobilize(VASE);
    } else if (obj == MIRROR) {
	if (closed) {
	    puts("You strike the mirror a resounding blow, whereupon it shatters into a\n"
		 "myriad tiny fragments.");
	    dwarves_upset();
	} else {
	    puts("It is too far up for you to reach.");
	}
    } else {
	puts(default_msg[BREAK]);
    }
}

void attempt_wake(ObjectWord obj)
{
    if (closed && obj == DWARF) {
	puts("You prod the nearest dwarf, who wakes up grumpily, takes one look at\n"
	     "you, curses, and grabs for his axe.");
	dwarves_upset();
    }
    puts(default_msg[WAKE]);
}

void throw_axe_at_dwarf(Location loc)  /* section 163 in Knuth */
{
    int j;
    for (j=1; j <= 5; ++j) {
	if (dloc[j] == loc) break;
    }
    if (ran(3) < 2) {
	if (dkill == 0) {
	    puts("You killed a little dwarf.  The body vanishes in a cloud of greasy\n"
		 "black smoke.");
	} else {
	    puts("You killed a little dwarf.");
	}
	dloc[j] = R_LIMBO;
	dseen[j] = false;
	++dkill;
    } else {
	puts("You attack a little dwarf, but he dodges out of the way.");
    }
}

/* Return true if we don't know what to fill. */
bool attempt_fill(ObjectWord obj, Location loc)  /* sections 110--111 in Knuth */
{
    if (obj == VASE) {
	if (!(flags[loc] & F_LIQUID)) {
	    puts("There is nothing here with which to fill the vase.");
	} else if (!toting(VASE)) {
	    puts(default_msg[DROP]);
	} else {
	    puts("The sudden change in temperature has delicately shattered the vase.");
	    objs[VASE].prop = 2;  /* worthless shards */
	    immobilize(VASE);
	}
    }
    if (!here(BOTTLE, loc)) {
	if (obj == NOTHING)
	    return true;
	puts(default_msg[FILL]);
    } else if (obj != NOTHING && obj != BOTTLE) {
	puts(default_msg[FILL]);
    } else if (bottle_contents() != NOTHING) {
	puts("Your bottle is already full.");
    } else if (!(flags[loc] & F_LIQUID)) {
	puts("There is nothing here with which to fill the bottle.");
    } else {
	objs[BOTTLE].prop = (flags[loc] & F_OIL);  /* sneaky! F_OIL == 2. */
	if (toting(BOTTLE)) objs[bottle_contents()].place = R_INHAND;
	printf("Your bottle is now full of %s.\n", objs[BOTTLE].prop ? "oil" : "water");
    }
    return false;  /* just continue normally */
}

void attempt_feed(ObjectWord obj, Location loc)
{
    switch (obj) {
	case BIRD:
	    puts("It's not hungry (it's merely pinin' for the fjords).  Besides, you\n"
		 "have no bird seed.");
	    break;
	case TROLL:
	    puts("Gluttony is not one of the troll's vices.  Avarice, however, is.");
	    break;
	case DRAGON:
	    if (objs[DRAGON].prop) {
		puts(default_msg[EAT]);  /* reject feeding the dead dragon */
	    } else {
		puts("There's nothing here it wants to eat (except perhaps you).");
	    }
	    break;
	case SNAKE:
	    if (!closed && here(BIRD, loc)) {
	        destroy(BIRD);
	        objs[BIRD].prop = 0;
	        ++lost_treasures;
		puts("The snake has now devoured your bird.");
	    } else {
		puts("There's nothing here it wants to eat (except perhaps you).");
	    }
	    break;
	case BEAR:
	    if (!here(FOOD, loc)) {
		if (objs[BEAR].prop == 0) break;  /* ferocious bear, no food */
		if (objs[BEAR].prop == 3) {
		    /* Apparently "FEED BEAR" is interpreted the same as
		     * "EAT BEAR", when the bear is dead. [ajo] TODO check Fortran version */
		    puts("I think I just lost my appetite.");
		} else {
		    puts(default_msg[FEED]);
		}
	    } else {
		destroy(FOOD);
		objs[BEAR].prop = 1;
		objs[AXE].prop = 0;
		mobilize(AXE);	/* if it was immobilized by the bear */
		puts("The bear eagerly wolfs down your food, after which he seems to calm\n"
		     "down considerably and even becomes rather friendly.");
	    }
	    break;
	case DWARF:
	    if (here(FOOD, loc)) {
		++dflag;
		puts("You fool, dwarves eat only coal!  Now you've made him REALLY mad!");
	    } else {
		puts(default_msg[FEED]);
	    }
	    break;
	default:
	    puts(default_msg[CALM]);
    }
}

void attempt_open_or_close(ActionWord verb, ObjectWord obj, Location loc)
{
    bool verb_is_open = (verb == OPEN);  /* otherwise it's CLOSE */
    switch (obj) {
	/* [ajo] Knuth's version sets k to 0 for the clam or 1 for the oyster,
	 * but apparently never uses k after that. TODO: understand this. */
	case OYSTER:
	case CLAM: {
	    const char *clam_oyster = (obj == CLAM ? "clam" : "oyster");
	    if (!verb_is_open) {
		puts("What?");
	    } else if (!toting(TRIDENT)) {
		printf("You don't have anything with which to open the %s.\n", clam_oyster);
	    } else if (toting(obj)) {
		printf("I advise you to put down the %s before opening it.  %s\n",
		       clam_oyster, (obj == CLAM ? ">STRAIN!<" : ">WRENCH!<"));
	    } else if (obj == CLAM) {
		/* The pearl will appear in the cul-de-sac no matter where
		 * we are; we don't allow the player to carry the clam out
		 * of the Shell Room area. */
		destroy(CLAM);
		drop(OYSTER, loc);
		drop(PEARL, R_SAC);
		puts("A glistening pearl falls out of the clam and rolls away.  Goodness,\n"
		     "this must really be an oyster.  (I never was very good at identifying\n"
		     "bivalves.)  Whatever it is, it has now snapped shut again.");
	    } else {
		puts("The oyster creaks open, revealing nothing but oyster inside.\n"
		     "It promptly snaps shut again.");
	    }
	    break;
	}
	case GRATE:
	    if (!here(KEYS, loc)) {
		puts("You have no keys!");
	    } else if (cave_is_closing()) {
		/* Trying to get out through the grate after closing. */
		panic_at_closing_time();
	    } else {
		bool was_open = objs[GRATE].prop;
		objs[GRATE].prop = verb_is_open;
		switch (was_open + 2*verb_is_open) {
		    case 0: puts("It was already locked."); break;
		    case 1: puts("The grate is now locked."); break;
		    case 2: puts("The grate is now unlocked."); break;
		    case 3: puts("It was already unlocked."); break;
		}
	    }
	    break;
	case CHAIN:
	    if (!here(KEYS, loc)) {
		puts("You have no keys!");
	    } else if (verb_is_open) {
		/* UNLOCK CHAIN */
		if (objs[CHAIN].prop == 0) {
		    puts("It was already unlocked.");
		} else if (objs[BEAR].prop == 0) {
		    puts("There is no way to get past the bear to unlock the chain, which is\n"
			 "probably just as well.");
		} else {
		    objs[CHAIN].prop = 0;
		    mobilize(CHAIN);
		    if (objs[BEAR].prop == 3) {
			/* [ajo] TODO isn't the dead bear already immobile? */
		        immobilize(BEAR);
		    } else {
			objs[BEAR].prop = 2;
			mobilize(BEAR);
		    }
		    puts("The chain is now unlocked.");
		}
	    } else {
		/* LOCK CHAIN */
		if (loc != R_BARR) {
		    puts("There is nothing here to which the chain can be locked.");
		} else if (objs[CHAIN].prop) {
		    puts("It was already locked.");
		} else {
		    objs[CHAIN].prop = 2;
		    immobilize(CHAIN);
		    if (toting(CHAIN)) drop(CHAIN, loc);
		    puts("The chain is now locked.");
		}
	    }
	    break;
	case KEYS:
	    puts("You can't lock or unlock the keys.");
	    break;
	case CAGE:
	    puts("It has no lock.");
	    break;
	case DOOR:
	    if (objs[DOOR].prop) {
		puts(ok);
	    } else {
		/* Notice that CLOSE DOOR also gives this response. */
		puts("The door is extremely rusty and refuses to open.");
	    }
	    break;
	default:
	    puts(default_msg[verb]);
	    break;
    }
}

bool now_in_darkness(Location loc)
{
    if (flags[loc] & F_LIGHTED) return false;
    if (here(LAMP, loc) && objs[LAMP].prop) return false;
    return true;
}

/* This is some pretty baroque logic for an obscure case.
 * We're deciding what the noun should be if the player types
 * "READ" as a one-word command. If we return NOTHING, the
 * main loop will branch to get_object; otherwise it will
 * be handled as if we'd typed "READ <obj>". */
ObjectWord read_what(Location loc)
{
    ObjectWord obj = NOTHING;
    if (now_in_darkness(loc))
	return NOTHING;  /* can't read in the dark */
    if (here(MAG, loc)) obj = MAG;
    if (here(TABLET, loc)) {
	if (obj) return NOTHING;
	obj = TABLET;
    } else if (here(MESSAGE, loc)) {
	if (obj) return NOTHING;
	obj = MESSAGE;
    } else if (closed && toting(OYSTER)) {
	obj = OYSTER;
    }
    return obj;
}

void attempt_read(ObjectWord obj)  /* section 135 in Knuth */
{
    switch (obj) {
	case MAG:
	    puts("I'm afraid the magazine is written in dwarvish.");
	    break;
	case TABLET:
	    puts("\"CONGRATULATIONS ON BRINGING LIGHT INTO THE DARK-ROOM!\"");
	    break;
	case MESSAGE:
	    puts("\"This is not the maze where the pirate hides his treasure chest.\"");
	    break;
	case OYSTER:
	    if (closed && toting(OYSTER)) {
		if (hints[1].given) {
		    puts("It says the same thing it did before.");
		} else {
		    offer(1);
		}
		break;
	    }
	    /* FALLTHROUGH */
	default:
	    puts(default_msg[READ]);
	    break;
    }
}

int check_noun_validity(ObjectWord obj, Location loc)
{
    if (toting(obj) || is_at_loc(obj, loc))
	return 0;  /* no problem */
    switch (obj) {
	case GRATE:
	    if (loc < MIN_LOWER_LOC) {
		switch (loc) {
		    case R_ROAD: case R_VALLEY: case R_SLIT:
			return 'd';  /* try moving to DEPRESSION */
		    case R_COBBLES: case R_DEBRIS: case R_AWK: case R_BIRD: case R_SPIT:
			return 'e';  /* try moving to ENTRANCE */
		    default:
			break;
		}
	    }
	    return 'c';  /* can't see it */
	case DWARF:
	    if (dflag >= 2 && dwarf_in(loc)) return 0;
	    return 'c';  /* can't see it */
	case PLANT:
	    if (is_at_loc(PLANT2, loc) && objs[PLANT2].prop != 0) {
		obj = PLANT2;
		return 0;
	    }
	    return 'c';  /* can't see it */
	case KNIFE:
	    if (loc != knife_loc) return 'c';
	    knife_loc = -1;
	    puts("The dwarves' knives vanish as they strike the walls of the cave.");
	    return 'f';  /* action is finished */
	case ROD:
	    if (!here(ROD2, loc)) return 'c';
	    obj = ROD2;
	    return 0;
	case WATER:
	    if (here(BOTTLE, loc) && objs[BOTTLE].prop == 0) return 0;
	    if ((flags[loc] & F_LIQUID) && !(flags[loc] & F_OIL)) return 0;
	    return 'c';
	case OIL:
	    if (here(BOTTLE, loc) && objs[BOTTLE].prop == 2) return 0;
	    if (flags[loc] & F_OIL) return 0;
	    return 'c';
    }
    return 'c';
}

Instruction *determine_motion_instruction(Location loc, MotionWord mot)
{
    Instruction *q;
    for (q = start[loc]; q < start[loc+1]; ++q) {
	if (FORCED_MOVE(loc) || q->mot == mot) break;
    }
    if (q == start[loc+1]) {
	/* Report on inapplicable motion. */
	switch (mot) {
	    case CRAWL: puts("Which way?"); break;
	    case XYZZY: case PLUGH: puts(default_msg[WAVE]); break;
	    case FIND: case INVENTORY: puts(default_msg[FIND]); break;
	    case N: case S: case E: case W: case NE: case SE: case NW: case SW:
	    case U: case D:
		puts("There is no way to go in that direction.");
		break;
	    case IN: case OUT:
		puts("I don't know in from out here.    Use compass points or name something\n"
		     "in the general direction you want to go.");
		break;
	    case FORWARD: case L: case R:
		puts("I am unsure how you are facing.    Use compass points or nearby objects.");
		break;
	    default:
		puts("I don't know how to apply that word here.");
		break;
	}
	return NULL;  /* newloc = loc at this point */
    }
    while (true) {
	int j = q->cond;
	if (j > 300) {
	    if (objs[j%100].prop != (j/100)-3) break;
	} else if (j <= 100) {
	    if (j == 0 || pct(j)) break;
	} else if (toting(j%100) || (j >= 200 && is_at_loc(j%100, loc))) {
	    break;
	}
      {
	/* [ajo] TODO understand this loop. */
	const Instruction *oldq;
	for (oldq = q++; q->dest == oldq->dest && q->cond == oldq->cond; ++q) ;
      }
    }
    return q;
}

/* Modify newloc in place, and return true if the player died crossing the troll bridge. */
bool determine_next_newloc(Location loc, Location *newloc, MotionWord mot)
{
    Instruction *q = determine_motion_instruction(loc, mot);
    if (q == NULL)
	return false;  /* This happens with commands like "LEFT" */
    *newloc = q->dest;
    if (*newloc <= MAX_LOC) {
	/* Normal movement, possibly with some conditions which we
	 * verified in determine_motion_instruction() already. */
	return false;
    }
    if (*newloc > MAX_SPEC) {
	puts(remarks[*newloc - MAX_SPEC]);
	*newloc = loc;
	return false;
    }
    switch (*newloc) {
	case R_PPASS: {
	    *newloc = attempt_plover_passage(loc);
	    return false;
	}
	case R_PDROP: {
	    drop(EMERALD, loc);
	    /* [ajo] Knuth used a goto here to resume table processing;
	     * I believe this version is equivalent, given the contents
	     * of the table. */
	    *newloc = R_Y2 + R_PROOM - loc;
	    return false;
	}
	case R_TROLL: {
	    /* Troll bridge crossing is treated as a special motion so
	     * that dwarves won't wander across and encounter the bear.
	     * You can get here only if TROLL is in limbo but TROLL2 has
	     * taken its place. Moreover, if you're on the southwest side,
	     * objs[TROLL].prop will be nonzero. If objs[TROLL].prop is 1,
	     * you've crossed since paying, or you've stolen away the payment.
	     */
	    if (objs[TROLL].prop == 1) {
		/* Block the troll bridge and stay put. */
		move(TROLL, R_SWSIDE); move(TROLL_, R_NESIDE);
		objs[TROLL].prop = 0;
		destroy(TROLL2); destroy(TROLL2_);
		move(BRIDGE, R_SWSIDE); move(BRIDGE_, R_NESIDE);
		puts(note[4]);
		*newloc = loc;  /* stay put */
		return false;
	    }
	    *newloc = R_NESIDE + R_SWSIDE - loc;  /* cross it */
	    if (objs[TROLL].prop == 0)
		objs[TROLL].prop = 1;
	    if (!toting(BEAR))
		return false;
	    puts("Just as you reach the other side, the bridge buckles beneath the\n"
		 "weight of the bear, who was still following you around.  You\n"
		 "scrabble desperately for support, but as the bridge collapses you\n"
		 "stumble back and fall into the chasm.");
	    objs[BRIDGE].prop = 1;
	    objs[TROLL].prop = 2;
	    drop(BEAR, *newloc);
	    objs[BEAR].prop = 3;  /* the bear is dead */
	    immobilize(BEAR);
	    if (objs[SPICES].prop < 0 && objs[SPICES].place >= R_NESIDE) ++lost_treasures;
	    if (objs[CHAIN].prop < 0 && objs[CHAIN].place >= R_NESIDE) ++lost_treasures;
	    return true;  /* goto death */
	}
    }
    assert(false);
    return false;
}

void simulate_an_adventure(void)
{
    Location oldoldloc, oldloc, loc, newloc;
    MotionWord mot = NOWHERE;  /* currently specified motion */
    ActionWord verb;  /* currently specified action */
    ActionWord oldverb;  /* verb before it was changed */
    ObjectWord obj = NOTHING;  /* currently specified object, if any */
    ObjectWord oldobj;  /* former value of obj */
    WordType command_type;
    bool was_dark = false;
    int look_count = 0;
    
    oldoldloc = oldloc = loc = newloc = R_ROAD;
    
    while (true) {
	/* Check for interference with the proposed move to newloc. */
	if (cave_is_closing() && newloc < MIN_IN_CAVE && newloc != R_LIMBO) {
	    panic_at_closing_time();
	    newloc = loc;
	} else if (newloc != loc && newloc <= R_PIRATES_NEST) {
	    /* Stay in loc if a dwarf is blocking the way to newloc */
	    int j;
	    for (j=1; j <= 5; ++j) {
		if (odloc[j] == newloc && dseen[j]) {
		    puts("A little dwarf with a big knife blocks your way.");
		    newloc = loc; break;
		}
	    }
	}
	loc = newloc;  /* hey, we actually moved you */
	if (move_dwarves_and_pirate(loc)) {
	    oldoldloc = loc;
	    goto death;
	}
    commence:
        if (loc == R_LIMBO) goto death;
	switch (look_around(loc, now_in_darkness(loc), was_dark)) {
	    case 'p': goto pitch_dark;
	    case 't': goto try_move;
	    default: break;
	}
	while (true) {
	    int k;
	    verb = oldverb = ABSTAIN;
	    oldobj = obj;
	    obj = NOTHING;
	cycle:
	    maybe_give_a_hint(loc, oldloc, oldoldloc, oldobj);
	    was_dark = now_in_darkness(loc);
	    adjustments_before_listening(loc);
	    listen();
	pre_parse:
	    ++turns;
	    if (verb == SAY) {
		/* Maybe you said "say" and we said "say what?" and you replied
		 * with two things to say. Then we assume you don't really want
		 * us to say anything. Section 82 in Knuth. */
		if (*word2 != '\0') {
		    verb = ABSTAIN;
		} else {
		    goto transitive;
		}
	    }
	    /* Just after every command you give, we make the foobar counter
	     * negative if you're on track, otherwise we zero it.
	     * This is section 138 in Knuth. */
	    foobar = (foobar > 0) ? -foobar : 0;
	    if (check_clocks_and_lamp(loc)) {
		/* The cave just closed! */
		loc = oldloc = R_NEEND;
		mot = NOWHERE;
		goto try_move;
	    }
	    /* Handle additional special cases of input (Knuth sections 83, 105) */
	    if (streq(word1, "enter")) {
		if (streq(word2, "water") || streq(word2, "strea")) {
		    if ((flags[loc] & F_LIQUID) && !(flags[loc] & F_OIL)) {
			puts("Your feet are now wet.");
		    } else {
			puts(default_msg[GO]);
		    }
		    continue;
		} else if (*word2 != '\0') {
		    goto shift;
		}
	    }
	    if (streq(word1, "water") || streq(word1, "oil")) {
		/* Sometimes "water" and "oil" are used as verbs. */
		if (streq(word2, "plant") && loc == objs[PLANT].place)
		    strcpy(word2, "pour");
		if (streq(word2, "door") && loc == objs[DOOR].place)
		    strcpy(word2, "pour");
	    }
	parse:
	    advise_about_going_west(word1);
	    k = lookup(word1);
	    if (k < 0) {
		printf("Sorry, I don't know the word \"%s\".\n", word1);
		goto cycle;
	    }
	branch:
	    command_type = hash_table[k].word_type;
	    switch (command_type) {
		case WordType_Motion:
		    mot = hash_table[k].meaning;
		    goto try_move;
		case WordType_Object:
		    obj = hash_table[k].meaning;
		    /* Make sure obj is meaningful at the current location.
		     * When you specify an object, it must be here, unless
		     * the verb is already known to be FIND or INVENTORY.
		     * A few other special cases are permitted; for example,
		     * water and oil might be present inside the bottle or
		     * as a feature of the location. */
		    switch (check_noun_validity(obj, loc)) {
			case 'c': goto cant_see_it;
			case 'd': mot = DEPRESSION; goto try_move;
			case 'e': mot = ENTRANCE; goto try_move;
			case 'f': continue;
			/* case 0: break; */
		    }
		    if (*word2 != '\0') break;
		    if (verb != ABSTAIN) goto transitive;
		    printf("What do you want to do with the %s?\n", word1);
		    goto cycle;
		case WordType_Action:
		    verb = hash_table[k].meaning;
		    if (verb == SAY) {
			obj = *word2;
		    } else if (*word2 != '\0') {
			break;
		    }
		    if (obj != NOTHING) goto transitive;
		    else goto intransitive;
		case WordType_Message:
		    puts(message[(int)hash_table[k].meaning]);
		    continue;
	    }
	shift:
	    strcpy(word1, word2);
	    *word2 = '\0';
	    goto parse;

	intransitive:
	    k = 0;
	    switch (verb) {
		case GO: case RELAX:
		    puts(default_msg[verb]);
		    continue;
		case ON: case OFF: case POUR: case FILL: case DRINK: case BLAST: case KILL:
		    goto transitive;
		case TAKE:
		    /* TAKE makes sense by itself if there's only one possible thing to take. */
		    if (first[loc] == NULL || first[loc]->link || dwarf_in(loc))
			goto get_object;
		    obj = first[loc] - &objs[0];
		    goto transitive;
		case EAT:
		    if (!here(FOOD, loc))
			goto get_object;
		    obj = FOOD;
		    goto transitive;
		case OPEN: case CLOSE:
		    if (objs[GRATE].place == loc || objs[GRATE_].place == loc) {
			obj = GRATE;
		    } else if (objs[DOOR].place == loc) {
			obj = DOOR;
		    } else if (here(CLAM, loc)) {
			obj = CLAM;
		    } else if (here(OYSTER, loc)) {
			obj = OYSTER;
		    }
		    if (here(CHAIN, loc)) {
			if (obj) goto get_object;
			obj = CHAIN;
		    }
		    if (obj) goto transitive;
		    puts("There is nothing here with a lock!");
		    continue;
		case READ:
		    obj = read_what(loc);
		    if (obj != NOTHING) goto transitive;
		    goto get_object;
		case INVENTORY:
		    attempt_inventory();
		    continue;
		case BRIEF:
		    verbose_interval = 10000;
		    look_count = 3;
		    puts("Okay, from now on I'll only describe a place in full the first time\n"
			 "you come to it.  To get the full description, say \"LOOK\".");
		    continue;
		case SCORE:
		    say_score();
		    continue;
		case QUIT:
		    if (yes("Do you really wish to quit now?", ok, ok)) give_up();
		    continue;
		case FEEFIE: {
		    static const char *const incantation[] = { "fee", "fie", "foe", "foo", "fum" };
		    int k = 0;
		    while (!streq(word1, incantation[k])) ++k;
		    if (foobar == -k) {
			foobar = k+1;
			if (foobar != 4) {
			    puts(ok);
			    continue;
			}
			foobar = 0;
			if (objs[EGGS].place == R_GIANT || (toting(EGGS) && loc == R_GIANT)) {
			    puts(default_msg[WAVE]);
			    continue;
			}
			if (objs[EGGS].place == R_LIMBO && objs[TROLL].place == R_LIMBO && objs[TROLL].prop == 0)
			    objs[TROLL].prop = 1;  /* the troll returns */
			k = (loc == R_GIANT ? 0 : here(EGGS, loc) ? 1 : 2);
			move(EGGS, R_GIANT);
			puts(note[34+k]);
			continue;
		    } else if (foobar == 0) {
			puts(default_msg[WAVE]);
		    } else {
		        puts("What's the matter, can't you read?  Now you'd best start over.");
		    }
		    continue;
		}
		default:
		    goto get_object;
	    }
	transitive:
	    k = 0;
	    switch (verb) {
		case SAY: {
		    if (*word2 != '\0') strcpy(word1, word2);
		    k = lookup(word1);
		    switch (hash_table[k].meaning) {
			case XYZZY: case PLUGH: case PLOVER: case FEEFIE:
			    *word2 = '\0';
			    obj = NOTHING;
			    goto branch;
			default:
			    printf("Okay, \"%s\".\n", word1);
			    continue;
		    }
		}
		case EAT:
		    attempt_eat(obj);
		    continue;
		case WAVE:
		    attempt_wave(obj, loc);
		    continue;
		case BLAST:
		    attempt_blast(loc);
		    continue;
		case RUB:
		    attempt_rub(obj);
		    continue;
		case FIND: case INVENTORY:
		    attempt_find(obj, loc);
		    continue;
		case BREAK:
		    attempt_break(obj, loc);
		    continue;
		case WAKE:
		    attempt_wake(obj);
		    continue;
		case ON:
		    if (!here(LAMP, loc)) {
			puts(default_msg[ON]);
			continue;
		    }
		    if (lamp_limit < 0) {
			puts("Your lamp has run out of power.");
			continue;
		    }
		    objs[LAMP].prop = 1;
		    puts("Your lamp is now on.");
		    if (was_dark) goto commence;
		    continue;
		case OFF:
		    if (!here(LAMP, loc)) {
			puts(default_msg[OFF]);
			continue;
		    }
		    objs[LAMP].prop = 0;
		    puts("Your lamp is now off.");
		    if (now_in_darkness(loc))
			puts(pitch_dark_msg);
		    continue;
		case DRINK: {
		    bool stream_here = (flags[loc] & F_LIQUID) && !(flags[loc] & F_OIL);
		    bool evian_here = here(BOTTLE, loc) && (bottle_contents() == WATER);
		    if (obj == NOTHING) {
			if (!stream_here && !evian_here)
			    goto get_object;
		    } else if (obj != WATER) {
			puts(default_msg[EAT]);
			continue;
		    }
		    /* Drink from the bottle if we can; otherwise from the stream. */
		    if (evian_here) {
			objs[BOTTLE].prop = 1;  /* empty */
			objs[WATER].place = R_LIMBO;
			puts("The bottle of water is now empty.");
		    } else {
			puts(default_msg[DRINK]);
		    }
		    continue;
		}
		case POUR:
		    if (obj == NOTHING || obj == BOTTLE) {
			obj = bottle_contents();
			if (obj == NOTHING) goto get_object;
		    }
		    if (toting(obj)) {
			if (obj != WATER && obj != OIL) {
			    puts("You can't pour that.");
			    continue;
			}
			objs[BOTTLE].prop = 1;  /* empty */
			objs[obj].place = R_LIMBO;
			if (loc == objs[PLANT].place) {
			    /* Try to water the plant. */
			    if (obj != WATER) {
				puts("The plant indignantly shakes the oil off its leaves and asks, \"Water?\"");
				continue;
			    } else {
				puts(note[58+objs[PLANT].prop+1]);
				objs[PLANT].prop += 2;
				if (objs[PLANT].prop > 4) objs[PLANT].prop = 0;
				objs[PLANT2].prop = objs[PLANT].prop >> 1;
				mot = NOWHERE;
				goto try_move;
			    }
			} else if (loc == objs[DOOR].place) {
			    /* Pour water or oil on the door. */
			    switch (obj) {
				case WATER:
				    objs[DOOR].prop = 0;
				    puts("The hinges are quite thoroughly rusted now and won't budge.");
				    break;
				case OIL:
				    objs[DOOR].prop = 1;
				    puts("The oil has freed up the hinges so that the door will now open.");
				    break;
			    }
			    continue;
			}
		    }
		    puts(default_msg[POUR]);
		    continue;
		case FILL:
		    if (attempt_fill(obj, loc)) {
			goto get_object;
		    }
		    continue;
		case TAKE:
		    if (attempt_take(obj, loc)) {
			oldverb = TAKE;
			verb = FILL;
			obj = BOTTLE;
			goto transitive;
		    }
		    continue;
		case DROP:
		    attempt_drop(obj, loc);
		    continue;
		case TOSS:
		    if (obj == ROD && toting(ROD2) && !toting(ROD)) obj = ROD2;
		    if (!toting(obj)) {
			puts(default_msg[TOSS]);
			continue;
		    }
		    if (IS_TREASURE(obj) && is_at_loc(TROLL, loc)) {
			/* Snarf a treasure for the troll. */
			drop(obj, R_LIMBO);
			destroy(TROLL); destroy(TROLL_);
			drop(TROLL2, R_SWSIDE); drop(TROLL2_, R_NESIDE);
			move(BRIDGE, R_SWSIDE); move(BRIDGE_, R_NESIDE);
			puts("The troll catches your treasure and scurries away out of sight.");
			continue;
		    }
		    if (obj == FOOD && here(BEAR, loc)) {
			oldverb = TOSS;
			verb = FEED;
			obj = BEAR;
			goto transitive;
		    }
		    if (obj != AXE) {
			oldverb = TOSS;
			verb = DROP;
			goto transitive;
		    }
		    if (dwarf_in(loc)) {
			throw_axe_at_dwarf(loc);
		    } else if (is_at_loc(DRAGON, loc) && !objs[DRAGON].prop) {
			puts("The axe bounces harmlessly off the dragon's thick scales.");
		    } else if (is_at_loc(TROLL, loc)) {
			puts("The troll deftly catches the axe, examines it carefully, and tosses it\n"
			     "back, declaring, \"Good workmanship, but it's not valuable enough.\"");
		    } else if (here(BEAR, loc) && objs[BEAR].prop) {
			/* Throw the axe at the bear. */
			drop(AXE, loc);
			objs[AXE].prop = 1;
			immobilize(AXE);
			if (objs[BEAR].place == loc) move(BEAR, loc);  /* keep bear first in the list */
			puts("The axe misses and lands near the bear where you can't get at it.");
			continue;
		    } else {
			obj = NOTHING;
			oldverb = TOSS;
			verb = KILL;
			goto transitive;
		    }
		    drop(AXE, loc);
		    mot = NOWHERE;
		    goto try_move;
		case KILL:
		    if (obj == NOTHING) {
			/* See if there's a unique object to attack. */
			int k = 0;
			if (dwarf_in(loc)) { ++k; obj = DWARF; }
			if (here(SNAKE, loc)) { ++k; obj = SNAKE; }
			if (is_at_loc(DRAGON, loc) && !objs[DRAGON].prop) { ++k; obj = DRAGON; }
			if (is_at_loc(TROLL, loc)) { ++k; obj = TROLL; }
			if (here(BEAR, loc) && !objs[BEAR].prop) { ++k; obj = BEAR; }
			if (k == 0) {
			    /* no enemies present */
			    if (here(BIRD, loc) && oldverb != TOSS) { ++k; obj = BIRD; }
			    if (here(CLAM, loc) || here(OYSTER, loc)) { ++k; obj = CLAM; }
			}
			if (k > 1) goto get_object;
		    }
		    switch (obj) {
			case NOTHING:
			    puts("There is nothing here to attack.");
			    continue;
			case BIRD:
			    if (closed) {
				puts("Oh, leave the poor unhappy bird alone.");
			    } else {
				destroy(BIRD);
				objs[BIRD].prop = 0;
				if (objs[SNAKE].place == R_HMK) ++lost_treasures;
				puts("The little bird is now dead.  Its body disappears.");
				continue;
			    }
			case DRAGON:
			    if (objs[DRAGON].prop) {
				puts("For crying out loud, the poor thing is already dead!");
				continue;
			    } else {
				/* If you insist on attacking the dragon, you win!
				 * He dies, the Persian rug becomes free, and R_SCAN2
				 * takes the place of R_SCAN1 and R_SCAN3. */
				puts("With what?  Your bare hands?");
				verb = ABSTAIN; obj = NOTHING;
				listen();
				if (streq(word1, "yes") || streq(word1, "y")) {
				    int t;
				    puts(note[9]);
				    objs[DRAGON].prop = 2;  /* dead */
				    objs[RUG].prop = 0;
				    mobilize(RUG);
				    immobilize(DRAGON_);
				    destroy(DRAGON_);
				    immobilize(RUG_);
				    destroy(RUG_);
				    for (t = 1; t <= MAX_OBJ; ++t) {
					if (objs[t].place == R_SCAN1 || objs[t].place == R_SCAN3)
					    move(t, R_SCAN2);
				    }
				    loc = R_SCAN2;
				    mot = NOWHERE;
				    goto try_move;
				} else {
				    goto pre_parse;
				}
			    }
			case CLAM:
			case OYSTER:
			    puts("The shell is very strong and impervious to attack.");
			    continue;
			case SNAKE:
			    puts("Attacking the snake both doesn't work and is very dangerous.");
			    continue;
			case DWARF:
			    if (closed) dwarves_upset();
			    puts("With what? Your bare hands?");
			    continue;
			case TROLL:
			    puts("Trolls are close relatives with the rocks and have skin as tough as\n"
				 "a rhinoceros hide.  The troll fends off your blows effortlessly.");
			    continue;
			case BEAR:
			    switch (objs[BEAR].prop) {
				case 0: puts("With what?  Your bare hands?  Against HIS bare hands?"); break;
				case 3: puts("For crying out loud, the poor thing is already dead!"); break;
				default: puts("The bear is confused; he only wants to be your friend."); break;
			    }
			    continue;
			default:
			    puts(default_msg[KILL]);
			    continue;
		    }
		case FEED:
		    attempt_feed(obj, loc);
		    continue;
		case OPEN: case CLOSE:
		    attempt_open_or_close(verb, obj, loc);
		    continue;
		case READ:
		    if (now_in_darkness(loc)) goto cant_see_it;
		    attempt_read(obj);
		    continue;
		default:
		    goto report_default;
	    }
	report_default:
	    if (default_msg[verb]) puts(default_msg[verb]);
	    continue;
	get_object:
	    printf("%s what?\n", word1);
	    goto cycle;
	cant_see_it:
	    if ((verb == FIND || verb == INVENTORY) && *word2 != '\0')
		goto transitive;
	    printf("I see no %s here.\n", word1);
	    continue;
	}
    try_move:
	/* A major cycle comes to an end when a motion verb mot has been
	 * given and we have computed the appropriate newloc accordingly. */
	newloc = loc;  /* by default we will stay put */
	if (mot == NOWHERE) continue;
	if (mot == BACK) {
	    /* Interestingly, the BACK command does not simply move the player back
	     * to oldloc. Instead, it attempts to trace a path connecting loc to
	     * oldloc; if no such passage exists, we fail to move. If such a passage
	     * does exist, then we react as if the player had typed the appropriate
	     * motion-word in the first place. */
	    Location l = (FORCED_MOVE(oldloc) ? oldoldloc : oldloc);
	    const Instruction *q, *qq;
	    oldoldloc = oldloc;
	    oldloc = loc;
	    if (l == loc) {
		puts("Sorry, but I no longer seem to remember how you got here.");
		continue;
	    }
	    for (q = start[loc], qq = NULL; q < start[loc+1]; ++q) {
		Location ll = q->dest;
		if (ll == l) goto found;
		if (ll <= MAX_LOC && FORCED_MOVE(ll) && start[ll]->dest == l) qq = q;
	    }
	    if (qq == NULL) {
		puts("You can't get there from here.");
		continue;
	    } else {
		q = qq;
	    }
	  found:
	    mot = q->mot;
	    goto go_for_it;
	} else if (mot == LOOK) {
	    /* Repeat the long description and continue. */
	    if (++look_count <= 3) {
		puts("Sorry, but I am not allowed to give more detail.  I will repeat the\n"
		     "long description of your location.");
	    }
	    was_dark = false;  /* pretend it wasn't dark, so you won't fall into a pit */
	    visits[loc] = 0;
	    continue;
	} else if (mot == CAVE) {
	    if (loc < MIN_IN_CAVE) {
		puts("I can't see where the cave is, but hereabouts no stream can run on\n"
		     "the surface for long. I would try the stream.");
	    } else {
		puts("I need more detailed instructions to do that.");
	    }
	    continue;
	}
	oldoldloc = oldloc;
	oldloc = loc;
    go_for_it:
	/* Determine the next newloc. */
	if (determine_next_newloc(loc, &newloc, mot)) {
	    /* Player died trying to cross the troll bridge. */
	    oldoldloc = newloc;  /* if you are revived, you got across */
	    goto death;
	}
	continue;
    }
  pitch_dark:
    puts("You fell into a pit and broke every bone in your body!");
    oldoldloc = loc;
  death:
    /* When you die, newloc is undefined (often R_LIMBO) and oldloc is what
     * killed you. So we look at oldoldloc, the last place you were safe. */
    kill_the_player(oldoldloc);
    loc = oldloc = R_HOUSE;
    goto commence;
}

void dwarves_upset(void)
{
    puts("The resulting ruckus has awakened the dwarves.  There are now several\n"
	 "threatening little dwarves in the room with you!  Most of them throw\n"
	 "knives at you!  All of them get you!");
    quit();
}

int main()
{
    offer(0);
    lamp_limit = (hints[0].given ? 1000 : 330);  /* interesting */
    build_vocabulary();
    build_travel_table();
    build_object_table();
    simulate_an_adventure();
    return 0;
}

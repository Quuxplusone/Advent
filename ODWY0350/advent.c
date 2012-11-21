#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef Z_MACHINE
#ifdef SAVE_AND_RESTORE
int attempt_save(void) = "\t@save -> r0;\n";
int attempt_restore(void) =
    "\trestore Rmaybe;\n"
    "\tr0 = 0;\n" /* restore failed */
    "\t.RMaybe;\n"
    "\tr0 = 2;\n";  /* ok */

#endif /* SAVE_AND_RESTORE */
/* The Z-machine's "@random 42" instruction returns a value in the range 1..42. */
int ran(int range) =
    "\t@random r0 -> r0;\n"
    "\t@sub r0 0+1 -> r0;\n";
#else
int ran(int range) { return rand() % range; }
#endif /* Z_MACHINE */

#define SOFT_NL "\n"
#define SOFT_HYPHEN "-\n"
#define EMDASH(x) "\xE2\x80\x94"  /* U+2014 "EM DASH" */

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
    WordClass_None, WordClass_Motion, WordClass_Object,
    WordClass_Action, WordClass_Message
} WordClass;

struct HashEntry {
    char text[6];
    int meaning;
};

#define HASH_PRIME 1009
struct HashEntry hash_table[HASH_PRIME];

void new_word(const char *w, int m)
{
    int h = 0;
    for (const char *p = w; *p != '\0'; ++p) {
        h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].meaning != 0)
        h = (h+1) % HASH_PRIME;
    strcpy(hash_table[h].text, w);
    hash_table[h].meaning = m;
}

#define new_motion_word new_word
#define new_object_word new_word
#define new_action_word new_word
#define new_message_word new_word

int lookup(const char *w)
{
    int h = 0;
    for (const char *p = w; *p != '\0' && p != w+5; ++p) {
        h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].meaning != 0) {
        if (streq(w, hash_table[h].text)) return hash_table[h].meaning;
        h = (h+1) % HASH_PRIME;
    }
    return 0;
}
#undef HASH_PRIME

#define NOTHING 0

typedef enum {
    MIN_MOTION=100,
    ROAD=MIN_MOTION,ENTER,UPSTREAM,DOWNSTREAM,FOREST,FORWARD,
    BACK,VALLEY,STAIRS,OUT,HOUSE,GULLY,STREAM,ROCK,
    BED,CRAWL,COBBLES,IN,SURFACE,NOWHERE,DARK,PASSAGE,
    LOW,CANYON,AWKWARD,GIANT,VIEW,U,D,PIT,OUTDOORS,
    CRACK,STEPS,DOME,LEFT,RIGHT,HALL,JUMP,BARREN,
    OVER,ACROSS,E,W,N,S,NE,SE,SW,NW,DEBRIS,HOLE,
    WALL,BROKEN,Y2,CLIMB,LOOK,FLOOR,ROOM,SLIT,
    SLAB,XYZZY,DEPRESSION,ENTRANCE,PLUGH,SECRET,
    CAVE,CROSS,BEDQUILT,PLOVER,ORIENTAL,CAVERN,
    SHELL,RESERVOIR,OFFICE,FORK,
    MAX_MOTION=FORK
} MotionWord;

typedef enum {
    MIN_OBJ=200,
    KEYS=MIN_OBJ, LAMP, GRATE, GRATE_, CAGE, ROD, ROD2, TREADS, TREADS_,
    BIRD, RUSTY_DOOR, PILLOW, SNAKE, FISSURE, FISSURE_, TABLET, CLAM, OYSTER,
    MAG, DWARF, KNIFE, FOOD, BOTTLE, WATER, OIL, MIRROR, MIRROR_, PLANT,
    PLANT2, PLANT2_, STALACTITE, SHADOW, SHADOW_, AXE, DRAWINGS, PIRATE,
    DRAGON, DRAGON_, CHASM, CHASM_, TROLL, TROLL_, NO_TROLL, NO_TROLL_,
    BEAR, MESSAGE, GORGE, MACHINE, BATTERIES, MOSS,
    GOLD, DIAMONDS, SILVER, JEWELS, COINS, CHEST, EGGS,
    TRIDENT, VASE, EMERALD, PYRAMID, PEARL, RUG, RUG_, SPICES, CHAIN,
    MAX_OBJ=CHAIN
} ObjectWord;

typedef enum {
    MIN_ACTION=300,
    TAKE=MIN_ACTION, DROP, OPEN, CLOSE, ON, OFF, WAVE, CALM, GO,
    RELAX, POUR, EAT, DRINK, RUB, TOSS, WAKE, FEED, FILL, BREAK, BLAST,
    KILL, SAY, READ, FEEFIE, BRIEF, FIND, INVENTORY, SCORE,
#ifdef SAVE_AND_RESTORE
    SAVE, RESTORE,
#endif /* SAVE_AND_RESTORE */
    QUIT,
    MAX_ACTION=QUIT
} ActionWord;

typedef enum {
    MIN_MESSAGE=400,
    ABRA=MIN_MESSAGE, HELP, TREES, DIG, LOST, MIST, FUCK, STOP, INFO, SWIM,
    MAX_MESSAGE=SWIM
} MessageWord;

WordClass word_class(int word)
{
    if (word == NOTHING) {
        return WordClass_None;
    } else if (MIN_MOTION <= word && word <= MAX_MOTION) {
        return WordClass_Motion;
    } else if (MIN_OBJ <= word && word <= MAX_OBJ) {
        return WordClass_Object;
    } else if (MIN_ACTION <= word && word <= MAX_ACTION) {
        return WordClass_Action;
    } else if (MIN_MESSAGE <= word && word <= MAX_MESSAGE) {
        return WordClass_Message;
    } else {
        assert(false);
        return WordClass_None;
    }
}

const char ok[] = "OK.";  /* Woods' Fortran version didn't include the period, by the way. */
const char pitch_dark_msg[] = "It is now pitch dark.  If you proceed you will most likely fall into a pit.";

void build_vocabulary(void)
{
    new_motion_word("road", ROAD); new_motion_word("hill", ROAD);
    new_motion_word("enter", ENTER);
    new_motion_word("upstr", UPSTREAM);
    new_motion_word("downs", DOWNSTREAM);
    new_motion_word("fores", FOREST);
    new_motion_word("forwa", FORWARD); new_motion_word("conti", FORWARD);
    new_motion_word("onwar", FORWARD);
    new_motion_word("back", BACK); new_motion_word("retur", BACK);
    new_motion_word("retre", BACK);
    new_motion_word("valle", VALLEY);
    new_motion_word("stair", STAIRS);
    new_motion_word("out", OUT); new_motion_word("outsi", OUT);
    new_motion_word("exit", OUT); new_motion_word("leave", OUT);
    new_motion_word("build", HOUSE); new_motion_word("house", HOUSE);
    new_motion_word("gully", GULLY);
    new_motion_word("strea", STREAM);
    new_motion_word("rock", ROCK);
    new_motion_word("bed", BED);
    new_motion_word("crawl", CRAWL);
    new_motion_word("cobbl", COBBLES);
    new_motion_word("inwar", IN); new_motion_word("insid", IN);
    new_motion_word("in", IN);
    new_motion_word("surfa", SURFACE);
    new_motion_word("null", NOWHERE); new_motion_word("nowhe", NOWHERE);
    new_motion_word("dark", DARK);
    new_motion_word("passa", PASSAGE); new_motion_word("tunne", PASSAGE);
    new_motion_word("low", LOW);
    new_motion_word("canyo", CANYON);
    new_motion_word("awkwa", AWKWARD);
    new_motion_word("giant", GIANT);
    new_motion_word("view", VIEW);
    new_motion_word("upwar", U); new_motion_word("up", U);
    new_motion_word("u", U); new_motion_word("above", U);
    new_motion_word("ascen", U);
    new_motion_word("d", D); new_motion_word("downw", D);
    new_motion_word("down", D); new_motion_word("desce", D);
    new_motion_word("pit", PIT);
    new_motion_word("outdo", OUTDOORS);
    new_motion_word("crack", CRACK);
    new_motion_word("steps", STEPS);
    new_motion_word("dome", DOME);
    new_motion_word("left", LEFT);
    new_motion_word("right", RIGHT);
    new_motion_word("hall", HALL);
    new_motion_word("jump", JUMP);
    new_motion_word("barre", BARREN);
    new_motion_word("over", OVER);
    new_motion_word("acros", ACROSS);
    new_motion_word("east", E); new_motion_word("e", E);
    new_motion_word("west", W); new_motion_word("w", W);
    new_motion_word("north", N); new_motion_word("n", N);
    new_motion_word("south", S); new_motion_word("s", S);
    new_motion_word("ne", NE);
    new_motion_word("se", SE);
    new_motion_word("sw", SW);
    new_motion_word("nw", NW);
    new_motion_word("debri", DEBRIS);
    new_motion_word("hole", HOLE);
    new_motion_word("wall", WALL);
    new_motion_word("broke", BROKEN);
    new_motion_word("y2", Y2);
    new_motion_word("climb", CLIMB);
    new_motion_word("look", LOOK); new_motion_word("exami", LOOK);
    new_motion_word("touch", LOOK); new_motion_word("descr", LOOK);
    new_motion_word("floor", FLOOR);
    new_motion_word("room", ROOM);
    new_motion_word("slit", SLIT);
    new_motion_word("slab", SLAB); new_motion_word("slabr", SLAB);
    new_motion_word("xyzzy", XYZZY);
    new_motion_word("depre", DEPRESSION);
    new_motion_word("entra", ENTRANCE);
    new_motion_word("plugh", PLUGH);
    new_motion_word("secre", SECRET);
    new_motion_word("cave", CAVE);
    new_motion_word("cross", CROSS);
    new_motion_word("bedqu", BEDQUILT);
    new_motion_word("plove", PLOVER);
    new_motion_word("orien", ORIENTAL);
    new_motion_word("caver", CAVERN);
    new_motion_word("shell", SHELL);
    new_motion_word("reser", RESERVOIR);
    new_motion_word("main", OFFICE); new_motion_word("offic", OFFICE);
    new_motion_word("fork", FORK);

    new_object_word("key", KEYS); new_object_word("keys", KEYS);
    new_object_word("lamp", LAMP); new_object_word("lante", LAMP);
    new_object_word("headl", LAMP);
    new_object_word("grate", GRATE);
    new_object_word("cage", CAGE);
    new_object_word("rod", ROD);
    new_object_word("bird", BIRD);
    new_object_word("door", RUSTY_DOOR);
    new_object_word("pillo", PILLOW);
    new_object_word("snake", SNAKE);
    new_object_word("fissu", FISSURE);
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
    new_object_word("drawi", DRAWINGS);
    new_object_word("pirat", PIRATE);
    new_object_word("drago", DRAGON);
    new_object_word("chasm", CHASM);
    new_object_word("troll", TROLL);
    new_object_word("bear", BEAR);
    new_object_word("messa", MESSAGE);
    new_object_word("volca", GORGE); new_object_word("geyse", GORGE);
    new_object_word("vendi", MACHINE); new_object_word("machi", MACHINE);
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
    new_action_word("drop", DROP); new_action_word("relea", DROP);
    new_action_word("free", DROP); new_action_word("disca", DROP);
    new_action_word("dump", DROP);
    new_action_word("open", OPEN); new_action_word("unloc", OPEN);
    new_action_word("close", CLOSE); new_action_word("lock", CLOSE);
    new_action_word("light", ON); new_action_word("on", ON);
    new_action_word("extin", OFF); new_action_word("off", OFF);
    new_action_word("wave", WAVE); new_action_word("shake", WAVE);
    new_action_word("swing", WAVE);
    new_action_word("calm", CALM); new_action_word("placa", CALM);
    new_action_word("tame", CALM);
    new_action_word("walk", GO); new_action_word("run", GO);
    new_action_word("trave", GO); new_action_word("go", GO);
    new_action_word("proce", GO); new_action_word("explo", GO);
    new_action_word("goto", GO); new_action_word("follo", GO);
    new_action_word("turn", GO);
    new_action_word("nothi", RELAX);
    new_action_word("pour", POUR);
    new_action_word("eat", EAT); new_action_word("devou", EAT);
    new_action_word("drink", DRINK);
    new_action_word("rub", RUB);
    new_action_word("throw", TOSS); new_action_word("toss", TOSS);
    new_action_word("wake", WAKE); new_action_word("distu", WAKE);
    new_action_word("feed", FEED);
    new_action_word("fill", FILL);
    new_action_word("break", BREAK); new_action_word("smash", BREAK);
    new_action_word("shatt", BREAK);
    new_action_word("blast", BLAST); new_action_word("deton", BLAST);
    new_action_word("ignit", BLAST); new_action_word("blowu", BLAST);
    new_action_word("attac", KILL); new_action_word("kill", KILL);
    new_action_word("fight", KILL); new_action_word("hit", KILL);
    new_action_word("strik", KILL); new_action_word("slay", KILL);
    new_action_word("say", SAY);
    new_action_word("chant", SAY);
    new_action_word("sing", SAY);
    new_action_word("utter", SAY);
    new_action_word("mumbl", SAY);
    new_action_word("read", READ); new_action_word("perus", READ);
    new_action_word("fee", FEEFIE); new_action_word("fie", FEEFIE);
    new_action_word("foe", FEEFIE); new_action_word("foo", FEEFIE);
    new_action_word("fum", FEEFIE);
    new_action_word("brief", BRIEF);
    new_action_word("find", FIND); new_action_word("where", FIND);
    new_action_word("inven", INVENTORY);
    new_action_word("score", SCORE);
    new_action_word("quit", QUIT);
#ifdef SAVE_AND_RESTORE
    new_action_word("save", SAVE);
    new_action_word("resto", RESTORE);
#endif /* SAVE_AND_RESTORE */

    /* Finally, our vocabulary is rounded out by words like HELP, which
     * trigger the printing of fixed messages. */
    new_message_word("abra", ABRA);
    new_message_word("abrac", ABRA);
    new_message_word("opens", ABRA);
    new_message_word("sesam", ABRA);
    new_message_word("shaza", ABRA);
    new_message_word("hocus", ABRA);
    new_message_word("pocus", ABRA);
    new_message_word("help", HELP);
    new_message_word("?", HELP);
    new_message_word("tree", TREES);
    new_message_word("trees", TREES);
    new_message_word("dig", DIG);
    new_message_word("excav", DIG);
    new_message_word("lost", LOST);
    new_message_word("mist", MIST);
    new_message_word("fuck", FUCK);
    new_message_word("stop", STOP);
    new_message_word("info", INFO);
    new_message_word("infor", INFO);
    new_message_word("swim", SWIM);
}

/*========== Locations. ===================================================
 * This section corresponds to sections 18--62 in Knuth.
 */

typedef enum {
    R_INHAND = -1, R_LIMBO = 0,
    R_ROAD, R_HILL, R_HOUSE, R_VALLEY, R_FOREST, R_FOREST2, R_SLIT, R_OUTSIDE,
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
    R_SHELL, R_ARCHED, R_RAGGED, R_SAC, R_ANTE, R_WITT,
    R_BEDQUILT, R_SWISS, R_SOFT,
    R_E2PIT, R_W2PIT, R_EPIT, R_WPIT,
    R_NARROW, R_GIANT, R_BLOCK, R_IMMENSE,
    R_FALLS, R_INCLINE, R_ABOVEP, R_SJUNC,
    R_TITE, R_LOW, R_CRAWL, R_WINDOW,
    R_ORIENTAL, R_MISTY, R_ALCOVE, R_PLOVER, R_DARK,
    R_SLAB, R_ABOVER, R_MIRROR, R_RES,
    R_SCAN1, R_SCAN2, R_SCAN3, R_SECRET,
    R_WIDE, R_TIGHT, R_TALL, R_BOULDERS,
    R_SLOPING, R_SWSIDE,
    R_DEAD0, R_DEAD1, R_PIRATES_NEST, R_DEAD3, R_DEAD4, R_DEAD5,
    R_DEAD6, R_DEAD7, R_DEAD8, R_DEAD9, R_DEAD10, R_DEAD11,
    R_NESIDE, R_CORR, R_FORK, R_WARM, R_VIEW, R_CHAMBER,
    R_LIME, R_FBARR, R_BARR,
    R_NEEND, R_SWEND,

    R_NECK, R_LOSE, R_CLIMB, R_CHECK,
    R_THRU, R_DUCK, R_UPNOUT,
    R_DIDIT, MAX_LOC = R_DIDIT,
    R_PPASS, R_PDROP,
    R_TROLL,
    FIRST_REMARK
} Location;

typedef struct {
    MotionWord mot;
    int cond;
    Location dest;
} Instruction;

enum flagsBits {
    F_LIGHTED    = 0x001,
    F_WATER      = 0x004,
    F_CAVE_HINT  = 0x008,
    F_BIRD_HINT  = 0x010,
    F_SNAKE_HINT = 0x020,
    F_TWIST_HINT = 0x040,
    F_DARK_HINT  = 0x080,
    F_WITT_HINT  = 0x100
};

Instruction travels[740];
Instruction *start[MAX_LOC+2];
struct Place {
    const char *long_desc;
    const char *short_desc;
    unsigned int flags;
    struct ObjectData *objects;
    int visits;
};
struct Place places[MAX_LOC+1];

void make_loc(Instruction *q, Location x, const char *l, const char *s, unsigned int f)
{
    static int last_x = 0;
    assert(x == last_x + 1);
    last_x = x;

    places[x].long_desc = l;
    places[x].short_desc = s;
    places[x].flags = f;
    places[x].objects = NULL;
    places[x].visits = 0;
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
#define only_if_toting(t) (100 + (t-MIN_OBJ))
#define only_if_here(t) (200 + (t-MIN_OBJ))
#define unless_prop(t, p) (300 + (t-MIN_OBJ) + 100*p)

#define remark(n) (FIRST_REMARK + n)


void build_travel_table(void)
{
    Instruction *q = travels;
    static const char all_alike[] = "You are in a maze of twisty little passages, all alike.";
    static const char dead_end[] = "Dead end.";
    make_loc(q, R_ROAD,
             "You are standing at the end of a road before a small brick building." SOFT_NL
             "Around you is a forest.  A small stream flows out of the building and" SOFT_NL
             "down a gully.",
             "You're at end of road again.", F_LIGHTED | F_WATER);
    make_ins(W, R_HILL); ditto(U); ditto(ROAD);
    make_ins(E, R_HOUSE); ditto(IN); ditto(HOUSE); ditto(ENTER);
    make_ins(S, R_VALLEY); ditto(D); ditto(GULLY); ditto(STREAM); ditto(DOWNSTREAM);
    make_ins(N, R_FOREST); ditto(FOREST);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_HILL,
             "You have walked up a hill, still in the forest.  The road slopes back" SOFT_NL
             "down the other side of the hill.  There is a building in the distance.",
             "You're at hill in road.", F_LIGHTED);
    make_ins(ROAD, R_ROAD); ditto(HOUSE); ditto(FORWARD); ditto(E); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(N); ditto(S);
    make_loc(q, R_HOUSE,
             "You are inside a building, a well house for a large spring.",
             "You're inside building.", F_LIGHTED | F_WATER);
    make_ins(ENTER, R_ROAD); ditto(OUT); ditto(OUTDOORS); ditto(W);
    make_ins(XYZZY, R_DEBRIS);
    make_ins(PLUGH, R_Y2);
    make_ins(DOWNSTREAM, remark(17)); ditto(STREAM);
    make_loc(q, R_VALLEY,
             "You are in a valley in the forest beside a stream tumbling along a" SOFT_NL
             "rocky bed.",
             "You're in valley.", F_LIGHTED | F_WATER);
    make_ins(UPSTREAM, R_ROAD); ditto(HOUSE); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(U);
    make_ins(DOWNSTREAM, R_SLIT); ditto(S); ditto(D);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_FOREST,
             "You are in open forest, with a deep valley to one side.",
             "You're in forest.", F_LIGHTED);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(D);
    make_cond_ins(FOREST, 50, R_FOREST); ditto(FORWARD); ditto(N);
    make_ins(FOREST, R_FOREST2);
    make_ins(W, R_FOREST); ditto(S);
    make_loc(q, R_FOREST2,
             "You are in open forest near both a valley and a road.",
             "You're in forest.", F_LIGHTED);
    make_ins(ROAD, R_ROAD); ditto(N);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(W); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(S);
    make_loc(q, R_SLIT,
             "At your feet all the water of the stream splashes into a 2-inch slit" SOFT_NL
             "in the rock.  Downstream the streambed is bare rock.",
             "You're at slit in streambed.", F_LIGHTED | F_WATER);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_VALLEY); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W);
    make_ins(DOWNSTREAM, R_OUTSIDE); ditto(ROCK); ditto(BED); ditto(S);
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D);
    make_loc(q, R_OUTSIDE,
             "You are in a 20-foot depression floored with bare dirt.  Set into the" SOFT_NL
             "dirt is a strong steel grate mounted in concrete.  A dry streambed" SOFT_NL
             "leads into the depression.",
             "You're outside grate.", F_LIGHTED | F_CAVE_HINT);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(S);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_SLIT); ditto(GULLY); ditto(N);
    make_cond_ins(ENTER, unless_prop(GRATE, 0), R_INSIDE); ditto(IN); ditto(D);
    make_ins(ENTER, remark(1));
    make_loc(q, R_INSIDE,
             "You are in a small chamber beneath a 3x3 steel grate to the surface." SOFT_NL
             "A low crawl over cobbles leads inwards to the west.",
             "You're below the grate.", F_LIGHTED);
    make_cond_ins(OUT, unless_prop(GRATE, 0), R_OUTSIDE); ditto(U);
    make_ins(OUT, remark(1));
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(IN); ditto(W);
    make_ins(PIT, R_SPIT);
    make_ins(DEBRIS, R_DEBRIS);
    make_loc(q, R_COBBLES,
             "You are crawling over cobbles in a low passage.  There is a dim light" SOFT_NL
             "at the east end of the passage.",
             "You're in cobble crawl.", F_LIGHTED);
    make_ins(OUT, R_INSIDE); ditto(SURFACE); ditto(E);
    make_ins(IN, R_DEBRIS); ditto(DARK); ditto(W); ditto(DEBRIS);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_DEBRIS,
             "You are in a debris room filled with stuff washed in from the surface." SOFT_NL
             "A low wide passage with cobbles becomes plugged with mud and debris" SOFT_NL
             "here, but an awkward canyon leads upward and west.  A note on the wall" SOFT_NL
             "says \"MAGIC WORD XYZZY\".",
             "You're in debris room.", 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(PASSAGE); ditto(LOW); ditto(E);
    make_ins(CANYON, R_AWK); ditto(IN); ditto(U); ditto(W);
    make_ins(XYZZY, R_HOUSE);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_AWK,
             "You are in an awkward sloping east/west canyon.",
             NULL, 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(D, R_DEBRIS); ditto(E); ditto(DEBRIS);
    make_ins(IN, R_BIRD); ditto(U); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_BIRD,
             "You are in a splendid chamber thirty feet high. The walls are frozen" SOFT_NL
             "rivers of orange stone. An awkward canyon and a good passage exit" SOFT_NL
             "from east and west sides of the chamber.",
             "You're in bird chamber.", F_BIRD_HINT);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(DEBRIS, R_DEBRIS);
    make_ins(CANYON, R_AWK); ditto(E);
    make_ins(PASSAGE, R_SPIT); ditto(PIT); ditto(W);
    make_loc(q, R_SPIT,
             "At your feet is a small pit breathing traces of white mist. An east" SOFT_NL
             "passage ends here except for a small crack leading on.",
             "You're at top of small pit.", 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(DEBRIS, R_DEBRIS);
    make_ins(PASSAGE, R_BIRD); ditto(E);
    make_cond_ins(D, only_if_toting(GOLD), R_NECK); ditto(PIT); ditto(STEPS);
    /* good thing you weren't loaded down with GOLD */
    make_ins(D, R_EMIST);
    make_ins(CRACK, remark(14)); ditto(W);
    make_loc(q, R_EMIST,
             "You are at one end of a vast hall stretching forward out of sight to" SOFT_NL
             "the west.  There are openings to either side.  Nearby, a wide stone" SOFT_NL
             "staircase leads downward.  The hall is filled with wisps of white mist" SOFT_NL
             "swaying to and fro almost as if alive.  A cold wind blows up the" SOFT_NL
             "staircase.  There is a passage at the top of a dome behind you.",
             "You're in Hall of Mists.", 0);
    make_ins(LEFT, R_NUGGET); ditto(S);
    make_ins(FORWARD, R_EFISS); ditto(HALL); ditto(W);
    make_ins(STAIRS, R_HMK); ditto(D); ditto(N);
    make_cond_ins(U, only_if_toting(GOLD), remark(15)); ditto(PIT); ditto(STEPS); ditto(DOME); ditto(PASSAGE); ditto(E);
    make_ins(U, R_SPIT);
    make_ins(Y2, R_JUMBLE);
    make_loc(q, R_NUGGET,
             "This is a low room with a crude note on the wall. The note says," SOFT_NL
             "\"You won't get it up the steps\".",
             "You're in nugget of gold room.", 0);
    make_ins(HALL, R_EMIST); ditto(OUT); ditto(N);
    make_loc(q, R_EFISS,
             "You are on the east bank of a fissure slicing clear across the hall." SOFT_NL
             "The mist is quite thick here, and the fissure is too wide to jump.",
             "You're on east bank of fissure.", 0);
    make_ins(HALL, R_EMIST); ditto(E);
    make_cond_ins(JUMP, unless_prop(FISSURE, 0), remark(2));
    make_cond_ins(FORWARD, unless_prop(FISSURE, 1), R_LOSE);
    make_cond_ins(OVER, unless_prop(FISSURE, 1), remark(3)); ditto(ACROSS); ditto(W); ditto(CROSS);
    make_ins(OVER, R_WFISS);
    make_loc(q, R_WFISS,
             "You are on the west side of the fissure in the Hall of Mists.",
             NULL, 0);
    make_cond_ins(JUMP, unless_prop(FISSURE, 0), remark(2));
    make_cond_ins(FORWARD, unless_prop(FISSURE, 1), R_LOSE);
    make_cond_ins(OVER, unless_prop(FISSURE, 1), remark(3)); ditto(ACROSS); ditto(E); ditto(CROSS);
    make_ins(OVER, R_EFISS);
    make_ins(W, R_WMIST);
    make_ins(N, R_THRU);

    make_loc(q, R_WMIST,
             "You are at the west end of the Hall of Mists. A low wide crawl" SOFT_NL
             "continues west and another goes north. To the south is a little" SOFT_NL
             "passage 6 feet off the floor.",
             "You're at west end of Hall of Mists.", 0);
    make_ins(S, R_LIKE1); ditto(U); ditto(PASSAGE); ditto(CLIMB);
    make_ins(E, R_WFISS);
    make_ins(W, R_ELONG); ditto(CRAWL);
    make_ins(N, R_DUCK);

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
             "You are on the brink of a thirty-foot pit with a massive orange column" SOFT_NL
             "down one wall.  You could climb down here but you could not get back" SOFT_NL
             "up.  The maze continues at this level.",
             "You're at brink of pit.", 0);
    make_ins(D, R_BIRD); ditto(CLIMB);
    make_ins(W, R_LIKE10);
    make_ins(S, R_DEAD8);
    make_ins(N, R_LIKE12);
    make_ins(E, R_LIKE13);
    make_loc(q, R_ELONG,
             "You are at the east end of a very long hall apparently without side" SOFT_NL
             "chambers.  To the east a low wide crawl slants up.  To the north a" SOFT_NL
             "round two-foot hole slants down.",
             "You're at east end of long hall.", 0);
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL);
    make_ins(W, R_WLONG);
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE);
    make_loc(q, R_WLONG,
             "You are at the west end of a very long featureless hall.  The hall" SOFT_NL
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
        "You are in the Hall of the Mountain King, with passages off in all" SOFT_NL
        "directions.",
        "You're in Hall of Mt King.", F_SNAKE_HINT);
    make_ins(STAIRS, R_EMIST); ditto(U); ditto(E);
    /* I suppose our adventurer must be walking on the ceiling! */
    make_cond_ins(N, unless_prop(SNAKE, 0), R_NS); ditto(LEFT);
    make_cond_ins(S, unless_prop(SNAKE, 0), R_SOUTH); ditto(RIGHT);
    make_cond_ins(W, unless_prop(SNAKE, 0), R_WEST); ditto(FORWARD);
    make_ins(N, remark(16));
    make_cond_ins(SW, 35, R_SECRET);
    make_cond_ins(SW, only_if_here(SNAKE), remark(16));
    make_ins(SECRET, R_SECRET);
    make_loc(q, R_WEST,
        "You are in the west side chamber of the Hall of the Mountain King." SOFT_NL
        "A passage continues west and up here.",
        "You're in west side chamber.", 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(E);
    make_ins(W, R_CROSS); ditto(U);
    make_loc(q, R_SOUTH,
        "You are in the south side chamber.",
        NULL, 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(N);
    make_loc(q, R_NS,
             "You are in a low N/S passage at a hole in the floor.  The hole goes" SOFT_NL
             "down to an E/W passage.",
             "You're in N/S passage.", 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(S);
    make_ins(N, R_Y2); ditto(Y2);
    make_ins(D, R_DIRTY); ditto(HOLE);
    make_loc(q, R_Y2,
             "You are in a large room, with a passage to the south, a passage to the" SOFT_NL
             "west, and a wall of broken rock to the east.  There is a large \"Y2\" on" SOFT_NL
             "a rock in the room's center.",
             "You're at \"Y2\".", 0);
    make_ins(PLUGH, R_HOUSE);
    make_ins(S, R_NS);
    make_ins(E, R_JUMBLE); ditto(WALL); ditto(BROKEN);
    make_ins(W, R_WINDOE);
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP);
    make_ins(PLOVER, R_PLOVER);
    make_loc(q, R_JUMBLE,
             "You are in a jumble of rock, with cracks everywhere.",
             NULL, 0);
    make_ins(D, R_Y2); ditto(Y2);
    make_ins(U, R_EMIST);
    make_loc(q, R_WINDOE,
             "You're at a low window overlooking a huge pit, which extends up out of" SOFT_NL
             "sight.  A floor is indistinctly visible over 50 feet below.  Traces of" SOFT_NL
             "white mist cover the floor of the pit, becoming thicker to the right." SOFT_NL
             "Marks in the dust around the window would seem to indicate that" SOFT_NL
             "someone has been here recently.  Directly across the pit from you and" SOFT_NL
             "25 feet away there is a similar window looking into a lighted room." SOFT_NL
             "A shadowy figure can be seen there peering back at you.",
             "You're at window on pit.", 0);
    make_ins(E, R_Y2); ditto(Y2);
    make_ins(JUMP, R_NECK);
    make_loc(q, R_DIRTY,
             "You are in a dirty broken passage.  To the east is a crawl.  To the" SOFT_NL
             "west is a large passage.  Above you is a hole to another passage.",
             "You're in dirty passage.", 0);
    make_ins(E, R_CLEAN); ditto(CRAWL);
    make_ins(U, R_NS); ditto(HOLE);
    make_ins(W, R_DUSTY);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_loc(q, R_CLEAN,
             "You are on the brink of a small clean climbable pit.  A crawl leads" SOFT_NL
             "west.",
             "You're by a clean pit.", 0);
    make_ins(W, R_DIRTY); ditto(CRAWL);
    make_ins(D, R_WET); ditto(PIT); ditto(CLIMB);
    make_loc(q, R_WET,
             "You are in the bottom of a small pit with a little stream, which" SOFT_NL
             "enters and exits through tiny slits.",
             "You're in pit by stream.", F_WATER);
    make_ins(CLIMB, R_CLEAN); ditto(U); ditto(OUT);
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D); ditto(UPSTREAM); ditto(DOWNSTREAM);
    make_loc(q, R_DUSTY,
             "You are in a large room full of dusty rocks.  There is a big hole in" SOFT_NL
             "the floor.  There are cracks everywhere, and a passage leading east.",
             "You're in dusty rock room.", 0);
    make_ins(E, R_DIRTY); ditto(PASSAGE);
    make_ins(D, R_COMPLEX); ditto(HOLE); ditto(FLOOR);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_loc(q, R_COMPLEX,
             "You are at a complex junction.  A low hands-and-knees passage from the" SOFT_NL
             "north joins a higher crawl from the east to make a walking passage" SOFT_NL
             "going west.  There is also a large room above.  The air is damp here.",
             "You're at complex junction.", 0);
    make_ins(U, R_DUSTY); ditto(CLIMB); ditto(ROOM);
    make_ins(W, R_BEDQUILT); ditto(BEDQUILT);
    make_ins(N, R_SHELL); ditto(SHELL);
    make_ins(E, R_ANTE);
    make_loc(q, R_SHELL,
             "You're in a large room carved out of sedimentary rock.  The floor" SOFT_NL
             "and walls are littered with bits of shells embedded in the stone." SOFT_NL
             "A shallow passage proceeds downward, and a somewhat steeper one" SOFT_NL
             "leads up.  A low hands-and-knees passage enters from the south.",
             "You're in Shell Room.", 0);
    make_ins(U, R_ARCHED); ditto(HALL);
    make_ins(D, R_RAGGED);
    make_cond_ins(S, only_if_toting(CLAM), remark(4));
    make_cond_ins(S, only_if_toting(OYSTER), remark(5));
    make_ins(S, R_COMPLEX);
    make_loc(q, R_ARCHED,
             "You are in an arched hall.  A coral passage once continued up and east" SOFT_NL
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
             "You are in an anteroom leading to a large passage to the east.  Small" SOFT_NL
             "passages go west and up.  The remnants of recent digging are evident." SOFT_NL
             "A sign in midair here says \"CAVE UNDER CONSTRUCTION BEYOND THIS POINT." SOFT_NL
             "PROCEED AT OWN RISK.  [WITT CONSTRUCTION COMPANY]\"",
             "You're in anteroom.", 0);
    make_ins(U, R_COMPLEX);
    make_ins(W, R_BEDQUILT);
    make_ins(E, R_WITT);
    make_loc(q, R_WITT,
             "You are at Witt's End.  Passages lead off in *ALL* directions.",
             "You're at Witt's End.", F_WITT_HINT);
    make_cond_ins(E, 95, remark(6)); ditto(N); ditto(S);
    ditto(NE); ditto(SE); ditto(SW); ditto(NW); ditto(U); ditto(D);
    make_ins(E, R_ANTE);
    make_ins(W, remark(7));

    make_loc(q, R_BEDQUILT,
             "You are in Bedquilt, a long east/west passage with holes everywhere." SOFT_NL
             "To explore at random select north, south, up, or down.",
             "You're in Bedquilt.", 0);
    make_ins(E, R_COMPLEX);
    make_ins(W, R_SWISS);
    make_cond_ins(S, 80, remark(6));
    make_ins(SLAB, R_SLAB);
    make_cond_ins(U, 80, remark(6));
    make_cond_ins(U, 50, R_ABOVEP);
    make_ins(U, R_DUSTY);
    make_cond_ins(N, 60, remark(6));
    make_cond_ins(N, 75, R_LOW);
    make_ins(N, R_SJUNC);
    make_cond_ins(D, 80, remark(6));
    make_ins(D, R_ANTE);

    make_loc(q, R_SWISS,
             "You are in a room whose walls resemble Swiss cheese.  Obvious passages" SOFT_NL
             "go west, east, NE, and NW.  Part of the room is occupied by a large" SOFT_NL
             "bedrock block.",
             "You're in Swiss cheese room.", 0);
    make_ins(NE, R_BEDQUILT);
    make_ins(W, R_E2PIT);
    make_cond_ins(S, 80, remark(6));
    make_ins(CANYON, R_TALL);
    make_ins(E, R_SOFT);
    make_cond_ins(NW, 50, remark(6));
    make_ins(ORIENTAL, R_ORIENTAL);
    make_loc(q, R_SOFT,
             "You are in the Soft Room.  The walls are covered with heavy curtains," SOFT_NL
             "the floor with a thick pile carpet.  Moss covers the ceiling.",
             "You're in Soft Room.", 0);
    make_ins(W, R_SWISS); ditto(OUT);
    make_loc(q, R_E2PIT,
             "You are at the east end of the Twopit Room.  The floor here is" SOFT_NL
             "littered with thin rock slabs, which make it easy to descend the pits." SOFT_NL
             "There is a path here bypassing the pits to connect passages from east" SOFT_NL
             "and west.  There are holes all over, but the only big one is on the" SOFT_NL
             "wall directly over the west pit where you can't get to it.",
             "You're at east end of Twopit Room.", 0);
    make_ins(E, R_SWISS);
    make_ins(W, R_W2PIT); ditto(ACROSS);
    make_ins(D, R_EPIT); ditto(PIT);
    make_loc(q, R_W2PIT,
             "You are at the west end of the Twopit Room.  There is a large hole in" SOFT_NL
             "the wall above the pit at this end of the room.",
             "You're at west end of Twopit Room.", 0);
    make_ins(E, R_E2PIT); ditto(ACROSS);
    make_ins(W, R_SLAB); ditto(SLAB);
    make_ins(D, R_WPIT); ditto(PIT);
    make_ins(HOLE, remark(8));
    make_loc(q, R_EPIT,
             "You are at the bottom of the eastern pit in the Twopit Room.  There is" SOFT_NL
             "a small pool of oil in one corner of the pit.",
             "You're in east pit.", 0);
    make_ins(U, R_E2PIT); ditto(OUT);
    make_loc(q, R_WPIT,
             "You are at the bottom of the western pit in the Twopit Room.  There is" SOFT_NL
             "a large hole in the wall about 25 feet above you.",
             "You're in west pit.", 0);
    make_ins(U, R_W2PIT); ditto(OUT);
    make_cond_ins(CLIMB, unless_prop(PLANT, 2), R_CHECK);
    make_ins(CLIMB, R_CLIMB);
    make_loc(q, R_NARROW,
             "You are in a long, narrow corridor stretching out of sight to the" SOFT_NL
             "west.  At the eastern end is a hole through which you can see a" SOFT_NL
             "profusion of leaves.",
             "You're in narrow corridor.", 0);
    make_ins(D, R_WPIT); ditto(CLIMB); ditto(E);
    make_ins(JUMP, R_NECK);
    make_ins(W, R_GIANT); ditto(GIANT);
    make_loc(q, R_GIANT,
             "You are in the Giant Room.  The ceiling here is too high up for your" SOFT_NL
             "lamp to show it.  Cavernous passages lead east, north, and south.  On" SOFT_NL
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
    make_cond_ins(N, unless_prop(RUSTY_DOOR, 0), R_FALLS); ditto(ENTER); ditto(CAVERN);
    make_ins(N, remark(9));
    make_loc(q, R_FALLS,
             "You are in a magnificent cavern with a rushing stream, which cascades" SOFT_NL
             "over a sparkling waterfall into a roaring whirlpool that disappears" SOFT_NL
             "through a hole in the floor.  Passages exit to the south and west.",
             "You're in cavern with waterfall.", F_WATER);
    make_ins(S, R_IMMENSE); ditto(OUT);
    make_ins(GIANT, R_GIANT);
    make_ins(W, R_INCLINE);
    make_loc(q, R_INCLINE,
             "You are at the top of a steep incline above a large room.  You could" SOFT_NL
             "climb down here, but you would not be able to climb up.  There is a" SOFT_NL
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
             "You are in a secret canyon at a junction of three canyons, bearing" SOFT_NL
             "north, south, and SE.  The north one is as tall as the other two" SOFT_NL
             "combined.",
             "You're at junction of three secret canyons.", 0);
    /* In Crowther's original, this was pretty much the edge of the cave. Going UP here
     * would take you on a one-way trip back to the dusty rock room. Woods replaced
     * that connection with a northerly passage to R_WINDOW. */
    make_ins(SE, R_BEDQUILT);
    make_ins(S, R_ABOVEP);
    make_ins(N, R_WINDOW);
    make_loc(q, R_TITE,
             "A large stalactite extends from the roof and almost reaches the floor" SOFT_NL
             "below.  You could climb down it, and jump from it to the floor, but" SOFT_NL
             "having done so you would be unable to reach it to climb back up.",
             "You're at top of stalactite.", 0);
    make_ins(N, R_ABOVEP);
    make_cond_ins(D, 40, R_LIKE6); ditto(JUMP); ditto(CLIMB);
    make_cond_ins(D, 50, R_LIKE9);
    make_ins(D, R_LIKE4);
    make_loc(q, R_LOW,
             "You are in a large low room.  Crawls lead north, SE, and SW.",
             NULL, 0);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_ins(SW, R_SLOPING);
    make_ins(N, R_CRAWL);
    make_ins(SE, R_ORIENTAL); ditto(ORIENTAL);
    make_loc(q, R_CRAWL,
             "Dead end crawl.",
             NULL, 0);
    make_ins(S, R_LOW); ditto(CRAWL); ditto(OUT);
    make_loc(q, R_WINDOW,
             "You're at a low window overlooking a huge pit, which extends up out of" SOFT_NL
             "sight.  A floor is indistinctly visible over 50 feet below.  Traces of" SOFT_NL
             "white mist cover the floor of the pit, becoming thicker to the left." SOFT_NL
             "Marks in the dust around the window would seem to indicate that" SOFT_NL
             "someone has been here recently.  Directly across the pit from you and" SOFT_NL
             "25 feet away there is a similar window looking into a lighted room." SOFT_NL
             "A shadowy figure can be seen there peering back at you.",
             "You're at window on pit.", 0);
    make_ins(W, R_SJUNC);
    make_ins(JUMP, R_NECK);
    make_loc(q, R_ORIENTAL,
             "This is the Oriental Room.  Ancient oriental cave drawings cover the" SOFT_NL
             "walls.  A gently sloping passage leads upward to the north, another" SOFT_NL
             "passage leads SE, and a hands-and-knees crawl leads west.",
             "You're in Oriental Room.", 0);
    make_ins(SE, R_SWISS);
    make_ins(W, R_LOW); ditto(CRAWL);
    make_ins(U, R_MISTY); ditto(N); ditto(CAVERN);
    make_loc(q, R_MISTY,
             "You are following a wide path around the outer edge of a large cavern." SOFT_NL
             "Far below, through a heavy white mist, strange splashing noises can be" SOFT_NL
             "heard.  The mist rises up through a fissure in the ceiling.  The path" SOFT_NL
             "exits to the south and west.",
             "You're in misty cavern.", 0);
    make_ins(S, R_ORIENTAL); ditto(ORIENTAL);
    make_ins(W, R_ALCOVE);
    make_loc(q, R_ALCOVE,
             "You are in an alcove.  A small NW path seems to widen after a short" SOFT_NL
             "distance.  An extremely tight tunnel leads east.  It looks like a very" SOFT_NL
             "tight squeeze.  An eerie light can be seen at the other end.",
             "You're in alcove.", F_DARK_HINT);
    make_ins(NW, R_MISTY); ditto(CAVERN);
    make_ins(E, R_PPASS); ditto(PASSAGE);
    make_ins(E, R_PLOVER);  /* never performed, but seen by "BACK" */
    make_loc(q, R_PLOVER,
             "You're in a small chamber lit by an eerie green light.  An extremely" SOFT_NL
             "narrow tunnel exits to the west.  A dark corridor leads NE.",
             "You're in Plover Room.", F_LIGHTED | F_DARK_HINT);
    make_ins(W, R_PPASS); ditto(PASSAGE); ditto(OUT);
    make_ins(W, R_ALCOVE);  /* never performed, but seen by "BACK" */
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP);
    make_ins(PLOVER, R_Y2);
    make_ins(NE, R_DARK); ditto(DARK);
    make_loc(q, R_DARK,
             "You're in the Dark-Room.  A corridor leading south is the only exit.",
             "You're in Dark-Room.", F_DARK_HINT);
    make_ins(S, R_PLOVER); ditto(PLOVER); ditto(OUT);
    make_loc(q, R_SLAB,
             "You are in a large low circular chamber whose floor is an immense slab" SOFT_NL
             "fallen from the ceiling (Slab Room).  There once were large passages" SOFT_NL
             "to the east and west, but they are now filled with boulders.  Low" SOFT_NL
             "small passages go north and south, and the south one quickly bends" SOFT_NL
             "east around the boulders.",
             "You're in Slab Room.", 0);
    make_ins(S, R_W2PIT);
    make_ins(U, R_ABOVER); ditto(CLIMB);
    make_ins(N, R_BEDQUILT);
    make_loc(q, R_ABOVER,
             "You are in a secret N/S canyon above a large room.",
             NULL, 0);
    make_ins(D, R_SLAB); ditto(SLAB);
    make_cond_ins(S, unless_prop(DRAGON, 0), R_SCAN2);
    make_ins(S, R_SCAN1);
    make_ins(N, R_MIRROR);
    make_ins(RESERVOIR, R_RES);
    make_loc(q, R_MIRROR,
             "You are in a north/south canyon about 25 feet across.  The floor is" SOFT_NL
             "covered by white mist seeping in from the north.  The walls extend" SOFT_NL
             "upward for well over 100 feet.  Suspended from some unseen point far" SOFT_NL
             "above you, an enormous two-sided mirror is hanging parallel to and" SOFT_NL
             "midway between the canyon walls.  (The mirror is obviously provided" SOFT_NL
             "for the use of the dwarves, who as you know are extremely vain.)" SOFT_NL
             "A small window can be seen in either wall, some fifty feet up.",
             "You're in mirror canyon.", 0);
    make_ins(S, R_ABOVER);
    make_ins(N, R_RES); ditto(RESERVOIR);
    make_loc(q, R_RES,
             "You are at the edge of a large underground reservoir.  An opaque cloud" SOFT_NL
             "of white mist fills the room and rises rapidly upward.  The lake is" SOFT_NL
             "fed by a stream, which tumbles out of a hole in the wall about 10 feet" SOFT_NL
             "overhead and splashes noisily into the water somewhere within the" SOFT_NL
             "mist.  The only passage goes back toward the south.",
             "You're at reservoir.", F_WATER);
    make_ins(S, R_MIRROR); ditto(OUT);

    /* R_SCAN1 and R_SCAN3 are the rooms the player sees when entering the
     * secret canyon from the north and the east, respectively. The dragon
     * blocks different exits in each room (and items dropped at one end of
     * the canyon are not visible or accessible from the other end).
     * Once the dragon has been vanquished, R_SCAN2 replaces both rooms.
     */
    make_loc(q, R_SCAN1,
             "You are in a secret canyon that exits to the north and east.",
             NULL, 0);
    make_ins(N, R_ABOVER); ditto(OUT);
    make_ins(E, remark(10)); ditto(FORWARD);
    make_loc(q, R_SCAN2,
             places[R_SCAN1].long_desc,
             NULL, 0);
    make_ins(N, R_ABOVER);
    make_ins(E, R_SECRET);
    make_loc(q, R_SCAN3,
             places[R_SCAN1].long_desc,
             NULL, 0);
    make_ins(E, R_SECRET); ditto(OUT);
    make_ins(N, remark(10)); ditto(FORWARD);

    make_loc(q, R_SECRET,
             "You are in a secret canyon which here runs E/W.  It crosses over a" SOFT_NL
             "very tight canyon 15 feet below.  If you go down you may not be able" SOFT_NL
             "to get back up.",
             "You're in secret E/W canyon above tight canyon.", 0);
    make_ins(E, R_HMK);
    make_cond_ins(W, unless_prop(DRAGON, 0), R_SCAN2);
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
             "You are in a tall E/W canyon.  A low tight crawl goes 3 feet north and" SOFT_NL
             "seems to open up.",
             "You're in tall E/W canyon.", 0);
    make_ins(E, R_WIDE);
    make_ins(W, R_BOULDERS);
    make_ins(N, R_SWISS); ditto(CRAWL);
    make_loc(q, R_BOULDERS,
             "The canyon runs into a mass of boulders " EMDASH("--") " dead end.",
             NULL, 0);
    make_ins(S, R_TALL);
    make_loc(q, R_SLOPING,
             "You are in a long winding corridor sloping out of sight in both" SOFT_NL
             "directions.",
             "You're in sloping corridor.", 0);
    make_ins(D, R_LOW);
    make_ins(U, R_SWSIDE);
    make_loc(q, R_SWSIDE,
             "You are on one side of a large, deep chasm.  A heavy white mist rising" SOFT_NL
             "up from below obscures all view of the far side.  A SW path leads away" SOFT_NL
             "from the chasm into a winding corridor.",
             "You're on SW side of chasm.", 0);
    make_ins(SW, R_SLOPING);
    make_cond_ins(OVER, only_if_here(TROLL), remark(11)); ditto(ACROSS); ditto(CROSS); ditto(NE);
    make_cond_ins(OVER, unless_prop(CHASM, 0), remark(12));
    make_ins(OVER, R_TROLL);
    make_cond_ins(JUMP, unless_prop(CHASM, 0), R_LOSE);
    make_ins(JUMP, remark(2));
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
             "You are on the far side of the chasm.  A NE path leads away from the" SOFT_NL
             "chasm on this side.",
             "You're on NE side of chasm.", 0);
    make_ins(NE, R_CORR);
    make_cond_ins(OVER, only_if_here(TROLL), remark(11)); ditto(ACROSS); ditto(CROSS); ditto(SW);
    make_ins(OVER, R_TROLL);
    make_ins(JUMP, remark(2));
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_CORR,
             "You're in a long east/west corridor.  A faint rumbling noise can be" SOFT_NL
             "heard in the distance.",
             "You're in corridor.", 0);
    make_ins(W, R_NESIDE);
    make_ins(E, R_FORK); ditto(FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_FORK,
             "The path forks here.  The left fork leads northeast.  A dull rumbling" SOFT_NL
             "seems to get louder in that direction.  The right fork leads southeast" SOFT_NL
             "down a gentle slope.  The main corridor enters from the west.",
             "You're at fork in path.", 0);
    make_ins(W, R_CORR);
    make_ins(NE, R_WARM); ditto(LEFT);
    make_ins(SE, R_LIME); ditto(RIGHT); ditto(D);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_WARM,
             "The walls are quite warm here.  From the north can be heard a steady" SOFT_NL
             "roar, so loud that the entire cave seems to be trembling.  Another" SOFT_NL
             "passage leads south, and a low crawl goes east.",
             "You're at junction with warm walls.", 0);
    make_ins(S, R_FORK); ditto(FORK);
    make_ins(N, R_VIEW); ditto(VIEW);
    make_ins(E, R_CHAMBER); ditto(CRAWL);
    make_loc(q, R_VIEW,
             "You are on the edge of a breath-taking view.  Far below you is an" SOFT_NL
             "active volcano, from which great gouts of molten lava come surging" SOFT_NL
             "out, cascading back down into the depths.  The glowing rock fills the" SOFT_NL
             "farthest reaches of the cavern with a blood-red glare, giving every" SOFT_HYPHEN
             "thing an eerie, macabre appearance.  The air is filled with flickering" SOFT_NL
             "sparks of ash and a heavy smell of brimstone.  The walls are hot to" SOFT_NL
             "the touch, and the thundering of the volcano drowns out all other" SOFT_NL
             "sounds.  Embedded in the jagged roof far overhead are myriad twisted" SOFT_NL
             "formations, composed of pure white alabaster, which scatter the murky" SOFT_NL
             "light into sinister apparitions upon the walls.  To one side is a deep" SOFT_NL
             "gorge, filled with a bizarre chaos of tortured rock that seems to have" SOFT_NL
             "been crafted by the Devil himself.  An immense river of fire crashes" SOFT_NL
             "out from the depths of the volcano, burns its way through the gorge," SOFT_NL
             "and plummets into a bottomless pit far off to your left.  To the" SOFT_NL
             "right, an immense geyser of blistering steam erupts continuously" SOFT_NL
             "from a barren island in the center of a sulfurous lake, which bubbles" SOFT_NL
             "ominously.  The far right wall is aflame with an incandescence of its" SOFT_NL
             "own, which lends an additional infernal splendor to the already" SOFT_NL
             "hellish scene.  A dark, foreboding passage exits to the south.",
             "You're at breath-taking view.", F_LIGHTED);
    make_ins(S, R_WARM); ditto(PASSAGE); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(D, remark(13)); ditto(JUMP);
    make_loc(q, R_CHAMBER,
             "You are in a small chamber filled with large boulders.  The walls are" SOFT_NL
             "very warm, causing the air in the room to be almost stifling from the" SOFT_NL
             "heat.  The only exit is a crawl heading west, through which is coming" SOFT_NL
             "a low rumbling.",
             "You're in chamber of boulders.", 0);
    make_ins(W, R_WARM); ditto(OUT); ditto(CRAWL);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_LIME,
             "You are walking along a gently sloping north/south passage lined with" SOFT_NL
             "oddly shaped limestone formations.",
             "You're in limestone passage.", 0);
    make_ins(N, R_FORK); ditto(U); ditto(FORK);
    make_ins(S, R_FBARR); ditto(D); ditto(BARREN);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_FBARR,
             "You are standing at the entrance to a large, barren room.  A sign" SOFT_NL
             "posted above the entrance reads:  \"CAUTION!  BEAR IN ROOM!\"",
             "You're in front of barren room.", 0);
    make_ins(W, R_LIME); ditto(U);
    make_ins(FORK, R_FORK);
    make_ins(E, R_BARR); ditto(IN); ditto(BARREN); ditto(ENTER);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_BARR,
             "You are inside a barren room.  The center of the room is completely" SOFT_NL
             "empty except for some dust.  Marks in the dust lead away toward the" SOFT_NL
             "far end of the room.  The only exit is the way you came in.",
             "You're in barren room.", 0);
    make_ins(W, R_FBARR); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    /* The end-game repository. */
    make_loc(q, R_NEEND,
             "You are at the northeast end of an immense room, even larger than the" SOFT_NL
             "Giant Room.  It appears to be a repository for the \"Adventure\"" SOFT_NL
             "program.  Massive torches far overhead bathe the room with smoky" SOFT_NL
             "yellow light.  Scattered about you can be seen a pile of bottles (all" SOFT_NL
             "of them empty), a nursery of young beanstalks murmuring quietly, a bed" SOFT_NL
             "of oysters, a bundle of black rods with rusty stars on their ends, and" SOFT_NL
             "a collection of brass lanterns.  Off to one side a great many dwarves" SOFT_NL
             "are sleeping on the floor, snoring loudly.  A sign nearby reads: \"DO" SOFT_NL
             "NOT DISTURB THE DWARVES!\"  An immense mirror is hanging against one" SOFT_NL
             "wall, and stretches to the other end of the room, where various other" SOFT_NL
             "sundry objects can be glimpsed dimly in the distance.",
             "You're at NE end.", F_LIGHTED);
    make_ins(SW, R_SWEND);
    /* The following description has several minor differences from Woods' original.
     * Woods' line breaks come after "A" on lines 4 and 5, "large" on line 6, and
     * "vault" on line 7. Knuth's "that reads" corresponds to Woods' "which reads";
     * presumably Knuth changed it to avoid ugly repetition, and I agree. */
    make_loc(q, R_SWEND,
             "You are at the southwest end of the repository.  To one side is a pit" SOFT_NL
             "full of fierce green snakes.  On the other side is a row of small" SOFT_NL
             "wicker cages, each of which contains a little sulking bird.  In one" SOFT_NL
             "corner is a bundle of black rods with rusty marks on their ends." SOFT_NL
             "A large number of velvet pillows are scattered about on the floor." SOFT_NL
             "A vast mirror stretches off to the northeast.  At your feet is a" SOFT_NL
             "large steel grate, next to which is a sign that reads, \"TREASURE" SOFT_NL
             "VAULT.  KEYS IN MAIN OFFICE.\"",
             "You're at SW end.", F_LIGHTED);
    make_ins(NE, R_NEEND);
    make_ins(D, remark(1));  /* You can't go through a locked steel grate! */

    /* The following pseudo-locations have "forced" movement.
     * In such cases we don't ask you for input; we assume that you have told
     * us to force another instruction through. For example, if you try to
     * JUMP across the fissure (R_EFISS), the instruction there sends you to
     * R_LOSE, which prints the room description ("You didn't make it.") and
     * immediately sends you to R_LIMBO, i.e., death.
     * Crowther (and therefore Woods and therefore Knuth) implemented several
     * responses as pseudo-locations; for example, "The dome is unclimbable"
     * and "The crack is far too small for you to follow". For the ones that
     * behave indistinguishably from remarks, I've converted them to remarks.
     */
    make_loc(q, R_NECK, "You are at the bottom of the pit with a broken neck.", NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_LOSE, "You didn't make it.", NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_CLIMB, "You clamber up the plant and scurry through the hole at the top.", NULL, 0);
    make_ins(0, R_NARROW);
    /* Typing CLIMB from the bottom of the west pit triggers a clever bit
     * of gymnastics. We want to branch three ways on the state of the
     * plant (too small to climb; "up the plant and out of the pit"; and
     * all the way up the beanstalk). But the only operation available to
     * us is "travel if objs(PLANT).prop is NOT x". So R_WPIT's instruction
     * brings us to R_CHECK if objs(PLANT).prop != 2, and R_CHECK dispatches
     * to one of the two non-narrow-corridor locations. */
    make_loc(q, R_CHECK, NULL, NULL, 0);
    make_cond_ins(0, unless_prop(PLANT, 1), R_UPNOUT);
    make_ins(0, R_DIDIT);
    make_loc(q, R_THRU,
             "You have crawled through a very low wide passage parallel to and north" SOFT_NL
             "of the Hall of Mists.",
             NULL, 0);
    make_ins(0, R_WMIST);
    make_loc(q, R_DUCK, places[R_THRU].long_desc, NULL, 0);
    make_ins(0, R_WFISS);
    make_loc(q, R_UPNOUT,
             "There is nothing here to climb.  Use \"up\" or \"out\" to leave the pit.",
             NULL, 0);
    make_ins(0, R_WPIT);
    make_loc(q, R_DIDIT, "You have climbed up the plant and out of the pit.", NULL, 0);
    make_ins(0, R_W2PIT);

    /* The remaining "locations" R_PPASS, R_PDROP, and R_TROLL are special. */
    start[R_PPASS] = q;
}

bool is_forced(Location loc)
{
    switch (loc) {
        case R_NECK:
        case R_LOSE:
        case R_CLIMB:
        case R_CHECK:
        case R_THRU:
        case R_DUCK:
        case R_UPNOUT:
        case R_DIDIT:
            return true;
        default:
            return false;
    }
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
    const char *desc[4];  /* .prop ranges from 0 to 3 */
} objs_[MAX_OBJ+1 - MIN_OBJ];
#define objs(t) objs_[(t)-MIN_OBJ]

int holding_count;  /* how many objects have objs(t).place < 0? */
Location last_knife_loc = R_LIMBO;
int tally = 15;  /* treasures awaiting you */
int lost_treasures;  /* treasures that you won't find */

#define toting(t) (objs(t).place < 0)
#define there(t, loc) (objs(t).place == (loc))

/* Return true if t is a treasure. Notice that RUG_ (the other half
 * of the schizoid rug) does not need to be a treasure. You can
 * never be carrying it; the pirate can't steal it; and its prop
 * value is irrelevant (we use the prop value of the base object RUG).
 */
bool is_treasure(ObjectWord t)
{
    switch (t) {
        case GOLD: case DIAMONDS: case SILVER: case JEWELS:
        case COINS: case CHEST: case EGGS: case TRIDENT:
        case VASE: case EMERALD: case PYRAMID: case PEARL:
        case RUG: case SPICES: case CHAIN:
            return true;
        default:
            return false;
    }
}

ObjectWord bottle_contents(void)
{
    switch (objs(BOTTLE).prop) {
        case 0: return WATER;
        case 2: return OIL;
        /* other valid values: 1, -2 (post-closing) */
    }
    return NOTHING;
}

/* Return true if t is at loc, or being carried. */
bool here(ObjectWord t, Location loc)
{
    return toting(t) || there(t, loc);
}

void drop(ObjectWord t, Location l)
{
    assert(objs(t).place == R_INHAND || objs(t).place == R_LIMBO);
    assert(objs(t).link == NULL);
    if (toting(t)) --holding_count;
    objs(t).place = l;
    if (l == R_INHAND) {
        ++holding_count;
    } else if (l != R_LIMBO) {
        objs(t).link = places[l].objects;
        places[l].objects = &objs(t);
    }
}

#define move(t,l) do { carry(t); drop((t),l); } while (0)
#define juggle(t) do { Location l = objs(t).place; move(t, l); } while (0)
#define destroy(t) move((t), R_LIMBO)

void carry(ObjectWord t)
{
    Location l = objs(t).place;
    if (l != R_INHAND) {
        if (l > R_LIMBO) {
            /* Remove t from l's object-list */
            struct ObjectData **p = &places[l].objects;
            while (*p != &objs(t)) p = &(*p)->link;
            *p = (*p)->link;
        }
        objs(t).place = R_INHAND;
        objs(t).link = NULL;
        ++holding_count;
    }
}

bool is_at_loc(ObjectWord t, Location loc)
{
    if (objs(t).base == NULL)
        return there(t, loc);
    /* Check the "alternative" objects based on this one. */
    for (ObjectWord tt = t; objs(tt).base == &objs(t); ++tt) {
        if (there(tt, loc))
            return true;
    }
    return false;
}

void mobilize(ObjectWord t) { objs(t).base = NULL; }
void immobilize(ObjectWord t) { objs(t).base = &objs(t); }

void new_obj(ObjectWord t, const char *n, ObjectWord b, Location l)
{
    objs(t).name = n;
    objs(t).prop = (is_treasure(t) ? -1 : 0);
    objs(t).base = (b != 0 ? &objs(b) : NULL);
    objs(t).place = l;
    objs(t).link = NULL;
    if (l > R_LIMBO) {
       /* Drop the object at the *end* of its list. Combined with the
         * ordering of the item numbers, this ensures that the CHASM
         * is described before the TROLL, the DRAGON before the RUG,
         * and so on. */
        struct ObjectData **p = &places[l].objects;
        while (*p != NULL)
            p = &(*p)->link;
        *p = &objs(t);
    }
}

void build_object_table(void)
{
    new_obj(KEYS, "Set of keys", 0, R_HOUSE);
    objs(KEYS).desc[0] = "There are some keys on the ground here.";
    new_obj(LAMP, "Brass lantern", 0, R_HOUSE);
    objs(LAMP).desc[0] = "There is a shiny brass lamp nearby.";
    objs(LAMP).desc[1] = "There is a lamp shining nearby.";
    new_obj(GRATE, 0, GRATE, R_OUTSIDE);
    new_obj(GRATE_, 0, GRATE, R_INSIDE);
    objs(GRATE).desc[0] = "The grate is locked.";
    objs(GRATE).desc[1] = "The grate is open.";
    new_obj(CAGE, "Wicker cage", 0, R_COBBLES);
    objs(CAGE).desc[0] = "There is a small wicker cage discarded nearby.";
    new_obj(ROD, "Black rod", 0, R_DEBRIS);
    objs(ROD).desc[0] = "A three-foot black rod with a rusty star on an end lies nearby.";
    new_obj(ROD2, "Black rod", 0, R_LIMBO);
    objs(ROD2).desc[0] = "A three-foot black rod with a rusty mark on an end lies nearby.";
    new_obj(TREADS, 0, TREADS, R_SPIT);
    new_obj(TREADS_, 0, TREADS, R_EMIST);
    objs(TREADS).desc[0] = "Rough stone steps lead down the pit.";
    objs(TREADS).desc[1] = "Rough stone steps lead up the dome.";
    new_obj(BIRD, "Little bird in cage", 0, R_BIRD);
    objs(BIRD).desc[0] = "A cheerful little bird is sitting here singing.";
    objs(BIRD).desc[1] = "There is a little bird in the cage.";
    new_obj(RUSTY_DOOR, 0, RUSTY_DOOR, R_IMMENSE);
    objs(RUSTY_DOOR).desc[0] = "The way north is barred by a massive, rusty, iron door.";
    objs(RUSTY_DOOR).desc[1] = "The way north leads through a massive, rusty, iron door.";
    new_obj(PILLOW, "Velvet pillow", 0, R_SOFT);
    objs(PILLOW).desc[0] = "A small velvet pillow lies on the floor.";
    new_obj(SNAKE, 0, SNAKE, R_HMK);
    objs(SNAKE).desc[0] = "A huge green fierce snake bars the way!";
    objs(SNAKE).desc[1] = NULL;  /* chased away */
    new_obj(FISSURE, 0, FISSURE, R_EFISS);
    new_obj(FISSURE_, 0, FISSURE, R_WFISS);
    objs(FISSURE).desc[0] = NULL;
    objs(FISSURE).desc[1] ="A crystal bridge now spans the fissure.";
    new_obj(TABLET, 0, TABLET, R_DARK);
    objs(TABLET).desc[0] =  /* Woods has "imbedded". */
        "A massive stone tablet embedded in the wall reads:" SOFT_NL
        "\"CONGRATULATIONS ON BRINGING LIGHT INTO THE DARK-ROOM!\"";
    new_obj(CLAM, "Giant clam >GRUNT!<", 0, R_SHELL);
    objs(CLAM).desc[0] = "There is an enormous clam here with its shell tightly closed.";
    new_obj(OYSTER, "Giant oyster >GROAN!<", 0, R_LIMBO);
    objs(OYSTER).desc[0] = "There is an enormous oyster here with its shell tightly closed.";
    new_obj(MAG, "\"Spelunker Today\"", 0, R_ANTE);
    objs(MAG).desc[0] = "There are a few recent issues of \"Spelunker Today\" magazine here.";
    new_obj(DWARF, 0, DWARF, R_LIMBO);
    new_obj(KNIFE, 0, 0, R_LIMBO);
    new_obj(FOOD, "Tasty food", 0, R_HOUSE);
    objs(FOOD).desc[0] = "There is food here.";
    new_obj(BOTTLE, "Small bottle", 0, R_HOUSE);
    objs(BOTTLE).desc[0] = "There is a bottle of water here.";
    objs(BOTTLE).desc[1] = "There is an empty bottle here.";
    objs(BOTTLE).desc[2] = "There is a bottle of oil here.";
    /* WATER and OIL never appear on the ground; they are invariably
     * either in R_LIMBO or R_INHAND. */
    new_obj(WATER, "Water in the bottle", 0, R_LIMBO);
    new_obj(OIL, "Oil in the bottle", 0, R_LIMBO);
    new_obj(MIRROR, 0, MIRROR, R_MIRROR);
    new_obj(MIRROR_, 0, MIRROR, R_LIMBO);  /* joins up with MIRROR later */
    objs(MIRROR).desc[0] = NULL;
    new_obj(PLANT, 0, PLANT, R_WPIT);
    objs(PLANT).desc[0] = "There is a tiny little plant in the pit, murmuring \"Water, water, ...\"";
    objs(PLANT).desc[1] =
        "There is a 12-foot-tall beanstalk stretching up out of the pit," SOFT_NL
        "bellowing \"Water!! Water!!\"";
    objs(PLANT).desc[2] = "There is a gigantic beanstalk stretching all the way up to the hole.";
    new_obj(PLANT2, 0, PLANT2, R_W2PIT);
    new_obj(PLANT2_, 0, PLANT2, R_E2PIT);
    objs(PLANT2).desc[0] = NULL;
    objs(PLANT2).desc[1] = "The top of a 12-foot-tall beanstalk is poking out of the west pit.";
    objs(PLANT2).desc[2] = "There is a huge beanstalk growing out of the west pit up to the hole.";
    new_obj(STALACTITE, 0, STALACTITE, R_TITE);
    objs(STALACTITE).desc[0] = NULL;
    new_obj(SHADOW, 0, SHADOW, R_WINDOE);
    new_obj(SHADOW_, 0, SHADOW, R_WINDOW);
    objs(SHADOW).desc[0] = "The shadowy figure seems to be trying to attract your attention.";
    new_obj(AXE, "Dwarf's axe", 0, R_LIMBO);
    objs(AXE).desc[0] = "There is a little axe here.";
    objs(AXE).desc[1] = "There is a little axe lying beside the bear.";
    new_obj(DRAWINGS, 0, DRAWINGS, R_ORIENTAL);
    objs(DRAWINGS).desc[0] = NULL;
    new_obj(PIRATE, 0, PIRATE, R_LIMBO);
    new_obj(DRAGON, 0, DRAGON, R_SCAN1);
    new_obj(DRAGON_, 0, DRAGON, R_SCAN3);
    objs(DRAGON).desc[0] = "A huge green fierce dragon bars the way!";
    objs(DRAGON).desc[1] = "The body of a huge green dead dragon is lying off to one side.";
    new_obj(CHASM, 0, CHASM, R_SWSIDE);
    new_obj(CHASM_, 0, CHASM, R_NESIDE);
    objs(CHASM).desc[0] =
        "A rickety wooden bridge extends across the chasm, vanishing into the" SOFT_NL
        "mist. A sign posted on the bridge reads, \"STOP! PAY TROLL!\"";
    objs(CHASM).desc[1] =
        "The wreckage of a bridge (and a dead bear) can be seen at the bottom" SOFT_NL
        "of the chasm.";
    new_obj(TROLL, 0, TROLL, R_SWSIDE);
    new_obj(TROLL_, 0, TROLL, R_NESIDE);
    objs(TROLL).desc[0] =
        "A burly troll stands by the bridge and insists you throw him a" SOFT_NL
        "treasure before you may cross.";
    objs(TROLL).desc[1] = NULL;  /* not present, but not paid off either */
    objs(TROLL).desc[2] = NULL;  /* chased away */
    new_obj(NO_TROLL, 0, NO_TROLL, R_LIMBO);
    new_obj(NO_TROLL_, 0, NO_TROLL, R_LIMBO);
    objs(NO_TROLL).desc[0] = "The troll is nowhere to be seen.";
    new_obj(BEAR, 0, BEAR, R_BARR);
    objs(BEAR).desc[0] = "There is a ferocious cave bear eying you from the far end of the room!";
    objs(BEAR).desc[1] = "There is a gentle cave bear sitting placidly in one corner.";
    objs(BEAR).desc[2] = "There is a contented-looking bear wandering about nearby.";
    objs(BEAR).desc[3] = NULL;  /* the dead bear remains as scenery where it fell */
    new_obj(MESSAGE, 0, MESSAGE, R_LIMBO);
    objs(MESSAGE).desc[0] =
        "There is a message scrawled in the dust in a flowery script, reading:" SOFT_NL
        "\"This is not the maze where the pirate hides his treasure chest.\"";
    new_obj(GORGE, 0, GORGE, R_VIEW);
    objs(GORGE).desc[0] = NULL;  /* it's just scenery */
    new_obj(MACHINE, 0, MACHINE, R_PONY);
    objs(MACHINE).desc[0] =
        "There is a massive vending machine here. The instructions on it read:" SOFT_NL
        "\"Drop coins here to receive fresh batteries.\"";
    new_obj(BATTERIES, "Batteries", 0, R_LIMBO);
    objs(BATTERIES).desc[0] = "There are fresh batteries here.";
    objs(BATTERIES).desc[1] = "Some worn-out batteries have been discarded nearby.";
    new_obj(MOSS, 0, MOSS, R_SOFT);
    objs(MOSS).desc[0] = NULL;  /* it's just scenery */
    new_obj(GOLD, "Large gold nugget", 0, R_NUGGET);
    objs(GOLD).desc[0] = "There is a large sparkling nugget of gold here!";
    new_obj(DIAMONDS, "Several diamonds", 0, R_WFISS);
    objs(DIAMONDS).desc[0] = "There are diamonds here!";
    new_obj(SILVER, "Bars of silver", 0, R_NS);
    objs(SILVER).desc[0] = "There are bars of silver here!";
    new_obj(JEWELS, "Precious jewelry", 0, R_SOUTH);
    objs(JEWELS).desc[0] = "There is precious jewelry here!";
    new_obj(COINS, "Rare coins", 0, R_WEST);
    objs(COINS).desc[0] = "There are many coins here!";
    new_obj(CHEST, "Treasure chest", 0, R_LIMBO);
    objs(CHEST).desc[0] = "The pirate's treasure chest is here!";
    new_obj(EGGS, "Golden eggs", 0, R_GIANT);
    objs(EGGS).desc[0] = "There is a large nest here, full of golden eggs!";
    new_obj(TRIDENT, "Jeweled trident", 0, R_FALLS);
    objs(TRIDENT).desc[0] = "There is a jewel-encrusted trident here!";
    new_obj(VASE, "Ming vase", 0, R_ORIENTAL);
    objs(VASE).desc[0] = "There is a delicate, precious, Ming vase here!";
    objs(VASE).desc[1] = "The floor is littered with worthless shards of pottery.";
    new_obj(EMERALD, "Egg-sized emerald", 0, R_PLOVER);
    objs(EMERALD).desc[0] = "There is an emerald here the size of a plover's egg!";
    new_obj(PYRAMID, "Platinum pyramid", 0, R_DARK);
    objs(PYRAMID).desc[0] = "There is a platinum pyramid here, 8 inches on a side!";
    new_obj(PEARL, "Glistening pearl", 0, R_LIMBO);
    objs(PEARL).desc[0] = "Off to one side lies a glistening pearl!";
    new_obj(RUG_, 0, RUG, R_SCAN3);
    new_obj(RUG, "Persian rug", RUG, R_SCAN1);
    objs(RUG).desc[0] = "There is a Persian rug spread out on the floor!";
    objs(RUG).desc[1] = "The dragon is sprawled out on a Persian rug!!";
    new_obj(SPICES, "Rare spices", 0, R_CHAMBER);
    objs(SPICES).desc[0] = "There are rare spices here!";
    new_obj(CHAIN, "Golden chain", CHAIN, R_BARR);
    objs(CHAIN).desc[0] = "There is a golden chain lying in a heap on the floor!";
    objs(CHAIN).desc[1] = "The bear is locked to the wall with a golden chain!";
    objs(CHAIN).desc[2] = "There is a golden chain locked to the wall!";
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

void shift_words(void)
{
    strcpy(word1, word2);
    *word2 = '\0';
}

/*========== Dwarves and pirate. ==========================================
 * This section corresponds to sections 159--175 in Knuth.
 */

int dflag;  /* how angry are the dwarves? */
Location dloc[6] = { R_PIRATES_NEST, R_HMK, R_WFISS, R_Y2, R_LIKE3, R_COMPLEX };
Location odloc[6];
bool dseen[6];

bool dwarf_in(int loc)  /* is a dwarf present? Section 160 in Knuth. */
{
    if (dflag < 2) return false;
    for (int j=1; j <= 5; ++j) {
        if (dloc[j] == loc) return true;
    }
    return false;
}

void return_pirate_to_lair(bool with_chest)
{
    if (with_chest) {
        drop(CHEST, R_PIRATES_NEST);
        drop(MESSAGE, R_PONY);
    }
    dloc[0] = odloc[0] = R_PIRATES_NEST;
    dseen[0] = false;
}

bool too_easy_to_steal(ObjectWord t, Location loc)
{
    return (t == PYRAMID && (loc == R_PLOVER || loc == R_DARK));
}

void steal_all_your_treasure(Location loc)  /* sections 173--174 in Knuth */
{
    puts("Out from the shadows behind you pounces a bearded pirate!  \"Har, har,\"" SOFT_NL
         "he chortles. \"I'll just take all this booty and hide it away with me" SOFT_NL
         "chest deep in the maze!\"  He snatches your treasure and vanishes into" SOFT_NL
         "the gloom.");
    for (int t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (!is_treasure(t)) continue;
        if (too_easy_to_steal(t, loc)) continue;
        if (here(t, loc) && objs(t).base == NULL) {
            /* The vase, rug, and chain can all be immobile at times. */
            move(t, R_PIRATES_NEST);
        }
    }
}

void pirate_tracks_you(Location loc)
{
    bool chest_needs_placing = there(MESSAGE, R_LIMBO);
    bool stalking = false;
    /* The pirate leaves you alone once you've found the chest. */
    if (loc == R_PIRATES_NEST || objs(CHEST).prop >= 0) return;
    for (int i = MIN_OBJ; i <= MAX_OBJ; ++i) {
        if (!is_treasure(i)) continue;
        if (too_easy_to_steal(i, loc)) continue;
        if (toting(i)) {
            steal_all_your_treasure(loc);
            return_pirate_to_lair(chest_needs_placing);
            return;
        }
        if (there(i, loc)) {
            /* There is a treasure in this room, but we're not carrying
             * it. The pirate won't pounce unless we're carrying the
             * treasure; so he'll try to remain quiet. */
            stalking = true;
        }
    }
    /* tally is the number of treasures we haven't seen; lost_treasures is
     * the number we never will see (due to killing the bird or destroying
     * the troll bridge). */
    if (tally == lost_treasures+1 && !stalking && chest_needs_placing &&
        objs(LAMP).prop && here(LAMP, loc)) {
        /* As soon as we've seen all the treasures (except the ones that are
         * lost forever), we "cheat" and let the pirate be spotted. Of course
         * there have to be shadows to hide in, so check the lamp. */
        puts("There are faint rustling noises from the darkness behind you. As you" SOFT_NL
             "turn toward them, the beam of your lamp falls across a bearded pirate." SOFT_NL
             "He is carrying a large chest. \"Shiver me timbers!\" he cries, \"I've" SOFT_NL
             "been spotted! I'd best hie meself off to the maze to hide me chest!\"" SOFT_NL
             "With that, he vanishes into the gloom.");
        return_pirate_to_lair(true);
        return;
    }
    if (odloc[0] != dloc[0] && pct(20)) {
        puts("There are faint rustling noises from the darkness behind you.");
    }
}

bool forbidden_to_pirate(Location loc)
{
    return (loc > R_PIRATES_NEST);
}

/* Return true if the player got killed by a dwarf this turn.
 * This function represents sections 161--168, 170--175 in Knuth. */
bool move_dwarves_and_pirate(Location loc)
{
    assert(R_LIMBO <= loc && loc <= MAX_LOC);
    if (forbidden_to_pirate(loc) || loc == R_LIMBO) {
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
            dflag = 2;
            if (pct(50)) dloc[1+ran(5)] = R_LIMBO;
            if (pct(50)) dloc[1+ran(5)] = R_LIMBO;
            for (int j=1; j <= 5; ++j) {
                if (dloc[j] == loc) dloc[j] = R_NUGGET;
                odloc[j] = dloc[j];
            }
            /* Knuth quietly fixes the garden-path grammar here:
             *   A little dwarf just walked around a corner, saw you, threw a
             *   little axe at you, cursed, and ran away. (The axe missed.)
             * But Woods' version matches Crowther's original source code,
             * and I don't think the deviation is justified. */
            puts("A little dwarf just walked around a corner, saw you, threw a little" SOFT_NL
                 "axe at you which missed, cursed, and ran away.");
            drop(AXE, loc);
        }
    } else {
        /* Move dwarves and the pirate. */
        int dtotal = 0;  /* this many dwarves are in the room with you */
        int attack = 0;  /* this many have had time to draw their knives */
        int stick = 0;  /* this many have hurled their knives accurately */
        for (int j=0; j <= 5; ++j) {
            if (dloc[j] != R_LIMBO) {
                Location ploc[19];  /* potential locations for the next random step */
                int i = 0;
                /* Make a table of all potential exits.
                 * Dwarves think R_SCAN1, R_SCAN2, R_SCAN3 are three different locations,
                 * although you will never have that perception. */
                for (Instruction *q = start[dloc[j]]; q < start[dloc[j]+1]; ++q) {
                    Location newloc = q->dest;
                    if (i != 0 && newloc == ploc[i-1]) continue;
                    if (newloc < MIN_LOWER_LOC) continue;  /* don't follow above level 2 */
                    if (newloc == odloc[j] || newloc == dloc[j]) continue;  /* don't double back */
                    if (q->cond == 100) continue;
                    if (j == 0 && forbidden_to_pirate(newloc)) continue;
                    if (is_forced(newloc) || newloc > MAX_LOC) continue;
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
                            last_knife_loc = loc;
                            if (ran(1000) < 95*(dflag-2)) ++stick;
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
                    puts("One sharp nasty knife is thrown at you!");
                    if (stick == 0) puts("It misses!");
                    else puts("It gets you!");
                } else {
                    printf("%d of them throw knives at you!\n", attack);
                    if (stick == 0) puts("None of them hit you!");
                    else if (stick == 1) puts("One of them gets you!");
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
bool closed;  /* set only when you're in the repository */
int bonus;  /* extra points awarded for exceptional adventuring skills */

bool cave_is_closing(void)
{
    return (clock1 < 0);
}

void close_the_cave(void)
{
    /* Woods has "entones", but that's not a word, so I'm changing it.
     * Knuth writes "Then your eyes refocus;" in place of "As your eyes
     * refocus," but I don't see any reason to deviate from Woods'
     * wording there. Maybe Knuth was working from a slightly earlier
     * or later version than the one I'm looking at. */
    puts("The sepulchral voice intones, \"The cave is now closed.\"  As the echoes" SOFT_NL
         "fade, there is a blinding flash of light (and a small puff of orange" SOFT_NL
         "smoke). . . .    As your eyes refocus, you look around and find...");
    move(BOTTLE, R_NEEND); objs(BOTTLE).prop = -2;  /* empty */
    move(PLANT, R_NEEND); objs(PLANT).prop = -1;
    move(OYSTER, R_NEEND); objs(OYSTER).prop = -1;
    move(LAMP, R_NEEND); objs(LAMP).prop = -1;
    move(ROD, R_NEEND); objs(ROD).prop = -1;
    move(DWARF, R_NEEND); objs(DWARF).prop = -1;
    move(MIRROR, R_NEEND); objs(MIRROR).prop = -1;
    move(GRATE, R_SWEND); objs(GRATE).prop = 0;
    move(SNAKE, R_SWEND); objs(SNAKE).prop = -2;  /* not blocking the way */
    move(BIRD, R_SWEND); objs(BIRD).prop = -2;  /* caged */
    move(CAGE, R_SWEND); objs(CAGE).prop = -1;
    move(ROD2, R_SWEND); objs(ROD2).prop = -1;
    move(PILLOW, R_SWEND); objs(PILLOW).prop = -1;
    move(MIRROR_, R_SWEND);
    for (int j = MIN_OBJ; j <= MAX_OBJ; ++j) {
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
        puts("A sepulchral voice, reverberating through the cave, says \"Cave" SOFT_NL
             "closing soon.  All adventurers exit immediately through main office.\"");
        clock1 = -1;
        objs(GRATE).prop = 0;
        objs(FISSURE).prop = 0;
        for (int j=0; j <= 5; ++j) {
            dseen[j] = false;
            dloc[j] = R_LIMBO;
        }
        destroy(TROLL); destroy(TROLL_);
        move(NO_TROLL, R_SWSIDE); move(NO_TROLL_, R_NESIDE);
        juggle(CHASM); juggle(CHASM_);
        if (objs(BEAR).prop != 3) destroy(BEAR);
        objs(CHAIN).prop = 0; mobilize(CHAIN);
        objs(AXE).prop = 0; mobilize(AXE);
    } else {
        if (cave_is_closing()) --clock2;
        if (clock2 == 0) {
            close_the_cave();
            return true;
        } else {
            static bool warned = false;
            /* On every turn (if the cave is not closed), we check to see
             * if you are in trouble lampwise. */
            if (objs(LAMP).prop == 1) --lamp_limit;
            if (lamp_limit <= 30 && here(LAMP, loc) && here(BATTERIES, loc) && objs(BATTERIES).prop == 0) {
                puts("Your lamp is getting dim.  I'm taking the liberty of replacing" SOFT_NL
                     "the batteries.");
                objs(BATTERIES).prop = 1;
                if (toting(BATTERIES)) drop(BATTERIES, loc);
                lamp_limit = 2500;
            } else if (lamp_limit == 0) {
                if (here(LAMP, loc)) puts("Your lamp has run out of power.");
                objs(LAMP).prop = 0;
                lamp_limit = -1;
            } else if (lamp_limit < 0 && loc < MIN_IN_CAVE) {
                puts("There's not much point in wandering around out here, and you can't" SOFT_NL
                     "explore the cave without a lamp.  So let's just call it a day.");
                give_up();
            } else if (lamp_limit < 30 && !warned && here(LAMP, loc)) {
                printf("Your lamp is getting dim");
                if (objs(BATTERIES).prop == 1) {
                    puts(", and you're out of spare batteries.  You'd" SOFT_NL
                         "best start wrapping this up.");
                } else if (there(BATTERIES, R_LIMBO)) {
                    puts(".  You'd best start wrapping this up, unless" SOFT_NL
                         "you can find some fresh batteries.  I seem to recall that there's" SOFT_NL
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
    static bool panic = false;
    if (!panic) {
        clock2 = 15;
        panic = true;
    }
    puts("A mysterious recorded voice groans into life and announces:" SOFT_NL
         "   \"This exit is closed.  Please leave via main office.\"");
    /* Woods does NOT set "was_dark = false" at this point.
     * This means that if you've gotten into the habit of turning
     * off your lamp before you use a magic word to teleport out
     * of the cave, we might well add injury to insult by causing
     * you to fall into a pit and die (thus ending the game) right
     * at this point. */
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
    if (dark && !is_forced(loc)) {
        if (was_dark && pct(35)) return 'p';  /* goto pitch_dark; */
        room_description = pitch_dark_msg;
    } else if (places[loc].short_desc == NULL || places[loc].visits % verbose_interval == 0) {
        room_description = places[loc].long_desc;
    } else {
        room_description = places[loc].short_desc;
    }
    if (toting(BEAR)) {
        puts("You are being followed by a very large, tame bear.");
    }
    if (room_description != NULL) {
        /* R_CHECK's description is NULL. */
        printf("\n%s\n", room_description);
    }
    if (is_forced(loc)) return 't';  /* goto try_move; */
    give_optional_plugh_hint(loc);
    if (!dark) {
        places[loc].visits += 1;
        /* Describe the objects at this location. */
        for (struct ObjectData *t = places[loc].objects; t != NULL; t = t->link) {
            struct ObjectData *tt = t->base ? t->base : t;
            if (tt->prop < 0) {  /* you've spotted a treasure */
                if (closed) continue;  /* no automatic prop change after hours */
                tt->prop = (tt == &objs(RUG) || tt == &objs(CHAIN));
                tally--;
                if (tally == lost_treasures && tally > 0 && lamp_limit > 35) {
                    /* Zap the lamp if the remaining treasures are too elusive */
                    lamp_limit = 35;
                }
            }
            if (tt == &objs(TREADS) && toting(GOLD)) {
                /* The rough stone steps disappear if we are carrying the nugget. */
            } else {
                int going_up = (tt == &objs(TREADS) && loc == R_EMIST);
                const char *obj_description = tt->desc[tt->prop + going_up];
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
    "Somewhere nearby is Colossal Cave, where others have found fortunes in" SOFT_NL
    "treasure and gold, though it is rumored that some who enter are never" SOFT_NL
    "seen again.  Magic is said to work in the cave.  I will be your eyes" SOFT_NL
    "and hands.  Direct me with commands of 1 or 2 words.  I should warn" SOFT_NL
    "you that I look at only the first five letters of each word, so you'll" SOFT_NL
    "have to enter \"NORTHEAST\" as \"NE\" to distinguish it from \"NORTH\"." SOFT_NL
    "(Should you get stuck, type \"HELP\" for some general hints.  For infor" SOFT_HYPHEN
    "mation on how to end your adventure, etc., type \"INFO\".)\n"
    "                             -  -  -\n"
    "The first Adventure program was developed by Willie Crowther.\n"
    "Most of the features of the current program were added by Don Woods.\n"
    "This particular program was translated from Fortran to CWEB by Don Knuth," SOFT_NL
    "and then from CWEB to ANSI C by Arthur O'Dwyer." },
    { 0, false, 0, 10,
    "Hmmm, this looks like a clue, which means it'll cost you 10 points to" SOFT_NL
    "read it.  Should I go ahead and read it anyway?",
    "It says, \"There is something strange about this place, such that one" SOFT_NL
    "of the words I've always known now has a new effect.\"" },
    { 0, false, 4, 2, "Are you trying to get into the cave?",
    "The grate is very solid and has a hardened steel lock.  You cannot" SOFT_NL
    "enter without a key, and there are no keys in sight.  I would recommend" SOFT_NL
    "looking elsewhere for the keys." },
    { 0, false, 5, 2, "Are you trying to catch the bird?",
    "Something seems to be frightening the bird just now and you cannot" SOFT_NL
    "catch it no matter what you try. Perhaps you might try later." },
    { 0, false, 8, 2, "Are you trying to deal somehow with the snake?",
    "You can't kill the snake, or drive it away, or avoid it, or anything" SOFT_NL
    "like that.  There is a way to get by, but you don't have the necessary" SOFT_NL
    "resources right now." },
    { 0, false, 75, 4, "Do you need help getting out of the maze?",
    "You can make the passages look less alike by dropping things." },
    { 0, false, 25, 5, "Are you trying to explore beyond the Plover Room?",
    "There is a way to explore that region without having to worry about" SOFT_NL
    "falling into a pit.  None of the objects available is immediately" SOFT_NL
    "useful in discovering the secret." },
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
    for (int j = 2; j <= 7; ++j, k <<= 1) {
        if (hints[j].given) continue;
        if ((places[loc].flags & k) == 0) {
            /* We've left the map region associated with this hint. */
            hints[j].count = 0;
            continue;
        }
        /* Count the number of turns spent here. */
        if (++hints[j].count >= hints[j].thresh) {
            switch (j) {
                case 2:  /* How to get into the cave. */
                    if (!objs(GRATE).prop && !here(KEYS, loc)) {
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
                    if (places[loc].objects == NULL &&
                            places[oldloc].objects == NULL &&
                            places[oldoldloc].objects == NULL &&
                            holding_count > 1) {
                        offer(j);
                    }
                    hints[j].count = 0;
                    break;
                case 6:  /* How to explore beyond the Plover Room. */
                    if (objs(EMERALD).prop != -1 && objs(PYRAMID).prop == -1) {
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
    int s = 2;
    if (dflag != 0) s += 25;
    for (int i = MIN_OBJ; i <= MAX_OBJ; ++i) {
        if (!is_treasure(i)) continue;
        if (objs(i).prop >= 0) {
            s += 2;  /* two points just for seeing a treasure */
            if (there(i, R_HOUSE) && objs(i).prop == 0) {
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
    if (there(MAG, R_WITT)) s += 1;  /* last lousy point */
    if (cave_is_closing()) s += 25;  /* bonus for making it this far */
    s += bonus;
    for (int i = 0; i < 8; ++i) {
        if (hints[i].given) s -= hints[i].cost;
    }
    return s;
}

void say_score(void)
{
    printf("If you were to quit now, you would score %d" SOFT_NL
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
    "All of Adventuredom gives tribute to you, Adventurer Grandmaster!"
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
#ifdef Z_MACHINE
    puts("\n");
#endif /* Z_MACHINE */
    exit(0);
}

const char *death_wishes[2*MAX_DEATHS] = {
    "Oh dear, you seem to have gotten yourself killed.  I might be able to" SOFT_NL
      "help you out, but I've never really done this before.  Do you want me" SOFT_NL
      "to try to reincarnate you?",
    "All right. But don't blame me if something goes wr......\n"
      "                   --- POOF!! ---\n"
      "You are engulfed in a cloud of orange smoke.  Coughing and gasping," SOFT_NL
      "you emerge from the smoke and find....",
    "You clumsy oaf, you've done it again!  I don't know how long I can" SOFT_NL
      "keep this up.  Do you want me to try reincarnating you again?",
    "Okay, now where did I put my orange smoke?....  >POOF!<\n"
      "Everything disappears in a dense cloud of orange smoke.",
    "Now you've really done it!  I'm out of orange smoke!  You don't expect" SOFT_NL
      "me to do a decent reincarnation without any orange smoke, do you?",
    "Okay, if you're so smart, do it yourself!  I'm leaving!"
};

void kill_the_player(Location last_safe_place)
{
    death_count++;
    if (cave_is_closing()) {
        puts("It looks as though you're dead.  Well, seeing as how it's"
             " so close to closing time anyway, let's just call it a day.");
        quit();
    }
    if (!yes(death_wishes[2*death_count-2], death_wishes[2*death_count-1], ok) || death_count == MAX_DEATHS)
        quit();
    /* At this point you are reborn. */
    if (toting(LAMP)) objs(LAMP).prop = 0;
    objs(WATER).place = R_LIMBO;
    objs(OIL).place = R_LIMBO;
    for (int j = MAX_OBJ; j >= MIN_OBJ; --j) {
        if (toting(j)) drop(j, (j == LAMP) ? R_ROAD : last_safe_place);
    }
}

/*========== Main loop. ===================================================
 * This section corresponds to sections 74--158 in Knuth.
 */

bool now_in_darkness(Location loc)
{
    if (places[loc].flags & F_LIGHTED) return false;
    if (here(LAMP, loc) && objs(LAMP).prop) return false;
    return true;
}

/* Sections 158, 169, 182 in Knuth */
void adjustments_before_listening(Location loc)
{
    if (last_knife_loc != loc) {
        /* When we move, the dwarf's vanishing knife goes out of scope. */
        last_knife_loc = R_LIMBO;
    }
    if (closed) {
        /* After the cave is closed, we look for objects being toted with
         * prop < 0; their prop value is changed to -1 - prop. This means
         * they won't be described until they've been picked up and put
         * down, separate from their respective piles. Section 182 in Knuth. */
        if (objs(OYSTER).prop < 0 && toting(OYSTER)) {
            puts("Interesting. There seems to be something written on the underside of" SOFT_NL
                 "the oyster.");
        }
        for (int j = MIN_OBJ; j <= MAX_OBJ; ++j) {
            if (toting(j) && objs(j).prop < 0)
                objs(j).prop = -1 - objs(j).prop;
        }
    }
}

Location attempt_plover_passage(Location from)  /* section 149 in Knuth */
{
    if (holding_count == !!toting(EMERALD))
        return R_ALCOVE + R_PLOVER - from;
    puts("Something you're carrying won't fit through the tunnel with you." SOFT_NL
         "You'd best take inventory and drop something.");
    return from;
}

void attempt_inventory(void)  /* section 94 in Knuth */
{
    bool holding_anything = false;
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (toting(t) && t != BEAR) {
            if (!holding_anything) {
                holding_anything = true;
                puts("You are currently holding the following:");
            }
            printf(" %s\n", objs(t).name);
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
            puts("Don't be ridiculous!");
            break;
    }
}

void take_something_immobile(ObjectWord obj)
{
    if (obj == CHAIN && objs(BEAR).prop != 0) {
        puts("The chain is still locked.");
    } else if (obj == BEAR && objs(BEAR).prop == 1) {
        puts("The bear is still chained to the wall.");
    } else if (obj == PLANT && objs(PLANT).prop <= 0) {
        puts("The plant has exceptionally deep roots and cannot be pulled free.");
    } else {
        puts("You can't be serious!");
    }
}

/* The verb is TAKE. Returns true if the action is finished. */
bool take_bird_or_cage(ObjectWord obj)
{
    if (obj == BIRD && !objs(BIRD).prop) {
        if (toting(ROD)) {
            puts("The bird was unafraid when you entered, but as you approach it becomes" SOFT_NL
                 "disturbed and you cannot catch it.");
            return true;
        } else if (!toting(CAGE)) {
            puts("You can catch the bird, but you cannot carry it.");
            return true;
        } else {
            objs(BIRD).prop = 1;
        }
    }
    /* At this point the TAKE action is guaranteed to succeed, so
     * let's also deal with the cage. Taking a caged bird means also
     * taking the cage; taking a cage with a bird in it means also
     * taking the bird. */
    if (obj == BIRD) carry(CAGE);
    if (obj == CAGE && objs(BIRD).prop) carry(BIRD);
    return false;
}

/* Return true if the command was TAKE WATER or TAKE OIL,
 * and there's not a bottle of that substance on the ground.
 * We'll redirect it to FILL BOTTLE and try again. */
bool attempt_take(ObjectWord obj, Location loc)
{
    if (toting(obj)) {
        puts("You are already carrying it!");
        return false;
    } else if (objs(obj).base != NULL) {
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
            objs(bottle_contents()).place = R_INHAND;
        puts(ok);
    }
    return false;
}

void attempt_drop(ObjectWord obj, Location loc)
{
    if (obj == ROD && toting(ROD2) && !toting(ROD)) {
        obj = ROD2;
    }

    if (!toting(obj)) {
        puts("You aren't carrying it!");
    } else if (obj == COINS && here(MACHINE, loc)) {
        /* Put coins in the vending machine. */
        destroy(COINS);
        drop(BATTERIES, loc);
        objs(BATTERIES).prop = 0;
        puts("There are fresh batteries here.");
    } else if (obj == VASE && loc != R_SOFT) {
        drop(VASE, loc);
        if (there(PILLOW, loc)) {
            /* In Long's "Adventure 6", the response is this message
             * plus the default "Dropped." */
            puts("The vase is now resting, delicately, on a velvet pillow.");
        } else {
            puts("The Ming vase drops with a delicate crash.");
            objs(VASE).prop = 1;  /* the vase is now broken */
            immobilize(VASE);
        }
    } else if (obj == BEAR && is_at_loc(TROLL, loc)) {
        /* Chase the troll away. */
        puts("The bear lumbers toward the troll, who lets out a startled shriek and" SOFT_NL
             "scurries away.  The bear soon gives up the pursuit and wanders back.");
        destroy(TROLL); destroy(TROLL_);
        drop(NO_TROLL, R_SWSIDE); drop(NO_TROLL_, R_NESIDE);
        objs(TROLL).prop = 2;
        juggle(CHASM); juggle(CHASM_);  /* put first in their lists */
        drop(BEAR, loc);
    } else if (obj == BIRD && here(SNAKE, loc)) {
        puts("The little bird attacks the green snake, and in an astounding flurry" SOFT_NL
             "drives the snake away.");
        if (closed) dwarves_upset();
        drop(BIRD, loc);
        objs(BIRD).prop = 0;  /* no longer in the cage */
        destroy(SNAKE);
        objs(SNAKE).prop = 1;  /* used in conditional Instructions */
    } else if (obj == BIRD && is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) {
        puts("The little bird attacks the green dragon, and in an astounding flurry" SOFT_NL
             "gets burnt to a cinder.  The ashes blow away.");
        destroy(BIRD);
        objs(BIRD).prop = 0;  /* no longer in the cage */
        /* Now that the bird is dead, you can never get past the snake
         * into the south side chamber, so the precious jewelry is lost. */
        if (there(SNAKE, R_HMK)) ++lost_treasures;
    } else {
        /* The usual case for dropping any old object. */

        if (obj == BIRD) objs(BIRD).prop = 0;  /* no longer caged */
        if (obj == CAGE && objs(BIRD).prop) drop(BIRD, loc);

        /* Special cases for dropping a liquid. */
        if (obj == WATER && objs(BOTTLE).prop == 0) obj = BOTTLE;
        if (obj == OIL && objs(BOTTLE).prop == 2) obj = BOTTLE;
        if (obj == BOTTLE && bottle_contents() != NOTHING)
            move(bottle_contents(), R_LIMBO);

        drop(obj, loc);
        puts(ok);
    }
}

void attempt_wave(ObjectWord obj, Location loc)  /* section 99 in Knuth */
{
    if (obj == ROD && (loc == R_EFISS || loc == R_WFISS) &&
            toting(ROD) && !cave_is_closing()) {
        if (objs(FISSURE).prop) {
            puts("The crystal bridge has vanished!");
            objs(FISSURE).prop = 0;
        } else {
            puts("A crystal bridge now spans the fissure.");
            objs(FISSURE).prop = 1;
        }
    } else if (toting(obj) || (obj == ROD && toting(ROD2))) {
        puts("Nothing happens.");
    } else {
        puts("You aren't carrying it!");
    }
}

void attempt_blast(Location loc)  /* section 99 in Knuth */
{
    if (closed && objs(ROD2).prop >= 0) {
        if (here(ROD2, loc)) {
            bonus = 25;
            puts("There is a loud explosion and you are suddenly splashed across the" SOFT_NL
                 "walls of the room.");
        } else if (loc == R_NEEND) {
            bonus = 30;
            puts("There is a loud explosion and a twenty-foot hole appears in the far" SOFT_NL
                 "wall, burying the snakes in the rubble. A river of molten lava pours" SOFT_NL
                 "in through the hole, destroying everything in its path, including you!");
        } else {
            bonus = 45;
            puts("There is a loud explosion and a twenty-foot hole appears in the far" SOFT_NL
                 "wall, burying the dwarves in the rubble.  You march through the hole" SOFT_NL
                 "and find yourself in the main office, where a cheering band of" SOFT_NL
                 "friendly elves carry the conquering adventurer off into the sunset.");
        }
        quit();
    } else {
        puts("Blasting requires dynamite.");
    }
}

void attempt_rub(ObjectWord obj)  /* section 99 in Knuth */
{
    if (obj == LAMP) {
        puts("Rubbing the electric lamp is not particularly rewarding. Anyway," SOFT_NL
             "nothing exciting happens.");
    } else {
        puts("Peculiar.  Nothing unexpected happens.");
    }
}

void attempt_find(ObjectWord obj, Location loc)  /* section 100 in Knuth */
{
    if (toting(obj)) {
        puts("You are already carrying it!");
    } else if (closed) {
        puts("I daresay whatever you want is around here somewhere.");
    } else {
        bool its_right_here = false;
        if (is_at_loc(obj, loc)) {
            its_right_here = true;
        } else if (obj != NOTHING && obj == bottle_contents() && there(BOTTLE, loc)) {
            its_right_here = true;
        } else if (obj == WATER && (places[loc].flags & F_WATER)) {
            its_right_here = true;
        } else if (obj == OIL && loc == R_EPIT) {
            its_right_here = true;
        } else if (obj == DWARF && dwarf_in(loc)) {
            its_right_here = true;
        }
        if (its_right_here) {
            puts("I believe what you want is right here with you.");
        } else {
            puts("I can only tell you what you see as you move about and manipulate" SOFT_NL
                 "things.  I cannot tell you where remote things are.");
        }
    }
}

void attempt_break(ObjectWord obj, Location loc)  /* section 101 in Knuth */
{
    if (obj == VASE && objs(VASE).prop == 0) {
        if (toting(VASE))
            drop(VASE, loc);
        puts("You have taken the vase and hurled it delicately to the ground.");
        objs(VASE).prop = 1;  /* worthless shards */
        immobilize(VASE);
    } else if (obj == MIRROR) {
        if (closed) {
            puts("You strike the mirror a resounding blow, whereupon it shatters into a" SOFT_NL
                 "myriad tiny fragments.");
            dwarves_upset();
        } else {
            puts("It is too far up for you to reach.");
        }
    } else {
        puts("It is beyond your power to do that.");
    }
}

void attempt_wake(ObjectWord obj)  /* section 101 in Knuth */
{
    if (closed && obj == DWARF) {
        puts("You prod the nearest dwarf, who wakes up grumpily, takes one look at" SOFT_NL
             "you, curses, and grabs for his axe.");
        dwarves_upset();
    }
    puts("Don't be ridiculous!");
}

void attempt_off(Location loc)  /* section 102 in Knuth */
{
    if (!here(LAMP, loc)) {
        puts("You have no source of light.");
    } else {
        objs(LAMP).prop = 0;
        puts("Your lamp is now off.");
        if (now_in_darkness(loc))
            puts(pitch_dark_msg);
    }
}

void throw_axe_at_dwarf(Location loc)  /* section 163 in Knuth */
{
    int j;
    for (j=1; j <= 5; ++j) {
        if (dloc[j] == loc) break;
    }
    assert(j <= 5);
    if (ran(3) < 2) {
        static bool first_time = true;
        if (first_time) {
            puts("You killed a little dwarf.  The body vanishes in a cloud of greasy" SOFT_NL
                 "black smoke.");
            first_time = false;
        } else {
            puts("You killed a little dwarf.");
        }
        dloc[j] = R_LIMBO;  /* Once killed, a dwarf never comes back. */
        dseen[j] = false;
    } else {
        puts("You attack a little dwarf, but he dodges out of the way.");
    }
}

/* Return true if we don't know what to fill. */
bool attempt_fill(ObjectWord obj, Location loc)  /* sections 110--111 in Knuth */
{
    if (obj == VASE) {
        if (loc != R_EPIT && !(places[loc].flags & F_WATER)) {
            puts("There is nothing here with which to fill the vase.");
        } else if (!toting(VASE)) {
            puts("You aren't carrying it!");
        } else {
            /* In Crowther and Woods' original, after shattering the vase this
             * way, we GOTO the generic "drop" code. This produces a silly
             * combination of messages --- and repairs the vase! --- if the
             * pillow is on the ground next to you as you fill the vase.
             * In Long's "Adventure 6", we skip the pillow-checking code, but
             * still end up in the default handler, which would normally
             * print "Dropped." but in this instance prints "There is nothing
             * here with which to fill the vase." */
            puts("The sudden change in temperature has delicately shattered the vase.");
            objs(VASE).prop = 1;  /* worthless shards */
            drop(VASE, loc);
            immobilize(VASE);
        }
    } else if (!here(BOTTLE, loc)) {
        if (obj == NOTHING)
            return true;
        puts("You can't fill that.");
    } else if (obj != NOTHING && obj != BOTTLE) {
        puts("You can't fill that.");
    } else if (bottle_contents() != NOTHING) {
        puts("Your bottle is already full.");
    } else if (loc == R_EPIT) {
        puts("Your bottle is now full of oil.");
        objs(BOTTLE).prop = 2;
        if (toting(BOTTLE)) objs(OIL).place = R_INHAND;
    } else if (places[loc].flags & F_WATER) {
        puts("Your bottle is now full of water.");
        objs(BOTTLE).prop = 0;
        if (toting(BOTTLE)) objs(WATER).place = R_INHAND;
    } else {
        puts("There is nothing here with which to fill the bottle.");
    }
    return false;  /* just continue normally */
}

void attempt_feed(ObjectWord obj, Location loc)  /* section 129 in Knuth */
{
    switch (obj) {
        case BIRD:
            puts("It's not hungry (it's merely pinin' for the fjords).  Besides, you" SOFT_NL
                 "have no bird seed.");
            break;
        case TROLL:
            puts("Gluttony is not one of the troll's vices.  Avarice, however, is.");
            break;
        case DRAGON:
            if (objs(DRAGON).prop) {
                puts("Don't be ridiculous!");  /* reject feeding the dead dragon */
            } else {
                puts("There's nothing here it wants to eat (except perhaps you).");
            }
            break;
        case SNAKE:
            if (!closed && here(BIRD, loc)) {
                destroy(BIRD);
                objs(BIRD).prop = 0;
                ++lost_treasures;
                puts("The snake has now devoured your bird.");
            } else {
                puts("There's nothing here it wants to eat (except perhaps you).");
            }
            break;
        case BEAR:
            if (here(FOOD, loc)) {
                assert(objs(BEAR).prop == 0);
                destroy(FOOD);
                objs(BEAR).prop = 1;
                objs(AXE).prop = 0;
                mobilize(AXE);  /* if it was immobilized by the bear */
                puts("The bear eagerly wolfs down your food, after which he seems to calm" SOFT_NL
                     "down considerably and even becomes rather friendly.");
            } else if (objs(BEAR).prop == 0) {
                puts("There's nothing here it wants to eat (except perhaps you).");
            } else if (objs(BEAR).prop == 3) {
                puts("Don't be ridiculous!");
            } else {
                /* The bear is tame; therefore the food is gone. */
                puts("There is nothing here to eat.");
            }
            break;
        case DWARF:
            if (here(FOOD, loc)) {
                ++dflag;
                puts("You fool, dwarves eat only coal!  Now you've made him *REALLY* mad!!");
            } else {
                puts("There is nothing here to eat.");
            }
            break;
        default:
            puts("I'm game.  Would you care to explain how?");
            break;
    }
}

void attempt_open_or_close(ActionWord verb, ObjectWord obj, Location loc)  /* sections 130--134 in Knuth */
{
    bool verb_is_open = (verb == OPEN);  /* otherwise it's CLOSE */
    switch (obj) {
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
                puts("A glistening pearl falls out of the clam and rolls away.  Goodness," SOFT_NL
                     "this must really be an oyster.  (I never was very good at identifying" SOFT_NL
                     "bivalves.)  Whatever it is, it has now snapped shut again.");
            } else {
                puts("The oyster creaks open, revealing nothing but oyster inside." SOFT_NL
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
                bool was_open = objs(GRATE).prop;
                objs(GRATE).prop = verb_is_open;
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
                if (objs(CHAIN).prop == 0) {
                    puts("It was already unlocked.");
                } else if (objs(BEAR).prop == 0) {
                    puts("There is no way to get past the bear to unlock the chain, which is" SOFT_NL
                         "probably just as well.");
                } else {
                    objs(CHAIN).prop = 0;
                    mobilize(CHAIN);
                    if (objs(BEAR).prop == 1) {
                        objs(BEAR).prop = 2;
                        mobilize(BEAR);
                    }
                    puts("The chain is now unlocked.");
                }
            } else {
                /* LOCK CHAIN */
                if (loc != R_BARR) {
                    puts("There is nothing here to which the chain can be locked.");
                } else if (objs(CHAIN).prop) {
                    puts("It was already locked.");
                } else {
                    objs(CHAIN).prop = 2;
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
        case RUSTY_DOOR:
            if (objs(RUSTY_DOOR).prop) {
                puts(ok);
            } else {
                /* Notice that CLOSE DOOR also gives this response. */
                puts("The door is extremely rusty and refuses to open.");
            }
            break;
        default:
            puts("I don't know how to lock or unlock such a thing.");
            break;
    }
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
            puts("\"This is not the maze where the pirate leaves his treasure chest.\"");
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
            puts("I'm afraid I don't understand.");
            break;
    }
}

int check_noun_validity(ObjectWord obj, Location loc)  /* sections 90--91 in Knuth */
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
            if (is_at_loc(PLANT2, loc) && objs(PLANT2).prop != 0) {
                return 'p';  /* obj = PLANT2 */
            }
            return 'c';  /* can't see it */
        case KNIFE: {
            /* You're allowed to try to GET KNIFE, but only once. The game
             * informs you that the knife has vanished; and from then on,
             * it never reappears. */
            static bool have_tried_to_get_knife = false;
            if (have_tried_to_get_knife || (loc != last_knife_loc)) {
                return 'c';  /* can't see it */
            }
            puts("The dwarves' knives vanish as they strike the walls of the cave.");
            have_tried_to_get_knife = true;
            return 'f';  /* action is finished */
        }
        case ROD:
            if (!here(ROD2, loc)) return 'c';
            return 'r';  /* obj = ROD2 */
        case WATER:
            if (places[loc].flags & F_WATER) return 0;
            if (here(BOTTLE, loc) && bottle_contents() == WATER) return 0;
            return 'c';
        case OIL:
            if (loc == R_EPIT) return 0;
            if (here(BOTTLE, loc) && bottle_contents() == OIL) return 0;
            return 'c';
    }
    return 'c';
}

Instruction *determine_motion_instruction(Location loc, MotionWord mot)
{
    Instruction *q;
    for (q = start[loc]; q < start[loc+1]; ++q) {
        if (is_forced(loc) || q->mot == mot) break;
    }
    if (q == start[loc+1]) {
        return NULL;  /* newloc = loc at this point */
    }
    while (true) {
        const int cond = q->cond;
        if (cond == 0) {
            break;  /* the usual case */
        } else if (cond <= 100) {
            if (pct(cond)) break;  /* dwarves won't take these routes */
        } else if (cond <= 200) {
            if (toting(MIN_OBJ + cond%100)) break;
        } else if (cond <= 300) {
            if (is_at_loc(MIN_OBJ + cond%100, loc)) break;
        } else {
            if (objs(MIN_OBJ + cond%100).prop != (cond/100)-3) break;
        }
        /* The verb in this instruction matches the "mot" typed by the user,
         * but the condition failed, either randomly (j < 100) or because
         * the prerequisite wasn't satisfied. So instead of taking this
         * instruction, we'll take the next one that is not a "ditto" of
         * this one --- regardless of its "mot" field! */
        const Location dest = q->dest;
        while (q->dest == dest && q->cond == cond)
            ++q;
    }
    return q;
}

void report_inapplicable_motion(MotionWord mot, ActionWord verb)
{
    if (mot == CRAWL) {
        puts("Which way?");
    } else if (mot == XYZZY || mot == PLUGH) {
        puts("Nothing happens.");
    } else if (verb == FIND || verb == INVENTORY) {
        /* This catches inputs such as "FIND BEDQUILT" or "INVENTORY WEST". */
        puts("I can only tell you what you see as you move about and manipulate" SOFT_NL
             "things.  I cannot tell you where remote things are.");
    } else {
        switch (mot) {
            case N: case S: case E: case W: case NE: case SE: case NW: case SW:
            case U: case D:
                puts("There is no way to go in that direction.");
                break;
            case IN: case OUT:
                puts("I don't know in from out here.  Use compass points or name something" SOFT_NL
                     "in the general direction you want to go.");
                break;
            case FORWARD: case LEFT: case RIGHT:
                puts("I am unsure how you are facing.  Use compass points or nearby objects.");
                break;
            default:
                puts("I don't know how to apply that word here.");
                break;
        }
    }
}

void print_remark(int which)
{
    switch (which) {
        case 0:
            puts("You don't fit through a two-inch slit!");
            break;
        case 1:
            puts("You can't go through a locked steel grate!");
            break;
        case 2:
            puts("I respectfully suggest you go across the bridge instead of jumping.");
            break;
        case 3:
            puts("There is no way across the fissure.");
            break;
        case 4:
            puts("You can't fit this five-foot clam through that little passage!");
            break;
        case 5:
            puts("You can't fit this five-foot oyster through that little passage!");
            break;
        case 6:
            puts("You have crawled around in some little holes and wound up back in the" SOFT_NL
                 "main passage.");
            break;
        case 7:
            puts("You have crawled around in some little holes and found your way" SOFT_NL
                 "blocked by a recent cave-in.  You are now back in the main passage.");
            break;
        case 8:
            puts("It is too far up for you to reach.");
            break;
        case 9:
            puts("The door is extremely rusty and refuses to open.");
            break;
        case 10:
            puts("The dragon looks rather nasty.  You'd best not try to get by.");
            break;
        case 11:
            puts("The troll refuses to let you cross.");
            break;
        case 12:
            puts("There is no longer any way across the chasm.");
            break;
        case 13:
            puts("Don't be ridiculous!");
            break;
        case 14:
            puts("The crack is far too small for you to follow.");
            break;
        case 15:
            puts("The dome is unclimbable.");
            break;
        case 16:
            puts("You can't get by the snake.");
            break;
        case 17:
            puts("The stream flows out through a pair of 1-foot-diameter sewer pipes." SOFT_NL
                 "It would be advisable to use the exit.");
            break;
    }
}

MotionWord try_going_back_to(Location l, Location from)
{
    /* Interestingly, the BACK command does not simply move the player
     * back to oldloc. Instead, it attempts to trace a path connecting
     * loc to oldloc; if no such passage exists, we fail to move. If
     * such a passage does exist, then we react as if the player had
     * typed the appropriate motion-word in the first place.
     * Notice that Woods' code makes an effort to deal with FORCED_MOVE
     * locations, but doesn't succeed 100 percent. ("l" is set to oldloc,
     * or to oldoldloc if oldloc is a FORCED_MOVE location.)
     *     R_SWSIDE : CROSS -> R_TROLL -> R_NESIDE : BACK
     * gives "You can't get there from here." as apparently intended,
     * because R_TROLL is a special location.
     *     R_WPIT : CLIMB -> R_CHECK -> R_UPNOUT -> R_WPIT : BACK
     * unintentionally gives "There is nothing here to climb.", because
     * oldoldloc is R_CHECK, and there *is* a passage connecting our
     * current location (R_WPIT) to R_CHECK, so we take it again.
     *     R_WPIT : CLIMB -> R_CHECK -> R_DIDIT -> R_W2PIT : BACK
     * unintentionally gives "You can't get there from here.", because
     * oldoldloc is R_CHECK, and there is no passage connecting our
     * current location (R_W2PIT) to R_CHECK. */

    if (l == from) {
        puts("Sorry, but I no longer seem to remember how you got here.");
        return NOWHERE;
    }
    for (Instruction *q = start[from]; q < start[from+1]; ++q) {
        /* Take the first exit that goes to "l", either directly... */
        Location ll = q->dest;
        if (ll == l)
            return q->mot;
        /* ...or via a single forced move. */
        if (is_forced(ll) && start[ll]->dest == l)
            return q->mot;
    }
    puts("You can't get there from here.");
    return NOWHERE;
}

void collapse_the_troll_bridge(void)
{
    puts("Just as you reach the other side, the bridge buckles beneath the" SOFT_NL
         "weight of the bear, who was still following you around.  You" SOFT_NL
         "scrabble desperately for support, but as the bridge collapses you" SOFT_NL
         "stumble back and fall into the chasm.");
    objs(CHASM).prop = 1;
    objs(TROLL).prop = 2;
    objs(BEAR).prop = 3;  /* the bear is dead */
    drop(BEAR, R_SWSIDE);
    immobilize(BEAR);
    /* If you collapse the troll bridge without seeing the spices, we'll
     * increment lost_treasures so you can still get to the endgame.
     * However, if you threw the trident to the troll before opening the
     * oyster, you're strictly out of luck. And in fact if you pick up
     * a treasure in the dark, you'll never "see" it until you drop it
     * in a lighted area; so there can be arbitrarily many unseen
     * treasures on the far side of the bridge, if the player is doing
     * it deliberately. But we don't care about that case.
     */
    if (objs(SPICES).prop < 0 && objs(SPICES).place >= R_NESIDE)
        ++lost_treasures;
}

/* Modify newloc in place, and return true if the player died crossing the troll bridge. */
bool determine_next_newloc(Location loc, Location *newloc, MotionWord mot, ActionWord verb)
{
    Instruction *q = determine_motion_instruction(loc, mot);
    if (q == NULL) {
        report_inapplicable_motion(mot, verb);
        return false;
    }
    *newloc = q->dest;
    
    if (*newloc >= FIRST_REMARK) {
        print_remark(*newloc - FIRST_REMARK);
        *newloc = loc;
    } else if (*newloc == R_PPASS) {
        *newloc = attempt_plover_passage(loc);
    } else if (*newloc == R_PDROP) {
        drop(EMERALD, loc);
        *newloc = R_Y2 + R_PLOVER - loc;
    } else if (*newloc == R_TROLL) {
        /* Troll bridge crossing is treated as a special motion so
         * that dwarves won't wander across and encounter the bear.
         * You can get here only if TROLL is in limbo but NO_TROLL has
         * taken its place. Moreover, if you're on the southwest side,
         * objs(TROLL).prop will be nonzero. If objs(TROLL).prop is 1,
         * you've crossed since paying, or you've stolen away the payment.
         */
        if (objs(TROLL).prop == 1) {
            /* Block the troll bridge and stay put. */
            objs(TROLL).prop = 0;
            destroy(NO_TROLL); destroy(NO_TROLL_);
            drop(TROLL, R_SWSIDE); drop(TROLL_, R_NESIDE);
            juggle(CHASM); juggle(CHASM_);
            puts("The troll steps out from beneath the bridge and blocks your way.");
            *newloc = loc;  /* stay put */
        } else {
            *newloc = R_NESIDE + R_SWSIDE - loc;  /* cross it */
            if (objs(TROLL).prop == 0)
                objs(TROLL).prop = 1;
            if (toting(BEAR)) {
                assert(*newloc == R_SWSIDE);
                collapse_the_troll_bridge();
                return true;  /* goto death */
            }
        }
    }

    return false;
}

void print_message(MessageWord msg)
{
    switch (msg) {
        case ABRA:
            puts("Good try, but that is an old worn-out magic word.");
            break;
        case HELP:
            puts("I know of places, actions, and things. Most of my vocabulary" SOFT_NL
                 "describes places and is used to move you there. To move, try words" SOFT_NL
                 "like forest, building, downstream, enter, east, west, north, south," SOFT_NL
                 "up, or down. I know about a few special objects, like a black rod" SOFT_NL
                 "hidden in the cave. These objects can be manipulated using some of" SOFT_NL
                 "the action words that I know. Usually you will need to give both the" SOFT_NL
                 "object and action words (in either order), but sometimes I can infer" SOFT_NL
                 "the object from the verb alone. Some objects also imply verbs; in" SOFT_NL
                 "particular, \"inventory\" implies \"take inventory\", which causes me to" SOFT_NL
                 "give you a list of what you're carrying. The objects have side" SOFT_NL
                 "effects; for instance, the rod scares the bird. Usually people having" SOFT_NL
                 "trouble moving just need to try a few more words. Usually people" SOFT_NL
                 "trying unsuccessfully to manipulate an object are attempting something" SOFT_NL
                 "beyond their (or my!) capabilities and should try a completely" SOFT_NL
                 "different tack. To speed the game you can sometimes move long" SOFT_NL
                 "distances with a single word. For example, \"building\" usually gets" SOFT_NL
                 "you to the building from anywhere above ground except when lost in the" SOFT_NL
                 "forest. Also, note that cave passages turn a lot, and that leaving a" SOFT_NL
                 "room to the north does not guarantee entering the next from the south." SOFT_NL
                 "Good luck!");
            break;
        case TREES:
            puts("The trees of the forest are large hardwood oak and maple, with an" SOFT_NL
                 "occasional grove of pine or spruce.  There is quite a bit of under" SOFT_HYPHEN
                 "growth, largely birch and ash saplings plus nondescript bushes of" SOFT_NL
                 "various sorts.  This time of year visibility is quite restricted by" SOFT_NL
                 "all the leaves, but travel is quite easy if you detour around the" SOFT_NL
                 "spruce and berry bushes.");
            break;
        case DIG:
            puts("Digging without a shovel is quite impractical.  Even with a shovel" SOFT_NL
                 "progress is unlikely.");
            break;
        case LOST:
            puts("I'm as confused as you are.");
            break;
        case MIST:
            puts("Mist is a white vapor, usually water, seen from time to time in" SOFT_NL
                 "caverns.  It can be found anywhere but is frequently a sign of a deep" SOFT_NL
                 "pit leading down to water.");
            break;
        case FUCK:
            puts("Watch it!");
            break;
        case STOP:
            puts("I don't know the word \"stop\".  Use \"quit\" if you want to give up.");
            break;
        case INFO:
            puts("If you want to end your adventure early, say \"quit\". To get full" SOFT_NL
                 "credit for a treasure, you must have left it safely in the building," SOFT_NL
                 "though you get partial credit just for locating it. You lose points" SOFT_NL
                 "for getting killed, or for quitting, though the former costs you more." SOFT_NL
                 "There are also points based on how much (if any) of the cave you've" SOFT_NL
                 "managed to explore; in particular, there is a large bonus just for" SOFT_NL
                 "getting in (to distinguish the beginners from the rest of the pack)," SOFT_NL
                 "and there are other ways to determine whether you've been through some" SOFT_NL
                 "of the more harrowing sections. If you think you've found all the" SOFT_NL
                 "treasures, just keep exploring for a while. If nothing interesting" SOFT_NL
                 "happens, you haven't found them all yet. If something interesting" SOFT_NL
                 "DOES happen, it means you're getting a bonus and have an opportunity" SOFT_NL
                 "to garner many more points in the master's section." SOFT_NL
                 "I may occasionally offer hints if you seem to be having trouble." SOFT_NL
                 "If I do, I'll warn you in advance how much it will affect your score" SOFT_NL
                 "to accept the hints. Finally, to save paper, you may specify \"brief\"," SOFT_NL
                 "which tells me never to repeat the full description of a place" SOFT_NL
                 "unless you explicitly ask me to.");
            break;
        case SWIM:
            puts("I don't know how.");
            break;
    }
}

void simulate_an_adventure(void)
{
    Location oldoldloc, oldloc, loc, newloc;
    MotionWord mot = NOWHERE;  /* currently specified motion */
    ActionWord verb = NOTHING;  /* currently specified action */
    ActionWord oldverb;  /* verb before it was changed */
    ObjectWord obj = NOTHING;  /* currently specified object, if any */
    ObjectWord oldobj;  /* former value of obj */
    bool was_dark = false;
    int look_count = 0;

    oldoldloc = oldloc = loc = newloc = R_ROAD;

    while (true) {
        /* Check for interference with the proposed move to newloc. */
        if (cave_is_closing() && newloc < MIN_IN_CAVE && newloc != R_LIMBO) {
            panic_at_closing_time();
            newloc = loc;
        } else if (newloc != loc && !forbidden_to_pirate(loc)) {
            /* Stay in loc if a dwarf is blocking the way to newloc */
            for (int j=1; j <= 5; ++j) {
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
            verb = oldverb = NOTHING;
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
                    verb = NOTHING;
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
                    if (places[loc].flags & F_WATER) {
                        puts("Your feet are now wet.");
                    } else {
                        puts("Where?");
                    }
                    continue;
                } else if (*word2 != '\0') {
                    shift_words();
                    goto parse;
                }
            }
            if (streq(word1, "water") || streq(word1, "oil")) {
                /* Sometimes "water" and "oil" are used as verbs. */
                if (streq(word2, "plant") && there(PLANT, loc))
                    strcpy(word2, "pour");
                if (streq(word2, "door") && there(RUSTY_DOOR, loc))
                    strcpy(word2, "pour");
            }

        parse:
            advise_about_going_west(word1);
            k = lookup(word1);
            switch (word_class(k)) {
                case WordClass_None:
                    printf("Sorry, I don't know the word \"%s\".\n", word1);
                    goto cycle;
                case WordClass_Motion:
                    mot = k;
                    goto try_move;
                case WordClass_Object:
                    obj = k;
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
                        case 'p': obj = PLANT2; break;
                        case 'r': obj = ROD2; break;
                        /* case 0: break; */
                    }
                    if (*word2 != '\0') break;
                    if (verb != NOTHING) goto transitive;
                    printf("What do you want to do with the %s?\n", word1);
                    goto cycle;
                case WordClass_Action:
                    verb = k;
                    if (verb == SAY) {
                        /* "FOO SAY" results in the incorrect message
                         * "Say what?" instead of "Okay, "foo"". */
                        if (*word2 == '\0') goto intransitive;
                        goto transitive;
                    }
                    if (*word2 != '\0') break;
                    if (obj != NOTHING) goto transitive;
                    else goto intransitive;
                case WordClass_Message:
                    print_message(k);
                    continue;
            }

            shift_words();
            goto parse;

        intransitive:
            switch (verb) {
                case GO:
                    puts("Where?");
                    continue;
                case RELAX:
                    puts(ok);
                    continue;
                case ON: case OFF: case POUR: case FILL: case DRINK: case BLAST: case KILL:
                    goto transitive;
                case TAKE: {
                    /* TAKE makes sense by itself if there's only one possible thing to take. */
                    struct ObjectData *object_here = places[loc].objects;
                    if (dwarf_in(loc))
                        goto get_object;
                    if (object_here != NULL && object_here->link == NULL) {
                        obj = MIN_OBJ + (object_here - &objs(MIN_OBJ));
                        goto transitive;
                    }
                    goto get_object;
                }
                case EAT:
                    if (!here(FOOD, loc))
                        goto get_object;
                    obj = FOOD;
                    goto transitive;
                case OPEN: case CLOSE:
                    if (is_at_loc(GRATE, loc)) {
                        obj = GRATE;
                    } else if (there(RUSTY_DOOR, loc)) {
                        obj = RUSTY_DOOR;
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
                    puts("Okay, from now on I'll only describe a place in full the first time" SOFT_NL
                         "you come to it.  To get the full description, say \"LOOK\".");
                    continue;
                case SCORE:
                    say_score();
                    continue;
                case QUIT:
                    if (yes("Do you really want to quit now?", ok, ok)) give_up();
                    continue;
#ifdef SAVE_AND_RESTORE
                case SAVE:
                    switch (attempt_save()) {
                        case 0: puts("Save failed!"); break;
                        case 1: puts("Saved."); break;
                        case 2: puts("Restored."); break;
                    }
                    continue;
                case RESTORE:
                    /* On the fizmo interpreter, @restore yields 2
                     * when the save file doesn't exist, or when it
                     * has the wrong serial number for this game.
                     * I don't know what return value 0 would mean. */
                    attempt_restore();
                    puts("Restore failed!");
                    continue;
#endif /* SAVE_AND_RESTORE */
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
                        if (there(EGGS, R_GIANT) || (toting(EGGS) && loc == R_GIANT)) {
                            puts("Nothing happens.");
                            continue;
                        }
                        if (there(EGGS, R_LIMBO) && there(TROLL, R_LIMBO) && objs(TROLL).prop == 0)
                            objs(TROLL).prop = 1;  /* the troll returns */
                        if (loc == R_GIANT) {
                            puts("There is a large nest here, full of golden eggs!");
                        } else if (here(EGGS, loc)) {
                            puts("The nest of golden eggs has vanished!");
                        } else {
                            puts("Done!");
                        }
                        move(EGGS, R_GIANT);
                        continue;
                    } else if (foobar == 0) {
                        puts("Nothing happens.");
                    } else {
                        puts("What's the matter, can't you read?  Now you'd best start over.");
                    }
                    continue;
                }
                default:
                    goto get_object;
            }
        transitive:
            switch (verb) {
                case SAY: {
                    if (*word2 != '\0') strcpy(word1, word2);
                    int k = lookup(word1);
                    switch (k) {
                        case XYZZY: case PLUGH: case PLOVER:
                            mot = k;
                            goto try_move;
                        case FEEFIE:
                            verb = k;
                            goto intransitive;
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
                        puts("You have no source of light.");
                        continue;
                    }
                    if (lamp_limit < 0) {
                        puts("Your lamp has run out of power.");
                        continue;
                    }
                    objs(LAMP).prop = 1;
                    puts("Your lamp is now on.");
                    if (was_dark) goto commence;
                    continue;
                case OFF:
                    attempt_off(loc);
                    continue;
                case DRINK: {
                    bool stream_here = (places[loc].flags & F_WATER);
                    bool evian_here = here(BOTTLE, loc) && (bottle_contents() == WATER);
                    if (obj == NOTHING) {
                        if (!stream_here && !evian_here)
                            goto get_object;
                    } else if (obj != WATER) {
                        puts("Don't be ridiculous!");
                        continue;
                    }
                    /* Drink from the bottle if we can; otherwise from the stream. */
                    if (evian_here) {
                        objs(BOTTLE).prop = 1;  /* empty */
                        objs(WATER).place = R_LIMBO;
                        puts("The bottle of water is now empty.");
                    } else {
                        puts("You have taken a drink from the stream.  The water tastes strongly of" SOFT_NL
                             "minerals, but is not unpleasant.  It is extremely cold.");
                    }
                    continue;
                }
                case POUR:
                    if (obj == NOTHING || obj == BOTTLE) {
                        obj = bottle_contents();
                        if (obj == NOTHING) goto get_object;
                        /* Notice that POUR BOTTLE when the bottle is empty
                         * will actually result in the message "Bottle what?". */
                    }
                    if (toting(obj)) {
                        if (obj != WATER && obj != OIL) {
                            puts("You can't pour that.");
                            continue;
                        }
                        objs(BOTTLE).prop = 1;  /* empty */
                        objs(obj).place = R_LIMBO;
                        if (there(PLANT, loc)) {
                            /* Try to water the plant. */
                            if (obj != WATER) {
                                puts("The plant indignantly shakes the oil off its leaves and asks, \"Water?\"");
                                continue;
                            } else {
                                if (objs(PLANT).prop == 0) {
                                    puts("The plant spurts into furious growth for a few seconds.");
                                    objs(PLANT).prop = 1;
                                } else if (objs(PLANT).prop == 1) {
                                    puts("The plant grows explosively, almost filling the bottom of the pit.");
                                    objs(PLANT).prop = 2;
                                } else if (objs(PLANT).prop == 2) {
                                    puts("You've over-watered the plant! It's shriveling up! It's, it's...");
                                    objs(PLANT).prop = 0;
                                }
                                objs(PLANT2).prop = objs(PLANT).prop;
                                mot = NOWHERE;
                                goto try_move;
                            }
                        } else if (there(RUSTY_DOOR, loc)) {
                            /* Pour water or oil on the door. */
                            switch (obj) {
                                case WATER:
                                    objs(RUSTY_DOOR).prop = 0;
                                    puts("The hinges are quite thoroughly rusted now and won't budge.");
                                    break;
                                case OIL:
                                    objs(RUSTY_DOOR).prop = 1;
                                    puts("The oil has freed up the hinges so that the door will now open.");
                                    break;
                            }
                        } else {
                            puts("Your bottle is empty and the ground is wet.");
                        }
                    } else {
                        puts("You aren't carrying it!");
                    }
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
                        puts("You aren't carrying it!");
                        continue;
                    }
                    if (is_treasure(obj) && is_at_loc(TROLL, loc)) {
                        /* Snarf a treasure for the troll. */
                        drop(obj, R_LIMBO);
                        destroy(TROLL); destroy(TROLL_);
                        drop(NO_TROLL, R_SWSIDE); drop(NO_TROLL_, R_NESIDE);
                        juggle(CHASM); juggle(CHASM_);
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
                    } else if (is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) {
                        puts("The axe bounces harmlessly off the dragon's thick scales.");
                    } else if (is_at_loc(TROLL, loc)) {
                        puts("The troll deftly catches the axe, examines it carefully, and tosses it" SOFT_NL
                             "back, declaring, \"Good workmanship, but it's not valuable enough.\"");
                    } else if (here(BEAR, loc) && objs(BEAR).prop == 0) {
                        /* Throw the axe at the (un-tame) bear. */
                        drop(AXE, loc);
                        objs(AXE).prop = 1;
                        immobilize(AXE);
                        juggle(BEAR);  /* keep bear first in the list */
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
                        if (is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) { ++k; obj = DRAGON; }
                        if (is_at_loc(TROLL, loc)) { ++k; obj = TROLL; }
                        if (here(BEAR, loc) && !objs(BEAR).prop) { ++k; obj = BEAR; }
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
                                objs(BIRD).prop = 0;
                                if (there(SNAKE, R_HMK)) ++lost_treasures;
                                puts("The little bird is now dead.  Its body disappears.");
                                continue;
                            }
                        case DRAGON:
                            if (objs(DRAGON).prop) {
                                puts("For crying out loud, the poor thing is already dead!");
                                continue;
                            } else {
                                /* If you insist on attacking the dragon, you win!
                                 * He dies, the Persian rug becomes free, and R_SCAN2
                                 * takes the place of R_SCAN1 and R_SCAN3. */
                                puts("With what?  Your bare hands?");
                                verb = obj = NOTHING;
                                listen();
                                if (streq(word1, "yes") || streq(word1, "y")) {
                                    puts("Congratulations!  You have just vanquished a dragon with your bare" SOFT_NL
                                         "hands! (Unbelievable, isn't it?)");
                                    objs(DRAGON).prop = 1;  /* dead */
                                    objs(RUG).prop = 0;
                                    mobilize(RUG);
                                    destroy(DRAGON_);
                                    destroy(RUG_);
                                    for (int t = MIN_OBJ; t <= MAX_OBJ; ++t) {
                                        if (there(t, R_SCAN1) || there(t, R_SCAN3))
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
                            puts("With what?  Your bare hands?");
                            continue;
                        case TROLL:
                            puts("Trolls are close relatives with the rocks and have skin as tough as" SOFT_NL
                                 "a rhinoceros hide.  The troll fends off your blows effortlessly.");
                            continue;
                        case BEAR:
                            switch (objs(BEAR).prop) {
                                case 0: puts("With what?  Your bare hands?  Against *HIS* bear hands??"); break;
                                case 3: puts("For crying out loud, the poor thing is already dead!"); break;
                                default: puts("The bear is confused; he only wants to be your friend."); break;
                            }
                            continue;
                        default:
                            puts("Don't be ridiculous!");
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
                case CALM:
                    puts("I'm game. Would you care to explain how?");
                    continue;
                case GO:
                    puts("Where?");
                    continue;
                case RELAX:
                    puts(ok);  /* this corresponds to the command "NOTHING LAMP" */
                    continue;
                case FEEFIE:
                    puts("I don't know how.");  /* "FOO BARS" */
                    continue;
                case BRIEF:
                    puts("On what?");  /* har har har */
                    continue;
                case SCORE:
                case QUIT:
#ifdef SAVE_AND_RESTORE
                case SAVE:
                case RESTORE:
#endif /* SAVE_AND_RESTORE */
                    puts("Eh?");
                    continue;
            }
        get_object:
            word1[0] = toupper(word1[0]);
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
        assert(R_LIMBO <= oldloc && oldloc <= MAX_LOC);
        assert(R_LIMBO < loc && loc <= MAX_LOC);
        newloc = loc;  /* by default we will stay put */
        
        if (mot == CAVE) {
            /* No CAVE appears in the travel table; its sole purpose is to
             * give these messages. */
            if (loc < MIN_IN_CAVE) {
                puts("I can't see where the cave is, but hereabouts no stream can run on" SOFT_NL
                     "the surface for long. I would try the stream.");
            } else {
                puts("I need more detailed instructions to do that.");
            }
        } else if (mot == LOOK) {
            /* Repeat the long description and continue. */
            if (++look_count <= 3) {
                puts("Sorry, but I am not allowed to give more detail.  I will repeat the" SOFT_NL
                     "long description of your location.");
            }
            was_dark = false;  /* pretend it wasn't dark, so you won't fall into a pit */
            places[loc].visits = 0;
        } else {
            if (mot == BACK) {
                Location l = (is_forced(oldloc) ? oldoldloc : oldloc);
                mot = try_going_back_to(l, loc); /* may return NOWHERE */
            }
    
            if (mot != NOWHERE) {
                /* Determine the next newloc. */
                oldoldloc = oldloc;
                oldloc = loc;
                if (determine_next_newloc(loc, &newloc, mot, verb)) {
                    /* Player died trying to cross the troll bridge. */
                    oldoldloc = newloc;  /* if you are revived, you got across */
                    goto death;
                }
            }
        }
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
    puts("The resulting ruckus has awakened the dwarves.  There are now several" SOFT_NL
         "threatening little dwarves in the room with you!  Most of them throw" SOFT_NL
         "knives at you!  All of them get you!");
    quit();
}

int main()
{
#ifdef Z_MACHINE
    puts("\n\n\n\n\n\n\n\n");
#endif /* Z_MACHINE */

    offer(0);
    /* Reading the instructions marks you as a newbie who will need the
     * extra lamp power. However, it will also cost you 5 points. */
    lamp_limit = (hints[0].given ? 1000 : 330);
    build_vocabulary();
    build_travel_table();
    build_object_table();
    simulate_an_adventure();
    return 0;
}

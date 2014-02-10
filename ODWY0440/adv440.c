#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef Z_MACHINE
#ifdef SAVE_AND_RESTORE
extern int attempt_save(void);
extern int attempt_restore(void);
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
    LOW,CANYON,AWKWARD,PANTRY,VIEW,U,D,PIT,OUTDOORS,
    CRACK,STEPS,DOME,LEFT,RIGHT,HALL,JUMP,BARREN,
    OVER,ACROSS,E,W,N,S,NE,SE,SW,NW,HOLE,
    WALL,BROKEN,Y2,CLIMB,LOOK,FLOOR,ROOM,SLIT,
    SLAB,XYZZY,DEPRESSION,ENTRANCE,PLUGH,SECRET,
    CAVE,CROSS,BEDQUILT,PLOVER,ORIENTAL,CAVERN,
    SHELL,RESERVOIR,OFFICE,FORK,
    REFLECT,TESTO,TOWER,BEDCHAMBER,GALLEY,
    MAX_MOTION=GALLEY
} MotionWord;

typedef enum {
    MIN_OBJ=200,
    KEYS=MIN_OBJ, LAMP, GRATE, GRATE_, CAGE, ROD, ROD2, TREADS, TREADS_,
    BIRD, RUSTY_DOOR, PILLOW, SNAKE, FISSURE, FISSURE_, TABLET, CLAM, OYSTER,
    MAG, DWARF, KNIFE, FOOD, BOTTLE, WATER, OIL, MIRROR, MIRROR_, PLANT,
    PLANT2, PLANT2_, STALACTITE, SHADOW, SHADOW_, AXE, DRAWINGS, PIRATE,
    DRAGON, DRAGON_, CHASM, CHASM_, TROLL, TROLL_, NO_TROLL, NO_TROLL_,
    BEAR, MESSAGE, GORGE, MACHINE, BATTERIES, MOSS,
    OWL, WEB, SPIDER, DOCUMENTS, SPOON, HORN, RATS, GIANT, FLAGSTONE,
    GOLD, DIAMONDS, SILVER, JEWELS, COINS, CHEST, EGGS,
    TRIDENT, VASE, EMERALD, PYRAMID, PEARL, RUG, RUG_, SPICES, CHAIN,
    CROWN, TUSK, CHALICE, RUBY, ORB, FAKE_ORB,
    MAX_OBJ=FAKE_ORB
} ObjectWord;

typedef enum {
    MIN_ACTION=300,
    TAKE=MIN_ACTION, DROP, OPEN, CLOSE, ON, OFF, WAVE, CALM, GO,
    RELAX, POUR, EAT, DRINK, RUB, TOSS, WAKE, FEED, FILL, BREAK, BLAST,
    KILL, SAY, READ, FEEFIE, BRIEF, FIND,
    HOOT, BLOW, FLY, RIDE, SLEEP, REST, SCREAM,
    INVENTORY, SCORE,
#ifdef SAVE_AND_RESTORE
    SAVE, RESTORE,
#endif /* SAVE_AND_RESTORE */
    QUIT,
    MAX_ACTION=QUIT
} ActionWord;

typedef enum {
    MIN_MESSAGE=400,
    ABRA=MIN_MESSAGE, HELP, TREES, DIG, LOST, MIST, FUCK, STOP, INFO, SWIM,
    PUSH, EPNS, BONES, DEBRIS, DUNG, GRILL, CURTAIN, PORTCULLIS, STONE,
    MAX_MESSAGE=STONE
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
    new_motion_word("pantr", PANTRY);
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
    new_motion_word("refle", REFLECT);
    new_motion_word("testo", TESTO);
    new_motion_word("tower", TOWER);
    new_motion_word("bedch", BEDCHAMBER);
    new_motion_word("galle", GALLEY);

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
    new_object_word("owl", OWL);
    new_object_word("web", WEB);
    new_object_word("spide", SPIDER);
    new_object_word("docum", DOCUMENTS); new_object_word("legal", DOCUMENTS);
    new_object_word("deed", DOCUMENTS); new_object_word("deeds", DOCUMENTS);
    new_object_word("plan", DOCUMENTS); new_object_word("plans", DOCUMENTS);
    new_object_word("paper", DOCUMENTS);
    new_object_word("spoon", SPOON); new_object_word("inscr", SPOON);
    new_object_word("horn", HORN);
    new_object_word("rat", RATS); new_object_word("rats", RATS);
    new_object_word("giant", GIANT); new_object_word("man", GIANT);
    new_object_word("flags", FLAGSTONE);
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
    new_object_word("shard", VASE); new_object_word("potte", VASE);
    new_object_word("emera", EMERALD);
    new_object_word("plati", PYRAMID); new_object_word("pyram", PYRAMID);
    new_object_word("pearl", PEARL);
    new_object_word("persi", RUG); new_object_word("rug", RUG);
    new_object_word("spice", SPICES);
    new_object_word("chain", CHAIN);
    new_object_word("crown", CROWN);
    new_object_word("ivory", TUSK); new_object_word("tusk", TUSK);
    new_object_word("inlai", CHALICE); new_object_word("chali", CHALICE);
    new_object_word("unico", CHALICE);
    new_object_word("ruby", RUBY);
    /* The noun "GLOBE" works at the cellar view; the nouns "CRYSTAL" and "ORB"
     * work only in the circular cellar itself. */
    new_object_word("orb", ORB); new_object_word("cryst", ORB);
    new_object_word("globe", FAKE_ORB);

    new_action_word("take", TAKE); new_action_word("carry", TAKE);
    new_action_word("keep", TAKE); new_action_word("catch", TAKE);
    new_action_word("captu", TAKE); new_action_word("steal", TAKE);
    new_action_word("get", TAKE); new_action_word("tote", TAKE);
    new_action_word("lift", TAKE); new_action_word("raise", TAKE);
    new_action_word("drop", DROP); new_action_word("relea", DROP);
    new_action_word("free", DROP); new_action_word("disca", DROP);
    new_action_word("dump", DROP);
    new_action_word("open", OPEN); new_action_word("unloc", OPEN);
    new_action_word("close", CLOSE); new_action_word("lock", CLOSE);
    new_action_word("shut", CLOSE);
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
    new_action_word("pour", POUR); new_action_word("empty", POUR);
    new_action_word("eat", EAT); new_action_word("devou", EAT);
    new_action_word("drink", DRINK);
    new_action_word("rub", RUB); new_action_word("polis", RUB);
    new_action_word("throw", TOSS); new_action_word("toss", TOSS);
    new_action_word("wake", WAKE); new_action_word("distu", WAKE);
    new_action_word("feed", FEED);
    new_action_word("fill", FILL);
    new_action_word("break", BREAK); new_action_word("smash", BREAK);
    new_action_word("shatt", BREAK);
    new_action_word("blast", BLAST); new_action_word("deton", BLAST);
    new_action_word("ignit", BLAST); new_action_word("blowu", BLAST);
    new_action_word("explo", BLAST);
    new_action_word("attac", KILL); new_action_word("kill", KILL);
    new_action_word("fight", KILL); new_action_word("hit", KILL);
    new_action_word("strik", KILL);
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
    new_action_word("seek", FIND);
    new_action_word("read", READ); new_action_word("perus", READ);
    new_action_word("hoot", HOOT);
    new_action_word("blow", BLOW); new_action_word("play", BLOW);
    new_action_word("fly", FLY);
    new_action_word("mount", RIDE); new_action_word("ride", RIDE);
    new_action_word("sleep", SLEEP);
    new_action_word("rest", REST);
    new_action_word("screa", SCREAM);
    new_action_word("inven", INVENTORY);
    new_action_word("score", SCORE);
    new_action_word("quit", QUIT); new_action_word("end", QUIT);
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
    new_message_word("push", PUSH); new_message_word("pull", PUSH);
    new_message_word("epns", EPNS);
    new_message_word("bone", BONES); new_message_word("bones", BONES);
    new_message_word("debri", DEBRIS);
    new_message_word("dung", DUNG); new_message_word("dropp", DUNG);
    new_message_word("turd", DUNG); new_message_word("turds", DUNG);
    new_message_word("shit", DUNG);
    new_message_word("grill", GRILL);
    new_message_word("curta", CURTAIN);
    new_message_word("portc", PORTCULLIS);
    new_message_word("stone", STONE); new_message_word("pebbl", STONE);
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

    R_141,
    R_142,
    R_143,
    R_144,
    R_145,
    R_146,
    R_147,
    R_148,
    R_149,
    R_150,
    R_151,
    R_152,
    R_153,
    R_CELLAR,
    R_155,
    R_156,
    R_157,
    R_158,
    R_159,
    R_160,
    R_161,
    R_162,
    R_163,
    R_164,
    R_165,
    R_166,
    R_168,
    R_169,
    R_170,
    R_171,
    R_172,
    R_PANTRY,
    R_174,
    R_175,
    R_176,
    R_177,
    R_178,
    R_179,
    R_180,
    R_181,
    R_182,
    R_183,
    R_184,
    R_185,
    R_186,
    R_187,
    R_188,
    R_189,
    R_190,
    R_191,
    R_192,
    R_193,
    R_194,
    R_195,
    R_197,
    R_198,

    R_NEEND, R_SWEND,

    R_NECK, R_LOSE, R_CLIMB, R_CHECK,
    R_THRU, R_DUCK, R_UPNOUT,
    R_DIDIT,
    R_167, R_196,
    R_199, MAX_LOC = R_199,
    R_PPASS, R_PDROP,
    R_TROLL,
    R_TDROP, R_GRAB,
    FIRST_REMARK
} Location;

typedef struct {
    MotionWord mot;
    int cond;
    Location dest;
} Instruction;

enum flagsBits {
    F_CAVE_HINT  = 0x008,
    F_BIRD_HINT  = 0x010,
    F_SNAKE_HINT = 0x020,
    F_TWIST_HINT = 0x040,
    F_DARK_HINT  = 0x080,
    F_WITT_HINT  = 0x100
};

Instruction travels[950];
Instruction *start[MAX_LOC+2];
struct Place {
    const char *long_desc;
    const char *short_desc;
    unsigned int flags;
    ObjectWord objects;  /* first object at this location, or NOTHING */
    int visits;
};
struct Place places[MAX_LOC+1];

bool now_in_darkness(Location loc);
void describe_object(ObjectWord t, Location loc);

void make_loc(Instruction *q, Location x, const char *l, const char *s, unsigned int f)
{
    static int last_x = 0;
    assert(x == last_x + 1);
    last_x = x;

    places[x].long_desc = l;
    places[x].short_desc = s;
    places[x].flags = f;
    places[x].objects = NOTHING;
    places[x].visits = 0;
    start[x] = q;
}

void make_inst(Instruction *q, MotionWord m, int c, Location d)
{
    assert(&travels[0] <= q && q < &travels[950]);
    assert(m==0 || (MIN_MOTION <= m && m <= MAX_MOTION));
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
             "You're at the end of the road again.", 0);
    make_ins(W, R_HILL); ditto(U); ditto(ROAD);
    make_ins(E, R_HOUSE); ditto(IN); ditto(HOUSE); ditto(ENTER);
    make_ins(S, R_VALLEY); ditto(D); ditto(GULLY); ditto(STREAM); ditto(DOWNSTREAM);
    make_ins(N, R_FOREST); ditto(FOREST);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_HILL,
             "You have walked up a hill, still in the forest.  The road slopes back" SOFT_NL
             "down the other side of the hill.  There is a building in the distance." SOFT_NL
             "To the north lies a snow-capped mountain range with peaks that rise" SOFT_NL
             "into the sky.  To the south, partly obscured by a thin haze, lies a" SOFT_NL
             "white fortress with seven towers.  Beyond the fortress, shimmering in" SOFT_NL
             "the sun, there sparkles a great expanse of water.",
             "You're at the hill in road.", 0);
    make_ins(ROAD, R_ROAD); ditto(HOUSE); ditto(FORWARD); ditto(E); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(N); ditto(S);
    make_loc(q, R_HOUSE,
             "You are inside a building, a well house for a large spring.",
             "You're inside the building.", 0);
    make_ins(ENTER, R_ROAD); ditto(OUT); ditto(OUTDOORS); ditto(W);
    make_ins(XYZZY, R_DEBRIS);
    make_ins(PLUGH, R_Y2);
    make_ins(DOWNSTREAM, remark(17)); ditto(STREAM);
    make_loc(q, R_VALLEY,
             "You are in a valley in the forest beside a stream tumbling along a" SOFT_NL
             "rocky bed.",
             "You're in the valley.", 0);
    make_ins(UPSTREAM, R_ROAD); ditto(HOUSE); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(U);
    make_ins(DOWNSTREAM, R_SLIT); ditto(S); ditto(D);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_FOREST,
             "You are in open forest, with a deep valley to one side.",
             "You're in the forest.", 0);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(D);
    make_cond_ins(FOREST, 50, R_FOREST); ditto(FORWARD); ditto(N);
    make_ins(FOREST, R_FOREST2);
    make_ins(W, R_FOREST); ditto(S);
    make_loc(q, R_FOREST2,
             "You are in open forest near both a valley and a road.",
             "You're in the forest.", 0);
    make_ins(ROAD, R_ROAD); ditto(N);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(W); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(S);
    make_loc(q, R_SLIT,
             "At your feet all the water of the stream splashes into a 2-inch slit" SOFT_NL
             "in the rock.  Downstream the streambed is bare rock.",
             "You're at the slit in the streambed.", 0);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_VALLEY); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W);
    make_ins(DOWNSTREAM, R_OUTSIDE); ditto(ROCK); ditto(BED); ditto(S);
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D);
    make_loc(q, R_OUTSIDE,
             "You are in a 20-foot depression floored with bare dirt.  Set into the" SOFT_NL
             "dirt is a strong steel grate mounted in concrete.  A dry streambed" SOFT_NL
             "leads into the depression.",
             "You're outside the grate.", F_CAVE_HINT);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(S);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_SLIT); ditto(GULLY); ditto(N);
    make_cond_ins(ENTER, unless_prop(GRATE, 0), R_INSIDE); ditto(IN); ditto(D);
    make_ins(ENTER, remark(1));
    make_loc(q, R_INSIDE,
             "You are in a small chamber beneath a 3x3 steel grate to the surface." SOFT_NL
             "A low crawl over cobbles leads inward to the west.",
             "You're below the grate.", 0);
    make_cond_ins(OUT, unless_prop(GRATE, 0), R_OUTSIDE); ditto(U);
    make_ins(OUT, remark(1));
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(IN); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_COBBLES,
             "You are crawling over cobbles in a low passage.  There is a dim light" SOFT_NL
             "at the east end of the passage.",
             "You're in the cobble crawl.", 0);
    make_ins(OUT, R_INSIDE); ditto(SURFACE); ditto(E);
    make_ins(IN, R_DEBRIS); ditto(DARK); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_DEBRIS,
             "You are in a debris room filled with stuff washed in from the surface." SOFT_NL
             "A low wide passage with cobbles becomes plugged with mud and debris" SOFT_NL
             "here, but an awkward canyon leads upward and west.  A note on the wall" SOFT_NL
             "says \"MAGIC WORD XYZZY\".",
             "You're in the debris room.", 0);
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
    make_ins(D, R_DEBRIS); ditto(E);
    make_ins(IN, R_BIRD); ditto(U); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_BIRD,
             "You are in a splendid chamber thirty feet high. The walls are frozen" SOFT_NL
             "rivers of orange stone. An awkward canyon and a good passage exit" SOFT_NL
             "from east and west sides of the chamber.",
             "You're in bird chamber.", F_BIRD_HINT);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(CANYON, R_AWK); ditto(E);
    make_ins(PASSAGE, R_SPIT); ditto(PIT); ditto(W);
    make_loc(q, R_SPIT,
             "At your feet is a small pit breathing traces of white mist. An east" SOFT_NL
             "passage ends here except for a small crack leading on.",
             "You're at the top of the small pit.", 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
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
             "You're in the hall of mists.", 0);
    make_ins(LEFT, R_NUGGET); ditto(S);
    make_ins(FORWARD, R_EFISS); ditto(HALL); ditto(W);
    make_ins(STAIRS, R_HMK); ditto(D); ditto(N);
    make_cond_ins(U, only_if_toting(GOLD), remark(15)); ditto(PIT); ditto(STEPS); ditto(DOME); ditto(PASSAGE); ditto(E);
    make_ins(U, R_SPIT);
    make_ins(Y2, R_JUMBLE);
    make_loc(q, R_NUGGET,
             "This is a low room with a crude note on the wall. The note says," SOFT_NL
             "\"You won't get it up the steps\".",
             "You're in the nugget of gold room.", 0);
    make_ins(HALL, R_EMIST); ditto(OUT); ditto(N);
    make_loc(q, R_EFISS,
             "You are on the east bank of a fissure slicing clear across the hall." SOFT_NL
             "The mist is quite thick here, and the fissure is too wide to jump.",
             "You're on the east bank of the fissure.", 0);
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
             "You are at the west end of the hall of mists. A low wide crawl" SOFT_NL
             "continues west and another goes north. To the south is a little" SOFT_NL
             "passage 6 feet off the floor.",
             "You're at the west end of the hall of mists.", 0);
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
             "You're at the brink of the pit.", 0);
    make_ins(D, R_BIRD); ditto(CLIMB);
    make_ins(W, R_LIKE10);
    make_ins(S, R_DEAD8);
    make_ins(N, R_LIKE12);
    make_ins(E, R_LIKE13);
    make_loc(q, R_ELONG,
             "You are at the east end of a very long hall apparently without side" SOFT_NL
             "chambers.  To the east a low wide crawl slants up.  To the north a" SOFT_NL
             "round two-foot hole slants down.",
             "You're at the east end of the long hall.", 0);
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL);
    make_ins(W, R_WLONG);
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE);
    make_loc(q, R_WLONG,
             "You are at the west end of a very long featureless hall.  The hall" SOFT_NL
             "joins up with a narrow north/south passage.",
             "You're at the west end of the long hall.", 0);
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
    twist(R_DIFF4,R_DIFF1,R_DIFF7,R_DIFF5,R_149,R_DIFF0,R_DIFF3,R_DIFF2,R_DIFF10,R_DIFF8,R_DIFF6,
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
        "You're in the Hall of the Mt King.", F_SNAKE_HINT);
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
        "You are in the south side chamber.  A spiral staircase descends into" SOFT_NL
        "a shaft which has been bored into the rock.  The staircase is very" SOFT_NL
        "old and ridden through with rust.  A breeze blows towards the shaft.",
        "You're in the south side chamber.", 0);
    make_ins(HALL, R_HMK); ditto(N);
    make_ins(D, R_197);
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
             "You're at the window on the pit.", 0);
    make_ins(E, R_Y2); ditto(Y2);
    make_ins(JUMP, R_NECK);
    make_ins(REFLECT, R_WINDOW);
    make_loc(q, R_DIRTY,
             "You are in a dirty broken passage.  To the east is a crawl.  To the" SOFT_NL
             "west is a large passage.  Above you is a hole to another passage.",
             "You're in the dirty passage.", 0);
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
             "You're in pit by stream.", 0);
    make_ins(CLIMB, R_CLEAN); ditto(U); ditto(OUT);
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D); ditto(UPSTREAM); ditto(DOWNSTREAM);
    make_loc(q, R_DUSTY,
             "You are in a large room full of dusty rocks.  There is a big hole in" SOFT_NL
             "the floor.  There are cracks everywhere, and a passage leading east.",
             "You're in the dusty rock room.", 0);
    make_ins(E, R_DIRTY); ditto(PASSAGE);
    make_ins(D, R_COMPLEX); ditto(HOLE); ditto(FLOOR);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_loc(q, R_COMPLEX,
             "You are at a complex junction.  A low hands-and-knees passage from the" SOFT_NL
             "north joins a higher crawl from the east to make a walking passage" SOFT_NL
             "going west.  There is also a large room above.  The air is damp here.",
             "You're at the complex junction.", 0);
    make_ins(U, R_DUSTY); ditto(CLIMB); ditto(ROOM);
    make_ins(W, R_BEDQUILT); ditto(BEDQUILT);
    make_ins(N, R_SHELL); ditto(SHELL);
    make_ins(E, R_ANTE);
    make_loc(q, R_SHELL,
             "You're in a large room carved out of sedimentary rock.  The floor and" SOFT_NL
             "walls are littered with bits of shells embedded in the stone.  A" SOFT_NL
             "shallow passage proceeds downward, and a somewhat steeper one leads" SOFT_NL
             "up.  A low hands-and-knees passage enters from the south.",
             "You're in the shell room.", 0);
    make_ins(U, R_ARCHED); ditto(HALL);
    make_ins(D, R_RAGGED);
    make_cond_ins(S, only_if_toting(CLAM), remark(4));
    make_cond_ins(S, only_if_toting(OYSTER), remark(5));
    make_ins(S, R_COMPLEX);
    make_loc(q, R_ARCHED,
             "You are in an arched hall.  A coral passage once continued up and east" SOFT_NL
             "from here, but is now blocked by debris.  The air smells of sea water.",
             "You're in the arched hall.", 0);
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
             "passages go west, south and north-east.  The remnants of recent" SOFT_NL
             "digging are evident.  A sign in midair says: \"WARNING " EMDASH("-") SOFT_NL
             "DANGEROUS AREA!\" and a smaller one beneath reads: \"Cave under" SOFT_NL
             "construction beyond this point.  Proceed at your own risk!  (Witt" SOFT_NL
             "Construction Company).\"",
             "You're in the anteroom.", 0);
    make_ins(NE, R_174);
    make_ins(W, R_COMPLEX);
    make_ins(E, R_WITT);
    make_ins(S, R_141);
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
    make_ins(S, R_190);
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
             "You're in the Swiss cheese room.", 0);
    make_ins(NE, R_BEDQUILT);
    make_ins(W, R_E2PIT);
    make_cond_ins(S, 80, remark(6));
    make_ins(CANYON, R_TALL);
    make_ins(E, R_SOFT);
    make_cond_ins(NW, 50, remark(6));
    make_ins(ORIENTAL, R_ORIENTAL);
    make_loc(q, R_SOFT,
             "You are in the soft room.  The walls are covered with heavy curtains," SOFT_NL
             "the floor with a thick pile carpet and moss covers the ceiling.",
             "You're in the soft room.", 0);
    make_ins(W, R_SWISS);
    make_ins(E, R_192);
    make_ins(SE, R_153);
    make_ins(N, R_196); ditto(S); ditto(NE); ditto(SE); ditto(NW); ditto(SW);  // TODO: SE appears twice
    make_loc(q, R_E2PIT,
             "You are at the east end of the twopit room.  The floor here is" SOFT_NL
             "littered with thin rock slabs, which make it easy to descend the pits." SOFT_NL
             "There is a path here bypassing the pits to connect passages from east" SOFT_NL
             "and west.  There are holes all over, but the only big one is on the" SOFT_NL
             "wall directly over the west pit where you can't get to it.",
             "You're at the east end of the twopit room.", 0);
    make_ins(E, R_SWISS);
    make_ins(W, R_W2PIT); ditto(ACROSS);
    make_ins(D, R_EPIT); ditto(PIT);
    make_loc(q, R_W2PIT,
             "You are at the west end of the twopit room.  There is a large hole in" SOFT_NL
             "the wall above the pit at this end of the room.",
             "You're at west end of the twopit room.", 0);
    make_ins(E, R_E2PIT); ditto(ACROSS);
    make_ins(W, R_SLAB); ditto(SLAB);
    make_ins(D, R_WPIT); ditto(PIT);
    make_ins(HOLE, remark(8));
    make_loc(q, R_EPIT,
             "You are at the bottom of the eastern pit in the Twopit Room.  There is" SOFT_NL
             "a small pool of oil in one corner of the pit.",
             "You're in the east pit.", 0);
    make_ins(U, R_E2PIT); ditto(OUT);
    make_loc(q, R_WPIT,
             "You are at the bottom of the western pit in the Twopit Room.  There is" SOFT_NL
             "a large hole in the wall about 25 feet above you.",
             "You're in the west pit.", 0);
    make_ins(U, R_W2PIT); ditto(OUT);
    make_cond_ins(CLIMB, unless_prop(PLANT, 2), R_CHECK);
    make_ins(CLIMB, R_CLIMB);
    make_loc(q, R_NARROW,
             "You are in a long, narrow corridor stretching out of sight to the" SOFT_NL
             "west.  At the eastern end is a hole through which you can see a" SOFT_NL
             "profusion of leaves.",
             "You're in the narrow corridor.", 0);
    make_ins(D, R_WPIT); ditto(CLIMB); ditto(E);
    make_ins(JUMP, R_NECK);
    make_ins(W, R_GIANT);
    make_loc(q, R_GIANT,
             "You are in the giant room.  The ceiling here is too high up for your" SOFT_NL
             "lamp to show it.  Cavernous passages lead east, north, and south.  On" SOFT_NL
             "the west wall is scrawled the inscription, \"FEE FIE FOE FOO\" [sic].",
             "You're in the giant room.", 0);
    make_ins(S, R_NARROW);
    make_ins(E, R_BLOCK);
    make_ins(N, R_IMMENSE);
    make_loc(q, R_BLOCK,
             "The passage here is blocked by a recent cave-in.",
             NULL, 0);
    make_ins(S, R_GIANT); ditto(OUT);
    make_loc(q, R_IMMENSE,
             "You are at one end of an immense north/south passage.",
             NULL, 0);
    make_ins(S, R_GIANT); ditto(PASSAGE);
    make_cond_ins(N, unless_prop(RUSTY_DOOR, 0), R_FALLS); ditto(ENTER); ditto(CAVERN);
    make_ins(N, remark(9));
    make_loc(q, R_FALLS,
             "You are in a magnificent cavern with a rushing stream, which cascades" SOFT_NL
             "over a sparkling waterfall into a roaring whirlpool that disappears" SOFT_NL
             "through a hole in the floor.  Passages exit to the south and west.",
             "You're in the cavern with the waterfall.", 0);
    make_ins(S, R_IMMENSE); ditto(OUT);
    make_ins(W, R_INCLINE);
    make_loc(q, R_INCLINE,
             "You are at the top of a steep incline above a large room.  You could" SOFT_NL
             "climb down here, but you would not be able to climb up.  There is a" SOFT_NL
             "passage leading back to the north.",
             "You're at the steep incline above a large room.", 0);
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
             "You're at the junction of three secret canyons.", 0);
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
             "You're at the top of the stalactite.", 0);
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
             "You're at the window on the pit.", 0);
    make_ins(W, R_SJUNC);
    make_ins(JUMP, R_NECK);
    make_ins(REFLECT, R_WINDOE);
    make_loc(q, R_ORIENTAL,
             "This is the oriental room.  Ancient oriental cave drawings cover the" SOFT_NL
             "walls.  A gently sloping passage leads upward to the north, another" SOFT_NL
             "passage leads SE, and a hands-and-knees crawl leads west.",
             "You're in the oriental room.", 0);
    make_ins(SE, R_SWISS);
    make_ins(W, R_LOW); ditto(CRAWL);
    make_ins(U, R_MISTY); ditto(N); ditto(CAVERN);
    make_loc(q, R_MISTY,
             "You are following a wide path around the outer edge of a large cavern." SOFT_NL
             "Far below, through a heavy white mist, strange splashing noises can be" SOFT_NL
             "heard.  The mist rises up through a fissure in the ceiling.  The" SOFT_NL
             "path exits to the south and west and passes alongside the walls of" SOFT_NL
             "the cavern, which have crude drawings on them.  The drawings" SOFT_NL
             "are of animals long extinct and a tiny people.  The people seem to" SOFT_NL
             "resemble dwarves although some of the figures are considerably" SOFT_NL
             "larger than those of the dwarves.  On the wall opposite is a" SOFT_NL
             "large drawing of a castle, complete with turrets and battlements.",
             "You're in the misty cavern.", 0);
    make_ins(S, R_ORIENTAL); ditto(ORIENTAL);
    make_ins(W, R_ALCOVE);
    make_loc(q, R_ALCOVE,
             "You are in an alcove.  A small NW path seems to widen after a short" SOFT_NL
             "distance.  An extremely tight tunnel leads east.  It looks like a very" SOFT_NL
             "tight squeeze.  An eerie light can be seen at the other end.",
             "You're in the alcove.", F_DARK_HINT);
    make_ins(NW, R_MISTY); ditto(CAVERN);
    make_ins(E, R_PPASS); ditto(PASSAGE);
    make_ins(E, R_PLOVER);  /* never performed, but seen by "BACK" */
    make_loc(q, R_PLOVER,
             "You're in a small chamber lit by an eerie green light.  An extremely" SOFT_NL
             "narrow tunnel exits to the west.  A dark corridor leads NE.",
             "You're in the plover room.", F_DARK_HINT);
    make_ins(W, R_PPASS); ditto(PASSAGE); ditto(OUT);
    make_ins(W, R_ALCOVE);  /* never performed, but seen by "BACK" */
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP);
    make_ins(PLOVER, R_Y2);
    make_ins(NE, R_DARK); ditto(DARK);
    make_loc(q, R_DARK,
             "You're in the dark-room.  A corridor leading south is the only exit.",
             "You're in the dark-room.", F_DARK_HINT);
    make_ins(S, R_PLOVER); ditto(PLOVER); ditto(OUT);
    make_loc(q, R_SLAB,
             "You are in a large low circular chamber whose floor is an immense slab" SOFT_NL
             "fallen from the ceiling (Slab Room).  There once were large passages" SOFT_NL
             "to the east and west, but they are now filled with boulders.  Low" SOFT_NL
             "small passages go north and south, and the south one quickly bends" SOFT_NL
             "east around the boulders.",
             "You're in the slab room.", 0);
    make_ins(S, R_W2PIT);
    make_ins(U, R_189); ditto(CLIMB);
    make_ins(N, R_BEDQUILT);
    make_loc(q, R_ABOVER,
             "You are in a secret N/S canyon above a large room.",
             NULL, 0);
    make_ins(D, R_190);
    make_ins(SLAB, R_SLAB);
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
             "You're in the mirror canyon.", 0);
    make_ins(S, R_ABOVER);
    make_ins(N, R_RES); ditto(RESERVOIR);
    make_loc(q, R_RES,
             "You are at the edge of a large underground reservoir.  An opaque cloud" SOFT_NL
             "of white mist fills the room and rises rapidly upward.  The lake is" SOFT_NL
             "fed by a stream, which tumbles out of a hole in the wall about 10 feet" SOFT_NL
             "overhead and splashes noisily into the water somewhere within the" SOFT_NL
             "mist.  A passage goes back toward the south, although you" SOFT_NL
             "can see to the north what appears to be a large cavern, just" SOFT_NL
             "discernible through a dense wall of mist.",
             "You're at the reservoir.", 0);
    make_ins(S, R_MIRROR); ditto(OUT);
    make_ins(ENTER, R_156); ditto(W); ditto(N); ditto(NW);

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
             "You're in the secret E/W canyon above a tight canyon.", 0);
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
             "You're in the sloping corridor.", 0);
    make_ins(D, R_LOW);
    make_ins(U, R_SWSIDE);
    make_loc(q, R_SWSIDE,
             "You are on one side of a large, deep chasm.  A heavy white mist rising" SOFT_NL
             "up from below obscures all view of the far side.  A SW path leads away" SOFT_NL
             "from the chasm into a winding corridor.",
             "You're on SW side of the chasm.", 0);
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
             "You're on NE side of the chasm.", 0);
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
             "You're in the corridor.", 0);
    make_ins(W, R_NESIDE);
    make_ins(E, R_FORK); ditto(FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_FORK,
             "The path forks here.  The left fork leads northeast.  A dull rumbling" SOFT_NL
             "seems to get louder in that direction.  The right fork leads southeast" SOFT_NL
             "down a gentle slope.  The main corridor enters from the west.",
             "You're at the fork in the path.", 0);
    make_ins(W, R_CORR);
    make_ins(NE, R_WARM); ditto(LEFT);
    make_ins(SE, R_LIME); ditto(RIGHT); ditto(D);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_WARM,
             "The walls are quite warm here.  From the north can be heard a steady" SOFT_NL
             "roar, so loud that the entire cave seems to be trembling.  Another" SOFT_NL
             "passage leads south, and a low crawl goes east.",
             "You're at the junction with the warm walls.", 0);
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
             "You're at the breath-taking view.", 0);
    make_ins(S, R_WARM); ditto(PASSAGE); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(D, remark(13)); ditto(JUMP);
    make_loc(q, R_CHAMBER,
             "You are in a small chamber filled with large boulders.  The walls are" SOFT_NL
             "very warm, causing the air in the room to be almost stifling from the" SOFT_NL
             "heat.  The only exit is a crawl heading west, through which is coming" SOFT_NL
             "a low rumbling.",
             "You're in the chamber of boulders.", 0);
    make_ins(W, R_WARM); ditto(OUT); ditto(CRAWL);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_LIME,
             "You are walking along a gently sloping north/south passage lined with" SOFT_NL
             "oddly shaped limestone formations.",
             "You're in the limestone passage.", 0);
    make_ins(N, R_FORK); ditto(U); ditto(FORK);
    make_ins(S, R_FBARR); ditto(D); ditto(BARREN);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_FBARR,
             "You are standing at the entrance to a large, barren room.  A sign" SOFT_NL
             "posted above the entrance reads:  \"CAUTION!  BEAR IN ROOM!\"",
             "You're in the front of the barren room.", 0);
    make_ins(W, R_LIME); ditto(U);
    make_ins(FORK, R_FORK);
    make_ins(E, R_BARR); ditto(IN); ditto(BARREN); ditto(ENTER);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_BARR,
             "You are inside a barren room.  The center of the room is completely" SOFT_NL
             "empty except for some dust.  Marks in the dust lead away toward the" SOFT_NL
             "far end of the room.  The only exit is the way you came in.",
             "You're in the barren room.", 0);
    make_ins(W, R_FBARR); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);

    make_loc(q, R_141,
             "You are in a magnificently ornate room that looks like a place" SOFT_NL
             "of worship for some old and mysterious religion. There are exits" SOFT_NL
             "to the north, west and east and a spiral staircase leading both" SOFT_NL
             "up and down. The ceiling of this chapel seems to be made of large" SOFT_NL
             "wooden beams. Hanging from one of these beams is a thick rope.",
             "You're in the chapel.", 0);
    make_ins(TESTO, R_HOUSE);
    make_ins(CLIMB, R_144);
    make_ins(N, R_ANTE);
    make_cond_ins(E, 100, R_142);  /* dwarves not permitted */
    make_cond_ins(W, 100, R_169);  /* dwarves not permitted */
    make_ins(D, R_168);
    make_loc(q, R_142,
             "You are in a room that appears to be a stable for a fearsome" SOFT_NL
             "animal.  Against one wall is a battered and dirty trough that" SOFT_NL
             "is quite empty and on the other wall is a huge harness.  Beside" SOFT_NL
             "the harness is a small window that overlooks a courtyard.  The" SOFT_NL
             "courtyard is deserted and shows no signs of any recent activity." SOFT_NL
             "At the far end of the stable is a wooden partition that has" SOFT_NL
             "numerous dents and holes in it and you can see that it is" SOFT_NL
             "securely fixed to the massive stone walls so that whatever" SOFT_NL
             "is behind it cannot get out.  If you listen carefully you can" SOFT_NL
             "hear the muted sounds of growling and the scratching of claws" SOFT_NL
             "against wood.  The only exit is the way you came in.",
             "You're in the stable room.", 0);
    make_ins(W, R_141);
    make_loc(q, R_143,
             "You are in a chamber which is obviously some sort of living quarters." SOFT_NL
             "The room is lit, although you cannot determine how. At the far end" SOFT_NL
             "is a very large solidly built couch.",
             "You're in the living quarters.", 0);
    make_ins(E, R_PANTRY);
    make_cond_ins(N, 100, R_145);
    make_ins(S, R_146);
    make_loc(q, R_144,
             "You have climbed the rope and crawled into a small recess in the" SOFT_NL
             "beams.",
             "You're in the recess in the beams.", 0);
    make_ins(D, R_141); ditto(OUT);
    make_loc(q, R_145,
             "You are in a dungeon.  The walls and ceiling appear to be made of" SOFT_NL
             "solid rock and the floor is made up of tightly fitting flagstones." SOFT_NL
             "High on the walls are some stanchions for chains, but the chains are" SOFT_NL
             "no longer in place.  The only sound, apart from your own breathing," SOFT_NL
             "is the occasional plop of water as it drips from the ceiling.  The" SOFT_NL
             "atmosphere is dank and clammy and there is an unusual smell in the" SOFT_NL
             "air.  A passage leads back to the south.",
             "You're in the dungeon.", 0);
    make_ins(S, R_GRAB); ditto(OUT);

    /* This is the spider maze. */
    const char *tangled = "You are in a tangled web of intersecting passages.";
    make_loc(q, R_146, tangled, NULL, 0);
      make_ins(N, R_GRAB);
      make_ins(E, R_147);
      make_ins(W, R_151);
      make_ins(U, R_150);
      make_cond_ins(D, 100, R_148);
      make_ins(PANTRY, R_PANTRY);
    make_loc(q, R_147, tangled, NULL, 0);
      make_ins(N, R_146);
      make_ins(E, R_147); ditto(D);
      make_ins(W, R_151);
      make_cond_ins(S, 100, R_148);
      make_ins(U, R_149);
    make_loc(q, R_148, tangled, NULL, 0);
      make_ins(N, R_147);
      make_ins(E, R_148); ditto(D);
      make_ins(W, R_146);
      make_ins(S, R_149);
      make_ins(U, R_150);
    make_loc(q, R_149, tangled, NULL, 0);
      make_ins(N, R_147);
      make_cond_ins(E, 100, R_148);
      make_ins(W, R_151);
      make_ins(U, R_150);
    make_loc(q, R_150, tangled, NULL, 0);
      make_ins(N, R_151);
      make_ins(E, R_146);
      make_ins(W, R_150); ditto(U);
      make_ins(S, R_149);
      make_cond_ins(D, 100, R_148);
    make_loc(q, R_151, tangled, NULL, 0);
      make_ins(N, R_151); ditto(W);
      make_ins(E, R_146);
      make_ins(S, R_149);
      make_ins(U, R_150);
      make_ins(D, R_147);

    make_loc(q, R_152,
             "You are in a room which appears to be a basement for a very large" SOFT_NL
             "building. There is an opening to the south above which hangs a slab" SOFT_NL
             "of rock precisely balanced by counterweights suspended from the" SOFT_NL
             "ceiling.  A notice carved in the rock says \"Wizards only, Keep Out\"" SOFT_NL
             "There are some steps leading upward.",
             "You're in the basement.", 0);
    make_cond_ins(S, 100, R_CELLAR);
    make_ins(U, R_153);
    make_loc(q, R_153,
             "You are at an entrance to a large room which you cannot see into" SOFT_NL
             "because of a curtain blocking your view. There are flights of" SOFT_NL
             "steps here leading both upward and downward.",
             "You're at the curtain entrance.", 0);
    make_ins(D, R_152);
    make_ins(U, R_PANTRY);
    make_ins(ENTER, R_SOFT); ditto(IN); ditto(NW);
    make_loc(q, R_CELLAR,
             "You are in a round cellar made completely of brick. In the center" SOFT_NL
             "of the floor is a small circular drain, towards which the floor" SOFT_NL
             "slopes.  Set in the south wall is a portcullis which is so rusty" SOFT_NL
             "it clearly will never move again. Though the portcullis a small" SOFT_NL
             "chamber with a spiral staircase can be seen.  An entrance leads" SOFT_NL
             "north but it is blocked by a large stone slab.",
             "You're in the circular cellar.", 0);
    make_ins(BACK, R_CELLAR);  /* perhaps because every room needs at least one exit? TODO investigate this line */
    const char *causeway =
        "In front of you, just discernible in the gloom, lies an old streambed" SOFT_NL
        "which has been long dry and you can just determine that there is a" SOFT_NL
        "stone causeway leading across the gap.";
    make_loc(q, R_155, causeway, "You're west of the causeway.", 0);
    make_ins(OVER, R_156); ditto(ACROSS); ditto(CROSS); ditto(E);
    make_cond_ins(E, 100, R_195); ditto(N); ditto(S); ditto(NE); ditto(SE);
    make_ins(D, R_199);
    make_loc(q, R_156, causeway, "You're east of the causeway.", 0);
    make_ins(OVER, R_155); ditto(ACROSS); ditto(CROSS); ditto(W);
    make_ins(S, R_RES); ditto(SE); ditto(SW);
    make_ins(D, R_199);
    make_loc(q, R_157, "", NULL, 0); /* unused? TODO remove these rooms */
    make_loc(q, R_158, "", NULL, 0);
    make_loc(q, R_159, "", NULL, 0);
    make_loc(q, R_160,  /* completely unreachable? TODO investigate these rooms */
             "You are at a window halfway up a flight of steps. The view is" SOFT_NL
             "quite beautiful, a lake with rolling green meadows beyond it," SOFT_NL
             "gradually rising to large snow-capped mountains.",
             NULL, 0);
    make_cond_ins(U, 50, R_161);
    make_cond_ins(U, 50, R_162);
    make_ins(U, R_163); ditto(TOWER);
    make_ins(D, R_CELLAR); ditto(BEDCHAMBER);
    make_loc(q, R_161,
             "You are at a window halfway up a flight of steps. The view is" SOFT_NL
             "quite beautiful, a deep ravine with a raging torrent" SOFT_NL
             "tumbling over rapids.",
             NULL, 0);
    make_cond_ins(U, 50, R_162);
    make_ins(U, R_163); ditto(TOWER);
    make_cond_ins(D, 50, R_160);
    make_ins(D, R_CELLAR); ditto(BEDCHAMBER);
    make_loc(q, R_162,
             "You are at a window halfway up a flight of steps. The view is" SOFT_NL
             "quite beautiful, below you is a cliff, an enraged sea battering" SOFT_NL
             "at its base.",
             NULL, 0);
    make_ins(U, R_163); ditto(TOWER);
    make_cond_ins(D, 50, R_161);
    make_cond_ins(D, 50, R_160);
    make_ins(D, R_CELLAR); ditto(BEDCHAMBER);  /* end TODO investigate these rooms */
    make_loc(q, R_163, NULL, NULL, 0);  /* dwarves' quarters -- no access */
    make_ins(D, R_149);  /* dwarves reappear in the tangled web of passages */
    make_loc(q, R_164, NULL, NULL, 0);  /* unused? TODO remove this room */
    make_loc(q, R_165,
             "You are in a winding earthen passage which slopes down to the south." SOFT_NL
             "The lower end of the passage becomes narrower and damp.",
             NULL, 0);
    make_ins(D, R_166); ditto(S);
    make_loc(q, R_166,
             "You are in a small circular chamber which appears to be a disused" SOFT_NL
             "well. Looking up, you can see that the walls of the well are" SOFT_NL
             "damp and crumbling, but should be easy to climb. However, there is" SOFT_NL
             "no indication of an opening at the top of the well." SOFT_NL
             "An earthen passage leads up and to the north.",
             NULL, 0);
    make_ins(U, R_165); ditto(N);
    make_ins(CLIMB, R_167);
    make_loc(q, R_168,
             "You are in a crypt.  All around you are the tombs of people long" SOFT_NL
             "dead.  The dust on the vaults would seem to indicate that the place" SOFT_NL
             "is rarely used. High above you massive cobwebs almost cover the" SOFT_NL
             "ceiling.  A spiral staircase which leads upwards is the only exit." SOFT_NL
             "Brrr! It is cold down here!",
             "You're in the crypt.", 0);
    make_ins(U, R_141); ditto(OUT);
    make_loc(q, R_169,
             "You are in a chamber above the entrance to the chapel. A spiral" SOFT_NL
             "staircase leads down and openings lead to the north and west.",
             "You're in the chamber above the chapel.", 0);
    make_ins(D, R_141); ditto(OUT);
    make_ins(N, R_170);
    make_ins(W, R_PANTRY);
    make_loc(q, R_170,
             "You are at the south end of a long low attic.",
             NULL, 0);
    make_ins(S, R_169);
    make_ins(N, R_171);
    make_ins(SW, R_PANTRY);
    make_loc(q, R_171,
             "You are at the north end of a long low attic.\n"
             "A passage leads on to the north.",
             NULL, 0);
    make_ins(S, R_170);
    make_ins(N, R_172);
    make_loc(q, R_172,
             "This must be a thieves den!\n"
             "High on one wall, out of reach, hangs a large bag labelled \"SWAG\"" SOFT_NL
             "and a black mask.  The only exit is to the south.",
             "You're in the thieves den.", 0);
    make_ins(S, R_171); ditto(OUT);
    make_loc(q, R_PANTRY,
             "You are in a room which shows signs of having been used as a pantry." SOFT_NL
             "The dust on the shelves would seem to indicate that it has had" SOFT_NL
             "little use.  The walls are damp and there is a stream of water" SOFT_NL
             "running down the walls in one corner. Passages lead to the east," SOFT_NL
             "northeast and west. From an archway to the north, stone steps" SOFT_NL
             "lead down.",
             "You're in the pantry.", 0);
    make_ins(W, R_GRAB);
    make_ins(E, R_169);
    make_ins(NE, R_170);
    make_ins(D, R_153); ditto(N);
    make_loc(q, R_174,
             "You are in a long north/south passage whose roof is supported" SOFT_NL
             "in places by wooden props.  Some of the props have recently been" SOFT_NL
             "replaced with new timber, but fresh rock falls are also apparent." SOFT_NL
             "One of these falls has exposed a fracture in the west wall which you" SOFT_NL
             "could squeeze through.",
             "You're in the unsafe tunnel.", 0);
    make_ins(S, R_ANTE);
    make_ins(N, R_175);
    make_cond_ins(CRACK, 100, R_TDROP); ditto(W); ditto(HOLE); ditto(SLIT);
    make_loc(q, R_175,
             "A smell of decaying timber fills this part of the passage." SOFT_NL
             "To the north the passage is blocked by a larger than usual rock fall" SOFT_NL
             "and the whole passage looks as if it could collapse at any moment.",
             NULL, 0);
    make_ins(S, R_174); ditto(OUT);
    make_loc(q, R_176,
             "You are in a very low sloping room whose roof is barely 3' from" SOFT_NL
             "the floor.  At the lower end a trickle of water enters from a" SOFT_NL
             "tunnel in the west and runs down a shaft in the floor.  At the" SOFT_NL
             "upper end the roof nearly meets the floor to give a narrow exit." SOFT_NL
             "Water drips from the roof making everything damp and the floor" SOFT_NL
             "slippery. There are no signs that anyone has been this way before.",
             "You're in the very low room.", 0);
    make_ins(U, R_TDROP); ditto(E);
    make_ins(W, R_177); ditto(UPSTREAM);
    make_ins(D, R_178);
    make_loc(q, R_177,
             "The passage opens into a sizable cave which does not appear to" SOFT_NL
             "have any other exit although a number of huge bones lie here." SOFT_NL
             "Close inspection shows that they are very old and are covered with" SOFT_NL
             "strange score marks which have been made recently. There are also" SOFT_NL
             "some droppings which might have been made by rats if they weren't" SOFT_NL
             "so large.",
             "You're in the bones room.", 0);
    make_ins(E, R_176); ditto(OUT);
    make_loc(q, R_178,
             "You are in an inclined shaft which steepens at this point. The walls" SOFT_NL
             "are covered in a thin layer of slime making it very slippery.  Below" SOFT_NL
             "you the shaft is filled with dark water which surges rhythmically." SOFT_NL
             "The smell of the sea is mingled with an altogether more unpleasant" SOFT_NL
             "odour.",
             "You're on the inclined shaft.", 0);
    make_ins(U, R_176); ditto(OUT);
    make_ins(JUMP, R_181); ditto(D);

    make_loc(q, R_179,
             "You are in an inclined shaft which steepens at this point. The walls" SOFT_NL
             "of the shaft are covered in a thin layer of slime making it very" SOFT_NL
             "slippery.  Below you the shaft steepens and enters a N/S culvert" SOFT_NL
             "partially filled with a brackish evil smelling liquid.  You could" SOFT_NL
             "slide down here but you could not climb back up.",
             "You're on the inclined shaft above a N/S culvert.", 0);
    make_ins(U, R_176);
    make_ins(JUMP, R_181); ditto(D);
    make_loc(q, R_180,
             "The sea can be seen through a strong metal grill which spans the" SOFT_NL
             "tunnel.  A quantity of debris is stacked against the grill and pieces" SOFT_NL
             "of debris are caught in the grill nearly to the roof.  High on the" SOFT_NL
             "opposite side of the culvert is a line scored into the brick" SOFT_HYPHEN
             "work with a faded ornate inscription above it that reads:" SOFT_NL
             "\"CAUTION " EMDASH(" - ") " HIGH TIDE LEVEL!\"",
             "You're at the sea-view.", 0);
    make_ins(S, R_181); ditto(IN);
    make_loc(q, R_181,
             "The brick culvert goes north and south. It seems to narrow to the" SOFT_NL
             "south.",
             NULL, 0);
    make_ins(N, R_180);
    make_ins(S, R_182);
    make_loc(q, R_182,
             "Sewage pours into the culvert from a large number of sewage pipes" SOFT_NL
             "of all sizes.  Those entering from the east, se, south, sw, west and" SOFT_NL
             "nw are large enough to crawl through. A culvert exits to the north.",
             "You're at the south end of the culvert.", 0);
    make_ins(N, R_181);
    make_ins(E, R_183); ditto(S); ditto(SE);
    make_ins(W, R_191); ditto(NW);
    make_ins(SW, R_186);
    make_loc(q, R_183,
             "You are at a junction of 3 sewer pipes which go se, sw and north.",
             NULL, 0);
    make_ins(N, R_182);
    make_ins(SE, R_184); ditto(SW);
    make_loc(q, R_184,
             "The large diameter pipe from the north splits into 2 smaller diameter" SOFT_NL
             "pipes going south and south-east. You could get through either of" SOFT_NL
             "them but it does not look advisable.",
             "You are at the large sewer pipe.", 0);
    make_ins(N, R_183);
    make_ins(S, R_185); ditto(SE);
    make_loc(q, R_185,
             "The sewer pipe has got too narrow, you will have to go back.",
             "Dead end, the sewer pipe has become too narrow.", 0);
    make_ins(N, R_184); ditto(OUT);
    make_loc(q, R_186,
             "You are at a junction of 3 sewer pipes, which has partially collapsed" SOFT_NL
             "where a stream enters the pipe. You cannot get past but you could" SOFT_NL
             "crawl through the fracture in the pipe where the stream enters.",
             NULL, 0);
    make_ins(NE, R_182);
    make_ins(CRAWL, R_187); ditto(IN); ditto(U); ditto(CRACK);
    make_loc(q, R_187,
             "You're in a maze of twisting passages through which a stream cascades.",
             NULL, 0);
    make_cond_ins(E, 60, R_BEDQUILT); ditto(W); ditto(N); ditto(S); ditto(U);
    make_ins(E, R_190); ditto(W); ditto(N); ditto(S); ditto(U);  /* TODO: dittoes are unneeded, right? */
    make_ins(DOWNSTREAM, R_187); ditto(D);
    make_loc(q, R_188, NULL, NULL, 0);  /* Dwarves' store (no access) */
    make_ins(OUT, R_163);
    make_loc(q, R_189,
             "You are at the west end of a very large and long tunnel. To the west" SOFT_NL
             "the way is almost blocked by rock falls. Several very large" SOFT_NL
             "footprints can be seen indistinctly in the dust. High above you there" SOFT_NL
             "is a huge arrow on the wall pointing east.",
             NULL, 0);
    make_ins(D, R_SLAB); ditto(W);
    make_ins(E, R_190);
    make_loc(q, R_190,
             "You are in a very large room with several rock falls at the eastern" SOFT_NL
             "end. A very large passage goes west or you could crawl between" SOFT_NL
             "some of the large boulders in the rockfalls.",
             NULL, 0);
    make_ins(W, R_189);
    make_ins(E, R_ABOVER); ditto(CRAWL); ditto(U);
    make_loc(q, R_191,
             "The pipe has got too narrow, you will have to go back.",
             NULL, 0);
    make_ins(E, R_182); ditto(OUT);
    make_loc(q, R_192,
             "You are in a small, low cavern that is completely bare.  On the" SOFT_NL
             "walls are scratch marks that look as if they were made by tiny" SOFT_NL
             "claws.  The cavern exits to the west and the north.",
             "You're in the low cavern.", 0);
    make_ins(W, R_SOFT); ditto(OUT);
    make_ins(N, R_193);
    make_loc(q, R_193,
             "You are in a long, low passage that appears to have made by some" SOFT_NL
             "subterranean water source.  The passage itself is of bare rock," SOFT_NL
             "polished smooth by the water that apparently flows no longer, for" SOFT_NL
             "the floor and walls are quite dry.  The passage heads north-south" SOFT_NL
             "and bends after a short distance.",
             "You're in the low, long passage.", 0);
    make_ins(S, R_192);
    make_ins(N, R_194);
    make_loc(q, R_194,
             "You are in a large cavern whose roof extends further than the beam" SOFT_NL
             "of your lamp can penetrate.  To the north is a large opening but an" SOFT_NL
             "eerie mist fills the opening and you cannot see what lies beyond." SOFT_NL
             "A passage also leads south-west.",
             "You're in the large cavern.", 0);
    make_ins(SW, R_193);
    make_cond_ins(N, 100, R_195);
    make_loc(q, R_195,
             "You are in the middle of a swirling mist that eddys to and fro around" SOFT_NL
             "you. The sound of thundering water can be heard afar off and" SOFT_NL
             "occasionally quick footsteps closer, but it is difficult to tell the" SOFT_NL
             "direction of the sound.",
             "You're in the swirling mist.", 0);
    make_cond_ins(SE, 20, R_194); ditto(S); ditto(SW);
    make_cond_ins(N, 20, R_155); ditto(NE); ditto(NW);
    make_ins(E, R_195); ditto(W); ditto(N); ditto(S); ditto(NE); ditto(SE); ditto(SW); ditto(NW);
    make_loc(q, R_197,
             "You are at the halfway stage on the spiral staircase.  The beam from" SOFT_NL
             "your lamp is reflected by the minerals contained in the walls and you" SOFT_NL
             "can see great seams of gold-bearing rock.  A strong draft from the" SOFT_NL
             "top of the staircase moans and whistles through the rusted tracery" SOFT_NL
             "of the spiral staircase.",
             "You're at the halfway stage on the spiral staircase.", 0);
    make_ins(U, R_SOUTH);
    make_cond_ins(D, 100, R_198);
    make_loc(q, R_198,
             "You are at the bottom of the spiral staircase in a small room carved" SOFT_NL
             "out of the rock.  A strong wind blows down the shaft and through" SOFT_NL
             "a large portcullis forming one wall of the room. The portcullis is" SOFT_NL
             "rusted solidly in position and clearly no longer moves. A round" SOFT_NL
             "cellar with a circular drain can be seen through the portcullis.",
             "You're at the cellar view.", 0);
    make_ins(U, R_197);

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
             "You're at the NE end.", 0);
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
             "You're at the SW end.", 0);
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
    make_loc(q, R_167,
             "You clamber up the sides of the well and upon reaching the top" SOFT_NL
             "find a stone slab blocking your ascent. You give the slab a push and" SOFT_NL
             "it opens. You clamber through into daylight and look round just in" SOFT_NL
             "time to see the slab fall perfectly into the floor, so perfectly you" SOFT_NL
             "cannot see the join.",
             NULL, 0);
    make_ins(0, R_HOUSE);
    make_loc(q, R_196,
             "There is solid rock behind the curtain.",
             NULL, 0);
    make_ins(0, R_SOFT);
    make_loc(q, R_199, "You are at the bottom of the streambed and all your bones are broken.", NULL, 0);
    make_ins(0, R_LIMBO);

    /* The remaining "locations" R_PPASS, R_PDROP, R_TROLL, R_TDROP, and R_GRAB are special. */
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
        case R_167:
        case R_196:
        case R_199:
            return true;
        default:
            return false;
    }
}

bool has_light(Location loc)
{
    switch (loc) {
        case R_ROAD: case R_HILL: case R_HOUSE: case R_VALLEY:
        case R_FOREST: case R_FOREST2: case R_SLIT: case R_OUTSIDE:
        case R_INSIDE: case R_COBBLES:
        case R_PLOVER: case R_VIEW:
        case R_NEEND: case R_SWEND:
        case R_142: case R_143:
        case R_155: case R_156:
        case R_157: case R_158:
        case R_159: case R_160:
        case R_161: case R_162: case R_163:
        case R_180:
            return true;
        default:
            return false;
    }
}

bool has_water(Location loc)
{
    switch (loc) {
        case R_ROAD: case R_HOUSE: case R_VALLEY: case R_SLIT:
        case R_WET: case R_FALLS: case R_RES:
        case R_PANTRY: case R_176: case R_178: case R_179:
        case R_186: case R_187:
            return true;
        default:
            return false;
    }
}

bool has_oil(Location loc)
{
    return (loc == R_EPIT);
}

bool has_sewage(Location loc)
{
    switch (loc) {
        case R_176: case R_178: case R_179:
        case R_181: case R_182: case R_183:
        case R_184: case R_185: case R_186:
            return true;
        default:
            return false;
    }
}

bool dwarves_wont_follow_into(Location loc)
{
    switch (loc) {
        case R_176: case R_177: case R_178:
        case R_179: case R_180: case R_181:
        case R_182: case R_183: case R_184:
        case R_185: case R_186: case R_187:
        case R_188:
            return true;
        default:
            return false;
    }
}

/*========== Data structures for objects. =================================
 * This section corresponds to sections 63--70 in Knuth.
 */

struct ObjectData {
    ObjectWord link;  /* next object at this location, or NOTHING */
    ObjectWord base;  /* NOTHING for mobile objects */
    int prop;
    Location original_place;
    Location place;
    const char *name;
    const char *desc[4];  /* .prop ranges from 0 to 3 */
} objs_[MAX_OBJ+1 - MIN_OBJ];
#define objs(t) objs_[(t)-MIN_OBJ]

int holding_count;  /* how many objects have objs(t).place < 0? */
Location last_knife_loc = R_LIMBO;
int tally = 15;  /* treasures awaiting you */
int lost_treasures;  /* treasures that you won't find */
int thirst = -200;  /* when it reaches 900, you're dead */

#define toting(t) (objs(t).place < 0)
#define is_immobile(t) (objs(t).base != NOTHING)
#define there(t, loc) (objs(t).place == (loc))

/* Return true if t is a treasure. Notice that RUG_ (the other half
 * of the schizoid rug) does not need to be a treasure. You can
 * never be carrying it; the pirate can't steal it; and its prop
 * value is irrelevant (we use the prop value of the base object RUG).
 * Also, FAKE_ORB *must* not be a treasure, or else the pirate might
 * steal it.
 */
bool is_treasure(ObjectWord t)
{
    switch (t) {
        case GOLD: case DIAMONDS: case SILVER: case JEWELS:
        case COINS: case CHEST: case EGGS: case TRIDENT:
        case VASE: case EMERALD: case PYRAMID: case PEARL:
        case RUG: case SPICES: case CHAIN:
        case CROWN: case TUSK: case CHALICE: case RUBY: case ORB:
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
    assert(objs(t).link == NOTHING);
    if (toting(t)) --holding_count;
    objs(t).place = l;
    if (l == R_INHAND) {
        ++holding_count;
    } else if (l != R_LIMBO) {
        objs(t).link = places[l].objects;
        places[l].objects = t;
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
            ObjectWord *p = &places[l].objects;
            while (*p != t) p = &objs(*p).link;
            *p = objs(*p).link;
        }
        objs(t).place = R_INHAND;
        objs(t).link = NOTHING;
        ++holding_count;
    }
}

bool is_at_loc(ObjectWord t, Location loc)
{
    if (objs(t).base == NOTHING)
        return there(t, loc);
    /* Check the "alternative" objects based on this one. */
    for (ObjectWord tt = t; objs(tt).base == t; ++tt) {
        if (there(tt, loc))
            return true;
    }
    return false;
}

void mobilize(ObjectWord t) { objs(t).base = NOTHING; }
void immobilize(ObjectWord t) { objs(t).base = t; }

void new_obj(ObjectWord t, const char *n, ObjectWord b, Location l)
{
    objs(t).name = n;
    objs(t).prop = (is_treasure(t) ? -1 : 0);
    objs(t).base = b;
    objs(t).original_place = l;
    objs(t).place = l;
    objs(t).link = NOTHING;
    if (l > R_LIMBO) {
       /* Drop the object at the *end* of its list. Combined with the
         * ordering of the item numbers, this ensures that the CHASM
         * is described before the TROLL, the DRAGON before the RUG,
         * and so on. */
        ObjectWord *p = &places[l].objects;
        while (*p != NOTHING)
            p = &objs(*p).link;
        *p = t;
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
        "\"Congratulations on bringing light into the dark room!\"";
    new_obj(CLAM, "Giant clam >GRUNT!<", 0, R_SHELL);
    objs(CLAM).desc[0] = "There is an enormous clam here with its shell tightly closed.";
    new_obj(OYSTER, "Giant oyster >GROAN!<", 0, R_LIMBO);
    objs(OYSTER).desc[0] = "There is an enormous oyster here with its shell tightly closed.";
    new_obj(MAG, "\"Spelunker Today\"", 0, R_ANTE);
    objs(MAG).desc[0] = "There are a few recent issues of \"Spelunker Today\" magazine here.";
    new_obj(DWARF, 0, DWARF, R_LIMBO);
    new_obj(KNIFE, 0, 0, R_LIMBO);
    new_obj(FOOD, "Tasty food", 0, R_PANTRY);
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
    new_obj(OWL, 0, OWL, R_169);
    objs(OWL).desc[0] = NULL;  /* it's just scenery */
    new_obj(WEB, 0, WEB, R_148);
    objs(WEB).desc[0] = "A large untidy spider's web covers one corner of the cave.";
    new_obj(SPIDER, 0, SPIDER, R_148);
    objs(SPIDER).desc[0] = "A huge black hairy spider crouches in one corner of the web.";
    new_obj(DOCUMENTS, "Legal documents", 0, R_LIMBO);
    objs(DOCUMENTS).desc[0] = "Some legal documents are stuck to the web where the spider was.";
    objs(DOCUMENTS).desc[1] = "There are some legal documents here.";
    /* Pike uses a global variable INSC1 to say whether we've read the spoon's inscription.
     * I use objs(SPOON).prop instead, since it's not used for anything else. */
    new_obj(SPOON, "Tarnished spoon", 0, R_180);
    const char* spoon_description = "A tarnished spoon lies here.";
    objs(SPOON).desc[0] = spoon_description;
    objs(SPOON).desc[1] = spoon_description;
    new_obj(HORN, "Little horn", 0, R_188);
    objs(HORN).desc[0] = "There is a little horn here.";
    new_obj(RATS, 0, RATS, R_182);
    /* objs(RATS).prop ranges from 0 to 6; rat-related messages are given elsewhere */
    new_obj(GIANT, 0, GIANT, R_143);
    objs(GIANT).desc[0] = NULL;  /* it's just scenery */
    objs(GIANT).desc[1] =
        "Suddenly a giant hand descends from above, picks you up" SOFT_NL
        "and shakes you scattering your possessions across the floor.";
    objs(GIANT).desc[2] = "The giant reclines on the couch eating eggs!";
    objs(GIANT).desc[3] = "A giant hand descends from above and picks you up.";
    new_obj(FLAGSTONE, 0, FLAGSTONE, R_145);
    objs(FLAGSTONE).desc[0] = NULL;  /* it's just scenery */
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
    new_obj(CROWN, "Sparkling crown", 0, R_172);
    objs(CROWN).desc[0] = "There is a sparkling crown here!";
    new_obj(TUSK, "Ivory tusk", 0, R_177);
    objs(TUSK).desc[0] = "A large ivory tusk lies here!";
    new_obj(CHALICE, "Inlaid chalice", 0, R_144);
    objs(CHALICE).desc[0] = "A magnificent inlaid chalice lies here!";
    objs(CHALICE).desc[1] = "A unicorn stands here pawing the ground.";
    new_obj(RUBY, "Large ruby", 0, R_188);
    objs(RUBY).desc[0] = "A very large ruby lies here.";
    new_obj(ORB, "Crystal orb", 0, R_CELLAR);
    objs(ORB).desc[0] = "A crystal orb lies here!";
    new_obj(FAKE_ORB, 0, FAKE_ORB, R_198);
    objs(FAKE_ORB).desc[0] =
        "A glistening globe lies in a small depression on the sloping cellar" SOFT_NL
        "floor!";
    objs(FAKE_ORB).desc[1] = NULL;  /* once the real orb has been taken, this one disappears */

    /* The food's natural place is the pantry, of course; but we still
     * start it out in the building. If the food is eaten or otherwise
     * winds up in R_LIMBO, the dwarves will helpfully tote it back to
     * R_PANTRY given enough time. */
    move(FOOD, R_HOUSE);
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

struct Dwarf {
    bool seen;
    ObjectWord toted;  /* Pike's DOBJ array */
    Location oldloc;
    Location loc;
} dwarves[6] = {
    { false, NOTHING, R_LIMBO, R_PIRATES_NEST },  /* this one is really the pirate */
    { false, NOTHING, R_LIMBO, R_HMK },
    { false, NOTHING, R_LIMBO, R_WFISS },
    { false, NOTHING, R_LIMBO, R_RES },  /* in Woods this dwarf starts at R_Y2 */
    { false, NOTHING, R_LIMBO, R_LIKE3 },
    { false, NOTHING, R_LIMBO, R_COMPLEX },
};
struct Dwarf *pirate = &dwarves[0];

bool dwarf_at(Location loc)  /* is a dwarf present? Section 160 in Knuth. */
{
    if (dflag < 2) return false;
    for (int j=1; j <= 5; ++j) {
        if (loc == dwarves[j].loc) return true;
    }
    return false;
}

void return_pirate_to_lair(bool with_chest)
{
    if (with_chest) {
        drop(CHEST, R_PIRATES_NEST);
        drop(MESSAGE, R_PONY);
    }
    pirate->loc = pirate->oldloc = R_PIRATES_NEST;
    pirate->seen = false;
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
        if (here(t, loc) && !is_immobile(t)) {
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
             "He is carrying a copy of Jane's Fighting Ships dated 1693 and a" SOFT_NL
             "large chest. \"Shiver me timbers!\" he cries, \"I've been spotted!" SOFT_NL
             "I'd best hie meself off to the maze and hide me chest!\"  And with" SOFT_NL
             "that he vanishes into the gloom.");
        return_pirate_to_lair(true);
        return;
    }
    if (pirate->oldloc != pirate->loc && pct(20)) {
        puts("There are faint rustling noises from the darkness behind you.");
    }
}

bool forbidden_to_pirate(Location loc)
{
    if (R_DEAD3 <= loc && loc <= R_BARR) {
        return true;  /* as in Woods: dead ends, far side of chasm */
    } else if (R_182 <= loc && loc <= R_187) {
        return true;  /* also forbid the sewer maze, so as not to steal the tusk */
    } else {
        return false;
    }
}

/* Certain objects will never be carried by the dwarves. */
bool dwarves_wont_tote(ObjectWord obj)
{
    switch (obj) {
        case BOTTLE: case CAGE: case BIRD: case PILLOW: case BEAR:
        case WATER: case OIL:
            /* these have special logic for TAKE/DROP */
        case CLAM: case OYSTER:
            /* these shouldn't leave the Shell Room area */
        case KNIFE:
            /* this one isn't a real item */
        case AXE:
            /* don't be evil */
        case ROD: case ROD2:
            /* not sure why we need to disallow these. TODO investigate */
        /* Pike also lists the STEPS, DOOR, SNAKE, FISSURE, TABLET, TROLL,
         * and OWL, but these are already 100% immobile, so we don't need
         * to repeat them here. */
            return true;
        default:
            return false;
    }
}

void dwarves_tote_objects(Location loc)
{
    /* Only three of the dwarves tote things? Check this! TODO FIXME BUG HACK */
    for (int i=0; i < 3; ++i)
    {
        struct Dwarf *d = &dwarves[i];
        if (d->loc == R_LIMBO) {
            if (d->toted != NOTHING) {
                /* Dead dwarf drops items in front of player.
                 * TODO: is this codepath dead?
                 * Certainly objs(d->toted).place should always be R_188! */
                if (objs(d->toted).place == R_188) {
                    move(d->toted, loc);
                }
                describe_object(d->toted, loc);
                d->toted = NOTHING;
            }
            continue;
        }
        if (d->toted != NOTHING && objs(d->toted).original_place == d->loc)
        {
            /* Drop the object in its rightful place. */
            move(d->toted, d->loc);
            d->toted = NOTHING;
        }

        /* Okay, the dwarf is not dead (he's somewhere). Let's see what's
         * available here for him to tote around. */
        ObjectWord candidate = d->toted;  /* Pike's "KN" */
        for (ObjectWord t = places[d->loc].objects; t != NOTHING; t = objs(t).link) {
            if (is_immobile(t)) continue;
            if (d->loc == objs(t).original_place) continue;
            if (dwarves_wont_tote(t)) continue;
            /* Maybe the dwarf prefers object "t" (Pike's "KH") to whatever
             * he's currently toting. */
            if (t > candidate) candidate = t;
        }

        if (candidate != d->toted) {
            if (d->toted != NOTHING) {
                /* Drop the originally carried object here. */
                move(d->toted, d->loc);
            }
            /* Pick up the new object. */
            d->toted = candidate;
            move(candidate, R_188);
        }
    }
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
        if (loc >= MIN_LOWER_LOC && pct(5) && !now_in_darkness(loc)) {
            /* When level 2 of the cave is reached, we silently kill 0, 1,
             * or 2 of the dwarves. Then if any of the survivors is in
             * the current location, we move him to R_NUGGET; thus no
             * dwarf is presently tracking you. Another dwarf does,
             * however, toss an axe and grumpily leave the scene. */
            dflag = 2;
            if (pct(50)) dwarves[1+ran(5)].loc = R_LIMBO;
            if (pct(50)) dwarves[1+ran(5)].loc = R_LIMBO;
            for (int j=1; j <= 5; ++j) {
                if (dwarves[j].loc == loc) dwarves[j].loc = R_NUGGET;
                dwarves[j].oldloc = dwarves[j].loc;
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
        dwarves_tote_objects(loc);

        int dtotal = 0;  /* this many dwarves are in the room with you */
        int attack = 0;  /* this many have had time to draw their knives */
        int stick = 0;  /* this many have hurled their knives accurately */
        bool dwarf_is_toting_something = false;
        for (int j=0; j <= 5; ++j) {
            struct Dwarf *d = &dwarves[j];
            if (d->loc == R_LIMBO) {
                /* Pike allows dead dwarves to resurrect themselves! */
                if (ran(400) == 0) {
                    d->loc = R_163;
                }
            } else {
                Location ploc[19];  /* potential locations for the next random step */
                int i = 0;
                /* Make a table of all potential exits.
                 * Dwarves think R_SCAN1, R_SCAN2, R_SCAN3 are three different locations,
                 * although you will never have that perception. */
                for (Instruction *q = start[d->loc]; q < start[d->loc + 1]; ++q) {
                    Location newloc = q->dest;
                    if (i != 0 && newloc == ploc[i-1]) continue;
                    if (newloc < MIN_LOWER_LOC) continue;  /* don't wander above level 2 */
                    if (newloc == d->oldloc || newloc == d->loc) continue;  /* don't double back */
                    if (q->cond == 100) continue;
                    if (d == pirate && forbidden_to_pirate(newloc)) continue;
                    if (is_forced(newloc) || newloc > MAX_LOC) continue;
                    if (there(OWL, newloc)) continue;  /* dwarves avoid the owl */
                    ploc[i++] = newloc;
                }
                if (i==0) ploc[i++] = d->oldloc;
                d->oldloc = d->loc;
                d->loc = ploc[ran(i)];  /* this is the random walk */

                /* Dwarves follow the player once they've spotted him. But
                 * they won't follow outside the lower cave. */
                if (d->loc == loc || d->oldloc == loc) {
                    d->seen = true;
                } else if (loc < MIN_LOWER_LOC || dwarves_wont_follow_into(loc)) {
                    d->seen = false;
                }

                if (d->seen) {
                    /* Make dwarf d follow */
                    d->loc = loc;
                    if (d == pirate) {
                        pirate_tracks_you(loc);
                    } else {
                        if (d->toted != NOTHING) {
                            dwarf_is_toting_something = true;
                        }
                        ++dtotal;
                        if (d->oldloc == d->loc) {
                            ++attack;
                            last_knife_loc = loc;
                            if (ran(10) < (dflag-2)) ++stick;
                        }
                    }
                }
            }
        }
        if (dtotal != 0 && !now_in_darkness(loc)) {
            /* Make the threatening dwarves attack.
             * Unlike Woods', Pike's dwarves don't attack in the dark. */
            if (dtotal == 1) {
                puts("There is a threatening little dwarf in the room with you!");
                if (dwarf_is_toting_something) {
                    puts("He scuttles along clutching his bulging coat front and muttering.");
                }
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
            dwarves[j].seen = false;
            dwarves[j].loc = R_LIMBO;
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
bool wizard_mode = false;

void give_optional_plugh_hint(Location loc)
{
    if (loc == R_Y2 && pct(25) && !cave_is_closing()) {
        puts("A hollow voice says \"PLUGH\".");
    }
}

void spot_treasure(ObjectWord t)
{
    if (objs(t).prop >= 0) return;
    assert(is_treasure(t) && !closed);  /* You've spotted a treasure */
    switch (t) {
        case RUG:  /* trapped */
        case CHAIN:  /* locked */
            objs(t).prop = 1;
            break;
        default:
            objs(t).prop = 0;
            break;
    }
    tally--;
    if (tally == lost_treasures && tally > 0 && lamp_limit > 35) {
        /* Zap the lamp if the remaining treasures are too elusive */
        lamp_limit = 35;
    }
}

void describe_object(ObjectWord t, Location loc)
{
    if (t == TREADS && toting(GOLD)) {
        /* The rough stone steps disappear if we are carrying the nugget. */
        return;
    }
    int going_up = (t == TREADS && loc == R_EMIST);
    const char *obj_description = objs(t).desc[objs(t).prop + going_up];
    if (obj_description != NULL) {
        puts(obj_description);
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
        for (ObjectWord t = places[loc].objects; t != NOTHING; t = objs(t).link) {
            ObjectWord tt = objs(t).base ? objs(t).base : t;
            if (closed && (objs(tt).prop < 0)) continue;  /* scenery objects */
            spot_treasure(tt);
            describe_object(tt, loc);
        }
    }
    return 0;  /* just continue normally */
}

Location announce_tides(Location loc)
{
    /* The tides start flowing as soon as you enter R_178 for the first time. */
    static bool tides_have_started = false;
    if (!tides_have_started) {
        if (loc == R_178) {
            tides_have_started = true;
        } else {
            return loc;
        }
    }

    static int tide = 3;
    ++tide;
    if (tide == 15) tide = -15;

    switch (loc) {
        case R_178:
            if (abs(tide) >= 5) {
                /* A few turns after the culvert becomes passable, the description of
                 * the shaft with sewage below will change to describe the culvert
                 * below. This is implemented in a clever way: R_176 is always adjacent
                 * to R_178 in the travel table, but if the tide is low, then we simply
                 * teleport the player to R_179 before describing where he is. The
                 * effect is that DOWN from R_176 appears to lead to either R_178 or
                 * R_179 depending on the tides, but dwarves always see it as leading
                 * to R_178. Thus, dwarves can never wander into R_179. */
                return R_179;
            }
            break;
        case R_180: case R_181: case R_182:
            if (loc == R_182) {
                /* The rats get hungrier the longer you stay in their location. */
                objs(RATS).prop = (objs(RATS).prop + 1) % 7;
            }
            switch (abs(tide)) {
                case 0: case 1: case 2: case 3: case 4:
                    puts("You have drowned horribly in a mixture of sea-water and sewage!");
                    return R_LIMBO;
                case 5: puts("You are nearly up to your neck in sewage.  Help!"); return loc;
                case 6: puts("The sewage comes nearly up to your chest! Lets get out of here!"); return loc;
                case 7: puts("You are up to your waist in sewage. Lets leave!"); return loc;
                case 8: puts("The sewage comes nearly up to your waist. This is not at all" SOFT_NL
                             "pleasant!"); return loc;
                case 9: puts("Sewage laps about your legs."); return loc;
                case 10: puts("The sewage comes above your knees."); return loc;
                default: puts("Sewage laps about your knees."); return loc;
            }
            break;
        case R_183: case R_184: case R_185: case R_186:
            if (tide == 0) {
                puts("The pipe has completely filled with sewage.........");
                return R_LIMBO;
            }
            break;
        default:
            break;
    }
    return loc;
}

bool maybe_die_of_thirst(Location loc)
{
    int previous_thirst = thirst;
    thirst += (holding + 2);
    if (loc == R_VIEW) {
        /* Visiting the volcano view pushes your thirst counter rapidly
         * toward 880, for some reason. TODO understand this. */
        thirst = (880 + thirst) / 2;
    }
    if (thirst >= 900) {
        puts("You have collapsed and died of thirst!");
        return true;
    } else if (thirst >= 880) {
        puts("I cannot go much farther without a drink.");
    } else if (previous_thirst < 800 && thirst > 800) {
        puts("Exploring is thirsty work, you must soon take a drink or lighten" SOFT_NL
             "your load.");
    }
    return false;
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
    { 0, false, 0, 5, "Welcome to Adventure II!!  Would you like instructions?",
    "Somewhere nearby is Colossal Cave, where others have found fortunes in" SOFT_NL
    "treasure and gold, though it is rumored that some who enter are never" SOFT_NL
    "seen again.  Magic is said to work in the cave.  I will be your eyes" SOFT_NL
    "and hands.  Direct me with commands of 1 or 2 words.  I should warn" SOFT_NL
    "you that I look at only the first five letters of each word, so you'll" SOFT_NL
    "have to enter \"NORTHEAST\" as \"NE\" to distinguish it from \"NORTH\"." SOFT_NL
    "(Should you get stuck, type \"HELP\" for some general hints.  For infor" SOFT_HYPHEN
    "mation on how to end your adventure, etc., type \"INFO\".)\n"
    "                             -  -  -\n"
    "This adventure (Adventure II) comes to you courtesy of Jack Pike &" SOFT_NL
    "Peter Luckett (R.A.E Bedford/Farnborough).\n"
    "The program was originally developed by Willie Crowther.  Most of the" SOFT_NL
    "features of the earlier program were added by Don Woods (Don @ SU-AI).\n"
    "Queries please to JP or PL.\n"
    "This particular program was translated from Fortran to ANSI C by Arthur O'Dwyer." },
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
                    if (places[loc].objects == NOTHING &&
                            places[oldloc].objects == NOTHING &&
                            places[oldoldloc].objects == NOTHING &&
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
#define MAX_DEATHS 4
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
                    s += 12;
                } else if (i == CHEST) {
                    s += 14;
                } else {
                    s += 16;
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

/* Interestingly, Pike's scoring system tops out at 430 points;
 * you can lose 10 points (relative to the highest possible score)
 * and still be declared an "Adventurer Grandmaster".
 */
#define HIGHEST_CLASS 8
static int class_score[] = { 50, 100, 150, 200, 250, 300, 360, 430, 9999 };
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
    "Hmmm, I don't know how much orange smoke I have left.  Now let me" SOFT_NL
      "see.....(rummage rummage).....ah, here we are!  Not much for a *real*" SOFT_NL
      "reincarnation though, do you want me to try nevertheless?",
    "", /* no message, just reincarnate the dope */
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
    if (has_light(loc)) return false;
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

Location attempt_tusk_passage(Location from)
{
    /* TODO write this */
    assert(false);
    return R_LIMBO;
}

Location attempt_giant_quarters(Location from)
{
    /* TODO write this */
    assert(false);
    return R_LIMBO;
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
    } else if (is_immobile(obj)) {
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

Location get_kicked_out_of_cellar()
{
    puts("Through the portcullis steps a tall wizard clothed in grey. \"Be Off!\"" SOFT_NL
         "he commands, raising an arm. As the scene fades his distant voice" SOFT_NL
         "continues \"And take your belongings with you!\".");

    Location loc;
    switch (ran(6))
    {
        case 0: loc = R_WMIST; break;
        case 1: loc = R_LIKE1; break;
        case 2: loc = R_LIKE2; break;
        case 3: loc = R_ABOVER; break;
        case 4: loc = R_ABOVEP; break;
        case 5: loc = R_SJUNC; break;
    }

    /* Once you've picked up the orb, the "fake" orb visible from the
     * cellar view must disappear. It will never reappear, since
     * nothing can ever be successfully dropped in the cellar.
     * Pike handles this by making the fake orb invisible via prop
     * manipulation; a better solution would be to just destroy it.
     * The difference is observable if you go back to the cellar view
     * and type "GET" or "GET GLOBE". */
    if (!there(ORB, R_CELLAR)) {
        objs(FAKE_ORB).prop = 1;
    }

    /* Pike's wizard not only moves you out of the cellar, but also
     * attempts (line 9023) to move every non-orb object at R_198 to
     * the dwarves' quarters. However, due to a bug in that loop
     * (namely, following LINK(I) after object I has already been moved),
     * if you drop anything at the R_198 before annoying the wizard,
     * Pike's loop never terminates and the game crashes. Even without
     * that bug, I don't understand what purpose the loop was originally
     * meant to accomplish. I've omitted it here. */

    return loc;
}

Location attempt_drop(ObjectWord obj, Location loc)
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
    } else if (obj == BIRD && here(SPIDER, loc)) {
        puts("The bird attacks the spider and in an astounding flurry gets" SOFT_NL
             "very messily eaten. It was a horrible sight, a flurry of" SOFT_NL
             "feathers, hairy legs and blood.");
        destroy(BIRD);
        objs(BIRD).prop = 0;  /* no longer in the cage */
        /* Now that the bird is dead, you can never get past the snake
         * into the south side chamber, so the precious jewelry is lost. */
        if (there(SNAKE, R_HMK)) ++lost_treasures;
    } else {
        /* The usual case for dropping any old object. */

        if (has_sewage(loc)) {
            if (obj == FOOD && loc == R_182) {
                puts("The food disappears into the sewage and the water thrashes" SOFT_NL
                     "with activity.  My, those rats *are* hungry!");
                objs(RATS).prop = 0;
            } else if (loc == R_176 || loc == R_178 || loc == R_179) {
                puts("It resides momentarily on the slippery surface, then slides down" SOFT_NL
                     "the shaft and disappears.");
            } else {
                puts("It has disappeared in the sewage!");
            }
            drop(obj, (obj == FOOD) ? R_163 : R_180);
        } else {
            puts(ok);
            if (obj == TUSK && loc == R_HOUSE) {
                puts("Tusk....Tusk....Tusk...      Thats not on the list!" SOFT_NL
                     "Its ivory isn't it? must be valuable....." SOFT_NL
                     "OK we'll give you the benefit of the doubt.");
            } else if (obj == SPOON && objs(SPOON).prop == 0) {
                puts("Oh! There appears to be an inscription on the spoon.");
            }
        }

        if (loc == R_CELLAR) {
            loc = get_kicked_out_of_cellar();
        }

        if (obj == BIRD) objs(BIRD).prop = 0;  /* no longer caged */
        if (obj == CAGE && objs(BIRD).prop) drop(BIRD, loc);

        /* Special cases for dropping a liquid. */
        if (obj == WATER && objs(BOTTLE).prop == 0) obj = BOTTLE;
        if (obj == OIL && objs(BOTTLE).prop == 2) obj = BOTTLE;
        if (obj == BOTTLE && bottle_contents() != NOTHING)
            move(bottle_contents(), R_LIMBO);

        drop(obj, loc);

        /* Items dropped in the swirling mist end up in the dwarves' quarters.
         * However, dropping the cage in the mist leaves the caged bird
         * still in the mist even as the cage itself goes to the dwarves.
         * TODO: fix this original bug? */
        if (loc == R_195) {
            move(obj, R_163);
        } else if (loc == R_148) {
            if (here(SPIDER, R_148)) juggle(SPIDER);
            juggle(WEB);
        }
    }

    return loc;
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
        } else if (obj == WATER && has_water(loc)) {
            its_right_here = true;
        } else if (obj == OIL && has_oil(loc)) {
            its_right_here = true;
        } else if (obj == DWARF && dwarf_at(loc)) {
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

void kill_a_dwarf(Location loc)
{
    static bool first_time = true;
    if (first_time) {
        puts("You killed a little dwarf.  The body vanishes in a cloud of greasy" SOFT_NL
             "black smoke.");
        first_time = false;
    } else {
        puts("You killed a little dwarf.");
    }

    int j;
    for (j=1; j <= 5; ++j) {
        if (dwarves[j].loc == loc) break;
    }
    assert(j <= 5);
    dwarves[j].loc = R_LIMBO;  /* Once killed, a dwarf never comes back. */
    dwarves[j].seen = false;
}

void throw_axe_at_dwarf(Location loc)  /* section 163 in Knuth */
{
    if (ran(3) < 2) {
        kill_a_dwarf(loc);
    } else {
        puts("You attack a little dwarf, but he dodges out of the way.");
    }
}

/* Return true if we don't know what to fill. */
bool attempt_fill(ObjectWord obj, Location loc)  /* sections 110--111 in Knuth */
{
    if (obj == VASE) {
        if (loc != R_EPIT && !has_water(loc)) {
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
    } else if (has_oil(loc)) {
        puts("Your bottle is now full of oil.");
        objs(BOTTLE).prop = 2;
        if (toting(BOTTLE)) objs(OIL).place = R_INHAND;
    } else if (has_water(loc)) {
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

void attempt_ride(ObjectWord obj)
{
    if (obj == CHALICE && objs(CHALICE).prop == 1)
    {
        puts("With you aboard, the unicorn gallops off at a furious pace. The" SOFT_NL
             "tunnel walls flash past dimly illuminated by an ethereal glow." SOFT_NL
             "Suddenly tunnel walls, unicorn and everything disappear and you" SOFT_NL
             "fly on in pitch blackness. Far below you a door opens revealing a" SOFT_NL
             "dull red glow and grotesque creatures pour forth and dance and sway" SOFT_NL
             "as you fall towards the aperture. Your descent is impeded, slowly at" SOFT_NL
             "first and then more firmly by a web of gossamer threads, on which" SOFT_NL
             "you can clearly see stamped at intervals  \"Made in Fairyland.\"" SOFT_NL
             "Below you the creatures leap but try as they might they cannot reach" SOFT_NL
             "you until there appears our horned fiend.  \"You chose the losing" SOFT_NL
             "side my friend!\" he says and slashes through the supporting threads." SOFT_NL
             "As you fall you hear his evil laugh and him shout \"Welcome to Hell!!\"\n\n"
             "Now you really are in a mess, lost one of our best agents too." SOFT_NL
             "People like you didn't ought to be allowed in the cave, poking your" SOFT_NL
             "nose into things you can't handle. You'll always lose if you tangle" SOFT_NL
             "with *him* and there is no way of helping the princess that I know of." SOFT_NL
             "I don't think I can stand any more of your bungling I'm going........");
        quit();
    }
    else
    {
        puts("Don't be ridiculous!");
    }
}

/* This is some pretty baroque logic for an obscure case.
 * We're deciding what the noun should be if the player types
 * "READ" as a one-word command. If we return NOTHING, the
 * main loop will branch to get_object; otherwise it will
 * be handled as if we'd typed "READ <obj>". */
ObjectWord read_what(Location loc)
{
    if (now_in_darkness(loc))
        return NOTHING;  /* can't read in the dark */
    if (closed && toting(OYSTER))
        return OYSTER;
    if (here(DOCUMENTS, loc))
        return DOCUMENTS;

    bool magazines_here = here(MAG, loc);
    if (here(TABLET, loc))
        return magazines_here ? NOTHING : TABLET;
    if (here(MESSAGE, loc))
        return magazines_here ? NOTHING : MESSAGE;
    return magazines_here ? MAG : NOTHING;
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
        case DOCUMENTS:
            puts("The documents seem to be a set of deeds with some associated plans," SOFT_NL
                 "but the language is odd and the plans are incomprehensible.");
            break;
        case SPOON:
            objs(SPOON).prop = 1;  /* we've seen the inscription */
            puts("It says EPNS in flowery script.");
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
            if (dflag >= 2 && dwarf_at(loc)) return 0;
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
            if (has_water(loc)) return 0;
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
    } else if (*newloc == R_TDROP) {
        *newloc = attempt_tusk_passage(loc);
    } else if (*newloc == R_GRAB) {
        *newloc = attempt_giant_quarters(loc);
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
        case PUSH:
            puts("Nothing happens.");
            break;
        case EPNS:
            puts("Electro-plated nickel silver.  Its nearly worthless.");
            break;
        case BONES:
            puts("Bones UGH!  I've got a thing about bones, I will have nothing" SOFT_NL
                 "to do with them.");
            break;
        case DEBRIS:
            puts("I've seen none that would easily come free. There would hardly" SOFT_NL
                 "seem much future in it anyway.");
            break;
        case DUNG:
            puts("This is getting beyond the bounds of decency!");
            break;
        case GRILL:
            puts("If you are trying to get through a solid metal grill, you're wasting" SOFT_NL
                 "your time.");
            break;
        case CURTAIN:
            puts("Curtains improve the comfort without blocking the passage.");
            break;
        case PORTCULLIS:
            puts("If you're trying to open the rusty portcullis, you're wasting your" SOFT_NL
                 "time.");
            break;
        case STONE:
            puts("The rock around here is very hard and although large boulders are" SOFT_NL
                 "numerous it would appear that someone or something has meticulously" SOFT_NL
                 "removed any small stones or pebbles which could be carried.");
            break;
    }
}

void maybe_become_a_wizard(void)
{
    /* If on the first turn of the game the player types something
     * like "MAGIC MOE", he'll gain the ability to FLY to distant
     * locations. This is analogous to Platt's LPSD command. */
    if (turns == 0 && word1[0] == 'm' && word2[0] == 'm')
        wizard_mode = true;
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
        if (cave_is_closing() && (newloc < MIN_IN_CAVE || newloc == R_167) && newloc != R_LIMBO) {
            panic_at_closing_time();
            newloc = loc;
        } else if (newloc != loc && !forbidden_to_pirate(loc)) {
            /* Stay in loc if a dwarf is blocking the way to newloc */
            for (int j=1; j <= 5; ++j) {
                if (dwarves[j].seen && dwarves[j].oldloc == newloc) {
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
        loc = announce_tides(loc);
        if (loc == R_LIMBO) goto death;
        if (maybe_die_of_thirst(loc)) goto death;
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
            maybe_become_a_wizard();
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
                    if (has_water(loc)) {
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
                    ObjectWord object_here = places[loc].objects;
                    if (dwarf_at(loc))
                        goto get_object;
                    if (object_here != NOTHING && objs(object_here).link == NOTHING) {
                        obj = object_here;
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
                case FLY:
                    /* Wizards can move to a room by number. Unfortunately, this port's
                     * room numbers don't match up to the original's, so if you enter
                     * 163 to visit the dwarves' quarters, you'll actually wind up in
                     * the inclined shaft. Oops. TODO: perhaps correct our numbering? */
                    if (wizard_mode) {
                        puts("Note: This means of travel can generate internal inconsistencies," SOFT_NL
                              "so take care. Now enter new LOCATION number.");
                        verb = NOTHING;
                        obj = NOTHING;
                        listen();
                        int wizloc = atoi(word1);
                        if (R_ROAD <= wizloc && wizloc <= R_SWEND) {
                            loc = wizloc;
                            mot = NOWHERE;
                            goto try_move;
                        } else {
                            /* Let's not allow a wizard to crash the game. */
                            puts("Nothing happens!");
                        }
                    } else {
                        puts("Only wizards can do that!");
                    }
                    continue;
                case SLEEP:
                case REST:
                case SCREAM:
                    goto transitive;
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
                    bool stream_here = has_water(loc);
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
                case DROP: {
                    Location droploc = attempt_drop(obj, loc);
                    if (droploc != loc) {
                        loc = droploc;
                        mot = NOWHERE;
                        goto try_move;
                    }
                    continue;
                }
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
                    if (dwarf_at(loc)) {
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
                        if (dwarf_at(loc)) { ++k; obj = DWARF; }
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
                                verb = NOTHING;
                                obj = NOTHING;
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
                case HOOT:
                    puts("I don't understand that!");
                    continue;
                case BLOW:
                    puts("Nothing happens.");
                    continue;
                case RIDE:
                    attempt_ride(obj);
                    continue;
                case FLY:
                    puts("Nothing happens.");
                    continue;
                case SLEEP:
                    /* These three new verbs might as well have been message
                     * words, but Pike made them actions instead. Their parser
                     * category is observable by combining them with motion
                     * verbs; for example, MIST WEST is a synonym for MIST,
                     * but SLEEP WEST is a synonym for WEST. */
                    puts("It is *far* too dangerous to sleep around here!");
                    continue;
                case REST:
                    puts("Rest?  Rest?  You don't have time to rest!");
                    continue;
                case SCREAM:
                    puts("Ok..................AAAAAAAAAARRRRRRRRRGGGGGGHHHHHHHHH!!!!!!!!!!!!");
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

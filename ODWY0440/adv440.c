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
int abs(int x) { return x < 0 ? -x : x; }
#else
int ran(int range) { return rand() % range; }
#endif /* Z_MACHINE */

#define SOFT_NL "\n"
#define SOFT_HYPHEN "-\n"
#define EMDASH(x) "\xE2\x80\x94"  /* U+2014 "EM DASH" */

bool pct(int percent) { return (ran(100) < percent); }
bool streq(const char *input, const char *pattern)
{
    /* Pattern "NORTH-W" matches "NORTH", "NORTH-", and "NORTH-WIBBLE",
     * but not "NORT" or "NORTHWEST".
     * The order of vocabulary words in Pike means that "NORTH-" is
     * a synonym for "NORTH-E" and "SOUTH-" is a synonym for "SOUTH-E".
     */
    int n = strlen(input);
    if (n < 5) n = 5;
    if (n > 7) n = 7;
    return !strncmp(input, pattern, n);
}

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
    char text[8];
    int meaning;
};

#define HASH_PRIME 1009
struct HashEntry hash_table[HASH_PRIME];

void new_word(const char *w, int m)
{
    int h = 0;
    assert(strlen(w) <= 7);
    for (const char *p = w; *p != '\0' && p != w+5; ++p) {
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
    new_motion_word("upstrea", UPSTREAM);
    new_motion_word("downstr", DOWNSTREAM);
    new_motion_word("forest", FOREST);
    new_motion_word("forward", FORWARD); new_motion_word("continu", FORWARD);
    new_motion_word("onward", FORWARD);
    new_motion_word("back", BACK); new_motion_word("return", BACK);
    new_motion_word("retreat", BACK);
    new_motion_word("valley", VALLEY);
    new_motion_word("stair", STAIRS); new_motion_word("stairs", STAIRS);
    new_motion_word("stairca", STAIRS);
    new_motion_word("out", OUT); new_motion_word("outside", OUT);
    new_motion_word("exit", OUT); new_motion_word("leave", OUT);
    new_motion_word("buildin", HOUSE); new_motion_word("house", HOUSE);
    new_motion_word("gully", GULLY);
    new_motion_word("stream", STREAM);
    new_motion_word("rock", ROCK);
    new_motion_word("bed", BED);
    new_motion_word("crawl", CRAWL);
    new_motion_word("cobble", COBBLES); new_motion_word("cobbles", COBBLES);
    new_motion_word("inward", IN); new_motion_word("inwards", IN);
    new_motion_word("inside", IN); new_motion_word("in", IN);
    new_motion_word("surface", SURFACE);
    new_motion_word("null", NOWHERE); new_motion_word("nowhere", NOWHERE);
    new_motion_word("dark", DARK);
    new_motion_word("passage", PASSAGE); new_motion_word("tunnel", PASSAGE);
    new_motion_word("low", LOW);
    new_motion_word("canyon", CANYON);
    new_motion_word("awkward", AWKWARD);
    new_motion_word("pantry", PANTRY);
    new_motion_word("view", VIEW);
    new_motion_word("upward", U); new_motion_word("upwards", U);
    new_motion_word("up", U); new_motion_word("u", U);
    new_motion_word("above", U); new_motion_word("ascend", U);
    new_motion_word("d", D); new_motion_word("downwar", D);
    new_motion_word("down", D); new_motion_word("descend", D);
    new_motion_word("pit", PIT);
    new_motion_word("outdoor", OUTDOORS);
    new_motion_word("crack", CRACK);
    new_motion_word("steps", STEPS);
    new_motion_word("dome", DOME);
    new_motion_word("left", LEFT);
    new_motion_word("right", RIGHT);
    new_motion_word("hall", HALL);
    new_motion_word("jump", JUMP);
    new_motion_word("barren", BARREN);
    new_motion_word("over", OVER);
    new_motion_word("across", ACROSS);
    new_motion_word("east", E); new_motion_word("e", E);
    new_motion_word("west", W); new_motion_word("w", W);
    new_motion_word("north", N); new_motion_word("n", N);
    new_motion_word("south", S); new_motion_word("s", S);
    new_motion_word("ne", NE); new_motion_word("north-e", NE);
    new_motion_word("se", SE); new_motion_word("south-e", SE);
    new_motion_word("sw", SW); new_motion_word("south-w", SW);
    new_motion_word("nw", NW); new_motion_word("north-w", NW);
    new_motion_word("hole", HOLE);
    new_motion_word("wall", WALL);
    new_motion_word("broken", BROKEN);
    new_motion_word("y2", Y2);
    new_motion_word("climb", CLIMB);
    new_motion_word("look", LOOK); new_motion_word("examine", LOOK);
    new_motion_word("touch", LOOK); new_motion_word("describ", LOOK);
    new_motion_word("floor", FLOOR);
    new_motion_word("room", ROOM);
    new_motion_word("slit", SLIT);
    new_motion_word("slab", SLAB); new_motion_word("slabroo", SLAB);
    new_motion_word("xyzzy", XYZZY);
    new_motion_word("depress", DEPRESSION);
    new_motion_word("entranc", ENTRANCE);
    new_motion_word("plugh", PLUGH);
    new_motion_word("secret", SECRET);
    new_motion_word("cave", CAVE);
    new_motion_word("cross", CROSS);
    new_motion_word("bedquil", BEDQUILT);
    new_motion_word("plover", PLOVER);
    new_motion_word("orienta", ORIENTAL);
    new_motion_word("cavern", CAVERN);
    new_motion_word("shell", SHELL);
    new_motion_word("reservo", RESERVOIR);
    new_motion_word("main", OFFICE); new_motion_word("office", OFFICE);
    new_motion_word("fork", FORK);
    new_motion_word("reflect", REFLECT);
    new_motion_word("testo", TESTO);
    new_motion_word("tower", TOWER);
    new_motion_word("bedch", BEDCHAMBER);
    new_motion_word("galley", GALLEY);
    new_object_word("key", KEYS); new_object_word("keys", KEYS);
    new_object_word("lamp", LAMP); new_object_word("lantern", LAMP);
    new_object_word("headlam", LAMP);
    new_object_word("grate", GRATE);
    new_object_word("cage", CAGE);
    new_object_word("rod", ROD);
    new_object_word("bird", BIRD);
    new_object_word("door", RUSTY_DOOR);
    new_object_word("pillow", PILLOW); new_object_word("velvet", PILLOW);
    new_object_word("snake", SNAKE);
    new_object_word("fissure", FISSURE);
    new_object_word("tablet", TABLET);
    new_object_word("clam", CLAM);
    new_object_word("oyster", OYSTER);
    new_object_word("magazin", MAG); new_object_word("issue", MAG);
    new_object_word("spelunk", MAG); new_object_word("\"spelun", MAG);
    new_object_word("dwarf", DWARF); new_object_word("dwarves", DWARF);
    new_object_word("knife", KNIFE); new_object_word("knives", KNIFE);
    new_object_word("food", FOOD); new_object_word("rations", FOOD);
    new_object_word("bottle", BOTTLE); new_object_word("jar", BOTTLE);
    new_object_word("water", WATER); new_object_word("h2o", WATER);
    new_object_word("oil", OIL);
    new_object_word("mirror", MIRROR);
    new_object_word("plant", PLANT); new_object_word("beansta", PLANT);
    new_object_word("stalact", STALACTITE);
    new_object_word("shadow", SHADOW); new_object_word("figure", SHADOW);
    new_object_word("axe", AXE);
    new_object_word("drawing", DRAWINGS);
    new_object_word("pirate", PIRATE);
    new_object_word("dragon", DRAGON);
    new_object_word("chasm", CHASM);
    new_object_word("troll", TROLL);
    new_object_word("bear", BEAR);
    new_object_word("message", MESSAGE);
    new_object_word("volcano", GORGE); new_object_word("geyser", GORGE);
    new_object_word("vending", MACHINE); new_object_word("machine", MACHINE);
    new_object_word("battery", BATTERIES); new_object_word("batteri", BATTERIES);
    new_object_word("moss", MOSS); new_object_word("carpet", MOSS);
    new_object_word("owl", OWL);
    new_object_word("web", WEB);
    new_object_word("spider", SPIDER);
    new_object_word("documen", DOCUMENTS); new_object_word("legal", DOCUMENTS);
    new_object_word("deed", DOCUMENTS); new_object_word("deeds", DOCUMENTS);
    new_object_word("plan", DOCUMENTS); new_object_word("plans", DOCUMENTS);
    new_object_word("papers", DOCUMENTS);
    new_object_word("spoon", SPOON); new_object_word("inscrip", SPOON);
    new_object_word("horn", HORN);
    new_object_word("rat", RATS); new_object_word("rats", RATS);
    new_object_word("giant", GIANT); new_object_word("man", GIANT);
    new_object_word("flagsto", FLAGSTONE);
    new_object_word("gold", GOLD); new_object_word("nugget", GOLD);
    new_object_word("diamond", DIAMONDS);
    new_object_word("silver", SILVER); new_object_word("bars", SILVER);
    new_object_word("jewels", JEWELS);
    new_object_word("jewelry", JEWELS);
    new_object_word("coins", COINS);
    new_object_word("chest", CHEST); new_object_word("box", CHEST);
    new_object_word("treasur", CHEST);
    new_object_word("eggs", EGGS); new_object_word("egg", EGGS);
    new_object_word("nest", EGGS);
    new_object_word("trident", TRIDENT);
    new_object_word("ming", VASE); new_object_word("vase", VASE);
    new_object_word("shards", VASE); new_object_word("pottery", VASE);
    new_object_word("emerald", EMERALD);
    new_object_word("platinu", PYRAMID); new_object_word("pyramid", PYRAMID);
    new_object_word("pearl", PEARL);
    new_object_word("persian", RUG); new_object_word("rug", RUG);
    new_object_word("spices", SPICES);
    new_object_word("chain", CHAIN);
    new_object_word("crown", CROWN);
    new_object_word("ivory", TUSK); new_object_word("tusk", TUSK);
    new_object_word("inlaid", CHALICE); new_object_word("chalice", CHALICE);
    new_object_word("unicorn", CHALICE);
    new_object_word("ruby", RUBY);
    /* The noun "GLOBE" works at the cellar view; the nouns "CRYSTAL" and "ORB"
     * work only in the circular cellar itself. */
    new_object_word("orb", ORB); new_object_word("crystal", ORB);
    new_object_word("globe", FAKE_ORB);

    new_action_word("take", TAKE); new_action_word("carry", TAKE);
    new_action_word("keep", TAKE); new_action_word("catch", TAKE);
    new_action_word("capture", TAKE); new_action_word("steal", TAKE);
    new_action_word("get", TAKE); new_action_word("tote", TAKE);
    new_action_word("lift", TAKE); new_action_word("raise", TAKE);
    new_action_word("drop", DROP); new_action_word("release", DROP);
    new_action_word("free", DROP); new_action_word("discard", DROP);
    new_action_word("dump", DROP);
    new_action_word("open", OPEN); new_action_word("unlock", OPEN);
    new_action_word("close", CLOSE); new_action_word("lock", CLOSE);
    new_action_word("shut", CLOSE);
    new_action_word("light", ON); new_action_word("on", ON);
    new_action_word("extingu", OFF); new_action_word("off", OFF);
    new_action_word("wave", WAVE); new_action_word("shake", WAVE);
    new_action_word("swing", WAVE);
    new_action_word("calm", CALM); new_action_word("placate", CALM);
    new_action_word("tame", CALM);
    new_action_word("walk", GO); new_action_word("run", GO);
    new_action_word("travel", GO); new_action_word("go", GO);
    new_action_word("proceed", GO); new_action_word("explore", GO);
    new_action_word("goto", GO); new_action_word("follow", GO);
    new_action_word("turn", GO);
    new_action_word("nothing", RELAX);
    new_action_word("pour", POUR); new_action_word("empty", POUR);
    new_action_word("eat", EAT); new_action_word("devour", EAT);
    new_action_word("drink", DRINK);
    new_action_word("rub", RUB); new_action_word("polish", RUB);
    new_action_word("throw", TOSS); new_action_word("toss", TOSS);
    new_action_word("wake", WAKE); new_action_word("disturb", WAKE);
    new_action_word("feed", FEED);
    new_action_word("fill", FILL);
    new_action_word("break", BREAK); new_action_word("smash", BREAK);
    new_action_word("shatter", BREAK);
    new_action_word("blast", BLAST); new_action_word("detonat", BLAST);
    new_action_word("ignite", BLAST); new_action_word("blowup", BLAST);
    new_action_word("explode", BLAST);
    new_action_word("attack", KILL); new_action_word("kill", KILL);
    new_action_word("fight", KILL); new_action_word("hit", KILL);
    new_action_word("strike", KILL);
    new_action_word("say", SAY);
    new_action_word("chant", SAY);
    new_action_word("sing", SAY);
    new_action_word("utter", SAY);
    new_action_word("mumble", SAY);
    new_action_word("read", READ); new_action_word("perus", READ);
    new_action_word("fee", FEEFIE); new_action_word("fie", FEEFIE);
    new_action_word("foe", FEEFIE); new_action_word("foo", FEEFIE);
    new_action_word("fum", FEEFIE);
    new_action_word("brief", BRIEF);
    new_action_word("find", FIND); new_action_word("where", FIND);
    new_action_word("seek", FIND);
    new_action_word("read", READ); new_action_word("peruse", READ);
    new_action_word("hoot", HOOT);
    new_action_word("blow", BLOW); new_action_word("play", BLOW);
    new_action_word("fly", FLY);
    new_action_word("mount", RIDE); new_action_word("ride", RIDE);
    new_action_word("sleep", SLEEP);
    new_action_word("rest", REST);
    new_action_word("scream", SCREAM);
    new_action_word("invento", INVENTORY);
    new_action_word("score", SCORE);
    new_action_word("quit", QUIT); new_action_word("end", QUIT);
#ifdef SAVE_AND_RESTORE
    new_action_word("save", SAVE);
    new_action_word("resto", RESTORE);
#endif /* SAVE_AND_RESTORE */

    /* Finally, our vocabulary is rounded out by words like HELP, which
     * trigger the printing of fixed messages. */
    new_message_word("abra", ABRA);
    new_message_word("abracad", ABRA);
    new_message_word("openses", ABRA);
    new_message_word("sesamy", ABRA);  /* Pike does not recognize SESAME (the correct spelling). */
    new_message_word("shazah", ABRA);  /* Pike does not recognize SHAZAM (the spelling Woods presumably intended). */
    new_message_word("hocus", ABRA);
    new_message_word("pocus", ABRA);
    new_message_word("help", HELP);
    new_message_word("?", HELP);
    new_message_word("tree", TREES);
    new_message_word("trees", TREES);
    new_message_word("dig", DIG);
    new_message_word("excavat", DIG);
    new_message_word("lost", LOST);
    new_message_word("mist", MIST);
    new_message_word("fuck", FUCK);
    new_message_word("stop", STOP);
    new_message_word("info", INFO);
    new_message_word("informa", INFO);
    new_message_word("swim", SWIM);
    new_message_word("push", PUSH); new_message_word("pull", PUSH);
    new_message_word("epns", EPNS);
    new_message_word("bone", BONES); new_message_word("bones", BONES);
    new_message_word("debris", DEBRIS);
    new_message_word("dung", DUNG); new_message_word("droppin", DUNG);
    new_message_word("turd", DUNG); new_message_word("turds", DUNG);
    new_message_word("shit", DUNG);
    new_message_word("grill", GRILL);
    new_message_word("curtain", CURTAIN);
    new_message_word("portcul", PORTCULLIS);
    new_message_word("stone", STONE);
    new_message_word("pebbl", STONE);  /* Pike recognizes PEBBLABC. */

    /* Sanity checking. See the comments in streq(). */
    assert(lookup("north") == N);
    assert(lookup("south") == S);
    assert(lookup("north-") == NE);
    assert(lookup("south-") == SE);
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
    R_DUNGEON,
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
    R_TPASS, R_GRAB,
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

Instruction travels[951];
Instruction *start[MAX_LOC+2];
struct Place {
    bool has_long_desc;
    bool has_short_desc;
    unsigned int flags;
    ObjectWord objects;  /* first object at this location, or NOTHING */
    int visits;
};
struct Place places[MAX_LOC+1];

bool now_in_darkness(Location loc);
void describe_object(ObjectWord t, Location loc);
bool is_forced(Location loc);

void make_loc(Instruction *q, Location x, bool has_long_desc, bool has_short_desc, unsigned int f)
{
    static int last_x = 0;
    assert(x == last_x + 1);
    last_x = x;

    assert(has_long_desc || is_forced(x));

    places[x].has_long_desc = has_long_desc;
    places[x].has_short_desc = has_short_desc;
    places[x].flags = f;
    places[x].objects = NOTHING;
    places[x].visits = 0;
    start[x] = q;
}

void make_inst(Instruction *q, MotionWord m, int c, Location d)
{
    assert(&travels[0] <= q && q < &travels[951]);
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
    /* This is a huge kludge to make room descriptions optimizable on the Z-machine.
     * We don't allow the description strings to show up outside of "sizeof" context
     * in this routine. This does have the nasty fragile side effect that any short
     * description of 4 or 8 characters will be ignored; but fortunately no such
     * descriptions exist. */
#define make_loc(q,x,l,s,f) make_loc(q,x,sizeof(l)!=sizeof(NULL),sizeof(s)!=sizeof(NULL),f)
#define make_ins(m, d) make_inst(q++, m, 0, d)
#define make_cond_ins(m, c, d) make_inst(q++, m, c, d)
#define ditto(m) make_inst(q, m, q[-1].cond, q[-1].dest); ++q;
#include "locs.h"
#undef ditto
#undef make_cond_ins
#undef make_ins
#undef make_loc
    /* The remaining "locations" R_PPASS, R_PDROP, R_TROLL, R_TPASS, and R_GRAB are special. */
    start[R_PPASS] = q;
}

void print_long_room_description(Location loc)
{
    if (!places[loc].has_long_desc) {
        assert(is_forced(loc));
        return;
    }
    switch (loc) {
#define make_loc(q,x,l,s,f) case x: puts(l); break;
#define make_ins(x,y)
#define make_cond_ins(x,y,z)
#define ditto(x)
#include "locs.h"
#undef ditto
#undef make_cond_ins
#undef make_ins
#undef make_loc
        default:
            assert(false);
    }
}

void print_short_room_description(Location loc)
{
    if (!places[loc].has_short_desc) {
        assert(is_forced(loc));
        return;
    }
    switch (loc) {
#define make_loc(q,x,l,s,f) case x: puts(s); break;
#define make_ins(x,y)
#define make_cond_ins(x,y,z)
#define ditto(x)
#include "locs.h"
#undef ditto
#undef make_cond_ins
#undef make_ins
#undef make_loc
        default:
            assert(false);
    }
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

bool forbidden_to_owl(Location loc)
{
    switch (loc) {
        case R_ROAD: case R_HILL: case R_HOUSE: case R_VALLEY:
        case R_FOREST: case R_FOREST2: case R_SLIT: case R_OUTSIDE:
        case R_INSIDE:
        case R_176: case R_177: case R_178:
        case R_179: case R_180: case R_181:
        case R_182: case R_183: case R_184:
        case R_185: case R_186: case R_187:
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
int tally = 20;  /* treasures awaiting you */
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
        "\nSuddenly a giant hand descends from above, picks you up," SOFT_NL
        "and shakes you, scattering your possessions across the floor.";
    objs(GIANT).desc[2] = "The giant reclines on the couch eating eggs!";
    objs(GIANT).desc[3] = "\nA giant hand descends from above and picks you up.";
    new_obj(FLAGSTONE, 0, FLAGSTONE, R_DUNGEON);
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
    objs(RUBY).desc[0] = "A very large ruby lies here!";
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

    /* Contrariwise, the little horn starts out in the dwarves'
     * quarters (inaccessible), and wants to move to R_188 (also
     * inaccessible). What this means is that it cannot be encountered
     * by normal means; but the first dwarf you kill will pick it up,
     * and then carry it forever (since the dwarf will never reach R_188
     * by wandering). When you kill that dwarf, it will drop the horn.
     */
    move(HORN, R_163);
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
    if (is_forced(loc)) {
        return true;
    }
    switch (loc) {
        case R_DEAD3: case R_DEAD4: case R_DEAD5:
        case R_DEAD6: case R_DEAD7: case R_DEAD8:
        case R_DEAD9: case R_DEAD10: case R_DEAD11:
        case R_NESIDE: case R_CORR: case R_FORK: case R_WARM:
        case R_VIEW: case R_CHAMBER: case R_LIME: case R_FBARR:
        case R_BARR:  /* dwarves won't meet the bear */
        case R_182: case R_183: case R_184: case R_185:
        case R_186: case R_187:  /* also forbid the sewer maze, so as not to steal the tusk */
            return true;
        default:
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
            /* These are disallowed basically by accident. Pike had
             * originally disallowed merely CLAM/OYSTER, but realized in
             * March 1980 (according to codbug.txt) that he also needed
             * to disallow CAGE/BIRD. The items whose numbers fell
             * between those two ranges were collateral damage. */
            return true;
        /* Pike also lists the TREADS, DOOR, SNAKE, FISSURE, TABLET, TROLL,
         * and OWL, but these are already 100% immobile, so we don't need
         * to repeat them here. */
        default:
            return false;
    }
}

void dwarves_tote_objects(Location loc)
{
    /* Jack Pike wrote to me on 2014-03-20: "All the dwarves toting makes it
     * difficult to solve the maze where all locations are the same because
     * they move the dropped objects. To make it easier I restricted the
     * number of toting dwarves [from Woods' 5 down to 3]."
     * Dwarf number 0 is the pirate; he never totes anything.
     */
    for (int i=1; i <= 3; ++i)
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

Location distnt(Location from, int howfar)
{
    Location loc = from;
    for (int h=0; h < howfar; ++h) {
        Location ploc[19];  /* potential locations for the next random step */
        int i = 0;
        /* Make a table of all potential exits, similar to how
         * move_dwarves_and_pirate() does it. */
        for (Instruction *q = start[loc]; q < start[loc + 1]; ++q) {
            Location newloc = q->dest;
            if (i != 0 && newloc == ploc[i-1]) continue;
            /* Unlike dwarves, the owl is permitted to double back. */
            if (forbidden_to_owl(newloc)) continue;
            if (is_forced(newloc) || newloc > MAX_LOC) continue;
            ploc[i++] = newloc;
        }
        if (i == 0) {
            /* Here, Pike restarts the loop if we've gotten ourselves into a dead end.
             * This could cause an infinite loop if there were a room all of whose
             * exits led (after one or two hops) to dead ends; but since the only
             * dead end on the map is the wizard's cellar, and the room outside the
             * cellar has two exits, we're all good. */
            return distnt(from, howfar);
        }
        loc = ploc[ran(i)];
    }
    /* Mike Arnautov added some code here to ensure that the owl never
     * accidentally flies into the wizard's cellar and gets trapped.
     * I've preserved that code here. */
    if (loc == R_CELLAR || loc == from) {
        loc = R_141;
    }
    return loc;
}

void move_owl(Location loc)
{
    if (there(OWL, loc) && !now_in_darkness(loc)) {
        puts("The light from your lamp disturbs an enormous owl which" SOFT_NL
             "flies off with a flurry of wingbeats (and a loud \"HOOT\").");

        Location owl_loc;
        do {
            /* The owl flies off to somewhere three "hops" from your current room,
             * which is dark and free of spiders. (That room can't currently be
             * lit by the lamp because you must be carrying the lamp with you
             * in order to scare the owl in the first place.) */
            owl_loc = distnt(loc, 3);
        } while (owl_loc == loc || owl_loc == R_148 || has_light(owl_loc));
        move(OWL, owl_loc);
    }
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
    objs(WATER).place = R_LIMBO;
    objs(OIL).place = R_LIMBO;
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
int dung0 = 0;  /* how many turns the player has been sitting in the dungeon */
bool wizard_mode = false;

int original_room_number(int loc, bool convert_backwards)
{
    Location original_locations[200] = {
        0, R_ROAD, R_HILL, R_HOUSE, R_VALLEY, R_FOREST, R_FOREST2, R_SLIT, R_OUTSIDE,
        R_INSIDE, R_COBBLES, R_DEBRIS, R_AWK, R_BIRD, R_SPIT, R_EMIST,
        0, R_EFISS, R_NUGGET, R_HMK, R_NECK, R_LOSE,
        0, R_W2PIT, R_EPIT, R_WPIT, R_CLIMB, R_WFISS, R_NS, R_SOUTH, R_WEST, R_CHECK,
        0, R_Y2, R_JUMBLE, R_WINDOE, R_DIRTY, R_CLEAN, R_WET, R_DUSTY, R_THRU, R_WMIST,
        R_LIKE1, R_LIKE2, R_LIKE3, R_LIKE4, R_DEAD3, R_DEAD4, R_DEAD5,
        R_LIKE5, R_LIKE6, R_LIKE7, R_LIKE8, R_LIKE9, R_DEAD6, R_LIKE10,
        R_DEAD7, R_BRINK, R_DEAD8, R_DUCK,
        R_ELONG, R_WLONG, R_CROSS, R_DEAD0,
        R_COMPLEX, R_BEDQUILT, R_SWISS, R_E2PIT, R_SLAB, R_ABOVER, R_ABOVEP,
        R_SJUNC, R_LOW, R_CRAWL, R_SECRET, R_WIDE, R_TIGHT, R_TALL, R_BOULDERS,
        0, R_LIKE11, R_DEAD1, R_DEAD9, R_LIKE12, R_LIKE13, R_DEAD10, R_DEAD11, R_LIKE14,
        R_NARROW, R_UPNOUT, R_DIDIT, R_INCLINE, R_GIANT, R_BLOCK, R_IMMENSE, R_FALLS,
        R_SOFT, R_ORIENTAL, R_MISTY, R_ALCOVE, R_PLOVER, R_DARK,
        R_ARCHED, R_SHELL, R_RAGGED, R_SAC, R_ANTE, R_DIFF0, R_WITT,
        R_MIRROR, R_WINDOW, R_TITE, R_DIFF10, R_RES, R_PIRATES_NEST, R_NEEND, R_SWEND,
        R_SWSIDE, R_SLOPING, R_SCAN1, R_SCAN2, R_SCAN3, R_NESIDE, R_CORR, R_FORK, R_WARM,
        R_VIEW, R_CHAMBER, R_LIME, R_FBARR, R_BARR,
        R_DIFF1, R_DIFF2, R_DIFF3, R_DIFF4, R_DIFF5, R_DIFF6, R_DIFF7, R_DIFF8, R_DIFF9, R_PONY,
        R_141, R_142, R_143, R_144, R_DUNGEON, R_146, R_147, R_148, R_149, R_150, R_151, R_152, R_153,
        R_CELLAR, R_155, R_156, R_157, R_158, R_159, R_160, R_161, R_162, R_163, R_164, R_165,
        R_166, R_167, R_168, R_169, R_170, R_171, R_172, R_PANTRY, R_174, R_175, R_176, R_177,
        R_178, R_179, R_180, R_181, R_182, R_183, R_184, R_185, R_186, R_187, R_188,
        R_189, R_190, R_191, R_192, R_193, R_194, R_195, R_196, R_197, R_198, R_199
    };
    assert(original_locations[16] == 0);  /* pseudo-rooms that I've turned into remarks */
    assert(original_locations[22] == 0);
    assert(original_locations[32] == 0);
    assert(original_locations[79] == 0);
    assert(original_locations[114] == R_PIRATES_NEST);
    assert(original_locations[140] == R_PONY);
    assert(original_locations[199] == R_199);

    if (convert_backwards) {
        /* Convert an original number to an internal number, for use by FLY. */
        if (1 <= loc && loc <= 199) {
            return original_locations[loc];
        } else {
            return 0;
        }
    } else {
        /* Convert an internal number to an original number, for use by HOOT. */
        for (int i=0; i < 200; ++i) {
            if (original_locations[i] == loc) {
                return i;
            }
        }
        assert(false);
        return 0;
    }
}

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
    if (t == RATS) {
        switch (objs(RATS).prop) {
            case 0: case 1: break;
            case 2: puts("There is some *thing* swimming in the sewage!"); break;
            case 3: puts("You feel a vicious bite on the ankle from something in the sewage!"); break;
            case 4: puts("A large black sewer rat eyes you hungrily from the edge of the" SOFT_NL
                         "sewage, then slips below the surface............."); break;
            case 5: puts("There are sewer rats all around you, their eyes glinting in" SOFT_NL
                         "the lamp light."); break;
            case 6: puts("You feel several bites on your legs.  You try to protect them, but" SOFT_NL
                         "more rats sink their teeth into your arms; then one leaps from" SOFT_NL
                         "the sewage and hangs on to your ear, weighing you down; now they are" SOFT_NL
                         "at your throat.............");
                    /* At this point you die, but that's handled elsewhere. */
                    break;
            default: assert(false);
        }
        return;
    }
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

int look_around(Location loc, bool dark, bool was_dark, ActionWord verb)
{
    enum { PITCH_DARK=1, LONG_DESC, SHORT_DESC };
    int room_description = 0;
    if (dark && !is_forced(loc)) {
        if (was_dark && (verb != BLOW) && (verb != HOOT) && pct(35)) return 'p';  /* goto pitch_dark; */
        room_description = PITCH_DARK;
    } else if (!places[loc].has_short_desc || places[loc].visits % verbose_interval == 0) {
        room_description = LONG_DESC;
    } else {
        room_description = SHORT_DESC;
    }
    if (toting(BEAR)) {
        puts("You are being followed by a very large, tame bear.");
    }
    switch (room_description) {
        case 0:  /* R_CHECK's description is NULL. */
            break;
        case PITCH_DARK:
            puts("It is now pitch dark.  If you proceed you will likely fall into a pit.");
            break;
        case LONG_DESC:
            print_long_room_description(loc);
            break;
        case SHORT_DESC:
            print_short_room_description(loc);
            break;
        default:
            assert(false);
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
                    puts("You have drowned horribly in a mixture of sea-water and sewage!\n");
                    return R_LIMBO;
                case 5: puts("You are nearly up to your neck in sewage.  Help!"); return loc;
                case 6: puts("The sewage comes nearly up to your chest! Let's get out of here!"); return loc;
                case 7: puts("You are up to your waist in sewage. Let's leave!"); return loc;
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
    thirst += (holding_count + 2);
    if (loc == R_VIEW) {
        /* Visiting the volcano view pushes your thirst counter rapidly
         * toward 880. Jack Pike wrote to me on 2014-03-20: "The logic behind
         * this was that it was very hot in the volcano room and you were
         * going to get very thirsty if you stayed there. This worked with
         * the original thirst counter at about 800 but introduced a bug
         * when it was increased to 900!"
         *
         * After that thirst-counter increase, it became possible to hang out
         * at the volcano indefinitely, since you'd never thirst to death and
         * you'd never fall in a pit (the volcano view has its own light).
         */
        thirst = (880 + thirst) / 2;
    }
    if (thirst >= 900) {
        puts("You have collapsed and died of thirst!");
        return true;
    } else if (thirst >= 880) {
        puts("I cannot go much farther without a drink.");
    } else if (previous_thirst < 800 && thirst > 800) {
        puts("Exploring is thirsty work; you must soon take a drink or lighten" SOFT_NL
             "your load.");
    }
    return false;
}

void maybe_spot_orc(Location loc)
{
    static int orct = 0;
    static int previous_tally = 0;
    if (orct < 41) {
        ++orct;
        if (orct != 41) return;
    }
    if (cave_is_closing() || closed) return;
    if (dwarf_at(loc)) return;  /* Pike actually tests DTOTAL, but that should be equivalent here. */
    int roomno = original_room_number(loc, /*convert_backwards=*/false);
    if (roomno < 20 || 150 < roomno) return;
    if (tally == previous_tally && !now_in_darkness(loc)) {
        static bool given_hint[3] = { false, false, false };
        if (objs(FISSURE).prop == 0 && !given_hint[0]) {
            puts("Suddenly, as if from nowhere, appears a little figure.  He looks at" SOFT_NL
                 "you quizzically and then says in a deep voice:\n"
                 "\"I'll tell you a secret. You can't fly or jump across the fissure," SOFT_NL
                 "but you might get across by using magic.\"\n"
                 "Then he hops from foot to foot chuckling to himself and disappears" SOFT_NL
                 "into the gloom just as suddenly as he came.");
            given_hint[0] = true;
        } else if (objs(CHALICE).prop == -1 && !given_hint[1]) {
            puts("Out from the gloom jumps a little figure.  He looks at you and" SOFT_NL
                 "says in a surprisingly deep voice, \"Chalice, chalice? now where did" SOFT_NL
                 "I put that chalice?  If you should perchance find it, be careful," SOFT_NL
                 "for it is said to have strange powers.\"" SOFT_NL
                 "With that he scurries off back into the gloom.");
            given_hint[1] = true;
        } else if (objs(RUSTY_DOOR).prop == 1 && !given_hint[2]) {
            puts("A deep voice behind you says:\n"
                 "\"Where do the eggs come from? There is no goose around here, and the" SOFT_NL
                 "owl can't have done it...\" You turn round in time to see a small" SOFT_NL
                 "figure disappear muttering, \"It must be magic again!\"");
            given_hint[2] = true;
        }
    }
    previous_tally = tally;
    orct = 1;
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
    "This particular program was translated from Fortran to ANSI C by Arthur O'Dwyer.\n\n" },
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

#define MAX_SCORE 440
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

/* Interestingly, Pike's scoring system tops out at 430 points;
 * you can lose nine points (relative to the highest possible score)
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
    for (rank = 0; class_score[rank] < s; ++rank) ;
    printf("%s\nTo achieve the next higher rating", class_message[rank]);
    if (rank < HIGHEST_CLASS) {
        int delta = class_score[rank] + 1 - s;
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
    if (has_sewage(last_safe_place)) {
        last_safe_place = R_180;
    }

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
    thirst = 0;
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
    /* Sewer section.
     * Any item allowed through this passage will have to work properly when
     * dropped into either the slippery shaft or the sewage. The bottle, cage,
     * and vase must be kept out.
     * Contrariwise, the point of the puzzle is to keep the tusk in.
     * Pike forbids various "hard items" (nugget, chest, chalice, bars, trident,
     * pyramid, eggs, crown) for no reason other than realism.
     */
    ObjectWord prohibited_items[] = {
        VASE, BOTTLE, CAGE,
        TUSK,
        GOLD, CHEST, CHALICE, SILVER, TRIDENT, PYRAMID, EGGS, CROWN
    };
    for (int i=0; i < (int)(sizeof prohibited_items / sizeof *prohibited_items); ++i) {
        if (toting(prohibited_items[i])) {
            puts("You may just get your lamp though that awkward slit, but most hard" SOFT_NL
                 "objects will not go through."); /* Pike has "though" */
            return from;
        }
    }
    return (from == R_174) ? R_176 : R_174;
}

bool giant_can_shake_loose(ObjectWord obj)
{
    /* The giant will shake loose certain items from you. Pike starts iterating
     * through the objects starting with the axe's index (number 28), which
     * means you get to keep a lot of items, including the lamp.
     * However, the list of shake-loose-able items includes all the treasures.
     */
    switch (obj)
    {
        case AXE: case BEAR: case BATTERIES: case DOCUMENTS:
        case SPOON: case HORN:
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

Location giant_grabs_you(Location oldoldloc, Location oldloc, Location loc);

Location attempt_giant_quarters(Location oldoldloc, Location oldloc, Location loc)  /* line 30500 in Pike */
{
    if (objs(GIANT).prop <= 0 && !toting(EGGS)) {
        objs(GIANT).prop = (holding_count <= 1) ? 3 : 1;
        return R_143;
    }

    if (objs(GIANT).prop != 2) {
        if (!toting(EGGS)) goto line_30530;
        objs(GIANT).prop = 2;
        move(EGGS, R_LIMBO);
        puts("\"AH... EGG FOO... YUM YUM!\"" SOFT_NL
             "the giant drawls, and grabs the eggs, leaving you shaken but unharmed.");
    }
    if (toting(DOCUMENTS)) {
        puts("\"MY DEEDS!\" the giant shouts. \"For those you shall be rewarded!\"" SOFT_NL
             "He tosses a large ruby in your direction as he takes the documents.");
        move(DOCUMENTS, R_LIMBO);
        move(RUBY, R_143);
    }
    return R_143;
line_30530:
    loc = R_DUNGEON;
    if (toting(FOOD)) {
        puts("The giant grabs your food and bellows \"A MISERABLE MORSEL!\"" SOFT_NL
             "but stuffs it in his mouth and swallows it whole.");
        move(FOOD, R_163);
        loc = R_PANTRY;
    }
    for (int kobj = MIN_OBJ; kobj <= MAX_OBJ; ++kobj) {
        if (giant_can_shake_loose(kobj) && toting(kobj)) {
            drop(kobj, R_143);
            if (is_treasure(kobj)) loc = R_PANTRY;
        }
    }
    if (loc == R_PANTRY) {
        puts("\"FEE FIE FOE FOO " EMDASH("--") SOFT_NL
             "FIN FOO OR I'LL EAT YOU!\"" SOFT_NL
             "the giant thunders, and as you cast about for your possessions you" SOFT_NL
             "are pushed back into the pantry.");
        objs(GIANT).prop = 0;
        return R_PANTRY;
    }
    return giant_grabs_you(oldoldloc, oldloc, loc);
}

Location giant_grabs_you(Location oldoldloc, Location oldloc, Location loc)
{
    if (oldloc != R_DUNGEON) {
        dung0 = 0;
        puts("\"FEE FIE FOE FOO!\"" SOFT_NL
             "the giant thunders, and at the thought of foo' great slivers of" SOFT_NL
             "saliva issue from the giant's mouth and further soil his already" SOFT_NL
             "filthy shirt front.");
    }
    objs(GIANT).prop = 0;
    if (oldoldloc == R_DUNGEON) {
        puts("This time he gets you. He is now wearing a bib which is even more" SOFT_NL
             "soiled than his shirt. He opens his mouth wide, revealing great" SOFT_NL
             "rows of black teeth. The stench of his breath mercifully renders you" SOFT_NL
             "unconscious as he bites into your leg and pulls...................");
        return R_LIMBO;
    }
    puts("\"STAY IN THERE UNTIL I AM READY TO EAT YOU!!\"" SOFT_NL
         "the giant bellows and throws you into his dungeon.");
    return loc;
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
            move(FOOD, R_163);
            puts("Thank you, it was delicious!");
            break;
        case BIRD: case SNAKE: case CLAM: case OYSTER:
        case DWARF: case DRAGON: case TROLL: case BEAR:
        case SPIDER:
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
    } else if (obj == FAKE_ORB) {
        puts("It lies out of reach on the cellar floor!");
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
        if (obj == BOTTLE && bottle_contents() != NOTHING) {
            objs(bottle_contents()).place = R_INHAND;
        } else if (obj == DOCUMENTS) {
            objs(DOCUMENTS).prop = 1;  /* no longer stuck to the web */
        }
        puts(ok);
    }
    return false;
}

Location get_kicked_out_of_cellar()
{
    puts("Through the portcullis steps a tall wizard clothed in grey. \"Be off!\"" SOFT_NL
         "he commands, raising an arm. As the scene fades his distant voice" SOFT_NL
         "continues, \"And take your belongings with you!\".");

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
     * the (real) orb can never be successfully dropped in the cellar.
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
        if (there(PILLOW, loc)) {
            /* In Long's "Adventure 6", the response is this message
             * plus the default "Dropped." */
            puts("The vase is now resting, delicately, on a velvet pillow.");
        } else {
            puts("The Ming vase drops with a delicate crash.");
            objs(VASE).prop = 1;  /* the vase is now broken */
            immobilize(VASE);
        }
        if (loc == R_CELLAR) {
            loc = get_kicked_out_of_cellar();
        }
        drop(VASE, loc);
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
            return loc;
        } else {
            puts(ok);
            if (obj == TUSK && loc == R_HOUSE) {
                puts("Tusk... Tusk... Tusk... That's not on the list!" SOFT_NL
                     "It's ivory isn't it? must be valuable..." SOFT_NL
                     "OK, we'll give you the benefit of the doubt.");
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
        if (obj == BOTTLE && bottle_contents() != NOTHING) {
            objs(bottle_contents()).place = R_LIMBO;
        }

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
    } else if (obj == SPOON && objs(SPOON).prop == 0) {
        puts("Oh! There appears to be an inscription on the spoon.");
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
    } else if (obj == CHALICE) {
        puts("The chalice is very old and has several holes in the bottom!" SOFT_NL
             "At best you might use it to scoop a drink from a pool or stream.");
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
        case SPIDER:
            if (!closed && here(BIRD, loc)) {
                if (obj == SNAKE) {
                    puts("The snake has now devoured your bird.");
                } else {
                    puts("The spider has eaten the poor little bird. It was a" SOFT_NL
                         "horrible sight, all blood, feathers and hairy legs.");
                }
                destroy(BIRD);
                objs(BIRD).prop = 0;
                /* Now that the bird is dead, you can never get past the snake
                 * into the south side chamber, so the precious jewelry is lost. */
                if (there(SNAKE, R_HMK)) ++lost_treasures;
            } else {
                puts("There's nothing here it wants to eat (except perhaps you).");
            }
            break;
        case BEAR:
            if (here(FOOD, loc)) {
                assert(objs(BEAR).prop == 0);
                destroy(FOOD);  /* it doesn't move back to the pantry in this case */
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
        case RATS:
            if (here(FOOD, loc)) {
                puts("The food disappears into the sewage and the water thrashes" SOFT_NL
                     "with activity.  My, those rats *are* hungry!");
                move(FOOD, R_163);
                objs(RATS).prop = 0;
            } else {
                puts("There is nothing here to eat.");
            }
            break;
        case CHALICE:
            if (objs(CHALICE).prop != 1) {
                puts("I'm game.  Would you care to explain how?");
            } else if (here(FOOD, loc)) {
                puts("The unicorn eats the food and looks around for more. On finding none" SOFT_NL
                     "it raises its tail and presents you with a neat pile of dung.");
                move(FOOD, R_163);
                objs(RATS).prop = 0;  /* Pike combines the RATS and CHALICE cases, producing this odd side-effect. */
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

int attempt_hoot(Location loc)
{
    if (closed) {
        puts("An enormous owl appears from nowhere and gives a loud \"HOOT\".");
        dwarves_upset();
    }

    if ((R_176 <= loc && loc <= R_187) || loc == R_DUNGEON || (R_CELLAR <= loc && loc <= R_166)) {
        puts("Nothing happens.");
        return 'c';
    } else if (has_light(loc) || (abs(original_room_number(loc, /*convert_backwards=*/false) - original_room_number(objs(OWL).place, /*convert_backwards=*/false)) / 25 > ran(5))) {
        puts("A distant owl calls \"HOOT\".");
        return 'd';  /* move the dwarves but not the owl; this is Pike's line 7404 */
    } else if (!now_in_darkness(loc)) {
        puts("With a flurry of wingbeats (and a loud \"HOOT\") an enormous owl" SOFT_NL
             "appears. Disturbed by the light from your lamp, he flies off again.");
        return 'o';  /* move the owl; this is Pike's line 7402 */
    } else if ((is_at_loc(DRAGON, loc) && objs(DRAGON).prop == 0) ||
               there(SNAKE, loc) ||
               there(BEAR, loc) ||  /* even a dead bear! */
               (is_at_loc(TROLL, loc) && objs(TROLL).prop < 2) ||
               loc == R_PIRATES_NEST) {
        /* Some creatures scare away the owl. The only mobile such creature
         * is the bear, and Pike doesn't let the bear meet the dwarves or
         * spider. */
        puts("A distant owl calls \"HOOT\".");
        return 'o';  /* move the owl */
    } else {
        /* Summon the owl! */
        const bool owl_was_here = there(OWL, loc);
        bool owl_ate_something = false;
        if (there(SPIDER, loc)) {
            move(SPIDER, R_LIMBO);  /* The owl eats spiders... */
            move(DOCUMENTS, loc);
            move(OWL, loc);
            juggle(WEB);
            owl_ate_something = true;
        } else {
            move(OWL, loc);
        }
        /* ...and also dwarves. */
        for (int i=1; i < 5; ++i) {
            if (dwarves[i].loc == loc) {
                dwarves[i].loc = R_LIMBO;
                dwarves[i].oldloc = R_LIMBO;
                owl_ate_something = true;
            }
        }
        if (owl_ate_something) {
            puts("You hear a flurry of wingbeats and sounds of a tremendous battle" SOFT_NL
                 "(rustle .. rustle .. hisss .. HOOOT!  scrape .. GULP! .. hoot?)");
        } else if (owl_was_here) {
            puts("Nearby an owl cries \"HOOT\".");
        } else {
            puts("You hear a flurry of wingbeats and a loud \"HOOT\".");
        }
        return 'd';  /* move the dwarves but not the owl */
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
             "dull red glow; grotesque creatures pour forth and dance and sway" SOFT_NL
             "as you fall towards the aperture. Your descent is impeded, slowly at" SOFT_NL
             "first and then more firmly, by a web of gossamer threads on which" SOFT_NL
             "you can clearly see stamped at intervals \"Made in Fairyland.\"" SOFT_NL
             "Below you the creatures leap, but try as they might they cannot reach" SOFT_NL
             "you until there appears our horned fiend.  \"You chose the losing" SOFT_NL
             "side, my friend!\" he says, and slashes through the supporting threads." SOFT_NL
             "As you fall you hear his evil laugh and his shout, \"Welcome to Hell!!\"\n\n"
             "Now you really are in a mess! Lost one of our best agents, too." SOFT_NL
             "People like you didn't ought to be allowed in the cave, poking your" SOFT_NL
             "noses into things you can't handle. You'll always lose if you tangle" SOFT_NL
             "with *him* and there is no way of helping the princess that I know of." SOFT_NL
             "I don't think I can stand any more of your bungling! I'm going........\n");
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
bool determine_next_newloc(Location oldloc, Location loc, Location *newloc, MotionWord mot, ActionWord verb)
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
    } else if (*newloc == R_TPASS) {
        *newloc = attempt_tusk_passage(loc);
    } else if (*newloc == R_GRAB) {
        *newloc = attempt_giant_quarters(oldloc, loc, loc);
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
            puts("Electro-plated nickel silver.  It's nearly worthless.");
            break;
        case BONES:
            puts("Bones " EMDASH("--") " UGH!  I've got a thing about bones.  I will have nothing" SOFT_NL
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
    label_move_owl:
        move_owl(loc);
    label_move_dwarves:
        if (move_dwarves_and_pirate(loc)) {
            oldoldloc = loc;
            goto death;
        }
    commence:  /* line 2000 in Pike */
        if (true) {
            Location newloc = announce_tides(loc);
            if (newloc == R_LIMBO) {
                oldoldloc = loc;
                goto death;
            } else if (maybe_die_of_thirst(newloc)) {
                oldoldloc = newloc;
                goto death;
            }
            loc = newloc;
        }
        maybe_spot_orc(loc);
        switch (look_around(loc, now_in_darkness(loc), was_dark, verb)) {
            case 'p': goto pitch_dark;
            case 't': goto try_move;
            default: break;
        }

        if (loc == R_182 && objs(RATS).prop == 6) {
            /* Spend too much time in the culvert in the dark and you'll
             * die with no explanation, since the "rats kill you" message
             * is visible only when the room is lighted. */
            oldoldloc = loc;
            goto death;
        }

        if (objs(GIANT).prop % 2 == 1) {
            /* The player has just entered the giant's quarters. */
            loc = attempt_giant_quarters(oldoldloc, oldloc, loc);
            goto commence;
        } else if (loc == R_DUNGEON) {
            /* line 30600 in Pike */
            dung0 += 1;
            if (now_in_darkness(loc)) {
                /* As long as he's in darkness, the player is safe. */
            } else if ((dung0 == 3 || dung0 == 5) && objs(GIANT).prop != 2) {
                puts("The giant's hand enters the dungeon and makes a grab, but he misses!");
            } else if (dung0 == 4) {
                puts("A tiny mouse appears from beneath a flagstone, sees you and hastily" SOFT_NL
                     "retreats.");
            } else if (dung0 == 7 && objs(GIANT).prop != 2) {
                loc = giant_grabs_you(oldoldloc, oldloc, loc);
                goto commence;
            }
        } else if (objs(CHALICE).prop == 1 && !there(CHALICE, loc)) {
            /* The unicorn runs away if you don't jump on. */
            assert(objs(CHALICE).place == R_WET || objs(CHALICE).place == R_RES || objs(CHALICE).place == R_LIMBO);
            move(CHALICE, R_LIMBO);
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
            if (lookup(word1) == ENTER) {
                if (lookup(word2) == WATER || lookup(word2) == STREAM) {
                    if (has_water(loc)) {
                        puts("Your feet are now wet.");
                    } else {
                        puts("Where?");
                    }
                    continue;
                } else if (*word2 != '\0') {
                    /* ENTER TUNNEL becomes just TUNNEL. */
                    shift_words();
                    goto parse;
                }
            }
            if (lookup(word1) == WATER || lookup(word1) == OIL) {
                /* Sometimes "water" and "oil" are used as verbs. */
                if (lookup(word2) == PLANT && there(PLANT, loc))
                    strcpy(word2, "pour");
                if (lookup(word2) == RUSTY_DOOR && there(RUSTY_DOOR, loc))
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
                            objs(TROLL).prop = 1;  /* the troll returns, but Pike's giant is unaffected */
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
                case HOOT:
                    switch (attempt_hoot(loc))
                    {
                        case 'c': break;
                        case 'd': goto label_move_dwarves;
                        case 'o': goto label_move_owl;
                        default: assert(false);
                    }
                    continue;
                case FLY:
                    if (wizard_mode) {
                        puts("Note: This means of travel can generate internal inconsistencies," SOFT_NL
                              "so take care. Now enter new LOCATION number.");
                        verb = NOTHING;
                        obj = NOTHING;
                        listen();
                        int wizloc = atoi(word1);
                        wizloc = original_room_number(wizloc, /*convert_backwards=*/ true);
                        if (wizloc == R_LIMBO) {
                            /* for the sake of completeness */
                            goto death;
                        } else if (R_ROAD <= wizloc && wizloc <= R_SWEND) {
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
                    if (was_dark) goto label_move_owl;
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
                    if (stream_here) {
                        thirst = 0;
                    }
                    if (toting(CHALICE) && (loc == R_WET || loc == R_RES)) {
                        /* Drink from the chalice! Line 9155 in Pike. */
                        bool drank = yes(
                            "A glowing figure of a beautiful woman appears. In a distant voice she" SOFT_NL
                            "says, \"You have drunk pure water from the sacred chalice, my brave" SOFT_NL
                            "adventurer. I come to you as the spirit of goodness, but my power is" SOFT_NL
                            "weak, for the wicked dwarves have stolen the Ring of Orion and have" SOFT_NL
                            "hidden it deep in the Land of Mists, where I cannot go. Return the" SOFT_NL
                            "ring to me and together we shall drive the evil from the caves.....\"\n"
                            "A distant roll of thunder drowns the voice, and as the woman fades" SOFT_NL
                            "from a cloud of smoke steps a horned creature with a pointed tail." SOFT_NL
                            "\"If you have anything more to do with that wisp,\" he says, \"you will" SOFT_NL
                            "find yourself in infernal hot water. Just you stick to stealing a few" SOFT_NL
                            "treasures and you won't come to much harm!\" With that he points to the" SOFT_NL
                            "chalice and it crumbles to dust; then with a flick of his tail he" SOFT_NL
                            "disappears.\n"
                            "A ringing in your ears seems to say, \"Will you help me adventurer?\".",

                            "A swirl of mist rises from the floor. Gosh! it's turned into a" SOFT_NL
                            "unicorn!",

                            "The ringing is replaced by a distant laugh.");
                        if (!drank) {
                            move(CHALICE, R_LIMBO);
                        } else {
                            drop(CHALICE, loc);
                            objs(CHALICE).prop = 1;
                            immobilize(CHALICE);
                        }
                        goto commence;
                    }
                    /* Drink from the bottle if we can; otherwise from the stream. */
                    if (evian_here) {
                        if (thirst >= 800) {
                            puts("Thank you, it was delicious!");
                        }
                        objs(BOTTLE).prop = 1;  /* empty */
                        objs(WATER).place = R_LIMBO;
                        thirst = 0;
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
                    if (obj == FLAGSTONE && loc == R_DUNGEON) {
                        puts("The flagstone lifts suddenly and you topple headlong into the" SOFT_NL
                             "darkness below.\n");
                        dung0 = 0;  /* escape the dungeon */
                        loc = R_165;
                        mot = NOWHERE;
                        goto try_move;
                    } else if (attempt_take(obj, loc)) {
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
                    if (now_in_darkness(loc)) {
                        /* Throwing things in darkness never works: you can't see your target
                         * (the troll, dwarves, bear, or orb). This is a change from Woods' version. */
                        oldverb = TOSS;
                        verb = DROP;
                        goto transitive;
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
                    if (loc == R_198) {
                        /* Throw an object at the portcullis. Line 9173 in Pike. */
                        static int throwc = 0;
                        switch (obj) {
                            case VASE: case CAGE: case TUSK: case GOLD:
                            case CHALICE: case BIRD: case BOTTLE: case OIL: case WATER:
                                oldverb = TOSS;
                                verb = DROP;
                                goto transitive;
                            default:
                                break;
                        }
                        drop(obj, (obj == FOOD) ? R_163 : R_180);
                        throwc += ran(3) + 1;
                        if (throwc < 4 || objs(FAKE_ORB).prop != 0) {
                            puts("It lands near the round drain in the cellar, slides towards it and" SOFT_NL
                                 "drops in!");
                        } else {
                            puts("It strikes the globe a glancing blow and slides off into the drain." SOFT_NL
                                 "The globe rocks slightly, revealing a small cross attached to its" SOFT_NL
                                 "far side; then it rolls towards the drain and disappears also.");
                            move(ORB, R_180);
                            objs(FAKE_ORB).prop = 1;
                            destroy(FAKE_ORB); /* Pike doesn't actually destroy FAKE_ORB, but he should. */
                        }
                        continue;
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
                    } else if (here(GIANT, loc)) {
                        drop(AXE, loc);
                        puts("The axe hits the giant, who lets out a roar of pain and rage, raises" SOFT_NL
                             "one foot high in the air..........and brings it down *SPLAT*.");
                        goto death;
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
                        if (here(SPIDER, loc)) { ++k; obj = SPIDER; }
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
                        case SPIDER:
                            puts("I'm not going anywhere near that horrible hairy thing!!");
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
                    puts("OK..................AAAAAAAAAARRRRRRRRRGGGGGGHHHHHHHHH!!!!!!!!!!!!");
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
            printf("What do you want to %s?\n", word1);
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
                if (determine_next_newloc(oldloc, loc, &newloc, mot, verb)) {
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

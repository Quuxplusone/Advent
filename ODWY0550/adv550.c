#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "advdat.h"

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

bool pct(int percent) { return (ran(100) < percent); }
bool streq(const char *a, const char *b) { return !strncmp(a, b, 5); }

/*========== Some global variables. =====================================*/

#define MAX_GAME_LENGTH 800
#define MAX_DEATHS 3

bool blob_is_coming;  /* Platt names this variable TICKER */
bool nomagic;
bool panicked;
bool wizard_mode;  /* Platt names this variable OLORIN */
bool lamp_just_ran_out;
bool helped_him;
bool quipped;  /* set when a dwarf dies dramatically */

/* The phases of cave closure are as follows:
 *     0 - cave open and operating normally
 *     1 - cave open, but all treasures are in well-house
 *     2 - "Cave closing soon" message has been given
 *     3 - cave closed; in cylindrical room
 *     4 - escaped from cylindrical room, back outside
 *     5 - has located the treasure room and won the game!
 */
int closure;

bool moved;  /* true if we've moved recently */
VerbWord verbosity_mode = BRIEF;  /* one of FAST, BRIEF, FULL */
bool please_clarify;
int turns;  /* how many times we've read your commands */
int foobar;  /* progress in the FEE FIE FOE FOO incantation */
int backlash = 35;  /* percent chance of dwarf attack */
int fleetfoot = 25;  /* player's modifier to dodge an attack */
int clock, lastclock;
int lamplife = 300;
bool lamp_just_ran_out = false;
int dwarfcount;  /* total number of dwarves in the cave */
int holding_count = 0;  /* Platt calls this INVCT */
int strength = 7;  /* is affected by the mushroom */
int mushtime;
int hint_time;
int death_count;
int penalties;  /* score modifier */
int cylinder_escape_count = 0;  /* Platt calls this ESCAPE */
VerbWord password;  /* for the magic safe */
Location safe_exit;

/*========== Special flags. =============================================*/

bool have_used_noside_samoht = false;
bool dwarf_warning_shot = false;
bool dwarves_enraged = false;
bool pirate_stalking_you = false;
bool djinn_is_grateful = false;
bool djinn_gave_hint = false;
bool successfully_opened_safe = false;
bool have_drowned_in_quicksand = false;
bool have_stolen_back_eggs = false;
bool have_awoken_goblins = false;
bool gave_up;

/*========== Some common messages. ======================================*/

const char ok[] = "OK.";
void hah() { puts("Don't be ridiculous!"); }
void I_see_no(const char *s) { printf("I see no %s here.\n", s); }
void you_have_it() { puts("You are already carrying it!"); }

/*========== Some forward declarations. =================================*/

void phog(Location loc);
void hint_logic(Location loc);

/*========== The vocabulary. ============================================*/

struct HashEntry {
    const char *text;
    WordType word_type;
    int meaning;
};

#define HASH_PRIME 1009
struct HashEntry hash_table[HASH_PRIME];

void new_word(WordType t, const char *w, int m)
{
    int h = 0;
    for (const char *p = w; *p != '\0' && p != w+5; ++p) {
        h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].word_type != WordType_None)
        h = (h+1) % HASH_PRIME;
    hash_table[h].text = w;
    hash_table[h].word_type = t;
    hash_table[h].meaning = m;
}

void new_verb(const char *w, VerbWord m) { new_word(WordType_Verb, w, m); }
void new_noun(const char *w, ObjectWord m) { new_word(WordType_Object, w, m); }
void new_place_word(const char *w, Location m) { new_word(WordType_Place, w, m); }

struct HashEntry *lookup(const char *w)
{
    int h = 0;
    for (const char *p = w; *p != '\0' && p != w+5; ++p) {
        h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].word_type != WordType_None) {
        if (streq(w, hash_table[h].text)) return &hash_table[h];
        h = (h+1) % HASH_PRIME;
    }
    return NULL;
}

void build_vocabulary(void)
{
    new_verb("say", SAY); new_verb("chant", SAY); new_verb("sing", SAY);
    new_verb("utter", SAY); new_verb("mumble", SAY);
    new_verb("save", SAVE); new_verb("suspend", SAVE); new_verb("pause", SAVE);
    new_verb("restore", RESTORE);
    new_verb("upstream", UPSTREAM);
    new_verb("downstream", DOWNSTREAM);
    new_verb("forward", FORWARD); new_verb("continue", FORWARD);
    new_verb("onward", FORWARD);
    new_verb("back", BACK); new_verb("return", BACK);
    new_verb("retreat", BACK);
    new_verb("out", OUT); new_verb("outside", OUT);
    new_verb("exit", OUT); new_verb("leave", OUT);
    new_verb("entrance", ENTRANCE);
    new_verb("gully", GULLY);
    new_verb("stream", STREAM);
    new_verb("rock", ROCK); new_verb("rocks", ROCK);
    new_verb("bed", BED); new_verb("streambed", BED);
    new_verb("crawl", CRAWL); new_verb("crawls", CRAWL);
    new_verb("in", IN); new_verb("inward", IN);
    new_verb("inside", IN); new_verb("enter", IN);
    new_verb("surface", SURFACE);
    new_verb("null", NOWHERE); new_verb("nowhere", NOWHERE);
    new_verb("passage", PASSAGE); new_verb("tunnel", PASSAGE);
    new_verb("view", VIEW);
    new_verb("hole", HOLE);
    new_verb("stairs", STAIRS);
    new_verb("up", UP); new_verb("upward", UP); new_verb("u", UP);
    new_verb("above", UP); new_verb("ascend", UP);
    new_verb("down", DOWN); new_verb("d", DOWN);
    new_verb("downwards", DOWN); new_verb("descend", DOWN);
    new_verb("outdoors", OUTDOORS);
    new_verb("crack", CRACK);
    new_verb("dome", DOME);
    new_verb("left", LEFT);
    new_verb("right", RIGHT);
    new_verb("hall", HALL);
    new_verb("jump", JUMP);
    new_verb("cross", CROSS); new_verb("across", CROSS);
    new_verb("over", CROSS);
    new_verb("east", EAST); new_verb("e", EAST);
    new_verb("west", WEST); new_verb("w", WEST);
    new_verb("north", NORTH); new_verb("n", NORTH);
    new_verb("south", SOUTH); new_verb("s", SOUTH);
    new_verb("northeast", NE); new_verb("ne", NE);
    new_verb("southeast", SE); new_verb("se", SE);
    new_verb("southwest", SW); new_verb("sw", SW);
    new_verb("northwest", NW); new_verb("nw", NW);
    new_verb("secret", SECRET);
    new_verb("climb", CLIMB);
    new_verb("look", LOOK); new_verb("examine", LOOK);
    new_verb("touch", LOOK); new_verb("describe", LOOK);
    new_verb("get", GET); new_verb("carry", GET); new_verb("take", GET);
    new_verb("keep", GET); new_verb("catch", GET); new_verb("steal", GET);
    new_verb("capture", GET); new_verb("tote", GET);
    new_verb("drop", DROP); new_verb("release", DROP); new_verb("free", DROP);
    new_verb("discard", DROP); new_verb("dump", DROP);
    new_verb("open", OPEN); new_verb("unlock", OPEN);
    new_verb("close", CLOSE); new_verb("lock", CLOSE);
    new_verb("on", ON); new_verb("light", ON);
    new_verb("off", OFF); new_verb("extinguish", OFF);
    new_verb("wave", WAVE); new_verb("shake", WAVE); new_verb("swing", WAVE);
    new_verb("placate", PLACATE); new_verb("calm", PLACATE);
    new_verb("tame", PLACATE);
    new_verb("walk", WALK); new_verb("run", WALK); new_verb("travel", WALK);
    new_verb("go", WALK); new_verb("proceed", WALK); new_verb("explore", WALK);
    new_verb("goto", WALK); new_verb("follow", WALK); new_verb("turn", WALK);
    new_verb("kill", KILL); new_verb("attack", KILL); new_verb("fight", KILL);
    new_verb("hit", KILL); new_verb("strike", KILL); new_verb("slay", KILL);
    new_verb("pour", POUR);
    new_verb("eat", EAT); new_verb("devour", EAT);
    new_verb("drink", DRINK);
    new_verb("rub", RUB);
    new_verb("throw", THROW); new_verb("toss", THROW);
    new_verb("quit", QUIT); new_verb("stop", QUIT); new_verb("q", QUIT);
    new_verb("find", FIND); new_verb("where", FIND);
    new_verb("inventory", INVENTORY); new_verb("inv", INVENTORY);
    new_verb("invent", INVENTORY);
    new_verb("feed", FEED);
    new_verb("fill", FILL);
    new_verb("score", SCORE);
    new_verb("brief", BRIEF);
    new_verb("read", READ); new_verb("peruse", READ);
    new_verb("break", BREAK); new_verb("shatter", BREAK);
    new_verb("smash", BREAK);
    new_verb("ride", RIDE);
    new_verb("hours", HOURS);
    new_verb("fum", FUM);
    new_verb("sesame", SESAME); new_verb("opensesame", SESAME);
    new_verb("abra", SESAME); new_verb("abracadabra", SESAME);
    new_verb("shazam", SESAME); new_verb("hocus", SESAME);
    new_verb("pocus", SESAME); new_verb("cadabra", SESAME);
    new_verb("help", HELP); new_verb("?", HELP);
    new_verb("tree", TREES); new_verb("trees", TREES);
    new_verb("dig", DIG); new_verb("excavate", DIG);
    new_verb("lost", LOST);
    new_verb("mist", MIST);
    new_verb("info", INFO); new_verb("information", INFO);
    new_verb("swim", SWIM);
    new_verb("lpsd", LPSD);
    new_verb("fast", FAST);
    new_verb("full", FULL);
    new_verb("news", NEWS);
    new_verb("cave", CAVE);
    new_verb("fix", FIX); new_verb("repair", FIX);
    new_verb("wizard", WIZARD);
    new_verb("zorton", ZORTON);
    new_verb("xyzzy", XYZZY);
    new_verb("thurb", THURB);
    new_verb("snoeze", SNOEZE);
    new_verb("samoht", SAMOHT);
    new_verb("plugh", PLUGH);
    new_verb("phuggg", PHUGGG);
    new_verb("noside", NOSIDE);
    new_verb("melenkurion", MELENKURION);
    new_verb("knerl", KNERL);
    new_verb("klaetu", KLAETU);
    new_verb("foo", FOO);
    new_verb("foe", FOE);
    new_verb("fie", FIE);
    new_verb("fee", FEE);
    new_verb("blerbi", BLERBI);
    new_verb("curious", CURIOUS);
    new_verb("green", GREEN);
    new_verb("ideas", IDEAS);
    new_verb("slept", SLEPT);
    new_verb("furiously", FURIOUSLY);
    new_verb("runout", RUNOUT);

    new_noun("grate", GRATE);
    new_noun("steps", STEPS);
    new_noun("door", DOOR);
    new_noun("snake", SNAKE);
    new_noun("fissure", FISSURE);
    new_noun("tablet", TABLET);
    new_noun("mirror", MIRROR);
    new_noun("plant", PLANT); new_noun("beanstalk", PLANT);
    new_noun("stalactite", STALACTITE);
    new_noun("fog", FOG);
    new_noun("glow", GLOW);
    new_noun("shadow", SHADOW); new_noun("figure", SHADOW);
    new_noun("blob", BLOB);
    new_noun("drawing", DRAWINGS);
    new_noun("pirate", PIRATE);
    new_noun("dragon", DRAGON);
    new_noun("chasm", CHASM);
    new_noun("troll", TROLL);
    new_noun("ogre", OGRE);
    new_noun("basilisk", BASILISK);
    new_noun("gong", GONG);
    new_noun("djinn", DJINN); new_noun("djann", DJINN);
    new_noun("genie", DJINN);
    new_noun("turtle", TURTLE); new_noun("tortoise", TURTLE);
    new_noun("darwin", TURTLE);
    new_noun("message", MESSAGE);
    new_noun("volca", GEYSER); new_noun("geyse", GEYSER); new_noun("gorge", GEYSER);
    new_noun("statue", STATUE); new_noun("minotaur", STATUE);
    new_noun("quicksand", QUICKSAND);
    new_noun("slime", SLIME);
    new_noun("vending", PONY); new_noun("machine", PONY);
    new_noun("safe", SAFE);
    new_noun("throne", THRONE);
    new_noun("skeleton", SKELETON);
    new_noun("bear", BEAR);
    new_noun("batteries", BATTERIES);
    new_noun("moss", MOSS); new_noun("carpet", MOSS); new_noun("curtains", MOSS);
    new_noun("dinghy", DINGHY);
    new_noun("bag", BAG); new_noun("pieces", BAG); new_noun("eight", BAG);
    new_noun("crown", CROWN);
    new_noun("gold", GOLD); new_noun("nugget", GOLD);
    new_noun("diamonds", DIAMONDS);
    new_noun("silver", SILVER); new_noun("bars", SILVER);
    new_noun("jewelry", JEWELS); new_noun("jewels", JEWELS);
    new_noun("coins", COINS);
    new_noun("chest", CHEST); new_noun("box", CHEST);
    new_noun("treasure", CHEST);
    new_noun("eggs", EGGS); new_noun("egg", EGGS);
    new_noun("nest", EGGS);
    new_noun("trident", TRIDENT);
    new_noun("helmet", HELMET);
    new_noun("ming", VASE); new_noun("vase", VASE);
    new_noun("pottery", SHARDS); new_noun("shards", SHARDS);
    new_noun("emerald", EMERALD);
    new_noun("sceptre", SCEPTRE);
    new_noun("yacht", YACHT); new_noun("ruby", YACHT);
    new_noun("rubaiyat", YACHT);
    new_noun("platinum", PYRAMID); new_noun("pyramid", PYRAMID);
    new_noun("pearl", PEARL);
    new_noun("persian", RUG); new_noun("rug", RUG);
    new_noun("spices", SPICES);
    new_noun("beads", BEADS); new_noun("indian", BEADS);
    new_noun("string", BEADS); new_noun("turquoise", BEADS);
    new_noun("chain", CHAIN);
    new_noun("ring", RING); new_noun("mithril", RING);
    new_noun("spyglass", SPYGLASS); new_noun("scrimshaw", SPYGLASS);
    new_noun("sculpture", SCULPTURE); new_noun("crystal", SCULPTURE);
    new_noun("bracelet", BRACELET); new_noun("jade", BRACELET);
    new_noun("casket", CASKET); new_noun("opals", CASKET);
    new_noun("key", KEYS); new_noun("keys", KEYS); new_noun("set", KEYS);
    new_noun("lamp", LAMP); new_noun("lantern", LAMP);
    new_noun("cage", CAGE); new_noun("wicker", CAGE);
    new_noun("bird", BIRD);
    new_noun("rod", ROD);
    new_noun("plate", PLATE);
    new_noun("dynamite", DYNAMITE);
    new_noun("pillow", PILLOW); new_noun("velvet", PILLOW);
    new_noun("clam", CLAM);
    new_noun("oyster", OYSTER);
    new_noun("magazines", MAG); new_noun("issues", MAG);
    new_noun("spelunker", MAG); new_noun("\"spelunker\"", MAG);
    new_noun("knife", KNIFE); new_noun("knives", KNIFE);
    new_noun("food", FOOD); new_noun("rations", FOOD);
    new_noun("bottle", BOTTLE); new_noun("jar", BOTTLE);
    new_noun("water", WATER); new_noun("h2o", WATER);
    new_noun("oil", OIL);
    new_noun("flask", FLASK);
    new_noun("axe", AXE);
    new_noun("sword", SWORD);
    new_noun("teeth", TEETH);
    new_noun("vial", VIAL); new_noun("phial", VIAL);
    new_noun("mushroom", MUSHROOM);
    new_noun("goblins", GOBLINS);
    new_noun("dwarf", DWARF); new_noun("dwarves", DWARF);
}

/*========== Locations. =================================================*/

void build_place_names(void)
{
    /* Platt lists HERE,THERE,ARG1,ARG2,STATUS as synonymous placenames.
     * I think this may have been a typo, or a limitation of his system;
     * I don't bother with them here. */
    new_place_word("road", R_ROAD);
    new_place_word("hill", R_HILL);
    new_place_word("building", R_HOUSE); new_place_word("house", R_HOUSE);
    new_place_word("valley", R_VALLEY);
    new_place_word("forest", R_FOREST);
    new_place_word("slit", R_SLIT);
    new_place_word("depression", R_DEPRESSION);
    new_place_word("cobbles", R_COBBLES);
    new_place_word("debris", R_DEBRIS);
    new_place_word("canyon", R_AWK);
    new_place_word("pit", R_SPIT);
    new_place_word("mists", R_EMIST);
    new_place_word("mtking", R_HMK);
    new_place_word("y2", R_Y2);
    new_place_word("jumble", R_JUMBLE); new_place_word("wall", R_JUMBLE);
    new_place_word("broken", R_JUMBLE);
    new_place_word("window", R_WINDOW);
    new_place_word("dirty", R_DIRTY);
    new_place_word("brink", R_CLEAN);
    new_place_word("dusty", R_DUSTY);
    new_place_word("crossover", R_CROSS);
    new_place_word("complex", R_COMPLEX);
    new_place_word("bedquilt", R_BEDQUILT);
    new_place_word("swiss", R_SWISS);
    new_place_word("slab", R_SLAB);
    new_place_word("low", R_LOW);
    new_place_word("incline", R_INCLINE);
    new_place_word("giant", R_GIANT);
    new_place_word("cavern", R_FALLS);
    new_place_word("soft", R_SOFT);
    new_place_word("oriental", R_ORIENTAL);
    new_place_word("misty", R_MISTY);
    new_place_word("alcove", R_ALCOVE);
    new_place_word("plover", R_PLOVER);
    new_place_word("dark", R_DARK);
    new_place_word("arched", R_ARCHED);
    new_place_word("shell", R_SHELL);
    new_place_word("anteroom", R_ANTE);
    new_place_word("reservoir", R_RES);
    new_place_word("sloping", R_SLOPING);
    new_place_word("corridor", R_CORRIDOR);
    new_place_word("fork", R_FORK);
    new_place_word("boulders", R_CHAMBER);
    new_place_word("limestone", R_LIME);
    new_place_word("barren", R_FBARR);
    new_place_word("limbo", R_LIMBO);  /* Odd. Platt has this. */
    new_place_word("lair", R_LAIR);
    new_place_word("ice", R_ICE);
    new_place_word("glassy", R_GLASSY);
    new_place_word("tool", R_TOOL);
    new_place_word("morion", R_MORION);
    new_place_word("vault", R_VAULT);
    new_place_word("cubicle", R_CUBICLE);
    new_place_word("spherical", R_SPHERICAL);
    new_place_word("slide", R_SLIDE);
    new_place_word("shelf", R_SHELF);
    new_place_word("beach", R_BEACH);
    new_place_word("fourier", R_FOURIER);
    new_place_word("jonah", R_JONAH);
    new_place_word("faces", R_FACES);
    new_place_word("nondescript", R_NONDESCRIPT);
    new_place_word("pentagram", R_PENTAGRAM);
    new_place_word("peelgrunt", R_PEELGRUNT);
    new_place_word("storage", R_STORAGE);
    new_place_word("audience", R_AUDIENCE);
    new_place_word("arabesque", R_ARABESQUE);
    new_place_word("translucent", R_TRANSLUCENT);
    new_place_word("insafe", R_INSAFE);
    new_place_word("platform", R_PLATFORM);
    new_place_word("sandstone", R_SANDSTONE);
    new_place_word("chimney", R_CHIMNEY);
    new_place_word("tube", R_TUBE);
    new_place_word("warm", R_WARM);
    new_place_word("balcony", R_BALCONY);
    new_place_word("cylindrical", R_CYLINDRICAL);
}

/*========== Data structures for objects. ===============================*/

int holding_count;  /* how many objects are being carried? */

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
    return toting(t) || there(t, loc);
}

void apport(ObjectWord t, Location loc)
{
    objs[t].place = loc;
    objs[t].place2 = loc;
    if (!(objs[t].flags & F_SCHIZOID)) return;
    if (loc == R_LIMBO || loc == R_YLEM) return;
    objs[t].place2 = loc+1;  /* Schizoid objects are in two places at once. */
}

void build_object_table(void)
{
    objs[GRATE].desc[0] = "The grate is locked.";
    objs[GRATE].desc[1] = "The grate is open.";
    objs[DOOR].desc[0] = "The way north is barred by a massive, rusty, iron door.";
    objs[DOOR].desc[0] = "The way north leads through a massive, rusty, iron door.";
    objs[SNAKE].desc[0] = "A huge green fierce snake bars the way!";
    objs[FISSURE].desc[1] = "A crystalline bridge now spans the fissure.";
    objs[FISSURE].desc[2] = "The crystalline bridge has vanished!";
    objs[TABLET].desc[0] = "A massive stone tablet imbedded in the wall reads:\n"
                           "\"Congratulations on bringing light into the Dark room!\"";
    objs[PLANT].desc[0] = "There is a tiny little plant in the pit, murmuring \"Water, water, ...\"";                
    objs[PLANT].desc[1] = "There is a 12-foot-tall beanstalk stretching up out of the pit,\n"
                          "bellowing \"WATER!! WATER!!\"";
    objs[PLANT].desc[2] = "There is a gigantic beanstalk stretching all the way up to the hole.";                

    objs[PLANT2].desc[1] = "The top of a 12-foot-tall beanstalk is poking out of the west pit.";
    objs[PLANT2].desc[2] = "There is a huge beanstalk growing out of the west pit up to the hole.";
    objs[SHADOW].desc[0] = "The shadowy figure seems to be trying to attract your attention.";
    objs[DRAGON].desc[0] = "A huge green fierce dragon bars the way!\n"
                           "The dragon is sprawled out on a Persian rug!!";
    objs[DRAGON].desc[2] = "The body of a huge green dead dragon is lying off to one side.";
    objs[DRAGON].desc[3] = "The rotting carcass of a dead dragon is lying off to one side.";
    objs[CHASM].desc[0] = "A rickety wooden bridge extends across the chasm, vanishing into the\n"
                          "mist.  A sign posted on the bridge reads, \"STOP! Pay troll!\"";
    objs[CHASM].desc[1] = "The wreckage of a bridge (and a dead bear) can be seen at the bottom\n"
                          "of the chasm.";
    objs[CHASM].desc[2] = "The charred remains of a wooden bridge can be seen at the bottom\n"
                          "of the chasm.";
    objs[TROLL].desc[0] = "A burly troll stands by the bridge and insists you throw him a\n"
                          "treasure before you may cross.";
    objs[TROLL].desc[3] = "The troll steps out from beneath the bridge and blocks your way.";

    objs[TROLL2].desc[0] = "The troll is nowhere to be seen.";
    objs[OGRE].desc[0] = "There is a large, nasty-looking ogre blocking your path!";
    objs[BASILISK].desc[0] = "There is a basilisk lying in the corridor to the north, snoring\n"
                             "quietly.";
    objs[BASILISK].desc[1] = "There is a basilisk lying in the corridor to the south.  It is\n"
                             "asleep, but is twitching and grumbling as if restless.";
    objs[BASILISK].desc[2] = "There is a petrified basilisk in the corridor to the north.";
    objs[BASILISK].desc[3] = "There is a petrified basilisk in the corridor to the south.";
    objs[GONG].desc[0] = "There is a large brass gong fastened to the wall here.";
    objs[DJINN].desc[0] = "There is a twelve-foot djinn standing in the center of the pentagram,\n"
                          "glowering at you.";
    objs[TURTLE].desc[0] = "Darwin the tortoise is swimming in the reservoir nearby.";
    objs[MESSAGE].desc[0] = "There is a message scrawled in the dust in a flowery script, reading:\n"
                            "\"This is not the maze where the pirate leaves his treasure chest.\"";
    objs[GEYSER].desc[2] = "There is a wheat-colored stone bridge arching over the gorge.";
    objs[STATUE].desc[1] = "Dark tunnels lead northeast, north, and northwest.";
    objs[SLIME].desc[0] = "The passage to the south is swathed with sheets of evil-looking\n"
                          "green slime, which twitches and flows as if aware of your presence.";
    objs[PONY].desc[0] = "There is a massive vending machine here. The instructions on it read:\n"
                         "\"Drop coins here to receive fresh batteries.\"";
    objs[SAFE].desc[0] = "A massive walk-in safe takes up one entire wall.  It is tightly\n"
         "closed, and has no handle, lock, nor keyhole.";
    objs[SAFE].desc[1] = "A massive walk-in safe takes up one entire wall.  Its door has been\n"
         "swung open and blocks the exit passage.";
    objs[SAFE].desc[2] = "A massive walk-in safe takes up one entire wall.  It is closed, and\n"
         "there are signs of melting around the edges of its door.";
    objs[SKELETON].desc[0] = "Resting on the throne (\"sitting\" isn't really the right word) is an\n"
         "incredible skeleton.  It is fairly humanoid from the waist up (except\n"
         "for its incredible size and four extra arms);  below that, it resembles\n"
         "the body of a giant python, and is wrapped in and around the bars and\n"
         "rods of the throne.  Clutched in one bony hand is a long sceptre,\n"
         "ornately encrusted with sapphires!!";
    objs[BEAR].desc[0] = "There is a ferocious cave bear eying you from the far end of the room!";
    objs[BEAR].desc[1] = "There is a gentle cave bear sitting placidly in one corner.";
    objs[BEAR].desc[2] = "There is a contented-looking bear wandering about nearby.";
    objs[BATTERIES].desc[0] = "There are fresh batteries here.";
    objs[BATTERIES].desc[1] = "Some worn-out batteries have been discarded nearby.";
    objs[DINGHY].desc[0] = "Lying upon the beach are the shattered remains of what must once have\n"
                           "been a dinghy.  The remains consist of little more than a few broken\n"
                           "boards, upon one of which may be seen a crude sketch of a skull and two\n"
                           "crossed thighbones (perhaps this dinghy was once owned by a cook?)";
    objs[DINGHY].desc[1] = "The shattered remains of a dinghy lie forlornly on the beach.";
    objs[BAG].desc[0] = "There is a bag (obviously filled with pieces of eight) in the dinghy!";
    objs[BAG].desc[1] = "There is a bag filled with pieces of eight lying off to one side!";
    objs[CROWN].desc[0] = "There is a massive crown made of solid iridium floating in midair!";
    objs[CROWN].desc[1] = "There is a massive iridium crown here!";
    objs[GOLD].desc[0] = "There is a large sparkling nugget of gold here!";
    objs[DIAMONDS].desc[0] = "There are diamonds here!";
    objs[SILVER].desc[0] = "There are bars of silver here!";
    objs[JEWELS].desc[0] = "There is precious jewelry here!";
    objs[COINS].desc[0] = "There are many coins here!";
    objs[CHEST].desc[0] = "The pirate's treasure chest is here, half-hidden behind a rock!";
    objs[CHEST].desc[1] = "The pirate's treasure chest is here!";
    objs[EGGS].desc[0] = "There is a large nest here, full of golden eggs!";
    objs[TRIDENT].desc[0] = "There is a jewel-encrusted trident here!";
    objs[HELMET].desc[0] = "There is a gem-encrusted visorless helmet sitting on the floor!";
    objs[VASE].desc[0] = "There is a delicate, precious, Ming vase here!";
    objs[SHARDS].desc[0] = "The floor is covered with worthless shards of pottery.";
    objs[EMERALD].desc[0] = "There is an emerald here the size of a plover's egg!";
    objs[SCEPTRE].desc[1] = "A sapphire-encrusted sceptre is lying on the ground!";
    objs[YACHT].desc[0] = "There is a small toy yacht sitting on the floor. It is totally\n"
                          "covered with rubies, and has the words \"Omar Khayyam\" engraved\n"
                          "on the side!!";
    objs[YACHT].desc[1] = "The ruby yacht of Omar Khayyam is sitting on the floor!";
    objs[PYRAMID].desc[0] = "There is a platinum pyramid here, 8 inches on a side!";
    objs[PEARL].desc[0] = "Off to one side lies a glistening pearl!";
    objs[RUG].desc[0] = "There is a Persian rug spread out on the floor!";
    objs[SPICES].desc[0] = "There are rare spices here!";
    objs[BEADS].desc[0] = "There is a string of ancient Indian turquoise beads draped casually\n"
                          "over the edge of the balcony!";
    objs[CHAIN].desc[0] = "There is a golden chain lying in a heap on the floor!";
    objs[CHAIN].desc[1] = "The bear is locked to the wall with a golden chain!";
    objs[CHAIN].desc[2] = "There is a golden chain locked to the wall!";
    objs[RING].desc[0] = "There is a shiny ring (crafted of the finest mithril) lying here!";
    objs[SPYGLASS].desc[0] = "There is a small spyglass carved out of whale baleen sitting here!";
    objs[BRACELET].desc[0] = "There is an ancient Chinese jade bracelet here!";
    objs[CASKET].desc[0] = "There is a casket full of rare black opals sitting on a shelf!";
    objs[CASKET].desc[1] = "There is a casket full of rare black opals here!";
    objs[KEYS].desc[0] = "There are some keys on the ground here.";
    objs[LAMP].desc[0] = "There is a shiny brass lamp nearby.";
    objs[LAMP].desc[1] = "There is a lamp shining nearby.";
    objs[CAGE].desc[0] = "There is a small wicker cage discarded nearby.";
    objs[CAGE].desc[1] = "There is a small wicker cage nearby.";
    objs[BIRD].desc[0] = "A cheerful little bird is sitting here singing.";
    objs[BIRD].desc[1] = "There is a little bird in the cage.";
    objs[ROD].desc[0] = "A three foot black rod with a rusty star on an end lies nearby.";
    objs[PLATE].desc[0] = "A highly polished metal plate is leaning against the wall.";
    objs[PLATE].desc[1] = "A polished metal plate lies nearby.";
    objs[DYNAMITE].desc[0] = "A three foot black rod with a rusty mark on an end lies nearby.";
    objs[PILLOW].desc[0] = "A small velvet pillow lies on the floor.";
    objs[CLAM].desc[0] = "There is an enormous clam here with its shell tightly closed.";
    objs[OYSTER].desc[0] = "There is an enormous oyster here with its shell tightly closed.";
    objs[MAG].desc[0] = "There are a few recent issues of \"Spelunker Today\" magazine here.";
    objs[FOOD].desc[0] = "There is food here.";
    objs[BOTTLE].desc[0] = "There is a bottle of water here.";
    objs[BOTTLE].desc[1] = "There is an empty bottle here.";
    objs[BOTTLE].desc[2] = "There is a bottle of oil here.";
    objs[FLASK].desc[0] = "There is a sealed earthenware flask sitting in the center of the\n"
                          "pentagram.";
    objs[FLASK].desc[1] = "There is a small, tightly-sealed earthenware flask on the ground here.\n"
                          "It has the words, \"London Dry\" written on the side.";
    objs[FLASK].desc[2] = "There is a small, empty earthenware flask here.";
    objs[AXE].desc[0] = "There is a little axe here.";
    objs[AXE].desc[1] = "There is a little axe lying beside the bear.";
    objs[TEETH].desc[0] = "There are several dragon's teeth scattered haphazardly about.";
    objs[TEETH].desc[1] = "There are several dragon's teeth in a pile on the floor.";
    objs[VIAL].desc[0] = "On the ground lies a small glass vial filled with an oily liquid.";
    objs[MUSHROOM].desc[0] = "There is a small mushroom growing on the wall.";
    objs[MUSHROOM].desc[1] = "There is a small mushroom lying on the floor.";
}

void describe_object(ObjectWord t)
{
    if (t == DWARF) {
        switch (objs[DWARF].prop) {
            case 1: puts("There is a threatening little dwarf in the room with you!"); break;
            case 2: puts("There are two bloodthirsty little dwarves in the room with you!"); break;
            case 3: puts("There are three nasty little dwarves in the room with you!"); break;
            case 4: puts("There are four hostile little dwarves in the room with you!"); break;
            case 5: puts("There are five threatening little dwarves in the room with you!"); break;
            case 6: puts("There are six bloodthirsty little dwarves in the room with you!"); break;
            case 7: puts("There are seven nasty, hostile little dwarves in the room with you!"); break;
            case 8: puts("There are eight angry little dwarves in the room with you!"); break;
            case 9: puts("There are nine hostile and nasty little dwarves in the room with you!"); break;
            case 10: puts("There are ten very angry little dwarves in the room with you!"); break;
            default: assert(false);
        }
    } else if (t == GOBLINS) {
        /* TODO: deal with the gooseberry goblins */        
    } else if (t == SWORD) {
        /* TODO: deal with the singing sword */
    } else if (t == SCULPTURE) {
        /* TODO: deal with the rock-crystal sculpture */
    } else if (t == BLOB) {
        /* TODO: deal with Rover */
    } else if (t == FOG) {
        switch (objs[FOG].prop) {
            case 0: puts("You are standing, badly befuddled, in a pale purple fog."); break;
            case 1: puts("You are wandering around in the middle of a bright red fog."); break;
            case 2: puts("You are lost in the midst of a thick, pea-green fog."); break;
            case 3: puts("You are trying to find your way through a dense coal-black fog."); break;
            case 4: puts("You are lost in the heart of a strange yellow fog."); break;
            case 5: puts("You are standing, badly bedazzled, in a day-glow orange fog."); break;
            case 6: puts("You are hunting your way through a shimmering magenta fog."); break;
            case 7: puts("You are somewhere in the center of a weird, pearly pink fog."); break;
            default: assert(false);
        }
    } else if (t == GLOW) {
        /* N.B. - the directions listed here must match the first 8 directions
         * in the VerbWord enumeration. */
        switch (objs[GLOW].prop) {
            case 0: puts("A faint glow of light is visible through the fog to the east."); break;
            case 1: puts("A glimmer of light is visible through the fog in the west."); break;
            case 2: puts("A glow of light is visible through the fog to the north."); break;
            case 3: puts("A faint shimmer of light is visible to the south."); break;
            case 4: puts("A flickering light is visible through the fog in the northeast."); break;
            case 5: puts("A dim light is visible in the southeast."); break;
            case 6: puts("A dim glow of light is visible in the southwest."); break;
            case 7: puts("A dim flickering light is visible through the fog in the northwest."); break;
            default: assert(false);
        }
    } else {
        assert(0 <= objs[t].prop && objs[t].prop <= 3);
        const char *d = objs[t].desc[objs[t].prop];
        if (d != NULL) puts(d);
    }
}

/*========== Input routines. ============================================*/

char buffer[100];
struct InputWordInfo word1, word2;

bool yes(const char *q)
{
    while (true) {
        /* Platt's version actually doesn't print the ">" prompt, but
         * I think it looks better this way. */
        printf("%s\n> ", q); fflush(stdout);
        fgets(buffer, sizeof(buffer), stdin);
        if (*buffer == '\0' || tolower(*buffer) == 'y') {
            /* A blank response is treated as YES.
             * This makes the dragon puzzle a little easier, but whatever. */
            return true;
        } else if (tolower(*buffer) == 'n') {
            return false;
        } else {
            puts("Please answer the question.");
        }
    }
}

void parse(struct InputWordInfo *w)
{
    if (w->text[0] == '\0') {
        w->type = WordType_None;
        w->meaning = 0;
    } else {
        struct HashEntry *h = lookup(w->text);
        if (h == NULL) {
            w->type = WordType_BadWord;
            w->meaning = 0;
        } else {
            w->type = h->word_type;
            w->meaning = h->meaning;
            /* Turn "PLATI" into "platinum", e.g., so we
             * can use it in certain messages. */
            strcpy(w->text, h->text);
        }
    }
}

void listen(void)
{
    static struct InputWordInfo saved_word;
    static char *p = NULL;
    char *q;
    bool got_a_command = false;
    saved_word = word1;

    if (p != NULL && *p == ',') {
        /* GET KEYS, LAMP does the expected thing.
         * GET KEYS, DROP KEYS is an error. */
        for (++p; isspace(*p); ++p) ;
        if (*p != '\0') {
            for (q = word2.text; !isspace(*p) && *p != ','; ++p, ++q) {
                if (q == word2.text+15) break;  /* buffer overflow prevention */
                *q = tolower(*p);
            }
            *q = '\0';
            got_a_command = true;
        }
    }
    
    if (!got_a_command) {
        printf("> "); fflush(stdout);
        fgets(buffer, sizeof buffer, stdin);
        for (p = buffer; isspace(*p); ++p) ;
        if (*p == '\0') {
            word1.text[0] = '\0';
        } else {
            for (q = word1.text; !isspace(*p); ++p, ++q) {
                if (q == word1.text+15) break;  /* buffer overflow prevention */
                *q = tolower(*p);
            }
            *q = '\0';
            for (++p; isspace(*p); ++p) ;
        }
        if (*p == '\0') {
            word2.text[0] = '\0';
        } else {
            for (q = word2.text; !isspace(*p) && *p != ','; ++p, ++q) {
                if (q == word2.text+15) break;  /* buffer overflow prevention */
                *q = tolower(*p);
            }
            *q = '\0';
        }
    }

    parse(&word1);
    parse(&word2);
    if (please_clarify && word2.type != WordType_None) {
        /* If we just asked the user for a verb or noun, we should
         * see if maybe they gave us one. */
        if ((word1.type == WordType_Verb) != (saved_word.type == WordType_Verb))
            word2 = saved_word;
    }
    /* BIRD GET works just as well as GET BIRD.
     * KEYS DROP, GET works just as well as DROP KEYS followed by GET KEYS. */
    if (word1.type != WordType_Verb && word2.type == WordType_Verb) {
        saved_word = word1;
        word1 = word2;
        word2 = saved_word;
    }
}

static bool keyword(WordType type, int meaning)
{
    if (word1.type == type && word1.meaning == meaning) return true;
    if (word2.type == type && word2.meaning == meaning) return true;
    return false;
}

bool keywordv(VerbWord meaning) { return keyword(WordType_Verb, meaning); }
bool keywordo(ObjectWord meaning) { return keyword(WordType_Object, meaning); }
bool keywordp(Location meaning) { return keyword(WordType_Place, meaning); }


/*========== Dwarves. ===================================================*/

bool dwarf_attack(void)
{
    /* The global variable "fleetfoot" determines the ease with which
     * the dwarf can hit the dude --- 0 for just attacked, 50 for running,
     * 25 for neither. */
    const int number_of_dwarves = objs[DWARF].prop;
    const int knives_thrown = ran(number_of_dwarves + 2) - 1;
    if (knives_thrown <= 0) return false;
    const bool plural = (knives_thrown > 1);
    if (!plural) {
        puts("One nasty sharp knife is thrown at you!");
    } else {
        printf("%d nasty sharp knives are thrown at you!\n", knives_thrown);
    }
    fleetfoot += 5*(strength - holding_count);
    if (dwarves_enraged) fleetfoot -= 20;
    fleetfoot += 35;
    fleetfoot /= knives_thrown;
    if (toting(RING)) {
        /* The magic mithril ring will usually defend you. */
        if (pct(80)) {
            switch (ran(4)) {
                case 0:
                    printf("Your magic ring shoots a bolt of lightning that splinters the\n"
                           "kni%s into dust before %s can reach you!\n",
                           plural?"ves":"fe", plural?"they":"it");
                    break;
                case 1:
                    printf("A glowing disk of black fire jumps out of your magic ring and swallows\n"
                           "the hurtling kni%s before %s can harm you!\n",
                           plural?"ves":"fe", plural?"they":"it");
                    break;
                case 2:
                    printf("%s ball%s of searing flame %s out of your magic ring, %s\n"
                           "off of the ground, and %s the kni%s before %s can reach you!\n",
                           plural?"Several":"A", plural?"s":"",
                           plural?"burst":"leaps", plural?"rebound":"bounces",
                           plural?"vaporize":"melts", plural?"ves":"fe",
                           plural?"they":"it");
                    break;
                case 3:
                    printf("Your magic ring emits a blast of some strange magical energy that\n"
                           "knocks the kni%s aside!\n",
                           plural?"ves":"fe");
                    break;
            }
            return false;
        }
    }
    if (dwarf_warning_shot || pct(fleetfoot)) {
        if (knives_thrown == 1) puts("It misses you!");
        else puts("They miss you!");
        dwarf_warning_shot = false;
    } else {
        if (knives_thrown == 1) puts("It gets you!");
        else puts("One of them gets you!");
        return true;  /* you are dead */
    }
    return false;  /* you survived */
}

/*========== Rover. =====================================================*/

bool tick(void)
{
    if (objs[BLOB].prop == 0) return false;
    objs[BLOB].prop += 1;
    if (objs[BLOB].prop == 17) {
        puts("\nThe blob >ROAR<s triumphantly and bounces straight at you with amazing\n"
             "speed, crushing you to the ground and cutting off your air supply\n"
             "with its body.  You quickly suffocate.");
        return true;  /* you are dead */
    }
    /* Each of these messages ends with a blank line. */
    switch (objs[BLOB].prop) {
        case 3: puts("From somewhere in the distance comes an ominous bubbling sound.\n"); break;
        case 4: puts("The bubbling sound grows louder.\n"); break;
        case 5: puts("The bubbling sound ends with a loud >splash<.\n"); break;
        case 6: puts("A hollow, echoing >ROAR< sounds in the distance.\n"); break;
        case 7: break;
        case 8: puts("A strange throbbing sound can be heard in the distance.\n"); break;
        case 9: break;
        case 10: puts("The throbbing sound is growing louder.\n"); break;
        case 11: break;
        case 12: puts("The source of the throbbing sound is approaching quickly.  Another\n"
                      "hollow >ROAR< echoes through the cave.\n"); break;
        case 13: break;
        case 14: puts("There is a loud >ROAR< only a short distance away!!\n"); break;
        case 15: break;
        case 16:
            puts("Into view there bounces a horrible creature!!  Six feet across, it\n"
                 "resembles a large blob of translucent white jelly;  although it looks\n"
                 "massive, it is bouncing lightly up and down as though it were as light\n"
                 "as a feather.  It is emitting a constant throbbing sound, and it\n"
                 ">ROAR<s loudly as it sees you.\n");
            /* We'll apport(BLOB, loc), but only once the player has finished
             * moving to his new location. */
            break;
    }
    return false;  /* you have survived, so far */
}

/*========== Cameos. ====================================================*/

void maybe_do_cameo(Location loc)
{
    /* In one out of every twenty games, there is the possibility of a
     * "cameo appearance" by some freakish character or other. We decide
     * at the start of the game whether anyone's going to show up, and
     * if so, exactly when --- it'll be sometime between 100 and 500 turns
     * into the game.
     *   "Acceptable locations" for cameo appearances must meet all
     * of the criteria for a pirate appearance (dwarves allowed, no
     * natural light, lamp on), but in addition you can't have any
     * dwarves, pirate, dragon, troll, snake, or quicksand present.
     *   If the first admin clock tick of the game takes place before
     * "cameo_time", and you are in an acceptable location when the tick
     * takes place, then you'll see a random cameo. Otherwise (if you
     * take too long to enter the cave or if you are not in an acceptable
     * location when the tick happens), "cameo_time" is cleared and no
     * cameo will appear in *this* game either.
     *   In short: Cameos are ridiculously rare. Having grown up with David
     * Malmberg's "Humongous Cave", in which one of these seven cameos
     * happens on 3 out of every 100 turns, I give a "heart-warming cheer"
     * for Dave Platt's remarkable restraint.
     */
    static bool no_more_cameos = false;
    if (no_more_cameos) return;
    
    no_more_cameos = true;
    if (pct(95)) return;
    if (turns >= 100+ran(400)) return;
    
    clock = 10+ran(10);
    lastclock = clock = 10+ran(10);
    if (places[loc].flags & (F_NODWARF | F_LIGHTED | F_ONE_EXIT)) return;
    if (!toting(LAMP) || !objs[LAMP].prop) return;
    if (pirate_stalking_you) return;
    if (there(DWARF, loc) || there(DRAGON, loc) || there(TROLL, loc)) return;
    if (there(SNAKE, loc) || there(QUICKSAND, loc)) return;
    puts("");
    switch (ran(5)) {
        case 0:
            /* Platt has a knack for these sentences that don't quite parse
             * on the first reading: a skirl of light? Perhaps this is a nod
             * to Crowther's "axe which missed, cursed and ran away." */
            puts("From somewhere in the distance, there comes a musical skirl of\n"
                 "light, elvish laughter and the sounds of merriment.\n");
            break;
        case 1:
            puts("From somewhere nearby, there suddenly comes a sound of something\n"
                 "mechanical in motion.  As you turn towards it, an incredible\n"
                 "figure rolls into the light of your lamp.  It stands about five\n"
                 "feet high on a wheeled metal pedestal, and has a globular light-\n"
                 "filled head, accordion-pleated metal arms, and a cylindrical body\n"
                 "the size of an oil drum with a plastic panel on the front.  It rolls\n"
                 "past without taking any notice of you, all the while waving its\n"
                 "arms, flashing a light behind its front panel and bellowing \"WARNING!\n"
                 "WARNING!  DANGER!\" at the top of its not inconsiderable voice.  It\n"
                 "rolls on out of sight, and moments later there is an immense flash\n"
                 "of light and a tremendous blast of sparks and smoke.  When the air\n"
                 "clears, you find that no trace remains of the strange apparition.\n");
            break;
        case 2:
            puts("With a sudden gust of air, a large cave bat flutters into view,\n"
                 "flies around your head several times, squeaks with disgust, and\n"
                 "flutters on out of sight.\n");
            break;
        case 3:
            puts("Suddenly, the ground quivers underfoot;  a dull rumbling sound\n"
                 "resounds from the rock around you, and in the distance you can\n"
                 "hear the crash of falling rock.  The earth tremor subsides after\n"
                 "a few seconds without causing any major damage.\n");
            break;
        case 4:
            puts("From the darkness nearby comes the sound of shuffling feet.  As you\n"
                "turn towards the sound, a nine-foot cyclops ambles into the light of\n"
                "your lamp.  The cyclops is dressed in a three-piece suit of worsted\n"
                "wool, and is wearing a black silk top-hat and cowboy boots and is\n"
                "carrying an ebony walking-stick.  It catches sight of you and stops,\n"
                "seeming frozen in its tracks, with its bloodshot eye bulging in\n"
                "amazement and its fang-filled jaw drooping with shock.  After staring\n"
                "at you in incredulous disbelief for a few moments, it reaches into\n"
                "the pocket of its vest and pulls out a small plastic bag filled with\n"
                "a leafy green substance, and examines it carefully.  \"It must be\n"
                /* I added the first comma on the line below. */
                "worth eighty pazools an ounce after all,\" mumbles the cyclops, who\n"
                "casts one final look at you, shudders, and staggers away out of\n"
                "sight.\n");
        case 5:
            puts("From somewhere in the distance comes a heart-wrenching scream of\n"
                 "mortal terror!  \"NO!  DON'T!  NO!  NO!  HELP!!!!\" cries the voice,\n"
                 "and then lets out a wail of agony that is cut off abruptly.  Subdued\n"
                 "crunching and slurping sounds can be heard for a minute or so, and\n"
                 "then silence falls.\n");
            break;
        case 6:
            puts("From somewhere nearby come the sounds of sliding, stumbling feet.\n"
                 "As you turn towards them, the beam of your lamp falls upon a tall,\n"
                 "shambling figure approaching you out of the darkness.  Standing no\n"
                 "more than five feet tall, it cannot possibly weigh more than fifty\n"
                 "pounds including the shroud and bandages in which it is wrapped;\n"
                 "a musty reek like the scent of old, dead earth seeps from it and\n"
                 "fills the air.  As you cower back in disgust and horror, the figure\n"
                 "halts, examines you through eyes resembling wet pebbles, and\n"
                 "whispers \"Peace, man!\" in a voice like wind rustling through dead\n"
                 "trees.  It then turns and shambles away into the darkness.\n");
            break;
    }
}

/*========== Hints. =====================================================*/

struct Hint {
    const char *prompt;
    const char *spoiler;
    bool given;
} hints[] = { {
    "Are you trying to get into the cave?",
        "The grate is very solid and has a hardened steel lock.  You cannot\n"
        "enter without a key, and there are no keys nearby.  I would recommend\n"
        "looking elsewhere for the keys.", false
    }, {
    "Are you trying to catch the bird?",
        "The bird is frightened right now and you cannot catch it no matter\n"
        "what you try.  Perhaps you might try later.", false
    }, {
    "Are you trying to somehow deal with the snake?",
        "You can't kill the snake, or drive it away, or avoid it, or anything\n"
        "like that.  There is a way to get by, but you don't have the necessary\n"
        "resources right now.", false
    }, {
    "Do you need help getting out of here?",
        "Don't go west.", false
    }, {
    "Are you trying to explore beyond the Plover room?",
        /* Platt's help text is less subtle than Woods'. */
        "You will definitely need a source of light to explore that region, and\n"
        "it is impossible to get the lamp into the Plover room through the\n"
        "narrow tunnel entering from the alcove.", false
    }, {
    "Having problems?",
        "Ok - what you need to do is apply a little philosophy.  To wit:  there\n"
        "is a question that you need to ask whenever you explore a new room\n"
        "in this cave.  In most places, the answer to the question is \"yes\".\n"
        "In some other places, it's \"no\" for an obvious reason.  Right here,\n"
        "the answer is \"no\" but the reason isn't so obvious.  If you can figure\n"
        "out what the question is, you can get out of here easily.  I can tell\n"
        "you this - it's always a vital question if you wish to survive.", false
        /* The question is something like "Should I turn on my lantern?" */
    }, {
    "Do you need help getting out of here?",
        /* Platt's help text is less subtle than Woods'. */
        "You can make the passages look less alike by dropping things.  You\n"
        "could then make a map that would let you find your way around.", false
    }, {
    "Are you having problems getting out of the ice tunnels?",
        /* Platt's help text is not subtle at all. But the puzzle in question
         * is ingenious. */
        "To get out of here, you'll first have to get your bearings so that\n"
        "you know where you are.  I suggest that you draw a careful, accurate\n"
        "map of the tunnel system;  for clarity's sake, keep your lines as\n"
        "straight as is feasible and draw in all of the dead ends and such.\n"
        "Once you've got a complete and accurate map, examine it carefully; if\n"
        "your thoughts refuse to clarify, you might try using the old Yoga\n"
        "trick of standing on your head, and see if that helps.", false
    }
};

void hint_logic(Location loc)
{
    int which_hint = -1;
    if (loc == R_DEPRESSION && !objs[GRATE].prop && !toting(KEYS)) {
        which_hint = 0;
    } else if (loc == R_BIRD && there(BIRD, R_BIRD) && !objs[BIRD].prop && toting(ROD)) {
        which_hint = 1;
    } else if (loc == R_HMK && there(SNAKE, R_HMK) && !here(BIRD, R_HMK)) {
        which_hint = 2;
    } else if (loc == R_WITT) {
        which_hint = 3;
    } else if ((loc == R_PLOVER || loc == R_ALCOVE || loc == R_DARK) && !(places[R_DARK].flags & F_BEENHERE)) {
        which_hint = 4;
    } else if (loc == R_PLAIN_2) {
        which_hint = 5;
    } else if (places[loc].flags & F_INMAZE) {
        which_hint = 6;
    } else if (R_SLIDE <= loc && loc <= R_ICECAVE30) {
        which_hint = 7;
    }
    if (which_hint == -1) return;
    if (hints[which_hint].given) return;
    hint_time = 0;
    if (yes(hints[which_hint].prompt)) {
        puts("I can give you some advice that might help you solve your problem,\n"
             "but I'll have to charge you 20 points for it.  TANSTAAFL, y'know!");
        if (yes("Do you want the hint?")) {
            puts(hints[which_hint].spoiler);
            penalties += 20;
            hints[which_hint].given = true;
        }
    }
}

/*========== Scoring. ===================================================*/

int score(void)
{
    int s = 0;
    if (!gave_up || closure > 2) s += 9;
    for (int i = MIN_TREASURE; i <= MAX_OBJ; ++i) {
        /* Each treasure is worth a flat 15 points. */
        if (there(i, R_HOUSE) || closure > 2)
            s += 15;
        else if (objs[i].flags & F_SEEN)
            s += 2;
    }
    if (there(MAG, R_WITT)) s += 1;
    if ((places[R_DEBRIS].flags | places[R_Y2].flags) & F_BEENHERE) {
        /* 20 points for getting inside the cave. */
        s += 20;
        /* Bonuses for seeing the new stuff in Adventure 550. */
        if (places[R_LAIR].flags & F_BEENHERE) s += 10;
        if (places[R_BEACH].flags & F_BEENHERE) s += 10;
        if (places[R_FACES].flags & F_BEENHERE) s += 10;
    }
    s += 20*closure;
    s -= 10*death_count;
    s -= penalties;
    return (s > 0) ? s : 0;
}

#define HIGHEST_CLASS 8
static int class_score[] = { 35, 100, 130, 200, 250, 300, 330, 349, 9999 };
static const char *class_message[] = {
    "You are obviously a rank amateur.  Better luck next time.",
    "Your score qualifies you as a novice-class adventurer.",
    "You have achieved the rating \"Experienced Adventurer\".",
    "You may now consider yourself a \"Seasoned Adventurer\".",
    "You have reached \"Junior Master\" status.",
    "Your score puts you in Master Adventurer class C.",
    "Your score puts you in Master Adventurer class B.",
    "Your score puts you in Master Adventurer class A.",
    "All of Adventuredom gives tribute to you, Adventurer Grandmaster!"
};

void finis(void)
{
    /* Print the score and say adieu. */
    int s = score();
    printf("You have scored a total of %d points, out of a possible maximum of\n", s);
    puts("550 points.  During this game of Adventure, you have taken a total of");
    printf("%d turns.\n", turns);

    int rank;
    for (rank = 0; class_score[rank] <= s; ++rank) ;
    puts(class_message[rank]);
    puts("");
    if (rank < HIGHEST_CLASS) {
        int delta = class_score[rank] - s;
        if (delta == 1) {
            puts("You only need one more point to reach the next level of expertise!\n");
        } else {
            printf("To reach the next qualification level you need %d more points.\n\n", delta);
        }
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

void kill_the_player(Location loc)
{
    gave_up = false;
    puts("");
    nomagic = false;
    blob_is_coming = false;
    apport(BLOB, R_LIMBO); objs[BLOB].prop = 0;
    apport(GOBLINS, R_LIMBO);
    if (objs[BASILISK].prop == 1) {
        objs[BASILISK].prop = 0;
    } else if (objs[BASILISK].prop == 3) {
        objs[BASILISK].prop = 2;
    }
    ++death_count;
    if (closure == 2) {
        puts("\nIt looks as though you're dead.  Well, seeing as how it's so close to\n"
             "closing time anyway, I think we'll just call it a day.");
        finis();
    } else if (closure > 2) {
        --death_count;  /* no charge for dying in the repository */
        finis();
    }
    switch (death_count) {
        case 1:
            puts("Oh dear, you seem to have gotten yourself killed.  I might be able to\n"
                 "help you out, but I've never really done this before.  Do you want me\n"
                 "to try to reincarnate you?");
            break;
        case 2:
            puts("Tsk, tsk - you did it again!  Remember - you're only human, and you\n"
                 "don't have as many lives as a cat!  (at least, I don't think so...)\n"
                 "That's twice you've ended up dead - want to try for three?");
            break;
        case 3:
            puts("You clumsy oaf, you've done it again!  I don't know how long I can\n"
                 "keep this up.  Do you want me to try reincarnating you again?");
            break;
        case 4:
            puts("Now you've really done it!  I'm out of orange smoke!  You don't expect\n"
                 "me to do a decent reincarnation without any orange smoke, do you?");
            break;
    }
    if (!yes(NULL)) {
        finis();
    }
    switch (death_count) {
        case 1:
            puts("All right.  But don't blame me if something goes wr......\n"
                 "                     -- poof!! --\n"
                 "You are engulfed in a cloud of orange smoke.  Coughing and gasping,\n"
                 "you emerge from the smoke and find....\n");
            break;
        case 2:
            puts("As you wish.  Hang on for just a second while I fire up my thurible...\n"
                 ">foof!<    An immense cloud of orange smoke fills the air.  As it\n"
                 "clears, you find that once again....\n");
            break;
        case 3:
            puts("Okay, now where did I put my orange smoke?....  >poof!<\n"
                 "Everything disappears in a dense cloud of orange smoke.\n");
            break;
        case 4:
            puts("Yes....  well, that's the kind of blinkered, Philistine pig-ignorance\n"
                 "that I've come to expect from you mortals.  You sit there on your\n"
                 "loathsome, spotty behind, squeezing blackheads and not caring a\n"
                 "thinker's damn about the struggling cave-sprite, you simpering,\n"
                 "whining, mouldy-faced heap of parrot droppings!  If you're so\n"
                 "smart, then you can just reincarnate yourself, because quite\n"
                 "frankly I'm as mad as hell and I'm not going to take this\n"
                 "anymore - I'm leaving!!!!\n");
            finis();  /* does not return; you're really dead now */
            break;
    }

    /* At this point you are reborn. */
    if (toting(VASE)) {
        apport(VASE, R_LIMBO);
        apport(SHARDS, R_INHAND);  /* How unfortunate! */
    }
    apport(WATER, R_LIMBO);
    apport(OIL, R_LIMBO);
    for (int i=0; i <= MAX_OBJ; ++i) {
        if (toting(i))
            apport(i, loc);
    }
    holding_count = 0;
    apport(LAMP, R_ROAD); objs[LAMP].prop = 0;
    apport(FOG, R_PLAIN_1); objs[FOG].prop = 8;
    phog(R_ROAD);  /* chase glow into place */
    if (lamplife == 0) {
        if (have_used_noside_samoht || !(places[R_LAIR].flags & F_BEENHERE)) {
            /* We either don't know NOSIDE SAMOHT or haven't learned it. */
            if (objs[BATTERIES].prop || !(places[objs[BATTERIES].place].flags & F_NOTINCAVE)) {
                /* The batteries are either dead or lost in the cave. */
                apport(LAMP, R_YLEM);
            }
        }
    }
    apport(DWARF, R_LIMBO); objs[DWARF].prop = 0;
    pirate_stalking_you = false;
}

/*========== Closing the cave. ==========================================*/

void close_the_cave(void)
{
    /* Platt, following Woods, has "entones". I change it to "intones". */
    puts("\nThe sepulchral voice intones, \"The cave is now closed.\"  As the echoes\n"
         "fade, there is a blinding flash of light (and a small puff of orange\n"
         "smoke). . . .    As your eyes refocus, you look around and find...\n");
    verbosity_mode = FULL;

    /* Clear all objects from the above-ground portion of the map. */
    for (int i=0; i <= MAX_OBJ; ++i) {
        if (objs[i].flags & F_PORTABLE) {
            if (places[objs[i].place].flags & F_NOTINCAVE) apport(i, R_YLEM);
            else if (toting(i)) apport(i, R_YLEM);
        }
    }

    /* Pretend all this terrain is new to us. */
    for (int i=0; i <= R_YLEM; ++i) {
        if (places[i].flags & F_NOTINCAVE)
            places[i].flags &= ~F_BEENHERE;
    }
    
    closure = 3;
    clock = 999;
    cylinder_escape_count = 0;
}

/*========== Verb processing. ===========================================*/

bool now_in_darkness(Location loc)
{
    if (places[loc].flags & F_LIGHTED) return false;
    if (here(LAMP, loc) && objs[LAMP].prop) return false;
    return true;
}

void attempt_inventory(void)
{
    if (holding_count == 0) {
        puts("You're not carrying anything.");
        return;
    }
    puts("You are currently holding the following:");
    for (int i=0; i <= MAX_OBJ; ++i) {
        if (!toting(i)) continue;
        if (i == WATER || i == OIL || i == BEAR) continue;
        if (i == BOTTLE) {
            if (toting(WATER)) {
                puts("Small bottle full of water");
            } else if (toting(OIL)) {
                puts("Small bottle full of oil");
            } else {
                puts("Small bottle");
            }
        } else if (i == BATTERIES) {
            if (objs[BATTERIES].prop) {
                puts("Worn-out batteries");
            } else {
                puts("Fresh batteries");
            }
        } else {
            puts(objs[i].name);
        }
    }
    /* Platt doesn't mention the bear. */
}

ObjectWord default_to_something_portable(Location loc)
{
    int candidate = NOTHING;
    for (int i=1; i <= MAX_OBJ; ++i) {
        if (!there(i, loc)) continue;
        if (!portable(i)) continue;
        if (candidate != NOTHING) return NOTHING;
        candidate = i;
    }
    /* We don't update word2.text, but that's okay, because the caller
     * never tries to print the name of the object in this case. */
    if (candidate != NOTHING) {
        word2.type = WordType_Object;
        word2.meaning = candidate;
    }
    return candidate;
}

void getbird(Location loc)
{
    if (toting(BIRD)) {
        you_have_it();
    } else if (!there(BIRD, loc)) {
        I_see_no(word2.text);
    } else if (objs[BIRD].prop) {
        apport(CAGE, R_INHAND);
        apport(BIRD, R_INHAND);
        puts(ok);
    } else if (toting(CAGE)) {
        if (toting(ROD)) {
            puts("The bird was unafraid when you entered, but as you approach it becomes\n"
                 "disturbed and you cannot catch it.");
        } else {
            places[R_BIRD].flags &= ~F_HINTABLE;
            apport(BIRD, R_INHAND);
            objs[BIRD].prop = 1;  /* caged */
            puts(ok);
        }
    } else {
        puts("You might be able to catch the bird, but you could not carry it.");
    }
}

void getliquid(ObjectWord obj)
{
    if (toting(BOTTLE)) {
        if (objs[BOTTLE].prop == 1) {
            apport(obj, R_INHAND);
            if (obj == OIL) {
                objs[BOTTLE].prop = 2;
                puts("Your bottle is now full of oil.");
            } else {
                objs[BOTTLE].prop = 0;
                puts("Your bottle is now full of water.");
            }
        } else {
            puts("Your bottle is already full.");
        }
    } else {
        puts("You have nothing in which to carry it.");
    }
}

void getsceptre(void)
{
    /* We select the password randomly at the time the hint is given,
     * rather than at the start of the game, to prevent save-scumming. */
    const VerbWord possible_passwords[5] = { BLERBI, KLAETU, KNERL, SNOEZE, ZORTON };
    const char *possible_spellings[5] = { "BLERBI", "KLAETU", "KNERL", "SNOEZE", "ZORTON" };
    int i = ran(5);
    const char *spelling = possible_spellings[i];
    password = possible_passwords[i];
    if (objs[SAFE].prop) {
        puts("As you pluck the sceptre from the skeleton's grasp, it raises its head\n"
             "and whispers, \"You blew it!\".  It then shivers and collapses into a\n"
             "pile of fine dust which quickly vanishes.");
    } else {
        printf("You pluck the sceptre from the skeleton's bony hand.  As you do, the\n"
               "skeleton raises its head and whispers \"Remember - %s!\" in a\n",
               spelling);
        /* Platt has "forboding"; I'm fixing it. */
        puts("foreboding tone; it then sags to the ground and crumbles into dust which\n"
             "drifts away into the still air of the cave.");
    }
    apport(SKELETON, R_LIMBO);
    apport(SCEPTRE, R_INHAND);
    objs[SCEPTRE].prop = 1;  /* no longer clutched by skeleton */
}

int attempt_take(ObjectWord obj, Location loc)
{
    if (now_in_darkness(loc)) {
        if (word2.type == WordType_None) {
            puts("It's pitch dark in here - I can't tell whether there's anything here\n"
                 "that I can pick up!");
            return loc;
        } else if (word2.type == WordType_Object && !toting(obj) && portable(obj)) {
            if (pct(60 - 5*(strength - holding_count))) {
                printf("Hmmph - you're not asking for much, are you - it's pitch dark in\n"
                       "here!  Well, I'll grope around and try to find the %s.....\n", word2.text);
                puts(" {hunt}\n"
                     "         {hunt}\n"
                     "                  {rummage}\n"
                     "                              {trip!}\n"
                     "                                       Aaaiiii\n"
                     "                                              iie\n"
                     "                                                 ee\n"
                     "                                                   e\n"
                     "                                                    e\n"
                     "                                                    e\n"
                     "                                                    e\n"
                     "                                                     >SPLAT!<\n\n"
                     "You stumbled into a pit and broke your back!");
                return you_are_dead_at(loc);
            } else {
                printf("Hmmph - you're not asking for much, are you - it's pitch dark\n"
                       "in here, and I'll have to grope around to try to find the %s.\n", word2.text);
                puts("Well, if I must, I must.....\n"
                     "                                 {hunt}\n"
                     "                                           {search}\n"
                     "                                                      {hunt}\n"
                     "                  {rummage}\n"
                     "         {peer}\n"
                     "                  {touch}\n"
                     "             {hunt}\n"
                     "                                 >scrape<\n"
                     "                                          Youch! {curse}\n"
                     "                  {hunt}\n"
                     "                              {seek}\n");
                printf("No luck - I can't find the %s!  If you could get me some light in\n"
                       "here, maybe I'd be able to do better.  Sorry....", word2.text);
                return loc;
            }
        }
    }
    if (word2.type == WordType_None) {
        obj = default_to_something_portable(loc);
    }
    if (keywordv(INVENTORY)) {
        attempt_inventory();
        return loc;
    } else if (word2.type == WordType_Object) {
        switch (obj) {
            case BIRD:
                getbird(loc);
                return loc;
            case OIL:
                if (loc == R_EPIT) {
                    getliquid(OIL); return loc;
                }
                break;
            case WATER:
                if (places[loc].flags & F_WATER) {
                    getliquid(WATER);
                    return loc;
                }
                break;
            case CHAIN:
                if (there(CHAIN, loc) && !objs[CHAIN].prop) {
                    puts("The chain is still locked.");
                    return loc;
                }
                break;
            case BEAR:
                if (toting(BEAR)) {
                    puts("You are being followed by a very large, tame bear.");
                } else if (there(BEAR, loc) && (objs[BEAR].prop != 2)) {
                    puts("The bear is still chained to the wall.");
                    return loc;
                }
                break;
            case SWORD:
                if (there(SWORD, loc) && !objs[SWORD].prop && holding_count < strength) {
                    if (objs[MUSHROOM].prop == 2) {
                        puts("The singing sword slides easily out of the rock.");
                        objs[SWORD].prop = 1;
                        apport(SWORD, R_INHAND);
                    } else {
                        puts("The sword is firmly embedded in the stone, and you aren't strong\n"
                             "enough to pull it out.");
                    }
                    return loc;
                }
                break;
            case SCEPTRE:
                if (there(SCEPTRE, loc) && !objs[SCEPTRE].prop && holding_count < strength) {
                    getsceptre();
                    return loc;
                }
                break;
            case KNIFE:
                if (objs[AXE].flags & F_SEEN) {
                    puts("The dwarves' knives vanish as they strike the walls of the cave.");
                    return loc;
                }
                break;
        }  /* end switch */
    }
    if (word2.type == WordType_None)
        return 0;  /* should be handled by main loop */
    if (word2.type != WordType_Object) {
        hah();
    } else if (toting(obj)) {
        you_have_it();
    } else if (!there(obj, loc)) {
        I_see_no(word2.text);
    } else if (!(objs[obj].flags & F_PORTABLE)) {
        hah();
    } else if (holding_count == strength && obj != CAGE) {
        /* Platt lets you pick the cage for free. This is certainly a bug. */
        puts("You can't carry anything more.  You'll have to drop something first.");
    } else {
        puts(ok);
        apport(obj, R_INHAND);
        if (obj == CAGE && objs[BIRD].prop)
            apport(BIRD, R_INHAND);
        if (obj == BOTTLE && objs[BOTTLE].prop == 0)
            apport(WATER, R_INHAND);
        if (obj == BOTTLE && objs[BOTTLE].prop == 2)
            apport(OIL, R_INHAND);
        if ((objs[obj].flags & F_UNSTABLE) && !objs[obj].prop) {
            /* Unstable objects, such as the casket and the sculpture,
             * flip to state 1 the first time they're picked up. */
            objs[obj].prop = 1;
        }
    }
    return loc;
}

int dropbird(Location loc)
{
    if (there(BIRD, loc)) {
        puts("You aren't carrying it!");
        return loc;
    } else if (!toting(BIRD)) {
        I_see_no(word2.text);
        return loc;
    }
    apport(BIRD, loc);
    objs[BIRD].prop = 0;  /* uncaged */
    if (there(SNAKE, loc)) {
        puts("The little bird attacks the green snake, and in an astounding flurry\n"
             "drives the snake away.");
        apport(SNAKE, R_LIMBO);
        places[R_HMK].flags &= ~F_HINTABLE;
    } else if (there(DRAGON, loc) && !objs[DRAGON].prop) {
        puts("The little bird attacks the green dragon, and in an astounding flurry\n"
             "gets burnt to a cinder.  The ashes blow away.");
        apport(BIRD, R_LIMBO);
    } else if (there(TROLL, loc)) {
        puts("The little bird attacks the troll, who blithly ignores it.  The bird\n"
             "soon becomes discouraged, flies back to land at your feet, and begins\n"
             "singing to itself.");
    } else if (there(OGRE, loc)) {
        puts("The little bird attacks the ogre, and in an astounding flurry gets\n"
             "caught and crushed.  The mangled remains vanish into thin air.");
        apport(BIRD, R_LIMBO);
    } else if (there(BEAR, loc)) {
        if (!objs[BEAR].prop) {
            puts("The little bird attacks the ferocious cave bear, and in an astounding\n"
                 "flurry gets ripped to shreds.  The bloody remains disappear.");
            apport(BIRD, R_LIMBO);
        } else {
            puts("The little bird flies over to the bear, circles curiously around\n"
                 "its head several times, and then flies back to land at your feet\n"
                 "and begins singing.");
        }
    } else if (there(BASILISK, loc) && objs[BASILISK].prop <= 1) {
        puts("The little bird attacks the basilisk, and in an astounding flurry\n"
            "succeeds in awakening it.  The basilisk glares at the bird, causing\n"
            "the bird to explode into a cloud of fine sand, and then turns and\n"
            "stares balefully at you, turning you into a pillar of fine marble\n"
            "suitable for decorative lawn or garden use.");
        apport(BIRD, R_LIMBO);
        return you_are_dead_at(loc);
    } else if (there(DWARF, loc)) {
        const bool plural = (objs[DWARF].prop > 1);
        printf("The little bird attacks the dwar%s, and in an astounding flurry\n"
               "gets cut to shreds by %s nasty sharp kni%s!  The tattered\n"
               "remains fade from sight.\n",
               plural?"ves":"f", plural?"their":"his", plural?"ves":"f");
        apport(BIRD, R_LIMBO);
    } else {
        puts(ok);
    }
    return loc;
}

bool dropliquid(ObjectWord obj, Location loc)
{
    /* DROP WATER does not work to water the plant; you must say
     * either WATER PLANT or POUR WATER. */
    if (!toting(obj)) return false;
    apport(obj, R_LIMBO);
    objs[BOTTLE].prop = 1;
    if (there(DWARF, loc) && keywordv(THROW)) {
        if (objs[DWARF].prop == 1) {
            printf("The %s flies through the air and thoroughly drenches the dwarf.  He\n", obj==OIL?"oil":"water");
            puts("shakes himself off and curses violently; he *REALLY* looks angry!");
        } else {
            printf("The %s flies through the air and thoroughly drenches the dwarves.  They\n", obj==OIL?"oil":"water");
            puts("shake themselves off and curse violently; they *REALLY* look angry!");
        }
        dwarves_enraged = true;
    } else {
        puts("Your bottle is empty and the ground is wet.");
    }
    return true;
}

bool freedjinn(Location loc)
{
    if (!there(DJINN, loc)) return false;
    puts("The pentagram's magical barrier sparks fitfully and goes down.  The\n"
         "Djinn stretches gratefully and smiles at you.  \"AGAIN, MY THANKS,\" he\n"
         "says.  \"MY ADVICE TO YOU WILL TAKE THE FORM OF A HISTORY LESSON.\n"
         "WHEN RALPH WITT, THE ARCHITECT AND CONSTRUCTOR OF THIS CAVE, WAS VERY\n"
         "YOUNG, HE BECAME VERY INCENSED THAT HIS NAME WAS AT THE END OF THE\n"
         "ALPHABET.  HE FELT (FOR SOME REASON) THAT THE LETTER W BELONGED NEAR\n"
         "THE BEGINNING OF THE ALPHABET, AND THAT ALL OF THOSE \"UPSTART LETTERS\n"
         "WHICH UNFAIRLY USURPED THE BEST PLACES\" SHOULD BE FORCED INTO EXILE\n"
         "AT THE END OF THE ALPHABET.  HIS INSTINCT FOR MATTERS MAGICAL AND\n"
         "MYSTICAL LED HIM TO APPLY THIS STRANGE BELIEF INTO THE CAVE'S\n"
         "STRUCTURE WHEN HE EXCAVATED IT.  YOU HAVEN'T YET BEEN AFFECTED BY HIS\n"
         "STRANGE HABITS, BUT YOU SHOULD REMEMBER THIS.  FAREWELL, AND GOOD\n"
         "LUCK.\"  With that, the Djinn evaporates into a cloud of smoke and\n"
         "drifts rapidly away.");
    apport(DJINN, R_LIMBO);
    djinn_is_grateful = true;
    return true;
}

int attempt_drop(ObjectWord obj, Location loc)
{
    if (word2.type == WordType_None) return 0;  /* unhandled */
    if (word2.type == WordType_Object) {
        switch (obj) {
            case BIRD: dropbird(loc); return loc;
            case OIL: if (dropliquid(obj, loc)) return loc; break;
            case WATER: if (dropliquid(obj, loc)) return loc; break;
            case DJINN: if (freedjinn(loc)) return loc; break;
        }
    }
    if (word2.type != WordType_Object) {
        hah();
    } else if (toting(obj)) {
        apport(obj, loc);
        if (obj == CAGE) {
            if (toting(BIRD))
                apport(BIRD, loc);
        } else if (obj == BOTTLE) {
            apport(WATER, R_LIMBO);
            apport(OIL, R_LIMBO);
        } else if (obj == VASE && loc != R_SOFT) {
            if (there(PILLOW, loc)) {
                puts("The vase is now resting, delicately, on a velvet pillow.");
            } else {
                puts("The Ming vase drops with a delicate crash.");
                apport(VASE, R_LIMBO);
                apport(SHARDS, loc);
            }
            return loc;
        } else if (obj == VIAL && pct(10)) {
            puts("The vial strikes the ground and explodes with a violent >FOOM!<,\n"
                 "neatly severing your foot.  You bleed to death quickly and messily.");
            apport(VIAL, R_LIMBO);
            return you_are_dead_at(loc);
        }
        /* For bear/troll logic, see at_neofchasm(). Platt's code has a
         * subroutine named DROPBEAR, but it doesn't do anything special. */
        puts(ok);
    } else {
        puts("You aren't carrying it!");
    }
    return loc;
}

int process_verb(Location loc)
{
    if (word1.type != WordType_Verb) return 0;  /* unhandled */
    const int verb = word1.meaning;
    const int obj = word2.meaning;
    switch (verb) {
        case GET:
            return attempt_take(obj, loc);
        case DROP:
            return attempt_drop(obj, loc);
        case WALK:
            if (word2.type == WordType_None) {
                printf("Which way should I %s?\n", word1.text);
                return loc;
            } else if (word1.type == WordType_Verb &&
                       is_walkable_direction(obj)) {
                verb = obj;
                
            }
            
    }
    return 0;  /* unhandled */
}

/*========== The main loop. =============================================*/

void look_around(Location loc, bool familiar)
{
    bool blank_line_before_objects = false;
    if (verbosity_mode == FULL ||
        (verbosity_mode == BRIEF && !familiar) ||
        places[loc].short_desc == NULL) {
        puts(places[loc].long_desc);
        blank_line_before_objects = true;
    } else {
        puts(places[loc].short_desc);
    }
    if (places[loc].flags & F_DAMP)
        puts("The ground here is damp.");
    /* Now we've seen this place. */
    places[loc].flags |= F_BEENHERE;
    for (int i=0; i <= MAX_OBJ; ++i) {
        if (there(i, loc)) {
            objs[i].flags |= F_SEEN;
            if (!(objs[i].flags & F_INVISIBLE)) {
                if (blank_line_before_objects) {
                    puts("");
                    blank_line_before_objects = false;
                }
                describe_object(i);
            }
        }
    }
    if (toting(BEAR)) {
        puts("You are being followed by a very large, tame bear.");
    }
}

void phog(Location loc)
{
    if (objs[FOG].prop < 8) {
        objs[FOG].prop = ran(8);
    }
    objs[GLOW].prop = ran(8);
    if (now_in_darkness(loc)) {
        /* If your lamp is on, you can't see the faint glow. */
        apport(GLOW, R_LIMBO);
    } else {
        apport(GLOW, R_PLAIN_2);
        if (loc == R_PLAIN_2 && !moved) {
            describe_object(GLOW);
        }
    }
}

/* Return true if we closed the cave on you. */
bool lamprey(Location loc)
{
    /* Interestingly, the lamp's batteries do not run down while you are
     * elsewhere. This is a minor difference from Woods' Adventure. It also
     * means that this logic will not run after closing time. */
    if (!here(LAMP, loc) || !objs[LAMP].prop)
        return false;
    lamplife -= 1;
    if (lamplife == 40) {
        if (objs[BATTERIES].prop == 1) {
            puts("Your lamp is getting dim, and you're out of spare batteries.  You'd\n"
                 "best start wrapping this up.");
        } else if (here(BATTERIES, loc)) {
            puts("Your lamp is getting dim.  I'm taking the liberty of replacing the\n"
                 "batteries.");
            objs[BATTERIES].prop = 1;
            lamplife += 300;
            have_used_noside_samoht = false;
        } else if (objs[BATTERIES].flags & F_SEEN) {
            puts("Your lamp is getting dim.  You'd best go back for those batteries.");
        } else {
            puts("Your lamp is getting dim.  You'd best start wrapping this up, unless\n"
                 "you can find some fresh batteries.  I seem to recall there's a vending\n"
                 "machine in the maze.  Bring some coins with you.");
        }
    } else if (lamplife == 0) {
        if (closure == 2) {
            /* If the lamp runs out during closing time,
            * just go ahead and close the cave early. */
            close_the_cave();
            return true;
        } else if (here(BATTERIES, loc) && !objs[BATTERIES].prop) {
            puts("Your lamp is getting dim.  I'm taking the liberty of replacing the\n"
                 "batteries.");
            objs[BATTERIES].prop = 1;
            lamplife += 300;
        } else {
            puts("Your lamp has run out of power.");
            objs[LAMP].prop = 0;
            lamp_just_ran_out = true;
            phog(loc);  /* chase glow into place */
        }
    }
    return false;  /* cave is still open */
}

/* The administrative clock has ticked. */
bool clock4(Location loc)
{
    if (closure == 0) {
        closure = 1;
        for (int i = MIN_TREASURE; i <= MAX_OBJ; ++i) {
            if (!there(i, R_HOUSE))
                closure = 0;
        }
        if (closure == 1) {
            clock = 35;
        } else {
            clock = 30+ran(10);
        }
        if (objs[SCULPTURE].prop != 0) {
            /* Once you pick up the sculpture, it starts morphing. */
            objs[SCULPTURE].prop = 1+ran(14);
        }
        if (objs[SWORD].prop != 0) {
            /* The singing sword also morphs its appearance. */
            objs[SWORD].prop = 1+ran(11);
        }
        if (objs[DRAGON].prop == 2) {
            /* Here Platt's A-code counts down a variable called DRAGTIME,
             * which I suppose is intended to make the dragon carcass last
             * for a while before it turns "rotting". DRAGTIME should
             * presumably have been set to some large positive value in
             * the handler for KILL DRAGON; but in fact it is not, at least
             * in the versions I've seen. Since variables are initialized
             * to zero, this makes the dragon rot immediately at the next
             * "admin clock tick" interval. */
            objs[DRAGON].prop = 3;
        }
        if (djinn_is_grateful && !djinn_gave_hint && !there(DWARF, loc)) {
            puts("A large phosphorescent cloud of smoke drifts into view, and a large\n"
                 "mouth and two dark eyes take shape on the side.  One of the eyes winks\n"
                 "at you, and the djinn's deep voice says \"GREETINGS AGAIN, MORTAL.  I\n"
                 "HAVE REMEMBERED A PIECE OF ANCIENT LORE THAT I LEARNED FROM MY AUNT,\n"
                 "AN AFREET OF GREAT KNOWLEDGE.  THERE IS ANOTHER MAGIC WORD THAT YOU\n"
                 "MIGHT FIND OF USE IF YOU SHOULD EVER FIND YOURSELF BEING ATTACKED BY\n"
                 "THOSE PESTIFEROUS DWARVES.  YOU SHOULD USE IT ONLY AS A LAST RESORT,\n"
                 "THOUGH, SINCE IT IS A MOST POTENT WORD AND IS PRONE TO BACKFIRE FOR\n"
                 "NO OBVIOUS REASON;  ALSO, IT SHOULD NEVER BE USED NEAR WATER OR NEAR\n"
                 "ANY SHARP WEAPON OR THE RESULTS MAY BE MOST UNFORTUNATE.  THE WORD\n"
                 "IS 'phuggg'\", whispers the djinn, \"AND IT MUST BE PRONOUNCED CAREFULLY\n"
                 "IF IT IS TO HAVE THE PROPER EFFECT.  FAREWELL AGAIN, AND GOOD LUCK!\"\n"
                 "With that, the djinn-cloud drifts away out of sight.");
            djinn_gave_hint = true;
            lastclock = clock = 5;
        }
        if (objs[MUSHROOM].prop > 1) {
            mushtime -= lastclock;
            if (mushtime < 0) {
                if (objs[MUSHROOM].prop == 2) {
                    /* The mushroom is no longer "eaten". It will go back
                     * to the cubicle during the next admin-cycle. */
                    objs[MUSHROOM].prop = 3;
                    mushtime = 40;
                    puts("A strange malaise suddenly afflicts you.  You shiver with chill,\n"
                         "and your muscles seem to turn to putty;  everything around you becomes\n"
                         "grey and unreal.  The fit quickly passes, and you find that your body\n"
                         "has degenerated back to what it was like before you ate the mushroom.");
                    strength = 7;
                    lastclock = clock = 8;
                } else {
                    /* The mushroom grows back, in the cubicle. */
                    objs[MUSHROOM].prop = 0;
                    apport(MUSHROOM, R_CUBICLE);
                }
            }
        }
        maybe_do_cameo(loc);
        if ((places[R_EMIST].flags | places[R_Y2].flags) & F_BEENHERE) {
            if (places[loc].flags & F_NODWARF) {
                clock = 8+ran(10);
            } else {
                /* Have we been here a while without seeing the pirate?
                 * Maybe he should show up. */
                bool pirate_appears = pirate_stalking_you || pct(15);
                if (turns > 150 && !(objs[PIRATE].flags & F_SEEN))
                    pirate_appears = true;
                if (objs[CHEST].flags & F_SEEN)
                    pirate_appears = false;
                if (there(DWARF, loc))
                    pirate_appears = false;  /* he's shy */
                if ((places[loc].flags & F_LIGHTED) || now_in_darkness(loc))
                    pirate_appears = false;  /* he likes the shadows */
                if (pirate_appears) {
                    pirate_stalking_you = false;
                    bool pirate_stole_something = false;
                    for (int i = MIN_TREASURE; i <= MAX_OBJ; ++i) {
                        if (i == RING) continue;  /* never steal this */
                        if (here(i, loc)) {
                            apport(i, R_PIRATES_NEST);
                            pirate_stole_something = true;
                        }
                    }
                    if (pirate_stole_something) {
                        if (objs[PIRATE].flags & F_SEEN) {
                            puts("Out from the darkness behind you leaps the pirate!  \"Har, har,\" he\n"
                                 "chortles nastily, \"I see you've found some more treasure for me!\n"
                                 "Thanks, me hearty, you're doing just fine!\"  With that, he snatches\n"
                                 "your treasure and slips away into the gloom, chuckling with glee.");
                        } else {
                            puts("Out from the shadows behind you pounces a bearded pirate!  \"Har, har,\"\n"
                                 "he chortles, \"I'll just take all this booty and hide it away with me\n"
                                 "chest deep in the maze!\"  He snatches your treasure and vanishes into\n"
                                 "the gloom.");
                            objs[PIRATE].flags |= F_SEEN;
                            apport(CHEST, R_PIRATES_NEST);
                            apport(MESSAGE, R_PONY);
                        }
                    } else {
                        if (turns > 150 && !(objs[PIRATE].flags & F_SEEN)) {
                            apport(CHEST, R_PIRATES_NEST);
                            apport(MESSAGE, R_PONY);
                            puts("\nThere are faint rustling noises from the darkness behind you.  As you\n"
                                 "turn toward them, the beam of your lamp falls across a bearded pirate.\n"
                                 "He is carrying a large chest.  \"Shiver me timbers!\" he cries, \"I've\n"
                                 "been spotted!  I'd best hie meself off to the maze to hide me chest!\"\n"
                                 "With that, he vanishes into the gloom.");
                            objs[PIRATE].flags |= F_SEEN;
                        } else {
                            puts("There are faint rustling noises from the darkness behind you.");
                            pirate_stalking_you = true;
                            clock = 4+ran(10);
                        }
                    }
                } else {
                    /* The pirate is not appearing right now. */
                    if (there(DWARF, loc) || (objs[DWARF].prop < dwarfcount && pct(20+10*dwarfcount))) {
                        if (objs[AXE].flags & F_SEEN) {
                            apport(DWARF, loc);
                            objs[DWARF].prop += 1;
                            if (objs[DWARF].prop == 1) {
                                /* Announce the arrival of this dwarf. */
                                dwarf_warning_shot = true;
                                dwarves_enraged = false;
                            }
                        } else {
                            puts("\nA little dwarf just walked around a corner, saw you, threw a little\n"
                                 "axe at you which missed, cursed, and ran away.");
                            objs[AXE].flags |= F_SEEN;
                            apport(AXE, loc);
                        }
                    }
                }
            }
        }
    } else if (closure == 1) {
        closure = 2;
        objs[GRATE].prop = 0;  /* lock the grate */
        puts("\nA sepulchral voice reverberating through the cave, says, \"Cave closing\n"
             "soon.  All adventurers please report to the treasure room via the\n"
             "alternate entrance to claim your treasure.\n");
        if (there(DWARF, loc)) {
            bool more_than_one = (objs[DWARF].prop > 1);
            printf("The dwar%s quickly scuttle%s off into the gloom.\n",
                   more_than_one ? "ves" : "f",
                   more_than_one ? "" : "s");
        }
        apport(DWARF, R_LIMBO);
        objs[DWARF].prop = 0;
        dwarfcount = 0;  /* kill all the dwarves */
        objs[FISSURE].prop = 0;  /* destroy crystal bridge */
        objs[GEYSER].prop = 0;  /* destroy wheatstone bridge */
        apport(TROLL, R_LIMBO); objs[TROLL].prop = 5;
        apport(TROLL2, R_SWOFCHASM);  /* fetch fake troll */
        apport(DRAGON, R_LIMBO);
        apport(TROLL, R_LIMBO);
        objs[FISSURE].flags |= F_INVISIBLE;
        objs[GEYSER].flags |= F_INVISIBLE;
        clock = 25;
    } else {
        /* It's closing time! This is a more evil trick than the
         * one in Woods' code: as long as the panicked player keeps
         * trying to exit the cave, it will never close. Only when
         * you've spent 15 turns quietly observing your surroundings
         * will the cave finally close on its own. */
        if (panicked) {
            panicked = false;
            clock = 15;
        } else {
            close_the_cave();
        }
    }
    lastclock = clock;
    return (closure == 3);  /* true if the cave is now closed */
}

void deal_with_syntax_errors(Location loc)
{
    if (word1.type == WordType_Object) {
        if (there(word1.meaning, loc)) {
            printf("What do you want me to do with the %s?\n", word1.text);
            please_clarify = true;
        } else {
            I_see_no(word1.text);
        }
    } else {
        if (word2.type == WordType_None) {
            word1.text[0] = toupper(word1.text[0]);
            printf("%s what?\n", word1.text);
            please_clarify = true;
        } else if (word2.type == WordType_Object) {
            if (there(word2.meaning, loc)) {
                printf("I don't know how to %s such a thing.\n", word1.text);
            } else {
                I_see_no(word2.text);
            }
        } else {
            printf("I don't know how to %s such a thing.\n", word1.text);
        }
    }
}

void say_foof(void)
{
    if (pct(95)) {
        switch (ran(6)) {
            case 0: puts("\n>>Foof!<<\n"); break;
            case 1: puts("\n>>Foom!<<\n"); break;
            case 2: puts("\n>>Poof!<<\n"); break;
            case 3: puts("\n>>Whoooosh!<<\n"); break;
            case 4: puts("\nA thick cloud of orange smoke appears from thin air and blocks your\n"
                         "view.   When it vanishes, you find:\n"); break;
            case 5: puts("\nThere is a brilliant flash of light and a sudden fanfare of trumpets!\n"
                         "When your eyes recover from the flash, you find that:\n"); break;
        }
    } else {
        puts("\n>sputter<\n"
        "            >spark<\n"
        "      >phut<\n"
        "               >hsssssssssss\n"
        "                            ssssss\n"
        "                                  sss\n"
        "                                     s\n"
        "                                      s\n"
        "                                       .\n"
        "                                        .\n"
        "                                         .\n\n"
        "Oh, blast.  Excuse me for a moment, please......\n\n"
        "                                                          {KICK!}\n\n"
        ">>Foof!<<");
    }
}

void presay(void)
{
    if (keywordv(SAY) && (word2.type != WordType_None)) {
        printf("Ok - \"%s\".\n", word2.text);
    }
}

void recount_inventory(void)
{
    holding_count = 0;
    for (int i=0; i <= MAX_OBJ; ++i) {
        if (i == BIRD) continue;  /* it's caged */
        if (i == OIL || i == WATER) continue;  /* they're bottled */
        if (i == BEAR) continue;  /* it's following you */
        if (toting(i))
            holding_count += 1;
    }
}

void simulate_an_adventure(void)
{
    Location oldloc;
    Location loc = R_LIMBO;
    Location newloc = R_ROAD;

    clock = 15+ran(10);
    dwarfcount = 4+ran(5);
    objs[CHAIN].prop = 1;
    objs[FOG].prop = 8;
    objs[FLASK].prop = 1;

    if (false) {
      death:
        kill_the_player(loc);
        /* Coming from R_LIMBO ensures that we'll see a room description
         * when we resurrect. */
        oldloc = loc = R_LIMBO;
        newloc = R_HOUSE;
    }
    
    while (true) {
        if (blob_is_coming) {
            if (tick()) goto death;
        }
        moved = (newloc != loc);
        if (!moved) {
            if (there(DWARF, loc) && pct(backlash)) {
                /* If you stand still, dwarves in the room will attack. */
                if (dwarf_attack()) goto death;
            }
        } else if ((places[loc].flags & F_ONE_EXIT) && there(DWARF, loc) && !wizard_mode) {
            puts("A little dwarf with a big knife blocks your way.");
            newloc = loc;
        } else {
            if (there(FOG, loc)) {
                phog(newloc);
            }
            /* Okay, we moved! */
            moved = false;
            oldloc = loc;
            loc = newloc;
            ++turns;
            if (turns == MAX_GAME_LENGTH) {
                puts("\nA large cloud of green smoke appears in front of you.  It clears away\n"
                     "to reveal a tall wizard, clothed in grey.  He fixes you with a steely\n"
                     "glare and declares, \"This adventure has lasted too long.\"  With that\n"
                     "he makes a single pass over you with his hands, and everything around\n"
                     "you fades away into a grey nothingness.");
                finis();
            }
            if (lamprey(loc))
                continue;  /* the cave is now closed */
            if (!there(GOBLINS, R_LIMBO)) {
                /* If the goblins are active, they follow you. */
                apport(GOBLINS, loc);
                if (objs[GOBLINS].prop >= 0)
                    puts("You are being pursued by a vicious horde of little gooseberry goblins!\n");
                objs[GOBLINS].prop += 1;
            }
            bool familiar_place = (places[loc].flags & F_BEENHERE);
            if (now_in_darkness(loc)) {
                if (!(places[oldloc].flags & F_LIGHTED) &&
                    pct(25) &&
                    !lamp_just_ran_out) {
                    puts("You fell into a pit and broke every bone in your body!");
                    goto death;
                } else {
                    puts("It is now pitch dark.  If you proceed you will likely fall into a pit.");
                }
            } else {
                /* If the dwarf is active, he follows you.
                 * Notice that the dwarf will not immediately follow you
                 * if you sneak away in the dark; but as soon as you turn
                 * the lights back on, there he is again. */
                if (!there(DWARF, R_LIMBO) && !(places[loc].flags & (F_NOTINCAVE|F_NODWARF))) {
                    apport(DWARF, loc);
                }
                
                look_around(loc, familiar_place);
            }
            lamp_just_ran_out = false;
            if (loc == R_Y2 && pct(35))
                puts("A hollow voice says \"Plugh\".");
            if (there(GOBLINS, loc)) {
                objs[GOBLINS].prop += 1;
                if (objs[GOBLINS].prop == 7) goto death;
            }
            if (closure <= 2 && !(places[loc].flags & F_NOTINCAVE)) {
                /* Platt's code actually uses a global variable "K" to
                 * hold the clock-decrementing value. K is set to 2 or 3
                 * in the "lighted" branch of the code above, but if
                 * you're in the dark, the clock will run at an unpredictable
                 * rate depending on whether you came from a familiar place,
                 * and whether in the meantime you've been attacked by
                 * dwarves or jumped into the whirlpool (two other places
                 * where "K" is used for scratch space).
                 *   Technically, Platt's clock does not stop ticking during
                 * the endgame (closure >= 3), but he sets CLOCK to 999 when
                 * the cave closes, and MAX_GAME_TURNS is only 800, so unless
                 * you can reach the endgame in ~350 turns, you will never see
                 * the bug introduced by this shortcut. This code is cleaner
                 * if I don't preserve that bug, so I haven't. */
                clock -= (familiar_place ? 2 : 3);
                if (clock <= 0) {
                    if (clock4(loc)) {
                        /* We just closed the cave on you. */
                        continue;
                    }
                }
            }
            if (there(DWARF, loc)) {
                fleetfoot = 50;
                if (dwarf_attack()) goto death;
            }
            if (there(LAMP, R_YLEM) && loc == R_ROAD && closure < 4) {
                puts("\nThere's not much point in wandering around out here, and you can't\n"
                     "explore the cave without a lamp.  So let's just call it a day.");
                gave_up = true;
                finis();
            }
        }
        /* The player has now successfully either moved to a new room,
         * or been blocked from moving. */
        if (places[loc].flags & F_HINTABLE) {
            const int limit = (places[loc].flags & F_INMAZE) ? 50 : 30;
            hint_time += 1;
            if (hint_time > limit) {
                hint_logic(loc);
            }
        } else {
            hint_time = 0;
        }
        if (objs[BLOB].prop == 16) {
            /* Rover finally makes his appearance on the scene! */
            apport(BLOB, loc);
        }
        recount_inventory();
        fleetfoot = 25;  /* not running */
        backlash = 35;  /* dwarves not vengeful */
        listen();
        puts("");
        --foobar;
        /* Here Platt increments the variable TURNS. I've left it out because
         * I don't see anywhere its value is actually used. The value of
         * our variable "turns" is what Platt calls MOVES.
         * Notice that entering a blank command will cause time to pass.
         * Hold down enter long enough, and a dwarf will probably kill you. */
        if (word1.type == WordType_None)
            continue;
        if (word1.type == WordType_BadWord) {
            puts("Huh??");
            continue;
        }
        presay();
        if (word2.type == WordType_BadWord && !keywordv(SAY)) {
            printf("I don't know the word \"%s\".\n", word2.text);
            continue;
        }

        newloc = places[loc].f();
        if (R_LIMBO < newloc && newloc < R_YLEM) {
            continue;
        } else if (R_YLEM < newloc && newloc < 2*R_YLEM) {
            newloc -= R_YLEM;
            goto death;  /* moving that direction killed you */
        } else {
            assert(newloc == 0);  /* unhandled */
            newloc = loc;
        }

        if (word1.type == WordType_Place) {
            if (word1.meaning == loc) {
                puts("That's where you are now!");
            } else {
                puts("I don't know how to get there from here.");
            }
        } else {
            newloc = process_verb(loc);
            if (R_LIMBO < newloc && newloc < R_YLEM) {
                continue;
            } else if (R_YLEM < newloc && newloc < 2*R_YLEM) {
                newloc -= R_YLEM;
                goto death;  /* moving that direction killed you */
            } else {
                assert(newloc == 0);  /* unhandled */
                newloc = loc;
            }
            deal_with_syntax_errors(loc);
        }
    }
}

int main()
{
#ifdef Z_MACHINE
    puts("\n\n\n\n\n\n\n\n");
#endif /* Z_MACHINE */

    puts("Welcome to CP-6 Adventure!!   Say \"NEWS\" to get up-to-date\n"
         "game details as of 9/18/84.\n");

    if (yes("Would you like instructions?")) {
        puts("Somewhere nearby is Colossal Cave, where others have found fortunes in\n"
             "treasure and gold, though it is rumored that some who enter are never\n"
             "seen again.  Magic is said to work in the cave.  I will be your eyes\n"
             "and hands.  Direct me with commands of 1 or 2 words.\n\n"
             "Should you get stuck, type \"HELP\" for some general hints.  For infor-\n"
             "mation on how to end your adventure, etc., type \"INFO\".\n\n"
             "                              - - -\n\n");
    }
    puts("");
    build_vocabulary();
    build_place_names();
    build_object_table();
    simulate_an_adventure();
    return 0;
}

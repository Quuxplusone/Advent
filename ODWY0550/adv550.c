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
VerbWord verbosity_mode = FULL;  /* one of FAST, BRIEF, FULL */
bool please_clarify;
int turns;  /* how many times we've read your commands */
int foobar;  /* progress in the FEE FIE FOE FOO incantation */
int backlash = 35;  /* percent chance of dwarf attack */
int fleetfoot = 25;  /* player's modifier to dodge an attack */
int curclock, lastclock;
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
bool have_stolen_back_eggs = false;
bool panicked_at_grate = false;
bool gave_up;

/*========== Some common messages. ======================================*/

void ok(void) { if (verbosity_mode != FAST) puts("Ok."); }
void hah(void) { puts("Don't be ridiculous!"); }
void nothing_happens(void) { puts("Nothing happens."); }
void I_see_no(const char *s) { printf("I see no %s here.\n", s); }
void you_have_it(void) { puts("You are already carrying it!"); }
void You_have_no(const char *s) { printf("You have no %s!\n", s); }
void what_do(const char *s) { printf("What do you want me to do with the %s?\n", s); }
void it_is_dead(void) { puts("For crying out loud, the poor thing is already dead!"); }
void already_open(const char *s) { printf("The %s is already open!\n", s); }
void already_shut(const char *s) { printf("The %s is already closed!\n", s); }
void dunno_hao(const char *s) { printf("I don't know how to %s such a thing.\n", s); }
void dunno_how(void) { puts("I don't know how."); }
void say_what(void) { puts("Huh??"); }
void except_perhaps_you(void) { puts("There's nothing here it wants to eat (except perhaps you)."); }

/*========== Some forward declarations. =================================*/

bool is_treasure(ObjectWord o);
bool lamprey(Location loc);
void phog(Location loc);
void hint_logic(Location loc);
void look_around(Location loc, bool familiar_place);
int process_verb(Location loc);

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
    new_noun("volcano", GORGE); new_noun("geyser", GORGE); new_noun("gorge", GORGE);
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
    new_place_word("limbo", R_LIMBO);  /* Helpful to wizards. */
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
    /* Platt has "imbedded". */
    objs[TABLET].desc[0] = "A massive stone tablet embedded in the wall reads:\n"
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
    objs[GORGE].desc[2] = "There is a wheat-colored stone bridge arching over the gorge.";
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
        puts("");  /* ensure that the dwarf message is preceded by a blank line */
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
            default: puts("There is a nonexistent little dwarf in the room with you!"); assert(wizard_mode); break;
        }
    } else if (t == GOBLINS) {
        switch (objs[GOBLINS].prop) {
            case 0:
                puts("\nSuddenly and without warning, there appears from within the very\n"
                     "cave walls around you a horde of vicious little goblins!  Each one\n"
                     "stands about eight inches high on a pair of spindly black legs, has\n"
                     "a globular, spine-covered body resembling a giant gooseberry, a\n"
                     "wide mouth filled with sharp teeth, and a pair of glittering little\n"
                     "green eyes!  They swarm around you and try to block your way.\n");
                break;
            case 1:
                puts("You are surrounded by a horde of silent little gooseberry goblins!");
                break;
            case 2:
                puts("\nOne of the gooseberry goblins begins to giggle in a high-pitched\n"
                     "voice.  Another takes up the giggling, then another....  soon all\n"
                     "of them are giggling insanely and jumping up and down with glee!\n");
                break;
            case 3:
                puts("You are surrounded by a giggling horde of little gooseberry goblins!");
                break;
            case 4:
                puts("\nThe goblins are jumping up and down frantically, and are working\n"
                     "themselves into a real slavering frenzy!!\n");
                break;
            case 5:
                puts("You are surrounded by a slavering horde of gooseberry goblins!");
                break;
            case 6:
                puts("\nWith a shrill cry, the gooseberry goblins hurl themselves upon you,\n"
                     "tickling you mercilessly.  You crush and hurl away several of them,\n"
                     "but are soon borne down to the ground by the endless attack.  The\n"
                     "goblins then gleefully rip out your throat, and you sink into\n"
                     "unconsciousness.");
                break;
            default: assert(false);
        }
    } else if (t == SWORD) {
        switch (objs[SWORD].prop) {
            case 0:
                puts("There is a sword here with its blade plunged deep into the block\n"
                     "of stone.  The sword is singing quietly to itself.");
                break;
            case 1:
                puts("There is a magic sword here, chiming out the bell-like tones of\n"
                     "\"Khumbu Ice-fall\" by ringing its blade against the ground.");
                break;
            case 2:
                puts("There is a sword here, singing \"A Day in the Life\" in a quiet,\n"
                     "introspective voice.");
                break;
            case 3:
                puts("There is a magic sword here, singing \"Cold Blue Steel and Sweet\n"
                     "Fire\" to itself in a plaintive, hopeless voice.");
                break;
            case 4:
                puts("There is a sharp and obviously magical sword here. It is\n"
                     "quietly humming excerpts from Prokofiev's \"Romeo and Juliet\"\n"
                     "ballet to itself.");
                break;
            case 5:
                puts("There is a sword lying on the ground, jauntily whistling the\n"
                     "March from Tchaikovsky's \"Nutcracker Suite\".");
                break;
            case 6:
                puts("There is a sharp sword lying here. It is (somehow) singing\n"
                     "Tchaikovsky's \"1812 Overture\" in twelve parts, by itself!");
                break;
            case 7:
                puts("The stirring strains of Rossini's \"William Tell\" overture fill\n"
                     "the room, coming from a singing sword lying on the ground.");
                break;
            case 8:
                puts("There is a singing sword lying on the ground. From it resound the\n"
                     "massed voices of a two-hundred-singer choir, filling the air with\n"
                     "the stirring sound of the Hallelujah Chorus from Handel's \"Messiah\".");
                break;
            case 9:
                puts("There is a sharp and shiny sword here. It is somehow managing to\n"
                     "sing Harry Partch's \"Daphne of the Dunes\" without destroying its\n"
                     "singing organs (whatever they happen to be...)");
                break;
            case 10:
                puts("There is a sword here, singing \"Witchi-Tai-To\" in two-part harmony\n"
                     "with itself.");
                break;
            case 11:
                puts("There is a very strange singing sword here - it is glowing and\n"
                     "vibrating, and the eerie electronic notes of Charles Wuorinen's\n"
                     "\"Time's Encomium\" issue from its blade and fill the air.");
                break;
            default:
                assert(false);
        }
    } else if (t == SCULPTURE) {
        const int x = objs[SCULPTURE].prop;
        if (x == 0) {
            puts("A finely-carved crystalline sculpture of a pig is resting in a niche\n"
                 "melted out of the icy wall of the tunnel!");
        } else if (x == 14) {
            puts("There is a crude sculpture of a very bedraggled phoenix here!");
        } else {
            assert(1 <= x && x <= 13);
            static const char *animals[] = {
                "pig", "eel", "emu", "elf", "mouse", "rabbit", "ibex", "frog", "tiger",
                "mule", "moose", "dog", "spider"
            };
            const char *animal = animals[x-1];
            printf("There is a fine crystalline sculpture of a%s %s here!\n",
                   (animal[0]=='e' || animal[0] == 'i') ? "n" : "",
                   animal);
        }
    } else if (t == BLOB) {
        puts("There is an immense and unfriendly-looking blob in the room with you!");
        if (!wizard_mode) {
            assert(objs[BLOB].prop == 16);
        }
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
            case 8: break;  /* this is the fog's "non-puzzling" state, visible only in R_PLAIN_3 */
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
        if (*buffer == '\n' || tolower(*buffer) == 'y') {
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
    static bool reversed = false;
    char *q;
    bool got_a_command = false;
    saved_word = word1;

    if (p != NULL && *p == ',') {
        /* GET KEYS, LAMP does the expected thing.
         * GET KEYS, DROP KEYS is an error. */
        if (reversed) word1 = word2;
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
            for (q = word1.text; !isspace(*p) && *p != ','; ++p, ++q) {
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
    if (please_clarify && word1.type != WordType_None &&
            word1.type != WordType_BadWord && word2.type == WordType_None) {
        /* If we just asked the user for a verb or noun, we should
         * see if maybe they gave us one. */
        if ((word1.type == WordType_Verb) != (saved_word.type == WordType_Verb))
            word2 = saved_word;
    }
    /* BIRD GET works just as well as GET BIRD.
     * KEYS DROP, GET works just as well as DROP KEYS followed by GET KEYS. */
    reversed = false;
    if (word1.type != WordType_Verb && word2.type == WordType_Verb) {
        saved_word = word1;
        word1 = word2;
        word2 = saved_word;
        reversed = true;
    }
    please_clarify = false;
    puts("");
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
                    if (plural) {
                        puts("Your magic ring shoots a bolt of lightning that splinters the\n"
                             "knives into dust before they can reach you!");
                      
                    } else {
                        puts("Your magic ring shoots a bolt of lightning that splinters the\n"
                             "knife into dust before it can reach you!");
                    }
                    break;
                case 1:
                    if (plural) {
                        puts("A glowing disk of black fire jumps out of your magic ring and swallows\n"
                             "the hurtling knives before they can harm you!");
                    } else {
                        puts("A glowing disk of black fire jumps out of your magic ring and swallows\n"
                             "the hurtling knife before it can harm you!");
                    }
                    break;
                case 2:
                    if (plural) {
                        puts("Several balls of searing flame burst out of your magic ring, rebound\n"
                             "off of the ground, and vaporize the knives before they can reach you!");
                    } else {
                        puts("A ball of searing flame leaps out of your magic ring, bounces\n"
                             "off of the ground, and melts the knife before it can reach you!");
                    }
                    break;
                case 3:
                    if (plural) {
                        puts("Your magic ring emits a blast of some strange magical energy that\n"
                             "knocks the knives aside!");
                    } else {
                        puts("Your magic ring emits a blast of some strange magical energy that\n"
                             "knocks the knife aside!");
                    }
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
    /* Each of these messages begins and ends with a blank line. */
    switch (objs[BLOB].prop) {
        case 3: puts("\nFrom somewhere in the distance comes an ominous bubbling sound.\n"); break;
        case 4: puts("\nThe bubbling sound grows louder.\n"); break;
        case 5: puts("\nThe bubbling sound ends with a loud >splash<.\n"); break;
        case 6: puts("\nA hollow, echoing >ROAR< sounds in the distance.\n"); break;
        case 7: break;
        case 8: puts("\nA strange throbbing sound can be heard in the distance.\n"); break;
        case 9: break;
        case 10: puts("\nThe throbbing sound is growing louder.\n"); break;
        case 11: break;
        case 12: puts("\nThe source of the throbbing sound is approaching quickly.  Another\n"
                      "hollow >ROAR< echoes through the cave.\n"); break;
        case 13: break;
        case 14: puts("\nThere is a loud >ROAR< only a short distance away!!\n"); break;
        case 15: break;
        case 16:
            puts("\nInto view there bounces a horrible creature!!  Six feet across, it\n"
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
  
    curclock = 10+ran(10);
    lastclock = curclock = 10+ran(10);
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
    }, {
    "Do you need help getting out of here?",
        /* Platt's help text is less subtle than Woods'. */
        "You can make the passages look less alike by dropping things.  You\n"
        "could then make a map that would let you find your way around.", false
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
    } else if (R_SLIDE <= loc && loc <= R_ICECAVE30) {
        which_hint = 6;
    } else if (places[loc].flags & F_INMAZE) {
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
    for (int i = 1; i <= MAX_OBJ; ++i) {
        if (!is_treasure(i)) continue;
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
static int class_score[] = { 20, 130, 240, 350, 470, 510, 530, 550, 9999 };
static const char *class_message[] = {
    "You are obviously a rank amateur.  Better luck next time.",
    "Your score qualifies you as a novice-class adventurer.",
    /* Platt has an extraneous semicolon here: "rating;". */
    "You have achieved the rating \"Experienced Adventurer\".",
    "You may now consider yourself a \"Seasoned Adventurer\".",
    "You have reached \"Junior Master\" status.",
    "Your score puts you in Master Adventurer class C.",
    "Your score puts you in Master Adventurer class B.",
    "Your score puts you in Master Adventurer class A.",
    /* Woods has "Adventure Grandmaster". Personally, I like Platt's version better. */
    "All of Adventuredom gives tribute to you, Adventurer Grandmaster!"
};

void finis(void)
{
    /* Print the score and say adieu. */
    int s = score();
    printf("You have scored a total of %d points, out of a possible maximum of\n"
           "550 points.  During this game of Adventure, you have taken a total of\n"
           "%d turns.\n\n\n", s, turns);

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
    if (!yes("")) {
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
    curclock = 999;
    cylinder_escape_count = 0;
}

/*========== Verb processing. ===========================================*/

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

bool portable(ObjectWord o)
{
    return (objs[o].flags & F_PORTABLE);
}

bool openable(ObjectWord o)
{
    return (o == GRATE || o == DOOR || o == CLAM || o == OYSTER ||
            o == FLASK || o == CHAIN || o == SAFE || o == VIAL);
}

bool mortal(ObjectWord o)
{
    /* The GONG is "mortal" because it's a legitimate target for HIT. */
    return (o == TROLL || o == DWARF || o == DRAGON || o == SNAKE ||
            o == BLOB || o == BEAR || o == CLAM || o == OYSTER ||
            o == OGRE || o == BIRD || o == DJINN || o == GOBLINS ||
            o == BASILISK || o == GONG);
}

bool edible(ObjectWord o)
{
    return (o == FOOD || o == MUSHROOM);
}

bool is_treasure(ObjectWord o)
{
    return (BAG <= o && o <= CASKET) && (o != SHARDS);
}

/* Platt's kernel has a DEFAULT instruction that finds a nearby object
 * with the given attribute (portable, edible, mortal, or openable).
 * Objects in the player's inventory are ignored. This makes perfect sense
 * when the predicate is "portable"; GET works even when we're carrying
 * things. It's a little odd that KILL defaults to the bird when it's in
 * the room, but not when it's carried; and OPEN defaults to the clam in
 * the same manner. For EAT, see the comment in attempt_eat(). */
ObjectWord default_to_something(bool (*predicate)(ObjectWord), Location loc)
{
    int candidate = NOTHING;
    for (int i=1; i <= MAX_OBJ; ++i) {
        if (!there(i, loc)) continue;
        if (!predicate(i)) continue;
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
        ok();
    } else if (toting(CAGE)) {
        if (toting(ROD)) {
            puts("The bird was unafraid when you entered, but as you approach it becomes\n"
                 "disturbed and you cannot catch it.");
        } else {
            places[R_BIRD].flags &= ~F_HINTABLE;
            apport(BIRD, R_INHAND);
            objs[BIRD].prop = 1;  /* caged */
            ok();
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
               "skeleton raises its head and whispers \"Remember - %s!\" in a\n"
               /* Platt has "forboding". */
               "foreboding tone; it then sags to the ground and crumbles into dust which\n"
               "drifts away into the still air of the cave.\n", spelling);
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
            return STAY_STILL;
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
                       "here, maybe I'd be able to do better.  Sorry....\n", word2.text);
                return STAY_STILL;
            }
        }
    }
    if (word2.type == WordType_None) {
        obj = default_to_something(portable, loc);
    }
    if (keywordv(INVENTORY)) {
        attempt_inventory();
        return STAY_STILL;
    } else if (word2.type == WordType_Object) {
        switch (obj) {
            case BIRD:
                getbird(loc);
                return STAY_STILL;
            case OIL:
                if (loc == R_EPIT) {
                    getliquid(OIL);
                    return STAY_STILL;
                }
                break;
            case WATER:
                if (places[loc].flags & F_WATER) {
                    getliquid(WATER);
                    return STAY_STILL;
                }
                break;
            case CHAIN:
                if (there(CHAIN, loc) && objs[CHAIN].prop) {
                    puts("The chain is still locked.");
                    return STAY_STILL;
                }
                break;
            case BEAR:
                if (toting(BEAR)) {
                    puts("You are being followed by a very large, tame bear.");
                    return STAY_STILL;
                } else if (there(BEAR, loc) && (objs[BEAR].prop != 2)) {
                    puts("The bear is still chained to the wall.");
                    return STAY_STILL;
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
                    return STAY_STILL;
                }
                break;
            case SCEPTRE:
                if (there(SCEPTRE, loc) && !objs[SCEPTRE].prop && holding_count < strength) {
                    getsceptre();
                    return STAY_STILL;
                }
                break;
            case KNIFE:
                if (objs[AXE].flags & F_SEEN) {
                    puts("The dwarves' knives vanish as they strike the walls of the cave.");
                    return STAY_STILL;
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
        ok();
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
    return STAY_STILL;
}

int dropbird(Location loc)
{
    if (there(BIRD, loc)) {
        puts("You aren't carrying it!");
        return STAY_STILL;
    } else if (!toting(BIRD)) {
        I_see_no(word2.text);
        return STAY_STILL;
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
        ok();
    }
    return STAY_STILL;
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
            printf("The %s flies through the air and thoroughly drenches the dwarf.  He\n"
                   "shakes himself off and curses violently; he *REALLY* looks angry!\n", obj==OIL?"oil":"water");
        } else {
            printf("The %s flies through the air and thoroughly drenches the dwarves.  They\n"
                   "shake themselves off and curse violently; they *REALLY* look angry!\n", obj==OIL?"oil":"water");
        }
        dwarves_enraged = true;
    } else {
        puts("Your bottle is empty and the ground is wet.");
        /* THROW WATER doesn't dampen the ground; only POUR WATER does that. */
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
            case BIRD: dropbird(loc); return STAY_STILL;
            case OIL: if (dropliquid(obj, loc)) return STAY_STILL; break;
            case WATER: if (dropliquid(obj, loc)) return STAY_STILL; break;
            case DJINN: if (freedjinn(loc)) return STAY_STILL; break;
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
            return STAY_STILL;
        } else if (obj == VIAL && pct(10)) {
            puts("The vial strikes the ground and explodes with a violent >FOOM!<,\n"
                 "neatly severing your foot.  You bleed to death quickly and messily.");
            apport(VIAL, R_LIMBO);
            return you_are_dead_at(loc);
        }
        /* For bear/troll logic, see at_neofchasm(). Platt's code has a
         * subroutine named DROPBEAR, but it doesn't do anything special. */
        ok();
    } else {
        puts("You aren't carrying it!");
    }
    return STAY_STILL;
}

int weaponry(ObjectWord obj, Location loc)
{
    if (!toting(obj)) {
        You_have_no(word2.text);
        return STAY_STILL;
    }
    if (keywordv(THROW)) {
        apport(obj, loc);
    }
    if (there(DWARF, loc)) {
        /* Try to hit a dwarf with your axe or sword. */
        int hit_chance = 5*(strength - holding_count) + 15*objs[DWARF].prop + 30;
        if (keywordv(WAVE) != keywordo(AXE))
            hit_chance += 15;
        if (pct(hit_chance)) {
            if (pct(5) && !quipped) {
                printf("You have mortally injured a little dwarf with your %s.  The dwarf\n"
                       "staggers, slumps to the ground, gasps \"Epo'u mfu uifn vtf fncbmnjoh\n"
                       "gmvje.  J xbou up cf tuvggfe xjui dsbc nfbu!\" and then expires in\n"
                       "a puff of oily black smoke.\n", word2.text);
                quipped = true;
            } else {
                puts("You killed a little dwarf.  The body vanishes in a cloud of greasy\n"
                     "black smoke.");
            }
            objs[DWARF].prop -= 1;
            dwarfcount -= 1;
            if (objs[DWARF].prop == 0) {
                apport(DWARF, R_LIMBO);
            } else {
                fleetfoot = 0;
                backlash = 100;
            }
        } else {
            puts("You attack a little dwarf, but he dodges out of the way.");
            fleetfoot = 0;
            backlash = 100;
        }
    } else if (there(SNAKE, loc)) {
        puts("Attacking the snake both doesn't work and is very dangerous.");
        apport(obj, R_INHAND);
    } else if (there(DRAGON, loc)) {
        printf("The %s bounces harmlessly off the dragon's thick scales.\n", word2.text);
    } else if (here(BEAR, loc)) {
        if (objs[BEAR].prop == 0) {
            if (keywordv(THROW)) {
                if (obj == AXE) {
                    puts("The axe misses and lands near the bear where you can't get at it.");
                    objs[AXE].prop = 1;  /* immobilize it */
                } else {
                    puts("The sword misses the bear, bounces off of the wall, and lands at\n"
                         "your feet.");
                }
            } else if (pct(50)) {
                puts("The bear dodges away from your attack, growls, and swipes at you\n"
                     "with his claws.  Fortunately, he misses.");
            } else {
                puts("The bear snarls, ducks away from your attack and slashes you to death\n"
                     "with his claws.");
                return you_are_dead_at(loc);
            }
        } else {
            puts("The bear is confused; he only wants to be your friend.");
        }
        return STAY_STILL;
    } else if (there(TROLL, loc)) {
        puts("Trolls are close relatives with the rocks and have skin as tough as\n"
             "that of a rhinoceros.  The troll fends off your blows effortlessly.");
    } else if (there(OGRE, loc)) {
        if (keywordv(WAVE)) {
            /* Platt has "contemptously".
             * It's pretty evil that SWING SWORD is fatal, given that the
             * solution to this puzzle is the less plausible THROW SWORD. */
            printf("The ogre contemptuously catches the %s in mid-swing, rips it out\n"
                   "of your hands, and uses it to chop off your head.\n", word2.text);
            return you_are_dead_at(loc);
        } else if (obj == AXE) {
            puts("The ogre casually catches the axe in mid-air, braces his feet, winds\n"
                 "up and throws the axe straight back at you with incredible force.  You\n"
                 "are unable to dodge it and it chops you in half.");
            return you_are_dead_at(loc);
        } else {
            puts("The sword halts in mid-air, twirls like a dervish, and chants several\n"
                 /* Platt has "Dies Ire". */
                 "bars of \"Dies Irae\" in a rough tenor voice.  It then begins to spin\n"
                 "like a rip-saw blade and flies directly at the ogre, who attempts to\n"
                 "catch it without success;  it strikes him full on the chest.  There is\n"
                 "a brilliant flash of light, a deafening roar and a cloud of oily grey\n"
                 "smoke;  when the smoke clears (and your eyes begin working properly\n"
                 "again) you see that the ogre has vanished.  The sword is lying on the\n"
                 "ground, sparking and flaming.  Before your eyes it softens and melts,\n"
                 "writhes as if in pain, and shrinks rapidly until all that is left is\n"
                 "a small silvery ring which cools rapidly.");
            apport(SWORD, R_LIMBO);
            apport(OGRE, R_LIMBO);
            apport(RING, loc);
        }
    } else if (there(BLOB, loc)) {
        printf("The %s cuts through the blob's body (?) without harming it.\n", word2.text);
    } else if (there(BASILISK, loc)) {
        if (objs[BASILISK].prop >= 2) {
            it_is_dead();
            apport(obj, R_INHAND);
        } else {
            printf("The %s rebounds harmlessly from the basilisk's tough scales.  The\n"
                   "basilisk awakens, grunting in shock, and glares at you.  You are\n"
                   "instantly turned into a solid rock statue (and not a particularly\n"
                   "impressive one, at that).\n", word2.text);
            return you_are_dead_at(loc);
        }
    } else if (there(DJINN, loc)) {
        printf("The %s rebounds harmlessly from the pentagram's magic force\n"
               "field.  It's just as well - the djinn doesn't seem dangerous.\n",
               word2.text);
    } else if (there(GOBLINS, loc)) {
        printf("You kill several of the gooseberry goblins with your %s, but\n"
               "the others swarm at you, force you to the ground, and rip out\n"
               "your throat.\n", word2.text);
        return you_are_dead_at(loc);
    } else {
        /* We're just aimlessly throwing/swinging this object. */
        if (keywordv(THROW)) {
            apport(obj, R_INHAND);
            return 0;  /* not yet handled */
        } else {
            ok();
        }
    }
    return STAY_STILL;
}

bool throw_food(Location loc)
{
    if (!toting(FOOD)) {
        return false;  /* unhandled */
    } else if (there(TROLL, loc)) {
        puts("Gluttony is not one of the troll's vices.  Avarice, however, is.");
    } else if (there(DWARF, loc)) {
        puts("You fool, dwarves eat only coal!  Now you've made him *really* mad!!");
        apport(FOOD, loc);
        dwarves_enraged = true;
    } else if (there(BEAR, loc)) {
        puts("The bear eagerly wolfs down your food, after which he seems to calm\n"
             "down considerably and even becomes rather friendly.");
        apport(FOOD, R_LIMBO);
        objs[BEAR].prop = 1;
        if (objs[AXE].prop)
            objs[AXE].prop = 0;  /* re-mobilize the axe */
    } else {
        return false;  /* unhandled */
    }
    return true;
}

int break_vial(Location loc)
{
    apport(VIAL, R_LIMBO);
    puts("The vial explodes into splinters and disintegrates, releasing an\n"
         "oily liquid which rapidly sublimes into a large mushroom-shaped cloud");
    switch (ran(7)) {
        case 0: puts("of pale puce vapor smelling like sequoia sap and ozone."); break;
        case 1: puts("of bright green vapor smelling like pine needles and sea water."); break;
        case 2: puts("of thick yellow vapor smelling like cheddar cheese and bananas."); break;
        case 3: puts("of choking green vapor smelling like chlorine and apples."); break;
        case 4: puts("of misty white vapor with no scent."); break;
        case 5: puts("of nearly-invisible vapor with a strong odor of walnuts and onions."); break;
        case 6: puts("of ominously glowing vapor smelling of hot iron."); break;
    }
    puts("");
    if (there(DWARF, loc)) {
        if (objs[DWARF].prop == 1) {
            puts("The dwarf catches a lungful of the cloud, gags audibly and stumbles\n"
                 "out of the room retching, sneezing, and cursing up a storm.");
        } else {
            /* Platt has "blanche". */
            puts("The dwarves blanch in horror and dash away as fast as their brief\n"
                 "and misshapen legs can carry them.");
        }
        apport(DWARF, R_LIMBO);
        dwarfcount -= objs[DWARF].prop;
        objs[DWARF].prop = 0;
    }
    if (there(TROLL, loc)) {
        puts("The troll calmly waves the vapor away.");
    }
    if (there(BEAR, loc)) {
        if (objs[BEAR].prop) {
            puts("The bear inhales some of the vapor and moans appreciatively.");
        } else {
            puts("The bear inhales some of the vapor and snarls angrily.");
        }
    }
    if (there(SNAKE, loc)) {
        puts("The snake completely ignores the vapor.");
    }
    if (there(BIRD, loc)) {
        puts("The bird breathes in a miniscule amount of the vapor and immediately\n"
             "sings a twenty-second segment of Morton Subotnik's \"Sidewinder\".");
    }
    if (there(SLIME, loc)) {
        puts("The slime filling the passage to the south blackens and shrivels\n"
             "away into nothingness.");
        apport(SLIME, R_LIMBO);
    }
    if (there(DRAGON, loc) && objs[DRAGON].prop == 0) {
        puts("The dragon sniffs the air, rumbles deep in his chest, and shoots out\n"
             "a small puff of flame that ignites and incinerates the vapor.");
    }
    if (there(DJINN, loc)) {
        /* Platt has "TRUELY". */
        puts("The djinn takes a deep sniff of the vapor and comments, \"AH!  A TRULY\n"
             "FINE ARABIAN PERFUME!  I HAVEN'T SMELLED ANYTHING LIKE THAT FOR YEARS!\"");
    }
    if (there(BASILISK, loc) && objs[BASILISK].prop <= 1) {
        puts("The basilisk doesn't wake up.");
    }
    if (there(GOBLINS, loc)) {
        puts("The gooseberry goblins sniff the vapor, screech in terror, and dash off\n"
             "into the distance.");
        apport(GOBLINS, R_LIMBO);
    }
    return STAY_STILL;
}

bool upchuck(ObjectWord obj, Location loc)
{
    static const char pit[] = "down into the pit";
    static const char fissure[] = "down into the fissure";
    static const char hole[] = "down into the hole in the floor";
    static const char room[] = "down into the room below you";
    static const char canyon[] = "down into the canyon beneath your feet";
    static const char whirlpool[] = "into the middle of the whirlpool";
    static const char cavern[] = "down into the mist filling the cavern";
    static const char chasm[] = "down into the misty bottom of the chasm";
    static const char gorge[] = "down into the boiling lava at the bottom\nof the volcanic gorge";
    static const char chimney[] = "down the chimney";
    static const char tube[] = "down into the lava tube and out of sight";
    static const char steps[] = "down the steps and out of view";
    static const char slide[] = "down the icy slide and out of sight";
    static const char beach[] = "down onto the beach";
    static const char reservoir[] = "into the center of the reservoir";
    Location newloc = loc;
    const char *tossed = "tossed";
    const char *how = NULL;
    switch (loc) {
        case R_SPIT: newloc = R_EMIST; how = pit; break;
        case R_EFISS: case R_WFISS: newloc = R_FALLS; tossed = "thrown"; how = fissure; break;
        case R_W2PIT: newloc = R_WPIT; how = pit; break;
        case R_E2PIT: newloc = R_EPIT; how = pit; break;
        case R_NS: newloc = R_DIRTY; how = hole; break;
        case R_WINDOE: case R_WINDOW: newloc = R_MIRROR; how = pit; break;
        case R_CLEAN: newloc = R_WET; how = pit; break;
        case R_DUSTY: newloc = R_COMPLEX; how = hole; break;
        case R_MAZEA57_PIT: newloc = R_BIRD; how = pit; break;
        case R_ABOVER: newloc = R_SLAB; how = room; break;
        case R_ABOVEP: newloc = R_BEDQUILT; how = room; break;
        case R_SECRETEW_TITE: newloc = R_NSCANYONWIDE; how = canyon; break;
        case R_INCLINE: newloc = R_LOW; how = room; break;
        case R_FALLS: newloc = R_YLEM; tossed = "hurled"; how = whirlpool; break;
        case R_MISTY: newloc = R_FALLS; tossed = "thrown"; how = cavern; break;
        case R_STALACT: newloc = R_MAZEA53; how = room; break;
        case R_RES: case R_RESERVOIR_N: newloc = R_YLEM; tossed = "negligently tossed"; how = reservoir; break;
        case R_BALCONY: newloc = R_YLEM; how = room; break;
        case R_SWOFCHASM: case R_NEOFCHASM: newloc = R_YLEM; tossed = "hurled"; how = chasm; break;
        case R_VIEW: case R_FACES: case R_PLATFORM: newloc = R_YLEM; tossed = "hurled"; how = gorge; break;
        case R_TUBE: newloc = R_CHIMNEY; how = chimney; break;
        case R_TUBE_SLIDE: newloc = R_PLAIN_1; how = tube; break;
        case R_BASQUE_FORK: newloc = R_ON_STEPS; tossed = "thrown"; how = steps; break;
        case R_ON_STEPS: newloc = R_STEPS_EXIT; tossed = "thrown"; how = steps; break;
        case R_STEPS_EXIT: newloc = R_STORAGE; tossed = "thrown"; how = steps; break;
        case R_BRINK_1: case R_BRINK_2: case R_BRINK_3: newloc = R_YLEM; how = pit; break;
        case R_ICE: newloc = R_SLIDE; how = slide; break;
        case R_SHELF: newloc = R_BEACH; tossed = "thrown"; how = beach; break;
    }
    if (how == NULL) return false;  /* not at an F_THROWER location */
    if (obj == BEAR) return false;  /* can't THROW BEAR into the chasm! */
    printf("You have %s the %s %s.\n", tossed, word2.text, how);
    apport(obj, newloc);
    if (obj == VASE) {
        apport(VASE, R_YLEM);
        apport(SHARDS, newloc);
        if (newloc != R_YLEM)
            puts("A delicate crash sounds from below.");
    } else if (obj == BOTTLE) {
        apport(OIL, R_LIMBO);
        apport(WATER, R_LIMBO);
    } else if (obj == WATER || obj == OIL) {
        objs[BOTTLE].prop = 1;  /* threw away the liquid */
        apport(obj, R_LIMBO);
    } else if (obj == CAGE) {
        if (toting(BIRD)) apport(BIRD, newloc);
    } else if (obj == LAMP && objs[LAMP].prop && now_in_darkness(loc)) {
        puts("It is now pitch dark.  If you proceed you will likely fall into a pit.");
    } else if (obj == BIRD) {
        objs[BIRD].prop = 0;  /* free the bird */
    }
    return true;
}

int attempt_throw(ObjectWord obj, Location loc)
{
    if (word2.type == WordType_Object) {
        switch (obj) {
            case AXE:
            case SWORD: {
                int newloc = weaponry(obj, loc);
                if (newloc != 0) return newloc;
                break;  /* go on to the following cases */
            }
            case FOOD:
                if (throw_food(loc)) return STAY_STILL;
                break;  /* unhandled so far */
            case TEETH:
                if (toting(TEETH)) {
                    objs[TEETH].prop = 0;  /* "scattered" */
                    if (there(GOBLINS, loc)) {
                        puts("\nAs each of the dragon's teeth strikes the ground, a fully-armed human\n"
                             "skeleton springs up from where it struck and leaps to your defense!\n"
                             "The skeletal warriors attack the vicious gooseberry goblins and drive\n"
                             "them away in screaming panic;  they then salute you with their ancient\n"
                             "and rusty swords, and fade silently into nothingness.\n");
                        apport(TEETH, R_LIMBO);
                        apport(GOBLINS, R_LIMBO);
                        return STAY_STILL;
                    }
                }
                break;  /* unhandled or only partly handled */
            case VIAL:
                if (toting(VIAL)) return break_vial(loc);
                break;  /* unhandled */
        }
    }
    if (word2.type == WordType_Object && toting(obj)) {
        if (upchuck(obj, loc)) return STAY_STILL;
    }
    if (word2.type != WordType_None) {
        word1.meaning = DROP;
        strcpy(word1.text, "drop");
        return process_verb(loc);
    } else {
        return 0;  /* unhandled */
    }
}

bool is_walkable_direction(VerbWord v)
{
    switch (v) {
        case NORTH: case SOUTH: case EAST: case WEST:
        case NE: case NW: case SE: case SW:
        case UP: case DOWN: case IN: case OUT:
        case BACK: case CAVE:
            return true;
        default:
            return false;
    }
}

void wave_rod(Location loc)
{
    if (there(FISSURE, loc) && closure < 2) {
        if (objs[FISSURE].prop) {
            puts("The crystalline bridge has vanished!");
            objs[FISSURE].prop = 0;
            objs[FISSURE].flags |= F_INVISIBLE;
        } else {
            puts("A crystalline bridge now spans the fissure.");
            objs[FISSURE].prop = 1;
            objs[FISSURE].flags &= ~F_INVISIBLE;
        }
    } else if (there(GORGE, loc) && closure < 2) {
        if (objs[GORGE].prop) {
            puts("The earth shudders violently, and steam blasts upwards from the geyser.\n"
                 "The wheat-stone bridge cracks and splits, and the fragments fall into\n"
                 "the gorge.");
            objs[GORGE].prop = 0;
            objs[GORGE].flags |= F_INVISIBLE;
        } else {
            puts("The earth begins to shudder violently, and smoke flows up from the\n"
                 "gorge beneath your feet.  With a violent >GLOP!<, the volcano\n"
                 "belches out an immense blast of molten lava which flies into the\n"
                 "air above the gorge and suddenly solidifies into a fragile-looking arch\n"
                 "of wheat-colored stone that bridges the gorge.");
            objs[GORGE].prop = 2;
            objs[GORGE].flags &= ~F_INVISIBLE;
        }
    } else if (there(QUICKSAND, loc)) {
        /* Notice that the rod bridges the quicksand regardless of
         * the cave's closure state. This is probably unintentional. */
        puts("Nothing obvious happens.");
        objs[QUICKSAND].prop = 1;
    } else {
        nothing_happens();
    }
}

int no_move_possible(Location loc)
{
    if (now_in_darkness(loc)) {
        if (pct(25)) {
            puts("You fell into a pit and broke every bone in your body!");
            return you_are_dead_at(loc);
        } else {
            puts("You have run into a wall of rock and can go no further in this\n"
                 "direction.");
        }
    } else {
        const char *dirs[8] = {
            "east", "west", "north", "south", "northeast", "southeast",
            "northwest", "southwest"
        };
        VerbWord dir_verb = (keywordv(WALK) ? word2.meaning : word1.meaning);
        const char *dir = (dir_verb == UP) ? "up" :
                          (dir_verb == DOWN) ? "down" :
                          dirs[dir_verb - EAST];
        switch (ran(4)) {
            case 0: printf("I'm sorry, but there's no way you can go %s from here.\n", dir); break;
            case 1: printf("There is no way for you to go %s.\n", dir); break;
            case 2: printf("You cannot go %s.\n", dir); break;
            case 3: printf("Going %s is not possible from here.  Sorry.\n", dir); break;
        }
    }
    if (lamprey(loc)) {
        return R_CYLINDRICAL;  /* the cave is now closed */
    }
    return STAY_STILL;
}

int examine_mist(void)
{
    puts("Mist is a white vapor, usually water, seen from time to time in\n"
         "caverns.  It can be found anywhere but is frequently a sign of a deep\n"
         "pit leading down to water.");
    return STAY_STILL;
}

int examine_trees(void)
{
    /* Platt's version contains a verb TREE,TREES, and a message labeled
     * INFOREST containing the familiar message from Woods' Adventure;
     * but at least in the version I have, the verb is not implemented
     * and the message is not referenced. This is certainly a bug; I'm
     * taking the liberty of fixing it here. */
    puts("The trees of the forest are large hardwood oak and maple, with an\n"
         "occasional grove of pine or spruce.  There is quite a bit of under-\n"
         "growth, largely birch and ash saplings plus nondescript bushes of\n"
         "various sorts.  This time of year visibility is quite restricted by\n"
         "all the leaves, but travel is quite easy if you detour around the\n"
         "spruce and berry bushes.");
    return STAY_STILL;
}

int attempt_open(Location loc)
{
    if (word2.type == WordType_None) {
        default_to_something(openable, loc);
    }
    if (keywordo(GRATE) && there(GRATE, loc)) {
        if (!toting(KEYS)) {
            puts("You have no keys!");
        } else if (closure >= 2 || nomagic) {
            puts("The lock seems to be stuck - the key refuses to turn!");
            if (closure >= 2) {
                panicked = true;
                if (!panicked_at_grate) {
                    puts("\nA mysterious recorded voice groans into life and announces:\n"
                         "   \"This exit is closed.  Please report to the treasure room via\n"
                         "   the alternate entrance to claim your treasure.\"\n");
                    panicked_at_grate = true;
                }
            }
        } else {
            puts("The grate is now unlocked.");
            objs[GRATE].prop = 1;
            places[R_DEPRESSION].flags &= ~F_HINTABLE;
        }
    } else if (keywordo(CHAIN) && here(CHAIN, loc)) {
        if (!toting(KEYS)) {
            puts("You have no keys!");
        } else if (!objs[CHAIN].prop) {
            /* Platt provides a message "It was already unlocked.", but doesn't
             * refer to it anywhere. This is probably a bug. */
            puts("The chain is now unlocked.");
        } else if (objs[CHAIN].prop == 1) {
            if (objs[BEAR].prop == 0) {
                puts("There is no way to get past the bear to unlock the chain, which is\n"
                     "probably just as well.");
            } else {
                puts("The chain is now unlocked.");
                objs[CHAIN].prop = 0;
                objs[BEAR].prop = 2;
            }
        } else {
            puts("The chain is now unlocked.");
            objs[CHAIN].prop = 0;
        }
    } else if (keywordo(DOOR) && there(DOOR, loc)) {
        puts("It has no lock.");
    } else if (keywordo(KEYS)) {
        /* Platt forgets to check NEAR KEYS; this is certainly a bug. */
        puts("You can't unlock the keys.");
    } else if (keywordo(SAFE) && there(SAFE, loc)) {
        if (objs[SAFE].prop == 0) {
            puts("The door to the safe has no keyhole, dial, or handle - I can't figure\n"
                 "out how to open it!");
        } else if (objs[SAFE].prop == 1) {
            already_open(word2.text);
        } else {
            puts("The door to the safe seems to be fused shut - I can't open it.");
        }
    } else if (keywordo(CLAM) && here(CLAM, loc)) {
        if (toting(CLAM)) {
            puts("I advise you to put down the clam before opening it.  >strain!<");
        } else if (toting(TRIDENT)) {
            apport(CLAM, R_LIMBO);
            apport(OYSTER, loc);
            apport(PEARL, R_CULDESAC);
            puts("A glistening pearl falls out of the clam and rolls away.  Goodness,\n"
                 "this must really be an oyster.  (I never was very good at identifying\n"
                 "bivalves.)  Whatever it is, it has now snapped shut again.");
        } else {
            puts("You don't have anything strong enough to open the clam.");
        }
    } else if (keywordo(OYSTER) && here(OYSTER, loc)) {
        if (toting(OYSTER)) {
            puts("I advise you to put down the oyster before opening it.  >wrench!<");
        } else if (toting(TRIDENT)) {
            puts("The oyster creaks open, revealing nothing but oyster inside.  It\n"
                 "promptly snaps shut again.");
        } else {
            puts("You don't have anything strong enough to open the oyster.");
        }
    } else if (keywordo(VIAL) && here(VIAL, loc)) {
        return break_vial(loc);
    } else if (keywordo(FLASK) && here(FLASK, loc)) {
        if (objs[FLASK].prop <= 1) {
            if (there(FLASK, R_PENTAGRAM)) {
                apport(DJINN, R_PENTAGRAM);
                puts("The wax seal breaks away easily.  A cloud of dark smoke pours up from\n"
                     "the mouth of the flask and condenses into the form of a twelve-foot\n"
                     "Djinn standing in the pentagram.  He pushes experimentally at the\n"
                     "magical wall of the pentagram (which holds), and nods politely to\n"
                     "you.  \"MY THANKS, OH MORTAL,\" he says in an incredibly deep bass\n"
                     "voice.  \"IT HAS BEEN THREE THOUSAND YEARS SINCE SOLOMON SEALED ME\n"
                     "INTO THAT BOTTLE, AND I AM GRATEFUL THAT YOU HAVE RELEASED ME.  IF\n"
                     "YOU WILL OPEN THIS PENTAGRAM AND LET ME GO FREE, I WILL GIVE YOU SOME\n"
                     "ADVICE THAT YOU MAY ONE DAY WISH TO POSSESS.\"");
            } else {
                puts("The flask's wax seal crumbles at your touch.  A large cloud of black\n"
                     "smoke pours out, solidifying into the form of a twelve-foot Djinn.\n"
                     "\"AT LAST!\" he says in an earth-shaking voice, \"I KNEW THAT SOMEDAY\n"
                     "SOMEONE WOULD RELEASE ME!  I WOULD REWARD YOU FOR THIS, MORTAL, BUT\n"
                     "IT HAS BEEN THREE THOUSAND YEARS SINCE I HAD A SOLID MEAL, AND I'M\n"
                     "NOT GOING TO STAND HERE CHATTERING WHEN I COULD BE OUT EATING A SIX-\n"
                     "INCH SIRLOIN STEAK.  FAREWELL.\"  With that, he somewhat rudely explodes\n"
                     "back into smoke and drifts quickly out of sight.");
            }
            objs[FLASK].prop = 2;
        } else {
            already_open(word2.text);
        }
    } else if (keywordp(R_PENTAGRAM)) {
        if (loc == R_PENTAGRAM) {
            if (there(DJINN, loc)) {
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
            } else {
                puts("The pentagram is empty - there's nothing to let out!");
            }
        } else {
            I_see_no("pentagram");
        }
    } else {
        return 0;  /* unhandled */
    }
    return STAY_STILL;  /* it must have been handled somewhere above */
}

int attempt_close(Location loc)
{
    if (word2.type == WordType_None) {
        default_to_something(openable, loc);
    }
    if (keywordo(GRATE) && there(GRATE, loc)) {
        objs[GRATE].prop = 0;
        puts("The grate is now locked.");
    } else if (keywordo(CHAIN) && here(CHAIN, loc)) {
        if (loc == R_BARR) {
            apport(CHAIN, loc);  /* drop it */
            if (objs[CHAIN].prop == 0) {
                if (here(BEAR, loc)) {
                    apport(BEAR, loc);
                    objs[BEAR].prop = 1;
                    objs[CHAIN].prop = 1;
                } else {
                    objs[CHAIN].prop = 2;
                }
            }
            puts("The chain is now locked.");
        } else {
            puts("There is nothing here to which the chain can be locked.");
        }
    } else if (keywordo(DOOR) && there(DOOR, loc)) {
        puts("It has no lock.");
    } else if (keywordo(SAFE) && there(SAFE, loc)) {
        if (objs[SAFE].prop == 1) {
            puts(">screeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeech<\n"
                 "                                              >ker-CHUNK!<");
            objs[SAFE].prop = 0;
        } else {
            already_shut(word2.text);
        }
    } else if (keywordo(FLASK) && here(FLASK, loc)) {
        if (objs[FLASK].prop == 0) {
            already_shut(word2.text);
        } else {
            dunno_hao(word1.text);
        }
    } else if (keywordv(CAVE) && wizard_mode) {
        /* CLOSE CAVE works for wizards! But since this bypasses the clock4()
         * code that re-locks the grate, the wizard will be able to explore
         * the cave after it's been closed. */
        close_the_cave();
        return R_CYLINDRICAL;
    } else {
        return 0;  /* unhandled */
    }
    return STAY_STILL;  /* it must have been handled somewhere above */
}

void attempt_help(Location loc)
{
    if (hint_time == 0)
        hint_time = 1;
    hint_logic(loc);
    if (hint_time > 0) {
        /* We didn't find a hint for this location. */
        if (there(DWARF, loc) || there(DRAGON, loc) || there(TROLL, loc) ||
                there(OGRE, loc) || there(QUICKSAND, loc) || there(BASILISK, loc) ||
                objs[BLOB].prop || objs[GOBLINS].prop) {
            puts("Well, you do seem to be having problems, don't you?  Unfortunately,\n"
                 "I'm not allowed to help you out;  you got yourself into this, and\n"
                 "you'll have to find a solution to your problems yourself.");
        } else {
            static bool been_here_before = false;
            if (been_here_before) {
                puts("I'm afraid that I can't tell you anything more that would be useful;\n"
                     "you'll have to figure out what to do on your own.");
            } else {
                puts("\n"
                     "I know of places, actions, and things.  Most of my vocabulary\n"
                     "describes places and is used to move you there.  To move, try words\n"
                     "like FOREST, BUILDING, DOWNSTREAM, ENTER, EAST, WEST, NORTH, SOUTH,\n"
                     "UP, or DOWN.  I know about a few special objects, like a black rod\n"
                     "hidden in the cave.  These objects can be manipulated using some of\n"
                     "the action words that I know.  Usually you will need to give both the\n"
                     "object and action words (in either order), but sometimes I can infer\n"
                     "the object from the verb alone.  Some objects also imply verbs; in\n"
                     "particular, \"INVENTORY\" implies \"TAKE INVENTORY\", which causes me to\n"
                     "give you a list of the things you're carrying (you can say just\n"
                     "\"INV\" instead of \"INVENTORY\" if you wish).  The objects have side\n"
                     "effects; for instance, the rod scares the bird.  Usually people having\n"
                     "trouble moving just need to try a few more words.  Usually people\n"
                     "trying unsuccessfully to manipulate an object are attempting something\n"
                     "beyond their (or my!) capabilities and should try a completely\n"
                     "different tack.  To speed the game you can sometimes move long\n"
                     "distances with a single word.  For example, \"BUILDING\" usually gets\n"
                     "you to the building from anywhere above ground except when lost in the\n"
                     "forest.  Also, note that cave passages turn a lot, and that leaving a\n"
                     "room to the north does not guarantee entering the next from the south.\n"
                     "Good luck!\n");
                been_here_before = true;
            }
        }
    }
}

void attempt_info(void)
{
    puts("If you want to end your adventure early, say \"QUIT\".\n"
         "To suspend your adventure such that you can continue later, say\n"
         "\"SUSPEND\" (or \"PAUSE\" or \"SAVE\").  To re-start your game at a later\n"
         "time, start up a new adventure and after I say \"You are standing...\",\n"
         "you must say \"RESTORE\".  You can also name your game by saying\n"
         "\"SUSPEND mine\" (and \"RESTORE mine\") where \"mine\" is the name that\n"
         "you wish your suspended name to be known by (1-12 characters).\n"
         "To see what hours the cave is normally open, say \"HOURS\".\n"
         "To see how well you're doing, say \"SCORE\".  To get full credit for a\n"
         "treasure, you must have left it safely in the building, though you get\n"
         "partial credit just for locating it.  You lose points for getting\n"
         "killed, or for quitting, though the former costs you more.  There are\n"
         "also points based on how much (if any) of the cave you've managed to\n"
         "explore; in particular, there is a large bonus just for getting in (to\n"
         "distinguish the beginners from the rest of the pack), and there are\n"
         "other ways to determine whether you've been through some of the more\n"
         "harrowing sections.  If you think you've found all the treasures, just\n"
         "keep exploring for a while.  If nothing interesting happens, you\n"
         "haven't found them all yet.  If something interesting *does* happen,\n"
         "it means you're getting a bonus and have an opportunity to garner many\n"
         "more points in the master's section.  I may occasionally offer hints\n"
         "if you seem to be having trouble.  If I do, I'll warn you in advance\n"
         "how much it will affect your score.  To save paper, you may specify\n"
         "\"BRIEF\", which tells me never to repeat the full description of a\n"
         "place unless you explicitly ask me to by saying \"LOOK\".  If you\n"
         "are an experienced adventurer, you may wish to specify \"FAST\", which\n"
         "is like \"BRIEF\" but more so;  in \"FAST\" mode I will *never* under\n"
         "any circumstances give the full description of a place.  Finally, if\n"
         "you are in \"BRIEF\" or \"FAST\" modes, you may return to the normal mode\n"
         "of operation by saying \"FULL\".\n");
}

void attempt_news(void)
{
    puts("This version of Adventure is based on a version by David Platt.\n"
         "Here is the NEWS command as presented by that version:\n\n"
         "9/18/84 - A set of maps which describe Colossal Cave may be ordered\n"
         "from the following address:\n\n"
         "         David Platt\n"
         "         c/o Honeywell Information Systems\n"
         "         5250 West Century Boulevard\n"
         "         Los Angeles, CA  90045\n\n"
         "There is no charge for the maps;  however, each request MUST include\n"
         "a large (legal-sized or better) self-addressed envelope with at least\n"
         "two first-class stamps.  Requests received without envelopes, or with\n"
         "insufficient return postage, will be tossed into a bottomless pit and\n"
         "never heard from again.\n\n"
         "The set of maps consists of a two-page map of the main cave, plus\n"
         "detail maps of the mazes, tunnels, and other complex areas.\n"
         "The locations of various treasures and tools are given in the main\n"
         "map... however, the maps do not reveal ways past the cave's obstacles.\n\n"
         "1/19/82 - Based on many requests, the SAVE/RESTORE logic has been\n"
         "modified in a way that makes exploration of dangerous sections of the\n"
         "cave rather easier.  Three changes are effective:  (1) you need not\n"
         "wait 30 minutes after SAVE'ing a game to RESTORE it;  (2) when you\n"
         "have SAVE'd a game, you will have the chance to continue with your\n"
         "exploration immediately;  (3) when you RESTORE a game, you will have\n"
         "the option to keep the saved game on disk, and can thus RESTORE it\n"
         "an unlimited number of times.\n\n"
         "A minor bug in the pirate logic has been fixed.\n\n"
         "10/18/79 - Several problems have been fixed in this version of\n"
         "Adventure.  Both the new run unit and the new database file should\n"
         "be installed at the same time.  (1)  The interpreter trap during\n"
         "\"RESTORE\" operations has been fixed.  (2)  The 'vanishing fog' problem\n"
         "has been corrected.  (3)  If you pour water or oil on the ground, the\n"
         "ground will remain wet (although you can't get the water or oil back\n"
         "by mopping up the floor or any such thing).  (4)  Games saved under\n"
         "previous versions of the database cannot be restored using this\n"
         "version due to some slight changes in the object status controls.\n\n"
         "Suggestions are being entertained for possible additions to the\n"
         "cave.  If you have any interesting ideas, please write them down\n"
         "and mail them to Dave Platt, c/o Honeywell, 5250 W. Century Blvd,\n"
         "Los Angeles CA 90254, or give me a call at (213)216-6232.\n");
}

bool with_your_bare_hands_p(void)
{
    if (yes("With your bare hands??"))
        return true;
    printf("That may be just as well.  If you do insist upon trying to %s\n"
           "the %s, you might be well advised to use a weapon of some sort.\n",
           word1.text, word2.text);
    return false;
}

int kill_dwarf(Location loc)
{
    if (with_your_bare_hands_p()) {
        int chance = 20 + 10*(strength - holding_count);
        if (pct(chance)) {
            puts("You killed a little dwarf.");
            objs[DWARF].prop -= 1;
            dwarfcount -= 1;
            if (objs[DWARF].prop == 0) {
                apport(DWARF, R_LIMBO);  /* no more dwarves in this room */
            }
        } else if (pct(chance)) {
            puts("You attack a little dwarf, but he dodges out of the way.");
        } else {
            puts("You attack a little dwarf, but he dodges out of the way and stabs\n"
                 "you with his nasty sharp knife!");
            return you_are_dead_at(loc);
        }
    }
    return STAY_STILL;
}

int kill_dragon(Location loc)
{
    if (objs[DRAGON].prop) {
        it_is_dead();
    } else if (with_your_bare_hands_p()) {
        puts("Congratulations! You have just vanquished a dragon with your bare\n"
             "hands!  (Unbelievable, isn't it?)");
        objs[DRAGON].prop = 2;
        apport(RUG, R_SECRETCYNNE1);
        apport(TEETH, R_SECRETCYNNE1);
        objs[DRAGON].flags &= ~F_SCHIZOID;
        for (ObjectWord i=1; i < MAX_OBJ; ++i) {
            /* This condition isn't quite correct. If you drop something at
             * R_SECRETCYNNE2 and then circle around to R_SECRETCYNNE1 before
             * killing the dragon, you'll find that the dropped object has
             * become completely inaccessible: might as well be in R_YLEM.
             * This is certainly a bug in Platt's code. */
            if (there(i, loc))
                apport(i, R_SECRETCYNNE1);
        }
        return R_SECRETCYNNE1;
    }
    return STAY_STILL;
}

int kill_ogre(Location loc)
{
    if (with_your_bare_hands_p()) {
        if (pct(50)) {
            puts("You attack the ogre, but he fends off your attack easily and comes\n"
                 "very close to crushing your skull with *his* bare (but extremely\n"
                 "strong) hands.  You are forced to retreat in disgrace.");
        } else {
            puts("You attack the ogre - a brave but foolish action.  He quickly grabs\n"
                 "you and with a heave of his mighty arms rips your body limb from limb.");
            return you_are_dead_at(loc);
        }
    }
    return STAY_STILL;
}

int attempt_kill(Location loc)
{
    if (word2.type == WordType_None) {
        default_to_something(mortal, loc);
    }
    ObjectWord obj = word2.meaning;
    if (word2.type == WordType_Object && here(obj, loc)) {
        switch (obj) {
            case TROLL:
                puts("Trolls are close relatives with the rocks and have skin as tough as\n"
                     "that of a rhinoceros.  The troll fends off your blows effortlessly.");
                return STAY_STILL;
            case DWARF:
                return kill_dwarf(loc);
            case DRAGON:
                return kill_dragon(loc);
            case SNAKE:
                puts("Attacking the snake both doesn't work and is very dangerous.");
                return STAY_STILL;
            case BLOB:
                puts("You attack the strange blob, but bounce harmlessly off of its strong\n"
                     "but very rubbery skin.");
                return STAY_STILL;
            case BEAR:
                if (objs[BEAR].prop == 0) {
                    puts("With what?  Your bare hands??  Against *his* bear hands???  Don't be\n"
                         "ridiculous - he'd tear you to shreds!");
                } else {
                    puts("The bear is confused; he only wants to be your friend.");
                }
                return STAY_STILL;
            case CLAM: case OYSTER:
                puts("The shell is very strong and is impervious to attack.");
                return STAY_STILL;
            case OGRE:
                return kill_ogre(loc);
            case BIRD:
                if (closure <= 2) {
                    apport(BIRD, R_LIMBO);
                    puts("The little bird is now dead.  Its body disappears.");
                } else {
                    puts("Oh, leave the poor unhappy bird alone.");
                }
                return STAY_STILL;
            case DJINN:
                /* This seems like a rather broad hint. */
                puts("That's not a wise thing to try - djinni are essentially immortal\n"
                     "and thus are pretty hard to hurt.  Besides, this one seems rather\n"
                     "friendly - why don't you just try releasing him?");
                return STAY_STILL;
            case GOBLINS:
                puts("You attack the goblins and manage to squash a few, but the others\n"
                     "overwhelm you, forcing you to the ground and ripping out your throat.");
                return you_are_dead_at(loc);
            case BASILISK:
                if (objs[BASILISK].prop <= 1) {
                    puts("You attack the basilisk mightily.  It instantly awakens and looks you\n"
                         "dead in the eye, and your body turns to solid rock.");
                    return you_are_dead_at(loc);
                } else {
                    it_is_dead();
                }
                return STAY_STILL;
            case GONG:
                puts(">BONNNNGGGGGGGGGG<\n");
                if (there(TURTLE, loc)) {
                    puts("Darwin the Tortoise blinks in surprise at the noise, but does nothing.");
                } else {
                    puts("A hollow voice says, \"The GallopingGhost Tortoise Express is now at\n"
                         "your service!\n\n"
                         "With a swoosh and a swirl of water, a large tortoise rises to the\n"
                         "surface of the reservoir and paddles over to the shore near you.\n"
                         "The message, \"I'm Darwin - ride me!\" is inscribed on his back in\n"
                         "ornate letters.\n");
                    apport(TURTLE, loc);
                }
                return STAY_STILL;
            default: return 0;  /* other nouns are unhandled */
        }
        return STAY_STILL;  /* handled */
    }
    if (word2.type == WordType_None) {
        puts("There is nothing here to attack.");
        return STAY_STILL;
    }
    return 0;  /* unhandled */
}

bool feedable(ObjectWord o)
{
    return (o == BEAR || o == TROLL || o == BIRD || o == SNAKE ||
            o == DWARF || o == DRAGON || o == BASILISK || o == GOBLINS);
}

int attempt_feed(Location loc)
{
    ObjectWord o = word2.meaning;
    if (word2.type == WordType_Object && feedable(o)) {
        if (!here(o, loc)) {
            I_see_no(word2.text);
            return STAY_STILL;
        }
        switch (o) {
            case BEAR:
                if (here(FOOD, loc)) {
                    puts("The bear eagerly wolfs down your food, after which he seems to calm\n"
                         "down considerably and even becomes rather friendly.");
                    apport(FOOD, R_LIMBO);
                    objs[BEAR].prop = 1;  /* tame but still chained */
                    objs[AXE].prop = 0;  /* re-mobilize it */
                } else {
                    except_perhaps_you();
                }
                break;
            case TROLL:
                puts("Gluttony is not one of the troll's vices.  Avarice, however, is.");
                break;
            case SNAKE:
                if (toting(BIRD)) {
                    puts("The snake has now devoured your bird.");
                    apport(BIRD, R_LIMBO);
                } else {
                    except_perhaps_you();
                }
                break;
            case DWARF:
                puts("You fool, dwarves eat only coal!  Now you've made him *really* mad!!");
                dwarves_enraged = true;
                break;
            case BIRD:
                puts("It's not hungry (it's merely pinin' for the fjords).  Besides, you\n"
                     "have no bird seed.");
                break;
            case DRAGON:
                if (objs[DRAGON].prop == 0) {
                    except_perhaps_you();
                } else {
                    it_is_dead();
                }
                break;
            case BASILISK:
                if (objs[BASILISK].prop <= 1) {
                    except_perhaps_you();
                } else {
                    it_is_dead();
                }
                break;
            case GOBLINS:
                puts("Goblins live exclusively on human flesh, and you can't spare\n"
                     "any of your own to placate them.  On the other hand, I suspect\n"
                     "that they're going to eat you pretty soon whether you like it\n"
                     "or not - you'd better find some way of killing them or driving\n"
                     "them away!");
                break;
        }
    } else if (word2.type == WordType_None) {
        say_what();
    } else {
        hah();
    }
    return STAY_STILL;
}

int attempt_score(void)
{
    gave_up = true;
    printf("If you were to quit now, you would score a total of %d points, out\n"
           "of a possible maximum of 550 points.\n", score());
    gave_up = false;
    return STAY_STILL;
}

void cant_find(void)
{
    puts("I can only tell you what you see as you move about and manipulate\n"
         "things.  I cannot tell you where remote things or places are.");
}

int attempt_find(Location loc)
{
    if (keywordv(CAVE)) {
        if ((places[loc].flags & F_NOTINCAVE) && !(places[R_INSIDE].flags & F_BEENHERE)) {
            puts("I don't know where the cave is, but hereabouts no stream can run on\n"
                 "the surface for long.  I would try the stream.");
        } else {
            puts("I need more detailed instructions to do that.");
        }
    } else if (word2.type == WordType_Object) {
        ObjectWord o = word2.meaning;
        if (toting(o)) {
            puts("I believe what you want is right here with you.");
        } else if (there(o, loc)) {
            puts("I daresay whatever you want is around here somewhere.");
        } else {
            cant_find();
        }
    } else if (word2.type == WordType_Place) {
        Location dest = word2.meaning;
        if (loc == dest) {
            puts("That's where you are now!");
        } else {
            cant_find();
        }
    } else {
        say_what();
    }
    return STAY_STILL;
}

void pour_water(Location loc)
{
    assert(toting(WATER));
    apport(WATER, R_LIMBO);
    objs[BOTTLE].prop = 1;  /* empty the bottle */
    if (there(DOOR, loc)) {
        puts("The hinges are quite thoroughly rusted now and won't budge.");
        objs[DOOR].prop = 0;
    } else if (there(PLANT, loc)) {
        if (objs[PLANT].prop == 0) {
           puts("The plant spurts into furious growth for a few seconds.");
           objs[PLANT].prop = 1;
           objs[PLANT2].flags &= ~F_INVISIBLE;
        } else if (objs[PLANT].prop == 1) {
           puts("The plant grows explosively, almost filling the bottom of the pit.");
           objs[PLANT].prop = 2;
        } else if (objs[PLANT].prop == 2) {
           puts("You've over-watered the plant! It's shriveling up! It's, it's...");
           objs[PLANT].prop = 0;
           objs[PLANT2].flags |= F_INVISIBLE;
        }
        objs[PLANT2].prop = objs[PLANT].prop;
        look_around(loc, /*familiar_place=*/true);
    } else {
        puts("Your bottle is empty and the ground is wet.");
        places[loc].flags |= F_DAMP;
    }
}

void pour_oil(Location loc)
{
    assert(toting(OIL));
    apport(OIL, R_LIMBO);
    objs[BOTTLE].prop = 1;
    if (there(DOOR, loc)) {
        puts("The oil has freed up the hinges so that the door will now move,\n"
             "although it requires some effort.");
        objs[DOOR].prop = 1;
    } else if (there(PLANT, loc)) {
        puts("The plant indignantly shakes the oil off its leaves and asks, \"Water?\"");
    } else {
        puts("Your bottle is empty and the ground is wet.");
        places[loc].flags |= F_DAMP;
    }
}

int attempt_eat(Location loc)
{
    if (word2.type == WordType_None) {
        /* Platt's logic here may or may not have been intentional.
         * If only one edible thing is here (toted or otherwise), we'll
         * eat it. Tiebreaker: we'll prefer the thing on the ground.
         * Second tiebreaker: we'll prefer the mushroom. */
        default_to_something(edible, loc);
        if (word2.type == WordType_None && here(MUSHROOM, loc)) {
            word2.type = WordType_Object;
            word2.meaning = MUSHROOM;
        } else if (word2.type == WordType_None && here(FOOD, loc)) {
            word2.type = WordType_Object;
            word2.meaning = FOOD;
        }
    }
    ObjectWord obj = word2.meaning;
    if (word2.type == WordType_None) {
        puts("There is nothing here to eat.");
        return STAY_STILL;
    } else if (word2.type == WordType_Object) {
        switch (obj) {
            case MUSHROOM:
                if (here(MUSHROOM, loc)) {
                    puts("As you swallow the mushroom your mouth becomes numb, and everything\n"
                         "seems to swirl around you.  The effect quickly passes, and you find\n"
                         "that your muscles have bulged unbelievably.");
                    mushtime = 30;
                    mushtime += lastclock;
                    apport(MUSHROOM, R_LIMBO);
                    objs[MUSHROOM].prop = 2;
                    strength = 12;
                    return STAY_STILL;
                }
                return 0;  /* unhandled */
            case FOOD:
                if (here(FOOD, loc)) {
                    apport(FOOD, R_LIMBO);
                    puts("Thank you, it was delicious!");
                    return STAY_STILL;
                }
                return 0;  /* unhandled */
            case DWARF: case DRAGON: case BIRD: case SNAKE: case BEAR:
            case TROLL: case PLANT: case OGRE: case BASILISK: case GOBLINS:
                if (here(obj, loc)) {
                    puts("Yeetttch!  I think I just lost my appetite.");
                } else {
                    I_see_no(word2.text);
                }
                return STAY_STILL;
            default:
                if (here(obj, loc)) {
                    puts("That's a repulsive idea!");
                    return STAY_STILL;
                }
                return 0;
        }
    }
    return 0;  /* unhandled */
}

int passphrase(Location loc)
{
    if (there(SAFE, loc) && objs[SAFE].prop == 0) {
        if ((word2.type == WordType_None && keywordv(password)) ||
            (word2.type == WordType_Verb && word2.meaning == (int)password)) {
            puts(">ker-THUNK<\n"
                 "            >screeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeech<\n\n"
                 "The (somewhat rusty) safe is now open.");
            objs[SAFE].prop = 1;
            successfully_opened_safe = true;
        } else if (!successfully_opened_safe) {
            puts("\n>bong<                 The very air quivers with sound as though\n"
                 "   >bong<               someone, somewhere in the distance, has struck\n"
                 "      >bong<             three powerful blows on an immense brass gong.\n\n"
                 "Smoke trickles out from around the edges of the safe's door, and the\n"
                 "door itself glows red with heat for a moment.\n\n"
                 "A hollow voice says,  \"This is a Class 1 security alarm.  All cave\n"
                 "security forces go to Orange Alert.  I repeat - Orange Alert.\"\n");
            objs[SAFE].prop = 2;  /* melted */
            objs[BLOB].prop = 1;  /* wake up the blob */
            blob_is_coming = true;
            nomagic = true;  /* inhibit magic words */
            objs[GRATE].prop = 0;  /* locked */
        }
    } else {
        nothing_happens();
    }
    return STAY_STILL;
}

void wizmode_passphrase(void)
{
    static int magick = 0;
    assert(word1.type == WordType_Verb);
    switch (word1.meaning) {
        case CURIOUS:
            if (magick == 0) {
                magick = 1;
                return;
            }
            break;
        case GREEN:
            if (magick == 1) {
                magick = 2;
                return;
            }
            break;
        case IDEAS:
            if (magick == 2) {
                magick = 3;
                return;
            }
            break;
        case SLEPT:
            if (magick == 3) {
                magick = 4;
                return;
            }
            break;
        case FURIOUSLY:
            if (magick == 4) {
                magick = -1;
                wizard_mode = true;
                ok();
                return;
            }
            break;
    }
    magick = -1;
    say_what();
}

void destroy_cave(void)
{
    puts("The ground begins to shudder ominously, and the very cave walls around\n"
         "you begin to creak and groan!  A sulphurious stench fills the air!\n\n"
         "With an incredible lurch, the ground begins to dance and ripple as\n"
         "though it were liquid!  You are thrown off of your feet and tossed\n"
         "violently up and down!  The cave walls begin to crumble and split from\n"
         "the stress!\n\n"
         "There is a terrible ROAR of rending rock!!  The cave ceiling splits,\n"
         "and rocks plunge down and smash your lower body to a gooey paste!!\n\n"
         "There is a violent blast in the distance!  Steam and smoke drift into\n"
         "view through the rents in the walls, and furiously-bubbling red-hot\n"
         "lava flows in and surrounds you.  The cave ceiling disintegrates in\n"
         "an incredible orgy of grinding destruction, and the cave walls fall\n"
         "and are pounded into fine dust.\n\n\n\n"
         "You are lying, badly mangled, on a small rock island in a sea of\n"
         "molten lava.  Above you, the sky is faintly visible through a thick\n"
         "pall of smoke and steam.  A short distance to the north, the remains\n"
         "of a well-house are sinking slowly into the bubbling ooze.\n\n\n"
         "There is a distant, sourceless screech of incredible anguish!  With\n"
         "a sharp >poof< and a small puff of orange smoke, a bent and bearded\n"
         "elf appears.  He is dressed in working clothes, and has a name-tag\n"
         "marked \"Ralph\" on his shirt.  \"You blithering idiot!\" he storms.\n"
         /* Platt has "work" for "word" again. */
         "\"You were warned quite clearly not to use that word near water!!  I\n"
         "hadn't gotten all of the bugs out of it yet, and now your incredible\n"
         /* Platt has "incompetance". */
         "incompetence has totally destroyed Colossal Cave!!  Do you have the\n"
         "faintest scintilla of an iota of an understanding of how much work\n"
         "I'm going to have to do to get the cave rebuilt?!?  I'll have to go\n"
         "all the way to Peking for another dragon, and I'll have to convince\n"
         "the Goblin's Union to send me another team of gooseberry goblins;\n"
         "I'll have to sub-contract the building of the volcano out to the\n"
         "local totrugs, and worst of all I'll have to go through eight months\n"
         "of paperwork and red tape to file a new Environmental Impact\n"
         "statement!!  All because you couldn't follow directions, you\n"
         "purblind and meatbrained moron!  I'm rescinding all of your game\n"
         "points and throwing you out!  Out!   OUT!   GET OUT!$!%#&'@%!!%%!\"\n");
    /* You expected your score to be printed, perhaps?
     * You get NOTHING! Good DAY sir! */
    exit(0);
}

int attempt_phuggg(Location loc)
{
    djinn_gave_hint = true;  /* since you obviously don't need the hint */
    if (places[loc].flags & F_NOTINCAVE) {
        nothing_happens();
    } else if ((here(BOTTLE, loc) && !objs[BOTTLE].prop) || (places[loc].flags & F_WATER)) {
        if (pct(85)) {
            nothing_happens();
        } else if (pct(95)) {
            puts("\n>splurch!<\n\n"
                 "Oh, no!  You've turned yourself into a jellyfish, and fallen to the\n"
                 "ground and been splattered far and wide!  Well, that certainly wasn't\n"
                 /* Platt has "work" instead of "word"; I'm fixing it. */
                 "very smart!!!  You were warned not to use that word near water!");
            return you_are_dead_at(loc);
        } else {
            destroy_cave();
        }
    } else if (here(AXE, loc) || here(SWORD, loc)) {
        if (here(AXE, loc)) {
            puts("Your axe glows bright orange and fades into nothingness.");
            apport(AXE, R_LIMBO);
        }
        if (here(SWORD, loc)) {
            puts("Your sword jumps into the air, chants several bars of the \"Volga\n"
                 "Boatman\", shoots off several fitful blue sparks, and disintegrates.");
            apport(SWORD, R_LIMBO);
        }
    } else {
        int i = 2*ran(3) + (objs[DWARF].prop > 1);
        if (there(DWARF, loc)) {
            if (pct(70)) {
                switch (i) {
                    case 0:
                        puts("A clear, liquid chime sounds in midair.  A large, four-clawed hand\n"
                             "reaches out of the ground, grabs the dwarf, and pulls it down into\n"
                             "nothingness.");
                        break;
                    case 1:
                        puts("A clear, liquid chime sounds in midair.  A long green tentacle\n"
                             "covered with sucker disks reaches out from nowhere, grabs the\n"
                             "dwarves, and pulls them back to wherever it came from.");
                        break;
                    case 2:
                        puts("There is a sharp sizzling sound.  The dwarf explodes into flame\n"
                             "and vanishes.");
                        break;
                    case 3:
                        puts("There is a sharp sizzling sound.  The dwarves are engulfed in\n"
                             "a wave of fire that appears from nowhere, and are completely\n"
                             "incinerated;  the flames then vanish into nothingness again.");
                        break;
                    case 4:
                        puts("There is a sharp whistling sound from nowhere.  The dwarf shudders\n"
                             "and turns into a moth, which then flies away.");
                        break;
                    case 5:
                        puts("There is a sharp whistling sound from nowhere.  The dwarves stiffen,\n"
                             "shudder, and melt down into a large puddle of soggy goo that quickly\n"
                             "soaks into the ground and vanishes.");
                        break;
                }
                apport(DWARF, R_LIMBO);
                dwarfcount -= objs[DWARF].prop;
                objs[DWARF].prop = 0;
            } else {
                switch (i) {
                    case 0:
                        puts("A clear, liquid chime sounds in midair.  A large, four-clawed foot\n"
                             "appears in midair and stomps violently downward, missing the dwarf\n"
                             "but thoroughly squashing you.");
                        break;
                    case 1:
                        puts("A clear, liquid chime sounds in midair.  A large and very toothy\n"
                             "mouth appears in midair and chomps ferociously.  The dwarves manage\n"
                             "to evade it, but it bites you in half.");
                        break;
                    case 2:
                        puts("There is a sharp sizzling sound.  A ball of fire roars out of nowhere,\n"
                             "misses the dwarf, bounces off of a wall, and incinerates you.");
                        break;
                    case 3:
                        puts("There is a sharp sizzling sound.  A ball of fire appears from nowhere,\n"
                             "bounces off of the ground, and explodes violently, incinerating both\n"
                             "you and the dwarves.");
                        break;
                    case 4:
                        puts("There is a sharp crackling sound from the air above you.  The dwarf\n"
                             "shudders and turns into a sabre-toothed tiger, which attacks and\n"
                             "kills you in short order.");
                        break;
                    case 5:
                        puts("There is a sharp crackling sound from the air above you.  The dwarves\n"
                             "stiffen, fall to the ground, and melt into a large puddle of soggy\n"
                             "goo.  The goo twitches a few times and then flows at you with\n"
                             "incredible speed;  it attacks and strangles you with little\n"
                             "difficulty.");
                        break;
                }
                return you_are_dead_at(loc);
            }
        } else {
            nothing_happens();
        }
    }
    return STAY_STILL;
}

int process_verb(Location loc)
{
    const int verb = word1.meaning;
    const int obj = word2.meaning;
    if (word1.type == WordType_Verb)
    switch (verb) {
        case GET:
            return attempt_take(obj, loc);
        case DROP:
            return attempt_drop(obj, loc);
        case WALK:
            if (word2.type == WordType_None) {
                printf("Which way should I %s?\n", word1.text);
                return STAY_STILL;
            } else if (word2.type == WordType_Verb &&
                       is_walkable_direction(obj)) {
                /* Redirect WALK SOUTH to just SOUTH. */
                word1 = word2;
                word2.type = WordType_None;
                word2.meaning = 0;
                return process_verb(loc);
            } else if (word2.type == WordType_Place) {
                if (word2.meaning == loc) {
                    puts("I need more detailed instructions to do that.");
                } else {
                    puts("I don't know how to get there from here.");
                }
            } else {
                puts("I don't understand that!");
            }
            return STAY_STILL;
        case DIG:
            puts("Digging without a shovel is quite impractical.  Even with a shovel\n"
                 "progress is unlikely.");
            return STAY_STILL;
        case CAVE:
            /* Platt misses R_HILL in his explicit list of not-cave
             * locations. This is certainly a bug. */
            if ((places[loc].flags & F_NOTINCAVE) && (loc != R_HILL)) {
                puts("I don't know where the cave is, but hereabouts no stream can run on\n"
                     "the surface for long.  I would try the stream.");
                return STAY_STILL;
            }
            break;  /* unhandled */
        case THROW:
            return attempt_throw(obj, loc);
        case WAVE:
            if (word2.type == WordType_Object) {
                if (!toting(obj)) {
                    puts("You aren't carrying it!");
                } else if (obj == ROD) {
                    wave_rod(loc);
                } else if (obj == AXE || obj == SWORD) {
                    return weaponry(obj, loc);
                } else {
                    nothing_happens();
                }
            } else if (word2.type == WordType_None) {
                nothing_happens();
            } else {
                hah();
            }
            return STAY_STILL;
        case NORTH:
        case SOUTH:
        case EAST:
        case WEST:
        case NE: case NW: case SE: case SW:
        case UP: case DOWN:
            return no_move_possible(loc);
        case INVENTORY:
            attempt_inventory();
            return STAY_STILL;
        case LOOK:
            if (keywordo(MIST)) return examine_mist();
            if (keywordo(TREES)) return examine_trees();
            if (now_in_darkness(loc)) {
                puts("You have no source of light.");
            } else {
                look_around(loc, /*familiar=*/false);
            }
            return STAY_STILL;
        case ON:
            if (keywordo(LAMP) || word2.type == WordType_None) {
                if (!here(LAMP, loc)) {
                    if (keywordo(LAMP)) {
                        I_see_no("lamp");
                    } else {
                        puts("You have no source of light.");
                    }
                } else if (objs[LAMP].prop) {
                    puts("Your lamp was already on!");
                } else {
                    if (lamplife < 40 && here(BATTERIES, loc) && !objs[BATTERIES].prop) {
                        /* Notice that Platt has "the the". I'm leaving it. */
                        puts("The old batteries in your lamp were pretty well shot - I've taken the\n"
                             "the liberty of putting in the new ones.");
                        objs[BATTERIES].prop = 1;
                        lamplife += 300;
                        have_used_noside_samoht = false;
                    }
                    if (lamplife == 0) {
                        puts("Your lamp has run out of power.");
                    } else {
                        objs[LAMP].prop = 1;
                        puts("Your lamp is now on.");
                        if (!(places[loc].flags & F_LIGHTED))
                            look_around(loc, /*familiar=*/false);
                        phog(loc);
                    }
                }
                return STAY_STILL;
            }
            return 0;  /* LIGHT AXE, e.g., is unhandled */
        case OFF:
            if (keywordo(LAMP) || word2.type == WordType_None) {
                if (!here(LAMP, loc)) {
                    if (keywordo(LAMP)) {
                        I_see_no("lamp");
                    } else {
                        return 0;  /* unhandled */
                    }
                } else if (!objs[LAMP].prop) {
                    puts("Your lamp was already off!");
                } else {
                    objs[LAMP].prop = 0;
                    puts("Your lamp is now off.");
                    if (!(places[loc].flags & F_LIGHTED))
                       puts("It is now pitch dark.  If you proceed you will likely fall into a pit.");
                    phog(loc);
                }
            } else {
                dunno_how();
            }
            return STAY_STILL;
        case OPEN:
            return attempt_open(loc);
        case CLOSE:
            return attempt_close(loc);
        case HELP:
            attempt_help(loc);
            return STAY_STILL;
        case INFO:
            attempt_info();
            return STAY_STILL;
        case QUIT:
            if (yes("Do you really want to quit now?")) {
                gave_up = true;
                finis();
            }
            ok();
            return STAY_STILL;
        case BRIEF:
            puts("Okay, from now on I'll only describe a place in full the first time\n"
                 "you come to it.  To get the full description, say \"LOOK\".");
            verbosity_mode = BRIEF;
            return STAY_STILL;
        case FAST:
            ok();
            verbosity_mode = FAST;
            return STAY_STILL;
        case FULL:
            verbosity_mode = FULL;
            ok();
            return STAY_STILL;
        case LPSD:
            if (wizard_mode && word2.type == WordType_Place) {
                say_foof();
                return word2.meaning;
            } else {
                say_what();
            }
            return STAY_STILL;
        case KILL:
            return attempt_kill(loc);
        case FEED:
            return attempt_feed(loc);
        case SCORE:
            return attempt_score();
        case JUMP:
            puts("There is nowhere for me to jump to.");
            return STAY_STILL;
        case IN:
        case OUT:
            puts("I don't know in from out here.  Use compass points or name something\n"
                 "in the general direction you want to go.");
            return STAY_STILL;
        case SESAME:
            puts("Good try, but that is an old worn-out magic word.");
            return STAY_STILL;
        case FEE:
            if (keywordv(FIE) || keywordv(FOE) || keywordv(FOO) || keywordv(FUM)) {
                foobar = 0;
                nothing_happens();
            } else {
                /* FEE LAMP will still work! */
                foobar = 1;
                ok();
            }
            return STAY_STILL;
        case FIE:
            if (foobar == 0) {
                foobar = 2;
                ok();
            } else {
                foobar = 0;
                nothing_happens();
            }
            return STAY_STILL;
        case FOE:
            if (foobar == 1) {
                foobar = 3;
                ok();
            } else {
                foobar = 0;
                nothing_happens();
            }
            return STAY_STILL;
        case FUM:
            foobar = 0;
            nothing_happens();
            return STAY_STILL;
        case FOO:
            if (there(EGGS, R_YLEM) || foobar == 2) {
                if (there(EGGS, R_GIANT)) {
                    nothing_happens();
                } else if (here(EGGS, loc)) {
                    puts("The nest of golden eggs has vanished!");
                } else if (loc == R_GIANT) {
                    puts("There is a large nest here, full of golden eggs!");
                } else {
                    puts("Done!");
                }
                if (there(EGGS, R_LIMBO)) {
                    have_stolen_back_eggs = true;
                    if (objs[TROLL].prop == 1 || objs[TROLL].prop == 2) {
                        if (there(TROLL2, loc)) {
                            puts("From beneath the bridge there sounds a muffled bellow of frustrated\n"
                                 "avarice.  The troll stomps out, resumes his position by the end of\n"
                                 "the bridge, and glowers fiercely in your direction.  \"Very funny,\"\n"
                                 "you hear him mutter, \"but not very smart - you'll pay for that!\"");
                        }
                        objs[TROLL].prop = 0;
                        apport(TROLL, R_SWOFCHASM);
                        apport(TROLL2, R_LIMBO);
                    }
                }
                apport(EGGS, R_GIANT);
            } else {
                nothing_happens();
            }
            return STAY_STILL;
        case XYZZY:
        case PLUGH:
            nothing_happens();
            return STAY_STILL;
        case FIND:
            return attempt_find(loc);
        case SWIM:
            dunno_how();
            return STAY_STILL;
        case BREAK:
            if (keywordo(VASE) && here(VASE, loc)) {
                puts("You have taken the vase and hurled it delicately to the ground.");
                apport(VASE, R_LIMBO);
                apport(SHARDS, loc);
                return STAY_STILL;
            } else if (keywordo(VIAL) && here(VIAL, loc)) {
                return break_vial(loc);
            }
            return 0;  /* unhandled */
        case FIX:
            if (keywordo(VASE) && here(SHARDS, loc)) {
                puts("It is beyond your power to do that.");
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        case FILL:
            if (keywordo(VASE) && here(VASE, loc)) {
                if ((places[loc].flags & F_WATER) || loc == R_EPIT) {
                    puts("The sudden change in temperature has delicately shattered the vase.");
                    apport(VASE, R_LIMBO);
                    apport(SHARDS, loc);
                } else {
                    puts("There is nothing here with which to fill the vase.");
                }
                return STAY_STILL;
            } else if (keywordo(BOTTLE) && here(BOTTLE, loc)) {
                if (objs[BOTTLE].prop != 1) {
                    puts("Your bottle is already full.");
                } else if (places[loc].flags & F_WATER) {
                    puts("Your bottle is now full of water.");
                    objs[BOTTLE].prop = 0;
                    if (toting(BOTTLE)) apport(WATER, R_INHAND);
                } else if (loc == R_EPIT) {
                    puts("Your bottle is now full of oil.");
                    objs[BOTTLE].prop = 2;
                    if (toting(BOTTLE)) apport(OIL, R_INHAND);
                } else {
                    puts("There is nothing here with which to fill the bottle.");
                }
                return STAY_STILL;
            } else if (word2.type != WordType_None) {
                puts("You can't fill that.");
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        case POUR:
            if (keywordo(WATER)) {
                if (toting(WATER)) {
                    pour_water(loc);
                } else {
                    You_have_no("water");
                }
                return STAY_STILL;
            } else if (keywordo(OIL)) {
                if (toting(OIL)) {
                    pour_oil(loc);
                } else {
                    You_have_no("oil");
                }
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        case PLACATE:
            if (there(DWARF, loc) || there(SNAKE, loc) || there(BIRD, loc) ||
                there(DRAGON, loc) || there(TROLL, loc) || there(BEAR, loc) ||
                there(OGRE, loc) || there(BASILISK, loc) || there(GOBLINS, loc)) {
                puts("I'm game.  Would you care to explain how?");
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        case EAT:
            return attempt_eat(loc);
        case RUB:
            if (keywordo(LAMP) && here(LAMP, loc)) {
                printf("Rubbing the electric %s is not particularly rewarding.  Anyway,\n"
                       "nothing exciting happens.\n", word2.text);
                return STAY_STILL;
            } else if (word2.type == WordType_Object && here(word2.meaning, loc)) {
                puts("Peculiar.  Nothing unexpected happens.");
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        case SAY:
            if (keywordv(PLUGH) || keywordv(XYZZY) || keywordp(R_PLOVER) ||
                keywordv(THURB) || keywordv(MELENKURION) || keywordv(NOSIDE) ||
                keywordv(SAMOHT) || keywordv(KNERL) || keywordv(ZORTON) ||
                keywordv(KLAETU) || keywordv(SNOEZE) || keywordv(BLERBI) ||
                keywordv(PHUGGG)) {
                word1 = word2;
                word2.type = WordType_None;
                return process_verb(loc);
            } else if (word2.type != WordType_None) {
                /* We've already printed a message via presay(). */
                return STAY_STILL;
            }
            return 0;  /* unhandled */
#ifdef SAVE_AND_RESTORE
        case SAVE:
            switch (attempt_save()) {
                case 0: puts("Save failed!"); break;
                case 1: puts("Saved."); break;
                case 2: puts("Restored."); break;
            }
            return STAY_STILL;
        case RESTORE:
            /* On the fizmo interpreter, @restore yields 2
             * when the save file doesn't exist, or when it
             * has the wrong serial number for this game.
             * I don't know what return value 0 would mean. */
            attempt_restore();
            puts("Restore failed!");
            return STAY_STILL;
#else
        case SAVE:
        case RESTORE:
            puts("Sorry, saving and restoring games is not implemented in this version\n"
                 "of Adventure.");
            return STAY_STILL;
#endif /* SAVE_AND_RESTORE */
        case DRINK:
            /* This action produces a different message at R_BEACH. */
            if ((word2.type == WordType_None) || keywordo(WATER)) {
                if (places[loc].flags & F_WATER) {
                    puts("You have taken a drink from the stream.  The water tastes strongly of\n"
                         "minerals, but is not unpleasant.  It is extremely cold.");
                } else if (here(BOTTLE, loc) && objs[BOTTLE].prop == 0) {
                    puts("The bottle of water is now empty.");
                    apport(WATER, R_LIMBO);
                    objs[BOTTLE].prop = 1;
                } else if (keywordo(WATER)) {
                    I_see_no("water");
                } else {
                    puts("There is nothing here that I can drink!");
                }
            } else if (word2.type == WordType_Object) {
                if (here(word2.meaning, loc)) {
                    hah();
                } else {
                    I_see_no(word2.text);
                }
            } else {
                hah();
            }
            return STAY_STILL;
        case NEWS:
            attempt_news();
            return STAY_STILL;
        case READ:
            if (word2.type == WordType_Object) {
                ObjectWord obj = word2.meaning;
                if (here(obj, loc)) {
                    switch (obj) {
                        case MAG:
                            puts("I'm afraid the magazine is written in a form of Dwarvish runes\n"
                                 "with which I am not familiar.");
                            break;
                        case MESSAGE:
                            puts("\"This is not the maze where the pirate leaves his treasure chest.\"");
                            break;
                        case TABLET:
                            puts("It says, \"Congratulations on bringing light into the Dark-room!\"");
                            break;
                        /* Platt has a label for READ.MACHINE, but it is unused. */
                        default:
                            dunno_hao(word1.text);
                            break;
                    }
                } else {
                    I_see_no(word2.text);
                }
            } else if (word2.type != WordType_None) {
                hah();
            } else {
                return 0;  /* unhandled */
            }
            return STAY_STILL;
        case HOURS:
            puts("\nColossal Cave is always open.\n");
            return STAY_STILL;
        case WIZARD:
            /* This is an amusing codepath. Platt includes a command WIZARD,
             * which prompts the player for the "magic word"; but in fact
             * no matter what the player types, he'll be docked ten points
             * for attempting to enter wizard mode by such an obvious route. */
            if (yes("Are you actually a wizard?")) {
                puts("Prove it - say the magic word!");
                listen();
                puts("Oh, pooh - you are nothing but a charlatan!  That little piece of\n"
                     "deception is going to cost you 10 points!!");
                penalties += 10;
            } else {
                ok();
            }
            return STAY_STILL;
            /* The actual way into wizard mode is to type the five-word
             * passphrase "CURIOUS GREEN IDEAS SLEPT FURIOUSLY" (a distortion
             * of Noam Chomsky's "colorless green ideas sleep furiously")
             * one word at a time, the same way as FEE FIE FOE FOO.
             * Presumably Platt picked SLEPT over SLEEP because the latter
             * is a plausible "in-character" input. As for CURIOUS versus
             * COLORLESS, perhaps the five-character limit on input words
             * was relevant; to the Adventure interpreter, COLORLESS and
             * COLORFUL are considered the same word. */
        case CURIOUS:
        case GREEN:
        case IDEAS:
        case SLEPT:
        case FURIOUSLY:
            wizmode_passphrase();
            return STAY_STILL;
        case CLIMB:
            puts("There's nothing climbable here.");
            return STAY_STILL;
        case LOST:
            puts("I'm as confused as you are.");
            return STAY_STILL;
        case MELENKURION:
            if (loc == R_BY_FIGURE && objs[STATUE].prop == 0) {
                /* TODO: This codepath should really be in at_by_figure(). */
                puts("Rock silently crumbles off of the wall in front of you, revealing\n"
                     "dark passages leading northwest, north, and northeast.");
                objs[STATUE].prop = 1;
            } else {
                nothing_happens();
            }
            return STAY_STILL;
        case NOSIDE:
            if (keywordv(SAMOHT)) {
                if (have_used_noside_samoht ||
                    (places[loc].flags & F_LIGHTED) ||
                    !(places[R_LAIR].flags & F_BEENHERE) ||
                    !here(LAMP, loc)) {
                    nothing_happens();
                } else if (lamplife > 40) {
                    /* Using the magic word prematurely will kill your lamp...
                     * permanently. */
                    apport(LAMP, R_YLEM);
                    objs[BATTERIES].prop = 1;
                    objs[LAMP].prop = 0;
                    lamplife = 0;
                    if (pct(50)) {
                        puts("With a loud \"zap\" a bolt of lightning springs out of midair and strikes\n"
                             "your lamp, which immediately and violently explodes.  You narrowly\n"
                             "miss being torn to shreds by the flying metal.\n\n"
                             "It is now pitch dark.  If you proceed you will likely fall into a pit.");
                    } else {
                        puts("In a loud crackle of electricity, a bolt of lightning jumps out of\n"
                             "nowhere and strikes your lamp.  The lamp instantly explodes like a\n"
                             "grenade, and you are mown down by a cloud of shrapnel.");
                        return you_are_dead_at(loc);
                    }
                } else {
                    puts("The air fills with tension, and there is a subdued crackling sound.\n"
                         "A blue aura forms about your lantern, and small sparks jump from the\n"
                         "lantern to the ground.  The aura fades away after several seconds,\n"
                         "and your lamp is once again shining brightly.");
                    lamplife += 150;
                    objs[LAMP].prop = 1;  /* shining brightly */
                    have_used_noside_samoht = true;
                }
            } else {
                nothing_happens();
            }
            return STAY_STILL;
        case SAMOHT:
        case THURB:
            /* THURB is handled in at_icecave30(). */
            nothing_happens();
            return STAY_STILL;
        case KNERL:
        case ZORTON:
        case KLAETU:
        case SNOEZE:
        case BLERBI:
            return passphrase(loc);
        case RIDE:
            if (word2.type == WordType_None || keywordo(TURTLE)) {
                if (there(TURTLE, loc)) {
                    puts("You step gently on Darwin the Tortoise's back, and he carries you\n"
                         "smoothly over to the southern side of the reservoir.  He then blows\n"
                         "a couple of bubbles at you and sinks back out of sight.\n");
                    apport(TURTLE, R_LIMBO);
                    return R_RES;
                }
            }
            return 0;  /* unhandled */
        case PHUGGG:
            return attempt_phuggg(loc);
        case RUNOUT:
            if (wizard_mode) {
                curclock = 0;
                look_around(loc, /*familiar=*/true);
                ok();
            } else {
                say_what();
            }
            return STAY_STILL;
    }
    else if (word1.type == WordType_Object)
    switch (verb) {
        case WATER:
            if (word2.type == WordType_Object && word2.meaning == DOOR) {
                if (there(DOOR, loc)) {
                    if (toting(WATER)) {
                        pour_water(loc);
                    } else {
                        You_have_no("water");
                    }
                } else {
                    I_see_no("door");
                }
            } else if (word2.type == WordType_Object && word2.meaning == PLANT) {
                if (there(PLANT, loc)) {
                    if (toting(WATER)) {
                        pour_water(loc);
                    } else {
                        You_have_no("water");
                    }
                } else {
                    I_see_no("plant");
                }
            } else if (word2.type == WordType_None) {
                if ((there(BOTTLE, loc) && objs[BOTTLE].prop == 0) ||
                    toting(WATER) || places[loc].flags & F_WATER) {
                    what_do("water");
                } else {
                    I_see_no("water");
                }
            } else {
                hah();
            }
            return STAY_STILL;
        case OIL:
            if (word2.type == WordType_Object && word2.meaning == DOOR) {
                if (there(DOOR, loc)) {
                    if (toting(OIL)) {
                        pour_oil(loc);
                    } else {
                        You_have_no("oil");
                    }
                } else {
                    I_see_no("door");
                }
            } else if (word2.type == WordType_Object && word2.meaning == PLANT) {
                if (there(PLANT, loc)) {
                    if (toting(OIL)) {
                        pour_oil(loc);
                    } else {
                        You_have_no("oil");
                    }
                } else {
                    I_see_no("plant");
                }
            } else if (word2.type == WordType_None) {
                if ((there(BOTTLE, loc) && objs[BOTTLE].prop == 2) ||
                    toting(OIL) || loc == R_EPIT) {
                    what_do("oil");
                } else {
                    I_see_no("oil");
                }
            } else {
                hah();
            }
            return STAY_STILL;
        case MIST:
            return examine_mist();
    }
    return 0;  /* unhandled */
}

/*========== The main loop. =============================================*/

void look_around(Location loc, bool familiar)
{
    bool blank_line_before_objects = false;
    if (verbosity_mode == FULL ||
        (verbosity_mode == BRIEF && !familiar)) {
        if (places[loc].long_desc != NULL)
            puts(places[loc].long_desc);
        blank_line_before_objects = true;
    } else {
        if (places[loc].short_desc != NULL)
            puts(places[loc].short_desc);
        else if (places[loc].long_desc != NULL)
            puts(places[loc].long_desc);
    }
    if (places[loc].flags & F_DAMP)
        puts("The ground here is damp.");
    /* Now we've seen this place. */
    places[loc].flags |= F_BEENHERE;
    for (int i=0; i <= MAX_OBJ; ++i) {
        if (there(i, loc)) {
            objs[i].flags |= F_SEEN;
            if (!(objs[i].flags & F_INVISIBLE)) {
                /* F_INVISIBLE seems to be a kludge to avoid printing
                 * the blank line in front of objects whose description
                 * is going to be NULL anyway. Platt gives the DWARF
                 * F_INVISIBLE, and then has each of the dwarf's
                 * descriptions begin with a blank line; this ensures
                 * that the dwarf's description is always preceded by
                 * exactly one blank line, regardless of what else is
                 * in the room.*/
                if (blank_line_before_objects) {
                    puts("");
                    blank_line_before_objects = false;
                }
            }
            /* Yes, even F_INVISIBLE objects are described; see above. */
            describe_object(i);
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
    if (here(LAMP, loc) && objs[LAMP].prop) {
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
        for (int i = 1; i <= MAX_OBJ; ++i) {
            if (is_treasure(i) && !there(i, R_HOUSE))
                closure = 0;
        }
        if (closure == 1) {
            curclock = 35;
        } else {
            curclock = 30+ran(10);
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
            lastclock = curclock = 5;
        }
        if (objs[MUSHROOM].prop > 1) {
            mushtime -= lastclock;
            if (mushtime < 0) {
                if (objs[MUSHROOM].prop == 2) {
                    /* The mushroom is no longer "eaten". It will go back
                     * to the cubicle during the next admin-cycle. */
                    objs[MUSHROOM].prop = 3;
                    mushtime = 40;
                    puts("\nA strange malaise suddenly afflicts you.  You shiver with chill,\n"
                         "and your muscles seem to turn to putty;  everything around you becomes\n"
                         "grey and unreal.  The fit quickly passes, and you find that your body\n"
                         "has degenerated back to what it was like before you ate the mushroom.\n");
                    strength = 7;
                    lastclock = curclock = 8;
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
                curclock = 8+ran(10);
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
                    for (int i = 1; i <= MAX_OBJ; ++i) {
                        if (!is_treasure(i)) continue;
                        if (i == RING) continue;  /* never steal this */
                        if (here(i, loc)) {
                            apport(i, R_PIRATES_NEST);
                            pirate_stole_something = true;
                        }
                    }
                    if (pirate_stole_something) {
                        if (objs[PIRATE].flags & F_SEEN) {
                            puts("\nOut from the darkness behind you leaps the pirate!  \"Har, har,\" he\n"
                                 "chortles nastily, \"I see you've found some more treasure for me!\n"
                                 "Thanks, me hearty, you're doing just fine!\"  With that, he snatches\n"
                                 "your treasure and slips away into the gloom, chuckling with glee.");
                        } else {
                            puts("\nOut from the shadows behind you pounces a bearded pirate!  \"Har, har,\"\n"
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
                            curclock = 4+ran(10);
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
                                describe_object(DWARF);
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
        objs[GORGE].prop = 0;  /* destroy wheatstone bridge */
        apport(TROLL, R_LIMBO); objs[TROLL].prop = 5;
        apport(TROLL2, R_SWOFCHASM);  /* fetch fake troll */
        apport(DRAGON, R_LIMBO);
        apport(TROLL, R_LIMBO);
        objs[FISSURE].flags |= F_INVISIBLE;
        objs[GORGE].flags |= F_INVISIBLE;
        curclock = 25;
    } else {
        /* It's closing time! This is a more evil trick than the
         * one in Woods' code: as long as the panicked player keeps
         * trying to exit the cave, it will never close. Only when
         * you've spent 15 turns quietly observing your surroundings
         * will the cave finally close on its own. */
        if (panicked) {
            panicked = false;
            curclock = 15;
        } else {
            close_the_cave();
        }
    }
    lastclock = curclock;
    return (closure == 3);  /* true if the cave is now closed */
}

void deal_with_syntax_errors(Location loc)
{
    if (word1.type == WordType_Object) {
        if (here(word1.meaning, loc)) {
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
            if (here(word2.meaning, loc)) {
                dunno_hao(word1.text);
            } else {
                I_see_no(word2.text);
            }
        } else {
            dunno_hao(word1.text);
        }
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
    Location loc = R_YLEM;
    Location newloc = R_ROAD;

    curclock = 15+ran(10);
    dwarfcount = 4+ran(5);

    if (false) {
      death:
        kill_the_player(loc);
        /* Coming from R_YLEM ensures that we'll see a room description
         * when we resurrect. Notice that R_YLEM has F_NOBACK. */
        oldloc = loc = R_YLEM;
        newloc = R_HOUSE;
    } else if (false) {
      cave_just_closed:
        oldloc = loc = R_YLEM;
        newloc = R_CYLINDRICAL;
    }

    moved = true;
    while (true) {
        if (blob_is_coming) {
            if (tick()) goto death;
        }
        if (!moved) {
            if (there(DWARF, loc) && pct(backlash)) {
                /* If you stand still, dwarves in the room will attack. */
                if (dwarf_attack()) goto death;
            }
        } else if ((places[loc].flags & F_ONE_EXIT) && there(DWARF, loc) && !wizard_mode) {
            puts("A little dwarf with a big knife blocks your way.");
            newloc = loc;
        } else {
            if (there(FOG, newloc)) {
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
            if (lamprey(loc)) {
                goto cave_just_closed;
            }
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
                curclock -= (familiar_place ? 2 : 3);
                if (curclock <= 0) {
                    if (clock4(loc)) {
                        goto cave_just_closed;
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
        --foobar;
        /* Here Platt increments the variable TURNS. I've left it out because
         * I don't see anywhere its value is actually used. The value of
         * our variable "turns" is what Platt calls MOVES.
         * Notice that entering a blank command will cause time to pass.
         * Hold down enter long enough, and a dwarf will probably kill you. */
        if (word1.type == WordType_None)
            continue;
        if (word1.type == WordType_BadWord) {
            say_what();
            continue;
        }
        presay();
        if (word2.type == WordType_BadWord && !keywordv(SAY)) {
            printf("I don't know the word \"%s\".\n", word2.text);
            continue;
        }

        newloc = places[loc].f();
        if (R_LIMBO < newloc && newloc < R_YLEM) {
            moved = true;
            continue;
        } else if (R_YLEM < newloc && newloc <= 2*R_YLEM) {
            newloc -= R_YLEM;
            goto death;  /* moving that direction killed you */
        } else if (newloc == STAY_STILL) {
            newloc = loc;
            moved = false;
            continue;
        }
        assert(newloc == 0);  /* unhandled */
        newloc = loc;

        if (word1.type == WordType_Place) {
            if (word1.meaning == loc) {
                puts("That's where you are now!");
            } else {
                puts("I don't know how to get there from here.");
            }
        } else if (word1.type == WordType_Verb && word1.meaning == BACK) {
            if ((places[oldloc].flags | places[loc].flags) & F_NOBACK) {
                puts("Sorry, but I no longer seem to remember how it was you got here.");
            } else {
                newloc = oldloc;
                moved = true;
            }
        } else {
            newloc = process_verb(loc);
            if (wizard_mode && keywordv(LPSD) && newloc == R_LIMBO) {
                /* Special case, since I'm using 0 (R_LIMBO) to mean
                 * "unhandled". If a wizard types "LPSD LIMBO", we do
                 * in fact want to move to room number zero. Why?
                 * To get the mithril ring, of course!
                 * There's no way to LPSD to R_YLEM, so that's okay. */
                moved = true;
                continue;
            } else if (R_LIMBO < newloc && newloc < R_YLEM) {
                moved = true;
                continue;
            } else if (R_YLEM < newloc && newloc < 2*R_YLEM) {
                newloc -= R_YLEM;
                goto death;  /* moving that direction killed you */
            } else if (newloc == STAY_STILL) {
                newloc = loc;
                moved = false;
                continue;
            }
            assert(newloc == 0);  /* unhandled */
            newloc = loc;
            moved = false;
            deal_with_syntax_errors(loc);
        }
    }
}

int main(void)
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

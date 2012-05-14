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
bool juggled;  /* true if we've taken or dropped something recently */
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
bool gave_up;


const char ok[] = "OK.";

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

#define IS_TREASURE(t) ((t) >= MIN_TREASURE)

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

bool used_movement_placeword(Location where_to)
{
    /* HOUSE FOO will take you to the house, even if FOO is unrecognized;
     * whereas FOO HOUSE will not. SAY HOUSE does basically the same thing
     * as HOUSE. */
    switch (word1.type) {
        case WordType_Place:
            return (word1.meaning == where_to);
        case WordType_Verb:
            if (word1.meaning == WALK || word1.meaning == SAY)
                return (word2.type == WordType_Place && word2.meaning == where_to);
            return false;
    }
    return false;
}

bool used_movement_verb(VerbWord where_to)
{
    if (word1.type != WordType_Verb) return false;
    if (word1.meaning == (int)where_to) return true;
    if (word1.meaning == WALK || word1.meaning == SAY)
        return (word2.type == WordType_Verb && word2.meaning == (int)where_to);
    return false;
}

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

struct ObjectData objs[MAX_OBJ+1];
int holding_count;  /* how many objects are being carried? */

#define toting(t) (objs[t].place < 0)
#define there(t, loc) (objs[t].place == (loc))

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
    juggled = true;
}

void new_obj(ObjectWord t, const char *n, ObjectWord b, Location l)
{
    objs[t].name = n;
    objs[t].base = (b != 0 ? &objs[b] : NULL);
    objs[t].prop = (IS_TREASURE(t) ? -1 : 0);
    apport(t, l);
}

void build_object_table(void)
{
    new_obj(GRATE, 0, GRATE, R_DEPRESSION);
    objs[GRATE].desc[0] = "The grate is locked.";
    objs[GRATE].desc[1] = "The grate is open.";

    new_obj(OIL, "Oil in the bottle", 0, R_LIMBO);
    new_obj(WATER, "Water in the bottle", 0, R_LIMBO);
    /* These two items never appear on the ground; they are either in R_LIMBO or R_INHAND. */
    new_obj(BOTTLE, "Small bottle", 0, R_HOUSE);
    objs[BOTTLE].desc[0] = "There is a bottle of water here.";
    objs[BOTTLE].desc[1] = "There is an empty bottle here.";
    objs[BOTTLE].desc[2] = "There is a bottle of oil here.";
    new_obj(FOOD, "Tasty food", 0, R_HOUSE);
    objs[FOOD].desc[0] = "There is food here.";
    new_obj(DWARF, 0, DWARF, R_LIMBO);
    new_obj(ROD, "Black rod", 0, R_DEBRIS);
    objs[ROD].desc[0] = "A three-foot black rod with a rusty star on an end lies nearby.";
    new_obj(CAGE, "Wicker cage", 0, R_COBBLES);
    objs[CAGE].desc[0] = "There is a small wicker cage discarded nearby.";
    new_obj(LAMP, "Brass lantern", 0, R_HOUSE);
    objs[LAMP].desc[0] = "There is a shiny brass lamp nearby.";
    objs[LAMP].desc[1] = "There is a lamp shining nearby.";
    new_obj(KEYS, "Set of keys", 0, R_HOUSE);
    objs[KEYS].desc[0] = "There are some keys on the ground here.";
}

void describe_object(ObjectWord t)
{
    const char *d = objs[t].desc[objs[t].prop];
    assert(0 <= objs[t].prop && objs[t].prop < 14);
    if (d != NULL) puts(d);
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

bool keyword(WordType type, int meaning)
{
    if (word1.type == type && word1.meaning == meaning) return true;
    if (word2.type == type && word2.meaning == meaning) return true;
    return false;
}

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
    if (objs[BLOB].prop > 0) {
        if (objs[BLOB].prop == 16) {
            objs[BLOB].prop = 17;
            puts("\nThe blob >ROAR<s triumphantly and bounces straight at you with amazing\n"
                 "speed, crushing you to the ground and cutting off your air supply\n"
                 "with its body.  You quickly suffocate.");
            return true;  /* you are dead */
        }
        puts("TODO FIXME BUG HACK blob warnings go here");
        objs[BLOB].prop += 1;
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
     * happens on 3 out of every 100 turns, I give a "heartwarming cheer"
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
                 "you emerge from the smoke and find....");
            break;
        case 2:
            puts("As you wish.  Hang on for just a second while I fire up my thurible...\n"
                 ">foof!<    An immense cloud of orange smoke fills the air.  As it\n"
                 "clears, you find that once again....");
            break;
        case 3:
            puts("Okay, now where did I put my orange smoke?....  >poof!<\n"
                 "Everything disappears in a dense cloud of orange smoke.");
            break;
        case 4:
            puts("Yes....  well, that's the kind of blinkered, Philistine pig-ignorance\n"
                 "that I've come to expect from you mortals.  You sit there on your\n"
                 "loathsome, spotty behind, squeezing blackheads and not caring a\n"
                 "thinker's damn about the struggling cave-sprite, you simpering,\n"
                 "whining, mouldy-faced heap of parrot droppings!  If you're so\n"
                 "smart, then you can just reincarnate yourself, because quite\n"
                 "frankly I'm as mad as hell and I'm not going to take this\n"
                 "anymore - I'm leaving!!!!");
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

/*========== The main loop. =============================================*/

int process_verb(Location loc)
{
    return 0;  /* unhandled */
}

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

bool now_in_darkness(Location loc)
{
    if (places[loc].flags & F_LIGHTED) return false;
    if (here(LAMP, loc) && objs[LAMP].prop) return false;
    return true;
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

void lamprey(Location loc)
{
    if (lamplife > 0) {
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
    } else if (closure == 2) {
        /* If the lamp starts running low during closing time,
         * just go ahead and close the cave already. */
        close_the_cave();
    } else {
        if (here(BATTERIES, loc) && !objs[BATTERIES].prop) {
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
            printf("I see no %s here.\n", word1.text);
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
                printf("I see no %s here.\n", word2.text);
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
    if (word2.type != WordType_None) {
        printf("Ok - \"%s\".\n", word2.text);
    }
}


void simulate_an_adventure(void)
{
    Location oldloc;
    Location loc = R_ROAD;
    Location newloc = R_ROAD;

    clock = 15+ran(10);
    dwarfcount = 4+ran(5);
    objs[CHAIN].prop = 1;
    objs[FOG].prop = 8;
    objs[FLASK].prop = 1;

    if (false) {
      death:
        kill_the_player(loc);
        oldloc = loc = newloc = R_HOUSE;
    }
    
    while (true) {
        if (blob_is_coming) {
            if (tick()) goto death;
        }
        if (newloc != loc) {
            /* If you stand still, dwarves in the room will attack. */
            if (there(DWARF, loc) && pct(backlash)) {
                if (dwarf_attack()) goto death;
            }
        }
        if (places[loc].flags & F_ONE_EXIT && there(DWARF, loc) && !wizard_mode) {
            puts("A little dwarf with a big knife blocks your way.");
            newloc = loc;
        } else {
            if (there(FOG, newloc)) {
                phog(newloc);
            }
            /* Okay, we moved! */
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
            if (here(LAMP, loc) && objs[LAMP].prop) {
                /* Interestingly, the lamp's batteries do not
                 * run down while you are elsewhere. This is
                 * a minor difference from Woods' Adventure. */
                lamplife -= 1;
                if (lamplife == 40 || lamplife == 0) {
                    lamprey(loc);
                    if (closure == 3) {
                        /* We just closed the cave on you. */
                        continue;
                    }
                }
            }
            if (!there(GOBLINS, R_LIMBO)) {
                /* If the goblins are active, they follow you. */
                apport(GOBLINS, loc);
                if (objs[GOBLINS].prop >= 0)
                    puts("You are being pursued by a vicious horde of little gooseberry goblins!");
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
        if (juggled) {
            /* If we've been shuffling objects, we'd better recompute the
             * player's inventory, so it doesn't get out of sync. */
            juggled = false;
            holding_count = 0;
            for (int i=0; i <= MAX_OBJ; ++i) {
                if (i == BIRD) continue;  /* it's caged */
                if (i == OIL || i == WATER) continue;  /* they're bottled */
                if (i == BEAR) continue;  /* it's following you */
                if (toting(i))
                    holding_count += 1;
            }
        }
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
            puts("Huh??");
            continue;
        }
        if (keyword(WordType_Verb, SAY)) {
            presay();
        }
        if (word2.type == WordType_BadWord) {
            printf("I don't know the word \"%s\".\n", word2.text);
            continue;
        }

        newloc = places[loc].f();
        if (R_LIMBO < newloc && newloc < R_YLEM) {
            continue;
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
            } else {
                assert(newloc == 0);  /* unhandled */
            }
            deal_with_syntax_errors(loc);
            newloc = loc;
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
             "                              - - -\n\n\n");
    }
    build_vocabulary();
    build_place_names();
    build_object_table();
    simulate_an_adventure();
    return 0;
}

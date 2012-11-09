
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

#define SOFT_NL "\n"
#define SOFT_HYPHEN "-\n"
#define HARD_HYPHEN "-\n"
#define EMDASH(x) "\xE2\x80\x94"  /* U+2014 "EM DASH" */
#define ENDASH(x) "\xE2\x80\x93"  /* U+2013 "EN DASH" */

bool pct(int percent) { return (ran(100) < percent); }
bool streq(const char *a, const char *b) { return !strncmp(a, b, 5); }

/*========== Forward declarations. ========================================
 */
void dwarves_upset(void);
void give_up(void);
void quit(void);
void history_of_adventure(void);
void attempt_take(Location loc, ActionWord verb, ObjectWord obj, PrepositionWord prep, ObjectWord iobj);
void attempt_insert_into(Location loc, ObjectWord obj, ObjectWord iobj);
void attempt_feed(Location loc, ObjectWord obj, ObjectWord iobj);
int attempt_kill(Location loc, ObjectWord obj, ObjectWord iobj);
int attempt_toss(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj);
void kill_a_dwarf(Location loc);
bool now_in_darkness(Location loc);


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
    } else if (MIN_PREPOSITION <= word && word <= MAX_PREPOSITION) {
        return WordClass_Preposition;
    } else if (MIN_ADJECTIVE <= word && word <= MAX_ADJECTIVE) {
        return WordClass_Adjective;
    } else if (word == AND) {
        return WordClass_Conjunction;
    } else if (word == BAD_WORD) {
        return WordClass_BadWord;
    } else {
        assert(false);
        return WordClass_None;
    }
}

const char ok[] = "OK.";  /* Woods' Fortran version didn't include the period, by the way. */
const char pitch_dark_msg[] = "It is now pitch dark.  If you proceed you will most likely fall into a pit.";


struct HashEntry {
    char text[7];
    int meaning;
};

#define HASH_PRIME 1009
struct HashEntry hash_table[HASH_PRIME];

void new_word(const char *w, int m)
{
    int h = 0;
    assert(strlen(w) <= 6);
    for (const char *p = w; *p != '\0'; ++p) {
        h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].meaning != 0)
        h = (h+1) % HASH_PRIME;
    strcpy(hash_table[h].text, w);
    hash_table[h].meaning = m;
}

int lookup(const char *w, WordClass c)
{
    int h = 0;
    for (const char *p = w; *p != '\0' && p != w+6; ++p) {
        h = ((h << 1) + *p) % HASH_PRIME;
    }
    while (hash_table[h].meaning != 0) {
        if (streq(w, hash_table[h].text)) {
            int m = hash_table[h].meaning;
            if (c == WordClass_None || word_class(m) == c)
                return m;
        }
        h = (h+1) % HASH_PRIME;
    }
    return NOTHING;
}
#undef HASH_PRIME

#define new_motion_word(s,m) new_word(s,m); assert(word_class(m)==WordClass_Motion)
#define new_object_word(s,m) new_word(s,m); assert(word_class(m)==WordClass_Object)
#define new_action_word(s,m) new_word(s,m); assert(word_class(m)==WordClass_Action)
#define new_message_word(s,m) new_word(s,m); assert(word_class(m)==WordClass_Message)
#define new_preposition(s,m) new_word(s,m); assert(word_class(m)==WordClass_Preposition)
#define new_adjective(s,m) new_word(s,m); assert(word_class(m)==WordClass_Adjective)
#define new_conjunction(s,m) new_word(s,m); assert(word_class(m)==WordClass_Conjunction)

void build_vocabulary(void)
{
    new_motion_word("road", ROAD); new_motion_word("hill", ROAD);
    new_motion_word("enter", ENTER);
    new_motion_word("upstre", UPSTREAM);
    new_motion_word("downst", DOWNSTREAM);
    new_motion_word("forest", FOREST);
    new_motion_word("forwar", FORWARD); new_motion_word("contin", FORWARD);
    new_motion_word("onward", FORWARD);
    new_motion_word("back", BACK); new_motion_word("return", BACK);
    new_motion_word("retrea", BACK);
    new_motion_word("valley", VALLEY);
    new_motion_word("stair", STAIRS); new_motion_word("stairs", STAIRS);
    new_motion_word("out", OUT); new_motion_word("outsid", OUT);
    new_motion_word("exit", OUT); new_motion_word("leave", OUT);
    new_motion_word("buildi", HOUSE); new_motion_word("house", HOUSE);
    new_motion_word("gully", GULLY);
    new_motion_word("stream", STREAM);
    new_motion_word("rock", ROCK);
    new_motion_word("bed", BED);
    new_motion_word("crawl", CRAWL);
    new_motion_word("cobble", COBBLES);
    new_motion_word("inward", IN); new_motion_word("inside", IN);
    new_motion_word("in", IN);
    new_motion_word("surfac", SURFACE);
    new_motion_word("null", NOWHERE); new_motion_word("nowher", NOWHERE);
    new_motion_word("dark", DARK);
    new_motion_word("passag", PASSAGE); new_motion_word("tunnel", PASSAGE);
    new_motion_word("defile", PASSAGE);
    new_motion_word("low", LOW);
    new_motion_word("canyon", CANYON);
    new_motion_word("awkwar", AWKWARD);
    new_motion_word("giant", GIANT);
    new_motion_word("view", VIEW);
    new_motion_word("upward", U); new_motion_word("up", U);
    new_motion_word("u", U); new_motion_word("above", U);
    new_motion_word("ascend", U);
    new_motion_word("d", D); new_motion_word("downwa", D);
    new_motion_word("down", D); new_motion_word("descen", D);
    new_motion_word("pit", PIT);
    new_motion_word("outdoo", OUTDOORS);
    new_motion_word("crack", CRACK);
    new_motion_word("steps", STEPS);
    new_motion_word("dome", DOME);
    new_motion_word("left", LEFT);
    new_motion_word("right", RIGHT);
    new_motion_word("hall", HALL);
    new_motion_word("jump", JUMP); new_motion_word("leap", JUMP);
    new_motion_word("barren", BARREN);
    new_motion_word("over", OVER);
    new_motion_word("across", ACROSS);
    new_motion_word("east", E); new_motion_word("e", E);
    new_motion_word("west", W); new_motion_word("w", W);
    new_motion_word("north", N); new_motion_word("n", N);
    new_motion_word("south", S); new_motion_word("s", S);
    new_motion_word("ne", NE); new_motion_word("northe", NE);
    new_motion_word("se", SE); new_motion_word("southe", SE);
    new_motion_word("sw", SW); new_motion_word("southw", SW);
    new_motion_word("nw", NW); new_motion_word("northw", NW);
    new_motion_word("debris", DEBRIS);
    new_motion_word("hole", HOLE);
    new_motion_word("wall", WALL);
    new_motion_word("broken", BROKEN);
    new_motion_word("y2", Y2);
    new_motion_word("climb", CLIMB);
    new_motion_word("pantry", PANTRY);
    new_motion_word("floor", FLOOR);
    new_motion_word("room", ROOM);
    new_motion_word("slit", SLIT);
    new_motion_word("slab", SLAB); new_motion_word("slabro", SLAB);
    new_motion_word("xyzzy", XYZZY);
    new_motion_word("depres", DEPRESSION);
    new_motion_word("entran", ENTRANCE);
    new_motion_word("plugh", PLUGH);
    new_motion_word("secret", SECRET);
    new_motion_word("cave", CAVE);
    new_motion_word("cross", CROSS);
    new_motion_word("bedqui", BEDQUILT);
    new_motion_word("plover", PLOVER);
    new_motion_word("orient", ORIENTAL);
    new_motion_word("cavern", CAVERN);
    new_motion_word("shell", SHELL);
    new_motion_word("reserv", RESERVOIR);
    new_motion_word("main", OFFICE); new_motion_word("office", OFFICE);
    new_motion_word("fork", FORK);
    new_motion_word("chimne", CHIMNEY);
    new_motion_word("slide", SLIDE);
    new_motion_word("pool", WHIRLPOOL); new_motion_word("whirl", WHIRLPOOL);
    new_motion_word("whirlp", WHIRLPOOL);
    new_motion_word("knoll", KNOLL);
    new_motion_word("phuce", PHUCE);
    new_motion_word("ledge", LEDGE);
    new_motion_word("shelf", LEDGE);
    new_motion_word("thunde", THUNDER);
    new_motion_word("click", CLICK);
    new_motion_word("ice", ICE);
    new_motion_word("bridge", BRIDGE);
    new_motion_word("altar", ALTAR);
    new_motion_word("grotto", GROTTO);
    new_motion_word("pray", PRAY); new_motion_word("prayer", PRAY);
    new_motion_word("saint-", SAINT_MICHEL);

    new_object_word("lamp", LAMP); new_object_word("lanter", LAMP);
    new_object_word("headla", LAMP); new_object_word("light", LAMP);
    new_object_word("grate", GRATE);
    new_object_word("cage", CAGE);
    new_object_word("rod", ROD); new_object_word("wand", ROD);  /* or ROD2 */
    new_object_word("steps", TREADS);
    new_object_word("pole", POLE);
    new_object_word("pillow", PILLOW);
    new_object_word("snake", SNAKE);
    new_object_word("fissur", FISSURE);
    new_object_word("tablet", TABLET);
    new_object_word("clam", CLAM);
    new_object_word("oyste", OYSTER);
    new_object_word("magazi", MAG); new_object_word("issue", MAG);
    new_object_word("spelun", MAG); new_object_word("\"spelu", MAG);
    new_object_word("dwarf", DWARF); new_object_word("dwarve", DWARF);
    new_object_word("knife", KNIFE); new_object_word("knives", KNIFE);
    new_object_word("food", FOOD); new_object_word("ration", FOOD);
    new_object_word("sandwi", FOOD);
    new_object_word("bottle", BOTTLE); new_object_word("jar", BOTTLE);
    new_object_word("chasm", CHASM);
    new_object_word("chasm", CLAY);
    new_object_word("mirror", MIRROR);
    new_object_word("plant", PLANT); new_object_word("beanst", PLANT);
    /* Long mistakenly extends the spelling of this noun from
     * Woods' "STALA" to "STALAGMITE". However, it really represents
     * the "large stalactite" which is scenery in R_TITE.
     * I've decided to accept both words. */
    new_object_word("stalag", STALACTITE);
    new_object_word("stalac", STALACTITE);
    new_object_word("figure", SHADOW);
    new_object_word("axe", AXE);
    new_object_word("drawin", DRAWINGS);
    new_object_word("pirate", PIRATE);
    new_object_word("dragon", DRAGON);
    new_object_word("troll", TROLL);
    new_object_word("bear", BEAR);
    new_object_word("messag", MESSAGE);
    new_object_word("volcan", GORGE); new_object_word("geyser", GORGE);
    new_object_word("machin", MACHINE);
    new_object_word("batter", BATTERIES);
    new_object_word("moss", MOSS); new_object_word("carpet", MOSS);
    new_object_word("door", RUSTY_DOOR);  /* or TINY_DOOR or BOOTH_DOOR */
    new_object_word("flower", FLOWERS);
    new_object_word("cloak", CLOAK); new_object_word("cape", CLOAK);
    new_object_word("boat", BOAT); new_object_word("ship", BOAT);
    new_object_word("rowboa", BOAT);
    new_object_word("stick", STICKS); new_object_word("sticks", STICKS);
    new_object_word("nugget", GOLD);
    new_object_word("diamon", DIAMONDS);
    new_object_word("horn", HORN);
    new_object_word("jewelr", JEWELS); new_object_word("jewels", JEWELS);
    new_object_word("coins", COINS);
    new_object_word("chest", CHEST); new_object_word("box", CHEST);
    new_object_word("treas", CHEST);
    new_object_word("eggs", EGGS); new_object_word("egg", EGGS);
    new_object_word("nest", EGGS);
    new_object_word("triden", TRIDENT);
    new_object_word("vase", VASE); new_object_word("shards", VASE);
    new_object_word("shard", VASE); new_object_word("potter", VASE);
    new_object_word("emeral", EMERALD);
    new_object_word("pyrami", PYRAMID);
    new_object_word("pearl", PEARL);
    new_object_word("rug", RUG);
    new_object_word("spices", SPICES);
    new_object_word("chain", CHAIN);
    new_object_word("sword", SWORD);
    new_object_word("crown", CROWN);
    new_object_word("shoes", SHOES); new_object_word("heels", SHOES);
    new_object_word("slippe", SHOES);
    new_object_word("lyre", LYRE);
    new_object_word("sapphi", SAPPHIRE);
    new_object_word("grail", GRAIL); new_object_word("cup", GRAIL);
    new_object_word("chalic", GRAIL);
    new_object_word("cask", CASK); new_object_word("keg", CASK);
    new_object_word("ring", RING);
    new_object_word("clover", CLOVER);
    new_object_word("tree", TREE);
    new_object_word("drople", DROPLET);
    /* Notice the misspelling of "H2O" as "H20". This misspelling
     * is not found in Woods, but it is found in both versions of
     * Long's expansion that I have seen. Perhaps Long or one of
     * the anonymous authors miscorrected it at some point. */
    new_object_word("water", WATER); new_object_word("h20", WATER);
    new_object_word("oil", OIL);
    new_object_word("wine", WINE);
    new_object_word("bee", BEES); new_object_word("bees", BEES);
    new_object_word("bumble", BEES);
    new_object_word("wall", ECHO);  /* or WALL or SAFE_WALL */
    new_object_word("key", TINY_KEY);
    new_object_word("anvil", ANVIL);
    new_object_word("rocks", CLOAKROOM_ROCKS);  /* or CARVING */
    /* Notice that PHONEBOOTH is a synonym for PHONE, not BOOTH. */
    new_object_word("booth", BOOTH);
    new_object_word("teleph", PHONE);
    new_object_word("phone", PHONE);
    new_object_word("phoneb", PHONE);
    new_object_word("slugs", SLUGS);
    new_object_word("honey", HONEY); new_object_word("comb", HONEY);
    new_object_word("honeyc", HONEY);
    new_object_word("beehiv", HIVE); new_object_word("hive", HIVE);
    new_object_word("dog", DOG); new_object_word("hound", DOG);
    new_object_word("wolf", DOG);
    new_object_word("wumpus", WUMPUS);
    new_object_word("bird", BIRD);
    new_object_word("keys", KEYS);
    new_object_word("founta", FOUNTAIN);
    new_object_word("bat", BATS); new_object_word("bats", BATS);
    new_object_word("batshi", BATS); new_object_word("guano", BATS);
    new_object_word("gnome", GNOME);
    new_object_word("mushro", MUSHROOMS); new_object_word("peyote", MUSHROOMS);
    new_object_word("cakes", CAKES); new_object_word("cake", CAKES);
    new_object_word("cupcak", CAKES);
    new_object_word("sack", SACK);
    new_object_word("knapsa", SACK);
    new_object_word("all", ALL); new_object_word("everyt", ALL);
    new_object_word("book", BOOK); new_object_word("tome", BOOK);
    new_object_word("volume", BOOK);  /* or REPO_BOOK */
    new_object_word("safe", SAFE);
    new_object_word("poster", POSTER);
    /* Notice that WHISKBROOM is not a synonym of BROOM. */
    new_object_word("broom", BROOM); new_object_word("whisk", BROOM);
    new_object_word("brush", BROOM);
    new_object_word("carvin", CARVING);
    new_object_word("billbo", BILLBOARD);
    /* Long had "cannister" throughout; I've fixed the spelling
     * in this version. */
    new_object_word("canist", CANISTER); new_object_word("tube", CANISTER);
    new_object_word("shield", CANISTER);
    new_object_word("radium", RADIUM); new_object_word("rock", RADIUM);
    new_object_word("stone", RADIUM);
    new_object_word("sphere", BALL); new_object_word("ball", BALL);

    new_action_word("take", TAKE); new_action_word("carry", TAKE);
    new_action_word("keep", TAKE); new_action_word("catch", TAKE);
    new_action_word("captur", TAKE); new_action_word("steal", TAKE);
    new_action_word("tote", TAKE);
    new_action_word("drop", DROP); new_action_word("releas", DROP);
    new_action_word("free", DROP); new_action_word("discar", DROP);
    new_action_word("dump", DROP); new_action_word("doff", DROP);
    new_action_word("say", SAY); new_action_word("chant", SAY);
    new_action_word("sing", SAY); new_action_word("utter", SAY);
    new_action_word("mumble", SAY);
    new_action_word("open", OPEN);
    new_action_word("nothin", RELAX);
    new_action_word("close", CLOSE);
    new_action_word("shut", CLOSE);
    new_action_word("light", LIGHT);
    new_action_word("exting", EXTINGUISH);
    new_action_word("wave", WAVE); new_action_word("shake", WAVE);
    new_action_word("swing", WAVE);
    new_action_word("calm", CALM); new_action_word("placat", CALM);
    new_action_word("tame", CALM);
    new_action_word("walk", GO); new_action_word("run", GO);
    new_action_word("travel", GO); new_action_word("go", GO);
    new_action_word("procee", GO); new_action_word("explor", GO);
    new_action_word("goto", GO); new_action_word("follow", GO);
    /* Note that CONTINUE is both a synonym for motion-FORWARD and
     * a synonym for action-GO. CONTINUE WEST means GO WEST, but
     * CONTINUE alone means FORWARD. */
    new_action_word("contin", GO); new_action_word("hike", GO);
    new_action_word("attack", KILL); new_action_word("kill", KILL);
    new_action_word("fight", KILL); new_action_word("slay", KILL);
    new_action_word("pour", POUR); new_action_word("empty", POUR);
    new_action_word("eat", EAT); new_action_word("devour", EAT);
    new_action_word("drink", DRINK);
    new_action_word("rub", RUB);
    new_action_word("throw", TOSS); new_action_word("toss", TOSS);
    new_action_word("hurl", TOSS); new_action_word("fling", TOSS);
    new_action_word("heave", TOSS);
    new_action_word("quit", QUIT); new_action_word("q", QUIT);
    new_action_word("find", FIND); new_action_word("where", FIND);
    new_action_word("invent", INVENTORY); new_action_word("i", INVENTORY);
    new_action_word("feed", FEED);
    new_action_word("fill", FILL); new_action_word("refill", FILL);
    new_action_word("blast", BLAST); new_action_word("detona", BLAST);
    new_action_word("ignite", BLAST); new_action_word("blowup", BLAST);
    new_action_word("score", SCORE);
    new_action_word("fee", FEEFIE); new_action_word("fie", FEEFIE);
    new_action_word("foe", FEEFIE); new_action_word("foo", FEEFIE);
    new_action_word("fum", FEEFIE);
    new_action_word("brief", BRIEF); new_action_word("unbrie", BRIEF);
    new_action_word("read", READ); new_action_word("peruse", READ);
    new_action_word("break", BREAK); new_action_word("smash", BREAK);
    new_action_word("shatt", BREAK);
    new_action_word("wake", WAKE); new_action_word("distur", WAKE);
    new_action_word("waken", WAKE); new_action_word("awaken", WAKE);
    new_action_word("yank", YANK); new_action_word("jerk", YANK);
    new_action_word("pull", YANK); new_action_word("grab", YANK);
    new_action_word("wear", WEAR); new_action_word("don", WEAR);
    new_action_word("hit", HIT); new_action_word("strike", HIT);
    new_action_word("kick", HIT); new_action_word("punch", HIT);
    new_action_word("pound", HIT); new_action_word("whack", HIT);
    new_action_word("answer", ANSWER); new_action_word("reply", ANSWER);
    new_action_word("blow", BLOW);
    new_action_word("leave", LEAVE);
    new_action_word("call", CALL);
    new_action_word("dial", DIAL);
    new_action_word("play", PLAY); new_action_word("strum", PLAY);
    new_action_word("pick", PICK);
    new_action_word("put", PUT);
    new_action_word("turn", TURN);
    new_action_word("get", GET);
    new_action_word("insert", INSERT);
    new_action_word("remove", REMOVE);
    new_action_word("burn", BURN);
    new_action_word("gripe", GRIPE); new_action_word("bitch", GRIPE);
    new_action_word("compla", GRIPE); new_action_word("sugges", GRIPE);
    new_action_word("report", GRIPE);
    new_action_word("lock", LOCK);
    new_action_word("unlock", UNLOCK);
    new_action_word("health", DIAGNOSE); new_action_word("diagno", DIAGNOSE);
    new_action_word("look", LOOK); new_action_word("l", LOOK);
    new_action_word("examin", LOOK); new_action_word("descri", LOOK);
    new_action_word("7", COMBO); new_action_word("22", COMBO);
    new_action_word("34", COMBO);
    new_action_word("sweep", SWEEP); new_action_word("brush", SWEEP);
    new_action_word("dust", SWEEP);
    new_action_word("terse", TERSE); new_action_word("unters", TERSE);
#ifdef SAVE_AND_RESTORE
    new_action_word("save", SAVE); new_action_word("pause", SAVE);
    new_action_word("suspen", SAVE);
    new_action_word("restor", RESTORE);
#endif /* SAVE_AND_RESTORE */

    /* Finally, our vocabulary is rounded out by words like HELP, which
     * trigger the printing of fixed messages. */
    new_message_word("abra", ABRA);
    new_message_word("abracd", ABRA);  /* Long's typo */
    new_message_word("alacaz", ABRA); new_message_word("opense", ABRA);
    new_message_word("hocus", ABRA); new_message_word("sesame", ABRA);
    new_message_word("pocus", ABRA); new_message_word("shazam", ABRA);
    new_message_word("help", HELP); new_message_word("?", HELP);
    /* TODO: disambiguate TREE */
    new_message_word("tree", TREES); new_message_word("trees", TREES);
    new_message_word("dig", DIG); new_message_word("excava", DIG);
    new_message_word("lost", LOST);
    new_message_word("mist", MIST);
    new_message_word("fuck", FUCK); new_message_word("fuck!", FUCK);
    new_message_word("stop", STOP);
    new_message_word("swim", SWIM);
    new_message_word("shit", SHIT); new_message_word("shit!", SHIT);
    new_message_word("crap", SHIT); new_message_word("crap!", SHIT);
    new_message_word("piss", PISS); new_message_word("piss!", PISS);

    /* Long's "Adventure" adds prepositions, adjectives, and
     * conjunctions. */
    new_preposition("in", INTO); new_preposition("into", INTO);
    new_preposition("inside", INTO);
    new_preposition("on", ONTO); new_preposition("onto", ONTO);
    new_preposition("upon", ONTO);
    new_preposition("with", WITH);
    new_preposition("to", TO);
    new_preposition("from", FROM);
    new_preposition("off", OFF);
    new_preposition("up", PREP_UP);
    new_preposition("down", PREP_DOWN);
    new_preposition("at", AT);

    new_adjective("golden", GOLDEN); new_adjective("gold", GOLDEN);
    new_adjective("persia", PERSIAN);
    new_adjective("wooden", WOODEN);
    new_adjective("brass", BRASS);
    new_adjective("wicker", WICKER);
    new_adjective("black", BLACK);
    new_adjective("velvet", VELVET);
    new_adjective("vendin", VENDING);
    new_adjective("worn-o", WORNOUT); new_adjective("wornou", WORNOUT);
    new_adjective("worn", WORNOUT); new_adjective("used", WORNOUT);
    new_adjective("fresh", FRESH);
    new_adjective("iron", IRON);
    new_adjective("tiny", TINY); new_adjective("small", TINY);
    new_adjective("little", TINY);
    new_adjective("silken", SILKEN); new_adjective("silk", SILKEN);
    new_adjective("silver", SILVER);
    new_adjective("treasu", TREASURE);
    new_adjective("ming", MING);
    new_adjective("platin", PLATINUM);
    new_adjective("rare", RARE);
    new_adjective("elfin", ELFIN);
    new_adjective("ruby", RUBY);
    new_adjective("holy", HOLY);
    new_adjective("oaken", OAKEN); new_adjective("oak", OAKEN);
    new_adjective("leaden", LEADEN); new_adjective("lead", LEADEN);
    new_adjective("leathe", LEATHER);
    new_adjective("steel", STEEL);
    new_adjective("star", STAR);
    new_adjective("shadow", SHADOWY);
    new_adjective("rusty", RUSTY);
    new_adjective("giant", ADJ_GIANT);
    new_adjective("tasty", TASTY);
    new_adjective("glowin", GLOWING);
    new_adjective("green", GREEN);
    new_adjective("grey", GREY);
    new_adjective("crysta", CRYSTAL);
    new_adjective("4-leaf", FOURLEAF); new_adjective("four-l", FOURLEAF);
    new_adjective("metal", METAL);
    new_adjective("quartz", QUARTZ);

    new_conjunction("and", AND);
}

/*========== Locations. ===================================================
 * This section corresponds to sections 18--62 in Knuth.
 */

typedef struct {
    MotionWord mot;
    int cond;
    Location dest;
} Instruction;

Instruction travels[1191];
Instruction *start[MAX_LOC+2];
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
    assert(&travels[0] <= q && q <= &travels[1191]);
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
             "You're at end of road again.", F_LIGHTED | F_WATER | F_OUTSIDE);
    make_ins(W, R_HILL); ditto(U); ditto(ROAD); ditto(CLIMB);
    make_ins(E, R_HOUSE); ditto(IN); ditto(HOUSE); ditto(ENTER);
    make_ins(S, R_VALLEY); ditto(D); ditto(GULLY); ditto(STREAM); ditto(DOWNSTREAM);
    make_ins(N, R_FOREST); ditto(FOREST);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_ins(KNOLL, R_KNOLL);
    make_ins(THUNDER, R_THUNDER); ditto(HOLE);
    make_loc(q, R_HILL,
             "You have walked up a hill, still in the forest.  The road slopes back" SOFT_NL
             "down the other side of the hill.  There is a building in the distance.",
             "You're at hill in road.", F_LIGHTED | F_OUTSIDE);
    make_ins(ROAD, R_ROAD); ditto(HOUSE); ditto(FORWARD); ditto(E); ditto(N); ditto(D);
    make_cond_ins(FOREST, 30, R_KNOLL);
    make_ins(FOREST, R_FOREST); ditto(S);
    make_ins(W, R_KNOLL);
    make_loc(q, R_HOUSE,
             "You are inside a building, a well house for a large spring.  Off" SOFT_NL
             "to one side is a small pantry.",
             "You're inside building.", F_LIGHTED | F_WATER | F_OUTSIDE);
    make_ins(OUT, R_ROAD); ditto(OUTDOORS); ditto(W);
    make_ins(XYZZY, R_DEBRIS);
    make_ins(PLUGH, R_Y2);
    make_cond_ins(CLICK, unless_prop(SHOES, 1), remark(29));
    make_ins(CLICK, R_RAINBOW);
    make_ins(DOWNSTREAM, remark(17)); ditto(STREAM);
    make_ins(PANTRY, R_PANTRY); ditto(ENTER); ditto(IN);
    make_loc(q, R_VALLEY,
             "You are in a valley in the forest beside a stream tumbling along a" SOFT_NL
             "rocky bed.",
             "You're in valley.", F_LIGHTED | F_WATER | F_OUTSIDE);
    make_ins(UPSTREAM, R_ROAD); ditto(HOUSE); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(U);
    make_ins(DOWNSTREAM, R_SLIT); ditto(S); ditto(D);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_FOREST,
             "You are in open forest, with a deep valley to one side.  Not far" SOFT_NL
             "is a large billboard.",
             "You're in forest.", F_LIGHTED | F_OUTSIDE);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(D);
    make_cond_ins(FOREST, 50, R_FOREST); ditto(FORWARD); ditto(N);
    make_ins(FOREST, R_FOREST2);
    make_ins(W, R_FOREST); ditto(S);
    make_loc(q, R_FOREST2,
             "You are in open forest near both a valley and a road.",
             "You're in forest.", F_LIGHTED | F_OUTSIDE);
    make_ins(ROAD, R_ROAD); ditto(N);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(W); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(S);
    make_loc(q, R_SLIT,
             "At your feet all the water of the stream splashes into a 2-inch slit" SOFT_NL
             "in the rock.  Downstream the streambed is bare rock.",
             "You're at slit in streambed.", F_LIGHTED | F_WATER | F_OUTSIDE);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_VALLEY); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W);
    /* Long actually breaks the ROCK direction here by editing it to ROCKY, but
     * since this is clearly unintentional, I'm keeping Woods' spelling. */
    make_ins(DOWNSTREAM, R_OUTSIDE); ditto(ROCK); ditto(BED); ditto(S);
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D);
    make_loc(q, R_OUTSIDE,
             "You are in a 20-foot depression floored with bare dirt.  Set into the" SOFT_NL
             "dirt is a strong steel grate mounted in concrete.  A dry streambed" SOFT_NL
             "leads into the depression.",
             "You're outside grate.", F_LIGHTED | F_CAVE_HINT | F_OUTSIDE);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(S);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_SLIT); ditto(GULLY); ditto(N);
    make_cond_ins(ENTER, unless_prop(GRATE, 0), R_INSIDE); ditto(IN); ditto(D);
    make_ins(ENTER, remark(1));
    make_loc(q, R_INSIDE,
             "You are in a small chamber beneath a 3x3 steel grate to the surface." SOFT_NL
             "A low crawl over cobbles leads inwards to the west.",
             "You're below the grate.", F_LIGHTED | F_PORTAL);
    make_cond_ins(OUT, unless_prop(GRATE, 0), R_OUTSIDE); ditto(U);
    make_ins(OUT, remark(1));
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(IN); ditto(W);
    make_ins(PIT, R_SPIT);
    make_ins(DEBRIS, R_DEBRIS);
    /* Crowther gives the cobble crawl F_LIGHTED; Long, strangely, does not. */
    make_loc(q, R_COBBLES,
             "You are crawling over cobbles in a low passage.  There is a dim light" SOFT_NL
             "at the east end of the passage.",
             "You're in cobble crawl.", F_PORTAL);
    make_ins(OUT, R_INSIDE); ditto(SURFACE); ditto(E);
    make_ins(IN, R_DEBRIS); ditto(DARK); ditto(W); ditto(DEBRIS);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_DEBRIS,
             "You are in a debris room filled with stuff washed in from the surface." SOFT_NL
             "A low wide passage with cobbles becomes plugged with mud and debris" SOFT_NL
             "here, but an awkward canyon leads upward and west.  A note on the wall" SOFT_NL
             "says \"MAGIC WORD XYZZY\".",
             "You're in Debris Room.", F_PORTAL);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(PASSAGE); ditto(LOW); ditto(E);
    make_ins(CANYON, R_AWK); ditto(IN); ditto(U); ditto(W);
    make_ins(XYZZY, R_HOUSE);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_AWK,
             "You are in an awkward sloping east/west canyon.",
             NULL, F_PORTAL);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(D, R_DEBRIS); ditto(E); ditto(DEBRIS);
    make_ins(IN, R_BIRD); ditto(U); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_BIRD,
             "You are in a splendid chamber thirty feet high. The walls are frozen" SOFT_NL
             "rivers of orange stone. An awkward canyon and a good passage exit" SOFT_NL
             "from east and west sides of the chamber.",
             "You're in Bird Chamber.", F_BIRD_HINT | F_PORTAL);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(DEBRIS, R_DEBRIS);
    make_ins(CANYON, R_AWK); ditto(E);
    make_ins(PASSAGE, R_SPIT); ditto(PIT); ditto(W);
    make_loc(q, R_SPIT,
             "At your feet is a small pit breathing traces of white mist. An east" SOFT_NL
             "passage ends here except for a small crack leading on.",
             "You're at top of small pit.", F_PORTAL);
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
    /* Crowther and Woods make "Y2" teleport you past the snake into the
     * jumble of rocks R_JUMBLE. Long straightens this out and lets you
     * teleport to R_Y2 instead. */
    make_ins(Y2, R_Y2);
    make_loc(q, R_NUGGET,
             "This is a low room with a crude note on the wall. The note says," SOFT_NL
             "\"You won't get it up the steps\".",
             "You're in Nugget of Gold Room.", 0);
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
    make_ins(S, R_MAZEA42); ditto(U); ditto(PASSAGE); ditto(CLIMB);
    make_ins(E, R_WFISS);
    make_ins(W, R_ELONG); ditto(CRAWL);
    make_ins(N, R_DUCK);

    make_loc(q, R_MAZEA42, all_alike, NULL, F_TWIST_HINT);
    make_ins(U, R_WMIST);
    make_ins(N, R_MAZEA42);
    make_ins(E, R_MAZEA43);
    make_ins(S, R_MAZEA45);
    make_ins(W, R_MAZEA80);
    make_loc(q, R_MAZEA43, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA42);
    make_ins(S, R_MAZEA44);
    make_ins(E, R_MAZEA45);
    make_loc(q, R_MAZEA44, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_MAZEA43);
    make_ins(D, R_MAZEA48);
    make_ins(S, R_MAZEA50);
    make_ins(N, R_MAZEA82);
    make_loc(q, R_MAZEA45, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA42);
    make_ins(N, R_MAZEA43);
    make_ins(E, R_MAZEA46);
    make_ins(S, R_MAZEA47);
    make_ins(U, R_MAZEA87); ditto(D);
    make_loc(q, R_MAZEA49, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_MAZEA50);
    make_ins(W, R_MAZEA51);
    make_loc(q, R_MAZEA50, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_MAZEA44);
    make_ins(W, R_MAZEA49);
    make_ins(D, R_MAZEA51);
    make_ins(S, R_MAZEA52);
    make_loc(q, R_MAZEA51, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA49);
    make_ins(U, R_MAZEA50);
    make_ins(E, R_MAZEA52);
    make_ins(S, R_MAZEA53);
    make_loc(q, R_MAZEA52, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA50);
    make_ins(E, R_MAZEA51);
    make_ins(S, R_MAZEA52);
    make_ins(U, R_MAZEA53);
    make_ins(N, R_MAZEA55);
    make_ins(D, R_MAZEA86);
    make_loc(q, R_MAZEA53, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA51);
    make_ins(N, R_MAZEA52);
    make_ins(S, R_MAZEA54);
    make_loc(q, R_MAZEA55, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA52);
    make_ins(N, R_MAZEA55);
    make_ins(D, R_MAZEA56);
    make_ins(E, R_BRINK);
    make_loc(q, R_MAZEA80, all_alike, NULL, F_TWIST_HINT);
    make_ins(N, R_MAZEA42);
    make_ins(W, R_MAZEA80); ditto(S);
    make_ins(E, R_MAZEA81);
    make_loc(q, R_MAZEA83, all_alike, NULL, F_TWIST_HINT);
    make_ins(S, R_BRINK);
    make_ins(E, R_MAZEA84);
    make_ins(W, R_MAZEA85);
    make_loc(q, R_MAZEA84 , all_alike, NULL, F_TWIST_HINT);
    make_ins(N, R_BRINK);
    make_ins(W, R_MAZEA83);
    make_ins(NW, R_PIRATES_NEST);  /* NW: a dirty trick! */
    make_loc(q, R_MAZEA87, all_alike, NULL, F_TWIST_HINT);
    make_ins(U, R_MAZEA45); ditto(D);
    make_loc(q, R_BRINK,
             "You are on the brink of a thirty-foot pit with a massive orange column" SOFT_NL
             "down one wall.  You could climb down here but you could not get back" SOFT_NL
             "up.  The maze continues at this level.",
             "You're at brink of pit.", 0);
    make_ins(D, R_BIRD); ditto(CLIMB);
    make_ins(W, R_MAZEA55);
    make_ins(S, R_MAZEA58);
    make_ins(N, R_MAZEA83);
    make_ins(E, R_MAZEA84);
    make_loc(q, R_ELONG,
             "You are at the east end of a very long hall apparently without side" SOFT_NL
             "chambers.  In the south wall are several wide cracks and a high" SOFT_NL
             "hole, but the hole is far above your head.  To the east a wide" SOFT_NL
             "crawl slants up.  To the north a round two foot hole slants down.",
             "You're at east end of Long Hall.", 0);
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL);
    make_ins(W, R_WLONG);
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE);
    make_loc(q, R_WLONG,
             "You are at the west end of a very long featureless hall.  The hall" SOFT_NL
             "joins up with a narrow north/south passage.",
             "You're at west end of Long Hall.", 0);
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
    make_cond_ins(U, unless_prop(ECHO, 0), R_ARCHED_RUBBLE); ditto(HALL);
    make_ins(U, R_ARCHED); ditto(HALL);
    make_ins(D, R_RAGGED);
    make_cond_ins(S, only_if_toting(CLAM), remark(4));
    make_cond_ins(S, only_if_toting(OYSTER), remark(5));
    make_ins(S, R_COMPLEX);
    make_loc(q, R_ARCHED,
             "You are in an arched hall.  A coral passage once continued up and east" SOFT_NL
             "from here, but is now blocked by debris.  The air smells of sea water.",
             "You're in Arched Hall.", 0);
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
             "A sign in midair here says \"Cave under construction beyond this point." SOFT_NL
             "Proceed at own risk.  [Witt Construction Company]\"",
             "You're in Anteroom.", 0);
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
    make_cond_ins(S, 40, remark(6));  /* lowered from 80 in Woods' version */
    make_ins(SLAB, R_SLAB);
    make_cond_ins(U, 40, remark(6));  /* lowered from 80 */
    make_cond_ins(U, 50, R_ABOVEP);
    make_ins(U, R_DUSTY);
    make_cond_ins(N, 40, remark(6));  /* lowered from 60 */
    make_cond_ins(N, 50, R_LOW);      /* lowered from 75 */
    make_ins(N, R_SJUNC);
    make_cond_ins(D, 50, remark(6));  /* lowered from 80 */
    make_ins(D, R_ANTE);
    make_loc(q, R_SWISS,
             "You are in a room whose walls resemble Swiss cheese.  Obvious passages" SOFT_NL
             "go west, east, NE, and NW.  Part of the room is occupied by a large" SOFT_NL
             "bedrock block.",
             "You're in Swiss Cheese Room.", 0);
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
    make_ins(SE, R_BEDQUILT);
    make_ins(S, R_ABOVEP);
    make_ins(N, R_WINDOW);
    make_loc(q, R_TITE,
             "A large stalactite extends from the roof and almost reaches the floor" SOFT_NL
             "below.  You could climb down it, and jump from it to the floor, but" SOFT_NL
             "having done so you would be unable to reach it to climb back up.",
             "You're at top of stalactite.", 0);
    make_ins(N, R_ABOVEP);
    make_cond_ins(D, 40, R_MAZEA50); ditto(JUMP); ditto(CLIMB);
    make_cond_ins(D, 50, R_MAZEA53);
    make_ins(D, R_MAZEA45);
    make_loc(q, R_LOW,
             "You are in a large low room.  Crawls lead north, NE, and SW.",
             NULL, 0);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_ins(NE, R_SLOPING);  /* this was SW in Woods */
    make_ins(N, R_CRAWL);
    make_ins(SW, R_ORIENTAL); ditto(ORIENTAL);  /* this was SE in Woods */
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
    /* Long flips some of the geometry in this area. The crawl to R_LOW went west,
     * not east, in the original. */
    make_loc(q, R_ORIENTAL,
             "This is the Oriental Room.  Ancient oriental cave drawings cover the" SOFT_NL
             "walls.  A gently sloping passage leads upward to the north, another" SOFT_NL
             "passage leads SE, and a hands-and-knees crawl leads east.",
             "You're in Oriental Room.", 0);
    make_ins(SE, R_SWISS);
    make_ins(E, R_LOW); ditto(CRAWL);
    make_ins(U, R_MISTY); ditto(N); ditto(CAVERN);
    make_loc(q, R_MISTY,
             "You are following a wide path around the outer edge of a large cavern." SOFT_NL
             "Far below, through a heavy white mist, strange splashing noises can be" SOFT_NL
             "heard.  The mist rises up through a fissure in the ceiling.  The path" SOFT_NL
             "hugs the cavern's rim to the NE and south, while another branch forks" SOFT_NL
             "west.  A round chute with extremely smooth walls angles sharply up" SOFT_NL
             "to the southwest.",
             "You're in Misty Cavern.", 0);
    make_ins(NE, R_DANTES);
    make_ins(S, R_ORIENTAL); ditto(ORIENTAL);
    make_ins(W, R_ALCOVE);
    make_cond_ins(U, 3, R_CHIMTOP); ditto(SW); ditto(CLIMB); ditto(SLIDE);
    make_cond_ins(U, 75, remark(25)); ditto(SW); ditto(CLIMB); ditto(SLIDE);
    make_ins(U, remark(26)); ditto(SW); ditto(CLIMB); ditto(SLIDE);
    make_ins(CHIMNEY, R_SWORD);
    /* Woods doesn't light the alcove; Long does. */
    make_loc(q, R_ALCOVE,
             "You are in an alcove.  A small NW path seems to widen after a short" SOFT_NL
             "distance.  An extremely tight tunnel leads east.  It looks like a very" SOFT_NL
             "tight squeeze.  An eerie light can be seen at the other end.",
             "You're in Alcove.", F_LIGHTED | F_DARK_HINT);
    make_ins(NW, R_MISTY); ditto(CAVERN);
    make_ins(E, R_PPASS); ditto(PASSAGE);
    make_ins(E, R_PLOVER);  /* never performed, but seen by "BACK" */
    make_loc(q, R_PLOVER,
             "You're in a small chamber lit by an eerie green light.  An extremely" SOFT_NL
             "narrow tunnel exits to the west.  A dark corridor leads NE.",
             "You're in Plover Room.", F_LIGHTED | F_DARK_HINT);
    make_ins(W, R_PPASS); ditto(PASSAGE); ditto(OUT);
    make_ins(W, R_ALCOVE);  /* never performed, but seen by "BACK" */
    /* Long adds the word "Y2" for teleporting back to R_Y2 from here. */
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP); ditto(Y2);
    make_ins(PLOVER, R_Y2); ditto(Y2);
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
             "You're in Mirror Canyon.", 0);
    make_ins(S, R_ABOVER);
    make_ins(N, R_RES); ditto(RESERVOIR);
    make_loc(q, R_RES,
             "You are at the edge of a large underground reservoir.  An opaque cloud" SOFT_NL
             "of white mist fills the room and rises rapidly upward.  The lake is" SOFT_NL
             "fed by a stream, which tumbles out of a hole in the wall about 10 feet" SOFT_NL
             "overhead and splashes noisily into the water somewhere within the" SOFT_NL
             "mist.  The only passage goes back toward the south.",
             "You're at Reservoir.", F_WATER);
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
    make_cond_ins(OVER, unless_prop(BRIDGE, 0), remark(12));
    make_ins(OVER, R_TROLL);
    make_cond_ins(JUMP, unless_prop(BRIDGE, 0), R_LOSE);
    make_ins(JUMP, remark(2));
    make_loc(q, R_DEAD0, dead_end, NULL, 0);
    make_ins(S, R_CROSS); ditto(OUT);
    make_loc(q, R_MAZEA81, dead_end, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA80); ditto(OUT);
    make_loc(q, R_PIRATES_NEST, dead_end, NULL, 0);
    make_ins(SE, R_MAZEA84);
    make_loc(q, R_MAZEA46, dead_end, NULL, F_TWIST_HINT);
    make_ins(W, R_MAZEA45); ditto(OUT);
    make_loc(q, R_MAZEA47, dead_end, 0, F_TWIST_HINT);
    make_ins(E, R_MAZEA45); ditto(OUT);
    make_loc(q, R_MAZEA48, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_MAZEA44); ditto(OUT);
    make_loc(q, R_MAZEA54, dead_end, 0, F_TWIST_HINT);
    make_ins(W, R_MAZEA53); ditto(OUT);
    make_loc(q, R_MAZEA56, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_MAZEA55); ditto(OUT);
    make_loc(q, R_MAZEA58, dead_end, 0, 0);
    make_ins(E, R_BRINK); ditto(OUT);
    make_loc(q, R_MAZEA82, dead_end, 0, F_TWIST_HINT);
    make_ins(S, R_MAZEA44); ditto(OUT);
    make_loc(q, R_MAZEA85, dead_end, 0, F_TWIST_HINT);
    make_ins(E, R_MAZEA83); ditto(OUT);
    make_loc(q, R_MAZEA86, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_MAZEA52); ditto(OUT);
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
             "heat.  The only exit is a crawl heading west, through which a low" SOFT_NL
             "rumbling noise is coming.",
             "You're in Chamber of Boulders.", 0);
    make_ins(W, R_WARM); ditto(OUT); ditto(CRAWL);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_LIME,
             "You are walking along a gently sloping north/south passage lined with" SOFT_NL
             "oddly shaped limestone formations.",
             "You're in Limestone Passage.", 0);
    make_ins(N, R_FORK); ditto(U); ditto(FORK);
    make_ins(S, R_FBARR); ditto(D); ditto(BARREN);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_FBARR,
             "You are standing at the entrance to a large, barren room.  A sign" SOFT_NL
             "posted above the entrance reads:  \"CAUTION!  BEAR IN ROOM!\"",
             "You're in front of Barren Room.", 0);
    make_ins(W, R_LIME); ditto(U);
    make_ins(FORK, R_FORK);
    make_ins(E, R_BARR); ditto(IN); ditto(BARREN); ditto(ENTER);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_BARR,
             "You are inside a barren room.  The center of the room is completely" SOFT_NL
             "empty except for some dust.  Marks in the dust lead away toward the" SOFT_NL
             "far end of the room.  The only exit is the way you came in.",
             "You're in Barren Room.", 0);
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
             "are sleeping on the floor, snoring loudly.  A sign nearby reads:  \"DO" SOFT_NL
             "NOT DISTURB THE DWARVES!\"  An immense mirror is hanging against one" SOFT_NL
             "wall, and stretches to the other end of the room, where various other" SOFT_NL
             "sundry objects can be glimpsed dimly in the distance.",
             "You're at NE end.", F_LIGHTED);
    make_ins(SW, R_SWEND);
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

    make_loc(q, R_SWORD,
             "You are on a narrow promontory at the foot of a waterfall, which" SOFT_NL
             "spurts from an overhead hole in the rock wall and splashes into a" SOFT_NL
             "large reservoir, sending up clouds of mist and spray." SOFT_NL
             "Through the thick white mist looms a polished marble slab, to" SOFT_NL
             "which is affixed an enormous rusty iron anvil.  In golden letters" SOFT_NL
             "are written the words: \"Whoso Pulleth Out This Sword of This" SOFT_NL
             "Stone and Anvil, is Rightwise King-Born of All This Mountain.\"" SOFT_NL
             "There is a narrow chimney on the east side of the promontory.",
             "You're at Sword Point.", 0);
    make_ins(U, R_CHIMTOP); ditto(E); ditto(CLIMB); ditto(CHIMNEY); ditto(SLIDE);
    make_loc(q, R_CHIMTOP,
             "You are on a narrow shelf above and east of the top of a very steep" SOFT_NL
             "chimney.  A long smooth granite slide curves down out of sight" SOFT_NL
             "to the east.  If you go down the slide, you may not be able to" SOFT_NL
             "climb back up.",
             "You're at top of slide.", 0);
    make_ins(E, R_MISTY); ditto(D); ditto(FORWARD); ditto(SLIDE);
    make_ins(W, R_SWORD); ditto(CHIMNEY);
    /* Long's travel table has explicit entries for "BACK" in several places,
     * mostly leading out of one-entrance rooms such as R_THRONE and R_HIGH.
     * He also uses it here (R_CHIMTOP -> R_SWORD).
     * However, since "BACK" is handled as a special case before consulting
     * the table, these entries do absolutely nothing; I've removed them. */
    make_loc(q, R_PRIVATE,
             "You are in the private chamber of the Mountain King.  Hewn into the" SOFT_NL
             "solid rock of the east wall of the chamber is an intricately-wrought" SOFT_NL
             "throne of elvish design.  There is an exit to the west.",
             "You're at entrance to Throne Room.", 0);
    make_ins(W, R_HMK); ditto(OUT);
    make_ins(E, R_THRONE);
    make_loc(q, R_THRONE,
             "You are on the east side of the throne room.  On the arm of the throne" SOFT_NL
             "has been hung a sign which reads \"Gone for the day: visiting" SOFT_NL
             "sick snake. --M.K.\"",
             "You're on east side of Throne Room.", 0);
    make_ins(W, R_PRIVATE);
    make_ins(OUT, R_HMK);
    make_loc(q, R_WHIRLPOOL,
             "You are dragged down, down, into the depths of the whirlpool." SOFT_NL
             "Just as you can no longer hold your breath, you are shot out over" SOFT_NL
             "a waterfall into the shallow end of a large reservoir.  Gasping" SOFT_NL
             "and sputtering, you crawl weakly towards the shore....",
             NULL, 0);
    make_ins(0, R_SWORD);
    make_loc(q, R_DENSE,
             "You are in dense forest, with a hill to one side.  The trees appear" SOFT_NL
             "to thin out towards the north and east.",
             "You're in forest.", F_LIGHTED | F_OUTSIDE);
    make_ins(E, R_HILL); ditto(U); ditto(ROAD); ditto(CLIMB);
    make_cond_ins(S, 50, R_FOREST2); ditto(FOREST);
    make_ins(S, R_DENSE); ditto(FOREST);
    make_ins(W, R_KNOLL);
    make_ins(N, R_MARSH_EDGE);
    make_loc(q, R_KNOLL,
             "You are at the high point of a wide grassy knoll, partially surrounded" SOFT_NL
             "by dense forest.  The land rises to the south and east, and drops off" SOFT_NL
             "sharply to the north and west.  The air smells of sea water.",
             "You're on grassy knoll.", F_LIGHTED | F_OUTSIDE);
    make_ins(E, R_DENSE); ditto(S);
    make_ins(N, R_MARSH_EDGE);
    make_ins(W, R_BEACH);
    make_ins(HOLE, R_THUNDER); ditto(THUNDER);
    make_ins(HOUSE, R_HOUSE);

    make_loc(q, R_MARSH_EDGE,
             "You are at the edge of a trackless salt marsh.  Tall reeds obscure" SOFT_NL
             "the view.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_cond_ins(S, 50, R_KNOLL);
    make_ins(S, R_DENSE);
    make_ins(E, R_MARSH149); ditto(W); ditto(N);
    make_ins(HOUSE, R_HOUSE);
    /* I don't know why this connection is here. Testing, I guess. It's not clued
     * at all that there should be a linkage between the marsh and the grotto.
     * In fact, the whole marsh-maze is pretty pointless; perhaps it was slated
     * for further expansion. */
    make_ins(GROTTO, R_WBLUE);

    make_loc(q, R_MARSH149, "You're in salt marsh.", NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(S, R_MARSH149);
    make_ins(E, R_MARSH151);
    make_ins(W, R_MARSH150);
    make_ins(N, R_MARSH152);
    make_loc(q, R_MARSH150, "You're in salty marsh.", NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(N, R_MARSH151); ditto(S);
    make_ins(E, R_MARSH149); ditto(W);
    make_loc(q, R_MARSH151, "You are in salt marsh.", NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(E, R_MARSH_EDGE);
    make_ins(W, R_MARSH152);
    make_ins(N, R_MARSH150);
    make_ins(S, R_MARSH149);
    make_loc(q, R_MARSH152, "Dead end.", NULL, F_LIGHTED | F_OUTSIDE);
    /* Oddly, this "dead end" has two exits. */
    make_ins(E, R_MARSH151);
    make_ins(S, R_MARSH150);

    make_loc(q, R_BEACH,
             "You're on a sandy beach at the edge of the open sea.  The beach" SOFT_NL
             "ends a short distance south and the land rises to a point. To" SOFT_NL
             "the north, the beach ends cliffs and broken rocks.",
             "You're on sandy beach.", F_LIGHTED | F_OUTSIDE);
    make_ins(N, R_BROKEN);
    make_ins(S, R_BAY);
    make_ins(E, R_KNOLL);
    make_ins(HOUSE, R_HOUSE);
    make_loc(q, R_BROKEN,
             "You are at a jumble of large broken rocks.  A gentle path leads up" SOFT_NL
             "to the top of the nearby cliffs.  A narrow treacherous path" SOFT_NL
             "disappears among the rocks at the foot of the cliff.",
             "You're at broken rocks.", F_LIGHTED | F_OUTSIDE);
    make_cond_ins(N, 50, R_CLIFF);
    make_ins(N, R_THUNDER); ditto(D);
    make_ins(U, R_CLIFF);
    make_ins(S, R_BEACH);
    make_ins(HOUSE, R_HOUSE);
    make_loc(q, R_CLIFF,
             "You are on a high cliff overlooking the sea.  Far below the" SOFT_NL
             "rolling breakers smash into a jumble of large broken rocks." SOFT_NL
             "The thunder of the surf is deafening.",
             /* Long doesn't give this room F_OUTSIDE, but it clearly should be. */
             "You're at Ocean Vista.", F_LIGHTED | F_OUTSIDE);
    make_ins(D, R_BROKEN); ditto(S);
    make_ins(JUMP, R_CLIFFJUMP);
    make_ins(HOUSE, R_HOUSE);
    make_loc(q, R_CLIFFJUMP,
             "You're at the bottom of the cliff, smashed to smithereens by the" SOFT_NL
             "pounding surf.",
             NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_THUNDER,
             "You are at Thunder Hole, a funnel shaped cavern opening onto the sea." SOFT_NL
             "The noise of the surf pounding against the outer rocks of the cave is" SOFT_NL
             "amplified by the peculiar shape of the cave, causing a thunder-like" SOFT_NL
             "booming sound to reverberate throughout the cave.  Outside, a narrow" SOFT_NL
             "path leads south towards some large rocks.",
             "You're at Thunder Hole.", F_LIGHTED | F_OUTSIDE);
    make_ins(IN, R_APPROACH); ditto(E);
    make_ins(OUT, R_BROKEN); ditto(S); ditto(U);
    make_ins(HOUSE, R_HOUSE);
    make_loc(q, R_TINYDOOR,
             "You are at the top of some arched steps.  On one side is a blank wall" SOFT_NL
             "with a tiny door at the base and a shelf overhead.  On the other side" SOFT_NL
             "a westward passage leads to the sea.",
             "You're at top of steps in back of Thunder Hole.", F_LIGHTED | F_OUTSIDE);
    make_ins(W, R_ESTYX); ditto(STEPS); ditto(D);
    make_ins(OUT, R_THUNDER);  /* fast way out */
    make_ins(PHUCE, R_SHRINKING);
    make_ins(CLIMB, remark(22)); ditto(U);
    make_ins(LEDGE, remark(23));
    make_ins(ENTER, remark(24)); ditto(IN); ditto(E); ditto(ENTRANCE);
    make_loc(q, R_CRAMPED,
             "You are in a low cramped chamber at the back of a small cave." SOFT_NL
             "There is a shelf in the rock wall at about the height of your" SOFT_NL
             "shoulder.",
             "You're in cramped chamber.", F_LIGHTED | F_OUTSIDE);
    make_ins(OUT, remark(27)); ditto(W); ditto(STEPS);
    make_loc(q, R_HUGEDOOR,
             "You are on a wide ledge, bounded on one side by a rock wall," SOFT_NL
             "and on the other by a sheer cliff.  The only way past is through" SOFT_NL
             "a large wrought-iron door.",
             "You're at ledge by wrought-iron door.", F_LIGHTED | F_OUTSIDE);
    make_cond_ins(E, unless_prop(HUGE_DOOR, 0), R_SEA); ditto(IN); ditto(ENTER);
    make_ins(E, remark(34));
    make_ins(PHUCE, R_GROWING);
    make_ins(W, remark(35)); ditto(CLIMB); ditto(D);
    make_ins(JUMP, R_NECK2);
    make_loc(q, R_SHRINKING,
             "You feel dizzy...Everything around you is spinning, expanding," SOFT_NL
             "growing larger....  Dear me!  Is the cave bigger or are you smaller?",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_GROWING,
             "You are again overcome by a sickening vertigo, but this time" SOFT_NL
             "everything around you is shrinking...Shrinking...",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_GROWING2,
             "You are again overcome by a sickening vertigo, but this time" SOFT_NL
             "everything is shrinking... I mean, you are growing.  This is" SOFT_NL
             "terribly confusing!",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_SHRINKING2,
             "You feel dizzy...Everything around you is spinning, expanding," SOFT_NL
             "growing larger....",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_NECK2,
             "You're at the bottom of the cliff with a broken neck.", NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_WBLUE,
             "You are at the western tip of the Blue Grotto.  A large lake almost" SOFT_NL
             "covers the cavern floor, except for where you are standing.  Small" SOFT_NL
             "holes high in the rock wall to the east admit a dim light.  The" SOFT_NL
             "reflection of the light from the water suffuses the cavern with" SOFT_NL
             "a hazy bluish glow.",
             "You're at west wall of Blue Grotto.", F_LIGHTED | F_PORTAL);
    make_ins(ENTER, remark(24)); ditto(OUT); ditto(IN); ditto(W);
    /* The game ensures that toting(BOAT) implies toting(POLE); you're not
     * allowed to board the boat without the pole, and you can't get rid of
     * the pole while you're in the boat. (Dropping and throwing are blocked,
     * and it's too big to put inside anything else.) */
    make_cond_ins(E, only_if_toting(BOAT), R_POLE_E); ditto(CROSS); ditto(OVER); ditto(ACROSS);
    make_cond_ins(NE, only_if_toting(BOAT), R_BUBBLE);
    make_cond_ins(S, only_if_toting(BOAT), R_POLE_S);
    make_cond_ins(N, only_if_toting(BOAT), R_POLE_N);
    make_ins(E, remark(20)); ditto(CROSS); ditto(OVER); ditto(ACROSS); ditto(NE);
    /* The remark isn't given for SOUTH or NORTH. This was probably an
     * oversight. */
    make_cond_ins(PHUCE, unless_prop(BOAT, 1), R_SHRINKING2);
    make_ins(PHUCE, remark(19));
    make_loc(q, R_SEA,
             "You are on the shore of an underground sea.  A high wooden" SOFT_NL
             "structure of vast proportions extends out into the water to the" SOFT_NL
             "east.  The way west is through a wrought-iron door.",
             "You're at underground sea.", F_LIGHTED | F_PORTAL);
    make_ins(PHUCE, R_GROWING2);
    make_cond_ins(W, unless_prop(HUGE_DOOR, 0), R_HUGEDOOR); ditto(OUT); ditto(ENTER); ditto(IN);
    make_ins(W, remark(38));
    make_ins(E, remark(37)); ditto(CROSS); ditto(OVER); ditto(ACROSS);
    make_loc(q, R_EBLUE,
             "You are on the eastern shore of the Blue Grotto.  An ascending" SOFT_NL
             "tunnel disappears into the darkness to the SE.",
             "You're on east side of the Blue Grotto.", F_LIGHTED);
    make_cond_ins(SE, unless_prop(BOAT, 1), R_WINDY); ditto(U); ditto(PASSAGE);
    make_ins(SE, remark(36)); ditto(U); ditto(PASSAGE);
    make_cond_ins(N, only_if_toting(BOAT), R_BUBBLE);
    make_cond_ins(S, only_if_toting(BOAT), R_GRAVEL);
    make_cond_ins(W, only_if_toting(BOAT), R_WBLUE); ditto(CROSS); ditto(OVER); ditto(ACROSS);
    make_ins(W, remark(20)); ditto(N); ditto(S);
    make_loc(q, R_BUBBLE,
             "You are at a high rock on the NE side of a watery chamber at the mouth" SOFT_NL
             "of a small brook.  An unknown gas bubbles up through the water from" SOFT_NL
             "the chamber floor.  A bluish light can be seen to the southwest.",
             "You're in Bubble Chamber.", F_LIGHTED);
    make_cond_ins(S, only_if_toting(BOAT), R_EBLUE);
    make_cond_ins(SW, only_if_toting(BOAT), R_WBLUE);
    make_ins(SW, remark(20)); ditto(S);
    make_cond_ins(PASSAGE, unless_prop(BOAT, 1), R_DEFILE); ditto(E); ditto(U);
    make_ins(PASSAGE, remark(36)); ditto(U);
    make_loc(q, R_WINDY,
             "You are in a windy tunnel between two large rooms.",
             NULL, 0);
    make_ins(E, R_BATCAVE); ditto(U);
    make_ins(W, R_EBLUE); ditto(D);
    make_loc(q, R_BATCAVE,
             "You are in the Bat Cave.  The walls and ceiling are covered with" SOFT_NL
             "sleeping bats.  The floor is buried by a mass of dry, foul-smelling" SOFT_NL
             "guano.  The stench is overpowering.  Exits to the NW and east.",
             "You're in Bat Cave.", 0);
    make_ins(NW, R_WINDY); ditto(D);
    make_ins(E, R_TONGUE); ditto(U);
    make_loc(q, R_TIGHT_NS,
             "You are in a very tight N/S crack.  The passage seems to widen to" SOFT_NL
             "the south.",
             NULL, 0);
    make_ins(N, R_ELONG);
    make_ins(S, R_CLOAKROOM);
    make_loc(q, R_TIGHT_NS_BLOCKED,
             "You are in a very tight N/S crack.  The passage south is blocked" SOFT_NL
             "by a recent cave-in.",
             NULL, 0);
    make_ins(N, R_ELONG);
    make_loc(q, R_CLOAKROOM,
             "You're in the Cloakroom.  This is where the dreaded Wumpus repairs" SOFT_NL
             "to sleep off heavy meals.  (Adventurers are his favorite dinner!)" SOFT_NL
             "Two very narrow passages exit NW and NE.",
             "You're in the Cloakroom.", 0);
    make_cond_ins(NE, unless_prop(CLOAKROOM_ROCKS, 0), remark(18));
    /* Notice that typing CRACK will get you out the way you came in, even
     * after the rockslide blocks the NE passage. */
    make_ins(NE, R_TIGHT_NS); ditto(CRACK);
    make_ins(NW, R_PITS);
    make_loc(q, R_PITS, /* Long has "climable" */
             "You're in a room containing several small climbable pits.  Passages" SOFT_NL
             "exit to the east and north.",
             NULL, 0);
    make_ins(N, R_HIGH);
    make_ins(E, R_CLOAKROOM);
    make_ins(PIT, R_FPIT); ditto(D); ditto(CLIMB);
    make_loc(q, R_FPIT,
             "You are at the bottom of a small featureless pit.",
             NULL, 0);
    make_ins(U, R_PITS); ditto(CLIMB); ditto(OUT);
    make_loc(q, R_HIGH,
             "You are at a high hole in a rock wall.",
             NULL, 0);
    make_ins(D, R_ELONG); ditto(CLIMB); ditto(JUMP);
    make_ins(S, R_PITS);
    make_loc(q, R_DEFILE,
             "You are in a sloping muddy defile, next to a tumbling brook.",
             NULL, 0);
    make_ins(D, R_BUBBLE); ditto(W);
    /* Following the stream in this location takes you *up*stream. */
    make_ins(U, R_FAIRY); ditto(E); ditto(STREAM);
    make_loc(q, R_TONGUE,
             "You are in a level E/W passage partially blocked by an overhanging" SOFT_NL
             "tongue of rock.  A steep scramble would take you up over the tongue," SOFT_NL
             "whence continues an upward crawl.",
             "You're at Tongue of Rock.", 0);
    make_ins(W, R_BATCAVE);
    make_ins(U, R_STAR); ditto(CLIMB);
    make_ins(E, R_ELEVEL);
    make_loc(q, R_UPPER,
             "You're in the Upper Passage, a long level E/W tunnel.",
             NULL, 0);
    make_ins(W, R_TONGUE);
    make_ins(E, R_STAR);
    make_loc(q, R_STAR,
             "You are in a star-shaped chamber.  Passages exit north, east, south," SOFT_NL
             "and west.",
             "You're in Star Chamber.", 0);
    make_ins(W, R_UPPER);
    make_cond_ins(E, 50, R_ELBOW);
    make_ins(E, R_NWNE);
    make_ins(S, R_DEAD185);
    make_ins(N, R_NARROW_EW);
    make_loc(q, R_ELBOW,
             "You are at an elbow in a winding E/W passage.",
             NULL, 0);
    make_ins(SW, R_STAR);
    make_ins(SE, R_ROTUNDA);
    make_loc(q, R_DEAD185, "Dead end.", NULL, 0);
    make_ins(N, R_STAR);
    make_loc(q, R_NWNE,
             "You're at the intersection of two long tunnels.  One goes NW," SOFT_NL
             "the other NE.",
             NULL, 0);
    make_ins(NW, R_STAR);
    make_ins(NE, R_ROTUNDA);
    make_loc(q, R_NARROW_EW,
             "You're in a long narrow east-west passage which curves out of sight" SOFT_NL
             "at both ends.",
             NULL, 0);
    make_ins(W, R_STAR);
    make_ins(E, R_ROTUNDA);
    make_loc(q, R_ROTUNDA,
             "You're in the Rotunda.  Corridors radiate in all directions." SOFT_NL
             "There is a telephone booth standing against the north wall.",
             "You're in Rotunda.", 0);
    make_ins(N, R_NARROW_EW);
    make_ins(W, R_ELBOW);
    make_cond_ins(SW, 65, R_DEVILS); ditto(D);
    make_ins(SW, R_NWNE);
    make_ins(IN, R_GNOME); ditto(ENTER);
    make_loc(q, R_BOOTH,
             "You are standing in a telephone booth at the side of a large chamber." SOFT_NL
             "Hung on the wall is a banged-up pay telephone of ancient design.",
             "You're in phone booth.", 0);
    make_ins(OUT, R_ROTUNDA);
    make_loc(q, R_DEVILS,
             "You're at the Devil's Chair, a large crystallization shaped like a" SOFT_NL
             "seat, at the edge of a black abyss.  You can't see the bottom." SOFT_NL
             "An upward path leads away from the abyss.",
             "You're at Devil's Chair.", 0);
    make_cond_ins(CROSS, unless_prop(CLAY, 1), R_CLAY); ditto(N); ditto(ACROSS); ditto(BRIDGE);
    make_ins(CROSS, remark(12));
    make_ins(U, R_ROTUNDA);
    make_loc(q, R_DEAD191, "You're in a dead-end crack.", NULL, 0);
    make_ins(N, R_ELONG);
    make_loc(q, R_GRAVEL,
             "You're on a small gravel beach at the south wall of the Blue Grotto." SOFT_NL
             "A gravelly path leads east.",
             "You're on gravel beach.", F_LIGHTED);
    make_cond_ins(N, only_if_toting(BOAT), R_WBLUE);
    make_cond_ins(NE, only_if_toting(BOAT), R_EBLUE);
    make_ins(N, remark(20)); ditto(NE);
    make_cond_ins(E, unless_prop(BOAT, 1), R_VESTIBULE);
    make_ins(E, remark(19));
    make_loc(q, R_FLOWER,
             "You are in the Flower Room.  The walls are covered with colorful," SOFT_NL
             "intricate, flower-like patterns of crystallized gypsum. A hole leads" SOFT_NL
             "to the west.",
             "You're in Flower Room.", 0);
    make_ins(W, R_VESTIBULE);
    make_loc(q, R_ESHORT_RUBBLE,
             "You are looking west from the end of a short E/W corridor.  At your" SOFT_NL
             "feet is a pile of loose rubble.  On your left is a hole into another" SOFT_NL
             "chamber.",
             "You are at east end of short E/W corridor.", 0);
    make_ins(HOLE, R_ARCHED_RUBBLE); ditto(WALL); ditto(S); ditto(CROSS); ditto(LEFT);
    make_ins(W, R_CRYPT);
    make_loc(q, R_ARCHED_RUBBLE,
             "You are in an arched hall.  The remnants of a now-plugged coral" SOFT_NL
             "passage lie to the east.  The north wall has partially crumbled," SOFT_NL
             "exposing a large connecting hole to another room.",
             "You're in Arched Hall.", 0);
    make_ins(D, R_SHELL); ditto(SHELL);
    make_cond_ins(HOLE, only_if_toting(CLAM), remark(4)); ditto(WALL); ditto(N); ditto(CROSS);
    make_ins(HOLE, R_ESHORT_RUBBLE);
    make_loc(q, R_VESTIBULE,
             "You're in the Vestibule, a short east-west passage between two rooms.",
             NULL, 0);
    make_ins(E, R_FLOWER);
    make_ins(W, R_GRAVEL);
    make_loc(q, R_FAIRY,
             "You are in the Fairy Grotto.  All around you innumerable stalactites," SOFT_NL
             "arranged in immense colonnades, form elegant arches.  On every side" SOFT_NL
             "you hear the dripping of water, like the footsteps of a thousand" SOFT_NL
             "fairies.  A small stream runs from the SW corner.  A bright glow" SOFT_NL
             "emanates from the south side of the grotto, and a steep passage" SOFT_NL
             "descends to the east.",
             "You're in the Fairy Grotto.", F_LIGHTED);
    make_ins(SW, R_DEFILE); ditto(STREAM); ditto(DOWNSTREAM); ditto(PASSAGE);
    make_ins(D, R_COLD); ditto(E);
    make_cond_ins(S, unless_prop(LAMP, 1), R_CRYSTAL); ditto(U);
    make_ins(S, remark(21));
    make_loc(q, R_CRYSTAL,
             "You are in the Crystal Palace.  An overhead vein of phosphorescent" SOFT_NL
             "quartz casts a liminous glow which is reflected by countless chips of" SOFT_NL
             "mica embedded in both walls, which consist of some sort of highly" SOFT_NL
             "reflective glass, apparently of volcanic origin.  A winding path" SOFT_NL
             "of yellow sandstone leads west and rises steeply to the east.",
             "You're in the Crystal Palace.", F_LIGHTED);
    make_ins(W, R_FAIRY);
    make_ins(E, R_YELLOW); ditto(U);
    make_loc(q, R_YELLOW,
             "You are following a yellow sandstone path.  There is a glow" SOFT_NL
             "to the west.",
             NULL, F_LIGHTED);
    /* The vagaries of the travel table force Long to fudge this puzzle a little bit.
     * It is not sufficient to DROP LAMP outside the Crystal Palace; you must actually
     * have turned it off, no matter whether it's present in the current room. */
    make_cond_ins(W, unless_prop(LAMP, 1), R_CRYSTAL); ditto(D);
    make_ins(W, remark(21));
    make_ins(E, R_RAINBOW);
    make_ins(N, R_SPIRES);
    make_loc(q, R_RAINBOW,
             /* Long has "comprised". */
             "You are in a very tall chamber whose walls are composed of many" SOFT_NL
             "different rock strata.  Layers of red and yellow sandstone" SOFT_NL
             "intertwine with bright bands of calcareous limestone in a rainbow-" SOFT_NL
             "like profusion of color.  The rainbow effect is so real, you" SOFT_NL
             "are almost tempted to look for a pot of gold!  Poised far over" SOFT_NL
             "your head, a gigantic slab, wedged tightly between the north and" SOFT_NL
             "south walls, forms a natural bridge across the roof of the chamber." SOFT_NL
             "A trail leads east and west.",
             "You're in the Rainbow Room.", 0);
    make_ins(W, R_YELLOW);
    make_cond_ins(CLICK, unless_prop(SHOES, 1), remark(29));
    make_ins(CLICK, R_OVER);
    make_ins(E, R_GREEN);
    make_loc(q, R_COLD,
             "You're in a steeply sloping passage.  It is very cold here.",
             NULL, 0);
    make_cond_ins(D, unless_prop(CLOAK, 1), remark(28)); ditto(E); ditto(ICE);
    make_ins(D, R_ICE);
    make_ins(U, R_FAIRY); ditto(W);
    make_loc(q, R_ICE,
             "You are in the Hall of Ice, in the deepest part of the caverns." SOFT_NL
             "During winter, frigid outside air settles here, making this room" SOFT_NL
             "extremely cold all year round.  The walls and ceilings are covered" SOFT_NL
             "with a thick coating of ice.  An upward passage exits to the west.",
             "You're in the Hall of Ice.", 0);
    make_ins(U, R_COLD); ditto(W); ditto(OUT);
    make_loc(q, R_OVER,
             "You are standing on a natural bridge far above the floor of a circular" SOFT_NL
             "chamber whose walls are a rainbow of multi-colored rock.  The bridge" SOFT_NL
             "was formed eons ago by a huge slab which fell from the ceiling and" SOFT_NL
             "is now jammed between the north and south walls of the chamber.",
             "You are over the Rainbow (Room).", 0);
    make_ins(N, R_GOTHIC);
    make_cond_ins(CLICK, unless_prop(SHOES, 1), remark(29));
    make_ins(CLICK, R_RAINBOW);
    make_ins(JUMP, R_ALLOVER);
    make_loc(q, R_GREEN,
             "You are in a low, wide room below another chamber.  A small green" SOFT_NL
             "pond fills the center of the room.  The lake is apparently spring" HARD_HYPHEN
             "fed.  A small stream exits through a narrow passage to the north." SOFT_NL
             "A larger passage continues west.",
             "You're in Green Lake Room.", 0);
    make_ins(N, R_RED); ditto(CRAWL);
    make_ins(W, R_RAINBOW);
    make_ins(U, remark(8)); ditto(HOLE);
    make_loc(q, R_RED,
             "You are in a tight north/south crawl through a stratum of red" SOFT_NL
             "colored rock.  The air is damp with mist.",
             "You're in red rock crawl.", 0);
    make_ins(N, R_SLOST);
    make_ins(S, R_GREEN);
    make_loc(q, R_SLOST,
             "You are in a tall canyon on the south side of a swift, wide river." SOFT_NL
             "Written in the mud in crude letters are the words: \"You Have Found" SOFT_NL
             "Lost River.\"  A wide path leads east and west along the bank.  A tight" SOFT_NL
             "crawlway would take you south out of the canyon.",
             "You're on south side of Lost River Canyon.", 0);
    make_ins(E, R_ELOST);
    make_ins(S, R_RED); ditto(CRAWL);
    make_ins(W, R_WLOST);
    make_ins(CROSS, remark(30)); ditto(ACROSS); ditto(N);
    make_loc(q, R_WLOST,
             "You are standing on a large flat rock table at the western end of" SOFT_NL
             "Lost River Canyon.  Beneath your feet, the river disappears amidst" SOFT_NL
             "foam and spray into a large sinkhole.  A gentle path leads east" SOFT_NL
             "along the river's south shore.  Another leads sharply upward along" SOFT_NL
             "the river's north side.",
             "You're at end of Lost River Canyon.", 0);
    make_ins(E, R_SLOST);
    make_ins(U, R_NICHE); ditto(CLIMB);
    make_loc(q, R_NICHE,
             "You are at a niche in the canyon wall, far above a raging river." SOFT_NL
             "The air is filled with mist and spray, making it difficult to see" SOFT_NL
             "ahead.  A downward sloping ledge narrows to the east. The path" SOFT_NL
             "to the west is easier.",
             "You're at niche in ledge above Lost River.", 0);
    make_ins(D, R_TREACHEROUS); ditto(E);
    make_ins(W, R_WLOST);
    make_loc(q, R_TREACHEROUS,
             "The ledge is growing very narrow and treacherous, and falls off almost" SOFT_NL
             "vertically.  You could go down, but you won't be able to climb" SOFT_NL
             "back.",
             NULL, 0);
    /* This is a one-way trip to the far side of the troll bridge! */
    make_ins(U, R_NICHE); ditto(W);
    make_ins(D, R_NESIDE); ditto(E);
    make_loc(q, R_REPO_BOOTH,
             "You are standing in a telephone booth at the side of the Repository.",
             "You're in phone booth.", F_LIGHTED);
    make_ins(OUT, R_NEEND); ditto(S);
    make_loc(q, R_ELEVEL,
             "You're at the east end of a level passage at a hole in the floor.",
             NULL, 0);
    make_ins(D, R_GREEN); ditto(HOLE);
    make_ins(W, R_TONGUE);
    make_loc(q, R_COVE,
             "You're at the north edge of a dark cove.",
             "You're in dark cove.", F_LIGHTED);
    make_cond_ins(S, only_if_toting(BOAT), R_WBLUE);
    make_cond_ins(SE, only_if_toting(BOAT), R_BUBBLE);
    make_ins(S, remark(20));
    make_cond_ins(NE, unless_prop(BOAT, 1), R_BASIN);
    make_ins(NE, remark(19));
    make_loc(q, R_BASIN,
             "You are in a dry granite basin, worn smooth eons ago by water" SOFT_NL
             "swirling down from a now-dry spillway.",
             "You're in dry basin.", 0);
    make_ins(SW, R_COVE);
    make_ins(U, R_SPILLWAY);
    make_loc(q, R_SPILLWAY,
             "You're in a dry spillway east of and above a smooth rock basin.",
             "You're in old spillway", 0);
    make_ins(D, R_BASIN);
    make_ins(E, R_WINERY); ditto(U);
    make_loc(q, R_WINERY,
             "You are in the Winery, a cool dark room which extends some" SOFT_NL
             "distance off to the east.",
             "You're in the Winery.", 0);
    make_ins(W, R_SPILLWAY); ditto(D); ditto(OUT);
    make_ins(E, R_PINNACLES); ditto(PIT);
    /* I'm not sure why the motion word "PIT" should get you from the Winery to
     * the limestone pinnacles, but it does. */
    make_loc(q, R_PINNACLES, /* Long has "climable" */
             "You are to the east of the Winery, where the room ends in a thicket" SOFT_NL
             "of high, sharp, pointed, climbable limestone pinnacles.  There is a" SOFT_NL
             "narrow ledge just above the top of the spires.  If you go up, it" SOFT_NL
             "might be difficult to get back down.",
             "You're at limestone pinnacles.", 0);
    make_ins(W, R_WINERY);
    make_ins(U, R_SPIRES); ditto(CLIMB);
    make_loc(q, R_GOTHIC,
             "You are in a high-vaulted cavern whose roof rises over fifty" SOFT_NL
             "meters to culminate in a series of pointed arches directly over" SOFT_NL
             "your head.  There are also two low arches to either side, forming" SOFT_NL
             "side portals.  The whole effect is that of a gothic cathedral." SOFT_NL
             "You can proceed north, south, east, or west.",
             "You're in Gothic Cathedral.", 0);
    make_ins(S, R_OVER); ditto(BRIDGE);
    make_ins(N, R_ALTAR); ditto(ALTAR);
    make_ins(E, R_EGOTHIC);
    make_ins(W, R_WGOTHIC);
    /* Praying in the Cathedral or at the Altar teleports you home. */
    make_ins(PRAY, R_HOUSE);
    make_loc(q, R_EGOTHIC,
             "You're at the east portal of the Gothic Cathedral. The path leads" SOFT_NL
             "east and west.",
             NULL, 0);
    make_ins(W, R_GOTHIC);
    make_ins(E, R_CHAPEL);
    make_loc(q, R_WGOTHIC,
             "You're at the west portal of the Gothic Cathedral.",
             NULL, 0);
    make_ins(E, R_GOTHIC);
    make_loc(q, R_ALTAR,
             "You are at the foot of the Altar, an immense, broad stalagmite." SOFT_NL
             "An opening leads south.",
             NULL, 0);
    make_ins(S, R_GOTHIC);
    make_ins(U, R_BROAD);
    make_ins(PRAY, R_HOUSE);
    make_loc(q, R_BROAD,
             "You're on top of an enormous, broad stalagmite.  There is a hole" SOFT_NL
             "in the ceiling overhead.",
             "You're on top of stalagmite.", 0);
    make_ins(D, R_ALTAR);
    make_ins(U, R_CRYPT);
    make_loc(q, R_CRYPT,
             "You are in a room the size and shape of a small crypt.  A narrow" SOFT_NL
             "cut exits east.  There is a hole in the floor.",
             "You're in the Crypt.", 0);
    make_ins(D, R_BROAD); ditto(HOLE);
    make_ins(E, R_ESHORT_RUBBLE);
    make_loc(q, R_CHAPEL,
             "You are in the Gothic Chapel, a small chamber adjoining the Gothic" SOFT_NL
             "Cathedral. A path leads west.",
             "You're in the Gothic Chapel.", 0);
    make_ins(W, R_EGOTHIC);
    make_loc(q, R_ALLOVER,
             "You are on the floor of the Rainbow Room.  In fact, you are spread" SOFT_NL
             "*ALL OVER* the floor of the Rainbow Room.",
             NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_APPROACH,
             "You are in a dimly lit passage behind Thunder Hole.  Etched into" SOFT_NL
             "the rock wall are the ominous words:\n"
             "*       \"You are approaching the River Styx.      *\n"
             "*        Laciate Ogni Speranza Voi Ch'Entrate.\"   *",
             "You are at approach to River Styx.", F_LIGHTED | F_OUTSIDE);
    make_ins(W, R_THUNDER); ditto(OUT); ditto(U);
    make_cond_ins(E, unless_prop(DOG, 1), R_WSTYX); ditto(IN); ditto(D);
    make_ins(E, remark(31));
    make_loc(q, R_WSTYX,
             "You are at the River Styx, a narrow little stream cutting directly" SOFT_NL
             "across the passageway.  The edge of the stream is littered with sticks" SOFT_NL
             "and other debris washed in by a recent rainfall.  On the far side" SOFT_NL
             "of the river, the passage continues east.",
             "You're at the River Styx.", F_LIGHTED | F_OUTSIDE);
    make_ins(N, remark(32)); ditto(S); ditto(UPSTREAM); ditto(DOWNSTREAM); ditto(CRACK);
    make_ins(U, R_APPROACH); ditto(W); ditto(OUT);
    make_ins(JUMP, R_ESTYX);
    make_ins(E, remark(33)); ditto(IN); ditto(ACROSS); ditto(CROSS); ditto(OVER);
    make_loc(q, R_ESTYX,
             "You're on the east side of the river's sticks.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(N, remark(32)); ditto(S); ditto(UPSTREAM); ditto(DOWNSTREAM); ditto(CRACK);
    make_ins(JUMP, R_WSTYX);
    make_ins(W, remark(33)); ditto(OUT); ditto(ACROSS); ditto(CROSS); ditto(OVER);
    make_ins(E, R_TINYDOOR); ditto(IN); ditto(STEPS);
    make_loc(q, R_SPIRES,
             "You are on a ledge at the northern end of a long N/S crawl.  The" SOFT_NL
             "ledge is above a large number of sharp vertical limestone spires." SOFT_NL
             "An attempt to climb down could be dangerous, if you get my *point*!",
             "You're on ledge above limestone pinnacles.", 0);
    make_cond_ins(D, 65, R_PINNACLES); ditto(JUMP);
    make_ins(D, R_SKEWERED);
    make_ins(S, R_YELLOW); ditto(CRAWL);
    make_loc(q, R_SKEWERED, "You are very neatly skewered on the point of a sharp rock.",
             NULL, 0);
    make_ins(0, R_LIMBO);
    make_loc(q, R_POLE_E, "You have poled your boat across the calm water.", NULL, 0);
    make_ins(0, R_EBLUE);
    make_loc(q, R_POLE_S, "You have poled your boat across the dark water.", NULL, 0);
    make_ins(0, R_GRAVEL);
    make_loc(q, R_POLE_N, "You have poled your boat across the Blue Grotto.", NULL, 0);
    make_ins(0, R_COVE);
    make_loc(q, R_DANTES,
             "You're at Dante's Rest, on the north side of a yawning dark chasm." SOFT_NL
             "A passage continues west along the chasm's edge.",
             "You're at Dante's Rest.", 0);
    make_cond_ins(CROSS, unless_prop(CLAY, 1), R_CLAY); ditto(S); ditto(ACROSS); ditto(BRIDGE);
    make_ins(CROSS, remark(12));
    make_ins(W, R_MISTY);
    make_loc(q, R_ELOST,
             "You are at the east end of a riverbank path in Lost River Canyon.",
             "You're at east end of Lost River Canyon.", 0);
    make_ins(W, R_SLOST);
    make_loc(q, R_PANTRY,
             "You're in the caretaker's pantry.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(OUT, R_HOUSE); ditto(ENTRANCE);
    make_loc(q, R_BAY,
             "You are on a small rise overlooking a beautiful bay. In the center" SOFT_NL
             "of the bay is the castle of the elves.",
             "You're on a small rise over the bay.", F_LIGHTED | F_OUTSIDE);
    make_ins(N, R_BEACH); ditto(NE);
    make_ins(SAINT_MICHEL, R_CASTLE);
    make_loc(q, R_CASTLE,
             "You are on the highest pinnacle of the castle in the bay." SOFT_NL
             "Steps lead down into the garden.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(SAINT_MICHEL, R_BAY); ditto(ACROSS); ditto(CROSS); ditto(NE);
    make_ins(D, R_OUTER);
    make_loc(q, R_OUTER,
             "You are in the outer courtyard of the garden of the elves." SOFT_NL
             "Steps lead up to the tower, and to the west, separating you" SOFT_NL
             "from the inner courtyard, is a maze of hedges, living things," SOFT_NL
             "but almost crystalline in their multicolored splendor.",
             "You are in the outer courtyard of the elves.", F_LIGHTED | F_OUTSIDE);
    make_ins(U, R_CASTLE);
    make_ins(W, R_KAL_RED);
    make_loc(q, R_KAL_RED,
             "From the inside the maze looks like a kaleidoscope, with" SOFT_NL
             "swatches of color dancing as you move. In this part the colors" SOFT_NL
             "are produced by shining red berries on the branches.",
             "You are in the living maze. There are red berries here.", F_LIGHTED | F_OUTSIDE);
    make_ins(E, R_OUTER);
    make_ins(SW, R_KAL_ORANGE);
    make_ins(W, R_KAL_YELLOW);
    make_ins(NW, R_KAL_BLUE);
    make_loc(q, R_KAL_ORANGE,
             "You are surrounded by a tall hedge with sharp iridescent leaves" SOFT_NL
             "and metallic orange flowers.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(NE, R_KAL_RED);
    make_ins(N, R_KAL_YELLOW);
    make_ins(NW, R_KAL_PURPLE);
    make_loc(q, R_KAL_YELLOW,
             "You are in the center of the living maze. The plants here are" SOFT_NL
             "dormant this season, but still carry brilliant yellow leaves.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(E, R_KAL_RED);
    make_ins(S, R_KAL_ORANGE);
    make_ins(W, R_KAL_GREEN);
    make_ins(N, R_KAL_BLUE);
    make_loc(q, R_KAL_GREEN,
             "Unlike the other areas of the hedge system, this area seems to " SOFT_NL
             "have no metallic gleam; nevertheless it is still breathtaking." SOFT_NL
             "The trees and bushes are all varigated shades of green, the" SOFT_NL
             "evergreens being a rich dark shade while the seasonal bushes" SOFT_NL
             "are a lighter yellowish green, making a startling contrast.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(S, R_KAL_ORANGE);
    make_ins(E, R_KAL_YELLOW);
    make_ins(N, R_KAL_BLUE);
    make_ins(W, R_KAL_PURPLE);
    make_loc(q, R_KAL_BLUE,
             "You are near the edge of the maze. You sample the blueberries" SOFT_NL
             "on the bushes. They are delicious.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(SE, R_KAL_RED);
    make_ins(S, R_KAL_GREEN);
    make_ins(SW, R_KAL_PURPLE);
    make_loc(q, R_KAL_PURPLE,
             "You are at the western end of the living maze. Beside the" SOFT_NL
             "shrubs forming the walls are tastefully planted beds of" SOFT_NL
             "violets and brilliant purple pansies." SOFT_NL
             "To the west is the inner garden.",
             NULL, F_LIGHTED | F_OUTSIDE);
    make_ins(SE, R_KAL_ORANGE);
    make_ins(E, R_KAL_GREEN);
    make_ins(NE, R_KAL_BLUE);
    make_ins(W, R_KALEIDOSCOPE);
    make_loc(q, R_INNER,
             "You are in the inner garden of the elves. In the center is" SOFT_NL
             "a living tree, with shimmering silvery bark, glistening metallic" SOFT_NL
             "green leaves, and flowers ripe with nectar. As the nectar falls" SOFT_NL
             "to the ground it forms droplets of silver. Around the tree is" SOFT_NL
             "a hedge of briars which cannot be crossed. Unfortunately for" SOFT_NL
             "adventurers such as you, most of the nectar falls inside the hedge." SOFT_NL
             "The exit is to the east.",
             "You're in the inner courtyard of the elves.", F_LIGHTED | F_OUTSIDE);
    make_ins(E, R_KAL_PURPLE);

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

    /* The remaining "locations" R_PPASS, R_PDROP, R_TROLL,
     * R_PHUCE, R_GNOME, R_CLAY, and R_KALEIDOSCOPE are special. */
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

#define is_outside(loc) (places[loc].flags & F_OUTSIDE)
#define is_portal(loc) (places[loc].flags & F_PORTAL)

bool is_well_inside(Location loc)
{
    return !is_outside(loc) && !is_portal(loc);
}

/*========== Data structures for objects. =================================
 * This section corresponds to sections 63--70 in Knuth.
 */

struct ObjectData objs_[MAX_OBJ+1 - MIN_OBJ];

Location last_knife_loc = R_LIMBO;
int tally = 15;  /* treasures awaiting you */
int lost_treasures;  /* treasures that you won't find */
bool troll_has_sapphire = false;
bool river_has_sapphire = false;

ObjectWord liquid_contents(ObjectWord t)
{
    assert(t == BOTTLE || t == CASK);
    switch (objs(t).prop) {
        case -2: /* post-closing */ assert(t == BOTTLE); break;
        case 0: return WATER;
        case 1: /* empty */ break;
        case 2: return OIL;
        case 3: /* broken */ assert(t == BOTTLE); break;
        case 4: return WINE;
    }
    return NOTHING;
}

bool water_at(Location loc)
{
    switch (loc) {
	case R_EPIT: case R_Y2: case R_WINERY: case R_ROAD: case R_HOUSE:
	case R_VALLEY: case R_SLIT: case R_CLEAN: case R_MISTY:
        case R_WBLUE: case R_SEA: case R_EBLUE:
	case R_BUBBLE: case R_GRAVEL: case R_FAIRY: case R_GREEN:
        case R_RED: case R_SLOST: case R_COVE: case R_WSTYX:
	case R_ESTYX: case R_RES:
        /* Long includes R_RED here, despite there being no water in that
         * room's description; and doesn't include R_WLOST, R_ELOST, or
         * R_BEACH. */
	    return true;
	default:
	    return false;
    }
}

ObjectWord liquid_at_location(Location loc)
{
    if (water_at(loc)) return WATER;
    if (loc == R_EPIT) return OIL;
    if (loc == R_WINERY) return WINE;
    return NOTHING;
}

bool blind_at(Location loc)
{
    /* True if the adventurer is blind at this location, due to
     * darkness or (in the Crystal Palace) glare. */
    bool has_lighted_lamp = here(LAMP, loc) && objs(LAMP).prop;
    if (loc == R_CRYSTAL) {
	return has_lighted_lamp;
    } else {
	return !(has_lighted_lamp || (places[loc].flags & F_LIGHTED));
    }
}

void mobilize(ObjectWord t) { objs(t).base = NULL; }
void immobilize(ObjectWord t) { objs(t).base = &objs(t); }

void new_obj(ObjectWord t, const char *n, ObjectWord b, Location l)
{
    objs(t).name = n;
    objs(t).prop = (is_treasure(t) ? -1 : 0);
    objs(t).base = (b != 0 ? &objs(b) : NULL);
    objs(t).place = l;
    objs(t).flags = 0;
    objs(t).contents = NULL;
    objs(t).link = NULL;
    assert(l >= R_LIMBO);
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
    new_obj(LAMP, "Brass lantern", 0, R_HOUSE);
    objs(LAMP).desc[0] = "There is a shiny brass lamp nearby.";
    objs(LAMP).desc[1] = "There is a lamp shining nearby.";
    new_obj(GRATE, "Grate", GRATE, R_OUTSIDE);
    new_obj(GRATE_, NULL, GRATE, R_INSIDE);
    objs(GRATE).desc[0] = "The grate is locked.";
    objs(GRATE).desc[1] = "The grate is open.";
    new_obj(CAGE, "Wicker cage", 0, R_COBBLES);
    objs(CAGE).desc[0] = "There is a small wicker cage discarded nearby.";
    new_obj(ROD, "Black rod", 0, R_DEBRIS);
    objs(ROD).desc[0] = "A three-foot black rod with a rusty star on an end lies nearby.";
    new_obj(ROD2, "Black rod", 0, R_LIMBO);
    objs(ROD2).desc[0] = "A three-foot black rod with a rusty mark on an end lies nearby.";
    new_obj(TREADS, "Steps", TREADS, R_SPIT);
    new_obj(TREADS_, NULL, TREADS, R_EMIST);
    objs(TREADS).desc[0] = "Rough stone steps lead down the pit.";
    objs(TREADS).desc[1] = "Rough stone steps lead up the dome.";
    new_obj(POLE, "Wooden pole", 0, R_MARSH_EDGE);
    objs(POLE).desc[0] = "A wooden pole is lying nearby.";
    objs(POLE).desc[1] = "A wooden pole has been stuck in the mud here.";
    new_obj(PILLOW, "Velvet pillow", 0, R_SOFT);
    objs(PILLOW).desc[0] = "A small velvet pillow lies on the floor.";
    new_obj(SNAKE, "Snake", SNAKE, R_HMK);
    objs(SNAKE).desc[0] = "A huge green fierce snake bars the way!";
    objs(SNAKE).desc[1] = NULL;  /* chased away */
    new_obj(FISSURE, "Fissure", FISSURE, R_EFISS);
    new_obj(FISSURE_, NULL, FISSURE, R_WFISS);
    objs(FISSURE).desc[0] = NULL;
    objs(FISSURE).desc[1] ="A crystal bridge now spans the fissure.";
    new_obj(TABLET, "Stone tablet", TABLET, R_DARK);
    objs(TABLET).desc[0] =  /* Woods has "imbedded". */
        "A massive stone tablet embedded in the wall reads:" SOFT_NL
        "\"Congratulations on bringing light into the Dark-Room!\"";
    new_obj(CLAM, "Giant clam  >grunt!<", 0, R_SHELL);
    objs(CLAM).desc[0] = "There is an enormous clam here with its shell tightly closed.";
    new_obj(OYSTER, "Giant oyster  >groan!<", 0, R_LIMBO);
    objs(OYSTER).desc[0] = "There is an enormous oyster here with its shell tightly closed.";
    new_obj(MAG, "\"Spelunker Today\"", 0, R_ANTE);
    objs(MAG).desc[0] = "There are a few recent issues of \"Spelunker Today\" magazine here.";
    new_obj(FOOD, "Tasty food", 0, R_PANTRY);
    new_obj(DWARF, "Dwarf", DWARF, R_LIMBO);
    new_obj(KNIFE, "Dwarf's knife", 0, R_LIMBO);
    objs(FOOD).desc[0] = "There is food here.";
    new_obj(BOTTLE, "Small bottle", 0, R_PANTRY);
    objs(BOTTLE).desc[0] = "There is a bottle of water here.";
    objs(BOTTLE).desc[1] = "There is an empty bottle here.";
    objs(BOTTLE).desc[2] = "There is a bottle of oil here.";
    objs(BOTTLE).desc[3] = "There is a broken bottle here.";
    objs(BOTTLE).desc[4] = "There is a bottle of wine here.";
    new_obj(CHASM, "Troll bridge", CHASM, R_SWSIDE);
    new_obj(CHASM_, NULL, CHASM, R_NESIDE);
    objs(CHASM).desc[0] =
        "A rickety wooden bridge extends across the chasm, vanishing into the" SOFT_NL
        "mist. A sign posted on the bridge reads, \"STOP! PAY TROLL!\"";
    objs(CHASM).desc[1] =
        "The wreckage of a bridge (and a dead bear) can be seen at the bottom" SOFT_NL
        "of the chasm.";
    new_obj(CLAY, "Decrepit bridge", CLAY, R_DANTES);
    new_obj(CLAY_, NULL, CLAY, R_DEVILS);
    objs(CLAY).desc[0] =
        "A decrepit natural bridge spans the chasm.  A message scrawled into" SOFT_NL
        "the rock wall reads: \"Bridge out of repair.  Maximum load: 35 Foonts.\"";
    objs(CLAY).desc[1] =
        "The remnants of a natural bridge partially overhang the chasm.";
    new_obj(MIRROR, "Mirror", MIRROR, R_MIRROR);
    new_obj(MIRROR_, NULL, MIRROR, R_LIMBO);  /* joins up with MIRROR later */
    objs(MIRROR).desc[0] = NULL;  /* it's just scenery */
    new_obj(PLANT, "Plant", PLANT, R_WPIT);
    objs(PLANT).desc[0] = "There is a tiny little plant in the pit, murmuring \"Water, water, ...\"";
    objs(PLANT).desc[1] =
        "There is a 12-foot-tall beanstalk stretching up out of the pit," SOFT_NL
        "bellowing \"Water!! Water!!\"";
    objs(PLANT).desc[2] = "There is a gigantic beanstalk stretching all the way up to the hole.";
    new_obj(PLANT2, "Beanstalk", PLANT2, R_W2PIT);
    new_obj(PLANT2_, NULL, PLANT2, R_E2PIT);
    objs(PLANT2).desc[0] = NULL;
    objs(PLANT2).desc[1] = "The top of a 12-foot-tall beanstalk is poking out of the west pit.";
    objs(PLANT2).desc[2] = "There is a huge beanstalk growing out of the west pit up to the hole.";
    new_obj(STALACTITE, "Stalactite", STALACTITE, R_TITE);
    objs(STALACTITE).desc[0] = NULL;
    new_obj(SHADOW, "Shadowy figure", SHADOW, R_WINDOE);
    new_obj(SHADOW_, NULL, SHADOW, R_WINDOW);
    objs(SHADOW).desc[0] = "The shadowy figure seems to be trying to attract your attention.";
    new_obj(AXE, "Dwarf's axe", 0, R_LIMBO);
    objs(AXE).desc[0] = "There is a little axe here.";
    objs(AXE).desc[1] = "There is a little axe lying beside the bear.";
    new_obj(DRAWINGS, "Cave drawings", DRAWINGS, R_ORIENTAL);
    objs(DRAWINGS).desc[0] = NULL;
    new_obj(PIRATE, "Pirate", PIRATE, R_LIMBO);
    new_obj(DRAGON, "Dragon", DRAGON, R_SCAN1);
    new_obj(DRAGON_, NULL, DRAGON, R_SCAN3);
    objs(DRAGON).desc[0] = "A huge green fierce dragon bars the way!";
    objs(DRAGON).desc[1] = "The body of a huge green dead dragon is lying off to one side.";
    new_obj(TROLL, "Troll", TROLL, R_SWSIDE);
    new_obj(TROLL_, NULL, TROLL, R_NESIDE);
    objs(TROLL).desc[0] =
        "A burly troll stands by the bridge and insists you throw him a" SOFT_NL
        "treasure before you may cross.";
    objs(TROLL).desc[1] = "The troll steps out from beneath the bridge and blocks your way.";
    objs(TROLL).desc[2] = NULL;  /* chased away */
    new_obj(NO_TROLL, "Troll", NO_TROLL, R_LIMBO);
    new_obj(NO_TROLL_, NULL, NO_TROLL, R_LIMBO);
    objs(NO_TROLL).desc[0] = "The troll is nowhere to be seen.";
    new_obj(BEAR, "Bear", BEAR, R_BARR);
    objs(BEAR).desc[0] = "There is a ferocious cave bear eying you from the far end of the room!";
    objs(BEAR).desc[1] = "There is a gentle cave bear sitting placidly in one corner.";
    objs(BEAR).desc[2] = "There is a contented-looking bear wandering about nearby.";
    objs(BEAR).desc[3] = NULL;  /* the dead bear remains as scenery where it fell */
    new_obj(MESSAGE, "Message", MESSAGE, R_LIMBO);
    objs(MESSAGE).desc[0] =
        "There is a message scrawled in the dust in a flowery script, reading:" SOFT_NL
        "\"This is not the maze where the pirate hides his treasure chest.\"";
    new_obj(GORGE, "Volcano", GORGE, R_VIEW);
    objs(GORGE).desc[0] = NULL;  /* it's just scenery */
    new_obj(MACHINE, "Vending machine", MACHINE, R_PONY);
    objs(MACHINE).desc[0] =
        "There is a massive vending machine here. The instructions on it read:" SOFT_NL
        "\"Drop coins here to receive fresh batteries.\"";
    new_obj(BATTERIES, "Batteries", 0, R_LIMBO);
    objs(BATTERIES).desc[0] = "There are fresh batteries here.";
    objs(BATTERIES).desc[1] = "Some worn-out batteries have been discarded nearby.";
    new_obj(MOSS, "Moss", MOSS, R_SOFT);
    objs(MOSS).desc[0] = NULL;  /* it's just scenery */
    new_obj(RUSTY_DOOR, "Rusty door", RUSTY_DOOR, R_IMMENSE);
    objs(RUSTY_DOOR).desc[0] = "The way north is barred by a massive, rusty, iron door.";
    objs(RUSTY_DOOR).desc[1] = "The way north leads through a massive, rusty, iron door.";
    new_obj(TINY_DOOR, "Tiny door", TINY_DOOR, R_TINYDOOR);
    new_obj(TINY_DOOR_, NULL, TINY_DOOR, R_WBLUE);
    objs(TINY_DOOR).desc[0] = "The only way past the wall is through a tiny locked door.";
    objs(TINY_DOOR).desc[1] = "The only way past the wall is through a tiny open door.";
    new_obj(HUGE_DOOR, "Huge door", HUGE_DOOR, R_HUGEDOOR);
    new_obj(HUGE_DOOR_, NULL, HUGE_DOOR, R_SEA);
    objs(HUGE_DOOR).desc[0] = "The door is locked.";
    objs(HUGE_DOOR).desc[1] = "The door is open.";
    new_obj(BOOTH_DOOR, "Phone booth door", BOOTH_DOOR, R_ROTUNDA);
    new_obj(BOOTH_DOOR_, NULL, BOOTH_DOOR, R_BOOTH);
    objs(BOOTH_DOOR).desc[0] = NULL;  /* it's just scenery */
    new_obj(FLOWERS, "Beautiful flowers", 0, R_CLIFF);
    /* Long has an exclamation mark "here!", but that's misleading.
     * The flowers aren't a treasure. So I'm removing the "!". */
    objs(FLOWERS).desc[0] =
        "There are some beautiful flowers here.";
    objs(FLOWERS).desc[1] =
        "On the other side of the room a swarm of bees eagerly buzzes over" SOFT_NL
        "a bunch of fresh flowers.";
    new_obj(CLOAK, "Silken cloak", 0, R_CLOAKROOM);
    objs(CLOAK).desc[0] = "There is a silken cloak here!";
    objs(CLOAK).desc[1] = NULL;  /* it's being worn */
    objs(CLOAK).desc[2] =
        "A lovely silken cloak lies partially buried under a pile of" SOFT_NL
        "loose rocks.";
    new_obj(BOAT, "Wooden boat", 0, R_WBLUE);
    objs(BOAT).desc[0] = "There is a small wooden boat here.";
    new_obj(STICKS, "Sticks at Styx", STICKS, R_WSTYX);
    new_obj(STICKS_, NULL, STICKS, R_ESTYX);
    new_obj(GOLD, "Large gold nugget", 0, R_NUGGET);
    objs(GOLD).desc[0] = "There is a large sparkling nugget of gold here!";
    new_obj(DIAMONDS, "Several diamonds", 0, R_ICE);
    objs(DIAMONDS).desc[0] = "There are diamonds here!";
    new_obj(HORN, "Silver horn", 0, R_WFISS);
    objs(HORN).desc[0] = "There is a silver horn here!";
    new_obj(JEWELS, "Precious jewelry", 0, R_SOUTH);
    objs(JEWELS).desc[0] = "There is precious jewelry here!";
    objs(JEWELS).desc[1] = NULL;  /* being worn */
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
    new_obj(RUG, "Persian rug", RUG, R_SCAN1);
    new_obj(RUG_, NULL, RUG, R_SCAN3);
    objs(RUG).desc[0] = "There is a Persian rug spread out on the floor!";
    objs(RUG).desc[1] = "The dragon is sprawled out on a Persian rug!!";
    new_obj(SPICES, "Rare spices", 0, R_CHAMBER);
    objs(SPICES).desc[0] = "There are rare spices here!";
    new_obj(CHAIN, "Golden chain", CHAIN, R_BARR);
    objs(CHAIN).desc[0] = "There is a golden chain lying in a heap on the floor!";
    objs(CHAIN).desc[1] = "The bear is locked to the wall with a golden chain!";
    objs(CHAIN).desc[2] = "There is a golden chain locked to the wall!";
    new_obj(SWORD, "Gleaming sword", 0, R_SWORD);
    objs(SWORD).desc[0] = "There is a gleaming sword here!";
    objs(SWORD).desc[1] = "A gleaming sword is stuck into the anvil!";
    objs(SWORD).desc[2] = "Rusty shards of a elven sword lie scattered about.";
    objs(SWORD).desc[3] = "A very clean sword is stuck into the anvil!";
    objs(SWORD).desc[4] = "An oily sword is stuck into the anvil.";
    new_obj(CROWN, "Elfin crown", 0, R_THRONE);
    objs(CROWN).desc[0] = "An ancient crown of elfin kings lies here!";
    new_obj(SHOES, "Ruby slippers", 0, R_OVER);
    /* Long has "here." instead of "here!", but I've normalized all the
     * treasures' descriptions to include exclamation marks. */
    objs(SHOES).desc[0] = "There is a pair of ruby slippers here!";
    objs(SHOES).desc[1] = NULL;  /* being worn */
    /* TODO: Long has desc[2] = desc[0]. Why? What makes prop=2? */
    new_obj(LYRE, "Delicate lyre", 0, R_NS);
    objs(LYRE).desc[0] = "There is a delicate lyre here!";
    new_obj(SAPPHIRE, "Star sapphire", 0, R_STAR);
    objs(SAPPHIRE).desc[0] = "There is a star sapphire here!";
    new_obj(GRAIL, "Holy Grail", 0, R_CHAPEL);
    objs(GRAIL).desc[0] = "There is an ornate silver chalice here!";
    new_obj(CASK, "Oaken cask", 0, R_SPIRES);
    objs(CASK).desc[0] = "There is a cask of water here.";
    objs(CASK).desc[1] = "There is an empty oaken cask here.";
    objs(CASK).desc[2] = "There is a cask of oil here.";
    objs(CASK).desc[3] = NULL;  /* unused; "broken cask" */
    objs(CASK).desc[4] = "There is a cask of wine here.";
    new_obj(RING, "Golden ring", 0, R_LIMBO);
    objs(RING).desc[0] = "There is a small gold ring here.";
    objs(RING).desc[1] = NULL;  /* being worn */
    objs(RING).desc[2] = "On the Wumpus' finger is a small gold ring.";
    new_obj(CLOVER, "Four-leafed clover", 0, R_KNOLL);
    objs(CLOVER).desc[0] = "There is a four-leafed clover here!";
    objs(CLOVER).desc[1] = NULL;  /* being worn */
    new_obj(TREE, "Gold tree", 0, R_OUTER);
    /* Long has "."; I've normalized it. */
    objs(TREE).desc[0] = "There is a gold statue of a tree here!";
    new_obj(DROPLET, "Silver droplet", 0, R_INNER);
    /* Long has "."; I've normalized it. */
    objs(DROPLET).desc[0] = "There is a single droplet of silver on the ground here!";
    /* In Long's game, each liquid has two forms; this is so that
     * you can carry both water-in-bottle and water-in-cask at the
     * same time. In the case of WATER, the two objects actually
     * have distinct descriptions as well: "clear water" in the
     * bottle, "cool water" in the cask.
     * Liquids never appear on the ground; they are invariably
     * enclosed in either the BOTTLE or the CASK. */
    new_obj(WATER, "Clear water", 0, R_LIMBO);
    new_obj(WATER_IN_CASK, "Cool water", 0, R_LIMBO);
    new_obj(OIL, "Thick, black oil", 0, R_LIMBO);
    new_obj(OIL_IN_CASK, "Thick, black oil", 0, R_LIMBO);
    new_obj(WINE, "Vintage wine", 0, R_LIMBO);
    new_obj(WINE_IN_CASK, "Vintage wine", 0, R_LIMBO);
    /* TODO: What's the deal with objs(BEES).prop? */
    new_obj(BEES, "Bumblebees", BEES, R_FLOWER);
    objs(BEES).desc[0] = NULL;
    objs(BEES).desc[1] = "Some bumblebees are swarming around a bunch of fresh flowers.";
    new_obj(ECHO, "Hollow wall", ECHO, R_ARCHED);
    objs(ECHO).desc[0] = "Your footsteps echo hollowly throughout the chamber.";
    objs(ECHO).desc[1] = NULL;  /* the wall is just rubble now */
    /* TODO: I'm fairly sure that SAFE_WALL could productively be combined
     * with SAFE. */
    new_obj(SAFE_WALL, "Wall with safe", SAFE_WALL, R_LIMBO);
    objs(SAFE_WALL).desc[0] = "A steel safe is embedded in the wall.";
    new_obj(TINY_KEY, "Tiny brass key", 0, R_CRAMPED);
    objs(TINY_KEY).desc[0] = "There is a tiny brass key here.";
    objs(TINY_KEY).desc[1] = "There is a tiny brass key on the shelf.";
    new_obj(ANVIL, "Anvil", ANVIL, R_SWORD);
    objs(ANVIL).desc[0] = NULL;
    new_obj(CLOAKROOM_ROCKS, "Rocks", CLOAKROOM_ROCKS, R_CLOAKROOM);
    objs(CLOAKROOM_ROCKS).desc[0] = NULL;  /* on cloak */
    objs(CLOAKROOM_ROCKS).desc[1] = NULL;  /* after rockslide */
    new_obj(BOOTH, "Telephone booth", BOOTH, R_ROTUNDA);
    new_obj(BOOTH_, NULL, BOOTH, R_LIMBO);  /* joins up with BOOTH later */
    objs(BOOTH).desc[0] =
        "The telephone booth is empty.  The phone is ringing.";
    objs(BOOTH).desc[1] =
        "The phone booth is occupied by a gnome.  He is talking excitedly" SOFT_NL
        "to someone at the other end.";
    objs(BOOTH).desc[2] =
        "The telephone booth is empty.";
    objs(BOOTH).desc[3] = NULL;  /* it's just scenery in the repository */
    new_obj(PHONE, "Telephone", PHONE, R_BOOTH);
    objs(PHONE).desc[0] = "The phone is ringing.";
    objs(PHONE).desc[1] = "The telephone is out of order.";
    objs(PHONE).desc[2] = "The telephone is out of order.  It is badly dented.";
    objs(PHONE).desc[3] = NULL;  /* it's just scenery in the repository */
    new_obj(SLUGS, "Lead slugs", 0, R_LIMBO);
    /* Long has "!"; I've normalized it. */
    objs(SLUGS).desc[0] = "There are some lead slugs here.";
    new_obj(HONEY, "Sweet honeycomb", 0, R_LIMBO);
    /* Long has "!"; I've normalized it. */
    objs(HONEY).desc[0] = "There is a sweet honeycomb here.";
    new_obj(HIVE, "Beehive", HIVE, R_FLOWER);
    objs(HIVE).desc[0] =
        "There is an active beehive nearby.  The bees hum protectively" SOFT_NL
        "around the hive.";
    objs(HIVE).desc[1] =
        "There is an empty beehive nearby.";
    new_obj(DOG, "Black dog", DOG, R_APPROACH);
    objs(DOG).desc[0] =
        "A hideous black dog bares his teeth and growls at your approach.";
    objs(DOG).desc[1] =
        "Nearby, a large black dog is in a deep slumber.";
    /* TODO: The Wumpus is like Platt's "Rover" or the gooseberry goblins;
     * it really only has two states, but it also has a "timer" that triggers
     * its other messages. Can we deal better with this? */
    new_obj(WUMPUS, "Dreaded Wumpus", WUMPUS, R_CLOAKROOM);
    objs(WUMPUS).desc[0] =
        "In the corner, a Wumpus is sleeping peacefully.";
    objs(WUMPUS).desc[1] =
        "A sleepy Wumpus is ambling towards you.  He wants to invite you to" SOFT_NL
        "dinner.  He wants you to *be* the dinner!";
    objs(WUMPUS).desc[2] =
        "The Wumpus is still on your trail!  And he's getting closer!!";
    objs(WUMPUS).desc[3] =
        "The Wumpus is only a few steps behind you!  All this exercise is" SOFT_NL
        "making him veerrrrry hungry!";
    objs(WUMPUS).desc[4] =
        "The Wumpus almost has you in his grasp!  You can feel his hot breath" SOFT_NL
        "on your neck!";
    objs(WUMPUS).desc[5] =
        "\"Chomp, chomp.\"  Crunch!  Chew!  Slurp!  Smack!  Yum!!!";
    objs(WUMPUS).desc[6] =
        "Nearby is the smashed body of a defunct Wumpus.";
    new_obj(BIRD, "Little bird in cage", 0, R_BIRD);
    objs(BIRD).desc[0] = "A cheerful little bird is sitting here singing.";
    objs(BIRD).desc[1] = "There is a little bird in the cage.";
    new_obj(KEYS, "Set of keys", 0, R_PANTRY);
    objs(KEYS).desc[0] = "There are some keys on the ground here.";
    new_obj(FOUNTAIN, "Fountain", FOUNTAIN, R_WINERY);
    objs(FOUNTAIN).desc[0] = "There is a fountain of sparkling vintage wine here!";
    new_obj(BATS, "Bats", BATS, R_BATCAVE);
    objs(BATS).desc[0] = NULL;  /* they're just scenery */
    new_obj(GNOME, "Gnome in phone booth", GNOME, R_LIMBO);
    objs(GNOME).desc[0] = NULL;  /* it's just scenery */
    new_obj(MUSHROOMS, "Colored mushrooms", 0, R_DENSE);
    objs(MUSHROOMS).desc[0] = "There are some oddly-colored mushrooms here.";
    /* We don't need a non-"shelf" description for the cakes, because it's
     * impossible to take them out of R_CRAMPED. */
    new_obj(CAKES, "Tiny cakes", 0, R_CRAMPED);
    objs(CAKES).desc[0] = "There are some tiny cakes on the shelf.";
    new_obj(SACK, "Leather sack", 0, R_HOUSE);
    objs(SACK).desc[0] = "There is a leather sack here.";
    new_obj(BOOK, "Rare book", 0, R_LIMBO);
    objs(BOOK).desc[0] = "There is a dusty, leather-bound volume here.";
    new_obj(REPO_BOOK, "Rare book", 0, R_LIMBO);
    objs(REPO_BOOK).desc[0] = objs(BOOK).desc[0];
    new_obj(SAFE, "Steel wall-safe", SAFE, R_LIMBO);
    objs(SAFE).desc[0] = "The safe door is locked.";
    objs(SAFE).desc[1] = "The safe door is open.";
    new_obj(POSTER, "Faded poster", 0, R_HOUSE);
    objs(POSTER).desc[0] = "Taped to the wall is a faded poster.";
    objs(POSTER).desc[1] = "There is a faded poster here.";
    new_obj(BROOM, "Whiskbroom", 0, R_TONGUE);
    objs(BROOM).desc[0] = "There is a small whiskbroom here.";
    new_obj(CARVING, "Carving on dusty rocks", CARVING, R_DUSTY);
    objs(CARVING).desc[0] = NULL;  /* it's just scenery */
    new_obj(BILLBOARD, "Billboard", BILLBOARD, R_FOREST);
    objs(BILLBOARD).desc[0] = NULL;  /* it's just scenery */
    new_obj(CANISTER, "Small metal canister", 0, R_WINDOW);
    objs(CANISTER).desc[0] = "There is a heavy, grey, metal canister here.";
    new_obj(RADIUM, "Glowing stone", 0, R_BUBBLE);
    /* Long has "."; I've normalized it. */
    objs(RADIUM).desc[0] = "Nearby, a strange, greenish stone is glowing brightly!";
    new_obj(BALL, "Quartz sphere", 0, R_CRYSTAL);
    objs(BALL).desc[0] = "There is a polished sphere of pure quartz here!";
    new_obj(ALL, NULL, 0, R_LIMBO);  /* this is a fake object */

    insert(WATER, BOTTLE);
    insert(BOOK, SAFE);
    objs(POLE).prop = 1;  /* stuck in the mud */
    objs(TINY_KEY).prop = 1;  /* on the shelf */
    objs(SACK).flags |= F_OPEN;
    objs(GRATE).flags |= F_LOCKED;
    objs(TINY_DOOR).flags |= F_LOCKED;
    objs(HUGE_DOOR).flags |= F_LOCKED;
    objs(CHEST).flags |= F_LOCKED;
    objs(CHAIN).flags |= F_LOCKED;
    objs(SAFE).flags |= F_LOCKED;
    objs(RUSTY_DOOR).flags |= F_LOCKED;
}


/*========== Input routines. ==============================================
 */

bool yes(const char *q, const char *y, const char *n)
{
    static char buffer[72];
    while (true) {
        printf("%s\n\n> ", q); fflush(stdout);
        fgets(buffer, sizeof(buffer), stdin);
        if (tolower(*buffer) == 'y') {
            if (y) puts(y);
            return true;
        } else if (tolower(*buffer) == 'n') {
            if (n) puts(n);
            return false;
        } else {
            puts("Please answer the question.");
        }
    }
}

/*========== Dwarves and pirate. ==========================================
 * This section corresponds to sections 159--175 in Knuth.
 */

int dflag;  /* how angry are the dwarves? */
Location dloc[6] = { R_PIRATES_NEST, R_HMK, R_ROTUNDA, R_Y2, R_MAZEA44, R_SHELL };
Location odloc[6];
bool dseen[6];
int being_chased;  /* by a Wumpus */

bool dwarf_at(int loc)  /* is a dwarf present? Section 160 in Knuth. */
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

bool pirate_covets(ObjectWord t, Location loc)
{
    if (t == PYRAMID && (loc == R_PLOVER || loc == R_DARK)) return false;
    if (t == CASK && liquid_contents(CASK) != WINE) return false;
    return is_treasure(t);
}

void steal_all_your_treasure(Location loc)  /* sections 173--174 in Knuth */
{
    puts("Out from the shadows behind you pounces a bearded pirate!  \"Har, har,\"" SOFT_NL
         "he chortles. \"I'll just take all this booty and hide it away with me" SOFT_NL
         "chest deep in the maze!\"  He snatches your treasure and vanishes into" SOFT_NL
         "the gloom.");
    for (int t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (!pirate_covets(t, loc)) continue;
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
        if (!pirate_covets(i, loc)) continue;
        if (toting(i) && at_hand(i, loc)) {
            steal_all_your_treasure(loc);
            return_pirate_to_lair(chest_needs_placing);
            return;
        }
        if (here(i, loc)) {
            /* There is a treasure in this room, but we're not carrying
             * it openly. The pirate won't pounce unless he can steal
             * the treasure; so he'll try to remain quiet. */
            stalking = true;
        }
    }
    /* tally is the number of treasures we haven't seen; lost_treasures is
     * the number we never will see (due to killing the bird or destroying
     * the troll bridge). */
    if (tally == lost_treasures+1 && !stalking && chest_needs_placing &&
        objs(LAMP).prop && at_hand(LAMP, loc)) {
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
    if (odloc[0] != dloc[0] && pct(30)) {
        puts("There are faint rustling noises from the darkness behind you.");
    }
}

bool forbidden_to_pirate(Location loc)
{
    switch (loc) {
        case R_MAZEA46: case R_MAZEA47: case R_MAZEA48:
        case R_MAZEA54: case R_MAZEA56: case R_MAZEA58:
        case R_MAZEA82: case R_MAZEA85: case R_MAZEA86:
        case R_NESIDE: case R_CORR: case R_FORK: case R_WARM:
        case R_VIEW: case R_CHAMBER: case R_LIME: case R_FBARR:
        case R_BARR:  /* dwarves won't meet the bear */
        case R_SWORD: case R_CHIMTOP: /* pirate won't steal the sword */
        case R_BOOTH: /* not enough shadows */
            return true;
        default:
            return false;
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
        if (is_well_inside(loc)) dflag = 1;
    } else if (dflag == 1) {
        if (is_well_inside(loc) && pct(5)) {
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
                    if (!is_well_inside(newloc)) continue;  /* don't follow above level 2 */
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
                            (dseen[j] && is_well_inside(loc)));
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
                            if (ran(1000) < 250*(dflag-2)) ++stick;
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

void immobilize_axe_via(ObjectWord animal, Location loc)
{
    move(AXE, loc);
    immobilize(AXE);
    juggle(animal);
    switch (animal) {
        case BEAR:
            puts("The axe misses and lands near the bear where you can't get at it.");
            objs(AXE).prop = 1;
            break;
        case WUMPUS:
            puts("You can't even hit a sleeping Wumpus!  The axe is now lying too near" SOFT_NL
                 "the Wumpus for you to retrieve it.");
            objs(AXE).prop = 2;
            break;
        case DOG:
            puts("The dog easily dodges the axe, which lands beyond him where you can't" SOFT_NL
                 "get at it.");
            objs(AXE).prop = 3;
            break;
        default: assert(false);
    }
}

void remobilize_axe(int expected_prop)
{
    assert(1 <= expected_prop && expected_prop <= 3);
    if (objs(AXE).prop == expected_prop) {
        objs(AXE).prop = 0;
        mobilize(AXE);
    }
}


/*========== Closing the cave. ============================================
 * This section corresponds to sections 103, 178--181 in Knuth.
 */

int lamp_limit;  /* countdown till darkness */
bool have_warned_about_lamp = false;
int clock1 = 30, clock2 = 50;  /* clocks that govern closing time */
int clock3;  /* limits the endgame */
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
    move(REPO_BOOK, R_NEEND); objs(REPO_BOOK).prop = -1;
    move(BOOTH, R_NEEND); objs(BOOTH).prop = 3;
    move(GRATE, R_SWEND); objs(GRATE).prop = 0;
    move(SNAKE, R_SWEND); objs(SNAKE).prop = -2;  /* not blocking the way */
    move(BIRD, R_SWEND); objs(BIRD).prop = -2;  /* caged */
    move(CAGE, R_SWEND); objs(CAGE).prop = -1;
    move(ROD2, R_SWEND); objs(ROD2).prop = -1;
    move(PILLOW, R_SWEND); objs(PILLOW).prop = -1;
    move(MIRROR_, R_SWEND);
    move(BOOTH_, R_SWEND);
    move(PHONE, R_REPO_BOOTH); objs(PHONE).prop = 3;
    
    for (int j = MIN_OBJ; j <= MAX_OBJ; ++j) {
        if (toting(j)) destroy(j);
    }
    closed = true;
    bonus = 10;
    clock3 = 20+ran(20);
}

/* Return true if the cave just closed. */
bool check_clocks(Location loc)
{
    if (clock1 > 0) {
        clock1 -= (tally == 0 && is_well_inside(loc) && loc != R_Y2);
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
            if (objs(BEAR).prop != 3) {
                destroy(BEAR);
                remobilize_axe(1);
            }
            objs(CHAIN).prop = 0; mobilize(CHAIN);
            /* The Wumpus must be dead, or we wouldn't have seen the ring. */
            assert(objs(WUMPUS).prop == 6);
            assert(objs(AXE).prop != 2);
            /* The dragon, dog, and gnome are still around post-closing.
             * Long unconditionally remobilizes the axe; I've decided that
             * the proper behavior is to leave it immobilized if it was
             * immobilized by the dog. */
        }
    } else if (clock2 > 0) {
        --clock2;
        if (clock2 == 0) {
            close_the_cave();
            return true;
        }
    } else {
        assert(clock3 > 0);
        --clock3;
        if (clock3 == 7) {
            puts("The telephone in the booth has begun to ring.");
            objs(BOOTH).prop = 0;
            objs(PHONE).prop = 0;
        } else if (clock3 == 0) {
            puts("The constant ringing has awakened the dwarves!\n"
                 "There are now several threatening little dwarves in the room with" SOFT_NL
                 "you!  Most of them throw knives at you!  All of them get you!");
            quit();
        }
    }
    return false;
}

void check_lamp(Location loc)
{
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
int health = 100;  /* from 0 to 100; used only by the radium puzzle */
int verbose_interval = 5;  /* command BRIEF sets this to 10000 */
int look_count = 0;  /* the first three times you LOOK, you get a message */
bool terse = false;  /* command TERSE sets this */
int foobar;  /* progress in the FEE FIE FOE FOO incantation */
int combo;  /* progress in the safe combination 7-22-34 */
int kaleidoscope_count;  /* progress in the kaleidoscope maze */

void give_optional_magic_hints(Location loc)
{
    if (loc == R_Y2 && pct(25) && !cave_is_closing()) {
        puts("A hollow voice says \"PLUGH\".");
    } else if (loc == R_KNOLL && places[R_KNOLL].visits == 1) {
        puts("A tiny elf runs straight at you, shouts \"Phuce!\", and" SOFT_NL
             "disappears into the forest.");
    }
}

void give_optional_lamp_hint(Location loc)
{
    static int waste = 0;
    if (is_outside(loc) && objs(LAMP).prop) {
        /* Long gives this warning even if the lamp isn't being toted. */
        waste += 1;
        if (waste <= 12) return;
        puts("You know, you are wasting your batteries by wandering around out" SOFT_NL
             "here with your light on.");
    }
    waste = 0;
}

bool update_wumpus(Location loc)
{
    if (being_chased != 0) {
        being_chased += 1;
        objs(WUMPUS).prop = being_chased/2;
        move(WUMPUS, loc);
        if (being_chased == 10) {
            if (now_in_darkness(loc)) {
                puts("A hairy paw reaches out of the darkness and....");
            }
            puts("\"Chomp, chomp.\"  Crunch!  Chew!  Slurp!  Smack!  Yum!!!");
            return true;
        }
    }
    return false;
}

bool update_health(Location loc)
{
    health += (is_outside(loc) ? 3 : 1);  /* recover faster outdoors */
    if (health > 100) health = 100;
    bool radium_in_canister = (objs(RADIUM).place == -CANISTER) && !is_ajar(CANISTER);
    if (here(RADIUM, loc) && !radium_in_canister) {
        health -= 7;
        if (health < 60) {
            switch ((60-health) / 10) {
                case 0:
                    puts("Is it hot in here?  You are flushed and sweating.");
                    break;
                case 1:
                    puts("You are feeling definitely peculiar, weak....");
                    break;
                case 2:
                    puts("You're dizzy, nauseous.  You can barely stand.");
                    break;
                case 3:
                    puts("You are really ill.  If you don't find an antidote soon, it's" SOFT_NL
                         "curtains.");
                    break;
                case 4:
                    puts("You are a walking wound.  You are very weak.  You'd better find out" SOFT_NL
                         "what's wrong before it's too late.");
                    break;
                case 5:
                    /* Long has "asprin". */
                    puts("Sheeesh!  What a mess!  Your hair has fallen out and your skin is" SOFT_NL
                         "covered with blisters.  And not an aspirin in sight!");
                    break;
                case 6:
                    puts("Well, you tried, but your strength is gone.  The agony is finally" SOFT_NL
                         "over.");
                    return true;
            }
        }
    }
    return false;
}

void spot_treasure(ObjectWord t)
{
    if (closed) return;
    if (objs(t).prop >= 0) return;
    assert(is_treasure(t));  /* You've spotted a treasure */
    switch (t) {
        case CLOAK:  /* trapped */
        case RING:  /* worn */
            objs(t).prop = 2;
            break;
        case RUG:  /* trapped */
        case CHAIN:  /* locked */
        case SWORD:  /* embedded */
        case CASK:  /* empty */
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

int look_around(Location loc, bool dark, bool was_dark)
{
    const char *room_description;
    bool too_bright = (loc == R_CRYSTAL && objs(LAMP).prop && at_hand(LAMP, loc));
    if (too_bright) {
        if (pct(35)) return 'p';  /* fall in a pit */
        room_description =
            "The glare is absolutely blinding.  If you proceed you are likely" SOFT_NL
            "to fall into a pit.";
    } else if (!is_forced(loc) && dark) {
        if (was_dark && pct(35)) return 'p';  /* fall in a pit */
        room_description =
            "It is now pitch dark.  If you proceed you will likely fall into a pit.";
    } else if (places[loc].short_desc == NULL ||
               (!terse && places[loc].visits % verbose_interval == 0)) {
        room_description = places[loc].long_desc;
    } else {
        room_description = places[loc].short_desc;
    }
    if (holding(BEAR) && !dark) {
        puts("You are being followed by a very large, tame bear.");
    }
    if (room_description != NULL) {
        /* R_CHECK's description is NULL. */
        printf("%s\n", room_description);
    }
    if (is_forced(loc)) return 't';  /* goto try_move; */
    give_optional_magic_hints(loc);
    give_optional_lamp_hint(loc);
    if (update_wumpus(loc)) return 'd';  /* eaten by the Wumpus */
    if (update_health(loc)) return 'd';  /* poisoned to death */
    if (!dark && !too_bright) {
        places[loc].visits += 1;
        /* Describe the objects at this location. */
        for (struct ObjectData *t = places[loc].objects; t != NULL; t = t->link) {
            struct ObjectData *tt = t->base ? t->base : t;
            spot_treasure(tt - &objs(0));
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

struct {
    int count;
    bool given;
    int thresh;
    int cost;
    const char *prompt;
    const char *hint;
} hints[] = {
    { 0, false, 0, 5, "Welcome to ADVENTURE!!  Would you like instructions?",
    "Somewhere nearby is Colossal Cave, where others have found fortunes in" SOFT_NL
    "treasure and gold, though it is rumored that some who enter are never" SOFT_NL
    "seen again.  Magic is said to work in the cave.  I will be your eyes" SOFT_NL
    "and hands.  Direct me with natural English commands.  I should warn" SOFT_NL
    "you that I look at only the first six letters of each word. Also you" SOFT_NL
    "should enter \"Northeast\" as \"NE\" to distinguish it from \"North\"." SOFT_NL
    /* I don't know why Long kept the above line in. Six letters is enough to
     * distinguish NORTHE(AST) from NORTH. */
    "(Should you get stuck, type \"HELP\" or \"?\" for some general hints.)" SOFT_NL
    "Good luck!\n" /* Long has "Good Luck!" */
    "                        - - - -\n"
    "This version of Adventure was written by Willie Crowther, Don Woods," SOFT_NL
    "David Long, and Anonymous. It was translated into modern Fortran by" SOFT_NL
    "Doug McDonald, and then to ANSI C by Arthur O'Dwyer.\n\n" },
    { 0, false, 0, 10,
    "Hmmm, this looks like a clue, which means it'll cost you 10 points to" SOFT_NL
    "read it.  Should I go ahead and read it anyway?",
    /* Long's hint is more to the point than Woods'. */
    "It says, \"Not all black rods are magic wands. Some are useful for" SOFT_NL
    "other cave construction purposes. There might be some around here." },
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
    "Don't go west." },
    { 0, false, 6, 10, "Do you want to extract the sword from the anvil?",
    "If you could convince me of your royal blood, you might get somewhere." },
    { 0, false, 16, 5, "Are you trying to go up the slide?",
    /* This is Long's original message, but it doesn't seem appropriate here. */
    "Maybe you'd better look for another way into the cave." }
};

void offer(int j)
{
    if (j == 0) {
        hints[j].given = yes(hints[j].prompt, hints[j].hint, NULL);
    } else if (j == 1) {
        hints[j].given = yes(hints[j].prompt, hints[j].hint, ok);
    } else {
        if (!yes(hints[j].prompt, NULL, ok)) return;
        printf("I am prepared to give you a hint, but it will cost you %d points.\n", hints[j].cost);
        hints[j].given = yes("Do you want the hint?", hints[j].hint, ok);
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
                            burden(0) > 1) {
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

#define MAX_SCORE 551
#define MAX_DEATHS 3
bool gave_up;
int death_count;

int score(void)
{
    /* Calculate what the player's score would be if he quit now.
     * This may be the end of the game, or he may just be wondering
     * how he's doing. */
    int score = 0;

    static const struct {
	ObjectWord obj;
	int place;
	int prop;
	int value;
    } qk[] = {
	{ CLOAK,    -SAFE,  0, 3 },
	{ GOLD,    -CHEST,  0, 2 },
	{ DIAMONDS, -SAFE,  0, 2 },
	{ HORN,     -SAFE,  0, 2 },
	{ JEWELS,   -SAFE,  0, 1 },
	{ COINS,    -SAFE,  0, 5 },
	{ CHEST,  R_HOUSE,  0, 5 },
	{ EGGS,     -SAFE,  0, 3 },
	{ TRIDENT, -CHEST,  0, 2 },
        { VASE,   R_HOUSE,  0, 2 },
	{ EMERALD,  -SAFE,  0, 3 },
	{ PYRAMID,  -SAFE,  0, 4 },
	{ PEARL,    -SAFE,  0, 4 },
	{ RUG,    R_HOUSE,  0, 3 },
	{ SPICES,   -SAFE,  0, 1 },
	{ CHAIN,    -SAFE,  0, 4 },
	{ SWORD,    -SAFE,  0, 4 },
	{ CROWN,    -SAFE,  0, 2 },
	{ SHOES,    -SAFE,  0, 3 },
	{ LYRE,     -SAFE,  0, 1 },
	{ SAPPHIRE, -SAFE,  0, 1 },
	{ GRAIL,    -SAFE,  0, 2 },
	{ CASK,    -CHEST,  0, 3 },
	{ RING,     -SAFE,  0, 4 },
	{ CLOVER,   -SAFE,  0, 1 },
	{ TREE,     -SAFE,  0, 5 },
	{ DROPLET,  -SAFE,  0, 5 },
        { BOOK,     -SAFE,  0, 2 },
        { RADIUM, -CANISTER,0, 4 },
        { BALL,     -SAFE,  0, 2 }
    };
    /* First tally up the treasures. */
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
	if (!is_treasure(t)) continue;
	for (int i=0; qk[i].obj != NOTHING; ++i) {
	    if (qk[i].obj != t) continue;
	    /* Found the data entry for this treasure. */

	    /* If you've picked up this treasure, score some points. */
	    if (objs(t).prop >= 0) score += 2*qk[i].value;

	    /* The treasure must be in a particular place... */
	    if (objs(t).place != qk[i].place) continue;

	    /* ...and (if it's the vase, for example) not be broken. */
	    if (objs(t).prop != qk[i].prop) continue;

	    /* Some objects need to be in the chest in the well house. */
	    if (qk[i].place == -CHEST && objs(CHEST).place != R_HOUSE) continue;

	    /* The radium needs to be in the canister in the safe. */
	    if (qk[i].place == -CANISTER && objs(CANISTER).place != -SAFE) continue;

	    /* Okay, score full points for this treasure! */
	    score += 5*qk[i].value;
	}
    }

    if (objs(MAG).place == R_WITT) score += 1;  /* last lousy point */

    /* Subtract points for hints given. */
    for (int i = 0; i < 8; ++i) {
        if (hints[i].given) score -= hints[i].cost;
    }

    /* This code is a little evil: the following components of your
     * score are pro-rated based on how long you've been playing! */

    int ascore = 10 * (MAX_DEATHS - death_count);
    if (!gave_up) ascore += 4;
    if (dflag != 0) ascore += 25;  /* bonus for getting into the cave */
    if (cave_is_closing()) ascore += 20;  /* bonus for making it this far */
    ascore += bonus;
    ascore += 16;  /* Round up the score to an even 551 points. */

    if (turns < 100) score += 0;
    else if (turns < 200) score += ascore/3;
    else if (turns < 300) score += 2*ascore/3;
    else score += ascore;

    if (score < 0) score = 0;

    return score;
}

void attempt_score(void)
{
    gave_up = true;
    printf("If you were to quit now, you would score %d out of %d using %d turns.\n",
           score(), MAX_SCORE, turns);
    gave_up = false;
    /* Here Woods asks if you indeed wish to quit now.
     * Long's version removes the prompt. */
}

#define HIGHEST_CLASS 8
static int class_score[] = { 72, 130, 200, 250, 350, 450, 500, 550, 9999 };
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
    printf("%s\n\nTo achieve the next higher rating", class_message[rank]);
    if (rank < HIGHEST_CLASS) {
        int delta = class_score[rank] - s;
        printf(", you need %d more point%s.\n",
               delta, (delta==1 ? "" : "s"));
    } else {
        puts(" would be a neat trick!\n"
             "CONGRATULATIONS!!");
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
    health = 100;
    if (toting(LAMP)) objs(LAMP).prop = 0;
    destroy(WATER);
    destroy(OIL);
    if (being_chased != 0) {
        /* Reset the dreaded Wumpus. */
        being_chased = 0;
        objs(WUMPUS).prop = 0;
        move(WUMPUS, R_CLOAKROOM);
    }
    for (int j = MAX_OBJ; j >= MIN_OBJ; --j) {
        /* Note that containers retain their contents. */
        if (holding(j)) {
            move(j, (j == LAMP) ? R_ROAD : last_safe_place);
        }
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

Location attempt_plover_passage(Location from)
{
    if (burden(0) == (burden(EMERALD) * holding(EMERALD)))
        return R_ALCOVE + R_PLOVER - from;
    puts("Something you're carrying won't fit through the tunnel with you." SOFT_NL
         "You'd best take inventory and drop something.");
    return from;
}

void attempt_break(Location loc, ObjectWord obj)
{
    if (obj == MIRROR && closed) {
        puts("You strike the mirror a resounding blow, whereupon it shatters into a" SOFT_NL
             "myriad tiny fragments.");
        dwarves_upset();
        assert(false);  /* dwarves_upset() should not return */
    }

    if (obj == VASE && objs(VASE).prop == 0) {
        puts("You have taken the vase and hurled it delicately to the ground.");
        if (toting(VASE)) move(VASE, loc);
        objs(VASE).prop = 2;  /* broken */
        immobilize(VASE);
    } else if (obj == BOTTLE && objs(BOTTLE).prop != 3) {
        puts("You have smashed your bottle all over the ground.");
        ObjectWord liq = liquid_contents(BOTTLE);
        if (liq != NOTHING) destroy(liq);
        if (toting(BOTTLE)) move(BOTTLE, loc);
        objs(BOTTLE).prop = 3;  /* broken */
        immobilize(BOTTLE);
    } else if (obj == SWORD) {
        if (!holding(SWORD)) {
            puts("You aren't carrying it!");
        } else {
            /* Long incorrectly sets PROP(SWORD)=4 instead of =3, making
             * it impossible to interact with a broken sword except at
             * Sword Point (post-YANK). */
            puts("You have smashed your sword to smithereens against a rock.");
            move(SWORD, loc);
            objs(SWORD).prop = 2;  /* broken */
            immobilize(SWORD);
        }
    } else if (obj == MIRROR) {
        puts("It is too far up for you to reach.");
    } else {
        puts("It is beyond your power to do that.");
    }
}

bool maybe_wake_the_wumpus(Location loc)
{
    if (there(WUMPUS, loc) && objs(WUMPUS).prop == 0) {
        being_chased = 1;
        objs(WUMPUS).prop = 1;
        puts("You turkey!!!  Now you've done it!  It took some effort, but you" SOFT_NL
             "woke up the Wumpus.  He slowly opens one red eye, and then another," SOFT_NL
             "and then one more (!!), and looks at you sleepily.  He had been" SOFT_NL
             "dreaming of a late snack.  If you don't act quickly, you'll" SOFT_NL
             "be a *late* adventurer!");
        return true;
    } else {
        return false;
    }
}

void attempt_wake(Location loc, ObjectWord obj)
{
    /* WAKE DWARF will wake the wumpus, too. */
    if (maybe_wake_the_wumpus(loc)) {
        /* Nothing else to do. */
    } else if (there(DOG, loc) && objs(DOG).prop) {
        puts("That wouldn't be wise.  It is best to let sleeping dogs lie.");
    } else if (closed && (obj == DWARF)) {
        dwarves_upset();
    } else {
        puts("Don't be ridiculous!");
    }
}

void attempt_drop(Location loc, ActionWord verb, ObjectWord obj, int prep, int iobj)
{
    switch (verb) {
        case DROP:
        case YANK:
        case LEAVE:
        case TOSS:
        case TAKE:  /* TAKE OFF */
            break;
        default:
            assert(false);
    }

    if (obj == ROD && !toting(ROD) && toting(ROD2)) {
        obj = ROD2;
    } else if ((obj == liquid_contents(BOTTLE)) && toting(BOTTLE)) {
        obj = BOTTLE;
    } else if ((obj == liquid_contents(CASK)) && toting(CASK)) {
        obj = CASK;
    }

    if (!toting(obj)) {
        indent_appropriately();
        printf("You aren't carrying %s!\n", is_plural(obj) ? "them" : "it");
        return;
    }

    if (prep == INTO) {
        /* DROP COINS INTO PHONE */
        attempt_insert_into(loc, obj, iobj);
        return;
    }

    if (obj == BIRD && here(SNAKE, loc)) {
        puts("The little bird attacks the green snake, and in an astounding flurry" SOFT_NL
             "drives the snake away.");
        if (closed) dwarves_upset();
        move(BIRD, loc);
        destroy(SNAKE);
        objs(SNAKE).prop = 1;  /* gone */
        return;
    }

    if (obj == POLE && holding(BOAT)) {
        puts("Setting yourself adrift in the boat with no way to propel it would" SOFT_NL
             "not be very smart.  Best to keep the pole.");
        return;
    } else if (obj == BIRD && is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) {
        puts("The little bird attacks the green dragon, and in an astounding flurry" SOFT_NL
             "gets burnt to a cinder.  The ashes blow away.");
        destroy(BIRD);
        if (there(SNAKE, R_HMK)) ++lost_treasures;  /* the jewelry */
        return;
    } else if (obj == BEAR && is_at_loc(TROLL, loc)) {
        puts("The bear lumbers toward the troll, who lets out a startled shriek and" SOFT_NL
             "scurries away.  The bear soon gives up the pursuit and wanders back.");
        destroy(TROLL); destroy(TROLL_);
        move(NO_TROLL, R_SWSIDE); move(NO_TROLL_, R_NESIDE);
        juggle(CHASM);
        objs(TROLL).prop = 2;  /* permanently scared away */
    } else if (obj == VASE && loc != R_SOFT) {
        if (there(PILLOW, loc)) {
            puts("The vase is now resting, delicately, on a velvet pillow.");
            objs(VASE).prop = 0;
        } else {
            /* Long has "ming". */
            puts("The Ming vase drops with a delicate crash.");
            objs(VASE).prop = 1;
            immobilize(VASE);
        }
    }

    if (obj == BOAT) {
        objs(BOAT).prop = 0;  /* unnavigable */
    }

    move(obj, loc);
    switch (verb) {
        case TAKE: puts(ok); break;
        case LEAVE: puts("Left."); break;
        case TOSS: puts("Thrown."); break;
        default: puts("Dropped."); break;
    }
}

void attempt_fill(Location loc, ObjectWord obj, ObjectWord iobj)
{
    /* FILL obj WITH iobj */
    assert(obj != NOTHING);
    if (!is_vessel(obj)) {
        puts("You can't fill that.");
        return;
    }
    if (iobj == NOTHING) {
        iobj = liquid_at_location(loc);
    } else if (!is_liquid(iobj) || is_broken(obj)) {
        /* Long misses these two cases; PUT LAMP IN BOTTLE
         * causes serious mischief, FILL (smashed) BOTTLE
         * repairs the bottle but leaves it immobile, and
         * FILL (smashed) VASE breaks the vase all over again. */
        puts("Don't be ridiculous!");
        return;
    }
    if (obj == BOTTLE || obj == CASK) {
        const char *Your_bottle = (obj==CASK ? "The cask" : "Your bottle");
        if (iobj == NOTHING) {
            indent_appropriately();
            printf("There is nothing here %s.\n",
                   obj==CASK ?
                       "which you would want to put into the cask" :
                       "with which to fill the bottle");
        } else if (liquid_contents(obj) != NOTHING) {
            indent_appropriately();
            printf("%s is already full.\n", Your_bottle);
        } else {
            /* Fill the vessel. */
            assert(objs(obj).prop == 1);
            if (iobj == WATER) {
                indent_appropriately();
                printf("%s is now full of water.\n", Your_bottle);
                objs(obj).prop = 0;
                insert(obj==CASK ? WATER_IN_CASK : WATER, obj);
            } else if (iobj == OIL) {
                indent_appropriately();
                printf("%s is now full of oil.\n", Your_bottle);
                objs(obj).prop = 2;
                insert(obj==CASK ? OIL_IN_CASK : OIL, obj);
            } else if (iobj == WINE) {
                /* Long has "The bottle" in this case, but I don't think
                 * it's worth the inconsistency. I'm bugfixing a lot of this
                 * logic, so the messages won't precisely match up anyway. */
                indent_appropriately();
                printf("%s is now full of wine.\n", Your_bottle);
                objs(obj).prop = 4;
                insert(obj==CASK ? WINE_IN_CASK : WINE, obj);
            }
        }
    } else if (obj == VASE) {
        if (iobj == NOTHING) {
            /* Long also prints this message if !holding(VASE), but I don't
             * see how that makes any sense. */
            puts("There is nothing here with which to fill the vase.");
        } else {
            puts("The sudden change in temperature has delicately shattered the vase.");
            objs(VASE).prop = 2;  /* broken */
            move(VASE, loc);
            immobilize(VASE);
        }
    } else if (obj == GRAIL) {
        puts("The chalice is slightly cracked. It won't hold any liquids.");
    } else {
        puts("You can't fill that.  It would leak all over the place.");
    }
}

void attempt_blast(Location loc)
{
    if (closed) {
        /* Unlike the original, you don't have to pick up the rod in order
         * for it to kill you. */
        if (there(ROD2, R_REPO_BOOTH)) {
            if (loc == R_SWEND) {
                bonus = 30;
                puts("There is a loud explosion, and a twenty-foot hole appears in the far" SOFT_NL
                     "wall, burying the dwarves in the rubble.  You march through the hole" SOFT_NL
                     "and find yourself in the Main Office, where a cheering band of" SOFT_NL
                     "friendly elves carry the conquering adventurer off into the sunset.");
            } else {
                bonus = 25;
                puts("There is a loud explosion, and a twenty-foot hole appears in the far" SOFT_NL
                     "wall, burying the snakes in the rubble.  A river of molten lava pours" SOFT_NL
                     "in through the hole, destroying everything in its path, including you!");
            }
        } else {
            bonus = 20;
            puts("There is a loud explosion, and you are suddenly splashed across the" SOFT_NL
                 "walls of the room.");
        }
        quit();
    } else {
        puts("Blasting requires dynamite.");
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
    } else if (obj == SWORD && objs(SWORD).prop == 4) {
        puts("The handle is now too slippery to grasp.");
    } else if (obj == CLOAK && objs(CLOAK).prop == 2) {
        puts("The cloak is stuck tight under the rocks.  You'll probably have to" SOFT_NL
             "yank it out.");
    } else if (obj == AXE && objs(AXE).prop == 2) {
        /* prop=1 is axe trapped by bear; prop=3 is axe trapped by dog. */
        puts("The Wumpus is between you and the axe.  You can't reach it.");
    } else if (obj == PHONE) {
        puts("It's attached to the wall!");
    } else if (obj == BEES || obj == HIVE) {
        puts("The hum of the bees rises to an angry buzz as you move towards" SOFT_NL
             "the hive.");
    } else if (obj == STICKS) {
        puts("The sticks and branches are all in a tangle and are stuck in the" SOFT_NL
             "mud.  You'd need a shovel to dig them out.");
    } else {
        noway();
    }
}

bool maybe_reveal_safe(Location loc)
{
    /* Long's original game fails to reveal the safe via PUT POSTER IN SACK. */
    if (there(SAFE, R_LIMBO)) {
        assert(loc == R_HOUSE);
        puts("Hidden behind the poster is a steel safe, embedded in the wall.");
        objs(POSTER).prop = 1;
        drop(SAFE, R_HOUSE);
        drop(SAFE_WALL, R_HOUSE);
        return true;
    }
    return false;
}

void attempt_insert_into(Location loc, ObjectWord obj, ObjectWord iobj)
{
    if (obj == SWORD && iobj == ANVIL && objs(SWORD).prop == 0) {
        puts("Only wizards can do that!");
    } else if (obj == iobj) {
        puts("You can't put a thing into itself!");
    } else if (!is_vessel(iobj)) {
        noway();
    } else if (obj == BOAT && iobj == CHEST) {
        noway();
    } else if (objs(obj).base != NULL) {
        take_something_immobile(obj);
    } else if (is_liquid(obj)) {
        /* PUT WATER IN LAMP redirects to FILL LAMP WITH WATER.
         * Long has a bug here: he also redirects PUT X IN VESSEL
         * to FILL VESSEL WITH X, for values of VESSEL equal to
         * BOTTLE, CASK, VASE, or GRAIL. This drops us into the
         * middle of the FILL codepath with a bad iobj; major
         * mischief ensues.
         *   I've fixed that bug here, by not redirecting anything
         * to FILL unless the obj is a liquid, and also in
         * fits_inside(), by ensuring that nothing fits inside
         * those four vessels. */
        attempt_fill(loc, iobj, obj);
    } else if (!is_ajar(iobj)) {
        puts("It's not open.");
    } else if (obj == BIRD || iobj == CAGE) {
        if (obj != BIRD) {
            puts("It won't fit!");
        } else if (iobj == BOAT) {
            /* Long has a bug here: PUT BIRD IN BOAT will
             * succeed, allowing you to have a wooden boat
             * containing "Little bird in cage" without
             * having the cage. I've patched the bug.
             * (Ditto PUT BIRD IN CHEST.) */
            attempt_drop(loc, DROP, BIRD, NOTHING, NOTHING);
        } else if (iobj != CAGE) {
            puts("Are you kidding?  Do you want to suffocate the poor thing?");
        } else {
            attempt_take(loc, TAKE, BIRD, NOTHING, NOTHING);
        }
    } else if ((obj == COINS || obj == SLUGS) && iobj == PHONE) {
        destroy(obj);
        puts("The coin drops into the slot with a dull \"clunk\".  There is no" SOFT_NL
             "dial tone.");
    } else if ((obj == COINS || obj == SLUGS) && iobj == MACHINE) {
        destroy(obj);
        move(BATTERIES, loc);
        if (objs(BATTERIES).prop) {
            puts("Hmmm, I see you have already gone through an extra set of batteries." SOFT_NL
                 "I'll get rid of the trash for you.\n");
            objs(MACHINE).prop = 1;
        }
        objs(BATTERIES).prop = 0;
        puts("There are fresh batteries here.");
    } else if (iobj == LAMP) {
        if (obj != BATTERIES || objs(BATTERIES).prop) {
            noway();
        } else {
            objs(BATTERIES).prop = 1;
            move(BATTERIES, loc);
            lamp_limit = 400;
            objs(LAMP).prop = 1;  /* turn it on */
            have_warned_about_lamp = false;
            puts("Your lamp is now shining with renewed strength.");
        }
    } else if (!fits_inside(obj, iobj)) {
        puts("It won't fit.");
    } else {
        insert(obj, iobj);
        if (obj == POSTER && maybe_reveal_safe(loc)) {
            /* No additional message. */
        } else {
            puts(ok);
        }
    }
}

void attempt_remove_from(Location loc, ObjectWord obj, int iobj)
{
    /* Long doesn't check iobj here, but we might as well. */
    if (obj == RING && iobj == WUMPUS && objs(RING).prop == 2) {
        /* TAKE RING FROM WUMPUS */
        attempt_take(loc, TAKE, RING, NOTHING, NOTHING);
        return;
    }
    /* Handle REMOVE RING (from whatever it's in). */
    if (iobj == NOTHING) {
        if (enclosed(obj)) {
            iobj = -objs(obj).place;
            assert(MIN_OBJ <= iobj && iobj <= MAX_OBJ);
        } else {
            puts("It's not inside anything.");
            return;
        }
    }
    assert(iobj != NOTHING);
    if (objs(obj).place != -iobj) {
        puts("It isn't there!");
    } else if (!is_ajar(iobj)) {
        puts("You can't get at it.");
    } else if (obj == WATER || obj == OIL || obj == WINE) {
        /* TAKE WINE FROM CASK, for example */
        puts("How?");
    } else if (!toting(obj) && burden(0)+burden(obj) > 15) {
        puts("It's too heavy.  You'll have to drop something first.");
    } else {
        remove_from_containers(obj);
        if (obj == BIRD) {
            /* The bird can't be held. */
            attempt_drop(loc, DROP, BIRD, NOTHING, NOTHING);
        } else {
            puts("Taken.");
        }
    }
}

void print_taken_message(ActionWord verb)
{
    if (verb == YANK) {
        puts("Ok, ok.  No need to be grabby.");
    } else {
        puts("Taken.");
    }
}

void attempt_take(Location loc, ActionWord verb, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    if (obj == BIRD && !closed && at_hand(BIRD, loc) && enclosed(BIRD)) {
        /* Strangely, Long prints this message only for the wording
         * TAKE BIRD FROM CAGE (or whatever container, actually).
         * The wording REMOVE BIRD will drop the bird regardless.
         * Also, when the cave is closed, TAKE BIRD FROM CAGE will
         * drop the bird --- just to increase the player's chances
         * of messing up in the endgame, I guess! */
        puts("Fiddling with the bird in its cage is not useful." SOFT_NL
             "If you had it in your hand it would make a mess.");
        /* Here Long does a GOTO 2, but I see no reason to reproduce that. */
    } else if (verb == YANK && obj == BEAR && objs(BEAR).prop <= 1) {
        puts("Pulling an angry bear around is a good way to get your arm ripped off.");
    } else if (verb == YANK && obj == CLOAK && objs(CLOAK).prop == 2) {
        assert(loc == R_CLOAKROOM);
        if (burden(0)+burden(CLOAK) > 15) {
            puts("It's too heavy.  You'll have to drop something first.");
        } else {
            puts("You have jerked the cloak free of the rocks.  However, in doing" SOFT_NL
                 "so you have caused a small rockslide, blocking the entrance" SOFT_NL
                 "and making an unholy din.");
            objs(CLOAKROOM_ROCKS).prop = 1;
            objs(CLOAK).prop = 0;
            mobilize(CLOAK);
            carry(CLOAK);
            maybe_wake_the_wumpus(loc);
        }
    } else if (prep == OFF) {
        /* TAKE OFF CLOAK redirects to DROP CLOAK */
        if (obj != NOTHING && iobj != NOTHING) {
            confuz();
        } else {
            attempt_drop(loc, TAKE, (obj != NOTHING) ? obj : iobj, OFF, NOTHING);
        }
    } else if (holding(obj)) {
        if (obj == BOAT) {
            puts("You're already in it!");
        } else if (is_plural(obj)) {
            puts("You're already carrying them!");
        } else {
            puts("You are already carrying it!");
        }
    } else if (objs(obj).base != NULL) {
        take_something_immobile(obj);
    } else if (prep == INTO) {
        attempt_insert_into(loc, obj, iobj);
    } else if (prep == FROM || enclosed(obj)) {
        attempt_remove_from(loc, obj, iobj);
    } else if (obj == WATER || obj == OIL || obj == WINE) {
        /* TAKE WATER. Long's line 20110.
         * Unfortunately Long's code is ridiculously buggy; rather than try
         * to emulate all of the bugs, I'm simply rewriting it from scratch.
         * Long includes message 314: "Do you want it in the bottle or the
         * cask?" but doesn't use that message anywhere.
         */
        bool can_fill_bottle = at_hand(BOTTLE, loc) && objs(BOTTLE).prop == 1;
        bool can_fill_cask = at_hand(CASK, loc) && objs(CASK).prop == 1;
        bool good_bottle_on_ground = here(BOTTLE, loc) && !holding(BOTTLE) && liquid_contents(BOTTLE) == obj;
        bool good_cask_on_ground = here(CASK, loc) && !holding(CASK) && liquid_contents(CASK) == obj;
        bool full_bottle_here = here(BOTTLE, loc) && liquid_contents(BOTTLE) != NOTHING;
        bool full_cask_here = here(CASK, loc) && liquid_contents(CASK) != NOTHING;
        if (good_bottle_on_ground) {
            /* Even if the cask is here too, let the bottle
             * take precedence. */
            attempt_take(loc, verb, BOTTLE, NOTHING, NOTHING);
        } else if (good_cask_on_ground) {
            attempt_take(loc, verb, CASK, NOTHING, NOTHING);
        } else if (can_fill_bottle && can_fill_cask) {
            puts("How?");
        } else if (can_fill_bottle) {
            attempt_fill(loc, BOTTLE, obj);
        } else if (can_fill_cask) {
            attempt_fill(loc, CASK, obj);
            /* Below this point, we fail for some reason or other. */
        } else if (full_bottle_here && full_cask_here) {
            puts("Your containers are both full.");
        } else if (full_bottle_here && at_hand(BOTTLE, loc)) {
            assert(!full_cask_here);
            puts("Your bottle is already full.");
        } else if (full_cask_here && at_hand(CASK, loc)) {
            assert(!full_bottle_here);
            puts("The cask is already full.");
        } else {
            puts("You have nothing in which to carry it.");
        }
    } else if (obj != BEAR && burden(0)+burden(obj) > 15) {
        puts("It's too heavy.  You'll have to drop something first.");
    } else if (obj == BOAT) {
        if (!toting(POLE) && !there(POLE, -BOAT)) {
            puts("The boat's oars were stolen by the dwarves to play bing-bong." SOFT_NL
                 "(That's dwarvish ping-pong " EMDASH("--") " with rocks!).  You have no way" SOFT_NL
                 "to propel the boat.");
            /* So you can't get in the boat after all. */
        } else {
            puts("You are now sitting in a small boat.");
            objs(BOAT).prop = 1;
            carry(BOAT);
        }
    } else if (obj == BIRD && !objs(BIRD).prop) {
        /* In Long's version, simply dropping the rod isn't good enough
         * to charm the bird; you have to put it away somewhere the bird
         * can't see it. */
        if (at_hand(ROD, loc)) {
            puts("The bird was unafraid when you entered, but as you approach it becomes" SOFT_NL
                 "disturbed and you cannot catch it.");
        } else if (!holding(CAGE)) {
            puts("You can catch the bird, but you cannot carry it.");
        } else {
            /* Long closes the cage when you catch the bird, and won't let
             * you drop the bird until you open it again. This is mostly
             * just annoying. */
            insert(BIRD, CAGE);
            objs(CAGE).flags &= ~F_OPEN;
            print_taken_message(verb);
        }
    } else if (obj == SWORD && (objs(SWORD).prop == 1 || objs(SWORD).prop == 3)) {
        /* Long doesn't let you yank the "very clean" sword, but it doesn't
         * make much sense for POUR WATER ON SWORD to make the game unwinnable.
         * I've taken the liberty of fixing this possible oversight. */
        assert(loc == R_SWORD);
        if (verb != YANK) {
            puts("You grasp the sword's handle and pull, but the sword won't budge.");
            if (!yes("Do you want to try yanking it out?", NULL, ok))
                return;
        }
        if (objs(CROWN).flags & F_WORN) {
            puts("Taken.");
            carry(SWORD);
        } else {
            puts("You grasp the sword's handle and give a mighty heave, but with a" SOFT_NL
                 "loud clang the sword blade shatters into several fragments.");
            objs(SWORD).prop = 2;  /* broken */
            immobilize(SWORD);
        }
    } else if (obj == POSTER && maybe_reveal_safe(loc)) {
        /* No additional message. */
        carry(obj);
    } else {
        print_taken_message(verb);
        carry(obj);
    }
}

void attempt_wear(Location loc, ObjectWord obj)
{
    if (obj == SWORD && objs(SWORD).prop != 3) {
        puts("You have no scabbard!");  /* Long has "scabbord" */
    } else if (!is_wearable(obj)) {
        indent_appropriately();
        printf("Just exactly how does one wear a %s?\n", otxt[objx]);
    } else if (obj == CLOAK && objs(CLOAK).prop == 2) {
        puts("The cloak is stuck tight under the rocks.  You'll probably have to" SOFT_NL
             "yank it out.");
    } else if (objs(obj).flags & F_WORN) {
        indent_appropriately();
        printf("You are already wearing %s!\n",
               (obj == SHOES) ? "them" : "it");
    } else if (!holding(obj) && burden(0)+burden(obj) > 15) {
        /* Long's code removes the object and wears it, then
         * redirects to line 20120 (attempt_take), which will
         * unwear and drop the object if you're overburdened.
         * This code is equivalent. Note that attempting to
         * WEAR CLOAK while it's in the sack and you're
         * overburdened will quietly remove the cloak from
         * the sack. */
        remove_from_containers(obj);
        puts("It's too heavy.  You'll have to drop something first.");
    } else {
        remove_from_containers(obj);
        objs(obj).flags |= F_WORN;
        if (!holding(obj)) {
            attempt_take(loc, TAKE, obj, NOTHING, NOTHING);
        }
    }
}

void attempt_hit(Location loc, ObjectWord obj, ObjectWord iobj)
{
    if (maybe_wake_the_wumpus(loc)) {
        /* Nothing else to do. */
    } else if (obj != PHONE) {
        attempt_kill(loc, obj, iobj);
    } else if (closed) {
        puts("You've hit the jackpot!!  Hundreds of coins and slugs cascade from" SOFT_NL
             "the telephone's coin return slot and spill all over the floor of" SOFT_NL
             "the booth.");
        dwarves_upset();
    } else if (objs(PHONE).prop == 2) {
        puts("The telephone is out of order and your hand is sore.");
    } else {
        puts("A couple of lead slugs drop from the coinbox.  (Gnomes are" SOFT_NL
             "notoriously cheap....)  But you've broken the phone beyond" SOFT_NL
             "all hope.");
        assert(there(SLUGS, R_LIMBO));
        drop(SLUGS, loc);
        objs(PHONE).prop = 2;
        objs(BOOTH).prop = 2;  /* the phone has stopped ringing */
    }
}

void attempt_use_phone(void)
{
    if (closed) {
        /* Another example of the author's just being mean. */
        puts("Whoops!  The floor has opened out from under you!  It seems you" SOFT_NL
             "have fallen into a bottomless pit.  As a matter of fact, you're" SOFT_NL
             "still falling!  Well, I have better things to do than wait around" SOFT_NL
             "for you to strike bottom, so let's just assume you're dead." SOFT_NL
             "Sorry about that, Chief.");
        quit();
    } else {
        /* This message is misleading: PUT COINS IN PHONE is allowed. */
        puts("You don't have the correct change.");
    }
}

void attempt_dial(ObjectWord obj)
{
    if (obj != PHONE) {
        puts("I'm game.  Would you care to explain how?");
    } else {
        attempt_use_phone();
    }
}

void attempt_answer(ObjectWord obj)
{
    switch (obj) {
        case PHONE:
            if (objs(PHONE).prop) {
                puts("It isn't ringing!");
            } else if (closed) {
                attempt_use_phone();
            } else {
                puts("No one replies.  The line goes dead with a faint \"Click\".");
                objs(PHONE).prop = 1;
                objs(BOOTH).prop = 2;
            }
            break;
        case DWARF: case WUMPUS: case SNAKE: case BEAR:
        case DRAGON:
            puts("He didn't say anything!"); break;
        case TROLL:
            puts("He wants treasure, not gab."); break;
        case BIRD:
            /* TODO: "KILL AND ANSWER BIRD" */
            puts("It isn't a parrot.  He didn't say anything."); break;
        default:
            puts("I think you are a little confused!"); break;
    }
}

bool attempt_play(Location loc, ActionWord verb, ObjectWord obj, ObjectWord iobj)
{
    assert(verb == PLAY || verb == BLOW);
    if (obj == NOTHING) {
        assert(iobj != NOTHING);
        obj = iobj;
    }
    if (obj == HORN) {
        if (loc == R_ARCHED && !objs(ECHO).prop) {
            /* Joshua fit de battle of Jericho, and de walls...
             * Notice that the player cannot possibly be on the other side of the
             * wall yet. There are only two ways to get into the Cathedral-Altar-
             * Crypt part of the cave: via BLOW HORN or via CLICK (which requires
             * the slippers you can't get until you've been to the Cathedral). */
            puts("As the blast of the horn reverberates through the chamber, the" SOFT_NL
                 "seemingly solid rock wall crumbles away, revealing another room just" SOFT_NL
                 "beyond.  The wall was most likely worn thin by an ancient watercourse" SOFT_NL
                 "which dried up just before completely wearing away the rock.");
            objs(ECHO).prop = 1;  /* vanished */
            for (int t = MIN_OBJ; t <= MAX_OBJ; ++t) {
                if (there(t, R_ARCHED)) {
                    move(t, R_ARCHED_RUBBLE);
                }
            }
            return true;
        } else if (is_outside(loc)) {
            puts("The blast of your horn echos throughout hill and dale.");
        } else {
            puts("The chamber reverberates to the blast of the horn." SOFT_NL
                 "(Satchmo you ain't!)");
            maybe_wake_the_wumpus(loc);
        }
    } else if (verb == PLAY && obj == LYRE) {
        if (there(DOG, loc) && !objs(DOG).prop) {
            puts("The air fills with beautiful music.  The dog gradually becomes" SOFT_NL
                 "less fierce, and after a short while he lies down by the side of" SOFT_NL
                 "the cavern and falls into a deep sleep.");
            objs(DOG).prop = 1;  /* sleeping */
            remobilize_axe(3);
        } else {
            puts("The air is filled with beautiful music.");
        }
    } else {
        puts("I'm game.  Would you care to explain how?");
    }
    return false;
}

void attempt_pick(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    assert(prep == PREP_UP || prep == NOTHING);
    if (obj == NOTHING) {
        obj = iobj;
        iobj = NOTHING;
    }
    if (prep == PREP_UP || obj == FLOWERS || obj == MUSHROOMS) {
        /* PICK FLOWERS works, but PICK LAMP doesn't.
         * Notice that PICK LAMP UP KEYS will also be
         * quietly redirected to TAKE LAMP. */
        attempt_take(loc, TAKE, obj, NOTHING, NOTHING);
    } else {
        confuz();
    }
}

void attempt_put(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    /* PUT DOWN LAMP ==> DROP LAMP
     * PUT WATER IN BOTTLE ==> FILL BOTTLE
     * PUT ON CLOAK ==> WEAR CLOAK
     * PUT CLOAK ON LAMP ==> DROP CLOAK (no matter where the lamp is)
     * PUT LAMP IN SACK ==> insert()
     */
    if (obj == NOTHING) {
        obj = iobj;
        iobj = NOTHING;
        strcpy(otxt[objx], iotxt[iobx]);
    }

    if (prep == NOTHING) {
        assert(iobj == NOTHING);
        indent_appropriately();
        printf("Where do you want to put the %s?\n", otxt[objx]);
    } else if (prep == INTO) {
        attempt_insert_into(loc, obj, iobj);
    } else if (prep == ONTO) {
        /* Long's logic here is buggy; I've rewritten it. */
        if (iobj == NOTHING) {
            attempt_wear(loc, obj);
        } else {
            /* PUT CLOAK ON LAMP ==> DROP CLOAK */
            attempt_drop(loc, DROP, obj, NOTHING, NOTHING);
        }
    } else {
        assert(prep == PREP_DOWN);
        if (iobj == NOTHING) {
            attempt_drop(loc, DROP, obj, NOTHING, NOTHING);
        } else {
            noway();
        }
    }
}

void attempt_diagnose(void)
{
    if (health < 100) {
        /* Long has "You're". */
        printf("Your health rating is %d out of a possible 100.\n", health);
    }

    if (health >= 95) {
        if (pct(50)) {
            puts("You are fit as a fiddle.");
        } else {
            puts("You are in perfect health.");
        }
    } else if (health > 80) {
        puts("You are a bit off top form, but nothing to worry about.");
    } else if (health > 60) {
        puts("You are weaker than usual.  Better avoid fights.");
    } else if (health > 40) {
        puts("You really ought to take a break.  You're in tough shape.");
    } else if (health > 20) {
        puts("You are on the edge of collapse.  Lots of sun and fresh air will" SOFT_NL
             "speed your recovery.");
    } else {
        assert(health > 0);
        puts("Your strength is nearly gone.  Only a miracle can save you now.");
    }
}

void attempt_combo(Location loc)
{
    if (!there(SAFE, loc)) {
        confuz();
        return;
    }
    /* Long doesn't check that the safe door is closed first.
     * TODO: maybe fix this. */
    int number = atoi(vtxt[vrbx]);  /* TODO: don't use atoi on the Z-machine */
    switch (number) {
        case 7: number = 0; break;
        case 22: number = 1; break;
        case 34: number = 2; break;
        default: assert(false);
    }
    if (combo == -number) {
        combo = number+1;
        puts("\"Click.\"");
        if (combo == 3) {
            puts("The safe door smoothly swings open.");
            objs(SAFE).flags |= F_OPEN;
            objs(SAFE).flags &= ~F_LOCKED;
            objs(SAFE).prop = 1;
            spot_treasure(BOOK);
        }
    } else if (combo != 0) {
        /* Notice that "7-22-7-22-34" will not unlock the safe. */
        puts("Maybe if you rubbed your fingertips with sandpaper....");
    } else {
        puts("Nothing happens.");
    }
}

void attempt_feefie(Location loc)
{
    int number;
    if (streq(vtxt[vrbx], "fee")) {
        number = 0;
    } else if (streq(vtxt[vrbx], "fie")) {
        number = 1;
    } else if (streq(vtxt[vrbx], "foe")) {
        number = 2;
    } else if (streq(vtxt[vrbx], "foo")) {
        number = 3;
    } else {
        assert(streq(vtxt[vrbx], "fum"));
        number = 4;
    }

    if (foobar == -number) {
        foobar = number+1;
        if (foobar == 4) {
            foobar = 0;
            if (there(EGGS, R_GIANT) || (toting(EGGS) && loc == R_GIANT)) {
                puts("Nothing happens.");
                return;
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
        } else {
            puts(ok);
        }
    } else if (foobar != 0) {
        puts("What's the matter, can't you read?  Now you'd best start over.");
    } else {
        puts("Nothing happens.");
    }
}

void attempt_sweep(Location loc)
{
    /* Long uses PROP(CARVNG) for this purpose. */
    static bool found_carving = false;
    if (!at_hand(BROOM, loc)) {
        puts("How?");
    } else if (loc != R_DUSTY || found_carving) {
        puts("Enough dusting, already!  You're making me sneeze.");
    } else {
        puts("Brushing the dust from one of the larger rocks reveals some carved" SOFT_NL
             "characters.");
        puts("In the rock is carved the message \"7-22-34\".");
        found_carving = true;
    }
}

void attempt_get(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    if (prep == NOTHING || prep == FROM) {
        attempt_take(loc, TAKE, obj, prep, iobj);
    } else if (obj != NOTHING) {
        confuz();
    } else {
        /* Other prepositions are ignored, which
         * makes GET IN BOAT act like TAKE BOAT.
         * Long comments: "NEEDS WORK". In
         * particular, GET OUT is not a synonym
         * for EXIT. */
        attempt_get(loc, iobj, NOTHING, NOTHING);
    }
}

void attempt_read(Location loc, ObjectWord obj)
{
    if (blind_at(loc)) {
        /* I like this message a lot. Long cleverly made it work for both
         * pitch darkness and the blindingly bright Crystal Palace. */
        puts("I certainly can't read in this light.");
    } else if (obj == BOOK || obj == REPO_BOOK) {
        history_of_adventure();
    } else if (obj == BILLBOARD) {
        puts("The billboard reads:\n"
             "\"Visit Beautiful Colossal Cave.  Open Year Around.  Fun for" SOFT_NL
             "the entire family, or at least for those who survive.\"" SOFT_NL
             "Below the headline is an impossibly complicated map showing how" SOFT_NL
             "to find Colossal Cave.  Not that it matters, because all the" SOFT_NL
             "directions are written in Elvish.");
    } else if (obj == CARVING) {
        /* Notice that you don't need to DUST first in order to read the
         * carving, if you already know it's there. */
        puts("In the rock is carved the message \"7-22-34\".");
    } else if (obj == MAG) {
        puts("I'm afraid the magazine is written in dwarvish.");
    } else if (obj == MESSAGE) {
        puts("\"This is not the maze where the pirate leaves his treasure chest.\"");
    } else if (obj == POSTER) {
        puts("The poster has a picture of a thin man with a long white beard." SOFT_NL
             "He is wearing a high pointed cap embroidered with strange symbols," SOFT_NL
             "and he is pointing a finger at you.  Below the picture are the words:" SOFT_NL
             "\"I want you!--To report all good ideas for extensions to this game" SOFT_NL
             "to me without delay.  Remember: ask not what ADVENTURE can do to" SOFT_NL
             "you; ask what you can do for ADVENTURE.\"\n"
             "-                       *  *  *                              -\n"
             "\"A public service of the John Dillinger Died for You Society.\"");
    } else if (obj == TABLET) {
        puts("\"Congratulations on bringing light into the dark-room!\"");
    } else if (obj == OYSTER && holding(OYSTER) && closed) {
        if (hints[1].given) {
            puts("It says the same thing it did before.");
        } else {
            offer(1);
        }
    } else {
        confuz();
    }
}

bool is_lighted_in_absentia(Location loc)
{
    if (loc < 0) {
        ObjectWord container = -loc;
        assert(MIN_OBJ <= container && container <= MAX_OBJ);
        assert(is_vessel(container));
    } else {
        /* Notice that you can leave the lit lamp in the Crystal Palace
         * and it won't blind your sapphire-vision. */
        if (places[loc].flags & F_LIGHTED)
            return true;
    }
    return (objs(LAMP).place == loc && objs(LAMP).prop);
}

void lookin(ObjectWord container)
{
    /* List contents if container is open or transparent.
     * Used by INVENTORY and by LOOK IN SACK. */
    assert(word_class(container) == WordClass_Object);
    if (!is_vessel(container)) return;
    if (is_opaque(container) && !is_ajar(container)) return;
    if (objs(container).contents == NULL) return;
    puts("It contains:");
    for (struct ObjectData *p = objs(container).contents; p != NULL; p = p->link) {
        indent_appropriately();
	printf("     %s\n", p->name);
    }
}

int attempt_look(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    /* Long doesn't actually verify that the preposition matches the indirect
     * object here, but it is somewhat constrained by the parser. */
    assert(prep == NOTHING || prep == INTO || prep == AT);
    if (obj == NOTHING && iobj == NOTHING) {
        /* Look around. */
        if (look_count < 3) {
            puts("Sorry, but I am not allowed to give more detail.  I will repeat the" SOFT_NL
                 "long description of your location.");
            ++look_count;
        }
        places[loc].visits = 0;
        return 'l';  /* goto commence */
    } else if (obj != NOTHING) {
        /* LOOK LAMP IN SACK */
        confuz();
    } else {
        assert(iobj != NOTHING);
        assert(word_class(iobj) == WordClass_Object);
        if (is_vessel(iobj)) {
            /* Look into something (a container). */
            if (is_opaque(iobj) && !is_ajar(iobj)) {
                puts("It's not open.");
            } else if (objs(iobj).contents == NULL) {
                puts("There's nothing inside.");
            } else {
                indent_appropriately();
                lookin(iobj);
            }
        } else if (is_readable(iobj)) {
            /* Look at something. If written, read it. */
            attempt_read(loc, iobj);
        } else if (iobj == BALL) {
            /* Looking into the crystal ball. */
            if (!is_well_inside(loc) || at_hand(SAPPHIRE, loc)) {
                puts("Nothing happens.");
            } else {
                puts("You feel rather disembodied, as if you were suddenly somewhere" SOFT_NL
                     "else entirely.\n");
                Location sloc = objs(SAPPHIRE).place;
                if (is_lighted_in_absentia(sloc)) {
                    if (sloc > R_LIMBO) {
                        assert(sloc <= MAX_LOC);
                        assert(places[sloc].long_desc != NULL);
                        puts(places[sloc].long_desc);
                        static bool have_seen_elf = false;
                        if (sloc == R_BAY && !have_seen_elf) {
                            puts("A large, stately elf walks up the rise, says the word" SOFT_NL
                                 "\"Saint-Michel\", and is instantly transported to the castle.");
                            have_seen_elf = true;
                        }
                    } else if (sloc == -SACK) {
                        /* Here Long drops the ball (heh). If you put the
                         * ball and the lamp both in the sack, Long's code
                         * will print LTEXT(-SACK), which is garbage. I'm
                         * taking the liberty of fixing the bug. */
                        puts("You are in a sack.");
                    } else if (sloc == -SAFE) {
                        /* Ditto for the safe. */
                        if (is_ajar(SAFE)) {
                            puts(places[R_HOUSE].long_desc);
                        } else {
                            puts("You are in a small cubical room with no exits.");
                        }
                    } else if (sloc == -CHEST) {
                        /* Ditto for the chest. */
                        puts("You are in a small wooden room with no exits.");
                    } else if (sloc == R_LIMBO) {
                        /* You can get here in at least two ways:
                         * if you give the sapphire to the troll, or
                         * if you fall into Lost River carrying it.
                         * I've decided to add a couple of Easter eggs
                         * here, just for fun. */
                        if (troll_has_sapphire) {
                            puts("You are in a wide niche burrowed into the sheer rock face of a" SOFT_NL
                                 "canyon and open to the sky.  From here you can see a beautiful" SOFT_NL
                                 "panoramic view of Lost River Falls.  At the back of the niche," SOFT_NL
                                 "a narrow chimney leads upward out of sight.  Marks in the dust" SOFT_NL
                                 "indicate that someone has been here recently.");
                            if (there(TROLL, R_LIMBO)) {
                                puts("A burly troll putters about in one corner of the room.");
                            } else if (objs(TROLL).prop == 2) {
                                puts("The troll is hunched shivering in the corner of the room," SOFT_NL
                                     "staring out over the falls with tear-stained empty eyes.");
                            }
                        } else if (river_has_sapphire) {
                            puts("You sense that you are in a dark place, murky greenish shadows" SOFT_NL
                                 "swirling around you in the gloom. The only thing in sight appears" SOFT_NL
                                 "to be a companion to the crystal ball which holds your gaze.  It" SOFT_NL
                                 "seems to be searching the murk for something to show you, but all" SOFT_NL
                                 "it can see is itself: a dim bluish star half-buried in the silt.");
                        } else {
                            goto dark_place;
                        }
                    } else {
                        assert(false);
                    }
                } else {
                  dark_place:
                    puts("You sense that you are in a dark place. The only thing in sight" SOFT_NL
                         "appears to be a companion to the crystal ball which holds your" SOFT_NL
                         "gaze. It seems to be searching the gloom for something to" SOFT_NL
                         "show you, but all it can see is itself: a brilliant blue" SOFT_NL
                         "six-pointed star suspended in space.");
                }
                puts("\nYour gaze withdraws from the crystal ball, and you are now back" SOFT_NL
                     "in your normal senses.");
            }
        } else {
            /* LOOK AT anything else redirects to LOOK AROUND */
            return attempt_look(loc, NOTHING, NOTHING, NOTHING);
        }
    }
    return 0;
}

bool find_an_openable_object(Location loc, ActionWord verb, int *obj)  /* line 10400 in Long */
{
    int objects_found = 0;
    assert(*obj == NOTHING);
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (!here(t, loc)) continue;
        if (!is_hinged(t)) continue;
        *obj = t;
        ++objects_found;
    }
    if (objects_found == 1) {
        assert(*obj != NOTHING);
        return false;  /* got an object, but didn't handle it */
    } else if ((verb == LOCK || verb == UNLOCK) && *obj == NOTHING) {
        puts("There is nothing here with a lock!");
        return true;  /* handled */
    } else {
        *obj = NOTHING;
        return false;  /* not yet handled */
    }
}

ObjectWord find_an_edible_object(Location loc)  /* line 11400 in Long */
{
    int objects_found = 0;
    ObjectWord obj = NOTHING;
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (!here(t, loc)) continue;
        if (!is_edible(t)) continue;
        obj = t;
        ++objects_found;
    }
    if (objects_found == 1) return obj;
    return NOTHING;
}

void attempt_lock(Location loc, ObjectWord obj)
{
#define close_and_lock(t) objs(t).flags |= F_LOCKED; objs(t).flags &= ~F_OPEN
    if (!is_hinged(obj)) {
        indent_appropriately();
        printf("I don't know how to lock or unlock the %s.\n", otxt[objx]);
    } else if (!at_hand(KEYS, loc) && !at_hand(TINY_KEY, loc) && obj != SAFE) {
        puts("You have no keys!");
    } else if (objs(obj).flags & F_LOCKED) {
        puts("It was already locked.");
    } else if (!has_lock(obj)) {
        puts("It has no lock.");
    } else if ((obj == CHAIN) && at_hand(KEYS, loc)) {
        if (loc != R_BARR) {
            puts("There is nothing here to which the chain can be locked.");
        } else {
            remove_from_containers(CHAIN);
            if (holding(CHAIN)) drop(CHAIN, loc);
            objs(CHAIN).prop = 2;
            close_and_lock(CHAIN);
            mobilize(CHAIN);
            puts("The chain is now locked.");
        }
    } else if ((obj == CHEST) && at_hand(KEYS, loc)) {
        puts("The chest is now locked.");
        close_and_lock(CHEST);
    } else if (obj == TINY_DOOR || obj == HUGE_DOOR) {
        if (!toting(TINY_KEY)) {
            assert(at_hand(KEYS, loc));
            puts("Your keys are all too large for the lock.");
        } else {
            indent_appropriately();
            printf("The %s door is now locked.\n",
                   (obj == TINY_DOOR ? "tiny" : "wrought-iron"));
            objs(TINY_DOOR).prop = 0;
            close_and_lock(TINY_DOOR);
            objs(HUGE_DOOR).prop = 0;
            close_and_lock(HUGE_DOOR);
        }
    } else if ((obj == GRATE) && at_hand(KEYS, loc)) {
        puts("The grate is now locked.");
        objs(GRATE).prop = 0;
        close_and_lock(GRATE);
    } else if (obj == SAFE) {
        puts("The safe's door clicks shut.");
        objs(SAFE).prop = 0;
        close_and_lock(SAFE);
    } else {
        puts("You don't have the right key.");
    }
#undef close_and_lock
}

void attempt_unlock(Location loc, ObjectWord obj, ObjectWord iobj)
{
#define unlock_and_open(t) objs(t).flags &= ~F_LOCKED; objs(t).flags |= F_OPEN
    if (obj == KEYS || obj == TINY_KEY) {
        puts("You can't unlock the keys.");
    } else if (!is_hinged(obj)) {
        indent_appropriately();
        printf("I don't know how to lock or unlock the %s.\n", otxt[objx]);
    } else if (!at_hand(KEYS, loc) && !at_hand(TINY_KEY, loc) && obj != SAFE) {
        puts("You have no keys!");
    } else if (obj == SAFE) {
        if (iobj == KEYS || iobj == TINY_KEY) {
            puts("This is a combination safe.  The keys won't help.");
        } else {
            puts("How?");
        }
    } else if (!has_lock(obj)) {
        puts("It has no lock.");
    } else if (!(objs(obj).flags & F_LOCKED)) {
        puts("It was already unlocked.");
    } else if ((obj == CHAIN) && at_hand(KEYS, loc)) {
        if (objs(BEAR).prop == 0) {
            puts("There is no way to get past the bear to unlock the chain, which is" SOFT_NL
                 "probably just as well.");
        } else {
            puts("The chain is now unlocked.");
            objs(CHAIN).prop = 0;
            mobilize(CHAIN);
            if (objs(BEAR).prop == 1) {
                objs(BEAR).prop = 2;
                mobilize(BEAR);
            }
            unlock_and_open(CHAIN);
        }
    } else if ((obj == CHEST) && at_hand(KEYS, loc)) {
        puts("The chest is now unlocked.");
        unlock_and_open(CHEST);
    } else if (obj == TINY_DOOR || obj == HUGE_DOOR) {
        /* Long comments: "The damn thing is really at four places..." */
        if (!at_hand(TINY_KEY, loc)) {
            assert(at_hand(KEYS, loc));
            puts("Your keys are all too large for the lock.");
        } else if (cave_is_closing()) {
            panic_at_closing_time();
        } else {
            indent_appropriately();
            printf("The %s door is now unlocked.\n",
                   (obj == TINY_DOOR ? "tiny" : "wrought-iron"));
            objs(TINY_DOOR).prop = 1;
            unlock_and_open(TINY_DOOR);
            objs(HUGE_DOOR).prop = 1;
            unlock_and_open(HUGE_DOOR);
        }
    } else if ((obj == GRATE) && at_hand(KEYS, loc)) {
        if (cave_is_closing()) {
            panic_at_closing_time();
        } else {
            puts("The grate is now unlocked.");
            objs(GRATE).prop = 1;
            unlock_and_open(GRATE);
        }
    } else {
        puts("You don't have the right key.");
    }
#undef unlock_and_open
}

void attempt_open(Location loc, ObjectWord obj, ObjectWord iobj)
{
    /* Long forgets about the smashed bottle. I've fixed it. */
    if (!is_hinged(obj) || (obj == BOTTLE && objs(BOTTLE).prop == 3)) {
        noway();
    } else if (obj == BOOTH_DOOR && objs(BOOTH_DOOR).prop == 1) {
        puts("The gnome firmly blocks the door of the booth.  You can't enter.");
    } else if (is_ajar(obj)) {
        puts("It's already open.");
    } else if (has_lock(obj) || iobj == KEYS || iobj == TINY_KEY) {
        attempt_unlock(loc, obj, iobj);
    } else if (obj == RUSTY_DOOR && (objs(RUSTY_DOOR).flags & F_LOCKED)) {
        puts("The door is extremely rusty and refuses to open.");
    } else if (objs(obj).flags & F_LOCKED) {
        puts("It's locked.");
    } else if (obj == CLAM || obj == OYSTER) {
        bool is_clam = (obj == CLAM);
        const char *clam_or_oyster = (is_clam ? "clam" : "oyster");
        if (iobj != NOTHING && iobj != TRIDENT) {
            indent_appropriately();
            printf("That's not strong enough to open the %s.\n", clam_or_oyster);
        } else if (!at_hand(TRIDENT, loc)) {
            indent_appropriately();
            printf("You don't have anything strong enough to open the %s.\n", clam_or_oyster);
        } else if (holding(obj)) {
            indent_appropriately();
            printf("I advise you to put down the %s before opening it.  >%s!<\n",
                   clam_or_oyster, (is_clam ? "Strain" : "Wrench"));
        } else if (obj == OYSTER) {
            puts("The oyster creaks open, revealing nothing but oyster inside.  It" SOFT_NL
                 "promptly snaps shut again.");
        } else {
            /* Success! */
            puts("A glistening pearl falls out of the clam and rolls away.  Goodness," SOFT_NL
                 "this must really be an oyster.  (I never was very good at identifying" SOFT_NL
                 "bivalves.)  Whatever it is, it has now snapped shut again.");
            destroy(CLAM);
            drop(OYSTER, loc);
            drop(PEARL, R_SAC);
        }
    } else {
        puts(ok);
        objs(obj).flags |= F_OPEN;
    }
}

void attempt_close(Location loc, ObjectWord obj)
{
    /* Long forgets about the smashed bottle. I've fixed it. */
    if (!is_hinged(obj) || (obj == BOTTLE && objs(BOTTLE).prop == 3)) {
        noway();
    } else if (!is_ajar(obj)) {
        puts("It's already closed.");
    } else if (has_lock(obj)) {
        /* CLOSE TINY DOOR WITH POLE, for example */
        attempt_lock(loc, obj);
    } else {
        /* Long comments "The following can be closed without keys:"
         * and lists RUSTY_DOOR, BOOTH_DOOR, BOTTLE, CASK, and CAGE.
         * He forgets SACK, BOOK, and CANISTER, implying perhaps that
         * these were relatively late additions to the game. */
        puts(ok);
        objs(obj).flags &= ~F_OPEN;
    }
}

bool attempt_light(Location loc)
{
    if (!at_hand(LAMP, loc)) {
        puts("You have no source of light.");
    } else if (lamp_limit <= 0) {
        puts("Your lamp has run out of power.");
    } else if (objs(LAMP).prop) {
        puts("Your lamp is already on.");
    } else {
        objs(LAMP).prop = 1;  /* lighted */
        if (loc == R_CRYSTAL) {
            puts("Your lamp is now on, but the glare from the walls is absolutely" SOFT_NL
                 "blinding.  If you proceed you are likely to fall into a pit.");
        } else {
            puts("Your lamp is now on.");
            return true;
        }
    }
    return false;
}

void attempt_extinguish(Location loc)
{
    if (!at_hand(LAMP, loc)) {
        puts("You have no source of light.");
    } else if (!objs(LAMP).prop) {
        puts("Your lamp is already off.");
    } else {
        objs(LAMP).prop = 0;  /* extinguished */
        puts("Your lamp is now off.");
        if (now_in_darkness(loc)) {
            puts("It is now pitch dark.  If you proceed you will likely fall into a pit.");
        }
    }
}

void attempt_wave(Location oldloc, Location loc, ObjectWord obj, ObjectWord iobj)
{
    if (obj == ROD && !holding(ROD) && holding(ROD2)) {
        obj = ROD2;  /* TODO: combine with other places this happens */
    }
    const bool at_fissure = (iobj == NOTHING || iobj == FISSURE);
    if (!holding(obj)) {
        puts("You aren't carrying it!");
    } else if (obj == ROD && is_at_loc(FISSURE, loc) && at_fissure && !cave_is_closing()) {
        if (!objs(FISSURE).prop) {
            puts("A crystal bridge now spans the fissure.");
            objs(FISSURE).prop = 1;
        } else {
            puts("The crystal bridge has vanished!");
            objs(FISSURE).prop = 0;
            if (being_chased != 0) {
                /* The demise of the Wumpus.
                 * Long assumes that the Wumpus is always one step behind the
                 * player. This ain't necessarily so, if we modeled the world
                 * better; he has enough time to navigate to Y2 and use PLUGH
                 * to teleport aboveground. TODO: maybe reimplement this?
                 * That would be a huge gameplay change, though. */
                if ((loc + oldloc) == (R_EFISS + R_WFISS)) {
                    puts("As the bridge disappears, the Wumpus scrambles frantically to reach" SOFT_NL
                         "your side of the fissure.  He misses by inches, and with a horrible" SOFT_NL
                         "shriek plunges to his death in the depths of the fissure!");
                    being_chased = 0;
                    assert(there(RING, R_LIMBO));
                    drop(RING, R_WLOST);
                    move(WUMPUS, R_WLOST);
                    objs(WUMPUS).prop = 6;
                    /* The Wumpus is "between you and the axe" even during
                     * the chase --- and if you die, the axe remains trapped
                     * when the Wumpus is reset. When the Wumpus dies, we
                     * release the axe. */
                    remobilize_axe(2);
                }
            }
        }
    } else {
        puts("Nothing happens.");
    }
}

int attempt_kill(Location loc, ObjectWord obj, ObjectWord iobj)
{
    if (obj == NOTHING) {
        /* TODO: factor out similar code from here and attempt_toss */
        if (dwarf_at(loc)) {
            obj = DWARF;
        } else {
            int k = 0;
            if (there(SNAKE, loc)) { ++k; obj = SNAKE; }
            if (is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) { ++k; obj = DRAGON; }
            if (is_at_loc(TROLL, loc)) { ++k; obj = TROLL; }
            if (there(GNOME, loc)) { ++k; obj = GNOME; }
            if (there(BEAR, loc) && objs(BEAR).prop == 0) { ++k; obj = BEAR; }
            if (there(WUMPUS, loc) && objs(WUMPUS).prop == 0) { ++k; obj = WUMPUS; }
            /* but *not* BEES or DOG; oversight on Long's part */
            if (k == 0) {
                /* TODO: THROW AXE shouldn't kill the bird. Verify that it doesn't. */
                if (here(BIRD, loc)) { ++k; obj = BIRD; }
                if (here(CLAM, loc) || here(OYSTER, loc)) { ++k; obj = CLAM; }
            }
            if (k > 1) return 'a';  /* goto act_on_what */
        }
    }
    if (obj == BIRD) {
        if (closed) {
            puts("Oh, leave the poor unhappy bird alone.");
        } else {
            puts("The little bird is now dead.  Its body disappears.");
            destroy(BIRD);
            if (there(SNAKE, R_HMK)) ++lost_treasures; /* the jewelry */
        }
    } else if (obj == DWARF) {
        /* He is attacking a dwarf. If using something other than axe or
         * sword, goodbye Charlie. If using nothing, don't let him. If using
         * axe or sword, the following odds prevail:
         *     .25 -- hero kills dwarf
         *     .75 -- hero misses
         *         .25 -- hero gets knife in ribs. dies.
         *         .75 -- hero can't make a clean thrust
         *             .36 -- standoff
         *             .64 -- dwarf slashes
         *                 .61 -- dwarf misses!
         *                 .39 -- dwarf kills hero
         * Adventurer has 1/3 chance of getting nailed, 1/4 chance of
         * nailing dwarf. All by way of encouraging him to throw the axe. */
        if (closed) {
            dwarves_upset();
        } else if (!dwarf_at(loc)) {
            /* KILL DWARF, DWARF, DWARF WITH AXE */
            puts("Don't be ridiculous!");
        } else if (iobj == NOTHING) {
            puts("With what?  Your bare hands?");
        } else if ((iobj != AXE && iobj != SWORD) || (ran(16) < 3)) {
            puts("As you move in for the kill, the dwarf neatly slips a knife"
                 "between your ribs.");
            return 'x';  /* dead */
        } else if (pct(25)) {
            kill_a_dwarf(loc);
        } else {
            puts("You can't get close enough for a clean thrust.");
            if (pct(64)) {
                puts("As you approach, the dwarf slashes out with his knife!");
                if (pct(61)) {
                    puts("It misses!");
                } else {
                    puts("It gets you!");
                    return 'x';  /* dead */
                }
            }
            /* Here Long clears the input buffer; not sure why. TODO. */
        }
    } else if (obj == NOTHING) {
        puts("There is nothing here to attack.");
    } else if (obj == CLAM || obj == OYSTER) {
        puts("The shell is very strong and is impervious to attack.");
    } else if (obj == DOG && objs(DOG).prop) {
        puts("That wouldn't be wise.  It is best to let sleeping dogs lie.");
    } else if (obj == SNAKE) {
        puts("Attacking the snake both doesn't work and is very dangerous.");
    } else if ((obj == DRAGON && objs(DRAGON).prop) ||
               (obj == WUMPUS && objs(WUMPUS).prop == 6) ||
               (obj == BEAR && objs(BEAR).prop == 3)) {
        puts("For crying out loud, the poor thing is already dead!");
    } else if (obj == TROLL) {
        puts("Trolls are close relatives with the rocks and have skin as tough as" SOFT_NL
             "that of a rhinoceros.  The troll fends off your blows effortlessly.");
    } else if (obj == BEAR) {
        assert(objs(BEAR).prop != 3);  /* handled above */
        if (objs(BEAR).prop == 0) {
            if (iobj == NOTHING) {
                puts("With what?  Your bare hands?  Against *HIS* bear hands??");
            } else {
                puts("Don't be ridiculous!");
            }
        } else {
            puts("The bear is confused; he only wants to be your friend.");
        }
    } else if (obj == GNOME) {
        puts("You can't get at him.  He is inside the phone booth.");
    } else if (iobj == AXE &&
               !(obj == DOG || obj == WUMPUS || obj == DRAGON || obj == TROLL)) {
        /* KILL BEES WITH AXE becomes THROW AXE AT BEES */
        return attempt_toss(loc, AXE, AT, obj);
    } else if (obj == DRAGON) {
        puts("With what?  Your bare hands?");
        if (with_your_bare_hands()) {
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
            return 'd';  /* move to R_SCAN2 */
        }
    } else {
        puts("Don't be ridiculous!");
    }
    return 0;
}

int attempt_pour(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    assert(obj != NOTHING);

    const bool original_obj_was_liquid = is_liquid(obj);
    if (obj == BOTTLE || obj == CASK) {
        prep = FROM;
        iobj = obj;
        obj = liquid_contents(obj);
        if (obj == NOTHING) {
            puts("It's empty.");
            return 0;
        }
    } else if (!original_obj_was_liquid) {
        puts("You can't pour that.");
        return 0;
    }

    if (prep == FROM) {
        /* Long forgets this case, and will prefer to empty the cask even
         * if the player has explicitly told us to POUR WATER FROM BOTTLE.
         * That has the advantage of not needing to handle the case of
         * POUR WATER FROM VASE; I have to handle it specially here. */
        if (iobj == VASE || iobj == GRAIL) {
            puts("It's empty.");
            return 0;
        }
        assert(iobj == BOTTLE || iobj == CASK);
    } else {
        assert(prep == NOTHING || prep == ONTO);
        /* Here we deliberately forget about the player's original iobj.
         * If you POUR WATER ON CLOVER while at R_WPIT, the game will
         * act as if you typed POUR WATER; i.e., the plant will grow. */
        prep = FROM;
        if (holding(BOTTLE) && liquid_contents(BOTTLE) == obj) {
            iobj = BOTTLE;
        } else if (holding(CASK) && liquid_contents(CASK) == obj) {
            iobj = CASK;
        } else {
            puts("You aren't carrying it!");
            return 0;
        }
    }

    assert(is_liquid(obj));
    assert(prep == FROM);
    assert(iobj == BOTTLE || iobj == CASK);

    if (!is_ajar(iobj)) {
        /* More annoying than Witt's End...
         * Long gives "You can't get at it" even when the player's original
         * command was POUR BOTTLE or WATER PLANT. I'm taking the liberty
         * of fixing this bug. */
        if (original_obj_was_liquid) {
            puts("You can't get at it.");
        } else {
            indent_appropriately();
            printf("The %s is closed.\n", iobj==BOTTLE ? "bottle" : "cask");
        }
        return 0;
    }

    if (iobj == CASK) {
        /* WATER_IN_CASK is WATER+1, and so on. */
        assert(objs(obj+1).place == -CASK);
        destroy(obj+1);
    } else {
        assert(objs(obj).place == -BOTTLE);
        destroy(obj);
    }
    objs(iobj).prop = 1;  /* empty */

    if (there(RUSTY_DOOR, loc)) {
        if (obj == OIL) {
            objs(RUSTY_DOOR).prop = 1;
            objs(RUSTY_DOOR).flags &= ~F_LOCKED;
            objs(RUSTY_DOOR).flags |= F_OPEN;
            puts("The oil has freed up the hinges so that the door will now move," SOFT_NL
                 "although it requires some effort.");
        } else {
            objs(RUSTY_DOOR).prop = 0;
            /* Long doesn't close the door here, so it will look "open" even
             * though the player's movement is blocked by the travel table. */
            puts("The hinges are quite thoroughly rusted now and won't budge.");
        }
    } else if (there(SWORD, loc) && objs(SWORD).prop != 0) {
        /* The sword has the following prop values, from 0 to 4:
         * "normal", "stuck in anvil", "broken", "clean in anvil",
         * "oily in anvil". */
        if (objs(SWORD).prop == 4) {
            /* Once oiled, the sword cannot be cleaned by any means.
             * Long has "now"; I changed it to "still" for clarity. */
            puts("The sword is still covered with oil.");
        } else if (objs(SWORD).prop == 2) {
            /* Long forgets this case. OIL SWORD or WATER SWORD will
             * resurrect it from the shards, but not re-mobilize it,
             * so you won't be able to yank it out anymore. I've
             * fixed this bug with a new message. */
            indent_appropriately();
            printf("The shards are now %s.\n",
                   (obj == OIL) ? "covered with oil" : "very clean");
        } else {
            if (obj == OIL) {
                puts("The sword is now covered with oil.");
                objs(SWORD).prop = 4;
            } else {
                puts("The sword is now very clean.");
                objs(SWORD).prop = 3;
            }
        }
    } else if (there(PLANT, loc)) {
        if (obj != WATER) {
            /* Long has "shakes dry its leaves", but we might as well be specific. */
            indent_appropriately();
            printf("The plant indignantly shakes the %s off its leaves and asks, \"Water?\"\n",
                   (obj == WINE) ? "wine" : "oil");
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
            return 'm';  /* look around */
        }
    } else if (iobj == BOTTLE) {
        puts("Your bottle is empty and the ground is wet.");
    } else {
        assert(iobj == CASK);
        puts("Your cask is empty and the ground is soaked.");
    }
    return 0;
}

Location attempt_eat(Location loc, ObjectWord obj)
{
    switch (obj) {
        case HONEY:
            /* Long increments lost_treasures (his TALLY2) here if you eat
             * the honey. The effect of this is to zap your lamp a bit early,
             * since it's still possible to *see* the golden chain, even if
             * you can no longer *get* it. */
            ++lost_treasures;
            /*FALLTHRU*/
        case FOOD:
            destroy(FOOD);
            puts("Thank you, it was delicious!");
            break;
        case BIRD: case SNAKE:
        case CLAM: case OYSTER:
        case FLOWERS:
            puts("Yeeeecchhh!!");
            break;
        case DWARF: case DRAGON: case TROLL: case DOG:
        case WUMPUS: case BEAR: case GNOME:
            puts("You've got to be kidding!");
            break;
        case MUSHROOMS:
            /* I think Long's logic here is: If you waste the mushrooms,
             * you'll never get through the elfin door, and therefore never
             * get the boat (unless you use the magic word "GROTTO", which
             * is where his logic falls down); therefore you'll never get
             * the honey, therefore never get the chain (although you might
             * still see it, which is a second logical hole). */
            if (loc != R_TINYDOOR) ++lost_treasures;
            destroy(MUSHROOMS);
            if (!is_at_loc(TINY_DOOR, loc)) {
                puts("You thought maybe these were peyote??  You feel a little dizzy," SOFT_NL
                     "but nothing happens.");
            } else {
                /* "GET PEYOTE; N; GROTTO; EAT PEYOTE" will accidentally
                 * teleport you through the wall. TODO: fix this whole
                 * passage. */
                puts("You are growing taller, expanding like a telescope!  Just before" SOFT_NL
                     "your head strikes the top of the chamber, the mysterious process" SOFT_NL
                     "stops as suddenly as it began.");
                /* Move all the objects at R_TINYDOOR to R_CRAMPED. */
                for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
                    if ((objs(t).base == NULL) && there(t, R_TINYDOOR))
                        move(t, R_CRAMPED);
                }
                return R_CRAMPED;
            }
            break;
        case CAKES:
            destroy(CAKES);
            /* In Long's version, if you vanish the tiny key (e.g. by container
             * mischief) before eating the cakes, you can get the message
             * "You thought maybe these were peyote??" here. However, this
             * is clearly a bug, so I haven't preserved it. */
            /* Long has "accordian". */
            puts("You are closing up like an accordion....shrinking..shrinking.  You" SOFT_NL
                 "are now your normal size.");
            /* Move all the objects at R_CRAMPED to R_TINYKEY,
             * except for the key on the shelf. If you missed the key,
             * Long adds another "lost" treasure, presumably the chain.
             * Which means it's possible to "lose" the chain four times
             * in a single game! */
            for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
                if (t == TINY_KEY && objs(TINY_KEY).prop) {
                    assert(there(t, R_TINYDOOR));
                    ++lost_treasures;
                } else if ((objs(t).base == NULL) && there(t, R_TINYDOOR)) {
                    move(t, R_TINYDOOR);
                }
            }
            return R_TINYDOOR;
        default:
            puts("Don't be ridiculous!");
            break;
    }

    return loc;
}

Location attempt_drink(Location loc, ObjectWord obj, ObjectWord iobj)
{
    /* In Long's version, you can DRINK FROM [closed] BOTTLE, but you
     * can't DRINK WATER FROM [closed] BOTTLE; "you can't get at it."
     * Long handles DRINK OIL FROM LAMP first by checking if the OIL
     * is accessible, and only *then* whether it's actually in the lamp.
     * Long also has some bugs that cause DRINK OIL and/or DRINK in
     * the east pit to be accidentally handled as DRINK WINE.
     * I've refactored to fix all these bugs. */
    if (obj == NOTHING) {
        assert(iobj != NOTHING);
        if (iobj == BOTTLE || iobj == CASK)
            obj = liquid_contents(iobj);
    }
    if (!is_liquid(obj)) {
        /* DRINK FOOD, or DRINK FROM [empty] CASK */
        puts("Don't be ridiculous!");
        return loc;
    }
    switch (iobj) {
        case BOTTLE:
        case CASK:
            if (obj != liquid_contents(iobj)) {
                /* Long didn't handle this case. */
                puts("It isn't there!");
            }
            break;
        case NOTHING:
            break;
        default:
            puts("Don't be ridiculous!");
            return loc;
    }

    if (iobj == NOTHING) {
        /* DRINK WATER. */
        assert(is_liquid(obj));
        const bool could_use_bottle = at_hand(BOTTLE, loc) && (liquid_contents(BOTTLE) == obj);
        const bool could_use_cask = at_hand(CASK, loc) && (liquid_contents(CASK) == obj);
        const bool could_use_ground = (liquid_at_location(loc) == obj);

        if (could_use_bottle) {
            iobj = BOTTLE;
        } else if (could_use_cask) {
            iobj = CASK;
        } else if (could_use_ground) {
            assert(iobj == NOTHING);
        } else {
            /* DRINK WATER AND WATER AND WATER */
            puts("It isn't here!");
            return loc;
        }
    }

    if (iobj == 0) {
        assert(liquid_at_location(loc) == obj);
    } else {
        assert(liquid_contents(iobj) == obj);
    }

    if (obj == OIL) {
        puts("Yeeeecchhh!!");
    } else if (obj == WATER) {
        if (iobj == BOTTLE) {
            destroy(WATER);
            objs(BOTTLE).prop = 1;
            puts("The bottle is now empty.");
        } else if (iobj == CASK) {
            destroy(WATER_IN_CASK);
            objs(CASK).prop = 1;
            puts("The cask is now empty.");
        } else {
            assert(iobj == NOTHING);
            puts("You have taken a drink from the stream.  The water tastes strongly of" SOFT_NL
                 "minerals, but is not unpleasant.  It is extremely cold.");
        }
    } else {
        assert(obj == WINE);
        if (iobj == BOTTLE) {
            assert(there(WINE, -BOTTLE));
            destroy(WINE);
            objs(BOTTLE).prop = 1;
        } else if (iobj == CASK) {
            assert(there(WINE_IN_CASK, -CASK));
            destroy(WINE_IN_CASK);
            objs(CASK).prop = 1;
        } else {
            assert(iobj == NOTHING);
        }
        /* Uh-oh. He's a wino. Let him reap the rewards of incontinence.
         * He'll wander around for awhile, then wake up somewhere or other,
         * having dropped most of his stuff. */
        puts("The wine goes right to your head.  You reel around in a drunken" SOFT_NL
             "stupor and finally pass out.  You awaken with a splitting headache," SOFT_NL
             "and try to focus your eyes....");
        if (objs(LAMP).prop) {
            /* In Long's version, Fortran's lack of multi-line IF bodies
             * led to an amusing case here: you could perpetually stave off
             * lamp burnout by getting drunk every 24 turns. */
            lamp_limit -= ran(lamp_limit)/2;
            if (lamp_limit < 10) lamp_limit = 25;
        }
        /* Determine the location he wakes up. */
        Location newloc;
        if (is_outside(loc)) {
            newloc = R_FOREST;
        } else if (pct(15)) {
            newloc = R_MAZEA49;
        } else if (pct(15)) {
            newloc = R_MAZEA53;
        } else if (pct(25)) {
            newloc = R_DIFF2;  /* Nasty! */
        } else {
            newloc = R_PITS;
        }
        /* Long does this next logic only if loc != newloc, which is a
         * little strange. */
        if (loc == newloc) return loc;
        /* As with the collapsing clay bridge: if you're toting the axe
         * or lamp inside a container, you're out of luck at this point. */
        if (holding(AXE)) drop(AXE, newloc);
        if (holding(LAMP)) drop(LAMP, newloc);
        for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
            if (holding(t)) drop(t, loc);
        }
        return newloc;
    }

    return loc;
}

void attempt_rub(ObjectWord obj)
{
    if (obj == LAMP) {
        puts("Rubbing the electric lamp is not particularly rewarding.  Anyway," SOFT_NL
             "nothing exciting happens.");
    } else {
        puts("Peculiar.  Nothing unexpected happens.");
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
        if (dloc[j] == loc) break;
    }
    assert(j <= 5);
    dloc[j] = R_LIMBO;  /* Once killed, a dwarf never comes back. */
    dseen[j] = false;
}

int throw_axe_at_dwarf(Location loc)
{
    if (pct(75)) {
        kill_a_dwarf(loc);
    } else {
        puts("You attack a little dwarf, but he dodges out of the way.");
    }
    drop(AXE, loc);
    return 'l';
}

int attempt_toss(Location loc, ObjectWord obj, PrepositionWord prep, ObjectWord iobj)
{
    if (prep == PREP_DOWN) {
        /* THROW DOWN AXE: "Thrown." */
        attempt_drop(loc, TOSS, (obj != NOTHING) ? obj : iobj, NOTHING, NOTHING);
    }
    /* TODO: factor this out from here and DROP */
    if (obj==ROD && !holding(ROD) && holding(ROD2)) obj = ROD2;
    if (!holding(obj)) {
        indent_appropriately();
        printf("You aren't carrying %s!\n", is_plural(obj) ? "them" : "it");
    }
    if (obj == BOAT || obj == BEAR) {
        /* THROW BEAR? It's much too heavy! */
        noway();
        return 0;
    }
    if (obj == FLOWERS && iobj == HIVE) {
        iobj = BEES;
    } else if (iobj == NOTHING) {
        /* No indirect object was specified. If a dwarf is present,
         * assume it is the iobj. If not, look for any other living thing.
         * Otherwise treat THROW as DROP. */
        if (dwarf_at(loc)) {
            obj = DWARF;
        } else {
            int k = 0;
            if (there(SNAKE, loc)) { ++k; iobj = SNAKE; }
            if (there(CLAM, loc)) { ++k; iobj = CLAM; }
            if (there(OYSTER, loc)) { ++k; iobj = OYSTER; }
            if (is_at_loc(DRAGON, loc)) { ++k; iobj = DRAGON; }
            if (is_at_loc(TROLL, loc)) { ++k; iobj = TROLL; }
            if (there(BEAR, loc)) { ++k; iobj = BEAR; }
            if (there(BEES, loc)) { ++k; iobj = BEES; }
            /* but *not* GNOME or DOG; oversight on Long's part */
            if (k > 1) {
                puts("Where?");
                return 0;
            } else if (k == 0 || iobj == BIRD) {
                attempt_drop(loc, TOSS, obj, NOTHING, NOTHING);
                return 0;
            }
            /* Long has a line that intends THROW COINS to select the troll
             * even in the presence of another mortal, e.g. the oyster.
             * But it's in the wrong place, so it never triggers. */
        }
    }

    assert(iobj != NOTHING);
    if (iobj == TROLL &&
        (is_treasure(obj) || (obj == CASK && liquid_contents(CASK) == WINE))) {
        /* Snarf a treasure for the troll. */
        destroy(obj);
        destroy(TROLL); destroy(TROLL_);
        drop(NO_TROLL, R_SWSIDE); drop(NO_TROLL_, R_NESIDE);
        juggle(CHASM); juggle(CHASM_);
        puts("The troll catches your treasure and scurries away out of sight.");
    } else if (obj == SWORD || obj == BOTTLE) {
        /* Throwing a fragile object will break it */
        attempt_break(loc, obj);
        return 0;
    } else if (is_edible(obj) && is_living(iobj)) {
        attempt_feed(loc, obj, iobj);
        return 0;
    } else if (obj == AXE) {
        /* THROW AXE AT something */
        if (iobj == DRAGON && !objs(DRAGON).prop) {
            puts("The axe bounces harmlessly off the dragon's thick scales.");
            move(AXE, loc);
            return 'l';
        } else if (iobj == TROLL) {
            puts("The troll deftly catches the axe, examines it carefully, and tosses it" SOFT_NL
                 "back, declaring, \"Good workmanship, but it's not valuable enough.\"");
            move(AXE, loc);
            return 'l';
        } else if (iobj == DWARF) {
            return throw_axe_at_dwarf(loc);
        } else if (iobj == BEAR && objs(BEAR).prop == 0) {
            immobilize_axe_via(BEAR, loc);
        } else if (iobj == WUMPUS && objs(WUMPUS).prop == 0) {
            immobilize_axe_via(WUMPUS, loc);
        } else if (iobj == DOG && !objs(DOG).prop) {
            immobilize_axe_via(DOG, loc);
        } else if (iobj == WUMPUS && objs(WUMPUS).prop != 6) {
            puts("The Wumpus grabs the axe, stops and picks his teeth with it for a few" SOFT_NL
                 "moments while looking thoughtfully at you.  When he finishes picking" SOFT_NL
                 "his teeth, he eats the axe, belches, farts... and starts after" SOFT_NL
                 "you again!");
            destroy(AXE);
        } else {
            return attempt_kill(loc, iobj, obj);
        }
    } else {
        /* THROW LAMP AT BEAR redirects to DROP LAMP */
        attempt_drop(loc, TOSS, obj, NOTHING, NOTHING);
    }
    return 0;
}

void attempt_find(Location loc, ObjectWord obj)
{
    /* Long doesn't check your inventory, nor the contents of the cask. */
    bool its_visibly_here =
            is_at_loc(obj, loc) ||
            (obj == liquid_contents(BOTTLE) && there(BOTTLE, loc)) ||
            (obj == liquid_contents(CASK) && there(CASK, loc) && is_ajar(CASK)) ||
            (obj == liquid_at_location(loc)) ||
            (obj == DWARF && dwarf_at(loc));
    if (its_visibly_here && toting(obj)) {
        indent_appropriately();
        printf("You are already carrying %s!\n", is_plural(obj) ? "them" : "it");
    } else if (closed) {
        puts("I daresay whatever you want is around here somewhere.");
    } else if (its_visibly_here) {
        puts("I believe what you want is right here with you.");
    } else {
        puts("I can only tell you what you see as you move about and manipulate" SOFT_NL
             "things.  I cannot tell you where remote things are.");
    }
}

void attempt_inventory(void)
{
    bool holding_anything = false;
    bool wearing_anything = false;
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (t == BOAT || t == BEAR) continue;
        if (!holding(t)) continue;
        if (objs(t).flags & F_WORN) continue;
        if (!holding_anything) {
            holding_anything = true;
            puts("You are currently holding the following:");
        }
        printf("%s\n", objs(t).name);
        lookin(t);
    }
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
        if (!(objs(t).flags & F_WORN)) continue;
        if (!wearing_anything) {
            wearing_anything = true;
            puts("You are wearing:");
        }
        printf("    %s\n", objs(t).name);
    }
    if (holding(BOAT)) {
        /* Long has "You are _now_ sitting...", because he
         * reuses the getting-in-the-boat message. */
        puts("You are sitting in a small boat.");
        lookin(BOAT);
    }
    if (holding(BEAR)) {
        puts("You are being followed by a very large, tame bear.");
    } else if (!holding_anything) {
        /* This message applies even if you're wearing something. */
        puts("You're not carrying anything.");
    }
}

void attempt_feed(Location loc, ObjectWord obj, ObjectWord iobj)
{
    /* FEED BEAR HONEY is transformed by the parser into FEED HONEY TO BEAR. */
    if (iobj == NOTHING || !is_living(iobj)) {
        if (obj == BIRD) {
            /* TODO: erroneously printed for FEED BIRD TO CHASM
             * (and FEED BIRD TO DOG, for that matter) */
            puts("It's not hungry (it's merely pinin' for the fjords).  Besides, you" SOFT_NL
                 "have no bird seed.");
        } else if (is_living(obj)) {
            /* FEED BEAR. See if there is anything edible around here. */
            ObjectWord food = find_an_edible_object(loc);
            if (food == NOTHING) {
                indent_appropriately();
                printf("What do you want to feed the %s?\n", otxt[objx]);
                /* Here Long sets OBJS(1)=0 OBJX=0 to end this clause,
                 * in the case of FEED BIRD AND BEAR. Line 22112 in Long.
                 * TODO: emulate this. */
            } else {
                attempt_feed(loc, food, obj);
            }
        } else {
            noway();
        }
        return;
    }

    if (iobj == DRAGON && objs(DRAGON).prop) {
        noway();  /* feeding a dead dragon */
    } else if (iobj == DRAGON) {
        puts("There's nothing here it wants to eat (except perhaps you).");
    } else if (iobj == TROLL) {
        puts("Gluttony is not one of the troll's vices.  Avarice, however, is.");
    } else if (iobj == SNAKE) {
        if (obj == BIRD && !closed) {
            puts("The snake has now devoured your bird.");
            destroy(BIRD);
            ++lost_treasures;  /* the jewelry */
        } else {
            puts("There's nothing here it wants to eat (except perhaps you).");
        }
    } else if (iobj == DWARF) {
        puts("You fool, dwarves eat only coal!  Now you've made him *REALLY* mad!!");
        dflag += 1;
    } else if (iobj == BEAR) {
        if (objs(BEAR).prop == 3) {
            noway();  /* feeding a dead bear */
        } else if (obj == HONEY) {
            puts("The bear eagerly licks up the honeycomb, after which he seems to calm" SOFT_NL
                 "down considerably and even becomes rather friendly.");
            destroy(HONEY);
            objs(BEAR).prop = 1;
            remobilize_axe(1);
        } else if (obj == FOOD) {
            puts("All you have are watercress sandwiches.  The bear is less than" SOFT_NL
                 "interested.");
        } else if (objs(BEAR).prop == 0) {
            puts("There's nothing here it wants to eat (except perhaps you).");
        } else {
            puts("He isn't hungry.");
        }
    } else if (iobj == DOG) {
        if (objs(DOG).prop) {
            puts("That wouldn't be wise.  It is best to let sleeping dogs lie.");
        } else if (obj == FOOD) {
            destroy(FOOD);
            puts("The dog wolfs (natch) down the food and looks around hungrily for" SOFT_NL
                 "more.  However, he does not appear to be any better disposed towards" SOFT_NL
                 "your presence.");
        } else {
            puts("There's nothing here it wants to eat (except perhaps you).");
        }
    } else if (iobj == WUMPUS) {
        if (objs(WUMPUS).prop == 6) {
            puts("Dead wumpi, as a rule, are light eaters.  Nothing happens.");
        } else if (objs(WUMPUS).prop == 0) {
            /* Long has "How do expect". */
            puts("How do you expect to feed a sleeping Wumpus?");
        } else if (obj == FOOD) {
            /* Long gives this message even if the Wumpus is dead or asleep. */
            puts("The Wumpus looks at the food with distaste.  He looks at *YOU* with" SOFT_NL
                 "relish!");
        } else {
            puts("There's nothing here it wants to eat (except perhaps you).");
        }
    } else if (iobj == BEES && obj == FLOWERS && !objs(FLOWERS).prop) {
        puts("The bees swarm over the fresh flowers, leaving the hive unguarded" SOFT_NL
             "and revealing a sweet honeycomb.");
        move(FLOWERS, loc);
        immobilize(FLOWERS);
        objs(FLOWERS).prop = 1;  /* swarmed by bees */
        objs(HIVE).prop = 1;
        drop(HONEY, loc);
    } else {
        noway();
    }
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
        case 18:
            puts("The NE passage is blocked by a recent cave-in.");
            break;
        case 19:
            puts("You had better leave the boat first.");
            break;
        case 20:
            puts("You can't swim.  You'd best go by boat.");
            break;
        case 21:
            puts("You go a short way down the bright passage, but the light" SOFT_NL
                 "grows to blinding intensity.  You can't continue.");
            break;
        case 22:
            puts("The wall is too smooth to climb.");
            break;
        case 23:
            puts("The shelf is beyond your reach.");
            break;
        case 24:
            puts("You can't fit through a six-inch door!");
            break;
        case 25:
            puts("You managed to climb about halfway up before losing your hold and" SOFT_NL
                 "sliding back.");
            break;
        case 26:
            puts("You were only a few yards from the top when you slipped and tumbled" SOFT_NL
                 "all the way back down.");
            break;
        case 27:
            puts("You are now too big to leave the way you came in.");
            break;
        case 28:
            puts("You have approached the lower end of a steep passage, but it is" SOFT_NL
                 "just too cold here to hang around, and you aren't properly equipped" SOFT_NL
                 "to continue.  With teeth chattering, you climb back up....");
            break;
        case 29:
            puts("I don't understand what you are trying to do!");
            break;
        case 30:
            puts("The river is too wide and deep to cross.");
            break;
        case 31:
            puts("The dog won't let you pass.");
            break;
        case 32:
            puts("The stream flows out of one very small crack and into another." SOFT_NL
                 "You can't go that way.");
            break;
        case 33:
            puts("How do you propose to cross the river?");
            break;
        case 34:
            puts("You are blocked by a wall of solid rock.");
            break;
        case 35:
            puts("The cliff is unscalable.");
            break;
        case 36:
            puts("You are still in the boat.  It is too heavy to carry.");
            break;
        case 37:
            puts("You couldn't possibly cross this sea without a large ship.");
            break;
        case 38:
            /* This remark is printed when you try to go WEST from
             * R_HUGEDOOR but the door is still locked.
             * Long's remark 239 is ">$<", meaning "don't print anything".
             * (Oddly, McDonald has the variant spelling "<$$<" instead.)
             * This is certainly a mistake. I've taken the liberty of
             * making up an appropriate remark. */
            puts("The door is locked.");
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
    assert(holding(BEAR));
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

Location attempt_phuce(Location from)
{
    Location newloc = (from == R_WBLUE) ? R_SEA :
                      (from == R_SEA) ? R_WBLUE :
		      (from == R_TINYDOOR) ? R_HUGEDOOR :
                      R_TINYDOOR;
    /* Move all the objects from oldloc to newloc,
     * except for the boat. */
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
	if (t == BOAT) continue;
	if (t == TINY_DOOR) continue;
	if (objs(t).place == from) {
	    move(t, newloc);
	}
    }
    return newloc;
}

Location attempt_phonebooth(void)
{
    if (objs(BOOTH).prop == 1) {
	puts("The gnome firmly blocks the door of the booth.  You can't enter.");
    } else if (places[R_ROTUNDA].visits == 1 || pct(55)) {
        /* TODO: Long allows this to happen during closing time, which seems wrong. */
	puts("As you move towards the phone booth, a gnome suddenly streaks" SOFT_NL
             "around the corner, jumps into the booth and rudely slams the door" SOFT_NL
             "in your face.  You can't get in.");
	objs(BOOTH).prop = 1;
	move(GNOME, R_ROTUNDA);
    } else {
	return R_BOOTH;
    }
    return R_ROTUNDA;
}

Location attempt_clay_bridge(Location from)
{
    /* Collapsing clay bridge. He can cross with three (or fewer)
     * things. If more, or if carrying obviously heavy things,
     * he may end up in the drink. */
    assert(from == R_DANTES || from == R_DEVILS);
    Location newloc = (R_DANTES + R_DEVILS) - from;

    int current_burden = burden(0);
    static int bcross = 0;
    bcross += 1;
    int danger = (current_burden+bcross)*(current_burden+bcross) / 10;
    if (current_burden <= 4) {
	/* An uneventful crossing. */
    } else if (pct(danger > 10 ? danger : 10)) {
	/* Splash! */
	puts("The load is too much for the bridge!  With a roar, the entire" SOFT_NL
             "structure gives way, plunging you headlong into the raging river at" SOFT_NL
             "the bottom of the chasm and scattering all your holdings.  As the" SOFT_NL
             "icy waters close over your head, you flail and thrash with all your" SOFT_NL
             "might, and with your last ounce of strength pull yourself onto the" SOFT_NL
             "south bank of the river.");
	/* TODO: Rewrite this logic.
	 * Notice that if you put the lamp in the sack before crossing, you'll lose
	 * it forever; but the same is not true of the axe. */
	if (holding(LAMP)) move(LAMP, R_ELOST);
	if (toting(AXE)) move(AXE, R_SLOST);
        if (toting(SAPPHIRE)) river_has_sapphire = true;
	for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
	    if (toting(t)) destroy(t);
	}
	objs(CLAY).prop = 1;  /* no more bridge */
	return R_ELOST;
    } else {
	puts("The bridge shakes as you cross.  Large hunks of clay and rock near" SOFT_NL
             "the edge break off and hurtle far down into the chasm.  Several of" SOFT_NL
             "the cracks on the bridge surface widen perceptibly.");
    }
    return newloc;
}

Location attempt_kaleidoscope(void)
{
    if (kaleidoscope_count == 6) {
	return R_INNER;
    } else {
	puts("You get a tingling feeling as you walk through the gate, and ...");
	return R_KAL_RED + ran(R_KAL_BLUE - R_KAL_RED + 1);
    }
}

/* Modify newloc in place, and return true if the player died crossing the troll bridge. */
bool determine_next_newloc(Location loc, Location *oldloc, Location *newloc,
                           MotionWord mot, ActionWord verb)
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
        assert(toting(EMERALD));
        move(EMERALD, loc);
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
    } else if (*newloc == R_PHUCE) {
        *newloc = attempt_phuce(loc);
    } else if (*newloc == R_GNOME) {
        *newloc = attempt_phonebooth();
    } else if (*newloc == R_CLAY) {
        *newloc = attempt_clay_bridge(loc);
    } else if (*newloc == R_KALEIDOSCOPE) {
        *newloc = attempt_kaleidoscope();
        if (*newloc != R_INNER) {
            assert(R_KAL_RED <= *newloc && *newloc <= R_KAL_PURPLE);
            *oldloc = *newloc - 1;
        }

    } else {
        assert(*newloc >= R_LIMBO);
        assert(*newloc <= MAX_LOC);
    }
    return false;
}

void history_of_adventure(void)
{
    /* Another descendant of Long's version available online, "Adventure 5.2/2",
     * contains a MUCH longer History of Adventure, including contact information
     * for David Long and teasers pertaining to LONG0751.
     * TODO: perhaps copy that version here instead? */
    puts("-          *** THE HISTORY OF ADVENTURE (ABRIDGED) ***          -\n"
         "-                    ** By Ima Wimp **                          -\n"
         "ADVENTURE was originally developed by William Crowther, and later" SOFT_NL
         "substantially rewritten and expanded by Don Woods at Stanford Univ." SOFT_NL
         "According to legend, Crowther's original version was modelled on an" SOFT_NL
         "a real cavern, called Colossal Cave, which is a part of Kentucky's" SOFT_NL
         "Mammoth Caverns.  That version of the game included the main maze" SOFT_NL
         "and a portion of the third-level (Complex Junction" ENDASH(" - ") "Bedquilt" ENDASH(" -") SOFT_NL
         "Swiss Cheese rooms, etc.), but not much more." SOFT_NL
         "Don Woods and some others at Stanford later rewrote portions of" SOFT_NL
         "the original program, and greatly expanded the cave.  That version" SOFT_NL
         "of the game is recognizable by the maximum score of 350 points." SOFT_NL
         "Some major additions were done by David Long while at the University" SOFT_NL
         "of Chicago, Graduate School of Business. Long's additions include the" SOFT_NL
         "seaside entrance and all of the cave on the \"far side\" of Lost River" SOFT_NL
         "(Rainbow Rm" ENDASH(" - ") "Crystal Palace" ENDASH(" - ") "Blue Grotto, etc.)." SOFT_NL
         "The castle problem was added in late 1984 by an anonymous writer." SOFT_NL
         "Thanks are owed to Roger Matus and David Feldman, both of U. of C.," SOFT_NL
         "for several suggestions, including the Rainbow Room, the telephone" SOFT_NL
         "booth and the fearsome Wumpus. Most thanks (and apologies)" SOFT_NL
         "go to Thomas Malory, Charles Dodgson, the Grimm Brothers, Dante," SOFT_NL
         "Homer, Frank Baum and especially Anon., the real authors of ADVENTURE.");
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
        case SWIM:
            puts("I don't know how.");
            break;
        case SHIT:
            puts("I don't need to, thank you.");
            break;
        case PISS:
            puts("The ground is wet and the air is smelly.");
            break;
    }
}

void simulate_an_adventure(void)
{
    Location oldoldloc, oldloc, loc, newloc;
    MotionWord mot = NOWHERE;  /* currently specified motion */
    int verb = NOTHING;  /* currently specified action */
    int oldverb;  /* verb before it was changed */
    int obj = NOTHING;  /* currently specified object, if any */
    int oldobj;  /* former value of obj */
    int iobj = NOTHING;
    bool was_dark = false;
    bool please_clarify = false;

    oldoldloc = oldloc = loc = newloc = R_ROAD;

    while (true) {
    movement:  /* Long's line 2 */
        set_indentation(0);

        /* Check for interference with the proposed move to newloc. */
        if (cave_is_closing() && is_outside(newloc) && newloc != R_LIMBO) {
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
        set_indentation(0);
        if (loc == R_LIMBO) goto death;
        switch (look_around(loc, now_in_darkness(loc), was_dark)) {
            case 'd': goto death;
            case 'p': goto pitch_dark;
            case 't': goto try_move;
            default: break;
        }
        /* Upon leaving the rotunda, cause the gnome to vanish. Line 2045 in Long. */
        if (oldloc == R_ROTUNDA && loc != R_ROTUNDA && objs(BOOTH).prop == 1) {
            assert(there(GNOME, R_ROTUNDA));
            assert(loc != R_BOOTH);  /* can't enter booth while gnome's there */
            destroy(GNOME);
            objs(BOOTH).prop = 0;
        }
        while (true) {
            verb = oldverb = NOTHING;
            oldobj = obj;
            set_indentation(0);

            maybe_give_a_hint(loc, oldloc, oldoldloc, oldobj);
            was_dark = now_in_darkness(loc);
            adjustments_before_listening(loc);

            /* Get a new input line, or finish getting current one.
             * Long's line 2605. */
            if (please_clarify) {
                assert(w_verbs[0] != NOTHING);
                getwds(loc);
                verb = w_verbs[vrbx = 0];
                obj = w_objs[objx = 0];
                iobj = w_iobjs[iobx = 0];
                please_clarify = false;
            } else {
                shift_words(&verb, &obj, &iobj, loc);
            }

            assert(w_iobjs[1] == NOTHING);  /* TODO is this possible? verify */
            if (w_objs[1] != NOTHING) {
                puts(objs(w_objs[objx]).name);
                set_indentation(4);
            }

            ++turns;

            /* Just after every command you give, we make the foobar counter
             * negative if you're on track, otherwise we zero it. */
            foobar = (foobar > 0) ? -foobar : 0;
            combo = (combo > 0) ? -combo : 0;

            if (turns == 310 && !terse && (verbose_interval != 10000)) {
                puts("You know, if you type \"BRIEF\", I won't have to keep repeating these" SOFT_NL
                     "long descriptions of each room.");
            }

            if (check_clocks(loc)) {
                /* The cave just closed! */
                loc = oldloc = R_NEEND;
                mot = NOWHERE;
                goto try_move;
            }
            check_lamp(loc);

            if ((obj == KNIFE || iobj == KNIFE) && last_knife_loc == loc) {
                puts("The dwarves' knives vanish as they strike the walls of the cave.");
                last_knife_loc = R_LIMBO;
                continue;
            }

            switch (word_class(verb)) {
                case WordClass_Motion:
                    mot = verb;
                    goto try_move;
                case WordClass_Action:
                    break;
                case WordClass_Message:
                    print_message(verb);
                    continue;
                default:
                    assert(false);
            }

            if (obj != NOTHING || iobj != NOTHING) goto transitive;

            /* Analyze an intransitive verb. */
            switch (verb) {
                case TAKE:
                case YANK:
                case WEAR:
                case GET:
                case INSERT:
                case REMOVE:
                case BURN:
                    if (blind_at(loc) ||
                            places[loc].objects == NULL ||
                            places[loc].objects->link != NULL ||
                            dwarf_at(loc))
                        goto act_on_what;
                    obj = (places[loc].objects - &objs(MIN_OBJ) + MIN_OBJ);
                    goto transitive;
                case OPEN:
                case CLOSE:
                case LOCK:
                case UNLOCK:
                    if (find_an_openable_object(loc, verb, &obj)) {
                        continue;
                    } else if (obj == NOTHING) {
                        goto act_on_what;
                    } else {
                        goto transitive;
                    }
                case RELAX:
                case LIGHT:
                case EXTINGUISH:
                case GO:
                case KILL:
                case BLAST:
                case SWEEP:
                    goto transitive;
                case POUR: {
                    const bool could_use_bottle = holding(BOTTLE) && (liquid_contents(BOTTLE) != NOTHING);
                    const bool could_use_cask = holding(CASK) && (liquid_contents(CASK) != NOTHING);
                    if (could_use_bottle == could_use_cask)
                        goto act_on_what;
                    obj = (could_use_cask ? CASK : BOTTLE);
                    goto transitive;
                }
                case EAT:
                    obj = find_an_edible_object(loc);
                    if (obj == NOTHING) {
                        goto act_on_what;
                    } else {
                        goto transitive;
                    }
                case DRINK: {
                    const bool could_use_bottle = at_hand(BOTTLE, loc) && (liquid_contents(BOTTLE) != NOTHING);
                    const bool could_use_cask = at_hand(CASK, loc) && (liquid_contents(CASK) != NOTHING);
                    const ObjectWord liquid_here = liquid_at_location(loc);
                    const bool could_use_ground = (liquid_here == WATER || liquid_here == WINE);
                    /* We could drink from three places, in order of
                     * preference: the bottle, the cask, the ground.
                     * If we're carrying two different liquids, prompt
                     * the user for which one he wants to drink.
                     * If we're not carrying anything and there's oil
                     * on the ground, that's not appetizing.
                     * Long has a bug such that DRINK when there's oil
                     * present is treated as DRINK WINE. I've heavily
                     * refactored this code to remove the bug. */
                    if (could_use_bottle && could_use_cask &&
                            liquid_contents(BOTTLE) != liquid_contents(CASK)) {
                        goto act_on_what;
                    } else if (could_use_bottle) {
                        assert(obj == NOTHING);
                        prep = FROM;
                        iobj = BOTTLE;
                    } else if (could_use_cask) {
                        assert(obj == NOTHING);
                        prep = FROM;
                        iobj = CASK;
                    } else if (could_use_ground) {
                        obj = liquid_here;
                        assert(prep == NOTHING);
                        assert(iobj == NOTHING);
                    } else {
                        goto act_on_what;
                    }
                    goto transitive;
                }
                case INVENTORY:
                    attempt_inventory();
                    continue;
                case FILL: {
                    const bool could_use_bottle = here(BOTTLE, loc) && (objs(BOTTLE).prop == 1);
                    const bool could_use_cask = here(CASK, loc) && (objs(CASK).prop == 1);
                    if (could_use_bottle && !here(CASK, loc)) {
                        assert(!could_use_cask);
                        obj = BOTTLE;
                    } else if (could_use_cask && !here(BOTTLE, loc)) {
                        assert(!could_use_bottle);
                        obj = CASK;
                    } else {
                        goto act_on_what;
                    }
                    goto transitive;
                }
                case SCORE:
                    attempt_score();
                    continue;
                case FEEFIE:
                    attempt_feefie(loc);
                    continue;
                case BRIEF:
                    look_count = 3;
                    terse = false;
                    if (verbose_interval == 10000) {
                        verbose_interval = 5;
                        puts("Ok, I'll give you the full description whenever you enter a room" SOFT_NL
                             "for the first time.");
                    } else {
                        verbose_interval = 10000;
                        puts("Okay, from now on I'll only describe a place in full the first time" SOFT_NL
                             "you come to it.  To get the full description, say \"LOOK\".");
                    }
                    continue;
                case ANSWER:
                    if (loc == R_BOOTH && !objs(PHONE).prop) {
                        attempt_answer(PHONE);
                    } else {
                        goto act_on_what;
                    }
                case BLOW:
                    puts("You are now out of breath.");
                    continue;
                case LEAVE:
                    assert(false);  /* Long's BUG(29) */
                case CALL:
                    if (!here(PHONE, loc)) goto act_on_what;
                    attempt_use_phone();
                    continue;
                case GRIPE:
                    /* In Adventure 5.2/2, there is some logic here, and the message reads:
                     *     Thank you for your comments. They will be dispatched immediately
                     *     via Gnome Express to the Wizard.
                     * I don't know if the following message is due to Anonymous or
                     * to Doug McDonald.
                     */
                    puts("Please read the supplied documentation files to find out where to" SOFT_NL
                         "send complaints, suggestions, and bug reports.");
                    break;
                case DIAGNOSE:
                    attempt_diagnose();
                    continue;
                case LOOK:
                    goto transitive;
                case COMBO:
                    attempt_combo(loc);
                    continue;
                case TERSE:
                    look_count = 3;
                    terse = !terse;
                    puts(ok);
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
                case DROP:
                case SAY:
                case WAVE:
                case CALM:
                case RUB:
                case TOSS:
                case FIND:
                case FEED:
                case READ:  /* Long removes the baroque intransitive-READ logic */
                case BREAK:
                case WAKE:
                case HIT:  /* not the same as KILL */
                case DIAL:
                case PLAY:
                case PICK:
                case PUT:
                case TURN:
                act_on_what:
                    vtxt[vrbx][0] = toupper(vtxt[vrbx][0]);
                    printf("%s what?\n", vtxt[vrbx]);
                    please_clarify = true;
                    continue;
                default:
                    assert(false);  /* BUG(23) in Long's code */
            }

        transitive:
            /* Analyze a transitive verb, with obj or iobj. */
            switch (verb) {
                case TAKE:
                    /* TAKE INVENTORY is redirected in the parser. */
                    attempt_take(loc, TAKE, obj, prep, iobj);
                    continue;
                case DROP:
                case LEAVE:
                    attempt_drop(loc, verb, obj, prep, iobj);
                    continue;
                case SAY:
                case CALL:
                    assert(false);  /* Long's BUG(34) */
                case OPEN:
                    attempt_open(loc, obj, iobj);
                    continue;
                case RELAX:
                    puts(ok);
                    continue;
                case CLOSE:
                    attempt_close(loc, obj);
                    continue;
                case LIGHT:
                light_lamp:
                    /* Long lazily allows "LIGHT KEYS". */
                    if (attempt_light(loc) && was_dark) {
                        goto commence;
                    }
                    continue;
                case EXTINGUISH:
                extinguish_lamp:
                    /* Long lazily allows "EXTINGUISH KEYS". */
                    attempt_extinguish(loc);
                    continue;
                case WAVE:
                    attempt_wave(oldloc, loc, obj, iobj);
                    continue;
                case CALM:
                    puts("I'm game.  Would you care to explain how?");
                    continue;
                case GO:
                    puts("Where?");
                    continue;
                case KILL:
                    switch (attempt_kill(loc, obj, iobj)) {
                        case 'a':
                            assert(obj == NOTHING);
                            goto act_on_what;
                        case 'd':  /* just killed the dragon */
                            newloc = R_SCAN2;
                            goto movement;
                        case 'l':
                            newloc = loc;
                            goto movement;
                        case 'x':
                            oldloc = oldoldloc = loc;
                            goto death;
                        case 0: break;
                        default: assert(false);
                    }
                    continue;
                case POUR:
                    switch (attempt_pour(loc, obj, prep, iobj)) {
                        case 'm':
                            /* This is the same as "goto commence", except
                             * that it gives the dwarves a chance to move. */
                            newloc = loc;
                            goto movement;
                        case 0: break;
                        default: assert(false);
                    }
                    continue;
                case EAT: {
                    newloc = attempt_eat(loc, obj);
                    if (newloc != loc) {
                        goto movement;
                    }
                    continue;
                }
                case DRINK: {
                    newloc = attempt_drink(loc, obj, iobj);
                    if (newloc != loc) {
                        goto movement;
                    }
                    continue;
                }
                case RUB:
                    attempt_rub(obj);
                    continue;
                case TOSS:
                    switch (attempt_toss(loc, obj, prep, iobj)) {
                        case 'l':
                            newloc = loc;
                            goto movement;
                        case 0: break;
                        default: assert(false);
                    }
                    continue;
                case FIND:
                case INVENTORY:
                    attempt_find(loc, obj);
                    continue;
                case FEED:
                    attempt_feed(loc, obj, iobj);
                    continue;
                case FILL:
                    attempt_fill(loc, obj, iobj);
                    continue;
                case BLAST:
                    attempt_blast(loc);
                    continue;
                case FEEFIE:
                    puts("I don't know how.");
                    continue;
                case BRIEF:
                    /* Long has some logic to print Woods' punny "On what?" when
                     * is_living(obj), but the logic is both backwards (it's
                     * trying to print the message when iobj *has* been provided)
                     * and unreachable (the parser won't let you BRIEF X ON Y). */
                    confuz();
                    continue;
                case READ:
                    if (obj == NOTHING) obj = iobj;
                    attempt_read(loc, obj);
                    continue;
                case BREAK:
                    /* Notice that we don't look at iobj in this case, so
                     * we'll give inappropriate responses to SMASH VASE
                     * WITH AXE or BREAK SWORD WITH SWORD. */
                    attempt_break(loc, obj);
                    continue;
                case WAKE:
                    attempt_wake(loc, obj);
                    continue;
                case YANK:
                    if (toting(obj)) {
                        attempt_drop(loc, YANK, obj, prep, iobj);
                    } else {
                        attempt_take(loc, YANK, obj, prep, iobj);
                    }
                    continue;
                case WEAR:
                    attempt_wear(loc, obj);
                    continue;
                case HIT:
                    attempt_hit(loc, obj, iobj);
                    continue;
                case ANSWER:
                    attempt_answer(obj);
                    continue;
                case BLOW:
                case PLAY:
                    if (attempt_play(loc, verb, obj, iobj)) {
                        assert(loc == R_ARCHED);
                        assert(obj == HORN);
                        newloc = R_ARCHED_RUBBLE;
                        goto movement;
                    }
                    continue;
                case DIAL:
                    attempt_dial(obj);
                    continue;
                case PICK:
                    attempt_pick(loc, obj, prep, iobj);
                    continue;
                case PUT:
                    attempt_put(loc, obj, prep, iobj);
                    continue;
                case TURN:
                    if (prep == NOTHING) {
                        confuz();
                    } else if ((obj == LAMP) != (iobj == LAMP) &&
                               (obj == NOTHING) != (iobj == NOTHING)) {
                        if (prep == ONTO) {
                            goto light_lamp;
                        } else {
                            goto extinguish_lamp;
                        }
                    } else {
                        noway();
                    }
                    continue;
                case GET:
                    attempt_get(loc, obj, prep, iobj);
                    continue;
                case INSERT:
                    assert(prep == INTO || prep == NOTHING);
                    attempt_insert_into(loc, obj, iobj);
                    continue;
                case REMOVE:
                    assert(prep == FROM || prep == NOTHING);
                    attempt_remove_from(loc, obj, iobj);
                    continue;
                case BURN:
                    puts("You haven't any matches.");
                    continue;
                case LOCK:
                    attempt_lock(loc, obj);
                    continue;
                case UNLOCK:
                    attempt_unlock(loc, obj, iobj);
                    continue;
                case LOOK:
                    switch (attempt_look(loc, obj, prep, iobj)) {
                        case 'l':
                            was_dark = false; /* so you don't fall into a pit */
                            newloc = loc;
                            goto movement;
                        case 0:
                            continue;
                    }
                case COMBO:
                    noway();
                    continue;
                case SWEEP:
                    attempt_sweep(loc);
                    continue;
                case GRIPE:
                case DIAGNOSE:
                case TERSE:
                    confuz();
                    continue;
                case SCORE:
                case QUIT:
#ifdef SAVE_AND_RESTORE
                case SAVE:
                case RESTORE:
#endif /* SAVE_AND_RESTORE */
                    puts("I don't understand that!");
                default:
                    assert(false);  /* BUG(24) in Long's code */
            }
        }

    try_move:
        /* A major cycle comes to an end when a motion verb mot has been
         * given and we have computed the appropriate newloc accordingly. */
        assert(R_LIMBO <= oldloc && oldloc <= MAX_LOC);
        assert(R_LIMBO < loc && loc <= MAX_LOC);
        newloc = loc;  /* by default we will stay put */
        set_indentation(0);

        if (mot == CAVE) {
            /* No CAVE appears in the travel table; its sole purpose is to
             * give these messages. */
            if (is_outside(loc)) {
                puts("I can't see where the cave is, but hereabouts no stream can run on" SOFT_NL
                     "the surface for long. I would try the stream.");
            } else {
                puts("I need more detailed instructions to do that.");
            }
        } else {
            if (mot == BACK) {
                Location l = (is_forced(oldloc) ? oldoldloc : oldloc);
                mot = try_going_back_to(l, loc); /* may return NOWHERE */
            }

            if (mot != NOWHERE) {
                /* Determine the next newloc. */
                oldoldloc = oldloc;
                oldloc = loc;
                if (determine_next_newloc(loc, &oldloc, &newloc, mot, verb)) {
                    /* Player died trying to cross the troll bridge. */
                    oldoldloc = newloc;  /* if you are revived, you got across */
                    goto death;
                }
            }

            if (R_KAL_RED <= newloc && newloc <= R_KAL_PURPLE) {
                if (newloc == R_KAL_RED) kaleidoscope_count = 0;
                else if (newloc != loc+1) kaleidoscope_count = -10;
                else kaleidoscope_count += 1;
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
    lamp_limit = (hints[0].given ? 650 : 400);
    build_vocabulary();
    build_travel_table();
    build_object_table();
    simulate_an_adventure();
    return 0;
}

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

#define apport(obj, loc) object_position[obj] = loc
#define toting(obj) (object_position[obj] == -1)
#define there(obj, loc) (object_position[obj] == loc)
#define here(obj, loc) (there(obj, loc) || toting(obj))

enum {
    R_INHAND = -1, R_LIMBO = 0,
    R_START, R_TREE, R_HOUSE, R_CUBIC1, R_SMALL, R_MAZE1, R_MAZE2, R_MAZE3, R_CHEST, R_INSIDE,
    R_MYSTERY, R_SEA, R_CATACOMB, R_TURKISH, R_GARDEN, R_GREEN, R_STORE, R_TONE, R_TIME, R_SLIME,
    R_LIBRARY, R_KITCHEN, R_LIVING, R_FOYER, R_CUBIC2, R_FALL, R_MINE, R_CUBIC3, R_SEWER, R_TREASURE,
    R_VIPER, R_YARD, R_NORTH, R_BURGER, R_HALL, R_ATTIC, R_BATH, R_BED, R_SITTING, R_CUBIC4,
    R_EW, R_COLD, R_SANDY, R_QUICK, R_ROCKY, R_BASE, R_INDY, R_CHAPEL, R_SUMMIT, R_EXPERT,
    R_BUNNY, R_SHACK, R_CAVE, R_CRAWL, R_STROBE, R_SEWER2, R_WET, R_RROOM, R_LAB, R_CLOSET,
    R_RCORE, R_HOSPITAL, R_NBURGER, R_STATION1, R_TRAIN, R_STATION2, R_ART,
};
typedef int Location;

struct Place {
    const char *long_desc;
    const char *short_desc;
    Location neighbors[8];
    char hole_depth;                  // M in the original program
    bool visited;                     // also M, somehow
};
struct Place places[R_ART+1];

Location object_position[69+1];       // O[n,1] in the original program
char active_command[130];             // C1$ in the original program
const char *active_verb = NULL;       // B$ in the original program
const char *active_object = NULL;     // O1$ in the original program
const char *default_object = NULL;    // D$ in the original program
int brief_mode = 0;                   // B9 in the original; 0=brief, 1=superbrief, 2=long
bool chest_is_unlocked = false;       // C8 in the original program
bool chest_is_open = false;           // C9 in the original program
int quicksand_timer = 0;              // D2 in the original program
int lamp_power = 350;                 // P0 in the original program
bool lamp_is_lit = false;             // L in the original program
Location loc = R_START;               // R0 in the original program
Location oldloc = R_LIMBO;            // R1 in the original program
bool train_at_museum = false;         // handled via R1 in the original
bool reactor_is_active = true;        // R3 in the original program
bool married = false;                 // M9 in the original program
int score = 0;                        // S in the original program
int turns = 0;                        // T in the original program
bool wizard_mode = false;             // Z7 in the original program
int zarka_status = 0;                 // Z9 in the original; 1=friendly, 2=enraged
bool aboard_disc = false;             // D1 in the original program
bool fucked = false;                  // F9 in the original program
bool really_look = false;             // handled via control flow in the original

#define X_MACRO_OBJECTS \
    X(LAMP, "LAMP", 5, "Battery-powered lamp") \
    X(KEYS, "KEYS", 4, "Set of iron keys") \
    X(CHEST, "CHEST", 500, "Dusty oak chest") \
    X(JEWEL, "JEWEL", 30, "Sparkling diamond jewelry") \
    X(GOLD, "GOLD", 60, "Dazzling nugget of gold") \
    X(TREE, "TREE", 500, "Small structure in the tree") \
    X(WITCH, "WITCH", -1, "Wrinkly old witch") \
    X(LEVER, "LEVER", 500, "Black 6-inch lever embedded in the wall of the cubicle") \
    X(GIRL, "GIRL", 80, "Sexy young girl") \
    X(PEARL, "PEARL", 30, "Perfectly round pearl") \
    X(NOTE, "NOTE", 3, "Obscure, wrinkled note") \
    X(CELLO, "CELLO", 30, "Stradivarius cello") \
    X(WATCH, "WATCH", 30, "Finely crafted Swiss timepiece") \
    X(PARCHMENT, "PARCHMENT", 30, "Ancient piece of parchment") \
    X(VASE, "VASE", 30, "Delicate Ming vase") \
    X(STAFF, "STAFF", 17, "Three-foot long staff") \
    X(COINS, "COINS", 0, "Set of very rare coins") \
    X(KNIFE, "KNIFE", 10, "Sharp butcher's knife") \
    X(SLIME, "SLIME", -1, "Threatening mass of slime on the floor") \
    X(STOOL, "STOOL", 30, "Soft velvet foot stool") \
    X(FLASK, "FLASK", 7, "Transparent glass flask") \
    X(HILLS, "HILLS", 8, "Great number of little dirt hills") \
    X(BEAST, "BEAST", -1, "Huge gorilla-like beast") \
    X(CYLINDER, "CYLINDER", 50, "White crystal cylinder") \
    X(ZARKA, "ZARKA", -1, "Horrible Zarka") \
    X(GLOBE, "GLOBE", 60, "Shining platinum globe with readable coating") \
    X(SNAKE, "SNAKE", -1, "Hungry-looking black snake") \
    X(ROCKS, "ROCK", 500, "Wall of large rocks blocking your exit") \
    X(FRIES, "FRIES", 5, "Large order of french fries") \
    X(PLATE, "PLATE", 4, "Burgerland dinner plate") \
    X(FISH, "FISH", 4, "Filet-o-fish sandwich") \
    X(CLOWN, "CLOWN", -1, "Odd clown with red hair and yellow overalls") \
    X(SPRAY, "SPRAY", 8, "Can of disinfectant spray") \
    X(PIZZA, "PIZZA", 5, "Generous slice of pizza") \
    X(BED, "BED", 500, "King-size bed") \
    X(JOHN, "JOHN", 500, "Porcelain toilet") \
    X(TUB, "TUB", 500, "Old-fashioned bathtub") \
    X(BOOK, "BOOK", 5, "Tiny yellow book with a broken binding") \
    X(COMPUTER, "COMPUTER", 500, "Large machine") \
    X(KNOB, "KNOB", 500, "Small red knob [on the machine]") \
    X(LABEL, "LABEL", 500, "Tiny tan label [on the machine]") \
    X(RING, "RING", 30, "Emerald wedding ring") \
    X(BLOCK, "BLOCK", 5, "Steamy white brick-like block") \
    X(WATER, "WATER", 5, "Clear water [in waterfall]") \
    X(DOOR, "DOOR", 500, "Stately wooden door") \
    X(GLOVE, "GLOVE", 5, "Leather work glove") \
    X(DISC, "DISC", 30, "Silver disc with markings") \
    X(BOARD, "BOARD", 500, "Immense bulletin board") \
    X(POST, "POST", 500, "Signpost partially covered with snow") \
    X(BUTTON, "BUTTON", 500, "Beige button marked ~ HIGH VOLTAGE ~") \
    X(LIFT, "LIFT", 500, "Strange contraption") \
    X(SPADE, "SPADE", 10, "Dirty shovel") \
    X(STATUE, "STATUE", 40, "Beautiful jade statue") \
    X(COMB, "COMB", 20, "Ivory-handled comb") \
    X(SHARDS, "PIECES", 18, "Worthless shards of pottery") \
    X(SAPPHIRE, "SAPPHIRE", 75, "Flawless blue sapphire") \
    X(PLATFORM, "PLATFORM", 500, "Low platform") \
    X(GRATE, "GRATE", 500, "Small grating on the southern wall") \
    X(SLABS, "SLABS", 500, "Metal insulation slabs") \
    X(WHEEL, "WHEEL", 500, "Large wheel mounted on the platform") \
    X(BOOTS, "BOOTS", 0, "Yellow rubber boots") \
    X(GUM, "CUBE", 5, "Cube of pliable material") \
    X(NURSE, "NURSE", -1, "Menacing nurse with huge needle") \
    X(NEEDLE, "NEEDLE", 500, "Sickening amber solution [in the needle]") \
    X(PILLAR, "PILLAR", 500, "Large black pillar with a bronze F near the top") \
    X(BOX, "BOX", 500, "Small box with a slot") \
    X(CARD, "CARD", 8, "Card with a green F and a brown strip on it") \
    X(TRAIN, "TRAIN", 500, "Eighty-foot Froboliner train") \
    X(ART, "ART", 70, "Priceless painting of King Frobozz II")

typedef enum {
    NO_OBJECT,
#define X(o, s, w, d) o,
X_MACRO_OBJECTS
#undef X
} Object;

bool lookup_noun_helper(const char *input, const char *spelling, Object obj) {
    unsigned int n = strlen(spelling);
    if (n >= 5) {
        n = 5;
    } else if (strlen(input) != n) {
        // "GET LANTEFOO" is OK; "GET LAMPFOO" is not
        return false;
    }
    if (strncmp(input, spelling, n) == 0) {
        // The original game doesn't do this. It's nice to make
        // "GET LANTEFOO" print "Lantern: Taken" instead of
        // "Lantefoo: Taken"; so if the named object is present,
        // we'll aggressively substitute the correct spelling.
        // (But if the object isn't present, we won't.)
        //
        if (here(obj, loc)) {
            active_object = spelling;
        }
        return true;
    }
    return false;
}

Object lookup_noun(const char *word) {
    assert(word != NULL);
#define N(e, w) if (lookup_noun_helper(word, w, e)) return e;
    N(LAMP, "LAMP"); N(LAMP, "LANTERN"); N(LAMP, "LIGHT");
    N(KEYS, "KEYS"); N(KEYS, "SET"); N(KEYS, "IRON");
    N(CHEST, "CHEST"); N(CHEST, "TRUNK");
    N(JEWEL, "JEWEL"); N(JEWEL, "DIAMOND"); N(JEWEL, "GEM");
    N(GOLD, "GOLD"); N(GOLD, "NUGGET");
    N(TREE, "TREE"); N(TREE, "OAK"); N(TREE, "CARVINGS");
    N(WITCH, "WITCH"); N(WITCH, "HAG");
    N(LEVER, "LEVER");
    // The original game doesn't recognize BRIDE as a synonym for GIRL
    // except in certain codepaths: KISS, SCREW, and EAT.
    //
    // Instead of that logic, we'll check whether the player is married.
    N(GIRL, "GIRL"); N(GIRL, "WOMAN"); N(GIRL, "BRIDE");
    N(PEARL, "PEARL");
    N(NOTE, "NOTE");
    N(CELLO, "CELLO"); N(CELLO, "STRADIVARIUS");
    N(WATCH, "WATCH"); N(WATCH, "TIMEPIECE");
    N(PARCHMENT, "PARCHMENT"); N(PARCHMENT, "MAP");
    N(VASE, "VASE"); N(VASE, "MING");
    N(STAFF, "STAFF"); N(STAFF, "ROD"); N(STAFF, "WAND");
    N(COINS, "COINS"); N(COINS, "COIN");
    N(KNIFE, "KNIFE"); N(KNIFE, "BUTCHER");
    N(SLIME, "SLIME"); N(SLIME, "OOZE");
    N(STOOL, "STOOL"); N(STOOL, "FOOTSTOOL"); N(STOOL, "FOOT STOOL");
    N(FLASK, "FLASK"); N(FLASK, "BOTTLE");
    N(HILLS, "HILLS"); N(HILLS, "MINE"); N(HILLS, "MINES"); N(HILLS, "HILL"); N(HILLS, "DIRT");
    N(BEAST, "BEAST"); N(BEAST, "GORILLA");
    N(CYLINDER, "CYLINDER"); N(CYLINDER, "CRYSTAL"); N(CYLINDER, "WHITE");
    N(ZARKA, "ZARKA");
    N(GLOBE, "GLOBE"); N(GLOBE, "SPHERE"); N(GLOBE, "COATING");
    N(SNAKE, "SNAKE"); N(SNAKE, "VIPER");
    N(ROCKS, "ROCKS"); N(ROCKS, "STONES"); N(ROCKS, "ROCK");
    N(FRIES, "FRIES"); N(FRIES, "FRENCH THINGS");
    N(PLATE, "PLATE"); N(PLATE, "DINNER");
    N(FISH, "FISH"); N(FISH, "FILET"); N(FISH, "SANDWICH");
    N(CLOWN, "CLOWN");
    N(SPRAY, "SPRAY"); N(SPRAY, "DISINFECTANT"); N(SPRAY, "CAN");
    N(PIZZA, "PIZZA"); N(PIZZA, "SLICE");
    N(BED, "BED");
    N(JOHN, "JOHN"); N(JOHN, "TOILET");
    N(TUB, "TUB"); N(TUB, "BATHTUB"); N(TUB, "BATH");
    N(BOOK, "BOOK"); N(BOOK, "YELLOW");
    N(COMPUTER, "COMPUTER"); N(COMPUTER, "MACHINE"); N(COMPUTER, "TRINITY-3000");
    N(KNOB, "KNOB"); N(KNOB, "RED");
    N(LABEL, "LABEL"); N(LABEL, "SIGN"); N(LABEL, "TAN");
    N(RING, "RING"); N(RING, "EMERALD"); N(RING, "WEDDING");
    N(BLOCK, "BLOCK"); N(BLOCK, "BRICK"); N(BLOCK, "DRY ICE"); N(BLOCK, "ICE");
    // The original seems to have "H20" in place of "H2O"
    N(WATER, "WATER"); N(WATER, "H2O");
    N(DOOR, "DOOR"); N(DOOR, "WOODEN THING");
    N(GLOVE, "GLOVE"); N(GLOVE, "MITTEN"); N(GLOVE, "WORK");
    N(DISC, "DISC"); N(DISC, "DISK"); N(DISC, "SAUCER"); N(DISC, "SILVER");
    N(BOARD, "BOARD"); N(BOARD, "BULLETIN"); N(BOARD, "NOTICE");
    N(POST, "POST"); N(POST, "SIGNPOST");
    N(BUTTON, "BUTTON"); N(BUTTON, "KEY"); N(BUTTON, "BEIGE");
    N(LIFT, "LIFT"); N(LIFT, "TRAM"); N(LIFT, "CONTRAPTION"); N(LIFT, "GONDOLA");
    N(SPADE, "SPADE"); N(SPADE, "SHOVEL"); N(SPADE, "TOOL");
    N(STATUE, "STATUE"); N(STATUE, "JADE");
    N(COMB, "COMB"); N(COMB, "BRUSH"); N(COMB, "IVORY");
    N(SHARDS, "PIECES"); N(SHARDS, "SHARDS"); N(SHARDS, "POTTERY");
    N(SAPPHIRE, "SAPPHIRE");
    N(PLATFORM, "PLATFORM"); N(PLATFORM, "DAIS");
    N(GRATE, "GRATE"); N(GRATE, "GRATING");
    N(SLABS, "SLABS");
    N(WHEEL, "WHEEL");
    N(BOOTS, "BOOTS"); N(BOOTS, "BOOT"); N(BOOTS, "RUBBER");
    N(GUM, "CUBE"); N(GUM, "OXYGUM"); N(GUM, "GUM"); N(GUM, "MATERIAL");
    N(NURSE, "NURSE");
    N(NEEDLE, "NEEDLE"); N(NEEDLE, "AMBER"); N(NEEDLE, "SOLUTION");
    N(PILLAR, "PILLAR"); N(PILLAR, "BRONZE"); N(PILLAR, "F");
    N(BOX, "BOX"); N(BOX, "GATE"); N(BOX, "SLOT");
    N(CARD, "CARD"); N(CARD, "FARECARD"); N(CARD, "FARE");
    N(TRAIN, "TRAIN");
    N(ART, "ART"); N(ART, "PAINTING"); N(ART, "PICTURE");
    return NO_OBJECT;
}

int object_weight(Object obj) {
    if (obj == ZARKA) {
        // The Zarka becomes portable when it is fed.
        return (zarka_status == 0) ? -1 : 50;
    }
    switch (obj) {
#define X(o, s, w, d) case o: return w;
X_MACRO_OBJECTS
#undef X
    }
    assert(false);
}

const char *spelling_of(Object obj) {
    switch (obj) {
#define X(o, s, w, d) case o: return s;
X_MACRO_OBJECTS
#undef X
    }
    assert(false);
}

void describe_object(Object obj)
{
    switch (obj) {
#define X(o, s, w, d) case o: puts(d); break;
X_MACRO_OBJECTS
#undef X
    }
}

const char *lowercase(const char *s) {
    static char buf[16];
    strncpy(buf, s, 16);
    buf[15] = '\0';
    for (int i = 0; buf[i] != '\0'; ++i) {
        buf[i] = tolower(buf[i]);
    }
    static const char *capitalized_words[] = {
        "zarka", "strad", "stradivarius", "ming", "trinity", "trinity-3000"
    };
    for (int i=0; i < 6; ++i) {
        if (strcmp(buf, capitalized_words[i]) == 0) {
            buf[0] = toupper(buf[0]);
        }
    }
    return buf;
}

const char *Titlecase(const char *s) {
    static char buf[16];
    strncpy(buf, s, 16);
    buf[0] = toupper(buf[0]);
    for (int i = 1; i < 16; ++i) {
        buf[i] = tolower(buf[i]);
    }
    buf[15] = '\0';
    return buf;
}

bool prompt_for(char ch) {
    printf("> "); fflush(stdout);
    char buffer[130];
    fgets(buffer, sizeof buffer, stdin);
    return toupper(buffer[0]) == toupper(ch);
}

bool has_light(Location loc) {
    switch (loc) {
        case R_SMALL: case R_MAZE1: case R_MAZE2: case R_MAZE3: case R_CHEST:
        case R_INSIDE: case R_MYSTERY: case R_SEA: case R_CATACOMB: case R_TURKISH:
        case R_STORE: case R_TONE: case R_TIME: case R_SLIME: case R_SEWER:
        case R_EW: case R_COLD: case R_INDY: case R_CAVE: case R_CRAWL:
        case R_SEWER2:
            return false;
        default:
            return true;
    }
}

bool has_dirt(Location loc) {
    switch (loc) {
        case R_START: case R_TREE: case R_GARDEN: case R_GREEN: case R_FALL:
        case R_MINE: case R_VIPER: case R_YARD: case R_NORTH: case R_SANDY:
        case R_QUICK: case R_BASE: case R_INDY: case R_NBURGER:
            return true;
        default:
            return false;
    }
}

bool is_readable(Object obj) {
    switch (obj) {
        case TREE: case NOTE: case PARCHMENT: case GLOBE: case BOOK:
        case LABEL: case DISC: case BOARD: case POST:
            return true;
        default:
            return false;
    }
}

int current_burden() {
    // The original game tracks your current burden in L0;
    // but various places forget to update L0 when acquiring
    // or losing an object. Let's just recompute it every time.
    //
    int total = 0;
    for (int i = LAMP; i <= ART; ++i) {
        if (toting(i)) {
            assert(0 <= object_weight(i) && object_weight(i) < 500);
            total += object_weight(i);
        }
    }
    return total;
}

void die() {
#ifdef Z_MACHINE
    puts("\n\n");
#endif /* Z_MACHINE */
    exit(0);
}

// Line 2240 in advent.txt
void attempt_score() {
    int total = score;
    // Exclude the lamp from scoring, even though it weighs 5.
    for (int i = KEYS; i <= ART; ++i) {
        if (there(i, R_HOUSE)) {
            total += object_weight(i) + 10;
        } else if (toting(i)) {
            total += object_weight(i);
        }
    }
    total += (fucked * 69); // Nice.
    printf("Your score is %d out of a possible 1400 in %d turns.\n", total, turns);
    if (total < 200) {
        puts("This qualifies you as a Beginner Adventurer.");
    } else if (total < 400) {
        puts("This qualifies you as a Novice Adventurer.");
    } else if (total < 600) {
        puts("This qualifies you as an Intermediate Adventurer.");
    } else if (total < 800) {
        puts("This qualifies you as a Class IV Adventurer.");
    } else if (total < 1000) {
        puts("This qualifies you as an Advanced Adventurer.");
    } else if (total < 1200) {
        puts("This qualifies you as an Expert Adventurer.");
    } else {
        puts("This qualifies you as a Cheater Adventurer.");
    }
    puts("Do you wish to stop now?");
    if (!prompt_for('N')) {
        puts("Okay.");
        die();
    }
}

// Line 7360 in advent.txt
void extract_object() {
    // The parser simply assumes that the player has entered the verb
    // as expected, plus at most one fudge character, plus a space.
    // In other words, for verb "UNLOC", the regex is "UNLOC.[ ]*DOOR".
    //
    const char *noun = active_command + strlen(active_verb);
    if (*noun != '\0') noun += 1;
    while (*noun == ' ') noun += 1;
    active_object = (*noun == '\0') ? default_object : noun;
}

const char *get_direct_object(const char *verb) {
    printf("%s what? > ", verb); fflush(stdout);
    static char input[130];
    fgets(input, sizeof input, stdin);
    char *p = input;
    while (*p != ' ' && *p != '\n' && *p != '\0') {
        *p = toupper(*p);
        ++p;
    }
    *p = '\0';
    return (p != input) ? input : NULL;
}

const char *get_indirect_object(const char *prompt) {
    printf("%s: %s > ", Titlecase(active_object), prompt); fflush(stdout);
    static char input[130];
    fgets(input, sizeof input, stdin);
    char *p = input;
    while (*p != ' ' && *p != '\n' && *p != '\0') {
        *p = toupper(*p);
        ++p;
    }
    *p = '\0';
    return (p != input) ? input : NULL;
}

void initialize_rooms()
{
#define new_place(loc, sdesc, ldesc) \
    ++i; assert(i == loc); \
    places[i].long_desc = ldesc; places[i].short_desc = sdesc
#define n_to(loc) places[i].neighbors[0] = loc
#define s_to(loc) places[i].neighbors[1] = loc
#define e_to(loc) places[i].neighbors[2] = loc
#define w_to(loc) places[i].neighbors[3] = loc
#define u_to(loc) places[i].neighbors[4] = loc
#define d_to(loc) places[i].neighbors[5] = loc
#define i_to(loc) places[i].neighbors[6] = loc
#define o_to(loc) places[i].neighbors[7] = loc
#define object(obj) apport(obj, i);

    Location i = 0;
    new_place(R_START, "Meadow",
        "You're in a vast meadow that stretches endlessly in all directions." SOFT_NL
        "To the west is a tree."
    ); n_to(R_START); s_to(R_START); e_to(R_START); w_to(R_TREE);
    new_place(R_TREE, "Meadow near tree",
        "You're in the meadow, underneath a large oak tree.  There are small" SOFT_NL
        "carvings on the tree, and they seem to mean something."
    ); n_to(R_START); s_to(R_START); e_to(R_START); w_to(R_START); u_to(R_HOUSE); object(TREE);
    new_place(R_HOUSE, "Treehouse",
        "You're in a treehouse, barren of furnishings except a cubicle to the south," SOFT_NL
        "which is filled with shimmering green light."
    ); s_to(R_CUBIC1); d_to(R_TREE); i_to(R_CUBIC1); object(LAMP);
    new_place(R_CUBIC1, "Cubicle",
        "You're in a cubicle. A green glow surrounds you on all sides," SOFT_NL
        "but it is impossible to see the source of it."
    ); n_to(R_HOUSE); o_to(R_HOUSE); object(LEVER);
    new_place(R_SMALL, "Small Room",
        "You are in a small, dark room with a passage to the north and a" SOFT_NL
        "set of ascending stone stairs to the east."
    ); n_to(R_CHEST); e_to(R_TURKISH); u_to(R_TURKISH);
    new_place(R_MAZE1, "Maze",
        "You are in a twisting maze of little passages, all alike."
    ); e_to(R_MAZE1); w_to(R_MAZE2); u_to(R_MAZE1); d_to(R_MAZE1); object(KEYS);
    new_place(R_MAZE2, "Maze",
        "You are in a little maze of twisty passages, all alike."
    ); n_to(R_MAZE2); s_to(R_MAZE3); e_to(R_MAZE2); u_to(R_STROBE); d_to(R_MAZE2);
    new_place(R_MAZE3, "Maze",
        "You are in a twisty maze of little passages, all alike."
    ); n_to(R_MAZE2); s_to(R_MAZE3); w_to(R_MAZE3); u_to(R_MAZE3); d_to(R_MYSTERY);
    new_place(R_CHEST, "Chest Room",
        "You are in a very dusty room with a teakwood floor.  You may leave this" SOFT_NL
        "room to the north and south."
    ); n_to(R_MAZE1); s_to(R_SMALL); object(CHEST);
    new_place(R_INSIDE, "Inside Chest",
        "You are in a very small room with oak walls, and it's a little cramped."
    ); object(GOLD); n_to(R_MAZE1); s_to(R_SMALL);
    new_place(R_MYSTERY, "Mystery Room",
        "You are in the Mystery Room, a very lovely room with exits to the" SOFT_NL
        "east, west and south.  The hole in the ceiling is too far up to use."
    ); s_to(R_SLIME); e_to(R_TIME); w_to(R_SEA); object(WITCH);
    new_place(R_SEA, "Hall of the Sea",
        "You have entered the Hall of the Sea, a room filled with blue-green" SOFT_NL
        "mists.  Mysterious shadows seem to dance upon the walls as your Lamp" SOFT_NL
        "sheds light through the eerie colored fog.  Exits are east and west."
    ); e_to(R_MYSTERY); w_to(R_CATACOMB); object(PEARL);
    new_place(R_CATACOMB, "Catacomb",
        "You're in a damp, somewhat musty chamber that appears to be an" SOFT_NL
        "ancient catacomb.  There is a passage to the east."
    ); e_to(R_SEA); object(NOTE);
    new_place(R_TURKISH, "Turkish Bath",
        "You're in the Turkish Bath.  Soothing mist surrounds you.  You may go" SOFT_NL
        "down stone stairs to the west, or up a metal ramp to the east."
    ); e_to(R_GARDEN); w_to(R_SMALL); u_to(R_GARDEN); d_to(R_SMALL);
    new_place(R_GARDEN, "Garden",
        "You have entered a small garden.  The soil is very loose here.  To the west" SOFT_NL
        "is a metal ramp that leads downward; to the north is a large glass building."
    ); n_to(R_GREEN); w_to(R_TURKISH); d_to(R_TURKISH); object(VASE);
    new_place(R_GREEN, "Greenhouse",
        "You're in an empty glass building that seems to have been used for a" SOFT_NL
        "greenhouse.  To the north is a house; to the south is a garden; to the" SOFT_NL
        "west is a store room."
    ); n_to(R_LIVING); s_to(R_GARDEN); w_to(R_STORE);
    new_place(R_STORE, "Store Room",
        "This is a very tiny store room that has a large hole in the floor on" SOFT_NL
        "the western side of the room, and an exit to the east."
    ); e_to(R_GREEN); w_to(R_TONE); d_to(R_TONE); object(STOOL);
    new_place(R_TONE, "Hall of Tones",
        "You're in the Hall of Tones.  Pleasant tunes can be heard as you decide" SOFT_NL
        "whether to go west, or up steep steps to the east."
    ); e_to(R_STORE); w_to(R_TIME); u_to(R_STORE); object(CELLO);
    new_place(R_TIME, "Chamber of Time",
        "You are in a very beautiful room, the Chamber of Time.  Exits are to the" SOFT_NL
        "east and west."
    ); e_to(R_TONE); w_to(R_MYSTERY); object(WATCH);
    new_place(R_SLIME, "Slime Room",
        "This room's walls are covered with a mixture of blood and slimy ooze." SOFT_NL
        "Exits: North and south."
    ); n_to(R_MYSTERY); s_to(R_FALL); object(SLIME);
    new_place(R_LIBRARY, "Library",
        "You're in a library.  The only apparent exists to the room are: a passage" SOFT_NL
        "to the south and an archway to the east."
    ); s_to(R_MAZE1); e_to(R_KITCHEN); object(BOOK);
    new_place(R_KITCHEN, "Kitchen",
        "You're in a kitchen.  There are two exits, one to the west and one to" SOFT_NL
        "the south.  The scent of food is in the air."
    ); s_to(R_LIVING); w_to(R_LIBRARY); object(KNIFE); object(PIZZA);
    new_place(R_LIVING, "Living Room",
        "You are in the living room of the house.  Exits: North, south, east, and" SOFT_NL
        "a very narrow passage to the west."
    ); n_to(R_KITCHEN); s_to(R_GREEN); e_to(R_FOYER); w_to(R_MAZE1); object(PARCHMENT);
    new_place(R_FOYER, "Foyer",
        "You are in a small foyer, barren of any furnishings, except for a large" SOFT_NL
        "glass cubicle to the south, which is filled with shimmering red light." SOFT_NL
        "Exits: East and west, and a northern exit leading upstairs."
    ); n_to(R_HALL); s_to(R_CUBIC2); e_to(R_YARD); w_to(R_LIVING); u_to(R_HALL); i_to(R_CUBIC2);
    new_place(R_CUBIC2, "Cubicle",
        "You are in a glass cubicle.  Surrounding you is a shimmering red glow," SOFT_NL
        "but it is impossible to see the source of it."
    ); n_to(R_FOYER); o_to(R_FOYER); object(STAFF);
    new_place(R_FALL, "Waterfall",
        "You're in a cool room with a towering waterfall filling up the eastern" SOFT_NL
        "part of the room.  You're on a path that bends north and west, and there's" SOFT_NL
        "a glass cubicle to the south, which is filled with shimmering blue light."
    ); n_to(R_SLIME); s_to(R_CUBIC3); w_to(R_MINE); i_to(R_CUBIC3); object(WATER);
    new_place(R_MINE, "Mine Field",
        "This room is only five feet high, and has exits to the south and east."
    ); s_to(R_SEWER); e_to(R_FALL); object(HILLS);
    new_place(R_CUBIC3, "Cubicle",
        "You are in a glass cubicle.  Surrounding you is a shimmering blue glow," SOFT_NL
        "but it is impossible to see the source of it."
    ); n_to(R_FALL); o_to(R_FALL);
    new_place(R_SEWER, "Sewer Pipe",
        "You are at the northern end of a very long sewer pipe.  To the north" SOFT_NL
        "is a low room, and there are numerous holes in the floor of the pipe."
    ); n_to(R_MINE); s_to(R_SEWER2); d_to(R_TREASURE); object(BEAST);
    new_place(R_TREASURE, "Treasure Room",
        "This giant room is the home of the most valuable treasure in the cave." SOFT_NL
        "In the direct center of the room, a sphere of great beauty was placed" SOFT_NL
        "many years ago.  The only exits to this room are the stairs above you."
    ); u_to(R_SEWER); /* w_to(R_EW) if Zarka is gone */ object(GLOBE);
    new_place(R_VIPER, "Viper Pit",
        "You're in a deep pit.  The only exit is straight up."
    ); n_to(R_YARD); u_to(R_YARD); o_to(R_YARD); object(SNAKE);
    new_place(R_YARD, "Front Yard",
        "You're in the front yard of the house. To the west is a door, to the" SOFT_NL
        "south a dark hole, and meadow everywhere else."
    ); n_to(R_NORTH); s_to(R_VIPER); e_to(R_START); w_to(R_FOYER); d_to(R_VIPER); i_to(R_FOYER);
    new_place(R_NORTH, "North of House",
        "You're in a meadow, north of a house.  Far to the north, a tiny oasis" SOFT_NL
        "is visible."
    ); n_to(R_BURGER); s_to(R_YARD); e_to(R_START); w_to(R_START);
    new_place(R_BURGER, "Burgerland",
        "You're in Burgerland!  A meadow surrounds this tiny oasis, which has" SOFT_NL
        "no apparent entrance."
    ); n_to(R_STATION1); s_to(R_NORTH); e_to(R_START); w_to(R_START); object(PLATE); object(FISH); object(CLOWN);
    new_place(R_HALL, "Hallway",
        "You're in a hallway.  There are various openings to the north and west," SOFT_NL
        "and two stairways: one leading up to the east and one down to the foyer."
    ); n_to(R_SITTING); e_to(R_ATTIC); w_to(R_BATH); u_to(R_ATTIC); d_to(R_FOYER);
    new_place(R_ATTIC, "Attic",
        "You're in the attic.  The only exit is a stairway leading downward."
    ); d_to(R_HALL); object(GLOVE);
    new_place(R_BATH, "Bathroom",
        "You're in and old-fashioned bathroom, a very tidy place, with exits to" SOFT_NL
        "the north and east,"
    ); n_to(R_BED); e_to(R_HALL); object(SPRAY); object(JOHN); object(TUB);
    new_place(R_BED, "Bedroom",
        "You're in a large room, probably the master bedroom.  There is an exit" SOFT_NL
        "to the south, and one to the east,"
    ); s_to(R_BATH); e_to(R_SITTING); object(BED); object(CARD);
    new_place(R_SITTING, "Sitting Room",
        "You're in the sitting room.  Exits: west and south, and a cubicle to the" SOFT_NL
        "east, filled with shimmering purple light."
    ); s_to(R_HALL); e_to(R_CUBIC4); w_to(R_BED); i_to(R_CUBIC4); object(COMB);
    new_place(R_CUBIC4, "Cubicle",
        "You're in a cubicle.  A purple glow surrounds you, but you cannot see the" SOFT_NL
        "source of it."
    ); w_to(R_SITTING); o_to(R_SITTING);
    new_place(R_EW, "East-West Passage",
        "You're in a very tight east-west passageway.  There is also a small hole" SOFT_NL
        "in the floor on the northern side of the passage.  A freezing wind blows" SOFT_NL
        "out of the hole."
    ); n_to(R_COLD); e_to(R_TREASURE); w_to(R_SANDY); d_to(R_COLD);
    new_place(R_COLD, "Cold Room",
        "You're in an extremely cold room.  Icicles hang from the ceiling, and near" SOFT_NL
        "a particularly large icicle in the southern part of the room, is a hole."
    ); s_to(R_EW); u_to(R_EW); object(BLOCK);
    new_place(R_SANDY, "Sandy Shore",
        "You're on a sandy shore.  To the west is some more sand, but it seems" SOFT_NL
        "to be mixed with water.  There is a tight passage leading east."
    ); e_to(R_EW); w_to(R_QUICK);
    new_place(R_QUICK, "Quicksand",
        "You're in a pool of sand.  To the west you can see a path leading outside," SOFT_NL
        "and to the east, a shore."
    ); e_to(R_SANDY); w_to(R_ROCKY);
    new_place(R_ROCKY, "Rocky Path",
        "The floor here is quite rocky, making your footing quite unsure.  You're" SOFT_NL
        "on a path with a lighted passage to the south and a dark tunnel to the north." SOFT_NL
        "To the east is a somewhat soupy pool of sand."
    ); n_to(R_INDY); s_to(R_BASE); e_to(R_QUICK);
    new_place(R_BASE, "Base of Mountain",
        "You're at the base of a mountain.  It is snowing lightly here, partially" SOFT_NL
        "covering an assortment of heavy wires.  To the north is a cave leading" SOFT_NL
        "underground."
    ); n_to(R_ROCKY); object(POST); object(BUTTON); object(LIFT);
    new_place(R_INDY, "Archaeology Room",
        "You're in a room filled with cobwebs.  There appear to have been recent" SOFT_NL
        "archaeological diggings in this area.  Passages exit to the north and south."
    ); n_to(R_CHAPEL); s_to(R_ROCKY); object(SPADE);
    new_place(R_CHAPEL, "Chapel",
        "You're in a small, but elaborately decorated chapel.  There is a large" SOFT_NL
        "machine in one corner of the room, and it is covered with dust." SOFT_NL
        "The only exit to the room is an archway to the south."
    ); s_to(R_ROCKY); object(COMPUTER); object(KNOB); object(LABEL);
    new_place(R_SUMMIT, "Summit of Mountain",
        "You're at the summit of Mount Frobozz.  The eastern slope is impassable," SOFT_NL
        "hut there are trails to the north and south.  An inviting, cozy lighted" SOFT_NL
        "shack is to the west.  It is snowing quite heavily here."
    ); n_to(R_EXPERT); s_to(R_BUNNY); w_to(R_SHACK); i_to(R_SHACK); object(BOARD);
    new_place(R_EXPERT, "Expert Trail",
        "You're at the top of the Expert Slope, a sheet of ice plunging downward" SOFT_NL
        "out of sight at an unpleasant angle.  A small snowy trail leads away south."
    ); s_to(R_SUMMIT);
    new_place(R_BUNNY, "Bunny Slope",
        "You're at the top of the Bunny Slope!  It seems as though a child would" SOFT_NL
        "have no problem skiing down, although walking down is out of the question." SOFT_NL
        "A path leads away to the north."
    ); n_to(R_SUMMIT);
    new_place(R_SHACK, "Ski Shack",
        "You are in a comfortable ski shack, apparently a shelter from the cold" SOFT_NL
        "weather outside.  There's a rickety staircase leading downward in this building."
    ); e_to(R_SUMMIT); d_to(R_CAVE); o_to(R_SUMMIT); object(DISC);
    new_place(R_CAVE, "Cave",
        "You're in a dark cave with a rocky floor.  There is a stairway leading up" SOFT_NL
        "and an eastern passage."
    ); e_to(R_CRAWL); u_to(R_SHACK); object(JEWEL);
    new_place(R_CRAWL, "East-West Crawl",
        "You're crawling over rocky cobbles in an east-west passage.  At the eastern" SOFT_NL
        "end is a formidable wooden door, which is open."
    ); e_to(R_CATACOMB); w_to(R_CAVE); object(DOOR);
    // Yes, the conspicuous door is a one-way passage to R_CATACOMB. It is thus in the
    // original game (Guma's Adventure II); I don't know what it was like in Adventure ]I[.
    new_place(R_STROBE, "Strobe Room",
        "You are in a very peculiar room with no apparent exits.  There is an unusual" SOFT_NL
        "flashing light that fills the room, seemingly coming from far above you."
    ); d_to(R_MAZE2); object(GUM);
    new_place(R_SEWER2, "Sewer Pipe",
        "You're in a very long, north-south sewer pipe.  You can hear a strange," SOFT_NL
        "distant, echoing sound, apparently an animal humming to himself."
    ); n_to(R_SEWER); s_to(R_SEWER2);
    new_place(R_WET, "Wet Room",
        "You're in a very wet room on the eastern side of an immense waterfall." SOFT_NL
        "There are openings to the north and south, and a small room to the east."
    ); n_to(R_LAB); s_to(R_RROOM); e_to(R_CLOSET);
    new_place(R_RROOM, "Reactor Room",
        "You're in a warm room with an exit to the north.  The walls are covered" SOFT_NL
        "with large metal slabs, apparently for insulation purposes.  In the center" SOFT_NL
        "of the room is a low platform.  A bright orange glow emanates from a small" SOFT_NL
        "grating on the southern wall."
    ); n_to(R_WET); /* s_to(R_RCORE) if grate is gone */ object(PLATFORM); object(GRATE); object(SLABS); object(WHEEL);
    new_place(R_LAB, "Laboratory",
        "You're in a mysterious little room that appears to have been a scientific" SOFT_NL
        "laboratory, judging from the assortment of junk around the room.  Exits" SOFT_NL
        "are an open passage to the south and an odd thin hole to the east."
    ); s_to(R_WET); e_to(R_HOSPITAL);
    new_place(R_CLOSET, "Closet",
        "You're in a small closet, perhaps used for storage of scientific supplies." SOFT_NL
        "The exit is to the west."
    ); w_to(R_WET); object(BOOTS);
    new_place(R_RCORE, "Reactor",
        "You are inside the reactor core, a large, circular room with a reinforced" SOFT_NL
        "spherical ceiling.  The blinding glare from the reactor makes it difficult" SOFT_NL
        "to make out any other features of the room.  There is an access grating" SOFT_NL
        "on the northern wall."
    ); n_to(R_RROOM); object(SAPPHIRE);
    new_place(R_HOSPITAL, "Hospital Room",
        "You're in a spotless white room, devoid of any furnishings or windows." SOFT_NL
        "Although there is no visible heat source in the room, the air is very" SOFT_NL
        "warm.  The most interesting feature of this room is the peculiar exit:" SOFT_NL
        "a tall thin hole on the western wall."
    ); w_to(R_LAB); object(NURSE); object(NEEDLE);
    new_place(R_NBURGER, "Burgerland North",
        "You're in a grassy arena north of Burgerland.  There is what appears to have" SOFT_NL
        "been an escalator (now inoperative) leading downward here."
    ); n_to(R_START); s_to(R_BURGER); e_to(R_START); w_to(R_START); d_to(R_STATION1); object(PILLAR);
    new_place(R_STATION1, "Burgerland Station",
        "You're standing in a very long, cylindrical room.  From the objects about" SOFT_NL
        "the room, it would appear that this is a subway station.  Beyond a gate" SOFT_NL
        "you can see the open doorway of an abandoned train."
    ); u_to(R_NBURGER);
    new_place(R_TRAIN, "Train",
        "You're in a Froboliner subway train."
    );
    new_place(R_STATION2, "Museum Station",
        "You're standing in a very long, cylindrical room.  From the objects about" SOFT_NL
        "the room, it would appear that this is a subway station.  Beyond a gate" SOFT_NL
        "you can see the open doorway of an abandoned train."
    ); u_to(R_ART);
    new_place(R_ART, "Art Museum",
        "You are in a museum named for the great leader of Erehwon, King Frobozz II." SOFT_NL
        "The only exit is down to Museum Station."
    ); d_to(R_STATION2); object(ART);
}

void attempt_help() {
    puts("");
    puts("Somewhere nearby is the land of Erehwon, a mysterious place");
    puts("filled with fantasy and magic.  It is your task to explore this");
    puts("unknown region, requiring your physical skill and cunning to");
    puts("be at their peak.  I will be your eyes and ears in this exped-");
    puts("ition, describing your surroundings and any points of interest");
    puts("that you happen to run across.  Direct me with commands of one");
    puts("or two words.  To move from one place to another, try words like");
    puts("NORTH, WEST, CLIMB, DOWN.  To get a list of your current possessions");
    puts("use the word INVENTORY.  All words, objects, etc. may be abbrev-");
    puts("iated to their first five letters; for example, type UNLOC for");
    puts("UNLOCK, INVEN for INVENTORY, and so forth.");
    puts("");
    puts("The object of the game is to collect all of the valuables and to");
    puts("put them in the treehouse (you will find the treehouse soon).");
    puts("Special words that do not affect the game but may help you:");
    puts("");
    puts("BRIEF:  Prints a long description of each place you come to");
    puts("        the first time, then only the name of the place there-");
    puts("        after.");
    puts("LONG:   Prints a long description of your location every time");
    puts("        you move from place to place.");
    puts("SUPER:  Stands for SUPERBRIEF; means to print only the name");
    puts("        of your location when you move.");
    puts("LOOK:   Gives you a long description of your location no matter");
    puts("        which of the three print modes (SUPER, LONG, BRIEF)");
    puts("        was_specified.");
    puts("");
    puts("You may manipulate special objects in Erehwon, such as a lamp");
    puts("you will find in the treehouse.  Manipulation words include:");
    puts("TAKE, PUSH, EAT, KILL, WAVE.  (There are MANY more!)");
    puts("");
    puts("If you are having trouble moving or using an object, try");
    puts("anything that comes to mind, not just the commands listed here.");
    puts("Multiple commands on a line are separated thus:  N. S. E. W. EAT TREE.");
    puts("");
    puts("More help will be available here at a later time as the game");
    puts("develops further.");
}

void apport_schizoid_objects() {
    // For the lever, ski lift, etc., rather than make a new kind of
    // "schizoid" object in Adventure's style, the original game
    // simply describes the object regardless of its true location.
    // This has the disadvantage that e.g. "GET LEVER" works only in
    // R_CUBIC1, and reports "I see no lever here" in the other three cubicles.
    // It also means that LEVER is listed *after* LAMP, KEYS, etc. in
    // R_CUBIC1, but *before* LAMP, KEYS, etc. in R_CUBIC2.
    // For this port, we'll just teleport the object into the current
    // room whenever the player moves.
    //
    if (loc == R_CUBIC1 || loc == R_CUBIC2 || loc == R_CUBIC3 || loc == R_CUBIC4) {
        apport(LEVER, loc);
    } else if (loc == R_STATION1 || loc == R_STATION2) {
        apport(BOX, loc);
        apport(TRAIN, loc);
    } else if (loc == R_BASE || loc == R_SUMMIT) {
        apport(BOARD, loc);
        apport(BUTTON, loc);
        apport(LIFT, loc);
    }
}

void move_xeener_bugs() {
    // Alex Guma's "Adventure II" sources make this a 2/3 chance.
    // Rick Hammerstone's "Adventure ]I[" sources make it 9/10.
    // We'll go even lower, to 1/3.
    if (has_light(loc)) {
        // Do nothing.
    } else if (lamp_is_lit && here(LAMP, loc)) {
        // Do nothing.
        // (The original game doesn't require the lamp to be here, just lit.)
    } else if (ran(3) == 1) {
        puts("Oh no!!! Wild xeener bugs just ate your head!");
        die();
    } else {
        puts("Wild xeener bugs are approaching!");
    }
}

// Line 1350 in advent.txt
void describe_room() {
    // The original game seems to increase S by 1 (giving the player a point)
    // each time they pass through this codepath. That feels wrong, so we
    // don't do that.
    // Notice that darkness doesn't affect your ability to observe the room,
    // merely the chance of xeener bugs showing up (above).
    //
    bool print_long_description =
        really_look ||
        (brief_mode == 2) ||
        (brief_mode == 0 && !places[loc].visited);
    if (print_long_description) {
        places[loc].visited = 1;
        puts(places[loc].long_desc);
        if (places[loc].hole_depth > 0) {
            puts("Someone has been digging a hole here.");
        }
        really_look = false;
    } else {
        puts(places[loc].short_desc);
    }

    apport_schizoid_objects();
    default_object = NULL;
    bool seen_something = false;
    for (Object i = LAMP; i <= ART; ++i) {
        if (there(i, loc)) {
            if (!seen_something) {
                puts("I see:");
            }
            puts("  "); describe_object(i);
            default_object = spelling_of(i);
            seen_something = true;
        }
    }

    // We're planning to update timers again on the next turn (this
    // codepath drops straight into line 1830 in the original code),
    // so, ensure that (oldloc != loc) as if we'd successfully moved.
    oldloc = R_LIMBO;
}

void reactor_countdown(int t) {
    switch (t) {
        case 200: puts("An EPA official runs by muttering, \"Tsk, tsk.  Another TMI.\""); break;
        case 215: puts("A calm, pleasant voice reports, \"Meltdown...4 minutes.  Warning only.\""); break;
        case 216: puts("There are now 3 minutes 57 seconds until meltdown."); break;
        case 217: puts("There are now 2 minutes 9 seconds until meltdown."); break;
    }
    if (218 <= t && t <= 244) {
        int arr[] = {
            90, 69, 56, 47, 41, 35, 31, 28, 25, 22,
            20, 18, 16, 15, 13, 12, 11, 10, 9, 8,
            7, 6, 5, 4, 3, 2, 1,
        };
        printf("There are now %d seconds until meltdown.\n", arr[t-218]);
    } else if (t == 245) {
        // The original game seems to have exploded the reactor at 240 turns,
        // when the countdown timer was still at 6 seconds. We let it go to zero.
        puts("Booooom!!!"); // and two bell characters
        die();
    }
}

// Line 1830 in advent.txt
void update_timers_and_look()
{
    turns += 1;
    if (loc != oldloc) {
        move_xeener_bugs();
    }
    if ((loc != oldloc) || really_look) {
        describe_room();
        if (reactor_is_active) {
            reactor_countdown(turns);
        }
        if (lamp_is_lit) {
            lamp_power -= 1;
            if (lamp_power == 25) {
                puts("The lamp is getting dim.");
            } else if (lamp_power == 0) {
                puts("The lamp's batteries are dead.");
                lamp_is_lit = false;
            }
        }
        if (quicksand_timer != 0) {
            quicksand_timer -= 1;
        }
    }
    if (loc == R_QUICK && quicksand_timer != 0) {
        puts("You are sinking in quicksand.");
    }
    oldloc = loc;
}

// Line 2460 in advent.txt
void attempt_take() {
    extract_object();
    if (active_object == NULL) {
        // Line 2940
        for (Object i = LAMP; i <= ART; ++i) {
            if (there(i, loc)) {
                active_object = spelling_of(i);
                break;
            }
        }
    }
    if (active_object == NULL) {
        puts("I see nothing to take.");
    } else {
        Object obj = lookup_noun(active_object);
        if (obj == NO_OBJECT || !there(obj, loc)) {
            printf("I see no %s here.\n", lowercase(active_object));
        } else if (object_weight(obj) == -1) {
            printf("Not only is taking the %s illegal, it is also very dangerous!\n", lowercase(active_object));
        } else if (object_weight(obj) == 500) {
            switch (ran(4)) {
                case 0: printf("%s: A valiant attempt.\n", Titlecase(active_object)); break;
                case 1: printf("%s: Want a hernia?\n", Titlecase(active_object)); break;
                case 2: printf("%s: Close but no cigar.\n", Titlecase(active_object)); break;
                case 3: printf("%s: Nice try.\n", Titlecase(active_object)); break;
            }
        } else if (obj == BLOCK && !toting(GLOVE)) {
            puts("The dry ice instantly freezes your hand, and you die from shock.");
            die();
        } else if (obj == GLOBE && there(ZARKA, R_LIMBO)) {
            puts("As you reach for the globe, an alarm rings briefly, and a horrible" SOFT_NL
                 "Zarka appears from inside to defend it!");
            apport(ZARKA, loc);
            default_object = spelling_of(ZARKA);
        } else if (obj == GLOBE && zarka_status == 0 && there(GLOBE, loc)) {
            puts("The Zarka won't let you have the globe.");
        } else {
            if (obj == NOTE && there(ROCKS, R_LIMBO)) {
                puts("Avalanche!  Huge boulders!  Aaaaaah!");
                apport(ROCKS, loc);
            }
            if (current_burden() + object_weight(obj) > 150) {
                puts("Your load is too heavy.  You'll have to drop something first.");
            } else {
                apport(obj, R_INHAND);
                printf("%s: Taken.\n", Titlecase(active_object));
            }
        }
    }
}

// Line 3060 in advent.txt
void attempt_drop() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Drop");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == NO_OBJECT) {
        printf("You don't have the %s.\n", lowercase(active_object));
    } else {
        active_object = spelling_of(obj);
        if (!toting(obj)) {
            printf("You don't have the %s.\n", lowercase(active_object));
        } else if (obj == VASE && !there(STOOL, loc)) {
            puts("The vase falls to the floor and smashes into a million pieces!");
            apport(VASE, R_LIMBO);
            apport(SHARDS, loc);
        } else {
            puts("Dropped.");
            apport(obj, loc);
            if (loc == R_QUICK && obj == BLOCK) {
                puts("As the block sinks, the quicksand instantly freezes over hard enough" SOFT_NL
                     "to support your weight.");
                apport(BLOCK, R_LIMBO);
                quicksand_timer = 100;
            } else if (obj == GLOVE && toting(BLOCK)) {
                puts("The dry ice instantly freezes your hand, and you die from shock.");
                die();
            }
        }
    }
}

// Line 1220 in adven2.txt
void attempt_eat(Object obj) {
    if (obj == NO_OBJECT) {
        extract_object();
        if (active_object == NULL) {
            active_object = get_direct_object("Eat");
            if (active_object == NULL) {
                return;
            }
        }
        obj = lookup_noun(active_object);
    }
    switch (obj) {
        case TREE:
            // The original game takes this codepath only for "TREE",
            // not "OAK" or "CARVINGS".
            if (loc == R_TREE) {
                puts("You are not Yule Gibbons!"); // [sic]
            } else {
                printf("You don't have the %s.\n", lowercase(active_object));
            }
            break;
        case GUM:
            // The original game takes this codepath only for "GUM",
            // not "OXYGUM" or "MATERIAL".
            if (toting(GUM)) {
                // The original game forgets to lower your score here.
                score += object_weight(GUM);
                apport(GUM, R_LIMBO);
                puts("Somewhat chewy and refreshing.");
            } else {
                printf("You don't have the %s.\n", lowercase(active_object));
            }
            break;
        case FRIES:
        case FISH:
        case PIZZA:
            if (toting(obj)) {
                // The original game forgets to destroy the fries.
                apport(obj, R_LIMBO);
                puts("Yummy.");
            } else {
                printf("You don't have the %s.\n", lowercase(active_object));
            }
            break;
        case SLIME:
        case ZARKA:
        case CLOWN:
        case SNAKE:
            printf("The %s will become angry if you persist!\n", lowercase(active_object));
            break;
        case WITCH:
        case GIRL:
        case NURSE:
            puts("This isn't that kind of game!");
            break;
        default:
            puts("I don't see how to eat that!");
            break;
    }
}

// Line 3540 in advent.txt
void attempt_light() {
    if (!toting(LAMP)) {
        puts("You don't have the lamp.");
    } else if (lamp_is_lit) {
        puts("The lamp is already lit.");
    } else if (lamp_power == 0) {
        puts("The batteries are drained.");
    } else {
        puts("Lighted.");
        lamp_is_lit = true;
    }
}

// Line 3710 in advent.txt
void attempt_douse() {
    if (!toting(LAMP)) {
        puts("You don't have the lamp.");
    } else if (!lamp_is_lit) {
        puts("The lamp is already off.");
    } else {
        puts("Lamp: Turned off.");
        lamp_is_lit = false;
    }
}

// adread.txt
void attempt_read() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Read");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == NO_OBJECT || !here(obj, loc)) {
        printf("I can't find any %s here!\n", lowercase(active_object));
    } else if (!is_readable(obj)) {
        if (active_object == default_object) {
            printf("%s: ", Titlecase(active_object));
        }
        printf("I don't know how to read the %s.\n", lowercase(active_object));
    } else {
        if (active_object == default_object) {
            printf("%s:\n", Titlecase(active_object));
        }
        if (obj == TREE) {
            puts("The carvings say:   --> Morton's treehouse -- Six metres above <--");
        } else if (obj == NOTE) {
            puts("The note says:\n");
            puts("    Thou who hast desecrated the Hallowed Tomb of");
            puts("    Malak Fa'ar shall be forever plagued by");
            puts("    his deadly curse!!");
            puts("At the bottom is scribbled the word:\n");
            puts("    ARGYLE");
        } else if (obj == PARCHMENT) {
            puts("The parchment contains words and sketches:\n");
            puts("              ------------------------");
            puts("              | Le Jardin       /    |");
            puts("              |                /     |");
            puts("              |               /      |");
            puts("              |  La Route -> /       |");
            puts("              |             /        |");
            puts("              |   ----------         |");
            puts("              |  /                   |");
            puts("              |_/          \\ /       |");
            puts("              |             X        |");
            puts("              |            / \\       |");
            puts("              ------------------------");
        } else if (obj == GLOBE) {
            puts("    This Globe is protected internally by");
            puts("       FROBOZZ MAGIC SECURITY SYSTEM");
            puts("    *************************************");
        } else if (obj == BOOK) {
            puts("                  Care and Feeding of Your Zarka");
            puts("                      Copyright (C) 1999 by\n");
            puts("                    FROBOZZ MAGIC BOOK COMPANY.");
            puts("                      All rights reserved.\n");
            puts("               Zarkas need a daily supply of water,");
            puts("               and like a good combing every week.");
            puts("               They eat very little, but when they");
            puts("               are hungry, they are very picky.\n");
            puts("               Most zarkas will eat only pizza, and");
            puts("               this preference was probably estab-");
            puts("               lished by the unusual climate in which");
            puts("               they live.  Zarkas always enjoy a day");
            puts("               outside, which helps to make their");
            puts("               reddish-purple fur shiny.  Because of");
            puts("               their size, they can be found anywhere,");
            puts("               particularly near caves, usually in");
            puts("               an enclosed place.  They love precious");
            puts("               metals.  Zarkas have an interesting");
            puts("               way of reproducing: they find anoth...\n");
            puts("The printing has faded with age, and becomes undecipherable here.");
        } else if (obj == LABEL) {
            puts("               This is the TRINITY/3000 Automatic Coupler");
            puts("                       Pull knob for directions...");
            puts("                 Mfgd. by FROBOZZ MAGIC COMPUTER COMPANY");
        } else if (obj == DISC) {
            puts("              FROBOZZ MAGIC SNO-DISC COMPANY");
            puts("              Instructions for use:\n");
            puts("              (1) First, BOARD the disc, and");
            puts("              (2) Second, LAUNCH it!\n");
            puts("This SNO-DISC is guaranteed for 3.45 seconds from date of purchase,");
            puts("whichever comes first.   Happy landing!");
        } else if (obj == BOARD) {
            puts("                        MOUNT FROBOZZ SKI RESORT");
            puts("                           Ski lodge ^ (west)");
            puts("              <- Bunny Slope (south)   Expert Slope (north) ->");
        } else if (obj == POST) {
            puts("   Remove Pole Straps From Wrists");
            puts("         Do Not Swing Cars");
            puts("   Do Not Throw Objects From Lift");
            puts("See Attendant Should Lift Malfunction");
        } else {
            assert(false);
        }
    }
}

// Line 3810 in advent.txt
void attempt_kiss() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Kiss");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == NO_OBJECT && strcmp(active_object, "BRIDE") == 0) {
        if (married) {
            obj = GIRL;
        } else {
            printf("I see no bride here.\n");
            return;
        }
    }
    if (obj == WITCH) {
        if (there(WITCH, loc)) {
            puts("Witch: Smack!\nThere is a billowing cloud of yellowish-brown smoke!" SOFT_NL
                 "As the smoke clears, the hideous hag transforms into a sexy young girl!");
            apport(WITCH, R_LIMBO);
            apport(GIRL, loc);
            default_object = spelling_of(GIRL);
        } else {
            printf("I see no %s here.\n", lowercase(active_object));
        }
    } else if (obj == GIRL) {
        if (there(GIRL, loc) || toting(GIRL)) {
            puts("The girl gazes dreamily into your eyes as your lips press gently together.");
        } else {
            printf("I see no %s here.\n", lowercase(active_object));
        }
    } else if (obj == SLIME || obj == ZARKA || obj == CLOWN || obj == BEAST || obj == SNAKE || obj == NURSE) {
        // Yes, even if the object is not present.
        // The original game also gets here in string-matching corner cases such as KISS WITCHFOO.
        printf("The %s will become angry if you persist!\n", lowercase(active_object));
    } else {
        printf("Okay, but you're a strange one!");
    }
}

// Line 4060 in advent.txt
void attempt_climb() {
    extract_object();
    if (active_object == NULL) {
        if (loc == R_TREE) {
            loc = R_HOUSE;
        } else {
            puts("I see nothing to climb here.");
        }
    } else if (strcmp(active_object, "TREE") == 0) {
        if (loc == R_TREE) {
            loc = R_HOUSE;
        } else {
            puts("I see no tree here.");
        }
    } else {
        puts("I don't see how to climb that.");
    }
}

// Line 4190 in advent.txt
void attempt_move(char direction) {
    direction = toupper(direction);
    if (aboard_disc) {
        puts("You're still on the disk!");
    } else if (loc == R_QUICK && quicksand_timer == 0) {
        // Print nothing, and be sinking in quicksand again during update_timers_and_look()
    } else {
        const char *dirs = "NSEWUDIO";
        assert(strchr(dirs, direction) != NULL);
        int i = strchr(dirs, direction) - dirs;
        assert(0 <= i && i <= 7);
        Location newloc = places[loc].neighbors[i];
        if (loc == R_RROOM && direction == 'S' && there(GRATE, R_LIMBO)) {
            newloc = R_RCORE;
        } else if (loc == R_TREASURE && direction == 'W' && zarka_status != 0) {
            newloc = R_EW;
        } else if (loc == R_TRAIN && direction == 'O') {
            // The original game inspects R1 (oldloc) here, but that doesn't
            // play well with ALLEZ, so let's just do the natural thing.
            newloc = (there(TRAIN, R_STATION1) ? R_STATION1 : R_STATION2);
        }
        if (newloc == R_LIMBO) {
            puts("You can't go that way.");
        } else if (there(ROCKS, loc)) {
            puts("You can't go that way.");
        } else if (newloc == R_RCORE && reactor_is_active) {
            puts("You're in an active nuclear reactor. You do not survive.");
            die();
        } else if (there(HILLS, loc)) {
            puts("As you walk across the room, your foot crushes one of the little" SOFT_NL
                 "dirt hills, and you suddenly feel very dead.");
            die();
        } else if (there(WITCH, loc)) {
            puts("The witch won't let you go.");
        } else if (there(SLIME, loc)) {
            puts("You can't get past the slime.");
        } else if (there(BEAST, loc)) {
            puts("You can't get past the beast.");
        } else if (there(SNAKE, loc)) {
            puts("You can't get past the snake.");
        } else if (there(CLOWN, loc)) {
            puts("You can't get by the clown.");
        } else if (there(NURSE, loc)) {
            puts("You can't get past the nurse.");
        } else if (newloc == R_BED && toting(GIRL) && !married) {
            puts("A mysterious force prevents you from entering the room (leave the girl outside).");
        } else {
            // Describe the new room, even if it's the same as the old room
            // (e.g. in the maze of twisty passages).
            oldloc = R_LIMBO;
            loc = newloc;
        }
    }
}

// Line 4730 in advent.txt
void attempt_inventory() {
    bool seen_something = false;
    puts("You are carrying:");
    for (Object i = LAMP; i <= ART; ++i) {
        if (toting(i)) {
            printf("  "); describe_object(i);
            seen_something = true;
        }
    }
    if (!seen_something) {
        puts("  Nothing.");
    }
}


// Line 4650 in adven2.txt
void attempt_kill(Object obj, Object weapon) {
    if (obj == NO_OBJECT) {
        extract_object();
        if (active_object == NULL) {
            active_object = get_direct_object("Kill");
            if (active_object == NULL) {
                return;
            } 
        }
        obj = lookup_noun(active_object);
    }
    switch (obj) {
        case WITCH: case GIRL: case SNAKE: case CLOWN: case ZARKA:
        case SLIME: case BEAST: case NURSE:
            break;
        default:
            puts("I don't see how to kill that!");
            return;
    }
    if (weapon == NO_OBJECT) {
        const char *input = get_indirect_object("With what?");
        if (input == NULL) {
            return;
        }
        // The original game matches only the strings KNIFE and SPRAY.
        weapon = lookup_noun(input);
    }
    if (weapon == SPRAY) {
        if (!toting(SPRAY)) {
            puts("You don't have the spray.");
        } else if (obj == SLIME) {
            puts("The slime quickly dehydrates and becomes a powder that falls" SOFT_NL
                 "into the crevices in the floor.");
            apport(SLIME, R_LIMBO);
        } else {
            printf("The spray is ineffective against the %s.\n", lowercase(active_object));
        }
    } else if (weapon == KNIFE) {
        if (!toting(KNIFE)) {
            puts("You don't have the knife.");
        } else if (!there(obj, loc)) {
            printf("I see no %s here.\n", lowercase(active_object));
        } else if (obj == SNAKE) {
            puts("The snake is wounded. He crawls away to reveal a large number of rare coins!");
            apport(SNAKE, R_LIMBO);
            apport(COINS, loc);
        } else if (obj == WITCH || obj == ZARKA || obj == SLIME || obj == BEAST) {
            switch (obj) {
                case WITCH: puts("The witch laughs at your attempt, and your knife disintegrates."); break;
                case ZARKA: puts("The Zarka grabs the knife from you and breaks it in half."); break;
                case SLIME: puts("The slime dissolves your knife and a small part of your hand."); break;
                case BEAST: puts("As you thrust toward the beast, he grabs your arm, causing you" SOFT_NL
                                 "to release the knife. He then proceeds to bend the knife in half."); break;
            } 
            apport(KNIFE, R_LIMBO);
        } else if (obj == GIRL) {
            puts("At your attempt, the girl flees in terror!");
            apport(GIRL, R_LIMBO);
        } else if (obj == CLOWN) {
            puts("The clown becomes terrified, drops his french fries, and runs into the meadow.");
            apport(CLOWN, R_LIMBO);
            apport(FRIES, loc);
        } else if (obj == NURSE) {
            puts("The nurse becomes terrified, injects herself with the needle, and" SOFT_NL
                 "suddenly disappears into thin air!");
            apport(NURSE, R_LIMBO);
        } else {
            assert(false);  // unreachable
        }
    } else {
        puts("I don't see how to kill with that.");
    }
}

// Line 1640 in adven2.c
void attempt_insert() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Insert");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj != CARD && obj != CYLINDER) {
        printf("I don't see how to do that with the %s.\n", lowercase(active_object));
    } else {
        const char *input = get_indirect_object("Where?");
        if (input == NULL) {
            return;
        } else if (lookup_noun(input) != BOX) {
            printf("I don't see how to get the %s in there!\n", lowercase(active_object));
        } else if (!toting(obj)) {
            printf("You don't have the %s.\n", lowercase(active_object));
        } else if (obj == CYLINDER) {
            if (loc == R_RROOM) {
                puts("As the cylinder goes in, the hum from the reactor ceases.");
                score += object_weight(CYLINDER);
                apport(CYLINDER, R_LIMBO);
                reactor_is_active = false;
            } else {
                puts("I see no slot here.");
            }
        } else {
            assert(obj == CARD);
            if (loc == R_STATION1 || loc == R_STATION2) {
                puts("The gate swings open as you pass through into the train.\n"
                     "The card pops out onto the floor.\n"
                     "Doong, doong! Swoosh! The doors close and the train immediately starts to" SOFT_NL
                     "move very rapidly. After about 4 seconds, it stops suddenly. Swoosh!");
                apport(CARD, R_TRAIN);
                apport(TRAIN, (there(TRAIN, R_STATION1) ? R_STATION2 : R_STATION1));
                oldloc = loc;
                loc = R_TRAIN;
            } else {
                puts("I see no slot here.");
            }
        }
    }
}

// Line 4870 in advent.txt
void attempt_open() {
    bool chest_is_visible = (loc == R_CHEST || loc == R_INSIDE);
    bool door_is_visible = (loc == R_CRAWL);
    extract_object();
    Object obj = (active_object != NULL) ? lookup_noun(active_object) :
        chest_is_visible ? CHEST : door_is_visible ? DOOR : NO_OBJECT;
    if (chest_is_visible && obj == CHEST) {
        if (chest_is_open) {
            puts("The chest is already open.");
        } else if (!chest_is_unlocked) {
            puts("The chest is locked with a rusty padlock.");
        } else {
            // This codepath is confusing in the original.
            // Opening the chest teleports us to "Inside Chest,"
            // which has the same exits as R_CHEST (thanks
            // to line 4270 in the original movement code).
            // It also prematurely shows the contents of that room,
            // except that it shows only the gold (nothing else that
            // might be in that room). There's no way to get from
            // R_INSIDE back to R_CHEST, and no way to get from
            // R_CHEST back to R_INSIDE except to close and open
            // the chest again.
            //
            chest_is_open = true;
            loc = R_INSIDE;
            puts("Chest: Open.\nThe chest contains:");
            if (there(GOLD, R_INSIDE)) {
                puts("Dazzling nugget of gold.");
                default_object = spelling_of(GOLD);
            } else {
                puts("Nothing.");
            }
        }
    } else if (door_is_visible && obj == DOOR) {
        puts("The door is already open.");
    } else if (obj == DOOR) {
        puts("I see no door here.");
    } else if (obj == NO_OBJECT) {
        puts("I see nothing to open here.");
    } else {
        puts("I don't see how to open that!");
    }
}

// Line 5130 in advent.txt
void attempt_close() {
    bool chest_is_visible = (loc == R_CHEST || loc == R_INSIDE);
    bool door_is_visible = (loc == R_CRAWL);
    extract_object();
    Object obj = (active_object != NULL) ? lookup_noun(active_object) :
        chest_is_visible ? CHEST : door_is_visible ? DOOR : NO_OBJECT;
    if (chest_is_visible && obj == CHEST) {
        if (!chest_is_open) {
            puts("The chest is already closed.");
        } else {
            chest_is_open = false;
            puts("Chest: Closed.");
        }
    } else if (door_is_visible && obj == DOOR) {
        puts("The door won't budge.");
    } else if (obj == DOOR) {
        // The original game treats "CLOSE DOOR" as "CLOSE CHEST" when the chest is visible.
        puts("I see no door here.");
    } else if (obj == NO_OBJECT) {
        puts("I see nothing to close here.");
    } else {
        puts("I don't see how to close that!");
    }
}

// Line 5330 in advent.txt
void attempt_lock() {
    bool chest_is_visible = (loc == R_CHEST || loc == R_INSIDE);
    extract_object();
    Object obj = (active_object != NULL) ? lookup_noun(active_object) :
        chest_is_visible ? CHEST : NO_OBJECT;
    if (chest_is_visible && obj == CHEST) {
        if (loc == R_INSIDE) {
            puts("You can't lock the chest from within.");
        } else if (!chest_is_unlocked) {
            puts("The chest is already locked.");
        } else if (!toting(KEYS)) {
            puts("You don't have the key.");
        } else {
            puts("Chest: Locked.");
            chest_is_unlocked = false;
        }
    } else if (obj == NO_OBJECT) {
        puts("I see nothing to lock here.");
    } else {
        puts("I don't see how to lock that.");
    }
}

// Line 5530 in advent.txt
void attempt_unlock() {
    bool chest_is_visible = (loc == R_CHEST || loc == R_INSIDE);
    extract_object();
    Object obj = (active_object != NULL) ? lookup_noun(active_object) :
        chest_is_visible ? CHEST : NO_OBJECT;
    if (chest_is_visible && obj == CHEST) {
        if (chest_is_unlocked || loc == R_INSIDE) {
            puts("The chest is already unlocked.");
        } else if (!toting(KEYS)) {
            puts("You don't have the key.");
        } else {
            puts("Chest: Unlocked.");
            chest_is_unlocked = true;
        }
    } else if (obj == NO_OBJECT) {
        puts("I see nothing to unlock here.");
    } else {
        puts("I don't see how to unlock that.");
    }
}

// Line 5690 in advent.txt
void attempt_argyle() {
    if (loc == R_CATACOMB || loc == R_LIBRARY) {
        puts("In a flash...");
        oldloc = loc;
        loc = (loc == R_CATACOMB) ? R_LIBRARY : R_CATACOMB;
    } else {
        puts("They make socks, don't they?");
    }
}

// Line 5760 in advent.txt
void attempt_stab() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object(Titlecase(active_verb));
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    switch (obj) {
        case WITCH: case GIRL: case SNAKE: case CLOWN: case ZARKA:
        case SLIME: case BEAST: case NURSE:
            attempt_kill(obj, KNIFE);
            break;
        case TREE:
            if (there(loc, TREE)) {
                puts("You have no axe.");
                puts("An EPA official runs up and, upon discovering your evil intent," SOFT_NL
                     "sentences you to death.");
                die();
            } else {
                puts("I see no tree here.");
            }
        default:
            puts("I don't see how to kill that!");
            return;
    }

}

// Line 5910 in advent.txt
void attempt_break() {
    extract_object();
    if (active_object == NULL || lookup_noun(active_object) == VASE) {
        if (here(VASE, loc)) {
            puts("The vase is reduced to worthless shards of pottery.");
            apport(VASE, R_LIMBO);
            apport(SHARDS, loc);
        } else {
            puts("I see no vase here.");
        }
    } else {
        printf("I see no reason to break the %s.\n", lowercase(active_object));
    }
}

// Line 6180 in advent.txt
void attempt_push_pull() {
    extract_object();
    if (active_object == NULL) {
        if (there(KNOB, loc)) {
            active_object = spelling_of(KNOB);
        } else if (there(LEVER, loc)) {
            active_object = spelling_of(LEVER);
        } else if (there(BUTTON, loc)) {
            active_object = spelling_of(BUTTON);
        } else {
            // The original game omits this codepath.
            active_object = get_direct_object(Titlecase(active_verb));
            if (active_object == NULL) {
                return;
            }
        }
    }
    Object obj = lookup_noun(active_object);
    if (!here(obj, loc)) {
        printf("I see no %s here.\n", lowercase(spelling_of(obj)));
    } else if (obj == KNOB && married) {
        puts("As you pull the knob, a metallic voice is heard (seemingly emanating" SOFT_NL
             "from within): 'I cannot join those who are already married.'");
    } else if (obj == KNOB) {
        puts("Welcome to the FROBOZZ Automatic Coupler!\n\n"
             "This machine will perform a wedding ceremony provided that both" SOFT_NL
             "parties consent.\n\n");
        if (!here(GIRL, loc)) {
            puts("There's only one problem:  I don't see any girl here!");
        } else {
            puts("Dearly beloved, we are gathered here to join this man and this woman in Holy" SOFT_NL
                 "Matrimony.\n"
                 "Do you take this girl to be your wife?");
            if (!prompt_for('Y')) {
                puts("This wedding is officially aborted.");
                return;
            }
            puts("Girl: 'I guess.'\n\n"
                 "Is there anyone here who knows a reason why this couple should not be wed?");
            if (prompt_for('Y')) {
                puts("This wedding is officially aborted.");
                return;
            }
            puts("Good...By the power vested in me by the FROBOZZ Magic Computer Company, I now" SOFT_NL
                 "pronounce you man and wife.\n"
                 "[A sparkling emerald wedding ring drops from a slot in the machine!]\n"
                 "You may kiss the bride.");
            apport(RING, loc);
            default_object = spelling_of(RING);
            married = true;
        }
    } else if (obj == LEVER) {
        puts("Lever: Your molecules shimmer away...\n");
        switch (loc) {
            case R_CUBIC1: loc = R_SMALL; break;
            case R_CUBIC2: loc = R_CUBIC1; break;
            case R_CUBIC3: loc = R_CUBIC2; break;
            case R_CUBIC4: loc = R_CUBIC3; break;
        }
    } else if (obj == BUTTON && strcmp(active_verb, "PUSH") == 0) {
        puts("The lift lurches forward and carries you swiftly into the sky.  A" SOFT_NL
             "striking view of the Mt. Frobozz panorama can be seen.  After travelling" SOFT_NL
             "for several minutes, the lift stops with a jerk and drops you in the snow.\n");
        loc = (loc == R_BASE) ? R_SUMMIT : R_BASE;
    } else {
        printf("I don't see how to %s that!\n", lowercase(active_verb));
    }
}

// Line 6840 in advent.txt
void attempt_allez() {
#if WIZARD_MODE
    char input[130];
    printf("Go where? > "); fflush(stdout);
    fgets(input, sizeof input, stdin);
    int r = atoi(input);
    if (R_START <= r && r <= R_ART) {
        default_object = NULL;
        oldloc = loc;
        loc = r;
    } else {
        puts("No.");
    }
#else
    puts("You are not a wizard.");
#endif
}

// Line 6920 in advent.txt
void attempt_play() {
    extract_object();
    Object obj = (active_object == NULL) ? NO_OBJECT : lookup_noun(active_object);
    if (active_object == NULL) {
        if (here(CELLO, loc)) {
            obj = CELLO;
        } else {
            puts("I see nothing to play here.");
        }
    }
    if (obj == CELLO) {
        if (!here(CELLO, loc)) {
            puts("I see no cello here.");
        } else {
            puts("Squeeek!");
            if (there(BEAST, loc)) {
                puts("The beast is calmed by your music and leaves.");
                apport(BEAST, R_LIMBO);
            }
        }
    } else {
        puts("I don't see how to play that!");
    }
}

// Line 7070 in advent.txt
void attempt_jump() {
    if (there(HILLS, loc)) {
        printf("As you %s around the room, you crush one of the little" SOFT_NL
               "dirt hills, and you suddenly feel very dead.\n", lowercase(active_verb));
        die();
    } else {
        puts("Very good. Now you can go to the second grade.");
    }
}

// Line 7130 in advent.txt
void attempt_wave() {
    extract_object();
    if (active_object == NULL) {
        puts("That doesn't accomplish much.");
    } else {
        Object obj = lookup_noun(active_object);
        if (obj != STAFF) {
            printf("I see no reason to wave the %s.\n", lowercase(active_object));
        } else if (!toting(STAFF)) {
            printf("I see no %s here.\n", lowercase(active_object));
        } else if (there(HILLS, loc)) {
            puts("The little hills are gone!");
            apport(HILLS, R_LIMBO);
        } else {
            puts("Nothing seems to happen.");
        }
    }
}

// Line 2030 in adven2.txt
void attempt_say() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Say");
        if (active_object == NULL) {
            return;
        }
    }
    if (strncmp(lowercase(active_object), "argyl", 5) == 0) {
        attempt_argyle();
    } else {
        puts("Zowee!");
    }
}

// Line 2540 in adven2.txt
void attempt_bug() {
    puts("Bug reporting isn't supported in this C port.");
    puts("In the original, you would have been prompted to leave a two-line message" SOFT_NL "describing the problem.");
}

// Line 3010 in advent.txt
void attempt_find() {
#if WIZARD_MODE
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Find");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == NO_OBJECT) {
        printf("%s: unable to locate this object.\n", Titlecase(active_object));
    } else if (toting(obj)) {
        printf("%s: you are carrying it.\n", Titlecase(active_object));
    } else if (there(obj, R_LIMBO)) {
        printf("%s: object does not exist.\n", Titlecase(active_object));
    } else {
        int where = object_position[obj];
        assert(1 <= where && where <= 99);
        printf("%s: room %d%d.\n", Titlecase(active_object), where / 10, where % 10);
    }
#else
    puts("You are not a wizard.");
#endif
}

// Line 3220 in adven2.txt
void attempt_fuck() {
    extract_object();
    Object obj = (active_object != NULL) ? lookup_noun(active_object) : GIRL;
    if (obj == GIRL || strcmp(active_object, "BRIDE") == 0) {
        if (!there(BED, loc)) {
            puts("I see no bed here.");
        } else if (!here(GIRL, loc)) {
            puts("I see no girl here.");
        } else if (fucked) {
            puts("Not now; the girl has a headache.");
        } else {
            puts("Cosmic!  What a WOMAN!");
            fucked = true;
        }
    } else if (obj == NO_OBJECT || !here(obj, loc)) {
        printf("I see no %s here.\n", lowercase(active_object));
    } else {
        // The original game tests whether the object is not there OR not toted,
        // rather than AND, so this codepath is never hit. We fix that.
        puts("Okay, but you're a strange one!");
        printf("%s: A valiant attempt.\n", Titlecase(active_object));
    }
}

// Line 3490 in adven2.txt
void attempt_feed() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Feed");
        if (active_object == NULL) {
            return;
        }
    }
    const char *input = get_indirect_object("Feed it what?");
    if (input == NULL) {
        return;
    }
    Object obj = lookup_noun(active_object);
    Object food = lookup_noun(input);
    if (obj == ZARKA) {
        if (!there(ZARKA, loc)) {
            puts("I see no Zarka here.");
        } else if (food != PIZZA) {
            printf("The Zarka is not interested in the %s.\n", lowercase(input));
        } else if (!toting(PIZZA)) {
            printf("You don't have the %s.\n", lowercase(input));
        } else {
            score += object_weight(PIZZA);
            apport(PIZZA, R_LIMBO);
            puts("The Zarka becomes your friend for life and shows you an exit on the western wall.");
            zarka_status = 1; // friendly
        }
    } else {
        printf("The %s is not interested", lowercase(active_object));
        printf(" in the %s.\n", lowercase(input));
        // The original game does GOTO 1650 here instead of GOTO 1080;
        // that dumps it in the middle of the PUT/INSERT action and produces
        // an additional bogus message. Let's not do that.
        // Perhaps the original intent was a (broken) gesture in the
        // direction of FEED CARD TO SLOT.
    }
}

// Line 3750 in adven2.txt
void attempt_spray() {
    extract_object();
    Object obj = (active_object != NULL) ? lookup_noun(active_object) : NO_OBJECT;
    if (active_object == NULL) {
        if (there(SLIME, loc)) {
            obj = SLIME;
        } else {
            puts("I see nothing to spray here.");
            return;
        }
    }
    if (!toting(SPRAY)) {
        puts("You don't have the spray.");
    } else if (obj == SLIME) {
        attempt_kill(SLIME, SPRAY);
    } else {
        printf("I see no reason to spray the %s.\n", lowercase(active_object));
    }
}

// Line 3860 in adven2.txt
void attempt_kick() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object(Titlecase(active_verb));
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    switch (obj) {
        case WITCH: case GIRL: case SNAKE: case CLOWN: case ZARKA:
        case SLIME: case BEAST: case NURSE:
            printf("The %s will become angry if you persist!\n", lowercase(active_object));
            break;
        default:
            puts("That doesn't accomplish much.");
            break;
    }
}

// Line 3880 in adven2.txt
void attempt_use() {
    extract_object();
    if (active_object == NULL) {
        // The original game accidentally omits this codepath.
        active_object = get_direct_object("Use");
        if (active_object == NULL) {
            return;
        }
    }
    printf("Fiddling with the %s in this manner is not very productive.\n", lowercase(active_object));
}

// Line 4010 in adven2.txt
void attempt_swim() {
    if (loc == R_FALL || loc == R_WET) {
        if (there(GUM, R_LIMBO)) {
            puts("For some reason, you can breathe underwater!  After swimming for a while, you" SOFT_NL
                 "surface on the other side of the waterfall.");
            loc = (loc == R_FALL) ? R_WET : R_FALL;
        } else {
            puts("You can't breathe under water; you suddenly feel very dead.");
            die();
        }
    } else {
        puts("I don't see how to swim here.");
    }
}

// Line 4120 in adven2.txt
void attempt_taste() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Taste");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == WATER) {
        // The original game does a string comparison with "WATER" here.
        if (!toting(WATER)) {
            puts("You don't have the water.");
        } else {
            puts("Cool and delicious.");
            apport(WATER, R_LIMBO);
        }
    } else if (obj == NO_OBJECT) {
        puts("I don't see how to eat that!");
    } else {
        attempt_eat(obj);
    }
}

// Line 4220 in adven2.txt
void attempt_dig() {
    if (!toting(SPADE)) {
        puts("You don't have anything to dig with.");
    } else if (!has_dirt(loc)) {
        puts("You can't dig here.");
    } else {
        if (loc == R_GARDEN && places[loc].hole_depth == 4 && there(STATUE, R_LIMBO)) {
            puts("You have unearthed a beautiful jade statue.");
            apport(STATUE, loc);
        } else if (there(HILLS, loc)) {
            puts("You are digging a hole.");
            puts("Booom!  You just dug up a live mine!"); // and a bell character
            die();
        } else switch (places[loc].hole_depth) {
            case 0: puts("You are digging a hole."); break;
            case 1: puts("The hole is getting deeper, but that's about all."); break;
            case 2: puts("You are getting your nails dirty."); break;
            default: puts("You are surrounded on all sides by earth."); break;
        }
        places[loc].hole_depth += (places[loc].hole_depth < 4);
    }
}

// Line 4480 in adven2.txt
void attempt_listen() {
    if (loc == R_TONE) {
        puts("You hear pleasant tones emanating from this room.");
    } else if (loc == R_FALL) {
        puts("You hear the sound of cool water splashing all around you.");
    } else if (loc == R_SEWER2) {
        puts("You hear a strange, distant echoing sound, apparently an animal" SOFT_NL
             "humming to himself.");
    } else if (loc == R_RROOM && reactor_is_active) {
        puts("You hear a humming noise probably coming from the reactor.");
    } else {
        puts("I don't hear anything.");
    }
}

// Line 5380 in adven2.txt
void attempt_board() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Board");
        if (active_object == NULL) {
            return;
        }
    }
    // The original game matches only DISC/DISK here, not e.g. SAUCER.
    Object obj = lookup_noun(active_object);
    if (obj != DISC) {
        puts("I don't see how to board that!");
    } else if (!here(DISC, loc)) {
        puts("You don't have the disc.");
    } else if (toting(DISC)) {
        puts("The disc must be on the ground to board.");
    } else {
        puts("You are aboard the disc!");
        aboard_disc = true;
    }
}

// Line 1090 in adven2.txt
void attempt_disembark() {
    if (aboard_disc) {
        aboard_disc = false;
        apport(DISC, loc);
        puts("You are off the SNO-DISC.");
    } else {
        puts("You aren't on anything.");
    }
}

// Line 5570 in adven2.txt
void attempt_launch() {
    if (!aboard_disc) {
        puts("You are not aboard the disc.");
    } else if (loc == R_EXPERT) {
        puts("You are going 90 meters per second on a sheet of ice.");
        puts("A cliff!  Watch out!"); // and one bell character
        puts("You impaled yourself on a tree limb.  You do not survive.");
        die();
    } else if (loc == R_BUNNY) {
        puts("You coast down the gentle grade with ease.");
        loc = R_BASE;
        apport(DISC, R_BASE);
    } else {
        puts("You don't seem to go anywhere.");
    }
}

// Line 5820 in adven2.txt
void attempt_toss(bool verb_is_toss) {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object(verb_is_toss ? "Toss" : "Throw");
        if (active_object == NULL) {
            return;
        }
    }
    // This ought to be basically the same logic as in `attempt_drop`,
    // but in fact it's different in several minor messages; and
    // the BLOCK/GLOVE interaction isn't implemented. So in the original,
    // you can successfully TOSS GLOVE away even while holding the BLOCK.
    // We fix that.
    //
    Object obj = lookup_noun(active_object);
    if (obj == NO_OBJECT) {
        printf("I see no %s here.\n", lowercase(active_object));
    } else if (!toting(obj)) {
        printf("You don't have the %s.\n", lowercase(active_object));
    } else if (obj == VASE && !there(STOOL, loc)) {
        puts("The vase falls to the floor and smashes into a million pieces!");
        apport(VASE, R_LIMBO);
        apport(SHARDS, loc);
    } else {
        puts(verb_is_toss ? "Tossed." : "Thrown.");
        apport(obj, loc);
        // The following interactions aren't in the original.
        if (loc == R_QUICK && obj == BLOCK) {
            puts("As the block sinks, the quicksand instantly freezes over hard enough" SOFT_NL
                 "to support your weight.");
            apport(BLOCK, R_LIMBO);
            quicksand_timer = 100;
        } else if (obj == GLOVE && toting(BLOCK)) {
            puts("The dry ice instantly freezes your hand, and you die from shock.");
            die();
        }
    }
}

// Line 6110 in adven2.txt
void attempt_turn() {
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Turn");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == WHEEL) {
        if (!there(WHEEL, loc)) {
            puts("I see no wheel here.");
        } else {
            // This happens even if the grate has already been removed.
            puts("The gratings slide away.");
            apport(GRATE, R_LIMBO);
        }
    } else {
        puts("I don't see how to turn that!");
    }
}

// Line 6270 in adven2.txt
void attempt_comb() {
    // This codepath is all messed up in the original game.
    // It doesn't GOSUB to extract_object(), so the object is
    // never set properly; and it does GOTO 6360 instead of 6350,
    // which means O1$ isn't set either, so it's never "ZARKA",
    // so there's no way to comb the Zarka.
    //
    if (!toting(COMB)) {
        puts("You don't have the comb.");
        return;
    }
    extract_object();
    if (active_object == NULL) {
        active_object = get_direct_object("Comb");
        if (active_object == NULL) {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == ZARKA) {
        if (!toting(ZARKA)) {
            puts("You don't have the Zarka.");
        } else if (zarka_status == 1) {
            puts("The Zarka becomes enraged [but shiny].");
            zarka_status = 2;
        } else {
            puts("The Zarka is not very amused.");
        }
    } else {
        puts("I don't see how to comb that!");
    }
}

// Line 6480 in adven2.txt
void attempt_pray() {
    puts("Oh father, who art in heaven...");
    if (loc == R_CHAPEL) {
        puts("\n\nThou hast honoured me with thy words...");
        puts("For thy worship thou shalt be blessed.");
        puts("\n\nLET THERE BE LIGHT!\n\n");
        lamp_power = 450;
    }
}

void set_active_command(char *input) {
    char *p = input;
    char *q = active_command;
    while (*p == ' ') ++p;
    while (*p != '.') *q++ = toupper(*p++);
    while (q > active_command && q[-1] == ' ') --q;
    *q = '\0';
    q = input;
    p += 1; // skip over the period we just found
    while (*p != '\0') *q++ = *p++;
    *q = '\0';
}

void parse_input() {
    // The user will enter something like " w . Climb... get lamp".
    // We notionally preprocess that into "W.CLIMB.GET LAMP." and then
    // pull off the first dot-terminated command into `active_command`
    // (C1$ in advent.txt). Except that really the uppercasing and
    // space-stripping is done by `set_active_command`, so it's not
    // actually "preprocessing."
    while (true) {
        static char input[130];  // C$ in advent.txt
        while (input[0] == '\0') {
            printf("> "); fflush(stdout);
            fgets(input, sizeof input, stdin);
            char *p = strchr(input, '\0');
            while (p > input && (p[-1] == '\n' || p[-1] == ' ')) {
                --p;
            }
            if (p != input && p[-1] != '.') {
                *p++ = '.';
            }
            *p = '\0';
        }

        set_active_command(input);
        if (active_command[0] != '\0') {
            break;
        }
    }
    active_verb = NULL;
    active_object = NULL;
#define CMDIS(s) (strncmp(active_command, s, sizeof(s)-1) == 0 ? ((active_verb = s), true) : false)

    if (strlen(active_command) == 1) {
        const char *r = strchr("NSEWUDIO", active_command[0]);
        if (r == NULL) {
            puts("I don't understand.");
        } else {
            attempt_move(active_command[0]);
        }
    } else if (CMDIS("BRIEF")) {
        puts("From now on I will only give descriptions of rooms you haven't been to.");
        brief_mode = 0;
    } else if (CMDIS("SUPER")) {
        puts("From now on, I will only give room names.");
        brief_mode = 1;
    } else if (CMDIS("LONG")) {
        puts("From now on I'll give you a complete description of each room.");
        brief_mode = 2;
    } else if (CMDIS("GET OFF") || CMDIS("DISEM")) {
        attempt_disembark();
    } else if (CMDIS("TAKE") || CMDIS("GET") || CMDIS("PICK UP") || CMDIS("CARRY") || CMDIS("KEEP")) {
        attempt_take();
    } else if (CMDIS("DROP")) {
        attempt_drop();
    } else if (CMDIS("EAT")) {
        attempt_eat(NO_OBJECT);
    } else if (CMDIS("LIGHT") || CMDIS("ON") || CMDIS("TURN ON")) {
        attempt_light();
    } else if (CMDIS("OFF") || CMDIS("TURN OFF")) {
        attempt_douse();
    } else if (CMDIS("READ")) {
        attempt_read();
    } else if (CMDIS("KISS")) {
        attempt_kiss();
    } else if (CMDIS("CLIMB")) {
        attempt_climb();
    } else if (CMDIS("NORTH") || CMDIS("SOUTH") || CMDIS("EAST") || CMDIS("WEST") || CMDIS("UP") || CMDIS("DOWN") || CMDIS("OUT")) {
        attempt_move(active_verb[0]);
    } else if (CMDIS("IN")) {
        attempt_inventory();
    } else if (CMDIS("KILL") || CMDIS("ATTAC")) {
        attempt_kill(NO_OBJECT, NO_OBJECT);
    } else if (CMDIS("PUT") || CMDIS("INSERT")) {
        attempt_insert();
    } else if (CMDIS("OPEN")) {
        attempt_open();
    } else if (CMDIS("CLOSE")) {
        attempt_close();
    } else if (CMDIS("LOCK")) {
        attempt_lock();
    } else if (CMDIS("UNLOC")) {
        attempt_unlock();
    } else if (CMDIS("ARGYL")) {
        attempt_argyle();
    } else if (CMDIS("STAB") || CMDIS("CHOP")) {
        attempt_stab();
    } else if (CMDIS("BREAK")) {
        attempt_break();
    } else if (CMDIS("ENTER")) {
        attempt_move('I');
    } else if (CMDIS("EXIT") || CMDIS("LEAVE")) {
        attempt_move('O');
    } else if (CMDIS("LOOK")) {
        really_look = true;
    } else if (CMDIS("PUSH") || CMDIS("PULL")) {
        attempt_push_pull();
    } else if (CMDIS("ALLEZ")) {
        attempt_allez();
    } else if (CMDIS("PLAY")) {
        attempt_play();
    } else if (CMDIS("JUMP") || CMDIS("HOP")) {
        attempt_jump();
    } else if (CMDIS("WAVE")) {
        attempt_wave();
    } else if (CMDIS("SAY") || CMDIS("SPEAK") || CMDIS("CALL")) {
        attempt_say();
    } else if (CMDIS("GO") || CMDIS("WALK") || CMDIS("RUN") || CMDIS("CRAWL")) {
        // Notice there is no special case for e.g. GO WEST.
        puts("Yes, but in which direction?");
    } else if (CMDIS("QUIT") || CMDIS("SCORE") || CMDIS("STOP")) {
        attempt_score();
    } else if (CMDIS("HELP")) {
        attempt_help();
    } else if (CMDIS("BUG")) {
        attempt_bug();
#ifdef SAVE_AND_RESTORE
    } else if (CMDIS("SAVE")) {
        switch (attempt_save()) {
            case 0: puts("Save failed!\n"); break;
            case 1: puts("Saved.\n"); break;
            case 2: puts("Restored.\n"); break;
        }
    } else if (CMDIS("LOAD") || CMDIS("RESTORE")) {
        /* On the fizmo interpreter, @restore yields 2
         * when the save file doesn't exist, or when it
         * has the wrong serial number for thisgame.
         * I don't know what return value 0 would mean. */
        attempt_restore();
        puts("Restore failed!\n");
#endif
    } else if (CMDIS("FIND")) {
        attempt_find();
    } else if (CMDIS("HAVE SEX") || CMDIS("SCREW")) {
        attempt_fuck();
    } else if (CMDIS("FEED")) {
        attempt_feed();
    } else if (CMDIS("SPRAY")) {
        attempt_spray();
    } else if (CMDIS("HIT") || CMDIS("KICK") || CMDIS("PUNCH") || CMDIS("BITE") || CMDIS("POKE")) {
        attempt_kick();
    } else if (CMDIS("USE")) {
        attempt_use();
    } else if (CMDIS("SING")) {
        puts("If I were a rich man...");
    } else if (CMDIS("SWIM")) {
        attempt_swim();
    } else if (CMDIS("TASTE")) {
        attempt_taste();
    } else if (CMDIS("HIDE")) {
        puts("There is no place to hide!");
    } else if (CMDIS("DIG")) {
        attempt_dig();
    } else if (CMDIS("LISTE")) {
        attempt_listen();
    } else if (CMDIS("BOARD")) {
        attempt_board();
    } else if (CMDIS("LAUNC")) {
        attempt_launch();
    } else if (CMDIS("THROW")) {
        attempt_toss(false);
    } else if (CMDIS("TOSS")) {
        attempt_toss(true);
    } else if (CMDIS("BURN")) {
        puts("You have no matches.");
        puts("An EPA official runs up and, upon discovering your evil intent," SOFT_NL
             "sentences you to death.");
        die();
    } else if (CMDIS("TURN")) {
        attempt_turn();
    } else if (CMDIS("COMB")) {
        attempt_comb();
    } else if (CMDIS("PRAY")) {
        attempt_pray();
    } else {
        puts("I don't understand.");
    }
}

int main()
{
#ifdef Z_MACHINE
    puts("\n\n\n\n\n\n\n\n");
#endif /* Z_MACHINE */

    initialize_rooms();

    puts("Welcome to Adventure ]I[!\n");
    puts("Do you need instructions?");
    if (!prompt_for('N')) {
        attempt_help();
    }

    while (true) {
        apport_schizoid_objects();
        update_timers_and_look();
        parse_input();
    }
}

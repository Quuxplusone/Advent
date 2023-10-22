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
#define putsn(s) do { puts(s); extra_newline(); } while (0)

enum {
    R_INHAND = -1, R_LIMBO = 0,
    R_START, R_MTREE, R_HOUSE, R_CUBIC1, R_SMALL, R_MAZE1, R_MAZE2, R_MAZE3, R_CHEST, R_INSIDE,
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
    Location neighbors[6];
    bool visited;
};
struct Place places[R_ART+1];

Location object_position[??];         // O[n,1] in the original program
int object_weight[??];                // O[n,2] in the original program; 500 is "immobile"
bool visited[??];                     // M in the original program
char active_command[130];             // C1$ in the original program
const char *active_verb = NULL;       // B$ in the original program
const char *active_object = NULL;     // O1$ in the original program
const char *default_object = NULL;    // D$ in the original program
int brief_mode = 2;                   // B9 in the original; 0=brief, 1=superbrief, 2=long
bool chest_is_unlocked = false;       // C8 in the original program
bool chest_is_open = false;           // C9 in the original program
int quicksand_timer = 0;              // D2 in the original program
int lamp_counter = 0;                 // P0 in the original program
bool lamp_is_on = false;              // L in the original program
int current_load = 0;                 // L0 in the original; >150 is "too heavy"
Location loc = R_LIMBO;               // R0 in the original program
Location oldlocation = 0;             // R1 in the original program
int reactor_timer = 0;                // R3 in the original program
bool married = false;                 // M9 in the original program
int score = 0;                        // S in the original program
int turns = 0;                        // T in the original program
bool wizard_mode = false;             // Z7 in the original program
int zarka_status = 0;                 // Z9 in the original; 1=friendly, 2=enraged
bool aboard_disk = false;             // D1 in the original program
bool fucked = false;                  // F9 in the original program

typedef enum Object {
    NO_OBJECT,
    LAMP, KEYS, CHEST, JEWEL, GOLD, TREE, WITCH, LEVER, GIRL, PEARL,
    NOTE, CELLO, WATCH, PARCHMENT, VASE, STAFF, COINS, KNIFE, SLIME, STOOL,
    FLASK, HILLS, BEAST, CYLINDER, ZARKA, GLOBE, SNAKE, ROCKS, FRIES, PLATE,
    FISH, CLOWN, SPRAY, PIZZA, BED, JOHN, TUB, BOOK, COMPUTER, KNOB,
    LABEL, RING, BLOCK, WATER, DOOR, GLOVE, DISC, BOARD, POST, BUTTON,
    LIFT, SPADE, STATUE, COMB, PIECES, SAPPHIRE, PLATFORM, GRATE, SLABS, WHEEL,
    BOOTS, CUBE, NURSE, NEEDLE, PILLAR, BOX, CARD, TRAIN, ART,
};

Object lookup_noun(const char *word) {
#define N(e, w) if (strncmp(word, w, strlen(w) == 0) return e;
    N(LAMP, "lamp"); N(LAMP, "lantern"); N(LAMP, "light");
    N(KEYS, "keys"); N(KEYS, "set"); N(KEYS, "iron");
    N(CHEST, "chest"); N(CHEST, "trunk");
    N(JEWEL, "jewel"); N(JEWEL, "diamond"); N(JEWEL, "gem");
    N(GOLD, "gold"); N(GOLD, "nugget");
    N(TREE, "tree"); N(TREE, "oak"); N(TREE, "carvings");
    N(WITCH, "witch"); N(WITCH, "hag");
    N(LEVER, "lever");
    N(GIRL, "girl"); N(GIRL, "woman");
    N(PEARL, "pearl");
    N(NOTE, "note");
    N(CELLO, "cello"); N(CELLO, "Stradivarius");
    N(WATCH, "watch"); N(WATCH, "timepiece");
    N(PARCHMENT, "parchment"); N(PARCHMENT, "map");
    N(VASE, "vase"); N(VASE, "Ming");
    N(STAFF, "staff"); N(STAFF, "rod"); N(STAFF, "wand");
    N(COINS, "coins"); N(COINS, "coin");
    N(KNIFE, "knife"); N(KNIFE, "butcher");
    N(SLIME, "slime"); N(SLIME, "ooze");
    N(STOOL, "stool"); N(STOOL, "footstool"); N(STOOL, "foot stool");
    N(FLASK, "flask"); N(FLASK, "bottle");
    N(HILLS, "hills"); N(HILLS, "mine"); N(HILLS, "mines"); N(HILLS, "hill"); N(HILLS, "dirt");
    N(BEAST, "beast"); N(BEAST, "gorilla");
    N(CYLINDER, "cylinder"); N(CYLINDER, "crystal"); N(CRYSTAL, "white");
    N(ZARKA, "zarka");
    N(GLOBE, "globe"); N(GLOBE, "sphere"); N(GLOBE, "coating");
    N(SNAKE, "snake"); N(SNAKE, "viper");
    N(ROCKS, "rocks"); N(ROCKS, "stones"); N(ROCKS, "rock");
    N(FRIES, "fries"); N(FRIES, "french things");
    N(PLATE, "plate"); N(PLATE, "dinner");
    N(FISH, "fish"); N(FISH, "filet"); N(FISH, "sandwich");
    N(CLOWN, "clown");
    N(SPRAY, "spray"); N(SPRAY, "disinfectant"); N(SPRAY, "can");
    N(PIZZA, "pizza"); N(PIZZA, "slice");
    N(BED, "bed");
    N(JOHN, "john");
    N(JOHN, "toilet");
    N(TUB, "tub"); N(TUB, "bathtub"); N(TUB, "bath");
    N(BOOK, "book"); N(BOOK, "yellow");
    N(COMPUTER, "computer"); N(COMPUTER, "machine");
    N(KNOB, "knob"); N(KNOB, "red");
    N(LABEL, "label"); N(LABEL, "sign"); N(LABEL, "tan");
    N(RING, "ring"); N(RING, "emerald"); N(RING, "wedding");
    N(BLOCK, "block"); N(BLOCK, "brick"); N(BLOCK, "dry ice"); N(BLOCK, "ice");
    N(WATER, "water"); N(WATER, "H20");
    N(DOOR, "door"); N(DOOR, "wooden thing");
    N(GLOVE, "glove"); N(GLOVE, "mitten"); N(GLOVE, "work");
    N(DISC, "disc"); N(DISC, "disk"); N(DISC, "saucer"); N(DISC, "silver");
    N(BOARD, "board"); N(BOARD, "bulletin"); N(BOARD, "notice");
    N(POST, "post"); N(POST, "signpost");
    N(BUTTON, "button"); N(BUTTON, "key"); N(BUTTON, "beige");
    N(LIFT, "lift"); N(LIFT, "tram"); N(LIFT, "contraption"); N(LIFT, "gondola");
    N(SPADE, "spade"); N(SPADE, "shovel"); N(SPADE, "tool");
    N(STATUE, "statue"); N(STATUE, "jade");
    N(COMB, "comb"); N(COMB, "brush"); N(COMB, "ivory");
    N(PIECES, "pieces"); N(PIECES, "shards"); N(PIECES, "pottery");
    N(SAPPHIRE, "sapphire");
    N(PLATFORM, "platform"); N(PLATFORM, "dias");
    N(GRATE, "grate"); N(GRATE, "grating");
    N(SLABS, "slabs");
    N(WHEEL, "wheel");
    N(BOOTS, "boots"); N(BOOTS, "boot"); N(BOOTS, "rubber");
    N(CUBE, "cube"); N(CUBE, "oxygum"); N(CUBE, "gum"); N(CUBE, "material");
    N(NURSE, "nurse");
    N(NEEDLE, "needle"); N(NEEDLE, "amber"); N(NEEDLE, "solution");
    N(PILLAR, "pillar"); N(PILLAR, "bronze"); N(PILLAR, "F");
    N(BOX, "box"); N(BOX, "gate"); N(BOX, "slot");
    N(CARD, "card"); N(CARD, "farecard"); N(CARD, "fare");
    N(TRAIN, "train");
    N(ART, "art"); N(ART, "painting"); N(ART, "picture");
    return NO_OBJECT;
}

void describe_object(Object obj)
{
    switch (obj) {
        case DIAMOND: putsn("A large diamond is lying here!"); break;
        case EYE: putsn("There's an emerald eye here!"); break;
        case CHAIN: putsn("A platinum chain is lying here!"); break;
        case APPLE: putsn("There is a golden apple here!"); break;
        case PEARLS: putsn("A string of pearls is lying here!"); break;
        case SPICES: putsn("There's a bottle of rare spices lying here!"); break;
        case DAGGER: putsn("There is a jewel-encrusted dagger here!"); break;
        case FIGURINE: putsn("A priceless figurine is sitting on some hay!"); break;
        case LAMP: putsn("There is an old oil lamp here."); break;
        case KEY: putsn("An old brass key is sitting here."); break;
        case RING: putsn("There is a worthless-looking ring here."); break;
        case ROD: putsn("An old black rod is sitting here."); break;
        case STAFF: putsn("There's an ancient-looking staff lying here."); break;
        case INCENSE: putsn("There's a package of incense lying here."); break;
        case HAY: putsn("There's a bale of hay here."); break;
        case CHEESE: putsn("A package of Kraft cheese is lying here."); break;
        case MOUSE: putsn("There is a noisy little mouse here."); break;
        case MATCHES: putsn("There's a box of matches lying here."); break;
        case BOTTLE: putsn("There's an empty bottle here."); break;
        case WATER_BOTTLE: putsn("There's a brownish bottle of water here."); break;
        case EYE_IN_PLACE: putsn("An emerald eye sits on top of the idol."); break;
        case GNOME: putsn("There is a knife-wielding gnome in the room with you!"); break;
        case SPICES_IN_PLACE: putsn("There are rare spices sitting on the ground."); break;
        case WIZARD: putsn("An angry wizard wearing a platinum chain is in the room with you!"); break;
        case SNAKE: putsn("A giant snake is jealously guarding a golden apple."); break;
        case BALL: putsn("There is a large crystal ball on the ground."); break;
        case FIGURINE_IN_PLACE: putsn("A priceless figurine stands on the table."); break;
        case OPEN_WINDOW: putsn("The window is open."); break;
        case CLOSED_WINDOW: putsn("The window is closed."); break;
        case OPEN_PANEL: putsn("There is a panel open to the north."); break;
        default: assert(false);
    }
}

bool has_darkness(int loc)
{
    switch (loc) {
        case R_BROKEN: case R_REMAINS: case R_HALL1:
        case R_TOOL: case R_CORR: case R_MAZE18: case R_MAZE19:
        case R_MAZE20: case R_MAZE21: case R_MAZE22: case R_MAZE23:
        case R_TALL: case R_HALL2: case R_MEET: case R_CHASM:
        case R_LEDGE: case R_NARROW: case R_MINE33: case R_MINE34:
        case R_MINE35: case R_MINE36: case R_MINE37: case R_MINE38:
            return true;
        default:
            return false;
    }
}

bool has_river(Location loc)
{
    return (loc == R_RIVER || loc == R_WELCOME || loc == R_RIVERFRONT);
}

void describe_room(Location loc)
{
    bool has_light = (toting(LAMP) || there(LAMP, loc)) && lamp_is_on;
    if (has_darkness(loc) && !has_light) {
        putsn("There is insufficient light to see by.");
        if (darkness_counter == 0) {
            darkness_counter = 1;
        }
    } else {
        darkness_counter = 0;
        if (places[loc].visited || places[loc].long_desc == NULL) {
            putsn(places[loc].short_desc);
        } else {
            putsn(places[loc].long_desc);
            places[loc].visited = true;
        }
        for (int i = DIAMOND; i <= OPEN_PANEL; ++i) {
            if (there(i, loc) && i != GNOME) {
                describe_object(i);
            }
        }
    }
    if (loc == R_HILL && !heard_voiceover) {
        putsn("A loud voice emanates from beyond, saying: \"Time grows short," SOFT_NL
              "mortal, before we feast again.\"");
        heard_voiceover = true;
    }
}

// Line 7000 in the original program
Location attempt_move_to_room(Location loc, Location nextloc)
{
    if (!river_is_open && ((loc == R_RIVER && nextloc == R_WELCOME) ||
                           (loc == R_WELCOME && nextloc == R_RIVER))) {
        putsn("You cannot cross the river.");
        return loc;
    } else if (!chasm_is_bridged && ((loc == R_CHASM && nextloc == R_LEDGE) ||
                                     (loc == R_LEDGE && nextloc == R_CHASM))) {
        putsn("The abyss is too wide to cross.");
        return loc;
    } else if (loc == R_BASEMENT && nextloc != R_BASEMENT && !wizard_is_dead) {
        putsn("The wizard blocks your way.");
        return loc;
    } else if (loc == R_CLOSET && nextloc == R_LIFT && !wizard_is_dead) {
        putsn("There is no way to go in that direction.");
        return loc;
    }

    loc = nextloc;

    describe_room(loc);
    return loc;
}

void describe_inventory_object(Object x)
{
    switch (x) {
        case DIAMOND: puts("    Large diamond"); break;
        case EYE: puts("    Emerald eye"); break;
        case CHAIN: puts("    Platinum chain"); break;
        case APPLE: puts("    Golden apple"); break;
        case PEARLS: puts("    String of pearls"); break;
        case SPICES: puts("    Bottle of spices"); break;
        case DAGGER: puts("    Jeweled dagger"); break;
        case FIGURINE: puts("    Priceless figurine"); break;
        case LAMP: puts("    Oil lamp"); break;
        case KEY: puts("    Brass key"); break;
        case RING: puts("    Ring"); break;
        case ROD: puts("    Black rod"); break;
        case STAFF: puts("    Wooden staff"); break;
        case INCENSE: puts("    Incense"); break;
        case HAY: puts("    Hay"); break;
        case CHEESE: puts("    Kraft cheese"); break;
        case MOUSE: puts("    Noisy mouse"); break;
        case MATCHES: puts("    Matches"); break;
        case BOTTLE: puts("    Empty bottle"); break;
        case WATER_BOTTLE: puts("    Bottle of water"); break;
        default: assert(false);
    }
}

// Line 400 in the original program
void attempt_inventory()
{
    puts("You are carrying:");
    for (int x = DIAMOND; x <= WATER_BOTTLE; ++x) {
        if (toting(x)) {
            describe_inventory_object(x);
        }
    }
    if (burden() == 0) {
        puts("    Nothing");
    }
    extra_newline();
}

// Line 600 in the original program
int attempt_score(bool winning)
{
    int pt = 0;
    for (int x = DIAMOND; x <= FIGURINE; ++x) {
        if (toting(x)) pt += 7;
        else if (there(x, R_VAULT)) pt += 10;
    }
    pt -= 10 * death_counter;
    pt -= hint_penalties;
    if (winning) pt += 10;
    printf("You have scored %d points.\n", pt);
    extra_newline();
    return pt;
}

// Line 7300 in the original program
void final_score_and_exit(bool winning)
{
    int pt = attempt_score(winning);
    if (pt == 90) putsn("All honor thee, Grandmaster Adventurer!");
    else if (pt >= 80) putsn("You are now an advanced adventurer.");
    else if (pt >= 36) putsn("You may now consider yourself an intermediate in this game.");
    else if (pt >= 8) putsn("You are obviously a rank novice in this game.");
    else putsn("You obviously have no idea what's happening in this game.");
#ifdef Z_MACHINE
    puts("\n\n");
#endif /* Z_MACHINE */
    exit(0);
}

// Line 7210 in the original program
Location random_death_drop_room()
{
    // In the original program, R_LIMBO was a possible outcome here.
    return R_START + ran(R_LIGHTED - R_START + 1);
}

// Line 7200 in the original program
Location death()
{
    death_counter += 1;
    if (death_counter == 3) {
        putsn("You have died three times. The game is therefore suspended.");
        final_score_and_exit(false);
    }
    if (!yes("Do you want another chance?")) {
        final_score_and_exit(false);
    }

    // Reincarnate the player.
    if (death_counter == 1) {
        putsn("Everything spins around you as a wall of fire encircles you." SOFT_NL
              "When you wake up you find that...");
    } else {
        putsn("A small wall of fire barely surrounds you. You regain" SOFT_NL
              "consciousness and find that...");
    }
    for (int x = DIAMOND; x <= WATER_BOTTLE; ++x) {
        // In the original program, this loop didn't include WATER_BOTTLE.
        if (toting(x)) apport(x, random_death_drop_room());
    }
    apport(LAMP, R_SHACK);
    apport(MATCHES, R_FOREST3);
    apport(STAFF, R_ROCK);
    lamp_is_on = false;
    river_is_open = false;
    river_counter = 0;
    chased_by_gnome = false;
    return attempt_move_to_room(R_LIMBO, R_START);
}

// Line 700 in the original program
void attempt_quit()
{
    if (yes("Are you sure you want to quit?")) {
        final_score_and_exit(false);
    } else {
        putsn("OK.");
    }
}

bool is_portable(Object obj)
{
    return (DIAMOND <= obj && obj <= BOTTLE);
}

// Line 2660 in advent.txt
void die_by_dry_ice() {
    puts("The dry ice instantly freezes your hand, and you die from shock.");
    exit();
}

// Line 2460 in advent.txt
void attempt_get() {
    extract_object();
    if (active_object == NULL) {
        // Line 2940
        for (Object i = LAMP; i <= ART; ++i) {
            if (object_location[i] == loc) {
                active_object = name_of_object(i);
                break;
            }
        }
    }
    if (active_object == NULL) {
        puts("I see nothing to take.");
    } else {
        Object obj = lookup_noun(active_object);
        if (obj == NO_OBJECT || object_location[obj] != loc) {
            printf("I see no %s here.\n", active_object);
        } else if (object_weight[obj] == -1) {
            printf("Not only is taking the %s illegal, it is also very dangerous!\n", active_object);
        } else if (object_weight[obj] == 500) {
            switch (ran(4)) {
                case 0: printf("%s: A valiant attempt.\n", active_object); break;
                case 1: printf("%s: Want a hernia?\n", active_object); break;
                case 2: printf("%s: Close but no cigar.\n", active_object); break;
                case 3: printf("%s: Nice try.\n", active_object); break;
            }
        } else if (obj == BLOCK && !toting(GLOVE)) {
            die_by_dry_ice();
        } else if (obj == GLOBE && there(ZARKA, R_LIMBO)) {
            puts("As you reach for the globe, an alarm rings briefly, and a horrible" SOFT_NL
                 "Zarka appears from inside to defend it!");
            apport(ZARKA, loc);
            default_object = "Zarka";
        } else if (obj == 26 && zarka_status == 0 && object_location[26] == loc) {
            puts("The Zarka won't let you have the globe.");
        } else {
            if (obj == 11 && object_location[28] == 0) {
                puts("Avalanche!  Huge boulders!  Aaaaaah!");
                apport(28, loc);
            }
            if (current_load + object_weight[obj] > 150) {
                puts("You're load is too heavy.  You'll have to drop something first.");
            } else {
                current_load += object_weight[obj];
                apport(obj, R_INHAND);
                printf("%s: Taken.\n", active_object);
                if (loc == 3) {
                    score -= 10 * (obj != LAMP);
                }
            }
        }
    }
}

// Line 3060 in advent.txt
void attempt_drop() {
    extract_object();
    if (active_object == NULL) {
        printf("Drop what? "); fflush(stdout);
        static char input[130];
        fgets(input, sizeof input, stdin);
        for (char *p = input; *p != '\0'; ++p) {
            *p = toupper(*p);
        }
        active_object = input;
        if (active_object[0] == '\0') {
            return;
        }
    }
    Object obj = lookup_noun(active_object);
    if (obj == NO_OBJECT) {
        printf("You don't have the %s.\n", active_object);
    } else {
        active_object = spelling_of(obj);
        if (!toting(obj)) {
            printf("I see no %s here.\n", active_object);
        } else if (obj == VASE && !there(STOOL, loc)) {
            puts("The vase falls to the floor and smashes into a million pieces!");
            apport(VASE, R_LIMBO);
            apport(SHARDS, loc);
            score -= 30;
            current_load -= 30;
        } else {
            puts("Dropped.");
            apport(obj, loc);
            if (loc == R_QUICK && obj == BLOCK) {
                puts("As the block sinks, the quicksand instantly freezes over hard enough" SOFT_NL
                     "to support your weight.");
                apport(BLOCK, R_LIMBO);
                quicksand_timer = 100;
            } else if (obj == GLOVE && toting(BLOCK)) {
                die_by_dry_ice();
            }
            current_load -= object_weight[obj];
            if (loc == R_HOUSE) {
                score += 10 * (obj != LAMP);
            } else {
                score -= object_weight[obj];
            }
        }
    }
}

void print_default_message(Object iobj)
{
    if (toting(iobj)) {
        putsn("Nothing happens.");
    } else if (is_portable(iobj)) {
        putsn("You're not carrying it.");
    } else {
        putsn("I don't understand.");
    }
}

// Line 1200 in the original program
void attempt_wave(Object iobj, Location loc)
{
    if (iobj == STAFF && toting(STAFF) && (loc == R_RIVER || loc == R_WELCOME)) {
        if (river_is_open) {
            putsn("A loud noise surrounds you as the river closes.");
            river_is_open = false;
        } else {
            putsn("Holy Moses! The river just split in two! A dry path leads to" SOFT_NL
                  "the other side.");
            river_is_open = true;
        }
    } else if (iobj == ROD && toting(ROD) && (loc == R_CHASM || loc == R_LEDGE)) {
        if (chasm_is_bridged) {
            putsn("The bridge has just as suddenly disappeared.");
            chasm_is_bridged = false;
        } else {
            putsn("A crystal bridge now spans the chasm.");
            chasm_is_bridged = true;
        }
    } else {
        print_default_message(iobj);
    }
}

// Line 1300 in the original program
void attempt_show(Object iobj, Location loc)
{
    if (iobj == RING && toting(RING) && loc == R_BASEMENT && !wizard_is_dead) {
        putsn("The ring glows brightly. A lightning bolt strikes the wizard!");
        putsn("A platinum chain lies before you.");
        wizard_is_dead = true;
        apport(CHAIN, loc);
        apport(WIZARD, R_LIMBO);
    } else {
        print_default_message(iobj);
    }
}

// Line 1400 in the original program
Location attempt_read(Object iobj, Location loc)
{
    if (iobj != MESSAGE && iobj != SIGN && iobj != BOOK) {
        putsn("I don't understand.");
    } else if (loc == R_HALL2 && iobj == MESSAGE) {
        putsn("\"Merlin was here.\"");
    } else if (loc == R_TREE && iobj == MESSAGE) {
        putsn("\"The great Almazar bids you well. Though you will encounter many" SOFT_NL
              "trials, he shall provide for you. He that is both water and flame" SOFT_NL
              "shall send you a gift to aid you in your quest. Thus saith" SOFT_NL
              "Almazar: 'Live, and you shall live.'\"");
    } else if (loc == R_RIVER && iobj == SIGN) {
        putsn("\"Fish from the Amazon.\"");
    } else if (loc == R_WELCOME && (iobj == MESSAGE || iobj == SIGN)) {
        // The original game didn't accept "READ SIGN" here, only "READ MESSAGE".
        putsn("\"Discretion will prevent transgression.\"");
    } else if (loc == R_LIBRARY && iobj == BOOK) {
        if (there(OPEN_WINDOW, loc)) {
            putsn("A small axe flies through the window and strikes you dead.");
            return death();
        } else {
            putsn("\"And so it came to pass that in the third day of the seventh" SOFT_NL
                  "month of the twenty-first year a great event happened. The earth" SOFT_NL
                  "opened and took within it the fruits of the people's labor and" SOFT_NL
                  "gave upon them a greater glory.\"");
        }
    } else if (loc == R_OUTHOUSE && iobj == MESSAGE) {
        bool collected_all_the_treasures = true;
        for (Object x = DIAMOND; x <= FIGURINE; ++x) {
            if (!there(x, R_VAULT)) collected_all_the_treasures = false;
        }
        if (collected_all_the_treasures && toting(RING)) {
            putsn("In a blaze of glory you find yourself in a land far away!");
            final_score_and_exit(true);
        } else {
            putsn("\"Frodo lives!\"");
        }
    } else {
        putsn("I don't see it here.");
    }
    return loc;
}

// Line 1500 in the original program
void attempt_fill(Object iobj, Location loc)
{
    if (iobj == LAMP) {
        if (!toting(LAMP)) {
            putsn("You don't have a lamp.");
        } else if (loc != R_OIL) {
            putsn("There is nothing to fill it with.");
        } else {
            attempt_get(OIL, loc);
        }
    } else if (iobj == BOTTLE) {
        if (!toting(SPICES) && !toting(BOTTLE) && !toting(WATER_BOTTLE)) {
            putsn("You're not carrying a bottle.");
        } else if (!has_river(loc)) {
            putsn("There is nothing to fill it with.");
        } else {
            attempt_get(WATER, loc);
        }
    } else {
        putsn("I don't understand.");
    }
}

// Line 1600 in the original program
void attempt_light(Object iobj, Location loc)
{
    if (iobj == LAMP) {
        if (!toting(LAMP)) {
            putsn("You're not carrying it.");
        } else if (lamp_is_on) {
            putsn("The lamp is already lit.");
        } else if (lamp_counter <= 0) {
            putsn("You cannot light an empty oil lamp.");
        } else if (!toting(MATCHES)) {
            putsn("You don't have any matches.");
        } else {
            putsn("OK.");
            lamp_is_on = true;
            darkness_counter = 0;
        }
    } else if (iobj == INCENSE) {
        if (!toting(INCENSE)) {
            putsn("You're not carrying it.");
        } else if (!toting(MATCHES)) {
            putsn("You don't have any matches.");
        } else {
            apport(INCENSE, R_LIMBO);
            if (loc == R_IDOL) {
                putsn("An emerald eye falls to the ground.");
                apport(EYE_IN_PLACE, R_LIMBO);
                apport(EYE, loc);
            } else {
                putsn("OK.");
            }
        }
    } else {
        putsn("I don't understand.");
    }
}

// Line 1700 in the original program
Location attempt_kill(Object iobj, Location loc)
{
    if (iobj == SNAKE || iobj == WIZARD || iobj == GNOME || iobj == BALL) {
        if (!there(iobj, loc)) {
            putsn("I don't see it here.");
        } else if (!yes("With what, your bare hands?")) {
            putsn("Nothing happens.");
        } else if (iobj == SNAKE) {
            putsn("The snake severs your hand. You have been poisoned.");
            return death();
        } else if (iobj == WIZARD) {
            putsn("The wizard raises his hand and in an instant you find yourself" SOFT_NL
                  "uncomfortably hot. As you look up into the sky you see a large" SOFT_NL
                  "stone idol. You are being offered as a sacrifice.");
            return death();
        } else if (iobj == GNOME) {
            putsn("The gnome is too fast for you.");
        } else {
            assert(iobj == BALL);
            putsn("Eech! Your hands just turned into a bloody mess.");
        }
    } else {
        putsn("I don't understand.");
    }
    return loc;
}

// Line 1800 in the original program
Location attempt_kick(Object iobj, Location loc)
{
    if (iobj == BALL) {
        if (there(BALL, loc)) {
            putsn("Ouch! Every bone in your foot just broke.");
        } else {
            putsn("I don't see it here.");
        }
    } else if (iobj == WIZARD) {
        putsn("The wizard raises his hand and in an instant you find yourself" SOFT_NL
              "uncomfortably hot. As you look up into the sky you see a large" SOFT_NL
              "stone idol. You are being offered as a sacrifice.");
        wizard_counter = 0;
        return death();
    } else if (!is_portable(iobj)) {
        putsn("I don't understand.");
    } else if (iobj == FIGURINE && toting(FIGURINE)) {
        // The original game allowed KICK FIGURINE to safely drop the figurine sans hay.
        putsn("The delicate figurine breaks upon hitting the ground.");
        apport(FIGURINE, R_LIMBO);
    } else {
        if (toting(iobj)) {
            apport(iobj, loc);
            putsn("OK.");
        } else if (there(iobj, loc)) {
            putsn("You're not carrying it.");
        } else {
            putsn("I don't see it here.");
        }
    }
    return loc;
}

// Line 1900 in the original program
void attempt_break(Object iobj, Location loc)
{
    if (iobj == BALL) {
        if (there(BALL, loc)) {
            attempt_kill(BALL, loc);
        } else {
            putsn("I don't see it here.");
        }
    } else if (iobj == FIGURINE && toting(FIGURINE)) {
        putsn("The delicate figurine breaks upon hitting the ground.");
        apport(FIGURINE, R_LIMBO);
    } else if (iobj == BOTTLE && toting(BOTTLE)) {
        putsn("The bottle breaks on impact.");
        apport(BOTTLE, R_LIMBO);
    } else if (iobj == BOTTLE && toting(WATER_BOTTLE)) {
        putsn("The bottle breaks on impact.");
        apport(WATER_BOTTLE, R_LIMBO);
    } else if ((iobj == BOTTLE || iobj == SPICES) && toting(SPICES)) {
        putsn("The bottle breaks on impact.");
        putsn("A sudden wind blows from behind and scatters the spices to the" SOFT_NL
              "four corners of the earth.");
        apport(SPICES, R_LIMBO);
    } else {
        print_default_message(iobj);
    }
}

// Line 2000 in the original program
void attempt_toss(Object iobj, Location loc)
{
    if (iobj == BOTTLE || iobj == SPICES) {
        attempt_break(iobj, loc);
    } else if (iobj == DAGGER && toting(DAGGER) && there(GNOME, loc)) {
        int p = ran(100) - burden();
        if (p > 40) {
            putsn("You killed a nasty knife-throwing gnome!");
            apport(GNOME, R_LIMBO);
        } else {
            putsn("You missed! You ought to get your eyes examined.");
        }
        apport(DAGGER, loc);
    } else {
        attempt_drop(iobj, loc);
    }
}

// Line 2100 in the original program
Location attempt_eat(Object iobj, Location loc)
{
    if (iobj == MOUSE && toting(MOUSE)) {
        putsn("Yuck! The mouse was used for carcinogenic testing. You're dead.");
        apport(MOUSE, R_LIMBO);
        return death();
    }

    if (iobj == SPICES && (there(SPICES, loc) || toting(SPICES))) {
        putsn("Whew! That's strong stuff!");
        apport(BOTTLE, object_position[SPICES]);
        apport(SPICES, R_LIMBO);
    } else if (iobj == SPICES && there(SPICES_IN_PLACE, loc)) {
        putsn("Whew! That's strong stuff!");
        apport(SPICES_IN_PLACE, R_LIMBO);
    } else if (iobj == CHEESE && (there(CHEESE, loc) || toting(CHEESE))) {
        putsn("The cheese was stale, but you managed to consume the entire box.");
        apport(CHEESE, R_LIMBO);
    } else if (iobj == MOUSE && there(MOUSE, loc)) {
        putsn("The mouse runs away.");
        apport(MOUSE, R_LIMBO);
    } else {
        putsn("The only thing edible here is you!");
    }
    return loc;
}

// Line 2200 in the original program
void attempt_drink(Object iobj, Location loc)
{
    if (iobj == WATER) {
        if (toting(WATER_BOTTLE)) {
            putsn("Eech! What lousy-tasting water.");
            apport(WATER_BOTTLE, R_LIMBO);
            apport(BOTTLE, -1);
        } else if (has_river(loc)) {
            putsn("Eech! What lousy-tasting water.");
        } else {
            putsn("There's no water within reach.");
        }
    } else {
        putsn("I don't understand.");
    }
}

// Line 2300 in the original program
void attempt_feed(Object iobj, Location loc)
{
    if (iobj == MOUSE) {
        if (!there(MOUSE, loc)) {
            putsn("I don't see it here.");
        } else if (!toting(CHEESE)) {
            putsn("The only thing edible here is you!");
        } else {
            putsn("OK.");
            mouse_is_fed = true;
        }
    } else if (iobj == SNAKE) {
        if (!there(SNAKE, loc)) {
            puts("I don't see it here.");
        } else if (!toting(MOUSE)) {
            puts("The only thing edible here is you!");
        } else {
            putsn("The mouse was used for carcinogenic testing. The snake is dead.");
            putsn("A golden apple hangs by an apple tree.");
            apport(MOUSE, R_LIMBO);
            apport(SNAKE, R_LIMBO);
            apport(APPLE, loc);
        }
    } else {
        putsn("I don't understand.");
    }
}

// Line 2400 in the original program
void attempt_turn(Object iobj, Location loc)
{
    if (iobj == HEAD && loc == R_LIVING) {
        if (there(OPEN_PANEL, loc)) {
            putsn("The panel closes and seals the passage.");
            apport(OPEN_PANEL, R_LIMBO);
        } else {
            putsn("A panel slides open, revealing a passage to the north.");
            apport(OPEN_PANEL, loc);
        }
    } else {
        print_default_message(iobj);
    }
}

// Line 2500 in the original program
void attempt_unlock(Object iobj, Location loc)
{
    // Notice that unlocking an open container makes it no longer open.
    if (iobj == VAULT) {
        if (loc != R_OUTSIDE) {
            putsn("I don't see it here.");
        } else if (!vault_challenge()) {
            putsn("That combination is incorrect.");
        } else {
            putsn("You have managed to unlock the vault.");
            vault_status = STATUS_UNLOCKED;
        }
    } else if (iobj == CHEST) {
        if (loc != R_ATTIC) {
            putsn("I don't see it here.");
        } else if (!toting(KEY)) {
            putsn("You have no keys.");
        } else {
            putsn("OK.");
            chest_status = STATUS_UNLOCKED;
        }
    } else if (iobj == DRAWER) {
        if (loc != R_BEDROOM) {
            putsn("I don't see it here.");
        } else if (!toting(KEY)) {
            putsn("You have no keys.");
        } else {
            putsn("OK.");
            drawer_status = STATUS_UNLOCKED;
        }
    } else {
        puts("I don't understand.");
    }
}

// Line 500 in the original program
Location attempt_jump(Location loc)
{
    if (loc == R_CHASM || loc == R_LEDGE) {
        putsn("You fell into the chasm. You are dead.");
        return death();
    } else if (loc == R_HILL) {
        putsn("That jump would have won a gold medal. Unfortunately, you landed" SOFT_NL
              "head-first on a pile of rocks.");
        return death();
    } else if (has_river(loc)) {
        // It doesn't matter whether the river is open.
        putsn("You did not jump far enough. You fell into the river.");
        putsn("A school of piranhas devours you.");
        return death();
    } else {
        putsn("Nothing happens.");
        return loc;
    }
}

// Line 2600 in the original program
Location attempt_cross(Object iobj, Location loc)
{
    if (iobj == RIVER && (loc == R_RIVER || loc == R_WELCOME)) {
        if (river_is_open) {
            return attempt_move_to_room(loc, (loc == R_RIVER) ? R_WELCOME : R_RIVER);
        } else {
            putsn("You can't cross the river.");
        }
    } else if (iobj == CHASM && (loc == R_CHASM || loc == R_LEDGE)) {
        if (chasm_is_bridged) {
            return attempt_move_to_room(loc, (loc == R_CHASM) ? R_LEDGE : R_CHASM);
        } else {
            putsn("The abyss is too wide to cross.");
        }
    } else if (iobj == RIVER || iobj == CHASM) {
        putsn("I don't see it here.");
    } else {
        putsn("I don't understand.");
    }
    return loc;
}

// Line 2700 in the original program
Location attempt_swim(Object iobj, Location loc)
{
    if (iobj == RIVER || iobj == WATER) {
        if (has_river(loc)) {
            putsn("A school of piranhas devours you.");
            return death();
        } else {
            putsn("There's no water within reach.");
        }
    } else {
        putsn("I don't understand.");
    }
    return loc;
}

// Line 2800 in the original program
void attempt_open(Object iobj, Location loc)
{
    if (iobj == VAULT) {
        if (loc != R_OUTSIDE) {
            putsn("I don't see it here.");
        } else if (vault_status == STATUS_OPEN) {
            putsn("It's already open.");
        } else if (vault_status == STATUS_LOCKED) {
            putsn("The vault is locked.");
        } else {
            putsn("OK.");
            vault_status = STATUS_OPEN;
        }
    } else if (iobj == CHEST) {
        if (loc != R_ATTIC) {
            putsn("I don't see it here.");
        } else if (chest_status == STATUS_OPEN) {
            putsn("It's already open.");
        } else if (chest_status == STATUS_LOCKED) {
            putsn("It's locked.");
        } else {
            putsn("The chest is empty.");
            chest_status = STATUS_OPEN;
        }
    } else if (iobj == DRAWER) {
        if (loc != R_BEDROOM) {
            putsn("I don't see it here.");
        } else if (drawer_status == STATUS_OPEN) {
            putsn("It's already open.");
        } else if (drawer_status == STATUS_LOCKED) {
            putsn("It's locked.");
        } else {
            drawer_status = STATUS_OPEN;
            if (there(PEARLS, R_LIMBO)) {
                putsn("A string of pearls is lying in the drawer!");
                apport(PEARLS, loc);
            } else {
                putsn("OK.");
            }
        }
    } else if (iobj == WINDOW) {
        if (loc != R_LIBRARY) {
            putsn("I don't see it here.");
        } else if (there(OPEN_WINDOW, loc)) {
            putsn("It's already open.");
        } else {
            putsn("OK.");
            apport(OPEN_WINDOW, loc);
            apport(CLOSED_WINDOW, R_LIMBO);
        }
    } else {
        putsn("I don't understand.");
    }
}

// Line 2900 in the original program
void attempt_close(Object iobj, Location loc)
{
    if (iobj == VAULT) {
        if (loc != R_OUTSIDE) {
            putsn("I don't see it here.");
        } else if (vault_status == STATUS_LOCKED) {
            putsn("It's already closed.");
        } else {
            putsn("OK.");
            vault_status = STATUS_LOCKED;
        }
    } else if (iobj == CHEST) {
        if (loc != R_ATTIC) {
            putsn("I don't see it here.");
        } else if (chest_status == STATUS_LOCKED) {
            putsn("It's already closed.");
        } else {
            putsn("OK.");
            chest_status = STATUS_LOCKED;
        }
    } else if (iobj == DRAWER) {
        if (loc != R_BEDROOM) {
            putsn("I don't see it here.");
        } else if (drawer_status == STATUS_LOCKED) {
            putsn("It's already closed.");
        } else {
            putsn("OK.");
            drawer_status = STATUS_LOCKED;
        }
    } else if (iobj == WINDOW) {
        if (loc != R_LIBRARY) {
            putsn("I don't see it here.");
        } else if (there(CLOSED_WINDOW, loc)) {
            putsn("It's already closed.");
        } else {
            putsn("OK.");
            apport(CLOSED_WINDOW, loc);
            apport(OPEN_WINDOW, R_LIMBO);
        }
    } else {
        puts("I don't understand.");
    }
}

// Line 3000 in the original program
void attempt_off(Object iobj)
{
    if (iobj != LAMP) {
        putsn("I don't understand.");
    } else if (!toting(LAMP)) {
        putsn("You're not carrying it.");
    } else if (!lamp_is_on) {
        putsn("Your lamp is already off.");
    } else {
        putsn("OK.");
        lamp_is_on = false;
    }
}

// Line 3100 in the original program
Location attempt_pull(Object iobj, Location loc)
{
    if (iobj == ROPE && loc == R_HIDDEN) {
        putsn("A trap door opens beneath you. The fall knocks you unconscious. When" SOFT_NL
              "you awaken you find that...\n");
        return attempt_move_to_room(R_LIMBO, R_BASEMENT);
    } else {
        print_default_message(iobj);
        return loc;
    }
}

// Line 3200 in the original program
void attempt_empty(Object iobj)
{
    if (iobj == BOTTLE) {
        if (toting(SPICES)) {
            putsn("A sudden wind blows from behind and scatters the spices to the" SOFT_NL
                  "four corners of the earth.");
            apport(SPICES, R_LIMBO);
            apport(BOTTLE, -1);
        } else if (toting(WATER_BOTTLE)) {
            putsn("OK.");
            apport(WATER_BOTTLE, R_LIMBO);
            apport(BOTTLE, -1);
        } else if (toting(BOTTLE)) {
            putsn("Your bottle does not contain anything.");
        } else {
            putsn("You're not carrying it.");
        }
    } else {
        print_default_message(iobj);
    }
}

// Line 3300 in the original program
Location attempt_enter(Location loc)
{
    int nextloc = loc;
    switch (loc) {
        case R_START: nextloc = R_SHACK; break;
        case R_CAVE: nextloc = R_FILTER; break;
        case R_MINE: nextloc = R_MINE33; break;
        case R_HOUSE: nextloc = R_FOYER; break;
        case R_OUTOUT: nextloc = R_OUTHOUSE; break;
        case R_OUTSIDE: {
            if (vault_status == STATUS_OPEN) {
                nextloc = R_VAULT;
            } else {
                putsn("You have to open the vault to get inside it.");
            }
            break;
        }
        default: {
            putsn("You can't enter anything from here.");
            break;
        }
    }
    if (nextloc != loc) {
        return attempt_move_to_room(loc, nextloc);
    } else {
        return loc;
    }
}

// Line 3400 in the original program
Location attempt_leave(Location loc) {
    int nextloc = loc;
    switch (loc) {
        case R_SHACK: nextloc = R_START; break;
        case R_FILTER: nextloc = R_CAVE; break;
        case R_MINE33: nextloc = R_MINE; break;
        case R_FOYER: nextloc = R_HOUSE; break;
        case R_OUTHOUSE: nextloc = R_OUTOUT; break;
        case R_VAULT: nextloc = R_OUTSIDE; break;
        default: {
            putsn("You cannnot leave from here.");
            break;
        }
    }
    if (nextloc != loc) {
        return attempt_move_to_room(loc, nextloc);
    } else {
        return loc;
    }
}

void initialize_rooms()
{
#define new_place(loc, sdesc, ldesc) \
    ++i; assert(i == loc); places[i].visited = false; \
    places[i].long_desc = ldesc; places[i].short_desc = sdesc
#define n_to(loc) places[i].neighbors[NORTH - NORTH] = R_##loc
#define e_to(loc) places[i].neighbors[EAST - NORTH] = R_##loc
#define s_to(loc) places[i].neighbors[SOUTH - NORTH] = R_##loc
#define w_to(loc) places[i].neighbors[WEST - NORTH] = R_##loc
#define u_to(loc) places[i].neighbors[UP - NORTH] = R_##loc
#define d_to(loc) places[i].neighbors[DOWN - NORTH] = R_##loc
#define object(obj) apport(obj, i);

    Location i = 0;
    new_place(R_START, "You are outside an old shack.",
        "You are standing at the entrance to an old abandoned shack. To" SOFT_NL
        "the west is a rocky path. A road goes north.");
        n_to(ROAD); e_to(SHACK); w_to(STREWN);
    new_place(R_SHACK, "You are inside the shack.",
        "You are inside an old shack. There is a door to the west.");
        w_to(START); object(LAMP); object(KEY);
    new_place(R_STREWN, "You are on the edge of a large path.",
        "You are standing at the edge of a rock-strewn path. The path" SOFT_NL
        "continues east. Small walks lead to the north and west.");
        e_to(START); w_to(FOREST1); n_to(FOREST3);
    new_place(R_FOREST1, "You are in a forest.", NULL);
        n_to(FOREST2); e_to(STREWN);
    new_place(R_FOREST2, "You are in a forest.", NULL);
        s_to(FOREST1); e_to(FOREST3);
    new_place(R_FOREST3, "You are in a forest.", NULL);
        s_to(STREWN); w_to(FOREST2); object(MATCHES);
    new_place(R_ROAD, "You're in the middle of a road.",
        "You are in the middle of a road. The road continues to the north" SOFT_NL
        "and south. A small path leads eastward.");
        s_to(START); n_to(JUNC); e_to(OIL);
    new_place(R_OIL, "You are by a murky pool.",
        "You are on the edge of a murky pool. A dark liquid is floating" SOFT_NL
        "on top of the pool. There is a small path from the north. A" SOFT_NL
        "small walk leads west.");
        n_to(TREE); w_to(ROAD);
    new_place(R_JUNC, "You're at three-way junction.",
        "You are at a three-way junction. A road goes north and south." SOFT_NL
        "There is a passage leading west.");
        n_to(TEE); s_to(ROAD); w_to(CAVE);
    new_place(R_CAVE, "You're at entrance to cave.",
        "You are at an entrance to a small cave. A small narrow hole is" SOFT_NL
        "west. A passage leads east. On top of the hole a sign reads:" SOFT_NL
        "\"Beware, brave adventurer. For it is the small things in life" SOFT_NL
        "that so often destroy it.\"");
        e_to(JUNC); w_to(FILTER);
    new_place(R_FILTER, "You're in the place where light filters from the east.",
        "You are inside a cave. Light filters through from the east. A" SOFT_NL
        "small path leads west. A smaller path leads south.");
        e_to(CAVE); w_to(NARROW); s_to(BROKEN);
    new_place(R_BROKEN, "You're in passage of broken rocks.",
        "The passage is full of broken rocks. The path continues to the" SOFT_NL
        "north and south.");
        n_to(FILTER); s_to(REMAINS);
    new_place(R_REMAINS, "You're inside small cavern.",
        "You are inside a small cavern. Scattered throughout the room are" SOFT_NL
        "the remains of less fortunate adventurers. A path leads north." SOFT_NL
        "A passage leads west.");
        n_to(BROKEN); w_to(HALL1);
    new_place(R_HALL1, "You're in long hall.",
        "You are in a long hall extending east and west.");
        w_to(LIGHTED); e_to(REMAINS);
    new_place(R_LIGHTED, "This is a room lighted by a sign.",
        "You are in a room lighted by a sign that says, \"Gnomes need few" SOFT_NL
        "tools to do their dirty deeds.\" Passages lead east and west. A" SOFT_NL
        "small path leads south.");
        w_to(CORR); e_to(HALL1); s_to(TOOL); object(DAGGER);
    new_place(R_TOOL, "You're in the tool room.",
        "This is the tool room. Strewn all over the floor are small" SOFT_NL
        "knives which disappear to the touch. A passage is north." SOFT_NL
        "A walk leads up.");
        u_to(LEDGE); n_to(LIGHTED);
    new_place(R_CORR, "You're in long corridor.",
        "You are in a long corridor leading east and west.");
        w_to(MAZE18); e_to(LIGHTED);
    new_place(R_MAZE18, "You are in a twisty little maze of passages.", NULL);
        n_to(MAZE18); e_to(MAZE18); s_to(MAZE19); w_to(MAZE23);
        u_to(MAZE18); d_to(MAZE18);
    new_place(R_MAZE19, "You are in a maze of twisty little passages.", NULL);
        n_to(MAZE18); e_to(MAZE19); s_to(MAZE19); w_to(MAZE20);
        u_to(MAZE19); d_to(CORR);
    new_place(R_MAZE20, "You are in a maze of twisty little passages.", NULL);
        n_to(MAZE21); e_to(MAZE19); s_to(MAZE20); w_to(MAZE22);
        u_to(MAZE20); d_to(MAZE20);
    new_place(R_MAZE21, "You are at a dead end.", NULL);
        s_to(MAZE20); object(ROD);
    new_place(R_MAZE22, "You are in a maze of twisty little passages.", NULL);
        n_to(MAZE23); e_to(MAZE20); s_to(MAZE22); w_to(MAZE22);
        u_to(MAZE22); d_to(MAZE22);
    new_place(R_MAZE23, "You are in a little maze of twisty passages.", NULL);
        n_to(MAZE23); e_to(MAZE18); s_to(MAZE22); w_to(MAZE23);
        u_to(TALL); d_to(MAZE23);
    new_place(R_TALL, "You're in tall wall canyon.",
        "A tall wall rises above you. You are nearly encircled by large" SOFT_NL
        "unclimbable canyons. The only exits lead south and east.");
        s_to(MAZE23); e_to(HALL2); object(BOTTLE);
    new_place(R_HALL2, "You're in long hall.",
        "You are in a long hall. Engraved on the rocks is a message." SOFT_NL
        "Exits lead east and west.");
        w_to(TALL); e_to(MEET);
    new_place(R_MEET, "You're at place where three paths meet.",
        "You have come to a place where three paths meet. A small path" SOFT_NL
        "leads north. Larger paths lead east and west.");
        w_to(HALL2); e_to(NARROW); n_to(CHASM);
    new_place(R_CHASM, "You're at chasm.",
        "A broad chasm separates you from a ledge to the north. A small" SOFT_NL
        "path leads south.");
        s_to(MEET); n_to(LEDGE);
    new_place(R_LEDGE, "You are on a narrow ledge.",
        "You are on a small narrow ledge. There is a cavern across the" SOFT_NL
        "chasm to the south. A walk leads down.");
        s_to(CHASM); d_to(TOOL); object(SPICES_IN_PLACE);
    new_place(R_NARROW, "You're in narrow part of cave.",
        "You are in an extremely narrow part of the cave. Exits lead east" SOFT_NL
        "and west.");
        w_to(MEET); e_to(FILTER);
    new_place(R_TEE, "You are at the \"T.\"",
        "You are at a \"T.\" A large path leads east. A road continues to" SOFT_NL
        "the north and to the south.");
        e_to(MINE); s_to(JUNC); n_to(WINDS);
    new_place(R_MINE, "You are at the mine entrance.",
        "You are at an entrance to an old abandoned mine. A small path" SOFT_NL
        "leads south. A large path leads west.");
        e_to(MINE33); s_to(TREE); w_to(TEE);
    new_place(R_TREE, "You are near an oak tree.",
        "You are standing by a large oak tree. Engraved on the bark is" SOFT_NL
        "a message. Paths lead north and south.");
        n_to(MINE); s_to(OIL); object(BALL);
    new_place(R_MINE33, "You are inside a mine. There are passages in every direction.", NULL);
        n_to(MINE38); e_to(MINE36); s_to(MINE34); w_to(MINE);
        u_to(MINE33); d_to(MINE33);
    new_place(R_MINE34, "You are inside a mine. Paths lead in all directions.", NULL);
        n_to(MINE33); e_to(MINE35); s_to(MINE34); w_to(MINE34);
        u_to(MINE34); d_to(MINE34);
    new_place(R_MINE35, "You are inside a mine. Paths lead in all directions.", NULL);
        n_to(MINE36); e_to(MINE35); s_to(MINE35); w_to(MINE34);
        u_to(MINE35); d_to(MINE35);
    new_place(R_MINE36, "You are inside a mine. Paths lead in all directions.", NULL);
        n_to(MINE37); e_to(MINE36); s_to(MINE35); w_to(MINE33);
        u_to(MINE36); d_to(MINE36);
    new_place(R_MINE37, "You are inside a mine. Paths lead in all directions.", NULL);
        n_to(MINE37); e_to(MINE37); s_to(MINE36); w_to(MINE38);
        u_to(MINE37); d_to(MINE37);
    new_place(R_MINE38, "You are inside a mine. Passages lead in all directions.", NULL);
        n_to(MINE38); e_to(MINE37); s_to(MINE33); w_to(MINE38);
        u_to(MINE38); d_to(MINE38);
        object(DIAMOND);
    new_place(R_WINDS, "You are in a place where cold winds blow.",
        "You're on a road. A cold wind blows in your face. It seems to" SOFT_NL
        "say, \"Luck doesn't run forever.\" The road continues to the" SOFT_NL
        "north and to the south.");
        s_to(TEE); n_to(ROAD2);
    new_place(R_ROAD2, "The road narrows here.",
        "The road grows smaller. To the east is a seldom-used gravel" SOFT_NL
        "path. The road continues northward and southward.");
        e_to(HILL); n_to(RIVER); s_to(WINDS);
    new_place(R_HILL, "You're on top of hill.",
        "You have climbed up a hill. The view is spectacular, as one" SOFT_NL
        "would expect. Sharp rocks protrude from the base of the hill." SOFT_NL
        "Beyond a river, a strange house can be seen. The silence here" SOFT_NL
        "is uncanny. Paths lead east and west.");
        w_to(ROAD2); e_to(ROCK);
    new_place(R_ROCK, "You are by a large rock.",
        "The gravel path ends here. A large rock marks this spot.");
        w_to(HILL);
        object(STAFF);
    new_place(R_RIVER, "You are at the river's southern edge.",
        "You are at the river's southern edge. There is a small sign on" SOFT_NL
        "the shore. A road goes south.");
        n_to(WELCOME); s_to(ROAD2);
    new_place(R_WELCOME, "You are at the river's northern edge.",
        "You are at the river's northern edge. A large sign says \"WELCOME" SOFT_NL
        "TO YOUR DEATH.\" Below the sign (scribbled in red) is a message." SOFT_NL
        "A road goes north.");
        s_to(RIVER); n_to(CROSSROADS);
    new_place(R_CROSSROADS, "You're at a crossroads.",
        "You are at a crossroads. A road goes north and south. Exits lead" SOFT_NL
        "east and west.");
        n_to(GARGOYLE); e_to(HOLEY); s_to(WELCOME); w_to(END);
    new_place(R_HOLEY, "You are on holey ground.",
        "The ground here is littered with large holes. There are paths" SOFT_NL
        "leading east and west.");
        w_to(CROSSROADS); e_to(STABLE);
    new_place(R_STABLE, "You're near the stable.",
        "You are standing near an old stable. A sign reads: \"Old odors" SOFT_NL
        "never die; they just accumulate.\" Paths lead north and west.");
        w_to(HOLEY); n_to(ORCHARD); object(HAY);
    new_place(R_END, "The road ends here.",
        "The road ends here. Small paths lead north and south. A road" SOFT_NL
        "goes to the east.");
        s_to(RIVERFRONT); n_to(OUTOUT); e_to(CROSSROADS);
    new_place(R_RIVERFRONT, "You are at the riverfront.",
        "You are at the riverfront. Upon the shore you see the bones" SOFT_NL
        "of unsuccessful river crossers. A path leads north.");
        n_to(END); object(RING);
    new_place(R_GARGOYLE, "You are by gargoyle statue.",
        "You are standing by a large gargoyle statue. It seems to be" SOFT_NL
        "staring at you. Inscribed at its base is the number 13. A road" SOFT_NL
        "goes south. Exits lead east and west. A house is north.");
        w_to(OUTOUT); e_to(GARDEN); s_to(CROSSROADS); n_to(HOUSE);
    new_place(R_OUTOUT, "You're outside outhouse.",
        "You are outside an outhouse. A path leads south. Other paths" SOFT_NL
        "lead east and west.");
        w_to(IDOL); e_to(GARGOYLE); s_to(END); n_to(OUTHOUSE);
    new_place(R_IDOL, "A large stone idol stands in front of you.",
        "A large stone idol stands in front of you. There is a small" SOFT_NL
        "charred pit in front of the idol. A path leads east.");
        e_to(OUTOUT); object(EYE_IN_PLACE);
    new_place(R_GARDEN, "You're in garden.",
        "You are in a garden. Half a dozen freshly picked carrots are" SOFT_NL
        "lying on the ground. An orchard is east. A path leads west.");
        w_to(GARGOYLE); e_to(ORCHARD);
    new_place(R_ORCHARD, "You're in an orchard.",
        "You are in an orchard. Surrounded by a flood of oranges is one" SOFT_NL
        "solitary apple tree. A path leads west. Another path goes south.");
        w_to(GARDEN); s_to(STABLE); object(SNAKE);
    new_place(R_HOUSE, "You are standing at the entrance to a house.",
        "You are at the entrance of an odd-looking house. In the mailbox" SOFT_NL
        "is a letter marked \"c/o Wizard of Oz.\" A walk leads south.");
        n_to(FOYER); s_to(GARGOYLE);
    new_place(R_FOYER, "You're near three doorways.",
        "You are inside the house. A large door is to the south. A large" SOFT_NL
        "hallway continues north. There is an arched door to the east." SOFT_NL
        "A small doorway is west.");
        w_to(DINING); e_to(LIVING); n_to(RECEPTION); s_to(HOUSE);
    new_place(R_DINING, "This is the dining room.",
        "This is the dining room. It looks like somebody is expecting" SOFT_NL
        "guests. There are seven plates on the table. A door is north." SOFT_NL
        "A doorway is east.");
        n_to(KITCHEN); e_to(FOYER); object(CHEESE);
    new_place(R_KITCHEN, "You're in the kitchen.",
        "This is the kitchen. A big pot of boiling water is over a wood" SOFT_NL
        "stove. A door is south.");
        s_to(DINING); object(INCENSE);
    new_place(R_LIVING, "You are in the living room.",
        "You are in the living room. A small statue with a round head is" SOFT_NL
        "sitting on a table. A large doorway is west.");
        w_to(FOYER); n_to(HIDDEN);
    new_place(R_HIDDEN, "You're in small hidden room.",
        "You are in a small hidden room. A rope hangs from the ceiling." SOFT_NL
        "There is an open panel to the south.");
        s_to(LIVING);
    new_place(R_BASEMENT, "You are in the basement.",
        "You are in the basement. A corridor leads north.");
        n_to(OUTSIDE); object(WIZARD);
    new_place(R_OUTSIDE, "You're outside vault.",
        "A large combination vault is standing in front of you. A sign on" SOFT_NL
        "top of the vault says, \"Deposit treasures inside the vault for" SOFT_NL
        "full credit.\" A corridor leads south. A door is east.");
        n_to(VAULT); s_to(BASEMENT); e_to(LIFT);
    new_place(R_RECEPTION, "This is the reception hall.",
        "You are in the reception hall. A large table is on one side of" SOFT_NL
        "the room. As you gaze upon the table you see someone stare back" SOFT_NL
        "at you. A can of Pledge sits on the far side of the room. Stairs" SOFT_NL
        "lead up. The hall extends south.");
        u_to(MIDDLE); s_to(FOYER); object(FIGURINE_IN_PLACE);
    new_place(R_MIDDLE, "You're on the middle floor.",
        "You are on the middle floor. Stairs lead up and down. Doorways" SOFT_NL
        "are to the east and west.");
        u_to(TOP); d_to(RECEPTION); w_to(BEDROOM); e_to(LIBRARY);
    new_place(R_LIBRARY, "You are in the library.",
        "You are in the library. A large heavy book lies open on top of" SOFT_NL
        "a desk by a window. A door is west.");
        w_to(MIDDLE); object(OPEN_WINDOW);
    new_place(R_BEDROOM, "You are in the bedroom.",
        "You are in the bedroom. A large walk-in closet is south. There" SOFT_NL
        "is a large dresser with a central drawer near the bed. A door" SOFT_NL
        "is to the east.");
        e_to(MIDDLE); s_to(CLOSET);
    new_place(R_CLOSET, "You are inside a walk-in closet.",
        "You are in a large walk-in closet with a door to the north.");
        n_to(BEDROOM); d_to(LIFT);
    new_place(R_TOP, "This is the top floor.",
        "You are on the upper floor. A door is west. Stairs lead down.");
        d_to(MIDDLE); w_to(ATTIC);
    new_place(R_ATTIC, "You are in the attic.",
        "You are in a musty attic. An old chest is sitting in one corner" SOFT_NL
        "of the room. A door is east.");
        e_to(TOP); object(MOUSE);
    new_place(R_OUTHOUSE, "You're inside the outhouse.",
        "You are inside the outhouse. A strange message is pasted on the" SOFT_NL
        "wall. A doorway is south.");
        s_to(OUTOUT);
    new_place(R_LIFT, "You are inside a small cubicle.",
        "You are inside a small cubicle. Inscribed above the door are" SOFT_NL
        "the letters I, F, and T. One other letter appears to be missing." SOFT_NL
        "The door is west.");
        w_to(OUTSIDE); u_to(CLOSET);
    new_place(R_VAULT, "You are inside vault.",
        "You are inside a vault.");
        s_to(OUTSIDE);
}

Location random_gnome_room()
{
    // Somewhere between R_START and R_MINE34, or maybe still R_LIMBO.
    return ran(35);
}

// Line 7400 in the original program
Location deal_with_gnomes(Location loc)
{
    if (!chased_by_gnome) {
        return loc;
    }

    if (there(GNOME, R_LIMBO)) {
        apport(GNOME, random_gnome_room());
    }

    // The gnome walks in your direction, but numerically instead of geographically.
    if (object_position[GNOME] > loc) object_position[GNOME] -= 1;
    if (object_position[GNOME] < loc) object_position[GNOME] += 1;

    if (there(GNOME, loc)) {
        putsn("There is a knife-wielding gnome in the same room as you!");
        int p = ran(100) + burden();
        if (p > 70) {
            putsn("He throws a small knife at you!");
            if (p > 90) {
                putsn("It gets you!");
                return death();
            } else {
                putsn("It misses you!");
            }
        }
    }
    return loc;
}

// Line 100 in the original program; "loc" is RN in the original program
Location deal_with_room(Location loc)
{
    if (R_FILTER <= loc && loc <= R_NARROW) {
        loc = deal_with_gnomes(loc);
    }
    turn_counter += 1;
    if (turn_counter > 400) {
        putsn("The heavens part at their seams as the gods prepare their next" SOFT_NL
              "meal. Almazar can no longer protect you, for even the mightiest" SOFT_NL
              "of beings cannot alter that which is preordained. Your life is" SOFT_NL
              "now over. May you rest in peace.");
        death_counter = 3;
        final_score_and_exit(false);
    }
    if (river_is_open) {
        river_counter += 1;
    } else {
        river_counter = 0;
    }
    if (lamp_is_on) {
        lamp_counter -= 1;
        if (lamp_counter <= 0) lamp_is_on = false;
    }
    if (lamp_is_on && (lamp_counter < 20) && toting(LAMP) && !lamp_has_dimmed) {
        putsn("Your lamp is getting dim.");
        lamp_has_dimmed = true;
    }
    if (darkness_counter > 0) {
        darkness_counter += 1;
        if (darkness_counter > 5) {
            putsn("A giant ogre grabs you and throws you into a pit.");
            return death();
        }
    }
    if (turn_counter > 360 && !better_hurry) {
        putsn("You had better hurry. Time is running out.");
        better_hurry = true;
    }
    if (loc == R_LIGHTED) {
        chased_by_gnome = 1;
    }
    if (there(WIZARD, loc)) {
        wizard_counter += 1;
        if (wizard_counter > 5) {
            return attempt_kick(WIZARD, loc);
        }
    }
    if (river_counter >= 8) {
        putsn("You incompetent fool! You forgot to close the river and as a" SOFT_NL
              "result a flood has overrun the countryside.");
        river_is_open = false;
        river_counter = 0;
        return death();
    }

    Verb iverb;
    Object iobj;
    listen(&iverb, &iobj);
    if ((iverb == NO_VERB) || (is_transitive(iverb) && iobj == NO_OBJECT)) {
        putsn("I don't understand.");
        return loc;
    }
    if (NORTH <= iverb && iverb <= DOWN) {
        if (iverb == NORTH && vault_status != STATUS_OPEN && loc == R_OUTSIDE) {
            putsn("You have to open the vault to get inside it.");
            return loc;
        } else if (iverb == NORTH && loc == R_LIVING && !there(OPEN_PANEL, loc)) {
            putsn("There is no way to go in that direction.");
            return loc;
        } else {
            Location nextloc = places[loc].neighbors[iverb - NORTH];
            if (nextloc == R_LIMBO) {
                putsn("There is no way to go in that direction.");
                return loc;
            } else {
                return attempt_move_to_room(loc, nextloc);
            }
        }
    } else {
        switch (iverb) {
            case LOOK: {
                places[loc].visited = false;
                loc = attempt_move_to_room(loc, loc);
                break;
            }
            case HINT: attempt_hint(loc); break;
            case INVENTORY: attempt_inventory(); break;
            case JUMP: loc = attempt_jump(loc); break;
            case SCORE: (void)attempt_score(false); break;
            case QUIT: attempt_quit(); break;
            case ENTER: loc = attempt_enter(loc); break;
            case LEAVE: loc = attempt_leave(loc); break;
            case GET: loc = attempt_get(iobj, loc); break;
            case DROP: attempt_drop(iobj, loc); break;
            case WAVE: attempt_wave(iobj, loc); break;
            case SHOW: attempt_show(iobj, loc); break;
            case READ: loc = attempt_read(iobj, loc); break;
            case FILL: attempt_fill(iobj, loc); break;
            case LIGHT: attempt_light(iobj, loc); break;
            case KILL: loc = attempt_kill(iobj, loc); break;
            case KICK: loc = attempt_kick(iobj, loc); break;
            case BREAK: attempt_break(iobj, loc); break;
            case TOSS: attempt_toss(iobj, loc); break;
            case EAT: loc = attempt_eat(iobj, loc); break;
            case DRINK: attempt_drink(iobj, loc); break;
            case FEED: attempt_feed(iobj, loc); break;
            case TURN: attempt_turn(iobj, loc); break;
            case UNLOCK: attempt_unlock(iobj, loc); break;
            case CROSS: loc = attempt_cross(iobj, loc); break;
            case SWIM: loc = attempt_swim(iobj, loc); break;
            case OPEN: attempt_open(iobj, loc); break;
            case CLOSE: attempt_close(iobj, loc); break;
            case OFF: attempt_off(iobj); break;
            case PULL: loc = attempt_pull(iobj, loc); break;
            case EMPTY: attempt_empty(iobj); break;
        }
    }
    return loc;
}

void title_screen()
{
    puts("Welcome to Adventure ]I[!\n");
    printf("Do you need instructions? ");
    fflush(stdout);
    fgets(buffer, sizeof buffer, stdin);
    if (buffer[0] == 'N' || buffer[0] == 'n') {
        // do nothing
    } else {
        attempt_help();
    }
}

void attempt_help()
{
    puts("");
    puts("Somewhere nearby is the land of Erehwon, a mysterious place");
    puts("Filled with fantasy and magic.  It is your task to explore this");
    puts("unknown region, requiring your physical skill and cunning to");
    puts("be at their peak.  I will be your eyes and ears in this exped-");
    puts("ition, describing your surroundings and any points of interest");
    puts("that you happen to run across.  Direct me with commands of one");
    puts("or two words.  To move from one place to another, try words like");
    puts("NORTH, WEST, CLIMB, DOWN.  To get a list of your current possessions");
    puts("use the word INVENTORY.  All words, objects, etc. may be abbriev-");
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
    puts("Multiple commands on a line are separated thus:  N, S, E, W, EAT TREE.");
    puts("");
    puts("More help will be available here at a later time as the game");
    puts("develops further.");
}

// Line 1830 in GumaReconstruction/advent.txt
void update_timers()
{
    t += 1;
    if (reactor_timer != 0) {
        if (t == 200) {
            puts("An EPA official runs by muttering, \"Tsk, tsk.  Another TMI.\"");
        } else if (t == 215) {
            puts("A calm, pleasent voice reports, \"Meltdown...4 minutes.  Warning only.\"");
        } else if (t >= 216) {
            reactor_timer = (log(30) / log(t-214)) - 0.95
            if (reactor_timer < 0.1) {
                puts("Booooom!!!"); // and two bell characters
                exit();
            }
            printf("There are now %d minutes until meltdown.", int(reactor_timer));
        }
    }
    lamp_power -= lamp_on;
    if (lamp_power == 25) {
        puts("The lamp is getting dim.");
        lamp_power = 25.5;
    }
    if (lamp_power == 0.5) {
        puts("The lamp's batteries are dead.");
        lamp_power = 0;
    }
    if (quicksand_timer != 0) {
        quicksand_timer -= 1;
    }
    if (loc == 44 && quicksand_timer != 0) {
        puts("You are sinking in quicksand.");
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

// Line 2540 in adven2.txt
void attempt_bug() {
    puts("Bug reporting isn't supported in this C port.");
    puts("In the original, you would have been prompted to leave a two-line message" SOFT_NL "describing the problem.");
}

void parse_input() {
    static char input[130];
    while (input[0] == '\0') {
        printf("> "); fflush(stdout);
        fgets(input, sizeof input, stdin);
        char *p = strchr(input, '\0');
        while (p > input && isspace(p[-1])) --p;
        *p = '\0';
    }
    // Append a period, peel off the first period-delimited command into $C1,
    // and update $C (`input`). Lines 2140-2270.
    strcat(input, ".");
    char *p = input;
    char *q = active_command;
    while (*p == ' ') ++p;
    while (*p != '.') *q++ = toupper(*p++);
    while (q > active_command && q[-1] == ' ') --q;
    *q = '\0';
    q = input;
    while (*p != '\0') *q++ = *p++;
    *q = '\0';

    active_verb = NULL;
#define CMDIS(s) (memcmp(active_command, s, strlen(s)) == 0 ? ((active_verb = s), true) : false)

    if (strlen(active_command) == 1) {
        const char *r = strchr("NSEWUDIO", active_command[0]);
        if (r == NULL) {
            puts("I don't understand.");
        } else {
            //4190
        }
    } else if (CMDIS("TAKE") || CMDIS("GET") || CMDIS("PICK UP")) {
        attempt_take();
    } else if (CMDIS("DROP")) {
        // 3060
    } else if (CMDIS("GET OFF")) {
        // 3480
    } else if (CMDIS("EAT")) {
        // 3510
    } else if (CMDIS("LIGHT") || CMDIS("TURN ON")) {
        // 3540
    } else if (CMDIS("LONG")) {
        // 3670
    } else if (CMDIS("ON")) {
        // 3790
    } else if (CMDIS("OFF")) {
        // 3810
    } else if (CMDIS("READ")) {
        // 3790
    } else if (CMDIS("KISS")) {
        // 3810
    } else if (CMDIS("CLIMB")) {
        // 4060
    } else if (CMDIS("NORTH") || CMDIS("SOUTH") || CMDIS("EAST") || CMDIS("WEST") || CMDIS("UP") || CMDIS("DOWN") || CMDIS("OUT")) {
        // 4190
    } else if (CMDIS("IN")) {
        // 4730
    } else if (CMDIS("KILL")) {
        // 7291
    } else if (CMDIS("PUT")) {
        // 5880
    } else if (CMDIS("OPEN")) {
        // 4870
    } else if (CMDIS("CLOSE")) {
        // 5130
    } else if (CMDIS("LOCK")) {
        // 5330
    } else if (CMDIS("UNLOC")) {
        // 5530
    } else if (CMDIS("ARGYL")) {
        // 5690
    } else if (CMDIS("STAB")) {
        // 5760
    } else if (CMDIS("BREAK")) {
        // 5910
    } else if (CMDIS("ENTER")) {
        // 6060
    } else if (CMDIS("EXIT")) {
        // 6090
    } else if (CMDIS("BRIEF")) {
        // 6120
    } else if (CMDIS("LOOK")) {
        // 6160
    } else if (CMDIS("PUSH") || CMDIS("PULL")) {
        // 6180
    } else if (CMDIS("SUPER")) {
        // 6800
    } else if (CMDIS("ALLEZ")) {
        attempt_allez();
    } else if (CMDIS("PLAY")) {
        // 6920
    } else if (CMDIS("JUMP") || CMDIS("HOP")) {
        // 7070
    } else if (CMDIS("WAVE")) {
        // 7130
    } else if (CMDIS("CHOP")) {
        // 5760
    } else if (CMDIS("SAY")) {
        // 7300
    } else if (CMDIS("GO")) {
        // 7330
    } else if (CMDIS("WALK")) {
        // 
    } else if (CMDIS("RUN")) {
        // 
    } else if (CMDIS("CRAWL")) {
        // 
    } else if (CMDIS("LEAVE")) {
        // 
    } else if (CMDIS("QUIT") || CMDIS("SCORE") || CMDIS("STOP")) {
        // 
    } else if (CMDIS("HELP")) {
        attempt_help();
    } else if (CMDIS("BUG")) {
        attempt_bug();
#ifdef SAVE_AND_RESTORE
    } else if (CMDIS("SAVE")) {
        switch (attempt_save()) {
            case 0: putsn("Save failed!"); break;
            case 1: putsn("Saved."); break;
            case 2: putsn("Restored."); break;
        }
    } else if (CMDIS("LOAD") || CMDIS("RESTORE")) {
        /* On the fizmo interpreter, @restore yields 2
         * when the save file doesn't exist, or when it
         * has the wrong serial number for thisgame.
         * I don't know what return value 0 would mean. */
        attempt_restore();
        putsn("Restore failed!");
#endif
    } else if (CMDIS("FIND")) {
        // 3010
    } else if (CMDIS("HAVE SEX") || CMDIS("SCREW")) {
        // 3220
    } else if (CMDIS("FEED")) {
        // 3490
    } else if (CMDIS("SPRAY")) {
        // 3750
    } else if (CMDIS("HIT") || CMDIS("KICK") || CMDIS("PUNCH") || CMDIS("BITE") || CMDIS("POKE")) {
        // 3860
    } else if (CMDIS("USE")) {
        // 3880
    } else if (CMDIS("SING")) {
        // 3980
    } else if (CMDIS("SWIM")) {
        // 4020
    } else if (CMDIS("SPEAK") || CMDIS("CALL")) {
        // 2030
    } else if (CMDIS("TASTE")) {
        // 4120
    } else if (CMDIS("CARRY")) {
        // 4140
    } else if (CMDIS("KEEP")) {
        // 4170
    } else if (CMDIS("HIDE")) {
        // 4190
    } else if (CMDIS("DIG")) {
        // 4220
    } else if (CMDIS("LISTE")) {
        // 4480
    } else if (CMDIS("ATTAC")) {
        // 4650
    } else if (CMDIS("BOARD")) {
        // 5380
    } else if (CMDIS("LAUNC")) {
        // 5570
    } else if (CMDIS("THROW")) {
        // 5720
    } else if (CMDIS("BURN")) {
        // 5770
    } else if (CMDIS("TOSS")) {
        // 5820
    } else if (CMDIS("TURN OFF")) {
        // 6080
    } else if (CMDIS("TURN")) {
        // 6110
    } else if (CMDIS("COMB")) {
        // 6270
    } else if (CMDIS("DISEM")) {
        // 1090
    } else if (CMDIS("PRAY")) {
        // 6480
    } else if (CMDIS("INSERT")) {
        // 1640
    } else {
        puts("I don't understand.");
    }
}

int main()
{
#ifdef Z_MACHINE
    puts("\n\n\n\n\n\n\n\n");
#endif /* Z_MACHINE */

    char input[130]; // C$ in advent.txt

    title_screen();
    while (true) {
        update_timers();
        parse_input(input, sizeof input);
    }
}

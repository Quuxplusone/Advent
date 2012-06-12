
#include <stdbool.h>
#include <stdio.h>
#include "advdat.h"

bool pct(int percent);
bool yes(const char *prompt);
void apport(ObjectWord t, Location loc);
void say_foof(void);
void ok(void);
void hah(void);
void nothing_happens(void);
void already_open(const char *s);
void finis(void);
extern Location safe_exit;
bool keywordv(VerbWord meaning);
bool keywordo(ObjectWord meaning);
bool keywordp(Location meaning);
bool portable(ObjectWord o);
bool is_treasure(ObjectWord o);

extern VerbWord verbosity_mode;
extern int closure;
extern int death_count;
extern bool gave_up;
extern bool panicked;
extern bool nomagic;
extern int lamplife;
extern int holding_count;
extern int cylinder_escape_count;
extern bool have_stolen_back_eggs;

void cantpasslock(void) { puts("You can't go through a locked steel grate!"); }

static int you_didnt_move(void)
{
    puts("You have crawled around in some little holes and wound up back in the\n"
         "main passage.");
    return STAY_STILL;
}

static int oof(Location newloc)
{
    puts("Wheeeeeeeeeeeeeeeee..........\n\n"
         "                                           >oof!<\n\n");
    return newloc;
}

static int splatter(Location newloc)
{
    switch (death_count) {
        case 0:
            puts("Aaaaaaaaaiiiiiiiiiiieeeeeeeeee........\n"
                 "                                                     >splat!<");
            break;
        case 1:
            puts("Aaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhh........\n"
                 "                                                    >squish!<");
            break;
        case 2:
            puts("Gaaaaaaaaaaaaaaaaaaaaaaaaaahhhhhh...........\n"
                 "                                                    >crunch!<");
            break;
        case 3:
            puts("Haaaaaaaaaaaaaalllllllllllllllllpppppp...........\n"
                 "                                                   >smash!<");
            break;
    }
    return you_are_dead_at(newloc);
}

static int plunge(void)
{
    if (objs[LAMP].prop) {
        lamplife = 0;  /* "First your lamp goes out." */
        if (toting(LAMP)) {
            puts("You have jumped into a bottomless pit.  You continue to fall for\n"
                 "a very long time.  First, your lamp runs out of power and goes\n"
                 "dead. Later, you die of hunger and thirst.");
        } else {
            puts("You have jumped into a bottomless pit.  Eventually, you die of thirst.");
        }
    } else {
        puts("You have jumped into a bottomless pit.  Eventually, you die of thirst.");
    }
    return you_are_dead_at(R_YLEM);
}

bool used_movement_obj(ObjectWord where_to)
{
    switch (word1.type) {
        case WordType_Object:
            return ((ObjectWord)word1.meaning == where_to);
        case WordType_Verb:
            if (word1.meaning == WALK || word1.meaning == SAY)
                return (word2.type == WordType_Object && (ObjectWord)word2.meaning == where_to);
            return false;
    }
    return false;
}

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

bool used_movement_verb2(VerbWord a, VerbWord b)
{
    return used_movement_verb(a) || used_movement_verb(b);
}

bool used_movement_verb3(VerbWord a, VerbWord b, VerbWord c)
{
    return used_movement_verb(a) || used_movement_verb(b) || used_movement_verb(c);
}

int at_limbo(void)
{
    if (used_movement_verb(OUT)) return R_HOUSE;
    return 0;  /* command hasn't been processed yet */
}

int at_road(void)
{
    if (used_movement_placeword(R_ROAD)) return R_HILL;
    if (used_movement_verb2(WEST, UP)) return R_HILL;
    if (used_movement_verb2(IN, EAST)) return R_HOUSE;
    if (used_movement_placeword(R_HOUSE)) return R_HOUSE;
    if (used_movement_verb3(DOWNSTREAM, GULLY, STREAM)) return R_VALLEY;
    if (used_movement_verb2(SOUTH, DOWN)) return R_VALLEY;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb2(NORTH, EAST)) return R_FOREST;
    if (used_movement_placeword(R_DEPRESSION)) return R_DEPRESSION;
    return 0;  /* command hasn't been processed yet */
}

int at_hill(void)
{
    if (used_movement_placeword(R_ROAD)) return R_ROAD;
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb3(FORWARD, EAST, NORTH)) return R_ROAD;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb2(NORTH, SOUTH)) return R_FOREST;
    return 0;  /* command hasn't been processed yet */
}

int at_house(void)
{
    if (used_movement_placeword(R_ROAD)) return R_ROAD;
    if (used_movement_verb3(OUT, OUTDOORS, WEST)) return R_ROAD;
    if (closure < 2) {
        if (used_movement_verb(XYZZY)) {
            say_foof();
            return R_DEBRIS;
        }
        if (used_movement_verb(PLUGH)) {
            say_foof();
            return R_Y2;
        }
    }
    if (used_movement_verb2(STREAM, DOWNSTREAM)) {
        puts("The stream flows out through a pair of 1 foot diameter sewer pipes.\n"
             "It would be advisable to leave via the exit.");
        return R_HOUSE;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_valley(void)
{
    if (used_movement_verb2(UPSTREAM, NORTH)) return R_ROAD;
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb3(EAST, WEST, UP)) return R_FOREST;
    if (used_movement_placeword(R_DEPRESSION)) return R_DEPRESSION;
    if (used_movement_verb3(SOUTH, DOWN, DOWNSTREAM))
        return (closure < 2) ? R_SLIT : R_FAKE_SLIT;
    return 0;  /* command hasn't been processed yet */
}

int at_forest(void)
{
    if (used_movement_placeword(R_VALLEY)) return R_VALLEY;
    if (used_movement_verb(EAST)) return R_VALLEY;
    if (used_movement_verb(DOWN)) return R_VALLEY;
    if (used_movement_verb(WEST)) return R_FOREST;
    if (used_movement_verb(SOUTH)) return R_FOREST;
    if (used_movement_placeword(R_FOREST) || used_movement_verb2(FORWARD, NORTH))
        return pct(50) ? R_FOREST : R_FOREST2;
    return 0;  /* command hasn't been processed yet */
}

int at_forest2(void)
{
    if (used_movement_placeword(R_ROAD)) return R_ROAD;
    if (used_movement_verb(NORTH)) return R_ROAD;
    if (used_movement_placeword(R_VALLEY)) return R_VALLEY;
    if (used_movement_verb3(EAST, WEST, DOWN)) return R_VALLEY;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb(SOUTH)) return R_FOREST;
    return 0;  /* command hasn't been processed yet */
}

int at_slit(void)
{
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb(UPSTREAM)) return R_VALLEY;
    if (used_movement_verb(NORTH)) return R_VALLEY;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb2(EAST, WEST)) return R_FOREST;
    if (used_movement_verb(DOWNSTREAM)) return R_DEPRESSION;
    if (used_movement_verb3(ROCK, BED, SOUTH)) return R_DEPRESSION;
    if (used_movement_placeword(R_SLIT) || used_movement_verb3(STREAM, DOWN, IN)) {
        puts("You don't fit through a two-inch slit!");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_fake_slit(void)
{
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb(UPSTREAM)) return R_VALLEY;
    if (used_movement_verb(NORTH)) return R_VALLEY;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb(EAST)) return R_FOREST;
    if (used_movement_verb(WEST)) return R_FOREST;
    if (used_movement_verb(DOWNSTREAM)) return R_DEPRESSION;
    if (used_movement_verb(ROCK)) return R_DEPRESSION;
    if (used_movement_verb(BED)) return R_DEPRESSION;
    if (used_movement_verb(SOUTH)) return R_DEPRESSION;
    if (used_movement_placeword(R_SLIT) || used_movement_verb3(STREAM, DOWN, IN)) {
        closure = 5;  /* victory! */
        puts("You plunge into the stream and are carried down into total blackness.\n\n"
        "Deeper\n"
        "        and\n"
        "            deeper\n"
        "                   you\n"
        "                       go,\n"
        "                           down\n"
        "                                into\n"
        "                                 the\n"
        "                              very\n"
        "                           bowels\n"
        "                       of\n"
        "                  the\n"
        "            earth,\n"
        "       until\n"
        "    your\n"
        "         lungs\n"
        "               are\n"
        "                  aching\n"
        "                        with\n"
        "                             the\n"
        "                                 need\n"
        "                                       for\n"
        "                                          fresh\n"
        "                                             air.\n"
        "                                                Suddenly,\n"
        "                                               with\n"
        "                                             a\n"
        "                                          violent\n"
        "                                                >splash!!<\n\n"
         "you find yourself sitting on the edge of a pool of water in\n"
         "a vast chamber lit by dozens of flaring torches.\n\n"
         "The floor is covered with thick layers of precious Persian rugs!\n\n"
         "Rare coins, bars of silver, and lumps of gold and platinum are\n"
         "strewn carelessly about!\n\n"
         "There are diamonds, rubies, sapphires, emeralds, opals, pearls, and\n"
         "fabulous sculptures and ornaments carved out of jade and imperishable\n"
         "crystal resting on display shelves, along with rare Ming vases and\n"
         "ancient Indian turquoise beads!\n\n"
         "A flotilla of ruby-encrusted toy boats is floating in the pool of\n"
         "water beside you!\n\n"
         /* Platt capitalized "Iridium" here; I'm lowercasing it. */
         "A network of golden chains supports a fantastic iridium crown!\n\n"
         "There is a display case on the wall filled with a fantastic selection\n"
         "of magical swords, which are singing \"Hail to the Chief\" in perfect\n"
         "pitch and rhythm!\n\n"
         "There are a dozen friendly little dwarves in the room, displaying\n"
         "their talents by deftly juggling hundreds of golden eggs!\n\n"
         "A large troll, a gigantic ogre, and a bearded pirate are tossing\n"
         "knives, axes, and clubs back and forth in a friendly demonstration\n"
         "of martial skill!\n\n"
         "A horde of cheerful little gooseberry goblins are performing\n"
         "talented acrobatics to an appreciative audience composed of a dragon,\n"
         "a large green snake, a cute little bird (which is sitting, unmolested,\n"
         "on the snake's head), a peaceful basilisk, and a large Arabian Djinn.\n\n"
         "Everyone turns and sees you, and lets out a heart-warming cheer\n"
         "of welcome!\n");
        gave_up = false;
        finis();  /* end the game in triumph */
    }
    return 0;  /* command hasn't been processed yet */
}

int at_depression(void)
{
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb2(EAST, SOUTH)) return R_FOREST2;
    if (used_movement_verb(WEST)) return R_FOREST;
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb3(UPSTREAM, GULLY, NORTH) || used_movement_placeword(R_SLIT)) {
        return (closure < 2) ? R_SLIT : R_FAKE_SLIT;
    }
    if (used_movement_verb2(IN, DOWN)) {
        if (objs[GRATE].prop == 1) {
            return R_INSIDE;
        } else {
            cantpasslock();
            return STAY_STILL;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_inside(void)
{
    if (used_movement_verb3(UP, OUT, SURFACE)) {
        if (objs[GRATE].prop == 1) {
            return R_DEPRESSION;
        } else {
            cantpasslock();
            return STAY_STILL;
        }
    }
    if (used_movement_verb(CRAWL)) return R_COBBLES;
    if (used_movement_placeword(R_COBBLES)) return R_COBBLES;
    if (used_movement_verb2(IN, WEST)) return R_COBBLES;
    if (used_movement_placeword(R_SPIT)) return R_SPIT;
    if (used_movement_placeword(R_DEBRIS)) return R_DEBRIS;
    return 0;  /* command hasn't been processed yet */
}

int at_cobbles(void)
{
    if (used_movement_verb(OUT)) return R_INSIDE;
    if (used_movement_verb(SURFACE)) return R_INSIDE;
    if (used_movement_verb(NOWHERE)) return R_INSIDE;  /* what the heck? */
    if (used_movement_verb(ENTRANCE)) return R_INSIDE;
    if (used_movement_verb(EAST)) return R_INSIDE;
    if (used_movement_verb(IN)) return R_DEBRIS;
    if (used_movement_placeword(R_DARK)) return R_DEBRIS;
    if (used_movement_verb(WEST)) return R_DEBRIS;
    if (used_movement_placeword(R_DEBRIS)) return R_DEBRIS;
    if (used_movement_placeword(R_SPIT)) return R_SPIT;
    return 0;  /* command hasn't been processed yet */
}

int at_debris(void)
{
    if (used_movement_verb(ENTRANCE)) return R_INSIDE;
    if (used_movement_verb(CRAWL)) return R_COBBLES;
    if (used_movement_placeword(R_COBBLES)) return R_COBBLES;
    if (used_movement_verb(PASSAGE)) return R_COBBLES;
    if (used_movement_placeword(R_LOW)) return R_COBBLES;
    if (used_movement_verb(EAST)) return R_COBBLES;
    if (used_movement_placeword(R_AWK)) return R_AWK;
    if (used_movement_verb(IN)) return R_AWK;
    if (used_movement_verb(UP)) return R_AWK;
    if (used_movement_verb(WEST)) return R_AWK;
    if (used_movement_placeword(R_SPIT)) return R_SPIT;
    if (objs[GRATE].prop) {
        if (used_movement_placeword(R_DEPRESSION)) return R_DEPRESSION;
    } else {
        if (used_movement_placeword(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    if (keywordv(XYZZY)) {
        if (closure >= 2 || nomagic) {
            nothing_happens();
            panicked = true;
            return STAY_STILL;
        } else if (used_movement_verb(XYZZY)) {
            say_foof();
            return R_HOUSE;
        } else {
            /* This seems like a bug in Platt's code. */
            return STAY_STILL;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_awk(void)
{
    if (used_movement_verb(ENTRANCE)) return R_INSIDE;
    if (used_movement_verb(DOWN)) return R_DEBRIS;
    if (used_movement_verb(EAST)) return R_DEBRIS;
    if (used_movement_placeword(R_DEBRIS)) return R_DEBRIS;
    if (used_movement_verb(IN)) return R_BIRD;
    if (used_movement_verb(UP)) return R_BIRD;
    if (used_movement_verb(WEST)) return R_BIRD;
    if (used_movement_placeword(R_SPIT)) return R_SPIT;
    if (objs[GRATE].prop) {
        if (used_movement_placeword(R_DEPRESSION))
            return R_DEPRESSION;
    } else {
        if (used_movement_placeword(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_bird(void)
{
    if (used_movement_verb(ENTRANCE)) return R_INSIDE;
    if (used_movement_placeword(R_DEBRIS)) return R_DEBRIS;
    if (used_movement_placeword(R_AWK)) return R_AWK;
    if (used_movement_verb(EAST)) return R_AWK;
    if (used_movement_verb(PASSAGE)) return R_SPIT;
    if (used_movement_placeword(R_SPIT)) return R_SPIT;
    if (used_movement_verb(WEST)) return R_SPIT;
    if (objs[GRATE].prop) {
        if (used_movement_placeword(R_DEPRESSION))
            return R_DEPRESSION;
    } else {
        if (used_movement_placeword(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_spit(void)
{
    if (used_movement_verb(ENTRANCE)) return R_INSIDE;
    if (used_movement_placeword(R_DEBRIS)) return R_DEBRIS;
    if (used_movement_verb2(PASSAGE, EAST)) return R_BIRD;
    if (objs[GRATE].prop) {
        if (used_movement_placeword(R_DEPRESSION))
            return R_DEPRESSION;
    } else {
        if (used_movement_placeword(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    if (used_movement_verb(WEST) || used_movement_verb(CRACK)) {
        puts("The crack is far too small for you to enter.");
        return STAY_STILL;
    }
    /* This is clearly a bug in Platt's code. Going DOWN (the most natural
     * verb) will work fine, but going STEPS or PIT will kill you.
     * TODO: perhaps fix this? */
    if (used_movement_verb(DOWN)) return R_EMIST;
    if (used_movement_obj(STEPS) || used_movement_placeword(R_SPIT)) {
        if (toting(GOLD)) {
            puts("You are at the bottom of the pit with a broken neck.");
            return you_are_dead_at(R_EMIST);
        }
        return R_EMIST;
    }
    /* Another funny sort-of-bug: JUMP kills you, as does JUMP SOUTH,
     * but JUMP WEST is caught by the case above, and JUMP DOWN
     * will get you safely down the stairs after all. */
    if (keywordv(JUMP)) {
        return splatter(R_EMIST);
    }
    return 0;  /* command hasn't been processed yet */
}

int at_emist(void)
{
    if (used_movement_verb2(LEFT, SOUTH)) return R_NUGGET;
    if (used_movement_verb(EAST)) return R_SANDSTONE;
    if (used_movement_verb(FORWARD)) return R_EFISS;
    if (used_movement_verb(HALL)) return R_EFISS;
    if (used_movement_verb(WEST)) return R_EFISS;
    if (used_movement_verb(STAIRS)) return R_HMK;
    if (used_movement_verb(DOWN)) return R_HMK;
    if (used_movement_verb(NORTH)) return R_HMK;
    if (used_movement_placeword(R_Y2)) return R_JUMBLE;
    if (used_movement_verb(UP) || used_movement_obj(STEPS) ||
        used_movement_placeword(R_SPIT) || used_movement_verb(DOME) ||
        used_movement_verb(PASSAGE) || used_movement_verb(IN)) {
        if (toting(GOLD)) {
            puts("The dome is unclimbable.");
            return STAY_STILL;
        } else {
            return R_SPIT;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int cross_fissure(VerbWord direction, Location destination)
{
    if (used_movement_verb3(FORWARD, CROSS, direction)) {
        if (objs[FISSURE].prop) return destination;
        puts("There is no way across the fissure.");
        return STAY_STILL;
    }
    if (used_movement_verb(JUMP)) {
        if (objs[FISSURE].prop) {
            puts("I respectfully suggest you go across the bridge instead of jumping.");
            return STAY_STILL;
        }
        /* This connection is original to Platt's version. */
        return splatter(R_FALLS);
    }
    return 0;  /* command hasn't been processed yet */
}

int at_efiss(void)
{
    if (used_movement_verb(HALL)) return R_EMIST;
    if (used_movement_verb(EAST)) return R_EMIST;
    return cross_fissure(WEST, R_WFISS);
}

int at_nugget(void)
{
    if (used_movement_verb3(HALL, OUT, NORTH)) return R_EMIST;
    return 0;  /* command hasn't been processed yet */
}

int at_hmk(void)
{
    if (used_movement_verb3(STAIRS, UP, EAST)) return R_EMIST;
    if (there(SNAKE, R_HMK)) {
        if (used_movement_verb3(NORTH, LEFT, SOUTH) ||
            used_movement_verb3(RIGHT, WEST, FORWARD) ||
            used_movement_verb3(SW, NW, SE) ||
            used_movement_verb3(NE, DOWN, SECRET)) {
            puts("You can't get past the snake.");
            return STAY_STILL;
        }
    } else {
        /* no snake */
        if (used_movement_verb2(NORTH, LEFT)) return R_NS;
        if (used_movement_verb2(SOUTH, RIGHT)) return R_SOUTH;
        if (used_movement_verb2(WEST, FORWARD)) return R_WEST;
        if (used_movement_verb(DOWN)) return R_VAULT;
        if (used_movement_verb(NE)) return R_MORION;
        if (used_movement_verb(NW)) return R_CORRID_3;
        if (used_movement_verb(SE)) return R_CORRID_1;
        if (used_movement_verb(SECRET)) return R_SECRETEW_TITE;
        if (used_movement_verb(SW)) {
            if (pct(35)) {
                return R_SECRETEW_TITE;
            } else {
                return you_didnt_move();
            }
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_w2pit(void)
{
    if (used_movement_verb2(EAST, CROSS)) return R_E2PIT;
    if (used_movement_verb(WEST)) return R_SLAB;
    if (used_movement_placeword(R_SLAB)) return R_SLAB;
    if (used_movement_verb(DOWN)) return R_WPIT;
    if (used_movement_placeword(R_SPIT)) return R_WPIT;
    if (keywordv(HOLE)) {
        puts("It is too far up for you to reach.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_epit(void)
{
    if (used_movement_verb2(UP, OUT)) return R_E2PIT;
    return 0;  /* command hasn't been processed yet */
}

int at_wpit(void)
{
    if (used_movement_verb2(UP, OUT)) return R_W2PIT;
    if (keywordv(CLIMB)) {
        if (objs[PLANT].prop == 0) {
            puts("There's nothing to climb here.  Say \"UP\" or \"OUT\" to leave the pit.");
            return STAY_STILL;
        } else if (objs[PLANT].prop == 1) {
            puts("You have climbed up the plant and out of the pit.");
            return R_W2PIT;
        } else {
            puts("You scurry up the plant and into the hole in the wall.");
            return R_NARROW;
        }
    }
    if (keywordv(GET) && keywordo(PLANT)) {
        puts("The plant has exceptionally deep roots and cannot be pulled free.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_wfiss(void)
{
    if (used_movement_verb(WEST)) return R_WMIST;
    /* The only reason I use this awkward construction is to preserve
     * the behavior of the command "NORTH EAST". */
    int result = cross_fissure(EAST, R_EFISS);
    if (result != 0) return result;
    if (keywordv(NORTH)) {
        puts("You have crawled around in a little passage north of and parallel\n"
             "to the Hall of Mists.");
        return R_WMIST;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_ns(void)
{
    if (used_movement_verb3(HALL, OUT, SOUTH)) return R_HMK;
    if (used_movement_verb(NORTH)) return R_Y2;
    if (used_movement_placeword(R_Y2)) return R_Y2;
    if (used_movement_verb2(DOWN, HOLE)) return R_DIRTY;
    return 0;  /* command hasn't been processed yet */
}

int at_south(void)
{
    if (used_movement_verb3(HALL, OUT, NORTH)) return R_HMK;
    return 0;  /* command hasn't been processed yet */
}

int at_west(void)
{
    if (used_movement_verb3(HALL, OUT, EAST)) return R_HMK;
    if (used_movement_verb2(WEST, UP)) return R_CROSS;
    return 0;  /* command hasn't been processed yet */
}

int at_y2(void)
{
    if (used_movement_verb(SOUTH)) return R_NS;
    if (used_movement_verb(EAST)) return R_JUMBLE;
    if (used_movement_placeword(R_JUMBLE)) return R_JUMBLE;
    if (used_movement_verb(WEST)) return R_WINDOE;
    if (used_movement_verb(PLUGH) || used_movement_placeword(R_PLOVER)) {
        if (word2.type != WordType_None && !keywordv(SAY))
            return 0;  /* DROP PLUGH isn't handled here */
        if (closure >= 2 || nomagic) {
            nothing_happens();
            panicked = true;
            return STAY_STILL;
        } else {
            say_foof();
            return used_movement_verb(PLUGH) ? R_HOUSE : R_PLOVER;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_jumble(void)
{
    if (used_movement_verb(DOWN)) return R_Y2;
    if (used_movement_placeword(R_Y2)) return R_Y2;
    if (used_movement_verb(UP)) return R_EMIST;
    return 0;  /* command hasn't been processed yet */
}

int at_dirty(void)
{
    if (used_movement_verb2(EAST, CRAWL)) return R_CLEAN;
    if (used_movement_verb2(UP, HOLE)) return R_NS;
    if (used_movement_verb(WEST)) return R_DUSTY;
    if (used_movement_placeword(R_BEDQUILT)) return R_BEDQUILT;
    return 0;  /* command hasn't been processed yet */
}

int at_windoe(void)
{
    if (used_movement_verb(EAST)) return R_Y2;
    if (used_movement_placeword(R_Y2)) return R_Y2;
    if (used_movement_verb(JUMP)) {
        puts("You are at the bottom of the pit with a broken neck.");
        return you_are_dead_at(R_MIRROR);
    }
    return 0;  /* command hasn't been processed yet */
}

int at_clean(void)
{
    if (used_movement_verb2(WEST, CRAWL)) return R_DIRTY;
    if (used_movement_verb(DOWN)) return R_WET;
    if (used_movement_placeword(R_SPIT)) return R_WET;
    if (used_movement_verb(CLIMB)) return R_WET;
    return 0;  /* command hasn't been processed yet */
}

int at_wet(void)
{
    if (used_movement_verb3(CLIMB, UP, OUT)) return R_CLEAN;
    if (used_movement_verb3(STREAM, DOWN, UPSTREAM) ||
            used_movement_verb(DOWNSTREAM) || used_movement_placeword(R_SLIT)) {
        puts("You don't fit through a two-inch slit!");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_dusty(void)
{
    if (used_movement_verb2(EAST, PASSAGE)) return R_DIRTY;
    if (used_movement_verb2(DOWN, HOLE)) return R_COMPLEX;
    if (used_movement_placeword(R_BEDQUILT)) return R_BEDQUILT;
    return 0;  /* command hasn't been processed yet */
}

int at_wmist(void)
{
    if (used_movement_verb(SOUTH)) return R_MAZEA42;
    if (used_movement_verb(UP)) return R_MAZEA42;
    if (used_movement_verb(PASSAGE)) return R_MAZEA42;
    if (used_movement_verb(CLIMB)) return R_MAZEA42;
    if (used_movement_verb(EAST)) return R_WFISS;
    if (used_movement_verb(WEST)) return R_ELONG;
    if (used_movement_verb(CRAWL)) return R_ELONG;
    if (keywordv(NORTH)) {
        puts("You have crawled around in a little passage north of and parallel\n"
             "to the Hall of Mists.");
        return R_WFISS;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_mazea42(void)
{
    if (used_movement_verb(UP)) return R_WMIST;
    if (used_movement_verb(NORTH)) return R_MAZEA42;
    if (used_movement_verb(EAST)) return R_MAZEA43;
    if (used_movement_verb(SOUTH)) return R_MAZEA45;
    if (used_movement_verb(WEST)) return R_MAZEA80;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea43(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA42;
    if (used_movement_verb(SOUTH)) return R_MAZEA44;
    if (used_movement_verb(EAST)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea44(void)
{
    if (used_movement_verb(EAST)) return R_MAZEA43;
    if (used_movement_verb(DOWN)) return R_MAZEA48;
    if (used_movement_verb(SOUTH)) return R_MAZEA50;
    if (used_movement_verb(NORTH)) return R_MAZEA82;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea45(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA42;
    if (used_movement_verb(NORTH)) return R_MAZEA43;
    if (used_movement_verb(EAST)) return R_MAZEA46;
    if (used_movement_verb(SOUTH)) return R_MAZEA47;
    if (used_movement_verb(UP)) return R_MAZEA87;
    if (used_movement_verb(DOWN)) return R_MAZEA87;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea46(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA45;
    if (used_movement_verb(OUT)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea47(void)
{
    if (used_movement_verb(EAST)) return R_MAZEA45;
    if (used_movement_verb(OUT)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea48(void)
{
    if (used_movement_verb(UP)) return R_MAZEA44;
    if (used_movement_verb(OUT)) return R_MAZEA44;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea49(void)
{
    if (used_movement_verb(EAST)) return R_MAZEA50;
    if (used_movement_verb(WEST)) return R_MAZEA51;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea50(void)
{
    if (used_movement_verb(EAST)) return R_MAZEA44;
    if (used_movement_verb(WEST)) return R_MAZEA49;
    if (used_movement_verb(DOWN)) return R_MAZEA51;
    if (used_movement_verb(SOUTH)) return R_MAZEA52;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea51(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA49;
    if (used_movement_verb(UP)) return R_MAZEA50;
    if (used_movement_verb(EAST)) return R_MAZEA52;
    if (used_movement_verb(SOUTH)) return R_MAZEA53;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea52(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA50;
    if (used_movement_verb(EAST)) return R_MAZEA51;
    if (used_movement_verb(SOUTH)) return R_MAZEA52;
    if (used_movement_verb(UP)) return R_MAZEA53;
    if (used_movement_verb(NORTH)) return R_MAZEA55;
    if (used_movement_verb(DOWN)) return R_MAZEA86;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea53(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA51;
    if (used_movement_verb(NORTH)) return R_MAZEA52;
    if (used_movement_verb(SOUTH)) return R_MAZEA54;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea54(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA53;
    if (used_movement_verb(OUT)) return R_MAZEA53;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea55(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA52;
    if (used_movement_verb(NORTH)) return R_MAZEA55;
    if (used_movement_verb(DOWN)) return R_MAZEA56;
    if (used_movement_verb(EAST)) return R_MAZEA57_PIT;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea56(void)
{
    if (used_movement_verb(UP)) return R_MAZEA55;
    if (used_movement_verb(OUT)) return R_MAZEA55;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea57_pit(void)
{
    if (used_movement_verb(DOWN)) return R_BIRD;
    if (used_movement_verb(CLIMB)) return R_BIRD;
    if (used_movement_verb(WEST)) return R_MAZEA55;
    if (used_movement_verb(SOUTH)) return R_MAZEA58;
    if (used_movement_verb(NORTH)) return R_MAZEA83;
    if (used_movement_verb(EAST)) return R_MAZEA84;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea58(void)
{
    if (used_movement_verb(EAST)) return R_MAZEA57_PIT;
    if (used_movement_verb(OUT)) return R_MAZEA57_PIT;
    return 0;  /* command hasn't been processed yet */
}

int at_elong(void)
{
    if (used_movement_verb(EAST)) return R_WMIST;
    if (used_movement_verb(UP)) return R_WMIST;
    if (used_movement_verb(CRAWL)) return R_WMIST;
    if (used_movement_verb(WEST)) return R_WLONG;
    if (used_movement_verb(NORTH)) return R_CROSS;
    if (used_movement_verb(DOWN)) return R_CROSS;
    if (used_movement_verb(HOLE)) return R_CROSS;
    return 0;  /* command hasn't been processed yet */
}

int at_wlong(void)
{
    if (used_movement_verb(EAST)) return R_ELONG;
    if (used_movement_verb(NORTH)) return R_CROSS;
    if (used_movement_verb(SOUTH)) return R_MAZED107;
    return 0;  /* command hasn't been processed yet */
}

int at_cross(void)
{
    if (used_movement_verb(WEST)) return R_ELONG;
    if (used_movement_verb(NORTH)) return R_DEADEND1;
    if (used_movement_verb(EAST)) return R_WEST;
    if (used_movement_verb(SOUTH)) return R_WLONG;
    return 0;  /* command hasn't been processed yet */
}

int at_deadend1(void)
{
    if (used_movement_verb2(SOUTH, OUT)) return R_CROSS;
    return 0;  /* command hasn't been processed yet */
}

int at_complex(void)
{
    if (used_movement_verb2(CLIMB, UP)) return R_DUSTY;
    if (used_movement_verb(WEST)) return R_BEDQUILT;
    if (used_movement_placeword(R_BEDQUILT)) return R_BEDQUILT;
    if (used_movement_verb(NORTH)) return R_SHELL;
    if (used_movement_placeword(R_SHELL)) return R_SHELL;
    if (used_movement_verb(EAST)) return R_ANTE;
    return 0;  /* command hasn't been processed yet */
}

int at_bedquilt(void)
{
    if (used_movement_verb(EAST)) return R_COMPLEX;
    if (used_movement_verb(WEST)) return R_SWISS;
    if (used_movement_placeword(R_SLAB)) return R_SLAB;
    if (used_movement_verb2(NORTH, SOUTH) || used_movement_verb2(UP, DOWN)) {
        if (pct(65)) {
            puts("You have crawled around in some little holes and found your way\n"
                 "blocked by a recent cave-in.  You are now back in the main passage.");
            return STAY_STILL;
        }
        if (used_movement_verb(DOWN)) return R_ANTE;
        if (used_movement_verb(NORTH)) return pct(75) ? R_LOW : R_SJUNC;
        if (used_movement_verb(UP)) return pct(75) ? R_DUSTY : R_ABOVEP;
        if (used_movement_verb(SOUTH)) return pct(75) ? R_SLAB : R_TALLEWCNYN;
    }
    if (keywordv(NW)) return pct(50) ? R_ORIENTAL : you_didnt_move();
    return 0;  /* command hasn't been processed yet */
}

int at_swiss(void)
{
    if (used_movement_verb(NE)) return R_BEDQUILT;
    if (used_movement_verb(WEST)) return R_E2PIT;
    if (used_movement_placeword(R_AWK)) return R_TALLEWCNYN;
    if (used_movement_verb(EAST)) return R_SOFT;
    if (used_movement_placeword(R_ORIENTAL)) return R_ORIENTAL;
    if (used_movement_placeword(R_SOFT)) return R_SOFT;
    if (used_movement_verb(NW)) return pct(65) ? R_ORIENTAL : you_didnt_move();
    if (used_movement_verb(SOUTH)) return pct(65) ? R_TALLEWCNYN : you_didnt_move();
    return 0;  /* command hasn't been processed yet */
}

int at_e2pit(void)
{
    if (used_movement_verb(EAST)) return R_SWISS;
    if (used_movement_verb2(WEST, CROSS)) return R_W2PIT;
    if (used_movement_verb(DOWN)) return R_EPIT;
    if (used_movement_placeword(R_SPIT)) return R_EPIT;
    return 0;  /* command hasn't been processed yet */
}

int at_slab(void)
{
    if (used_movement_verb(SOUTH)) return R_W2PIT;
    if (used_movement_verb(UP)) return R_ABOVER;
    if (used_movement_verb(CLIMB)) return R_ABOVER;
    if (used_movement_verb(NORTH)) return R_BEDQUILT;
    if (used_movement_placeword(R_BEDQUILT)) return R_BEDQUILT;
    return 0;  /* command hasn't been processed yet */
}

int at_abover(void)
{
    if (used_movement_verb(DOWN)) return R_SLAB;
    if (used_movement_placeword(R_SLAB)) return R_SLAB;
    if (used_movement_verb(NORTH)) return R_MIRROR;
    if (keywordo(MIRROR)) return R_MIRROR;
    if (used_movement_placeword(R_RES)) return R_RES;
    if (used_movement_verb(SOUTH)) return R_SECRETCYNNE1;
    return 0;  /* command hasn't been processed yet */
}

int at_abovep(void)
{
    if (used_movement_verb(NORTH)) return R_SJUNC;
    if (used_movement_verb(DOWN)) return R_BEDQUILT;
    if (used_movement_verb(PASSAGE)) return R_BEDQUILT;
    if (used_movement_verb(SOUTH)) return R_STALACT;
    return 0;  /* command hasn't been processed yet */
}

int at_sjunc(void)
{
    if (used_movement_verb(SE)) return R_BEDQUILT;
    if (used_movement_verb(SOUTH)) return R_ABOVEP;
    if (used_movement_verb(NORTH)) return R_WINDOW;
    return 0;  /* command hasn't been processed yet */
}

int at_low(void)
{
    if (used_movement_placeword(R_BEDQUILT)) return R_BEDQUILT;
    if (used_movement_verb(SW)) return R_SLOPING;
    if (used_movement_verb(NORTH)) return R_DEADEND2;
    if (used_movement_verb(SE)) return R_ORIENTAL;
    if (used_movement_placeword(R_ORIENTAL)) return R_ORIENTAL;
    return 0;  /* command hasn't been processed yet */
}

int at_deadend2(void)
{
    if (used_movement_verb3(SOUTH, CRAWL, OUT)) return R_LOW;
    return 0;  /* command hasn't been processed yet */
}

int at_secretew_tite(void)
{
    if (used_movement_verb(EAST)) return R_HMK;
    if (used_movement_verb(DOWN)) return R_NSCANYONWIDE;
    if (keywordv(WEST)) {
        return objs[DRAGON].prop ? R_SECRETCYNNE1 : R_SECRETCYNNE2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_nscanyonwide(void)
{
    if (used_movement_verb(SOUTH)) return R_TIGHTERSTILL;
    if (used_movement_verb(NORTH)) return R_TALLEWCNYN;
    return 0;  /* command hasn't been processed yet */
}

int at_tighterstill(void)
{
    if (used_movement_verb(NORTH)) return R_NSCANYONWIDE;
    return 0;  /* command hasn't been processed yet */
}

int at_tallewcnyn(void)
{
    if (used_movement_verb(EAST)) return R_NSCANYONWIDE;
    if (used_movement_verb(WEST)) return R_DEADEND3;
    if (used_movement_verb2(NORTH, CRAWL)) return R_SWISS;
    return 0;  /* command hasn't been processed yet */
}

int at_deadend3(void)
{
    if (used_movement_verb(SOUTH)) return R_TALLEWCNYN;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea80(void)
{
    if (used_movement_verb(NORTH)) return R_MAZEA42;
    if (used_movement_verb(WEST)) return R_MAZEA80;
    if (used_movement_verb(SOUTH)) return R_MAZEA80;
    if (used_movement_verb(EAST)) return R_MAZEA81;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea81(void)
{
    if (used_movement_verb(WEST)) return R_MAZEA80;
    if (used_movement_verb(OUT)) return R_MAZEA80;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea82(void)
{
    if (used_movement_verb(SOUTH)) return R_MAZEA44;
    if (used_movement_verb(OUT)) return R_MAZEA44;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea83(void)
{
    if (used_movement_verb(SOUTH)) return R_MAZEA57_PIT;
    if (used_movement_verb(EAST)) return R_MAZEA84;
    if (used_movement_verb(WEST)) return R_MAZEA85;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea84(void)
{
    if (used_movement_verb(NORTH)) return R_MAZEA57_PIT;
    if (used_movement_verb(WEST)) return R_MAZEA83;
    if (used_movement_verb(NW)) return R_PIRATES_NEST;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea85(void)
{
    if (used_movement_verb(EAST)) return R_MAZEA83;
    if (used_movement_verb(OUT)) return R_MAZEA83;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea86(void)
{
    if (used_movement_verb(UP)) return R_MAZEA52;
    if (used_movement_verb(OUT)) return R_MAZEA52;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea87(void)
{
    if (used_movement_verb(UP)) return R_MAZEA45;
    if (used_movement_verb(DOWN)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_narrow(void)
{
    if (used_movement_verb(DOWN)) return R_WPIT;
    if (used_movement_verb(CLIMB)) return R_WPIT;
    if (used_movement_verb(EAST)) return R_WPIT;
    if (used_movement_verb(WEST)) return R_GIANT;
    if (used_movement_placeword(R_GIANT)) return R_GIANT;
    if (keywordv(JUMP)) return splatter(R_WPIT);
    return 0;  /* command hasn't been processed yet */
}

int at_incline(void)
{
    if (used_movement_verb(NORTH)) return R_FALLS;
    if (used_movement_placeword(R_FALLS)) return R_FALLS;
    if (used_movement_verb(PASSAGE)) return R_FALLS;
    if (used_movement_verb2(DOWN, CLIMB)) return oof(R_LOW);
    return 0;  /* command hasn't been processed yet */
}

int at_giant(void)
{
    if (used_movement_verb(SOUTH)) return R_NARROW;
    if (used_movement_placeword(R_CORRIDOR)) return R_NARROW;
    if (used_movement_verb(EAST)) return R_TUNNEL_1;
    if (used_movement_verb(NORTH)) return R_IMMENSENSPASS;
    return 0;  /* command hasn't been processed yet */
}

int at_immensenspass(void)
{
    if (used_movement_verb2(SOUTH, PASSAGE)) return R_GIANT;
    if (used_movement_placeword(R_GIANT)) return R_GIANT;
    if (used_movement_verb2(NORTH, IN) || used_movement_placeword(R_FALLS)) {
        if (objs[DOOR].prop) return R_FALLS;
        puts("The door is extremely rusty and refuses to open.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_falls(void)
{
    if (used_movement_verb(SOUTH)) return R_IMMENSENSPASS;
    if (used_movement_verb(OUT)) return R_IMMENSENSPASS;
    if (used_movement_placeword(R_GIANT)) return R_GIANT;
    if (used_movement_verb(WEST)) return R_INCLINE;
    if (used_movement_placeword(R_INCLINE)) return R_INCLINE;
    if (used_movement_verb2(DOWN, JUMP)) {
        if (!yes("Into the whirlpool??")) {
            ok();
            return STAY_STILL;
        }
        bool something_got_lost = false;
        for (int i=1; i <= MAX_OBJ; ++i) {
            if (toting(i) && i != LAMP) {
                apport(i, R_YLEM);
                something_got_lost = true;
            }
        }
        if (toting(LAMP)) {
            holding_count = 1;
            if (something_got_lost) {
                puts("You plunge into the water and are sucked down by the whirlpool.  The\n"
                     "current is incredibly strong, and you barely manage to hold onto\n"
                     "your lamp;  everything else is pulled from your grasp and is lost in\n"
                     "the swirling waters.");
            } else {
                puts("You plunge into the water and are sucked down by the whirlpool.");
            }
        } else {
            holding_count = 0;
            if (something_got_lost) {
                puts("You plunge into the water and are sucked down by the whirlpool into\n"
                     "pitch darkness.  The current is incredibly strong, and everything that\n"
                     "you are carrying is ripped from your grasp and is lost in the swirling\n"
                     "waters.");
            } else {
                puts("You plunge into the water and are sucked down by the whirlpool into\n"
                     "pitch darkness.");
            }
        }
        puts("\nThe swirling waters deposit you, not ungently, on solid ground.\n");
        return R_RESERVOIR_N;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_soft(void)
{
    if (used_movement_verb2(WEST, OUT)) return R_SWISS;
    if (used_movement_placeword(R_SWISS)) return R_SWISS;
    return 0;  /* command hasn't been processed yet */
}

int at_oriental(void)
{
    if (used_movement_verb(SE)) return R_SWISS;
    if (used_movement_verb(WEST)) return R_LOW;
    if (used_movement_verb(CRAWL)) return R_LOW;
    if (used_movement_verb(UP)) return R_MISTY;
    if (used_movement_verb(NORTH)) return R_MISTY;
    if (used_movement_placeword(R_FALLS)) return R_MISTY;
    return 0;  /* command hasn't been processed yet */
}

int at_misty(void)
{
    if (used_movement_verb(SOUTH)) return R_ORIENTAL;
    if (used_movement_placeword(R_ORIENTAL)) return R_ORIENTAL;
    if (used_movement_verb(WEST)) return R_ALCOVE;
    if (keywordv(JUMP)) return splatter(R_FALLS);
    return 0;  /* command hasn't been processed yet */
}

int tight_squeeze(void)
{
    puts("Something you're carrying won't fit through the tunnel with you.\n"
         "You'd best take inventory and drop something.");
    return STAY_STILL;
}

int at_alcove(void)
{
    if (used_movement_verb2(EAST, PASSAGE) || used_movement_placeword(R_PLOVER)) {
        if (holding_count == 0) return R_PLOVER;
        if (holding_count == 1 && toting(EMERALD)) return R_PLOVER;
        return tight_squeeze();
    }
    if (used_movement_verb(NW)) return R_MISTY;
    if (used_movement_placeword(R_FALLS)) return R_MISTY;
    return 0;  /* command hasn't been processed yet */
}

int at_plover(void)
{
    if (keywordp(R_PLOVER) && (word2.type==WordType_None || keywordv(SAY))) {
        if (toting(EMERALD)) apport(EMERALD, R_PLOVER);
        say_foof();
        return R_Y2;
    }
    if (used_movement_verb2(WEST, PASSAGE) || used_movement_placeword(R_ALCOVE)) {
        if (holding_count == 0) return R_ALCOVE;
        if (holding_count == 1 && toting(EMERALD)) return R_ALCOVE;
        return tight_squeeze();
    }
    if (used_movement_verb(NE)) return R_DARK;
    if (used_movement_placeword(R_DARK)) return R_DARK;
    return 0;  /* command hasn't been processed yet */
}

int at_dark(void)
{
    if (used_movement_verb2(SOUTH, OUT)) return R_PLOVER;
    if (used_movement_placeword(R_PLOVER)) return R_PLOVER;
    return 0;  /* command hasn't been processed yet */
}

int at_arched(void)
{
    if (used_movement_verb(DOWN)) return R_SHELL;
    if (used_movement_placeword(R_SHELL)) return R_SHELL;
    if (used_movement_verb(UP)) return R_ARCH_COR_1;
    if (used_movement_verb(EAST)) return R_ARCH_COR_1;
    return 0;  /* command hasn't been processed yet */
}

int at_shell(void)
{
    if (used_movement_verb(UP)) return R_ARCHED;
    if (used_movement_verb(HALL)) return R_ARCHED;
    if (used_movement_verb(DOWN)) return R_RAGGED;
    if (used_movement_placeword(R_CORRIDOR)) return R_RAGGED;
    if (keywordv(SOUTH) || keywordp(R_COMPLEX)) {
        if (toting(CLAM)) {
            puts("You can't fit this five-foot clam through that little passage!");
            return STAY_STILL;
        } else if (toting(OYSTER)) {
            puts("You can't fit this five-foot oyster through that little passage!");
            return STAY_STILL;
        } else {
            return R_COMPLEX;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_ragged(void)
{
    if (used_movement_verb(UP)) return R_SHELL;
    if (used_movement_placeword(R_SHELL)) return R_SHELL;
    if (used_movement_verb(DOWN)) return R_CULDESAC;
    return 0;  /* command hasn't been processed yet */
}

int at_culdesac(void)
{
    if (used_movement_verb2(UP, OUT)) return R_RAGGED;
    if (used_movement_placeword(R_CORRIDOR)) return R_RAGGED;
    if (used_movement_placeword(R_SHELL)) return R_SHELL;
    return 0;  /* command hasn't been processed yet */
}

int at_anteroom(void)
{
    if (used_movement_verb(UP)) return R_COMPLEX;
    if (used_movement_verb(WEST)) return R_BEDQUILT;
    if (used_movement_verb(EAST)) return R_WITT;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed107(void)
{
    if (used_movement_verb(SOUTH)) return R_MAZED131;
    if (used_movement_verb(SW)) return R_MAZED132;
    if (used_movement_verb(NE)) return R_MAZED133;
    if (used_movement_verb(SE)) return R_MAZED134;
    if (used_movement_verb(UP)) return R_MAZED135;
    if (used_movement_verb(NW)) return R_MAZED136;
    if (used_movement_verb(EAST)) return R_MAZED137;
    if (used_movement_verb(WEST)) return R_MAZED138;
    if (used_movement_verb(NORTH)) return R_MAZED139;
    if (used_movement_verb(DOWN)) return R_WLONG;
    return 0;  /* command hasn't been processed yet */
}

int at_witt(void)
{
    if (used_movement_verb3(NORTH,SOUTH,UP) ||
        used_movement_verb3(EAST,DOWN,NE) ||
        used_movement_verb3(NW,SE,SW)) {
        return pct(95) ? R_WITT : R_ANTE;
    }
    if (used_movement_verb(WEST)) return R_WITT;
    return 0;  /* command hasn't been processed yet */
}

int at_mirror(void)
{
    if (keywordo(MIRROR)) {
        puts("It is too far up for you to reach.");
        return STAY_STILL;
    }
    if (used_movement_verb(SOUTH)) return R_ABOVER;
    if (used_movement_verb(NORTH)) return R_RES;
    if (used_movement_placeword(R_RES)) return R_RES;
    return 0;  /* command hasn't been processed yet */
}

int at_window(void)
{
    if (used_movement_verb(WEST)) return R_SJUNC;
    if (keywordv(JUMP)) {
        puts("You are at the bottom of the pit with a broken neck.");
        return you_are_dead_at(R_MIRROR);
    }
    return 0;  /* command hasn't been processed yet */
}

int at_stalact(void)
{
    if (used_movement_verb(NORTH)) return R_ABOVEP;
    if (keywordv(DOWN) || keywordv(JUMP) || keywordv(CLIMB)) {
        return pct(40) ? R_MAZEA50 : pct(50) ? R_MAZEA53 : R_MAZEA45;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_mazed112(void)
{
    if (used_movement_verb(SW)) return R_MAZED131;
    if (used_movement_verb(NORTH)) return R_MAZED132;
    if (used_movement_verb(EAST)) return R_MAZED133;
    if (used_movement_verb(NW)) return R_MAZED134;
    if (used_movement_verb(SE)) return R_MAZED135;
    if (used_movement_verb(NE)) return R_MAZED136;
    if (used_movement_verb(WEST)) return R_MAZED137;
    if (used_movement_verb(DOWN)) return R_MAZED138;
    if (used_movement_verb(UP)) return R_MAZED139;
    if (used_movement_verb(SOUTH)) return R_PONY;
    return 0;  /* command hasn't been processed yet */
}

int cant_walk_on_water(void)
{
    puts("I can't swim, or walk on water.  You'll have to find some other way\n"
         "to get across, or get someone to assist you.");
    return STAY_STILL;
}

int at_res(void)
{
    if (used_movement_verb2(SOUTH, OUT)) return R_MIRROR;
    if (used_movement_obj(MIRROR)) return R_MIRROR;
    if (keywordv(NORTH) || keywordv(CROSS)) {
        return cant_walk_on_water();
    }
    return 0;  /* command hasn't been processed yet */
}

int at_reservoir_n(void)
{
    if (used_movement_verb2(NORTH, PASSAGE)) return R_WARM;
    if (used_movement_placeword(R_WARM)) return R_WARM;
    if (used_movement_placeword(R_BALCONY)) return R_BALCONY;
    if (used_movement_verb2(SOUTH, CROSS)) {
        if (there(TURTLE, R_RESERVOIR_N)) {
            puts("You step gently on Darwin the Tortoise's back, and he carries you\n"
                 "smoothly over to the southern side of the reservoir.  He then blows\n"
                 "a couple of bubbles at you and sinks back out of sight.\n");
            apport(TURTLE, R_LIMBO);
            return R_RES;
        } else {
            return cant_walk_on_water();
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_warm(void)
{
    if (used_movement_verb(SOUTH)) return R_RESERVOIR_N;
    if (used_movement_placeword(R_RES)) return R_RESERVOIR_N;
    if (used_movement_verb(NE)) return R_BALCONY;
    if (used_movement_placeword(R_BALCONY)) return R_BALCONY;
    return 0;  /* command hasn't been processed yet */
}

int at_balcony(void)
{
    if (used_movement_verb2(WEST, OUT)) return R_WARM;
    if (used_movement_placeword(R_WARM)) return R_WARM;
    if (used_movement_placeword(R_RES)) return R_RESERVOIR_N;
    /* Jumping from the balcony will make your possessions unrecoverable. */
    if (keywordv(JUMP)) return splatter(R_YLEM);
    return 0;  /* command hasn't been processed yet */
}

int at_pirates_nest(void)
{
    if (used_movement_verb(SE)) return R_MAZEA84;
    return 0;  /* command hasn't been processed yet */
}

int at_swofchasm(void)
{
    if (used_movement_verb(SW)) return R_SLOPING;
    if (used_movement_placeword(R_SLOPING)) return R_SLOPING;
    if (used_movement_placeword(R_CORRIDOR)) return R_SLOPING;

    if (keywordv(THROW)) {
        if (word2.type == WordType_None || there(TROLL2, R_SWOFCHASM))
            return 0;  /* unhandled */
        if (keywordo(AXE) || keywordo(SWORD)) {
            if (toting(word2.meaning)) {
                printf("The troll deftly catches the %s, examines it carefully, and tosses it\n"
                       "back, declaring, \"Good workmanship, but it's not valuable enough.\"\n", word2.text);
                apport(word2.meaning, R_SWOFCHASM);
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        } else if (word2.type == WordType_Object && is_treasure(word2.meaning)) {
            const ObjectWord o = word2.meaning;
            if (!toting(o)) return 0;  /* unhandled */
            if ((o == EGGS) && have_stolen_back_eggs) {
                puts("The troll nimbly steps to one side and grins nastily as the nest of\n"
                     "golden eggs flies past him and plummets into the chasm.  \"Fool me\n"
                     "once, shame on you;  fool me twice, shame on me!\" he sneers.  \"I want\n"
                     "something a touch more substantial this time!\"");
                apport(EGGS, R_YLEM);
                objs[TROLL].prop = 0;
                return STAY_STILL;
            }
            printf("The troll catches the %s and scurries away out of sight.\n", word2.text);
            objs[TROLL].prop = 1;
            apport(TROLL, R_LIMBO);
            apport(TROLL2, R_SWOFCHASM);
            apport(o, R_LIMBO);
            return STAY_STILL;
        }
    }
    if (used_movement_verb2(CROSS, NE)) {
        if (objs[CHASM].prop) {
            puts("There is no longer any way across the chasm.");
        } else if (objs[TROLL].prop == 0) {
            puts("The troll refuses to let you cross.");
        } else if (objs[TROLL].prop == 2) {
            puts("The troll steps out from beneath the bridge and blocks your way.");
            objs[TROLL].prop = 0;
            apport(TROLL2, R_LIMBO);
            apport(TROLL, R_SWOFCHASM);
        } else {
            if (objs[TROLL].prop == 1)
                objs[TROLL].prop = 2;  /* no longer appeased */
            return R_NEOFCHASM;
        }
        return STAY_STILL;
    }
    if (keywordv(JUMP)) {
        if (objs[CHASM].prop) return splatter(R_YLEM);
        puts("I respectfully suggest you go across the bridge instead of jumping.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_sloping(void)
{
    if (used_movement_verb(DOWN)) return R_LOW;
    if (used_movement_verb(UP)) return R_SWOFCHASM;
    if (used_movement_obj(CHASM)) return R_SWOFCHASM;
    if (used_movement_placeword(R_LOW)) return R_LOW;
    if (used_movement_verb(OUT)) return R_LOW;
    return 0;  /* command hasn't been processed yet */
}

int at_secretcynne1(void)
{
    if (keywordo(RUG) && !objs[DRAGON].prop) {
        puts("You can't get by the dragon to get at the rug.");
        return STAY_STILL;
    }
    if (used_movement_verb(NORTH)) return R_ABOVER;
    if (used_movement_verb(OUT)) return R_ABOVER;
    if (keywordv(FORWARD) || keywordv(EAST)) {
        if (!objs[DRAGON].prop) {
            puts("The dragon looks rather nasty.  You'd best not try to get by.");
            return STAY_STILL;
        } else {
            return R_SECRETEW_TITE;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_secretcynne2(void)
{
    if (keywordo(RUG) && !objs[DRAGON].prop) {
        puts("You can't get by the dragon to get at the rug.");
        return STAY_STILL;
    }
    if (used_movement_verb(EAST)) return R_SECRETEW_TITE;
    if (used_movement_verb(OUT)) return R_SECRETEW_TITE;
    if (used_movement_verb2(FORWARD, NORTH)) {
        if (!objs[DRAGON].prop) {
            puts("The dragon looks rather nasty.  You'd best not try to get by.");
            return STAY_STILL;
        } else {
            return R_ABOVER;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_neofchasm(void)
{
    if (keywordv(DROP) && keywordo(BEAR) && toting(BEAR) && there(TROLL, R_NEOFCHASM)) {
        objs[TROLL].prop = 4;
        puts("The bear lumbers toward the troll, who lets out a startled shriek and\n"
             "scurries away.  The bear soon gives up the pursuit and wanders back.");
        apport(TROLL, R_LIMBO);
        apport(TROLL2, R_SWOFCHASM);
        apport(BEAR, R_NEOFCHASM);
        return STAY_STILL;
    }
    if (keywordv(THROW)) {
        if (word2.type == WordType_None || there(TROLL2, R_NEOFCHASM))
            return 0;  /* unhandled */
        if (keywordo(AXE) || keywordo(SWORD)) {
            if (toting(word2.meaning)) {
                printf("The troll deftly catches the %s, examines it carefully, and tosses it\n"
                       "back, declaring, \"Good workmanship, but it's not valuable enough.\"\n", word2.text);
                apport(word2.meaning, R_NEOFCHASM);
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        } else if (word2.type == WordType_Object && is_treasure(word2.meaning)) {
            const ObjectWord o = word2.meaning;
            if (!toting(o)) return 0;  /* unhandled */
            if ((o == EGGS) && have_stolen_back_eggs) {
                puts("The troll nimbly steps to one side and grins nastily as the nest of\n"
                     "golden eggs flies past him and plummets into the chasm.  \"Fool me\n"
                     "once, shame on you;  fool me twice, shame on me!\" he sneers.  \"I want\n"
                     "something a touch more substantial this time!");
                apport(EGGS, R_YLEM);
                objs[TROLL].prop = 0;
                return STAY_STILL;
            }
            printf("The troll catches the %s and scurries away out of sight.\n", word2.text);
            objs[TROLL].prop = 1;
            apport(TROLL, R_LIMBO);
            apport(TROLL2, R_SWOFCHASM);
            apport(o, R_LIMBO);
            return STAY_STILL;
        }
    }

    if (used_movement_verb2(CROSS, SW)) {
        if (objs[CHASM].prop > 0) {
            puts("There is no longer any way across the chasm.");
        } else if (objs[TROLL].prop == 0) {
            puts("The troll refuses to let you cross.");
        } else if (objs[TROLL].prop == 2) {
            puts("The troll steps out from beneath the bridge and blocks your way.");
            objs[TROLL].prop = 0;
            apport(TROLL2, R_LIMBO);
            apport(TROLL, R_SWOFCHASM);
        } else {
            /* We're getting away with it! */
            if (objs[TROLL].prop == 1)
                objs[TROLL].prop = 2;  /* no longer appeased */
            if (toting(BEAR)) {
                puts("Just as you reach the other side, the bridge buckles beneath the\n"
                     "weight of the bear, which was still following you around.  You\n"
                     "scrabble desperately for support, but as the bridge collapses you\n"
                     "stumble back and fall into the chasm.");
                objs[CHASM].prop = 1;
                apport(TROLL2, R_LIMBO);
                return you_are_dead_at(R_YLEM);
            }
            if (objs[TROLL].prop == 4 && have_stolen_back_eggs) {
                puts("As you reach the middle of the bridge, the troll appears from out\n"
                     "of the tunnel behind you, wearing a large backpack.  \"So, Mister\n"
                     "Magician,\" he shouts, \"you like to use magic to steal back my hard-\n"
                     "earned toll?  Let's see how you like a little of MY magic!!\"  With");
                if (there(BEAR, R_NEOFCHASM)) {
                    puts("that, he aims a tube running from the backpack directly at the bear\n"
                         "and pulls a trigger.  A spout of magical fire roars out and singes the\n"
                         "bear's fur;  the bear bellows in pain and dashes onto the bridge to\n"
                         "escape.  The bridge shudders, groans, and collapses under the weight,\n"
                         "and you and the bear plunge down into the chasm.");
                    apport(BEAR, R_YLEM);
                    objs[CHASM].prop = 1;
                } else {
                    puts("that, he aims a tube running from the backpack directly at the bridge\n"
                         "and pulls a trigger.  A spout of magical fire roars out and incinerates\n"
                         "the bridge supports, causing the bridge to sway giddily and collapse\n"
                         "into the chasm.  You plunge down to your death.");
                    objs[CHASM].prop = 2;
                }
                return you_are_dead_at(R_YLEM);
            }
            return R_SWOFCHASM;
        }
        return STAY_STILL;
    }
    if (keywordv(JUMP)) {
        if (objs[CHASM].prop) return splatter(R_YLEM);
        puts("I respectfully suggest you go across the bridge instead of jumping.");
        return STAY_STILL;
    }
    if (used_movement_verb(NE)) return R_CORRIDOR;
    if (used_movement_placeword(R_CORRIDOR)) return R_CORRIDOR;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb(VIEW)) return R_VIEW;
    if (used_movement_placeword(R_FBARR)) return R_FBARR;
    return 0;  /* command hasn't been processed yet */
}

int at_corridor(void)
{
    if (used_movement_verb(CHASM)) return R_NEOFCHASM;
    if (used_movement_verb(WEST)) return R_NEOFCHASM;
    if (used_movement_verb(EAST)) return R_FORK;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb(VIEW)) return R_VIEW;
    if (used_movement_placeword(R_FBARR)) return R_FBARR;
    return 0;  /* command hasn't been processed yet */
}

int at_fork(void)
{
    if (used_movement_verb(CHASM)) return R_NEOFCHASM;
    if (used_movement_verb(WEST)) return R_CORRIDOR;
    if (used_movement_placeword(R_CORRIDOR)) return R_CORRIDOR;
    if (used_movement_verb(NE)) return R_WARMJUNCTN;
    if (used_movement_verb(LEFT)) return R_WARMJUNCTN;
    if (used_movement_verb(SE)) return R_LIME;
    if (used_movement_verb(RIGHT)) return R_LIME;
    if (used_movement_verb(DOWN)) return R_LIME;
    if (used_movement_verb(VIEW)) return R_VIEW;
    if (used_movement_placeword(R_FBARR)) return R_FBARR;
    return 0;  /* command hasn't been processed yet */
}

int at_warmjunctn(void)
{
    if (used_movement_verb(SOUTH)) return R_FORK;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb(NORTH)) return R_VIEW;
    if (used_movement_verb(VIEW)) return R_VIEW;
    if (used_movement_verb2(EAST, CRAWL)) return R_CHAMBER;
    return 0;  /* command hasn't been processed yet */
}

int at_view(void)
{
    if (used_movement_verb(SOUTH)) return R_WARMJUNCTN;
    if (used_movement_verb(PASSAGE)) return R_WARMJUNCTN;
    if (used_movement_verb(OUT)) return R_WARMJUNCTN;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (keywordv(JUMP) || keywordv(DOWN) || keywordv(CLIMB))
        return splatter(R_YLEM);
    if (keywordp(R_VALLEY) || keywordv(CROSS) || keywordo(GORGE) || keywordv(NORTH)) {
        if (objs[GORGE].prop == 0) {
            puts("I'm afraid I can't go that way - walking on red-hot lava is contrary\n"
                 "to union regulations (and is bad for your health anyhow).");
            return STAY_STILL;
        } else if (toting(RING)) {
            if (toting(BEAR)) {
                puts("As you approach the center of the archway, hot vapors saturated with\n"
                     "brimstone drift up from the lava in the gorge beneath your feet.  The\n"
                     "mithril ring in your hand quivers and glows, and a swirling vortex\n"
                     "of white vapor encircles you and protects you from the fumes.  The\n"
                     "bear is not so lucky, though, since the vortex is rather small; the\n"
                     "noxious gasses choke it and it staggers off of the side of the arch\n"
                     "and plummets into the gorge below.\n");
                apport(BEAR, R_YLEM);
            } else if (!(places[R_FACES].flags & F_BEENHERE)) {
                puts("As you approach the center of the archway, hot vapors saturated with\n"
                     "brimstone drift up from the lava in the gorge beneath your feet.  The\n"
                     "mithril ring in your hand quivers and glows, and the fumes eddy away\n"
                     "from the bridge without harming you.\n");
            }
            return R_FACES;
        } else {
            if (toting(BEAR)) {
                puts("As you approach the center of the archway, hot vapors saturated with\n"
                     "brimstone drift up from the lava in the gorge beneath your feet.  You\n"
                     "and the bear are both overcome by the noxious gasses and, with your\n"
                     "lungs burned out, slip off of the bridge and plummet into the gorge.");
                /* Notice that the bear winds up at R_YLEM with the
                 * rest of your possessions, in this case. */
            } else {
                puts("As you approach the center of the archway, hot vapors saturated with\n"
                     "brimstone drift up from the lava in the gorge beneath your feet.  You\n"
                     "are swiftly overcome by the foul gasses and, with your lungs burned\n"
                     "out, fall off of the bridge and into the gorge.");
            }
            return you_are_dead_at(R_YLEM);
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_chamber(void)
{
    if (used_movement_verb(WEST)) return R_WARMJUNCTN;
    if (used_movement_verb(OUT)) return R_WARMJUNCTN;
    if (used_movement_verb(CRAWL)) return R_WARMJUNCTN;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_lime(void)
{
    if (used_movement_verb(NORTH)) return R_FORK;
    if (used_movement_verb(UP)) return R_FORK;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb(SOUTH)) return R_FBARR;
    if (used_movement_verb(DOWN)) return R_FBARR;
    if (used_movement_placeword(R_FBARR)) return R_FBARR;
    if (used_movement_verb(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_fbarr(void)
{
    if (used_movement_verb(WEST)) return R_LIME;
    if (used_movement_verb(UP)) return R_LIME;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb2(EAST, IN)) return R_BARR;
    if (used_movement_placeword(R_FBARR)) return R_BARR;
    if (used_movement_verb(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_barr(void)
{
    if (keywordv(GET) && keywordo(AXE) && there(AXE, R_BARR) && objs[AXE].prop) {
        puts("As you approach the bear, it snarls threateningly;  you are forced\n"
             "to retreat without the axe.");
        return STAY_STILL;
    }
    if (used_movement_verb(WEST)) return R_FBARR;
    if (used_movement_verb(OUT)) return R_FBARR;
    if (used_movement_placeword(R_FORK)) return R_FORK;
    if (used_movement_verb(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed131(void)
{
    if (used_movement_verb(WEST)) return R_MAZED107;
    if (used_movement_verb(SE)) return R_MAZED132;
    if (used_movement_verb(NW)) return R_MAZED133;
    if (used_movement_verb(SW)) return R_MAZED134;
    if (used_movement_verb(NE)) return R_MAZED135;
    if (used_movement_verb(UP)) return R_MAZED136;
    if (used_movement_verb(DOWN)) return R_MAZED137;
    if (used_movement_verb(NORTH)) return R_MAZED138;
    if (used_movement_verb(SOUTH)) return R_MAZED139;
    if (used_movement_verb(EAST)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed132(void)
{
    if (used_movement_verb(NW)) return R_MAZED107;
    if (used_movement_verb(UP)) return R_MAZED131;
    if (used_movement_verb(NORTH)) return R_MAZED133;
    if (used_movement_verb(SOUTH)) return R_MAZED134;
    if (used_movement_verb(WEST)) return R_MAZED135;
    if (used_movement_verb(SW)) return R_MAZED136;
    if (used_movement_verb(NE)) return R_MAZED137;
    if (used_movement_verb(EAST)) return R_MAZED138;
    if (used_movement_verb(DOWN)) return R_MAZED139;
    if (used_movement_verb(SE)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed133(void)
{
    if (used_movement_verb(UP)) return R_MAZED107;
    if (used_movement_verb(DOWN)) return R_MAZED131;
    if (used_movement_verb(WEST)) return R_MAZED132;
    if (used_movement_verb(NE)) return R_MAZED134;
    if (used_movement_verb(SW)) return R_MAZED135;
    if (used_movement_verb(EAST)) return R_MAZED136;
    if (used_movement_verb(NORTH)) return R_MAZED137;
    if (used_movement_verb(NW)) return R_MAZED138;
    if (used_movement_verb(SE)) return R_MAZED139;
    if (used_movement_verb(SOUTH)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed134(void)
{
    if (used_movement_verb(NE)) return R_MAZED107;
    if (used_movement_verb(NORTH)) return R_MAZED131;
    if (used_movement_verb(NW)) return R_MAZED132;
    if (used_movement_verb(SE)) return R_MAZED133;
    if (used_movement_verb(EAST)) return R_MAZED135;
    if (used_movement_verb(DOWN)) return R_MAZED136;
    if (used_movement_verb(SOUTH)) return R_MAZED137;
    if (used_movement_verb(UP)) return R_MAZED138;
    if (used_movement_verb(WEST)) return R_MAZED139;
    if (used_movement_verb(SW)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed135(void)
{
    if (used_movement_verb(NORTH)) return R_MAZED107;
    if (used_movement_verb(SE)) return R_MAZED131;
    if (used_movement_verb(DOWN)) return R_MAZED132;
    if (used_movement_verb(SOUTH)) return R_MAZED133;
    if (used_movement_verb(EAST)) return R_MAZED134;
    if (used_movement_verb(WEST)) return R_MAZED136;
    if (used_movement_verb(SW)) return R_MAZED137;
    if (used_movement_verb(NE)) return R_MAZED138;
    if (used_movement_verb(NW)) return R_MAZED139;
    if (used_movement_verb(UP)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed136(void)
{
    if (used_movement_verb(EAST)) return R_MAZED107;
    if (used_movement_verb(WEST)) return R_MAZED131;
    if (used_movement_verb(UP)) return R_MAZED132;
    if (used_movement_verb(SW)) return R_MAZED133;
    if (used_movement_verb(DOWN)) return R_MAZED134;
    if (used_movement_verb(SOUTH)) return R_MAZED135;
    if (used_movement_verb(NW)) return R_MAZED137;
    if (used_movement_verb(SE)) return R_MAZED138;
    if (used_movement_verb(NE)) return R_MAZED139;
    if (used_movement_verb(NORTH)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed137(void)
{
    if (used_movement_verb(SE)) return R_MAZED107;
    if (used_movement_verb(NE)) return R_MAZED131;
    if (used_movement_verb(SOUTH)) return R_MAZED132;
    if (used_movement_verb(DOWN)) return R_MAZED133;
    if (used_movement_verb(UP)) return R_MAZED134;
    if (used_movement_verb(NW)) return R_MAZED135;
    if (used_movement_verb(NORTH)) return R_MAZED136;
    if (used_movement_verb(SW)) return R_MAZED138;
    if (used_movement_verb(EAST)) return R_MAZED139;
    if (used_movement_verb(WEST)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed138(void)
{
    if (used_movement_verb(DOWN)) return R_MAZED107;
    if (used_movement_verb(EAST)) return R_MAZED131;
    if (used_movement_verb(NE)) return R_MAZED132;
    if (used_movement_verb(UP)) return R_MAZED133;
    if (used_movement_verb(WEST)) return R_MAZED134;
    if (used_movement_verb(NORTH)) return R_MAZED135;
    if (used_movement_verb(SOUTH)) return R_MAZED136;
    if (used_movement_verb(SE)) return R_MAZED137;
    if (used_movement_verb(SW)) return R_MAZED139;
    if (used_movement_verb(NW)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed139(void)
{
    if (used_movement_verb(SW)) return R_MAZED107;
    if (used_movement_verb(NW)) return R_MAZED131;
    if (used_movement_verb(EAST)) return R_MAZED132;
    if (used_movement_verb(WEST)) return R_MAZED133;
    if (used_movement_verb(NORTH)) return R_MAZED134;
    if (used_movement_verb(DOWN)) return R_MAZED135;
    if (used_movement_verb(SE)) return R_MAZED136;
    if (used_movement_verb(UP)) return R_MAZED137;
    if (used_movement_verb(SOUTH)) return R_MAZED138;
    if (used_movement_verb(NE)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_pony(void)
{
    if (used_movement_verb(NORTH)) return R_MAZED112;
    if (used_movement_verb(OUT)) return R_MAZED112;
    if (keywordv(DROP) && keywordo(COINS) && toting(COINS)) {
        apport(COINS, R_LIMBO);
        apport(BATTERIES, R_PONY);
        ok();
        puts("There are fresh batteries here.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_sandstone(void)
{
    if (used_movement_verb2(WEST, OUT)) return R_EMIST;
    if (used_movement_placeword(R_EMIST)) return R_EMIST;
    return 0;  /* command hasn't been processed yet */
}

int at_morion(void)
{
    if (used_movement_verb2(SOUTH, OUT)) return R_HMK;
    return 0;  /* command hasn't been processed yet */
}

int enter_safe(Location start)
{
    if (objs[SAFE].prop == 1) {
        safe_exit = start;
        return R_INSAFE;
    } else {
        puts("The safe's door is closed, and you can't get in!");
        return STAY_STILL;
    }
}

int at_vault(void)
{
    if (used_movement_verb3(UP,OUT,NORTH)) {
        if (objs[SAFE].prop == 1) {
            puts("The safe's door is blocking the exit passage - you'll have to close\n"
                 "the safe to get out of here.");
            return STAY_STILL;
        } else {
            return R_HMK;
        }
    }
    if (keywordv(IN)) return enter_safe(R_VAULT);
    return 0;  /* command hasn't been processed yet */
}

int at_insafe(void)
{
    if (keywordv(OUT)) return safe_exit;
    if (keywordo(SAFE)) {
        if (word2.type == WordType_None) {
            puts("What do you want me to do with the safe?");
        } else if (keywordv(CLOSE)) {
            puts("There is no handle on the inside of the safe door, nor any other way\n"
                 "to get a grip on it.  You'll have to leave the safe before shutting it.");
        } else if (keywordv(OPEN)) {
            already_open(word2.text);
        } else {
            hah();
        }
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_corrid_1(void)
{
    if (used_movement_verb(SOUTH)) return R_HMK;
    if (used_movement_verb(NORTH)) return R_CORRID_2;
    return 0;  /* command hasn't been processed yet */
}

int at_corrid_2(void)
{
    if (used_movement_verb(SOUTH)) return R_CORRID_1;
    if (used_movement_verb(WEST)) return R_CORRID_1;
    if (used_movement_verb2(NORTH, EAST)) return R_TOOL;
    return 0;  /* command hasn't been processed yet */
}

int at_tool(void)
{
    if (used_movement_verb2(OUT, SOUTH)) return R_CORRID_2;
    return 0;  /* command hasn't been processed yet */
}

int at_corrid_3(void)
{
    if (used_movement_verb(SOUTH)) return R_HMK;
    if (used_movement_verb(NORTH)) return R_SPHERICAL;
    if (used_movement_verb(EAST)) return R_CUBICLE;
    return 0;  /* command hasn't been processed yet */
}

int at_cubicle(void)
{
    if (used_movement_verb(OUT)) return R_CORRID_3;
    if (used_movement_placeword(R_CORRIDOR)) return R_CORRID_3;
    if (used_movement_verb(SOUTH)) return R_CORRID_3;
    return 0;  /* command hasn't been processed yet */
}

int at_spherical(void)
{
    if (used_movement_verb(OUT)) return R_CORRID_3;
    if (used_movement_verb(NORTH)) return R_CORRID_3;
    if (used_movement_placeword(R_CORRIDOR)) return R_CORRID_3;
    return 0;  /* command hasn't been processed yet */
}

int at_tunnel_1(void)
{
    if (used_movement_verb(SOUTH)) return R_GIANT;
    if (used_movement_verb(NORTH)) return R_GLASSY;
    return 0;  /* command hasn't been processed yet */
}

int at_glassy(void)
{
    if (used_movement_verb(SOUTH)) return R_TUNNEL_1;
    if (keywordv(NORTH) || keywordp(R_LAIR)) {
        if (there(OGRE, R_GLASSY)) {
            puts("The ogre growls at you and refuses to let you pass.");
            return STAY_STILL;
        } else {
            return R_LAIR;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_lair(void)
{
    if (used_movement_verb(EAST)) return R_BRINK_1;
    if (used_movement_verb(WEST)) return R_GLASSY;
    return 0;  /* command hasn't been processed yet */
}

int at_brink_1(void)
{
    if (used_movement_verb(NORTH)) return R_LAIR;
    if (used_movement_verb(WEST)) return R_BRINK_2;
    if (used_movement_verb(EAST)) return R_BRINK_3;
    if (keywordv(JUMP)) return plunge();
    return 0;  /* command hasn't been processed yet */
}

int at_brink_2(void)
{
    if (used_movement_verb(NORTH)) return R_BRINK_1;
    if (used_movement_verb(SE)) return R_ICE;
    if (keywordv(JUMP)) return plunge();
    return 0;  /* command hasn't been processed yet */
}

int at_ice(void)
{
    if (used_movement_verb(NW)) return R_BRINK_2;
    if (used_movement_verb2(DOWN, EAST)) return oof(R_SLIDE);
    if (used_movement_placeword(R_SLIDE)) return oof(R_SLIDE);
    return 0;  /* command hasn't been processed yet */
}

int at_slide(void)
{
    if (used_movement_verb3(UP, NORTH, CLIMB)) {
        puts("The icy slide is far too steep and slippery to climb.");
        return STAY_STILL;
    }
    if (used_movement_verb(SOUTH)) return R_ICECAVE2A;
    if (used_movement_verb(NW)) return R_ICECAVE4;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave1(void)
{
    if (used_movement_verb(WEST)) return R_ICECAVE2;
    if (used_movement_verb(NORTH)) return R_ICECAVE1A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave1a(void)
{
    if (used_movement_verb(SOUTH)) return R_ICECAVE1;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave2(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE1;
    if (used_movement_verb(WEST)) return R_ICECAVE3;
    if (used_movement_verb(NORTH)) return R_ICECAVE2A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave2a(void)
{
    if (used_movement_verb(NORTH)) return R_SLIDE;
    if (used_movement_verb(SOUTH)) return R_ICECAVE2;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave3(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE2;
    if (used_movement_verb(NORTH)) return R_ICECAVE3A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave3a(void)
{
    if (used_movement_verb(SOUTH)) return R_ICECAVE3;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave4(void)
{
    if (used_movement_verb(EAST)) return R_SLIDE;
    if (used_movement_verb(WEST)) return R_ICECAVE5;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave5(void)
{
    if (used_movement_verb(NE)) return R_ICECAVE4;
    if (used_movement_verb(SOUTH)) return R_ICECAVE6;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave6(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE5;
    if (used_movement_verb(SOUTH)) return R_ICECAVE7;
    if (used_movement_verb(WEST)) return R_ICECAVE9;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave7(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE6;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave8(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE9;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave9(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE6;
    if (used_movement_verb(SOUTH)) return R_ICECAVE8;
    if (used_movement_verb(NORTH)) return R_ICECAVE10;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave10(void)
{
    if (used_movement_verb(SOUTH)) return R_ICECAVE9;
    if (used_movement_verb(NW)) return R_ICECAVE11;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave11(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE10;
    if (used_movement_verb(WEST)) return R_ICECAVE12;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave12(void)
{
    if (used_movement_verb(NE)) return R_ICECAVE11;
    if (used_movement_verb(SOUTH)) return R_ICECAVE12A;
    if (used_movement_verb(WEST)) return R_ICECAVE15;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave12a(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE12;
    if (used_movement_verb(SOUTH)) return R_ICECAVE13;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave13(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE12A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave14(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE15A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave15(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE12;
    if (used_movement_verb(SOUTH)) return R_ICECAVE15A;
    if (used_movement_verb(NW)) return R_ICECAVE16;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave15a(void)
{
    if (used_movement_verb(SOUTH)) return R_ICECAVE14;
    if (used_movement_verb(NORTH)) return R_ICECAVE15;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave16(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE15;
    if (used_movement_verb(WEST)) return R_ICECAVE17;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave17(void)
{
    if (used_movement_verb(NE)) return R_ICECAVE16;
    if (used_movement_verb(SOUTH)) return R_ICECAVE18;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave18(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE17;
    if (used_movement_verb(SOUTH)) return R_ICECAVE19;
    if (used_movement_verb(WEST)) return R_ICECAVE21;
    if (used_movement_verb(NW)) return R_ICECAVE22;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave19(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE18;
    if (used_movement_verb(WEST)) return R_ICECAVE20;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave20(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE19;
    if (used_movement_verb(NORTH)) return R_ICECAVE21;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave21(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE18;
    if (used_movement_verb(SOUTH)) return R_ICECAVE20;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave22(void)
{
    if (used_movement_verb(SE)) return R_ICECAVE18;
    if (used_movement_verb(NW)) return R_ICECAVE23;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave23(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE22;
    if (used_movement_verb(WEST)) return R_ICECAVE24;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave24(void)
{
    if (used_movement_verb(NE)) return R_ICECAVE23;
    if (used_movement_verb(SOUTH)) return R_ICECAVE25;
    if (used_movement_verb(WEST)) return R_ICECAVE29;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave25(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE24;
    if (used_movement_verb(SOUTH)) return R_ICECAVE26;
    if (used_movement_verb(WEST)) return R_ICECAVE28;
    if (used_movement_verb(NW)) return R_ICECAVE28A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave26(void)
{
    if (used_movement_verb(NORTH)) return R_ICECAVE25;
    if (used_movement_verb(NW)) return R_ICECAVE27;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave27(void)
{
    if (used_movement_verb(SE)) return R_ICECAVE26;
    if (used_movement_verb(NORTH)) return R_ICECAVE28;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave28(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE25;
    if (used_movement_verb(SOUTH)) return R_ICECAVE27;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave28a(void)
{
    if (used_movement_verb(SE)) return R_ICECAVE25;
    if (used_movement_verb(NORTH)) return R_ICECAVE29;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave29(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE24;
    if (used_movement_verb(SOUTH)) return R_ICECAVE28A;
    if (used_movement_verb(NW)) return R_ICECAVE30;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave30(void)
{
    if (used_movement_verb(EAST)) return R_ICECAVE29;
    if (used_movement_verb(THURB)) {
        say_foof();
        return R_ICE;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_brink_3(void)
{
    if (used_movement_verb(NORTH)) return R_BRINK_1;
    if (used_movement_verb(NE)) return R_CRACK_1;
    if (used_movement_verb(CRACK)) return R_CRACK_1;
    if (keywordv(JUMP)) return plunge();
    return 0;  /* command hasn't been processed yet */
}

int at_crack_1(void)
{
    if (used_movement_verb(SW)) return R_BRINK_3;
    if (used_movement_verb(SE)) return R_CRACK_2;
    return 0;  /* command hasn't been processed yet */
}

int at_crack_2(void)
{
    if (used_movement_verb(WEST)) return R_CRACK_1;
    if (keywordv(SOUTH)) {
        if (there(SLIME, R_CRACK_2)) {
            puts("As you enter into the passage, you are forced to brush up against\n"
                 "some of the green slime.  Instantly it flows down and covers your\n"
                 "body, and rapidly digests away all of your flesh.");
            return you_are_dead_at(R_CRACK_2);
        } else {
            return R_CRACK_3;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_crack_3(void)
{
    if (used_movement_verb(NORTH)) return R_CRACK_2;
    if (used_movement_verb2(SOUTH, CRAWL)) return R_CRACK_4;
    return 0;  /* command hasn't been processed yet */
}

int at_crack_4(void)
{
    if (used_movement_verb3(NORTH, OUT, CRAWL)) return R_CRACK_3;
    return 0;  /* command hasn't been processed yet */
}

static int quicksand(Location dest)
{
    static bool have_drowned_in_quicksand = false;
    if (!objs[QUICKSAND].prop || toting(CLAM) || toting(OYSTER)) {
        objs[QUICKSAND].prop = 0;
        if (have_drowned_in_quicksand) {
            puts("You know, I've heard of people who really fell in for the soft sell,\n"
                 "but\n"
                 "     >glub<\n"
                 "            this\n"
                 "                  >glub<\n"
                 "                          is\n"
                 "                              >glub<\n"
                 "                                       ridiculous!\n\n"
                 "                                                          >blop!<");
        } else {
            have_drowned_in_quicksand = true;
            puts("Hmmmm..  This sand is rather soft, and you're sinking in a little...\n"
                 "In fact you're sinking in a lot!   Oh, no - it's QUICKSAND!!  HELP!!\n"
                 "HELP!! HELP!!!\n"
                 "               >glub<\n"
                 "                        >glub<\n"
                 "                                   >glub<\n"
                 "                                                 >blurp<");
        }
        return you_are_dead_at(R_YLEM);
    } else {
        objs[QUICKSAND].prop = 0;  /* it's only good for one crossing */
        return dest;
    }
}

int at_arch_cor_1(void)
{
    if (used_movement_verb(WEST)) return R_ARCHED;
    if (keywordv(EAST)) return quicksand(R_ARCH_COR_2);
    return 0;  /* command hasn't been processed yet */
}

int at_arch_cor_2(void)
{
    if (used_movement_verb(NORTH)) return R_ARCH_FORK;
    if (keywordv(WEST)) return quicksand(R_ARCH_COR_1);
    return 0;  /* command hasn't been processed yet */
}

int at_arch_fork(void)
{
    if (used_movement_verb(SOUTH)) return R_ARCH_COR_2;
    if (used_movement_verb(NORTH)) return R_FOURIER;
    if (used_movement_verb(EAST)) return R_JONAH;
    if (used_movement_placeword(R_JONAH)) return R_JONAH;
    if (used_movement_placeword(R_FOURIER)) return R_FOURIER;
    return 0;  /* command hasn't been processed yet */
}

int at_fourier(void)
{
    if (used_movement_verb(NW)) return R_ARCH_FORK;
    if (used_movement_verb(SW)) return R_SHELF;
    return 0;  /* command hasn't been processed yet */
}

int at_shelf(void)
{
    if (used_movement_verb(WEST)) return R_FOURIER;
    if (used_movement_verb(DOWN)) return R_BEACH;
    if (used_movement_obj(STEPS)) return R_BEACH;
    return 0;  /* command hasn't been processed yet */
}

int at_beach(void)
{
    if (used_movement_verb2(UP, WEST) || keywordp(R_SHELF) || keywordo(STEPS)) {
        objs[DINGHY].prop = 1;
        return R_SHELF;
    }
    if (keywordo(WATER) || keywordv(FILL) || keywordv(DRINK)) {
        puts("I'm afraid that all that's available here is salt water, which\n"
             /* Platt has "you'de". */
             "isn't good for anything much... you'd better try elsewhere.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_jonah(void)
{
    if (used_movement_verb(SOUTH)) return R_IN_JONAH;
    if (used_movement_verb(WEST)) return R_ARCH_FORK;
    return 0;  /* command hasn't been processed yet */
}

int at_in_jonah(void)
{
    if (used_movement_verb2(NORTH, OUT)) return R_JONAH;
    return 0;  /* command hasn't been processed yet */
}

int at_faces(void)
{
    if (used_movement_verb(NORTH)) return R_BY_FIGURE;
    if (keywordv(CROSS) || keywordv(SOUTH) || keywordo(GORGE)) {
        if (!objs[GORGE].prop) {
            puts("I'm afraid I can't go that way - walking on red-hot lava is contrary\n"
                 "to union regulations (and is bad for your health anyhow).");
            return STAY_STILL;
        } else if (!toting(RING)) {
            puts("As you approach the center of the archway, hot vapors saturated with\n"
                 "brimstone drift up from the lava in the gorge beneath your feet.  You\n"
                 "are swiftly overcome by the foul gasses and, with your lungs burned\n"
                 "out, fall off of the bridge and into the gorge.");
            return you_are_dead_at(R_YLEM);
        } else if (toting(SCEPTRE)) {
            puts("As you reach the center of the bridge, a ghostly figure appears in\n"
                 "front of you.  He (?) stands at least eight feet tall, and has the\n"
                 "lower body of an enormous snake, six arms, and an angry expression on\n"
                 "his face.  \"You'll not have my sceptre that easily!\" he cries, and\n"
                 "makes a complex magical gesture with his lower right arm.  There is a\n"
                 "brilliant flash of light and a vicious >crack<, and the bridge cracks\n"
                 "and plummets into the gorge.");
            objs[GORGE].prop = 0;
            return you_are_dead_at(R_YLEM);
        } else {
            return R_VIEW;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_by_figure(void)
{
    if (objs[STATUE].prop) {
        /* The statue has moved, revealing dark passages. */
       if (used_movement_verb(NW)) return R_PLAIN_1;
       if (used_movement_verb(NORTH)) return R_BASQUE_1;
       if (used_movement_verb(NE)) return R_BANSHEE;
    }
    if (used_movement_verb(SOUTH)) return R_FACES;
    return 0;  /* command hasn't been processed yet */
}

int at_plain_1(void)
{
    if (used_movement_verb(SOUTH)) return R_BY_FIGURE;
    if (keywordv(NORTH)) {
        objs[FOG].prop = 0;
        apport(FOG, R_PLAIN_2);
        return R_PLAIN_2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_plain_2(void)
{
    if (keywordv(NORTH) || keywordv(EAST) || keywordv(SOUTH) || keywordv(WEST) ||
        keywordv(NE) || keywordv(NW) || keywordv(SE) || keywordv(SW)) {
        /* If this turkey left anything here in the fog,
         * he's going to lose it forever. */
        for (int i=0; i <= MAX_OBJ; ++i) {
            if (there(i, R_PLAIN_2) && portable(i)) {
                apport(i, R_YLEM);
            }
        }
        if (there(GLOW, R_PLAIN_2)) {
            const int correct = objs[GLOW].prop + EAST;
            const int m1 = word1.meaning;
            const int m2 = word2.meaning;
            if ((m1 == correct || m2 == correct)) {
                objs[FOG].prop = 8;
                return R_PLAIN_3;
            }
        }
        return R_PLAIN_2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_plain_3(void)
{
    if (keywordv(DOWN)) {
        apport(FOG, R_PLAIN_1);
        return R_NONDESCRIPT;
    }
    if (keywordv(NORTH) || keywordv(EAST) || keywordv(SOUTH) || keywordv(WEST) ||
        keywordv(NE) || keywordv(NW) || keywordv(SE) || keywordv(SW)) {
        objs[FOG].prop = 0;  /* so it starts changing again */
        return R_PLAIN_2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_nondescript(void)
{
    if (used_movement_verb(NORTH)) return R_PENTAGRAM;
    if (used_movement_placeword(R_PENTAGRAM)) return R_PENTAGRAM;
    if (used_movement_verb2(UP, SOUTH)) {
        apport(FOG, R_PLAIN_2);
        return R_PLAIN_3;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_pentagram(void)
{
    if (keywordo(FLASK) && (keywordv(DROP) || keywordv(THROW))) {
        if (toting(FLASK) && objs[FLASK].prop == 1) {
            apport(FLASK, R_PENTAGRAM);
            puts("You have set the flask down in the center of the pentagram.");
            objs[FLASK].prop = 0;
            return STAY_STILL;
        }
    }
    if (used_movement_verb2(WEST, OUT)) return R_NONDESCRIPT;
    if (used_movement_placeword(R_NONDESCRIPT)) return R_NONDESCRIPT;
    if (used_movement_verb2(NORTH, CRACK)) return R_CHIMNEY;
    if (used_movement_placeword(R_CHIMNEY)) return R_CHIMNEY;
    return 0;  /* command hasn't been processed yet */
}

int at_chimney(void)
{
    if (used_movement_verb2(UP, CLIMB)) return R_TUBE;
    if (used_movement_placeword(R_TUBE)) return R_TUBE;
    if (used_movement_verb(SOUTH)) return R_PENTAGRAM;
    if (used_movement_placeword(R_PENTAGRAM)) return R_PENTAGRAM;
    if (used_movement_verb(CRACK)) return R_PENTAGRAM;
    return 0;  /* command hasn't been processed yet */
}

int at_tube(void)
{
    if (used_movement_verb(DOWN)) return R_CHIMNEY;
    if (used_movement_placeword(R_CHIMNEY)) return R_CHIMNEY;
    if (used_movement_verb(CLIMB)) return R_CHIMNEY;
    if (used_movement_placeword(R_TUBE)) return R_TUBE_SLIDE;
    if (used_movement_placeword(R_SLIDE)) return R_TUBE_SLIDE;
    if (used_movement_verb(SOUTH)) return R_TUBE_SLIDE;
    return 0;  /* command hasn't been processed yet */
}

int at_tube_slide(void)
{
    if (used_movement_verb2(SOUTH, DOWN)) return oof(R_PLAIN_1);
    if (used_movement_placeword(R_SLIDE)) return oof(R_PLAIN_1);
    if (used_movement_verb(NORTH)) return R_TUBE;
    if (used_movement_placeword(R_CHIMNEY)) return R_CHIMNEY;
    if (used_movement_placeword(R_TUBE)) return R_CHIMNEY;
    return 0;  /* command hasn't been processed yet */
}

int at_basque_1(void)
{
    if (used_movement_verb(SOUTH)) return R_BY_FIGURE;
    if (keywordv(NORTH)) {
        objs[BASILISK].prop += 1;
        if (objs[BASILISK].prop == 1) {
            puts("The basilisk stirs restlessly and grumbles in its sleep as you pass,\n"
                 "but it does not awaken.\n");
        }
        return R_BASQUE_2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_basque_2(void)
{
    if (used_movement_verb(NORTH)) return R_BASQUE_FORK;
    if (keywordv(SOUTH)) {
        objs[BASILISK].prop -= 1;
        if (objs[BASILISK].prop == 0) {
            if (toting(PLATE)) {
                puts("The basilisk stirs grumpily and awakens, peering sleepily about.  It\n"
                     "sees its reflection in the metal plate that you are carrying,\n"
                     "shudders, and turns into solid granite.\n");
                objs[BASILISK].prop = 2;
            } else {
                puts("The basilisk stirs grumpily and awakens, peering sleepily about.  It\n"
                     "spies you, growls, and stares you straight in the eye.  Your body\n"
                     "is instantly petrified.");
                return you_are_dead_at(R_BASQUE_2);
            }
        }
        return R_BASQUE_1;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_basque_fork(void)
{
    if (used_movement_verb(NORTH)) return R_PEELGRUNT;
    if (used_movement_placeword(R_PEELGRUNT)) return R_PEELGRUNT;
    if (used_movement_verb(SOUTH)) return R_BASQUE_2;
    if (used_movement_verb(DOWN)) return R_ON_STEPS;
    if (used_movement_obj(STEPS)) return R_ON_STEPS;
    return 0;  /* command hasn't been processed yet */
}

int at_peelgrunt(void)
{
    if (keywordv(SOUTH) || keywordv(OUT) || keywordp(R_FORK)) {
        if (objs[SAFE].prop == 1) {
            puts("The safe's door is blocking the exit passage - you'll have to close\n"
                 "the safe to get out of here.");
            return STAY_STILL;
        } else {
            return R_BASQUE_FORK;
        }
    }
    if (keywordv(IN)) return enter_safe(R_PEELGRUNT);
    return 0;  /* command hasn't been processed yet */
}

int at_on_steps(void)
{
    if (used_movement_verb(UP)) return R_BASQUE_FORK;
    if (used_movement_verb(DOWN)) return R_STEPS_EXIT;
    if (used_movement_obj(STEPS)) return R_STEPS_EXIT;
    return 0;  /* command hasn't been processed yet */
}

int at_steps_exit(void)
{
    if (used_movement_verb(UP)) return R_ON_STEPS;
    if (used_movement_verb(DOWN)) return R_STORAGE;
    if (used_movement_verb(NORTH)) return R_FAKE_Y2;
    if (used_movement_obj(STEPS)) return R_STORAGE;
    if (used_movement_verb(OUT)) return R_FAKE_Y2;
    return 0;  /* command hasn't been processed yet */
}

int at_storage(void)
{
    if (used_movement_verb(UP)) return R_STEPS_EXIT;
    if (used_movement_obj(STEPS)) return R_STEPS_EXIT;
    return 0;  /* command hasn't been processed yet */
}

int at_fake_y2(void)
{
    if (used_movement_verb(SOUTH)) return R_STEPS_EXIT;
    if (used_movement_verb(WEST)) return R_CATACOMBS1;
    if (used_movement_verb(EAST)) return R_FAKE_JUMBLE;
    if (keywordv(PLUGH) || keywordp(R_PLOVER)) {
        if (nomagic) {
            nothing_happens();
            return STAY_STILL;
        } else {
            say_foof();
            return R_PLATFORM;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_fake_jumble(void)
{
    if (used_movement_verb2(DOWN, WEST)) return R_FAKE_Y2;
    if (used_movement_verb(UP)) return R_CATACOMBS1;
    return 0;  /* command hasn't been processed yet */
}

int at_audience(void)
{
    if (used_movement_verb(EAST)) return R_AUDIENCE_E;
    if (used_movement_verb(WEST)) return R_CATACOMBS11;
    return 0;  /* command hasn't been processed yet */
}

int at_audience_e(void)
{
    if (used_movement_verb(WEST)) return R_AUDIENCE;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs1(void)
{
    if (used_movement_verb(SOUTH)) return R_CATACOMBS2;
    if (used_movement_verb3(NORTH,NW,WEST)) return R_CATACOMBS1;
    if (used_movement_verb3(SW,SE,DOWN)) return R_CATACOMBS1;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS1;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs2(void)
{
    if (used_movement_verb(SW)) return R_CATACOMBS3;
    if (used_movement_verb3(NORTH,NW,WEST)) return R_CATACOMBS1;
    if (used_movement_verb3(SOUTH,SE,DOWN)) return R_CATACOMBS1;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS1;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs3(void)
{
    if (used_movement_verb(NW)) return R_CATACOMBS4;
    if (used_movement_verb3(NORTH,SW,WEST)) return R_CATACOMBS2;
    if (used_movement_verb3(SOUTH,SE,DOWN)) return R_CATACOMBS2;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS2;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs4(void)
{
    if (used_movement_verb(SOUTH)) return R_CATACOMBS5;
    if (used_movement_verb3(NORTH,SW,WEST)) return R_CATACOMBS3;
    if (used_movement_verb3(NW,SE,DOWN)) return R_CATACOMBS3;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS3;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs5(void)
{
    if (used_movement_verb(DOWN)) return R_CATACOMBS6;
    if (used_movement_verb3(NORTH,NW,WEST)) return R_CATACOMBS4;
    if (used_movement_verb3(SW,SE,SOUTH)) return R_CATACOMBS4;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS4;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs6(void)
{
    if (used_movement_verb(WEST)) return R_CATACOMBS7;
    if (used_movement_verb3(NORTH,NW,DOWN)) return R_CATACOMBS5;
    if (used_movement_verb3(SW,SE,SOUTH)) return R_CATACOMBS5;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS5;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs7(void)
{
    if (used_movement_verb(NW)) return R_CATACOMBS8;
    if (used_movement_verb3(NORTH,WEST,DOWN)) return R_CATACOMBS6;
    if (used_movement_verb3(SW,SE,SOUTH)) return R_CATACOMBS6;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS6;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs8(void)
{
    if (used_movement_verb(NORTH)) return R_CATACOMBS9;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS7;
    if (used_movement_verb3(SW,SE,SOUTH)) return R_CATACOMBS7;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS7;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs9(void)
{
    if (used_movement_verb(SOUTH)) return R_CATACOMBS10;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS8;
    if (used_movement_verb3(SW,SE,NORTH)) return R_CATACOMBS8;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS8;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs10(void)
{
    if (used_movement_verb(NORTH)) return R_CATACOMBS11;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS9;
    if (used_movement_verb3(SW,SE,SOUTH)) return R_CATACOMBS9;
    if (used_movement_verb3(EAST,NE,UP)) return R_CATACOMBS9;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs11(void)
{
    if (used_movement_verb(SW)) return R_CATACOMBS12;
    if (used_movement_verb(EAST)) return R_AUDIENCE;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS10;
    if (used_movement_verb3(NORTH,SE,SOUTH)) return R_CATACOMBS10;
    if (used_movement_verb2(NE,UP)) return R_CATACOMBS10;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs12(void)
{
    if (used_movement_verb(EAST)) return R_CATACOMBS13;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS11;
    if (used_movement_verb3(NORTH,SE,SOUTH)) return R_CATACOMBS11;
    if (used_movement_verb3(NE,UP,SW)) return R_CATACOMBS11;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs13(void)
{
    if (used_movement_verb(SE)) return R_CATACOMBS14;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS12;
    if (used_movement_verb3(NORTH,EAST,SOUTH)) return R_CATACOMBS12;
    if (used_movement_verb3(NE,UP,SW)) return R_CATACOMBS12;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs14(void)
{
    if (used_movement_verb(NE)) return R_CATACOMBS15;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS13;
    if (used_movement_verb3(NORTH,EAST,SOUTH)) return R_CATACOMBS13;
    if (used_movement_verb3(SE,UP,SW)) return R_CATACOMBS13;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs15(void)
{
    if (used_movement_verb(EAST)) return R_CATACOMBS16;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS14;
    if (used_movement_verb3(NORTH,NE,SOUTH)) return R_CATACOMBS14;
    if (used_movement_verb3(SE,UP,SW)) return R_CATACOMBS14;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs16(void)
{
    if (used_movement_verb(SE)) return R_CATACOMBS17;
    if (used_movement_verb3(NW,WEST,DOWN)) return R_CATACOMBS15;
    if (used_movement_verb3(NORTH,NE,SOUTH)) return R_CATACOMBS15;
    if (used_movement_verb3(EAST,UP,SW)) return R_CATACOMBS15;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs17(void)
{
    if (used_movement_verb(DOWN)) return R_CATACOMBS18;
    if (used_movement_verb3(NW,WEST,SE)) return R_CATACOMBS16;
    if (used_movement_verb3(NORTH,NE,SOUTH)) return R_CATACOMBS16;
    if (used_movement_verb3(EAST,UP,SW)) return R_CATACOMBS16;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs18(void)
{
    if (used_movement_verb(SOUTH)) return R_CATACOMBS19;
    if (used_movement_verb3(NW,WEST,SE)) return R_CATACOMBS17;
    if (used_movement_verb3(NORTH,NE,DOWN)) return R_CATACOMBS17;
    if (used_movement_verb3(EAST,UP,SW)) return R_CATACOMBS17;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs19(void)
{
    if (used_movement_verb(NORTH)) return R_FAKE_Y2;
    if (used_movement_verb3(NW,WEST,SE)) return R_CATACOMBS18;
    if (used_movement_verb3(SOUTH,NE,DOWN)) return R_CATACOMBS18;
    if (used_movement_verb3(EAST,UP,SW)) return R_CATACOMBS18;
    return 0;  /* command hasn't been processed yet */
}

int at_banshee(void)
{
    if (used_movement_verb(NW)) return R_BY_FIGURE;
    if (used_movement_verb(NORTH)) return R_GOLDEN;
    if (used_movement_placeword(R_GOLDEN)) return R_GOLDEN;
    return 0;  /* command hasn't been processed yet */
}

int at_golden(void)
{
    if (used_movement_verb(NE)) return R_ARABESQUE;
    if (used_movement_verb(NW)) return R_TRANSLUCENT;
    if (used_movement_placeword(R_ARABESQUE)) return R_ARABESQUE;
    if (used_movement_placeword(R_TRANSLUCENT)) return R_TRANSLUCENT;
    if (used_movement_verb(SOUTH)) return R_BANSHEE;
    return 0;  /* command hasn't been processed yet */
}

int at_arabesque(void)
{
    if (used_movement_verb2(SOUTH, OUT)) return R_GOLDEN;
    if (used_movement_placeword(R_GOLDEN)) return R_GOLDEN;
    return 0;  /* command hasn't been processed yet */
}

int at_translucent(void)
{
    if (used_movement_verb2(EAST, OUT) || keywordp(R_GOLDEN)) {
        static bool have_awoken_goblins = false;
        if (!have_awoken_goblins) {
            have_awoken_goblins = true;
            apport(GOBLINS, R_TRANSLUCENT);
            objs[GOBLINS].prop = -1;
        }
        return R_GOLDEN;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_platform(void)
{
    if (used_movement_verb(PLUGH) || used_movement_placeword(R_PLOVER)) {
        say_foof();
        return R_FAKE_Y2;
    }
    if (used_movement_verb3(DOWN,CLIMB,NORTH) ||
        used_movement_verb3(SOUTH,EAST,WEST) ||
        used_movement_verb3(NE,NW,SE) || used_movement_verb(NW) ||
        keywordv(JUMP)) {
        puts("EEEEEEEEEAAAAAAAAAAAaaaaaahhhhhhhhhhhhh..........\n\n"
        "                                                      >sizzle<");
        return you_are_dead_at(R_YLEM);
    }
    return 0;  /* command hasn't been processed yet */
}

static bool correctly_used_magic_word(VerbWord v)
{
    return (keywordv(v) && (keywordv(SAY) || word2.type == WordType_None));
}

static bool try_escaping_with_word(VerbWord word, int i)
{
    if (correctly_used_magic_word(word)) {
        if (cylinder_escape_count == i && word == BLERBI) {
            say_foof();
            closure = 4;  /* On to the next stage of the endgame! */
        } else if (cylinder_escape_count == i) {
            ok();
            ++cylinder_escape_count;
        } else {
            nothing_happens();
            cylinder_escape_count = 0;
        }
        return true;
    }
    return false;
}

int at_cylindrical(void)
{
    if (try_escaping_with_word(ZORTON,0) ||
        try_escaping_with_word(XYZZY,1) ||
        try_escaping_with_word(THURB,2) ||
        try_escaping_with_word(SNOEZE,3) ||
        try_escaping_with_word(SAMOHT,4) ||
        try_escaping_with_word(PLUGH,5) ||
        try_escaping_with_word(PHUGGG,6) ||
        try_escaping_with_word(NOSIDE,7) ||
        try_escaping_with_word(MELENKURION,8) ||
        try_escaping_with_word(KNERL,9) ||
        try_escaping_with_word(KLAETU,10) ||
        try_escaping_with_word(FOO,11) ||
        try_escaping_with_word(FOE,12) ||
        try_escaping_with_word(FIE,13) ||
        try_escaping_with_word(FEE,14))
        return STAY_STILL;

    if (try_escaping_with_word(BLERBI,15)) {
        if (closure == 4) return R_ROAD;
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

static const char all_alike[] = "You are in a maze of twisty little passages, all alike.";
static const char dead_end[] = "Dead end.";
static const char ice_tunnels[] = "You are in an intricate network of ice tunnels.";
static const char enchanted_tunnels[] = "You are in the catacombs.  Enchanted tunnels lead in all directions.";

/* R_INSAFE and R_BALCONY are both marked F_NODWARF|F_ONE_EXIT, which is
 * somewhat silly, as F_ONE_EXIT has no effect without a dwarf in the room. */

struct Place places[] = {
    /* R_LIMBO */ {
        "You are in Limbo.",
        "You are in Limbo (program bug!).  Say OUT to get out of here.",
        0, &at_limbo
    },
    /* R_ROAD */ {
        "You're at end of road again.",
        "You are standing at the end of a road before a small brick building.\n"
        "Around you is a forest.  A small stream flows out of the building and\n"
        "down a gully.",
        F_LIGHTED|F_WATER|F_NOTINCAVE|F_NODWARF, &at_road
    },
    /* R_HILL */ {
        "You're at hill in road.",
        "You have walked up a hill, still in the forest. The road slopes back\n"
        "down the other side of the hill.  There is a building in the distance.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF, &at_hill
    },
    /* R_HOUSE */ {
        "You're inside building.",
        "You are inside a building, a well house for a large spring.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF|F_WATER, &at_house
    },
    /* R_VALLEY */ {
        "You're in valley.",
        "You are in a valley in the forest beside a stream tumbling along a\n"
        "rocky bed.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF|F_WATER, &at_valley
    },
    /* R_FOREST */ {
        "You're in forest.",
        "You are in open forest, with a deep valley to one side.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF, &at_forest
    },
    /* R_FOREST2 */ {
        "You're in forest.",
        "You are in open forest near both a valley and a road.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF, &at_forest2
    },
    /* R_SLIT */ {
        "You're at slit in streambed.",
        "At your feet all the water of the stream splashes into a 2-inch slit\n"
        "in the rock.  Downstream the streambed is bare rock.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF|F_WATER, &at_slit
    },
    /* R_DEPRESSION */ {
        "You're outside grate.",
        "You are in a 20-foot depression floored with bare dirt. Set into the\n"
        "dirt is a strong steel grate mounted in concrete.  A dry streambed\n"
        "leads into the depression.",
        F_LIGHTED|F_HINTABLE|F_NOTINCAVE|F_NODWARF, &at_depression
    },
    /* R_INSIDE */ {
        "You're below the grate.",
        "You are in a small chamber beneath a 3x3 steel grate to the surface.\n"
        "A low crawl over cobbles leads inward to the west.",
        F_LIGHTED|F_NODWARF, &at_inside
    },
    /* R_COBBLES */ {
        "You're in cobble crawl.",
        "You are crawling over cobbles in a low passage. There is a dim light\n"
        "at the east end of the passage.",
        F_LIGHTED|F_NODWARF, &at_cobbles
    },
    /* R_DEBRIS */ {
        "You're in debris room.",
        "You are in a debris room filled with stuff washed in from the surface.\n"
        "A low wide passage with cobbles becomes plugged with mud and debris\n"
        "here, but an awkward canyon leads upward and west.  A note on the wall\n"
        "says \"Magic word XYZZY\".",
        0, &at_debris
    },
    /* R_AWK */ {
        NULL,
        "You are in an awkward sloping east/west canyon.",
        0, &at_awk
    },
    /* R_BIRD */ {
        "You're in bird chamber.",
        "You are in a splendid chamber thirty feet high. The walls are frozen\n"
        "rivers of orange stone.  An awkward canyon and a good passage exit\n"
        "from east and west sides of the chamber.",
        F_HINTABLE, &at_bird
    },
    /* R_SPIT */ {
        "You're at top of small pit.",
        "At your feet is a small pit breathing traces of white mist. An east\n"
        "passage ends here except for a small crack leading on.  Rough stone\n"
        "steps lead down the pit.",
        0, &at_spit
    },
    /* R_EMIST */ {
        "You're in Hall of Mists.",
        "You are at one end of a vast hall stretching forward out of sight to\n"
        "the west, filled with wisps of white mist that sway to and fro\n"
        "almost as if alive.  Rough stone steps lead up to a passage at the\n"
        "top of a dome above you.  A wide staircase runs downward into\n"
        "the darkness;  a chill wind blows up from below.  There are small\n"
        "passages to the north and south, and a small crack leads east.",
        0, &at_emist
    },
    /* R_EFISS */ {
        "You're on east bank of fissure.",
        "You are on the east bank of a fissure slicing clear across the hall.\n"
        "The mist is quite thick here, and the fissure is too wide to jump.",
        0, &at_efiss
    },
    /* R_WFISS */ {
        NULL,
        "You are on the west side of the fissure in the Hall of Mists.",
        0, &at_wfiss
    },
    /* R_NUGGET */ {
        "You're in nugget of gold room.",
        "This is a low room with a crude note on the wall. The note says,\n"
        "\"You won't get it up the steps\".",
        F_ONE_EXIT, &at_nugget
    },
    /* R_HMK */ {
        "You're in Hall of Mt King.",
        "You are in the Hall of the Mountain King, with passages off in all\n"
        "directions.",
        F_HINTABLE, &at_hmk
    },
    /* R_W2PIT */ {
        "You're at west end of Twopit room.",
        "You are at the west end of the Twopit room. There is a large hole in\n"
        "the wall above the pit at this end of the room.",
        0, &at_w2pit
    },
    /* R_E2PIT */ {
        "You're at east end of Twopit room.",
        "You are at the east end of the Twopit room. The floor here is\n"
        "littered with thin rock slabs, which make it easy to descend the pits.\n"
        "There is a path here bypassing the pits to connect passages from east\n"
        "and west.  There are holes all over, but the only big one is on the\n"
        "wall directly over the west pit where you can't get to it.",
        0, &at_e2pit
    },
    /* R_EPIT */ {
        "You're in east pit.\nThere is a small pool of oil here.",
        "You are at the bottom of the eastern pit in the Twopit room. There is\n"
        "a small pool of oil in one corner of the pit.",
        F_NODWARF, &at_epit
    },
    /* R_WPIT */ {
        "You're in west pit.",
        "You are at the bottom of the western pit in the Twopit room. There is\n"
        "a large hole in the wall about 25 feet above you.",
        F_NODWARF, &at_wpit
    },
    /* R_NS */ {
        "You're in a low N/S passage.",
        "You are in a low N/S passage at a hole in the floor. The hole goes\n"
        "down to an E/W passage.",
        0, &at_ns
    },
    /* R_SOUTH */ {
        NULL,
        "You are in the south side chamber.",
        F_ONE_EXIT, &at_south
    },
    /* R_WEST */ {
        "You're in the west side chamber.",
        "You are in the west side chamber of the Hall of the Mountain King.\n"
        "A passage continues west and up here.",
        0, &at_west
    },
    /* R_Y2 */ {
        "You're at \"Y2\".",
        "You are in a large room, with a passage to the south, a passage to the\n"
        "west, and a wall of broken rock to the east.  There is a large \"Y2\" on\n"
        "a rock in the room's center.",
        0, &at_y2
    },
    /* R_JUMBLE */ {
        NULL,
        "You are in a jumble of rock, with cracks everywhere.",
        0, &at_jumble
    },
    /* R_WINDOE */ {
        "You're at window on pit.",
        "You're at a low window overlooking a huge pit, which extends up out of\n"
        "sight.  A floor is indistinctly visible over 50 feet below.  Traces of\n"
        "white mist cover the floor of the pit, becoming thicker to the right.\n"
        "Marks in the dust around the window would seem to indicate that\n"
        "someone has been here recently.  Directly across the pit from you and\n"
        "25 feet away there is a similar window looking into a lighted room.  A\n"
        "shadowy figure can be seen there peering back at you.",
        F_ONE_EXIT, &at_windoe
    },
    /* R_WINDOW */ {
        "You're at window on pit.",
        "You're at a low window overlooking a huge pit, which extends up out of\n"
        "sight.  A floor is indistinctly visible over 50 feet below.  Traces of\n"
        "white mist cover the floor of the pit, becoming thicker to the left.\n"
        "Marks in the dust around the window would seem to indicate that\n"
        "someone has been here recently.  Directly across the pit from you and\n"
        "25 feet away there is a similar window looking into a lighted room.  A\n"
        "shadowy figure can be seen there peering back at you.",
        F_ONE_EXIT, &at_window
    },
    /* R_DIRTY */ {
        "You're in dirty passage.",
        "You are in a dirty broken passage. To the east is a crawl. To the\n"
        "west is a large passage.  Above you is a hole to another passage.",
        0, &at_dirty
    },
    /* R_CLEAN */ {
        "You are on the brink of a small clean climbable pit.",
        "You are on the brink of a small clean climbable pit. A crawl leads\n"
        "west.",
        0, &at_clean
    },
    /* R_WET */ {
        "You are at the pit's bottom with a stream nearby.",
        "You are in the bottom of a small pit with a little stream, which\n"
        "enters and exits through tiny slits.",
        F_WATER|F_NODWARF, &at_wet
    },
    /* R_DUSTY */ {
        "You're in dusty rock room.",
        "You are in a large room full of dusty rocks. There is a big hole in\n"
        "the floor.  There are cracks everywhere, and a passage leading east.",
        0, &at_dusty
    },
    /* R_WMIST */ {
        "You're at west end of Hall of Mists.",
        "You are at the west end of Hall of Mists. A low wide crawl continues\n"
        "west and another goes north.  To the south is a little passage 6 feet\n"
        "off the floor.",
        0, &at_wmist
    },
    /* R_MAZEA42 */ { NULL, all_alike, F_HINTABLE|F_INMAZE, &at_mazea42 },
    /* R_MAZEA43 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea43 },
    /* R_MAZEA44 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea44 },
    /* R_MAZEA45 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea45 },
    /* R_MAZEA46 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea46 },
    /* R_MAZEA47 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea47 },
    /* R_MAZEA48 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea48 },
    /* Platt marks MAZEA.49 as ONE.EXIT, even though there are two. */
    /* R_MAZEA49 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea49 },
    /* R_MAZEA50 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea50 },
    /* R_MAZEA51 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea51 },
    /* R_MAZEA52 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea52 },
    /* R_MAZEA53 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea53 },
    /* R_MAZEA54 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea54 },
    /* R_MAZEA55 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea55 },
    /* R_MAZEA56 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea56 },
    /* R_MAZEA57_PIT */ {
        "You're at brink of pit.",
        "You are on the brink of a thirty foot pit with a massive orange column\n"
        "down one wall.  You could climb down here but you could not get back\n"
        "up.  The maze continues at this level.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea57_pit
    },
    /* R_MAZEA58 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea58 },
    /* R_ELONG */ {
        "You're at east end of long hall.",
        "You are at the east end of a very long hall apparently without side\n"
        "chambers.  To the east a low wide crawl slants up.  To the north a\n"
        "round two foot hole slants down.",
        0, &at_elong
    },
    /* R_WLONG */ {
        "You're at west end of long hall.",
        "You are at the west end of a very long featureless hall. The hall\n"
        "joins up with a narrow north/south passage.",
        0, &at_wlong
    },
    /* R_CROSS */ {
        NULL,
        "You are at a crossover of a high N/S passage and a low E/W one.",
        0, &at_cross
    },
    /* R_DEADEND1 */ {
        NULL,
        "Dead end passage. Scratched on a rock is the message, \"Stand where\n"
        "the statue gazes, and make use of the proper tool.\"",
        F_ONE_EXIT, &at_deadend1
    },
    /* R_COMPLEX */ {
        "You're at complex junction.",
        "You are at a complex junction. A low hands and knees passage from the\n"
        "north joins a higher crawl from the east to make a walking passage\n"
        "going west.  There is also a large room above.  The air is damp here.",
        0, &at_complex
    },
    /* R_BEDQUILT */ {
        "You're back at Bedquilt.",
        "You are in Bedquilt, a long east/west passage with holes everywhere.\n"
        "To explore at random select NORTH, SOUTH, UP, or DOWN.",
        0, &at_bedquilt
    },
    /* R_SWISS */ {
        "You're in Swiss cheese room.",
        "You are in a room whose walls resemble Swiss cheese. Obvious passages\n"
        "go west, east, NE, and NW.  Part of the room is occupied by a large\n"
        "bedrock block.",
        0, &at_swiss
    },
    /* R_SLAB */ {
        "You're in Slab room.",
        "You are in a large low circular chamber whose floor is an immense slab\n"
        "fallen from the ceiling (Slab room).  East and west there once were\n"
        "large passages, but they are now filled with boulders.  Low small\n"
        "passages go north and south, and the south one quickly bends west\n"
        "around the boulders.",
        0, &at_slab
    },
    /* R_ABOVER */ {
        NULL,
        "You are in a secret N/S canyon above a large room.",
        0, &at_abover
    },
    /* R_ABOVEP */ {
        NULL,
        "You are in a secret N/S canyon above a sizable passage.",
        0, &at_abovep
    },
    /* R_SJUNC */ {
        "You're at junction of three secret canyons.",
        "You are in a secret canyon at a junction of three canyons, bearing\n"
        "north, south, and SE.  The north one is as tall as the other two\n"
        "combined.",
        0, &at_sjunc
    },
    /* R_LOW */ {
        NULL,
        "You are in a large low room.  Crawls lead north, SE, and SW.",
        0, &at_low
    },
    /* R_DEADEND2 */ {
        NULL, "Dead end crawl.", F_ONE_EXIT, &at_deadend2
    },
    /* R_SECRETEW_TITE */ {
        "You're in secret E/W canyon above tight canyon.",
        "You are in a secret canyon which here runs E/W. It crosses over a\n"
        "very tight canyon 15 feet below.  If you go down you may not be able\n"
        "to get back up.",
        0, &at_secretew_tite
    },
    /* R_NSCANYONWIDE */ {
        NULL,
        "You are at a wide place in a very tight N/S canyon.",
        F_NOBACK, &at_nscanyonwide
    },
    /* R_TIGHTERSTILL */ {
        NULL,
        "The canyon here becomes too tight to go further south.",
        0, &at_tighterstill
    },
    /* R_TALLEWCNYN */ {
        "You are in a tall E/W canyon.",
        "You are in a tall E/W canyon. A low tight crawl goes 3 feet north and\n"
        "seems to open up.",
        0, &at_tallewcnyn
    },
    /* R_DEADEND3 */ {
        "The canyon runs into a mass of boulders -- dead end.",
        "The canyon runs into a mass of boulders -- dead end.  Scratched on\n"
        "one of the boulders are the words, \"Jerry Cornelius was here.\"",
        F_ONE_EXIT, &at_deadend3
    },
    /* R_MAZEA80 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea80 },
    /* R_MAZEA81 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea81 },
    /* R_MAZEA82 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea82 },
    /* R_MAZEA83 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea83 },
    /* R_MAZEA84 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea84 },
    /* R_MAZEA85 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea85 },
    /* R_MAZEA86 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea86 },
    /* R_MAZEA87 */ { NULL, all_alike, F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea87 },
    /* R_NARROW */ {
        "You're in narrow corridor.",
        "You are in a long, narrow corridor stretching out of sight to the\n"
        "west.  At the eastern end is a hole through which you can see a\n"
        "profusion of leaves.",
        0, &at_narrow
    },
    /* R_INCLINE */ {
        "You're at steep incline above large room.",
        "You are at the top of a steep incline above a large room. You could\n"
        "climb down here, but you would not be able to climb up.  There is a\n"
        "passage leading back to the north.",
        F_NOBACK, &at_incline
    },
    /* R_GIANT */ {
        "You're in Giant room.",
        "You are in the Giant room. The ceiling here is too high up for your\n"
        "lamp to show it.  Cavernous passages lead east, north, and south.  On\n"
        "the west wall is scrawled the inscription, \"Fee Fie Foe Foo\" {sic}.",
        /* Platt only uses curly braces because square brackets mean something
         * special to the A-code interpreter. */
        0, &at_giant
    },
    /* R_IMMENSENSPASS */ {
        NULL,
        "You are at one end of an immense north/south passage.",
        0, &at_immensenspass
    },
    /* R_FALLS */ {
        "You're in cavern with waterfall.",
        "You are in a magnificent cavern with a rushing stream, which cascades\n"
        "over a sparkling waterfall into a roaring whirlpool which disappears\n"
        "through a hole in the floor.  Passages exit to the south and west.",
        F_WATER, &at_falls
    },
    /* R_SOFT */ {
        "You're in soft room.",
        "You are in the soft room. The walls are covered with heavy curtains,\n"
        "the floor with a thick pile carpet.  Moss covers the ceiling.",
        F_ONE_EXIT, &at_soft
    },
    /* R_ORIENTAL */ {
        "You're in Oriental room.",
        "This is the Oriental room. Ancient oriental cave drawings cover the\n"
        "walls.  A gently sloping passage leads upward to the north, another\n"
        "passage leads SE, and a hands and knees crawl leads west.",
        0, &at_oriental
    },
    /* R_MISTY */ {
        "You're in misty cavern.",
        "You are following a wide path around the outer edge of a large cavern.\n"
        "Far below, through a heavy white mist, strange splashing noises can be\n"
        "heard.  The mist rises up through a fissure in the ceiling.  The path\n"
        "exits to the south and west.",
        0, &at_misty
    },
    /* R_ALCOVE */ {
        "You're in alcove.",
        "You are in an alcove. A small NW path seems to widen after a short\n"
        "distance.  An extremely tight tunnel leads east.  It looks like a very\n"
        "tight squeeze.  An eerie light can be seen at the other end.",
        F_NOBACK|F_NODWARF|F_HINTABLE, &at_alcove
    },
    /* R_PLOVER */ {
        "You're in Plover room.",
        "You're in a small chamber lit by an eerie green light. An extremely\n"
        "narrow tunnel exits to the west.  A dark corridor leads NE.",
        F_LIGHTED|F_NOBACK|F_NODWARF|F_HINTABLE, &at_plover
    },
    /* R_DARK */ {
        "You're in Dark room.",
        "You're in the Dark room. A corridor leading south is the only exit.",
        F_NOBACK|F_NODWARF|F_HINTABLE, &at_dark
    },
    /* R_ARCHED */ {
        "You're in arched hall.",
        "You are in an arched hall. A coral passage continues up and east.\n"
        "The air smells of sea water.",
        0, &at_arched
    },
    /* R_SHELL */ {
        "You're in Shell room.",
        "You're in a large room carved out of sedimentary rock. The floor and\n"
        /* Platt has "imbedded". */
        "walls are littered with bits of shells embedded in the stone.  A\n"
        "shallow passage proceeds downward, and a somewhat steeper one leads\n"
        "up.  A low hands and knees passage enters from the south.",
        F_NOBACK, &at_shell
    },
    /* R_RAGGED */ {
        NULL,
        "You are in a long sloping corridor with ragged sharp walls.",
        0, &at_ragged
    },
    /* R_CULDESAC */ {
        NULL,
        "You are in a cul-de-sac about eight feet across.",
        F_ONE_EXIT, &at_culdesac
    },
    /* R_ANTE */ {
        "You're in anteroom.",
        "You are in an anteroom leading to a large passage to the east. Small\n"
        "passages go west and up.  The remnants of recent digging are evident.\n"
        "A sign in midair here says \"Cave under construction beyond this point.\n"
        "Proceed at own risk.  {Witt Construction Company}\"",
        0, &at_anteroom
    },
    /* R_MAZED107 */ {
        NULL,
        "You are in a maze of twisty little passages, all different.",
        F_HINTABLE|F_INMAZE, &at_mazed107
        /* This room is deliberately *not* marked F_NOBACK, so that a player
         * who wanders into the maze by mistake can get back out if they want. */
    },
    /* R_WITT */ {
        "You're at Witt's End.",
        "You are at Witt's End. Passages lead off in *all* directions.",
        F_HINTABLE|F_NOBACK, &at_witt
    },
    /* R_MIRROR */ {
        "You're in mirror canyon.",
        "You are in a north/south canyon about 25 feet across. The floor is\n"
        "covered by white mist seeping in from the north.  The walls extend\n"
        "upward for well over 100 feet.  Suspended from some unseen point far\n"
        "above you, an enormous two-sided mirror is hanging parallel to and\n"
        "midway between the canyon walls.  (The mirror is obviously provided\n"
        "for the use of the dwarves, who as you know, are extremely vain.)  A\n"
        "small window can be seen in either wall, some fifty feet up.",
        0, &at_mirror
    },
    /* R_STALACT */ {
        "You're at top of stalactite.",
        "A large stalactite extends from the roof and almost reaches the floor\n"
        "below.  You could climb down it, and jump from it to the floor, but\n"
        "having done so you would be unable to reach it to climb back up.",
        F_NOBACK, &at_stalact
    },
    /* R_MAZED112 */ {
        NULL,
        "You are in a little maze of twisting passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed112
    },
    /* R_RES */ {
        "You're on southern edge of reservoir.",
        "You are on the southern edge of a large underground reservoir.  A\n"
        "thick cloud of white mist fills the room, rising from the surface\n"
        "of the water and drifting rapidly upwards.  The lake is fed by a\n"
        "stream, which tumbles out of a hole in the wall about 10 feet overhead\n"
        "and splashes noisily into the water near the reservoir's northern wall.\n"
        "A dimly-seen passage exits through the northern wall, but you can't get\n"
        "across the water to get to it.  Another passage leads south from here.",
        F_WATER|F_ONE_EXIT, &at_res
    },
    /* R_RESERVOIR_N */ {
        "You are at the northern end of the reservoir.",
        "You are at the northern end of a large underground reservoir.  Across\n"
        "the water to the south, a dark passage is visible.  Another passage\n"
        "leads north from here.  Large, clawed tracks are visible in the damp\n"
        "ground, leading from the passage into the water.",
        F_WATER|F_NODWARF|F_NOBACK, &at_reservoir_n
    },
    /* R_WARM */ {
        "You're in small, warm chamber.",
        "You are in a small chamber with warm walls.  Mist drifts into the\n"
        "chamber from a passage entering from the south and evaporates in\n"
        "the heat.  Another passage leads out to the northeast.",
        F_NODWARF, &at_warm
    },
    /* R_BALCONY */ {
        "You're on balcony above treasure chamber.",
        "You are in a high balcony carved out of solid rock overlooking a large,\n"
        "bare chamber lit by dozens of flickering torches.  A rushing stream\n"
        "pours into the chamber through a two-foot slit in the east wall and\n"
        "drains into a large pool along the north side of the chamber.  A small\n"
        "plaque riveted to the edge of the balcony reads, \"You are looking at\n"
        "the Witt Company's main treasure room, constructed by the famous\n"
        "architect Ralph Witt in 4004 B.C., and dedicated to the proposition\n"
        "that all adventurers are created equal (although some are more equal\n"
        "than others).  NO ADMITTANCE VIA THIS ENTRANCE!\"  A small, dark\n"
        "tunnel leads out to the west.",
        F_LIGHTED|F_NODWARF|F_ONE_EXIT, &at_balcony
    },
    /* R_FAKE_SLIT */ {
        "You're at slit in streambed.",
        "At your feet all the water of the stream splashes into a 2-foot slit\n"
        "in the rock.  Downstream the streambed is bare rock.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF, &at_fake_slit
    },
    /* R_CYLINDRICAL */ {
        "You're in cylindrical chamber.",
        "You are in a small cylindrical room with very smooth walls and a flat\n"
        "floor and ceiling.  There are no exits visible anywhere.",
        F_LIGHTED|F_NOBACK, &at_cylindrical
    },
    /* R_PIRATES_NEST */ { NULL, dead_end, F_INMAZE|F_NODWARF, &at_pirates_nest },
    /* R_SWOFCHASM */ {
        "You're on SW side of chasm.",
        "You are on one side of a large, deep chasm. A heavy white mist rising\n"
        "up from below obscures all view of the far side.  A SW path leads away\n"
        "from the chasm into a winding corridor.",
        F_NOBACK|F_NODWARF, &at_swofchasm
    },
    /* R_NEOFCHASM */ {
        "You're on NE side of chasm.",
        "You are on the far side of the chasm. A NE path leads away from the\n"
        "chasm on this side.",
        F_NOBACK|F_NODWARF, &at_neofchasm
    },
    /* R_SLOPING */ {
        "You're in sloping corridor.",
        "You are in a long winding corridor sloping out of sight in both\n"
        "directions.",
        0, &at_sloping
    },
    /* R_SECRETCYNNE1 */ {
        NULL,
        "You are in a secret canyon which exits to the north and east.",
        0, &at_secretcynne1
    },
    /* R_SECRETCYNNE2 */ {
        NULL,
        "You are in a secret canyon which exits to the north and east.",
        0, &at_secretcynne2
    },
    /* R_CORRIDOR */ {
        "You're in corridor.",
        "You're in a long east/west corridor. A faint rumbling noise can be\n"
        "heard in the distance.",
        F_NODWARF, &at_corridor
    },
    /* R_FORK */ {
        "You're at fork in path.",
        "The path forks here. The left fork leads northeast. A dull rumbling\n"
        "seems to get louder in that direction.  The right fork leads southeast\n"
        "down a gentle slope.  The main corridor enters from the west.",
        F_NODWARF, &at_fork
    },
    /* R_WARMJUNCTN */ {
        "You're at junction with warm walls.",
        "The walls are quite warm here. From the north can be heard a steady\n"
        "roar, so loud that the entire cave seems to be trembling.  Another\n"
        "passage leads south, and a low crawl goes east.",
        F_NODWARF, &at_warmjunctn
    },
    /* R_VIEW */ {
        "You're at breath-taking view.",
        "You are on the edge of a breath-taking view. Far below you is an\n"
        "active volcano, from which great gouts of molten lava come surging\n"
        "out, cascading back down into the depths.  The glowing rock fills the\n"
        "farthest reaches of the cavern with a blood-red glare, giving every-\n"
        "thing an eerie, macabre appearance.  The air is filled with flickering\n"
        "sparks of ash and a heavy smell of brimstone.  The walls are hot to\n"
        "the touch, and the thundering of the volcano drowns out all other\n"
        "sounds.  Embedded in the jagged roof far overhead are myriad twisted\n"
        "formations composed of pure white alabaster, which scatter the murky\n"
        "light into sinister apparitions upon the walls.  To one side is a deep\n"
        "gorge, filled with a bizarre chaos of tortured rock which seems to\n"
        "have been crafted by the devil himself.  An immense river of fire\n"
        "crashes out from the depths of the volcano, burns its way through the\n"
        "gorge, and plummets into a bottomless pit far off to your left.\n"
        "Across the gorge, the entrance to a valley is dimly visible.  To\n"
        "the right, an immense geyser of blistering steam erupts continuously\n"
        "from a barren island in the center of a sulfurous lake, which bubbles\n"
        "ominously.  The far right wall is aflame with an incandescence of its\n"
        "own, which lends an additional infernal splendor to the already\n"
        "hellish scene.  A dark, foreboding passage exits to the south.",
            F_LIGHTED|F_NODWARF, &at_view
    },
    /* R_FACES */ {
        "You're at the south end of the Valley of the Stone Faces.",
        "You are standing at the southern end of a long valley illuminated by\n"
        "flickering red light from the volcanic gorge behind you.  Carved\n"
        "into the walls of the valley is an incredible series of stone faces.\n"
        "Some of them look down into the valley with expressions of\n"
        "benevolence that would credit a saint;  others glare with a malice\n"
        "that makes the heart grow faint.  All of them are imbued with a\n"
        "fantastic seeming of life by the shifting and flickering light of\n"
        "the volcano.  The entire far end of the valley is taken up by an\n"
        "immense carving of a seated figure;  its exact form cannot be seen\n"
        "from here due to the uncertainty of the light.",
        F_LIGHTED|F_NOBACK|F_NODWARF, &at_faces
    },
    /* R_BY_FIGURE */ {
        "You're at north end of the Valley of the Stone Faces.",
        "You are standing at the north end of the Valley of the Stone Faces.\n"
        "Above you, an incredible bas-relief statue of an immense minotaur\n"
        "has been carved out of the rock.  At least sixty feet high, it sits\n"
        "gazing down at you with a faint but definite expression of amusement.\n"
        "Between its feet and the floor is a rock wall about ten feet high\n"
        "which extends across the entire north end of the valley.",
        F_LIGHTED|F_NODWARF, &at_by_figure
    },
    /* R_PLAIN_1 */ {
        "You're at south end of fog-filled room.",
        "You are standing at the southern end of what appears to be a\n"
        "large room filled with multicolored fog.  The sides and far end\n"
        "of the room cannot be seen due to the thickness of the fog - it's\n"
        "a real pea-souper (even to the color in places!).  A passage leads\n"
        "back to the south;  a dull rumbling sound issues from the passage.",
        F_NOBACK|F_NODWARF, &at_plain_1
    },
    /* R_PLAIN_2 */ {
        NULL, NULL, F_LIGHTED|F_NOBACK|F_NODWARF|F_HINTABLE, &at_plain_2
    },
    /* R_PLAIN_3 */ {
        "You're in foggy room by cairn of rocks.",
        "You are standing in a fog-filled room next to a tall cairn of glowing\n"
        "rocks.  An opening in the cairn leads down to a dark passage.",
        F_LIGHTED|F_NODWARF, &at_plain_3
    },
    /* R_NONDESCRIPT */ {
        "You're in nondescript chamber.",
        "You're in a small, nondescript chamber.  A dark passage leads up\n"
        "and to the south, and a wide but low crawl leads north.",
        F_NODWARF, &at_nondescript
    },
    /* R_PENTAGRAM */ {
        "You're in room with pentagram.",
        "You're in a small room with a very smooth rock floor, onto which\n"
        "has been marked a pentagram.  A low crawl leads out to the west, and\n"
        "a crack in the rock leads north.",
        F_NODWARF, &at_pentagram
    },
    /* R_CHIMNEY */ {
        "You're at the end of the crack, at the bottom of the chimney.",
        "The crack in the rock ends here, but a narrow chimney leads up.  You\n"
        "should be able to climb it.",
        F_NODWARF, &at_chimney
    },
    /* R_TUBE */ {
        "You're in lava tube at top of chimney.",
        "You're at the top of a narrow chimney in the rock.  A cylindrical\n"
        "tube composed of hardened lava leads south.",
        F_NODWARF, &at_tube
    },
    /* R_TUBE_SLIDE */ {
        "You're at steep slide in lava tube.",
        "The lava tube continues down and to the south, but it becomes very\n"
        "steep here - if you go down it you probably won't be able to get\n"
        "back up.",
        F_NODWARF, &at_tube_slide
    },
    /* R_BASQUE_1 */ {
        "You're in rough and narrow passage.",
        "You are in a narrow and rough passage running north and south.\n"
        "A dull rumbling sound can be heard from the south.",
        F_NOBACK|F_NODWARF, &at_basque_1
    },
    /* R_BASQUE_2 */ {
        "You're in rough passage to the north of the basilisk's den.",
        "You are in a rough passage to the north of the basilisk's den.",
        F_NODWARF, &at_basque_2
    },
    /* R_BASQUE_FORK */ {
        "You're at fork in passage by steps.",
        "The passage here enters from the south and divides, with a wide\n"
        "tunnel exiting to the north and a set of steps leading downward.",
            F_NODWARF, &at_basque_fork
    },
    /* R_ON_STEPS */ {
        "You're on the steps.",
        "You are on a long, spiral set of steps leading downwards into the\n"
        "earth.",
            F_NODWARF, &at_on_steps
    },
    /* R_STEPS_EXIT */ {
        "You're at exit on steps.",
        "A small tunnel exits from the steps and leads north.  The steps\n"
        "continue downwards.",
            F_NODWARF, &at_steps_exit
    },
    /* R_STORAGE */ {
        "You're in the storage room.",
        "You're in what was once a storage room.  A set of steps lead up.",
            F_NODWARF, &at_storage
    },
    /* R_FAKE_Y2 */ {
        "You're at \"Y2\"?",
        "You are in a large room, with a passage to the south, a passage to the\n"
        "west, and a wall of broken rock to the east.  There is a large \"Y2\" on\n"
        "a rock in the room's center.",
            F_NODWARF, &at_fake_y2
    },
    /* R_FAKE_JUMBLE */ {
        NULL,
        "You are in a jumble of rock, with cracks everywhere.",
        F_NODWARF, &at_fake_jumble
    },
    /* R_CATACOMBS1 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs1 },
    /* R_CATACOMBS2 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs2 },
    /* R_CATACOMBS3 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs3 },
    /* R_CATACOMBS4 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs4 },
    /* R_CATACOMBS5 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs5 },
    /* R_CATACOMBS6 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs6 },
    /* R_CATACOMBS7 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs7 },
    /* R_CATACOMBS8 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs8 },
    /* R_CATACOMBS9 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs9 },
    /* R_CATACOMBS10 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs10 },
    /* R_CATACOMBS11 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs11 },
    /* R_CATACOMBS12 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs12 },
    /* R_CATACOMBS13 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs13 },
    /* R_CATACOMBS14 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs14 },
    /* R_CATACOMBS15 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs15 },
    /* R_CATACOMBS16 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs16 },
    /* R_CATACOMBS17 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs17 },
    /* R_CATACOMBS18 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs18 },
    /* R_CATACOMBS19 */ { NULL, enchanted_tunnels, F_NOBACK|F_NODWARF, &at_catacombs19 },
    /* R_AUDIENCE */ {
        "You're at west end of Audience Hall.",
        "You are standing at the west end of the royal Audience Hall.\n"
        "The walls here are composed of the finest marble, and the floor is\n"
        "built of slabs of rare onyx and bloodstone.  The ceiling is high and\n"
        "vaulted, and is supported by pillars of rare Egyptian red granite;\n"
        "it gives off a nacreous glow that fills the entire chamber with\n"
        "a light like moon-light shining off of polished silver.",
            F_LIGHTED|F_NODWARF, &at_audience
    },
    /* R_AUDIENCE_E */ {
        "You're at east end of Audience Hall.",
        "You are at the eastern end of the Audience Hall.  There is a large\n"
        "dais rising out of the floor here;  resting upon the dais is a strange-\n"
        "looking throne made out of interlocking bars and rods of metal.",
            F_LIGHTED|F_NODWARF, &at_audience_e
    },
    /* R_BANSHEE */ {
        "You're in winding passage.",
        "You are in a winding passage which enters from the northwest,\n"
        "loops around several times, and exits to the north.",
            F_NODWARF, &at_banshee
    },
    /* R_GOLDEN */ {
        "You're in golden chamber.",
        "You are in a chamber with golden walls and a high ceiling.  Passages\n"
        "lead south, northeast, and northwest.",
            F_NODWARF, &at_golden
    },
    /* R_ARABESQUE */ {
        "You're in Arabesque room.",
        "You are in a small room whose walls are covered with an elaborate\n"
        "pattern of arabesque figures and designs.",
            F_NODWARF, &at_arabesque
    },
    /* R_TRANSLUCENT */ {
        "You're in room with translucent walls.",
        "You are in a large room whose walls are composed of some translucent\n"
        "whitish mineral.  The room is illuminated by a flickering reddish\n"
        "glow shining through the southern wall.  A passage leads east.",
            F_LIGHTED|F_NODWARF, &at_translucent
    },
    /* R_CHAMBER */ {
        "You're in chamber of boulders.",
        "You are in a small chamber filled with large boulders. The walls are\n"
        "very warm, causing the air in the room to be almost stifling from the\n"
        "heat.  The only exit is a crawl heading west, through which is coming\n"
        "a low rumbling.",
            F_NODWARF, &at_chamber
    },
    /* R_LIME */ {
        "You're in limestone passage.",
        "You are walking along a gently sloping north/south passage lined with\n"
        "oddly shaped limestone formations.",
            F_NODWARF, &at_lime
    },
    /* R_FBARR */ {
        "You are at entrance of the barren room.",
        "You are standing at the entrance to a large, barren room. A sign\n"
        "posted above the entrance reads:  \"Caution!  Bear in room!\"",
            F_NODWARF, &at_fbarr
    },
    /* R_BARR */ {
        "You are in the barren room.",
        "You are inside a barren room. The center of the room is completely\n"
        "empty except for some dust.  Marks in the dust lead away toward the\n"
        "far end of the room.  The only exit is the way you came in.",
        F_NODWARF, &at_barr
    },
    /* R_MAZED131 */ {
        NULL,
        "You are in a maze of twisting little passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed131
    },
    /* R_MAZED132 */ {
        NULL,
        "You are in a little maze of twisty passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed132
    },
    /* R_MAZED133 */ {
        NULL,
        "You are in a twisting maze of little passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed133
    },
    /* R_MAZED134 */ {
        NULL,
        "You are in a twisting little maze of passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed134
    },
    /* R_MAZED135 */ {
        NULL,
        "You are in a twisty little maze of passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed135
    },
    /* R_MAZED136 */ {
        NULL,
        "You are in a twisty maze of little passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed136
    },
    /* R_MAZED137 */ {
        NULL,
        "You are in a little twisty maze of passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed137
    },
    /* R_MAZED138 */ {
        NULL,
        "You are in a maze of little twisting passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed138
    },
    /* R_MAZED139 */ {
        NULL,
        "You are in a maze of little twisty passages, all different.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazed139
    },
    /* R_PONY */ {
        NULL,
        "Dead end.",
        F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_pony
    },
    /* R_SANDSTONE */ {
        "You're in sandstone chamber.",
        "You are in a small chamber to the east of the Hall of Mists.  The\n"
        "walls are composed of rough red sandstone.  There is a large, cubical\n"
        "chunk of rock in the center of the room.",
        F_ONE_EXIT, &at_sandstone
    },
    /* R_MORION */ {
        "You're in the Morion room.",
        "You are in a small room.  The walls are composed of a dark, almost\n"
        "black form of smoky quartz; they glisten like teeth in the lamp-light.\n"
        "The only exit is the passage to the south through which you entered.",
        0, &at_morion
    },
    /* R_VAULT */ {
        "You're in room with vaulted ceiling.",
        "You are in a room with a high, vaulted ceiling.  A tunnel leads\n"
        "upwards and to the north.",
        0, &at_vault
    },
    /* R_PEELGRUNT */ {
        "You're in Peelgrunt room.",
        "You are in the Peelgrunt room.",
        F_NODWARF, &at_peelgrunt
    },
    /* R_INSAFE */ {
        "You are in the safe.",
        "You are inside the safe.",
        F_ONE_EXIT|F_NODWARF, &at_insafe
    },
    /* R_CORRID_1 */ {
        NULL,
        "You are standing in a wide, north-and-south corridor.",
        0, &at_corrid_1
    },
    /* R_CORRID_2 */ {
        "You're at bend in wide corridor.",
        "You are standing at a bend in a wide corridor which runs to the east\n"
        "and west.  To the east, the corridor turns left and continues north;\n"
        "to the west, it turns left and continues south.",
        0, &at_corrid_2
    },
    /* R_TOOL */ {
        "You're in Tool room.",
        "You are in a small, low-ceilinged room with the words \"Witt Company\n"
        "Tool Room - Melenkurion division\" carved into one of the walls.  A\n"
        "wide corridor runs south from here.",
        F_ONE_EXIT, &at_tool
    },
    /* R_CORRID_3 */ {
        "You're at division in passage.",
        "You are at a division in a narrow passage.  Two spurs run east and\n"
        "north;  the main passage exits to the south.",
        0, &at_corrid_3
    },
    /* R_CUBICLE */ {
        "You're in dank cubicle.",
        "You are in a small, dank cubicle of rock.  A small passage leads back\n"
        "out to the south;  there is no other obvious exit.",
        F_ONE_EXIT, &at_cubicle
    },
    /* R_SPHERICAL */ {
        "You're in spherical room.",
        "You're in a large, completely spherical room with polished walls.  A\n"
        "narrow passage leads out to the north.",
        F_ONE_EXIT, &at_spherical
    },
    /* R_TUNNEL_1 */ {
        "You're in low tunnel with irregular ceiling.",
        "You are in a low tunnel with an irregular ceiling.  To the north, the\n"
        "tunnel is partially blocked by a recent cave-in, but you can probably\n"
        "get past the blockage without too much trouble.",
        0, &at_tunnel_1
    },
    /* R_GLASSY */ {
        "You're in large room with glassy walls.",
        "You're standing in a very large room (which however is smaller than the\n"
        "Giant room) which has smooth, glassy-looking walls.  A passage enters\n"
        "from the south and exits to the north.",
        0, &at_glassy
    },
    /* R_LAIR */ {
        "You're in the Sorcerer's Lair.",
        "This is the Sorcerer's Lair.  The walls are covered with exotic runes\n"
        "written in strange, indecipherable scripts;  the only readable phrase\n"
        "reads \"noside samoht\".  Strange shadows flit about on the walls, but\n"
        "there is nothing visible to cast them.  Iridescent blue light drips\n"
        "from a stalactite far above, falls towards the floor, and evaporates\n"
        "before touching the ground.  A deep, resonant chanting sound vibrates\n"
        "from deep in the ground beneath your feet, and a whispering sound\n"
        "composed of the echoes of long-forgotten spells and cantrips seeps\n"
        "from the walls and fills the air.  Passages exit to the east and west.",
        F_LIGHTED, &at_lair
    },
    /* R_BRINK_1 */ {
        "You're at brink of bottomless pit.",
        "You are standing on the brink of what appears to be a bottomless pit\n"
        "plunging down into the bowels of the earth.  Ledges run around the\n"
        "pit to the east and west, and a passage leads back to the north.",
        0, &at_brink_1
    },
    /* R_BRINK_2 */ {
        "You're on southern edge of bottomless pit.",
        "You are standing at the south end of a ledge running around the west\n"
        "side of a bottomless pit.  The ledge once continued around to the east\n"
        "side of the pit, but was apparently obliterated by a rock-slide\n"
        "years ago.  A cold wind blows out of a tunnel leading to the southeast.",
        0, &at_brink_2
    },
    /* R_ICE */ {
        "You're in Ice room.",
        "You are in the Ice room.  The walls and ceiling here are composed of\n"
        "clear blue glacial ice;  the floor is fortunately made of rock and\n"
        "is easy to walk upon.  There is a passage leading to the northwest,\n"
        "and a slide of polished ice leading downwards to the east - if you\n"
        "were to slide down it you probably couldn't get back up.",
        F_NOBACK, &at_ice
    },
    /* R_SLIDE */ {
        "You're at bottom of icy slide.",
        "You're at the entrance to an extensive and intricate network of tunnels\n"
        "carved out of solid ice.  A slippery slope leads upwards and north, but\n"
        "you cannot possibly climb up it.",
        F_INMAZE|F_HINTABLE|F_NOBACK, &at_slide
    },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave1 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK|F_ONE_EXIT, &at_icecave1a },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave2 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave2a },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave3 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK|F_ONE_EXIT, &at_icecave3a },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave4 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave5 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave6 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK|F_ONE_EXIT, &at_icecave7 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK|F_ONE_EXIT, &at_icecave8 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave9 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave10 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave11 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave12 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave12a },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK|F_ONE_EXIT, &at_icecave13 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK|F_ONE_EXIT, &at_icecave14 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave15 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave15a },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave16 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave17 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave18 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave19 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave20 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE|F_NOBACK, &at_icecave21 },
    /* Strangely, Platt stops marking these rooms NOBACK at number 21.
     * Maybe this is a reward for the player's persistence, or maybe
     * it's just a bug. */
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave22 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave23 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave24 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave25 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave26 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave27 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave28 },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave28a },
    /* R_ICECAVE */ { NULL, ice_tunnels, F_INMAZE|F_HINTABLE, &at_icecave29 },
    /* R_ICECAVE30 */ {
        "You're in small, icy chamber.",
        "You are in a small chamber melted out of the ice.  Glowing letters\n"
        "in midair spell out the words \"This way out\".",
        F_INMAZE|F_HINTABLE, &at_icecave30
        },
    /* R_BRINK_3 */ {
        "You're on eastern side of bottomless pit.",
        "You are standing on the eastern side of a bottomless pit.  A narrow\n"
        "ledge runs north towards a dimly-visible passage;  the ledge once\n"
        "continued south of this point but has been shattered by falling rock.\n"
        "A narrow crack in the rock leads northeast.",
        0, &at_brink_3
    },
    /* R_CRACK_1 */ {
        "You're in narrow, twisting crack.",
        "You are following a narrow crack in the rock which enters from the\n"
        "southwest, turns and twists somewhat, and exits to the southeast.",
        0, &at_crack_1
    },
    /* R_CRACK_2 */ {
        "You're at north end of tight passage.",
        "You are standing at the northern end of a rather tight passage.  A\n"
        "narrow crack in the rock leads west.",
        0, &at_crack_2
    },
    /* R_CRACK_3 */ {
        "You're at south end of tight passage.",
        "You are at the southern end of a tight passage.  A hands-and-knees\n"
        "crawl continues to the south.",
        0, &at_crack_3
    },
    /* R_CRACK_4 */ {
        "You're in very small chamber.",
        "You are in a very small chamber.  A narrow crawl leads north.",
        F_ONE_EXIT, &at_crack_4
    },
    /* R_ARCH_COR_1 */ {
        "You're in coral passage.",
        "You are in an arched coral passage which enters from the west, splits,\n"
        "and continues on to the east over a smooth and damp-looking patch of\n"
        "sand.  The fork in the passage once led to the south, but it is now\n"
        "completely blocked by debris.",
        F_NOBACK, &at_arch_cor_1
    },
    /* R_ARCH_COR_2 */ {
        "You're at bend in arched coral corridor.",
        /* Platt has "corral". */
        "You are at a bend in an arched coral passage;  the passage enters\n"
        "from the west over a patch of damp sand, turns, and continues north.",
        F_NOBACK, &at_arch_cor_2
    },
    /* R_ARCH_FORK */ {
        /* Platt has "corral". */
        "You're at fork in arched coral passage.",
        "You are at a fork in a high, arched coral passage.  The main portion\n"
        "of the passage enters from the south;  two smaller passages lead\n"
        "east and north.  The smell of salt water is very strong here.",
        0, &at_arch_fork
    },
    /* R_JONAH */ {
        "You're at entrance to the Jonah room.",
        "You are standing at the entrance of the Jonah room, a cavernous\n"
        "hall with high ribbed walls.  The hall extends far to the south;\n"
        "a coral passage leads west.",
        0, &at_jonah
    },
    /* R_IN_JONAH */ {
        "You're at south end of the Jonah room.",
        "You are at the south end of the Jonah room.  Ahead of you, the way\n"
        "is barred by a large set of immense stalactites and stalagmites\n"
        "which intermesh like clenched teeth.  Nothing except blackness is\n"
        "visible between the stone formations.",
        0, &at_in_jonah
    },
    /* R_FOURIER */ {
        "You're in the Fourier passage.",
        "You are in the Fourier passage.  This is a long and highly convoluted\n"
        "passage composed of coral, which twists and turns like the path of\n"
        "an earthworm tripping on LSD.  The passage here enters from the\n"
        "northwest, convulses, and exits to the southwest (from which\n"
        "direction can be felt a cool and salty-smelling breeze).",
        0, &at_fourier
    },
    /* R_SHELF */ {
        "You're on shelf of rock above beach.",
        /* Platt has "sedementary"; I've fixed the spelling. */
        "You are standing on a large shelf of sedimentary rock overlooking\n"
        "a lava beach.  The shelf is an extension of an incredible cliff\n"
        "which extends north, south, and upwards for as far as the eye can\n"
        "see.  Crudely carved steps lead down from the shelf to the beach, and\n"
        "a twisting coral passage exits to the west.",
        F_LIGHTED, &at_shelf
    },
    /* R_BEACH */ {
        "You're on beach.",
        "You are standing on a short, barren beach composed of hardened lava.\n"
        "Rugged and unclimbable volcanic hills block all view to the north\n"
        "and south, and a seemingly infinite cliff fills the entire western\n"
        "hemisphere.  To the east, a narrow inlet of ocean water laps gently\n"
        "upon the beach.  The scene is illuminated by the light of three small\n"
        "moons shining through the shimmering glow of an aurora that fills\n"
        "the entire sky with golden splendor.  Steps lead up the cliff to a\n"
        "shelf of rock.",
        F_LIGHTED|F_ONE_EXIT, &at_beach
    },
    /* R_PLATFORM */ {
        "You're on tiny platform above volcano.",
        "You are precariously perched on a tiny platform suspended in midair.\n"
        "Two thousand feet below you is the mouth of a very active volcano,\n"
        "spewing out a river of hot lava.",
        F_LIGHTED|F_NOBACK, &at_platform
    },
    /* R_YLEM */ {
        NULL, NULL, F_NOBACK, NULL
    }
};


struct ObjectData objs[] = {
#define XX {0,0,0,0}
#define in(loc) loc, loc
#define schiz(loc) loc, loc+1
    { XX, 0, NULL /* NOTHING */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* GRATE */, F_SCHIZOID, schiz(R_DEPRESSION) },
    { XX, 0, NULL /* STEPS */, F_INVISIBLE|F_SCHIZOID, schiz(R_SPIT) },
    { XX, 0, NULL /* DOOR */, 0, in(R_IMMENSENSPASS) },
    { XX, 0, NULL /* SNAKE */, 0, in(R_HMK) },
    { XX, 0, NULL /* FISSURE */, F_INVISIBLE|F_SCHIZOID, schiz(R_EFISS) },
    { XX, 0, NULL /* TABLET */, 0, in(R_DARK) },
    { XX, 0, NULL /* MIRROR */, F_INVISIBLE, in(R_MIRROR) },
    { XX, 0, NULL /* PLANT */, 0, in(R_WPIT) },
    { XX, 0, NULL /* PLANT2 */, F_INVISIBLE|F_SCHIZOID, schiz(R_W2PIT) },
    { XX, 0, NULL /* STALACTITE */, F_INVISIBLE, in(R_STALACT) },
    { XX, 8, NULL /* FOG */, F_INVISIBLE|F_SCHIZOID, schiz(R_PLAIN_2) },
    { XX, 0, NULL /* GLOW */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* SHADOW */, F_SCHIZOID, schiz(R_WINDOE) },
    { XX, 0, NULL /* BLOB */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* DRAWINGS */, F_INVISIBLE, in(R_ORIENTAL) },
    { XX, 0, NULL /* PIRATE */, F_INVISIBLE, in(R_LIMBO) },
    { XX, 0, NULL /* DRAGON */, F_SCHIZOID, schiz(R_SECRETCYNNE1) },
    { XX, 0, NULL /* CHASM */, F_SCHIZOID, schiz(R_SWOFCHASM) },
    { XX, 0, NULL /* TROLL */, F_SCHIZOID, schiz(R_SWOFCHASM) },
    { XX, 0, NULL /* TROLL2 */, F_SCHIZOID, in(R_LIMBO) },
    { XX, 0, NULL /* OGRE */, 0, in(R_GLASSY) },
    { XX, 0, NULL /* BASILISK */, F_SCHIZOID, schiz(R_BASQUE_1) },
    { XX, 0, NULL /* GONG */, 0, in(R_RESERVOIR_N) },
    { XX, 0, NULL /* DJINN */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* TURTLE */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* MESSAGE */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* GORGE */, F_INVISIBLE|F_SCHIZOID, schiz(R_VIEW) },
    { XX, 0, NULL /* STATUE */, F_INVISIBLE, in(R_BY_FIGURE) },
    { XX, 0, NULL /* QUICKSAND */, F_INVISIBLE|F_SCHIZOID, schiz(R_ARCH_COR_1) },
    { XX, 0, NULL /* SLIME */, 0, in(R_CRACK_2) },
    { XX, 0, NULL /* PONY */, 0, in(R_PONY) },
    { XX, 0, NULL /* SAFE */, F_SCHIZOID, schiz(R_VAULT) },
    { XX, 0, NULL /* THRONE */, F_INVISIBLE, in(R_AUDIENCE_E) },
    { XX, 0, NULL /* SKELETON */, 0, in(R_AUDIENCE_E) },
    { XX, 0, NULL /* BEAR */, F_PORTABLE, in(R_BARR) },
    { XX, 0, NULL /* BATTERIES */, F_PORTABLE, in(R_LIMBO) },
    { XX, 0, NULL /* MOSS */, F_INVISIBLE, in(R_SOFT) },
    { XX, 0, NULL /* DINGHY */, 0, in(R_BEACH) },
    { XX, 0, "Bag filled with pieces of eight", F_PORTABLE|F_UNSTABLE, in(R_BEACH) },
    { XX, 0, "Crown", F_PORTABLE|F_UNSTABLE, in(R_INSAFE) },
    { XX, 0, "Large gold nugget", F_PORTABLE, in(R_NUGGET) },
    { XX, 0, "Several diamonds", F_PORTABLE, in(R_WFISS) },
    { XX, 0, "Bars of silver", F_PORTABLE, in(R_NS) },
    { XX, 0, "Precious jewelry", F_PORTABLE, in(R_SOUTH) },
    { XX, 0, "Rare coins", F_PORTABLE, in(R_WEST) },
    { XX, 0, "Treasure chest", F_PORTABLE|F_UNSTABLE, in(R_LIMBO) },
    { XX, 0, "Golden eggs", F_PORTABLE, in(R_GIANT) },
    { XX, 0, "Jeweled trident", F_PORTABLE, in(R_FALLS) },
    { XX, 0, "Helmet", F_PORTABLE, in(R_MORION) },
    { XX, 0, "Ming vase", F_PORTABLE, in(R_ORIENTAL) },
    { XX, 0, "Worthless shards of pottery", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Egg-sized emerald", F_PORTABLE, in(R_PLOVER) },
    { XX, 0, "Sapphire sceptre", F_PORTABLE, in(R_AUDIENCE_E) },
    { XX, 0, "Ruby-covered toy yacht", F_PORTABLE|F_UNSTABLE, in(R_NONDESCRIPT) },
    { XX, 0, "Platinum pyramid", F_PORTABLE, in(R_DARK) },
    { XX, 0, "Glistening pearl", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Persian rug", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Rare spices", F_PORTABLE, in(R_CHAMBER) },
    { XX, 0, "Ancient Indian turquoise beads.", F_PORTABLE|F_UNSTABLE, in(R_BALCONY) },
    { XX, 1, "Golden chain", F_PORTABLE, in(R_BARR) },
    { XX, 0, "Mithril ring", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Scrimshaw spyglass", F_PORTABLE, in(R_IN_JONAH) },
    { XX, 0, "Rock-crystal sculpture", F_PORTABLE|F_UNSTABLE, in(R_ICECAVE14) },
    { XX, 0, "Jade bracelet", F_PORTABLE, in(R_TRANSLUCENT) },
    { XX, 0, "Casket of opals", F_PORTABLE|F_UNSTABLE, in(R_CRACK_4) },
    { XX, 0, "Set of keys", F_PORTABLE, in(R_HOUSE) },
    { XX, 0, "Brass lantern", F_PORTABLE, in(R_HOUSE) },
    { XX, 0, "Wicker cage", F_PORTABLE|F_UNSTABLE, in(R_COBBLES) },
    { XX, 0, "Little bird in cage", F_PORTABLE, in(R_BIRD) },
    { XX, 0, "Black rod", F_PORTABLE, in(R_DEBRIS) },
    { XX, 0, "Metal plate", F_PORTABLE|F_UNSTABLE, in(R_STORAGE) },
    { XX, 0, "Black rod", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Velvet pillow", F_PORTABLE, in(R_SOFT) },
    { XX, 0, "Giant clam  >grunt!<", F_PORTABLE, in(R_SHELL) },
    { XX, 0, "Giant oyster  >groan!<", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "\"Spelunker Today\"", F_PORTABLE, in(R_ANTE) },
    { XX, 0, NULL /* KNIFE */, F_INVISIBLE|F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Tasty food", F_PORTABLE, in(R_HOUSE) },
    { XX, 0, NULL /* BOTTLE */, F_PORTABLE, in(R_HOUSE) },
    { XX, 0, NULL /* WATER */, F_PORTABLE, in(R_LIMBO) },
    { XX, 0, NULL /* OIL */, F_PORTABLE, in(R_LIMBO) },
    { XX, 1, "Earthenware flask", F_PORTABLE|F_UNSTABLE, in(R_ARABESQUE) },
    { XX, 0, "Dwarf's axe", F_PORTABLE, in(R_LIMBO) },
    { XX, 0, "Singing sword", F_PORTABLE, in(R_SANDSTONE) },
    { XX, 0, "Several dragon's teeth", F_PORTABLE|F_UNSTABLE, in(R_LIMBO) },
    { XX, 0, "Vial of oily liquid", F_PORTABLE, in(R_SPHERICAL) },
    { XX, 0, "Mushroom", F_PORTABLE|F_UNSTABLE, in(R_CUBICLE) },
    { XX, 0, NULL /* GOBLINS */, 0, in(R_LIMBO) },
    { XX, 0, NULL /* DWARF */, F_INVISIBLE, in(R_LIMBO) }
#undef XX
#undef in
#undef schiz
};

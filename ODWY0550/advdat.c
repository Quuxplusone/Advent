
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
    puts("You have crawled around in some little holes and wound up back in the" SOFT_NL
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
            puts("You have jumped into a bottomless pit.  You continue to fall for" SOFT_NL
                 "a very long time.  First, your lamp runs out of power and goes" SOFT_NL
                 "dead. Later, you die of hunger and thirst.");
        } else {
            puts("You have jumped into a bottomless pit.  Eventually, you die of thirst.");
        }
    } else {
        puts("You have jumped into a bottomless pit.  Eventually, you die of thirst.");
    }
    return you_are_dead_at(R_YLEM);
}

bool movement(WordClass t, int meaning)
{
    /* HOUSE FOO will take you to the house, even if FOO is unrecognized;
     * whereas FOO HOUSE will not. SAY HOUSE does basically the same thing
     * as HOUSE. */
    if (word1.type == t && word1.meaning == meaning)
	return true;
    if (word1.type == WordClass_Verb) {
	if (word1.meaning == WALK || word1.meaning == SAY)
	    return (word2.type == t && word2.meaning == meaning);
    }
    return false;
}

bool movemento(ObjectWord m) { return movement(WordClass_Object, m); }
bool movementp(Location m) { return movement(WordClass_Place, m); }
bool movementv(VerbWord m) { return movement(WordClass_Verb, m); }

bool movementv2(VerbWord a, VerbWord b)
{
    return movementv(a) || movementv(b);
}

bool movementv3(VerbWord a, VerbWord b, VerbWord c)
{
    return movementv2(a, b) || movementv(c);
}

static bool correctly_used_magic_word(VerbWord v)
{
    return (keywordv(v) && (keywordv(SAY) || word2.type == WordClass_None));
}

static bool correctly_used_magic_word_PLOVER(void)
{
    return (keywordp(R_PLOVER) && (keywordv(SAY) || word2.type == WordClass_None));
}

int at_limbo(void)
{
    if (movementv(OUT)) return R_HOUSE;
    return 0;  /* command hasn't been processed yet */
}

int at_road(void)
{
    if (movementp(R_ROAD)) return R_HILL;
    if (movementv2(WEST, UP)) return R_HILL;
    if (movementv2(IN, EAST)) return R_HOUSE;
    if (movementp(R_HOUSE)) return R_HOUSE;
    if (movementv3(DOWNSTREAM, GULLY, STREAM)) return R_VALLEY;
    if (movementv2(SOUTH, DOWN)) return R_VALLEY;
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv2(NORTH, EAST)) return R_FOREST;
    if (movementp(R_DEPRESSION)) return R_DEPRESSION;
    return 0;  /* command hasn't been processed yet */
}

int at_hill(void)
{
    if (movementp(R_ROAD)) return R_ROAD;
    if (movementp(R_HOUSE)) return R_ROAD;
    if (movementv3(FORWARD, EAST, NORTH)) return R_ROAD;
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv2(NORTH, SOUTH)) return R_FOREST;
    return 0;  /* command hasn't been processed yet */
}

int at_house(void)
{
    if (movementp(R_ROAD)) return R_ROAD;
    if (movementv3(OUT, OUTDOORS, WEST)) return R_ROAD;
    if (closure < 2) {
        if (correctly_used_magic_word(XYZZY)) {
            say_foof();
            return R_DEBRIS;
        }
        if (correctly_used_magic_word(PLUGH)) {
            say_foof();
            return R_Y2;
        }
    }
    if (movementv2(STREAM, DOWNSTREAM)) {
        puts("The stream flows out through a pair of 1 foot diameter sewer pipes." SOFT_NL
             "It would be advisable to leave via the exit.");
        return R_HOUSE;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_valley(void)
{
    if (movementv2(UPSTREAM, NORTH)) return R_ROAD;
    if (movementp(R_HOUSE)) return R_ROAD;
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv3(EAST, WEST, UP)) return R_FOREST;
    if (movementp(R_DEPRESSION)) return R_DEPRESSION;
    if (movementv3(SOUTH, DOWN, DOWNSTREAM))
        return (closure < 2) ? R_SLIT : R_FAKE_SLIT;
    return 0;  /* command hasn't been processed yet */
}

int at_forest(void)
{
    if (movementp(R_VALLEY)) return R_VALLEY;
    if (movementv(EAST)) return R_VALLEY;
    if (movementv(DOWN)) return R_VALLEY;
    if (movementv(WEST)) return R_FOREST;
    if (movementv(SOUTH)) return R_FOREST;
    if (movementp(R_FOREST) || movementv2(FORWARD, NORTH))
        return pct(50) ? R_FOREST : R_FOREST2;
    return 0;  /* command hasn't been processed yet */
}

int at_forest2(void)
{
    if (movementp(R_ROAD)) return R_ROAD;
    if (movementv(NORTH)) return R_ROAD;
    if (movementp(R_VALLEY)) return R_VALLEY;
    if (movementv3(EAST, WEST, DOWN)) return R_VALLEY;
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv(SOUTH)) return R_FOREST;
    return 0;  /* command hasn't been processed yet */
}

int at_slit(void)
{
    if (movementp(R_HOUSE)) return R_ROAD;
    if (movementv(UPSTREAM)) return R_VALLEY;
    if (movementv(NORTH)) return R_VALLEY;
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv2(EAST, WEST)) return R_FOREST;
    if (movementv(DOWNSTREAM)) return R_DEPRESSION;
    if (movementv3(ROCK, BED, SOUTH)) return R_DEPRESSION;
    if (movementp(R_SLIT) || movementv3(STREAM, DOWN, IN)) {
        puts("You don't fit through a two-inch slit!");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_fake_slit(void)
{
    if (movementp(R_HOUSE)) return R_ROAD;
    if (movementv(UPSTREAM)) return R_VALLEY;
    if (movementv(NORTH)) return R_VALLEY;
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv(EAST)) return R_FOREST;
    if (movementv(WEST)) return R_FOREST;
    if (movementv(DOWNSTREAM)) return R_DEPRESSION;
    if (movementv(ROCK)) return R_DEPRESSION;
    if (movementv(BED)) return R_DEPRESSION;
    if (movementv(SOUTH)) return R_DEPRESSION;
    if (movementp(R_SLIT) || movementv3(STREAM, DOWN, IN)) {
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
         "you find yourself sitting on the edge of a pool of water in" SOFT_NL
         "a vast chamber lit by dozens of flaring torches.\n\n"
         "The floor is covered with thick layers of precious Persian rugs!\n\n"
         "Rare coins, bars of silver, and lumps of gold and platinum are" SOFT_NL
         "strewn carelessly about!\n\n"
         "There are diamonds, rubies, sapphires, emeralds, opals, pearls, and" SOFT_NL
         "fabulous sculptures and ornaments carved out of jade and imperishable" SOFT_NL
         "crystal resting on display shelves, along with rare Ming vases and" SOFT_NL
         "ancient Indian turquoise beads!\n\n"
         "A flotilla of ruby-encrusted toy boats is floating in the pool of" SOFT_NL
         "water beside you!\n\n"
         /* Platt capitalized "Iridium" here; I'm lowercasing it. */
         "A network of golden chains supports a fantastic iridium crown!\n\n"
         "There is a display case on the wall filled with a fantastic selection" SOFT_NL
         "of magical swords, which are singing \"Hail to the Chief\" in perfect" SOFT_NL
         "pitch and rhythm!\n\n"
         "There are a dozen friendly little dwarves in the room, displaying" SOFT_NL
         "their talents by deftly juggling hundreds of golden eggs!\n\n"
         "A large troll, a gigantic ogre, and a bearded pirate are tossing" SOFT_NL
         "knives, axes, and clubs back and forth in a friendly demonstration" SOFT_NL
         "of martial skill!\n\n"
         "A horde of cheerful little gooseberry goblins are performing" SOFT_NL
         "talented acrobatics to an appreciative audience composed of a dragon," SOFT_NL
         "a large green snake, a cute little bird (which is sitting, unmolested," SOFT_NL
         "on the snake's head), a peaceful basilisk, and a large Arabian Djinn.\n\n"
         "Everyone turns and sees you, and lets out a heart-warming cheer" SOFT_NL
         "of welcome!\n");
        gave_up = false;
        finis();  /* end the game in triumph */
    }
    return 0;  /* command hasn't been processed yet */
}

int at_depression(void)
{
    if (movementp(R_FOREST)) return R_FOREST;
    if (movementv2(EAST, SOUTH)) return R_FOREST2;
    if (movementv(WEST)) return R_FOREST;
    if (movementp(R_HOUSE)) return R_ROAD;
    if (movementv3(UPSTREAM, GULLY, NORTH) || movementp(R_SLIT)) {
        return (closure < 2) ? R_SLIT : R_FAKE_SLIT;
    }
    if (movementv2(IN, DOWN)) {
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
    if (movementv3(UP, OUT, SURFACE)) {
        if (objs[GRATE].prop == 1) {
            return R_DEPRESSION;
        } else {
            cantpasslock();
            return STAY_STILL;
        }
    }
    if (movementv(CRAWL)) return R_COBBLES;
    if (movementp(R_COBBLES)) return R_COBBLES;
    if (movementv2(IN, WEST)) return R_COBBLES;
    if (movementp(R_SPIT)) return R_SPIT;
    if (movementp(R_DEBRIS)) return R_DEBRIS;
    return 0;  /* command hasn't been processed yet */
}

int at_cobbles(void)
{
    if (movementv3(OUT, SURFACE, ENTRANCE)) return R_INSIDE;
    if (movementv(EAST)) return R_INSIDE;
    if (movementv2(IN, WEST)) return R_DEBRIS;
    if (movementp(R_DARK)) return R_DEBRIS;
    if (movementp(R_DEBRIS)) return R_DEBRIS;
    if (movementp(R_SPIT)) return R_SPIT;
    return 0;  /* command hasn't been processed yet */
}

int at_debris(void)
{
    if (movementv(ENTRANCE)) return R_INSIDE;
    if (movementv3(CRAWL, PASSAGE, EAST)) return R_COBBLES;
    if (movementp(R_COBBLES)) return R_COBBLES;
    if (movementp(R_LOW)) return R_COBBLES;
    if (movementp(R_AWK)) return R_AWK;
    if (movementv3(IN, UP, WEST)) return R_AWK;
    if (movementp(R_SPIT)) return R_SPIT;
    if (objs[GRATE].prop) {
        if (movementp(R_DEPRESSION)) return R_DEPRESSION;
    } else {
        if (movementp(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    if (correctly_used_magic_word(XYZZY)) {
        if (closure >= 2 || nomagic) {
            nothing_happens();
            panicked = true;
            return STAY_STILL;
        } else {
            say_foof();
            return R_HOUSE;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_awk(void)
{
    if (movementv(ENTRANCE)) return R_INSIDE;
    if (movementv2(DOWN, EAST)) return R_DEBRIS;
    if (movementp(R_DEBRIS)) return R_DEBRIS;
    if (movementv3(IN, UP, WEST)) return R_BIRD;
    if (movementp(R_SPIT)) return R_SPIT;
    if (objs[GRATE].prop) {
        if (movementp(R_DEPRESSION))
            return R_DEPRESSION;
    } else {
        if (movementp(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_bird(void)
{
    if (movementv(ENTRANCE)) return R_INSIDE;
    if (movementp(R_DEBRIS)) return R_DEBRIS;
    if (movementp(R_AWK)) return R_AWK;
    if (movementv(EAST)) return R_AWK;
    if (movementp(R_SPIT)) return R_SPIT;
    if (movementv2(PASSAGE, WEST)) return R_SPIT;
    if (objs[GRATE].prop) {
        if (movementp(R_DEPRESSION))
            return R_DEPRESSION;
    } else {
        if (movementp(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_spit(void)
{
    if (movementv(ENTRANCE)) return R_INSIDE;
    if (movementp(R_DEBRIS)) return R_DEBRIS;
    if (movementv2(PASSAGE, EAST)) return R_BIRD;
    if (objs[GRATE].prop) {
        if (movementp(R_DEPRESSION))
            return R_DEPRESSION;
    } else {
        if (movementp(R_DEPRESSION)) {
            cantpasslock();
            return R_INSIDE;
        }
    }
    if (movementv(WEST) || movementv(CRACK)) {
        puts("The crack is far too small for you to enter.");
        return STAY_STILL;
    }
    /* Platt takes these in a different order. */
    if (keywordv(JUMP)) {
        return splatter(R_EMIST);
    } else if (movementv(DOWN) || movemento(STEPS) || movementp(R_SPIT)) {
        if (toting(GOLD)) {
            puts("You are at the bottom of the pit with a broken neck.");
            return you_are_dead_at(R_EMIST);
        }
        return R_EMIST;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_emist(void)
{
    if (movementv2(LEFT, SOUTH)) return R_NUGGET;
    if (movementv(EAST)) return R_SANDSTONE;
    if (movementv3(FORWARD, HALL, WEST)) return R_EFISS;
    if (movementv3(STAIRS, DOWN, NORTH)) return R_HMK;
    if (movementp(R_Y2)) return R_JUMBLE;
    if (movementv3(UP, PASSAGE, IN) || movemento(STEPS) ||
        movementp(R_SPIT) || movementv(DOME)) {
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
    if (movementv3(FORWARD, CROSS, direction)) {
        if (objs[FISSURE].prop) return destination;
        puts("There is no way across the fissure.");
        return STAY_STILL;
    }
    if (movementv(JUMP)) {
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
    if (movementv2(HALL, EAST)) return R_EMIST;
    return cross_fissure(WEST, R_WFISS);
}

int at_nugget(void)
{
    if (movementv3(HALL, OUT, NORTH)) return R_EMIST;
    return 0;  /* command hasn't been processed yet */
}

int at_hmk(void)
{
    if (movementv3(STAIRS, UP, EAST)) return R_EMIST;
    if (there(SNAKE, R_HMK)) {
        if (movementv3(NORTH, LEFT, SOUTH) ||
            movementv3(RIGHT, WEST, FORWARD) ||
            movementv3(SW, NW, SE) ||
            movementv3(NE, DOWN, SECRET)) {
            puts("You can't get past the snake.");
            return STAY_STILL;
        }
    } else {
        /* no snake */
        if (movementv2(NORTH, LEFT)) return R_NS;
        if (movementv2(SOUTH, RIGHT)) return R_SOUTH;
        if (movementv2(WEST, FORWARD)) return R_WEST;
        if (movementv(DOWN)) return R_VAULT;
        if (movementv(NE)) return R_MORION;
        if (movementv(NW)) return R_CORRID_3;
        if (movementv(SE)) return R_CORRID_1;
        if (movementv(SECRET)) return R_SECRETEW_TITE;
        if (movementv(SW)) {
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
    if (movementv2(EAST, CROSS)) return R_E2PIT;
    if (movementv(WEST)) return R_SLAB;
    if (movementp(R_SLAB)) return R_SLAB;
    if (movementv(DOWN)) return R_WPIT;
    if (movementp(R_SPIT)) return R_WPIT;
    if (keywordv(HOLE)) {
        puts("It is too far up for you to reach.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_epit(void)
{
    if (movementv2(UP, OUT)) return R_E2PIT;
    return 0;  /* command hasn't been processed yet */
}

int at_wpit(void)
{
    if (movementv2(UP, OUT)) return R_W2PIT;
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
    if (movementv(WEST)) return R_WMIST;
    /* The only reason I use this awkward construction is to preserve
     * the behavior of the command "NORTH EAST". */
    int result = cross_fissure(EAST, R_EFISS);
    if (result != 0) return result;
    if (keywordv(NORTH)) {
        puts("You have crawled around in a little passage north of and parallel" SOFT_NL
             "to the Hall of Mists.");
        return R_WMIST;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_ns(void)
{
    if (movementv3(HALL, OUT, SOUTH)) return R_HMK;
    if (movementv(NORTH)) return R_Y2;
    if (movementp(R_Y2)) return R_Y2;
    if (movementv2(DOWN, HOLE)) return R_DIRTY;
    return 0;  /* command hasn't been processed yet */
}

int at_south(void)
{
    if (movementv3(HALL, OUT, NORTH)) return R_HMK;
    return 0;  /* command hasn't been processed yet */
}

int at_west(void)
{
    if (movementv3(HALL, OUT, EAST)) return R_HMK;
    if (movementv2(WEST, UP)) return R_CROSS;
    return 0;  /* command hasn't been processed yet */
}

int at_y2(void)
{
    if (movementv(SOUTH)) return R_NS;
    if (movementv(EAST)) return R_JUMBLE;
    if (movementp(R_JUMBLE)) return R_JUMBLE;
    if (movementv(WEST)) return R_WINDOE;
    if (correctly_used_magic_word(PLUGH) || correctly_used_magic_word_PLOVER()) {
        if (closure >= 2 || nomagic) {
            nothing_happens();
            panicked = true;
            return STAY_STILL;
        } else {
            say_foof();
            return keywordv(PLUGH) ? R_HOUSE : R_PLOVER;
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_jumble(void)
{
    if (movementv(DOWN)) return R_Y2;
    if (movementp(R_Y2)) return R_Y2;
    if (movementv(UP)) return R_EMIST;
    return 0;  /* command hasn't been processed yet */
}

int at_dirty(void)
{
    if (movementv2(EAST, CRAWL)) return R_CLEAN;
    if (movementv2(UP, HOLE)) return R_NS;
    if (movementv(WEST)) return R_DUSTY;
    if (movementp(R_BEDQUILT)) return R_BEDQUILT;
    return 0;  /* command hasn't been processed yet */
}

int at_windoe(void)
{
    if (movementv(EAST)) return R_Y2;
    if (movementp(R_Y2)) return R_Y2;
    if (movementv(JUMP)) {
        puts("You are at the bottom of the pit with a broken neck.");
        return you_are_dead_at(R_MIRROR);
    }
    return 0;  /* command hasn't been processed yet */
}

int at_clean(void)
{
    if (movementv2(WEST, CRAWL)) return R_DIRTY;
    if (movementv(DOWN)) return R_WET;
    if (movementp(R_SPIT)) return R_WET;
    if (movementv(CLIMB)) return R_WET;
    return 0;  /* command hasn't been processed yet */
}

int at_wet(void)
{
    if (movementv3(CLIMB, UP, OUT)) return R_CLEAN;
    if (movementv3(STREAM, DOWN, UPSTREAM) ||
            movementv(DOWNSTREAM) || movementp(R_SLIT)) {
        puts("You don't fit through a two-inch slit!");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_dusty(void)
{
    if (movementv2(EAST, PASSAGE)) return R_DIRTY;
    if (movementv2(DOWN, HOLE)) return R_COMPLEX;
    if (movementp(R_BEDQUILT)) return R_BEDQUILT;
    return 0;  /* command hasn't been processed yet */
}

int at_wmist(void)
{
    if (movementv(SOUTH)) return R_MAZEA42;
    if (movementv(UP)) return R_MAZEA42;
    if (movementv(PASSAGE)) return R_MAZEA42;
    if (movementv(CLIMB)) return R_MAZEA42;
    if (movementv(EAST)) return R_WFISS;
    if (movementv(WEST)) return R_ELONG;
    if (movementv(CRAWL)) return R_ELONG;
    if (keywordv(NORTH)) {
        puts("You have crawled around in a little passage north of and parallel" SOFT_NL
             "to the Hall of Mists.");
        return R_WFISS;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_mazea42(void)
{
    if (movementv(UP)) return R_WMIST;
    if (movementv(NORTH)) return R_MAZEA42;
    if (movementv(EAST)) return R_MAZEA43;
    if (movementv(SOUTH)) return R_MAZEA45;
    if (movementv(WEST)) return R_MAZEA80;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea43(void)
{
    if (movementv(WEST)) return R_MAZEA42;
    if (movementv(SOUTH)) return R_MAZEA44;
    if (movementv(EAST)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea44(void)
{
    if (movementv(EAST)) return R_MAZEA43;
    if (movementv(DOWN)) return R_MAZEA48;
    if (movementv(SOUTH)) return R_MAZEA50;
    if (movementv(NORTH)) return R_MAZEA82;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea45(void)
{
    if (movementv(WEST)) return R_MAZEA42;
    if (movementv(NORTH)) return R_MAZEA43;
    if (movementv(EAST)) return R_MAZEA46;
    if (movementv(SOUTH)) return R_MAZEA47;
    if (movementv2(UP, DOWN)) return R_MAZEA87;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea46(void)
{
    if (movementv2(WEST, OUT)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea47(void)
{
    if (movementv2(EAST, OUT)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea48(void)
{
    if (movementv2(UP, OUT)) return R_MAZEA44;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea49(void)
{
    if (movementv(EAST)) return R_MAZEA50;
    if (movementv(WEST)) return R_MAZEA51;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea50(void)
{
    if (movementv(EAST)) return R_MAZEA44;
    if (movementv(WEST)) return R_MAZEA49;
    if (movementv(DOWN)) return R_MAZEA51;
    if (movementv(SOUTH)) return R_MAZEA52;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea51(void)
{
    if (movementv(WEST)) return R_MAZEA49;
    if (movementv(UP)) return R_MAZEA50;
    if (movementv(EAST)) return R_MAZEA52;
    if (movementv(SOUTH)) return R_MAZEA53;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea52(void)
{
    if (movementv(WEST)) return R_MAZEA50;
    if (movementv(EAST)) return R_MAZEA51;
    if (movementv(SOUTH)) return R_MAZEA52;
    if (movementv(UP)) return R_MAZEA53;
    if (movementv(NORTH)) return R_MAZEA55;
    if (movementv(DOWN)) return R_MAZEA86;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea53(void)
{
    if (movementv(WEST)) return R_MAZEA51;
    if (movementv(NORTH)) return R_MAZEA52;
    if (movementv(SOUTH)) return R_MAZEA54;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea54(void)
{
    if (movementv(WEST)) return R_MAZEA53;
    if (movementv(OUT)) return R_MAZEA53;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea55(void)
{
    if (movementv(WEST)) return R_MAZEA52;
    if (movementv(NORTH)) return R_MAZEA55;
    if (movementv(DOWN)) return R_MAZEA56;
    if (movementv(EAST)) return R_MAZEA57_PIT;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea56(void)
{
    if (movementv2(UP, OUT)) return R_MAZEA55;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea57_pit(void)
{
    if (movementv2(DOWN, CLIMB)) return R_BIRD;
    if (movementv(WEST)) return R_MAZEA55;
    if (movementv(SOUTH)) return R_MAZEA58;
    if (movementv(NORTH)) return R_MAZEA83;
    if (movementv(EAST)) return R_MAZEA84;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea58(void)
{
    if (movementv2(EAST, OUT)) return R_MAZEA57_PIT;
    return 0;  /* command hasn't been processed yet */
}

int at_elong(void)
{
    if (movementv3(EAST, UP, CRAWL)) return R_WMIST;
    if (movementv(WEST)) return R_WLONG;
    if (movementv3(NORTH, DOWN, HOLE)) return R_CROSS;
    return 0;  /* command hasn't been processed yet */
}

int at_wlong(void)
{
    if (movementv(EAST)) return R_ELONG;
    if (movementv(NORTH)) return R_CROSS;
    if (movementv(SOUTH)) return R_MAZED107;
    return 0;  /* command hasn't been processed yet */
}

int at_cross(void)
{
    if (movementv(WEST)) return R_ELONG;
    if (movementv(NORTH)) return R_DEADEND1;
    if (movementv(EAST)) return R_WEST;
    if (movementv(SOUTH)) return R_WLONG;
    return 0;  /* command hasn't been processed yet */
}

int at_deadend1(void)
{
    if (movementv2(SOUTH, OUT)) return R_CROSS;
    return 0;  /* command hasn't been processed yet */
}

int at_complex(void)
{
    if (movementv2(CLIMB, UP)) return R_DUSTY;
    if (movementv(WEST)) return R_BEDQUILT;
    if (movementp(R_BEDQUILT)) return R_BEDQUILT;
    if (movementv(NORTH)) return R_SHELL;
    if (movementp(R_SHELL)) return R_SHELL;
    if (movementv(EAST)) return R_ANTE;
    return 0;  /* command hasn't been processed yet */
}

int at_bedquilt(void)
{
    if (movementv(EAST)) return R_COMPLEX;
    if (movementv(WEST)) return R_SWISS;
    if (movementp(R_SLAB)) return R_SLAB;
    if (movementv2(NORTH, SOUTH) || movementv2(UP, DOWN)) {
        if (pct(65)) {
            puts("You have crawled around in some little holes and found your way" SOFT_NL
                 "blocked by a recent cave-in.  You are now back in the main passage.");
            return STAY_STILL;
        }
        if (movementv(DOWN)) return R_ANTE;
        if (movementv(NORTH)) return pct(75) ? R_LOW : R_SJUNC;
        if (movementv(UP)) return pct(75) ? R_DUSTY : R_ABOVEP;
        if (movementv(SOUTH)) return pct(75) ? R_SLAB : R_TALLEWCNYN;
    }
    if (keywordv(NW)) return pct(50) ? R_ORIENTAL : you_didnt_move();
    return 0;  /* command hasn't been processed yet */
}

int at_swiss(void)
{
    if (movementv(NE)) return R_BEDQUILT;
    if (movementv(WEST)) return R_E2PIT;
    if (movementp(R_AWK)) return R_TALLEWCNYN;
    if (movementv(EAST)) return R_SOFT;
    if (movementp(R_ORIENTAL)) return R_ORIENTAL;
    if (movementp(R_SOFT)) return R_SOFT;
    if (movementv(NW)) return pct(65) ? R_ORIENTAL : you_didnt_move();
    if (movementv(SOUTH)) return pct(65) ? R_TALLEWCNYN : you_didnt_move();
    return 0;  /* command hasn't been processed yet */
}

int at_e2pit(void)
{
    if (movementv(EAST)) return R_SWISS;
    if (movementv2(WEST, CROSS)) return R_W2PIT;
    if (movementv(DOWN)) return R_EPIT;
    if (movementp(R_SPIT)) return R_EPIT;
    return 0;  /* command hasn't been processed yet */
}

int at_slab(void)
{
    if (movementv(SOUTH)) return R_W2PIT;
    if (movementv2(UP, CLIMB)) return R_ABOVER;
    if (movementv(NORTH)) return R_BEDQUILT;
    if (movementp(R_BEDQUILT)) return R_BEDQUILT;
    return 0;  /* command hasn't been processed yet */
}

int at_abover(void)
{
    if (movementv(DOWN)) return R_SLAB;
    if (movementp(R_SLAB)) return R_SLAB;
    if (movementv(NORTH)) return R_MIRROR;
    if (keywordo(MIRROR)) return R_MIRROR;
    if (movementp(R_RES)) return R_RES;
    if (movementv(SOUTH)) return R_SECRETCYNNE1;
    return 0;  /* command hasn't been processed yet */
}

int at_abovep(void)
{
    if (movementv(NORTH)) return R_SJUNC;
    if (movementv2(DOWN, PASSAGE)) return R_BEDQUILT;
    if (movementv(SOUTH)) return R_STALACT;
    return 0;  /* command hasn't been processed yet */
}

int at_sjunc(void)
{
    if (movementv(SE)) return R_BEDQUILT;
    if (movementv(SOUTH)) return R_ABOVEP;
    if (movementv(NORTH)) return R_WINDOW;
    return 0;  /* command hasn't been processed yet */
}

int at_low(void)
{
    if (movementp(R_BEDQUILT)) return R_BEDQUILT;
    if (movementv(SW)) return R_SLOPING;
    if (movementv(NORTH)) return R_DEADEND2;
    if (movementv(SE)) return R_ORIENTAL;
    if (movementp(R_ORIENTAL)) return R_ORIENTAL;
    return 0;  /* command hasn't been processed yet */
}

int at_deadend2(void)
{
    if (movementv3(SOUTH, CRAWL, OUT)) return R_LOW;
    return 0;  /* command hasn't been processed yet */
}

int at_secretew_tite(void)
{
    if (movementv(EAST)) return R_HMK;
    if (movementv(DOWN)) return R_NSCANYONWIDE;
    if (keywordv(WEST)) {
        return objs[DRAGON].prop ? R_SECRETCYNNE1 : R_SECRETCYNNE2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_nscanyonwide(void)
{
    if (movementv(SOUTH)) return R_TIGHTERSTILL;
    if (movementv(NORTH)) return R_TALLEWCNYN;
    return 0;  /* command hasn't been processed yet */
}

int at_tighterstill(void)
{
    if (movementv(NORTH)) return R_NSCANYONWIDE;
    return 0;  /* command hasn't been processed yet */
}

int at_tallewcnyn(void)
{
    if (movementv(EAST)) return R_NSCANYONWIDE;
    if (movementv(WEST)) return R_DEADEND3;
    if (movementv2(NORTH, CRAWL)) return R_SWISS;
    return 0;  /* command hasn't been processed yet */
}

int at_deadend3(void)
{
    if (movementv(SOUTH)) return R_TALLEWCNYN;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea80(void)
{
    if (movementv(NORTH)) return R_MAZEA42;
    if (movementv(WEST)) return R_MAZEA80;
    if (movementv(SOUTH)) return R_MAZEA80;
    if (movementv(EAST)) return R_MAZEA81;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea81(void)
{
    if (movementv(WEST)) return R_MAZEA80;
    if (movementv(OUT)) return R_MAZEA80;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea82(void)
{
    if (movementv(SOUTH)) return R_MAZEA44;
    if (movementv(OUT)) return R_MAZEA44;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea83(void)
{
    if (movementv(SOUTH)) return R_MAZEA57_PIT;
    if (movementv(EAST)) return R_MAZEA84;
    if (movementv(WEST)) return R_MAZEA85;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea84(void)
{
    if (movementv(NORTH)) return R_MAZEA57_PIT;
    if (movementv(WEST)) return R_MAZEA83;
    if (movementv(NW)) return R_PIRATES_NEST;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea85(void)
{
    if (movementv(EAST)) return R_MAZEA83;
    if (movementv(OUT)) return R_MAZEA83;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea86(void)
{
    if (movementv(UP)) return R_MAZEA52;
    if (movementv(OUT)) return R_MAZEA52;
    return 0;  /* command hasn't been processed yet */
}

int at_mazea87(void)
{
    if (movementv(UP)) return R_MAZEA45;
    if (movementv(DOWN)) return R_MAZEA45;
    return 0;  /* command hasn't been processed yet */
}

int at_narrow(void)
{
    if (movementv(DOWN)) return R_WPIT;
    if (movementv(CLIMB)) return R_WPIT;
    if (movementv(EAST)) return R_WPIT;
    if (movementv(WEST)) return R_GIANT;
    if (movementp(R_GIANT)) return R_GIANT;
    if (keywordv(JUMP)) return splatter(R_WPIT);
    return 0;  /* command hasn't been processed yet */
}

int at_incline(void)
{
    if (movementv(NORTH)) return R_FALLS;
    if (movementp(R_FALLS)) return R_FALLS;
    if (movementv(PASSAGE)) return R_FALLS;
    if (movementv2(DOWN, CLIMB)) return oof(R_LOW);
    return 0;  /* command hasn't been processed yet */
}

int at_giant(void)
{
    if (movementv(SOUTH)) return R_NARROW;
    if (movementp(R_CORRIDOR)) return R_NARROW;
    if (movementv(EAST)) return R_TUNNEL_1;
    if (movementv(NORTH)) return R_IMMENSENSPASS;
    return 0;  /* command hasn't been processed yet */
}

int at_immensenspass(void)
{
    if (movementv2(SOUTH, PASSAGE)) return R_GIANT;
    if (movementp(R_GIANT)) return R_GIANT;
    if (movementv2(NORTH, IN) || movementp(R_FALLS)) {
        if (objs[DOOR].prop) return R_FALLS;
        puts("The door is extremely rusty and refuses to open.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_falls(void)
{
    if (movementv(SOUTH)) return R_IMMENSENSPASS;
    if (movementv(OUT)) return R_IMMENSENSPASS;
    if (movementp(R_GIANT)) return R_GIANT;
    if (movementv(WEST)) return R_INCLINE;
    if (movementp(R_INCLINE)) return R_INCLINE;
    if (movementv2(DOWN, JUMP)) {
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
                puts("You plunge into the water and are sucked down by the whirlpool.  The" SOFT_NL
                     "current is incredibly strong, and you barely manage to hold onto" SOFT_NL
                     "your lamp; everything else is pulled from your grasp and is lost in" SOFT_NL
                     "the swirling waters.");
            } else {
                puts("You plunge into the water and are sucked down by the whirlpool.");
            }
        } else {
            holding_count = 0;
            if (something_got_lost) {
                puts("You plunge into the water and are sucked down by the whirlpool into" SOFT_NL
                     "pitch darkness.  The current is incredibly strong, and everything that" SOFT_NL
                     "you are carrying is ripped from your grasp and is lost in the swirling" SOFT_NL
                     "waters.");
            } else {
                puts("You plunge into the water and are sucked down by the whirlpool into" SOFT_NL
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
    if (movementv2(WEST, OUT)) return R_SWISS;
    if (movementp(R_SWISS)) return R_SWISS;
    return 0;  /* command hasn't been processed yet */
}

int at_oriental(void)
{
    if (movementv(SE)) return R_SWISS;
    if (movementv(WEST)) return R_LOW;
    if (movementv(CRAWL)) return R_LOW;
    if (movementv(UP)) return R_MISTY;
    if (movementv(NORTH)) return R_MISTY;
    if (movementp(R_FALLS)) return R_MISTY;
    return 0;  /* command hasn't been processed yet */
}

int at_misty(void)
{
    if (movementv(SOUTH)) return R_ORIENTAL;
    if (movementp(R_ORIENTAL)) return R_ORIENTAL;
    if (movementv(WEST)) return R_ALCOVE;
    if (keywordv(JUMP)) return splatter(R_FALLS);
    return 0;  /* command hasn't been processed yet */
}

int tight_squeeze(void)
{
    puts("Something you're carrying won't fit through the tunnel with you." SOFT_NL
         "You'd best take inventory and drop something.");
    return STAY_STILL;
}

int at_alcove(void)
{
    if (movementv2(EAST, PASSAGE) || movementp(R_PLOVER)) {
        if (holding_count == 0) return R_PLOVER;
        if (holding_count == 1 && toting(EMERALD)) return R_PLOVER;
        return tight_squeeze();
    }
    if (movementv(NW)) return R_MISTY;
    if (movementp(R_FALLS)) return R_MISTY;
    return 0;  /* command hasn't been processed yet */
}

int at_plover(void)
{
    if (correctly_used_magic_word_PLOVER()) {
        if (toting(EMERALD)) apport(EMERALD, R_PLOVER);
        say_foof();
        return R_Y2;
    }
    if (movementv2(WEST, PASSAGE) || movementp(R_ALCOVE)) {
        if (holding_count == 0) return R_ALCOVE;
        if (holding_count == 1 && toting(EMERALD)) return R_ALCOVE;
        return tight_squeeze();
    }
    if (movementv(NE)) return R_DARK;
    if (movementp(R_DARK)) return R_DARK;
    return 0;  /* command hasn't been processed yet */
}

int at_dark(void)
{
    if (movementv2(SOUTH, OUT)) return R_PLOVER;
    if (movementp(R_PLOVER)) return R_PLOVER;
    return 0;  /* command hasn't been processed yet */
}

int at_arched(void)
{
    if (movementv(DOWN)) return R_SHELL;
    if (movementp(R_SHELL)) return R_SHELL;
    if (movementv(UP)) return R_ARCH_COR_1;
    if (movementv(EAST)) return R_ARCH_COR_1;
    return 0;  /* command hasn't been processed yet */
}

int at_shell(void)
{
    if (movementv(UP)) return R_ARCHED;
    if (movementv(HALL)) return R_ARCHED;
    if (movementv(DOWN)) return R_RAGGED;
    if (movementp(R_CORRIDOR)) return R_RAGGED;
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
    if (movementv(UP)) return R_SHELL;
    if (movementp(R_SHELL)) return R_SHELL;
    if (movementv(DOWN)) return R_CULDESAC;
    return 0;  /* command hasn't been processed yet */
}

int at_culdesac(void)
{
    if (movementv2(UP, OUT)) return R_RAGGED;
    if (movementp(R_CORRIDOR)) return R_RAGGED;
    if (movementp(R_SHELL)) return R_SHELL;
    return 0;  /* command hasn't been processed yet */
}

int at_anteroom(void)
{
    if (movementv(UP)) return R_COMPLEX;
    if (movementv(WEST)) return R_BEDQUILT;
    if (movementv(EAST)) return R_WITT;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed107(void)
{
    if (movementv(SOUTH)) return R_MAZED131;
    if (movementv(SW)) return R_MAZED132;
    if (movementv(NE)) return R_MAZED133;
    if (movementv(SE)) return R_MAZED134;
    if (movementv(UP)) return R_MAZED135;
    if (movementv(NW)) return R_MAZED136;
    if (movementv(EAST)) return R_MAZED137;
    if (movementv(WEST)) return R_MAZED138;
    if (movementv(NORTH)) return R_MAZED139;
    if (movementv(DOWN)) return R_WLONG;
    return 0;  /* command hasn't been processed yet */
}

int at_witt(void)
{
    if (movementv3(NORTH,SOUTH,UP) ||
        movementv3(EAST,DOWN,NE) ||
        movementv3(NW,SE,SW)) {
        return pct(95) ? R_WITT : R_ANTE;
    }
    if (movementv(WEST)) return R_WITT;
    return 0;  /* command hasn't been processed yet */
}

int at_mirror(void)
{
    if (keywordo(MIRROR)) {
        puts("It is too far up for you to reach.");
        return STAY_STILL;
    }
    if (movementv(SOUTH)) return R_ABOVER;
    if (movementv(NORTH)) return R_RES;
    if (movementp(R_RES)) return R_RES;
    return 0;  /* command hasn't been processed yet */
}

int at_window(void)
{
    if (movementv(WEST)) return R_SJUNC;
    if (keywordv(JUMP)) {
        puts("You are at the bottom of the pit with a broken neck.");
        return you_are_dead_at(R_MIRROR);
    }
    return 0;  /* command hasn't been processed yet */
}

int at_stalact(void)
{
    if (movementv(NORTH)) return R_ABOVEP;
    if (keywordv(DOWN) || keywordv(JUMP) || keywordv(CLIMB)) {
        return pct(40) ? R_MAZEA50 : pct(50) ? R_MAZEA53 : R_MAZEA45;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_mazed112(void)
{
    if (movementv(SW)) return R_MAZED131;
    if (movementv(NORTH)) return R_MAZED132;
    if (movementv(EAST)) return R_MAZED133;
    if (movementv(NW)) return R_MAZED134;
    if (movementv(SE)) return R_MAZED135;
    if (movementv(NE)) return R_MAZED136;
    if (movementv(WEST)) return R_MAZED137;
    if (movementv(DOWN)) return R_MAZED138;
    if (movementv(UP)) return R_MAZED139;
    if (movementv(SOUTH)) return R_PONY;
    return 0;  /* command hasn't been processed yet */
}

int cant_walk_on_water(void)
{
    puts("I can't swim, or walk on water.  You'll have to find some other way" SOFT_NL
         "to get across, or get someone to assist you.");
    return STAY_STILL;
}

int at_res(void)
{
    if (movementv2(SOUTH, OUT)) return R_MIRROR;
    if (movemento(MIRROR)) return R_MIRROR;
    if (keywordv(NORTH) || keywordv(CROSS)) {
        return cant_walk_on_water();
    }
    return 0;  /* command hasn't been processed yet */
}

int at_reservoir_n(void)
{
    if (movementv2(NORTH, PASSAGE)) return R_WARM;
    if (movementp(R_WARM)) return R_WARM;
    if (movementp(R_BALCONY)) return R_BALCONY;
    if (movementv2(SOUTH, CROSS)) {
        if (there(TURTLE, R_RESERVOIR_N)) {
            puts("You step gently on Darwin the Tortoise's back, and he carries you" SOFT_NL
                 "smoothly over to the southern side of the reservoir.  He then blows" SOFT_NL
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
    if (movementv(SOUTH)) return R_RESERVOIR_N;
    if (movementp(R_RES)) return R_RESERVOIR_N;
    if (movementv(NE)) return R_BALCONY;
    if (movementp(R_BALCONY)) return R_BALCONY;
    return 0;  /* command hasn't been processed yet */
}

int at_balcony(void)
{
    if (movementv2(WEST, OUT)) return R_WARM;
    if (movementp(R_WARM)) return R_WARM;
    if (movementp(R_RES)) return R_RESERVOIR_N;
    /* Jumping from the balcony will make your possessions unrecoverable. */
    if (keywordv(JUMP)) return splatter(R_YLEM);
    return 0;  /* command hasn't been processed yet */
}

int at_pirates_nest(void)
{
    if (movementv(SE)) return R_MAZEA84;
    return 0;  /* command hasn't been processed yet */
}

int at_swofchasm(void)
{
    if (movementv(SW)) return R_SLOPING;
    if (movementp(R_SLOPING)) return R_SLOPING;
    if (movementp(R_CORRIDOR)) return R_SLOPING;

    if (keywordv(THROW)) {
        if (word2.type == WordClass_None || there(TROLL2, R_SWOFCHASM))
            return 0;  /* unhandled */
        if (keywordo(AXE) || keywordo(SWORD)) {
            if (toting(word2.meaning)) {
                printf("The troll deftly catches the %s, examines it carefully, and tosses it" SOFT_NL
                       "back, declaring, \"Good workmanship, but it's not valuable enough.\"\n", word2.text);
                apport(word2.meaning, R_SWOFCHASM);
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        } else if (word2.type == WordClass_Object && is_treasure(word2.meaning)) {
            const ObjectWord o = word2.meaning;
            if (!toting(o)) return 0;  /* unhandled */
            if ((o == EGGS) && have_stolen_back_eggs) {
                puts("The troll nimbly steps to one side and grins nastily as the nest of" SOFT_NL
                     "golden eggs flies past him and plummets into the chasm.  \"Fool me" SOFT_NL
                     "once, shame on you; fool me twice, shame on me!\" he sneers.  \"I want" SOFT_NL
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
    if (movementv2(CROSS, NE)) {
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
    if (movementv(DOWN)) return R_LOW;
    if (movementv(UP)) return R_SWOFCHASM;
    if (movemento(CHASM)) return R_SWOFCHASM;
    if (movementp(R_LOW)) return R_LOW;
    if (movementv(OUT)) return R_LOW;
    return 0;  /* command hasn't been processed yet */
}

int at_secretcynne1(void)
{
    if (keywordo(RUG) && !objs[DRAGON].prop) {
        puts("You can't get by the dragon to get at the rug.");
        return STAY_STILL;
    }
    if (movementv(NORTH)) return R_ABOVER;
    if (movementv(OUT)) return R_ABOVER;
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
    if (movementv(EAST)) return R_SECRETEW_TITE;
    if (movementv(OUT)) return R_SECRETEW_TITE;
    if (movementv2(FORWARD, NORTH)) {
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
        puts("The bear lumbers toward the troll, who lets out a startled shriek and" SOFT_NL
             "scurries away.  The bear soon gives up the pursuit and wanders back.");
        apport(TROLL, R_LIMBO);
        apport(TROLL2, R_SWOFCHASM);
        apport(BEAR, R_NEOFCHASM);
        return STAY_STILL;
    }
    if (keywordv(THROW)) {
        if (word2.type == WordClass_None || there(TROLL2, R_NEOFCHASM))
            return 0;  /* unhandled */
        if (keywordo(AXE) || keywordo(SWORD)) {
            if (toting(word2.meaning)) {
                printf("The troll deftly catches the %s, examines it carefully, and tosses it" SOFT_NL
                       "back, declaring, \"Good workmanship, but it's not valuable enough.\"\n", word2.text);
                apport(word2.meaning, R_NEOFCHASM);
                return STAY_STILL;
            }
            return 0;  /* unhandled */
        } else if (word2.type == WordClass_Object && is_treasure(word2.meaning)) {
            const ObjectWord o = word2.meaning;
            if (!toting(o)) return 0;  /* unhandled */
            if ((o == EGGS) && have_stolen_back_eggs) {
                puts("The troll nimbly steps to one side and grins nastily as the nest of" SOFT_NL
                     "golden eggs flies past him and plummets into the chasm.  \"Fool me" SOFT_NL
                     "once, shame on you; fool me twice, shame on me!\" he sneers.  \"I want" SOFT_NL
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

    if (movementv2(CROSS, SW)) {
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
                puts("Just as you reach the other side, the bridge buckles beneath the" SOFT_NL
                     "weight of the bear, which was still following you around.  You" SOFT_NL
                     "scrabble desperately for support, but as the bridge collapses you" SOFT_NL
                     "stumble back and fall into the chasm.");
                objs[CHASM].prop = 1;
                apport(TROLL2, R_LIMBO);
                return you_are_dead_at(R_YLEM);
            }
            if (objs[TROLL].prop == 4 && have_stolen_back_eggs) {
                printf("As you reach the middle of the bridge, the troll appears from out" SOFT_NL
                       "of the tunnel behind you, wearing a large backpack.  \"So, Mister" SOFT_NL
                       "Magician,\" he shouts, \"you like to use magic to steal back my hard" HARD_HYPHEN
                       "earned toll?  Let's see how you like a little of MY magic!!\"  With" SOFT_NL);
                if (there(BEAR, R_NEOFCHASM)) {
                    puts("that, he aims a tube running from the backpack directly at the bear" SOFT_NL
                         "and pulls a trigger.  A spout of magical fire roars out and singes the" SOFT_NL
                         "bear's fur; the bear bellows in pain and dashes onto the bridge to" SOFT_NL
                         "escape.  The bridge shudders, groans, and collapses under the weight," SOFT_NL
                         "and you and the bear plunge down into the chasm.");
                    apport(BEAR, R_YLEM);
                    objs[CHASM].prop = 1;
                } else {
                    puts("that, he aims a tube running from the backpack directly at the bridge" SOFT_NL
                         "and pulls a trigger.  A spout of magical fire roars out and incinerates" SOFT_NL
                         "the bridge supports, causing the bridge to sway giddily and collapse" SOFT_NL
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
    if (movementv(NE)) return R_CORRIDOR;
    if (movementp(R_CORRIDOR)) return R_CORRIDOR;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv(VIEW)) return R_VIEW;
    if (movementp(R_FBARR)) return R_FBARR;
    return 0;  /* command hasn't been processed yet */
}

int at_corridor(void)
{
    if (movemento(CHASM)) return R_NEOFCHASM;
    if (movementv(WEST)) return R_NEOFCHASM;
    if (movementv(EAST)) return R_FORK;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv(VIEW)) return R_VIEW;
    if (movementp(R_FBARR)) return R_FBARR;
    return 0;  /* command hasn't been processed yet */
}

int at_fork(void)
{
    if (movemento(CHASM)) return R_NEOFCHASM;
    if (movementv(WEST)) return R_CORRIDOR;
    if (movementp(R_CORRIDOR)) return R_CORRIDOR;
    if (movementv(NE)) return R_WARMJUNCTN;
    if (movementv(LEFT)) return R_WARMJUNCTN;
    if (movementv(SE)) return R_LIME;
    if (movementv(RIGHT)) return R_LIME;
    if (movementv(DOWN)) return R_LIME;
    if (movementv(VIEW)) return R_VIEW;
    if (movementp(R_FBARR)) return R_FBARR;
    return 0;  /* command hasn't been processed yet */
}

int at_warmjunctn(void)
{
    if (movementv(SOUTH)) return R_FORK;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv(NORTH)) return R_VIEW;
    if (movementv(VIEW)) return R_VIEW;
    if (movementv2(EAST, CRAWL)) return R_CHAMBER;
    return 0;  /* command hasn't been processed yet */
}

int at_view(void)
{
    if (movementv(SOUTH)) return R_WARMJUNCTN;
    if (movementv(PASSAGE)) return R_WARMJUNCTN;
    if (movementv(OUT)) return R_WARMJUNCTN;
    if (movementp(R_FORK)) return R_FORK;
    if (keywordv(JUMP) || keywordv(DOWN) || keywordv(CLIMB))
        return splatter(R_YLEM);
    if (keywordp(R_VALLEY) || keywordv(CROSS) || keywordo(GORGE) || keywordv(NORTH)) {
        if (objs[GORGE].prop == 0) {
            puts("I'm afraid I can't go that way " EMDASH("-") " walking on red-hot lava is contrary" SOFT_NL
                 "to union regulations (and is bad for your health anyhow).");
            return STAY_STILL;
        } else if (toting(RING)) {
            if (toting(BEAR)) {
                puts("As you approach the center of the archway, hot vapors saturated with" SOFT_NL
                     "brimstone drift up from the lava in the gorge beneath your feet.  The" SOFT_NL
                     "mithril ring in your hand quivers and glows, and a swirling vortex" SOFT_NL
                     "of white vapor encircles you and protects you from the fumes.  The" SOFT_NL
                     "bear is not so lucky, though, since the vortex is rather small; the" SOFT_NL
                     "noxious gasses choke it and it staggers off of the side of the arch" SOFT_NL
                     "and plummets into the gorge below.\n");
                apport(BEAR, R_YLEM);
            } else if (!(places[R_FACES].flags & F_BEENHERE)) {
                puts("As you approach the center of the archway, hot vapors saturated with" SOFT_NL
                     "brimstone drift up from the lava in the gorge beneath your feet.  The" SOFT_NL
                     "mithril ring in your hand quivers and glows, and the fumes eddy away" SOFT_NL
                     "from the bridge without harming you.\n");
            }
            return R_FACES;
        } else {
            if (toting(BEAR)) {
                puts("As you approach the center of the archway, hot vapors saturated with" SOFT_NL
                     "brimstone drift up from the lava in the gorge beneath your feet.  You" SOFT_NL
                     "and the bear are both overcome by the noxious gasses and, with your" SOFT_NL
                     "lungs burned out, slip off of the bridge and plummet into the gorge.");
                /* Notice that the bear winds up at R_YLEM with the
                 * rest of your possessions, in this case. */
            } else {
                puts("As you approach the center of the archway, hot vapors saturated with" SOFT_NL
                     "brimstone drift up from the lava in the gorge beneath your feet.  You" SOFT_NL
                     "are swiftly overcome by the foul gasses and, with your lungs burned" SOFT_NL
                     "out, fall off of the bridge and into the gorge.");
            }
            return you_are_dead_at(R_YLEM);
        }
    }
    return 0;  /* command hasn't been processed yet */
}

int at_chamber(void)
{
    if (movementv(WEST)) return R_WARMJUNCTN;
    if (movementv(OUT)) return R_WARMJUNCTN;
    if (movementv(CRAWL)) return R_WARMJUNCTN;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_lime(void)
{
    if (movementv(NORTH)) return R_FORK;
    if (movementv(UP)) return R_FORK;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv(SOUTH)) return R_FBARR;
    if (movementv(DOWN)) return R_FBARR;
    if (movementp(R_FBARR)) return R_FBARR;
    if (movementv(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_fbarr(void)
{
    if (movementv(WEST)) return R_LIME;
    if (movementv(UP)) return R_LIME;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv2(EAST, IN)) return R_BARR;
    if (movementp(R_FBARR)) return R_BARR;
    if (movementv(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_barr(void)
{
    if (keywordv(GET) && keywordo(AXE) && there(AXE, R_BARR) && objs[AXE].prop) {
        puts("As you approach the bear, it snarls threateningly; you are forced" SOFT_NL
             "to retreat without the axe.");
        return STAY_STILL;
    }
    if (movementv(WEST)) return R_FBARR;
    if (movementv(OUT)) return R_FBARR;
    if (movementp(R_FORK)) return R_FORK;
    if (movementv(VIEW)) return R_VIEW;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed131(void)
{
    if (movementv(WEST)) return R_MAZED107;
    if (movementv(SE)) return R_MAZED132;
    if (movementv(NW)) return R_MAZED133;
    if (movementv(SW)) return R_MAZED134;
    if (movementv(NE)) return R_MAZED135;
    if (movementv(UP)) return R_MAZED136;
    if (movementv(DOWN)) return R_MAZED137;
    if (movementv(NORTH)) return R_MAZED138;
    if (movementv(SOUTH)) return R_MAZED139;
    if (movementv(EAST)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed132(void)
{
    if (movementv(NW)) return R_MAZED107;
    if (movementv(UP)) return R_MAZED131;
    if (movementv(NORTH)) return R_MAZED133;
    if (movementv(SOUTH)) return R_MAZED134;
    if (movementv(WEST)) return R_MAZED135;
    if (movementv(SW)) return R_MAZED136;
    if (movementv(NE)) return R_MAZED137;
    if (movementv(EAST)) return R_MAZED138;
    if (movementv(DOWN)) return R_MAZED139;
    if (movementv(SE)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed133(void)
{
    if (movementv(UP)) return R_MAZED107;
    if (movementv(DOWN)) return R_MAZED131;
    if (movementv(WEST)) return R_MAZED132;
    if (movementv(NE)) return R_MAZED134;
    if (movementv(SW)) return R_MAZED135;
    if (movementv(EAST)) return R_MAZED136;
    if (movementv(NORTH)) return R_MAZED137;
    if (movementv(NW)) return R_MAZED138;
    if (movementv(SE)) return R_MAZED139;
    if (movementv(SOUTH)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed134(void)
{
    if (movementv(NE)) return R_MAZED107;
    if (movementv(NORTH)) return R_MAZED131;
    if (movementv(NW)) return R_MAZED132;
    if (movementv(SE)) return R_MAZED133;
    if (movementv(EAST)) return R_MAZED135;
    if (movementv(DOWN)) return R_MAZED136;
    if (movementv(SOUTH)) return R_MAZED137;
    if (movementv(UP)) return R_MAZED138;
    if (movementv(WEST)) return R_MAZED139;
    if (movementv(SW)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed135(void)
{
    if (movementv(NORTH)) return R_MAZED107;
    if (movementv(SE)) return R_MAZED131;
    if (movementv(DOWN)) return R_MAZED132;
    if (movementv(SOUTH)) return R_MAZED133;
    if (movementv(EAST)) return R_MAZED134;
    if (movementv(WEST)) return R_MAZED136;
    if (movementv(SW)) return R_MAZED137;
    if (movementv(NE)) return R_MAZED138;
    if (movementv(NW)) return R_MAZED139;
    if (movementv(UP)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed136(void)
{
    if (movementv(EAST)) return R_MAZED107;
    if (movementv(WEST)) return R_MAZED131;
    if (movementv(UP)) return R_MAZED132;
    if (movementv(SW)) return R_MAZED133;
    if (movementv(DOWN)) return R_MAZED134;
    if (movementv(SOUTH)) return R_MAZED135;
    if (movementv(NW)) return R_MAZED137;
    if (movementv(SE)) return R_MAZED138;
    if (movementv(NE)) return R_MAZED139;
    if (movementv(NORTH)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed137(void)
{
    if (movementv(SE)) return R_MAZED107;
    if (movementv(NE)) return R_MAZED131;
    if (movementv(SOUTH)) return R_MAZED132;
    if (movementv(DOWN)) return R_MAZED133;
    if (movementv(UP)) return R_MAZED134;
    if (movementv(NW)) return R_MAZED135;
    if (movementv(NORTH)) return R_MAZED136;
    if (movementv(SW)) return R_MAZED138;
    if (movementv(EAST)) return R_MAZED139;
    if (movementv(WEST)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed138(void)
{
    if (movementv(DOWN)) return R_MAZED107;
    if (movementv(EAST)) return R_MAZED131;
    if (movementv(NE)) return R_MAZED132;
    if (movementv(UP)) return R_MAZED133;
    if (movementv(WEST)) return R_MAZED134;
    if (movementv(NORTH)) return R_MAZED135;
    if (movementv(SOUTH)) return R_MAZED136;
    if (movementv(SE)) return R_MAZED137;
    if (movementv(SW)) return R_MAZED139;
    if (movementv(NW)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_mazed139(void)
{
    if (movementv(SW)) return R_MAZED107;
    if (movementv(NW)) return R_MAZED131;
    if (movementv(EAST)) return R_MAZED132;
    if (movementv(WEST)) return R_MAZED133;
    if (movementv(NORTH)) return R_MAZED134;
    if (movementv(DOWN)) return R_MAZED135;
    if (movementv(SE)) return R_MAZED136;
    if (movementv(UP)) return R_MAZED137;
    if (movementv(SOUTH)) return R_MAZED138;
    if (movementv(NE)) return R_MAZED112;
    return 0;  /* command hasn't been processed yet */
}

int at_pony(void)
{
    if (movementv(NORTH)) return R_MAZED112;
    if (movementv(OUT)) return R_MAZED112;
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
    if (movementv2(WEST, OUT)) return R_EMIST;
    if (movementp(R_EMIST)) return R_EMIST;
    return 0;  /* command hasn't been processed yet */
}

int at_morion(void)
{
    if (movementv2(SOUTH, OUT)) return R_HMK;
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

int safes_door(Location loc)
{
    if (objs[SAFE].prop == 1) {
	puts("The safe's door is blocking the exit passage " EMDASH("-") " you'll have to close" SOFT_NL
	     "the safe to get out of here.");
	return STAY_STILL;
    } else {
	return loc;
    }
}

int at_vault(void)
{
    if (movementv3(UP,OUT,NORTH)) {
	return safes_door(R_HMK);
    }
    if (keywordv(IN)) return enter_safe(R_VAULT);
    return 0;  /* command hasn't been processed yet */
}

int at_insafe(void)
{
    if (keywordv(OUT)) return safe_exit;
    if (keywordo(SAFE)) {
        if (word2.type == WordClass_None) {
            puts("What do you want me to do with the safe?");
        } else if (keywordv(CLOSE)) {
            puts("There is no handle on the inside of the safe door, nor any other way" SOFT_NL
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
    if (movementv(SOUTH)) return R_HMK;
    if (movementv(NORTH)) return R_CORRID_2;
    return 0;  /* command hasn't been processed yet */
}

int at_corrid_2(void)
{
    if (movementv(SOUTH)) return R_CORRID_1;
    if (movementv(WEST)) return R_CORRID_1;
    if (movementv2(NORTH, EAST)) return R_TOOL;
    return 0;  /* command hasn't been processed yet */
}

int at_tool(void)
{
    if (movementv2(OUT, SOUTH)) return R_CORRID_2;
    return 0;  /* command hasn't been processed yet */
}

int at_corrid_3(void)
{
    if (movementv(SOUTH)) return R_HMK;
    if (movementv(NORTH)) return R_SPHERICAL;
    if (movementv(EAST)) return R_CUBICLE;
    return 0;  /* command hasn't been processed yet */
}

int at_cubicle(void)
{
    if (movementv(OUT)) return R_CORRID_3;
    if (movementp(R_CORRIDOR)) return R_CORRID_3;
    if (movementv(SOUTH)) return R_CORRID_3;
    return 0;  /* command hasn't been processed yet */
}

int at_spherical(void)
{
    if (movementv(OUT)) return R_CORRID_3;
    if (movementv(NORTH)) return R_CORRID_3;
    if (movementp(R_CORRIDOR)) return R_CORRID_3;
    return 0;  /* command hasn't been processed yet */
}

int at_tunnel_1(void)
{
    if (movementv(SOUTH)) return R_GIANT;
    if (movementv(NORTH)) return R_GLASSY;
    return 0;  /* command hasn't been processed yet */
}

int at_glassy(void)
{
    if (movementv(SOUTH)) return R_TUNNEL_1;
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
    if (movementv(EAST)) return R_BRINK_1;
    if (movementv(WEST)) return R_GLASSY;
    return 0;  /* command hasn't been processed yet */
}

int at_brink_1(void)
{
    if (movementv(NORTH)) return R_LAIR;
    if (movementv(WEST)) return R_BRINK_2;
    if (movementv(EAST)) return R_BRINK_3;
    if (keywordv(JUMP)) return plunge();
    return 0;  /* command hasn't been processed yet */
}

int at_brink_2(void)
{
    if (movementv(NORTH)) return R_BRINK_1;
    if (movementv(SE)) return R_ICE;
    if (keywordv(JUMP)) return plunge();
    return 0;  /* command hasn't been processed yet */
}

int at_ice(void)
{
    if (movementv(NW)) return R_BRINK_2;
    if (movementv2(DOWN, EAST)) return oof(R_SLIDE);
    if (movementp(R_SLIDE)) return oof(R_SLIDE);
    return 0;  /* command hasn't been processed yet */
}

int at_slide(void)
{
    if (movementv3(UP, NORTH, CLIMB)) {
        puts("The icy slide is far too steep and slippery to climb.");
        return STAY_STILL;
    }
    if (movementv(SOUTH)) return R_ICECAVE2A;
    if (movementv(NW)) return R_ICECAVE4;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave1(void)
{
    if (movementv(WEST)) return R_ICECAVE2;
    if (movementv(NORTH)) return R_ICECAVE1A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave1a(void)
{
    if (movementv(SOUTH)) return R_ICECAVE1;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave2(void)
{
    if (movementv(EAST)) return R_ICECAVE1;
    if (movementv(WEST)) return R_ICECAVE3;
    if (movementv(NORTH)) return R_ICECAVE2A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave2a(void)
{
    if (movementv(NORTH)) return R_SLIDE;
    if (movementv(SOUTH)) return R_ICECAVE2;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave3(void)
{
    if (movementv(EAST)) return R_ICECAVE2;
    if (movementv(NORTH)) return R_ICECAVE3A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave3a(void)
{
    if (movementv(SOUTH)) return R_ICECAVE3;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave4(void)
{
    if (movementv(EAST)) return R_SLIDE;
    if (movementv(WEST)) return R_ICECAVE5;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave5(void)
{
    if (movementv(NE)) return R_ICECAVE4;
    if (movementv(SOUTH)) return R_ICECAVE6;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave6(void)
{
    if (movementv(NORTH)) return R_ICECAVE5;
    if (movementv(SOUTH)) return R_ICECAVE7;
    if (movementv(WEST)) return R_ICECAVE9;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave7(void)
{
    if (movementv(NORTH)) return R_ICECAVE6;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave8(void)
{
    if (movementv(NORTH)) return R_ICECAVE9;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave9(void)
{
    if (movementv(EAST)) return R_ICECAVE6;
    if (movementv(SOUTH)) return R_ICECAVE8;
    if (movementv(NORTH)) return R_ICECAVE10;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave10(void)
{
    if (movementv(SOUTH)) return R_ICECAVE9;
    if (movementv(NW)) return R_ICECAVE11;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave11(void)
{
    if (movementv(EAST)) return R_ICECAVE10;
    if (movementv(WEST)) return R_ICECAVE12;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave12(void)
{
    if (movementv(NE)) return R_ICECAVE11;
    if (movementv(SOUTH)) return R_ICECAVE12A;
    if (movementv(WEST)) return R_ICECAVE15;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave12a(void)
{
    if (movementv(NORTH)) return R_ICECAVE12;
    if (movementv(SOUTH)) return R_ICECAVE13;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave13(void)
{
    if (movementv(NORTH)) return R_ICECAVE12A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave14(void)
{
    if (movementv(NORTH)) return R_ICECAVE15A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave15(void)
{
    if (movementv(EAST)) return R_ICECAVE12;
    if (movementv(SOUTH)) return R_ICECAVE15A;
    if (movementv(NW)) return R_ICECAVE16;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave15a(void)
{
    if (movementv(SOUTH)) return R_ICECAVE14;
    if (movementv(NORTH)) return R_ICECAVE15;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave16(void)
{
    if (movementv(EAST)) return R_ICECAVE15;
    if (movementv(WEST)) return R_ICECAVE17;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave17(void)
{
    if (movementv(NE)) return R_ICECAVE16;
    if (movementv(SOUTH)) return R_ICECAVE18;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave18(void)
{
    if (movementv(NORTH)) return R_ICECAVE17;
    if (movementv(SOUTH)) return R_ICECAVE19;
    if (movementv(WEST)) return R_ICECAVE21;
    if (movementv(NW)) return R_ICECAVE22;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave19(void)
{
    if (movementv(NORTH)) return R_ICECAVE18;
    if (movementv(WEST)) return R_ICECAVE20;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave20(void)
{
    if (movementv(EAST)) return R_ICECAVE19;
    if (movementv(NORTH)) return R_ICECAVE21;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave21(void)
{
    if (movementv(EAST)) return R_ICECAVE18;
    if (movementv(SOUTH)) return R_ICECAVE20;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave22(void)
{
    if (movementv(SE)) return R_ICECAVE18;
    if (movementv(NW)) return R_ICECAVE23;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave23(void)
{
    if (movementv(EAST)) return R_ICECAVE22;
    if (movementv(WEST)) return R_ICECAVE24;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave24(void)
{
    if (movementv(NE)) return R_ICECAVE23;
    if (movementv(SOUTH)) return R_ICECAVE25;
    if (movementv(WEST)) return R_ICECAVE29;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave25(void)
{
    if (movementv(NORTH)) return R_ICECAVE24;
    if (movementv(SOUTH)) return R_ICECAVE26;
    if (movementv(WEST)) return R_ICECAVE28;
    if (movementv(NW)) return R_ICECAVE28A;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave26(void)
{
    if (movementv(NORTH)) return R_ICECAVE25;
    if (movementv(NW)) return R_ICECAVE27;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave27(void)
{
    if (movementv(SE)) return R_ICECAVE26;
    if (movementv(NORTH)) return R_ICECAVE28;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave28(void)
{
    if (movementv(EAST)) return R_ICECAVE25;
    if (movementv(SOUTH)) return R_ICECAVE27;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave28a(void)
{
    if (movementv(SE)) return R_ICECAVE25;
    if (movementv(NORTH)) return R_ICECAVE29;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave29(void)
{
    if (movementv(EAST)) return R_ICECAVE24;
    if (movementv(SOUTH)) return R_ICECAVE28A;
    if (movementv(NW)) return R_ICECAVE30;
    return 0;  /* command hasn't been processed yet */
}

int at_icecave30(void)
{
    if (movementv(EAST)) return R_ICECAVE29;
    if (correctly_used_magic_word(THURB)) {
        say_foof();
        return R_ICE;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_brink_3(void)
{
    if (movementv(NORTH)) return R_BRINK_1;
    if (movementv(NE)) return R_CRACK_1;
    if (movementv(CRACK)) return R_CRACK_1;
    if (keywordv(JUMP)) return plunge();
    return 0;  /* command hasn't been processed yet */
}

int at_crack_1(void)
{
    if (movementv(SW)) return R_BRINK_3;
    if (movementv(SE)) return R_CRACK_2;
    return 0;  /* command hasn't been processed yet */
}

int at_crack_2(void)
{
    if (movementv(WEST)) return R_CRACK_1;
    if (keywordv(SOUTH)) {
        if (there(SLIME, R_CRACK_2)) {
            puts("As you enter into the passage, you are forced to brush up against" SOFT_NL
                 "some of the green slime.  Instantly it flows down and covers your" SOFT_NL
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
    if (movementv(NORTH)) return R_CRACK_2;
    if (movementv2(SOUTH, CRAWL)) return R_CRACK_4;
    return 0;  /* command hasn't been processed yet */
}

int at_crack_4(void)
{
    if (movementv3(NORTH, OUT, CRAWL)) return R_CRACK_3;
    return 0;  /* command hasn't been processed yet */
}

static int quicksand(Location dest)
{
    static bool have_drowned_in_quicksand = false;
    if (!objs[QUICKSAND].prop || toting(CLAM) || toting(OYSTER)) {
        objs[QUICKSAND].prop = 0;
        if (have_drowned_in_quicksand) {
            puts("You know, I've heard of people who really fell in for the soft sell," SOFT_NL
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
            puts("Hmmmm..  This sand is rather soft, and you're sinking in a little..." SOFT_NL
                 "In fact you're sinking in a lot!   Oh, no " EMDASH("-") " it's QUICKSAND!!  HELP!!" SOFT_NL
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
    if (movementv(WEST)) return R_ARCHED;
    if (keywordv(EAST)) return quicksand(R_ARCH_COR_2);
    return 0;  /* command hasn't been processed yet */
}

int at_arch_cor_2(void)
{
    if (movementv(NORTH)) return R_ARCH_FORK;
    if (keywordv(WEST)) return quicksand(R_ARCH_COR_1);
    return 0;  /* command hasn't been processed yet */
}

int at_arch_fork(void)
{
    if (movementv(SOUTH)) return R_ARCH_COR_2;
    if (movementv(NORTH)) return R_FOURIER;
    if (movementv(EAST)) return R_JONAH;
    if (movementp(R_JONAH)) return R_JONAH;
    if (movementp(R_FOURIER)) return R_FOURIER;
    return 0;  /* command hasn't been processed yet */
}

int at_fourier(void)
{
    if (movementv(NW)) return R_ARCH_FORK;
    if (movementv(SW)) return R_SHELF;
    return 0;  /* command hasn't been processed yet */
}

int at_shelf(void)
{
    if (movementv(WEST)) return R_FOURIER;
    if (movementv(DOWN)) return R_BEACH;
    if (movemento(STEPS)) return R_BEACH;
    return 0;  /* command hasn't been processed yet */
}

int at_beach(void)
{
    if (movementv2(UP, WEST) || keywordp(R_SHELF) || keywordo(STEPS)) {
        objs[DINGHY].prop = 1;
        return R_SHELF;
    }
    if (keywordo(WATER) || keywordv(FILL) || keywordv(DRINK)) {
        puts("I'm afraid that all that's available here is salt water, which" SOFT_NL
             /* Platt has "you'de". */
             "isn't good for anything much... you'd better try elsewhere.");
        return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_jonah(void)
{
    if (movementv(SOUTH)) return R_IN_JONAH;
    if (movementv(WEST)) return R_ARCH_FORK;
    return 0;  /* command hasn't been processed yet */
}

int at_in_jonah(void)
{
    if (movementv2(NORTH, OUT)) return R_JONAH;
    return 0;  /* command hasn't been processed yet */
}

int at_faces(void)
{
    if (movementv(NORTH)) return R_BY_FIGURE;
    if (keywordv(CROSS) || keywordv(SOUTH) || keywordo(GORGE)) {
        if (!objs[GORGE].prop) {
            puts("I'm afraid I can't go that way " EMDASH("-") " walking on red-hot lava is contrary" SOFT_NL
                 "to union regulations (and is bad for your health anyhow).");
            return STAY_STILL;
        } else if (!toting(RING)) {
            puts("As you approach the center of the archway, hot vapors saturated with" SOFT_NL
                 "brimstone drift up from the lava in the gorge beneath your feet.  You" SOFT_NL
                 "are swiftly overcome by the foul gasses and, with your lungs burned" SOFT_NL
                 "out, fall off of the bridge and into the gorge.");
            return you_are_dead_at(R_YLEM);
        } else if (toting(SCEPTRE)) {
            puts("As you reach the center of the bridge, a ghostly figure appears in" SOFT_NL
                 "front of you.  He (?) stands at least eight feet tall, and has the" SOFT_NL
                 "lower body of an enormous snake, six arms, and an angry expression on" SOFT_NL
                 "his face.  \"You'll not have my sceptre that easily!\" he cries, and" SOFT_NL
                 "makes a complex magical gesture with his lower right arm.  There is a" SOFT_NL
                 "brilliant flash of light and a vicious >crack<, and the bridge cracks" SOFT_NL
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
    if (movementv(SOUTH)) return R_FACES;
    if (objs[STATUE].prop) {
        /* The statue has moved, revealing dark passages. */
       if (movementv(NW)) return R_PLAIN_1;
       if (movementv(NORTH)) return R_BASQUE_1;
       if (movementv(NE)) return R_BANSHEE;
    } else if (correctly_used_magic_word(MELENKURION)) {
        puts("Rock silently crumbles off of the wall in front of you, revealing" SOFT_NL
             "dark passages leading northwest, north, and northeast.");
        objs[STATUE].prop = 1;
	return STAY_STILL;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_plain_1(void)
{
    if (movementv(SOUTH)) return R_BY_FIGURE;
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
    if (movementv(NORTH)) return R_PENTAGRAM;
    if (movementp(R_PENTAGRAM)) return R_PENTAGRAM;
    if (movementv2(UP, SOUTH)) {
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
    if (movementv2(WEST, OUT)) return R_NONDESCRIPT;
    if (movementp(R_NONDESCRIPT)) return R_NONDESCRIPT;
    if (movementv2(NORTH, CRACK)) return R_CHIMNEY;
    if (movementp(R_CHIMNEY)) return R_CHIMNEY;
    return 0;  /* command hasn't been processed yet */
}

int at_chimney(void)
{
    if (movementv2(UP, CLIMB)) return R_TUBE;
    if (movementp(R_TUBE)) return R_TUBE;
    if (movementv(SOUTH)) return R_PENTAGRAM;
    if (movementp(R_PENTAGRAM)) return R_PENTAGRAM;
    if (movementv(CRACK)) return R_PENTAGRAM;
    return 0;  /* command hasn't been processed yet */
}

int at_tube(void)
{
    if (movementv(DOWN)) return R_CHIMNEY;
    if (movementp(R_CHIMNEY)) return R_CHIMNEY;
    if (movementv(CLIMB)) return R_CHIMNEY;
    if (movementp(R_TUBE)) return R_TUBE_SLIDE;
    if (movementp(R_SLIDE)) return R_TUBE_SLIDE;
    if (movementv(SOUTH)) return R_TUBE_SLIDE;
    return 0;  /* command hasn't been processed yet */
}

int at_tube_slide(void)
{
    if (movementv2(SOUTH, DOWN)) return oof(R_PLAIN_1);
    if (movementp(R_SLIDE)) return oof(R_PLAIN_1);
    if (movementv(NORTH)) return R_TUBE;
    if (movementp(R_CHIMNEY)) return R_CHIMNEY;
    if (movementp(R_TUBE)) return R_CHIMNEY;
    return 0;  /* command hasn't been processed yet */
}

int at_basque_1(void)
{
    if (movementv(SOUTH)) return R_BY_FIGURE;
    if (keywordv(NORTH)) {
        objs[BASILISK].prop += 1;
        if (objs[BASILISK].prop == 1) {
            puts("The basilisk stirs restlessly and grumbles in its sleep as you pass," SOFT_NL
                 "but it does not awaken.\n");
        }
        return R_BASQUE_2;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_basque_2(void)
{
    if (movementv(NORTH)) return R_BASQUE_FORK;
    if (keywordv(SOUTH)) {
        objs[BASILISK].prop -= 1;
        if (objs[BASILISK].prop == 0) {
            if (toting(PLATE)) {
                puts("The basilisk stirs grumpily and awakens, peering sleepily about.  It" SOFT_NL
                     "sees its reflection in the metal plate that you are carrying," SOFT_NL
                     "shudders, and turns into solid granite.\n");
                objs[BASILISK].prop = 2;
            } else {
                puts("The basilisk stirs grumpily and awakens, peering sleepily about.  It" SOFT_NL
                     "spies you, growls, and stares you straight in the eye.  Your body" SOFT_NL
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
    if (movementv(NORTH)) return R_PEELGRUNT;
    if (movementp(R_PEELGRUNT)) return R_PEELGRUNT;
    if (movementv(SOUTH)) return R_BASQUE_2;
    if (movementv(DOWN)) return R_ON_STEPS;
    if (movemento(STEPS)) return R_ON_STEPS;
    return 0;  /* command hasn't been processed yet */
}

int at_peelgrunt(void)
{
    if (keywordv(SOUTH) || keywordv(OUT) || keywordp(R_FORK)) {
	return safes_door(R_BASQUE_FORK);
    }
    if (keywordv(IN)) return enter_safe(R_PEELGRUNT);
    return 0;  /* command hasn't been processed yet */
}

int at_on_steps(void)
{
    if (movementv(UP)) return R_BASQUE_FORK;
    if (movementv(DOWN)) return R_STEPS_EXIT;
    if (movemento(STEPS)) return R_STEPS_EXIT;
    return 0;  /* command hasn't been processed yet */
}

int at_steps_exit(void)
{
    if (movementv(UP)) return R_ON_STEPS;
    if (movementv(DOWN)) return R_STORAGE;
    if (movementv(NORTH)) return R_FAKE_Y2;
    if (movemento(STEPS)) return R_STORAGE;
    if (movementv(OUT)) return R_FAKE_Y2;
    return 0;  /* command hasn't been processed yet */
}

int at_storage(void)
{
    if (movementv(UP)) return R_STEPS_EXIT;
    if (movemento(STEPS)) return R_STEPS_EXIT;
    return 0;  /* command hasn't been processed yet */
}

int at_fake_y2(void)
{
    if (movementv(SOUTH)) return R_STEPS_EXIT;
    if (movementv(WEST)) return R_CATACOMBS1;
    if (movementv(EAST)) return R_FAKE_JUMBLE;
    if (correctly_used_magic_word(PLUGH) || correctly_used_magic_word_PLOVER()) {
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
    if (movementv2(DOWN, WEST)) return R_FAKE_Y2;
    if (movementv(UP)) return R_CATACOMBS1;
    return 0;  /* command hasn't been processed yet */
}

int at_audience(void)
{
    if (movementv(EAST)) return R_AUDIENCE_E;
    if (movementv(WEST)) return R_CATACOMBS11;
    return 0;  /* command hasn't been processed yet */
}

int at_audience_e(void)
{
    if (movementv(WEST)) return R_AUDIENCE;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs1(void)
{
    if (movementv(SOUTH)) return R_CATACOMBS2;
    if (movementv3(NORTH,NW,WEST)) return R_CATACOMBS1;
    if (movementv3(SW,SE,DOWN)) return R_CATACOMBS1;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS1;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs2(void)
{
    if (movementv(SW)) return R_CATACOMBS3;
    if (movementv3(NORTH,NW,WEST)) return R_CATACOMBS1;
    if (movementv3(SOUTH,SE,DOWN)) return R_CATACOMBS1;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS1;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs3(void)
{
    if (movementv(NW)) return R_CATACOMBS4;
    if (movementv3(NORTH,SW,WEST)) return R_CATACOMBS2;
    if (movementv3(SOUTH,SE,DOWN)) return R_CATACOMBS2;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS2;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs4(void)
{
    if (movementv(SOUTH)) return R_CATACOMBS5;
    if (movementv3(NORTH,SW,WEST)) return R_CATACOMBS3;
    if (movementv3(NW,SE,DOWN)) return R_CATACOMBS3;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS3;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs5(void)
{
    if (movementv(DOWN)) return R_CATACOMBS6;
    if (movementv3(NORTH,NW,WEST)) return R_CATACOMBS4;
    if (movementv3(SW,SE,SOUTH)) return R_CATACOMBS4;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS4;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs6(void)
{
    if (movementv(WEST)) return R_CATACOMBS7;
    if (movementv3(NORTH,NW,DOWN)) return R_CATACOMBS5;
    if (movementv3(SW,SE,SOUTH)) return R_CATACOMBS5;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS5;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs7(void)
{
    if (movementv(NW)) return R_CATACOMBS8;
    if (movementv3(NORTH,WEST,DOWN)) return R_CATACOMBS6;
    if (movementv3(SW,SE,SOUTH)) return R_CATACOMBS6;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS6;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs8(void)
{
    if (movementv(NORTH)) return R_CATACOMBS9;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS7;
    if (movementv3(SW,SE,SOUTH)) return R_CATACOMBS7;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS7;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs9(void)
{
    if (movementv(SOUTH)) return R_CATACOMBS10;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS8;
    if (movementv3(SW,SE,NORTH)) return R_CATACOMBS8;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS8;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs10(void)
{
    if (movementv(NORTH)) return R_CATACOMBS11;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS9;
    if (movementv3(SW,SE,SOUTH)) return R_CATACOMBS9;
    if (movementv3(EAST,NE,UP)) return R_CATACOMBS9;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs11(void)
{
    if (movementv(SW)) return R_CATACOMBS12;
    if (movementv(EAST)) return R_AUDIENCE;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS10;
    if (movementv3(NORTH,SE,SOUTH)) return R_CATACOMBS10;
    if (movementv2(NE,UP)) return R_CATACOMBS10;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs12(void)
{
    if (movementv(EAST)) return R_CATACOMBS13;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS11;
    if (movementv3(NORTH,SE,SOUTH)) return R_CATACOMBS11;
    if (movementv3(NE,UP,SW)) return R_CATACOMBS11;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs13(void)
{
    if (movementv(SE)) return R_CATACOMBS14;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS12;
    if (movementv3(NORTH,EAST,SOUTH)) return R_CATACOMBS12;
    if (movementv3(NE,UP,SW)) return R_CATACOMBS12;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs14(void)
{
    if (movementv(NE)) return R_CATACOMBS15;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS13;
    if (movementv3(NORTH,EAST,SOUTH)) return R_CATACOMBS13;
    if (movementv3(SE,UP,SW)) return R_CATACOMBS13;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs15(void)
{
    if (movementv(EAST)) return R_CATACOMBS16;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS14;
    if (movementv3(NORTH,NE,SOUTH)) return R_CATACOMBS14;
    if (movementv3(SE,UP,SW)) return R_CATACOMBS14;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs16(void)
{
    if (movementv(SE)) return R_CATACOMBS17;
    if (movementv3(NW,WEST,DOWN)) return R_CATACOMBS15;
    if (movementv3(NORTH,NE,SOUTH)) return R_CATACOMBS15;
    if (movementv3(EAST,UP,SW)) return R_CATACOMBS15;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs17(void)
{
    if (movementv(DOWN)) return R_CATACOMBS18;
    if (movementv3(NW,WEST,SE)) return R_CATACOMBS16;
    if (movementv3(NORTH,NE,SOUTH)) return R_CATACOMBS16;
    if (movementv3(EAST,UP,SW)) return R_CATACOMBS16;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs18(void)
{
    if (movementv(SOUTH)) return R_CATACOMBS19;
    if (movementv3(NW,WEST,SE)) return R_CATACOMBS17;
    if (movementv3(NORTH,NE,DOWN)) return R_CATACOMBS17;
    if (movementv3(EAST,UP,SW)) return R_CATACOMBS17;
    return 0;  /* command hasn't been processed yet */
}

int at_catacombs19(void)
{
    if (movementv(NORTH)) return R_FAKE_Y2;
    if (movementv3(NW,WEST,SE)) return R_CATACOMBS18;
    if (movementv3(SOUTH,NE,DOWN)) return R_CATACOMBS18;
    if (movementv3(EAST,UP,SW)) return R_CATACOMBS18;
    return 0;  /* command hasn't been processed yet */
}

int at_banshee(void)
{
    if (movementv(NW)) return R_BY_FIGURE;
    if (movementv(NORTH)) return R_GOLDEN;
    if (movementp(R_GOLDEN)) return R_GOLDEN;
    return 0;  /* command hasn't been processed yet */
}

int at_golden(void)
{
    if (movementv(NE)) return R_ARABESQUE;
    if (movementv(NW)) return R_TRANSLUCENT;
    if (movementp(R_ARABESQUE)) return R_ARABESQUE;
    if (movementp(R_TRANSLUCENT)) return R_TRANSLUCENT;
    if (movementv(SOUTH)) return R_BANSHEE;
    return 0;  /* command hasn't been processed yet */
}

int at_arabesque(void)
{
    if (movementv2(SOUTH, OUT)) return R_GOLDEN;
    if (movementp(R_GOLDEN)) return R_GOLDEN;
    return 0;  /* command hasn't been processed yet */
}

int at_translucent(void)
{
    if (movementv2(EAST, OUT) || keywordp(R_GOLDEN)) {
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
    if (correctly_used_magic_word(PLUGH) || correctly_used_magic_word_PLOVER()) {
        say_foof();
        return R_FAKE_Y2;
    }
    if (movementv3(DOWN,CLIMB,NORTH) ||
        movementv3(SOUTH,EAST,WEST) ||
        movementv3(NE,NW,SE) || movementv(NW) ||
        keywordv(JUMP)) {
        puts("EEEEEEEEEAAAAAAAAAAAaaaaaahhhhhhhhhhhhh..........\n\n"
        "                                                      >sizzle<");
        return you_are_dead_at(R_YLEM);
    }
    return 0;  /* command hasn't been processed yet */
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
        "You are standing at the end of a road before a small brick building." SOFT_NL
        "Around you is a forest.  A small stream flows out of the building and" SOFT_NL
        "down a gully.",
        F_LIGHTED|F_WATER|F_NOTINCAVE|F_NODWARF, &at_road
    },
    /* R_HILL */ {
        "You're at hill in road.",
        "You have walked up a hill, still in the forest. The road slopes back" SOFT_NL
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
        "You are in a valley in the forest beside a stream tumbling along a" SOFT_NL
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
        "At your feet all the water of the stream splashes into a 2-inch slit" SOFT_NL
        "in the rock.  Downstream the streambed is bare rock.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF|F_WATER, &at_slit
    },
    /* R_DEPRESSION */ {
        "You're outside grate.",
        "You are in a 20-foot depression floored with bare dirt. Set into the" SOFT_NL
        "dirt is a strong steel grate mounted in concrete.  A dry streambed" SOFT_NL
        "leads into the depression.",
        F_LIGHTED|F_HINTABLE|F_NOTINCAVE|F_NODWARF, &at_depression
    },
    /* R_INSIDE */ {
        "You're below the grate.",
        "You are in a small chamber beneath a 3x3 steel grate to the surface." SOFT_NL
        "A low crawl over cobbles leads inward to the west.",
        F_LIGHTED|F_NODWARF, &at_inside
    },
    /* R_COBBLES */ {
        "You're in cobble crawl.",
        "You are crawling over cobbles in a low passage. There is a dim light" SOFT_NL
        "at the east end of the passage.",
        F_LIGHTED|F_NODWARF, &at_cobbles
    },
    /* R_DEBRIS */ {
        "You're in debris room.",
        "You are in a debris room filled with stuff washed in from the surface." SOFT_NL
        "A low wide passage with cobbles becomes plugged with mud and debris" SOFT_NL
        "here, but an awkward canyon leads upward and west.  A note on the wall" SOFT_NL
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
        "You are in a splendid chamber thirty feet high. The walls are frozen" SOFT_NL
        "rivers of orange stone.  An awkward canyon and a good passage exit" SOFT_NL
        "from east and west sides of the chamber.",
        F_HINTABLE, &at_bird
    },
    /* R_SPIT */ {
        "You're at top of small pit.",
        "At your feet is a small pit breathing traces of white mist. An east" SOFT_NL
        "passage ends here except for a small crack leading on.  Rough stone" SOFT_NL
        "steps lead down the pit.",
        0, &at_spit
    },
    /* R_EMIST */ {
        "You're in Hall of Mists.",
        "You are at one end of a vast hall stretching forward out of sight to" SOFT_NL
        "the west, filled with wisps of white mist that sway to and fro" SOFT_NL
        "almost as if alive.  Rough stone steps lead up to a passage at the" SOFT_NL
        "top of a dome above you.  A wide staircase runs downward into" SOFT_NL
        "the darkness; a chill wind blows up from below.  There are small" SOFT_NL
        "passages to the north and south, and a small crack leads east.",
        0, &at_emist
    },
    /* R_EFISS */ {
        "You're on east bank of fissure.",
        "You are on the east bank of a fissure slicing clear across the hall." SOFT_NL
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
        "This is a low room with a crude note on the wall. The note says," SOFT_NL
        "\"You won't get it up the steps\".",
        F_ONE_EXIT, &at_nugget
    },
    /* R_HMK */ {
        "You're in Hall of Mt King.",
        "You are in the Hall of the Mountain King, with passages off in all" SOFT_NL
        "directions.",
        F_HINTABLE, &at_hmk
    },
    /* R_W2PIT */ {
        "You're at west end of Twopit room.",
        "You are at the west end of the Twopit room. There is a large hole in" SOFT_NL
        "the wall above the pit at this end of the room.",
        0, &at_w2pit
    },
    /* R_E2PIT */ {
        "You're at east end of Twopit room.",
        "You are at the east end of the Twopit room. The floor here is" SOFT_NL
        "littered with thin rock slabs, which make it easy to descend the pits." SOFT_NL
        "There is a path here bypassing the pits to connect passages from east" SOFT_NL
        "and west.  There are holes all over, but the only big one is on the" SOFT_NL
        "wall directly over the west pit where you can't get to it.",
        0, &at_e2pit
    },
    /* R_EPIT */ {
        "You're in east pit.\nThere is a small pool of oil here.",
        "You are at the bottom of the eastern pit in the Twopit room. There is" SOFT_NL
        "a small pool of oil in one corner of the pit.",
        F_NODWARF, &at_epit
    },
    /* R_WPIT */ {
        "You're in west pit.",
        "You are at the bottom of the western pit in the Twopit room. There is" SOFT_NL
        "a large hole in the wall about 25 feet above you.",
        F_NODWARF, &at_wpit
    },
    /* R_NS */ {
        "You're in a low N/S passage.",
        "You are in a low N/S passage at a hole in the floor. The hole goes" SOFT_NL
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
        "You are in the west side chamber of the Hall of the Mountain King." SOFT_NL
        "A passage continues west and up here.",
        0, &at_west
    },
    /* R_Y2 */ {
        "You're at \"Y2\".",
        "You are in a large room, with a passage to the south, a passage to the" SOFT_NL
        "west, and a wall of broken rock to the east.  There is a large \"Y2\" on" SOFT_NL
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
        "You're at a low window overlooking a huge pit, which extends up out of" SOFT_NL
        "sight.  A floor is indistinctly visible over 50 feet below.  Traces of" SOFT_NL
        "white mist cover the floor of the pit, becoming thicker to the right." SOFT_NL
        "Marks in the dust around the window would seem to indicate that" SOFT_NL
        "someone has been here recently.  Directly across the pit from you and" SOFT_NL
        "25 feet away there is a similar window looking into a lighted room.  A" SOFT_NL
        "shadowy figure can be seen there peering back at you.",
        F_ONE_EXIT, &at_windoe
    },
    /* R_WINDOW */ {
        "You're at window on pit.",
        "You're at a low window overlooking a huge pit, which extends up out of" SOFT_NL
        "sight.  A floor is indistinctly visible over 50 feet below.  Traces of" SOFT_NL
        "white mist cover the floor of the pit, becoming thicker to the left." SOFT_NL
        "Marks in the dust around the window would seem to indicate that" SOFT_NL
        "someone has been here recently.  Directly across the pit from you and" SOFT_NL
        "25 feet away there is a similar window looking into a lighted room.  A" SOFT_NL
        "shadowy figure can be seen there peering back at you.",
        F_ONE_EXIT, &at_window
    },
    /* R_DIRTY */ {
        "You're in dirty passage.",
        "You are in a dirty broken passage. To the east is a crawl. To the" SOFT_NL
        "west is a large passage.  Above you is a hole to another passage.",
        0, &at_dirty
    },
    /* R_CLEAN */ {
        "You are on the brink of a small clean climbable pit.",
        "You are on the brink of a small clean climbable pit. A crawl leads" SOFT_NL
        "west.",
        0, &at_clean
    },
    /* R_WET */ {
        "You are at the pit's bottom with a stream nearby.",
        "You are in the bottom of a small pit with a little stream, which" SOFT_NL
        "enters and exits through tiny slits.",
        F_WATER|F_NODWARF, &at_wet
    },
    /* R_DUSTY */ {
        "You're in dusty rock room.",
        "You are in a large room full of dusty rocks. There is a big hole in" SOFT_NL
        "the floor.  There are cracks everywhere, and a passage leading east.",
        0, &at_dusty
    },
    /* R_WMIST */ {
        "You're at west end of Hall of Mists.",
        "You are at the west end of Hall of Mists. A low wide crawl continues" SOFT_NL
        "west and another goes north.  To the south is a little passage 6 feet" SOFT_NL
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
        "You are on the brink of a thirty foot pit with a massive orange column" SOFT_NL
        "down one wall.  You could climb down here but you could not get back" SOFT_NL
        "up.  The maze continues at this level.",
        F_HINTABLE|F_INMAZE|F_NOBACK, &at_mazea57_pit
    },
    /* R_MAZEA58 */ { NULL, dead_end, F_HINTABLE|F_INMAZE|F_NOBACK|F_ONE_EXIT, &at_mazea58 },
    /* R_ELONG */ {
        "You're at east end of long hall.",
        "You are at the east end of a very long hall apparently without side" SOFT_NL
        "chambers.  To the east a low wide crawl slants up.  To the north a" SOFT_NL
        "round two foot hole slants down.",
        0, &at_elong
    },
    /* R_WLONG */ {
        "You're at west end of long hall.",
        "You are at the west end of a very long featureless hall. The hall" SOFT_NL
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
        "Dead end passage. Scratched on a rock is the message, \"Stand where" SOFT_NL
        "the statue gazes, and make use of the proper tool.\"",
        F_ONE_EXIT, &at_deadend1
    },
    /* R_COMPLEX */ {
        "You're at complex junction.",
        "You are at a complex junction. A low hands and knees passage from the" SOFT_NL
        "north joins a higher crawl from the east to make a walking passage" SOFT_NL
        "going west.  There is also a large room above.  The air is damp here.",
        0, &at_complex
    },
    /* R_BEDQUILT */ {
        "You're back at Bedquilt.",
        "You are in Bedquilt, a long east/west passage with holes everywhere." SOFT_NL
        "To explore at random select NORTH, SOUTH, UP, or DOWN.",
        0, &at_bedquilt
    },
    /* R_SWISS */ {
        "You're in Swiss cheese room.",
        "You are in a room whose walls resemble Swiss cheese. Obvious passages" SOFT_NL
        "go west, east, NE, and NW.  Part of the room is occupied by a large" SOFT_NL
        "bedrock block.",
        0, &at_swiss
    },
    /* R_SLAB */ {
        "You're in Slab room.",
        "You are in a large low circular chamber whose floor is an immense slab" SOFT_NL
        "fallen from the ceiling (Slab room).  East and west there once were" SOFT_NL
        "large passages, but they are now filled with boulders.  Low small" SOFT_NL
        "passages go north and south, and the south one quickly bends west" SOFT_NL
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
        "You are in a secret canyon at a junction of three canyons, bearing" SOFT_NL
        "north, south, and SE.  The north one is as tall as the other two" SOFT_NL
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
        "You are in a secret canyon which here runs E/W. It crosses over a" SOFT_NL
        "very tight canyon 15 feet below.  If you go down you may not be able" SOFT_NL
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
        "You are in a tall E/W canyon. A low tight crawl goes 3 feet north and" SOFT_NL
        "seems to open up.",
        0, &at_tallewcnyn
    },
    /* R_DEADEND3 */ {
        "The canyon runs into a mass of boulders -- dead end.",
        "The canyon runs into a mass of boulders -- dead end.  Scratched on" SOFT_NL
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
        "You are in a long, narrow corridor stretching out of sight to the" SOFT_NL
        "west.  At the eastern end is a hole through which you can see a" SOFT_NL
        "profusion of leaves.",
        0, &at_narrow
    },
    /* R_INCLINE */ {
        "You're at steep incline above large room.",
        "You are at the top of a steep incline above a large room. You could" SOFT_NL
        "climb down here, but you would not be able to climb up.  There is a" SOFT_NL
        "passage leading back to the north.",
        F_NOBACK, &at_incline
    },
    /* R_GIANT */ {
        "You're in Giant room.",
        "You are in the Giant room. The ceiling here is too high up for your" SOFT_NL
        "lamp to show it.  Cavernous passages lead east, north, and south.  On" SOFT_NL
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
        "You are in a magnificent cavern with a rushing stream, which cascades" SOFT_NL
        "over a sparkling waterfall into a roaring whirlpool which disappears" SOFT_NL
        "through a hole in the floor.  Passages exit to the south and west.",
        F_WATER, &at_falls
    },
    /* R_SOFT */ {
        "You're in soft room.",
        "You are in the soft room. The walls are covered with heavy curtains," SOFT_NL
        "the floor with a thick pile carpet.  Moss covers the ceiling.",
        F_ONE_EXIT, &at_soft
    },
    /* R_ORIENTAL */ {
        "You're in Oriental room.",
        "This is the Oriental room. Ancient oriental cave drawings cover the" SOFT_NL
        "walls.  A gently sloping passage leads upward to the north, another" SOFT_NL
        "passage leads SE, and a hands and knees crawl leads west.",
        0, &at_oriental
    },
    /* R_MISTY */ {
        "You're in misty cavern.",
        "You are following a wide path around the outer edge of a large cavern." SOFT_NL
        "Far below, through a heavy white mist, strange splashing noises can be" SOFT_NL
        "heard.  The mist rises up through a fissure in the ceiling.  The path" SOFT_NL
        "exits to the south and west.",
        0, &at_misty
    },
    /* R_ALCOVE */ {
        "You're in alcove.",
        "You are in an alcove. A small NW path seems to widen after a short" SOFT_NL
        "distance.  An extremely tight tunnel leads east.  It looks like a very" SOFT_NL
        "tight squeeze.  An eerie light can be seen at the other end.",
        F_NOBACK|F_NODWARF|F_HINTABLE, &at_alcove
    },
    /* R_PLOVER */ {
        "You're in Plover room.",
        "You're in a small chamber lit by an eerie green light. An extremely" SOFT_NL
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
        "You are in an arched hall. A coral passage continues up and east." SOFT_NL
        "The air smells of sea water.",
        0, &at_arched
    },
    /* R_SHELL */ {
        "You're in Shell room.",
        "You're in a large room carved out of sedimentary rock. The floor and" SOFT_NL
        /* Platt has "imbedded". */
        "walls are littered with bits of shells embedded in the stone.  A" SOFT_NL
        "shallow passage proceeds downward, and a somewhat steeper one leads" SOFT_NL
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
        "You are in an anteroom leading to a large passage to the east. Small" SOFT_NL
        "passages go west and up.  The remnants of recent digging are evident." SOFT_NL
        "A sign in midair here says \"Cave under construction beyond this point." SOFT_NL
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
        "You are in a north/south canyon about 25 feet across. The floor is" SOFT_NL
        "covered by white mist seeping in from the north.  The walls extend" SOFT_NL
        "upward for well over 100 feet.  Suspended from some unseen point far" SOFT_NL
        "above you, an enormous two-sided mirror is hanging parallel to and" SOFT_NL
        "midway between the canyon walls.  (The mirror is obviously provided" SOFT_NL
        "for the use of the dwarves, who as you know, are extremely vain.)  A" SOFT_NL
        "small window can be seen in either wall, some fifty feet up.",
        0, &at_mirror
    },
    /* R_STALACT */ {
        "You're at top of stalactite.",
        "A large stalactite extends from the roof and almost reaches the floor" SOFT_NL
        "below.  You could climb down it, and jump from it to the floor, but" SOFT_NL
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
        "You are on the southern edge of a large underground reservoir.  A" SOFT_NL
        "thick cloud of white mist fills the room, rising from the surface" SOFT_NL
        "of the water and drifting rapidly upwards.  The lake is fed by a" SOFT_NL
        "stream, which tumbles out of a hole in the wall about 10 feet overhead" SOFT_NL
        "and splashes noisily into the water near the reservoir's northern wall." SOFT_NL
        "A dimly-seen passage exits through the northern wall, but you can't get" SOFT_NL
        "across the water to get to it.  Another passage leads south from here.",
        F_WATER|F_ONE_EXIT, &at_res
    },
    /* R_RESERVOIR_N */ {
        "You are at the northern end of the reservoir.",
        "You are at the northern end of a large underground reservoir.  Across" SOFT_NL
        "the water to the south, a dark passage is visible.  Another passage" SOFT_NL
        "leads north from here.  Large, clawed tracks are visible in the damp" SOFT_NL
        "ground, leading from the passage into the water.",
        F_WATER|F_NODWARF|F_NOBACK, &at_reservoir_n
    },
    /* R_WARM */ {
        "You're in small, warm chamber.",
        "You are in a small chamber with warm walls.  Mist drifts into the" SOFT_NL
        "chamber from a passage entering from the south and evaporates in" SOFT_NL
        "the heat.  Another passage leads out to the northeast.",
        F_NODWARF, &at_warm
    },
    /* R_BALCONY */ {
        "You're on balcony above treasure chamber.",
        "You are in a high balcony carved out of solid rock overlooking a large," SOFT_NL
        "bare chamber lit by dozens of flickering torches.  A rushing stream" SOFT_NL
        "pours into the chamber through a two-foot slit in the east wall and" SOFT_NL
        "drains into a large pool along the north side of the chamber.  A small" SOFT_NL
        "plaque riveted to the edge of the balcony reads, \"You are looking at" SOFT_NL
        "the Witt Company's main treasure room, constructed by the famous" SOFT_NL
        "architect Ralph Witt in 4004 B.C., and dedicated to the proposition" SOFT_NL
        "that all adventurers are created equal (although some are more equal" SOFT_NL
        "than others).  NO ADMITTANCE VIA THIS ENTRANCE!\"  A small, dark" SOFT_NL
        "tunnel leads out to the west.",
        F_LIGHTED|F_NODWARF|F_ONE_EXIT, &at_balcony
    },
    /* R_FAKE_SLIT */ {
        "You're at slit in streambed.",
        "At your feet all the water of the stream splashes into a 2-foot slit" SOFT_NL
        "in the rock.  Downstream the streambed is bare rock.",
        F_LIGHTED|F_NOTINCAVE|F_NODWARF, &at_fake_slit
    },
    /* R_CYLINDRICAL */ {
        "You're in cylindrical chamber.",
        "You are in a small cylindrical room with very smooth walls and a flat" SOFT_NL
        "floor and ceiling.  There are no exits visible anywhere.",
        F_LIGHTED|F_NOBACK, &at_cylindrical
    },
    /* R_PIRATES_NEST */ { NULL, dead_end, F_INMAZE|F_NODWARF, &at_pirates_nest },
    /* R_SWOFCHASM */ {
        "You're on SW side of chasm.",
        "You are on one side of a large, deep chasm. A heavy white mist rising" SOFT_NL
        "up from below obscures all view of the far side.  A SW path leads away" SOFT_NL
        "from the chasm into a winding corridor.",
        F_NOBACK|F_NODWARF, &at_swofchasm
    },
    /* R_NEOFCHASM */ {
        "You're on NE side of chasm.",
        "You are on the far side of the chasm. A NE path leads away from the" SOFT_NL
        "chasm on this side.",
        F_NOBACK|F_NODWARF, &at_neofchasm
    },
    /* R_SLOPING */ {
        "You're in sloping corridor.",
        "You are in a long winding corridor sloping out of sight in both" SOFT_NL
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
        "You're in a long east/west corridor. A faint rumbling noise can be" SOFT_NL
        "heard in the distance.",
        F_NODWARF, &at_corridor
    },
    /* R_FORK */ {
        "You're at fork in path.",
        "The path forks here. The left fork leads northeast. A dull rumbling" SOFT_NL
        "seems to get louder in that direction.  The right fork leads southeast" SOFT_NL
        "down a gentle slope.  The main corridor enters from the west.",
        F_NODWARF, &at_fork
    },
    /* R_WARMJUNCTN */ {
        "You're at junction with warm walls.",
        "The walls are quite warm here. From the north can be heard a steady" SOFT_NL
        "roar, so loud that the entire cave seems to be trembling.  Another" SOFT_NL
        "passage leads south, and a low crawl goes east.",
        F_NODWARF, &at_warmjunctn
    },
    /* R_VIEW */ {
        "You're at breath-taking view.",
        "You are on the edge of a breath-taking view. Far below you is an" SOFT_NL
        "active volcano, from which great gouts of molten lava come surging" SOFT_NL
        "out, cascading back down into the depths.  The glowing rock fills the" SOFT_NL
        "farthest reaches of the cavern with a blood-red glare, giving every" SOFT_HYPHEN
        "thing an eerie, macabre appearance.  The air is filled with flickering" SOFT_NL
        "sparks of ash and a heavy smell of brimstone.  The walls are hot to" SOFT_NL
        "the touch, and the thundering of the volcano drowns out all other" SOFT_NL
        "sounds.  Embedded in the jagged roof far overhead are myriad twisted" SOFT_NL
        "formations composed of pure white alabaster, which scatter the murky" SOFT_NL
        "light into sinister apparitions upon the walls.  To one side is a deep" SOFT_NL
        "gorge, filled with a bizarre chaos of tortured rock which seems to" SOFT_NL
        "have been crafted by the devil himself.  An immense river of fire" SOFT_NL
        "crashes out from the depths of the volcano, burns its way through the" SOFT_NL
        "gorge, and plummets into a bottomless pit far off to your left." SOFT_NL
        "Across the gorge, the entrance to a valley is dimly visible.  To" SOFT_NL
        "the right, an immense geyser of blistering steam erupts continuously" SOFT_NL
        "from a barren island in the center of a sulfurous lake, which bubbles" SOFT_NL
        "ominously.  The far right wall is aflame with an incandescence of its" SOFT_NL
        "own, which lends an additional infernal splendor to the already" SOFT_NL
        "hellish scene.  A dark, foreboding passage exits to the south.",
            F_LIGHTED|F_NODWARF, &at_view
    },
    /* R_FACES */ {
        "You're at the south end of the Valley of the Stone Faces.",
        "You are standing at the southern end of a long valley illuminated by" SOFT_NL
        "flickering red light from the volcanic gorge behind you.  Carved" SOFT_NL
        "into the walls of the valley is an incredible series of stone faces." SOFT_NL
        "Some of them look down into the valley with expressions of" SOFT_NL
        "benevolence that would credit a saint; others glare with a malice" SOFT_NL
        "that makes the heart grow faint.  All of them are imbued with a" SOFT_NL
        "fantastic seeming of life by the shifting and flickering light of" SOFT_NL
        "the volcano.  The entire far end of the valley is taken up by an" SOFT_NL
        "immense carving of a seated figure; its exact form cannot be seen" SOFT_NL
        "from here due to the uncertainty of the light.",
        F_LIGHTED|F_NOBACK|F_NODWARF, &at_faces
    },
    /* R_BY_FIGURE */ {
        "You're at north end of the Valley of the Stone Faces.",
        "You are standing at the north end of the Valley of the Stone Faces." SOFT_NL
        "Above you, an incredible bas-relief statue of an immense minotaur" SOFT_NL
        "has been carved out of the rock.  At least sixty feet high, it sits" SOFT_NL
        "gazing down at you with a faint but definite expression of amusement." SOFT_NL
        "Between its feet and the floor is a rock wall about ten feet high" SOFT_NL
        "which extends across the entire north end of the valley.",
        F_LIGHTED|F_NODWARF, &at_by_figure
    },
    /* R_PLAIN_1 */ {
        "You're at south end of fog-filled room.",
        "You are standing at the southern end of what appears to be a" SOFT_NL
        "large room filled with multicolored fog.  The sides and far end" SOFT_NL
        "of the room cannot be seen due to the thickness of the fog " EMDASH("-") " it's" SOFT_NL
        "a real pea-souper (even to the color in places!).  A passage leads" SOFT_NL
        "back to the south; a dull rumbling sound issues from the passage.",
        F_NOBACK|F_NODWARF, &at_plain_1
    },
    /* R_PLAIN_2 */ {
        NULL, NULL, F_LIGHTED|F_NOBACK|F_NODWARF|F_HINTABLE, &at_plain_2
    },
    /* R_PLAIN_3 */ {
        "You're in foggy room by cairn of rocks.",
        "You are standing in a fog-filled room next to a tall cairn of glowing" SOFT_NL
        "rocks.  An opening in the cairn leads down to a dark passage.",
        F_LIGHTED|F_NODWARF, &at_plain_3
    },
    /* R_NONDESCRIPT */ {
        "You're in nondescript chamber.",
        "You're in a small, nondescript chamber.  A dark passage leads up" SOFT_NL
        "and to the south, and a wide but low crawl leads north.",
        F_NODWARF, &at_nondescript
    },
    /* R_PENTAGRAM */ {
        "You're in room with pentagram.",
        "You're in a small room with a very smooth rock floor, onto which" SOFT_NL
        "has been marked a pentagram.  A low crawl leads out to the west, and" SOFT_NL
        "a crack in the rock leads north.",
        F_NODWARF, &at_pentagram
    },
    /* R_CHIMNEY */ {
        "You're at the end of the crack, at the bottom of the chimney.",
        "The crack in the rock ends here, but a narrow chimney leads up.  You" SOFT_NL
        "should be able to climb it.",
        F_NODWARF, &at_chimney
    },
    /* R_TUBE */ {
        "You're in lava tube at top of chimney.",
        "You're at the top of a narrow chimney in the rock.  A cylindrical" SOFT_NL
        "tube composed of hardened lava leads south.",
        F_NODWARF, &at_tube
    },
    /* R_TUBE_SLIDE */ {
        "You're at steep slide in lava tube.",
        "The lava tube continues down and to the south, but it becomes very" SOFT_NL
        "steep here " EMDASH("-") " if you go down it you probably won't be able to get" SOFT_NL
        "back up.",
        F_NODWARF, &at_tube_slide
    },
    /* R_BASQUE_1 */ {
        "You're in rough and narrow passage.",
        "You are in a narrow and rough passage running north and south." SOFT_NL
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
        "The passage here enters from the south and divides, with a wide" SOFT_NL
        "tunnel exiting to the north and a set of steps leading downward.",
            F_NODWARF, &at_basque_fork
    },
    /* R_ON_STEPS */ {
        "You're on the steps.",
        "You are on a long, spiral set of steps leading downwards into the" SOFT_NL
        "earth.",
            F_NODWARF, &at_on_steps
    },
    /* R_STEPS_EXIT */ {
        "You're at exit on steps.",
        "A small tunnel exits from the steps and leads north.  The steps" SOFT_NL
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
        "You are in a large room, with a passage to the south, a passage to the" SOFT_NL
        "west, and a wall of broken rock to the east.  There is a large \"Y2\" on" SOFT_NL
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
        "You are standing at the west end of the royal Audience Hall." SOFT_NL
        "The walls here are composed of the finest marble, and the floor is" SOFT_NL
        "built of slabs of rare onyx and bloodstone.  The ceiling is high and" SOFT_NL
        "vaulted, and is supported by pillars of rare Egyptian red granite;" SOFT_NL
        "it gives off a nacreous glow that fills the entire chamber with" SOFT_NL
        "a light like moon-light shining off of polished silver.",
            F_LIGHTED|F_NODWARF, &at_audience
    },
    /* R_AUDIENCE_E */ {
        "You're at east end of Audience Hall.",
        "You are at the eastern end of the Audience Hall.  There is a large" SOFT_NL
        "dais rising out of the floor here; resting upon the dais is a strange" HARD_HYPHEN
        "looking throne made out of interlocking bars and rods of metal.",
            F_LIGHTED|F_NODWARF, &at_audience_e
    },
    /* R_BANSHEE */ {
        "You're in winding passage.",
        "You are in a winding passage which enters from the northwest," SOFT_NL
        "loops around several times, and exits to the north.",
            F_NODWARF, &at_banshee
    },
    /* R_GOLDEN */ {
        "You're in golden chamber.",
        "You are in a chamber with golden walls and a high ceiling.  Passages" SOFT_NL
        "lead south, northeast, and northwest.",
            F_NODWARF, &at_golden
    },
    /* R_ARABESQUE */ {
        "You're in Arabesque room.",
        "You are in a small room whose walls are covered with an elaborate" SOFT_NL
        "pattern of arabesque figures and designs.",
            F_NODWARF, &at_arabesque
    },
    /* R_TRANSLUCENT */ {
        "You're in room with translucent walls.",
        "You are in a large room whose walls are composed of some translucent" SOFT_NL
        "whitish mineral.  The room is illuminated by a flickering reddish" SOFT_NL
        "glow shining through the southern wall.  A passage leads east.",
            F_LIGHTED|F_NODWARF, &at_translucent
    },
    /* R_CHAMBER */ {
        "You're in chamber of boulders.",
        "You are in a small chamber filled with large boulders. The walls are" SOFT_NL
        "very warm, causing the air in the room to be almost stifling from the" SOFT_NL
        "heat.  The only exit is a crawl heading west, through which is coming" SOFT_NL
        "a low rumbling.",
            F_NODWARF, &at_chamber
    },
    /* R_LIME */ {
        "You're in limestone passage.",
        "You are walking along a gently sloping north/south passage lined with" SOFT_NL
        "oddly shaped limestone formations.",
            F_NODWARF, &at_lime
    },
    /* R_FBARR */ {
        "You are at entrance of the barren room.",
        "You are standing at the entrance to a large, barren room. A sign" SOFT_NL
        "posted above the entrance reads:  \"Caution!  Bear in room!\"",
            F_NODWARF, &at_fbarr
    },
    /* R_BARR */ {
        "You are in the barren room.",
        "You are inside a barren room. The center of the room is completely" SOFT_NL
        "empty except for some dust.  Marks in the dust lead away toward the" SOFT_NL
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
        "You are in a small chamber to the east of the Hall of Mists.  The" SOFT_NL
        "walls are composed of rough red sandstone.  There is a large, cubical" SOFT_NL
        "chunk of rock in the center of the room.",
        F_ONE_EXIT, &at_sandstone
    },
    /* R_MORION */ {
        "You're in the Morion room.",
        "You are in a small room.  The walls are composed of a dark, almost" SOFT_NL
        "black form of smoky quartz; they glisten like teeth in the lamp-light." SOFT_NL
        "The only exit is the passage to the south through which you entered.",
        0, &at_morion
    },
    /* R_VAULT */ {
        "You're in room with vaulted ceiling.",
        "You are in a room with a high, vaulted ceiling.  A tunnel leads" SOFT_NL
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
        "You are standing at a bend in a wide corridor which runs to the east" SOFT_NL
        "and west.  To the east, the corridor turns left and continues north;" SOFT_NL
        "to the west, it turns left and continues south.",
        0, &at_corrid_2
    },
    /* R_TOOL */ {
        "You're in Tool room.",
        "You are in a small, low-ceilinged room with the words \"Witt Company" SOFT_NL
        "Tool Room " EMDASH("-") " Melenkurion division\" carved into one of the walls.  A" SOFT_NL
        "wide corridor runs south from here.",
        F_ONE_EXIT, &at_tool
    },
    /* R_CORRID_3 */ {
        "You're at division in passage.",
        "You are at a division in a narrow passage.  Two spurs run east and" SOFT_NL
        "north; the main passage exits to the south.",
        0, &at_corrid_3
    },
    /* R_CUBICLE */ {
        "You're in dank cubicle.",
        "You are in a small, dank cubicle of rock.  A small passage leads back" SOFT_NL
        "out to the south; there is no other obvious exit.",
        F_ONE_EXIT, &at_cubicle
    },
    /* R_SPHERICAL */ {
        "You're in spherical room.",
        "You're in a large, completely spherical room with polished walls.  A" SOFT_NL
        "narrow passage leads out to the north.",
        F_ONE_EXIT, &at_spherical
    },
    /* R_TUNNEL_1 */ {
        "You're in low tunnel with irregular ceiling.",
        "You are in a low tunnel with an irregular ceiling.  To the north, the" SOFT_NL
        "tunnel is partially blocked by a recent cave-in, but you can probably" SOFT_NL
        "get past the blockage without too much trouble.",
        0, &at_tunnel_1
    },
    /* R_GLASSY */ {
        "You're in large room with glassy walls.",
        "You're standing in a very large room (which however is smaller than the" SOFT_NL
        "Giant room) which has smooth, glassy-looking walls.  A passage enters" SOFT_NL
        "from the south and exits to the north.",
        0, &at_glassy
    },
    /* R_LAIR */ {
        "You're in the Sorcerer's Lair.",
        "This is the Sorcerer's Lair.  The walls are covered with exotic runes" SOFT_NL
        "written in strange, indecipherable scripts; the only readable phrase" SOFT_NL
        "reads \"noside samoht\".  Strange shadows flit about on the walls, but" SOFT_NL
        "there is nothing visible to cast them.  Iridescent blue light drips" SOFT_NL
        "from a stalactite far above, falls towards the floor, and evaporates" SOFT_NL
        "before touching the ground.  A deep, resonant chanting sound vibrates" SOFT_NL
        "from deep in the ground beneath your feet, and a whispering sound" SOFT_NL
        "composed of the echoes of long-forgotten spells and cantrips seeps" SOFT_NL
        "from the walls and fills the air.  Passages exit to the east and west.",
        F_LIGHTED, &at_lair
    },
    /* R_BRINK_1 */ {
        "You're at brink of bottomless pit.",
        "You are standing on the brink of what appears to be a bottomless pit" SOFT_NL
        "plunging down into the bowels of the earth.  Ledges run around the" SOFT_NL
        "pit to the east and west, and a passage leads back to the north.",
        0, &at_brink_1
    },
    /* R_BRINK_2 */ {
        "You're on southern edge of bottomless pit.",
        "You are standing at the south end of a ledge running around the west" SOFT_NL
        "side of a bottomless pit.  The ledge once continued around to the east" SOFT_NL
        "side of the pit, but was apparently obliterated by a rock-slide" SOFT_NL
        "years ago.  A cold wind blows out of a tunnel leading to the southeast.",
        0, &at_brink_2
    },
    /* R_ICE */ {
        "You're in Ice room.",
        "You are in the Ice room.  The walls and ceiling here are composed of" SOFT_NL
        "clear blue glacial ice; the floor is fortunately made of rock and" SOFT_NL
        "is easy to walk upon.  There is a passage leading to the northwest," SOFT_NL
        "and a slide of polished ice leading downwards to the east " EMDASH("-") " if you" SOFT_NL
        "were to slide down it you probably couldn't get back up.",
        F_NOBACK, &at_ice
    },
    /* R_SLIDE */ {
        "You're at bottom of icy slide.",
        "You're at the entrance to an extensive and intricate network of tunnels" SOFT_NL
        "carved out of solid ice.  A slippery slope leads upwards and north, but" SOFT_NL
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
        "You are in a small chamber melted out of the ice.  Glowing letters" SOFT_NL
        "in midair spell out the words \"This way out\".",
        F_INMAZE|F_HINTABLE, &at_icecave30
        },
    /* R_BRINK_3 */ {
        "You're on eastern side of bottomless pit.",
        "You are standing on the eastern side of a bottomless pit.  A narrow" SOFT_NL
        "ledge runs north towards a dimly-visible passage; the ledge once" SOFT_NL
        "continued south of this point but has been shattered by falling rock." SOFT_NL
        "A narrow crack in the rock leads northeast.",
        0, &at_brink_3
    },
    /* R_CRACK_1 */ {
        "You're in narrow, twisting crack.",
        "You are following a narrow crack in the rock which enters from the" SOFT_NL
        "southwest, turns and twists somewhat, and exits to the southeast.",
        0, &at_crack_1
    },
    /* R_CRACK_2 */ {
        "You're at north end of tight passage.",
        "You are standing at the northern end of a rather tight passage.  A" SOFT_NL
        "narrow crack in the rock leads west.",
        0, &at_crack_2
    },
    /* R_CRACK_3 */ {
        "You're at south end of tight passage.",
        "You are at the southern end of a tight passage.  A hands-and-knees" SOFT_NL
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
        "You are in an arched coral passage which enters from the west, splits," SOFT_NL
        "and continues on to the east over a smooth and damp-looking patch of" SOFT_NL
        "sand.  The fork in the passage once led to the south, but it is now" SOFT_NL
        "completely blocked by debris.",
        F_NOBACK, &at_arch_cor_1
    },
    /* R_ARCH_COR_2 */ {
        "You're at bend in arched coral corridor.",
        /* Platt has "corral". */
        "You are at a bend in an arched coral passage; the passage enters" SOFT_NL
        "from the west over a patch of damp sand, turns, and continues north.",
        F_NOBACK, &at_arch_cor_2
    },
    /* R_ARCH_FORK */ {
        /* Platt has "corral". */
        "You're at fork in arched coral passage.",
        "You are at a fork in a high, arched coral passage.  The main portion" SOFT_NL
        "of the passage enters from the south; two smaller passages lead" SOFT_NL
        "east and north.  The smell of salt water is very strong here.",
        0, &at_arch_fork
    },
    /* R_JONAH */ {
        "You're at entrance to the Jonah room.",
        "You are standing at the entrance of the Jonah room, a cavernous" SOFT_NL
        "hall with high ribbed walls.  The hall extends far to the south;" SOFT_NL
        "a coral passage leads west.",
        0, &at_jonah
    },
    /* R_IN_JONAH */ {
        "You're at south end of the Jonah room.",
        "You are at the south end of the Jonah room.  Ahead of you, the way" SOFT_NL
        "is barred by a large set of immense stalactites and stalagmites" SOFT_NL
        "which intermesh like clenched teeth.  Nothing except blackness is" SOFT_NL
        "visible between the stone formations.",
        0, &at_in_jonah
    },
    /* R_FOURIER */ {
        "You're in the Fourier passage.",
        "You are in the Fourier passage.  This is a long and highly convoluted" SOFT_NL
        "passage composed of coral, which twists and turns like the path of" SOFT_NL
        "an earthworm tripping on LSD.  The passage here enters from the" SOFT_NL
        "northwest, convulses, and exits to the southwest (from which" SOFT_NL
        "direction can be felt a cool and salty-smelling breeze).",
        0, &at_fourier
    },
    /* R_SHELF */ {
        "You're on shelf of rock above beach.",
        /* Platt has "sedementary"; I've fixed the spelling. */
        "You are standing on a large shelf of sedimentary rock overlooking" SOFT_NL
        "a lava beach.  The shelf is an extension of an incredible cliff" SOFT_NL
        "which extends north, south, and upwards for as far as the eye can" SOFT_NL
        "see.  Crudely carved steps lead down from the shelf to the beach, and" SOFT_NL
        "a twisting coral passage exits to the west.",
        F_LIGHTED, &at_shelf
    },
    /* R_BEACH */ {
        "You're on beach.",
        "You are standing on a short, barren beach composed of hardened lava." SOFT_NL
        "Rugged and unclimbable volcanic hills block all view to the north" SOFT_NL
        "and south, and a seemingly infinite cliff fills the entire western" SOFT_NL
        "hemisphere.  To the east, a narrow inlet of ocean water laps gently" SOFT_NL
        "upon the beach.  The scene is illuminated by the light of three small" SOFT_NL
        "moons shining through the shimmering glow of an aurora that fills" SOFT_NL
        "the entire sky with golden splendor.  Steps lead up the cliff to a" SOFT_NL
        "shelf of rock.",
        F_LIGHTED|F_ONE_EXIT, &at_beach
    },
    /* R_PLATFORM */ {
        "You're on tiny platform above volcano.",
        "You are precariously perched on a tiny platform suspended in midair." SOFT_NL
        "Two thousand feet below you is the mouth of a very active volcano," SOFT_NL
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

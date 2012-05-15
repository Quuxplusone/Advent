
#include <stdbool.h>
#include <stdio.h>
#include "advdat.h"

bool pct(int percent);
void say_foof(void);
void finis(void);
bool keywordv(VerbWord meaning);
bool keywordo(ObjectWord meaning);
bool keywordp(Location meaning);

extern VerbWord verbosity_mode;
extern int closure;
extern int death_count;
extern bool gave_up;
extern bool panicked;
extern bool nomagic;

static const char cantpasslock[] = "You can't go through a locked steel grate!";
static const char you_didnt_move[] =
    "You have crawled around in some little holes and wound up back in the\n"
    "main passage.";

int splatter(Location newloc)
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
	return R_SLIT;
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
	verbosity_mode = BRIEF;
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
         "of welcome!");
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
	    puts("You can't go through a locked steel grate!");
	    return R_DEPRESSION;
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
	    puts("You can't go through a locked steel grate!");
	    return R_INSIDE;
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
	    puts(cantpasslock);
	    return R_INSIDE;
	}
    }
    if (keywordv(XYZZY)) {
	if (closure >= 2 || nomagic) {
	    puts("Nothing happens.");
	    panicked = true;
	    return R_DEBRIS;
	} else if (used_movement_verb(XYZZY)) {
	    say_foof();
	    return R_HOUSE;
	} else {
	    /* This seems like a bug in Platt's code. */
	    return R_DEBRIS;
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
	    puts(cantpasslock);
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
	    puts(cantpasslock);
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
	    puts(cantpasslock);
	    return R_INSIDE;
	}
    }
    if (used_movement_verb(WEST) || used_movement_verb(CRACK)) {
	puts("The crack is far too small for you to enter.");
	return R_SPIT;
    }
    /* This is clearly a bug in Platt's code. Going DOWN (the most natural
     * verb) will work fine, but going STEPS or PIT will kill you.
     * TODO: perhaps fix this? */
    if (used_movement_verb(DOWN)) return R_EMIST;
    if (used_movement_verb(STEPS) || used_movement_placeword(R_SPIT)) {
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
    if (used_movement_verb(UP) || used_movement_verb(STEPS) ||
	used_movement_placeword(R_SPIT) || used_movement_verb(DOME) ||
	used_movement_verb(PASSAGE) || used_movement_verb(IN)) {
	if (toting(GOLD)) {
	    puts("The dome is unclimbable.");
	    return R_EMIST;
	} else {
	    return R_SPIT;
	}
    }
    return 0;  /* command hasn't been processed yet */
}

int at_efiss(void)
{
    if (used_movement_verb(HALL)) return R_EMIST;
    if (used_movement_verb(EAST)) return R_EMIST;
    if (used_movement_verb3(FORWARD, CROSS, WEST)) {
	if (objs[FISSURE].prop) return R_WFISS;
	puts("There is no way across the fissure.");
	return R_EFISS;
    }
    if (used_movement_verb(JUMP)) {
	if (objs[FISSURE].prop) {
	    puts("I respectfully suggest you go across the bridge instead of jumping.");
	    return R_EFISS;
	}
	/* This connection is original to Platt's version. */
	return splatter(R_FALLS);
    }
    return 0;  /* command hasn't been processed yet */
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
	    return R_HMK;
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
		puts(you_didnt_move);
		return R_HMK;
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
	return R_W2PIT;
    }
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
	    return R_WPIT;
	} else if (objs[PLANT].prop == 2) {
	    puts("You have climbed up the plant and out of the pit.");
	    return R_W2PIT;
	} else {
	    puts("You scurry up the plant and into the hole in the wall.");
	    return R_NARROW;
	}
    }
    if (keywordv(GET) && keywordo(PLANT)) {
	puts("The plant has exceptionally deep roots and cannot be pulled free.");
	return R_WPIT;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_wfiss(void)
{
    if (used_movement_verb(WEST)) return R_WMIST;
    if (used_movement_verb3(FORWARD, CROSS, EAST)) {
	if (objs[FISSURE].prop) return R_EFISS;
	puts("There is no way across the fissure.");
	return R_WFISS;
    }
    if (used_movement_verb(JUMP)) {
	if (objs[FISSURE].prop) {
	    puts("I respectfully suggest you go across the bridge instead of jumping.");
	    return R_WFISS;
	}
	/* This connection is original to Platt's version. */
	return splatter(R_FALLS);
    }
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
    if (used_movement_verb(WEST)) return R_WINDOW;
    if (used_movement_verb(PLUGH) || used_movement_placeword(R_PLOVER)) {
	if (word2.type != WordType_None && !keywordv(SAY))
	    return 0;  /* DROP PLUGH isn't handled here */
	if (closure >= 2 || nomagic) {
	    puts("Nothing happens.");
	    panicked = true;
	    return R_Y2;
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

int at_window(void)
{
    if (used_movement_verb(WEST)) return R_SJUNC;
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
	return R_WET;
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
	    return R_BEDQUILT;
	}
	if (used_movement_verb(DOWN)) return R_ANTE;
	if (used_movement_verb(NORTH)) return pct(75) ? R_LOW : R_SJUNC;
	if (used_movement_verb(UP)) return pct(75) ? R_DUSTY : R_ABOVEP;
	if (used_movement_verb(SOUTH)) return pct(75) ? R_SLAB : R_TALLEWCNYN;
    }
    if (keywordv(NW)) {
	if (pct(50)) {
	    puts(you_didnt_move);
	    return R_BEDQUILT;
	} else {
	    return R_ORIENTAL;
	}
    }
    return 0;  /* command hasn't been processed yet */
}


static const char all_alike[] = "You are in a maze of twisty little passages, all alike.";
static const char dead_end[] = "Dead end.";

struct Place places[] = {
    /* R_LIMBO */ {
	NULL, NULL, 0, NULL
    },
    /* R_ROAD */ {
        "You're at end of road again.",
        "You are standing at the end of a road before a small brick building.\n"
        "Around you is a forest.  A small stream flows out of the building and\n"
        "down a gully.",
        F_LIGHTED | F_NOTINCAVE,
        &at_road
    },
    /* R_HILL */ {
	"You're at hill in road.",
        "You have walked up a hill, still in the forest. The road slopes back\n"
        "down the other side of the hill.  There is a building in the distance.",
	F_LIGHTED | F_NOTINCAVE,
	&at_hill
    },
    /* R_HOUSE */ {
	"You're inside building.",
        "You are inside a building, a well house for a large spring.",
	F_LIGHTED | F_NOTINCAVE | F_WATER,
	&at_house
    },
    /* R_VALLEY */ {
        "You're in valley.",
        "You are in a valley in the forest beside a stream tumbling along a\n"
        "rocky bed.",
	F_LIGHTED | F_NOTINCAVE | F_WATER,
	&at_valley
    },
    /* R_FOREST */ {
	"You're in forest.",
        "You are in open forest, with a deep valley to one side.",
	F_LIGHTED | F_NOTINCAVE,
	&at_forest
    },
    /* R_FOREST2 */ {
	"You're in forest.",
        "You are in open forest near both a valley and a road.",
	F_LIGHTED | F_NOTINCAVE,
	&at_forest2
    },
    /* R_SLIT */ {
	"You're at slit in streambed.",
        "At your feet all the water of the stream splashes into a 2-inch slit\n"
        "in the rock.  Downstream the streambed is bare rock.",
	F_LIGHTED | F_NOTINCAVE | F_WATER,
	&at_slit
    },
    /* R_DEPRESSION */ {
	"You're outside grate.",
        "You are in a 20-foot depression floored with bare dirt. Set into the\n"
        "dirt is a strong steel grate mounted in concrete.  A dry streambed\n"
        "leads into the depression.",
	F_LIGHTED | F_NOTINCAVE,
	&at_depression
    },
    /* R_INSIDE */ {
	"You're below the grate.",
        "You are in a small chamber beneath a 3x3 steel grate to the surface.\n"
	"A low crawl over cobbles leads inward to the west.",
	F_LIGHTED,
	&at_inside
    },
    /* R_COBBLES */ {
	"You're in cobble crawl.",
        "You are crawling over cobbles in a low passage. There is a dim light\n"
        "at the east end of the passage.",
	F_LIGHTED,
	&at_cobbles
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
	F_HINTABLE,
	&at_bird
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
	0, &at_nugget
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
	0, &at_epit
    },
    /* R_WPIT */ {
	"You're in west pit.",
        "You are at the bottom of the western pit in the Twopit room. There is\n"
        "a large hole in the wall about 25 feet above you.",
	0, &at_wpit
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
	0, &at_south
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
	0, &at_windoe
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
	0, &at_window
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
	F_WATER, &at_wet
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
    /* R_MAZEA42 */ { NULL, all_alike, F_HINTABLE, &at_mazea42 },
    /* R_MAZEA43 */ { NULL, all_alike, F_HINTABLE, &at_mazea43 },
    /* R_MAZEA44 */ { NULL, all_alike, F_HINTABLE, &at_mazea44 },
    /* R_MAZEA45 */ { NULL, all_alike, F_HINTABLE, &at_mazea45 },
    /* R_MAZEA46 */ { NULL, dead_end, F_HINTABLE, &at_mazea46 },
    /* R_MAZEA47 */ { NULL, dead_end, F_HINTABLE, &at_mazea47 },
    /* R_MAZEA48 */ { NULL, dead_end, F_HINTABLE, &at_mazea48 },
    /* R_MAZEA49 */ { NULL, all_alike, F_HINTABLE, &at_mazea49 },
    /* R_MAZEA50 */ { NULL, all_alike, F_HINTABLE, &at_mazea50 },
    /* R_MAZEA51 */ { NULL, all_alike, F_HINTABLE, &at_mazea51 },
    /* R_MAZEA52 */ { NULL, all_alike, F_HINTABLE, &at_mazea52 },
    /* R_MAZEA53 */ { NULL, all_alike, F_HINTABLE, &at_mazea53 },
    /* R_MAZEA54 */ { NULL, dead_end, F_HINTABLE, &at_mazea54 },
    /* R_MAZEA55 */ { NULL, all_alike, F_HINTABLE, &at_mazea55 },
    /* R_MAZEA56 */ { NULL, dead_end, F_HINTABLE, &at_mazea56 },
    /* R_MAZEA57_PIT */ {
	"You're at brink of pit.",
        "You are on the brink of a thirty foot pit with a massive orange column\n"
        "down one wall.  You could climb down here but you could not get back\n"
        "up.  The maze continues at this level.",
	0, &at_mazea57_pit
    },
    /* R_MAZEA58 */ { NULL, dead_end, F_HINTABLE, &at_mazea58 },
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
	0, &at_deadend1
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
#if 0
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
	"You are in a secret N/S canyon above a sizable passage."
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
	"You are in a large low room.  Crawls lead north, SE, and SW."
	0, &at_low
    },
    /* R_DEADEND2 */ {
	NULL, "Dead end crawl.", 0, &at_deadend2
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
	"You are at a wide place in a very tight N/S canyon."
	0, &at_nscanyonwide
    },
    /* R_TIGHTERSTILL */ {
	NULL,
	"The canyon here becomes too tight to go further south.",
	0, &at_tighterstill
    },
    /* R_TALLEWCNYN */ {
	"You are in a tall E/W canyon.",
        "You are in a tall E/W canyon. A low tight crawl goes 3 feet north and\n"
        "seems to open up."
	0, &at_tallewcnyn
    },
    /* R_DEADEND3 */ {
	"The canyon runs into a mass of boulders -- dead end.",
        "The canyon runs into a mass of boulders -- dead end.  Scratched on\n"
        "one of the boulders are the words, \"Jerry Cornelius was here.\"",
	0, &at_deadend3
    },
    /* R_MAZEA80 */ { NULL, all_alike, F_HINTABLE, &at_mazea80 },
    /* R_MAZEA81 */ { NULL, dead_end, F_HINTABLE, &at_mazea81 },
    /* R_MAZEA82 */ { NULL, dead_end, F_HINTABLE, &at_mazea82 },
    /* R_MAZEA83 */ { NULL, all_alike, F_HINTABLE, &at_mazea83 },
    /* R_MAZEA84 */ { NULL, all_alike, F_HINTABLE, &at_mazea84 },
    /* R_MAZEA85 */ { NULL, dead_end, F_HINTABLE, &at_mazea85 },
    /* R_MAZEA86 */ { NULL, dead_end, F_HINTABLE, &at_mazea86 },
    /* R_MAZEA87 */ { NULL, all_alike, F_HINTABLE, &at_mazea87 },
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
	0, &at_incline
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
	0, &at_falls
    },
    /* R_SOFT */ {
	"You're in soft room.",
        "You are in the soft room. The walls are covered with heavy curtains,\n"
        "the floor with a thick pile carpet.  Moss covers the ceiling.",
	0, &at_soft
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
	0, &at_alcove
    },
    /* R_PLOVER */ {
	"You're in Plover room.",
        "You're in a small chamber lit by an eerie green light. An extremely\n"
        "narrow tunnel exits to the west.  A dark corridor leads NE.",
	F_LIGHTED, &at_plover
    },
    /* R_DARK */ {
	"You're in Dark room.",
        "You're in the Dark room. A corridor leading south is the only exit.",
	0, &at_dark
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
        "walls are littered with bits of shells imbedded in the stone.  A\n"
        "shallow passage proceeds downward, and a somewhat steeper one leads\n"
        "up.  A low hands and knees passage enters from the south.",
	0, &at_shell
    },
    /* R_RAGGEDCORRID */ {
	NULL,
	"You are in a long sloping corridor with ragged sharp walls.",
	0, &at_raggedcorrid
    },
    /* R_CULDESAC */ {
	NULL,
	"You are in a cul-de-sac about eight feet across.",
	0, &at_culdesac
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
	0, &at_mazed107
    },
    /* R_WITT */ {
	"You're at Witt's End.",
        "You are at Witt's End. Passages lead off in *all* directions.",
	0, &at_witt
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
	0, &at_stalact
    },
    /* R_MAZED112 */ {
	NULL,
	"You are in a little maze of twisting passages, all different."
	0, &at_mazed112
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
	F_WATER, &at_res
    },
    /* R_RESERVOIR_N */ {
	"You are at the northern end of the reservoir.",
        "You are at the northern end of a large underground reservoir.  Across\n"
        "the water to the south, a dark passage is visible.  Another passage\n"
        "leads north from here.  Large, clawed tracks are visible in the damp\n"
        "ground, leading from the passage into the water.",
	F_WATER, &at_reservoir_n
    },
    /* R_WARM */ {
	"You're in small, warm chamber.",
        "You are in a small chamber with warm walls.  Mist drifts into the\n"
        "chamber from a passage entering from the south and evaporates in\n"
        "the heat.  Another passage leads out to the northeast.",
	0, &at_warm
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
	0, &at_balcony
    },
    /* R_FAKE_SLIT */ {
	"You're at slit in streambed.",
        "At your feet all the water of the stream splashes into a 2-foot slit\n"
        "in the rock.  Downstream the streambed is bare rock.",
	F_LIGHTED | F_NOTINCAVE, &at_fake_slit
    },
    /* R_CYLINDRICAL */ {
	"You're in cylindrical chamber.",
        "You are in a small cylindrical room with very smooth walls and a flat\n"
        "floor and ceiling.  There are no exits visible anywhere.",
	F_LIGHTED, &at_cylindrical
    },
    /* R_PIRATES_NEST */ { NULL, dead_end, 0, &at_pirates_nest },
    /* R_SWOFCHASM */ {
	"You're on SW side of chasm.",
        "You are on one side of a large, deep chasm. A heavy white mist rising\n"
        "up from below obscures all view of the far side.  A SW path leads away\n"
        "from the chasm into a winding corridor.",
	0, &at_swofchasm
    },
    /* R_NEOFCHASM */ {
	"You're on NE side of chasm.",
        "You are on the far side of the chasm. A NE path leads away from the\n"
        "chasm on this side.",
	0, &at_neofchasm
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
	0, &at_corridor
    },
    /* R_FORK */ {
	"You're at fork in path.",
        "The path forks here. The left fork leads northeast. A dull rumbling\n"
        "seems to get louder in that direction.  The right fork leads southeast\n"
        "down a gentle slope.  The main corridor enters from the west.",
	0, &at_fork
    },
    /* R_WARMJUNCTN */ {
	"You're at junction with warm walls.",
        "The walls are quite warm here. From the north can be heard a steady\n"
        "roar, so loud that the entire cave seems to be trembling.  Another\n"
        "passage leads south, and a low crawl goes east.",
	0, &at_warmjunctn
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
    	0, &at_view
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
	0, &at_faces
    },
    /* R_BY_FIGURE */ {
	"You're at north end of the Valley of the Stone Faces.",
        "You are standing at the north end of the Valley of the Stone Faces.\n"
        "Above you, an incredible bas-relief statue of an immense minotaur\n"
        "has been carved out of the rock.  At least sixty feet high, it sits\n"
        "gazing down at you with a faint but definite expression of amusement.\n"
        "Between its feet and the floor is a rock wall about ten feet high\n"
        "which extends across the entire north end of the valley.",
	0, &at_by_figure
    },
    /* R_PLAIN_1 */ {
	"You're at south end of fog-filled room.",
        "You are standing at the southern end of what appears to be a\n"
        "large room filled with multicolored fog.  The sides and far end\n"
        "of the room cannot be seen due to the thickness of the fog - it's\n"
        "a real pea-souper (even to the color in places!).  A passage leads\n"
        "back to the south;  a dull rumbling sound issues from the passage.",
	0, &at_plain_1
    },
    /* R_PLAIN_2 */ {
	NULL, NULL, 0, &at_plain_2
    },
    /* R_PLAIN_3 */ {
	"You're in foggy room by cairn of rocks.",
        "You are standing in a fog-filled room next to a tall cairn of glowing\n"
        "rocks.  An opening in the cairn leads down to a dark passage.",
	0, &at_plain_3
    },
    /* R_NONDESCRIPT */ {
	"You're in nondescript chamber.",
        "You're in a small, nondescript chamber.  A dark passage leads up\n"
        "and to the south, and a wide but low crawl leads north.",
	0, &at_nondescript
    },
    /* R_PENTAGRAM */ {
	"You're in room with pentagram.",
        "You're in a small room with a very smooth rock floor, onto which\n"
        "has been marked a pentagram.  A low crawl leads out to the west, and\n"
        "a crack in the rock leads north.",
	0, &at_pentagram
    },
    /* R_CHIMNEY */ {
	"You're at the end of the crack, at the bottom of the chimney.",
        "The crack in the rock ends here, but a narrow chimney leads up.  You\n"
        "should be able to climb it.",
	0, &at_chimney
    },
    /* R_TUBE */ {
	"You're in lava tube at top of chimney.",
        "You're at the top of a narrow chimney in the rock.  A cylindrical\n"
        "tube composed of hardened lava leads south.",
	0, &at_tube
    },
    /* R_TUBE_SLIDE */ {
	"You're at steep slide in lava tube.",
        "The lava tube continues down and to the south, but it becomes very\n"
        "steep here - if you go down it you probably won't be able to get\n"
        "back up.",
	0, &at_tube_slide
    },
    /* R_BASQUE_1 */ {
	"You're in rough and narrow passage.",
        "You are in a narrow and rough passage running north and south.\n"
        "A dull rumbling sound can be heard from the south.",
	0, &at_basque_1
    },
    /* R_BASQUE_2 */ {
    },
    /* R_BASQUE_FORK */ {
    },
    /* R_ON_STEPS */ {
    },
    /* R_STEPS_EXIT */ {
    },
    /* R_STORAGE */ {
    },
    /* R_FAKE_Y2 */ {
    },
    /* R_FAKE_JUMBLE */ {
    },
    /* R_CATACOMBS1 */ {
    },
    /* R_CATACOMBS2 */ {
    },
    /* R_CATACOMBS3 */ {
    },
    /* R_CATACOMBS4 */ {
    },
    /* R_CATACOMBS5 */ {
    },
    /* R_CATACOMBS6 */ {
    },
    /* R_CATACOMBS7 */ {
    },
    /* R_CATACOMBS8 */ {
    },
    /* R_CATACOMBS9 */ {
    },
    /* R_CATACOMBS10 */ {
    },
    /* R_CATACOMBS11 */ {
    },
    /* R_CATACOMBS12 */ {
    },
    /* R_CATACOMBS13 */ {
    },
    /* R_CATACOMBS14 */ {
    },
    /* R_CATACOMBS15 */ {
    },
    /* R_CATACOMBS16 */ {
    },
    /* R_CATACOMBS17 */ {
    },
    /* R_CATACOMBS18 */ {
    },
    /* R_CATACOMBS19 */ {
    },
    /* R_AUDIENCE */ {
    },
    /* R_AUDIENCE_E */ {
    },
    /* R_BANSHEE */ {
    },
    /* R_GOLDEN */ {
    },
    /* R_ARABESQUE */ {
    },
    /* R_TRANSLUCENT */ {
    },
    /* R_CHAMBER */ {
    },
    /* R_LIME */ {
    },
    /* R_FBARR */ {
    },
    /* R_BARR */ {
    },
    /* R_MAZED131 */ {
    },
    /* R_MAZED132 */ {
    },
    /* R_MAZED133 */ {
    },
    /* R_MAZED134 */ {
    },
    /* R_MAZED135 */ {
    },
    /* R_MAZED136 */ {
    },
    /* R_MAZED137 */ {
    },
    /* R_MAZED138 */ {
    },
    /* R_MAZED139 */ {
    },
    /* R_PONY */ {
    },
    /* R_SANDSTONE */ {
    },
    /* R_MORION */ {
    },
    /* R_VAULT */ {
    },
    /* R_PEELGRUNT */ {
    },
    /* R_INSAFE */ {
    },
    /* R_CORRID_1 */ {
    },
    /* R_CORRID_2 */ {
    },
    /* R_TOOL */ {
    },
    /* R_CORRID_3 */ {
    },
    /* R_CUBICLE */ {
    },
    /* R_SPHERICAL */ {
    },
    /* R_TUNNEL_1 */ {
    },
    /* R_GLASSY */ {
    },
    /* R_LAIR */ {
    },
    /* R_BRINK_1 */ {
    },
    /* R_BRINK_2 */ {
    },
    /* R_ICE */ {
    },
    /* R_SLIDE */ {
    },
    /* R_ICECAVE1 */ {
    },
    /* R_ICECAVE1A */ {
    },
    /* R_ICECAVE2 */ {
    },
    /* R_ICECAVE2A */ {
    },
    /* R_ICECAVE3 */ {
    },
    /* R_ICECAVE3A */ {
    },
    /* R_ICECAVE4 */ {
    },
    /* R_ICECAVE5 */ {
    },
    /* R_ICECAVE6 */ {
    },
    /* R_ICECAVE7 */ {
    },
    /* R_ICECAVE8 */ {
    },
    /* R_ICECAVE9 */ {
    },
    /* R_ICECAVE10 */ {
    },
    /* R_ICECAVE11 */ {
    },
    /* R_ICECAVE12 */ {
    },
    /* R_ICECAVE12A */ {
    },
    /* R_ICECAVE13 */ {
    },
    /* R_ICECAVE14 */ {
    },
    /* R_ICECAVE15 */ {
    },
    /* R_ICECAVE15A */ {
    },
    /* R_ICECAVE16 */ {
    },
    /* R_ICECAVE17 */ {
    },
    /* R_ICECAVE18 */ {
    },
    /* R_ICECAVE19 */ {
    },
    /* R_ICECAVE20 */ {
    },
    /* R_ICECAVE21 */ {
    },
    /* R_ICECAVE22 */ {
    },
    /* R_ICECAVE23 */ {
    },
    /* R_ICECAVE24 */ {
    },
    /* R_ICECAVE25 */ {
    },
    /* R_ICECAVE26 */ {
    },
    /* R_ICECAVE27 */ {
    },
    /* R_ICECAVE28 */ {
    },
    /* R_ICECAVE28A */ {
    },
    /* R_ICECAVE29 */ {
    },
    /* R_ICECAVE30 */ {
    },
    /* R_BRINK_3 */ {
    },
    /* R_CRACK_1 */ {
    },
    /* R_CRACK_2 */ {
    },
    /* R_CRACK_3 */ {
    },
    /* R_CRACK_4 */ {
    },
    /* R_ARCH_COR_1 */ {
    },
    /* R_ARCH_COR_2 */ {
    },
    /* R_ARCH_FORK */ {
    },
    /* R_JONAH */ {
    },
    /* R_IN_JONAH */ {
    },
    /* R_FOURIER */ {
    },
    /* R_SHELF */ {
    },
    /* R_BEACH */ {
    },
    /* R_PLATFORM */ {
    },
#endif
};


struct ObjectData objs[] = {
#define XX {0,0,0,0}
    { XX, 0, NULL /* NOTHING */, 0, R_LIMBO },
    { XX, 0, NULL /* GRATE */, F_SCHIZOID, R_DEPRESSION },
    { XX, 0, NULL /* STEPS */, F_INVISIBLE|F_SCHIZOID, R_SPIT },
    { XX, 0, NULL /* DOOR */, 0, R_IMMENSENSPASS },
    { XX, 0, NULL /* SNAKE */, 0, R_HMK },
    { XX, 0, NULL /* FISSURE */, F_INVISIBLE, R_EFISS },
    { XX, 0, NULL /* TABLET */, 0, R_DARK },
    { XX, 0, NULL /* MIRROR */, F_INVISIBLE, R_MIRROR },
    { XX, 0, NULL /* PLANT */, 0, R_WPIT },
    { XX, 0, NULL /* PLANT2 */, F_INVISIBLE|F_SCHIZOID, R_W2PIT },
    { XX, 0, NULL /* STALACTITE */, F_INVISIBLE, R_STALACT },
    { XX, 0, NULL /* FOG */, F_INVISIBLE|F_SCHIZOID, R_PLAIN_2 },
    { XX, 0, NULL /* GLOW */, 0, R_LIMBO },
    { XX, 0, NULL /* SHADOW */, F_SCHIZOID, R_WINDOE },
    { XX, 0, NULL /* BLOB */, 0, R_LIMBO },
    { XX, 0, NULL /* DRAWINGS */, F_INVISIBLE, R_ORIENTAL },
    { XX, 0, NULL /* PIRATE */, F_INVISIBLE, R_LIMBO },
    { XX, 0, NULL /* DRAGON */, F_SCHIZOID, R_SECRETCYNNE1 },
    { XX, 0, NULL /* CHASM */, F_SCHIZOID, R_SWOFCHASM },
    { XX, 0, NULL /* TROLL */, F_SCHIZOID, R_SWOFCHASM },
    { XX, 0, NULL /* TROLL2 */, F_SCHIZOID, R_LIMBO },
    { XX, 0, NULL /* OGRE */, 0, R_GLASSY },
    { XX, 0, NULL /* BASILISK */, F_SCHIZOID, R_BASQUE_1 },
    { XX, 0, NULL /* GONG */, 0, R_RESERVOIR_N },
    { XX, 0, NULL /* DJINN */, 0, R_LIMBO },
    { XX, 0, NULL /* TURTLE */, 0, R_LIMBO },
    { XX, 0, NULL /* MESSAGE */, 0, R_LIMBO },
    { XX, 0, NULL /* GEYSER */, F_INVISIBLE|F_SCHIZOID, R_VIEW },
    { XX, 0, NULL /* STATUE */, F_INVISIBLE, R_BY_FIGURE },
    { XX, 0, NULL /* QUICKSAND */, F_INVISIBLE|F_SCHIZOID, R_ARCH_COR_1 },
    { XX, 0, NULL /* SLIME */, 0, R_CRACK_2 },
    { XX, 0, NULL /* PONY */, 0, R_PONY },
    { XX, 0, NULL /* SAFE */, F_SCHIZOID, R_VAULT },
    { XX, 0, NULL /* THRONE */, F_INVISIBLE, R_AUDIENCE_E },
    { XX, 0, NULL /* SKELETON */, 0, R_AUDIENCE_E },
    { XX, 0, NULL /* BEAR */, F_PORTABLE, R_BARR },
    { XX, 0, NULL /* BATTERIES */, F_PORTABLE, R_LIMBO },
    { XX, 0, NULL /* MOSS */, F_INVISIBLE, R_SOFT },
    { XX, 0, NULL /* DINGHY */, 0, R_BEACH },
    { XX, 0, "Worthless shards of pottery", F_PORTABLE, R_LIMBO },
    { XX, 0, "Set of keys", F_PORTABLE, R_HOUSE },
    { XX, 0, "Brass lantern", F_PORTABLE, R_HOUSE },
    { XX, 0, "Wicker cage", F_PORTABLE|F_UNSTABLE, R_COBBLES },
    { XX, 0, "Little bird in cage", F_PORTABLE, R_BIRD },
    { XX, 0, "Black rod", F_PORTABLE, R_DEBRIS },
    { XX, 0, "Metal plate", F_PORTABLE|F_UNSTABLE, R_STORAGE },
    { XX, 0, "Black rod", F_PORTABLE, R_LIMBO },
    { XX, 0, "Velvet pillow", F_PORTABLE, R_SOFT },
    { XX, 0, "Giant clam  >grunt!<", F_PORTABLE, R_SHELL },
    { XX, 0, "Giant oyster  >groan!<", F_PORTABLE, R_LIMBO },
    { XX, 0, "\"Spelunker Today\"", F_PORTABLE, R_ANTE },
    { XX, 0, NULL /* KNIFE */, F_PORTABLE, R_LIMBO },
    { XX, 0, "Tasty food", F_PORTABLE, R_HOUSE },
    { XX, 0, NULL /* BOTTLE */, F_PORTABLE, R_HOUSE },
    { XX, 0, NULL /* WATER */, F_PORTABLE, R_LIMBO },
    { XX, 0, NULL /* OIL */, F_PORTABLE, R_LIMBO },
    { XX, 0, "Earthenware flask", F_PORTABLE|F_UNSTABLE, R_ARABESQUE },
    { XX, 0, "Dwarf's axe", F_PORTABLE, R_LIMBO },
    { XX, 0, "Singing sword", F_PORTABLE, R_SANDSTONE },
    { XX, 0, "Several dragon's teeth", F_PORTABLE|F_UNSTABLE, R_LIMBO },
    { XX, 0, "Vial of oily liquid", F_PORTABLE, R_SPHERICAL },
    { XX, 0, "Mushroom", F_PORTABLE|F_UNSTABLE, R_CUBICLE },
    { XX, 0, NULL /* GOBLINS */, 0, R_LIMBO },
    { XX, 0, NULL /* DWARF */, F_INVISIBLE, R_LIMBO },
    { XX, 0, "Bag filled with pieces of eight", F_PORTABLE|F_UNSTABLE, R_BEACH },
    { XX, 0, "Crown", F_PORTABLE|F_UNSTABLE, R_INSAFE },
    { XX, 0, "Large gold nugget", F_PORTABLE, R_NUGGET },
    { XX, 0, "Several diamonds", F_PORTABLE, R_WFISS },
    { XX, 0, "Bars of silver", F_PORTABLE, R_NS },
    { XX, 0, "Precious jewelry", F_PORTABLE, R_SOUTH },
    { XX, 0, "Rare coins", F_PORTABLE, R_WEST },
    { XX, 0, "Treasure chest", F_PORTABLE|F_UNSTABLE, R_LIMBO },
    { XX, 0, "Golden eggs", F_PORTABLE, R_GIANT },
    { XX, 0, "Jeweled trident", F_PORTABLE, R_FALLS },
    { XX, 0, "Helmet", F_PORTABLE, R_MORION },
    { XX, 0, "Ming vase", F_PORTABLE, R_ORIENTAL },
    { XX, 0, "Egg-sized emerald", F_PORTABLE, R_PLOVER },
    { XX, 0, "Sapphire sceptre", F_PORTABLE, R_AUDIENCE_E },
    { XX, 0, "Ruby-covered toy yacht", F_PORTABLE|F_UNSTABLE, R_NONDESCRIPT },
    { XX, 0, "Platinum pyramid", F_PORTABLE, R_DARK },
    { XX, 0, "Glistening pearl", F_PORTABLE, R_LIMBO },
    { XX, 0, "Persian rug", F_PORTABLE, R_LIMBO },
    { XX, 0, "Rare spices", F_PORTABLE, R_CHAMBER },
    { XX, 0, "Ancient Indian turquoise beads.", F_PORTABLE|F_UNSTABLE, R_BALCONY },
    { XX, 0, "Golden chain", F_PORTABLE, R_BARR },
    { XX, 0, "Mithril ring", F_PORTABLE, R_LIMBO },
    { XX, 0, "Scrimshaw spyglass", F_PORTABLE, R_IN_JONAH },
    { XX, 0, "Rock-crystal sculpture", F_PORTABLE|F_UNSTABLE, R_ICECAVE14 },
    { XX, 0, "Jade bracelet", F_PORTABLE, R_TRANSLUCENT },
    { XX, 0, "Casket of opals", F_PORTABLE|F_UNSTABLE, R_CRACK_4 }
#undef XX
};


#include <stdbool.h>
#include <stdio.h>
#include "advdat.h"

bool pct(int percent);
bool used_movement_verb(VerbWord where_to);
bool used_movement_placeword(Location where_to);
void say_foof(void);
void finis(void);
extern VerbWord verbosity_mode;
extern int closure;
extern bool gave_up;


int at_road(void)
{
    if (used_movement_placeword(R_ROAD)) return R_HILL;
    if (used_movement_verb(WEST)) return R_HILL;
    if (used_movement_verb(UP)) return R_HILL;
    if (used_movement_verb(IN)) return R_HOUSE;
    if (used_movement_placeword(R_HOUSE)) return R_HOUSE;
    if (used_movement_verb(EAST)) return R_HOUSE;
    if (used_movement_verb(DOWNSTREAM)) return R_VALLEY;
    if (used_movement_verb(GULLY)) return R_VALLEY;
    if (used_movement_verb(STREAM)) return R_VALLEY;
    if (used_movement_verb(SOUTH)) return R_VALLEY;
    if (used_movement_verb(DOWN)) return R_VALLEY;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb(NORTH)) return R_FOREST;
    if (used_movement_verb(EAST)) return R_FOREST;
    if (used_movement_placeword(R_DEPRESSION)) return R_DEPRESSION;
    return 0;  /* command hasn't been processed yet */
}

int at_hill(void)
{
    if (used_movement_placeword(R_ROAD)) return R_ROAD;
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb(FORWARD)) return R_ROAD;
    if (used_movement_verb(EAST)) return R_ROAD;
    if (used_movement_verb(NORTH)) return R_ROAD;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb(NORTH)) return R_FOREST;
    if (used_movement_verb(SOUTH)) return R_FOREST;
    return 0;  /* command hasn't been processed yet */
}

int at_house(void)
{
    if (used_movement_placeword(R_ROAD)) return R_ROAD;
    if (used_movement_verb(OUT)) return R_ROAD;
    if (used_movement_verb(OUTDOORS)) return R_ROAD;
    if (used_movement_verb(WEST)) return R_ROAD;
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
    if (used_movement_verb(STREAM) || used_movement_verb(DOWNSTREAM)) {
	puts("The stream flows out through a pair of 1 foot diameter sewer pipes.\n"
	     "It would be advisable to leave via the exit.");
	return R_HOUSE;
    }
    return 0;  /* command hasn't been processed yet */
}

int at_valley(void)
{
    if (used_movement_verb(UPSTREAM)) return R_ROAD;
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb(NORTH)) return R_ROAD;
    if (used_movement_placeword(R_FOREST)) return R_FOREST;
    if (used_movement_verb(EAST)) return R_FOREST;
    if (used_movement_verb(WEST)) return R_FOREST;
    if (used_movement_verb(UP)) return R_FOREST;
    if (used_movement_placeword(R_DEPRESSION)) return R_DEPRESSION;
    if (used_movement_verb(SOUTH) || used_movement_verb(DOWN) || used_movement_verb(DOWNSTREAM))
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
    if (used_movement_placeword(R_FOREST) || used_movement_verb(FORWARD) || used_movement_verb(NORTH))
	return pct(50) ? R_FOREST : R_FOREST2;
    return 0;  /* command hasn't been processed yet */
}

int at_forest2(void)
{
    if (used_movement_placeword(R_ROAD)) return R_ROAD;
    if (used_movement_verb(NORTH)) return R_ROAD;
    if (used_movement_placeword(R_VALLEY)) return R_VALLEY;
    if (used_movement_verb(EAST)) return R_VALLEY;
    if (used_movement_verb(WEST)) return R_VALLEY;
    if (used_movement_verb(DOWN)) return R_VALLEY;
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
    if (used_movement_verb(EAST)) return R_FOREST;
    if (used_movement_verb(WEST)) return R_FOREST;
    if (used_movement_verb(DOWNSTREAM)) return R_DEPRESSION;
    if (used_movement_verb(ROCK)) return R_DEPRESSION;
    if (used_movement_verb(BED)) return R_DEPRESSION;
    if (used_movement_verb(SOUTH)) return R_DEPRESSION;
    if (used_movement_placeword(R_SLIT) || used_movement_verb(STREAM) ||
	    used_movement_verb(DOWN) || used_movement_verb(IN)) {
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
    if (used_movement_placeword(R_SLIT) || used_movement_verb(STREAM) ||
	    used_movement_verb(DOWN) || used_movement_verb(IN)) {
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
    if (used_movement_verb(EAST)) return R_FOREST2;
    if (used_movement_verb(WEST)) return R_FOREST;
    if (used_movement_verb(SOUTH)) return R_FOREST2;
    if (used_movement_placeword(R_HOUSE)) return R_ROAD;
    if (used_movement_verb(UPSTREAM) || used_movement_verb(GULLY) || used_movement_verb(NORTH) || used_movement_placeword(R_SLIT)) {
	return (closure < 2) ? R_SLIT : R_FAKE_SLIT;
    }
    if (used_movement_verb(IN) || used_movement_verb(DOWN)) {
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
    if (used_movement_verb(UP) ||
	used_movement_verb(OUT) ||
	used_movement_verb(SURFACE)) {
	if (objs[GRATE].prop == 1) {
	    return R_DEPRESSION;
	} else {
	    puts("You can't go through a locked steel grate!");
	    return R_INSIDE;
	}
    }
    if (used_movement_verb(CRAWL)) return R_COBBLES;
    if (used_movement_placeword(R_COBBLES)) return R_COBBLES;
    if (used_movement_verb(IN)) return R_COBBLES;
    if (used_movement_verb(WEST)) return R_COBBLES;
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
#if 0
    /* R_DEBRIS */ {
    },
    /* R_AWK */ {
    },
    /* R_BIRD */ {
    },
    /* R_SPIT */ {
    },

    /* R_EMIST */ {
    },

    /* R_NUGGET */ {
    },
    /* R_EFISS */ {
    },
    /* R_WFISS */ {
    },
    /* R_WMIST */ {
    },

    /* R_LIKE1 */ {
    },
    /* R_LIKE2 */ {
    },
    /* R_LIKE3 */ {
    },
    /* R_LIKE4 */ {
    },
    /* R_LIKE5 */ {
    },
    /* R_LIKE6 */ {
    },
    /* R_LIKE7 */ {
    },

    /* R_LIKE8 */ {
    },
    /* R_LIKE9 */ {
    },
    /* R_LIKE10 */ {
    },
    /* R_LIKE11 */ {
    },
    /* R_LIKE12 */ {
    },
    /* R_LIKE13 */ {
    },
    /* R_LIKE14 */ {
    },

    /* R_BRINK */ {
    },
    /* R_ELONG */ {
    },
    /* R_WLONG */ {
    },

    /* R_DIFF0 */ {
    },
    /* R_DIFF1 */ {
    },
    /* R_DIFF2 */ {
    },
    /* R_DIFF3 */ {
    },
    /* R_DIFF4 */ {
    },
    /* R_DIFF5 */ {
    },

    /* R_DIFF6 */ {
    },
    /* R_DIFF7 */ {
    },
    /* R_DIFF8 */ {
    },
    /* R_DIFF9 */ {
    },
    /* R_DIFF10 */ {
    },

    /* R_PONY */ {
    },
    /* R_CROSS */ {
    },
    /* R_HMK */ {
    },
    /* R_WEST */ {
    },
    /* R_SOUTH */ {
    },
    /* R_NS */ {
    },
    /* R_Y2 */ {
    },
    /* R_JUMBLE */ {
    },
    /* R_WINDOE */ {
    },

    /* R_DIRTY */ {
    },
    /* R_CLEAN */ {
    },
    /* R_WET */ {
    },
    /* R_DUSTY */ {
    },
    /* R_COMPLEX */ {
    },

    /* R_SHELL */ {
    },
    /* R_ARCH */ {
    },
    /* R_RAGGED */ {
    },
    /* R_SAC */ {
    },
    /* R_ANTE */ {
    },
    /* R_WITT */ {
    },

    /* R_BEDQUILT */ {
    },
    /* R_CHEESE */ {
    },
    /* R_SOFT */ {
    },

    /* R_E2PIT */ {
    },
    /* R_W2PIT */ {
    },
    /* R_EPIT */ {
    },
    /* R_WPIT */ {
    },

    /* R_NARROW */ {
    },
    /* R_GIANT */ {
    },
    /* R_BLOCK */ {
    },
    /* R_IMMENSE */ {
    },

    /* R_FALLS */ {
    },
    /* R_STEEP */ {
    },
    /* R_ABOVEP */ {
    },
    /* R_SJUNC */ {
    },

    /* R_TITE */ {
    },
    /* R_LOW */ {
    },
    /* R_CRAWL */ {
    },
    /* R_WINDOW */ {
    },

    /* R_ORIENTAL */ {
    },
    /* R_MISTY */ {
    },
    /* R_ALCOVE */ {
    },
    /* R_PROOM */ {
    },
    /* R_DROOM */ {
    },

    /* R_SLAB */ {
    },
    /* R_ABOVER */ {
    },
    /* R_MIRROR */ {
    },
    /* R_RES */ {
    },

    /* R_SCAN1 */ {
    },
    /* R_SCAN2 */ {
    },
    /* R_SCAN3 */ {
    },
    /* R_SECRET */ {
    },

    /* R_WIDE */ {
    },
    /* R_TIGHT */ {
    },
    /* R_TALL */ {
    },
    /* R_BOULDERS */ {
    },

    /* R_SCORR */ {
    },
    /* R_SWSIDE */ {
    },

    /* R_DEAD0 */ {
    },
    /* R_DEAD1 */ {
    },
    /* R_PIRATES_NEST */ {
    },
    /* R_DEAD3 */ {
    },
    /* R_DEAD4 */ {
    },
    /* R_DEAD5 */ {
    },

    /* R_DEAD6 */ {
    },
    /* R_DEAD7 */ {
    },
    /* R_DEAD8 */ {
    },
    /* R_DEAD9 */ {
    },
    /* R_DEAD10 */ {
    },
    /* R_DEAD11 */ {
    },

    /* R_NESIDE */ {
    },
    /* R_CORR */ {
    },
    /* R_FORK */ {
    },
    /* R_WARM */ {
    },
    /* R_VIEW */ {
    },
    /* R_CHAMBER */ {
    },

    /* R_LIME */ {
    },
    /* R_FBARR */ {
    },
#endif
};

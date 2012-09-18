
#include <stdbool.h>
#include "advdat.h"

/*========== Functions independent of objs(t). ============================
 */

bool is_hinged(ObjectWord t)
{
    switch (t) {
	case GRATE: case CAGE: case CLAM: case OYSTER:
	case BOTTLE: case RUSTY_DOOR: case TINY_DOOR: case HUGE_DOOR:
	case BOOTH_DOOR: case CHAIN: case CASK: case SACK: case BOOK:
	case REPO_BOOK: case SAFE: case CANISTER:
	    return true;
	default:
	    return false;
    }
}

bool has_lock(ObjectWord t)
{
    switch (t) {
	case GRATE: case TINY_DOOR: case HUGE_DOOR:
	case CHEST: case CHAIN: case SAFE:
	    return true;
	default:
	    return false;
    }
}

bool is_edible(ObjectWord t)
{
    switch (t) {
	case FOOD: case PLANT: case SPICES: case HONEY:
	case BIRD: case MUSHROOMS: case CAKES:
	    return true;
	default:
	    return false;
    }
}

bool is_readable(ObjectWord t)
{
    switch (t) {
	case TABLET: case CLAM: case OYSTER: case MAG: case MESSAGE:
	case BOOK: case REPO_BOOK: case POSTER: case CARVING:
	    return true;
	default:
	    return false;
    }
}

bool is_living(ObjectWord t)
{
    switch (t) {
        case SNAKE: case CLAM: case OYSTER: case DWARF:
        case PIRATE: case DRAGON: case TROLL: case BEAR:
        case BEES: /* but *not* GNOME or DOG */
	    return true;
	default:
	    return false;
    }
}

bool is_wearable(ObjectWord t)
{
    switch (t) {
	case CLOAK: case JEWELS: case CROWN: case SHOES:
	case RING: case CLOVER:
	    return true;
	default:
	    return false;
    }
}

bool is_plural(ObjectWord t)
{
    switch (t) {
	case STEPS: case DRAWINGS: case BATTERIES: case FLOWERS:
	case JEWELS: case COINS: case EGGS: case SHOES: case BEES:
	case KEYS: case MUSHROOMS: case CAKES:
	    /* but not DIAMONDS or MAGAZINES or SPICES */
	    return true;
	default:
	    return false;
    }
}

bool is_treasure(ObjectWord t)
{
    switch (t) {
	case FLOWERS: case GOLD: case DIAMONDS: case HORN: case JEWELS:
	case COINS: case CHEST: case EGGS: case TRIDENT: case VASE:
	case EMERALD: case PYRAMID: case PEARL: case RUG: case SPICES:
	case CHAIN: case SWORD: case CROWN: case SHOES: case LYRE:
	case SAPPHIRE: case GRAIL: case CASK: case RING: case CLOVER:
	case TREE: case DROPLET: case BOOK: case RADIUM: case BALL:
	    return true;
	default:
	    return false;
    }
}

bool is_vessel(ObjectWord t)
{
    switch (t) {
	case LAMP: case CAGE: case BOTTLE: case MACHINE: case BOAT:
	case CHEST: case VASE: case GRAIL: case CASK: case SACK:
	case SAFE: case CANISTER:
	    return true;
	default:
	    return false;
    }
}

bool is_small(ObjectWord t)
{
    switch (t) {
	case LAMP: case ROD: case ROD2: case PILLOW: case MAG:
	case FOOD: case BOTTLE: case AXE: case BATTERIES: case CLOAK:
	case DIAMONDS: case HORN: case JEWELS: case COINS: case EGGS:
	case VASE: case EMERALD: case PYRAMID: case PEARL: case SPICES:
        case CHAIN: case SWORD:/*!*/ case CROWN: case SHOES: case LYRE:
        case SAPPHIRE: case GRAIL: case RING: case CLOVER: case TREE:
	case DROPLET: case BIRD: case KEYS: case MUSHROOMS: case CAKES:
	case SACK: case BOOK: case POSTER:/*!*/ case BROOM: case CANISTER:
	case RADIUM: case BALL:  /* but not REPO_BOOK */
	    return true;
	default:
	    return false;
    }
}

bool is_opaque(ObjectWord t)
{
    return (t == CHEST || t == SACK || t == SAFE || t == CANISTER);
}


/*========== Functions dependent on objs(t). ==============================
 */

/* Return true if t is at loc, or being carried. */
bool here(ObjectWord t, Location loc)
{
    return toting(t) || there(t, loc);
}

bool holding(ObjectWord t)
{
    /* "Holding" does not include "carrying in an open container". */
    return (objs(t).place == R_INHAND);
}

bool enclosed(ObjectWord t)
{
    return (objs(t).place < -1);
}

bool is_ajar(ObjectWord t)
{
    return (objs(t).flags & F_OPEN) || (is_vessel(t) && !is_hinged(t));
}

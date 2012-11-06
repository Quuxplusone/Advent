
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "advdat.h"

/*========== Functions independent of objs(t). ============================
 */

bool is_hinged(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    switch (t) {
        case SNAKE: case CLAM: case OYSTER: case DWARF:
        case PIRATE: case DRAGON: case TROLL: case BEAR:
        case BEES: /* but *not* GNOME or DOG or WUMPUS */
	    return true;
	default:
	    return false;
    }
}

bool is_wearable(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    switch (t) {
	case LAMP: case CAGE: case BOTTLE: case MACHINE: case BOAT:
	case CHEST: case VASE: case GRAIL: case CASK: case SACK:
	case SAFE: case CANISTER:
	    return true;
	default:
	    return false;
    }
}

bool fits_inside(ObjectWord obj, ObjectWord iobj)
{
    /* Long misses the BEAR case, and in addition checks only if you are
     * "holding" the bear (rather than "toting") during the bridge crossing.
     * These two facts allow you to smuggle the bear across the bridge.
     * This is cute, but it's obviously a bug. (You can also put the bear
     * in the chest and throw it to the troll; nothing unusual happens.) */
    if (obj == BEAR) return false;

    /* Long allows the sequence
     * GET POLE. GET BOAT. PUT POLE IN CHEST. THROW CHEST. NORTH.
     * so that you can move around in the boat without the pole.
     * This is clearly a bug, but it's easy to fix. */
    if (obj == POLE) return false;

    /* Long mishandles PUT X IN BOTTLE for non-liquid X, leading to
     * major mischief; see my comment in attempt_insert_into().
     * For example:
     *   "PUT SACK IN BOTTLE; PUT BOTTLE IN SACK" naturally causes both
     * objects to pop out of existence. This codepath is otherwise safe.
     *   "PUT NUGGET IN CASK" causes the FILL logic to fill the cask with
     * the pseudo-object "NUGGET_IN_CASK", a.k.a. NUGGET+1, a.k.a. DIAMONDS.
     * However, since that logic expects its object to be in limbo, it
     * doesn't bother to update ATLOC/LINK. So DIAMONDS ends up in a weird
     * state where it's in the ATLOC/LINK linked list for its old location
     * while simultaneously being in the HOLDER/HLINK list for CASK. Meanwhile,
     * since diamonds aren't oil or wine, PROP(CASK) was set to 1 (water).
     *   Now GET DIAMONDS. That works, but the cask is still in a bad state;
     * it's full of water (so we can't fill it again) but WATER_IN_CASK is
     * actually in limbo. Every codepath that empties the cask (limited to
     * POUR and DRINK, I think) tries to REMOVE(WATER_IN_CASK) and ends up
     * hanging the game in an infinite loop. Therefore this exploit isn't
     * the treasure factory it appears at first. */
    if (iobj == BOTTLE || iobj == CASK || iobj == VASE || iobj == GRAIL) return false;

    if (iobj == CAGE) return (obj == BIRD);
    if (iobj == CANISTER) return (obj == RADIUM);
    if (iobj == SAFE && obj == VASE) return false;
    if (iobj == SAFE && obj == PILLOW) return false;
    if (iobj == BOAT || iobj == CHEST) return true;
    return is_small(obj);
}

bool is_small(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
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
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    return (t == CHEST || t == SACK || t == SAFE || t == CANISTER);
}

bool is_liquid(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    return (t == WATER || t == OIL || t == WINE) ||
           (t == WATER+1 || t == OIL+1 || t == WINE+1);
}

int weight(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    switch (t) {
        case LAMP: case CAGE: case PILLOW: case MAG: case BOTTLE:
        case FLOWERS: case CLOAK: case DIAMONDS: case PEARL:
        case SPICES: case CROWN: case SHOES: case RING:
        case CLOVER: case DROPLET: case TINY_KEY: case KEYS:
        case MUSHROOMS: case CAKES: case POSTER: case BROOM:
        case RADIUM:
            return 1;
        case ROD: case ROD2: case POLE: case FOOD: case HORN:
        case JEWELS: case VASE: case SWORD: case LYRE:
        case SAPPHIRE: case GRAIL: case WATER: case WATER_IN_CASK:
        case OIL: case OIL_IN_CASK: case WINE: case WINE_IN_CASK:
        case HONEY: case BIRD: case SACK: case BALL:
            return 2;
        case AXE: case BATTERIES: case COINS: case TRIDENT:
        case EMERALD: case RUG: case CHAIN: case CASK: case TREE:
        case SLUGS: case BOOK: case REPO_BOOK: case CANISTER:
            return 3;
        case EGGS: case PYRAMID:
            return 4;
        case CHEST:
            return 5;
        case GOLD:
            return 6;
        case CLAM: case OYSTER:
            return 7;
        default:
            assert(false);
    }
}

/*========== Functions dependent on objs(t). ==============================
 */

/* Return true if t is at loc, or being carried. */
bool here(ObjectWord t, Location loc)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    assert(R_LIMBO < loc && loc <= MAX_LOC);
    return toting(t) || is_at_loc(t, loc);
}

bool is_at_loc(ObjectWord t, Location loc)
{
    if (objs(t).base == NULL)
        return there(t, loc);
    /* Check the "alternative" objects based on this one. */
    for (ObjectWord tt = t; objs(tt).base == &objs(t); ++tt) {
        if (there(tt, loc))
            return true;
    }
    return false;
}

/* Return true if t is at loc, or being carried in an open container.
 * Such objects are "reachable", and can be used e.g. to open doors. */
bool at_hand(ObjectWord t, Location loc)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    assert(R_LIMBO < loc && loc <= MAX_LOC);
    if (there(t, loc) || holding(t)) return true;
    if (!enclosed(t)) return false;
    ObjectWord container = -objs(t).place;
    return here(container, loc) && is_ajar(container);
}

/* Return true if t is being carried (in hand or container). The object
 * might not be reachable, if the container is closed. This condition
 * is useful for the travel table (think PLOVER).
 * Long's version manually unrolls the recursion to depth 3 and then
 * returns false. */
bool toting(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    return holding(t) ||
	   (enclosed(t) && toting(-objs(t).place));
}

bool holding(ObjectWord t)
{
    /* "Holding" does not include "carrying in an open container". */
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    return (objs(t).place == R_INHAND);
}

bool enclosed(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    return (objs(t).place < -1);
}

bool is_ajar(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    switch (t) {
	case LAMP: case MACHINE: case BOAT: case VASE:
	case GRAIL:  /* These vessels are always open. */
	    return true;
	default:
	    return (objs(t).flags & F_OPEN);
    }
}

bool is_broken(ObjectWord t)
{
    assert(MIN_OBJ <= t && t <= MAX_OBJ);
    if (t == VASE && objs(VASE).prop == 2) return true;
    if (t == BOTTLE && objs(BOTTLE).prop == 3) return true;
    return false;
}

int burden(ObjectWord t)
{
    if (t == 0) {
	/* Compute the total weight of the player's inventory. */
	int result = 0;
	for (ObjectWord i = MIN_OBJ; i <= MAX_OBJ; ++i) {
            /* Long erroneously uses TOTING instead of HOLDNG, and skips the
	     * contents of the boat. This double-counts objects contained
	     * within the sack; counts objects contained within the sack
	     * even when the sack is the boat; and counts the weight of the
	     * empty boat itself. My fix counts everything once, and doesn't
	     * count the boat or its contents. */
	    if (holding(i) && i != BOAT)
		result += weight(i);
	}
	return result;
    } else {
	/* Compute the weight of this one object and its contents.
	 * The boat is a special case; we don't count its contents. */
	int result = weight(t);
	if (t == BOAT) return result;
	/* Long doesn't recurse here, due to Fortran's lack of recursion. */
	for (struct ObjectData *p = objs(t).contents; p != NULL; p = p->link) {
	    result += weight(p - &objs(MIN_OBJ) + MIN_OBJ);
	}
	return result;
    }
}

/*========== Functions that mutate objs(t). ===============================
 */

void drop(ObjectWord t, Location l)
{
    assert(l >= R_LIMBO);
    assert(objs(t).place == R_INHAND || objs(t).place == R_LIMBO);
    assert(objs(t).link == NULL);
    objs(t).place = l;
    if (is_wearable(t)) {
	/* Long's code forgets to do this in several places; for example,
	 * if you DRINK WINE and drop all your items. (This generally
	 * causes the worn object (clover, shoes, etc.) to become
	 * invisible.) So I'm just moving all that code right here. */
        objs(t).flags &= ~F_WORN;
	objs(t).prop = 0;
    }
    if (l > R_LIMBO) {
        objs(t).link = places[l].objects;
        places[l].objects = &objs(t);
    }
}

void insert(ObjectWord t, ObjectWord container)
{
    assert(is_vessel(container));
    move(t, R_LIMBO);
    objs(t).place = -container;
    objs(t).link = objs(container).contents;
    objs(container).contents = &objs(t);
    if (is_wearable(t)) {
        objs(t).flags &= ~F_WORN;
	objs(t).prop = 0;
    }
}

void remove_from_containers(ObjectWord t)
{
    if (!enclosed(t)) return;
    ObjectWord container = -objs(t).place;
    assert(MIN_OBJ <= container && container <= MAX_OBJ);
    assert(is_vessel(container));

    /* Remove t from container's object-list */
    struct ObjectData **p = &objs(container).contents;
    while (*p != &objs(t)) p = &(*p)->link;
    *p = (*p)->link;

    objs(t).place = R_INHAND;
    objs(t).link = NULL;
}

void carry(ObjectWord t)
{
    remove_from_containers(t);
    Location l = objs(t).place;
    if (l != R_INHAND) {
        if (l > R_LIMBO) {
            /* Remove t from l's object-list */
            struct ObjectData **p = &places[l].objects;
            while (*p != &objs(t)) p = &(*p)->link;
            *p = (*p)->link;
        }
        objs(t).place = R_INHAND;
        objs(t).link = NULL;
    }
}

void destroy(ObjectWord t)
{
    /* Long doesn't bother to preserve the invariant that destroyed objects
     * are always in R_LIMBO, but I think it's definitely worth it. */
    move(t, R_LIMBO);
    if (t == BIRD) objs(BIRD).prop = 0;  /* uncaged */
    for (int u = MIN_OBJ; u <= MAX_OBJ; ++u) {
	if (objs(u).place == -(int)t)
	    destroy(u);
    }
}

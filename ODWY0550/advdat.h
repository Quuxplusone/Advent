
#ifndef H_ADVDAT
 #define H_ADVDAT

/* Three enumeration types are used for internal identification. */

typedef enum {
    SAY,SAVE,RESTORE,UPSTREAM,DOWNSTREAM,FORWARD,BACK,OUT,ENTRANCE,GULLY,
    STREAM,ROCK,BED,CRAWL,IN,SURFACE,NOWHERE,PASSAGE,VIEW,HOLE,STAIRS,UP,
    DOWN,OUTDOORS,CRACK,DOME,LEFT,RIGHT,HALL,JUMP,CROSS,EAST,WEST,NORTH,
    SOUTH,NE,SE,SW,NW,SECRET,
    CLIMB,LOOK,GET,DROP,OPEN,CLOSE,ON,OFF,WAVE,
    PLACATE,WALK,KILL,POUR,EAT,DRINK,RUB,THROW,QUIT,FIND,INVENTORY,FEED,
    FILL,SCORE,BRIEF,READ,BREAK,RIDE,HOURS,FUM,SESAME,HELP,TREES,DIG,LOST,
    MIST,INFO,SWIM,LPSD,FAST,FULL,NEWS,CAVE,FIX,WIZARD,
    /* The following magic words MUST be defined in order! */
    ZORTON,XYZZY,THURB,SNOEZE,SAMOHT,PLUGH,PHUGGG,NOSIDE,MELENKURION,KNERL,
    KLAETU,FOO,FOE,FIE,FEE,BLERBI,
    /* End of reverse-alphabetical-order section. */
    /* The following verbs are useful only to wizards. */
    CURIOUS,GREEN,IDEAS,SLEPT,FURIOUSLY,RUNOUT
} VerbWord;

typedef enum {
    NOTHING, GRATE, STEPS, DOOR, SNAKE, FISSURE, TABLET, MIRROR, PLANT,
    PLANT2, STALACTITE, FOG, GLOW, SHADOW, BLOB, DRAWINGS, PIRATE, DRAGON,
    CHASM, TROLL, TROLL2, OGRE, BASILISK, GONG, DJINN, TURTLE, MESSAGE, GORGE,
    STATUE, QUICKSAND, SLIME, PONY, SAFE, THRONE, SKELETON, BEAR,
    BATTERIES, MOSS, DINGHY, BAG, CROWN, GOLD, DIAMONDS, SILVER, JEWELS,
    COINS, CHEST, EGGS, TRIDENT, HELMET, VASE, SHARDS, EMERALD, SCEPTRE,
    YACHT, PYRAMID, PEARL, RUG, SPICES, BEADS, CHAIN, RING, SPYGLASS,
    SCULPTURE, BRACELET, CASKET, KEYS, LAMP, CAGE, BIRD, ROD, PLATE,
    DYNAMITE, PILLOW, CLAM, OYSTER, MAG, KNIFE, FOOD, BOTTLE, WATER, OIL,
    FLASK, AXE, SWORD, TEETH, VIAL, MUSHROOM, GOBLINS, DWARF,
    MAX_OBJ=DWARF
    /* DWARF must be the last object, because look_around() prints everything
     * in the room in order, and we want to make sure the "threatening little
     * dwarf" is always the last object description printed. */
} ObjectWord;

typedef enum {
    R_INHAND = -1, R_LIMBO = 0,
    R_ROAD, R_HILL, R_HOUSE, R_VALLEY, R_FOREST, R_FOREST2, R_SLIT,
    R_DEPRESSION,
    R_INSIDE,
    R_COBBLES, R_DEBRIS, R_AWK, R_BIRD, R_SPIT,
    R_EMIST,
    R_EFISS, R_WFISS, R_NUGGET, R_HMK,
    R_W2PIT, R_E2PIT, R_EPIT, R_WPIT,
    R_NS, R_SOUTH, R_WEST,
    R_Y2, R_JUMBLE, R_WINDOE, R_WINDOW,
    R_DIRTY, R_CLEAN, R_WET, R_DUSTY, R_WMIST,
    R_MAZEA42, R_MAZEA43, R_MAZEA44, R_MAZEA45, R_MAZEA46, R_MAZEA47,
    R_MAZEA48, R_MAZEA49, R_MAZEA50, R_MAZEA51, R_MAZEA52, R_MAZEA53,
    R_MAZEA54, R_MAZEA55, R_MAZEA56, R_MAZEA57_PIT, R_MAZEA58,
    R_ELONG, R_WLONG, R_CROSS, R_DEADEND1,
    R_COMPLEX, R_BEDQUILT, R_SWISS, R_SLAB, R_ABOVER, R_ABOVEP, R_SJUNC,
    R_LOW, R_DEADEND2, R_SECRETEW_TITE, R_NSCANYONWIDE, R_TIGHTERSTILL,
    R_TALLEWCNYN, R_DEADEND3, R_MAZEA80, R_MAZEA81,
    R_MAZEA82, R_MAZEA83, R_MAZEA84, R_MAZEA85, R_MAZEA86, R_MAZEA87,
    R_NARROW, R_INCLINE, R_GIANT, R_IMMENSENSPASS, R_FALLS,
    R_SOFT, R_ORIENTAL, R_MISTY, R_ALCOVE, R_PLOVER, R_DARK,
    R_ARCHED, R_SHELL, R_RAGGED, R_CULDESAC, R_ANTE,
    R_MAZED107, R_WITT, R_MIRROR, R_STALACT, R_MAZED112,
    R_RES, R_RESERVOIR_N, R_WARM, R_BALCONY, R_FAKE_SLIT,
    R_CYLINDRICAL, R_PIRATES_NEST, R_SWOFCHASM, R_NEOFCHASM,
    R_SLOPING, R_SECRETCYNNE1, R_SECRETCYNNE2, R_CORRIDOR,
    R_FORK, R_WARMJUNCTN, R_VIEW, R_FACES, R_BY_FIGURE, R_PLAIN_1,
    R_PLAIN_2, R_PLAIN_3, R_NONDESCRIPT, R_PENTAGRAM, R_CHIMNEY,
    R_TUBE, R_TUBE_SLIDE, R_BASQUE_1, R_BASQUE_2, R_BASQUE_FORK,
    R_ON_STEPS, R_STEPS_EXIT, R_STORAGE, R_FAKE_Y2, R_FAKE_JUMBLE,
    R_CATACOMBS1, R_CATACOMBS2, R_CATACOMBS3, R_CATACOMBS4, R_CATACOMBS5,
    R_CATACOMBS6, R_CATACOMBS7, R_CATACOMBS8, R_CATACOMBS9, R_CATACOMBS10,
    R_CATACOMBS11, R_CATACOMBS12, R_CATACOMBS13, R_CATACOMBS14,
    R_CATACOMBS15, R_CATACOMBS16, R_CATACOMBS17, R_CATACOMBS18,
    R_CATACOMBS19, R_AUDIENCE, R_AUDIENCE_E, R_BANSHEE, R_GOLDEN,
    R_ARABESQUE, R_TRANSLUCENT,
    R_CHAMBER, R_LIME, R_FBARR, R_BARR,
    R_MAZED131, R_MAZED132, R_MAZED133, R_MAZED134,
    R_MAZED135, R_MAZED136, R_MAZED137, R_MAZED138,
    R_MAZED139, R_PONY,
    R_SANDSTONE, R_MORION, R_VAULT, R_PEELGRUNT, R_INSAFE, R_CORRID_1,
    R_CORRID_2, R_TOOL, R_CORRID_3, R_CUBICLE, R_SPHERICAL, R_TUNNEL_1,
    R_GLASSY, R_LAIR, R_BRINK_1, R_BRINK_2, R_ICE, R_SLIDE,
    R_ICECAVE1, R_ICECAVE1A, R_ICECAVE2, R_ICECAVE2A, R_ICECAVE3, R_ICECAVE3A,
    R_ICECAVE4, R_ICECAVE5, R_ICECAVE6, R_ICECAVE7, R_ICECAVE8, R_ICECAVE9,
    R_ICECAVE10, R_ICECAVE11, R_ICECAVE12, R_ICECAVE12A, R_ICECAVE13,
    R_ICECAVE14, R_ICECAVE15, R_ICECAVE15A, R_ICECAVE16, R_ICECAVE17,
    R_ICECAVE18, R_ICECAVE19, R_ICECAVE20, R_ICECAVE21, R_ICECAVE22,
    R_ICECAVE23, R_ICECAVE24, R_ICECAVE25, R_ICECAVE26, R_ICECAVE27,
    R_ICECAVE28, R_ICECAVE28A, R_ICECAVE29, R_ICECAVE30,
    R_BRINK_3, R_CRACK_1, R_CRACK_2, R_CRACK_3, R_CRACK_4, R_ARCH_COR_1,
    R_ARCH_COR_2, R_ARCH_FORK, R_JONAH, R_IN_JONAH, R_FOURIER, R_SHELF,
    R_BEACH, R_PLATFORM,
    R_YLEM
} Location;

/* The player's words are mapped onto these three enums via WordClass. */

typedef enum {
    WordClass_None,  /* .meaning is zero: no word entered */
    WordClass_Verb,  /* .meaning is a VerbWord */
    WordClass_Object,  /* .meaning is an ObjectWord */
    WordClass_Place,  /* .meaning is a Location */
    WordClass_BadWord  /* .meaning is zero: word wasn't recognized */
} WordClass;

struct InputWordInfo {
    char text[16];
    WordClass type;
    int meaning;
};
extern struct InputWordInfo word1, word2;


enum flagsBits {
    /* Bits that can be set in places[].flags */
    F_LIGHTED    = 0x001,
    F_BEENHERE   = 0x002,
    F_NODWARF    = 0x004,
    F_NOBACK     = 0x008,
    F_NOTINCAVE  = 0x010,
    F_HINTABLE   = 0x020,
    F_WATER      = 0x040,
    F_INMAZE     = 0x080,
    F_ONE_EXIT   = 0x100,
    F_DAMP       = 0x400,
  
    /* Bits that can be set in objs[].flags */
    F_PORTABLE   = 0x001,
    F_SEEN       = 0x002,
    F_SCHIZOID   = 0x004,
    F_UNSTABLE   = 0x008,  /* the object has an "initial" shortdesc different
                            * from its normal one, so ++prop when it's taken */
    F_INVISIBLE  = 0x010
    /* Platt had a lot more flags, but they encoded static data that it's
     * easy to turn into regular code. For example, the food and mushroom had
     * F_EDIBLE to indicate that they were acceptable targets for the command
     * "eat" with no object. */
};

struct Place {
    const char *short_desc;
    const char *long_desc;
    unsigned int flags;
    int (*f)(void);
};

struct ObjectData {
    const char *desc[4];  /* .prop ranges from 0 to 3 */
    int prop;
    const char *name;
    unsigned int flags;
    Location place;
    Location place2;  /* only used for F_SCHIZOID objects */
};

extern struct Place places[];
extern struct ObjectData objs[];

#define toting(t) (objs[t].place < 0)
#define there(t, loc) (objs[t].place == (loc) || objs[t].place2 == (loc))

#define you_are_dead_at(loc) ((loc)+R_YLEM)
#define STAY_STILL (-1)

#define SOFT_NL "\n"
#define SOFT_HYPHEN "-\n"
#define HARD_HYPHEN "-\n"
#define EMDASH(x) x

#endif /* H_ADVDAT */

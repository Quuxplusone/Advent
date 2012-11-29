
/*
 * This file is #included three times by "adv551.c", preprocessed differently
 * each time. This is a huge kludge to make the room description strings
 * optimizable on the Z-machine by making sure they never appear outside
 * of a call to puts().
 */

#define all_alike "You are in a maze of twisty little passages, all alike."
#define dead_end "Dead end."

    make_loc(q, R_ROAD,
             "You are standing at the end of a road before a small brick building." SOFT_NL
             "Around you is a forest.  A small stream flows out of the building and" SOFT_NL
             "down a gully.",
             "You're at end of road again.", F_LIGHTED | F_OUTSIDE);
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
             "You're inside building.", F_LIGHTED | F_OUTSIDE);
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
             "You're in valley.", F_LIGHTED | F_OUTSIDE);
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
             "You're at slit in streambed.", F_LIGHTED | F_OUTSIDE);
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
             "A low crawl over cobbles leads inward to the west.",
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
    make_cond_ins(U, only_if_toting(GOLD), remark(15)); ditto(PIT);
        ditto(STEPS); ditto(DOME); ditto(PASSAGE); ditto(E);
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
             "crawl slants up.  To the north a round two-foot hole slants down.",
             "You're at east end of Long Hall.", 0);
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL);
    make_ins(W, R_WLONG);
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE);
    make_cond_ins(S, 50, R_DEAD191);
    make_cond_ins(S, unless_prop(CLOAKROOM_ROCKS, 0), R_TIGHT_NS_BLOCKED);
    make_ins(S, R_TIGHT_NS);
    make_ins(CLIMB, remark(8));
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
    /* Long makes these stairs unclimbable with the nugget, too;
     * not just the steps up the dome. */
    make_cond_ins(U, only_if_toting(GOLD), remark(39)); ditto(PIT);
        ditto(STEPS); ditto(DOME); ditto(E); ditto(PASSAGE); ditto(CLIMB);
        ditto(STAIRS);
    make_ins(U, R_EMIST);
    /* There's no royal road to the Mountain King's chamber; just luck. */
    make_cond_ins(NE, 75, remark(6));
    make_cond_ins(NE, unless_prop(SNAKE, 0), R_PRIVATE);
    /* I suppose our adventurer must be walking on the ceiling! */
    make_cond_ins(N, unless_prop(SNAKE, 0), R_NS); ditto(LEFT);
    make_cond_ins(S, unless_prop(SNAKE, 0), R_SOUTH); ditto(RIGHT);
    make_cond_ins(W, unless_prop(SNAKE, 0), R_WEST); ditto(FORWARD);
    make_ins(N, remark(16));
    make_cond_ins(SW, 15, R_SECRET);
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
             "You're in pit by stream.", 0);
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
    make_ins(U, R_ARCHED);
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
             "Proceed at own risk.  [Witt Construction Company]\".",
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
             "You're in cavern with waterfall.", 0);
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
    make_cond_ins(U, 75, remark(25));
    make_ins(U, remark(26));
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
    make_ins(PLOVER, R_Y2);
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
             "You're at Reservoir.", 0);
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
             "You are in a secret canyon that exits to the north and east.",
             NULL, 0);
    make_ins(N, R_ABOVER);
    make_ins(E, R_SECRET);
    make_loc(q, R_SCAN3,
             "You are in a secret canyon that exits to the north and east.",
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
    /* Long lets you cross the chasm even after the bridge has been wrecked, thanks
     * to a buggy shuffling of the object numbers. (In Woods' version, CHASM was
     * object #32; in Long's version, it's object #21, but the condition here still
     * keyed off of the old numeric value.)
     * Once that's fixed, Long still forgets that it's possible to reach R_NESIDE
     * via Lost River Canyon after the bridge is wrecked, so we need to handle that
     * case in the travel table as well. I've fixed that bug. */
    make_ins(SW, R_SLOPING);
    make_cond_ins(OVER, only_if_here(TROLL), remark(11));
        ditto(ACROSS); ditto(CROSS); ditto(NE); ditto(BRIDGE);  /* Long omits BRIDGE */
    make_cond_ins(OVER, unless_prop(CHASM, 0), remark(12));
    make_ins(OVER, R_TROLL);
    make_cond_ins(JUMP, unless_prop(CHASM, 0), R_LOSE);
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
    make_cond_ins(OVER, only_if_here(TROLL), remark(11));
        ditto(ACROSS); ditto(CROSS); ditto(SW); ditto(BRIDGE);
    make_cond_ins(OVER, unless_prop(CHASM, 0), remark(12));
    make_ins(OVER, R_TROLL);
    make_cond_ins(JUMP, unless_prop(CHASM, 0), R_LOSE);
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
             /* Long has "smokey". I've changed it back to Woods' "smoky". */
             "program.  Massive torches far overhead bathe the room with smoky" SOFT_NL
             "yellow light.  Scattered about you can be seen a pile of bottles (all" SOFT_NL
             "of them empty), a nursery of young beanstalks murmuring quietly, a bed" SOFT_NL
             "of oysters, a bundle of black rods with rusty stars on their ends, and" SOFT_NL
             "a collection of brass lanterns.  Off to one side a great many dwarves" SOFT_NL
             "are sleeping on the floor, snoring loudly.  A sign nearby reads:  \"DO" SOFT_NL
             "NOT DISTURB THE DWARVES!\"  An immense mirror is hanging against one" SOFT_NL
             "wall, and stretches to the other end of the room, where various other" SOFT_NL
             "sundry objects can be glimpsed dimly in the distance.  An unoccupied" SOFT_NL
             "telephone booth stands against the north wall.",
             "You're at NE end.", F_LIGHTED);
    make_ins(SW, R_SWEND); ditto(ACROSS); ditto(CROSS);
    make_ins(IN, R_REPO_BOOTH); ditto(ENTER); ditto(N);
    make_loc(q, R_SWEND,
             "You are at the southwest end of the repository.  To one side is a pit" SOFT_NL
             "full of fierce green snakes.  On the other side is a row of small" SOFT_NL
             "wicker cages, each of which contains a little sulking bird.  In one" SOFT_NL
             "corner is a bundle of black rods with rusty marks on their ends.  A" SOFT_NL
             "large number of velvet pillows are scattered about on the floor." SOFT_NL
             "Beside one of the pillows is a large, dusty, leather-bound volume" SOFT_NL
             "with the title \"History of Adventure\" embossed in pure gold." SOFT_NL
             "A vast mirror stretches off to the northeast, almost reaching the" SOFT_NL
             "phone booth.  At your feet is a large steel grate, next to which is" SOFT_NL
             "a sign which reads, \"Treasure Vault.  Keys in Main Office.\"",
             "You're at SW end.", F_LIGHTED);
    /* Long accidentally has SW here instead of CROSS. */
    make_ins(NE, R_NEEND); ditto(ACROSS); ditto(CROSS);
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
    make_ins(S, R_DENSE);
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
             /* Long has "ends cliffs" */
             "the north, the beach ends in cliffs and broken rocks.",
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
             "growing larger....  Dear me!  Is the cave bigger or are you smaller?\n",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_GROWING,
             "You are again overcome by a sickening vertigo, but this time" SOFT_NL
             "everything around you is shrinking...Shrinking...\n",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_GROWING2,
             "You are again overcome by a sickening vertigo, but this time" SOFT_NL
             "everything is shrinking... I mean, you are growing.  This is" SOFT_NL
             "terribly confusing!\n",
             NULL, 0);
    make_ins(0, R_PHUCE);
    make_loc(q, R_SHRINKING2,
             "You feel dizzy...Everything around you is spinning, expanding," SOFT_NL
             "growing larger....\n",
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
    make_ins(E, remark(20));
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
    make_ins(SE, remark(36));
    make_cond_ins(N, only_if_toting(BOAT), R_BUBBLE);
    make_cond_ins(S, only_if_toting(BOAT), R_GRAVEL);
    make_cond_ins(W, only_if_toting(BOAT), R_WBLUE); ditto(CROSS); ditto(OVER); ditto(ACROSS);
    make_ins(W, remark(20));
    make_loc(q, R_BUBBLE,
             "You are at a high rock on the NE side of a watery chamber at the mouth" SOFT_NL
             "of a small brook.  An unknown gas bubbles up through the water from" SOFT_NL
             "the chamber floor.  A bluish light can be seen to the southwest.",
             "You're in Bubble Chamber.", F_LIGHTED);
    make_cond_ins(S, only_if_toting(BOAT), R_EBLUE);
    make_cond_ins(SW, only_if_toting(BOAT), R_WBLUE);
    make_ins(SW, remark(20));
    make_cond_ins(PASSAGE, unless_prop(BOAT, 1), R_DEFILE); ditto(E); ditto(U);
    make_ins(PASSAGE, remark(36));
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
    make_ins(N, remark(20));
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
    /* Notice that none of Lost River Canyon has F_LIGHTED. */
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
             "You're in old spillway.", 0);
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
    make_cond_ins(E, unless_prop(DOG, 0), R_WSTYX); ditto(IN); ditto(D);
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
    /* Moving from R_SPIRES to R_PINNACLES is always dangerous, and the player
     * cannot visit the Winery twice without braving the pinnacles. I believe Long
     * intended the player to descend to this location first via the slippers,
     * pick up the cask, and take it the long way around to the Winery via (his
     * one trip through) the seaside entrance.
     * The alternative "expert" solution of filling the bottle with wine and then
     * filling the cask from the bottle works in this port, but did not work in
     * Long's original, because of a bug in the FILL logic. See attempt_fill(). */
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
    make_loc(q, R_DUCK,
             "You have crawled through a very low wide passage parallel to and north" SOFT_NL
             "of the Hall of Mists.",
             NULL, 0);
    make_ins(0, R_WFISS);
    make_loc(q, R_UPNOUT,
             "There is nothing here to climb.  Use \"up\" or \"out\" to leave the pit.",
             NULL, 0);
    make_ins(0, R_WPIT);
    make_loc(q, R_DIDIT, "You have climbed up the plant and out of the pit.", NULL, 0);
    make_ins(0, R_W2PIT);

#undef dead_end
#undef all_alike

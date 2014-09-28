
/*
 * This file is #included three times by "adv440.c", preprocessed differently
 * each time. This is a huge kludge to make the room description strings
 * optimizable on the Z-machine by making sure they never appear outside
 * of a call to puts().
 */

#define all_alike "You are in a maze of twisty little passages, all alike."
#define dead_end "Dead end."
#define tangled "You are in a tangled web of intersecting passages."

    make_loc(q, R_ROAD,
             "You are standing at the end of a road before a small brick building." SOFT_NL
             "Around you is a forest.  A small stream flows out of the building and" SOFT_NL
             "down a gully.",
             "You're at the end of the road again.", 0);
    make_ins(W, R_HILL); ditto(U); ditto(ROAD);
    make_ins(E, R_HOUSE); ditto(IN); ditto(HOUSE); ditto(ENTER);
    make_ins(S, R_VALLEY); ditto(D); ditto(GULLY); ditto(STREAM); ditto(DOWNSTREAM);
    make_ins(N, R_FOREST); ditto(FOREST);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_HILL,
             "You have walked up a hill, still in the forest.  The road slopes back" SOFT_NL
             "down the other side of the hill.  There is a building in the distance." SOFT_NL
             "To the north lies a snow-capped mountain range with peaks that rise" SOFT_NL
             "into the sky.  To the south, partly obscured by a thin haze, lies a" SOFT_NL
             "white fortress with seven towers.  Beyond the fortress, shimmering in" SOFT_NL
             "the sun, there sparkles a great expanse of water.",
             "You're at the hill in road.", 0);
    make_ins(ROAD, R_ROAD); ditto(HOUSE); ditto(FORWARD); ditto(E); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(N); ditto(S);
    make_loc(q, R_HOUSE,
             "You are inside a building, a well house for a large spring.",
             "You're inside the building.", 0);
    make_ins(ENTER, R_ROAD); ditto(OUT); ditto(OUTDOORS); ditto(W);
    make_ins(XYZZY, R_DEBRIS);
    make_ins(PLUGH, R_Y2);
    make_ins(DOWNSTREAM, remark(17)); ditto(STREAM);
    make_loc(q, R_VALLEY,
             "You are in a valley in the forest beside a stream tumbling along a" SOFT_NL
             "rocky bed.",
             "You're in the valley.", 0);
    make_ins(UPSTREAM, R_ROAD); ditto(HOUSE); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(U);
    make_ins(DOWNSTREAM, R_SLIT); ditto(S); ditto(D);
    make_ins(DEPRESSION, R_OUTSIDE);
    make_loc(q, R_FOREST,
             "You are in open forest, with a deep valley to one side.",
             "You're in the forest.", 0);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(D);
    make_cond_ins(FOREST, 50, R_FOREST); ditto(FORWARD); ditto(N);
    make_ins(FOREST, R_FOREST2);
    make_ins(W, R_FOREST); ditto(S);
    make_loc(q, R_FOREST2,
             "You are in open forest near both a valley and a road.",
             "You're in the forest.", 0);
    make_ins(ROAD, R_ROAD); ditto(N);
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(W); ditto(D);
    make_ins(FOREST, R_FOREST); ditto(S);
    make_loc(q, R_SLIT,
             "At your feet all the water of the stream splashes into a 2-inch slit" SOFT_NL
             "in the rock.  Downstream the streambed is bare rock.",
             "You're at the slit in the streambed.", 0);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_VALLEY); ditto(N);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W);
    make_ins(DOWNSTREAM, R_OUTSIDE); ditto(ROCK); ditto(BED); ditto(S);
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D);
    make_loc(q, R_OUTSIDE,
             "You are in a 20-foot depression floored with bare dirt.  Set into the" SOFT_NL
             "dirt is a strong steel grate mounted in concrete.  A dry streambed" SOFT_NL
             "leads into the depression.",
             "You're outside the grate.", F_CAVE_HINT);
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(S);
    make_ins(HOUSE, R_ROAD);
    make_ins(UPSTREAM, R_SLIT); ditto(GULLY); ditto(N);
    make_cond_ins(ENTER, unless_prop(GRATE, 0), R_INSIDE); ditto(IN); ditto(D);
    make_ins(ENTER, remark(1));
    make_loc(q, R_INSIDE,
             "You are in a small chamber beneath a 3x3 steel grate to the surface." SOFT_NL
             "A low crawl over cobbles leads inward to the west.",
             "You're below the grate.", 0);
    make_cond_ins(OUT, unless_prop(GRATE, 0), R_OUTSIDE); ditto(U);
    make_ins(OUT, remark(1));
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(IN); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_COBBLES,
             "You are crawling over cobbles in a low passage.  There is a dim light" SOFT_NL
             "at the east end of the passage.",
             "You're in the cobble crawl.", 0);
    make_ins(OUT, R_INSIDE); ditto(SURFACE); ditto(E);
    make_ins(IN, R_DEBRIS); ditto(DARK); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_DEBRIS,
             "You are in a debris room filled with stuff washed in from the surface." SOFT_NL
             "A low wide passage with cobbles becomes plugged with mud and debris" SOFT_NL
             "here, but an awkward canyon leads upward and west.  A note on the wall" SOFT_NL
             "says \"MAGIC WORD XYZZY\".",
             "You're in the debris room.", 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(PASSAGE); ditto(LOW); ditto(E);
    make_ins(CANYON, R_AWK); ditto(IN); ditto(U); ditto(W);
    make_ins(XYZZY, R_HOUSE);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_AWK,
             "You are in an awkward sloping east/west canyon.",
             NULL, 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(D, R_DEBRIS); ditto(E);
    make_ins(IN, R_BIRD); ditto(U); ditto(W);
    make_ins(PIT, R_SPIT);
    make_loc(q, R_BIRD,
             "You are in a splendid chamber thirty feet high. The walls are frozen" SOFT_NL
             "rivers of orange stone. An awkward canyon and a good passage exit" SOFT_NL
             "from east and west sides of the chamber.",
             "You're in bird chamber.", F_BIRD_HINT);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
    make_ins(CANYON, R_AWK); ditto(E);
    make_ins(PASSAGE, R_SPIT); ditto(PIT); ditto(W);
    make_loc(q, R_SPIT,
             "At your feet is a small pit breathing traces of white mist. An east" SOFT_NL
             "passage ends here except for a small crack leading on.",
             "You're at the top of the small pit.", 0);
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE);
    make_ins(ENTRANCE, R_INSIDE);
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
             "You're in the hall of mists.", 0);
    make_ins(LEFT, R_NUGGET); ditto(S);
    make_ins(FORWARD, R_EFISS); ditto(HALL); ditto(W);
    make_ins(STAIRS, R_HMK); ditto(D); ditto(N);
    make_cond_ins(U, only_if_toting(GOLD), remark(15)); ditto(PIT); ditto(STEPS); ditto(DOME); ditto(PASSAGE); ditto(E);
    make_ins(U, R_SPIT);
    make_ins(Y2, R_JUMBLE);
    make_loc(q, R_NUGGET,
             "This is a low room with a crude note on the wall. The note says," SOFT_NL
             "\"You won't get it up the steps\".",
             "You're in the nugget of gold room.", 0);
    make_ins(HALL, R_EMIST); ditto(OUT); ditto(N);
    make_loc(q, R_EFISS,
             "You are on the east bank of a fissure slicing clear across the hall." SOFT_NL
             "The mist is quite thick here, and the fissure is too wide to jump.",
             "You're on the east bank of the fissure.", 0);
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
             "You are at the west end of the hall of mists. A low wide crawl" SOFT_NL
             "continues west and another goes north. To the south is a little" SOFT_NL
             "passage 6 feet off the floor.",
             "You're at the west end of the hall of mists.", 0);
    make_ins(S, R_LIKE1); ditto(U); ditto(PASSAGE); ditto(CLIMB);
    make_ins(E, R_WFISS);
    make_ins(W, R_ELONG); ditto(CRAWL);
    make_ins(N, R_DUCK);

    make_loc(q, R_LIKE1, all_alike, NULL, F_TWIST_HINT);
    make_ins(U, R_WMIST);
    make_ins(N, R_LIKE1);
    make_ins(E, R_LIKE2);
    make_ins(S, R_LIKE4);
    make_ins(W, R_LIKE11);
    make_loc(q, R_LIKE2, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE1);
    make_ins(S, R_LIKE3);
    make_ins(E, R_LIKE4);
    make_loc(q, R_LIKE3, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_LIKE2);
    make_ins(D, R_DEAD5);
    make_ins(S, R_LIKE6);
    make_ins(N, R_DEAD9);
    make_loc(q, R_LIKE4, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE1);
    make_ins(N, R_LIKE2);
    make_ins(E, R_DEAD3);
    make_ins(S, R_DEAD4);
    make_ins(U, R_LIKE14); ditto(D);
    make_loc(q, R_LIKE5, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_LIKE6);
    make_ins(W, R_LIKE7);
    make_loc(q, R_LIKE6, all_alike, NULL, F_TWIST_HINT);
    make_ins(E, R_LIKE3);
    make_ins(W, R_LIKE5);
    make_ins(D, R_LIKE7);
    make_ins(S, R_LIKE8);
    make_loc(q, R_LIKE7, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE5);
    make_ins(U, R_LIKE6);
    make_ins(E, R_LIKE8);
    make_ins(S, R_LIKE9);
    make_loc(q, R_LIKE8, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE6);
    make_ins(E, R_LIKE7);
    make_ins(S, R_LIKE8);
    make_ins(U, R_LIKE9);
    make_ins(N, R_LIKE10);
    make_ins(D, R_DEAD11);
    make_loc(q, R_LIKE9, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE7);
    make_ins(N, R_LIKE8);
    make_ins(S, R_DEAD6);
    make_loc(q, R_LIKE10, all_alike, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE8);
    make_ins(N, R_LIKE10);
    make_ins(D, R_DEAD7);
    make_ins(E, R_BRINK);
    make_loc(q, R_LIKE11, all_alike, NULL, F_TWIST_HINT);
    make_ins(N, R_LIKE1);
    make_ins(W, R_LIKE11); ditto(S);
    make_ins(E, R_DEAD1);
    make_loc(q, R_LIKE12, all_alike, NULL, F_TWIST_HINT);
    make_ins(S, R_BRINK);
    make_ins(E, R_LIKE13);
    make_ins(W, R_DEAD10);
    make_loc(q, R_LIKE13 , all_alike, NULL, F_TWIST_HINT);
    make_ins(N, R_BRINK);
    make_ins(W, R_LIKE12);
    make_ins(NW, R_PIRATES_NEST);  /* NW: a dirty trick! */
    make_loc(q, R_LIKE14, all_alike, NULL, F_TWIST_HINT);
    make_ins(U, R_LIKE4); ditto(D);
    make_loc(q, R_BRINK,
             "You are on the brink of a thirty-foot pit with a massive orange column" SOFT_NL
             "down one wall.  You could climb down here but you could not get back" SOFT_NL
             "up.  The maze continues at this level.",
             "You're at the brink of the pit.", 0);
    make_ins(D, R_BIRD); ditto(CLIMB);
    make_ins(W, R_LIKE10);
    make_ins(S, R_DEAD8);
    make_ins(N, R_LIKE12);
    make_ins(E, R_LIKE13);
    make_loc(q, R_ELONG,
             "You are at the east end of a very long hall apparently without side" SOFT_NL
             "chambers.  To the east a low wide crawl slants up.  To the north a" SOFT_NL
             "round two-foot hole slants down.",
             "You're at the east end of the long hall.", 0);
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL);
    make_ins(W, R_WLONG);
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE);
    make_loc(q, R_WLONG,
             "You are at the west end of a very long featureless hall.  The hall" SOFT_NL
             "joins up with a narrow north/south passage.",
             "You're at the west end of the long hall.", 0);
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
    twist(R_DIFF4,R_DIFF1,R_DIFF7,R_DIFF5,R_149,R_DIFF0,R_DIFF3,R_DIFF2,R_DIFF10,R_DIFF8,R_DIFF6,
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
        "You're in the Hall of the Mt King.", F_SNAKE_HINT);
    make_ins(STAIRS, R_EMIST); ditto(U); ditto(E);
    /* I suppose our adventurer must be walking on the ceiling! */
    make_cond_ins(N, unless_prop(SNAKE, 0), R_NS); ditto(LEFT);
    make_cond_ins(S, unless_prop(SNAKE, 0), R_SOUTH); ditto(RIGHT);
    make_cond_ins(W, unless_prop(SNAKE, 0), R_WEST); ditto(FORWARD);
    make_ins(N, remark(16));
    make_cond_ins(SW, 35, R_SECRET);
    make_cond_ins(SW, only_if_here(SNAKE), remark(16));
    make_ins(SECRET, R_SECRET);
    make_loc(q, R_WEST,
        "You are in the west side chamber of the Hall of the Mountain King." SOFT_NL
        "A passage continues west and up here.",
        "You're in west side chamber.", 0);
    make_ins(HALL, R_HMK); ditto(OUT); ditto(E);
    make_ins(W, R_CROSS); ditto(U);
    make_loc(q, R_SOUTH,
        "You are in the south side chamber.  A spiral staircase descends into" SOFT_NL
        "a shaft which has been bored into the rock.  The staircase is very" SOFT_NL
        "old and ridden through with rust.  A breeze blows towards the shaft.",
        "You're in the south side chamber.", 0);
    make_ins(HALL, R_HMK); ditto(N);
    make_ins(D, R_197);
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
             "You're at the window on the pit.", 0);
    make_ins(E, R_Y2); ditto(Y2);
    make_ins(JUMP, R_NECK);
    make_ins(REFLECT, R_WINDOW);
    make_loc(q, R_DIRTY,
             "You are in a dirty broken passage.  To the east is a crawl.  To the" SOFT_NL
             "west is a large passage.  Above you is a hole to another passage.",
             "You're in the dirty passage.", 0);
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
             "You're in the dusty rock room.", 0);
    make_ins(E, R_DIRTY); ditto(PASSAGE);
    make_ins(D, R_COMPLEX); ditto(HOLE); ditto(FLOOR);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_loc(q, R_COMPLEX,
             "You are at a complex junction.  A low hands-and-knees passage from the" SOFT_NL
             "north joins a higher crawl from the east to make a walking passage" SOFT_NL
             "going west.  There is also a large room above.  The air is damp here.",
             "You're at the complex junction.", 0);
    make_ins(U, R_DUSTY); ditto(CLIMB); ditto(ROOM);
    make_ins(W, R_BEDQUILT); ditto(BEDQUILT);
    make_ins(N, R_SHELL); ditto(SHELL);
    make_ins(E, R_ANTE);
    make_loc(q, R_SHELL,
             "You're in a large room carved out of sedimentary rock.  The floor and" SOFT_NL
             "walls are littered with bits of shells embedded in the stone.  A" SOFT_NL
             "shallow passage proceeds downward, and a somewhat steeper one leads" SOFT_NL
             "up.  A low hands-and-knees passage enters from the south.",
             "You're in the shell room.", 0);
    make_ins(U, R_ARCHED); ditto(HALL);
    make_ins(D, R_RAGGED);
    make_cond_ins(S, only_if_toting(CLAM), remark(4));
    make_cond_ins(S, only_if_toting(OYSTER), remark(5));
    make_ins(S, R_COMPLEX);
    make_loc(q, R_ARCHED,
             "You are in an arched hall.  A coral passage once continued up and east" SOFT_NL
             "from here, but is now blocked by debris.  The air smells of sea water.",
             "You're in the arched hall.", 0);
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
             "passages go west, south and north-east.  The remnants of recent" SOFT_NL
             "digging are evident.  A sign in midair says: \"WARNING " EMDASH("-") SOFT_NL
             "DANGEROUS AREA!\" and a smaller one beneath reads: \"Cave under" SOFT_NL
             "construction beyond this point.  Proceed at your own risk!  (Witt" SOFT_NL
             "Construction Company).\"",
             "You're in the anteroom.", 0);
    make_ins(NE, R_174);
    make_ins(W, R_COMPLEX);
    make_ins(E, R_WITT);
    make_ins(S, R_141);
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
    make_cond_ins(S, 80, remark(6));
    make_ins(S, R_190);
    make_ins(SLAB, R_SLAB);
    make_cond_ins(U, 80, remark(6));
    make_cond_ins(U, 50, R_ABOVEP);
    make_ins(U, R_DUSTY);
    make_cond_ins(N, 60, remark(6));
    make_cond_ins(N, 75, R_LOW);
    make_ins(N, R_SJUNC);
    make_cond_ins(D, 80, remark(6));
    make_ins(D, R_ANTE);

    make_loc(q, R_SWISS,
             "You are in a room whose walls resemble Swiss cheese.  Obvious passages" SOFT_NL
             "go west, east, NE, and NW.  Part of the room is occupied by a large" SOFT_NL
             "bedrock block.",
             "You're in the Swiss cheese room.", 0);
    make_ins(NE, R_BEDQUILT);
    make_ins(W, R_E2PIT);
    make_cond_ins(S, 80, remark(6));
    make_ins(CANYON, R_TALL);
    make_ins(E, R_SOFT);
    make_cond_ins(NW, 50, remark(6));
    make_ins(ORIENTAL, R_ORIENTAL);
    make_loc(q, R_SOFT,
             "You are in the soft room.  The walls are covered with heavy curtains," SOFT_NL
             "the floor with a thick pile carpet and moss covers the ceiling.",
             "You're in the soft room.", 0);
    make_ins(W, R_SWISS);
    make_ins(E, R_192);
    make_ins(SE, R_153);
    make_ins(N, R_196); ditto(S); ditto(NE); ditto(SE); ditto(NW); ditto(SW);  // TODO: SE appears twice
    make_loc(q, R_E2PIT,
             "You are at the east end of the twopit room.  The floor here is" SOFT_NL
             "littered with thin rock slabs, which make it easy to descend the pits." SOFT_NL
             "There is a path here bypassing the pits to connect passages from east" SOFT_NL
             "and west.  There are holes all over, but the only big one is on the" SOFT_NL
             "wall directly over the west pit where you can't get to it.",
             "You're at the east end of the twopit room.", 0);
    make_ins(E, R_SWISS);
    make_ins(W, R_W2PIT); ditto(ACROSS);
    make_ins(D, R_EPIT); ditto(PIT);
    make_loc(q, R_W2PIT,
             "You are at the west end of the twopit room.  There is a large hole in" SOFT_NL
             "the wall above the pit at this end of the room.",
             "You're at west end of the twopit room.", 0);
    make_ins(E, R_E2PIT); ditto(ACROSS);
    make_ins(W, R_SLAB); ditto(SLAB);
    make_ins(D, R_WPIT); ditto(PIT);
    make_ins(HOLE, remark(8));
    make_loc(q, R_EPIT,
             "You are at the bottom of the eastern pit in the Twopit Room.  There is" SOFT_NL
             "a small pool of oil in one corner of the pit.",
             "You're in the east pit.", 0);
    make_ins(U, R_E2PIT); ditto(OUT);
    make_loc(q, R_WPIT,
             "You are at the bottom of the western pit in the Twopit Room.  There is" SOFT_NL
             "a large hole in the wall about 25 feet above you.",
             "You're in the west pit.", 0);
    make_ins(U, R_W2PIT); ditto(OUT);
    make_cond_ins(CLIMB, unless_prop(PLANT, 2), R_CHECK);
    make_ins(CLIMB, R_CLIMB);
    make_loc(q, R_NARROW,
             "You are in a long, narrow corridor stretching out of sight to the" SOFT_NL
             "west.  At the eastern end is a hole through which you can see a" SOFT_NL
             "profusion of leaves.",
             "You're in the narrow corridor.", 0);
    make_ins(D, R_WPIT); ditto(CLIMB); ditto(E);
    make_ins(JUMP, R_NECK);
    make_ins(W, R_GIANT);
    make_loc(q, R_GIANT,
             "You are in the giant room.  The ceiling here is too high up for your" SOFT_NL
             "lamp to show it.  Cavernous passages lead east, north, and south.  On" SOFT_NL
             "the west wall is scrawled the inscription, \"FEE FIE FOE FOO\" [sic].",
             "You're in the giant room.", 0);
    make_ins(S, R_NARROW);
    make_ins(E, R_BLOCK);
    make_ins(N, R_IMMENSE);
    make_loc(q, R_BLOCK,
             "The passage here is blocked by a recent cave-in.",
             NULL, 0);
    make_ins(S, R_GIANT); ditto(OUT);
    make_loc(q, R_IMMENSE,
             "You are at one end of an immense north/south passage.",
             NULL, 0);
    make_ins(S, R_GIANT); ditto(PASSAGE);
    make_cond_ins(N, unless_prop(RUSTY_DOOR, 0), R_FALLS); ditto(ENTER); ditto(CAVERN);
    make_ins(N, remark(9));
    make_loc(q, R_FALLS,
             "You are in a magnificent cavern with a rushing stream, which cascades" SOFT_NL
             "over a sparkling waterfall into a roaring whirlpool that disappears" SOFT_NL
             "through a hole in the floor.  Passages exit to the south and west.",
             "You're in the cavern with the waterfall.", 0);
    make_ins(S, R_IMMENSE); ditto(OUT);
    make_ins(W, R_INCLINE);
    make_loc(q, R_INCLINE,
             "You are at the top of a steep incline above a large room.  You could" SOFT_NL
             "climb down here, but you would not be able to climb up.  There is a" SOFT_NL
             "passage leading back to the north.",
             "You're at the steep incline above a large room.", 0);
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
             "You're at the junction of three secret canyons.", 0);
    /* In Crowther's original, this was pretty much the edge of the cave. Going UP here
     * would take you on a one-way trip back to the dusty rock room. Woods replaced
     * that connection with a northerly passage to R_WINDOW. */
    make_ins(SE, R_BEDQUILT);
    make_ins(S, R_ABOVEP);
    make_ins(N, R_WINDOW);
    make_loc(q, R_TITE,
             "A large stalactite extends from the roof and almost reaches the floor" SOFT_NL
             "below.  You could climb down it, and jump from it to the floor, but" SOFT_NL
             "having done so you would be unable to reach it to climb back up.",
             "You're at the top of the stalactite.", 0);
    make_ins(N, R_ABOVEP);
    make_cond_ins(D, 40, R_LIKE6); ditto(JUMP); ditto(CLIMB);
    make_cond_ins(D, 50, R_LIKE9);
    make_ins(D, R_LIKE4);
    make_loc(q, R_LOW,
             "You are in a large low room.  Crawls lead north, SE, and SW.",
             NULL, 0);
    make_ins(BEDQUILT, R_BEDQUILT);
    make_ins(SW, R_SLOPING);
    make_ins(N, R_CRAWL);
    make_ins(SE, R_ORIENTAL); ditto(ORIENTAL);
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
             "You're at the window on the pit.", 0);
    make_ins(W, R_SJUNC);
    make_ins(JUMP, R_NECK);
    make_ins(REFLECT, R_WINDOE);
    make_loc(q, R_ORIENTAL,
             "This is the oriental room.  Ancient oriental cave drawings cover the" SOFT_NL
             "walls.  A gently sloping passage leads upward to the north, another" SOFT_NL
             "passage leads SE, and a hands-and-knees crawl leads west.",
             "You're in the oriental room.", 0);
    make_ins(SE, R_SWISS);
    make_ins(W, R_LOW); ditto(CRAWL);
    make_ins(U, R_MISTY); ditto(N); ditto(CAVERN);
    make_loc(q, R_MISTY,
             "You are following a wide path around the outer edge of a large cavern." SOFT_NL
             "Far below, through a heavy white mist, strange splashing noises can be" SOFT_NL
             "heard.  The mist rises up through a fissure in the ceiling.  The" SOFT_NL
             "path exits to the south and west and passes alongside the walls of" SOFT_NL
             "the cavern, which have crude drawings on them.  The drawings" SOFT_NL
             "are of animals long extinct and a tiny people.  The people seem to" SOFT_NL
             "resemble dwarves although some of the figures are considerably" SOFT_NL
             "larger than those of the dwarves.  On the wall opposite is a" SOFT_NL
             "large drawing of a castle, complete with turrets and battlements.",
             "You're in the misty cavern.", 0);
    make_ins(S, R_ORIENTAL); ditto(ORIENTAL);
    make_ins(W, R_ALCOVE);
    make_loc(q, R_ALCOVE,
             "You are in an alcove.  A small NW path seems to widen after a short" SOFT_NL
             "distance.  An extremely tight tunnel leads east.  It looks like a very" SOFT_NL
             "tight squeeze.  An eerie light can be seen at the other end.",
             "You're in the alcove.", F_DARK_HINT);
    make_ins(NW, R_MISTY); ditto(CAVERN);
    make_ins(E, R_PPASS); ditto(PASSAGE);
    make_ins(E, R_PLOVER);  /* never performed, but seen by "BACK" */
    make_loc(q, R_PLOVER,
             "You're in a small chamber lit by an eerie green light.  An extremely" SOFT_NL
             "narrow tunnel exits to the west.  A dark corridor leads NE.",
             "You're in the plover room.", F_DARK_HINT);
    make_ins(W, R_PPASS); ditto(PASSAGE); ditto(OUT);
    make_ins(W, R_ALCOVE);  /* never performed, but seen by "BACK" */
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP);
    make_ins(PLOVER, R_Y2);
    make_ins(NE, R_DARK); ditto(DARK);
    make_loc(q, R_DARK,
             "You're in the dark-room.  A corridor leading south is the only exit.",
             "You're in the dark-room.", F_DARK_HINT);
    make_ins(S, R_PLOVER); ditto(PLOVER); ditto(OUT);
    make_loc(q, R_SLAB,
             "You are in a large low circular chamber whose floor is an immense slab" SOFT_NL
             "fallen from the ceiling (Slab Room).  There once were large passages" SOFT_NL
             "to the east and west, but they are now filled with boulders.  Low" SOFT_NL
             "small passages go north and south, and the south one quickly bends" SOFT_NL
             "east around the boulders.",
             "You're in the slab room.", 0);
    make_ins(S, R_W2PIT);
    make_ins(U, R_189); ditto(CLIMB);
    make_ins(N, R_BEDQUILT);
    make_loc(q, R_ABOVER,
             "You are in a secret N/S canyon above a large room.",
             NULL, 0);
    make_ins(D, R_190);
    make_ins(SLAB, R_SLAB);
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
             "You're in the mirror canyon.", 0);
    make_ins(S, R_ABOVER);
    make_ins(N, R_RES); ditto(RESERVOIR);
    make_loc(q, R_RES,
             "You are at the edge of a large underground reservoir.  An opaque cloud" SOFT_NL
             "of white mist fills the room and rises rapidly upward.  The lake is" SOFT_NL
             "fed by a stream, which tumbles out of a hole in the wall about 10 feet" SOFT_NL
             "overhead and splashes noisily into the water somewhere within the" SOFT_NL
             "mist.  A passage goes back toward the south, although you" SOFT_NL
             "can see to the north what appears to be a large cavern, just" SOFT_NL
             "discernible through a dense wall of mist.",
             "You're at the reservoir.", 0);
    make_ins(S, R_MIRROR); ditto(OUT);
    make_ins(ENTER, R_156); ditto(W); ditto(N); ditto(NW);

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
             "You're in the secret E/W canyon above a tight canyon.", 0);
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
             "You're in the sloping corridor.", 0);
    make_ins(D, R_LOW);
    make_ins(U, R_SWSIDE);
    make_loc(q, R_SWSIDE,
             "You are on one side of a large, deep chasm.  A heavy white mist rising" SOFT_NL
             "up from below obscures all view of the far side.  A SW path leads away" SOFT_NL
             "from the chasm into a winding corridor.",
             "You're on SW side of the chasm.", 0);
    make_ins(SW, R_SLOPING);
    make_cond_ins(OVER, only_if_here(TROLL), remark(11)); ditto(ACROSS); ditto(CROSS); ditto(NE);
    make_cond_ins(OVER, unless_prop(CHASM, 0), remark(12));
    make_ins(OVER, R_TROLL);
    make_cond_ins(JUMP, unless_prop(CHASM, 0), R_LOSE);
    make_ins(JUMP, remark(2));
    make_loc(q, R_DEAD0, dead_end, NULL, 0);
    make_ins(S, R_CROSS); ditto(OUT);
    make_loc(q, R_DEAD1, dead_end, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE11); ditto(OUT);
    make_loc(q, R_PIRATES_NEST, dead_end, NULL, 0);
    make_ins(SE, R_LIKE13);
    make_loc(q, R_DEAD3, dead_end, NULL, F_TWIST_HINT);
    make_ins(W, R_LIKE4); ditto(OUT);
    make_loc(q, R_DEAD4, dead_end, 0, F_TWIST_HINT);
    make_ins(E, R_LIKE4); ditto(OUT);
    make_loc(q, R_DEAD5, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_LIKE3); ditto(OUT);
    make_loc(q, R_DEAD6, dead_end, 0, F_TWIST_HINT);
    make_ins(W, R_LIKE9); ditto(OUT);
    make_loc(q, R_DEAD7, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_LIKE10); ditto(OUT);
    make_loc(q, R_DEAD8, dead_end, 0, 0);
    make_ins(E, R_BRINK); ditto(OUT);
    make_loc(q, R_DEAD9, dead_end, 0, F_TWIST_HINT);
    make_ins(S, R_LIKE3); ditto(OUT);
    make_loc(q, R_DEAD10, dead_end, 0, F_TWIST_HINT);
    make_ins(E, R_LIKE12); ditto(OUT);
    make_loc(q, R_DEAD11, dead_end, 0, F_TWIST_HINT);
    make_ins(U, R_LIKE8); ditto(OUT);
    make_loc(q, R_NESIDE,
             "You are on the far side of the chasm.  A NE path leads away from the" SOFT_NL
             "chasm on this side.",
             "You're on NE side of the chasm.", 0);
    make_ins(NE, R_CORR);
    make_cond_ins(OVER, only_if_here(TROLL), remark(11)); ditto(ACROSS); ditto(CROSS); ditto(SW);
    make_ins(OVER, R_TROLL);
    make_ins(JUMP, remark(2));
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_CORR,
             "You're in a long east/west corridor.  A faint rumbling noise can be" SOFT_NL
             "heard in the distance.",
             "You're in the corridor.", 0);
    make_ins(W, R_NESIDE);
    make_ins(E, R_FORK); ditto(FORK);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_FORK,
             "The path forks here.  The left fork leads northeast.  A dull rumbling" SOFT_NL
             "seems to get louder in that direction.  The right fork leads southeast" SOFT_NL
             "down a gentle slope.  The main corridor enters from the west.",
             "You're at the fork in the path.", 0);
    make_ins(W, R_CORR);
    make_ins(NE, R_WARM); ditto(LEFT);
    make_ins(SE, R_LIME); ditto(RIGHT); ditto(D);
    make_ins(VIEW, R_VIEW);
    make_ins(BARREN, R_FBARR);
    make_loc(q, R_WARM,
             "The walls are quite warm here.  From the north can be heard a steady" SOFT_NL
             "roar, so loud that the entire cave seems to be trembling.  Another" SOFT_NL
             "passage leads south, and a low crawl goes east.",
             "You're at the junction with the warm walls.", 0);
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
             "You're at the breath-taking view.", 0);
    make_ins(S, R_WARM); ditto(PASSAGE); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(D, remark(13)); ditto(JUMP);
    make_loc(q, R_CHAMBER,
             "You are in a small chamber filled with large boulders.  The walls are" SOFT_NL
             "very warm, causing the air in the room to be almost stifling from the" SOFT_NL
             "heat.  The only exit is a crawl heading west, through which is coming" SOFT_NL
             "a low rumbling.",
             "You're in the chamber of boulders.", 0);
    make_ins(W, R_WARM); ditto(OUT); ditto(CRAWL);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_LIME,
             "You are walking along a gently sloping north/south passage lined with" SOFT_NL
             "oddly shaped limestone formations.",
             "You're in the limestone passage.", 0);
    make_ins(N, R_FORK); ditto(U); ditto(FORK);
    make_ins(S, R_FBARR); ditto(D); ditto(BARREN);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_FBARR,
             "You are standing at the entrance to a large, barren room.  A sign" SOFT_NL
             "posted above the entrance reads:  \"CAUTION!  BEAR IN ROOM!\"",
             "You're in the front of the barren room.", 0);
    make_ins(W, R_LIME); ditto(U);
    make_ins(FORK, R_FORK);
    make_ins(E, R_BARR); ditto(IN); ditto(BARREN); ditto(ENTER);
    make_ins(VIEW, R_VIEW);
    make_loc(q, R_BARR,
             "You are inside a barren room.  The center of the room is completely" SOFT_NL
             "empty except for some dust.  Marks in the dust lead away toward the" SOFT_NL
             "far end of the room.  The only exit is the way you came in.",
             "You're in the barren room.", 0);
    make_ins(W, R_FBARR); ditto(OUT);
    make_ins(FORK, R_FORK);
    make_ins(VIEW, R_VIEW);

    make_loc(q, R_141,
             "You are in a magnificently ornate room that looks like a place" SOFT_NL
             "of worship for some old and mysterious religion. There are exits" SOFT_NL
             "to the north, west and east and a spiral staircase leading both" SOFT_NL
             "up and down. The ceiling of this chapel seems to be made of large" SOFT_NL
             "wooden beams. Hanging from one of these beams is a thick rope.",
             "You're in the chapel.", 0);
    make_ins(TESTO, R_HOUSE);
    make_ins(CLIMB, R_144);
    make_ins(N, R_ANTE);
    make_cond_ins(E, 100, R_142);           /* dwarves not permitted */
    make_cond_ins(W, 100, R_169); ditto(U); /* dwarves not permitted */
    make_ins(D, R_168);
    make_loc(q, R_142,
             "You are in a room that appears to be a stable for a fearsome" SOFT_NL
             "animal.  Against one wall is a battered and dirty trough that" SOFT_NL
             "is quite empty and on the other wall is a huge harness.  Beside" SOFT_NL
             "the harness is a small window that overlooks a courtyard.  The" SOFT_NL
             "courtyard is deserted and shows no signs of any recent activity." SOFT_NL
             "At the far end of the stable is a wooden partition that has" SOFT_NL
             "numerous dents and holes in it and you can see that it is" SOFT_NL
             "securely fixed to the massive stone walls so that whatever" SOFT_NL
             "is behind it cannot get out.  If you listen carefully you can" SOFT_NL
             "hear the muted sounds of growling and the scratching of claws" SOFT_NL
             "against wood.  The only exit is the way you came in.",
             "You're in the stable room.", 0);
    make_ins(W, R_141);
    make_loc(q, R_143,
             "You are in a chamber which is obviously some sort of living quarters." SOFT_NL
             "The room is lit, although you cannot determine how. At the far end" SOFT_NL
             "is a very large solidly built couch.",
             "You're in the living quarters.", 0);
    make_ins(E, R_PANTRY);
    make_cond_ins(N, 100, R_DUNGEON);
    make_ins(S, R_146);
    make_loc(q, R_144,
             "You have climbed the rope and crawled into a small recess in the" SOFT_NL
             "beams.",
             "You're in the recess in the beams.", 0);
    make_ins(D, R_141); ditto(OUT);
    make_loc(q, R_DUNGEON,
             "You are in a dungeon.  The walls and ceiling appear to be made of" SOFT_NL
             "solid rock and the floor is made up of tightly fitting flagstones." SOFT_NL
             "High on the walls are some stanchions for chains, but the chains are" SOFT_NL
             "no longer in place.  The only sound, apart from your own breathing," SOFT_NL
             "is the occasional plop of water as it drips from the ceiling.  The" SOFT_NL
             "atmosphere is dank and clammy and there is an unusual smell in the" SOFT_NL
             "air.  A passage leads back to the south.",
             "You're in the dungeon.", 0);
    make_ins(S, R_GRAB); ditto(OUT);

    /* This is the spider maze. */
    make_loc(q, R_146, tangled, NULL, 0);
      make_ins(N, R_GRAB);
      make_ins(E, R_147);
      make_ins(W, R_151);
      make_ins(U, R_150);
      make_cond_ins(D, 100, R_148);
      make_ins(PANTRY, R_PANTRY);
    make_loc(q, R_147, tangled, NULL, 0);
      make_ins(N, R_146);
      make_ins(E, R_147); ditto(D);
      make_ins(W, R_151);
      make_cond_ins(S, 100, R_148);
      make_ins(U, R_149);
    make_loc(q, R_148, tangled, NULL, 0);
      make_ins(N, R_147);
      make_ins(E, R_148); ditto(D);
      make_ins(W, R_146);
      make_ins(S, R_149);
      make_ins(U, R_150);
    make_loc(q, R_149, tangled, NULL, 0);
      make_ins(N, R_147);
      make_cond_ins(E, 100, R_148);
      make_ins(W, R_151);
      make_ins(U, R_150);
    make_loc(q, R_150, tangled, NULL, 0);
      make_ins(N, R_151);
      make_ins(E, R_146);
      make_ins(W, R_150); ditto(U);
      make_ins(S, R_149);
      make_cond_ins(D, 100, R_148);
    make_loc(q, R_151, tangled, NULL, 0);
      make_ins(N, R_151); ditto(W);
      make_ins(E, R_146);
      make_ins(S, R_149);
      make_ins(U, R_150);
      make_ins(D, R_147);

    make_loc(q, R_152,
             "You are in a room which appears to be a basement for a very large" SOFT_NL
             "building. There is an opening to the south above which hangs a slab" SOFT_NL
             "of rock precisely balanced by counterweights suspended from the" SOFT_NL
             "ceiling.  A notice carved in the rock says \"Wizards Only. Keep Out.\"" SOFT_NL
             "There are some steps leading upward.",
             "You're in the basement.", 0);
    make_cond_ins(S, 100, R_CELLAR);
    make_ins(U, R_153);
    make_loc(q, R_153,
             "You are at an entrance to a large room which you cannot see into" SOFT_NL
             "because of a curtain blocking your view. There are flights of" SOFT_NL
             "steps here leading both upward and downward.",
             "You're at the curtain entrance.", 0);
    make_ins(D, R_152);
    make_ins(U, R_PANTRY);
    make_ins(ENTER, R_SOFT); ditto(IN); ditto(NW);
    make_loc(q, R_CELLAR,
             "You are in a round cellar made completely of brick. In the center" SOFT_NL
             "of the floor is a small circular drain, towards which the floor" SOFT_NL
             "slopes.  Set in the south wall is a portcullis which is so rusty" SOFT_NL
             "it clearly will never move again. Through the portcullis a small" SOFT_NL
             "chamber with a spiral staircase can be seen.  An entrance leads" SOFT_NL
             "north, but it is blocked by a large stone slab.",
             "You're in the circular cellar.", 0);
    make_ins(BACK, R_CELLAR);  /* perhaps because every room needs at least one exit? TODO investigate this line */
    make_loc(q, R_155,
             "In front of you, just discernible in the gloom, lies an old streambed" SOFT_NL
             "which has been long dry. You can just determine that there is a" SOFT_NL
             "stone causeway leading across the gap.",
             "You're west of the causeway.", 0);
    make_ins(OVER, R_156); ditto(ACROSS); ditto(CROSS); ditto(E);
    make_cond_ins(E, 100, R_195); ditto(N); ditto(S); ditto(NE); ditto(SE);
    make_ins(D, R_199);
    make_loc(q, R_156,
             "In front of you, just discernible in the gloom, lies an old streambed" SOFT_NL
             "which has been long dry. You can just determine that there is a" SOFT_NL
             "stone causeway leading across the gap.",
             "You're east of the causeway.", 0);
    make_ins(OVER, R_155); ditto(ACROSS); ditto(CROSS); ditto(W);
    make_ins(S, R_RES); ditto(SE); ditto(SW);
    make_ins(D, R_199);
    make_loc(q, R_157, "", NULL, 0); /* unused? TODO remove these rooms */
    make_loc(q, R_158, "", NULL, 0);
    make_loc(q, R_159, "", NULL, 0);
    make_loc(q, R_160,  /* completely unreachable? TODO investigate these rooms */
             "You are at a window halfway up a flight of steps. The view is" SOFT_NL
             "quite beautiful: a lake with rolling green meadows beyond it," SOFT_NL
             "gradually rising to large snow-capped mountains.",
             NULL, 0);
    make_cond_ins(U, 50, R_161);
    make_cond_ins(U, 50, R_162);
    make_ins(U, R_163); ditto(TOWER);
    make_ins(D, R_CELLAR); ditto(BEDCHAMBER);
    make_loc(q, R_161,
             "You are at a window halfway up a flight of steps. The view is" SOFT_NL
             "quite beautiful: a deep ravine with a raging torrent" SOFT_NL
             "tumbling over rapids.",
             NULL, 0);
    make_cond_ins(U, 50, R_162);
    make_ins(U, R_163); ditto(TOWER);
    make_cond_ins(D, 50, R_160);
    make_ins(D, R_CELLAR); ditto(BEDCHAMBER);
    make_loc(q, R_162,
             "You are at a window halfway up a flight of steps. The view is" SOFT_NL
             "quite beautiful: below you is a cliff, an enraged sea battering" SOFT_NL
             "at its base.",
             NULL, 0);
    make_ins(U, R_163); ditto(TOWER);
    make_cond_ins(D, 50, R_161);
    make_cond_ins(D, 50, R_160);
    make_ins(D, R_CELLAR); ditto(BEDCHAMBER);  /* end TODO investigate these rooms */
    make_loc(q, R_163, "", NULL, 0);  /* dwarves' quarters -- no access */
    make_ins(D, R_149);  /* dwarves reappear in the tangled web of passages */
    make_loc(q, R_164, "", NULL, 0);  /* unused? TODO remove this room */
    make_loc(q, R_165,
             "You are in a winding earthen passage which slopes down to the south." SOFT_NL
             "The lower end of the passage becomes narrower and damp.",
             NULL, 0);
    make_ins(D, R_166); ditto(S);
    make_loc(q, R_166,
             "You are in a small circular chamber which appears to be a disused" SOFT_NL
             "well. Looking up, you can see that the walls of the well are" SOFT_NL
             "damp and crumbling, but should be easy to climb. However, there is" SOFT_NL
             "no indication of an opening at the top of the well." SOFT_NL
             "An earthen passage leads up and to the north.",
             NULL, 0);
    make_ins(U, R_165); ditto(N);
    make_ins(CLIMB, R_167);
    make_loc(q, R_168,
             "You are in a crypt.  All around you are the tombs of people long" SOFT_NL
             "dead.  The dust on the vaults would seem to indicate that the place" SOFT_NL
             "is rarely used. High above you massive cobwebs almost cover the" SOFT_NL
             "ceiling.  A spiral staircase which leads upwards is the only exit.\n"
             "Brrr! It is cold down here!",
             "You're in the crypt.", 0);
    make_ins(U, R_141); ditto(OUT);
    make_loc(q, R_169,
             "You are in a chamber above the entrance to the chapel. A spiral" SOFT_NL
             "staircase leads down and openings lead to the north and west.",
             "You're in the chamber above the chapel.", 0);
    make_ins(D, R_141); ditto(OUT);
    make_ins(N, R_170);
    make_ins(W, R_PANTRY);
    make_loc(q, R_170,
             "You are at the south end of a long low attic.",
             NULL, 0);
    make_ins(S, R_169);
    make_ins(N, R_171);
    make_ins(SW, R_PANTRY);
    make_loc(q, R_171,
             "You are at the north end of a long low attic.\n"
             "A passage leads on to the north.",
             NULL, 0);
    make_ins(S, R_170);
    make_ins(N, R_172);
    make_loc(q, R_172,
             "This must be a thieves' den!\n"
             "High on one wall, out of reach, hangs a large bag labelled \"SWAG\"" SOFT_NL
             "and a black mask.  The only exit is to the south.",
             "You're in the thieves' den.", 0);
    make_ins(S, R_171); ditto(OUT);
    make_loc(q, R_PANTRY,
             "You are in a room which shows signs of having been used as a pantry." SOFT_NL
             "The dust on the shelves would seem to indicate that it has had" SOFT_NL
             "little use.  The walls are damp and there is a stream of water" SOFT_NL
             "running down the walls in one corner. Passages lead to the east," SOFT_NL
             "northeast and west. From an archway to the north, stone steps" SOFT_NL
             "lead down.",
             "You're in the pantry.", 0);
    make_ins(W, R_GRAB);
    make_ins(E, R_169);
    make_ins(NE, R_170);
    make_ins(D, R_153); ditto(N);
    make_loc(q, R_174,
             "You are in a long north/south passage whose roof is supported" SOFT_NL
             "in places by wooden props.  Some of the props have recently been" SOFT_NL
             "replaced with new timber, but fresh rock falls are also apparent." SOFT_NL
             "One of these falls has exposed a fracture in the west wall which you" SOFT_NL
             "could squeeze through.",
             "You're in the unsafe tunnel.", 0);
    make_ins(S, R_ANTE);
    make_ins(N, R_175);
    make_cond_ins(CRACK, 100, R_TPASS); ditto(W); ditto(HOLE); ditto(SLIT);
    make_loc(q, R_175,
             "A smell of decaying timber fills this part of the passage." SOFT_NL
             "To the north the passage is blocked by a larger than usual rock fall" SOFT_NL
             "and the whole passage looks as if it could collapse at any moment.",
             NULL, 0);
    make_ins(S, R_174); ditto(OUT);
    make_loc(q, R_176,
             "You are in a very low sloping room whose roof is barely three feet" SOFT_NL
             "from the floor.  At the lower end a trickle of water enters from a" SOFT_NL
             "tunnel in the west and runs down a shaft in the floor.  At the" SOFT_NL
             "upper end the roof nearly meets the floor to give a narrow exit." SOFT_NL
             "Water drips from the roof making everything damp and the floor" SOFT_NL
             "slippery. There are no signs that anyone has been this way before.",
             "You're in the very low room.", 0);
    make_ins(U, R_TPASS); ditto(E);
    make_ins(W, R_177); ditto(UPSTREAM);
    make_ins(D, R_178);
    make_loc(q, R_177,
             "The passage opens into a sizable cave which does not appear to" SOFT_NL
             "have any other exit, although a number of huge bones lie here." SOFT_NL
             "Close inspection shows that they are very old and are covered with" SOFT_NL
             "strange score marks which have been made recently. There are also" SOFT_NL
             "some droppings which might have been made by rats if they weren't" SOFT_NL
             "so large.",
             "You're in the bones room.", 0);
    make_ins(E, R_176); ditto(OUT);
    make_loc(q, R_178,
             "You are in an inclined shaft which steepens at this point. The walls" SOFT_NL
             "are covered in a thin layer of slime, making it very slippery.  Below" SOFT_NL
             "you the shaft is filled with dark water which surges rhythmically." SOFT_NL
             "The smell of the sea is mingled with an altogether more unpleasant" SOFT_NL
             "odour.",
             "You're on the inclined shaft.", 0);
    make_ins(U, R_176); ditto(OUT);
    make_ins(JUMP, R_181); ditto(D);

    make_loc(q, R_179,
             "You are in an inclined shaft which steepens at this point. The walls" SOFT_NL
             "of the shaft are covered in a thin layer of slime, making it very" SOFT_NL
             "slippery.  Below you the shaft steepens and enters a N/S culvert" SOFT_NL
             "partially filled with a brackish evil-smelling liquid.  You could" SOFT_NL
             "slide down here but you could not climb back up.",
             "You're on the inclined shaft above a N/S culvert.", 0);
    make_ins(U, R_176);
    make_ins(JUMP, R_181); ditto(D);
    make_loc(q, R_180,
             "The sea can be seen through a strong metal grill which spans the" SOFT_NL
             "tunnel.  A quantity of debris is stacked against the grill and pieces" SOFT_NL
             "of debris are caught in the grill nearly to the roof.  High on the" SOFT_NL
             "opposite side of the culvert is a line scored into the brick" SOFT_HYPHEN
             "work with a faded ornate inscription above it that reads:" SOFT_NL
             "\"CAUTION " EMDASH(" - ") " HIGH TIDE LEVEL!\"",
             "You're at the sea-view.", 0);
    make_ins(S, R_181); ditto(IN);
    make_loc(q, R_181,
             "The brick culvert goes north and south. It seems to narrow to the" SOFT_NL
             "south.",
             NULL, 0);
    make_ins(N, R_180);
    make_ins(S, R_182);
    make_loc(q, R_182,
             "Sewage pours into the culvert from a large number of sewage pipes" SOFT_NL
             "of all sizes.  Those entering from the east, se, south, sw, west and" SOFT_NL
             "nw are large enough to crawl through. A culvert exits to the north.",
             "You're at the south end of the culvert.", 0);
    make_ins(N, R_181);
    make_ins(E, R_183); ditto(S); ditto(SE);
    make_ins(W, R_191); ditto(NW);
    make_ins(SW, R_186);
    make_loc(q, R_183,
             "You are at a junction of three sewer pipes which go se, sw and north.",
             NULL, 0);
    make_ins(N, R_182);
    make_ins(SE, R_184); ditto(SW);
    make_loc(q, R_184,
             "The large-diameter pipe from the north splits into two smaller-diameter" SOFT_NL
             "pipes going south and south-east. You could get through either of" SOFT_NL
             "them but it does not look advisable.",
             "You are at the large sewer pipe.", 0);
    make_ins(N, R_183);
    make_ins(S, R_185); ditto(SE);
    make_loc(q, R_185,
             "The sewer pipe has got too narrow; you will have to go back.",
             "Dead end; the sewer pipe has become too narrow.", 0);
    make_ins(N, R_184); ditto(OUT);
    make_loc(q, R_186,
             "You are at a junction of three sewer pipes, which has partially collapsed" SOFT_NL
             "where a stream enters the pipe. You cannot get past but you could" SOFT_NL
             "crawl through the fracture in the pipe where the stream enters.",
             NULL, 0);
    make_ins(NE, R_182);
    make_ins(CRAWL, R_187); ditto(IN); ditto(U); ditto(CRACK);
    make_loc(q, R_187,
             "You're in a maze of twisting passages through which a stream cascades.",
             NULL, 0);
    make_cond_ins(E, 60, R_BEDQUILT); ditto(W); ditto(N); ditto(S); ditto(U);
    make_ins(E, R_190); ditto(W); ditto(N); ditto(S); ditto(U);  /* TODO: dittoes are unneeded, right? */
    make_ins(DOWNSTREAM, R_187); ditto(D);
    make_loc(q, R_188, "", NULL, 0);  /* Dwarves' store (no access) */
    make_ins(OUT, R_163);
    make_loc(q, R_189,
             "You are at the west end of a very large and long tunnel. To the west" SOFT_NL
             "the way is almost blocked by rock falls. Several very large" SOFT_NL
             "footprints can be seen indistinctly in the dust. High above you there" SOFT_NL
             "is a huge arrow on the wall pointing east.",
             NULL, 0);
    make_ins(D, R_SLAB); ditto(W);
    make_ins(E, R_190);
    make_loc(q, R_190,
             "You are in a very large room with several rock falls at the eastern" SOFT_NL
             "end. A very large passage goes west or you could crawl between" SOFT_NL
             "some of the large boulders in the rockfalls.",
             NULL, 0);
    make_ins(W, R_189);
    make_ins(E, R_ABOVER); ditto(CRAWL); ditto(U);
    make_loc(q, R_191,
             "The pipe has got too narrow; you will have to go back.",
             NULL, 0);
    make_ins(E, R_182); ditto(OUT);
    make_loc(q, R_192,
             "You are in a small, low cavern that is completely bare.  On the" SOFT_NL
             "walls are scratch marks that look as if they were made by tiny" SOFT_NL
             "claws.  The cavern exits to the west and the north.",
             "You're in the low cavern.", 0);
    make_ins(W, R_SOFT); ditto(OUT);
    make_ins(N, R_193);
    make_loc(q, R_193,
             "You are in a long, low passage that appears to have made by some" SOFT_NL
             "subterranean water source.  The passage itself is of bare rock," SOFT_NL
             "polished smooth by the water that apparently flows no longer, for" SOFT_NL
             "the floor and walls are quite dry.  The passage heads north-south" SOFT_NL
             "and bends after a short distance.",
             "You're in the low, long passage.", 0);
    make_ins(S, R_192);
    make_ins(N, R_194);
    make_loc(q, R_194,
             "You are in a large cavern whose roof extends further than the beam" SOFT_NL
             "of your lamp can penetrate.  To the north is a large opening but an" SOFT_NL
             "eerie mist fills the opening and you cannot see what lies beyond." SOFT_NL
             "A passage also leads south-west.",
             "You're in the large cavern.", 0);
    make_ins(SW, R_193);
    make_cond_ins(N, 100, R_195);
    make_loc(q, R_195,
             "You are in the middle of a swirling mist that eddies to and fro around" SOFT_NL
             "you. The sound of thundering water can be heard afar off and" SOFT_NL
             "occasionally quick footsteps closer, but it is difficult to tell the" SOFT_NL
             "direction of the sound.",
             "You're in the swirling mist.", 0);
    make_cond_ins(SE, 20, R_194); ditto(S); ditto(SW);
    make_cond_ins(N, 20, R_155); ditto(NE); ditto(NW);
    make_ins(E, R_195); ditto(W); ditto(N); ditto(S); ditto(NE); ditto(SE); ditto(SW); ditto(NW);
    make_loc(q, R_197,
             "You are at the halfway stage on the spiral staircase.  The beam from" SOFT_NL
             "your lamp is reflected by the minerals contained in the walls and you" SOFT_NL
             "can see great seams of gold-bearing rock.  A strong draft from the" SOFT_NL
             "top of the staircase moans and whistles through the rusted tracery" SOFT_NL
             "of the spiral staircase.",
             "You're at the halfway stage on the spiral staircase.", 0);
    make_ins(U, R_SOUTH);
    make_cond_ins(D, 100, R_198);
    make_loc(q, R_198,
             "You are at the bottom of the spiral staircase in a small room carved" SOFT_NL
             "out of the rock.  A strong wind blows down the shaft and through" SOFT_NL
             "a large portcullis forming one wall of the room. The portcullis is" SOFT_NL
             "rusted solidly in position and clearly no longer moves. A round" SOFT_NL
             "cellar with a circular drain can be seen through the portcullis.",
             "You're at the cellar view.", 0);
    make_ins(U, R_197);

    /* The end-game repository. */
    make_loc(q, R_NEEND,
             "You are at the northeast end of an immense room, even larger than the" SOFT_NL
             "Giant Room.  It appears to be a repository for the \"Adventure\"" SOFT_NL
             "program.  Massive torches far overhead bathe the room with smoky" SOFT_NL
             "yellow light.  Scattered about you can be seen a pile of bottles (all" SOFT_NL
             "of them empty), a nursery of young beanstalks murmuring quietly, a bed" SOFT_NL
             "of oysters, a bundle of black rods with rusty stars on their ends, and" SOFT_NL
             "a collection of brass lanterns.  Off to one side a great many dwarves" SOFT_NL
             "are sleeping on the floor, snoring loudly.  A sign nearby reads: \"DO" SOFT_NL
             "NOT DISTURB THE DWARVES!\"  An immense mirror is hanging against one" SOFT_NL
             "wall, and stretches to the other end of the room, where various other" SOFT_NL
             "sundry objects can be glimpsed dimly in the distance.",
             "You're at the NE end.", 0);
    make_ins(SW, R_SWEND);
    /* The following description has several minor differences from Woods' original.
     * Woods' line breaks come after "A" on lines 4 and 5, "large" on line 6, and
     * "vault" on line 7. Knuth's "that reads" corresponds to Woods' "which reads";
     * presumably Knuth changed it to avoid ugly repetition, and I agree. */
    make_loc(q, R_SWEND,
             "You are at the southwest end of the repository.  To one side is a pit" SOFT_NL
             "full of fierce green snakes.  On the other side is a row of small" SOFT_NL
             "wicker cages, each of which contains a little sulking bird.  In one" SOFT_NL
             "corner is a bundle of black rods with rusty marks on their ends." SOFT_NL
             "A large number of velvet pillows are scattered about on the floor." SOFT_NL
             "A vast mirror stretches off to the northeast.  At your feet is a" SOFT_NL
             "large steel grate, next to which is a sign that reads, \"TREASURE" SOFT_NL
             "VAULT.  KEYS IN MAIN OFFICE.\"",
             "You're at the SW end.", 0);
    make_ins(NE, R_NEEND);
    make_ins(D, remark(1));  /* You can't go through a locked steel grate! */

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
    make_loc(q, R_167,
             "You clamber up the sides of the well and upon reaching the top" SOFT_NL
             "find a stone slab blocking your ascent. You give the slab a push and" SOFT_NL
             "it opens. You clamber through into daylight and look round just in" SOFT_NL
             "time to see the slab fall perfectly into the floor, so perfectly you" SOFT_NL
             "cannot see the join.\n",
             NULL, 0);
    make_ins(0, R_HOUSE);
    make_loc(q, R_196,
             "There is solid rock behind the curtain.",
             NULL, 0);
    make_ins(0, R_SOFT);
    make_loc(q, R_199, "You are at the bottom of the streambed and all your bones are broken.", NULL, 0);
    make_ins(0, R_LIMBO);

#undef tangled
#undef dead_end
#undef all_alike


#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "advdat.h"
#include "getwds.h"

static char txt[35][13];  /* original spellings */
static int wdx;  /* count of words in "txt" */
static int words[45];  /* holds vocabulary words */

int w_verbs[45], w_objs[45], w_iobjs[15], prep;  /* holds parsed words */
char vtxt[45][16], otxt[45][16], iotxt[15][16];  /* spellings */
int vrbx, objx, iobx;  /* which word we're processing right now */

#define REPARSE_ME -2

static int getobj(ObjectWord t, Location loc);
static bool is_appropriate_adjective_for_noun(AdjectiveWord adj, ObjectWord noun);
static bool is_valid_verb_prep_iobj(int verb, int prep, ObjectWord iobj);

void clrlin()
{
    memset(w_objs, 0, sizeof w_objs);
    memset(w_verbs, 0, sizeof w_verbs);
    memset(w_iobjs, 0, sizeof w_iobjs);
    memset(vtxt, 0, sizeof vtxt);
    memset(otxt, 0, sizeof otxt);
    memset(iotxt, 0, sizeof iotxt);
    vrbx = objx = iobx = 0;
    prep = NOTHING;
}

/* This function is called only from getwds().
 * Its job is to repopulate the "txt" array, and set "wdx"
 * to the number of words in the array. */
void getlin()
{
    static char buffer[100];
    char *q = &txt[0][0];

    memset(txt, '\0', sizeof txt);
    memset(words, '\0', sizeof words);
    wdx = 0;

    printf("\n> "); fflush(stdout);
    fgets(buffer, sizeof buffer, stdin);

    /* Trim leading and trailing whitespace.
     * Lowercase the entire string.
     * Replace punctuation with the word "and".
     * Truncate words to 12 characters and store them in txt[].
     */
    for (char *p = buffer; *p != '\0'; ++p) {
        const bool character_is_comma = (*p == ',' || *p == ';' || *p == '.');
        const bool start_of_word = (q == &txt[wdx][0]);
        if (isspace(*p) || character_is_comma) {
            if (!start_of_word) {
                /* Finish the current word. */
                ++wdx;
                q = &txt[wdx][0];
            }
            if (character_is_comma) {
                strcpy(q, "and");
                ++wdx;
                q = &txt[wdx][0];
            }
        } else {
            /* Avoid buffer overflow. */
            if (q < &txt[wdx][12])
                *q++ = tolower(*p);
        }
    }

    /* Finish off the last word, if needed. */
    if (q != &txt[wdx][0]) {
        wdx += 1;
    }

    /* Collapse two adjacent "and"s into one. */
    int v = 0;
    for (int w = 0; w < wdx; ++v, ++w) {
        assert(txt[w][0] != '\0');  /* every word must be non-blank */
        if (w+1 < wdx && streq(txt[w], "and") && streq(txt[w+1], "and")) {
            /* do not preserve this word */
            --v;
        } else if (v != w) {
            assert(v < w);
            strcpy(txt[v], txt[w]);
        }
    }
    assert(v <= wdx);
    wdx = v;

    if (wdx == 0) {
        /* If nothing was entered, get another command. In Long's code,
         * there's no message here; we just go back to the prompt. */
        puts("Tell me to do something.");
        getlin();
    }
}


/* Synonyms for WATER in the construct "ENTER WATER". */
static bool is_water_spelling(const char *what)
{
    return (streq(what, "stream") || streq(what, "water") ||
        streq(what, "reserv") || streq(what, "ocean") ||
        streq(what, "sea") || streq(what, "pool"));
}

/* Things that the player can WATER or OIL. */
static bool is_oilable_spelling(const char *what)
{
    return (streq(what, "plant") || streq(what, "door") ||
            streq(what, "sword") || streq(what, "anvil"));
}

/* Synonyms for WALK in the construct "TAKE WALK". */
static bool is_take_a_hike_spelling(const char *what)
{
    return (streq(what, "walk") || streq(what, "run") ||
            streq(what, "hike"));
}

static bool can_TAKE_direction(MotionWord mot)
{
    switch (mot) {
        case ROAD: case FOREST: case VALLEY: case STAIRS:
        case OUT: case GULLY: case STREAM: case CRAWL:
        case PASSAGE: case CANYON: case CRACK: case STEPS:
        case LEFT: case RIGHT: case JUMP: case CHIMNEY:
        case SLIDE: case WHIRLPOOL: case BRIDGE:
            return true;
    }
    return false;
}

void confuz()
{
    switch (ran(5)) {
        case 0: puts("I'm afraid I don't understand."); break;
        case 1: puts("Huh?"); break;
        case 2: puts("I don't understand that!"); break;
        case 3: puts("What?"); break;
        case 4: puts("I can't make any sense out of that."); break;
    }
}

void noway(void)
{
    switch (ran(8)) {
        case 0: puts("Interesting.  How?"); break;
        case 1: puts("I don't think I can."); break;
        case 2: puts("You can't be serious!"); break;
        case 3: puts("I think you are a little confused!"); break;
        case 4: puts("You've got to be kidding!"); break;
        case 5: puts("I don't know how."); break;
        case 6: puts("Don't be ridiculous!"); break;
        case 7: puts("I'm game.  Would you care to explain how?"); break;
    }
}

static void advise_about_going_west(const char *spelling)
{
    /* Here's a freely offered hint that may save you typing. */
    static int west_count = 0;
    if (streq(spelling, "west")) {
        ++west_count;
        if (west_count == 10) {
            puts("If you prefer, simply type \"W\" rather than \"WEST\".\n");
        }
    }
}

static int I_see_no(const char *what)
{
    printf("I see no %s here.\n", what);
    return NOTHING;
}

/* "With what, your bare hands?"
 * Call getlin() to get a completely new line of input.
 * */
bool with_your_bare_hands(void)
{
    clrlin();
    getlin();
    if (streq(txt[0], "y") || streq(txt[0], "yes")) {
        /* What a brave adventurer! */
        return true;
    } else {
        /* Re-parse this line next time getwds() is called. */
        words[0] = REPARSE_ME;
        assert(words[wdx+1] == NOTHING);  /* see top of getwds() */
        return false;
    }
}

#if 0  /* TODO: Debugging code. */
#define lin(x) lin##x: puts("lin" #x);
#else
#define lin(x) lin##x:
#endif /* 0 */

/* This function is called from the main loop.
 * Its job is to parse out the next clause of the player's input
 * into the globals "w_verbs", "w_objs", "w_iobjs", and "prep";
 * and then update wdx to point to the first word of the next
 * clause. Nobody above this level should be accessing "txt" or
 * "wdx" directly. */
void getwds(Location loc)
{
    int wclass = WordClass_None;
    bool pflag;
    int word;

    if (words[wdx] == NOTHING) {
        if (words[0] == REPARSE_ME) {
            /* with_your_bare_hands() sets words[0] if it's read in the line
             * but the line wasn't a "yes". In that case we come back here
             * to parse it as a regular command. */
        } else {
            /* We need to grab a new line. */
lin(20)
            getlin();
        }
        assert(wdx > 0);
        for (int i=0; i < wdx; ++i) {
            words[i] = lookup(txt[i], WordClass_None);
            if (words[i] == NOTHING)
                words[i] = BAD_WORD;
        }
        words[wdx] = NOTHING;

        /* getlin() sets wdx to the number of words read.
         * getwds() sets wdx to the index of the next unprocessed word. */
        wdx = 0;
    }

    assert(words[wdx] != NOTHING);

    pflag = false;
    word = words[wdx++];
    if (word == NOTHING) goto lin800;
    if (word == BAD_WORD) goto lin841;  /* We didn't recognize this word. */
    switch (word_class(word)) {
        case WordClass_None:
        case WordClass_BadWord:
            assert(false);
        case WordClass_Action:
        case WordClass_Motion:
        case WordClass_Message:
            /* Begin a new clause. */
            clrlin();
            break;
    }
    if (word == SAY || word == CALL) {
        if (words[wdx] == NOTHING || words[wdx] == AND) {
            goto lin99;
        }
        if (word == SAY) {
            printf("Okay, \"%s\".\n", txt[wdx]);
        } else {
            printf("Okay, \"%s\"!!!!!\n", txt[wdx]);
        }
        word = words[wdx++];
        switch (word) {
            case XYZZY:
            case PLUGH:
            case PLOVER:
            case PHUCE:
            case FEEFIE:
                /* Note that SAINTMICHEL is not handled here.
                 * That's because that puzzle was added later in the
                 * game's development. Long added PHUCE in the right
                 * place here; Anon didn't do as good a job. */
                goto lin99;
        }
        goto lin860;
    }

    if (word == ENTER) {
        if (words[wdx] == NOTHING) goto lin99;
        if (is_water_spelling(txt[wdx])) {
            if (liquid_at_location(loc) == WATER) {
                puts("Your feet are now wet.");
            } else {
                puts("Where?");
            }
            /* "ENTER WATER APPLES ORANGES" is quietly accepted,
             * thanks to this call to clrlin(). */
            clrlin();
            goto lin20;
        } else if (words[wdx] == BOAT) {
            /* Long accepts only the spellings "BOAT" and "ROWBOA" here,
             * not "SHIP"; but it's simpler to accept "SHIP" as well. */
            word = TAKE;
            goto lin99;
        }
    }

    if (streq(txt[wdx-1], "leave") && word_class(words[wdx]) == WordClass_Object) {
        if (!is_hinged(words[wdx]) && !is_immobile(words[wdx]))
            word = LEAVE;
        goto lin99;
    }

    /* The spelling "light" usually refers to the LAMP in this version;
     * for example, "GET LIGHT". The exception is Long's line 55, which
     * handles the command "LIGHT LAMP". I don't understand how, but
     * empirically "LIGHT" is interpreted in the following ways:
     *     LIGHT -- verb
     *     LIGHT LAMP -- verb
     *     LIGHT POSTER -- verb
     *     LAMP LIGHT -- noun
     *     LIGHT LIGHT -- verb noun
     *     GET LIGHT -- noun
     *     LIGHT GET -- verb
     *     GET AND LIGHT LAMP -- noun
     * This suggests that somehow the rule is much simpler than Long's
     * line 55 implies on first reading: iff LIGHT is the first word on
     * the line, it's treated as a verb. The same rule applies to TREE
     * (MessageWord preferred over ObjectWord) and WALL (MotionWord
     * over ObjectWord).
     *
     * TODO: investigate this codepath.
     */
    if (wdx == 1 && streq(txt[wdx-1], "light")) {
        word = LIGHT;
        goto lin99;
    } else if (wdx == 1 && streq(txt[wdx-1], "tree")) {
        word = TREES;
        goto lin99;
    } else if (wdx == 1 && streq(txt[wdx-1], "wall")) {
        word = WALL;
        goto lin99;
    }

    if ((streq(txt[wdx-1], "water") || streq(txt[wdx-1], "oil")) &&
            is_oilable_spelling(txt[wdx])) {
        ObjectWord object_to_oil = lookup(txt[wdx], WordClass_Object);
        assert(object_to_oil != NOTHING);
        if (here(object_to_oil, loc)) {
            words[wdx] = words[wdx-1];
            strcpy(txt[wdx], txt[wdx-1]);
        }
        /* Apparently OIL SWORD redirects to POUR SWORD when the
         * sword is not in scope; but OIL LAMP remains as OIL LAMP.
         * The first gives "I see no sword here."; the second
         * gives either "I see no oil here." or "Huh?", depending
         * on whether the oil is in scope.
         * I think that's simply an off-by-one in Long's gotos.
         * TODO: fix this.
         */
        word = POUR;
        goto lin99;
    }

    /* Long has some unfinished code here to deal with
     * the spelling EMPTY, as in "EMPTY SAFE". But it doesn't
     * do anything, so I'm skipping it. */

    /* Now for the inner loop! */
    goto lin91;
lin(90)
    word = words[wdx++];
lin(91)
    if (word == BAD_WORD) goto lin841;  /* unrecognized word */
    if (word == NOTHING) goto lin900;
    goto lin92;

lin(96)
    /* The last word didn't make sense in context. Try again with the next
     * meaning of the spelling; for example if LOOK(action) IN(motion)...
     * doesn't make sense, then try LOOK(action) IN(preposition)... */
    assert(wclass != WordClass_None);
    while (wclass != WordClass_Conjunction) {
        ++wclass;
        word = lookup(txt[wdx-1], wclass);
        assert(word >= 0);
        if (word != NOTHING) {
            words[wdx-1] = word;
            goto lin92;
        }
    }
    goto lin800;  /* nothing made sense */

lin(92)
    assert(wdx > 0);
    if (word_class(word) == WordClass_Object) {
	/* TODO: this doesn't actually work at the moment */
        /* "THROW BEAR EGGS" => THROW EGGS [TO] BEAR
         * "THROW BEAR EGGS AND HONEY" => THROW {EGGS,HONEY} [TO] BEAR
         * "THROW EGGS TO BEAR AND HONEY" => THROW EGGS [TO] {BEAR,HONEY} */
        int toss_verb = ((vrbx >= 1) ? w_verbs[vrbx-1] : NOTHING);
        if ((toss_verb == TOSS || toss_verb == FEED) &&
            (objx == 1 && is_living(w_objs[0])) &&
            (iobx == 0) &&
            !pflag) {
            /* We're parsing "THROW BEAR EGGS"; the cursor is on EGGS.
             * Shift "BEAR" from w_objs to w_iobjs. */
            w_iobjs[iobx++] = w_objs[objx-1];
            w_objs[--objx] = NOTHING;
        }
    }

lin(99)
    wclass = word_class(word);
    switch (wclass) {
        case WordClass_Motion: goto lin100;
        case WordClass_Object: goto lin200;
        case WordClass_Action: goto lin300;
        case WordClass_Message: goto lin400;
        case WordClass_Preposition: goto lin500;
        case WordClass_Adjective: goto lin600;
        case WordClass_Conjunction: goto lin700;
        default: assert(false);  /* corresponds to BUG(33) in McDonald */
    }

lin(100)
    if (vrbx > 0) {
        int lastverb = w_verbs[vrbx-1];
        /* Long messes up Woods' original three motion-helpers
         * (CRAWL, JUMP, and CLIMB): "CRAWL NORTH" discards the
         * second verb and acts as if you typed just "CRAWL":
         * "Which way?".
         * I've taken the liberty of disentangling this logic
         * and restoring Woods' behavior for those three verbs. */
        if (word_class(lastverb) == WordClass_Motion) {
            /* Replace the old verb. */
        } else if (lastverb == TAKE && can_TAKE_direction(word)) {
            /* Replace the old verb. */
        } else {
            /* Otherwise, this parse doesn't make sense. */
            goto lin96;
        }
    }
    w_verbs[0] = word;  /* Long's line 180 */
    vrbx = 1;
    advise_about_going_west(txt[wdx-1]);
    goto lin90;

lin(200)
    /* Analyze an object. */
    if (pflag) goto lin503;
    if (vrbx == 0) {
        /* We haven't seen a verb yet. But maybe this noun could be
         * an action word too? For example, FEEFIE. */
        int k = lookup(txt[wdx-1], WordClass_Action);
        if (k != NOTHING) {
            word = k;
            goto lin300;
        }
    }
    if (word == ALL) {
        bool get_all;
        assert(vrbx >= 1);
        switch (w_verbs[vrbx-1]) {
            case DROP: case PUT: case LEAVE:
                get_all = false;
                break;
            case TAKE: case PICK: case GET:
                get_all = true;
                if (blind_at(loc)) {
                    /* Notice that TAKE ALL in the Crystal Palace gives
                     * a bogus message. TODO: fix this. */
                    puts("It's too dark to see anything!");
                    clrlin();
                    goto lin20;
                }
                break;
            default:
                goto lin800;
        }
        for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) {
            if (!at_hand(t, loc) || is_immobile(t)) continue;
            if (is_liquid(t)) continue;
            if (toting(t) == get_all) continue;
            w_objs[objx] = t;
            ++objx;
            if (objx == 44) break;  /* buffer overflow prevention */
        }
        goto lin90;
    }
    word = getobj(word, loc);
    if (word == NOTHING) {
        goto lin860;  /* we're done */
    } else if (word < 0) {
        /* It wasn't really an object. Go see what it was. */
        word = -word;
        goto lin99;
    }

    /* It was really an object, and it is here. */
    w_objs[objx] = word;
    strcpy(otxt[objx], txt[wdx-1]);
    ++objx;
    goto lin90;

lin(300)
    if (vrbx > 0) {
        /* We already have a verb! */
        if (w_verbs[vrbx-1] == TAKE) {
            switch (word) {
                case DRINK:
                case INVENTORY:
                case SCORE:
                case LOOK:
                case RELAX: /* "TAKE NOTHING" */
                    break;
                case GO:
                    if (is_take_a_hike_spelling(txt[wdx]))
                        break;
                    goto lin800;  /* "Huh?" */
                default:
                    goto lin800;  /* "Huh?" */
            }
            --vrbx;  /* replace the current verb */
        } else {
            /* Long's line 320.
             * Notice that "TAKE AND EAT FOOD" is rejected
             * by the switch above. TODO: maybe fix this. */
            if (words[wdx-2] == AND && objx == 0) {
                /* "GET AND EAT FOOD" */
            } else {
                goto lin800;
            }
        }
    }
    ++vrbx;
    assert(vrbx >= 1);
    w_verbs[vrbx-1] = word;
    strcpy(vtxt[vrbx-1], txt[wdx-1]);
    goto lin90;

lin(400)
    /* Miscellaneous words/verbs. */
    if (vrbx != 0) goto lin800;
    w_verbs[0] = word;
    vrbx = 1;
    goto lin90;

lin(500)
    /* Analyze a preposition and its object. */
    if (word_class(w_verbs[vrbx-1]) != WordClass_Action) goto lin800;
    if (iobx > 0) goto lin800;  /* we already got an iobj! */
    if (pflag) goto lin503;
    if (!is_valid_verb_prep_iobj(w_verbs[vrbx-1], ALL, ALL)) goto lin800;
    prep = word;
    pflag = true;
    /* Get the next word, which we expect to be an (indirect) object. */
    word = words[wdx++];
    switch (word_class(word)) {
        case WordClass_None: goto lin510;
        case WordClass_Motion: goto lin800;
        case WordClass_Object: goto lin503;
        case WordClass_Action: goto lin800;
        case WordClass_Message: goto lin800;
        case WordClass_Preposition: goto lin800;
        case WordClass_Adjective: goto lin600;
        case WordClass_Conjunction: goto lin510;
        case WordClass_BadWord: goto lin841;
        default: assert(false);
    }
lin(503)
    if (word != ALL) {
        if (word == BAD_WORD) goto lin570;
        word = getobj(word, loc);
        if (word == NOTHING) goto lin860;

        w_iobjs[iobx++] = word;
        strcpy(iotxt[iobx-1], txt[wdx-1]);
    }
lin(510)
    /* See if "verb ... prep word" is a valid combination. */
    assert(vrbx >= 1);
    int verb = w_verbs[vrbx-1];
    if (is_valid_verb_prep_iobj(verb, prep, ALL)) {
        /* prep is valid with this verb. Now check object of prep. */
        if (word == NOTHING || word == AND) goto lin530;
        if (is_valid_verb_prep_iobj(verb, prep, word)) {
            goto lin90;
        }
    }
    goto lin570;
lin(530)
    /* No object follows prep. Check special cases. */
    pflag = false;
    --wdx;
    {
    const char *dk = txt[wdx-1];  /* TODO: straighten this up */
    const char *dkk = vtxt[vrbx-1];
    if (!streq(dk, "on") && !streq(dk, "off") &&
        !(streq(dkk, "turn") && w_objs[objx-1] == LAMP) &&
        (!streq(dkk, "take") && !streq(dkk, "put"))) goto lin570;
    if (streq(dk, "up") && !streq(dkk, "pick")) goto lin570;
    if (streq(dk, "down") && !streq(dkk, "put") && !streq(dkk, "throw")) goto lin570;
    ++wdx;
    word = words[wdx-1];
    if (word == NOTHING) goto lin900;
    if (word == AND) goto lin91;
    goto lin800;
    }
lin(570)
    noway();
    clrlin();
    goto lin20;

lin(600)
    /* Handle an adjective. */
    {
    int adj = word;
    word = words[wdx++];  /* get the noun */
    if (word == BAD_WORD) goto lin841;
    if (word == NOTHING || word == AND) {
        txt[wdx-2][0] = toupper(txt[wdx-2][0]);
        printf("%s what?\n", txt[wdx-2]);  /* Brass what? */
        goto lin20;
    }
    if (word_class(word) != WordClass_Object) {
        /* Look up the word again as a noun. */
        words[wdx-1] = word = lookup(txt[wdx-1], WordClass_Object);
        if (word == NOTHING) goto lin800;
    }
    if (is_appropriate_adjective_for_noun(adj, word)) {
        goto lin92;
    } else {
        goto lin800;
    }
    }

lin(700)
    /* Analyze a conjunction (AND). */
    assert(word == AND);
    word = words[wdx++];
    assert(word != AND);
    if (word == BAD_WORD) goto lin841;
    if (word == NOTHING) goto lin800;
    switch (word_class(word)) {
        case WordClass_None: goto lin800;
        case WordClass_Motion: goto lin790;
        case WordClass_Object: goto lin92;
        case WordClass_Action: goto lin720;
        case WordClass_Message: goto lin790;
        case WordClass_Preposition: goto lin800;
        case WordClass_Adjective: goto lin92;
        default: assert(false);
    }
lin(720)
    /* A new action verb follows. If no previous verb has been
     * typed, or if the previous verb wasn't an action verb, fail.
     * If an obj/iobj was specified for previous verb, fail.
     * The only valid syntax is "GET AND OPEN CAGE". */
    if (vrbx != 0 && word_class(w_verbs[vrbx-1]) == WordClass_Action &&
            objx==0 && iobx==0)
        goto lin92;  /* Success! */
lin(790)
    --wdx;
    goto lin900;

lin(800)
    /* Gee, I don't understand. */
    confuz();
    clrlin();
    goto lin20;

lin(841)
    /* Upper-case the input word. */
    for (int i=0; txt[wdx-1][i] != '\0'; ++i)
        txt[wdx-1][i] = toupper(txt[wdx-1][i]);
    if (pct(25)) {
        printf("Mumble?  %s?\n", txt[wdx-1]);
    } else {
        printf("I don't understand the word %s.\n", txt[wdx-1]);
    }
    clrlin();
    goto lin20;

lin(860)
    clrlin();
    pflag = false;
    while (true) {
        if (words[wdx] == NOTHING) goto lin20;
        if (words[wdx++] == AND) goto lin90;
    }

lin(900)
    /* We appear to have reached the end of a sentence.
     * Either it was terminated by end-of-line, or else the conjunction
     * analyzer claims that the next words are not part of this clause.
     * Decide whether or not we have enough to work with. */
    pflag = false;
    if (w_verbs[0] == NOTHING) {
        if (w_objs[0] == NOTHING) goto lin800;
        if (w_objs[1] == NOTHING) {
            printf("What do you want to do with the %s?\n", otxt[0]);
        } else {
            puts("What do you want to do with them?");
        }
        /* Even though Long prints this message, his parser can't actually
         * deal with a single-verb response e.g. "GET". Upon parsing the
         * action-word GET, we call clrlin (Long's line 32) and thus forget
         * about the original object LAMP.
         * TODO: attempt to fix this bug. */
        goto lin20;
    }
    if (objx >= 2 && iobx >= 2) {
        /* GIVE LAMP AND KEYS TO TROLL AND DWARF */
        goto lin800;
    }

    /* Otherwise we successfully navigated the whole clause! */
    return;
}

static int getobj(ObjectWord t, Location loc)
{
    assert(word_class(t) == WordClass_Object);
    if (holding(t)) return t;
    if (blind_at(loc)) return I_see_no(txt[wdx-1]);
    if (is_at_loc(t, loc) || at_hand(t, loc)) return t;
    if (here(t, loc)) {
        /* It's here, but not at_hand; therefore it must be inside
         * a closed transparent container. */
        if (is_plural(t)) {
            puts("You can't get at them.");
        } else {
            puts("You can't get at it.");
        }
        return 0;
    }
    /* Okay, it's definitely not here.
     * Handle some special cases. */
    if (t == GRATE) {
        /* These locations are pretty ad-hoc, but they match McDonald's code. */
        if (loc == R_ROAD || loc == R_VALLEY || loc == R_SLIT) return -DEPRESSION;
        if (R_OUTSIDE <= loc && loc <= R_EMIST) return -ENTRANCE;
        return I_see_no(txt[wdx-1]);
    } else if (t == DWARF) {
        if (dwarf_at(loc)) return t;
        return I_see_no(txt[wdx-1]);
    } else if (t == liquid_at_location(loc) ||
               (at_hand(BOTTLE, loc) && t == liquid_contents(BOTTLE)) ||
               (at_hand(CASK, loc) && t == liquid_contents(CASK))) {
        return t;
    } else if (t == PLANT && here(PLANT2, loc) && objs(PLANT).prop != 0) {
        return PLANT2;
    } else if (t == CLOAKROOM_ROCKS && here(CARVING, loc)) {
        return CARVING;
    } else if (t == ROD && at_hand(ROD2, loc)) {
        return ROD2;
    } else if (t == RUSTY_DOOR) {
        if (is_at_loc(TINY_DOOR, loc)) return TINY_DOOR;
        if (is_at_loc(HUGE_DOOR, loc)) return HUGE_DOOR;
        if (is_at_loc(BOOTH_DOOR, loc)) return BOOTH_DOOR;
        if (there(SAFE, loc)) return SAFE;
    } else if (t == BOOK && at_hand(REPO_BOOK, loc)) {
        return REPO_BOOK;
    }

    /* FIND and INVENTORY are also special; e.g. FIND GOLD. */
    if (w_verbs[vrbx] == FIND || w_verbs[vrbx] == INVENTORY) {
        return t;
    }

    /* Otherwise, it's not here. */
    return I_see_no(txt[wdx-1]);
}

static bool is_appropriate_adjective_for_noun(AdjectiveWord adj, ObjectWord noun)
{
    /* Adjectives are basically ignored, for purposes of disambiguation.
     * For example, you can type "OPEN TINY DOOR" and the game will assume
     * that you mean RUSTY_DOOR, if that's the only door here.
     * But DOOR is the only noun that could theoretically be disambiguated
     * by the choice of adjective, and no two doors can be present at once. */
    assert(word_class(adj) == WordClass_Adjective);
    assert(word_class(noun) == WordClass_Object);
    switch (adj) {
        case BLACK: return (noun == ROD || noun == DOG || noun == OIL);
        case BRASS: return (noun == LAMP || noun == TINY_KEY);
        case ELFIN: return (noun == CROWN || noun == RUSTY_DOOR || noun == TINY_KEY);
        case FRESH: return (noun == BATTERIES);
        case ADJ_GIANT: return (noun == RUSTY_DOOR || noun == CLAM || noun == OYSTER ||
                                noun == GOLD || noun == SNAKE);
        case GOLDEN: return (noun == GOLD || noun == CHAIN || noun == EGGS ||
                             noun == RING || noun == TREE);
        case TASTY: return (noun == FOOD);
        case GREEN: return (noun == RADIUM || noun == EMERALD || noun == CLOVER);
        case GREY: return (noun == CANISTER);
        case GLOWING: return (noun == RADIUM);
        case HOLY: return (noun == GRAIL);
        case IRON: return (noun == RUSTY_DOOR);
        case LEADEN: return (noun == SLUGS || noun == CANISTER);
        case METAL: return (noun == CANISTER);
        case MING: return (noun == VASE);
        case OAKEN: return (noun == CASK);
        case QUARTZ: return (noun == BALL);
        case LEATHER: return (noun == BOOK || noun == SACK);
        case CRYSTAL: return (noun == BALL);
        case FOURLEAF: return (noun == CLOVER);
        case PERSIAN: return (noun == RUG);
        case PLATINUM: return (noun == PYRAMID);
        case RARE: return (noun == BOOK || noun == COINS || noun == SPICES);
        case RUBY: return (noun == SHOES);
        case RUSTY: return (noun == RUSTY_DOOR);
        case SHADOWY: return (noun == SHADOW);
        case SILKEN: return (noun == CLOAK);
        case SILVER: return (noun == HORN || noun == DROPLET);
        case STAR: return (noun == SAPPHIRE);
        case STEEL: return (noun == SWORD || noun == SAFE || noun == GRATE);
        case TINY: return (noun == RUSTY_DOOR || noun == RING || noun == TINY_KEY ||
                           noun == BOTTLE || noun == BIRD || noun == CAKES ||
                           noun == CANISTER);
        case TREASURE: return (noun == CHEST);
        case VELVET: return (noun == PILLOW);
        case VENDING: return (noun == MACHINE);
        case WICKER: return (noun == CAGE);
        case WOODEN: return (noun == BOAT || noun == POLE || noun == CASK);
        case WORNOUT: return (noun == BATTERIES);
        default: assert(false);
    }
}

static bool is_valid_verb_prep_iobj(int verb, int prep, ObjectWord iobj)
{
    static int ptab[] = {
        BREAK, WITH, AXE,SWORD,POLE,ROD,
        DRINK, FROM, CASK,GRAIL,VASE,BOTTLE,
        DROP, INTO, CHEST,SACK,MACHINE,BOTTLE,CASK,GRAIL,VASE,CAGE,
                    BOAT,ANVIL,PHONE,SAFE,LAMP,CANISTER,
        DROP, ONTO, RUG,CROWN,CLOAK,SHOES,JEWELS,SWORD,PILLOW,
        SWEEP, WITH, BROOM,
        FEED, TO, TROLL,SNAKE,BIRD,BEAR,WUMPUS,DOG,DWARF,BATS,
                  CLAM,OYSTER,BEES,
        FILL, WITH, OIL,WINE,WATER,
        GET, FROM, CHEST,SACK,BOTTLE,CASK,GRAIL,VASE,CAGE,BOAT,
                   ANVIL,SAFE,WUMPUS,CANISTER,
        GET, INTO, CHEST,SACK,BOTTLE,CASK,GRAIL,VASE,CAGE,BOAT,
                   ANVIL,SAFE,WUMPUS,CANISTER,
        HIT, WITH, AXE,SWORD,POLE,ROD,KNIFE,
        INSERT, INTO, CHEST,SACK,MACHINE,BOTTLE,CASK,GRAIL,VASE,CAGE,
                      BOAT,ANVIL,PHONE,SAFE,LAMP,CANISTER,
        KILL, WITH, AXE,SWORD,KNIFE,
        LEAVE, INTO, CHEST,SACK,MACHINE,BOTTLE,CASK,GRAIL,VASE,
                     CAGE,BOAT,ANVIL,PHONE,SAFE,CANISTER,
        LEAVE, ONTO, RUG,CROWN,CLOAK,SHOES,JEWELS,SWORD,PILLOW,
        LOCK, WITH, KEYS,TINY_KEY,
        LOOK, INTO, CHEST,SACK,BOTTLE,CASK,GRAIL,VASE,CAGE,BOAT,
                    ANVIL,CLAM,OYSTER,SAFE,CANISTER,BALL,
        LOOK, AT, ALL,
        OPEN, WITH, TINY_KEY,KEYS,AXE,TRIDENT,SWORD,POLE,ROD,
        PICK, PREP_UP, ALL,
        PLAY, ONTO, HORN,LYRE,
        POUR, ONTO, PLANT,SWORD,ANVIL,RUSTY_DOOR,CLOVER,
        POUR, FROM, CASK,GRAIL,VASE,BOTTLE,
        PUT, PREP_DOWN, ALL,
        PUT, ONTO, RUG,CROWN,CLOAK,SHOES,JEWELS,SWORD,PILLOW,CLOVER,
        PUT, INTO, CHEST,SACK,MACHINE,BOTTLE,CASK,GRAIL,VASE,CAGE,
                   BOAT,ANVIL,PHONE,SAFE,LAMP,CANISTER,
        READ, ONTO, CLAM,OYSTER,TABLET,BILLBOARD,
        READ, FROM, CLAM,OYSTER,TABLET,MAG,POSTER,BILLBOARD,
        REMOVE, FROM, CHEST,SACK,BOTTLE,CASK,GRAIL,VASE,CAGE,BOAT,
                      ANVIL,CLAM,OYSTER,SAFE,WUMPUS,CANISTER,
        CLOSE, WITH, KEYS,TINY_KEY,
        TAKE, OFF, CROWN,CLOAK,SHOES,JEWELS,RING,CLOVER,
        TAKE, FROM, CHEST,SACK,BOTTLE,CASK,GRAIL,CAGE,BOAT,
                    /* McDonald omits "VASE" here but has "CAGE" twice. */
                    ANVIL,SAFE,WUMPUS,CANISTER,VASE,
        TAKE, INTO, CHEST,SACK,BOTTLE,CASK,GRAIL,VASE,CAGE,BOAT,
                    ANVIL,CANISTER,
        TOSS, TO, TROLL,SNAKE,BIRD,BEAR,WUMPUS,DOG,DWARF,BATS,
                   SHADOW,BEES,
        TOSS, AT, ALL,
        TOSS, PREP_DOWN, ALL,
        TURN, ONTO, LAMP,
        TURN, OFF, LAMP,
        UNLOCK, WITH, KEYS,TINY_KEY,
        WAVE, AT, ALL,
        WAVE, TO, TROLL,SNAKE,BIRD,BEAR,WUMPUS,BEES,DOG,DWARF,
                  BATS,SHADOW,
        YANK, FROM, ALL,
        NOTHING
    };
    int current_verb = NOTHING;
    int current_prep = NOTHING;

    for (int i=0; ptab[i] != NOTHING; ++i) {
        switch (word_class(ptab[i])) {
            case WordClass_Action:
                current_verb = ptab[i];
                assert(word_class(ptab[i+1]) == WordClass_Preposition);
                if (verb == current_verb && prep == ALL) return true;
                break;
            case WordClass_Preposition:
                assert(current_verb != NOTHING);
                current_prep = ptab[i];
                assert(word_class(ptab[i+1]) == WordClass_Object);
                break;
            case WordClass_Object:
                assert(current_verb != NOTHING);
                assert(current_prep != NOTHING);
                if (verb == current_verb && prep == current_prep) {
                    if (ptab[i] == ALL) return true; /* works for any object */
                    if (iobj == ALL) return true; /* caller didn't specify one yet */
                    if (ptab[i] == (int)iobj) return true;
                }
                break;
            default:
                assert(false);
        }
    }
    return false;
}

void shift_words(int *verb, int *obj, int *iobj, Location loc)
{
    /* Update verb, obj, iobj.
     * We will never have 2+ objs AND 2+ iobjs.
     * But we may have 2+ verbs and 2+ (i)objs.
     * In that case, we want to iterate over the (i)objs using the first
     * verb, and then again using the second verb, etc. That is, GET AND
     * DROP LAMP AND KEYS is (except for the printing of the object names)
     * equivalent to GET LAMP; GET KEYS; DROP LAMP; DROP KEYS.
     */
    extern int turns;
    if (turns != 0) {
        /* After the first turn, we should always be coming in here
         * from a valid previous state. */
        assert(w_verbs[0] != NOTHING);
    }
    const bool multiple_objs = (w_objs[1] != NOTHING);
    const bool multiple_iobjs = (w_iobjs[1] != NOTHING);
    assert(!(multiple_objs && multiple_iobjs));
    const bool goto_next_verb = (!multiple_iobjs && w_objs[objx+1] == NOTHING)
                             || (!multiple_objs && w_iobjs[objx+1] == NOTHING);
    if (goto_next_verb) {
        ++vrbx;
        objx = 0;
        iobx = 0;
    } else {
        assert(multiple_objs || multiple_iobjs);
        if (multiple_objs) ++objx; else ++iobx;
    }
    if (w_verbs[vrbx] == NOTHING) {
        assert(vrbx > 0);
        clrlin();
        getwds(loc);
        vrbx = objx = iobx = 0;
    }
    *verb = w_verbs[vrbx];
    *obj = w_objs[objx];
    *iobj = w_iobjs[iobx];
    assert(*verb != NOTHING);
}


extern int w_verbs[45];
extern int w_objs[45];
extern int w_iobjs[15];
extern int prep;

extern char vtxt[45][16];
extern char otxt[45][16];
extern char iotxt[15][16];

extern int vrbx, objx, iobx;  /* which word we're processing right now */

extern bool indent_default_responses;

/* This function is called from the main loop.
 * Its job is to parse out the next clause of the player's input
 * into the globals "w_verbs", "w_objs", "w_iobjs", and "prep";
 * and then update wdx to point to the first word of the next
 * clause. Nobody above this level should be accessing "txt" or
 * "wdx" directly. */
void getwds(Location loc);

/* This function is called from the main loop.
 * Its job is to manage "vrbx", "objx", and "iobx". Nobody
 * above this level should even be accessing "w_verbs" directly.
 * However, we occasionally do need to access "vtxt" or "otxt". */
void shift_words(int *verb, int *obj, int *iobj, Location loc);

/* Clear the whole rest of the input line. */
void clrlin(void);

/* Read a line, return TRUE if it's "yes", and return FALSE if
 * it's anything else. In that case we set words[0] to REPARSE_ME
 * and expect getwds() to do its duty. */
bool with_your_bare_hands(void);

/* adv00.c: A-code kernel - copyleft Mike Arnautov 1990-2010.
 */
#define KERNEL_VERSION "12.32, 12 Nov 2010"
/*
 * 12 Nov 10   MLA        bug: Use width, not max-width for HTML td elements.
 * 02 Nov 10   MLA        BUG: Fixed text buffer trimming in outbuf().
 *                        Bug: Store constrained conf values.
 * 29 Jul 10   MLA        Wait for *genuine* initial load request!
 * 30 Apr 10   MLA        Improved HTML display characteristics.
 * 12 Apr 10   MLA        Bug: If creating the conf file, read it too!
 *                        Also, deamonize if running via a browser.
 * 01 Apr 10   MLA        Bug: Issue prompt after null command if no readline.
 * 21 Mar 10   MLA        Bug: Use arg<n>_word for echoing nested ARG<n>.
 * 19 Mar 10   MLA        BUG: Initialise type in find_word.
 *                        Ignore SIGCLD without creating zombies.
 * 25 Feb 10   MLA        Always use non-HTML format for the log.
 * 19 Feb 10   MLA        Added strong compression in all modes.
 * 15 Feb 10   MLA        Rewrote output routines.
 * 04 Feb 10   MLA        Added SHOW config file section.
 * 30 Jan 10   MLA        Replaced proxy mode with direct browser code.
 * 29 Jan 10   MLA        Added supporting code for IFHTML.
 * 20 Jan 10   MLA        Merged browser interface into main program.
 * 17 Jan 10   MLA        BUG: Don't check char preceding start of text_buf!
 * 25 Aug 09   MLA        Don't count 'B' mode as being a cgi mode.
 * 31 Jul 09   MLA        Create_db() now exits after constructing the database.
 * 21 Jul 09   MLA        Ditched data_file, now using try_db() and DBNAME.
 * 20 Jul 09   MLA        Added HAVE_CONFIG_H check for GNU automake tools.
 * 15 Jul 09   MLA        bug: Fixed gcc -Wall --pedantic warnings.
 * 13 Jul 09   MLA        Bug: Fixed .adl file handling.
 *                        Allowed browser mode in all styles!
 * 31 Mar 09   MLA        Added last output auto-restore in browser mode.
 * 22 Mar 09   MLA        Added auto-restore in browser mode.
 * 07 Mar 09   MLA        Added auto-save in browser mode.
 * 16 Feb 09   MLA        Added browser mode.
 * 29 Jan 09   MLA        Bug: Don't use orig1/orig2 for style < 11.
 *                        bug: Don't define typed() for style < 11.
 * 12 Nov 08   MLA        Added QT mode.
 * 22 Oct 08   MLA        Bug: Fixed fakecom word retrieval in fake().
 * 02 Oct 08   MLA        Bug: Always check for FSPECIAL and LSPECIAL being 
 *                        defined!
 * 21 Sep 08   MLA        Randomised texts should not repeat the same value.
 *                        Also, cyclic text switches now cycle independently.
 * 23 May 08   MLA        Improved handling of AGAIN.
 *                        Added typed().
 * 22 May 08   MLA        Postpone word swap for better parsing.
 * 21 May 08   MLA        Bug: restore orphan refno in CGI mode!
 * 18 May 08   MLA        BUG: adjust backup image when UNDOing!!
 *                        BUG: save undo changes within multiple commands too!
 * 30 Apr 08   MLA        Added PROMPT.
 * 12 Apr 08   MLA        Pass game name in AJAXTEST call to play.
 * 10 Apr 08   MLA        AJAXTEST added for browser interface.
 * 09 Apr 08   MLA        BUG: Fixed detail display.
 * 06 Apr 08   R.Mathews  bug: add MacOS to the list of OS with the '/' 
 *                        separator.
 * 03 Apr 08   MLA        Bug: outline() needs the terminate arg if using
 *                        readline.
 * 22 Mar 08   MLA        BADWORD etc now go down from -2.
 * 20 Mar 08   MLA        BUG: Fixed IGNORE_EOL handling.
 *                        Bug: PARA_END only defined if ADVCONTEXT is.
 *                        Bug: Fixed "nbsp" bug in non-CGI modes.
 * 11 Mar 08   MLA        Version 12 (to work with the two-pass acdc).
 * 09 Jan 08   MLA        Added h/H cgi state.
 * 22 Nov 07   MLA        Bomb if cgi-mode context dump file not found.
 * 10 Nov 07   MLA        Added interactive data dump.
 * 01 Nov 07   MLA        Finess the AND separator if amatching suppressed.
 * 14 May 07   MLA        Split off M$ specific code to adv01.c.
 * 04 May 07   MLA        Added MS-specific code for usleep and process_saved.
 * 03 May 07   S.Munro    Define unlink as _unlink for WIN32.
 *                        bug: Initialise len when saving history.
 * 01 May 07   S.Munro    Fixed an #ifdef which should have been just #if.
 *             MLA        Corrected NOSLOW to be NO_SLOW.
 * 08 Apr 07   MLA        Added -o for output speed in cps (chars per second).
 *                        Allowed for different exec values in adv550.
 * 15 Oct 06   MLA        Reinstated HTML tag handling. 
 *                        Also added PROMPTED symbol processing.
 * 30 Jul 05   MLA        Cleaned up compiler warnings.
 * 25 May 05   MLA        BUG: Don't gripe for flags in excess of 15!
 * 17 Apr 05   MLA        BUG: Dump readline buffer on new line.
 * 09 Apr 05   MLA        BUG: allow for voc growth in restoring varbits.
 * 26 Mar 05   MLA        BUG: Fixed UNDO/REDO for the CGI mode.
 * 13 Mar 05   MLA        BUG: Restore computed image length, not real one!
 * 12 Mar 05   MLA        Bug: time_t is long, not int!
 * 20 Feb 05   MLA        Bug: Don't strip off dots if no matching!
 * 18 Jan 05   MLA        Allowed for nested vars and others.
 * 09 Jan 05   MLA        Command line and symbol handling of UNDO status.
 * 03 Jan 05   MLA        Added undo history keeping.
 * 31 Dec 04   MLA        Bug: cgi only defined if ADVCONTEXT is defined!
 * 28 Dec 04   MLA        Assembled state arrays into one array.
 * 22 Dec 04   MLA        bug: PRINTF2 has to be braced -- it's compound!
 * 05 Dec 04   MLA        Bug: Verbatim glitch.
 * 19 Aug 04   MLA        Replaced special(31) with verbatim().
 * 08 Aug 04   MLA        bug: Force logfile in CGI mode.
 * 07 Aug 04   MLA        Bug: Remember game name on initial restore in
 *                        the cgi mode.
 * 31 Jul 04   MLA        bug: Don't abort compound commands after "except".
 * 09 Apr 04   MLA        bug: Don't print kernel version twice if GLK.
 * 14 Mar 04   MLA        BUG: Or for (internal) "-z" ones. And don't leave
 *                        debug statements lying around!!!
 * 28 Feb 04   MLA        BUG: Don't use memstore for CGI -y restores!
 * 16 Feb 04   MLA        G++ didn't like initialisation of fname in memstore().
 *                        Bug: Initialise rseed before calling p1()!
 * 12 Feb 04   MLA        Reused exec 12.
 * 06 Feb 04   MLA        Flag pseudo-objects on entry.
 * 07 Jan 04   MLA        Split memstore() off special(). Reused exec 28.
 *                        Added exec 33.
 * 06 Jan 04   MLA        Exec 30 reused to strip off trailing LF.
 * 02 Jan 04   MLA        Exec 9/10 now also fake the arg word.
 * 30 Dec 03   MLA        Bug: Fixed termination of ALL loops.
 * 09 Dec 03   MLA        Bug: Fixed BLOCK_END handling in outbuf().
 * 16 Nov 03   MLA        Bug: check for bad syntax before skipping rest
 *                        of command!
 * 11 Oct 03   MLA        Allow for TYPO not being declared in style >= 11.
 * 23 Sep 03   MLA        BUG: don't zap tokens in find_word, if no matching!
 *                        bug: don't convert _ into blank on input.
 * 20 Sep 03   MLA        BUG: readline does not append '\n'!!
 * 18 Sep 03   MLA        Bug: don't call ferror() on a closed file handle!
 * 29 Jun 03   MLA        Eliminated redundant restore mode 966.
 * 27 Jun 03   MLA        BUG: Use separate memory for checking restore!
 * 22 Jun 03   MLA        BUG: Make sure command is lowercased!!!
 * 16 Jun 03   MLA        Bug: Fixed variant bit testing in voc().
 * 31 May 03   MLA        More tolerance for shorter (older) save-files.
 * 26 May 03   MLA        Parser re-write: THEN is now equivalent to semicolon.
 *                        Also, let the game do pre-saying.
 * 10 May 03   MLA        BUG: Fixed "drop treasure and <something>".
 * 30 Apr 03   MLA        Bug: Strip off '\r' from MS line terminator.
 * 08 Apr 03   MLA        bug: Initialise to zero values beyond end of
 *                        values array in a restored game.
 * 01 Apr 03   MLA        BUG: fixed restore of incompatible versions.
 *                        Also, improved version checking.
 *                        Also changed -l to -log iff GLK.
 * 24 Mar 03   MLA        BUG: no SCHIZOID behaviour for INHAND in isat().
 * 22 Mar 03   MLA        BUG: run p1() when restoring from command line!
 * 20 Mar 03   MLA        Intelligent version checks on restore,
 * 09 Mar 03   S. Munro   bug: Remove some redundant declarations.
 *                        Work around an MS oddity in using CHKSUM.
 *             MLA        Default to SWAP.
 * 08 Mar 03   S. Munro   Bug: Fix several non-ANSI declaration.
 * 04 Mar 03   MLA        Trap bad command-line restores.
 *                        Re-do the handling of VERSION as GAMEID and DBNAME.
 * 24 Feb 03   MLA        Bug: Fixed create_db sign handling.
 *                        bug: Suppressed unnecessary "More?" prompts.
 * 23 Feb 03   MLA        Bug: Fixed CGI version handling.
 * 21 Feb 03   MLA        Rationalised use of database use or non-use.
 * 20 Feb 03   MLA        Merged database into executable.
 * 16 Feb 03   MLA        bug: several small command handling bugs.
 * 14 Feb 03   MLA        Bug: Fixed '-l -c name' handling in command args.
 * 09 Feb 03   MLA        Use F/LFULL and F/LDIR. Also use SAY.
 * 04 Feb 03   MLA        Bug: fix qualifying by ARG2. 
 *                        Also hacked in EXCEPT code.
 * 31 Jan 03   MLA        Virgin mode entirely file based -- no rapes! :-)
 * 26 Jan 03   MLA        bug: avoid looping in INITs even if code QUITs there.
 * 24 Jan 03   MLA        BUG: BADWORD etc do *not* suppress amatch!
 * 06 Jan 03   MLA        BUG: Fixed varbits[] handling!
 * 04 Jan 03   MLA        Bug: fixed the typo reporting code.
 *                        Also added case 31 of special().
 * 02 Jan 03   MLA        bug: removed redundant fprintf arg.
 * 30 Dec 02   MLA        bug: Use arg2 resolution on iterated objects.
 *                        Also added vocabulary search suppression.
 * 22 Dec 02   MLA        Suppress 'More?" if replaying from log.
 *                        Also preserve rseed sequence outside the A-code.
 * 21 Dec 02   MLA        Added isat().
 * 20 Dec 02   MLA        Bug: Allow for ARG1/ARG2 not having bits set to -1!
 * 12 Nov 02   MLA        Added "exec 30" to check for typos.
 * 28 Sep 02   MLA        Undo the fix suppressing end-of-buffer blanks
 *                        if buffer ends on a block end (why was it there???).
 * 13 Jul 02   MLA        Complete re-write of set().
 * 12 Jul 02   MLA        Don't use +1024 flag in have, ishere and isnear.
 * 10 Jul 02   MLA        kludge: Ignore description words ending in 'ing'.
 *                        kludge: Guess whether to capinitial # words.
 * 07 Jul 02   MLA        BUG: Fail memory restore if no memory image!
 * 23 Jun 02   MLA        Added jrand for "true" randomisation.
 * 11 May 02   MLA        Translate matched vocabulary words into Dwarvish too!
 * 14 Apr 02   MLA        Allow comma as command separator.
 *                        Disambiguate abbreviations.
 * 12 Feb 02   MLA        Added 'first' check in parse ().
 * 11 Jan 02   MLA        Added ARG1/ARG2 swap.
 * 05 Jan 02   MLA        Bug: Don't set JUGGLED unless old or new location
 *                        is INHAND!
 * 04 Jan 02   D.Picton   Bug: Append '\n' to GLK-obtained commands!
 * 18 Dec 01   MLA        Bug: don't use cgi outside #ifdef CGI!
 * 11 Dec 01   D.Kinder   Fixed agrc (sic.) etc typos.
 * 09 Dec 01   MLA        Allowed increasing text counts without
 *                        invalidating old saved games or relaxing checks.
 * 04 Dec 01   MLA        Save old_comline in CGI mode.
 * 27 Nov 01   MLA        Rationalised command line keyword handling. 
 * 25 Nov 01   MLA        Bug: fixed short/long/detail description juggling.
 *                        Added IGNORE_EOL.
 * 19 Nov 01   MLA        Fixed the "More?" query in new output framework.
 * 17 Nov 01   MLA        Simplified in-memory plaintext operation.
 * 10 Nov 01   MLA        New output framework.
 * 07 Nov 01   D.Kinder   Tweaked to work with Visual C++.
 * 06 Nov 01   MLA        Adjusted handling of FULL, BRIEF and
 *                        TERSE for STYLE 1 compatibility.
 * 04 Nov 01   MLA        bug: Use of PLSCLARIFY must be conditional.
 * 03 Nov 01   MLA        Compress multiple LFs into at most two.
 * 02 Nov 01   MLA        Added BADSYNTAX handling;
 * 30 Oct 01   MLA        BUG: Even in STYLE == 1, implicit switches start 
 *                        from 0!
 * 20 Oct 01   MLA        BUG: Don't use AMBIGWORD when STYLE == 1 !
 * 18 Oct 01   MLA        Improved back compatibility with Platt's A-code.
 * 07 Oct 01   MLA        Bug: Fixed orphan handling (abandoning attempts
 *                        to make orphans work across "repeat").
 *                        bug: Wrap on hyphenation too!
 * 05 Aug 01   MLA        Bug: Defaulting sets 'it'.
 * 28 Jul 01   MLA        Bug: Remeber to copy typo stuff for continuations.
 * 24 Jul 01   MLA        BUG: Fixed find_word declaration in compatibility
 *                        mode.
 * 14 Jul 01   MLA        Bug: Apply dwarvish translation to separate command,
 *                        not to the whole command line!
 * 20 May 01   MLA        Bug: fixed new parser. Add scene text only if
 *                        no error.
 * 19 May 01   MLA        bug: Re-wrote parse() due to minor problems.
 *                        Bug: Check entity has flags before accesing a flag.
 *                        BUG: Fake checking the VERB flag.
 * 17 May 01   MLA        Added fake() for FAKECOM.
 * 16 May 01   MLA        Bug: HTML-quote angled brackets!
 * 12 May 01   MLA        Bug: conditionally switch off amatching for ARG2.
 * 06 May 01   MLA        bug: Fixed approximate matching.
 * 02 May 01   MLA        Bug: Fixed yet another "null command" bug. :-(
 * 16 Apr 01   MLA        Fixed zapping save file on initial restore.
 * 30 Mar 01   MLA        Added typo reporting
 * 25 Mar 01   MLA        Added scenery word matching.
 * 15 Mar 01   MLA        Added readline.
 * 14 Mar 01   MLA        Added approximate matching.
 * 28 Feb 01   MLA        Allowed assigned text type. Added in-text
 *                        dwarvish tag. Added tied text type.
 * 26 Feb 01   MLA        Adjust hlead if not justifying.
 * 25 Feb 01   MLA        Added DWARVEN game/kernel integration.
 * 24 Feb 01   MLA        Added special for checking "do all" status.
 *                        Remapped quotes and underscores to blanks on input.
 * 20 Feb 01   MLA        bug: Fudged around the problem of blocks in switches.
 * 09 Feb 01   MLA        Yet better ANSI compliance!
 * 27 Jan 01   MLA        Replaced decrementing texts with cycling ones.
 * 21 Jan 01   MLA        Bug: if CGI, must use disk for memory save!
 * 12 Jan 01   MLA        Added random and inc/decrementing texts.
 *                        Allowed setting value of texts.
 * 07 Jan 01   MLA        Added CENTRE_START.
 * 05 Jan 01   MLA        Adapted to cgi-bin use.
 * 08 Dec 00   MLA        Save/restore player's verbatim input too!
 * 05 Dec 00   MLA        If special context, accept null input.
 * 21 Oct 00   MLA        Default creen size & margin can be specified at
 *                        compile time. Also changed command line format
 *                        to N[xN[-N]].
 * 30 Sep 00   MLA        Allow for no output between two prompts.
 *                        Added -b command line option. On Unix/Windows
 *                        dbs first sought in the executable's directory.
 * 18 Sep 00   MLA        Added screen length. Added command line parsing.
 * 17 Sep 00   MLA        Fixed justifying code. De-wired screen width
 *                        and offset.
 * 17 Dec 99   MLA        Added block handling.
 * 16 Sep 99   Jon Skeet  HERE and THERE need initialising.
 * 12 Dec 99   MLA        Ignore embedded HTML tags. Allow for "hard" spaces
 * 13 Oct 99   MLA        Allowed for Cygwin's strange sign promotion.
 * 30 Aug 99   MLA        Allowed text nesting.
 * 09 Aug 99   MLA        Don't give strcmp a NULL for an arg!
 *                        And don't search vocabulary for null words!
 * 06 Aug 99   MLA        Merged with chopped down advcust.c
 * 05 Aug 99   MLA        Don't close log file twice! (Linux don't like it.)
 *                        Also, added dwarven mode.
 * 29 Jul 99   MLA        Added "to exit press enter" for MS O/S.
 * 24 Jul 99   MLA        Eliminated spurious warnings.
 * 10 Apr 99   MLA        Strengthened encryption, added virgin mode,
 *                        checksummed dumpfiles.
 * 05 Apr 99   MLA        Added voc().
 * 01 Apr 94   MLA        Added dynamic log file handling.
 * 26 Mar 94   MLA        Fixed move() a2 handling.
 * 14 Mar 94   MLA        MOVED, JUGGLED and SCHIZOID now truly optional.
 * 16 Feb 94   MLA        Merged 10.04 with the experimental 11.00,
 *                        creating 10.05.
 * 12 Dec 91   MLA        The ? prompt overridden by pending text.
 *                        input() takes an optional argument.
 * 10 Mar 91   MLA        Added flush_command ().
 * 09 Mar 91   MLA        Stopped SCHIZOID confusion in INHAND.
 * 06 Mar 91   MLA        Allowed extra minute on checking restore lag.
 *                        Also, do own lowercasing to avoid trouble.
 * 26 Feb 91   MLA        Use flagged return instead of exit.
 * 23 Feb 91   MLA        Allowed variable bit fields.
 * 22 Feb 91   MLA        Corrected parser response to wrong syntax. 
 * 29 Dec 90   MLA        Reduced disk accesses via "locate buffers".
 * 27 Dec 90   MLA        Allowed flags and states with IFHERE/HAVE/NEAR.
 * 14 Dec 90   MLA        Allowed null keyword lists for (S)MOVE.
 * 12 Dec 90   MLA        New minor directive QUIP.
 * 11 Dec 90   MLA        Variable keyword counts for MOVE and SMOVE.
 * 25 Nov 90   MLA        Outline now checks for zero break_count.
 * 23 Nov 90   MLA        VMS compilation. Also, "unvoided" declarations
 *                        of advcpy and srand - Prime's CI -DEBUG didn't
 *                        like them being cast to voids subsequently.
 * 22 Nov 90   MLA        Changed 'here' and 'near' to 'ishere' and
 *                        'isnear' to avoid MSC clashes.
 * 17 Nov 90   MLA        Introduced NOVARARGS compilation symbol
 *                        to cope with a nasty Ultrix compiler.
 * 15 Sep 90   MLA        Initial coding.
 *
 */

/* Remember that adv1.h and all the symbol fiddling must precede adv0.h! */

#include "adv1.h"

#ifdef USEDB
#  undef USEDB
#endif /* USEDB */

#ifdef PRELOADED
#  undef PRELOADED
#  if defined(MEMORY)
#     undef MEMORY
#  endif
#  if defined(SWAP)
#     undef SWAP
#  endif
#  if defined(FILE)
#     undef FILE
#  endif
#  if defined(DBSTATUS)
#     undef DBSTATUS
#  endif
#  define DBSTATUS 0
#endif /* PRELOADED */

#ifdef MEMORY
#  undef MEMORY
#  if defined(SWAP)
#     undef SWAP
#  endif
#  if defined(FILE)
#     undef FILE
#  endif
#  if defined(DBSTATUS)
#     undef DBSTATUS
#  endif
#  define DBSTATUS 1
#endif /* MEMORY */

#if defined(SWAP)
#  if !(SWAP >= 16 && SWAP <= 128)
#     undef SWAP
#  endif
#  if defined(FILE)
#     undef FILE
#  endif
#  if defined(DBSTATUS)
#     undef DBSTATUS
#  endif
#  define DBSTATUS 2
#endif /* SWAP */

#ifdef FILE
#  undef FILE
#  if defined(DBSTATUS)
#     undef DBSTATUS
#  endif
#  define DBSTATUS 3
#endif /* FILE */

#ifdef DBSTATUS
#  if DBSTATUS == 0
#     define PRELOADED
#  endif
#  if DBSTATUS == 1
#     define MEMORY
#  endif
#  if DBSTATUS == 2 && !defined(SWAP)
#     define SWAP 32
#  endif
#  if DBSTATUS == 3
#     define READFILE
#  endif
#endif /* DBSTATUS */

#ifdef SWAP
#  if SWAP < 16
#     undef SWAP
#     define SWAP 16
#  endif /* SWAP < 16 */
#  if SWAP > 128
#     undef SWAP
#     define SWAP 128
#  endif /* SWAP > 128 */
#endif /* SWAP */

#ifndef PRELOADED
#  define USEDB
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#  define PLATFORM "LINUX"
#else
#  if defined(__MACH__)
#     define PLATFORM "MACOS"
#  else
#     define PLATFORM "UNKNOWN"
#  endif
#endif

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#if defined(HAVE_CONFIG_H)
#  include "config.h"
#  if defined (HAVE_STRING_H)
#     include <string.h>
#  else /* ! HAVE_STRING_H */
#     if defined(HAVE_STRINGS_H)
#        include <strings.h>
#     endif /* HAVE_STRINGS_H */
#  endif /* HAVE_STRING_H */
#  if defined(HAVE_STDLIB_H)
#     include <stdlib.h>
#  endif /* HAVE_STDLIB_H */
#  if defined (HAVE_SYS_STAT_H)
#     include <sys/stat.h>
#  endif /* HAVE_SYS_STAT_H */
#  if defined (HAVE_SYS_TYPES_H)
#     include <sys/types.h>
#  endif /* HAVE_SYS_TYPES_H */
#  if defined(HAVE_UNISTD_H)
#     include <unistd.h>
#  else /* ! HAVE_UNISTD_H */
#     ifdef _WIN32
#        define unlink _unlink
#     else /* ! _WIN32 */
         int unlink(char *);
#     endif /* _WIN32 */
#  endif /* HAVE_UNISTD_H */
#else /* ! HAVE_CONFIG_H */
#  include <string.h>
#  include <stdlib.h>
#  if defined(HAVE_UNISTD_H)
#     include <unistd.h>
#  else
#     ifdef _WIN32
#        define unlink _unlink
#     else
         int unlink(char *);
#     endif
#  endif
#  ifdef vms
#     define unlink delete
#     include <time.h>
#  else /* not vms */
#     include <sys/types.h>
#     include <sys/stat.h>
#  endif /* vms */
#endif /* HAVE_CONFIG_H */

#if defined(unix) || defined(__CYGWIN__) || defined(__MACH__)
#  define SEP '/'
#else
#  if defined(MSDOS) || defined(_WIN32)
#     define SEP '\\'
#  else
#     ifdef __50SERIES
#        define SEP '>'
#     else
#        define SEP '?'
#     endif
#  endif
#endif

#ifdef __50SERIES
   extern int time ();
#  define RMODE "i"
#  define WMODE "o"
#  define unlink delete
#else /* not __50SERIES */
#  ifdef vms
#     define unlink delete
#     include <time.h>
#  else /* not vms */
#     include <sys/types.h>
#     include <sys/stat.h>
#  endif /* vms */
/*   extern time_t time(); */
#  define RMODE "rb"
#  define WMODE "wb"
#  define UMODE "ab+"
#endif /* __50SERIES */
/*
 * extern char *strncpy ();
 * extern void exit ();
 */
#ifdef __STDC__
   void shift_up (char *);
   void shift_down (char *, int);
#  if STYLE >= 11
      extern int process_saved (int, char *);
#  endif
#else
   void shift_up();
   void shift_down();
#  if STYLE >= 11
      extern int process_saved();
#  endif
#endif

#if defined(HAVE_LIBNCURSES) && defined(HAVE_LIBREADLINE)
#  define READLINE
#endif /* HAVE_LIBNCURSES && HAVE_LIBREADLINE */

#ifdef CONSOLE
#  ifdef BROWSER
#     undef BROWSER
#  endif
#  ifdef GLK
#     undef GLK
#  endif
#  ifdef QT
#     undef QT
#  endif
#else
#  if !(defined(MSDOS) || defined (_WIN32))
#     define BROWSER
#  endif
#endif

#if defined(QT) || defined(GLK)
#  ifdef READLINE
#     undef READLINE
#  endif
#  ifdef BROWSER
#     undef BROWSER
#  endif
#endif

#ifdef READLINE
#include "readline/readline.h"
#include "readline/history.h"
char *prompt_line;
char *prompt_ptr;
#endif

#include "adv0.h"

FILE *text_file;
FILE *gv_file;
FILE *log_file = NULL;
char log_path [160];

FILE *com_file = NULL;
char *com_name = NULL;
char *dump_name = NULL;

#ifndef USEDB
#  include "adv6.h"
#endif /* ! USEDB */

#define SW_START     '\377'
#define SW_BREAK     '\376'
#define HOLDER       '\375'
#define IGNORE_EOL   '\373'
#define NEST_TEXT    '\372'
#define QUOTE_START  '\371'
#define TAG_START    '\370'
#define TAG_END      '\367'
#define NBSP         '\366'
#define BLOCK_START  '\365'
#define BLOCK_END    '\364'
#define CENTRE_START '\363'
#if STYLE >= 10
#  ifdef DWARVEN
#     define DWARVISH  '\362'
#  endif /* DWARVEN */
#  define VHOLDER      '\361'
#endif /* STYLE */
#define PARA_START     '\360'
#define PARA_END       '\357'

#if (defined(QT) || defined(GLK)) && !defined(NO_SLOW)
#  define NO_SLOW
#endif

#if (defined(QT) || defined(BROWSER)) && defined(GLK)
#  undef GLK
#endif

#if defined(QT) && defined(BROWSER)
#  undef BROWSER
#endif

#ifdef GLK
#  define ACDCHAR(X) glk_put_char(X)
#else
#  define ACDCHAR(X) putchar(X)
#endif

void outbuf (int);
int value_all;
jmp_buf loop_back;
#ifdef USEDB
   char *dbs_dir = NULL;
#endif /* USEDB */
char exec [128];
char virgin;
#define VAL_SIZE (LTEXT * sizeof (int))
#define OFFSET_LOCS VAL_SIZE
#define LOCS_SIZE ((LOBJ + 1) * sizeof (int))
#define OFFSET_OBJBIT (OFFSET_LOCS + LOCS_SIZE)
#define OBJBIT_SIZE (OBJSIZE * (LOBJ - FOBJ + 1) * sizeof (short))
#define OFFSET_LOCBIT (OFFSET_OBJBIT + OBJBIT_SIZE)
#define LOCBIT_SIZE (LOCSIZE * (LLOC - FLOC + 1) * sizeof (short))
#define OFFSET_VARBIT (OFFSET_LOCBIT + LOCBIT_SIZE)
#define VARBIT_SIZE (VARSIZE * (LVAR - FVERB + 1) * sizeof (short))
#define VRBBIT_SIZE (VARSIZE * (LVERB - FVERB + 1) * sizeof (short))
#define IMAGE_SIZE (OFFSET_VARBIT + VARBIT_SIZE)
char IMAGE [IMAGE_SIZE];
int *value = (int *)IMAGE;
int *location = (int *)(IMAGE + OFFSET_LOCS);
short *objbits = (short *)(IMAGE + OFFSET_OBJBIT);
short *placebits = (short *)(IMAGE + OFFSET_LOCBIT);
short *varbits = (short *)(IMAGE + OFFSET_VARBIT);
#ifdef UNDO
   char image [sizeof (IMAGE)];
   int inhand [LOCS_SIZE];
   unsigned char *diffs = NULL;
   unsigned char *dptr = NULL;
   unsigned char *edptr;
   int diffsz = 0;
#ifdef NO_UNDO
   int undo_def = -2;
#else
   int undo_def = 0;
#endif /* NO_UNDO */
#endif
char *prog;
char *optr;
char *obuf = NULL;
int oblen;
char comline [161] = "\n";
char old_comline [161] = "\n";
char raw_comline [161];
char long_word [161];
int arg1, arg2;
int orphan;
int qvals [5];
int *qtext = &qvals[0];
int *qstat = &qvals [1];
int *qarg1 = &qvals [2];
int *qarg2 = &qvals [3];
int *qcon = &qvals [4];
char say_buf[160];
#define WORDSIZE 20
char qwords [5 * WORDSIZE];
char *arg1_word = qwords;
char *arg2_word = qwords + WORDSIZE;
char arg3_word [WORDSIZE];
char *orphan_word = qwords + 2 * WORDSIZE;
char *qargw1 = qwords + 3 * WORDSIZE;
char *qargw2 = qwords + 4 * WORDSIZE;
char temp_word [WORDSIZE];
#if STYLE >= 11
#  define ORIG -1
   char orig [WORDSIZE];
   char orig1 [WORDSIZE];
   char orig2 [WORDSIZE];
#  if defined(ALL) && defined(EXCEPT)
      int except_count;
      int except [100];
#  endif /* ALL and EXCEPT */
#endif /* STYLE == 11 */
int rseed;
int mainseed = 0;
int amatch = 1;
#ifdef DWARVEN
   int extra_dwarvish = 0;
#endif
#if STYLE == 10
   int justify = 1;
#else
   int justify = 0;
#endif
#if STYLE == 1
   int compress = 1;
#else
   int compress = 0;
#endif
#if defined(PAUSE) || defined(MSDOS) || defined(_WIN32) || defined (GLK)
   int end_pause = 1;
#else
   int end_pause = 0;
#endif
#ifdef BROWSER
#  include <fcntl.h>
   char *brbuf;
   int brbuflen = 0;
   int get_command_from_browser (char *);
   void send_response_to_browser (char *);
   void invoke_browser (void);
#endif
int cgi = 0;
char cgicom [160];
char *cgi_name = CGINAME;

#include "adv3.h"
#include "adv4.h"
#include "adv2.h"
#include "adv5.h"

#ifdef AGAIN
   int done_again;
#endif /* AGAIN */

#ifdef MEMORY
   unsigned char text [TEXT_BYTES];
#endif /* MEMORY */

#ifdef SWAP
#  define TEXT_CHUNK 1024
   unsigned char text [SWAP * TEXT_CHUNK];
   int chunk_start [SWAP];
   int chunk_end [SWAP];
   int chunk_age [SWAP];
#endif /* SWAP */

char *tp [100];
char separator [100];
short tindex;
#if defined(GLK) || defined(QT)
   int Screen = 32767;
   int Linlen = 32767;
   int Margin = 0;
#else
   int Screen = 24;
   int Linlen = 80;
   int Margin = 1;
#endif
int Maxlen;
int ta;
int locate_faults;
int locate_demands;
#ifdef LOC_STATS
   int old_faults;
   int old_demands;
#endif /* LOC_STATS */
char *text_buf;
int text_buf_len = 4096;
#if STYLE >= 11
   short *word_buf = NULL;
   int upcase = 1;
#endif
char *lptr;
int text_len = 0;
int location_all;
#if !defined(NO_SLOW) && !defined(SLOW)
#  define SLOW
#endif

#if (defined(QT) || defined(GLK)) && defined(SLOW)
#  undef SLOW
#endif

int cps = 0;
#ifdef SLOW
#  if (defined(MSDOS) || defined(_WIN32)) && !defined(DJGPP)
#     ifdef __STDC__
         extern void my_usleep (int);
#     else
         extern void my_usleep();
#     endif
#     define usleep(X) my_usleep(X)
#  else
#     if ! defined(HAVE_UNISTD_H)
         void usleep(int);
#     endif
#  endif
#endif /* SLOW */
int query_flag = 0;
int type_all;
char title [80];
int titlen;
int quitting;
int eol_count = 0;
#ifdef TAG_START
   int html_tag = 0;
#endif /* TAG_START */
char *lp;

#define PRINTF(X)    { char *ptr = X; while (*ptr) outchar(*ptr++); }
#define PRINTF1(X)   fputs(X, stdout); if (log_file) fputs(X,log_file);
#define PRINTF2(X,Y) printf(X,Y); if (log_file) fprintf(log_file,X,Y);
#define LOGERROR(X)  if (log_file) fprintf(log_file,"ERROR: %d",X)
#define CHKSUM(X,Y)  for (cptr=(char *)X,cnt=1; \
                     (unsigned int)cnt<=(unsigned int)Y;cnt++,cptr++) \
                     {chksum+=(*cptr+cnt)*(((int)(*cptr)+cnt)<<4)+Y; \
                     chksum&=07777777777L;} 

#define RESTORING "Restoring game in progress...\n\n"

/******************************************************************/

#ifdef GLK
#  include "glk.h"
   static winid_t mainwin = NULL;
#  ifdef __STDC__
void glkgets (char *buf, int buflen)
#  else
void glkgets (buf, buflen)
   char *buf;
   int buflen;
#  endif
{
   int gotline = 0;
   int len;
   event_t event;
   
   glk_request_line_event(mainwin, buf, buflen, 0);
   while (!gotline)
   {
      glk_select(&event);
       
      switch (event.type) 
      {
         case evtype_LineInput:
            gotline = 1;

         default:
            break;
      }
   }
   len = event.val1;
   *(buf + len) = '\n';
   *(buf + len + 1) = '\0';
}
#endif

/*===========================================================*/

#if !defined(NOVARARGS) && defined(__STDC__)
int anyof (int first, ...)
{
    va_list ap;
    int val;

    va_start (ap, first);
    val = first;
    while (val >= 0)
    {
       if (KEY (val))
       {
          va_end (ap);
          return (1);
       }
       val = va_arg (ap, int);
    }
    va_end (ap);
    return (0);
}

/*===========================================================*/

int keyword (int first, ...)
{
    va_list ap;
    int val;

    va_start (ap, first);
    val = first;
    while (val >= 0)
    {
       if (!KEY (val))
       {  
          va_end (ap);
          return (0);
       }
       val = va_arg (ap, int);
    }
    va_end (ap);
    return (1);
}
#else /* not (NOVARARGS && __STDC_)*/

/*===========================================================*/

#ifdef __STDC__
int anyof (int l0,int l1,int l2,int l3,int l4,int l5,int l6,int l7,
           int l8,int l9,int l10,int l11,int l12,int l13,int l14,int l15)
#else
int anyof (l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15)
int l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15;
#endif
{
   if (l0 == -1) return (0);
   if (KEY (l0)) return (1);
   if (l1 == -1) return (0);
   if (KEY (l1)) return (1);
   if (l2 == -1) return (0);
   if (KEY (l2)) return (1);
   if (l3 == -1) return (0);
   if (KEY (l3)) return (1);
   if (l4 == -1) return (0);
   if (KEY (l4)) return (1);
   if (l5 == -1) return (0);
   if (KEY (l5)) return (1);
   if (l6 == -1) return (0);
   if (KEY (l6)) return (1);
   if (l7 == -1) return (0);
   if (KEY (l7)) return (1);
   if (l8 == -1) return (0);
   if (KEY (l8)) return (1);
   if (l9 == -1) return (0);
   if (KEY (l9)) return (1);
   if (l10 == -1) return (0);
   if (KEY (l10)) return (1);
   if (l11 == -1) return (0);
   if (KEY (l11)) return (1);
   if (l12 == -1) return (0);
   if (KEY (l12)) return (1);
   if (l13 == -1) return (0);
   if (KEY (l13)) return (1);
   if (l14 == -1) return (0);
   if (KEY (l14)) return (1);
   if (l15 == -1) return (0);
   if (KEY (l15)) return (1);
   return (0);
}

/*===========================================================*/

#ifdef __STDC__
int keyword (int l0,int l1,int l2,int l3,int l4,int l5,int l6,int l7,
             int l8,int l9,int l10,int l11,int l12,int l13,int l14,int l15)
#else
int keyword (l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15)
int l0,l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15;
#endif
{
   if (l0 == -1) return (1);
   if (!KEY (l0)) return (0);
   if (l1 == -1) return (1);
   if (!KEY (l1)) return (0);
   if (l2 == -1) return (1);
   if (!KEY (l2)) return (0);
   if (l3 == -1) return (1);
   if (!KEY (l3)) return (0);
   if (l4 == -1) return (1);
   if (!KEY (l4)) return (0);
   if (l5 == -1) return (1);
   if (!KEY (l5)) return (0);
   if (l6 == -1) return (1);
   if (!KEY (l6)) return (0);
   if (l7 == -1) return (1);
   if (!KEY (l7)) return (0);
   if (l8 == -1) return (1);
   if (!KEY (l8)) return (0);
   if (l9 == -1) return (1);
   if (!KEY (l9)) return (0);
   if (l10 == -1) return (1);
   if (!KEY (l10)) return (0);
   if (l11 == -1) return (1);
   if (!KEY (l11)) return (0);
   if (l12 == -1) return (1);
   if (!KEY (l12)) return (0);
   if (l13 == -1) return (1);
   if (!KEY (l13)) return (0);
   if (l14 == -1) return (1);
   if (!KEY (l14)) return (0);
   if (l15 == -1) return (1);
   if (!KEY (l15)) return (0);
   return (1);
}
#endif /* NOVARARGS && __STDC__*/

/*===========================================================*/

#ifdef __STDC__
int irand (int less_then)
#else
int irand (less_then)
int less_then;
#endif
{
   rseed = (((rseed << 10) + rseed) / 13) & 32767;
   return (rseed % less_then);
}

/*===========================================================*/

#ifdef __STDC__
int jrand (int less_then)
#else
int jrand (less_then)
int less_then;
#endif
{
   int rvalue;
   rvalue = (((rseed << 10) + (int) time (NULL)) / 13) & 32767;
   return (rvalue % less_then);
}

#if STYLE >= 11
#define BT_LSIB   -1
#define BT_UP      1
#define BT_RSIB    1
#define BT_BAL     3
#define BT_TXT     4
#define BT_STP  4096
#define BT_SIZE *((unsigned short *)(root + 2))

/*===========================================================*/

short *btinit (short *root)
{
   if (root == NULL)
   {
      if ((root = (short *)malloc(BT_STP * sizeof(short))) == NULL)
         return (NULL);
      *root = 3;
      *(root + 1) = 0;
      BT_SIZE = BT_STP;
   }
   else
   {
      if ((root = (short *)realloc 
         (root, (*(root + 2) + BT_STP) * sizeof(short))) == NULL)
            return (NULL);
      BT_SIZE += BT_STP;
   }
   return (root);
}

#ifdef DEBUG
void btshow (short *root, char *text)
{
   short *iptr;

   printf ("Show: %s, free: %hd, root: %hd\n", text, *root, *(root + 1));
   iptr = root + 3;
   while (iptr < root + *root)
   {
      fprintf (stderr, "Offset %d: Up %hd, L %hd, R %hd, B %hd, T: %s\n",
         iptr - root, *(iptr + BT_UP), *(iptr + BT_UP + BT_LSIB), 
            *(iptr + BT_UP + BT_RSIB), *(iptr + BT_BAL), 
               (char *)(iptr + BT_TXT));
      iptr += BT_TXT + 1 + strlen ((char *) (iptr + BT_TXT)) / 2;
   }
}
#endif /* DEBUG */

void btcopy (char *start, int len, short *new_rec)
{
   char *cptr = (char *) (new_rec + BT_TXT);
   while (len--)
      *cptr++ = tolower (*start++);
   *cptr = '\0';
}

int btcompare (char *word, char *cword)
{
   int cchar, wchar;

   while (*word)
   {
      wchar = tolower ((unsigned char) *word++);
      if (!isalpha (wchar))
         return (*cword ? -1 : 0);
      cchar = (unsigned char) *cword++;
      if (wchar != cchar)
         return (wchar > cchar ? 1 : -1);         
   }       
   return (0);
}

void sing_rot (short *root, int parent, int child, int dir)
{
   int gparent = *(root + parent + BT_UP);
   int gchild = *(root + child + BT_UP - dir);
   
   *(root + parent + BT_UP + dir) = *(root + child + BT_UP - dir);
   *(root + child + BT_UP - dir) = parent;
   *(root + parent + BT_BAL) -= *(root + child + BT_BAL);
   *(root + child + BT_BAL) = -(*(root + parent + BT_BAL));
   if (gparent > 0)
      *(root + gparent + BT_UP +
         (*(root + gparent + BT_UP + BT_LSIB) == parent ? BT_LSIB : BT_RSIB)) =
            child;
   else
      *(root + 1) = child;
   *(root + child + BT_UP) = gparent;
   *(root + parent + BT_UP) = child;
   if (gchild)
      *(root + gchild + BT_UP) = parent;
}

void dub_rot (short *root, int parent, int child, int dir)
{
   int gparent = *(root + parent + BT_UP);
   int gchild = *(root + child + BT_UP - dir);

   *(root + child  + BT_UP - dir) = *(root + gchild + BT_UP + dir);
   if (*(root + gchild + BT_UP + dir))
      *(root + *(root + gchild + BT_UP + dir) + BT_UP) = child;
   *(root + parent + BT_UP + dir) = *(root + gchild + BT_UP - dir);
   if (*(root + gchild + BT_UP - dir))
      *(root + *(root + gchild + BT_UP - dir) + BT_UP) = parent;
   *(root + gchild + BT_UP + dir) = child;
   *(root + gchild + BT_UP - dir) = parent;

   if (*(root + gchild + BT_BAL) == *(root + child + BT_BAL))
      *(root + child + BT_BAL) *= -1;
   else
      *(root + child + BT_BAL) = 0;
   if (*(root + gchild + BT_BAL) == *(root + parent + BT_BAL))
      *(root + parent + BT_BAL) *= -1;
   else
      *(root + parent + BT_BAL) = 0;
   *(root + gchild + BT_BAL) = 0;

   *(root + child + BT_UP) = gchild;
   *(root + parent + BT_UP) = gchild;
   *(root + gchild + BT_UP) = gparent;
   if (gparent > 0)
      *(root + gparent + BT_UP +
         (*(root + gparent + BT_UP + BT_LSIB) == parent ? BT_LSIB : BT_RSIB)) =
            gchild;
   else
      *(root + 1) = gchild;
}

short *btadd (short *root, char *word, int len)
{
   int parent = 0;
   int child = *(root + 1);
   int dir;
   short *new_rec;
   int reclen = BT_TXT  + 1 + len / 2;

   if (*(root + 1) > 0)
   {
      while (child > 0)
      {
         if ((dir = btcompare (word, (char *)(root + child + BT_TXT))) == 0)
            return (root);
         parent = child;
         child = *(root + child + BT_UP + dir);
      }
   }
   if (*root + reclen > BT_SIZE && 
      (BT_SIZE > 65535L - reclen || (root = btinit (root)) == NULL))
         return (NULL);
   new_rec = root + (child = *root);
   *(new_rec + BT_UP) = parent;
   *(new_rec + BT_UP + BT_LSIB) = *(new_rec + BT_UP + BT_RSIB) = 0;
   *(new_rec + BT_BAL) = 0;
   btcopy (word, len, new_rec);
   *root += reclen;
   if (parent)
   {
      *(root + parent + BT_UP + dir) = child;
      while (1)
      {
         dir = *(root + parent + BT_UP + BT_LSIB) == child ? BT_LSIB : BT_RSIB;
         if (*(root + parent + BT_BAL) == dir)
         {
            if (*(root + child + BT_BAL) == -dir)
               dub_rot (root, parent, child, dir);
            else 
               sing_rot (root, parent, child, dir);
            break;
         }
         if ((*(root + parent + BT_BAL) += dir) == 0)
            break;
         child = parent;
         parent = *(root + parent + BT_UP);
         if (parent == 0) break;
      }
   }
   else
      *(root + 1) = child;
   return (root);
}

int btfind (short *root, char *word)
{
   int node;
   int dir;
   
   if ((node = *(root + 1)) == 0)
      return (0);
   while (node)
   {
      if ((dir = btcompare (word, (char *)(root + node + BT_TXT))) == 0)
         return (node);
      node = *(root + node + BT_UP + dir);
   }
   return (0);
}

/*===========================================================*/

#ifdef __STDC__
void word_update (void)
#else
void word_update ()
#endif
{
   char *aptr = text_buf;
   char *zptr;
   
   while (1)
   {
      while (*aptr && ! isalpha (*aptr)) aptr++;
      if (*aptr == '\0') break;
      zptr = aptr + 1;
      while (*zptr && isalpha (*zptr)) zptr++;
      if (zptr - aptr > 4 &&
         *(zptr - 3) != 'i' && *(zptr - 2) != 'n' && *(zptr - 1) != 'g')
            btadd (word_buf, aptr, zptr - aptr);
      if (*zptr == '\0') break;
      aptr = zptr + 1;
   }   
   return;
}
#endif /* STYLE */

#define MAX_BREAKS 100

/* PRIVATE */
#if defined(PLAIN) && (defined(MEMORY) || defined(PRELOADED))
#  define get_char(X) text[X]
#else /* !PLAIN || (!MEMORY && !PRELOADED)*/

/*===========================================================*/

#ifdef __STDC__
char get_char (int char_addr)
#else
char get_char (char_addr)
int char_addr;
#endif
{
#ifndef PLAIN
   int mask;
#endif /* not PLAIN */
#if defined(MEMORY) || defined (PRELOADED)
   mask = (char_addr >> 4) & 127;
   if (mask == 0)
      mask = char_addr & 127;
   if (mask == 0)
      mask = 'X';
   mask = (17 * mask + 13) & 127;
   return (mask ^ text [char_addr] ^ title [char_addr % titlen]);
#endif /* MEMORY || PRELOADED */
#ifdef SWAP
   int index;
   unsigned char *buf_ptr;
   int oldest_chunk;
   int new_addr;
   void file_oops ();
   
   oldest_chunk = 0;
   buf_ptr = text;
   for (index = 0; index < SWAP; index++)
   {
      if (char_addr >= chunk_start [index] && char_addr < chunk_end [index])
         goto gotit;
      buf_ptr += TEXT_CHUNK;
   }

   for (index = 0; index < SWAP; index++)
   {
      if (chunk_end [index] == 0)
         goto readit;
      if (chunk_age [oldest_chunk] > chunk_age [index])
         oldest_chunk = index;
   }
   index = oldest_chunk;

readit:
   locate_faults++;
   buf_ptr = text + TEXT_CHUNK * index;
   new_addr = (char_addr / TEXT_CHUNK) * TEXT_CHUNK ;
   if (fseek (text_file, new_addr, 0))
      file_oops ();
   chunk_start [index] = new_addr;
   chunk_end [index] = fread (buf_ptr, sizeof (char), 
      TEXT_CHUNK, text_file) + new_addr;
#ifdef LOC_STATS
   printf ("Wanted %ld.  Buffer %d: from %ldK.\n",
      char_addr, index, new_addr / TEXT_CHUNK);
#endif /* LOC_STATS */
   if (chunk_start [index] > chunk_end [index])
      file_oops ();

gotit:
   chunk_age [index] = locate_demands;
#ifdef PLAIN
   return (*(buf_ptr + char_addr - chunk_start [index]));
#else /* not PLAIN */
   mask = (char_addr >> 4) & 127;
   if (mask == 0)
      mask = char_addr & 127;
   if (mask == 0)
      mask = 'X';
   mask = (17 * mask + 13) & 127;
   return 
      (*(buf_ptr + char_addr - chunk_start [index]) ^ mask ^ 
         title [char_addr % titlen]);
#endif /* PLAIN */
#endif /* SWAP */
#ifdef READFILE
   void file_oops ();
   static int file_addr = -1;
   char this_char;   

   if (file_addr != char_addr)
   {
      locate_faults++;
      if (fseek (text_file, char_addr, 0))
         file_oops ();
      file_addr = char_addr;
   }
   this_char = fgetc (text_file);
   file_addr++;
#ifdef PLAIN
   return (this_char);
#else /* not PLAIN */
   mask = (char_addr >> 4) & 127;
   if (mask == 0)
      mask = char_addr & 127;
   if (mask == 0)
      mask = 'X';
   mask = (17 * mask + 13) & 127;
   return (this_char ^ mask ^ title [char_addr % titlen]);
#endif /* PLAIN */
#endif /* READFILE */
}

#if defined(SWAP) || defined(READFILE)

/*===========================================================*/

#ifdef __STDC__
void file_oops (void)
#else
void file_oops ()
#endif
{
   PRINTF ("\n \nUnable to retrieve required data! Sorry...\n");
   outbuf (1);
   fclose (text_file);
   if (log_file) fclose (log_file);
   exit (0);
}
#endif /* SWAP || READFILE */
#endif /* PLAIN && MEMORY */
/* ENDPRIVATE */

/*===========================================================*/

#ifdef __STDC__
void voc (int word, int what, int test, int vtext)
#else
void voc (word, what, test, vtext)
int word;
int what;
int test;
int vtext;
#endif
{
   int index;
   int tc;
   static int vc = 0;
   
   if (word == 0)
   {
      vc = 0;
      return;
   }
   if (what == 0)
      what = word;
   if (test >0 && bitest (what, test) == 0)
      return;
   if (vc++ > 0)
   {
      outchar (',');
      outchar (' ');
   }      
   index = (vtext == 0) ? word : vtext;
   ta = textadr [index];
   tc = get_char (ta);
   if (tc == '!')
      tc = get_char (++ta);
   while (tc != '\0')
   {
      outchar (tc);
      tc = get_char (++ta);
   }
}

/*===========================================================*/

#ifdef __STDC__
int scrchk (char *iptr)
#else
int scrchk (iptr)
int clear;
char *iptr;
#endif
{
#ifdef GLK
   return (0);
#else /* !GLK */
   static int lincnt = 0;
   char reply [160];

   if (cgi) return (0);
   if (!iptr || com_file) 
   {
      lincnt = 0;
      return (0);
   }
   
   while (*iptr && *iptr != '\n') iptr++;

   if (*iptr == '\n' && ++lincnt >= Screen)
   {
      memset (reply, ' ', 20);
      strcpy (reply + Margin, "[More?] ");
      PRINTF1 (reply);
      fgets (reply, sizeof (reply) - 1, com_file ? com_file : stdin);
      if (log_file)
         fprintf (log_file, "\nREPLY: %s", reply);
      lincnt = 1;
      if (*reply == 'n' || *reply == 'N' || *reply == 'q' || *reply == 'Q')
      {
         memset (reply, ' ', Margin);
         strcpy (reply + Margin, "OK.\n");
         if (!compress) strcat (reply, "\n");
         PRINTF1 (reply);
#ifdef READLINE
         memset (prompt_line, ' ', Margin);
         strcpy (prompt_line + Margin, "? ");
#else
         strcpy (reply + Margin, "? ");
         PRINTF1 (reply);         
#endif
         return (1);
      }
   }
   return (0);
#endif /* GLK */
}

/*====================================================================*/
char *advalloc (int len)
{
   char *ptr;
   if ((ptr = (char *) malloc (len)) == NULL)
   {
      fprintf (stderr, 
         "Failed to allocate a %d character memory chunk!\n", len);
      exit (1);
   }
   return (ptr);
}
/*====================================================================*/
char *advrealloc (char *buf, int newlen)
{
   char *ptr;
   if ((ptr = (char *) realloc (buf, newlen)) == NULL)
   {
      fprintf (stderr,
         "Failed to extend a memory chunk to %d character!\n", newlen);
      exit (1);
   }
   return (ptr);
}
/*====================================================================*/
/* Append a character to the formatted output buffer, while checking 
 * for and preventing buffer overflow, as well as ensuring thet the
 * text remains null-terminated.
 */
void oputc (char c)
{
   int ofst = optr - obuf;       /* Number of chars currently in the buffer */
   if (ofst + 2 >= oblen)        /* Enough room for at least one more? */
   {
      oblen += 4096;             
      obuf = advrealloc (obuf, oblen);    /* Enlarge the buffer */
      optr = obuf + ofst;        /* Point to the right place in new buffer */
   }
   *optr++ = c;                  /* Put in the char */
   *optr = '\0';                 /* Ensure the lot is terminated */
}
/*====================================================================*/
/* Append a null-terminated string to the formatted output buffer,
 * while checking for and preventing buffer overflow.
 */
void oputs (char *string)
{
   int len = strlen (string);    /* Number of characters to add */
   int ofst = optr - obuf;       /* Number of character already in there */
   if (ofst + len + 1 >= oblen)  /* Enough room? */
   {
      oblen += 4096;
      obuf = advrealloc (obuf, oblen); /* Enlarge buffer */
      optr = obuf + ofst;        /* Point to the right place in bew buffer */
   }
   strcpy (optr, string);        /* Append the string */
   optr += len;                  /* And remember to point to its end */
}
/*====================================================================*/
/* Almost all preformatting now done and characters are being output to
 * the console (or accumulated in the readline() prompt line).
 */
void showchar (char c, char target)
{
   FILE *fh = (target == 'L' ? log_file : stdout);
   
/* Non-breaking space conversion deliberately postponed until the last
 * possible moment, to avoid it being used for line breaks in non-HTML
 * modes.
 */
   if (c == NBSP) c = ' ';

#ifdef READLINE
   if (cps && target != 'L') /* No readline() if slow output */
      { usleep (cps); putchar (c); fflush (stdout); }
   else 
   {
      *prompt_ptr++ = c; *prompt_ptr = '\0'; /* Add to potential prompt line */
      if (c == '\n')                         /* It wasn't a prompt line! */
      { 
         if (*prompt_line) fputs (prompt_line, fh);   /* So print it */
         fflush (fh);
         prompt_ptr = prompt_line;
         *prompt_ptr = '\0';                 /* Set prompt line to null */
      }
   }
#endif /* READLINE */
#ifdef GLK
   glk_put_char (c);                         /* GLK does its own thing */
#endif /* GLK */
#if !defined(READLINE) && !defined (GLK)
   if (cps && target != 'L')
   {
      usleep (cps);   /* Pause is slow output */
      fputc (c, fh);
      fflush (stdout);                          /* Make sure slowness is visible */
   }
   else 
      fputc (c, fh);
#endif /* !READLINE && !GLK */
}
/*====================================================================*/
/* Output character filter to do special things.
 */
char *ofilter (char *iptr)
{
   if (*iptr == TAG_START)
   {
      if (!cgi) while (*iptr && *iptr != TAG_END) iptr++;
      else oputc ('<');
   }
   else if (!cgi) oputc (*iptr);
   else if (*iptr == '>' || *iptr == '<') 
      oputs (*iptr == '<' ? "&lt;" : "&gt;");
   else if (*iptr == TAG_END) oputc ('>');
   else 
   {
      if (*iptr == ' ' && *(iptr + 1) == ' ' && cgi) oputs ("&nbsp;");
      oputc(*iptr);
   }
   return (iptr);
}
/*====================================================================*/
/* Centers lines individually.
 */
char *centre_text_lines (char *iptr)
{
   char *jptr;

   while (1)
   {
      iptr++;
      while (*iptr == ' ') iptr++;
      jptr = iptr;
      while (* iptr && *iptr != '\n' && *iptr != BLOCK_END)
         iptr++;
      if (!cgi)
      {
         int l = (Maxlen - (iptr - jptr)) / 2;
         while (l--) oputc (NBSP);
      }
      while (jptr < iptr) ofilter (jptr++);
      if (*iptr != '\n')
         break;
      oputs (cgi ? "<br />\n" : "\n");
   }
   return (iptr);
}
/*====================================================================*/
/* Called both in cgi and non-cgi modes. Works out and discards the
 * shortest blank lead. In non-cgi mode also works out the longest
 * remaining line length and centers all lines as if they had that length,
 * thereby actually centering the block as a whole.
 */
char *centre_block (char *iptr)
{
   int lead = 4096;
   int mxl = 0;
   int i;
   int type = *iptr++;
   char *jptr = iptr;
   int offset;
   char *bptr;
   
   while (1)
   {
      bptr = jptr;
      while (*jptr == ' ') jptr++;
      if (jptr - bptr < lead) lead = jptr - bptr;
      while (*jptr != '\n' && *jptr != BLOCK_END) jptr++;
      if (jptr - bptr > mxl) mxl = (jptr - bptr);
      if (!*jptr || *jptr == BLOCK_END) break;
      jptr++;
   }
   offset = (Maxlen - mxl - lead) / 2;
   while (1)
   {
      for (i = 0; i < lead; i++) iptr++;
      if (!cgi) for (i = 0; i < offset; i++) oputc (' ');
      while (*iptr != '\n' && *iptr != BLOCK_END)
      {
         if (*iptr == NBSP) oputc (' ');
         else iptr = ofilter (iptr);
         iptr++;
      }
      if (*iptr != BLOCK_END)
         oputs (cgi && type != QUOTE_START ? "<br />\n" : "\n");
      else
         break;
      iptr++;
   }
   if (!cgi) oputc ('\n');
   
   return (*iptr == BLOCK_END ? iptr + 1 : iptr);
}
/*====================================================================*/
void format_buffer (int terminate, int html)
{
   char *iptr = text_buf;
   char *bptr = lptr;
   int frag = (bptr > text_buf && *(bptr - 1) != '\n');
   int type;

   optr = obuf + (cgi == 'b' ? 84 : 0);
   
   while (*iptr == '\n')
      iptr++;
   if (frag && html)
   {
      bptr--;
      if (*bptr == ' ' || *bptr == NBSP) bptr--;
   }

/* Prepare the output opening and skip any introductory line feeds. */

   if (html) oputs ("<div name=\"chunk\">");
      
/* Now process the text buffer */

   while (iptr < bptr)
   {
      switch (*iptr)
      {
         case '\n':
            if (!html)
            {  if (compress < 2 || *(optr - 1) != '\n')
               oputc ('\n');
            }
            if (*(iptr + 1) == '\n')
            {
               if (html)
                  oputs (compress > 1 ? "<br />\n" : "<br />&nbsp;<br />\n");
               else if (compress < 2) oputc ('\n');
               while (*(iptr + 1) == '\n') iptr++;
            }
            else if (html && *(iptr + 1))
               oputs ("<br />\n");  
            break;
         case NBSP:
            if (html) oputs ("&nbsp;");
            else oputc (*iptr);
            break;
         case QUOTE_START:
         case BLOCK_START:
         case CENTRE_START:
#ifdef GLK
            glk_set_style (style_Preformatted);
            iptr = centre_block (iptr);
            glk_set_style (style_Normal);
#else
            if (html)
            {
               oputs ("<center>\n");
               if (*iptr != CENTRE_START) oputs ("<table><tr><td>\n");
               if (*iptr == QUOTE_START) oputs ("<pre>");
            }
            type = *iptr;
            if (type == CENTRE_START)
               iptr = centre_text_lines (iptr);
            else
               iptr = centre_block (iptr);
            if (html)
            {           
               if (type == QUOTE_START) oputs ("</pre>");
               if (type != CENTRE_START) oputs ("</td></tr></table>\n");
               oputs ("</center>\n");
               if ((type == CENTRE_START || compress > 1) && *iptr)
               {
                  if (*(iptr + 1) == '\n') iptr++;
                  if (compress > 1 && *(iptr + 1) == '\n') iptr++;
               }
            }  
#endif
            break;
         case BLOCK_END:   /* Should not happen at all! */
            break;         /* Ignore */
         default:
            iptr = ofilter (iptr);
            break;
      }
      if (*iptr == '\0') break;
      iptr++;
      if (*iptr == '\0') break;
   }
   
   if (html)
   {
      char tf;
      if (frag)
      {
         char c = *iptr;
         oputs (terminate == 0 ? 
            "<span id=\"prompt\">" : "<span class=\"query\">");
         oputc (c);
         oputs ("</span>");
         if (!compress) oputs ("<br />&nbsp;<br />");
      }
      else
      {
         if (!compress && bptr > text_buf) oputs("<br />&nbsp;<br />");
         oputs ("<span id=\"prompt\"></span>");
      };
      oputs ("</div>\n");
      if (quitting)                          tf = 'f';   /* Finish! */
#ifdef ADVCONTEXT
      else if (frag)
      {
         if      (value [ADVCONTEXT] ==  10) tf = 'm';   /* Magic word */
         else if (value [ADVCONTEXT] ==  15) tf = 's';   /* Save name */
         else if (value [ADVCONTEXT] ==  44) tf = 'r';   /* Restore name */
         else                                tf = 'q';   /* Query */
      }
#else
      else if (frag)                         tf = 'q';   /* Query */
#endif /* ADVCONTEXT */
      else if (cgi == 'b' && compress)       tf = 'c';   /* Compressed text */
      else                                   tf = 't';   /* Normal text */
      oputc (tf);
   }
   else if (frag)
   {
      char *eptr = lptr - 1;
      if (eptr >= text_buf && *eptr != ' ' && *eptr != NBSP) 
         oputc (' ');
   }
   else if (!quitting)
      oputs ("\n? ");
}
/*====================================================================*/
void stretch_line (char *iptr, char t)
{
   char *jptr = iptr;   /* Local pointer into line */
   int len = 0;         /* Line length */
   int cnt = 0;         /* Word break count */
   int need;            /* Number of blanks to insert */
   int add;             /* Basic count of blanks to echo per brake */
   int extra;           /* Number of extra blanks needed on top of that */
   int lb;              /* Lower break bound for adding the extra blank */
   int ub;              /* Upper break bound ditto */
   static int flip = 0; /* 0/1 flip-flop to minimise "rivers" */

   while (*jptr)     /* Count the number of breaks and the overall length */
   {
      if (*jptr++ == ' ') cnt++; /* Another break */
      len++;                     /* Another char */
   }
   if (cnt == 0)                 /* No breaks? Give in. */
      { while (*iptr) showchar (*iptr++, t); return; }
   need = Maxlen - len;          /* Need this many extra characters */
   add = 1 + need / cnt;         /* Basic blanks per break */
   extra = need % cnt;           /* This mane breaks need one more */
   if (flip)                     /* Which end should the more padding go? */
      { lb = 0; ub = extra; }          /* Left hand end  or... */
   else
      { lb = cnt - extra; ub = cnt; }  /* ... right hand one */
   flip = 1 - flip;              /* Next time do it the other way */
   cnt = 0;
   while (*iptr)                 /* Now actually process the line */
   {
      if (*iptr != ' ')          /* If not a break... */
         showchar (*iptr, t);        /* ... just print the char */
      else
      {                          /* It's a break */
         int i;
         for (i = 0; i < add; i++) showchar (' ', t);  /* Print basic blanks */
         if (cnt >= lb && cnt < ub) showchar (' ', t); /* Add extra blank */
         cnt++;                  /* Count breaks looked at */
      }
      iptr++;                    /* Move one character up */
   }
}
/*====================================================================*/
void outtext (char t)
{
   char *iptr = obuf + (cgi == 'b' ? 84 : 0);
   char *jptr = iptr;
   char *bptr;
   char c;
   int i;
   int mx = cgi ? 80 : Maxlen;
   int mg = cgi ? 0 : Margin;
   
   while (1)
   {
      jptr = iptr;
      bptr = NULL;
      while (jptr - iptr <= mx)
      {
         if (*jptr == '\0' || *jptr == '\n') { bptr = jptr; break; }
         if (*jptr == ' ' || *jptr == '-') bptr = jptr;
         jptr++;
      }
#ifdef READLINE
      prompt_ptr = prompt_line;
      *prompt_ptr = '\0';
#endif
      if (*jptr == '\n' || *jptr == '\0')
      {
         for (i = 0; i < mg; i++) showchar (' ', t);
         while (iptr < jptr) showchar (*iptr++, t);
         if (*jptr == '\0') break;
         showchar (*iptr++, t);
         if (*(iptr - 1) == '\n' && scrchk (iptr)) break;
         continue;
      }
      if (!bptr) bptr = jptr;
      if (*bptr) bptr++;
      c = *bptr;
      *bptr = '\0';                    /* Only temporarily! */
      jptr = bptr;
      while (*(jptr - 1) == ' ') jptr--;
      if (jptr < bptr) *jptr = '\0';   /* Also temporary! */
      if (*iptr != '\n')
         for (i = 0; i < mg; i++) showchar (' ', t);
      if (justify)
         stretch_line (iptr, t);
      else
         while (*iptr)
         {
            showchar (*iptr++, t);
            if (*(iptr - 1) == '\n' && scrchk (iptr)) break;
         }
      if (c && *(bptr - 1) != '\n') showchar ('\n', t);
      if (!c) break;
      if (jptr < bptr) *jptr = ' ';    /* Restore the blank */
      *bptr = c;                       /* Restore whatever it was */
      iptr = bptr;
      if (scrchk (iptr)) break;
   }
}
/*====================================================================*/
void outbuf (int terminate)
{
   int first = 0;
   
   if (!obuf)
   {
      first = 1;
      obuf = advalloc (4096);
      oblen = 4096;
   }
#ifdef BROWSER
   if (first)
   {
      if (cgi == 'b') 
         invoke_browser ();
      else
         puts ("");
   }
#endif 
   if (lptr > text_buf)
   {
      char *eptr = lptr - 1;
      char trm = '\0';
      while ((*eptr == ' ' || *eptr == NBSP || *eptr == '\n') && eptr > text_buf)
      {
         if (!trm) trm = *eptr;
         eptr--;
      }
      if (trm)
      {
         *(++eptr) = trm;
         lptr = eptr + 1;
         *lptr = '\0';
      }
/*
 *      if (eptr + 2 < lptr)
 *         lptr = eptr + 2;
 *      *lptr = '\0';
 */
   }
   
   if (log_file || !cgi)
      format_buffer (terminate, 0);    /* Non-HTML format for the log */
   if (log_file)
      outtext ('L');
   if (!cgi)
      outtext ('C');
   else
   {
      format_buffer (terminate, 1);    /* Need HTML format */
#ifdef READLINE
      prompt_ptr = prompt_line;
      *prompt_line = '\0';
#endif /* READLINE */
#ifdef BROWSER
      if (cgi == 'b')
         send_response_to_browser (obuf);
      else
#endif /* BROWSER */
         fputs (obuf, stdout);
   }

/* Now zap the text buffer */

   lptr = text_buf;
   *lptr = '\0';
   text_len = 0;
}
/*====================================================================*/

#define VOC_FLAG     128
#define QUIP_FLAG     64
#if defined(DETAIL) || STYLE >= 11
#  define DETAIL_FLAG 32
#endif
#ifdef FULL
#  define FULL_FLAG   16
#else
#  define FULL_FLAG    0
#endif
#define QUAL_FLAG      8
#define VQUAL_FLAG     4
#define VAR_FLAG       2
#define VALUE_FLAG     1

#ifdef NEST_TEXT

#ifdef __STDC__
void nested_say (int addr, int key, int qualifier)
#else
void nested_say (addr, key, qualifier)
int addr;
int key;
int qualifier;
#endif
{
   int refno;
   int type;
   
   if (key &= 14)
      key = 8;
   type = get_char (addr++);
   refno = get_char (addr++) << 8;
   refno |= (get_char (addr) & 255);
   if (type == 0)
      key = VOC_FLAG;
   else if (type == 1)
      key = VOC_FLAG;
   else if (type == 3)
   {
      key = VAR_FLAG | VOC_FLAG;
      qualifier = 0;
   }

   if (refno == ARG1 || refno == ARG2)
   {
      char *cptr = (refno == ARG1) ? arg1_word : arg2_word;
      while (*cptr) outchar (*cptr++);
   }
   else
   {
      if (type < 2) qualifier = 0;
      say (key, refno, qualifier);
   }
   ta = addr;
}
#endif /* NEST_TEXT */

/*===========================================================*/

#ifdef __STDC__
void say (int key, int what, int qualifier)
#else
void say (key, what, qualifier)
int key;
int what;
int qualifier;
#endif
{
   int index;
   int offset;
   int value_flag;
   int qual_flag;
   int var_what_flag;
   int var_qual_flag;
   int quip_flag;
   int brief_flag = 0;
   int voc_flag;
#ifdef FULL
   int full_flag;
#endif /* FULL */
#if defined(DETAIL) || STYLE >= 11
   int detail_flag;
#endif /* DETAIL */
   int switch_size;
   int textqual;
   int swqual;
   int given_qualifier;
#ifdef NEST_TEXT
   int given_key = key;
#endif /* NEST_TEXT */
   char auxbuf [WORDSIZE];
   int auxa;
   int ea;
   char *cp;
   char tc;
   char tl = 0;

/* NB: The below flag values are hard-coded in acdc's dominor.c! */

   if (lptr == NULL) lptr = text_buf;
   locate_demands++;
   quip_flag =     key & QUIP_FLAG;
#if defined(DETAIL) || STYLE >= 11
   detail_flag =   key & DETAIL_FLAG;
#endif
#ifdef FULL
   full_flag =     key & FULL_FLAG;
#endif
   qual_flag =     key & QUAL_FLAG;
   var_qual_flag = key & VQUAL_FLAG;
   var_what_flag = key & VAR_FLAG;
   value_flag =    key & VALUE_FLAG;
   voc_flag =      key & VOC_FLAG;
#ifdef OBSOLETE
#ifdef FULL
   if (bitest (STATUS, TERSE))
      full_flag = 0;
   else if (bitest (STATUS, FULL) ||
      (bitest (STATUS, TERSE) == 0 && bitest (STATUS, BRIEF) == 0))
         full_flag = 1;
   else if (bitest (STATUS, BRIEF))
      full_flag = bitest (value [HERE], BEENHERE) ? 0 : 1;
#endif /* FULL */
#endif /* OBSOLETE */

   if (var_what_flag)
   {
      int tmp = value [what];
#if STYLE >= 11
      if ((what != ARG1 && what != ARG2 && what != ARG3) ||
         (tmp != BADWORD && tmp != AMBIGWORD && tmp != AMBIGTYPO &&
         tmp != SCENEWORD && tmp != BADSYNTAX))
#else
#  if STYLE == 1
      if ((what != ARG1 && what != ARG2 && what != ARG3) || tmp != BADWORD)
#  else
      if ((what != ARG1 && what != ARG2 && what != ARG3) ||
         (tmp != BADWORD && tmp != AMBIGWORD))
#  endif
#endif /* STYLE */
            what = tmp;
      else
            voc_flag = 1;
   }

   given_qualifier = qualifier;
   if (var_qual_flag &&
      ((qualifier != ARG1 && qualifier != ARG2 && qualifier != ARG3) || 
         value_flag))
            qualifier = value [qualifier];

   if (what > LLOC || voc_flag)
   {
      if (voc_flag && (what == ARG1 || what == ARG2))
      {
         if (what == ARG1)
            cp = arg1_word;
         else if (what == ARG2)
            cp = arg2_word;
         else
            cp = arg3_word;
         while (*cp != '\0')
            outchar (*cp++);
         return;
      }
      ta = textadr [what];
   }
   else if (what >= FLOC)
   {
#if defined(TERSE) && defined(FULL) && defined(BEENHERE)
      if (bitest (STATUS, TERSE) || 
         (! bitest (STATUS, FULL) && bitest (what, BEENHERE)))
            ta = brief_desc [what];
      else
#endif
         ta = long_desc [what];
   }
   else if (what >= FOBJ)
   {
#if defined(DETAIL) && STYLE == 10
      if (bitest (STATUS, DETAIL))
         ta = detail_desc [what];
      else
#endif
#if STYLE >= 11
      if (detail_flag)
         ta = detail_desc [what];
      else
#endif
      if (location [what] == INHAND)
      {
         brief_flag = 1;
         ta = brief_desc [what];
      }
      else
         ta = long_desc [what];
   }

   if ((tc = get_char (ta)) == '\0') goto shutup;

#define RANDOM_TEXT      1
#define INCREMENTAL_TEXT 2
#define CYCLIC_TEXT      4
#define ASSIGNED_TEXT    8
#define TIED_TEXT       16

   if (what >= FTEXT)
   {
      int twat = 2 * (what - FTEXT);
      textqual = qualifier;
      if (text_info [twat] == RANDOM_TEXT)
      {
         textqual = text_info [twat + 1];
         if (textqual <= 1)
            textqual = 0;
         else
         {
            textqual = irand (textqual - 1);
            if (textqual >= value[what])
               textqual++;
         }
         value[what] = textqual;
      }
      else if (text_info [twat] == INCREMENTAL_TEXT)
      {
         textqual = value [what];
         if (value [what] < text_info [twat + 1] - 1)
            value[what]++;
      }
      else if (text_info [twat] == CYCLIC_TEXT)
      {
         textqual = value [what];
         if (value [what] < text_info [twat + 1] - 1)
            value[what]++;
         else
            value [what] = 0;
      }
      else if (text_info [twat] == ASSIGNED_TEXT)
         textqual = value [what];
      else if (text_info [twat] == TIED_TEXT)
         textqual = value [value [what]];
      else if (qualifier == ARG2 && value [ARG2] >= 0)
         textqual = value [value [qualifier]];
   }

   if (!qual_flag)
      qualifier = (what <= LLOC) ? value [what] : what;
   
   while (tc != '\0')
   {
      tl = tc;
#ifdef NEST_TEXT
      if (tc == NEST_TEXT)
      {
         nested_say (ta + 1, given_key, qualifier);
         tc = get_char (++ta);
         continue;
      }
#endif /* NEST_TEXT */
#ifdef DWARVEN
      if (tc == DWARVISH)
      {
         extra_dwarvish ^= 1;
         goto next_char;
      }
#endif /* DWARVEN */
      if (tc == SW_START)
      {
         switch_size = get_char (++ta);
         ea = ta + 2 * switch_size -1;
         swqual = (what >= FTEXT) ? textqual : qualifier;
#if STYLE == 1
         if (brief_flag)
            swqual = (swqual == 1 || switch_size == 1) ? 0 : 1;
         if (swqual <= 0 || (swqual == 1 && what >= FTEXT))
#else /* STYLE > 1 */
#  if STYLE >= 11
         if (swqual >= switch_size && what >= FTEXT &&
            text_info[2 * (what - FTEXT)] == CYCLIC_TEXT)
               swqual %= switch_size;
#  endif
         if (swqual <= 0)
#endif /* STYLE */
            ta = ea + 1;
         else
         {
#if STYLE == 1
            ta = ta - 1 + 2 *
               ((swqual > switch_size) ? switch_size - 1 : swqual - 1);
            if (what < FTEXT) ta += 2;
#else /* STYLE > 1 */
            ta = ta - 1 + 2 *
               ((swqual >= switch_size) ? switch_size - 1 : swqual);
#endif /* STYLE */
            offset = get_char (ta + 1);
            if (offset < 0)
               offset  += 256;
            ta = ea + 256 * get_char (ta) + offset + 1;
         }
         offset = get_char (ea + 1);
         if (offset < 0)
            offset += 256;
         ea = ea + 256 * get_char (ea) + offset + 1;
         tc = get_char (ta);
      }
      else if (tc == SW_BREAK)
      {
         tc = get_char (ta = ea++);
         if (tc == SW_BREAK)
            goto next_char;
      }
#if STYLE >= 11
      else if (tc == HOLDER || tc == VHOLDER)
      {
         if (upcase) upcase = 2;
         if (value_flag || tc == VHOLDER)
#else
      else if (tc == HOLDER)
      {
         if (value_flag)
#endif
         {
            sprintf (auxbuf, "%d", qualifier);
            cp = auxbuf - 1;
            while (*(++cp) != '\0')
               outchar (*cp);
            goto next_char;
         }
#if STYLE >= 11
         else if (qualifier == ARG1 || qualifier == ARG2 || qualifier == ARG3)
         {
            if (qualifier == ARG1)
               cp = arg1_word;
            else if (qualifier == ARG2)
               cp = arg2_word;
            else
               cp = arg3_word;
            while (*cp != '\0')
               outchar (*cp++);
            goto next_char;
         }
         else if (qualifier == ORIG)
         {
            cp = orig;
            while (*cp != '\0')
               outchar (*cp++);
            goto next_char;
         }
#else /* STYLE < 11 */
         else if (qualifier == ARG1 || qualifier == ARG2)
         {
            cp = (qualifier == ARG1 ? arg1_word : arg2_word);
            while (*cp != '\0')
               outchar (*cp++);
            goto next_char;
         }
#endif /* STYLE */
         else
         {
            index = (var_qual_flag && given_qualifier <= LLOC) ?
               given_qualifier : qualifier;
            auxa = ta;
            locate_demands++;
            ta = textadr [index];
            tc = get_char (ta);
            if (tc == '!')
               tc = get_char (++ta);

            while (tc != '\0')
            {
               outchar (tc);
               tc = get_char (++ta);
            }
            ta = auxa;
         }
      }
      else
         outchar (tc);
next_char:
      tc = get_char (++ta);
   }

shutup:
   if (quip_flag) longjmp (loop_back, 1);
   return;
}

/*===========================================================*/

#ifdef __STDC__
void outchar (char text_char)
#else
void outchar (text_char)
char text_char;
#endif
{
   if (text_len == text_buf_len - 8)
   {
      text_buf_len += 1024;
      text_buf = advrealloc (text_buf, text_buf_len);
      lptr = text_buf + text_len;
   }

#ifdef TAG_START
   
   if (text_char == TAG_START)
   {
      html_tag = 1;
      if (cgi)
      {
         *lptr++ = '<';
         text_len++;
      }
      return;
   }
   else if (html_tag)
   {
      if (text_char == TAG_END)
      {
         html_tag = 0;
         if (cgi)
         {
            text_len++;
            *lptr++ = '>';
         }
         return;
      }
   }
   if (html_tag && !cgi)
      return;

#endif /* TAG */

   if (text_char == '\n')
   {
      eol_count++;
      if (eol_count > 2)
      return;
   }
#ifdef BLOCK_END
   else if (text_char && text_char != BLOCK_END && eol_count)
#else
   else if (text_char && eol_count)
#endif /* BLOCK_END */
      eol_count = 0;

#if STYLE >= 11
   if (isalpha (text_char))
   {
      if (upcase == 2)
         text_char = toupper (text_char);
      upcase = 0;
   }
   else if (isdigit (text_char))
      upcase = 0;
   else if (strchr (".!?", text_char))
      upcase = 1;
#endif /* STYLE >= 11 */

   if (cgi && (text_char == '<' || text_char == '>'))
   {
      *lptr++ = '&';
      *lptr++ = (text_char == '<') ? 'l' : 'g';
      *lptr++ = 't';
      *lptr = ';';
      text_len += 4;
   }
   else
   {
      text_len++;
      *lptr = text_char;
   }
#ifdef DWARVEN
   if (value [DWARVEN] || extra_dwarvish)
      shift_up (lptr);
#endif /* DWARVEN */
   lptr++;
   return;
}

#ifdef UNDO

/*===========================================================*/

#ifdef __STDC__
void save_changes (void)
#else
void save_changes ();
#endif
{
   char *iptr;
   char *optr;
   unsigned int cnt;
   int some = 0;

   if (value [ARG1] <= BADWORD || value [ARG2] <= BADWORD ||
#ifdef ADVCONTEXT
       value [ADVCONTEXT] > 1 ||  cgi == 'y' ||
#endif
       value [ARG1] == UNDO || value [ARG1] == REDO)
          return;
   if (dptr > edptr)
      dptr = edptr;
   if (diffs == NULL)
   {
      if ((diffs = (unsigned char *)malloc(8192)) == NULL)
      {
         printf ("GLITCH: Unable to allocate diffs array!\n");
         return;
      }
      else
      {
         diffsz = 8192;
         memset (diffs, '\0', 4);
         edptr = dptr = diffs + 4;
      }
   }
   else
   {
      for (cnt = 0, optr = image, iptr = IMAGE; cnt < sizeof (IMAGE); 
         cnt++, optr++, iptr++)
      {
         if (*optr != *iptr && 
          ! ((cnt >= ARG1*sizeof(int) && cnt < (ARG1 + 1)*sizeof(int)) ||
             (cnt >= ARG2*sizeof(int) && cnt < (ARG2 + 1)*sizeof(int)) ||
             (cnt >= STATUS*sizeof(int) && cnt < (STATUS + 1)*sizeof(int))))
         {
            if (dptr - diffs + 8 >= diffsz)
            {
               int doffset = dptr - diffs;
               if ((diffs = (unsigned char *)realloc(diffs, diffsz + 8192)) == NULL)
               {
                  printf ("GLITCH: Unable to re-allocate diffs array!\n");
                  return;
               }
               diffsz += 8192;
               dptr = diffs + doffset;
            }
            if (cnt || cgi < 'x')       /* ??? */
            {
               *dptr++ = cnt / 256;
               *dptr++ = cnt % 256;
               *dptr++ = *optr;
               *dptr++ = *iptr;
               some = 1;
            }
         }
      }
      if (some)
      {
         for (cnt = 0; cnt < 4; cnt++)
            *dptr++ = '\0';
         edptr = dptr;
      }
   }
   memcpy (image, IMAGE, sizeof (IMAGE));
}
#endif /* UNDO */

/*===========================================================*/

#ifdef __STDC__
void close_files (void)
#else
void close_files ()
#endif
{
   if (com_file)
      fclose (com_file);
      
   if (log_file)
   {
      int cnt;
      fprintf (log_file, "\nINTERACT\n");
      for (cnt = 0; cnt < 39; cnt++)
         fprintf (log_file, "*-");
      fprintf (log_file, "*\n");
      fclose (log_file);
   }
#if STYLE >= 11
   if (word_buf) free (word_buf);
#endif
}

/*===========================================================*/

#ifdef __STDC__
void getinput (char *inbuf, int insize)
#else
void getinput (inbuf, insize)
char *inbuf;
int insize;
#endif
{
   char *cptr;
#ifdef DWARVEN
   extra_dwarvish = 0;
#endif
#ifdef ADVCONTEXT
   if (text_len > 3 && value [ADVCONTEXT] == 0 && 
      !query_flag && cgi != 'y' && cgi <= 'b')
#else /* !ADVCONTEXT */
   if (text_len > 3 && !query_flag && !quitting)
#endif /* ADVCONTEXT */
   {
      FILE *adl;
      char name[64];
      special (998, &value [0]);
      sprintf (name, "%s.adl", CGINAME);
      if ((adl = fopen(name, WMODE)) != NULL)
      {
         char *cptr = text_buf;
         int len = strlen (RESTORING);
         while (*cptr == '\n')
            cptr++;
         if (strncmp (cptr, RESTORING, len) == 0)
            cptr += len;
         fwrite(cptr, 1, text_len - (cptr - text_buf), adl);
         fclose (adl);
      }
   }
#ifdef QT
   if (value[ADVCONTEXT] > 2)
   {      
      outchar(1);
      outchar('?');
   }
#endif /* QT */
#ifndef READLINE
   if (!*text_buf && !quitting)
      PRINTF ("\n? ");
#endif /* READLINE */
   if (cgi != 'b' && *text_buf)
   {
      outbuf (0);
      scrchk (NULL);
   }
#if STYLE >= 11
   upcase = 1;
#endif /* STYLE >= 11 */
   *inbuf = '\0';

#ifdef ADVCONTEXT
#  ifdef PROMPTED
   bitmod ('s', ADVCONTEXT, PROMPTED);
#  endif
   if (cgi == 'x' || cgi == 'z')
   {
      special (998, &value [0]);
      if (word_buf) free (word_buf);
      exit (value [ADVCONTEXT]);
   }
   if (cgi == 'y')
   {
      strncpy (inbuf, cgicom, insize - 1);
      cgi = 'z';
      return;
   }
#endif
   query_flag = 0;
   if (com_file)
   {
      char mybuf [256];
      char lastbuf [256];
      while (1)
      {
         if (fgets (mybuf, insize, com_file) == NULL || 
             strncmp (mybuf, "INTERACT", 8) == 0)
         {
            fclose (com_file);
            com_file = NULL;
            break;
         }
         cptr = mybuf + strlen (mybuf);
         while (*(cptr - 1) == '\n' || *(cptr - 1) == '\r') cptr--;
         *cptr = '\0';
         if (strncmp (mybuf, "REPLY: ", 7) == 0)
         {
            strncpy (inbuf, mybuf + 7, insize);
            if (!cgi)
               printf ("%s%s%s\n", lastbuf, inbuf, compress ? "" : "\n");
            else
            {
               char prmpt;
               while (lptr > text_buf && 
                  (*(lptr - 1) == ' ' || *(lptr - 1) == NBSP))
                     lptr--;
               if (*(lptr - 1) == '\n')
               {
                  if (! compress)
                     lptr += sprintf (lptr, "%cbr /%c", TAG_START, TAG_END);
                  prmpt = '?';
               }
               else
                  prmpt = *(--lptr);
                  
               lptr += sprintf (lptr, 
                  "%cspan class=\"query\"%c%c %s%c/span%c",
                     TAG_START, TAG_END, prmpt, mybuf + 7, TAG_START, TAG_END);
               lptr += sprintf (lptr,
                  "%c/div%c%c%cdiv name=\"chunk\"%c",
                     TAG_START, TAG_END, compress ? ' ' : '\n', TAG_START, TAG_END);
            }
            break;
         }
         else
         {
            int len = strlen (mybuf);
            if (len > 0)
            {
               strcpy (lastbuf, mybuf);
               if (*(lastbuf + len - 1) != ' ')
                  strcat (lastbuf, " ");
            }
         }
      }
   }
#ifdef BROWSER
   if (com_file == NULL && cgi == 'b')
   {
      outbuf (0);
      (void) get_command_from_browser (inbuf);
   }
#endif /* BROWSER */
   if (com_file == NULL && cgi != 'b')
#ifdef READLINE
   {
      if (cgi || cps)
         fgets (inbuf, insize, stdin);
      else
      {
         char *rdl = readline (prompt_line);
         if (!compress) putchar ('\n');
         memcpy (inbuf, rdl, insize);
         add_history (rdl);
         free (rdl);
         *(inbuf + insize - 1) = 0;
      }
   }
   if (log_file && cgi != 'b')
   {
      fputs (prompt_line, log_file);
      fflush (log_file);
   }
#else /* ! READLINE */
#  ifdef GLK
      glkgets (inbuf, insize - 1);
#  else
      fgets (inbuf, insize, stdin);
#  endif
/*   if (*inbuf == '\0' || *inbuf == '\n')
      PRINTF("\n? "); */
#endif /* READLINE */
#ifdef ADVCONTEXT
   if (cgi == 'x') cgi = 'z';
#endif
   *(inbuf + insize - 1) = '\0';
   *(inbuf + insize - 2) = '\n';
   cptr = inbuf;
   while (*cptr)
   {
      if (strchr ("\"\'", *cptr))
         *cptr = ' ';
      cptr++;
   }
   if (log_file)
      fprintf (log_file,"\nREPLY: %s\n", inbuf);
}

/*===========================================================*/

#ifdef __STDC__
void advcpy (char *word_buff, int word_addr)
#else
void advcpy (word_buff, word_addr)
char *word_buff;
int word_addr;
#endif
{
   int wlen;

   if (get_char (word_addr) == '!')
      word_addr++;
   for (wlen = 1; wlen <= 20; wlen++)
      if ((*word_buff++ = get_char (word_addr++)) == '\0')
         return;
}

/*===========================================================*/

#ifdef __STDC__
void fake (int which_arg, int refno)
#else
void fake (which_arg, refno)
int which_arg;
int refno;
#endif
{
   int word;
   
   if (textadr[refno])
   {
      advcpy (which_arg == 1 ? arg1_word : arg2_word, textadr [refno]);
      return;
   }
   for (word = 0; word < VOCAB_SIZE; word++)
   {
      if (voc_refno [word] == refno)
      {
         advcpy (which_arg == 1 ? arg1_word : arg2_word, 
            voc_word [word]);
         return;
      }
   }
   strcpy (which_arg == 1 ? arg1_word : arg2_word, "*GLITCH*");
   return;
}

/*===========================================================*/

#ifdef __STDC__
void default_to (int key, int place, int type)
#else
void default_to (key, place, type)
int key;   /* 0 is "default", 1 is initial "doall", 2 is continued "doall" */
int place;
int type;
#endif
{
   int index;
   int fits;
#ifdef ALL
   int first;
#endif /* ALL */

   if (key == 0 && value [STATUS] != 1) return;
   fits = -1;
#ifdef ALL
   first = (key == 2) ? value_all + 1 : FOBJ;
   if (first > LOBJ)
      goto failed;
   for (index = first; index <= LOBJ; index++)
   {
#if STYLE >= 11 && defined (EXCEPT)
      if (except_count > 0)
      {
         int i, j;
         j = 0;
         for (i = 0; i < except_count; i++)
            if (index == except [i])
            { 
               except [i] = except [except_count - 1];
               except_count--;
               j = 1;
               break;
            }
         if (j)
            continue;
      }
#endif /* STYLE == 11 */
#else /* not ALL */
   for (index = FOBJ; index <= LOBJ; index++)
   {
#endif /* ALL */
#ifdef SCHIZOID
      if ((location [index] == place || (place != INHAND &&
         bitest (index, SCHIZOID) && location [index] + 1 == place)) &&
#else
      if (location [index] == place &&
#endif
            (type < 0 || bitest (index, type)))
      {
         if (fits >= 0) return;   /* Can't happen if key > 0 */
         fits = index;
         if (key) break;
      }
   }
   if (fits >= 0)
   {
      value [ARG2] = fits;
      advcpy (arg2_word, textadr [fits]);
#ifdef DWARVEN
      if (value [DWARVEN] || extra_dwarvish)
      {
         char *wp = arg2_word;
         while (*wp)
            shift_up (wp++);
      }
#endif /* DWARVEN */
      arg2_word [19] = '\0';
      value [STATUS] = 2;

#ifdef IT
      value [IT] = fits;
#endif

#ifdef ALL
      if (key > 0)
         value_all = fits;

      if (key == 1)
      {
         location_all = place;
         type_all = type;
      }
#endif /* ALL */
      return;
   }

#ifdef ALL
failed:
/*   if (key == 1 && value [STATUS] == 2)
      value [STATUS] = 1; */      /* Force the please clarify response */
   if (key > 0)
      value_all = 0;
#endif /* ALL */
   return;
}

#if STYLE >= 11
#ifdef TYPO

/*===========================================================*/

#ifdef __STDC__
void report_typo (int ovoc, int olen)
#else
void report_typo (ovoc, olen)
int ovoc;
int olen;
#endif
{
   char save_char;

   strncpy (orig, tp [tindex], WORDSIZE);
#ifdef DWARVEN
   if (value [DWARVEN]) shift_down (orig, WORDSIZE);
#endif
   say (QUAL_FLAG, TYPO, ORIG);
   advcpy (orig, ovoc);
   save_char = *(orig + olen);
   *(orig + olen) = '\0';
   say (QUAL_FLAG, TYPO, ORIG);
   *(orig + olen) = save_char;
   if ((unsigned int)olen >= strlen (orig)) 
      value [TYPO]++;
   else
      say (QUAL_FLAG, TYPO, ORIG);
   say (0, TYPO, 0);
   *orig = '\0';
   PRINTF("\n\n");
}
#endif

/*===========================================================*/

#ifdef __STDC__
void find_word (int *type, int *refno, int *tadr, int which_arg, int gripe)
#else
void find_word (type, refno, tadr, which_arg, gripe)
int *type;
int *refno;
int *tadr;
int which_arg;
int gripe;
#endif
#else
/*===========================================================*/

#ifdef __STDC__
void find_word (int *type, int *refno, int *tadr, int which_arg)
#else
void find_word (type, refno, tadr, which_arg)
int *type;
int *refno;
int *tadr;
int which_arg;
#endif
#endif
{
   int bottom, middle, top;
   int old_refno;
#if STYLE > 1
   int old_type;
   int old_tadr;
#endif /* STYLE */
#if STYLE >= 11
   int ovoc;
   int olen;
#endif
   int exact;
   int va;
   char *wp;
   int ra;
   char myword [WORDSIZE];
  
   *type = -1;
   strncpy (myword, 
      which_arg == 0 ? orphan_word : tp [tindex], WORDSIZE);
#ifdef DWARVEN
   if (value [DWARVEN]) shift_down (myword, WORDSIZE);
#endif /* DWARVEN */
   if (*myword == '\0')
   {
      *type = NOISE;
      goto done;
   }
   bottom = -1;
   top = VOCAB_SIZE + 1;
   while (top > bottom + 1)
   {
      locate_demands++;
      middle = (bottom + top) / 2;
      if (get_char (va = voc_addr [middle]) == '!')
         va++;
      wp = myword;
      while (get_char (va) == *wp)
         if (*wp != '\0')
         {
            wp++;
            va++;
         }
         else
            break;

      if (get_char (va) < *wp && *wp != '\0')
         bottom = middle;
      else
         top = middle;
   }
   *refno = BADWORD;
   bottom++;
   top = VOCAB_SIZE;
   old_refno = BADWORD;

   while (bottom < top)
   {
      wp = myword;

      if (get_char (va = voc_addr [bottom]) == '!')
      {
         va++;
         exact = 1;
      }
      else
         exact = 0;
      ra = va;
      while (*wp == get_char (ra))
      {
         if (*wp == '\0')
            break;
         else
         {
            ra++;
            wp++;
         }
      }
      if (*wp != '\0') break;
#if STYLE > 1
      if (!exact || (exact && get_char (ra) == '\0'))
#else
      if (get_char (ra) == '\0' || ra - va >= 5)
#endif
      {
         *type = voc_type [bottom];
         *refno = voc_refno [bottom];
         *tadr = voc_word [bottom];
         if (get_char (ra) == '\0')
#if STYLE == 1
         {
            if (ra - va <= 2)
            {
               wp = myword;
               while (++bottom < top)
                  if (*refno == voc_refno [bottom] &&
                     *wp == get_char (voc_addr [bottom]))
                        *tadr = voc_word [bottom];
            }
            goto done;
         }
#else /* STYLE > 1 */
            goto done;
         if (old_refno != BADWORD && *refno != old_refno)
#if STYLE >= 11
         {
#define REF(X) (which_arg == 1 ? X > LOBJ : (X <= LOBJ && \
                   (location [X] == INHAND || location [X] == value [HERE])))
            int oref = REF (old_refno);
            int nref = REF (*refno);
            
            if ((nref && oref) || (!nref && !oref))
               { *refno = AMBIGWORD; goto done; }
            if (!nref && oref)
            { 
               *refno = old_refno; 
               *tadr = old_tadr;
               *type = old_type;
               bottom++; 
               continue;
            }
            if (nref && !oref)
               { old_refno = *refno; if (get_char (va) == '\0') break; }
         }
#endif /* STYLE >= 11 */
         old_tadr = *tadr;
         old_type = *type;
#endif /* STYLE */
         old_refno = *refno;
         if (get_char (va) == '\0') break;
      }
      bottom++;
   }

#if STYLE >= 11

   if (*refno == BADWORD && amatch > 0)
   {
      char *bp = NULL;
      int ba = 0;
      old_refno = -1;
      for (bottom = 0; bottom < VOCAB_SIZE; bottom++)
      {
         if (get_char (va = voc_addr [bottom]) == '!')
            continue;
         wp = myword;
         while (*wp == get_char (va))
            { wp++; va++; }
         if (*wp == '\0' && get_char (va + 1) == 0 &&
               voc_type [bottom] != NOISE)
         {
            if (old_refno >= 0 && 
               voc_refno [old_refno] != voc_refno [bottom])
                  {old_refno = AMBIGTYPO; break;}
            old_refno = bottom; 
            ovoc = voc_addr [bottom];
            olen = va - ovoc;
            continue;
         }
         bp = wp;
         ba = va;
         if (*wp == get_char (va + 1) && *(wp + 1) == get_char (va))
         {
            va += 2;
            wp += 2;
            while (*wp && *wp == get_char (va))
               {wp++; va++;}
            if (*wp == '\0' && get_char (va) == '\0' && 
               voc_type [bottom] != NOISE)
            {
               if (old_refno >= 0 && 
                  voc_refno [old_refno] != voc_refno [bottom])
                     {old_refno = AMBIGTYPO; break;}
               old_refno = bottom; 
               ovoc = voc_addr [bottom];
               olen = va - ovoc;
               continue;
            }
           wp = bp; 
           va = ba;
         }
         if (*(wp + 1) == get_char (va + 1))
         {
            wp++;
            va++;
            while (*wp && *wp == get_char (va))
               {wp++; va++;}
            if (*wp == '\0' && get_char (va) == '\0' && 
               voc_type [bottom] != NOISE)
            {
               if (old_refno >= 0 && 
                  voc_refno [old_refno] != voc_refno [bottom])
                     {old_refno = AMBIGTYPO; break;}
               old_refno = bottom; 
               ovoc = voc_addr [bottom];
               olen = va - ovoc;
               continue;
            }
            wp = bp;
            va = ba;
         }
         if (*wp == get_char (va + 1))
         {
            va++;
            while (*wp && *wp == get_char (va))
               {wp++; va++;}
            if (*wp == '\0' && get_char (va) == '\0' && 
               voc_type [bottom] != NOISE)
            {
               if (old_refno >= 0 && 
                  voc_refno [old_refno] != voc_refno [bottom])
                     {old_refno = AMBIGTYPO; break;}
               old_refno = bottom; 
               ovoc = voc_addr [bottom];
               olen = va - ovoc;
               continue;
            }
            wp = bp;
            va = ba;
         }
         if (*(wp + 1) == get_char (va) || *(wp + 1) == '\0')
         {
            wp++;
            while (*wp && *wp == get_char (va))
               {wp++; va++;}
            if (*wp == '\0' && get_char (va) == '\0' && 
               voc_type [bottom] != NOISE)
            {
               if (old_refno >= 0 && 
                  voc_refno [old_refno] != voc_refno [bottom])
                     {old_refno = AMBIGTYPO; break;}
               old_refno = bottom; 
               ovoc = voc_addr [bottom];
               olen = va - ovoc;
               continue;
            }
         }
      }
      if (old_refno == AMBIGTYPO)
         *refno = AMBIGTYPO;
      else if (old_refno >= 0)
      {
#ifdef TYPO
         *type = voc_type [old_refno];
         *refno = voc_refno [old_refno];
         *tadr = voc_word [old_refno];
         if (gripe)
            report_typo (ovoc, olen);
#else
         *refno = BADWORD;
#endif
      }
   }
#endif /* STYLE >= 11 */
done:
#if STYLE >= 11
      if (which_arg > 1 && (*refno == BADWORD || *refno == AMBIGTYPO) && 
         btfind (word_buf, tp [tindex]))
            value [ARG2] = SCENEWORD;
#endif /* STYLE >= 11 */

   if (which_arg == 1)
      wp = arg1_word;
   else if (which_arg == 2)
      wp = arg2_word; 
   else
      wp = arg3_word;
   
   if (*refno <= BADWORD)
   {
      strncpy (wp, tp [tindex], WORDSIZE);
      if (which_arg <= 2 && strlen (tp [tindex]) > 16)
         strcpy (long_word, tp[tindex]);
   }
   else
   {
      advcpy (wp, *tadr);
#ifdef DWARVEN
      if (value [DWARVEN] || extra_dwarvish)
      {
         char *cp = wp;
         while (*cp)
            shift_up (cp++);
      }
#endif /* DWARVEN */
   }

#if defined(FDIR) && defined(LDIR) 
   if ((*refno > FDIR && *refno < LDIR) && separator [tindex + 1] == ' ')
      separator [tindex + 1] = ';';
#endif
   return;
}

/*===========================================================*/

#ifdef __STDC__
void parse (void)
#else
void parse ()
#endif
{
   char *cptr, *lptr;
   char sep;
   
   for (cptr = comline; *cptr; cptr++)
      *cptr = tolower (*cptr);
   cptr = lptr = comline;
   while (*cptr == ' ' || *cptr == ',' || *cptr == ';' || 
         (*cptr == '.' && *(cptr + 1) != '.' && *(cptr + 1) != '/' &&
                          *(cptr + 1) != '\\'))
      cptr++;
   while (*cptr)
   {
      if (*cptr == '.' && (*(cptr + 1) == '.' || *(cptr + 1) == '/' ||
                           *(cptr + 1) == '\\'))
      {
         *lptr++ = *cptr++;
         continue;
      }
      
      while (*cptr && *cptr != ' ' && *cptr != ',' && *cptr != ';' && 
             *cptr != '.' && *cptr != '\n')
         *lptr++ = *cptr++;
      sep = ' ';
      while (*cptr == ' ' || *cptr == ',' || *cptr == ';' || 
            *cptr == '\n' || 
            (*cptr == '.' && *(cptr + 1) != '.' && *(cptr + 1) != '/' &&
                             *(cptr + 1) != '\\'))
      {
         if (*cptr == '.')
            *cptr = ';';
         if (sep == ' ' || *cptr == '\n')
            sep = *cptr;
         else if ((sep == ' ' || sep == ',') &&
                  (*cptr == ';' || *cptr == '\n'))
            sep = *cptr;
         cptr++;
      }
      if (*cptr)
         *lptr++ = sep;
   }
   if (sep != '\n')
      *lptr++ = '\n';
   *lptr++ = '\0';
   *lptr = '\0';
   tindex = 0;
   cptr = comline;
   while (*cptr)
   {
      if (*cptr == '\n')
         break;
      tp [tindex] = cptr;
      while (*cptr && *cptr != ' ' && *cptr != ',' && 
         *cptr != ';' && *cptr != '\n')
            cptr++;
      separator [tindex + 1] = *cptr;
      *cptr++ = '\0';
/* Convert ANDs into comma separators *unless* approximate matching is
 * suppressed, in which case we just discard the AND -- the latter
 * is an Adv770 specific fudge to allow a sensible response to
 * REST AND CONTEMPLATE in the "little joke" stage of the game.
 */
      if (strcmp (tp [tindex], AND) == 0)
         separator [tindex] = value [STATUS] < 90 ? ',' : ' ';
      else if (strcmp (tp [tindex], THEN) == 0)
         separator [tindex] = ';';
      else
         tindex++;
   }
   tp [tindex] = NULL;
   separator [tindex] = '\n';
   tindex = 0;
   value [ARG1] = -1;
   value [ARG2] = -1;
   value [STATUS] = 0;

   return;      
}

/*===========================================================*/

#ifdef __STDC__
void input (int textref)
#else
void input (textref)
int textref;
#endif
{
   int type;
   int refno;
   int tadr;
   int continuation = 0;

   if (textref == -1)
      goto get_arg1;

#if STYLE >= 11
   if (value[STATUS] == -1 && value [ARG3] == -1)
   {
      printf ("\nSorry... This game does not support command line restore.\n\n");
      exit (1);
   }
#endif /* STYLE */
   if (value [STATUS] < 90 || value [STATUS] >= LTEXT)
      amatch = 1;
   else if (value [STATUS] == 99)
      amatch = 0;
   else
      amatch = -1;
      
#if STYLE >= 11
#if defined (ALL) && defined (EXCEPT)
   if (value_all == 0)
      except_count = 0;
#endif
   *orig = '\0';
#endif
   *bitword (ARG1) = -1;        /* Just in case! */
   *bitword (ARG2) = -1; 

#ifdef ALL
   if (value_all)
   {
      default_to (2, location_all, type_all);
      if (separator [tindex] == ',')
         arg1 = value [ARG1];
      if (value_all)
         return;
   }
   else
   {
      arg1 = value [ARG1];
      arg2 = value [ARG2];
   }
#else /* not ALL */
   arg1 = value [ARG1];
   arg2 = value [ARG2];
#endif /* ALL */

#ifdef PLSCLARIFY
   if (bitest ((STATUS), (PLSCLARIFY)))
   {
      orphan = arg1;
      strncpy (orphan_word, arg1_word, 20);
   }
#ifdef ADVCONTEXT
   else if (cgi > 'b' && *orphan_word && orphan == 0)
      find_word (&type, &orphan, &tadr, 0, 0);
#endif
   else
   {
      orphan = 0;
      *orphan_word = '\0';
   }
   bitmod ('c', (STATUS), (PLSCLARIFY));
#endif /* PLSCLARIFY */
      
   continuation = (tindex != 0 && tp [tindex] && separator [tindex] == ',');
   if (separator [tindex] == ';')
   {
      value [ARG1] = -1;
      value [ARG2] = -1;
      outchar ('\n');
   }
#  ifdef PROMPTED
   bitmod ('c', ADVCONTEXT, PROMPTED);
#  endif

restart:

#ifdef UNDO
   if (value [UNDO_STAT] >= 0)
      save_changes ();
   else if (diffs && dptr > diffs)
      edptr = dptr = diffs;
#endif /* UNDO */

   if (tp [tindex] == NULL)
   {
      if (raw_comline [0] != '\0' && raw_comline [0] != '\n')
         strncpy (old_comline, raw_comline, 160);
      comline [0] = '\0';
      while (comline [0] == '\0' || comline [0] == '\n')
      {
#ifdef AGAIN
         done_again = 0;
#endif /* AGAIN */
#ifdef LOC_STATS
         printf ("\n(Locates: demanded %ld (+%ld), faults %ld (+%ld))",
            locate_demands, locate_demands - old_demands, 
            locate_faults, locate_faults - old_faults);
         old_demands = locate_demands;
         old_faults = locate_faults;
#endif /* LOC_STATS */
         scrchk (NULL);
         if (textref)
            say (0, textref, 0);
         if (! lptr) lptr = text_buf;
         getinput (comline, 160);
#ifdef vms
         putchar('\n');         /* VMS... sigh... */
#endif /* vms */
         strncpy (raw_comline, comline, 160);
#ifdef ADVCONTEXT
         if (value [ADVCONTEXT] && (*comline == '\n' || *comline == '\0'))
         {
            value [STATUS] = 0;
            value [ARG1] = -1;
            value [ARG2] = -1;
            return;
         }
#endif /* ADVCONTEXT */
      }
      scrchk (NULL);
      parse ();
   }

get_arg1:
   if (tp[tindex] == NULL)
      goto restart;
   
#if STYLE >= 11
   *orig1 = '\0';
   *orig = '\0';
   find_word (&type, &refno, &tadr, separator [tindex] == ',' ? 2 : 1, 1);
   strncpy (orig1, tp [tindex], WORDSIZE - 1);
#else
   find_word (&type, &refno, &tadr, separator [tindex] == ',' ? 2 : 1);
#endif
   tindex++;
   if (type == NOISE)
   {
      if (tp [tindex] == NULL) goto restart;
      if (separator [tindex] == ' ' || separator [tindex] == ',')
         goto get_arg1;
   }

#ifdef PLSCLARIFY
   if ((bitest (STATUS, PLSCLARIFY) && refno <= LLOC) || continuation)
   {
      value [ARG1] = arg1;
      value [ARG2] = refno;
      value [STATUS] = 2;
      goto got_command;
   }
#endif /* PLSCLARIFY */
   value [ARG1] = refno;
   value [STATUS] = 1;

get_arg2:
#if STYLE >= 11
   *orig2 = '\0';
#endif
   if (separator [tindex] == ' ' && tp [tindex])
   {
#if defined(FSPECIAL) && defined(LSPECIAL)
      int bmatch = amatch;
      if (amatch == 1)
         amatch = (value [ARG1] < FSPECIAL || value [ARG1] > LSPECIAL);
#endif
#if defined(FACT) && defined (LACT)
      if (value [ARG1] > FACT && value [ARG1] < LACT)
      {
         separator [tindex] = ';';
         goto got_command;
      }
#endif
#if STYLE >= 11
#ifdef SAY
      if (value [ARG1] == SAY)
      {
         int i = tindex;
         strcpy (say_buf, tp [i]);
         while (tp [i] && (separator[i + 1] == ' ' || separator[i + 1] == ','))
         {
            strcat (say_buf, " ");
            strcat (say_buf, tp [i + 1]);
            if (separator [i + 1] == ' ')
               separator [i + 1] = ',';
            i++;
         }
      }
#endif /* SAY */
      *orig = '\0';
      find_word (&type, &refno, &tadr, 2, 1);
      strncpy (orig2, tp [tindex], WORDSIZE - 1);
#else
      find_word (&type, &refno, &tadr, 2);
#endif
#if defined(FSPECIAL) && defined(LSPECIAL)
      amatch = bmatch;
#endif
      tindex++;
      if (type == NOISE)
         goto get_arg2;

      value [ARG2] = refno;
      value [STATUS] = 2;
   }

got_command:
   if (value [STATUS] == 1 && orphan)
   {
      if ((orphan > LOBJ && value [ARG1] < LOBJ) ||
          (orphan < LOBJ && value [ARG1] > LOBJ))
      {
         value [STATUS] = 2;
         value [ARG2] = orphan;
         strncpy (arg2_word, orphan_word, 20);
      }
      orphan = 0;
      *orphan_word = '\0';
   }

#ifdef AGAIN
   if ((value [ARG1] == AGAIN || value [ARG2] == AGAIN) && tindex > done_again)
   {
      if (procs[AGAIN] != procs[0]) /* If ACTION AGAIN exists, call it */
         (*procs[AGAIN])();         /* and if still AGAIN, go on to handle it */
   }
   if (value [ARG1] == AGAIN || value [ARG2] == AGAIN)
   {
      if (tp [done_again] == NULL)
         goto restart;
      if (done_again >= tindex)
         goto get_arg1;
      done_again = tindex;
      tindex -= value [STATUS] + 1;
      while (tindex > 0 && separator [tindex] != ';')
         tindex--;;
      if (tindex < 0)
      {
         if (*old_comline == '\n')
            goto restart;
         strncpy (comline, old_comline, 160);
         strncpy (raw_comline, old_comline, 160);
         parse ();
      }
      goto get_arg1;
   }
#endif /* AGAIN */

#if defined(FSPECIAL) && defined(LSPECIAL)
   if (value [STATUS] == 2 &&
       value [ARG2] >= FSPECIAL && value [ARG2] <= LSPECIAL &&
      (value [ARG1] < FSPECIAL || value [ARG1] > LSPECIAL))
   {
      special (29, &value[0]);   /* Swap the command words! */
      if (value [ARG2] >= BADWORD)
      {
         value [STATUS] = 1;
         value [ARG2] = -1;
      }
   }
#endif

   if (   value [ARG1] == BADWORD   || value [ARG2] == BADWORD
#if STYLE > 1
       || value [ARG1] == AMBIGWORD || value [ARG2] == AMBIGWORD
#endif
#if STYLE >= 11
       || value [ARG1] == AMBIGTYPO || value [ARG2] == AMBIGTYPO
#endif
                                                                 )
         tp [tindex] = NULL;    /* Forget rest of command */
#if STYLE < 11
   else if (value [STATUS] == 2 && (bitest (value [ARG2], VERB)) &&
      !(bitest (value [ARG1], VERB)))
#else /* STYLE >= 11 */
   else if (value [STATUS] == 2 && value [ARG2] != SCENEWORD && 
      (bitest (value [ARG2], VERB)) && !(bitest (value [ARG1], VERB)))
#endif /* STYLE */
   {
      int temp_val;
      temp_val = value [ARG1];
      value [ARG1] = value [ARG2];
      value [ARG2] = temp_val;
      strncpy (temp_word, arg1_word, 20);
      strncpy (arg1_word, arg2_word, 20);
      strncpy (arg2_word, temp_word, 20);
   }
   arg1_word [19] = '\0';
   arg2_word [19] = '\0';
#ifdef DWARVISH
   if (value[DWARVEN])
   {
      shift_down (arg1_word, WORDSIZE);
      shift_down (arg2_word, WORDSIZE);
   }
#endif
   if (separator [tindex] == ' ')
   {
#if STYLE >= 11 && defined(ALL) && defined (EXCEPT)
      if (tp [tindex])
      {
         find_word (&type, &refno, &tadr, 3, 0);
         if (refno == EXCEPT)
         {
            tindex++;
            while (refno >= 0 && 
               (separator [tindex] == ' ' || separator [tindex] == ','))
            {
               if (strcmp (tp [tindex], AND) != 0)
               {
                  find_word (&type, &refno, &tadr, 3, 1);
                  if (refno <= BADWORD)
                     break;
                  else if (refno > LOBJ)
                  {
                     refno = BADSYNTAX;
                     break;
                  }
                  else
                  {
                     advcpy (arg3_word, tadr);
                     except [except_count++] = refno;
                  }
 	       }
               tindex++;
            }
            value [ARG3] = -1;
            if (refno <= BADWORD)
               return;
         }
      }
#endif /* ALL and EXCEPT */
      if (tp [tindex] && strcmp (tp [tindex], AND) == 0 && 
         separator [tindex] == ' ')
            separator [++tindex] = ',';
      else if (separator[tindex] != ';')
      {
#if STYLE >= 11
         if (value [STATUS] > 1 && 
            (separator [tindex] == ' ' || separator [tindex] == ','))
               value [STATUS] = BADSYNTAX;
         if (tp [tindex])
            while (separator [++tindex] == ' ');
      }
   }
#ifdef AGAIN
   if (value [ARG1] == -AGAIN && value [STATUS] > 1)
   {
      value [ARG1] = AGAIN;
      value [STATUS] = BADSYNTAX;
   }
#endif /* AGAIN */
   if (value [STATUS] == BADSYNTAX || value [ARG1] > STATUS || 
       value [ARG2] > STATUS)       /* Bad command! Ignore rest of line */
          tp [tindex + 1] = NULL;
#else /* STYLE < 11 */
         while (separator [++tindex] == ' ');
      }
   }
#endif /* STYLE */
   if (value [STATUS] == 1)
      value [ARG2] = -1;
#if STYLE >= 11
   else if (value [STATUS] == BADSYNTAX)
   {
/*      value [ARG1] = -1; */
      value [ARG2] = -1;
   }
#endif
   return;
}

/*===========================================================*/

#ifdef __STDC__
int query (int textref)
#else
int query (textref)
int textref;
#endif
{
   char reply [10];
   char *rp;
   int which = 0;

   if (textref >= 0) say (0, textref, 0);
   else scrchk (NULL);

try_again:
   query_flag = 1;
   getinput (reply, 10);
   scrchk (NULL);
#ifdef DWARVEN
   if (value [DWARVEN])  shift_down (reply, 10);
#endif /* DWARVEN */
   rp = reply;
   if (*rp == '\0' || *rp == '\n') return (1);
   while (*rp == ' ') rp++;
   if (*rp == 'y' || *rp == 'Y') return (1);
   if (*rp == 'n' || *rp == 'N') return (0);
   if (*rp == 'q' || *rp == 'Q') return (0);
   if (which)
   {
      PRINTF ("(OK, smartass... I'll assume you mean YES - so there!)\n \n");
      return (1);
   }
   PRINTF ("Eh? Do me a favour and answer yes or no!\nWhich will it be? ");
   which = 1;
   goto try_again;
}

/*===========================================================*/

#ifdef __STDC__
void make_name (char *file_name, char *save_name)
#else
void make_name (file_name, save_name)
char *file_name;
char *save_name;
#endif
{
   char *cptr;                 /* Discardable pointer to file_name */

   strcpy (save_name, file_name);
   cptr = save_name;
   while (*cptr)
   {
      if (*cptr == '\n')
      {
         *cptr = '\0';            /* Strip off trailing \n, if any */
         break;
      }
#if defined (MSDOS) || defined (vms) || defined (_WIN32)
      if (*cptr == '.')
         *cptr = '-';     /* DOMESTOS doesn't like double suffixes */
#endif /* MSDOS or VMS or Windows */
      cptr++;
   }
   
#ifdef MSDOS
   *(save_name + 8) = '\0';
#else /* !MSDOS */
#if !defined(unix) && ! defined(__CYGWIN__)
   *(save_name + 16) = '\0';
#endif
#endif /* MSDOS */
   if (strcmp (save_name + strlen (save_name) - 4, ".adv") != 0)
      strcat (save_name, ".adv");
   return;
}

/*===========================================================*/

#ifdef __STDC__
int check_version (FILE *infile)
#else
int check_version (infile)
FILE *infile;
#endif
{
   int majvalg = 0;
   int minvalg = 0;
   int majvalt = 0;
   int minvalt = 0;
   char *gptr = GAME_ID;
   char tchr = fgetc (infile);
   
   while (1)
   {
      if (*gptr == '\0' && tchr == '\n') return (0);
      if (! isalnum (tchr) && ! isalnum(*gptr)) break;
      if (tchr != *gptr) return (1);
      gptr++;
      tchr = fgetc (infile);
   }
   while (*gptr && ! isdigit (*gptr)) gptr++;
   while (tchr != '\n' && ! isdigit (tchr)) tchr = fgetc (infile);
   if (tchr == '\n' && *gptr == '\0') return (0);
   while (isdigit (*gptr) || *gptr == '.')
   {
      if (*gptr == '.') { majvalg = minvalg; minvalg = 0; }
      else                minvalg = 10 * minvalg + *gptr - '0';
      gptr++;
   }
   while (isdigit (tchr) || tchr == '.')
   {
      if (tchr == '.') { majvalt = minvalt; minvalt = 0; }
      else               minvalt = 10 * minvalt + tchr - '0';
      tchr = fgetc (infile);
   }
   if (majvalg != majvalt) return (1);
   if (minvalg < minvalt) return (1);
   while (tchr != '\n') tchr = fgetc (infile);
   return (0);
}

/*===========================================================*/

#ifdef __STDC__
int memstore (int key)
#else
int memstore (key)
int key;
#endif
{
/* Key -1  check for existence of memory image
 * Key 0   true memory save
 * Key 1   true memory restore
 * Key 2   temporary memory save
 * Key 3   temporary memory restore
 */
#ifdef ADVCONTEXT
   FILE *memory_file = NULL;
#endif /* ADVCONTEXT */
   static char *image_base = NULL;   /* True memory save area */
   static char *image_temp = NULL;   /* Temp save over restore area */
   char *image_ptr;
#ifdef ADVCONTEXT
   char *fname = (char *)(key < 2 ? ".M.adv" : ".T.adv");
   int result = 1;
#endif /* ADVCONTEXT */

   if (key < 0)
   {
#ifdef ADVCONTEXT
      if (cgi > 'b')
      {
         if ((memory_file = fopen (fname, RMODE)) != NULL)
         {
            fclose (memory_file);
            result = 0;
         }
         return (result);
      }
#endif /* ADVCONTEXT */
      return (image_base ? 0 : 1);
   }

   image_ptr = key < 2 ? image_base : image_temp;
   if (key == 0 || key == 2)
   {
      if (image_ptr == NULL)
      {
         image_ptr = (char *) malloc (IMAGE_SIZE);
         if (image_ptr == NULL)
            return (1);
         if (key == 0)
            image_base = image_ptr;
         else
            image_temp = image_ptr;
      }
      memcpy (image_ptr, IMAGE, IMAGE_SIZE);
#ifdef ADVCONTEXT
      if (cgi > 'b')
      {
         if ((memory_file = fopen (fname, WMODE)) != NULL &&
            fwrite (image_base, 1, IMAGE_SIZE, memory_file) == IMAGE_SIZE)
               result = 0;
         if (memory_file)
            fclose (memory_file);
         return (result);
      }
#endif
      return (0);
   }
   else
   {
#ifdef ADVCONTEXT
      if (cgi > 'b')
      {
         if ((image_ptr = (char *) malloc (IMAGE_SIZE)) != NULL &&
             (memory_file = fopen (fname, RMODE)) != NULL &&
             (fread (image_ptr, 1, IMAGE_SIZE, memory_file)) == IMAGE_SIZE)
                result = 0;
         if (memory_file)
            fclose (memory_file);
         if (result)
            return (1);
      }
      else if (image_ptr  == NULL)
         return (1);
#else
      if (image_ptr  == NULL)
         return (1);
#endif /* ADVCONTEXT */
      memcpy (IMAGE, image_ptr, IMAGE_SIZE);      
      return (0);
   }
}

#if STYLE > 11
/*===========================================================*/

int get_pdata (int offset)
{
   int retval = -1;

   if ((gv_file = fopen (PERSISTENT_DATA, RMODE)) != NULL)
   {
      if (offset < 0)
         retval = 1;
      else if (fseek (gv_file, (long) offset, SEEK_SET) == 0)
         retval = fgetc (gv_file);
      fclose (gv_file);
   }
   return (retval);
}

/*===========================================================*/

int set_pdata (int offset, char val)
{
   int retval = 1;

   if ((gv_file = fopen (PERSISTENT_DATA, UMODE)) != NULL)
   {
      int len;
      fseek (gv_file, 0L, SEEK_END);
      len = ftell (gv_file);
      if (len > offset)
         fseek (gv_file, (long) offset, SEEK_SET);
      else
      {
         offset -= len;
         while (offset--)
            fputc ('\0', gv_file);
      }
      fputc (val, gv_file);
      fclose (gv_file);
   }
   return (retval);
}

/*===========================================================*/
#endif /* STYLE > 11 */

#ifdef __STDC__
int special (int key, int *var)
#else
int special (key, var)
int key;
int *var;
#endif
{
   static char save_name [168];
   static char *scratch;
   char *image_ptr;
   char file_name [168];
   FILE *game_file;
   int val, val1;
#if STYLE >= 10
   int lval;
#endif /* STYLE >= 10 */
   char tval [12];
   static int tsiz = sizeof (time_t);
   int chksum;
   int chksav;
   char *cptr;
   int cnt;
   int lobj, lplace, lverb, lvar, ltext;
   static int saved_value;
   static long game_time;
   void adv_hours ();
   void adv_news ();

   switch (key)
   {
      case 1:          /* Dump game to disc */
      case 2:          /* Restore game from disc */
         val = value [ARG2];
#ifndef ADVCONTEXT
try_again:
#endif
         if (val != -1)
         {
            if (*long_word && 
               strncmp (long_word, arg2_word, 16) == 0)
                  strcpy (file_name, long_word);
            else
               strcpy (file_name, arg2_word);
         }
         else
#ifdef ADVCONTEXT
         {
            printf ("*** Glitch! No save/restore name specified!\n");
            *var = 3;
            return (0);
         }
      case 999:        /* Restore context */
      case 997:        /* Restore from context comline */
         if (key > 2)
         {
            strncpy (arg2_word, cgi_name, WORDSIZE - 1);
            make_name (cgi_name, save_name);
         }
         else
            make_name (file_name, save_name);
#else /* !ADVCONTEXT */
      case 999:
         if (key == 999)
            strncpy (file_name, cgi_name, WORDSIZE - 1);
         else if (val == -1)
         {
            if (key == 1)
            {
               PRINTF ("\nName to save game under: ");
            }
            else
            {
#if STYLE >= 11
               int cnt = -1;
               if (cgi < 'x')
                  cnt = process_saved (0, file_name);
               if (cnt == 0)
                  PRINTF (
   "Can't see any saved games here, but you may know of some elsewhere.\n")
               else if (cnt == 1)
               {
                  value [ARG2] = strlen(file_name);
                  goto got_name;
               }
               else
               {
                  PRINTF ("You have the following saved games: ")
                  if (cgi < 'x')
                     process_saved (1, NULL);
               }
#endif /* STYLE >=11 */
               PRINTF ("\nName of saved game to restore: ");
	    }
            getinput (file_name, 16);
            scrchk (NULL);
#ifdef DWARVEN
            if (value [DWARVEN]) shift_down (file_name, 16);
#endif /* DWARVEN */
            if (file_name [0] == '\0' || file_name [0] == '\n')
            {
               PRINTF ("\nChanged your mind, eh?  Fine by me...\n");
               *var = 3;
               return (0);
            }
         }
#if STYLE >= 11
got_name:
#endif /* STYLE >= 11 */
         make_name (file_name, save_name);
#endif /* ADVCONTEXT */
         if ((game_file = fopen (save_name, RMODE)) != NULL)
         {
            if (key == 2 || key == 999 || key == 997) 
               goto restore_it;
            fclose (game_file);
            PRINTF ("\nThere's already a game dumped under that name.\n");
#ifdef ADVCONTEXT
            *var = 2;
            return (0);
#else /* !ADVCONTEXT */
            PRINTF ("Do you really mean to overwrite it? ");
            if (!query (-1))
            {
               val = -1;
               goto try_again;
            }
            PRINTF ("\nAs you wish...\n");
#endif /* ADVCONTEXT */
         }
         else if (cgi > 'b' && (key == 999 || key == 997))
         {
            if (key == 999)
               return (0);
            PRINTF ("Oops! We seem to have lost your current game session!\n");
            PRINTF ("\nSorry about that!\n");
            LOGERROR(errno);
            outbuf(1);
            exit (255);
         }
         if (key == 2)
         {
            *var = 1;
            return (0);
         }
      case 998:
         if (key == 998) 
         {
            make_name (cgi_name, save_name);
#ifdef ADVCONTEXT
            if (value [ADVCONTEXT] == 0 && cgi > 'b') value [ADVCONTEXT] = 1;
            *qcon = value [ADVCONTEXT];
#endif /* ADVCONTEXT */
         }
         else
         {
            if (key != 1) return (0);
         }
         if ((game_file = fopen (save_name, WMODE)) == NULL)
         {
            *var = 1;
            return (1);
         }
         time ((time_t *) &tval[0]);
         fprintf (game_file, "%s\n", GAME_ID);
         val = FOBJ;
         fwrite (&val, sizeof (int), 1, game_file);
         val = LOBJ;
         fwrite (&val, sizeof (int), 1, game_file);
         val = LLOC;
         fwrite (&val, sizeof (int), 1, game_file);
         val = LVERB;
         fwrite (&val, sizeof (int), 1, game_file);
         val = LVAR;
         fwrite (&val, sizeof (int), 1, game_file);
         val = LTEXT;
         fwrite (&val, sizeof (int), 1, game_file);
         *value = -1;
         chksum = 0;
         CHKSUM(tval, sizeof(time_t))
         CHKSUM(IMAGE, VAL_SIZE)
         CHKSUM(IMAGE + OFFSET_LOCS, LOCS_SIZE)
         CHKSUM(IMAGE + OFFSET_OBJBIT, OBJBIT_SIZE)
         CHKSUM(IMAGE + OFFSET_LOCBIT, LOCBIT_SIZE)
         CHKSUM(IMAGE + OFFSET_VARBIT, VARBIT_SIZE)
#ifdef ADVCONTEXT
         if (cgi > 'b' && key == 998)
         {
            CHKSUM(qwords, sizeof(qwords));
            CHKSUM(qvals, sizeof(qvals));
         }
#endif /* ADVCONTEXT */
         fwrite (&chksum, sizeof (int), 1, game_file);
         fwrite (tval, 1, sizeof(time_t), game_file);
         fwrite (IMAGE, 1, IMAGE_SIZE, game_file);
#if STYLE >=11
         if (cgi > 'b' && key == 998)
         {
            fwrite (qwords, sizeof (char), sizeof (qwords), game_file);
            fwrite (qvals, sizeof (char), sizeof (qvals), game_file);
            fwrite (&mainseed, sizeof (int), 1, game_file);
            fwrite (word_buf, sizeof (short), *word_buf - 1, game_file);
            fwrite (old_comline, sizeof (char), sizeof (old_comline),
               game_file);
            fwrite (long_word, sizeof (char), sizeof (long_word),
               game_file);
         }
#endif
         *var = (ferror (game_file)) ? 1 : 0;
         fclose (game_file);
#ifdef UNDO
         if (value [UNDO_STAT] >= 0 && diffs && diffs < dptr)
         {
            strcpy (save_name + strlen(save_name) - 3, "adh");
            if (((diffs && dptr > diffs + 4) || 
               (cgi > 'b' && value [ADVCONTEXT] <= 1)) &&
                  (game_file = fopen (save_name, WMODE)))
            {
               int len = dptr - diffs;
               fwrite (&len, 1, sizeof (int), game_file);
               fwrite (diffs, 1, dptr - diffs, game_file);
               CHKSUM(diffs, len);
               fwrite (&chksum, 1, sizeof (chksum), game_file);
               len = edptr - diffs;
               fwrite (&len, 1, sizeof (int), game_file);
               fclose (game_file);
            }
         }
#endif /* UNDO */
         return (*var);

restore_it:
         *var = 0;
#ifdef ADVCONTEXT
         if (cgi == 'x')
         {
            if (log_file)
               fprintf (log_file, "\nREPLY: restore %s\n", save_name);
            *qstat = 2;
            *qarg2 = value [ARG2];
            strncpy (qargw2, arg2_word, 20);
         }
#endif
         if (check_version (game_file) != 0)
         {
            *var = 4;
            return (0);
         }
         val1 = 0;
         fread (&val, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("FOBJ: image %3d, expected %3d\n", val, FOBJ);
#endif /* DEBUG */
         if (val != FOBJ) val1++;
         fread (&lobj, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LOBJ: image %3d, expected %3d\n", lobj, LOBJ);
#endif /* DEBUG */
         if (lobj > LOBJ) val1++;
         fread (&lplace, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LLOC: image %3d, expected %3d\n", lplace, LLOC);
#endif /* DEBUG */
         if (lplace > LLOC) val1++;
         fread (&lverb, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LVERB: image %3d, expected %3d\n", lverb, LVERB);
#endif /* DEBUG */
         if (lverb > LVERB) val1++;
         fread (&lvar, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LVAR: image %3d, expected %3d\n", lvar, LVAR);
#endif /* DEBUG */
         if (lvar > LVAR) val1++;
         fread (&ltext, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LTEXT: image %3d, expected %3d\n", ltext, LTEXT);
#endif /* DEBUG */
         if (ltext > LTEXT) val1++;

         if (val1)
         {
            *var = 2;
            fclose (game_file);
            return (0);
         }
         chksav = 0;
         if (cgi < 'x')
         {
            *var = memstore (2);
            if (*var != 0)
               return (0);
         }
         if (scratch == NULL)
         {
            scratch = (char *) malloc (IMAGE_SIZE);
            if (scratch == NULL)
               return (0);
         }
         image_ptr = scratch;
#ifdef DEBUG
         puts ("Reading image...");
#endif
         fread (&chksav, sizeof (int), 1, game_file);
#ifdef DEBUG
         PRINTF2("CHKSAV %d\n", chksav)
#endif
         fread (tval, 1, sizeof (tval), game_file);
         if (*((int *)(tval + 4)) == -1)
            tsiz = 4;
         else if (*((int *)(tval + 8)) == -1)
            tsiz = 8;
         else
            tsiz = sizeof(time_t);
         fseek (game_file, tsiz - 12L, SEEK_CUR);
         {
            int locso = ltext * sizeof (int);
            int locss = (lobj + 1) * sizeof (int);
            int objbo = locso + locss;
            int objbs = OBJSIZE * (lobj - FOBJ + 1) * sizeof (short);
            int plabo = objbo + objbs;
            int plabs = LOCSIZE * (lplace - lobj) * sizeof (short);
            int varbo = plabo + plabs;
            int varbs = VARSIZE * (lvar - lplace) * sizeof (short);
            int imgsiz = varbo + varbs;
            fread (scratch, 1, imgsiz, game_file);
         }
#ifdef ADVCONTEXT
         if (cgi > 'b' && key == 999)
         {
            fread (qwords, sizeof (char), sizeof (qwords), game_file);
            fread (qvals, sizeof (char), sizeof (qvals), game_file);
            fread (&mainseed, sizeof (int), 1, game_file);
            if (! ferror (game_file))
            {
               fread (word_buf, sizeof (short), 2, game_file);
               if (ferror (game_file))
               {
                  *word_buf = 3;
                  *(word_buf + 1) = 0;
               }
               else
                  fread (word_buf + 2, sizeof (short),
                     *word_buf - 3, game_file);
            }
            if (! ferror (game_file))
            {
               fread (old_comline, sizeof (char), sizeof (old_comline),
                  game_file);
            }
            *long_word = '\0';
            if (! ferror (game_file))
            {
               fread (long_word, sizeof (char), sizeof (long_word),
                  game_file);
            }
            clearerr (game_file);
         }
#endif /* ADVCONTEXT */
         if (ferror (game_file))
         {
            *var = 2;
            return (0);
         }
#ifdef DEBUG
         puts ("Checking image...");
#endif
         fclose (game_file);
         chksum = 0;
         {
            int locso = ltext * sizeof (int);
            int locss = (lobj + 1) * sizeof (int);
            int objbo = locso + locss;
            int objbs = OBJSIZE * (lobj - FOBJ + 1) * sizeof (short);
            int plabo = objbo + objbs;
            int plabs = LOCSIZE * (lplace - lobj) * sizeof (short);
            int varbo = plabo + plabs;
            int varbs = VARSIZE * (lvar - lplace) * sizeof (short);
            int vrbbs = VARSIZE * (lverb - lplace) * sizeof (short);
            CHKSUM(tval, tsiz);
            CHKSUM(scratch, locso)
            CHKSUM(scratch + locso, locss)
            CHKSUM(scratch + objbo, objbs)
            CHKSUM(scratch + plabo, plabs)
            CHKSUM(scratch + varbo, varbs)
#ifdef ADVCONTEXT
            if (cgi > 'b' && key == 999)
            {
               CHKSUM(qwords, sizeof(qwords));
               CHKSUM(qvals, sizeof(qvals));
            }
#endif /* ADVCONTEXT */
            if (chksav != chksum)
            {
               *var = 2;
               return (0);
            }
            if (tsiz == sizeof(game_time))
               memcpy (&game_time, tval, tsiz);
            else 
               game_time = 1;
            memset (IMAGE, '\0', IMAGE_SIZE);
            memcpy (value, scratch, (lobj + 1) * sizeof (int));
            memcpy (value + FLOC, scratch + (lobj + 1) * sizeof (int), 
               (lplace - lobj) * sizeof (int));
            memcpy (value + FVERB, scratch + (lplace + 1) * sizeof (int), 
               (lverb - lplace) * sizeof (int));
            memcpy (value + FVAR, scratch + (lverb + 1) * sizeof (int),
               (lvar - lverb - 1) * sizeof (int));
            memcpy (value + FTEXT, scratch + lvar * sizeof (int),
               (ltext - lvar) * sizeof (int));
            memcpy (location,  scratch + locso, locss);
            memcpy (objbits,   scratch + objbo, objbs);
            memcpy (placebits, scratch + plabo, plabs);
            memcpy (varbits,   scratch + varbo, vrbbs);
            memcpy ((char *)varbits + VRBBIT_SIZE, scratch + varbo + vrbbs, 
               varbs - vrbbs);
	 }
#ifdef ADVCONTEXT
         if (key == 997) value [ADVCONTEXT] = 2;
#endif

#ifdef UNDO
         if (value [UNDO_STAT] >= 0)
         {
            strcpy (save_name + strlen(save_name) - 3, "adh");
            if ((game_file = fopen (save_name, RMODE)))
            {
               int len = 0;
               int diflen = 0;
               unsigned char *d;
               if (diffs)
                  edptr = dptr = diffs + 4;
               fread (&len, 1, sizeof (int), game_file);
               if (len > 0)
               {
                  diflen = 8192 * ((len + 8191) / 8192);
                  d = (unsigned char *)malloc(diflen);
                  fread (d, 1, len, game_file);
                  fread (&chksav, 1, sizeof (chksav), game_file);
                  CHKSUM(d, len);
                  if (chksum == chksav)
                  {
                     if (diffs)
                        free (diffs);
                     diffs = d;
                     diffsz = diflen;
                     dptr = diffs + len;
                     memcpy (image, IMAGE, sizeof(IMAGE));
                     fread (&len, 1, sizeof (int), game_file);
                     edptr = diffs + len;
                  }
               }
               fclose (game_file);
	    }            
         }
#endif /* UNDO */
         *var = (key == 999) ? 999 : 0;
         return (0);

      case 3:          /* Delete saved game */
#ifdef ADVCONTEXT
         make_name (arg2_word, save_name);
#endif /* ADVCONTEXT */
         *var = unlink (save_name);
 if (*var)
 {
   printf ("Failed: %s - error code: %d<br />\n", save_name, *var);
   system ("pwd");
 }
         strcpy (save_name + strlen(save_name) - 3, "adh");
         unlink (save_name);
         return (0);
      case 4:          /* Adv550 legacy - flush game cache */
      case 5:          /* Adv550 legacy - get prime time flag */
         *var = 0;
         return (0); 
      case 6:          /* Save value of a variable (only one can be saved!) */
#if STYLE < 10
         PRINTF("Open at all times.\n");
#else
         saved_value = *var;
#endif
         return (0);
      case 7:          /* Restore saved variable value */
#if STYLE < 10
         saved_value = *var;
#else
         *var = saved_value;
#endif
         return (0);
      case 8:          /* Get minutes since restored game dumped */
#if STYLE < 10
         *var = saved_value;
#else
         time ((time_t *) &lval);
         *var = 1 + (lval - game_time) / 60;      /* Be generous! */
#endif
         return (0);
      case 9:         /* Fudge a value into ARG1 */
#if STYLE < 10
         *var = 0;
#else
         value [ARG1] = *var;
         fake (1, *var);
#endif
         return (0);
      case 10:        /* Fudge a value into ARG2 */
         value [ARG2] = *var;
         fake (2, *var);
         return (0);
      case 11:         /* Pretend player said "X X" instead of "X" */
         value [ARG2] = value [ARG1];
         strncpy (arg2_word, arg1_word, 20);
         value [STATUS] = 2;
         return (0);
      case 12:         /* Check for end of command */
         *var = (tp[tindex] == NULL);
         return (0);
/*    case 13: */      /* Spare */
#if STYLE > 11
      case 14:         /* Get persistent data item */
         *var = get_pdata (*var);
         return (0);
      case 15:         /* Store persistent data item */
      case 16:         /* Clear persistent data item */
         set_pdata (*var, key == 15 ? 1 : 0);
         return (0);
#endif /* STYLE > 11 */
/*    case 17: */        /* Spare */
/*    case 18: */        /* Spare */
      case 19:    /* Fiddle justification */
         val = *var;
         justify = val < 2 ? val : 1 - justify;
         *var = justify;
         return (0);
         
      case 20:    /* Set screen line length */
         val = atoi (arg2_word);
         *var = 0;
         if (val == 0) return (0);
         if (val < 16) val = 16;
         if (val > 1024) val = 1024;
         *var = val;
         Linlen = val;
         if (Linlen - 2 * Margin < 16)
            Margin = (Linlen - 16) / 2;
         if (Margin < 0) Margin = 0;
         Maxlen = Linlen - 2 * Margin;
#ifdef READLINE
         if (!cgi && !cps)
         {
            int offset = prompt_ptr - prompt_line;
            prompt_line = advrealloc(prompt_line, 2 * Maxlen + 1);
            prompt_ptr = prompt_line + offset;
         }
#endif /* READLINE */
         return (0);

      case 21:    /* Set page offset */
         val = atoi (arg2_word);
         *var = 0;
         if (val <0) val = 0;
         if (val > 9) val = 9;
         if (Linlen - val - val < 16)
            val = (Linlen - 16) / 2;
         if (val < 0) val = 0;
         *var = val;
         Margin = val;
         Maxlen = Linlen - val - val;
#ifdef READLINE
         if (!cgi && !cps)
         {
            int offset = prompt_ptr - prompt_line;
            prompt_line = advrealloc(prompt_line, 2 * Maxlen + 1);
            prompt_ptr = prompt_line + offset;
         }
#endif /* READLINE */
         return (0);

      case 22:    /* Set screen depth */
         val = atoi (arg2_word);
         if (val < 4)
         {
            *var = 0;
            return (0);
	 }
         if (val > 1024) val = 1024;
         *var = val;
         Screen = val;
         return (0);

      case 23:    /* Preserve player input */
         *qtext = *var;
         *qstat = value [STATUS];
         *qarg1 = value [ARG1];
         strncpy (qargw1, arg1_word, 20);
         if (*qstat == 2)
         {
            *qarg2 = value [ARG2];
            strncpy (qargw2, arg2_word, 20);
         }
         else
         {
            *qarg2 = -1;
            *qargw2 = '\0';
         }
         return (0);

      case 24:    /* Restore player input */
         *var = *qtext;
         value [STATUS] = *qstat;
         value [ARG1] = *qarg1;
         value [ARG2] = *qarg2;
         strncpy (arg1_word, qargw1, 20);
         strncpy (arg2_word, qargw2, 20);
         return (0);

/*    case 25: */   /* Spare */
/*    case 26: */   /* Spare */

      case 27:    /* Pre-say */
         *var = 1;
         if (*say_buf)
         {
            PRINTF("\nOk - \"")
            PRINTF(say_buf)
            PRINTF("\"\n")
            *say_buf = '\0';
         }
         return (0);
         
      case 28:   /* Recover from failed restore */
         *var = memstore (3);
         return (0);
         
      case 29:    /* Swap ARG1 and ARG2 */
         *var = 0;
         if (value [STATUS] > 1)
         {
            val = value [ARG1];
            value [ARG1] = value [ARG2];
            value [ARG2] = val;
            for (cnt = 0; cnt < VARSIZE; cnt++)
            {
               val = varbits [ARG1 - FVERB + cnt];
               varbits [ARG1 - FVERB + cnt] = varbits [ARG2 - FVERB + cnt];
               varbits [ARG2 - FVERB + cnt] = val;
            }
            strcpy (temp_word, arg1_word);
            strcpy (arg1_word, arg2_word);
            strcpy (arg2_word, temp_word);
         }
         return (0);

/*      case 30:   Spare */
         
/*      case 31:   Spare */

      case 32:   /* Is the object on the exception list? */
#if STYLE >= 11 && defined (ALL) && defined (EXCEPT)
         if (except_count == 0)
         {
            *var = 0;
            return (0);
         }
         for (cnt = 0; cnt < except_count; cnt++)
         {
            if (*var == except [cnt])
            {
               *var = 1;
               return (0);
            }
         }
#endif /* ALL and EXCEPT */
         *var = 0;
         return (0);

      case 33:  /* Check for existence of a memory save */
#if STYLE >= 11
         *var = memstore (-1);
#else
         *var = 0;
#endif
         return (0);

      case 34:  /* List available saved games */
#if STYLE >= 11
         *var = process_saved (*var, arg2_word);
         value [ARG2] = strlen (arg2_word);
#else
         *var = 0;
#endif
         return (0);
         
      default:
         PRINTF2 ("\n \nGLITCH! Bad special code: %d\n", key);
         return (1);
   }
}

#ifdef USEDB

/*===========================================================*/

#ifdef __STDC__
void create_db (char *dbfile)
#else
void create_db (dbfile)
char *dbfile;
#endif
{
   FILE *db_file;
   char ch;
   int val = 0;
   int bytes = 0;
   int sgn = 0;
   
   if ((db_file = fopen (dbfile, WMODE)) == NULL)
      return;
   while (fgetc (text_file) != '{')
      if (feof (text_file))
         return;
   while (1)
   {
      ch = fgetc (text_file);
      if (feof (text_file))
         return;
      if (isdigit (ch))
      {
         if (sgn == 0)
            sgn = 1;
         val = 10 * val + ch - '0';   
      }
      else if (ch == '-' && sgn == 0)
         sgn = -1;
      else if (sgn)
      {
         if (sgn == -1)
            val = - val;
         fputc (val & 255, db_file);
         sgn = 0;
         val = 0;
         bytes++;
      }
      if (ch == '}')
         break;
   }
   fclose (db_file);
   if (bytes != TEXT_BYTES)
      return;
   printf ("\nText database %s created.\n\n", DBNAME);
   exit (0);
}

/*===========================================================*/

#ifdef __STDC__
FILE *try_db (char *dbdir, char *dbname, int trim)
#else
FILE *try_db (dbdir, dbname, trim)
char *dbdir;
char *dbname;
int trim;
#endif
{
   FILE *dbf;
   char *dptr;
   int len;

   len = strlen (dbname) + 1;
   if (dbdir && *dbdir && SEP != '?')
      len += strlen (dbdir) + 1;
   if ((dptr = (char *) malloc (len)) == NULL)
   {
      fprintf (stderr, "Unable to allocate db name buffer!\n");
      exit (1);
   }
   if (dbdir && *dbdir && SEP != '?')
   {
      strcpy (dptr, dbdir);
      if (trim)
      {
         char *cptr = strrchr (dptr, SEP);
         len = cptr ? cptr - dptr + 1 : 0;
      }
      else
         len = strlen (dptr);
   }
   else
      len = 0;
   if (len)
      sprintf (dptr + len, "%c%s", SEP, dbname);
   else
      strcpy (dptr, dbname);
   dbf = fopen (dptr, RMODE);
   free (dptr);
   return (dbf);
}

#endif /* USEDB */

#define BROWEXE  0
#define BG       1
#define FG       2
#define PROMPT   3
#define DBG      4
#define SURROUND 5
#define BUTTON   6
#define COMPACT  7
#define COMMANDS 8
#define PXWIDTH  9
#define LOGFILE 10
#define JUSTIFY 11
#define MARGIN  12
#define WIDTH   13
#define HEIGHT  14

char *conf[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                 NULL, NULL, NULL, NULL, NULL, NULL, NULL };

#define SUBMIT_BUTTON "&nbsp;&nbsp;<input type=\"submit\" name=\"Submit\", value=\"Submit\">"

#define CONFFILE "acode.conf"

/*====================================================================*/

void store_conf (int index, char *value)
{
   if (*(conf + index) == NULL)
   {
      *(conf + index) = (char *) malloc (strlen (value) + 1);
      if (*value == '\\')
         value++;
      else if (*value == '\'')
      {
         char *tptr = value + strlen(value) - 1;
         if (*tptr == '\'')
            *tptr = '\0';
         value++;
      }
      strcpy (*(conf + index), value);
   }
}

/*====================================================================*/

void parse_line (char *line, char **tokens)
{
   int i;
   int spc;
   char *cptr = line;
   *tokens = NULL;
   *(tokens + 1) = NULL;
   *(tokens + 2) = NULL;


   for (i = 0; i < 3; i++)
   {
      spc = 0;
      while (*cptr == ' ' || *cptr == '\t')
         cptr++;
      if (*cptr == '\0' || *cptr == '#' || *cptr == '\n')
         return;
      if (*cptr == '\\' || *cptr == '\'')
         spc = *cptr++;
      *(tokens + i) = cptr;
      while (*cptr && *cptr != ' ' && *cptr != '\t' && *cptr != '\n')
         cptr++;
      if (cptr > line && *(cptr - 1) == spc)
         cptr--;
      *cptr++ = '\0';
   }
}

/*====================================================================*/

char *recase (char *token, int cflag)
{
   char *cptr = token;
   if (cptr == NULL)
      return (NULL);
   while (*cptr)
   {
      if (cflag == 'L' && *cptr >= 'A' && *cptr <= 'Z')
         *cptr += 'a' - 'A';
      else if (cflag == 'U' && *cptr >= 'a' && *cptr <= 'z')
         *cptr += 'A' - 'a';
      cptr++;
   }
   return (token);
}

/*====================================================================*/

void handle_token (int type, int attribute, char *aptr, int max, int *val)
{
   char buf [16]; 
   int lval;
   if (conf [attribute] != NULL)
      return;
   if ((type == 'p' && atoi (aptr) > 0) ||  /* A positive number */
      (type == 'n' && atoi (aptr) >= 0))    /* A non-negative number */
   {
      lval= atoi (aptr);
      if (max > 0 && lval > max) lval = max; 
      if (max < 0 && lval < -max) lval = -max;
      if (val) *val = lval;
      sprintf (buf, "%d", lval);
      store_conf (attribute, buf);
   }
   else if (type == 'b')                     /* A binary choice */
   {
      if (*aptr == 'T' || *aptr == '1' || *aptr == 'Y' ||
         (*aptr == 'O' && *(aptr + 1) == 'N'))
      {
         store_conf (attribute, "Y");
         if (val) *val = 1;
      }
      else if (*aptr == 'F' || *aptr == '0' || *aptr == 'N' ||
         (*aptr == 'O' && *(aptr + 1) == 'F'))
      {
         store_conf (attribute, "N");
         if (val) *val = 0;
      }
   }
   else if (type == 'c')                     /* Compactification choice */
   {
      if (*aptr == 'N' || *aptr == 'F' || *aptr == '0')
      {
         store_conf (attribute, "0");
         if (val) *val = 0;
      }
      else if (*aptr == 'P' || *aptr == 'S' || *aptr == '1')
      {
         store_conf (attribute, "1");
         if (val) *val = 1;
      }
      else if (*aptr == 'Y' || *aptr == 'T' || *aptr == 'A' || *aptr == '2')
      {
         store_conf (attribute, "2");
         if (val) *val = 2;
      }
      else if (*aptr == 'D' || *aptr == '?')
      {
         char num [2];
         if (*val < 0) *val = 0;
         if (*val > 2) *val = 2;
         *num = '0' + *val;
         *(num + 1) = '\0';
         store_conf (attribute, num);
      }
   }
}

/*====================================================================*/

void check_browser (char *name)
{
   struct stat stat_buf;
   char gpath[256];
   char *path;
   
   if (strchr (name, SEP))
   {
      if (stat (name, &stat_buf) != -1)
         store_conf (BROWEXE, name);
      return;
   }
   if ((path = getenv ("PATH")) != NULL)
   {
      char *sptr = path;
      char *eptr = sptr;
      while (1)
      {
         while (*eptr && *eptr != ':')
            eptr++;
         if (eptr - sptr < 255 - strlen (name))
         {
            strncpy (gpath, sptr, eptr - sptr);
            *(gpath + (eptr - sptr)) = SEP;
            strcpy (gpath + (eptr + 1 - sptr), name);
            if (stat (gpath, &stat_buf) != -1)
            {
               store_conf (BROWEXE, gpath);
               return;
            }
         }
         if (*eptr == '\0')
            break;
         sptr = ++eptr;
      }
   }   
}

/*====================================================================*/

void read_conf ()
{
   char cbuf [256];
   char *tkn [3];
   char *cptr;
   FILE *cfile = NULL;
   
   strcpy (cbuf, prog);
   if ((cptr = strrchr (cbuf, SEP)) != NULL)
   {
      strcpy (cptr + 1, CONFFILE);
      cfile = fopen (cbuf, RMODE);
   }
   if (cfile == NULL)
   {
      sprintf (cbuf, "%s/acode/%s", getenv ("HOME"), CONFFILE);
      cfile = fopen (cbuf, RMODE);
   }
   if (cfile == NULL)
      cfile = fopen (CONFFILE, RMODE);
   if (cfile == NULL)
      return;

   *(cbuf + sizeof(cbuf) - 1) = '\0';
   while (fgets (cbuf, sizeof(cbuf) - 1, cfile))
   {
      char *aptr;
      *(cbuf + strlen(cbuf) - 1) = '\0';
      parse_line (cbuf, tkn);
      if (*tkn == NULL || *(tkn + 1) == NULL)
         continue;
      recase (*tkn, 'U');

#ifdef BROWSER
      if (strcmp (*tkn, "BROWSER") == 0)
      {
         if (conf [BROWEXE])
            continue;
         cptr = *(tkn + 1);
         if (*(tkn + 2) && **(tkn + 2))
         {
            if (strcmp (cptr, PLATFORM) != 0)
               continue;
            cptr = *(tkn + 2);
         }
         if (*cptr == '$')
         {
            cptr = getenv(cptr + 1);
            if (!cptr)
               continue;
         }
         if (strcmp (cptr, "NONE") == 0)
            cgi = 0;
         else
            check_browser (cptr);
      }
      else if (strcmp (*tkn, "COLOUR") == 0 || strcmp (*tkn, "COLOR") == 0)
      {
         cptr = recase (*(tkn + 1), 'U');
         aptr = recase (*(tkn + 2), 'L');
         if (strcmp (cptr, "BACKGROUND") == 0 || strcmp (cptr, "BG") == 0)
               store_conf (BG, aptr);
         if (strcmp (cptr, "TEXT") == 0 || strcmp (cptr, "FG") == 0)
               store_conf (FG, aptr);
         if (strcmp (cptr, "PROMPT") == 0)
            store_conf (PROMPT, aptr);
         if (strcmp (cptr, "SURROUND") == 0)
            store_conf (SURROUND, aptr);
         if (strcmp (cptr, "DEBUG") == 0 || strcmp(cptr, "DBG") == 0)
            store_conf (DBG, aptr);
      }
      else if (strcmp (*tkn, "SHOW") == 0)
      {
         if (! (tkn + 2) || ! *(tkn + 2))
            continue;
         cptr = recase (*(tkn + 1), 'U');
         aptr = recase (*(tkn + 2), 'U');
         if (strcmp (cptr, "BUTTON") == 0)
            handle_token ('b', BUTTON, aptr, 0,  NULL);
         else if (strcmp (cptr, "COMPACT") == 0)
            handle_token ('c', COMPACT, aptr, 0, &compress);
         else if (strcmp (cptr, "COMMANDS") == 0)
            handle_token ('p', COMMANDS, aptr, 2000, NULL);
         else if (strcmp (cptr, "WIDTH") == 0)
            handle_token ('p', PXWIDTH, aptr, -500, NULL);
      }
      else if (strcmp (*tkn, "LOG") == 0 || strcmp (*tkn, "LOGFILE") == 0)
#else /* ! BROWSER */
      if (strcmp (*tkn, "LOG") == 0 || strcmp (*tkn, "LOGFILE") == 0)
#endif /* BROWSER */
      {
         int need_log = 0;
         aptr = *(tkn + 2);
         cptr = recase (*(tkn + 1), 'U');
         if (conf [LOGFILE] != NULL)
            continue;
         if (strcmp (cptr, "OFF") == 0 || 
             strcmp (cptr, "NO") == 0 || strcmp (cptr, "NONE") == 0)
               store_conf (LOGFILE, "N");
         if (strcmp (cptr, "APPEND") == 0 || strcmp (cptr, "ON") == 0)
         {
            store_conf (LOGFILE, UMODE);
            need_log = 1;
         }
         else if (strcmp (cptr, "OVERWRITE") == 0 || 
                  strcmp (cptr, "WRITE") == 0)
         {
            store_conf (LOGFILE, "w");
            need_log = 1;
         }
         if (need_log)
         {
            if (aptr == NULL)
               sprintf (log_path, "%s.log", cgi_name + 1);
            else
            {
               struct stat stat_buf;
               char *rptr = strrchr (aptr, SEP);

               if (stat (aptr, &stat_buf) == 0)
               {
                  if (stat_buf.st_mode & S_IFREG)
                     strncpy (log_path, aptr, sizeof (log_path));
                  else if (stat_buf.st_mode & S_IFDIR)
                     sprintf (log_path, "%s%c%s.log", aptr, SEP, cgi_name + 1);
                  else
                     *conf[LOGFILE] = 'N';
               }
               else if (rptr)
               {
                  *rptr = '\0';
                  if (stat (aptr, &stat_buf) == 0)
                  {
                     if (stat_buf.st_mode & S_IFDIR)
                     {
                        *rptr = SEP; 
                        strncpy (log_path, aptr, sizeof (log_path));
                     }
                  }
                  else 
                     *conf[LOGFILE] = 'N';
               }
               else
               {
                  strncpy (log_path, aptr, sizeof (log_path));
                  *(log_path + sizeof (log_path) - 1) = '\0';
               }
            }
         }
      }
      else if (strcmp (*tkn, "LAYOUT") == 0)
      {
         if (!*(tkn + 2) || !**(tkn + 2))
            continue;
         aptr = recase (*(tkn + 2), 'U');
         cptr = *(tkn + 1);
         if (strcmp (cptr, "JUSTIFY") == 0)
         {
            if (strcmp (aptr, "DEFAULT") == 0 || strcmp (aptr, "ANY") == 0)
               store_conf (JUSTIFY, justify ? "Y" : "N");
            else
               handle_token ('b', JUSTIFY, aptr, 0, &justify);
	 }
         else if (strcmp (cptr, "MARGIN") == 0)
            handle_token ('n', MARGIN, aptr, 5, &Margin);
         else if (strcmp (cptr, "WIDTH") == 0)
            handle_token ('p', WIDTH, aptr, -30, &Linlen);
         else if (strcmp (cptr, "HEIGHT") == 0)
            handle_token ('p', HEIGHT, aptr, 0, &Screen);
         Maxlen = Linlen - 2 * Margin;
      }
   }

/* Now set default values for anything still missing. */

   if (conf [BROWEXE] == NULL)
   {
      char *cptr;
      if ((cptr = getenv ("BROWSER")) != NULL)
         store_conf (BROWEXE, cptr);
      else
#ifdef __MACH__
         store_conf (BROWEXE, "/usr/bin/open");
#else
         store_conf (BROWEXE, "/usr/bin/xdg-open");
#endif
   }

#ifdef BROWSER
   if (conf [BG] == NULL)
      store_conf (BG, "#d0e0ff");
   if (conf [FG] == NULL)
      store_conf (FG, "black");
   if (conf [PROMPT] == NULL)
      store_conf (PROMPT, "red");
   if (conf [DBG] == NULL)
      store_conf (DBG, "blue");
   if (conf [SURROUND] == NULL)
      store_conf (SURROUND, "#222222");
   if (conf [BUTTON] == NULL)
      store_conf (BUTTON, "Y");
   if (conf [COMMANDS] == NULL || atoi (conf [COMMANDS]) <= 0)
      store_conf (COMMANDS, "200");
   if (conf [PXWIDTH] == NULL || atoi (conf [PXWIDTH]) <= 500)
      store_conf (PXWIDTH, "700");
#endif /* BROWSER */
   if (conf [COMPACT] == NULL)
      store_conf (COMPACT, compress ? "Y" : "N");
   if (conf [LOGFILE] == NULL)
      store_conf (LOGFILE, UMODE );
   if (conf [JUSTIFY] == NULL)
      store_conf (JUSTIFY, justify ? "Y" : "N");
   if (conf [MARGIN] == NULL)
      store_conf (MARGIN, "1");
   if (conf [WIDTH] == NULL)
      store_conf (WIDTH, "80");
   if (conf [HEIGHT] == NULL)
      store_conf (HEIGHT, "24");
}

/*===========================================================*/

#ifdef __STDC__
int initialise (char *prog)
#else
int initialise (prog)
char *prog;
#endif
{
#ifdef MEMORY
   int text_bytes;
#endif
#ifdef READFILE
   char text [100];
#endif
   int index;

   if (com_name && *com_name && (com_file = fopen (com_name, RMODE)) == NULL)
   {
      printf ("Sorry, unable to open command file '%s'!\n", com_name);
      exit (0);
   }
   if (com_file)
   {
      fgets (comline, sizeof (comline), com_file);
      if (strncmp (comline, "Wiz: ", 5) == 0)
         mainseed = atol (comline + 5);
      else if (strncmp (comline, GAME_ID, strlen (GAME_ID)) != 0)
      {
         printf ("%s: wrong adventure version!\n", com_name);
         exit (0);
      }
      else
         mainseed = atol (comline + strlen (GAME_ID) + 1);
   }

   if (cgi < 'x')
   {
      struct stat stat_buf;
      char dirname [64];
      
      strcpy (dirname, CGINAME);

      chdir (getenv ("HOME"));
      if (chdir ("acode") != 0)
      {
#if defined(MSDOS) || defined(_WIN32)
         mkdir ("acode", 0700);
#else
         mkdir ("acode", 0700);
#endif
         chdir ("acode");
      }
/*
 * If necessary, create the acode.conf file.
 */
      if (stat ("acode.conf", &stat_buf) != 0)
      {
         void make_conf (void);
         make_conf ();
      }
/*
 * Go to where A-code games live!
 */
      if (chdir (dirname + 1) != 0)
      {
#if defined(MSDOS) || defined(_WIN32)
         mkdir (dirname + 1, 0700);
#else
         mkdir (dirname + 1, 0700);
#endif
         chdir (dirname + 1);
      }
#if !defined(GLK) && !defined(QT)
   read_conf (); /* Process the config file if any and set defaults if needed */
   if (!cgi)
   {
      putchar ('\n');
      for (index = 0; index < Margin; index++)
         putchar (' ');
   }
   PRINTF2("A-code kernel %s\n", KERNEL_VERSION);
#endif

/* Do a one-off copy of saved games from where they might be to the new
 * game home location.
 */
 
#if STYLE > 11
      if (strcmp (GAME_NAME, "Adv770") == 0 && get_pdata(0) <= 0)
      {
         char path [128];
         char *nptr = getenv ("HOME");
         
         if (nptr == NULL)
            nptr = "";
         
         strcpy (path, nptr);
         nptr = path + strlen (nptr);
         process_saved (-1, path);
         if (nptr > path)
            *nptr++ = SEP;
#ifdef __MACH__
         strcpy (nptr, "Library/Preferences/advent");
         process_saved (-1, path);
         process_saved (-1, "/Applications/adv770");
#endif
#if defined(MSDOS) || defined(_WIN32) || defined(__CYGWIN__)
         process_saved (-1, "C:\\Program Files\\Adv770");
#endif
#ifdef unix
         strcpy (nptr, "acode");         
         process_saved (-1, path);
         strcpy (nptr, GAME_NAME);         
         process_saved (-1, path);
#endif
         set_pdata (0, 1);
      }
#endif /* STYLE > 11 */

/* End of one-off game copy code */
   }

   if (*log_path)
   {
      if ((log_file = fopen (log_path, conf[LOGFILE])) == NULL)
         printf ("(Sorry, unable to open log file...)\n");
      else 
      if (cgi <= 'x')
         fprintf (log_file, "%s: %u\n", GAME_ID, mainseed);
   }

   text_buf = advalloc (text_buf_len);
   lptr = text_buf;
   *lptr++ = '\n';
   text_len = 1;
#if STYLE >= 11
   word_buf = btinit (NULL);
#endif /* STYLE */

#ifdef USEDB
   text_file = try_db (prog, DBNAME, 1);
   if (text_file == NULL)
      text_file = try_db (dbs_dir, DBNAME, 0);
#if defined(DDIR)
   if (text_file == NULL)
      text_file = try_db (DDIR, DBNAME, 0);
#endif
#if defined(DRDIR)
   if (text_file == NULL)
      text_file = try_db (DRDIR, DBNAME, 0);
#endif
   if (text_file == NULL)
      text_file = fopen (DBNAME, RMODE);
   if (text_file == NULL)
   {
      if ((text_file = fopen ("adv6.h", RMODE)))
         create_db (DBNAME);     /* This will exit, if successful */
      else
      {
         printf ("Sorry, can't find the %s data file.\n", DBNAME);
         close_files ();
         return (1);
      }
      if ((text_file = fopen (DBNAME, RMODE)) == NULL)
      {
         printf ("Unable to find or construct the data file %s.\n", DBNAME);
         return (1);
      }
   }

#ifdef MEMORY
   text_bytes = fread (text, sizeof (char), TEXT_BYTES+1, text_file);
   clearerr (text_file);
   fclose (text_file);
   if (text_bytes != TEXT_BYTES)
   {
      printf ("Wrong data file length: expected %d, got %d.\n",
         TEXT_BYTES, text_bytes);
      return (1);
   }
#endif /* MEMORY */
#ifdef SWAP
   chunk_end [0] = fread (text, sizeof (char), TEXT_CHUNK, text_file) - 1;
   chunk_start [0] = 0;
#endif /* SWAP */
#ifdef READFILE
   fread (text, sizeof (char), sizeof (text), text_file);
#endif /* READFILE */
#endif /* USEDB */

   virgin = *text;
#ifdef PLAIN
   strcpy (title, (char *)text + 1);
#else
   title [0] = text [1] ^ KNOT;
   titlen = 0;
   while (++titlen < 80)
      if ((title [titlen] = text [titlen] ^ text [titlen + 1]) == '\0')
         break;
#endif /* PLAIN */
   
   if (strcmp (title, GAME_ID) != 0)
   {
      printf ("Version stamp mismatch!\n");
      return (1);
   }
   tp [0] = NULL;

#if STYLE < 12
   for (index = FOBJ; index <= LOBJ; index++)
      bitmod ('s', index, OBJ);

   for (index = FLOC; index <= LLOC; index++)
      bitmod ('s', index, LOC);
#endif

#if STYLE == 1 && defined(BRIEF)
   bitmod ('s', STATUS, BRIEF);
#endif

   for (index = 0; (unsigned int)index < 
                             sizeof (voc_refno) / sizeof (voc_refno [0]);
         index++)
      if (voc_refno [index] < 0)
      {
         voc_refno [index] *= -1;
         bitmod ('s', voc_refno [index], VERB + 1);
      }
            
   return (0);
}

/*===========================================================*/

void zap_cgi_dump (void)
{
   char name [64];
   int len =sprintf (name, "%s.adv", CGINAME) - 1;
   unlink (name);
   *(name + len) = 'h';
   unlink (name);
   *(name + len) = 'l';
   unlink (name);
}

/*===========================================================*/

#ifdef GLK
int argc = 0;
char **argv = NULL;

#if defined(unix) || defined(linux) || defined(XGLK)
#define glkunix_arg_End (0)
#define glkunix_arg_ValueFollows (1)
#define glkunix_arg_NoValue (2)
#define glkunix_arg_ValueCanFollow (3)
#define glkunix_arg_NumberValue (4)

typedef struct glkunix_argumentlist_struct {
    char *name;
    int argtype;
    char *desc;
} glkunix_argumentlist_t;

typedef struct glkunix_startup_struct {
    int argc;
    char **argv;
} glkunix_startup_t;

/*===========================================================*/

#ifdef __STDC__
int glkunix_startup_code(glkunix_startup_t *data)
#else
int glkunix_startup_code(data)
glkunix_startup_t *data;
#endif
{
   argc = data->argc;
   argv = data->argv;
   return 1;
}

glkunix_argumentlist_t glkunix_arguments[] = 
   {
      {"",   glkunix_arg_ValueFollows, "[<dumpfile]: game to restore"},
      {"-b", glkunix_arg_NoValue, "-b: toggle blank lines around prompt"},
#ifdef USEDB
      {"-d", glkunix_arg_ValueFollows, "-d <dirname>: dbs directory"},
#endif /* USEDB */
      {"-r", glkunix_arg_ValueFollows, "[-r] <savefile>: restore game"},
      {"-c", glkunix_arg_ValueFollows, "-c <comfile>: replay game from log"},
      {"-log", glkunix_arg_ValueCanFollow, "-log [<logfile>]: log the game"},
      {"-h", glkunix_arg_NoValue, "-h: print help text"},
      {NULL, glkunix_arg_End, NULL }
   };
#endif /* unix || linux || XGLK */
void glk_main (void)
#else

/*===========================================================*/

#  ifdef __STDC__
int main (int argc, char **argv)
#  else
int main (argc, argv)
int argc;
char **argv;
#  endif
#endif
{
   char *kwrd;
   int val;
   char oc;
   char *opt;
   int new_game = 0;
   
   prog = *argv;
#if defined(BROWSER)
   cgi = getenv("DISPLAY") ? 'b' : 0;
#endif
   if (Linlen == 0) Linlen = 32767;
   if (Screen == 0) Screen = 32767;
   Maxlen = Linlen - 2 * Margin;
#ifdef GLK
   mainwin = glk_window_open(0, 0, 0, wintype_TextBuffer, 1);
   if (!mainwin)
   {
      fprintf (stderr, "Failed to open main window!\n");
      exit (0);
   }
    
   /* Set the current output stream to print to it. */
   glk_set_window (mainwin);
   
#endif
   strncpy (exec, *argv, sizeof (exec) - 1);
   while (--argc)
   {
      argv++;
      if (**argv != '-') 
      {
         if (! dump_name)
         {
            if (*argv) dump_name = *argv; 
         }
         else if (! *log_path)
         {
            strncpy (log_path, *argv, sizeof (log_path)); 
            *(log_path + sizeof (log_path) - 1) = '\0';
         }
      }
      kwrd = *argv + 1;          /* Point at the keyword specifier */
      if (*kwrd == '-') kwrd++;  /* Skip GNU-style prefix */
      opt = kwrd + 1;            /* Point at option value, if any */
      if (*opt == '=') opt++;    /* Skip optional '=' */
      if (!*opt && strchr ("Brclso", *kwrd) &&  /* Cater for old-style... */
         *(argv + 1) && **(argv + 1) != '-')    /* blank separator for... */
      {                                         /* some keywords */
         argc--; argv++;         /* Advance the command line pointer */
         opt = *argv;            /* This is presumably the option value */
      }
      oc = *opt;                              /* Make single-char values... */
      if (oc >= 'A' && oc <= 'Z') oc += ('a' - 'A');    /* case independent */

/* Now process individual keywords. */

      if (*kwrd == 'j')
      {
         if (oc == '0' || oc == 'n') justify = 0;
         else if (oc == '1' || oc == 'y') justify = 1;
         else justify = 1 - justify;
         store_conf (JUSTIFY, justify ? "Y" : "N");
         cgi = 0;
         continue;
      }
      else if (*kwrd == 'b')
      {
         if (oc == '0' || oc == 'n') compress = 0;
         else if (oc == '1' || oc == 'y') compress = 1;
         else if (oc == '2' || oc == 'a') compress = 2;
         else if (compress >= 0 && compress <= 1) compress = 1 - compress;
         store_conf (COMPACT, compress ? "Y" : "N");
         continue;
      }
      else if (*kwrd == 'p')
      {
         if (oc == '0' || oc == 'n') end_pause = 0;
         else if (oc == '1' || oc == 'y') end_pause = 1;
         else end_pause = 1 - end_pause;
         continue;
      }
      else if (*kwrd == 'n')
         new_game = 1;
      else if (*kwrd == 'v')
      {
#if defined(GAME_VERSION)
         printf ("%s version %s", GAME_NAME, 
            strcmp (GAME_VERSION, "99.99") == 0 ? "UNKNOWN" : GAME_VERSION);
#if defined(GAME_DATE)
         printf (", %s.\n", GAME_DATE);
#else /* ! GAME_DATE */
         puts (".");
#endif /* GAME_DATE */

#else /* ! GAME_VERSION */
         if (strchr (GAME_ID, ' '))
            puts (GAME_ID);
         else
            puts ("Game version UNKNOWN\n");
#endif /* GAME_VERSION */

#if defined(ACDC_VERSION)
         printf ("Acdc translator version %s.\n", ACDC_VERSION);
#else
         puts ("Acdc translator version UNKNOWN");
#endif /* ACDC_VERSION */

         printf ("A-code kernel version %s.\n", KERNEL_VERSION);
#if defined(DBSTATUS)
#  if DBSTATUS == 0
         puts ("Text database preloaded into the executable.");
#  endif /* DBSTATUS == 0 */
#  if DBSTATUS == 1
         puts ("Text database read into memory on startup.");
#  endif /* DBSTATUS == 1 */
#  if defined(SWAP)
         printf ("Text database paged via %d internal 1KB swap buffers.\n", 
            SWAP);
#  endif /* DBSTATUS == 2 */
#  if DBSTATUS == 3
         puts ("All text read directly from the text database as required.");
#  endif /* DBSTATUS == 3 */
#endif /* DBSTATUS */
         exit (0);
      }
#ifdef SLOW
      else if (*kwrd == 'o')
      {
         cgi = 0;
         if (oc)
         {
            cps = (atoi (opt)) / 10;
                 if (cps >= 960) cps = 960;
            else if (cps >= 480) cps = 480;
            else if (cps >= 240) cps = 240;
            else if (cps >= 120) cps = 120;
            else if (cps >=  60) cps =  60;
            else if (cps >=  30) cps =  30;
            else                 cps =  11;
         }
         else 
            cps = 30;
      }
#endif /* SLOW */
      else if (*kwrd == 'l')
      {
         if (oc) strncpy (log_path, opt, sizeof(log_path));
         else sprintf (log_path, "%s.log", cgi_name + 1);
         *(log_path + sizeof (log_path) - 1) = '\0';
         store_conf (LOGFILE, UMODE);
      }
#ifdef BROWSER
      else if (*kwrd == 'B')
      {
         if (getenv ("DISPLAY"))
            cgi = 'b';
         else
            fputs (
               "\n*** No display available ***\nDefaulting to the console mode.\n\n",
                  stderr);
         if (oc) check_browser (opt);
      }
      else if (*kwrd == 'C')
         cgi = 0;
#endif /* BROWSER */
      else if (*kwrd == 'h')
      {
         printf ("\nUsage: %s [options]\n\nOptions:\n", prog);
         puts ("    -n                  force a new game (ignore an aborted one)");
         puts ("   [-r]<dumpfile>       restore game from dump");
         printf ("    -l[<logfile>]       log the game (by default in ~/acode/%s/)\n",
            cgi_name + 1);
#ifdef BROWSER
         puts ("    -C                  Force console (non-browser) display");
         puts ("    -B[browser]         Force display through browser (default)");
#endif /* BROWSER */
#if !(defined(GLK) || defined (QT))
         puts ("    -b[0|1|all]         invert or specify blank line suppression level");
         puts ("    -p[0|1]             invert or specify pausing before exit");
#endif /* !(GLK || QT) */
#ifdef USEDB
         puts ("    -d<dbsdir>          specify dbs directory");
#endif /* USEDB */
         puts ("    -c<cominfile>       replay game from log");
#ifdef UNDO
         if (undo_def != -2)
            puts ("    -u{0|1|none}        override default UNDO status");
#endif /* UNDO */
#if !(defined(GLK) || defined (QT))
#ifdef BROWSER
         puts ("\nThese options force console mode display:");
#endif /* BROWSER */
         puts ("    -j[0|1]             invert or specify right-justification of text");
         puts ("    -s<W>.<H>[.<M>]     set screen size and margin");
         puts ("    -o[<baudrate>]      set output speed for authentic experience\n");
#endif /* !(GLK || QT) */
         puts ("Two information only options:");
         puts ("    -v                  print version info and exit");
         puts ("    -h                  print this usage summary\n");
         exit (0);
      }
      else if (*kwrd == 's')
      {
         cgi = 0;
         val = strtol (opt, &opt, 10);
         if (val == 0) val = 32767;
         if (val >= 16 && val <= 32767)  
         {
            Linlen = val;
            store_conf (WIDTH, opt);
         }
         if (*opt++)
         {
            val = strtol (opt, &opt, 10);
            if (val == 0) val = 32767;
            if (val >= 16 && val <= 32767) 
            {
               Screen = val;
               store_conf (HEIGHT, opt);
            }
            if (*opt++)
            {
               val = strtol (opt, &opt, 10);
               if (val >= 0 && val < (Linlen - 16 )/ 2)
               {
                  Margin = val;
                  store_conf (MARGIN, opt);
               }
            }
            Maxlen = Linlen - 2 * Margin;
         }               
      }
#ifdef USEDB
      else if (*kwrd == 'd')
      {
         dbs_dir = opt;
         break;
      }
#endif /* USEDB */
      else if (*kwrd == 'c')
      {
         com_name = opt;
         break;
      }
      else if (*kwrd == 'r')
      {
         if (*opt) dump_name = opt;
         break;
      }
#ifdef UNDO
      else if (*kwrd == 'u' && undo_def != -2)
      {
         if (oc == '0' || strcmp (opt, "off") == 0)
            undo_def = -1;
         else if (oc == 'n' || strcmp (opt, "forbid") == 0)
            undo_def = -2;
         else if (oc == '1' || strcmp (opt, "on") == 0)
            undo_def = 1;
      }
#endif /* UNDO */

#ifdef ADVCONTEXT
      else if (*kwrd == 'x' || *kwrd == 'y')
      {
         cgi = *kwrd;
         strncpy (cgicom, opt, sizeof (cgicom));
         if (*log_path == '\0')
         {
            strcpy (log_path, cgi_name + 1);
            strcat (log_path, ".log");
            store_conf (LOGFILE, UMODE);
         }
      }
#endif /* ADVCONTEXT */               
   }

#ifdef READLINE
   prompt_line = advalloc (2 * Maxlen + 1);
   prompt_ptr = prompt_line;
   *prompt_ptr = '\0';
#endif
#ifdef SLOW
   if (cgi)
      cps = 0;
   if (cps)
#  if defined(MSDOS) || defined(_WIN32)
      cps = 1000/cps;
#  else
      cps = 1000000/cps;
#  endif
#endif /* SLOW */
   if (cgi) 
      Margin = 0;
   if (mainseed == 0)
      time ((time_t *) &mainseed);
   rseed = mainseed %= 32768L;
   irand (1);

   if (com_name) new_game = 1;
      
   if (initialise (prog) != 0)
   {
      printf ("Sorry, unable to set up the world.\n");
#ifdef GLK
      glk_exit ();
#else
      return (0);
#endif
   }

#ifdef GLK
   glk_stylehint_set (wintype_TextBuffer, style_Normal, 
      stylehint_Justification, stylehint_just_LeftFlush);
   glk_set_style (style_Normal);
#endif
   value [THERE] = value [HERE] = FLOC;

   if (cgi < 'x' && !new_game)
   {
      if (! dump_name || ! *dump_name)
         special(999, &value [0]);
      if (value[0] == 999 && !com_name)
      {
         FILE *adl;
         char name [64];
         int c;
         PRINTF (RESTORING);
         sprintf (name, "%s.adl", CGINAME);
         if ((adl = fopen (name, RMODE)) != NULL)
         {
            while ((c = fgetc(adl)) != EOF)
               outchar (c & 0377);
            fclose (adl);
         }
         goto run_it;
      }
   }

#ifdef ADVCONTEXT
   if (cgi == 'x' && dump_name && *dump_name)
   {
      cgi_name = dump_name;
      special (997, &value [0]);
      cgi_name = CGINAME;
   }
   else if (cgi == 'y')
      special (999, &value [0]);
   else
#endif /* ADVCONTEXT */
   {
      if (dump_name && *dump_name)
      {
         INIT_PROC ();
         value [STATUS] = -1;
         value [ARG3] = -1;
         tindex = 1;
         tp[0] = dump_name;
         strncpy (arg2_word, dump_name, WORDSIZE);
         *(arg2_word + WORDSIZE - 1) = '\0';
      }
      else     
      {
         tindex = 0;
         tp [0] = NULL;
         if (setjmp (loop_back) == 0)
            INIT_PROC ();
#ifdef MLATEST
         {
            INIT_PROC ();
            if (cgi < 'x')
               special (999, &value [0]);
         }
#endif
      }
   }

run_it:
#ifdef UNDO
   if (undo_def == -2)
      bitmod ('s', UNDO_STAT, UNDO_NONE);
   else if (undo_def == 1)
      bitmod ('s', UNDO_STAT, UNDO_INFO);
   else if (undo_def == -1)
   {
      bitmod ('s', UNDO_STAT, UNDO_INFO);
      bitmod ('s', UNDO_STAT, UNDO_OFF);
   }
#endif /* UNDO */

   setjmp (loop_back);

   if (quitting) 
   {
      zap_cgi_dump ();
#ifdef QT
      close_files ();
      exit (0);
#else
      if (end_pause)
      {
          PRINTF ("(To exit, press ENTER)");
#ifdef GLK
         outbuf (1);
#else         
         getinput (comline, 160);
         ACDCHAR('\n');
#endif
      }
      else
      {
         if (text_len > 0)
            outbuf (1);
         ACDCHAR('\n');
      }
      close_files ();
#ifdef GLK
      glk_exit ();
#else
      if (cgi)
         zap_cgi_dump ();
      return (255);
#endif
#endif /* QT */
   }
   while (1)
   {
      rseed = mainseed;
      irand (1);
      mainseed = rseed;
      rseed = mainseed ^ 255;
      REPEAT_PROC ();
   }
}

/*===========================================================*/

#ifdef __STDC__
int have (int l1,int l2,int l3)
#else
int have (l1,l2,l3)
int l1;
int l2;
int l3;
#endif
{
   if (l1 > LOBJ) return (0);
   if (location [l1] != INHAND) return (0);
   if (l2 < 0) return (1);
   if (l2 == 0)
   {
      if (value [l1] == l3) return (1);
   }
   else
      if (bitest (l1, l3)) return (1);
   return (0);
}

/*===========================================================*/

#ifdef __STDC__
int isat (int l1,int l2,int l3, int l4)
#else
int isat (l1,l2,l3,l4)
int l1;
int l2;
int l3;
int l4;
#endif
{
   if (l1 > LOBJ) return (0);
   if (l2 != -1)
   {
      if (l2 == 0)
      {
         if (value [l1] != l3) return (0);
      }
      else
         if (!(bitest (l1, l3))) return (0);
   }
   if (location [l1] == l4) return (1);
#ifdef SCHIZOID
   if (l4 == INHAND || !(bitest (l1, SCHIZOID))) return (0);
   if (location [l1] + 1 == l4) return (1);
#endif
   return (0);
}

/*===========================================================*/

#ifdef __STDC__
int ishere (int l1,int l2,int l3)
#else
int ishere (l1,l2,l3)
int l1;
int l2;
int l3;
#endif
{
   return (isat (l1, l2, l3, value [HERE]));
}

/*===========================================================*/

#ifdef __STDC__
int isnear (int l1,int l2,int l3)
#else
int isnear (l1,l2,l3)
int l1;
int l2;
int l3;
#endif
{
   if (have (l1,l2,l3)) return (1);
   if (ishere (l1,l2,l3)) return (1);
   return (0);
}

/*===========================================================*/

#if !defined(NOVARARGS) && defined(__STDC__)
void move (int a1, int a2, ...)
{
   va_list ap;
   int arg_end;
   int kwrd;

   va_start (ap, a2);
   if (a2 < 0) goto gothere;
   arg_end = 0;
   while (!arg_end)
   {
      if ((kwrd = va_arg (ap, int)) < 0)
      {
         kwrd = -kwrd;
         arg_end = 1;
      }
      if (KEY (kwrd))
         goto gothere;
   }
   va_end (ap);
   return;
gothere:
   va_end (ap);
#else /* not (NOVARARGS && __STDC__)*/

/*===========================================================*/

#ifdef __STDC__
void move (int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,
           int a9,int a10,int a11,int a12,int a13,int a14,int a15,int a16)
#else
void move (a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16)
int a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16;
#endif
{
   int kwrd;

   if (a2 < 0) goto gothere;
   if ((kwrd = a3) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a3 < 0) return;
   if ((kwrd = a4) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a4 < 0) return;
   if ((kwrd = a5) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a5 < 0) return;
   if ((kwrd = a6) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a6 < 0) return;
   if ((kwrd = a7) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a7 < 0) return;
   if ((kwrd = a8) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a8 < 0) return;
   if ((kwrd = a9) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a9 < 0) return;
   if ((kwrd = a10) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a10 < 0) return;
   if ((kwrd = a11) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a11 < 0) return;
   if ((kwrd = a12) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a12 < 0) return;
   if ((kwrd = a13) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a13 < 0) return;
   if ((kwrd = a14) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a14 < 0) return;
   if ((kwrd = a15) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; else if (a15 < 0) return;
   if ((kwrd = a16) < 0) kwrd = -kwrd;
   if (KEY (kwrd)) goto gothere; return;
gothere:
#endif /* NOVARARGS && __STDC__ */

#if STYLE >= 11
   if (value [THERE] != value [HERE])
   {
      *word_buf = 3;
      *(word_buf + 1) = 0;
   }
#endif /* STYLE */
   value [THERE] = value [HERE];
   *bitword (THERE) = -1;
   value [HERE] = a1;
   *bitword (HERE) = -1;
#if defined (MOVED) && defined (STATUS)
   bitmod ('s', STATUS, MOVED);
#endif /* MOVED and STATUS */
   if (a2 < -2)
      a2 = -a2;
   if (a2 > 0)
      say (0, a2, 0);
   if (a2 != -1) longjmp (loop_back, 1);
   return;
}

/*===========================================================*/

#ifdef __STDC__
void apport (int l1,int l2)
#else
void apport (l1,l2)
int l1,l2;
#endif
{
#if defined (JUGGLED) && defined (STATUS)
   if (location [l1] == INHAND || l2 == INHAND)
      bitmod ('s', STATUS, JUGGLED);
#endif
   location [l1] =  (l2 <= LLOC || l2 == INHAND) ? l2 : value [l2];
   return;
}

/*===========================================================*/

#ifdef __STDC__
void set (char t1, int v1, char t2, int v2, int *lv, short *lb)
#else
void set (t1, v1, t2, v2, lv, lb)
int t1, v1, t2, v2;
int *lv;
short *lb;
#endif
{
   int val, bts, cur = 0;
  
   if      (t2 == 'e') { val = value [v2]; bts = 0; } /* OBJ, LOC, TEXT */
   else if (t2 == 'c') { val = v2; bts = 0; }                    /* CONSTANT */
   else if (t2 == 'v') { val = value [v2];                       /* VAR */
                       if (v2 == ARG1 || v2 == ARG2) bts = -1;
                       else bts = varbits [VARSIZE * (v2 - FVERB)]; }
   else  /* t2 == 'l'*/ { val = lv [v2];                            /* LOCAL */
                       bts = lb [VARSIZE * v2]; }
   if      (t1 == 'V') { value [v1] = val;                       /* VAR */
                       cur = varbits [VARSIZE * (v1 - FVERB)]; }
   else if (t1 == 'L') { lv [v1] = val;                             /* LOCAL */
                       cur = lb [VARSIZE * v1]; }
   else  /* t1 == 'E'*/ value [v1] = val;             /* OBJ, LOC, TEXT */
   
   if      (t1 == 'V') {                                         /* VAR */
      if      (bts == -1 && cur != -1) varbits [VARSIZE * (v1 - FVERB)] = -1;
      else if (bts != -1 && cur == -1) varbits [VARSIZE * (v1 - FVERB)] = 0; }
   else if (t1 == 'L') {                                            /* LOCAL */
      if      (bts == -1 && cur != -1) lb [VARSIZE * v1] = -1;
      else if (bts != -1 && cur == -1) lb [VARSIZE * v1] = 0; }
   else if (t1 == 'T')                                               /* TEXT */
   {
      val = text_info [2 * (v1 - FTEXT) + 1];
      if (val <= value [v1]) value [v1] = val - 1;
   }
}

/*===========================================================*/

#ifdef __STDC__
void lda (int l1, int l2)
#else
void lda (l1, l2)
int l1,l2;
#endif
{
   value [l1] = l2;
   *bitword (l1) = -1;
   textadr [l1] = textadr [l2];
   return;
}

/*===========================================================*/

#ifdef __STDC__
void eval (int l1, int l2)
#else
void eval (l1, l2)
int l1,l2;
#endif
{
   value [l1] = value [value [l2]];
   return;
}

/*===========================================================*/

#ifdef __STDC__
void deposit (int l1, int l2)
#else
void deposit (l1, l2)
int l1,l2;
#endif
{
    value [value [l1]] = (l2 > LVAR || l2 < FVAR) ? l2 :
       value [l2];
    return;
}

/*===========================================================*/

#ifdef __STDC__
void locate (int l1, int l2)
#else
void locate (l1, l2)
int l1,l2;
#endif
{
   value [l1] = location [(l2 < FVAR || l2 > LVAR) ? l2 :
      value [l2]];
   *bitword (l1) = -1;
   return;
}

/*===========================================================*/

#ifdef __STDC__
int evar (int l1)
#else
int evar (l1)
int l1;
#endif
{
   if (*bitword (l1) == -1)
      return value [l1];
   else
      return l1;
}

/*===========================================================*/

#ifdef __STDC__
int levar (int l1, int *l2, short *l3)
#else
int levar (l1, l2, l3)
int l1;
int *l2;
short *l3;
#endif
{
   if (*(l3 + VARSIZE * l1) == -1)
      return (*(l2 + l1));
   else
      return l1;
}

/*===========================================================*/

#ifdef __STDC__
void finita (void)
#else
void finita ()
#endif
{
#if !defined(MEMORY) && !defined(PRELOADED)
   fclose (text_file);
#endif /* MEMORY */
#ifdef LOC_STATS
   printf ("\n(Locates: demanded %ld (+%ld), faults %ld (+%ld))\n",
      locate_demands, locate_demands - old_demands, 
      locate_faults, locate_faults - old_faults);
   printf ("(Locate ratio %ld%%)\n", 
      (((1000 * locate_faults) / locate_demands) + 5) / 10);
#endif /* LOC_STATS */
#ifdef QT
   ACDCHAR(1);
   ACDCHAR('q');
   outbuf(1);
   exit (0);
#else
   quitting = 1;
   longjmp (loop_back, 1);
#endif /* QT */
}

/*===========================================================*/

#ifdef __STDC__
short *bitword (int a1)
#else
short *bitword (a1)
int a1;
#endif
{
   short *adr;
   
   adr = NULL;
   if (a1 <= LOBJ)
       adr = &objbits [OBJSIZE * (a1 - FOBJ)];
   else if (a1 <= LLOC)
      adr = &placebits [LOCSIZE * (a1 - FLOC)];
   else if (a1 <= LVAR)
      adr = &varbits [VARSIZE * (a1 - FVERB)];
   return (adr);
}

/*===========================================================*/

#ifdef __STDC__
void bitmod (char a1, int a2, int a3)
#else
void bitmod (a1, a2, a3)
char a1;
int a2;
int a3;
#endif
{
   short *bitadr;
   
   if (a2 > LVAR) 
   {
      printf (
         "*** Run-time glitch! Setting flag on a flagless entity %d! ***\n", a2);
      return;
   }
#if STYLE >= 20
   if (a2 <= LLOC)
   {
      if (a3 < 3)
      {
         printf (
            "*** Run-time glitch! Attempting to modify major type of entity %d!\n", a2);
         return;
      }
      a3 -= 3;
   }
#endif
   bitadr = bitword (a2);
   while (a3 > 15)
   {
      bitadr++;
      a3 -= 16;
   }
   if (a1 == 's')
      *bitadr |= 1 << a3;
   else
      *bitadr &= ~(1 << a3);
   return;
}
      
/*===========================================================*/

#ifdef __STDC__
void lbitmod (int a0, char a1, int a2, int a3, int *a4, short *a5)
#else
void lbitmod (a0, a1, a2, a3, a4, a5)
int a0;
char a1;
int a2;
int a3;
int *a4;
short *a5;
#endif
{
/* a0 locals
 * a1 type ('c', 's')
 * a2 entity
 * a3 flag
 * a4 *lval
 * a5 *lbts
 */
   short *bitadr;

   if (a2 < 0 || a2 >= a0)
   {
      printf (
         "*** Run-time glitch! Local entity %d >= locals %d! ***\n", a2, a0);
      return;
   }
   if (*(a5 + VARSIZE * a2) == -1)
   {
      a2 = *(a4 + a2);
#if STYLE >= 20
      if (a2 <= LLOC)
      {
         if (a3 < 3)
         {
            printf (
               "*** Run-time glitch! Attempting to modify major type of entity %d!\n", a2);
            return;
         }
         a3 -= 3;
      }
#endif
      bitadr = bitword (a2);
   }
   else
      bitadr = a5 + VARSIZE * a2;

   while (a3 > 15)
   {
      bitadr++;
      a3 -= 16;
   }
   if (a1 == 's')
      *bitadr |= 1 << a3;
   else
      *bitadr &= ~(1 << a3);
   return;
}
      
/*===========================================================*/

#ifdef __STDC__
int bitest (int a1, int a2)
#else
int bitest (a1, a2)
int a1;
int a2;
#endif
{
   short *bitadr;
   
   if (a1 > LVAR)
      return (0);
#if STYLE >= 10
   if (a1 <= LVERB)
   {
      if (a2 == 0)
         return (a1 <= LOBJ && a1 >= FOBJ);
      else if (a2 == 1)
         return (a1 <= LLOC && a1 >= FLOC);
      else if (a2 == 2)
         return (a1 >= FVERB);
#if STYLE >= 20
      a2 -= 3;
#endif
   }
#endif
   bitadr = bitword (a1);
   if (bitadr == NULL)
      return (0);
   while (a2 > 15)
   {
      bitadr++;
      a2 -= 16;
   }
   return (*bitadr & 1 << a2);
}

/*===========================================================*/

#ifdef __STDC__
int lbitest (int a1, int a2, int *a3, short *a4)
#else
int lbitest (a1, a2, a3, a4)
int a1;
int a2;
int *a3;
short *a4;
#endif
{
   short *bitadr;
   if (*(a4 + VARSIZE * a1) == -1)
   {
      a1 = *(a3 + a1);
      if (a1 <= LVERB)
      {
         if (a2 == 0)
            return (a1 <= LOBJ && a1 >= FOBJ);
         else if (a2 == 1)
            return (a1 <= LLOC && a1 >= FLOC);
         else if (a2 == 2)
            return (a1 >= FVERB);
   #if STYLE >= 20
         a2 -= 3;
   #endif
      }
      bitadr = bitword (a1);
   }
   else
      bitadr = a4 + VARSIZE * a1;
   if (bitadr == NULL)
      return (0);
   while (a2 > 15)
   {
      bitadr++;
      a2 -= 16;
   }
   return (*bitadr & 1 << a2);
}

/*===========================================================*/

#ifdef __STDC__
void flush_command (void)
#else
void flush_command ()
#endif
{
   value_all = 0;
   tp [tindex] = NULL;
   return;
}

/*===========================================================*/

#ifdef __STDC__
void shift_up (char *aptr)
#else
void shift_up (aptr)
char *aptr;
#endif
{
   if ((*aptr >= 'a' && *aptr < 'z') || (*aptr >= 'A' && *aptr < 'Z'))
      (*aptr)++;
   else if (*aptr == 'z' || *aptr == 'Z')
      *aptr -= 25;
}

/*===========================================================*/

#ifdef __STDC__
void shift_down (char *aptr, int maxlen)
#else
void shift_down (aptr, maxlen)
char *aptr;
int maxlen;
#endif
{
   char *sptr = aptr;
   while (maxlen-- && *aptr)
   {
      if ((*aptr > 'a' && *aptr <= 'z') || (*aptr > 'A' && *aptr <= 'Z'))
         (*aptr)--;
      else if (*aptr == 'a' || *aptr == 'A')
         *aptr += 25;
      aptr++;
   }
   if (log_file) fprintf (log_file, "TRANSLATION: %s\n", sptr);
}

/*===========================================================*/

#ifdef __STDC__
void tie (int text, int holder)
#else
void tie (text, holder)
int text;
int holder;
#endif
{
   text_info [2 * (text - FTEXT)] = TIED_TEXT;
   value [text] = holder;
}

/*===========================================================*/

#ifdef __STDC__
void svar (int type, int *var)
#else
void svar (type, var)
int type;
int *var;
#endif
{
   time_t now;
   struct tm *tp;
   switch (type)
   {
      case 4:
      case 5:
         now = time (NULL);
         tp = localtime (&now);         
         *var = (type == 4 ? tp -> tm_hour : tp -> tm_min);
         break;
        
      default:
         PRINTF2 ("GLITCH! Bad svar code: %d\n", type);
   }
   return;
}

/*===========================================================*/

#ifdef __STDC__
void glue_text (void)
#else
void glue_text();
#endif
{
/* Convert last message output into a fragment */
   while (*(lptr - 1) == '\n') lptr--;
}

/*===========================================================*/

#ifdef __STDC__
void zap_text (void)
#else
void zap_text();
#endif
{
/* Zap all of the accumulated text */
   lptr = text_buf;
   *lptr = '\0';
   text_len = 0;
}

/*===========================================================*/

#ifdef __STDC__
void verbatim (int arg)
#else
void verbatim(arg);
int arg;
#endif
{
   if (arg == ARG1)
      strncpy (arg1_word, tp [tindex - 1], WORDSIZE);
   else if (arg == ARG2 && value [STATUS] == 2)
      strncpy (arg2_word, tp [tindex - 1], WORDSIZE);
   else if (arg != ARG2)
   {
      PRINTF2 ("GLITCH! Bad ARGn indicator: %d\n", arg);
   }
}

/*===========================================================*/

#ifdef __STDC__
int test (char *type)
#else
int test (type);
char *type;
#endif
{
   if (strcmp(type, "cgi") == 0)
      return (cgi && cgi != 'b');
   if (strcmp(type, "doall") == 0)
      return (value_all);
   if (strcmp(type, "html") == 0)
#ifdef BROWSER
      return (cgi == 'b');
#else
      return (cgi);
#endif /* BROWSER */
      
   PRINTF2 ("GLITCH! Bad test type: %s\n", type);
   return (0);
}
#ifdef UNDO

/*===========================================================*/

#ifdef __STDC__
int checkdo (void)
#else
int checkdo ()
#endif
{
   char *a;
   int val = 0;

   bitmod ('c', UNDO_STAT, UNDO_BAD);
   bitmod ('c', UNDO_STAT, UNDO_TRIM);
   bitmod ('c', UNDO_STAT, UNDO_INV);
#ifdef ALL
   if (value [ARG2] == ALL)
      val = 32767;
   else if (value[STATUS] > 1)
#else
   if (value[STATUS] > 1)
#endif
   {
      a = arg2_word;
      while (*a)
      {
         if (*a < '0' || *a >'9')
         {
            bitmod ('s', UNDO_STAT, UNDO_BAD);
            return (-1);
         }
         val = 10 * val + *a++ - '0';
      }
   }
   else
      val = 1;
   return (val);
}

/*===========================================================*/

#ifdef __STDC__
int inv_check (int mode)
#else
int inv_check (mode)
int mode;
#endif
{
   int i;
   if (mode == 0)
   {
      memcpy (inhand, location, LOCS_SIZE * sizeof(int));
      return (0);
   }
   for (i = 0; i <= LOBJ; i++)
   {
      if ((inhand[i] == INHAND && location[i] != INHAND) ||
          (inhand[i] != INHAND && location[i] == INHAND))
             return (1);
   }
   return (0);
}

/*===========================================================*/

#ifdef __STDC__
void show_bits (int refno, int size)
#else
void show_bits (refno, size)
int refno;
int size;
#endif
{
   int i;
   fprintf (stderr, " - ");
   for (i = 0; i < 8 * size * sizeof(short); i++)
      fprintf (stderr, "%c", bitest (refno, i) ? '1' : '0');
}

/*===========================================================*/

#ifdef __STDC__
void show_data (void)
#else
void show_data ()
#endif
{
   int i;
   FILE *rrefs;
   char buf [80];
   
   rrefs = fopen ("game.rrefs", RMODE);
   
   for (i = FOBJ; i <= LTEXT; i++)
   {
      if (rrefs)
      {
         fgets (buf, sizeof(buf) - 1, rrefs);
         *(buf + strlen(buf) - 1) = '\0';
         fprintf (stderr, "%25s", buf + 8);
      }
      else
      {
         fprintf (stderr, "%4d", i);
      }
      
      fprintf (stderr, "%5d", *(value + i));
      if (i >= FOBJ && i <= LOBJ)
      {
         fprintf (stderr, " @ %4d", *(location + i));
         show_bits (i, OBJSIZE);
      }
      else if (i >= FLOC && i <= LLOC )
         show_bits (i, LOCSIZE);
      else if (i >= FVAR && i <= LVAR )
         show_bits (i, VARSIZE);
      fprintf (stderr, "\n");
   }
   
   if (rrefs)
      fclose (rrefs);
}


/*===========================================================*/

#ifdef __STDC__
void  undo (void)
#else
void undo ()
#endif
{
   int cnt;
   int acnt;
      
   if ((cnt = checkdo ()) < 0)
      return;
   if (diffsz == 0 || edptr <= diffs + 4)
      acnt = 0;
   else
   {
      inv_check (0);
      for (acnt = 0; acnt < cnt; acnt++)
      {
         if (edptr <= diffs + 4)
            break;
         edptr -= 4;
         while (edptr > diffs)
         {
            int adr;
            edptr -= 4;
            adr = 256 * (*edptr) + *(edptr + 1);
            if (adr)
               *(IMAGE + adr) = *(edptr + 2);
            else
               break;
         }
         edptr += 4;
      }
      bitmod (inv_check (1) ? 's' : 'c', UNDO_STAT, UNDO_INV);
   }

   value [UNDO_STAT] = acnt;
#ifdef ALL
   if (value [ARG2] == ALL)
      cnt = acnt;
#endif
   bitmod (cnt > acnt ? 's' : 'c', UNDO_STAT, UNDO_TRIM);
   memcpy (image, IMAGE, sizeof (IMAGE));
   return;
}

/*===========================================================*/

#ifdef __STDC__
void redo (void)
#else
void redo ()
#endif
{
   int acnt;
   int cnt;
   
   if ((cnt = checkdo ()) < 0)
      return;

   if (edptr == dptr)
      acnt = 0;
   else
   {
      inv_check (0);
      for (acnt = 0; acnt < cnt; acnt++)
      {
         if (edptr > dptr)
            edptr = dptr;
         while (1)
         {
            int adr = 256 * (*edptr) + *(edptr + 1);
            edptr += 4;
            if (adr)
               *(IMAGE + adr) = *(edptr - 1);
            else
               break;
         }
         if (edptr == dptr)
         {
            acnt++;
            break;
         }
      }
      bitmod (inv_check (1) ? 's' : 'c', UNDO_STAT, UNDO_INV);
   }
   value [UNDO_STAT] = acnt;
#ifdef ALL
   if (value [ARG2] == ALL)
      cnt = acnt;
#endif
   bitmod (cnt > acnt ? 's' : 'c', UNDO_STAT, UNDO_TRIM);
}
#endif /* UNDO */

/*===========================================================*/

#ifdef __STDC__
void pcall (int procno)
#else
void pcall (procno)
int procno;
#endif
{
   fprintf (stderr, 
      "\n*GLITCH* Called proc offest %d, but game's offset range is 0 to %d!\n",
         procno, LPROC);
#if STYLE >= 11
   if (procno == BADWORD || procno == AMBIGWORD ||
       procno == AMBIGTYPO || procno == SCENEWORD || procno == BADSYNTAX)
#else
#if STYLE > 1
   if (procno == BADWORD || procno == AMBIGWORD)
#  else
   if (procno == BADWORD)
#  endif
#endif
   {
      fprintf (stderr, "   Probable cause: ");
      if (procno == BADWORD)   fprintf (stderr, "bad word");
#if STYLE > 1
      if (procno == AMBIGWORD) fprintf (stderr, "ambiguous word");
#endif
#if STYLE >= 11
      if (procno == AMBIGTYPO) fprintf (stderr, "ambiguous typo");
      if (procno == SCENEWORD) fprintf (stderr, "mere scenery word");
      if (procno == BADSYNTAX) fprintf (stderr, "bad syntax");
#endif
      fprintf (stderr, " in player command not handled by game's code.\n");
   }
   fprintf (stderr, "\n");                  
}

/*===========================================================*/

#if STYLE >= 11

#ifdef __STDC__
int typed (char *string)
#else
int typed (string)
char *string;
#endif
{
   return (strcmp(orig1, string) == 0 || strcmp(orig2, string) == 0);
}
#endif /* STYLE */

/*************************************************************/
/* All the remaining code is strictly for direct browser display,
 * i.e. not even for remote cgi-bin operation.
 */
#ifdef BROWSER

#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#if defined (unix) || defined(__MACH__)
#  define SEP '/'
#endif /* unix */

#ifdef DEBUG
#  define TIMELIMIT 300
#  define INITTIME 2000
#else
#  define TIMELIMIT 3
#  define INITTIME 20
#endif

int lsock = 0;
int wsock = 0;
int port = 1978;
int timer = INITTIME;
int child = 0;

#define sclose close

/*====================================================================*/
/* This is the initial page to send to the browser, including all
 * the necessary JavaScrip code.
 */
char *page[] = 
{
   "<html><head><title>",
   "=G",
   "</title>",
   "<script type=\"text/javascript\">\n",
#ifdef DEBUG
   "debug = 1;\n",
   "timeLimit = 250000;\n",
#else
   "timeLimit = 2500;\n",
#endif /* DEBUG */
   "state = 0;\n",
   "=M",
   "interval = 0;\n",
   "=C",
   "rType = 0;\n",
   "turn = 1;\n",
   "wake = 0;\n",
   "hpos = 0\n",
   "hsiz = 0;\n",
   "hcom = [];\n",
   "conbot = 0;\n",
   "console = 0;\n",
   "command = 0;\n",
   "function scrollit()\n",
   "   { window.scrollTo(0, conbot.offsetTop); }\n",
   "function writeit(text)\n",
   "   { console.innerHTML += text; scrollit(); }\n",
   "   \n",
   "function zapit()\n",
   "   { document.getElementById('comform').innerHTML = ''; }\n",
#ifdef DEBUG
   "function report(text)\n",
   "   { writeit('<span class=\"debug\">' + text + '</span><br />'); }\n",
#endif /* DEBUG */
   "function shutit(text)\n",
   "{\n",
   "   window.clearInterval(interval);\n",
   "   zapit();\n",
   "   writeit(text + '<div class=\"red\"><center>' +\n",
   "      '<input class=\"custom-button\" ' +\n",
   "      'onclick=\"javascript:self.close()\" id=\"shutit\"' +\n",
   "      'type=\"button\" value=\"Close this tab/window\" /><br />' +\n",
   "      '<span class=\"red\">This button will only work if your ' +\n",
   "      ' browser<br /> is not being paranoid about security.</span></p>' +\n",
   "      '</center></div>');\n",
   "}\n",
   "function sendit(text)\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report(\"Sending '\" + text + \"'\");\n",
#endif /* DEBUG */
   "   var http = null;\n",
   "   try { http = new XMLHttpRequest(); }\n",
   "   catch(e) {\n",
   "      try { http = new ActiveXObject(\"Msxml2.XMLHTTP\"); }\n",
   "      catch(e) {\n",
   "         try { http = new ActiveXObject(\"Microsoft.XMLHTTP\"); }\n",
   "         catch(e) {\n",
   "            shutit('<h2>Sorry, your browser is not compatible with this game!</h2><br />');\n",
   "         }\n      }\n   }\n",
   "   http.onreadystatechange = function(e)\n",
   "      {\n",
   "         if (http.readyState == 4)\n",
   "         {\n",
   "            if (http.status == 200)\n",
   "               showit (http.responseText);\n",
   "            else\n",
   "               shutit('<h2>Server process not responding!</h2><br />');\n",
   "         }\n",
   "      };         \n",
   "   http.open('GET', '/?status=' + escape(text), true);\n",
   "   http.send(null);\n",
   "}\n",
   "function startit()\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Entering startit');\n",
#endif /* DEBUG */
   "   conbot = document.getElementById('conbot');\n",
   "   console = document.getElementById('console');\n",
   "   command = document.getElementById('command');\n",
   "   command.focus();\n",
   "   command.onkeydown = filter;\n",
   "   sendit (\"0x0\");\n",
   "   timeit();\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting startit');\n",
#endif /* DEBUG */
   "}\n",
   "function filter(event)\n",
   "{\n",
   "   var e = event || window.event;\n",
   "   var code = (e.charCode || e.keyCode);\n",
   "   if (code == 38 || code == 63232) \n",
   "      code = 1;\n",
   "   else if (code == 40 || code == 63233)\n",
   "      code = -1;\n",
   "   else \n",
   "      code = 0;\n",
   "   \n",
   "   if (code != 0)\n",
   "   {\n",
   "      hist(code);\n",
   "      return false;\n",
   "   }\n",
   "   return true;\n",
   "}\n",
   "function hist(step)\n",
   "{\n",
   "   var h = 2 * hpos;\n",
   "   hcom[h] = command.value;\n",
   "   hpos += step;\n",
   "   if (hpos < 0) hpos = 0;\n",
   "   if (hpos > hsiz) hpos = hsiz;\n",
   "   h = 2 * hpos;\n",
   "   if (! hcom[h + 1])\n",
   "      hcom[h + 1] = hcom[h];\n",
   "   command.value = hcom[h];\n",
   "   command.focus();\n",
   "   return;\n",
   "}\n",
   "function timeit()\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Entering timeit, state is ' + state + ', interval is ' + interval);\n",
#endif /* DEBUG */
   "   if (interval)\n",
   "      window.clearInterval(interval);\n",
   "   interval = 0;\n",
   "   if (state < 2)\n",
   "      interval = window.setInterval(doit, timeLimit, 2);\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting timeit, interval is now ' + interval);\n",
#endif /* DEBUG */
   "}\n",
   "function showit(text)\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Entering showit');\n",
#endif /* DEBUG */
   "   if (text == null || text == '')\n",
   "   {\n",
   "      if (state != 2)\n",
   "      {\n",
   "         writeit(\n",
   "            '<p><h2>Server process not responding!<br />' +\n",
   "            'Please close this window and restart.</h2></p>');\n",
   "      }\n",
   "      window.clearInterval(interval);\n",
   "      interval = 0;\n",
   "      state = 2;\n",
   "   }\n",
   "   else\n",
   "   {\n",
   "      var len = text.length;\n",
   "      var type = text.substr(len - 1, 1);\n",
   "      if (type == 'k')\n",
   "         return;\n",
   "      rType = type;\n",
   "      writeit(text.substr(0, len - 1));\n",
   "      if (type == 'f')\n",
   "         shutit('');\n",
   "      else\n",
   "      {\n",
   "         command.focus();\n",
   "         if (interval == 0)\n",
   "            timeit();\n",
   "      }\n",
   "   }\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting showit');\n",
#endif /* DEBUG */
   "}\n",
   "function doit(start)\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Entering doit');\n",
#endif /* DEBUG */
   "   if (state == 0)\n",
   "   {\n",
   "      state = 1;\n",
   "      startit();\n",
   "      start = 2;\n",
   "   }\n",
   "   var com = command.value;\n",
   "   var pars;\n",
   "   if (start < 2)\n",
   "   {\n",
   "      if (interval)\n",
   "      {\n",
   "         window.clearInterval(interval);\n",
   "         interval = 0;\n",
   "      }\n",
   "      pars = turn + 'x' + com;\n",
   "      hcom[0] = com;\n",
   "      turn++;\n",
   "      var prompt = document.getElementById('prompt');\n",
   "      if (prompt)\n",
   "      {\n",
   "         var el = document.createElement('span');\n",
   "         var tx = (rType == 'q' || rType == 'm' ||\n",
   "                    rType == 'r' || rType == 's') ?\n",
   "                       prompt.innerHTML : '?';\n",
   "         if (rType == 'c') tx = '<br />?';\n",
   "         el.className = 'query';\n",
   "         el.innerHTML = tx + ' ' + com +\n", 
   "             (rType == 't' ? '<br />&nbsp<br />' : '');\n",
   "         prompt.parentNode.replaceChild(el, prompt);\n",
   "      }\n",
   "      if (com)\n",
   "      {\n",
   "         var i;\n",
   "         for (i = 1; i < hcom.length; i += 2)\n",
   "         {\n",
   "            if (hcom[i])\n",
   "            {\n",
   "               hcom[i - 1] = hcom[i];\n",
   "               hcom[i] = '';\n",
   "            }\n",
   "         }\n",
   "         if (hcom[0] == hcom[2])\n",
   "            hcom[0] = '';\n",
   "         else\n",
   "         {\n",
   "            hcom.unshift('');\n",
   "            hcom.unshift('');\n",
   "            hsiz++;\n",
   "         }\n",
   "         command.value = '';\n",
   "      }\n",
   "      hpos = 0;\n",
   "   }\n",
   "   else\n",
   "   {\n",
   "      wake++;\n",
   "      pars = \"0x0\" + wake;\n",
   "   }\n",
   "   sendit(pars);\n",
   "   if (start != 2)\n",
   "   {\n",
   "      var allMoves = document.getElementsByName('chunk');\n",
   "      if (allMoves.length > 2 * moveLimit)\n",
   "      {\n",
   "         allMoves[0].parentNode.removeChild(allMoves[0]);\n",
   "         allMoves[0].parentNode.removeChild(allMoves[0]);\n",
   "         hcom.pop();\n",
   "         hcom.pop();\n",
   "         hsiz--;\n",
   "      }\n",
   "   }\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting doit');\n",
#endif /* DEBUG */
   "}\n",
   "</script><style type=\"text/css\">\nspan.query{color:",
   "=Q",
   "}\nspan.debug{color:",
   "=D",
   "}\nspan.red{color:red}\ntd{width:",
   "=W",
   "px}\n",
   "input{max-width:",
   "=T",
   "px}\ntd{background-color:",
   "=B",
   "}</style></head><body bgcolor=\"",
   "=S",
   "\" text=\"",
   "=F",
   "\" onLoad=\"javascript:doit(0);\">\n",
   "<center><table border=\"1\" cellpadding=\"16\"><tr><td>\n",
   "<div id=\"console\">\n",
   "</div>\n<div id=\"comform\">",
   "<form method=\"get\" action=\"javascript:doit(1)\" id=\"inform\">",
   "<span class=\"query\">?&nbsp;</span><input id=\"command\" size=\"",
   "=I",
   "\" ",
   "maxlength=\"160\" type=\"text\">\n",
   "=V",
   "<br /></form></div><div id=\"conbot\">",
   "</div></td></tr></table></center></body></html>\n",
   "=="
};

char *utf8 = "20 21!22\"23#25%26&27'28(29)2A*2B+2C,2F/3A:3B;3D=3F?40@5B[5D]";

/*====================================================================*/
/* Prepare the initial HTML page and send it off to the browser.
 */
void send_page(void)
{
   int i = 0;
   char mlimit [32];
   char compact [32];
   char tdlen [16];
   char inplen [16];
   
#ifdef DEBUG
   printf ("=== Entering %s\n", "send_page"); 
#endif /* DEBUG */
   sprintf (mlimit, "moveLimit = %d;\n", atoi (conf [COMMANDS]));      
   sprintf (compact, "compact = %d;\n", *conf[COMPACT] == 'Y' ? 1 : 0);
   sprintf (tdlen, "%d", atoi (conf [PXWIDTH]) - 20);      
   sprintf (inplen, "%d", 
      atoi (conf [PXWIDTH]) / 10 - (*conf[BUTTON] == 'Y' ? 10 : 0));
   optr = obuf + 84;
   while (1)
   {
      char *cptr = page[i++];
      if (*cptr == '=') 
      {
         int key = *(cptr + 1);
         if (key == '=')
            break;
         cptr = NULL;
         switch (key)
         {
            case 'G': cptr = GAME_NAME;         break;
            case 'Q': cptr = conf[PROMPT];      break; /* Prompt (query) colour */
            case 'D': cptr = conf[DBG];         break; /* Debug text colour */
            case 'B': cptr = conf[BG];          break; /* Background colour */
            case 'F': cptr = conf[FG];          break; /* Text colour */
            case 'S': cptr = conf[SURROUND];    break; /* Surround colour */
            case 'W': cptr = conf[PXWIDTH];     break; /* Table width */
            case 'T': cptr = tdlen;             break; /* Text display width */
            case 'I': cptr = inplen;            break; /* Input field lemgth */
            case 'M': cptr = mlimit;            break;
            case 'C': cptr = compact;           break;
            case 'V': if (*(conf [BUTTON]) == 'Y')
                         cptr = SUBMIT_BUTTON;  break;
            default:
               fprintf (stderr, "Bad substitution tag in page.html: ='%c'!", key);
               exit (1);
         }
      }
      if (cptr) oputs (cptr);
   }
   *optr = '\0';
   send_response_to_browser (obuf);
#ifdef DEBUG
   printf ("=== Exiting %s\n", "send_page"); 
#endif /* DEBUG */
}

/*====================================================================*/
/* Sets up listener socket lsock for browser operation.
 */
void set_up_listener (void)
{
   int one = 1;              /* Needed by setsockopt */
   struct protoent *pf;      /* Protocol family pointer */
   struct sockaddr_in ladr;  /* Local server address structure */
   int pcnt = 0;

   memset ((char *)&ladr, 0, sizeof(ladr));
   ladr.sin_family = AF_INET;
   ladr.sin_addr.s_addr = INADDR_ANY;

   if ((pf = getprotobyname ("tcp")) == NULL)
   {
      fprintf (stderr, "Failed to find protocol TCP!\n");
      exit (-1);
   }

   if ((lsock = socket (AF_INET, SOCK_STREAM, pf->p_proto)) < 0)
   {
      perror("");
      fprintf (stderr, "Failed to create listennig socket!\n");
      exit (-1);
   }

   setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, 
      (void *)&one, sizeof(int));

   while (1)
   {
      ladr.sin_port = htons (port);
      if (bind (lsock, (struct sockaddr *)&ladr, sizeof(ladr)) == 0)
         break;
      port++;
      pcnt++;
      if (pcnt == 100)
      {       
         fprintf (stderr,"Failed to bind listening socket!\n");
         exit (-1);
      }
   }

   if (listen (lsock, 10) < 0)
   {
      fprintf (stderr,"Failed to set up listener!\n");
      exit (-1);
   }
}


/*====================================================================*/
/* Generates the HTTP header for browser operation, based on the payload 
 * length given as a string in the <number> buffer.
 */
char *make_header (char *buf, char *number)
{
   char *cptr = buf + 84 - (77 + strlen (number));
   sprintf (cptr, "%s%s%s\n",
      "HTTP/1.0 200 OK\nServer: A-code/1.1\n",
      "Content-Type: text/html\nContent-Length: ",
      number);
   *(buf + 83) = '\n';
   return (cptr);
}

/*====================================================================*/
/* Send the assembled response to the browser in 1KB chunks. (Possibly
 * unnecessary to split it like that these days.)
 */
void send_response_to_browser (char *cptr)
{
   int size;
   char number [10];
   int len = strlen (cptr + 84);
   
#ifdef DEBUG
   printf ("=== Entering %s\n", "send_response_to_browser"); 
#endif /* DEBUG */
   sprintf (number, "%d", len);
   cptr = make_header (cptr, number);
   len = 1024;
   size = strlen (cptr);
   while (size > 0)
   {
      if (size < len)
         len = size;
      write (wsock, cptr, len);
      cptr += len;
      size -= len;
   }
#ifdef DEBUG
   printf ("=== Exiting %s\n", "send_response_to_browser"); 
#endif /* DEBUG */
}

/*====================================================================*/
/* Wait for the browser to call and either respond (to a keep-alive
 * request), or extract and return the player's command.
 */
int get_command_from_browser (char *command)
{
   int len;
   char combuf[4096];
   char *cptr = combuf;
   char *aptr;
   int lchar = ' ';
   int adrlen;               /* Length of socket address */
   struct sockaddr_in radr;  /* Remote client address structure */
   
#ifdef DEBUG
   printf ("=== Entering %s\n", "get_command_from_browser"); 
#endif /* DEBUG */
   while (1)
   {
      adrlen = sizeof(radr);
      alarm (timer);
      if (wsock) sclose (wsock);
      wsock = 0;
      if ((wsock = accept (lsock, (struct sockaddr *)&radr, 
                                  (socklen_t *)&adrlen)) < 0)
      {
         fprintf(stderr, "Failed to accept incoming call!\n");
         exit (-1);
      }
      alarm (0);
      
      cptr = combuf;
      while (1)
      {
         len = read (wsock, cptr, sizeof(combuf) - 1);
         *(cptr + len) = '\0';
         aptr = cptr + 1;
         lchar = *cptr;
         cptr += len;
         while (aptr < cptr)
         {
            if (lchar == '\n' && *aptr == '\n')
            {
               len = 0;
               break;
            }
            if (*aptr != '\r')
               lchar = *aptr;
            aptr++;
         }
         if (len == 0)
            break;
      }
   
      if (strstr (combuf, "IE 5.0"))
      {
         fprintf (stderr,
            "*ERROR* Sorry, this setup does not work with IE 5.0.\n");
         exit (1);
      }
         
      if (command == NULL) 
         return (strstr (combuf, "status=0x0") ? 0 : 1);

      if ((cptr = strstr (combuf, "status=")) != NULL)
      {
         int turn = 0;
         int scale = 1;
         char *bptr = strchr(cptr, 'x');
         aptr = 1 + bptr--;
         while (*bptr >= '0' && *bptr <= '9')
         {
            turn += (*bptr-- - '0') * scale;
            scale *= 10;
         }
         if (*(aptr - 2) == '0' && *aptr == '0')
         {
            char kbuf [90];
            strcpy (kbuf + 84, "k");
            send_response_to_browser (kbuf);
            continue;
         }
         break;
      }
   }

/* Got a command! */

   *command = '\0';
   cptr = command;
   while (*aptr != ' ' && *aptr != '\n')
   {
      if (*aptr == '%')
      {
         char *uptr = utf8;
         while (*uptr)
         {
            if (*uptr == *(aptr + 1) && *(uptr + 1) == *(aptr + 2))
            {
               *cptr++ = *(uptr + 2);
               aptr += 3;
               break;
            }
            uptr += 3;
         }
         if (*uptr)
            continue;
      }
      *cptr++ = *aptr;
      aptr++;
   }
   *cptr++ = '\n';
   *cptr = '\0';
   if ((cptr = strstr (command, "&_=\n")) != NULL)
   {
      *cptr++ = '\n';
      *cptr = '\0';
   }
#ifdef DEBUG
   printf ("=== Exiting %s: '%s'\n", "get_command_from_browser", command); 
#endif /* DEBUG */
   return (0);
}

/*====================================================================*/
/* Alarm signal handler.
 */
void terminate (int sig)
{
   if (! quitting && sig == SIGALRM)
      fprintf (stderr, "*** No response from browser! ***\n");
   exit (0);
}

/*====================================================================*/
/* Sets up the SIGALRM trap.
 */
void set_alarm_trap (void)
{
   struct sigaction addsig;         /* Signal info for sigaction () */
   sigset_t my_sigset;              /* Signal set mask */
   
/* Make sure SIGALRM and SIGCHLD not masked out! */

   sigemptyset (&my_sigset);
   sigaddset (&my_sigset, SIGALRM);
   sigaddset (&my_sigset, SIGCHLD);
   sigprocmask (SIG_UNBLOCK, &my_sigset, NULL);

/* Set ALARM handler and ignore SIGCHLD without leaving zombies */
   
   sigemptyset (&my_sigset);
   addsig.sa_mask = my_sigset;
   
   addsig.sa_handler = terminate;
   addsig.sa_flags = SA_NODEFER;
   sigaction (SIGALRM, &addsig, NULL);
   
   addsig.sa_handler = SIG_IGN;         /* Not intersted */
#if !(defined (MSDOS) || defined(_WIN32))
   addsig.sa_flags = SA_NOCLDWAIT;      /* No zombies */
   sigaction (SIGCHLD, &addsig, NULL);  /* Handle (ignore) CHILD */
#endif /* !(MSDOS || _WIN32) */
}

/*====================================================================*/
/* Invokes the specified browser.
 */
void invoke_browser (void)
{
   char urlbuf [256];

#ifdef _WIN32
   WSADATA wsaData;
   WSAStartup(0x0101, &wsaData);
#endif

#ifdef DEBUG
   printf ("=== Entering %s\n", "invoke_browser"); 
#endif /* DEBUG */

   set_up_listener ();

   if (isatty (1)) 
      fputs ("Invoking browser and detaching server process...\n", stderr);
   
   if ((child = fork ()) == 0)        /* I.e. this is the child process */
   {
      int fd;
      sclose (lsock);
      lsock = 0;
      sprintf (urlbuf, "http://localhost:%d/", port);
      fflush(stdout);
      close (0); close (1); fd = dup(2); close(2);
      execl (conf [BROWEXE], conf [BROWEXE], urlbuf, (char *)NULL);
      dup(fd); dup (fd);
      printf ("*ERROR* Failed to exec %s!\n", conf [BROWEXE]);
      exit (1);
   }

#if !(defined(MSDOS) || defined(_WIN32))
/* Daemonize, if necessary */
   if (getppid() != 1 && (child = fork ()) >= 0)
   {
      int i;
      if (child > 0) exit (0);   /* Parent process exits */
      setsid ();                 /* New process group */
      close (0); close (1); close (2); /* Close stdin, stdout, stderr */
      i = open ("/dev/null", O_RDWR); dup (i); dup (i); /* Reopen them */
   }
#endif

   set_alarm_trap ();
   (void) get_command_from_browser (NULL); /* Accept initial call (page request?) */
   do
      send_page();                          /* Send the initial page */
   while (get_command_from_browser (NULL)); /* Accept next call (AJAX request?) */
   timer = TIMELIMIT;
#ifdef DEBUG
   printf ("=== Exiting %s\n", "invoke_browser"); 
#endif /* DEBUG */
}

/*====================================================================*/

#endif

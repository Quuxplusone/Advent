/* adv00.c: A-code kernel - copyright Mike Arnautov 1990-2021, licensed
 * under GPL (version 3 or later) or the Modified BSD Licence, whichever
 * is asserted by the supplied LICENCE file. GPL3 if no licence file.
 */
#define KERNEL_VERSION "12.86, 04 Jan 2022"
/*
 * 04 Jan 22   MLA        BUG: use strncpy not memcpy to copy rdl!
 * 11 Nov 21   MLA        Updated HTML text centering to HTML 5.
 *                        Fixed centering of lines and blocks.
 * 10 Nov 21   MLA        Added handling of IFTURN (single turn mode).
 *                        Removed the rep/nrep mechanism as redundant.
 * 09 Nov 21   MLA        BUG: Set PROMPTED flag when resuming game.
 * 22 Aug 21   MLA        Added possible baud rates to the help text.
 * 01 Aug 21   MLA        Bug: say_buf scan should restore amatch!
 * 31 Jul 21   MLA        Allowed for barred name in place of PARSER.
 * 22 Jul 21   MLA        Bug: standardise arg1/2_word at WORDSIZE, not 20!
 * 08 Jul 21   MLA        Bug: Fix line centering ignoring 1st character.
 * 05 May 21   MLA        Added test for needcmd.
 * 19 Apr 21   MLA        Bug: Fix game invocation by name (not pathname).
 *                        Modified browser invocation to suit new Firefox.
 * 17 Apr 21   MLA        BUG: Fix executable pathname detection.
 * 04 Feb 21   MLA        BUG: place descriptions: added check of detail_flag.
 * 17 Aug 20   MLA        Detect and ignore spurious line-feeds if style < 10.
 * 05 Aug 20   MLA        UTF8 now fully supported.
 * 26 Jul 20   MLA        Renamed outchar() to outbyte(). Added new outchar().
 * 25 Jul 20   MLA        Added UTF8 compatibility.
 * 22 Jun 20   MLA        Allowed for change in definitions or THEN and AND.
 *                        Allowed dots in ENTNAME vocabulary search.
 *                        Changed . to mean ; only if followed by a space.
 * 28 May 20   MLA        bug: removed command parsing dependency on PLSCLARIFY.
 * 17 Mar 20   MLA        bug: parsing args: if mode is set to CGI, set it
 *                        to STARTGAME. 
 * 15 Mar 20   MLA        Regularised advturn's TEXT/HTML handling.
 * 04 Mar 20   MLA        Bug: ADVLIB's _LIST_ needs gothere() call!
 * 03 Jan 20   MLA        Slightly relaxed  UNDO structure checks.
 *                        bug: explicitly signal game end in single turn mode.
 * 26 Feb 19   MLA        Ditched redundant CONTEXT-specific response codes.
 * 25 Feb 19   MLA        Bug: history restore needs tighter structure check.
 * 14 Jan 19   MLA        Bug: always convert NBSP to either &nbsp; or a blank.
 * 29 Nov 18   MLA        Bug: never append binary junk to .adl files.
 * 07 Nov 18   MLA        Bug: orphans: don't ignore the highest refno object.
 * 02 Nov 18   MLA        Typo matching takes SEEN into account, if defined.
 * 31 Oct 18   MLA        bug: resolve UNDO/REDO/ADVCONTEXT ifdef dependebcies.
 *                        Bug: fix the "ing" check in word_update().
 * 28 Oct 18   MLA        bug: nested_say must reject out of bound refnos.
 * 16 Oct 18   MLA        bug: allow for there being just one declared object.
 * 11 Oct 18   MLA        Bug: typo-matching no longer acknowledges distant
 *                        objects.
 * 28 Sep 18   MLA        Bug: sceneword matching should not accept
 *                        abbreviations.
 * 19 Aug 18   MLA        Removed Adv770-specific parsing fudge.
 * 18 Aug 18   MLA        Bug: Fixed CGI move count oddities.
 *                        Removed -u invocation option as pointless.
 * 05 Jul 18   MLA        bug: Bail out of say() if text address is zero.
 * 24 Jun 18   MLA        Finally split input_store() from special().
 * 06 Jun 18   MLA        Stripped off line feeds from HTML output (for QT).
 *                        Fixed HTML handling of " in centered lines and blocks.
 * 02 Dec 17   MLA        Unwanted shutterm() exterminated.
 * 08 Nov 17   MLA        BUG: Don't reuse 'exact' in find_word.
 * 21 Jun 17   MLA        BUG: set html_ok if invoking a browser!
 * 09 Jan 17   MLA        bug: initialise html_ok to 0 if CONSOLE.
 * 05 Jan 17   MLA        Removed readline.h and history.h dependency.
 * 30 Dec 16   MLA        BUG: CGI: fixed save/restore of player command.
 * 28 Dec 16   MLA        BUG: Zero obuf on entry to advturn() in ADVLIB mode!
 * 08 Nov 16   MLA        BUG: Don't save undo changes in CGI unless HAVECMD!
 * 26 Aug 16   MLA        Bug: Don't skip saveing changes in the CGI mode!
 * 23 Aug 16   MLA        Bug: Forced correct memory alignement in JS mode.
 *                        Improved ADVLIB mode handling via rep and nrep.
 * 20 Aug 16   MLA        Bug: Don't insert "? " prompt in the ADVLIB mode!
 * 01 Aug 16   MLA        BUG: Eliminate noise words in parse() not in input()!
 * 30 Jul 16   MLA        Honour replacement text in vocab initialisation.
 * 23 Apr 16   MLA        Bug: Make sure test() and have() return 0 or 1!
 * 09 Apr 16   MLA        Replaced cgi with mode plus compilation symbols.
 *                        Offloaded build mode evaluation to adv0.h
 * 04 Apr 16   MLA        Replaced setjmp/jongjmp with the loop mechanism.
 * 31 Mar 16   MLA        Added ENTNAME support.
 * 08 Mar 16   MLA        bug: When splitting output lines suppress lead blanks.
 * 03 Mar 16   MLA        Removed non-ANSI C support.
 * 29 Feb 16   MLA        Added exec 17 and 18.
 *                        Added call to INIT_PROC() before in progress restore.
 * 28 Feb 16   MLA        BUG: say() now *does* handle non-pointer vars.
 * 24 Feb 16   MLA        On OSX, call shutterm(1) on exit.
 * 21 Feb 16   MLA        Offloaded some macro fiddling to adv0.h.
 * 18 Feb 16   MLA        Console mode on Macs: force terminal window close.
 * 10 Feb 16   MLA        Bug: fixed -C keyword for new Mac builds.
 * 05 Feb 16   MLA        Bug: fixed initial restore in library mode.
 *                        Bug: don't search vocabulary if no matching!
 * 03 Feb 16   MLA        Bug: test("html") should simply return 1 if JS!
 * 16 May 15   MLA        Bug: must get input if quitting but end_pause!
 * 21 Apr 15   BTB        Bug: need ios.h in adv00.c too!
 * 19 Apr 15   MLA        Bug: fixed block centering.
 *                        Adjusted centering to be slightly left biased.
 * 15 Apr 15   MLA        BUG: Oops!!! Reinstated call to read_conf.
 * 09 Jan 15   MLA        Truname now simply defaults to CGI_NAME.
 *                        Command line parsing split off into parse_args().
 * 05 Jan 15   MLA        Moved special character definitions to adv0.h
 *                        Bug: Don't translate TAG_START/END twice!
 * 30 Dec 14   MLA        Added emscripten support for JavaScript version.
 * 28 Dec 14   MLA        Bug: stopped file handle leak in ADVLIB mode.
 * 27 Dec 14   MLA        Renamed advmain to advturn() and simplified arguments.
 *                        BUG: call to zap_cgi_dump() must be unconditional!
 *                        Renamed zap_cgi_dump() to zap_dump().
 *                        bug: ADVLIB: always translate NBSP.
 * 24 Dec 14   MLA        Removed redundant call to zap_cgi_dump().
 * 22 Dec 14   MLA        Bug: set word_buf to NULL after freeing.
 *                        Bug: initialise IMAGE[] before calling INIT_PROC.
 *             BTB & MLA  Added IOS symbol.
 * 20 Dec 14   MLA        Bug: Corrected truname setting.
 * 08 Dec 14   MLA        -C suppresses HTML in CGI mode.
 * 09 Sep 14   MLA        Bug: fixed BROWSER NONE in conffile.
 * 30 May 14   MLA        Bug: prompt_line needs ifndef NO_READLINE.
 * 26 May 13   MLA        BUG: Don't restore undo history unless safe to do so.
 * 20 Apr 13   MLA        BUG: Don't use break when parsing comline!
 *                        Bug: unset CONTEXT on undo/redo.
 *                        bug: don't say Oops! if starting a CGI game.
 * 05 Apr 13   MLA        BUG: allow for readline() returning a null pointer!
 * 10 Mar 13   MLA        BUG: Use RMODE in try_db, not "r"!
 *                        Added MSDOS overrides.
 *                        DOS/Windows: return to original dir on normal exit.
 * 09 Mar 13   MLA        bug: Fixed swap usage reporting
 * 07 Mar 13   MLA        BUG: in db mode, open dat file *before* chdir()!
 * 03 Mar 13   MLA        bug: Fixed command line handling of saved games.
 * 11 Nov 12   MLA        Bug: Fixed Windows EOL in acode.conf.
 * 09 Nov 12   MLA        Bug: Fixed "started" bug in JS.
 * 04 Nov 12   MLA        Bug: Fixed conffile handling in console mode.
 *                        bug: Fixed 1st justified line being too short.
 * 03 Nov 12   MLA        bug: unset end_pause if invoking a browser.
 * 02 Nov 12   MLA        Bug: fixed NBSP handling in centred text.
 * 05 Oct 12   MLA        bug: always honour the right margin!
 *                        Bug: fixed conffile location code to work on Windows.
 * 30 Sep 12   MLA        Bug: Don't count log lines for screen length!
 * 17 Jun 12   MLA        Removed some unused symbols.
 * 14 May 12   MLA        BUG: Fixed STYLE 1 orphan handling.
 *                        BUG: Fixed STYLE 1 full display mode.
 * 18 Feb 12   A.Staniforth
 *                        Bug: Don't use AMBIGWORD in STYLE 1!
 * 24 Dec 11   MLA        Added BORDER config directive.
 *                        Background and text colour directives now
 *                        apply to input and submit button as well.
 * 23 Dec 11   MLA        Removed QT mode.
 *                        Readline now default, suppressed by NO_READLINE.
 *                        Slow capability now default, suppressed by NO_SLOW.
 * 21 Dec 11   MLA        Browser mode for Windows/MinGW (no Cygwin!).
 *                        Unwired HTTP header buffer size.
 * 14 Dec 11   MLA        Bug: defined HOME as "HOMEPATH" for Windows.
 * 08 Dec 11   MLA        Fixed "don't cache" http header.
 *                        Fixed JS code to allow for browser cacheing anyway!
 *                        Added WINDOWS symbol for CYGWIN builds.
 * 08 Dec 11   MLA        Stripped off GLK code.
 * 05 Aug 11   MLA        Replaced <br /> with <br>.
 * 09 Mar 11   MLA        Bug: Fixed log contents and log replay.
 *                        Bug: Also fixed IGNORE_EOL -- again!
 * 05 Mar 11   MLA        Finally finished A-code 12 CGI mode.
 * 12 Jan 11   MLA        Added MKDIR2ARG symbol check for own scripts.
 * 09 Jan 11   MLA        default_to() now sets ARG2 to indicate failure type.
 *                        bug: don't append NL to obuf if not wanted.
 * 20 Nov 10   MLA        Open .rrefs file in show_data() as <game_name>.rrefs.
 *                        Always write refno into the dump.
 *                        bug: Make dump usable without undo being defined.
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
 * 12 Feb 02   MLA        Added 'first' check in parse().
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

/* Define ADV00 to make adv0.h to include UTF8 stuff. */

#define ADV00 1
#include "adv0.h"

/* DOS executables are dumb console only */

#ifdef MSDOS
#  ifndef CONSOLE
#     define CONSOLE
#  endif
#  ifndef NO_READLINE
#     define NO_READLINE
#  endif
#  ifndef NO_SLOW
#     define NO_SLOW
#  endif
#  ifndef SWAP
#    define SWAP 32
#  endif
#endif

/* Deal with all possible combinations of handling the game's data set */

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

/* Windows does its own thing, of course! */

#if WINDOWS
#  define HOME "HOMEPATH"
#  define EOL "\r\n"
#  define ISKWRD '/'
#  define SEP '\\'
#else /* !WINDOWS */
#  define HOME "HOME"
#  define EOL "\n"
#  define ISKWRD '-'
#  define SEP '/'
#endif /* WINDOWS */

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#define RMODE "rb"
#define WMODE "wb"
#define UMODE "ab+"
/*
 * extern char *strncpy ();
 * extern void exit ();
 */

#if HTTP
   int http_offset = 0;
   char *brbuf;
   int brbuflen = 0;
#endif /* HTTP */

void shift_up (char *);
void shift_down (char *, int);
extern int process_saved (int, char *);

#ifndef NO_READLINE
char *readline(char *);
void add_history(char *);
char *prompt_line;
char *prompt_ptr;
#endif /* NO_READLINE */

#define COMLEN 4095

FILE *text_file;
FILE *gv_file;
FILE *log_file = NULL;
char log_path [COMLEN];

FILE *com_file = NULL;
char *com_name = NULL;
char *dump_name = NULL;

#ifndef USEDB
#  include "adv6.h"
#endif /* ! USEDB */

void outbuf (int);
int value_all;
int loop = 0;

#ifdef USEDB
   char *dbs_dir = NULL;
#endif /* USEDB */
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
#define IMSZ (1 + IMAGE_SIZE / sizeof (int))
int intIM [IMSZ];
unsigned char *IMAGE = (unsigned char *)intIM;
int *value = intIM;
int *location = (int *)((unsigned char *)intIM + OFFSET_LOCS);
short *objbits = (short *)((unsigned char *)intIM + OFFSET_OBJBIT);
short *placebits = (short *)((unsigned char *)intIM + OFFSET_LOCBIT);
short *varbits = (short *)((unsigned char *)intIM + OFFSET_VARBIT);
int *objlocs = NULL;
#ifdef UNDO
   int intim [IMSZ];
   unsigned char *image = (unsigned char *)intim;
   int *inhand = (int *)((unsigned char *)intim + OFFSET_LOCS);
   unsigned char *diffs = NULL;
   unsigned char *dptr = NULL;
   unsigned char *edptr;
   int diffsz = 0;
   int undo_def = 0;
#endif
char prog[128];
char *optr;
char *obuf = NULL;
int oblen;
char comline [COMLEN + 1] = "\n";
char old_comline [COMLEN + 1] = "\n";
char raw_comline [COMLEN + 1];
char long_word [COMLEN + 1];
int arg1, arg2;
int orphan;
int qvals [5];
int *qtext = &qvals[0];
int *qstat = &qvals [1];
int *qarg1 = &qvals [2];
int *qarg2 = &qvals [3];
int *qcon = &qvals [4];
char say_buf[COMLEN];
#define WORDSIZE 128
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

#if CONSOLE
int html_ok = 0;
#else
int html_ok = 1;
#endif

int compress;
char compact[2];

#if defined(PAUSE) || WINDOWS
   int end_pause = 1;
#else
   int end_pause = 0;
#endif /* PAUSE or WINDOWS */

#if CGI
int mode = CGI;
#else
int mode = 0;
#endif /* CGI */

char extcom [COMLEN];

int mode;
char truname [80];
char *autoname;

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

#if WINDOWS
  char odir [COMLEN];
#endif /* WINDOWS */

char *tp [100];
char separator [100];
short tindex;
int Screen = 24;
int Linlen = 80;
int Margin = 1;
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
char *scratch;
short *word_buf;
int upcase = 1;
char *lptr;
int text_len = 0;
int location_all;

int cps = 0;
#ifndef NO_SLOW
#  if WINDOWS
      extern void my_usleep (int);
#     define usleep(X) my_usleep(X)
#  else /* ! WINDOWS */
#     if defined(NO_UNISTD)
         void usleep(int);
#     endif
#  endif /* WINDOWS */
#endif /* NO_SLOW */
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

#define PRINTF(X)    { char *ptr = X; while (*ptr) outbyte(*ptr++); }
#define PRINTF1(X)   fputs(X, stdout); if (log_file) fputs(X,log_file);
#define PRINTF2(X,A) { char tmp[COMLEN]; char *ptr = tmp; \
                     sprintf(tmp,X,A); \
                     while (*ptr) outbyte(*ptr++); \
                     if (log_file) fprintf(log_file,X,A); }
#define PRINTF5(X,A,B,C,D) { char tmp[COMLEN]; char *ptr = tmp; \
                     sprintf(tmp,X,A,B,C,D); \
                     while (*ptr) outbyte(*ptr++); \
                     if (log_file) fprintf(log_file,X,A,B,C,D); }
#define LOGERROR(X)  if (log_file) fprintf(log_file,"ERROR: %d",X)
#define CHKSUM(X,Y)  for (cptr=(char *)X,cnt=1; \
                     (unsigned int)cnt<=(unsigned int)Y;cnt++,cptr++) \
                     {chksum+=(*cptr+cnt)*(((int)(*cptr)+cnt)<<4)+Y; \
                     chksum&=07777777777L;}

#define RESTORING "Restoring game in progress...\n\n"

char *conf[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

#define BROWEXE    0
#define BG         1
#define FG         2
#define PROMPT     3
#define DBG        4
#define SURROUND   5
#define BUTTON     6
#define COMPACT    7
#define HISTORY    8
#define PXWIDTH    9
#define LOGFILE   10
#define JUSTIFY   11
#define MARGIN    12
#define WIDTH     13
#define HEIGHT    14
#define BORDER    15
#define INITTIME  16
#define PINGTIME  17
#define GRACETIME 18

#define SUBMIT_BUTTON "&nbsp;&nbsp;<input type=\"submit\" name=\"Submit\", value=\"Submit\">"

int timeout = 100;

/******************************************************************/
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
/*===========================================================*/
int irand (int less_then)
{
   rseed = (((rseed << 10) + rseed) / 13) & 32767;
   return (rseed % less_then);
}
/*===========================================================*/
int jrand (int less_then)
{
   int rvalue;
   rvalue = (((rseed << 10) + (int) time (NULL)) / 13) & 32767;
   return (rvalue % less_then);
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

/*===========================================================*/
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

/* The payload here is the utf8_recase() function, supporting UTF8 charsets:
 *
 * int utf8_recase(int casing, char *string, int len, char *recased, int room)
 *
 *   casing   is the desired casing: 'L', 'U', or 'C' (for Capitalise)
 *   string   points to the string to be recased
 *   len      byte length of the string
 *   recased  points to buffer into which recased string is to be placed;
 *            this may be identical to the original 'string' buffer!
 *   room     gives the byte length of the buffer pointed at by 'recased'
 *
 * Returns the length of the recased string, or a negative value, indicating
 * the number of bytes not fitted into the tagret buffer.
 */

int utf8_bytes(unsigned c)
{
  unsigned mask;
  int bytes;
  if (c < 0xc0) return 1;  /* 0... is ASCII, 10... invalid, assume Latin1 */
  bytes = 2;               /* So at least two chars byte long */
  mask = 0x20;
  while (c & mask)
  {
     bytes++;             /* Count the leading ones, i.e. number of bytes */
     mask >>= 1;
  }
  return bytes;
}

#define BYTE_LEN(X) utf8_bytes(X)

int utf8_to_unicode(char *line, unsigned index, unsigned len, unsigned *res) 
{
/*
 * Convert a UTF-8 sequence to its unicode value, and return the length of
 * the sequence in bytes. NB Invalid UTF-8 will be converted to a one-byte
 * sequence.
 */
  if (index >= len) 
  {
    *res = 0;
    return 0;
  }

  unsigned value;
  unsigned char c = line[index];
  unsigned bytes, mask, i;

  *res = c;
  line += index;
  len -= index;
  if (c < 0xc0) return 1;  /* 0... is ASCII, 10... invalid, assume Latin1 */
  mask = 0x20;
  bytes = 2;               /* So at least two chars byte long */
  while (c & mask) 
  {
    bytes++;             /* Count the leading ones, i.e. number of bytes */
    mask >>= 1;
  }
  if (bytes > MAX_UTF8_LEN || bytes > len)  /* Invalid, so single byte */
    return 1;
  value = c & (mask-1);
  for (i = 1; i < bytes; i++) /* Extract unicode bits from those bytes */
  {
    c = line[i];
    if ((c & 0xc0) != 0x80) return 1;
    value = (value << 6) | (c & 0x3f);
  }
  if (value > MAX_UTF8_CHAR) return 1; /* Not valid, so single byte */
  *res = value;                        /* Unicode value to be returned */
  return bytes;                        /* The UTF8 encoding's length */
}

void reverse_string(char *begin, char *end)
{
 do 
 {
   char a = *begin, b = *end;
   *end = a; *begin = b;
   begin++; end--;
  } while (begin < end);
}

unsigned unicode_to_utf8(unsigned int c, char *utf8) 
{
/*
 * Convert a unicode value to its canonical utf-8 sequence.
 * NB: We do not check for (or care about) the "invalid" unicode values.
 */
  int bytes = 1;
  *utf8 = c;
  if (c >= 0x80)    /* It's multi-byte */
  {
    unsigned int prefix = 0x80;
    unsigned max = 0x3f;
    char *p = utf8;
    do 
    {
      *p++ = 0x80 + (c & 0x3f);
      bytes++;
      prefix = 0x80 | (prefix >> 1);
      max >>= 1;
      c >>= 6;            /* We use 6-bits in each extension byte */
    } while (c > max);
    *p = (prefix | c);      /* Add in the final byte */
    reverse_string(utf8, p);
  }
  return bytes;
}

int utf8_recase(int casing, char *string, int len, char *recased, int room)
{
  unsigned gph;
  unsigned *cm;
  static unsigned char *buf = NULL;
  static int blen = 0;
  unsigned char *ob;
  
  if (buf == NULL)
  {
    buf = advalloc (1024);
    blen = 1024;
  }
  if (blen < 4 * len)
  {
    blen = 4 * len;
    buf = advrealloc (buf, blen);
  }
  ob = buf;
  
  while (*string && len--)
  {
    if ((unsigned char)*string < 0x80 || 
      (len = utf8_to_unicode (string, 0, 4, &gph)) == 1)
    {
      *ob++ = casing == 'L' ? tolower(*string) : toupper(*string);
      string++;
    }
    else
    {
      cm = casing == 'L' ? u2l : l2u;     /* Which case mapping ? */
      string += len;
      while (*cm)
      {
        if (*cm == gph)       /* Found it */
        {
          gph = *(cm + 1);    /* Do the recasing */
          break;
        }
        if (*cm > gph)        /* Char not recasable */
          break;
        cm += 2;              /* Look at the next pair */
      }
      ob += unicode_to_utf8 (gph, ob);  /* Encode to UTF8 */
    }
    if (casing == 'C') casing = 'L'; /* Rest is lower case */
  }
  *ob = '\0';
  len = strlen(buf);
  if (len < room)
    strcpy (recased, buf);
  else if (len == room)
    strncpy (recased, buf, len);
  else
    len = room - len;
  return len;
}

/*===========================================================*/

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

#if defined(DEBUG) || defined(SCENERY)
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

// void btcopy (char *start, int len, short *new_rec)
// {
//    char *cptr = (char *) (new_rec + BT_TXT);
//    while (len--)
//       *cptr++ = tolower (*start++);
//    *cptr = '\0';
// }

int btcompare (char *word, char *cword)
{
   int cchar, wchar;
   while (*word || *cword)
   {
      wchar = *word++;
      if (!isalpha (wchar))
         return (*cword ? -1 : 0);
      cchar = (unsigned char) *cword++;
      if (wchar != cchar)
         return (wchar > cchar ? 1 : -1);
   }
   return (*cword);
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
   char wbuf [80];
#if UTF8 == 1
   len = utf8_recase ('L', word, len, wbuf, 80); 
#else
   strncpy (wbuf, word, len);
   *(wbuf + len) = 0;
   recase (wbuf, 'L');
#endif /* UTF8 */
#ifdef SCENERY
fprintf(stderr, "ADDING '%s'\n", wbuf);
#endif /* SCENERY */
   int parent = 0;
   int child = *(root + 1);
   int dir;
   short *new_rec;
   int reclen = BT_TXT  + 1 + len / 2;
   if (*(root + 1) > 0)
   {
      while (child > 0)
      {
#ifdef SCENERY
fprintf(stderr, "COMPARING '%s'\n", (char *)(root + child + BT_TXT));
#endif /* SCENERY */
         if ((dir = btcompare (wbuf, (char *)(root + child + BT_TXT))) == 0)
            return (root);
         parent = child;
         child = *(root + child + BT_UP + dir);
      }
   }
   if (*root + reclen > BT_SIZE &&
      (BT_SIZE > 65535L - reclen || (root = btinit (root)) == NULL))
         return (NULL);
   new_rec = root + (child = *root);
   memset ((char *)new_rec, '\0', reclen * sizeof (short));
   *(new_rec + BT_UP) = parent;
   *(new_rec + BT_UP + BT_LSIB) = *(new_rec + BT_UP + BT_RSIB) = 0;
   *(new_rec + BT_BAL) = 0;
   strncpy ((char *)(new_rec + BT_TXT), wbuf, len);
//   *((char *)(new_rec + BT_TXT + len)) = '\0';
//   btcopy (wbuf, len, new_rec);
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
void word_update (void)
{
   char *aptr = text_buf;
   char *zptr;
   static int last_here = 0;

   if (value [HERE] != last_here)
   {
      *(word_buf + 1) = 0;            // Zap the accumulated list!
      last_here = value [HERE];
   }
   while (1)
   {
      while (*aptr && ! isalpha (*aptr)) aptr++;
      if (*aptr == '\0') break;
      zptr = aptr + 1;
      while (*zptr && isalpha (*zptr)) zptr++;
      if (zptr - aptr > 4 &&
         !(*(zptr - 3) == 'i' && *(zptr - 2) == 'n' && *(zptr - 1) == 'g'))
            btadd (word_buf, aptr, zptr - aptr);
      if (*zptr == '\0') break;
      aptr = zptr + 1;
   }
#ifdef SCENERY
   btshow (word_buf, "Collected words");
#endif /* SCENERY */
   return;
}
#endif /* STYLE */

#define MAX_BREAKS 100

/* PRIVATE */
#if PLAIN && (defined(MEMORY) || defined(PRELOADED))
#  define get_char(X) (unsigned char)text[X]
#else /* !PLAIN || (!MEMORY && !PRELOADED)*/
/*===========================================================*/
unsigned char get_char (int char_addr)
{
#if !PLAIN
   int mask;
#endif /* !PLAIN */
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
   fprintf (stderr, "Wanted %6d.  Buffer %3d: from %dK.\n",
      char_addr, index, new_addr / TEXT_CHUNK);
#endif /* LOC_STATS */
   if (chunk_start [index] > chunk_end [index])
      file_oops ();

gotit:
   chunk_age [index] = locate_demands;
#if PLAIN
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
#if PLAIN
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
void file_oops (void)
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
void voc (int word, int what, int test, int vtext)
{
   int tc;
   static int vc = 0;

   if (word == 0)
      vc = 0;
   else
   {
      if (what == 0)
         what = word;
      if (test >0 && bitest (what, test) == 0)
         return;
      if (vc++ > 0)
      {
         outbyte (',');
         outbyte (' ');
      }
   }
   if (vtext)
      say (0, vtext, 0);
   else if (word)
   {
      ta = textadr [word];
      tc = get_char (ta);
      if (tc == '!')
         tc = get_char (++ta);
      while (tc != '\0')
      {
         outbyte (tc);
         tc = get_char (++ta);
      }
   }
}
/*===========================================================*/
#if CONSOLE
int scrchk (char *iptr)
{
   static int lincnt = 0;
   char reply [COMLEN];

   if (mode != CONSOLE) return (0);
   if (!iptr || com_file)
   {
      lincnt = 0;
      return (0);
   }

   while (*iptr && *iptr != '\n') iptr++;

   if (*iptr == '\n' && ++lincnt >= Screen - (compress ? 1 : 2))
   {
      if (strcmp(iptr + 1, "? ") == 0)
         return (0);
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
#ifdef NO_READLINE
         strcpy (reply + Margin, "? ");
         PRINTF1 (reply);
#else
         memset (prompt_line, ' ', Margin);
         strcpy (prompt_line + Margin, "? ");
#endif /* NO_READLINE */
         return (1);
      }
   }
   return (0);
}
#endif /* CONSOLE */
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
void showline (char *cptr)
{
#ifdef NO_SLOW
   fputs (cptr, stdout);
   fflush (stdout);
#else
   if (cps)
   {
      while (*cptr)
      {
         usleep (cps);
         putchar (*cptr++);
         fflush (stdout);
      }
   }
   else
   {
      fputs (cptr, stdout);
      fflush (stdout);
   }
#endif /* NO_SLOW */
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

#ifndef NO_READLINE
   if (target == 'L')
      fputc (c, fh);
   else
   {
      *prompt_ptr++ = c; *prompt_ptr = '\0'; /* Add to potential prompt line */
      if (c == '\n')                         /* It wasn't a prompt line! */
      {
         if (*prompt_line)                   /* So print it */
            showline (prompt_line);
         prompt_ptr = prompt_line;
         *prompt_ptr = '\0';                 /* Set prompt line to null */
      }
   }
#else
   if (cps && target != 'L')
   {
      usleep (cps);   /* Pause for slow output */
      fputc (c, fh);
      fflush (stdout);                          /* Make sure slowness is visible */
   }
   else
      fputc (c, fh);
#endif /* NO_READLINE */
}
/*====================================================================*/
/* Output character filter to do special things.
 */
char *ofilter (char *iptr, int html)
{
   if (*iptr == TAG_START)
   {
      if (!html) while (*iptr && *iptr != TAG_END) iptr++;
      else oputc ('<');
   }
   else if (!html) oputc (*iptr);
   else if (*iptr == '"') oputs("&quot;");
   else if (*iptr == '>' || *iptr == '<')
      oputs (*iptr == '<' ? "&lt;" : "&gt;");
   else if (*iptr == TAG_END) oputc ('>');
   else
   {
      if (*iptr == ' ' && *(iptr + 1) == ' ' && html) oputs ("&nbsp;");
      oputc(*iptr);
   }
   return (iptr);
}
/*====================================================================*/
/* Centers lines individually.
 */
char *centre_text_lines (char *iptr, int html)
{
   char *jptr;
   while (1)
   {
      iptr++;
      while (*iptr == ' ') iptr++;
      jptr = iptr;
      while (*iptr && *iptr != '\n' && *iptr != BLOCK_END)
         iptr++;
      if (!html)
      {
         int l = (Maxlen - (iptr - jptr)) / 2 - 2;
         if (l < 0) l = 0 ;
         while (l--) oputs (html ? "&nbsp;" : " ");
      }
      while (jptr < iptr)
      {
         if (*jptr == NBSP) oputs (html ? "&nbsp;" : " ");
         else if (*jptr != '\n') ofilter (jptr, html);
         jptr++;
      }
      oputs (html ? "<br>" : "\n");
      if (*iptr != '\n')
         break;
   }
   return (iptr);
}
/*====================================================================*/
/* Called both in cgi and non-cgi modes. Works out and discards the
 * shortest blank lead. In non-cgi mode also works out the longest
 * remaining line length and centers all lines as if they had that length,
 * thereby actually centering the block as a whole.
 */
char *centre_block (char *iptr, int html)
{
   int lead = 4096;
   int mxl = 0;
   int i;
   char *jptr = ++iptr;
   int offset;
   char *bptr;
   
   while (1)
   {
      bptr = jptr;
      while (*jptr == ' ') jptr++;
      if (*jptr && *jptr != '\n' && *jptr != BLOCK_END &&
         jptr - bptr < lead) lead = jptr - bptr;
      bptr = jptr;
      while (*jptr != '\n' && *jptr != BLOCK_END) jptr++;
      if (jptr - bptr > mxl) mxl = (jptr - bptr);
      if (!*jptr || *jptr == BLOCK_END) break;
      jptr++;
   }
   offset = (Maxlen - mxl) / 2 - 2;
   if (offset < 0) offset = 0;
   while (1)
   {
      if (*iptr == BLOCK_END) break;
      for (i = 0; i < lead; i++) iptr++;
      if (!html) for (i = 0; i < offset; i++) oputc (' ');
      while (*iptr != '\n' && *iptr != BLOCK_END)
      {
         if (*iptr == NBSP) oputs (html ? "&nbsp;" : " ");
         else if (*iptr == '\n') oputs ("<br>");
         else iptr = ofilter (iptr, html);
         iptr++;
      }
      if (*iptr != BLOCK_END)
         oputs (html ? "<br>" : "\n");
      else
         break;
      iptr++;
   }
   if (!html) oputc ('\n');

   return (*iptr == BLOCK_END ? iptr + 1 : iptr);
}
/*====================================================================*/
void format_buffer (int terminate, int html)
{
   char *iptr = text_buf;
   char *bptr = lptr;
   int frag = (bptr > text_buf && *(bptr - 1) != '\n');
   int type;
   int ignore_eol = 0;

#if HTTP
   optr = obuf + http_offset + (html ? 1 : 0);
#else
#if CONSOLE
   optr = obuf;
#else /* !CONSOLE */
   optr = obuf + 1;
   *obuf = 't';
#endif /* CONSOLE */
#endif /* HTTP */

/* Prepare the output opening and skip any introductory line feeds. */

   while (*iptr == '\n')
      iptr++;
   if (frag && html)
   {
      bptr--;
      if (*bptr == ' ' || *bptr == NBSP) bptr--;
   }

/* Now process the text buffer */

   while (iptr < bptr)
   {
      switch (*iptr)
      {
         case '"':
            if (html)
               oputs ("&quot;");
            else
               oputc (*iptr);
            break;
         case '\n':
            if (STYLE < 10 && iptr > text_buf && 
                islower (*(iptr + 1)) && islower (*(iptr - 1)))
            {
              oputc(' ');
              break;
            }
            if (ignore_eol)
            {
               ignore_eol = 0;
               break;
            }
            if (!html)
            {
               if (compress < 2 || *(optr - 1) != '\n')
               oputc ('\n');
            }
            if (*(iptr + 1) == '\n')
            {
               if (html)
                  oputs (compress > 1 ? "<br>" : "<br>&nbsp;<br>");
               else
               {
                  if (compress < 2)
                     oputc ('\n');
               }
               while (*(iptr + 1) == '\n') iptr++;
            }
            else if (html && *(iptr + 1))
               oputs ("<br>");
            break;
         case NBSP:
            if (html) oputs ("&nbsp;");
            else oputc (' ');
            break;
         case QUOTE_START:
         case BLOCK_START:
         case CENTRE_START:
            if (html)
            {
               if (*iptr != CENTRE_START)
                 oputs ("<table style='margin:auto' width=automatic><tr><td>");
               else
                 oputs("<div style='text-align:center'>");
               if (*iptr == QUOTE_START) oputs ("<pre>");
            }
            type = *iptr;
            if (type == CENTRE_START)
               iptr = centre_text_lines (iptr, html);
            else
               iptr = centre_block (iptr, html);
            if (html)
            {
               if (type == QUOTE_START) oputs ("</pre>");
               if (type != CENTRE_START)
                 oputs ("</td></tr></table>");
               else
                 oputs ("</div>");
               if ((type == CENTRE_START || compress > 1) && *iptr)
               {
                  if (*(iptr + 1) == '\n') iptr++;
                  if (compress > 1 && *(iptr + 1) == '\n') iptr++;
               }
            }
            break;
         case IGNORE_EOL:
            ignore_eol = 1; /* Ignore the next EOL char */
            break;
         case BLOCK_END:    /* Should not happen at all! */
            break;          /* Ignore */
         default:
            iptr = ofilter (iptr, html);
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
            "<span class='query' id='prompt'>" : "<span> ");
         oputc (c);
         oputs (" </span>");
         if (!compress) oputs ("<br>&nbsp;<br>");
      }
      else
      {
         if (!compress && bptr > text_buf) oputs("<br>&nbsp;<br>");
         oputs ("<span class='query' id='prompt'></span>");
      };
      if (quitting)                          tf = 'f';   /* Finish! */
      else if (frag)                         tf = 'q';   /* Query */
      else if (mode == HTTP && compress)     tf = 'c';   /* Compressed text */
      else                                   tf = 't';   /* Ordinary text */
#if HTTP
      *(obuf + http_offset) = tf;
#else
      *obuf = tf;
#endif /* HTTP */
   }
   else if (frag)
   {
      char *eptr = lptr - 1;
      if (*(optr - 1) == NBSP && !html_ok)
        *(optr - 1) = ' ';
      else if (eptr >= text_buf && *eptr != ' ' && *eptr != NBSP)
         oputc (' ');
#if !CONSOLE
      *obuf = 'q';
#endif /* CONSOLE */
   }
   else if (!quitting)
   {
      if (!compress) oputc('\n');
      oputs ("? ");
   }
#if CGI
   else
     *obuf = 'f';
#endif /* CGI */
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
   extra = need % cnt;           /* This many breaks need one more */
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
#if HTTP
   char *iptr = obuf + http_offset;
#else
   char *iptr = obuf;
#endif /* HTTP */
   char *jptr = iptr;
   char *bptr;
   char c;
   int i;
   int mx = mode != CONSOLE ? 80 : Maxlen;
   int mg = mode != CONSOLE ? 0 : Margin;

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
#ifndef NO_READLINE
      prompt_ptr = prompt_line;
      *prompt_ptr = '\0';
#endif /* NO_READLINE */
      if (*jptr == '\n' || *jptr == '\0')
      {
         for (i = 0; i < mg; i++) showchar (' ', t);
         while (iptr < jptr) showchar (*iptr++, t);
         if (*jptr == '\0') break;
         showchar (*iptr++, t);
#if CONSOLE && !HTTP
         if (*(iptr - 1) == '\n' && t != 'L' && scrchk (iptr)) break;
#endif /* CONSOLE */
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
      while (*iptr == ' ') iptr++; /* When splitting line ignore lead blanks */
      if (justify)
         stretch_line (iptr, t);
      else
         while (*iptr)
         {
            showchar (*iptr++, t);
#if CONSOLE && !HTTP
            if (*(iptr - 1) == '\n' && t != 'L' && scrchk (iptr)) break;
#endif /* CONSOLE */
         }
      if (c && *(bptr - 1) != '\n') showchar ('\n', t);
      if (!c) break;
      if (jptr < bptr) *jptr = ' ';    /* Restore the blank */
      *bptr = c;                       /* Restore whatever it was */
      iptr = bptr;
#if CONSOLE && !HTTP
      if (t != 'L' && scrchk (iptr)) break;
#endif /* CONSOLE */
   }
}
/*====================================================================*/
void outbuf (int terminate)
{
#if HTTP
   int first = 0;     /* Default to not sending HTTP header */
#endif /* HTTP */
   if (!obuf)
   {
#if HTTP
      first = 1;      /* Just starting, so may need to send the header */
#endif /* HTTP */
      obuf = advalloc (4096);
      oblen = 4096;
   }
#if HTTP
   if (first)         /* Send HTTP header is only just starting */
   {
      if (mode == HTTP)
      {
         end_pause = 0;
         html_ok = 1; /* Browser mode, so need HTML! */
         http_offset = sizeof (HTTP_HEADER) + 8;
         invoke_browser (conf [BROWEXE], atoi (conf [INITTIME]));
      }
      else
         puts ("");
   }
#endif /* HTTP */
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
   }

   if (log_file || mode == CONSOLE)
      format_buffer (terminate, 0);    /* Non-HTML format for the log */
   if (log_file)
   {
      outtext ('L');
#ifndef NO_READLINE
      if (log_file && mode != CONSOLE)
         fputs(prompt_line, log_file);
#endif /* NO_READLINE */
   }
   if (mode == CONSOLE)
      outtext ('C');
   else
   {
      format_buffer (terminate, html_ok);    /* Need HTML format */
#if CONSOLE || CGI
#ifndef NO_READLINE
      prompt_ptr = prompt_line;
      *prompt_line = '\0';
#endif /* NO_READLINE */
#if HTTP
      browser_write (obuf);
#else /* !HTTP */
      fputs (obuf, stdout);
      fflush (stdout);
#endif /* HTTP */
#endif /* CONSOLE || CGI*/
   }
/* Add words uttered by the guide to the list of scene words. */
#if STYLE >= 11
   word_update();
#endif
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

void nested_say (int addr, int key, int qualifier)
{
   int refno;
   int type;

   if (key &= 14)
      key = 8;
   type = get_char (addr++);
   refno = get_char (addr++) << 8;
   refno |= (get_char (addr) & 255);
   if (refno < 0 || refno > LTEXT)
   {
      printf ("GLitch! Nested variable refno out of range!\n");
      return;
   }
   if (type == 0)
      key = VOC_FLAG;
   else if (type == 1)
      key = VOC_FLAG;
   else if (type == 3)
   {
      key = VAR_FLAG | VALUE_FLAG;
      qualifier = 0;
   }

   if (refno == ARG1 || refno == ARG2)
   {
      char *cptr = (refno == ARG1) ? arg1_word : arg2_word;
      while (*cptr) outbyte (*cptr++);
   }
#ifdef ENTNAME
   else if (refno == ENTNAME)
   {
      int ta = ent_name [value [ENTNAME]];
      int tc = get_char(ta);
      while (tc)
      {
        outbyte(tc);
        tc = get_char (++ta);
      }
   }
#endif /* ENTNAME */
   else
   {
      if (type < 2) qualifier = 0;
      say (key, refno, qualifier);
   }
   ta = addr;
}
#endif /* NEST_TEXT */
/*===========================================================*/
void say (int key, int what, int qualifier)
{
   int index;
   int offset;
   int value_flag;
   int qual_flag;
   int var_what_flag;
   int var_qual_flag;
   int quip_flag;
#if STYLE < 10
   int brief_flag = 0;
#endif /* STYLE */
   int voc_flag;
// #if defined(DETAIL) || STYLE >= 11
   int detail_flag = 0;
// #endif /* DETAIL */
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

/* NB: The below flag values are hard-coded in acdc's dominor.c! */

   if (lptr == NULL) lptr = text_buf;
   locate_demands++;
   quip_flag =     key & QUIP_FLAG;
#if defined(DETAIL) || STYLE >= 11
   detail_flag =   key & DETAIL_FLAG;
#endif
   qual_flag =     key & QUAL_FLAG;
   var_qual_flag = key & VQUAL_FLAG;
   var_what_flag = key & VAR_FLAG;
   value_flag =    key & VALUE_FLAG;
   voc_flag =      key & VOC_FLAG;

   if (var_what_flag)
   {
      int tmp = value [what];
#if STYLE >= 11
      if ((what != ARG1 && what != ARG2 && what != ARG3) ||
         (tmp != BADWORD && tmp != AMBIGWORD && tmp != AMBIGTYPO &&
         tmp != SCENEWORD && tmp != BADSYNTAX))
#else
#  if STYLE < 10
      if ((what != ARG1 && what != ARG2 && what != ARG3) || tmp != BADWORD)
#  else
      if ((what != ARG1 && what != ARG2 && what != ARG3) ||
         (tmp != BADWORD && tmp != AMBIGWORD))
#  endif
#endif /* STYLE */
      {
         if (*bitword(what) != -1)    /* Not a pointer! */
         {
            char valbuf[32];
            sprintf(valbuf, "%d", tmp);
            cp = valbuf;
            while (*cp)
               outbyte(*cp++);
            return;
         }
         what = tmp;
      }
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
            outbyte (*cp++);
         return;
      }
      ta = textadr [what];
   }
#if LLOC > FLOC
   else if (what >= FLOC)
   {
#if STYLE < 10
      if (bitest (STATUS, TERSE) ||
         (bitest (what, BEENHERE) && bitest (STATUS, BRIEF) &&
         !bitest (STATUS, FULL)))
            ta = brief_desc [what];
      else
#else /* STYLE >= 10 */
#  if defined(TERSE) && defined(FULL) && defined(BEENHERE)
      if (!detail_flag && (bitest (STATUS, TERSE) ||
         (!bitest (STATUS, FULL) && bitest (what, BEENHERE))))
            ta = brief_desc [what];
      else
#  endif /* TERSE... */
#endif /* STYLE */
         ta = long_desc [what];
   }
#endif /* LLOC > FLOC */
#if LOBJ >= FOBJ
   else if (what >= FOBJ)
   {
#if defined(DETAIL) && STYLE == 10
      if (bitest (STATUS, DETAIL))
         ta = detail_desc [what];
      else
#endif
#if STYLE >= 11 || defined(DETAIL)
      if (detail_flag)
         ta = detail_desc [what];
      else
#endif
      if (location [what] == INHAND)
      {
#if STYLE < 10
         brief_flag = 1;
#endif /* STYLE */
         ta = brief_desc [what];
      }
      else
         ta = long_desc [what];
   }
#endif /* LOBJ >= FOBJ */
   if (ta == 0 || (tc = get_char (ta)) == '\0') goto shutup;

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
#if STYLE < 10
      else if (qualifier == ARG2 && value [ARG2] < BADWORD)
#else
      else if (qualifier == ARG2 && value [ARG2] >= 0)
#endif  /* STYLE */
         textqual = value [value [qualifier]];
   }

   if (!qual_flag)
      qualifier = (what <= LLOC) ? value [what] : what;

   while (tc != '\0')
   {
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
#if STYLE < 10
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
#if STYLE < 10
            ta = ta - 1 + 2 *
               ((swqual > switch_size) ? switch_size - 1 : swqual - 1);
            if (what < FTEXT) ta += 2;
#else /* STYLE >= 10 */
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
               outbyte (*cp);
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
               outbyte (*cp++);
            goto next_char;
         }
         else if (qualifier == ORIG)
         {
            cp = orig;
            while (*cp != '\0')
               outbyte (*cp++);
            goto next_char;
         }
#else /* STYLE < 11 */
         else if (qualifier == ARG1 || qualifier == ARG2)
         {
            cp = (qualifier == ARG1 ? arg1_word : arg2_word);
            while (*cp != '\0')
               outbyte (*cp++);
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
               outbyte (tc);
               tc = get_char (++ta);
            }
            ta = auxa;
         }
      }
      else
         outbyte (tc);
next_char:
      tc = get_char (++ta);
   }

shutup:
   if (quip_flag) loop = 1;
   return;
}
/*===========================================================*/
int outchar (unsigned char *cptr)
{
#if UTF8 == 0
  outbyte (*cptr);
  return 1;
#else /* UTF8 == 1 */
  int i, len;
  char utf8buf [8];
  int lead = *cptr >> 5;
  if (*cptr < 127 || (lead != 7 && lead != 6 && lead != 4))
  {
    outbyte (*cptr);  /* Not a UTF8 character */
    return 1;
  }
  len  = lead == 4 ? 2 : lead - 3;
  (void) 
  utf8_recase ('U', cptr, len, utf8buf, 8);
  for (i = 0; i < len; i++)
    outbyte (*(utf8buf + i));
  return  len;
#endif /* UTF8 */
}
/*===========================================================*/
void outbyte (unsigned text_char)
{
   if (!text_buf)
   {
      text_buf = advalloc (text_buf_len);
      lptr = text_buf;
   }

   if (text_len == text_buf_len - 16)
   {
      text_buf_len += 1024;
      text_buf = advrealloc (text_buf, text_buf_len);
      lptr = text_buf + text_len;
   }


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
      {
         if (text_char >= 'a' && text_char <= 'z')
            text_char += 'A' - 'a';
      }
      upcase = 0;
   }
   else if (isdigit (text_char))
      upcase = 0;
   else if (strchr (".!?", text_char))
      upcase = 1;
#endif /* STYLE >= 11 */

   text_len++;
   *lptr = text_char;

#ifdef DWARVEN
   if (value [DWARVEN] || extra_dwarvish)
      shift_up (lptr);
#endif /* DWARVEN */
   lptr++;
   *lptr = '\0';
   return;
}

#ifdef UNDO
/*===========================================================*/
void save_changes (void)
{
   unsigned char *iptr;
   unsigned char *optr;
   unsigned int cnt;
   int some = 0;

   if (value [ARG1] <= BADWORD || value [ARG2] <= BADWORD ||
#ifdef ADVCONTEXT
       value [ADVCONTEXT] > 1 ||
#if !CGI
       mode == HAVECMD ||
#endif
#endif
#ifdef REDO
       value [ARG1] == UNDO || value [ARG1] == REDO)
          return;
#else
       value [ARG1] == UNDO)
          return;
#endif
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
      for (cnt = 0, optr = image, iptr = IMAGE; cnt < IMAGE_SIZE;
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
#ifdef LEGACY
            if (cnt || CONSOLE)       /* ? Why CONSOLE ? (MLA) */
#endif
            if (cnt)
            {
               *dptr++ = cnt / 256;
               *dptr++ = cnt % 256;
               *dptr++ = *optr;
               *dptr++ = *iptr;
               some++;
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
   memcpy (image, IMAGE, IMAGE_SIZE);
}
#endif /* UNDO */
/*===========================================================*/
void close_files (void)
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
      log_file = NULL;
   }
#if STYLE >= 11
   if (word_buf) free (word_buf);
   word_buf = NULL;
#endif
}
/*===========================================================*/
void getinput (char *inbuf, int insize)
{
   char *cptr;
#ifndef NO_READLINE
   if (!*prompt_line)
   {
      memset (prompt_line, ' ', 20);
      strcpy (prompt_line + Margin, "? ");
   }
#endif
#ifdef DWARVEN
   extra_dwarvish = 0;
#endif
#ifdef ADVCONTEXT
   if (text_len > 3 && value [ADVCONTEXT] == 0 &&
      !query_flag)
#else /* !ADVCONTEXT */
   if (text_len > 3 && !query_flag && !quitting)
#endif /* ADVCONTEXT */

   {
      FILE *adl;
      char name[64];
      special (998, &value [0]);
      sprintf (name, "%s.adl", truname);
      if ((adl = fopen(name, WMODE)) != NULL)
      {
         char *cptr = text_buf;
         int len = strlen (RESTORING);
         while (*cptr == '\n')
            cptr++;
         if (strncmp (cptr, RESTORING, len) == 0)
            cptr += len;
         fputs(cptr, adl);
         fclose (adl);
      }
   }
#if defined(NO_READLINE) && !ADVLIB
   if (!*text_buf && !quitting)
      PRINTF (compress ? "? " : "\n? ");
#endif /* NO_READLINE && ! ADVLIB */
   if (mode != HTTP && mode != HAVECMD && *text_buf)
   {
      outbuf (0);
#if CONSOLE
      scrchk (NULL);
#endif /* CONSOLE */
   }
#if STYLE >= 11
   upcase = 1;
#endif /* STYLE >= 11 */
   *inbuf = '\0';

#ifdef PROMPTED
   bitmod ('s', ADVCONTEXT, PROMPTED);
#endif
   if (mode == STARTGAME || mode == NEEDCMD)
   {
      if (!query_flag)
        special (998, &value [0]);
#if ADVLIB || CGI
      if (loop) loop++;
      else loop = 2;
      return;
#else
#if STYLE >= 11
      if (word_buf) free (word_buf);
      word_buf = NULL;
#endif /* STYLE >= 11 */
      exit (0);
#endif /* ADVLIB  || CGI */
   }
   if (mode == HAVECMD)
   {
      strncpy (inbuf, extcom, insize - 1);
      mode = NEEDCMD;
      if (log_file)
         fprintf (log_file,"\nREPLY: %s\n", inbuf);
      return;
   }
#
   query_flag = 0;
   if (com_file)
   {
      char mybuf [256];
      char lastbuf [256];
      while (1)
      {
#ifndef NO_READLINE
         if (mode != CONSOLE)
         {
            printf ("%s", prompt_line);
            *prompt_line = '\0';
         }
#endif /* NO_READLINE */
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
         *lastbuf = '\0';
         if (strncmp (mybuf, "REPLY:", 6) == 0)
         {
            strncpy (inbuf, mybuf + 6, insize);
            if (mode != CONSOLE)
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
#if HTTP
   if (com_file == NULL && mode == HTTP)
   {
      outbuf (0);
      (void) browser_read (inbuf, timeout);
   }
#endif /* HTTP */
   if (com_file == NULL && mode != HTTP && (!quitting || end_pause))
#ifdef NO_READLINE
      fgets (inbuf, insize, stdin);
#else
   {
      char *rdl;
      if (mode != CONSOLE)
      {
         fgets (inbuf, insize, stdin);
      }
      else
      {
         showline (prompt_line);
         putchar ('\r');
         rdl = readline (prompt_line);
         if (!compress) putchar ('\n');
         if (rdl)
         {
            strncpy(inbuf, rdl, insize);
            add_history (rdl);
            free (rdl);
            *(inbuf + insize - 1) = 0;
         }
         else
            *inbuf = 0;
      }
   }
   if (log_file && mode != HTTP)
   {
      fputs (prompt_line, log_file);
      fflush (log_file);
   }
#endif /* NO_READLINE */
#ifdef ADVCONTEXT
   if (mode == STARTGAME) mode = NEEDCMD;
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
#ifndef NO_READLINE
   *prompt_line = '\0';
#endif
}
/*===========================================================*/
void advcpy (char *word_buff, int word_addr)
{
   int wlen;

   if (get_char (word_addr) == '!')
      word_addr++;
   for (wlen = 1; wlen <= WORDSIZE; wlen++)
      if ((*word_buff++ = get_char (word_addr++)) == '\0')
         return;
}
/*===========================================================*/
void fake (int which_arg, int refno)
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
/* key: 0 is "default", 1 is initial "doall", 2 is continued "doall" */
void default_to (int key, int place, int type)
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
         if (fits >= 0)
         {
#if STYLE >= 10
            value [ARG2] = AMBIGWORD;
#else
            value [ARG2] = BADWORD;
#endif
            return;   /* Can't happen if key > 0 */
         }
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
      arg2_word [WORDSIZE - 1] = '\0';
      value [STATUS] = 2;

#ifdef IT
      if (!value_all)       /* I.e. not in a DOALL loop! */
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
   value [ARG2] = 0;
   return;
}
/*===========================================================*/
#if STYLE >= 11
#ifdef TYPO
void report_typo (int ovoc, int olen)
{
   char save_char;

   strncpy (orig, tp [tindex], WORDSIZE);
#ifdef DWARVEN
   if (value [DWARVEN]) shift_down (orig, WORDSIZE);
#endif /* DWARVEN */
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
#endif /* TYPO */
/*===========================================================*/
int allowed (int which_arg, int refno)
{
  if (which_arg == 1) return (refno > LOBJ);
#ifdef SEEN
  return (bitest (refno, SEEN));
#else
  return (location [refno] == INHAND || location [refno] == value [HERE]);
#endif
}
/*===========================================================*/
int advcmp (unsigned char *word, int vocnum)
{
  unsigned char vchar;
  int va = voc_addr [vocnum];
  if (get_char (va) == '!') va++; // Guaranteed to be 1 byt long
  while (1)
  {
    vchar = get_char (va);
    if (*word != vchar || vchar == '\0') break;
    word += BYTE_LEN (*word);
    va += BYTE_LEN (vchar);
  }
  return (vchar - *word);
}
/*===========================================================*/
#ifdef DEBUG
void show_word(int index)
{
  int va = voc_addr [index];
  int ch;
  while ((ch = get_char (va++)) != 0)
    fputc (ch, stderr);
}
#endif
/*===========================================================*/
void find_word (int *type, int *refno, int *tadr, int which_arg, int gripe)
#else /* STYLE < 11 */
void find_word (int *type, int *refno, int *tadr, int which_arg)
#endif /* STYLE */
{
   int bottom, middle, top;
#if STYLE >= 10
   int old_refno;
   int exact;
#endif /* STYLE */
#if STYLE >= 11
   int old_type;
   int old_tadr;
   int ovoc;
   int olen;
#endif
   int va;
   unsigned char *wp;
   unsigned char ch;
   int ra;
   char myword [WORDSIZE];

   *type = -1;
   strncpy (myword,
      which_arg == 0 ? orphan_word : tp [tindex], WORDSIZE);
   if (amatch < 0)
   {
      *refno = BADWORD;
      goto done;
   }
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
         va++;    // Guaranteed to be 1 byte long
      wp = myword;
#ifdef DEBUG
   fprintf (stderr, "+++ %d '", bottom);
   if (bottom > -1)
      show_word(bottom);
   else
      fputc('-', stderr);
   fprintf (stderr, "' => %d '", middle);
   show_word(middle);
   fprintf (stderr, "' => %d '", top);
   if (top <= VOCAB_SIZE)
      show_word(top);
   else
      fputc('-', stderr);
   fputs("'\n", stderr);
#endif /* DEBUG */
      while ((ch = get_char (va)) == *wp)
         if (*wp != '\0')
         {
            wp += BYTE_LEN (*wp);
            va += BYTE_LEN (ch);
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
#if STYLE >= 10
   old_refno = BADWORD;
#endif /* STYLE */

   while (bottom < top)
   {
      wp = myword;

      if (get_char (va = voc_addr [bottom]) == '!')
#if STYLE < 10
        va++;
#else
      {
         va++;      // Guaranteed to be 1 byte
         exact = 1;
      }
      else
         exact = 0;
#endif /* STYLE */
      ra = va;
      while (*wp == (ch = get_char (ra)))
      {
         if (*wp == '\0')
            break;
         else
         {
            ra += BYTE_LEN (ch);
            wp += BYTE_LEN (*wp);
         }
      }
      if (*wp != '\0') break;
#if STYLE >= 10
      if (!exact || (exact && get_char (ra) == '\0'))
#else
      if (get_char (ra) == '\0' || ra - va >= 5)
#endif
      {
         *type = voc_type [bottom];
         *refno = voc_refno [bottom];
         *tadr = voc_word [bottom];
         if (get_char (ra) == '\0')
#if STYLE < 10
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
#else /* STYLE < 10 */
            goto done;
         if (old_refno != BADWORD && *refno != old_refno)
#if STYLE >= 11
         {
            int oref = allowed (which_arg, old_refno);
            int nref = allowed (which_arg, *refno);

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
         old_tadr = *tadr;
         old_type = *type;
#endif /* STYLE >= 11 */
         old_refno = *refno;
#endif /* STYLE */
         if (get_char (va) == '\0') break;
      }
      bottom++;
   }

#if STYLE >= 11

#ifdef ENTNAME
   {
      int i;
      char cp;
      int match = 0;
      for (i = 0; i <= LVAR; i++)
      {
         if (i >= LVERB && i < FVAR)
            continue;
         wp = myword;
         va = *(ent_name + i);
         if (va == 0)
            continue;
         while (1)
         {
            cp = get_char (va);
            if (*wp != cp) break;
            if (*wp == '\0')
            {
               match = 1;
               break;
            }
            va++;
            wp++;
         }
         if (match)
         {
            *refno = i;
            *tadr = *(ent_name + i);
            if (i <= LOBJ)
               *type = 0;
            else
               *type = 1;
            goto done;
         }
      }
   }
#endif /* ENTNAME */

   if (*refno == BADWORD && amatch > 0)
   {
      char *bp = NULL;
      int ba = 0;
      old_refno = -1;
      for (bottom = 0; bottom < VOCAB_SIZE; bottom++)
      {
         if (get_char (va = voc_addr [bottom]) == '!' ||
             get_char (va) == '|')  // Barred names fail to match!
            continue;
         wp = myword;
         while (*wp == get_char (va))
            { wp++; va++; }
         if (*wp == '\0' && get_char (va + 1) == 0 &&
               voc_type [bottom] != NOISE &&
                 allowed (which_arg, voc_refno [bottom]))
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
               voc_type [bottom] != NOISE &&
                 allowed (which_arg, voc_refno [bottom]))
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
               voc_type [bottom] != NOISE &&
                 allowed (which_arg, voc_refno [bottom]))
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
               voc_type [bottom] != NOISE &&
                 allowed (which_arg, voc_refno [bottom]))
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
               voc_type [bottom] != NOISE &&
                 allowed (which_arg, voc_refno [bottom]))
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
/* Scene matching only done if full matching allowed. */
      if (which_arg > 1 && amatch > 0 &&
        (*refno == BADWORD || *refno == AMBIGTYPO) &&
          btfind (word_buf, tp [tindex]))
            *refno = SCENEWORD;
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
void parse (void)
{
   char *cptr, *lptr;
   char sep;

#if UTF8 == 1
   (void) utf8_recase('L', comline, COMLEN, comline, COMLEN);
#else
   recase (comline, 'L');
#endif /* UTF8 */
   cptr = lptr = comline;
   while (*cptr == ' ' || *cptr == ',' || *cptr == ';' ||
         (*cptr == '.' && *(cptr + 1) != '.' && *(cptr + 1) != '/' &&
                          *(cptr + 1) != '\\'))
      cptr++;
   while (*cptr)
   {
      if (*cptr == '.' && (*(cptr + 1) == '.' || *(cptr + 1) == '/' ||
                           *(cptr + 1) == '\\' || *(cptr + 1) == ' '))
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
                             *(cptr + 1) != '\\' && *(cptr + 1) != ' '))
      {
         if (*cptr == '.' && *cptr == ' ')
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

/* Convert ANDs/THENs into comma/semicolon separators.
 */
//      if (strcmp (tp [tindex], AND) == 0)
//         separator [tindex] = ',';
//      else if (strcmp (tp [tindex], THEN) == 0)
//         separator [tindex] = ';';
//      else
      {
/*
 * Check for comma and semicolon equivalent, plus ignore noise words.
 * This used to be done in input(), hence the repeated calls to find_word.
 * Unfortunately, the logic of object iteration got wobbly as the result
 * and this is the simplest cure. I really ought to rewrite all the command
 * parsing stuff. :-(
 */
         int type, refno, tadr;
#if STYLE >= 11
         find_word (&type, &refno, &tadr, 1, 0);
#else
         find_word (&type, &refno, &tadr, 1);
#endif /* STYLE */
         if (type == COMMA || -refno == COMMA)
            separator [tindex] = ',';
         else if (type == SEMICOLON || -refno == SEMICOLON)
            separator [tindex] = ';';
         else if (type != NOISE) tindex++;
      }
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
void input (int textref)
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

   amatch = 1;       /* Full vocabulary matching */
#if STYLE >= 10
   if (value[STATUS] == NO_MATCH)
      amatch = -1;      /* No scene or abbreviation matching */
   else if (value [STATUS] == NO_AMATCH)
      amatch = 0;       /* No typo or scene matching */
#endif /* STYLE */


#if STYLE >= 11
#if defined (ALL) && defined (EXCEPT)
   if (value_all == 0)
      except_count = 0;
#endif
   *orig = '\0';
#endif /* STYLE */
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
      strncpy (orphan_word, arg1_word, WORDSIZE);
   }
#if defined(ADVCONTEXT) && STYLE >= 11
   else if (ADVLIB && *orphan_word && orphan == 0)
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
      outbyte ('\n');
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
         strncpy (old_comline, raw_comline, COMLEN);
      comline [0] = '\0';
      while (comline [0] == '\0' || comline [0] == '\n')
      {
#ifdef AGAIN
         done_again = 0;
#endif /* AGAIN */
#ifdef LOC_STATS
         fprintf (stderr, "(Locates: %d (+%d), faults %d (+%d))\n",
            locate_demands, locate_demands - old_demands,
            locate_faults, locate_faults - old_faults);
         old_demands = locate_demands;
         old_faults = locate_faults;
#endif /* LOC_STATS */
#if CONSOLE
         scrchk (NULL);
#endif /* CONSOLE */
         if (textref)
            say (0, textref, 0);
         if (! lptr) lptr = text_buf;
         getinput (comline, COMLEN);
         if (loop) return;
         strncpy (raw_comline, comline, COMLEN);
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
#if CONSOLE
      scrchk (NULL);
#endif /* CONSOLE */
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
#endif /* STYLE */
   tindex++;

#ifdef PLSCLARIFY
   if ((bitest (STATUS, PLSCLARIFY) && refno <= LLOC) || continuation)
#else
   if (continuation)
#endif
   {
      value [ARG1] = arg1;
      value [ARG2] = refno;
      value [STATUS] = 2;
      goto got_command;
   }
   value [ARG1] = refno;
   value [STATUS] = 1;

#if STYLE >= 11
   *orig2 = '\0';
#endif
   if (separator [tindex] == ' ' && tp [tindex])
   {
#if defined(FSPECIAL) && defined(LSPECIAL)
      int bmatch = amatch;
      if (amatch == 1)
         amatch = 
           (value [ARG1] < FSPECIAL || value [ARG1] > LSPECIAL) ? 1 : -1;
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
         amatch = bmatch;
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
      value [ARG2] = refno;
      value [STATUS] = 2;
   }

got_command:
   if (value [STATUS] == 1 && orphan)
   {
      if ((orphan > LOBJ && value [ARG1] <= LOBJ) ||
          (orphan <= LOBJ && value [ARG1] > LOBJ))
      {
         value [STATUS] = 2;
         value [ARG2] = orphan;
         strncpy (arg2_word, orphan_word, WORDSIZE);
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
         strncpy (comline, old_comline, COMLEN);
         strncpy (raw_comline, old_comline, COMLEN);
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
#if STYLE > 10
       || value [ARG1] == AMBIGWORD || value [ARG2] == AMBIGWORD
#endif
#if STYLE >= 11
       || value [ARG1] == AMBIGTYPO || value [ARG2] == AMBIGTYPO
#endif
                                                                 )
         tp [tindex] = NULL;    /* Forget rest of command */
#if STYLE < 11
   else if (value [STATUS] == 2 &&
      value [ARG2] <= LVERB && value [ARG2] >= FVERB &&
      (value[ARG1] > LVERB || value[ARG1] < FVERB))
#else /* STYLE >= 11 */
   else if (value [STATUS] == 2 && value [ARG2] != SCENEWORD &&
      value [ARG2] <= LVERB && value [ARG2] >= FVERB &&
      (value[ARG1] > LVERB || value[ARG1] < FVERB))
#endif /* STYLE */
   {
      int temp_val;
      temp_val = value [ARG1];
      value [ARG1] = value [ARG2];
      value [ARG2] = temp_val;
      strncpy (temp_word, arg1_word, WORDSIZE);
      strncpy (arg1_word, arg2_word, WORDSIZE);
      strncpy (arg2_word, temp_word, WORDSIZE);
   }
   arg1_word [WORDSIZE - 1] = '\0';
   arg2_word [WORDSIZE - 1] = '\0';
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
               tindex++;
            }
            value [ARG3] = -1;
            if (refno <= BADWORD)
               return;
         }
      }
#endif /* ALL and EXCEPT */
      if (separator[tindex] != ';')
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
      value [ARG2] = BADSYNTAX;
   }
#endif
   return;
}
/*===========================================================*/
int query (int textref)
{
   char reply [10];
   char *rp;
   int which = -1;
   if (textref >= 0) say (0, textref, 0);
#if CONSOLE
   else scrchk (NULL);
#endif /* CONSOLE */
   while (which < 0)
   {
      query_flag = 1;
      getinput (reply, 10);
      if (loop > 1) return (0);
#if CONSOLE
      scrchk (NULL);
#endif /* CONSOLE */
#ifdef DWARVEN
      if (value [DWARVEN])  shift_down (reply, 10);
#endif /* DWARVEN */
      rp = reply;
      while (*rp == ' ') rp++;
      if (*rp == '\0' || *rp == '\n') which = 1;
      else if (*rp == 'y' || *rp == 'Y') which = 1;
      else if (*rp == 'n' || *rp == 'N') which = 0;
      else if (*rp == 'q' || *rp == 'Q') which = 0;
      if (which < -1)
      {
         PRINTF ("(OK, smartass... I'll assume you mean YES - so there!)\n \n");
         which = 1;
      }
      if (which >= 0)
         break;
      PRINTF ( "Eh? Do me a favour and answer yes or no!\nWhich will it be? ");
      which--;
   }
   return which;
}
/*===========================================================*/
void make_name (char *file_name, char *save_name)
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
#if MSDOS
      if (*cptr == '.')
         *cptr = '_';     /* DOMESTOS doesn't like double suffixes */
#endif /* MSDOS */
      cptr++;
   }

#ifdef MSDOS
   *(save_name + 8) = '\0';
#else /* !MSDOS */
   *(save_name + 16) = '\0';
#endif /* MSDOS */
   if (strcmp (save_name + strlen (save_name) - 4, ".adv") != 0)
      strcat (save_name, ".adv");
   return;
}
/*===========================================================*/
int check_version (FILE *infile)
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
int memstore (int key)
{
/* Key 0  check for existence of memory image
 * Key 1   true memory save
 * Key 2   true memory restore
 * Key 3   true memory save zap
 * Key 4   temporary memory save
 * Key 5   temporary memory restore
 */
#if CGI
   FILE *memory_file = NULL;
#endif /* CGI */
   static char *image_base = NULL;   /* True memory save area */
   static char *image_temp = NULL;   /* Temp save over restore area */
   char *image_ptr;
#if CGI
   char *fname = (char *)(key < 4 ? ".M.adv" : ".T.adv");
   int result = 1;
#endif /* CGI */

   if (key == 0)
   {
#if CGI
      if ((memory_file = fopen (fname, RMODE)) != NULL)
      {
         fclose (memory_file);
         result = 0;
      }
      return (result);
#else
      return (image_base ? 0 : 1);
#endif /* CGI */
   }

   image_ptr = key < 4 ? image_base : image_temp;
   if (key == 1 || key == 4)
   {
      if (image_ptr == NULL)
      {
         image_ptr = (char *) malloc (IMAGE_SIZE);
         if (image_ptr == NULL)
            return (1);
         if (key == 1)
            image_base = image_ptr;
         else
            image_temp = image_ptr;
      }
      memcpy (image_ptr, IMAGE, IMAGE_SIZE);
#if CGI
      if ((memory_file = fopen (fname, WMODE)) != NULL &&
         fwrite (image_base, 1, IMAGE_SIZE, memory_file) == IMAGE_SIZE)
            result = 0;
      if (memory_file)
         fclose (memory_file);
      return (result);
#else
      return (0);
#endif /* CGI */
   }
   else if (key == 3)
   {
#if CGI
      return (unlink(fname));
#else
      if (image_base)
      {
         free (image_base);
         image_base = NULL;
         return (0);
      }
      else
         return (1);
#endif
   }
   else
   {
#if CGI
      if ((image_ptr = (char *) malloc (IMAGE_SIZE)) != NULL &&
          (memory_file = fopen (fname, RMODE)) != NULL &&
          (fread (image_ptr, 1, IMAGE_SIZE, memory_file)) == IMAGE_SIZE)
             result = 0;
      if (memory_file)
         fclose (memory_file);
      if (result)
         return (1);
#else
      if (image_ptr  == NULL)
         return (1);
#endif /* CGI */
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

void input_store(int op, int *var)
{
   if (op == 'S')
   {
      *qtext = *var;
      *qstat = value [STATUS];
      *qarg1 = value [ARG1];
      strncpy (qargw1, arg1_word, WORDSIZE);
      if (*qstat == 2)
      {
         *qarg2 = value [ARG2];
         strncpy (qargw2, arg2_word, WORDSIZE);
      }
      else
      {
         *qarg2 = -1;
         *qargw2 = '\0';
      }

   }
   else
   {
      *var = *qtext;
      value [STATUS] = *qstat;
      value [ARG1] = *qarg1;
      value [ARG2] = *qarg2;
      strncpy (arg1_word, qargw1, WORDSIZE);
      strncpy (arg2_word, qargw2, WORDSIZE);
   }
}

/*===========================================================*/

int special (int key, int *var)
{
   static char save_name [168];
   char file_name [168];
   FILE *game_file;
   int val, val1, val2, val3;
#if STYLE >= 10
   int lval;
#endif /* STYLE >= 10 */
   char tval [12];
   static int saved_value = 0;
   static int tsiz = sizeof (time_t);
   int chksum;
   int chksav;
   char *cptr;
   int cnt;
   int lobj, lplace, lverb, lvar, ltext;
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
         if (value [STATUS] < 2)
            strcpy (file_name, "default");
         else
         if (val != -1)
         {
            if (*long_word &&
               strncmp (long_word, arg2_word, WORDSIZE - 1) == 0)
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
            strncpy (arg2_word, autoname, WORDSIZE - 1);
            make_name (autoname, save_name);
         }
         else
            make_name (file_name, save_name);
#else /* !ADVCONTEXT */
      case 999:
         if (key == 999)
            strncpy (file_name, autoname, WORDSIZE - 1);
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
               cnt = process_saved (0, file_name);
               if (cnt == 0)
               {
                  if (ADVLIB && mode != STARTGAME)
                  {
                     PRINTF("You have no saved games to restore.\n")
                     loop = 1; return (0);
                  }
                  PRINTF (
   "Can't see any saved games here, but you may know of some elsewhere.\n")
	       }
               else if (cnt == 1)
               {
                  value [ARG2] = strlen(file_name);
                  goto got_name;
               }
               else
               {
                  PRINTF ("You have the following saved games: ")
                  if (mode != STARTGAME && mode != HAVECMD)
                     process_saved (1, NULL);
               }
#endif /* STYLE >=11 */
               PRINTF ("\nName of saved game to restore: ");
	    }
            getinput (file_name, 16);
#if CONSOLE
            scrchk (NULL);
#endif /* CONSOLE */
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
         else if ((mode == HAVECMD || mode == NEEDCMD) &&
           (key == 999 || key == 997))
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
      case 998:             /* Writing persistent state image */
         if (key == 998)
         {
            make_name (autoname, save_name);
#ifdef ADVCONTEXT
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
         if ((ADVLIB || CGI) && key == 998)
         {
            CHKSUM(qwords, sizeof(qwords));
            CHKSUM(qvals, sizeof(qvals));
         }
#endif /* ADVCONTEXT */
         fwrite (&chksum, sizeof (int), 1, game_file);
         fwrite (tval, 1, sizeof(time_t), game_file);
         fwrite (IMAGE, 1, IMAGE_SIZE, game_file);
         if ((ADVLIB || CGI) && key == 998)
         {
            fwrite (qwords, sizeof (char), sizeof (qwords), game_file);
            fwrite (qvals, sizeof (char), sizeof (qvals), game_file);
            fwrite (&mainseed, sizeof (int), 1, game_file);
#if STYLE >=11
            fwrite (word_buf, sizeof (short), word_buf ? *word_buf - 1 : 0, game_file);
#endif
            fwrite (old_comline, sizeof (char), sizeof (old_comline),
               game_file);
            fwrite (long_word, sizeof (char), sizeof (long_word),
               game_file);
         }
         *var = (ferror (game_file)) ? 1 : 0;
         fclose (game_file);
#ifdef UNDO
         if (value [UNDO_STAT] >= 0 && diffs && diffs < dptr)
         {
            strcpy (save_name + strlen(save_name) - 3, "adh");
            if (((diffs && dptr > diffs + 4) ||
#ifdef ADVCONTEXT
               (ADVLIB && value [ADVCONTEXT] <= 1)) &&
#else
               ADVLIB) &&
#endif
                  (game_file = fopen (save_name, WMODE)))
            {
               int len = dptr - diffs;
               fwrite (&len, 1, sizeof (int), game_file);
               fwrite (diffs, 1, dptr - diffs, game_file);
               CHKSUM(diffs, len);
               fwrite (&chksum, 1, sizeof (chksum), game_file);
//fprintf(stderr, "\n=== CHK SAVED %d, mode %d\n", chksum, mode);
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
         if (mode == STARTGAME)
         {
            if (log_file)
               fprintf (log_file, "\nREPLY: restore %s\n", save_name);
            *qstat = 2;
            *qarg2 = value [ARG2];
            strncpy (qargw2, arg2_word, WORDSIZE);
         }
#endif
         if (check_version (game_file) != 0)
         {
            *var = 4;
            return (0);
         }
         val1 = 0;
         val2 = 0;
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
         else if (lobj < LOBJ) val2++;
         fread (&lplace, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LLOC: image %3d, expected %3d\n", lplace, LLOC);
#endif /* DEBUG */
         if (lplace > LLOC) val1++;
         else if (lplace < LLOC) val2++;
         fread (&lverb, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LVERB: image %3d, expected %3d\n", lverb, LVERB);
#endif /* DEBUG */
         if (lverb > LVERB) val1++;
         else if (lverb < LVERB) val2++;
         fread (&lvar, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LVAR: image %3d, expected %3d\n", lvar, LVAR);
#endif /* DEBUG */
         if (lvar > LVAR) val1++;
         else if (lvar < LVAR) val2++;
         fread (&ltext, sizeof (int), 1, game_file);
#ifdef DEBUG
         printf ("LTEXT: image %3d, expected %3d\n", ltext, LTEXT);
#endif /* DEBUG */
/*
 * For undo history, we don't mind about *more* texts, on the slightly
 * dodgy assumption that they are either added at the end (in line with
 * documentation on upward compatibility), or are in-line and no in-line
 * texts have morphing features. Hence val3 which is like val1 but ignores
 * increased number of texts.
 */
         val3 = val1;
         if (ltext > LTEXT) val1++;
         else if (ltext < LTEXT) val2++;

         if (val1)
         {
            *var = 2;
            fclose (game_file);
            return (0);
         }
         chksav = 0;
#if CONSOLE
         *var = memstore (4);
         if (*var != 0)
            return (0);
#endif /* CONSOLE */
         if (scratch == NULL)
         {
            scratch = (char *) malloc (IMAGE_SIZE);
            if (scratch == NULL)
               return (0);
         }
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
#if defined(ADVCONTEXT)
            if ((ADVLIB || CGI) && key == 999)
         {
            fread (qwords, sizeof (char), sizeof (qwords), game_file);
            fread (qvals, sizeof (char), sizeof (qvals), game_file);
            fread (&mainseed, sizeof (int), 1, game_file);
#if STYLE >= 11
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
#endif
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
            if ((ADVLIB || CGI) && key == 999)
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
               if (val3 == 0 && val2 == 0)   /* Not if structure changed! */
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
                     memcpy (image, IMAGE, IMAGE_SIZE);
                     fread (&len, 1, sizeof (int), game_file);
                     edptr = diffs + len;
                  }
                  else
                     free(d);
               }
               fclose (game_file);
	    }
         }
#endif /* UNDO */
         *var = (key == 999) ? 999 : 0;
         return (0);

      case 3:          /* Delete saved game */
         if (value [STATUS] < 2)
            strcpy (arg2_word, "default");
#ifdef ADVCONTEXT
         make_name (arg2_word, save_name);
#endif /* ADVCONTEXT */
         *var = unlink (save_name);
         if (*var)
           *var = 2;
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
#  if STYLE == 2
      PRINTF("Segmentation fault. VM halted.\n");
      outbuf(1);
      exit (1);
#endif
         *var = 0;    /* Style 1 "exit if demo games not allowed" */
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
         strncpy (arg2_word, arg1_word, WORDSIZE);
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

      case 17:         /* Save locations of objects */
         if (!objlocs)
            if ((objlocs = (int *) advalloc (LOCS_SIZE)) != NULL)
               memcpy(objlocs, location, LOCS_SIZE);
         return (0);

      case 18:         /* Get saved location of an object */
         *var = objlocs ? objlocs[*var] : 0;
         return (0);

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
#ifndef NO_READLINE
         if (mode == CONSOLE && !cps)
         {
            int offset = prompt_ptr - prompt_line;
            prompt_line = advrealloc(prompt_line, 2 * Maxlen + 1);
            prompt_ptr = prompt_line + offset;
         }
#endif /* NO_READLINE */
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
#ifndef NO_READLINE
         if (mode == CONSOLE && !cps)
         {
            int offset = prompt_ptr - prompt_line;
            prompt_line = advrealloc(prompt_line, 2 * Maxlen + 1);
            prompt_ptr = prompt_line + offset;
         }
#endif /* NO_READLINE */
         return (0);

      case 22:    /* Set screen depth */
         val = atoi (arg2_word);
         if (val == 0)
            val = *var;
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
         input_store ('S', var);
         return (0);

      case 24:    /* Restore player input */
         input_store ('R', var);
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
         *var = memstore (5);
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
         *var = memstore (0);
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
void create_db (char *dbfile)
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
FILE *try_db (char *dbdir, char *dbname, int trim)
{
   FILE *dbf;
   char *dptr;
   int len;

   len = strlen (dbname) + 1;
   if (dbdir && *dbdir)
      len += strlen (dbdir) + 1;
   if ((dptr = (char *) malloc (len)) == NULL)
   {
      fprintf (stderr, "Unable to allocate db name buffer!\n");
      exit (1);
   }
   if (dbdir && *dbdir)
   {
      strcpy (dptr, dbdir);
      if (trim)
      {
         char *cptr = strrchr (dptr, SEP);
         len = cptr ? cptr - dptr : 0;
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

#if WINDOWS
#  define CONFDIR "acode"
#else
#  define CONFDIR  ".acode"
#endif /* WINDOWS */

#ifdef MSDOS
#  define CONFFILE "acode.cfg"
#else
#  define CONFFILE "acode.conf"
#endif /* MSDOS */
/*====================================================================*/
#if !ADVLIB
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
#endif /* !ADVLIB */
/*====================================================================*/
void parse_line (char *line, char **tokens)
{
   int i;
   char *cptr = line;
   int quote;
   int lesc;

   *tokens = NULL;
   *(tokens + 1) = NULL;
   *(tokens + 2) = NULL;

   for (i = 0; i < 3; i++)
   {
      quote = 0;
      lesc = 0;
      while (*cptr == ' ' || *cptr == '\t') cptr++;
      if (*cptr == '\0' || *cptr == '\n') break;
      if (*cptr == '#' && lesc == 0 && quote == 0) break;
      if (*cptr == '\\' || *cptr == '\'' || *cptr == '"')
      {
         if (*cptr == '\\') lesc = 1;
         else quote = *cptr;
         cptr++;
      }
      *(tokens + i) = cptr;
      if (*cptr == '#' && (quote || lesc)) cptr++;
      lesc = 0;
      while (1)
      {
         if (*cptr == '\t') *cptr = ' ';
         if (*cptr == ' ' && lesc == 0 && quote == 0) break;
         if (*cptr == '\0' || *cptr == '\n') break;
         if (*cptr == '#' && lesc == 0 && quote == 0) break;
         if (*cptr == quote) break;
         cptr++;
      }
      *cptr++ = '\0';
   }
}
/*====================================================================*/
#if !ADVLIB
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
#if HTTP
void check_browser (char *name)
{
   struct stat stat_buf;
   char gpath[256];
   char *path;
   int sep = SEP;

#if WINDOWS
   sep = strchr(name, '/') ? '/' : SEP;
#endif
   if (strchr (name, sep))
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
#if WINDOWS
            sep = strchr(gpath, '/') ? '/' : SEP;
#endif
            *(gpath + (eptr - sptr)) = sep;
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
#endif /* HTTP */
/*====================================================================*/
/*
 * Process the configuration file.
 */
void read_conf ()
{
   char cbuf [256];
   char *tkn [3];
   char *cptr;
   FILE *cfile = NULL;
#if HTTP
   int use_default = 0;
   char *default_browser;

/* Different platfoms have different ways of invoking the default browser */

   if (OSX)
      default_browser = "/usr/bin/open";
   else if (UNIX)
      default_browser = "/usr/bin/xdg-open";
   else if (WINDOWS)
      default_browser =
        "c:\\windows\\system32\\rundll32.exe url.dll,FileProtocolHandler";
   else
      default_browser = "NONE";
#endif /* HTTP */

/* Look for the config file */

   strcpy (cbuf, prog);             /* Start with the executable's pathname */
   if ((cptr = strrchr (cbuf, SEP)) != NULL)  /* Find the last separator */
   {
      strcpy (cptr + 1, CONFFILE);  /* Append conffile name to it */
      cfile = fopen (cbuf, RMODE);  /* Try opening */
   }
   if (cfile == NULL)   /* No good? Well try hidden homedir place */
   {
      sprintf (cbuf, "%s%c%s%c%s", getenv (HOME), SEP, CONFDIR, SEP, CONFFILE);
      cfile = fopen (cbuf, RMODE);  /* Try that one */
   }
   if (cfile == NULL)   /* No good? Well try standard homedir place */
   {
      sprintf (cbuf, "%s%c%s%c%s", getenv (HOME), SEP, CONFDIR + 1, SEP, CONFFILE);
      cfile = fopen (cbuf, RMODE);  /* TRy that one */
   }
   if (cfile == NULL)               /* Still no good? */
      cfile = fopen (CONFFILE, RMODE); /* OK, try current dir */
   if (cfile == NULL)
      return;                       /* Give up */

/* Now process the conffile */

   while (fgets (cbuf, sizeof(cbuf) - 1, cfile))  /* Keep reading! */
   {
      char *aptr;
      *(cbuf + strlen(cbuf) - 1) = '\0';  /* Ensure string terminated! */
      parse_line (cbuf, tkn);             /* Parse the line */
      if (*tkn == NULL || *(tkn + 1) == NULL) /* Nothing for us there */
         continue;
      recase (*tkn, 'U');                 /* Uppercase the 1st token */

      if (strcmp (*tkn, "LOG") == 0 || strcmp (*tkn, "LOGFILE") == 0)
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
               sprintf (log_path, "%s.log", autoname + 1);
            else
            {
               struct stat stat_buf;
               char *rptr = strrchr (aptr, SEP);

               if (stat (aptr, &stat_buf) == 0)
               {
                  if (stat_buf.st_mode & S_IFREG)
                     strncpy (log_path, aptr, sizeof (log_path));
                  else if (stat_buf.st_mode & S_IFDIR)
                     sprintf (log_path, "%s%c%s.log", aptr, SEP, autoname + 1);
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
               handle_token ('b', JUSTIFY, justify ? "Y" : "N", 0, NULL);
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
#if HTTP
      else if (strcmp (*tkn, "BROWSER") == 0)  /* It's a browser specification */
      {
         if (mode == CONSOLE) continue;   /* Command line mode forced! */
         if (use_default) continue;       /* We'll be using the default one */
         if (conf [BROWEXE])      /* But we already have a valid browser! */
            continue;             /* So ignore the line */
         cptr = *(tkn + 1);       /* Look at second token */
         if (strcmp (cptr, "NONE") == 0)
         {
            mode = 0;
            continue;
         }
         if (strcmp(cptr, "DEFAULT") == 0)
         {
            use_default = 1;      /* Note that default browser is to be used */
            mode = HTTP;
            continue;
         }
         if ((UNIX && !OSX && (strcmp (*(tkn + 1),   "LINUX") == 0)) ||
             (UNIX && !OSX && (strcmp (*(tkn + 1),    "UNIX") == 0)) ||
             (OSX          && (strcmp (*(tkn + 1),   "OSX") == 0)) ||
             (WINDOWS        && (strcmp (*(tkn + 1), "WINDOWS") == 0)))
                cptr = *(tkn + 2);
         else
            continue;         /* Wrong platform! */
         if (*cptr == '$')
         {
            cptr = getenv(cptr + 1);
            if (!cptr)
               continue;
         }
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
         if (strcmp (cptr, "BORDER") == 0)
            handle_token ('b', BORDER, aptr, 0,  NULL);
         if (strcmp (cptr, "BUTTON") == 0)
            handle_token ('b', BUTTON, aptr, 0,  NULL);
         else if (strcmp (cptr, "COMPACT") == 0)
            handle_token ('c', COMPACT, aptr, 0, &compress);
         else if (strcmp (cptr, "HISTORY") == 0 ||
            strcmp (cptr, "COMMANDS") == 0)
               handle_token ('p', HISTORY, aptr, 2000, NULL);
         else if (strcmp (cptr, "WIDTH") == 0)
            handle_token ('p', PXWIDTH, aptr, -500, NULL);
      }
      else if (strcmp (*tkn, "TIMEOUT"))
      {
         if (! (tkn + 2) || ! *(tkn + 2))
            continue;
         cptr = recase (*(tkn + 1), 'U');
         aptr = *(tkn + 2);
         if (strncmp(cptr, "INV", 3) == 0)
            handle_token ('p', INITTIME, aptr, -20, NULL);
         else if (strncmp(cptr, "GRACE", 5) == 0)
            handle_token ('p', GRACETIME, aptr, -2, NULL);
         else if (strncmp(cptr, "KEEP", 4) == 0)
            handle_token ('p', PINGTIME, aptr, -1, NULL);
      }
#endif /* HTTP */
   }

/* Now set default values for anything still missing. */

#if HTTP
   if (use_default)
      store_conf (BROWEXE, default_browser);
   else if (conf [BROWEXE] == NULL)
   {
      char *cptr;
      if ((cptr = getenv ("BROWSER")) != NULL)
         store_conf (BROWEXE, cptr);
      else
         store_conf (BROWEXE, default_browser);
   }
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
   if (conf [BORDER] == NULL)
      store_conf (BORDER, "Y");
   if (conf [BUTTON] == NULL)
      store_conf (BUTTON, "Y");
   if (conf [HISTORY] == NULL || atoi (conf [HISTORY]) <= 0)
      store_conf (HISTORY, "200");
   if (conf [PXWIDTH] == NULL || atoi (conf [PXWIDTH]) <= 500)
      store_conf (PXWIDTH, "700");
#endif /* HTTP */
   if (conf [COMPACT] == NULL)
      handle_token ('c', COMPACT, compact, 0, &compress);
   if (conf [LOGFILE] == NULL)
      store_conf (LOGFILE, UMODE );
   if (conf [INITTIME] == NULL)
      store_conf (INITTIME, "20");
   if (conf [PINGTIME] == NULL)
      store_conf (PINGTIME, "2");
   if (conf [GRACETIME] == NULL)
      store_conf (GRACETIME, "2");
   if (conf [JUSTIFY] == NULL)
      handle_token ('b', JUSTIFY, justify ? "Y" : "N", 0, NULL);
   if (conf [MARGIN] == NULL)
      handle_token ('p', MARGIN, "1", 1, &Margin);
   if (conf [WIDTH] == NULL)
      handle_token ('p', WIDTH, "80", 80, &Linlen);
   if (conf [HEIGHT] == NULL)
      handle_token ('p', HEIGHT, "24", 24, &Screen);

#if !HTTP
   Maxlen = Linlen - 2 * Margin;
#endif /* !HTTP */

/* Finally make GRACETIME hold the sum of the PINGTIME and GRACETIME values. */

   timeout = atoi (conf [PINGTIME]) + atoi (conf [GRACETIME]);
}

/**********************************************************************/

char *cftext [] = {
   "##################### Config file for A-code games ####################"EOL,
   "#"EOL,
   "# Values as initally supplied are the ones the game will default to"EOL,
   "# in the absence of this file. Where equivalent command line options"EOL,
   "# exist, they override values specified by this config file."EOL,
   "#"EOL,
   "#######################################################################"EOL,
   "#"EOL,
#ifndef MSDOS
#if UNIX
   "# Browsers will be checked for in the order in which they are listed,"EOL,
   "# until one is located. Browsers can be specified by their full pathname,"EOL,
   "# or just by the executable name. In the latter case they will be"EOL,
   "# searched for using the search path as specified by the PATH variable."EOL,
   "# If a pathname contains spaces, the whole pathname must be quoted."EOL,
   "#"EOL,
   "   BROWSER $BROWSER              # Environment variables get evaluated"EOL,
   "#  BROWSER NONE                  # Force console mode."EOL,
#  if !OSX
   "#  BROWSER chromium-browser      # Google's chrome"EOL,
   "#  BROWSER firefox               # Mozilla's browser"EOL,
   "#  BROWSER konqueror             # KDE native browser"EOL,
#  endif /* !OSX */
#else /* !UNIX */
   "# A specific non-default browser may be specified by its full pathname"EOL,
   "# as in the blow IE example. If the pathname contains spaces, the whole"EOL,
   "# pathname must be quoted."EOL,
   "#"EOL,
   "#  BROWSER \"C:\\Program Files\\Internet Explorer\\iexplore.exe\""EOL,
#endif /* UNIX */
   "   BROWSER DEFAULT               # Use my default browser"EOL,
   "#"EOL,
   "# Browser display colours can be specified by names or by hex RGB codes;"EOL,
   "# if an RGB code is is used, it must be prefixed with \\#, e.g. \\#d0e0ff"EOL,
   "# or quoted, e.g. \"#d0e0ff\". And yes, our American friends can use COLOR,"EOL,
   "# if they like!"EOL,
   "#"EOL,
   "   COLOUR BACKGROUND  \\#d0e0ff   # Pale blue is the default background"EOL,
   "   COLOUR TEXT           black   # Default output text colour"EOL,
   "   COLOUR PROMPT           red   # Make prompt lines red"EOL,
   "   COLOUR SURROUND    \"#222222\"  # Dark grey default for surround"EOL,
   "#"EOL,
   "# Other things to do with browser display. Game commands can be always"EOL,
   "# submitted simply by pressing the <ENTER> (or <RETURN>) key, but setting"EOL,
   "# BUTTON to YES, will also show a submit button for those who must have it."EOL,
   "# The COMPACT setting allows the settings of None, Some, All and Default."EOL,
   "# suppressing respectively none, some or all blank lines, or accepting"EOL,
   "# the game's default. The HISTORY value shows how many commands and"EOL,
   "# responses the browser display should retain."EOL,
   "#"EOL,
   "   SHOW BORDER   Yes             # Show the command area border"EOL,
   "   SHOW BUTTON   Yes             # Show the submit button"EOL,
   "   SHOW COMPACT  Default         # Accept whatever the game defaults to"EOL,
   "   SHOW HISTORY  200             # Show up to 200 last commands and responses"EOL,
   "   SHOW WIDTH    700             # Set display area width to 700 pixels"EOL,
   "#"EOL,
   "# The log file section is applicable both to the browser and the console"EOL,
   "# modes. Log file can be OFF (or NO, or NONE) or WRITE or APPEND, the"EOL,
   "# last two being optionally followed by name or pathname; this may indicate"EOL,
   "# either a specific logfile or a directory in which the logfile will be"EOL,
   "# placed. The default logfile name is <game_name>.log and its default"EOL,
   "# location is ~/" CONFDIR "/<game_name>. "EOL,
   "#"EOL,
   "   LOGFILE NONE                  # No logfile"EOL,
   "#"EOL,
   "# If the game claims server or browser not responding, you can try adjusting"EOL,
   "# upwards the TIMEOUT limits. This really ought not be necessary, but"EOL,
   "# if the machine is busy, memory short and the js engine inefficient, you"EOL,
   "# never know..."EOL,
   "#"EOL,
   "   TIMEOUT INVOCATION  45        # Up to 45 seconds to fully invoke browser"EOL,
   "   TIMEOUT KEEPALIVE    2        # Keep-alive pings every 2 seconds"EOL,
   "   TIMEOUT GRACETIME    2        # Grace time for delayed pings"EOL,
   "#"EOL,
   "# Finally for the console mode only, some screen layout requirements."EOL,
#endif /* MSDOS */
   "# For the JUSTIFY attribute, the possible values are Yes, No and Default."EOL,
   "#"EOL,
   "   LAYOUT JUSTIFY DEFAULT        # Accept game's default"EOL,
   "   LAYOUT MARGIN        1        # Left and right text margin"EOL,
   "   LAYOUT WIDTH        80        # Console window width in columns"EOL,
   "   LAYOUT HEIGHT       24        # Console window height in lines"EOL,
   "#"EOL,
   "############################# End of file #############################"EOL,
   ""
};

void make_conf (void)
{
   char **cfptr = cftext;
   FILE *cfile = fopen (CONFFILE, "wb");
   if (!cfile) return;
   while (**cfptr) fputs (*cfptr++, cfile);
   fclose (cfile);
}
#endif /* !ADVLIB */
/*===========================================================*/
void gothere(void)
{
     struct stat stat_buf;
#if WINDOWS
      getcwd (odir, sizeof(odir));
#endif /* WINDOWS */
      chdir (getenv (HOME));
      if (chdir (CONFDIR) != 0 && chdir(CONFDIR + 1) != 0)
      {
#if !WINDOWS || defined(MSDOS)
         mkdir (CONFDIR, 0700);
#else
         mkdir (CONFDIR);
#endif /* MSDOS */ 
         chdir (CONFDIR);
      }   
/*
 * If necessary, create the config file.
 */
#if !ADVLIB
      if (stat (CONFFILE, &stat_buf) != 0) 
      {
         void make_conf (void);
         make_conf ();
      }
#endif /* ADVLIB */
/*
 * Go to where A-code games live!
 */
      if (chdir (truname + 1) != 0)
      {
#if !WINDOWS || defined(MSDOS)
         mkdir (truname + 1, 0700);
#else
         mkdir (truname + 1);
#endif /* MSDOS */
         chdir (truname + 1);
      }
}
/*===========================================================*/
int initialise (char *prog)
{
#ifdef MEMORY
   int text_bytes;
#endif
#ifdef READFILE
   char text [100];
#endif
   int index;

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
#if CONSOLE || QT
   {
      gothere();                 /* Go to where saved games live */
#if !QT
      read_conf(); /* Process the config file, if any */
#endif /* QT */

      if (!mode)                /* need the default therefrom */
      {
         putchar ('\n');
         for (index = 0; index < Margin; index++)
            putchar (' ');
      }
   }
#endif /* CONSOLE || QT*/
   if (*log_path)
   {
#if CONSOLE
      log_file = fopen (log_path, UMODE);
      if (log_file)
         fprintf (log_file, "%s: %u\n", GAME_ID, mainseed);
      else
#else
      if (!log_file && (log_file = fopen (log_path, conf[LOGFILE])) == NULL)
#endif /* ADVLIB || CGI*/
         printf ("(Sorry, unable to open log file...)\n");
   }

   virgin = *text;
#if PLAIN
   strcpy (title, (char *)text + 1);
#else
   titlen = 0;
   if (*text)
      titlen = -1;
#  ifdef KNOT
   else
     title [0] = text [1] ^ KNOT;
#  endif
   while (++titlen < 80)
      if ((title [titlen] = text [titlen] ^ text [titlen + 1]) == '\0')
         break;
#endif /* PLAIN */

   if (strcmp (title, GAME_ID) != 0)
   {
      printf ("Version stamp mismatch: '%s' != '%s'!\n", title, GAME_ID);
      return (1);
   }
   tp [0] = NULL;

#if STYLE < 12
   for (index = FOBJ; index <= LOBJ; index++)
      bitmod ('s', index, OBJ);

   for (index = FLOC; index <= LLOC; index++)
      bitmod ('s', index, LOC);
#endif

#if STYLE < 10 && defined(BRIEF)
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
void zap_dump (void)
{
   char name [64];
   int len =sprintf (name, "%s.adv", truname) - 1;
   unlink (name);
   *(name + len) = 'h';
   unlink (name);
   *(name + len) = 'l';
   unlink (name);
}
/*===========================================================*/
#if !ADVLIB
int parse_args (int argc, char **argv)
{
   char *kwrd;
   int val;
   char oc;
   char *opt;
   int new_game = 0;
#if STYLE < 10
   *compact = 'Y';
#else
   *compact = 'N';
#endif
   *(compact + 1) = '\0';
   *log_path = '\0';
   while (--argc)
   {
      argv++;
      if (**argv != ISKWRD && **argv != '-') /* Allow '-' on Windows too */
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
         continue;
      }
      kwrd = *argv + 1;          /* Point at the keyword specifier */
      if (*kwrd == '-' && *(kwrd + 1)) kwrd++;  /* Skip GNU-style prefix */
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
         handle_token ('b', JUSTIFY, justify ? "Y" : "N", 0, NULL);
         mode = 0;
         continue;
      }
      else if (*kwrd == 'b')
      {
         *compact = (*compact == 'Y' ? 'N' : 'Y');
         if (oc == '0' || oc == 'n') *compact = '0';
         else if (oc == '1' || oc == 'y') *compact = '1';
         else if (oc == '2' || oc == 'a') *compact = '2';
         handle_token ('c', COMPACT, compact, 0, &compress);
         continue;
      }
      if (*kwrd == 'p')
      {
         if (oc == '0' || oc == 'n') end_pause = 0;
         else if (oc == '1' || oc == 'y') end_pause = 1;
         else end_pause = 1 - end_pause;
         continue;
      }
#if !CGI
      else if (*kwrd == 'n')
         new_game = 1;
#endif /* !CGI */
      else if (*kwrd == 'v')
      {
         printf ("%s.\n", GAME_ID);
#if defined(STYLE)
         printf ("A-code style %d\n", STYLE);
#endif
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
#ifndef NO_SLOW
      else if (*kwrd == 'o')
      {
         mode = 0;
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
#endif /* NO_SLOW */
      else if (*kwrd == 'l')
      {
         if (oc) strncpy (log_path, opt, sizeof(log_path));
         else sprintf (log_path, "%s.log", autoname + 1);
         *(log_path + sizeof (log_path) - 1) = '\0';
         store_conf (LOGFILE, UMODE);
      }
#if HTTP
      else if (*kwrd == 'B')
      {
#if WINDOWS
         mode = HTTP;
         if (oc) check_browser (opt);
#else /* !WINDOWS */
         if (getenv ("DISPLAY"))
         {
            mode = HTTP;
            if (oc) check_browser (opt);
         }
         else
            fputs (
               "\n*** No display available ***\nDefaulting to the console mode.\n\n",
                  stderr);
#endif /* WINDOWS */
      }
#endif /* HTTP */
      else if (*kwrd == 'C')
      {
         if (CONSOLE) mode = CONSOLE;
         html_ok = 0;
      }
      else if (*kwrd == 'h')
      {
         printf ("\nUsage: %s [options]\n\nOptions:\n", prog);
         puts ("    -n                  force a new game (ignore an aborted one)");
         puts ("   [-r]<dumpfile>       restore game from dump");
         printf ("    -l[<logfile>]       log the game (by default in ~/" CONFDIR "/%s/)\n",
            autoname + 1);
#if HTTP
         puts ("    -C                  Force console (non-browser) display");
         puts ("    -B[browser]         Force display through browser (default)");
#endif /* HTTP */
#if CGI
         puts ("    -- <command>        Submits player commandfor execution");
#else /* !CGI */
         puts ("    -b[0|1|all]         invert or specify blank line suppression level");
         puts ("    -p[0|1]             invert or specify pausing before exit");
#ifdef USEDB
         puts ("    -d<dbsdir>          specify dbs directory");
#endif /* USEDB */
#if HTTP
#ifndef MSDOS
         puts ("\nThese options force console mode display:");
#endif /* MSDOS */
#endif /* HTTP */
         puts ("    -j[0|1]             invert or specify right-justification of text");
         puts ("    -s<W>.<H>[.<M>]     set screen size and margin");
#ifndef MSDOS
#if CONSOLE
         puts ("    -o[<baudrate>]      set output speed for authentic retro experience");
         puts ("                        (e.g. 110, 300, 600, 1200, 2400 or 4800)\n");
#endif /* CONSOLE */
#endif /* CGI */
#endif /* MSDOS */
         puts ("Two information only options:");
         puts ("    -v                  print version info and exit");
         puts ("    -h                  print this usage summary\n");
         exit (0);
      }
      else if (*kwrd == 's')
      {
         mode = 0;
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
         dbs_dir = opt;
#endif /* USEDB */
      else if (*kwrd == 'c')
         com_name = opt;
      else if (*kwrd == 'r')
      {
         if (*opt) dump_name = opt;
      }

#ifdef CGI
      else if (*kwrd == 'x' || *kwrd == 'y' || *kwrd == 'n' || *kwrd == '-')
      {
         mode = *kwrd == 'x' || *kwrd == 'n'  ? STARTGAME : HAVECMD;
         strncpy (extcom, opt, sizeof (extcom));
         if (*kwrd == 'y' || *kwrd == '-')
         {
            while (--argc)
            {
               argv++;
               if (sizeof(extcom) - strlen(extcom) < strlen(*argv) + 2)
                  continue;
               strcat (extcom, " ");
               strcat (extcom, *argv);
            }
            break;
            *kwrd = 'y';
         }
         else
           *kwrd = 'x';
         if (*log_path == '\0')
         {
            strcpy (log_path, autoname + 1);
            strcat (log_path, ".log");
            store_conf (LOGFILE, UMODE);
         }
      }
#endif /* CGI */
   }
#if CONSOLE
#if HTTP
#  if OSX
   if (!mode) mode = HTTP;
#else
   if (!mode) mode = getenv("DISPLAY") ? HTTP : CONSOLE;
#endif /* OSX */
#else /* !HTTP */
   mode = CONSOLE;
#endif /* HTTP */
#endif /* CONSOLE */
#if CGI
   if (mode == CGI) mode = STARTGAME;
#endif /* CGI */
   return (new_game);
}
#endif /* !ADVLIB */
/*===========================================================*/
#if ADVLIB
   char *advturn (char *cmd)
#else
   int main (int argc, char **argv)
#endif /* ADVLIB  */
{
   int new_game = 0;
#if ADVLIB
   char *cptr;
//   if (obuf) *obuf = '\0';
#ifdef PROMPTED
   bitmod ('s', ADVCONTEXT, PROMPTED);
#endif

   if (obuf) 
   {
     *obuf = 'n';
     *(obuf + 1) = '\0';
   }
#endif
   if (!text_buf)
     text_buf = advalloc (text_buf_len);
   *text_buf = '\0';
   lptr = text_buf;
   *lptr++ = '\n';
   text_len = 1;
   loop = 0;
#if STYLE >= 11
   if (!word_buf)
     word_buf = btinit (NULL);
#endif /* STYLE */

/* NB: CGINAME has '.' or '_' prefix, but it might be wrong. */

   strncpy (truname, CGINAME, sizeof (truname) - 1);
   *(truname + sizeof(truname) - 1) = '\0';
#  if WINDOWS || MSDOS
   *truname = '_';
#  else
   *truname = '.';
#  endif /* MSDOS || WINDOWS */
   autoname = truname;

#if ADVLIB
   strcpy (prog, ".");
//   prog = ".";
   mode = HAVECMD;
   strncpy (extcom, cmd, sizeof (extcom));
   if (*cmd == '_')
   {
      cptr = strchr (cmd + 1, '_');
      cptr++;
      if (strncmp(cptr, "TEXT", 4) == 0)
         { html_ok = 0; cptr += 4; }
      else if (strncmp(cptr, "HTML", 4) == 0)
         { html_ok = 1; cptr += 4; }
      if (*cptr && *cptr == '_') cptr++;
      if (strncmp (cmd, "_INFO_", 6) == 0)
      {
         PRINTF (GAME_NAME);
         outbyte ('|');
         PRINTF (GAME_ID);
         return (text_buf);
      }
      else if (strncmp (cmd, "_LIST_", 6) == 0)
      {
#if QT
         gothere();
#endif
         lptr = text_buf;
         *text_buf = '\0';
         process_saved (-1, "");
         return (text_buf);
      }
      else if (strncmp (cmd, "_RESUME_", 6) == 0)
      {
         mode = RESUMEGAME;
         *extcom = '\0';
      }
      else if (strncmp (cmd, "_LOAD_", 6) == 0)
      {
         mode = LOADGAME;
         value[STATUS] = -1;
         strcpy (comline, "restore ");
         strcat (comline, cptr);
         dump_name = comline + 8;
      }
      else if (strncmp (cmd, "_START_", 7) == 0)
      {
         mode = STARTGAME;
         new_game = 1;
      }
      else
         goto run_it;
   }
#else /* !ADVLIB */
   if (loop)
     goto run_it;
   strncpy(truname, CGINAME, sizeof(truname) - 1);   
//#if UNIX
//   if (readlink ("/proc/self/exe", prog, sizeof (prog) - 1) <= 0)
//#endif /* UNIX */
//     strncpy (prog, *argv, sizeof (prog - 1));
//   if (strrchr(prog, SEP))
//     strncpy(truname + 1, strrchr(prog, SEP) + 1, sizeof(truname) - 1);
//   else 
//     strncpy(truname + 1, prog, sizeof(truname) - 1);
//#if OSX
//   {
//      char *cptr = strrchr (truname + 1, '_');
//   if (cptr && strcmp (cptr, "_browser") == 0)
//      *cptr = '\0';
//   }
//#endif /* OSX */
#if HTTP
#  if WINDOWS
   mode = HTTP;
#  endif /* WINDOWS */
#endif /* HTTP */
   new_game = parse_args(argc, argv);
#endif /* ADVLIB */

   if (Linlen == 0) Linlen = 32767;
   if (Screen == 0) Screen = 32767;
   Maxlen = Linlen - 2 * Margin;

#ifndef NO_READLINE
   prompt_line = advalloc (2 * Maxlen + 1);
   prompt_ptr = prompt_line;
   *prompt_ptr = '\0';
#endif /* NO_READLINE */
#ifndef NO_SLOW
   if (mode != CONSOLE) cps = 0;
   if (cps)
#  if WINDOWS
      cps = 1000/cps;
#  else /* !WINDOWS */
      cps = 1000000/cps;
#  endif /* WINDOWS */
#endif /* NO_SLOW */
   if (mode != CONSOLE)
      Margin = 0;
   if (mainseed == 0)
      time ((time_t *) &mainseed);
   rseed = mainseed %= 32768L;
   irand (1);

   if (com_name && !new_game) new_game = 1;

   memset (IMAGE, '\0', IMAGE_SIZE);
   if (initialise (prog) != 0)
   {
      printf ("Sorry, unable to set up the world.\n");
      return (0);
   }
/*   if (!mode) mode = CONSOLE; */
   value [THERE] = value [HERE] = FLOC;

   value[0] = 0;
   if (mode != HAVECMD)
#if STYLE >= 10
      while (1)
      {
         loop = 0;
         INIT_PROC ();
         if (loop < 2) break;
      }
#else
      INIT_PROC();
#endif /* STYLE >= 10 */
   if ((CONSOLE || mode == RESUMEGAME) && !new_game)
   {
      if (mode == RESUMEGAME) mode = HAVECMD;
      if (! dump_name || ! *dump_name)
         special(999, &value [0]);
#ifdef PROMPTED
      bitmod ('s', ADVCONTEXT, PROMPTED);
#endif       
   }
   if ((value[0] == 999 && !com_name) || new_game < 0)
   {
      FILE *adl;
      char name [64];
      int c;
      lptr = text_buf;
      *lptr = '\0';
      PRINTF (RESTORING);
      sprintf (name, "%s.adl", truname);
      if ((adl = fopen (name, RMODE)) != NULL)
      {
         while ((c = fgetc(adl)) != EOF)
            outbyte (c & 0377);
         fclose (adl);
      }
#if ADVLIB
      outbuf(0);
      return (obuf);
#else
      goto run_it;
#endif /* ADVLIB */
   }
   
#if JS
   if (new_game)
     value[STATUS] = -2;     /* Signal new game with no restore choice */
#endif /* JS */
#ifdef ADVCONTEXT
   if (mode == STARTGAME && dump_name && *dump_name)
   {
      autoname = dump_name;
      special (997, &value [0]);
      autoname = truname;
   }
   else if (mode == HAVECMD) /* Not in queries! */
      special (999, &value [0]);
   else
#endif /* ADVCONTEXT */
   {
      if (dump_name && *dump_name)
      {
/*         INIT_PROC (); */
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
/*         if (setjmp (loop_back) == 0) */
/*            INIT_PROC ();             */
      }
   }

run_it:
   while (1)
   {
#if ADVLIB || CGI
      {
         if (mode == LOADGAME) mode = STARTGAME;
         if (loop > 1)
         {
#if ADVLIB
           return (obuf);
#else
           return (0);
#endif
         }
      }
#endif /* ADVLIB || CGI */
#if CGI
      if (loop > 1)
#ifdef ADVCONTEXT
        exit (value [ADVCONTEXT]);
#endif /*ADVCONTEXT */
#endif /* CGI */
      loop = 0;
      if (quitting)
      {
         if (end_pause)
         {
            PRINTF ("(To exit, press ENTER)");
            getinput (comline, COMLEN);
            putchar('\n');
         }
         else
         {
            if (text_len > 0)
               outbuf (1);
            putchar('\n');
         }
         close_files ();
         zap_dump ();
#if WINDOWS
         chdir(odir);
#endif /* WINDOWS */
#if ADVLIB
         *obuf = 'f';
         quitting = 0;
         return (obuf);
#else /* !ADVLIB && !CGI */
         free (text_buf);
         free (obuf);
         if (scratch) free (scratch);
         return (255);
#endif /* ADVLIB || CGI */
      }
      rseed = mainseed;
      irand (1);
      mainseed = rseed;
      rseed = mainseed ^ 255;
      REPEAT_PROC ();
   }
}
/*===========================================================*/
int have (int l1,int l2,int l3)
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
int isat (int l1,int l2,int l3, int l4)
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
int ishere (int l1,int l2,int l3)
{
   return (isat (l1, l2, l3, value [HERE]));
}
/*===========================================================*/
int isnear (int l1,int l2,int l3)
{
   if (have (l1,l2,l3)) return (1);
   if (ishere (l1,l2,l3)) return (1);
   return (0);
}
/*===========================================================*/
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
   if (a2 != -1)
      loop = 1;
   return;
}
/*===========================================================*/
void apport (int l1,int l2)
{
#if defined (JUGGLED) && defined (STATUS)
   if (location [l1] == INHAND || l2 == INHAND)
      bitmod ('s', STATUS, JUGGLED);
#endif
   location [l1] =  (l2 <= LLOC || l2 == INHAND) ? l2 : value [l2];
   return;
}
/*===========================================================*/
void set (char t1, int v1, char t2, int v2, int *lv, short *lb)
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
void lda (int l1, int l2)
{
   value [l1] = l2;
   *bitword (l1) = -1;
   textadr [l1] = textadr [l2];
   return;
}
/*===========================================================*/
void eval (int l1, int l2)
{
   value [l1] = value [value [l2]];
   return;
}
/*===========================================================*/
void deposit (int l1, int l2)
{
    value [value [l1]] = (l2 > LVAR || l2 < FVAR) ? l2 :
       value [l2];
    return;
}
/*===========================================================*/
void locate (int l1, int l2)
{
   value [l1] = location [(l2 < FVAR || l2 > LVAR) ? l2 :
      value [l2]];
   *bitword (l1) = -1;
   return;
}
/*===========================================================*/
int evar (int l1)
{
   if (*bitword (l1) == -1)
      return value [l1];
   else
      return l1;
}
/*===========================================================*/
int levar (int l1, int *l2, short *l3)
{
   if (*(l3 + VARSIZE * l1) == -1)
      return (*(l2 + l1));
   else
      return l1;
}
/*===========================================================*/
void finita (void)
{
#if !defined(MEMORY) && !defined(PRELOADED)
   fclose (text_file);
#endif /* MEMORY */
#ifdef LOC_STATS
   fprintf (stderr, "(Locates: %d (+%d), faults %d (+%d))\n",
      locate_demands, locate_demands - old_demands,
      locate_faults, locate_faults - old_faults);
   fprintf (stderr, "(Locate ratio %d%%)\n",
      (((1000 * locate_faults) / locate_demands) + 5) / 10);
#endif /* LOC_STATS */
   quitting = 1;
   loop = 1;
}
/*===========================================================*/
short *bitword (int a1)
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
void bitmod (char a1, int a2, int a3)
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
void lbitmod (int a0, char a1, int a2, int a3, int *a4, short *a5)
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
int bitest (int a1, int a2)
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
   return (*bitadr & 1 << a2) ? 1 : 0;
}
/*===========================================================*/
int lbitest (int a1, int a2, int *a3, short *a4)
{
   short *bitadr;
   if (*(a4 + VARSIZE * a1) == -1)
   {
      a1 = *(a3 + a1);
      if (a1 <= LVERB)
      {
         if (a2 == 0)
            return (a1 <= LOBJ && a1 >= FOBJ) ? 1 : 0;
         else if (a2 == 1)
            return (a1 <= LLOC && a1 >= FLOC) ? 1 : 0;
         else if (a2 == 2)
            return (a1 >= FVERB) ? 1 : 0;
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
   return (*bitadr & 1 << a2) ? 1 : 0;
}
/*===========================================================*/
void flush_command (void)
{
   value_all = 0;
   tp [tindex] = NULL;
   return;
}
/*===========================================================*/
void shift_up (char *aptr)
{
   if ((*aptr >= 'a' && *aptr < 'z') || (*aptr >= 'A' && *aptr < 'Z'))
      (*aptr)++;
   else if (*aptr == 'z' || *aptr == 'Z')
      *aptr -= 25;
}
/*===========================================================*/
void shift_down (char *aptr, int maxlen)
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
void tie (int text, int holder)
{
   text_info [2 * (text - FTEXT)] = TIED_TEXT;
   value [text] = holder;
}
/*===========================================================*/
void svar (int type, int *var)
{
   time_t now;
   struct tm *tp;
   if (type == 8)     /* Read sense switch given by *var */
     *var = 1;    /* We always pretend it is set */
   else
   {
     now = time (NULL);
     tp = localtime (&now);
     switch (type)
     {
        case 4:   /* Get time (minutes) */
           *var = tp -> tm_min;
           break;
        case 5:   /* Get time (hours) */
           *var = tp -> tm_hour;
           break;
        case 6:   /* Get year day */
           *var = tp -> tm_yday;
           break;
        case 7:   /* Get year */
           *var = tp -> tm_year;
           break;
        default:
           PRINTF2 ("GLITCH! Bad svar code: %d\n", type);
     }
   }
   return;
}
/*===========================================================*/
int getloc(int refno)
{
  return (refno <= LOBJ ? location [refno] : 0);
}
/*===========================================================*/
int randsel(int cnt, ...)
{
  va_list ap;
  int choice;

  cnt = irand(cnt) + 1;
  va_start (ap, cnt);
  while (cnt--)
    choice = va_arg(ap, int);
  va_end (ap);
  return (choice);
}
/*===========================================================*/
void glue_text (void)
{
/* Convert last message output into a fragment */
   while (*(lptr - 1) == '\n') lptr--;
}
/*===========================================================*/
void zap_text (void)
{
/* Zap all of the accumulated text */
   lptr = text_buf;
   *lptr = '\0';
   text_len = 0;
}
/*===========================================================*/
void verbatim (int arg)
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
int test (char *type)
{
   if (strcmp(type, "turn") == 0)
      return (CGI || ADVLIB ? 1 : 0);
   if (strcmp(type, "cgi") == 0)
      return (CGI ? 1 : 0);
   if (strcmp(type, "doall") == 0)
      return (value_all ? 1 : 0);
   if (strcmp(type, "html") == 0)
      return (html_ok ? 1 : 0);
   if (strcmp(type, "needcmd") == 0)
      return (tp[tindex] ? 0 : 1);
   return (0);
}
/*===========================================================*/
void show_bits (int refno, int size)
{
   int i;
   fprintf (stderr, " - ");
   for (i = 0; i < 8 * size * sizeof(short); i++)
      fprintf (stderr, "%c", bitest (refno, i) ? '1' : '0');
}
/*===========================================================*/
void show_data (void)
{
   int i;
   FILE *rrefs;
   char buf [80];

   sprintf (buf, "%s.rrefs", truname);
   rrefs = fopen (buf + 1, RMODE);
   for (i = FOBJ; i < LTEXT; i++)
   {
      if (rrefs)
      {
         fgets (buf, sizeof(buf) - 1, rrefs);
         *(buf + strlen(buf) - 1) = '\0';
         fprintf (stderr, "%25s %4d", buf + 8, i);
      }
      else
      {
         fprintf (stderr, "%4d", i);
      }

      fprintf (stderr, "%5d", *(value + i));
      if (i >= FOBJ && i <= LOBJ)
      {
         show_bits (i, OBJSIZE);
         fprintf (stderr, " @ %4d", *(location + i));
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
#ifdef UNDO

void inv_check (void)
{
   int i;
   int diff = 0;
   for (i = 0; i <= LOBJ; i++)
   {
      if ((inhand[i] == INHAND && location[i] != INHAND) ||
          (inhand[i] != INHAND && location[i] == INHAND))
      {
         diff = 1;
         break;
      }
   }
   bitmod (diff ? 's' : 'c', UNDO_STAT, UNDO_INV);
}
/*===========================================================*/
/* The checkdo() function extracts from player's command the bunary
 * value of the number of mives to undo (or redo).
 */
int checkdo (void)
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
void  undo (void)
{
   int cnt;
   int acnt;
   if ((cnt = checkdo ()) < 0)  /* Get number of commands to undo */
      return;
   if (diffsz == 0 || edptr <= diffs + 4)
      acnt = 0;
   else
   {

/* In the CGI mode, two UNDO frames get created -- one before getting
 * a command and one after.
 */

      for (acnt = 0; acnt < cnt; acnt++)
      {
#if CGI
         int n;
         for (n = 0; n < 2; n++)  /* Process 2 frames! */
         {
#endif
            if (edptr <= diffs + 4)
               break;
            edptr -= 4;
            while (edptr > diffs)
            {
               int adr;
               edptr -= 4;
               adr = 256 * (*edptr) + *(edptr + 1);
               if (!adr) break;
               *(IMAGE + adr) = *(edptr + 2);
            }
            edptr += 4;
#if CGI
         }
         if (edptr <= diffs + 4)
         {
            acnt++;
            break;
         }
#endif
      }
      inv_check ();
   }
   value [UNDO_STAT] = acnt;

#ifdef ALL
   if (value [ARG2] == ALL)
      cnt = acnt;
#endif
   bitmod (cnt > acnt ? 's' : 'c', UNDO_STAT, UNDO_TRIM);
#ifdef ADVCONTEXT
   value[ADVCONTEXT] = 0;
#endif
   memcpy (image, IMAGE, IMAGE_SIZE);
   return;
}
/*===========================================================*/
void redo (void)
{
   int acnt;
   int cnt;

   if ((cnt = checkdo ()) < 0)
      return;

   if (edptr == dptr)
      acnt = 0;
   else
   {

/* In the CGI mode, two UNDO frames get created -- one before getting
 * a command and one after.
 */

      for (acnt = 0; acnt < cnt; acnt++)
      {
#if CGI
         int n;
         for (n = 0; n < 2; n++)  /* Process 2 frames! */
         {
#endif
            if (edptr > dptr)
               edptr = dptr;
            while (1)
            {
               int adr = 256 * (*edptr) + *(edptr + 1);
               edptr += 4;
               if (!adr) break;
               *(IMAGE + adr) = *(edptr - 1);
            }
            if (edptr == dptr) break;
#if CGI
         }
         if (edptr == dptr) break;
#endif
      }
      inv_check ();
      memcpy (image, IMAGE, IMAGE_SIZE);
   }
   if (cnt > acnt) acnt++;
   value [UNDO_STAT] = acnt;
#ifdef ALL
   if (value [ARG2] == ALL)
      cnt = acnt;
#endif
#ifdef ADVCONTEXT
   value[ADVCONTEXT] = 0;
#endif
   bitmod (cnt > acnt ? 's' : 'c', UNDO_STAT, UNDO_TRIM);
}
#endif /* UNDO */
/*===========================================================*/
void pcall (int procno)
{
   fprintf (stderr,
      "\n*GLITCH* Called proc offest %d, but game's offset range is 0 to %d!\n",
         procno, LPROC);
#if STYLE >= 11
   if (procno == BADWORD || procno == AMBIGWORD ||
       procno == AMBIGTYPO || procno == SCENEWORD || procno == BADSYNTAX)
#else
#if STYLE > 10
   if (procno == BADWORD || procno == AMBIGWORD)
#  else
   if (procno == BADWORD)
#  endif
#endif
   {
      fprintf (stderr, "   Probable cause: ");
      if (procno == BADWORD)   fprintf (stderr, "bad word");
#if STYLE > 10
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

int typed (char *string)
{
   return (strcmp(orig1, string) == 0 || strcmp(orig2, string) == 0);
}
#endif /* STYLE */

/*************************************************************/
/* All the remaining code is strictly for direct browser display,
 * i.e. not even for remote cgi-bin operation.
 */
#if HTTP

/*====================================================================*/
/* This is the initial page to send to the browser, including all
 * the necessary JavaScrip code.
 */
char *page[] =
{
   "<html><head><title>",
   "=G",
   "</title>",
   "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n",
   "<script type=\"text/javascript\"><!--\n",
   "=L",
#ifdef DEBUG
   "var debug = 1;\n",
#endif /* DEBUG */
   "var state = 'S';\n",
   "=M",
   "var interval = 0;\n",
   "=C",
   "var rType = 0;\n",
   "var turn = 0;\n",
   "var wake = 0;\n",
   "var hpos = 0\n",
   "var hsiz = 0;\n",
   "var hcom = [];\n",
   "var conbot = 0;\n",
   "var conreg = 0;\n",
   "var command = 0;\n",
   "var started = 0;\n",
   "function scrollit()\n",
   "   { window.scrollTo(0, conbot.offsetTop); }\n",
   "function writeit(text)\n",
   "   { var newDiv = document.createElement('div'); newDiv.id = 'chunk';\n",
   "     newDiv.setAttribute('name', 'chunk'); newDiv.innerHTML = text;\n",
   "     document.getElementById('conreg').insertBefore(newDiv, null); scrollit(); }\n",
   "   \n",
   "function zapit()\n",
   "   { document.getElementById('comform').innerHTML = ''; }\n",
#ifdef DEBUG
   "function report(text)\n",
   "   { writeit('<span class=\"debug\">' + text + '</span><br>'); }\n",
#endif /* DEBUG */
   "function shutit(text)\n",
   "{\n",
   "   window.clearInterval(interval);\n",
   "   timeLimit = 0;\n",
   "   state = 'X';\n",
   "   zapit();\n",
   "   writeit(text + '<div align=center class=\"red\">' +\n",
   "      '<input class=\"custom-button\" ' +\n",
   "      'onclick=\"javascript:self.close()\" id=\"shutit\"' +\n",
   "      'type=\"button\" value=\"Close this tab/window\" /><br>' +\n",
   "      '<span class=\"red\">This button will only work if your ' +\n",
   "      ' browser<br> is not being paranoid about security.</span></p>' +\n",
   "      '</div>');\n",
   "   document.getElementById('shutit').focus();\n",
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
   "            shutit('<h2>Sorry, your browser is not compatible with this game!</h2><br>');\n",
   "         }\n      }\n   }\n",
   "   http.onreadystatechange = function(e)\n",
   "      {\n",
   "         if (http.readyState == 4)\n",
   "         {\n",
   "            if (http.status == 200)\n",
   "               showit (http.responseText);\n",
   "            else\n",
   "               shutit('<h2>Server process not responding!</h2><br>');\n",
   "         }\n",
   "      };\n",
   "   http.open('GET', '/?status=' + escape(text), true);\n",
   "   http.send(null);\n",
   "}\n",
   "function init()\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Entering init');\n",
#endif /* DEBUG */
   "   conbot = document.getElementById('conbot');\n",
   "   conreg = document.getElementById('conreg');\n",
   "   command = document.getElementById('command');\n",
   "   command.onkeydown = filter;\n",
   "   if (document.getElementsByName('chunk').length > 0)\n",
   "   {\n",
   "      wake = 1;\n",
   "      state = 'R';\n",
   "   }\n",
   "   sendit('0x' + state);\n",
   "   setAlarm(timeLimit);\n",
   "   state = 'R';\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting init');\n",
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
   "   return;\n",
   "}\n",
  "function checkServer()\n",
   "{\n",
   "   wake++;\n",
   "   sendit('0x' + wake);\n",
   "   setAlarm(timeLimit);\n",
   "}\n",
    "function setAlarm(limit)\n",
   "{\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Entering setAlarm, state is ' + state + ', limit is ' + timeLimit);\n",
#endif /* DEBUG */
   "   if (interval)\n",
   "      window.clearInterval(interval);\n",
   "   if (limit)\n",
   "      interval = window.setInterval(checkServer, limit, 2);\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting setAlarm, interval is now ' + interval);\n",
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
   "      if (state == 'R')\n",
   "      {\n",
   "         writeit(\n",
   "            '<p><h2>Server process not responding!<br>' +\n",
   "            'Please close this window and restart.</h2></p>');\n",
   "      }\n",
   "      window.clearInterval(interval);\n",
   "      interval = 0;\n",
   "      state = 'X';\n",
   "   }\n",
   "   else\n",
   "   {\n",
   "      var type = text.substr(0, 1);\n",
   "      if (type != 'k')\n",
   "      {\n",
   "         rType = type;\n",
   "         writeit(text.substr(1));\n",
   "         if (type == 'f')\n",
   "            shutit('');\n",
   "         else if (started)\n",
   "            document.getElementById('command').focus();\n",
   "      }\n",
   "      setAlarm(timeLimit);\n",
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
   "   var com = command.value;\n",
   "   var pars;\n",
   "   if (interval)\n",
   "   {\n",
   "      window.clearInterval(interval);\n",
   "      interval = 0;\n",
   "   }\n",
   "   turn++;\n",
   "   pars = turn + 'x' + com;\n",
   "   hcom[0] = com;\n",
   "   var prompt = document.getElementById('prompt');\n",
   "   if (prompt)\n",
   "   {\n",
   "      var el = document.createElement('span');\n",
   "      var tx = (rType == 'q' || rType == 'm' ||\n",
   "                rType == 'r' || rType == 's') ?\n",
   "                   prompt.innerHTML : '?';\n",
   "      if (rType == 'c') tx = '<br>?';\n",
   "      el.className = 'query';\n",
   "      el.innerHTML = tx + ' ' + com +\n",
   "          (rType == 't' ? '<br>&nbsp<br>' : '');\n",
   "      prompt.parentNode.replaceChild(el, prompt);\n",
   "      el = null;\n",
   "   }\n",
   "   if (com)\n",
   "   {\n",
   "      var i;\n",
   "      for (i = 1; i < hcom.length; i += 2)\n",
   "      {\n",
   "         if (hcom[i])\n",
   "         {\n",
   "            hcom[i - 1] = hcom[i];\n",
   "            hcom[i] = '';\n",
   "         }\n",
   "      }\n",
   "      if (hcom[0] == hcom[2])\n",
   "         hcom[0] = '';\n",
   "      else\n",
   "      {\n",
   "         hcom.unshift('');\n",
   "         hcom.unshift('');\n",
   "         hsiz++;\n",
   "      }\n",
   "      command.value = '';\n",
   "   }\n",
   "   hpos = 0;\n",
   "   sendit(pars);\n",
   "   var kids = document.getElementsByName('chunk');\n",
   "   if (kids.length > moveLimit)\n",
   "   {\n",
   "      kids[0].parentNode.removeChild(kids[0]);\n",
   "      hcom.pop();\n",
   "      hcom.pop();\n",
   "      hsiz--;\n",
   "   }\n",
#ifdef DEBUG
   "   if (debug > 0)\n",
   "      report('Exiting doit');\n",
#endif /* DEBUG */
   "}\n",
   "function startit()\n",
   "{\n",
   "   document.getElementById('startit').style.display = 'none';\n",
   "   document.getElementById('gameview').style.display = 'block';\n",
   "   document.getElementById('command').focus();\n",
   "   started = 1;\n",
   "}\n",
   "--></script><style type=\"text/css\">\n*.query{color:",
   "=Q",
   "}\n*.debug{color:",
   "=D",
   "}\n*.red{color:red}\ntd.view{width:",
   "=W",
   "px}\n*,input{color:",
   "=F",
   "}\ninput{max-width:",
   "=T",
   "px;}\ntd,input{background-color:",
   "=B",
   "}\ninput.command{border: ",
   "=X",
   "px inset #666666}\n",
   "</style></head><body bgcolor=\"",
   "=S",
   "\" text=\"",
   "=F",
   "\" onLoad=\"javascript:init();\">\n",
   "<table style='margin:auto' border=1 height=100% cellpadding=16>",
   "<tr><td class=view style=vertical-align:top>\n",
   "<form autocomplete=\"off\" method=\"get\" ",
   "action=\"javascript:doit(1)\" id=\"inform\">",
   "<div id=startit>\n",
   "&nbsp;<br>\n",
   "<h2 align=center>Welcome to the A-code browser interface</h2>",
   "<h3 align=center>",
   "=V",
   "</h3>\n",
   "<p>This interface can be used entirely from the keyboard, as long \n",
   "as the command input field remains in focus. Commands can be submitted\n",
   "by pressing the RETURN key and arrow keys can be used to recall and\n",
   "edit previous commands.</p>\n",
   "<p>The appearance of the game can be customized by modifying the A-code\n",
   "configuration file <b>acode.conf</b> &ndash; this can be found in\n",
   "the A-code directory, called either <b>.acode</b> or <b>acode</b>,\n",
   "within your home directory.\n",
   "There you can also find individual game directories called by the\n",
   "game name, which is where saved games and log files are stored by\n",
   "default.</p>\n",
   "<p>The configuration file has ample comments to guide you, should\n",
   "you decide to make any changes. If by any chance this interface\n",
   "does not work for you, you can change the browser specification in\n",
   "that file to NONE and then run the game in the console mode from a\n",
   "terminal window.\n",
   "</p><p align=center>&nbsp;<br>\n",
   "<input value='Play ",
   "=G",
   "' type=button class=query ",
   "onclick=javascript:startit() style=font-size:x-large></p>\n",
   "<p>&nbsp;<br>(In case you are wondering why it is not possible to skip\n",
   "this intro page &ndash; it's because it is in fact a cunning workaround\n",
   "for a browser mis-feature, no names named. By clicking on the 'Play'\n",
   "button you will ensure that the browser behaves itself and gives\n",
   "keyboard focus to where the interface requests it to be.)</p>\n",
   "</div>\n",
   "<div id=gameview style=display:none>\n",
   "<div id=\"conreg\">\n",
   "</div>\n<span id=\"comform\">",
   "<span class=\"query\">?&nbsp;</span><input class=\"command\" ",
   "id=\"command\" size=\"",
   "=I",
   "\" ",
   "maxlength=\"160\" type=\"text\">\n",
   "=R",
   "</span><br></span></div><div id=\"conbot\">",
   "</div></form></td></tr></table></body></html>\n",
   "=="
};

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
   char tlimit [32];
#
#ifdef DEBUG
   printf ("=== Entering %s\n", "send_page");
#endif /* DEBUG */
   sprintf (mlimit, "var moveLimit = %d;\n", atoi (conf [HISTORY]));
   sprintf (compact, "var compact = %d;\n", compress ? 1 : 0);
   sprintf (tdlen, "%d", atoi (conf [PXWIDTH]) - 20);
   sprintf (inplen, "%d",
      atoi (conf [PXWIDTH]) / 10 - (*conf[BUTTON] == 'Y' ? 6 : 0));
#ifdef DEBUG
   sprintf (tlimit, "var timeLimit = %d;\n", 10000 * atoi (conf [PINGTIME]));
#else
   sprintf (tlimit, "var timeLimit = %d;\n", 1000 * atoi (conf [PINGTIME]));
#endif /* DEBUG */
   optr = obuf + http_offset;
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
            case 'I': cptr = inplen;            break; /* Input field length */
            case 'M': cptr = mlimit;            break; /* History moves limit */
            case 'C': cptr = compact;           break; /* Compactification */
            case 'X': cptr = *(conf[BORDER]) == 'Y' ?  /* Want a border? */
                                     "1" : "0"; break;
            case 'R': if (*(conf [BUTTON]) == 'Y')     /* Want submit button? */
                         cptr = SUBMIT_BUTTON;  break;
            case 'L': cptr = tlimit;            break; /* Timeout limit */
            case 'V': cptr = GAME_ID;           break;
            default:
               fprintf (stderr, "Bad substitution tag in page.html: ='%c'!", key);
               exit (1);
         }
      }
      if (cptr) oputs (cptr);
   }
   *optr = '\0';
   browser_write (obuf);
#ifdef DEBUG
   printf ("=== Exiting %s\n", "send_page");
#endif /* DEBUG */
}
/*====================================================================*/
/* Generates the HTTP header for browser operation, based on the payload
 * length given as a string in the <number> buffer.
 */
char *make_header (char *buf)
{
  char length[10];
  char *rptr;
  sprintf (length, "%d", strlen (buf + http_offset));
  rptr =  buf + 7 - strlen (length);
  sprintf (rptr, "%s%s\n", HTTP_HEADER, length);
  *(buf + http_offset - 1) = '\n';
  return (rptr);
}
/*====================================================================*/
void send_null()
{
   char kbuf [sizeof(HTTP_HEADER) + 20];
   strcpy (kbuf + http_offset, "k");
   browser_write (kbuf);
}
/*====================================================================*/
#endif /* HTTP */

/* adv1.h: Adv660 - copyright Mike Arnautov 2022. Licensed under
 * GPL (version 3 or later) or the Modified BSD Licence, whichever
 * is asserted by the supplied LICENCE file.*/

#ifndef ADV1_H
#define ADV1_H
#define GAME_NAME "Adv660"
#define GAME_VERSION "10.12"
#define GAME_ID "Adv660 version 10.12, 10 Nov 2021"
#define STYLE 10
#ifndef ADV01
#define GAME_DATE "10 Nov 2021"
#define ACDC_VERSION "12.44, 10 Nov 2021"
#if defined(MSDOS) || defined(_WIN32)
#  define CGINAME "_adv660"
#else
#  define CGINAME ".adv660"
#endif
#define DBNAME "adv660.dat"
#define OBJSIZE 2
#define LOCSIZE 1
#define VARSIZE 1
#define FOBJ 3
#define LOBJ 135
#define FLOC 136
#define LLOC 486
#define FVERB 487
#define LVERB 670
#define FVAR 671
#define LVAR 776
#define FTEXT 776
#define LTEXT 1715
#define NOISE 19
#define KNOT 52
#define COMMA -667
#define SEMICOLON -669
#define INHAND 486
#define HERE 673
#define THERE 674
#define STATUS 699
#define ARG1 671
#define ARG2 672
#define ARG3 775
#define SCHIZOID 6
#define JUGGLED 6
#define MOVED 0
#define PLSCLARIFY 5
#define SEEN 4
#define AGAIN 488
#define ALL 3
#define DETAIL 4
#define FSPECIAL 487
#define LSPECIAL 494
#define FACT 495
#define LACT 512
#define SAY 493
#define NOWORD -1
#define BADWORD -2
#define AMBIGWORD -3
#define NO_MATCH 98
#define NO_AMATCH 99
#define OBJ 0
#define LOC 1
#define VERB 2
#define FULL 3
#define BEENHERE 4
#define TERSE 2
#define BRIEF 1
#define PLAIN 0
#define DBSTATUS 0
#define LPROC 671
#define KEY(X) (value[671]==X || value[672]==X)
#define INIT_PROC p1
#define REPEAT_PROC p2
#endif
#define SW_START     037
#define SW_BREAK     036
#define HOLDER       035
#define IGNORE_EOL   032
#define NEST_TEXT    031
#define QUOTE_START  030
#define TAG_START    027
#define TAG_END      026
#define NBSP         025
#define BLOCK_START  024
#define BLOCK_END    023
#define CENTRE_START 022
#define UTF8         1
#define VOCAB_SIZE 612
#define TEXT_BYTES 226741
#define USEQUERY 1
#endif

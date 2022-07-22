/* advcust.c (adv4) - copyleft @ M.L.Arnautov 1990-1994.
 *
 * === Functional routine stubs for you to customise === 
 *
 * 07 Mar 91   MLA         Alliants have getlogin (), not cuserid ().
 * 22 Feb 91   MLA         Replace '.'s with '-'s in MS-DOS file names.
 * 22 Nov 90   MLA         Change save name format for MSD.
 * 15 Sep 90   MLA         Initial coding.
 *
 */

#include <stdio.h>
#include "autod1.h"

extern FILE *log_file;
#define PRINTF(X) printf (X); if (log_file) (void) fprintf (log_file, X);

/*.......................... adv_data_path .............................
 * 
 * This routine returns the pathname of the data file.  As  supplied, it
 * is  assumed  that the data file is in the player's current directory. 
 * You may  -  very  sensibly -  wish  to  put  it  elsewhere,  e.g.  in 
 * /usr/games/lib/adv4+.    To   do   that,   redefine  DATA_DIR  below, 
 * remembering to terminate the directory pathname with a '/'.
 *
 */
 
void adv_data_path (data_path)
char *data_path;
{
#define DATA_DIR ""                       /* Defined as current dir     */
                                          /* Redefine as appropriate!   */
   int len;

   (void) strcpy (data_path, DATA_DIR);   /* Copy in the directory part */
   len = strlen (data_path);              /* Get length so far          */
   (void) strncat (data_path, TITLE, 10); /* Follow by start of title   */
   *(data_path + len + 10) = '\0';        /* Make sure it's terminated! */
   (void) strcat (data_path, ".dat");     /* Append the suffix          */
   return;
}

/*......................... adv_save_path ..............................
 *
 * This  routine  constructs  the pathname for saving/restoring/deleting 
 * games.   As supplied, the games  get  saved/restored/deleted  in  the
 * current directory.  You may want to put  them  elsewhere  -  e.g.  in
 * /usr/games/savedir.   To  do  so,  change  the SAVE_DIR symbol below,
 * remembering to terminate the specified directory pathname with a '/'.
 * In  case  saved  games  from different programs and players are mixed
 * together, the name has a structure appropriate to the  system:  under
 * PRIMOS,  VMS  and  MSDOS  it is given the .asv suffix, while on other
 * systems it is prefixed with an 'A';  on  multiuser  systems  it  also
 * incorporates  at  least  some  of the player login id.  The dump name
 * supplied by the player gets trunctade to make sure that  the  overall
 * length  is OK.  (Note that it already has been forced to lower case!)
 *
 */
 
void adv_save_path (save_name, save_path)
char *save_name;
char *save_path;
{
#define SAVE_DIR ""         /* Could be "/usr/games/lib/" or whatever */

#ifndef MSD
   char lognam [32];                     /* Player's ASCII login name */
#endif /* MSD */
#ifdef GETLOGIN
   char *getlogin ();
#endif /* GETLOGIN */
   char *save_nm;                 /* Discardable pointer to save_name */

   save_nm = save_name;
   while (*save_nm)
   {
      if (*save_nm == '\n')
      {
         *save_nm = '\0';            /* Strip off trailing \n, if any */
         break;
      }
#ifdef MSD
      if (*save_nm == '.')
         *save_nm = '-';     /* DOMESTOS doesn't like double suffixes */
#endif /* MSD */
      save_nm++;
   }

#ifdef MSD
   *(save_name + 8) = '\0';
   (void) sprintf (save_path, "%s%s.asv", SAVE_DIR, save_name);
#else /* !MSD */
#ifdef GETLOGIN
   (void) strncpy (lognam, getlogin (), 32);
#else /* !GETLOGIN */
   (void) cuserid (lognam);              /* Get login name            */
#endif /* GETLOGIN */
#if defined (__50SERIES) || defined (vms)
   lognam [10] = '\0';                   /* Truncate id to 10 chars   */
   *(save_name + 16) = '\0';             /* Chop given name at 16     */
   (void) sprintf (save_path, "%s%s_%s.ASV", SAVE_DIR, lognam, save_name);
#else /* !__50SERIES && !vms */
   lognam [3] = '\0';                    /* Ensure it's <= 3 chars!   */
   *(save_name + 10) = '\0';             /* Ensure name <= 10 chars   */
   (void) sprintf (save_path, "%sA%s%s", SAVE_DIR, lognam, save_name);
#endif /* __50SERIES, vms */
#endif /* MSD */
   return;
}

/*.............................. adv_news ..............................
 *
 * Most  likely  you  will want this routine to list a file stashed away
 * somewhere  handy  -  e.g.  in  /usr/games/lib/adv4+ ?   As  supplied,  
 * there's no news.
 *
 */
 
void adv_news ()
{
   (void) PRINTF ("There's no Adventure4 news on this system at present.");
   return;
}

/*.............................. adv_hours .............................
 *
 * Prints  out  the  hours the cave is open.  If restrictions other than
 * time are applied, these really ought to be spelled out to the hapless
 * player too!  As supplied, there are no resrictions.
 *
 */
 
void adv_hours ()
{
   (void) PRINTF ("Open at all times!\n");
   return;
}

/*........................... adv_allowed ..............................
 *
 * Returns  0  iff player may start a full game at this moment.  If not,
 * it would be polite to specify the reasons (e.g. out of hours,  system
 * too  busy  or whatever...).   Just phrase it carefully - the game may
 * still offer him a demo session instead.
 *
 */
 
int adv_allowed ()
{
   return (0);                               /* 0 mens OK, 1 means no */
}

/*......................... adv_still_allowed ..........................
 *
 * This  routine  differs  subtly  from  adv_allowed  in  that it checks 
 * whether it is OK to carry on with the game in progress.   The  checks
 * here  may  (or  may  not) differ from the ones for *starting* a game. 
 * Again, be polite in stating the facts of life  -  the  adventure  may 
 * decide  to set a tight limit on his turns, instead of simply chucking 
 * him out.
 *
 */
 
int adv_still_allowed ()
{
   return (0);                               /* 0 mens OK, 1 means no */
}

/* adv01.c: A-code kernel - copyleft Mike Arnautov 1990-2010.
 *
 * 01 Apr 10   MLA             Added make_copy functionality.
 * 21 Jul 09   MLA             Allow for GNU automake builds.
 * 13 Feb 09   MLA             Don't need process_saved for styles < 11.
 * 24 May 07   Stuart Munro    Added check for _MSC_EXTENSIONS.
 *                             Also define __STDC__ explicitly as 1.
 * 14 May 07   MLA             Split off from adv00.c (M$ sucks!)
 */
 
#if (defined(__cplusplus) || defined(_MSC_EXTENSIONS)) && !defined(__STDC__)
#  define __STDC__ 1
#endif

#if defined(HAVE_CONFIG_H)
#  include "config.h"
#  if defined (HAVE_STRING_H)
#     include <string.h>
#  else /* ! HAVE_STRING_H */
#     if defined(HAVE_STRINGS_H)
#        include <strings.h>
#     endif /* HAVE_STRINGS_H */
#  endif /* HAVE_STRING_H */
#else /* ! HAVE_CONFIG_H */
#  include <stdio.h>
#  include <sys/stat.h>
#endif /* HAVE_CONFIG_H */
#include <string.h>

#ifdef HTTP
#  undef GLK
#  undef READLINE
   int lsock;
   int wsock;
#endif

#define ADV01
#include "adv1.h"

#ifdef __STDC__
extern void outchar (int);
#else
extern void outchar ();
#endif

#define PRINTF(X)    { char *ptr = X; while (*ptr) outchar(*ptr++); }

#ifdef _WIN32
#  include <windows.h>
#  ifdef HTTP
#     include <winsock.h>
#     define sclose closesocket
#  endif
#else
#  define sclose close
#endif

#ifdef GLK
#ifdef _WIN32

char* arglist[1] = { "" };
extern char **argv;
extern int argc;

int winglk_startup_code(const char* cmdline)
{
   winglk_app_set_name(GAME_NAME);
   winglk_window_set_title(GAME_ID);
   argc = 1;
   argv = arglist;
   return 1;
}
/* Entry point for all Glk applications */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  /* Attempt to initialise Glk */
  if (InitGlk(0x00000601) == 0)
    exit(0);

  /* Call the Windows specific initialization routine */
  if (winglk_startup_code(lpCmdLine) != 0)
  {
/* Run the application */
    glk_main();

/* There is no return from this routine */
    glk_exit();
  }

  return 0;
}
#endif /* _WIN32 */
#endif /* GLK */

/*===========================================================*/

#if (defined(MSDOS) || defined (_WIN32)) && !defined(DJGPP)

#if !defined(NO_SLOW) && !defined(READLINE) && !defined(GLK)
#ifdef __STDC__
void my_usleep (int delay)
#else
void my_usleep (delay)
int delay;
#endif
{ SleepEx(delay, 0); return; }
#endif /* SLOW */
#endif /* MSDOS || WIN32 || DJGPP */

/*===========================================================*/

#if STYLE > 11

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

#if (!defined(MSDOS) && !defined(_WIN32)) || defined (DJGPP)
#include <dirent.h>
#endif
#include <stdio.h>

#ifdef __STDC__
void make_copy (char *srcdir, char *name, char *sfx)
#else
void make_copy (srcdir, name, sfx)
char *srcdir;
char *name;
char *sfx;
#endif
{
   char path [128];
   FILE *infile;
   FILE *outfile;
   int c;
   
   sprintf (path, "%s%c%s.%s", srcdir, SEP, name, sfx);
   if ((infile = fopen (path, "rb")) == NULL) return;
   sprintf (path, "%s.%s", name, sfx);
   if ((outfile = fopen (path, "wb")) == NULL)
   {
      fclose (infile);
      return;
   }
   while ((c = fgetc (infile)) != EOF)
      fputc (c, outfile);
   fclose (infile);
   fclose (outfile);
}

/*====================================================================*/

#ifdef __STDC__
int process_saved (int action, char *name)
#else
int process_saved (action, name)
int action;
char *name;
#endif
{
   int cnt = 0;
   char buf[64];
#if (defined(MSDOS) || defined(_WIN32)) && !defined(DJGPP)
   WIN32_FIND_DATA wfd;
   HANDLE hFind; 
#else
   DIR *dp;
   struct dirent *de;
#endif
   char *sfx;
   *(buf + 63) = '\0';

#if (defined(MSDOS) || defined(_WIN32)) && !defined(DJGPP)
   if (action < 1 && strlen (name) > 57)
      return (0);
   sprintf (buf, "%s\\*.adv", action >= 0 ? "." : name);
   if ((hFind = FindFirstFile (buf, &wfd)) == INVALID_HANDLE_VALUE)
#else
   if ((dp = opendir(action >= 0 ? "." : name)) == NULL)
#endif
      return (0);

   while (1)
   {
#if (defined(MSDOS) || defined(_WIN32)) && !defined(DJGPP)
      if (cnt) 
      {
         if (FindNextFile(hFind, &wfd) == 0)
            break;
      }
      strncpy(buf, wfd.cFileName, 63);
#else
      if ((de = readdir(dp)) == NULL)
         break;
      strncpy (buf, de->d_name, 63);
#endif
      if (*buf != '.' &&
         strcmp (sfx = buf + strlen(buf) - 4, ".adv") == 0)
      {
         *sfx = '\0';
         if (action < 0)
         {
            struct stat stat_buf;
            if (stat (buf, &stat_buf) == -1)
            {
               make_copy (name, buf, "adv");
               make_copy (name, buf, "adh");
            }
         }
         else if (action > 0)
         {
            if (cnt) PRINTF (", ")
            PRINTF (buf)
         }
         else if (name && cnt == 0)
            strcpy (name, buf);
         cnt++;
      }
   }
#if (defined(MSDOS) || defined(_WIN32)) && !defined(DJGPP)
      FindClose(hFind);
#else
      closedir (dp);
#endif

   if (cnt == 0) return (0);
   if (action > 0) PRINTF (".\n");
   return (cnt);
}

#endif /* STYLE */

/**********************************************************************/

char *cftext [] = {
   "##################### Config file for A-code games ####################\n",
   "#\n",
   "# Values as initally supplied are the ones the game will default to\n",
   "# in the absence of this file. Where equivalent command line options\n",
   "# exist, they override values specified by this config file.\n",
   "#\n",
   "# All browser-related settings are ignored by games built for console\n",
   "# operation only.\n",
   "#\n",
   "#######################################################################\n",
   "#\n",
   "# Browsers will be checked for in the order in which they are listed,\n",
   "# until one is located. Browsers can be specified by their full pathname,\n",
   "# or just by the executable name. In the latter case they will be\n",
   "# searched for using the search path as specified by the PATH variable.\n",
   "# If the BROWSER keyword is followed by either LINUX or MACOS, then the\n",
   "# line applies only on the respective operating system.\n",
   "#\n",
   "   BROWSER $BROWSER              # Environment variables get evaluated\n",
   "   BROWSER LINUX      xdg-open   # This starts default browser on Linux\n",
   "   BROWSER MACOS /usr/bin/open   # This starts Safari on MacOS\n",
   "   BROWSER konqueror             # KDE native browser\n",
   "   BROWSER epiphany              # Gnome native browser\n",
   "   BROWSER firefox\n",
   "   BROWSER opera\n",
   "   BROWSER seamonkey\n",
   "   BROWSER NONE                  # Assume console mode\n",
   "#\n",
   "# Browser display colours can be specified by names or by hex RGB codes;\n",
   "# if an RGB code is is used, it must be prefixed with \\#, e.g. \\#d0e0ff.\n",
   "# And yes, our American friends can use COLOR, if they like!\n",
   "#\n",
   "   COLOUR BACKGROUND  \\#d0e0ff   # Pale blue is the default background\n",
   "   COLOUR TEXT           black   # Default output text colour\n",
   "   COLOUR PROMPT           red   # Make prompt lines red\n",
   "   COLOUR SURROUND    \\#222222   # Dark grey default for surround\n",
   "#\n",
   "# Other things to do with browser display. Game commands can be always\n",
   "# submitted simply by pressing the <ENTER> (or <RETURN>) key, but setting\n",
   "# BUTTON to YES, will also show a submit button for those who must have it.\n",
   "# The COMPACT setting allows the settings of None, Some, All and Default.\n",
   "# suppressing respectively none, some or all blank lines, or accepting\n",
   "# the game's default. The COMMANDS value shows how many commands and\n",
   "# responses the browser display should retain.\n",
   "#\n",
   "   SHOW BUTTON   Yes             # Show the submit button\n",
   "   SHOW COMPACT  Default         # Accept whatever the game defaults to\n",
   "   SHOW COMMANDS 200             # Show up to 200 last commands and responses\n",
   "   SHOW WIDTH    700             # Set display area width to 700 pixels\n",
   "#\n",
   "# The log file section is applicable both to the browser and the console\n",
   "# modes. Log file can be OFF (or NO, or NONE) or WRITE or APPEND, the\n",
   "# last two being optionally followed by name or pathname; this may indicate\n",
   "# either a specific logfile or a directory in which the logfile will be\n",
   "# placed. The default logfile name is <game_name>.log and its default\n",
   "# location is ~/acode/<game_name>\n",
   "#\n",
   "   LOGFILE NONE                  # No logfile\n",
   "#\n",
   "# Finally for the console mode only, some screen layout requirements.\n",
   "# For the JUSTIFY attribute, the possible values are Yes, No and Default.\n",
   "#\n",
   "   LAYOUT JUSTIFY DEFAULT        # Accept game's default\n",
   "   LAYOUT MARGIN        1        # Left and right text margin\n",
   "   LAYOUT WIDTH        80        # Console window width in columns\n",
   "   LAYOUT HEIGHT       24        # Console window height in lines\n",
   "#\n",
   "############################# End of file #############################\n",
   ""
};

void make_conf (void)
{
   char **cfptr = cftext;
   FILE *cfile = fopen ("acode.conf", "wb");
   if (!cfile) return;
   while (**cfptr) fputs (*cfptr++, cfile);
   fclose (cfile);  
}

/**********************************************************************/


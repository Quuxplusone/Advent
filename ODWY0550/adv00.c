/* adv00.c: A-code kernel - copyleft Mike Arnautov 1990-2010.
 */

#include "adv1.h"

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#  define WMODE "wb"
#  define UMODE "ab+"

#include "adv0.h"
#include "adv6.h"

#define SW_START     '\377'
#define SW_BREAK     '\376'
#define HOLDER       '\375'
#define IGNORE_EOL   '\373'
#define NEST_TEXT    '\372'
#define QUOTE_START  '\371'
#define BLOCK_START  '\365'
#define BLOCK_END    '\364'
#define CENTRE_START '\363'
#define PARA_START     '\360'
#define PARA_END       '\357'

#define ACDCHAR(X) putchar(X)

void outbuf (void);
int value_all;
jmp_buf loop_back;
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
int rseed;
int mainseed = 0;
int amatch = 1;
   int compress = 1;
   int end_pause = 0;
int cgi = 0;
char cgicom [160];
char *cgi_name = CGINAME;

#include "adv3.h"
#include "adv4.h"
#include "adv2.h"
#include "adv5.h"

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
char *text_buf;
int text_buf_len = 4096;
char *lptr;
int text_len = 0;
int location_all;

int query_flag = 0;
int type_all;
char title [80];
int titlen;
int quitting;
int eol_count = 0;
char *lp;

#define PRINTF(X)    { char *ptr = X; while (*ptr) outchar(*ptr++); }
#define PRINTF1(X)   fputs(X, stdout);
#define PRINTF2(X,Y) printf(X,Y);
#define LOGERROR(X)
#define CHKSUM(X,Y)  for (cptr=(char *)X,cnt=1; \
                     (unsigned int)cnt<=(unsigned int)Y;cnt++,cptr++) \
                     {chksum+=(*cptr+cnt)*(((int)(*cptr)+cnt)<<4)+Y; \
                     chksum&=07777777777L;} 

#define RESTORING "Restoring game in progress...\n\n"

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

int anyof (int l0,int l1,int l2,int l3,int l4,int l5,int l6,int l7,
           int l8,int l9,int l10,int l11,int l12,int l13,int l14,int l15)
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

int keyword (int l0,int l1,int l2,int l3,int l4,int l5,int l6,int l7,
             int l8,int l9,int l10,int l11,int l12,int l13,int l14,int l15)
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

int irand (int less_than)
{
    return rand() % less_than;
}

/*===========================================================*/

#define MAX_BREAKS 100

char get_char (int char_addr)
{
   int mask;
   mask = (char_addr >> 4) & 127;
   if (mask == 0)
      mask = char_addr & 127;
   if (mask == 0)
      mask = 'X';
   mask = (17 * mask + 13) & 127;
   return (mask ^ text [char_addr] ^ title [char_addr % titlen]);
}

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

int scrchk (char *iptr)
{
   static int lincnt = 0;
   char reply [160];

   if (!iptr) 
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
      fgets (reply, sizeof (reply) - 1, stdin);
      lincnt = 1;
      if (*reply == 'n' || *reply == 'N' || *reply == 'q' || *reply == 'Q')
      {
         memset (reply, ' ', Margin);
         strcpy (reply + Margin, "OK.\n");
         if (!compress) strcat (reply, "\n");
         PRINTF1 (reply);
         strcpy (reply + Margin, "? ");
         PRINTF1 (reply);         
         return (1);
      }
   }
   return (0);
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

void showchar(char c)
{
   putchar(c);
}

/*====================================================================*/
void format_buffer(void)
{
   char *iptr = text_buf;
   char *bptr = lptr;
   int frag = (bptr > text_buf && *(bptr - 1) != '\n');
   int type;

   optr = obuf;
   
   while (*iptr == '\n')
      iptr++;

   while (iptr < bptr)
   {
      switch (*iptr)
      {
         case '\n':
            {  if (compress < 2 || *(optr - 1) != '\n')
               oputc ('\n');
            }
            if (*(iptr + 1) == '\n')
            {
               if (compress < 2) oputc ('\n');
               while (*(iptr + 1) == '\n') iptr++;
            }
            break;
         default:
	    oputc(*iptr);
            break;
      }
      if (*iptr == '\0') break;
      iptr++;
      if (*iptr == '\0') break;
   }
   
   if (frag)
   {
      char *eptr = lptr - 1;
      if (eptr >= text_buf && *eptr != ' ') 
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
      { fputs(iptr, stdout); return; }
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
         showchar (*iptr);        /* ... just print the char */
      else
      {                          /* It's a break */
         int i;
         for (i = 0; i < add; i++) showchar (' ');  /* Print basic blanks */
         if (cnt >= lb && cnt < ub) showchar (' '); /* Add extra blank */
         cnt++;                  /* Count breaks looked at */
      }
      iptr++;                    /* Move one character up */
   }
}
/*====================================================================*/
void outtext(void)
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
      if (*jptr == '\n' || *jptr == '\0')
      {
         for (i = 0; i < mg; i++) showchar (' ');
         while (iptr < jptr) showchar (*iptr++);
         if (*jptr == '\0') break;
         showchar (*iptr++);
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
         for (i = 0; i < mg; i++) showchar (' ');
      while (*iptr) {
            showchar (*iptr++);
            if (*(iptr - 1) == '\n' && scrchk (iptr)) break;
      }
      if (c && *(bptr - 1) != '\n') showchar ('\n');
      if (!c) break;
      if (jptr < bptr) *jptr = ' ';    /* Restore the blank */
      *bptr = c;                       /* Restore whatever it was */
      iptr = bptr;
      if (scrchk (iptr)) break;
   }
}
/*====================================================================*/
void outbuf(void)
{
   int first = 0;
   
   if (!obuf)
   {
      first = 1;
      obuf = advalloc (4096);
      oblen = 4096;
   }
   if (lptr > text_buf)
   {
      char *eptr = lptr - 1;
      char trm = '\0';
      while ((*eptr == ' ' || *eptr == '\n') && eptr > text_buf)
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
   
    format_buffer();
    outtext();

/* Now zap the text buffer */

   lptr = text_buf;
   *lptr = '\0';
   text_len = 0;
}
/*====================================================================*/

#define VOC_FLAG     128
#define QUIP_FLAG     64
#define FULL_FLAG     16
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

void say (int key, int what, int qualifier)
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
      if ((what != ARG1 && what != ARG2 && what != ARG3) || tmp != BADWORD)
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
      if (tc == SW_START)
      {
         switch_size = get_char (++ta);
         ea = ta + 2 * switch_size -1;
         swqual = (what >= FTEXT) ? textqual : qualifier;
         if (brief_flag)
            swqual = (swqual == 1 || switch_size == 1) ? 0 : 1;
         if (swqual <= 0 || (swqual == 1 && what >= FTEXT))
            ta = ea + 1;
         else
         {
            ta = ta - 1 + 2 *
               ((swqual > switch_size) ? switch_size - 1 : swqual - 1);
            if (what < FTEXT) ta += 2;
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
      else if (tc == HOLDER)
      {
         if (value_flag)
         {
            sprintf (auxbuf, "%d", qualifier);
            cp = auxbuf - 1;
            while (*(++cp) != '\0')
               outchar (*cp);
            goto next_char;
         }
         else if (qualifier == ARG1 || qualifier == ARG2)
         {
            cp = (qualifier == ARG1 ? arg1_word : arg2_word);
            while (*cp != '\0')
               outchar (*cp++);
            goto next_char;
         }
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

void outchar (char text_char)
{
   if (text_len == text_buf_len - 8)
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
   else if (text_char && eol_count)
      eol_count = 0;

      text_len++;
      *lptr = text_char;
   lptr++;
   return;
}

/*===========================================================*/

void getinput (char *inbuf, int insize)
{
   char *cptr;
   if (!*text_buf && !quitting)
      PRINTF ("\n? ");
   if (cgi != 'b' && *text_buf)
   {
      outbuf();
      scrchk(NULL);
   }
   *inbuf = '\0';

   query_flag = 0;
   if (cgi != 'b')
      fgets (inbuf, insize, stdin);
   *(inbuf + insize - 1) = '\0';
   *(inbuf + insize - 2) = '\n';
   cptr = inbuf;
   while (*cptr)
   {
      if (strchr ("\"\'", *cptr))
         *cptr = ' ';
      cptr++;
   }
}

/*===========================================================*/

void advcpy (char *word_buff, int word_addr)
{
   int wlen;

   if (get_char (word_addr) == '!')
      word_addr++;
   for (wlen = 1; wlen <= 20; wlen++)
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

void default_to (int key, int place, int type)
{
   int index;
   int fits;

   if (key == 0 && value [STATUS] != 1) return;
   fits = -1;
   for (index = FOBJ; index <= LOBJ; index++)
   {
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
      arg2_word [19] = '\0';
      value [STATUS] = 2;
      return;
   }

   return;
}

void find_word (int *type, int *refno, int *tadr, int which_arg)
{
   int bottom, middle, top;
   int old_refno;
   int exact;
   int va;
   char *wp;
   int ra;
   char myword [WORDSIZE];
  
   *type = -1;
   strncpy (myword, 
      which_arg == 0 ? orphan_word : tp [tindex], WORDSIZE);
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
      if (get_char (ra) == '\0' || ra - va >= 5)
      {
         *type = voc_type [bottom];
         *refno = voc_refno [bottom];
         *tadr = voc_word [bottom];
         if (get_char (ra) == '\0')
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
         old_refno = *refno;
         if (get_char (va) == '\0') break;
      }
      bottom++;
   }

done:
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

void input (int textref)
{
   int type;
   int refno;
   int tadr;
   int continuation = 0;

   if (textref == -1)
      goto get_arg1;

   if (value [STATUS] < 90 || value [STATUS] >= LTEXT)
      amatch = 1;
   else if (value [STATUS] == 99)
      amatch = 0;
   else
      amatch = -1;
      
   *bitword (ARG1) = -1;        /* Just in case! */
   *bitword (ARG2) = -1; 

   arg1 = value [ARG1];
   arg2 = value [ARG2];

#ifdef PLSCLARIFY
   if (bitest ((STATUS), (PLSCLARIFY)))
   {
      orphan = arg1;
      strncpy (orphan_word, arg1_word, 20);
   }
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

restart:

   if (tp [tindex] == NULL)
   {
      if (raw_comline [0] != '\0' && raw_comline [0] != '\n')
         strncpy (old_comline, raw_comline, 160);
      comline [0] = '\0';
      while (comline [0] == '\0' || comline [0] == '\n')
      {
         scrchk (NULL);
         if (textref)
            say (0, textref, 0);
         if (! lptr) lptr = text_buf;
         getinput (comline, 160);
         strncpy (raw_comline, comline, 160);
      }
      scrchk (NULL);
      parse ();
   }

get_arg1:
   if (tp[tindex] == NULL)
      goto restart;
   
   find_word (&type, &refno, &tadr, separator [tindex] == ',' ? 2 : 1);
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
   if (separator [tindex] == ' ' && tp [tindex])
   {
      find_word (&type, &refno, &tadr, 2);
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

   if (   value [ARG1] == BADWORD   || value [ARG2] == BADWORD
                                                                 )
         tp [tindex] = NULL;    /* Forget rest of command */
   else if (value [STATUS] == 2 && (bitest (value [ARG2], VERB)) &&
      !(bitest (value [ARG1], VERB)))
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
   if (separator [tindex] == ' ')
   {
      if (tp [tindex] && strcmp (tp [tindex], AND) == 0 && 
         separator [tindex] == ' ')
            separator [++tindex] = ',';
      else if (separator[tindex] != ';')
      {
         while (separator [++tindex] == ' ');
      }
   }
   if (value [STATUS] == 1)
      value [ARG2] = -1;
   return;
}

/*===========================================================*/

int query (int textref)
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
      cptr++;
   }
   
   if (strcmp (save_name + strlen (save_name) - 4, ".adv") != 0)
      strcat (save_name, ".adv");
   return;
}

/*===========================================================*/

int memstore (int key)
{
/* Key -1  check for existence of memory image
 * Key 0   true memory save
 * Key 1   true memory restore
 * Key 2   temporary memory save
 * Key 3   temporary memory restore
 */
   static char *image_base = NULL;   /* True memory save area */
   static char *image_temp = NULL;   /* Temp save over restore area */
   char *image_ptr;

   if (key < 0)
   {
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
      return (0);
   }
   else
   {
      if (image_ptr  == NULL)
         return (1);
      memcpy (IMAGE, image_ptr, IMAGE_SIZE);      
      return (0);
   }
}

void save_game()
{
    // [ajo] This is a stub to be filled in later.
}

void restore_game()
{
    // [ajo] This is a stub to be filled in later.
}

int special (int key, int *var)
{
   static char save_name [168];
   static char *scratch;
   char file_name [168];
   int val, val1;
   char tval [12];
   static int tsiz = sizeof (time_t);
   int chksum;
   int chksav;
   char *cptr;
   int cnt;
   static int saved_value;

   switch (key)
   {
      case 1:          /* Dump game to disc */
      case 2:          /* Restore game from disc */
         val = value [ARG2];
try_again:
         if (val != -1)
         {
            if (*long_word && 
               strncmp (long_word, arg2_word, 16) == 0)
                  strcpy (file_name, long_word);
            else
               strcpy (file_name, arg2_word);
         }
         else
      case 999:
      case 998:
	 save_game();
         return 0;

restore_it:
	 restore_game();
         return (0);

      case 3:          /* Delete saved game */
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
         PRINTF("Open at all times.\n");
         return (0);
      case 7:          /* Restore saved variable value */
         saved_value = *var;
         return (0);
      case 8:          /* Get minutes since restored game dumped */
         *var = saved_value;
         return (0);
      case 9:         /* Fudge a value into ARG1 */
         *var = 0;
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
         *var = 0;
         return (0);

      case 33:  /* Check for existence of a memory save */
         *var = 0;
         return (0);

      case 34:  /* List available saved games */
         *var = 0;
         return (0);
         
      default:
         PRINTF2 ("\n \nGLITCH! Bad special code: %d\n", key);
         return (1);
   }
}

/*====================================================================*/

int initialise (char *prog)
{
   int index;
   text_buf = advalloc (text_buf_len);
   lptr = text_buf;
   *lptr++ = '\n';
   text_len = 1;
   title [0] = text [1] ^ KNOT;
   titlen = 0;
   while (++titlen < 80)
      if ((title [titlen] = text [titlen] ^ text [titlen + 1]) == '\0')
         break;
   
   tp [0] = NULL;

   for (index = FOBJ; index <= LOBJ; index++)
      bitmod ('s', index, OBJ);

   for (index = FLOC; index <= LLOC; index++)
      bitmod ('s', index, LOC);

   bitmod ('s', STATUS, BRIEF);

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

int main(int argc, char **argv)
{
   prog = *argv;
   Maxlen = Linlen - 2 * Margin;

   if (initialise (prog) != 0)
   {
      printf ("Sorry, unable to set up the world.\n");
      return (0);
   }

   value [THERE] = value [HERE] = FLOC;

         tindex = 0;
         tp [0] = NULL;
         if (setjmp (loop_back) == 0)
            initial();

   setjmp (loop_back);

   if (quitting) 
   {
      if (text_len > 0)
          outbuf();
      putchar('\n');
      exit(0);
   }
   while (1)
   {
      repeat();
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
   quitting = 1;
   longjmp (loop_back, 1);
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

int lbitest (int a1, int a2, int *a3, short *a4)
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

void flush_command (void)
{
   value_all = 0;
   tp [tindex] = NULL;
   return;
}

/*===========================================================*/

void svar (int type, int *var)
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

void pcall (int procno)
{
   fprintf (stderr, 
      "\n*GLITCH* Called proc offest %d, but game's offset range is 0 to %d!\n",
         procno, LPROC);
   if (procno == BADWORD)
   {
      fprintf (stderr, "   Probable cause: ");
      if (procno == BADWORD)   fprintf (stderr, "bad word");
      fprintf (stderr, " in player command not handled by game's code.\n");
   }
   fprintf (stderr, "\n");                  
}

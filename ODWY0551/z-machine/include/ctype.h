
#ifndef H_CTYPE
 #define H_CTYPE

#define toupper(ch) ((ch) & ~(('a' <= (ch) && (ch) <= 'z') ? 0x20 : 0x00))
#define tolower(ch) ((ch) | (('A' <= (ch) && (ch) <= 'Z') ? 0x20 : 0x00))
#define isspace(ch) ((ch)==' ' || (ch)=='\n')

#endif /* H_CTYPE */
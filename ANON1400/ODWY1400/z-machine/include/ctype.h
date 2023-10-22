
#ifndef H_CTYPE
 #define H_CTYPE

static inline int toupper(int ch) { return ((ch) - (('a' <= (ch) && (ch) <= 'z') ? 0x20 : 0x00)); }
static inline int tolower(int ch) { return ((ch) + (('A' <= (ch) && (ch) <= 'Z') ? 0x20 : 0x00)); }
static inline int isspace(int ch) { return ((ch) == ' ' || (ch) == '\n'); }

#endif /* H_CTYPE */

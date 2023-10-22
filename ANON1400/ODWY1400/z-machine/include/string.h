
#ifndef H_STRING
 #define H_STRING

void strcpy(char *dst, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, int n);
char *strchr(const char *s, char c);
void strncpy(char *dst, const char *src, int n);

static inline unsigned strlen(const char *s) {
    return strchr(s, '\0') - s;
}

static inline void strcat(char *s, const char *t) {
    strcpy(strchr(s, '\0'), t);
}

#endif /* H_STRING */

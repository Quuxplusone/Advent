#define LABDEF 1
#define LABUSE 2
#define BARRIER 4
#define LBARRIER 8
#define BRANCH 16 /* only move if target also moved */

typedef struct line {
  struct line *next,*prev;
  char *code;
  int flags;
  int size;
  int idx;
  int l1,l2;
} line;

#define MAXMLEN 64

typedef struct match {
  struct match *next;
  line *start;
  int max;
  int size[MAXMLEN];
} match;

typedef struct {
  int usecnt;
} labinfo;

#define LABNUM 10000

#define MAXLEN 1024


/* functions available to backend */
void *mymalloc(size_t sz);
int new_label(void);
void extend_labinfos(int n);
line *new_line(void);
void insert_line(line *after, line *new);
void remove_line(line *p);
void free_line(line *p);

/* functions provided by the backend */
extern int minsave;
extern const char tg_copyright[];

void parse_line(char *s,line *p);
void add_header(line *after);
void add_ret(line *after);
void add_jsr(line *after);

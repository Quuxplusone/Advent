#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "supp.h"

extern FILE *infile;
#define MAXLEN 1024
extern char tkname[MAXLEN];
extern int label;
extern int nesting;
extern int local_offset,parm_offset,framesize;

#define REAL DOUBLE
#define FUNCTION FUNKT

#define YYERROR_VERBOSE 1

enum nodeflags {
  NVARIABLE,NNUMBER,NADD,NMUL,NSUB,NDIV,NINDEX,
  NEQUALS,NLT,NGT,NLEQ,NGEQ,NNEQ,NAND,NOR,
  NI2R,NR2I,NASSGN,NCALL,NARG
};

extern char *nodename[];

typedef struct Typ type;

typedef struct obj operand;

typedef struct var {
  int nesting;
  char *name;
  struct Typ *type;
  struct var *next;
  int offset;
  struct Var *vbccvar;
} var;

#define MAXNESTING 128
extern var *first_var[128];

typedef struct {
  int flags;
  char *name;
} *stype;

typedef struct mnode {
  enum nodeflags flags;
  struct mnode *left,*right;
  type *type;
  var *var;
  int ivalue;
  double rvalue;
} node;

void enter_block(void);
void leave_block(void);
void enter_func(char *,type *);
void leave_func(void);
var *find_var(char *,int);
var *add_var(char *,type *);
var *new_temp(int);
char *add_string(char *);
void error(int,...);
void *getmem(size_t);
node *number_node(void);
node *var_node(void);
node *conv_tree(node *,int);
node *binary_node(enum nodeflags,node *,node *);
void print_tree(node *);
void simplify_tree(node *);
void free_tree(node *);
type *new_type(int);
type *new_array(type *,node *);
void return_statement(node *);
void while_statement(node *);
void while_end(void);
void if_statement(node *);
void if_end(void);
void if_else(void);
void assign_statement(node *,node *);
void push_int(int);
int pop_int(void);
void push_name(char *);
char *pop_name(void);
operand *gen_tree(node *);
void gen_cond(node *,int,int);
void gen_label(int);
int push_arg(node *);
void gen_global(char *,int);

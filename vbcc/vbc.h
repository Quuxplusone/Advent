/*  $VER: vbcc (vbc.h) V0.8     */

#include "supp.h"

#define eval_constn(a) eval_const(&a->val,a->ntyp->flags)

struct identifier_list{
    char *identifier;
    int length;
    struct identifier_list *next;
};
struct struct_identifier{
/*    int flags;*/
  char *identifier;
  struct struct_declaration *sd;
  struct struct_identifier *next;
};

#ifndef NODES
#error wrong node
struct node{
    int flags,lvalue,sidefx;
    struct Typ *ntyp;
    struct node *left;
    struct node *right;
    struct argument_list *alist;
    char *identifier;
    union atyps val;
    struct obj o;
};

typedef struct node *np;

#define NODES sizeof(struct node)
#endif

struct argument_list{
  np  arg;
  struct argument_list *next;
  struct IC *pushic;
};


#define MAXI 512 /* maximale Laenge von Identifiers in Bytes    */
#define MAXN 128 /* maximale Verschachtelung von Bloecken */

extern struct tunit *first_tunit,*last_tunit;
extern struct Typ *arith_typ(struct Typ*,struct Typ *);
extern void insert_constn(np);
extern int int_erw(int);
extern int type_expression(np),compatible_types(struct Typ *,struct Typ *,int),
    compare_sd(struct struct_declaration *,struct struct_declaration *);
#if HAVE_AOS4
extern int aos4_attr(struct Typ *,char *);
#endif
extern np identifier_expression(void),constant_expression(void),string_expression(void),
   postfix_expression(void),unary_expression(void),cast_expression(void),
   multiplicative_expression(void),additive_expression(void),
   shift_expression(void),relational_expression(void),equality_expression(void),
   and_expression(void),exclusive_or_expression(void),
   inclusive_or_expression(void),logical_and_expression(void),
   logical_or_expression(void),conditional_expression(void),
   assignment_expression(void),expression(void),primary_expression(void);
extern struct argument_list *argument_list_expression(void);
extern struct const_list *cl_from_string(char *start, char *end);

extern np new_node(void);

/* puh  */
extern int is_keyword(char *);
extern void pre(FILE *,np),pra(FILE *,struct argument_list *);
extern void free_expression(np),free_alist(struct argument_list *);
extern void cpbez(char *m,int ckw),cpnum(char *m),killsp(void);
extern void copy_token(/*no prototype because cpp.h not always included*/);
extern void push_token(/*no prototype because cpp.h not always included*/);
extern void next_token(void);
extern struct struct_declaration *add_sd(struct struct_declaration *,int);
extern void add_sl(struct struct_declaration *,struct struct_list (*)[]);
extern void free_sd(struct struct_declaration *);
extern void prl(FILE *,struct struct_declaration *);
extern char *add_identifier(char *,int);
extern struct Typ *declarator(struct Typ *),*direct_declarator(struct Typ *),
           *pointer(struct Typ *),*declaration_specifiers(void);
extern int declaration(int),type_uncomplete(struct Typ *);
extern struct const_list *initialization(struct Typ *,int,int,int,struct struct_declaration *,struct const_list *);
extern void init_local_compound(struct Var *);
extern zmax init_dyn_sz,init_const_sz;
extern int init_dyn_cnt,init_const_cnt;
extern struct struct_declaration *find_struct(char *,int);
extern void add_struct_identifier(char *,struct struct_declaration *);
extern void free_si(struct struct_identifier *);
extern char *ident;
extern char number[MAXI],buff[MAXI];
extern struct struct_declaration *first_sd[MAXN],*last_sd[MAXN],*merk_sdf,*merk_sdl;
extern struct struct_identifier *first_si[MAXN],*last_si[MAXN],*merk_sif,*merk_sil;
extern struct identifier_list *first_ilist[MAXN],*last_ilist[MAXN],*merk_ilistf,*merk_ilistl;
extern void free_ilist(struct identifier_list *);
extern int nesting;
extern struct Var *first_var[MAXN],*last_var[MAXN],*merk_varf,*merk_varl,*first_ext,*last_ext;
extern struct Var *block_vla[MAXN];
extern struct llist *vladeflabels[MAXN],*vlajmplabels[MAXN];
extern struct vlaadjust_list *vlaadjusts[MAXN];
extern void freevl(void);
extern void clearvl(void);
extern void free_var(struct Var *);
extern void var_declaration(void);
extern int storage_class_specifiers(void);
extern void enter_block(void),leave_block(void);
extern struct Var *find_var(char *,int);
extern struct Var *find_ext_var(char *);
extern struct Var *add_var(char *,struct Typ *,int,struct const_list *);
extern void fi_from_attr(struct Var *);

extern int usz;

extern int c99;
extern int opencl;
extern int disallow_statics;
extern int header_cnt;
extern int softfloat;
#define MAGIC_WPO 123
extern int wpo,wpo_key;
extern FILE *input_wpo;

extern void gen_IC(np,int,int),convert(np,int),gen_label(int);
extern void gen_test(struct obj *,int,int,int);
extern void savescratch(int,struct IC *,int,struct obj *);
struct regargs_list{
  struct regargs_list *next;
  struct argument_list *al;
  int reg;
  struct Var *v;
};
#ifdef HAVE_REGPARMS
extern zmax push_args(struct argument_list *,struct struct_declaration *,int,struct regargs_list **,struct reg_handle *,struct obj *,struct Typ *,int,struct Typ *);
#else
extern zmax push_args(struct argument_list *,struct struct_declaration *,int,struct regargs_list **);
#endif
extern int allocreg(int,int);
extern void free_reg(int),alloc_hardreg(int);

extern FILE *out,*ic1,*ic2,*ppout,*cmdfile;

extern void statement(void),labeled_statement(void),if_statement(void);
extern void switch_statement(void),while_statement(void),for_statement(void);
extern void do_statement(void),goto_statement(void),continue_statement(void);
extern void break_statement(void),return_statement(void);
extern void expression_statement(void),compound_statement(void);
extern void translation_unit(void);
extern int main(int, char *[]);
extern int nocode,dontdelete,registerpri,looppri,currentpri;
extern int no_cast_free;

extern np makepointer(np);

struct vlaadjust_list {
  struct IC *branch,*first;
  struct vlaadjust_list *next;
};

extern int switch_typ,switch_count,switch_act;
struct llist{
    char *identifier;
    int label,flags,switch_count;
    struct llist *next;
    union atyps val;
};
#define LABELDEFINED 1
#define LABELUSED 2
#define LABELDEFAULT 4
#define LSIZE sizeof(struct llist)
extern struct llist *first_llist,*last_llist;
extern struct llist *find_label(char *),*add_label(char *);
extern void free_llist(struct llist *);

extern int endok,return_label,return_value,break_label;
extern int const_expr,for_decl;
extern struct Var *return_var;
extern struct Typ *return_typ;
extern zmax local_offset[MAXN];

extern void scratch_var(struct obj *,int,struct Typ *);
extern void get_scratch(struct obj *,int,int,struct Typ *);
extern void gen_cond(struct obj *,int,int,int);

#define MAXCF 60
extern int c_flags[MAXCF];
extern char *c_flags_name[MAXCF];
extern union ppi c_flags_val[MAXCF];

extern FILE *open_out(char *,char *);

extern char *inname;

extern void gen_vars(struct Var *);

/*  Format der Tabelle fuer Fehlermeldungen */
struct err_out{
    char *text;
    int  flags;
};


/*  Flags fuer err_out.flags    */
#define ERROR       1
#define WARNING     2
#define ANSIV       4
#define INTERNAL    8
#define FATAL      16
#define MESSAGE    32
#define DONTWARN   64
#define PREPROC   128
#define NOLINE    256
#define INFUNC    512
#define INIC     1024
#define NORAUS   2048

extern struct err_out err_out[];
extern int err_num;

struct misra_err_out{
		int chapter;
		int rule;
		char *text;
		int flags;
};

extern struct misra_err_out misra_err_out[];

#define MISRA								1
#define	MISRA_PREPROC				2
#define MISRA_2004					4
#define MISRA_1998					8

#ifdef HAVE_ECPP
struct ecpp_dtor_list {
  struct Var *var;
  struct ecpp_dtor_list *next;
};
extern struct struct_declaration *current_class;
extern struct Typ *current_func;
extern struct ecpp_dtor_list *ecpp_dlist[MAXN];
struct Var *ecpp_find_ext_var(char *identifier);
struct Var *ecpp_find_var(char *identifier);
struct struct_declaration *ecpp_find_scope(char* nested_name,char** identifier);
struct struct_list *ecpp_find_member(char* identifier,struct struct_declaration *scope,struct struct_declaration** ret_scope,int search_flag);
void ecpp_auto_call_dtors();
int ecpp_is_friend(struct struct_declaration *class);
#endif

extern int afterlabel;

extern int errors;

extern int float_used;
extern char *cur_func;
extern int line;
extern void free_clist(struct const_list *);
extern struct const_list *first_clist,*last_clist;
extern struct Var *regsbuf[MAXR+1];
extern int regbnesting[MAXR+1];



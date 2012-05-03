#ifndef SUPP_H
#define SUPP_H 1
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
/* typenames */
#define CHAR 1
#define SHORT 2
#define INT 3
#define LONG 4
#define LLONG 5
#define FLOAT 6
#define DOUBLE 7
#define LDOUBLE 8
#define VOID 9
#define POINTER 10
#define ARRAY 11
#define STRUCT 12
#define UNION 13
#define ENUM 14
#define FUNKT 15
#define MAXINT 16 /* should not be accesible to application */
#define MAX_TYPE MAXINT
#define NQ 127   /* f&NQ gives type without any qualifiers */
#define NU 255   /* f&NU gives type without any qualifiers but UNSIGNED */
#define q1typ(p) ((p->code==ADDI2P||p->code==SUBIFP||p->code==CONVERT||p->code==SUBPFP)?p->typf2:p->typf)
#define q2typ(p) ((p->code==SUBPFP||p->code==LSHIFT||p->code==RSHIFT)?p->typf2:p->typf)
#define ztyp(p) ((p->code==ADDI2P||p->code==SUBIFP||p->code==ADDRESS)?p->typf2:p->typf)
#define iclabel(p) (p->typf)
#define opsize(p) (p->q2.val.vmax)
#define pushsize(p) (p->z.val.vmax)
#define pushedargsize(p) (p->q2.val.vmax)
#define isstatic(x) ((x)==STATIC)
#define isextern(x) ((x)==EXTERN)
#define isauto(x)   ((x)==AUTO||(x)==REGISTER)
/* operations on bit-vectors */
typedef unsigned int bvtype;
#define BVBITS (sizeof(bvtype)*CHAR_BIT)
#define BVSIZE(x) ((((x)+BVBITS-1)/BVBITS)*sizeof(bvtype))
#define BSET(array,bit) (array)[(bit)/BVBITS]|=1<<((bit)%BVBITS)
#define BCLR(array,bit) (array)[(bit)/BVBITS]&=~(1<<((bit)%BVBITS))
#define BTST(array,bit) ((array)[(bit)/BVBITS]&(1<<((bit)%BVBITS)))
/* type-qualifiers */
#define UNSIGNED 128
#define CONST 256
#define VOLATILE 512
#define RESTRICT 1024
#define UNCOMPLETE 2048
#define STRINGCONST 4096
#define BOOLEAN 8192
#define SIGNED_CHARACTER	16384
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
#define t_min(x) (((x)&UNSIGNED)?l2zm(0L):t_min[(x)&NQ])
#define t_max(x) (((x)&UNSIGNED)?tu_max[(x)&NQ]:t_max[(x)&NQ])
/*  macro for internal errors */
#define ierror(a) error(-1,(a),__LINE__,FILE_)
/* macros for target errors */
#define twarning(x) error(323,x);
#define terror(x) error(324,x);
/* this header is provided by the code generator */
#include "machine.h"
#ifndef HAVE_EXT_TYPES
typedef zllong zmax;
typedef zullong zumax;
#endif
#ifndef ISPOINTER
#define ISPOINTER(x) ((x&NQ)==POINTER)
#endif
#ifndef ISINT
#define ISINT(x) ((x&NQ)>=CHAR&&(x&NQ)<=LLONG)
#endif
#ifndef ISFLOAT
#define ISFLOAT(x) ((x&NQ)>=FLOAT&&(x&NQ)<=LDOUBLE)
#endif
#ifndef ISFUNC
#define ISFUNC(x) ((x&NQ)==FUNKT)
#endif
#ifndef ISSTRUCT
#define ISSTRUCT(x) ((x&NQ)==STRUCT)
#endif
#ifndef ISUNION
#define ISUNION(x) ((x&NQ)==UNION)
#endif
#ifndef ISARRAY
#define ISARRAY(x) ((x&NQ)==ARRAY)
#endif
#ifndef ISSCALAR
#define ISSCALAR(x) ((x&NQ)>=CHAR&&(x&NQ)<=POINTER)
#endif
#ifndef ISARITH
#define ISARITH(x) (ISINT(x)||ISFLOAT(x))
#endif
#ifndef POINTER_TYPE
#define POINTER_TYPE(x) POINTER
#endif
#ifndef MAXINT
#define MAXINT LLONG
#endif
#ifndef HAVE_INT_SIZET
#define HAVE_INT_SIZET 0
#endif
#ifndef PTRDIFF_T
#define PTRDIFF_T(x) INT
#endif
#ifndef MAXADDI2P
#define MAXADDI2P LLONG
#endif
#define RSIZE BVSIZE(MAXR+1)
#define ALL_CALLS 1
#define ALL_REGS  2
#define ALL_USES  4
#define ALL_MODS  8
#define ALL_STACK 16
#define ALWAYS_RETURNS 32
#define NEVER_RETURNS 64
#define NOSIDEFX 128
#define WARNED_STACK 256
#define WARNED_REGS 512
#define USES_VLA 1024
struct tunit{
  struct Var *statics;
  struct tunit *next;
};
/*  additional information for functions; used by the optimizer  */
struct function_info{
  struct IC *first_ic;    /* inline copy of function starts here */
  struct IC *last_ic;     /*  "       "       "      ends here   */
  struct IC *opt_ic;      /* code after optimizing */
  struct Var *vars;       /* pointer to list of vars of that function */
  struct Var *statics;    /* static variables of translation-unit */
  char *inline_asm;       /* pointer to code for inline assembler */
  unsigned long flags;    /* misc flags, see above */
  /* functions called, variables used and changed */
  int call_cnt,use_cnt,change_cnt;
  struct varlist *call_list;
  struct varlist *use_list;
  struct varlist *change_list;
  zmax max_offset;
  /* registers used and modified by that function */
  bvtype regs_modified[RSIZE/sizeof(bvtype)];
#if HAVE_OSEK
/* removed */
/* removed */
/* removed */
#endif
  zumax stack1;
  zumax stack2;
};
/*  struct for types.    */
struct Typ{
  int flags;  /*  see above   */
  struct Typ *next;
  struct struct_declaration *exact;   /* used for STRUCT/UNION/FUNKT  */
  zmax size;     /*  used for ARRAY  */
  struct Var *dsize; /* used for variable-length arrays */
  char *attr;
  int reg;
#ifdef HAVE_ECPP
/* removed */
#endif
};
#define TYPS sizeof(struct Typ)
#ifndef HAVE_EXT_TYPES
union atyps{
  zchar vchar;
  zuchar vuchar;
  zshort vshort;
  zushort vushort;
  zint vint;
  zuint vuint;
  zlong vlong;
  zulong vulong;
  zllong vllong;
  zullong vullong;
  zmax vmax;
  zumax vumax;
  zfloat vfloat;
  zdouble vdouble;
  zldouble vldouble;
};
#endif
/* This struct represents objects in the intermediate code. */
struct obj{
  int flags;      /* see below */
  int reg;        /* number of reg if flags&REG */
  int dtyp;       /* type of pointer for DREFOBJ */
  struct Var *v;
  struct AddressingMode *am;
  union atyps val;
};
/*  Available flags in struct obj.  */
                    /*  KONST muss immer am kleinsten sein, um beim Swappen */
                    /*  fuer available_expressions und Konstanten nach      */
                    /*  rechts nicht in eine Endlosschleife zu kommen.      */
#define KONST 1     /*  The object is a constant. Its value is stored in    */
                    /*  val.                                                */
#define VAR 2       /*  The object is a variable (stored in v).             */
#define SCRATCH 8   /*  The object is a temporary.                          */
#define STACK 16    /*  obsolete                                            */
#define DREFOBJ 32  /*  The object must be dereferenced.                    */
#define REG 64      /*  The object is contained in a hardware register.     */
#define VARADR 128  /*  The object is the address of a static variable.     */
#define DONTREGISTERIZE 256 /*  Do not put this object into a register.     */
#define VKONST 512  /*  Variable containing a constant (for reg-alloc).     */
struct Var{
  int storage_class;  /* see below    */
  int reg;            /* Var is assigned to this hard-reg */
  int priority;       /* Priority to be used in simple_regs() */
  long flags;         /* see below */
  char *identifier;   /* name of the variable */
  int nesting;        /* can be freely used by the frontend */
  int index;          /* used by the optimizer */
  int inr;            /* counter for num_vars */
  zmax offset;        /* offset relative to the stack frame */
  int line;           /* line number of first declaration */
  char *filename;     /* filename of first declaration */
  int dline;          /* line number of definition */
  char *dfilename;    /* filename of definition */
  struct Typ *vtyp;   /* type of the variable */
  struct const_list *clist;   /* initialized? */
  struct Var *next;   /* pointer to next variable */
  struct function_info *fi;   /* used by the optimizer */
  struct Var *inline_copy;    /* used for function-inlining */
  struct tunit *tunit; /* translation unit the variable belongs to */
  char *vattr;         /* extended variable attributes */
  char *description;   /* description of variable meaning */
  struct obj cobj;     /* used for register-allocation of constants */
  int ctyp;            /*   ----------------- " -----------------   */
#ifdef HAVE_TARGET_ATTRIBUTES
  unsigned long tattr;        /* target-specific attributes */
#endif
#ifdef ALEX_REG
  int iRegCopyNr;
  int iRegCopyNrHc12V;
#endif
};
/* available storage-classes */
#define AUTO 1          /* var is allocated on the stack */
#define REGISTER 2      /* basically the same as AUTO (C-only) */
#define STATIC 3        /* var is static but has no external linkage */
#define EXTERN  4       /* var is static and has external linkage */
#define TYPEDEF 5       /* C-only */
/* available flags in struct Var */
#define USEDASSOURCE 1      /* the var has been read */
#define USEDASDEST 2        /* the var has been written */
#define DEFINED 4           /* the var has been defined (i.e. storage will
                               be allocated in the current file) */
#define USEDASADR 8         /* the address of the var has been taken */
#define GENERATED 16        /* code for static vars has been generated */
#define CONVPARAMETER 32
#define TENTATIVE 64        /* C-only */
#define USEDBEFORE 128      /* used by the optimizer */
#define INLINEV 256         /*  "               "    */
#define PRINTFLIKE 512      /* C-only */
#define SCANFLIKE 1024      /* C-only */
#define NOTTYPESAFE 2048    /* used by the optimizer */
#define DNOTTYPESAFE 4096   /*  "               "    */
#define REGPARM 8192        /* the var is a register parameter */
#define DBLPUSH 16384       /* parameter is also on the stack */
#define NOTINTU 32768       /* variable not (yet) defined in this translation-unit */
#define REFERENCED 65536    /* variable referenced */
#define INLINEFUNC (REFERENCED*2)
#define INLINEEXT (INLINEFUNC*2)


/* C-only */
struct struct_list{
  char *identifier;   /* name of the struct/union-tag */
  struct Typ *styp;   /* type of the member/parameter */
  zmax align;         /* alignment of struct-member */
  int bfoffset;       /* bitfield-offset */
  int bfsize;         /* bitfield-size */
  int storage_class;  /* storage-class of function-parameter */
  int reg;            /* register to pass function-parameter */
#ifdef HAVE_ECPP
/* removed */
#endif
};

#define SLSIZE 32   /* realloc struct_lists in those steps */
/*  These structs are used to describe members of STRUCT/UNION or   */
/*  parameters of FUNKT. Some of the entries in struct_list are not */
/*  relevant for both alternatives.                                 */
struct struct_declaration{
  int count;  /* number of members/parameters */
  int label; /* used for debug output */
  int typ;
  struct tunit *tunit;
  struct struct_declaration *next;
  struct struct_list (*sl)[];
  char *identifier;
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
};

/*  The quads in the intermediate code. */
struct IC{
  struct IC *prev;    /* pointer to the next IC */
  struct IC *next;    /* pointer to the previous IC */
  int code;           /* see below */
  int typf;           /* usually type of the operands, see interface.doc */
  int typf2;          /* used for CONVERT,ADDI2P,SUBIFP,SUBPFP */
  int defindex;       /* used by optimizer */
  int expindex;
  int copyindex;
  int change_cnt;
  int use_cnt;
  int call_cnt;
  int line;           /* corresponding line in source file (or 0) */
  struct varlist *change_list;    /* used by optimizer */
  struct varlist *use_list;
  struct varlist *call_list;
  struct obj q1;      /* source 1 */
  struct obj q2;      /* source 2 */
  struct obj z;       /* target */
  char *file;         /* filename of the source file */
  int arg_cnt;
  struct IC **arg_list; /* pointers to function argument ICs (only for code==CALL) */
  struct IC *copy;    /* for copying/inlining */
  struct Typ *ityp;
  long flags;
  struct Var *savedsp; /* only for branch instructions and vlas */
#ifdef HAVE_EXT_IC
  struct ext_ic ext;
#endif
#ifdef ALEX_REG
  int iZWebIndex; /* web for z in case z is a pointer and its value is defed*/
  int iQ1WebIndex; /* web for argument q1 */
  int iQ2WebIndex; /* web for argument q2 */
  void *pFlow;           /* flowgraph reverse lookup */
#endif
};
#define ICS sizeof(struct IC)
/* flags for ICs */
#define LOOP_COND_TRUE 1  /* loop condition is true at first iteration */
#define EFF_IC 2          /* do not transform IC with doubtful optimizations */
/*  Available codes for struct IC. See interface.doc. */
#define KOMMA 1
#define ASSIGN 2
#define ASSIGNOP 3

#if 0
/* obsolete */
#define ASSIGNADD 3
#define ASSIGNSUB 4
#define ASSIGNMULT 5
#define ASSIGNDIV 6
#define ASSIGNMOD 7
#define ASSIGNAND 8
#define ASSIGNXOR 9
#define ASSIGNOR 10
#define ASSIGNLSHIFT 11
#define ASSIGNRSHIFT 12
#endif

#define COND 13
#define LOR 14
#define LAND 15
#define OR 16
#define XOR 17
#define AND 18
#define EQUAL 19
#define INEQUAL 20
#define LESS 21
#define LESSEQ 22
#define GREATER 23
#define GREATEREQ 24
#define LSHIFT 25
#define RSHIFT 26
#define ADD 27
#define SUB 28
#define MULT 29
#define DIV 30
#define MOD 31
#define NEGATION 32
#define KOMPLEMENT 33
#define PREINC 34
#define POSTINC 35
#define PREDEC 36
#define POSTDEC 37
#define MINUS 38
#define CONTENT 39
#define ADDRESS 40
#define CAST 41
#define CALL 42
#define INDEX 43
#define DPSTRUCT 44
#define DSTRUCT 45
#define IDENTIFIER 46
#define CEXPR 47
#define STRING 48
#define MEMBER 49
#define CONVERT 50
#if 0
#define CONVCHAR 50
#define CONVSHORT 51
#define CONVINT 52
#define CONVLONG 53
#define CONVFLOAT 54
#define CONVDOUBLE 55
#define CONVVOID 56
#define CONVPOINTER 57
#define CONVUCHAR 58
#define CONVUSHORT 59
#define CONVUINT 60
#define CONVULONG 61
#endif
#define ADDRESSA 62
#define FIRSTELEMENT 63
#define PMULT 64
#define ALLOCREG 65
#define FREEREG 66
#define PCEXPR 67
#define TEST 68
#define LABEL 69
#define BEQ 70
#define BNE 71
#define BLT 72
#define BGE 73
#define BLE 74
#define BGT 75
#define BRA 76
#define COMPARE 77
#define PUSH 78
#define POP 79
#define ADDRESSS 80
#define ADDI2P 81
#define SUBIFP 82
#define SUBPFP 83
#define PUSHREG 84
#define POPREG 85
#define POPARGS 86
#define SAVEREGS 87
#define RESTOREREGS 88
#define ILABEL 89
#define DC 90
#define ALIGN 91
#define COLON 92
#define GETRETURN 93
#define SETRETURN 94
#define MOVEFROMREG 95
#define MOVETOREG 96
#define NOP 97
#define BITFIELD 98
#define LITERAL 99
extern char *typname[];
extern zmax sizetab[];
extern char *storage_class_name[];
extern char *ename[];
extern int align_arguments; /* arguments correctly aligned on stack or
			       just stackalign? */
extern zmax align[],maxalign,stackalign;
/*  an empty string */
extern char *empty;
extern zchar vchar; extern zuchar vuchar;
extern zshort vshort; extern zushort vushort;
extern zint vint; extern zuint vuint;
extern zlong vlong; extern zulong vulong;
extern zllong vllong; extern zullong vullong;
extern zmax vmax; extern zumax vumax;
extern zfloat vfloat; extern zdouble vdouble;
extern zldouble vldouble;
extern union atyps gval;
#ifndef DEBUG
extern int DEBUG;
#endif
#ifdef HAVE_MISRA
/* removed */
#endif
/*  used by the optimizer */
/* for lists in ICs flags may be DREFOBJ to mark dereferences */
/* for lists in function infos flags is the type of access */
struct varlist{
  struct Var *v;
  int flags;
};
#define VLS sizeof(struct varlist)
extern struct IC *first_ic,*last_ic;
extern int regs[MAXR+1],regsa[MAXR+1],regused[MAXR+1],simple_scratch[MAXR+1];
extern int reg_prio[MAXR+1],regscratch[MAXR+1];
extern zmax regsize[MAXR+1];
extern struct Typ *regtype[MAXR+1];
extern struct Var *regsv[MAXR+1];
extern char *regnames[];
extern bvtype regs_modified[RSIZE/sizeof(bvtype)];
extern int optspeed,optsize,unroll_all,stack_check;
extern int cross_module,final,no_emit;
extern int debug_info;
extern int label,return_label;
/* The structures used for available options of the code generator. */
union ppi {char *p;long l;void (*f)(char *);};
#define USEDFLAG 1
#define STRINGFLAG 2
#define VALFLAG 4
#define FUNCFLAG 8
extern int g_flags[MAXGF];
extern char *g_flags_name[MAXGF];
extern union ppi g_flags_val[MAXGF];
extern zmax max_offset;
extern int function_calls,vlas;
extern int coloring;
extern int dmalloc;
extern int disable;
extern int misracheck,misraversion,misracomma,misratok;
extern int pack_align;
extern int short_push;
extern int default_unsigned;

/*  Das haette ich gern woanders    */
extern struct struct_declaration *add_sd(struct struct_declaration *,int);
struct node{
  int flags,lvalue,sidefx;
  int bfs,bfo;
  struct Typ *ntyp;
  struct node *left;
  struct node *right;
  struct argument_list *alist;
  char *identifier;
  struct const_list *cl;
  struct Var *dsize;
  union atyps val;
  struct obj o;
};
typedef struct node *np;
#define NODES sizeof(struct node)
struct const_list{
  union atyps val;
  np tree;
  zmax idx;
  struct const_list *other,*next;
};
#define CLS sizeof(struct const_list)
struct case_table{
  int num,typf;
  struct IC *next_ic;
  double density;
  union atyps *vals;
  int *labels;
  union atyps min,max;
  zumax diff;
};
extern zmax t_min[];
extern zumax t_max[];
extern zumax tu_max[];
extern zmax char_bit;
extern char cg_copyright[];
#ifdef HAVE_TARGET_ATTRIBUTES
extern char *g_attr_name[];
#endif
extern char **target_macros;
extern int goto_used;
extern int ic_count;
extern int only_inline;
extern int multiple_ccs;
extern int float_used;
extern struct IC *err_ic;
extern long maxoptpasses,optflags,inline_size,unroll_size,inline_depth;
extern int noaliasopt,fp_assoc,noitra;
extern struct Var *vl0,*vl1,*vl2,*vl3;
extern char *filename;
extern char *emit_buffer[EMIT_BUF_DEPTH];
extern char *emit_p;
extern int emit_f,emit_l;
extern int no_inline_peephole;
extern int ecpp;
/* functions which must be provided by the frontend */
extern void add_IC(struct IC *);
extern void error(int,...);
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
extern struct Var *add_tmp_var(struct Typ *);
extern void free_var(struct Var *);
extern void raus(void);
/* functions provided by supp.c */
#define printtype prd
#define printobj probj
#define printic pric2
#define printiclist pric
extern void free_IC(struct IC *);
extern void insert_IC(struct IC *,struct IC *);
extern void move_IC(struct IC *,struct IC *);
extern void pric(FILE *,struct IC *);
extern void pric2(FILE *,struct IC *);
extern void probj(FILE *,struct obj *,int);
extern void emit(FILE *,const char *,...);
extern void emit_char(FILE *,int);
void emit_flush(FILE *);
void remove_asm(void);
#if HAVE_TARGET_PEEPHOLE
extern int emit_peephole(void);
#endif
extern void emit_remove(void);
extern void emit_inline_asm(FILE *,char *);
extern void emitzm(FILE *,zmax);
extern void emitzum(FILE *,zumax);
extern void emitzld(FILE *,zldouble);
extern void emitval(FILE *,union atyps *,int);
extern void printzm(FILE *,zmax);
extern void printzum(FILE *,zumax);
extern void printzld(FILE *,zldouble);
extern void printval(FILE *,union atyps *,int);
extern void insert_const(union atyps *,int);
extern void prd(FILE *,struct Typ *);
extern void print_var(FILE *,struct Var *);
extern void freetyp(struct Typ *);
extern struct Typ *new_typ(void);
extern struct Typ *clone_typ(struct Typ *);
#ifdef HAVE_EXT_TYPES
extern void conv_typ(struct Typ *);
#endif
extern zmax szof(struct Typ *);
extern int is_vlength(struct Typ *);
extern struct Var *vlength_szof(struct Typ *);
extern zmax struct_offset(struct struct_declaration *,const char *);
extern zmax falign(struct Typ *);
int get_first_base_type(struct Typ *);
extern void eval_const(union atyps *,int);
extern struct function_info *new_fi(void);
extern void free_fi(struct function_info *);
extern void print_fi(FILE *,struct function_info *);
extern void static_stack_check(struct Var *);
#ifdef HAVE_REGPARMS
extern zmax va_offset(struct Var *);
#endif
extern void print_varlist(FILE *f,struct varlist *,int);
extern int switch_IC(struct IC *);
extern int collides(struct obj *,struct obj *);
extern int is_const(struct Typ *);
extern int is_volatile_obj(struct obj *);
extern int is_volatile_ic(struct IC *);
extern struct case_table *calc_case_table(struct IC *,double);
int calc_regs(struct IC *,int);
extern void emit_jump_table(FILE *,struct case_table *,char *,char *,int);
extern void optimize(long, struct Var *);
int bvcmp(bvtype *dest,bvtype *src,size_t len);
int bvdointersect(bvtype *,bvtype *,size_t len);
void bvunite(bvtype *dest,bvtype *src,size_t len);
void bvintersect(bvtype *dest,bvtype *src,size_t len);
void bvdiff(bvtype *dest,bvtype *src,size_t len);
void vqsort (void *,size_t,size_t,int (*)(const void *,const void *));
#define bvcopy(dest,src,len) memcpy(dest,src,len)
#define bvclear(dest,len) memset(dest,0,len)
#define bvsetall(dest,len) memset(dest,-1,len)
extern void remove_IC(struct IC *);
extern struct IC *clone_ic(struct IC *);
extern struct IC *new_IC(void);
extern struct Var *new_var(void);
extern void insert_bitfield(struct obj *,struct obj *,struct obj *,int,int,int,int);
extern void *mymalloc(size_t);
extern void *myrealloc(void *,size_t);
extern void myfree(void *p);
#define free(x) myfree(x)
extern char *mystrdup(char *);
extern void simple_regs(void);
extern int bflayout(int,int,int);
/*  functions provided by the code generator */
extern int regok(int,int,int);
struct rpair {int r1,r2;};
int reg_pair(int,struct rpair *);
extern struct rpair rp;
extern int freturn(struct Typ *);
#define ffreturn(x) ((x)->reg?(x)->reg:freturn(x))
extern void gen_code(FILE *,struct IC *,struct Var *,zmax);
extern int init_cg(void);
extern void cleanup_cg(FILE *);
extern void init_db(FILE *);
void cleanup_db(FILE *);
extern int dangerous_IC(struct IC *);
extern void gen_dc(FILE *,int,struct const_list *);
extern void gen_ds(FILE *,zmax,struct Typ *);
extern void gen_var_head(FILE *,struct Var *);
extern void gen_align(FILE *,zmax);
extern int shortcut(int, int);
extern int must_convert(int,int,int);
#ifdef HAVE_TARGET_PRAGMAS
extern int handle_pragma(const char *);
#endif
#ifdef HAVE_LIBCALLS
extern char *use_libcall(np);
#endif
extern int cost_savings(struct IC *,int,struct obj *);
/* additional declarations for targets which pass arguments in */
/* registers by default.                                       */
#ifdef HAVE_REGPARMS
extern struct reg_handle empty_reg_handle;
extern int reg_parm(struct reg_handle *, struct Typ *,int,struct Typ *);
#endif
#ifdef HAVE_TARGET_EFF_IC
extern void mark_eff_ics(void);
#else
#define mark_eff_ics()
#endif
#ifndef JUMP_TABLE_DENSITY
#define JUMP_TABLE_DENSITY 1
#endif
#ifndef JUMP_TABLE_LENGTH
#define JUMP_TABLE_LENGTH 1000
#endif
#ifndef ALLOCVLA_REG
#define ALLOCVLA_REG 0
#endif
#ifndef ALLOCVLA_INLINEASM
#define ALLOCVLA_INLINEASM 0
#endif
#ifndef FREEVLA_REG
#define FREEVLA_REG 0
#endif
#ifndef FREEVLA_INLINEASM
#define FREEVLA_INLINEASM 0
#endif
#ifndef OLDSPVLA_INLINEASM
#define OLDSPVLA_INLINEASM 0
#endif
#ifndef FPVLA_REG
#define FPVLA_REG 0
#endif
#ifndef MIN_INT_TO_FLOAT_TYPE
#define MIN_INT_TO_FLOAT_TYPE CHAR
#endif
#ifndef MIN_FLOAT_TO_INT_TYPE
#define MIN_FLOAT_TO_INT_TYPE CHAR
#endif
#ifndef AVOID_FLOAT_TO_UNSIGNED
#define AVOID_FLOAT_TO_UNSIGNED 0
#endif
#ifndef AVOID_UNSIGNED_TO_FLOAT
#define AVOID_UNSIGNED_TO_FLOAT 0
#endif
#if HAVE_OSEK
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif


#endif /* SUPP_H */

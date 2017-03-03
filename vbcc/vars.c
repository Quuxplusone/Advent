/*  $VER: vbcc (vars.c) V0.8    */
#include "vbc.h"
#ifdef AMIGA
static const char *__ver="$VER: vbcc 0.9e (07.02.2016)\r\n";
long __stack=65536;
#endif
char *s,*ident;
char number[MAXI],buff[MAXI];
struct tunit *first_tunit,*last_tunit;
struct struct_declaration *first_sd[MAXN],*last_sd[MAXN],*merk_sdf,*merk_sdl;
struct struct_identifier *first_si[MAXN],*last_si[MAXN],*merk_sif,*merk_sil;
struct identifier_list *first_ilist[MAXN],*last_ilist[MAXN],*merk_ilistf,*merk_ilistl;
struct llist *first_llist,*last_llist;
int nesting;
struct Var *first_var[MAXN],*last_var[MAXN],*merk_varf,*merk_varl,*first_ext,*last_ext;
struct Var *block_vla[MAXN];
struct llist *vladeflabels[MAXN],*vlajmplabels[MAXN];
struct vlaadjust_list *vlaadjusts[MAXN];
struct rpair rp;
FILE *out,*ic1,*ic2,*ppout,*cmdfile;
int c99;
int opencl;
int disallow_statics;
int header_cnt;
int wpo,wpo_key;
FILE *input_wpo;
int nocode,dontdelete;
int const_expr,for_decl;
int registerpri=200,currentpri,looppri=10;
int return_value,break_label,switch_typ,switch_count,switch_act;
int pointer_call;
int softfloat;
int ecpp;
struct Typ *return_typ;
struct Var *return_var;
zmax local_offset[MAXN];
int c_flags[MAXCF]={
    VALFLAG,STRINGFLAG,0,0,
    VALFLAG,0,0,0,
    VALFLAG,FUNCFLAG,FUNCFLAG,VALFLAG,
    VALFLAG,0,0,0,
    0,0,0,0,
    0,0,0,0,0,
    VALFLAG,0,0,0,0,
    0,VALFLAG,0,0,0,STRINGFLAG,0,
    VALFLAG,VALFLAG,0,VALFLAG,0,
    FUNCFLAG,FUNCFLAG,FUNCFLAG,0,
    0,0,0,0,
    0,0
};
char *c_flags_name[MAXCF]={
    "O","o","ic1","ic2",
    "debug","noasm","quiet","ansi",
    "maxerrors","dontwarn","warn","maxoptpasses",
    "inline-size","+","cpp-comments","no-trigraphs",
    "no-inline-peephole","final","E","dontkeep-initialized-data",
    "strip-path","fp-associative","iso","no-alias-opt","no-multiple-ccs",
    "unroll-size","double-push","speed","size","unroll-all",
    "stack-check","inline-depth","g","c99","wpo","cmd","noitra",
    "misra","coloring","dmalloc","disable","soft-float",
    "misrawarn","misradontwarn","reserve-reg","ecpp",
    "short-push","unsigned-char","opencl","no-include-stack",
    "deps","deps-for-libs"
};
union ppi c_flags_val[MAXCF];
char *inname;
char **target_macros;
struct Var *regsbuf[MAXR+1];
int regbnesting[MAXR+1];
struct const_list *first_clist,*last_clist;
int afterlabel;
struct err_out err_out[]={
#include "errors.h"
"",0
};
int err_num=sizeof(err_out)/sizeof(struct err_out)-1;

struct misra_err_out misra_err_out[]={
#include "misra_errors.h"
0,0,"",0
};

#ifdef HAVE_ECPP
struct struct_declaration *current_class=0;
struct Typ *current_func=0;
struct ecpp_dtor_list *ecpp_dlist[MAXN];
#endif

char *cur_func="shouldn't happen!";
char *copyright="vbcc V0.9e (c) in 1995-2016 by Volker Barthelmann";

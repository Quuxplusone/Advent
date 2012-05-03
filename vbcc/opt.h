/*  $VER: vbcc (opt.h) V0.8     */

#include "supp.h"

extern int gchanged;   /*  Merker, ob Optimierungslauf etwas geaendert hat */
extern int norek;      /*  diese Funktion wird nicht rekursiv auf          */
extern int nocall;     /*  diese Funktion kehrt nicht zum Caller zurueck   */
extern int inr;        /*  number of num_Vars iteration */

/*  temporary fuer verschiedene Bitvektoren */
extern bvtype *tmp;

/*  fuer aktive Variablen   */
extern struct Var **vilist;
extern unsigned int vcount;    /*  0..vcount-rcount-1: vars, vcount-rcount..vcount: DREFOBJs */
extern unsigned int rcount;
extern size_t vsize;
extern bvtype *av_globals,*av_address,*av_statics,*av_drefs;
extern int report_dead_statements;

/*  fuer verfuegbare Definitionen   */
extern unsigned int dcount;
extern size_t dsize;
extern struct IC **dlist;
extern bvtype **var_defs,**var_undefs;
extern bvtype **defs_kill,**defs_gen;
extern bvtype *rd_defs,*rd_tmp;
extern bvtype *rd_matrix;
#define UNDEF(x) (x+dcount)

/*  fuer verfuegbare Ausdruecke */
extern struct IC **elist;
extern unsigned int ecount;
extern size_t esize;
extern bvtype *ae_globals,*ae_address,*ae_statics,*ae_drefs;

/*  fuer verfuegbare Kopien */
extern unsigned int ccount;
extern size_t csize;
extern struct IC **clist;

/*  fuer frequency-reduction    */
extern bvtype *inloop,*invariant;

/*  alle Assignments, globaler oder Adr. fuer propagation etc.         */
extern bvtype *cp_globals,*cp_address,*cp_statics,*cp_drefs,*cp_act,*cp_dest;
/*  alle Kopieranweisungen, die eine best. Variable als Quelle haben    */
extern bvtype **copies;

extern int have_alias;
extern int static_cse,dref_cse;

struct flowgraph{
  struct IC *start,*end;
  struct flowgraph *normalout,*branchout;
  struct flowlist *in;
  int index;
  /*  Letzter Block der Schleife, falls Block Start einer Schleife ist    */
  struct flowgraph *loopend;
  /*  Anzahl Funktionsaufrufe im Block/der Schleife   */
  int calls,loop_calls;
  /*  Bitvektoren fuer aktive Variablen, reaching-definitions,
      available-expressions und available-copies */
  bvtype *av_in,*av_out,*av_gen,*av_kill;
  bvtype *rd_in,*rd_out,*rd_gen,*rd_kill;
  bvtype *ae_in,*ae_out,*ae_gen,*ae_kill;
  bvtype *cp_in,*cp_out,*cp_gen,*cp_kill;
  /* points-to-info */
  bvtype **pt;
  /*  Registervariablen   */
  struct Var *regv[MAXR+1];
  /*  Merker, ob Register gebraucht wurde; MACR+1 Bits    */
  bvtype regused[RSIZE/sizeof(bvtype)];
#ifdef ALEX_REG
  int loop_depth; /* schleifentiefe des blocks. Wird nur fuer echte Schleifen gezaehlt. */
#endif
};

extern unsigned int basic_blocks;

struct flowlist{
    struct flowgraph *graph;
    struct flowlist *next;
};

int compare_const(union atyps *q1,union atyps *q2,int t);
int compare_objs(struct obj *o1,struct obj *o2,int t);
void simple_regs(void);
void load_simple_reg_parms(void);
void remove_IC_fg(struct flowgraph *g,struct IC *p);

extern int lastlabel;

struct flowgraph *new_flowgraph(void);
struct flowgraph *construct_flowgraph(void);
void print_av(bvtype *bitvector);
void print_rd(bvtype *bitvector);
void print_ae(bvtype *bitvector);
void print_cp(bvtype *bitvector);
void print_flowgraph(struct flowgraph *g);
void free_flowgraph(struct flowgraph *g);
void num_vars(void);
void print_vi(void);
void av_change(struct IC *p,bvtype *use,bvtype *def);
void av_update(struct IC *,bvtype *);
void active_vars(struct flowgraph *fg);
int dead_assignments(struct flowgraph *fg);
void insert_IC(struct IC *p,struct IC *new);
void insert_IC_fg(struct flowgraph *fg,struct IC *p,struct IC *new);
void insert_allocreg(struct flowgraph *fg,struct IC *p,int code,int reg);
void used_objects(struct Var *);
void used_clist(struct Typ *,struct const_list *);

extern struct Var *lregv[MAXR+1],*first_const,*last_const;
extern struct flowgraph *lfg;

extern int report_weird_code,report_suspicious_loops,pointer_call;

int replace_local_reg(struct obj *);
void local_regs(struct flowgraph *);
void local_combine(struct flowgraph *);
void create_const_vars(struct flowgraph *);
void free_const_vars(void);
void loop_regs(struct flowgraph *,int);
void block_regs(struct flowgraph *);
void insert_saves(struct flowgraph *);
struct flowgraph *jump_optimization(void);
void num_defs(void);
void reaching_definitions(struct flowgraph *);
void rd_change(struct IC *);
void calc(int c,int t,union atyps *q1,union atyps *q2,union atyps *z,struct IC *p);
int fold(struct IC *p);
int peephole(void);
int propagate(struct IC *p,struct obj *o,int replace,int global);
int constant_propagation(struct flowgraph *fg,int global);
int compare_exp(const void *a1,const void *a2);
void num_exp(void);
void available_expressions(struct flowgraph *fg);
void available_copies(struct flowgraph *fg);
int cse(struct flowgraph *fg,int global);
void num_copies(void);
int copy_propagation(struct flowgraph *fg,int global);
int loops(struct flowgraph *fg,int mode);
struct flowgraph *create_loop_headers(struct flowgraph *fg,int av);
struct flowgraph *create_loop_footers(struct flowgraph *fg,int av);
void insert_regs(struct flowgraph *fg);
void recalc_offsets(struct flowgraph *fg);
void optimize(long flags,struct Var *function);
int loop_optimizations(struct flowgraph *fg);
void ic_uses(struct IC *p,bvtype *result);
void ic_changes(struct IC *p,bvtype *result);
void create_alias(struct flowgraph *fg);
void free_alias(struct flowgraph *fg);
void update_alias(struct Var *old,struct Var *new);
void print_pt(bvtype **);
void free_pt(bvtype **);
#define CALC_CALLS 1
#define CALC_USES  2
#define CALC_CHANGES 4
void calc_finfo(struct Var *,int);

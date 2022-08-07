/*  $VER: vbcc (opt.h) $Revision: 1.4 $    */

#include "supp.h"

extern int gchanged;   /*  Merker, ob Optimierungslauf etwas geaendert hat */
extern int norek;      /*  diese Funktion wird nicht rekursiv auf          */
extern int nocall;     /*  diese Funktion kehrt nicht zum Caller zurueck   */
extern int inr;        /*  number of num_Vars iteration */

/*  temporary fuer verschiedene Bitvektoren */
extern bvtype *tmp;

/*  fuer aktive Variablen   */
extern Var **vilist;
extern unsigned int vcount;    /*  0..vcount-rcount-1: vars, vcount-rcount..vcount: DREFOBJs */
extern unsigned int rcount;
extern size_t vsize;
extern bvtype *av_globals,*av_address,*av_statics,*av_drefs;
extern int report_dead_statements;

/*  fuer verfuegbare Definitionen   */
extern unsigned int dcount;
extern size_t dsize;
extern IC **dlist;
extern bvtype **var_defs,**var_undefs;
extern bvtype **defs_kill,**defs_gen;
extern bvtype *rd_defs,*rd_tmp;
extern bvtype *rd_matrix;
#define UNDEF(x) (x+dcount)

/*  fuer verfuegbare Ausdruecke */
extern IC **elist;
extern unsigned int ecount;
extern size_t esize;
extern bvtype *ae_globals,*ae_address,*ae_statics,*ae_drefs;

/*  fuer verfuegbare Kopien */
extern unsigned int ccount;
extern size_t csize;
extern IC **clist;

/*  fuer frequency-reduction    */
extern bvtype *inloop,*invariant;

/*  alle Assignments, globaler oder Adr. fuer propagation etc.         */
extern bvtype *cp_globals,*cp_address,*cp_statics,*cp_drefs,*cp_act,*cp_dest;
/*  alle Kopieranweisungen, die eine best. Variable als Quelle haben    */
extern bvtype **copies;

extern int have_alias;
extern int static_cse,dref_cse;

typedef struct {
  union atyps min,max;
} range;

extern range *rangebuf;

typedef struct flowgraph{
  IC *start,*end;
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
  /* range-info */
  range *ra_normalout,*ra_branchout;
  /*  Registervariablen   */
  Var *regv[MAXR+1];
  /*  Merker, ob Register gebraucht wurde; MACR+1 Bits    */
  bvtype regused[RSIZE/sizeof(bvtype)];
#ifdef ALEX_REG
  int loop_depth; /* schleifentiefe des blocks. Wird nur fuer echte Schleifen gezaehlt. */
#endif
} flowgraph;

extern unsigned int basic_blocks;

typedef struct flowlist{
    flowgraph *graph;
    struct flowlist *next;
} flowlist;

int compare_const(union atyps *q1,union atyps *q2,int t);
int compare_objs(obj *o1,obj *o2,int t);
void simple_regs(void);
void load_simple_reg_parms(void);
void remove_IC_fg(flowgraph *g,IC *p);

extern int lastlabel;

flowgraph *new_flowgraph(void);
flowgraph *construct_flowgraph(void);
void print_av(bvtype *bitvector);
void print_rd(bvtype *bitvector);
void print_ae(bvtype *bitvector);
void print_cp(bvtype *bitvector);
void print_flowgraph(flowgraph *g);
void free_flowgraph(flowgraph *g);
void num_vars(void);
void print_vi(void);
void av_change(IC *p,bvtype *use,bvtype *def);
void av_update(IC *,bvtype *);
void active_vars(flowgraph *fg);
int dead_assignments(flowgraph *fg);
void insert_IC(IC *p,IC *new);
void insert_IC_fg(flowgraph *fg,IC *p,IC *new);
void insert_allocreg(flowgraph *fg,IC *p,int code,int reg);
void used_objects(Var *);
void used_clist(type *,const_list *);
void fix_shortop(flowgraph *,IC *);

extern Var *lregv[MAXR+1],*first_const,*last_const;
extern flowgraph *lfg;

extern int report_weird_code,report_suspicious_loops,pointer_call;

int replace_local_reg(obj *);
void local_regs(flowgraph *);
void local_combine(flowgraph *);
void create_const_vars(flowgraph *);
void free_const_vars(void);
void loop_regs(flowgraph *,int);
void block_regs(flowgraph *);
void insert_saves(flowgraph *);
flowgraph *jump_optimization(void);
void num_defs(void);
void reaching_definitions(flowgraph *);
void rd_change(IC *);
void calc(int c,int t,union atyps *q1,union atyps *q2,union atyps *z,IC *p);
int fold(IC *p);
int peephole(void);
int propagate(IC *p,obj *o,int replace,int global);
int constant_propagation(flowgraph *fg,int global);
int compare_exp(const void *a1,const void *a2);
void num_exp(void);
void available_expressions(flowgraph *fg);
void available_copies(flowgraph *fg);
int cse(flowgraph *fg,int global);
void num_copies(void);
int copy_propagation(flowgraph *fg,int global);
int loops(flowgraph *fg,int mode);
flowgraph *create_loop_headers(flowgraph *fg,int av);
flowgraph *create_loop_footers(flowgraph *fg,int av);
void insert_regs(flowgraph *fg);
void recalc_offsets(flowgraph *fg);
void optimize(long flags,Var *function);
int loop_optimizations(flowgraph *fg);
int decide_reverse(zmax);
void range_analysis(flowgraph *fg);
void ic_uses(IC *p,bvtype *result);
void ic_changes(IC *p,bvtype *result);
void create_alias(flowgraph *fg);
void free_alias(flowgraph *fg);
void update_alias(Var *old,Var *new);
void print_pt(bvtype **);
void free_pt(bvtype **);
#define CALC_CALLS 1
#define CALC_USES  2
#define CALC_CHANGES 4
void calc_finfo(Var *,int);

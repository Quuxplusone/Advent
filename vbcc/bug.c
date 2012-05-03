/*  $VER: vbcc (loop.c) V0.8     */
/*  schleifenorientierte Optimierungen  */

#include "opt.h"

static char FILE_[]=__FILE__;

#define MOVE_IC 1
#define MOVE_COMP 2

/*  Liste, in die ICs eingetragen werden, die aus Schleifen */
/*  gezogen werden sollen.                                  */
struct movlist{
  struct movlist *next;
  struct IC *IC;
  struct flowgraph *target_fg;
  int flags;
};

struct movlist *first_mov,*last_mov;

int report_weird_code,report_suspicious_loops;

/*  Bitvektoren fuer schleifeninvariante ICs    */
bvtype *invariant,*inloop,*moved,*moved_completely;
bvtype *fg_tmp;
bvtype *not_movable;
size_t bsize;


/*  Liste, in die ICs fuer strength-reduction eingetragen   */
/*  werden.                                                 */
struct srlist{
  struct srlist *next;
  struct IC *ind_var;
  struct IC *IC;
  struct flowgraph *target_fg;
  /*  Hilfsvariable, falls eine aequivalente Operation schon reduziert    */
  /*  wurde.                                                              */
  struct Var *hv;
};

struct srlist *first_sr,*last_sr;

/*  Liste, in die Daten fuer loop-unrolling eingetragen werden. */
struct urlist{
  int flags;
  long total,unroll;
  struct IC *cmp,*branch,*ind;
  struct flowgraph *start,*head;
  struct urlist *next;
} *first_ur;

#define UNROLL_COMPLETELY 1
#define UNROLL_MODULO 2
#define UNROLL_INVARIANT 4
#define UNROLL_REVERSE 8
#define IND_ONLY_COUNTS 16
#define MULTIPLE_EXITS 32

/*  Hier werden Induktionsvariablen vermerkt    */
struct IC **ind_vars;

static struct flowgraph *first_fg;


void calc_movable(struct flowgraph *start,struct flowgraph *end)
/*  Berechnet, welche Definitionen nicht aus der Schleife start-end     */
/*  verschoben werden duerfen. Eine Def. p von z darf nur verschoben    */
/*  werden, wenn keine andere Def. von p existiert und alle             */
/*  Verwendungen von z nur von p erreicht werden.                       */
/*  Benutzt rd_defs.                                                    */
{
  struct flowgraph *g;struct IC *p;
  int i,j,k,d;
  bvtype *changed_vars;
  if(DEBUG&1024) printf("calculating not_movable for blocks %d to %d\n",start->index,end->index);
  if(0/*!(optflags&1024)*/){
    memset(not_movable,UCHAR_MAX,dsize);
    return;
  }
  memset(not_movable,0,dsize);
  changed_vars=mymalloc(vsize);
  memset(changed_vars,0,vsize);
  for(i=0;i<vcount-rcount;i++){
    if(vilist[i]->vtyp->flags&VOLATILE) BSET(changed_vars,i);
    if(i<rcount){
      if(!vilist[i]->vtyp->next||(vilist[i]->vtyp->next->flags&VOLATILE)) BSET(changed_vars,i+vcount-rcount);
    }
  }
  for(g=start;g;g=g->normalout){
    if(!g->rd_in) ierror(0);
    memcpy(rd_defs,g->rd_in,dsize);
    for(p=g->start;p;p=p->next){
      for(j=0;j<p->change_cnt;j++){
	i=p->change_list[j].v->index;
	if(p->change_list[j].flags&DREFOBJ) i+=vcount-rcount;
	if(i>=vcount) continue;
	if(BTST(changed_vars,i)||(q1typ(p)&VOLATILE)||(q2typ(p)&VOLATILE)||(ztyp(p)&VOLATILE)){
	  bvunite(not_movable,var_defs[i],dsize);
	}else{
	  BSET(changed_vars,i);
	}
      }
      for(k=0;k<p->use_cnt;k++){
	i=p->use_list[k].v->index;
	if(p->use_list[k].flags&DREFOBJ) i+=vcount-rcount;
	if(i>=vcount) continue;
	for(d=-1,j=1;j<=dcount;j++){
	  if(BTST(rd_defs,j)&&BTST(var_defs[i],j)){
	    if(d>=0){  /*  mehr als eine Def.  */
	      bvunite(not_movable,var_defs[i],dsize);
	      d=-1;break;
	    }else d=j;
	  }
	  if(BTST(rd_defs,UNDEF(j))&&BTST(var_defs[i],UNDEF(j))){
	    bvunite(not_movable,var_defs[i],dsize);
	    d=-1;break;
	  }
	}
      }
      /*  Das hier, um rd_defs zu aktualisieren.  */
      rd_change(p);
      if(p==g->end) break;
    }
    if(g==end) break;
  }
  free(changed_vars);
}


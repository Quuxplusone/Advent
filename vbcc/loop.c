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

#ifdef ALEX_REG
void IncrementLoopDepth(struct flowgraph* fg,int start,int end)
     /* erhoeht loop_depth in den Bloecken [start,end] */
{
  if ( (fg->index >= start) && (fg->index <= end) )
    {
      fg->loop_depth++;
    }
  if (fg->normalout)
    IncrementLoopDepth(fg->normalout,start,end);
}
#endif

int loops(struct flowgraph *fg,int footers)
/*  kennzeichnet Schleifen im Flussgraph; wenn footers!=0 werden darf eine  */
/*  Schleife nur einen gemeinsamen Austrittspunkt haben                     */
{
  int i,start,end,c=0;struct flowlist *lp;struct flowgraph *g,*loopend;
  if(DEBUG&1024) printf("searching loops\n");
  g=fg;
  while(g){
    start=g->index;
    end=-1;
    for(lp=g->in;lp;lp=lp->next){
      if(!lp->graph) continue;
      if(lp->graph->branchout==g||!lp->graph->end||lp->graph->end->code!=BRA){
	i=lp->graph->index;
	if(i>=start&&i>end){ end=i;loopend=lp->graph; }
      }
    }
    if(end>=0){
      /*  Schleife oder etwas aehnliches  */
      struct flowgraph *p=g;
      if(DEBUG&1024) printf("found possible loop from blocks %d to %d\n",start,end);
      if(1/*goto_used*/){
	if(DEBUG&1024) printf("have to check...\n");
	do{
	  if(!p||p->index>end) break;

	  /*  testen, ob aus der Schleife gesprungen wird */
	  if(p->branchout&&footers){
	    i=p->branchout->index;
	    if(i<start){
	      end=-1;
	      break;
	    }
	    if(i>end&&(DEBUG&1024)){
	      puts("jump out of loop");
	      if(p->branchout!=loopend->normalout){
		puts("no break");
		if(p->branchout->start->typf!=return_label) puts("no return");
	      }
	    }
	    if(i>end&&p->branchout!=loopend->normalout&&p->branchout->start->typf!=return_label){
	      /*  Sprung zu anderem als dem normalen Austritt oder return */
	      end=-1;
	      break;
	    }
	  }
	  /*  testen, ob in die Schleife gesprungen wird  */
	  if(p!=g){
	    for(lp=p->in;lp;lp=lp->next){
	      if(!lp->graph) continue;
	      if(lp->graph->branchout==p){
		i=lp->graph->index;
		if(i<start){
		  if(report_weird_code){error(175);report_weird_code=0;}
		  end=-1;
		  break;
		}
		if(i>end){
		  end=-1;
		  break;
		}
	      }
	    }
	  }
	  if(p->index==end) break;
	  p=p->normalout;
	}while(end>=0);
      }else{
	if(DEBUG&1024) printf("must be a loop, because there was no goto\n");
      }
    }
    if(end>=0){
      if(DEBUG&1024) printf("confirmed that it is a loop\n");
      g->loopend=loopend;
      c++;
#ifdef ALEX_REG
      IncrementLoopDepth(fg,start,end);
#endif
    }
    g=g->normalout;
  }
  return c;
}

struct flowgraph *create_loop_headers(struct flowgraph *fg,int av)
/*  Fuegt vor jede Schleife einen Kopf-Block ein, wenn noetig.  */
/*  Wenn av!=0 werden aktive Variablen korrekt uebertragen und  */
/*  diverse Registerlisten uebernommen und index=-1 gesetzt.    */
/*  Kann einen Block mehrmals in der ->in Liste eintragen       */
{
  struct flowgraph *g,*last,*new,*rg=fg;
  struct IC *lic,*lastic;
  if(DEBUG&1024) printf("creating loop-headers\n");
  g=fg;last=0;lastic=0;
  while(g){
    new=0;
    if(g->loopend){
      if(!last){
	struct flowlist *lp;
	new=new_flowgraph();
	rg=new;
	new->in=0;
	lic=new_IC();
	lic->code=LABEL;
	lic->typf=++label;
	lic->q1.flags=lic->q2.flags=lic->z.flags=0;
	lic->q1.am=lic->q2.am=lic->z.am=0;
	new->start=new->end=lic;
	lic->next=first_ic;
	lic->prev=0;
	first_ic->prev=lic;
	first_ic=lic;
	lp=mymalloc(sizeof(struct flowlist));
	lp->graph=new;
	lp->next=g->in;
	g->in=lp;
      }else{
	struct flowlist *lp,*nl,**ls;
	new=new_flowgraph();
	last->normalout=new;
	lic=new_IC();
	new->start=new->end=lic;
	lic->code=LABEL;
	lic->typf=++label;
	lic->q1.flags=lic->q2.flags=lic->z.flags=0;
	lic->q1.am=lic->q2.am=lic->z.am=0;
	if(lastic) lastic->next=lic;
	else   first_ic=lic;
	lic->prev=lastic;
	g->start->prev=lic;
	lic->next=g->start;
	lp=g->in;ls=&new->in;
	while(lp){
	  if(lp->graph&&lp->graph->index<g->index){
	    /*  Eintritt von oben soll in den Kopf  */
	    nl=mymalloc(sizeof(struct flowlist));
	    nl->graph=lp->graph;
	    nl->next=0;
	    (*ls)=nl;
	    ls=&nl->next;
	    if(lp->graph->branchout==g){
	      struct IC *p=lp->graph->end;
	      if(DEBUG&1024) printf("changing branch\n");
	      while(p&&p->code==FREEREG) p=p->prev;
	      if(!p||p->code<BEQ||p->code>BRA) ierror(0);
	      p->typf=lic->typf;
	      lp->graph->branchout=new;
	    }
	    lp->graph=new;
	  }
	  lp=lp->next;
	}
	if(!new->in) ierror(0);
      }
      if(new){
	if(DEBUG&1024) printf("must insert loop-header before block %d\n",g->index);
	basic_blocks++;
	new->branchout=0;
	new->loopend=0;
	if(av)
	  new->index=-1;
	else
	  new->index=basic_blocks;
	new->normalout=g;
	new->calls=0;
	new->loop_calls=0;
	new->rd_in=new->rd_out=new->rd_kill=new->rd_gen=0;
	new->ae_in=new->ae_out=new->ae_kill=new->ae_gen=0;
	new->cp_in=new->cp_out=new->cp_kill=new->cp_gen=0;
	if(!av){
	  new->av_in=new->av_out=new->av_kill=new->av_gen=0;
	}else{
	  new->av_in=mymalloc(vsize);
	  new->av_out=mymalloc(vsize);
	  new->av_gen=mymalloc(vsize);
	  new->av_kill=mymalloc(vsize);
	  memset(new->av_gen,0,vsize);
	  memset(new->av_kill,0,vsize);
	  memcpy(new->av_out,g->av_in,vsize);
	  memcpy(new->av_in,g->av_in,vsize);
	  memcpy(&new->regv,&g->regv,sizeof(new->regv));
	  memcpy(&new->regused,&g->regused,sizeof(new->regused));
	}
      }
    }
    last=g;if(last->end) lastic=last->end;
    g=g->normalout;
  }
  return rg;
}
struct flowgraph *create_loop_footers(struct flowgraph *fg,int av)
/*  Fuegt hinter jede Schleife einen Fuss-Block ein, wenn noetig.   */
/*  Wenn av!=0 werden aktive Variablen korrekt uebertragen und      */
/*  diverse Registerlisten uebernommen und index auf -2 gesetzt.    */
{
  struct flowgraph *g,*loopend,*out,*new;
  struct IC *lic;
  if(DEBUG&1024) printf("creating loop-footers\n");
  g=fg;
  while(g){
    new=0;
    loopend=g->loopend;
    if(loopend){
      struct flowlist *lp,*nl,**ls;
      out=loopend->normalout;
      new=new_flowgraph();
      new->normalout=out;
      loopend->normalout=new;
      lic=new_IC();
      lic->line=0;
      lic->file=0;
      new->start=new->end=lic;
      lic->code=LABEL;
      lic->typf=++label;
      lic->q1.flags=lic->q2.flags=lic->z.flags=0;
      lic->q1.am=lic->q2.am=lic->z.am=0;
      lic->use_cnt=lic->change_cnt=0;
      lic->use_list=lic->change_list=0;
      if(out) lp=out->in; else {lp=0;new->in=0;}
      ls=&new->in;
      while(lp){
	if(lp->graph&&lp->graph->index<=loopend->index&&lp->graph->index>=g->index){
	  /*  Austritt aus Schleife soll in den Fuss  */
	  nl=mymalloc(sizeof(struct flowlist));
	  nl->graph=lp->graph;
	  nl->next=0;
	  (*ls)=nl;
	  ls=&nl->next;
	  if(lp->graph->branchout==out){
	    struct IC *p=lp->graph->end;
	    if(DEBUG&1024) printf("changing branch\n");
	    while(p&&p->code==FREEREG) p=p->prev;
	    if(!p||p->code<BEQ||p->code>BRA) ierror(0);
	    p->typf=lic->typf;
	    lp->graph->branchout=new;
	  }
	  lp->graph=new;
	}
	lp=lp->next;
      }
      if(out&&!new->in) ierror(0);
      if(DEBUG&1024) printf("must insert loop-footer after block %d\n",loopend->index);
      basic_blocks++;
      new->branchout=0;
      new->loopend=0;
      if(av)
	new->index=-2;
      else
	new->index=basic_blocks;
      new->normalout=out;
      new->calls=0;
      new->loop_calls=0;
      new->rd_in=new->rd_out=new->rd_kill=new->rd_gen=0;
      new->ae_in=new->ae_out=new->ae_kill=new->ae_gen=0;
      new->cp_in=new->cp_out=new->cp_kill=new->cp_gen=0;
      if(!av){
	new->av_in=new->av_out=new->av_kill=new->av_gen=0;
      }else{
	new->av_in=mymalloc(vsize);
	new->av_out=mymalloc(vsize);
	new->av_kill=mymalloc(vsize);
	new->av_gen=mymalloc(vsize);
	memset(new->av_gen,0,vsize);
	memset(new->av_kill,0,vsize);
	if(out){
	  memcpy(new->av_out,out->av_in,vsize);
	  memcpy(new->av_in,out->av_in,vsize);
	}else{
	  memcpy(new->av_out,av_globals,vsize);
	  bvunite(new->av_out,av_statics,vsize);
	  memcpy(new->av_in,new->av_out,vsize);
	}
	memcpy(&new->regv,&g->regv,sizeof(new->regv));
	memcpy(&new->regused,&g->regused,sizeof(new->regused));
      }
      insert_IC_fg(new,loopend->end,lic);
    }
    g=g->normalout;
  }
  return fg;
}
void add_movable(struct IC *p,struct flowgraph *fg,int flags)
/*  Fuegt IC p, das aus der Schleife in Block fg mit Flags flags    */
/*  verschoben werden darf in eine Liste.                           */
{
  struct movlist *new=mymalloc(sizeof(*new));
  new->IC=p;
  new->target_fg=fg;
  new->flags=flags;
  new->next=0;
  if(last_mov){
    last_mov->next=new;
    last_mov=new;
  }else{
    first_mov=last_mov=new;
  }
  BSET(moved,p->defindex);
  if(flags==MOVE_IC) BSET(moved_completely,p->defindex);
}
int move_to_head(void)
/*  Geht die Liste mit verschiebbaren ICs durch und schiebt die ICs */
/*  in den Vorkopf der Schleife. Ausserdem wird die Liste           */
/*  freigegeben.                                                    */
/*  Der Rueckgabewert hat Bit 1 gesetzt, wenn ICs ganz verschoben   */
/*  wurden und Bit 2, falls eine Berechnung mit Hilfsvariable vor   */
/*  die Schleife gezogen wurde.                                     */
{
  struct IC **fglist; /* Letztes IC vor jedem Block   */
  struct flowgraph *g;struct IC *p;struct movlist *m;
  int changed=0;

  if(!first_mov) return 0;

  if(DEBUG&1024) printf("moving the ICs out of the loop\n");

  fglist=mymalloc((basic_blocks+1)*sizeof(*fglist));
  p=0;
  for(g=first_fg;g;g=g->normalout){
    if(g->index>basic_blocks) ierror(0);
    if(g->end) p=g->end;
    fglist[g->index]=p;
  }
  while(first_mov){
    p=first_mov->IC;
    g=first_mov->target_fg;
    if(first_mov->flags==MOVE_IC){
      if(DEBUG&1024) {printf("moving IC out of loop:\n");pric2(stdout,p);}
      if(!p->prev||!p->next) ierror(0);
      p->next->prev=p->prev;
      p->prev->next=p->next;
      insert_IC_fg(g,fglist[g->index],p);
      fglist[g->index]=p;
      changed|=1;
    }else if(1){
      struct Typ *t=new_typ();
      struct IC *new=new_IC();
      struct Var *v;
      if(DEBUG&1024) {printf("moving computation out of loop:\n");pric2(stdout,p);}
      t->flags=ztyp(p);
      if(p->code==COMPARE||p->code==TEST) t->flags=0;
      if(ISPOINTER(t->flags)){
	t->next=new_typ();
	t->next->flags=VOID;
      }
      v=add_tmp_var(t);
      *new=*p;
      new->z.flags=VAR;
      new->z.v=v;
      new->z.val.vmax=l2zm(0L);

      /*  Die neue Operation benutzt maximal, was die andere benutzte */
      /*  und aendert nur die Hilfsvariable.                          */
      if(have_alias){
	new->use_cnt=p->use_cnt;
	new->use_list=mymalloc(new->use_cnt*VLS);
	memcpy(new->use_list,p->use_list,new->use_cnt*VLS);
	new->change_cnt=1;
	new->change_list=mymalloc(VLS);
	new->change_list[0].v=v;
	new->change_list[0].flags=0;
      }
      insert_IC_fg(g,fglist[g->index],new);
      fglist[g->index]=new;
      p->code=ASSIGN;
      p->typf=t->flags;
      p->q1.flags=VAR;
      p->q1.v=v;
      p->q1.val.vmax=l2zm(0L);
      p->q2.flags=0;
      p->q2.val.vmax=szof(t);
      /*  Die Operation in der Schleife benutzt nun zusaetzlich   */
      /*  noch die Hilfsvariable.                                 */
      if(have_alias){
	void *m=p->use_list;
	p->use_cnt++;
	p->use_list=mymalloc(p->use_cnt*VLS);
	memcpy(&p->use_list[1],m,(p->use_cnt-1)*VLS);
	free(m);
	p->use_list[0].v=v;
	p->use_list[0].flags=0;
      }
      changed|=2;
    }
    m=first_mov->next;
    free(first_mov);
    first_mov=m;
  }
  if(DEBUG&1024) print_flowgraph(first_fg);
  free(fglist);
  return changed;
}
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
	if(BTST(changed_vars,i)||is_volatile_ic(p)){
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
/*  Testet, ob Variable nur in der Schleife benutzt wird.               */
/* could be improved */
int used_in_loop_only(struct flowgraph *start,struct flowgraph *end,struct obj *o)
{
  struct Var *v;struct flowgraph *g;struct IC *p;
  if((o->flags&(VAR|DREFOBJ))!=VAR) return 0;
  v=o->v;
  if((v->flags&USEDASADR)||v->nesting==0||v->storage_class==EXTERN||v->storage_class==STATIC)
    return 0;
  for(g=first_fg;g;g=g->normalout){
    if(g==start) g=end->normalout;
    if(!g) break;
    for(p=g->start;p;p=p->next){
      if((p->q1.flags&VAR)&&p->q1.v==v) return 0;
      if((p->q2.flags&VAR)&&p->q2.v==v) return 0;
      if((p->z.flags&VAR)&&p->z.v==v) return 0;
      if(p==g->end) break;
    }
    if(g==end) break;
  }
  return 1;
}

/*  Testet, ob z immer ausgefuehrt wird, falls start in fg ausgefuehrt  */
/*  wird. fg_tmp ist ein Bitvektor, um zu merken, welche Bloecke sicher */
/*  zu z fuehren. Das ganze fuer die Schleife start-end.                */
/*  Wenn ignorecall!=0 ist, wird angenommen, dass jeder CALL            */
/*  zurueckkehrt (das ist nuetzlich fuer loop-unrolling).               */
int always_reached(struct flowgraph *start,struct flowgraph *end,struct flowgraph *fg,struct IC *z,int ignorecall)
{
  bvtype *bmk=fg_tmp;
  struct IC *p;struct flowgraph *g;
  int changed;
  
  for(p=z;p;p=p->prev){
    if(!ignorecall&&p->code==CALL) return 0;
    if(p==fg->start) break;
  }
  
  if(fg==start) return 1;
  
  memset(bmk,0,bsize);
  BSET(bmk,fg->index);
  
  do{
    changed=0;
    for(g=start;g;g=g->normalout){
      if(!BTST(bmk,g->index)){
	struct flowgraph *n=g->normalout;
	struct flowgraph *b=g->branchout;
	if(n||b){
	  if((!b||BTST(bmk,b->index))&&
	     (!n||(g->end&&g->end->code==BRA)||BTST(bmk,n->index))){
	    for(p=g->end;p;p=p->prev){
	      if(!ignorecall&&p->code==CALL) break;
	      if(p==g->start){
		if(g==start) return 1;
		changed=1; BSET(bmk,g->index);
		break;
	      }
	    }
	  }
	}
      }
      if(g==end) break;
    }
  }while(changed);
  return 0;
}

/*  Ermittelt, ob Variable vindex schleifeninvariant unter den Bedingungen  */
/*  rd_defs, inloop und invariant ist.                                      */
/*  Definition ignore wird nicht beachtet. Wenn ignore auf eine gueltige    */
/*  Definition gesetzt wird, kann man somit auf Induktionsvariablen testen  */
/*  (das Ergebnis sagt dann, ob das die einzige Definition in der Schleife  */
/*  ist).                                                                   */
int def_invariant(int vindex,int ignore)
{
  int i,k,j,d=0;
  /*printf("def_inv(%d)=%s(%ld)\n",vindex,vilist[vindex]->identifier,zm2l(vilist[vindex]->offset));*/
  for(j=1;j<=dcount;j++){
    if(j!=ignore&&BTST(rd_defs,j)){
      if(BTST(var_defs[vindex],j)&&BTST(inloop,j)){
	/*  Mehr als eine moegliche Def. innerhalb der Schleife oder    */
	/*  eine invariante Def. in der Schleife => nicht invariant.    */
	if(d) return 0;
	if(!BTST(moved_completely,j)) return 0;
	d=1;
      }
    }  
    if(BTST(rd_defs,UNDEF(j))){
      if(BTST(var_defs[vindex],UNDEF(j))&&BTST(inloop,UNDEF(j))){
	/*  Mehr als eine moegliche Def. innerhalb der Schleife oder    */
	/*  eine invariante Def. in der Schleife => nicht invariant.    */
	if(d) return 0;
	if(!BTST(moved_completely,UNDEF(j))) return 0;
	d=1;
      }
    }      
  }
  return 1;
#if 0
  if(!BTST(rd_defs,vindex+dcount+1)){
    memcpy(rd_tmp,rd_defs,dsize);
    bvintersect(rd_tmp,var_defs[vindex],dsize);
    for(j=1;j<=dcount;j++){
      if(j!=ignore&&BTST(rd_tmp,j)&&BTST(inloop,j)){
	/*  Mehr als eine moegliche Def. innerhalb der Schleife oder    */
	/*  eine invariante Def. in der Schleife => nicht invariant.    */
	if(d) return 0;
	if(!BTST(moved_completely,j)) return 0;
	d=1;
      }
    }
  }else{
    for(j=1;j<=dcount;j++){
      if(j!=ignore&&BTST(rd_defs,j)&&BTST(inloop,j)){
	struct IC *p=dlist[j];
	for(k=0;k<p->change_cnt;k++){
	  i=p->change_list[k].v->index;
	  if(p->change_list[k].flags&DREFOBJ) i+=vcount-rcount;
	  if(i==vindex) break;
	}
	if(k>=p->change_cnt) continue;
	/*  Mehr als eine moegliche Def. innerhalb der Schleife oder    */
	/*  eine invariante Def. in der Schleife => nicht invariant.    */
	if(d) return 0;
	if(!BTST(moved_completely,j)) return 0;
	d=1;
      }
    }
  }
  return 1;
#endif
}

void frequency_reduction(struct flowgraph *start,struct flowgraph *end,struct flowgraph *head)
/*  Schleifeninvariante ICs finden und in eine Liste eintragen, falls   */
/*  sie vor die Schleife gezogen werden koennen.                        */
{
  struct IC *p;struct flowgraph *g;
  int i,changed;

  if(head&&start->loopend){
    end=start->loopend;

    if(DEBUG&1024){
      printf("searching for loop-invariant code in loop from block %d to %d\n",start->index,end->index);
      printf("head_fg=%d\n",head->index);
    }
    calc_movable(start,end);
    /*  erstmal kein IC invariant   */
    memset(invariant,0,dsize);

    /*  kennzeichnen, welche ICs in der Schleife liegen */
    memset(inloop,0,dsize);
    for(g=start;g;g=g->normalout){
      for(p=g->start;p;p=p->next){
	if(p->defindex) BSET(inloop,p->defindex);
	if(p==g->end) break;
      }
      if(g==end) break;
    }

    do{
      changed=0;
      if(DEBUG&1024) printf("loop-invariant pass\n");

      /*  Schleifeninvariante ICs suchen  */

      for(g=start;g;g=g->normalout){
	memcpy(rd_defs,g->rd_in,dsize);
	for(p=g->start;p;p=p->next){
	  int k1,k2;
	  /*  testen, ob IC neu als invariant markiert werden kann    */
	  if(p->defindex&&p->code!=CALL&&p->code!=GETRETURN&&!BTST(invariant,p->defindex)){
	    if(!BTST(inloop,p->defindex)) ierror(0);
	    if(p->code==ADDRESS||!p->q1.flags||(p->q1.flags&(KONST|DREFOBJ))==KONST||(p->q1.flags&VARADR)){
	      k1=1;
	    }else{
	      if(!(p->q1.flags&VAR)){
		k1=0;
	      }else{
		i=p->q1.v->index;
		if(p->q1.flags&DREFOBJ){
		  i+=vcount-rcount;
		  if(p->q1.dtyp&VOLATILE)
		    k1=0;
		  else
		    k1=def_invariant(i,-1);
		}else
		  k1=def_invariant(i,-1);
	      }
	    }
	    if(k1){
	      if(!p->q2.flags||(p->q2.flags&(KONST|DREFOBJ))==KONST||(p->q2.flags&VARADR)){
		k2=1;
	      }else{
		if(!(p->q2.flags&VAR)){
		  k2=0;
		}else{
		  i=p->q2.v->index;
		  if(p->q2.flags&DREFOBJ){
		    i+=vcount-rcount;
		    if(p->q2.dtyp&VOLATILE)
		      k2=0;
		    else
		      k2=def_invariant(i,-1);
		  }else
		    k2=def_invariant(i,-1);
		}
	      }
	    }
	    if(k1&&k2&&!(ztyp(p)&VOLATILE)&&!(q1typ(p)&VOLATILE)){
	      if(DEBUG&1024){ printf("found loop-invariant IC:\n");pric2(stdout,p);}
	      if(!BTST(moved,p->defindex)&&(always_reached(start,end,g,p,0)||(!dangerous_IC(p)&&used_in_loop_only(start,end,&p->z)))){
		if(p->z.flags&DREFOBJ){
		  if(p->z.flags&KONST)
		    k1=0;
		  else
		    k1=def_invariant(p->z.v->index,-1);
		}else
		  k1=1;
		/*if(DEBUG&1024) printf("always reached or used only in loop\n");*/
		if(k1&&!BTST(not_movable,p->defindex)&&!(ztyp(p)&VOLATILE)&&(!(p->z.flags&VAR)||!p->z.v->reg)){
		  /*if(DEBUG&1024) printf("movable\n");*/
		  add_movable(p,head,MOVE_IC);
		}else{
		  if(p->code==ADDRESS||(ISSCALAR(p->typf)&&(p->q2.flags||(p->q1.flags&DREFOBJ)))){
		    /*if(DEBUG&1024) printf("move computation out of loop\n");*/
		    if(!(disable&256))
		      add_movable(p,head,MOVE_COMP);
		  }
		}
	      }else{
		/*  Wenn IC immer erreicht wird oder ungefaehrlich  */
		/*  ist, kann man zumindest die Operation           */
		/*  rausziehen, falls das lohnt.                    */
		if(!BTST(moved,p->defindex)&&(!dangerous_IC(p)&&ISSCALAR(p->typf)&&(p->q2.flags||(p->q1.flags&DREFOBJ)||p->code==ADDRESS))){
		  /*if(DEBUG&1024) printf("move computation out of loop\n");*/
		  if(!(disable&256))
		    add_movable(p,head,MOVE_COMP);
		}
	      }
	      BSET(invariant,p->defindex);
	      changed=1;
	    }
	  }

	  /*  Das hier, um rd_defs zu aktualisieren.  */
	  rd_change(p);

	  if(p==g->end) break;
	}
	if(g==end) break;
      }
    }while(changed);

  }
  return;
}
void add_sr(struct IC *p,struct flowgraph *fg,int i_var)
/*  Fuegt IC p, das aus der Schleife in Block fg lineare Fkt. der   */
/*  Induktionsvariable i_var ist, in Liste ein.                     */
/*  Funktioniert als Stack. Da mit aeusseren Schleifen angefangen   */
/*  wird, werden ICs zuerst in inneren Schleifen reduziert. Da ein  */
/*  IC nur einmal reduziert wird, sollte dadurch das Problem eines  */
/*  ICs, das potentiell in mehreren Schleifen reduziert werden      */
/*  koennte, geloest werden.                                        */
{
  struct srlist *new=mymalloc(sizeof(*new));
  if(DEBUG&1024) printf("all:%p\n",(void*)new);
  new->IC=p;
  new->target_fg=fg;
  new->ind_var=ind_vars[i_var];
  new->next=first_sr;
  new->hv=0;
  first_sr=new;
#if 0
  if(last_sr){
    last_sr->next=new;
    last_sr=new;
  }else{
    first_sr=last_sr=new;
  }
#endif
}
int do_sr(void)
/*  Durchlaufe die Liste aller strength-reduction-Kandidaten und    */
/*  ersetze sie durch neue Induktionsvariablen. Dabei aufpassen,    */
/*  falls ein IC schon von frequency-reduction bearbeitet wurde.    */
/*  Ausserdem wird die Liste freigegeben.                           */
{
  struct IC **fglist; /* Letztes IC vor jedem Block   */
  struct IC *p;
  struct flowgraph *g;
  struct srlist *mf;
  int changed=0;

  if(!first_sr) return 0;

  if(DEBUG&1024) printf("performing strength-reductions\n");

  fglist=mymalloc((basic_blocks+1)*sizeof(*fglist));
  p=0;
  for(g=first_fg;g;g=g->normalout){
    if(g->index>basic_blocks) ierror(0);
    if(g->end) p=g->end;
    fglist[g->index]=p;
  }

  while(first_sr){
    struct Var *niv,*nstep;
    struct Typ *t1,*t2;
    struct IC *iv_ic,*new,*m;
    int i,c;
    p=first_sr->IC;
    i=p->defindex;
    /*  Falls IC noch nicht verschoben und noch nicht reduziert wurde.  */
    if(!BTST(moved,i)&&p->code!=ASSIGN){
      if(first_sr->hv){
	/*  Es wurde schon eine aequivalente Operation reduziert, wir   */
	/*  koennen also dieselbe Hilfsvariable benutzen.               */
	p->code=ASSIGN;
	p->q1.flags=VAR;
	p->q1.v=first_sr->hv;
	p->q1.val.vmax=l2zm(0L);
	p->q2.flags=0;
	p->q2.val.vmax=szof(p->z.v->vtyp);
        p->typf=p->z.v->vtyp->flags;
	/*  Hilfsvariable wird jetzt auch benutzt.  */
	if(have_alias){
	  void *m=p->use_list;
	  p->use_cnt++;
	  p->use_list=mymalloc(p->use_cnt*VLS);
	  memcpy(&p->use_list[1],m,(p->use_cnt-1)*VLS);
	  free(m);
	  p->use_list[0].v=first_sr->hv;
	  p->use_list[0].flags=0;
	}
      }else{
	int minus=0;
	if(DEBUG&1024){ printf("performing strength-reduction on IC:\n");pric2(stdout,p);}
	c=p->code;
	g=first_sr->target_fg;
	iv_ic=first_sr->ind_var;
	/*  Merken, wenn IC von der Form SUB x,ind_var->z   */
	if((c==SUB||c==SUBIFP)&&!compare_objs(&p->q2,&iv_ic->z,iv_ic->typf))
	  minus=1;
	t1=new_typ();
	t1->flags=p->typf;
	if(c==ADDI2P||c==SUBIFP){
	  t1->flags=p->typf2;
	  t1->next=new_typ();
	  t1->next->flags=VOID;
	}
	niv=add_tmp_var(t1);
	/*  Suchen, ob es noch aequivalente Operationen gibt.   */
	/*  Noch sehr ineffizient...                            */
	for(mf=first_sr->next;mf;mf=mf->next){
	  if(mf->target_fg==g&&mf->ind_var==iv_ic){
	    m=mf->IC;
	    if(c==m->code&&p->typf==m->typf&&
	       !compare_objs(&p->q1,&m->q1,p->typf)&&
	       !compare_objs(&p->q2,&m->q2,p->typf)){
	      if(mf->hv) ierror(0);
	      mf->hv=niv;
	      if(DEBUG&1024){ printf("equivalent operation\n");pric2(stdout,m);}
	    }
	  }
	}
	/*  Initialisierung der Hilfsinduktionsvariablen    */
	new=new_IC();
	*new=*p;
	new->z.flags=VAR;
	new->z.v=niv;
	new->z.val.vmax=l2zm(0L);
	/*  IC benutzt dasselbe wie p und aendert nur niv.  */
	if(have_alias){
	  new->change_cnt=1;
	  new->change_list=mymalloc(VLS);
	  new->change_list[0].v=niv;
	  new->change_list[0].flags=0;
	  new->use_cnt=p->use_cnt;
	  new->use_list=mymalloc(new->use_cnt*VLS);
	  memcpy(new->use_list,p->use_list,new->use_cnt*VLS);
	}
	insert_IC_fg(g,fglist[g->index],new);
	fglist[g->index]=m=new;
	/*  Ersetzen der Operation durch die Hilfsvariable  */
	p->code=ASSIGN;
	p->typf=t1->flags;
	p->q1=m->z;
	p->q2.flags=0;
	p->q2.val.vmax=szof(t1);
	/*  Benutzt jetzt auch Hilfsvariable.               */
	if(have_alias){
	  void *mr=p->use_list;
	  p->use_cnt++;
	  p->use_list=mymalloc(p->use_cnt*VLS);
	  memcpy(&p->use_list[1],mr,(p->use_cnt-1)*VLS);
	  free(mr);
	  p->use_list[0].v=niv;
	  p->use_list[0].flags=0;
	}
	/*  Berechnen der Schrittweite fuer Hilfsvariable   */
	if(c==MULT){
	  t2=new_typ();
	  t2->flags=iv_ic->typf;
	  nstep=add_tmp_var(t2);
	  new=new_IC();
	  new->line=iv_ic->line;
	  new->file=iv_ic->file;
	  new->code=MULT;
	  new->typf=p->typf;
	  new->z.flags=VAR;
	  new->z.v=nstep;
	  new->z.val.vmax=l2zm(0L);
	  if(!compare_objs(&m->q1,&iv_ic->z,iv_ic->typf)) new->q1=m->q2;
	  else new->q1=m->q1;
	  if(!compare_objs(&iv_ic->q1,&iv_ic->z,iv_ic->typf)) new->q2=iv_ic->q2;
	  else new->q2=iv_ic->q1;
	  /*  Benutzt dasselbe wie iv_ic und m.   */
	  if(have_alias){
	    new->use_cnt=iv_ic->use_cnt+m->use_cnt;
	    new->use_list=mymalloc(new->use_cnt*VLS);
	    memcpy(new->use_list,iv_ic->use_list,iv_ic->use_cnt*VLS);
	    memcpy(&new->use_list[iv_ic->use_cnt],m->use_list,m->use_cnt*VLS);
	    new->change_cnt=1;
	    new->change_list=mymalloc(VLS);
	    new->change_list[0].v=nstep;
	    new->change_list[0].flags=0;
	  }
	  insert_IC_fg(g,fglist[g->index],new);
	  fglist[g->index]=m=new;
	}
	/*  Erhoehen der Hilfsvariable um Schrittweite      */
	new=new_IC();
	new->line=iv_ic->line;
	new->file=iv_ic->file;

	new->code=iv_ic->code;
	if(minus){
	  switch(new->code){
	  case ADD:     new->code=SUB; break;
	  case SUB:     new->code=ADD; break;
	  case ADDI2P:  new->code=SUBIFP; break;
	  case SUBIFP:  new->code=ADDI2P; break;
	  }
	}
	if(ISPOINTER(t1->flags)){
	  if(new->code==ADD) new->code=ADDI2P;
	  if(new->code==SUB) new->code=SUBIFP;
	  new->typf=t1->flags;
	}
	new->typf=iv_ic->typf;
	new->q1.flags=VAR;
	new->q1.v=niv;
	new->typf2=niv->vtyp->flags;
	new->q1.val.vmax=l2zm(0L);
	new->z=new->q1;
	if(c==MULT){
	  new->q2=m->z;
	}else{
	  if(!compare_objs(&iv_ic->q1,&iv_ic->z,iv_ic->typf)) new->q2=iv_ic->q2;
	  else new->q2=iv_ic->q1;
	}
	if(have_alias){
	  new->use_cnt=iv_ic->use_cnt+m->use_cnt;
	  new->use_list=mymalloc(new->use_cnt*VLS);
	  memcpy(new->use_list,iv_ic->use_list,iv_ic->use_cnt*VLS);
	  memcpy(&new->use_list[iv_ic->use_cnt],m->use_list,m->use_cnt*VLS);
	  new->change_cnt=1;
	  new->change_list=mymalloc(VLS);
	  new->change_list[0].v=niv;
	  new->change_list[0].flags=0;
	}
	/*  Flussgraph muss nur bei den Schleifenkoepfen ok sein.   */
	insert_IC(iv_ic,new);
	changed|=2;
      }
    }

    mf=first_sr->next;
    free(first_sr);
    first_sr=mf;
  }
  free(fglist);
  return changed;
}
void strength_reduction(struct flowgraph *start,struct flowgraph *end,struct flowgraph *head)
/*  Ersetzen von Operationen mit einer Induktionsvariablen und einem    */
/*  schleifeninvarianten Operanden durch eine zusaetzliche              */
/*  Hilfsinduktionsvariable.                                            */
{
  struct flowgraph *g;struct IC *p;
  int i;
  if(DEBUG&1024) printf("performing strength_reduction on blocks %d to %d\n",start->index,end->index);
  for(i=0;i<vcount;i++) ind_vars[i]=0;
  /*  Nach Induktionsvariablen suchen.    */
  for(g=start;g;g=g->normalout){
    memcpy(rd_defs,g->rd_in,dsize);
    for(p=g->start;p;p=p->next){
      int c=p->code;
      if(c==ADD||c==ADDI2P||c==SUB||c==SUBIFP){
	/* TODO: what is possible/useful with floating point induction variables? */
	if(!compare_objs(&p->q1,&p->z,p->typf)&&!ISFLOAT(p->typf)){
	  if(DEBUG&1024){printf("possible induction:\n");pric2(stdout,p);}
	  if(p->q2.flags&VAR){
	    i=p->q2.v->index;
	    if(p->q2.flags&DREFOBJ) i+=vcount-rcount;
	  }
	  if((p->q2.flags&(VAR|VARADR))!=VAR||def_invariant(i,-1)){
	    i=p->z.v->index;
	    if(p->z.flags&DREFOBJ) i+=vcount-rcount;
	    if(def_invariant(i,p->defindex)){
	      if(DEBUG&1024) {printf("found basic induction var:\n");pric2(stdout,p);}
	      ind_vars[i]=p;
	    }
	  }
	}
	if(USEQ2ASZ&&c!=SUB&&c!=SUBIFP&&!compare_objs(&p->q2,&p->z,p->typf)){
	  if(DEBUG&1024){printf("possible induction:\n");pric2(stdout,p);}
	  if(p->q1.flags&VAR){
	    i=p->q1.v->index;
	    if(p->q1.flags&DREFOBJ) i+=vcount-rcount;
	  }
	  if((p->q1.flags&(VAR|VARADR))!=VAR||def_invariant(i,-1)){
	    i=p->z.v->index;
	    if(p->z.flags&DREFOBJ) i+=vcount-rcount;
	    if(def_invariant(i,p->defindex)){
	      if(DEBUG&1024) {printf("found basic induction var:\n");pric2(stdout,p);}
	      ind_vars[i]=p;
	    }
	  }
	}
      }

      /*  Das hier, um rd_defs zu aktualisieren.  */
      rd_change(p);

      if(p==g->end) break;
    }
    if(g==end) break;
  }

  if(disable&1024) return;

  /*  Nach reduzierbaren Operationen suchen   */
  for(g=start;g;g=g->normalout){
    memcpy(rd_defs,g->rd_in,dsize);
    for(p=g->start;p;p=p->next){
      if((p->code==MULT||((p->code==ADD||p->code==SUB||p->code==ADDI2P||p->code==SUBIFP)&&!(disable&512)))&&
	 ((!ISFLOAT(p->typf))||fp_assoc)&&!(p->flags&EFF_IC) ){
	int k1,k2,iv;
	if((p->q1.flags&(VAR|VARADR))==VAR){
	  i=p->q1.v->index;
	  if(p->q1.flags&DREFOBJ) i+=vcount-rcount;
	  if(ind_vars[i]){
	    k1=1;iv=i;
	  }else if(def_invariant(i,-1))
	    k1=2;
	  else
	    k1=0;
	}else
	  k1=2;
	if((p->q2.flags&(VAR|VARADR))==VAR){
	  i=p->q2.v->index;
	  if(p->q2.flags&DREFOBJ) i+=vcount-rcount;
	  if(ind_vars[i]){
	    k2=1;iv=i;
	  }else if(def_invariant(i,-1))
	    k2=2;
	  else
	    k2=0;
	}else
	  k2=2;
	if(p->z.flags&VAR){
	  /*  Aufpassen, dass eine Induktion nicht selbst reduziert   */
	  /*  wird.                                                   */
	  i=p->z.v->index;
	  if(p->z.flags&DREFOBJ) i+=vcount-rcount;
	  if(ind_vars[i]) k1=0;
	}
	if(k1+k2==3){
	  /*                    if(DEBUG&1024) {printf("could perform strength-reduction on:\n");pric2(stdout,p);}*/
	  add_sr(p,head,iv);
	}
      }
      /*  Das hier, um rd_defs zu aktualisieren.  */
      rd_change(p);

      if(p==g->end) break;
    }
    if(g==end) break;
  }
}
void copy_code(struct IC *start,struct IC *end,struct IC *dest,int n,struct IC *ignore)
/*  Kopiert Code von start bis end n-mal hinter dest. Generiert         */
/*  entsprechend neue Labels. Allerdings wird der Flussgraph und        */
/*  aliasing-info nicht angepasst und muss danach neu generiert werden. */
/*  IC ignore wird nicht kopiert.                                       */
{
  int firstl=0,lastl=0,*larray,i,j;
  struct IC *p,*new,*current;
  if(DEBUG&1024) printf("copy_code %d times\n",n);
  /*  Feststellen, welche Labels in der Schleife definiert werden.    */
  for(p=start;p;p=p->next){
    if(p->code==LABEL){
      if(firstl==0||firstl>p->typf) firstl=p->typf;
      if(lastl ==0|| lastl<p->typf) lastl =p->typf;
    }
    if(p==end) break;
  }
  if(DEBUG&1024) printf("firstl=%d, lastl=%d\n",firstl,lastl);
  larray=mymalloc((lastl-firstl+1)*sizeof(*larray));
  for(i=0;i<=lastl-firstl;i++) larray[i]=0;
  for(p=start;p;p=p->next){
    if(p->code==LABEL) larray[p->typf-firstl]=1;
    if(p==end) break;
  }
  current=dest;
  /*  Hauptschleife.  */
  for(i=0;i<n;i++){
    /*  Neue Labels erzeugen.   */
    for(j=0;j<=lastl-firstl;j++)
      if(larray[j]) larray[j]=++label;
    /*  Code kopieren (rueckwaerts).    */
    for(p=start;p;p=p->next){
      if(p!=ignore){
	new=new_IC();
	*new=*p;
	p->copy=new;
	/*  Fuer free_alias.    */
	new->change_cnt=new->use_cnt=0;
	new->change_list=new->use_list=0;
	/*  Evtl. Label anpassen.   */
	if(p->code>=LABEL&&p->code<=BRA){
	  if(p->typf>=firstl&&p->typf<=lastl&&larray[p->typf-firstl])
	    new->typf=larray[p->typf-firstl];
	}
	if(p->code==CALL){
	  int i;
	  new->arg_list=mymalloc(sizeof(*new->arg_list)*new->arg_cnt);
	  for(i=0;i<new->arg_cnt;i++) new->arg_list[i]=p->arg_list[i]->copy;
	  new->call_list=mymalloc(new->call_cnt*sizeof(*new->call_list));
	  memcpy(new->call_list,p->call_list,new->call_cnt*sizeof(*new->call_list));
	}
	insert_IC(current,new);
	current=new;
      }
      if(p==end) break;
    }
  }
  free(larray);
}
void add_ur(int flags,long total,long unroll,struct flowgraph *start,struct flowgraph *head,struct IC *cmp,struct IC *branch,struct IC *ind)
/*  Fuegt Daten fuer loop-unrolling in Stack ein.                       */
{
  struct urlist *new=mymalloc(sizeof(struct urlist));
  if(DEBUG&1024) printf("add_ur, flags=%d\n",flags);
  new->flags=flags;
  new->total=total;
  new->unroll=unroll;
  new->start=start;
  new->head=head;
  new->cmp=cmp;
  new->branch=branch;
  new->ind=ind;
  new->next=first_ur;
  first_ur=new;
}
int do_unroll(int donothing)
/*  Fuehrt loop-unrolling durch. Wenn donothing!=0, wird die Liste nur  */
/*  freigegeben.                                                        */
{
  int changed=0; struct urlist *m;
  struct IC *div;
  while(m=first_ur){
    int flags=m->flags;
    long total=m->total,unroll=m->unroll;
    struct flowgraph *start=m->start,*head=m->head;
    struct IC *cmp=m->cmp,*branch=m->branch,*ind=m->ind;
    if(donothing) flags=0;
    if(flags&UNROLL_COMPLETELY){
      /*  Schleife komplett ausrollen.    */
      if(DEBUG&1024) printf("unroll loop completely\n");
      copy_code(start->start->next,cmp->prev,start->start,total-1,0);
      if(DEBUG&1024) printf("removing loop branch\n");
      remove_IC(branch);
      if(!cmp->z.flags){
	remove_IC(cmp);
	if(DEBUG&1024) printf("removing loop compare\n");
      }
      changed|=1;
    }
    if(flags&UNROLL_MODULO){
      /*  Schleife teilweise ausrollen.   */
      if(DEBUG&1024) printf("unroll loop partially, n=%ld,r=%ld\n",unroll,total%unroll);
      if(unroll>1){
	if((flags&(IND_ONLY_COUNTS|MULTIPLE_EXITS))==IND_ONLY_COUNTS){
	  struct IC *new=new_IC();
	  copy_code(start->start->next,cmp->prev,head->start,total%unroll,ind);
	  *new=*ind;
	  new->use_cnt=new->change_cnt=0;
	  new->use_list=new->change_list=0;
	  if(!(ind->q2.flags&KONST)) ierror(0);
	  eval_const(&ind->q2.val,ind->typf);
	  if(ind->typf&UNSIGNED){
	    gval.vumax=zummult(vumax,ul2zum((unsigned long)(total%unroll)));
	    eval_const(&gval,(UNSIGNED|MAXINT));
	  }else{
	    gval.vumax=zummult(vumax,ul2zum((unsigned long)(total%unroll)));
	    eval_const(&gval,MAXINT);
	  }
	  insert_const(&new->q2.val,ind->typf);
	  insert_IC(head->end,new);
	  copy_code(start->start->next,cmp->prev,start->start,unroll-1,ind);
	  eval_const(&ind->q2.val,ind->typf);
	  if(ind->typf&UNSIGNED){
	    gval.vumax=zummult(vumax,ul2zum((unsigned long)(unroll)));
	    eval_const(&gval,(UNSIGNED|MAXINT));
	  }else{
	    gval.vumax=zummult(vumax,ul2zum((unsigned long)(unroll)));
	    eval_const(&gval,MAXINT);
	  }
	  insert_const(&ind->q2.val,ind->typf);		
	  changed|=1;
	}else{
	  copy_code(start->start->next,cmp->prev,head->start,total%unroll,0);
	  copy_code(start->start->next,cmp->prev,start->start,unroll-1,0);
	  changed|=1;
	}
      }
    }
    if(flags&UNROLL_INVARIANT){
      struct IC *new,*mc,*mn; struct Var *v; int out=++label,code;
      long i; struct Typ *t;static struct Typ tptrdiff={0};
      if(DEBUG&1024) printf("unrolling non-constant loop\n");
      if(ISPOINTER(cmp->typf)){
	tptrdiff.flags=PTRDIFF_T(cmp->typf);
	t=&tptrdiff;
      }else{
	if(cmp->q1.flags&VAR)
	  t=cmp->q1.v->vtyp;
	else
	  t=cmp->q2.v->vtyp;
      }
      v=add_tmp_var(clone_typ(t));
      /*  branch dient hier teilweise als leere Schablone.    */
      /*  Label an Schleifenausgang setzen.   */
      new=new_IC(); 
      *new=*branch;
      new->q1.flags=0;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=LABEL;
      new->typf=out;
      insert_IC(branch,new);
      /*  Test vor die unroll-Variante.   */
      new=new_IC(); *new=*branch;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      if(branch->code==BLT) new->code=BGE;
      if(branch->code==BLE) new->code=BGT;
      if(branch->code==BGT) new->code=BLE;
      if(branch->code==BGE) new->code=BLT;
      if(branch->code==BEQ) ierror(0);
      if(branch->code==BNE) ierror(0);
      code=branch->code;
      mc=new;
      new->typf=out;
      insert_IC(head->start,new);
      new=new_IC(); *new=*cmp;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      insert_IC(head->start,new);
      /*  Einsprungpunkte fuer die Modulos.   */
      label+=unroll;
      for(i=1;i<unroll;i++){
	copy_code(start->start->next,cmp->prev,head->start,1,0);
	new=new_IC(); *new=*branch;
	new->q1.flags=0;
	new->change_cnt=new->use_cnt=0;
	new->change_list=new->use_list=0;
	new->code=LABEL;
	new->typf=out+i;
	insert_IC(head->start,new);
      }
      /*  Testen, welches Modulo. */
      for(i=unroll-2;i>=0;i--){
	new=new_IC(); *new=*branch;
	new->change_cnt=new->use_cnt=0;
	new->change_list=new->use_list=0;
	new->code=BEQ;
	if(i>0) new->typf=out+i;
	else new->typf=start->start->typf;
	insert_IC(head->start,new);
	new=new_IC(); *new=*branch;
	new->change_cnt=new->use_cnt=0;
	new->change_list=new->use_list=0;
	if(SWITCHSUBS) gval.vint=zm2zi(l2zm(1L));
	else       gval.vint=zm2zi(l2zm(i));
	eval_const(&gval,INT);
	new->q1.flags=VAR;
	new->q1.v=v;
	new->q1.val.vmax=l2zm(0L);
	new->typf=t->flags;
	if(SWITCHSUBS||i==0){
	  new->code=TEST;
	  insert_IC(head->start,new);
	  if(i>0){
	    new=new_IC();
	    *new=*head->start->next;
	    new->change_cnt=new->use_cnt=0;
	    new->change_list=new->use_list=0;
	    new->code=SUB;
	    new->z=new->q1;
	    new->q2.flags=KONST;
	    insert_const(&new->q2.val,new->typf&NU);
	    insert_IC(head->start,new);
	  }
	}else{
	  new->code=COMPARE;
	  new->q2.flags=KONST;
	  insert_const(&new->q2.val,new->typf&NU);
	  insert_IC(head->start,new);
	}
      }
      /*  Durchlaeufe modulo unroll berechnen.    */
      new=new_IC(); *new=*branch;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=AND;
      new->typf=t->flags;
      new->q1.flags=VAR;
      new->q1.v=v;
      new->q1.val.vmax=l2zm(0L);
      new->z=new->q1;
      new->q2.flags=KONST;
      gval.vmax=l2zm(unroll-1);
      eval_const(&gval,MAXINT);
      insert_const(&new->q2.val,new->typf);
      insert_IC(head->start,new);
      new=new_IC();
      *new=*ind;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=DIV;
      div=new;
      new->q1=head->start->next->z;
      new->z=new->q1;
      insert_IC(head->start,new);
      new=new_IC();
      *new=*head->start->next;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=ADD;
      insert_IC(head->start,new);
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      if(code==BLT||code==BGT){
	new=new_IC();
	*new=*head->start->next;
	new->change_cnt=new->use_cnt=0;
	new->change_list=new->use_list=0;
	new->code=SUB;
	gval.vmax=l2zm(1L);
	eval_const(&gval,MAXINT);
	insert_const(&new->q2.val,new->typf);
	insert_IC(head->start,new);
      }
      new=new_IC();
      *new=*head->start->next;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=SUB;
      if(t==&tptrdiff)
	new->code=SUBPFP;

      if(!compare_objs(&ind->z,&cmp->q1,new->typf)){
	if(code==BLT||code==BLE){
	  new->q1=cmp->q2;new->q2=ind->z;
	}else{
	  new->q2=cmp->q2;new->q1=ind->z;
	}
      }else{
	if(code==BLT||code==BLE){
	  new->q1=cmp->q1;new->q2=ind->z;
	}else{
	  new->q2=cmp->q1;new->q1=ind->z;
	}
      }
#if 0
      if(ind->code==SUB){
	struct obj o;
	o=new->q1;new->q1=new->q2;new->q2=o;
      }
#endif
      if(!new->q1.flags){
	if(!new->q2.flags) ierror(0);
	new->q1.flags=KONST;
	gval.vmax=l2zm(0L);
	eval_const(&gval,new->typf);
	insert_const(&new->q1.val,new->typf);
      }
      if(!new->q2.flags){
	if(!new->q1.flags) ierror(0);
	new->q2.flags=KONST;
	gval.vmax=l2zm(0L);
	eval_const(&gval,new->typf);

	insert_const(&new->q2.val,new->typf);
      }
      insert_IC(head->start,new);
      new=new_IC();
      *new=*mc;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->typf=out+1;
      insert_IC(head->start,new);
      new=new_IC();
      *new=*cmp; new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      insert_IC(head->start,new);
      if((flags&(IND_ONLY_COUNTS|MULTIPLE_EXITS))==IND_ONLY_COUNTS){
	if(!(ind->q2.flags&KONST)) ierror(0);
	copy_code(start->start->next,cmp->prev,start->start,unroll-1,ind);
	eval_const(&ind->q2.val,ind->typf);
	if(ind->typf&UNSIGNED){
	  gval.vumax=zummult(vumax,ul2zum((unsigned long)(unroll)));
	  eval_const(&gval,(UNSIGNED|MAXINT));
	}else{
	  gval.vumax=zummult(vumax,ul2zum((unsigned long)(unroll)));
	  eval_const(&gval,MAXINT);
	}
	insert_const(&ind->q2.val,ind->typf);		
	changed|=2;
      }else{
	copy_code(start->start->next,cmp->prev,start->start,unroll-1,0);
	changed|=2;
      }
      div->typf|=UNSIGNED;
    }
    if(flags&UNROLL_REVERSE){
      struct IC *new,*mc; struct Var *v; int out=++label,code;
      long i; struct Typ *t;static struct Typ tptrdiff={0};
      if(DEBUG&1024) printf("reversing loop\n");
      if(ISPOINTER(cmp->typf)){
	tptrdiff.flags=PTRDIFF_T(cmp->typf);
	t=&tptrdiff;
      }else{
	if(cmp->q1.flags&VAR)
	  t=cmp->q1.v->vtyp;
	else
	  t=cmp->q2.v->vtyp;
      }
      v=add_tmp_var(clone_typ(t));
      new=new_IC();
      *new=*ind;
      new->code=SUB;
      new->typf=t->flags;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->q1.flags=VAR;
      new->q1.v=v;
      new->q1.val.vmax=l2zm(0L);
      new->z=new->q1;
      new->q2.flags=KONST;
      gval.vmax=l2zm(1L);
      eval_const(&gval,MAXINT);
      insert_const(&new->q2.val,new->typf);
      insert_IC(cmp->prev,new);
      code=branch->code;
#if HAVE_WANTBNE
      branch->code=BNE;
#else
      branch->code=BGT;
#endif
      /*  Durchlaeufe berechnen.    */
      new=new_IC();
      *new=*ind;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=DIV;
      div=new;
      new->q1.flags=VAR;
      new->q1.v=v;
      new->q1.val.vmax=l2zm(0L);
      new->z=new->q1;
      insert_IC(head->start,new);
      new=new_IC();
      *new=*head->start->next;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=ADD;
      insert_IC(head->start,new);
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      if(code==BLT||code==BGT||code==BNE){
	struct IC *a=head->start->next;
	gval.vmax=l2zm(1L);
	eval_const(&gval,MAXINT);
	insert_const(&gval,a->typf);
	calc(SUB,a->typf,&a->q2.val,&gval,&a->q2.val,0);
      }
      new=new_IC();
      *new=*head->start->next;
      new->change_cnt=new->use_cnt=0;
      new->change_list=new->use_list=0;
      new->code=SUB;
      if(t==&tptrdiff)
	new->code=SUBPFP;

      if(!compare_objs(&ind->z,&cmp->q1,new->typf)){
	if(code==BLT||code==BLE||(code==BNE&&(ind->code==ADD||ind->code==ADDI2P))){
	  new->q1=cmp->q2;new->q2=ind->z;
	}else{
	  new->q2=cmp->q2;new->q1=ind->z;
	}
      }else{
	if(code==BLT||code==BLE||(code==BNE&&(ind->code==ADD||ind->code==ADDI2P))){
	  new->q1=cmp->q1;new->q2=ind->z;
	}else{
	  new->q2=cmp->q1;new->q1=ind->z;
	}
      }
#if 0
      if(ind->code==SUB){
	struct obj o;
	o=new->q1;new->q1=new->q2;new->q2=o;
      }
#endif
      if(!new->q1.flags||!new->q2.flags) ierror(0);
      insert_IC(head->start,new);
      cmp->code=TEST;
      cmp->q1.flags=VAR;
      cmp->q1.v=v;
      cmp->q1.val.vmax=l2zm(0L);
      cmp->q2.flags=0;
      div->typf|=UNSIGNED;
      changed|=2;
    }
    first_ur=m->next;
    free(m);
  }
  return changed;
}

void unroll(struct flowgraph *start,struct flowgraph *head)
/*  Versucht loop-unrolling.                                            */
{
  struct flowlist *lp;struct flowgraph *end,*g;struct IC *p,*m,*branch,*cmp;
  struct obj *o,*e,*cc; union atyps init_val,end_val,step_val;
  bvtype *tmp;
  long dist,step,ic_cnt,n;
  int bflag=0,t=0,i,flags=0; /* 1: sub, 2: init_val gefunden  */
  int ind_only_counts,multiple_exits,cfl;
  end=start->loopend;
  if(DEBUG&1024) printf("checking for possible unrolling from %d to %d\n",start->index,end->index);
  if(end->end->code==BRA){
    if(DEBUG&1024) printf("loop ends with BRA\n");
    return;
  }
  for(lp=start->in;lp;lp=lp->next)
    if(lp->graph->index>start->index&&lp->graph->index<=end->index&&lp->graph!=end) return;
  if(DEBUG&1024) printf("only one backward-branch\n");
  e=0; p=end->end; cfl=0;
  do{
    if(p->code>=BEQ&&p->code<BRA){ branch=p;bflag=p->code;cc=&p->q1; cfl=1;}
    if(p->code==TEST){
      if(!cfl) return;
      if(compare_objs(cc,&p->z,p->typf)) return;
      o=&p->q1;t=p->typf;cmp=p;
      end_val.vmax=l2zm(0L); eval_const(&end_val,MAXINT);
      insert_const(&end_val,t);
      break;
    }
    if(p->code==COMPARE){
      if(!cfl) return;
      if(compare_objs(cc,&p->z,p->typf)) return;
      cmp=p;
      if(p->q1.flags&VAR){
	if(ind_vars[p->q1.v->index]){
	  o=&p->q1;t=p->typf;
	  e=&p->q2;
	  break;
	}
      }
      if(p->q2.flags&VAR){
	if(ind_vars[p->q2.v->index]){
	  o=&p->q2;t=p->typf;
	  e=&p->q1;
	  if(bflag==BLT) bflag=BGT;
	  if(bflag==BLE) bflag=BGE;
	  if(bflag==BGT) bflag=BLT;
	  if(bflag==BGE) bflag=BLE;
	  break;
	}
      }
      return;
    }
    if(p==end->start) return;
    p=p->prev;
  }while(p);
  if(!e||(e->flags&KONST)){
    if(e) end_val=e->val;
    if(DEBUG&1024) printf("end condition is constant\n");
  }else{
    if(!(e->flags&VAR)) return;
    i=e->v->index;
    if(e->flags&DREFOBJ) i+=vcount-rcount;
    if(DEBUG&1024) printf("testing end-condition\n");
    memcpy(rd_defs,end->rd_in,dsize);
    for(m=end->start;m;m=m->next){
      if(m==cmp){
	if(DEBUG&1024) pric2(stdout,m);
	if(!def_invariant(i,-1)) return;
	if(DEBUG&1024) printf("end condition loop-invariant\n");
	break;
      }
      rd_change(m);
      if(m==end->end) ierror(0);
    }
  }
  p=ind_vars[o->v->index];
  if(!p) return;
  if(compare_objs(o,&p->z,t)) return;
  if(DEBUG&1024) printf("loop condition only dependant on induction var\n");
  if(!(p->q2.flags&KONST)) return;
  if(DEBUG&1024) printf("induction is constant\n");
  ind_only_counts=IND_ONLY_COUNTS;
  multiple_exits=0;
  for(ic_cnt=0,g=start;g;g=g->normalout){
    for(m=g->start;m;m=m->next){
      if(m!=p&&m!=cmp&&ind_only_counts){
	int i;
	for(i=0;i<m->use_cnt;i++){
	  if(m->use_list[i].v==p->q1.v){
	    if(DEBUG&1024){printf("use of ind_var:");pric2(stdout,m);}
	    ind_only_counts=0;break;
	  }
	}
      }
      if(m==p&&!always_reached(start,end,g,p,1)) return;
      ic_cnt++;
      if(m==g->end) break;
    }
    if(g==end) break;
    if(!multiple_exits&&g->branchout&&(g->branchout->index<start->index||g->branchout->index>end->index))
      multiple_exits=MULTIPLE_EXITS;
  }
  ic_cnt-=2;  /*  Branch und Test */
  if(DEBUG&1024) printf("induction always reached\n");
  if(ind_only_counts&&(DEBUG&1024)) printf("induction variable only used as counter\n");
  if(multiple_exits&&(DEBUG&1024)) printf("loop has multiple exits\n");
  if(DEBUG&1024) printf("ICs in loop: %ld\n",ic_cnt);
  step_val=p->q2.val;
  if(p->code==SUB) flags|=1;
  if(start->start->code==LABEL&&(start->start->flags&LOOP_COND_TRUE)&&e&&ind_only_counts){
    if(e->flags&KONST)
      eval_const(&e->val,t);
    if(!(e->flags&KONST)||!zmeqto(vmax,l2zm(0L))||!zumeqto(vumax,ul2zum(0UL))||!zldeqto(vldouble,d2zld(0.0))){
      int bc=branch->code;
      eval_const(&step_val,t);
      if((!(flags&1)&&(bc==BLT||bc==BLE))
	 ||((flags&1)&&(bc==BGT||bc==BGE))
	 ||(bc==BNE&&zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL)))){
	if((t&UNSIGNED)||zmleq(l2zm(0L),vmax)){
	  if(optspeed){
	    add_ur(UNROLL_REVERSE|ind_only_counts|multiple_exits,0,1,start,head,cmp,branch,p);
	    return;
	  }else{
	    if(zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))&&zldeqto(vldouble,d2zld(1.0))){
	      add_ur(UNROLL_REVERSE|ind_only_counts|multiple_exits,0,1,start,head,cmp,branch,p);
	      return;
	    }
	  }
	}
      }
    }
  }
  if(!e||(e->flags&KONST)){
    struct IC tmpic;
    i=p->z.v->index;
    if(p->z.flags&DREFOBJ) i+=vcount-rcount;
    memcpy(rd_defs,head->rd_out,dsize);
    tmpic=*p;
    if(propagate(&tmpic,&tmpic.z,1,1)){
      init_val=tmpic.z.val;
    }else{
      goto inv;
    }
    if(DEBUG&1024){
      printf("loop number determinable\n");
      printf("init_val: ");printval(stdout,&init_val,t);
      printf("\nend_val: ");printval(stdout,&end_val,t);
      printf("\nstep_val: ");printval(stdout,&step_val,t);
      printf("\nflags=%d bflag=%d\n",flags,bflag);
    }
    /*  Nur integers als Induktionsvariablen.   */
    if(!ISINT(t)) goto inv;
    /*  Distanz und Step werden als long behandelt, deshalb pruefen, ob */
    /*  alles im Bereich des garantierten Mindestwerte fuer long.       */
    /*  Wenn man hier die Arithmetik der Zielmaschine benutzen wuerde,  */
    /*  koennte man theoretisch mehr Faelle erkennen, aber das waere    */
    /*  recht popelig und man muss sehr aufpassen.                      */
    if(t&UNSIGNED){
      eval_const(&step_val,t);
      if(!zumleq(vumax,l2zm(2147483647))) return;
      step=zum2ul(vumax);
      if(flags&1) step=-step;
      eval_const(&end_val,t);
      if(!zumleq(vumax,l2zm(2147483647))) goto inv;
      dist=zum2ul(vumax);
      eval_const(&init_val,t);
      if(!zumleq(vumax,l2zm(2147483647))) goto inv;
      dist-=zum2ul(vumax);
    }else{
      eval_const(&step_val,t);
      if(!zmleq(vmax,l2zm(2147483647))) return;
      if(zmleq(vmax,l2zm(-2147483647))) return; /*  eins weniger als moeglich waere */
      step=zm2l(vmax);
      if(flags&1) step=-step;
      eval_const(&end_val,t);
      if(!zmleq(vmax,l2zm(2147483647/2))) goto inv;
      if(zmleq(vmax,l2zm(-2147483647/2))) goto inv; /*  eins weniger als moeglich waere */
      dist=zm2l(vmax);
      eval_const(&init_val,t);
      if(!zmleq(vmax,l2zm(2147483647/2))) goto inv;
      if(zmleq(vmax,l2zm(-2147483647/2))) goto inv; /*  eins weniger als moeglich waere */
      dist-=zm2l(vmax);
    }
    if(DEBUG&1024) printf("dist=%ld, step=%ld\n",dist,step);
    if(step==0) ierror(0);
    /*  Die Faelle kann man noch genauer untersuchen, ob die Schleife   */
    /*  evtl. nur einmal durchlaufen wird o.ae.                         */
    if(step<0&&dist>=0){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
    if(step>0&&dist<=0){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
    if(bflag==BEQ){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
    /*  Aufpassen, ob das Schleifenende bei BNE auch getroffen wird.    */
    if(bflag==BNE){
      if(dist%step){
	if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
	return;
      }
    }
    if(bflag==BLT||bflag==BGT||bflag==BNE){
      if(step>0) dist--; else dist++;
    }
    if(dist/step<0) ierror(0);
    if(DEBUG&1024) printf("loop is executed %ld times\n",dist/step+1);
    if(start->start->code!=LABEL) ierror(0);
    if(dist/step+1==1||ic_cnt*(dist/step+1)<=unroll_size){
      /*  Schleife komplett ausrollen.    */
      add_ur(UNROLL_COMPLETELY|ind_only_counts|multiple_exits,dist/step+1,dist/step+1,start,head,cmp,branch,p);
    }else{
      /*  Schleife teilweise ausrollen.   */
      n=(unroll_size-ic_cnt-2)/(2*ic_cnt);
      if(n>0)
	add_ur(UNROLL_MODULO|ind_only_counts|multiple_exits,dist/step+1,n,start,head,cmp,branch,p);
    }
    return;
  }
 inv:
  /*  Anzahl der Schleifendurchlaeufe kann beim Eintritt in die   */
  /*  Schleife zur Laufzeit berechnet werden.                     */
  if(!(p->q2.flags&KONST)) return;
  if(t&UNSIGNED){
    eval_const(&step_val,t);
    if(!zumleq(vumax,l2zm(2147483647))) return;
    step=zum2ul(vumax);
    if(flags&1) step=-step;
  }else{
    eval_const(&step_val,t);
    if(!zmleq(vmax,l2zm(2147483647))) return;
    if(zmleq(vmax,l2zm(0))) return; /* gibt Probleme */
    step=zm2l(vmax);
    if(flags&1) step=-step;
  }
  if(!compare_objs(&cmp->q1,&p->z,t)){
    if(step>0&&(branch->code==BGT||branch->code==BGE)){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
    if(step<0&&(branch->code==BLT||branch->code==BLE)){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
  }else{
    if(step<0&&(branch->code==BGT||branch->code==BGE)){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
    if(step>0&&(branch->code==BLT||branch->code==BLE)){
      if(report_suspicious_loops){ error(208);report_suspicious_loops=0;}
      return;
    }
  }
  if(!unroll_all) return;
  if(bflag!=BEQ&&bflag!=BNE){
    if(unroll_size>=8*ic_cnt+8)
      add_ur(UNROLL_INVARIANT|ind_only_counts|multiple_exits,0,8,start,head,cmp,branch,p);
    else if(unroll_size>=4*ic_cnt+4)
      add_ur(UNROLL_INVARIANT|ind_only_counts|multiple_exits,0,4,start,head,cmp,branch,p);
    else if(unroll_size>=4*ic_cnt+2)
      add_ur(UNROLL_INVARIANT|ind_only_counts|multiple_exits,0,2,start,head,cmp,branch,p);
    return;
  }
}

int loop_optimizations(struct flowgraph *fg)
/*  steuert Optimierungen in Schleifen  */
{
  int changed=0,i;
  struct flowgraph *g,*last;
  if(DEBUG&1024) print_flowgraph(fg);
  if(loops(fg,0)==0) return 0;
  if(DEBUG&1024) print_flowgraph(fg);
  first_fg=fg=create_loop_headers(fg,0);
  if(DEBUG&1024) print_flowgraph(fg);
  num_defs();

  /*bsize=(basic_blocks+CHAR_BIT)/CHAR_BIT;*/
  bsize=BVSIZE(basic_blocks+1);
  fg_tmp=mymalloc(bsize);
  ind_vars=mymalloc(vcount*sizeof(*ind_vars));
  invariant=mymalloc(dsize);
  inloop=mymalloc(dsize);
  rd_defs=mymalloc(dsize);
  reaching_definitions(fg);
  if(DEBUG&1024) print_flowgraph(fg);
  moved=mymalloc(dsize);
  memset(moved,0,dsize);
  moved_completely=mymalloc(dsize);
  memset(moved_completely,0,dsize);
  not_movable=mymalloc(dsize);

  first_mov=last_mov=0;
  first_sr=last_sr=0;

  for(last=0,g=fg;g;g=g->normalout){
    if(g->loopend){
      frequency_reduction(g,g->loopend,last);
      strength_reduction(g,g->loopend,last);
      if(optflags&2048) unroll(g,last);
    }
    last=g;
  }

  free(rd_matrix);
  free(var_defs);
  free(defs_kill);
  free(defs_gen);
  free(dlist);
  free(rd_defs);
  free(invariant);
  free(inloop);
  changed|=move_to_head();
  if(DEBUG&1024) puts("done");
  changed|=do_sr();
  if(DEBUG&1024) puts("done");
  changed|=do_unroll(changed);
  if(DEBUG&1024) puts("done");
  free(moved);
  free(not_movable);
  free(moved_completely);
  if(DEBUG&1024) puts("4");
  if(changed&2){
    if(DEBUG&1024) printf("must repeat num_vars\n");
    free(vilist);
    free(av_globals);free(av_statics);
    free(av_drefs);free(av_address);
    num_vars();
  }

  free(ind_vars);
  free(fg_tmp);

  return changed;
}


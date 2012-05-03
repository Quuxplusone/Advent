/*  $VER: vbcc (av.c) V0.8     */
/*  aktive Variablen und Elimination unnoetiger Anweisungen */

#include "opt.h"

static char FILE_[]=__FILE__;

/*  fuer aktive Variablen   */
struct Var **vilist;
unsigned int vcount;    /*  0..vcount-rcount-1: vars, vcount-rcount..vcount: DREFOBJs */
unsigned int rcount;
size_t vsize;
bvtype *av_globals,*av_address,*av_statics,*av_drefs;
int report_dead_statements;

void print_av(bvtype *bitvector)
/*  druckt Variablen in einem Bitvektor */
{
    int i;
    if(!bitvector) {printf("active variables not available\n");return;}
    for(i=0;i<vcount-rcount;i++)
        if(BTST(bitvector,i)) printf("%3d: %s,%ld\n",i,vilist[i]->identifier,zm2l(vilist[i]->offset));
    for(i=vcount-rcount;i<vcount;i++)
        if(BTST(bitvector,i)) printf("%3d: (%s),%ld\n",i,vilist[i]->identifier,zm2l(vilist[i]->offset));
}

static int clcnt;

/* walks through a clist and numbers variables whose addresses are
   contained; if pass==-1, index is set to -1, if pass==0, index is set,
   otherwise vilist
   clcnt is used as index-counter in pass 0 */
void num_clist_refs(int pass,struct Typ *t,struct const_list *cl)
{
  /*FIXME: bei Aufrufen auch auf locale, nicht USEDASDEST|USEDASADDR */
  int i;zmax sz;
  if(ISARRAY(t->flags)){
    for(sz=l2zm(0L);!zmleq(t->size,sz)&&cl;sz=zmadd(sz,l2zm(1L)),cl=cl->next){
      if(!cl->other){ierror(0);return;}
      num_clist_refs(pass,t->next,cl->other);
    }
    return;
  }
  if(ISUNION(t->flags)){
    num_clist_refs(pass,(*t->exact->sl)[0].styp,cl);
    return;
  }
  if(ISSTRUCT(t->flags)&&!cl->tree){
    struct Typ *st;
    for(i=0;i<t->exact->count&&cl;i++){
      st=(*t->exact->sl)[i].styp;
      if(!(*t->exact->sl)[i].identifier) ierror(0);
      if((*t->exact->sl)[i].identifier[0]){
        if(!cl->other)
          ierror(0);
        num_clist_refs(pass,st,cl->other);
        cl=cl->next;
      }
    }
    return;
  }
  if(cl->tree&&(cl->tree->o.flags&VARADR)){
    struct Var *v=cl->tree->o.v;
    if(pass==-1){
      v->index=-1;
    }else if(pass==0){
      if(v->index<0) {v->index=clcnt++;v->inr=inr;}
    }else{
      if(v->index<0||v->index>=vcount-rcount)
	ierror(0);
      vilist[v->index]=v;
    }
  }
}

int inr;

void num_vars(void)
/*  Numeriert Variablen und erzeugt Indexliste  */
{
    unsigned int i,j,done;struct IC *p;struct Var *v,*a[4],*vp;
    unsigned long heapsize=0;
    if(DEBUG&1024) printf("numerating variables loop1\n");
    inr++;
    /*  alle Indizes auf -1 */
    a[0]=vl0;
    a[1]=vl1;
    a[2]=vl2;
    a[3]=vl3;
#if 1
    for(j=0;j<4;j++){
        v=a[j];
        while(v){
            v->index=-1;
            /*  Variablen von inline-Funktionen */
            if(j==0&&v->fi&&v->fi->first_ic){
                for(vp=v->fi->vars;vp;vp=vp->next) vp->index=-1;
            }
            v=v->next;
        }
    }
    /* variables that may be referenced in inter-proc. dflow-info */
    for(p=first_ic;p;p=p->next){
      if(p->code==CALL&&(p->q1.flags&VAR)&&p->q1.v->fi){
	struct function_info *fi=p->q1.v->fi;
	if(fi->flags&ALL_USES){
	  for(i=0;i<fi->use_cnt;i++){
	    if(v=fi->use_list[i].v) fi->use_list[i].v->index=-1;
	  }
	}
	if(fi->flags&ALL_MODS){
	  for(i=0;i<fi->change_cnt;i++){
	    if(v=fi->change_list[i].v) fi->change_list[i].v->index=-1;
	  }
	}
      }
      /* const-lists */
      if((p->q1.flags&VAR))
	if(p->q1.v->clist&&is_const(p->q1.v->vtyp)) num_clist_refs(-1,p->q1.v->vtyp,p->q1.v->clist);
      if((p->q2.flags&VAR))
	if(p->q2.v->clist&&is_const(p->q2.v->vtyp)) num_clist_refs(-1,p->q2.v->vtyp,p->q2.v->clist);
      if((p->z.flags&VAR))
	if(p->z.v->clist&&is_const(p->z.v->vtyp)) num_clist_refs(-1,p->z.v->vtyp,p->z.v->clist);
    }
#endif
    /* Do we need this? */
    for(p=first_ic;p;p=p->next){
      if(p->q1.flags&VAR) {p->q1.v->index=-1;p->q1.v->flags&=~USEDASADR;}
      if(p->q2.flags&VAR) {p->q2.v->index=-1;p->q2.v->flags&=~USEDASADR;}
      if(p->z.flags&VAR) {p->z.v->index=-1;p->z.v->flags&=~USEDASADR;}
    }
    /*  erst alle Variablen, die als DREFOBJ benutzt werden */
    if(DEBUG&1024) printf("numerating variables loop2\n");
    i=0;
    do{
      done=1;
      if(DEBUG&1024) printf("pass\n");
      for(p=first_ic;p;p=p->next){
        if(p->code<LABEL||p->code>BRA){
	  int c=p->code;
	  /* mark variables ad USEDASADR */
	  if(c==ADDRESS) p->q1.v->flags|=USEDASADR;
	  if(p->q1.flags&VARADR) p->q1.v->flags|=USEDASADR;
	  if(p->q2.flags&VARADR) p->q2.v->flags|=USEDASADR;
	  if(p->z.flags&VARADR) p->z.v->flags|=USEDASADR;
	  j=(q1typ(p)&NQ);
	  if((p->q1.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)){
	    v=p->q1.v;
	    if(!v->vtyp->next||(v->vtyp->next->flags&NQ)!=j) v->flags|=DNOTTYPESAFE;
	    if(v->index<0) {v->index=i++;v->inr=inr;done=0;}
	  }
	  j=(q2typ(p)&NQ);
	  if((p->q2.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)){
	    v=p->q2.v;
	    if(!v->vtyp->next||(v->vtyp->next->flags&NQ)!=j) v->flags|=DNOTTYPESAFE;
	    if(v->index<0) {v->index=i++;v->inr=inr;done=0;}
	  }
	  j=(ztyp(p)&NQ);
	  if((p->z.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)){
	    v=p->z.v;
	    if(!v->vtyp->next||(v->vtyp->next->flags&NQ)!=j) v->flags|=DNOTTYPESAFE;
	    if(v->index<0) {v->index=i++;v->inr=inr;done=0;}
	  }
	  /* mark copies from DREFs also as DREFs (necessary?) (check z for !DREFOBJ?) */
	  if((c==ASSIGN||c==ADDI2P||c==SUBIFP)&&(p->q1.flags&VAR)&&p->q1.v->index>=0&&(p->z.flags&VAR)&&p->z.v->index<0){
	    if(!(p->z.flags&VAR)) ierror(0);
	    p->z.v->index=i++;p->z.v->inr=inr;done=0;
	  }
	  /* mark copies to DREFs as DREFs (because of copy-propagation */
	  /* and post-op reordering */
	  if((c==ASSIGN||c==ADDI2P||c==SUBIFP)&&(p->z.flags&VAR)&&p->z.v->index>=0&&(p->q1.flags&VAR)&&p->q1.v->index<0){
	    p->q1.v->index=i++;p->q1.v->inr=inr;done=0;
	  }	  
        }
      }
    }while(!done);
    if(DEBUG&1024) printf("numerating variables loop3\n");
    rcount=i;    /*  Anzahl der DREFOBJ-Variablen    */
    /*  jetzt den Rest  */
    for(p=first_ic;p;p=p->next){
        int c=p->code;
        if(1/*p->code<LABEL||p->code>BRA*/){
            if((p->q1.flags&(VAR|DREFOBJ))==VAR){
	      j=(q1typ(p)&NQ);
	      v=p->q1.v;
	      if((v->vtyp->flags&NQ)!=j) v->flags|=NOTTYPESAFE;
	      if(v->index<0) {v->index=i++;v->inr=inr;}
            }
            if((p->q2.flags&(VAR|DREFOBJ))==VAR){
	      j=(q2typ(p)&NQ);
	      v=p->q2.v;
	      if((v->vtyp->flags&NQ)!=j) v->flags|=NOTTYPESAFE;
	      if(v->index<0) {v->index=i++;v->inr=inr;}
            }
            if((p->z.flags&(VAR|DREFOBJ))==VAR){
	      j=(ztyp(p)&NQ);
	      v=p->z.v;
	      if((v->vtyp->flags&NQ)!=j) v->flags|=NOTTYPESAFE;
	      if(v->index<0) {v->index=i++;v->inr=inr;}
            }
        }
	/* const-lists */
	if((p->q1.flags&VAR))
	  if(p->q1.v->clist&&is_const(p->q1.v->vtyp)) {clcnt=i;num_clist_refs(0,p->q1.v->vtyp,p->q1.v->clist);i=clcnt;}
	if((p->q2.flags&VAR))
	  if(p->q2.v->clist&&is_const(p->q2.v->vtyp)) {clcnt=i;num_clist_refs(0,p->q2.v->vtyp,p->q2.v->clist);i=clcnt;}
	if((p->z.flags&VAR))
	  if(p->z.v->clist&&is_const(p->z.v->vtyp)) {clcnt=i;num_clist_refs(0,p->z.v->vtyp,p->z.v->clist);i=clcnt;}
    }
    if(DEBUG&1024) printf("numerating variables loop4\n");
    vcount=i+rcount; /*  alle benutzten Variablen+Anzahl der DREFOBJs    */
    vilist=mymalloc(vcount*sizeof(struct Var *));
    heapsize+=vcount*sizeof(struct Var *);
#if 0
    for(j=0;j<4;j++){
        int i;
        v=a[j];
        while(v){
            i=v->index;
/*            printf("%s has index %d\n",v->identifier,i);*/
            if(i>=0){
                if(i>=vcount-rcount) ierror(0);
                vilist[i]=v;
                if(i<rcount) vilist[i+vcount-rcount]=v;
            }
            /*  Variablen von inline-Funktionen */
            if(j==0&&v->fi&&v->fi->first_ic){
                for(vp=v->fi->vars;vp;vp=vp->next){
                    i=vp->index;
                    if(i>=0){
                        if(i>=vcount-rcount) ierror(0);
                        vilist[i]=vp;
                        if(i<rcount) vilist[i+vcount-rcount]=vp;
                    }
                }
            }
            v=v->next;
        }
    }
#endif

    for(p=first_ic;p;p=p->next){
      struct Var *v;
      if(p->q1.flags&VAR){
	i=p->q1.v->index;
	vilist[i]=v=p->q1.v;
	if(v->clist&&is_const(v->vtyp)) num_clist_refs(1,v->vtyp,v->clist);
      }
      if(p->q2.flags&VAR){
	i=p->q2.v->index;
	vilist[i]=v=p->q2.v;
	if(v->clist&&is_const(v->vtyp)) num_clist_refs(1,v->vtyp,v->clist);

      }
      if(p->z.flags&VAR){
	i=p->z.v->index;
	vilist[i]=v=p->z.v;
	if(v->clist&&is_const(v->vtyp)) num_clist_refs(1,v->vtyp,v->clist);
      }
    }
    for(i=0;i<rcount;i++)
      vilist[i+vcount-rcount]=vilist[i];

    /*vsize=(vcount+CHAR_BIT-1)/CHAR_BIT;*/
    vsize=BVSIZE(vcount);
    if(DEBUG&(16384|1024)) printf("%lu variables (%lu DREFOBJs), vsize=%lu\n",(unsigned long)vcount,(unsigned long)rcount,(unsigned long)vsize);

    av_drefs=mymalloc(vsize);
    memset(av_drefs,0,vsize);
    /*  alle DREFOBJs   */
    for(i=vcount-rcount;i<vcount;i++) BSET(av_drefs,i);

    /*  av_globals enthaelt alle globalen Variablen und av_address      */
    /*  zusaetzlich noch alle Variablen, deren Adressen genommen wurden */
    av_globals=mymalloc(vsize);
    memset(av_globals,0,vsize);
    av_statics=mymalloc(vsize);
    memset(av_statics,0,vsize);
    av_address=mymalloc(vsize);
    memcpy(av_address,av_globals,vsize);
    heapsize+=4*vsize;
    for(i=0;i<vcount-rcount;i++){
        if(vilist[i]->nesting==0||vilist[i]->storage_class==EXTERN) BSET(av_globals,i);
        if(vilist[i]->flags&USEDASADR) BSET(av_address,i);
        if(vilist[i]->storage_class==STATIC) BSET(av_statics,i);
        if(i<rcount){
/*            if(!ISPOINTER(vilist[i]->vtyp->flags)){ printf("%s(%ld)\n",vilist[i]->identifier,zm2l(vilist[i]->offset));ierror(0);}*/
            BSET(av_address,i+vcount-rcount);
            BSET(av_globals,i+vcount-rcount);
        }
    }
    if(DEBUG&16384) printf("num_vars heapsize=%lu\n",heapsize);
}
void print_vi(void)
/*  Druckt vilist und testet Konsistenz */
{
    int i;
    printf("\nprint_vi()\n");
    for(i=0;i<vcount;i++){
        if(!vilist[i]||(i<rcount&&vilist[i]->index!=i)) ierror(0);
        printf("%3d: %s\n",i,vilist[i]->identifier);
    }
}
void av_change(struct IC *p,bvtype *use,bvtype *def)
/*  Berechnet die Aenderungen, die sich durch IC p an use und def ergeben.  */
{
    int i,j,n=-1;
    int g1,g2;

    /*  Wenn eine Quelle==Ziel, dann wird dadurch kein neuer use erzeugt,   */
    /*  um z.B. unbenutzte Induktionsvariablen in Schleifen zu eliminieren. */
    g1=compare_objs(&p->q1,&p->z,p->typf);
    g2=compare_objs(&p->q2,&p->z,p->typf);
    if(!g1&&(p->q1.flags&(VAR|DREFOBJ))==VAR) n=p->q1.v->index;
    if(!g2&&(p->q2.flags&(VAR|DREFOBJ))==VAR) n=p->q2.v->index;

    for(j=0;j<p->use_cnt;j++){
        i=p->use_list[j].v->index;
        if(p->use_list[j].flags&DREFOBJ) i+=vcount-rcount;
        if(i>=vcount) continue;
        if(i!=n&&!BTST(def,i)) BSET(use,i);
    }

    /*  Ein Wert wird nicht zerstoert, wenn es kein elementarer Typ ist und */
    /*  die Groesse kleiner als die Variable (steht in alle solchen ICs in  */
    /*  q2.val.max.                                                         */
    if((p->z.flags&(VAR|DREFOBJ))==VAR&&(ISSCALAR(p->z.v->vtyp->flags)||p->z.v->vtyp->flags==0||zmeqto(p->q2.val.vmax,szof(p->z.v->vtyp)))){
        i=p->z.v->index;
        if(i>=vcount) ierror(0);
        if(g1&&g2&&!BTST(use,i)) BSET(def,i);
        /*  Wenn p geaendert wird, wird auch *p geaendert   */
        if(i<rcount&&!BTST(def,i+vcount-rcount)) BSET(use,i+vcount-rcount);
    }
    if((p->z.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)&&g1&&g2&&!(p->z.v->flags&DNOTTYPESAFE)){
        i=p->z.v->index+vcount-rcount;
        if(i>=vcount) ierror(0);
        if(!BTST(use,i)) BSET(def,i);
    }
}
void active_vars(struct flowgraph *fg)
/*  analysiert aktive Variablen im Flussgraphen, nomain==0, wenn zu     */
/*  optimierende Funktion main() ist                                    */
{
    struct IC *p;
    int changed,pass;struct flowgraph *g;
    unsigned long heapsize=0;

    if(DEBUG&1024){printf("analysing active variables\n");/*scanf("%d",&i);*/}
    tmp=mymalloc(vsize);
    heapsize+=vsize;
    /*  av_gen und av_kill fuer jeden Basic Block berechnen */
    if(DEBUG&1024){printf("active_vars(): loop1\n");/*scanf("%d",&i);*/}
    g=fg;
    while(g){
        g->av_gen=mymalloc(vsize);
        memset(g->av_gen,0,vsize);
        g->av_kill=mymalloc(vsize);
        memset(g->av_kill,0,vsize);
        g->av_in=mymalloc(vsize);
        memset(g->av_in,0,vsize);
        g->av_out=mymalloc(vsize);
        memset(g->av_out,0,vsize);
	heapsize+=4*vsize;
        for(p=g->start;p;p=p->next){
            av_change(p,g->av_gen,g->av_kill);
            if(p==g->end) break;
        }
        g=g->normalout;
    }

    /*  av_in und av_out fuer alle Bloecke berechnen    */
    if(DEBUG&1024){printf("active_vars(): loop2\npass: ");/*scanf("%d",&i);*/}
    pass=0;
    do{
        if(DEBUG&1024) {printf(" %d",++pass);fflush(stdout);}
        changed=0;
        g=fg;
        while(g){
            /* out(B)=U in(C) ueber alle Nachfolger C von B */
            memset(g->av_out,0,vsize);  /*  noetig? */
            if(g->branchout) bvunite(g->av_out,g->branchout->av_in,vsize);
            if((!g->end||g->end->code!=BRA)&&g->normalout) bvunite(g->av_out,g->normalout->av_in,vsize);
            /*  Am Ende muessen alle globalen Variablen bekannt sein    */
            if(!g->normalout){
                bvunite(g->av_out,av_globals,vsize);
                /*if(!nocall)*/ bvunite(g->av_out,av_statics,vsize);
            }
            /* in(B)=use(B)U(out(B)-def(B)) */
            memcpy(tmp,g->av_out,vsize);
            bvdiff(tmp,g->av_kill,vsize);
            bvunite(tmp,g->av_gen,vsize);

            if(!bvcmp(tmp,g->av_in,vsize)){changed=1;memcpy(g->av_in,tmp,vsize);}
            g=g->normalout;
        }
    }while(changed);
    if(DEBUG&1024) printf("\n");
    free(tmp);
    if(DEBUG&16384) printf("av heapsize=%lu\n",heapsize);
}
void av_update(struct IC *p,bvtype *isused)
{
  int i,j;
  if((p->z.flags&(VKONST|VAR))==VAR){
    i=p->z.v->index;
    if(p->z.flags&DREFOBJ) i+=vcount-rcount;    
    if(i<0||i>=vcount){
      printf("i=%d\n",i);pric2(stdout,p); ierror(0);
    }
    if(p->z.flags&DREFOBJ){
      if(!(p->z.v->flags&DNOTTYPESAFE))
	BCLR(isused,i);
    }else{
      if(ISSCALAR(p->z.v->vtyp->flags)||(p->code==ASSIGN&&zmeqto(p->q2.val.vmax,szof(p->z.v->vtyp))))
	BCLR(isused,i);
    }
    /*  bei Zuweisung an p wird *p aktiv    */
    if(i<rcount) BSET(isused,i+vcount-rcount);
  }
  for(j=0;j<p->use_cnt;j++){
    i=p->use_list[j].v->index;
    if(p->use_list[j].flags&DREFOBJ) i+=vcount-rcount;
    if(i<0||i>=vcount) continue;
    BSET(isused,i);
  }  
}
/* tests, if IC p uses or modifies var v */
static int var_conflicts(struct Var *v,struct IC *p)
{
  int i;
  for(i=0;i<p->use_cnt;i++)
    if(p->use_list[i].v==v&&!(p->use_list[i].flags&DREFOBJ)) 
      return 1;
  for(i=0;i<p->change_cnt;i++)
    if(p->change_list[i].v==v&&!(p->change_list[i].flags&DREFOBJ)) 
      return 1;
  return 0;
}
int dead_assignments(struct flowgraph *fg)
/*  Findet Zuweisungen, die unnoetig sind, da die Variable nie mehr     */
/*  benutzt werden kann.                                                */
{
    int changed=0;struct IC *p;bvtype *isused;
    int i,j;
    if(DEBUG&1024) printf("searching for dead assignments\n");
    isused=mymalloc(vsize);
    while(fg){
      memcpy(isused,fg->av_out,vsize);
      p=fg->end;
      while(p){
	if(p->z.flags&VAR){
	  i=p->z.v->index;
	  if(p->z.flags&DREFOBJ) i+=vcount-rcount;
	  if(!BTST(isused,i)&&!is_volatile_ic(p)&&!(disable&1)){
	    if(DEBUG&1024){printf("dead assignment deleted:\n");pric2(stdout,p);}
	    if(*p->z.v->identifier&&p->code!=ASSIGN){ err_ic=p;error(170,i>=vcount-rcount?"*":"",p->z.v->identifier);err_ic=0;}
	    if(p->code!=GETRETURN) changed=1;
	    if(p==fg->start){remove_IC_fg(fg,p);break;}
	    p=p->prev;remove_IC_fg(fg,p->next);
	    continue;
	  }
	}
	if(p->code!=SETRETURN&&p->code!=TEST&&p->code!=COMPARE&&(p->q1.flags&VAR)&&!BTST(isused,p->q1.v->index)&&(!(p->z.flags&VAR)||!p->z.v->reg||p->z.v->identifier)){
          struct IC *m,*a;int f=p->q1.flags,dt=p->q1.dtyp;
	  p->q1.flags&=~DREFOBJ;
	  a=p->prev;if(a) m=a->prev; else m=0;
	  if(m&&a&&m->code==ASSIGN&&(a->q1.flags&(VAR|DREFOBJ))==VAR&&!compare_objs(&p->q1,&m->z,0)&&!compare_objs(&a->q1,&a->z,0)&&!compare_objs(&m->q1,&a->z,0)&&(a->q2.flags&KONST)&&!var_conflicts(a->q1.v,p)){
	    if(DEBUG&1024){
	      printf("reorder post-op(q1):\n");
	      pric2(stdout,m);pric2(stdout,a);pric(stdout,p);
	    }
	    p->q1=a->q1;
	    m->next=p;p->prev=m;
	    if(p->next) p->next->prev=a;
	    a->next=p->next;
	    a->prev=p;p->next=a;
	    if(fg->end==p) fg->end=a;
	    if(p==last_ic) last_ic=a;
	    remove_IC_fg(fg,m);
	    av_update(a,isused);
	    p->use_list=myrealloc(p->use_list,(p->use_cnt+a->use_cnt)*VLS);
	    memcpy(&p->use_list[p->use_cnt],a->use_list,a->use_cnt*VLS);
	    p->use_cnt+=a->use_cnt;
	    changed=1;
	    if((f&DREFOBJ)&&p->q1.v->index>=rcount)
	      ierror(0);
	  }
	  p->q1.flags=f;
	  p->q1.dtyp=dt;
	}
	if(p->code!=TEST&&p->code!=COMPARE&&(p->q2.flags&VAR)&&!BTST(isused,p->q2.v->index)&&(!(p->z.flags&VAR)||!p->z.v->reg||p->z.v->identifier)){
          struct IC *m,*a;int f=p->q2.flags,dt=p->q2.dtyp;
	  p->q2.flags&=~DREFOBJ;
	  a=p->prev;if(a) m=a->prev; else m=0;
	  if(m&&a&&m->code==ASSIGN&&(a->q1.flags&(VAR|DREFOBJ))==VAR&&!compare_objs(&p->q2,&m->z,0)&&!compare_objs(&a->q1,&a->z,0)&&!compare_objs(&m->q1,&a->z,0)&&(a->q2.flags&KONST)&&!var_conflicts(a->q1.v,p)){
	    if(DEBUG&1024){
	      printf("reorder post-op(q2):\n");
	      pric2(stdout,m);pric2(stdout,a);pric(stdout,p);
	    }
	    p->q2=a->q1;
	    m->next=p;p->prev=m;
	    if(p->next) p->next->prev=a;
	    a->next=p->next;
	    a->prev=p;p->next=a;
	    if(fg->end==p) fg->end=a;
	    if(p==last_ic) last_ic=a;
	    remove_IC_fg(fg,m);
	    av_update(a,isused);
	    p->use_list=myrealloc(p->use_list,(p->use_cnt+a->use_cnt)*VLS);
	    memcpy(&p->use_list[p->use_cnt],a->use_list,a->use_cnt*VLS);
	    p->use_cnt+=a->use_cnt;
	    changed=1;
	    if((f&DREFOBJ)&&p->q2.v->index>=rcount)
	      ierror(0);
	  }
	  p->q2.flags=f;
	  p->q2.dtyp=dt;
	}
	if(p->code!=TEST&&p->code!=COMPARE&&(p->z.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)&&!BTST(isused,p->z.v->index)){
          struct IC *m,*a;int f=p->z.flags,dt=p->z.dtyp;
	  p->z.flags&=~DREFOBJ;
	  a=p->prev;if(a) m=a->prev; else m=0;
	  if(m&&a&&m->code==ASSIGN&&(a->q1.flags&(VAR|DREFOBJ))==VAR&&!compare_objs(&p->z,&m->z,0)&&!compare_objs(&a->q1,&a->z,0)&&!compare_objs(&m->q1,&a->z,0)&&(a->q2.flags&KONST)&&!var_conflicts(a->q1.v,p)){
	    if(DEBUG&1024){
	      printf("reorder post-op(z):\n");
	      pric2(stdout,m);pric2(stdout,a);pric2(stdout,p);
	      printf("--");
	    }
	    p->z=a->q1;
	    m->next=p;p->prev=m;
	    if(p->next) p->next->prev=a;
	    a->next=p->next;
	    a->prev=p;p->next=a;
	    if(fg->end==p) fg->end=a;
	    if(p==last_ic) last_ic=a;
	    remove_IC_fg(fg,m);
	    av_update(a,isused);
	    p->use_list=myrealloc(p->use_list,(p->use_cnt+a->use_cnt)*VLS);
	    memcpy(&p->use_list[p->use_cnt],a->use_list,a->use_cnt*VLS);
	    p->use_cnt+=a->use_cnt;
	    changed=1;
	    if((f&DREFOBJ)&&p->z.v->index>=rcount)
	      ierror(0);
	  }
	  p->z.flags=f;
	  p->z.dtyp=dt;
	}
	av_update(p,isused);
	if(p==fg->start) break;
	p=p->prev;
      }
      fg=fg->normalout;
    }
    free(isused);
    return(changed);
}


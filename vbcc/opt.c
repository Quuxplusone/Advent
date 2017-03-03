/*  $VER: vbcc (opt.c) V0.8     */
/*  allgemeine Routinen fuer den Optimizer und Steuerung der einzelnen  */
/*  Laeufe                                                              */

#include "opt.h"

static char FILE_[]=__FILE__;

/*  die naechsten Funktionen sollten evtl. in ic.c                  */

/*  Sind use/change-Listen initialisiert?   */
int have_alias;

int static_cse=1,dref_cse=1;

#ifdef ALEX_REG
extern struct flowgraph *pFg;
#endif

void insert_IC(struct IC *p,struct IC *new)
/*  fuegt new hinter p ein; p darf 0 sein                           */
{
  if((new->code==ADDI2P||new->code==SUBIFP||new->code==SUBPFP)&&!ISPOINTER(new->typf2))
    ierror(0);
  new->prev=p;
  if(p){
    new->next=p->next; p->next=new;
  }else{
    new->next=first_ic; first_ic=new; 
  }
  if(new->next) 
    new->next->prev=new; 
  else 
    last_ic=new;
  new->q1.am=new->q2.am=new->z.am=0;
}

#ifndef NO_OPTIMIZER

int gchanged;   /*  Merker, ob Optimierungslauf etwas geaendert hat */
int norek;      /*  diese Funktion wird nicht rekursiv auf          */
int nocall;     /*  diese Funktion kehrt nicht zum Caller zurueck   */

#if HAVE_LIBCALLS
extern np gen_libcall(char *fname,np arg1,struct Typ *t1,np arg2,struct Typ *t2);


/* insert libcalls just before register allocation */
static int insert_libcalls(struct flowgraph *fg)
{
  struct IC *p,*next,*add;
  int replaced=0;
  static struct node n,nl,nr;
  static struct Typ t,tl,tr;
  if(DEBUG&1024) printf("insert_libcalls\n");
  while(fg){
    for(p=fg->start;p;p=next){
      int c=p->code,end=0;
      char *libname;
      next=p->next;
      if((c>=OR&&c<=XOR)||(c>=LSHIFT&&c<=KOMPLEMENT)||c==COMPARE||c==CONVERT||c==MINUS||c==TEST){
	if(libname=use_libcall(c,p->typf,p->typf2)){
	  struct IC *merk_first,*merk_last;
	  static struct node n1,n2;
	  static struct Typ t1,t2;
	  if(DEBUG&1024){
	    printf("converting IC to libcall:\n");
	    pric2(stdout,p);
	  }
	  replaced=1;
	  merk_last=last_ic;
	  merk_first=first_ic;
	  first_ic=last_ic=0;
	  n1.flags=REINTERPRET;
	  n1.o=p->q1;
	  n1.ntyp=&t1;
	  t1.flags=q1typ(p);
	  if(p->q2.flags){
	    n2.flags=REINTERPRET;
	    n2.o=p->q2;
	    n2.ntyp=&t2;
	    t2.flags=q2typ(p);
	    gen_libcall(libname,&n1,&t1,&n2,&t2);
	  }else
	    gen_libcall(libname,&n1,&t1,0,0);
	  if(!last_ic||last_ic->code!=GETRETURN) ierror(0);
	  last_ic->z=p->z;
	  add=first_ic;
	  last_ic=merk_last;
	  first_ic=merk_first;
	  for(;add;add=next){
	    next=add->next;
	    insert_IC_fg(fg,p->prev,add);
	  }
	  next=p->next;
	  if(fg->end==p) end=1;
	  if(p->z.flags){
	    remove_IC_fg(fg,p);
	  }else{
	    struct Typ *t=new_typ();
	    t->flags=INT;
	    p->code=TEST;
	    p->typf=INT;
	    p->q1.flags=VAR;
	    p->q1.v=add_tmp_var(t);
	    p->q1.val.vmax=l2zm(0L);
	    p->prev->z=p->q1;
	    p->q2.flags=p->z.flags=0;
	  }
	}
      }
      if(end||p==fg->end) break;
    }
    fg=fg->normalout;
  }
  return replaced;
}
#endif

/*  temporary fuer verschiedene Bitvektoren */
bvtype *tmp;

int in_varlist(struct varlist *vl,int cnt,struct Var *v,int flags)
{
  int i;
  /*FIXME: slow */
  for(i=0;i<cnt;i++){
    if(vl[i].v==v&&vl[i].flags==flags) return 1;
  }
  return 0;
}

static void add_call_list(void)
{
  struct IC *p;
  for(p=first_ic;p;p=p->next){
    if(p->code==CALL&&p->call_cnt==0){
      if((p->q1.flags&(VAR|DREFOBJ))==VAR){
	p->call_cnt=1;
	p->call_list=mymalloc(sizeof(*p->call_list));
	p->call_list[0].v=p->q1.v;
	p->call_list[0].flags=0;
      }
    }
  }
}

void calc_finfo(struct Var *v,int flags)
{
  struct IC *p;int i,known,maxtyp,t,c;
  /* 128 types should be enough for everyone :-) */
  #define TSIZE BVSIZE(128)
  static bvtype tf[TSIZE];
  if(!v->fi) ierror(0);
  if(flags&CALC_USES){
    if(v->fi->flags&ALL_USES) return;
    free(v->fi->use_list);
    v->fi->use_cnt=0;
    v->fi->use_list=0;
    memset(tf,0,TSIZE);
    known=1;maxtyp=-1;
    for(p=first_ic;p;p=p->next){
      c=p->code;
      if(p->q1.flags&DREFOBJ){
	t=(q1typ(p)&NQ);
	if(t>maxtyp) maxtyp=t;
	BSET(tf,t);
      }
      if(p->q2.flags&DREFOBJ){
	t=(q2typ(p)&NQ);
	if(t>maxtyp) maxtyp=t;
	BSET(tf,t);
      }
      if(c==CALL){
	if((p->q1.flags&(VAR|DREFOBJ))!=VAR||!p->q1.v->fi||!(p->q1.v->fi->flags&ALL_USES)){
	  free(v->fi->use_list);
	  v->fi->use_cnt=0;
	  v->fi->use_list=0;
	  known=0;
	  break;
	}else{
	  for(i=0;i<p->q1.v->fi->use_cnt;i++){
	    if(p->q1.v->fi->use_list[i].v){
	      if(!in_varlist(v->fi->use_list,v->fi->use_cnt,p->q1.v->fi->use_list[i].v,p->q1.v->fi->use_list[i].flags)){
		v->fi->use_cnt++;
		v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(struct varlist));
		v->fi->use_list[v->fi->use_cnt-1]=p->q1.v->fi->use_list[i];
	      }
	    }else{
	      BSET(tf,p->q1.v->fi->use_list[i].flags&NQ);
	    }
	  }
	}
      }else{
	for(i=0;i<p->use_cnt;i++){
	  if(p->use_list[i].v->nesting!=0) continue;
	  if(!in_varlist(v->fi->use_list,v->fi->use_cnt,p->use_list[i].v,p->use_list[i].flags)){
	    v->fi->use_cnt++;
	    v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(struct varlist));
	    v->fi->use_list[v->fi->use_cnt-1]=p->use_list[i];
	  }
	}
      }
    }
    for(c=0;c<=maxtyp;c++){
      if(BTST(tf,c)){
	v->fi->use_cnt++;
	v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(struct varlist));
	v->fi->use_list[v->fi->use_cnt-1].v=0;
	v->fi->use_list[v->fi->use_cnt-1].flags=c;
      }
    }
    if(known) v->fi->flags|=ALL_USES;
  }
  if(flags&CALC_CHANGES){
    if(v->fi->flags&ALL_MODS) return;
    free(v->fi->change_list);
    v->fi->change_cnt=0;
    v->fi->change_list=0;
    memset(tf,0,TSIZE);
    known=1;maxtyp=-1;
    for(p=first_ic;p;p=p->next){
      c=p->code;
      if(p->z.flags&DREFOBJ){
	t=(ztyp(p)&NQ);
	if(t>maxtyp) maxtyp=t;
	BSET(tf,t);
      }
      if(c==CALL){
	if((p->q1.flags&(VAR|DREFOBJ))!=VAR||!p->q1.v->fi||!(p->q1.v->fi->flags&ALL_MODS)){
	  free(v->fi->change_list);
	  v->fi->change_cnt=0;	  
	  v->fi->change_list=0;
	  known=0;
	  break;
	}else{
	  for(i=0;i<p->q1.v->fi->change_cnt;i++){
	    if(p->q1.v->fi->change_list[i].v){
	      if(!in_varlist(v->fi->change_list,v->fi->change_cnt,p->q1.v->fi->change_list[i].v,p->q1.v->fi->change_list[i].flags)){
		v->fi->change_cnt++;
		v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(struct varlist));
		v->fi->change_list[v->fi->change_cnt-1]=p->q1.v->fi->change_list[i];
	      }
	    }else{
	      BSET(tf,p->q1.v->fi->change_list[i].flags&NQ);
	    }
	  }
	}
      }else{
	for(i=0;i<p->change_cnt;i++){
	  if(p->change_list[i].v->nesting!=0) continue;
	  if(!in_varlist(v->fi->change_list,v->fi->change_cnt,p->change_list[i].v,p->change_list[i].flags)){
	    v->fi->change_cnt++;
	    v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(struct varlist));
	    v->fi->change_list[v->fi->change_cnt-1]=p->change_list[i];
	  }
	}
      }
    }
    for(c=0;c<=maxtyp;c++){
      if(BTST(tf,c)){
	v->fi->change_cnt++;
	v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(struct varlist));
	v->fi->change_list[v->fi->change_cnt-1].v=0;
	v->fi->change_list[v->fi->change_cnt-1].flags=c;
      }
    }
    if(known) v->fi->flags|=ALL_MODS;
  }
  if(flags&CALC_CALLS){
    if(v->fi->flags&ALL_CALLS) return;
    free(v->fi->call_list);
    v->fi->call_list=0;
    v->fi->call_cnt=0;
    p=v->fi->opt_ic;
    if(!p)
      p=v->fi->first_ic;
    for(;p;p=p->next){
      if(p->code==CALL){
	if(p->q1.flags&DREFOBJ) return;
	if(!in_varlist(v->fi->call_list,v->fi->call_cnt,p->q1.v,0)){
	  v->fi->call_cnt++;
	  v->fi->call_list=myrealloc(v->fi->call_list,v->fi->call_cnt*sizeof(struct varlist));
	  v->fi->call_list[v->fi->call_cnt-1].v=p->q1.v;
	  v->fi->call_list[v->fi->call_cnt-1].flags=0;
	}
      }
    }
    v->fi->flags|=ALL_CALLS;
  }
}

void used_clist(struct Typ *t,struct const_list *cl)
{
  int i;zmax l;
  if(ISARRAY(t->flags)){
    for(l=l2zm(0L);!zmleq(t->size,l)&&cl;l=zmadd(l,l2zm(1L)),cl=cl->next){
      if(!cl->other){ierror(0);return;}
      used_clist(t->next,cl->other);
    }
    return;
  }
  if(ISUNION(t->flags)){
    used_clist((*t->exact->sl)[0].styp,cl);
    return;
  }
  if(ISSTRUCT(t->flags)){
    struct Typ *st;
    for(i=0;i<t->exact->count&&cl;i++){
      st=(*t->exact->sl)[i].styp;
      if(!(*t->exact->sl)[i].identifier) ierror(0);
      if((*t->exact->sl)[i].identifier[0]){
        if(cl->other) used_clist(st,cl->other);
        cl=cl->next;
      }
    }
    return;
  }
  if(cl->tree&&(cl->tree->o.flags&VAR))
    used_objects(cl->tree->o.v);
  return;
}

void used_objects(struct Var *v)
{
  int i;
  if(v->flags&REFERENCED) return;
  v->flags|=REFERENCED;
  if(ISFUNC(v->vtyp->flags)){
    struct IC *p;
    if(!(v->flags&DEFINED)) return;
    if(!v->fi) ierror(0);
    for(i=0;i<v->fi->call_cnt;i++){
      if(v->fi->call_list[i].v->flags&DEFINED)
	used_objects(v->fi->call_list[i].v);
    }
    /*FIXME: use/change aus fi */
    for(p=v->fi->opt_ic;p;p=p->next){
      if(p->q1.flags&VAR) used_objects(p->q1.v);
      if(p->q2.flags&VAR) used_objects(p->q2.v);
      if(p->z.flags&VAR) used_objects(p->z.v);
    }
  }else{
    if(v->clist) used_clist(v->vtyp,v->clist);
  }
}

zmax recalc_start_offset;

void recalc_offsets(struct flowgraph *fg)
/*  berechnet Offsets fuer auto-Variablen neu und versucht, fuer Variablen, */
/*  die nicht gleichzeitig aktiv sind, den gleichen Platz zu belegen        */
{
  int i,b,*eqto;size_t bsize;zmax *al,*sz;
  bvtype **used,*tmp,*empty;
  struct IC *p;

  if(DEBUG&1024) printf("recalculating offsets\n");
  if(DEBUG&1024) printf("setting up arrays\n");
  /*bsize=(basic_blocks+CHAR_BIT-1)/CHAR_BIT; /* we don's use fg->index but count from 0 */
  bsize=BVSIZE(basic_blocks);
  if(DEBUG&1024) printf("bsize=%lu\n",(unsigned long)bsize);
  tmp=mymalloc(bsize);
  al=mymalloc(sizeof(*al)*(vcount-rcount));
  eqto=mymalloc(sizeof(int)*(vcount-rcount));
  sz=mymalloc(sizeof(*sz)*(vcount-rcount));
  empty=mymalloc(bsize);
  memset(empty,0,bsize);
  used=mymalloc(sizeof(bvtype *)*(vcount-rcount));
  /*  Tabelle, welche Variable in welchem Block belegt ist, aufbauen  */
  for(i=0;i<vcount-rcount;i++){
    if(zmleq(l2zm(0L),vilist[i]->offset)&&(vilist[i]->storage_class==AUTO||vilist[i]->storage_class==REGISTER)){
      if(DEBUG&2048) printf("setting up for %s,%ld\n",vilist[i]->identifier,zm2l(vilist[i]->offset));
      used[i]=mymalloc(bsize);
      memset(used[i],0,bsize);
    }else{
      used[i]=0;
    }
    sz[i]=szof(vilist[i]->vtyp);
    al[i]=falign(vilist[i]->vtyp);
    eqto[i]=-1;
  }
  b=0;
  while(fg){
    if(b>=basic_blocks) ierror(0);
    for(i=0;i<vcount-rcount;i++){
      if(used[i]&&(BTST(fg->av_in,i)||BTST(fg->av_out,i))){
	int r;
	BSET(used[i],b);
	for(r=1;r<=MAXR;r++)
	  if(fg->regv[r]&&fg->regv[r]->index==i) BCLR(used[i],b);
      }
    }
    for(p=fg->start;p;p=p->next){
      if((p->q1.flags&(VAR|REG))==VAR){
	i=p->q1.v->index;
	if(i!=-1){
	  if(i<0||i>=vcount) ierror(0);
	  if(i>=0&&used[i]){
	    BSET(used[i],b);
	  }
	}
      }
      if((p->q2.flags&(VAR|REG))==VAR){
	i=p->q2.v->index;
	if(i!=-1){
	  if(i<0||i>=vcount) ierror(0);
	  if(i>=0&&used[i]){
	    BSET(used[i],b);
	  }
	}
      }
      if((p->z.flags&(VAR|REG))==VAR){
	i=p->z.v->index;
	if(i!=-1){
	  if(i<0||i>=vcount) ierror(0);
	  if(i>=0&&used[i]){
	    BSET(used[i],b);
	  }
	}
      }
      if(p==fg->end) break;
    }
    fg=fg->normalout;
    b++;
  }
  /*  schauen, ob Variablen in gleichen Speicher koennen  */
  if(DEBUG&1024) printf("looking for distinct variables\n");
  for(i=0;i<vcount-rcount;i++){
    if(!used[i]||eqto[i]>=0) continue;
    if(!memcmp(used[i],empty,bsize)){ free(used[i]);used[i]=0;continue;}
    for(b=i+1;b<vcount-rcount;b++){
      if(!used[b]||eqto[b]>=0) continue;
      if(!memcmp(used[b],empty,bsize)){ free(used[b]);used[b]=0;continue;}
      if(DEBUG&2048) printf("comparing %s(%p) and %s(%p)\n",vilist[i]->identifier,(void *)vilist[i],vilist[b]->identifier,(void *)vilist[b]);
      
      memcpy(tmp,used[i],bsize);
      bvintersect(tmp,used[b],bsize);
      if(!memcmp(tmp,empty,bsize)){
	if(DEBUG&1024) printf("memory for %s(%p) and %s(%p) equal\n",vilist[i]->identifier,(void *)vilist[i],vilist[b]->identifier,(void *)vilist[b]);
	eqto[b]=i;
	if(!zmleq(al[b],al[i])) al[i]=al[b];
	if(!zmleq(sz[b],sz[i])) sz[i]=sz[b];
	bvunite(used[i],used[b],bsize);
      }
    }
  }
  if(DEBUG&1024) printf("final recalculating\n");
  max_offset=recalc_start_offset;
  for(i=0;i<vcount-rcount;i++){
    if(!used[i]) continue;
    free(used[i]);
    if(DEBUG&2048) printf("adjusting offset for %s,%ld\n",vilist[i]->identifier,zm2l(vilist[i]->offset));
    if(eqto[i]>=0){
      vilist[i]->offset=vilist[eqto[i]]->offset;
      continue;
    }
    vilist[i]->offset=zmmult(zmdiv(zmadd(max_offset,zmsub(al[i],l2zm(1L))),al[i]),al[i]);
    max_offset=zmadd(vilist[i]->offset,sz[i]);
  }
  recalc_start_offset=l2zm(0L);
  free(used);
  free(sz);
  free(al);
  free(tmp);
  free(empty);
  free(eqto);
}
void remove_IC_fg(struct flowgraph *g,struct IC *p)
/*  Entfernt IC p und beachtet Flussgraph. Ausserdem werden             */
/*  use/change-Listen freigegeben.                                      */
{
  if(p->q1.am||p->q2.am||p->z.am) ierror(0);
  if(have_alias){
    free(p->use_list);
    free(p->change_list);
  }
  if(g->start==g->end){
    g->start=g->end=0;
  }else{
    if(p==g->end) g->end=p->prev;
    if(p==g->start) g->start=p->next;
  }
  remove_IC(p);
}

/* allowed to create new variables */
int early_peephole(void)
{
  struct IC *p;
  int changed,gchanged=0,c,t;
  do{
    changed=0;
    p=first_ic;
    while(p){
      c=p->code;
      t=p->typf;
      if(c==COMPARE&&(p->q2.flags&KONST)&&ISINT(p->typf)){
	struct case_table *ct;
	if((ct=calc_case_table(p,JUMP_TABLE_DENSITY))&&ct->num>1&&ct->density==1){
	  int i;
	  for(i=0;i<ct->num;i++){
	    if(ct->labels[i]!=ct->labels[0])
	      break;
	  }
	  if(i>=ct->num){
	    struct IC *new;
	    static struct Typ ityp;
	    if(DEBUG&1024) printf("converting cases to range-check\n");
	    if(multiple_ccs) ierror(0);
	    new=new_IC();
	    new->code=SUB;
	    new->typf=ct->typf;
	    new->q1=p->q1;
	    new->q2.flags=KONST;
	    if(ct->typf&UNSIGNED)
	      eval_const(&ct->min,UNSIGNED|MAXINT);
	    else
	      eval_const(&ct->min,MAXINT);
	    insert_const(&new->q2.val,ct->typf);
	    new->z.flags=VAR;
	    ityp.flags=ct->typf;
	    new->z.v=add_tmp_var(clone_typ(&ityp));
	    new->z.val.vmax=l2zm(0L);
	    insert_IC(p->prev,new);
	    new=new_IC();
	    new->code=COMPARE;
	    new->q1=p->prev->z;
	    new->z.flags=0;
	    new->q2.flags=KONST;
	    new->typf=(ct->typf|UNSIGNED);
	    gval.vumax=ct->diff;
	    eval_const(&gval,UNSIGNED|MAXINT);
	    insert_const(&new->q2.val,new->typf);
	    insert_IC(p->prev,new);
	    new=new_IC();
	    new->code=BLE;
	    new->typf=ct->labels[0];
	    insert_IC(p->prev,new);
 	    while(p!=ct->next_ic){
	      struct IC *m=p->next;
	      remove_IC(p);
	      p=m;
	    }
	    changed=1;
	    continue;
	  }
	}
      }
      p=p->next;
    }
    gchanged|=changed;
  }while(changed);
  return gchanged;
}

int peephole()
/*  macht alle moeglichen Vereinfachungen/Vereinheitlichungen   */
{
  struct IC *p;struct obj o;int t,c,null,eins,changed,done=0;
  function_calls=0;
  do{
    if(DEBUG&1024) printf("searching for peephole optimizations\n");
    changed=0;ic_count=0;
    p=first_ic;
    while(p){
      c=p->code;
      t=p->typf;
      if(c==NOP&&!p->q1.flags&&!p->q2.flags&&!p->z.flags){
	struct IC *m;
	if(DEBUG&1024) printf("removing nop\n");
	m=p;p=p->next;
	remove_IC(m);
	continue;
      }
      ic_count++;
      if(c==LABEL&&report_suspicious_loops&&p->next&&p->next->code==BRA&&p->next->typf==t){
	error(208);report_suspicious_loops=0;
      }
      if(c==COMPARE&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
	eval_const(&p->q2.val,t);
	if(ISINT(t)&&zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))){
	  struct IC *p2=p->next;
	  if(p2->code==BGE){
	    vmax=l2zm(0L);insert_const(&gval,MAXINT);
	    p->q2.val=gval;p2->code=BGT;changed=1;
	    if(DEBUG&1024) printf("cmp #1 replaced by cmp #0(1)\n");
	  }else if(p2->code==BLT){
	    vmax=l2zm(0L);insert_const(&gval,MAXINT);
	    p->q2.val=gval;p2->code=BLE;changed=1;
	    if(DEBUG&1024) printf("cmp #1 replaced by cmp #0(2)\n");
	  }
	}else if(ISINT(t)&&!(t&UNSIGNED)&&zmeqto(vmax,l2zm(-1L))){
	  struct IC *p2=p->next;
	  if(p2->code==BGT){
	    vmax=l2zm(0L);insert_const(&gval,MAXINT);
	    p->q2.val=gval;p2->code=BGE;changed=1;
	    if(DEBUG&1024) printf("cmp #-1 replaced by cmp #0(1)\n");
	  }else if(p2->code==BLE){
	    vmax=l2zm(0L);insert_const(&gval,MAXINT);
	    p->q2.val=gval;p2->code=BLT;changed=1;
	    if(DEBUG&1024) printf("cmp #-1 replaced by cmp #0\n(2)");
	  }
	}
      }
      if(c>=BEQ&&c<BRA&&p->next&&p->next->code==BRA&&p->typf==p->next->typf){
	struct IC *p2,*m;
	if(DEBUG&1024){
	  printf("removing bcc followed by bra\n");
	  pric2(stdout,p);
	}
	m=p->next;
	p2=p->prev;
	remove_IC(p);
	if(p2&&(p2->code==TEST||p2->code==COMPARE)&&p2->z.flags==0){
	  if(DEBUG&1024){
	    printf("removing comparison:\n");
	    pric2(stdout,p2);
	  }
	  remove_IC(p2);
	}
	p=m;
	changed=1;
	continue;
      }
      if(c==CALL) function_calls++;
      if((p->q1.flags&(KONST|DREFOBJ))==KONST){
	if(((p->q2.flags&(KONST|DREFOBJ))==KONST)||!p->q2.flags){
	  struct IC *old=p->prev;
	  if(fold(p)){ changed=1; p=old;continue;}
	  p=p->next;continue;
	}else{
	  if(c==ADD||c==MULT||(c>=OR&&c<=AND)||(c==COMPARE&&!(p->z.flags)&&p->next&&p->next->code>=BEQ&&p->next->code<BRA)){ /*  const nach rechts   */
	    if(DEBUG&1024){ printf("swapped commutative op:\n");pric2(stdout,p);}
	    o=p->q1;p->q1=p->q2;p->q2=o;
	    if(c==COMPARE){
	      struct IC *br=p->next;
	      if(br->code==BLT) br->code=BGT;
	      else if(br->code==BLE) br->code=BGE;
	      else if(br->code==BGT) br->code=BLT;
	      else if(br->code==BGE) br->code=BLE;
	    }
	  }
	}
      }
      if((p->q2.flags&(KONST|DREFOBJ))==KONST){
	/*  algebraische Optimierungen  */
	eval_const(&p->q2.val,q2typ(p));
	if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))) null=1; else null=0;
	if(zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))&&zldeqto(vldouble,d2zld(1.0))) eins=1; else eins=0;
	if(zmeqto(vmax,l2zm(-1L))&&zldeqto(vldouble,d2zld(-1.0))) eins=-1;
	if(eins<0&&(c==MULT||c==DIV)){
	  if(DEBUG&1024){ printf("MULT/DIV with (-1) converted to MINUS:\n");pric2(stdout,p);}
	  p->code=c=MINUS;p->q2.flags=0;
	  changed=1;
	}

	if(c==LSHIFT||c==RSHIFT){
	  zmax size;
	  size=zmmult(sizetab[q1typ(p)&NQ],char_bit);
	  if(zmleq(size,vmax)){
	    if(c==LSHIFT){
	      if(DEBUG&1024){ printf("lshift converted to ASSIGN 0:\n");pric2(stdout,p);}
	      o.val.vmax=l2zm(0L);eval_const(&o.val,MAXINT);
	      insert_const(&p->q1.val,t);p->q1.flags=KONST;
	      p->code=c=ASSIGN;p->q2.flags=0;p->q2.val.vmax=sizetab[t&NQ];
	      changed=1;
	    }else{
	      if(DEBUG&1024){ printf("rshift changed to maxlength:\n");pric2(stdout,p);}
	      o.val.vmax=zmsub(size,l2zm(1L));eval_const(&o.val,MAXINT);
	      insert_const(&p->q2.val,t);
	      changed=1;	      
	    }
	  }
	}

	if((c==SUB||c==ADD||c==ADDI2P||c==SUBIFP)&&!(q2typ(p)&UNSIGNED)&&zmleq(vmax,l2zm(0L))&&zldleq(vldouble,d2zld(0.0))){
	  struct obj o;int ct=q2typ(p);
	  o=p->q2;
	  calc(MINUS,ct,&o.val,0,&o.val,0);
	  eval_const(&o.val,ct);
	  if(!zmleq(vmax,l2zm(0L))&&!zldleq(vldouble,d2zld(0.0))){
	    calc(MINUS,ct,&o.val,0,&o.val,0);
	    if(!compare_objs(&p->q2,&o,ct)){
	      if(DEBUG&1024){ printf("SUB converted to ADD:\n");pric2(stdout,p);}
	      calc(MINUS,ct,&p->q2.val,0,&p->q2.val,0);
	      if(p->code==ADD) p->code=SUB;
	      else if(p->code==SUB) p->code=ADD;
	      else if(p->code==ADDI2P) p->code=SUBIFP;
	      else if(p->code==SUBIFP) p->code=ADDI2P;
	      c=p->code;
	      changed=1;
	    }
	  }
	}
	if((eins>0&&(c==MULT||c==DIV))||(null&&(c==ADD||c==SUB||c==ADDI2P||c==SUBIFP||c==LSHIFT||c==RSHIFT||c==OR||c==XOR))){
	  if(DEBUG&1024){ printf("operation converted to simple assignment:\n");pric2(stdout,p);}
	  if(c==ADDI2P||c==SUBIFP) p->typf=t=p->typf2;
	  p->code=c=ASSIGN;p->q2.flags=0;p->q2.val.vmax=sizetab[t&NQ];
	  changed=1;
	}
	if(null&&(c==MULT||c==DIV||c==MOD||c==AND)){
	  if(c==DIV||c==MOD){ err_ic=p;error(210);err_ic=0;}
	  if(DEBUG&1024){ printf("operation converted to ASSIGN 0:\n");pric2(stdout,p);}
	  o.val.vmax=l2zm(0L);eval_const(&o.val,MAXINT);
	  insert_const(&p->q1.val,t);p->q1.flags=KONST;
	  p->code=c=ASSIGN;p->q2.flags=0;p->q2.val.vmax=sizetab[t&NQ];
	  changed=1;
	}
	if((ISINT(t)||fp_assoc)&&(c==ADD||c==SUB||c==ADDI2P||c==SUBIFP||c==MULT||c==LSHIFT||c==RSHIFT||c==OR||c==AND)){
	  /*  assoziative Operatoren  */
	  struct IC *n=p->next;
	  int nc,tp,tn;
	  tp=q2typ(p);
	  if(n){
	    nc=n->code;
	    tn=q2typ(n);
	    if(c==ADD&&nc==SUB) nc=ADD;
	    if(c==ADDI2P&&nc==SUBIFP) nc=ADDI2P;
	    if(c==SUB&&nc==ADD) nc=SUB;
	    if(c==SUBIFP&&nc==ADDI2P) nc=SUBIFP;
	  }
	  if(n&&nc==c&&(n->q2.flags&KONST)&&tn==tp&&n->q1.flags==p->z.flags&&n->q1.v==p->z.v&&zmeqto(n->q1.val.vmax,p->z.val.vmax)){
	    if(p->q1.flags==p->z.flags&&p->q1.v==p->z.v&&zmeqto(p->q1.val.vmax,p->z.val.vmax)){
	      if(n->q1.flags==n->z.flags&&n->q1.v==n->z.v&&zmeqto(n->q1.val.vmax,n->z.val.vmax)){
		if(DEBUG&1024){ printf("using associativity(1) with:\n");pric2(stdout,p);pric2(stdout,p->next);}
		n->q1=p->q1;
		if(nc!=n->code){
		  gval.vmax=l2zm(0L);
		  eval_const(&gval,MAXINT);
		  insert_const(&gval,t);
		  calc(SUB,tn,&gval,&n->q2.val,&n->q2.val,0);
		  n->code=nc;
		}
		if(c==LSHIFT||c==RSHIFT||c==ADDI2P||c==SUB||c==SUBIFP)
		  calc(ADD,tp,&p->q2.val,&n->q2.val,&n->q2.val,0);
		else
		  calc(c,tp,&p->q2.val,&n->q2.val,&n->q2.val,0);
		changed=1;
		if(DEBUG&1024) printf("must remove first operation\n");
		n=p;p=p->next;
		if(have_alias){ free(n->use_list); free(n->change_list); }
		remove_IC(n);
		continue;
	      }
	    }else{
	      if(DEBUG&1024){ printf("using associativity(2) with:\n");pric2(stdout,p);pric2(stdout,p->next);}
	      n->q1=p->q1;
	      if(nc!=n->code){
		gval.vmax=l2zm(0L);
		eval_const(&gval,MAXINT);
		insert_const(&gval,t);
		calc(SUB,tn,&gval,&n->q2.val,&n->q2.val,0);
		n->code=nc;
	      }
	      if(c==LSHIFT||c==RSHIFT||c==ADDI2P||c==SUB||c==SUBIFP)
		calc(ADD,tp,&p->q2.val,&n->q2.val,&n->q2.val,0);
	      else
		calc(c,tp,&p->q2.val,&n->q2.val,&n->q2.val,0);
	      changed=1;
	    }
	  }
	}
	if((c==ADDI2P||c==SUBIFP)&&(p->q1.flags&VARADR)){
	  /*  add #var,#const -> move #var+const      */
	  union atyps val;
	  if(DEBUG&1024){printf("add/sub #var,#const changed to assign:\n");pric2(stdout,p);}
	  eval_const(&p->q2.val,t);
	  insert_const(&val,MAXINT);
	  if(c==ADDI2P)
	    calc(ADD,MAXINT,&p->q1.val,&val,&p->q1.val,0);
	  else
	    calc(SUB,MAXINT,&p->q1.val,&val,&p->q1.val,0);
	  p->code=c=ASSIGN;
	  p->q2.flags=0;
	  p->typf=t=p->typf2;
	  p->q2.val.vmax=sizetab[t&NQ];
	  changed=1;
	}
	if((c==ADD||c==SUB)&&ISINT(t)&&p->next&&p->next->next){
	  /*FIXME: using SCRATCH is not nice */
	  struct IC *p1=p->next,*p2=p1->next;
	  if(p1->code==MULT&&p2->code==ADDI2P&&
	     p1->typf==t&&p2->typf==t&&
	     (p1->q2.flags&KONST)&&(p->z.flags&(SCRATCH|DREFOBJ))==SCRATCH&&(p1->z.flags&(SCRATCH|DREFOBJ))==SCRATCH&&
	     !compare_objs(&p->z,&p1->q1,t)&&
	     !compare_objs(&p1->z,&p2->q2,t)){

	    if(DEBUG&1024){ printf("rearranging array-access(1):\n");pric2(stdout,p);pric2(stdout,p1);pric2(stdout,p2);}
	    p1->q1=p->q1;
	    p->q1=p2->q1;
	    p2->q1=p2->z;
	    p->z=p2->z;
	    calc(MULT,t,&p->q2.val,&p1->q2.val,&p->q2.val,0);
	    if(c==ADD) p->code=ADDI2P; else p->code=SUBIFP;
	    p->typf2=p2->typf2;
	    if(!(p->q1.flags&VARADR)){
	      /* for static addresses it is best to add the constant
		 part of the offset first (can be folded); for others
		 the constant part might better be the last, as it can
		 frequentlz be reduced bz addressing-modes */
	      if(DEBUG&1024) printf("reversing\n");
	      p2->q1=p->q1;
	      p->q1=p->z;
	      if(p->prev) p->prev->next=p1;
	      p1->prev=p->prev;
	      if(p==first_ic) first_ic=p1;
	      p->next=p2->next;
	      if(p->next) p->next->prev=p;
	      p->prev=p2;
	      p2->next=p;
	      p=p1;
	    }
	    changed=1;continue;
	  }
	}
      }
      if(p->q1.flags&KONST){
	/*  algebraische Optimierungen  */
	eval_const(&p->q1.val,t);
	if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))) null=1; else null=0;
	if(null&&(c==DIV||c==MOD||c==LSHIFT||c==RSHIFT)){
	  if(DEBUG&1024){ printf("operation converted to ASSIGN 0:\n");pric2(stdout,p);}
	  o.val.vmax=l2zm(0L);eval_const(&o.val,MAXINT);
	  insert_const(&p->q1.val,t);p->q1.flags=KONST;
	  p->code=c=ASSIGN;p->q2.flags=0;p->q2.val.vmax=sizetab[t&NQ];
	  changed=1;
	}
      }
      if(!USEQ2ASZ&&p->z.flags&&!compare_objs(&p->q2,&p->z,p->typf)){
	if(c==ADD||c==MULT||(c>=OR&&c<=AND)){
	  struct obj o;
	  if(DEBUG&1024){printf("swapping objs because USEQ2ASZ\n");pric2(stdout,p);}
	  o=p->q2;p->q2=p->q1;p->q1=o;
	  /*  kein changed hier!  */
	}else{pric2(stdout,p); ierror(0);}
      }
      if((c==ADD||c==SUB)&&p->next){
	/*FIXME: using SCRATCH is not nice */
	struct IC *p1=p->next;
	if(p1->code==ADDI2P&&p1->typf==t&&(p->z.flags&(SCRATCH|DREFOBJ))==SCRATCH&&!compare_objs(&p->z,&p1->q2,t)){
	  if(DEBUG&1024){ printf("rearranging array-access(2):\n");pric2(stdout,p);pric2(stdout,p1);}
	  p1->q2=p->q1;
	  p->q1=p1->q1;
	  p->z=p1->z;
	  p1->q1=p1->z;
	  if(c==ADD) p->code=c=ADDI2P; else p->code=c=SUBIFP;
	  p->typf2=p1->typf2;
	  changed=1;continue;
	}
      }
      if((c==SUB||c==DIV||c==MOD)&&!compare_objs(&p->q1,&p->q2,p->typf)){
	/*  x-x=0, x/x=1, x%x=0 */
	if(DEBUG&1024){ printf("i-i, i/i, i%%i converted to ASSIGN 0/1:\n");pric2(stdout,p);}
	if(c==DIV) o.val.vmax=l2zm(1L); else o.val.vmax=l2zm(0L);
	eval_const(&o.val,MAXINT);insert_const(&p->q1.val,t);p->q1.flags=KONST;
	p->code=c=ASSIGN;p->q2.flags=0;p->q2.val.vmax=sizetab[t&NQ];
	changed=1;
      }
      if(c==ASSIGN&&(p->z.flags&VAR)&&p->z.flags==p->q1.flags&&p->z.v==p->q1.v&&zmeqto(p->z.val.vmax,p->q1.val.vmax)){
	struct IC *d;
	if(DEBUG&1024){ printf("removing redundant move:\n");pric2(stdout,p);}
	changed=1;
	d=p; p=p->next;
	if(have_alias){ free(d->use_list); free(d->change_list);}
	remove_IC(d); continue;
      }
      if(c>=BEQ&&c<=BGT){
	struct IC *p2=p->prev;
	if(p2&&p2->code==COMPARE&&!compare_objs(&p->q1,&p2->z,0)){
	  struct IC *p3=p2->prev;
	  if(p3&&p3->code==c){
	    struct IC *p4=p3->prev;
	    if(p4->code==COMPARE&&!compare_objs(&p3->q1,&p4->z,0)
	       &&!compare_objs(&p2->q1,&p4->q1,p4->typf)&&!compare_objs(&p2->q2,&p4->q2,p4->typf)){
	      if(DEBUG&1024){printf("removing redundant compare\n");pric2(stdout,p2);pric2(stdout,p);}
	      p->code=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	      p->typf=0;
	      p2->code=NOP;
	      p2->typf=0;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      changed=1;
	    }
	  }
	}
      }

      p=p->next;
    }
    if(changed) done|=changed;
    gchanged|=changed;
  }while(changed);
  return done;
}

void insert_loads()
/*  Laedt Speicher in temporaere Variablen */
{
  struct IC *p,*new;
  struct Typ t={0},v={VOID};
  int c;
  if(DEBUG&1024) printf("insert_loads()\n");
  for(p=first_ic;p;p=p->next){
    c=p->code;
    if(p->q2.flags||c==PUSH){
      if((dref_cse&&(p->q1.flags&DREFOBJ))||((p->q1.flags&(VAR|VARADR))==VAR&&(p->q1.v->storage_class==EXTERN||p->q1.v->storage_class==STATIC))){
        new=new_IC();
        new->code=ASSIGN;
	new->typf=q1typ(p);
	if(ISSCALAR(new->typf)){
	  new->q1.am=new->q2.am=new->z.am;
	  new->q1=p->q1;
	  new->q2.flags=0;
	  new->q2.val.vmax=sizetab[new->typf&NQ];
	  new->z.flags=VAR;
	  new->z.val.vmax=l2zm(0L);
	  t.flags=new->typf;
	  if(ISPOINTER(new->typf)) t.next=&v; else t.next=0;
	  new->z.v=add_tmp_var(clone_typ(&t));
	  insert_IC(p->prev,new);
	  p->q1=new->z;
	}else
	  free(new);
      }
      if((dref_cse&&(p->q2.flags&DREFOBJ))||(static_cse&&(p->q2.flags&(VAR|VARADR))==VAR&&(p->q2.v->storage_class==EXTERN||p->q2.v->storage_class==STATIC))){
        new=new_IC();
        new->code=ASSIGN;
	new->typf=q2typ(p);
	if(ISSCALAR(new->typf)){
	  new->q1.am=new->q2.am=new->z.am;
	  new->q1=p->q2;
	  new->q2.flags=0;
	  new->q2.val.vmax=sizetab[new->typf&NQ];
	  new->z.flags=VAR;
	  new->z.val.vmax=l2zm(0L);
	  t.flags=new->typf;
	  if(ISPOINTER(new->typf)) t.next=&v; else t.next=0;
	  new->z.v=add_tmp_var(clone_typ(&t));
	  insert_IC(p->prev,new);
	  p->q2=new->z;
	}else
	  free(new);
      }
      if(p->q2.flags&&((dref_cse&&(p->z.flags&DREFOBJ))||(static_cse&&(p->z.flags&(VAR|VARADR))==VAR&&(p->z.v->storage_class==EXTERN||p->z.v->storage_class==STATIC)))){
	/* translate x op y -> mem to x op y -> tmp; move tmp -> mem */
        new=new_IC();
        new->code=ASSIGN;
	new->typf=ztyp(p);
	if(ISSCALAR(new->typf)){
	  new->q1.am=new->q2.am=new->z.am;
	  new->z=p->z;
	  new->q2.flags=0;
	  new->q2.val.vmax=sizetab[new->typf&NQ];
	  new->q1.flags=VAR;
	  new->q1.val.vmax=l2zm(0L);
	  t.flags=new->typf;
	  if(ISPOINTER(new->typf)) t.next=&v; else t.next=0;
	  new->q1.v=add_tmp_var(clone_typ(&t));
	  insert_IC(p,new);
	  p->z=new->q1;
	}else
	  free(new);
      }
    }
  }
}

void insert_ccs(void)
/*  Fuegt Variablen fuer ccs ein.   */
{
    struct IC *p; struct Var *v; struct Typ *t;
    if(DEBUG&1024) printf("insert_ccs()\n");
    for(p=first_ic;p;p=p->next){
        if(p->code==COMPARE||p->code==TEST){
            p->z.flags=VAR;
            p->z.val.vmax=l2zm(0L);
            t=new_typ();
            t->flags=0;
            v=add_tmp_var(t);
            p->z.v=v;
            p=p->next;
            if(p->code<BEQ||p->code>BGT){
                p=p->prev;
                p->code=NOP;
                p->q1.flags=p->q2.flags=p->z.flags=0;
		p->typf=0;
            }else{
                p->q1.flags=VAR;
                p->q1.val.vmax=l2zm(0L);
                p->q1.v=v;
            }
        }
    }
}

static int to_be_inlined(struct Var *v)
{
  /* decide whether function should be inlined */
  int c;struct IC *p;
  /* no code available */
  if(!v->fi||!v->fi->first_ic) return 0;
  /* returns something not in a register (FIXME) */
  if(!ffreturn(v->vtyp->next)&&(v->vtyp->next->flags&NQ)!=VOID) return 0;
  /* varargs function */
  if((c=v->vtyp->exact->count)!=0&&(*v->vtyp->exact->sl)[c-1].styp->flags!=VOID)
    return 0;
  /* uses variable length arrays */
  if(v->fi->flags&USES_VLA) return 0;
  for(c=0,p=v->fi->first_ic;p;p=p->next) c++;
  /* try to always inline functions specified as inline */
  if(v->flags&INLINEFUNC)
    return 1;
  if(c>inline_size) return 0;
  /* we assume that inlining saves size if the number of arguments
     is larger than the number of ICs + CALL +SETRETURN  */
  if(optsize&&c-2>(c=v->vtyp->exact->count)) return 0;
  return 1;
}

static void cross_module_inline(void)
{
  struct IC *p,*np,*new,*ip,*cp,*getreturn;
  struct Var *v,*vp;
  int i,c,firstl,lastl;
  if(DEBUG&1024) printf("cross_module_inline()\n");
  for(p=first_ic;p;){
    np=p->next;
    if(p->code==CALL&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&to_be_inlined(p->q1.v)){
      zmax pushed=p->q2.val.vmax;
      struct IC *gr;
      v=p->q1.v;
      if(DEBUG&1024){
	printf("inlining call to %s\n",p->q1.v->identifier);
	for(vp=v->fi->vars;vp;vp=vp->next)
	  printf("%s(%ld)/%p\n",vp->identifier,zm2l(vp->offset),(void*)vp);
      }
      gr=p->next;
      while(gr&&gr->code==NOP) gr=gr->next;
      if(gr&&gr->code==GETRETURN){
	getreturn=gr;
	np=getreturn->next;
      }else{
	getreturn=0;
      }
      /*  Kopien der Variablen erzeugen   */
      for(vp=v->fi->vars;vp;vp=vp->next){
	vp->inline_copy=0;
      }
      cp=p;
      /* find first and last label used */
      firstl=lastl=0;
      for(ip=v->fi->first_ic;ip;ip=ip->next){
	if(ip->code>=LABEL&&ip->code<=BRA){
	  if(firstl==0||ip->typf<firstl) firstl=ip->typf;
	  if(lastl==0||ip->typf>lastl) lastl=ip->typf;
	}
      }
      /* Argumente einfuegen */
      vp=v->fi->vars;
      for(i=0;i<p->arg_cnt;i++){
	while(vp&&(!*vp->identifier||(zmleq(l2zm(0L),vp->offset)&&!vp->reg)||(vp->storage_class!=REGISTER&&vp->storage_class!=AUTO))) vp=vp->next;
	if(!vp)
	  continue;
	if(DEBUG&1024) printf("arg nr. %d to var <%s>\n",i,vp->identifier);
	vp->inline_copy=add_tmp_var(clone_typ(vp->vtyp));
	ip=p->arg_list[i];
	if(ip->code==ASSIGN){
	  /* find and delete nop for register passing */
	  struct IC *nop;
	  if(!(ip->z.flags&VAR)||ip->z.v->reg==0) ierror(0);
	  for(nop=ip->next;nop;nop=nop->next){
	    if(nop->code==NOP&&(nop->q1.flags&(VAR|DREFOBJ))==VAR&&nop->q1.v==ip->z.v)
	      break;
	  }
	  if(!nop) ierror(0);
	  if(np==nop) np=nop->next;
	  remove_IC(nop);
	}else if(ip->code==PUSH){
	  ip->code=ASSIGN;
	  pushed=zmsub(pushed,ip->q2.val.vmax);
	}else{
	  pric2(stdout,ip);
	  ierror(0);
	}
	ip->typf&=~VOLATILE;
	ip->z.flags=VAR;
	ip->z.v=vp->inline_copy;
	if((ip->z.v->vtyp->flags&NU)!=(ip->typf&NU)){
	  /* have to convert */
	  ip->typf2=ip->typf;
	  ip->typf=ip->z.v->vtyp->flags;
	  ip->code=CONVERT;
	}	  
	ip->z.val.vmax=l2zm(0L);
	vp=vp->next;
      }

      if(!zmeqto(pushed,l2zm(0L))){
	/* ueberfluessige PUSHs entfernen */
	struct IC *m;
	ip=p->prev;
	while(1){
	  m=ip->prev;
	  if(ip->code==PUSH){
	    if(0/*!(c_flags[26]&USEDFLAG)&&ip->q1.flags*/) ierror(0);
	    if(!zmleq(ip->q2.val.vmax,pushed)) ierror(0);
	    pushed=zmsub(pushed,ip->q2.val.vmax);
	    if(np==ip) np=np->next;
	    remove_IC(ip);
	    if(zmeqto(pushed,l2zm(0L))) break;
	  }
	  if(!m) ierror(0);
	  ip=m;
	}
      }

      /*  Code einfuegen und Labels umschreiben   */
      ip=v->fi->first_ic;
      while(ip){
	struct Var *iv;
	int c;
	new=new_IC();
	*new=*ip;
	ip->copy=new;
	c=ip->code;
	/*  evtl. ist ein IC praktisch ein SETRETURN, falls das */
	/*  Rueckgabeziel ueber Parameterzeiger angespr. wird   */
	if(ip->z.flags&VAR){
	  iv=ip->z.v;
	  if(iv->storage_class==AUTO||iv->storage_class==REGISTER){
	    if(0/*!*iv->identifier&&zmeqto(iv->offset,l2zm(0L))*/){
	      if(getreturn){
		new->z=getreturn->z;
	      }else{
		new->code=NOP;
		new->q1.flags=new->q2.flags=new->z.flags=0;
		new->typf=0;
	      }
	    }else{
	      if(!iv->inline_copy){
		iv->inline_copy=add_tmp_var(clone_typ(iv->vtyp));
		iv->inline_copy->reg=iv->reg;
	      }
	      new->z.v=iv->inline_copy;
	    }/*else if(iv->inline_copy) ierror(0);*/
	  }
	}
	/*  Kopien aller auto/register Variablen erzeugen   */
	if(ip->q1.flags&VAR){
	  iv=ip->q1.v;
	  if(iv->storage_class==AUTO||iv->storage_class==REGISTER){
	    if(!iv->inline_copy){
	      iv->inline_copy=add_tmp_var(clone_typ(iv->vtyp));
	      iv->inline_copy->reg=iv->reg;
	    }
	    new->q1.v=iv->inline_copy;
	    if(c==ADDRESS) new->q1.v->flags|=USEDASADR;
	  }/*else if(iv->inline_copy) ierror(0);*/
	}
	if(ip->q2.flags&VAR){
	  iv=ip->q2.v;
	  if(iv->storage_class==AUTO||iv->storage_class==REGISTER){
	    if(!iv->inline_copy){
	      iv->inline_copy=add_tmp_var(clone_typ(iv->vtyp));
	      iv->inline_copy->reg=iv->reg;
	    }
	    new->q2.v=iv->inline_copy;
	  }/*else if(iv->inline_copy) ierror(0);*/
	}
	if(c==CALL){
	  int i;
	  function_calls+=1;
	  new->arg_list=mymalloc(sizeof(*new->arg_list)*new->arg_cnt);
	  for(i=0;i<new->arg_cnt;i++) new->arg_list[i]=ip->arg_list[i]->copy;
	}
	if(c>=LABEL&&c<=BRA){
	  new->typf+=label+1-firstl;
	}
	if(c==SETRETURN){
	  if(getreturn){
	    new->code=ASSIGN;
	    new->z=getreturn->z;
	  }else{
	    new->code=NOP;
	    new->q1.flags=new->q2.flags=new->z.flags=0;
	    new->typf=0;
	  }
	}
	new->q1.flags&=~SCRATCH;
	new->q2.flags&=~SCRATCH;
	new->z.flags&=~SCRATCH;
	if(new->code==ASSIGN&&!new->q1.flags) ierror(0);
	insert_IC(cp,new);
	cp=new;
	ip=ip->next;
      }

      label+=lastl-firstl+1;
      remove_IC(p);
      if(getreturn) remove_IC(getreturn);
    }
    p=np;
  }
}

#endif
#define FREEAV free(av_globals);free(av_statics);free(av_drefs);free(av_address);
void optimize(long flags,struct Var *function)
/*  flags:   1=Register, 2=optimize, 4=cse/cp, 8=constant_propagation,  */
/*          16=dead_assignments, 32=global-optimizations                */
/*          64=blockweise Registervergabe, 128=loop_optimizations (nur  */
/*             in Verbindung mit 32), 256=recalc_offsets                */
{
#ifndef NO_OPTIMIZER
  struct flowgraph *g,*fg=0;
  int r,i,pass=0,mustrepeat,intask;
  int lc_freed,lc_done=0;
  if(!function) ierror(0);
  norek=nocall=0;
  report_suspicious_loops=report_weird_code=1;
  if(!strcmp(function->identifier,"main")){norek=1;nocall=1;}
  /*  falls main() rekursiv aufgerufen werden kann, muss nomain==0 sein   */
  
#else

  flags&=1;

#endif

  intask=(function->vattr&&strstr(function->vattr,"taskprio("));

  if(flags&2){
#ifndef NO_OPTIMIZER
    if(DEBUG&1024) printf("\nOptimizing function %s\n",function->identifier);
    /*  Variablen fuer ccs einsetzen.   */
    if(cross_module&&(flags&4096)){
      int i;
      for(i=0;i<inline_depth;i++)
	cross_module_inline();
    }
    if(multiple_ccs) insert_ccs();
    /*  Speicherzugriffe in extra tmp-Vars umleiten, wegen cse */
    if(flags&4) insert_loads();
    /*  nur ein pass, wenn nur lokale Optimierungen */
    if(!(flags&32)) maxoptpasses=1;
    do{
      gchanged=0;pass++;
      av_globals=av_statics=av_address=av_drefs=0;
      ae_globals=ae_statics=ae_address=ae_drefs=0;
      cp_globals=cp_statics=cp_address=cp_drefs=0;
      dlist=0;vilist=0;elist=0;
      
      if(DEBUG&1024) printf("\noptimizer (function %s) pass %d\n",function->identifier,pass);
      early_peephole();
      num_vars();
      peephole();
      fg=jump_optimization();
      create_alias(fg);
      if(DEBUG&2048) print_vi();
      if(flags&8){
#if 0
	do{
	  num_defs();
	  if(flags&32){
	    reaching_definitions(fg);
	    if(DEBUG&1024) print_flowgraph(fg);
	    r=constant_propagation(fg,1);
	  }else{
	    r=constant_propagation(fg,0);
	  }
	  if(DEBUG&1024) {printf("constant_propagation returned %d\n",r);print_flowgraph(fg);}
	  if(r){
	    if(peephole()){free_flowgraph(fg);fg=jump_optimization();}
	  }
	  if(r&2){
	    if(DEBUG&1024) printf("must repeat num_vars\n");
	    free(vilist);
	    FREEAV;
	    num_vars();
	  }
	}while(r);
#endif
	num_defs();
	mustrepeat=0;
	if(DEBUG&1024) printf("trying constant propagation\n");
	if(DEBUG&1024) print_flowgraph(fg);
	if((flags&32)&&!(disable&4))
	  reaching_definitions(fg);
	for(g=fg;g;g=g->normalout){
	  do{
	    r=constant_propagation(g,(flags&32)&&!(disable&4));
	    gchanged|=r;
	    if(r){
	      struct IC *p;
	      if(r&2) mustrepeat=1;
	      r=0;
	      for(p=g->start;p;p=p->next){
		if((p->q1.flags&(KONST|DREFOBJ))==KONST&&(!p->q2.flags||(p->q2.flags&(KONST|DREFOBJ))==KONST)){
		  if(p->code!=TEST&&p->code!=COMPARE&&fold(p)) r=1;
		}
		if(p==g->end) break;
	      }
	    }
	  }while(r);
	}
	free(rd_matrix);
	free(var_defs);
	free(defs_kill);
	free(defs_gen);
	free(dlist);
	free_flowgraph(fg);
	if(mustrepeat){
	  if(DEBUG&1024) printf("must repeat num_vars()\n");
	  free(vilist);
	  FREEAV;
	  early_peephole();
	  num_vars();
	}
	peephole();
	fg=jump_optimization();
      }
      if(flags&4){
	int repeat;
	do{
	  do{
	    num_exp();
	    if(DEBUG&1024) print_flowgraph(fg);
	    if(disable&32)
	      repeat=r=0;
	    else
	      repeat=r=cse(fg,0);    /*  local cse   */
	    if(DEBUG&1024) printf("local cse returned %d\n",r);
	    gchanged|=r;
	    if(r){  /*  neue Variablen eingefuegt   */
	      if(DEBUG&1024) printf("must repeat num_vars\n");
	      free(vilist);
	      FREEAV;
	      num_vars();
	    }
	    do{
	      num_copies();
	      if(DEBUG&1024) print_flowgraph(fg);
	      if(disable&128)
		r=0;
	      else
		r=copy_propagation(fg,0);   /*  copy propagation    */
	      if(DEBUG&1024) printf("local copy propagation returned %d\n",r);
	      if(r&2){
		if(DEBUG&1024) printf("must repeat num_vars\n");
		free(vilist);
		FREEAV;
		num_vars();
	      }
	      gchanged|=r;repeat|=r;
	    }while(r);
	  }while(repeat);
	  repeat=0;
	  if(flags&32){
	    num_exp();
	    if(DEBUG&1024) print_flowgraph(fg);
	    available_expressions(fg);
	    if(DEBUG&1024) print_flowgraph(fg);
	    if(disable&48)
	      r=0;
	    else
	      r=cse(fg,1);
	    gchanged|=r;repeat|=r;
	    if(DEBUG&1024) printf("global cse returned %d\n",r);
	    if(r){  /*  neue Variablen eingefuegt   */
	      if(DEBUG&1024) printf("must repeat num_vars\n");
	      free(vilist);
	      FREEAV;
	      num_vars();
	      gchanged|=r;repeat|=r;
	      do{
		num_copies();
		if(DEBUG&1024) print_flowgraph(fg);
		if(disable&128)
		  r=0;
		else
		  r=copy_propagation(fg,0);   /*  copy propagation    */
		if(DEBUG&1024) printf("local copy propagation returned %d\n",r);
		if(r&2){
		  if(DEBUG&1024) printf("must repeat num_vars\n");
		  free(vilist);
		  FREEAV;
		  num_vars();
		}
		gchanged|=r;repeat|=r;
	      }while(r);
	    }
	    num_copies();
	    available_copies(fg);
	    if(DEBUG&1024) print_flowgraph(fg);
	    if(disable&192)
	      r=0;
	    else
	      r=copy_propagation(fg,1);   /*  copy propagation    */
	    if(DEBUG&1024) printf("global copy propagation returned %d\n",r);
	    if(r&2){
	      if(DEBUG&1024) printf("must repeat num_vars\n");
	      free(vilist);
	      FREEAV;
	      num_vars();
	    }
	    gchanged|=r;repeat|=r;
	  }
	}while(0/*repeat*/);
      }
      if((flags&160)==160){
	mark_eff_ics();

	r=loop_optimizations(fg);
	gchanged|=r;
	fg=jump_optimization();
      }
      if((flags&16)||((flags&1)&&pass>=maxoptpasses)){
	/*                num_vars();*/
	free_alias(fg);
	create_alias(fg);
	active_vars(fg);
	if(DEBUG&1024) print_flowgraph(fg);
	if((flags&16)&&pass<=maxoptpasses){
	  r=dead_assignments(fg);
	  if(DEBUG&1024) printf("dead_assignments returned %d\n",r);
	  gchanged|=r;
	}
      }


#if HAVE_LIBCALLS
      /* if no further optimizations are found, insert libcalls
	 and look some more */
      if(!lc_done&&(!gchanged||pass>=maxoptpasses)){
	int r;
	r=insert_libcalls(fg);
	lc_done=1;
	if(r){
	  gchanged|=1;
	  pass--;
	}
      }
#endif
      
      if((!gchanged||pass>=maxoptpasses)){
	/*  Funktion evtl. fuer inlining vorbereiten und    */
	/*  Registervergabe                                 */
	int varargs=0,c;
	if((c=function->vtyp->exact->count)!=0&&(*function->vtyp->exact->sl)[c-1].styp->flags!=VOID)
	  varargs=1;
	
	/*  default-Wert fuer inline-Entscheidung   */
	if(!cross_module&&!varargs&&(flags&4096)&&(only_inline||ic_count<=inline_size)){
	  /*  fuer function inlining vorbereiten   */
	  struct IC *p,*new;
	  if(DEBUG&1024) printf("function <%s> prepared for inlining(ic_count=%d)\n",function->identifier,ic_count);
	  if(!function->fi) function->fi=new_fi();
	  function->fi->first_ic=first_ic;
	  function->fi->last_ic=last_ic;
	  first_ic=last_ic=0;
	  p=function->fi->first_ic;
	  while(p){
	    new=new_IC();
	    memcpy(new,p,ICS);
	    p->copy=new;
	    if((p->code>=BEQ&&p->code<=BRA)||p->code==LABEL)
	      new->typf-=lastlabel;
	    new->q1.flags&=~SCRATCH;
	    new->q2.flags&=~SCRATCH;
	    new->z.flags&=~SCRATCH;
	    add_IC(new);
	    new->file=p->file;
	    new->line=p->line;
	    if(new->code==CALL){
	      int i;
	      new->arg_list=mymalloc(sizeof(*new->arg_list)*new->arg_cnt);
	      for(i=0;i<new->arg_cnt;i++) new->arg_list[i]=p->arg_list[i]->copy;
	    }
	    p=p->next;
	  }
	  p=first_ic;first_ic=function->fi->first_ic;function->fi->first_ic=p;
	  p=last_ic;last_ic=function->fi->last_ic;function->fi->last_ic=p;
	  function->fi->vars=0;
	}
	
	if(cross_module) calc_finfo(function,CALC_USES|CALC_CHANGES);
       

	if(flags&1){
	  local_combine(fg);
	  if(DEBUG&1024) print_flowgraph(fg);
	  if(coloring){
#ifdef ALEX_REG
	    if(DEBUG&1024) print_flowgraph(fg);
	    loops(fg,1);
	    num_defs();
	    reaching_definitions(fg);
	    GCAssignRegs(fg,function->identifier);
	    { 
	      int stack_used;struct IC *p;
#if 1
	      for(p=pFg->start;p;p=p->next){
		if((p->q1.flags&(REG|VAR))==VAR&&zmleq(l2zm(0L),p->q1.v->offset))
		  break;
		if((p->q2.flags&(REG|VAR))==VAR&&zmleq(l2zm(0L),p->q2.v->offset))
		  break;
		if((p->z.flags&(REG|VAR))==VAR&&zmleq(l2zm(0L),p->z.v->offset))
		  break;
	      }
	      if(!p) max_offset=l2zm(0L);
#endif
	    }
	      
	    fg = pFg;
#else
	    ierror(0);
#endif
	  }else{
	    local_regs(fg);
	    if(DEBUG&1024) print_flowgraph(fg);
	    loops(fg,1);
	    if(DEBUG&1024) print_flowgraph(fg);
	    fg=create_loop_headers(fg,1);
	    if(DEBUG&1024) print_flowgraph(fg);
	    fg=create_loop_footers(fg,1);
	    if(DEBUG&1024) print_flowgraph(fg);
	    if(!(disable&8192))
	      create_const_vars(fg);
	    loop_regs(fg,intask);
	    if(DEBUG&1024) print_flowgraph(fg);
#if 0
	    if(flags&64){
	      block_regs(fg);
	      if(DEBUG&1024) print_flowgraph(fg);
	    }
#endif
	    insert_regs(fg);
	    insert_saves(fg);
	    if(flags&256) recalc_offsets(fg);
	    free_const_vars();
	  }
	}
      }

      free_alias(fg);
      free_flowgraph(fg);
      free(vilist);
      FREEAV;

      if((flags&32)&&gchanged&&pass>=maxoptpasses) error(172,maxoptpasses);

    }while(gchanged&&pass<maxoptpasses);
    /*  nur, um nochmal ueberfluessige Labels zu entfernen  */
    fg=construct_flowgraph();
    free_flowgraph(fg);
    
#endif

  }else{
    /*  keine Optimierungen     */
    if(flags&1) simple_regs();
    load_simple_reg_parms();
  }
  add_call_list();
  lastlabel=label;
}




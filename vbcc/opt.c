/*  $VER: vbcc (opt.c) $Revision: 1.48 $    */
/*  allgemeine Routinen fuer den Optimizer und Steuerung der einzelnen  */
/*  Laeufe                                                              */

#include "opt.h"
#include "vbc.h"

static char FILE_[]=__FILE__;

/*  die naechsten Funktionen sollten evtl. in ic.c                  */

/*  Sind use/change-Listen initialisiert?   */
int have_alias;

int static_cse=1,dref_cse=1;
int no_eff_ics,early_eff_ics;

#ifdef ALEX_REG
extern flowgraph *pFg;
#endif

void insert_IC(IC *p,IC *new)
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

void fix_shortop(flowgraph *fg,IC *p)
{
  if((p->typf&NQ)<INT&&!shortcut(p->code,p->typf)){
    static type ta;
    type *t;
    IC *new;
    if(DEBUG&1024){puts("fix shortop");pric2(stdout,p);puts("to;");}
    ta.flags=int_erw(p->typf);
    t=arith_typ(&ta,&ta);
    new=new_IC();
    new->code=CONVERT;
    new->q1=p->q1;
    new->z.flags=VAR;
    new->z.v=add_tmp_var(clone_typ(t));
    new->typf2=p->typf|UNSIGNED;
    new->typf=t->flags|UNSIGNED;
    p->q1=new->z;
    insert_IC_fg(fg,p->prev,new);
    new=new_IC();
    new->code=CONVERT;
    new->q1=p->q2;
    new->z.flags=VAR;
    new->z.v=add_tmp_var(clone_typ(t));
    new->typf2=p->typf|UNSIGNED;
    new->typf=t->flags|UNSIGNED;
    p->q2=new->z;
    insert_IC_fg(fg,p->prev,new);
    new=new_IC();
    new->code=CONVERT;
    new->z=p->z;
    new->q1.flags=VAR;
    new->q1.v=add_tmp_var(t);
    new->typf=p->typf|UNSIGNED;
    new->typf2=t->flags|UNSIGNED;
    p->z=new->q1;
    insert_IC_fg(fg,p,new);   
    p->typf=t->flags|UNSIGNED;
    if(DEBUG&1024){
      pric2(stdout,p->prev->prev);
      pric2(stdout,p->prev);
      pric2(stdout,p);
      pric2(stdout,p->next);
    }
  }
}

#if HAVE_LIBCALLS
extern np gen_libcall(char *fname,np arg1,type *t1,np arg2,type *t2);


/* insert libcalls just before register allocation */
static int insert_libcalls(flowgraph *fg)
{
  IC *p,*next,*add;
  int replaced=0;
  static node n,nl,nr;
  static type t,tl,tr;
  if(DEBUG&1024) printf("insert_libcalls\n");
  while(fg){
    for(p=fg->start;p;p=next){
      int c=p->code,end=0;
      char *libname;
      next=p->next;
      if((c>=OR&&c<=AND)||(c>=LSHIFT&&c<=KOMPLEMENT)||c==COMPARE||c==CONVERT||c==MINUS||c==TEST){
	if(libname=use_libcall(c,p->typf,p->typf2)){
	  IC *merk_first,*merk_last;
	  static node n1,n2;
	  static type t1,t2;
	  if(DEBUG&1024){
	    printf("converting IC to libcall:\n");
	    pric2(stdout,p);
	  }
#if FIXED_SP
	  {
	    /* move nested pushs */
	    IC *m;flowgraph *g=fg;int fd=0,i;
	    for(i=0;i<p->arg_cnt;i++){
	      m=p->arg_list[i];
	      if(m->code==PUSH){
		Var *v;type *t;IC *new;
		fd=1;
		if(DEBUG&1024){
		  printf("moving push\n");
		  pric2(stdout,m);
		}
		if((m->typf&NQ)==STRUCT) ierror(0);
		if((m->typf&NQ)==CHAR&&!zmeqto(m->q2.val.vmax,Z1)) ierror(0);
		t=new_typ();
		t->flags=m->typf;
		v=add_tmp_var(t);
		new=new_IC();
		new->code=PUSH;
		new->q2.val.vmax=m->q2.val.vmax;
		new->typf=m->typf;
		new->q1.flags=VAR;
		new->q1.v=v;
		new->q1.val.vmax=Z0;
		m->z=new->q1;
		m->code=ASSIGN;
		insert_IC_fg(g,p,new);
		if(DEBUG&1024){
		  pric2(stdout,m);
		  pric2(stdout,new);
		}
		p->arg_list[i]=new;
	      }
	    }
	  }
#endif	  
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
	    type *t=new_typ();
	    t->flags=LIBCALL_CMPTYPE;
	    p->code=COMPARE;
	    p->q2.flags=KONST;
	    gval.vmax=Z0;
	    eval_const(&gval,MAXINT);
	    insert_const(&p->q2.val,LIBCALL_CMPTYPE);
	    p->typf=LIBCALL_CMPTYPE;
	    p->q1.flags=VAR;
	    p->q1.v=add_tmp_var(t);
	    p->q1.val.vmax=l2zm(0L);
	    p->prev->z=p->q1;
	    p->z.flags=0;
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

int in_varlist(varlist *vl,int cnt,Var *v,int flags)
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
  IC *p;
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

void calc_finfo(Var *v,int flags)
{
  IC *p;int i,known,maxtyp,t,c;
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
		v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(varlist));
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
	    v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(varlist));
	    v->fi->use_list[v->fi->use_cnt-1]=p->use_list[i];
	  }
	}
      }
    }
    for(c=0;c<=maxtyp;c++){
      if(BTST(tf,c)){
	v->fi->use_cnt++;
	v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(varlist));
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
		v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(varlist));
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
	    v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(varlist));
	    v->fi->change_list[v->fi->change_cnt-1]=p->change_list[i];
	  }
	}
      }
    }
    for(c=0;c<=maxtyp;c++){
      if(BTST(tf,c)){
	v->fi->change_cnt++;
	v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(varlist));
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
	  v->fi->call_list=myrealloc(v->fi->call_list,v->fi->call_cnt*sizeof(varlist));
	  v->fi->call_list[v->fi->call_cnt-1].v=p->q1.v;
	  v->fi->call_list[v->fi->call_cnt-1].flags=0;
	}
      }
    }
    v->fi->flags|=ALL_CALLS;
  }
}

void used_clist(type *t,const_list *cl)
{
  int i;zmax l;
  if(ISARRAY(t->flags)){
    for(l=l2zm(0L);!zmleq(t->size,l)&&cl;l=zmadd(l,l2zm(1L)),cl=cl->next){
      if(!cl->other){ierror(0);return;}
      used_clist(t->next,cl->other);
    }
    return;
  }
  if(ISUNION(t->flags)&&!cl->tree){
    used_clist((*t->exact->sl)[zm2l(cl->idx)].styp,cl->other);
    return;
  }
  if(ISSTRUCT(t->flags)){
    type *st;
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

void used_objects(Var *v)
{
  int i;
  if(v->flags&REFERENCED) return;
  v->flags|=REFERENCED;
  if(ISFUNC(v->vtyp->flags)){
    IC *p;
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

static void mark_na(bvtype *tmp,int r,int *eqto)
{
  int i;
  if(r<0||r>=vcount-rcount||BTST(tmp,r)) return;
  /*if(DEBUG&1024) printf("mark_na %s(%p):\n",vilist[r]->identifier,(void*)vilist[r]);*/
  BSET(tmp,r);
  for(i=0;i<vcount-rcount;i++){
    if(eqto[i]!=r&&eqto[r]!=i) continue;
    /*if((DEBUG&1024)&&!BTST(tmp,i)) printf("  mark_na %s(%p)\n",vilist[i]->identifier,(void*)vilist[i]);*/
    mark_na(tmp,i,eqto);
  }
}

/* TODO: remove ugly hack by decent exit criteria */
static int reccheck_maxcnt;

/* check if function v can not call callee */
static int check_nonrecursive(Var *v,Var *callee)
{
  int i;
  Var *f;
  if(--reccheck_maxcnt==0) return 0;
  if(v->fi&&(v->fi->flags&ALL_CALLS)){
    for(i=0;i<v->fi->call_cnt;i++){
      f=v->fi->call_list[i].v;
      if(f==v)
	return 0;
      if(!check_nonrecursive(f,callee))
	return 0;
    }
    return 1;
  }
  return 0;
}

static void cnv_static(Var *v)
{
  if((v->storage_class==AUTO||v->storage_class==REGISTER)&&zmleq(Z0,v->offset)&&v->reg==0/*&&!(v->flags&CONVPARAMETER)*/){
    v->storage_class=STATIC;
    v->offset=++label;
    v->flags|=(USEDASSOURCE|USEDASDEST|DEFINED|STATICAUTO);
    if(v->clist){free_clist(v->clist);v->clist=0;}
    if(DEBUG&1024) printf("changing %s(%p) to static (L%ld)\n",v->identifier,v,zm2l(v->offset));
  }
}

/* replace auto variables by statics if possible */
void replace_statics(Var *v)
{
  IC *p;
  if(DEBUG&1024) printf("replace_statics()\n");
  if(force_statics||(v->fi&&(v->fi->flags&ALL_CALLS))){
    if(!force_statics){
      reccheck_maxcnt=100;
      if(!check_nonrecursive(v,v))
	return;
      if(DEBUG&1024) printf("non-recursive\n");
    }

    for(p=first_ic;p;p=p->next){
      if((p->q1.flags&(VAR|REG))==VAR) cnv_static(p->q1.v);
      if((p->q2.flags&(VAR|REG))==VAR) cnv_static(p->q2.v);
      if((p->z.flags&(VAR|REG))==VAR) cnv_static(p->z.v);

      if(p->code==ADDRESS&&p->q1.v->storage_class==STATIC){
	p->code=ASSIGN;
	p->q1.flags|=VARADR;
	p->q2.val.vmax=sizetab[p->typf2&NQ];
	p->typf=p->typf2;
      }
    }
  }
}


void recalc_offsets(flowgraph *g)
/*  berechnet Offsets fuer auto-Variablen neu und versucht, fuer Variablen, */
/*  die nicht gleichzeitig aktiv sind, den gleichen Platz zu belegen        */
{
  int i,j,b,bcnt,r,*eqto,pass;size_t bsize,vsize;zmax *al,*sz;
  bvtype **used,*tmp,*empty,*vtmp,*vpos;
  IC *p;
  flowgraph *fg;

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
    if((vilist[i]->storage_class==AUTO||vilist[i]->storage_class==REGISTER)&&zmleq(l2zm(0L),vilist[i]->offset)){
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
  b=0; fg=g;
  while(fg){
    if(b>=basic_blocks) ierror(0);
    for(i=0;i<vcount-rcount;i++){
      if(used[i]&&(BTST(fg->av_in,i)||BTST(fg->av_out,i))){
	BSET(used[i],b);
	for(r=1;r<=MAXR;r++)
	  if(fg->regv[r]&&fg->regv[r]->index==i) BCLR(used[i],b);
      }
    }
    for(p=fg->start;p;p=p->next){
      if((p->q1.flags&(VAR|REG))==VAR){
	i=p->q1.v->index;
	if(i!=-1&&i<vcount-rcount){
	  if(i<0||i>=vcount) ierror(0);
	  if(i>=0&&used[i]){
	    BSET(used[i],b);
	  }
	}
      }
      if((p->q2.flags&(VAR|REG))==VAR){
	i=p->q2.v->index;
	if(i!=-1&&i<vcount-rcount){
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
  bcnt=b;

  if(DEBUG&1024) printf("local recalc\n");
  vsize=BVSIZE(vcount-rcount);
  vtmp=mymalloc(vsize);
  vpos=mymalloc(vsize);
  j=0;
  for(fg=g;fg;fg=fg->normalout){
    bvcopy(vtmp,fg->av_in,vsize);
    bvunite(vtmp,fg->av_out,vsize);
    for(r=1;r<=MAXR;r++)
      if(fg->regv[r]&&fg->regv[r]->index<vcount-rcount) BSET(vtmp,fg->regv[r]->index);
    for(i=0;i<vcount-rcount;i++){
      if(!BTST(vtmp,i)){
	if(!used[i]||!BTST(used[i],j)||(vilist[i]->storage_class!=AUTO&&vilist[i]->storage_class!=REGISTER))
	  BSET(vtmp,i);
	else{
	  for(r=0;r<bcnt;r++)
	    if(r!=j&&BTST(used[i],r))
	      BSET(vtmp,i);
	}
      }
    }
#if 1
    if(DEBUG&1024){
      printf("possible for local recalc block %d (%d):\n",j,fg->index);
      for(i=0;i<vcount-rcount;i++)
	if(!BTST(vtmp,i))
	  printf("  %s(%p)\n",vilist[i]->identifier,(void*)vilist[i]);
    }
#endif
    for(p=fg->start;p;p=p->next){
      if((p->z.flags&(VAR|DREFOBJ))==VAR){
	b=p->z.v->index;
	if(b>=0&&b<vcount-rcount&&eqto[b]<0&&!BTST(vtmp,b)){
	  /* imprecise, but should do the job */
	  IC *m;
	  /*if(DEBUG&1024) printf("checking %s(%p)\n",vilist[b]->identifier,(void*)vilist[b]);*/
	  bvcopy(vpos,vtmp,vsize);
	  for(m=p->next;m;m=m->next){
	    if(m->q1.flags&VAR) mark_na(vpos,m->q1.v->index,eqto);
	    if(m->q2.flags&VAR) mark_na(vpos,m->q2.v->index,eqto);
	    if(m->z.flags&VAR){
	      mark_na(vpos,m->z.v->index,eqto);
	      if(m->z.v->index==b) BSET(vtmp,b);
	    }
	    if(m==fg->end) break;
	  }
	  for(i=vcount-rcount-1;i>=0;i--){
	    if(!BTST(vpos,i)&&i!=b){
	      if(DEBUG&1024) printf("local memory for %s(%p) and %s(%p) equal",vilist[i]->identifier,(void *)vilist[i],vilist[b]->identifier,(void *)vilist[b]);
	      eqto[b]=i;
	      if(!zmleq(al[b],al[i])) al[i]=al[b];
	      if(!zmleq(sz[b],sz[i])) sz[i]=sz[b];
	      if(!zmleq(al[i],al[b])) al[b]=al[i];
	      if(!zmleq(sz[i],sz[b])) sz[b]=sz[i];
	      if(DEBUG&1024) printf(" sz=%ld al=%ld\n",(long)zm2l(al[i]),(long)zm2l(sz[i]));
	      BSET(vtmp,i);
	      /*bvunite(used[i],used[b],bsize);*/
	      break;
	    }
	  }
	}
      }
      if(p==fg->end) break;
    }
    j++;
  }
  free(vtmp);
  free(vpos);


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
	if(!zmleq(al[i],al[b])) al[b]=al[i];
	if(!zmleq(sz[i],sz[b])) sz[b]=sz[i];
	bvunite(used[i],used[b],bsize);
      }
    }
  }

  if(DEBUG&1024) printf("propagating sz/al\n");
  do{
    if(DEBUG&1024) printf("ppass\n");
    b=0;
    for(i=0;i<vcount-rcount;i++){
      j=i;
      while((j=eqto[j])>=0){
	if(!zmleq(sz[j],sz[i])){sz[i]=sz[j];b=1;j=i;if(DEBUG&1024) printf("lmset %d(%p) to sz %ld\n",i,vilist[i],zm2l(sz[i]));}
	if(!zmleq(sz[i],sz[j])){sz[j]=sz[i];b=1;if(DEBUG&1024) printf("lmset %d(%p) to sz %ld\n",j,vilist[j],zm2l(sz[j]));}
	if(!zmleq(al[j],al[i])){al[i]=al[j];b=1;j=i;if(DEBUG&1024) printf("lmset %d(%p) to al %ld\n",i,vilist[i],zm2l(al[i]));}
	if(!zmleq(al[i],al[j])){al[j]=al[i];b=1;if(DEBUG&1024) printf("lmset %d(%p) to al %ld\n",j,vilist[j],zm2l(al[j]));}

      }
    }
  }while(b);

  if(DEBUG&1024) printf("final recalculating\n");
  max_offset=recalc_start_offset;
  for(pass=0;pass<3;pass++){
    zmax a;
    for(a=maxalign;!zmeqto(a,Z0);a=zmsub(a,Z1)){
      for(i=0;i<vcount-rcount;i++){
	if(!used[i]) continue;
	if(!zmeqto(a,al[i])) continue;
	if(eqto[i]>=0&&pass<2) continue;

	if(pass==0){
	  if(!zmleq(sz[i],sizetab[MAXINT])&&!zmleq(sz[i],sizetab[LDOUBLE])) continue;
	}
	free(used[i]);
	used[i]=0;
	if(DEBUG&2048) printf("adjusting offset for %s,%ld(%p)\n",vilist[i]->identifier,zm2l(vilist[i]->offset),(void*)vilist[i]);
	if(eqto[i]>=0){
	  j=eqto[i];
	  do{
	  }while(used[j]&&(j=eqto[j])>=0);
	  if(j<0) ierror(0);
	  if(!zmleq(sz[i],sz[j])) 
	    {printf("%d(%p) %d(%p) %ld %ld\n",i,vilist[i],j,vilist[j],zm2l(sz[i]),zm2l(sz[j]));ierror(0);}
	  if(!zmleq(al[i],al[j]))
	    {printf("%d(%p) %d(%p) %ld %ld\n",i,vilist[i],j,vilist[j],zm2l(al[i]),zm2l(al[j]));ierror(0);}
	  vilist[i]->offset=vilist[j]->offset;
	  if(DEBUG&2048) printf("set to %ld (eqto)\n",(long)zm2l(vilist[i]->offset));
	  continue;
	}
	vilist[i]->offset=zmmult(zmdiv(zmadd(max_offset,zmsub(al[i],l2zm(1L))),al[i]),al[i]);
	max_offset=zmadd(vilist[i]->offset,sz[i]);
	if(DEBUG&2048) printf("set to %ld (std)\n",(long)zm2l(vilist[i]->offset));
      }
    }
  }
  recalc_start_offset=l2zm(0L);
  free(used);
  free(sz);
  free(al);
  free(tmp);
  free(empty);
  free(eqto);
}
void remove_IC_fg(flowgraph *g,IC *p)
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

/* function to decide whether a possible const memcpy shall
   be implemented using typ t; if noptr is non-zero decide
   if not introducing a pointer is preferable */
int decide_const_memcpy(IC *p,int typ,int noptr)
{
  zmax cnt;

  /* usually int is the most efficient */
  if(typ==CHAR){
  }else if(typ==BESTCOPYT){
    if((p->z.flags&(VAR|DREFOBJ))!=VAR)
      return 0;
    if(!zmleq(align[typ],falign(p->z.v->vtyp)))
      return 0;
    if(!zmeqto(zmmod(p->z.val.vmax,align[typ]),Z0))
      return 0;
  }else
    return 0;
  
  cnt=zmdiv(p->q2.val.vmax,sizetab[typ]);

  if((p->z.flags&(DREFOBJ|KONST))!=DREFOBJ){
    int sc;
    if(p->z.flags&DREFOBJ)
      sc=STATIC;
    else
      sc=p->z.v->storage_class;
    if(sc==AUTO||sc==REGISTER){
      /* we assume that writes on the stack can be combined */
      if(typ==CHAR&&noptr)
	if(zmleq(cnt,zmmult(clist_copy_pointer,sizetab[BESTCOPYT])))
	  return 1;
      if(zmleq(cnt,l2zm(clist_copy_stack)))
	return 1;
    }else{
      if(zmleq(cnt,l2zm(clist_copy_static)))
	return 1;
    }
  }

  if(noptr==0&&zmleq(cnt,l2zm(clist_copy_pointer)))
    return 1;

  return 0;
}

int insert_const_memcpy(flowgraph *fg)
{
  int changed =0;
  flowgraph *g;
  for(g=fg;g;g=g->normalout){
    IC *p;
    for(p=g->start;p;p=p->next){
      int c=p->code;
      
      /* TODO: finish support for PUSH */
      if((c==ASSIGN/*||c==PUSH*/)&&(p->q1.flags&(VAR|VARADR|DREFOBJ))==VAR&&p->q1.v->clist&&is_const(p->q1.v->vtyp)){
	zmax i,sz=p->q2.val.vmax;
	zuchar zuc;
	Var *v=p->q1.v;
	type *t=v->vtyp;
	int ok=1;

	for(i=Z0;zmleq(i,sz);i=zmadd(i,Z1)){
	  if(!get_clist_byte(t,v->clist,i,&zuc))
	    ok=0;
	}
	
	if(ok==1){
	  int typ=0,noptr=1;
	  int a1,a2;
	  if(DEBUG&1024) {printf("can replace const memcpy:\n");pric2(stdout,p);}
	  
	  if(c==ASSIGN&&(p->z.flags&(DREFOBJ|KONST))!=DREFOBJ){
	    if(decide_const_memcpy(p,BESTCOPYT,noptr))
	      typ=BESTCOPYT;
	    else if(decide_const_memcpy(p,CHAR,noptr))
	      typ=CHAR;
	  }
	  if(typ==0){
	    noptr=0;
	    if(decide_const_memcpy(p,BESTCOPYT,noptr))
	      typ=BESTCOPYT;
	    else if(decide_const_memcpy(p,CHAR,noptr))
	      typ=CHAR;
	  }
	  
	  if(typ){
	    Var *ptr;
	    IC *new;

	    if(DEBUG&1024) printf("will do it (typ=%d noptr=%d)\n",typ,noptr);
	    if(!noptr){
	      type *pt;
	      if(p->z.flags&VAR){
		if(p->z.flags&DREFOBJ){
		  type *new=new_typ();
		  new->flags=p->z.dtyp&NQ;
		  new->next=new_typ();
		  new->next->flags=typ;
		  pt=new;
		}else{
		  pt=new_typ();
		  pt->flags=POINTER_TYPE(p->z.v->vtyp);
		  pt->next=clone_typ(p->z.v->vtyp);
		}
	      }else{
		static type ct={CHAR};
		type *new;
		ct.flags=typ;
		new=new_typ();
		new->flags=POINTER_TYPE(&ct);
		new->next=new_typ();
		new->next->flags=typ;
		pt=new;
	      }

	      ptr=add_tmp_var(pt);
	      new=new_IC();
	      new->z.flags=VAR;
	      new->z.v=ptr;
	      new->q1=p->z;
	      if(p->z.flags&DREFOBJ){
		new->code=ASSIGN;
		new->q1.flags&=~DREFOBJ;
		new->typf=ptr->vtyp->flags;
	      }else if(p->z.v->storage_class==STATIC||p->z.v->storage_class==EXTERN){
		new->code=ASSIGN;
		new->q1.flags|=VARADR;
		new->typf=ptr->vtyp->flags;
	      }else if(p->z.v->storage_class==AUTO||p->z.v->storage_class==REGISTER){
		new->code=ADDRESS;
		new->typf=p->z.v->vtyp->flags;
		new->typf2=ptr->vtyp->flags;
	      }else
		ierror(0);
	      new->q2.val.vmax=sizetab[new->typf];
	      insert_IC_fg(g,p->prev,new);
	    }
	    i=Z0;
	    while(!zmleq(sz,i)){
	      /* use CHAR for possibly remaining bytes */
	      if(!zmleq(zmadd(sizetab[typ],i),sz))
		typ=CHAR;
	      new=new_IC();
	      new->code=p->code;
	      new->typf=typ;
	      new->q1.flags=KONST;
	      if(typ==CHAR){
		if(!get_clist_byte(t,v->clist,zmadd(i,p->q1.val.vmax),&zuc))
		  ierror(0);
		new->q1.val.vuchar=zuc;
	      }else{
		int state;
		gval.vmax=get_clist_int(t,v->clist,zmadd(i,p->q1.val.vmax),zm2l(sizetab[typ]),&state);
		if(!state) ierror(0);
		eval_const(&gval,MAXINT);
		insert_const(&new->q1.val,typ);
	      }
	      new->q2.val.vmax=sizetab[new->typf];
	      if(new->code==ASSIGN){
		if(noptr){
		  new->z=p->z;
		  new->z.val.vmax=zmadd(new->z.val.vmax,i);
		}else{
		  new->z.flags=VAR|DREFOBJ;
		  new->z.v=ptr;
		  new->z.dtyp=ptr->vtyp->flags;
		}
	      }else
		ierror(0); /* TODO: PUSH */
	      insert_IC_fg(g,p->prev,new);
	      if(!noptr){
		new=new_IC();
		new->code=ADDI2P;
		new->q1.flags=VAR;
		new->q1.v=ptr;
		new->z=new->q1;
		new->q2.flags=KONST;
		new->q2.val.vint=zm2zi(sizetab[typ]);
		new->typf=INT;
		new->typf2=ptr->vtyp->flags;
		insert_IC_fg(g,p->prev,new);
	      }	
	      i=zmadd(i,sizetab[typ]);
		
	    }
	    remove_IC_fg(g,p);
	    p=new;
	    changed=1;
	  }
	}
      }
      
      if(p==g->end) break;
    }
  }
  return changed;
}

/* combines assignments of constants into a larger type */
void combine_const_assigns(void)
{
  static int ctype[BESTCOPYT],i,j;
  IC *p=first_ic;

  /* calculate table of combine types */
  for(i=CHAR;i<BESTCOPYT;i++){
    ctype[i]=0;
    for(j=BESTCOPYT;j>CHAR;j--){
      if(zmeqto(sizetab[j],zmadd(sizetab[i],sizetab[i]))){
	if(DEBUG&1024) printf("possible combine type %s=>%s\n",typname[i],typname[j]);
	ctype[i]=j;
	break;
      }
    }
  }
  while(p){
    if(p->code==ASSIGN&&(p->q1.flags&(KONST|DREFOBJ))==KONST&&(p->z.flags&(DREFOBJ|VAR))==VAR){
      IC *p2=p->next;
      if(p2&&p2->code==ASSIGN&&(p2->q1.flags&(KONST|DREFOBJ))==KONST&&(p2->z.flags&(DREFOBJ|VAR))==VAR&&p->z.v==p2->z.v&&p->typf==p2->typf&&zmeqto(p->q2.val.vmax,p2->q2.val.vmax)&&zmeqto(p2->z.val.vmax,zmadd(p->z.val.vmax,sizetab[p->typf&NQ]))){
	int dt,t=p->typf&NQ,sc=p->z.v->storage_class;
	if(t<BESTCOPYT&&(dt=ctype[t])){
	  zmax os;
	  /* for stack variables we can use the actual offset, for statics
	     we rely on the type alignment */
	  if(sc==AUTO||sc==REGISTER)
	    os=zmadd(p->z.v->offset,p->z.val.vmax);
	  else{
	    if(zmeqto(zmmod(falign(p->z.v->vtyp),align[dt]),Z0))
	      os=p->z.val.vmax;
	    else
	      os=Z1;
	  }
	  if(zmeqto(zmmod(os,align[dt]),Z0)){
	    zumax v1,v2;
	    if(DEBUG&1024){
	      printf("combine const assignment:\n");
	      pric2(stdout,p);
	      pric2(stdout,p2);
	    }
	    p->typf=dt;
	    p->q2.val.vmax=sizetab[dt];
	    eval_const(&p->q1.val,t|UNSIGNED);
	    v1=vmax;
	    eval_const(&p2->q1.val,t|UNSIGNED);
	    v2=vmax;
	    if(BIGENDIAN)
	      v1=zmlshift(v1,zmmult(sizetab[t],char_bit));
	    else
	      v2=zmlshift(v2,zmmult(sizetab[t],char_bit));
	    gval.vmax=zmor(v1,v2);
	    eval_const(&gval,MAXINT);
	    insert_const(&p->q1.val,dt);
	    if(DEBUG&1024){printf("to:\n"); pric2(stdout,p);}
	    remove_IC(p2);
	    if(p->prev) p=p->prev; /* retry for more combining */
	    continue;
	  }
	}
      }
    }
    p=p->next;
  }
}
      

/* allowed to create new variables */
int early_peephole(void)
{
  IC *p;
  int changed,gchanged=0,c,t;
  do{
    changed=0;
    p=first_ic;
    while(p){
      c=p->code;
      t=p->typf;
     if(c==COMPARE&&(p->q2.flags&KONST)&&ISINT(p->typf)){
	case_table *ct;
	/* TODO: support multiple_ccs */
	if(!multiple_ccs&&(ct=calc_case_table(p,JUMP_TABLE_DENSITY))&&ct->num>1&&ct->density==1){
	  int i;
	  for(i=0;i<ct->num;i++){
	    if(ct->labels[i]!=ct->labels[0])
	      break;
	  }
	  if(i>=ct->num){
	    IC *new;
	    static type ityp;
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
	      IC *m=p->next;
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
  IC *p;obj o;int t,c,null,eins,changed,done=0;
  function_calls=0;
  do{
    if(DEBUG&1024) printf("searching for peephole optimizations\n");
    changed=0;ic_count=0;
    p=first_ic;
    while(p){
      c=p->code;
      t=p->typf;
      if(c==NOP&&!p->q1.flags&&!p->q2.flags&&!p->z.flags){
	IC *m;
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
	  IC *p2=p->next;
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
	  IC *p2=p->next;
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
	IC *p2,*m;
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
      if(p->code==COMPARE&&(p->q1.flags&(VAR|VARADR))==(VAR|VARADR)&&(p->q2.flags&(VAR|VARADR))==(VAR|VARADR)&&p->q1.v==p->q2.v){
	IC *old=p->prev;
	if(fold(p)){ changed=1; p=old;continue;}
	p=p->next;continue;
      }
      if((p->q1.flags&(KONST|DREFOBJ))==KONST){
	if(((p->q2.flags&(KONST|DREFOBJ))==KONST)||!p->q2.flags){
	  IC *old=p->prev;
	  if(fold(p)){ changed=1; p=old;continue;}
	  p=p->next;continue;
	}else{
	  if(c==ADD||c==MULT||(c>=OR&&c<=AND)||(c==COMPARE&&!(p->z.flags)&&p->next&&p->next->code>=BEQ&&p->next->code<BRA)){ /*  const nach rechts   */
	    if(DEBUG&1024){ printf("swapped commutative op:\n");pric2(stdout,p);}
	    o=p->q1;p->q1=p->q2;p->q2=o;
	    if(c==COMPARE){
	      IC *br=p->next;
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
#if HAVE_POF2OPT
	if(((c==MULT)||((c==DIV||c==MOD)&&(t&UNSIGNED)))&&ISINT(t)){
	  /*  ersetzt mul etc. mit Zweierpotenzen     */
	  long ln;
	  if(zmleq(l2zm(0L),vmax)&&zumleq(ul2zum(0UL),vumax)){
	    if(ln=get_pof2(vumax)){
	      if(c==MOD){
		vmax=zmsub(vmax,l2zm(1L));
		p->code=c=AND;
	      }else{
		if(c==DIV) p->code=RSHIFT; else p->code=LSHIFT;
		vmax=l2zm(ln-1);
	      }
	      gval.vmax=vmax;
	      eval_const(&gval,MAXINT);
	      insert_const(&p->q2.val,t);
	      p->typf2=p->typf;
	      changed=1;
	    }
	  }
	}
#endif
	if(c==LSHIFT||c==RSHIFT){
	  zmax size;
	  size=zmmult(sizetab[q1typ(p)&NQ],char_bit);
	  if(zmleq(size,vmax)){
	    if(c==LSHIFT||(q1typ(p)&UNSIGNED)){
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
	  obj o;int ct=q2typ(p);
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
	  IC *n=p->next;
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
	  if(n&&nc==c&&(n->q2.flags&KONST)&&tn==tp&&(!(tn&VOLATILE))&&(p->z.flags&VAR)&&n->q1.flags==p->z.flags&&n->q1.v==p->z.v&&zmeqto(n->q1.val.vmax,p->z.val.vmax)&&!is_volatile_obj(&n->q1)&&!is_volatile_obj(&p->z)){
	    if(p->q1.flags==p->z.flags&&p->q1.v==p->z.v&&zmeqto(p->q1.val.vmax,p->z.val.vmax)&&!is_volatile_obj(&p->q1)){
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
	  IC *p1=p->next,*p2=p1->next;
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
	  obj o;
	  if(DEBUG&1024){printf("swapping objs because USEQ2ASZ\n");pric2(stdout,p);}
	  o=p->q2;p->q2=p->q1;p->q1=o;
	  /*  kein changed hier!  */
	}else{pric2(stdout,p); ierror(0);}
      }
      if((c==ADD||c==SUB)&&p->next){
	/*FIXME: using SCRATCH is not nice */
	IC *p1=p->next;
	if(p1->code==ADDI2P&&p1->typf==t&&(p->z.flags&(SCRATCH|DREFOBJ))==SCRATCH&&!compare_objs(&p->z,&p1->q2,t)&&zmleq(sizetab[p1->typf2&NQ],sizetab[p1->typf&NQ])){
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
	IC *d;
	if(DEBUG&1024){ printf("removing redundant move:\n");pric2(stdout,p);}
	changed=1;
	d=p; p=p->next;
	if(have_alias){ free(d->use_list); free(d->change_list);}
	remove_IC(d); continue;
      }
#if 1
      if(c==CONVERT&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&(p->z.flags&(VAR|DREFOBJ))==VAR&&!is_volatile_ic(p)){
	IC *p1=p->next;
	if(p1&&p1->code==CONVERT&&(p->typf2&NQ)==(p1->typf&NQ)&&(p->typf&NQ)==(p1->typf2&NQ)&&zmleq(sizetab[p->typf2&NQ],sizetab[p->typf&NQ])&&!compare_objs(&p->z,&p1->q1,p->typf2)&&!is_volatile_ic(p1)&&ISFLOAT(p->typf)==ISFLOAT(p->typf2)){
	  if(DEBUG&1024){printf("propagating CONVERTS:\n");pric2(stdout,p);pric2(stdout,p1);}
	  p1->q1=p->q1;
	  p1->code=ASSIGN;
	  p1->q2.val.vmax=sizetab[p1->typf&NQ];
	  continue;
	}
	if(p1&&(p1->code==ADDI2P||p1->code==SUBIFP)&&(p1->typf&NU)==(p->typf&NU)&&!compare_objs(&p->z,&p1->q2,p1->typf)&&!is_volatile_ic(p1)&&(p->typf2&NQ)>=MINADDI2P&&(p->typf2&NQ)<=MAXADDI2P){
	  if((p->typf2&UNSIGNED)&&(p->typf2&NU)>=MINADDUI2P){
	    if(DEBUG&1024){printf("propagating CONVERT/ADDI2P:\n");pric2(stdout,p);pric2(stdout,p1);}
	    p1->q2=p->q1;
	    p1->typf=p->typf2;
	    continue;
	  }
	}
      }
#endif
      if(c>=BEQ&&c<=BGT){
	IC *p2=p->prev;
	if(p2&&p2->code==COMPARE&&!compare_objs(&p->q1,&p2->z,0)){
	  IC *p3=p2->prev;
	  if(p3&&p3->code==c){
	    IC *p4=p3->prev;
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
  IC *p,*new;
  type t={0},v={VOID};
  int c;
  if(DEBUG&1024) printf("insert_loads()\n");
  for(p=first_ic;p;p=p->next){
    c=p->code;
    if(p->typf&VOLATILE) continue;
    if(p->typf2&VOLATILE) continue;
    if(p->q2.flags||c==PUSH){
      if((dref_cse&&(p->q1.flags&DREFOBJ)&&!(p->q1.dtyp&(PVOLATILE|VOLATILE)))||(static_cse&&(p->q1.flags&(VAR|VARADR))==VAR&&(p->q1.v->storage_class==EXTERN||p->q1.v->storage_class==STATIC))){
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
      if((dref_cse&&(p->q2.flags&DREFOBJ)&&!(p->q2.dtyp&(PVOLATILE|VOLATILE)))||(static_cse&&(p->q2.flags&(VAR|VARADR))==VAR&&(p->q2.v->storage_class==EXTERN||p->q2.v->storage_class==STATIC))){
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
      if(p->q2.flags&&((dref_cse&&(p->z.flags&DREFOBJ)&&!(p->z.dtyp&(PVOLATILE|VOLATILE)))||(static_cse&&(p->z.flags&(VAR|VARADR))==VAR&&(p->z.v->storage_class==EXTERN||p->z.v->storage_class==STATIC)))){
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
    IC *p; Var *v; type *t;
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

static int to_be_inlined(Var *v)
{
  /* decide whether function should be inlined */
  int c;IC *p;
  /* no code available */
  if(!v->fi||!v->fi->first_ic) return 0;
  /* marked as noinline */
  if(v->fi->flags&NO_INLINE) return 0;
  /* returns something not in a register (FIXME) */
  if(!ffreturn(v->vtyp->next)&&(v->vtyp->next->flags&NQ)!=VOID) return 0;
  /* varargs function */
  if((c=v->vtyp->exact->count)!=0&&(*v->vtyp->exact->sl)[c-1].styp->flags!=VOID)
    return 0;
  /* uses variable length arrays */
  if(v->fi->flags&USES_VLA) return 0;
  if(v->fi->inline_size>=0){
    c=v->fi->inline_size;
  }else{
    for(c=0,p=v->fi->first_ic;p;p=p->next) c++;
  }
  /* try to always inline functions specified as inline */
  if(v->flags&INLINEFUNC)
    return 1;
  if(c>inline_size) return 0;
  /* we assume that inlining saves size if the number of arguments
     is larger than the number of ICs + CALL +SETRETURN  */
  if(optsize&&c-2>(v->vtyp->exact->count)) return 0;
  return 1;
}

static int depth_reached;

static int cross_module_inline(int only_full)
{
  IC *p,*np,*new,*ip,*cp,*getreturn;
  Var *v,*vp;
  int i,c,firstl,lastl,more_passes=0;
  if(DEBUG&1024) printf("cross_module_inline(only_full=%d)\n",only_full);
  for(p=first_ic;p;){
    np=p->next;
    if(p->code==CALL&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&to_be_inlined(p->q1.v)){
      zmax pushed=p->q2.val.vmax;
      IC *gr;
      v=p->q1.v;
      if(only_full&&((p->q1.flags&(VAR|DREFOBJ))!=VAR||!p->q1.v->fi||!(p->q1.v->fi->flags&FULL_INLINE))){
	depth_reached=1;
      }else{
	more_passes=1;
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
	    IC *nop;
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
	  IC *m;
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
	  Var *iv;
	  int c;
	  if(ip->code==CONVERT&&(ip->q1.flags&(VAR|DREFOBJ))==VAR&&(ip->q1.v->flags&CONVPARAMETER)&&(ip->z.flags&(VAR|DREFOBJ))==VAR&&ip->q1.v==ip->z.v){
	    if(DEBUG&1024) {printf("eliminate oldstyle CONVERT:\n");pric2(stdout,ip);}
	    ip=ip->next;
	    continue;
	  }
	  new=new_IC();
	  *new=*ip;
	  ip->copy=new;
	  c=ip->code;
	  /*  evtl. ist ein IC praktisch ein SETRETURN, falls das */
	  /*  Rueckgabeziel ueber Parameterzeiger angespr. wird   */
	  if(ip->z.flags&VAR){
	    iv=ip->z.v;
	    if(iv->storage_class==AUTO||iv->storage_class==REGISTER||(iv->flags&STATICAUTO)){
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
	  /* recreate ADDRESS when inlining STATICAUTO */
	  if((new->q1.flags&(VAR|VARADR))==(VAR|VARADR)&&(new->q1.v->flags&STATICAUTO)){
	    if(new->code!=ASSIGN) ierror(0);
	    if(DEBUG&1024) printf("recreating ADDRESS from STATICAUTO:\n");
	    if(DEBUG&1024) pric2(stdout,new);
	    new->code=ADDRESS;
	    new->q1.flags&=~VARADR;
	    new->typf2=new->typf;
	    new->typf=CHAR; /* TODO: Do we restore the old type? */
	    if(DEBUG&1024) pric2(stdout,new);
	  }
	  /*  Kopien aller auto/register Variablen erzeugen   */
	  if(ip->q1.flags&VAR){
	    iv=ip->q1.v;
	    if(iv->storage_class==AUTO||iv->storage_class==REGISTER||(iv->flags&STATICAUTO)){
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
	    if(iv->storage_class==AUTO||iv->storage_class==REGISTER||(iv->flags&STATICAUTO)){
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
    }
    p=np;
  }
  
  return more_passes;
}

#endif
#define FREEAV free(av_globals);free(av_statics);free(av_drefs);free(av_address);
void optimize(long flags,Var *function)
/*  flags:   1=Register, 2=optimize, 4=cse/cp, 8=constant_propagation,  */
/*          16=dead_assignments, 32=global-optimizations                */
/*          64=blockweise Registervergabe, 128=loop_optimizations (nur  */
/*             in Verbindung mit 32), 256=recalc_offsets                */
{
#ifndef NO_OPTIMIZER
  flowgraph *g,*fg=0;
  int r,i,pass=0,mustrepeat,intask;
  int lc_freed,lc_done=0;
  if(!function) ierror(0);
  norek=nocall=0;
  report_suspicious_loops=report_weird_code=1;
  if(!strcmp(function->identifier,"main")){norek=1;nocall=1;}
  /*  falls main() rekursiv aufgerufen werden kann, muss nomain==0 sein   */

  intask=(function->vattr&&strstr(function->vattr,"taskprio("));

  
#else

  flags&=1;

#endif


  if(flags&2){
#ifndef NO_OPTIMIZER
    if(DEBUG&1024) printf("\nOptimizing function %s\n",function->identifier);
    /*  Variablen fuer ccs einsetzen.   */
    if(cross_module&&(flags&4096)){
      int i,notdone=1;
      for(i=0;notdone;i++){
	depth_reached=0;
	notdone=cross_module_inline(i>=inline_depth);
      }
      if(!depth_reached){
	if(!function->fi) function->fi=new_fi();
	function->fi->flags|=FULL_INLINE;
	if(DEBUG&1024) printf("function %s set to full-inline\n",function->identifier);
      }
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

#if 0
      if(DEBUG&1024) printf("range analysis\n");
      range_analysis(fg);
      myfree(rangebuf);
      rangebuf=0;
#endif

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
	      IC *p;
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
	if(DEBUG&1024) print_flowgraph(fg);
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

	if(early_eff_ics&&!no_eff_ics)
	  mark_eff_ics();

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
	if(!no_eff_ics)
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

      if(!gchanged){
	int r;
	r=insert_const_memcpy(fg);
	if(r){
	  gchanged|=1;
	  pass--;
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
	  IC *p,*new;
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
	
	if(cross_module) calc_finfo(function,CALC_USES|CALC_CHANGES|(prefer_statics?CALC_CALLS:0));
       

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
	      int stack_used;IC *p;
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
	    /* calculate optimized size for cross-module-inlining */
	    {
	      IC *p;
	      if(!function->fi) function->fi=new_fi();
	      function->fi->inline_size=0;
	      for(p=first_ic;p;p=p->next) function->fi->inline_size++;
	      if(DEBUG&1024) printf("inline_size=%ld\n",function->fi->inline_size);
	    }

	    insert_regs(fg);
	    insert_saves(fg);
	    if(prefer_statics||force_statics) replace_statics(function);
	    
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
    combine_const_assigns();
    
#endif

  }else{
    /*  keine Optimierungen     */
    if(flags&1) simple_regs();
    load_simple_reg_parms();
  }
#ifndef NO_OPTIMIZER
  add_call_list();
#endif

  lastlabel=label;
}




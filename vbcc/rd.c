/*  $VER: vbcc (rd.c) $Revision: 1.16 $    */
/*  reaching definitions and constant propagation   */

#include "opt.h"

static char FILE_[]=__FILE__;

unsigned int dcount;
size_t dsize;
IC **dlist;
bvtype *rd_defs;
bvtype **defs_kill;    /* definitions killed */
bvtype **defs_gen;     /* definitions undefined */
bvtype **var_defs;     /* definitions of a variable */
bvtype **var_undefs;   /* definitions which are undefined by writing to this var */
bvtype *rd_matrix;

int def_overwrites(IC *new,IC *old);
#define NO_OVERWRITE 0
#define FULL_OVERWRITE 1
#define PARTIAL_OVERWRITE 2
#define EXACT_OVERWRITE 3

/*  compares two constants; -1, if q1<q2; 0, if q1==q1; 1 otherwise  */
int compare_const(union atyps *q1,union atyps *q2,int t)
{
  zldouble d1,d2;zmax l1,l2;zumax u1,u2;
  t&=NU;
  eval_const(q1,t);d1=vldouble;l1=vmax;u1=vumax;
  eval_const(q2,t);d2=vldouble;l2=vmax;u2=vumax;
  if(ISFLOAT(t)) return(zldleq(d2,d1)?!zldeqto(d1,d2):-1);
  if(ISPOINTER(t)||(t&UNSIGNED)) return(zumleq(u2,u1)?!zumeqto(u1,u2):-1);
  return(zmleq(l2,l1)?!zmeqto(l1,l2):-1);
}

/* prints definitions in a bitvector */
void print_rd(bvtype *bitvector)
{
  unsigned int i;
  if(!bitvector) {printf("reaching definitions not available\n");return;}
  for(i=1;i<=dcount;i++){
    if(BTST(bitvector,i)) {printf("%3u:",i);pric2(stdout,dlist[i]);}
    if(BTST(bitvector,UNDEF(i))) {printf("%3u(ud):",i);pric2(stdout,dlist[i]);}
  }
}

/* numbers all definitions and creates some bitvectors */
void num_defs(void)
{
  int i,j;IC *p;
  size_t matrix_size;
  bvtype *bp;
  unsigned long heapsize=0;
  if(DEBUG&1024) printf("numerating definitions\n");
  i=0;
  for(p=first_ic;p;p=p->next){
    if(!(p->z.flags&(VAR|DREFOBJ))&&p->code!=CALL){p->defindex=0;continue;}
    p->defindex=++i;
  }
  dcount=i;
  /*dsize=(2*dcount+1+CHAR_BIT)/CHAR_BIT;    /* +1, da bei 1 anfaengt */
  dsize=BVSIZE(2*(dcount+1));
  if(DEBUG&(16384|1024)) printf("%lu definitions, dsize=%lu\n",(unsigned long)dcount,(unsigned long)dsize);
  
  /* get big memory block to store defs_kill, defs_gen, vars, var_defs and var_undefs */
  matrix_size=(2*(dcount+vcount)*dsize);
  rd_matrix=bp=mymalloc(matrix_size);
  memset(bp,0,matrix_size);
  heapsize+=matrix_size;

  /* calculate bit-vector which contains all definitions killed or */
  /* undefined (=partially overwritten) by this definition         */
  defs_kill=mymalloc(sizeof(*defs_kill)*(dcount+1));
  defs_gen=mymalloc(sizeof(*defs_gen)*(dcount+1));
  heapsize+=2*sizeof(*defs_kill)*(dcount+1);

  for(i=1;i<=dcount;i++){
    defs_kill[i]=bp;
    bp+=dsize/sizeof(bvtype);
    defs_gen[i]=bp;
    bp+=dsize/sizeof(bvtype);
  }
  
  /* calculate bit vector with all undefined definitions of each variable */
  var_defs=mymalloc(sizeof(*var_defs)*vcount);
  heapsize+=sizeof(*var_defs)*vcount;
  var_undefs=mymalloc(sizeof(*var_undefs)*vcount);
  heapsize+=sizeof(*var_undefs)*vcount;
  for(i=0;i<vcount;i++){
    var_defs[i]=bp;
    bp+=dsize/sizeof(bvtype);
    var_undefs[i]=bp;
    bp+=dsize/sizeof(bvtype);
  }
  
  /* calculate pointers to IC for every definition */
  dlist=mymalloc((dcount+1)*sizeof(*dlist));
  heapsize+=(dcount+1)*sizeof(*dlist);

  for(p=first_ic;p;p=p->next){
    if(p->defindex){
      dlist[p->defindex]=p;
      if(p->z.flags&VAR){
        Var *v=p->z.v;
        i=v->index;
        if(p->z.flags&DREFOBJ) i+=vcount-rcount;
        BSET(var_defs[i],p->defindex);
	BSET(var_defs[i],UNDEF(p->defindex));
	if(i<rcount){
	  BSET(var_defs[i+vcount-rcount],p->defindex);
	  BSET(var_defs[i+vcount-rcount],UNDEF(p->defindex));
	}
        BSET(var_undefs[i],UNDEF(p->defindex));
      }
    }
  }
  
  for(i=1;i<=dcount;i++){
    p=dlist[i];
    for(j=0;j<p->change_cnt;j++){
      int idx;
      idx=p->change_list[j].v->index;
      if(p->change_list[j].flags&DREFOBJ) idx+=vcount-rcount;
      if(idx>=vcount) continue;	
      BSET(var_defs[idx],i);
      bvunite(defs_gen[i],var_undefs[idx],dsize);
    }
    if(p->z.flags&VAR){
      for(j=1;j<=dcount;j++){
	int ow;
	IC *p2;
	if(i==j) continue;
	p2=dlist[j];
	if(!(p2->z.flags&VAR)||p->z.v!=p2->z.v||p->z.flags!=p2->z.flags)
	  continue;
	ow=def_overwrites(p,p2);
	if(ow==FULL_OVERWRITE||ow==EXACT_OVERWRITE){
	  BSET(defs_kill[i],j);
	  BSET(defs_kill[i],UNDEF(j));
	}
	if(ow==EXACT_OVERWRITE||ow==NO_OVERWRITE){
	  BCLR(defs_gen[i],j);
	  BCLR(defs_gen[i],UNDEF(j));
	}
      }
    }
    /* every definition defines itself) */
    BSET(defs_gen[i],i);
    BCLR(defs_gen[i],UNDEF(i));
    BSET(defs_kill[i],UNDEF(i));
  }
  
  if(DEBUG&2048){
    for(i=0;i<vcount;i++){
      printf("var_defs for var %i %s(%p):\n",i,vilist[i]->identifier,vilist[i]);
      print_rd(var_defs[i]);
    }
	
    for(i=1;i<=dcount;i++){
      printf("Def%3d: ",i);pric2(stdout,dlist[i]);
      printf(" kills:\n");
      for(j=1;j<=dcount;j++){
	if(BTST(defs_kill[i],j)) {printf("  ");pric2(stdout,dlist[j]);}
	if(BTST(defs_kill[i],UNDEF(j))) {printf("  (ud)");pric2(stdout,dlist[j]);}
      }
      printf(" gens:\n");
      for(j=1;j<=dcount;j++){
	if(BTST(defs_gen[i],j)) {printf("  ");pric2(stdout,dlist[j]);}
	if(BTST(defs_gen[i],UNDEF(j))) {printf("  (ud)");pric2(stdout,dlist[j]);}
      }
    }
  }

  if(DEBUG&16384) printf("num_defs heapsize=%lu\n",heapsize);

  free(var_undefs);
}

/* returns whether n overwrites the definition p */
int def_overwrites(IC *n,IC *o)
{
  zmax nstart,nend,ostart,oend,nsize,osize;
  if(!(n->z.flags&VAR)) ierror(0);
  if(!(o->z.flags&VAR)) ierror(0);
  if((n->z.flags&DREFOBJ)!=(o->z.flags&DREFOBJ)) ierror(0);
  if(n->code==ASSIGN){
    nsize=n->q2.val.vmax;
  }else{
    nsize=sizetab[ztyp(n)&NQ];
  }
  if(o->code==ASSIGN){
    osize=o->q2.val.vmax;
  }else{
    osize=sizetab[ztyp(o)&NQ];
  }
  if(n->z.flags&DREFOBJ)
    return zmleq(osize,nsize);

  nstart=n->z.val.vmax;
  nend=zmsub(zmadd(nstart,nsize),l2zm(1L));
  ostart=o->z.val.vmax;
  oend=zmsub(zmadd(ostart,osize),l2zm(1L));

  if(zmeqto(nstart,ostart)&&zmeqto(nend,oend))
    return EXACT_OVERWRITE;

  if(zmleq(nstart,ostart)&&zmleq(oend,nend))
    return FULL_OVERWRITE;

  if(zmleq(ostart,nstart)&&zmleq(nstart,oend))
    return PARTIAL_OVERWRITE;
  if(zmleq(ostart,nend)&&zmleq(nend,oend))
    return PARTIAL_OVERWRITE;

  return NO_OVERWRITE;
}

/* performs data flow analysis for reaching definitions */
void reaching_definitions(flowgraph *fg)
{
  flowgraph *g;IC *p;bvtype *tmp,*init;
  int changed,pass,i,j;
  unsigned long heapsize=0;
  /*  rd_gen und rd_kill fuer jeden Block berechnen   */
  if(DEBUG&1024) printf("analysing reaching definitions\n");
  tmp=mymalloc(dsize);
  init=mymalloc(dsize);
  heapsize+=2*dsize;
  memset(init,0,dsize);
  for(i=1;i<=dcount;i++){
    p=dlist[i];
    if(p->z.flags&VAR){
      Var *v=p->z.v;
      if(v->storage_class==EXTERN||v->storage_class==STATIC||v->reg!=0||!zmleq(l2zm(0L),v->offset)){
	BSET(init,UNDEF(i));
      }
    }
  }
  g=fg;
  while(g){
    g->rd_in=mymalloc(dsize);
    memset(g->rd_in,0,dsize);
    g->rd_out=mymalloc(dsize);
    memset(g->rd_out,0,dsize);
    g->rd_gen=mymalloc(dsize);
    memset(g->rd_gen,0,dsize);
    g->rd_kill=mymalloc(dsize);
    memset(g->rd_kill,0,dsize);
    heapsize+=4*dsize;
    p=g->end;
    while(p){
      if(p->defindex){
	bvunite(g->rd_gen,defs_gen[p->defindex],dsize);
	bvdiff(g->rd_gen,g->rd_kill,dsize);
	bvunite(g->rd_kill,defs_kill[p->defindex],dsize);
	bvdiff(g->rd_kill,g->rd_gen,dsize);
      }
      if(p==g->start) break;
      p=p->prev;
    }
    memcpy(g->rd_out,g->rd_gen,dsize);
    g=g->normalout;
  }

  /*  rd_in und rd_out fuer jeden Block berechnen */
  /*  out(b)=gen(B) vorinitialisiert              */
  if(DEBUG&1024) {printf("pass:");pass=0;}
  do{
    if(DEBUG&1024) {printf(" %d",++pass);fflush(stdout);}
    changed=0;
    g=fg;
    while(g){
      flowlist *lp;
      /*  in(B)=U out(C) : C Vorgaenger von B */
      if(g==fg)
	memcpy(g->rd_in,init,dsize);
      else
	memset(g->rd_in,0,dsize);
      lp=g->in;
      while(lp){
	if(!lp->graph) ierror(0);
	if(lp->graph->branchout==g||!lp->graph->end||lp->graph->end->code!=BRA)
	  bvunite(g->rd_in,lp->graph->rd_out,dsize);
	lp=lp->next;
      }
      /*  out(b)=gen(B) U (in(B)-kill(B)  */
      memcpy(tmp,g->rd_in,dsize);
      bvdiff(tmp,g->rd_kill,dsize);
      bvunite(tmp,g->rd_gen,dsize);
      if(!bvcmp(tmp,g->rd_out,dsize)){changed=1;memcpy(g->rd_out,tmp,dsize);}
      g=g->normalout;
    }
  }while(changed);
  if(DEBUG&1024) printf("\n");
  if(DEBUG&16384) printf("reaching_defs heapsize=%lu\n",heapsize);
  free(tmp);
  free(init);
}

/* calculates z:=q1 op q2 with constants */
/* if p is non-zero q1typ,q2typ from p will be used */
void calc(int c,int t,union atyps *q1,union atyps *q2,union atyps *z,IC *p)
{
  zldouble d1,d2;zmax l1,l2;zumax u1,u2;
  if(p) t=q1typ(p);
  eval_const(q1,t);
  d1=vldouble;l1=vmax;u1=vumax;
  if(c!=MINUS&&c!=KOMPLEMENT){
    if(p)
      eval_const(q2,q2typ(p));
    else
      eval_const(q2,t);
    d2=vldouble;l2=vmax;u2=vumax;
  }
  if(c==ADD){ vldouble=zldadd(d1,d2);vmax=zmadd(l1,l2);vumax=zumadd(u1,u2);}
  if(c==SUB){ vldouble=zldsub(d1,d2);vmax=zmsub(l1,l2);vumax=zumsub(u1,u2);}
  if(c==ADDI2P){ vmax=zmadd(l1,l2);vumax=zumadd(u1,u2);}
  if(c==SUBIFP){ vmax=zmsub(l1,l2);vumax=zumsub(u1,u2);}
  if(c==MULT){ vldouble=zldmult(d1,d2);vmax=zmmult(l1,l2);vumax=zummult(u1,u2);}
  if(c==DIV||c==MOD){
    if(zldeqto(d2,d2zld(0.0))&&zmeqto(l2,l2zm(0L))&&zumeqto(u2,ul2zum(0UL))){
      err_ic=p;error(210);err_ic=0;
      vmax=l2zm(0L);vumax=ul2zum(0L);vldouble=d2zld(0.0);
    }else{
      if(c==DIV){
	vldouble=zlddiv(d1,d2);
	if(!zmeqto(l2,l2zm(0L))) vmax=zmdiv(l1,l2);
	if(!zumeqto(u2,ul2zum(0UL))) vumax=zumdiv(u1,u2);
      }else{
	if(!zmeqto(l2,l2zm(0L))) vmax=zmmod(l1,l2);
	if(!zumeqto(u2,ul2zum(0UL))) vumax=zummod(u1,u2);
      }
    }
  }
  if(c==AND){ vmax=zmand(l1,l2);vumax=zumand(u1,u2);}
  if(c==OR){ vmax=zmor(l1,l2);vumax=zumor(u1,u2);}
  if(c==XOR){ vmax=zmxor(l1,l2);vumax=zumxor(u1,u2);}
  if(c==LSHIFT){ vmax=zmlshift(l1,l2);vumax=zumlshift(u1,l2);}
  if(c==RSHIFT){ vmax=zmrshift(l1,l2);vumax=zumrshift(u1,l2);}
  if(c==MINUS){ vldouble=zldsub(d2zld(0.0),d1);vmax=zmsub(l2zm(0L),l1);vumax=zumsub(ul2zum(0UL),u1);}
  if(c==KOMPLEMENT){ vmax=zmkompl(l1);vumax=zumkompl(u1);}

  if(ISFLOAT(t)){
    gval.vldouble=vldouble;
    eval_const(&gval,LDOUBLE);
  }else if(t&UNSIGNED){
    gval.vumax=vumax;
    eval_const(&gval,(UNSIGNED|MAXINT));
  }else{
    gval.vmax=vmax;
    eval_const(&gval,MAXINT);
  }
  if(p) t=ztyp(p);
  insert_const(z,t);
}

/* folds constant ICs */
int fold(IC *p)
{
  int c;
  if(!p) ierror(0);
  c=p->code;
  if(c==SUBPFP||c==ASSIGN||c==PUSH||c==SETRETURN) return 0;
  if(DEBUG&1024) {printf("folding IC:\n");pric2(stdout,p);}
  if(c==TEST||c==COMPARE){
    union atyps val;int cc; /*  condition codes */
    IC *bp;
    if(c==TEST){
      eval_const(&p->q1.val,p->typf);
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
	cc=0; 
      else
	cc=1;
    }else{
      if(p->q1.flags&VARADR)
	cc=compare_const(&p->q1.val,&p->q2.val,UNSIGNED|MAXINT);
      else
	cc=compare_const(&p->q1.val,&p->q2.val,p->typf);
    }
    bp=p->next;
    if(bp->code>=BEQ&&bp->code<=BGT&&(!p->z.flags||p->z.v==bp->q1.v)){
      if(DEBUG&1024) printf("(cc=%d; comparison eliminated)\n",cc);
      if(have_alias){ free(p->use_list);free(p->change_list);}
      remove_IC(p);
      while(1){   /*  zugehoerigen Branch suchen  */
	if(!bp||bp->code==LABEL||bp->code==BRA) ierror(0);
	c=bp->code;
	if(c>=BEQ&&c<=BGT) break;
	bp=bp->next;
      }
      if((c==BEQ&&cc==0)||(c==BNE&&cc!=0)||(c==BLT&&cc<0)||(c==BGT&&cc>0)||(c==BLE&&cc<=0)||(c==BGE&&cc>=0)){
	if(DEBUG&1024){ printf("changed following branch to BRA:\n");pric2(stdout,bp);}
	bp->code=BRA;bp->q1.flags=0;
      }else{
	if(DEBUG&1024){ printf("removed following branch:\n");pric2(stdout,bp);}
	if(have_alias){ free(bp->use_list);free(bp->change_list);}
	remove_IC(bp);
      }
      return 1;
    }
    if(p->z.flags==0){
      p->code=NOP;
      p->q1.flags=p->q2.flags=p->z.flags=0;
      return 1;
    }else
      return 0;
  }
  if(c==CONVERT){
    eval_const(&p->q1.val,p->typf2);
    insert_const(&p->q1.val,p->typf);
  }else
    calc(c,p->typf,&p->q1.val,&p->q2.val,&p->q1.val,p);
  p->q2.flags=0;
  if(p->code==ADDI2P||p->code==SUBIFP)
    p->typf=p->typf2;
  p->q2.val.vmax=sizetab[p->typf&NQ];
  p->code=ASSIGN;
  if(DEBUG&1024){printf("becomes\n");pric2(stdout,p);}
  return 1;
}

/* tries to replace objects by constants and detects some uninitialized */
/* variables; if cponly==0, address-constants will be replaced, */
/* otherwise only real constants are replaced, sic points to the IC */
/* containing the object pointed to by o */
int propagate(IC *sic,obj *o,int cponly,int global)
{
  unsigned int i,j,t,found;union atyps *val=0;
  Var *v,*vaddr=0;IC *p;
  zmax voff;
  if(!o||!o->v) ierror(0);
  if(cponly){
    if(is_volatile_obj(o))
      return 0;
  }else{
    if((o->flags&VAR)&&(o->v->vtyp->flags&VOLATILE))
      return 0;
  }
  if(disable&8) return 0;
  v=o->v;
  i=v->index;
  /* do not replace in ICs of the form move char with size!=1,
     because these are builtin memcpy */
  if(cponly&&sic->code==ASSIGN&&(sic->typf&NQ)==CHAR&&!zmeqto(sic->q2.val.vmax,l2zm(1L)))
    return 0;
  if(cponly&&(o->flags&DREFOBJ)) i+=vcount-rcount;
  if(DEBUG&2048){
    printf("propagate(cponly=%d) <%s>(%p)\n",cponly,o->v->identifier,(void *)v);
    if(o->flags&DREFOBJ) printf("(DREFOBJ)");
    printf("\nall reaching definitions:\n");print_rd(rd_defs);
    printf("defs for var:\n");
    print_rd(var_defs[i]);
  }
  if(v->nesting==0||v->storage_class==STATIC||v->storage_class==EXTERN){
    /*  Wenn moeglich bei statischen Variablen den Wert bei der         */
    /*  Initialisierung ermitteln.                                      */
    /*if(cponly&&ISARITH(v->vtyp->flags)&&((v->vtyp->flags&CONST)||(v->nesting>0&&!(v->flags&(USEDASADR|USEDASDEST))))){*/
    if(cponly&&is_const(v->vtyp)){
      /*  Variable hat noch den Wert der Initialisierung.         */
      if(v->clist){
	int t;
	if(o==&sic->q1) t=q1typ(sic);
	else if(o==&sic->q2) t=q2typ(sic);
	else if(o==&sic->z) t=ztyp(sic);
	else ierror(0);
	if(ISARITH(v->vtyp->flags)){
	  /*  Der Wert der Initialisierung ist noch gespeichert.  */
	  if(DEBUG&1024) printf("using static initializer\n");
	  o->val=v->clist->val;
	  o->flags=KONST;
	  return 1;
	}else if(ISINT(t&NQ)){
	  int state;
	  if(DEBUG&1024) printf("using static initializer (new version)\n");
	  gval.vumax=get_clist_int(v->vtyp,v->clist,o->val.vmax,sizetab[t&NQ],&state);
	  if(state){
	    if(DEBUG&1024) printf("using static initializer (new version)\n");
	    eval_const(&gval,UNSIGNED|MAXINT);
	    insert_const(&o->val,t);
	    o->flags=KONST;
	    return 1;
	  }
	}
      }else{
	/*  Hier evtl. eine implizite 0 erkennen.               */
      }
    }
  }
  found=0;
  for(j=1;j<=dcount;j++){
    if((!BTST(rd_defs,UNDEF(j))||!BTST(var_defs[i],UNDEF(j)))&&
       (!BTST(rd_defs,j)||!BTST(var_defs[i],j))) continue;
    found=1;
    p=dlist[j];
    if(!(p->z.flags&VAR)) return 0;
    if(p->z.v!=o->v) continue;
    t=ztyp(p)&NU;
    if(cponly&&!ISSCALAR(t)) continue;
    if(!zmeqto(p->z.val.vmax,o->val.vmax)) continue;
    if(cponly){
      if(p->z.flags!=o->flags) continue;
    }else{
      if((p->z.flags|DREFOBJ)!=o->flags) continue;
      if(p->z.flags&DREFOBJ) continue;
    }
    if(BTST(rd_defs,UNDEF(j))&&BTST(var_defs[i],UNDEF(j))) return 0;
    if(BTST(rd_defs,UNDEF(j))&&BTST(var_defs[i],j)) return 0;

    if((p->code!=ASSIGN||((p->q1.flags&(KONST|DREFOBJ))!=KONST&&(p->q1.flags&(VARADR|DREFOBJ))!=VARADR))
       &&(p->code!=ADDRESS||!(o->flags&DREFOBJ))) return 0;
    if(p->q1.flags&KONST){
      if(vaddr) return 0;
      if(val){
	if((p->typf&NQ)!=(t&NQ)) return 0;
	if(compare_const(&p->q1.val,val,t)) return 0;
      }else{
	val=&p->q1.val;t=p->typf&NU;
      }
    }
    if(p->code==ADDRESS||(p->q1.flags&VARADR)){
      if(val) return 0;
      if(vaddr){
	if(p->q1.v!=vaddr||!zmeqto(p->q1.val.vmax,voff)) return 0;
      }
      vaddr=p->q1.v;
      voff=p->q1.val.vmax;
    }
  }

  /* found constant */
  if(val){
    if(cponly){
      if(o==&sic->q1&&(q1typ(sic)&NQ)!=(t&NQ)) return 0;
      if(o==&sic->q2&&(q2typ(sic)&NQ)!=(t&NQ)) return 0;
      if(o==&sic->z&&(ztyp(sic)&NQ)!=(t&NQ)) return 0;
      
      if(DEBUG&1024) printf("can replace %ld+<%s>(%p) by constant\n",zm2l(o->val.vmax),o->v->identifier,o->v);
      /* TODO: do we need eval_const/insert_const (if representation of unsigned is different? */
      o->val=*val;
      o->flags=KONST;
    }else{
      if(DEBUG&1024) printf("can replace <%s> by constant address\n",o->v->identifier);
      o->val=*val;
      o->flags|=KONST;
      o->flags&=~VAR;
      o->v=0;
    }
    return 1;
  }
  if(vaddr&&(vaddr->storage_class==EXTERN||vaddr->storage_class==STATIC)){
    if(DEBUG&1024) printf("can replace <%s> by varadr\n",o->v->identifier);
    o->v=vaddr;
    o->val.vmax=voff;
    if((o->flags&DREFOBJ)&&!cponly)
      o->flags=VAR;
    else
      o->flags=VAR|VARADR;
    return 2;
  }
  if(vaddr&&(o->flags&DREFOBJ)){
    t=vaddr->vtyp->flags&NU;
    if(o==&sic->q1&&(q1typ(sic)&NU)!=t) return 0;
    if(o==&sic->q2&&(q2typ(sic)&NU)!=t) return 0;
    if(o==&sic->z&&(ztyp(sic)&NU)!=t) return 0;
    if(DEBUG&1024) printf("can replace *<%s> by address\n",o->v->identifier);
    o->v=vaddr;
    o->val.vmax=voff;
    o->flags&=~DREFOBJ;
    return 2;
  }
  /* no definition found */
  if(!found&&global&&v->storage_class!=EXTERN&&v->storage_class!=STATIC&&!(v->flags&USEDBEFORE)&&v->reg==0&&zmleq(l2zm(0L),v->offset)){
    if(*v->identifier||!(optflags&4096)){
#ifdef HAVE_MISRA
/* removed */
#endif
      error(171,v->identifier);v->flags|=USEDBEFORE;
      if(!*v->identifier) {printf("<%p>\n",(void *)v);ierror(0);}
    }
  }
  return 0;
}

/* searches for constant objects and uninitialized variables; if  */
/* global!=0, reaching definitions are used, otherwise only local */
/* constant propagation will be done                              */
int constant_propagation(flowgraph *fg,int global)
{
  IC *p;int changed=0,i,t;flowgraph *g;
  rd_defs=mymalloc(dsize);
  g=fg;
  while(g){
    if(global)
      memcpy(rd_defs,g->rd_in,dsize);
    else
      memset(rd_defs,0,dsize);
    p=g->start;
    while(p){
      int c=p->code;
      /* if(DEBUG&1024){print_rd(rd_defs);pric2(stdout,p);}*/
      if(c!=ADDRESS&&c!=NOP&&ISSCALAR(p->typf)&&(c<LABEL||c>BRA)){
	if((p->q1.flags&(VAR|VARADR))==VAR){
	  if(!(q1typ(p)&VOLATILE)&&!is_volatile_obj(&p->q1))
	    changed|=propagate(p,&p->q1,1,global);
	  if((p->q1.flags&DREFOBJ)&&!(p->q1.v->vtyp->flags&VOLATILE))
	    changed|=propagate(p,&p->q1,0,global);
	}
	if((p->q2.flags&(VAR|VARADR))==VAR){
	  if(!(q2typ(p)&VOLATILE)&&!is_volatile_obj(&p->q2))
	    changed|=propagate(p,&p->q2,1,global);
	  if((p->q2.flags&DREFOBJ)&&!(p->q2.v->vtyp->flags&VOLATILE))
	    changed|=propagate(p,&p->q2,0,global);
	}
      }
      /* only there to detect uninitialized variables */
      if(((p->z.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ))){
	if(!(p->z.v->vtyp->flags&VOLATILE))
	  changed|=propagate(p,&p->z,0,global);
      }
      rd_change(p);
      
      if(p==g->end) break;
      p=p->next;
    }
    break;
    g=g->normalout;
  }
  
  gchanged|=changed;
  
  free(rd_defs);
  return changed;
}

/* performs changes to rd_defs which are caused by IC p */
void rd_change(IC *p)
{
  if(DEBUG&4096) print_rd(rd_defs);
  if(p->defindex){
    bvdiff(rd_defs,defs_kill[p->defindex],dsize);
    bvunite(rd_defs,defs_gen[p->defindex],dsize);
  }
  if(DEBUG&4096) pric2(stdout,p);
}


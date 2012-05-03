/*  $VER: vbcc (alias.c) V0.8   */
/*  Listen benutzter/veraenderter Variablen und Behandlung von Decknamen.   */

#include "opt.h"

static char FILE_[]=__FILE__;

static bvtype **gpt;

static unsigned long ptsize;

#define is_restrict(i) ((i)<=vcount-rcount&&(vilist[i]->vtyp->flags&RESTRICT))

/* sets points-to-info for var i to undefined */
void undef_pt(bvtype **pt,int i)
{
  if(i<0||i>=vcount) ierror(0);
  if(pt[i]) ptsize-=vsize;
  free(pt[i]);
  pt[i]=0;
  if(i<rcount){
    i+=vcount-rcount;
    if(pt[i]) ptsize-=vsize;
    free(pt[i]);
    pt[i]=0;
  }
}

/* walks through a clist and sets the corresponding bit in bv
   for every variable whose address is contained in the clist */
void add_clist_refs(bvtype *bv,struct Typ *t,struct const_list *cl)
{
  /*FIXME: bei Aufrufen auch auf locale, nicht USEDASDEST|USEDASADDR */
  int i;zmax sz;
  if(ISARRAY(t->flags)){
    for(sz=l2zm(0L);!zmleq(t->size,sz)&&cl;sz=zmadd(sz,l2zm(1L)),cl=cl->next){
      if(!cl->other){ierror(0);return;}
      add_clist_refs(bv,t->next,cl->other);
    }
    return;
  }
  if(ISUNION(t->flags)){
    add_clist_refs(bv,(*t->exact->sl)[0].styp,cl);
    return;
  }
  if(ISSTRUCT(t->flags)){
    struct Typ *st;
    for(i=0;i<t->exact->count&&cl;i++){
      if(!cl->other){ierror(0);return;}
      st=(*t->exact->sl)[i].styp;
      if(!(*t->exact->sl)[i].identifier) ierror(0);
      if((*t->exact->sl)[i].identifier[0]){
        add_clist_refs(bv,st,cl->other);
        cl=cl->next;
      }
    }
    return;
  }
  if(cl->tree&&(cl->tree->o.flags&VARADR)){
    /* careful: variable might not have a valid index if it is not
       used within the function optimized */
    i=cl->tree->o.v->index;
    if(i>=0&&i<vcount-rcount&&vilist[i]==cl->tree->o.v){
      /*printf("add %s\n",vilist[i]->identifier);*/
      BSET(bv,i);
    }
  }
}

/* copies points-to-info for one var to another */
void copy_pt(bvtype **pt,int to,int from)
{
  if(to<0||to>=vcount) ierror(0);
  if(from<0||from>=vcount) ierror(0);
  if(!pt[from]){
    if(from>=vcount-rcount&&pt[from-(vcount-rcount)]){
      /* if dref check, whether from only points to initialized const */
      int i;bvtype *new;
      for(i=0;i<vcount;i++){
	if(BTST(pt[from-(vcount-rcount)],i)){
	  struct Var *v=vilist[i];
	  if(!is_const(v->vtyp)||!v->clist)
	    break;
	}
      }
      if(i==vcount){
	/* yes, take the points-to-info from clist */
	if(!pt[to]||to+vcount-rcount==from){
	  new=mymalloc(vsize);
	  ptsize+=vsize;
	}else
	  new=pt[to];
	memset(new,0,vsize);
	for(i=0;i<vcount;i++){
	  if(BTST(pt[from-(vcount-rcount)],i))
	    add_clist_refs(new,vilist[i]->vtyp,vilist[i]->clist);
	}
	if(to+vcount-rcount==from){
	  ptsize-=vsize;
	  free(pt[to]);
	}
	pt[to]=new;
	return;
      }
    }
    undef_pt(pt,to);
  }else{
    if(!pt[to]){
      pt[to]=mymalloc(vsize);
      ptsize+=vsize;
    }
    memcpy(pt[to],pt[from],vsize);
  }
}

/* set var i points only to j in points-to-info */
void set_pt(bvtype **pt,int i,int j)
{
  if(i<0||i>=vcount) ierror(0);
  if(j<0||j>=vcount) ierror(0);
  if(!pt[i]){
    pt[i]=mymalloc(vsize);
    ptsize+=vsize;
  }
  memset(pt[i],0,vsize);
  BSET(pt[i],j);
}

void print_single_pt(bvtype *pt)
{
  int j;
  if(pt){
    for(j=0;j<vcount;j++){
      if(BTST(pt,j))
	printf("  %s<%s>(%p)\n",(j>=vcount-rcount)?"*":"",vilist[j]->identifier,(void*)vilist[j]);
    }
  }else{
    printf("  (undefined)\n");
  }
}

void print_pt(bvtype **pt)
{
  int i,j;
  if(!pt) return;
  printf("points-to:\n");
  for(i=0;i<vcount;i++){
    printf("%s<%s>(%p):\n",(i>=vcount-rcount)?"*":"",vilist[i]->identifier,(void*)vilist[i]);
    print_single_pt(pt[i]);
  }
}

/* creates new points-to-info; every var set to undefined */
bvtype **new_pt(void)
{
  bvtype **pt;
  int i;
  pt=mymalloc(vcount*sizeof(*pt));
  ptsize+=vcount*sizeof(*pt);
  for(i=0;i<vcount;i++){
    if(i<rcount&&(vilist[i]->vtyp->flags&RESTRICT)){
      pt[i]=mymalloc(vsize);
      ptsize+=vsize;
      memset(pt[i],0,vsize);
      BSET(pt[i],i+vcount-rcount);
    }else
      pt[i]=0;
  }
  return pt;
}

void free_pt(bvtype **pt)
{
  int i;
  if(pt){
    for(i=0;i<vcount;i++){
      if(pt[i]) ptsize-=vsize;
      free(pt[i]);
    }
    ptsize-=vcount*sizeof(*pt);
    free(pt);
  }
}

/* set points-to-info of *v to union of points-to of all vars in the
   points-to-info of v */
void dref_pt(bvtype **pt,int i)
{
  int j,d;
  if(i<0||i>=rcount) ierror(0);
  d=i+vcount-rcount;
  if(!pt[i]){
    undef_pt(pt,d);
    return;
  }
  if(!pt[d]){
    pt[d]=mymalloc(vsize);
    ptsize+=vsize;
  }
  memset(pt[d],0,vsize);
  for(j=0;j<vcount;j++){
    if(BTST(pt[i],j)){
      struct Var *v=vilist[j];
      if(v->clist&&is_const(v->vtyp)){
	add_clist_refs(pt[d],v->vtyp,v->clist);
      }else if(!pt[j]){
	undef_pt(pt,d);
	return;
      }else
	bvunite(pt[d],pt[j],vsize);
    }
  }
}

void trans_pt(bvtype **pt,struct IC *p)
{
  int i,j,newset=-1,sv,cp;
  if((p->code==ADDI2P||p->code==SUBIFP)&&!compare_objs(&p->q1,&p->z,ztyp(p)))
    return;
  if((p->z.flags&VAR)&&ISSCALAR(p->z.v->vtyp->flags)){
    i=p->z.v->index;
    if(i<0||i>=vcount) ierror(0);
    if(p->z.flags&DREFOBJ){
      if(i>=rcount) ierror(0);
      i+=vcount-rcount;
    }
    if(!is_restrict(i)){
      if(p->code==ASSIGN){
	newset=i;sv=-1;cp=-1;
	if(p->q1.flags&VARADR){
	  sv=p->q1.v->index;
	  set_pt(pt,i,p->q1.v->index);
	}else if(p->q1.flags&VAR){
	  j=p->q1.v->index;
	  if(j<0||j>=vcount) ierror(0);
	  if(p->q1.flags&DREFOBJ){
	    if(j>=rcount) ierror(0);
	    j+=vcount-rcount;
	  }
	  cp=j;
	  copy_pt(pt,i,j);
	}else{
	  if(!pt[i]){
	    pt[i]=mymalloc(vsize);
	    ptsize+=vsize;
	  }
	  memset(pt[i],0,vsize);
	}
      }  
      if(p->code==ADDRESS){
	newset=i;sv=p->q1.v->index;cp=-1;
	set_pt(pt,i,p->q1.v->index);
      }
      if((p->code==ADDI2P||p->code==SUBIFP)&&(p->q1.flags&VAR)){
	newset=i;sv=-1;cp=-1;
	if(p->q1.flags&VARADR){
	  sv=p->q1.v->index;
	  set_pt(pt,i,p->q1.v->index);
	}else{
	  j=p->q1.v->index;
	  if(j<0||j>=vcount) ierror(0);
	  if(p->q1.flags&DREFOBJ){
	    if(j>=rcount) ierror(0);
	    j+=vcount-rcount;
	  }
	  cp=j;
	  copy_pt(pt,i,j);
	}
      }
    }
  }
  if(newset>=0&&newset<rcount){
    if(newset<0) ierror(0);
    dref_pt(pt,newset);
  }
  for(i=0;i<p->change_cnt;i++){
    j=p->change_list[i].v->index;
    if(j<0||j>=vcount) ierror(0);
    if(p->change_list[i].flags&DREFOBJ){
      if(j>=rcount) ierror(0);
      j+=vcount-rcount;
    }
    if(j!=newset&&!is_restrict(j)){
      if(newset>=0&&pt[j]){
	if(sv>=0){
	  if(sv>=vcount) ierror(0);
	  BSET(pt[j],sv);
	}
	if(cp>=0&&pt[cp]){
	  if(cp<0||cp>=vcount) ierror(0);
	  bvunite(pt[j],pt[cp],vsize);
	}
      }else
	undef_pt(pt,j);
    }
  }
}

bvtype **clone_pt(bvtype **pt)
{
  int i;
  bvtype **new;
  if(!pt)
    return new_pt();
  new=mymalloc(vcount*sizeof(*new));
  ptsize+=vcount*sizeof(*new);
  for(i=0;i<vcount;i++){
    if(pt[i]){
      new[i]=mymalloc(vsize);
      ptsize+=vsize;
      memcpy(new[i],pt[i],vsize);
    }else
      new[i]=0;
  }
  return new;
}

/* tests if two points-to-infos are identical */
int equal_pt(bvtype **pt1,bvtype **pt2)
{
  int i;
  if(!pt1&&!pt2) 
    return 1;
  if(!pt1||!pt2)
    return 0;
  for(i=0;i<vcount;i++){
    if(!pt1[i]&&!pt2[i])
      continue;
    if(!pt1[i]||!pt2[i])
      return 0;
    if(memcmp(pt1[i],pt2[i],vsize))
      return 0;
  }
  return 1;
}
#if 0
void calc_pt(struct flowgraph *fg)
{
  struct flowgraph *g;
  struct flowlist *in;
  struct IC *p;
  bvtype **pt,**ppt;
  int i,all_preds,changed;

  changed=1;
  while(changed){
    if(DEBUG&1024) printf("calc_pt pass\n");
    changed=0;
    for(g=fg;g;g=g->normalout){
      /* do all predecessors already have points-to-info? */
      all_preds=1;
      for(in=g->in;in;in=in->next){
	if(!in->graph->pt){
	  all_preds=0;
	  break;
	}
      }
      if(all_preds&&g->in){
	/* calc union of all predecessors */
	pt=clone_pt(g->in->graph->pt);
	for(in=g->in->next;in;in=in->next){
	  ppt=in->graph->pt;
	  for(i=0;i<vcount;i++){
	    if(pt[i]){
	      if(!ppt[i])
		undef_pt(pt,i);
	      else
		bvunite(pt[i],ppt[i],vsize);
	    }
	  }
	}
      }else
	pt=new_pt();
      for(p=g->start;p;p=p->next){
	trans_pt(pt,p);
	if(p==g->end)
	  break;
      }
      if(pt==g->pt) ierror(0);
      if(!changed){
	if(!equal_pt(pt,g->pt)){
	  changed=1;
	  free_pt(g->pt);
	  g->pt=pt;
	}else{
	  free_pt(pt);
	}
      }else{
	free_pt(g->pt);
	g->pt=pt;
      }
    }
  }
}
#endif
int p_typ(struct Var *v)
/*  Liefert den Typ, auf den Variable zeigen kann. Falls nicht eindeutig    */
/*  wird CHAR zurueckgegeben, da ein char * auf alles zeigen kann.          */
{
    struct Typ *t=v->vtyp;int f;
    /*  Kein Zeiger? Dann moeglicherweise Struktur, die verschiedene Zeiger */
    /*  enthalten koennte. Koennte man evtl. noch genauer pruefen.          */
    if(!ISPOINTER(t->flags)||!t->next||(v->flags&DNOTTYPESAFE)) return CHAR;
    f=t->next->flags&NQ;
    if(f==VOID) f=CHAR;
    return f;
}

/* propagates information if a variable whose address may have been taken
   is modified */
static void propagate_pointers(bvtype *set,struct Var *v,int t,int i)
{
  int j,t2;
  if(v->nesting==0||v->storage_class==EXTERN||(v->flags&USEDASADR)){
    if(noaliasopt){
      bvunite(set,av_drefs,vsize);
    }else{
      for(j=0;j<rcount;j++){
	t2=p_typ(vilist[j]);
	if(t==t2||t2==CHAR||!ISSCALAR(t)||!ISSCALAR(t2)){
	  if(!gpt||!gpt[j]||(i>=0&&BTST(gpt[j],i)))
	    BSET(set,j+vcount-rcount);
	}	    
      }
    }
  }
}
void alias_propagate(bvtype *set,int i,int t,int wr)
{
  int j,t2;
  struct Var *v;
  if(i<0||i>=vcount) ierror(0);
  t&=NQ;
  BSET(set,i);
  if(wr&&i<rcount) BSET(set,i+vcount-rcount);
  if(i>=vcount-rcount){
    /* DREFOBJ */
    if(noaliasopt||t==CHAR||!ISSCALAR(t)){
      bvunite(set,av_drefs,vsize);
      bvunite(set,av_address,vsize);
      bvunite(set,av_globals,vsize);
    }else{
      for(j=0;j<vcount-rcount;j++){
	v=vilist[j];
	if(!gpt||!gpt[i-(vcount-rcount)]||BTST(gpt[i-(vcount-rcount)],j)){
	  v=vilist[j];
	  if(!v) ierror(0);
	  if(v->nesting==0||v->storage_class==EXTERN||(v->flags&USEDASADR)){
	    struct Typ *tp=v->vtyp;
	    if(!v->vtyp) ierror(0);
	    do{
	      t2=tp->flags&NQ;
	      tp=tp->next;
	    }while(ISARRAY(t2));
	    if(t==t2||!ISSCALAR(t2)){
	      BSET(set,j);
	      if(wr&&j<rcount) {BSET(set,j+vcount-rcount);continue;}
	    }
	  }
	}
	if(j<rcount){
	  if(i<(vcount-rcount)) ierror(0);
	  if(!gpt||
	     (!gpt[i-(vcount-rcount)]&&!is_restrict(j))||
	     (!gpt[j]&&!is_restrict(i-(vcount-rcount)))||
	     (gpt[j]&&gpt[i-(vcount-rcount)]&&bvdointersect(gpt[i-(vcount-rcount)],gpt[j],vsize))){
	    t2=p_typ(v);
	    if(t==t2||t2==CHAR||!ISSCALAR(t2))
	      BSET(set,j+vcount-rcount);
	  }
	}
      }
    }
  }else{
    v=vilist[i];
    propagate_pointers(set,v,t,i);
  }
}

void ic_changes(struct IC *p,bvtype *result)
/*  Initialisiert den Bitvektor result mit allen Variablen, die durch das   */
/*  IC p geaendert werden koennten.                                         */
{
    int i,j,t,t2;struct Var *v;
    memset(result,0,vsize);
    t=(ztyp(p)&NQ);
    if(p->z.flags&VAR){
        v=p->z.v;
        i=v->index;
        /*  Hilfsvariable, die waehrend diesem cse-Durchlauf eingefuehrt    */
        /*  wurde.                                                          */
        if(i<0) return;
	if(i>=vcount) ierror(0);
        if(p->z.flags&DREFOBJ){
	  if(i>=rcount) ierror(0);
	  alias_propagate(result,i+vcount-rcount,t,1);
	}else{
	  alias_propagate(result,i,t,1);
	  if(i<rcount) BSET(result,i+vcount-rcount);
	}
    }
    if(p->code==CALL){
      struct function_info *fi;
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&(fi=p->q1.v->fi)&&(fi->flags&ALL_MODS)&&!(disable&65536)){
	/*  we can get the set from the function_info */
	for(i=0;i<fi->change_cnt;i++){
	  if(v=fi->change_list[i].v){
	    if(v->inr==inr&&v->index>=0){
	      if(v->index>=vcount) ierror(0);
	      alias_propagate(result,v->index,v->vtyp->flags&NQ,1);
	    }else
	      propagate_pointers(result,v,v->vtyp->flags&NQ,-1);
	  }else{
	    for(j=0;j<vcount-rcount;j++){
	      v=vilist[j];
	      if(v->nesting==0||v->storage_class==EXTERN||(v->flags&USEDASADR)){
		t=v->vtyp->flags&NQ;
		if(t==(fi->change_list[i].flags&NQ)||!ISSCALAR(t)){
		  BSET(result,j);
		  if(j<rcount) BSET(result,j+vcount-rcount);
		}
	      }
	      if(j<rcount){
		t=p_typ(vilist[j]);
		if(t==CHAR||!ISSCALAR(t)||t==fi->change_list[i].flags)
		  BSET(result,j+vcount-rcount);
	      }
	    }
	  }
	}
      }else{
	bvunite(result,av_drefs,vsize);
	bvunite(result,av_address,vsize);
	bvunite(result,av_globals,vsize);
	bvunite(result,av_statics,vsize);
      }
    }
    if((p->z.flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
      bvunite(result,av_drefs,vsize);
    }
}
void ic_uses(struct IC *p,bvtype *result)
/*  Initialisiert den Bitvektor result mit allen Variablen, die durch das   */
/*  IC p benutzt werden koennten.                                           */
{
    int i,j,t,t2,c;struct Var *v;struct Typ *tp;
    memset(result,0,vsize);
    c=p->code;
    if(c!=ADDRESS){
        if((p->q1.flags&(VAR|VARADR))==VAR&&c!=ADDRESS&&(c!=CALL||(p->q1.flags&DREFOBJ))){
            v=p->q1.v;
            i=v->index;
	    if(i<0||i>=vcount) ierror(0);
	    t=q1typ(p);
	    if(p->q1.flags&DREFOBJ){
	      if(i>=rcount) ierror(0);
	      alias_propagate(result,i+vcount-rcount,t,0);
	    }
	    alias_propagate(result,i,t,0);
        }
        if((p->q2.flags&(VAR|VARADR))==VAR){
            v=p->q2.v;
            i=v->index;
	    if(i<0||i>=vcount) ierror(0);
	    t=q2typ(p);
	    if(p->q2.flags&DREFOBJ){
	      if(i>=rcount) ierror(0);
	      alias_propagate(result,i+vcount-rcount,t,0);
	    }
	    alias_propagate(result,i,t,0);
        }
    }
    if((p->z.flags&(VAR|VARADR|DREFOBJ))==(VAR|DREFOBJ)){
        v=p->z.v;
        i=v->index;
        if(i>=vcount) {pric2(stdout,p);ierror(0);}
	t=(ztyp(p)&NQ);
	alias_propagate(result,i,t,0);
    }
    if(p->code==CALL){
      struct function_info *fi;
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&(fi=p->q1.v->fi)&&(fi->flags&ALL_USES)&&!(disable&65536)){
	/*  we can get the set from the function_info */
	for(i=0;i<fi->use_cnt;i++){
	  if(v=fi->use_list[i].v){
	    if(v->inr==inr&&v->index>=0)
	      alias_propagate(result,v->index,v->vtyp->flags&NQ,0);
	    else
	      propagate_pointers(result,v,v->vtyp->flags&NQ,-1);
	  }else{
	    for(j=0;j<vcount-rcount;j++){
	      v=vilist[j];
	      if(v->nesting==0||v->storage_class==EXTERN||(v->flags&USEDASADR)){
		t=v->vtyp->flags&NQ;
		if(t==(fi->use_list[i].flags&NQ)||!ISSCALAR(t))
		  BSET(result,j);
	      }
	      if(j<rcount){
		t=p_typ(vilist[j]);
		if(t==CHAR||!ISSCALAR(t)||t==fi->use_list[i].flags)
		  BSET(result,j+vcount-rcount);
	      }
	    }
	  }
	}
      }else{
	bvunite(result,av_drefs,vsize);
	bvunite(result,av_address,vsize);
	bvunite(result,av_globals,vsize);
	bvunite(result,av_statics,vsize);
      }
    }
    if((p->q1.flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
      bvunite(result,av_drefs,vsize);
    }
    if((p->q2.flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
      bvunite(result,av_drefs,vsize);
    }
}
void free_alias(struct flowgraph *fg)
/*  Gibt alle use/change-Listen der ICs im Flussgraphen frei.               */
{
    struct IC *p;struct flowgraph *g;
    if(DEBUG&1024) printf("freeing alias info\n");
    for(g=fg;g;g=g->normalout){
        for(p=g->start;p;p=p->next){
            if(p->code==LABEL&&(p->use_cnt>0||p->change_cnt>0)) ierror(0);
            if(p->use_cnt>0) {free(p->use_list);p->use_cnt=0;}
            if(p->change_cnt>0) {free(p->change_list);p->change_cnt=0;}
            if(p==g->end) break;
        }
    }
    have_alias=0;
}
void create_alias(struct flowgraph *fg)
/*  Initialisiert jedes IC mit einer Liste aller Variablen, die dadurch     */
/*  benutzt und veraendert werden koennten. Z.Z. wird bis auf Typ-basierte  */
/*  Optimierungen der worst-case angenommen.                                */
{
  bvtype *vars=mymalloc(vsize);
  struct IC *p;struct flowgraph *g;
  struct flowlist *in;
  bvtype **ppt;
  int i,cnt,all_preds,changed;
  unsigned long heapsize;
  if(DEBUG&1024) printf("creating alias info\n");

  ptsize=0;
  
  changed=1;
  while(changed){
    if(DEBUG&1024) printf("create_alias pass\n");
    changed=0;
    if(have_alias)
      free_alias(fg);
    heapsize=0;
    for(g=fg;g;g=g->normalout){
      if((optflags&1024)&&!noaliasopt){
	/* do all predecessors already have points-to-info? */
	all_preds=1;
	for(in=g->in;in;in=in->next){
	  if(!in->graph->pt){
	    all_preds=0;
	    break;
	  }
	}
	if(/*all_preds&&*/g->in){
	  /* calc union of all predecessors */
	  gpt=clone_pt(g->in->graph->pt);
	  for(in=g->in->next;in;in=in->next){
	    ppt=in->graph->pt;
	    if(!ppt)
	      continue;
	    for(i=0;i<vcount;i++){
	      if(gpt[i]){
		if(!ppt[i])
		  undef_pt(gpt,i);
		else
		  bvunite(gpt[i],ppt[i],vsize);
	      }
	    }
	  }
	}else
	  gpt=new_pt();                 
      }else{
	gpt=0;
      }
      for(p=g->start;p;p=p->next){
	ic_uses(p,vars);
	for(i=0,cnt=0;i<vcount;i++)
	  if(BTST(vars,i)) cnt++;
	p->use_cnt=cnt;
	if(cnt==0){
	  p->use_list=0;
	}else{
	  p->use_list=mymalloc(cnt*VLS);
	  heapsize+=cnt*VLS;
	  for(cnt=0,i=0;i<vcount;i++){
	    if(BTST(vars,i)){
	      p->use_list[cnt].v=vilist[i];
	      if(i>=vcount-rcount) p->use_list[cnt].flags=DREFOBJ;
	      else         p->use_list[cnt].flags=0;
	      cnt++;
	    }
	  }
	}
	ic_changes(p,vars);
	for(i=0,cnt=0;i<vcount;i++)
	  if(BTST(vars,i)&&(i>=vcount-rcount||!is_const(vilist[i]->vtyp))) cnt++;
	p->change_cnt=cnt;
	if(cnt==0){
	  p->change_list=0;
	}else{
	  p->change_list=mymalloc(cnt*VLS);
	  heapsize+=cnt*VLS;
	  for(cnt=0,i=0;i<vcount;i++){
	    if(BTST(vars,i)&&(i>=vcount-rcount||!is_const(vilist[i]->vtyp))){
	      p->change_list[cnt].v=vilist[i];
	      if(i>=vcount-rcount) p->change_list[cnt].flags=DREFOBJ;
	      else         p->change_list[cnt].flags=0;
	      cnt++;
	    }
	  }
	}

	if(p->code==CALL){
	  if((p->q1.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)){
	    int i=p->q1.v->index;
	    if(i<0||i>=vcount) ierror(0);
	    if(gpt&&gpt[i]){
	      int j,cnt;
	      for(cnt=0,j=0;j<vcount-rcount;j++){
		if(BTST(gpt[i],j)&&ISFUNC(vilist[j]->vtyp->flags))
		  cnt++;
	      }
	      if(cnt){
		if(p->call_cnt) free(p->call_list);
		p->call_cnt=cnt;
		p->call_list=mymalloc(sizeof(*p->call_list)*cnt);
		for(cnt=0,j=0;j<vcount-rcount;j++){
		  if(BTST(gpt[i],j)&&ISFUNC(vilist[j]->vtyp->flags)){
		    p->call_list[cnt].v=vilist[j];
		    p->call_list[cnt].flags=0;
		    cnt++;
		  }
		}
		if(cnt==1){
		  p->q1.flags=VAR;
		  p->q1.val.vmax=l2zm(0L);
		  p->q1.v=p->call_list[0].v;
		}
	      }
	    }
	  }	  
	}

	if((optflags&1024)&&!noaliasopt)
	  trans_pt(gpt,p);
	if(p==g->end) break;
      }
      if((optflags&1024)&&!noaliasopt){
	if(!changed){
	  if(!equal_pt(gpt,g->pt)){
	    changed=1;
	    free_pt(g->pt);
	    g->pt=gpt;
	  }else{
	    free_pt(gpt);
	  }
	}else{
	  free_pt(g->pt);
	  g->pt=gpt;
	}          
      }           
    }
    if(DEBUG&16384) printf("create_alias heapsize=%lu\n",heapsize);
    have_alias=1;
  }
  if(DEBUG&16384) printf("points-to heapsize=%lu\n",ptsize);
  if((optflags&1024)&&!noaliasopt){
    for(g=fg;g;g=g->normalout){
      free_pt(g->pt);
      g->pt=0;
    }
  }
  free(vars);
}
#if 1
void update_alias(struct Var *old,struct Var *new)
/*  Aendert alle use/changes von (old) auf (new). Wird aufgerufen, wenn     */
/*  copy-propagation eine Variable neu zu einem DREFOBJ macht.              */
{
    struct IC *p;int i;
    if(DEBUG&1024) printf("update-alias\n");
    for(p=first_ic;p;p=p->next){
        for(i=0;i<p->use_cnt;i++){
            if(p->use_list[i].v==old&&(p->use_list[i].flags&DREFOBJ)){
                p->use_cnt++;
                p->use_list=myrealloc(p->use_list,p->use_cnt*VLS);
                p->use_list[p->use_cnt-1].v=new;
                p->use_list[p->use_cnt-1].flags=DREFOBJ;
                break;
            }
        }
        for(i=0;i<p->change_cnt;i++){
	  if(p->change_list[i].v==new||(p->change_list[i].v==old&&(p->change_list[i].flags&DREFOBJ))){
                p->change_cnt++;
                p->change_list=myrealloc(p->change_list,p->change_cnt*VLS);
                p->change_list[p->change_cnt-1].v=new;
                p->change_list[p->change_cnt-1].flags=DREFOBJ;
                break;
            }
        }
    }
}
#endif

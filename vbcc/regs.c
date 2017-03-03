/*  $VER: vbcc (regs.c) V0.8    */
/*  Registerzuteilung           */

#include "opt.h"

static char FILE_[]=__FILE__;

#ifndef NO_OPTIMIZER

int (*savings)[MAXR+1],regu[MAXR+1];
int *rvlist;

static int const_vars;

struct regp {int treg;struct Var *tvar,*tmp;};
void do_load_parms(struct regp [],struct flowgraph *);
void load_one_parm(int,int,struct Var *,struct Var *,struct flowgraph *);

static void insert_cobj(struct obj *o,struct obj *co);

#ifndef HAVE_TARGET_RALLOC
int reg_prio[MAXR+1];
#define cost_move_reg(x,y) 1
#define cost_load_reg(x,y) 2
#define cost_save_reg(x,y) 2
#define cost_pushpop_reg(x) 4

int cost_savings(struct IC *p,int r,struct obj *o)
{
  int c=p->code;
  if(o->flags&VKONST) return 0;
  if(o->flags&DREFOBJ){
    int t;
    if(o==&p->q1){
      t=q1typ(p);
    }else if(o==&p->q2){
      t=q2typ(p);
    }else{
      t=ztyp(p);
    }
    if(regok(r,o->v->vtyp->flags,t)) return 4;
  }
  if(c==SETRETURN&&r==p->z.reg&&!(o->flags&DREFOBJ)) return 3;
  if(c==GETRETURN&&r==p->q1.reg&&!(o->flags&DREFOBJ)) return 3;
  return 2;
}
#endif

int cmp_savings(const void *v1,const void *v2)
/*  Vergleichsfkt, um rvlist nach savings zu sortieren  */
{
    return savings[*(int *)v2][0]-savings[*(int *)v1][0];
}
int entry_load(struct flowgraph *fg,int i)
/*  Testet, ob die Variable in Register i am Anfang von Block fg geladen    */
/*  werden muss, d.h. ein Vorgaenger sie nicht im selben Register hat.      */
{
    struct flowlist *lp;
    lp=fg->in;
    while(lp){
        if(lp->graph&&lp->graph->regv[i]!=fg->regv[i]&&(fg->regv[i]->index>=vcount-rcount||BTST(lp->graph->av_out,fg->regv[i]->index))) return 1;
        lp=lp->next;
    }
    return 0;
}

/* checks if variable i can be held in a register in loop start-end */
/* currently we check whether an IC uses/modifies the variable and */
/* has a corresponding drefobj which might alias the variable */
int check_cacheable(int i,struct flowgraph *start,struct flowgraph *end)
{
  int j,k;
  struct IC *p;struct Var *v;
  struct flowgraph *g;
  /* constants are always cacheable */
  if(vilist[i]->identifier[0]==' ')
    return 1;
  if(i<vcount-rcount){
    /* simple local variables are also easy */
    v=vilist[i];
    if(v->storage_class!=STATIC&&v->storage_class!=EXTERN&&!(v->flags&USEDASADR))
      return 1;
  }
  if(disable&16384) return 0;
  for(g=start;g;g=g->normalout){
    for(p=g->start;p;p=p->next){
      for(j=0;j<p->use_cnt;j++){
	k=p->use_list[j].v->index;
	if(p->use_list[j].flags&DREFOBJ)
	  k+=vcount-rcount;
	if(k==i&&(p->code==CALL||(p->q1.flags&DREFOBJ)||(p->q2.flags&DREFOBJ)))
	  return 0;
      }
      for(j=0;j<p->change_cnt;j++){
	k=p->change_list[j].v->index;
	if(p->change_list[j].flags&DREFOBJ)
	  k+=vcount-rcount;
	if(k==i&&(p->code==CALL||(p->z.flags&DREFOBJ)))
	  return 0;
      }
      if(p==g->end) break;
    }
    if(g==end) break;
  }
  /*printf("assigned static %s to reg\n",vilist[i]->identifier);*/
  return 1;
}
int exit_save(struct flowgraph *fg,int i)
/*  Testet, ob die Variable in Register i am Ende von Block fg gespeichert  */
/*  werden muss, d.h. der Vorgaenger eines Nachfolgers nicht dieselbe       */
/*  Variable im selben Register hat.                                        */
{
    struct flowlist *lp;
    /* Konstanten muessen nie gespeichert werden */
    if(fg->regv[i]->index>=vcount-rcount) return 0;
    if(!fg->normalout){
      /* letzter Block, statische Variablen und drefobjs aktiv */
      int sc=fg->regv[i]->storage_class;
      return sc==STATIC||sc==EXTERN;
    }
    if((fg->normalout&&(!fg->end||fg->end->code!=BRA))&&BTST(fg->normalout->av_in,fg->regv[i]->index)){
        if(fg->normalout->regv[i]!=fg->regv[i]) return 1;
        lp=fg->normalout->in;
        while(lp){
            if(lp->graph&&lp->graph->regv[i]!=fg->regv[i]) return 1;
            lp=lp->next;
        }
    }
    if(fg->branchout&&BTST(fg->branchout->av_in,fg->regv[i]->index)){
        if(fg->branchout->regv[i]!=fg->regv[i]) return 1;
        lp=fg->branchout->in;
        while(lp){
            if(lp->graph&&lp->graph->regv[i]!=fg->regv[i]) return 1;
            lp=lp->next;
        }
    }
    return 0;
}
void load_reg_parms(struct flowgraph *fg)
/*  Laedt Registerparameter, falls noetig.                              */
{
    int i,j; struct Var *v;
    struct regp regp[MAXR+1]={0};
/*     for(i=1;i<=MAXR;i++){ regp[i].treg=0;regp[i].tvar=0;} */
    for(i=0;i<vcount-rcount;i++){
        v=vilist[i];
        if((v->flags&REGPARM)&&(fg->regv[abs(v->reg)]!=v||(v->flags&CONVPARAMETER))&&(BTST(fg->av_in,i)||(v->flags&USEDASADR))){
	  regp[abs(v->reg)].tvar=v;
	  for(j=1;j<=MAXR;j++)
	    if(fg->regv[j]==v) regp[abs(v->reg)].treg=j;
	}
    }
    do_load_parms(regp,fg);
}

void insert_regs(struct flowgraph *fg1)
/*  Fuegt Registervariablen in die ICs ein.                             */
{
    int i;struct IC *p,*lic=0,*new;struct flowgraph *lfg=0,*fg;
    if(DEBUG&9216) printf("inserting register variables\n");
    fg=fg1;
    while(fg){
        if(DEBUG&8192) printf("block %d:\n",fg->index);
        p=fg->start;
        while(p){
            for(i=1;i<=MAXR;i++){
                if(!fg->regv[i]) continue;
                if(p->code==ALLOCREG&&p->q1.reg==i) ierror(0);
                if((p->q1.flags&(VAR|REG|DONTREGISTERIZE))==VAR&&p->q1.v==fg->regv[i]){
		    if(p->q1.v->index>=vcount-rcount)
		      p->q1.flags&=~(KONST|VAR|VARADR);
		    p->q1.flags|=REG;
                    p->q1.reg=i;
                }
                if((p->q2.flags&(VAR|REG|DONTREGISTERIZE))==VAR&&p->q2.v==fg->regv[i]){
		    if(p->q2.v->index>=vcount-rcount)
		      p->q2.flags&=~(KONST|VAR|VARADR);
		    p->q2.flags|=REG;
                    p->q2.reg=i;
                }
                if((p->z.flags&(VAR|REG|DONTREGISTERIZE))==VAR&&p->z.v==fg->regv[i]){
		  if(p->z.v->index>=vcount-rcount)
		    p->z.flags&=~(KONST|VAR|VARADR);
		  p->z.flags|=REG;
		  p->z.reg=i;
                }
            }
	    /* evtl. const-Variablen ruecksubstituieren */
	    if((p->q1.flags&VAR)&&p->q1.v->index>=vcount-rcount){
	      insert_cobj(&p->q1,&p->q1.v->cobj);
	    }
	    if((p->q2.flags&VAR)&&p->q2.v->index>=vcount-rcount){
	      insert_cobj(&p->q2,&p->q2.v->cobj);
	    }
	    if((p->z.flags&VAR)&&p->z.v->index>=vcount-rcount){
	      insert_cobj(&p->z,&p->z.v->cobj);
	    }
            if(DEBUG&8192) pric2(stdout,p);
            if(p==fg->end) break;
            p=p->next;
        }
        if(fg->start&&fg->start->code==LABEL) lic=fg->start;
        for(i=1;i<=MAXR;i++){
            if(fg->regv[i]){
                if(DEBUG&8192){
                    printf("(%s),%ld(%p) assigned to %s\n",fg->regv[i]->identifier,zm2l(fg->regv[i]->offset),(void *)fg->regv[i],regnames[i]);
                    if(fg->regv[i]->index>=vcount-rcount||BTST(fg->av_in,fg->regv[i]->index)) printf("active at the start of block\n");
                    if(fg->regv[i]->index<vcount-rcount&&BTST(fg->av_out,fg->regv[i]->index)) printf("active at the end of block\n");
                }

                if(fg->regv[i]->index<vcount-rcount&&BTST(fg->av_out,fg->regv[i]->index)){
                /*  Variable beim Austritt aktiv?   */
                    if(exit_save(fg,i)){
                        struct IC *tp;
                        if(DEBUG&8192) printf("\thave to save it at end of block\n");
                        new=new_IC();
                        new->line=0;
                        new->file=0;
                        new->code=ASSIGN;
                        new->typf=fg->regv[i]->vtyp->flags;
                        /* vla */
                        if(ISARRAY(new->typf)) 
                          new->typf=POINTER_TYPE(fg->regv[i]->vtyp->next);
                        /*  cc  */
                        if(new->typf==0) ierror(0);
                        new->q1.flags=VAR|REG;
                        new->q1.val.vmax=l2zm(0L);
                        new->q1.v=fg->regv[i];
                        new->q1.reg=i;
                        new->q2.flags=0;
                        new->q2.val.vmax=szof(fg->regv[i]->vtyp);
                        new->z.flags=VAR|DONTREGISTERIZE;
                        new->z.val.vmax=l2zm(0L);
                        new->z.v=fg->regv[i];
                        new->q1.am=new->q2.am=new->z.am=0;
                        new->use_cnt=new->change_cnt=0;
                        new->use_list=new->change_list=0;
                        /*  Vor FREEREGs und evtl. Branch+COMPARE/TEST setzen   */
                        if(fg->end){
                            tp=fg->end;
                            while(tp!=fg->start&&(tp->code==FREEREG||tp->code==SETRETURN))
                                tp=tp->prev;
                            if(tp&&tp->code>=BEQ&&tp->code<=BRA){
                                if(tp->code<BRA){
                                    int c;
                                    do{
                                        tp=tp->prev;
                                        c=tp->code;
                                        if(c!=FREEREG&&c!=COMPARE&&c!=TEST) ierror(0);
                                    }while(c!=COMPARE&&c!=TEST);
                                }
                                tp=tp->prev;
                            }
                        }else tp=lic;
                        insert_IC_fg(fg,tp,new);
                    }
                }
                if(fg->regv[i]->index>=vcount-rcount||BTST(fg->av_in,fg->regv[i]->index)){
                    if((fg==fg1||entry_load(fg,i))&&(fg!=fg1||!(fg->regv[i]->flags&REGPARM))){
                        if(DEBUG&8192) printf("\thave to load it at start of block\n");

                        new=new_IC();
                        new->line=0;
                        new->file=0;
                        new->code=ASSIGN;
                        new->typf=fg->regv[i]->vtyp->flags;
                        /* vla */
                        if(ISARRAY(new->typf)) 
                          new->typf=POINTER_TYPE(fg->regv[i]->vtyp->next);
                        /*  cc  */
                        if(new->typf==0) ierror(0);
			if(fg->regv[i]->index>=vcount-rcount){
			  insert_cobj(&new->q1,&fg->regv[i]->cobj);
			}else{
			  new->q1.flags=VAR|DONTREGISTERIZE;
			  new->q1.val.vmax=l2zm(0L);
			  new->q1.v=fg->regv[i];
			}
			new->q2.flags=0;
                        new->q2.val.vmax=szof(fg->regv[i]->vtyp);
			if(fg->regv[i]->index>=vcount-rcount)
			  new->z.flags=REG;
			else
			  new->z.flags=VAR|REG;
                        new->z.val.vmax=l2zm(0L);
                        new->z.v=fg->regv[i];
                        new->z.reg=i;
                        new->q1.am=new->q2.am=new->z.am=0;
                        new->use_cnt=new->change_cnt=0;
                        new->use_list=new->change_list=0;
                        insert_IC_fg(fg,lic,new);
                    }
                }
                if(!lfg||!lfg->regv[i]) insert_allocreg(fg,lic,ALLOCREG,i);
                if(!fg->normalout||!fg->normalout->regv[i])
                    insert_allocreg(fg,fg->end?fg->end:lic,FREEREG,i);
            }
        }
        if(fg->end) lic=fg->end;
        lfg=fg;
        fg=fg->normalout;
    }
    load_reg_parms(fg1);
}

void do_loop_regs(struct flowgraph *start,struct flowgraph *end,int intask)
/*  Macht die Variablenzuweisung in Schleife start-end.                 */
/*  Wenn end==0 Registerzuweisung fuer die ganze Funktion, ansonsten    */
/*  fuer die Schleife, die zum Header start gehoert.                    */
{
  struct flowgraph *g,*lend;
  int i,r,iterations;
  struct Var *lregs[MAXR+1]={0};
  unsigned char regu[MAXR+1]={0};
  bvtype *isused=mymalloc(vsize);
  /*  Berechnen, wieviel ungefaehr eingespart wird, wenn eine Variable    */
  /*  fuer diese Schleife in einem best. Register gehalten wird.          */
  /*  Die savings in einer Schleife werden multipliziert, um das          */
  /*  Laden/Speichern ausserhalb der Schleife geringer zu wichten.        */
  /*    if(end&&(!g->normalout||!g->normalout->loopend||g->normalout->loopend->normalout->index!=-2)) ierror(0);*/
  /*  alle auf 0  */
  for(i=0;i<vcount-rcount+const_vars;i++){
    if(i<vcount-rcount&&(vilist[i]->vtyp->flags&VOLATILE)){
      for(r=1;r<=MAXR;r++){
	savings[i][r]=INT_MIN;
      }
    }else{
      for(r=1;r<=MAXR;r++){
	savings[i][r]=0;
      }
    }
  }
  if(end){
    struct Var *v;
    /* estimated number of iterations, avoid too big values */
    if(optsize)
      iterations=1;
    else
      iterations=4;
    /*  Evtl. Kosten fuers Laden/Speichern beim Ein-/Austritt in die    */
    /*  Schleife.                                                       */
    end=start->normalout->loopend;
    g=end->normalout;
    if(DEBUG&9216) printf("assigning regs to blocks %d to %d\n",start->normalout->index,end->index);
    /*  Werte modifizieren, falls Variable am Anfang/Ende der Schleife  */
    /*  geladen/gespeichert werden muss.                                */
    /*  FIXME: Evtl. noetige stores/loads in anderen Bloecken!          */
    for(i=0;i<vcount-rcount+const_vars;i++){
      v=vilist[i];
      if(i>=vcount-rcount||BTST(start->av_in,i)){
 	for(r=1;r<=MAXR;r++)
	  if(start->regv[r]!=v&&savings[i][r]!=INT_MIN)
	    savings[i][r]-=cost_load_reg(r,v);
      }
      if(i<vcount-rcount&&BTST(g->av_out,i)){
	for(r=1;r<=MAXR;r++)
	  if(g->regv[r]!=v&&savings[i][r]!=INT_MIN)
	    savings[i][r]-=cost_save_reg(r,v);
      }
    }
    /*  Werte modifizieren, falls eine andere Variable gespeichert oder */
    /*  geladen werden muss. Hmm..stimmt das so?                        */
    for(r=1;r<=MAXR;r++){
      v=start->regv[r];
      if(v&&v->index<vcount-rcount&&BTST(start->av_in,v->index)){
	for(i=0;i<vcount-rcount;i++)
	  if(v->index!=i&&savings[i][r]!=INT_MIN)
	    savings[i][r]-=cost_load_reg(r,vilist[i]);
      }
      if(v&&v->index<vcount-rcount&&BTST(g->av_out,v->index)){
	for(i=0;i<vcount-rcount;i++)
	  if(v->index!=i&&savings[i][r]!=INT_MIN)
	    savings[i][r]-=cost_save_reg(r,vilist[i]);
      }
    }
    g=start->normalout;
  }else{
    iterations=1;
    /*  Bei Registervergabe fuer die ganze Funktion muessen alle beim   */
    /*  Eintritt der Funktion aktiven Variablen geladen werden.         */
    /* statics have to be saved at the end */
    if(DEBUG&9216) printf("assigning regs to whole function\n");
    for(i=0;i<vcount-rcount+const_vars;i++){
      if(i<vcount-rcount&&(vilist[i]->storage_class==STATIC||vilist[i]->storage_class==EXTERN)){
	for(r=1;r<=MAXR;r++)
	  if(savings[i][r]!=INT_MIN)
	    savings[i][r]-=cost_save_reg(r,vilist[i]);
      }
      if(i>=vcount-rcount||BTST(start->av_in,i)){
	int pr=abs(vilist[i]->reg);
	for(r=1;r<=MAXR;r++){
	  if(savings[i][r]!=INT_MIN){
	    if(pr==0){
	      savings[i][r]-=cost_load_reg(r,vilist[i]);
	    }else if(!regsa[r]){
	      if(r==pr)
		savings[i][r]+=cost_save_reg(r,vilist[i]);
	      else
		savings[i][r]+=(cost_save_reg(r,vilist[i])-cost_move_reg(pr,r));
	    }
	  }
	}
      }
    }
    /*  Nonscratchregister muessen geladen/gesichert werden. */
    /*FIXME: einige unschoene Effekte(?) */
    if(!intask){
      for(r=1;r<=MAXR;r++){
	if(!regscratch[r]&&!regsa[r]){
	  for(i=0;i<vcount-rcount+const_vars;i++)
	    if(savings[i][r]!=INT_MIN)
	      savings[i][r]-=cost_pushpop_reg(r);
	}
      }
    }
    g=start;
  }
  if(DEBUG&9216) printf("calculating approximate savings\n");

  lend=0;
  for(;g;g=g->normalout){
    struct IC *p;struct Var *v;
    int t,vt;
    if(!end&&!lend){
      lend=g->loopend;
      if(lend&&iterations==1&&!optsize) iterations=4;
    }
    /*  Wenn das Register in dem Block benutzt wird, muss man es retten */
    for(r=1;r<=MAXR;r++){
      if(BTST(g->regused,r)||(reg_pair(r,&rp)&&(BTST(g->regused,rp.r1)||BTST(g->regused,rp.r2)))){
	int vi;
	if(g->regv[r]) vi=g->regv[r]->index; else vi=-1;
	for(i=0;i<vcount-rcount+const_vars;i++){
	  if(i>=vcount-rcount||BTST(g->av_out,i)||BTST(g->av_gen,i))
	    if(vi!=i&&savings[i][r]!=INT_MIN)
	      savings[i][r]-=iterations*cost_load_reg(r,vilist[i])+cost_save_reg(r,vilist[i]);
	}
      }
    }
    memcpy(isused,g->av_out,vsize);
    p=g->end;
    while(p){
      if((p->q1.flags&(VAR|VARADR|REG))==VAR){
	v=p->q1.v;vt=v->vtyp->flags;
        /* vla */
        if(ISARRAY(vt)&&is_vlength(v->vtyp))
          vt=POINTER_TYPE(v->vtyp->next);
	if((optflags&1024)||((v->storage_class==AUTO||v->storage_class==REGISTER)&&!(v->flags&USEDASADR))){
	  i=v->index;
	  if(p->code==ADDRESS||((p->code==ASSIGN||p->code==PUSH)&&(q1typ(p)&NQ)==(CHAR)&&!zmeqto(p->q2.val.vmax,l2zm(1L)))){
	    for(r=1;r<=MAXR;r++)
	      savings[i][r]=INT_MIN;
	  }else{
	    for(r=1;r<=MAXR;r++){
	      if(!regsa[r]&&!BTST(g->regused,r)&&savings[i][r]!=INT_MIN&&regok(r,vt,-1)&&(!reg_pair(r,&rp)||(!regsa[rp.r1]&&!regsa[rp.r2]&&!BTST(g->regused,rp.r1)&&!BTST(g->regused,rp.r2)))){
		int s=cost_savings(p,r,&p->q1);
		if(s==INT_MIN)
		  savings[i][r]=INT_MIN;
		else
		  savings[i][r]+=iterations*s;
	      }
	    }
	  }
	}
      }
      if((p->q2.flags&(VAR|VARADR|REG))==VAR){
	v=p->q2.v;vt=v->vtyp->flags;
        /* vla */
        if(ISARRAY(vt)&&is_vlength(v->vtyp))
          vt=POINTER_TYPE(v->vtyp->next);
	if((optflags&1024)||((v->storage_class==AUTO||v->storage_class==REGISTER)&&!(v->flags&USEDASADR))){
	  i=v->index;
	  for(r=1;r<=MAXR;r++){
	    if(!regsa[r]&&!BTST(g->regused,r)&&savings[i][r]!=INT_MIN&&regok(r,vt,-1)&&(!reg_pair(r,&rp)||(!regsa[rp.r1]&&!regsa[rp.r2]&&!BTST(g->regused,rp.r1)&&!BTST(g->regused,rp.r2)))){
	      int s=cost_savings(p,r,&p->q2);
	      if(s==INT_MIN)
		savings[i][r]=INT_MIN;
	      else
		savings[i][r]+=iterations*s;
	    }
	  }
	}
      }
      if((p->z.flags&(VAR|VARADR|REG))==VAR){
	v=p->z.v;vt=v->vtyp->flags;
        if(ISARRAY(vt)&&is_vlength(v->vtyp))
          vt=POINTER_TYPE(v->vtyp->next);
	if((optflags&1024)||((v->storage_class==AUTO||v->storage_class==REGISTER)&&!(v->flags&USEDASADR))){
	  i=v->index;
	  if(p->code==ASSIGN&&(ztyp(p)&NQ)==(CHAR)&&!zmeqto(p->q2.val.vmax,l2zm(1L))){
	    for(r=1;r<=MAXR;r++)
	      savings[i][r]=INT_MIN;
	  }
	  for(r=1;r<=MAXR;r++){
	    if(!regsa[r]&&!BTST(g->regused,r)&&savings[i][r]!=INT_MIN&&regok(r,vt,-1)&&(!reg_pair(r,&rp)||(!regsa[rp.r1]&&!regsa[rp.r2]&&!BTST(g->regused,rp.r1)&&!BTST(g->regused,rp.r2)))){
	      int s=cost_savings(p,r,&p->z);
	      if(s==INT_MIN)
		savings[i][r]=INT_MIN;
	      else
		savings[i][r]+=iterations*s;
	    }
	  }
	}
      }
      /* bei Funktionsaufruf zerstoert... */
      if(p->code==CALL){
	if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)&&!(disable&2048)){
	  for(i=0;i<vcount-rcount+const_vars;i++){
	    if(i>=vcount-rcount||BTST(isused,i)){
	      for(r=1;r<=MAXR;r++){
		if(savings[i][r]!=INT_MIN&&BTST(p->q1.v->fi->regs_modified,r))
		  savings[i][r]-=iterations*(cost_load_reg(r,vilist[i])+cost_load_reg(r,vilist[i]));
	      }
	    }	  
	  }
	}else{
	  for(i=0;i<vcount-rcount+const_vars;i++){
	    if(i>=vcount-rcount||BTST(isused,i)){
	      for(r=1;r<=MAXR;r++){
		if(regscratch[r]&&savings[i][r]!=INT_MIN)
		  savings[i][r]-=iterations*(cost_load_reg(r,vilist[i])+cost_load_reg(r,vilist[i]));
	      }
	    }
	  }
	}
      }
      if(p==g->start) break;
      if(p->change_cnt!=0||p->use_cnt!=0)
	av_update(p,isused);
      p=p->prev;
    }
    if(g==end) break;
    if(!end){
      if(g==lend) {lend=0;iterations=1;}
    }
  }
  /*  Maximum ermitteln   */
  for(i=0;i<vcount-rcount+const_vars;i++){
    int m=0,t;struct Var *v;
    v=vilist[i];t=v->vtyp->flags;
    if(ISARRAY(t)&&is_vlength(v->vtyp))
      t=POINTER_TYPE(v->vtyp->next);
    if(!(optflags&1024)&&((v->storage_class!=AUTO&&v->storage_class!=REGISTER)||(v->flags&USEDASADR))){
      for(r=0;r<=MAXR;r++)
	savings[i][r]=INT_MIN;
    }else{
      for(r=1;r<=MAXR;r++){
	/*  Falls Variable in best. Register muss.  */
	if(r==abs(v->reg)&&!(v->flags&REGPARM)) savings[i][r]=INT_MAX;
	if(regsa[r]||!regok(r,t,-1)) savings[i][r]=INT_MIN;
	if(savings[i][r]>m) m=savings[i][r];
      }
      savings[i][0]=m;
    }
  }
  if(DEBUG&8192){
    for(i=0;i<vcount-rcount+const_vars;i++){
      printf("(%s),%ld(best=%d):\n",vilist[i]->identifier,zm2l(vilist[i]->offset),savings[i][0]);
      for(r=1;r<=MAXR;r++)
	printf("%s=%d ",regnames[r],savings[i][r]);
      printf("\n");
    }
  }
  /*  Suchen, welche Variablen/Registerkombination das beste Ergebnis */
  /*  liefert. Nur angenaehert, da sonst wohl zu aufwendig. Simplex?  */
  for(i=0;i<vcount-rcount+const_vars;i++) rvlist[i]=i;
  vqsort(rvlist,vcount-rcount+const_vars,sizeof(*rvlist),cmp_savings);
  for(i=0;i<vcount-rcount+const_vars;i++){
    int use,m=0,prio=0,vi;
    vi=rvlist[i];
    /*if(vilist[vi]->flags&USEDASADR) continue;*/
    if(DEBUG&8192) printf("%d: (%s),%ld(best=%d)\n",i,vilist[vi]->identifier,zm2l(vilist[vi]->offset),savings[vi][0]);
    for(r=1;r<=MAXR;r++){
      if(!lregs[r]&&!regu[r]&&savings[vi][r]>=m&&(!reg_pair(r,&rp)||(!regu[rp.r1]&&!regu[rp.r2]))){
	if(savings[vi][r]>m||reg_prio[r]>prio){
	  m=savings[vi][r];prio=reg_prio[r];
	  use=r;
	}
      }
    }
    if(m>0&&check_cacheable(vi,start,end)){
      if(DEBUG&9216) printf("assigned (%s),%ld(%p) to %s, saving=%d\n",vilist[vi]->identifier,zm2l(vilist[vi]->offset),(void *)vilist[vi],regnames[use],m);
      lregs[use]=vilist[vi];
      regu[use]=1;
      if(reg_pair(use,&rp))
	regu[rp.r1]=regu[rp.r2]=1;
    }
  }
  /*  Registervariablen in alle Bloecke der Schleife eintragen    */
  /*  dabei beruecksichtigen, dass sie in manchen Bloecken nicht  */
  /*  in Register kommen koennen, wenn das Register da schon von  */
  /*  local_regs benutzt wird                                     */
  /*  Gegebenenfalls auch in Header/Footer einer Schleife         */
  /*  eintragen.                                                  */
  if(DEBUG&9216) printf("propagate register vars\n");
  for(g=start;g;g=g->normalout){
    for(r=1;r<=MAXR;r++){
      if(lregs[r]&&!BTST(g->regused,r)&&(!reg_pair(r,&rp)||(!BTST(g->regused,rp.r1)&&!BTST(g->regused,rp.r2)))){
	/*  Falls Variable schon in anderem Register, loeschen  */
	for(i=1;i<=MAXR;i++){
	  if(g->regv[i]==lregs[r]) g->regv[i]=0;
	  if(reg_pair(i,&rp)){
	    if(rp.r1==r||rp.r2==r) g->regv[i]=0;
	  }
	}
	g->regv[r]=lregs[r];
	if(reg_pair(r,&rp)) g->regv[rp.r1]=g->regv[rp.r2]=0;
      }
    }
    if(end&&g==end->normalout) break;
  }
}
void block_regs(struct flowgraph *fg)
/*  macht die Variablenzuweisung fuer einzelne Bloecke  */
{
    struct flowgraph *g,**fgp;
    int i,r,changed,fgz;
    if(DEBUG&9216) printf("block_regs\n");

    savings=mymalloc((vcount-rcount)*sizeof(*savings));
    rvlist=mymalloc((vcount-rcount)*sizeof(*rvlist));

    /*  Array auf Bloecke im Flussgraphen mangels doppelter Verkettung  */
    fgp=mymalloc(basic_blocks*sizeof(*fgp));
    g=fg;fgz=0;
    while(g){
        fgp[fgz]=g;fgz++;
        g=g->normalout;
    }
    if(fgz>basic_blocks) ierror(0); else basic_blocks=fgz;
    /*  alle auf 0  */
    do{
        changed=0;
        if(DEBUG&9216) printf("block_regs pass\n");
        for(fgz=basic_blocks-1;fgz>=0;fgz--){
            struct IC *p;struct Var *v;struct flowlist *lp;
            int t,vt;
            g=fgp[fgz];
            if(DEBUG&8192) printf("assigning regs to block %d\n",g->index);
            /*  berechnen, wieviel ungefaehr eingespart wird, wenn eine Variable    */
            /*  fuer diesen Block in einem best. Register gehalten wird             */
            if(DEBUG&8192) printf("calculating approximate savings\n");

            for(i=0;i<vcount-rcount;i++){
                for(r=1;r<=MAXR;r++){
                    if(!g->regv[r]||g->regv[r]->index!=i){
                        int w=0;
                        /*  Variable muss evtl. geladen/gespeichert werden  */
                        if(BTST(g->av_in,i)) w--;
                        if(BTST(g->av_out,i)) w--;
                        savings[i][r]=w;
                    }
                }
            }
            if(g->calls>0){
            /*  bei Funktionsaufrufen muessen Scratchregister gespeichert werden */
                for(r=1;r<=MAXR;r++)
                    if(regscratch[r])
                        for(i=0;i<vcount-rcount;i++) savings[i][r]-=g->calls*2;
            }
            /*  Wenn Vorgaenger/Nachfolger selbe Variable im selben */
            /*  Register hat, entfaellt Laden/Speichern in diesem   */
            /*  Block und vermutlich auch im Vorgaenger/Nachfolger  */
            /*  nicht immer, aber naeherungsweise...                */
            lp=g->in;
            while(lp){
                if(lp->graph){
                    for(r=1;r<=MAXR;r++){
                        if(lp->graph->regv[r]&&BTST(g->av_in,lp->graph->regv[r]->index)) savings[lp->graph->regv[r]->index][r]+=2;
                    }
                }
                lp=lp->next;
            }
            if(g->branchout){
                for(r=1;r<=MAXR;r++){
                    if(g->branchout->regv[r]&&BTST(g->av_out,g->branchout->regv[r]->index)) savings[g->branchout->regv[r]->index][r]+=2;
                }
            }
            if(g->normalout&&(!g->normalout->end||g->normalout->end->code!=BRA)){
                for(r=1;r<=MAXR;r++){
                    if(g->normalout->regv[r]&&BTST(g->av_out,g->normalout->regv[r]->index)) savings[g->normalout->regv[r]->index][r]+=2;
                }
            }

            p=g->start;
            while(p){
                if((p->q1.flags&(VAR|VARADR|REG))==VAR){
                    v=p->q1.v;
                    if((v->storage_class==AUTO||v->storage_class==REGISTER)&&!(v->flags&USEDASADR)){
                        vt=v->vtyp->flags&NU;
                        i=v->index;
                        if(p->q1.flags&DREFOBJ) t=p->typf&NU; else t=0;
                        for(r=1;r<=MAXR;r++){
                            if(!regsa[r]&&!BTST(g->regused,r)){
                                /*  extra saving, falls passendes Reg fuer DREF */
                                if(t&&regok(r,vt,t)) savings[i][r]++;
                                if(regok(r,vt,0)) savings[i][r]++;
                            }
                        }
                    }
                }
                if((p->q2.flags&(VAR|VARADR|REG))==VAR){
                    v=p->q2.v;
                    if((v->storage_class==AUTO||v->storage_class==REGISTER)&&!(v->flags&USEDASADR)){
                        vt=v->vtyp->flags&NU;
                        i=v->index;
                        if(p->q2.flags&DREFOBJ) t=p->typf&NU; else t=0;
                        for(r=1;r<=MAXR;r++){
                            if(!regsa[r]&&!BTST(g->regused,r)){
                                /*  extra saving, falls passendes Reg fuer DREF */
                                if(t&&regok(r,vt,t)) savings[i][r]++;
                                if(regok(r,vt,0)) savings[i][r]++;
                            }
                        }
                    }
                }
                if((p->z.flags&(VAR|VARADR|REG))==VAR){
                    v=p->z.v;
                    if((v->storage_class==AUTO||v->storage_class==REGISTER)&&!(v->flags&USEDASADR)){
                        vt=v->vtyp->flags&NU;
                        i=v->index;
                        if(p->z.flags&DREFOBJ) t=p->typf&NU; else t=0;
                        for(r=1;r<=MAXR;r++){
                            if(!regsa[r]&&!BTST(g->regused,r)){
                                /*  extra saving, falls passendes Reg fuer DREF */
                                if(t&&regok(r,vt,t)) savings[i][r]++;
                                if(regok(r,vt,0)) savings[i][r]++;
                            }
                        }
                    }
                }
                if(p==g->end) break;
                p=p->next;
            }
            /*  moegliche Kandidaten suchen; muss nicht immer die beste */
            /*  Kombination finden, sollte aber bei lokaler Vergabe     */
            /*  selten einen Unterschied machen                         */
            for(r=1;r<=MAXR;r++){
                if(g->regv[r]||BTST(g->regused,r)) continue;
                for(i=0;i<vcount-rcount;i++){
                    if(savings[i][r]>0){
                        int flag;struct Var *v=vilist[i];
                        /*  Variable schon in anderem Register? */
                        for(flag=1;flag<=MAXR;flag++)
                            if(g->regv[flag]==v){flag=-1;break;}
                        if(flag>0){
                            if(DEBUG&9216) printf("assigned (%s),%ld(%p) to %s; saving=%d\n",vilist[i]->identifier,zm2l(vilist[i]->offset),(void *)vilist[i],regnames[r],savings[i][r]);
			    if(vilist[i]&&!vilist[i]->vtyp->flags) ierror(0);
                            g->regv[r]=vilist[i];
                            changed=1;
                            break;
                        }
                    }
                }
            }
        }
    }while(changed);
    /*  jetzt nochmal globale Register vergeben */
/*    do_loop_regs(fgp[0],fgp[basic_blocks-1]);*/

    free(fgp);
    free(rvlist);
    free(savings);
}

void loop_regs(struct flowgraph *fg,int intask)
/*  weist Variablen in Schleifen Register zu    */
{
    struct flowgraph *g;
    if(disable&4096) return;
    if(DEBUG&9216) printf("assigning regs to function\n");
    savings=mymalloc((vcount-rcount+const_vars)*sizeof(*savings));
    rvlist=mymalloc((vcount-rcount+const_vars)*sizeof(*rvlist));
    do_loop_regs(fg,0,intask);
    if(DEBUG&9216) printf("assigning regs in loops\n");
    for(g=fg;g;g=g->normalout){
        if(g->index==-1&&!(disable&32768)) do_loop_regs(g,g,0);
    }
    free(rvlist);
    free(savings);
}
void insert_allocreg(struct flowgraph *fg,struct IC *p,int code,int reg)
/*  Fuegt ein ALLOCREG/FREEREG (in code) hinter p ein - bei p==0 in */
/*  first_ic.                                                       */
{
    struct IC *new=new_IC();
    /*    printf("%s %s",code==FREEREG?"freereg":"allocreg",regnames[reg]);pric2(stdout,p);*/
    new->line=0;
    new->file=0;
    BSET(fg->regused,reg);
    if(reg_pair(reg,&rp)){
	BSET(fg->regused,rp.r1);
	BSET(fg->regused,rp.r2);
	regused[rp.r1]=regused[rp.r2]=1;
    }
    regused[reg]=1;
    new->code=code;
    new->typf=0;
    new->q1.am=new->q2.am=new->z.am=0;
    new->q1.flags=REG;
    new->q1.reg=reg;
    new->q2.flags=new->z.flags=0;
    new->use_cnt=new->change_cnt=0;
    new->use_list=new->change_list=0;
    insert_IC_fg(fg,p,new);
}

struct Var *lregv[MAXR+1],*first_const,*last_const;
struct flowgraph *lfg;

/* re-inserts a cobj into an obj */
static void insert_cobj(struct obj *o,struct obj *co)
{
  int dt=o->dtyp,df=o->flags&DREFOBJ;
  *o=*co;
  o->flags|=df;
  o->dtyp=dt;
}

static struct Var *find_const_var(struct obj *o,int t)
{
  struct Var *v;
  for(v=first_const;v;v=v->next){
    if((v->ctyp&NQ)==(t&NQ)&&!compare_objs(o,&v->cobj,t))
      return v;
  }
  return 0;
}

static long cidx;

static struct Var *add_const_var(struct obj *o,int t)
{
  struct Var *v;
  static struct Typ vt={VOID},tt;
  int m=o->flags;
  if(o->flags&DREFOBJ){
    t=o->dtyp;
    o->flags&=~DREFOBJ;
  }
  if(v=find_const_var(o,t)){
    o->flags=m;
    return v;
  }
  v=mymalloc(sizeof(*v));
  v->storage_class=AUTO;
  v->cobj=*o;
  v->ctyp=t;
  tt.flags=t;
  if(ISPOINTER(t)) tt.next=&vt;
  v->vtyp=clone_typ(&tt);
  v->next=0;
  v->identifier=" constant";
  v->flags=0;
  v->reg=0;
  cidx++;
  v->offset=l2zm(cidx);
  if(last_const){
      last_const->next=v;
  }else{
      first_const=v;
      const_vars=0;
  }
  last_const=v;
  v->index=vcount-rcount+const_vars;
  const_vars++;
  /* Wir ueberschreiben die Eintraege von vcount-rcount bis vcount. */
  /* Da sie nicht mehr benutzt werden, sollte das ok sein.          */
  vilist=myrealloc(vilist,sizeof(*vilist)*(vcount-rcount+const_vars));
  vilist[vcount-rcount+const_vars-1]=v;
  o->flags=m;
  return v;
}

void create_const_vars(struct flowgraph *fg)
{
  struct IC *p;int t;
  struct Var *v;
  if(DEBUG&1024) printf("creating const-vars\n");
  cidx=0;
  const_vars=0;
  first_const=last_const=0;
  for(;fg;fg=fg->normalout){
    for(p=fg->start;p;p=p->next){
      if(p->q1.flags&(KONST|VARADR)){
	t=q1typ(p);
	v=add_const_var(&p->q1,t);
	p->q1.flags&=~(KONST|VARADR);
	p->q1.flags|=(VAR|VKONST);
	p->q1.v=v;
	p->q1.val.vmax=l2zm(0L);
      }
      if(p->q2.flags&(KONST|VARADR)){
	t=q2typ(p);
	v=add_const_var(&p->q2,t);
	p->q2.flags&=~(KONST|VARADR);
	p->q2.flags|=(VAR|VKONST);
	p->q2.v=v;
	p->q2.val.vmax=l2zm(0L);
      }
      if(p->z.flags&(KONST|VARADR)){
	t=ztyp(p);
	v=add_const_var(&p->z,t);
	p->z.flags&=~(KONST|VARADR);
	p->z.flags|=(VAR|VKONST);
	p->z.v=v;
	p->z.val.vmax=l2zm(0L);
      }
      if(p==fg->end) break;
    }
  }
  if(DEBUG&1024){
    struct Var *v;
    printf("const-vars:\n");
    for(v=first_const;v;v=v->next){
      printf("%d: ",v->index);
      probj(stdout,&v->cobj,v->ctyp);
      printf(" (t=%d)\n",v->ctyp);
    }
  }
}

void free_const_vars(void)
{
  struct Var *v,*m;
  if(DEBUG&1024) printf("free_const_vars()\n");
  for(v=first_const;v;){
    m=v->next;
    free(v);
    v=m;
  }
}

#if 0
int free_hreg(struct flowgraph *fg,struct IC *p,int reg,int mustr)
/*  Macht das Register reg frei, damit es als lokale Variable im IC p   */
/*  zur Verfuegung steht. Wenn mustr!=0, muss das Register unbedingt    */
/*  freigemacht werden, ansonsten kann davon abgesehen werden.          */
{
    struct IC *m,*first;struct Var *v;
    int preg[MAXR+1]={0},calls[MAXR+1]={0},rreg,i;
    first=0;
    v=lregv[reg];
    if(!v)
      ierror(0);
    if(DEBUG&8192) printf("free_hreg %s,%s,%d\n",regnames[reg],v->identifier,mustr);
    if(v->reg&&!*v->identifier) ierror(0);
    for(m=p;m;m=m->next){
      if(m->code==CALL){
	if((m->q1.flags&(VAR|DREFOBJ))==VAR&&m->q1.v->fi&&(m->q1.v->fi->flags&ALL_REGS)){
	  for(i=1;i<=MAXR;i++)
	    if(BTST(m->q1.v->fi->regs_modified,i)) calls[i]++;
	}else{
	  for(i=1;i<=MAXR;i++)
	    if(regscratch[i]) calls[i]++;
	}
      }
      if(m->code==ALLOCREG){
	preg[m->q1.reg]=1;
	if(m->q1.reg==reg) ierror(0);
      }
      if(m->code==FREEREG){
	preg[m->q1.reg]=1;
	if(m->q1.reg==reg) break;
      }
      if(!USEQ2ASZ){
	if((m->q2.flags&VAR)&&m->q2.v==v&&(m->z.flags&(REG|DREFOBJ))==REG&&
	   (!(m->z.flags&VAR)||m->z.v!=v))
	  preg[m->z.reg]=1;
      }
      if(((m->q1.flags&VAR)&&m->q1.v==v)||
	 ((m->q2.flags&VAR)&&m->q2.v==v)||
	 ((m->z.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)&&m->z.v==v))
	first=m;
      /*        if((m->z.flags&(REG|DREFOBJ))==REG&&m->z.reg==reg) break;*/
    }
    if(!first) {pric(stdout,p);ierror(0);}
    rp.r1=rp.r2=0;
    for(rreg=0,i=1;i<=MAXR;i++){
        if(preg[i]||regu[i]||regsa[i]||!regok(i,v->vtyp->flags,0)) continue;
	if(reg_pair(i,&rp)&&(preg[rp.r1]||preg[rp.r2]||regu[rp.r1]||regu[rp.r2]||regsa[rp.r1]||regsa[rp.r2])) continue;
        if(calls[i]==0&&regscratch[i]){rreg=i;break;}
	if(calls[i]==0) rreg=i;
    }
    if(!rreg&&!mustr) return 0;
    for(m=p;m!=first->next;m=m->next){
        if((m->q1.flags&VAR)&&m->q1.v==v)
            {if(!rreg) m->q1.flags&=~REG; else m->q1.reg=rreg;}
        if((m->q2.flags&VAR)&&m->q2.v==v)
            {if(!rreg) m->q2.flags&=~REG; else m->q2.reg=rreg;}
	if((m->z.flags&VAR)&&m->z.v==v)
	  {if(!rreg) m->z.flags&=~REG; else m->z.reg=rreg;} 
#if 0
	if((m->z.flags&(VAR|DREFOBJ))==VAR&&m->z.v==v)
	  break;
#endif
    }
    if(rreg){
	lregv[rreg]=lregv[reg];
	regused[rreg]=1;regu[rreg]=1;BSET(fg->regused,rreg);
	if(reg_pair(rreg,&rp)){
	    regused[rp.r1]=1;regu[rp.r1]=1;BSET(fg->regused,rp.r1);
	    regused[rp.r2]=1;regu[rp.r2]=1;BSET(fg->regused,rp.r2);
	}
    }
    lregv[reg]=0;regu[reg]=0;
    if(reg_pair(reg,&rp))
	regu[rp.r1]=regu[rp.r2]=0;

    for(m=first->next;m&&m->code==FREEREG;m=m->next){
        if(m->q1.reg==reg){
            if(!rreg) remove_IC_fg(fg,m); else m->q1.reg=rreg;
/*            if(rreg) insert_allocreg(fg,first,FREEREG,rreg);*/
            return rreg;
        }
    }
    insert_allocreg(fg,first->prev,ALLOCREG,reg);
    if(rreg)
      insert_allocreg(fg,first,FREEREG,rreg);
    return rreg;
}
#endif
int replace_local_reg(struct obj *o)
/*  tested, ob o eine Scratch-Variable ist und ersetzt sie gegebenenfalls   */
{
    int i;struct Var *v;
    if((o->flags&(VAR|REG|VARADR))==VAR){
        v=o->v;i=v->index;
        if(BTST(lfg->av_kill,i)&&!BTST(lfg->av_out,i)){
            for(i=1;i<=MAXR;i++){
                if(lregv[i]==v){
                    o->flags|=(REG|SCRATCH);
/*                    o->flags&=~VAR;*/
                    o->reg=i;
                    return i;
                }
            }
        }
    }
    return 0;
}
void local_combine(struct flowgraph *fg)
/*  Versucht, Zuweisungen der Form (x)->tmp in ein direkt folgendes IC */
/*  einzugliedern. Nur einfache Tests, da nur loads eliminiert werden  */
/*  sollen, die zum leichten Erkennen per cse explizit wurden.         */
{
  struct IC *p,*pprev;int i,cl;
  bvtype *used=mymalloc(vsize);
  if(DEBUG&1024) printf("local combining\n");
  for(;fg;fg=fg->normalout){
    memcpy(used,fg->av_out,vsize);
    for(p=fg->end;p;){
      if(p->code==NOP||(!p->q1.flags&&!p->q2.flags)){
	if(p==fg->start) break;
	p=p->prev;
	continue;
      }
      pprev=p->prev;
      while(pprev&&pprev->code==NOP) pprev=pprev->prev;
      if(pprev&&p->code==ASSIGN&&zmeqto(p->q2.val.vmax,sizetab[p->typf&NQ])&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&pprev->z.flags==p->q1.flags&&p->q1.v==pprev->z.v&&ztyp(pprev)==q1typ(p)&&!BTST(used,p->q1.v->index)&&(pprev->code!=ASSIGN||zmeqto(pprev->q2.val.vmax,sizetab[pprev->typf&NQ]))){
	/* x op y ->tmp; move tmp->*p => x op y ->*p */
	if(DEBUG&1024){
	  printf("local combine(3):\n");
	  pric2(stdout,pprev);pric2(stdout,p);
	}
	pprev->z=p->z;
	pprev->change_cnt=p->change_cnt;
	pprev->change_list=p->change_list;
	p->change_cnt=0;
	p->change_list=0;
	pprev->use_list=myrealloc(pprev->use_list,(pprev->use_cnt+p->use_cnt)*VLS);
	memcpy(&pprev->use_list[pprev->use_cnt],p->use_list,p->use_cnt*VLS);
	pprev->use_cnt+=p->use_cnt;
	p->q1.flags=p->z.flags=0;
	p->code=NOP;
	/* restart */
	memcpy(used,fg->av_out,vsize);
	p=fg->end;
	continue;
      }
      if((pprev)&&pprev->code==ASSIGN&&zmeqto(pprev->q2.val.vmax,sizetab[pprev->typf&NQ])
	 &&zmeqto(pprev->q2.val.vmax,sizetab[p->typf&NQ])
	 &&((pprev->q1.flags&DREFOBJ)||(static_cse&&(pprev->q1.flags&(VAR|VARADR))==VAR&&(pprev->q1.v->storage_class==EXTERN||pprev->q1.v->storage_class==STATIC)))&&(pprev->z.flags&(VAR|DREFOBJ))==VAR
	 &&!BTST(used,(i=pprev->z.v->index))&&pprev->z.v->storage_class==AUTO
	 &&!(pprev->z.v->flags&USEDASADR)
	 &&((p->code!=ASSIGN&&p->code!=PUSH)||zmeqto(p->q2.val.vmax,sizetab[p->typf&NQ]))){
	cl=0;
	if((p->z.flags&VAR)&&p->z.v->index==i) cl=4;
	if((p->q1.flags&(VAR|DREFOBJ))==VAR){
	  if(p->q1.v->index==i&&zmeqto(p->q1.val.vmax,pprev->z.val.vmax))
	    cl|=1;
	}
	if((p->q2.flags&(VAR|DREFOBJ))==VAR){
	  if(p->q2.v->index==i&&zmeqto(p->q2.val.vmax,pprev->z.val.vmax))
	    cl|=2;
	}
	if(cl==1||cl==2){
	  if(cl!=2||USEQ2ASZ||compare_objs(&pprev->q1,&p->z,p->typf)){
	    if(DEBUG&1024){printf("local combine:\n");pric2(stdout,pprev);pric2(stdout,p);}
	    if(cl==1)
	      p->q1=pprev->q1;
	    else
	      p->q2=pprev->q1;
	    p->use_list=myrealloc(p->use_list,(p->use_cnt+pprev->use_cnt)*VLS);
	    memcpy(&p->use_list[p->use_cnt],pprev->use_list,pprev->use_cnt*VLS);
	    p->use_cnt+=pprev->use_cnt;
	    pprev->code=NOP;
	    pprev->q1.flags=pprev->z.flags=0;
	    pprev->typf=0;
	    /* restart */
	    memcpy(used,fg->av_out,vsize);
	    p=fg->end;
	    continue;
	  }
	}
      }
      if(p==fg->start) break;
      if(p->q1.flags&VAR) BSET(used,p->q1.v->index);
      if(p->q2.flags&VAR) BSET(used,p->q2.v->index);
      if(p->z.flags&VAR) BSET(used,p->z.v->index);
      p=p->prev;
    }
  }
  free(used);
}

/* Fuegt evtl. noetige allocreg/freereg nach local_regs in Block ein. */
/* Kann durch free_hreg noetig werden. */
void fix_local_allocreg(struct flowgraph *fg)
{
  char regs[MAXR+1]={0};
  struct IC *p=fg->end;
  while(p){
    if(p->code==ALLOCREG){
      if(!regs[p->q1.reg]) ierror(0);
      regs[p->q1.reg]=0;
    }else if(p->code==FREEREG){
      if(regs[p->q1.reg]){
	ierror(0);
      }
      regs[p->q1.reg]=1;
    }else{
      if((p->q1.flags&REG)&&!regs[p->q1.reg]){
	insert_allocreg(fg,p,FREEREG,p->q1.reg);
	regs[p->q1.reg]=1;
      }
      if((p->q2.flags&REG)&&!regs[p->q2.reg]){
	insert_allocreg(fg,p,FREEREG,p->q2.reg);
	regs[p->q2.reg]=1;
      }
      if((p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&!regs[p->z.reg]){
	insert_allocreg(fg,p,FREEREG,p->z.reg);
	regs[p->z.reg]=1;
      }
      if((p->z.flags&(REG|DREFOBJ))==REG){
	if(!(p->q1.flags&REG)||p->q1.reg!=p->z.reg){
	  if(!(p->q2.flags&REG)||p->q2.reg!=p->z.reg){
	    struct IC *p2;
	    for(p2=p->prev;p2;p2=p2->prev){
	      if(p2->code==ALLOCREG&&p2->q1.reg==p->z.reg) break;
	      if(p2->code!=ALLOCREG&&p2->code!=FREEREG) break;
	    }
	    if(p2->code!=ALLOCREG||p2->q1.reg!=p->z.reg){
	      insert_allocreg(fg,p->prev,ALLOCREG,p->z.reg);
	    }
	  }
	}
      }
    }
    if(p==fg->start) return;
    p=p->prev;
  }
}

/* searches back to determine the best local register for v 
   register-pairs must not be alive in the same instruction as
   one of the halves */
int find_best_local_reg(struct IC *fp,struct Var *v,int preferred)
{
  int r,used,tmp,savings[MAXR+1]={0};
  struct IC *p=fp;
  if(v->reg&&!*v->identifier)
    return abs(v->reg);
  if(v->vtyp->flags&VOLATILE)
    return 0;
  if(!USEQ2ASZ&&(p->q2.flags&VAR)&&p->q2.v==v)
    savings[preferred]=INT_MIN;
  else
    savings[preferred]=1;
  if(reg_pair(preferred,&rp)){
    savings[rp.r1]=INT_MIN;
    savings[rp.r2]=INT_MIN;
  }
  for(r=1;r<=MAXR;r++){
    if(regu[r]||regsa[r]){
      savings[r]=INT_MIN;
      if(reg_pair(r,&rp)){
	savings[rp.r1]=INT_MIN;
	savings[rp.r2]=INT_MIN;
      }
    }else if(reg_pair(r,&rp)){
      if(rp.r1==preferred||rp.r2==preferred||regu[rp.r1]||regsa[rp.r1]||regu[rp.r2]||regsa[rp.r2])
	savings[r]=INT_MIN;
    }
    if(savings[r]!=INT_MIN&&!regok(r,v->vtyp->flags,-1))
      savings[r]=INT_MIN;
  }
  for(r=1;r<=MAXR;r++){
    if(savings[r]!=INT_MIN&&reg_pair(r,&rp)){
      if(regu[rp.r1]||regu[rp.r2]||regsa[rp.r1]||regsa[rp.r2])
	savings[r]=INT_MIN;
    }
  }
  while(1){
    if(!p||(p!=fp&&p->code>=LABEL&&p->code<=BRA)) return 0;
    if(p!=fp){
      if((p->z.flags&VAR)&&p->z.v->reg&&!*p->z.v->identifier){
	if(p->z.v==v)
	  return abs(p->z.v->reg);
	else
	  savings[p->z.v->reg]=INT_MIN;
      }
    }
    used=0;
    if((p->q1.flags&VAR)&&p->q1.v==v){
      if(p->code==ADDRESS) return 0;
      if((p->code==ASSIGN||p->code==PUSH)&&(q1typ(p)&NQ)==(CHAR)&&!zmeqto(p->q2.val.vmax,l2zm(1L)))
	return 0;
      used=1;
      for(r=1;r<=MAXR;r++){
	if(savings[r]!=INT_MIN){
	  tmp=cost_savings(p,r,&p->q1);
	  if(tmp==INT_MIN)
	    savings[r]=INT_MIN;
	  else
	    savings[r]+=tmp;
	}
      }
    }
    if((p->q2.flags&VAR)&&p->q2.v==v){
      used=1;
      for(r=1;r<=MAXR;r++){
	if(savings[r]!=INT_MIN){
	  tmp=cost_savings(p,r,&p->q2);
	  if(tmp==INT_MIN)
	    savings[r]=INT_MIN;
	  else
	    savings[r]+=tmp;
	}
      }
    }
    if((p->z.flags&VAR)&&p->z.v==v){
      if(p->code==ASSIGN&&(ztyp(p)&NQ)==(CHAR)&&!zmeqto(p->q2.val.vmax,l2zm(1L)))
	return 0;
      for(r=1;r<=MAXR;r++){
	if(savings[r]!=INT_MIN){
	  tmp=cost_savings(p,r,&p->z);
	  if(tmp==INT_MIN)
	    savings[r]=INT_MIN;
	  else
	    savings[r]+=tmp;
	}
      }
      if(used==0&&!(p->z.flags&DREFOBJ))
	break;
    }
    if(p->code==CALL){
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)&&!(disable&2048)){
	for(r=1;r<=MAXR;r++){
	  if(BTST(p->q1.v->fi->regs_modified,r))
	    savings[r]=INT_MIN;
	}
      }else{
	for(r=1;r<=MAXR;r++){
	  if(regscratch[r])
	    savings[r]=INT_MIN;
	}
      }
    }

    /* check for register arguments */
    if((p->z.flags&(VAR|DREFOBJ))==VAR&&!*p->z.v->identifier&&p->z.v->reg){
      savings[abs(p->z.v->reg)]=INT_MIN;
      if(reg_pair(abs(p->z.v->reg),&rp)){
	savings[rp.r1]=INT_MIN;
	savings[rp.r2]=INT_MIN;
      }
    }

    p=p->prev;
  }
  r=0;
  savings[0]=0;
  for(tmp=1;tmp<=MAXR;tmp++){
    if(savings[tmp]>savings[r]||(savings[tmp]==savings[r]&&reg_prio[tmp]>reg_prio[r]))
      r=tmp;
  }
  return r;
}
void local_regs(struct flowgraph *fg)
/*  versucht Variablen, die nur innerhalb eines Basic Blocks benutzt    */
/*  werden (kill==true und out==false), Register zuzuweisen.            */
{
  struct IC *p;
  int i,j,t,r,nr,mustalloc,savings,prio,nr1,nr2;
  bvtype *inmem=mymalloc(vsize);
  if(DEBUG&9216) printf("assigning temporary variables to registers\n");
  memset(inmem,0,vsize);
  lfg=fg;
  while(lfg){
    if(DEBUG&1024) printf("block %d\n",lfg->index);
    for(i=1;i<=MAXR;i++){lregv[i]=0; regu[i]=regsa[i]; lfg->regv[i]=0;}
    memset(&lfg->regused,0,RSIZE);
    lfg->calls=0;
    p=lfg->end;
    while(p){
      nr=nr1=nr2=0;
      i=replace_local_reg(&p->z);
      if((p->z.flags&(VAR|DREFOBJ))==VAR){
	if(i){
	  lregv[i]=0;regu[i]=0;
	  if(reg_pair(i,&rp)){
	    nr1=rp.r1;
	    nr2=rp.r2;
	    regu[nr1]=regu[nr2]=0;
	  }
	  nr=i;mustalloc=1;
	  if(DEBUG&8192) printf("regu[%s] decremented to %d\n",regnames[i],regu[i]);
	}else{
	  BCLR(inmem,p->z.v->index);
	}
      }
      if(p->code!=ADDRESS){
	if(replace_local_reg(&p->q1)==nr) mustalloc=0;
	if(replace_local_reg(&p->q2)==nr) mustalloc=0;
      }
      /*  hier wegen USEQ2ASZ aufpassen; kommutative ICs sollten so   */
      /*  angeordnet werden, dass ein evtl. Register rechts steht     */
      if((p->q2.flags&(VAR|REG|VARADR))==VAR&&!(p->q2.v->flags&USEDASADR)&&!(p->q2.v->vtyp->flags&VOLATILE)&&(p->q2.v->storage_class==AUTO||p->q2.v->storage_class==REGISTER)){
	j=p->q2.v->index;
	if(BTST(lfg->av_kill,j)&&!BTST(lfg->av_out,j)&&!BTST(inmem,j)){
	  r=find_best_local_reg(p,p->q2.v,nr);
	  if(r){
	    if(r!=nr) insert_allocreg(lfg,p,FREEREG,r);
	    else mustalloc=0;
	    lregv[r]=p->q2.v;regused[r]=regu[r]=1;
	    if(reg_pair(r,&rp)){
	      regu[rp.r1]=regu[rp.r2]=1;
	      regused[rp.r1]=regused[rp.r2]=1;
	    }
	    if(replace_local_reg(&p->q2)!=r) ierror(0);
	    replace_local_reg(&p->q1);
	    replace_local_reg(&p->z);
	    if((DEBUG&9216)&&*p->q2.v->identifier) printf("temporary <%s> assigned to %s (v1)\n",p->q2.v->identifier,regnames[r]);
	    if(DEBUG&8192) printf("temporary <%s>(%p) assigned to %s (v1)\n",p->q2.v->identifier,(void *)p->q2.v,regnames[r]);
	  }else BSET(inmem,j);
	}
      }
      if((p->z.flags&(VAR|REG|DREFOBJ))==(VAR|DREFOBJ)&&!(p->z.v->flags&USEDASADR)&&!(p->z.v->vtyp->flags&VOLATILE)&&(p->z.v->storage_class==AUTO||p->z.v->storage_class==REGISTER)){
	j=p->z.v->index;
	if(BTST(lfg->av_kill,j)&&!BTST(lfg->av_out,j)&&!BTST(inmem,j)){
	  r=find_best_local_reg(p,p->z.v,nr);
	  if(r){
	    insert_allocreg(lfg,p,FREEREG,r);
	    lregv[r]=p->z.v;regused[r]=regu[r]=1;
	    if(reg_pair(r,&rp)){
	      regu[rp.r1]=regu[rp.r2]=1;
	      regused[rp.r1]=regused[rp.r2]=1;
	    }
	    if(replace_local_reg(&p->z)!=r){
	      for(i=1;i<=MAXR;i++) if(lregv[i]) printf("%d:%s=%s(%p)\n",i,regnames[i],lregv[i]->identifier,(void*)lregv[i]);
	      ierror(r);}
	    replace_local_reg(&p->q1);
	    if((DEBUG&9216)&&*p->z.v->identifier) printf("temporary <%s> assigned to %s (v2)\n",p->z.v->identifier,regnames[r]);
	    if(DEBUG&8192) printf("temporary <%s>(%p) assigned to %s (v2)\n",p->z.v->identifier,(void *)p->z.v,regnames[r]);
	  }else BSET(inmem,j);
	}
      }
      if((p->q1.flags&(VAR|REG|VARADR))==VAR&&!(p->q1.v->flags&USEDASADR)&&(!(p->q1.v->vtyp->flags&VOLATILE)||p->q1.v->reg)&&(p->q1.v->storage_class==AUTO||p->q1.v->storage_class==REGISTER)){
	j=p->q1.v->index;
	if(BTST(lfg->av_kill,j)&&!BTST(lfg->av_out,j)&&!BTST(inmem,j)){
	  r=find_best_local_reg(p,p->q1.v,nr);
	  if(r){
	    if(r!=nr) 
	      insert_allocreg(lfg,p,FREEREG,r);
	    else
	      mustalloc=0;
	    lregv[r]=p->q1.v;regused[r]=regu[r]=1;
	    if(reg_pair(r,&rp)){
	      regu[rp.r1]=regu[rp.r2]=1;
	      regused[rp.r1]=regused[rp.r2]=1;
	    }
	    if(replace_local_reg(&p->q1)!=r) ierror(0);
	    if((DEBUG&9216)&&*p->q1.v->identifier) printf("temporary <%s> assigned to %s (v3)\n",p->q1.v->identifier,regnames[r]);
	    if(DEBUG&8192) printf("temporary <%s>(%p) assigned to %s (v3)\n",p->q1.v->identifier,(void *)p->q1.v,regnames[r]);
	  }else BSET(inmem,j);
	}
      }
      if(p->code==CALL){
	lfg->calls++;
#if 0
	/*  falls Scratchregister bei Funktionsaufruf benutzt   */
	/*  wird, moeglichst auf ein anderes ausweichen         */
	for(i=1;i<=MAXR;i++){
	  if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)){
	    if(lregv[i]&&regscratch[i]&&BTST(p->q1.v->fi->regs_modified,i)){
	      int j=lregv[i]->index;
	      if(lregv[i]->reg&&!*lregv[i]->identifier) ierror(0);
	      if(free_hreg(lfg,p,i,1/*0*/)==0) BSET(inmem,j);
	    }		    
	  }else{
	    if(lregv[i]&&regscratch[i]){
	      int j=lregv[i]->index;
	      if(lregv[i]->reg&&!*lregv[i]->identifier) ierror(0);
	      if(free_hreg(lfg,p,i,1/*0*/)==0) BSET(inmem,j);
	    }
	  }
	}
#endif
      }
      /*  die Faelle beachten, wenn schon im IC ein Register          */
      /*  angesprochen wird (sollte nur bei CALL und return auftreten */
      if(p->code==FREEREG){
	ierror(0);
      }
      if(p->code==ALLOCREG){
	ierror(0);
      }
      if(p==lfg->start) i=1; else i=0;
      p=p->prev;
      if(nr&&mustalloc) insert_allocreg(lfg,p,ALLOCREG,nr);
      if(i) break;
    }
    fix_local_allocreg(lfg);
    lfg=lfg->normalout;
  }
  free(inmem);
}
void insert_saves(struct flowgraph *fg)
/*  fuegt speichern von Registern bei Funktionsaufrufen ein */
{
  int r,c,i;struct IC *p,*new,*n,*before,*after;struct Var *v;
  bvtype *isused=mymalloc(vsize);
  if(DEBUG&9216) printf("insert_saves\n");
  for(;fg;fg=fg->normalout){
   memcpy(isused,fg->av_out,vsize);
    p=fg->end;
    while(p){
      c=p->code;
      if(c==CALL){
	if(p==fg->start) n=0; else n=p->prev;
	for(r=1;r<=MAXR;r++){
	  i=0;
	  if((v=fg->regv[r])&&(v->index>=vcount-rcount||BTST(isused,v->index))){
	    if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)&&!(disable&2048)){
	      if(BTST(p->q1.v->fi->regs_modified,r)) i=1;
	    }else{
	      if(regscratch[r]) i=1;
	    }
	  }
	  if(i){
	    if(!n) before=fg->start->prev; else before=n;
	    after=p->next;
	    while(after&&(after->code==FREEREG||after->code==ALLOCREG||after->code==NOP))
	      after=after->next;
	    if(!after||after->code!=GETRETURN){
	      after=p;
	    }else{
	      if((after->z.flags&REG)&&after->z.reg==r) after->z.flags&=~REG;
	    }
	    if(v->index>=vcount-rcount){
	      /* it is a const_var; reload instead of store */
	      new=new_IC();
	      new->code=ASSIGN;
	      new->typf=v->vtyp->flags;
	      insert_cobj(&new->q1,&v->cobj);
	      new->q2.flags=0;
	      new->q2.val.vmax=szof(v->vtyp);
	      new->z.flags=REG;
	      new->z.reg=r;
	      new->z.val.vmax=l2zm(0L);
	      new->z.v=v;
	      new->file=p->file;
	      new->line=p->line;
	      /* fake, we do nothing with this afterwards */
	      new->use_cnt=new->change_cnt=0;
	      new->change_list=new->use_list=0;
	      insert_IC_fg(fg,after,new);
	    }else{
	      new=new_IC();
	      new->code=ASSIGN;
	      new->typf=v->vtyp->flags;
              /* vla */
              if(ISARRAY(new->typf))
                new->typf=POINTER_TYPE(v->vtyp->next);
	      new->q1.flags=VAR|REG;
	      new->q1.v=v;
	      new->q1.val.vmax=l2zm(0L);
	      new->q1.reg=r;
	      new->q2.flags=0;
	      new->q2.val.vmax=szof(v->vtyp);
	      new->z.flags=VAR;
	      new->z.val.vmax=l2zm(0L);
	      new->z.v=v;
	      new->file=p->file;
	      new->line=p->line;
	      /* fake, we do nothing with this afterwards */
	      new->use_cnt=new->change_cnt=0;
	      new->change_list=new->use_list=0;
	      insert_IC_fg(fg,before,new);
	      new=new_IC();
	      new->code=ASSIGN;
	      new->typf=v->vtyp->flags;
              /* vla */
              if(ISARRAY(new->typf))
                new->typf=POINTER_TYPE(v->vtyp->next);
	      new->q1.flags=VAR;
	      new->q1.v=v;
	      new->q1.val.vmax=l2zm(0L);
	      new->q2.flags=0;
	      new->q2.val.vmax=szof(v->vtyp);
	      new->z.flags=VAR|REG;
	      new->z.v=v;
	      new->z.val.vmax=l2zm(0L);
	      new->z.reg=r;
	      new->file=p->file;
	      new->line=p->line;
	      /* fake, we do nothing with this afterwards */
	      new->use_cnt=new->change_cnt=0;
	      new->change_list=new->use_list=0;
	      insert_IC_fg(fg,after,new);
	    }
	  }
	}
	p=n;
	continue;
      }
      if(p==fg->start) break;
      if(p->change_cnt!=0||p->use_cnt!=0)
	av_update(p,isused);
      p=p->prev;
    }
  }
  free(isused);
}


#endif

void insert_simple_allocreg(struct IC *p,int code,int reg)
/*  Fuegt ein ALLOCREG/FREEREG (in code) hinter p ein - bei p==0 in */
/*  first_ic.                                                       */
{
    struct IC *new=new_IC();
    new->line=0;
    new->file=0;
    regused[reg]=1;
    if(reg_pair(reg,&rp))
	regused[rp.r1]=regused[rp.r2]=1;
    new->code=code;
    new->typf=0;
    new->q1.am=new->q2.am=new->z.am=0;
    new->q1.flags=REG;
    new->q1.reg=reg;
    new->q2.flags=new->z.flags=0;
    new->use_cnt=new->change_cnt=0;
    new->use_list=new->change_list=0;
    insert_IC(p,new);
}

void load_simple_reg_parms(void)
/*  Laedt Registerparameter, falls noetig. Nicht-optimierende Version.  */
{
  int i,j; struct Var *v;
  struct regp regp[MAXR+1]={0};
/*   for(i=1;i<=MAXR;i++) {regp[i].treg=0;regp[i].tvar=0;} */
  for(i=0;i<=1;i++){
    if(i==0) v=vl3; else v=vl2;
    for(;v;v=v->next){
      if((v->flags&REGPARM)&&(regsv[abs(v->reg)]!=v||(v->flags&CONVPARAMETER))){
	regp[abs(v->reg)].tvar=v;
	for(j=1;j<=MAXR;j++)
	  if(regsv[j]==v) regp[abs(v->reg)].treg=j;
      }
    }
  }
  do_load_parms(regp,0);
}

extern zmax recalc_start_offset;

void do_load_parms(struct regp regp[],struct flowgraph *fg)
{
  int i,i1,i2,j,j1,j2,c,k,notdone;
  struct {int freg,treg;struct Var *tvar,*tmp;} order[MAXR]={0};
  if(DEBUG&1){
    printf("do_load_parms:\n");
    for(i=1;i<=MAXR;i++)
      if(regp[i].tvar)
	printf("%s->%s(%s)\n",regnames[i],regnames[regp[i].treg],regp[i].tvar->identifier);
  }
  do{
    c=0;
    do{
      notdone=0;
      for(i=1;i<=MAXR;i++){
	int notfree;
	if(reg_pair(i,&rp)){
	  i1=rp.r1;
	  i2=rp.r2;
	}else
	  i1=i2=0;
	if(!regp[i].tvar) continue;
	j=regp[i].treg;
	if(j&&reg_pair(j,&rp)){
	  j1=rp.r1;
	  j2=rp.r2;
	}else
	  j1=j2=0;
	notfree=0;
	for(k=1;k<=MAXR;k++){
	  if(regp[k].tvar&&reg_pair(k,&rp)&&(rp.r1==j||rp.r1==j1||rp.r1==j2||rp.r2==j||rp.r2==j1||rp.r2==j2)){
	    notfree=1;
	    break;
	  }
	}
	if(j==0||(!notfree&&!regp[j].tvar&&(!j1||!regp[j1].tvar)&&(!j2||!regp[j2].tvar))||i==j||regp[i].tmp){
	  order[c].freg=i;
	  order[c].treg=j;
	  order[c].tvar=regp[i].tvar;
	  if(regp[i].tmp){order[c].treg=0;order[c].tvar=regp[i].tmp;}
	  c++; notdone=1;
	  regp[i].treg=0;
	  regp[i].tvar=0;
	}
      }
    }while(notdone);
    for(i=c-1;i>=0;i--)
      load_one_parm(order[i].freg,order[i].treg,0,order[i].tvar,fg);
    notdone=0;
    /* first try to spill register-pair (probably suboptimal, but should
       avoid a deadlock) */
    for(i=1;i<=MAXR;i++){
      if(regp[i].tvar&&reg_pair(i,&rp))
	break;
    }
    if(i>MAXR){
      for(i=1;i<=MAXR;i++){
	if(regp[i].tvar)
	  break;
      }
    }
    if(i<=MAXR){
      static struct Typ dtyp={DOUBLE};
      if(!regp[i].tvar) ierror(0);
      if(regp[i].tvar->flags&CONVPARAMETER)
	regp[i].tmp=add_tmp_var(clone_typ(&dtyp));
      else
	regp[i].tmp=add_tmp_var(clone_typ(regp[i].tvar->vtyp));
      /* num_vars is already done, so mark it by -1 */
      regp[i].tmp->index=-1;
      /* allocate memory for recalc_offsets */
      if(fg){
	regp[i].tmp->offset=l2zm(0L);
	if(zmleq(recalc_start_offset,regsize[i]))
	  recalc_start_offset=regsize[i];
      }
      load_one_parm(0,regp[i].treg,regp[i].tmp,regp[i].tvar,fg);
      notdone=1;
    }
  }while(notdone);
}
void load_one_parm(int freg,int treg,struct Var *fvar,struct Var *tvar,struct flowgraph *fg)
{
  struct IC *new;
  if(DEBUG&1) printf("lop: %s(%s)->%s(%s)\n",regnames[freg],fvar?fvar->identifier:empty,regnames[treg],tvar?tvar->identifier:empty);
  if(freg&&freg!=treg){
    if(fg)
      insert_allocreg(fg,0,FREEREG,freg);
    else
      insert_simple_allocreg(0,FREEREG,freg);
  }
  new=new_IC();
  new->line=0;
  new->file=0;
  if(tvar->flags&CONVPARAMETER){
    new->code=CONVERT;
    new->typf=tvar->vtyp->flags;
    new->typf2=DOUBLE;
  }else{
    new->code=ASSIGN;
    new->typf=tvar->vtyp->flags;
  }
  if(ISFLOAT(new->typf)) float_used=1;
  if(fvar){
    new->q1.flags=VAR;
    new->q1.v=fvar;
    new->q1.val.vmax=l2zm(0L);
  }else{
    new->q1.flags=REG;
    new->q1.reg=freg;
  }
  new->q2.flags=0;
  new->q2.val.vmax=szof(tvar->vtyp);
  if(treg)
    new->z.flags=REG|VAR;
  else
    new->z.flags=VAR;
  new->z.val.vmax=l2zm(0L);
  new->z.v=tvar;
  new->z.reg=treg;
  new->q1.am=new->q2.am=new->z.am=0;
  new->use_cnt=new->change_cnt=0;
  new->use_list=new->change_list=0;
  if((new->typf&NQ)==CHAR&&!regok(new->q1.reg,new->typf,0)){
    new->code=CONVERT;
    new->typf2=INT;
  }
  if(fg){
    insert_IC_fg(fg,0,new);
    if(freg&&freg!=treg) insert_allocreg(fg,0,ALLOCREG,freg);
  }else{
    insert_IC(0,new);
    if(freg&&freg!=treg) insert_simple_allocreg(0,ALLOCREG,freg);
  }
  if(new->z.flags&REG){
    /*  ALLOCREG verschieben    */
    struct IC *p;
    if(fg)
      insert_allocreg(fg,0,ALLOCREG,treg);
    else
      insert_simple_allocreg(0,ALLOCREG,treg);
    for(p=new->next;p;p=p->next){
      if(p->code==ALLOCREG&&p->q1.reg==treg){
	if(fg)
	  remove_IC_fg(fg,p);
	else
	  remove_IC(p);
	break;
      }
    }
    if(!p) ierror(0);
  }
}

void simple_regs(void)
/*  haelt Variablen in Registern, simple Version            */
{
  int i2,i,j;int pri;struct Var *v;
  struct IC *icp,*start=first_ic;
  if(!first_ic) return;
  for(i=1;i<=MAXR;i++) regsv[i]=0;
  for(i2=0;i2<=MAXR*4;i2++){
    int only_best,pointertype;
    if(i2<=MAXR*2){i=i2;only_best=1;} else {i=i2/2;pointertype=only_best=0;}
    if(i<=MAXR&&regsv[i]) continue;
    if(i>MAXR&&regsv[i-MAXR]) continue;
    if(i>MAXR||!regsv[i]){
      if(i>MAXR){
	i-=MAXR;
	if(regsv[i]) continue;
      }else{
	/*  Ziehe Scratchregister vor, wenn kein Funktionsaufruf */
	/*  erfolgt, sonst erst andere                           */
	if(!regscratch[i]) continue;
      }
      if(regused[i]) continue;
      if(reg_pair(i,&rp)){
	if(regused[rp.r1]||regused[rp.r2]) continue;
      }
      if(simple_scratch[i]) continue;
      /*pri=2;*/ pri=0;
      for(j=0;j<=1;j++){
	if(j==0) v=vl3; else v=vl2;
	while(v){
	  if(v->storage_class==AUTO||v->storage_class==REGISTER){
	    if(!(v->flags&USEDASADR)&&!(v->vtyp->flags&VOLATILE)){
              int vt=v->vtyp->flags&NU;
              /* vla */
              if(ISARRAY(vt)&&is_vlength(v->vtyp))
                vt=POINTER_TYPE(v->vtyp->next);
	      if(only_best&&v->vtyp->next) pointertype=v->vtyp->next->flags;
	      if(v->priority>pri&&regok(i,vt,pointertype)){
		regsv[i]=v;pri=v->priority;
	      }
	    }
	  }
	  v=v->next;
	}
      }
    }
    if(regsv[i]){
      if(DEBUG&1) printf("Assigned <%s>(%p) to %s,%d\n",regsv[i]->identifier,(void *)regsv[i],regnames[i],i);
      regsv[i]->priority=0;regused[i]=1;
      if(reg_pair(i,&rp)) {regused[rp.r1]=regused[rp.r2]=1;}
      if(!zmleq(l2zm(0L),regsv[i]->offset)&&!(regsv[i]->flags&(CONVPARAMETER|REGPARM))){
	icp=new_IC();
	icp->line=0;
	icp->file=0;
	icp->q1.am=icp->q2.am=icp->z.am=0;
	icp->code=ASSIGN;
	icp->typf=regsv[i]->vtyp->flags&NU;
	icp->q1.flags=VAR;
	icp->q1.v=regsv[i];
	icp->q1.val.vmax=l2zm(0L);
	icp->q2.flags=0;
	icp->q2.val.vmax=szof(regsv[i]->vtyp);
	icp->z.flags=REG;
	icp->z.reg=i;
	icp->next=first_ic;
	icp->prev=0;
	first_ic->prev=icp;
	first_ic=icp;
      }
      icp=new_IC();
      icp->line=0;
      icp->file=0;
      icp->q1.am=icp->q2.am=icp->z.am=0;
      icp->code=ALLOCREG;
      icp->q1.flags=REG;
      icp->q1.reg=i;
      icp->q2.flags=icp->z.flags=icp->typf=0;
      icp->next=first_ic;
      icp->prev=0;
      first_ic->prev=icp;
      first_ic=icp;
      icp=new_IC();
      icp->q1.am=icp->q2.am=icp->z.am=0;
      icp->code=FREEREG;
      icp->q1.flags=REG;
      icp->q1.reg=i;
      icp->q2.flags=icp->z.flags=icp->typf=0;
      icp->next=0;
      add_IC(icp);
    }
  }
  icp=start;
  while(icp){
    if((icp->code==ALLOCREG||icp->code==FREEREG)&&regsv[icp->q1.reg]){
      /*  irgendwelche allocreg/freereg im Code entfernen     */
      /*  sollte nur beim Returnregister vorkommen            */
      struct IC *m=icp->next;
      remove_IC(icp);
      icp=m;continue;
    }
    for(i=1;i<=MAXR;i++){
      if(!regsv[i]) continue;
      if((icp->q1.flags&(VAR|DONTREGISTERIZE))==VAR&&icp->q1.v==regsv[i]){
	icp->q1.flags|=REG;
	icp->q1.reg=i;
      }
      if((icp->q2.flags&(VAR|DONTREGISTERIZE))==VAR&&icp->q2.v==regsv[i]){
	icp->q2.flags|=REG;
	icp->q2.reg=i;
      }
      if((icp->z.flags&(VAR|DONTREGISTERIZE))==VAR&&icp->z.v==regsv[i]){
	icp->z.flags|=REG;
	icp->z.reg=i;
      }
    }
    icp=icp->next;
  }
}



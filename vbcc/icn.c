/*  $VER: vbcc (ic.c) V0.8  */

#include "vbc.h"

static char FILE_[]=__FILE__;

static struct IC *first_pushed;
static unsigned int opushed;

static int volatile_convert;

int do_arith(np,struct IC *,np,struct obj *);

static void handle_reglist(struct regargs_list *,struct obj *);

void gen_test(struct obj *o,int t,int branch,int label)
/*  Generiert ein test o, branch label und passt auf, dass      */
/*  kein TEST const generiert wird.                             */
{
    struct IC *new;
    if(o->flags&KONST){
        eval_const(&o->val,t);
        if(zldeqto(vldouble,d2zld(0.0))&&zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))){
            if(branch==BEQ) branch=BRA; else branch=0;
        }else{
            if(branch==BNE) branch=BRA; else branch=0;
        }
    }else{
        new=new_IC();
        new->code=TEST;
        new->q2.flags=new->z.flags=0;
        new->typf=t;
        new->q1=*o;
        add_IC(new);
    }
    if(branch){
        new=new_IC();
        new->code=branch;
        new->typf=label;
        add_IC(new);
    }
}

void inline_memcpy(np z,np q,zmax size)
/*  fuegt ein ASSIGN-IC ein, das memcpy(z,q,size) entspricht    */
{
    struct IC *new=new_IC();
    if(!ISPOINTER(z->ntyp->flags)) ierror(0);
    if(!ISPOINTER(q->ntyp->flags)) ierror(0);

    if(z->flags==ADDRESS||z->flags==ADDRESSA||z->flags==ADDRESSS){
      gen_IC(z,0,0);
      new->z=z->left->o;
      if(z->flags!=ADDRESSS&&z->left->flags==IDENTIFIER&&is_vlength(z->left->ntyp)){
	/* variable length array */
	new->z.flags|=DREFOBJ;
	new->z.dtyp=POINTER_TYPE(z->ntyp->next);
      }
    }else{
      gen_IC(z,0,0);
      if(z->o.flags&DREFOBJ){
        struct IC *n2=new_IC();
        n2->code=ASSIGN;
        n2->typf=q->ntyp->flags;
        n2->q1=z->o;
        get_scratch(&n2->z,z->ntyp->flags,z->ntyp->next->flags,z->ntyp);
        n2->q2.flags=0;
        n2->q2.val.vmax=sizetab[POINTER_TYPE(z->ntyp->next)];
        new->z=n2->z;
        add_IC(n2);
      }else{
        new->z=z->o;
      }
      if(new->z.flags&VARADR){
	new->z.flags&=~VARADR; 
      }else{
	new->z.flags|=DREFOBJ;
	new->z.dtyp=POINTER_TYPE(z->ntyp->next);
      }
    }
    if(q->flags==ADDRESS||q->flags==ADDRESSA||q->flags==ADDRESSS){
      gen_IC(q,0,0);
      new->q1=q->left->o;
      if(q->flags!=ADDRESSS&&q->left->flags==IDENTIFIER&&is_vlength(q->left->ntyp)){
	/* variable length array */
	new->q1.flags|=DREFOBJ;
	new->q1.dtyp=POINTER_TYPE(q->ntyp->next);
      }
    }else{    
      gen_IC(q,0,0);
      if(q->o.flags&DREFOBJ){
        struct IC *n2=new_IC();
        n2->code=ASSIGN;
        n2->typf=q->ntyp->flags;
        n2->q1=q->o;
        get_scratch(&n2->z,q->ntyp->flags,q->ntyp->next->flags,q->ntyp);
        n2->q2.flags=0;
        n2->q2.val.vmax=sizetab[POINTER_TYPE(q->ntyp->next)];

        new->q1=n2->z;
        add_IC(n2);
      }else{
        new->q1=q->o;
      }
      if(new->q1.flags&VARADR){
	new->q1.flags&=~VARADR;
      }else{
	new->q1.flags|=DREFOBJ;
	new->q1.dtyp=POINTER_TYPE(q->ntyp->next);
      }
    }
    new->code=ASSIGN;
    new->typf=UNSIGNED|CHAR;
    new->q2.flags=0;
    new->q2.val.vmax=size;
    add_IC(new);
}

void add_IC(struct IC *new)
/*  fuegt ein IC ein                                            */
{
    int code;
    if(!new) return;
    if(nocode) {
#ifdef HAVE_MISRA
		misra_neu(52,14,1,-1);
#endif
		free(new);return;}
    new->next=0;
    new->q1.am=new->q2.am=new->z.am=0;
    new->line=line; new->file=filename;
    code=new->code;
    if(code>=BEQ&&code<=BRA) new->q1.flags=new->q2.flags=new->z.flags=0;
    if(code==ALLOCREG||code==FREEREG||code==SAVEREGS||code==RESTOREREGS) new->typf=0;
    if(DEBUG&64){ pric(stdout,first_ic);printf("new\n");pric2(stdout,new);printf("-\n");}
    if(new->q1.flags&VAR){
        if(!new->q1.v) ierror(0);
        new->q1.v->flags|=USEDASSOURCE;
        if(code==ADDRESS||(new->q1.flags&VARADR))
	  if(!is_vlength(new->q1.v->vtyp))
	    new->q1.v->flags|=USEDASADR;
        new->q1.v->priority+=currentpri;
    }
    if(new->q2.flags&VAR){
        if(!new->q2.v) ierror(0);
        new->q2.v->flags|=USEDASSOURCE;
        if(code==ADDRESS||(new->q2.flags&VARADR))
   	  if(!is_vlength(new->q2.v->vtyp))
	    new->q2.v->flags|=USEDASADR;
        new->q2.v->priority+=currentpri;
    }
    if(new->z.flags&VAR){
        if(!new->z.v) ierror(0);
        if(new->z.flags&DREFOBJ)
	  new->z.v->flags|=USEDASSOURCE;
	else
	  new->z.v->flags|=USEDASDEST;
        new->z.v->priority+=currentpri;
    }
    if(block_vla[nesting]){
      /* special handling for blocks with variable-length arrays */
      struct llist *p;
      if(new->code==LABEL){
	p=mymalloc(LSIZE);
	p->label=new->typf;
	p->next=vladeflabels[nesting];
	vladeflabels[nesting]=p;
      }else if(0&&new->code==BRA&&!(new->flags&LOOP_COND_TRUE)){
	p=mymalloc(LSIZE);
	p->label=new->typf;
	p->next=vlajmplabels[nesting];
	vlajmplabels[nesting]=p;
	p=vladeflabels[nesting];
	while(p){
	  if(p->label==new->typf)
	    break;
	  p=p->next;
	}
	if(!p){ 
	  /* label not declared in this block so far;
	     re-adjust stack, but store the position - label may be
	     declared later in the same block and adjustement must be
	     removed */
	  struct vlaadjust_list *vl;
	  struct IC *first;
	  first=last_ic;
	  freevl();
	  first=first->next;
	  vl=mymalloc(sizeof(*vl));
	  vl->branch=new;
	  vl->first=first;
	  vl->next=vlaadjusts[nesting];
	  vlaadjusts[nesting]=vl;
	}
      }
    }
    if(/*(c_flags_val[0].l&2)&&*/code==LABEL){
    /*  entfernt Spruenge zu direkt folgenden Labels    */
        struct IC *p=last_ic;
        while(p){
            if(p->typf==new->typf&&p->code>=BEQ&&p->code<=BRA){
                struct IC *n;
		if(p->code!=BRA){
		  struct IC *cmp=p->prev;
		  while(cmp&&cmp->code==FREEREG) cmp=cmp->prev;
		  if(cmp->code==TEST){
		    if(is_volatile_obj(&cmp->q1))
		      break;
		    else{
		      if(DEBUG&1) printf("preceding test removed\n");
		      remove_IC(cmp);
		    }
		  }else if(cmp->code==COMPARE){
		    if(is_volatile_obj(&cmp->q1)||is_volatile_obj(&cmp->q2))
		      break;
		    else{
		      if(DEBUG&1) printf("preceding compare removed\n");
		      remove_IC(cmp);
		    }
		  }else
		    break;
		}
		if(vlaadjusts[nesting]){
		  /* remove it from vlaadjusts-lists, if necessary */
		  struct vlaadjust_list *vl=vlaadjusts[nesting];
		  while(vl){
		    if(vl->branch==p){
		      struct IC *np=vl->branch->prev;
		      while(np!=vl->first->prev){
			if(!np) ierror(0);
			np->code=NOP;
			np->q1.flags=np->q2.flags=np->z.flags=0;
			np=np->prev;
		      }
		      vl->branch=0;
		    }
		    vl=vl->next;
		  }
		}
                if(DEBUG&1) printf("%s l%d deleted\n",ename[p->code],p->typf);
                n=p->next;
                remove_IC(p);
                p=n;
            }else{
                if(p->code!=LABEL) break;
                p=p->prev;
            }
        }
    }
    if(last_ic){
        if(code==ASSIGN){
            if((last_ic->z.flags&(REG|SCRATCH|DREFOBJ))==(REG|SCRATCH)&&(new->q1.flags==last_ic->z.flags)&&last_ic->z.reg==new->q1.reg/*&&last_ic->code!=CALL*/){
                if(USEQ2ASZ||!(last_ic->q2.flags&REG)||!(new->z.flags&REG)||last_ic->q2.reg!=new->z.reg){
                    if(USEQ2ASZ||!(last_ic->q2.flags&VAR)||!(new->z.flags&VAR)||last_ic->q2.v!=new->z.v){
                    /*  verbindet op a,b->reg,move reg->c zu op a,b->c  */
                    /*  hier fehlt aber noch Registerfreigabe           */
                        last_ic->z=new->z;
                        if(DEBUG&1) printf("move and op combined\n");
                        if((new->q1.flags&(SCRATCH|REG))==(SCRATCH|REG)&&(new->q1.reg!=new->z.reg||!(new->z.flags&REG)))
                            free_reg(new->q1.reg);
                        free(new);
                        return;
                    }
                }
            }
            if((last_ic->z.flags&(VAR|SCRATCH|DREFOBJ))==(VAR|SCRATCH)&&(new->q1.flags==last_ic->z.flags)&&last_ic->z.v==new->q1.v/*&&last_ic->code!=CALL*/){
                if(USEQ2ASZ||!(last_ic->q2.flags&REG)||!(new->z.flags&REG)||last_ic->q2.reg!=new->z.reg){
                    if(USEQ2ASZ||!(last_ic->q2.flags&VAR)||!(new->z.flags&VAR)||last_ic->q2.v!=new->z.v){
                    /*  verbindet op a,b->scratch,move scratch->c zu op a,b->c  */
                    /*  hier fehlt aber noch Registerfreigabe           */
                        last_ic->z=new->z;
                        if(DEBUG&1) printf("move and op combined(2)\n");
/*                        if((new->q1.flags&(SCRATCH|REG))==(SCRATCH|REG)&&(new->q1.reg!=new->z.reg||!(new->z.flags&REG)))
                            free_reg(new->q1.reg);*/
                        free(new);
                        return;
                    }
                }
            }

        }
        if(last_ic->code==BRA){
            if(code!=LABEL&&code!=ALLOCREG&&code!=FREEREG){
            /*  loescht alles nach bra bis ein Label kommt  */
            /*  momentan noch nicht perfekt, da es bei alloc/freereg stoppt */
                free(new);
#ifdef HAVE_MISRA
		misra_neu(52,14,1,-1);
#endif
                if(DEBUG&1) printf("Unreachable Statement deleted\n");
                return;
            }
            if(last_ic->prev&&code==LABEL){
            /*  ersetzt bcc l1;bra l2;l1 durch b!cc l2      */
                if(last_ic->prev->code>=BEQ&&last_ic->prev->code<=BGT&&new->typf==last_ic->prev->typf){
		  if(last_ic->next) pric2(stdout,last_ic->next);
                    if(DEBUG&1) printf("%s l%d;%s l%d; substituted\n",ename[last_ic->prev->code],last_ic->prev->typf,ename[last_ic->code],last_ic->typf);
                    if(last_ic->prev->code&1) 
		      last_ic->prev->code--;
		    else
		      last_ic->prev->code++;
                    last_ic->prev->typf=last_ic->typf;
                    last_ic=last_ic->prev;
		    free(last_ic->next);
                    last_ic->next=new;new->prev=last_ic;
                    last_ic=new;
                    return;
                }
            }
        }
/*        }*/
        new->prev=last_ic;
        last_ic->next=new;
        last_ic=new;
    }else{
        last_ic=new;first_ic=new;new->prev=0;
    }
    ic_count++;
    /*  Merken, on Fliesskomma benutzt wurde    */
    if(code!=LABEL&&(code<BEQ||code>BRA)){
        if(ISFLOAT(new->typf)) float_used=1;
        if(code==CONVERT&&ISFLOAT(new->typf2)) float_used=1;
    }
    if((new->q1.flags&(SCRATCH|REG))==(SCRATCH|REG)&&(new->q1.reg!=new->z.reg||!(new->z.flags&REG)))
        free_reg(new->q1.reg);
    if((new->q2.flags&(SCRATCH|REG))==(SCRATCH|REG)&&(new->q2.reg!=new->z.reg||!(new->z.flags&REG)))
        free_reg(new->q2.reg);
}

np gen_libcall(char *fname,np arg1,struct Typ *t1,np arg2,struct Typ *t2)
/* generate call to a library function (emulate operation) */
{
  np new; 
  struct argument_list *al=0,*t;
  new=mymalloc(NODES);
  new->flags=CALL;
  new->right=0;
  new->left=mymalloc(NODES);
  new->left->flags=IDENTIFIER;
  new->left->left=new->left->right=0;
  new->left->identifier=add_identifier(fname,strlen(fname));
  new->left->ntyp=0;
  new->left->sidefx=0;
  new->left->val.vmax=l2zm(0L); 
  new->alist=0;
  if(arg1){
    al=mymalloc(sizeof(*al));
    al->arg=arg1;
    al->next=0;
    if(t1){
      np cnv=mymalloc(NODES);
      cnv->flags=CAST;
      cnv->left=arg1;

      cnv->right=0;
      cnv->ntyp=t1;
      al->arg=cnv;
    }
  }
  if(arg2){
    t=mymalloc(sizeof(*t));
    t->arg=arg2;
    t->next=0;
    al->next=t;
    if(t2){
      np cnv=mymalloc(NODES);
      cnv->flags=CAST;
      cnv->left=arg2;
      cnv->right=0;
      cnv->ntyp=t2;
      t->arg=cnv;
    }
  }
  new->alist=al;
  /* Kann man type_expr nochmal auf die Argumente anwenden? */
  if(!type_expression(new))
    ierror(0); 
  gen_IC(new,0,0);
  return new;
}


void gen_IC(np p,int ltrue,int lfalse)
/*  Erzeugt eine IC-Liste aus einer expression      */
{
    struct IC *new; struct regargs_list *rl;
    if(!p) return;
    if(p->flags==STRING){
    /*  hier fehlt noch die Verwaltung der String-Inhalte   */
        p->o.v=add_var(empty,clone_typ(p->ntyp),STATIC,p->cl);
        p->o.v->flags|=DEFINED;
        p->o.flags=VAR;
        p->o.reg=0;
        p->o.val=p->val;
        return;
    }
    if(p->flags==IDENTIFIER){
/*        p->o.v=find_var(p->identifier,0);*/
        p->o.flags=VAR;
        p->o.reg=0;
        p->o.val=p->val;
        return;
    }
    if(p->flags==CEXPR||p->flags==PCEXPR){
        if(p->left){
            if(p->left->flags==POSTINC) p->left->flags=PREINC;
            if(p->left->flags==POSTDEC) p->left->flags=PREDEC;
            gen_IC(p->left,0,0);
            if((p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->left->o.reg);
        }
        if(p->right){
            if(p->right->flags==POSTINC) p->right->flags=PREINC;
            if(p->right->flags==POSTDEC) p->right->flags=PREDEC;
            gen_IC(p->right,0,0);
            if((p->right->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->right->o.reg);
        }
        p->o.flags=KONST;
        p->o.val=p->val;
        p->o.reg=0;
        return;
    }
    if(p->flags==KOMMA){
        if(p->left->sidefx){
            gen_IC(p->left,0,0);
            if((p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->left->o.reg);
        }else if(!(p->left->ntyp->flags&VOLATILE)) 
	  error(129);
        gen_IC(p->right,0,0);
        p->o=p->right->o;
        return;
    }
    if(p->flags==CAST){
        gen_IC(p->left,0,0);
        if((p->ntyp->flags&NQ)==VOID){
            if((p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->left->o.reg);
            p->o.flags=0;
        }else{
	  if(ISPOINTER(p->ntyp->flags)&&(p->ntyp->next->flags&VOLATILE))
	    volatile_convert=1;
	  convert(p->left,p->ntyp->flags);
	  if(volatile_convert){
	    if(!(p->left->o.flags&VAR))
	      ierror(0);
	    if(!p->left->o.v->vtyp->next)
	      ierror(0);
	    p->left->o.v->vtyp->next->flags|=VOLATILE;
	    volatile_convert=0;
	  }
	  p->o=p->left->o;
        }
        return;
    }
    if(p->flags==FIRSTELEMENT){
        gen_IC(p->left,0,0);
        p->o=p->left->o;
        return;
    }
#if HAVE_LIBCALLS
    { char *libname;
    if(libname=use_libcall(p)){
      np lc;struct Typ *t1,*t2;
      if(p->flags==LSHIFT||p->flags==RSHIFT){
	t1=clone_typ(p->ntyp);
	t2=new_typ();
	t2->flags=INT;
      }else{
	t1=arith_typ(p->left->ntyp,p->right->ntyp);
	t2=clone_typ(t1);
      }
      lc=gen_libcall(libname,p->left,t1,p->right,t2);
      *p=*lc;
      free(lc);
      return;
    }
    }
#endif
    new=new_IC();
    new->typf=p->ntyp->flags;
    new->q1.reg=new->q2.reg=new->z.reg=0;
    new->q1.flags=new->q2.flags=new->z.flags=0;
    if((p->flags>=LSHIFT&&p->flags<=MOD)||(p->flags>=OR&&p->flags<=AND)||p->flags==PMULT){
      do_arith(p,new,0,0);
      return;
    }
    if(p->flags==CONTENT){
        gen_IC(p->left,0,0);
        if(p->left->o.flags&VARADR){
            free(new);
            p->o=p->left->o;
            p->o.flags&=~VARADR;
            return;
        }
        if(!(p->left->o.flags&DREFOBJ)){
            free(new);
            p->o=p->left->o;
            p->o.flags|=DREFOBJ;
	    p->o.dtyp=p->left->ntyp->flags;
        }else{
          if((p->left->o.flags&SCRATCH)&&p->left->o.dtyp==POINTER_TYPE(p->ntyp)){
            new->z=p->left->o;
            new->z.flags&=~DREFOBJ;
          }else{
            get_scratch(&new->z,p->left->ntyp->flags,p->ntyp->flags,p->left->ntyp);
          }
          new->code=ASSIGN;new->typf=POINTER_TYPE(p->ntyp);
          new->q1=p->left->o;
          new->q2.val.vmax=sizetab[new->typf];
          p->o=new->z;
          add_IC(new);
          p->o.flags|=DREFOBJ;
	  p->o.dtyp=POINTER_TYPE(p->ntyp);
        }
        return;
    }
    if(p->flags==ASSIGN){
        new->code=ASSIGN;
        gen_IC(p->right,0,0);
        gen_IC(p->left,0,0);
        convert(p->right,p->ntyp->flags);
        new->q1=p->right->o;
        new->z=p->left->o;
        new->q2.val.vmax=szof(p->left->ntyp);
        p->o=new->z;
        add_IC(new);
        return;
    }
    if(p->flags==ASSIGNOP){
    /*  das hier ist nicht besonders schoen */
      struct obj o,q;struct IC *n;int f;char *libname;
      if(p->right->right==0){
        /*  sowas wie a+=0 wurde wegoptimiert   */
	free(new);
	p->o=p->left->o;
	return;
      }
#if HAVE_LIBCALLS
      if(libname=use_libcall(p->right)){
	np lc;struct Typ *t1,*t2;
	t1=clone_typ(p->ntyp);
	if(p->right->flags==LSHIFT||p->right->flags==RSHIFT){
	  t2=new_typ();
	  t2->flags=INT;
	}else
	  t2=clone_typ(p->ntyp);
	lc=gen_libcall(libname,p->right->left,t1,p->right->right,t2);
	*p->right=*lc;
	o=p->left->o;
	p->left=0;
	free(lc);
      }else
#endif
	{
	  f=do_arith(p->right,new,p->left,&o);
	  if(!f) ierror(0);
	  if(f>1) ierror(0);
	}
      if(!nocode&&(o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&!regs[o.reg]){
	/*  ueberfluessiges FREEREG entfernen   */
	n=last_ic;
	while(n){
	  if(!nocode&&n->code==CALL&&regscratch[o.reg]){
	    struct IC *ret;
	    regs[o.reg]=1;
	    savescratch(MOVEFROMREG,n->prev,0,&n->q1);
	    ret=n->next;
	    while(ret->code==ALLOCREG||ret->code==FREEREG)
	      ret=ret->next;
	    if(ret->code==GETRETURN){
	      if((ret->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&ret->z.reg==o.reg) ret->z.flags&=~REG;
	    }else
	      ret=ret->prev;
	    savescratch(MOVETOREG,ret,0,&n->q1);
	  }
	  if(n->code==FREEREG&&n->q1.reg==o.reg){
	    remove_IC(n);if(!nocode) regs[o.reg]=1;
	    break;
	  }
	  n=n->prev;
	}
      }
      convert(p->right,p->ntyp->flags);
      new=new_IC();
      new->typf=p->ntyp->flags;
      new->q2.flags=0;
      new->code=ASSIGN;
      new->q1=p->right->o;
      new->z=o;
      new->q2.val.vmax=szof(p->ntyp);
      p->o=new->z;
      add_IC(new);
      return;
    }
    if(p->flags==MINUS||p->flags==KOMPLEMENT){
      new->code=p->flags;
      gen_IC(p->left,0,0);
      convert(p->left,p->ntyp->flags);
      if((p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&regok(p->left->o.reg,p->ntyp->flags,0)){
	new->z=p->left->o;
	new->z.flags&=~DREFOBJ;
      }else{
	get_scratch(&new->z,p->left->ntyp->flags,0,p->left->ntyp);
      }
      new->q1=p->left->o;
      p->o=new->z;
      add_IC(new);
      return;
    }
    if(p->flags==ADDRESS||p->flags==ADDRESSA||p->flags==ADDRESSS){
      if(p->flags!=ADDRESSS&&p->left->flags==IDENTIFIER&&is_vlength(p->left->ntyp)){
	gen_IC(p->left,0,0);
	if(!(p->left->o.flags&VAR))
	  ierror(0);
	free(new);
	p->o=p->left->o;
	return;
      }
      new->code=ADDRESS;
      new->typf=p->left->ntyp->flags;
      new->typf2=POINTER_TYPE(p->left->ntyp);
      gen_IC(p->left,0,0);
      if(p->left->o.flags&VAR) p->left->o.v->flags|=(USEDASSOURCE|USEDASDEST);
      if(p->left->o.flags&DREFOBJ){
	free(new);
	p->o=p->left->o;
	p->o.flags&=~DREFOBJ;
	return;
      }
      if((p->left->o.flags&VAR)&&!(p->left->o.flags&VARADR)
	 &&(p->left->o.v->storage_class==EXTERN||p->left->o.v->storage_class==STATIC)){
	free(new);
	p->o=p->left->o;
	p->o.flags|=VARADR;
	return;
      }
      new->q1=p->left->o;
      get_scratch(&new->z,POINTER_TYPE(p->ntyp->next),p->ntyp->next->flags,0);
      p->o=new->z;
      add_IC(new);
      return;
    }
    if(p->flags==LAND||p->flags==LOR){
        int l1,l2,l3,l4;
/*        printf("%s true=%d false=%d\n",ename[p->flags],ltrue,lfalse);*/
        l1=++label;
	if(!ltrue) {l2=++label;l3=++label;l4=++label;}
        if(!ltrue){
	  if(p->flags==LAND)
	    gen_IC(p->left,l1,l3);
	  else 
	    gen_IC(p->left,l3,l1);
        }else{
	  if(p->flags==LAND) 
	    gen_IC(p->left,l1,lfalse);
	  else 
	    gen_IC(p->left,ltrue,l1);
        }
        if(p->left->o.flags!=0){
            if(p->flags==LAND)
                gen_test(&p->left->o,p->left->ntyp->flags,BEQ,((!ltrue)?l3:lfalse));
            else
                gen_test(&p->left->o,p->left->ntyp->flags,BNE,((!ltrue)?l3:ltrue));
        }
        gen_label(l1);
        if(!ltrue){
	  if(p->flags==LAND) 
	    gen_IC(p->right,l2,l3);
	  else
	    gen_IC(p->right,l3,l2);
        }else
	  gen_IC(p->right,ltrue,lfalse);
        if(p->right->o.flags!=0){
            if(p->flags==LAND)
                gen_test(&p->right->o,p->right->ntyp->flags,BEQ,((!ltrue)?l3:lfalse));
            else
                gen_test(&p->right->o,p->right->ntyp->flags,BNE,((!ltrue)?l3:ltrue));
        }
        if(!ltrue){
            gen_label(l2);
            if(p->flags==LAND) gen_cond(&p->o,0,l3,l4); else gen_cond(&p->o,1,l3,l4);
        }else{
            new=new_IC();
            new->code=BRA;
            if(p->flags==LAND) new->typf=ltrue; else new->typf=lfalse;
            add_IC(new);
        }
        if(ltrue) p->o.flags=0;
        return;
    }
    if(p->flags==NEGATION){
        int l1,l2,l3;
        if(!ltrue) {l1=++label;l2=++label;l3=++label;}
        if(ltrue)
	  gen_IC(p->left,lfalse,ltrue);
	else
	  gen_IC(p->left,l1,l3);
        if(!p->left->o.flags){
	  free(new);p->o.flags=0;
        }else{
            gen_test(&p->left->o,p->left->ntyp->flags,BNE,((!ltrue)?l1:lfalse));
        }
        if(ltrue){
            new=new_IC();
            new->code=BRA;
            if(!ltrue) new->typf=l2; else new->typf=ltrue;
            add_IC(new);
            p->o.flags=0;
        }else{
            gen_label(l3);
            gen_cond(&p->o,0,l1,l2);
        }
        return;
    }
    if(p->flags>=EQUAL&&p->flags<=GREATEREQ){
        int l1,l2,l3,tl,tr;
        if(!ltrue) {l1=++label;l2=++label;l3=++label;}
        if(p->left->flags==CEXPR){
        /*  Konstanten nach rechts  */
            np merk;merk=p->left;p->left=p->right;p->right=merk;
            if(p->flags==LESS) p->flags=GREATER;
            else if(p->flags==LESSEQ) p->flags=GREATEREQ;
            else if(p->flags==GREATER) p->flags=LESS;
            else if(p->flags==GREATEREQ) p->flags=LESSEQ;
        }
        new->code=COMPARE;
        tl=p->left->ntyp->flags&NU;tr=p->right->ntyp->flags&NU;
        if(p->right->flags==CEXPR&&ISINT(tr)&&ISINT(tl)){
            int negativ;
            eval_constn(p->right);
            if(zmleq(vmax,l2zm(0L))) negativ=1; else negativ=0;
            if((tl&UNSIGNED)||(tr&UNSIGNED)) negativ=0;
            if((!negativ||zmleq(t_min(tl),vmax))&&(negativ||zumleq(vumax,t_max(tl)))){
                convert(p->right,tl);
                tr=tl;
            }
        }
        if(ISARITH(tl)&&(tl!=tr||!shortcut(COMPARE,tl))){
            struct Typ *t;
            t=arith_typ(p->left->ntyp,p->right->ntyp);
            new->typf=t->flags;
            freetyp(t);
        }else{
            new->typf=p->left->ntyp->flags;
        }
        gen_IC(p->left,0,0);
        convert(p->left,new->typf);
        gen_IC(p->right,0,0);
        convert(p->right,new->typf);
        new->q1=p->left->o;
        new->q2=p->right->o;
        new->z.flags=0;
	if(p->flags==EQUAL||p->flags==INEQUAL){
	  /* generate TEST, if possible */
	  if(new->q2.flags&KONST){
	    eval_const(&new->q2.val,new->typf);
	    if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))){
	      new->code=TEST;
	      new->q2.flags=0;
	    }
	  }
	}
        add_IC(new);
        new=new_IC();
        if(p->flags==EQUAL) new->code=BEQ;
        if(p->flags==INEQUAL) new->code=BNE;
        if(p->flags==LESS) new->code=BLT;
        if(p->flags==LESSEQ) new->code=BLE;
        if(p->flags==GREATER) new->code=BGT;
        if(p->flags==GREATEREQ) new->code=BGE;
        if(ltrue) new->typf=ltrue; else new->typf=l1;
        add_IC(new);
        if(ltrue){
            new=new_IC();
            new->code=BRA;
            new->typf=lfalse;
            add_IC(new);
            p->o.flags=0;
        }else{
            gen_label(l3);
            gen_cond(&p->o,1,l1,l2);
        }
        return;
    }
    if(p->flags==CALL){
        int r=0;struct obj *op,cfunc,ret_obj;zmax sz;
        int mregs[MAXR+1];
	struct IC *callic;
#ifdef ORDERED_PUSH
        struct IC *merk_fp,*lp;
	unsigned int merk_opushed=opushed;
#endif
        if(p->left->flags==ADDRESS&&p->left->left->flags==IDENTIFIER){
            struct Var *v;
            gen_IC(p->left,0,0); r=1;
            v=p->left->o.v;
            if(v->fi&&v->fi->first_ic&&!cross_module&&(c_flags_val[0].l&4096)){
            /*  function call inlining  */
                struct argument_list *al;
                struct Var *vp,**argl1,**argl2;
                struct IC *ip;int lc;
                int arg_cnt=0,i;
                if(DEBUG&1024){
                    printf("inlining call to <%s>\n",v->identifier);
                    for(vp=v->fi->vars;vp;vp=vp->next)
                        printf("%s(%ld)/%p\n",vp->identifier,zm2l(vp->offset),(void*)vp);
                }
                for(vp=v->fi->vars;vp;vp=vp->next){
		  /*FIXME: zmeqto hier ok? siehe cross_module_inline */
		  if((zmeqto(vp->offset,l2zm(0L))||vp->reg)&&*vp->identifier&&(vp->storage_class==AUTO||vp->storage_class==REGISTER)) arg_cnt++;
                }

                /*  Argumente in die ersten Parametervariablen kopieren */
                argl1=mymalloc(arg_cnt*sizeof(struct Var *));
                argl2=mymalloc(arg_cnt*sizeof(struct Var *));

                al=p->alist;vp=v->fi->vars;i=0;
                while(al){
		    /*FIXME: zmeqto hier ok? siehe cross_module_inline */
		    while(vp&&(!*vp->identifier||(!zmeqto(vp->offset,l2zm(0L))&&!vp->reg)||(vp->storage_class!=REGISTER&&vp->storage_class!=AUTO))) vp=vp->next;
                    if(!vp){ error(39); break; }
                    if(i>=arg_cnt) ierror(0);
                    if(DEBUG&1024) printf("arg: %s(%ld)\n",vp->identifier,zm2l(vp->offset));
                    argl1[i]=vp;
                    argl2[i]=add_var(empty,clone_typ(vp->vtyp),vp->storage_class,0);
                    if(!al->arg) ierror(0);
                    gen_IC(al->arg,0,0);
                    convert(al->arg,vp->vtyp->flags);
                    new=new_IC();
                    new->code=ASSIGN;
                    new->q1=al->arg->o;
                    new->q2.flags=0;
                    new->q2.val.vmax=szof(vp->vtyp);
                    new->z.flags=VAR;
                    new->z.val.vmax=l2zm(0L);
                    new->z.v=argl2[i];
                    new->typf=vp->vtyp->flags;
                    add_IC(new);
                    i++;
                    al=al->next;
                    vp=vp->next;
                }
                if(i<arg_cnt){ error(83); arg_cnt=i;}

                /*  Kopien der Variablen erzeugen   */
                for(vp=v->fi->vars;vp;vp=vp->next){
                    vp->inline_copy=0;
                }
                for(i=0;i<arg_cnt;i++){
                    if(argl1[i]){
                        if(!argl2[i]) ierror(0);
                        argl1[i]->inline_copy=argl2[i];
                    }
                }

                /*  Rueckgabewert   */
                if((p->ntyp->flags&NQ)!=VOID){
                    p->o.flags=SCRATCH|VAR;
                    p->o.reg=0;p->o.val.vmax=l2zm(0L);
                    p->o.v=add_var(empty,clone_typ(p->ntyp),AUTO,0);
                }else{
                    p->o.flags=0;
                }

                free(argl1);
                free(argl2);

                /*  Code einfuegen und Labels umschreiben   */
                ip=v->fi->first_ic;lc=0;
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
                        if(!*iv->identifier&&zmeqto(iv->offset,l2zm(0L))){
                          new->z=p->o;
                        }else{
                          if(!iv->inline_copy){
                            iv->inline_copy=add_var(empty,clone_typ(iv->vtyp),iv->storage_class,0);
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
                          iv->inline_copy=add_var(empty,clone_typ(iv->vtyp),iv->storage_class,0);
                          iv->inline_copy->reg=iv->reg;
                        }
                        new->q1.v=iv->inline_copy;
                      }/*else if(iv->inline_copy) ierror(0);*/
                    }
                    if(ip->q2.flags&VAR){
                      iv=ip->q2.v;
                      if(iv->storage_class==AUTO||iv->storage_class==REGISTER){
                        if(!iv->inline_copy){
                          iv->inline_copy=add_var(empty,clone_typ(iv->vtyp),iv->storage_class,0);
                          iv->inline_copy->reg=iv->reg;
                        }
                        new->q2.v=iv->inline_copy;
                      }/*else if(iv->inline_copy) ierror(0);*/
                    }
                    if(c==CALL){
		      int i;
		      function_calls+=currentpri;
		      new->arg_list=mymalloc(sizeof(*new->arg_list)*new->arg_cnt);
		      for(i=0;i<new->arg_cnt;i++) new->arg_list[i]=ip->arg_list[i]->copy;
		    }
                    if(c==LABEL||(c>=BEQ&&c<=BRA)){
                        if(new->typf>lc) lc=new->typf;
                        new->typf+=label;
                    }
                    if(c==SETRETURN){
                        new->code=ASSIGN;
                        new->z=p->o;
                    }
		    add_IC(new);
                    ip=ip->next;
                }
                label+=lc;
                return;
            }
            /*  einige spezielle Inline-Funktionen; das setzt voraus, dass  */
            /*  diese in den Headerfiles passend deklariert werden          */
            if(v->storage_class==EXTERN){
	        if((optflags&2)&&!strcmp(v->identifier,"strlen")&&p->alist&&p->alist->arg){
                    np n=p->alist->arg;
                    if(n->flags==ADDRESSA&&n->left->flags==STRING&&zmeqto(n->left->val.vmax,l2zm(0L))){
                        struct const_list *cl;zumax len=ul2zum(0UL);
                        cl=n->left->cl;
                        while(cl){
			  if(zmeqto(l2zm(0L),zc2zm(cl->other->val.vchar))) break;
			  len=zumadd(len,ul2zum(1UL));
			  cl=cl->next;
                        }
                        p->o.val.vumax=len;
                        eval_const(&p->o.val,UNSIGNED|MAXINT);
                        insert_constn(p);
                        insert_const(&p->o.val,p->ntyp->flags);
                        p->flags=CEXPR;
                        p->o.flags=KONST;
                        return;
                    }
                }

                if(INLINEMEMCPY>0&&(optflags&2)){
                    if(!strcmp(v->identifier,"strcpy")&&p->alist&&p->alist->next&&p->alist->next->arg){
                        np n=p->alist->next->arg;
                        if(n->flags==ADDRESSA&&n->left->flags==STRING&&zmeqto(n->left->val.vmax,l2zm(0L))){
                            struct const_list *cl;zmax len=l2zm(0L);
                            cl=n->left->cl;
                            while(cl){
                                len=zmadd(len,l2zm(1L));
                                if(zmeqto(zc2zm(cl->other->val.vchar),l2zm(0L))) break;
                                cl=cl->next;
                            }
                            if(zmleq(len,l2zm((long)INLINEMEMCPY))){
                                inline_memcpy(p->alist->arg,n,len);
                                p->o=p->alist->arg->o;
                                return;
                            }
                        }
                    }
                    if(!strcmp(v->identifier,"memcpy")){
                        if(p->alist&&p->alist->next&&p->alist->next->next
                           &&p->alist->next->next->arg
                           &&p->alist->next->next->arg->flags==CEXPR){
                            eval_constn(p->alist->next->next->arg);
                            if(zmleq(vmax,l2zm((long)INLINEMEMCPY))){
                                inline_memcpy(p->alist->arg,p->alist->next->arg,vmax);
                                p->o=p->alist->arg->o;
                                return;
                            }
                        }
                    }
                }
            }
        }
        rl=0;
        if(!(optflags&2)){
          int r;
          for(r=1;r<=MAXR;r++){mregs[r]=regs[r];regs[r]&=~32;}
        }
#ifdef ORDERED_PUSH
        merk_fp=first_pushed;
        first_pushed=0;
        lp=last_ic;
#endif
#ifdef HAVE_REGPARMS
        if(!freturn(p->ntyp)&&(p->ntyp->flags&NQ)!=VOID){
          struct IC *new2;static struct Typ ptyp={0};
	  struct reg_handle reg_handle=empty_reg_handle;
	  int reg;
          new2=new_IC();
          new2->code=ADDRESS;
          new2->typf=p->ntyp->flags;
	  new2->typf2=POINTER_TYPE(p->ntyp);
          new2->q1.flags=VAR;
          new2->q1.v=add_var(empty,clone_typ(p->ntyp),AUTO,0);
          new2->q1.val.vmax=l2zm(0L);
          op=&new2->q1;
          new2->q2.flags=0;
          get_scratch(&new2->z,POINTER_TYPE(p->ntyp),p->ntyp->flags,0);
          ret_obj=new2->z;
          add_IC(new2);
	  ptyp.next=p->ntyp;
	  ptyp.flags=POINTER_TYPE(p->ntyp);
	  reg=reg_parm(&reg_handle,&ptyp,0,p->left->ntyp);
	  if(!reg) ierror(0);
	  sz=push_args(p->alist,p->left->ntyp->next->exact,0,&rl,&reg_handle,&ret_obj,p->ntyp,reg,p->left->ntyp);
	  if(optflags&2)
	    handle_reglist(rl,&ret_obj);
	}else{
	  struct reg_handle reg_handle=empty_reg_handle;
	  sz=push_args(p->alist,p->left->ntyp->next->exact,0,&rl,&reg_handle,0,0,-1,p->left->ntyp);
	  if(optflags&2)
	    handle_reglist(rl,0);
	}
#else
        sz=push_args(p->alist,p->left->ntyp->next->exact,0,&rl);
	if(optflags&2)
	  handle_reglist(rl,0);
#endif
        if(!r) gen_IC(p->left,0,0);
        if(!(p->left->o.flags&DREFOBJ)){
            free(new);
            p->o=p->left->o;
            if(p->o.flags&VARADR){
	      p->o.flags&=~VARADR;
	    }else{
	      p->o.flags|=DREFOBJ;
	      p->o.dtyp=p->left->ntyp->flags;
	    }
        }else{
            if(p->left->o.flags&VARADR){
                free(new);
                p->o=p->left->o;
                p->o.flags&=~VARADR;
            }else{
                if((p->left->o.flags&SCRATCH)&&p->ntyp->flags==p->left->ntyp->flags){
		  new->z=p->left->o;
		  new->z.flags&=~DREFOBJ;
                }else{
		  get_scratch(&new->z,p->left->ntyp->flags,FUNKT,p->left->ntyp);
                }
                new->code=ASSIGN;
		new->typf=p->left->ntyp->flags;
                new->q1=p->left->o;
                new->q2.val.vmax=sizetab[new->typf];
                new->q2.flags=0;
                p->o=new->z;
                add_IC(new);
                p->o.flags|=DREFOBJ;
		p->o.dtyp=p->left->ntyp->flags;
            }
        }
/*            p->left->o.flags-=DREFOBJ|VARADR; Was sollte das??    */

#if 0
        if(optflags&2){
            while(rl){
                struct regargs_list *m;
                new=new_IC();
                new->code=NOP;
                new->q1.flags=VAR;
                new->q1.v=rl->v;
                new->q1.val.vmax=l2zm(0L);
                new->typf=0;
                new->q2.flags=new->z.flags=0;
                add_IC(new);
                m=rl->next;free(rl);rl=m;
            }
        }
#endif

#ifndef HAVE_REGPARMS
        /*  gegebenenfalls Adresse des Ziels auf den Stack  */
        if(!freturn(p->ntyp)&&(p->ntyp->flags&NQ)!=VOID){
#ifdef ORDERED_PUSH
            ierror(0);
#endif
            new=new_IC();
            new->code=ADDRESS;
            new->typf=p->ntyp->flags;
	    new->typf2=POINTER_TYPE(p->ntyp->next);
            new->q1.flags=VAR;
            new->q1.v=add_var(empty,clone_typ(p->ntyp),AUTO,0);
            new->q1.val.vmax=l2zm(0L);
            op=&new->q1;
            new->q2.flags=0;
            get_scratch(&new->z,POINTER_TYPE(p->ntyp),p->ntyp->flags,0);
            ret_obj=new->z;
            add_IC(new);
            new=new_IC();
            new->code=PUSH;
            new->typf=POINTER_TYPE(p->ntyp);
            new->q1=ret_obj;
            new->q2.flags=new->z.flags=0;
            new->q2.val.vmax=sizetab[new->typf];
	    new->z.val.vmax=new->q2.val.vmax;
            add_IC(new);
            sz=zmadd(sz,sizetab[new->typf]);
        }
#endif

        /*  Scratchregister evtl. sichern   */
	cfunc=p->o;
        if(!nocode)
	  savescratch(MOVEFROMREG,last_ic,0,&cfunc);
        function_calls+=currentpri;
        new=new_IC();
        new->code=CALL;
	callic=new;
	if(p->alist){
	  /* insert list of argument ICs */
	  int i=0;
	  struct argument_list *a=p->alist;
	  while(a){i++;a=a->next;}
	  new->arg_cnt=i;
	  new->arg_list=mymalloc(sizeof(*new->arg_list)*i);
	  for(a=p->alist,i=0;a;a=a->next,i++) new->arg_list[i]=a->pushic;
	}else{
	  new->arg_cnt=0;
	  new->arg_list=0;
	}
        new->typf=FUNKT;
        new->q1=p->o;
        new->q2.flags=new->z.flags=0;
        new->q2.val.vmax=sz; /*  Groesse der Parameter auf dem Stack */
        add_IC(new);

        if(optflags&2){
            while(rl){
                struct regargs_list *m;
                new=new_IC();
                new->code=NOP;
                new->q1.flags=VAR;
                new->q1.v=rl->v;
                new->q1.val.vmax=l2zm(0L);
                new->typf=0;
                new->q2.flags=new->z.flags=0;
                add_IC(new);
                m=rl->next;free(rl);rl=m;
            }
        }

        r=0;
        if((p->ntyp->flags&NQ)!=VOID){
            new=new_IC();
            new->code=GETRETURN;
            new->q1.flags=new->q2.flags=0;
            new->q1.reg=freturn(p->ntyp);
            new->q2.val.vmax=szof(p->ntyp);
            new->typf=p->ntyp->flags;
            if(freturn(p->ntyp)){
              int t=p->ntyp->flags&NQ;
              if(ISSTRUCT(t)||ISUNION(t)){
                new->z.v=add_var(empty,clone_typ(p->ntyp),AUTO,0);
                new->z.flags=VAR;
                new->z.val.vmax=l2zm(0L);
              }else{
                if(optflags&2){
                  get_scratch(&new->z,p->ntyp->flags,0,p->ntyp);
                }else{
                  /* Suche geeignetes Register, um Rueckgabewert zu speichern. */
                  struct regargs_list *l2;
                  int r;
                  r=new->q1.reg;
                  if(regs[r]||!regok(r,p->ntyp->flags,0)||(reg_pair(r,&rp)&&(regs[rp.r1]||regs[rp.r2]))){
                    r=0;
                  }else{
                    for(l2=rl;l2;l2=l2->next){
                      if(l2->v&&abs(l2->reg)==r) {r=0;break;}
                    }
                  }
                  if(r==0){
                    for(r=1;r<=MAXR;r++){
                      if(!regs[r]&&regok(r,p->ntyp->flags,0)&&(!reg_pair(r,&rp)||(!regs[rp.r1]&&!regs[rp.r2]))){
                        for(l2=rl;l2;l2=l2->next){
                          if(l2->v&&abs(l2->reg)==r) break;
                        }
                        if(l2) continue;
                        break;
                      }
                    }
                  }
                  if(r>=1&&r<=MAXR){
                    alloc_hardreg(r);
                    new->z.flags=(REG|SCRATCH);
                    new->z.reg=r;
                  }else{
                    get_scratch(&new->z,p->ntyp->flags,0,p->ntyp);
                  }
                }
              }
            } else
              new->z=*op;
            if((new->z.flags&(REG|DREFOBJ))==REG) r=new->z.reg;
            p->o=new->z;
            add_IC(new);
        }else{
            p->o.flags=0;
        }
        /*  Scratchregister evtl. wiederherstellen  */
        if(!nocode)
	  savescratch(MOVETOREG,last_ic,r,&cfunc);
        if(!(optflags&2)){
          int r;
          for(r=1;r<=MAXR;r++){
            if(regs[r])
              regs[r]|=(mregs[r]&32);
          }
        }
        /*  Evtl. gespeicherte Registerargumente wiederherstellen.  */
        while(rl){
            struct regargs_list *m;
            if(rl->v){
	      int r;
	      for(r=MAXR;r>=1;r--){
		if(regs[r]&&reg_pair(r,&rp)&&(rp.r1==abs(rl->reg)||rp.r2==abs(rl->reg)))
		  break;
	      }
	      if(r<=1) r=abs(rl->reg);
	      new=new_IC();
	      new->code=MOVETOREG;
	      new->typf=0;
	      new->q1.flags=VAR|DONTREGISTERIZE;
	      new->q1.v=rl->v;
	      new->q1.val.vmax=l2zm(0L);
	      new->z.flags=REG;
	      new->z.reg=abs(r);
	      new->q2.flags=0;
	      new->q2.val.vmax=regsize[r];
	      add_IC(new);
            }else{
                new=new_IC();
                new->code=FREEREG;
                new->typf=0;
                new->q1.flags=REG;
                new->q2.flags=new->z.flags=0;
                new->q1.reg=abs(rl->reg);
                add_IC(new);
                regs[abs(rl->reg)]=0;
            }
            m=rl->next;free(rl);rl=m;
        }
#ifdef ORDERED_PUSH
        /* If arguments have been pushed nested we have to copy them and */
        /* push them later. */
        if(merk_fp&&opushed!=merk_opushed){
          struct IC *p,*m=0,*np;
          if(!lp) ierror(0);
          for(p=merk_fp;p;){
            np=p->next;
            if(p->code==PUSH){
              new=new_IC();
              *new=*p;
	      /* be careful about the address because of arg_list! */
	      if(p->prev) p->prev->next=new;
	      new->prev=p->prev;
	      if(p->next) p->next->prev=new;
	      new->next=p->next;
	      if(p==last_ic) last_ic=new;
	      if(p==first_ic) first_ic=new;
              if(new->q1.flags){
                new->code=ASSIGN;
                new->z.flags=VAR;
                new->z.val.vmax=l2zm(0L);
		/* typ allocated in push_args and not used there */
		/*		if(!p->ityp) ierror(0);*/
		new->z.v=add_tmp_var(clone_typ(p->ityp)); /*FIXME??*/
                p->q1=new->z;
		/*		p->ityp=0;*/
              }else{
                remove_IC(new);
              }
	      p->next=p->prev=0;
              add_IC(p);
              if(!m) m=p;
            }
            if(p==lp) break;
            p=np;
          }
          if(!m) ierror(0);
          first_pushed=m;
        }else
          first_pushed=merk_fp;
#endif
        return;
    }
    if(p->flags>=PREINC&&p->flags<=POSTDEC){
        struct obj o;
#if HAVE_LIBCALLS
	char *libname;
	struct node tn={ADD},one={CEXPR};
#endif
        gen_IC(p->left,0,0);
        if(p->flags==POSTINC||p->flags==POSTDEC){
	  /*new=new_IC();*/
            new->code=ASSIGN;
            new->typf=p->ntyp->flags;
            new->q2.val.vmax=sizetab[p->ntyp->flags&NQ];
            new->q1=p->left->o;
            new->q1.flags&=~SCRATCH;
            get_scratch(&new->z,p->left->ntyp->flags,0,p->left->ntyp);
            new->q2.flags=0;
            o=new->z;
            add_IC(new);
            new=new_IC();
        }else
	  o=p->left->o;
#if HAVE_LIBCALLS
	if(p->flags==PREDEC||p->flags==POSTDEC)
	  tn.flags=SUB;
	tn.left=p->left;
	tn.right=&one;
	tn.ntyp=p->ntyp;
	one.flags=CEXPR;
	one.ntyp=p->ntyp;
	gval.vmax=l2zm(1L);
	eval_const(&gval,MAXINT);
	insert_const(&one.val,p->ntyp->flags&NU);
	if(libname=use_libcall(&tn)){
	  np lc;
	  lc=gen_libcall(libname,p->left,0,&one,0);
	  new=new_IC();
	  new->code=ASSIGN;
	  new->typf=p->ntyp->flags;
	  new->q2.val.vmax=sizetab[p->ntyp->flags&NQ];
	  new->q1=lc->o;
	  new->q1.flags&=~SCRATCH;
	  new->z=p->left->o;
	  add_IC(new);
	  new=new_IC();
	  free(lc);
	}else
#endif
        if(ISPOINTER(p->left->ntyp->flags)){
            if(p->flags==PREINC||p->flags==POSTINC)
	      new->code=ADDI2P; 
	    else
	      new->code=SUBIFP;
            vmax=szof(p->left->ntyp->next);
            new->q2.val.vint=zm2zi(vmax);
            new->typf=INT;
	    new->typf2=p->left->ntyp->flags;
            new->q1=p->left->o;
            new->z=p->left->o;
            new->q2.flags=KONST;
            add_IC(new);
        }else{
            if(p->flags==PREINC||p->flags==POSTINC)
	      new->code=ADD;
	    else 
	      new->code=SUB;
            new->typf=p->ntyp->flags;
            new->q1=p->left->o;
            new->z=p->left->o;
            new->q2.flags=KONST;
	    gval.vint=zm2zi(l2zm(1L));
	    eval_const(&gval,INT);
	    insert_const(&new->q2.val,new->typf);
            add_IC(new);
        }
        if(p->flags==POSTINC||p->flags==POSTDEC){
            if((p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) 
	      free_reg(p->left->o.reg);
        }
        p->o=o;
        return;
    }
    if(p->flags==COND){
        int ltrue,lfalse,lout;
        ltrue=++label;lfalse=++label;lout=++label;
        gen_IC(p->left,ltrue,lfalse);
        if(!p->left->o.flags){
            free(new);
        }else{
            if(p->left->flags!=CEXPR){
                gen_test(&p->left->o,p->left->ntyp->flags,BEQ,lfalse);
            }else{
                eval_constn(p->left);
                if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))){
                    gen_IC(p->right->right,0,0);
                    p->o=p->right->right->o;
                }else{
                    gen_IC(p->right->left,0,0);
                    p->o=p->right->left->o;
                }
                return;
            }
        }
        gen_label(ltrue);
        gen_IC(p->right->left,0,0);
        if((p->ntyp->flags&NQ)!=VOID){
            convert(p->right->left,p->ntyp->flags);
            if((p->right->left->o.flags&(SCRATCH|DREFOBJ))==SCRATCH){
                p->o=p->right->left->o;
            }else{
                get_scratch(&p->o,p->ntyp->flags,0,p->ntyp);
                new=new_IC();
                new->code=ASSIGN;
                new->q1=p->right->left->o;
                new->z=p->o;
                new->q2.flags=0;
                new->q2.val.vmax=szof(p->ntyp);
                new->typf=p->ntyp->flags;
                p->o=new->z;
                add_IC(new);
            }
        }else
	  p->o.flags=0;
        new=new_IC();
        new->code=BRA;
        new->typf=lout;
        add_IC(new);
        gen_label(lfalse);
        gen_IC(p->right->right,0,0);
        if((p->ntyp->flags&NQ)!=VOID){
            convert(p->right->right,p->ntyp->flags);
            new=new_IC();
            new->code=ASSIGN;
            new->q1=p->right->right->o;
            new->z=p->o;
            new->q2.flags=0;
            new->q2.val.vmax=szof(p->ntyp);
            new->typf=p->ntyp->flags;
            add_IC(new);
        }
        gen_label(lout);
        return;
    }
    printf("Operation: %d=%s\n",p->flags,ename[p->flags]);
    ierror(0);
    free(new);
    p->o.flags=0;
}

static void handle_reglist(struct regargs_list *nrl,struct obj *radr)
{
  struct IC *new;
  /*  Letztes Argument; jetzt in Register laden.  */
#ifdef HAVE_REGPARMS
  int didradr=0;
#endif
  while(nrl){
    new=new_IC();
    new->code=ASSIGN;
    new->typf=nrl->v->vtyp->flags/*|VOLATILE*/;
    new->q1.flags=VAR;
    new->q1.v=nrl->v;
    new->q1.val.vmax=l2zm(0L);
    new->q2.flags=0;
    new->q2.val.vmax=szof(nrl->v->vtyp);
    new->z.flags=VAR;
    new->z.val.vmax=l2zm(0L);
    new->z.v=add_var(empty,clone_typ(nrl->v->vtyp),AUTO,0);
    new->z.v->reg=nrl->reg;
    new->z.v->vtyp->flags|=VOLATILE;
    nrl->v=new->z.v;
    add_IC(new);
    
#ifdef HAVE_REGPARMS
    if(radr&&!didradr){
      didradr=1;
    }else{
#endif
      nrl->al->pushic=new;
#ifdef HAVE_REGPARMS
    }
#endif
    nrl=nrl->next;
  }
}

#ifdef HAVE_REGPARMS
zmax push_args(struct argument_list *al,struct struct_declaration *sd,int n,struct regargs_list **rl,struct reg_handle *reg_handle,struct obj *radr,struct Typ *rtype,int rreg,struct Typ *fkt)
#else
zmax push_args(struct argument_list *al,struct struct_declaration *sd,int n,struct regargs_list **rl)
#endif
/*  Legt die Argumente eines Funktionsaufrufs in umgekehrter Reihenfolge    */
/*  auf den Stack. Es wird Integer-Erweiterung vorgenommen und float wird   */
/*  nach double konvertiert, falls kein Prototype da ist.                   */
{
    int t,reg,regpush,evaluated=0;struct Typ *ft;
    struct IC *new;struct regargs_list *nrl;zmax sz,rsz,of;struct obj *arg;
#ifdef HAVE_REGPARMS
    int stdreg;
    if(!al&&!radr) return(0);
    if(radr){
      stdreg=rreg;
    }else{
      if(n<sd->count){
        stdreg=reg_parm(reg_handle,(*sd->sl)[n].styp,0,fkt);
      }else{
        if(sd->count)
          stdreg=reg_parm(reg_handle,al->arg->ntyp,1,fkt);
        else
          stdreg=reg_parm(reg_handle,al->arg->ntyp,0,fkt);
      }
    }
    reg=stdreg;
#else
    if(!al) return(0);
    reg=0;
#endif
#ifdef HAVE_REGPARMS
    if(!radr){
#endif
      if(!al->arg) ierror(0);
      if(!sd) ierror(0);
      if(n<sd->count){
        ft=clone_typ((*sd->sl)[n].styp);sz=szof(ft);
	t=ft->flags;
        reg=(*sd->sl)[n].reg;
      }else{
        ft=clone_typ(al->arg->ntyp);sz=szof(ft);
	t=ft->flags;
      }
      if(ISINT(t)) {t=int_erw(t);ft->flags=t;sz=sizetab[t&NQ];}
      if((t&NQ)==FLOAT&&n>=sd->count) {t=DOUBLE;ft->flags=t;sz=sizetab[t];}
#ifdef HAVE_REGPARMS
    }else{
      ft=new_typ();
      ft->flags=t=POINTER_TYPE(rtype);
      ft->next=clone_typ(rtype);
      sz=sizetab[t];
    }
#endif
    if(reg<0) {reg=-reg;regpush=1;} else regpush=0;
    rsz=sz;
    sz=zmmult(zmdiv(zmadd(sz,zmsub(stackalign,l2zm(1L))),stackalign),stackalign);
#ifdef ORDERED_PUSH
    if(reg==0||regpush){
      new=new_IC();
      if(!radr&&!evaluated){
        gen_IC(al->arg,0,0);
        convert(al->arg,t);
        evaluated=1;
        new->q1=al->arg->o;
	al->pushic=new;
      }else
        new->q1=*radr;
      /*  Parameteruebergabe ueber Stack. */
      new->code=PUSH;
      new->typf=t;
      new->ityp=ft;
      new->q2.flags=new->z.flags=0;
      new->q2.val.vmax=sz;
      new->z.val.vmax=rsz;
      if(regpush){
        if(c_flags[26]&USEDFLAG){
          new->q1.am=new->q2.am=new->z.am=0;
          new->line=line; new->file=filename;
          insert_IC(last_ic,new);
        }else{
          new->q1.flags=0;
          add_IC(new);
        }
      }else{
        add_IC(new);
      }
      opushed++;
      if(!first_pushed) first_pushed=new;
      if(al&&!al->next&&!regpush) return sz;
    }
#endif
#ifdef HAVE_REGPARMS
    if(radr){
      if(al) of=push_args(al,sd,0,rl,reg_handle,0,0,0,fkt); else of=l2zm(0L);
    }else{
      if(al->next) of=push_args(al->next,sd,n+1,rl,reg_handle,0,0,0,fkt); else of=l2zm(0L);
    }
#else
    if(al->next) of=push_args(al->next,sd,n+1,rl); else of=l2zm(0L);
#endif
#ifdef ORDERED_PUSH
    if(reg==0) return zmadd(of,sz);
#endif

    if(regpush) of=zmadd(of,sz);

#ifdef HAVE_REGPARMS
    if(radr){
      arg=radr;
    }else{
      if(!evaluated){
        gen_IC(al->arg,0,0);
        convert(al->arg,t);
        evaluated=1;
      }
      arg=&al->arg->o;
    }
#else
    if(!evaluated){
      gen_IC(al->arg,0,0);
      convert(al->arg,t);
      evaluated=1;
    }
    arg=&al->arg->o;
#endif
#ifndef ORDERED_PUSH
    if(reg==0||regpush){
        /*  Parameteruebergabe ueber Stack. */
        new=new_IC();
        new->code=PUSH;
        new->typf=t;
        new->q1=*arg;
        if(regpush&&!(c_flags[26]&USEDFLAG)) new->q1.flags=0;
        new->q2.flags=new->z.flags=0;
        new->q2.val.vmax=sz;
	new->z.val.vmax=rsz;
        add_IC(new);
	al->pushic=new;
        if(!regpush) return(zmadd(of,sz));
    }
#endif
    if(reg){
        /*  Parameteruebergabe in Register. */
        struct Var *v=0; struct Typ *t2;
        if(optflags&2){
        /*  Version fuer Optimizer. */
            t2=new_typ();
            t2->flags=t;
            if(ISPOINTER(t)){
                t2->next=new_typ();
                t2->next->flags=VOID;
            }
            v=add_var(empty,t2,AUTO,0);
            new=new_IC();
            new->code=ASSIGN;
            new->typf=t;
            new->q1=*arg;
            new->q2.flags=0;
            new->q2.val.vmax=sizetab[t&NQ];
            new->z.flags=VAR;
            new->z.v=v;
            new->z.val.vmax=l2zm(0L);
            add_IC(new);
            nrl=mymalloc(sizeof(*nrl));
            nrl->next=*rl;
            nrl->reg=reg;
            nrl->v=v;
	    nrl->al=al;
            *rl=nrl;
            return of;
        }else{
        /*  Nicht-optimierende Version. */
            if(!regs[reg]&&(!reg_pair(reg,&rp)||(!regs[rp.r1]&&!regs[rp.r2]))){
                new=new_IC();
                new->code=ALLOCREG;
                new->typf=0;
                new->q1.flags=REG;
                new->q1.reg=reg;
                new->q2.flags=new->z.flags=0;
                add_IC(new);
                regs[reg]=33;regused[reg]++;
		if(reg_pair(reg,&rp)){
		  regs[rp.r1]=33;regused[rp.r1]++;
		  regs[rp.r2]=33;regused[rp.r2]++;
		}
            }else{
                if(arg->flags!=(REG|SCRATCH)||arg->reg!=reg){
		  int r=0;
		  /* register pairs */
		  if(reg_pair(reg,&rp)){
		    if(regs[reg]||regs[rp.r1]||regs[rp.r2]) 
		      r=reg;
		  }else{
		    for(r=MAXR;r>=1;r--){
		      if(regs[r]&&reg_pair(r,&rp)&&(rp.r1==reg||rp.r2==reg))
			break;
		    }
		    if(r<1) r=reg;
		  }
		  t2=clone_typ(regtype[r]);
		  v=add_var(empty,t2,AUTO,0);
		  v->flags|=USEDASADR;
		  new=new_IC();
		  new->code=MOVEFROMREG;
		  new->typf=0;
		  new->q1.flags=REG;
		  new->q1.reg=r;
		  new->q2.flags=0;
		  new->q2.val.vmax=regsize[r];
		  new->z.flags=VAR|DONTREGISTERIZE;
		  new->z.v=v;
		  new->z.val.vmax=l2zm(0L);
		  add_IC(new);
                }else
		  regs[reg]|=32;
            }
            new=new_IC();
#ifdef HAVE_REGPARMS
	    if(!radr){
	      al->pushic=new;
	    }
#else
	    al->pushic=new;
#endif
	    new->code=ASSIGN;
	    new->typf=t;
            new->q1=*arg;
            /* Testen, ob Quellregister gesichert wurde. Unschoen. */
            if((new->q1.flags&REG)){
              struct regargs_list *p;
              for(p=*rl;p;p=p->next){
		int r;
                if(p->v&&(r=abs(p->reg))){
		  if(new->q1.reg==r||(reg_pair(new->q1.reg,&rp)&&(rp.r1==r||rp.r2==r))) {new->q1.v=p->v;new->q1.val.vmax=l2zm(0L);break;}
		  /*FIXME: andersrum bei LITTLEENDIAN?? */
		  if(reg_pair(r,&rp)&&rp.r1==new->q1.reg) {new->q1.v=p->v;new->q1.val.vmax=l2zm(0L);break;}
		  if(reg_pair(r,&rp)&&rp.r2==new->q1.reg) {new->q1.v=p->v;new->q1.val.vmax=regsize[rp.r1];break;}

		}
              }
              if(p&&new->q1.v){
		if(must_convert(new->typf,regtype[new->q1.reg]->flags,0)){
		  new->code=CONVERT;
		  new->typf2=regtype[new->q1.reg]->flags;
		}
                new->q1.flags&=~REG;
                new->q1.flags|=VAR;
                /*new->q1.val.vmax=l2zm(0L);*/
              }
            }
            new->q2.flags=new->z.flags=0;
            new->q2.val.vmax=sizetab[t&NQ];
            new->z.flags=REG;
            new->z.reg=reg;
            add_IC(new);
            nrl=mymalloc(sizeof(*nrl));
            nrl->next=*rl;
            nrl->reg=reg;
            nrl->v=v;
            *rl=nrl;
            return of;
        }
    }
}

void convert(np p,int f)
/*  konvertiert das Objekt in p->o in bestimmten Typ    */
/* wenn volatile_convert gesetzt ist, wird immer IC erzeugt */
{
  struct IC *new;
  int o=p->ntyp->flags;
  if((f&NQ)==VOID) return;
  if(p->flags==CEXPR||p->flags==PCEXPR){
#ifdef HAVE_MISRA
    if((o&NU)<(f&NU))
      misra_neu(18,0,0,-1);
#endif
    eval_constn(p);
    p->ntyp->flags=f;
    insert_constn(p);
    p->o.val=p->val;
    return;
  }
  if(!volatile_convert&&((o&NU)==(f&NU)||(!must_convert(o,f,const_expr)&&(const_expr||!(optflags&2))))){
    p->ntyp->flags=f;
    if(!ISPOINTER(f)&&!ISARRAY(f)){freetyp(p->ntyp->next);p->ntyp->next=0;}
    return;
  }
  new=new_IC();
  new->q1=p->o;
  new->q2.flags=0;
  new->code=CONVERT;
  new->typf2=o;
  new->typf=f;
  if((p->o.flags&(SCRATCH|REG))!=(SCRATCH|REG)||!regok(p->o.reg,f,0)){
    get_scratch(&new->z,f,0,0);
    p->o=new->z;
    add_IC(new);
  }else{
    new->z=p->o;new->z.flags&=~DREFOBJ;
    p->o=new->z;
    add_IC(new);
  }
}

void alloc_hardreg(int r)
/*  Belegt Register r.  */
{
  struct IC *new;
  if(nocode) return;
  if(DEBUG&16) printf("allocated %s\n",regnames[r]);
  regs[r]=1;regused[r]++;
  new=new_IC();
  new->code=ALLOCREG;
  new->typf=0;
  new->q1.flags=REG;
  new->q1.reg=r;
  new->q2.flags=new->z.flags=0;
  add_IC(new);
  if(reg_pair(r,&rp)){
    regs[rp.r1]=1;regused[rp.r1]++;
    regs[rp.r2]=1;regused[rp.r2]++;
  }
}

int allocreg(int f,int mode)
/*  Fordert Register fuer Typ f an.                     */
{
  int i,r=0,prio=-1;
  if(nocode) return(1);
  for(i=1;i<=MAXR;i++){
    if(!regs[i]&&reg_prio[i]>prio&&regok(i,f,mode)&&(!reg_pair(i,&rp)||(!regs[rp.r1]&&!regs[rp.r2]))){
      r=i;
      prio=reg_prio[i];
    }
  }
  if(r){
    if(DEBUG&16) printf("alloc %s\n",regnames[r]);
    alloc_hardreg(r);
    return r;
  }
  if(DEBUG&1) printf("Couldn't allocate register for type %d\n",f);
  return 0;
}
void free_reg(int r)
/*  Gibt Register r frei                                */
/*  Eintrag eines IC fehlt noch                         */
{
    struct IC *new;
    if(!r||nocode) return;
    if(regs[r]==0)
      {printf("Register %d(%s):\n",r,regnames[r]);ierror(0);}
    if(DEBUG&16) printf("freed %s\n",regnames[r]);
    new=new_IC();
    new->code=FREEREG;
    new->typf=0;
    new->q1.flags=REG;
    new->q1.reg=r;
    new->q2.flags=new->z.flags=0;
    add_IC(new);
    regs[r]=0;
    if(reg_pair(r,&rp)){
      regs[rp.r1]=0;
      regs[rp.r2]=0;
    }
}
void gen_label(int l)
/*  Erzeugt ein Label                                   */
{
    struct IC *new;
    new=new_IC();
    new->code=LABEL;
    new->typf=l;
    new->q1.flags=new->q2.flags=new->z.flags=0;
    add_IC(new);
}
void gen_cond(struct obj *p,int m,int l1,int l2)
/*  Generiert code, der 0 oder 1 in Register schreibt. Schreibt obj nach p. */
{
    struct IC *new;
    struct obj omerk;
    new=new_IC();
    new->code=ASSIGN;
    new->typf=INT;
    new->q1.flags=KONST;
    new->q2.flags=0;
    new->q2.val.vmax=sizetab[INT];
    if(!m) vmax=l2zm(1L); else vmax=l2zm(0L);
    new->q1.val.vint=zm2zi(vmax);
    get_scratch(&new->z,INT,0,0);
    omerk=new->z;
    add_IC(new);
    new=new_IC();
    new->code=BRA;
    new->typf=l2;
    add_IC(new);
    gen_label(l1);
    new=new_IC();
    new->code=ASSIGN;
    new->typf=INT;
    new->q1.flags=KONST;
    new->q2.flags=0;
    new->q2.val.vmax=sizetab[INT];
    if(!m) vmax=l2zm(0L); else vmax=l2zm(1L);
    new->q1.val.vint=zm2zi(vmax);
    new->z=omerk;
/*    new->z.reg=r;
    new->z.flags=SCRATCH|REG;*/
    add_IC(new);
    gen_label(l2);
    *p=omerk;
}
void scratch_var(struct obj *o,int t,struct Typ *typ)
/*  liefert eine temporaere Variable                            */
/*  nicht effizient, aber wer hat schon so wenig Register...    */
{
  struct Typ *nt;
  if(!ISSCALAR(t)){
    if(!typ) ierror(0);
    nt=clone_typ(typ);
  }else{
    nt=new_typ();
    nt->flags=t;
    if(ISPOINTER(t)){
      nt->next=new_typ();
      nt->next->flags=VOID;
    }
  }
  o->flags=SCRATCH|VAR;o->reg=0;
  o->v=add_var(empty,nt,AUTO,0);
  o->val.vmax=l2zm(0L);
}
void get_scratch(struct obj *o,int t1,int t2,struct Typ *typ)
/*  liefert ein Scratchregister oder eine Scratchvariable       */
{
    if(!(optflags&2)&&(o->reg=allocreg(t1,t2))){
        o->flags=SCRATCH|REG;
    }else{
        scratch_var(o,t1,typ);
    }
}
int do_arith(np p,struct IC *new,np dest,struct obj *o)
/*  erzeugt IC new fuer einen arithmetischen Knoten und speichert das   */
/*  Resultat vom Unterknoten dest in o (fuer a op= b)               */
/*  liefert 0, wenn dest nicht gefunden                             */
{
    int f=0,mflags;
    new->code=p->flags;
    if(new->code==PMULT) new->code=MULT;
    gen_IC(p->left,0,0);
    if(dest&&p->left==dest) {*o=p->left->o;f++;}
    gen_IC(p->right,0,0);
    if(dest&&p->right==dest) {*o=p->right->o;f++;}
    if(dest){mflags=dest->o.flags;dest->o.flags&=(~SCRATCH);}

    if(ISPOINTER(p->left->ntyp->flags)&&ISPOINTER(p->right->ntyp->flags)){
    /*  Subtrahieren zweier Pointer                                 */
      int dt=PTRDIFF_T(p->left->ntyp->flags);
      if(p->flags!=SUB) ierror(0);
      new->typf=dt;
      new->typf2=p->left->ntyp->flags;
      new->code=SUBPFP;
      new->q1=p->left->o;
      new->q2=p->right->o;
      if(!dest&&(p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&regok(p->left->o.reg,dt,0)){
	new->z=p->left->o;
	new->z.flags&=~DREFOBJ;
      }else{
	if(USEQ2ASZ&&(p->right->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&regok(p->right->o.reg,dt,0)){
	  new->z=p->right->o;
	  new->z.flags&=(~DREFOBJ);
	}else{
	  get_scratch(&new->z,dt,0,0);
	}
      }
      p->o=new->z;
      add_IC(new);
      if(!zmleq(szof(p->left->ntyp->next),l2zm(1L))){
	new=new_IC();
	new->code=DIV;
	new->q1=p->o;
	new->q2.flags=KONST;
	gval.vmax=szof(p->left->ntyp->next);
	eval_const(&gval,MAXINT);
	insert_const(&new->q2.val,dt);
	new->z=p->o;
	new->typf=dt;
	add_IC(new);
      }
      if(dest) dest->o.flags=mflags;
      return f;
    }
    if((p->flags==ADD||p->flags==SUB)&&ISPOINTER(p->ntyp->flags)){
      /*  Addieren und Subtrahieren eines Integers zu einem Pointer   */
      if(p->flags==ADD){
	new->code=ADDI2P;
	if(!ISPOINTER(p->left->ntyp->flags)){
	  np tmp=p->left;
	  p->left=p->right;
	  p->right=tmp;
	}
      }else
	new->code=SUBIFP;
      new->typf=p->right->ntyp->flags;
      new->typf2=p->ntyp->flags;
      new->q1=p->left->o;
      /*  kleinere Typen als MINADDI2P erst in diesen wandeln */
      if((new->typf&NQ)<MINADDI2P){convert(p->right,MINADDI2P);new->typf=MINADDI2P;}
      /*  groessere Typen als MAXADDI2P erst in diesen wandeln */
      if((new->typf&NQ)>MAXADDI2P){convert(p->right,MAXADDI2P);new->typf=MAXADDI2P;}      
      if((p->left->o.flags&VARADR)&&(p->right->o.flags&KONST)){
	eval_const(&p->right->o.val,p->right->ntyp->flags);
	p->o=p->left->o;
	p->o.val.vmax=zmadd(vmax,p->left->o.val.vmax);
	free(new);
	return f;
      }
      new->q2=p->right->o;
      if(!dest&&(p->left->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&regok(new->q1.reg,POINTER_TYPE(p->left->ntyp->next),p->left->ntyp->next->flags)){
	new->z=p->left->o;
	new->z.flags&=(~DREFOBJ);
      }else{
	get_scratch(&new->z,POINTER_TYPE(p->left->ntyp->next),p->left->ntyp->next->flags,0);
      }
      p->o=new->z;
      add_IC(new);
      if(dest) dest->o.flags=mflags;
      return f;
    }
    convert(p->left,p->ntyp->flags);
    if(p->flags==LSHIFT||p->flags==RSHIFT){
      struct Typ *st=clone_typ(p->right->ntyp);
      /*st->flags=int_erw(st->flags);*/
      st->flags=INT;
      convert(p->right,st->flags);
      new->typf2=st->flags;
      freetyp(st);
    }else
      convert(p->right,p->ntyp->flags);
    new->q1=p->left->o;
    new->q2=p->right->o;
    new->typf=p->ntyp->flags;
    /*  Bei dest!=0, d.h. ASSIGNOP, darf q1 nicht als Ziel benuzt werden!  */
    if(!dest&&(new->q1.flags&(SCRATCH|REG))==(SCRATCH|REG)&&regok(new->q1.reg,p->ntyp->flags,0)){
      new->z=new->q1;
      new->z.flags&=~DREFOBJ;
    }else{
      if((new->q2.flags&SCRATCH)&&regok(new->q2.reg,p->ntyp->flags,0)){
	if((p->flags>=OR&&p->flags<=AND)||p->flags==ADD||p->flags==MULT||p->flags==PMULT){
	  /*  bei kommutativen Operatoren vertauschen     */
	  new->z=new->q2;
	  new->q2=new->q1;
	  new->q1=new->z;
	  new->z.flags&=~DREFOBJ;
	}else{
	  if(USEQ2ASZ){
	    new->z=new->q2;
	    new->z.flags&=~DREFOBJ;
	  }else{
	    get_scratch(&new->z,new->typf,0,0);
	  }
	}
      }else{
	get_scratch(&new->z,new->typf,0,0);
      }
    }
    p->o=new->z;
    add_IC(new);
    if(dest) dest->o.flags=mflags;
    return f;
}
void savescratch(int code,struct IC *p,int dontsave,struct obj *o)
/*  speichert Scratchregister bzw. stellt sie wieder her (je nach code  */
/*  entweder MOVEFROMREG oder MOVETOREG)                                */
{
  int i,s,e,b,ds1,ds2;struct IC *new;
  if(code==MOVETOREG){ s=1;e=MAXR+1;b=1;} else {s=MAXR;e=0;b=-1;}
  if(reg_pair(dontsave,&rp)){
    ds1=rp.r1;
    ds2=rp.r2;
  }else
    ds1=ds2=0;
  for(i=s;i!=e;i+=b){
    int mustsave=0;
    if((o->flags&(VAR|DREFOBJ))==VAR&&o->v->fi&&(o->v->fi->flags&ALL_REGS))
      mustsave=BTST(o->v->fi->regs_modified,i);
    else
      mustsave=regscratch[i];
    if(regsa[i]) mustsave=0;
    if(mustsave) simple_scratch[i]=1;
    if(regs[i]&&!(regs[i]&32)&&mustsave&&i!=dontsave&&i!=ds1&&i!=ds2&&!reg_pair(i,&rp)){
      if(!regsbuf[i]){
	struct Typ *t;
	if(code!=MOVEFROMREG) continue;
	t=clone_typ(regtype[i]);
	regsbuf[i]=add_var(empty,t,AUTO,0);
	regsbuf[i]->flags|=USEDASADR;
	regbnesting[i]=nesting;
      }
      new=new_IC();
      new->typf=new->q2.flags=0;
      new->line=0;new->file=0;
      new->code=code;
      if(code==MOVEFROMREG){
	new->q1.flags=REG;new->q1.reg=i;
	new->z.flags=VAR|DONTREGISTERIZE;new->z.v=regsbuf[i];
	new->z.val.vmax=l2zm(0L);
      }else{
	new->z.flags=REG;new->z.reg=i;
	new->q1.flags=VAR|DONTREGISTERIZE;new->q1.v=regsbuf[i];
	new->q1.val.vmax=l2zm(0L);
      }
      new->use_cnt=new->change_cnt=0;
      new->use_list=new->change_list=0;
      insert_IC(p,new);
    }
  }
}


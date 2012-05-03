/*  $VER: vbcc (statements.c) V0.8  */

#include "vbcc_cpp.h"
#include "vbc.h"

static char FILE_[]=__FILE__;

int cont_label=0;
int test_assignment(struct Typ *,np);

static int switchbreak;

#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif

#define cr()
#ifndef cr
void cr(void)
/*  tested Registerbelegung */
{
  int i;
  for(i=0;i<=MAXR;i++)
    if(regs[i]!=regsa[i]) {error(149,regnames[i]);regs[i]=regsa[i];}
}
#endif


#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif

void statement(void)
/*  bearbeitet ein statement                                    */
{
  struct token mtok;
#ifdef HAVE_MISRA
/* removed */
#endif
  cr();
  killsp();
  if(ctok->type==LBRA){
    enter_block();
    if(nesting>0) local_offset[nesting]=local_offset[nesting-1];
    compound_statement();
    leave_block();
    return;
  }
  if(ctok->type==NAME){
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
#ifndef HAVE_MISRA
    if(!strcmp("if",ctok->name)){next_token();if_statement(); return;}
    if(!strcmp("switch",ctok->name)){next_token();switch_statement(); return;}
    if(!strcmp("for",ctok->name)){next_token();for_statement(); return;}
    if(!strcmp("while",ctok->name)){next_token();while_statement(); return;}
    if(!strcmp("do",ctok->name)){next_token();do_statement(); return;}
    if(!strcmp("goto",ctok->name)){next_token();goto_statement(); return;}
    if(!strcmp("continue",ctok->name)){next_token();continue_statement(); return;}
    if(!strcmp("break",ctok->name)){next_token();break_statement(); return;}
    if(!strcmp("return",ctok->name)){next_token();return_statement(); return;}
    if(!strcmp("case",ctok->name)){next_token(); labeled_statement(); return;}
    if(!strcmp("default",ctok->name)){cpbez(buff,0);next_token();labeled_statement(); return;}
#endif
	}
  cpbez(buff,1);
  copy_token(&mtok,ctok);
  next_token();killsp();
  if(ctok->type==T_COLON){labeled_statement();return;}
  push_token(&mtok);
  free(mtok.name);
  expression_statement(); /* if there is an expression statement set misra_last_break to false */
}
void labeled_statement(void)
/*  bearbeitet labeled_statement                                */
{
  struct llist *lp;int def=0;
  nocode=0;
  if(ctok->type==T_COLON){
    next_token();
    if(!*buff){
      error(130);
      return;
    }
    if(!strcmp("default",buff)){
      def=1;
      lp=0;
    } else lp=find_label(buff);
    if(lp&&lp->flags&LABELDEFINED){
      error(131,buff);
      return;
    }
    if(!lp) lp=add_label(buff);
    lp->flags|=LABELDEFINED;
    lp->switch_count=0;
    if(def){
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
#endif
      if(switch_act==0) error(150);
      lp->flags|=LABELDEFAULT;
      lp->switch_count=switch_act;
    }else {
#ifdef HAVE_MISRA
/* removed */
#endif
		}
    gen_label(lp->label);
    afterlabel=0;
  }else{
    /*  case    */
    np tree;struct llist *lp;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
    tree=expression();
    killsp();
    if(ctok->type==T_COLON){
      next_token();
      killsp();
    } else error(70);
    if(!switch_count){
      error(132);
    } else {
      if(!tree||!type_expression(tree))
	{
	} else {
	if(tree->flags!=CEXPR||tree->sidefx){
	  error(133);
	} else {
	  if(!ISINT(tree->ntyp->flags)){
	    error(134);
	  } else {
	    lp=add_label(empty);
	    lp->flags=LABELDEFINED;
	    lp->switch_count=switch_act;
	    eval_constn(tree);
	    insert_const(&lp->val,switch_typ);
	    gen_label(lp->label);
	  }
	}
      }
    }
    if(tree) free_expression(tree);
  }
  cr();
  killsp();
  if(ctok->type!=RBRA) statement();
}
void if_statement(void)
/*  bearbeitet if_statement                                     */
{
  int ltrue,lfalse,lout,cexpr=0,cm,tm,merk_elseneed;
  np tree;struct IC *new;
  static int elseneed;
  merk_elseneed=elseneed;
  elseneed=0;
  killsp();
  if(ctok->type==LPAR) next_token(); else error(151);
  killsp();cm=nocode;
  tree=expression();
  if(!tree){
    error(135);
  }else{
#ifdef HAVE_MISRA
/* removed */
#endif
    ltrue=++label;lfalse=++label;
    if(type_expression(tree)){
      tree=makepointer(tree);
      if(!ISARITH(tree->ntyp->flags)&&!ISPOINTER(tree->ntyp->flags)){
	error(136);
      }else{
	if(tree->flags==ASSIGN&&tree->right->flags!=CALL) error(164);
	gen_IC(tree,ltrue,lfalse);
	if(tree->flags==CEXPR){
	  eval_const(&tree->val,tree->ntyp->flags&NU);
	  if(zldeqto(vldouble,d2zld(0.0))&&zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))) cexpr=2; else cexpr=1;
	}else
	  cexpr=0;
	if((tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&cexpr) free_reg(tree->o.reg);
	if(tree->o.flags&&!cexpr){
	  new=new_IC();
	  new->code=TEST;
	  new->q1=tree->o;
	  new->q2.flags=new->z.flags=0;
	  new->typf=tree->ntyp->flags;
	  add_IC(new);
	  new=new_IC();
	  new->code=BEQ;
	  new->typf=lfalse;
	  add_IC(new);
	}
	if(cexpr==2){
	  new=new_IC();
	  new->code=BRA;
	  new->typf=lfalse;
	  add_IC(new);
	}
      }
    }
    tm=tree->o.flags;
    free_expression(tree);
  }
  killsp(); if(ctok->type==RPAR) next_token(); else error(59);
  if(cexpr==2) nocode=1;
  if(!cexpr&&!tm) gen_label(ltrue);
#ifdef HAVE_MISRA
/* removed */
#endif
  statement();
  killsp();
  if(ctok->type!=NAME||strcmp("else",ctok->name)){
#ifdef HAVE_MISRA
/* removed */
#endif
    nocode=cm;
    if(cexpr!=1) gen_label(lfalse);
    elseneed=merk_elseneed;
    return;
  }
  next_token();
  lout=++label;
  if(cexpr!=2){
    new=new_IC();
    new->code=BRA;
    new->typf=lout;
    add_IC(new);
  }
  if(cexpr!=1) {nocode=cm;gen_label(lfalse);}
  if(cexpr==1) nocode=1; else nocode=cm;
  if(ctok->type!=LBRA){
	  if(ctok->type!=NAME||strcmp("if",ctok->name)) {
#ifdef HAVE_MISRA
/* removed */
#endif
	  }
    else
      elseneed=1;
  }
  statement();
  elseneed=merk_elseneed;
  nocode=cm;
  if(cexpr!=2) gen_label(lout);
  cr();
}


void switch_statement(void)
/*  bearbeitet switch_statement                                 */
{
  np tree;int merk_typ,merk_count,merk_break,num_cases;
	struct IC *merk_fic,*merk_lic,*new;struct llist *lp,*l1,*l2;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
  nocode=0;
  killsp();
  if(ctok->type==LPAR){
		next_token();killsp();
	} else error(151);
  tree=expression(); killsp();
  if(ctok->type==RPAR){
		next_token();killsp();
	} else error(59);
  merk_typ=switch_typ;merk_count=switch_act;merk_break=break_label;
  if(!tree){
    error(137);
  } else {
    if(!type_expression(tree)){
    }else{
      if(!ISINT(tree->ntyp->flags)){
	error(138);
      } else {
	int m1,m2,m3,def=0,rm,minflag;
	zmax l,ml,s;zumax ul,mul,us;
	if(tree->flags==ASSIGN&&tree->right->flags!=CALL) error(164);
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif 
	m3=break_label=++label;m1=switch_act=++switch_count;
	m2=switch_typ=tree->ntyp->flags&NU;
	gen_IC(tree,0,0);
	if((SWITCHSUBS&&!shortcut(SUB,m2))||(!SWITCHSUBS&&!shortcut(COMPARE,m2))){
	  switch_typ=m2=int_erw(m2);
	  convert(tree,m2);
	}
	if((tree->o.flags&(DREFOBJ|SCRATCH))!=SCRATCH){
	  new=new_IC();
	  new->code=ASSIGN;
	  new->q1=tree->o;
	  new->q2.flags=0;
	  new->q2.val.vmax=sizetab[m2&NQ];
	  get_scratch(&new->z,m2,0,0);
	  new->typf=m2;
	  tree->o=new->z;
	  add_IC(new);
	}
	if((tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)){
	  int r=tree->o.reg;
	  rm=regs[r];
	  regs[r]=regsa[r];
	}
	merk_fic=first_ic;merk_lic=last_ic;
	
	first_ic=last_ic=0;
	{
	  int merk_sb=switchbreak;
	  switchbreak=1;
	  statement();
	  switchbreak=merk_sb;
	}
	
	if((tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) regs[tree->o.reg]=rm;
	minflag=0;s=l2zm(0L);us=ul2zum(0UL);
	num_cases=0;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
	for(l1=first_llist;l1;l1=l1->next)
	  if(l1->switch_count==m1) num_cases++;
	for(l1=first_llist;l1;l1=l1->next){
	  if(l1->switch_count!=m1) continue;
	  if(l1->flags&LABELDEFAULT){
	    if(def) error(139);
	    def=l1->label;
	    continue;
	  }
	  lp=0;minflag&=~1;
	  for(l2=first_llist;l2;l2=l2->next){
	    if(l2->switch_count!=m1) continue;
	    if(l2->flags&LABELDEFAULT) continue;
	    eval_const(&l2->val,m2);
	    if(minflag&2){
	      if(m2&UNSIGNED){
		if(zumleq(vumax,mul)||zumeqto(vumax,mul)) continue;
	      }else{
		if(zmleq(vmax,ml)||zmeqto(vmax,ml)) continue;
	      }
	    }
	    if(minflag&1){
	      if(m2&UNSIGNED){
		if(!(minflag&4)&&zumeqto(vumax,ul)){ 
		  error(201);
		  minflag|=4;
		}
		if(zumleq(vumax,ul)){
		  lp=l2;
		  ul=vumax;
		}
	      }else{
		if(!(minflag&4)&&zmeqto(vmax,l)){ error(201);minflag|=4;}
		if(zmleq(vmax,l)){lp=l2;l=vmax;}
	      }
	    }else{
	      minflag|=1;
	      l=vmax;
	      ul=vumax;
	      lp=l2;
	    }
	  }
	  if(!lp) continue;
	  ml=l;mul=ul;minflag|=2;
	  if(SWITCHSUBS&&num_cases<JUMP_TABLE_LENGTH){
	    new=new_IC();
	    new->line=0;
	    new->file=0;
	    new->typf=m2;
	    new->code=SUB;
	    new->q1=tree->o;
	    new->z=tree->o;
	    new->q2.flags=KONST;
	    eval_const(&lp->val,m2);
	    if(m2&UNSIGNED){
	      gval.vumax=zumsub(vumax,us);
	      eval_const(&gval,UNSIGNED|MAXINT);
	    }else{
	      gval.vmax=zmsub(vmax,s);
	      eval_const(&gval,MAXINT);
	    }
	    insert_const(&new->q2.val,m2);
	    new->q1.am=new->q2.am=new->z.am=0;
	    s=l;us=ul;
	    new->prev=merk_lic;
	    if(merk_lic) merk_lic->next=new; else merk_fic=new;
	    merk_lic=new;
	    new=new_IC();
	    new->line=0;
	    new->file=0;
	    new->typf=m2;
	    new->code=TEST;
	    new->q1=tree->o;
	    new->q2.flags=new->z.flags=0;
	    new->prev=merk_lic;
	    new->q1.am=new->q2.am=new->z.am=0;
	    if(merk_lic) merk_lic->next=new; else merk_fic=new;
	    merk_lic=new;
	  }else{
	    new=new_IC();
	    new->line=0;
	    new->file=0;
	    new->code=COMPARE;
	    new->typf=m2;
	    new->q1=tree->o;
	    new->q2.flags=KONST;
	    new->q2.val=lp->val;
	    new->z.flags=0;
	    new->prev=merk_lic;
	    new->q1.am=new->q2.am=new->z.am=0;
	    if(merk_lic) merk_lic->next=new; else merk_fic=new;
	    merk_lic=new;
	  }
	  new=new_IC();
	  new->line=0;
	  new->file=0;
	  new->code=BEQ;
	  new->typf=lp->label;
	  new->q1.flags=new->q2.flags=new->z.flags=0;
	  new->prev=merk_lic;
	  new->q1.am=new->q2.am=new->z.am=0;
	  merk_lic->next=new;
	  merk_lic=new;
	}
	if((tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)){   /* free_reg(tree->o.reg); */
	  new=new_IC();
	  new->line=0;
	  new->file=0;
	  new->code=FREEREG;new->typf=0;
	  new->q2.flags=new->z.flags=0;
	  new->q1.flags=REG;
	  new->q1.reg=tree->o.reg;
	  new->prev=merk_lic;
	  new->q1.am=new->q2.am=new->z.am=0;
	  if(merk_lic) merk_lic->next=new; else merk_fic=new;
	  merk_lic=new;
	  regs[tree->o.reg]=regsa[tree->o.reg];
	}
	new=new_IC();
	new->line=0;
	new->file=0;
	new->code=BRA;
	if(def) new->typf=def; else {
#ifdef HAVE_MISRA
/* removed */
#endif
	  new->typf=m3;
	}
	new->q1.flags=new->q2.flags=new->z.flags=0;
	if(merk_lic) merk_lic->next=new; else merk_fic=new;
	new->prev=merk_lic;
	if(first_ic){
	  first_ic->prev=new;
	  new->next=first_ic;
	}else{
	  last_ic=new;
	  new->next=first_ic;
	}
	new->q1.am=new->q2.am=new->z.am=0;
	first_ic=merk_fic;
	gen_label(m3);
      }
    }
  }
  switch_typ=merk_typ;switch_act=merk_count;break_label=merk_break;
  if(tree) free_expression(tree);
  cr();
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
#endif
}
void repair_tree(np p)
/*  Bearbeitet einen Ausdruckbaum so, dass er ein zweites Mal   */
/*  mit gen_IC erzeugt werden kann.                             */
{
  if(p->left) repair_tree(p->left);
  if(p->right) repair_tree(p->right);
  if(p->flags==IDENTIFIER||p->flags==(IDENTIFIER|256))
    p->o.v=find_var(p->identifier,0);
  if(p->flags==CALL){
    struct argument_list *al=p->alist;
    while(al){
      repair_tree(al->arg);
      al=al->next;
    }
  }
}
void while_statement(void)
/*  bearbeitet while_statement                                  */
{
  np tree;int lloop,lin,lout,cm,cexpr,contm,breakm;
  struct IC *new,*mic; int line;char *file;
  killsp();
  if(ctok->type==LPAR) {next_token();killsp();} else error(151);
  tree=expression();
  cexpr=0;
  if(tree){
    if(type_expression(tree)){
#ifdef HAVE_MISRA
/* removed */
#endif
      tree=makepointer(tree);
      if(!ISARITH(tree->ntyp->flags)&&!ISPOINTER(tree->ntyp->flags)){
	error(140);
	cexpr=-1;
      }else{
	if(tree->flags==ASSIGN&&tree->right->flags!=CALL) error(164);
	if(tree->flags==CEXPR){
	  eval_const(&tree->val,tree->ntyp->flags);
	  if(zldeqto(vldouble,d2zld(0.0))&&zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))) cexpr=1; else cexpr=2;
	  if(cexpr==1) error(152);
	}
      }
    }else cexpr=-1;
  } else error(141);
  lloop=++label;lin=++label;lout=++label;cm=nocode;
  contm=cont_label;breakm=break_label;
  if(!cexpr||tree->sidefx) cont_label=lin; else cont_label=lloop;
  if(!cexpr||tree->sidefx){
    if(c_flags_val[0].l&2){ /*  bei Optimierung */
      gen_IC(tree,lloop,lout);
      if(tree->o.flags){
	new=new_IC();
	new->code=TEST;
	new->typf=tree->ntyp->flags;
	new->q1=tree->o;
	new->q2.flags=new->z.flags=0;
	add_IC(new);
	new=new_IC();
	new->code=BEQ;
	new->typf=lout;
	add_IC(new);
      }
      repair_tree(tree);
    }else{
      new=new_IC();
      new->code=BRA;
      new->typf=lin;
      new->flags|=LOOP_COND_TRUE;
      add_IC(new);
    }
  }
  if(cexpr==1){
    new=new_IC();
    new->code=BRA;
    new->typf=lout;
    add_IC(new);
  }else{
    gen_label(lloop);
    last_ic->flags|=LOOP_COND_TRUE;
  }
  line=last_ic->line;file=last_ic->file;
  cm=nocode;break_label=lout;
  if(cexpr==1) nocode=1;
  currentpri*=looppri;
  killsp();
  if(ctok->type==RPAR) {next_token();killsp();} else error(59);
  {
    int merk_sb=switchbreak;
#ifdef HAVE_MISRA
/* removed */
#endif
    switchbreak=0;
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    statement();
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    switchbreak=merk_sb;
  }
  mic=last_ic;
  nocode=cm;cont_label=contm;break_label=breakm;
  if(!cexpr||tree->sidefx) gen_label(lin);
  if(tree&&cexpr>=0){
    if(cexpr!=1||tree->sidefx){
      gen_IC(tree,lloop,lout);
      if((tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&cexpr) free_reg(tree->o.reg);
    }
    if(tree->o.flags&&!cexpr){
      new=new_IC();
      new->code=TEST;
      new->typf=tree->ntyp->flags;
      new->q1=tree->o;
      new->q2.flags=new->z.flags=0;
      add_IC(new);
      new=new_IC();
      new->code=BNE;
      new->typf=lloop;
      add_IC(new);
    }
    if(cexpr==2){
      new=new_IC();
      new->code=BRA;
      new->typf=lloop;
      add_IC(new);
    }
  }
  if(tree) free_expression(tree);
  for(mic=mic->next;mic;mic=mic->next){
    mic->line=line;mic->file=file;
  }
  gen_label(lout);
  currentpri/=looppri;
  cr();
}
void for_statement(void)
/*  bearbeitet for_statement                                    */
{
  np tree1=0,tree2=0,tree3=0;int lloop,lin,lout,cm,cexpr=0,contm,breakm,with_decl;
  struct IC *new,*mic;int line;char *file;
  killsp();
  if(ctok->type==LPAR) {next_token();killsp();} else error(59);
#ifdef HAVE_MISRA
/* removed */
#endif
  if(c99&&declaration(0)){
    with_decl=1;
    enter_block();
    if(nesting>0) local_offset[nesting]=local_offset[nesting-1];
    for_decl=1;
    var_declaration();
    for_decl=0;
  }else{
    with_decl=0;
    if(ctok->type!=SEMIC) tree1=expression();
    if(tree1){
      if(tree1->flags==POSTINC) tree1->flags=PREINC;
      if(tree1->flags==POSTDEC) tree1->flags=PREDEC;
      if(type_expression(tree1)){
	if(tree1->sidefx){
	  gen_IC(tree1,0,0);
	  if(tree1&&(tree1->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(tree1->o.reg);
	}else{
#ifdef HAVE_MISRA
/* removed */
#endif
		error(153);
	}
      }
      free_expression(tree1);
    }
    killsp();
    if(ctok->type==SEMIC) {next_token();killsp();} else error(54);
  }
  if(ctok->type!=SEMIC) {tree2=expression();killsp();} else {cexpr=2;}
  if(ctok->type==SEMIC) {next_token();killsp();} else error(54);
  if(ctok->type!=RPAR) tree3=expression();
  killsp();
  if(ctok->type==RPAR) {next_token();killsp();} else error(59);
#ifdef HAVE_MISRA
/* removed */
#endif
  if(tree3){
    if(!type_expression(tree3)){
      free_expression(tree3);
      tree3=0;
    }
  }
  if(tree2){
    if(type_expression(tree2)){
#ifdef HAVE_MISRA
/* removed */
#endif
      tree2=makepointer(tree2);
      if(!ISARITH(tree2->ntyp->flags)&&!ISPOINTER(tree2->ntyp->flags)){
	error(142);
	cexpr=-1;
      }else{
	if(tree2->flags==ASSIGN&&tree2->right->flags!=CALL) error(164);
	if(tree2->flags==CEXPR){
	  eval_const(&tree2->val,tree2->ntyp->flags);
	  if(zldeqto(vldouble,d2zld(0.0))&&zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))) cexpr=1; else cexpr=2;
	  if(cexpr==1) error(152);
	}
      }
    }else cexpr=-1;
  }
  lloop=++label;lin=++label;lout=++label;cm=nocode;
  contm=cont_label;breakm=break_label;
  cont_label=++label;break_label=lout;
  if(!cexpr||(tree2&&tree2->sidefx)){
    if(c_flags_val[0].l&2){ /*  bei Optimierung */
      gen_IC(tree2,lloop,lout);
      if(tree2->o.flags){
	new=new_IC();
	new->code=TEST;
	new->typf=tree2->ntyp->flags;
	new->q1=tree2->o;
	new->q2.flags=new->z.flags=0;
	add_IC(new);
	new=new_IC();
	new->code=BEQ;
	new->typf=lout;
	add_IC(new);
      }
      repair_tree(tree2);
    }else{
      new=new_IC();
      new->code=BRA;
      new->typf=lin;
      new->flags|=LOOP_COND_TRUE;
      add_IC(new);
    }
  }
  if(cexpr==1){
    new=new_IC();
    new->code=BRA;
    new->typf=lout;
    add_IC(new);
  }else{
    gen_label(lloop);
    last_ic->flags|=LOOP_COND_TRUE;
  }
  line=last_ic->line;file=last_ic->file;
  cm=nocode;
  if(cexpr==1) nocode=1;
  currentpri*=looppri;
  {
    int merk_sb=switchbreak;
#ifdef HAVE_MISRA
/* removed */
#endif
    switchbreak=0;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
#endif
    statement();
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    switchbreak=merk_sb;
  }
  mic=last_ic;
  nocode=cm;
  gen_label(cont_label);
  cont_label=contm;break_label=breakm;
  if(tree3){
    if(tree3->flags==POSTINC) tree3->flags=PREINC;
    if(tree3->flags==POSTDEC) tree3->flags=PREDEC;
    if(tree3->sidefx){
      gen_IC(tree3,0,0);
      if(tree3&&(tree3->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(tree3->o.reg);
    }else{
#ifdef HAVE_MISRA
/* removed */
#endif
		error(153);}
    free_expression(tree3);
  }
  if(!cexpr||(tree2&&tree2->sidefx)) gen_label(lin);
  if(tree2&&cexpr>=0){
    if(cexpr!=1||tree2->sidefx){
      gen_IC(tree2,lloop,lout);
      if((tree2->o.flags&(SCRATCH|REG))==(SCRATCH|REG)&&cexpr) free_reg(tree2->o.reg);
    }
    if(tree2->o.flags&&!cexpr){
      new=new_IC();
      new->code=TEST;
      new->typf=tree2->ntyp->flags;
      new->q1=tree2->o;
      new->q2.flags=new->z.flags=0;
      add_IC(new);
      new=new_IC();
      new->code=BNE;
      new->typf=lloop;
      add_IC(new);
    }
    if(cexpr==2){
      new=new_IC();
      new->code=BRA;
      new->typf=lloop;
      add_IC(new);
    }
  }
  if(!tree2&&cexpr==2){
    new=new_IC();
    new->code=BRA;
    new->typf=lloop;
    add_IC(new);
  }
  if(tree2) free_expression(tree2);
  for(mic=mic->next;mic;mic=mic->next){
    mic->line=line;mic->file=file;
  }
  gen_label(lout);
  currentpri/=looppri;
  cr();
  if(with_decl)
    leave_block();
}
void do_statement(void)
/*  bearbeitet do_statement                                     */
{
  np tree;int lloop,lout,contm,breakm;
  struct IC *new;
  lloop=++label;lout=++label;currentpri*=looppri;
  gen_label(lloop);
  breakm=break_label;contm=cont_label;cont_label=++label;break_label=lout;
  {
    int merk_sb=switchbreak;
#ifdef HAVE_MISRA
/* removed */
#endif
    switchbreak=0;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
#endif
    statement();

#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    switchbreak=merk_sb;
  }
  killsp();
  gen_label(cont_label);cont_label=contm;break_label=breakm;
  if(ctok->type!=NAME||strcmp("while",ctok->name)) error(154);
  next_token();killsp();
  if(ctok->type==LPAR) {next_token();killsp();} else error(151);
  tree=expression();
  if(tree){
    if(type_expression(tree)){
#ifdef HAVE_MISRA
/* removed */
#endif
      tree=makepointer(tree);
      if(ISARITH(tree->ntyp->flags)||ISPOINTER(tree->ntyp->flags)){
	if(tree->flags==ASSIGN&&tree->right->flags!=CALL) error(164);
	if(tree->flags==CEXPR){
	  eval_const(&tree->val,tree->ntyp->flags);
	  if(tree->sidefx) gen_IC(tree,0,0);
	  if(!zldeqto(vldouble,d2zld(0.0))){
	    new=new_IC();
	    new->code=BRA;
	    new->typf=lloop;
	    add_IC(new);
	  }
	}else{
	  gen_IC(tree,lloop,lout);
	  if(tree->o.flags){
	    new=new_IC();
	    new->code=TEST;
	    new->typf=tree->ntyp->flags;
	    new->q1=tree->o;
	    new->q2.flags=new->z.flags=0;
	    add_IC(new);
	    new=new_IC();
	    new->code=BNE;
	    new->typf=lloop;
	    add_IC(new);
	  }
	}
      }else error(143);
    }
    free_expression(tree);
  }
  killsp();
  if(ctok->type==RPAR) {next_token();killsp();} else error(59);
  if(ctok->type==SEMIC) {next_token();killsp();} else error(54);
  gen_label(lout);
  currentpri/=looppri;
  cr();
}
void goto_statement(void)
/*  bearbeitet goto_statement                                   */
{
  struct llist *lp;
  struct IC *new;
#ifdef HAVE_MISRA
/* removed */
#endif
  killsp();
  if(ctok->type!=NAME){
    error(144);
  }else{
    if(is_keyword(ctok->name)) error(216,ctok->name);
    lp=find_label(ctok->name);
    if(!lp){
      lp=add_label(ctok->name);
      lp->switch_count=0;
    }
    lp->flags|=LABELUSED;
    new=new_IC();
    new->typf=lp->label;
    new->code=BRA;
    new->typf=lp->label;
    add_IC(new);
    next_token();
    killsp();
    if(ctok->type==SEMIC){next_token();killsp();} else error(54);
    cr();
    goto_used=1;
  }
}
void continue_statement(void)
/*  bearbeitet continue_statement                               */
{
  struct IC *new;
#ifdef HAVE_MISRA
/* removed */
#endif
  if(cont_label==0){error(145);return;}
  if(block_vla[nesting]) freevl();
  new=new_IC();
  new->code=BRA;
  new->typf=cont_label;
  add_IC(new);
  killsp();
  if(ctok->type==SEMIC) {next_token();killsp();} else error(54);
  cr();
}
void break_statement(void)
/*  bearbeitet break_statement                                  */
{
  struct IC *new;
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
  if(break_label==0){error(146);return;}
  if(block_vla[nesting]) freevl();
  new=new_IC();
  new->code=BRA;
  new->typf=break_label;
  add_IC(new);
  killsp();
  if(ctok->type==SEMIC) {next_token();killsp();} else error(54);
  cr();
}
static void check_auto_return(np tree)
/*  Testet, ob Knoten Adresse einer automatischen Variable ist. */
{
  if((tree->flags==ADDRESS||tree->flags==ADDRESSS||tree->flags==ADDRESSA)&&tree->left->flags==IDENTIFIER){
    struct Var *v;
    if(v=find_var(tree->left->identifier,0)){
      if(v->storage_class==AUTO) error(224);
    }
  }
}
extern int has_return;
#ifdef HAVE_MISRA
/* removed */
#endif
void return_statement(void)
/*  bearbeitet return_statement                                 */
/*  SETRETURN hat Groesse in q2.reg und z.reg==ffreturn(rtyp)    */
{
  np tree;
  struct IC *new;
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
  has_return=1;
  killsp();
  if(ctok->type!=SEMIC){
    if(tree=expression()){
      if(!return_typ){
	if(type_expression(tree)){
	  tree=makepointer(tree);
	  if((tree->ntyp->flags&NQ)!=VOID)
	    error(155);
	  else{
	    error(225);
#ifdef HAVE_MISRA
/* removed */
#endif
	  }
	  gen_IC(tree,0,0);
	  if((tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(tree->o.reg);
	}
      }else{
	if(type_expression(tree)){
	  tree=makepointer(tree);
	  if(tree->flags==ADD||(tree->flags==SUB&&ISPOINTER(tree->ntyp->flags))){
	    check_auto_return(tree->left);
	    check_auto_return(tree->right);
	  }else{
	    check_auto_return(tree);
	  }
	  if(!test_assignment(return_typ,tree)){free_expression(tree);return;}
	  gen_IC(tree,0,0);
	  convert(tree,return_typ->flags);
#ifdef OLDPARMS   /*  alte CALL/RETURN-Methode    */
	  new=new_IC();
	  new->code=ASSIGN;
	  new->typf=return_typ->flags&NU;
	  new->q1=tree->o;
	  new->q2.flags=0;
	  new->q2.val.vmax=szof(return_typ);
	  if(ffreturn(return_typ)){
	    new->z.flags=SCRATCH|REG;
	    new->z.reg=ffreturn(return_typ);
	    if(!regs[new->z.reg]){
	      struct IC *alloc=new_IC();
	      alloc->code=ALLOCREG;
	      alloc->q1.flags=REG;
	      alloc->q2.flags=alloc->z.flags=0;
	      alloc->q1.reg=new->z.reg;
	      regs[new->z.reg]=1;
	      add_IC(alloc);
	    }
	  }else{
	    new->z.reg=0;
	    new->z.v=return_var;
	    new->z.flags=SCRATCH|VAR;
	    new->z.val.vmax=l2zm(0L);
	  }
	  add_IC(new);
	  /*  das hier ist nicht sehr schoen, aber wie sonst? */
	  if((new->z.flags&(SCRATCH|REG))==(SCRATCH|REG)&&regs[new->z.reg]) free_reg(new->z.reg);
#else
	  new=new_IC();
	  if(return_var){ /*  Returnwert ueber Zeiger */
	    new->code=ASSIGN;
	    new->z.flags=VAR|DREFOBJ;
	    new->z.dtyp=POINTER_TYPE(return_typ);
	    new->z.val.vmax=l2zm(0L);
	    new->z.v=return_var;
	  }else{
	    new->code=SETRETURN;
	    new->z.reg=ffreturn(return_typ);
	    new->z.flags=0;
	  }
	  new->typf=return_typ->flags;
	  /*new->typf=tree->ntyp->flags;*/
	  new->q1=tree->o;
	  new->q2.flags=0;
	  new->q2.val.vmax=szof(return_typ);
	  add_IC(new);
#endif
	}
      }
      free_expression(tree);
      killsp();
      if(ctok->type==SEMIC) {next_token();killsp();} else error(54);
    }else{
      if(return_typ) error(156);
    }
  }else{ 
    next_token(); 
    if(return_typ) error(156);
  }
  new=new_IC();
  new->code=BRA;
  new->typf=return_label;
  add_IC(new);
  cr();
#ifdef HAVE_MISRA
/* removed */
#endif
}

void expression_statement(void)
/*  bearbeitet expression_statement                             */
{
  np tree;
  killsp();
  if(ctok->type==SEMIC){
    int oline=ctok->line;
    next_token();
#ifdef HAVE_MISRA
/* removed */
#endif
    return;
  }
  if(tree=expression()){
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    if(tree->flags==POSTINC) tree->flags=PREINC;
    if(tree->flags==POSTDEC) tree->flags=PREDEC;
    if(type_expression(tree)){
      if(DEBUG&2){
	pre(stdout,tree);
	printf("\n");
      }
      if(tree->sidefx){
	gen_IC(tree,0,0);
	if((tree->o.flags&(SCRATCH|REG))==REG) ierror(0);
	if(tree&&(tree->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(tree->o.reg);
      }else{
#ifdef HAVE_MISRA
/* removed */
#endif
	error(153);
	if(DEBUG&2) prd(stdout,tree->ntyp);
      }
    }
    free_expression(tree);
  }
  killsp();
  if(ctok->type==SEMIC) next_token(); else error(54);
  cr();
}
void compound_statement(void)
/*  bearbeitet compound_statement (block)                       */
{
  killsp();
  if(ctok->type==LBRA) next_token(); else error(157);
  if(c99||ecpp){
    killsp();
    while(ctok->type!=RBRA){
      if(declaration(0))
	var_declaration();
      else
	statement();
      killsp();
    }
  }else{
    killsp();
    while(declaration(0)){
      var_declaration();
      killsp();
    }
    while(ctok->type!=RBRA){
      statement();
      killsp();
    }
  }
  next_token();/*killsp();*/
}
struct llist *add_label(char *identifier)
/*  Fuegt label in Liste                                        */
{
  struct llist *new;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
#endif
  new=mymalloc(LSIZE);
  new->next=0;new->label=++label;new->flags=0;
  new->identifier=add_identifier(identifier,strlen(identifier));
  if(first_llist==0){
    first_llist=last_llist=new;
  }else{
    last_llist->next=new;
    last_llist=new;
  }
  return last_llist; /* return(new) sollte aequiv. sein */
}
struct llist *find_label(char *identifier)
/*  Sucht Label, gibt Zeiger auf llist oder 0 bei Fehler zurueck    */
{
  struct llist *p;
  p=first_llist;
  while(p){
    if(!strcmp(p->identifier,identifier)) return p;
    p=p->next;
  }
  return 0;
}
void free_llist(struct llist *p)
/*  Gibt llist frei                                             */

{
  struct llist *merk;
  while(p){
    merk=p->next;
    if(!(p->flags&LABELDEFINED)) error(147,p->identifier);
    if(!(p->flags&LABELUSED)&&!p->switch_count) error(148,p->identifier);
    free(p);
    p=merk;
  }
}

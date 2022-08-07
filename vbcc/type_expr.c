/*  $VER: vbcc (type_expr.c) $Revision: 1.58 $   */

#include "vbc.h"

static char FILE_[]=__FILE__;

#define CONSTADDR 256

int alg_opt(np,type *);
int test_assignment(type *,np);
int type_expression2(np,type *);
void make_cexpr(np);

int dontopt;
int no_cast_free;

#ifdef HAVE_ECPP
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





void insert_constn(np p)
/*  Spezialfall fuer np */
{
    if(!p||!p->ntyp) ierror(0);
    insert_const(&p->val,p->ntyp->flags);
}
int const_typ(type *p)
/*  Testet, ob Typ konstant ist oder konstante Elemente enthaelt    */
{
    int i;struct_declaration *sd;
    if(p->flags&CONST) return(1);
    if(ISSTRUCT(p->flags)||ISUNION(p->flags))
        for(i=0;i<p->exact->count;i++)
            if(const_typ((*p->exact->sl)[i].styp)) return(1);
    return 0;
}
type *arith_typ(type *a,type *b)
/*  Erzeugt Typ fuer arithmetische Umwandlung von zwei Operanden    */
{
  int ta,tb,va,vol;type *new;
    new=new_typ();
    ta=a->flags&NU;tb=b->flags&NU;
    vol=(a->flags&VOLATILE)|(b->flags&VOLATILE);
    if(ta==LDOUBLE||tb==LDOUBLE){new->flags=LDOUBLE|vol;return new;}
    if(ta==DOUBLE||tb==DOUBLE){new->flags=DOUBLE|vol;return new;}
    if(ta==FLOAT||tb==FLOAT){new->flags=FLOAT|vol;return new;}
    ta=int_erw(ta);tb=int_erw(tb);
    if(ta==(UNSIGNED|LLONG)||tb==(UNSIGNED|LLONG)){new->flags=UNSIGNED|LLONG|vol;return new;}
    if(ta==LLONG||tb==LLONG){new->flags=LLONG|vol;return new;}
    if(ta==(UNSIGNED|LONG)||tb==(UNSIGNED|LONG)){new->flags=UNSIGNED|LONG|vol;return new;}
    if((ta==LONG&&tb==(UNSIGNED|INT))||(ta==(UNSIGNED|INT)&&tb==LONG)){
        if(zumleq(t_max(UNSIGNED|INT),t_max(LONG))) new->flags=LONG|vol; else new->flags=UNSIGNED|LONG|vol;
        return new;
    }
    if(ta==LONG||tb==LONG){new->flags=LONG|vol;return new;}
    if(ta==(UNSIGNED|INT)||tb==(UNSIGNED|INT)){new->flags=UNSIGNED|INT|vol;return new;}
    new->flags=INT|vol;
    return new;
}
int int_erw(int t)
/*  Fuehrt Integer_Erweiterung eines Typen durch                */
{
    if((t&NQ)>=INT) return t;
    if(t&UNSIGNED){
      if((t&NQ)<=CHAR&&zumleq(t_max(UNSIGNED|CHAR),t_max(INT))) return INT;
      if((t&NQ)<=SHORT&&zumleq(t_max(UNSIGNED|SHORT),t_max(INT))) return INT;
      return UNSIGNED|INT;
    }
    return INT;
}
#if HAVE_AOS4
static int aos4err;
int aos4_attr(type *p,char *s)
{
  if(p->attr&&strstr(p->attr,s))
    return 1;
  if(p->next)
    return aos4_attr(p->next,s);
  else
    return 0;
}
static np aos4_clone_tree(np p)
{
  np new;
  if(!p) return 0;
  new=new_node();
  *new=*p;
  new->ntyp=clone_typ(p->ntyp);
  new->left=aos4_clone_tree(p->left);
  new->right=aos4_clone_tree(p->right);
  new->alist=0;new->cl=0;new->dsize=0;
  if(p->flags==CALL/*||p->cl||p->dsize*/) aos4err=1;
  return new;
}
#endif
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
#endif
int type_expression(np p,type *ttyp)
/*  Art Frontend fuer type_expression2(). Setzt dontopt auf 0   */
{
		int ret_val;
    dontopt=0;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
#endif
   ret_val = type_expression2(p,ttyp);
#ifdef HAVE_MISRA
/* removed */
#endif
		return ret_val;

}

static void fix_addr_ptype(np op)
{
  np p=op;
  while(p->flags==FIRSTELEMENT) p=p->left;
  if(p->left->flags==CONTENT){
    int pt=p->left->left->ntyp->flags;
    if(!ISPOINTER(pt)) ierror(0);
    if(!ISPOINTER(op->ntyp->flags)) ierror(0);
    op->ntyp->flags=pt;
  }
}

static int ptype(np op)
{
#ifdef POINTER_VARADR
  Var *v=0;
  np p=op;

  while(p->flags==FIRSTELEMENT/*||p->flags==CONTENT*/) p=p->left;
  
  if(p->flags==IDENTIFIER)
    v=find_var(p->identifier,0);
  if(v) 
    return POINTER_VARADR(v);
  else
    return POINTER_TYPE(op->ntyp);
#else
  return POINTER_TYPE(op->ntyp);
#endif
}

static int nullpointer(np p)
{
  if(p->flags!=CEXPR) return 0;
  eval_constn(p); /* TODO: do we need to use other measure to verify null pointer? */
  if(zmeqto(Z0,vmax)) return 1;
  return 0;
}

int type_expression2(np p,type *ttyp)
/*  Erzeugt Typ-Strukturen fuer jeden Knoten des Baumes und     */
/*  liefert eins zurueck, wenn der Baum ok ist, sonst 0         */
/*  Die Berechnung von Konstanten und andere Vereinfachungen    */
/*  sollten vielleicht in eigene Funktion kommen                */
{
  int ok,f=p->flags,mopt=dontopt,ttf;
  type *shorttyp;
  static int assignop;
  int aoflag;
#if HAVE_AOS4
  np thisp=0;
#endif
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
/* removed */
#endif
  if(!p){ierror(0);return(1);}
  if(ttyp) ttf=ttyp->flags&NQ;
  /*    if(p->ntyp) printf("Warnung: ntyp!=0\n");*/
  p->lvalue=0;
  p->sidefx=0;
  ok=1;
  if(!ecpp&&f==CALL&&p->left->flags==IDENTIFIER&&!find_var(p->left->identifier,0)){
    /*  implizite Deklaration bei Aufruf einer Funktion     */
    struct_declaration *sd;type *t;Var *v;
#ifdef HAVE_MISRA
/* removed */
#endif
    error(161,p->left->identifier);
    if(v=find_ext_var(p->left->identifier)){
      if(!ISFUNC(v->vtyp->flags)||v->vtyp->next->flags!=INT){
	error(68,p->left->identifier);
#ifdef HAVE_MISRA
/* removed */
#endif
      }
      v->flags&=~NOTINTU;
    }else{	
      sd=mymalloc(sizeof(*sd));
      sd->count=0;
      t=new_typ();
      t->flags=FUNKT;
      t->exact=add_sd(sd,FUNKT);
      t->next=new_typ();
      t->next->flags=INT;
      add_var(p->left->identifier,t,EXTERN,0);
    }
  }
#if HAVE_AOS4
  if(f==CALL&&p->left->flags==DSTRUCT){
    thisp=new_node();
    thisp->flags=ADDRESS;
    aos4err=0;
    thisp->left=aos4_clone_tree(p->left->left);
    thisp->right=0;
    thisp->alist=0;thisp->cl=0;thisp->dsize=0;thisp->ntyp=0;
  }
#endif
#if HAVE_ECPP
  if(ecpp&&f==CALL){
    argument_list *al=p->alist;
    if(al/*&&!al->arg->ntyp*/){
      while(al){
        if(!al->arg) ierror(0);
        if(!type_expression2(al->arg,0)) return 0;
        al->arg=makepointer(al->arg);
        if(type_uncomplete(al->arg->ntyp)) error(39);
        al=al->next;
      }
    }
  }
  if(ecpp&&f==CALL){
    ecpp_merk_al=ecpp_al;
    ecpp_al=p->alist;
  }
#endif
  dontopt=0;
  if(f==ADDRESS&&p->left->flags==IDENTIFIER) {p->left->flags|=CONSTADDR;/*puts("&const");*/}
  if(ttyp&&(f==OR||f==AND||f==XOR||f==ADD||f==SUB||f==MULT||f==PMULT||f==DIV||f==MOD||f==KOMPLEMENT||f==MINUS)&&!ISPOINTER(ttyp->flags)&&shortcut(f==PMULT?MULT:f,ttyp->flags&NU))
    shorttyp=ttyp;
  else
    shorttyp=0;
  if(assignop){
    aoflag=1;
    assignop=0;
  }else
    aoflag=0;
  if(p->left&&p->flags!=ASSIGNOP){
    struct_declaration *sd;
    /*  bei ASSIGNOP wird der linke Zweig durch den Link bewertet  */
    if(!p->left) ierror(0);
    if(p->flags==CAST)
      ok&=type_expression2(p->left,p->ntyp);
    else
      ok&=type_expression2(p->left,shorttyp);
    if(p->left) p->sidefx|=p->left->sidefx;
    if(!ok) return 0;
  }
  if(aoflag){
    if(!p->left||!p->right) ierror(0);
    shorttyp=p->left->ntyp;
    ttyp=shorttyp;
    ttf=ttyp->flags&NQ;
  }
  if(p->right&&p->right->flags!=MEMBER){
    struct_declaration *sd;
    if(p->flags==ASSIGNOP){
      dontopt=1;
      assignop=1;
    }else
      dontopt=0;
    if(p->flags==ASSIGN)
      ok&=type_expression2(p->right,p->left->ntyp);
    else
      ok&=type_expression2(p->right,shorttyp);
    p->sidefx|=p->right->sidefx;
    if(!ok) return 0;
  }
#if HAVE_AOS4
  if(thisp){
    if(aos4_attr(p->left->ntyp,"libcall")){
      argument_list *n=mymalloc(sizeof(*n));
      n->arg=thisp;
      n->next=p->alist;
      n->pushic=0;
      p->alist=n;
      if(aos4err) {pre(stdout,thisp);ierror(0);}
    }else{
      free_expression(thisp);
    }
  }
#endif
#if HAVE_ECPP
  if(ecpp&&f==CALL){
    ecpp_al=ecpp_merk_al;
  }
#endif
/*    printf("bearbeite %s\n",ename[p->flags]);*/
/*  Erzeugung von Zeigern aus Arrays                            */
/*  Hier muss noch einiges genauer werden (wie gehoert das?)    */
  if(p->left&&f!=PCEXPR&&(ISARRAY(p->left->ntyp->flags)||ISFUNC(p->left->ntyp->flags))){
    if(f!=ADDRESS&&f!=ADDRESSA&&f!=ADDRESSS&&f!=FIRSTELEMENT&&f!=DSTRUCT&&(f<PREINC||f>POSTDEC)&&(f<ASSIGN||f>ASSIGNOP)){
      np new=new_node();
      if((p->left->ntyp->flags&NQ)==ARRAY) new->flags=ADDRESSA;
      else new->flags=ADDRESS;
      new->ntyp=0;
      new->left=p->left;
      new->right=0;new->lvalue=0;new->sidefx=0; /* sind sidefx immer 0? */
      p->left=new;
      ok&=type_expression2(p->left,0);
    }
  }
  if(p->right&&f!=PCEXPR&&f!=FIRSTELEMENT&&f!=DSTRUCT&&f!=ADDRESSS&&p->right->ntyp&&(ISARRAY(p->right->ntyp->flags)||ISFUNC(p->right->ntyp->flags))){
    np new=new_node();
    if(ISARRAY(p->right->ntyp->flags)) new->flags=ADDRESSA;
    else new->flags=ADDRESS;
    new->ntyp=0;
    new->left=p->right;
    new->right=0;new->lvalue=0;new->sidefx=0; /* sind sidefx immer 0? */
    p->right=new;
    ok&=type_expression2(p->right,0);
  }

  if(f==IDENTIFIER||f==(IDENTIFIER|CONSTADDR)){
    int ff;Var *v;
#ifdef HAVE_ECPP
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
/* removed */
/* removed */
/* removed */
#endif
    if(!ecpp){
    if(p->identifier==empty)
      /* variable sizeof-expression */
      v=p->dsize;
    else
      v=find_var(p->identifier,0);
    }
    if(v==0){error(82,p->identifier);return(0);}
    if(disallow_statics&&v->storage_class==STATIC&&v->nesting==0&&*v->identifier){
      error(302,v->identifier);
      return 0;
    }
    ff=v->vtyp->flags&NQ;
    if(ISARITH(ff)||ISPOINTER(ff)||ISSTRUCT(ff)||ISUNION(ff)||ISVECTOR(ff)) p->lvalue=1;
    p->ntyp=clone_typ(v->vtyp);
    /*  arithmetischen const Typ als Konstante behandeln, das muss noch
	deutlich anders werden, bevor man es wirklich so machen kann
        if((p->ntyp->flags&CONST)&&ISARITH(p->ntyp->flags)&&v->clist&&!(f&CONSTADDR)){
	p->flags=CEXPR;
	p->val=v->clist->val;
	v->flags|=USEDASSOURCE;
        }*/
    p->flags&=~CONSTADDR;
    if((p->ntyp->flags&NQ)==ENUM){
      /*  enumerations auch als Konstante (int) behandeln */
      p->flags=CEXPR;
      if(!v->clist) ierror(0);
      p->val=v->clist->val;
      p->ntyp->flags=CONST|INT;
    }
    p->o.v=v;
	if (p->ntyp->flags&VOLATILE) p->sidefx=1; /* Touching a volatile may have side effects */
    return 1;
  }

  if(f==CEXPR||f==PCEXPR||f==STRING) return 1;

  if(f==REINTERPRET){
    /* todo: add checks */
    return 1;
  }

  if(f==BITFIELD) return 1;

  if(f==LITERAL){
    p->lvalue=1;
    return 1;
  }

  if(f==KOMMA){
    if(const_expr){error(46);return 0;}
    p->ntyp=clone_typ(p->right->ntyp);
    if(f==CEXPR) p->val=p->right->val;
    return ok;
  }
  if(f==ASSIGN||f==ASSIGNOP){
    if(!p) ierror(0);
    if(!p->left) ierror(0);
    if(p->left->lvalue==0) {error(86);/*prd(p->left->ntyp);*/return 0;}
    if(const_typ(p->left->ntyp)) {error(87);return 0;}
    if(type_uncomplete(p->left->ntyp)) {error(88);return 0;}
    if(type_uncomplete(p->right->ntyp)) {error(88);return 0;}
    p->ntyp=clone_typ(p->left->ntyp);
    p->sidefx=1;
    return(test_assignment(p->left->ntyp,p->right));
  }
  if(f==LOR||f==LAND){
    int a1=-1,a2=-1,m;
    if(ISVECTOR(p->left->ntyp->flags)){
      if(ISVECTOR(p->right->ntyp->flags)){
	if((p->left->ntyp->flags&NU)!=(p->right->ntyp->flags&NU)){error(89);return 0;}
      }else{
	if(!ISINT(p->right->ntyp->flags)){error(89);return 0;}
      }
      p->ntyp=new_typ();
      p->ntyp->flags=p->left->ntyp->flags&NQ;
      return ok;
    }
    if(ISVECTOR(p->right->ntyp->flags)){
      if(!ISINT(p->left->ntyp->flags)){error(89);return 0;}
      p->ntyp->flags=p->right->ntyp->flags&NQ;
      return ok;
    }
    if(f==LAND) m=1; else m=0;
#ifdef HAVE_MISRA
/* removed */
#endif
    p->ntyp=new_typ();
    p->ntyp->flags=INT;
    if(!ISARITH(p->left->ntyp->flags)&&!ISPOINTER(p->left->ntyp->flags))
      {error(89);ok=0;}
    if(!ISARITH(p->right->ntyp->flags)&&!ISPOINTER(p->right->ntyp->flags))
      {error(89);ok=0;}
    if(p->left->flags==CEXPR){
      eval_constn(p->left);
      if(!zldeqto(vldouble,d2zld(0.0))||!zumeqto(vumax,ul2zum(0UL))||!zmeqto(vmax,l2zm(0L))) a1=1; else a1=0;
    }
    if(p->right->flags==CEXPR){
      eval_constn(p->right);
      if(!zldeqto(vldouble,d2zld(0.0))||!zumeqto(vumax,ul2zum(0UL))||!zmeqto(vmax,l2zm(0L))) a2=1; else a2=0;
    }
    if(a1==1-m||a2==1-m||(a1==m&&a2==m)){
      p->flags=CEXPR;p->sidefx=0;
      if(!p->left->sidefx) {free_expression(p->left);p->left=0;} else p->sidefx=1;
      if(!p->right->sidefx||a1==1-m) {free_expression(p->right);p->right=0;} else p->sidefx=0;
      if(a1==1-m||a2==1-m) {p->val.vint=zm2zi(l2zm((long)(1-m)));}
      else {p->val.vint=zm2zi(l2zm((long)m));}
    }
    return ok;
  }
  if(f==OR||f==AND||f==XOR){
    if(ISVECTOR(p->left->ntyp->flags)){
      if(!ISINT(VECTYPE(p->left->ntyp->flags))){error(90);return 0;}
      if(ISVECTOR(p->right->ntyp->flags)){
	if((p->left->ntyp->flags&NU)!=(p->right->ntyp->flags&NU)){error(98);return 0;}
      }else{
	if(!ISINT(p->right->ntyp->flags)){error(90);return 0;}
      }
      p->ntyp=clone_typ(p->left->ntyp);
      return ok;
    }
    if(ISVECTOR(p->right->ntyp->flags)){
      if(!ISINT(VECTYPE(p->right->ntyp->flags))){error(90);return 0;}
      if(!ISINT(p->left->ntyp->flags)){error(90);return 0;}
      p->ntyp=clone_typ(p->right->ntyp);
      return ok;
    }
    if(!ISINT(p->left->ntyp->flags)){error(90);return 0;}
    if(!ISINT(p->right->ntyp->flags)){error(90);return 0;}
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    if(ttyp&&(ttf<=INT||ttf<(p->left->ntyp->flags&NQ)||ttf<(p->right->ntyp->flags&NQ))&&shortcut(f,ttyp->flags&NU)&&(!must_convert(p->left->ntyp->flags,ttyp->flags,0)||!must_convert(p->right->ntyp->flags,ttyp->flags,0)))
      p->ntyp=clone_typ(ttyp);
    else
      p->ntyp=arith_typ(p->left->ntyp,p->right->ntyp);
    if(!mopt){
      if(!alg_opt(p,ttyp)) ierror(0);
    }
    return ok;
  }
  if(f==LESS||f==LESSEQ||f==GREATER||f==GREATEREQ||f==EQUAL||f==INEQUAL){
    /*  hier noch einige Abfragen fuer sichere Entscheidungen einbauen  */
    /*  z.B. unigned/signed-Vergleiche etc.                             */
    /*  die val.vint=0/1-Zuweisungen muessen noch an zint angepasst     */
    /*  werden                                                          */
    zmax s1,s2;zumax u1,u2;zldouble d1,d2;int c=0;
    type *t;
    if(ISVECTOR(p->left->ntyp->flags)){
      if(ISVECTOR(p->right->ntyp->flags)){
	if((p->left->ntyp->flags&NU)!=(p->right->ntyp->flags&NU)){error(89);return 0;}
      }
      p->ntyp=new_typ();
      if(ISFLOAT(VECTYPE(p->left->ntyp->flags)))
	p->ntyp->flags=mkvec(INT,VECDIM(p->left->ntyp->flags));
      else
	p->ntyp->flags=p->left->ntyp->flags&NQ;
      return ok;
    }
    if(ISVECTOR(p->right->ntyp->flags)){
      p->ntyp=new_typ();
      if(ISFLOAT(VECTYPE(p->right->ntyp->flags)))
	p->ntyp->flags=mkvec(INT,VECDIM(p->left->ntyp->flags));
      else
	p->ntyp->flags=p->right->ntyp->flags&NQ;
      return ok;
    }
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
#endif
    if(!ISARITH(p->left->ntyp->flags)||!ISARITH(p->right->ntyp->flags)){
      if(!ISPOINTER(p->left->ntyp->flags)||!ISPOINTER(p->right->ntyp->flags)){
	if(f!=EQUAL&&f!=INEQUAL){
	  error(92);return 0;
	}else{
	  if((!ISPOINTER(p->left->ntyp->flags)||p->right->flags!=CEXPR)&&
	     (!ISPOINTER(p->right->ntyp->flags)||p->left->flags!=CEXPR)){
	    error(93);return 0;
	  }else{
	    if(p->left->flags==CEXPR) eval_constn(p->left);
	    else                     eval_constn(p->right);
	    if(!zldeqto(vldouble,d2zld(0.0))||!zmeqto(vmax,l2zm(0L))||!zumeqto(vumax,ul2zum(0UL)))
	      {error(40);return 0;}
	  }
	}
      }else{
	if(compatible_types(p->left->ntyp->next,p->right->ntyp->next,NQ)){
	}else{
	  if(f!=EQUAL&&f!=INEQUAL) error(41);
	  if((p->left->ntyp->next->flags&NQ)!=VOID&&(p->right->ntyp->next->flags&NQ)!=VOID)
	    {error(41);}
	}
      }
    }
    if(p->left->flags==CEXPR){
      eval_constn(p->left);
      d1=vldouble;u1=vumax;s1=vmax;c|=1;
      if((p->right->ntyp->flags&UNSIGNED)&&!(p->left->ntyp->flags&UNSIGNED)){
	if(zldleq(d1,d2zld(0.0))&&zmleq(s1,l2zm(0L))){
	  if(!zldeqto(d1,d2zld(0.0))||!zmeqto(s1,l2zm(0L))){
	    if(zumleq(tu_max[p->right->ntyp->flags&NQ],t_max[p->left->ntyp->flags&NQ]))
	      error(165);
	  }else{
	    if(f==GREATER||f==LESSEQ) error(165);
	  }
	}
      }
    }
    if(p->right->flags==CEXPR){
      eval_constn(p->right);
      d2=vldouble;u2=vumax;s2=vmax;c|=2;
      if((p->left->ntyp->flags&UNSIGNED)&&!(p->right->ntyp->flags&UNSIGNED)){
	if(zldleq(d2,d2zld(0.0))&&zmleq(s2,l2zm(0L))){
	  if(!zldeqto(d2,d2zld(0.0))||!zmeqto(s2,l2zm(0L))){
	    if(zumleq(tu_max[p->left->ntyp->flags&NQ],t_max[p->right->ntyp->flags&NQ]))
	      error(165);
	  }else{
	    if(f==LESS||f==GREATEREQ) error(165);
	  }
	}
      }
    }
    p->ntyp=new_typ();
    p->ntyp->flags=INT;
    if(c==3){
      p->flags=CEXPR;
      t=arith_typ(p->left->ntyp,p->right->ntyp);
      if(!p->left->sidefx) {free_expression(p->left);p->left=0;}
      if(!p->right->sidefx) {free_expression(p->right);p->right=0;}
      if(ISFLOAT(t->flags)){
	if(f==EQUAL) p->val.vint=zm2zi(l2zm((long)zldeqto(d1,d2)));
	if(f==INEQUAL) p->val.vint=zm2zi(l2zm((long)!zldeqto(d1,d2)));
	if(f==LESSEQ) p->val.vint=zm2zi(l2zm((long)zldleq(d1,d2)));
	if(f==GREATER) p->val.vint=zm2zi(l2zm((long)!zldleq(d1,d2)));
	if(f==LESS){
	  if(zldleq(d1,d2)&&!zldeqto(d1,d2))
	    p->val.vint=zm2zi(l2zm(1L));
	  else
	    p->val.vint=zm2zi(l2zm(0L));
	}
	if(f==GREATEREQ){
	  if(!zldleq(d1,d2)||zldeqto(d1,d2))
	    p->val.vint=zm2zi(l2zm(1L));
	  else
	    p->val.vint=zm2zi(l2zm(0L));
	}
      }else{
	if(t->flags&UNSIGNED){
	  if(f==EQUAL) p->val.vint=zm2zi(l2zm((long)zumeqto(u1,u2)));
	  if(f==INEQUAL) p->val.vint=zm2zi(l2zm((long)!zumeqto(u1,u2)));
	  if(f==LESSEQ) p->val.vint=zm2zi(l2zm((long)zumleq(u1,u2)));
	  if(f==GREATER) p->val.vint=zm2zi(l2zm((long)!zumleq(u1,u2)));
	  if(f==LESS){
	    if(zumleq(u1,u2)&&!zumeqto(u1,u2))
	      p->val.vint=zm2zi(l2zm(1L));
	    else
	      p->val.vint=zm2zi(l2zm(0L));
	  }
	  if(f==GREATEREQ){
	    if(!zumleq(u1,u2)||zumeqto(u1,u2))
	      p->val.vint=zm2zi(l2zm(1L));
	    else 
	      p->val.vint=zm2zi(l2zm(0L));
	  }
	}else{
	  if(f==EQUAL) p->val.vint=zm2zi(l2zm((long)zmeqto(s1,s2)));
	  if(f==INEQUAL) p->val.vint=zm2zi(l2zm((long)!zmeqto(s1,s2)));
	  if(f==LESSEQ) p->val.vint=zm2zi(l2zm((long)zmleq(s1,s2)));
	  if(f==GREATER) p->val.vint=zm2zi(l2zm((long)!zmleq(s1,s2)));
	  if(f==LESS){
	    if(zmleq(s1,s2)&&!zmeqto(s1,s2))
	      p->val.vint=zm2zi(l2zm(1L));
	    else 
	      p->val.vint=zm2zi(l2zm(0L));
	  }
	  if(f==GREATEREQ){
	    if(!zmleq(s1,s2)||zmeqto(s1,s2))
	      p->val.vint=zm2zi(l2zm(1L));
	    else 
	      p->val.vint=zm2zi(l2zm(0L));
	  }
	}
      }
      freetyp(t);
    }
    return ok;
  }
  if(f==ADD||f==SUB||f==MULT||f==DIV||f==MOD||f==LSHIFT||f==RSHIFT||f==PMULT){
    if(ISVECTOR(p->left->ntyp->flags)){
      if((f==MOD||f==LSHIFT||f==RSHIFT)&&ISFLOAT(VECTYPE(p->left->ntyp->flags))){
	error(98);
	return 0;
      }
      if(ISARITH(p->right->ntyp->flags)){
	p->ntyp=clone_typ(p->left->ntyp);
	return ok;
      }
      if((p->left->ntyp->flags&NU)==(p->right->ntyp->flags&NU)){
	p->ntyp=clone_typ(p->left->ntyp);
	return ok;
      }
      error(98);
      return 0;
    }
    if(ISVECTOR(p->right->ntyp->flags)){
      if((f==MOD||f==LSHIFT||f==RSHIFT)&&ISFLOAT(VECTYPE(p->right->ntyp->flags))){
	error(98);
	return 0;
      }
      if(ISARITH(p->left->ntyp->flags)){
	p->ntyp=clone_typ(p->right->ntyp);
	return ok;
      }
      error(98);
      return 0;
    }
    if(!ISARITH(p->left->ntyp->flags)||!ISARITH(p->right->ntyp->flags)){
      np new;zmax sz; int typf=0;
#ifdef MAXADDI2P
      static type pmt={MAXADDI2P};
#endif
      if(f!=ADD&&f!=SUB){error(94);return 0;}
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
      if(ISPOINTER(p->left->ntyp->flags)){
	if((p->left->ntyp->next->flags&NQ)==VOID)
	  {error(95);return 0;}
	if(ISPOINTER(p->right->ntyp->flags)){
	  if((p->right->ntyp->next->flags&NQ)==VOID)
	    {error(95);return 0;}
	  if(!compatible_types(p->left->ntyp->next,p->right->ntyp->next,NQ))
	    {error(41);}
	  if(f!=SUB){
	    error(96);
	    return 0;
	  }else{
	    typf=3;
	  }
	}else{
	  if(!ISINT(p->right->ntyp->flags))
	    {error(97,ename[f]);return 0;}
	  if(p->right->flags!=PMULT&&p->right->flags!=PCEXPR){
	    new=new_node();
	    new->flags=PMULT;
	    new->ntyp=0;
	    new->left=p->right;
	    new->right=new_node();
	    if(is_vlength(p->left->ntyp->next)){
	      new->right->flags=IDENTIFIER;
	      new->right->identifier=empty;
	      new->right->dsize=vlength_szof(p->left->ntyp->next);
	      new->right->val.vmax=l2zm(0L);
	      new->right->sidefx=0;
	    }else{
	      new->right->flags=PCEXPR;
	      sz=szof(p->left->ntyp->next);
	      if(zmeqto(l2zm(0L),sz)) error(78);
#if HAVE_INT_SIZET
	      new->right->val.vint=zm2zi(sz);
#else
	      new->right->val.vlong=zm2zl(sz);
#endif
	    }
	    new->right->left=new->right->right=0;
	    new->right->ntyp=new_typ();
#if HAVE_INT_SIZET
	    new->right->ntyp->flags=INT;
#else
	    new->right->ntyp->flags=LONG;
#endif
	    p->right=new;
#ifdef MAXADDI2P
	    ok&=type_expression2(new,&pmt);
#else
	    ok&=type_expression2(new,0);
#endif
	  }
	  typf=1;
	}
      }else{
	np merk;
	if(!ISPOINTER(p->right->ntyp->flags))
	  {error(98);return 0;}
	if((p->right->ntyp->next->flags&NQ)==VOID)
	  {error(95);return 0;}
	if(!ISINT(p->left->ntyp->flags))
	  {error(98);return 0;}
	if(p->flags==SUB){error(99);return 0;}
	if(p->left->flags!=PMULT&&p->left->flags!=PCEXPR){
	  new=new_node();
	  new->flags=PMULT;
	  new->ntyp=0;
	  new->left=p->left;
	  new->right=new_node();
	  if(is_vlength(p->right->ntyp->next)){
	    new->right->flags=IDENTIFIER;
	    new->right->identifier=empty;
	    new->right->dsize=vlength_szof(p->right->ntyp->next);
	    new->right->val.vmax=l2zm(0L);
	    new->right->sidefx=0;
	  }else{
	    new->right->flags=PCEXPR;
	    sz=szof(p->right->ntyp->next);
	    if(zmeqto(l2zm(0L),sz)) error(78);
#if HAVE_INT_SIZET
	    new->right->val.vint=zm2zi(sz);
#else
	    new->right->val.vlong=zm2zl(sz);
#endif
	  }
	  new->right->left=new->right->right=0;
	  new->right->ntyp=new_typ();
#if HAVE_INT_SIZET
	  new->right->ntyp->flags=INT;
#else
	  new->right->ntyp->flags=LONG;
#endif
	  p->left=new;
#ifdef MAXADDI2P
	  ok&=type_expression2(new,&pmt);
#else
	  ok&=type_expression2(new,0);
#endif
	}
	typf=2;
	merk=p->left;p->left=p->right;p->right=merk;
      }
      if(typf==0){ierror(0);return(0);}
      else{
	if(typf==3){
	  p->ntyp=new_typ();
	  p->ntyp->flags=PTRDIFF_T(p->left->ntyp->flags);
	}else{
	  /*if(typf==1)*/ p->ntyp=clone_typ(p->left->ntyp);
	  /* else       p->ntyp=clone_typ(p->right->ntyp);*/
	  /*  Abfrage wegen Vertauschen der Knoten unnoetig   */
	}
      }
    }else{
      if(f==LSHIFT||f==RSHIFT){
	if(ttyp&&f==LSHIFT&&(ttf<=INT||ttf<(p->left->ntyp->flags&NQ))&&shortcut(f,ttyp->flags&NU)){
	  p->ntyp=clone_typ(ttyp);
	}else if(ttyp&&f==RSHIFT&&ttf<=INT&&ttf<=(p->left->ntyp->flags&NQ)&&shortcut(f,p->left->ntyp->flags&NU)){
	  p->ntyp=clone_typ(p->left->ntyp);
	}else{
	  p->ntyp=arith_typ(p->left->ntyp,p->left->ntyp);
	  p->ntyp->flags&=~NQ;
	  p->ntyp->flags|=int_erw(p->left->ntyp->flags);
	}
	if(p->right->flags==CEXPR){
	  eval_constn(p->right);
	  if(!zmleq(l2zm(0L),vmax)) error(366);
	  if(zmleq(zmmult(sizetab[p->ntyp->flags&NQ],char_bit),vmax)&&zmleq(zmmult(sizetab[p->left->ntyp->flags&NQ],char_bit),vmax)) error(367);
	}
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
      }else{
	/* ggfs. in kleinerem Zieltyp auswerten - bei float keinen shortcut (wäre evtl. double=>float unkritisch?) */
	if(ttyp&&(ttf<=INT||ttf<(p->left->ntyp->flags&NQ)||ttf<(p->right->ntyp->flags&NQ))&&!ISFLOAT(ttf)&&!ISFLOAT(p->left->ntyp->flags)&&!ISFLOAT(p->right->ntyp->flags)&&shortcut(f==PMULT?MULT:f,ttyp->flags&NU)&&(!must_convert(p->left->ntyp->flags,ttyp->flags,0)||!must_convert(p->right->ntyp->flags,ttyp->flags,0)))
	  p->ntyp=clone_typ(ttyp);
	else
	  p->ntyp=arith_typ(p->left->ntyp,p->right->ntyp);
	if(!ISINT(p->ntyp->flags)&&(f==MOD||f==LSHIFT||f==RSHIFT))
	  {error(101);ok=0;}
      }
    }
    /*  fuegt &a+x zusammen, noch sub und left<->right machen   */
    /*  Bei CEXPR statt PCEXPR auch machen?                     */
    if((p->flags==ADD||p->flags==SUB)){
      np m,c=0,a=0;
      if(p->left->flags==PCEXPR&&p->flags==ADD) c=p->left;
      if(p->right->flags==PCEXPR) c=p->right;
      if(p->left->flags==ADDRESS||p->left->flags==ADDRESSA||p->left->flags==ADDRESSS) a=p->left;
      if(p->right->flags==ADDRESS||p->right->flags==ADDRESSA||p->right->flags==ADDRESSS) a=p->right;
      if(c&&a){
	m=a->left;
	/*  kann man das hier so machen oder muss man da mehr testen ?  */
	while(m->flags==FIRSTELEMENT||m->flags==ADDRESS||m->flags==ADDRESSA||m->flags==ADDRESSS) m=m->left;
	if((m->flags==IDENTIFIER||m->flags==STRING)&&!is_vlength(m->ntyp)){
	  if(DEBUG&1) printf("&a+x with %s combined\n",ename[p->left->flags]);
	  eval_const(&c->val,c->ntyp->flags);
	  if(p->flags==ADD)
	    m->val.vmax=zumadd(m->val.vmax,vmax);
	  else 
	    m->val.vmax=zmsub(m->val.vmax,vmax);
	  vmax=szof(m->ntyp);
	  if(!zmeqto(vmax,l2zm(0L))&&zumleq(vmax,m->val.vmax)){
	    if(zumeqto(vmax,m->val.vmax))
	      error(79);
	    else
	      error(80);
	  }
	  vmax=l2zm(0L);
	  if(!zmeqto(m->val.vmax,l2zm(0L))&&zumleq(m->val.vmax,vmax)) error(80);
	  free_expression(c);
	  if(p->ntyp) freetyp(p->ntyp);
	  *p=*a;
	  free(a);
	  return type_expression2(p,0);
	}
      }
    }
    if(!mopt){
      if(!alg_opt(p,ttyp)) ierror(0);
    }
    return ok;
  }
  if(f==CAST){
    int from=(p->left->ntyp->flags),to=(p->ntyp->flags);
#ifdef HAVE_MISRA
/* removed */
#endif
    from&=NQ;to&=NQ;
    if(to==VOID) return ok;
    if(from==VOID)
      {error(102);return 0;}
    if((!ISARITH(to)||!ISARITH(from))&&
       (!ISPOINTER(to)||!ISPOINTER(from))){
      if(ISPOINTER(to)){
	if(ISINT(from)){
#ifdef HAVE_MISRA
/* removed */
#endif
	  if(!zmleq(sizetab[from],sizetab[to])){
	    error(103);
	  }
	}else{
	  error(104);return 0;
	}
      }else{
	if(!ISPOINTER(from))
	  {error(105);return 0;}
	if(ISINT(to)){
#ifdef HAVE_MISRA
/* removed */
#endif
	  if(!zmleq(sizetab[from],sizetab[to])){
	    error(106);
	  }
	}else{
	  error(104);return 0;
	}
      }
    }
    if(ISINT(from)&&ISINT(to)&&!zmleq(sizetab[from],sizetab[to])&&p->left->flags!=CEXPR) error(166);
    if(ISPOINTER(to)&&ISPOINTER(from)&&!zmleq(falign(p->ntyp->next),falign(p->left->ntyp->next)))
      error(167);
    if(p->left->flags==CEXPR){
      eval_constn(p->left);
      if(ISPOINTER(p->ntyp->flags))
	if(!zumeqto(vumax,ul2zum(0UL))||!zmeqto(vmax,l2zm(0L))||!zldeqto(vldouble,d2zld(0.0)))
	  error(81);
      insert_constn(p);
      p->flags=CEXPR;
      if(!p->left->sidefx){
	if(!no_cast_free) 
	  free_expression(p->left);
	p->left=0;
      }
    }
#ifdef HAVE_ECPP
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
    return ok;
  }
  if(f==MINUS||f==KOMPLEMENT||f==NEGATION){
    if(ISVECTOR(p->left->ntyp->flags)){
      if(f==NEGATION){
	if(ISFLOAT(VECTYPE(p->left->ntyp->flags))){error(98);return 0;}
	p->ntyp=new_typ();
	p->ntyp->flags=p->left->ntyp->flags&NQ;
	return ok;
      }
      if(f==KOMPLEMENT&&ISFLOAT(VECTYPE(p->left->ntyp->flags))){error(109);return 0;}
      p->ntyp=clone_typ(p->left->ntyp);
      return ok;
    }
    if(!ISARITH(p->left->ntyp->flags)){
      if(f!=NEGATION){
	error(107);return 0;
      }else{
	if(!ISPOINTER(p->left->ntyp->flags))
	  {error(108);return 0;}
      }
    }
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
    if(f==KOMPLEMENT&&!ISINT(p->left->ntyp->flags))
      {error(109);return 0;}
    if(f==NEGATION){
      p->ntyp=new_typ();
      p->ntyp->flags=INT;
    }else{
      if(!p->left->ntyp) ierror(0);
      p->ntyp=clone_typ(p->left->ntyp);
      if(ISINT(p->ntyp->flags)){
	if(!ttyp||!shortcut(f,ttyp->flags&NU)){
	  p->ntyp->flags=int_erw(p->ntyp->flags);
	}else{ 
	  if((f==MINUS&&!zmleq(sizetab[ttyp->flags&NQ],sizetab[p->ntyp->flags&NQ]))||
	     (f==KOMPLEMENT&&!zmleq(sizetab[ttyp->flags&NQ],sizetab[p->ntyp->flags&NQ])&&(p->ntyp->flags&UNSIGNED)))
	    p->ntyp->flags=int_erw(p->ntyp->flags);
	}
      }
    }
    if(p->left->flags==CEXPR){
      eval_constn(p->left);
      if(f==KOMPLEMENT){
	if(p->ntyp->flags&UNSIGNED){
	  gval.vumax=zumkompl(vumax);
	  eval_const(&gval,(UNSIGNED|MAXINT));
	}else{
	  gval.vmax=zmkompl(vmax);
	  eval_const(&gval,MAXINT);
	}
      }
      if(f==MINUS){
	if(ISFLOAT(p->ntyp->flags)){
	  gval.vldouble=zldsub(d2zld(0.0),vldouble);
	  eval_const(&gval,LDOUBLE);
	}else{
	  if(p->ntyp->flags&UNSIGNED){
	    gval.vumax=zumsub(ul2zum(0UL),vumax);
	    eval_const(&gval,(UNSIGNED|MAXINT));
	  }else{
	    gval.vmax=zmsub(l2zm(0L),vmax);
	    eval_const(&gval,MAXINT);
	  }
	}
      }
      if(f==NEGATION){
	if(zldeqto(vldouble,d2zld(0.0))&&zumeqto(vumax,ul2zum(0UL))&&zmeqto(vmax,l2zm(0L)))
	  gval.vmax=l2zm(1L); 
	else 
	  gval.vmax=l2zm(0L);
	eval_const(&gval,MAXINT);
      }
      insert_constn(p);
      p->flags=CEXPR;
      if(!p->left->sidefx&&p->left) {free_expression(p->left);p->left=0;}
    }
    return ok;
  }
  if(f==CONTENT){
    if(!ISPOINTER(p->left->ntyp->flags))
      {error(111);return 0;}
    if(!ISARRAY(p->left->ntyp->next->flags)&&type_uncomplete(p->left->ntyp->next))
      {error(112);return 0;}
    p->ntyp=clone_typ(p->left->ntyp->next);
    if(!ISARRAY(p->ntyp->flags)) p->lvalue=1;
    if(p->left->flags==ADDRESS&&zumeqto(p->left->val.vumax,ul2zum(0UL))){
      /*  *&x durch x ersetzen                                */
      np merk;
      merk=p->left;
      if(p->ntyp) freetyp(p->ntyp);
      if(p->left->ntyp) freetyp(p->left->ntyp);
      *p=*p->left->left;
      free(merk->left);
      free(merk);
      return ok;
    }
    /*  *&ax durch firstelement-of(x) ersetzen  */
    if(p->left->flags==ADDRESSA||p->left->flags==ADDRESSS){
      if(!is_vlength(p->left->left->ntyp)){
	np merk;
	if(DEBUG&1) printf("substitutet * and %s with FIRSTELEMENT\n",ename[p->left->flags]);
	p->flags=FIRSTELEMENT;
	p->lvalue=1;    /*  evtl. hier erst Abfrage ?   */
	merk=p->left;
	p->left=merk->left;
	p->right=merk->right;
	if(merk->ntyp) freetyp(merk->ntyp);
	free(merk);
      }
    }
    if (p->ntyp->flags&VOLATILE) p->sidefx=1;
    return ok;
  }
  if(f==FIRSTELEMENT){
    if(ISARRAY(p->left->ntyp->flags)){
      p->ntyp=clone_typ(p->left->ntyp->next);
    }else{
      int i,n=-1;
      for(i=0;i<p->left->ntyp->exact->count;i++)
	if(!strcmp((*p->left->ntyp->exact->sl)[i].identifier,p->right->identifier)) n=i;
      if(n<0) ierror(0);
      p->ntyp=clone_typ((*p->left->ntyp->exact->sl)[n].styp);
    }
    p->lvalue=1;    /*  hier noch genauer testen ?  */
    return ok;
  }
  if(f==ADDRESS){
    if(!ISFUNC(p->left->ntyp->flags)&&!ISARRAY(p->left->ntyp->flags)){
      if(!p->left->lvalue){error(115);return 0;}
      if(p->left->flags==IDENTIFIER){
	Var *v;
	v=find_var(p->left->identifier,0);
	if(!v){error(116,p->left->identifier);return 0;}
	if(v->storage_class==REGISTER)
	  {error(117);return 0;}
      }
    }
    p->ntyp=new_typ();
    p->ntyp->flags=ptype(p->left);
    p->ntyp->next=clone_typ(p->left->ntyp);
    fix_addr_ptype(p);
    return ok;
  }
  if(f==ADDRESSA){
    p->ntyp=clone_typ(p->left->ntyp);
    p->ntyp->flags=ptype(p->left);
    fix_addr_ptype(p);
    return ok;
  }
  if(f==ADDRESSS){
    int i,n=-1;
    struct_list *sl=0;
    if(!ecpp){
      for(i=0;i<p->left->ntyp->exact->count;i++)
	if(!strcmp((*p->left->ntyp->exact->sl)[i].identifier,p->right->identifier)) n=i;
      if(n<0)
        return 0;
      else
        sl=&(*p->left->ntyp->exact->sl)[n];
    }
#ifdef HAVE_ECPP
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
    p->ntyp=new_typ();
    if(!p->left->ntyp) ierror(0);
    if(!p->left->ntyp->exact) ierror(0);
    if(!sl->styp) ierror(0);
    p->ntyp->next=clone_typ(sl->styp);
    p->ntyp->flags=ptype(p->left);
    fix_addr_ptype(p);
    return ok;
  }
  if(f==DSTRUCT){
    struct_declaration *sd=p->left->ntyp->exact;
    char *identifier=p->right->identifier;
    int i=0,f,bfs=-1,bfo=-1;type *t;np new;zmax offset=l2zm(0L);
    if(!ISSTRUCT(p->left->ntyp->flags)&&!ISUNION(p->left->ntyp->flags))
      {error(8);return 0;}
    if(type_uncomplete(p->left->ntyp)){error(11);return 0;}
    if(p->right->flags!=MEMBER) ierror(0);
    if(i>=p->left->ntyp->exact->count) {error(23,p->right->identifier);return 0;}
    if(!ecpp){
      offset=struct_offset(sd,identifier);
    }
#ifdef HAVE_ECPP
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
    if(ISUNION(p->left->ntyp->flags)) offset=l2zm(0L);
    p->flags=CONTENT;
    if(p->ntyp) {freetyp(p->ntyp);p->ntyp=0;}
    new=new_node();
    new->flags=ADD;
    new->ntyp=0;
    new->right=new_node();
    new->right->left=new->right->right=0;
    new->right->flags=PCEXPR;
    new->right->ntyp=new_typ();
    if(/*MINADDI2P<=INT&&*/zumleq(zm2zum(offset),t_max(INT))){
      new->right->ntyp->flags=INT;
      new->right->val.vint=zm2zi(offset);
    }else{
      new->right->ntyp->flags=LONG;
      new->right->ntyp->next=0;
      new->right->val.vlong=zm2zl(offset);
    }
    new->left=new_node();
    new->left->flags=ADDRESSS;
    new->left->left=p->left;
    new->left->right=p->right;
    new->left->ntyp=0;
    p->left=new;p->right=0;

    /* Check for bitfields */
    i=0;
    while(i<sd->count&&strcmp((*sd->sl)[i].identifier,identifier)){
      i++;
    }
    if(i<sd->count){
      bfo=(*sd->sl)[i].bfoffset;
      bfs=(*sd->sl)[i].bfsize;
    }else
      bfs=-1;
    if(bfs!=-1){
      /* create a special node for bitfields */
      ok|=type_expression2(p,0);
      new=new_node();
      *new=*p;
      p->flags=BITFIELD;
      p->left=new;
      p->right=0;
      p->sidefx=0;
      p->bfs=bfs;
      p->bfo=bfo;
      p->ntyp=clone_typ(new->ntyp);
      return ok;
    }
    return type_expression2(p,0);
  }
  if(f==PREINC||f==POSTINC||f==PREDEC||f==POSTDEC){
    if(!p->left->lvalue){error(86);return 0;}
    if(p->left->ntyp->flags&CONST){error(87);return 0;}
    if(!ISARITH(p->left->ntyp->flags)){
      if(!ISPOINTER(p->left->ntyp->flags)){
	error(24);
	return 0;
      }else{
#ifdef HAVE_MISRA
/* removed */
#endif
	if((p->left->ntyp->next->flags&NQ)==VOID)
	  {error(95);return 0;}
      }
    }
    p->ntyp=clone_typ(p->left->ntyp);
    p->sidefx=1;
    return ok;
  }
  if(f==CALL){
    argument_list *al;int i,flags=0;char *s=0;
    struct_declaration *sd;
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
    al=p->alist;
    if(!ISPOINTER(p->left->ntyp->flags)||!ISFUNC(p->left->ntyp->next->flags))
      {error(26);return 0;}
    if(ok&&p->left->left&&p->left->left->flags==IDENTIFIER&&p->left->left->o.v->storage_class==EXTERN){
      s=p->left->left->o.v->identifier;
      flags=p->left->left->o.v->flags;
    }
    sd=p->left->ntyp->next->exact;
    if(!sd) ierror(0);
    if(sd->count==0){
#ifdef HAVE_MISRA
/* removed */
#endif
      error(162);
      if(s){
	if(!strcmp(s,"printf")||!strcmp(s,"fprintf")||!strcmp(s,"sprintf")||
	   !strcmp(s,"scanf")|| !strcmp(s,"fscanf")|| !strcmp(s,"sscanf"))
	  error(213);
      }
    }
    if(!(ecpp&&al&&al->arg->ntyp)){
      i=0;
      while(al){
	if(!al->arg) ierror(0);
	if(!type_expression2(al->arg,(short_push&&i<sd->count)?(*sd->sl)[i].styp:0)) return 0;
	al->arg=makepointer(al->arg);
	if(type_uncomplete(al->arg->ntyp)) error(39);
	al=al->next;
	i++;
      }
    }
    p->sidefx=1;
    p->ntyp=clone_typ(p->left->ntyp->next->next);
    i=0;al=p->alist;
    while(al){
      if(i>=sd->count) return ok;
      if(!(*sd->sl)[i].styp) return ok; /* nur Bezeichner, aber kein Typ im Prototype */
      if(!test_assignment((*sd->sl)[i].styp,al->arg)) return 0;
#ifdef HAVE_MISRA
/* removed */
/* removed */
#endif
      if(i==sd->count-1&&(flags&(PRINTFLIKE|SCANFLIKE))){
	if(al->arg->left&&al->arg->left->flags==STRING){
	  /*  Argumente anhand des Formatstrings ueberpruefen */
	  const_list *cl=al->arg->left->cl;
	  int fused=0;
	  al=al->next;
	  while(cl&&cl->other){
	    int c,fflags=' ',at;
	    type *t;
	    enum{LL=1,HH};
	    c=(int)zm2l(zc2zm(cl->other->val.vchar));
	    c=CHARBACK(c);
	    cl=cl->next;
	    if(c==0){
	      if(cl) error(215);
	      break;
	    }
	    if(c!='%') continue;
	    if(!cl){error(214);return ok;}
	    c=(int)zm2l(zc2zm(cl->other->val.vchar));
	    c=CHARBACK(c);
	    cl=cl->next;
	    while(isdigit((unsigned char)c)||
		  c=='-'||c=='+'||c==' '||c=='#'||c=='.'||
		  c=='h'||c=='l'||c=='L'||c=='z'||c=='j'||c=='t'||c=='*'){
	      fused|=3;
	      if(c=='*'&&(flags&PRINTFLIKE)){
		if(!al) {error(214);return ok;}
		at=al->arg->ntyp->flags&NQ;
		al=al->next;
		if(at>INT) {error(214);return ok;}
	      }
	      if((fflags!='*'||(flags&PRINTFLIKE))&&(c=='h'||c=='l'||c=='L'||c=='*'||c=='t'||c=='z'||c=='j')){
		if(c=='l'&&fflags=='l')
		  fflags=LL;
		else if(c=='h'&&fflags=='h')
		  fflags=HH;
		else
		  fflags=c;
	      }
	      c=(int)zm2l(zc2zm(cl->other->val.vchar));
	      c=CHARBACK(c);
	      cl=cl->next;
	      if(!cl){error(214);return ok;}
	    }
	    /*FIXME: assumes intmax_t==long long */
	    if(fflags=='j') fflags=LL;
#if HAVE_INT_SIZET	    
	    if(fflags=='z') fflags=' ';
#else
	    if(fflags=='z') fflags='l';
#endif
	    if(fflags=='t'){
	      if(PTRDIFF_T(CHAR)==LLONG)
		fflags=LL;
	      else if(PTRDIFF_T(CHAR)==LONG)
		fflags='l';
	      else
		fflags=' ';
	    }
	    if(DEBUG&1) printf("format=%c%c\n",fflags,c);
	    if(fflags=='*'&&(flags&SCANFLIKE)) continue;
	    if(c!='%'){
	      if(!al){error(214);return ok;}
	      t=al->arg->ntyp;
	      if(DEBUG&1){ prd(stdout,t);printf("\n");}
	      if((flags&SCANFLIKE)){
		if(!ISPOINTER(t->flags)){error(214);return ok;}
		t=t->next;
	      }
	      at=t->flags&NU;
	    }
	    if(flags&PRINTFLIKE){
	      switch(c){
	      case '%':
		fused|=1;
		break;
	      case 'o':
	      case 'x':
	      case 'X':
	      case 'c':
		at&=NQ;     /*  fall through    */
	      case 'i':
	      case 'd':
		fused|=1;
		if(at==LLONG&&fflags!=LL){error(214);return ok;}
		if(at==LONG&&fflags!='l'){error(214);return ok;}
		if(fflags=='l'&&at!=LONG){error(214);return ok;}
		if(at<CHAR||at>LLONG){error(214);return ok;}
		break;
	      case 'u':
		fused|=1;
		if(al->arg->flags==CEXPR) at|=UNSIGNED;
		if(at==(UNSIGNED|LLONG)&&fflags!=LL){error(214);return ok;}
		if(at==(UNSIGNED|LONG)&&fflags!='l'){error(214);return ok;}
		if(fflags=='l'&&at!=(UNSIGNED|LONG)){error(214);return ok;}
		if(at<(UNSIGNED|CHAR)||at>(UNSIGNED|LLONG)){error(214);return ok;}
		break;
	      case 's':
		fused|=1;
		if(!ISPOINTER(at)||(t->next->flags&NQ)!=CHAR){error(214);return ok;}
		break;
	      case 'f':
	      case 'e':
	      case 'E':
	      case 'g':
	      case 'G':
		fused|=7;
		if(fflags=='L'){
		  if(at!=LDOUBLE){error(214); return ok;}
		}else{
		  if(at!=FLOAT&&at!=DOUBLE){error(214);return ok;}
		}
		break;
	      case 'p':
		fused|=3;
		if(!ISPOINTER(at)||(t->next->flags)!=VOID){error(214);return ok;}
		break;
	      case 'n':
		fused|=3;
		if(!ISPOINTER(at)){error(214);return ok;}
		at=t->next->flags&NU;
		if(fflags==HH&&at!=CHAR){error(214);return ok;}
		if(fflags=='h'&&at!=SHORT){error(214);return ok;}
		if(fflags==' '&&at!=INT){error(214);return ok;}
		if(fflags=='l'&&at!=LONG){error(214);return ok;}
		if(fflags==LL&&at!=LLONG){error(214);return ok;}
		break;
	      default:
		error(214);return ok;
	      }
	    }else{
	      switch(c){
	      case '%':
		fused|=1;
		break;
	      case '[':
		fused|=3;
		do{
		  c=(int)zm2l(zc2zm(cl->other->val.vchar));
		  c=CHARBACK(c);
		  cl=cl->next;
		  if(!cl){error(214);return ok;}
		}while(c!=']');     /*  fall through    */
	      case 's':
	      case 'c':
		fused|=1;
		if((at&NQ)!=CHAR){error(214);return(ok);}
		break;
	      case 'n':
		fused|=3;       /*  fall through    */
	      case 'd':
	      case 'i':
	      case 'o':
		fused|=1;
		if(fflags==HH&&at!=CHAR){error(214);return ok;}
		if(fflags=='h'&&at!=SHORT){error(214);return ok;}
		if(fflags==' '&&at!=INT){error(214);return ok;}
		if(fflags=='l'&&at!=LONG){error(214);return ok;}
		if(fflags==LL&&at!=LLONG){error(214);return ok;}
		break;
	      case 'x':
	      case 'u':
		fused|=1;
		if(fflags==HH&&at!=CHAR){error(214);return ok;}
		if(fflags=='h'&&at!=(UNSIGNED|SHORT)){error(214);return ok;}
		if(fflags==' '&&at!=(UNSIGNED|INT)){error(214);return ok;}
		if(fflags=='l'&&at!=(UNSIGNED|LONG)){error(214);return ok;}
		if(fflags==LL&&at!=(UNSIGNED|LLONG)){error(214);return ok;}
		break;
	      case 'e':
	      case 'f':
	      case 'g':
		fused|=7;
		if(fflags==' '&&at!=FLOAT){error(214);return ok;}
		if(fflags=='l'&&at!=DOUBLE){error(214);return ok;}
		if(fflags=='L'&&at!=LDOUBLE){error(214);return ok;}
		break;
	      case 'p':
		fused|=3;
		if(!ISPOINTER(at)||(t->next->flags&NQ)!=VOID){error(214);return ok;}
		break;
	      default:
		error(214);return ok;
	      }
	    }
	    if(c!='%') al=al->next;
	  }
	  if(al){ error(214);return ok;} /* zu viele */
	  if(DEBUG&1) printf("fused=%d\n",fused);
	  if(fused!=7&&s){
	    /*  Wenn kein Format benutzt wird, kann man printf, */
	    /*  scanf etc. durch aehnliches ersetzen.           */
	    Var *v;char repl[MAXI+6]="__v";
	    if(fused==3) fused=2;
	    repl[3]=fused+'0';repl[4]=0;
	    strcat(repl,s);
	    if(DEBUG&1) printf("repl=%s\n",repl);
	    while(fused<=2){
	      v=find_var(repl,0);
	      if(v&&v->storage_class==EXTERN){
		p->left->left->o.v=v;
		break;
	      }
	      fused++;repl[3]++;
	    }
	  }
	  return ok;
	}
      }
      i++;al=al->next;
    }
    if(i>=sd->count) return ok;
    if((*sd->sl)[i].styp&&((*sd->sl)[i].styp->flags&NQ)!=VOID){error(83);/*printf("sd->count=%d\n",sd->count);*/}
    return ok;
  }
  if(f==COND){
    if(!ISARITH(p->left->ntyp->flags)&&!ISPOINTER(p->left->ntyp->flags)){
      error(29);
      return 0;
    }
    if(p->left->flags==CEXPR&&!p->left->sidefx){
      int null;np merk;
      if(DEBUG&1) printf("constant conditional-expression simplified\n");
      eval_constn(p->left);
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
	null=1; 
      else 
	null=0;
      free_expression(p->left);
      merk=p->right;
      if(null){
	free_expression(p->right->left);
	*p=*p->right->right;
      }else{
	free_expression(p->right->right);
	*p=*p->right->left;
      }
      if(merk->ntyp) freetyp(merk->ntyp);
      free(merk);
      return 1;
    }
    if(const_expr){error(46);return 0;}
    p->ntyp=clone_typ(p->right->ntyp);
    return 1;
  }
  if(f==COLON){
    /*  Hier fehlt noch korrekte Behandlung der Typattribute    */
    if(ISARITH(p->left->ntyp->flags)&&ISARITH(p->right->ntyp->flags)){
      p->ntyp=arith_typ(p->left->ntyp,p->right->ntyp);
      return 1;
    }
    if(compatible_types(p->left->ntyp,p->right->ntyp,NQ)){
      p->ntyp=clone_typ(p->left->ntyp);
      return 1;
    }
    if(ISPOINTER(p->left->ntyp->flags)&&ISPOINTER(p->right->ntyp->flags)){
      if((p->left->ntyp->next->flags&NQ)==VOID){
	if(nullpointer(p->left))
	  p->ntyp=clone_typ(p->right->ntyp);
	else
	  p->ntyp=clone_typ(p->left->ntyp);
	return 1;
      }
      if((p->right->ntyp->next->flags&NQ)==VOID){
	if(nullpointer(p->right))
	  p->ntyp=clone_typ(p->left->ntyp);
	else
	  p->ntyp=clone_typ(p->right->ntyp);
	return 1;
      }
    }
    if(ISPOINTER(p->left->ntyp->flags)&&p->right->flags==CEXPR){
      eval_constn(p->right);
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(ul2zum(0UL),vumax)&&zldeqto(d2zld(0.0),vldouble)){
	p->ntyp=clone_typ(p->left->ntyp);
	return 1;
      }
    }
    if(ISPOINTER(p->right->ntyp->flags)&&p->left->flags==CEXPR){
      eval_constn(p->left);
      if(zmeqto(l2zm(0L),vmax)&&zumeqto(ul2zum(0UL),vumax)&&zldeqto(d2zld(0.0),vldouble)){
	p->ntyp=clone_typ(p->right->ntyp);
	return 1;
      }
    }
    error(31);
    return 0;
  }
  if(f) printf("type_testing fuer diesen Operator (%d) noch nicht implementiert\n",f);
  return 0;
}

np makepointer(np p)
/*  Fuehrt automatische Zeigererzeugung fuer Baumwurzel durch   */
/*  Durch mehrmaligen Aufruf von type_expression() ineffizient  */
{
    struct_declaration *sd;
    if(ISARRAY(p->ntyp->flags)||ISFUNC(p->ntyp->flags)){
      np new=new_node();
        if(ISARRAY(p->ntyp->flags)){
            new->flags=ADDRESSA;
            new->ntyp=clone_typ(p->ntyp);
            new->ntyp->flags=POINTER_TYPE(new->ntyp->next);
        }else{
            new->flags=ADDRESS;
            new->ntyp=new_typ();
            new->ntyp->flags=POINTER_TYPE(p->ntyp);
            new->ntyp->next=clone_typ(p->ntyp);
        }
        new->left=p;
        new->right=0;
        new->lvalue=0;  /* immer korrekt ?  */
        new->sidefx=p->sidefx;
/*        type_expression(new);*/
        return new;
    }else 
      return p;
}
int alg_opt(np p,type *ttyp)
/*  fuehrt algebraische Vereinfachungen durch                   */
/*  hier noch genau testen, was ANSI-gemaess erlaubt ist etc.   */
/*  v.a. Floating-Point ist evtl. kritisch                      */
{
    int c=0,f,komm,null1,null2,eins1,eins2;np merk;
    zldouble d1,d2;zumax u1,u2;zmax s1,s2;
    f=p->flags;
    
    /* do not optimze pointer-pointer */
    if(f==SUB&&ISPOINTER(p->left->ntyp->flags)&&ISPOINTER(p->right->ntyp->flags))
      return 1;

    /*  kommutativ? */
    if(f==ADD||f==MULT||f==PMULT||(f>=OR&&f<=AND)) komm=1; else komm=0;
    /*  Berechnet Wert, wenn beides Konstanten sind     */
    if(p->left->flags==CEXPR||p->left->flags==PCEXPR){
        eval_constn(p->left);
        d1=vldouble;u1=vumax;s1=vmax;c|=1;
    }
    if(p->right->flags==CEXPR||p->right->flags==PCEXPR){
        eval_constn(p->right);
        d2=vldouble;u2=vumax;s2=vmax;c|=2;
    }
    if(c==3){
        p->flags=CEXPR;
        if(DEBUG&1) printf("did simple constant folding\n");
        if(!p->left->sidefx) {free_expression(p->left);p->left=0;}
        if(!p->right->sidefx) {free_expression(p->right);p->right=0;}
        if(f==AND){
	  vumax=zumand(u1,u2);
	  vmax=zmand(s1,s2);
        }
        if(f==OR){
	  vumax=zumor(u1,u2);
	  vmax=zmor(s1,s2);
        }
        if(f==XOR){
	  vumax=zumxor(u1,u2);
	  vmax=zmxor(s1,s2);
        }
        if(f==ADD){
	  vumax=zumadd(u1,u2);
	  vmax=zmadd(s1,s2);
	  vldouble=zldadd(d1,d2);
        }
        if(f==SUB){
	  vumax=zumsub(u1,u2);
	  vmax=zmsub(s1,s2);
	  vldouble=zldsub(d1,d2);
        }
        if(f==MULT||f==PMULT){
	  vumax=zummult(u1,u2);
	  vmax=zmmult(s1,s2);
	  vldouble=zldmult(d1,d2);
	  if(f==PMULT) p->flags=PCEXPR;
        }
        if(f==DIV){
	  if(zmeqto(l2zm(0L),s2)&&zumeqto(ul2zum(0UL),u2)&&zldeqto(d2zld(0.0),d2)){
	    error(84);
	    vmax=l2zm(0L);vumax=ul2zum(0UL);vldouble=d2zld(0.0);
	  }else{
	    if(!zumeqto(ul2zum(0UL),u2)) vumax=zumdiv(u1,u2);
	    if(!zmeqto(l2zm(0L),s2)) vmax=zmdiv(s1,s2);
	    if(!zldeqto(d2zld(0.0),d2)) vldouble=zlddiv(d1,d2);
	  }
        }
        if(f==MOD){
	  if(zmeqto(l2zm(0L),s2)&&zumeqto(ul2zum(0UL),u2)){
	    error(84);
	    vmax=l2zm(0L);vumax=zm2zum(vmax);
	  }else{
	    if(!zumeqto(ul2zum(0UL),u2)) vumax=zummod(u1,u2);
	    if(!zmeqto(l2zm(0L),s2)) vmax=zmmod(s1,s2);
	  }
        }
        if(f==LSHIFT){
	  vumax=zumlshift(u1,u2);
	  vmax=zmlshift(s1,s2);
        }
        if(f==RSHIFT){
	  vumax=zumrshift(u1,u2);
	  vmax=zmrshift(s1,s2);
        }
	if(ISFLOAT(p->ntyp->flags)){
	  gval.vldouble=vldouble;
	  eval_const(&gval,LDOUBLE);
	}else{
	  if(p->ntyp->flags&UNSIGNED){
	    gval.vumax=vumax;
	    eval_const(&gval,(UNSIGNED|MAXINT));
	  }else{
	    gval.vmax=vmax;
	    eval_const(&gval,MAXINT);
	  }
	}
        insert_constn(p);
        return 1;
    }
    /*  Konstanten nach rechts, wenn moeglich       */
    if(c==1&&komm){
        if(DEBUG&1) printf("exchanged commutative constant operand\n");
        merk=p->left;p->left=p->right;p->right=merk;
        c=2;
        d2=d1;u2=u1;s2=s1;
    }
    /*  Vertauscht die Knoten, um Konstanten                */
    /*  besser zusammenzufassen (bei allen Type erlaubt?)   */
    /*  Hier muss noch einiges kontrolliert werden          */
    if(komm&&c==2&&p->flags==p->left->flags&&(ISINT(p->ntyp->flags)||c_flags[21]&USEDFLAG)){
        if(p->left->right->flags==CEXPR||p->left->right->flags==PCEXPR){
            np merk;
            merk=p->right;p->right=p->left->left;p->left->left=merk;
            if(DEBUG&1) printf("Vertausche Add-Nodes\n");
	    dontopt=0;
            return type_expression2(p,ttyp);
        }
    }
    null1=null2=eins1=eins2=0;
    if(c&1){
      if(zldeqto(d1,d2zld(0.0))&&zumeqto(u1,ul2zum(0UL))&&zmeqto(s1,l2zm(0L))) null1=1;   
      if(zldeqto(d1,d2zld(1.0))&&zumeqto(u1,ul2zum(1UL))&&zmeqto(s1,l2zm(1L))) eins1=1;
      if(!(p->ntyp->flags&UNSIGNED)&&zldeqto(d1,d2zld(-1.0))&&zmeqto(s1,l2zm(-1L))) eins1=-1;
    }
    if(c&2){
      if(zldeqto(d2,d2zld(0.0))&&zumeqto(u2,ul2zum(0UL))&&zmeqto(s2,l2zm(0L))) null2=1; 
      if(zldeqto(d2,d2zld(1.0))&&zumeqto(u2,ul2zum(1UL))&&zmeqto(s2,l2zm(1L))) eins2=1; 
      if(!(p->ntyp->flags&UNSIGNED)&&zldeqto(d2,d2zld(-1.0))&&zmeqto(s2,l2zm(-1L))) eins2=-1;
    }
    if(c==2){
        /*  a+0=a-0=a^0=a>>0=a<<0=a*1=a/1=a   */
        if(((eins2==1&&(f==MULT||f==PMULT||f==DIV))||(null2&&(f==ADD||f==SUB||f==OR||f==XOR||f==LSHIFT||f==RSHIFT)))&&!p->right->sidefx){
            if(DEBUG&1){if(f==MULT||f==PMULT||f==DIV) printf("a*/1->a\n"); else printf("a+-^0->a\n");}
            free_expression(p->right);
            merk=p->left;
            *p=*p->left;
/*            freetyp(merk->ntyp);  das war Fehler  */
            free(merk);
	    dontopt=0;
            return type_expression2(p,ttyp);
        }
        /*  a*0=0   */
        if(null2&&(f==MULT||f==PMULT||f==AND||f==DIV||f==MOD)){
            if(DEBUG&1) printf("a*&/%%0->0\n");
            if(null2&&(f==DIV||f==MOD)) error(84);
            if(p->flags==PMULT) p->flags=PCEXPR; else p->flags=CEXPR;
	    gval.vint=zm2zi(l2zm(0L));
	    eval_const(&gval,INT);
            /*  hier nur int,long,float,double moeglich, hoffe ich  */
            insert_constn(p);
            if(!p->left->sidefx){free_expression(p->left);p->left=0;} else make_cexpr(p->left);
            if(!p->right->sidefx){free_expression(p->right);p->right=0;} else make_cexpr(p->right);

/*            return(type_expression2(p,ttyp));   */
            return 1;
        }
        if(eins2==-1&&(f==MULT||f==PMULT||f==DIV)&&!p->right->sidefx){
            if(DEBUG&1) printf("a*/(-1)->-a\n");
            free_expression(p->right);
            p->right=0;
            p->flags=MINUS;
	    dontopt=0;
            return type_expression2(p,ttyp);
        }
	/* check volatile */
	if(0) /* TODO: make this efficient */
	{
	  type st={0};
	  if(!zmeqto(sizetab[CHAR],sizetab[SHORT])){
	    //if(f==AND&&(!ttyp||(ttyp->flags&NQ)>CHAR)&&shortcut(AND,CHAR)&&zumleq(u2,t_max[CHAR]))
	    //st.flags=CHAR;
	    if(f==AND&&(!ttyp||(ttyp->flags&NQ)>CHAR)/*&&(p->ntyp->flags&UNSIGNED)*/&&shortcut(AND,UNSIGNED|CHAR)&&zumleq(u2,tu_max[CHAR]))
	      st.flags=UNSIGNED|CHAR;
	  }
	  if(!zmeqto(sizetab[SHORT],sizetab[INT])){
	    //if(f==AND&&(!ttyp||(ttyp->flags&NQ)>SHORT)&&shortcut(AND,SHORT)&&zumleq(s2,t_max[SHORT]))
	    //st.flags=SHORT;
	    if(f==AND&&(!ttyp||(ttyp->flags&NQ)>SHORT)/*&&(p->ntyp->flags&UNSIGNED)*/&&shortcut(AND,UNSIGNED|SHORT)&&zumleq(u2,tu_max[SHORT]))
	      st.flags=UNSIGNED|SHORT;
	  }
	  //if(!ttyp&&(p->left->ntyp->flags&NQ)>INT) st.flags=0;
	  if(st.flags){
	    int ret;type *otyp;np n;
	    if(!p->ntyp) ierror(0);
	    otyp=clone_typ(p->ntyp);
	    n=new_node();
	    n->left=p->left;
	    n->right=0;
	    n->flags=CAST;
	    n->ntyp=clone_typ(&st);
	    eval_const(&p->right->val,p->right->ntyp->flags);
	    p->right->ntyp->flags=st.flags;
	    insert_const(&p->right->val,st.flags);
	    p->left=n;
	    n=new_node();
	    *n=*p;
	    p->left=n;
	    p->right=0;
	    p->flags=CAST;
	    p->ntyp=otyp;
	    ret=type_expression2(p->left,&st);

	    return ret;
	  }
	}
	
    }
    if(c==1){
        /*  0-a=-a  */
        if(f==SUB&&null1&&!p->left->sidefx){
            if(DEBUG&1) printf("0-a->-a\n");
            free_expression(p->left);
            p->flags=MINUS;
            p->left=p->right;
            p->right=0;
	    dontopt=0;
            return type_expression2(p,ttyp);
        }
        /*  0/a=0   */
        if(null1&&(f==DIV||f==MOD||f==LSHIFT||f==RSHIFT)){
            if(DEBUG&1) printf("0/%%<<>>a->0\n");
            p->flags=CEXPR;
	    gval.vint=zm2zi(l2zm(0L));
	    eval_const(&gval,INT);
            insert_constn(p);
            if(!p->left->sidefx){free_expression(p->left);p->left=0;}else make_cexpr(p->left);
            if(!p->right->sidefx){free_expression(p->right);p->right=0;} else make_cexpr(p->right);
	    dontopt=0;
            return type_expression2(p,ttyp);
        }
    }
    return 1;
}
void make_cexpr(np p)
/*  Macht aus einem Knoten, der durch constant-folding ueberfluessig    */
/*  wurde, eine PCEXPR, sofern er keine Nebenwirkungen von sich aus     */
/*  erzeugt. Hier noch ueberpruefen, ob CEXPR besser waere.             */
/*  Fuehrt rekursiven Abstieg durch. Ist das so korrekt?                */
{
    int f=p->flags;
    if(f!=ASSIGN&&f!=ASSIGNOP&&f!=CALL&&f!=POSTINC&&f!=POSTDEC&&f!=PREINC&&f!=PREDEC&&f!=LAND&&f!=LOR){
        p->flags=PCEXPR;
        if(p->left) make_cexpr(p->left);
        if(p->right) make_cexpr(p->right);
    }
}
int test_assignment(type *zt,np q)
/*  testet, ob q an Typ z zugewiesen werden darf    */
{
  type *qt=q->ntyp;
  if(ISARITH(zt->flags)&&ISARITH(qt->flags)){
    if(ISINT(zt->flags)&&ISINT(qt->flags)&&
      !zmleq(sizetab[qt->flags&NQ],sizetab[zt->flags&NQ])&&q->flags!=CEXPR){
#ifdef HAVE_MISRA
/* removed */
#endif
      error(166);
    }
    if(q->flags==CEXPR){
      zmax ms,ns;zumax mu,nu;
      eval_constn(q);
      ms=vmax;mu=vumax;
      insert_const(&gval,zt->flags&~UNSIGNED);
      eval_const(&gval,zt->flags&~UNSIGNED);
      ns=vmax;
      eval_constn(q);
      insert_const(&gval,zt->flags|UNSIGNED);
      eval_const(&gval,zt->flags|UNSIGNED);
      nu=vumax;
      if(!zumeqto(nu,mu)&&!zmeqto(ns,ms)) 
	error(363);
      else{
	if(zt->flags&UNSIGNED){
	  if(!zumeqto(nu,mu))
	    error(362);
	}else{
	  if(!zmeqto(ns,ms))
	    error(362);
	}
      }
    }
    return 1;
  }
  if((ISSTRUCT(zt->flags)&&ISSTRUCT(qt->flags))||
    (ISUNION(zt->flags)&&ISUNION(qt->flags))){
      if(!compatible_types(zt,qt,NU)){
        error(38);
      return 0;
    }else 
  return 1;
  }
  if(ISVECTOR(zt->flags)&&(zt->flags&NU)==(qt->flags&NU)){
      return 1;
  }
  if(ISPOINTER(zt->flags)&&ISPOINTER(qt->flags)){
    if((zt->next->flags&NQ)==VOID&&!ISFUNC(qt->next->flags)) return 1;
    if((qt->next->flags&NQ)==VOID&&!ISFUNC(qt->next->flags)) return 1;
    if(!compatible_types(zt->next,qt->next,(c_flags[7]&USEDFLAG)?NU:NQ)){
      if(!ecpp){
        error(85);
      }
#ifdef HAVE_ECPP
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
    }else{
      if((qt->next->flags&CONST)&&!(zt->next->flags&CONST))
        error(91);
      if((qt->next->flags&VOLATILE)&&!(zt->next->flags&VOLATILE))
        error(100);
      if((qt->next->flags&RESTRICT)&&!(zt->next->flags&RESTRICT))
        error(298);
      if(qt->next->next&&zt->next->next&&!compatible_types(zt->next->next,qt->next->next,NU))
        error(110);
      }
    return 1;
  }
  if(ISPOINTER(zt->flags)&&q->flags==CEXPR){
    eval_constn(q);
    if(!(zldeqto(d2zld(0.0),vldouble)&&zmeqto(l2zm(0L),vmax)&&zumeqto(ul2zum(0UL),vumax)))
      error(113);
    return 1;
  }
  error(39);
  return 0;
}
#ifdef HAVE_ECPP
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
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif

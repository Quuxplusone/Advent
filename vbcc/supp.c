/*  $VER: vbcc (supp.c) $Revision: 1.47 $     */

#include "supp.h"
#include "opt.h"

static char FILE_[]=__FILE__;

#ifndef HAVE_EXT_TYPES
char *typname[]={"strange","char","short","int","long","long long",
                 "float","double","long double","void",
                 "pointer","array","struct","union","enum","function",
                 "bool","vector"};
#endif

char *storage_class_name[]={"strange","auto","register","static","extern","typedef"};

char *ename[]={"strange","sequence","move","set+","set-","set*","set/","set%",
               "set&","set^","set|","set<<","set>>","?:","lor","land","or",
               "eor","and","eq","neq","lt","le","gt","ge","lsl",
               "lsr","add","sub","mul","div","mod","negate",
               "not","preinc","postinc","predec","postdec","neg",
               "dref-pointer","address-of","cast","call","index",
               "dref-struct-pointer","dref-struct","identifier","constant",
               "string","member",
                "convert","convert-short","convert-int","convert-long",
                "convert-float","convert-double","convert-void","convert-pointer",
                "convert-uchar","convert-ushort","convert-uint","convert-ulong",
                "address-of-array","first-element-of-array","pmult",
                "allocreg","freereg","pconstant","test","label","beq","bne",
                "blt","bge","ble","bgt","bra","compare","push","pop",
                "address-of-struct","add-int-to-pointer","sub-int-from-pointer",
                "sub-pointer-from-pointer","push-reg","pop-reg","pop-args",
                "save-regs","restore-regs","identifier-label","dc","align",
                "colon","get-return","set-return","move-from-reg","move-to-reg",
                "nop","bitfield"};

char *empty="";
zchar vchar; zuchar vuchar;
zshort vshort; zushort vushort;
zint vint; zuint vuint;
zlong vlong; zulong vulong;
zllong vllong; zullong vullong;
zmax vmax; zumax vumax;
zfloat vfloat; zdouble vdouble;
zldouble vldouble;

union atyps gval;

#ifndef DEBUG
int DEBUG;
#endif

int label;

int regs[MAXR+1],regused[MAXR+1],simple_scratch[MAXR+1];
Var *regsv[MAXR+1];
int goto_used;
int ic_count;
zmax max_offset;
int function_calls,vlas;
int coloring;
int dmalloc;
int disable;
int multiple_ccs;
int lastlabel,return_label;
int only_inline;
IC *err_ic;
long maxoptpasses=30;
long optflags;
int optsize,optspeed,unroll_all,stack_check;
int cross_module,final,no_emit;
int debug_info;
long inline_size=100;
long inline_depth=1;
long unroll_size=200;
long clist_copy_stack=6;
long clist_copy_static=6;
long clist_copy_pointer=20;
long inline_memcpy_sz=INLINEMEMCPY;
int fp_assoc,noaliasopt,noitra;
char *filename;
IC *first_ic,*last_ic;
int float_used;
bvtype regs_modified[RSIZE/sizeof(bvtype)];
/*  Das haette ich gern woanders    */
Var *vl0,*vl1,*vl2,*vl3;
int align_arguments=1;
zmax stackalign;
int misracheck,misraversion,misracomma,misratok;
int pack_align;
int short_push;
int default_unsigned;

char *emit_buffer[EMIT_BUF_DEPTH];
char *emit_p;
int emit_f,emit_l;
int no_inline_peephole;

static int get_scalar_byte(int f,union atyps *v,zmax n, zuchar *out)
/* Yields byte n of a constant */
{
  if(!zmleq(l2zm(0L),n)) ierror(0);
  f&=NQ;
  if(f>=CHAR&&f<=LLONG){
    int j;
    eval_const(v,f);
    if(LITTLEENDIAN)
      j=zm2l(n);
    else
      j=(int)sizetab[f]-(int)zm2l(n)-1;
    while(j){
      vumax=zumrshift(vumax,char_bit);
      j--;
    }
    *out=zum2zuc(vumax);
    return 1;
  }else
    return 0;
}

zumax get_clist_int(type *t, const_list *cl, zmax n, int sz,int *state)
/* yields an integer of size sz from offset n */
{
  zuchar zuc;
  zumax val = ZU0;

  if(LITTLEENDIAN)
    n=zmadd(n,l2zm((long)(sz-1)));
  
  while(sz){
    val=zumlshift(val,char_bit);
    if(!get_clist_byte(t,cl,n,&zuc)) {*state=0;return ZU0;}
    val=zumadd(val,zuc2zum(zuc));
    sz--;
    n=LITTLEENDIAN?zmsub(n,Z1):zmadd(n,Z1);
  }
  *state=1;
  return val;
}

int get_clist_byte(type *t,const_list *cl, zmax n, zuchar *out)
/* Yields byte n from a const list */
{
  if(!cl){
    *out=zum2zuc(ul2zum(0UL));
    return 1;
  }

  if(ISARRAY(t->flags)){
    zmax i,j,x;
    x=szof(t->next);
    i=zmdiv(n,x);
    while(1){
      if(!cl) break;
      if(zmleq(i,cl->idx)) break;
      cl=cl->next;
    }
    if(!cl||!zmeqto(cl->idx,i)){
      *out=zum2zuc(ul2zum(0UL));
      return 1;
    }
    return get_clist_byte(t->next,cl->other,zmmod(n,x),out);
  }
  if(ISUNION(t->flags)){
    int i=zm2l(cl->idx);
    if(cl->tree) return 0;
    return get_clist_byte((*t->exact->sl)[i].styp,cl->other,n,out);
  }
  if(ISSTRUCT(t->flags)){
    zmax al;int fl;type *st;
    int i,bfo,bfs;zmax sz;zumax bfval=ul2zum(0UL);

    sz=l2zm(0L);
    if(cl&&cl->tree){
      /* initialized by another */
      return 0;
    }else{
      for(i=0;i<t->exact->count&&cl;i++){
	if(!cl->other){ierror(0);}
	st=(*t->exact->sl)[i].styp;
	al=(*t->exact->sl)[i].align;
	if(!(*t->exact->sl)[i].identifier) ierror(0);
	bfo=(*t->exact->sl)[i].bfoffset;
	if(!zmeqto(zmmod(sz,al),l2zm(0L))){
	  sz=zmadd(sz,zmsub(al,zmmod(sz,al)));
	  if(!zmleq(sz,n)){
	    *out=zum2zuc(ul2zum(0L));
	    return 1;
	  }
	}
	if(bfo>=0){
	  /* bitfield */

	  if((*t->exact->sl)[i].identifier[0]){
	    bfs=(*t->exact->sl)[i].bfsize;
	    if(zmeqto(l2zm((long)i),cl->idx)){
	      eval_const(&cl->other->val,st->flags);
	      cl=cl->next;
	    }else{
	      vumax=ul2zum(0UL);
	    }
	    vumax=zumand(vumax,zumsub(zumlshift(ul2zum(1UL),ul2zum((unsigned long)bfs)),ul2zum(1UL)));
	    bfval=zumor(bfval,zumlshift(vumax,ul2zum((unsigned long)bflayout(bfo,bfs,st->flags))));
	  }          
	  if(i+1>=t->exact->count||(*t->exact->sl)[i+1].bfoffset<=0||!cl){
	    /* last bitfield in integer */
	    gval.vumax=bfval;
	    eval_const(&gval,UNSIGNED|MAXINT);
	    insert_const(&gval,st->flags&NU);
	    bfval=ul2zum(0L);
	    sz=zmadd(sz,szof(st));
	    if(!zmleq(sz,n))
	      return get_scalar_byte(st->flags,&gval,zmsub(n,zmsub(sz,szof(st))),out);
	  }
	}else{
	  sz=zmadd(sz,szof(st));
	  if(!zmleq(sz,n)){
	    if(zmeqto(l2zm((long)i),cl->idx)){
	      return get_clist_byte(st,cl->other,zmsub(n,zmsub(sz,szof(st))),out);
	    }else{
	      *out=zum2zuc(ul2zum(0UL));
	      return 1;
	    }
	  }
	  if(zmeqto(l2zm((long)i),cl->idx))
	    cl=cl->next;
	}
      }
    }
    *out=zum2zuc(ul2zum(0UL));
    return 1;
  }

  if(zmeqto(cl->idx,l2zm(-1L)))
    return 0;
  else{
    if(cl->tree)
      return 0;
    else
      return get_scalar_byte(t->flags,&cl->val,n,out);

  }
  ierror(0);
}

type *new_typ(void)
/*  Erzeigt neuen (leeren) Typ.  */
{
  type *new=mymalloc(TYPS);
  new->flags=0;
  new->next=0;
  new->exact=0;
  new->attr=0;
  new->dsize=0;
  new->reg=0;
#ifdef HAVE_ECPP
/* removed */
#endif
  return new;
}

type *clone_typ(type *old)
/*  Erzeugt Kopie eines Typs und liefert Zeiger auf Kopie.  */
{
  type *new;
  if(!old) return 0;
  new=new_typ();
  *new=*old;
  if(old->attr){
    new->attr=mymalloc(strlen(old->attr)+1);
    strcpy(new->attr,old->attr);
  }
  if(new->next) new->next=clone_typ(new->next);
  return new;
}
Var *new_var(void)
{
  Var *new=mymalloc(sizeof(*new));
  new->clist=0;
  new->vtyp=0;
  new->storage_class=0;
  new->reg=0;
  new->vattr=0;
  new->next=0;
  new->flags=0;
  new->fi=0;
  new->nesting=0;
  new->filename=0;
  new->line=0;
  new->dfilename=0;
  new->dline=0;
  new->description=0;
  new->tunit=0;
  new->offset=l2zm(0L);
  new->identifier=0;
#ifdef HAVE_TARGET_ATTRIBUTES
  new->tattr=0;
#endif 
#ifdef ALEX_REG
  new->iRegCopyNr = -1;
  new->iRegCopyNrHc12V = -1;
#endif
  return new;
}  

IC *new_IC(void)
{
  IC *p=mymalloc(ICS);
  p->change_cnt=0;
  p->use_cnt=0;
  p->call_cnt=0;
  p->arg_cnt=0;
  p->use_list=0;
  p->change_list=0;
  p->call_list=0;
  p->arg_list=0;
  p->line=0;
  p->file=0;
  p->flags=0;
  p->q1.flags=p->q2.flags=p->z.flags=0;
  p->q1.am=p->q2.am=p->z.am=0;
  p->q1.val.vmax=p->q2.val.vmax=p->z.val.vmax=l2zm(0L);
  p->savedsp=0;
  p->typf=p->typf2=0;
#ifdef ALEX_REG
  p->iZWebIndex = -1;
  p->iQ1WebIndex = -1;
  p->iQ2WebIndex = -1;
  p->pFlow = NULL;
#endif /* GC_RALLOC */

  return p;
}
/* (partially) clones an IC list */
IC *clone_ic(IC *p)
{
  IC *new,*first,*last;
  first=last=new=0;
  while(p){
    new=mymalloc(sizeof(*new));
    *new=*p;
    p->copy=new;

    if(p->q1.am||p->q2.am||p->z.am) ierror(0);
    
    if(new->code==CALL){
      int i;
      new->arg_list=mymalloc(sizeof(*new->arg_list)*new->arg_cnt);
      for(i=0;i<new->arg_cnt;i++){
	if(!p->arg_list[i])
	  ierror(0);
	if(!p->arg_list[i]->copy)
	  ierror(0);
        new->arg_list[i]=p->arg_list[i]->copy;
      }
    }
    new->prev=last;
    new->next=0;
    if(!first) first=new;
    if(last) last->next=new;
    last=new;
    p=p->next;
  }
  return first;
}    
void free_IC(IC *p)
/*  Gibt IC-Liste inkl. Typen frei.                 */
{
  IC *merk;
  if(DEBUG&1) printf("free_IC()\n");
  while(p){
    /*if(p->q1.am&&!p->q1.flags) ierror(0);
    if(p->q2.am&&!p->q2.flags) ierror(0);
    if(p->z.am&&!p->z.flags) ierror(0);*/

    if(p->q1.am&&p->q1.am==p->q2.am)
      ierror(0);

    if(p->q1.am) free(p->q1.am);
    if(p->q2.am) free(p->q2.am);
    if(p->z.am) free(p->z.am);
    if(p->code==CALL) free(p->arg_list);
    merk=p->next;
    free(p);
    p=merk;
  }
}
void move_IC(IC *after,IC *p)
{
  if(p->prev)
    p->prev->next=p->next;
  else
    first_ic=p->next;
  if(p->next)
    p->next->prev=p->prev;
  else
    last_ic=p->prev;
  insert_IC(after,p);
}
void remove_IC(IC *p)
/*  Entfernt IC p aus Liste. */
{
  if(p->prev) p->prev->next=p->next; else first_ic=p->next;
  if(p->next) p->next->prev=p->prev; else last_ic=p->prev;
  if(p->q1.am) free(p->q1.am);
  if(p->q2.am) free(p->q2.am);
  if(p->z.am) free(p->z.am);
  free(p);
}
void freetyp(type *p)
/* Gibt eine Typ-Liste frei, aber keine struct_declaration oder so. */
{
  int f;type *merk;
  if(DEBUG&8){printf("freetyp: ");prd(stdout,p);printf("\n");}
  while(p){
    merk=p->next;
    f=p->flags&NQ;
    if(merk&&!ISARRAY(f)&&!ISPOINTER(f)&&!ISFUNC(f)&&!ISVECTOR(f)) 
      ierror(0);
    free(p->attr);
    free(p);
    p=merk;
  }
}

#ifndef HAVE_TGT_FALIGN
zmax falign(type *t)
/*  Liefert Alignment eines Typs. Funktioniert im Gegensatz zum  */
/*  align[]-Array auch mit zusammengesetzten Typen.              */
{
  int i,f; zmax al,alt;
  f=t->flags&NQ;
  if(ISVECTOR(f)) return szof(t);
  al=align[f];
  if(ISSCALAR(f)) return al;
  if(ISARRAY(f)){
    do{ 
      t=t->next; 
      f=t->flags&NQ;
    }while(ISARRAY(f)||ISVECTOR(f));
    alt=falign(t);
    if(zmleq(al,alt)) return alt; else return al;
  }
  if(ISUNION(f)||ISSTRUCT(f)){
    if(!t->exact) ierror(0);
    for(i=0;i<t->exact->count;i++){
      alt=(*t->exact->sl)[i].align;
      if(!zmleq(alt,al)) al=alt;
    }
  }
  return al;
}
#endif

/* check, whether t is a variable length array */
int is_vlength(type *t)
{
  if(!ISARRAY(t->flags))
    return 0;
  if(t->dsize)
    return 1;
  else 
    return is_vlength(t->next);
}

/* calculate size of a variable length array */
Var *vlength_szof(type *t)
{
  IC *new;type *nt;
  if(!ISARRAY(t->flags))
    ierror(0);
  new=new_IC();
  new->code=MULT;
  new->typf=HAVE_INT_SIZET?(UNSIGNED|INT):(UNSIGNED|LONG);
  if(t->dsize){
    new->q1.flags=VAR;
    new->q1.v=t->dsize;
    new->q1.val.vmax=l2zm(0L);
  }else{
    new->q1.flags=KONST;
#if HAVE_INT_SIZET
    new->q1.val.vuint=zum2zui(zm2zum(t->size));
#else
    new->q1.val.vulong=zum2zul(zm2zum(t->size));
#endif
  }
  new->z.flags=VAR;
  nt=new_typ();
  nt->flags=new->typf;
  new->z.v=add_tmp_var(nt);
  new->z.val.vmax=l2zm(0L);
  if(is_vlength(t->next)){
    new->q2.flags=VAR;
    new->q2.v=vlength_szof(t->next);
    new->q2.val.vmax=l2zm(0L);
  }else{
    new->q2.flags=KONST;
#if HAVE_INT_SIZET
    new->q2.val.vuint=zum2zui(zm2zum(szof(t->next)));
#else
    new->q2.val.vulong=zum2zul(zm2zum(szof(t->next)));
#endif
  }
  add_IC(new);
  return new->z.v;
}

/* return the type of a d-dim vector of base type x */
int mkvec(int x,int d)
{
  int t=x&NQ,r;
  if(d!=2&&d!=3&&d!=4&&d!=8&&d!=16) ierror(0);
  switch(t){
  case BOOL:
    r=VECBOOL;break;
  case CHAR:
    r=VECCHAR;break;
  case SHORT:
    r=VECSHORT;break;
  case INT:
    r=VECINT;break;
  case LONG:
    r=VECLONG;break;
  case FLOAT:
    r=VECFLOAT;break;
  default:
    ierror(0);
  }
  return (r+d-1)|(x&~NQ);
}

/* return the base type of a vector */
int VECTYPE(int x)
{
  int t=x&NQ,r=0;
  if(t>=VECBOOL&&t<=VECBOOL+MAXVECDIM)
    r=BOOL;
  if(t>=VECCHAR&&t<=VECCHAR+MAXVECDIM)
    r=CHAR;
  if(t>=VECSHORT&&t<=VECSHORT+MAXVECDIM)
    r=SHORT;
  if(t>=VECINT&&t<=VECINT+MAXVECDIM)
    r=INT;
  if(t>=VECLONG&&t<=VECLONG+MAXVECDIM)
    r=LONG;
  if(t>=VECFLOAT&&t<=VECFLOAT+MAXVECDIM)
    r=FLOAT;
  if(!r) ierror(0);
  return r|(x&~NQ);
}


#ifndef HAVE_TGT_SZOF
zmax szof(type *t)
/*  Liefert die benoetigte Groesse eines Typs in Bytes.     */
{
  int i=t->flags&NQ,j;zmax size,m;
#ifdef HAVE_ECPP
/* removed */
#endif

  if(ISSCALAR(i)) return sizetab[i];
  if(ISARRAY(i)){
    if(is_vlength(t))
      return sizetab[POINTER_TYPE(t->next)];
    size=zmmult((t->size),szof(t->next));
    m=align[ARRAY];
    return zmmult(zmdiv(zmadd(size,zmsub(m,l2zm(1L))),m),m); /* align */
  }
  if(ISVECTOR(i)){
    zmax dim=VECDIM(i);
    if(zmeqto(dim,l2zm(3L))) dim=l2zm(4L);
    return zmmult(dim,sizetab[VECTYPE(i)&NQ]);
  }
  if(ISUNION(i)){
    for(j=0,size=l2zm(0L);j<t->exact->count;j++){
      m=szof((*t->exact->sl)[j].styp);
      if(zmeqto(m,l2zm(0L))) return l2zm(0L);
      if(!zmleq(m,size)) size=m;
    }
    m=falign(t);
    return zmmult(zmdiv(zmadd(size,zmsub(m,l2zm(1L))),m),m); /* align */
  }
  if(ISSTRUCT(i)){
    size=l2zm(0L);
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
    for(j=0;j<t->exact->count;j++){
      type *h=(*t->exact->sl)[j].styp;
      if((*t->exact->sl)[j].bfoffset<=0){
	int n;size_t al;
        al=(*t->exact->sl)[j].align;
#ifdef HAVE_ECPP
/* removed */
#endif
        if(zmeqto(al,l2zm(0L))) {prd(stdout,h);ierror(0);}
        m=szof(h);
	/* find the largest type in a bitfield */
	for(n=j+1;n<t->exact->count&&(*t->exact->sl)[n].bfoffset>0;n++){
	  size_t tmp=szof((*t->exact->sl)[n].styp);
	  if(zmleq(m,tmp))
	    m=tmp;
	}
        size=zmmult(zmdiv(zmadd(size,zmsub(al,l2zm(1L))),al),al);
        /*if(zmeqto(m,l2zm(0L))) return l2zm(0L);*/
        size=zmadd(size,m);
      }
    }
    m=falign(t);
    return zmmult(zmdiv(zmadd(size,zmsub(m,l2zm(1L))),m),m); /* align */
  }
  return sizetab[i];
}
zmax struct_offset(struct_declaration *sd,const char *identifier)
{
  int i=0,intbitfield=-1;zmax offset=l2zm(0),al;
  while(i<sd->count&&strcmp((*sd->sl)[i].identifier,identifier)){
    if((*sd->sl)[i].bfoffset>=0){
      if(i+1<sd->count&&(*sd->sl)[i+1].bfoffset>0){
        i++;
        continue;
      }
    }
    al=(*sd->sl)[i].align;
    offset=zmmult(zmdiv(zmadd(offset,zmsub(al,l2zm(1L))),al),al);
    offset=zmadd(offset,szof((*sd->sl)[i].styp));
    i++;
  }
  if(i>=sd->count) {error(23,identifier);return l2zm(0L);}
  al=(*sd->sl)[i].align;
  offset=zmmult(zmdiv(zmadd(offset,zmsub(al,l2zm(1L))),al),al);
  return offset;
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
#endif

#ifndef HAVE_TGT_PRINTVAL
void printval(FILE *f,union atyps *p,int t)
/*  Gibt atyps aus.                                     */
{
  t&=NU;
  if(t==CHAR){fprintf(f,"C");vmax=zc2zm(p->vchar);printzm(f,vmax);}
  if(t==(UNSIGNED|CHAR)){fprintf(f,"UC");vumax=zuc2zum(p->vuchar);printzum(f,vumax);}
  if(t==SHORT){fprintf(f,"S");vmax=zs2zm(p->vshort);printzm(f,vmax);}
  if(t==(UNSIGNED|SHORT)){fprintf(f,"US");vumax=zus2zum(p->vushort);printzum(f,vumax);}
  if(t==FLOAT){fprintf(f,"F");vldouble=zf2zld(p->vfloat);printzld(f,vldouble);}
  if(t==DOUBLE){fprintf(f,"D");vldouble=zd2zld(p->vdouble);printzld(f,vldouble);}
  if(t==LDOUBLE){fprintf(f,"LD");printzld(f,p->vldouble);}
  if(t==INT){fprintf(f,"I");vmax=zi2zm(p->vint);printzm(f,vmax);}
  if(t==(UNSIGNED|INT)){fprintf(f,"UI");vumax=zui2zum(p->vuint);printzum(f,vumax);}
  if(t==LONG){fprintf(f,"L");vmax=zl2zm(p->vlong);printzm(f,vmax);}
  if(t==(UNSIGNED|LONG)){fprintf(f,"UL");vumax=zul2zum(p->vulong);printzum(f,vumax);}
  if(t==LLONG){fprintf(f,"LL");vmax=zll2zm(p->vllong);printzm(f,vmax);}
  if(t==(UNSIGNED|LLONG)){fprintf(f,"ULL");vumax=zull2zum(p->vullong);printzum(f,vumax);}
  if(t==MAXINT){fprintf(f,"M");printzm(f,p->vmax);}
  if(t==(UNSIGNED|MAXINT)){fprintf(f,"UM");printzum(f,p->vumax);}
  /*FIXME*/
  if(t==POINTER){fprintf(f,"P");vumax=zul2zum(p->vulong);printzum(f,vumax);}
}
void emitval(FILE *f,union atyps *p,int t)
/*  Gibt atyps aus.                                     */
{
  t&=NU;
  if(t==CHAR){vmax=zc2zm(p->vchar);emitzm(f,vmax);}
  if(t==(UNSIGNED|CHAR)){vumax=zuc2zum(p->vuchar);emitzum(f,vumax);}
  if(t==SHORT){vmax=zs2zm(p->vshort);emitzm(f,vmax);}
  if(t==(UNSIGNED|SHORT)){vumax=zus2zum(p->vushort);emitzum(f,vumax);}
  if(t==FLOAT){vldouble=zf2zld(p->vfloat);emitzld(f,vldouble);}
  if(t==DOUBLE){vldouble=zd2zld(p->vdouble);emitzld(f,vldouble);}
  if(t==LDOUBLE){emitzld(f,p->vldouble);}
  if(t==INT){vmax=zi2zm(p->vint);emitzm(f,vmax);}
  if(t==(UNSIGNED|INT)){vumax=zui2zum(p->vuint);emitzum(f,vumax);}
  if(t==LONG){vmax=zl2zm(p->vlong);emitzm(f,vmax);}
  if(t==(UNSIGNED|LONG)){vumax=zul2zum(p->vulong);emitzum(f,vumax);}
  if(t==LLONG){vmax=zll2zm(p->vllong);emitzm(f,vmax);}
  if(t==(UNSIGNED|LLONG)){vumax=zull2zum(p->vullong);emitzum(f,vumax);}
  if(t==MAXINT){emitzm(f,p->vmax);}
  if(t==(UNSIGNED|MAXINT)){emitzum(f,p->vumax);}
  /*FIXME*/
  if(t==POINTER){vumax=zul2zum(p->vulong);emitzum(f,vumax);}
}
#endif

void pric2(FILE *f,IC *p)
/*  Gibt ein IC aus.  */
{
  if(p->code>NOP) ierror(0);
  if(p->next&&p->next->prev!=p) ierror(0);
  if(p->code>=LABEL&&p->code<=BRA){
    if(p->code==LABEL)
      fprintf(f,"L%d",p->typf);
    else{
      fprintf(f,"\t%s L%d",ename[p->code],p->typf);
      if(p->q1.flags){ fprintf(f,",");probj(f,&p->q1,0);}
    }
    if(p->code==LABEL&&(p->flags&LOOP_COND_TRUE)) fprintf(f," (while-loop)");
    if(p->code==BRA&&(p->flags&LOOP_COND_TRUE)) fprintf(f," (to-loop-test)");
  }else{
    fprintf(f,"\t%s ",ename[p->code]);
    if(p->typf&VOLATILE) fprintf(f,"volatile ");
    if(p->typf&CONST) fprintf(f,"const ");
    if(p->typf&UNSIGNED) fprintf(f,"unsigned ");
    if(p->typf){
      if(ISVECTOR(p->typf))
	fprintf(f,"%s%d ",typname[VECTYPE(p->typf)&NQ],VECDIM(p->typf));
      else
	fprintf(f,"%s ",typname[p->typf&NQ]);
    }
    probj(f,&p->q1,q1typ(p));
    if(p->q2.flags){fprintf(f,",");probj(f,&p->q2,q2typ(p));}
    if(p->z.flags){fprintf(f,"->");probj(f,&p->z,ztyp(p));}
    if(p->code==ASSIGN||p->code==PUSH||p->code==POP||p->code==CALL)
      fprintf(f," size=%ld",zm2l(p->q2.val.vmax));
    if((p->code==SAVEREGS||p->code==RESTOREREGS)&&p->q1.reg)
      fprintf(f," except %s",regnames[p->q1.reg]);
    if(p->code==CONVERT)
      fprintf(f," from %s%s",(p->typf2&UNSIGNED)?"unsigned ":"",typname[p->typf2&NQ]);
    if(p->code==LSHIFT||p->code==RSHIFT)
      fprintf(f," shift-type %s%s",(p->typf2&UNSIGNED)?"unsigned ":"",typname[p->typf2&NQ]);
    if(p->code==ADDI2P||p->code==SUBIFP||p->code==SUBPFP||p->code==ADDRESS)
      fprintf(f," ptype=%s%s",(p->typf2&UNSIGNED)?"unsigned ":"",typname[p->typf2&NQ]);
    if(p->code==ASSIGN||p->code==PUSH)
      if(p->typf2) fprintf(f," align=%d\n",p->typf2);
  }
  if(p->code==CALL){
    fprintf(f," =>");
    if(p->call_cnt==0)
      fprintf(f,"(unknown)");
    else{
      int i;
      for(i=0;i<p->call_cnt;i++)
        fprintf(f," %s",p->call_list[i].v->identifier);
    }
    
  }
  if(p->flags&EFF_IC) fprintf(f," (eff_ic)");
  fprintf(f,"\n");
#if 0
  if(p->code==CALL){
    int i;
    //fprintf(f,"c=%p\n",p);
    for(i=0;i<p->arg_cnt;i++){
      //fprintf(f,"%p!\n",p->arg_list[i]);
      fprintf(f,"%02d:",i);
      pric2(f,p->arg_list[i]);
    }
  }
#endif
}
void pric(FILE *f,IC *p)
/*  Gibt IC-Liste auf dem Bildschirm aus.             */
{
  while(p){
    pric2(f,p);
/*        if(p->q1.am||p->q2.am||p->z.am) ierror(0);*/
    p=p->next;
  }
}
void printzm(FILE *f,zmax x)
/*  Konvertiert zmax nach ASCII.                       */
/*  Basiert noch einigermassen auf                      */
/*  Zweierkomplementdarstellung (d.h. -MIN>MAX).        */
/*  Ausserdem muss max(abs(max))<=max(unsigned max).  */
{
  zmax zm;zumax zum;
  zm=l2zm(0L);
  if(zmleq(x,zm)&&!zmeqto(x,zm)){
    fprintf(f,"-");zm=zum2zm(t_max(MAXINT));
    if(zmleq(x,zmsub(l2zm(0L),zm))&&!zmeqto(x,zmsub(l2zm(0L),zm))){
      /*  aufpassen, da -x evtl. >LONG_MAX    */
      zum=t_max(MAXINT);
      x=zmadd(x,zm);
    }else 
      zum=ul2zum(0UL);
    x=zmsub(l2zm(0L),x);
    vumax=zm2zum(x);
    zum=zumadd(zum,vumax);
  }else
    zum=zm2zum(x);
  printzum(f,zum);
}
void printzum(FILE *f,zumax x)
/*  Konvertiert zumax nach ASCII.                      */
{
  zumax zum;unsigned long l;
  zum=ul2zum(10UL);
  if(!zumeqto(zumdiv(x,zum),ul2zum(0UL))) printzum(f,zumdiv(x,zum));
  zum=zummod(x,zum);l=zum2ul(zum);
  fprintf(f,"%c",(int)(l+'0'));
}

void printzld(FILE *f,zldouble x)
/*  Konvertiert zdouble nach ASCII, noch nicht fertig.  */
{
  fprintf(f,"fp-constant");
}
void emitzm(FILE *f,zmax x)
/*  Konvertiert zmax nach ASCII.                       */
/*  Basiert noch einigermassen auf                      */
/*  Zweierkomplementdarstellung (d.h. -MIN>MAX).        */
/*  Ausserdem muss max(abs(max))<=max(unsigned max).  */
{
  zmax zm;zumax zum;
  zm=l2zm(0L);
  if(zmleq(x,zm)&&!zmeqto(x,zm)){
    emit(f,"-");zm=zum2zm(t_max(MAXINT));
    if(zmleq(x,zmsub(l2zm(0L),zm))&&!zmeqto(x,zmsub(l2zm(0L),zm))){
      /*  aufpassen, da -x evtl. >LONG_MAX    */
      zum=t_max(MAXINT);
      x=zmadd(x,zm);
    }else 
      zum=ul2zum(0UL);
    x=zmsub(l2zm(0L),x);
    vumax=zm2zum(x);
    zum=zumadd(zum,vumax);
  }else
    zum=zm2zum(x);
  emitzum(f,zum);
}
void emitzum(FILE *f,zumax x)
/*  Konvertiert zumax nach ASCII.                      */
{
  zumax zum;unsigned long l;
  zum=ul2zum(10UL);
  if(!zumeqto(zumdiv(x,zum),ul2zum(0UL))) emitzum(f,zumdiv(x,zum));
  zum=zummod(x,zum);l=zum2ul(zum);
  emit(f,"%c",(int)(l+'0'));
}

void emitzld(FILE *f,zldouble x)
/*  Konvertiert zdouble nach ASCII, noch nicht fertig.  */
{
  emit(f,"fp-constant");
}

typedef struct memblock {struct memblock *next;void *p;} memblock;
static memblock *first_mb;

static void add_mb(void *p)
{
  memblock *mb_second=first_mb;
  memblock *mb=malloc(sizeof(*mb));
  if(!mb){
    error(12);
    raus();
  }
  first_mb=mb;
  mb->next=mb_second;
  mb->p=p;
}  

static void remove_mb(void *p)
{
  memblock *mb_prev=0;
  memblock *mb=first_mb;
  while(mb){
    if(mb->p==p){
      if(mb_prev==0) first_mb=mb->next;
      else mb_prev->next=mb->next;
      (free)(mb);
      return;
    }
    mb_prev=mb;
    mb=mb->next;
  }
  ierror(0);
}

void *mymalloc(size_t size)
/*  Allocate memory and quit on failure.  */
{
  void *p;
  if(dmalloc)
    size+=sizeof(size);
  else if(size==0)
    /* Not very nice, but simplest way to avoid a failure when size==0. */
    size=1;
  if(!(p=malloc(size))){
    error(12);
    raus();
  }
  if(DEBUG&32768){
    printf("malloc %p (s=%lu)\n",p,(unsigned long)size);
    fflush(stdout);
  }
  if(DEBUG&65536) add_mb(p);
  if(dmalloc){
    *(size_t *)p=size;
    p=((char *)p)+sizeof(size);
    memset(p,0xaa,size-sizeof(size));
  } 
  return p;
}

void *myrealloc(void *p,size_t size)
/*  Reallocate memory and quit on failure.  */
{
  void *new;
  if(!p) return mymalloc(size);
  if(dmalloc){
    size+=sizeof(size);
    new=realloc(((char *)p)-sizeof(size),size);
    if(!new){
      error(12);
      raus();
    }
    if(DEBUG&32768){
      printf("realloc %p to %p (s=%lu)\n",p,new,(unsigned long)size);
      fflush(stdout);
    }
    if(DEBUG&65536){
      remove_mb(p);
      add_mb(new);
    }
    *(size_t *)new=size;
    new=((char *)new)+sizeof(size);
    return new;
  }else{
    new=realloc(p,size);
    if(!new){
      error(12);
      raus();
    }
    if(DEBUG&32768){
      printf("realloc %p to %p (s=%lu)\n",p,new,(unsigned long)size);
      fflush(stdout);
    }
    if(DEBUG&65536){
      remove_mb(p);
      add_mb(new);
    }
    return new;
  }
}

void myfree(void *p)
{
  if(p&&dmalloc){
    p=((char*)p)-sizeof(size_t);
    memset(p,0xbb,*(size_t *)(p));
  }
  if(DEBUG&32768){
    printf("free %p\n",p);
    fflush(stdout);
  }
  if((DEBUG&65536)&&p) remove_mb(p);
  (free)(p);  /* supp.h has #define free(x) myfree(x) */
}

char *mystrdup(char *p)
{
  char *new=mymalloc(strlen(p)+1);
  strcpy(new,p);
  return new;
}

/* Testet, ob zwei objs dieselben Register belegen. */
int collides(obj *x,obj *y)
{
    int x1,x2,y1,y2;
    if(!(x->flags&REG)||!(y->flags&REG)) return 0;
    if(reg_pair(x->reg,&rp)){
        x1=rp.r1;x2=rp.r2;
    }else{
        x1=x->reg;x2=-1;
    }
    if(reg_pair(y->reg,&rp)){
        y1=rp.r1;y2=rp.r2;
    }else{
        y1=y->reg;y2=-2;
    }
    if(x1==y1||x1==y2||x2==y1||x2==y2)
        return 1;
    else
        return 0;
}

/* Versucht, ein IC so zu drehen, dass q2 und z kein gemeinsames */
/* Register belegen. Liefert Null, wenn das nicht moeglich ist.  */
int switch_IC(IC *p)
{
    int c;
    obj o;
    if(!collides(&p->q2,&p->z)) return 1;
    c=p->code;
    if((c<OR||c>AND)&&c!=ADD&&c!=MULT&&c!=ADDI2P) return 0;
    if(c==ADDI2P&&must_convert(q1typ(p),q2typ(p),0)) return 0;
    if(collides(&p->q1,&p->z)) return 0;
    o=p->q2;p->q2=p->q1;p->q1=o;
    return 1;
}

void probj(FILE *f,obj *p,int t)
/*  Gibt Objekt auf Bildschirm aus.                    */
{
  if(p->am){ fprintf(f,"[tgt-addressing-mode]");return;}
  if(p->flags&DREFOBJ){
    fprintf(f,"([");
    if(p->dtyp&CONST) fprintf(f,"const ");
    if(p->dtyp&VOLATILE) fprintf(f,"volatile ");
    if(p->dtyp&PVOLATILE) fprintf(f,"pvolatile ");
    fprintf(f,"%s]",typname[p->dtyp&NQ]);
  }
  if(p->flags&VARADR) fprintf(f,"#");
  if(p->flags&VAR) {
    if(!(p->flags&REG))
      printval(f,&p->val,MAXINT);
    if(p->flags&REG){
        fprintf(f,"%s",regnames[p->reg]);
    }else if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
        fprintf(f,"+%ld(FP)", zm2l(p->v->offset));
    }else{
      if(p->v->storage_class==STATIC){
        fprintf(f,"+L%ld",zm2l(p->v->offset));
      }else{
        fprintf(f,"+_%s",p->v->identifier);
      }
    }
    if(*p->v->identifier)
      fprintf(f,"(%s)",p->v->identifier);
    else if(p->v->description)
      fprintf(f,"(%s)",p->v->description);
    else
      fprintf(f,"(%p)",(void *)p->v);
    if(p->v->reg) fprintf(f,":%s",regnames[abs(p->v->reg)]);
  }
  if((p->flags&REG)&&!(p->flags&VAR)) fprintf(f,"%s",regnames[p->reg]);
  if(p->flags&KONST){
    fprintf(f,"#");
    if(p->flags&DREFOBJ)
      printval(f,&p->val,p->dtyp&NU);
    else
      printval(f,&p->val,t&NU);
  }
  if(p->flags&SCRATCH) fprintf(f,"[S]");
  if(p->flags&DREFOBJ) fprintf(f,")");
}
void prl(FILE *o,struct_declaration *p)
/* Gibt eine struct_declaration auf dem Bildschirm aus. */
{
  int i;
  static int recurse=2;
  int merk_recurse=recurse;
  --recurse;
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
#endif
  for(i=0;i<p->count;i++){
    fprintf(o," %d.:",i);
    if(recurse>=0){
      if((*p->sl)[i].identifier)fprintf(o," ident: %s; ",(*p->sl)[i].identifier);
      prd(o,(*p->sl)[i].styp);
    }
  }
  recurse=merk_recurse;
}
void prd(FILE *o,type *p)
/* Gibt einen Typ auf dem Bildschirm aus.    */
{
  int f;
  if(!p) {fprintf(o,"empty type ");return;}
    f=p->flags;
/*    fprintf(o,"(Sizeof=%ld,flags=%d)",zl2l(szof(p)),f);*/
/*    if(type_uncomplete(p)) {fprintf(o,"incomplete ");}*/
  if(f&CONST) {fprintf(o,"const ");f&=~CONST;}
  if(f&STRINGCONST) {fprintf(o,"string-const ");f&=~STRINGCONST;}
  if(f&VOLATILE) {fprintf(o,"volatile ");f&=~VOLATILE;}
  if(f&UNSIGNED) {fprintf(o,"unsigned ");f&=~UNSIGNED;}
  if(p->attr) fprintf(o,"attr(%s) ",p->attr);
  if(p->reg) fprintf(o,"reg(%s) ",regnames[p->reg]);
  if(ISFUNC(f)){
    fprintf(o,"%s with parameters (",typname[f&NQ]);
    prl(o,p->exact);
    fprintf(o,") returning ");prd(o,p->next);return;
  }
  if(ISSTRUCT(f)||ISUNION(f)){
    fprintf(o,"%s with components {",typname[f&NQ]);
    prl(o,p->exact); fprintf(o,"} ");
    return;
  }
  if(ISPOINTER(f)) {fprintf(o,"%s to ",typname[f&NQ]);prd(o,p->next);return;}
  if(ISARRAY(f)) {fprintf(o,"%s [size %ld] of ",typname[f&NQ],zm2l(p->size));prd(o,p->next);return;}
  if(ISVECTOR(f)) {fprintf(o,"vector [size %ld] of %s",zm2l(p->size),typname[VECTYPE(f)&NQ]);return;}

  fprintf(o,"%s",typname[f&NQ]);
}
void print_var(FILE *o,Var *p)
/* Gibt eine Variable aus. */
{
  if(p->identifier&&*p->identifier){
    fprintf(o, "ident: %s: ", p->identifier);
  }
  prd(o, p->vtyp);
}

/* returns the first base Type in an compound type */
int get_first_base_type(type *t) 
{
  if (!t) return 0;
  if (ISARRAY(t->flags)) {
        return get_first_base_type(t->next);
  } else if ((ISSTRUCT(t->flags)) || (ISUNION(t->flags))) {
    return get_first_base_type((*t->exact->sl)[0].styp);
  } else return t->flags;
}


#ifndef HAVE_EXT_TYPES

void insert_const(union atyps *p,int t)
/*  Traegt Konstante in entprechendes Feld ein.       */
{
  if(!p) ierror(0);
  t&=NU;
  if(t==CHAR) {p->vchar=vchar;return;}
  if(t==SHORT) {p->vshort=vshort;return;}
  if(t==INT) {p->vint=vint;return;}
  if(t==LONG) {p->vlong=vlong;return;}
  if(t==LLONG) {p->vllong=vllong;return;}
  if(t==MAXINT) {p->vmax=vmax;return;}
  if(t==(UNSIGNED|CHAR)) {p->vuchar=vuchar;return;}
  if(t==(UNSIGNED|SHORT)) {p->vushort=vushort;return;}
  if(t==(UNSIGNED|INT)) {p->vuint=vuint;return;}
  if(t==(UNSIGNED|LONG)) {p->vulong=vulong;return;}
  if(t==(UNSIGNED|LLONG)) {p->vullong=vullong;return;}
  if(t==(UNSIGNED|MAXINT)) {p->vumax=vumax;return;}
  if(t==FLOAT) {p->vfloat=vfloat;return;}
  if(t==DOUBLE) {p->vdouble=vdouble;return;}
  if(t==LDOUBLE) {p->vldouble=vldouble;return;}
  if(t==POINTER) {p->vulong=vulong;return;}
}
void eval_const(union atyps *p,int t)
/*  Weist bestimmten globalen Variablen Wert einer CEXPR zu.       */
{
  int f=t&NQ;
  if(!p) ierror(0);
  if(f==MAXINT||(f>=CHAR&&f<=LLONG)){
    if(!(t&UNSIGNED)){
      if(f==CHAR) vmax=zc2zm(p->vchar);
      else if(f==SHORT)vmax=zs2zm(p->vshort);
      else if(f==INT)  vmax=zi2zm(p->vint);
      else if(f==LONG) vmax=zl2zm(p->vlong);
      else if(f==LLONG) vmax=zll2zm(p->vllong);
      else if(f==MAXINT) vmax=p->vmax;
      else ierror(0);
      vumax=zm2zum(vmax);
      vldouble=zm2zld(vmax);
    }else{
      if(f==CHAR) vumax=zuc2zum(p->vuchar);
      else if(f==SHORT)vumax=zus2zum(p->vushort);
      else if(f==INT)  vumax=zui2zum(p->vuint);
      else if(f==LONG) vumax=zul2zum(p->vulong);
      else if(f==LLONG) vumax=zull2zum(p->vullong);
      else if(f==MAXINT) vumax=p->vumax;
      else ierror(0);
      vmax=zum2zm(vumax);
      vldouble=zum2zld(vumax);
    }
  }else{
    if(ISPOINTER(f)){
      vumax=zul2zum(p->vulong);
      vmax=zum2zm(vumax);vldouble=zum2zld(vumax);
    }else{
      if(f==FLOAT) vldouble=zf2zld(p->vfloat);
      else if(f==DOUBLE) vldouble=zd2zld(p->vdouble);
      else vldouble=p->vldouble;
      vmax=zld2zm(vldouble);
      vumax=zld2zum(vldouble);
    }
  }
  vfloat=zld2zf(vldouble);
  vdouble=zld2zd(vldouble);
  vuchar=zum2zuc(vumax);
  vushort=zum2zus(vumax);
  vuint=zum2zui(vumax);
  vulong=zum2zul(vumax);
  vullong=zum2zull(vumax);
  vchar=zm2zc(vmax);
  vshort=zm2zs(vmax);
  vint=zm2zi(vmax);
  vlong=zm2zl(vmax);
  vllong=zm2zll(vmax);
}
#endif

function_info *new_fi(void)
/*  Belegt neue function_info-Struktur und initialisiert sie.  */
{
  function_info *new;
  new=mymalloc(sizeof(*new));
  new->first_ic=new->last_ic=new->opt_ic=0;
  new->vars=0;
  new->statics=0;
  new->inline_asm=0;
  new->flags=0;
  new->call_cnt=new->use_cnt=new->change_cnt=0;
  new->call_list=new->use_list=new->change_list=0;
  memset(new->regs_modified,0,sizeof(new->regs_modified));
#if HAVE_OSEK
/* removed */
/* removed */
/* removed */
/* removed */
#endif
  new->stack1=ul2zum(0UL);
  new->stack2=ul2zum(0UL);
  new->inline_size=-1;
  new->inline_depth=0;
  return new;
}
void free_fi(function_info *p)
/*  Gibt ein function_info mit Inhalt frei.                     */
{
  if(p->first_ic) free_IC(p->first_ic);
  if(p->opt_ic) free_IC(p->opt_ic);
  if(p->vars) free_var(p->vars);
  /* do not free statics */
  free(p->call_list);
  free(p->use_list);
  free(p->change_list);
  free(p->inline_asm);
  free(p);
}

void print_fi(FILE *f,function_info *p)
/*  Gibt function_info aus. */
{
  int i;
  fprintf(f,"function_info:\n");
  if(p->first_ic){
    IC *ic=p->first_ic;
    fprintf(f," inline_code:\n");
    pric2(f,ic);
    while(ic!=p->last_ic){
      ic=ic->next;
      pric2(f,ic);
    }
  }
  if(p->inline_asm)
    fprintf(f," inline_asm:\n%s\n",p->inline_asm);
  if(p->flags&ALL_CALLS)
    fprintf(f," all calls have been evaluated\n");
  if(p->flags&ALL_USES)
    fprintf(f," all uses have been evaluated\n");
  if(p->flags&ALL_MODS)
    fprintf(f," all changes have been evaluated\n");
  if(p->flags&ALL_REGS){
    fprintf(f," all reg-changes have been evaluated\n");
    for(i=1;i<=MAXR;i++)
      if(BTST(p->regs_modified,i)) fprintf(f,"  %s\n",regnames[i]);
  }
  fprintf(f," call_list:\n");print_varlist(f,p->call_list,p->call_cnt);
  fprintf(f," use_list:\n");print_varlist(f,p->use_list,p->use_cnt);
  fprintf(f," change_list:\n");print_varlist(f,p->change_list,p->change_cnt);
  if(p->flags&ALL_STACK){
    fprintf(f,"stack1: ");printzum(f,p->stack1);fprintf(f,"\n");
    fprintf(f,"stack2: ");printzum(f,p->stack2);fprintf(f,"\n");
  }
  if(p->flags&NEVER_RETURNS)
    fprintf(f," never returns\n");
  if(p->flags&ALWAYS_RETURNS)
    fprintf(f," always returns\n");
  if(p->flags&NOSIDEFX)
    fprintf(f," no side effects\n");
}

void print_varlist(FILE *f,varlist *p,int n)
{
  int i;
  char *s;
  for(i=0;i<n;i++){
    if(p[i].v) s=p[i].v->identifier; else s="<unknown>";
    fprintf(f,"  %s(%p), flags=%d\n",s,(void*)p[i].v,p[i].flags);
  }
}

int is_const(type *t)
/*  tested, ob ein Typ konstant (und damit evtl. in der Code-Section) ist   */
{
  if(!(t->flags&(CONST|STRINGCONST))){
    do{
      if(t->flags&(CONST|STRINGCONST)) return(1);
      if((t->flags&NQ)!=ARRAY) return 0;
      t=t->next;
    }while(1);
  }else return 1;
}

/* is object volatile? */
int is_volatile_obj(obj *o)
{
  if(o->flags&DREFOBJ){
    if(o->dtyp&(VOLATILE|PVOLATILE)){
      return 1;
    }
    if((o->flags&VAR)&&ISPOINTER(o->v->vtyp->flags)&&(o->v->vtyp->next->flags&VOLATILE)){
      return 1;
    }
  }
  if(o->flags&VAR){
    return o->v->vtyp->flags&VOLATILE;
  }else{
    return 0;
  }
}

/* is IC volatile? */
int is_volatile_ic(IC *p)
{
  if(p->q1.flags){
    if(is_volatile_obj(&p->q1)||(q1typ(p)&VOLATILE)) return 1;
  }
  if(p->q2.flags){
    if(is_volatile_obj(&p->q2)||(q2typ(p)&VOLATILE)) return 1;
  }
  if(p->z.flags){
    if(is_volatile_obj(&p->z)||(ztyp(p)&VOLATILE)) return 1;
  }
  return 0;
}

/* removes last asm-line from emit-buffer */
void remove_asm(void)
{
  emit_l--;
  if(emit_l<0) emit_l=EMIT_BUF_DEPTH-1;
}

/* flush the asm-output buffer */
void emit_flush(FILE *f)
{
  if(!f||no_emit) return;
  while(emit_f!=emit_l){
    fputs(emit_buffer[emit_f],f);
    emit_f++;
    if(emit_f>=EMIT_BUF_DEPTH) emit_f=0;
  }
  emit_l=emit_f=0;
  emit_p=emit_buffer[0];
}

/* emit inline-asm, depending on no_inline_peephole, feed it through
   asm_peephole() or flush the buffer and print it directly to the file */
void emit_inline_asm(FILE *f,char *p)
{
  if(!f||no_emit) return;
  if(no_inline_peephole){
    emit_flush(f);
    fprintf(f,"%s\n",p);
  }else{
    while(*p){
      emit(f,"%c",*p++);
    }
    emit(f,"\n");
  }
}

/* print output; this is only to be used for assembly output! */
void emit(FILE *f,const char *fmt,...)
{
  static char tmp[EMIT_BUF_LEN];
  char *p;
  va_list vl;
  if(!f||no_emit) return;
  va_start(vl,fmt);
  vsprintf(tmp,fmt,vl);
  p=tmp;
  while(*p){
    *emit_p++=*p++;
    if(p[-1]=='\n'){
      *emit_p=0;
#if HAVE_TARGET_PEEPHOLE
      while(emit_peephole());
#endif
      emit_l++;
      if(emit_l>=EMIT_BUF_DEPTH) emit_l=0;
      emit_p=emit_buffer[emit_l];
      if(emit_l==emit_f){
        /* FIXME: error check */
        fputs(emit_buffer[emit_f],f);
        emit_f++;
        if(emit_f>=EMIT_BUF_DEPTH) emit_f=0;
      }
    }
  }
  *emit_p=0;
}

void emit_char(FILE *f,int c)
{
  static char tmp[2];
  tmp[0]=c;
  emit(f,tmp);
}

/* detect whether the following code resembles a switch-case-statement */
/* will return the longest sequence which has at least min_density */
case_table *calc_case_table(IC *p,double min_density)
{
  static case_table ct;
  obj *o,*ccr;
  static union atyps *vals;
  union atyps min,max;
  zumax diff;
  static int *labels,cur_size;
  double cur_density;
  int t,j,num;
  if(p->code!=COMPARE||!(p->q2.flags&KONST))
    return 0;
  o=&p->q1;t=p->typf;
  if(!ISINT(t)||(t&VOLATILE)) return 0;
  num=0;
  ct.num=0;
  if(t&UNSIGNED){
    max.vumax=t_min(UNSIGNED|MAXINT);
    min.vumax=t_max(UNSIGNED|MAXINT);
  }else{
    max.vmax=t_min(MAXINT);
    min.vmax=t_max(MAXINT);
  }
  while(p&&p->code==COMPARE&&(p->q2.flags&KONST)&&p->typf==t&&objs_equal(o,&p->q1,t)){
    zumax zum;zmax zm;
    if(multiple_ccs) ccr=&p->z;
    if(num>=cur_size){
      cur_size+=64;
      labels=myrealloc(labels,cur_size*sizeof(*labels));
      vals=myrealloc(vals,cur_size*sizeof(*vals));
    }
    for(j=0;j<num;j++){
      if(!compare_const(&vals[j],&p->q2.val,t)){
        return 0; /* FIXME? Could simply ignore? */
      }
    }
    vals[num]=p->q2.val;
    p=p->next;
    while(p&&(p->code==NOP||p->code==ALLOCREG||p->code==FREEREG)) p=p->next;
    if(p->code!=BEQ||(multiple_ccs&&!objs_equal(ccr,&p->q1,0))) break;
    labels[num]=p->typf;
    p=p->next;
    while(p&&(p->code==NOP||p->code==ALLOCREG||p->code==FREEREG)) p=p->next;
    eval_const(&vals[num],t);
    num++;
    if(t&UNSIGNED){
      if(zumleq(vumax,min.vumax))
        insert_const(&min,UNSIGNED|MAXINT);
      if(zumleq(max.vumax,vumax))
        insert_const(&max,UNSIGNED|MAXINT);
      cur_density=num/(1+zld2d(zum2zld(zumsub(max.vumax,min.vumax))));
      if(cur_density>=min_density){
        ct.num=num;
        ct.next_ic=p;
        ct.min=min;
        ct.max=max;
        ct.diff=zumsub(max.vumax,min.vumax);
        ct.density=cur_density;
      }
    }else{
      if(zmleq(vmax,min.vmax))
        insert_const(&min,MAXINT);
      if(zmleq(max.vmax,vmax))
        insert_const(&max,MAXINT);
      cur_density=num/zld2d(zum2zld((1+zumsub(zm2zum(max.vmax),zm2zum(min.vmax)))));
      if(cur_density>=min_density){
        ct.num=num;
        ct.next_ic=p;
        ct.min=min;
        ct.max=max;
        ct.diff=zumsub(zm2zum(max.vmax),zm2zum(min.vmax));
        ct.density=cur_density;
      }
    }
  }
  ct.vals=vals;
  ct.labels=labels;
  ct.typf=t;
  return &ct;
}

/* emit a list of jump-table entries */
void emit_jump_table(FILE *f,case_table *ct,char *da,char *labprefix,int defl)
{
  unsigned long l,e;
  int i;
  zmax zm;zumax zum;
  if(ct->typf&UNSIGNED){
    zum=ct->min.vumax;
    zm=zum2zm(zum);
  }else{
    zm=ct->min.vmax;
    zum=zm2zum(zm);
  }
  e=zum2ul(ct->diff);
  for(l=0;l<=e;l++){
    emit(f,"%s",da);
    for(i=0;i<ct->num;i++){
      eval_const(&ct->vals[i],ct->typf);
      if(zmeqto(vmax,zm)&&zumeqto(vumax,zum)){
        emit(f,"%s%d\n",labprefix,ct->labels[i]);
        break;
      }
    }
    if(i>=ct->num)
      emit(f,"%s%d\n",labprefix,defl);
    zm=zmadd(zm,l2zm(1L));
    zum=zumadd(zum,ul2zum(1UL));
  }
}
/* display warnings if specified stack-size cannot be guaranteed */
void static_stack_check(Var *v)
{
  /*FIXME*/
}

#ifdef HAVE_REGPARMS
/* return the offset of the first variable-argument-macro to the
   beginning of the argument-area (i.e. the space occupied by
   normal arguments on the stack */
zmax va_offset(Var *v)
{
  int i;
  zmax offset=l2zm(0L);
  static type rp={0};
  treg_handle rh=empty_reg_handle;
  if((v->vtyp->next->flags&NQ)!=VOID&&!freturn(v->vtyp->next)){
    rp.next=v->vtyp->next;
    rp.flags=POINTER_TYPE(v->vtyp->next);
    if(!reg_parm(&rh,&rp,0,v->vtyp)){
      ierror(0);
    }
  }
  for(i=0;i<v->vtyp->exact->count;i++){
#if 0
    if((*v->vtyp->exact->sl)[i].reg!=0)
      continue;
#endif
    if(((*v->vtyp->exact->sl)[i].styp->flags&NQ)==VOID)
      ierror(0);
    if(reg_parm(&rh,(*v->vtyp->exact->sl)[i].styp,0,v->vtyp)!=0)
      continue;
    offset=zmadd(offset,szof((*v->vtyp->exact->sl)[i].styp));
    offset=zmmult(zmdiv(zmadd(offset,zmsub(stackalign,l2zm(1L))),stackalign),stackalign); 
  }
  return offset;
}
#endif

/* We provide an own qsort to get reproducable results. */
void vqsort (void *base,size_t nmemb,size_t size,int (*compar)(const void *,const void *))
{ char *base2=(char *)base;
  char tmp;
  size_t i,a,b,c;
  while(nmemb>1)
  { a=0;
    b=nmemb-1;
    c=(a+b)/2; /* Middle element */
    for(;;)
    { while((*compar)(&base2[size*c],&base2[size*a])>0)
        a++; /* Look for one >= middle */
      while((*compar)(&base2[size*c],&base2[size*b])<0)
        b--; /* Look for one <= middle */
      if(a>=b)
        break; /* We found no pair */
      for(i=0;i<size;i++) /* swap them */
      { tmp=base2[size*a+i];
        base2[size*a+i]=base2[size*b+i];
        base2[size*b+i]=tmp; }
      if(c==a) /* Keep track of middle element */
        c=b;
      else if(c==b)
        c=a;
      a++; /* These two are already sorted */
      b--;
    } /* a points to first element of right intervall now (b to last of left) */
    b++;
    if(b<nmemb-b) /* do recursion on smaller intervall and iteration on larger one */
    { vqsort(base2,b,size,compar);
      base2=&base2[size*b];
      nmemb=nmemb-b;
    }
    else
    { vqsort(&base2[size*b],nmemb-b,size,compar);
      nmemb=b; }
  }
  return;
}

/* calculates registers used by this call IC
   returns 0 if not possible */
int calc_regs(IC *p,int showwarnings)
{
  int i;
  if(p->call_cnt){
    for(i=0;i<p->call_cnt;i++){
      if(p->call_list[i].v->fi&&(p->call_list[i].v->fi->flags&ALL_REGS)){
        bvunite(regs_modified,p->call_list[i].v->fi->regs_modified,RSIZE);
#if HAVE_OSEK
/* removed */
/* removed */
/* removed */
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
        int r;
        for(r=1;r<=MAXR;r++) if(regscratch[r]) BSET(regs_modified,r);
        err_ic=p;
        if(!p->call_list[i].v->fi) p->call_list[i].v->fi=new_fi();
        if(!(p->call_list[i].v->fi->flags&WARNED_REGS)){
          error(318,p->call_list[i].v->identifier);
          p->call_list[i].v->fi->flags|=WARNED_REGS;
        }
        return 0;
      }
    }
    return 1;
  }
  err_ic=p;if(showwarnings) error(320);
  return 0;
}



#ifndef HAVE_TARGET_BFLAYOUT
int bflayout(int bfoffset,int bfsize,int t)
{
  if(BIGENDIAN)
    return (int)zm2l(zmmult(sizetab[t&NQ],char_bit))-bfoffset-bfsize;
  else
    return bfoffset;
}
#endif

long get_pof2(zumax x)
/*  Yields log2(x)+1 oder 0. */
{
  zumax p;int ln=1,max=(int)zm2l(zmmult(sizetab[LLONG],char_bit));
  p=ul2zum(1L);
  while(ln<=max&&zumleq(p,x)){
    if(zumeqto(x,p)) return ln;
    ln++;p=zumadd(p,p);
  }
  return 0;
}

/* check if type is a varargs function */
int is_varargs(type *t)
{
  int c;
  if(t->exact&&(c=t->exact->count)!=0&&(*t->exact->sl)[c-1].styp->flags!=VOID)
    return 1;
  else
    return 0;
}

/* add string to type- oder variable-aatribute */
void add_attr(char **attr,char *new)
{
  int ln,lo;
  if(!new) return;
  ln=strlen(new);
  if(*attr){
    lo=strlen(*attr);
    *attr=myrealloc(*attr,lo+ln+2);
  }else{
    lo=0;
    *attr=mymalloc(ln+2);
  }
  (*attr)[lo]=';';
  strcpy(*attr+lo+1,new);
}


hashtable *new_hashtable(size_t size)
{
  hashtable *new = mymalloc(sizeof(*new));

  new->size = size;
  new->collisions = 0;
  new->entries = mymalloc(size*sizeof(*new->entries));
  memset(new->entries,0,size*sizeof(*new->entries));
  return new;
}

size_t hashcode(char *name)
{
  size_t h = 5381;
  int c;

  while (c = (unsigned char)*name++)
    h = ((h << 5) + h) + c;
  return h;
}

/* add to hashtable; name must be unique */
void add_hashentry(hashtable *ht,char *name,hashdata data)
{
  size_t i=(hashcode(name)%ht->size);
  hashentry *new=mymalloc(sizeof(*new));
  new->name=name;
  new->data=data;
  if(DEBUG&1){
    if(ht->entries[i])
      ht->collisions++;
  }
  new->next=ht->entries[i];
  ht->entries[i]=new;
}

/* finds unique entry in hashtable */
hashdata find_name(hashtable *ht,char *name)
{
  size_t i=hashcode(name)%ht->size;
  hashentry *p;
  for(p=ht->entries[i];p;p=p->next){
    if(!strcmp(name,p->name)){
      return p->data;
    }else
      ht->collisions++;
  }
  return 0;
}

#ifndef CHARBACK
unsigned char CHARBACK(unsigned char x)
{
  static unsigned char tab[256],init=0;
  if(!init){
    int i,j;
    for(i=0;i<256;i++)
      for(j=0;j<256;j++)
	if(tab[j]==0&&CHARCONV(i)==j){
	  tab[j&255]=i;
	  break;
	}
    init=1;
  }
  return tab[x&255];
}

void STRBACK(unsigned char *p)
{
  while(p&&*p){
    *p=CHARBACK(*p);
    p++;
  }
}
#endif

int objs_equal(obj *o1,obj *o2,int t)
/*  Vergleicht die beiden Objekte; liefert 1, wenn sie gleich sind, sonst 0 */
{
    int i1,i2;
    i1=o1->flags&~SCRATCH;i2=o2->flags&~SCRATCH;
    if(i1!=i2) return 0;
    if(i1&DREFOBJ){
      if(o1->dtyp!=o2->dtyp) return 0;
    }
    if(i1&KONST) return(!compare_const(&o1->val,&o2->val,t));
    if(i1&VAR){
      if(o1->v!=o2->v) return 0;
      if(!zmeqto(o1->val.vmax,o2->val.vmax)) return 0;
    }
    return 1;
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
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
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

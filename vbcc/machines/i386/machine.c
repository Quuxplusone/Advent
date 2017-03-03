
/*  Code generator for Intel 80386 or higher.                   */

#include "supp.h"
#include "vbc.h" /* nicht schoen, aber ... */

static char FILE_[]=__FILE__;

#include "dwarf2.c"

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for i386 V0.7a (c) in 1996-2006 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,VALFLAG,0,0,
                    0,0,0,0,
		    0};
char *g_flags_name[MAXGF]={"cpu","fpu","no-delayed-popping","const-in-data",
                           "merge-constants","elf","longalign","safe-fp",
			   "use-framepointer"};
union ppi g_flags_val[MAXGF];

/* Typenames (needed because of HAVE_EXT_TYPES). */
char *typname[]={"strange","char","short","bshort","int","bint",
		 "long","blong","long long","blong long",
		 "float","bfloat","double","bdouble",
		 "long double","blong double","void",
                 "pointer","bpointer",
		 "array","struct","union","enum","function"};

/*  Alignment-requirements for all types in bytes.              */
zmax align[MAX_TYPE+1];

/*  Alignment that is sufficient for every object.              */
zmax maxalign;

/*  CHAR_BIT of the target machine.                             */
zmax char_bit;

/*  Sizes of all elementary types in bytes.                     */
zmax sizetab[MAX_TYPE+1];

/*  Minimum and Maximum values each type can have.              */
/*  Must be initialized in init_cg().                           */
zmax t_min[MAX_TYPE+1];
zumax t_max[MAX_TYPE+1];
zumax tu_max[MAX_TYPE+1];

/*  Names of all registers.                                     */
char *regnames[MAXR+1]={"noreg","%eax","%ecx","%edx","%ebx",
                                "%esi","%edi","%ebp","%esp",
                                "%st(0)","%st(1)","%st(2)","%st(3)",
                                "%st(4)","%st(5)","%st(6)","%st(7)",
                                "%eax/%edx","%esi/%edi"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0};


/****************************************/
/*  Some private data and functions.    */
/****************************************/

#define USEFP (g_flags[8]&USEDFLAG)

static long malign[MAX_TYPE+1]=  {1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,1,1,2,2};
static long msizetab[MAX_TYPE+1]={0,1,2,2,4,4,4,4,8,8,4,4,8,8,8,8,0,4,4,0,0,0,4,0};

#define ISBE(x) (((x)&NQ)==BPOINTER||(((x)&NQ)>=BSHORT&&((x)&NQ)<=BLDOUBLE&&((x)&1)))
#define LETYPE(x) ((x)-1)

struct Typ ltyp={LONG},ldbl={DOUBLE};

#define DATA 0
#define BSS 1
#define CODE 2

static char *marray[]={"__I386__","__X86__",
		       "__bigendian=__attr(\"bigendian\")",
		       "__littleendian=__attr(\"littleendian\")",
		       0};

static int section=-1,newobj;
static char *codename="\t.text\n",*dataname="\t.data\n",*bssname="";
static const int ax=1,cx=2,dx=3,bx=4,si=5,di=6,bp=7,sp=8,axdx=17,sidi=18;
static char x_t[]={'?','b','w','w','l','l','l','l','?','?','s','s','l','l','l','l','v','l','l','a','s','u','e','f'};
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)
#define isvaddr(x) ((p->x.flags&(VARADR|DREFOBJ))==VARADR)
#define isvconst(x) (isconst(x)||isvaddr(x))

static long loff,stackoffset,notpopped,dontpop,maxpushed,stack;

static char *ccs[]={"z","nz","l","ge","le","g","mp"};
static char *ccu[]={"z","nz","b","ae","be","a","mp"};
static char *logicals[]={"or","xor","and"};
static char *arithmetics[]={"sal","sar","add","sub","imul","div","mod"};
static char *farithmetics[]={"f?","f?","fadd","fsub","fmul","fdiv","fsubr","fdivr"};
static char *dct[]={"","byte","short","short","long","long",
		    "long","long","quad","quad","long","long","long","long",
		    "long","long","long","long","long","long"};

static pushedsize,pushorder=2;
static int fst[8];
static int cxl,dil,sil;
static char *idprefix="",*labprefix="l";

static struct fpconstlist {
    struct fpconstlist *next;
    int label,typ;
    union atyps val;
} *firstfpc;

static int addfpconst(struct obj *o,int t)
{
  struct fpconstlist *p=firstfpc;
  t&=NQ;
  if(g_flags[4]&USEDFLAG){
    for(p=firstfpc;p;p=p->next){
      if(t==p->typ){
	eval_const(&p->val,t);
	if(t==FLOAT&&zldeqto(vldouble,zf2zld(o->val.vfloat))) return(p->label);
	if(t==DOUBLE&&zldeqto(vldouble,zd2zld(o->val.vdouble))) return(p->label);
	if(t==LDOUBLE&&zldeqto(vldouble,o->val.vldouble)) return(p->label);
      }
    }
  }
  p=mymalloc(sizeof(struct fpconstlist));
  p->next=firstfpc;
  p->label=++label;
  p->typ=t;
  p->val=o->val;
  firstfpc=p;
  return p->label;
}
/* pushed on the stack by a callee, no pop needed */
static void callee_push(long l)
{
}
static void push(long l)
{
  stackoffset-=l;
  if(stackoffset<maxpushed) maxpushed=stackoffset;
}
static void pop(long l)
{
  stackoffset+=l;
}

void title(FILE *f)
{
  static int done;
  extern char *inname; /*grmpf*/
  if(!done&&f){
    done=1;
    emit(f,"\t.file\t\"%s\"\n",inname);
  }
} 

static void emit_obj(FILE *f,struct obj *p,int t)
/*  Gibt Objekt auf Bildschirm aus                      */
{
  if((p->flags&(DREFOBJ|KONST))==(DREFOBJ|KONST)){
    emitval(f,&p->val,p->dtyp&NU);
    return;
  }
  if((p->flags&(DREFOBJ|REG))==(DREFOBJ|REG)) emit(f,"(");
  if(p->flags&VARADR) emit(f,"$");
  if((p->flags&VAR)&&!(p->flags&REG)) {
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      if(USEFP||vlas){
	if(!zmleq(l2zm(0L),p->v->offset))
	  emit(f,"%ld(%s)",(long)(-zm2l(p->v->offset)+zm2l(p->val.vmax))+4,regnames[bp]);
	else
	  emit(f,"%ld(%s)",(long)(zm2l(p->v->offset)+zm2l(p->val.vmax))-loff-pushedsize,regnames[bp]);
      }else{
	if(!zmleq(l2zm(0L),p->v->offset))
	  emit(f,"%ld(%s)",(long)(loff-zm2l(p->v->offset)+zm2l(p->val.vmax))-stackoffset+pushedsize,regnames[sp]);
	else
	  emit(f,"%ld(%s)",(long)(zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset),regnames[sp]);
      }
    }else{
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,LONG);emit(f,"+");}
      if(p->v->storage_class==STATIC){
        emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
        emit(f,"%s%s",idprefix,p->v->identifier);
      }
    }
  }
  if(p->flags&REG){
    if(p->reg>8){
      int i;
      for(i=0;i<8;i++){
        if(fst[i]==p->reg)
          emit(f,"%s",regnames[i+9]);
      }
    }else{
      t&=NQ;
      if(t==CHAR&&!(p->flags&DREFOBJ)) emit(f,"%%%cl",regnames[p->reg][2]);
      else if(t==SHORT&&!(p->flags&DREFOBJ)) emit(f,"%%%s",regnames[p->reg]+2);
      else emit(f,"%s",regnames[p->reg]);
    }
  }
  if(p->flags&KONST){
    if(ISFLOAT(t)){
      emit(f,"%s%d",labprefix,addfpconst(p,t));
    }else{
      emit(f,"$");emitval(f,&p->val,t&NU);
    }
  }
  if((p->flags&(DREFOBJ|REG))==(DREFOBJ|REG)) emit(f,")");
}

static char *mregname(int r,int t)
{
  static char s[8];
  t&=NQ;
  if(t==CHAR) sprintf(s,"%%%cl",regnames[r][2]);
  else if(t==SHORT) sprintf(s,"%%%s",regnames[r]+2);
  else sprintf(s,"%s",regnames[r]);
  return s;
}


static void emit_lword(FILE *f,struct obj *o)
{
  if(o->flags&KONST){
    static struct obj cobj;
    cobj.flags=KONST;
    eval_const(&o->val,UNSIGNED|LLONG);
    vumax=zumand(vumax,ul2zum(0xffffffff));
    cobj.val.vulong=zum2zul(vumax);
    emit_obj(f,&cobj,UNSIGNED|LONG);
  }else if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    emit(f,"%s",regnames[rp.r1]);
  }else
    emit_obj(f,o,UNSIGNED|LONG);
}

static void emit_hword(FILE *f,struct obj *o)
{
  if(o->flags&KONST){
    static struct obj cobj;
    cobj.flags=KONST;
    eval_const(&o->val,UNSIGNED|LLONG);
    vumax=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffffff));
    cobj.val.vulong=zum2zul(vumax);
    emit_obj(f,&cobj,UNSIGNED|LONG);
  }else if(o->flags&DREFOBJ){
    if(!(o->flags&REG)) ierror(0);
    emit(f,"4(%s)",regnames[o->reg]);
  }else if(o->flags&REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    emit(f,"%s",regnames[rp.r2]);
  }else{
    o->val.vmax=zmadd(o->val.vmax,l2zm(4L));
    emit_obj(f,o,UNSIGNED|LONG);
    o->val.vmax=zmsub(o->val.vmax,l2zm(4L));
  }
}

static void dwarf2_print_frame_location(FILE *f,struct Var *v)
{
  /*FIXME: needs a location list and correct register trabslation */
  struct obj o;
  o.flags=REG;
  if(USEFP||vlas)
    o.reg=bp;
  else
    o.reg=sp;
  o.val.vmax=l2zm(0L);
  o.v=0;
  dwarf2_print_location(f,&o);
}
static int dwarf2_regnumber(int r)
{
  static dwarf_regs[17]={-1,0,1,2,3,6,7,5,4,16,17,18,19,20,21,22,23};
  return dwarf_regs[r];
}
static zmax dwarf2_fboffset(struct Var *v)
{
  if(!v||(v->storage_class!=AUTO&&v->storage_class!=REGISTER)) ierror(0);
  if(USEFP||vlas){
    if(!zmleq(l2zm(0L),v->offset))
      return l2zm((long)(-zm2l(v->offset))+4);
    else
      return l2zm((long)(zm2l(v->offset))-loff-pushedsize);
  }else{
    if(!zmleq(l2zm(0L),v->offset))
      return l2zm((long)(loff-zm2l(v->offset))+pushedsize);
    else
      return v->offset;
  }
}
static void fxch(FILE *f,int i)
{
    int m;
    emit(f,"\tfxch\t%s\n",regnames[i+9]);
    m=fst[0];fst[0]=fst[i];fst[i]=m;
}
static int freest(void)
{
    int i;
    for(i=0;i<8;i++){
        if(fst[i]<0) return i;
    }
    for(i=0;i<8;i++){
        if(fst[i]==0) return i;
    }
    ierror(0);
}
static void fpush(FILE *f)
{
    int i;
    if(fst[7]>0){
        i=freest();
        if(fst[i]==0) emit(f,"\tffree\t%s\n",regnames[i+9]);
        fxch(f,i);fxch(f,7);
    }
    for(i=7;i>0;i--)
        fst[i]=fst[i-1];
    fst[0]=-1;
}
static void fpop(void)
{
    int i;
/*    if(fst[0]>0&&regs[fst[0]]) ierror(0);*/
    for(i=0;i<7;i++)
        fst[i]=fst[i+1];
    fst[7]=-1;
}
static void fload(FILE *f,struct obj *o,int t)
{
    emit(f,"\tfld");
    if((o->flags&(REG|DREFOBJ))==REG) emit(f,"\t");
        else emit(f,"%c\t",x_t[t&NQ]);
    emit_obj(f,o,t);emit(f,"\n");
    fpush(f);
}
static void fstore(FILE *f,struct obj *o,int t)
{
    int i;
    if((o->flags&(REG|DREFOBJ))==REG){
        for(i=0;i<8;i++)
            if(fst[i]==o->reg) fst[i]=0;
        fst[0]=o->reg;
    }else{
        emit(f,"\tfstp%c\t",x_t[t&NQ]);emit_obj(f,o,t);
        fpop();emit(f,"\n");
    }
}
static void prfst(FILE *f,char *s)
{
    int i;
    if(DEBUG==0) return;
    emit(f,"*\t%s\t",s);
    for(i=0;i<8;i++){
        if(fst[i]>=0){
            if(fst[i]==0) emit(f,"+++ ");
              else        emit(f,"%s ",regnames[fst[i]]+3);
        }else{
            emit(f,"--- ");
        }
    }
    emit(f,"\n");
}
static void finit(void)
{
    int i;
    for(i=0;i<8;i++){
        if(regs[i+9])
            fst[i]=i+9;
        else
            fst[i]=-1;
    }
}
static void forder(FILE *f)
{
    int i,m,unordered;
    prfst(f,"forder");
    for(i=0;i<8;i++){
        if(fst[i]==0){emit(f,"\tffree\t%s\n",regnames[i+9]);fst[i]=-1;}
    }
oloop:
    unordered=0;
    for(i=0;i<8;i++){
        if(fst[i]>0&&fst[i]!=i+9&&regs[fst[i]]){unordered=1;break;}
    }
    if(!unordered) return;
    if(fst[0]>=0&&regs[fst[0]]){
        if(fst[0]!=9){
            fxch(f,fst[0]-9);
            goto oloop;
        }else{
            fxch(f,freest());
        }
    }
    for(i=1;i<8;i++){
        if(fst[i]>=0&&fst[i]!=i+9&&regs[fst[i]]&&fst[i]!=9){
            fxch(f,i);
            goto oloop;
        }
    }
    if(regs[9]){
        for(i=1;i<8;i++){
            if(fst[i]==9){ fxch(f,i);return;}
        }
    }
}
static void pr(FILE *f,struct IC *p)
{
    int i;
    for(;pushorder>2;pushorder>>=1){
        for(i=1;i<=8;i++){
            if(regs[i]&pushorder){
	      if(p->code==PUSH||p->code==CALL){
		emit(f,"\tmovl\t%ld(%s),%s\n",loff-4-stackoffset,regnames[sp],regnames[i]);
	      }else{
                emit(f,"\tpopl\t%s\n",regnames[i]);
                pop(4);
	      }
	      regs[i]&=~pushorder;
            }
        }
    }
    for(i=1;i<=8;i++)
        if(regs[i]&2) regs[i]&=~2;
}
static void function_top(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionskopf                       */
{
    int i;
    if(section!=CODE){emit(f,codename);if(f) section=CODE;}
    if(v->storage_class==EXTERN){
      if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
	emit(f,"\t.globl\t%s%s\n",idprefix,v->identifier);
      emit(f,"%s%s:\n",idprefix,v->identifier);
    }else
      emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    if(USEFP||vlas)
      emit(f,"\tpushl\t%s\n\tmovl\t%s,%s\n",regnames[bp],regnames[sp],regnames[bp]);
    for(pushedsize=0,i=1;i<sp;i++){
        if(regused[i]&&!regscratch[i]){
            emit(f,"\tpushl\t%s\n",regnames[i]);
            pushedsize+=4;
        }
    }
    if(offset) emit(f,"\tsubl\t$%ld,%%esp\n",offset);
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionsende                       */
{
    int i;
    forder(f);
    if(offset) emit(f,"\taddl\t$%ld,%%esp\n",offset);
    for(i=sp-1;i>0;i--){
        if(regused[i]&&!regscratch[i]){
            emit(f,"\tpopl\t%s\n",regnames[i]);
        }
    }
    if(USEFP||vlas) emit(f,"\tpopl\t%s\n",regnames[bp]);
    emit(f,"\tret\n");
    if(v->storage_class==EXTERN){
      emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
      emit(f,"\t.size\t%s%s,.-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
    }else{
      emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
      emit(f,"\t.size\t%s%ld,.-%s%ld\n",labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
    }    
}
static int compare_objects(struct obj *o1,struct obj *o2)
{
  if(o1->flags==o2->flags&&o1->am==o2->am){
    if(!(o1->flags&VAR)||(o1->v==o2->v&&zmeqto(o1->val.vmax,o2->val.vmax))){
      if(!(o1->flags&REG)||o1->reg==o2->reg){
	return 1;
      }
    }
  }
  return 0;
}
static int get_reg(FILE *f,struct IC *p,int type)
{
  int i;
  /*  If we can use a register which was already used by the compiler */
  /*  or it is a sratch register then we can use it without problems. */
  for(i=1;i<=8;i++){
    if(!regs[i]&&(regused[i]||regscratch[i])&&regok(i,type,0)){
      regs[i]=2;
      BSET(regs_modified,i);
      return(i);
    }
  }
  /*  Otherwise we have to save this register.                        */
  /*  We may not use a register which is used in this IC.             */
  for(i=1;i<=8;i++){
    if(regs[i]<2&&regok(i,type,0)
       &&(!(p->q1.flags&REG)||p->q1.reg!=i)
       &&(!(p->q2.flags&REG)||p->q2.reg!=i)
       &&(!(p->z.flags&REG)||p->z.reg!=i) ){
      
      if(p->code==PUSH||p->code==CALL){
	emit(f,"\tmovl\t%s,%ld(%s)\n",regnames[i],loff-4-stackoffset,regnames[sp]);
      }else{
	emit(f,"\tpushl\t%s\n",regnames[i]);
	push(4);
      }
      /*  Mark register as pushed (taking care of the order). */
      pushorder<<=1; regs[i]|=pushorder;
      BSET(regs_modified,i);
      return i;
    }
  }
  ierror(0);
}
static void move(FILE *f,struct obj *q,int qr,struct obj *z,int zr,int t)
/*  Generates code to move object q (or register qr) into object z (or  */
/*  register zr).                                                       */
{
  t&=NQ;
  if(q&&(q->flags&(REG|DREFOBJ))==REG) qr=q->reg;
  if(z&&(z->flags&(REG|DREFOBJ))==REG) zr=z->reg;
  if((t&NQ)==LLONG){
    if(qr&&zr&&qr==zr) return;
    emit(f,"\tmovl\t");
    if(qr){
      if(!reg_pair(qr,&rp)) ierror(0);
      emit(f,"%s",regnames[rp.r1]);
    }else
      emit_lword(f,q);
    emit(f,",");
    if(zr){
      if(!reg_pair(zr,&rp)) ierror(0);
      emit(f,"%s",regnames[rp.r1]);
    }else
      emit_lword(f,z);
    emit(f,"\n");
    emit(f,"\tmovl\t");
    if(qr){
      if(!reg_pair(qr,&rp)) ierror(0);
      emit(f,"%s",regnames[rp.r2]);
    }else
      emit_hword(f,q);
    emit(f,",");
    if(zr){
      if(!reg_pair(zr,&rp)) ierror(0);
      emit(f,"%s",regnames[rp.r2]);
    }else
      emit_hword(f,z);
    emit(f,"\n");
    return;
  }
  if(qr&&zr){
    if(qr!=zr)
      emit(f,"\tmovl\t%s,%s\n",regnames[qr],regnames[zr]);
    return;
  }
  if(zr&&(q->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&q->val,t);
    if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))){
      emit(f,"\txorl\t%s,%s\n",regnames[zr],regnames[zr]);
      return;
    }
  }
  emit(f,"\tmov%c\t",x_t[t&NQ]);
  if(qr){
    emit(f,"%s",mregname(qr,t));
  }else
    emit_obj(f,q,t);
  emit(f,",");
  if(zr){
    emit(f,"%s",mregname(zr,t));
  }else
    emit_obj(f,z,t);
  emit(f,"\n");
}
static long pof2(zumax x)
/*  Yields log2(x)+1 oder 0. */
{
    zumax p;int ln=1;
    p=ul2zum(1UL);
    while(ln<=32&&zumleq(p,x)){
        if(zumeqto(x,p)) return ln;
        ln++;p=zumadd(p,p);
    }
    return 0;
}

/****************************************/
/*  End of private fata and functions.  */
/****************************************/


int init_cg(void)
/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
{
  int i;
  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign=l2zm(4L);
  char_bit=l2zm(8L);
  if(g_flags[6]&USEDFLAG){
    for(i=SHORT;i<=MAX_TYPE;i++) malign[i]=4;
  }
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i=1;i<= 8;i++) {regsize[i]=l2zm(4L);regtype[i]=&ltyp;}
  for(i=9;i<=16;i++) {regsize[i]=l2zm(8L);regtype[i]=&ldbl;}

  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-128L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[INT]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LONG]=t_min(INT);
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(127L);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[INT]=ul2zum(2147483647UL);
  t_max[LONG]=t_max(INT);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT]=t_max(LLONG);
  t_min[BSHORT]=l2zm(-32768L);
  t_min[BINT]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[BLONG]=t_min(INT);
  t_min[BLLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_max[BSHORT]=ul2zum(32767UL);
  t_max[BINT]=ul2zum(2147483647UL);
  t_max[BLONG]=t_max(INT);
  t_max[BLLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=ul2zum(4294967295UL);
  tu_max[LONG]=t_max(UNSIGNED|INT);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[BSHORT]=ul2zum(65535UL);
  tu_max[BINT]=ul2zum(4294967295UL);
  tu_max[BLONG]=t_max(UNSIGNED|INT);
  tu_max[BLLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

  /*  Reserve a few registers for use by the code-generator.      */
  /*  We only reserve the stack-pointer here.                     */
  regsa[sp]=1;
  if(USEFP) regsa[bp]=regscratch[bp]=1;
  /*  We need at least one free slot in the flaoting point stack  */
  regsa[16]=1;regscratch[16]=0;
  /*  Use l%d as labels and _%s as identifiers by default. If     */
  /*  -elf is specified we use .l%d and %s instead.               */
  if(g_flags[5]&USEDFLAG) labprefix=".l"; else idprefix="_";
  target_macros=marray;

  declare_builtin("__mulll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__addll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__subll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__andll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__orll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__eorll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__negll",LLONG,LLONG,0,0,0,1,0);
  declare_builtin("__lslll",LLONG,LLONG,0,INT,0,1,0);

  declare_builtin("__divll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__divull",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__modll",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__modull",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__lsrll",LLONG,LLONG,0,INT,0,1,0);
  declare_builtin("__lsrull",UNSIGNED|LLONG,UNSIGNED|LLONG,0,INT,0,1,0);
  declare_builtin("__cmpll",INT,LLONG,0,LLONG,0,1,0);
  declare_builtin("__cmpull",INT,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__sint64toflt32",FLOAT,LLONG,0,0,0,1,0);
  declare_builtin("__uint64toflt32",FLOAT,UNSIGNED|LLONG,0,0,0,1,0);
  declare_builtin("__sint64toflt64",DOUBLE,LLONG,0,0,0,1,0);
  declare_builtin("__uint64toflt64",DOUBLE,UNSIGNED|LLONG,0,0,0,1,0);
  declare_builtin("__flt32tosint64",LLONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt32touint64",UNSIGNED|LLONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64tosint64",LLONG,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint64",UNSIGNED|LLONG,DOUBLE,0,0,0,1,0);

  return 1;
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
{
    if(ISFLOAT(t->flags)) return 9;
    if((t->flags&NQ)==LLONG) return axdx;
    if(ISSCALAR(t->flags)) return ax;
    return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  if(r==axdx){
    p->r1=ax;
    p->r2=dx;
    return 1;
  }else if(r==sidi){
    p->r1=si;
    p->r2=di;
    return 1;
  }
  return 0;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
    if(r==0) return(0);
    t&=NQ;
    if(r>8&&r<axdx){
      if(g_flags[7]&USEDFLAG) return 0;
      if(ISFLOAT(t)) return 1;
        else                  return 0;
    }
    if(r==axdx||r==sidi) 
      return t==LLONG;
    if(t==CHAR&&(r==si||r==di||r==bp)) return 0;
    if(t<=BLONG) return 1;
    if(ISPOINTER(t)) return 1;
    return 0;
}

int dangerous_IC(struct IC *p)
/*  Returns zero if the IC p can be safely executed     */
/*  without danger of exceptions or similar things.     */
/*  vbcc may generate code in which non-dangerous ICs   */
/*  are sometimes executed although control-flow may    */
/*  never reach them (mainly when moving computations   */
/*  out of loops).                                      */
/*  Typical ICs that generate exceptions on some        */
/*  machines are:                                       */
/*      - accesses via pointers                         */
/*      - division/modulo                               */
/*      - overflow on signed integer/floats             */
{
  int c=p->code;
  if((p->q1.flags&DREFOBJ)||(p->q2.flags&DREFOBJ)||(p->z.flags&DREFOBJ))
    return 1;
  if((c==DIV||c==MOD)&&!isconst(q2))
    return 1;
  return 0;
}

int must_convert(int o,int t,int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
/*  In this generic 32bit RISC cpu pointers and 32bit   */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
{
  int op=o&NQ,tp=t&NQ,of,tf;
  of=ISBE(op);
  tf=ISBE(tp);
  if(of!=tf) return 1;
  if(of) op=LETYPE(op);
  if(tf) tp=LETYPE(tp);
    
  if(tp==POINTER&&op==POINTER) return 0;
  if((t&UNSIGNED)&&(o&UNSIGNED)&&zmeqto(sizetab[tp],sizetab[op])) return 0;
  if((tp==INT&&op==LONG)||(tp==LONG&&op==INT)) return 0;
  if(op==tp) return 0;
  return 1;
}

/* Return name of library function, if this node should be
   implemented via libcall. */
char *use_libcall(int c,int t,int t2)
{
  static char fname[16];
  char *ret=0;

  if(c==COMPARE){
    if((t&NQ)==LLONG){
      sprintf(fname,"__cmp%sll",(t&UNSIGNED)?"u":"");
      ret=fname;
    }
  }else{
    t&=NU;
    if(c==CONVERT){
      if(ISFLOAT(t)&&(t2&NU)==LLONG){
	sprintf(fname,"__%cint64toflt%d",(t2&UNSIGNED)?'u':'s',(t==FLOAT)?32:64);
	ret=fname;
      }
      if(ISFLOAT(t2)&&(t&NU)==LLONG){
	sprintf(fname,"__flt%dto%cint64",((t2&NU)==FLOAT)?32:64,(t&UNSIGNED)?'u':'s');
	ret=fname;
      }
    }
    if((t&NQ)==LLONG){
      if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==PMULT||(c>=EQUAL&&c<=GREATEREQ)||c==KOMPLEMENT||c==MINUS){
	if(t==(UNSIGNED|LLONG)&&(c==DIV||c==MOD||c==RSHIFT)){
	  sprintf(fname,"__%sull",ename[c]);
	  ret=fname;
	}else if((t&NQ)==LLONG){
	  sprintf(fname,"__%sll",ename[c]);
	  ret=fname;
	}else printf("un %d\n",c);
      }
    }
  }
  return ret;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  title(f);
  if(newobj) emit(f,"%ld\n",zm2l(size));
  else   emit(f,"\t.space\t%ld\n",zm2l(size));
  newobj=0;
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  title(f);
  if(zm2l(align)>1) emit(f,"\t.align\t4\n");
}

void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;
  title(f);
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->storage_class==STATIC){
    if((v->vtyp->flags&NQ)==FUNKT) return;
    emit(f,"\t.type\t%s%ld,@object\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,%ld\n",labprefix,zm2l(v->offset),zm2l(szof(v->vtyp)));
    if(v->clist&&(!constflag||(g_flags[3]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
    if(v->clist&&constflag&&!(g_flags[3]&USEDFLAG)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
    if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
    if(section!=BSS)
      emit(f,"\t.align\t4\n%s%ld:\n",labprefix,zm2l(v->offset));
    else{
      emit(f,"\t.lcomm\t%s%ld,",labprefix,zm2l(v->offset));
      newobj=1;
    }
  }
  if(v->storage_class==EXTERN){
    emit(f,"\t.globl\t%s%s\n",idprefix,v->identifier);
    if(v->flags&(DEFINED|TENTATIVE)){
      emit(f,"\t.type\t%s%s,@object\n",idprefix,v->identifier);
      emit(f,"\t.size\t%s%s,%ld\n",idprefix,v->identifier,zm2l(szof(v->vtyp)));
      if(v->clist&&(!constflag||(g_flags[3]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
      if(v->clist&&constflag&&!(g_flags[3]&USEDFLAG)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
      if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
      if(section!=BSS)
        emit(f,"\t.align\t4\n%s%s:\n",idprefix,v->identifier);
      else{
        emit(f,"\t.comm\t%s%s,",idprefix,v->identifier);
	newobj=1;
      }
    }
  }
}

void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
    title(f);
    emit(f,"\t.%s\t",dct[t&NQ]);
    if(!p->tree){
        if(ISFLOAT(t)){
        /*  auch wieder nicht sehr schoen und IEEE noetig   */
            unsigned char *ip;
            ip=(unsigned char *)&p->val.vdouble;
            emit(f,"0x%02x%02x%02x%02x",ip[3],ip[2],ip[1],ip[0]);
            if((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE){
                emit(f,",0x%02x%02x%02x%02x",ip[7],ip[6],ip[5],ip[4]);
            }
	}else if((t&NQ)==LLONG){
	  zumax tmp;
	  eval_const(&p->val,t);
	  tmp=vumax;
	  vumax=zumand(vumax,ul2zum(0xffffffff));
	  gval.vulong=zum2zul(vumax);
	  emitval(f,&gval,UNSIGNED|LONG);
	  emit(f,",");
	  vumax=zumand(zumrshift(tmp,ul2zum(32UL)),ul2zum(0xffffffff));
	  gval.vulong=zum2zul(vumax);
	  emitval(f,&gval,UNSIGNED|LONG);
        }else{
            emitval(f,&p->val,t&NU);
        }
    }else{
        int m=p->tree->o.flags;
        p->tree->o.flags&=~VARADR;
        emit_obj(f,&p->tree->o,t&NU);
        p->tree->o.flags=m;
    }
    emit(f,"\n");newobj=0;
}

/*  The main code-generation routine.                   */
/*  f is the stream the code should be written to.      */
/*  p is a pointer to a doubly linked list of ICs       */
/*  containing the function body to generate code for.  */
/*  v is a pointer to the function.                     */
/*  offset is the size of the stackframe the function   */
/*  needs for local variables.                          */
void gen_code(FILE *f,struct IC *p,struct Var *v,zmax offset)
{
    int c,t,lastcomp=0,reg;
    struct IC *m;
    if(DEBUG&1) printf("gen_code()\n");
    for(c=1;c<=15;c++) regs[c]=regsa[c];
    regs[16]=0;
    for(c=1;c<=MAXR;c++){
      if(regsa[c]||regused[c]){
	BSET(regs_modified,c);
      }
    }
    title(f);
    loff=((zm2l(offset)+1)/2)*2;
    for(m=p;m;m=m->next){
      /* do we need another stack-slot? */
      /* could do a more precise check */
      if((m->code==PUSH&&(m->q1.flags&(REG|DREFOBJ))==DREFOBJ)||
	 (m->code==CALL&&(m->q1.flags&DREFOBJ))||
	 (m->code==CONVERT&&!ISFLOAT(m->typf)&&ISFLOAT(m->typf2)) ){
	loff+=4;
	break;
      }
    }
    function_top(f,v,loff);
    stackoffset=notpopped=dontpop=0;
    finit();
    for(;p;pr(f,p),p=p->next){
        c=p->code;t=p->typf;
	if(debug_info)
	  dwarf2_line_info(f,p);
        if(c==NOP) continue;
        if(c==SUBPFP) c=SUB;
        if(c==SUBIFP) c=SUB;
        if(c==ADDI2P) c=ADD;
        if(c==ALLOCREG){
            regs[p->q1.reg]=1;
            continue;
        }
        if(c==FREEREG){
            if(p->q1.reg>=9){
                for(c=0;c<8;c++)
                    if(fst[c]==p->q1.reg) fst[c]=0;
            }
            regs[p->q1.reg]=0;
            continue;
        }
        if(notpopped&&!dontpop){
	  if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
	    emit(f,"\taddl\t$%ld,%%esp\n",notpopped);
	    pop(notpopped);
	    notpopped=0;
	  }
        }

	if(c==COMPARE&&isconst(q2)&&(t&NQ)!=LLONG){
	  struct case_table *ct=calc_case_table(p,JUMP_TABLE_DENSITY);
	  if(ct&&ct->num>=JUMP_TABLE_LENGTH){
	    int r,defl,tabl=++label;
	    long l;unsigned long ul;
	    /*FIXME: we do not generate a jump-table if we do not have a
	      free register */
	    for(r=1;r<=8;r++){
	      if(!regs[r]&&regscratch[r])
		break;
	    }
	    if(r<=8){
	      BSET(regs_modified,r);
	      if(ct->next_ic->code==BRA)
		defl=ct->next_ic->typf;
	      else
		defl=++label;
	      if((p->q1.flags&(REG|DREFOBJ))==DREFOBJ){
		p->q1.flags&=~DREFOBJ;
		emit(f,"\tmovl\t");
		emit_obj(f,&p->q1,POINTER);
		emit(f,",%s\n",regnames[r]);
		p->q1.flags|=(REG|DREFOBJ);
		p->q1.reg=r;
	      }
	      emit(f,"\tmovl\t");
	      emit_obj(f,&p->q1,t);
	      emit(f,",%s\n",regnames[r]);
	      emit(f,"\tsubl\t$");
	      if(t&UNSIGNED)
		emitzum(f,ct->min.vumax);
	      else
		emitzm(f,ct->min.vmax);
	      emit(f,",%s\n",regnames[r]);
	      emit(f,"\tcmpl\t$");
	      emitzum(f,ct->diff);
	      emit(f,",%s\n",regnames[r]);
	      emit(f,"\tja\t%s%d\n",labprefix,defl);
	      emit(f,"\tjmp\t*%s%d(,%s,4)\n",labprefix,tabl,regnames[r]);
	      emit(f,"\t.align\t2\n");
	      emit(f,"%s%d:\n",labprefix,tabl);
	      emit_jump_table(f,ct,"\t.long\t",labprefix,defl);
	      if(ct->next_ic->code!=BRA){
		emit(f,"%s%d:\n",labprefix,defl);
		p=ct->next_ic->prev;
	      }else
		p=ct->next_ic;
	      continue;
	    }
	  }
	}
        if(c==LABEL){
	  if(t>label) ierror(0);
	  forder(f);
	  emit(f,"%s%d:\n",labprefix,t);
	  continue;
	}
        if(c>=BEQ&&c<=BRA){
            forder(f);
            if(lastcomp&UNSIGNED) emit(f,"\tj%s\t%s%d\n",ccu[c-BEQ],labprefix,t);
                else              emit(f,"\tj%s\t%s%d\n",ccs[c-BEQ],labprefix,t);
            continue;
        }
        if(c==MOVETOREG){
            if(p->z.reg>8){
                for(c=0;c<8;c++){
                    if(fst[c]==p->z.reg) fst[c]=0;
                }
                fload(f,&p->q1,DOUBLE);
                fst[0]=p->z.reg;
                continue;
            }
            move(f,&p->q1,0,0,p->z.reg,LONG);
            continue;
        }
        if(c==MOVEFROMREG){
            if(p->q1.reg>8){
                if(fst[0]!=p->q1.reg){
                    for(c=0,reg=-1;c<8;c++){
                        if(fst[c]==p->q1.reg) reg=c;
                    }
                    if(reg<0) ierror(0);
                    fxch(f,reg);
                }
                emit(f,"\tfstpl\t");emit_obj(f,&p->z,DOUBLE);
                emit(f,"\n");fpop();
                continue;
            }
            move(f,0,p->q1.reg,&p->z,0,LONG);
            continue;
        }
        if((p->q1.flags&(DREFOBJ|REG|KONST))==DREFOBJ||((p->q1.flags&DREFOBJ)&&ISBE(p->q1.dtyp))){
	  reg=get_reg(f,p,LONG);
	  move(f,&p->q1,0,0,reg,LONG);
	  p->q1.flags|=REG;p->q1.reg=reg;
	  p->q1.flags&=~KONST;
	  if(ISBE(p->q1.dtyp))
	    emit(f,"\tbswap\t%s\n",regnames[reg]);
        }
        if((p->q2.flags&(DREFOBJ|REG|KONST))==DREFOBJ||((p->q2.flags&DREFOBJ)&&ISBE(p->q2.dtyp))){
	  reg=get_reg(f,p,LONG);
	  move(f,&p->q2,0,0,reg,LONG);
	  p->q2.flags|=REG;p->q2.reg=reg;
	  p->q2.flags&=~KONST;
	  if(ISBE(p->q2.dtyp))
	    emit(f,"\tbswap\t%s\n",regnames[reg]);
        }
        if((p->z.flags&(DREFOBJ|REG|KONST))==DREFOBJ||((p->z.flags&DREFOBJ)&&ISBE(p->z.dtyp))){
	  reg=get_reg(f,p,LONG);
	  /* sehr unschoen */
	  if(c==GETRETURN&&reg==p->q1.reg) reg=get_reg(f,p,LONG);
	  move(f,&p->z,0,0,reg,LONG);
	  p->z.flags|=REG;p->z.reg=reg;
	  p->z.flags&=~KONST;
	  if(ISBE(p->z.dtyp))
	    emit(f,"\tbswap\t%s\n",regnames[reg]);
        }
	if((t&NQ)==LDOUBLE) p->typf=t=DOUBLE;
	if(c==CONVERT&&(t&NQ)==LLONG&&(p->typf2&NQ)==LLONG) c=ASSIGN;
        if(c==CONVERT){
            int to=p->typf2&NU;
	    if(to==LDOUBLE) to=DOUBLE;
            if((t&NU)==LONG) t=INT;
            if((t&NU)==(UNSIGNED|LONG)||(t&NU)==POINTER) t=(UNSIGNED|INT);
            if((to&NU)==LONG) to=INT;
            if((to&NU)==(UNSIGNED|LONG)||(to&NU)==POINTER) to=(UNSIGNED|INT);
	    if((to&NQ)==LLONG){
	      int useqreg=0;
	      if(isreg(z)) 
		reg=p->z.reg;
	      else if(isreg(q1)&&reg_pair(p->q1.reg,&rp)&&regok(rp.r1,t,0))
		reg=useqreg=rp.r1;
	      else
		reg=get_reg(f,p,INT);
	      if(!useqreg){
		emit(f,"\tmovl\t");
		if(isreg(q1)){
		  if(!reg_pair(p->q1.reg,&rp)) ierror(0);
		  emit(f,"%s",regnames[rp.r1]);
		}else{
		  emit_obj(f,&p->q1,INT);
		}
		emit(f,",%s\n",regnames[reg]);
	      }
	      if(!isreg(z)||p->z.reg!=reg)
		move(f,0,reg,&p->z,0,t);
	      continue;
	    }
            if((to&NQ)<=INT&&(t&NQ)<=LLONG){
	      if(isreg(z)){
		reg=p->z.reg;
		if(reg_pair(reg,&rp)) reg=rp.r1;
	      }else if(isreg(q1)&&regok(p->q1.reg,t,0))
		reg=p->q1.reg;
	      else
		reg=get_reg(f,p,((to&NQ)==CHAR||(t&NQ)==CHAR)?CHAR:LONG);
	      if((to&NQ)<=SHORT){
		emit(f,"\tmov%c%cl\t",(to&UNSIGNED)?'z':'s',x_t[to&NQ]);
		emit_obj(f,&p->q1,to);
		emit(f,",%s\n",regnames[reg]);
	      }else{
		move(f,&p->q1,0,0,reg,to);
	      }
	      if((t&NQ)==LLONG){
		if(isreg(z)){
		  if(to&UNSIGNED)
		    emit(f,"\txorl\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
		  else{
		    move(f,0,reg,0,rp.r2,INT);
		    emit(f,"\tsarl\t$31,%s\n",regnames[rp.r2]);
		  }
		}else{
		  move(f,0,reg,&p->z,0,INT);
		  p->z.val.vmax=zmadd(p->z.val.vmax,l2zm(4L));
		  if(to&UNSIGNED){
		    emit(f,"\tmovl\t$0,");
		    emit_obj(f,&p->z,t);
		    emit(f,"\n");
		  }else{
		    emit(f,"\tsarl\t$31,%s\n",regnames[reg]);
		    move(f,0,reg,&p->z,0,INT);
		  }
		  p->z.val.vmax=zmsub(p->z.val.vmax,l2zm(4L));
		}
	      }else
		move(f,0,reg,&p->z,0,t);
	      continue;
            }
            if(ISFLOAT(t)){
                if(ISFLOAT(to)){
                    if(isreg(q1)&&fst[0]==p->q1.reg){
                        if(isreg(z)){
                            if(p->z.reg==fst[0]) continue;
                            for(reg=0,c=7;c>=0;c--){
                                if(fst[c]==p->z.reg){reg=c;break;}
                                if(fst[c]<0) reg=c;
                            }
                            fst[reg]=p->z.reg;
                        }
                        emit(f,"\tfst%c\t",x_t[t&NQ]);
                        emit_obj(f,&p->z,t);emit(f,"\n");
                        continue;
                    }
                    fload(f,&p->q1,to);
                    fstore(f,&p->z,t);
                    continue;
                }
                if((to&NQ)<=SHORT){
                    if(isreg(q1)){
                        reg=p->q1.reg;
                        if(to&UNSIGNED){
                            emit(f,"\tandl\t$%ld,%s\n",(to&NQ)==CHAR?255L:65535L,regnames[reg]);
                        }else{
/*                            emit(f,"\tc%ctl\t%s\n",x_t[to&NQ],regnames[reg]);*/
                            if((to&NQ)==SHORT){
                                emit(f,"\tmovswl\t%%%s,%s\n",regnames[reg]+2,regnames[reg]);
                            }else{
                                emit(f,"\tmovsbl\t%%%cl,%s\n",regnames[reg][2],regnames[reg]);
                            }
                        }
                    }else{
                        reg=get_reg(f,p,LONG);
                        if(to&UNSIGNED){
                            emit(f,"\tmovz%cl\t",x_t[to&NQ]);
                        }else{
                            emit(f,"\tmovs%cl\t",x_t[to&NQ]);
                        }
                        emit_obj(f,&p->q1,to);emit(f,",%s\n",regnames[reg]);
                    }
                    emit(f,"\tpushl\t%s\n",regnames[reg]);
                    emit(f,"\tfildl\t(%s)\n\taddl\t$4,%s\n",regnames[sp],regnames[sp]);
                }else{
                    if(to&UNSIGNED){
                        emit(f,"\tpushl\t$0\n\tpushl\t");
			push(4);
                        emit_obj(f,&p->q1,to);
                        emit(f,"\n\tfildq\t(%s)\n\taddl\t$8,%s\n",regnames[sp],regnames[sp]);
			pop(4);
                    }else{
                        if(isreg(q1)){
                            emit(f,"\tpushl\t%s\n\tfildl\t(%s)\n\taddl\t$4,%s\n",regnames[p->q1.reg],regnames[sp],regnames[sp]);
                        }else{
                            emit(f,"\tfildl\t");emit_obj(f,&p->q1,t);
                            emit(f,"\n");
                        }
                    }
                }
                fpush(f);
                fstore(f,&p->z,t);
                continue;
            }
            if(ISFLOAT(to)){
	      int cwr;
	      if(isreg(z)&&regok(p->z.reg,CHAR,0))
		cwr=p->z.reg;
	      else
		cwr=get_reg(f,p,CHAR);
	      emit(f,"\tfnstcw\t%d(%s)\n",loff-4-stackoffset,regnames[sp]);
	      emit(f,"\tmovw\t%d(%s),%%%s\n",loff-4-stackoffset,regnames[sp],regnames[cwr]+2);
	      emit(f,"\tmovb\t$12,%%%ch\n",regnames[cwr][2]);
	      emit(f,"\tmovw\t%%%s,%d(%s)\n",regnames[cwr]+2,loff-2-stackoffset,regnames[sp]);
	      emit(f,"\tfldcw\t%d(%s)\n",loff-2-stackoffset,regnames[sp]);
	      if(isreg(q1)&&fst[0]==p->q1.reg){
		if((t&NQ)==CHAR){
		  if(isreg(z)) reg=p->z.reg; else reg=get_reg(f,p,CHAR);
		  emit(f,"\tsubl\t$4,%s\n\tfistl\t(%s)\n\tmovsbl\t(%s),%s\n\taddl\t$4,%s\n",regnames[sp],regnames[sp],regnames[sp],regnames[reg],regnames[sp]);
		  move(f,0,reg,&p->z,0,t);
		}else{
		  if(isreg(z)){
		    emit(f,"\tsubl\t$4,%s\n\tfistl\t(%s)\n\tmov%c\t(%s),",regnames[sp],regnames[sp],x_t[t&NQ],regnames[sp]);
		    push(4);
		    emit_obj(f,&p->z,t);emit(f,"\n\taddl\t$4,%s\n",regnames[sp]);
		    pop(4);
		  }else{
		    emit(f,"\tfist%c\t",x_t[t&NQ]);
		    emit_obj(f,&p->z,t);emit(f,"\n");
		  }
		}
	      }else{
		fload(f,&p->q1,to);
		if((t&NQ)==CHAR){
		  if(isreg(z)) reg=p->z.reg; else reg=get_reg(f,p,CHAR);
		  emit(f,"\tsubl\t$4,%s\n\tfistpl\t(%s)\n\tmovsbl\t(%s),%s\n\taddl\t$4,%s\n",regnames[sp],regnames[sp],regnames[sp],regnames[reg],regnames[sp]);
		  fpop(); move(f,0,reg,&p->z,0,t);
		}else{
		  if(isreg(z)){
		    emit(f,"\tsubl\t$4,%s\n\tfistpl\t(%s)\n\tmov%c\t(%s),",regnames[sp],regnames[sp],x_t[t&NQ],regnames[sp]);
		    push(4);
		    emit_obj(f,&p->z,t);
		    emit(f,"\n\taddl\t$4,%s\n",regnames[sp]);
		    pop(4);
		    fpop();
		  }else{
		    emit(f,"\tfistp%s\t",(t&NQ)==SHORT?"":"l");
		    emit_obj(f,&p->z,t);emit(f,"\n");fpop();
		  }
		}
	      }
	      emit(f,"\tfldcw\t%d(%s)\n",loff-4-stackoffset,regnames[sp]);
	      continue;
            }
pric2(stdout,p);
            ierror(0);
        }
        if(c==MINUS||c==KOMPLEMENT){
            char *s;
            if(ISFLOAT(t)){
                if(isreg(z)&&p->z.reg==9&&isreg(q1)&&p->q1.reg==9){
                    emit(f,"\tfchs\n");
                    continue;
                }
                fload(f,&p->q1,t);
                emit(f,"\tfchs\n");
                emit(f,"\tfstp%c\t",x_t[t&NQ]);
                emit_obj(f,&p->z,t);emit(f,"\n");
                fpop();
                continue;
            }
            if(c==MINUS) s="neg"; else s="not";
            if(compare_objects(&p->q1,&p->q2)){
                emit(f,"\t%s%c\t",s,x_t[t&NQ]);
                emit_obj(f,&p->z,t);emit(f,"\n");
                continue;
            }
            if(isreg(z)) reg=p->z.reg; else reg=get_reg(f,p,t);
            move(f,&p->q1,0,0,reg,t);
            emit(f,"\t%s%c\t%s\n",s,x_t[t&NQ],regnames[reg]);
            move(f,0,reg,&p->z,0,t);
            continue;
        }
        if(c==SETRETURN){
            if(p->z.reg){
                if(p->z.reg==9){
                    if(!isreg(q1)||fst[0]!=p->q1.reg)
                        fload(f,&p->q1,t);
                }else{
                    move(f,&p->q1,0,0,p->z.reg,t);
                }
		BSET(regs_modified,p->z.reg);
            }
            continue;
        }
        if(c==GETRETURN){
            if(p->q1.reg){
                if(p->q1.reg==9){
                    if(!isreg(z)||fst[0]!=p->z.reg)
                        fstore(f,&p->z,t);
                }else{
                    move(f,0,p->q1.reg,&p->z,0,t);
                }
            }
            continue;
        }
        if(c==CALL){
            int reg;
            if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
              emit_inline_asm(f,p->q1.v->fi->inline_asm);
            }else{
              if(p->q1.flags&DREFOBJ){
                if(!(p->q1.flags&REG)) ierror(0);
                emit(f,"\tcall\t*%s\n",regnames[p->q1.reg]);
              }else{
                emit(f,"\tcall\t");emit_obj(f,&p->q1,t);
                emit(f,"\n");
              }
            }
            if(!zmeqto(l2zm(0L),p->q2.val.vmax)){
              notpopped+=zm2l(p->q2.val.vmax);
              dontpop-=zm2l(p->q2.val.vmax);
              if(!(g_flags[2]&USEDFLAG)&&!vlas&&stackoffset==-notpopped){
                /*  Entfernen der Parameter verzoegern  */
              }else{
                emit(f,"\taddl\t$%ld,%%esp\n",zm2l(p->q2.val.vmax));
                pop(zm2l(p->q2.val.vmax));
                notpopped-=zm2l(p->q2.val.vmax);
              }
            }
	    if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)){
	      bvunite(regs_modified,p->q1.v->fi->regs_modified,RSIZE);
	    }else{
	      int i;
	      for(i=1;i<=MAXR;i++){
		if(regscratch[i]) BSET(regs_modified,i);
	      }
	    }
            continue;
        }
        if(c==ASSIGN||c==PUSH){
            if(c==PUSH) dontpop+=zm2l(p->q2.val.vmax);
            if(ISFLOAT(t)){
                if(c==ASSIGN){
                    prfst(f,"fassign");
                    fload(f,&p->q1,t);
                    fstore(f,&p->z,t);
                    continue;
                }else if(isreg(q1)){
                    prfst(f,"fpush");
                    emit(f,"\tsubl\t$%ld,%s\n",zm2l(sizetab[t&NQ]),regnames[sp]);
                    push(zm2l(sizetab[t&NQ]));
                    if(fst[0]==p->q1.reg){
                        emit(f,"\tfst%c\t(%s)\n",x_t[t&NQ],regnames[sp]);
                    }else{
                        fload(f,&p->q1,t);
                        emit(f,"\tfstp%c\t(%s)\n",x_t[t&NQ],regnames[sp]);
                        fpop();
                    }
                    continue;
                }
            }
	    if((t&NQ)==LLONG){
	      if(c==ASSIGN){
		if(isconst(q1)||isreg(q1)||isreg(z)){
		  move(f,&p->q1,0,&p->z,0,t);
		}else{
		  int r=get_reg(f,p,INT);
		  emit(f,"\tmovl\t");
		  emit_lword(f,&p->q1);
		  emit(f,",%s\n",regnames[r]);
		  emit(f,"\tmovl\t%s,",regnames[r]);
		  emit_lword(f,&p->z);
		  emit(f,"\n");
		  emit(f,"\tmovl\t");
		  emit_hword(f,&p->q1);
		  emit(f,",%s\n",regnames[r]);
		  emit(f,"\tmovl\t%s,",regnames[r]);
		  emit_hword(f,&p->z);
		  emit(f,"\n");
		}
	      }else{
		emit(f,"\tpushl\t");
		emit_hword(f,&p->q1);
		emit(f,"\n");
		push(4);
		emit(f,"\tpushl\t");
		emit_lword(f,&p->q1);
		emit(f,"\n");
		push(4);
	      }
	      continue;
	    }
            if(!ISSCALAR(t)||!zmeqto(p->q2.val.vmax,sizetab[t&NQ])||!zmleq(p->q2.val.vmax,l2zm(4L))){
                int mdi=di,msi=si,m=0;long l;
                l=zm2l(p->q2.val.vmax);
                if(regs[cx]){m|=1;if(!cxl)cxl=++label;emit(f,"\tmovl\t%s,%s%d\n",regnames[cx],labprefix,cxl);}
                if(regs[msi]||!regused[msi]){m|=2;if(!sil)sil=++label;emit(f,"\tmovl\t%s,%s%d\n",regnames[msi],labprefix,sil);}
                if(regs[mdi]||!regused[mdi]){m|=4;if(!dil)dil=++label;emit(f,"\tmovl\t%s,%s%d\n",regnames[mdi],labprefix,dil);}
                if((p->z.flags&REG)&&p->z.reg==msi&&(p->q1.flags&REG)&&p->q1.reg==mdi){
                    msi=di;mdi=si;
                    m|=8;
                }
                if(!(p->z.flags&REG)||p->z.reg!=msi){
                    emit(f,"\tleal\t");emit_obj(f,&p->q1,t);
                    emit(f,",%s\n",regnames[msi]);
                }
                if(c==PUSH){
                    emit(f,"\tsubl\t$%ld,%s\n\tmovl\t%s,%s\n",l,regnames[sp],regnames[sp],regnames[mdi]);
                    push(l);
		    l=zm2l(p->z.val.vmax);
                }else{
                    emit(f,"\tleal\t");emit_obj(f,&p->z,t);
                    emit(f,",%s\n",regnames[mdi]);
                }
                if((p->z.flags&REG)&&p->z.reg==msi){
                    emit(f,"\tleal\t");emit_obj(f,&p->q1,t);
                    emit(f,",%s\n",regnames[msi]);
                }
                if(m&8){
                    msi=si;mdi=di;
                    emit(f,"\txchg\t%s,%s\n",regnames[msi],regnames[mdi]);
                }
                if((t&NQ)==ARRAY||(t&NQ)==CHAR||l<4){
                    emit(f,"\tmovl\t$%ld,%s\n\trep\n\tmovsb\n",l,regnames[cx]);
                }else{
                    if(l>=8)
                        emit(f,"\tmovl\t$%ld,%s\n\trep\n",l/4,regnames[cx]);
                    emit(f,"\tmovsl\n");
                    if(l&2) emit(f,"\tmovsw\n");
                    if(l&1) emit(f,"\tmovsb\n");
                }
                if(m&4) emit(f,"\tmovl\t%s%d,%s\n",labprefix,dil,regnames[mdi]);
                if(m&2) emit(f,"\tmovl\t%s%d,%s\n",labprefix,sil,regnames[msi]);
                if(m&1) emit(f,"\tmovl\t%s%d,%s\n",labprefix,cxl,regnames[cx]);
                continue;
            }
            if(c==PUSH){
                emit(f,"\tpush%c\t",(t&NQ)==FLOAT?x_t[LONG]:x_t[t&NQ]);
                emit_obj(f,&p->q1,t);emit(f,"\n");
                push(zm2l(p->q2.val.vmax));
                continue;
            }
            if(c==ASSIGN){
                if(isconst(q1)){
                    move(f,&p->q1,0,&p->z,0,t);
                    continue;
                }
                if(isreg(z)) reg=p->z.reg;
                else if(isreg(q1)) reg=p->q1.reg;
                else reg=get_reg(f,p,t);
                move(f,&p->q1,0,0,reg,t);
                move(f,0,reg,&p->z,0,t);
                continue;
            }
            ierror(0);
        }
        if(c==ADDRESS){
            if(isreg(z)) reg=p->z.reg; else reg=get_reg(f,p,LONG);
            emit(f,"\tleal\t");emit_obj(f,&p->q1,t);
            emit(f,",%s\n",regnames[reg]);
            move(f,0,reg,&p->z,0,POINTER);
            continue;
        }
        if(c==TEST){
            lastcomp=t;
            if(ISFLOAT(t)){
                if(isreg(q1)&&fst[0]==p->q1.reg){
                    emit(f,"\tftst\n");lastcomp|=UNSIGNED;
                    continue;
                }else{
                    p->code=c=COMPARE;
                    p->q2.flags=KONST;
                    p->q2.val.vldouble=d2zld(0.0);
                    if((t&NQ)==FLOAT) p->q2.val.vfloat=zld2zf(p->q2.val.vldouble);
                    if((t&NQ)==DOUBLE) p->q2.val.vdouble=zld2zd(p->q2.val.vldouble);

                    /*  fall through to COMPARE */
                }
            }else{
              if(isvconst(q1)){
                reg=get_reg(f,p,t);
                move(f,&p->q1,0,0,reg,t);
                p->q1.flags=REG;
                p->q1.reg=reg;
              }
	      if((t&NQ)==LLONG){
		reg=get_reg(f,p,INT);
		if(isreg(q1)){
		  if(!reg_pair(p->q1.reg,&rp)) ierror(0);
		  move(f,0,rp.r1,0,reg,INT);
		  emit(f,"\torl\t%s,%s\n",regnames[rp.r2],regnames[reg]);
		}else{
		  move(f,&p->q1,0,0,reg,INT);
		  p->q1.val.vmax=zmadd(p->q1.val.vmax,l2zm(4L));
		  emit(f,"\torl\t");
		  emit_obj(f,&p->q1,INT);
		  emit(f,",%s\n",regnames[reg]);
		  p->q1.val.vmax=zmadd(p->q1.val.vmax,l2zm(4L));
		}
		continue;
	      }
	      if(isreg(q1)){
		emit(f,"\ttest%c\t",x_t[t&NQ]);
                emit_obj(f,&p->q1,t);emit(f,",");
                emit_obj(f,&p->q1,t);emit(f,"\n");
	      }else{
		emit(f,"\tcmp%c\t$0,",x_t[t&NQ]);
		emit_obj(f,&p->q1,t);emit(f,"\n");
	      }
	      continue;
            }
        }
        if(c==COMPARE){
            lastcomp=t;
            if(isreg(q2)||isvconst(q1)){
                struct IC *b=p->next;
                struct obj o;
                o=p->q1;p->q1=p->q2;p->q2=o;
                while(b&&b->code==FREEREG) b=b->next;
                if(!b) ierror(0);
                if(b->code==BLT) b->code=BGT;
                else if(b->code==BLE) b->code=BGE;
                else if(b->code==BGT) b->code=BLT;
                else if(b->code==BGE) b->code=BLE;
            }
            if(ISFLOAT(t)){
                prfst(f,"fcomp");
                if(isreg(q1)&&p->q1.reg==fst[0]){
                    emit(f,"\tfcom%c\t",x_t[t&NQ]);
                    emit_obj(f,&p->q2,t);emit(f,"\n");
                }else{
                    fload(f,&p->q1,t);
                    emit(f,"\tfcomp%c\t",x_t[t&NQ]);
                    emit_obj(f,&p->q2,t);emit(f,"\n");
                    fpop();
                }
		if(regs[ax]) emit(f,"\tpushl\t%s\n",regnames[ax]);
                emit(f,"\tfstsw\n\tsahf\n");
		if(regs[ax]) emit(f,"\tpopl\t%s\n",regnames[ax]);
                lastcomp|=UNSIGNED;
                continue;
            }
            if(!isreg(q1)){
                if(!isreg(q2)&&(!isvconst(q2)||isvconst(q1))){
                    reg=get_reg(f,p,t);
                    move(f,&p->q1,0,0,reg,t);
                    p->q1.flags=REG;
                    p->q1.reg=reg;
                }
            }
	    if(isreg(q1)&&isconst(q2)){
	      eval_const(&p->q2.val,t);
	      if(zmeqto(vmax,l2zm(0L))){
		emit(f,"\ttest%c\t",x_t[t&NQ]);
                emit_obj(f,&p->q1,t);emit(f,",");
                emit_obj(f,&p->q1,t);emit(f,"\n");
		continue;
	      }
	    }
	    if(isreg(q2)&&isconst(q1)){
	      eval_const(&p->q1.val,t);
	      if(zmeqto(vmax,l2zm(0L))){
		emit(f,"\ttest%c\t",x_t[t&NQ]);
                emit_obj(f,&p->q2,t);emit(f,",");
                emit_obj(f,&p->q2,t);emit(f,"\n");
		continue;
	      }
	    }
            emit(f,"\tcmp%c\t",x_t[t&NQ]);
            emit_obj(f,&p->q2,t);emit(f,",");
            emit_obj(f,&p->q1,t);emit(f,"\n");
            continue;
        }
        if(ISFLOAT(t)){
            char s[2];
            prfst(f,"fmath");
            if(isreg(q2)) s[0]=0; else {s[0]=x_t[t&NQ];s[1]=0;}
            if(isreg(z)&&isreg(q1)&&p->q1.reg==fst[0]&&p->z.reg==fst[0]){
                emit(f,"\t%s%s\t",farithmetics[c-LSHIFT],s);
                emit_obj(f,&p->q2,t); emit(f,"\n");continue;
            }
            fload(f,&p->q1,t);
            emit(f,"\t%s%s\t",farithmetics[c-LSHIFT],s);
            emit_obj(f,&p->q2,t); emit(f,"\n");
            fstore(f,&p->z,t); continue;
        }
        if((c==MULT||((c==DIV||c==MOD)&&(p->typf&UNSIGNED)))&&isconst(q2)){
            long ln;
            eval_const(&p->q2.val,t);
            if(zmleq(l2zm(0L),vmax)&&zumleq(ul2zum(0UL),vumax)){
                if(ln=pof2(vumax)){
                    if(c==MOD){
                        vmax=zmsub(vmax,l2zm(1L));
                        p->code=AND;
                    }else{
                        vmax=l2zm(ln-1);
                        if(c==DIV) p->code=RSHIFT; else p->code=LSHIFT;
                    }
                    c=p->code;
		    gval.vmax=vmax;
		    eval_const(&gval,MAXINT);
		    if(c==AND){
		      insert_const(&p->q2.val,t);
		    }else{
		      insert_const(&p->q2.val,INT);
		      p->typf2=INT;
		    }
                }
            }
        }
        if(c==MOD||c==DIV){
            int m=0;
	    BSET(regs_modified,dx);
	    BSET(regs_modified,ax);
            if(regs[ax]&&(!isreg(z)||p->z.reg!=ax)){
                emit(f,"\tpushl\t%s\n",regnames[ax]);
		push(4);
		m|=1;
            }
            if(regs[dx]&&(!isreg(z)||p->z.reg!=dx)){
                emit(f,"\tpushl\t%s\n",regnames[dx]);
		push(4);
		m|=2;
            }
            if((p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&(p->q2.reg==ax||p->q2.reg==dx)){
                move(f,&p->q2,0,0,dx,t);
                emit(f,"\tpushl\t%s\n",regnames[dx]);
		push(4);
                m|=8;
            }
	    if(isreg(q1)&&p->q1.reg==dx&&(m&10)==10)
	      emit(f,"\tmovl\t4(%s),%s\n",regnames[sp],regnames[ax]);
	    else
	      move(f,&p->q1,0,0,ax,t);
            if(isconst(q2)){
                emit(f,"\tpush%c\t",x_t[t&NQ]);emit_obj(f,&p->q2,t);
                emit(f,"\n");
		push(4);
		m|=4;
            }
            if(t&UNSIGNED) emit(f,"\txorl\t%s,%s\n\tdivl\t",regnames[dx],regnames[dx]);
                else       emit(f,"\tcltd\n\tidivl\t");
            if((m&12)||(isreg(q2)&&p->q2.reg==dx)){
                emit(f,"(%s)",regnames[sp]);
            }else if(isreg(q2)&&p->q2.reg==ax){
                emit(f,"%s(%s)",(m&2)?"4":"",regnames[sp]);
            }else{
                emit_obj(f,&p->q2,t);
            }
            emit(f,"\n");
            if(c==DIV) move(f,0,ax,&p->z,0,t);
                else   move(f,0,dx,&p->z,0,t);
            if(m&4){ emit(f,"\taddl\t$%ld,%s\n",zm2l(sizetab[t&NQ]),regnames[sp]);pop(4);}
            if(m&8){ emit(f,"\tpopl\t%s\n",regnames[dx]);pop(4);}
            if(m&2){ emit(f,"\tpopl\t%s\n",regnames[dx]);pop(4);}
            if(m&1){ emit(f,"\tpopl\t%s\n",regnames[ax]);pop(4);}
            continue;
        }
        if(!isconst(q2)&&(c==LSHIFT||c==RSHIFT)){
            char *s=arithmetics[c-LSHIFT];
            int fl=0;
            if(c==RSHIFT&&(t&UNSIGNED)) s="shr";
            if(((p->q1.flags&REG)&&p->q1.reg==cx)||((p->z.flags&REG)&&p->z.reg==cx)
               ||(!compare_objects(&p->q1,&p->z)&&!isreg(q1))){
                fl=regs[cx];regs[cx]=2; /* don't want cx */
                reg=get_reg(f,p,t);
                regs[cx]=fl;
                if(isreg(z)&&p->z.reg==cx) fl=0;
                if(fl){emit(f,"\tpushl\t%s\n",regnames[cx]);push(4);}
                move(f,&p->q1,0,0,reg,t);
                move(f,&p->q2,0,0,cx,t);
                emit(f,"\t%s%c\t%%cl,%s\n",s,x_t[t&NQ],mregname(reg,t));
                move(f,0,reg,&p->z,0,t);
                if(fl){emit(f,"\tpopl\t%s\n",regnames[cx]);pop(4);}
                continue;
            }else{
                if(!isreg(q2)||p->q2.reg!=cx){
                    if(regs[cx]){emit(f,"\tpushl\t%s\n",regnames[cx]);push(4);fl=1;}
                    move(f,&p->q2,0,0,cx,t);
                }
                if(compare_objects(&p->q1,&p->z)){
                    emit(f,"\t%s%c\t%%cl,",s,x_t[t&NQ]);
                    emit_obj(f,&p->z,t);emit(f,"\n");
                }else{
                    move(f,0,p->q1.reg,&p->z,0,t);
                    emit(f,"\t%s%c\t%%cl,",s,x_t[t&NQ]);
                    emit_obj(f,&p->z,t);emit(f,"\n");
                }
                if(fl) {emit(f,"\tpopl\t%s\n",regnames[cx]);pop(4);}
                continue;
            }
        }
        if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)){
            char *s;
            if(c>=OR&&c<=AND) s=logicals[c-OR];
                else s=arithmetics[c-LSHIFT];
            if(c==RSHIFT&&(t&UNSIGNED)) s="shr";
            if(c!=MULT&&compare_objects(&p->q1,&p->z)){
                if(isreg(z)||isreg(q1)||isconst(q2)){
                    if(isconst(q2)&&(c==ADD||c==SUB)){
                        eval_const(&p->q2.val,t);
                        if(zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))&&zldeqto(vldouble,d2zld(1.0))){
                            if(c==ADD) s="inc"; else s="dec";
                            emit(f,"\t%s%c\t",s,x_t[t&NQ]);
                            emit_obj(f,&p->z,t);emit(f,"\n");
                            continue;
                        }
                    }
                    emit(f,"\t%s%c\t",s,x_t[t&NQ]);
                    emit_obj(f,&p->q2,t);emit(f,",");
                    emit_obj(f,&p->z,t);emit(f,"\n");
                    continue;
                }else{
                    if(isreg(q2)) reg=p->q2.reg; else reg=get_reg(f,p,t);
                    move(f,&p->q2,0,0,reg,t);
                    emit(f,"\t%s%c\t%s",s,x_t[t&NQ],mregname(reg,t));
                    emit(f,","); emit_obj(f,&p->z,t);emit(f,"\n");
                    continue;
                }
            }
	    if(c==AND&&isreg(z)){
	      /* can we use test instruction? */
	      struct IC *p2=p->next;
	      while(p2->code==ALLOCREG||p2->code==FREEREG)
		p2=p2->next;
	      if(p2->code==TEST&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==p->z.reg){
		struct IC *p3=p2->next;
		while(p3->code==ALLOCREG||p3->code==FREEREG){
		  if(p3->code==FREEREG&&p3->q1.reg==p->z.reg)
		    break;
		  p3=p3->next;
		}
		if(p3->code==FREEREG&&p3->q1.reg==p->z.reg){
		  /* we can use test */
		  p2->code=NOP;
		  if(notpopped&&!dontpop){
		    emit(f,"\taddl\t$%ld,%%esp\n",notpopped);
		    pop(notpopped);
		    notpopped=0;
		  }
		  if(isconst(q1)){
		    struct obj tmp=p->q1;
		    p->q1=p->q2;
		    p->q2=tmp;
		  }
		  if(!isreg(q1)&&!isreg(q2)&&!isconst(q2)){
		    int r=get_reg(f,p,t);
		    move(f,&p->q1,0,0,r,t);
		    p->q1.flags=REG;
		    p->q1.reg=r;
		  }
		  emit(f,"\ttest%c\t",x_t[t&NQ]);
		  emit_obj(f,&p->q2,t);
		  emit(f,",");
		  emit_obj(f,&p->q1,t);
		  emit(f,"\n");
		  continue;
		}
	      }
	    }
	      
	    if(isreg(z)&&(p->q2.flags&REG)&&p->q2.reg==p->z.reg){
	      if(c==ADD||c==AND||c==OR||c==XOR){
		struct obj tmp;
		tmp=p->q1;
		p->q1=p->q2;
		p->q2=tmp;
	      }
	    }
            if(isreg(z)&&(!(p->q2.flags&REG)||p->q2.reg!=p->z.reg)) reg=p->z.reg; else reg=get_reg(f,p,t);
            move(f,&p->q1,0,0,reg,t);
            if(isconst(q2)&&(c==ADD||c==SUB)){
                eval_const(&p->q2.val,t);
                if(zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))&&zldeqto(vldouble,d2zld(1.0))){
                    if(c==ADD) s="inc"; else s="dec";
                    emit(f,"\t%s%c\t%s\n",s,x_t[t&NQ],mregname(reg,t));
                }else{
                    emit(f,"\t%s%c\t",s,x_t[t&NQ]);
                    emit_obj(f,&p->q2,t);emit(f,",%s\n",mregname(reg,t));
                }
            }else{
                emit(f,"\t%s%c\t",s,x_t[t&NQ]);
                emit_obj(f,&p->q2,t);emit(f,",%s\n",mregname(reg,t));
            }
            move(f,0,reg,&p->z,0,t);
            continue;
        }
        ierror(0);
    }
    if(notpopped){
        emit(f,"\taddl\t$%ld,%%esp\n",notpopped);
        pop(notpopped);notpopped=0;
    }
    function_bottom(f,v,loff);
    if(debug_info){
      emit(f,"%s%d:\n",labprefix,++label);
      dwarf2_function(f,v,label);
      if(f) section=-1;
    }          
}

int shortcut(int code,int typ)
{
  return(0);
}

void cleanup_cg(FILE *f)
{
  struct fpconstlist *p;
  unsigned char *ip;

  title(f);
  while(p=firstfpc){
    if(f){
      if(section!=CODE){emit(f,codename);if(f) section=CODE;}
      emit(f,"\t.align\t4\n");
      emit(f,"%s%d:\n\t.long\t",labprefix,p->label);
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[3],ip[2],ip[1],ip[0]);
      if((p->typ&NQ)==DOUBLE||(p->typ&NQ)==LDOUBLE){
	emit(f,",0x%02x%02x%02x%02x",ip[7],ip[6],ip[5],ip[4]);
      }
      emit(f,"\n");
    }
    firstfpc=p->next;
    free(p);
  }
  if(f){
    if(section!=BSS){emit(f,bssname);if(f) section=BSS;}
    if(cxl) emit(f,"\t.lcomm\t%s%d,4\n",labprefix,cxl);
    if(sil) emit(f,"\t.lcomm\t%s%d,4\n",labprefix,sil);
    if(dil) emit(f,"\t.lcomm\t%s%d,4\n",labprefix,dil);
  }
}
void init_db(FILE *f)
{
  dwarf2_setup(sizetab[POINTER],".byte",".2byte",".4byte",".4byte",labprefix,idprefix,".section");
  dwarf2_print_comp_unit_header(f);
}
void cleanup_db(FILE *f)
{
  dwarf2_cleanup(f);
  if(f) section=-1;
}


void insert_const(union atyps *p,int t)
/*  Traegt Konstante in entprechendes Feld ein.       */
{
  if(!p) ierror(0);
  t&=NU;
  if(ISBE(t))
    t=LETYPE(t);
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
  if(ISBE(f))
    f=LETYPE(f);
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
void printval(FILE *f,union atyps *p,int t)
/*  Gibt atyps aus.                                     */
{
  t&=NU;
  if(ISBE(t)){
    fprintf(f,"B");
    t=LETYPE(t);
  }
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
  /*FIXME?*/
  if(ISBE(t))
    t=LETYPE(t);
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

void conv_typ(struct Typ *p)
/* Erzeugt extended types in einem Typ. */
{
  char *attr;
  while(p){
    if(ISSCALAR(p->flags)&&(p->flags&NQ)!=CHAR&&(attr=p->attr)&&strstr(attr,"bigendian")){
      if(!ISBE(p->flags))
	p->flags++;
    }
    if(ISSCALAR(p->flags)&&(p->flags&NQ)!=CHAR&&(attr=p->attr)&&strstr(attr,"littleendian")){
      if(ISBE(p->flags))
	p->flags--;
    }
    p=p->next;
  }
}

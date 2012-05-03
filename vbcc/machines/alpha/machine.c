/*  Code generator for a DEC Alpha 64bit RISC cpu with 32       */
/*  general purpose and 32 floating point registers.            */

#include "supp.h"

static char FILE_[]=__FILE__;

#include "dwarf2.c"

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for DEC Alpha V0.3c (c) in 1997-2000 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,VALFLAG,0,0,0,0,0};
char *g_flags_name[MAXGF]={"cpu","fpu","const-in-data","sd","merge-constants","stabs","no-builtins"};
union ppi g_flags_val[MAXGF];

/*  Alignment-requirements for all types in bytes.              */
zmax align[MAX_TYPE+1];

/*  Alignment that is sufficient for every object.              */
zmax maxalign;

/*  CHAR_BIT for the target machine.                            */
zmax char_bit;

/*  Tabelle fuer die Groesse der einzelnen Typen                */
zmax sizetab[MAX_TYPE+1];

/*  Minimum and Maximum values each type can have.              */
/*  Must be initialized in init_cg().                           */
zmax t_min[MAX_TYPE+1];
zumax t_max[MAX_TYPE+1];
zumax tu_max[MAX_TYPE+1];

/*  Names of all registers.                                     */
char *regnames[MAXR+1]={"noreg",
                        "$0","$1","$2","$3","$4","$5","$6","$7",
                        "$8","$9","$10","$11","$12","$13","$14","$15",
                        "$16","$17","$18","$19","$20","$21","$22","$23",
                        "$24","$25","$26","$27","$28","$29","$30","$31",
                        "$f0","$f1","$f2","$f3","$f4","$f5","$f6","$f7",
                        "$f8","$f9","$f10","$f11","$f12","$f13","$f14","$f15",
                        "$f16","$f17","$f18","$f19","$f20","$f21","$f22","$f23",
                        "$f24","$f25","$f26","$f27","$f28","$f29","$f30","$f31"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  A type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
                          1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,
                          1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
                          1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

struct reg_handle empty_reg_handle={0};


/****************************************/
/*  Private data and functions.         */
/****************************************/

static long malign[MAX_TYPE+1]=  {1,1,2,4,8,8,4,8,8,1,8,1,1,1,4,1};
static long msizetab[MAX_TYPE+1]={1,1,2,4,8,8,4,8,8,0,8,0,0,0,4,0};

static struct Typ ltyp={LONG};

static char x_t[MAX_TYPE+1]={'?','b','w','l','q','q','s','t','t','?','q'};

static int sp=31;                  /*  Stackpointer                        */
static int gp=30;                  /*  Global pointer                      */
static int lr=27;                  /*  Link Register                       */
static int vp=28;                  /*  Virtual frame pointer               */
static int r31=32;                 /*  Read as zero                        */
static int f31=64;                 /*  Read as zero                        */
static int sd=14;                  /*  SmallDataPointer                    */
static int t1=2,t2=3,t3=4;         /*  Temporaries used by code generator  */
static int t4=5,t5=6;
static int f1=34,f2=35,f3=36;      /*  Temporaries used by code generator  */

#define DATA 0
#define BSS 1
#define CODE 2

static char *marray[]={"__ALPHA__",0};

static int section=-1,newobj,crsave,helpl,helps,stabs;
static char *codename="\t.text\n",*dataname="\t.data\n",*bssname="";
static int balign(struct obj *);
static char *labprefix="$C",*idprefix="";
static long framesize,frameoffset;
static void probj2(FILE *f,struct obj *p,int t);

#define ESGN 1
#define EUNS 2
static int st[MAXR+1];

static struct fpconstlist {
  struct fpconstlist *next;
  int label,typ;
  union atyps val;
} *firstfpc;

static void move_reg(FILE *f,int s,int t)
{
  if(t==r31||t==f31) ierror(0);
  if(s<=32&&t>32) ierror(0);
  if(t<=32&&s>32) ierror(0);
  if(s==t) return;
  if(s<=32){
    emit(f,"\tbis\t%s,%s,%s\n",regnames[r31],regnames[s],regnames[t]);
  }else{
    emit(f,"\tcpys\t%s,%s,%s\n",regnames[s],regnames[s],regnames[t]);
  }
  st[t]=st[s];
}

static int addfpconst(struct obj *o,int t)
{
  struct fpconstlist *p=firstfpc;
  t&=NQ;
  if(g_flags[4]&USEDFLAG){
    for(p=firstfpc;p;p=p->next){
      if(t==p->typ){
        eval_const(&p->val,t);
        if(t==INT&&zmeqto(vmax,zi2zm(o->val.vint))) return p->label;
        if(t==LONG&&zmeqto(vmax,zl2zm(o->val.vlong))) return p->label;
        if(t==LLONG&&zmeqto(vmax,zll2zm(o->val.vllong))) return p->label;
        if(t==FLOAT&&zldeqto(vldouble,zf2zld(o->val.vfloat))) return p->label;
        if(t==DOUBLE&&zldeqto(vldouble,zd2zld(o->val.vdouble))) return p->label;
        if(t==LDOUBLE&&zldeqto(vldouble,o->val.vldouble)) return p->label;
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

#define REG_IND 1
#define IMM_IND 2

static struct obj *cam(int flags,int base,int align,long offset)
/*  Initializes an addressing-mode structure and returns a pointer to   */
/*  that object. Will not survive a second call!                        */
{
  static struct obj obj;
  static struct AddressingMode am;
  obj.am=&am;
  am.flags=flags;
  am.base=base;
  am.offset=offset;
  am.align=align;
  return &obj;
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

static char *dct[]={"??","byte","short","long","quad","long","long","quad","??","??","??","??"};

static void load_address(FILE *f,int r,struct obj *o,int typ)
/*  Generates code to load the address of a variable into register r.   */
{
  if((o->flags&VAR)&&(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)){
    long of=zm2l(o->v->offset);
    if(of>=0)
      of+=frameoffset+zm2l(o->val.vlong);
    else
      of=-of-zm2l(maxalign)+framesize+zm2l(o->val.vmax);
    if(of>32767) ierror(0);
    emit(f,"\tlda\t%s,%ld(%s)\n",regnames[r],of,regnames[sp]);
  }else{
    emit(f,"\tlda\t%s,",regnames[r]);
    probj2(f,o,typ);emit(f,"\n");
  }
  st[r]=ESGN;
}
static void load_obj(FILE *f,int r,struct obj *o,int typ,int tmp)
{
  int adr,shift,al;
  switch(typ&NQ){
  case POINTER:
  case LDOUBLE:
  case DOUBLE:
  case FLOAT:
  case LLONG:
  case LONG:
  case INT:
    emit(f,"\tld%c\t%s,",x_t[typ&NQ],regnames[r]);
    probj2(f,o,typ);emit(f,"\n");
    st[r]=ESGN;return;
  case SHORT:
    al=balign(o);
    if(al>=0){
      emit(f,"\tldq_u\t%s,",regnames[r]);
      probj2(f,o,typ);emit(f,"\n");
      shift=64-16-al*8;
      if(shift) emit(f,"\tsll\t%s,%d,%s\n",regnames[r],shift,regnames[r]);
      if(typ&UNSIGNED){
        emit(f,"\tsrl\t%s,%d,%s\n",regnames[r],64-16,regnames[r]);
        st[r]=EUNS;
      }else{
        emit(f,"\tsra\t%s,%d,%s\n",regnames[r],64-16,regnames[r]);
        st[r]=ESGN;
      }
      return;
    }
    adr=0;
    if(o->am&&o->am->offset==0)
      adr=o->am->base;
    else if(zmeqto(o->val.vmax,l2zm(0L)))
      adr=o->reg;
    if(!adr){
      adr=tmp;load_address(f,adr,o,POINTER);
    }

    if(typ&UNSIGNED){
      emit(f,"\tbic\t%s,6,%s\n",regnames[adr],regnames[t4]); /* ldq_u ? */
      emit(f,"\tldq\t%s,0(%s)\n",regnames[t5],regnames[t4]);
      emit(f,"\tbic\t%s,1,%s\n",regnames[adr],regnames[t4]);
      emit(f,"\textwl\t%s,%s,%s\n",regnames[t5],regnames[t4],regnames[r]);
      /*
      emit(f,"\tbic\t%s,2,%s\n",regnames[adr],regnames[t4]);
      emit(f,"\tand\t%s,2,%s\n",regnames[adr],regnames[t5]);
      emit(f,"\tldq\t%s,0(%s)\n",regnames[r],regnames[t4]);
      emit(f,"\textwh\t%s,%s,%s\n",regnames[r],regnames[t5],regnames[r]);
      */
      st[r]=EUNS;return;
    }else{
      emit(f,"\tbic\t%s,6,%s\n",regnames[adr],regnames[t4]);
      emit(f,"\tand\t%s,6,%s\n",regnames[adr],regnames[t5]);
      emit(f,"\tldq\t%s,0(%s)\n",regnames[r],regnames[t4]);
      emit(f,"\tlda\t%s,2(%s)\n",regnames[t5],regnames[t5]);
      emit(f,"\textqh\t%s,%s,%s\n",regnames[r],regnames[t5],regnames[r]);
      emit(f,"\tsra\t%s,48,%s\n",regnames[r],regnames[r]);
      st[r]=ESGN;return;
    }
  case CHAR:
    al=balign(o);
    if(al>=0){
      emit(f,"\tldq_u\t%s,",regnames[r]);
      probj2(f,o,typ);emit(f,"\n");
      shift=64-8-al*8;
      if(shift&&al) emit(f,"\tsll\t%s,%d,%s\n",regnames[r],shift,regnames[r]);
      if(typ&UNSIGNED){
        if(al==0)
          emit(f,"\tand\t%s,255,%s\n",regnames[r],regnames[r]);
        else
          emit(f,"\tsrl\t%s,%d,%s\n",regnames[r],64-8,regnames[r]);
        st[r]=EUNS;
      }else{
        if(al==0)
          emit(f,"\tsextb\t%s,%s\n",regnames[r],regnames[r]);
        else
          emit(f,"\tsra\t%s,%d,%s\n",regnames[r],64-8,regnames[r]);
        st[r]=ESGN;
      }
      return;
    }
    if(typ&UNSIGNED){
      adr=0;
      if(o->am&&o->am->offset==0)
        adr=o->am->base;
      else if(zmeqto(o->val.vmax,l2zm(0L)))
        adr=o->reg;
      if(!adr){
        adr=tmp; load_address(f,adr,o,POINTER);
      }
      emit(f,"\tldq_u\t%s,0(%s)\n",regnames[r],regnames[adr]);
      emit(f,"\textbl\t%s,%s,%s\n",regnames[r],regnames[adr],regnames[r]);
      st[r]=EUNS;return;
    }else{
      emit(f,"\tldq_u\t%s,",regnames[t5]);
      probj2(f,o,typ);emit(f,"\n\tlda\t%s,",regnames[t4]);
      if(o->am) o->am->offset++; else o->val.vmax=zmadd(o->val.vmax,l2zm(1L));
      probj2(f,o,typ);
      if(o->am) o->am->offset--; else o->val.vmax=zmsub(o->val.vmax,l2zm(1L));
      emit(f,"\n\textqh\t%s,%s,%s\n",regnames[t5],regnames[t4],regnames[t5]);
      emit(f,"\tsra\t%s,56,%s\n",regnames[t5],regnames[r]);
      st[r]=ESGN;return;
    }
  }
  ierror(0);
}
static void load_reg(FILE *f,int r,struct obj *o,int typ,int tmp)
/*  Generates code to load a memory object into register r. tmp is a    */
/*  general purpose register which may be used. tmp can be r.           */
{
  typ&=NU;
  if(o->am){
    load_obj(f,r,o,typ,tmp);
    return;
  }
  if(o->flags&KONST){
    long l;int lab;
    eval_const(&o->val,typ);
    if(ISFLOAT(typ)){
      lab=addfpconst(o,typ);
      emit(f,"\tlda\t%s,%s%d\n",regnames[tmp],labprefix,lab);
      emit(f,"\tld%c\t%s,0(%s)\n",x_t[typ&NQ],regnames[r],regnames[tmp]);
      st[r]=ESGN;
      return;
    }
    if(zmleq(vmax,l2zm(32767))&&zmleq(l2zm(-32768),vmax)){
      emit(f,"\tlda\t%s,%ld(%s)\n",regnames[r],zm2l(vmax),regnames[r31]);
    }else{
/*       if((typ&NQ)<INT) ierror(0); */
      lab=addfpconst(o,typ);
      emit(f,"\tlda\t%s,%s%d\n",regnames[tmp],labprefix,lab);
      emit(f,"\tldq\t%s,0(%s)\n",regnames[r],regnames[tmp]);
    }
    st[r]=ESGN;
    return;
  }
  if((o->flags&VAR)&&(o->v->storage_class==EXTERN||o->v->storage_class==STATIC)){
    if(!(o->flags&VARADR)){
      load_address(f,tmp,o,POINTER);
      load_obj(f,r,cam(IMM_IND,tmp,balign(o),0L),typ,tmp);
    }else{
      load_address(f,r,o,POINTER);
    }
  }else{
    if(o->am||(o->flags&(DREFOBJ|REG))==REG){
      if(r!=o->reg)
        move_reg(f,o->reg,r);
    }else{
      if(o->flags&DREFOBJ) ierror(0);
      load_obj(f,r,o,typ,tmp);
    }
  }
}


static void store_reg(FILE *f,int r,struct obj *o,int typ)
/*  Generates code to store register r into memory object o.            */
{
  int adr,t6,t7;
  if((o->flags&(REG|DREFOBJ))==REG) ierror(0);
  if(r==t1){ t6=t2;t7=t3;}
  else if(r==t2) {t6=t1;t7=t3;}
  else {t6=t1;t7=t2;}
  if((typ&NQ)>SHORT){
    if((o->flags&VAR)&&(o->v->storage_class==EXTERN||o->v->storage_class==STATIC)){
      load_address(f,t6,o,POINTER);
      emit(f,"\tst%c\t%s,0(%s)\n",x_t[typ&NQ],regnames[r],regnames[t6]);
      return;
    }
    emit(f,"\tst%c\t%s,",x_t[typ&NQ],regnames[r]);
    probj2(f,o,typ);emit(f,"\n");
    return;
  }else{
    adr=0;
    if(o->am&&o->am->offset==0){
      adr=o->am->base;
      if(adr<1||adr>32) ierror(0);
    }else if((o->flags&REG)&&zmeqto(o->val.vmax,l2zm(0L))){
      adr=o->reg;
      if(adr<1||adr>32) ierror(0);
    }
    if(!adr){
      adr=t7; load_address(f,adr,o,POINTER);
    }
    if(adr<1||adr>32) ierror(0);
    if((typ&NQ)==SHORT){
      emit(f,"\tbic\t%s,2,%s\n",regnames[adr],regnames[t4]);
      emit(f,"\tldl\t%s,0(%s)\n",regnames[t6],regnames[t4]);
      emit(f,"\tand\t%s,2,%s\n",regnames[adr],regnames[t5]);
      emit(f,"\tmskwl\t%s,%s,%s\n",regnames[t6],regnames[t5],regnames[t6]);
      emit(f,"\tinswl\t%s,%s,%s\n",regnames[r],regnames[t5],regnames[t7]);
      emit(f,"\tbis\t%s,%s,%s\n",regnames[t6],regnames[t7],regnames[t6]);
      emit(f,"\tstl\t%s,0(%s)\n",regnames[t6],regnames[t4]);
      return;
    }else{
      emit(f,"\tldq_u\t%s,0(%s)\n",regnames[t4],regnames[adr]);
      emit(f,"\tmskbl\t%s,%s,%s\n",regnames[t4],regnames[adr],regnames[t4]);
      emit(f,"\tinsbl\t%s,%s,%s\n",regnames[r],regnames[adr],regnames[t5]);
      emit(f,"\tbis\t%s,%s,%s\n",regnames[t4],regnames[t5],regnames[t4]);
      emit(f,"\tstq_u\t%s,0(%s)\n",regnames[t4],regnames[adr]);
      return;
    }
  }
}

static void extend(FILE *f,int r)
{
  if(!r) return;
  if(st[r]==ESGN) return;
  emit(f,"\taddl\t%s,%s,%s\n",regnames[r],regnames[r31],regnames[r]);
  st[r]=ESGN;
}
static void uextend(FILE *f,int r)
{
  if(!r) return;
  if(st[r]==EUNS) return;
  emit(f,"\tzapnot\t%s,15,%s\n",regnames[r],regnames[r]);
  st[r]=EUNS;
}

static struct IC *do_refs(FILE *,struct IC *);
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long,long);
static void function_bottom(FILE *f,struct Var *,long,long);

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)

static int q1reg,q2reg,zreg;

static char *ccs[]={"eq","ne","lt","ge","le","gt",""};
static char *logicals[]={"bis","xor","and"};
static char *arithmetics[]={"sll","srl","add","sub","mul","div","rem"};

static struct IC *do_refs(FILE *f,struct IC *p)
/*  Does some pre-processing like fetching operands from memory to      */
/*  registers etc.                                                      */
{
  int typ=p->typf,typ1,reg,c=p->code;

  typ=q1typ(p);
  if(ISPOINTER(typ)) typ=(UNSIGNED|LONG);

  q1reg=q2reg=zreg=0;
  if(p->q1.flags&REG) q1reg=p->q1.reg;
  if(p->q2.flags&REG) q2reg=p->q2.reg;
  if((p->z.flags&(REG|DREFOBJ))==REG) zreg=p->z.reg;


  if((p->q1.flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->q1.val,typ);
    if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))){
      if(ISFLOAT(typ)) q1reg=f31; else q1reg=r31;
    }else{
      if(ISFLOAT(typ)) reg=f1; else reg=t1;
      if(c==ASSIGN&&zreg) reg=zreg;
      if(c==SETRETURN&&p->z.reg) reg=p->z.reg;
      if((c==MOD||c==DIV)&&ISINT(typ)&&!regs[25]) reg=25; /* Linux-div */
      if(ISFLOAT(typ)||c==DIV||c==SUB||c==ASSIGN||c==PUSH||c==SETRETURN||c==LSHIFT||c==RSHIFT||c==COMPARE){
        load_reg(f,reg,&p->q1,typ,t1);
        q1reg=reg;
      }else{
        if(ISINT(typ)){
          if(!zumleq(vumax,ul2zum(255UL))||!zmleq(vmax,l2zm(255L))||zmleq(vmax,l2zm(-1L))){
            load_reg(f,reg,&p->q2,typ,t1);
            q1reg=reg;
          }
        }
      }
    }
  }else if(c!=ADDRESS){
    if(p->q1.flags&&!q1reg){
      if(ISFLOAT(typ)) reg=f1; else reg=t1;
      if((c==ASSIGN||c==CONVERT)&&zreg&&(reg-r31)*(zreg-r31)>0) reg=zreg;
      if(c==SETRETURN&&p->z.reg) reg=p->z.reg;
      if((p->q1.flags&DREFOBJ)||c==ADDI2P||c==SUBIFP) {typ1=POINTER;reg=t1;} else typ1=typ;
      if(c==CALL) reg=vp;
      if((c==MOD||c==DIV)&&ISINT(typ1)&&!regs[25]) reg=25; /* Linux-div */
      if(ISSCALAR(typ1)){
        int m=p->q1.flags;
        p->q1.flags&=~DREFOBJ;
        load_reg(f,reg,&p->q1,typ1,t1);
        p->q1.flags=m;
        q1reg=reg;
      }
    }
    if((p->q1.flags&DREFOBJ)&&ISSCALAR(typ)){
      if(ISFLOAT(typ)) reg=f1; else reg=t1;
      if((c==ASSIGN||c==CONVERT)&&zreg&&regok(zreg,typ,0)) reg=zreg;
      if(c==SETRETURN&&p->z.reg) reg=p->z.reg;
      if((c==MOD||c==DIV)&&(typ&NQ)<=LONG&&!regs[25]) reg=25; /* Linux-div */
      if(c==ADDI2P||c==SUBIFP)
        load_reg(f,reg,cam(IMM_IND,q1reg,-1,0),POINTER,t1);
      else
        load_reg(f,reg,cam(IMM_IND,q1reg,-1,0),typ,t1);
      q1reg=reg;
    }
  }
  typ=q2typ(p);
  if((p->q2.flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->q2.val,typ);
    if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))){
      if(ISFLOAT(typ)) q2reg=f31; else q2reg=r31;
    }else{
      if(ISFLOAT(typ)) reg=f2; else reg=t2;
      if((c==MOD||c==DIV)&&ISINT(typ)&&!regs[26]) reg=26; /* Linux-div */
      if(ISFLOAT(typ)){
        load_reg(f,reg,&p->q2,typ,t2);
        q2reg=reg;
      }else{
        if(ISINT(typ)){
          if(!zumleq(vumax,ul2zum(255UL))||!zmleq(vmax,l2zm(255L))||zmleq(vmax,l2zm(-1L))){
            load_reg(f,reg,&p->q2,typ,t2);
            q2reg=reg;
          }
        }
      }
    }
  }else{
    if(p->q2.flags&&!q2reg){
      if(p->q2.flags&DREFOBJ) typ1=POINTER; else typ1=typ;
      if(ISFLOAT(typ1)) reg=f2; else reg=t2;
      if((c==MOD||c==DIV)&&ISINT(typ1)&&!regs[26]) reg=26; /* Linux-div */
      if(ISSCALAR(typ1)){
        int m=p->q2.flags;
        p->q2.flags&=~DREFOBJ;
        load_reg(f,reg,&p->q2,typ1,t2);
        p->q2.flags=m;
        q2reg=reg;
      }
    }
    if((p->q2.flags&DREFOBJ)&&ISSCALAR(typ)){
      if(ISFLOAT(typ)) reg=f2; else reg=t2;
      if((c==MOD||c==DIV)&&ISINT(typ)&&!regs[26]) reg=26; /* Linux-div */
      load_reg(f,reg,cam(IMM_IND,q2reg,-1,0),typ,t2);
      q2reg=reg;
    }
  }
  if(p->z.flags&&!isreg(z)){
    typ=ztyp(p);
    if(ISFLOAT(typ)) zreg=f3; else zreg=t3;
    if((c==MOD||c==DIV)&&ISINT(typ)&&!regs[28]) zreg=28; /* Linux-div */
  }
  if(q1reg){ p->q1.flags=REG; p->q1.reg=q1reg;}
  if(q2reg){ p->q2.flags=REG; p->q2.reg=q2reg;}
  return p;
}
static long pof2(zumax x)
/*  Yields log2(x)+1 oder 0. */
{
    zumax p;int ln=1;
    p=ul2zum(1L);
    while(ln<=64&&zumleq(p,x)){
        if(zumeqto(x,p)) return ln;
        ln++;p=zumadd(p,p);
    }
    return 0;
}
static void pr(FILE *f,struct IC *p)
/*  Writes the destination register to the real destination if necessary.   */
{
  int typ=p->typf;
  if(p->z.flags){
    if(p->code==ADDRESS||p->code==ADDI2P||p->code==SUBIFP) typ=POINTER;
    if(!isreg(z)){
      if(p->z.flags&DREFOBJ){
        if(p->z.flags&REG){
          store_reg(f,zreg,cam(IMM_IND,p->z.reg,-1,0),typ);
        }else{
          int r,m;
          if(t1==zreg) r=t2; else r=t1;
          m=p->z.flags;
          p->z.flags&=~DREFOBJ;
          load_reg(f,r,&p->z,POINTER,r);
          p->z.flags=m;
          store_reg(f,zreg,cam(IMM_IND,r,balign(&p->z),0),typ);
        }
      }else{
        store_reg(f,zreg,&p->z,typ);
      }
    }else{
      if(p->z.reg!=zreg)
        move_reg(f,zreg,p->z.reg);
    }
  }
}

static void probj2(FILE *f,struct obj *p,int t)
/*  Prints an object.                               */
{
  if(p->am){
    if(p->am->flags==REG_IND) ierror(0);
    if(p->am->flags==IMM_IND) emit(f,"%ld(%s)",p->am->offset,regnames[p->am->base]);
    return;
  }
  if(p->flags&DREFOBJ) emit(f,"(");
  if(p->flags&VAR) {
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      if(p->flags&REG){
        emit(f,"%s",regnames[p->reg]);
      }else{
        long of=zm2l(p->v->offset);
        if(of>=0)
          of+=frameoffset+zm2l(p->val.vmax);
        else
          of=-of-zm2l(maxalign)+framesize+zm2l(p->val.vmax);
        if(of>32767) ierror(0);
        emit(f,"%ld(%s)",of,regnames[sp]);
      }
    }else{
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,MAXINT);emit(f,"+");}
      if(p->v->storage_class==STATIC){
        emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
        emit(f,"%s%s",idprefix,p->v->identifier);
      }
    }
  }
  if((p->flags&REG)&&!(p->flags&VAR)) emit(f,"%s",regnames[p->reg]);
  if(p->flags&KONST){
    emitval(f,&p->val,t&NU);
  }
  if(p->flags&DREFOBJ) emit(f,")");
}
static void dwarf2_print_frame_location(FILE *f,struct Var *v)
{
  struct obj o;
  o.flags=REG;
  /*FIXME: need correct register translation */
  o.reg=sp;
  o.val.vmax=l2zm(0L);
  o.v=0;
  dwarf2_print_location(f,&o);
}
static int dwarf2_regnumber(int r)
{
  /*FIXME: needs correct translation */
  return r-1;
}
static zmax dwarf2_fboffset(struct Var *v)
{
  long of;
  if(!v||(v->storage_class!=AUTO&&v->storage_class!=REGISTER)) ierror(0);
  of=zm2l(v->offset);
  if(of>=0)
    of+=frameoffset;
  else
    of=-of-zm2l(maxalign)+framesize;
  if(of>32767) ierror(0);
  return l2zm(of);
}
static void function_top(FILE *f,struct Var *v,long offset,long maxpushed)
/*  Generates function top.                             */
{
  int i;
   emit(f,"\t.set\tnoat\n");
  if(section!=CODE){emit(f,codename);if(f) section=CODE;}
  emit(f,"\t.align\t3\n");
  if(v->storage_class==EXTERN)
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
  emit(f,"\t.ent\t%s%s\n",idprefix,v->identifier);
  if(v->storage_class==STATIC)
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
  else
    emit(f,"%s%s:\n",idprefix,v->identifier);
  if(function_calls) emit(f,"\tldgp\t%s,0(%s)\n",regnames[gp],regnames[vp]);
  if(v->storage_class==STATIC)
    emit(f,"%s%ld..ng:\n",labprefix,zm2l(v->offset));
  else  
    emit(f,"%s%s..ng:\n",idprefix,v->identifier);
  framesize=offset+maxpushed;
  if(function_calls) framesize+=8; /* lr */
  for(i=1;i<=MAXR;i++)
    if(regused[i]&&!regscratch[i]&&!regsa[i])
      framesize+=8;
  framesize=((framesize+16-1)/16)*16;
  if(framesize>32767) ierror(0);
  if(framesize) emit(f,"\tlda\t%s,%ld(%s)\n",regnames[sp],-framesize,regnames[sp]);
  emit(f,"\t.frame\t%s,%ld,%s,0\n",regnames[sp],framesize,regnames[lr]);
  frameoffset=maxpushed;
  if(function_calls){
    emit(f,"\tstq\t%s,%ld(%s)\n",regnames[lr],frameoffset,regnames[sp]);
    frameoffset+=8;
  }
  for(i=1;i<=MAXR;i++){
    if(regused[i]&&!regscratch[i]&&!regsa[i]){
      if(i<=32){
        emit(f,"\tstq\t%s,%ld(%s)\n",regnames[i],frameoffset,regnames[sp]);
      }else{
        emit(f,"\tstt\t%s,%ld(%s)\n",regnames[i],frameoffset,regnames[sp]);
      }
      frameoffset+=8;
    }
  }
  emit(f,"\t.mask\t0x4000000,%ld\n",-framesize);
  emit(f,"\t.prologue\t%c\n",function_calls?'1':'0');
}
static void function_bottom(FILE *f,struct Var *v,long offset,long maxpushed)
/*  Generates function bottom.                          */
{
  int i;
  frameoffset=maxpushed;
  if(function_calls){
    emit(f,"\tldq\t%s,%ld(%s)\n",regnames[lr],frameoffset,regnames[sp]);
    frameoffset+=8;
  }
  for(i=1;i<=MAXR;i++){
    if(regused[i]&&!regscratch[i]&&!regsa[i]){
      if(i<=32){
        emit(f,"\tldq\t%s,%ld(%s)\n",regnames[i],frameoffset,regnames[sp]);
      }else{
        emit(f,"\tldt\t%s,%ld(%s)\n",regnames[i],frameoffset,regnames[sp]);

      }
      frameoffset+=8;
    }
  }
  if(framesize) emit(f,"\tlda\t%s,%ld(%s)\n",regnames[sp],framesize,regnames[sp]);
  emit(f,"\tret\t%s,(%s),1\n",regnames[r31],regnames[lr]);
  emit(f,"\t.end\t%s%s\n",idprefix,v->identifier);
  emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
  emit(f,"\t.size\t%s%s,$-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
}
static int balign(struct obj *o)
/*  Liefert die unteren 3 Bits des Objekts. -1 wenn unklar. */
{
  int sc;
  if(o->flags&DREFOBJ) return -1;
  if(o->am) return o->am->align;
  if(!(o->flags&VAR)) ierror(0);
  sc=o->v->storage_class;
  if(sc==EXTERN||sc==STATIC){
    /* Alle statischen Daten werden vom cg auf 64bit alignt. */
    return zm2l(zmand(o->val.vmax,l2zm(7L)));
  }
  if(sc==AUTO||sc==REGISTER){
    zmax of=o->v->offset;
    if(!zmleq(l2zm(0L),of))
      of=zmsub(l2zm(0L),zmadd(of,maxalign));
    return zm2l(zmand(zmadd(of,o->val.vmax),l2zm(7L)));
  }
  ierror(0);
}

/* Routinen fuer Debug-Informationen mit stabs. */

static int debug_blabel,debug_elabel;
static int debug_type(FILE *,struct Typ *);
static void debug_init(FILE *,struct Var *);
static void debug_exit(FILE *,struct Var *);
static void debug_var(FILE *,struct obj *);
static void debug_IC(FILE *,struct IC *);
static void debug_cleanup(FILE *);

static int debug_type(FILE *f,struct Typ *t)
     /* Liefert Typindex.  */
{
  return t->flags&NU;
}
static void debug_init(FILE *f,struct Var *v)
     /* Debug-Infos. Wird am Anfang von gen_code aufgerufen. */
{
  static int did_header;
  if(!did_header){
    emit(f,"\t.stabs \"\",100,0,0,%stext0\n",labprefix);
    emit(f,"\t.stabs \"%s\",100,0,0,%stext0\n",filename,labprefix);
    emit(f,"\t.text\n%stext0:\n",labprefix);if(f) section=CODE;
    emit(f,"\t.stabs\t\"char:t%d=r1;-128;127;\",128,0,0,0\n",CHAR);
    emit(f,"\t.stabs\t\"short:t%d=r1;-32768;32767;\",128,0,0,0\n",SHORT);
    emit(f,"\t.stabs\t\"int:t%d=r1;-2147483648;2147483647;\",128,0,0,0\n",INT);
    emit(f,"\t.stabs\t\"long:t%d=r1;001000000000000000000000;000777777777777777777777;\",128,0,0,0\n",LONG);
    emit(f,"\t.stabs\t\"unsigned char:t%d=r1;0;255;\",128,0,0,0\n",UNSIGNED|CHAR);
    emit(f,"\t.stabs\t\"unsigned short:t%d=r1;0;65535;\",128,0,0,0\n",UNSIGNED|SHORT);
    emit(f,"\t.stabs\t\"unsigned int:t%d=r1;0;-1;\",128,0,0,0\n",UNSIGNED|INT);
    emit(f,"\t.stabs\t\"unsigned long:t%d=r1;0;-1;\",128,0,0,0\n",UNSIGNED|LONG);
    emit(f,"\t.stabs\t\"float:t%d=r1;4;0;\",128,0,0,0\n",FLOAT);
    emit(f,"\t.stabs\t\"double:t%d=r1;8;0;\",128,0,0,0\n",DOUBLE);
    emit(f,"\t.stabs\t\"void:t%d=%d;8;0;\",128,0,0,0\n",VOID,VOID);
    did_header=1;
  }
  emit(f,"\t.stabs\t\"%s:F%d\",36,0,0,%s%s\n",v->identifier,debug_type(f,v->vtyp->next),idprefix,v->identifier);
  debug_blabel=++label;debug_elabel=++label;
}
static void debug_exit(FILE *f,struct Var *v)
     /* Debug-Infos. Wird am Ende von gen_code aufgerufen. */
{
  struct IC *p;
  ierror(0);
  emit(f,"\t.stabn\t192,0,0,%s%d\n",labprefix,debug_blabel);
  emit(f,"\t.stabn\t224,0,0,%s%d\n",labprefix,debug_elabel);
  for(p=first_ic;p;p=p->next){
    if(p->q1.flags&VAR) debug_var(f,&p->q1);
    if(p->q2.flags&VAR) debug_var(f,&p->q2);
    if(p->z.flags&VAR) debug_var(f,&p->z);
  }
}
static void debug_var(FILE *f,struct obj *o)
     /* Debug-Infos fuer eine Variable ausgeben. */
{
  struct Var *v=o->v; int td;
  if(!*v->identifier) return;
  td=debug_type(f,v->vtyp);
  if(ISFUNC(td)) return;
  if(o->flags&REG){
    emit(f,"\t.stabs\t\"%s:r%d\",0x40,0,0,%d\n",v->identifier,td,o->reg-1);
    return;
  }
  if(v->storage_class==AUTO||v->storage_class==REGISTER){
    long of=zm2l(v->offset);
    if(!zmleq(l2zm(0L),v->offset)){
      of=-of-zm2l(maxalign)+framesize;
      emit(f,"#\toffset %ld:\n",of);
      emit(f,"\t.stabs\t\"%s:p%d\",0x80,0,0,%ld\n",v->identifier,td,of-framesize);
    }else{
      of+=frameoffset;
      emit(f,"#\toffset %ld:\n",of);
      emit(f,"\t.stabs\t\"%s:%d\",0x80,0,0,%ld\n",v->identifier,td,of-framesize);
    }
    return;
  }
  ierror(td);
}
static void debug_IC(FILE *f,struct IC *p)
     /* Debug-Infos. Wird fuer jedes IC aufgerufen. */
{
  static int lastline;int lab;
  printf("%d",p->line);pric2(stdout,p);
  if(!p->prev)
    emit(f,"%s%d:\n",labprefix,debug_blabel);
/*   if(p->q1.flags&VAR) debug_var(f,&p->q1); */
/*   if(p->q2.flags&VAR) debug_var(f,&p->q2); */
/*   if(p->z.flags&VAR) debug_var(f,&p->z); */
  if(p->line&&p->line!=lastline){
    lab=++label;lastline=p->line;
    emit(f,"%s%d:\n",labprefix,lab);
    emit(f,"\t.stabn\t68,0,%d,%s%d\n",lastline,labprefix,lab);
  }
  if(!p->next) emit(f,"%s%d:\n",labprefix,debug_elabel);
}
static void debug_cleanup(FILE *f)
     /* Debug-Infos. Wird in cleanup_cg aufgerufen. */
{
}


/****************************************/
/*  End of private data and functions.  */
/****************************************/


int init_cg(void)
/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
{
  int i;
  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign=l2zm(8L);
  char_bit=l2zm(8L);
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i= 1;i<=32;i++) {regsize[i]=l2zm(8L);regtype[i]=&ltyp;}
  for(i=33;i<=64;i++) {regsize[i]=l2zm(8L);regtype[i]=&ltyp;}

  /*  Use multiple ccs.   */
  multiple_ccs=0; /* not yet */

  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-128L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[INT]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[LONG]=t_min[LLONG];
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(127L);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[INT]=ul2zum(2147483647UL);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[LONG]=t_max[LLONG];
  t_max[MAXINT]=t_max(LLONG);
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=ul2zum(4294967295UL);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[LONG]=tu_max[LLONG];
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

  /*  Reserve a few registers for use by the code-generator.      */
  /*  This is not optimal but simple.                             */
  regsa[t1]=regsa[t2]=regsa[t3]=regsa[t4]=regsa[t5]=1;
  regsa[f1]=regsa[f2]=regsa[f3]=1;
  regsa[sp]=regsa[gp]=regsa[sd]=1;
  regsa[lr]=regsa[r31]=regsa[f31]=1;
  regscratch[t1]=regscratch[t2]=regscratch[t3]=regscratch[t4]=regscratch[t5]=0;
  regscratch[f1]=regscratch[f2]=regscratch[f3]=0;
  regscratch[sp]=regscratch[gp]=regscratch[sd]=0;
  regscratch[lr]=regscratch[r31]=regscratch[f31]=0;
  /* reserve at - noch aendern */
  /* regsa[29]=1;regscratch[29]=0; */
  /* Debug stabs? */
  stabs=(g_flags[5]&USEDFLAG);
  target_macros=marray;
  return 1;
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
/*  A pointer MUST be returned in a register. The code-generator    */
/*  has to simulate a pseudo register if necessary.                 */
{
  if(ISFLOAT(t->flags)) return 33;
  if(ISSTRUCT(t->flags)||ISUNION(t->flags)) return 0;
  if(zmleq(szof(t),l2zm(8L)))
    return 1; 
  else
    return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  return 0;
}     

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if(r==0) return 0;
  if(t==0) return 0;
  if(ISFLOAT(t)){
    if(r>=33&&r<=64)
      return 1;
    else
      return 0;
  }
  if(ISSCALAR(t)&&r>=1&&r<=32) return 1;
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
  if((c==DIV||c==MOD)&&!(p->q2.flags&KONST))
    return 1;
  return 0;
}

int must_convert(int o,int t,int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
/*  On the PowerPC cpu pointers and 32bit               */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
{
  int op=o&NQ,tp=t&NQ;
  if((op==LONG||op==LLONG||op==POINTER)&&(tp==LONG||tp==LLONG||tp==POINTER)) return 0;
  if((op==DOUBLE||op==LDOUBLE)&&(tp==DOUBLE||tp==LDOUBLE)) return 0;
  return 1;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  title(f);
  if(newobj){
    size=zmmult(zmdiv(zmadd(size,l2zm(7L)),l2zm(8L)),l2zm(8L));
    emitzm(f,size);
  }else{
    emit(f,"\t.zero\t");
    emitzm(f,size);
  }
  emit(f,"\n");
  newobj=0;
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  title(f);
  if(zm2l(align)>1)
    emit(f,"\t.align\t3\n");
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
    if(ISFUNC(v->vtyp->flags)) return;
    if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
    if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
    if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
    emit(f,"\t.type\t%s%ld,@object\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,%ld\n",labprefix,zm2l(v->offset),zm2l(szof(v->vtyp)));
    if(section!=BSS)
      emit(f,"\t.align\t3\n%s%ld:\n",labprefix,zm2l(v->offset));
    else{
      emit(f,"\t.lcomm\t%s%ld,",labprefix,zm2l(v->offset));
      newobj=1;
    }
  }
  if(v->storage_class==EXTERN){
    emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    if(v->flags&(DEFINED|TENTATIVE)){
      if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
      if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
      if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
      emit(f,"\t.type\t%s%s,@object\n",idprefix,v->identifier);
      emit(f,"\t.size\t%s%s,%ld\n",idprefix,v->identifier,zm2l(szof(v->vtyp)));
      if(section!=BSS)
	emit(f,"\t.align\t3\n%s%s:\n",idprefix,v->identifier);
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
  if(ISPOINTER(t)) t=UNSIGNED|LONG;
  emit(f,"\t.%s\t",dct[t&NQ]);
  if(!p->tree){
    if(ISFLOAT(t)){
      /*  auch wieder nicht sehr schoen und IEEE noetig   */
      unsigned char *ip;
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[3],ip[2],ip[1],ip[0]);
      if((t&NQ)!=FLOAT){
        emit(f,",0x%02x%02x%02x%02x",ip[7],ip[6],ip[5],ip[4]);
      }
    }else{
      emitval(f,&p->val,t&NU);
    }
  }else{
    if(p->tree->o.am) ierror(9);
    probj2(f,&p->tree->o,t&NU);
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
/*  The main code-generation.                                           */
{
  int addbuf,c,t,cmpreg,cmpflag,wasnoreg,i,varargs=0,fixargs;struct IC *m;
  long pushed,maxpushed;
  if(DEBUG&1) printf("gen_code()\n");
  if(stabs) debug_init(f,v);
  for(c=1;c<=MAXR;c++){regs[c]=regsa[c];st[c]=ESGN;}
  /* We do a pass over the code to retrieve some info and prepare certain optimizations */
  addbuf=0;maxpushed=0;
  title(f);
  for(m=p;m;m=m->next){
    c=m->code;t=m->typf&NU;
    if(c==ALLOCREG) {regs[m->q1.reg]=1;continue;}
    if(c==FREEREG) {regs[m->q1.reg]=0;continue;}
    if(c==COMPARE&&(m->q2.flags&KONST)){
      eval_const(&m->q2.val,t);
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0))){
        m->q2.flags=0;m->code=c=TEST;
      }
    }
    if(ISINT(t)&&(m->q2.flags&KONST)&&(c==MULT||((c==DIV||c==MOD)&&(t&UNSIGNED)))){
      eval_const(&m->q2.val,t);
      i=pof2(vumax);
      if(i){
        if(c==MOD){
          vmax=zmsub(vmax,l2zm(1L));
          m->code=AND;
        }else{
          vmax=l2zm(i-1);
          if(c==DIV) m->code=RSHIFT; else m->code=LSHIFT;
        }
        c=m->code;
	gval.vmax=vmax;
	eval_const(&gval,MAXINT);
	if(c==AND){
	  insert_const(&m->q2.val,t&NU);
	}else{
	  insert_const(&m->q2.val,INT);
	  p->typf2=INT;
	}
      }
    }
    /* Need one stack slot for transfrring between integer and floating-point registers. */
    t&=NQ;
    if(c==CONVERT&&addbuf<8&&((ISFLOAT(t)&&!ISFLOAT(m->typf2))||(!ISFLOAT(t)&&ISFLOAT(p->typf2)))) addbuf=8;
    /* May need one stack slot for inline memcpy. */
/*     if((c==ASSIGN||c==PUSH)&&t>=POINTER&&addbuf<8) addbuf=8; */
    /* Need additional stack slots for passing function arguments. */
    if(c==CALL&&maxpushed<zm2l(m->q2.val.vmax)) maxpushed=zm2l(m->q2.val.vmax);
    if(c==CALL&&(m->q1.flags&VAR)&&!strcmp(m->q1.v->identifier,"__va_start")) varargs=1;
    /* Need up to 4 stack slots for calling div/mod-functions. */
    if((c==DIV||c==MOD)&&ISINT(t)&&addbuf<32){
      if(regs[25]||regs[26]||regs[28]||regs[29]) addbuf=32;
    }
  }
  /* Noch nicht ok. */
  if(varargs){
    fixargs=0;
    for(i=0;i<v->vtyp->exact->count;i++){
      c=(*v->vtyp->exact->sl)[i].styp->flags;
      if(ISPOINTER(c)) fixargs++;
    }
    if(fixargs<6) addbuf+=(6-fixargs)*16;
  }
  function_top(f,v,zm2l(offset+addbuf),maxpushed);
  if(varargs){
    for(i=fixargs+1;i<=6;i++){
      emit(f,"\tstq\t%s,%ld(%s)\n",regnames[16+i],framesize-(7-i)*16,regnames[sp]);
      emit(f,"\tstt\t%s,%ld(%s)\n",regnames[48+i],framesize-(7-i)*16+8,regnames[sp]);
    }
  }
  pushed=0;
  for(;p;pr(f,p),p=p->next){
    if(DEBUG) pric2(stdout,p);
    if(debug_info){
      if(stabs) 
	debug_IC(f,p);
      else
	dwarf2_line_info(f,p);
    }
    c=p->code;t=p->typf;
    if(c==NOP) continue;
    if(c==ALLOCREG) {regs[p->q1.reg]=1;continue;}
    if(c==FREEREG) {regs[p->q1.reg]=0;continue;}
    if(c==LABEL||(c>=BEQ&&c<=BRA)){
      int i;
      for(i=1;i<=32;i++)
        if(regs[i]&&!regsa[i]) extend(f,i);
    }
    if(c==LABEL) {emit(f,"%s%d:\n",labprefix,t);continue;}
    if(c==BRA) {emit(f,"\tbr\t%s%d\n",labprefix,t);continue;}
    if(c>=BEQ&&c<BRA){
      if(cmpflag){
        emit(f,"\t%sb%s\t%s,%s%d\n",cmpreg<=32?"":"f",cmpflag<0?"eq":"ne",regnames[cmpreg],labprefix,t);
      }else{
        emit(f,"\t%sb%s\t%s,%s%d\n",cmpreg<=32?"":"f",ccs[c-BEQ],regnames[cmpreg],labprefix,t);
      }
      continue;
    }
    if(c==MOVETOREG){
      load_reg(f,p->z.reg,&p->q1,p->z.reg<=32?LONG:DOUBLE,0);
      p->z.flags=0;
      continue;
    }
    if(c==MOVEFROMREG){
      store_reg(f,p->q1.reg,&p->z,p->q1.reg<=32?LONG:DOUBLE);
      p->z.flags=0;
      continue;
    }
    if(ISPOINTER(t)) t=((t&~NU)|UNSIGNED|LONG);
    if((c==ASSIGN||c==PUSH)&&(!ISSCALAR(t)||((t&NQ)==CHAR&&zm2l(p->q2.val.vmax)!=1))){
      zmax size; struct obj loops;
      int salign,dalign,cl,a1,a2,mr,ms;
      size=p->q2.val.vmax;
      salign=balign(&p->q1);
      if(c==PUSH) dalign=0; else dalign=balign(&p->z);
      if(salign>=0&&dalign>=0&&(salign&3)==(dalign&3)){
        a1=t1; a2=t2;
      }else{
        if(!helpl) helpl=++label;
        cl=0;mr=0;ms=0;
        for(i=1;i<=32;i++){
          if(i!=17&&i!=18&&i!=19&&regused[i]&&!regscratch[i]&&!regs[i]) mr=i;
          if(regs[i]&&regscratch[i]) ms=1;
        }
        if(mr==0) mr=t3;
        if(regs[17]||regs[18]||regs[19]||function_calls==0) ms=1;
        if(ms) emit(f,"\tlda\t%s,%s%d\n",regnames[mr],labprefix,helpl);
        if(regs[17]) emit(f,"\tstq\t%s,%d(%s)\n",regnames[17],8*cl++,regnames[mr]);
        if(regs[18]) emit(f,"\tstq\t%s,%d(%s)\n",regnames[18],8*cl++,regnames[mr]);
        if(regs[19]) emit(f,"\tstq\t%s,%d(%s)\n",regnames[19],8*cl++,regnames[mr]);
        a1=18;a2=17;
        if(p->z.am&&p->z.am->base==18) ierror(0);
        if(!p->z.am&&(p->z.flags&REG)&&p->z.reg==18) ierror(0);
      }
      if(p->q1.flags&DREFOBJ){
        p->q1.flags&=~DREFOBJ;
        load_reg(f,a1,&p->q1,POINTER,a1);
        p->q1.flags|=DREFOBJ;
      }else{
        load_address(f,a1,&p->q1,POINTER);
      }
      if(p->z.flags&DREFOBJ){
        p->z.flags&=~DREFOBJ;
        load_reg(f,a2,&p->z,POINTER,a2);
        p->z.flags|=DREFOBJ;
      }else{
        if(c==PUSH){
          emit(f,"\tlda\t%s,%ld(%s)\n",regnames[a2],pushed,regnames[sp]);
          pushed+=zm2l(p->q2.val.vmax);
        }else{
          load_address(f,a2,&p->z,POINTER);
        }
      }
      if(salign>=0&&dalign>=0&&(salign&3)==(dalign&3)){
        int do64,m,s;
        if((salign&7)==(dalign&7))
          {do64=1;m=8;s='q';}
        else
          {do64=0;m=4;s='l';salign&=3;}
        if(salign&7){
          emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t3],-salign,regnames[t1]);
          emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t4],-salign,regnames[t2]);
          cl=(1<<salign)-1;
          if(!zmleq(l2zm(m-salign),size)) cl+=((1<<(m-salign-zm2l(size)))-1)<<(salign+zm2l(size));
          emit(f,"\tzap\t%s,%d,%s\n",regnames[t3],cl,regnames[t3]);
          emit(f,"\tzapnot\t%s,%d,%s\n",regnames[t4],cl,regnames[t4]);
          emit(f,"\tbis\t%s,%s,%s\n",regnames[t3],regnames[t4],regnames[t3]);
          emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t3],-salign,regnames[t2]);
          size=zmsub(size,zm2l(m-salign));
          salign=m-salign;
        }else
          salign=0;
        loops.val.vmax=zmdiv(size,l2zm(8*m));
        if(zmleq(l2zm(2L),loops.val.vmax)){
          loops.flags=KONST;
          loops.am=0;
          load_reg(f,t3,&loops,LONG,t3);
          cl=++label;
          emit(f,"\t.align\t4\n%s%d:\n",labprefix,cl);
        }
        if(!zmeqto(loops.val.vmax,l2zm(0))){
          for(i=0;i<8;i+=2){
            emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t1]);
            emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t5],salign+i*m+m,regnames[t1]);
            emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t2]);
            emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t5],salign+i*m+m,regnames[t2]);
          }
        }
        if(zmleq(l2zm(2L),loops.val.vmax)){
          emit(f,"\taddq\t%s,%d,%s\n",regnames[t1],8*m,regnames[t1]);
          emit(f,"\taddq\t%s,%d,%s\n",regnames[t2],8*m,regnames[t2]);
          emit(f,"\tsubq\t%s,1,%s\n",regnames[t3],regnames[t3]);
          emit(f,"\tbne\t%s,%s%d\n",regnames[t3],labprefix,cl);
        }else{
          if(!zmeqto(loops.val.vmax,l2zm(0L)))
            salign+=8*m;
        }
        size=zmand(size,l2zm(8*m-1));
        for(i=0;i<(zm2l(size)/m/2)*2;i+=2){
          emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t1]);
          emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t5],salign+i*m+m,regnames[t1]);
          emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t2]);
          emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t5],salign+i*m+m,regnames[t2]);
        }
        size=zmand(size,l2zm(2*m-1));
        if(zm2l(size)>=m){
          emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t1]);
          emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t2]);
          size=zmsub(size,l2zm(m));i++;
        }
        if(zm2l(size)>0){
          if(zm2l(size)==4){
            emit(f,"\tldl\t%s,%d(%s)\n",regnames[t4],salign+i*m,regnames[t1]);
            emit(f,"\tstl\t%s,%d(%s)\n",regnames[t4],salign+i*m,regnames[t2]);
          }else{
            emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t1]);
            emit(f,"\tld%c\t%s,%d(%s)\n",s,regnames[t5],salign+i*m,regnames[t2]);
            cl=(1<<zm2l(size))-1;
            emit(f,"\tzapnot\t%s,%d,%s\n",regnames[t4],cl,regnames[t4]);
            emit(f,"\tzap\t%s,%d,%s\n",regnames[t5],cl,regnames[t5]);
            emit(f,"\tbis\t%s,%s,%s\n",regnames[t4],regnames[t5],regnames[t4]);
            emit(f,"\tst%c\t%s,%d(%s)\n",s,regnames[t4],salign+i*m,regnames[t2]);
          }
        }
        p->z.flags=0; /* to prevent pr() from... */
        continue;
      }
      for(i=1;i<=32;i++){
        if(i!=17&&i!=18&&i!=19&&regs[i]&&regscratch[i])
          emit(f,"\tstq\t%s,%d(%s)\n",regnames[i],8*cl++,regnames[mr]);
      }
      if(function_calls==0) emit(f,"\tstq\t%s,%d(%s)\n",regnames[lr],8*cl++,regnames[mr]);
      if(cl>helps) helps=cl;
      loops.val.vmax=size;
      loops.flags=KONST;
      loops.am=0;
      load_reg(f,19,&loops,LONG,19);
      emit(f,"\t.global\t%smemcpy\n",idprefix);
      emit(f,"\tjsr\t%s,%smemcpy\n",regnames[lr],idprefix);
      emit(f,"\tldgp\t%s,0(%s)\n",regnames[gp],regnames[lr]);
      cl=0;
      if(ms&&mr==t3) emit(f,"\tlda\t%s,%s%d\n",regnames[mr],labprefix,helpl);
      if(regs[17]) emit(f,"\tldq\t%s,%d(%s)\n",regnames[17],8*cl++,regnames[mr]);
      if(regs[18]) emit(f,"\tldq\t%s,%d(%s)\n",regnames[18],8*cl++,regnames[mr]);
      if(regs[19]) emit(f,"\tldq\t%s,%d(%s)\n",regnames[19],8*cl++,regnames[mr]);
      for(i=1;i<=32;i++){
        if(i!=17&&i!=18&&i!=19&&regs[i]&&regscratch[i])
          emit(f,"\tldq\t%s,%d(%s)\n",regnames[i],8*cl++,regnames[mr]);
      }
      if(function_calls==0) emit(f,"\tldq\t%s,%d(%s)\n",regnames[lr],8*cl++,regnames[mr]);
      p->z.flags=0;
      continue;
    }
    if(isreg(q1)) wasnoreg=1; else wasnoreg=0;
    p=do_refs(f,p);
    c=p->code;
    if(c==CONVERT){
      int to;
      to=p->typf2&NU;
      if(ISPOINTER(to)) to=(UNSIGNED|LONG);
      if(ISINT(to)&&ISINT(t)){
        if((to&NQ)>=(t&NQ)){
          if((t&NQ)<INT){
            if(t&UNSIGNED){
              emit(f,"\tzapnot\t%s,%d,%s\n",regnames[q1reg],(t&NQ)==CHAR?1:3,regnames[zreg]);
              st[zreg]=EUNS;continue;
            }else{
              emit(f,"\tsext%c\t%s,%s\n",(t&NQ)==CHAR?'b':'w',regnames[q1reg],regnames[zreg]);
              st[zreg]=ESGN;continue;
            }
          }
          zreg=q1reg;
        }else if((t&NQ)==LONG||(t&NQ)==LLONG){
          if(to&UNSIGNED)
            uextend(f,q1reg);
          else
            extend(f,q1reg);
          st[q1reg]=ESGN;
          zreg=q1reg;
        }else{
          if((to&UNSIGNED)==(t&UNSIGNED)){
            zreg=q1reg;
          }else{
            if(to&UNSIGNED){
              emit(f,"\tzapnot\t%s,%d,%s\n",regnames[q1reg],(t&NQ)==CHAR?1:3,regnames[zreg]);
              st[zreg]=EUNS;continue;
            }else{
              emit(f,"\tsext%c\t%s,%s\n",(t&NQ)==CHAR?'b':'w',regnames[q1reg],regnames[zreg]);
              st[zreg]=ESGN;continue;
            }
          }
        }
        continue;
      }
      if(ISFLOAT(to)){
        st[zreg]=ESGN;
        if(ISFLOAT(t)){
          emit(f,"\tcvt%c%c\t%s,%s\n",x_t[to&NQ],x_t[t&NQ],regnames[q1reg],regnames[zreg]);
          continue;
        }
/*      if(t&UNSIGNED) ierror(0); */
        emit(f,"\tcvttqc\t%s,%s\n",regnames[q1reg],regnames[f3]);
/*      emit(f,"\tftoit\t%s,%s\n",regnames[q1reg],regnames[zreg]);  */
/*      emit(f,"\t.long\t%ld\n",(0x1cl<<26)+(0x70l<<5)+(31l<<16)+((long)(q1reg-33)<<21)+zreg-1); */
        emit(f,"\tstt\t%s,%ld(%s)\n",regnames[f3],framesize-addbuf,regnames[sp]);
        emit(f,"\tldq\t%s,%ld(%s)\n",regnames[zreg],framesize-addbuf,regnames[sp]);
        st[zreg]=ESGN;
        continue;
      }
      if(ISFLOAT(t)){
/*      if(to&UNSIGNED) ierror(0); */
/*      emit(f,"\titoft\t%s,%s\n",regnames[q1reg],regnames[zreg]);  */
        emit(f,"\tstq\t%s,%ld(%s)\n",regnames[q1reg],framesize-addbuf,regnames[sp]);
        emit(f,"\tldt\t%s,%ld(%s)\n",regnames[zreg],framesize-addbuf,regnames[sp]);
        emit(f,"\tcvtq%c\t%s,%s\n",x_t[t&15],regnames[zreg],regnames[zreg]);

        continue;
      }
    }
    if(c==KOMPLEMENT){
      emit(f,"\tornot\t%s,%s,%s\n",regnames[r31],regnames[q1reg],regnames[zreg]);
      if((t&NQ)==INT) st[zreg]=0; else st[zreg]=ESGN;
      continue;
    }
    if(c==SETRETURN){
      if(p->z.reg){
        if(zreg==0) load_reg(f,p->z.reg,&p->q1,t,t3);
        extend(f,p->z.reg);
      }else
        ierror(0);
      continue;
    }
    if(c==GETRETURN){
      if(p->q1.reg){
        zreg=p->q1.reg;
        st[zreg]=ESGN;
      }else
        p->z.flags=0;
      continue;
    }
    if(c==CALL){
      int reg;
      if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
        emit_inline_asm(f,p->q1.v->fi->inline_asm);
        pushed-=zm2l(p->q2.val.vmax);
        continue;
      }
      if((p->q1.flags&VAR)&&p->q1.v->storage_class==EXTERN&&!(g_flags[6]&USEDFLAG)){
        char *s=p->q1.v->identifier;
        if(!strcmp("abs",s)||!strcmp("labs",s)){
          emit(f,"\tsubq\t%s,%s,%s\n",regnames[r31],regnames[17],regnames[1]);
          emit(f,"\tcmovge\t%s,%s,%s\n",regnames[17],regnames[17],regnames[1]);
          continue;
        }
        if(!strcmp("fabs",s)){
          emit(f,"\tfabs\t%s,%s\n",regnames[17],regnames[1]);
          continue;
        }
        if(!strcmp("__va_fixargs",s)){
          emit(f,"\tlda\t%s,%d(%s)\n",regnames[1],fixargs,regnames[r31]);
          continue;
        }
        if(!strcmp("__va_start",s)){
          emit(f,"\tlda\t%s,%ld(%s)\n",regnames[1],framesize-(6-fixargs)*16,regnames[sp]);
          continue;
        }
      }
      for(reg=17;reg<=22;reg++)
        extend(f,reg);
      if(q1reg){
        if(q1reg!=vp) move_reg(f,q1reg,vp);
        emit(f,"\tjsr\t%s,(%s),0\n",regnames[lr],regnames[vp]);
      }else{
        emit(f,"\tjsr\t%s,",regnames[lr]);
        probj2(f,&p->q1,t);emit(f,"\n");
      }
      emit(f,"\tldgp\t%s,0(%s)\n",regnames[gp],regnames[lr]);
      pushed-=zm2l(p->q2.val.vmax);
      continue;
    }
    if(c==ASSIGN||c==PUSH){
      if(t==0) ierror(0);
      if(q1reg){
        if(c==PUSH){
          extend(f,q1reg);
          if((t&NQ)==FLOAT){
            emit(f,"\tcvtst\t%s,%s\n",regnames[q1reg],regnames[f1]);
            q1reg=f1;
	    emit(f,"\tsts\t%s,%ld(%s)\n",regnames[q1reg],pushed,regnames[sp]);
          }else{
	    emit(f,"\tst%c\t%s,%ld(%s)\n",q1reg<=32?'q':'t',regnames[q1reg],pushed,regnames[sp]);
	  }
          pushed+=8;
          continue;
        }
        if(c==ASSIGN) zreg=q1reg;
        continue;
      }else ierror(0);
    }
    if(c==ADDRESS){
      load_address(f,zreg,&p->q1,POINTER);
      continue;
    }
    if(c==MINUS){
      if(ISINT(t))
        emit(f,"\tsub%c\t%s,%s,%s\n",x_t[t&NQ],regnames[r31],regnames[q1reg],regnames[zreg]);
      else
        emit(f,"\tsub%c\t%s,%s,%s\n",x_t[t&NQ],regnames[f31],regnames[q1reg],regnames[zreg]);
      st[zreg]=ESGN;
      continue;
    }
    if(c==TEST){
      if(st[q1reg]==0) extend(f,q1reg);
      cmpreg=q1reg;
      cmpflag=0;
      continue;
    }
    if(c==COMPARE){
      struct IC *br=p->next;
      while(1){
        if(br->code>=BEQ&&br->code<BRA) break;
        if(br->code!=FREEREG) ierror(0);
        br=br->next;
      }
      if(ISFLOAT(t)) cmpreg=f3; else cmpreg=t3;
      if(br->code==BEQ||br->code==BNE){
        if((t&NU)==(UNSIGNED|INT)){
          if(st[q1reg]==ESGN) extend(f,q2reg);
          else if(st[q2reg]==ESGN) extend(f,q1reg);
          else if(st[q1reg]==EUNS) uextend(f,q2reg);
          else if(st[q2reg]==EUNS) uextend(f,q1reg);
          else {extend(f,q1reg);extend(f,q2reg);}
        }
        if((t&NU)==INT){extend(f,q1reg);extend(f,q2reg);}
        if(ISFLOAT(t)) emit(f,"\tsub%c\t%s,",x_t[t&NQ],regnames[q1reg]);
          else emit(f,"\tsub%c\t%s,",x_t[t&NQ],regnames[q1reg]);
        probj2(f,&p->q2,t);emit(f,",%s\n",regnames[cmpreg]);
        cmpflag=0;st[cmpreg]=ESGN;
      }else{
        char *s="";
        if(t&UNSIGNED) s="u";
        if((t&NU)==(UNSIGNED|INT)){uextend(f,q1reg);uextend(f,q2reg);}
        if((t&NU)==INT){extend(f,q1reg);extend(f,q2reg);}
        if(ISFLOAT(t)) s="t";
        if(br->code==BLT||br->code==BGE){
          emit(f,"\tcmp%slt\t%s,",s,regnames[q1reg]);
          probj2(f,&p->q2,t);emit(f,",%s\n",regnames[cmpreg]);
          if(br->code==BGE) cmpflag=-1; else cmpflag=1;
        }else{
          emit(f,"\tcmp%sle\t%s,",s,regnames[q1reg]);
          probj2(f,&p->q2,t);emit(f,",%s\n",regnames[cmpreg]);
          if(br->code==BGT) cmpflag=-1; else cmpflag=1;
        }
      }
      continue;
    }
    if(c>=OR&&c<=AND){
      emit(f,"\t%s\t%s,",logicals[c-OR],regnames[q1reg]);
      probj2(f,&p->q2,t);emit(f,",%s\n",regnames[zreg]);
      /* hier ist mehr moeglich */
      if((t&NQ)==INT) st[zreg]=0; else st[zreg]=ESGN;
      continue;
    }
    if(c>=LSHIFT&&c<=MOD){
      int xt;
      if(c==LSHIFT&&(p->q2.flags&KONST)){
        eval_const(&p->q2.val,t);
        if(zumeqto(vumax,ul2zum(1UL))){
          emit(f,"\tadd%c\t%s,%s,%s\n",x_t[t&NQ],regnames[q1reg],regnames[q1reg],regnames[zreg]);
          st[zreg]=ESGN;continue;
        }
        if(zumeqto(vumax,ul2zum(2UL))){
          emit(f,"\ts4add%c\t%s,0,%s\n",x_t[t&NQ],regnames[q1reg],regnames[zreg]);
          st[zreg]=ESGN;continue;
        }
        if(zumeqto(vumax,ul2zum(3UL))){
          emit(f,"\ts8add%c\t%s,0,%s\n",x_t[t&NQ],regnames[q1reg],regnames[zreg]);
          st[zreg]=ESGN;continue;
        }
      }
      if(c==RSHIFT||c==LSHIFT){
        if(c==RSHIFT){
          if(t&UNSIGNED){
            if((t&NQ)<LONG) uextend(f,q1reg);
            emit(f,"\tsrl\t");
          }else{
            extend(f,q1reg);
            emit(f,"\tsra\t");
          }
          st[zreg]=st[q1reg];
        }else{
          emit(f,"\tsll\t");
          if((t&NQ)<=INT) st[zreg]=0; else st[zreg]=ESGN;
        }
        emit(f,"%s,",regnames[q1reg]);
        probj2(f,&p->q2,t);emit(f,",%s\n",regnames[zreg]);
        continue;
      }
      if((c==DIV||c==MOD)&&ISINT(t)){
        /* Linux-Routinen aufrufen. q1=$24 q2=$25 z=$27 $28 scratch */
        if(!q1reg) ierror(0);
        if(q1reg!=25){
          if(regs[25]) emit(f,"\tstq\t%s,%ld(%s)\n",regnames[25],framesize-addbuf,regnames[sp]);
          move_reg(f,q1reg,25);
        }
        if(q2reg!=26&&regs[26]) emit(f,"\tstq\t%s,%ld(%s)\n",regnames[26],framesize-addbuf+8,regnames[sp]);
        if(q2reg==25)
          emit(f,"\tldq\t%s,%ld(%s)\n",regnames[26],framesize-addbuf,regnames[sp]);
        else
          load_reg(f,26,&p->q2,t,26);
        if(zreg!=28&&regs[28]) emit(f,"\tstq\t%s,%ld(%s)\n",regnames[28],framesize-addbuf+16,regnames[sp]);
        if(regs[29]) emit(f,"\tstq\t%s,%ld(%s)\n",regnames[29],framesize-addbuf+24,regnames[sp]);
        emit(f,"\t%sq%s\t%s,%s,%s\n",arithmetics[c-LSHIFT],(t&UNSIGNED)?"u":"",regnames[25],regnames[26],regnames[28]);
        if(zreg!=28) move_reg(f,28,zreg);
        if(q1reg!=25&&regs[25]) emit(f,"\tldq\t%s,%ld(%s)\n",regnames[25],framesize-addbuf,regnames[sp]);
        if(q2reg!=26&&regs[26]) emit(f,"\tldq\t%s,%ld(%s)\n",regnames[26],framesize-addbuf+8,regnames[sp]);
        if(zreg!=28&&regs[28]) emit(f,"\tldq\t%s,%ld(%s)\n",regnames[28],framesize-addbuf+16,regnames[sp]);
        if(regs[29]) emit(f,"\tldq\t%s,%ld(%s)\n",regnames[29],framesize-addbuf+24,regnames[sp]);
        /* Was fuer st[zreg]? */
        continue;
      }
      xt=x_t[t&NQ];
      if((t&NQ)<INT) xt='l';
      emit(f,"\t%s%c\t%s,",arithmetics[c-LSHIFT],xt,regnames[q1reg]);
      probj2(f,&p->q2,t);emit(f,",%s\n",regnames[zreg]);
      st[zreg]=ESGN;
      continue;
    }
    if(c==SUBPFP){
      emit(f,"\tsubq\t%s,%s,%s\n",regnames[q1reg],regnames[q2reg],regnames[zreg]);
      st[zreg]=ESGN;continue;
    }
    if(c==ADDI2P||c==SUBIFP){
      if(t&UNSIGNED){
        if((t&NQ)<LONG) uextend(f,q2reg);
      }else
        extend(f,q2reg);
      if(c==ADDI2P) emit(f,"\taddq\t%s,",regnames[q1reg]);
      else emit(f,"\tsubq\t%s,",regnames[q1reg]);
      probj2(f,&p->q2,t);
      emit(f,",%s\n",regnames[zreg]);
      st[zreg]=ESGN;continue;
    }
    ierror(0);
  }
  function_bottom(f,v,zm2l(offset+addbuf),maxpushed);
  if(debug_info){
    if(stabs){
      debug_exit(f,v);
    }else{
      emit(f,"%s%d:\n",labprefix,++label);
      dwarf2_function(f,v,label);
      if(f) section=-1;    
    }
  }  
}

int shortcut(int code,int typ)
{
  return 0;
}

int reg_parm(struct reg_handle *m, struct Typ *t,int vararg,struct Typ *fkt)
{
  int f;
  f=t->flags&NQ;
  if(ISINT(f)||ISPOINTER(f)){
    if(m->nextr>=6) return 0;
    return 17+m->nextr++;
  }
  if(ISFLOAT(f)){
    if(m->nextr>=6) return 0;
    return 49+m->nextr++;
  }
  return 0;
}
void cleanup_cg(FILE *f)
{
  struct fpconstlist *p;
  unsigned char *ip;

  title(f);
  if(f&&stabs) debug_cleanup(f);
  while(p=firstfpc){
    if(f){
      int t=p->typ&NQ;
      if(section!=CODE){emit(f,codename);if(f) section=CODE;}
      emit(f,"\t.align\t3\n%s%d:\n\t",labprefix,p->label);
      if(ISFLOAT(t)){
        ip=(unsigned char *)&p->val.vdouble;
        emit(f,"\t.long\t0x%02x%02x%02x%02x",ip[3],ip[2],ip[1],ip[0]);
        if((p->typ&NQ)!=FLOAT){
          emit(f,",0x%02x%02x%02x%02x",ip[7],ip[6],ip[5],ip[4]);
        }
      }else{
        emit(f,"\t.quad\t");
        emitval(f,&p->val,p->typ);
      }
      emit(f,"\n");
    }
    firstfpc=p->next;
    free(p);
  }
  if(f&&helps) emit(f,"\t.lcomm\t%s%d,%d\n",labprefix,helpl,helps*8);
}

void init_db(FILE *f)
{
  if(!stabs){
    dwarf2_setup(sizetab[POINTER],".byte",".2byte",".4byte",".8byte",labprefix,idprefix,".section");
    dwarf2_print_comp_unit_header(f);
  }
} 
void cleanup_db(FILE *f)
{
  if(!stabs&&f)
    dwarf2_cleanup(f);
  if(f) section=-1;
}

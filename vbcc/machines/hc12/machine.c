/*  Code generator for Motorola 68hc12 microcontrollers.               */

/*TODO:
  regs_modified bei struct-copy
  savings verfeinern
  4-Byte Copy
  [static] testen
  peephole-Pass um ALLOCREGs zu entfernen
  MUL-Problemfälle
  DIV/MOD
  ACC_IND (Achtung?)
  struct-copy Problemfälle
  banked
  bit
  long, long long, float, double, long double

*/

#include "supp.h"
#include "vbc.h" /* nicht schoen, aber ... */

static char FILE_[]=__FILE__;

#include "dwarf2.c"

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for hc12 V0.0 (c) in 2000 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,VALFLAG,0,0,
                    0,0,0,0};
char *g_flags_name[MAXGF]={"cpu","fpu","no-delayed-popping","const-in-data",
                           "merge-constants","no-peephole","mem-cse","--"};
union ppi g_flags_val[MAXGF];

/* Typenames (needed because of HAVE_EXT_TYPES). */
char *typname[]={"strange","bit","char","short","int","long","long long",
		 "float","double","long double","void",
                 "near-pointer","far-pointer","huge-pointer",
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
char *regnames[]={"noreg","d","x","y","sp"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,1,1};

int reg_prio[MAXR+1]={0,0,1,1,0};

struct reg_handle empty_reg_handle={0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__interrupt",0};
#define INTERRUPT 1

/****************************************/
/*  Some private data and functions.    */
/****************************************/

static long malign[MAX_TYPE+1]=  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static long msizetab[MAX_TYPE+1]={0,1,1,2,2,4,4,4,8,8,0,2,4,4,0,0,0,2,0};

struct Typ ityp={SHORT},ltyp={LONG};

#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define SPECIAL 4

static int section=-1,newobj,scnt,pushed_acc;
static char *codename="\t.text\n",
            *dataname="\t.data\n",
            *bssname="\t.section\t.bss\n",
            *rodataname="\t.section\t.rodata\n";

#define IMM_IND  1
#define VAR_IND  2
#define POST_INC 3
#define POST_DEC 4
#define PRE_INC  5
#define PRE_DEC  6
#define ACC_IND  7

/* (user)stack-pointer, pointer-tmp, int-tmp; reserved for compiler */
static int acc=1,ix=2,iy=3,sp=4;
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

static char *marray[]={"__section(x,y)=__vattr(\"section(\"#x\",\"#y\")\")",
		       "__HC12__",
		       0};

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

static long loff,stackoffset,notpopped,dontpop,maxpushed,stack;

static char *x_t[]={"?","","b","","","","","","","","","","","","","",""};
static char *ccs[]={"eq","ne","lt","ge","le","gt"};
static char *uccs[]={"eq","ne","lo","hs","ls","hi"};
static char *logicals[]={"ora","eor","and"};
static char *dct[]={"",".bit",".byte",".2byte",".2byte",".4byte",".8byte",".4byte",".8byte",".8byte",
		    "(void)",".2byte",".34byte",".34byte"};
static char *idprefix="",*labprefix="l";
static int exit_label,have_frame;
static char *ret;
static int stackchecklabel;
static int frame_used,stack_valid;

static int cc_t;
static struct obj *cc;

static struct obj mobj;

#define STR_NEAR "near"
#define STR_FAR "far"
#define STR_HUGE "huge"
#define STR_BADDR "baddr"

#define ISNULL() (zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
#define ISLWORD(t) ((t&NQ)==LONG||(t&NQ)==FPOINTER||(t&NQ)==HPOINTER||(t&NQ)==FLOAT)
#define ISHWORD(t) ((t&NQ)==INT||(t&NQ)==SHORT||(t&NQ)==NPOINTER)
#define ISSTATIC(v) ((v)->storage_class==EXTERN||(v)->storage_class==STATIC)
#define ISBADDR(v) ((v)->vtyp->attr&&strstr(STR_BADDR,(v)->vtyp->attr))
/*FIXME*/
#define ISFAR(v) ((v)->vtyp->attr&&(strstr(STR_FAR,(v)->vtyp->attr)||strstr(STR_HUGE,(v)->vtyp->attr)))

#define ISACC(x) ((x)==acc)
#define ISX(x) ((x)==ix)
#define ISY(x) ((x)==iy)
#define ISIDX(x) (ISX(x)||ISY(x))
#define ISRACC(x) (isreg(x)&&ISACC(p->x.reg))
#define ISRX(x) (isreg(x)&&ISX(p->x.reg))
#define ISRY(x) (isreg(x)&&ISY(p->x.reg))
#define ISRIDX(x) (isreg(x)&&ISIDX(p->x.reg))

int emit_peephole(void)
{
  int entries,i;
  char *asmline[EMIT_BUF_DEPTH];
  i=emit_l;
  if(emit_f==0)
    entries=i-emit_f+1;
  else
    entries=EMIT_BUF_DEPTH;
  asmline[0]=emit_buffer[i];
  if(entries>=2){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[1]=emit_buffer[i];
    if(!strcmp(asmline[0],"\ttfr\tx,d\n")&&!strcmp(asmline[1],"\ttfr\td,x\n")){
      remove_asm();
      return 1;
    }
    if(!strcmp(asmline[0],"\ttfr\ty,d\n")&&!strcmp(asmline[1],"\ttfr\td,y\n")){
      remove_asm();
      return 1;
    }
    if(!strcmp(asmline[0],"\tstd\t0,sp\n")&&!strcmp(asmline[1],"\tpshd\n")){
      remove_asm();
      return 1;
    }
    if(!strcmp(asmline[0],"\tldd\t0,sp\n")&&!strcmp(asmline[1],"\tpshd\n")){
      remove_asm();
      return 1;
    }
    if((!strcmp(asmline[1],"\tcpx\t#0\n")||!strcmp(asmline[1],"\tcpy\t#0\n"))
       &&(!strncmp(asmline[0],"\tbne\t",5)||!strncmp(asmline[0],"\tbeq\t",5))){
      strcpy(asmline[1],asmline[0]);
      remove_asm();
      return 1;
    }  
    if((!strcmp(asmline[1],"\tdex\n")||!strcmp(asmline[1],"\tdey\n")||!strcmp(asmline[1],"\tsubd\t#1\n"))&&
       (!strncmp(asmline[0],"\tbne\t",5)||!strncmp(asmline[0],"\tbeq\t",5))){
      char r=asmline[1][3];
      if(r=='b') r='d';
      strcpy(asmline[1],"\td");
      strncpy(asmline[1]+2,asmline[0]+1,4);
      asmline[1][6]=r;asmline[1][7]=',';
      strcpy(asmline[1]+8,asmline[0]+5);
      remove_asm();
      return 1;
    }
    if((!strcmp(asmline[1],"\tinx\n")||!strcmp(asmline[1],"\tiny\n")||!strcmp(asmline[1],"\taddd\t#1\n"))&&
       (!strncmp(asmline[0],"\tbne\t",5)||!strncmp(asmline[0],"\tbeq\t",5))){
      char r=asmline[1][3];
      strcpy(asmline[1],"\ti");
      strncpy(asmline[1]+2,asmline[0]+1,4);
      asmline[1][6]=r;asmline[1][7]=',';
      strcpy(asmline[1]+8,asmline[0]+5);
      remove_asm();
      return 1;
    }
    if((!strcmp(asmline[1],"\tcpx\t#0\n")||!strcmp(asmline[1],"\tcpy\t#0\n")||!strcmp(asmline[1],"\tcpd\t#0\n"))&&
       (!strncmp(asmline[0],"\tbne\t",5)||!strncmp(asmline[0],"\tbeq\t",5))){
      char r=asmline[1][3];
      strcpy(asmline[1],"\tt");
      strncpy(asmline[1]+2,asmline[0]+1,4);
      asmline[1][6]=r;asmline[1][7]=',';
      strcpy(asmline[1]+8,asmline[0]+5);
      remove_asm();
      return 1;
    }
  }
  return 0;
}

static int special_section(FILE *f,struct Var *v)
{
  char *sec;
  if(!v->vattr) return 0;
  sec=strstr(v->vattr,"section(");
  if(!sec) return 0;
  sec+=strlen("section(");
  emit(f,"\t.section\t");
  while(*sec&&*sec!=')') emit_char(f,*sec++);
  emit(f,"\n");
  if(f) section=SPECIAL;
  return 1;
}

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

int pointer_type(struct Typ *p)
{
  if(!p) ierror(0);
  while((p->flags&NQ)==ARRAY) p=p->next;
  if((p->flags&NQ)==FUNKT) return NPOINTER; /*FIXME: banked*/
  if(p->attr){
    if(strstr(p->attr,STR_HUGE)) return HPOINTER;
    if(strstr(p->attr,STR_FAR)) return FPOINTER;
    if(strstr(p->attr,STR_NEAR)) return NPOINTER;
  }
  /*FIXME*/
  return NPOINTER;
}
static long voff(struct obj *p)
{
  if(zm2l(p->v->offset)<0) 
    return loff-zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset+1;
  else
    return zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset;
}

static void emit_obj(FILE *f,struct obj *p,int t)
/*  Gibt Objekt auf Bildschirm aus                      */
{
  if(p->am){
    int flags=p->am->flags;
    if(flags==ACC_IND){
      emit(f,"%s,%s",regnames[acc],regnames[p->am->base]);
      return;
    }
    emit(f,"%ld",p->am->offset);
    if(p->am->v){
      if(p->am->v->storage_class==STATIC)
	emit(f,"+%s%ld",labprefix,zm2l(p->am->v->offset));
      else
	emit(f,"+(%s%s)",idprefix,p->am->v->identifier);
    }
    emit(f,",");
    if(flags==PRE_INC){
      emit(f,"+");
    }else if(flags==PRE_DEC){
      emit(f,"-");
    }
    emit(f,"%s",regnames[p->am->base]);
    if(flags==POST_INC){
      emit(f,"+");
    }else if(flags==POST_DEC){
      emit(f,"-");
    }
    return;
  }
  if((p->flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
    emitval(f,&p->val,p->dtyp&NU);
    return;
  }
  if(p->flags&VARADR) emit(f,"#");
  if((p->flags&(DREFOBJ|REG))==(DREFOBJ|REG)) emit(f,"0,");
  if((p->flags&(DREFOBJ|REG))==DREFOBJ) emit(f,"[");
  if((p->flags&(VAR|REG))==VAR){
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      emit(f,"%ld,%s",voff(p),regnames[sp]);
    }else{
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,LONG);emit(f,"+");}
      if(p->v->storage_class==STATIC){
	emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
	emit(f,"(%s%s)",idprefix,p->v->identifier);
      }
    }
  }
  if(p->flags&REG){
    if(ISACC(p->reg)&&(t&NQ)==CHAR)
      emit(f,"b");
    else
      emit(f,"%s",regnames[p->reg]);
  }
  if(p->flags&KONST){
    if(ISFLOAT(t)){
      emit(f,"%s%d",labprefix,addfpconst(p,t));
    }else{
      emit(f,"#");emitval(f,&p->val,t&NU);
    }
  }
  if((p->flags&(DREFOBJ|REG))==DREFOBJ){
    if(p->v->storage_class==EXTERN||p->v->storage_class==STATIC)
      emit(f,",pc");
    emit(f,"]");
  }
}

static void dwarf2_print_frame_location(FILE *f,struct Var *v)
{
  /*FIXME: needs a location list and correct register translation */
  struct obj o;
  o.flags=REG;
  o.reg=sp;
  o.val.vmax=l2zm(0L);
  o.v=0;
  dwarf2_print_location(f,&o);
}
static int dwarf2_regnumber(int r)
{
  /*FIXME: always returns D as accumulator, even if byte size */
  static dwarf_regs[MAXR+1]={-1,3,7,8,15};
  return dwarf_regs[r];
}
static zmax dwarf2_fboffset(struct Var *v)
{
  /*FIXME*/
  if(!v||(v->storage_class!=AUTO&&v->storage_class!=REGISTER)) ierror(0);
  if(!zmleq(l2zm(0L),v->offset))
    return l2zm((long)(loff-zm2l(v->offset)));
  else
    return v->offset;
} 

/* test operand for mov instruction */
static int mov_op(struct obj *o)
{
  long off;
  if(o->am){
    int f=o->am->flags;
    if(f==POST_INC||f==PRE_INC||f==POST_DEC||f==PRE_DEC||f==ACC_IND)
      return 1;
    if(f==IMM_IND){
      if(o->am->v) return 0;
      off=o->am->offset;
      if(off>=-16&&off<=15)
	return 1;
      else
	return 0;
    }
    ierror(0);
  }
  if(o->flags&(KONST|VARADR)) return 1;
  if((o->flags&(REG|DREFOBJ))==(REG|DREFOBJ)) return 1;
  if((o->flags&(VAR|REG|DREFOBJ))==VAR){
    if(o->v->storage_class==STATIC||o->v->storage_class==EXTERN)
      return 1;
    off=voff(o);
    if(off>=-16&&off<=15)
      return 1;
    else
      return 0;
  }
  return 0;
}

/* add an offset to an object describing a memory address */
static void inc_addr(struct obj *o,long val)
{
  if(o->am){
    int f=o->am->flags;
    if(f==IMM_IND)
      o->am->offset+=val;
    else if(f==POST_INC||f==POST_DEC||f==PRE_INC||f==PRE_DEC){
      struct AddressingMode *old=o->am;
      o->am=mymalloc(sizeof(*o->am));
      o->am->flags=IMM_IND;
      o->am->base=old->base;
      o->am->v=0;
      if(f==POST_DEC) o->am->offset=old->offset-val;
      else if(f==POST_INC) o->am->offset=-old->offset+val;
      else if(f==PRE_DEC) o->am->offset=val;
      else o->am->offset=-val;
    }else
      ierror(0);
  }else if(o->flags&DREFOBJ){
    struct AddressingMode *am;
    o->am=am=mymalloc(sizeof(*am));
    am->flags=IMM_IND;
    if(!o->reg) ierror(0);
    am->base=o->reg;
    am->offset=zm2l(val);
    am->v=0;
  }else{
    o->val.vmax=zmadd(o->val.vmax,val);
  }
}

/* pushed on the stack by a callee, no pop needed */
static void callee_push(long l)
{
  if(l-stackoffset>stack)
    stack=l-stackoffset;
}
static void push(long l)
{
  stackoffset-=l;
  if(stackoffset<maxpushed) maxpushed=stackoffset;
  if(-maxpushed>stack) stack=-maxpushed;
}
static void pop(long l)
{
  stackoffset+=l;
}
static void gen_pop(FILE *f,long l)
{
  if(l==0) return;
  if(l==1){
    emit(f,"\tins\n");
  }else if(l==2&&!regs[acc]){
    emit(f,"\tpuld\n");
    BSET(regs_modified,acc);
  }else if(l==2&&!regs[ix]){
    emit(f,"\tpulx\n");
    BSET(regs_modified,ix);
  }else if(l==2&&!regs[iy]){
    emit(f,"\tpuly\n");
    BSET(regs_modified,iy);
  }else{
    emit(f,"\tleas\t%ld,%s\n",l,regnames[sp]);
  }
  pop(l);
}
static void pr(FILE *f,struct IC *p)
{
  if(pushed_acc){
    emit(f,"\tpuld\n");
    pop(2);
    pushed_acc=0;
  }
}
static void function_top(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionskopf                       */
{
  int i;
  emit(f,"# offset=%ld\n",offset);
  have_frame=0;stack_valid=1;stack=0;
  if(!special_section(f,v)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
  if(v->storage_class==EXTERN){
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier);
  }else{
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
  }
  if(stack_check){
    stackchecklabel=++label;
    emit(f,"\tldy\t#%s%d\n",labprefix,stackchecklabel);
    /* FIXME: banked */
    emit(f,"\tbsr\t%s__stack_check\n",idprefix);
  }
  if(offset){
    if(offset==1)
      emit(f,"\tpshb\n");
    else if(offset==2)
      emit(f,"\tpshd\n");
    else
      emit(f,"\tleas\t-%ld,%s\n",offset,regnames[sp]);
    have_frame=1;
  }
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionsende                       */
{
  int i;
  if(offset){
    if(offset==1)
      emit(f,"\tins\n");
    else if(offset==2)
      emit(f,"\tpulx\n");
    else
      emit(f,"\tleas\t%ld,%s\n",offset,regnames[sp]);
  }
  if(ret) emit(f,"\t%s\n",ret);
  if(v->storage_class==EXTERN){
    emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
    emit(f,"\t.size\t%s%s,$-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
  }else{
    emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,$-%s%ld\n",labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
  }
  if(stack_check)
    emit(f,"\t.equ\t%s%d,%ld\n",labprefix,stackchecklabel,offset-maxpushed);
  if(stack_valid){
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_STACK;
    v->fi->stack1=l2zm(stack+offset);
    emit(f,"# stacksize=%ld\n",stack+offset);
    emit(f,"\t.equ\t%s__stack_%s,%ld\n",idprefix,v->identifier,stack+offset);
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

/*FIXME*/
static void clear_ext_ic(struct ext_ic *p)
{
  p->flags=0;
  p->r=0;
  p->offset=0;
}
static long pof2(zumax x)
/*  Yields log2(x)+1 oder 0. */
{
  zumax p;int ln=1;
  p=ul2zum(1L);
  while(ln<=32&&zumleq(p,x)){
    if(zumeqto(x,p)) return ln;
    ln++;p=zumadd(p,p);
  }
  return 0;
}
static void peephole(struct IC *p)
{
  int c,c2,r,t;struct IC *p2;
  struct AddressingMode *am;
  frame_used=0;
  for(;p;p=p->next){
    c=p->code;
    if(!frame_used){
      if((p->q1.flags&(REG|VAR))==VAR&&!ISSTATIC(p->q1.v)) frame_used=1;
      if((p->q2.flags&(REG|VAR))==VAR&&!ISSTATIC(p->q2.v)) frame_used=1;
      if((p->z.flags&(REG|VAR))==VAR&&!ISSTATIC(p->z.v)) frame_used=1;
    }
    /* letztes Label merken */
    if(c!=FREEREG&&c!=ALLOCREG&&(c!=SETRETURN||!isreg(q1)||p->q1.reg!=p->z.reg)) exit_label=0;
    if(c==LABEL) exit_label=p->typf;
#if 0
    /* and x,#const;bne/beq, FIXME */
    if(c==AND&&isconst(q2)&&isreg(z)){
      long bit;
      eval_const(&p->q2.val,p->typf);
      if(bit=pof2(vumax)){
	struct IC *cmp=0;int fr=0;
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(c2==TEST){
	    if((p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==p->z.reg){
	      cmp=p2;continue;
	    }
	  }
	  if(c2==COMPARE&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==p->z.reg&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf);
	    if(ISNULL()){
	      cmp=p2;continue;
	    }
	    break;
	  }
	  if(c2==FREEREG&&p2->q1.reg==p->z.reg) {fr++;continue;}
	  if((c2==BNE||c2==BEQ)&&cmp&&fr==1){
	    p->ext.flags=EXT_IC_BTST;
	    p2->ext.flags=EXT_IC_BTST;
	    p2->ext.offset=bit-1;
	    cmp->code=NOP;
	    cmp->q1.flags=cmp->q2.flags=cmp->z.flags=0;
	    break;
	  }
	  if(((p2->q1.flags&REG)&&p2->q1.reg==p->z.reg)||((p2->q2.flags&REG)&&p2->q2.reg==p->z.reg)||((p2->z.flags&REG)&&p2->z.reg==p->z.reg)) break;
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	}
      }
    }
#endif
    /* Try d,idx */
    if(c==ADDI2P&&ISRACC(q2)&&ISRIDX(z)&&(ISRIDX(q1)||p->q2.reg!=p->z.reg)){
      int base,idx;struct obj *o;
      r=p->z.reg;idx=p->q2.reg;
      if(isreg(q1)) base=p->q1.reg; else base=r;
      o=0;
      for(p2=p->next;p2;p2=p2->next){
        c2=p2->code;
        if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
        if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
        if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
        if((p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==idx&&idx!=r) break;
	
        if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
          if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            if(o||!ISHWORD(q1typ(p2))) break;
            o=&p2->q1;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            if(o||!ISHWORD(q2typ(p2))) break;
            o=&p2->q2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            if(o||!ISHWORD(ztyp(p2))) break;
            o=&p2->z;
          }
        }
        if(c2==FREEREG||(p2->z.flags&(REG|DREFOBJ))==REG){
          int m;
          if(c2==FREEREG)
            m=p2->q1.reg;
          else
            m=p2->z.reg;
          if(m==r){
            if(o){
              o->am=am=mymalloc(sizeof(*am));
              am->flags=ACC_IND;
              am->base=base;
	      if(idx!=acc) ierror(0);
              am->offset=idx;
	      if(isreg(q1)){
		p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
	      }else{
		p->code=c=ASSIGN;p->q2.flags=0;
		p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
	      }
            }
            break;
          }
          if(c2!=FREEREG&&m==base) break;
	  continue;
	}
	/* better no instructions between, accu used too much */
	if(c2!=FREEREG&&c2!=ALLOCREG&&!o) break;
      }
    }
    /* POST_INC/DEC in q1 */
    if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->q1.reg; t=q1typ(p);
      if(ISIDX(r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((c2==ADD||c2==ADDI2P||c2==SUB||c2==SUBIFP)&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(c2==SUB||c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	    if(zmleq(vmax,l2zm(8L))&&zmleq(l2zm(-8L),vmax)){
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      p->q1.am=mymalloc(sizeof(*am));
	      p->q1.am->base=r;
	      p->q1.am->v=0;
	      if(zmleq(vmax,l2zm(0L))){
		p->q1.am->flags=POST_DEC;
		p->q1.am->offset=-zm2l(vmax);
	      }else{
		p->q1.am->flags=POST_INC;
		p->q1.am->offset=zm2l(vmax);
	      }
	    }else break;
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(((p2->q1.flags&REG)&&p2->q1.reg==r)||((p2->q2.flags&REG)&&p2->q2.reg==r)||((p2->z.flags&REG)&&p2->z.reg==r)) break;
	}
      }
    }
    /* POST_INC/DEC in q2 */
    if(!p->q2.am&&(p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->q2.reg; t=q2typ(p);
      if(ISIDX(r)&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  if((p2->code==ADD||p2->code==ADDI2P||c2==SUB||c2==SUBIFP)&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(c2==SUB||c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	    if(zmleq(vmax,l2zm(8L))&&zmleq(l2zm(-8L),vmax)){
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      p->q2.am=mymalloc(sizeof(*am));
	      p->q2.am->base=r;
	      p->q2.am->v=0;
	      if(zmleq(vmax,l2zm(0L))){
		p->q2.am->flags=POST_DEC;
		p->q2.am->offset=-zm2l(vmax);
	      }else{
		p->q2.am->flags=POST_INC;
		p->q2.am->offset=zm2l(vmax);
	      }
	    }else break;
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(((p2->q1.flags&REG)&&p2->q1.reg==r)||((p2->q2.flags&REG)&&p2->q2.reg==r)||((p2->z.flags&REG)&&p2->z.reg==r)) break;
	}
      }
    }
    /* POST_INC/DEC in z */
    if(!p->z.am&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->z.reg; t=ztyp(p);
      if(ISIDX(r)&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  if((p2->code==ADD||p2->code==ADDI2P||c2==SUB||c2==SUBIFP)&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(c2==SUB||c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	    if(zmleq(vmax,l2zm(8L))&&zmleq(l2zm(-8L),vmax)){
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      p->z.am=mymalloc(sizeof(*am));
	      p->z.am->base=r;
	      p->z.am->v=0;
	      if(zmleq(vmax,l2zm(0L))){
		p->z.am->flags=POST_DEC;
		p->z.am->offset=-zm2l(vmax);
	      }else{
		p->z.am->flags=POST_INC;
		p->z.am->offset=zm2l(vmax);
	      }
	    }else break;
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(((p2->q1.flags&REG)&&p2->q1.reg==r)||((p2->q2.flags&REG)&&p2->q2.reg==r)||((p2->z.flags&REG)&&p2->z.reg==r)) break;
	}
      }
    }

    /* R,#c */
    if((c==ADDI2P||c==SUBIFP)&&((p->typf2&NQ)==NPOINTER||(p->typf2&NQ)==FPOINTER)&&isreg(z)&&((p->q2.flags&(KONST|DREFOBJ))==KONST||(p->q1.flags&VARADR))){
      int base;zmax of;struct obj *o;struct Var *v;
      if(p->q1.flags&VARADR){
	v=p->q1.v;
	of=p->q1.val.vmax;
	r=p->z.reg;
	if(isreg(q2)&&ISIDX(p->q2.reg))
	  base=p->q2.reg;
	else
	  base=r;
      }else{
	eval_const(&p->q2.val,p->typf);
	if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
	v=0;
	r=p->z.reg;
	if(isreg(q1)&&ISIDX(p->q1.reg)) base=p->q1.reg; else base=r;
      }
      o=0;
      for(p2=p->next;p2;p2=p2->next){
	c2=p2->code;
	if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
	if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
	if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
	  if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
	    if(o) break;
	    o=&p2->q1;
	  }
	  if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
	    if(o) break;
	    o=&p2->q2;
	  }
	  if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
	    if(o) break;
	    o=&p2->z;
	  }
	}
	if(c2==FREEREG||(p2->z.flags&(REG|DREFOBJ))==REG){
	  int m;
	  if(c2==FREEREG) 
	    m=p2->q1.reg;
	  else
	    m=p2->z.reg;
	  if(m==r){
	    if(o){
	      o->am=am=mymalloc(sizeof(*am));
	      am->flags=IMM_IND;
	      am->base=base;
	      am->offset=zm2l(of);
	      am->v=v;
	      if(!v){
		if(isreg(q1)&&ISIDX(p->q1.reg)){
		  p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
		}else{
		  p->code=c=ASSIGN;p->q2.flags=0;
		  p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
		}
	      }else{
		if(isreg(q2)&&ISIDX(p->q2.reg)){
		  p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
		}else{
		  p->code=c=ASSIGN;p->q1=p->q2;p->q2.flags=0;
		  p->q2.val.vmax=sizetab[p->typf&NQ];
		}
	      }
	    }
	    break;
	  }
	  if(/*get_reg!! c2!=FREEREG&&*/m==base) break;
	  continue;
	}
      }
    }      
  }
}

static struct obj *cam(int flags,int base,long offset,struct Var *v)
/*  Initializes an addressing-mode structure and returns a pointer to   */
/*  that object. Will not survive a second call!                        */
{
  static struct obj obj;
  static struct AddressingMode am;
  obj.am=&am;
  am.flags=flags;
  am.base=base;
  am.offset=offset;
  am.v=v;
  return &obj;
}

static void get_acc(FILE *f,struct IC *p)
{
  if(regs[acc]){
    if(p->q2.am)
      if(p->q2.am->flags==ACC_IND) ierror(0);
    else
      if((p->q2.flags&REG)&&ISACC(p->q2.reg)) ierror(0);
    if(p->z.am)
      if(p->z.am->flags==ACC_IND) ierror(0);
    else
      if((p->z.flags&REG)&&ISACC(p->z.reg)) ierror(0);
    if(regs[acc]){
      emit(f,"\tpshd\n");
      push(2);
      pushed_acc=1;
    }
  }
}
static int get_reg(FILE *f,struct IC *p,int t)
{
  int reg;
  if(!regs[acc])
    reg=acc;
  else if(ISHWORD(t)&&!regs[ix])
    reg=ix;
  else if(ISHWORD(t)&&!regs[iy])
    reg=iy;
  else{
    get_acc(f,p);;
    reg=acc;
  }
  BSET(regs_modified,reg);
  return reg;
}
static void load_reg(FILE *f,int r,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==r) return;
    emit(f,"\ttfr\t%s,%s\n",regnames[o->reg],regnames[r]);
    return;
  }
  if(r==acc&&(o->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&o->val,t);
    if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))){
      emit(f,"\tclra\n\tclrb\n");
      cc=o;cc_t=t;
      return;
    }
  }
  emit(f,"\tld%s\t",(r==acc&&(t&NQ)==CHAR)?"ab":regnames[r]);
  emit_obj(f,o,t);emit(f,"\n");
  cc=o;cc_t=t;
}
static void store_reg(FILE *f,int r,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==r) return;
    emit(f,"\ttfr\t%s,%s\n",regnames[r],regnames[o->reg]);
  }else{
    emit(f,"\tst%s\t",(r==acc&&(t&NQ)==CHAR)?"ab":regnames[r]);
    emit_obj(f,o,t);emit(f,"\n");
    cc=o;cc_t=t;
  }
}
static void load_addr(FILE *f,int r,struct obj *o)
{
  if(o->am){
    if(o->am->flags==IMM_IND){
      if(o->am->base==r&&o->am->offset==0&&!o->am->v) return;
      if(ISIDX(r)){
	emit(f,"\tlea%s\t",regnames[r]);
	emit_obj(f,o,0);
	emit(f,"\n");
      }else{
	if(r!=acc) ierror(0);
	emit(f,"\ttfr\t%s,%s\n",regnames[o->am->base],regnames[r]);
	emit(f,"\taddd\t#%ld\n",o->am->offset);
	if(o->am->v){
	  if(o->am->v->storage_class==STATIC)
	    emit(f,"+%s%ld",labprefix,zm2l(o->am->v->offset));
	  else
	    emit(f,"+%s%s",idprefix,o->am->v->identifier);
	}
	emit(f,"\n");
	cc=0;
      }
      return;
    }
    ierror(0);
  }
  if(o->flags&DREFOBJ){
    o->flags&=~DREFOBJ;
    load_reg(f,r,o,o->dtyp);
    o->flags|=DREFOBJ;
    return;
  }
  if((o->flags&(VAR|VARADR))==VAR){
    if(o->v->storage_class==STATIC||o->v->storage_class==EXTERN){
      o->flags|=VARADR;
      load_reg(f,r,o,POINTER_TYPE(o->v->vtyp));
      o->flags&=~VARADR;
      return;
    }
    if(voff(o)==0){
      emit(f,"\ttfr\t%s,%s\n",regnames[sp],regnames[r]);
      return;
    }
    if(ISIDX(r)){
      emit(f,"\tlea%s\t",regnames[r]);
      emit_obj(f,o,0);
      emit(f,"\n");
    }else{
      if(r!=acc) ierror(0);
      emit(f,"\ttfr\t%s,%s\n",regnames[sp],regnames[r]);
      emit(f,"\taddd\t#%ld\n",voff(o));
      cc=0;
    }
    return;
  }
  ierror(0);
}

static int scratchreg(int r,struct IC *p)
{
  int c;
  while(1){
    p=p->next;
    if(!p||((c=p->code)==FREEREG&&p->q1.reg==r)) return 1;
    if(c==CALL||(c>=BEQ&&c<=BRA)) return 0;
    if((p->q1.flags&REG)&&p->q1.reg==r) return 0;
    if((p->q2.flags&REG)&&p->q2.reg==r) return 0;
    if((p->z.flags&REG)&&p->z.reg==r) return 0;
  }
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
  maxalign=l2zm(1L);
  char_bit=l2zm(8L);
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i=1;i<=MAXR;i++){
    regsize[i]=l2zm(2L);regtype[i]=&ityp;
  }	
  
  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-128L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[INT]=t_min[SHORT];
  t_min[LONG]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(127L);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[INT]=t_max[SHORT];
  t_max[LONG]=ul2zum(2147483647UL);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT]=t_max(LLONG);
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=tu_max[SHORT];
  tu_max[LONG]=ul2zum(4294967295UL);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

  /*  Reserve a few registers for use by the code-generator.      */
  regsa[sp]=1;
  regscratch[sp]=0;

  if(!(g_flags[6]&USEDFLAG)){
    extern int static_cse,dref_cse;
    static_cse=0;
    dref_cse=0;
  }

  target_macros=marray;


  /* TODO: set argument registers */
  declare_builtin("__mulint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__addint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__subint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__andint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__orint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__eorint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__negint32",LONG,LONG,0,0,0,1,0);
  declare_builtin("__lslint32",LONG,LONG,0,INT,0,1,0);

  declare_builtin("__divsint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__divuint32",UNSIGNED|LONG,UNSIGNED|LONG,0,UNSIGNED|LONG,0,1,0);
  declare_builtin("__modsint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__moduint32",UNSIGNED|LONG,UNSIGNED|LONG,0,UNSIGNED|LONG,0,1,0);
  declare_builtin("__lsrsint32",LONG,LONG,0,INT,0,1,0);
  declare_builtin("__lsruint32",UNSIGNED|LONG,UNSIGNED|LONG,0,INT,0,1,0);
  declare_builtin("__cmpsint32",INT,LONG,0,LONG,0,1,0);
  declare_builtin("__cmpuint32",INT,UNSIGNED|LONG,0,UNSIGNED|LONG,0,1,0);
  declare_builtin("__sint32toflt32",FLOAT,LONG,0,0,0,1,0);
  declare_builtin("__uint32toflt32",FLOAT,UNSIGNED|LONG,0,0,0,1,0);
  declare_builtin("__sint32toflt64",DOUBLE,LONG,0,0,0,1,0);
  declare_builtin("__uint32toflt64",DOUBLE,UNSIGNED|LONG,0,0,0,1,0);
  declare_builtin("__flt32tosint32",LONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt32touint32",UNSIGNED|LONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64tosint32",LONG,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint32",UNSIGNED|LONG,DOUBLE,0,0,0,1,0);



  declare_builtin("__mulint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__addint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__subint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__andint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__orint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__eorint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__negint64",LLONG,LLONG,0,0,0,1,0);
  declare_builtin("__lslint64",LLONG,LLONG,0,INT,0,1,0);

  declare_builtin("__divsint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__divuint64",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__modsint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__moduint64",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__lsrsint64",LLONG,LLONG,0,INT,0,1,0);
  declare_builtin("__lsruint64",UNSIGNED|LLONG,UNSIGNED|LLONG,0,INT,0,1,0);
  declare_builtin("__cmpsint64",INT,LLONG,0,LLONG,0,1,0);
  declare_builtin("__cmpuint64",INT,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__sint64toflt32",FLOAT,LLONG,0,0,0,1,0);
  declare_builtin("__uint64toflt32",FLOAT,UNSIGNED|LLONG,0,0,0,1,0);
  declare_builtin("__sint64toflt64",DOUBLE,LLONG,0,0,0,1,0);
  declare_builtin("__uint64toflt64",DOUBLE,UNSIGNED|LLONG,0,0,0,1,0);
  declare_builtin("__flt32tosint64",LLONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt32touint64",UNSIGNED|LLONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64tosint64",LLONG,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint64",UNSIGNED|LLONG,DOUBLE,0,0,0,1,0);

  declare_builtin("__flt32toflt64",DOUBLE,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64toflt32",FLOAT,DOUBLE,0,0,0,1,0);


  declare_builtin("__addflt32",FLOAT,FLOAT,0,FLOAT,0,1,0);
  declare_builtin("__subflt32",FLOAT,FLOAT,0,FLOAT,0,1,0);
  declare_builtin("__mulflt32",FLOAT,FLOAT,0,FLOAT,0,1,0);
  declare_builtin("__divflt32",FLOAT,FLOAT,0,FLOAT,0,1,0);
  declare_builtin("__negflt32",FLOAT,FLOAT,0,FLOAT,0,1,0);
  declare_builtin("__cmpflt32",INT,FLOAT,0,FLOAT,0,1,0);

  declare_builtin("__addflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__subflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__mulflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__divflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__negflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__cmpflt64",INT,DOUBLE,0,DOUBLE,0,1,0);


  return 1;
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
{
  int f=t->flags&NQ;
  if(ISSCALAR(f)&&(ISHWORD(f)||f==CHAR))
    return acc;
  return 0;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if(!ISSCALAR(t)) return 0;
  if(mode==-1){
    if(ISHWORD(t)) return 1;
    if((t&NQ)==CHAR&&ISACC(r)) return 1;
  }else{
    if(ISIDX(r)){
      if(ISPOINTER(t)&&ISHWORD(t))
	return 1;
    }
    if(ISACC(r)){
      if((t&NQ)==CHAR)
	return 1;
      if(ISINT(t)&&ISHWORD(t))
	return 1;
    }
  }
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

int cost_savings(struct IC *p,int r,struct obj *o)
{
  /*FIXME*/
  int c=p->code;
  if(o->flags&VKONST){
    struct obj *co=&o->v->cobj;
    if(o->flags&DREFOBJ)
      return 0;
    if(o==&p->q1&&p->code==ASSIGN&&((p->z.flags&DREFOBJ)||p->z.v->storage_class==STATIC||p->z.v->storage_class==EXTERN)){
      return 2;
    }
    return 0;
  }
  if((o->flags&DREFOBJ)){
    if(!ISIDX(r)) return INT_MIN;
    if(r<=4&&p->q2.flags&&o!=&p->z)
      return 6;
    else
      return 4;
  }else if(c==GETRETURN&&p->q1.reg==r){
    return 4;
  }else if(c==SETRETURN&&p->z.reg==r){
    return 4;
  }else if(c==CONVERT&&((p->typf&NQ)==CHAR||(p->typf2&NQ)==CHAR)&&regok(r,CHAR,0)){
    return 3;
  }
  if(o==&p->z&&r==acc){
    if(c==SUB||c==SUBIFP||c==SUBPFP||c==AND||c==OR||c==XOR)
      return 6;
    if((c==ADD||c==ADDI2P)&&!(p->q1.flags&(KONST|VKONST))&&!(p->q2.flags&(KONST|VKONST)))
      return 4;
    if(c==MULT) return 5;
    if(c==ASSIGN&&(p->q1.flags&KONST)){
      eval_const(&p->q1.val,p->typf);
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL)))
	return 3;
    }
  }
#if 1
  if((o==&p->q2/*||o==&p->z*/)&&!(o->flags&DREFOBJ)&&!ISACC(o->reg)&&(c==MULT||c==DIV||c==MOD))
    return INT_MIN;
#endif
  return 2;
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

/* Return name of library function, if this node should be
   implemented via libcall. */
char *use_libcall(int c,int t,int t2)
{
  static char fname[16];
  char *ret=0;

  if(c==COMPARE){
    if((t&NQ)==LONG||(t&NQ)==LLONG||ISFLOAT(t)){
      sprintf(fname,"__cmp%s%s%ld",(t&UNSIGNED)?"u":"s",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8);
      ret=fname;
    }
  }else{
    t&=NU;
    t2&=NU;
    if(t==LDOUBLE) t=DOUBLE;
    if(t2==LDOUBLE) t2=DOUBLE;
    if(c==CONVERT){
      if(t==t2) return 0;
      if(t==FLOAT&&t2==DOUBLE) return "__flt64toflt32";
      if(t==DOUBLE&&t2==FLOAT) return "__flt32toflt64";

      if(ISFLOAT(t)){
        sprintf(fname,"__%cint%ldtoflt%d",(t2&UNSIGNED)?'u':'s',zm2l(sizetab[t2&NQ])*8,(t==FLOAT)?32:64);
        ret=fname;
      }
      if(ISFLOAT(t2)){
        sprintf(fname,"__flt%dto%cint%ld",((t2&NU)==FLOAT)?32:64,(t&UNSIGNED)?'u':'s',zm2l(sizetab[t&NQ])*8);
        ret=fname;
      }
    }
    if((t&NQ)==LONG||(t&NQ)==LLONG||ISFLOAT(t)){
      if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==KOMPLEMENT||c==MINUS){
	if(t==(UNSIGNED|LLONG)&&(c==DIV||c==MOD||c==RSHIFT)){
	  sprintf(fname,"__%suint64",ename[c]);
	  ret=fname;
	}else if((t&NQ)==LLONG){
          sprintf(fname,"__%sint64",ename[c]);
          ret=fname;
	}else if(t==(UNSIGNED|LONG)&&(c==DIV||c==MOD||c==RSHIFT)){
	  sprintf(fname,"__%suint32",ename[c]);
	  ret=fname;
	}else if((t&NQ)==LONG){
          sprintf(fname,"__%sint32",ename[c]);
          ret=fname;
        }else{
	  sprintf(fname,"__%s%s%s%ld",ename[c],(t&UNSIGNED)?"u":"",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8);
          ret=fname;
	}
      }
    }
  }

  return ret;
}



int must_convert(int o,int t,int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
{
  int op=o&NQ,tp=t&NQ;
  if(op==tp) return 0;
  if(ISHWORD(op)&&ISHWORD(tp)) return 0;
  if(ISFLOAT(op)||ISFLOAT(tp)) return 1;
  if(ISLWORD(op)&&ISLWORD(tp)) return 0;
  return 1;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  if(newobj&&section!=SPECIAL)
    emit(f,"%ld\n",zm2l(size));
  else
    emit(f,"\t.space\t%ld\n",zm2l(size));
  newobj=0;  
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  /* nothing to do */
}

void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->storage_class==STATIC){
    if(ISFUNC(v->vtyp->flags)) return;
    if(!special_section(f,v)){
      if(v->clist&&(!constflag||(g_flags[3]&USEDFLAG))&&section!=DATA){
	emit(f,dataname);if(f) section=DATA;
      }
      if(v->clist&&constflag&&!(g_flags[3]&USEDFLAG)&&section!=RODATA){
	emit(f,rodataname);if(f) section=RODATA;
      }
      if(!v->clist&&section!=BSS){
	emit(f,bssname);if(f) section=BSS;
      }
    }
    emit(f,"\t.type\t%s%ld,@object\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,%ld\n",labprefix,zm2l(v->offset),zm2l(szof(v->vtyp)));
    if(v->clist||section==SPECIAL)
      emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    else
      emit(f,"\t.lcomm\t%s%ld,",labprefix,zm2l(v->offset));
    newobj=1; 
  }
  if(v->storage_class==EXTERN){
    if(v->flags&(DEFINED|TENTATIVE)){
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
      if(!special_section(f,v)){
	if(v->clist&&(!constflag||(g_flags[3]&USEDFLAG))&&section!=DATA){
	  emit(f,dataname);if(f) section=DATA;
	}
	if(v->clist&&constflag&&!(g_flags[3]&USEDFLAG)&&section!=RODATA){
	  emit(f,rodataname);if(f) section=RODATA;
	}
	if(!v->clist&&section!=BSS){
	  emit(f,bssname);if(f) section=BSS;
	}
      }
      emit(f,"\t.type\t%s%s,@object\n",idprefix,v->identifier);
      emit(f,"\t.size\t%s%s,%ld\n",idprefix,v->identifier,zm2l(szof(v->vtyp)));
      if(v->clist||section==SPECIAL)
        emit(f,"%s%s:\n",idprefix,v->identifier);
      else
        emit(f,"\t.global\t%s%s\n\t.lcomm\t%s%s,",idprefix,v->identifier,idprefix,v->identifier);
      newobj=1;   
    }
  }
}

void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  emit(f,"\t%s\t",dct[t&NQ]);
  if(!p->tree){
    if(ISFLOAT(t)){
      /*  auch wieder nicht sehr schoen und IEEE noetig   */
      unsigned char *ip;
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[3],ip[2],ip[1],ip[0]);
      if((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE){
	emit(f,",0x%02x%02x%02x%02x",ip[7],ip[6],ip[5],ip[4]);
      }
    }else{
      emitval(f,&p->val,(t&NU)|UNSIGNED);
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
  int c,t,lastcomp=0,reg,short_add,bit_reverse,need_return=0;
  struct obj *bit_obj;char *bit_reg;
  static int idone;
  struct obj o;
  struct IC *p2;
  if(v->tattr&INTERRUPT)
    ret="rti";
  else
    ret="rts"; /*FIXME: banked */
  if(DEBUG&1) printf("gen_code()\n");
  for(p2=p;p2;p2=p2->next) clear_ext_ic(&p2->ext);
  if(!g_flags[5]&USEDFLAG){
    peephole(p);
    if(!frame_used) offset=l2zm(0L);
  }
  for(c=1;c<=MAXR;c++) regs[c]=regsa[c];
  for(c=1;c<=MAXR;c++){
    if(regscratch[c]&&(regsa[c]||regused[c])){
      BSET(regs_modified,c);
    }
  }
  loff=zm2l(offset);
  function_top(f,v,loff);
  stackoffset=notpopped=dontpop=maxpushed=0;
  for(;p;pr(f,p),p=p->next){
    c=p->code;t=p->typf;
    if(debug_info)
      dwarf2_line_info(f,p); 
    short_add=0;
    if(c==NOP) continue;
    if(c==ALLOCREG){
      regs[p->q1.reg]=1;
      continue;
    }
    if(c==FREEREG){
      regs[p->q1.reg]=0;
      continue;
    }
    if(notpopped&&!dontpop){
      int flag=0;
      if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
	gen_pop(f,notpopped);
	notpopped=0;
      }
    }
    if(c==LABEL) {cc=0;emit(f,"%s%d:\n",labprefix,t);continue;}
    if(c>=BEQ&&c<=BGT&&t==exit_label) need_return=1;
    if(c==BRA){
      if(p->typf==exit_label&&!have_frame){
	emit(f,"\t%s\n",ret);
      }else{
	if(t==exit_label) need_return=1;
	emit(f,"\tbra\t%s%d\n",labprefix,t);
      }
      cc=0;continue;
    }
    if(c>=BEQ&&c<BRA){      
      if(t&UNSIGNED)
        emit(f,"\tb%s\t%s%d\n",uccs[c-BEQ],labprefix,t);
      else
        emit(f,"\tb%s\t%s%d\n",ccs[c-BEQ],labprefix,t);
      continue;
    }
    if(c==MOVETOREG){
      load_reg(f,p->z.reg,&p->q1,SHORT);
      continue;
    }
    if(c==MOVEFROMREG){
      store_reg(f,p->q1.reg,&p->z,SHORT);
      continue;
    }
    
    /*if(ISFLOAT(t)) {pric2(stdout,p);ierror(0);}*/

    if((t&NQ)==BIT){
      ierror(0);
    }

    if(c==TEST){
      lastcomp=t;
      p->code=c=COMPARE;
      gval.vmax=l2zm(0L);
      p->q2.flags=KONST;
      eval_const(&gval,MAXINT);
      insert_const(&p->q2.val,t);
    } 
    if(c==COMPARE&&isconst(q1)){
      struct IC *p2;
      o=p->q1;p->q1=p->q2;p->q2=o;
      p2=p->next;
      while(p2&&p2->code==FREEREG) p2=p2->next;
      if(!p2||p2->code<BEQ||p2->code>BGT) ierror(0);
      if(p2->code==BLT) p2->code=BGT;
      else if(p2->code==BGT) p2->code=BLT;
      else if(p2->code==BLE) p2->code=BGE;
      else if(p2->code==BGE) p2->code=BLE;
    }
    if(c==COMPARE&&isconst(q2)){
      eval_const(&p->q2.val,t);
      if(ISNULL()){
	if(cc&&(cc_t&NU)==(t&NU)&&compare_objects(cc,&p->q1)){
	  lastcomp=t;continue;
	}
      }
    }

    if(c==SUBPFP){
      ierror(0);
    }

    if(c==ADDI2P||c==SUBIFP){
      if((p->typf2&NQ)!=HPOINTER){
	p->typf=t=(UNSIGNED|SHORT);
	short_add=2;
      }else if(ISHWORD(t)){
	p->typf=t=(UNSIGNED|LONG);
	short_add=1;
      }
    }

    switch_IC(p);

    if(c==CONVERT){
      int to=p->typf2&NU;
      if(to==INT) to=SHORT;
      if(to==(UNSIGNED|INT)||to==NPOINTER) to=(UNSIGNED|SHORT);
      if(to==FPOINTER||to==HPOINTER) to=(UNSIGNED|LONG);
      if((t&NU)==INT) t=SHORT;
      if((t&NU)==(UNSIGNED|INT)||(t&NU)==NPOINTER) t=(UNSIGNED|SHORT);
      if((t&NQ)==FPOINTER||(t&NQ)==HPOINTER) t=(UNSIGNED|LONG);
      /*if((t&NQ)>=LONG||(to&NQ)>=LONG) ierror(0);*/
      if((to&NQ)<=LONG&&(t&NQ)<=LONG){
	if((to&NQ)<(t&NQ)){
	  if(!ISRACC(q1)&&!ISRACC(z))
	    get_acc(f,p);
	  load_reg(f,acc,&p->q1,to);
	  if(to&UNSIGNED)
	    emit(f,"\tclrb\n");
	  else
	    emit(f,"\tsex\tb,d\n");
	  store_reg(f,acc,&p->z,t);
	  cc=&p->z;cc_t=t;
	  continue;
	}else if((to&NQ)>(t&NQ)){
	  if(!ISRACC(q1)&&!ISRACC(z))
	    get_acc(f,p);
	  load_reg(f,acc,&p->q1,to);
	  store_reg(f,acc,&p->z,t);
	  continue;
	}else{
	  c=ASSIGN;
	  p->q2.val.vmax=sizetab[t&NQ];
	}
      }
    }
    if(c==KOMPLEMENT){
      cc=0;
      if(compare_objects(&p->q1,&p->z)&&!isreg(q1)&&(p->q1.flags&(REG|DREFOBJ))!=DREFOBJ&&(!p->q1.am||p->q1.am->flags!=ACC_IND)){
	emit(f,"\tcom\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	if(ISHWORD(t)){
	  mobj=p->z;
	  inc_addr(&mobj,1);
	  emit(f,"\tcom\t");
	  emit_obj(f,&mobj,t);
	  emit(f,"\n");
	}
	continue;
      }
      if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
	get_acc(f,p);
      load_reg(f,acc,&p->q1,t);
      emit(f,"\tcoma\n\tcomb\n");
      store_reg(f,acc,&p->z,t);
      continue;
    }
    if(c==MINUS){
      if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
	get_acc(f,p);
      if(isreg(q1)){
	load_reg(f,acc,&p->q1,t);
	emit(f,"\tcoma\n\tcomb\n\taddd\t#1\n");
      }else{
	emit(f,"\tclra\n\tclrb\n");
	emit(f,"\tsubd\t");
	emit_obj(f,&p->q1,t);
	emit(f,"\n");
      }
      cc=&p->z;cc_t=t;
      store_reg(f,acc,&p->z,t);
      continue;
    }
    if(c==SETRETURN){
      if(p->z.reg){
	load_reg(f,p->z.reg,&p->q1,t);
	BSET(regs_modified,p->z.reg);
      }
      continue;
    }
    if(c==GETRETURN){
      if(p->q1.reg){
	store_reg(f,p->q1.reg,&p->z,t);
      }
      continue;
    }
    if(c==CALL){
      int reg,jmp=0;
      cc=0;
      if(!calc_regs(p,f!=0)&&v->fi) v->fi->flags&=~ALL_REGS;
      if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
	emit_inline_asm(f,p->q1.v->fi->inline_asm);
	jmp=1;
      }else{
	if(stackoffset==0&&!have_frame&&!strcmp(ret,"rts")){
	  struct IC *p2;
	  jmp=1;
	  for(p2=p->next;p2;p2=p2->next){
	    if(p2->code!=FREEREG&&p2->code!=ALLOCREG&&p2->code!=LABEL){
	      jmp=0;break;
	    }
	  }
	}
	if(p->q1.flags&DREFOBJ){
	  /*FIXME: test this*/
	  if(jmp)
	    emit(f,"\tjmp\t");
	  else
	    emit(f,"\tjsr\t");
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	}else{
	  if(jmp){
	    emit(f,"\tbra\t");
	    if(!need_return) ret=0;
	  }else{
	    emit(f,"\tbsr\t");
	  }
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	}
      }
      if(stack_valid){
        int i;
        if(p->call_cnt<=0){
          err_ic=p;if(f) error(320);
          stack_valid=0;
        }
        for(i=0;stack_valid&&i<p->call_cnt;i++){
          if(p->call_list[i].v->fi&&(p->call_list[i].v->fi->flags&ALL_STACK)){
	    /*FIXME: size of return addr depends on mode */
	    if(!jmp) push(2);
	    callee_push(zm2l(p->call_list[i].v->fi->stack1));
	    if(!jmp) pop(2);
          }else{
            err_ic=p;if(f) error(317,p->call_list[i].v->identifier);
            stack_valid=0;
          }
        }
      }
      if(!zmeqto(l2zm(0L),p->q2.val.vmax)){
	notpopped+=zm2l(p->q2.val.vmax);
	dontpop-=zm2l(p->q2.val.vmax);
	if(!(g_flags[2]&USEDFLAG)&&stackoffset==-notpopped){
	  /*  Entfernen der Parameter verzoegern  */
	}else{
	  gen_pop(f,zm2l(p->q2.val.vmax));
	  notpopped-=zm2l(p->q2.val.vmax);
	}
      }
      continue;
    }
    if(c==ASSIGN||c==PUSH){
      if(c==PUSH) dontpop+=zm2l(p->q2.val.vmax);
      if(!zmleq(p->q2.val.vmax,l2zm(2L))){
	unsigned long size;int qr=0,zr=0,px=0,py=0,pd=0,lq=0,lz=0;
	size=zm2l(p->q2.val.vmax);
	if(c==ASSIGN){
	  if(!p->z.am&&(p->z.flags&(REG|DREFOBJ))&&ISIDX(p->z.reg))
	    zr=p->z.reg;lz=1;
	}
	if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))&&ISIDX(p->q1.reg)&&p->q1.reg!=zr){
	  qr=p->q1.reg;lq=1;
	}
	if(!qr){
	  if(zr==ix) qr=iy;
	  else if(zr==iy) qr=ix;
	  else{qr=ix;zr=iy;}
	}else if(!zr){
	  if(qr==ix) zr=iy; else zr=ix;
	}
	if(c==PUSH){
	  emit(f,"\tleas\t%ld,%s\n",-size,regnames[sp]);
	  push(size);
	}
	if(regs[ix]&&!scratchreg(ix,p)){emit(f,"\tpshx\n");push(2);px=1;}
	if(regs[iy]&&!scratchreg(iy,p)){emit(f,"\tpshy\n");push(2);py=1;}
	if(!lq) load_addr(f,qr,&p->q1);
	if(c==PUSH)
	  emit(f,"\ttfr\t%s,%s\n",regnames[sp],regnames[zr]);
	else
	  if(!lz) load_addr(f,zr,&p->z);
	if(size<=6||(size<=16&&!optsize)){
	  while(size>2){
	    emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    size-=2;
	  }
	  emit(f,"\tmov%c\t0,%s,0,%s\n",size==2?'w':'b',regnames[qr],regnames[zr]);
	}else{
	  int l=++label;
	  if(regs[acc]&&!scratchreg(acc,p)){emit(f,"\tpshd\n");push(2);pd=1;}
	  emit(f,"\tldd\t#%lu\n",optsize?size:size/8);
	  cc=0;
	  emit(f,"%s%d:\n",labprefix,l);
	  if(optsize){
	    emit(f,"\tmovb\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    emit(f,"\tdbne\td,%s%d\n",labprefix,l);
	  }else{
	    emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    emit(f,"\tdbne\td,%s%d\n",labprefix,l);
	    size=size&7;
	    while(size>=2){
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	      size-=2;
	    }
	    if(size)
	      emit(f,"\tmovb\t0,%s,0,%s\n",regnames[qr],regnames[zr]);
	  }
	}
	if(pd){emit(f,"\tpuld\n");pop(2);}
	if(py){emit(f,"\tpuly\n");pop(2);}
	if(px){emit(f,"\tpulx\n");pop(2);}
	continue;
      }
      if(!ISSCALAR(t)) t=zmeqto(p->q2.val.vmax,l2zm(0L))?CHAR:INT;
      if((t&NQ)==CHAR&&!zmeqto(p->q2.val.vmax,l2zm(1L))) t=INT;	
      if(mov_op(&p->q1)&&(c==PUSH||mov_op(&p->z))){
	emit(f,"\tmov%c\t",ISHWORD(t)?'w':'b');
	emit_obj(f,&p->q1,t);
	if(c==ASSIGN){
	  emit(f,",");
	  emit_obj(f,&p->z,t);
	  emit(f,"\n");
	}else{
	  emit(f,",%d,-%s\n",ISHWORD(t)?2:1,regnames[sp]);
	  push(ISHWORD(t)?2:1);
	}
	continue;
      }
      if((p->q1.flags&KONST)&&!isreg(z)){
	eval_const(&p->q1.val,t);
	if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&((p->z.flags&(REG|DREFOBJ))!=DREFOBJ||(t&NQ)==CHAR)&&(!p->z.am||p->z.am->flags!=ACC_IND||(t&NQ)==CHAR)){
	  emit(f,"\tclr\t");
	  if(c==ASSIGN){
	    emit_obj(f,&p->z,t);emit(f,"\n");
	  }else
	    emit(f,"1,-sp\n");
	  if(!ISHWORD(t)) continue;
	  emit(f,"\tclr\t");
	  if(c==ASSIGN){
	    mobj=p->z;
	    inc_addr(&mobj,1);
	    emit_obj(f,&mobj,t);emit(f,"\n");
	  }else
	    emit(f,"1,-sp\n");
	  continue;
	}

      }
      if(c==PUSH){
	if(isreg(q1)){
	  reg=p->q1.reg;
	}else{
	  reg=get_reg(f,p,t);
	  load_reg(f,reg,&p->q1,t);
	}
	if((t&NQ)==CHAR)
	  emit(f,"\tpshb\n");
	else if(reg==ix)
	  emit(f,"\tpshx\n");
	else if(reg==iy)
	  emit(f,"\tpshy\n");
	else
	  emit(f,"\tpshd\n");
	push(zm2l(p->q2.val.vmax));
	continue;
      }
      if(c==ASSIGN){
	if(isreg(q1)&&isreg(z)){
	  if(p->q1.reg!=p->z.reg)
	    emit(f,"\ttfr\t%s,%s\n",regnames[p->q1.reg],regnames[p->z.reg]);
	}else if(isreg(q1)){
	  store_reg(f,p->q1.reg,&p->z,t);
	}else if(isreg(z)){
	  load_reg(f,p->z.reg,&p->q1,t);
	}else{
	  reg=get_reg(f,p,t);
	  load_reg(f,reg,&p->q1,t);
	  store_reg(f,reg,&p->z,t);
	}
	continue;
      }
      ierror(0);
    }
    if(c==ADDRESS){
      int px=0;
      if(isreg(z)){
	reg=p->z.reg;
      }else if(!regs[ix]){
	reg=ix;
      }else if(!regs[iy]){
	reg=iy;
      }else{
	/*FIXME: test if x used in q1 */
	px=1;
	emit(f,"\tpshx\n");
	reg=ix;
	push(2);
      }
      load_addr(f,reg,&p->q1);
      if(!(p->z.flags&REG)||p->z.reg!=reg)
	store_reg(f,reg,&p->z,p->typf2);
      if(px){
	emit(f,"\tpulx\n");
	pop(2);
      }
      continue;
    }

    if((c==MULT||c==DIV||(c==MOD&&(p->typf&UNSIGNED)))&&isconst(q2)){
      long ln;
      eval_const(&p->q2.val,t);
      if(zmleq(l2zm(0L),vmax)&&zumleq(ul2zum(0UL),vumax)){
	if((ln=pof2(vumax))&&ln<5){
	  if(c==MOD){
	    vmax=zmsub(vmax,l2zm(1L));
	    c=p->code=c=AND;
	  }else{
	    vmax=l2zm(ln-1);
	    if(c==DIV) p->code=c=RSHIFT; else p->code=c=LSHIFT;
	  }
	  c=p->code;
	  gval.vmax=vmax;
	  eval_const(&gval,MAXINT);
	  if(c==AND){
	    insert_const(&p->q2.val,t);
	  }else{
	    insert_const(&p->q2.val,t);
	    p->typf2=INT;
	  }
	}
      }
    }
    if(c==MOD||c==DIV){
      ierror(0);
      continue;
    }
    if((c>=LSHIFT&&c<=MOD)||c==ADDI2P||c==SUBIFP||c==SUBPFP||(c>=OR&&c<=AND)||c==COMPARE){
      char *s;
      /*FIXME: nicht immer besser*/
      if(ISLWORD(t)&&c==LSHIFT&&isconst(q2)){
	eval_const(&p->q2.val,t);
	if(zm2l(vmax)==1){
	  p->code=c=ADD;
	  p->q2=p->q1;
	}
      }
      if((c==ADD||c==ADDI2P||c==MULT||(c>=OR&&c<=AND))&&isreg(q2)&&!isreg(q1)){
	o=p->q1;p->q1=p->q2;p->q2=o;
      }
      if((c==ADD||c==MULT||(c>=OR&&c<=AND))&&isreg(q2)&&p->q2.reg==acc){
	o=p->q1;p->q1=p->q2;p->q2=o;
      }
      if(c==MULT||c==MOD){
	if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
	  get_acc(f,p);
	reg=acc;
	/*FIXME: y bzw. x-Register*/
      }else if(c==LSHIFT||c==RSHIFT||c==AND||c==OR||c==XOR){
	if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
	  get_acc(f,p);
	reg=acc;
      }else if(c==DIV){
	reg=ix;
	ierror(0);
      }else if(isreg(z)){
	reg=p->z.reg;
      }else if(isreg(q1)&&(c==COMPARE||scratchreg(p->q1.reg,p))){
	reg=p->q1.reg;
      }else{
	if(c==ADD||c==SUB||c==ADDI2P||c==SUBIFP||c==COMPARE){
	  /*FIXME??*/
	  reg=get_reg(f,p,t);
	}else{
	  get_acc(f,p);
	  reg=acc;
	}
      }
      if(c==ADD||c==ADDI2P||c==SUB||c==SUBIFP){
	int opdone=0;
	if(isreg(q1)){
	  if(ISIDX(reg)&&ISIDX(p->q1.reg)&&isconst(q2)){
	    eval_const(&p->q2.val,q2typ(p));
	    if(p->q1.reg==reg&&zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))){
	      emit(f,"\t%s%s\n",c==SUB?"de":"in",regnames[reg]);
	      /*FIXME: condition-codes for bne/beq could be used */
	    }else{
	      emit(f,"\tlea%s\t%s%ld,%s\n",regnames[reg],(c==SUB?"-":""),zm2l(vmax),regnames[p->q1.reg]);
	    }
	    opdone=1;
	  }else	if((c==ADD||c==ADDI2P)&&ISIDX(reg)&&ISIDX(p->q1.reg)&&ISRACC(q2)){
	    emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[p->q1.reg]);
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&ISIDX(reg)&&ISACC(p->q1.reg)&&ISRIDX(q2)){
	    emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[p->q2.reg]);
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&p->q1.reg==acc&&ISIDX(reg)){
	    load_reg(f,reg,&p->q2,t);
	    emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[reg]);
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&ISACC(p->q1.reg)&&ISRACC(z)&&isreg(q2)&&ISIDX(p->q2.reg)){
	    if(!scratchreg(p->q2.reg,p)) emit(f,"\texg\t%s,%s\n",regnames[acc],regnames[p->q2.reg]);
	    emit(f,"\tlea%s\t%s,%s\n",regnames[p->q2.reg],regnames[acc],regnames[p->q2.reg]);
	    emit(f,"\texg\t%s,%s\n",regnames[acc],regnames[p->q2.reg]);
	    opdone=1;
	  }else	if(p->q1.reg!=reg)
	    emit(f,"\ttfr\t%s,%s\n",regnames[p->q1.reg],regnames[reg]);
	}else{
	  load_reg(f,reg,&p->q1,q1typ(p));
	}
	if(!opdone){
	  if(reg==acc){
	    if(ISRACC(q2)){
	      if(c==ADD||c==ADDI2P){
		emit(f,"\tasld\n");
	      }else{
		emit(f,"\tclrb\n");
		if((ztyp(p)&NQ)==CHAR) emit(f,"\tclra\n");
	      }
	    }else{
	      if(ISRIDX(q2)){
		emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
		push(2);pop(2);
		emit(f,"\t%sd\t2,%s+\n",(c==ADD||c==ADDI2P)?"add":"sub",regnames[sp]);
	      }else{
		emit(f,"\t%s%s\t",(c==ADD||c==ADDI2P)?"add":"sub",(t&NQ)==CHAR?"b":"d");
		emit_obj(f,&p->q2,t);emit(f,"\n");
	      }
	    }
	    cc=&p->z;cc_t=t;
	  }else{
	    if(isconst(q2)){
	      long l;
	      eval_const(&p->q2.val,t);
	      l=zm2l(vmax);
	      if(c==SUB) l=-l;
	      /*FIXME: condition-codes for bne/beq could be used */
	      if(l==1&&reg==ix){
		emit(f,"\tinx\n");
	      }else if(l==1&&reg==iy){
		emit(f,"\tiny\n");
	      }else if(l==-1&&reg==ix){
		emit(f,"\tdex\n");
	      }else if(l==-1&&reg==iy){
		emit(f,"\tdey\n");
	      }else{
		emit(f,"\tlea%s\t%ld,%s\n",regnames[reg],l,regnames[reg]);
	      }
	    }else{
	      if(c!=ADD&&c!=ADDI2P){
		get_acc(f,p);
		emit(f,"\tclrb\n");
		if((t&NQ)!=CHAR) emit(f,"\tclra\n");
		emit(f,"\tsub%s\t",(q2typ(p)&NQ)==CHAR?"b":"d");
		emit_obj(f,&p->q2,t);
		emit(f,"\n");
	      }else if(!ISRACC(q2)){
		get_acc(f,p);
		load_reg(f,acc,&p->q2,t);
	      }
	      emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[reg]);
	    }
	  }
	}
	store_reg(f,reg,&p->z,ztyp(p));
	continue;
      }
      load_reg(f,reg,&p->q1,t);
      if(c==MULT){
	int py=0;
	if(reg!=acc) ierror(reg);
	if(!ISRY(q2)&&regs[iy]){
	  emit(f,"\tpshy\n");
	  push(2);
	  py=1;
	}
	load_reg(f,iy,&p->q2,t);
	emit(f,"\temul\n");
	if(py){
	  emit(f,"\tpuly\n");
	  pop(2);
	}
	store_reg(f,acc,&p->z,t);
	continue;
      }
      if(c==LSHIFT||c==RSHIFT){
	if(isconst(q2)){
	  int l,oldl;
	  eval_const(&p->q2.val,t);
	  oldl=l=zm2l(vmax);
	  if(l>=16){ emit(f,"\tclra\n\tclrb\n");l=0;}
	  if(l>=8){
	    if(c==LSHIFT)
	      emit(f,"\ttba\n\tclrb\n");
	    else{
	      if(t&UNSIGNED)
		emit(f,"\ttab\n\tclra\n");
	      else
		emit(f,"\ttab\n\tsex\tb,d\n");
	    }
	    l-=8;
	  }
	  while(l--){
	    if(c==RSHIFT){
	      if(t&UNSIGNED)
		emit(f,"\tlsrd\n");
	      else{
		if(oldl>8)
		  emit(f,"\tasrb\n");
		else
		  emit(f,"\tasra\n\trorb\n");
	      }
	    }else{
	      emit(f,"\tasld\n");
	    }
	  }
	}else{
	  int px;char *s;
	  if(regs[ix]&&!scratchreg(ix,p)&&(!isreg(z)||p->z.reg!=ix)){
	    emit(f,"\tpsh%s\n",regnames[ix]);
	    push(2);px=1;
	  }else
	    px=0;
	  if(c==LSHIFT) s="lsl";
	  else if(t&UNSIGNED) s="lsr";
	  else s="asr";
	  load_reg(f,ix,&p->q2,t);
	  emit(f,"\t.global\t%s__%s\n",idprefix,s);
	  emit(f,"\tbsr\t%s__%s\n",idprefix,s);
	  if(px){
	    emit(f,"\tpul%s\n",regnames[ix]);
	    pop(2);
	  }
	}
	cc=0;
	store_reg(f,acc,&p->z,t);
	continue;
      }
      if(c>=OR&&c<=AND){
	s=logicals[c-OR];
	if(p->q2.am&&p->q2.am->flags==ACC_IND){
	  mobj=p->q1;p->q1=p->q2;p->q2=mobj;
	}
	if(p->q2.flags&KONST){
	  unsigned long l,h;
	  eval_const(&p->q2.val,t);
	  l=zum2ul(vumax);
	  h=(l>>8)&255;
	  if(c==AND&&h==0)
	    emit(f,"\tclra\n");
	  else if(c==XOR&&h==255)
	    emit(f,"\tcoma\n");
	  else if((c==AND&&h!=255)||(c==OR&&h!=0)||(c==XOR&&h!=0))
	    emit(f,"\t%sa\t#%lu\n",s,h);
	  if((t&NQ)!=CHAR){
	    h=l&255;
	    if(c==AND&&h==0)
	      emit(f,"\tclrb\n");
	    else if(c==XOR&&h==255)
	      emit(f,"\tcomb\n");
	    else if((c==AND&&h!=255)||(c==OR&&h!=0)||(c==XOR&&h!=0))
	      emit(f,"\t%sb\t#%lu\n",s,h);
	  }
	}else{
	  if(isreg(q2)){
	    if(p->q2.reg==acc){
	      if(c==XOR){
		emit(f,"\tclra\n");
		if((t&NQ)!=CHAR) emit(f,"\tclrb\n");
	      }
	    }else{
	      if((t&NQ)==CHAR){
		emit(f,"\tpsha\n");
		push(1);
		emit(f,"\t%sa\t1,%s+\n",s,regnames[sp]);
		pop(1);
	      }else{
		emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
		push(2);
		emit(f,"\t%sa\t1,%s+\n",s,regnames[sp]);
		emit(f,"\t%sb\t1,%s+\n",s,regnames[sp]);
		pop(2);
	      }
	    }
	  }else if((p->q2.flags&(REG|DREFOBJ))==DREFOBJ){
	    int xr=0;
	    if(!regs[ix]) xr=ix;
	    else if(!regs[iy]) xr=iy;
	    else{
	      xr=ix;
	      emit(f,"\tpsh%s\n",regnames[xr]);
	      push(2);

	    }
	    BSET(regs_modified,xr);
	    load_addr(f,xr,&p->q2);
	    emit(f,"\t%sa\t0,%s\n",s,regnames[xr]);
	    emit(f,"\t%sb\t1,%s\n",s,regnames[xr]);
	    pop(2);
	  }else{
	    emit(f,"\t%sa\t",s);emit_obj(f,&p->q2,t);
	    emit(f,"\n");
	    if((t&NQ)!=CHAR){
	      mobj=p->q2;
	      inc_addr(&mobj,1);
	      emit(f,"\t%sb\t",s);emit_obj(f,&mobj,t);
	      emit(f,"\n");
	    }
	  }
	}
	cc=0;
	store_reg(f,reg,&p->z,t);
	continue;
      }else if(c==COMPARE){
	lastcomp=t;
	if(isreg(q2)){
	  emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
	  push(2);
	}
	if(reg==acc){
	  if((t&NQ)==CHAR)
	    emit(f,"\tcmpb\t");
	  else
	    emit(f,"\tcpd\t");
	}else if(reg==ix){
	  emit(f,"\tcpx\t");
	}else if(reg==iy){
	  emit(f,"\tcpy\t");
	}else
	  ierror(0);
	if(isreg(q2)){
	  emit(f,"2,%s+\n",regnames[sp]);
	  pop(2);
	}else{
	  emit_obj(f,&p->q2,t);emit(f,"\n");
	}
	continue;
      }
      ierror(0);
    }
    pric2(stdout,p);
    ierror(0);
  }
  if(notpopped){
    gen_pop(f,notpopped);
    notpopped=0;
  }
  function_bottom(f,v,loff);
  if(debug_info){
    emit(f,"%s%d:\n",labprefix,++label);
    dwarf2_function(f,v,label);
    if(f) section=-1;
  }     
}

int shortcut(int c,int t)
{
  if(c==COMPARE||c==ADD||c==SUB||c==AND||c==OR||c==XOR) return 1;
  return 0;
}

void cleanup_cg(FILE *f)
{
  struct fpconstlist *p;
  unsigned char *ip;
  if(f&&stack_check)
    emit(f,"\t.global\t%s__stack_check\n",idprefix);
  while(p=firstfpc){
    if(f){
      if(section!=RODATA){
	emit(f,rodataname);if(f) section=RODATA;
      }
      emit(f,"%s%d\n\t%s\t",labprefix,p->label,dct[LONG]);
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
      if((p->typ&NQ)==DOUBLE||(p->typ&NQ)==LDOUBLE){
	emit(f,",0x%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
      }
      emit(f,"\n");
    }
    firstfpc=p->next;
    free(p);
  }
}

int reg_parm(struct reg_handle *p,struct Typ *t,int mode,struct Typ *fkt)
{
  if(p->gpr) return 0;
  if(ISSCALAR(t->flags)&&!ISFLOAT(t->flags)&&!ISLWORD(t->flags)){
    p->gpr=1;
    return acc;
  }
  return 0;
}

void insert_const(union atyps *p,int t)
/*  Traegt Konstante in entprechendes Feld ein.       */
{
  if(!p) ierror(0);
  t&=NU;
  if(t==BIT) {if(zmeqto(zc2zm(vchar),l2zm(0L))) p->vchar=zm2zc(l2zm(0L)); else p->vchar=zm2zc(l2zm(1L));return;}
  if(t==CHAR) {p->vchar=vchar;return;}
  if(t==SHORT) {p->vshort=vshort;return;}
  if(t==INT) {p->vint=vint;return;}
  if(t==LONG) {p->vlong=vlong;return;}
  if(t==LLONG) {p->vllong=vllong;return;}
  if(t==MAXINT) {p->vmax=vmax;return;}
  if(t==(UNSIGNED|BIT)) {if(zumeqto(zuc2zum(vuchar),ul2zum(0UL))) p->vuchar=zum2zuc(ul2zum(0UL)); else p->vuchar=zum2zuc(ul2zum(1UL));return;}  
  if(t==(UNSIGNED|CHAR)) {p->vuchar=vuchar;return;}
  if(t==(UNSIGNED|SHORT)) {p->vushort=vushort;return;}
  if(t==(UNSIGNED|INT)) {p->vuint=vuint;return;}
  if(t==(UNSIGNED|LONG)) {p->vulong=vulong;return;}
  if(t==(UNSIGNED|LLONG)) {p->vullong=vullong;return;}
  if(t==(UNSIGNED|MAXINT)) {p->vumax=vumax;return;}
  if(t==FLOAT) {p->vfloat=vfloat;return;}
  if(t==DOUBLE) {p->vdouble=vdouble;return;}
  if(t==LDOUBLE) {p->vldouble=vldouble;return;}
  if(t==NPOINTER) {p->vuint=vuint;return;}
  if(t==FPOINTER||t==HPOINTER) {p->vulong=vulong;return;}
}
void eval_const(union atyps *p,int t)
/*  Weist bestimmten globalen Variablen Wert einer CEXPR zu.       */
{
  int f=t&NQ;
  if(!p) ierror(0);
  if(f==MAXINT||(f>=BIT&&f<=LLONG)){
    if(!(t&UNSIGNED)){
      if(f==BIT){
	if(zmeqto(zc2zm(p->vchar),l2zm(0L))) vmax=l2zm(0L); else vmax=l2zm(1L);
      }else if(f==CHAR) vmax=zc2zm(p->vchar);
      else if(f==SHORT)vmax=zs2zm(p->vshort);
      else if(f==INT)  vmax=zi2zm(p->vint);
      else if(f==LONG) vmax=zl2zm(p->vlong);
      else if(f==LLONG) vmax=zll2zm(p->vllong);
      else if(f==MAXINT) vmax=p->vmax;
      else ierror(0);
      vumax=zm2zum(vmax);
      vldouble=zm2zld(vmax);
    }else{
      if(f==BIT){
	if(zumeqto(zuc2zum(p->vuchar),ul2zum(0UL))) vumax=ul2zum(0UL); else vumax=ul2zum(1UL);
      }else if(f==CHAR) vumax=zuc2zum(p->vuchar);
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
      if(f==NPOINTER)
	vumax=zui2zum(p->vuint);
      else
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
  if(t==BIT){vmax=zc2zm(p->vchar);fprintf(f,"B%d",!zmeqto(vmax,l2zm(0L)));}
  if(t==(UNSIGNED|BIT)){vumax=zuc2zum(p->vuchar);fprintf(f,"UB%d",!zumeqto(vmax,ul2zum(0UL)));}
  if(t==CHAR){vmax=zc2zm(p->vchar);printzm(f,vmax);}
  if(t==(UNSIGNED|CHAR)){fprintf(f,"UC");vumax=zuc2zum(p->vuchar);printzum(f,vumax);}
  if(t==SHORT){fprintf(f,"S");vmax=zs2zm(p->vshort);printzm(f,vmax);}
  if(t==(UNSIGNED|SHORT)){fprintf(f,"US");vumax=zus2zum(p->vushort);printzum(f,vumax);}
  if(t==FLOAT){fprintf(f,"F");vldouble=zf2zld(p->vfloat);printzld(f,vldouble);}
  if(t==DOUBLE){fprintf(f,"D");vldouble=zd2zld(p->vdouble);printzld(f,vldouble);}
  if(t==LDOUBLE){fprintf(f,"LD");printzld(f,p->vldouble);}
  if(t==INT){fprintf(f,"I");vmax=zi2zm(p->vint);printzm(f,vmax);}
  if(t==(UNSIGNED|INT)||t==NPOINTER){fprintf(f,"UI");vumax=zui2zum(p->vuint);printzum(f,vumax);}
  if(t==LONG){fprintf(f,"L");vmax=zl2zm(p->vlong);printzm(f,vmax);}
  if(t==(UNSIGNED|LONG)||t==FPOINTER||t==HPOINTER){fprintf(f,"UL");vumax=zul2zum(p->vulong);printzum(f,vumax);}
  if(t==LLONG){fprintf(f,"LL");vmax=zll2zm(p->vllong);printzm(f,vmax);}
  if(t==(UNSIGNED|LLONG)){fprintf(f,"ULL");vumax=zull2zum(p->vullong);printzum(f,vumax);}
  if(t==MAXINT) printzm(f,p->vmax);
  if(t==(UNSIGNED|MAXINT)) printzum(f,p->vumax);
} 
void emitval(FILE *f,union atyps *p,int t)
{
  t&=NU;
  if(t==BIT){vmax=zc2zm(p->vchar);emit(f,"%d",!zmeqto(vmax,l2zm(0L)));}
  if(t==(UNSIGNED|BIT)){vumax=zuc2zum(p->vuchar);emit(f,"%d",!zumeqto(vmax,ul2zum(0UL)));}
  if(t==CHAR){vmax=zc2zm(p->vchar);emitzm(f,vmax);}
  if(t==(UNSIGNED|CHAR)){vumax=zuc2zum(p->vuchar);emitzum(f,vumax);}
  if(t==SHORT){vmax=zs2zm(p->vshort);emitzm(f,vmax);}
  if(t==(UNSIGNED|SHORT)){vumax=zus2zum(p->vushort);emitzum(f,vumax);}
  if(t==FLOAT){vldouble=zf2zld(p->vfloat);emitzld(f,vldouble);}
  if(t==DOUBLE){vldouble=zd2zld(p->vdouble);emitzld(f,vldouble);}
  if(t==LDOUBLE){emitzld(f,p->vldouble);}
  if(t==INT){vmax=zi2zm(p->vint);emitzm(f,vmax);}
  if(t==(UNSIGNED|INT)||t==NPOINTER){vumax=zui2zum(p->vuint);emitzum(f,vumax);}
  if(t==LONG){vmax=zl2zm(p->vlong);emitzm(f,vmax);}
  if(t==(UNSIGNED|LONG)||t==FPOINTER||t==HPOINTER){vumax=zul2zum(p->vulong);emitzum(f,vumax);}
  if(t==LLONG){vmax=zll2zm(p->vllong);emitzm(f,vmax);}
  if(t==(UNSIGNED|LLONG)){vumax=zull2zum(p->vullong);emitzum(f,vumax);}
  if(t==MAXINT) emitzm(f,p->vmax);
  if(t==(UNSIGNED|MAXINT)) emitzum(f,p->vumax);
}

void conv_typ(struct Typ *p)
/* Erzeugt extended types in einem Typ. */
{
  char *attr;
  while(p){
    if(ISPOINTER(p->flags)){
      p->flags=((p->flags&~NU)|POINTER_TYPE(p->next));
      if(attr=p->next->attr){
	if(strstr(attr,STR_NEAR))
	  p->flags=((p->flags&~NU)|NPOINTER);
	if(strstr(attr,STR_FAR))
	  p->flags=((p->flags&~NU)|FPOINTER);
	if(strstr(attr,STR_HUGE))
	  p->flags=((p->flags&~NU)|HPOINTER);
      }
    }
    if(ISINT(p->flags)&&(attr=p->attr)&&strstr(attr,"bit"))
      p->flags=((p->flags&~NU)|BIT);
    p=p->next;
  }
}

void init_db(FILE *f)
{
  dwarf2_setup(sizetab[HPOINTER],".byte",".2byte",".4byte",".4byte",labprefix,idprefix,".section");
  dwarf2_print_comp_unit_header(f);
}
void cleanup_db(FILE *f)
{
  dwarf2_cleanup(f);
  if(f) section=-1;
} 

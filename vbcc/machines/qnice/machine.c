/*  Code generator for qnice cpu.               */

#include "supp.h"

static char FILE_[]=__FILE__;
/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for qnice V0.1 (c) in 2016 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,0,0,0,
                    0,0,0,0,
                    0,VALFLAG};
char *g_flags_name[MAXGF]={"cpu","int32","no-delayed-popping","const-in-data",
                           "merge-constants","no-peephole","mtiny","mlarge",
                           "mhuge","rw-threshold","soft-mul"};
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
char *regnames[]={"noreg","R0","R1","R2","R3","R4","R5","R6","R7",
		  "R8","R9","R10","R11","R12","R13","R14","R15",
		  "R0/R1","R2/R3","R4/R5","R6/R7",
		  "R8/R9","R11/R12"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1};

int reg_prio[MAXR+1]={0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1};

struct reg_handle empty_reg_handle={0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__interrupt","__rbank","__norbank",0};
#define INTERRUPT 1
#define RBANK 2
#define NORBANK 4


/****************************************/
/*  Some private data and functions.    */
/****************************************/

static long malign[MAX_TYPE+1]=  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static long msizetab[MAX_TYPE+1]={0,1,1,1,1,2,4,2,4,4,0,1,2,2,0,0,0,1,0,1,1};

struct Typ ityp={SHORT},ltyp={LONG};

#define INT32 (g_flags[1]&USEDFLAG)
#define NOPEEP  (g_flags[5]&USEDFLAG)
#define TINY  1 /*(g_flags[6]&USEDFLAG)*/
#define LARGE 0 /*(g_flags[7]&USEDFLAG)*/
#define HUGE  0 /*(g_flags[8]&USEDFLAG)*/
#define SOFTMUL  (g_flags[10]&USEDFLAG)


#define NDATA 0
#define NBSS 1
#define NCDATA 2
#define FDATA 3
#define FBSS 4
#define FCDATA 5
#define HDATA 6
#define HBSS 7
#define HCDATA 8
#define CODE 9
#define SPECIAL 10
#define BITS 11

static int section=-1,newobj,scnt;
static char *codename="\t.text\n",
  *ndataname="\t.data\n",
  *fdataname="\t.data\n",
  *hdataname="\t.data\n",
  *nbssname="\t.bss\n",
  *fbssname="\t.bss\n",
  *hbssname="\t.bss\n",
  *ncdataname="\t.text\n",
  *fcdataname="\t.text\n",
  *hcdataname="\t.text\n";

#define POST_INC 1
#define PRE_DEC  2

extern int static_cse, dref_cse;

/* (user)stack-pointer, pointer-tmp, int-tmp; reserved for compiler */
static const int sp=14,sr=15,pc=16,t1=12,t2=13,MTMP1=22;
static const int r8=9,r9=10,r8r9=21;
static int tmp1,tmp2,tmp3,tmp4,t2_used;
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);
static int scratchreg(int,struct IC *);
static struct Var nvar,fvar;
static int load_const;
static struct IC *icp;

static char *marray[]={0,
                       "__far=__attr(\"far\")",
		       "__near=__attr(\"near\")",
		       "__huge=__attr(\"huge\")",
                       "__section(x,y)=__vattr(\"section(\"#x\",\"#y\")\")",
		       "__interrupt(x)=__interrupt __vattr(\"ivec(\"__str(x)\")\")",
		       "__QNICE__",
		       "__SIZE_T_INT=1",
		       "__str(x)=#x",
		       0};

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

static long loff,stack,stackoffset,notpopped,dontpop,maxpushed;

static char *ccs[]={"z","!z","v","!v","??","??"};
static char *ccu[]={"z","!z","n","!n","??","??"};

static char *logicals[]={"or","xor","and"};
static char *arithmetics[]={"shl","shr","add","sub"};

static char *dct[]={"",".bit",".short",".short",".short",".short",".short",".short",".short",".short",
		    "(void)",".short",".long",".long"};
static int pushedsize,pushorder=2;
static char *idprefix="_",*labprefix="l";
static int exit_label,have_frame,stackchecklabel,stack_valid;
static char *ret,*call,*jump;
static int frame_used;
static int rwthreshold;

#define STR_NEAR "near"
#define STR_FAR "far"
#define STR_HUGE "huge"

#define ISNULL() (zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
#define ISLWORD(t) ((t&NQ)==LONG||(t&NQ)==FPOINTER||(t&NQ)==HPOINTER||(t&NQ)==FLOAT)
#define ISHWORD(t) ((t&NQ)==INT||(t&NQ)==SHORT||(t&NQ)==CHAR||(t&NQ)==NPOINTER)
#define ISSTATIC(v) ((v)->storage_class==EXTERN||(v)->storage_class==STATIC)

static int ISFAR(struct Var *v)
{
  struct Typ *vt;
  if(v==&nvar) return 0;
  if(v==&fvar) return 1;
  vt=v->vtyp;
  while(ISARRAY(vt->flags)) vt=vt->next;
  if(vt->attr&&strstr(STR_NEAR,vt->attr))
    return 0;
  if(HUGE||LARGE)
    return 1;
  if(vt->attr&&(strstr(STR_FAR,vt->attr)||strstr(STR_HUGE,vt->attr)))
    return 1;
  return 0;
}

static int special_section(FILE *f,struct Var *v)
{
  char *sec,*e;
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
static void callee_push(long push)
{
  if(push-stackoffset>stack)
    stack=push-stackoffset;
}
static void push(long push)
{
  stackoffset-=push;
  if(stackoffset<maxpushed) 
    maxpushed=stackoffset;
  if(-maxpushed>stack) stack=-maxpushed;
}
static void pop(long pop)
{
  stackoffset+=pop;
}
int pointer_type(struct Typ *p)
{
  struct Typ *merk=p;
  if(!p) ierror(0);
  while(ISARRAY(p->flags)||ISFUNC(p->flags)) p=p->next;
  if(p->attr){
    if(strstr(p->attr,STR_HUGE)) return HPOINTER;
    if(strstr(p->attr,STR_FAR)) return FPOINTER;
    if(strstr(p->attr,STR_NEAR)) return NPOINTER;
  }
  if((merk->flags&NQ)==FUNKT){
    if(TINY)
      return NPOINTER;
    else
      return HPOINTER;
  }
  if(LARGE)
    return FPOINTER;
  else if(HUGE)
    return HPOINTER;
  else
    return NPOINTER;
}

/* return non-zero if IC is implemented by a function call */
static int islibcall(struct IC *p)
{
  /* TODO: check if necessary */
  return 0;
}

static long voff(struct obj *p)
{
  if(p->v->offset<0)
    return loff-zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset+pushedsize;
  return zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset;
}
static int alignment(struct obj *o)
{
  if(o->flags&DREFOBJ) return 1;
  if(!(o->flags&VAR)) ierror(0);
  if(ISSTATIC(o->v)) return zm2l(o->val.vmax)&1;
  return voff(o)&1;
}

static void emit_obj(FILE *f,struct obj *p,int t)
/*  output an operand                      */
{
  if(p->am){
    if(p->am->flags==POST_INC){
      emit(f,"@%s++",regnames[p->am->base]);
      return;
    }else if(p->am->flags==PRE_DEC){
      emit(f,"@--%s",regnames[p->am->base]);
      return;
    }else{
      ierror(0);
    }
  }
  if((p->flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
    emitval(f,&p->val,p->dtyp&NU);
    return;
  }
  if((p->flags&(DREFOBJ|REG))==(DREFOBJ|REG)) emit(f,"@");
  /*if(p->flags&VARADR) ierror(0);*/
  if((p->flags&(VAR|REG))==VAR){
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      if(voff(p))
	emit(f,"@%s+#%ld",regnames[sp],voff(p));
      else emit(f,"@%s",regnames[sp]);
    }else{
      if(p->v->storage_class==STATIC){
	emit(f,"#%s%ld",labprefix,zm2l(p->v->offset));
      }else{
	emit(f,"#%s%s",idprefix,p->v->identifier);
      }
      if(!zmeqto(l2zm(0L),p->val.vmax))
	emit(f,"+%ld",(long)zm2l(p->val.vmax)*2);
    }
  }
  if(p->flags&REG){
    emit(f,"%s",regnames[p->reg]);
  }
  /* sometimes we just or a REG into a KONST */
  if((p->flags&(KONST|REG))==KONST){
    if(ISFLOAT(t)){
      ierror(0);
      emit(f,"#%s%d",labprefix,addfpconst(p,t));
    }else{
      emitval(f,&p->val,t&NU);
    }
  }
}

static void emit_lword(FILE *f,struct obj *p,int t,char *def)
/*  output low-word of an operand */
{
  if((p->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->val,t);
    if(ISFLOAT(t)){
      unsigned char *ip=(unsigned char *)&vfloat;
      emit(f,"0x%02x%02x",ip[1],ip[0]);
    }else{
      long v;
      v=zm2l(vmax);
      emit(f,"%ld",v&0xffff);
    }
    return;
  }
  if((p->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(p->reg,&rp))
      ierror(0);
    emit(f,"%s",regnames[rp.r1]);
    return;
  }
  if((p->flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
    emit(f,def,regnames[p->reg]);
    return;
  }
  ierror(0);
}

static void emit_hword(FILE *f,struct obj *p,int t,char *def)
/*  output high-word of an operand */
{
  if((p->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->val,t);
    if(ISFLOAT(t)){
      unsigned char *ip=(unsigned char *)&vfloat;
      emit(f,"0x%02x%02x",ip[3],ip[2]);
    }else{
      long v;
      v=zm2l(vmax);
      emit(f,"%ld",(v>>16)&0xffff);
    }
    return;
  }
  if((p->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(p->reg,&rp))
      ierror(0);
    emit(f,"%s",regnames[rp.r2]);
    return;
  }
  if((p->flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
    emit(f,def,regnames[p->reg]);
    return;
  }
  ierror(0);
}

static void cleanup_lword(FILE *f,struct obj *p)
/* cleanup increased address pointers of emit_hlword */
{
  if((p->flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&!scratchreg(p->reg,icp))
    emit(f,"\tsub\t2,%s\n",regnames[p->reg]);
}

static void pr(FILE *f,struct IC *p)
{
}

static void function_top(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionskopf                       */
{
  int i,rcnt;char *tmp;

  have_frame=0;stack_valid=1;
  pushedsize=0;

  if(!special_section(f,v)&&section!=CODE){
    emit(f,codename);
  }
  if(v->storage_class==EXTERN){
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier); 
  }else{
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));  
  }
  if(stack_check){
    stackchecklabel=++label;
    BSET(regs_modified,t1);
    emit(f,"\tmov\t%s,#%s%d\n",regnames[t1],labprefix,stackchecklabel);
    emit(f,"\t%s\t%s__stack_check\n",call,idprefix);/*FIXME:usrstack*/
  }
  for(rcnt=0,i=1;i<=16;i++){
    if(regused[i]&&!regscratch[i]&&!regsa[i])
      rcnt++;
  }
  if(v->tattr&NORBANK) rcnt=0;
  if(rcnt>((v->tattr&RBANK)?0:rwthreshold)){
    /*emit(f,"\tadd\t256,%s\n",regnames[sr]);*/
    emit(f,"\tincrb\n");
    have_frame=3;
  }else{
    for(i=1;i<=16;i++){
      if(regused[i]&&!regscratch[i]&&!regsa[i]){
	emit(f,"\tmove\t%s,@--%s\n",regnames[i],regnames[sp]);
	push(1);
	have_frame=1;pushedsize+=1;
      }
    }
  }
  if(offset){
    emit(f,"\tsub\t%ld,%s\n",offset,regnames[sp]);
    if(!have_frame) 
      have_frame|=1;
  }
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionsende                       */
{
  int i;
  if(offset) emit(f,"\tadd\t%ld,%s\n",offset,regnames[sp]);
  if(have_frame==3){
    /*emit(f,"\tsub\t256,%s\n",regnames[sr]);*/
    emit(f,"\tdecrb\n");
  }else{
    for(i=16;i>0;i--){
      if(regused[i]&&!regscratch[i]&&!regsa[i]){
	emit(f,"\tmove\t@%s++,%s\n",regnames[sp],regnames[i]);
	pop(1);
      }
    }
  }
  if(v->tattr&INTERRUPT){
    emit(f,"\trti\n");
  }else{
    if(ret) emit(f,"\t%s\n",ret);
  }
  if(v->storage_class==EXTERN){
    emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
    emit(f,"\t.size\t%s%s,$-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
  }else{
    emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,$-%s%ld\n",labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
  }
  if(stack_check)
    emit(f,"%s%d\tequ\t%ld\n",labprefix,stackchecklabel,offset+pushedsize-maxpushed);
  if(stack_valid){
    long ustack=stack+offset+pushedsize;
    if(!v->fi) v->fi=new_fi();
    if(v->fi->flags&ALL_STACK){
      if(v->fi->stack1!=stack)
        if(f) error(319,"stack",ustack,v->fi->stack1);
    }else{
      v->fi->flags|=ALL_STACK;
      v->fi->stack1=stack;
    }
    emit(f,"\t.equ\t%s__ustack_%s,%ld\n",idprefix,v->identifier,zm2l(v->fi->stack1));
  }
}
static int compare_objects(struct obj *o1,struct obj *o2)
{
  if((o1->flags&(REG|DREFOBJ))==REG&&(o2->flags&(REG|DREFOBJ))==REG&&o1->reg==o2->reg)
    return 1;
  if((o1->flags&(KONST|VAR|DREFOBJ|REG|VARADR))==(o2->flags&(KONST|VAR|DREFOBJ|REG|VARADR))&&o1->am==o2->am){
    if(!(o1->flags&VAR)||(o1->v==o2->v&&zmeqto(o1->val.vmax,o2->val.vmax))){
      if(!(o1->flags&REG)||o1->reg==o2->reg){
	return 1;
      }
    }
  }
  return 0;
}
static void clear_ext_ic(struct ext_ic *p)
{
  p->flags=0;
  p->r=0;
  p->offset=0;
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

    /* [Rx+] in q1 */
    if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->q1.reg; t=q1typ(p);
      if((!(p->q2.flags&REG)||p->q2.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((c2==ADD||(c2==ADDI2P&&(p2->typf2&NQ)!=HPOINTER))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(zmeqto(vmax,l2zm(1L))&&ISHWORD(t)){
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      p->q1.am=mymalloc(sizeof(*am));
	      p->q1.am->flags=POST_INC;
	      p->q1.am->base=r;
	    }else break;
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(((p2->q1.flags&REG)&&p2->q1.reg==r)||((p2->q2.flags&REG)&&p2->q2.reg==r)||((p2->z.flags&REG)&&p2->z.reg==r)) break;
	}
      }
    }
    /* [Rx+] in q2 */
    if(!p->q2.am&&(p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->q2.reg; t=q2typ(p);
      if((!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
	  if((c2==ADD||(c2==ADDI2P&&(p2->typf2&NQ)!=HPOINTER))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(zmeqto(vmax,l2zm(1L))&&ISHWORD(t)){
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      p->q2.am=mymalloc(sizeof(*am));
	      p->q2.am->flags=POST_INC;
	      p->q2.am->base=r;
	    }else break;
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(((p2->q1.flags&REG)&&p2->q1.reg==r)||((p2->q2.flags&REG)&&p2->q2.reg==r)||((p2->z.flags&REG)&&p2->z.reg==r)) break;
	}
      }
    }
    /* [Rx+] in z */
    /* currently we do it only if q2 is empty; could be more clever */
    if(!p->z.am&&!p->q2.flags&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->z.reg; t=ztyp(p);
      if((!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
	  if((c2==ADD||(c2==ADDI2P&&(p2->typf2&NQ)!=HPOINTER))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(zmeqto(vmax,l2zm(1L))&&ISHWORD(t)){
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	      p->z.am=mymalloc(sizeof(*am));
	      p->z.am->flags=POST_INC;
	      p->z.am->base=r;
	    }else break;
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(((p2->q1.flags&REG)&&p2->q1.reg==r)||((p2->q2.flags&REG)&&p2->q2.reg==r)||((p2->z.flags&REG)&&p2->z.reg==r)) break;
	}
      }
    }
    /* --@Rx */
    if(c==SUBIFP&&isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg&&p->q1.reg<=16&&isconst(q2)){
      long sz;
      r=p->q1.reg;
      eval_const(&p->q2.val,q2typ(p));
      sz=zm2l(vmax);
      if(sz==1){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r&&(!(p2->q2.flags&REG)||p2->q2.reg!=r)&&(!(p2->z.flags&REG)||p2->z.reg!=r)&&(c2!=CONVERT||(q1typ(p2)&NQ)<=(ztyp(p2)&NQ))){
	    t=(q1typ(p2)&NQ);
	    if((ISINT(t)||ISPOINTER(t))&&ISHWORD(t)&&zmeqto(sizetab[q1typ(p2)&NQ],l2zm(sz))&&!islibcall(p2)){
	      p2->q1.am=am=mymalloc(sizeof(*am));
	      p2->q1.val.vmax=l2zm(0L);
	      am->base=r;
	      am->flags=PRE_DEC;
	      p->code=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	      break;
	    }
	  }
	  if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r&&(!(p2->q1.flags&REG)||p2->q1.reg!=r)&&(!(p2->z.flags&REG)||p2->z.reg!=r)){
	    t=(q2typ(p2)&NQ);
	    if((ISINT(t)||ISPOINTER(t))&&ISHWORD(t)&&zmeqto(sizetab[q2typ(p2)&NQ],l2zm(sz))&&!islibcall(p2)){
	      p2->q2.am=am=mymalloc(sizeof(*am));
	      p2->q2.val.vmax=l2zm(0L);
	      am->base=r;
	      am->flags=PRE_DEC;
	      p->code=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	      break;
	    }
	  }
	  /* currently we do it only if q2 is empty; perhaps be more clever in the future */
	  if(!p2->z.am&&!p2->q2.flags&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r&&(!(p2->q2.flags&REG)||p2->q2.reg!=r)&&(!(p2->q1.flags&REG)||p2->q1.reg!=r)){
	    t=(ztyp(p2)&NQ);
	    if((ISINT(t)||ISPOINTER(t))&&ISHWORD(t)&&zmeqto(sizetab[ztyp(p2)&NQ],l2zm(sz))&&!islibcall(p2)){
	      p2->z.am=am=mymalloc(sizeof(*am));
	      p2->z.val.vmax=l2zm(0L);
	      am->base=r;
	      am->flags=PRE_DEC;
	      p->code=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	      break;
	    }
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
	  if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
	  if((p2->z.flags&REG)&&p2->z.reg==r) break;
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
  return &obj;
}


/* load an operand if necessary, use register r if needed */
static void load_op(FILE *f,struct obj *o,int t,int r)
{
  if(o->am)
    return;

  if(o->flags&(REG|VARADR))
    return;

  if(o->flags&KONST){
    if((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE){
      int l=addfpconst(o,t);
      emit(f,"\tmove\t%s%d,%s\n",labprefix,l,regnames[r]);
      o->reg=r;
      o->flags=REG|DREFOBJ;
    }else if(load_const||(o->flags&DREFOBJ)){
      emit(f,"\tmove\t");
      emit_obj(f,o,t);
      emit(f,",%s\n",regnames[r]);
      o->flags|=REG;
      o->flags&=~KONST;
    }else
      return;
  }else{
    if(o->v->storage_class==AUTO||o->v->storage_class==REGISTER){
      if(ISHWORD(t)&&voff(o)==0){
	if(o->flags&DREFOBJ){
	  emit(f,"\tmove\t@%s,%s\n",regnames[sp],regnames[r]);
	  o->reg=r;
	  o->flags|=REG|DREFOBJ;
	  BSET(regs_modified,r);
	  return;
	}else
	  r=sp;
      }else{
	emit(f,"\tmove\t%s,%s\n",regnames[sp],regnames[r]);
	if(voff(o))
	  emit(f,"\tadd\t%ld,%s\n",voff(o),regnames[r]);
      }
      
    }else{
      emit(f,"\tmove\t");
      emit_obj(f,o,t);
      emit(f,",%s\n",regnames[r]);
    }
    
    
    if(o->flags&DREFOBJ){
      emit(f,"\tmove\t@%s,%s\n",regnames[r],regnames[r]);
    }
    
    o->flags|=REG|DREFOBJ;
  }

  BSET(regs_modified,r);
  o->reg=r;
}

static void move(FILE *f,struct obj *q,int qr,struct obj *z,int zr,int t)
/*  Generates code to move object q (or register qr) into object z (or  */
/*  register zr). One must be a register and DREFOBJ only allowed with  */
/*  registers.                                                          */
{
  if(q&&z&&compare_objects(q,z))
    return;

  if(q&&(q->flags&(REG|DREFOBJ))==REG) qr=q->reg;
  if(z&&(z->flags&(REG|DREFOBJ))==REG) zr=z->reg;

  if(qr&&qr==zr)
    return;


  if(ISLWORD(t)){
    if(!qr) load_op(f,q,t,t1);
    if(!zr) load_op(f,z,t,t2);
    
    emit(f,"\tmove\t");
    if(qr&&reg_pair(qr,&rp))
      emit(f,"%s",regnames[rp.r1]);
    else
      emit_lword(f,q,t,"@%s++");
    emit(f,",");
    if(zr&&reg_pair(zr,&rp))
      emit(f,"%s",regnames[rp.r1]);
    else
      emit_lword(f,z,t,"@%s++");
    emit(f,"\n");
    emit(f,"\tmove\t");
    if(qr&&reg_pair(qr,&rp))
      emit(f,"%s",regnames[rp.r2]);
    else
      emit_hword(f,q,t,"@%s++");
    emit(f,",");
    if(zr&&reg_pair(zr,&rp))
      emit(f,"%s",regnames[rp.r2]);
    else
      emit_hword(f,z,t,"@%s++");
    emit(f,"\n");
    if(!qr) cleanup_lword(f,q);
    if(!zr) cleanup_lword(f,z);
    return;
  }


  emit(f,"\tmove\t");
  if(qr) emit(f,"%s",regnames[qr]); else emit_obj(f,q,t);
  emit(f,",");
  if(zr) emit(f,"%s",regnames[zr]); else emit_obj(f,z,t);
  emit(f,"\n");
}

static int get_reg(FILE *f,struct IC *p)
{
  int i;
  for(i=1;i<=16;i++){
    if(!regs[i]&&(regscratch[i]||regused[i])&&!regsa[i]){
      BSET(regs_modified,i);
      return i;
    }
  }
  return 0;
}



static void save_result(FILE *f,int r,struct IC *p,int t)
/*  Saves result in register r to object o. May use tp or ti. */ 
{
  ierror(0);
}

static int scratchreg(int r,struct IC *p)
{
  int c;
  if(r==t1||r==t2)
    return 1;
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

int emit_peephole(void)
{
  int entries,i,r1,r2,r3,r4;long x,y,z;
  char *asmline[EMIT_BUF_DEPTH];
  char s1[16],s2[16];

  if(NOPEEP)
    return 0;

  i=emit_l;
  if(emit_f==0)
    entries=i-emit_f+1;
  else
    entries=EMIT_BUF_DEPTH;
  asmline[0]=emit_buffer[i];
 
  if(entries>=1){
    if(sscanf(asmline[0],"\tmove\t0,R%d",&r1)==1){
      sprintf(asmline[0],"\txor\tR%d,R%d\n",r1,r1);
      return 1;
    }
  }

  if(entries>=2){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[1]=emit_buffer[i];
    if(sscanf(asmline[0],"\t%15s\t%ld,R%d",s1,&x,&r1)==3&&sscanf(asmline[1],"\t%15s\t%ld,R%d",s2,&y,&r2)==3&&r1==r2&&!strcmp(s1,s2)&&(!strcmp(s1,"add")||!strcmp(s1,"sub")||!strcmp(s1,"shl")||!strcmp(s1,"shr"))){
      sprintf(asmline[1],"\t%s\t%ld,R%d\n",s1,x+y,r1);
      remove_asm();
      return 1;
    }
    if(sscanf(asmline[0],"\t%15s\t%ld,@R%d",s1,&x,&r1)==3&&sscanf(asmline[1],"\t%15s\t%ld,@R%d",s2,&y,&r2)==3&&r1==r2&&!strcmp(s1,s2)&&(!strcmp(s1,"add")||!strcmp(s1,"sub")||!strcmp(s1,"shl")||!strcmp(s1,"shr"))){
      sprintf(asmline[1],"\t%s\t%ld,@R%d\n",s1,x+y,r1);
      remove_asm();
      return 1;
    }
    if(sscanf(asmline[0],"\tmove\tR%d,R%d",&r1,&r2)==2&&sscanf(asmline[1],"\tmove\tR%d,R%d",&r3,&r4)==2&&r1==r4&&r2==r3){
      remove_asm();
      return 1;
    }
    if(sscanf(asmline[0],"\tshl\t1,R%d",&r1)==1&&!strstr(asmline[0],"shr")){
      sprintf(asmline[0],"\tadd\tR%d,R%d\n",r1,r1);
      return 1;
    }
  } 

  return 0;
}

int init_cg(void)
/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
{
  int i;
  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign=l2zm(1L);
  stackalign=l2zm(1L);
  char_bit=l2zm(16L);
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i=1;i<=MAXR;i++){
    if(i<=16){
      regsize[i]=l2zm(1L);regtype[i]=&ityp;
    }else{
      regsize[i]=l2zm(2L);regtype[i]=&ltyp;
    }
  }	
  
  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-32768L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[INT]=t_min[SHORT];
  t_min[LONG]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(32767UL);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[INT]=t_max[SHORT];
  t_max[LONG]=ul2zum(2147483647UL);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT]=t_max(LLONG);
  tu_max[CHAR]=ul2zum(65535UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=tu_max[SHORT];
  tu_max[LONG]=ul2zum(4294967295UL);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

  /*  Reserve a few registers for use by the code-generator.      */
  regsa[sp]=regsa[sr]=regsa[pc]=regsa[t1]=regsa[t2]=regsa[MTMP1]=1;
  regscratch[sp]=regscratch[sr]=regscratch[pc]=regscratch[t1]=regscratch[t2]=0;
  target_macros=marray;
  if(TINY) marray[0]="__TINY__";
  else if(LARGE) marray[0]="__LARGE__";
  else if(HUGE) marray[0]="__HUGE__";
  else marray[0]="__MEDIUM__";

  /*static_cse=0;*/
  dref_cse=0;

  if(g_flags[9]&USEDFLAG)
    rwthreshold=g_flags_val[9].l;
  else{
    if(optspeed)
      rwthreshold=1;
    else
      rwthreshold=2;
  }

  /* TODO: set argument registers */
  declare_builtin("__mulint32",LONG,LONG,r8r9,LONG,0,1,0);
  declare_builtin("__divint32",LONG,LONG,r8r9,LONG,0,1,0);
  declare_builtin("__divuint32",LONG,LONG,r8r9,LONG,0,1,0);
  declare_builtin("__modint32",LONG,LONG,r8r9,LONG,0,1,0);
  declare_builtin("__moduint32",LONG,LONG,r8r9,LONG,0,1,0);
  declare_builtin("__lslint32",LONG,LONG,r8r9,INT,11,1,0);
  declare_builtin("__lsrint32",LONG,LONG,r8r9,INT,11,1,0);
  declare_builtin("__lsruint32",LONG,LONG,r8r9,INT,11,1,0);

  declare_builtin("__mulint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__addint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__subint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__andint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__orint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__eorint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__negint64",LLONG,LLONG,0,0,0,1,0);
  declare_builtin("__lslint64",LLONG,LLONG,0,INT,0,1,0);

  declare_builtin("__divint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__divuint64",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__modint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__moduint64",UNSIGNED|LLONG,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);
  declare_builtin("__lsrint64",LLONG,LLONG,0,INT,0,1,0);
  declare_builtin("__lsruint64",UNSIGNED|LLONG,UNSIGNED|LLONG,0,INT,0,1,0);
  declare_builtin("__cmpint64",INT,LLONG,0,LLONG,0,1,0);
  declare_builtin("__cmpuint64",INT,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);

  declare_builtin("__sint64toflt32",FLOAT,LLONG,0,0,0,1,0);
  declare_builtin("__uint64toflt32",FLOAT,UNSIGNED|LLONG,0,0,0,1,0);
  declare_builtin("__sint64toflt64",DOUBLE,LLONG,0,0,0,1,0);
  declare_builtin("__uint64toflt64",DOUBLE,UNSIGNED|LLONG,0,0,0,1,0);
  declare_builtin("__flt32tosint64",LLONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt32touint64",UNSIGNED|LLONG,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64tosint64",LLONG,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint64",UNSIGNED|LLONG,DOUBLE,0,0,0,1,0);

  declare_builtin("__sint32toflt32",FLOAT,LONG,r8r9,0,0,1,0);
  declare_builtin("__uint32toflt32",FLOAT,UNSIGNED|LONG,r8r9,0,0,1,0);
  declare_builtin("__sint32toflt64",DOUBLE,LONG,0,0,0,1,0);
  declare_builtin("__uint32toflt64",DOUBLE,UNSIGNED|LONG,0,0,0,1,0);
  declare_builtin("__flt32tosint32",LONG,FLOAT,r8r9,0,0,1,0);
  declare_builtin("__flt32touint32",UNSIGNED|LONG,FLOAT,r8r9,0,0,1,0);
  declare_builtin("__flt64tosint32",LONG,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint32",UNSIGNED|LONG,DOUBLE,0,0,0,1,0);

  declare_builtin("__sint16toflt32",FLOAT,INT,r8,0,0,1,0);
  declare_builtin("__uint16toflt32",FLOAT,UNSIGNED|INT,r8,0,0,1,0);
  declare_builtin("__sint16toflt64",DOUBLE,INT,r9,0,0,1,0);
  declare_builtin("__uint16toflt64",DOUBLE,UNSIGNED|INT,r9,0,0,1,0);
  declare_builtin("__flt32tosint16",INT,FLOAT,r8r9,0,0,1,0);
  declare_builtin("__flt32touint16",UNSIGNED|INT,FLOAT,r8r9,0,0,1,0);
  declare_builtin("__flt64tosint16",INT,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint16",UNSIGNED|INT,DOUBLE,0,0,0,1,0);




  declare_builtin("__flt32toflt64",DOUBLE,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64toflt32",FLOAT,DOUBLE,0,0,0,1,0);


  declare_builtin("__addflt32",FLOAT,FLOAT,r8r9,FLOAT,0,1,0);
  declare_builtin("__subflt32",FLOAT,FLOAT,r8r9,FLOAT,0,1,0);
  declare_builtin("__mulflt32",FLOAT,FLOAT,r8r9,FLOAT,0,1,0);
  declare_builtin("__divflt32",FLOAT,FLOAT,r8r9,FLOAT,0,1,0);
  declare_builtin("__negflt32",FLOAT,FLOAT,r8r9,0,0,1,0);
  declare_builtin("__cmpsflt32",INT,FLOAT,r8r9,FLOAT,0,1,0);

  declare_builtin("__addflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__subflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__mulflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__divflt64",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__negflt64",DOUBLE,DOUBLE,0,0,0,1,0);
  declare_builtin("__cmpsflt64",INT,DOUBLE,0,DOUBLE,0,1,0);

  return 1;
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
{
  int f=t->flags&NQ;
  if(f==LLONG||f==DOUBLE||f==LDOUBLE)
    return 0;
  if(ISSCALAR(f)){
    if(f==LONG||f==FPOINTER||f==HPOINTER||f==FLOAT)
      return r8r9;
    else
      return r8;
  }
  return 0;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  t&=NQ;
  if(r==0) return(0);
  if((t<LONG||t==NPOINTER)&&r<=16) return 1;
  if(t==LONG||t==FLOAT||t==FPOINTER||t==HPOINTER){
    if(r>16) return 1;
  }
  return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  if(r<=16) return 0;
  if(p){
    switch(r){
    case 17: p->r1=1;p->r2=2;break;
    case 18: p->r1=3;p->r2=4;break;
    case 19: p->r1=5;p->r2=6;break;
    case 20: p->r1=7;p->r2=8;break;
    case 21: p->r1=9;p->r2=10;break;
    case 22: p->r1=t1;p->r2=t2;break;
	
    default: ierror(0);
    }
  }
  return 1;
}

int cost_savings(struct IC *p,int r,struct obj *o)
{
  int c=p->code;
  if((o->flags&DREFOBJ)){
    if(o->flags&VKONST) return 1;
    if(r<=4&&p->q2.flags&&o!=&p->z)
      return 6;
    else
      return 4;
  }else if(o->flags&VKONST){
    struct obj *co=&o->v->cobj;
    if((p->code==ASSIGN&&(p->z.flags&DREFOBJ))||p->code==PUSH)
      return 4;
    if(co->flags&VARADR)
      return 4;
    if(o==&p->q1)
      eval_const(&co->val,q1typ(p));
    else
      eval_const(&co->val,q2typ(p));
    /*FIXME*/
    return 0;
  }else if(c==GETRETURN&&p->q1.reg==r){
    return 4;
  }else if(c==SETRETURN&&p->z.reg==r){
    return 4;
  }else if(c==CONVERT&&((p->typf&NQ)==CHAR||(p->typf2&NQ)==CHAR)&&regok(r,CHAR,0)){
    return 3;
  }
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
  return 0;
}

int must_convert(int o,int t,int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
/*  In this generic 32bit RISC cpu pointers and 32bit   */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
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
  long l=zm2l(size)*2;
  if(newobj&&section!=SPECIAL)
    emit(f,"%ld\n",l);
  else
    emit(f,"\t.space\t%ld\n",l);
  newobj=0; 
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
}
static void new_section(FILE *f,int nsec)
{
  if(!f||section==nsec) return;
  section=nsec;
  if(nsec==HDATA){
    emit(f,hdataname);
  }else if(nsec==FDATA){
    emit(f,fdataname);
  }else if(nsec==NDATA){
    emit(f,ndataname);
  }else if(nsec==HCDATA){
    emit(f,hcdataname);
  }else if(nsec==FCDATA){
    emit(f,fcdataname);
  }else if(nsec==NCDATA){
    emit(f,ncdataname);
  }else if(nsec==HBSS){
    emit(f,hbssname);
  }else if(nsec==FBSS){
    emit(f,fbssname);
  }else if(nsec==NBSS){
    emit(f,nbssname);
  }
}
void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;
  char *attr;struct Typ *tv;
  tv=v->vtyp;
  while(tv->flags==ARRAY) tv=tv->next;
  attr=tv->attr;
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->storage_class==STATIC){
    if(ISFUNC(v->vtyp->flags)) return;
    emit(f,"\t.type\t%s%ld,@object\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,%ld\n",labprefix,zm2l(v->offset),zm2l(szof(v->vtyp))*2);
    if(!special_section(f,v)){
      if((v->vtyp->flags&NQ)==BIT){
	new_section(f,BITS);
      }else{
	if(v->clist&&(!constflag||(g_flags[3]&USEDFLAG))){
	  if(attr&&strstr(attr,STR_HUGE))
	    new_section(f,HDATA);
	  else if(attr&&strstr(attr,STR_FAR))
	    new_section(f,FDATA);
	  else if(attr&&strstr(attr,STR_NEAR))
	    new_section(f,NDATA);
	  else{
	    if(HUGE)
	      new_section(f,HDATA);
	    else if(LARGE)
	      new_section(f,FDATA);
	    else
	      new_section(f,NDATA);
	  }
	}
	if(v->clist&&constflag&&!(g_flags[3]&USEDFLAG)){
	  if(attr&&strstr(attr,STR_HUGE))
	    new_section(f,HCDATA);
	  else if(attr&&strstr(attr,STR_FAR))
	    new_section(f,FCDATA);
	  else if(attr&&strstr(attr,STR_NEAR))
	    new_section(f,NCDATA);
	  else{
	    if(HUGE)
	      new_section(f,HCDATA);
	    else if(LARGE)
	      new_section(f,FCDATA);
	    else
	      new_section(f,NCDATA);
	  }
	}
	if(!v->clist){
	  if(attr&&strstr(attr,STR_HUGE))
	    new_section(f,HBSS);
	  else if(attr&&strstr(attr,STR_FAR))
	    new_section(f,FBSS);
	  else if(attr&&strstr(attr,STR_NEAR))
	    new_section(f,NBSS);
	  else{
	    if(HUGE)
	      new_section(f,HBSS);
	    else if(LARGE)
	      new_section(f,FBSS);
	    else
	      new_section(f,NBSS);
	  }
	}
      }
    }
    gen_align(f,falign(v->vtyp));
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
  }

  if(v->storage_class==EXTERN){
    if(v->flags&(DEFINED|TENTATIVE)){
      emit(f,"\t.type\t%s%s,@object\n",idprefix,v->identifier);
      emit(f,"\t.size\t%s%s,%ld\n",idprefix,v->identifier,zm2l(szof(v->vtyp))*2);
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
      if(!special_section(f,v)){
	if(v->clist&&(!constflag||(g_flags[3]&USEDFLAG))){
	  if(attr&&strstr(attr,STR_HUGE))
	    new_section(f,HDATA);
	  else if(attr&&strstr(attr,STR_FAR))
	    new_section(f,FDATA);
	  else if(attr&&strstr(attr,STR_NEAR))
	    new_section(f,NDATA);
	  else{
	    if(HUGE)
	      new_section(f,HDATA);
	    else if(LARGE)
	      new_section(f,FDATA);
	    else
	      new_section(f,NDATA);
	  }
	}
	if(v->clist&&constflag&&!(g_flags[3]&USEDFLAG)){
	  if(attr&&strstr(attr,STR_HUGE))
	    new_section(f,HCDATA);
	  else if(attr&&strstr(attr,STR_FAR))
	    new_section(f,FCDATA);
	  else if(attr&&strstr(attr,STR_NEAR))
	    new_section(f,NCDATA);
	  else{
	    if(HUGE)
	      new_section(f,HCDATA);
	    else if(LARGE)
	      new_section(f,FCDATA);
	    else
	      new_section(f,NCDATA);
	  }
	}
	if(!v->clist){
	  if(attr&&strstr(attr,STR_HUGE))
	    new_section(f,HBSS);
	  else if(attr&&strstr(attr,STR_FAR))
	    new_section(f,FBSS);
	  else if(attr&&strstr(attr,STR_NEAR))
	    new_section(f,NBSS);
	  else{
	    if(HUGE)
	      new_section(f,HBSS);
	    else if(LARGE)
	      new_section(f,FBSS);
	    else
	      new_section(f,NBSS);
	  }
	}
      }
      gen_align(f,falign(v->vtyp));
      emit(f,"%s%s:\n",idprefix,v->identifier);
    }else if(strcmp(v->identifier,"__va_start")){
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    }
  }
}

void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  if(ISPOINTER(t)){
    if(p->tree)
      emit(f,"\t%s\t",dct[t&NQ]); 
    if(ISLWORD(t))
      t=UNSIGNED|LONG;
    else
      t=UNSIGNED|SHORT;
    if(!p->tree)
      emit(f,"\t%s\t",dct[t&NQ]);
  }else{
    emit(f,"\t%s\t",dct[t&NQ]);
  }
  if(!p->tree){
    if(ISFLOAT(t)){
      /*  auch wieder nicht sehr schoen und IEEE noetig   */
      unsigned char *ip;

      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x,0x%02x%02x",ip[1],ip[0],ip[3],ip[2]);
      if((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE){
	emit(f,",0x%02x%02x,0x%02x%02x",ip[5],ip[4],ip[7],ip[6]);
      }
    }else{
      if(ISLWORD(t)){
	long l;
	eval_const(&p->val,t);
	l=zm2l(zmand(p->val.vmax,l2zm(65535L)));
	emit(f,"%ld",l);
	l=zm2l(zmand(zmrshift(p->val.vmax,l2zm(16L)),l2zm(65535L)));
	emit(f,",%ld",l);
      }else
	emitval(f,&p->val,(t&NU)|UNSIGNED);
    }
  }else{
    int m=p->tree->o.flags;
    p->tree->o.flags&=~VARADR;
    emit_obj(f,&p->tree->o,t&NU);
    p->tree->o.flags=m;
  }
  emit(f,"\n");
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
  int c,t,lastcomp=0,cmpzero=0,elab=0,reg,short_add,bit_reverse,need_return=0;
  struct obj *bit_obj;char *bit_reg;
  static int idone;
  struct obj o,*cc=0;int cc_t;
  struct IC *p2;
  if(TINY){
    call="asub";
    jump="abra";
  }else{
    call="asub";
    jump="abra";
  }
  if(v->tattr&INTERRUPT){
    ret="rti";
    need_return=1;
  }else
    ret="move\t@R13++,R15";

  if(DEBUG&1) printf("gen_code()\n");
  if(!v->fi) v->fi=new_fi();
  v->fi->flags|=ALL_REGS;

  for(p2=p;p2;p2=p2->next) clear_ext_ic(&p2->ext);
  if(!(g_flags[5]&USEDFLAG)){
    peephole(p);
    if(!frame_used) offset=l2zm(0L);
  }
  for(c=1;c<=15;c++) regs[c]=regsa[c];
  regs[16]=0;
  for(c=1;c<=MAXR;c++){
    if(regscratch[c]&&(regsa[c]||regused[c])){
      BSET(regs_modified,c);
    }
  }
  loff=zm2l(offset);
  function_top(f,v,loff);
  stackoffset=notpopped=dontpop=maxpushed=0;
  stack=0;
  for(;p;pr(f,p),p=p->next){
    c=p->code;t=p->typf;
    t2_used=0; short_add=0;
    icp=p;
    if(c==NOP) continue;
    if(c==ALLOCREG){
      regs[p->q1.reg]=1;
      if(reg_pair(p->q1.reg,&rp)) regs[rp.r1]=regs[rp.r2]=1;
      BSET(regs_modified,p->q1.reg);
      continue;
    }
    if(c==FREEREG){
      regs[p->q1.reg]=0;
      if(reg_pair(p->q1.reg,&rp)) regs[rp.r1]=regs[rp.r2]=0;
      continue;
    }
    if(notpopped&&!dontpop){
      int flag=0;
      if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
	emit(f,"\tadd\t%ld,%s\n",notpopped,regnames[sp]);
	pop(notpopped);notpopped=0;cc=0;
      }
    }
    if(c==LABEL) {cc=0;emit(f,"%s%d:\n",labprefix,t);continue;}
    if(c==BRA){
      if(p->typf==exit_label&&!have_frame){
	emit(f,"\t%s\n",ret);
      }else{
	if(t==exit_label) need_return=1;
	emit(f,"\trbra\t%s%d,1\n",labprefix,t);
      }
      cc=0;continue;
    }
    if(c>=BEQ&&c<BRA){
      char *s=0;
      cc=0;
      if(cmpzero){
	cmpzero=0;
	if(lastcomp&UNSIGNED){
	  if(c==BLT) continue;
	  if(c==BGE) s="1";
	  if(c==BLE) s="z";
	  if(c==BGT) s="!z";
	}else{
	  if(c==BLT) s="n";
	  if(c==BGE) s="!n";
	  if(c==BLE) ierror(0);
	  if(c==BGT) ierror(0);
	}
      }
      if(!s){
	if(lastcomp&UNSIGNED) s=ccu[c-BEQ]; else s=ccs[c-BEQ];
      }
      if(t==exit_label&&!have_frame){
	emit(f,"\tabra\t@%s++,%s\n",regnames[sp],s);
      }else{
	emit(f,"\trbra\t%s%d,%s\n",labprefix,t,s);
	if(t==exit_label) need_return=1;
      }
      continue;
    }
    if(c==MOVETOREG){
      load_op(f,&p->q1,SHORT,t1);
      if(p->z.reg<=16){
	move(f,&p->q1,0,0,p->z.reg,SHORT);
	cc=&p->q1;cc_t=SHORT;
      }else{
	move(f,&p->q1,0,0,p->z.reg,LONG);
	cc=0;
      }
      continue;
    }
    if(c==MOVEFROMREG){
      load_op(f,&p->z,SHORT,t2);
      if(p->q1.reg<=16){
	move(f,0,p->q1.reg,&p->z,0,SHORT);
	cc=&p->z;cc_t=SHORT;
      }else{
	move(f,0,p->q1.reg,&p->z,0,LONG);
	cc=0;
      }
      continue;
    }
    
    if(c==TEST){
      /* TODO: optimize in COMPARE? */
      lastcomp=t;
      if(cc&&(cc_t&NU)==(t&NU)&&compare_objects(cc,&p->q1)){
	continue;
      }

      p->code=c=COMPARE;
      gval.vmax=l2zm(0L);
      p->q2.flags=KONST;
      eval_const(&gval,MAXINT);
      insert_const(&p->q2.val,t);
    } 
    /* switch operands so that we have matching branch */
    if(c==COMPARE){
      p2=p->next;
      while(p2&&p2->code==FREEREG) p2=p2->next;
      if(!p2||p2->code<BEQ||p2->code>BGT) ierror(0);
      if(p2->code==BGT){
	o=p->q1;p->q1=p->q2;p->q2=o;
	p2->code=BLT;
      }else if(p2->code==BLE){
	o=p->q1;p->q1=p->q2;p->q2=o;
	p2->code=BGE;
      }
    }

    if(c==COMPARE&&isconst(q2)){
      eval_const(&p->q2.val,t);
      if(ISNULL()){
	if(cc&&(cc_t&NU)==(t&NU)&&compare_objects(cc,&p->q1)){
	  p2=p->next;
	  while(p2&&p2->code==FREEREG) p2=p2->next;
	  if(!p2||p2->code<BEQ||p2->code>BGT) ierror(0);
	  if((t&UNSIGNED)||p2->code==BNE||p2->code==BEQ||p2->code==BLT||p2->code==BGE){
	    lastcomp=t;
	    cmpzero=1;
	    continue;
	  }
	}
      }
    }

    if(c==SUBPFP){
      p->code=c=SUB;
      if((p->typf2&NQ)==NPOINTER) p->typf=t=INT;
      else if((p->typf2&NQ)==HPOINTER) p->typf=t=LONG;
      else ierror(0);
      if((p->typf2&NQ)==NPOINTER){
	cc=&p->z;cc_t=NPOINTER;
      }else{
	cc=0;
      }
    }

    if(c==ADDI2P||c==SUBIFP){
      /*if(c==ADDI2P) p->code=c=ADD; else p->code=c=SUB;*/
      if((p->typf2&NQ)!=HPOINTER){
	p->typf=t=(UNSIGNED|SHORT);
	short_add=2;
	if(isreg(q2)&&reg_pair(p->q2.reg,&rp)){
	  /*FIXME:warning*/
	  p->q2.reg=rp.r1;
	}
      }else if(ISHWORD(t)){
	p->typf=t=(UNSIGNED|LONG);
	short_add=1;
      }
      if((p->typf2&NQ)==NPOINTER){
	cc=&p->z;cc_t=NPOINTER;
      }else{
	cc=0;
      }      
    }

    if(c==CONVERT&&!must_convert(p->typf,p->typf2,0)){
      p->code=c=ASSIGN;
      p->q2.val.vmax=sizetab[p->typf&NQ];
    }

    if(c==CONVERT){
      int to=p->typf2&NU;
      if(to==INT||to==CHAR) to=SHORT;
      if(to==(UNSIGNED|INT)||to==(UNSIGNED|CHAR)||to==NPOINTER) to=(UNSIGNED|SHORT);
      if(to==FPOINTER||to==HPOINTER) to=(UNSIGNED|LONG);
      if((t&NU)==INT||(t&NU)==CHAR) t=SHORT;
      if((t&NU)==(UNSIGNED|INT)||(t&NU)==(UNSIGNED|CHAR)||(t&NU)==NPOINTER) t=(UNSIGNED|SHORT);
      if((t&NQ)==FPOINTER||(t&NQ)==HPOINTER) t=(UNSIGNED|LONG);
      if((to&NQ)==LONG){
	if((t&NQ)!=SHORT) ierror(0);
	load_op(f,&p->q1,to,t1);
	load_op(f,&p->z,t,t2);
	if(isreg(q1)){
	  if(!reg_pair(p->q1.reg,&rp))
	    ierror(0);
	  move(f,0,rp.r1,&p->z,0,t);
	}else
	  move(f,&p->q1,0,&p->z,0,t);
	cc=&p->z;cc_t=t;
	continue;
      }else if((t&NQ)==LONG){
	if((to&NQ)!=SHORT) ierror(0);
	load_op(f,&p->q1,to,t1);
	load_op(f,&p->z,t,t2);
	if(isreg(z)){
	  if(!reg_pair(p->z.reg,&rp))
	    ierror(0);
	  emit(f,"\txor\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
	  move(f,&p->q1,0,0,rp.r1,to);
	  if(!(to&UNSIGNED)){
	    emit(f,"\trbra\t%s%d,!n\n",labprefix,++label);
	    emit(f,"\tsub\t1,%s\n",regnames[rp.r2]);
	    emit(f,"%s%d:\n",labprefix,label);
	  }
	}else{
	  if(to&UNSIGNED){
	    emit(f,"\tmove\t");
	    emit_obj(f,&p->q1,to);
	    emit(f,",@%s++\n",regnames[p->z.reg]);
	    emit(f,"\tmove\t0,@%s\n",regnames[p->z.reg]);
	  }else{
	    if(isreg(q1)){
	      reg=p->q1.reg;
	    }else{
	      reg=t1;
	      move(f,&p->q1,0,0,t1,to);
	    }
	    emit(f,"\tmove\t%s,@%s++\n",regnames[reg],regnames[p->z.reg]);
	    emit(f,"\tmove\t0,@%s\n",regnames[p->z.reg]);
	    emit(f,"\tmove\t%s,%s\n",regnames[reg],regnames[reg]);
	    emit(f,"\trbra\t%s%d,!n\n",labprefix,++label);
	    emit(f,"\tsub\t1,@%s\n",regnames[p->z.reg]);
	    emit(f,"%s%d:\n",labprefix,label);
	  }
	  if(!scratchreg(p->z.reg,p))
	    emit(f,"\tsub\t1,%s\n",regnames[p->z.reg]);
	}
	cc=0;
	continue;
      }else
	ierror(0);
    }
    if(c==MINUS){
      p->code=c=SUB;
      p->q2=p->q1;
      gval.vmax=l2zm(0L);
      eval_const(&gval,t);
      insert_const(&p->q1.val,t);
      p->q1.flags=KONST;
    }
    if(c==KOMPLEMENT){
      load_op(f,&p->q1,t,t1);
      load_op(f,&p->z,t,t2);
      if(ISLWORD(t)){
	emit(f,"\tnot\t");
	emit_lword(f,&p->q1,t,"@%s++");
	emit(f,",");
	emit_lword(f,&p->z,t,"@%s++");
	emit(f,"\n");
	emit(f,"\tnot\t");
	emit_hword(f,&p->q1,t,"@%s++");
	emit(f,",");
	emit_hword(f,&p->z,t,"@%s++");
	emit(f,"\n");
	cleanup_lword(f,&p->q1);
	cleanup_lword(f,&p->z);
	cc=0;
      }else{
	emit(f,"\tnot\t");
	emit_obj(f,&p->q1,t);
	emit(f,",");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	cc=&p->z;cc_t=t;
      }
      continue;
    }
    if(c==SETRETURN){
      if(p->z.reg){
	if(reg_pair(p->z.reg,&rp))
	  load_op(f,&p->q1,t,t1);
	else
	  load_op(f,&p->q1,t,p->z.reg);
	move(f,&p->q1,0,0,p->z.reg,t);
	BSET(regs_modified,p->z.reg);
      }
      cc=0; /* probably not needed */
      continue;
    }
    if(c==GETRETURN){
      if(p->q1.reg){
	if((!isreg(z)||p->z.reg!=p->q1.reg)&&!ISLWORD(t)){ cc=&p->z;cc_t=t;}
	load_op(f,&p->z,t,t2);
	move(f,0,p->q1.reg,&p->z,0,t);
      }
      continue;
    }
    if(c==CALL){
      int reg,jmp=0;long cstack=0;
      cc=0;
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp("__va_start",p->q1.v->identifier)){
	long va_off=loff-stackoffset+pushedsize+zm2l(va_offset(v))+1;
	emit(f,"\tmove\t%s,%s\n",regnames[sp],regnames[r8]);
	if(va_off)
	  emit(f,"\tadd\t%ld,%s\n",va_off,regnames[r8]);
	BSET(regs_modified,r8);
	if(LARGE||HUGE){
	  emit(f,"\tmove\t0,%s\n",regnames[r9]);
	  BSET(regs_modified,r9);
	}
	continue;
      }
      if(stack_valid){
        int i;
        if(p->call_cnt<=0){
          err_ic=p;if(f) error(320);
          stack_valid=0;
        }
        for(i=0;stack_valid&&i<p->call_cnt;i++){
          if(p->call_list[i].v->fi&&(p->call_list[i].v->fi->flags&ALL_STACK)){
	    if(p->call_list[i].v->fi->stack1>cstack) cstack=p->call_list[i].v->fi->stack1;
          }else{
            err_ic=p;if(f) error(317,p->call_list[i].v->identifier);
            stack_valid=0;
          }
        }
      }
      if(!calc_regs(p,f!=0)&&v->fi) v->fi->flags&=~ALL_REGS;
      if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
	emit_inline_asm(f,p->q1.v->fi->inline_asm);
	callee_push(cstack);
      }else{
	if(stackoffset==0&&!have_frame&&!(v->tattr&INTERRUPT)){
	  struct IC *p2;
	  jmp=1;
	  for(p2=p->next;p2;p2=p2->next){
	    if(p2->code!=FREEREG&&p2->code!=ALLOCREG&&p2->code!=LABEL&&p2->code!=NOP&&
	       (p2->code!=GETRETURN||(p2->z.flags&(REG|DREFOBJ))!=REG||p2->q1.reg!=p2->z.reg)&&
	       (p2->code!=SETRETURN||(p2->q1.flags&(REG|DREFOBJ))!=REG||p2->q1.reg!=p2->z.reg)){
	      jmp=0;break;
	    }
	  }
	}
	if(p->q1.flags&DREFOBJ){
	  int clabel=++label;
	  if(ISLWORD(p->q1.dtyp)){
	    ierror(0);
	  }
	  p->q1.flags&=~DREFOBJ;
	  load_op(f,&p->q1,t,t1);
	  if(!ISLWORD(p->q1.dtyp)){
	    emit(f,"\t%s\t",jmp?jump:"asub");
	    emit_obj(f,&p->q1,p->q1.dtyp);
	    emit(f,",1\n");
	    push(1);
	    callee_push(cstack);
	    pop(1);
	  }else{
	    ierror(0);
	  }
	}else{
	  if(jmp){
	    emit(f,"\t%s\t",jump);
	    callee_push(cstack);
	  }else{
	    emit(f,"\t%s\t",call);
	    if(TINY)
	      push(1);
	    else
	      push(2);
	    callee_push(cstack);
	    if(TINY)
	      pop(1);
	    else
	      pop(2);
	  }
	  emit_obj(f,&p->q1,t);
	  emit(f,",1\n");
	}
      }
      if(jmp&&!need_return) ret="";
      if(!zmeqto(l2zm(0L),p->q2.val.vmax)){
	notpopped+=zm2l(p->q2.val.vmax);
	dontpop-=zm2l(p->q2.val.vmax);
	if(!(g_flags[2]&USEDFLAG)&&stackoffset==-notpopped){
	  /*  Entfernen der Parameter verzoegern  */
	}else{
	  emit(f,"\tadd\t%ld,%s\n",zm2l(p->q2.val.vmax),regnames[sp]);
	  pop(zm2l(p->q2.val.vmax));
	  notpopped-=zm2l(p->q2.val.vmax);cc=0;
	}
      }
      continue;
    }
    if(c==ASSIGN||c==PUSH){
      if(c==PUSH) dontpop+=zm2l(p->q2.val.vmax);

      if(c==ASSIGN||c==PUSH){
	long sz=zm2l(p->q2.val.vmax);
	int qreg,zreg,creg,i;
	if(sz==1){
	  load_op(f,&p->q1,t,t1);
	  if(c==ASSIGN){
	    load_op(f,&p->z,t,t2);
	    move(f,&p->q1,0,&p->z,0,t);
	    cc=&p->z;cc_t=t;
	  }else{
	    emit(f,"\tmove\t");
	    emit_obj(f,&p->q1,t);
	    emit(f,",@--%s\n",regnames[sp]);
	    push(zm2l(p->z.val.vmax));	
	    cc=&p->q1;cc_t=t;
	  }    
	  continue;
	}else if(ISLWORD(t)&&(isreg(q1)||isreg(z)||(p->q1.flags&(KONST|DREFOBJ))==KONST)){
	  if(c==ASSIGN){
	    move(f,&p->q1,0,&p->z,0,t);
	  }else{
	    load_op(f,&p->q1,t,t1);
	    
	    if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ))
	      emit(f,"\tadd\t2,%s\n",regnames[p->q1.reg]);
	    
	    emit(f,"\tmove\t");
	    emit_hword(f,&p->q1,t,"@--%s");
	    emit(f,",@--%s\n",regnames[sp]);
	    emit(f,"\tmove\t");
	    emit_lword(f,&p->q1,t,"@--%s");
	    emit(f,",@--%s\n",regnames[sp]);
	    push(2);
	  }	  
	  cc=0;
	  continue;
	}else{
	  static char cpstr[64];
	  cc=0;
	  load_op(f,&p->q1,t,t1);
	  if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&scratchreg(p->q1.reg,p)){
	    qreg=p->q1.reg;
	  }else{
	    if(!(p->q1.flags&REG))
	      ierror(0);
	    qreg=t1;
	    move(f,0,p->q1.reg,0,qreg,INT);
	  }
	  if(c==ASSIGN){
	    load_op(f,&p->z,t,t2);
	    if((p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&scratchreg(p->z.reg,p)){
	      zreg=p->z.reg;
	    }else{
	      if(!(p->z.flags&REG))
		ierror(0);
	      if(qreg==t1) zreg=t2; else zreg=t1;
	      move(f,0,p->z.reg,0,zreg,INT);
	    }
	    sprintf(cpstr,"\tmove\t@%s++,@%s++\n",regnames[qreg],regnames[zreg]);
	  }else{
	    zreg=sp;
	    emit(f,"\tadd\t%ld,%s\n",sz,regnames[qreg]);
	    sprintf(cpstr,"\tmove\t@--%s,@--%s\n",regnames[qreg],regnames[zreg]);
	    push(zm2l(p->q2.val.vmax));
	  }
	  if(sz<=9){
	    for(i=0;i<sz;i++)
	      emit(f,cpstr);
	  }else{
	    int cntpushed=0;
	    if(zreg!=t2)
	      creg=t2;
	    else{
	      creg=get_reg(f,p);
	      if(c==PUSH) ierror(0);
	      creg=r8;
	      emit(f,"\tmove\t%s,@--%s\n",regnames[creg],regnames[sp]);
	      cntpushed=1;
	    }
	    emit(f,"\tmove\t%ld,%s\n",sz/4,regnames[creg]);
	    emit(f,"%s%d:\n",labprefix,++label);
	    emit(f,cpstr);
	    emit(f,cpstr);
	    emit(f,cpstr);
	    emit(f,cpstr);
	    emit(f,"\tsub\t1,%s\n",regnames[creg]);
	    emit(f,"\trbra\t%s%d,!z\n",labprefix,label);
	    for(i=0;i<sz%4;i++)
	      emit(f,cpstr);
	    if(cntpushed)
	      emit(f,"\tmove\t@%s++,%s\n",regnames[sp],regnames[creg]);
	  }
	  continue;
	}

      }
      ierror(0);
    }
    if(c==ADDRESS){
      reg=0;
      if(0/*reg_pair(reg,&rp)*/){
	ierror(0);
	emit(f,"\tmov\t%s,%s\n",regnames[rp.r1],regnames[sp]);
	emit(f,"\tmov\t%s,#0\n",regnames[rp.r2]);
	if(voff(&p->q1))
	  emit(f,"\tadd\t%s,#%ld\n",regnames[rp.r1],voff(&p->q1)&0xffff);
      }else{
	if(isreg(z))
	  reg=p->z.reg;
	else{
	  load_op(f,&p->z,t,t2);
	}
	if(voff(&p->q1)){
	  if(!reg) reg=t1;
	  emit(f,"\tmove\t%s,%s\n",regnames[sp],regnames[reg]);
	  emit(f,"\tadd\t%ld,%s\n",voff(&p->q1),regnames[reg]);
	}else
	  reg=sp;
	move(f,0,reg,&p->z,0,ztyp(p));
      }
      cc=&p->z;cc_t=ztyp(p);
      continue;
    }

    if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==COMPARE||c==ADDI2P||c==SUBIFP){
      char *s;

      if(c==MULT||c==DIV||c==MOD){
	int code;
	load_op(f,&p->q1,t,t1);
	emit(f,"\tmove\t%ld,%s\n",(long)0xff1b,regnames[t2]);
	emit(f,"\tmove\t");
	emit_obj(f,&p->q1,t);
	emit(f,",@%s++\n",regnames[t2]);
	load_op(f,&p->q2,t,t1);
	emit(f,"\tmove\t");
	emit_obj(f,&p->q2,t);
	emit(f,",@%s++\n",regnames[t2]);
	emit(f,"\tmove\t%ld,%s\n",(long)0xff1f,regnames[t1]);
	if(c==MULT) code=0; else code=2;
	if(!(t&UNSIGNED)) code++;
	emit(f,"\tmove\t%d,@%s\n",code,regnames[t1]);
	if(c==MOD)
	  emit(f,"\tmove\t%ld,%s\n",(long)0xff1e,regnames[t2]);
	load_op(f,&p->z,t,t1);
	emit(f,"\tmove\t@%s,",regnames[t2]);
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	cc=&p->z;cc_t=t;
	continue;
      }

      if(c==LSHIFT&&isconst(q2)&&isreg(q1)){
	eval_const(&p->q2.val,INT);
	if(zmeqto(vmax,l2zm(1L))){
	  p->code=c=ADD;
	  p->q2=p->q1;
	}
      }

      if(compare_objects(&p->q2,&p->z)){
	if(!compare_objects(&p->q1,&p->z)){
	  if(c==LSHIFT||c==RSHIFT){
	    reg=get_reg(f,p);
	    if(reg){
	      move(f,&p->q2,0,0,reg,q2typ(p));
	      p->q2.flags=REG;
	      p->q2.reg=reg;
	    }else{
	      emit(f,"\tmove\t");
	      emit_obj(f,&p->q2,q2typ(p));
	      emit(f,",@--%s\n",regnames[sp]);
	      p->q2.flags=REG|DREFOBJ;
	      p->q2.reg=sp;
	      p->q2.am=mymalloc(sizeof(*p->q2.am));
	      p->q2.am->flags=POST_INC;
	      p->q2.am->base=sp;
	    }
	  }else if(c==SUB||c==SUBPFP||c==SUBIFP){
	    /* TODO: check pointer sizes subifp/subpfp */
	    if(ISLWORD(t)){
	      if(isreg(z)){
		if(!reg_pair(p->z.reg,&rp)) ierror(0);
		emit(f,"\tnot\t%s,%s\n",regnames[rp.r1],regnames[rp.r1]);
		emit(f,"\tnot\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
		emit(f,"\tadd\t1,%s\n",regnames[rp.r1]);
		emit(f,"\taddc\t0,%s\n",regnames[rp.r2]);
		p->q2=p->q1;
		p->q1=p->z;
		p->code=c=ADD;
	      }else
		ierror(0);
	    }else{
	      load_op(f,&p->z,t,t1);
	      emit(f,"\tnot\t");
	      emit_obj(f,&p->z,t);
	      emit(f,",");
	      emit_obj(f,&p->z,t);
	      emit(f,"\n");
	      emit(f,"\tadd\t1,");
	      emit_obj(f,&p->z,t);
	      emit(f,"\n");
	      load_op(f,&p->q1,t,t2);
	      emit(f,"\tadd\t");
	      emit_obj(f,&p->q1,t);
	      emit(f,",");
	      emit_obj(f,&p->z,t);
	      emit(f,"\n");
	      cc=&p->z;cc_t=t;
	      continue;
	    }
	  }else{
	    struct obj m;
	    m=p->q1;p->q1=p->q2;p->q2=m;
	  }
	}
      }

      if(c>=OR&&c<=AND){
	s=logicals[c-OR];
      }else{
	if(c==ADDI2P)
	  s=arithmetics[ADD-LSHIFT];
	else if(c==SUBIFP)
	  s=arithmetics[SUB-LSHIFT];
	else
	  s=arithmetics[c-LSHIFT];
      }

      if(c==COMPARE){
	lastcomp=t;
	load_op(f,&p->q1,t,t1);
	load_op(f,&p->q2,t,t2);

	if(ISLWORD(t)){
	  if((p->q1.flags&DREFOBJ)&&p->q1.reg!=t1){
	    emit(f,"\tmove\t%s,%s\n",regnames[p->q1.reg],regnames[t1]);
	    p->q1.reg=t1;
	  }
	  if((p->q2.flags&DREFOBJ)&&p->q2.reg!=t2){
	    emit(f,"\tmove\t%s,%s\n",regnames[p->q2.reg],regnames[t2]);
	    p->q2.reg=t2;
	  }
	  p2=p->next;
	  while(p2&&p2->code==FREEREG) p2=p2->next;
	  if(!p2||p2->code<BEQ||p2->code>BGT) ierror(0);
	  c=p2->code;
	  if(c==BEQ||c==BNE){
	    emit(f,"\tcmp\t",s);
	    emit_lword(f,&p->q1,t,"@%s++");
	    emit(f,",");
	    emit_lword(f,&p->q2,t,"@%s++");
	    emit(f,"\n");
	    if(c==BEQ)
	      emit(f,"\trbra\t%s%d,!z\n",labprefix,elab=++label);
	    else 
	      emit(f,"\trbra\t%s%d,!z\n",labprefix,p2->typf);
	    emit(f,"\tcmp\t",s);
	    emit_hword(f,&p->q1,t,"@%s++");
	    emit(f,",");
	    emit_hword(f,&p->q2,t,"@%s++");
	    emit(f,"\n");
	    emit(f,"\trbra\t%s%d,%s\n",labprefix,p2->typf,ccs[c-BEQ]);
	  }else{
	    elab=++label;
	    if(p->q1.flags&DREFOBJ) emit(f,"\tadd\t1,%s\n",regnames[p->q1.reg]);
	    if(p->q2.flags&DREFOBJ) emit(f,"\tadd\t1,%s\n",regnames[p->q2.reg]);
	    emit(f,"\tcmp\t",s);
	    emit_hword(f,&p->q2,t,"@%s");
	    emit(f,",");
	    emit_hword(f,&p->q1,t,"@%s");
	    emit(f,"\n");
	    emit(f,"\trbra\t%s%d,%s\n",labprefix,c==BLT?p2->typf:elab,(t&UNSIGNED)?ccu[BLT-BEQ]:ccs[BLT-BEQ]);
	    /* unfortunately flags are overwritten... */
	    emit(f,"\tcmp\t",s);
	    emit_hword(f,&p->q2,t,"@%s");
	    emit(f,",");
	    emit_hword(f,&p->q1,t,"@%s");
	    emit(f,"\n");
	    emit(f,"\trbra\t%s%d,!z\n",labprefix,c==BLT?elab:p2->typf);
	    if(p->q1.flags&DREFOBJ) emit(f,"\tsub\t1,%s\n",regnames[p->q1.reg]);
	    if(p->q2.flags&DREFOBJ) emit(f,"\tsub\t1,%s\n",regnames[p->q2.reg]);
	    emit(f,"\tcmp\t",s);
	    emit_lword(f,&p->q2,t,"@%s");
	    emit(f,",");
	    emit_lword(f,&p->q1,t,"@%s");
	    emit(f,"\n");
	    emit(f,"\trbra\t%s%d,%s\n",labprefix,p2->typf,ccu[c-BEQ]);
	  }
	  cc=0;
#if 0
	  /* last branch done in branch IC */
	  lastcomp=UNSIGNED|INT;
#else
	  if(c!=BNE)
	    emit(f,"%s%d:\n",labprefix,elab);
	  p2->code=NOP;
#endif
	  continue;
	}else{
	  emit(f,"\tcmp\t",s);
	  emit_obj(f,&p->q2,t);
	  emit(f,",");
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	}
	cc=0;
	continue;
      }


      if(!compare_objects(&p->q1,&p->z)){
	load_op(f,&p->q1,t,t1);
	load_op(f,&p->z,t,t2);
	if((p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&isreg(z)&&p->q2.reg==p->z.reg){
	  move(f,0,p->q2.reg,0,t2,NPOINTER);
	  p->q2.reg=t2;
	}
	move(f,&p->q1,0,&p->z,0,t);
	/* cleanup postinc if necessary (not done by cleanup_lword */
	if(p->z.reg==t2&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&ISLWORD(t))
	  emit(f,"\tsub\t2,%s\n",regnames[t2]);
      }else
	load_op(f,&p->z,t,t2);
      load_op(f,&p->q2,t,t1);

      if(ISLWORD(t)){

	emit(f,"\t%s\t",s);
	emit_lword(f,&p->q2,t,"@%s++");
	emit(f,",");
	emit_lword(f,&p->z,t,"@%s++");
	emit(f,"\n");
	emit(f,"\t%s%s\t",s,!strcmp(s,"add")||!strcmp(s,"sub")?"c":"");
	emit_hword(f,&p->q2,t,"@%s++");
	emit(f,",");
	emit_hword(f,&p->z,t,"@%s++");
	emit(f,"\n");
	cleanup_lword(f,&p->q2);
	cleanup_lword(f,&p->z);
	cc=0;

      }else{

	/* TODO: try to eliminate */
	if(c==LSHIFT){
	  emit(f,"\tcmp\t%s,%s\n",regnames[sp],regnames[sp]);
	}else if(c==RSHIFT){
	  if(t&UNSIGNED){
	    emit(f,"\tor\t%s,%s\n",regnames[sp],regnames[sp]);
	  }else{
	    emit(f,"\tshl\t1,");
	    emit_obj(f,&p->z,t);
	    emit(f,"\n");
	    emit(f,"\tshr\t1,");
	    emit_obj(f,&p->z,t);
	    emit(f,"\n");
	  }
	}
	emit(f,"\t%s\t",s);
	emit_obj(f,&p->q2,t);
	emit(f,",");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	cc=&p->z;cc_t=t;

      }
      if(ISLWORD(t)) cc=0;

      continue;
    }
    ierror(0);
  }
  if(notpopped){
    emit(f,"\tadd\t%ld,%s\n",notpopped,regnames[sp]);
    pop(notpopped);notpopped=0;
  }
  function_bottom(f,v,loff);
}

int shortcut(int c,int t)
{
  if(c==COMPARE||c==AND||c==OR||c==XOR) return 1;
  if(c==ADD||c==SUB) return 1;
  return 0;
}

void cleanup_cg(FILE *f)
{
  struct fpconstlist *p;
  unsigned char *ip;
  if(f&&stack_check){
    emit(f,"\t.global\t%s__stack_check\n",idprefix);
  }
  while(p=firstfpc){
    if(f){
      new_section(f,NDATA);
      emit(f,"%s%d:\n\t%s\t",labprefix,p->label,dct[SHORT]);
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x,0x%02x%02x",ip[1],ip[0],ip[3],ip[2]);
      if((p->typ&NQ)==DOUBLE||(p->typ&NQ)==LDOUBLE){
	emit(f,",0x%02x%02x,0x%02x%02x",ip[5],ip[4],ip[7],ip[6]);
      }
      emit(f,"\n");
    }
    firstfpc=p->next;
    free(p);
  }
}

int reg_parm(struct reg_handle *p,struct Typ *t,int mode,struct Typ *fkt)
{
  int f=t->flags&NQ;
  if(!ISSCALAR(f)) return 0;
  if(p->gpr>2||mode) return 0;
  if(f==LLONG||f==DOUBLE||f==LDOUBLE)
    return 0;
  else if(f==LONG||f==FLOAT||f==FPOINTER||f==HPOINTER){
    if(p->gpr==0) {p->gpr=2;return r8r9;}
    return 0;
  }else
    return r8+p->gpr++;
}

void insert_const(union atyps *p,int t)
/*  Traegt Konstante in entprechendes Feld ein.       */
{
  if(!p) ierror(0);
/*  *p = (union atyps) 0 ; /* rfi: clear unused bits */
  t&=NU;
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
  if(t==BIT){fprintf(f,"B");vmax=zc2zm(p->vchar);fprintf(f,"%d",!zmeqto(vmax,l2zm(0L)));}
  if(t==(UNSIGNED|BIT)){fprintf(f,"UB");vumax=zuc2zum(p->vuchar);fprintf(f,"%d",!zumeqto(vmax,ul2zum(0UL)));}
  if(t==CHAR){fprintf(f,"C");vmax=zc2zm(p->vchar);printzm(f,vmax);}
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
/*  Gibt atyps aus.                                     */
{
  t&=NU;
  if(t==BIT){vmax=zc2zm(p->vchar);emit(f,"%d",!zmeqto(vmax,l2zm(0L)));}
  if(t==(UNSIGNED|BIT)){vumax=zuc2zum(p->vuchar);emit(f,"%d",!zumeqto(vumax,ul2zum(0UL)));}
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
}

void cleanup_db(FILE *f)
{
}

char *use_libcall(int c,int t,int t2)
{
  static char fname[16];
  char *ret=0,*tt;

  if(c==COMPARE){
    if((t&NQ)==LLONG||ISFLOAT(t)){
      sprintf(fname,"__cmp%s%s%ld",(t&UNSIGNED)?"u":"s",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*16);
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
        sprintf(fname,"__%cint%ldtoflt%d",(t2&UNSIGNED)?'u':'s',zm2l(sizetab[t2&NQ])*16,(t==FLOAT)?32:64);
        ret=fname;
      }
      if(ISFLOAT(t2)){
        sprintf(fname,"__flt%dto%cint%ld",((t2&NU)==FLOAT)?32:64,(t&UNSIGNED)?'u':'s',zm2l(sizetab[t&NQ])*16);
        ret=fname;
      }
      if((t&NQ)==LLONG||(t2&NQ)==LLONG){
	sprintf(fname,"__%cint%ldto%cint%ld",(t2&UNSIGNED)?'u':'s',zm2l(sizetab[t2&NQ])*16,(t&UNSIGNED)?'u':'s',zm2l(sizetab[t&NQ])*16);
        ret=fname;
      }
    }
    if(ISINT(t)&&ISLWORD(t)){
      if(c==MINUS||c==KOMPLEMENT||c==ADD||c==SUB||c==OR||c==AND||c==XOR)
	return 0;
    }
    if((t&NQ)==LONG||(t&NQ)==LLONG||ISFLOAT(t)||(SOFTMUL&&(c==MULT||c==DIV||c==MOD))){
      if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==KOMPLEMENT||c==MINUS){
	if(c!=DIV&&c!=MOD&&c!=RSHIFT&&c!=COMPARE)
	  t&=~UNSIGNED;
	sprintf(fname,"__%s%s%s%ld",ename[c],(t&UNSIGNED)?"u":"",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*16);
	ret=fname;
      }
    }
  }


  return ret;
}

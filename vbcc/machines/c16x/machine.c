/*  Code generator for SAB c16x microcontrollers.               */

#include "supp.h"

static char FILE_[]=__FILE__;
/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for c16x V0.3 (c) in 1998-2005 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,0,0,0,
                    0,0,0,0,
                    0};
char *g_flags_name[MAXGF]={"cpu","int32","no-delayed-popping","const-in-data",
                           "merge-constants","no-peephole","mtiny","mlarge",
                           "mhuge","tasking"};
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
		  "R6/R7","R7/R8","R8/R9","R4/R5",
		  "R14/R15","R13/R14","R12/R13",
		  "R3/R4","R2/R3",
		  "MAXR+1","MAXR+2","ZEROS","ONES"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1};

int reg_prio[MAXR+1]={0,0,0,0,0,2,3,1,1,1,1,0,0,4,5,6,7,1,1,1,2,6,5,4,0,0};

struct reg_handle empty_reg_handle={0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__interrupt","__dpp0","__dpp1","__dpp2","__dpp3",0};
#define INTERRUPT 1
#define DPP0 2
#define DPP1 4
#define DPP2 8
#define DPP3 16


/****************************************/
/*  Some private data and functions.    */
/****************************************/

static char *bregnames[]={"nobreg","RL0","RL1","RL2","RL3","RL4","RL5","RL6","RL7",
 "e8","e9","e10","e11","e12","e13","e14","e15","e16",
 "e17","e18","e19","e20","e21","e22"
};

static long malign[MAX_TYPE+1]=  {1,1,1,2,2,2,2,2,2,2,1,2,2,2,1,1,1,2,1};
static long msizetab[MAX_TYPE+1]={0,1,1,2,2,4,8,4,8,8,0,2,4,4,0,0,0,2,0};

struct Typ ityp={SHORT},ltyp={LONG};

#define INT32 (g_flags[1]&USEDFLAG)
#define TINY (g_flags[6]&USEDFLAG)
#define LARGE (g_flags[7]&USEDFLAG)
#define HUGE (g_flags[8]&USEDFLAG)
#define TASKING (g_flags[9]&USEDFLAG)


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

#define SEG r2
#define SOF r1

#define MTMP1 MAXR+1
#define MTMP2 MAXR+2
#define ZEROS MAXR+3
#define ONES  MAXR+4

static int section=-1,newobj,scnt;
static char *codename="SCODE\tSECTION CODE WORD PUBLIC 'CPROGRAM'\n",*ecode="SCODE\tends\n",
            *ndataname="NDATA\tSECTION LDAT WORD PUBLIC\n",*endata="NDATA\tends\n",
            *fdataname="FDATA\tSECTION LDAT WORD PUBLIC\n",*efdata="FDATA\tends\n",
            *hdataname="HDATA\tSECTION LDAT WORD PUBLIC\n",*ehdata="HDATA\tends\n",
            *nbssname="NBSS\tSECTION LDAT WORD PUBLIC\n",*enbss="NBSS\tends\n",
            *fbssname="FBSS\tSECTION LDAT WORD PUBLIC\n",*efbss="FBSS\tends\n",
            *hbssname="HBSS\tSECTION LDAT WORD PUBLIC\n",*ehbss="HBSS\tends\n",
            *ncdataname="NCDATA\tSECTION LDAT WORD PUBLIC\n",*encdata="NCDATA\tends\n",
            *fcdataname="FCDATA\tSECTION LDAT WORD PUBLIC\n",*efcdata="FCDATA\tends\n",
            *hcdataname="HCDATA\tSECTION LDAT WORD PUBLIC\n",*ehcdata="HCDATA\tends\n",

            *bitsname="BITS\tSECTION BIT BIT PUBLIC 'CBITS'\n",*ebits="BITS\tends\n";

static char *even="\teven\n",*public="public",*comment="; ";

#define IMM_IND  1
#define VAR_IND  2
#define POST_INC 3
#define PRE_DEC  4

static char sec_end[32];
/* (user)stack-pointer, pointer-tmp, int-tmp; reserved for compiler */
static const int sp=1,tp=2,ti=11,ti2=12,r4=5,r5=6,r4r5=20;
static int tmp1,tmp2,tmp3,tmp4;
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);
static int scratchreg(int,struct IC *);
static struct Var nvar,fvar;

static char *marray[]={0,
                       "__far=__attr(\"far\")",
		       "__near=__attr(\"near\")",
		       "__huge=__attr(\"huge\")",
		       "__bit=__attr(\"bit\") unsigned char",
                       /*"__section(x)=__vattr(\"section(\"x\")\")",*/
                       "__section(x,y)=__vattr(\"section(\"#x\",\"#y\")\")",
		       "__rbank(x)=__vattr(\"rbank(\"__str(x)\")\")",
		       "__interrupt(x)=__interrupt __vattr(\"ivec(\"__str(x)\")\")",
		       "__sysstack(x)=__stack2(x)",
		       "__usrstack(x)=__stack(x)",
		       "__C16X__",
		       "__C167__",
		       "__ST10__",
		       "__sfr(x)=__vattr(\"sfr(\"__str(x)\")\") extern",
		       "__sfrbit(x,y)=__vattr(\"sfrbit(\"__str(x)\",\"__str(y)\")\") extern",
		       "__esfr(x)=__vattr(\"sfre(\"__str(x)\")\") extern",
		       "__esfrbit(x,y)=__vattr(\"sfrbite(\"__str(x)\",\"__str(y)\")\") extern",
		       "__SIZE_T_INT=1",
		       "__str(x)=#x",
		       0};

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)
#define issfrv(v) (v->vattr&&strstr(v->vattr,"sfr("))
#define issfrbitv(v) (v->vattr&&strstr(v->vattr,"sfrbit("))
#define isesfrv(v) (v->vattr&&strstr(v->vattr,"sfre("))
#define isesfrbitv(v) (v->vattr&&strstr(v->vattr,"sfrbite("))
#define issfr(x) ((p->x.flags&(VAR|DREFOBJ))==VAR&&issfrv(p->x.v))
#define issfrbit(x) ((p->x.flags&(VAR|DREFOBJ))==VAR&&issfrbitv(p->x.v))
#define isesfr(x) ((p->x.flags&(VAR|DREFOBJ))==VAR&&isesfrv(p->x.v))
#define isesfrbit(x) ((p->x.flags&(VAR|DREFOBJ))==VAR&&isesfrbitv(p->x.v))

static long loff,sysstackoffset,usrstackoffset,notpopped,dontpop,
  usrmaxpushed,sysmaxpushed,sysstack,usrstack;

static char *x_t[]={"?","","b","","","","","","","","","","","","","",""};
static char *ccs[]={"z","nz","lt","ge","le","gt"};
static char *logicals[]={"or","xor","and"};
static char *arithmetics[]={"shl","shr","add","sub","mul","div","mod"};
static char *dct[]={"","dbit","db","dw","dw","dw","dw","dw","dw","dw",
		    "(void)","dw","dsptr","dsptr"};
static char *vdct[]={"",".bit",".byte",".short",".short",".short",".short",".short",".short",".short",
		    "(void)",".short",".long",".long"};
static int pushedsize,pushorder=2;
static char *idprefix="_",*labprefix="l";
static int ti2_used;
static struct rpair qp;
static int exit_label,have_frame,stackchecklabel,stack_valid;
static char *ret,*call,*jump;
static int frame_used;

static char *dppprefix="___DPP_";
static int romdpp=0,ramdpp=1;

#define STR_NEAR "near"
#define STR_FAR "far"
#define STR_HUGE "huge"
#define STR_BADDR "baddr"

#define ISNULL() (zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
#define ISLWORD(t) ((t&NQ)==LONG||(t&NQ)==FPOINTER||(t&NQ)==HPOINTER||(t&NQ)==FLOAT)
#define ISHWORD(t) ((t&NQ)==INT||(t&NQ)==SHORT||(t&NQ)==NPOINTER)
#define ISSTATIC(v) ((v)->storage_class==EXTERN||(v)->storage_class==STATIC)
#define ISBADDR(v) ((v)->vtyp->attr&&strstr(STR_BADDR,(v)->vtyp->attr))

static int dppuse(struct Var *v,int section)
{
  if(v->tattr&DPP0) return 0;
  if(v->tattr&DPP1) return 1;
  if(v->tattr&DPP2) return 2;
  if(v->tattr&DPP3) return 3;
  if(section==NCDATA) return romdpp;
  if(section==NDATA||section==NBSS) return ramdpp;
  return -1;
}

static int ISFAR(struct Var *v)
{
  struct Typ *vt;
  if(v==&nvar) return 0;
  if(v==&fvar) return 1;
  if(issfrv(v)||isesfrv(v)||issfrbitv(v)||isesfrbitv(v))
    return 0;
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
  if(f&&TASKING){
    emit(f,sec_end);
    e=sec_end;
    while(*sec&&*sec!=')') {
      *e++=*sec;
      emit_char(f,*sec++);
    }
    *e=0;
    strcat(sec_end,"\tends\n");
    emit(f,"\tSECTION LDAT WORD PUBLIC\n");
  }else{
    emit(f,"\t.section\t");
    while(*sec&&*sec!=')') emit_char(f,*sec++);
    emit(f,"\n");
  }
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
static void callee_push(long usr,long sys)
{
  if(usr-usrstackoffset>usrstack)
    usrstack=usr-usrstackoffset;
  if(sys-sysstackoffset>sysstack)
    sysstack=sys-sysstackoffset;
}
static void push(long usr,long sys)
{
  usrstackoffset-=usr;
  if(usrstackoffset<usrmaxpushed) 
    usrmaxpushed=usrstackoffset;
  if(-usrmaxpushed>usrstack) usrstack=-usrmaxpushed;
  sysstackoffset-=sys;
  if(sysstackoffset<sysmaxpushed) 
    sysmaxpushed=sysstackoffset;
  if(-sysmaxpushed>sysstack) sysstack=-sysmaxpushed;
}
static void pop(long usr,long sys)
{
  usrstackoffset+=usr;
  sysstackoffset+=sys;
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
static long voff(struct obj *p)
{
  if(p->v->offset<0)
    return loff-zm2l(p->v->offset)+zm2l(p->val.vmax)-usrstackoffset+pushedsize-2;
  return zm2l(p->v->offset)+zm2l(p->val.vmax)-usrstackoffset;
}
static int alignment(struct obj *o)
{
  if(o->flags&DREFOBJ) return 1;
  if(!(o->flags&VAR)) ierror(0);
  if(ISSTATIC(o->v)) return zm2l(o->val.vmax)&1;
  return voff(o)&1;
}

static int isseg;

static void emit_obj(FILE *f,struct obj *p,int t)
/*  Gibt Objekt auf Bildschirm aus                      */
{
  if(p->am){
    static struct rpair bp;
    int base;
    if(reg_pair(p->am->base,&bp))
      base=bp.SOF;
    else
      base=p->am->base;
    if(p->am->flags==POST_INC){
      emit(f,"[%s+]",regnames[base]);
      return;
    }else if(p->am->flags==PRE_DEC){
      emit(f,"[-%s]",regnames[base]);
      return;
    }else{
      emit(f,"[%s+#%ld",regnames[base],p->am->offset);
      if(p->am->v){
	if(p->am->v->storage_class==STATIC)
	  emit(f,"+%s%ld",labprefix,zm2l(p->am->v->offset));
	else
	  emit(f,"+%s%s",idprefix,p->am->v->identifier);
      }
      emit(f,"]");
      return;
    }
  }
  if((p->flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
    emitval(f,&p->val,p->dtyp&NU);
    return;
  }
  if((p->flags&(DREFOBJ|REG))==(DREFOBJ|REG)) emit(f,"[");
  if(p->flags&VARADR) emit(f,"#");
  if((p->flags&(VAR|REG))==VAR){
    if(p->v==&nvar||p->v==&fvar){
      if(p->v==&fvar) emit(f,"SOF ");
      emitval(f,&p->val,p->dtyp&NU);
      return;
    }else if(issfrv(p->v)||isesfrv(p->v)||issfrbitv(p->v)||isesfrbitv(p->v)){
      emit(f,"%s",p->v->identifier);
    }else if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      if(voff(p))
	emit(f,"[%s+#%ld]",regnames[sp],voff(p));
      else emit(f,"[%s]",regnames[sp]);
    }else{
      if(!isseg&&!ISFUNC(p->v->vtyp->flags)){
        if(ISFAR(p->v)) 
          emit(f,"SOF ");
        else
          emit(f,"DPPX:");
      }
      isseg=0;
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,LONG);emit(f,"+");}
      if(p->v->storage_class==STATIC){
	emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
	emit(f,"%s%s",idprefix,p->v->identifier);
      }
    }
  }
  if(p->flags&REG){
    if(p->reg==MTMP1||p->reg==MTMP2){
      if(!reg_pair(p->reg,&rp)) ierror(0);
      emit(f,"%s",regnames[rp.SOF]);
    }else{
      emit(f,"%s",regnames[p->reg]);
      if((t&NQ)==BIT) emit(f,".0");
    }
  }
  /* sometimes we just or a REG into a KONST */
  if((p->flags&(KONST|REG))==KONST){
    if(ISFLOAT(t)){
      emit(f,"%s%d",labprefix,addfpconst(p,t));
    }else{
      emit(f,"#");emitval(f,&p->val,t&NU);
    }
  }
  if((p->flags&(DREFOBJ|REG))==(DREFOBJ|REG)) emit(f,"]");
}

static void pr(FILE *f,struct IC *p)
{
}
static int switched_bank;
static void function_top(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionskopf                       */
{
  int i;char *tmp;
  static int wrote_header;
  if(f&&!wrote_header){
    wrote_header=1;
    if(!TASKING){
      emit(f,"\t.sfr\tZEROS,0xfe,0x8e\n");
      emit(f,"\t.sfr\tONES,0xfe,0x8f\n");
      emit(f,"\t.sfr\tCP,0xfe,0x08\n");
      emit(f,"\t.sfr\tMDH,0xfe,0x06\n");
      emit(f,"\t.sfr\tMDL,0xfe,0x07\n");
      emit(f,"\t.sfr\tPSW,0xfe,0x88\n");
    }
  }
  have_frame=0;stack_valid=1;
  pushedsize=0;
  if(v->vattr&&(tmp=strstr(v->vattr,"ivec("))){
    int vec;char c;int rc;
    rc=sscanf(tmp+5,"%i%c",&vec,&c);
    if(rc!=2||c!=')') error(324,"illegal vector number");
    emit(f,"\t.section\t\".ivec%d\",\"axr\"\n",vec);
    emit(f,"\t.global\tivec%d\nivec%d:\n",vec,vec);
    if(v->storage_class==EXTERN)
      emit(f,"\tjmps\t%s%s\n",idprefix,v->identifier);
    else
      emit(f,"\tjmps\t%s%ld\n",labprefix,zm2l(v->offset));
    section=-1;
  }
  if(!special_section(f,v)&&section!=CODE){
    if(f&&TASKING){
      emit(f,sec_end);emit(f,codename);
      section=CODE;
      strcpy(sec_end,ecode);
    }else
      emit(f,codename);
  }
  if(TASKING){
    if(v->storage_class==EXTERN){
      if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
	emit(f,"\tpublic\t%s%s\n",idprefix,v->identifier);
      if(TINY)
	emit(f,"%s%s\tproc\tnear\n",idprefix,v->identifier);
      else
	emit(f,"%s%s\tproc\tfar\n",idprefix,v->identifier);
    }else{
      if(TINY)
	emit(f,"%s%ld\tproc\tnear\n",labprefix,zm2l(v->offset));
      else
	emit(f,"%s%ld\tproc\tfar\n",labprefix,zm2l(v->offset));
    }
  }else{
    if(v->storage_class==EXTERN){
      if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
	emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
      emit(f,"\t.align\t1\n%s%s:\n",idprefix,v->identifier); 
    }else{
      emit(f,"\t.align\t1\n%s%ld:\n",labprefix,zm2l(v->offset));  
    }
  }
  if(v->vattr&&(tmp=strstr(v->vattr,"rbank("))){
    char *p;
    tmp+=strlen("rbank(");
    emit(f,"\tmov\t%s",idprefix);
    for(p=tmp;*p&&*p!=')';p++) emit_char(f,*p);
    emit(f,",%s\n",regnames[sp]);
    emit(f,"\tscxt\tCP,#%s",idprefix);
    for(p=tmp;*p&&*p!=')';p++) emit_char(f,*p);
    emit(f,"\n");
    switched_bank=1;
  }else
    switched_bank=0;
  if(v->tattr&INTERRUPT){
    /*FIXME?*/
    emit(f,"\tpush\tMDL\n");
    emit(f,"\tpush\tMDH\n");
  }
  if(stack_check){
    stackchecklabel=++label;
    BSET(regs_modified,tp);
    emit(f,"\tmov\t%s,#%s%d\n",regnames[tp],labprefix,stackchecklabel);
    emit(f,"\t%s\t%s__stack_check\n",call,idprefix);/*FIXME:usrstack*/
  }
  for(i=1;i<=16;i++){
    if(regused[i]&&!regscratch[i]&&!regsa[i]){
      emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[i]);
      push(2,0);
      have_frame=1;pushedsize+=2;
    }
  }
  if(offset){
    emit(f,"\tsub\t%s,#%ld\n",regnames[sp],offset);
    have_frame=1;
  }
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionsende                       */
{
  int i;
  if(offset) emit(f,"\tadd\t%s,#%ld\n",regnames[sp],offset);
  for(i=16;i>0;i--){
    if(regused[i]&&!regscratch[i]&&!regsa[i]){
      emit(f,"\tmov\t%s,[%s+]\n",regnames[i],regnames[sp]);
      pop(2,0);
    }
  }
  if(v->tattr&INTERRUPT){
    emit(f,"\tpop\tMDH\n");
    emit(f,"\tpop\tMDL\n");
  }
  if(switched_bank)
    emit(f,"\tpop\tCP\n");
  if(ret) emit(f,"\t%s\n",ret);
  if(TASKING){
    if(v->storage_class==EXTERN){
      emit(f,"%s%s\tendp\n",idprefix,v->identifier);
      if(!strcmp("main",v->identifier)) emit(f,"\textern\t__CSTART:far\n");
    }else
      emit(f,"%s%ld\tendp\n",labprefix,zm2l(v->offset));
  }else{
    if(v->storage_class==EXTERN){
      emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
      emit(f,"\t.size\t%s%s,$-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
    }else{
      emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
      emit(f,"\t.size\t%s%ld,$-%s%ld\n",labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
    }
  }
  if(stack_check)
    emit(f,"%s%d\tequ\t%ld\n",labprefix,stackchecklabel,offset+pushedsize-usrmaxpushed);
  if(stack_valid){
    long ustack=usrstack+offset+pushedsize,sstack=sysstack;
    if(!v->fi) v->fi=new_fi();
    if(v->fi->flags&ALL_STACK){
      if(v->fi->stack1!=ustack)
        if(f) error(319,"user-",ustack,v->fi->stack1);
      if(v->fi->stack2!=sstack)
        if(f) error(319,"system-",sstack,v->fi->stack2);
    }else{
      v->fi->flags|=ALL_STACK;
      v->fi->stack1=ustack;
      v->fi->stack2=sstack;
    }
#if 0
    emit(f,"%s usrstacksize=%ld\n",comment,zm2l(v->fi->stack1));
    emit(f,"%s sysstacksize=%ld\n",comment,zm2l(v->fi->stack2));
#endif
    if(TASKING){
    }else{
      emit(f,"\t.equ\t%s__ustack_%s,%ld\n",idprefix,v->identifier,zm2l(v->fi->stack1));
      emit(f,"\t.equ\t%s__sstack_%s,%ld\n",idprefix,v->identifier,zm2l(v->fi->stack2));
    }
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
#define EXT_IC_CMPIA 1
#define EXT_IC_BTST  2
#define EXT_IC_CMPIB 3
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
    /* and x,#const;bne/beq */
    if(c==AND&&isconst(q2)&&isreg(z)&&!ISLWORD(ztyp(p))){
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
    /* [Rx+] in q1 */
    if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
      r=p->q1.reg; t=q1typ(p);
      if((!(p->q2.flags&REG)||p->q2.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((c2==ADD||(c2==ADDI2P&&(p2->typf2&NQ)!=HPOINTER))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if((zmeqto(vmax,l2zm(1L))&&(t&NQ)==CHAR)||(zmeqto(vmax,l2zm(2L))&&ISHWORD(t))){
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
	    if((zmeqto(vmax,l2zm(1L))&&(t&NQ)==CHAR)||(zmeqto(vmax,l2zm(2L))&&ISHWORD(t))){
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
    /* move x->t; add/sub x,#1/-1/2/-2-> x; test/cmp t,#0; freereg t => cmpid12 */
    if(c==ASSIGN&&isreg(q1)&&isreg(z)&&ISHWORD(p->typf)){
      p2=p->next;if(p2) c2=p2->code;
      if(p2&&(c2==ADD||c2==SUB||c2==ADDI2P||c2==SUBIFP)&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==p->q1.reg&&p2->z.reg==p2->q1.reg&&(p2->q2.flags&(KONST|DREFOBJ))==KONST&&ISHWORD(p2->typf)){
	long l;
	eval_const(&p2->q2.val,p2->typf);
	l=zm2l(vmax);
	if((l==1||l==2||l==-1||l==-2)&&p2->next){
	  struct IC *p3=p2->next;
	  if((p3->code==TEST||(p3->code==COMPARE&&(p3->q2.flags&(KONST|DREFOBJ))==KONST))&&(p3->q1.flags&(REG|DREFOBJ))==REG&&p3->q1.reg==p->z.reg&&scratchreg(p->z.reg,p3)&&ISHWORD(p3->typf)){
	    if(c2==SUB||c2==SUBIFP) l=-l;
	    p3->q1=p->q1;
	    p->code=c=NOP;
	    p->q1.flags=p->q2.flags=p->z.flags=0;
	    p2->code=NOP;
	    p2->q1.flags=p2->q2.flags=p2->z.flags=0;
	    p3->ext.flags=EXT_IC_CMPIB;
	    p3->ext.offset=l;
	    p3->ext.r=p->q1.reg;
	  }
	}
      }
    }
    /* add/sub x,#1/2/-1/-2 ->x; cmp x,#c => cmpid12 x,#c+-12 */
    if((c==ADD||c==SUB||c==ADDI2P||c==SUBIFP)&&isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg&&isconst(q2)&&ISHWORD(p->typf)&&!reg_pair(p->q1.reg,&rp)){
      eval_const(&p->q2.val,p->typf);
      if(zmeqto(vmax,l2zm(-1L))||zmeqto(vmax,l2zm(-2L))||zmeqto(vmax,l2zm(1L))||zmeqto(vmax,l2zm(2L))){
	long l=zm2l(vmax);
	if(c==SUB||c==SUBIFP) l=-l;
	r=p->q1.reg;
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(c2==TEST||(c2==COMPARE&&(p2->q2.flags&(KONST|DREFOBJ))==KONST))&&ISHWORD(p2->typf)){
	    unsigned long ul;
	    eval_const(&p2->q2.val,q2typ(p2));
	    ul=zum2ul(vumax);
	    if(ul<65534){
	      p2->ext.flags=EXT_IC_CMPIA;
	      p2->ext.offset=l;
	      p2->ext.r=r;
	      p->code=c=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	    }
	    break;

	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
	  if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
	  if((p2->z.flags&REG)&&p2->z.reg==r) break;
	}
      }
    }
    /* [Rx+#const] */
    if((c==ADDI2P||c==SUBIFP)&&((p->typf2&NQ)==NPOINTER||(p->typf2&NQ)==FPOINTER)&&isreg(z)&&((p->q2.flags&(KONST|DREFOBJ))==KONST||(p->q1.flags&VARADR))){
      int base;zmax of;struct obj *o;struct Var *v;
      if(p->q1.flags&VARADR){
	v=p->q1.v;
	of=p->q1.val.vmax;
	r=p->z.reg;
	if(isreg(q2)) base=p->q2.reg; else base=r;
      }else{
	eval_const(&p->q2.val,p->typf);
	if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
	v=0;
	r=p->z.reg;
	if(isreg(q1)) base=p->q1.reg; else base=r;
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
		if(isreg(q1)){
		  p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
		}else{
		  p->code=c=ASSIGN;p->q2.flags=0;
		  p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
		}
	      }else{
		if(isreg(q2)){
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
/* prints label types used by tasking assembler */
static void emit_label_type(FILE *f,struct Var *v)
{
  if(TASKING){
    if(ISFUNC(v->vtyp->flags)){
      if(TINY)
	emit(f,"near");
      else
	emit(f,"far");
    }else{
      if((v->vtyp->flags&NQ)==BIT)
	emit(f,"bit");
      else if((v->vtyp->flags&NQ)==CHAR)
	emit(f,"byte");
      else
	emit(f,"word");
    }
  }
}
static void move(FILE *f,struct obj *q,int qr,struct obj *z,int zr,int t)
/*  Generates code to move object q (or register qr) into object z (or  */
/*  register zr). One must be a register and DREFOBJ only allowed with  */
/*  registers.                                                          */
{
  long l=0;
  t&=NQ;
  if(q&&(q->flags&(REG|DREFOBJ))==REG) qr=q->reg;
  if(z&&(z->flags&(REG|DREFOBJ))==REG) zr=z->reg;
  if(qr&&qr==zr) return;
  if((t&NQ)==BIT){
    if(!zr&&((z->flags&(VAR|REG|DREFOBJ|VARADR))!=VAR||!ISSTATIC(z->v))) ierror(0);
    if(q&&(q->flags&(KONST|DREFOBJ))==KONST){
      if(z&&(z->flags&(VAR|DREFOBJ))==VAR&&isesfrbitv(z->v))
	emit(f,"\textr\t#1\n");
      eval_const(&q->val,BIT);
      if(zmeqto(vmax,l2zm(0L)))
	emit(f,"\tbclr\t");
      else
	emit(f,"\tbset\t");
      if(zr) emit(f,"%s.0",regnames[zr]); else emit_obj(f,z,t);
      emit(f,"\n");
    }else{
      if(!qr&&((q->flags&(VAR|REG|DREFOBJ|VARADR))!=VAR||!ISSTATIC(q->v))) ierror(0);
      if((q->flags&(VAR|DREFOBJ))==VAR&&isesfrbitv(q->v)){
	if(!zr&&((z->flags&(VAR|DREFOBJ))!=VAR||!isesfrbitv(z->v))){
	  emit(f,"\textr\t#1\n");
	  emit(f,"\tbmov\t%s.0,",regnames[ti]);
	  if(qr) emit(f,"%s.0",regnames[qr]); else emit_obj(f,q,t);
	  emit(f,"\n");
	  emit(f,"\tbmov\t");
	  if(zr) emit(f,"%s.0",regnames[zr]); else emit_obj(f,z,t);
	  emit(f,",%s.0\n",regnames[ti]);
	  return;
	}else
	  emit(f,"\textr\t#1\n");
      }else if(z&&(z->flags&(VAR|DREFOBJ))==VAR&&isesfrbitv(z->v)){
	if(!qr){
	  emit(f,"\tbmov\t%s.0,",regnames[ti]);
	  if(qr) emit(f,"%s.0",regnames[qr]); else emit_obj(f,q,t);
	  emit(f,"\n");
	  emit(f,"\textr\t#1\n");
	  emit(f,"\tbmov\t");emit_obj(f,z,t);
	  emit(f,",%s.0\n",regnames[ti]);
	  return;
	}else
	  emit(f,"\textr\t#1\n");
      }
      emit(f,"\tbmov\t");
      if(zr) emit(f,"%s.0",regnames[zr]); else emit_obj(f,z,t);
      emit(f,",");
      if(qr) emit(f,"%s.0",regnames[qr]); else emit_obj(f,q,t);
      emit(f,"\n");
    }
    return;
  }
  if(q){
    if(!zr) ierror(0);
    if(q->am&&reg_pair(q->am->base,&rp)){
      l=rp.SOF;
      if(zr>16)
	emit(f,"\texts\t%s,#2\n",regnames[rp.SEG]);
      else
	emit(f,"\texts\t%s,#1\n",regnames[rp.SEG]);
    }else if((q->flags&DREFOBJ)&&reg_pair(q->reg,&rp)){
      l=rp.SOF;
      if(zr>16)
	emit(f,"\texts\t%s,#2\n",regnames[rp.SEG]);
      else
	emit(f,"\texts\t%s,#1\n",regnames[rp.SEG]);
    }else if((q->flags&(VAR|REG|VARADR))==VAR&&ISSTATIC(q->v)&&ISFAR(q->v)){
      emit(f,"\texts\t#SEG ");
      isseg=1;emit_obj(f,q,INT);
      if(zr>16)
	emit(f,",#2\n");
      else
	emit(f,",#1\n");
    }
    if(reg_pair(zr,&rp)){
      if(q->am){
	emit(f,"\tmov\t%s,",regnames[rp.r1]);
	emit_obj(f,q,t);emit(f,"\n");
	q->am->offset+=2;
	emit(f,"\tmov\t%s,",regnames[rp.r2]);
	emit_obj(f,q,t);emit(f,"\n");
	q->am->offset-=2;
	return;
      }else if(q->flags&DREFOBJ){
	int tmp=0;
	if(!(q->flags&REG)) ierror(0);
	if(!l) l=q->reg;
	if(l==rp.r1){
	  tmp=rp.r1;
	  /*FIXME: tp hier immer frei? */
	  BSET(regs_modified,tp);
	  rp.r1=tp;
	}
	/*FIXME: test auf scratchreg*/
	if(q->reg>MAXR){
	  emit(f,"\tmov\t%s,[%s+]\n",regnames[rp.r1],regnames[l]);
	  emit(f,"\tmov\t%s,[%s]\n",regnames[rp.r2],regnames[l]);
	}else{
	  emit(f,"\tmov\t%s,[%s]\n",regnames[rp.r1],regnames[l]);
	  emit(f,"\tmov\t%s,[%s+#2]\n",regnames[rp.r2],regnames[l]);
	}
	if(tmp) emit(f,"\tmov\t%s,%s\n",regnames[tmp],regnames[rp.r1]);
	return;
      }else if(q->flags&VARADR){
	q->flags&=~VARADR;
	emit(f,"\tmov\t%s,#",regnames[rp.r1]);emit_obj(f,q,t);emit(f,"\n");
	emit(f,"\tmov\t%s,#SEG ",regnames[rp.r2]);isseg=1;emit_obj(f,q,t);emit(f,"\n");
	q->flags|=VARADR;
	return;
      }else if((q->flags&(KONST|DREFOBJ))==KONST){
	long l2;
	if(ISFLOAT(t)) ierror(0);
	eval_const(&q->val,t);
	l=zm2zl(zmand(vmax,l2zm(65535L)));
	emit(f,"\tmov\t%s,#%ld\n",regnames[rp.r1],l);
	l2=zm2l(zmrshift(vmax,l2zm(16L)));
	if(l2==l) emit(f,"\tmov\t%s,%s\n",regnames[rp.r2],regnames[rp.r1]);
	else emit(f,"\tmov\t%s,#%ld\n",regnames[rp.r2],l2);
	return;
      }else if(qr){
	if(!reg_pair(qr,&qp)) ierror(0);
	emit(f,"\tmov\t%s,%s\n",regnames[rp.r1],regnames[qp.r1]);
	emit(f,"\tmov\t%s,%s\n",regnames[rp.r2],regnames[qp.r2]);
	return;
      }else{
	if(!(q->flags&VAR)) ierror(0);
	if(q->v->storage_class==AUTO||q->v->storage_class==REGISTER){
	  l=voff(q);
	  if(l)
	    emit(f,"\tmov\t%s,[%s+#%ld]\n",regnames[rp.r1],regnames[sp],l);
	  else
	    emit(f,"\tmov\t%s,[%s]\n",regnames[rp.r1],regnames[sp]);
	  emit(f,"\tmov\t%s,[%s+#%ld]\n",regnames[rp.r2],regnames[sp],l+2);
	  return;
	}else{
	  emit(f,"\tmov\t%s,",regnames[rp.r1]);emit_obj(f,q,t);emit(f,"\n");
	  emit(f,"\tmov\t%s,",regnames[rp.r2]);emit_obj(f,q,t);emit(f,"+2\n");
	  return;
	}
      }
      ierror(0);
    }else{
      emit(f,"\tmov%s\t%s,",x_t[t&NQ],(t==CHAR?bregnames[zr]:regnames[zr]));
      if(qr&&t==CHAR)
	emit(f,"%s",bregnames[qr]); 
      else if(!q->am&&(q->flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&reg_pair(q->reg,&rp))
	emit(f,"[%s]",regnames[rp.SOF]);
      else
	emit_obj(f,q,t);
      emit(f,"\n");
      return;
    }
  }
  if(z){
    if(!qr) ierror(0);
    if(z->am&&reg_pair(z->am->base,&rp)){
      l=rp.SOF;
      if(qr>16)
	emit(f,"\texts\t%s,#2\n",regnames[rp.SEG]);
      else
	emit(f,"\texts\t%s,#1\n",regnames[rp.SEG]);      
    }else if((z->flags&DREFOBJ)&&reg_pair(z->reg,&rp)){
      l=rp.SOF;
      if(qr>16)
	emit(f,"\texts\t%s,#2\n",regnames[rp.SEG]);
      else
	emit(f,"\texts\t%s,#1\n",regnames[rp.SEG]);
    }else if((z->flags&(VAR|REG|VARADR))==VAR&&ISSTATIC(z->v)&&ISFAR(z->v)){
      emit(f,"\texts\t#SEG ");
      isseg=1;emit_obj(f,z,INT);
      if(qr>16)
	emit(f,",#2\n");
      else
	emit(f,",#1\n");
    }
    if(reg_pair(qr,&rp)){
      if(z->am){
	emit(f,"\tmov\t");emit_obj(f,z,t);emit(f,",%s\n",regnames[rp.r1]);
	z->am->offset+=2;
	emit(f,"\tmov\t");emit_obj(f,z,t);emit(f,",%s\n",regnames[rp.r2]);
	z->am->offset-=2;
	return;
      }else if(z->flags&DREFOBJ){
	if(!(z->flags&REG)) ierror(0);
	if(!l) l=z->reg;
	/*FIXME: test auf scratchreg statt >MAXR*/
	if(z->reg>MAXR&&l!=rp.r2){
	  emit(f,"\tmov\t[%s+],%s\n",regnames[l],regnames[rp.r1]);
	  emit(f,"\tmov\t[%s],%s\n",regnames[l],regnames[rp.r2]);
	}else{
	  emit(f,"\tmov\t[%s],%s\n",regnames[l],regnames[rp.r1]);
	  emit(f,"\tmov\t[%s+#2],%s\n",regnames[l],regnames[rp.r2]);
	}
	return;
      }else if(zr){
	if(!reg_pair(zr,&qp)) ierror(0);
	emit(f,"\tmov\t%s,%s\n",regnames[qp.r1],regnames[rp.r1]);
	emit(f,"\tmov\t%s,%s\n",regnames[qp.r2],regnames[rp.r2]);
	return;
      }else{
	if(!(z->flags&VAR)) ierror(0);
	if(z->v->storage_class==AUTO||z->v->storage_class==REGISTER){
	  l=voff(z);
	  if(l)
	    emit(f,"\tmov\t[%s+#%ld],%s\n",regnames[sp],l,regnames[rp.r1]);
	  else
	    emit(f,"\tmov\t[%s],%s\n",regnames[sp],regnames[rp.r1]);
	  emit(f,"\tmov\t[%s+#%ld],%s\n",regnames[sp],l+2,regnames[rp.r2]);
	  return;
	}else{
	  emit(f,"\tmov\t");emit_obj(f,z,t);emit(f,",%s\n",regnames[rp.r1]);
	  emit(f,"\tmov\t");emit_obj(f,z,t);emit(f,"+2,%s\n",regnames[rp.r2]);
	  return;
	}
      }
      ierror(0);
    }else{
      emit(f,"\tmov%s\t",x_t[t&NQ]);
      if(!z->am&&(z->flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&reg_pair(z->reg,&rp))
	emit(f,"[%s]",regnames[rp.SOF]);
      else if(zr&&t==CHAR)
	emit(f,"%s",bregnames[zr]);
      else
	emit_obj(f,z,t);
      emit(f,",%s\n",((t==CHAR)?bregnames[qr]:regnames[qr]));
      return;
    }
  }
  /*reg->reg*/
  if(t==CHAR){
    emit(f,"\tmovb\t%s,%s\n",bregnames[zr],bregnames[qr]);
  }else if(ISLWORD(t)){
    if(!reg_pair(qr,&qp)) ierror(0);
    if(!reg_pair(zr,&rp)) ierror(0);
    emit(f,"\tmov\t%s,%s\n",regnames[rp.r1],regnames[qp.r1]);
    emit(f,"\tmov\t%s,%s\n",regnames[rp.r2],regnames[qp.r2]);
  }else{
    emit(f,"\tmov\t%s,%s\n",regnames[zr],regnames[qr]);
  }
}

static int get_reg(FILE *f,struct IC *p)
{
  int i;
  for(i=1;i<=16;i++){
    if(!regs[i]&&regscratch[i]){
      BSET(regs_modified,i);
      return i;
    }
  }
  ierror(0);
}

static void save_result(FILE *f,int r,struct IC *p,int t)
/*  Saves result in register r to object o. May use tp or ti. */ 
{
  if((p->z.flags&(REG|DREFOBJ))==DREFOBJ){
    BSET(regs_modified,tp);
    if(ISLWORD(p->z.dtyp)){
      if(!ti2_used) tmp4=ti2; else tmp4=get_reg(f,p);
      BSET(regs_modified,tmp3);
      tmp3=tp;
      p->z.flags&=~DREFOBJ;
      move(f,&p->z,0,0,MTMP2,FPOINTER);
      p->z.flags=(DREFOBJ|REG);
      p->z.reg=MTMP2;
    }else if(!(p->z.flags&KONST)){
      int tmp=(r==tp)?ti:tp;
      BSET(regs_modified,tmp);
      p->z.flags&=~DREFOBJ;
      move(f,&p->z,0,0,tmp,NPOINTER);
      p->z.flags=(DREFOBJ|REG);
      p->z.reg=tmp;
    }
  }
  move(f,0,r,&p->z,0,t);
}

static int scratchreg(int r,struct IC *p)
{
  int c;
  if(r==tp||r==ti||r==ti2||r==MTMP1||r==MTMP2)
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

static char *longcmd(char *s)
{
  if(!strcmp(s,"add")) return "addc";
  if(!strcmp(s,"sub")) return "subc";
  /*FIXME*/
  return s;
}

/* perform long arithmetic using library functions */
/* the functions take arguments in r4/r5 and r10/r11 */
/* giving the result in r4/r5 */
/* returns , if the operation was performed, 0 if */
/* the IC should be handled by inline-code */
static int lib_larith(FILE *f,struct IC *p)
{
  int r4p,r5p,c=p->code,t=ztyp(p);char *s;
  if(regs[r4]&&(!isreg(z)||p->z.reg==r4r5)){
    emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[r4]);
    push(2,0);
    r4p=1;
  }else
    r4p=0;
  if(regs[r5]&&(!isreg(z)||p->z.reg==r4r5)){
    emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[r5]);
    push(2,0);
    r5p=1;
  }else
    r5p=0;
  if(!p->q1.am&&(p->q1.flags&DREFOBJ)){
    if(!(p->q1.flags&(REG))){
      if(ISLWORD(p->q1.dtyp)){
	tmp3=ti2;tmp4=ti;ti2_used=1;
	BSET(regs_modified,ti);
	BSET(regs_modified,ti2);
	p->q1.flags&=~DREFOBJ;
	move(f,&p->q1,0,0,MTMP2,FPOINTER);
	p->q1.reg=MTMP2;
	p->q1.flags=(REG|DREFOBJ);
      }else if(!(p->q1.flags&KONST)){
	BSET(regs_modified,ti);
	p->q1.flags&=~DREFOBJ;
	move(f,&p->q1,0,0,ti,NPOINTER);
	p->q1.reg=ti;
	p->q1.flags=(REG|DREFOBJ);
      }
    }
  }
  BSET(regs_modified,r4r5);
  move(f,&p->q1,0,0,r4r5,q1typ(p));
  if(!p->q2.am&&(p->q2.flags&DREFOBJ)){
    if(!(p->q2.flags&(REG))){
      if(ISLWORD(p->q2.dtyp)){
	BSET(regs_modified,ti); 
	BSET(regs_modified,ti2);
	tmp3=ti2;tmp4=ti;ti2_used=1;
	p->q2.flags&=~DREFOBJ;
	move(f,&p->q2,0,0,MTMP2,FPOINTER);
	p->q2.reg=MTMP2;
	p->q2.flags=(REG|DREFOBJ);
      }else if(!(p->q2.flags&KONST)){
	BSET(regs_modified,ti);
	p->q2.flags&=~DREFOBJ;
	move(f,&p->q2,0,0,ti,NPOINTER);
	p->q2.reg=ti;
	p->q2.flags=(REG|DREFOBJ);
      }
    }
  }  
  /* ti/ti2 must be r10/r11 */
  tmp3=ti;tmp4=ti2;ti2_used=1;
  BSET(regs_modified,ti); 
  BSET(regs_modified,ti2);
  move(f,&p->q2,0,0,MTMP2,q2typ(p));
  if(c==MULT)
    s="_mul";
  else if(c==DIV&&(t&UNSIGNED))
    s="_udil";
  else if(c==DIV&&!(t&UNSIGNED))
    s="_sdil";
  else if(c==MOD&&(t&UNSIGNED))
    s="_umol";
  else if(c==MOD&&!(t&UNSIGNED))
    s="_smol";
  else
    ierror(0);
  if(TASKING){
    emit(f,"\textern\t%s%s:%s\n",idprefix,s,TINY?"near":"far");
    emit(f,"\t%s\t%s%s\n",call,idprefix,s);
  }
  save_result(f,r4r5,p,ztyp(p));
  if(r5p){
    emit(f,"\tmov\t%s,[%s+]\n",regnames[r5],regnames[sp]);
    pop(2,0);
  }
  if(r4p){
    emit(f,"\tmov\t%s,[%s+]\n",regnames[r4],regnames[sp]);
    pop(2,0);
  }  
  return 1;
}

/****************************************/
/*  End of private fata and functions.  */
/****************************************/

int emit_peephole(void)
{
  int entries,i;long x,y,z;
  char *asmline[EMIT_BUF_DEPTH];
  i=emit_l;
  if(emit_f==0)
    entries=i-emit_f+1;
  else
    entries=EMIT_BUF_DEPTH;
  asmline[0]=emit_buffer[i];
  if(entries>=1){
    if(sscanf(asmline[0],"\tshl\tR%ld,#%ld\n",&x,&y)==2&&(y<0||y>15)){
      sprintf(asmline[0],"\tmov\tR%ld,#0\n",x);
      return 1;
    }
    if(sscanf(asmline[0],"\tshr\tR%ld,#%ld\n",&x,&y)==2&&(y<0||y>15)){
      sprintf(asmline[0],"\tmov\tR%ld,#0\n",x);
      return 1;
    }
  }
  if(entries>=2){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[1]=emit_buffer[i];
    if(sscanf(asmline[0],"\tadd\tR0,#%ld",&x)==1&&sscanf(asmline[1],"\tadd\tR0,#%ld",&y)==1){
      sprintf(asmline[1],"\tadd\tR0,#%ld\n",x+y);
      remove_asm();
      return 1;
    }
    if(sscanf(asmline[1],"\tmov\tR11,#%ld",&x)==1&&sscanf(asmline[0],"\texts\tR11,#%ld",&y)==1){
      sprintf(asmline[1],"\texts\t#%ld,#%ld\n",x,y);
      remove_asm();
      return 1;
    }
  }
  if(entries>=3){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[2]=emit_buffer[i];
    if(sscanf(asmline[2],"\tmov\tR1,#%ld",&x)==1&&sscanf(asmline[1],"\texts\t#%ld,#1",&y)==1&&sscanf(asmline[0],"\tmov\t[R1],R%ld",&z)==1){
      strcpy(asmline[2],asmline[1]);
      sprintf(asmline[1],"\tmov\t%ld,R%ld\n",x,z);
      remove_asm();
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
  maxalign=l2zm(2L);
  char_bit=l2zm(8L);
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i=1;i<=MAXR;i++){
    if(i<=16){
      regsize[i]=l2zm(2L);regtype[i]=&ityp;
    }else{
      regsize[i]=l2zm(4L);regtype[i]=&ltyp;
    }
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
  regsa[sp]=regsa[tp]=regsa[ti]=regsa[ti2]=1;
  regscratch[sp]=regscratch[tp]=regscratch[ti]=regscratch[ti2]=0;
  target_macros=marray;
  if(TINY) marray[0]="__TINY__";
  else if(LARGE) marray[0]="__LARGE__";
  else if(HUGE) marray[0]="__HUGE__";
  else marray[0]="__MEDIUM__";

  if(!TASKING){
    codename="\t.section\t.text,\"carx1\"\n";
    ndataname="\t.section\t.ndata,\"darw1\"\n";
    fdataname="\t.section\t.fdata,\"darw1\"\n";
    hdataname="\t.section\t.hdata,\"darw1\"\n";
    nbssname="\t.section\t.nbss,\"uarw1\"\n";
    fbssname="\t.section\t.fbss,\"uarw1\"\n";
    hbssname="\t.section\t.hbss,\"uarw1\"\n";
    ncdataname="\t.section\t.ncdata,\"dar1\"\n";
    fcdataname="\t.section\t.fcdata,\"dar1\"\n";
    hcdataname="\t.section\t.hcdata,\"dar1\"\n";
    bitsname="\t.section\t.bits,\"darw0\"\n";
    even="\t.align\t1\n";
    public=".global";
    comment="; ";
  }

  nvar.storage_class=STATIC;
  fvar.storage_class=STATIC;


  /* TODO: set argument registers */
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
  if(f==LLONG||f==DOUBLE||f==LDOUBLE)
    return 0;
  if(ISSCALAR(f)){
    if(f==LONG||f==FPOINTER||f==HPOINTER)
      return 20;
    else
      return 5;
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
  if(mode>0&&!ISPOINTER(t)) return 0;
  if(t==NPOINTER&&mode>0){
    if(r<=4) return 1; else return 0;
  }
  if(t<=CHAR&&r>8) return 0;
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
    case 17: p->r1=7;p->r2=8;break;
    case 18: p->r1=8;p->r2=9;break;
    case 19: p->r1=9;p->r2=10;break;
    case 20: p->r1=5;p->r2=6;break;
    case 21: p->r1=15;p->r2=16;break;
    case 22: p->r1=14;p->r2=15;break;
    case 23: p->r1=13;p->r2=14;break;
    case 24: p->r1=4;p->r2=5;break;
    case 25: p->r1=3;p->r2=4;break;
	
    /* pseudos */
    case MTMP1: p->r1=tmp1;p->r2=tmp2;break;
    case MTMP2: p->r1=tmp3;p->r2=tmp4;break;
    case ZEROS: return 0;
    case ONES: return 0;
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
#if 0
  emit(f,"\tds\t%ld\n",zm2l(size));
#endif
  /*FIXME: we currently do not use ds because of initialization */
  long l=zm2l(size);
  if(TASKING){
    if(t&&(t->flags&NQ)==BIT){
      emit(f,"\tdbit\n");
      return;
    }
    /*FIXME:alignment while(size>=4) {emit(f,"\tddw\t0\n");size-=4;}*/
    while(size--) emit(f,"\tdb\t0\n");
  }else{
    if(newobj&&section!=SPECIAL)
      emit(f,"%ld\n",zm2l(size));
    else
      emit(f,"\t.space\t%ld\n",zm2l(size));
    newobj=0; 
  }
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  if(section!=BITS&&!zmleq(align,l2zm(1L)))
    emit(f,even);
}
static void new_section(FILE *f,int nsec)
{
  if(!f||section==nsec) return;
  if(TASKING)
    emit(f,sec_end);
  section=nsec;
  if(nsec==HDATA){
    emit(f,hdataname);
    strcpy(sec_end,ehdata);
  }else if(nsec==FDATA){
    emit(f,fdataname);
    strcpy(sec_end,efdata);
  }else if(nsec==NDATA){
    emit(f,ndataname);
    strcpy(sec_end,endata);
  }else if(nsec==HCDATA){
    emit(f,hcdataname);
    strcpy(sec_end,ehcdata);
  }else if(nsec==FCDATA){
    emit(f,fcdataname);
    strcpy(sec_end,efcdata);
  }else if(nsec==NCDATA){
    emit(f,ncdataname);
    strcpy(sec_end,encdata);
  }else if(nsec==HBSS){
    emit(f,hbssname);
    strcpy(sec_end,ehbss);
  }else if(nsec==FBSS){
    emit(f,fbssname);
    strcpy(sec_end,efbss);
  }else if(nsec==NBSS){
    emit(f,nbssname);
    strcpy(sec_end,enbss);
  }else if(nsec==BITS){
    emit(f,bitsname);
    strcpy(sec_end,ebits);
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
    if(!TASKING){
      emit(f,"\t.type\t%s%ld,@object\n",labprefix,zm2l(v->offset));
      emit(f,"\t.size\t%s%ld,%ld\n",labprefix,zm2l(v->offset),zm2l(szof(v->vtyp)));
    }
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
    if(TASKING){
      emit(f,"%s%ld\tlabel\t",labprefix,zm2l(v->offset));
      emit_label_type(f,v);
      emit(f,"\n");
    }else{
      if(dppuse(v,section)>=0)
        emit(f,"\t.set\t%s%s%ld,%d\n",dppprefix,labprefix,zm2l(v->offset),dppuse(v,section));
      emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    }
  }
  if(v->storage_class==EXTERN){
    if(v->flags&(DEFINED|TENTATIVE)){
      if(!TASKING){
	emit(f,"\t.type\t%s%s,@object\n",idprefix,v->identifier);
	emit(f,"\t.size\t%s%s,%ld\n",idprefix,v->identifier,zm2l(szof(v->vtyp)));
      }
      emit(f,"\t%s\t%s%s\n",public,idprefix,v->identifier);
      if(!special_section(f,v)){
	if((v->vtyp->flags&NQ)==BIT){
	  if(f&&section!=BITS){
	    if(TASKING)
	      emit(f,sec_end);
	    strcpy(sec_end,ebits);
	    emit(f,bitsname);
	    section=BITS;	  
	  }
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
      if(TASKING){
	emit(f,"%s%s\tlabel\t",idprefix,v->identifier);
	emit_label_type(f,v);
	emit(f,"\n");
      }else{
        if(dppuse(v,section)>=0){
          emit(f,"\t.global\t%s%s%s\n",dppprefix,idprefix,v->identifier);
          emit(f,"\t.set\t%s%s%s,%d\n",dppprefix,idprefix,v->identifier,dppuse(v,section));
        }
	emit(f,"%s%s:\n",idprefix,v->identifier);
      }
    }else if(strcmp(v->identifier,"__va_start")&&!issfrv(v)&&!isesfrv(v)&&!issfrbitv(v)&&!isesfrbitv(v)){
      if(TASKING){
	emit(f,"\textern\t%s%s:",idprefix,v->identifier);
	emit_label_type(f,v);
	emit(f,"\n");
      }else{
	emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
      }
    }
  }
}

void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  if(ISPOINTER(t)){
    if(p->tree)
      emit(f,"\t%s\t",TASKING?dct[t&NQ]:vdct[t&NQ]); 
    if(ISLWORD(t))
      t=UNSIGNED|LONG;
    else
      t=UNSIGNED|SHORT;
    if(!p->tree)
      emit(f,"\t%s\t",TASKING?dct[t&NQ]:vdct[t&NQ]);
  }else{
    emit(f,"\t%s\t",TASKING?dct[t&NQ]:vdct[t&NQ]);
  }
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
      if(ISLWORD(t)){
	long l;
	eval_const(&p->val,t);
	l=zm2l(zmand(p->val.vmax,l2zm(65535L)));
	emit(f,"%ld",l);
	l=zm2l(zmand(zmrshift(p->val.vmax,l2zm(16L)),l2zm(65535L)));
	emit(f,",%ld",l);
      }else if((t&NQ)!=BIT)
	/*FIXME: initialization of bits impossible */
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
  int c,t,lastcomp=0,reg,short_add,bit_reverse,need_return=0;
  struct obj *bit_obj;char *bit_reg;
  static int idone;
  struct obj o,*cc=0;int cc_t;
  struct IC *p2;
  if(TINY){
    ret="ret";
    call="calla\tcc_uc,";
    jump="jmpa\tcc_uc,";
  }else{
    ret="rets";
    call="calls";
    jump="jmps\t";
  }
  if(v->tattr&INTERRUPT)
    ret="reti";
  if(DEBUG&1) printf("gen_code()\n");
  if(!v->fi) v->fi=new_fi();
  v->fi->flags|=ALL_REGS;
  if(f&&TASKING&&!idone){
    emit(f,"$EXTEND\n");
    emit(f,"$MODEL(SMALL)\n");
    idone=1;
  }
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
  loff=((zm2l(offset)+1)/2)*2;
  function_top(f,v,loff);
  usrstackoffset=sysstackoffset=notpopped=dontpop=usrmaxpushed=sysmaxpushed=0;
  sysstack=usrstack=0;
  for(;p;pr(f,p),p=p->next){
    if((p->q1.flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
      p->q1.flags=VAR;
      p->q1.v=ISLWORD(p->q1.dtyp)?&fvar:&nvar;
    }
    if((p->q2.flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
      p->q2.flags=VAR;
      p->q2.v=ISLWORD(p->q2.dtyp)?&fvar:&nvar;
    }
    if((p->z.flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
      p->z.flags=VAR;
      p->z.v=ISLWORD(p->z.dtyp)?&fvar:&nvar;
    }
    if(!TASKING){
      int rn,rb;char *rs;
      if(issfr(q1)){
	rs=strstr(p->q1.v->vattr,"sfr(");
	sscanf(rs+4,"%i",&rn);
	if(rn<=255) rn=0xfe00+2*rn;
	emit(f,"\t.sfr\t%s,%d\n",p->q1.v->identifier,rn);
      }
      if(issfr(q2)){
	rs=strstr(p->q2.v->vattr,"sfr(");
	sscanf(rs+4,"%i",&rn);
	if(rn<=255) rn=0xfe00+2*rn;
	emit(f,"\t.sfr\t%s,%d\n",p->q2.v->identifier,rn);
      }
      if(issfr(z)){
	rs=strstr(p->z.v->vattr,"sfr(");
	sscanf(rs+4,"%i",&rn);
	if(rn<=255) rn=0xfe00+2*rn;
	emit(f,"\t.sfr\t%s,%d\n",p->z.v->identifier,rn);
      }
      if(issfrbit(q1)){
	rs=strstr(p->q1.v->vattr,"sfrbit(");
	sscanf(rs+7,"%i,%i",&rn,&rb);
	if(rn<=255) rn=0xfe00+2*rn;
	emit(f,"\t.sfr\t%s,%d,%d\n",p->q1.v->identifier,rn,rb);
      }
      if(issfrbit(q2)){
	rs=strstr(p->q2.v->vattr,"sfrbit(");
	sscanf(rs+7,"%i,%i",&rn,&rb);
	if(rn<=255) rn=0xfe00+2*rn;
	emit(f,"\t.sfr\t%s,%d,%d\n",p->q2.v->identifier,rn,rb);
      }
      if(issfrbit(z)){
	rs=strstr(p->z.v->vattr,"sfrbit(");
	sscanf(rs+7,"%i,%i",&rn,&rb);
	if(rn<=255) rn=0xfe00+2*rn;
	emit(f,"\t.sfr\t%s,%d,%d\n",p->z.v->identifier,rn,rb);
      }
      if(isesfr(q1)){
	rs=strstr(p->q1.v->vattr,"sfre(");
	sscanf(rs+5,"%i",&rn);
	if(rn<=255) rn=0xf000+2*rn;
	emit(f,"\t.equ\t%s,%d\n",p->q1.v->identifier,rn);
      }
      if(isesfr(q2)){
	rs=strstr(p->q2.v->vattr,"sfre(");
	sscanf(rs+5,"%i",&rn);
	if(rn<=255) rn=0xf000+2*rn;
	emit(f,"\t.equ\t%s,%d\n",p->q2.v->identifier,rn);
      }
      if(isesfr(z)){
	rs=strstr(p->z.v->vattr,"sfre(");
	sscanf(rs+5,"%i",&rn);
	if(rn<=255) rn=0xf000+2*rn;
	emit(f,"\t.equ\t%s,%d\n",p->z.v->identifier,rn);
      }
      if(isesfrbit(q1)){
	rs=strstr(p->q1.v->vattr,"sfrbite(");
	sscanf(rs+8,"%i,%i",&rn,&rb);
	if(rn<=255) rn=0xf000+2*rn;
	emit(f,"\t.sfr\t%s,%d,%d\n",p->q1.v->identifier,rn,rb);
      }
      if(isesfrbit(q2)){
	rs=strstr(p->q2.v->vattr,"sfrbite(");
	sscanf(rs+8,"%i,%i",&rn,&rb);
	if(rn<=255) rn=0xf000+2*rn;
	emit(f,"\t.sfr\t%s,%d,%d\n",p->q2.v->identifier,rn,rb);
      }
      if(isesfrbit(z)){
	rs=strstr(p->z.v->vattr,"sfrbite(");
	sscanf(rs+8,"%i,%i",&rn,&rb);
	if(rn<=255) rn=0xf000+2*rn;
	emit(f,"\t.sfr\t%s,%d,%d\n",p->z.v->identifier,rn,rb);
      }      
    }

    c=p->code;t=p->typf;
    ti2_used=0; short_add=0;
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
	emit(f,"\tadd\t%s,#%ld\n",regnames[sp],notpopped);
	pop(notpopped,0);notpopped=0;cc=0;
      }
    }
    if(c==LABEL) {cc=0;emit(f,"%s%d:\n",labprefix,t);continue;}
    if(c>=BEQ&&c<=BGT&&t==exit_label) need_return=1;
    if(c==BRA){
      if(p->typf==exit_label&&!have_frame){
	emit(f,"\t%s\n",ret);
      }else{
	if(t==exit_label) need_return=1;
	emit(f,"\tjmpr\tcc_uc,%s%d\n",labprefix,t);
      }
      cc=0;continue;
    }
    if(c==BEQ||c==BNE){
      if(p->ext.flags==EXT_IC_BTST){
	if(bit_reg){
	  emit(f,"\tj%sb\t%s.%ld,%s%d\n",(c==BEQ)?"n":"",bit_reg,p->ext.offset,labprefix,t);
	}else{
	  if(!bit_obj) ierror(0);
	  emit(f,"\tj%sb\t",(c==BEQ)?"n":"");
	  emit_obj(f,bit_obj,t);emit(f,".%ld,%s%d\n",p->ext.offset,labprefix,t);
	}
      }else if(lastcomp==BIT){
	if(c==BEQ) bit_reverse=1-bit_reverse;
	emit(f,"\tj%sb\t",(bit_reverse==1)?"n":"");
	if(bit_obj)
	  emit_obj(f,bit_obj,t);
	else
	  emit(f,"%s",bit_reg);
	emit(f,",%s%d\n",labprefix,t);
      }else
	emit(f,"\tjmpr\tcc_%s,%s%d\n",ccs[c-BEQ],labprefix,t);
      cc=0;continue;
    }
    if(c>BNE&&c<BRA){      
      if(p->ext.flags==EXT_IC_BTST||lastcomp==BIT) ierror(0);
      if(lastcomp&UNSIGNED) emit(f,"\tjmpr\tcc_u%s,%s%d\n",ccs[c-BEQ],labprefix,t);
      else              emit(f,"\tjmpr\tcc_s%s,%s%d\n",ccs[c-BEQ],labprefix,t);
      cc=0;continue;
    }
    if(c==MOVETOREG){
      move(f,&p->q1,0,0,p->z.reg,SHORT);
      cc=&p->q1;cc_t=SHORT;continue;
    }
    if(c==MOVEFROMREG){
      move(f,0,p->q1.reg,&p->z,0,SHORT);
      cc=&p->z;cc_t=SHORT;continue;
    }
    
    if((t&NQ)==DOUBLE) {pric2(stdout,p);ierror(0);}
    if((t&NQ)==BIT){
      cc=0;
      if(c==ASSIGN){
	if(!isreg(z)&&((p->z.flags&(VAR|REG|DREFOBJ|VARADR))!=VAR||!ISSTATIC(p->z.v))) ierror(0);
	move(f,&p->q1,0,&p->z,0,t);
	continue;
      }
      if(c==COMPARE){
	if(!isconst(q2)){
	  if(!isreg(q2)&&((p->q2.flags&(VAR|REG|DREFOBJ|VARADR))!=VAR||!ISSTATIC(p->q2.v))) ierror(0);
	  bit_reg="PSW.1";bit_obj=0;lastcomp=BIT;bit_reverse=1;
	  if(isesfrbit(q1)){
	    if(!isesfrbit(q2)&&!isreg(q2)){
	      move(f,&p->q1,0,0,ti,t);
	      emit(f,"\tbcmp\t%s.0,",regnames[ti]);
	      emit_obj(f,&p->q2,t);emit(f,"\n");
	      continue;
	    }else
	      emit(f,"\textr\t#1\n");
	  }else if(isesfrbit(q2)){
	    if(!isreg(q1)){
	      move(f,&p->q2,0,0,ti,t);
	      emit(f,"\tbcmp\t");
	      emit_obj(f,&p->q1,t);
	      emit(f,",%s.0\n",regnames[ti]);
	      continue;
	    }else
	      emit(f,"\textr\t#1\n");
	  }
	  emit(f,"\tbcmp\t");emit_obj(f,&p->q1,t);
	  emit(f,",");emit_obj(f,&p->q2,t);emit(f,"\n");
	}else{
	  bit_reg=0;bit_obj=&p->q1;lastcomp=BIT;
	  eval_const(&p->q2.val,t);
	  if(ISNULL())
	    bit_reverse=0;
	  else
	    bit_reverse=1;
	  if(isesfrbit(q1)) emit(f,"\textr\t#1\n");
	}
	continue;
      }
      if(c==TEST){
	bit_reg=0;bit_obj=&p->q1;bit_reverse=0;lastcomp=BIT;
	if(isesfrbit(q1)) emit(f,"\textr\t#1\n");
	continue;
      }
      if(c==AND||c==OR||c==XOR){
	char *s;
	if(compare_objects(&p->z,&p->q2)){
	  struct obj m;
	  m=p->q1;p->q1=p->q2;p->q2=m;
	}
	if(!compare_objects(&p->q1,&p->z))
	  move(f,&p->q1,0,&p->z,0,t);
	/*FIXME: const, esfr etc. */
	if(c==AND) s="band";
	else if(c==OR) s="bor";
	else s="bxor";
	if(isesfrbit(q2)&&!isesfrbit(z)&&!isreg(z)){
	  move(f,&p->q2,0,0,ti,t);
	  emit(f,"\t%s\t",s);
	  emit_obj(f,&p->z,t);
	  emit(f,",%s.0\n",regnames[ti]);
	}else if(isesfrbit(z)&&!isesfrbit(q2)&&!isreg(q2)){
	  move(f,&p->q2,0,0,ti,t);
	  emit(f,"\textr\t#1\n");
	  emit(f,"\t%s\t",s);
	  emit_obj(f,&p->z,t);
	  emit(f,",%s.0\n",regnames[ti]);
	}else{
	  if(isesfrbit(z)) emit(f,"\textr\t#1\n");
	  emit(f,"\t%s\t",s);
	  emit_obj(f,&p->z,t);emit(f,",");
	  emit_obj(f,&p->q2,t);emit(f,"\n");
	}
	continue;
      }
      if(c!=CONVERT) ierror(0);
    }

    if(c==TEST){
      lastcomp=t;
      if(p->ext.flags==EXT_IC_CMPIA){
	if(p->ext.offset<0)
	  emit(f,"\tsub\t%s,#%ld\n",regnames[p->ext.r],-p->ext.offset);
	else
	  emit(f,"\tadd\t%s,#%ld\n",regnames[p->ext.r],p->ext.offset);
	cc=0;continue;
      }
      if(p->ext.flags==EXT_IC_CMPIB){
	if(p->ext.offset<0)
	  emit(f,"\tcmpd%ld\t%s,#0\n",-p->ext.offset,regnames[p->ext.r]);
	else
	  emit(f,"\tcmpi%ld\t%s,#0\n",p->ext.offset,regnames[p->ext.r]);
	cc=0;continue;
      }
      if(cc&&(cc_t&NU)==(t&NU)&&compare_objects(cc,&p->q1)){
	continue;
      }
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
    if(c==COMPARE&&p->ext.flags==EXT_IC_CMPIA){
      long l;
      if(!isconst(q2)) ierror(0);
      eval_const(&p->q2.val,t);
      l=zm2l(vmax);
      if(p->ext.offset<0)
	emit(f,"\tcmpd%ld\t%s,#%ld\n",-p->ext.offset,regnames[p->ext.r],l-p->ext.offset);
      else
	emit(f,"\tcmpi%ld\t%s,#%ld\n",p->ext.offset,regnames[p->ext.r],l-p->ext.offset);
      cc=0;lastcomp=t;continue;
    }
    if(c==COMPARE&&p->ext.flags==EXT_IC_CMPIB){
pric2(stdout,p);
      if(p->ext.offset<0)
	emit(f,"\tcmpd%ld\t%s,",-p->ext.offset,regnames[p->ext.r]);
      else
	emit(f,"\tcmpi%ld\t%s,",p->ext.offset,regnames[p->ext.r]);
      emit_obj(f,&p->q2,t);emit(f,"\n");
      cc=0;lastcomp=t;continue;
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
    /* try to avoid z==q2 */
    if((c==ADD||c==AND||c==OR||c==XOR||(c==ADDI2P&&!short_add))&&compare_objects(&p->q2,&p->z)){
      o=p->q1;p->q1=p->q2;p->q2=o;
    }    
    /* DREFOBJs nach q2, um evtl. op reg,[ri] zu nutzen */
    if(c==ADD||c==MULT||c==OR||c==AND||c==XOR){
      if(isreg(q2)&&scratchreg(p->q2.reg,p)){
	o=p->q1;p->q1=p->q2;p->q2=o;
      }
      if((p->q1.flags&DREFOBJ)&&(!(p->q1.flags&REG)||p->q1.reg<=4)&&(!p->q1.am||p->q1.am->flags!=IMM_IND)){
	if(!((p->q2.flags&DREFOBJ)&&(!p->q2.am||p->q2.am->flags!=IMM_IND))){
	  struct obj o;
	  o=p->q1;p->q1=p->q2;p->q2=o;
	}
      }
    }
    /*FIXME: ICs mit mehreren Typen*/
    if(switch_IC(p)&&isreg(z)){
      reg=p->z.reg;
    }else if(isreg(q1)&&(!ISLWORD(t)||reg_pair(p->q1.reg,&rp))&&(scratchreg(p->q1.reg,p)||(isreg(z)&&p->z.reg==p->q1.reg)||(!ISLWORD(t)&&c==COMPARE))){
      reg=p->q1.reg;
    }else{
      if(ISLWORD(ztyp(p))){
	tmp1=ti;tmp2=ti2;ti2_used=1;
	BSET(regs_modified,ti);
	BSET(regs_modified,ti2);
	reg=MTMP1;
      }else{
	if((t&NQ)==CHAR){
	  BSET(regs_modified,tp);
	  reg=tp;
	}else{
	  BSET(regs_modified,ti);
	  reg=ti;
	}
      }
    }
    /* op reg,mem/const */
    if(issfr(z)&&(c==ADD||c==SUB||c==AND||c==OR||c==XOR||c==ADDI2P)&&(compare_objects(&p->q1,&p->z)||((c!=SUB&&c!=SUBIFP)&&compare_objects(&p->q2,&p->z)))){
      char *s;
      if(!compare_objects(&p->q1,&p->z)){o=p->q1;p->q1=p->q2;p->q2=o;}
      if((p->q2.flags&(KONST|DREFOBJ))==KONST||((p->q2.flags&(VAR|DREFOBJ))==VAR&&ISSTATIC(p->q2.v))){
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
	if((p->q2.flags&VAR)&&ISFAR(p->q2.v)){
	  emit(f,"\texts\t#SEG ");isseg=1;emit_obj(f,&p->q2,t);emit(f,",#1\n");
	}
	emit(f,"\t%s%s\t",s,(t&NQ)==CHAR?"b":"");
	emit_obj(f,&p->q1,t);
	emit(f,",");
	emit_obj(f,&p->q2,t);
	emit(f,"\n");
	cc=&p->z;cc_t=t;
	continue;
      }
    }
    /* op mem,reg */
    if((c==ADD||c==SUB||c==AND||c==OR||c==XOR||c==ADDI2P)&&(compare_objects(&p->q1,&p->z)||((c!=SUB&&c!=SUBIFP)&&compare_objects(&p->q2,&p->z)))&&(p->z.flags&(VAR|VARADR|DREFOBJ|REG))==VAR&&ISSTATIC(p->z.v)){
      char *s;
      if(!compare_objects(&p->q1,&p->z)){o=p->q1;p->q1=p->q2;p->q2=o;}
      cc=&p->z;cc_t=t;
      if(isreg(q2)){
	reg=p->q2.reg;
      }else if(issfr(q2)){
	reg=0;
      }else if(isconst(q2)){
	eval_const(&p->q2.val,t);
	if(ISLWORD(t)){
	  unsigned long ul;
	  reg=MTMP1;
	  if((c==ADD||c==SUB||c==ADDI2P||c==SUBIFP)&&zumeqto(vumax,ul2zum(1UL))){
	    if(c==ADD) c=SUB;
	    else if(c==SUB) c=ADD;
	    else if(c==ADDI2P) c=SUBIFP;
	    else c=ADDI2P;
	    tmp1=ONES;tmp2=ONES;
	  }else{
	    ul=zum2ul(zumand(vumax,ul2zum(65535UL)));
	    if(ul==0) tmp1=ZEROS;
	    else if(ul==0xffff) tmp1=ONES;
	    else{
	      BSET(regs_modified,ti);
	      tmp1=ti;
	      emit(f,"\tmov\t%s,#%lu\n",regnames[ti],ul);
	    }
	    ul=zum2ul(zumand(zumrshift(vumax,ul2zum(16UL)),ul2zum(65535UL)));
	    if(ul==0) tmp2=ZEROS;
	    else if(ul==0xffff) tmp2=ONES;
	    else{
	      BSET(regs_modified,ti2);
	      tmp2=ti2;
	      emit(f,"\tmov\t%s,#%lu\n",regnames[ti2],ul);
	    }
	  }
	}else{
	  long l;
	  if(c==OR&&(l=pof2(vumax))&&ISBADDR(p->z.v)){
	    emit(f,"\tbset\t");emit_obj(f,&p->z,t);
	    emit(f,".%ld\n",l-1);
	    cc=0;continue;
	  }else if(c==AND&&(l=pof2(zumkompl(vumax)))&&ISBADDR(p->z.v)){
	    emit(f,"\tbclr\t");emit_obj(f,&p->z,t);
	    emit(f,".%ld\n",l-1);
	    cc=0;continue;
	  }else if((c==ADD||c==SUB||c==ADDI2P||c==SUBIFP)&&zumeqto(vumax,ul2zum(1UL))){
	    if(c==ADD) c=SUB; 
	    else if(c==SUB) c=ADD;
	    else if(c==ADDI2P) c=SUBIFP;
	    else c=ADDI2P;
	    reg=ONES;
	  }else{
	    if(zumeqto(vumax,ul2zum(0UL))) reg=ZEROS;
	    else if(zumeqto(vumax,ul2zum(0xffffUL))) reg=ONES;
	    else{
	      BSET(regs_modified,tp);
	      reg=tp;
	      move(f,&p->q2,0,0,reg,t);
	    }
	  }
	}
      }else{
	/*FIXME:ones/zeros nutzen*/
	if(ISLWORD(t)){
	  tmp1=ti;tmp2=ti2;
	  BSET(regs_modified,ti);
	  BSET(regs_modified,ti2);
	  reg=MTMP1;
	}else{
	  BSET(regs_modified,tp);
	  reg=tp;
	}
	if((p->q2.flags&(DREFOBJ|REG))==DREFOBJ){
	  if(ISLWORD(p->q2.dtyp)){
	    tmp3=ti2;tmp4=ti;
	    BSET(regs_modified,ti);
	    BSET(regs_modified,ti2);
	    p->q2.flags&=~DREFOBJ;
	    move(f,&p->q2,0,0,MTMP2,HPOINTER);
	    p->q2.flags=(REG|DREFOBJ);p->q2.reg=MTMP2;
	  }else if(!(p->q2.flags&KONST)){
	    BSET(regs_modified,ti);
	    move(f,&p->q2,0,0,ti,NPOINTER);
	    p->q2.flags=REG;p->q2.reg=ti;
	  }
	}
	move(f,&p->q2,0,0,reg,t);
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
      if(reg&&reg_pair(reg,&rp)){
	if(ISFAR(p->z.v)){
	  emit(f,"\texts\t#SEG ");isseg=1;emit_obj(f,&p->z,t);emit(f,",#2\n");
	}
	emit(f,"\t%s\t",s);
	emit_obj(f,&p->q1,t);emit(f,",%s\n",regnames[rp.r1]);
	s=longcmd(s);
	p->q1.val.vmax=zmadd(p->q1.val.vmax,l2zm(2L));
	emit(f,"\t%s\t",s);
	emit_obj(f,&p->q1,t);emit(f,",%s\n",regnames[rp.r2]);
	p->q1.val.vmax=zmsub(p->q1.val.vmax,l2zm(2L));
      }else{
	if(ISFAR(p->z.v)){
	  emit(f,"\texts\t#SEG ");isseg=1;emit_obj(f,&p->z,t);emit(f,",#1\n");
	}
	emit(f,"\t%s%s\t",s,(t&NQ)==CHAR?"b":"");
	emit_obj(f,&p->q1,t);
	if(reg)
	  emit(f,",%s\n",((t&NQ)==CHAR&&reg<=MAXR)?bregnames[reg]:regnames[reg]);
	else{
	  emit(f,",");
	  emit_obj(f,&p->q2,t);
	  emit(f,"\n");
	}
      }
      cc=&p->z;cc_t=t;
      continue;
    }
    if(p->ext.flags==EXT_IC_BTST){
      if(isreg(q1)){bit_reg=regnames[p->q1.reg];continue;}
      if((p->q1.flags&(VAR|VARADR|DREFOBJ|REG))==VAR&&ISSTATIC(p->q1.v)&&ISBADDR(p->q1.v)){
	bit_reg=0;bit_obj=&p->q1;
	continue;
      }
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
    if(ISLWORD(t)&&(c==MULT||c==DIV||c==MOD)){
      if(lib_larith(f,p)) continue;
    }

    if(!p->q1.am&&(p->q1.flags&DREFOBJ)){
      if(!(p->q1.flags&(REG))){
	if(ISLWORD(p->q1.dtyp)){
	  tmp3=ti2;tmp4=ti;ti2_used=1;
	  BSET(regs_modified,ti);
	  BSET(regs_modified,ti2);
	  p->q1.flags&=~DREFOBJ;
	  move(f,&p->q1,0,0,MTMP2,FPOINTER);
	  p->q1.reg=MTMP2;
	  p->q1.flags=(REG|DREFOBJ);
	}else if(!(p->q1.flags&KONST)){
	  BSET(regs_modified,ti);
	  p->q1.flags&=~DREFOBJ;
	  move(f,&p->q1,0,0,ti,NPOINTER);
	  p->q1.reg=ti;
	  p->q1.flags=(REG|DREFOBJ);
	}
      }
      if(p->q2.flags){
	move(f,&p->q1,0,0,reg,q2typ(p));/*FIXME*/
	if((!isreg(q1)||p->q1.reg!=reg)&&c==COMPARE&&!ISLWORD(t)&&isconst(q2)){
	  /* avoid cmp, if not needed */
	  eval_const(&p->q2.val,t);
	  if(ISNULL()){
	    lastcomp=t;continue;
	  }
	}
      }
    }else{
      if(p->q2.flags){
	move(f,&p->q1,0,0,reg,q1typ(p));/*FIXME*/
	if((!isreg(q1)||p->q1.reg!=reg)&&c==COMPARE&&!ISLWORD(t)&&isconst(q2)){
	  /* avoid cmp, if not needed */
	  eval_const(&p->q2.val,t);
	  if(ISNULL()){
	    lastcomp=t;continue;
	  }
	}
      }
    }
    if(p->ext.flags==EXT_IC_BTST){ bit_reg=regnames[reg];continue;}
    if(!p->q2.am&&(p->q2.flags&DREFOBJ)){
      if(!(p->q2.flags&(REG))||p->q2.reg>4){
	if(ISLWORD(p->q2.dtyp)){
	  if(ti2_used) tmp4=get_reg(f,p); else tmp4=ti2;
	  tmp3=tp;
	  BSET(regs_modified,tmp3);
	  BSET(regs_modified,tmp4);
	  p->q2.flags&=~DREFOBJ;
	  move(f,&p->q2,0,0,MTMP2,FPOINTER);
	  p->q2.reg=MTMP2;
	  p->q2.flags=(REG|DREFOBJ);
	}else if(!(p->q2.flags&KONST)){
	  BSET(regs_modified,tp);
	  p->q2.flags&=~DREFOBJ;
	  move(f,&p->q2,0,0,tp,NPOINTER);
	  p->q2.flags=(REG|DREFOBJ);
	  p->q2.reg=tp;
	}
      }
    }
    if(c==CONVERT&&!must_convert(p->typf,p->typf2,0)){
      p->code=c=ASSIGN;
      p->q2.val.vmax=sizetab[p->typf&NQ];
    }

    if(c==CONVERT){
      int to=p->typf2&NU;
      if(to==INT) to=SHORT;
      if(to==(UNSIGNED|INT)||to==NPOINTER) to=(UNSIGNED|SHORT);
      if(to==FPOINTER||to==HPOINTER) to=(UNSIGNED|LONG);
      if((t&NU)==INT) t=SHORT;
      if((t&NU)==(UNSIGNED|INT)||(t&NU)==NPOINTER) t=(UNSIGNED|SHORT);
      if((t&NQ)==FPOINTER||(t&NQ)==HPOINTER) t=(UNSIGNED|LONG);
      if((to&NQ)<=LONG&&(t&NQ)<=LONG){
	if((to&NQ)<=(t&NQ)){
	  if((to&NQ)==(t&NQ)) ierror(0);
	  if((to&NQ)==BIT){
	    cc=0;
	    if((p->q1.flags&(VAR|REG|DREFOBJ|VARADR))!=VAR||!ISSTATIC(p->q1.v)) ierror(0);
	    if(reg_pair(reg,&rp)){
	      emit(f,"\tmov\t%s,#0\n",regnames[rp.r1]);
	      emit(f,"\tmov\t%s,#0\n",regnames[rp.r2]);
	      emit(f,"\tbmov\t%s.0,",regnames[rp.r1]);
	      emit_obj(f,&p->q1,t);emit(f,"\n");
	    }else{
	      emit(f,"\tmov\t%s,#0\n",regnames[reg]);
	      emit(f,"\tbmov\t%s.0,",regnames[reg]);
	    }
	    emit_obj(f,&p->q1,t);emit(f,"\n");
	    save_result(f,reg,p,t);
	    continue;
	  }
	  if((p->q1.flags&(VAR|REG|VARADR))==VAR&&ISSTATIC(p->q1.v)&&ISFAR(p->q1.v)){
	    emit(f,"\texts\t#SEG ");isseg=1;emit_obj(f,&p->q1,t);
	    emit(f,",#1\n");
	  }
	  qp.SEG=0;
	  if(!p->q1.am&&(p->q1.flags&DREFOBJ)&&reg_pair(p->q1.reg,&qp))
	    emit(f,"\texts\t%s,#1\n",regnames[qp.SEG]);
	  if(p->q1.am&&reg_pair(p->q1.am->base,&qp))
	    emit(f,"\texts\t%s,#1\n",regnames[qp.SEG]);
	  if(reg_pair(reg,&rp)){
	    cc=0;
	    if((to&NQ)==CHAR){
	      if(isreg(q1)||((p->q1.flags&(VAR|VARADR|DREFOBJ))==VAR&&ISSTATIC(p->q1.v))){
		emit(f,"\tmovb%c\t%s,",(to&UNSIGNED)?'z':'s',regnames[rp.r1]);
		if((to&NQ)==CHAR&&isreg(q1)) emit(f,"%s",bregnames[p->q1.reg]);
		else if(qp.SEG) emit(f,"[%s]",regnames[qp.SOF]);
		else emit_obj(f,&p->q1,to);
		emit(f,"\n");
	      }else{
		BSET(regs_modified,tp);
		emit(f,"\tmovb\t%s,",bregnames[tp]);
		if(isreg(q1)) emit(f,"%s",bregnames[p->q1.reg]);
		else if(!p->q1.am&&qp.SEG) emit(f,"[%s]",regnames[qp.SOF]);
		else emit_obj(f,&p->q1,to);
		emit(f,"\n");
		emit(f,"\tmovb%c\t%s,%s\n",(to&UNSIGNED)?'z':'s',regnames[rp.r1],bregnames[tp]);
	      }
	    }else{
	      emit(f,"\tmov\t%s,",regnames[rp.r1]);
	      if(qp.SEG) emit(f,"[%s]",regnames[qp.SOF]);
	      else emit_obj(f,&p->q1,to);
	      emit(f,"\n");
	    }
	    if(to&UNSIGNED)
	      emit(f,"\tmov\t%s,#0\n",regnames[rp.r2]);
	    else
	      emit(f,"\tmov\t%s,%s\n\tashr\t%s,#15\n",regnames[rp.r2],regnames[rp.r1],regnames[rp.r2]);

	  }else{
	    cc=&p->z;cc_t=t;
	    if(isreg(q1)||((p->q1.flags&(VAR|VARADR|DREFOBJ))==VAR&&ISSTATIC(p->q1.v))){
	      emit(f,"\tmovb%c\t%s,",(to&UNSIGNED)?'z':'s',regnames[reg]);
	      if((to&NQ)==CHAR&&isreg(q1)) emit(f,"%s",bregnames[p->q1.reg]);
	      else if(qp.SEG) emit(f,"[%s]",regnames[qp.SOF]);
	      else emit_obj(f,&p->q1,to);
	      emit(f,"\n");
	    }else{
	      BSET(regs_modified,tp);
	      emit(f,"\tmovb\t%s,",bregnames[tp]);
	      if(isreg(q1)) emit(f,"%s",bregnames[p->q1.reg]);
	      else if(!p->q1.am&&qp.SEG) emit(f,"[%s]",regnames[qp.SOF]);
	      else emit_obj(f,&p->q1,to);
	      emit(f,"\n");
	      emit(f,"\tmovb%c\t%s,%s\n",(to&UNSIGNED)?'z':'s',regnames[reg],bregnames[tp]);
	    }	    
	  }
	  save_result(f,reg,p,t);
	  continue;
	}else{
	  if((t&NQ)==BIT){
	    if(!isreg(z)&&((p->z.flags&(VAR|REG|DREFOBJ|VARADR))!=VAR||!ISSTATIC(p->z.v))) ierror(0);
	    if(isreg(q1)){
	      reg=p->q1.reg;
	    }else{
	      if(ISLWORD(to)){
		tmp1=ti;tmp2=ti2;
		BSET(regs_modified,ti);
		BSET(regs_modified,ti2);
		reg=MTMP1;
	      }
	    }
	    move(f,&p->q1,0,0,reg,to);
	    if(reg_pair(reg,&rp)){
	      if(isreg(q1)&&!scratchreg(reg,p)){
		BSET(regs_modified,ti);
		emit(f,"\tmov\t%s,%s\n",regnames[ti],regnames[rp.r1]);
		emit(f,"\tor\t%s,%s\n",regnames[ti],regnames[rp.r2]);
	      }else
		emit(f,"\tor\t%s,%s\n",regnames[rp.r1],regnames[rp.r2]);
	    }else{
	      if(isreg(q1)) emit(f,"\tcmp%s\t%s,#0\n",(to&NQ)==CHAR?"b":"",(to&NQ)==CHAR?bregnames[reg]:regnames[reg]);
	    }
	    emit(f,"\tbmovn\t");
	    if(isesfrbit(z))
	      emit(f,"%s.0",regnames[ti]);
	    else
	      emit_obj(f,&p->z,t);
	    emit(f,",PSW.3\n");
	    if(isesfrbit(z)){
	      emit(f,"\textr\t#1\n");
	      emit(f,"\tbmov\t");
	      emit_obj(f,&p->z,t);
	      emit(f,",%s.0\n",regnames[ti]);
	    }
	    cc=0;continue;
	  }
	  cc=&p->z;cc_t=t;
	  if(ISLWORD(to)){
	    if(isreg(q1)) {reg_pair(p->q1.reg,&rp);p->q1.reg=rp.r1;}
	    to=SHORT;
	  }
	  if(isreg(q1)&&regok(p->q1.reg,t,0)) reg=p->q1.reg;
	  if(isreg(z)) reg=p->z.reg;
	  if(!regok(reg,t,0)){
	    reg=tp;
	    BSET(regs_modified,tp);
	  }
	  move(f,&p->q1,0,0,reg,to);
	  save_result(f,reg,p,t);
	}
	continue;
      }
      ierror(0);
    }
    if(c==MINUS||c==KOMPLEMENT){
      move(f,&p->q1,0,0,reg,t);
      if(reg_pair(reg,&rp)){
	emit(f,"\t%s\t%s\n",(c==MINUS?"neg":"cpl"),regnames[rp.r1]);
	if(c==MINUS) emit(f,"\taddc\t%s,#0\n",regnames[rp.r2]);
	emit(f,"\t%s\t%s\n",(c==MINUS?"neg":"cpl"),regnames[rp.r2]);
	cc=0;
      }else{
	emit(f,"\t%s%s\t%s\n",(c==MINUS?"neg":"cpl"),x_t[t&NQ],regnames[reg]);
	cc=&p->z;cc_t=t;
      }
      save_result(f,reg,p,t);
      continue;
    }
    if(c==SETRETURN){
      if(p->z.reg){
	move(f,&p->q1,0,0,p->z.reg,t);
	BSET(regs_modified,p->z.reg);
      }
      cc=0; /* probably not needed */
      continue;
    }
    if(c==GETRETURN){
      if(p->q1.reg){
	if(!isreg(z)||p->z.reg!=p->q1.reg){ cc=&p->z;cc_t=t;}
	save_result(f,p->q1.reg,p,t);
      }
      continue;
    }
    if(c==CALL){
      int reg,jmp=0;long csstack=0,custack=0;
      cc=0;
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp("__va_start",p->q1.v->identifier)){
	long va_off=loff-usrstackoffset+pushedsize+zm2l(va_offset(v));
	emit(f,"\tmov\t%s,%s\n",regnames[r4],regnames[sp]);
	if(va_off)
	  emit(f,"\tadd\t%s,#%ld\n",regnames[r4],va_off);
	BSET(regs_modified,r4);
	if(LARGE||HUGE){
	  emit(f,"\tmov\t%s,#0\n",regnames[r5]);
	  BSET(regs_modified,r5);
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
	    if(p->call_list[i].v->fi->stack1>custack) custack=p->call_list[i].v->fi->stack1;
	    if(p->call_list[i].v->fi->stack2>csstack) csstack=p->call_list[i].v->fi->stack2;
          }else{
            err_ic=p;if(f) error(317,p->call_list[i].v->identifier);
            stack_valid=0;
          }
        }
      }
      if(!calc_regs(p,f!=0)&&v->fi) v->fi->flags&=~ALL_REGS;
      if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
	emit_inline_asm(f,p->q1.v->fi->inline_asm);
	callee_push(custack,csstack);
      }else{
	if(usrstackoffset==0&&!have_frame&&!(v->tattr&INTERRUPT)){
	  struct IC *p2;
	  jmp=1;
	  for(p2=p->next;p2;p2=p2->next){
	    if(p2->code!=FREEREG&&p2->code!=ALLOCREG&&p2->code!=LABEL&&
	       (p2->code!=GETRETURN||(p2->z.flags&(REG|DREFOBJ))!=REG||p2->q1.reg!=p2->z.reg)&&
	       (p2->code!=SETRETURN||(p2->q1.flags&(REG|DREFOBJ))!=REG||p2->q1.reg!=p2->z.reg)){
	      jmp=0;break;
	    }
	  }
	}
	if(p->q1.flags&DREFOBJ){
	  int clabel=++label;
	  if(ISLWORD(p->q1.dtyp)){
	    int tmp;
	    if(!ti2_used) tmp=ti2; else tmp=tp;
	    BSET(regs_modified,tmp);
	    emit(f,"\tmov\t%s,#SEG %s%d\n",regnames[tmp],labprefix,clabel);
	    emit(f,"\tpush\t%s\n",regnames[tmp]);
	    emit(f,"\tmov\t%s,#SOF %s%d\n",regnames[tmp],labprefix,clabel);
	    emit(f,"\tpush\t%s\n",regnames[tmp]);
	    push(0,4);
	  }
	  if(!(p->q1.flags&REG)) ierror(0);
	  reg=p->q1.reg;
	  p->q1.flags&=~DREFOBJ;
	  if(!ISLWORD(p->q1.dtyp)){
	    emit(f,"\tcalli\tcc_uc,[%s]\n",regnames[reg]);
	    push(0,2);
	    callee_push(custack,csstack);
	    pop(0,2);
	  }else{
	    if(!reg_pair(reg,&rp)) ierror(0);
	    emit(f,"\tpush\t%s\n",regnames[rp.r2]);
	    emit(f,"\tpush\t%s\n",regnames[rp.r1]);
	    push(0,4);
	    callee_push(custack,csstack);
	    emit(f,"\trets\n");
	    emit(f,"%s%d:\n",labprefix,clabel);
	    pop(0,8);
	  }
	}else{
	  if(jmp){
	    emit(f,"\t%s",jump);
	    if(!need_return) ret=TASKING?"retv":"";
	    callee_push(custack,csstack);
	  }else{
	    emit(f,"\t%s\t",call);
	    if(TINY)
	      push(0,2);
	    else
	      push(0,4);
	    callee_push(custack,csstack);
	    if(TINY)
	      pop(0,2);
	    else
	      pop(0,4);
	  }
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	}
      }
      if(!zmeqto(l2zm(0L),p->q2.val.vmax)){
	notpopped+=zm2l(p->q2.val.vmax);
	dontpop-=zm2l(p->q2.val.vmax);
	if(!(g_flags[2]&USEDFLAG)&&usrstackoffset==-notpopped){
	  /*  Entfernen der Parameter verzoegern  */
	}else{
	  emit(f,"\tadd\t%s,#%ld\n",regnames[sp],zm2l(p->q2.val.vmax));
	  pop(zm2l(p->q2.val.vmax),0);
	  notpopped-=zm2l(p->q2.val.vmax);cc=0;
	}
      }
      continue;
    }
    if(c==ASSIGN||c==PUSH){
      if(c==PUSH) dontpop+=zm2l(p->q2.val.vmax);
      if(!ISSCALAR(t)||!zmeqto(p->q2.val.vmax,sizetab[t&NQ])||!zmleq(p->q2.val.vmax,l2zm(4L))){
	int alq,alz,rq,rz;unsigned long size;
	cc=0;
	size=zm2l(p->q2.val.vmax);
	/*FIXME: check for (s)huge */
	if(malign[t&NQ]==1){
	  alq=alignment(&p->q1);
	  alz=alignment(&p->z);
	}else{
	  alq=alz=0;
	}
	if(size==1||(size==2&&alq==0&&alz==0)){
	  if(c==PUSH&&zm2l(p->z.val.vmax)==1){
	    emit(f,"\tsub\t%s,#1\n",regnames[sp]);
	    push(1,0);size=1;
	  }
	  BSET(regs_modified,tp);
	  move(f,&p->q1,0,0,tp,size==1?CHAR:SHORT);
	  if(c==PUSH){
	    emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[tp]);
	    push(size,0);
	  }else
	    save_result(f,tp,p,size==1?CHAR:SHORT);
	  continue;
	}
	if(size==4&&alq==0&&alz==0){
	  tmp1=ti;tmp2=ti2;
	  BSET(regs_modified,ti);
	  BSET(regs_modified,ti2);
	  move(f,&p->q1,0,0,MTMP1,LONG);
	  if(c==PUSH){
	    emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[tmp2]);
	    emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[tmp1]);
	    push(4,0);
	  }else
	    save_result(f,MTMP1,p,LONG);
	  continue;
	}
	if(p->q1.am){
	  if(p->q1.am->flags!=IMM_IND) ierror(0);
	  if(!scratchreg(p->q1.am->base,p)){
	    BSET(regs_modified,ti);
	    emit(f,"\tmov\t%s,%s\n",regnames[ti],regnames[p->q1.am->base]);
	    rq=ti;
	  }else
	    rq=p->q1.am->base;
	  emit(f,"\tadd\t%s,#%ld",regnames[rq],p->q1.am->offset);
	  if(p->q1.am->v){
	    if(p->q1.am->v->storage_class==EXTERN)
	      emit(f,"+%s%s\n",idprefix,p->q1.am->v->identifier);

	    else
	      emit(f,"+%s%ld\n",labprefix,zm2l(p->q1.am->v->offset));
	  }
	  emit(f,"\n");
	}else if(p->q1.flags&REG){
	  if(scratchreg(p->q1.reg,p)){
	    rq=p->q1.reg;
	  }else{
	    BSET(regs_modified,ti);
	    emit(f,"\tmov\t%s,%s\n",regnames[ti],regnames[p->q1.reg]);
	    rq=ti;
	  }
	}else if((p->q1.flags&VAR)&&ISSTATIC(p->q1.v)){
	  int m=p->q1.flags;
	  rq=ti;
	  BSET(regs_modified,ti);
	  if(p->q1.flags&DREFOBJ)
	    p->q1.flags&=~DREFOBJ;
	  else
	    p->q1.flags|=VARADR;
	  move(f,&p->q1,0,0,ti,NPOINTER);
	  p->q1.flags&=~VARADR;
	}else{
	  BSET(regs_modified,ti);
	  rq=ti;
	  emit(f,"\tmov\t%s,%s\n",regnames[ti],regnames[sp]);
	  if(voff(&p->q1))
	    emit(f,"\tadd\t%s,#%ld\n",regnames[ti],voff(&p->q1));
	}
	if(!p->z.flags){
	  /* PUSH */
	  emit(f,"\tsub\t%s,#%lu\n",regnames[sp],size);
	  emit(f,"\tmov\t%s,%s\n",regnames[ti2],regnames[sp]);
	  rz=ti2;
	  push(size,0);
	  BSET(regs_modified,ti2);
	}else if(p->z.am){
	  if(p->z.am->flags!=IMM_IND) ierror(0);
	  if(!scratchreg(p->z.am->base,p)){
	    BSET(regs_modified,ti2);
	    emit(f,"\tmov\t%s,%s\n",regnames[ti2],regnames[p->z.am->base]);
	    rz=ti2;
	  }else
	    rz=p->z.am->base;
	  emit(f,"\tadd\t%s,#%ld",regnames[rz],p->z.am->offset);
	  if(p->z.am->v){
	    if(p->z.am->v->storage_class==EXTERN)
	      emit(f,"+%s%s",idprefix,p->z.am->v->identifier);
	    else
	      emit(f,"%+s%ld",labprefix,zm2l(p->z.am->v->offset));

	  }
	  emit(f,"\n");
	}else if(p->z.flags&REG){
	  if(scratchreg(p->z.reg,p)){
	    rz=p->z.reg;
	  }else{
	    BSET(regs_modified,ti2);
	    emit(f,"\tmov\t%s,%s\n",regnames[ti2],regnames[p->z.reg]);
	    rz=ti2;
	  }
	}else if(ISSTATIC(p->z.v)){
	  int m=p->z.flags;
	  rz=ti2;
	  BSET(regs_modified,ti2);
	  if(p->z.flags&DREFOBJ)
	    p->z.flags&=~DREFOBJ;
	  else
	    p->z.flags|=VARADR;
	  move(f,&p->z,0,0,ti2,NPOINTER);
	  p->z.flags=m;
	}else{
	  BSET(regs_modified,ti2);
	  rz=ti2;
	  emit(f,"\tmov\t%s,%s\n",regnames[ti2],regnames[sp]);
	  if(voff(&p->z))
	    emit(f,"\tadd\t%s,#%ld\n",regnames[ti2],voff(&p->z));
	}	
	if(alq==0&&alz==0){
	  if(optspeed){
	    if((size/8)>1){ 
	      BSET(regs_modified,tp);
	      emit(f,"\tmov\t%s,#%lu\n",regnames[tp],size/8);
	      emit(f,"%s%d:\n",labprefix,++label);
	    }
	    if(size>=8){
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      emit(f,"\tadd\t%s,#2\n",regnames[rz]);
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      emit(f,"\tadd\t%s,#2\n",regnames[rz]);	    
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      emit(f,"\tadd\t%s,#2\n",regnames[rz]);
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      emit(f,"\tadd\t%s,#2\n",regnames[rz]);	    
	    }
	    if((size/8)>1){
	      emit(f,"\tsub\t%s,#1\n",regnames[tp]);
	      emit(f,"\tjmpr\tcc_nz,%s%d\n",labprefix,label);	      
	    }
	    size&=7;
	    if(size>=4){
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      emit(f,"\tadd\t%s,#2\n",regnames[rz]);
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      if(size>4) emit(f,"\tadd\t%s,#2\n",regnames[rz]);	      
	    }
	    size&=3;
	    if(size>=2){
	      emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	      if(size>2) emit(f,"\tadd\t%s,#2\n",regnames[rz]);
	    }	      
	    if(size&1)
	      if(size&1) emit(f,"\tmovb\t[%s],[%s]\n",regnames[rz],regnames[rq]);
	  }else{
	    BSET(regs_modified,tp);
	    emit(f,"\tmov\t%s,#%lu\n",regnames[tp],size/2);
	    emit(f,"%s%d:\n",labprefix,++label);
	    emit(f,"\tmov\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	    emit(f,"\tadd\t%s,#2\n",regnames[rz]);
	    emit(f,"\tsub\t%s,#1\n",regnames[tp]);
	    emit(f,"\tjmpr\tcc_nz,%s%d\n",labprefix,label);
	    if(size&1) emit(f,"\tmovb\t[%s],[%s]\n",regnames[rz],regnames[rq]);
	  }
	}else{
	  BSET(regs_modified,tp);
	  emit(f,"\tmov\t%s,#%lu\n",regnames[tp],size);
	  emit(f,"%s%d:\n",labprefix,++label);
	  emit(f,"\tmovb\t[%s],[%s+]\n",regnames[rz],regnames[rq]);
	  emit(f,"\tadd\t%s,#1\n",regnames[rz]);
	  emit(f,"\tsub\t%s,#1\n",regnames[tp]);
	  emit(f,"\tjmpr\tcc_nz,%s%d\n",labprefix,label);
	}
	continue;
      }
      /* mov [rx],[ry] ; mov [rx+],[ry] ; mov [rx],[ry+] */
      if(!ISLWORD(t)&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&(!p->z.am||p->z.am->flags==POST_INC)&&(!p->q1.am||p->q1.am->flags==POST_INC)&&!reg_pair(p->q1.reg,&rp)&&!reg_pair(p->z.reg,&rp)){
	emit(f,"\tmov%s\t",(t&NQ)==CHAR?"b":"");
	emit_obj(f,&p->z,t);emit(f,",");
	emit_obj(f,&p->q1,t);emit(f,"\n");
	cc=&p->z;cc_t=t;continue;
      }
      /* mov mem,[rx]/reg */
      if(!ISLWORD(t)&&(issfr(q1)||(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ))&&(p->z.flags&(REG|DREFOBJ|VAR))==VAR&&!p->q1.am&&(p->z.v->storage_class==STATIC||p->z.v->storage_class==EXTERN)){
	emit(f,"\tmov%s\t",(t&NQ)==CHAR?"b":"");
	emit_obj(f,&p->z,t);emit(f,",");
	emit_obj(f,&p->q1,t);emit(f,"\n");
	cc=&p->z;cc_t=t;continue;
      } 
      /* mov [rx]/reg,mem */
      if(!ISLWORD(t)&&(issfr(z)||(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ))&&(p->q1.flags&(REG|DREFOBJ|VAR))==VAR&&!p->z.am&&(p->q1.v->storage_class==STATIC||p->q1.v->storage_class==EXTERN)){
	emit(f,"\tmov%s\t",(t&NQ)==CHAR?"b":"");
	emit_obj(f,&p->z,t);emit(f,",");
	emit_obj(f,&p->q1,t);emit(f,"\n");
	cc=&p->z;cc_t=t;continue;
      }     
      /* mov reg,op */
      if(issfr(z)&&(p->q1.flags&KONST)){
	emit(f,"\tmov%s\t",(t&NQ)==CHAR?"b":"");
	emit_obj(f,&p->z,t);emit(f,",");
	emit_obj(f,&p->q1,t);emit(f,"\n");
	cc=&p->z;cc_t=t;continue;
      }

      if(isreg(q1)){
	reg=p->q1.reg;
      }else if(isreg(z)){
	reg=p->z.reg;
      }else{
	BSET(regs_modified,ti);
	if(ISLWORD(t)){
	  BSET(regs_modified,ti2);
	  tmp1=ti;tmp2=ti2;
	  reg=MTMP1;
	}else
	  reg=ti;
      }
      if(c==PUSH){
	move(f,&p->q1,0,0,reg,t);
	if(reg_pair(reg,&rp)){
	  emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[rp.r2]);
	  emit(f,"\tmov\t[-%s],%s\n",regnames[sp],regnames[rp.r1]);
	  cc=0;
	}else{
	  emit(f,"\tmov%s\t[-%s],%s\n",x_t[t&NQ],regnames[sp],regnames[reg]);
	  cc=&p->q1;cc_t=t;
	}
	push(zm2l(p->z.val.vmax),0);
	continue;
      }
      if(c==ASSIGN){
	if((p->z.flags&(VAR|REG|DREFOBJ))==VAR&&ISSTATIC(p->z.v)&&isconst(q1)){
	  /*FIXME: long und ones */
	  eval_const(&p->q1.val,t);
	  if(ISNULL()&&!ISLWORD(t)){
	    emit(f,"\tmov%s\t",(t&NQ)==CHAR?"b":"");emit_obj(f,&p->z,t);
	    emit(f,",ZEROS\n");
	    cc=&p->z;cc_t=t;continue;
	  }
	}
	if((t&NQ)==CHAR&&!regok(reg,CHAR,0)){
	  reg=tp;
	  BSET(regs_modified,tp);
	}
	move(f,&p->q1,0,0,reg,t);
	save_result(f,reg,p,t);
	if(ISLWORD(t)){
	  cc=0;
	}else{
	  cc=&p->z;cc_t=t;
	}
	continue;
      }
      ierror(0);
    }
    if(c==ADDRESS){
      if(reg_pair(reg,&rp)){
	emit(f,"\tmov\t%s,%s\n",regnames[rp.r1],regnames[sp]);
	emit(f,"\tmov\t%s,#0\n",regnames[rp.r2]);
	if(voff(&p->q1))
	  emit(f,"\tadd\t%s,#%ld\n",regnames[rp.r1],voff(&p->q1)&0xffff);
      }else{
	emit(f,"\tmov\t%s,%s\n",regnames[reg],regnames[sp]);
	if(voff(&p->q1)) emit(f,"\tadd\t%s,#%ld\n",regnames[reg],voff(&p->q1));
      }
      save_result(f,reg,p,p->typf2);
      cc=0;
      continue;
    }

    if(c==MOD||c==DIV){
      if(ISLWORD(t)) ierror(0);
      /*FIXME:suboptimal*/
      emit(f,"\tmov\tMDL,%s\n",regnames[reg]);
      move(f,&p->q2,0,0,reg,t);
      emit(f,"\tdiv%s\t%s\n",(t&UNSIGNED)?"u":"",regnames[reg]);
      if(c==MOD) 
	emit(f,"\tmov\t%s,MDH\n",regnames[reg]);
      else
	emit(f,"\tmov\t%s,MDL\n",regnames[reg]);
      save_result(f,reg,p,t);
      cc=&p->z;cc_t=t;
      continue;
    }
    if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==COMPARE||c==ADDI2P||c==SUBIFP){
      char *s;
      /*FIXME: nicht immer besser*/
/* rfi: better performed in LSHIFT code generation
      if(ISLWORD(t)&&c==LSHIFT&&isconst(q2)){
	eval_const(&p->q2.val,t);
	if(zm2l(vmax)==1&&!p->q1.am){
	  p->code=c=ADD;
	  p->q2=p->q1;
	}
      }
*/
      if(ISLWORD(t)&&c==MULT) ierror(0);
      if(ISLWORD(t)&&(c==LSHIFT||c==RSHIFT)){
/* rfi: we now handle constant value greater than 16 in const shift generation
	if(isconst(q2))	
	  eval_const(&p->q2.val,q2typ(p));
	if(!isconst(q2)||zmleq(l2zm(16L),vmax)){
*/
	if(!isconst(q2)){
	  int cnt,lab1,lab2;
	  if(isreg(q2)&&scratchreg(p->q2.reg,p)){
	    cnt=p->q2.reg;
	  }else{
	    BSET(regs_modified,tp);
	    cnt=tp;
	    if(isreg(q2)&&reg_pair(p->q2.reg,&rp))
	      move(f,0,rp.r1,0,tp,INT);
	    else
	      move(f,&p->q2,0,0,tp,INT);
	  }
	  if(reg_pair(cnt,&rp))
	    cnt=rp.r1;
	  /*move(f,&p->q1,0,0,reg,ztyp(p));*/
	  lab1=++label;lab2=++label;
	  if(!reg_pair(reg,&rp)) ierror(0);
	  if(c==LSHIFT){
	    emit(f,"\tjmpr\tcc_uc,%s%d\n",labprefix,lab2);
	    emit(f,"%s%d:\n",labprefix,lab1);
	    emit(f,"\tadd\t%s,%s\n",regnames[rp.r1],regnames[rp.r1]);
	    emit(f,"\taddc\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
	    emit(f,"%s%d:\n",labprefix,lab2);
/* rfi: fix off by one and handle negative shift value
		emit(f,"\tsub\t%s,#1\n",regnames[cnt]);
	    emit(f,"\tjmpr\tcc_ne,%s%d\n",labprefix,lab1);
*/
	    emit(f,"\tcmpd1\t%s,#1\n",regnames[cnt]);
	    emit(f,"\tjmpr\tcc_sge,%s%d\n",labprefix,lab1);
	  }else{
	    emit(f,"\tjmpr\tcc_uc,%s%d\n",labprefix,lab2);
	    emit(f,"%s%d:\n",labprefix,lab1);
		emit(f,"\tshr\t%s,#1\n",regnames[rp.r1]);
	    emit(f,"\tbmov\t%s.15,%s.0\n",regnames[rp.r1],regnames[rp.r2]);
/* rfi: use ashr for signed shift
		emit(f,"\tshr\t%s,#1\n",regnames[rp.r2]);
*/
	    emit(f,"\t%sshr\t%s,#1\n",(t&UNSIGNED)?"":"a",regnames[rp.r2]);
	    emit(f,"%s%d:\n",labprefix,lab2);
/* rfi: fix off by one and handle negative shift value
		emit(f,"\tsub\t%s,#1\n",regnames[cnt]);
	    emit(f,"\tjmpr\tcc_ne,%s%d\n",labprefix,lab1);
*/
	    emit(f,"\tcmpd1\t%s,#1\n",regnames[cnt]);
	    emit(f,"\tjmpr\tcc_sge,%s%d\n",labprefix,lab1);
	  }
	  save_result(f,reg,p,ztyp(p));
	  continue;
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
      if(c==RSHIFT&&!(t&UNSIGNED)) s="ashr";
      if(c==COMPARE){
	lastcomp=t;
	if(ISLWORD(t)) s="sub"; else s="cmp";
      }
      if((c==MULT&&!isreg(q2))){
	BSET(regs_modified,tp);
	move(f,&p->q2,0,0,tp,t);
	p->q2.flags=REG;
	p->q2.reg=tp;
      }
      if((c==LSHIFT||c==RSHIFT)&&!isconst(q2)&&!isreg(q2)){
	BSET(regs_modified,tp);
	move(f,&p->q2,0,0,tp,t);
	p->q2.flags=REG;
	p->q2.reg=tp;
      }
      if(isreg(q2)){
	if(reg_pair(reg,&rp)){
	  if(!reg_pair(p->q2.reg,&qp)) qp.r1=p->q2.reg;
	  emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r1],regnames[qp.r1]);
	  s=longcmd(s);
	  if(!short_add) emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r2],regnames[qp.r2]);
	}else{
	  emit(f,"\t%s%s\t%s,%s\n",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg],(t&NQ)==CHAR?bregnames[p->q2.reg]:regnames[p->q2.reg]);
	}	    
      }else if(p->q2.am){
	if(reg_pair(p->q2.am->base,&rp)){
	  int seg=rp.SEG;
	  if(!short_add&&reg_pair(reg,&rp))
	    emit(f,"\texts\t%s,#3\n",regnames[seg]);
	  else
	    emit(f,"\texts\t%s,#1\n",regnames[seg]);
	}
	if(reg_pair(reg,&rp)){
	  BSET(regs_modified,tp);
	  emit(f,"\tmov\t%s,",regnames[tp]);emit_obj(f,&p->q2,t);emit(f,"\n");
	  emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r1],regnames[tp]);
	  s=longcmd(s);
	  if(!short_add){
	    p->q2.am->offset+=2;
	    emit(f,"\tmov\t%s,",regnames[tp]);emit_obj(f,&p->q2,t);emit(f,"\n");
	    emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r2],regnames[tp]);
	    p->q2.am->offset-=2;
	  }
	}else{
	  if(p->q2.am->flags==POST_INC&&p->q2.am->base<=4){
	    emit(f,"\t%s%s\t%s,",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg]);
	    emit_obj(f,&p->q2,t);emit(f,"\n");
	  }else{
	    BSET(regs_modified,tp);
	    emit(f,"\tmov%s\t%s,",x_t[t&NQ],(t&NQ)==CHAR?bregnames[tp]:regnames[tp]);
	    emit_obj(f,&p->q2,t);emit(f,"\n");
	    emit(f,"\t%s%s\t%s,%s\n",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg],(t&NQ)==CHAR?bregnames[tp]:regnames[tp]);
	  }
	}
      }else if((p->q2.flags&(VAR|VARADR|REG))==VAR&&ISSTATIC(p->q2.v)){
	if(ISFAR(p->q2.v)){
	  emit(f,"\texts\t#SEG ");isseg=1;emit_obj(f,&p->q2,t);
	  emit(f,",#%d\n",(reg_pair(reg,&rp)?2:1));
	}
	if(reg_pair(reg,&rp)){
	  emit(f,"\t%s\t%s,",s,regnames[rp.r1]);
	  emit_obj(f,&p->q2,t);emit(f,"\n");
	  s=longcmd(s);
	  if(!short_add){
	    emit(f,"\t%s\t%s,",s,regnames[rp.r2]);
	    emit_obj(f,&p->q2,t);emit(f,"+2\n");
	  }
	}else{
	  emit(f,"\t%s%s\t%s,",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg]);
	  emit_obj(f,&p->q2,t);emit(f,"\n");
	}
      }else if(isconst(q2)){
	if(ISFLOAT(t)) ierror(0);
	if(reg_pair(reg,&rp)){
	  long l;
/* rfi: shift constant must be evaluated according to own type
	  eval_const(&p->q2.val,t);
*/
	  eval_const(&p->q2.val,q2typ(p));
	  if(c==RSHIFT){
	    l=zm2l(vmax);
/* rfi: new generation for constant right shift */
		if( l < 0) {
		/* TODO: we should raise a warning */
		/* Implementation dependent: negative shift is same as null shift */
		/* avoid shift generation */
		} else if( l > 31) {
		/* TODO: we should raise a warning */
		/* Implementation dependent: same behaviour as non const shift */
			if( t & UNSIGNED)
				emit(f,"\tmov\t%s,#0\n",regnames[rp.r2]);
			else
	    		emit(f,"\t%s\t%s,#15\n",s,regnames[rp.r2]);

		   	emit(f,"\tmov\t%s,%s\n",regnames[rp.r1],regnames[rp.r2]);
		} else if( l > 15) {
	    	emit(f,"\tmov\t%s,%s\n",regnames[rp.r1],regnames[rp.r2]);
			if( t & UNSIGNED)
				emit(f,"\tmov\t%s,#0\n",regnames[rp.r2]);
			else
	    		emit(f,"\t%s\t%s,#15\n",s,regnames[rp.r2]);

			l -= 16 ;
			if( l > 0)
	    		emit(f,"\t%s\t%s,#%ld\n",s,regnames[rp.r1],l);
		} else if( l == 0) {
		/* avoid shift generation */
		} else if( l == 1) {
			emit(f,"\tshr\t%s,#1\n",regnames[rp.r1]);
	    	emit(f,"\tbmov\t%s.15,%s.0\n",regnames[rp.r1],regnames[rp.r2]);
	    	emit(f,"\t%s\t%s,#1\n",s,regnames[rp.r2]);
		} else {
/* rfi: warning should be raised in corresponding case (see TODO above)
	    if(l<1||l>15) ierror(0);
*/
	    BSET(regs_modified,tp);
	    emit(f,"\tmov\t%s,%s\n",regnames[tp],regnames[rp.r2]);
	    emit(f,"\tshr\t%s,#%ld\n",regnames[rp.r1],l);
	    emit(f,"\t%s\t%s,#%ld\n",s,regnames[rp.r2],l);
	    emit(f,"\tshl\t%s,#%ld\n",regnames[tp],16-l);
	    emit(f,"\tor\t%s,%s\n",regnames[rp.r1],regnames[tp]);
		}
	  }else if(c==LSHIFT){
	    l=zm2l(vmax);
/* rfi: new generation for constant left shift */
		if( l < 0) {
		/* TODO: we should raise a warning */
		/* Implementation dependent: negative shift is same as null shift */
		/* avoid shift generation */
		} else if( l > 31) {
		/* TODO: we should raise a warning */
		/* Implementation dependent: same behaviour as non const shift */
			emit(f,"\tmov\t%s,#0\n",regnames[rp.r1]);
		   	emit(f,"\tmov\t%s,%s\n",regnames[rp.r2],regnames[rp.r1]);
		} else if( l > 15) {
	   		emit(f,"\tmov\t%s,%s\n",regnames[rp.r2],regnames[rp.r1]);
	   		emit(f,"\tmov\t%s,#0\n",regnames[rp.r1]);
			l -= 16 ;
			if( l > 0)
	      		emit(f,"\tshl\t%s,#%ld\n",regnames[rp.r2],l);
		} else if( l == 0) {
		/* avoid shift generation */
		} else if( l == 1) {
	      emit(f,"\tadd\t%s,%s\n",regnames[rp.r1],regnames[rp.r1]);
	      emit(f,"\taddc\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
		} else
/* rfi: warning should be raised in corresponding case (see TODO above)
	    if(l<1||l>15) ierror(0);
*/
	    if(l==2){
	      emit(f,"\tadd\t%s,%s\n",regnames[rp.r1],regnames[rp.r1]);
	      emit(f,"\taddc\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
	      emit(f,"\tadd\t%s,%s\n",regnames[rp.r1],regnames[rp.r1]);
	      emit(f,"\taddc\t%s,%s\n",regnames[rp.r2],regnames[rp.r2]);
	    }else{
	      BSET(regs_modified,tp);
	      emit(f,"\tmov\t%s,%s\n",regnames[tp],regnames[rp.r1]);
	      emit(f,"\tshl\t%s,#%ld\n",regnames[rp.r1],l);
	      emit(f,"\tshl\t%s,#%ld\n",regnames[rp.r2],l);
	      emit(f,"\tshr\t%s,#%ld\n",regnames[tp],16-l);
	      emit(f,"\tor\t%s,%s\n",regnames[rp.r2],regnames[tp]);
	    }
	  }else{
	    l=zm2l(zmand(vmax,l2zm(65535L)));
	    emit(f,"\t%s\t%s,#%ld\n",s,regnames[rp.r1],l);
	    l=zm2l(zmrshift(vmax,l2zm(16L)));
	    s=longcmd(s);
	    if(!short_add) emit(f,"\t%s\t%s,#%ld\n",s,regnames[rp.r2],l);
	  }
	}else{
	  emit(f,"\t%s%s\t%s,",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg]);
	  emit_obj(f,&p->q2,t);emit(f,"\n");
	}
      }else if(p->q2.flags&VARADR){
	p->q2.flags&=~VARADR;
	if(reg_pair(reg,&rp)){
	  emit(f,"\t%s\t%s,#SOF ",s,regnames[rp.r1]);emit_obj(f,&p->q2,t);emit(f,"\n");
	  s=longcmd(s);
	  if(!short_add)
	    emit(f,"\t%s\t%s,#SEG ",s,regnames[rp.r2]);isseg=1;emit_obj(f,&p->q2,t);emit(f,"\n");
	}else{
	  emit(f,"\t%s\t%s,#",s,regnames[reg]);emit_obj(f,&p->q2,t);emit(f,"\n");
	}
	p->q2.flags|=VARADR;
      }else if(p->q2.flags&DREFOBJ){
	int seg=0,sof=0;
        /*	if(!(p->q2.flags&REG)) ierror(0);*/
	if(reg_pair(p->q2.reg,&rp)){
	  seg=rp.SEG;sof=rp.SOF;
	  if(!short_add&&reg_pair(reg,&rp)){
	    emit(f,"\texts\t%s,#2\n",regnames[seg]);
	  }else
	    emit(f,"\texts\t%s,#1\n",regnames[seg]);
	}
	if(reg_pair(reg,&rp)){
	  if(scratchreg(p->q2.reg,p)){
	    emit(f,"\t%s\t%s,[%s+]\n",s,regnames[rp.r1],regnames[sof?sof:p->q2.reg]);
	    s=longcmd(s);
	    if(!short_add) emit(f,"\t%s\t%s,[%s]\n",s,regnames[rp.r2],regnames[sof?sof:p->q2.reg]);
	  }else{
	    emit(f,"\t%s\t%s,[%s]\n",s,regnames[rp.r1],regnames[sof?sof:p->q2.reg]);
	    s=longcmd(s);
	    if(!short_add){
	      BSET(regs_modified,tp);
	      emit(f,"\tmov\t%s,[%s+#2]\n",regnames[tp],regnames[sof?sof:p->q2.reg]);
	      emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r2],regnames[tp]);
	    }
	  }
	}else{
	  emit(f,"\t%s%s\t%s,",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg]);
	  emit_obj(f,&p->q2,t);emit(f,"\n");
	}
      }else if((p->q2.flags&(VAR|DREFOBJ))==VAR&&(p->q2.v->storage_class==AUTO||p->q2.v->storage_class==REGISTER)){
	long l=voff(&p->q2);
	if(reg_pair(reg,&rp)){
	  if(l){
	    BSET(regs_modified,tp);
	    emit(f,"\tmov\t%s,[%s+#%ld]\n",regnames[tp],regnames[sp],l);
	    emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r1],regnames[tp]);
	  }else
	    emit(f,"\t%s\t%s,[%s]\n",s,regnames[rp.r1],regnames[sp]);
	  s=longcmd(s);
	  if(!short_add){
	    BSET(regs_modified,tp);
	    emit(f,"\tmov\t%s,[%s+#%ld]\n",regnames[tp],regnames[sp],l+2);
	    emit(f,"\t%s\t%s,%s\n",s,regnames[rp.r2],regnames[tp]);
	  }
	}else{
	  if(l){
	    BSET(regs_modified,tp);
	    emit(f,"\tmov%s\t%s,[%s+#%ld]\n",x_t[t&NQ],(t&NQ)==CHAR?bregnames[tp]:regnames[tp],regnames[sp],l);
	    emit(f,"\t%s%s\t%s,%s\n",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg],(t&NQ)==CHAR?bregnames[tp]:regnames[tp]);
	  }else
	    emit(f,"\t%s%s\t%s,[%s]\n",s,x_t[t&NQ],(t&NQ)==CHAR?bregnames[reg]:regnames[reg],regnames[sp]);
	}
      }else
	ierror(0);
      if(c==MULT) emit(f,"\tmov\t%s,MDL\n",regnames[reg]);
      /*FIXME:signed*/
      if(short_add==1) emit(f,"\t%s\t%s,#0\n",s,regnames[rp.r2]);
      if(c!=COMPARE) save_result(f,reg,p,t);
      if(!ISLWORD(t)||((c==ADD||c==SUB||c==ADDI2P||c==SUBIFP)&&!short_add)){
	cc=&p->z;cc_t=t;
      }else
	cc=0;
      continue;
    }
    ierror(0);
  }
  if(notpopped){
    emit(f,"\tadd\t%s,#%ld\n",regnames[sp],notpopped);
    pop(notpopped,0);notpopped=0;
  }
  function_bottom(f,v,loff);
}

int shortcut(int c,int t)
{
  if(c==COMPARE||c==AND||c==OR||c==XOR) return 1;
  if((t&NQ)!=BIT&&(c==ADD||c==SUB)) return 1;
  return 0;
}

void cleanup_cg(FILE *f)
{
  struct fpconstlist *p;
  unsigned char *ip;
  if(f&&stack_check){
    emit(f,"\t%s\t%s__stack_check\n",public,idprefix);
  }
  while(p=firstfpc){
    if(f){
      new_section(f,NDATA);
      emit(f,even);
      emit(f,"%s%d:\n\t%s\t",labprefix,p->label,TASKING?dct[SHORT]:vdct[SHORT]);
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
  if(f&&TASKING) emit(f,"%s\tREGDEF R0-R15\n\tend\n",sec_end);
}

int reg_parm(struct reg_handle *p,struct Typ *t,int mode,struct Typ *fkt)
{
  int f=t->flags&NQ;
  if(!ISSCALAR(f)) return 0;
  if(p->gpr>3||mode) return 0;
  if(f==LLONG||f==DOUBLE||f==LDOUBLE)
    return 0;
  else if(f==LONG||f==FPOINTER||f==HPOINTER){
    if(p->gpr==0) {p->gpr=2;return 23;}
    if(p->gpr==1) {p->gpr=3;return 22;}
    if(p->gpr==2) {p->gpr=4;return 21;}
    return 0;
  }else
    return 13+p->gpr++;
}

void insert_const(union atyps *p,int t)
/*  Traegt Konstante in entprechendes Feld ein.       */
{
  if(!p) ierror(0);
/*  *p = (union atyps) 0 ; /* rfi: clear unused bits */
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
      if(ISFLOAT(t2)&&(t&NU)==LLONG){
        sprintf(fname,"__flt%dto%cint%ld",((t2&NU)==FLOAT)?32:64,(t&UNSIGNED)?'u':'s',zm2l(sizetab[t&NQ])*8);
        ret=fname;
      }
    }
    if((t&NQ)==LLONG||ISFLOAT(t)){
      if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==KOMPLEMENT||c==MINUS){
	if(t==(UNSIGNED|LLONG)&&(c==DIV||c==MOD||c==RSHIFT)){
	  sprintf(fname,"__%suint64",ename[c]);
	  ret=fname;
	}else if((t&NQ)==LLONG){
          sprintf(fname,"__%sint64",ename[c]);
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

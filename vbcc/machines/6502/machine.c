/*  6502 backend for vbcc
    (c) Volker Barthelmann 2020

*/                                                                             

#include "supp.h"
#include "vbc.h"

#include <math.h>

static char FILE_[]=__FILE__;

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc 6502 code-generator V0.3 (c) in 2020 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts:
    0: just a flag
    VALFLAG: a value must be specified
    STRINGFLAG: a string can be specified
    FUNCFLAG: a function will be called
    apart from FUNCFLAG, all other versions can only be specified once */
int g_flags[MAXGF]={0,0,
		    VALFLAG,0,0,
		    0,0,
		    VALFLAG,VALFLAG,0,0,
		    VALFLAG,0,0,0,
		    0,0,0,
		    0,0,0,0,0,0};

/* the flag-name, do not use names beginning with l, L, I, D or U, because
   they collide with the frontend */
char *g_flags_name[MAXGF]={"std-syntax","no-rax",
			   "volatile-regs","ieee","no-peephole",
			   "cbmascii","softstack",
			   "reg-args","int-args","mainargs","no-bank-vars",
			   "common-banknr","btmp-zpage","oldfp","large",
			   "glob-acc","avoid-bank-switch","manual-banking",
			   "atascii","65c02","nox","mega65","ce02","c02"};

/* the results of parsing the command-line-flags will be stored here */
union ppi g_flags_val[MAXGF];

/*  Alignment-requirements for all types in bytes.              */
zmax align[MAX_TYPE+1];

/*  Alignment that is sufficient for every object.              */
zmax maxalign;

/*  CHAR_BIT for the target machine.                            */
zmax char_bit;

/*  sizes of the basic types (in bytes) */
zmax sizetab[MAX_TYPE+1];

/*  Minimum and Maximum values each type can have.              */
/*  Must be initialized in init_cg().                           */
zmax t_min[MAX_TYPE+1];
zumax t_max[MAX_TYPE+1];
zumax tu_max[MAX_TYPE+1];

/*  Names of all registers. will be initialized in init_cg(),
    register number 0 is invalid, valid registers start at 1 */
char *regnames[MAXR+1];

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  a type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1];

/* specifies the priority for the register-allocator, if the same
   estimated cost-saving can be obtained by several registers, the
   one with the highest priority will be used */
int reg_prio[MAXR+1];

/* an empty reg-handle representing initial state */
struct reg_handle empty_reg_handle={0,0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__interrupt","__zpage","__nocpr",0};

#define INTERRUPT 1
#define ZPAGE 2
#define NOCOMPRESS 4

/****************************************/
/*  Private data and functions.         */
/****************************************/

#define STDSYNTAX  (g_flags[0]&USEDFLAG)
#define NORAX      (g_flags[1]&USEDFLAG)
#define VOL_GPRS   ((g_flags[2]&USEDFLAG)?g_flags_val[2].l:NUM_GPRS/2)
#define IEEE       (g_flags[3]&USEDFLAG)
#define NOPEEP     (g_flags[4]&USEDFLAG)
#define CBMASCII   (g_flags[5]&USEDFLAG)
#define SOFTSTACK  (g_flags[6]&USEDFLAG)
#define GPR_ARGS   ((g_flags[7]&USEDFLAG)?g_flags_val[7].l:8)
#define MAINARGS   (g_flags[9]&USEDFLAG)
#define NOBANKVARS (g_flags[10]&USEDFLAG)
#define COMMONBANK ((g_flags[11]&USEDFLAG)?g_flags_val[11].l:0)
#define BIGZPAGE   (g_flags[12]&USEDFLAG)
#define OLDFP      (g_flags[13]&USEDFLAG)
#define LARGE      (g_flags[14]&USEDFLAG)
#define GLOBACC    (g_flags[15]&USEDFLAG)
#define NOSWITCH   (g_flags[16]&USEDFLAG)
#define NOBANKING  (g_flags[17]&USEDFLAG)
#define ATASCII    (g_flags[18]&USEDFLAG)
#define C02        (g_flags[19]&USEDFLAG)
#define NOX        (g_flags[20]&USEDFLAG)
#define MEGA65     (g_flags[21]&USEDFLAG)
#define CE02       (g_flags[22]&USEDFLAG)
#define C02ALT     (g_flags[23]&USEDFLAG)



#define STR_NEAR "near"
#define STR_FAR "far"
#define STR_HUGE "huge"

#define PLA (-1)
#define JMPIND (-2)

static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isacc(x) (isreg(x)&&(p->x.reg==ra||p->x.reg==rax))
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

#define iszpage(o) ((o->flags&(REG|DREFOBJ)==REG)||((o->flags&(VAR|DREFOBJ))==VAR&&o->v->tattr&ZPAGE))

#define isptr(r) ((r)>=FIRST_PAIR&&(r)<=LAST_PAIR)

static int q1reg,q2reg,zreg;

static char *ccs[]={"eq","ne","lt","ge","le","gt",""};
static char *logicals[]={"ora","eor","and"};
static char *arithmetics[]={"slw","srw","adc","sbc","mullw","divw","mod"};

/* alignment of basic data-types, used to initialize align[] */
static long malign[MAX_TYPE+1]=  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
/* sizes of basic data-types, used to initialize sizetab[] */
static long msizetab[MAX_TYPE+1]={1,1,2,2,4,8,4,4,4,0,2,3,3,0,0,0,1,0};
static char *mregnames[MAXR+1];

/* Typenames (needed because of HAVE_EXT_TYPES). */
char *typname[]={"strange","char","short","int","long","long long",
                 "float","double","long double","void",
                 "near-pointer","far-pointer","huge-pointer",
                 "array","struct","union","enum","function"};


/* used to initialize regtyp[] */
static struct Typ ityp={INT},ctyp={CHAR},ftyp={FLOAT},lltyp={LLONG};

/* macros defined by the backend */
static char *marray[]={"__section(x)=__vattr(\"section(\"#x\")\")",
		       "__6502__",
		       "__SIZE_T_INT",
		       "__bank(x)=__vattr(\"bank(\"#x\")\")",
                       "__far=__attr(\"far\")",
                       "__near=__attr(\"near\")",
                       "__huge=__attr(\"huge\")",
		       0};

/* special registers */
static int ra=1, rx=2, ry=3, sp1=4,sp2=5,sp=6,fp,fp1,fp2;
static int t4=LAST_GPR,t3=LAST_GPR-1,t2=LAST_GPR-2,t1=LAST_GPR-3;
static int rax=7;
static int yval;
#define NOVAL 1000

static int t1,t2,f1,f2; /*tODO: remove*/

static int pushedacc,pushedx,nopeep,cbmascii,atascii,ieee;
static int storedacc,storedx;
static int c02,m65,ce02,zzero,noy;
static char *jmpinst;
static int pass;
static int libsave;
static struct rpair rp2;
static int cbank;
static Var bankv;
static int bankvoffset;
static int bankcnum;
static obj zstore;
static int zstoreflag;
static int hasretval;

#define SECLEN 128
char *use_sec;
int use_bank=-1;

#define dt(t) (((t)&UNSIGNED)?udt[(t)&NQ]:sdt[(t)&NQ])
static char *sdt[MAX_TYPE+1]={"??","c","s","i","l","ll","f","d","ld","v","p"};
static char *udt[MAX_TYPE+1]={"??","uc","us","ui","ul","ull","f","d","ld","v","p"};

/* perhaps provide version with 8bit int? */
#define ISCHAR(t) ((t&NQ)==CHAR)
#define ISSHORT(t) ((t&NQ)==SHORT||(t&NQ)==INT||(t&NQ)==POINTER)
#define ISFPOINTER(t) ((t&NQ)==FPOINTER)
#define ISLONG(t) ((t&NQ)==LONG)
#define ISLLONG(t) ((t&NQ)==LLONG)

#define ISIDX(r) (r==rx)
#define ISRIDX(op) (isreg(op)&&ISIDX(p->op.reg))
#define ISPREG(op) (isreg(op)&&isptr(p->op.reg))

#define LONGM65(c) ((c)==ASSIGN||(c)==PUSH||((c)>=LSHIFT&&(c)<=MOD)||((c)>=OR&&(c)<=AND))

/* am */
#define IMM_IND 1
#define GPR_IND 2
#define ABS_IND 3

/* sections */
#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define SPECIAL 4

static long stack;
static int stack_valid;
static int section=-1,newobj;
static char *codename="\tsection\ttext",
  *dataname="\tsection\tdata",
  *bssname="\tsection\tbss",
  *rodataname="\tsection\trodata";

/* return-instruction */
static char *ret;

/* label at the end of the function (if any) */
static int exit_label;

/* assembly-prefixes for labels and external identifiers */
static char *labprefix="l",*idprefix="_";

/* variables to calculate the size and partitioning of the stack-frame
   in the case of FIXED_SP */
static long frameoffset,pushed,maxpushed,framesize;

static long localsize,rsavesize,rscnt,argsize;

static void emit_obj(FILE *f,struct obj *p,int t);

/* calculate the actual current offset of an object relativ to the
   stack-pointer; we use a layout like this:
   ------------------------------------------------
   | arguments to this function                   |
   ------------------------------------------------
   | caller-save registers [size=rsavesize]       |
   ------------------------------------------------
   | local variables [size=localsize]             |
   ------------------------------------------------
   | arguments to called functions [size=argsize] |
   ------------------------------------------------
   All sizes will be aligned as necessary.
   In the case of FIXED_SP, the stack-pointer will be adjusted at
   function-entry to leave enough space for the arguments and have it
   aligned to 16 bytes. Therefore, when calling a function, the
   stack-pointer is always aligned to 16 bytes.
   For a moving stack-pointer, the stack-pointer will usually point
   to the bottom of the area for local variables, but will move while
   arguments are put on the stack.

   This is just an example layout. Other layouts are also possible.
*/

static int bank(Var *v)
{
  char *s=v->vattr;
  int n,r;
  if(!NOBANKING&&s&&(s=strstr(s,"bank("))){
    if(sscanf(s+5,"%i",&n)==1)
      return n;
  }
  return -1;
}

static void ebank(FILE *f,int b)
{
  if(b>=0) emit(f,"%d",b);
  emit(f,"\n");
}

static long real_offset(struct obj *o)
{
  long off=zm2l(o->v->offset);
  if(off<0){
    /* function parameter */
    off=localsize+rsavesize-off-zm2l(maxalign);
  }

  off+=argsize;
  off+=zm2l(o->val.vmax);
  return off;
}

/*  Initializes an addressing-mode structure and returns a pointer to
    that object. Will not survive a second call! */
static struct obj *cam(int flags,int base,long offset)
{
  static struct obj obj;
  static struct AddressingMode am;
  obj.am=&am;
  am.flags=flags;
  am.base=base;
  am.offset=offset;
  return &obj;
}

/* changes to a special section, used for __section() */
static int special_section(FILE *f,struct Var *v)
{
  char *sec;
  if(v->tattr&ZPAGE){
    emit(f,"\tsection\tzpage\n");
  }else{
    if(!v->vattr) return 0;
    sec=strstr(v->vattr,"section(");
    if(!sec) return 0;
    sec+=strlen("section(");
    emit(f,"\tsection\t");
    while(*sec&&*sec!=')') emit_char(f,*sec++);
    emit(f,"\n");
  }
  if(f) section=SPECIAL;
  return 1;
}



#define chk_coll(x) do{if((x)==r||(x)==r1||(x)==r2) return 0;	\
    if(reg_pair((x),&rp)&&(rp.r1==r||rp.r2==r)) return 0;}while(0)

static int scratch(IC *p,int r,int isnext)
{
  int r1,r2;
  if(!p) return 1;
  if(reg_pair(r,&rp)){
    r1=rp.r1;
    r2=rp.r2;
  }else{
    r1=0;
    r2=0;
  }
  if(!isnext&&isreg(z)&&p->z.reg==r){
    if(!(p->q2.flags&REG))
      return 1;
    if(p->q2.reg==r||p->q2.reg==r1||p->q2.reg==r2)
      return 0;
    if(reg_pair(p->q2.reg,&rp))
      if(rp.r1==r||rp.r2==r)
	return 0;
    return 1;
  }
  while(p){
    if(p->code==LABEL||p->code==CALL)
      return 0;
    if(p->code>=BEQ&&p->code<=BRA)
      return 0;
    if(p->code==FREEREG||p->code==ALLOCREG){
      if(p->q1.reg==r)
	return 1;
      if(reg_pair(p->q1.reg,&rp)&&(rp.r1==r||rp.r2==r))
	return 1;
    }
    if(p->q1.am) chk_coll(p->q1.am->base);
    if(p->q2.am) chk_coll(p->q2.am->base);
    if(p->z.am) chk_coll(p->z.am->base);
    if(p->q1.flags&REG) chk_coll(p->q1.reg);
    if(p->q2.flags&REG) chk_coll(p->q2.reg);
    if(p->z.flags&REG){
      if(p->z.flags&DREFOBJ)
	chk_coll(p->z.reg);
      else{
	if(p->z.reg==r)
	  return 1;
	if(reg_pair(p->z.reg,&rp)&&(rp.r1==r||rp.r2==r))
	  return 1;
      }
    }

    p=p->next;
  }
  return 1;
}

static int rsavecur;
static int in_isr;

static int get_reg(FILE *f,IC *p,int t)
{
  int r,r1,r2,pass,flag;

  for(pass=0;pass<5;pass++){
    for(r=MAXR;r>sp;r--){
      if(reg_pair(r,&rp)){
	r1=rp.r1;
	r2=rp.r2;
      }else{
	r1=0;
	r2=0;
      }
      if((pass==0||pass==3)&&(!regscratch[r]||in_isr))
	continue;
      if(pass<3){
	if(regs[r]) continue;
	if(r1&&(regs[r1]||regs[r2])) continue;
      }
      if(pass==2&&!(regs[r]&4))
	continue;
      if((p->q1.flags&REG)&&(p->q1.reg==r||p->q1.reg==r1||p->q1.reg==r2)) continue;
      if((p->q2.flags&REG)&&(p->q2.reg==r||p->q2.reg==r1||p->q2.reg==r2)) continue;
      if((p->z.flags&REG)&&(p->z.reg==r||p->z.reg==r1||p->z.reg==r2)) continue;
      if(regok(r,t,1)){
	flag=8;
	if(regs[r]){
	  flag|=2;
	  if(p->code==COMPARE||p->code==TEST)
	    ierror(0);
	  if(regs[ra]){
	    emit(f,"\ttay\n");
	    yval=NOVAL;
	  }
	  if(r1){
	    emit(f,"\tlda\t%s\n",mregnames[r1]);
	    emit(f,"\tpha\n");
	    emit(f,"\tlda\t%s\n",mregnames[r2]);
	    emit(f,"\tpha\n");
	  }else{
	    emit(f,"\tlda\t%s\n",mregnames[r]);
	    emit(f,"\tpha\n");
	  }
	  if(regs[ra])
	    emit(f,"\ttya\n");
	}
	if(r1){
	  regs[r1]|=flag;
	  regs[r2]|=flag;
	}
	regs[r]|=flag;
	regused[r]=1;
	regused[r1]=1;
	regused[r2]=1;
	return r;
      }
    }
  }
  pric2(stdout,p);
  ierror(0);
}

static void get_acc(FILE *f, IC *p,int t)
{
  int r;
  if(isacc(z)) return;
  t&=NQ;
  if(ISCHAR(t)){
    if(1/*!isreg(q1)||(p->q1.reg!=ra&&p->q1.reg!=rax)*/){
      if((regs[ra]||regs[rax])&&!scratch(p,ra,0)&&!pushedacc){
	if(storedacc)
	  pushedacc=storedacc;
	else if(optsize||(regs[t1]&&regs[t2]&&regs[t3]&&regs[t4])){
	  emit(f,"\tpha\n");
	  pushedacc=-1;
	}else{
	  r=get_reg(f,p,CHAR);
	  emit(f,"\tsta\t%s\n",mregnames[r]);
	  pushedacc=r;
	}
      }
    }
  }else{
    if(1/*!isreg(q1)||p->q1.reg!=rax*/){
      if((regs[ra]||regs[rax])&&(!scratch(p,ra,0)||!scratch(p,rax,0))&&!pushedacc){
	if(!storedacc){
	  r=get_reg(f,p,CHAR);
	  emit(f,"\tsta\t%s\n",mregnames[r]);
	  pushedacc=r;
	}else
	  pushedacc=storedacc;
      }
      if((regs[rx]||regs[rax])&&!scratch(p,rax,0)&&!pushedx){
	if(!storedx){
	  r=get_reg(f,p,CHAR);
	  emit(f,"\tstx\t%s\n",mregnames[r]);
	  pushedx=r;
	}else
	  pushedx=storedx;
      }
    }
  }
}

static int cmp_get_acc(FILE *f,IC *p,IC *branch)
{
  if(!regs[ra]&&!regs[rax])
    return 0;
  if(branch==0&&pushedacc)
    return 0;
  if(branch&&isreg(q1)&&(p->q1.reg==ra||p->q1.reg==rax))
    if(branch->code==BEQ||branch->code==BNE||(p->typf&UNSIGNED))
      return 0;
  if(scratch(p,ra,0))
    return 0;
  if(!regs[rx]&&!regs[rax]){
    emit(f,"\ttax\n");
    pushedacc=rx;
  }
  emit(f,"\tpha\n");
  pushedacc=-1;
  return pushedacc;
}

static void reload_acc_opt(FILE *f,IC *p)
{
  if(pushedacc==0) return;
  if(pushedacc>0){
    while(p){
      if(p->code!=FREEREG) break;
      if(p->q1.reg==ra||p->q1.reg==rax){
	pushedacc=0;
	return;
      }
      p=p->next;
    }
  }
  if(pushedacc==-1)
    emit(f,"\tpla\n");
  else if(pushedacc==rx)
    emit(f,"\ttxa\n");
  else if(pushedacc==ry)
    emit(f,"\ttya\n");
  else if(pushedacc){
    emit(f,"\tlda\t%s\n",mregnames[pushedacc]);
    regs[pushedacc]&=~8;
  }
  pushedacc=0;
}

static void reload_acc(FILE *f)
{
  reload_acc_opt(f,0);
}

static void push(int i)
{
  pushed-=i;
  if(pushed<maxpushed)
    maxpushed=pushed;
}

static void pop(int i)
{
  pushed+=i;
  if(pushed>0) ierror(0);
}

static int indirect(obj *o)
{
  if(o->am){
    if(o->am->flags==ABS_IND&&o->am->idx==rx)
      return 0;
    else
      return 1;
  }
  if((o->flags&(DREFOBJ|KONST))==DREFOBJ)
    return 1;
  if((o->flags&(REG|VAR))!=VAR){
    if((o->flags&REG)&&(o->reg==ra||ISIDX(o->reg)||o->reg==rax))
      return 1;
    return 0;
  }
  if(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)
    return 1;
  return 0;
}

void convfloat(void)
{
}  

void sety(FILE *f,int val)
{
  if(yval==val)
    return;
  if(val-yval==1)
    emit(f,"\tiny\n");
  else if(yval-val==1)
    emit(f,"\tdey\n");
  else{
    emit(f,"\tldy\t#%d\n",val);
    if(val<0||val>255)
      ierror(0);
  }
  yval=val;
}

static void cnv_fp(void)
{
  double d,mant;
  int exp;
  unsigned long t;

  if(ieee){
    vfloat=zld2zf(vldouble);
    memcpy((void*)&vmax,(void*)&vfloat,4);
  }else{
    d=zld2d(vldouble);
    mant=frexp(d,&exp);
    exp=(exp+127)&255;
    t=((unsigned long)(mant*8388608))&0xffffff;
    t|=((long)exp)<<24;
    
    t=((t&0xff)<<24)|((t&0xff00)<<8)|((t&0xff0000)>>8)|((t&0xff000000)>>24);
    vmax=l2zm((long)t);
    if(mant==0&&d==0) vmax=Z0;
  }
}

static void emit_ieee(FILE *f,union atyps *p,int t)
{
  unsigned char *ip=(unsigned char *)&p->vdouble;
  emit(f,"0x%02x%02x,0x%02x%02x",ip[1],ip[0],ip[3],ip[2]);
  if(t==DOUBLE||t==LDOUBLE)
    emit(f,",0x%02x%02x,0x%02x%02x",ip[5],ip[4],ip[7],ip[6]);
  emit(f,"\n");
}

static void emit_lobyte(FILE *f,obj *o,int t)
{
  if(o->flags&KONST){
    if(o->flags&DREFOBJ){
      eval_const(&o->val,o->dtyp);
       emit(f,"%d",zm2l(vmax));
    }else{
      eval_const(&o->val,t);
      if(ISFLOAT(t)) cnv_fp();
      emit(f,"#%d",zm2l(vmax)&255);
    }
  }else if(o->flags&VARADR){
    emit(f,"#<(");
    emit_obj(f,o,t);
    emit(f,")");
  }else if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(o->reg,&rp))
      emit(f,"%s",mregnames[o->reg]);
    else
      emit(f,"%s",mregnames[rp.r1]);
  }else{
    if(zzero&&(o->flags&(DREFOBJ|KONST))==DREFOBJ) noy=1;
    emit_obj(f,o,t);
    noy=0;
  }
}

static void emit_hibyte(FILE *f,obj *o,int t)
{
  if(o->flags&KONST){
    if(o->flags&DREFOBJ){
      eval_const(&o->val,o->dtyp);
      emit(f,"%d",zm2l(vmax)+1);
    }else{
      eval_const(&o->val,t);
      if(ISFLOAT(t)) cnv_fp();
      emit(f,"#%d",(zm2l(vmax)>>8)&255);
    }
  }else if(o->flags&VARADR){
    emit(f,"#>(");
    emit_obj(f,o,t);
    emit(f,")");
  }else if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg>=FIRST_BIG&&o->reg<=LAST_BIGP){
      emit(f,"%s+1",mregnames[o->reg]);
    }else{
      if(!reg_pair(o->reg,&rp))
	ierror(0);
      emit(f,"%s",mregnames[rp.r2]);
    }
  }else{
    if(o->flags&VARADR)
      emit(f,"#");
    if(!(o->flags&DREFOBJ))
      o->val.vmax=zmadd(o->val.vmax,Z1);
    emit_obj(f,o,t);
    if(!(o->flags&DREFOBJ))
      o->val.vmax=zmsub(o->val.vmax,Z1);
  }
}

static void emit_byte3(FILE *f,obj *o,int t)
{
  if(o->flags&KONST){
    if(o->flags&DREFOBJ){
      eval_const(&o->val,o->dtyp);
      emit(f,"%d",zm2l(vmax)+2);
    }else{
      eval_const(&o->val,t);
      if(ISFLOAT(t)) cnv_fp();
      emit(f,"#%d",(zm2l(vmax)>>16)&255);
    }
  }else if((o->flags&(REG|DREFOBJ))==REG){
    emit(f,"%s+2",mregnames[o->reg]);
  }else if(o->flags&VARADR){
    emit(f,"#%d",bank(o->v));
  }else{
    if(!(o->flags&DREFOBJ))
      o->val.vmax=zmadd(o->val.vmax,l2zm(2L));
    emit_obj(f,o,t);
    if(!(o->flags&DREFOBJ))
      o->val.vmax=zmsub(o->val.vmax,l2zm(2L));
  }
}

static void emit_byte4(FILE *f,obj *o,int t)
{
  if(o->flags&KONST){
    if(o->flags&DREFOBJ){
      eval_const(&o->val,o->dtyp);
      emit(f,"%d",zm2l(vmax)+3);
    }else{
      eval_const(&o->val,t);
      if(ISFLOAT(t)) cnv_fp();
      emit(f,"#%d",(zm2l(vmax)>>24)&255);
    }
  }else if((o->flags&(REG|DREFOBJ))==REG){
    emit(f,"%s+3",mregnames[o->reg]);
  }else{
    if(!(o->flags&DREFOBJ))
      o->val.vmax=zmadd(o->val.vmax,l2zm(3L));
    emit_obj(f,o,t);
    if(!(o->flags&DREFOBJ))
      o->val.vmax=zmsub(o->val.vmax,l2zm(3L));
  }
}

static void do_lobyte(FILE *f,char *s,obj *o,int type)
{
  if(o->am){
    if(o->am->flags==IMM_IND)
      sety(f,o->am->offset);
    else if(o->am->flags==GPR_IND||o->am->flags==ABS_IND){
      if(o->am->idx==ra)
	emit(f,"\ttay\n");
      else{
	if(ISIDX(o->am->idx)){
	  if(o->am->flags==GPR_IND){
	    emit(f,"\tpha\n");
	    emit(f,"\tt%sa\n",mregnames[o->am->idx]);
	    emit(f,"\ttay\n");
	    emit(f,"\tpla\n");
	  }
	}else
	  emit(f,"\tldy\t%s\n",mregnames[o->am->idx]);
      }
      yval=NOVAL;
    }else
      ierror(0);
  }else if((o->flags&(DREFOBJ|KONST))==DREFOBJ){
    if(!zzero) sety(f,0);
  }else if((o->flags&(VAR|REG|VARADR))==VAR&&(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)){
    sety(f,(int)real_offset(o));
  }
  emit(f,"\t%s\t",s);
  emit_lobyte(f,o,type);
  emit(f,"\n");
}

static void do_hibyte(FILE *f,char *s,obj *o,int type)
{
  int ami=0;
  if(o->am){
    if(o->am->flags==IMM_IND)
      sety(f,o->am->offset+1);
    else if(o->am->flags==ABS_IND){
      if(o->am->offset==ra)
	emit(f,"\ttay\n");
      else
	emit(f,"\tldy\t%s\n",mregnames[o->am->idx]);
      o->am->offset++;ami=1;
      yval=NOVAL;
    }else
      ierror(0);
  }else  if((o->flags&(DREFOBJ|KONST))==DREFOBJ){
    sety(f,1);
  }else if((o->flags&(VAR|REG|VARADR))==VAR&&(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)){
    sety(f,(int)real_offset(o)+1);
  }
  emit(f,"\t%s\t",s);
  emit_hibyte(f,o,type);
  emit(f,"\n");
  if(ami) o->am->offset--;
}

static void do_byte3(FILE *f,char *s,obj *o,int type)
{
  int ami=0;
  if(o->am){
    if(o->am->flags==IMM_IND)
      sety(f,o->am->offset+2);
    else if(o->am->flags==ABS_IND){
      if(o->am->offset==ra)
	emit(f,"\ttay\n");
      else
	emit(f,"\tldy\t%s\n",mregnames[o->am->idx]);
      yval=NOVAL;
      o->am->offset+=2;ami=1;
    }else
      ierror(0);
  }else  if((o->flags&(DREFOBJ|KONST))==DREFOBJ){
    sety(f,2);
  }else if((o->flags&(VAR|REG|VARADR))==VAR&&(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)){
    sety(f,(int)real_offset(o)+2);
  }
  emit(f,"\t%s\t",s);
  emit_byte3(f,o,type);
  emit(f,"\n");
  if(ami) o->am->offset-=2;
}

static void do_byte4(FILE *f,char *s,obj *o,int type)
{
  int ami=0;
  if(o->am){
    if(o->am->flags==IMM_IND)
      sety(f,o->am->offset+3);
    else if(o->am->flags==ABS_IND){
      if(o->am->offset==ra)
	emit(f,"\ttay\n");
      else
	emit(f,"\tldy\t%s\n",mregnames[o->am->idx]);
      yval=NOVAL;
      o->am->offset+=3;ami=1;
    }else
      ierror(0);
  }else  if((o->flags&(DREFOBJ|KONST))==DREFOBJ){
    sety(f,3);
  }else if((o->flags&(VAR|REG|VARADR))==VAR&&(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)){
    sety(f,(int)real_offset(o)+3);
  }
  emit(f,"\t%s\t",s);
  emit_byte4(f,o,type);
  emit(f,"\n");
  if(ami) o->am->offset-=3;
}

static void load_lobyte(FILE *f,obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==ra||o->reg==rax)
      return;
    if(o->reg==rx){
      emit(f,"\ttxa\n");
      return;
    }
  }
  do_lobyte(f,"lda",o,t);
}

static void load_hibyte(FILE *f,obj *o,int t)
{
  if((o->flags&REG)&&(o->reg==rx||o->reg==rax))
    emit(f,"\ttxa\n");
  else
    do_hibyte(f,"lda",o,t);
}

static void store_lobyte(FILE *f,obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==ra||o->reg==rax)
      return;
    if(o->reg==rx){
      emit(f,"\ttax\n");
      return;
    }
  }
  do_lobyte(f,"sta",o,t);
}

static void store_hibyte(FILE *f,obj *o,int t)
{
  if((o->flags&REG)&&(o->reg==rx||o->reg==rax))
    emit(f,"\ttax\n");
  else
    do_hibyte(f,"sta",o,t);
}

static void load_acc(FILE *f,obj *o,int type)
{
  if((o->flags&REG)&&(o->reg==ra||o->reg==rax))
    return;
  if(!ISCHAR(type)){
    if(indirect(o)){
      do_hibyte(f,"lda",o,type);
      emit(f,"\ttax\n");
    }else
      do_hibyte(f,"ldx",o,type);
  }
  if((o->flags&(REG|DREFOBJ))==REG&&o->reg==rx)
    emit(f,"\ttxa\n");
  else
    do_lobyte(f,"lda",o,type);
}

static void store_acc(FILE *f,obj *o,int type)
{
  if((o->flags&(DREFOBJ|KONST))==DREFOBJ&&((!(o->flags&REG))||!isptr(o->reg))){
    ierror(0);
  }
  if((o->flags&REG)&&(o->reg==ra||o->reg==rax))
    return;
  if((o->flags&REG)&&o->reg==rx){
    emit(f,"\ttax\n");
    return;
  }
  store_lobyte(f,o,type);
  if(!ISCHAR(type)){
    if(indirect(o)){
      /*TODO: save accu */
      emit(f,"\ttxa\n");
      store_hibyte(f,o,type);
    }else
      do_hibyte(f,"stx",o,type);
  }
}

static void load_reg(FILE *f,int r,struct obj *o,int type)
{
  static obj ro;
  if(ISIDX(r)&&!indirect(o)){
    static char ldr[4]="ldr";
    ldr[2]=mregnames[r][0];
    do_lobyte(f,ldr,o,type);
    return;
  }
  ro.flags=REG;
  ro.reg=r;
  load_lobyte(f,o,type);
  store_lobyte(f,&ro,type);
  if(!ISCHAR(type)){
    load_hibyte(f,o,type);
    store_hibyte(f,&ro,type);
  }
}

static void store_reg(FILE *f,int r,struct obj *o,int type)
{
  static obj ro;
  if(ISIDX(r)){
    if((o->flags&(REG|DREFOBJ))==REG){
      if(o->reg==r)
	return;
      if(o->reg==ra){
	emit(f,"\tt%sa\n",mregnames[r]);
	return;
      }
    }
    if(!indirect(o)){
      static char str[4]="str";
      str[2]=mregnames[r][0];
      do_lobyte(f,str,o,type);
      return;
    }
  }
  ro.flags=REG;
  ro.reg=r;
  if(r!=ra&&r!=rax)
    load_acc(f,&ro,type);
  store_acc(f,o,type);
}

static struct fpconstlist {
  struct fpconstlist *next;
  int label;
  int t;
  union atyps val;
} *firstfpc;

static int addfpconst(struct obj *o,int t)
{
  struct fpconstlist *p=firstfpc;
  t&=NQ;
  if(t==LDOUBLE) t=DOUBLE;
  for(p=firstfpc;p;p=p->next){
    if(t==p->t){
      eval_const(&p->val,t);
      if(t==FLOAT&&zldeqto(vldouble,zf2zld(o->val.vfloat))) return p->label;
      if(t==DOUBLE&&zldeqto(vldouble,zd2zld(o->val.vdouble))) return p->label;
      if(t==LONG&&zmeqto(vmax,zl2zm(o->val.vlong))) return p->label;
      if(t==LLONG&&zmeqto(vmax,zll2zm(o->val.vllong))) return p->label;
    }
  }
  p=mymalloc(sizeof(struct fpconstlist));
  p->next=firstfpc;
  p->t=t;
  p->label=++label;
  p->val=o->val;
  firstfpc=p;
  return p->label;
}

/* generate code to load the address of a local variable into register r */
static void load_laddr(FILE *f,int r,struct obj *o)
{
  long l=real_offset(o);
  /* assumes acc is available */
  if(!reg_pair(r,&rp)) ierror(0);
  emit(f,"\tlda\t%s\n",mregnames[fp]);
  if(l!=0)
    emit(f,"\tclc\n");
  if(l&255)
    emit(f,"\tadc\t#%ld\n",l&255);
  emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
  emit(f,"\tlda\t%s\n",mregnames[fp2]);
  if(l!=0)
    emit(f,"\tadc\t#%ld\n",(l>>8)&255);
  emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
}

/* generate code to load the address of a variable into register r */
static void load_address(FILE *f,int r,struct obj *o,int t)
{
  if(o->flags&DREFOBJ){
    o->flags&=~DREFOBJ;
    load_reg(f,r,o,POINTER);
    o->flags|=DREFOBJ;
  }else if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(r,&rp)) ierror(0);
    emit(f,"\tlda\t#>%s\n",mregnames[o->reg]);
    emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
    emit(f,"\tlda\t#<%s\n",mregnames[o->reg]);
    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);    
  }else if(o->flags&VAR){
    if(o->v->storage_class==AUTO||o->v->storage_class==REGISTER){
      load_laddr(f,r,o);
    }else{
      o->flags|=VARADR;
      load_reg(f,r,o,POINTER);
      o->flags&=~VARADR;
    }
  }else if((o->flags&(KONST|DREFOBJ))==KONST){
    int l=addfpconst(o,t);
    if(!ieee) ierror(0);
    if(!reg_pair(r,&rp)) ierror(0);
    emit(f,"\tlda\t#>%s%d\n",labprefix,l);
    emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
    emit(f,"\tlda\t#<%s%d\n",labprefix,l);
    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);    
  }else
    ierror(0);
}


/*  Yields log2(x)+1 or 0. */
static long pof2(zumax x)
{
  zumax p;int ln=1;
  p=ul2zum(1L);
  while(ln<=32&&zumleq(p,x)){
    if(zumeqto(x,p)) return ln;
    ln++;p=zumadd(p,p);
  }
  return 0;
}

static void incmem(FILE *f,obj *o,int t,int op,int x)
{
  int i,idx=0;
  char *s;
  if(ce02&&ISSHORT(t)){
    if(((o->flags&(REG|DREFOBJ))==REG&&(op==ADD||op==SUB))||
       (op==LSHIFT&&!indirect(o)&&!o->am)){
      do{
	emit(f,"\t%s\t%s\n",op==ADD?"inw":(op==SUB?"dew":"asw"),mregnames[o->reg]);
      }while(--x);
      return;
    }
  }
  if((o->flags&(REG|DREFOBJ))==REG&&ISIDX(o->reg)){
    static char buf[4]="   ";
    idx=1;s=buf;
    if(op==ADD){s[0]='i';s[1]='n';}else{s[0]='d';s[1]='e';}
    s[2]=mregnames[o->reg][0];
  }else if(op==ADD)
    s="inc";
  else if(op==SUB)
    s="dec";
  else if(op==LSHIFT)
    s="asl";
  else if(op==RSHIFT&&(t&UNSIGNED))
    s="clc\n\tror";
  else if(op==RSHIFT){
    s="cmp\t#128\n\tror";
  }else
    ierror(0);
  if(ISCHAR(t)){
    for(i=0;i<x;i++){
      if(idx){
	emit(f,"\t%s\n",s);
      }else{
	emit(f,"\t%s\t",s);
	emit_obj(f,o,t);
	emit(f,"\n");
      }
    }
  }else{
    for(i=0;i<x;i++){
      if(op==SUB){
	if(m65&&ISLONG(t)){
	  emit(f,"\tdeq\t");
	  emit_obj(f,o,t);
	  emit(f,"\n");
	}else if(ce02&&iszpage(o)){
	  emit(f,"\tdew\n");
	}else{
	  /* caller mus make sure accu is available */
	  load_lobyte(f,o,t);
	  emit(f,"\tbne\t%s%d\n",labprefix,++label);
	  emit(f,"\tdec\t");
	  emit_hibyte(f,o,t);
	  emit(f,"\n");
	  if(ISLONG(t)){
	    ierror(0);
	  }
	  emit(f,"%s%d:\n",labprefix,label);
	  emit(f,"\tdec\t");
	  emit_lobyte(f,o,t);
	  emit(f,"\n");
	}
      }else if(op==ADD){
	if(!optspeed&&m65&&ISLONG(t)){
	  emit(f,"\tinq\t");
	  emit_obj(f,o,t);
	  emit(f,"\n");
	}else if(!optspeed&&ce02&&iszpage(o)){
	  emit(f,"\tinw\t");
	  emit_lobyte(f,o,t);
	  emit(f,"\n");
	  if(ISLONG(t)){
	    emit(f,"\tbne\t%s%d\n",labprefix,++label);
	    emit(f,"\tinw\t");
	    emit_byte3(f,o,t);
	    emit(f,"\n");
	    emit(f,"%s%d:\n",labprefix,label);
	  }
	}else{
	  emit(f,"\t%s\t",s);
	  emit_lobyte(f,o,t);
	  emit(f,"\n");
	  emit(f,"\tbne\t%s%d\n",labprefix,++label);
	  emit(f,"\t%s\t",s);
	  emit_hibyte(f,o,t);
	  emit(f,"\n");
	  if(ISLONG(t)){
	    emit(f,"\tbne\t%s%d\n",labprefix,label);
	    emit(f,"\t%s\t",s);
	    emit_byte3(f,o,t);
	    emit(f,"\n");
	    emit(f,"\tbne\t%s%d\n",labprefix,label);
	    emit(f,"\t%s\t",s);
	    emit_byte4(f,o,t);
	    emit(f,"\n");
	  }
	  emit(f,"%s%d:\n",labprefix,label);
	}
      }else if(op==LSHIFT){
	if(ce02&&!indirect(o)&&!o->am){
	  emit(f,"\tasw\t");
	  emit_obj(f,o,t);
	  emit(f,"\n");
	}else{
	  emit(f,"\tasl\t");
	  emit_lobyte(f,o,t);
	  emit(f,"\n");
	  emit(f,"\trol\t");
	  emit_hibyte(f,o,t);
	  emit(f,"\n");
	}
      }else if(op==RSHIFT&&(t&UNSIGNED)){
	/*emit(f,"\tclc\n");
	  emit(f,"\tror\t");*/
	emit(f,"\tlsr\t");
	emit_hibyte(f,o,t);
	emit(f,"\n");
	emit(f,"\tror\t");
	emit_lobyte(f,o,t);
	emit(f,"\n");
      }else if(op==RSHIFT){
	if(ce02){
	  emit(f,"\tasr\t");
	}else{
	  load_hibyte(f,o,t);
	  emit(f,"\tcmp\t#128\n");
	  emit(f,"\tror\t");
	}
	emit_hibyte(f,o,t);
	emit(f,"\n");
	emit(f,"\tror\t");
	emit_lobyte(f,o,t);
	emit(f,"\n");	
      }else{
	printf("op=%d\n",op);
	ierror(0);
      }
    }
  }
}

static void preload_obj(FILE *f,IC *p,obj *o)
{
  int r,pa=0,px=0,longm65=0;long of;

  if((p->typf&VOLATILE)||(p->typf2&VOLATILE)||
     ((p->q1.flags&DREFOBJ)&&(p->q1.dtyp&(VOLATILE|PVOLATILE)))||
     ((p->q2.flags&DREFOBJ)&&(p->q2.dtyp&(VOLATILE|PVOLATILE)))||
     ((p->z.flags&DREFOBJ)&&(p->z.dtyp&(VOLATILE|PVOLATILE))))
    emit(f,"; volatile barrier\n");


  if(m65){
    int t;
    if(o==&p->q1) t=q1typ(p);
    else if(o==&p->q2) t=q2typ(p);
    else if(o==&p->z) t=ztyp(p);
    else ierror(0);
    if(ISLONG(t)&&LONGM65(p->code)) longm65=1;
  }

  if((o->flags&(VAR|REG))==VAR&&(o->v->storage_class==AUTO||o->v->storage_class==REGISTER)&&(((of=real_offset(o))+zm2l(szof(o->v->vtyp))>255)||longm65)){
    r=get_reg(f,p,POINTER);
    if(o->flags&DREFOBJ){
      if(p->code==GETRETURN&&(p->q1.reg==ra||p->q1.reg==rax)){
	emit(f,"\tsta\t%s\n",mregnames[t1]);
	emit(f,"\tstx\t%s\n",mregnames[t2]);
	pa=px=1;
      }else
	get_acc(f,p,INT);
    }else{
      if(p->code==GETRETURN&&(p->q1.reg==ra||p->q1.reg==rax)){
	emit(f,"\tsta\t%s\n",mregnames[t1]);
	pa=1;
      }else
	cmp_get_acc(f,p,0);
    }
    if(!reg_pair(r,&rp)) ierror(0);
    emit(f,"\tlda\t%s\n",mregnames[fp1]);
    emit(f,"\tclc\n");
    if(of&0xff)
      emit(f,"\tadc\t#%ld\n",of&0xff);
    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
    emit(f,"\tlda\t%s\n",mregnames[fp2]);
    if(1/*of&0xff00*/)
      emit(f,"\tadc\t#%ld\n",(of>>8)&0xff);
    emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
    o->flags|=REG;
    o->reg=r;
    if(o->flags&DREFOBJ){
      if(!zzero) sety(f,0);
      emit(f,"\tlda\t(%s)%s\n",mregnames[r],zzero?"":",y");
      emit(f,"\ttxa\n");
      sety(f,1);
      emit(f,"\tlda\t(%s),y\n",mregnames[r]);
      emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
      emit(f,"\tstx\t%s\n",mregnames[rp.r1]);
    }else{
      o->flags|=DREFOBJ;
      o->dtyp=POINTER;
    }
    if(pa){ emit(f,"\tlda\t%s\n",mregnames[t1]); pa=0;}
    if(px){ emit(f,"\tldx\t%s\n",mregnames[t2]); px=0;}
  }

  if((o->flags&(DREFOBJ|KONST))==DREFOBJ&&(!(o->flags&REG)||!isptr(o->reg))&&(!(o->flags&VAR)||!(o->v->tattr&ZPAGE))&&!ISFPOINTER(o->dtyp)){
    if(p->code==GETRETURN&&(p->q1.reg==ra||p->q1.reg==rax)){
      emit(f,"\tsta\t%s\n",mregnames[t1]);
      pa=1;
    }else
      cmp_get_acc(f,p,0);
    r=get_reg(f,p,POINTER);
    o->flags&=~DREFOBJ;
    load_reg(f,r,o,POINTER);
    o->flags|=REG|DREFOBJ;
    o->reg=r;
    if(pa){ emit(f,"\tlda\t%s\n",mregnames[t1]); pa=0;}
  }
}

static void far_copy(FILE *f,IC *p)
{
  int b;long l;
  get_acc(f,p,INT);
  if(p->code==PUSH){
    if(!reg_pair(LAST_PAIR,&rp)) ierror(0);
    emit(f,"\tlda\t%s\n",mregnames[sp]);
    if(pushed) emit(f,"\tclc\n");
    if(pushed&0xff) emit(f,"\tadc\t#%d\n",(pushed&0xff));
    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
    emit(f,"\tlda\t%s+1\n",mregnames[sp]);
    if(pushed) emit(f,"\tadc\t#%d\n",((pushed>>8)&0xff));
    emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
    pushed+=zm2l(p->q2.val.vmax);
    emit(f,"\tldx\t#%d\n",bankcnum);
  }else if(p->z.flags&DREFOBJ){
    if(!reg_pair(LAST_PAIR,&rp)) ierror(0);
    p->z.flags&=~DREFOBJ;
    load_lobyte(f,&p->z,FPOINTER);
    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
    load_hibyte(f,&p->z,FPOINTER);
    emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
    if(indirect(&p->z)){
      do_byte3(f,"lda",&p->z,FPOINTER);
      emit(f,"\ttax\n");
    }else
      do_byte3(f,"ldx",&p->z,FPOINTER);
  }else{
    load_address(f,LAST_PAIR,&p->z,POINTER);
    b=bank(p->z.v);
    emit(f,"\tldx\t#%d\n",b>=0?b:bankcnum);
  }
  if(p->q1.flags&DREFOBJ){
    if(!reg_pair(LAST_PAIR-1,&rp)) ierror(0);
    p->q1.flags&=~DREFOBJ;
    load_lobyte(f,&p->q1,FPOINTER);
    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
    load_hibyte(f,&p->q1,FPOINTER);
    emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
    if(indirect(&p->q1)){
      do_byte3(f,"lda",&p->q1,FPOINTER);
      emit(f,"\ttay\n");
    }else
      do_byte3(f,"ldy",&p->q1,FPOINTER);
  }else{
    load_address(f,LAST_PAIR-1,&p->q1,POINTER);
    b=bank(p->q1.v);
    sety(f,b>=0?b:bankcnum);
  }
  l=zm2l(p->q2.val.vmax);
  emit(f,"\tlda\t#%d\n",(l>>8)&0xff);
  emit(f,"\tsta\t%s__bankcopy_len+1\n",idprefix);
  emit(f,"\tlda\t#%d\n",(l)&0xff);
  emit(f,"\tsta\t%s__bankcopy_len\n",idprefix);
  emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
  emit(f,"\tjsr\t%s__bankcopy\n",idprefix);
  yval=NOVAL;
}

static void load_far(FILE *f,IC *p,obj *o,int t)
{
  int pushed=0;
  if(!(o->flags&DREFOBJ)) ierror(0);
  o->flags&=~DREFOBJ;
  t&=NQ;
  if(zmeqto(sizetab[t],Z0)) return;
  /*get_acc(f,p,INT);*/
  if(pushedacc==t3||pushedacc==t4) ierror(0);
  if(pushedx==t3||pushedx==t4) ierror(0);
  if(regs[ra]||regs[rax]){
    if(isacc(q1)||isacc(q2)||!isacc(z)){
      pushed=1;
      emit(f,"\tpha\n");
      if(regs[rax]) emit(f,"\ttxa\n\tpha\n");
    }
  }
  load_reg(f,LAST_PAIR,o,POINTER);
  if(!indirect(o))
    do_byte3(f,"ldy",o,CHAR);
  else{
    do_byte3(f,"lda",o,CHAR);
    emit(f,"\ttay\n");
  }
  emit(f,"\tldx\t#%d\n",bankvoffset);
  emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
  emit(f,"\tjsr\t%s__bankload%d\n",idprefix,(int)zm2l(sizetab[t]));
  yval=NOVAL;
  o->flags=VAR;
  o->v=&bankv;
  o->val.vmax=l2zm((long)bankvoffset);
  bankvoffset+=zm2l(sizetab[t]);
  if(pushed){
    if(regs[rax]) emit(f,"\tpla\n\ttax\n");
    emit(f,"\tpla\n");
  }
}

static void load_banked(FILE *f,IC *p,obj *o,int t)
{
  int pushed=0,m;
  if(o->flags&DREFOBJ) t=o->dtyp;
  t&=NQ;
  if(zmeqto(sizetab[t],Z0)) return;
  /*get_acc(f,p,INT);*/
  if(pushedacc==t3||pushedacc==t4) ierror(0);
  if(pushedx==t3||pushedx==t4) ierror(0);
  if(regs[ra]||regs[rax]){
    if(isacc(q1)||isacc(q2)||!isacc(z)){
      pushed=1;
      emit(f,"\tpha\n");
      if(regs[rax]||regs[rx]) emit(f,"\ttxa\n\tpha\n");
    }
  }
  m=o->flags;
  o->flags&=~DREFOBJ;
  load_address(f,LAST_PAIR,o,POINTER);
  o->flags=m;
  emit(f,"\tldx\t#%d\n",bankvoffset);
  emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
  sety(f,bank(o->v));
  emit(f,"\tjsr\t%s__bankload%d\n",idprefix,(int)zm2l(sizetab[t]));
  yval=NOVAL;
  o->v=&bankv;
  o->val.vmax=l2zm((long)bankvoffset);
  bankvoffset+=zm2l(sizetab[t]);
  if(pushed){
    if(regs[rax]||regs[rx]) emit(f,"\tpla\n\ttax\n");
    emit(f,"\tpla\n");
  }
}

static void preload(FILE *f,IC *p)
{
  int r,mra=regs[ra],mrax=regs[rax];
  int bq1=-1,bq2=-1,bz=-1,sb=-1,zbuf=0;

  if(p->code==GETRETURN&&p->q1.reg==ra&&!regs[ra])
    regs[ra]=1;

  if(p->code==GETRETURN&&p->q1.reg==rax&&!regs[rax])
    regs[rax]=1;

  bankvoffset=0;

  if(!NOBANKVARS){
    if((p->q1.flags&(VAR|VARADR))==VAR) bq1=bank(p->q1.v);
    if((p->q2.flags&(VAR|VARADR))==VAR) bq2=bank(p->q2.v);
    if((p->z.flags&(VAR|VARADR))==VAR) bz=bank(p->z.v);

    if((p->q1.flags&(VAR|VARADR))==(VAR|VARADR)){
      r=bank(p->q1.v);
      /*if(r>=0&&r!=cbank) ierror(0);*/
    }
    
    if((p->code==ASSIGN||p->code==PUSH)&&!zmleq(p->q2.val.vmax,l2zm(4L))){
      if(p->q1.flags&DREFOBJ) preload_obj(f,p,&p->q1);
      if(p->z.flags&DREFOBJ) preload_obj(f,p,&p->z);
      return;
    }

    if(p->code!=CALL){
      /* TODO: some optimizations possible */
      if(bq1>=0&&bq1!=cbank){
	if(cbank<0&&!NOSWITCH)
	  sb=bq1;
	else
	  load_banked(f,p,&p->q1,q1typ(p));
      }
      if(bq2>=0&&bq2!=cbank){
	if((bq2==sb||(cbank<0&&sb<0))&&!NOSWITCH)
	  sb=bq2;
	else
	  load_banked(f,p,&p->q2,q2typ(p));
      }
      if(bz>=0&&bz!=cbank&&(p->z.flags&DREFOBJ)){
	if((bz==sb||(cbank<0&&sb<0))||!NOSWITCH)
	  sb=bz;
	else
	  load_banked(f,p,&p->z,ztyp(p));
      }
      
      if(sb>=0){
	if(NOSWITCH) ierror(0);
	sety(f,sb);
	emit(f,"\tjsr\t%s__bankswitch\n",idprefix);
	yval=NOVAL;
      }
    }
  }

  if((p->q1.flags&DREFOBJ)&&ISFPOINTER(p->q1.dtyp)&&p->code!=CALL) load_far(f,p,&p->q1,q1typ(p));
  if((p->q2.flags&DREFOBJ)&&ISFPOINTER(p->q2.dtyp)) load_far(f,p,&p->q2,q2typ(p));

  if(isacc(q2)){
    static obj o;
    int t=q2typ(p);
    r=get_reg(f,p,t);
    o.flags=REG;
    o.reg=r;
    store_acc(f,&o,t);
    p->q2.reg=r;
    if(!pushedacc){
      if((t&NQ)==CHAR)
	storedacc=r;
      else{
	if(!reg_pair(r,&rp)) ierror(0);
	storedacc=rp.r1;
	storedx=rp.r2;
      }
    }
  }

  if((p->code==ADDI2P||p->code==SUBIFP)&&ISRIDX(q2)){
    static obj o;
    if(p->q2.reg==rx){
      if(pushedx>0) r=pushedx;
      else if(storedx>0) r=storedx;
      else r=get_reg(f,p,CHAR);
    }else
      r=get_reg(f,p,CHAR);
    o.flags=REG;
    o.reg=r;
    store_reg(f,p->q2.reg,&o,CHAR);
    p->q2.reg=r;
    storedx=r;
  }

  if(p->code!=ADDRESS){
    preload_obj(f,p,&p->q1);
    if((p->q1.flags&(DREFOBJ|VAR|REG))==(DREFOBJ|VAR|REG)&&
       (p->q2.flags&(DREFOBJ|VAR|REG))==(DREFOBJ|VAR)&&
       p->q1.v==p->q2.v){
      p->q2.flags|=REG;
      p->q2.reg=p->q1.reg;
    }
    if((p->q1.flags&(DREFOBJ|VAR|REG))==(DREFOBJ|VAR|REG)&&
       (p->z.flags&(DREFOBJ|VAR|REG))==(DREFOBJ|VAR)&&
       p->q1.v==p->z.v){
      p->z.flags|=REG;
      p->z.reg=p->q1.reg;
    }
  }

  preload_obj(f,p,&p->q2);
  if((p->q2.flags&(DREFOBJ|VAR|REG))==(DREFOBJ|VAR|REG)&&
     (p->z.flags&(DREFOBJ|VAR|REG))==(DREFOBJ|VAR)&&
     p->q2.v==p->z.v){
    p->z.flags|=REG;
    p->z.reg=p->q2.reg;
  }


  if((p->z.flags&DREFOBJ)&&ISFPOINTER(p->z.dtyp)) zbuf=1;
  if((p->z.flags&(VAR|DREFOBJ))==VAR){
    bz=bank(p->z.v);
    if(bz>=0&&bz!=cbank) zbuf=1;
  }
  
  if(zbuf&&!NOBANKVARS){
    zstore=p->z;
    p->z.flags=VAR;
    p->z.v=&bankv;
    p->z.val.vmax=l2zm((long)bankvoffset);
    zstoreflag=1;
    /*bankvoffset+=zm2l(sizetab[p->typf&NQ]);*/
  }else{
    preload_obj(f,p,&p->z);

    if(isreg(z)){
      r=0;
      if(p->q1.am&&p->q1.am->base==p->z.reg){
	r=get_reg(f,p,POINTER);
	cmp_get_acc(f,p,0);
	if(!reg_pair(p->q1.am->base,&rp)) ierror(0);
	if(!reg_pair(r,&rp2)) ierror(0);
	emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
	emit(f,"\tsta\t%s\n",mregnames[rp2.r1]);
	emit(f,"\tlda\t%s\n",mregnames[rp.r2]);
	emit(f,"\tsta\t%s\n",mregnames[rp2.r2]);
	p->q1.am->base=p->q1.reg=r;
      }else if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q1.reg==p->z.reg){
	r=get_reg(f,p,POINTER);
	cmp_get_acc(f,p,0);
	if(!reg_pair(p->z.reg,&rp)) ierror(0);
	if(!reg_pair(r,&rp2)) ierror(0);
	emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
	emit(f,"\tsta\t%s\n",mregnames[rp2.r1]);
	emit(f,"\tlda\t%s\n",mregnames[rp.r2]);
	emit(f,"\tsta\t%s\n",mregnames[rp2.r2]);
	p->q1.reg=r;
      }
      if(p->q2.am&&p->q2.am->base==p->z.reg){
	if(r==0){
	  r=get_reg(f,p,POINTER);
	  cmp_get_acc(f,p,0);
	  if(!reg_pair(p->q2.am->base,&rp)) ierror(0);
	  if(!reg_pair(r,&rp2)) ierror(0);
	  emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
	  emit(f,"\tsta\t%s\n",mregnames[rp2.r1]);
	  emit(f,"\tlda\t%s\n",mregnames[rp.r2]);
	  emit(f,"\tsta\t%s\n",mregnames[rp2.r2]);
	}
	p->q2.am->base=p->q2.reg=r;
      }else if(!p->q2.am&&(p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q2.reg==p->z.reg){
	if(r==0){
	  r=get_reg(f,p,POINTER);
	  cmp_get_acc(f,p,0);
	  if(!reg_pair(p->z.reg,&rp)) ierror(0);
	  if(!reg_pair(r,&rp2)) ierror(0);
	  emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
	  emit(f,"\tsta\t%s\n",mregnames[rp2.r1]);
	  emit(f,"\tlda\t%s\n",mregnames[rp.r2]);
	  emit(f,"\tsta\t%s\n",mregnames[rp2.r2]);
	}
	p->q2.reg=r;
      }
    }
    if(isacc(z)){
      if(isacc(q2)){
	if(p->q2.reg==rax){
	  r=get_reg(f,p,INT);
	  if(!reg_pair(r,&rp)) ierror(0);
	  emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
	  emit(f,"\tstx\t%s\n",mregnames[rp.r2]);
	  storedacc=rp.r1;
	  storedx=rp.r2;
	}else{
	  r=get_reg(f,p,CHAR);
	  emit(f,"\tsta\t%s\n",mregnames[r]);
	  storedacc=r;
	}
	p->q2.reg=r;
	if(isacc(q1))
	  p->q1.reg=r;
      }
    }
  }     
 
  reload_acc(f);

  regs[ra]=mra;
  regs[rax]=mrax;
}


/* compare if two objects are the same */
static int compare_objects(struct obj *o1,struct obj *o2)
{
  if((o1->flags&(REG|DREFOBJ))==REG&&(o2->flags&(REG|DREFOBJ))==REG&&o1->reg==o2->reg)
    return 1;
  if(o1->flags==o2->flags){
    if(o1->am){
      if(o2->am){
	if(o1->am->flags!=o2->am->flags||o1->am->base!=o2->am->base||
	   o1->am->idx!=o2->am->idx||o1->am->offset!=o2->am->offset)
	  return 0;
	else
	  return 1;
      }else
	return 0;
    }else if(o2->am)
      return 0;
    if(!(o1->flags&VAR)||(o1->v==o2->v&&zmeqto(o1->val.vmax,o2->val.vmax))){
      if(!(o1->flags&REG)||o1->reg==o2->reg){
        return 1;
      }
    }
  }
  return 0;
}

/* save the result (in zreg) into p->z */
void save_result(FILE *f,struct IC *p)
{
  ierror(0);
}

/* prints an object */
static void emit_obj(FILE *f,struct obj *p,int t)
{
  if(p->am){
    if(p->am->flags==IMM_IND)
      emit(f,"(%s),y ;am(%ld)",mregnames[p->am->base],p->am->offset);
    else if(p->am->flags==GPR_IND)
      emit(f,"(%s),y ;am(%s)",mregnames[p->am->base],mregnames[p->am->idx]);
    else if(p->am->flags==ABS_IND){
      emit(f,"%ld",p->am->offset);
      if(p->am->v){
	Var *v=p->am->v;
	if(v->storage_class==EXTERN)
	  emit(f,"+%s%s",idprefix,v->identifier);
	else
	  emit(f,"+%s%ld",labprefix,zm2l(v->offset));
      }
      if(ISIDX(p->am->idx))
	emit(f,",%s ;am(%s)",mregnames[p->am->idx],mregnames[p->am->idx]);
      else
	emit(f,",y ;am(%s)",mregnames[p->am->idx]);
    }else
      ierror(0);
    return;
  }
  if((p->flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
    emitval(f,&p->val,p->dtyp&NU);
    return;
  }
  if(p->flags&DREFOBJ) emit(f,"(");
  if(p->flags&REG){
    emit(f,"%s",mregnames[p->reg]);
  }else if(p->flags&VAR) {
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER)
      emit(f,"(%s),y",mregnames[fp]);
    else{
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,MAXINT);emit(f,"+");}
      if(p->v->storage_class==STATIC){
        emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
        emit(f,"%s%s",idprefix,p->v->identifier);
      }
    }
  }
  if(p->flags&KONST){
    if(/*ieee&&((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)*/ISFLOAT(t))
      emit(f,"%s%d",labprefix,addfpconst(p,t));
    else
      emitval(f,&p->val,t&NU);
  }
  if(p->flags&DREFOBJ) emit(f,")%s",noy?"":",y");
}

/*  Test if there is a sequence of FREEREGs containing FREEREG reg.
    Used by peephole. */
static int exists_freereg(struct IC *p,int reg)
{
  while(p&&(p->code==FREEREG||p->code==ALLOCREG)){
    if(p->code==FREEREG&&p->q1.reg==reg) return 1;
    p=p->next;
  }
  return 0;
}

/* search for possible addressing-modes */
static void peephole(struct IC *p)
{
  int c,c2,r;struct IC *p2;struct AddressingMode *am;

  for(;p;p=p->next){
    c=p->code;
    if(c!=FREEREG&&c!=ALLOCREG&&(c!=SETRETURN||!isreg(q1)||p->q1.reg!=p->z.reg)) exit_label=0;
    if(c==LABEL) exit_label=p->typf;

    /* Try const(reg) */
    if((c==ADDI2P||c==SUBIFP)&&ISPREG(z)&&(p->q2.flags&(KONST|DREFOBJ))==KONST&&!ISFPOINTER(p->typf2)){
      int base;zmax of;struct obj *o;
      eval_const(&p->q2.val,p->typf);
      if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
      if(zmleq(Z0,of)&&zmleq(of,l2zm(255L))){
	r=p->z.reg;
	if(isreg(q1)&&isptr(p->q1.reg)) base=p->q1.reg; else base=r;
	o=0;
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
	  if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
	  if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
	    int t,mc;
	    if((c2==ASSIGN|c2==PUSH)&&(p2->typf&NQ)==CHAR&&!zmeqto(p2->q2.val.vmax,Z1))
	      mc=1;
	    else
	      mc=0;
	    if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
	      if(o||mc) break;
	      t=q1typ(p2)&NQ;
	      if(t>POINTER||ISFLOAT(t)) break;
	      if(m65&&ISLONG(t)) break;
	      o=&p2->q1;
	    }
	    if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
	      if(o||mc) break;
	      t=q2typ(p2)&NQ;
	      if(t>POINTER||ISFLOAT(t)) break;
	      if(m65&&ISLONG(t)) break;
	      o=&p2->q2;
	    }
	    if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
	      if(o||mc) break;
	      t=ztyp(p2)&NQ;
	      if(t>POINTER||ISFLOAT(t)) break;
	      if(m65&&ISLONG(t)) break;
	      o=&p2->z;
	    }
	  }
	  if(c2==FREEREG||(p2->z.flags&(REG|DREFOBJ))==REG){
	    int m;
	    if(c2==FREEREG)
	      m=p2->q1.reg;
	    else{
	      m=p2->z.reg;
	      if(o==&p->q1||o==&p->q2) break;
	    }
	    if(m==r){
	      if(o){
		o->am=am=mymalloc(sizeof(*am));
		am->flags=IMM_IND;
		am->base=base;
		am->offset=(int)zm2l(of);
		if(ISPREG(q1)){
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
        }
      }
    }
    /* Try reg,reg */
    if(c==ADDI2P&&(p->typf&NU)==(UNSIGNED|CHAR)&&!ISFPOINTER(p->typf2)&&isreg(q2)&&p->q2.reg!=ra&&ISPREG(z)&&(ISPREG(q1)||p->q2.reg!=p->z.reg)){
      int base,idx,ind;struct obj *o;IC *dub=0;
      r=p->z.reg;idx=p->q2.reg;
      if(ISPREG(q1)) base=p->q1.reg; else base=r;
      if((p->q1.flags&VARADR)||(p->q1.flags&(KONST|DREFOBJ))==KONST)
	ind=0;
      else
	ind=1;
      o=0;
      for(p2=p->next;p2;p2=p2->next){
        c2=p2->code;
        if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
        if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
        if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
        if((p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==idx&&idx!=r) break;
	
        if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
	  if(!o&&(c2==ADD||c2==SUB||c2==AND||c2==OR||c2==XOR)&&(p2->typf&NQ)==CHAR&&!p2->q1.am&&!p2->z.am&&
	     (p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&
	     p2->q1.flags==p2->z.flags&&p2->q1.reg==r&&p2->z.reg==r){
	    o=&p2->q1;
	    dub=p2;
	    continue;
	  }
          if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            if(o||(ind&&(q1typ(p2)&NQ)!=CHAR)) break;
            o=&p2->q1;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            if(o||(ind&&(q2typ(p2)&NQ)!=CHAR)) break;
            o=&p2->q2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            if(o||(ind&&(ztyp(p2)&NQ)!=CHAR)) break;
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
	      am->idx=idx;
	      if(ind){
		am->flags=GPR_IND;
		am->base=base;
		if(ISPREG(q1)){
		  p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
		}else{
		  p->code=c=ASSIGN;p->q2.flags=0;
		  p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
		}
	      }else{
		am->flags=ABS_IND;
		am->base=0;
		eval_const(&p->q1.val,MAXINT);
		am->offset=zm2l(vmax);
		if(p->q1.flags&VARADR)
		  am->v=p->q1.v;
		else
		  am->v=0;
		p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
	      }
	      if(dub){
		dub->z=*o;
		dub->z.am=mymalloc(sizeof(*am));
		*dub->z.am=*o->am;
	      }
            }
            break;
          }
          if(c2!=FREEREG&&m==base) break;
          continue;
        }
      }
    }
  }
}

static void pr(FILE *f,struct IC *p)
{
  int r;

  if(zstoreflag){
    int off;
    if(p->z.flags!=VAR||p->z.v!=&bankv) ierror(0);
    off=(int)zm2l(p->z.val.vmax);
    p->z=zstore;
    get_acc(f,p,INT);
    if(zstore.flags&DREFOBJ){
      if(!ISFPOINTER(zstore.dtyp)) ierror(0);
      zstore.flags&=~DREFOBJ;
      load_reg(f,LAST_PAIR,&zstore,POINTER);
      if(indirect(&zstore)){
	do_byte3(f,"lda",&zstore,FPOINTER);
	emit(f,"\ttay\n");
      }else
	do_byte3(f,"ldy",&zstore,FPOINTER);
      yval=NOVAL;
    }else{
      load_address(f,LAST_PAIR,&zstore,p->typf);
      r=bank(zstore.v);
      sety(f,r>=0?r:bankcnum);
    }
    emit(f,"\tldx\t#%d\n",off);
    emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
    emit(f,"\tjsr\t%s__bankstore%d\n",idprefix,(int)zm2l(sizetab[p->typf&NQ]));
    yval=NOVAL;
    zstoreflag=0;
  }

  for(r=1;r<=MAXR;r++){
    if(regs[r]&8)
      regs[r]&=~8;
  }
  for(r=FIRST_GPR;r<=LAST_GPR;r++){
    int ta=0;
    if(regs[r]&2){
      if(regs[ra]&&!pushedacc){
	emit(f,"\ttay\n");
	yval=NOVAL;
	ta=1;
      }
      emit(f,"\tpla\n");
      emit(f,"\tsta\t%s\n",mregnames[r]);
      regs[r]&=~2;
    }
    if(ta)
      emit(f,"\ttya\n");
  }
  if(pushedx){
    emit(f,"\tldx\t%s\n",mregnames[pushedx]);
    pushedx=0;
  }

  reload_acc_opt(f,p->next);

  storedacc=0;
  storedx=0;
}

struct cmplist {struct cmplist *next;int from,to,mode;} *first_cmplist;

static void add_cmplist(int from, int to, int mode)
{
  struct cmplist *new;
  new=mymalloc(sizeof(*new));
  new->next=first_cmplist;
  new->from=from;
  new->to=to;
  new->mode=mode;
  first_cmplist=new;
}

static void incsp(FILE *f,long of)
{
  if(of==0) return;
  if(of==1||of==-1){
    static obj o;
    o.flags=REG;
    o.reg=sp;
    if(of==1)
      incmem(f,&o,INT,ADD,1);
    else
      incmem(f,&o,INT,SUB,1);
  }else if(of==256){
    emit(f,"\tinc\t%s+1\n",mregnames[sp]);
  }else if(of==-256){
    emit(f,"\tdec\t%s+1\n",mregnames[sp]);
  }else{
    long abs;
    if(of>0){
      abs=of;
      emit(f,"\tclc\n");
    }else{
      abs=-of;
      emit(f,"\tsec\n");
    }
    if(abs&255){
      emit(f,"\tlda\t%s\n",mregnames[sp]);
      if(of>0){
	emit(f,"\tadc\t#%ld\n",abs&255);
      }else{
	emit(f,"\tsbc\t#%ld\n",abs&255);
      }
      emit(f,"\tsta\t%s\n",mregnames[sp]);
    }
    if((abs&0xff00)==0){
      if(of>0){
	emit(f,"\tbcc\t%s%d\n",labprefix,++label);
	emit(f,"\tinc\t%s+1\n",mregnames[sp]);
	emit(f,"%s%d:\n",labprefix,label);
      }else{
	emit(f,"\tbcs\t%s%d\n",labprefix,++label);
	emit(f,"\tdec\t%s+1\n",mregnames[sp]);
	emit(f,"%s%d:\n",labprefix,label);
      }	
    }else{
      emit(f,"\tlda\t%s+1\n",mregnames[sp]);
      if(of>0)
	emit(f,"\tadc\t#%ld\n",(abs>>8)&255);
      else
	emit(f,"\tsbc\t#%ld\n",(abs>>8)&255);
      emit(f,"\tsta\t%s+1\n",mregnames[sp]);
    }
  }
}

/* generates the function entry code */
static void function_top(FILE *f,struct Var *v,long offset)
{
  int i,r,of;
  rsavesize=0;
  libsave=1;
  hasretval=0;

  if(!optsize||(v->tattr&NOCOMPRESS)) emit(f,";vcprmin=10000\n");
  if(vlas) emit(f,"___fo\tset\t%ld\n",(long)argsize);
  r=0;
  for(i=FIRST_GPR;i<=LAST_GPR;i++){
    if((regused[i]&&(in_isr||!regscratch[i]))||(in_isr&&function_calls&&regscratch[i]&&!regsa[i])){
      if(i!=LAST_GPR-VOL_GPRS+1&&r==0) libsave=0;
      rsavesize++;
      r=1;
    }else{
      if(i==LAST_GPR-VOL_GPRS+1) libsave=0;
      r=0;
    }
  }
  rscnt=rsavesize;
  if(rscnt&&!SOFTSTACK){
    if(optspeed||rscnt<2)
      rsavesize=0;
    else if(!optsize&&rscnt<=5)
      rsavesize=0;
  }

  if(!special_section(f,v)){emit(f,codename);ebank(f,bank(v));if(f) section=CODE;} 
  if(v->storage_class==EXTERN){
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\tglobal\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier);
  }else
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));

  offset=localsize+argsize+rsavesize;

  if(in_isr){
    emit(f,"\tpha\n");
    if(c02){
      emit(f,"\tphx\n");
      emit(f,"\tphy\n");
    }else{
      emit(f,"\ttxa\n");
      emit(f,"\tpha\n");
      emit(f,"\ttya\n");
      emit(f,"\tpha\n");
    }
    if(offset||function_calls){
      emit(f,"\tlda\t%s\n",mregnames[sp]);
      emit(f,"\tpha\n");
      emit(f,"\tlda\t%s+1\n",mregnames[sp]);
      emit(f,"\tpha\n");
      emit(f,"\tlda\t#<(___isrstack-%ld)\n",offset);
      emit(f,"\tsta\t%s\n",mregnames[sp]);
      emit(f,"\tlda\t#>(___isrstack-%ld)\n",offset);
      emit(f,"\tsta\t%s+1\n",mregnames[sp]);
    }
  }

  if(MAINARGS&&v->storage_class==EXTERN&&!strcmp(v->identifier,"main")&&v->vtyp->exact->count>1)
    emit(f,"\tjsr\tinitmainargs\n");

  yval=NOVAL;
  of=argsize+localsize;

  if(rsavesize>0&&of+rsavesize>255){
    offset-=rsavesize;
    incsp(f,-rsavesize);
    of=0;
  }else{
    incsp(f,-offset);
    offset=0;
  }


  if(!libsave||rscnt!=rsavesize||optspeed||rscnt<=1||(rscnt<=3&&(!optsize)))
    libsave=0;
  
  if(libsave){
    sety(f,of+rsavesize-1);
    emit(f,"\tjsr\t%s__rsave%ld\n",idprefix,rscnt);
    yval=of;
  }else{
    int last=0;
    for(i=FIRST_GPR;i<=LAST_GPR;i++){
      if((regused[i]&&(in_isr||!regscratch[i]))||(in_isr&&function_calls&&regscratch[i]&&!regsa[i])){
	if(rscnt!=rsavesize){
	  if(ce02&&last==i-1){
	    emit(f,"\tphw\t%s\n",mregnames[last]);
	    last=0;
	  }else{
	    if(!ce02){
	      emit(f,"\tlda\t%s\n",mregnames[i]);
	      emit(f,"\tpha\n");
	    }else
	      last=i;
	  }
	}else{
	  sety(f,of++);
	  emit(f,"\tlda\t%s\n",mregnames[i]);
	  emit(f,"\tsta\t(%s),y\n",mregnames[sp]);
	}
      }
    }
    if(last){
      emit(f,"\tlda\t%s\n",mregnames[last]);
      emit(f,"\tpha\n");
    }
  }



  incsp(f,-offset);

  if(vlas){
    emit(f,"\tlda\t%s\n",mregnames[sp]);
    emit(f,"\tsta\t%s\n",mregnames[fp]);
    emit(f,"\tlda\t%s\n",mregnames[sp2]);
    emit(f,"\tsta\t%s\n",mregnames[fp2]);
  }
}

/* generates the function exit code */
static void function_bottom(FILE *f,struct Var *v,long offset)
{
  int i,of,ar;
  struct cmplist *p;
  offset=localsize+argsize+rsavesize;
  of=argsize+localsize;

  i=freturn(v->vtyp->next);
  if(hasretval&&(i==ra||i==rax)) ar=1; else ar=0;

  if(rscnt!=rsavesize){
    if(ar&&!pushedacc){
      pushedacc=t4;
      emit(f,"\tsta\t%s\n",mregnames[pushedacc]);
    }
    for(i=LAST_GPR;i>=FIRST_GPR;i--){
      if((regused[i]&&(in_isr||!regscratch[i]))||(in_isr&&function_calls&&regscratch[i]&&!regsa[i])){
	emit(f,"\tpla\n");
	emit(f,"\tsta\t%s\n",mregnames[i]);
      }
    }
  }
  if(rsavesize>0&&of+rsavesize>255){
    if(of!=1&&ar&&!pushedacc){
      pushedacc=t4;
      emit(f,"\tsta\t%s\n",mregnames[pushedacc]);
    }
    incsp(f,of);
    offset-=of;
    of=0;
  }
  if(rsavesize>0){
    if(ar&&!pushedacc){
      pushedacc=t4;
      emit(f,"\tsta\t%s\n",mregnames[pushedacc]);
    }
  }
  if(rsavesize){
    if(libsave){
      sety(f,of+rsavesize-1);
      emit(f,"\tjsr\t%s__rload%ld\n",idprefix,rscnt);
      yval=of;
    }else{
      for(i=FIRST_GPR;i<=LAST_GPR;i++){
	if((regused[i]&&(in_isr||!regscratch[i]))||(in_isr&&function_calls&&regscratch[i]&&!regsa[i])){
	  sety(f,of++);
	  emit(f,"\tlda\t(%s),y\n",mregnames[sp]);
	  emit(f,"\tsta\t%s\n",mregnames[i]);
	}
      }
    }
  }
  if(in_isr){
    if(offset||function_calls){
      emit(f,"\tpla\n");
      emit(f,"\tsta\t%s+1\n",mregnames[sp]);
      emit(f,"\tpla\n");
      emit(f,"\tsta\t%s\n",mregnames[sp]);
    }
  }else if(offset==2&&!pushedacc){
    emit(f,"\tinc\t%s\n",mregnames[sp]);
    emit(f,"\tbeq\t%s%d\n",labprefix,++label);
    emit(f,"\tinc\t%s\n",mregnames[sp]);
    emit(f,"\tbeq\t%s%d\n",labprefix,++label);
    reload_acc(f);
    emit(f,ret);
    emit(f,"%s%d:\n",labprefix,label-1);
    emit(f,"\tinc\t%s\n",mregnames[sp]);
    emit(f,"%s%d:\n",labprefix,label);
    emit(f,"\tinc\t%s+1\n",mregnames[sp]);
  }else{
    if(offset!=0&&ar&&!pushedacc){
      pushedacc=t4;
      emit(f,"\tsta\t%s\n",mregnames[pushedacc]);
    }
    incsp(f,offset);
  }

  reload_acc(f);

  if(in_isr){
    if(c02){
      emit(f,"\tply\n");
      emit(f,"\tplx\n");
    }else{
      emit(f,"\tpla\n");
      emit(f,"\ttay\n");
      emit(f,"\tpla\n");
      emit(f,"\ttax\n");
    }
    emit(f,"\tpla\n");
  }
  emit(f,ret);

  for(p=first_cmplist;p;){
    struct cmplist *m;
    emit(f,"%s%d:\n",labprefix,p->from);
    if(p->mode==JMPIND){
      /* indirect call */
      emit(f,"\tjmp\t(%s)\n",mregnames[p->to]);
    }else{
      pushedacc=p->mode;
      reload_acc(f);
      emit(f,"\t%s\t%s%d\n",jmpinst,labprefix,p->to);
    }
    m=p;
    p=p->next;
    free(m);
  }
  first_cmplist=0;
  pushedacc=0;
}

/****************************************/
/*  End of private data and functions.  */
/****************************************/

/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
int init_cg(void)
{
  int i;
  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign=l2zm(1L);
  char_bit=l2zm(8L);
  stackalign=l2zm(1);

  if(IEEE){
    ieee=1;
    msizetab[DOUBLE]=msizetab[LDOUBLE]=l2zm(8L);
  }


  mregnames[0]=regnames[0]="noreg";
  for(i=FIRST_GPR;i<=LAST_GPR;i++){
    regnames[i]=mymalloc(10);
    sprintf(regnames[i],"r%d",i-FIRST_GPR);
    mregnames[i]=regnames[i];
    regsize[i]=l2zm(1L);
    regtype[i]=&ctyp;
  }
  for(i=FIRST_PAIR;i<=LAST_PAIR;i++){
    int sr=(i-FIRST_PAIR)*2+FIRST_GPR;
    regnames[i]=mymalloc(10);
    sprintf(regnames[i],"%s/%s",mregnames[sr],mregnames[sr+1]);
    mregnames[i]=regnames[sr];
    regsize[i]=l2zm(2L);
    regtype[i]=&ityp;
  }
  for(i=FIRST_BIG;i<=LAST_BIG;i++){
    regnames[i]=mymalloc(10);
    sprintf(regnames[i],"btmp%d",i-FIRST_BIG);
    mregnames[i]=regnames[i];
    regsize[i]=msizetab[FLOAT];
    regtype[i]=&ftyp;
    regsa[i]=0;
    regscratch[i]=1;
  }
  for(i=FIRST_BIGP;i<=LAST_BIGP;i++){
    int sr=(i-FIRST_BIGP)*2+FIRST_BIG;
    regnames[i]=mymalloc(20);
    sprintf(regnames[i],"%s/%s",mregnames[sr],mregnames[sr+1]);
    mregnames[i]=regnames[sr];
    regsize[i]=msizetab[LLONG];
    regtype[i]=&lltyp;
    regsa[i]=0;
    regscratch[i]=1;
  }

  mregnames[ra] = regnames[ra] = "a";
  mregnames[rx] = regnames[rx] = "x";
  mregnames[ry] = regnames[ry] = "y";
  regsize[ra]=regsize[rx]=regsize[ry]=l2zm(1L);
  regtype[ra]=regtype[rx]=regtype[ry]=&ctyp;
  mregnames[sp]=regnames[sp] = "sp";
  mregnames[sp1]=regnames[sp1] = "sp";
  mregnames[sp2]=regnames[sp2] = "sp+1";

  mregnames[rax]=regnames[rax] = "a/x";
  regsize[rax]=regsize[sp]=l2zm(2L);
  regtype[rax]=regtype[sp]=&ityp;

  reg_prio[ra]=reg_prio[rax]=100;
  reg_prio[rx]=50;

  /*  Use multiple ccs.   */
  multiple_ccs=0;

  short_push=1;

  static_cse=0;

  dref_cse=1;

  /*prefer_statics=1; TODO */


  if(optsize){
    clist_copy_stack=2;
    clist_copy_pointer=2;
    clist_copy_static=2;
  }else if(optspeed){
    clist_copy_stack=64;
    clist_copy_pointer=64;
    clist_copy_static=64;
  }else{
    clist_copy_stack=8;
    clist_copy_pointer=8;
    clist_copy_static=8;
  }

  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-128L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[LONG]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[INT]=t_min(SHORT);
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(127L);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[LONG]=ul2zum(2147483647UL);
  t_max[INT]=t_max(SHORT);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT]=t_max(LLONG);
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[LONG]=ul2zum(4294967295UL);
  tu_max[INT]=t_max(UNSIGNED|SHORT);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);
  
  /*  Reserve a few registers for use by the code-generator.      */
  /*  This is not optimal but simple.                             */
  regsa[ry]=regsa[sp]=regsa[sp1]=regsa[sp2]=REGSA_NEVER;
  regsa[t1]=regsa[t2]=regsa[t3]=regsa[t4]=REGSA_NEVER;
  regscratch[ra]=regscratch[rx]=regscratch[rax]=1;
  if(!GLOBACC)
    regsa[ra]=regsa[rx]=regsa[rax]=REGSA_TEMPS;
  regsa[rx]=0;
  regscratch[sp]=regscratch[sp1]=regscratch[sp2]=regscratch[ry]=0;
  regscratch[t1]=regscratch[t2]=regscratch[t3]=regscratch[t4]=1;

  for(i=FIRST_GPR;i<=LAST_GPR-VOL_GPRS;i++){
    regscratch[i]=1;
    if(i&1)
      regscratch[FIRST_PAIR+(i-FIRST_GPR)/2]=1;
  }

  target_macros=marray;

  declare_builtin("__mulint16",INT,INT,FIRST_PAIR,INT,FIRST_PAIR+1,1,0);
  declare_builtin("__muluint16",UNSIGNED|INT,UNSIGNED|INT,FIRST_PAIR,UNSIGNED|INT,FIRST_PAIR+1,1,0);
  declare_builtin("__divuint16",UNSIGNED|INT,UNSIGNED|INT,FIRST_PAIR,UNSIGNED|INT,FIRST_PAIR+1,1,0);
  declare_builtin("__divint16",INT,INT,FIRST_PAIR,INT,FIRST_PAIR+1,1,0);
  declare_builtin("__moduint16",UNSIGNED|INT,UNSIGNED|INT,FIRST_PAIR,UNSIGNED|INT,FIRST_PAIR+1,1,0);
  declare_builtin("__modint16",INT,INT,FIRST_PAIR,INT,FIRST_PAIR+1,1,0);

  declare_builtin("__mulint32",LONG,LONG,FIRST_BIG+1,LONG,FIRST_BIG+2,1,0);
  declare_builtin("__muluint32",UNSIGNED|LONG,UNSIGNED|LONG,FIRST_BIG+1,UNSIGNED|LONG,FIRST_BIG+2,1,0);
  declare_builtin("__divint32",LONG,LONG,FIRST_BIG,LONG,FIRST_BIG+1,1,0);
  declare_builtin("__divuint32",UNSIGNED|LONG,UNSIGNED|LONG,FIRST_BIG,UNSIGNED|LONG,FIRST_BIG+1,1,0);
  declare_builtin("__modint32",LONG,LONG,FIRST_BIG,LONG,FIRST_BIG+1,1,0);
  declare_builtin("__moduint32",UNSIGNED|LONG,UNSIGNED|LONG,FIRST_BIG,UNSIGNED|LONG,FIRST_BIG+1,1,0);

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
  declare_builtin("__cmpsint64",INT,LLONG,0,LLONG,0,1,0);
  declare_builtin("__cmpuint64",INT,UNSIGNED|LLONG,0,UNSIGNED|LLONG,0,1,0);

  declare_builtin("__sint32toflt32",FLOAT,LONG,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__uint32toflt32",FLOAT,UNSIGNED|LONG,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__flt32tosint32",LONG,FLOAT,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__flt32touint32",UNSIGNED|LONG,FLOAT,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__sint16toflt32",FLOAT,INT,FIRST_PAIR,0,0,1,0);
  declare_builtin("__uint16toflt32",FLOAT,UNSIGNED|INT,FIRST_PAIR,0,0,1,0);
  declare_builtin("__flt32tosint16",INT,FLOAT,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__flt32touint16",UNSIGNED|INT,FLOAT,OLDFP?0:FIRST_BIG,0,0,1,0);

  declare_builtin("__sint32toflt64",DOUBLE,LONG,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__uint32toflt64",DOUBLE,UNSIGNED|LONG,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__flt64tosint32",LONG,DOUBLE,OLDFP?0:FIRST_BIGP,0,0,1,0);
  declare_builtin("__flt64touint32",UNSIGNED|LONG,DOUBLE,OLDFP?0:FIRST_BIGP,0,0,1,0);
  declare_builtin("__sint16toflt64",DOUBLE,INT,FIRST_PAIR,0,0,1,0);
  declare_builtin("__uint16toflt64",DOUBLE,UNSIGNED|INT,FIRST_PAIR,0,0,1,0);
  declare_builtin("__flt64tosint16",INT,DOUBLE,OLDFP?0:FIRST_BIGP,0,0,1,0);
  declare_builtin("__flt64touint16",UNSIGNED|INT,DOUBLE,OLDFP?0:FIRST_BIGP,0,0,1,0);

  declare_builtin("__flt64toflt32",FLOAT,DOUBLE,OLDFP?0:FIRST_BIGP,0,0,1,0);
  declare_builtin("__flt32toflt64",DOUBLE,FLOAT,OLDFP?0:FIRST_BIG,0,0,1,0);

  declare_builtin("__addflt32",FLOAT,FLOAT,OLDFP?0:FIRST_BIG+1,FLOAT,OLDFP?0:FIRST_BIG,1,0);
  declare_builtin("__subflt32",FLOAT,FLOAT,OLDFP?0:FIRST_BIG+1,FLOAT,OLDFP?0:FIRST_BIG,1,0);
  declare_builtin("__mulflt32",FLOAT,FLOAT,OLDFP?0:FIRST_BIG+1,FLOAT,OLDFP?0:FIRST_BIG,1,0);
  declare_builtin("__divflt32",FLOAT,FLOAT,OLDFP?0:FIRST_BIG+1,FLOAT,OLDFP?0:FIRST_BIG,1,0);
  declare_builtin("__negflt32",FLOAT,FLOAT,OLDFP?0:FIRST_BIG,0,0,1,0);
  declare_builtin("__cmpsflt32",CHAR,FLOAT,OLDFP?0:FIRST_BIG+1,FLOAT,OLDFP?0:FIRST_BIG,1,0);

#if 0
  for(i=1;i<MAXR;i++){
    printf("%02d %s scratch=%d ",i,regnames[i],regscratch[i]);
    if(reg_pair(i,&rp))
      printf("pair(%s,%s)",regnames[rp.r1],regnames[rp.r2]);
    printf("\n");
  }
#endif

  if(NOPEEP) nopeep=1;
  if(CBMASCII) cbmascii=1;
  if(ATASCII) atascii=1;
  jmpinst="jmp";
  if(C02||C02ALT){
    c02=1;zzero=1;
    jmpinst="bra";
  }
  if(CE02){
    c02=1;ce02=1;
    jmpinst="bra";
  }
  if(MEGA65){
    m65=1;c02=1;ce02=1;
    jmpinst="bra";
  }

  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }

  bankv.storage_class=EXTERN;
  bankv.identifier="__bankv";
  bankv.vtyp=new_typ();
  bankv.vtyp->flags=CHAR;

  bankcnum=COMMONBANK;

  return 1;
}

void init_db(FILE *f)
{
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
/*  A pointer MUST be returned in a register. The code-generator    */
/*  has to simulate a pseudo register if necessary.                 */
{
  int typ=t->flags&NQ;
  if(ISSTRUCT(typ)||ISUNION(typ)||typ==VOID) 
    return 0;
  if(OLDFP&&ISFLOAT(typ)) return FIRST_GPR;
  if(typ==LONG||typ==FLOAT||ISFPOINTER(typ)||(!ieee&&(typ==DOUBLE||typ==LDOUBLE)))
    return FIRST_BIG;
  if(typ==LLONG||(ieee&&(typ==DOUBLE||typ==LDOUBLE)))
    return FIRST_BIGP;
  if(zmleq(szof(t),l2zm(1L)))
    return ra;
  if(zmleq(szof(t),l2zm(2L))) 
    return rax;
  else
    return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  if(r>=FIRST_PAIR&&r<=LAST_PAIR){
    p->r1=(r-FIRST_PAIR)*2+FIRST_GPR;
    p->r2=p->r1+1;
    return 1;
  }else if(r>=FIRST_BIGP&&r<=LAST_BIGP){
    p->r1=(r-FIRST_BIGP)*2+FIRST_BIG;
    p->r2=p->r1+1;
    return 1;
  }else if(r==rax){
    p->r1=ra;
    p->r2=rx;
    return 1;
  }else if(r==sp){
    p->r1=sp1;
    p->r2=sp2;
    return 1;
  }
  return 0;
}

/* estimate the cost-saving if object o from IC p is placed in
   register r */
int cost_savings(struct IC *p,int r,struct obj *o)
{
  int c=p->code,co;
  if((o->flags&VAR)&&(o->v->tattr&ZPAGE)) return 0;
  if(indirect(o)) co=5; else co=2;

  /*TODO: adapt this */
  if(o->flags&VKONST){
    if(o!=&p->q1||p->q2.flags!=0) return 0;
    if(r==ra||r==rax) return co;
    if(r==rx&&(c==ASSIGN||c==PUSH)) return co-1;
    return 0;
  }

  if(ISIDX(r)){
    if(c==ADD||c==SUB||c==ADDI2P||c==SUBIFP||c==AND||c==OR||c==XOR){
      if(o==&p->q2&&c!=ADDI2P) return co-4;
      if(r==rx&&o==&p->q2&&((p->q1.flags&(VARADR|KONST|VKONST))==0||(p->q1.flags&DREFOBJ)))
	return co-4;
      if((c==ADD||c==SUB)&&o==&p->z&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
	eval_const(&p->q2.val,q2typ(p));
	if(zmeqto(vmax,Z1)&&zumeqto(vumax,ZU1))
	  return co+2;
      }
    }else if(c==ASSIGN||c==CONVERT){
      if(o==&p->q1&&indirect(&p->z)) return 1;
      if(o==&p->z&&indirect(&p->q1)) return 1;
    }else if(c==COMPARE){
      //if(o==&p->q1&&indirect(&p->q2)) return INT_MIN;
      //if(o==&p->q2&&indirect(&p->q1)) return INT_MIN;
    }else if(c==SETRETURN||c==GETRETURN||c==PUSH){
    }else if(c==TEST){
    }else
      return INT_MIN;
  }

  if(c==ADDI2P&&o==&p->q1&&(p->typf&NU)==CHAR&&(r==rax))
    return INT_MIN;


  if(o->flags&DREFOBJ){
    if(isptr(r))
      return co+co+10;
    if(r==rax)
      return INT_MIN;
  }
  if(c==SETRETURN&&r==p->z.reg&&!(o->flags&DREFOBJ)) return co+2;
  if(c==GETRETURN&&r==p->q1.reg&&!(o->flags&DREFOBJ)) return co+2;
  if((c==ADD||c==SUB)&&o==&p->z&&p->q1.flags==p->z.flags&&p->q1.v==p->z.v&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->q2.val,q2typ(p));
    if(zmeqto(vmax,Z1)&&zumeqto(vumax,ZU1))
      return co+1;
  }
  if(r==ra)
    return co+2;
  if(ISIDX(r))
    return co+1;
  if(r==rax)
    return co+co+4;
  return co;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if(r==0)
    return 0;
  if(r==rax&&NORAX)
    return 0;
  if(r==rx&&NOX)
    return 0;
  t&=NQ;
  if(ISCHAR(t))
    if(r==ra||(ISIDX(r)&&(optflags&2))||(r>=FIRST_GPR&&r<=LAST_GPR))
      return 1;
  if(ISSHORT(t)){
    if(r==rax){
      if(t==POINTER&&mode<0)
	return 1;
      if(t!=POINTER)
	return 1;
    }
    if(r>=FIRST_PAIR&&r<=LAST_PAIR)
      return 1;
  }
  if(r>=FIRST_BIG&&r<=LAST_BIG){
    if(t==LONG||t==FLOAT||((t==DOUBLE||t==LDOUBLE)&&!ieee))
      return 1;
    if(t==FPOINTER)
      return 1;
  }
  if(r>=FIRST_BIGP&&r<=LAST_BIGP){
    if(t==LLONG||((t==DOUBLE||t==LDOUBLE)&&ieee))
      return 1;
  }
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
/*  On the PowerPC cpu pointers and 32bit               */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
{
  int op=o&NQ,tp=t&NQ;

  if(op==tp) return 0;
  if(ISCHAR(op)&&ISCHAR(tp)) return 0;
  if(ISSHORT(op)&&ISSHORT(tp)) return 0;
  if(!ieee&&ISFLOAT(op)&&ISFLOAT(tp)) return 0;
  if(op==DOUBLE&&tp==LDOUBLE) return 0;
  if(op==LDOUBLE&&tp==DOUBLE) return 0;

  return 1;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  if(STDSYNTAX)
    emit(f,"\tspace\t%ld\n",zm2l(size));
  else
    emit(f,"\treserve\t%ld\n",zm2l(size));
  newobj=0;
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  if(zm2l(align)>1) emit(f,"\talign 1\n");
}

void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag,b=bank(v);char *sec;
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->storage_class==STATIC){
    if(ISFUNC(v->vtyp->flags)) return;
    if(v->tattr&ZPAGE)
      emit(f,"\tzpage\t%s%ld\n",labprefix,zm2l(v->offset));
    if(!special_section(f,v)){
      if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))){emit(f,dataname);ebank(f,b);if(f) section=DATA;}
      if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)){emit(f,rodataname);ebank(f,b);if(f) section=RODATA;}
      if(!v->clist){emit(f,bssname);ebank(f,b);if(f) section=BSS;}
    }
    gen_align(f,falign(v->vtyp));
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    newobj=1;
  }
  if(v->storage_class==EXTERN){
    emit(f,"\tglobal\t%s%s\n",idprefix,v->identifier);
    if(v->tattr&ZPAGE)
      emit(f,"\tzpage\t%s%s\n",idprefix,v->identifier);
    if(v->flags&(DEFINED|TENTATIVE)){
      if(!special_section(f,v)){
	if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))){emit(f,dataname);ebank(f,b);if(f) section=DATA;}
	if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)){emit(f,rodataname);ebank(f,b);if(f) section=RODATA;}
	if(!v->clist){emit(f,bssname);ebank(f,b);if(f) section=BSS;}
      }

      gen_align(f,falign(v->vtyp));
      emit(f,"%s%s:\n",idprefix,v->identifier);
      newobj=1;
    }
  }
}

void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  if(ISCHAR(t))
    emit(f,"\tbyte\t");
  else
    emit(f,"\tword\t");
  if(!p->tree){
    if(ISLONG(t)||ISFLOAT(t)){
      if(ieee&&ISFLOAT(t)){
	emit_ieee(f,&p->val,t&NQ);
      }else{
	eval_const(&p->val,t&NU);
	if(ISFLOAT(t)) cnv_fp();
	gval.vmax=zmand(vmax,l2zm(0xffffL));
	emitval(f,&gval,MAXINT);
	emit(f,",");
	gval.vmax=zmand(zmrshift(vmax,l2zm(16L)),l2zm(0xffffL));
	emitval(f,&gval,MAXINT);
      }
    }else{
      if(ISFPOINTER(t)){
	eval_const(&p->val,t&NU);
	emit(f,"%ld\n",(long)zm2l(vmax)&0xffff);
	emit(f,"\tbyte\t%d\n",(int)((zm2l(vmax)>>16)&0xff));
      }else
	emitval(f,&p->val,t&NU);
    }
  }else{
    emit_obj(f,&p->tree->o,t&NU);
    if(ISFPOINTER(t)){
      int b;
      if((p->tree->o.flags&(VAR|VARADR))!=(VAR|VARADR)) ierror(0);
      b=bank(p->tree->o.v);
      emit(f,"\n\tbyte\t%d",b>=0?b:bankcnum);
    }
  }
  emit(f,"\n");newobj=0;
}

static void do_quad(FILE *f,char *s,obj *o,int t)
{
  emit(f,"\t%s\t",s);
  noy=1;emit_obj(f,o,t);noy=0;
  emit(f,"\n");
}

static void storeq(FILE *f,obj *o,int t)
{
  emit(f,"\tq\t");
  noy=1;emit_obj(f,o,t);noy=0;
  emit(f,"\n");
}


static int handle_m65(FILE *f,IC *p)
{
  int t=p->typf,c=p->code,noop2=0;
  if(c==MULT&&ISSHORT(t)){
    get_acc(f,p,INT);
    load_acc(f,&p->q1,t);
    emit(f,"\tldz\t#$70\n");
    emit(f,"\tstz\t___m65mathptr\n");
    emit(f,"\tstq\t[___m65mathptr]\n");
    load_acc(f,&p->q2,t);
    emit(f,"\tldz\t#$74\n");
    emit(f,"\tstz\t___m65mathptr\n");
    emit(f,"\tstq\t[___m65mathptr]\n");    
    emit(f,"\tldz\t#$78\n");
    emit(f,"\tstz\t___m65mathptr\n");
    emit(f,"\tldz\t#0\n");
    emit(f,"\tldq\t[___m65mathptr],z\n");
    emit(f,"\tldz\t#0\n");
    yval=NOVAL;
    store_acc(f,&p->z,t);
    return 1;
  }
  if(ISLONG(t)&&LONGM65(c)){
    get_acc(f,p,INT);

    if(c==LSHIFT||c==RSHIFT){
      int r,cnt=-1,mzzero;
      
      if((p->q2.flags&(KONST|DREFOBJ))==KONST){
	eval_const(&p->q2.val,p->typf2);
	cnt=(int)(zm2l(vmax));
	cnt&=31;
      }

      if(cnt==24||cnt==16||cnt==8||(cnt>=0&&(!optsize||cnt<=2))){
	r=0;
      }else{
	if(isreg(q2)&&p->q2.reg>=FIRST_GPR&&scratch(p,r,0)){
	  r=p->q2.reg;
	  if(reg_pair(r,&rp)) r=rp.r1;
	}else{
	  /* TODO: improve for x or z */
	  load_lobyte(f,&p->q2,p->typf2);
	  r=t1;
	  emit(f,"\tsta\t%s\n",mregnames[r]);
	}
      }

      mzzero=zzero;
      zzero=0;
      if(cnt==24||(!optsize&&cnt>=24)){
	cnt-=24;
	if(c==LSHIFT){
	  if(indirect(&p->q1)){
	    load_lobyte(f,&p->q1,t);
	    emit(f,"\ttaz\n");
	  }else
	    do_lobyte(f,"ldz",&p->q1,t);
	  emit(f,"\tlda\t#0\n");
	  emit(f,"\ttax\n");
	  emit(f,"\ttay\n");
	  yval=0;
	}else if(t&UNSIGNED){
	  do_byte4(f,"lda",&p->q1,t);
	  emit(f,"\tldx\t#0\n");
	  emit(f,"\tldz\t#0\n");
	  emit(f,"\tldy\t#0\n");
	  yval=0;
	}else{
	  emit(f,"\tldx\t#0\n");
	  emit(f,"\tldz\t#0\n");
	  emit(f,"\tldy\t#0\n");
	  yval=0;
	  do_byte4(f,"lda",&p->q1,t);
	  emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	  emit(f,"\tdex\n");
	  if(yval==0) 
	    emit(f,"\tdey\n");
	  emit(f,"\tdez\n");
	  emit(f,"%s%d:\n",labprefix,label);
	  if(yval!=0){
	    emit(f,"\tphx\n");
	    emit(f,"\tply\n");
	  }
	}
      }else if(cnt==16||(!optsize&&cnt>=16)){
	cnt-=16;
	if(c==LSHIFT){
	  if(indirect(&p->q1)){
	    load_hibyte(f,&p->q1,t);
	    emit(f,"\ttaz\n");
	    load_lobyte(f,&p->q1,t);
	    emit(f,"\ttay\n");
	  }else{
	    do_lobyte(f,"ldy",&p->q1,t);
	    do_hibyte(f,"ldz",&p->q1,t);
	  }
	  emit(f,"\tlda\t#0\n");
	  emit(f,"\ttax\n");
	}else if(t&UNSIGNED){
	  if(indirect(&p->q1)){
	    do_byte4(f,"lda",&p->q1,t);
	    emit(f,"\ttax\n");
	    do_byte3(f,"lda",&p->q1,t);
	  }else{
	    do_byte3(f,"lda",&p->q1,t);
	    do_byte4(f,"ldx",&p->q1,t);
	  }
	  emit(f,"\tldy\t#0\n");
	  emit(f,"\tldz\t#0\n");
	}else{
	  emit(f,"\tldy\t#0\n");
	  emit(f,"\tldz\t#0\n");
	  yval=0;
	  if(indirect(&p->q1)){
	    do_byte4(f,"lda",&p->q1,t);
	    emit(f,"\ttax\n");
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\tcpx\t#0\n");
	  }else{
	    do_byte3(f,"lda",&p->q1,t);
	    do_byte4(f,"ldx",&p->q1,t);
	  }
	  emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	  emit(f,"\tdez\n");
	  if(yval==0)
	    emit(f,"\tdey\n");
	  emit(f,"%s%d:\n",labprefix,label);
	  if(yval!=0){
	    emit(f,"\tphz\n");
	    emit(f,"\tply\n");
	  }
	}
      }else if(cnt==8||(!optsize&&cnt>=8)){
	cnt-=8;
	if(c==LSHIFT){
	  if(indirect(&p->q1)){
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\ttaz\n");
	    load_lobyte(f,&p->q1,t);
	    emit(f,"\ttax\n");
	    load_hibyte(f,&p->q1,t);
	    emit(f,"\ttay\n");
	  }else{
	    do_lobyte(f,"ldx",&p->q1,t);
	    do_byte3(f,"ldz",&p->q1,t);
	    do_hibyte(f,"ldy",&p->q1,t);
	  }
	  emit(f,"\tlda\t#0\n");
	}else if(t&UNSIGNED){
	  emit(f,"\tldz\t#0\n");
	  if(indirect(&p->q1)){
	    do_byte4(f,"lda",&p->q1,t);
	    emit(f,"\tpha\n");
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\ttax\n");
	    do_hibyte(f,"lda",&p->q1,t);
	    emit(f,"\tply\n");
	  }else{
	    do_hibyte(f,"lda",&p->q1,t);
	    do_byte3(f,"ldx",&p->q1,t);
	    do_byte4(f,"ldy",&p->q1,t);
	  }
	}else{
	  emit(f,"\tldz\t#0\n");
	  if(indirect(&p->q1)){
	    do_byte4(f,"lda",&p->q1,t);
	    emit(f,"\tpha\n");
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\ttax\n");
	    do_hibyte(f,"lda",&p->q1,t);
	    emit(f,"\tply\n");
	  }else{
	    do_hibyte(f,"lda",&p->q1,t);
	    do_byte3(f,"ldx",&p->q1,t);
	    do_byte4(f,"ldy",&p->q1,t);
	  }
	  emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	  emit(f,"\tdez\n");
	  emit(f,"%s%d:\n",labprefix,label);
	}
      }else
	do_quad(f,"ldq",&p->q1,t);
	
      if(r){
	emit(f,"\tdec\t%s\n",mregnames[r]);
	emit(f,"\tbmi\t%s%d\n",labprefix,++label);
	emit(f,"%s%d:\n",labprefix,++label);
	emit(f,"\t%s\n",c==LSHIFT?"aslq":((t&UNSIGNED)?"lsrq":"asrq"));
	emit(f,"\tdec\t%s\n",mregnames[r]);
	emit(f,"\tbpl\t%s%d\n",labprefix,label);
	emit(f,"%s%d:\n",labprefix,label-1);
      }else{
	while(--cnt>=0)
	  emit(f,"\t%s\n",c==LSHIFT?"aslq":((t&UNSIGNED)?"lsrq":"asrq"));
      }
      yval=NOVAL;
      do_quad(f,"stq",&p->z,t);
      emit(f,"\tldz\t#0\n");
      zzero=mzzero;
      return 1;
    }

    if(c==PUSH){
      emit(f,"\tlda\t%s\n",mregnames[sp]);
      emit(f,"\tldx\t%s+1\n",mregnames[sp]);
      if(pushed){
	emit(f,"\tclc\n");
	emit(f,"\tadc\t#%d\n",pushed);
	emit(f,"\tbcc\t%s%d\n",labprefix,++label);
	emit(f,"\tinx\n");
	emit(f,"%s%d:\n",labprefix,label);
      }
      emit(f,"\tsta\t%s\n",mregnames[t1]);
      emit(f,"\tstx\t%s\n",mregnames[t2]);
      pushed+=4;
    }
    if(c==MULT||c==DIV){
      emit(f,"\tlda\t#$70\n");
      emit(f,"\tsta\t___m65mathptr\n");
    }
    yval=NOVAL;
    noy=1;
    if((p->q1.flags&(KONST|DREFOBJ))==KONST){
      unsigned long v;
      eval_const(&p->q1.val,q1typ(p));
      v=zum2ul(vumax);
      emit(f,"\tlda\t#%d\n",(int)(v&255));v>>=8;
      emit(f,"\tldx\t#%d\n",(int)(v&255));v>>=8;
      emit(f,"\tldy\t#%d\n",(int)(v&255));v>>=8;
      emit(f,"\tldz\t#%d\n",(int)(v&255));
    }else{
      emit(f,"\tldq\t");
      /*TODO: VARADR */
      if((p->q1.flags&VARADR)||(p->q1.flags&(KONST|DREFOBJ))==KONST){
	emit(f,"%s%d",labprefix,addfpconst(&p->q1,q1typ(p)));
      }else
	emit_obj(f,&p->q1,t);
      emit(f,"\n");
    }
    if(c==ADD){
      if((p->q2.flags&(KONST|DREFOBJ))==KONST){
	eval_const(&p->q2.val,t);
	if(zmeqto(vmax,Z1)){noop2=1;emit(f,"\tinq\n");}
	else if(zmeqto(vmax,l2zm(2L))){noop2=1;emit(f,"\tinq\n\tinq\n");}
	else if(zmeqto(vmax,l2zm(3L))){noop2=1;emit(f,"\tinq\n\tinq\n\tinq\n");}
	else if(zmeqto(vmax,l2zm(4L))){noop2=1;emit(f,"\tinq\n\tinq\n\tinq\n\tinq\n");}
      }
      if(!noop2){
	emit(f,"\tclc\n");
	emit(f,"\tadcq\t");
      }
    }else if(c==SUB){
      if((p->q2.flags&(KONST|DREFOBJ))==KONST){
	eval_const(&p->q2.val,t);
	if(zmeqto(vmax,Z1)){noop2=1;emit(f,"\tdeq\n");}
	else if(zmeqto(vmax,l2zm(2L))){noop2=1;emit(f,"\tdeq\n\tdeq\n");}
	else if(zmeqto(vmax,l2zm(3L))){noop2=1;emit(f,"\tdeq\n\tdeq\n\tdeq\n");}
	else if(zmeqto(vmax,l2zm(4L))){noop2=1;emit(f,"\tdeq\n\tdeq\n\tdeq\n\tdeq\n");}
      }
      if(!noop2){
	emit(f,"\tsec\n");
	emit(f,"\tsbcq\t");
      }
    }else if(c==OR){
      emit(f,"\torq\t");
    }else if(c==AND){
      emit(f,"\tandq\t");
    }else if(c==XOR){
      emit(f,"\teorq\t");
    }else if(c==MULT||c==DIV){
      emit(f,"\tstq\t[___m65mathptr]\n");
      emit(f,"\tlda\t#$74\n");
      emit(f,"\tsta\t___m65mathptr\n");
      emit(f,"\tldq\t");
    }
    if(c!=ASSIGN&&c!=PUSH&&!noop2){
      if((p->q2.flags&VARADR)||(p->q2.flags&(KONST|DREFOBJ))==KONST){
	emit(f,"%s%d",labprefix,addfpconst(&p->q2,q2typ(p)));
      }else
	emit_obj(f,&p->q2,t);
      emit(f,"\n");
    }
    if(c==MULT||c==DIV){
      emit(f,"\tstq\t[___m65mathptr]\n");
      emit(f,"\tlda\t#$%s\n",c==MULT?"78":"6c");
      emit(f,"\tsta\t___m65mathptr\n");
      emit(f,"\tldz\t#0\n");
      if(c==DIV) emit(f,"\tldq\t[___m65mathptr],z\n"); /* delay for division */
      emit(f,"\tldq\t[___m65mathptr],z\n");
    }
    emit(f,"\tstq\t");
    if(c==PUSH)
      emit(f,"(%s)",mregnames[t1]);
    else
      emit_obj(f,&p->z,t);
    emit(f,"\n");
    noy=0;
    if(zzero) 
      emit(f,"\tldz\t#0\n");
    else
      ierror(0);
    return 1;
  }
  return 0;
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
  int c,t,i;
  struct IC *mi;
  FILE *rf=f;
  static char *dbgfile;
  static int dbgline;

  if(vlas){
    fp=FPVLA_REG;
    if(!reg_pair(fp,&rp)) ierror(0);
    fp1=rp.r1;
    fp2=rp.r2;
    regused[fp]=regused[fp1]=regused[fp2]=1;
   }else{
    fp=sp;
    fp1=sp1;
    fp2=sp2;
  }
  argsize=0;
  localsize=offset;
  if(DEBUG&1) printf("gen_code()\n");

  if(ce02) zzero=1; /* TODO: check what to do */

  cbank=bank(v);

  if(!v->fi) v->fi=new_fi();
  v->fi->flags|=ALL_REGS;

  for(mi=p;mi;mi=mi->next)
    switch_IC(mi);

  for(pass=0;pass<2;pass++){

    if(DEBUG&1) printf("pass %d\n",pass);

    if(pass==0){
      f=0;
      mi=clone_ic(p);
    }else
      f=rf;

    for(c=1;c<=MAXR;c++) regs[c]=0; /*regsa[c];*/
    maxpushed=0;

    /*FIXME*/
    if(v->tattr&INTERRUPT){
      ret="\trti\n";
      in_isr=1;
    }else{
      ret="\trts\n";
      in_isr=0;
    }

    if(!nopeep) peephole(pass==0?p:mi);

    function_top(f,v,localsize);

    pushed=0;

    yval=NOVAL;

    dbgfile=0;
    dbgline=0;

    for(p=pass==0?p:mi;p;pr(f,p),p=p->next){



      if(DEBUG&1) pric2(stdout,p);

      if(debug_info){
	if(p->file&&p->line){
	  if(p->file!=dbgfile||p->line!=dbgline){
	    dbgfile=p->file;
	    dbgline=p->line;
	    emit(f,"; %d \"%s\"\n",dbgline,dbgfile);
	  }
	}
      }

      c=p->code;t=p->typf;

      if(c==NOP) {p->z.flags=0;continue;}
      if(c==ALLOCREG){
	regs[p->q1.reg]=1;
	if(reg_pair(p->q1.reg,&rp)){
	  regs[rp.r1]=1;
	  regs[rp.r2]=1;
	}
	continue;
      }
      if(c==FREEREG){
	regs[p->q1.reg]=0;
	if(reg_pair(p->q1.reg,&rp)){
	  regs[rp.r1]=0;
	  regs[rp.r2]=0;
	}
	continue;
      }
      if(c==LABEL) {emit(f,"%s%d:\n",labprefix,t);yval=NOVAL;continue;}
      if(c==BRA){
	yval=NOVAL;
	if(t==exit_label&&localsize+argsize+rsavesize+rscnt==0)
	  emit(f,ret);
	else
	  emit(f,"\t%s\t%s%d\n",jmpinst,labprefix,t);
	continue;
      }
      if(c>=BEQ&&c<BRA){
	yval=NOVAL;
	continue;
      }

      if(c==MOVETOREG){
	p->code=c=ASSIGN;
	p->typf=t=regtype[p->z.reg]->flags;
	p->q2.val.vmax=sizetab[regtype[p->z.reg]->flags];
      }
      if(c==MOVEFROMREG){
	p->code=c=ASSIGN;
	p->typf=t=regtype[p->q1.reg]->flags;
	p->q2.val.vmax=sizetab[regtype[p->q1.reg]->flags];
      }
      if(c==CONVERT&&ISCHAR(t)&&ISCHAR(p->typf2)){
	p->code=c=ASSIGN;
	p->q2.val.vmax=sizetab[CHAR];
      }
      if(c==CONVERT&&msizetab[t&NQ]==3&&msizetab[p->typf2&NQ]==3){
	p->code=c=ASSIGN;
	p->q2.val.vmax=sizetab[SHORT];
      }
      if(c==CONVERT&&ISSHORT(t)&&ISSHORT(p->typf2)){
	p->code=c=ASSIGN;
	p->q2.val.vmax=sizetab[SHORT];
      }
      if(c==CONVERT&&ISLONG(t)&&ISLONG(p->typf2)){
	p->code=c=ASSIGN;
	p->q2.val.vmax=sizetab[LONG];
      }
      if(c==CONVERT&&ISLLONG(t)&&ISLLONG(p->typf2)){
	p->code=c=ASSIGN;
	p->q2.val.vmax=sizetab[LLONG];
      }
      if(c==CONVERT&&ISFLOAT(t)&&ISFLOAT(p->typf2)){
	p->code=c=ASSIGN;
	p->q2.val.vmax=sizetab[t&NQ];
      }


      /* switch commutative operands if suitable */
      if(c==ADD||c==MULT||c==AND||c==XOR||c==OR||(c==ADDI2P&&ISSHORT(t)&&!ISFPOINTER(p->typf2))){
	if((compare_objects(&p->q2,&p->z)&&!isacc(q1))||isacc(q2)){
	  struct obj tmp;
	  tmp=p->q1;
	  p->q1=p->q2;
	  p->q2=tmp;
	}
      }

      if(c==COMPARE&&((p->q1.flags&(KONST|DREFOBJ))==KONST||ISRIDX(q2)||(isacc(q2)&&!ISRIDX(q1)))){
	obj tmp;IC *b;
	tmp=p->q1;
	p->q1=p->q2;
	p->q2=tmp;
	for(b=p->next;b;b=b->next){
	  int bc;
	  if(!b||b->code==LABEL) ierror(0);
	  bc=b->code;
	  if(bc==BGT){b->code=BLT;break;}
	  if(bc==BGE){b->code=BLE;break;}
	  if(bc==BLT){b->code=BGT;break;}
	  if(bc==BLE){b->code=BGE;break;}
	  if(bc==BNE||bc==BEQ) break;
	}
      }

      c=p->code;
      if(c==SUBPFP) c=SUB;
      /*if(c==ADDI2P) c=ADD;*/
      /*if(c==SUBIFP) c=SUB;*/


      if(c==MINUS){
	if(isacc(q1)&&isacc(z)){
	  emit(f,"\teor\t#255\n");
	  if(c02&&ISCHAR(t)){
	    emit(f,"\tina\n");
	  }else{
	    emit(f,"\tclc\n");
	    emit(f,"\tadc\t#1\n");
	  }
	  if(!ISCHAR(t)){
	    emit(f,"\tpha\n");
	    emit(f,"\ttxa\n");
	    emit(f,"\teor\t#255\n");
	    emit(f,"\tadc\t#0\n");
	    emit(f,"\ttax\n");
	    emit(f,"\tpla\n");
	  }
	  continue;
	}
	p->code=c=SUB;
	p->q2=p->q1;
	p->q1.flags=KONST;
	p->q1.am=0;
	gval.vmax=Z0;
	eval_const(&gval,MAXINT);
	insert_const(&p->q1.val,t);
      }

      preload(f,p);

      if((c==ADD||c==SUB||c==ADDI2P||c==SUBIFP)&&compare_objects(&p->q1,&p->z)&&(!indirect(&p->q1)||(isreg(q1)&&ISIDX(p->q1.reg)))&&isconst(q2)&&!isacc(z)){
	long l;
	eval_const(&p->q2.val,q2typ(p));
	l=zm2l(vmax);
	if(c==ADDI2P/*&&(t2&NQ)==POINTER*/) {c=ADD;t=UNSIGNED|INT;}
	if(c==SUBIFP/*&&(t2&NQ)==POINTER*/) {c=SUB;t=UNSIGNED|INT;}
	if(c==SUB||c==SUBIFP) l=-l;
	/*TODO: allow larger types */
	if(l<3&&l>-3&&(t&NQ)<=INT){
	  if(l<0){
	    if(!c02||!isreg(q1)) get_acc(f,p,CHAR);
	    incmem(f,&p->z,t,SUB,-l);
	  }else
	    incmem(f,&p->z,t,ADD,l);
	  continue;
	}
	if(ISLONG(t)&&((m65&&labs(l)<3)||(optspeed&&l>0&&l<4)||(l>0&&l<2))){
	  if(l<0){
	    incmem(f,&p->z,t,SUB,-l);
	  }else
	    incmem(f,&p->z,t,ADD,l);
	  continue;
	}
      }

      if(m65&&handle_m65(f,p))
	continue;

      if(c==CONVERT){
	int to=q1typ(p)&NU;
	t&=NU;
	if(ISCHAR(t)){
	  if(!isacc(q1))
	    get_acc(f,p,CHAR);
	  load_lobyte(f,&p->q1,to);
	  store_lobyte(f,&p->z,t);
	  continue;
	}
	if(ISLONG(to)){
	  if(!isacc(z))
	    get_acc(f,p,CHAR);
	  if(zm2l(sizetab[t&NQ])==3){
	    do_byte3(f,"lda",&p->q1,to);
	    do_byte3(f,"sta",&p->z,t);
	  }
	  if(zm2l(sizetab[t&NQ])>=2){
	    load_hibyte(f,&p->q1,to);
	    store_hibyte(f,&p->z,t);
	  }
	  load_lobyte(f,&p->q1,to);
	  store_lobyte(f,&p->z,t);
	  continue;
	}
	if(ISSHORT(to)){
	  if((t&UNSIGNED)||ISFPOINTER(t))
	    get_acc(f,p,CHAR);
	  else
	    get_acc(f,p,SHORT);
	  load_lobyte(f,&p->q1,to);
	  store_lobyte(f,&p->z,t);
	  load_hibyte(f,&p->q1,to);
	  store_hibyte(f,&p->z,t);
	  if(ISFPOINTER(t)){
	    int b=-1;
	    if((p->q1.flags&(VARADR|DREFOBJ))==VARADR) b=bank(p->q1.v);
	    emit(f,"\tlda\t#%d\n",b>=0?b:bankcnum);
	    do_byte3(f,"sta",&p->z,t);
	    continue;
	  }
	  if(to&UNSIGNED){
	    if(zzero&&!indirect(&p->z)){
	      do_byte3(f,"stz",&p->z,t);
	      do_byte4(f,"stz",&p->z,t);
	    }else{
	      emit(f,"\tlda\t#0\n");
	      do_byte3(f,"sta",&p->z,t);
	      do_byte4(f,"sta",&p->z,t);
	    }
	  }else{
	    emit(f,"\tldx\t#0\n");
	    emit(f,"\tcmp\t#0\n");
	    emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	    emit(f,"\tdex\n");
	    emit(f,"%s%d:\n",labprefix,label);
	    if(indirect(&p->z)){
	      emit(f,"\ttxa\n");
	      do_byte3(f,"sta",&p->z,t);
	      do_byte4(f,"sta",&p->z,t);
	    }else{
	      do_byte3(f,"stx",&p->z,t);
	      do_byte4(f,"stx",&p->z,t);
	    }
	  }
	  continue;
	}
	if(ISCHAR(to)){
	  if(to&UNSIGNED){
	    get_acc(f,p,CHAR);
	    if(ISLONG(t)){
	      load_lobyte(f,&p->q1,to);
	      store_lobyte(f,&p->z,t);
	      if(zzero&&!indirect(&p->z)){
		do_hibyte(f,"stz",&p->z,t);
		do_byte3(f,"stz",&p->z,t);
		if(ISLONG(t))
		  do_byte4(f,"stz",&p->z,t);
	      }else{
		emit(f,"\tlda\t#0\n");
		store_hibyte(f,&p->z,t);
		do_byte3(f,"sta",&p->z,t);
		if(ISLONG(t))
		  do_byte4(f,"sta",&p->z,t);
	      }
	      continue;
	    }
	    if(isacc(z))
	      emit(f,"\tldx\t#0\n");
	    else if(isacc(q1)){
	      emit(f,"\tldx\t#0\n");
	      store_acc(f,&p->z,t);
	      continue;
	    }else{
	      if(zzero&&!indirect(&p->z)){
		do_hibyte(f,"stz",&p->z,t);
	      }else{
		emit(f,"\tlda\t#0\n");
		store_hibyte(f,&p->z,t);
	      }
	    }
	    load_lobyte(f,&p->q1,to);
	    store_lobyte(f,&p->z,t);
	  }else{
	    int l=++label;
	    get_acc(f,p,SHORT);
	    emit(f,"\tldx\t#0\n");
	    if(isreg(q1)&&p->q1.reg==ra)
	      emit(f,"\tcmp\t#0\n");
	    else
	      load_lobyte(f,&p->q1,to);
	    emit(f,"\tbpl\t%s%d\n",labprefix,label);
	    emit(f,"\tdex\n");
	    emit(f,"%s%d:\n",labprefix,label);
	    store_lobyte(f,&p->z,t);
	    if(indirect(&p->z)&&(!isreg(z)||p->z.reg!=rax)){
	      emit(f,"\ttxa\n");
	      store_hibyte(f,&p->z,t);
	      if(ISLONG(t)){
		do_byte3(f,"sta",&p->z,t);
		do_byte4(f,"sta",&p->z,t);
	      }
	    }else{
	      if(!isreg(z)||p->z.reg!=rax){
		emit(f,"\tstx\t");
		emit_hibyte(f,&p->z,t);
		emit(f,"\n");
	      }
	      if(ISLONG(t)){
		do_byte3(f,"stx",&p->z,t);
		do_byte4(f,"stx",&p->z,t);
	      }
	    }
	  }
	  if(ISFPOINTER(t)){
	    emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
	    do_byte3(f,"sta",&p->z,t);
	  }
	  continue;
	}
	if(ISFPOINTER(to)){
	  get_acc(f,p,CHAR);
	  if(zm2l(sizetab[t&NQ])>=3){
	    do_byte3(f,"lda",&p->q1,to);
	    do_byte3(f,"sta",&p->z,t);
	  }
	  if(zm2l(sizetab[t&NQ])>=2){
	    load_hibyte(f,&p->q1,to);
	    store_hibyte(f,&p->z,t);
	  }
	  load_lobyte(f,&p->q1,to);
	  store_lobyte(f,&p->z,t);

	  continue;
	}
	ierror(0);
      }

      if(c==KOMPLEMENT){
	get_acc(f,p,CHAR);
	if(ISCHAR(t)){
	  load_acc(f,&p->q1,t);
	  emit(f,"\teor\t#-1\n");
	  store_acc(f,&p->z,t);
	}else{
	  if(isacc(q1))
	    emit(f,"\tpha\n");
	  if(ISLONG(t)){
	    do_byte4(f,"lda",&p->q1,t);
	    emit(f,"\teor\t#-1\n");
	    do_byte4(f,"sta",&p->z,t);
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\teor\t#-1\n");
	    do_byte3(f,"sta",&p->z,t);
	  }
	  load_hibyte(f,&p->q1,t);
	  emit(f,"\teor\t#-1\n");
	  store_hibyte(f,&p->z,t);
	  if(isacc(q1))
	    emit(f,"\tpla\n");
	  load_lobyte(f,&p->q1,t);
	  emit(f,"\teor\t#-1\n");
	  store_lobyte(f,&p->z,t);
	}
	continue;
      }
      if(c==SETRETURN){
	hasretval=1;
	t&=NQ;
	if(isreg(q1)&&p->q1.reg==p->z.reg) continue;
	if(t==LONG||t==LLONG||ISFLOAT(t)||ISFPOINTER(t)){
	  long l=zm2l(p->q2.val.vmax);
	  int zr=p->z.reg;
	  //get_acc(f,p,t);
	  if((optsize||l>4)&&!ISFPOINTER(t)){
	    int ind=indirect(&p->q1);
	    if(ind) load_address(f,LAST_PAIR,&p->q1,t);
	    BSET(regused,t3);
	    BSET(regused,t4);
	    sety(f,l-1);
	    emit(f,"%s%d:\n",labprefix,++label);
	    if(ind)
	      emit(f,"\tlda\t(%s),y\n",mregnames[LAST_PAIR]);
	    else{
	      emit(f,"\tlda\t");
	      if(!ISFLOAT(t)&&(p->q1.flags&(KONST|DREFOBJ))==KONST){
		emit(f,"%s%d",labprefix,addfpconst(&p->q1,t));
	      }else
		emit_obj(f,&p->q1,t);
	      emit(f,",y\n");
	    }
	    emit(f,"\tsta\t%s,y\n",mregnames[zr]);
	    emit(f,"\tdey\n");
	    emit(f,"\tbpl\t%s%d\n",labprefix,label);
	    yval=255;
	  }else{
	    if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
	      if(p->q1.reg==zr){
		int r=get_reg(f,p,POINTER);
		if(r==FIRST_PAIR||r==FIRST_PAIR+1)
		  ierror(0);
		if(!reg_pair(r,&rp2)) ierror(0);
		emit(f,"\tlda\t%s\n",mregnames[zr]);
		emit(f,"\tsta\t%s\n",mregnames[rp2.r1]);
		emit(f,"\tlda\t%s+1\n",mregnames[zr]);
		emit(f,"\tsta\t%s\n",mregnames[rp2.r2]);
		p->q1.reg=r;
	      }
	    }
	    load_lobyte(f,&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[zr]);
	    load_hibyte(f,&p->q1,t);
	    emit(f,"\tsta\t%s+1\n",mregnames[zr]);
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\tsta\t%s+2\n",mregnames[zr]);
	    if(!ISFPOINTER(t)){
	      do_byte4(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s+3\n",mregnames[zr]);
	    }
	    /*TODO:regused OLDFP */
	    regused[zr]=1;
	  }
	  continue;
	}
	//get_acc(f,p,t);
	load_acc(f,&p->q1,t);
	regused[ra]=1;
	regused[rx]=1;
	continue;
      }
      if(c==GETRETURN){
	t&=NQ;
	if(isreg(z)&&p->q1.reg==p->z.reg) continue;
	if(t==LONG||t==LLONG||ISFLOAT(t)||ISFPOINTER(t)){
	  long l=zm2l(p->q2.val.vmax);
	  int qr=p->q1.reg;
	  if((optsize||l>4)&&!ISFPOINTER(t)){
	    int ind=indirect(&p->z);
	    if(ind) load_address(f,LAST_PAIR,&p->z,t);
	    BSET(regused,t3);
	    BSET(regused,t4);
	    sety(f,l-1);
	    emit(f,"%s%d:\n",labprefix,++label);
	    emit(f,"\tlda\t%s,y\n",mregnames[qr]);
	    if(ind)
	      emit(f,"\tsta\t(%s),y\n",mregnames[LAST_PAIR]);
	    else{
	      emit(f,"\tsta\t");
	      emit_obj(f,&p->z,t);
	      emit(f,",y\n");
	    }
	    emit(f,"\tdey\n");
	    emit(f,"\tbpl\t%s%d\n",labprefix,label);
	    yval=255;
	  }else{
	    if((p->z.reg&(REG|DREFOBJ))==(REG|DREFOBJ)){
	      if(p->z.reg==qr) ierror(0);
	    }
	    emit(f,"\tlda\t%s\n",mregnames[qr]);
	    store_lobyte(f,&p->z,t);
	    emit(f,"\tlda\t%s+1\n",mregnames[qr]);
	    store_hibyte(f,&p->z,t);
	    emit(f,"\tlda\t%s+2\n",mregnames[qr]);
	    do_byte3(f,"sta",&p->z,t);
	    if(!ISFPOINTER(t)){
	      emit(f,"\tlda\t%s+3\n",mregnames[qr]);
	      do_byte4(f,"sta",&p->z,t);
	    }
	  }
	  continue;
	}
	if(p->q1.reg)
	  store_acc(f,&p->z,t);
	continue;
      }
      if(c==CALL){
	int reg;

	if(argsize<zm2l(p->q2.val.vmax)) argsize=zm2l(p->q2.val.vmax);

	/*FIXME*/
#if 0      
	if(stack_valid&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_STACK)){
	  if(framesize+zum2ul(p->q1.v->fi->stack1)>stack)
	    stack=framesize+zum2ul(p->q1.v->fi->stack1);
	}else
	  stack_valid=0;
#endif
	if((p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp("__va_start",p->q1.v->identifier)){
	  long of=va_offset(v)+localsize+rsavesize+argsize;
	  emit(f,"\tlda\t%s\n",mregnames[fp]);
	  if(of){
	    emit(f,"\tclc\n");
	    if(of&255)
	      emit(f,"\tadc\t#%d\n",(of&255));
	  }
	  emit(f,"\tldx\t%s+1\n",mregnames[fp]);
	  if(of&0xff00){
	    emit(f,"\tpha\n");
	    emit(f,"\ttxa\n");
	    emit(f,"\tadc\t#%d\n",(of>>8)&255);
	    emit(f,"\ttax\n");
	    emit(f,"\tpla\n");
	  }else if(of){
	    emit(f,"\tbcc\t%s%d\n",labprefix,++label);
	    emit(f,"\tinx\n");
	    emit(f,"%s%d:\n",labprefix,label);
	  }
	  continue;
	}
	if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
	  emit(f,";startinline\n");
	  emit_inline_asm(f,p->q1.v->fi->inline_asm);
	  emit(f,";endinline\n");
	}else if(p->q1.flags&DREFOBJ){
	  if(ISFPOINTER(p->q1.dtyp)){
	    p->q1.flags&=~DREFOBJ;
	    load_reg(f,LAST_PAIR,&p->q1,POINTER);
	    if(indirect(&p->q1)){
	      do_byte3(f,"lda",&p->q1,FPOINTER);
	      emit(f,"\ttay\n");
	    }else
	      do_byte3(f,"ldy",&p->q1,FPOINTER);
	    emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
	    emit(f,"\tjsr\t%s__bankjsr\n",idprefix);
	    yval=NOVAL;
	  }else{
	    if(!(p->q1.flags&REG)) ierror(0);
	    emit(f,"\tjsr\t%s%d\n",labprefix,++label);
	    yval=NOVAL;
	    add_cmplist(label,p->q1.reg,JMPIND);
	  }
	}else{
	  int tbank=-1;
	  if(p->q1.flags&VAR) tbank=bank(p->q1.v);
	  if(tbank!=cbank&&tbank>=0){
	    if(cbank>=0){
	      load_address(f,LAST_PAIR,&p->q1,t);
	      sety(f,tbank);
	      emit(f,"\tlda\t#%d\n",cbank);
	      emit(f,"\tjsr\t%s__bankjsr\n",idprefix);
	      yval=NOVAL;
	      continue;
	    }
	    sety(f,tbank);
	    emit(f,"\tjsr\t%s__bankswitch\n",idprefix);
	    yval=NOVAL;
	  }
	  if((p->q1.flags&VAR)&&!strcmp(p->q1.v->identifier,"_fmemcpy"))
	    emit(f,"\tlda\t#%d\n",cbank>=0?cbank:bankcnum);
	  emit(f,"\tjsr\t");
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	}
	pushed-=zm2l(p->q2.val.vmax);
	if(!calc_regs(p,f!=0)&&v->fi) v->fi->flags&=~ALL_REGS;
	yval=NOVAL;
	continue;
      }
      if(c==ASSIGN||c==PUSH){
	if(t==0) ierror(0);
	if((p->q1.flags&(KONST|DREFOBJ))==KONST&&(t&NQ)==LLONG){
	  int i;
	  eval_const(&p->q1.val,t);
	  for(i=0;i<8;i++){
	    emit(f,"\tlda\t#%d\n",zm2l(vmax)&255);
	    vmax=zmrshift(vmax,l2zm(8L));
	    if(c==PUSH||(p->z.flags&DREFOBJ)){
	      sety(f,i+((c==PUSH)?pushed:0));
	      emit(f,"\tsta\t(%s),y\n",(c==PUSH)?mregnames[sp]:mregnames[p->z.reg]);
	    }else{
	      p->z.val.vmax=zmadd(p->z.val.vmax,l2zm((long)i));
	      emit(f,"\tsta\t");
	      emit_lobyte(f,&p->z,t);
	      emit(f,"\n");
	      p->z.val.vmax=zmsub(p->z.val.vmax,l2zm((long)i));
	    }
	  }
	  if(c==PUSH) pushed+=8;
	  continue;
	}  

	if(!zmleq(p->q2.val.vmax,l2zm(4L))){
	  long len=zm2l(p->q2.val.vmax);
	  int r1,r2,loops,looplab;
	  if(len>32767) ierror(0);
	  if(!NOBANKVARS){
	    int bq=-1,bz=-1,s=-1;
	    if((p->q1.flags&(VAR|DREFOBJ))==VAR) bq=bank(p->q1.v);
	    if((p->z.flags&(VAR|DREFOBJ))==VAR) bz=bank(p->z.v);
	    if(((p->q1.flags&DREFOBJ)&&ISFPOINTER(p->q1.dtyp))||
	       ((p->z.flags&DREFOBJ)&&ISFPOINTER(p->z.dtyp))){
	      far_copy(f,p);
	      continue;
	    }
	    if(cbank<0){
	      if(bq>=0&&bz>=0){
		if(bq!=bz){
		  far_copy(f,p);
		  continue;
		}
		s=bq;
	      }else{
		if(bq>=0) s=bq;
		if(bz>=0) s=bz;
	      }
	      if(s>=0){
		sety(f,s);
		emit(f,"\tjsr\t%s__bankswitch\n",idprefix);
		yval=NOVAL;
	      }
	    }else{
	      if((bq>=0&&bq!=cbank)||(bz>=0&&bz!=cbank)){
		far_copy(f,p);
		continue;
	      }
	    }
	  }
	  get_acc(f,p,CHAR);
	  if((p->q1.flags&(DREFOBJ|KONST))==DREFOBJ){
	    if(p->q1.flags&REG){
	      r1=p->q1.reg;
	      if(!reg_pair(r1,&rp)) ierror(0);
	      if(len>128){
		emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
		emit(f,"\tclc\n");
		if(len&128)
		  emit(f,"\tadc\t#128\n");
		emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
		if(len>255){
		  emit(f,"\tlda\t%s\n",mregnames[rp.r2]);
		  emit(f,"\tadc\t#%ld\n",(len>>8)&255);
		  emit(f,"\tsta\t%s\n",mregnames[rp.r2]);	
		}else{
		  emit(f,"\tbcc\t%s%d\n",labprefix,++label);
		  emit(f,"\tinc\t%s\n",mregnames[rp.r2]);
		  emit(f,"%s%d:\n",labprefix,label);
		}
	      }
	    }else
	      ierror(0);
	  }else if(!indirect(&p->q1)&&len<=128&&(p->q1.flags&(DREFOBJ|KONST))!=KONST){
	    r1=0;
	  }else{
	    Var *v=p->q1.v;
	    /*if((p->q1.flags&(VARADR|VAR))!=VAR) ierror(0);*/
	    r1=get_reg(f,p,POINTER);
	    if(len>128) p->q1.val.vmax=zmadd(p->q1.val.vmax,l2zm(len&0xff80));
	    load_address(f,r1,&p->q1,t);
	    if(len>128) p->q1.val.vmax=zmsub(p->q1.val.vmax,l2zm(len&0xff80));
	  }
	  if((p->z.flags&(DREFOBJ|KONST))==DREFOBJ){
	    if(p->z.flags&REG){
	      r2=p->z.reg;
	      if(!reg_pair(r2,&rp)) ierror(0);
	      if(len>128){
		emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
		emit(f,"\tclc\n");
		if(len&128)
		  emit(f,"\tadc\t#128\n");
		emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
		if(len>255){
		  emit(f,"\tlda\t%s\n",mregnames[rp.r2]);
		  emit(f,"\tadc\t#%ld\n",(len>>8)&255);
		  emit(f,"\tsta\t%s\n",mregnames[rp.r2]);	
		}else{
		  emit(f,"\tbcc\t%s%d\n",labprefix,++label);
		  emit(f,"\tinc\t%s\n",mregnames[rp.r2]);
		  emit(f,"%s%d:\n",labprefix,label);
		}
	      }
	    }else
	      ierror(0);
	  }else if(c==PUSH){
	    if(len<=128&&pushed==0){
	      r2=sp;
	    }else{
	      r2=get_reg(f,p,POINTER);
	      if(!reg_pair(r2,&rp)) ierror(0);
	      emit(f,"\tlda\t%s\n",mregnames[sp]);
	      emit(f,"\tclc\n");
	      if(((pushed+(len&128))&255)!=0)
		emit(f,"\tadc\t#%ld\n",(pushed+(len&128))&255);
	      emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
	      emit(f,"\tlda\t%s\n",mregnames[sp2]);
	      emit(f,"\tadc\t#%ld\n",((pushed+len)>>8)&255);
	      emit(f,"\tsta\t%s\n",mregnames[rp.r2]);
	    }
	  }else if(!indirect(&p->z)&&len<=128){
	    r2=0;
	  }else{
	    Var *v=p->z.v;
	    r2=get_reg(f,p,POINTER);
	    if(len>128) p->z.val.vmax=zmadd(p->z.val.vmax,l2zm(len&0xff80));
	    load_address(f,r2,&p->z,t);
	    if(len>128) p->z.val.vmax=zmsub(p->z.val.vmax,l2zm(len&0xff80));
	  }
	  if(len>128){
	    get_acc(f,p,POINTER); /* get x */
	    emit(f,"\tldx\t#%ld\n",((len>>7)+1)&255);
	  }
	  sety(f,(len-1)&127);
	  if((optsize&&len>4)||len>8){
	    emit(f,"%s%d:\n",labprefix,looplab=++label);
	    if(optsize)
	      loops=1;
	    else{
	      if((len&3)==0)
		loops=4;
	      else if((len&1)==0)
		loops=2;
	      else
		loops=1;
	    }
	  }else
	    loops=len;
	  if(r1&&!reg_pair(r1,&rp)) ierror(0);
	  if(r2&&!reg_pair(r2,&rp2)) ierror(0);
	  for(i=0;i<loops;i++){
	    if(r1)
	      emit(f,"\tlda\t(%s),y\n",mregnames[rp.r1]);
	    else{
	      emit(f,"\tlda\t");
	      emit_obj(f,&p->q1,t);
	      emit(f,",y\n");
	    }
	    if(r2)
	      emit(f,"\tsta\t(%s),y\n",mregnames[rp2.r1]);
	    else{
	      emit(f,"\tsta\t");
	      emit_obj(f,&p->z,t);
	      emit(f,",y\n");
	    }
	    emit(f,"\tdey\n");
	  }
	  if(loops!=len){
	    emit(f,"\tbpl\t%s%d\n",labprefix,label);
	  }	  
	  if(len>128){
	    emit(f,"\tlda\t%s\n",mregnames[rp.r1]);
	    emit(f,"\tsec\n");
	    emit(f,"\tsbc\t#128\n");
	    emit(f,"\tsta\t%s\n",mregnames[rp.r1]);
	    emit(f,"\tbcs\t%s%d\n",labprefix,++label);
	    emit(f,"\tdec\t%s\n",mregnames[rp.r2]);
	    emit(f,"%s%d:\n",labprefix,label);
	    emit(f,"\tlda\t%s\n",mregnames[rp2.r1]);
	    emit(f,"\tsec\n");
	    emit(f,"\tsbc\t#128\n");
	    emit(f,"\tsta\t%s\n",mregnames[rp2.r1]);
	    emit(f,"\tbcs\t%s%d\n",labprefix,++label);
	    emit(f,"\tdec\t%s\n",mregnames[rp2.r2]);
	    emit(f,"%s%d:\n",labprefix,label);
	    emit(f,"\tldy\t#127\n");
	    emit(f,"\tdex\n");
	    emit(f,"\tbne\t%s%d\n",labprefix,looplab);
	  }
	  yval=NOVAL;
	  if(c==PUSH)
	    pushed+=zm2l(p->q2.val.vmax);
	  continue;
	}
	if(c==PUSH){
	  if(!scratch(p->next,ra,1))
	    get_acc(f,p,CHAR);
	  load_lobyte(f,&p->q1,t);
	  if(zzero&&pushed==0){
	    emit(f,"\tsta\t(%s)\n",mregnames[sp]);
	  }else{
	    sety(f,pushed);
	    emit(f,"\tsta\t(%s),y\n",mregnames[sp]);
	  }
	  if(!zmleq(p->q2.val.vmax,Z1)){
	    load_hibyte(f,&p->q1,t);
	    sety(f,pushed+1);
	    emit(f,"\tsta\t(%s),y\n",mregnames[sp]);
	  }
	  if(!zmleq(p->q2.val.vmax,l2zm(2L))){
	    do_byte3(f,"lda",&p->q1,t);
	    sety(f,pushed+2);
	    emit(f,"\tsta\t(%s),y\n",mregnames[sp]);
	  }
	  if(!zmleq(p->q2.val.vmax,l2zm(3L))){
	    do_byte4(f,"lda",&p->q1,t);
	    sety(f,pushed+3);
	    emit(f,"\tsta\t(%s),y\n",mregnames[sp]);
	  }

	  pushed+=zm2l(p->q2.val.vmax);
	  continue;
	}
	if(c==ASSIGN){
	  int c2m;unsigned long v;
	  if(isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg) continue;
	  if(isacc(q1)){
	    if(p->q1.reg==rax&&indirect(&p->z)) get_acc(f,p,CHAR);
	    store_acc(f,&p->z,t);
	    continue;
	  }
	  if(zzero&&!indirect(&p->z)&&(p->q1.flags&(DREFOBJ|KONST))==KONST){
	    eval_const(&p->q1.val,t);
	    if(ISFLOAT(t)){
	      cnv_fp();
	      v=zum2ul(zm2zum(vmax));
	    }else
	      v=zum2ul(vumax);
	    c2m=1;
	  }else
	    c2m=0;
	  if(!c2m||v!=0)
	    get_acc(f,p,CHAR);
	  if(0/*ISCHAR(t)*/){
	    load_acc(f,&p->q1,t);
	    store_acc(f,&p->z,t);
	  }else{
	    if(!zmleq(p->q2.val.vmax,l2zm(3L))){
	      if(c2m&&(v&0xFF000000)==0){
		do_byte4(f,"stz",&p->z,t);
	      }else{
		do_byte4(f,"lda",&p->q1,t);
		do_byte4(f,"sta",&p->z,t);
	      }
	    }
	    if(!zmleq(p->q2.val.vmax,l2zm(2L))){
	      if(c2m&&(v&0xFF0000)==0){
		do_byte3(f,"stz",&p->z,t);
	      }else{
		do_byte3(f,"lda",&p->q1,t);
		do_byte3(f,"sta",&p->z,t);
	      }
	    }
	    if(!zmleq(p->q2.val.vmax,l2zm(1L))){
	      if(isreg(z)&&p->z.reg==rax&&!indirect(&p->q1)){
		do_hibyte(f,"ldx",&p->q1,t);
	      }else if(c2m&&(v&0xFF00)==0&&!isreg(z)){
		do_hibyte(f,"stz",&p->z,t);
	      }else{
		load_hibyte(f,&p->q1,t);
		store_hibyte(f,&p->z,t);
	      }
	    }
	    if(c2m&&(v&0xFF)==0){
	      do_lobyte(f,"stz",&p->z,t);
	    }else{
	      if(isreg(q1)&&ISIDX(p->q1.reg)){
		store_reg(f,p->q1.reg,&p->z,CHAR);
	      }else if(isreg(z)&&ISIDX(p->z.reg)){
		load_reg(f,p->z.reg,&p->q1,t);
	      }else{
		load_lobyte(f,&p->q1,t);
		store_lobyte(f,&p->z,t);
	      }
	    }
	  }
	}
	continue;
      }
      if(c==ADDRESS){
	long o=real_offset(&p->q1);
	get_acc(f,p,CHAR);
	emit(f,"\tlda\t%s\n",mregnames[fp1]);
	if(o){
	  emit(f,"\tclc\n");
	  if((o&255)!=0)
	    emit(f,"\tadc\t#%ld\n",real_offset(&p->q1)&255);
	}
	store_lobyte(f,&p->z,t);
	if(isacc(z)) emit(f,"\tpha\n");
	emit(f,"\tlda\t%s\n",mregnames[fp2]);
	if(o!=0)
	  emit(f,"\tadc\t#%ld\n",real_offset(&p->q1)>>8&255);
	store_hibyte(f,&p->z,t);
	if(isacc(z)) emit(f,"\tpla\n");
	continue;
      }

      if(c==COMPARE||c==TEST){
	IC *branch=p->next;
	int pacc=0,bc,bout;
	if(ISPOINTER(t)) t|=UNSIGNED;
	while(branch){
	  if(branch->code>=BEQ&&branch->code<BRA)
	    break;
	  if(branch->code!=FREEREG&&branch->code!=ALLOCREG&&branch->code!=NOP)
	    ierror(0);
	  branch=branch->next;
	}
	bc=branch->code;
	bout=branch->typf;
	if(c==TEST){
	  p->q2.flags=KONST;
	  gval.vmax=Z0;
	  eval_const(&gval,MAXINT);
	  insert_const(&p->q2.val,t);
	}
	if((t&NU)==(UNSIGNED|CHAR)&&(bc==BLE||bc==BGT)&&(p->q2.flags&(DREFOBJ|KONST))==KONST){
	  eval_const(&p->q2.val,t);
	  if(!zmeqto(vmax,l2zm(255L))){
	    vmax=zmadd(vmax,Z1);
	    gval.vmax=vmax;
	    eval_const(&gval,t);
	    insert_const(&p->q2.val,t);
	    if(bc==BLE) bc=BLT; else bc=BGE;
	    branch->code=bc;
	  }
	}
	if(((t&NQ)==SHORT||(t&NQ)==INT)&&(bc==BNE||bc==BEQ)&&isconst(q2)&&!isacc(q1)){
	  eval_const(&p->q2.val,t);
	  if(zmeqto(vmax,Z0)&&zumeqto(vumax,ZU0)){
	    if(pacc=cmp_get_acc(f,p,branch))
	      bout=++label;
	    load_lobyte(f,&p->q1,t);
	    do_hibyte(f,"ora",&p->q1,t);
	    emit(f,"\t%s\t%s%d\n",(bc==BNE)?"bne":"beq",labprefix,bout);
	    if(pacc){
	      reload_acc(f);
	      add_cmplist(bout,branch->typf,pacc);
	    }
	    continue;
	  }
	}
	if(ieee&&ISFLOAT(t)){
	  if(!ieee) ierror(0);
	  t&=NQ;
	  if(regs[LAST_PAIR]) ierror(0);
	  regs[LAST_PAIR]=1;
	  if(regs[ra]||regs[rax])
	    ierror(0);
	  load_address(f,LAST_PAIR,&p->q2,t);
	  BSET(regused,t3);
	  BSET(regused,t4);
	  emit(f,"\tjsr\t%s__fload%c\n",idprefix,(t==FLOAT?'s':'d'));
	  yval=NOVAL;
	  load_address(f,LAST_PAIR,&p->q1,t);
	  emit(f,"\tjsr\t%s__fcmp%c\n",idprefix,(t==FLOAT?'s':'d'));
	  yval=NOVAL;
	  regs[LAST_PAIR]=0;
	  if(bc==BLT||bc==BLE)
	    emit(f,"\tbmi\t%s%d\n",labprefix,bout);
	  else if(bc==BGT|bc==BGE)
	    emit(f,"\tbvs\t%s%d\n",labprefix,bout);
	  if(bc==BEQ||bc==BLE||bc==BGE)
	    emit(f,"\tbeq\t%s%d\n",labprefix,bout);
	  if(bc==BNE)
	    emit(f,"\tbne\t%s%d\n",labprefix,bout);
	  continue;
	}
	if(ISCHAR(t)){
	  char *s=0;
	  if(isreg(q1)&&ISIDX(p->q1.reg)&&!indirect(&p->q2)&&(bc==BEQ||bc==BNE||(t&UNSIGNED))){
	    static char buf[4]="cpr";
	    s=buf;s[2]=mregnames[p->q1.reg][0];
	  }else{
	    if(pacc=cmp_get_acc(f,p,branch))
	      bout=++label;
	    load_acc(f,&p->q1,t);
	    if(bc==BEQ||bc==BNE||(t&UNSIGNED)){
	      s="cmp";
	    }else{
	      if(bc==BLT||bc==BGE)
		emit(f,"\tsec\n");
	      else
		emit(f,"\tclc\n");
	      s="sbc";
	    }
	  }
	  if(c==TEST)
	    emit(f,"\t%s\t#0\n",s);
	  else
	    do_lobyte(f,s,&p->q2,t);
	  if(bc==BEQ)
	    emit(f,"\tbeq\t%s%d\n",labprefix,bout);
	  else if(bc==BNE)
	    emit(f,"\tbne\t%s%d\n",labprefix,bout);
	  else if(t&UNSIGNED){
	    if(bc==BLT)
	      emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	    else if(bc==BGE)
	      emit(f,"\tbcs\t%s%d\n",labprefix,bout);
	    else if(bc==BLE){
	      emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	      emit(f,"\tbeq\t%s%d\n",labprefix,bout);
	    }else if(bc==BGT){
	      emit(f,";\n\tbeq\t%s%d\n",labprefix,++label);
	      emit(f,"\tbcs\t%s%d\n",labprefix,bout);
	      emit(f,"%s%d:\n",labprefix,label);
	    }else
	      ierror(0);
	  }else{
	    emit(f,"\tbvc\t%s%d\n",labprefix,++label);
	    emit(f,"\teor\t#128\n");
	    emit(f,"%s%d:\n",labprefix,label);
	    if(bc==BLT||bc==BLE)
	      emit(f,"\tbmi\t%s%d\n",labprefix,bout);
	    else
	      emit(f,"\tbpl\t%s%d\n",labprefix,bout);
	  }
	  if(pacc){
	    reload_acc(f);
	    add_cmplist(bout,branch->typf,pacc);
	  }
	  continue;
	}else if(bc==BEQ||bc==BNE||(t&UNSIGNED)||ISFPOINTER(t)){
	  int in=0;
	  if(pacc=cmp_get_acc(f,p,branch))
	    bout=++label;
	  if(ISLONG(t)){
	    do_byte4(f,"lda",&p->q1,t);
	    if(c==TEST)
	      emit(f,"\tcmp\t#0\n");
	    else
	      do_byte4(f,"cmp",&p->q2,t);
	    if(bc==BEQ)
	      emit(f,"\tbne\t%s%d\n",labprefix,in=++label);
	    else if(bc==BNE)
	      emit(f,"\tbne\t%s%d\n",labprefix,bout);
	    else if(bc==BLT||bc==BLE){
	      emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	      emit(f,"\tbne\t%s%d\n",labprefix,in=++label);
	    }else if(bc==BGE||bc==BGT){
	      emit(f,"\tbcc\t%s%d\n",labprefix,in=++label);	
	      emit(f,"\tbne\t%s%d\n",labprefix,bout);
	    }else
	      ierror(0);
	  }
	  if(ISLONG(t)||ISFPOINTER(t)){
	    do_byte3(f,"lda",&p->q1,t);
	    if(c==TEST)
	      emit(f,"\tcmp\t#0\n");
	    else
	      do_byte3(f,"cmp",&p->q2,t);
	    if(bc==BEQ)
	      emit(f,"\tbne\t%s%d\n",labprefix,in?in:(in=++label));
	    else if(bc==BNE)
	      emit(f,"\tbne\t%s%d\n",labprefix,bout);
	    else if(bc==BLT||bc==BLE){
	      emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	      emit(f,"\tbne\t%s%d\n",labprefix,in?in:(in=++label));
	    }else if(bc==BGE||bc==BGT){
	      emit(f,"\tbcc\t%s%d\n",labprefix,in?in:(in=++label));	
	      emit(f,"\tbne\t%s%d\n",labprefix,bout);
	    }else
	      ierror(0);	  
	  }
	  if(isacc(q1)){
	    if(!indirect(&p->q2)){
	      do_hibyte(f,"cpx",&p->q2,t);
	    }else{
	      int r=get_reg(f,p,CHAR);
	      emit(f,"\tpha\n");
	      load_hibyte(f,&p->q2,t);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      emit(f,"\tpla\n");
	      emit(f,"\tcpx\t%s\n",mregnames[r]);
	    }
	  }else{
	    load_hibyte(f,&p->q1,t);
	    if(c==TEST)
	      emit(f,"\tcmp\t#0\n");
	    else
	      do_hibyte(f,"cmp",&p->q2,t);
	  }
	  if(bc==BEQ)
	    emit(f,"\tbne\t%s%d\n",labprefix,in?in:(in=++label));
	  else if(bc==BNE)
	    emit(f,"\tbne\t%s%d\n",labprefix,bout);
	  else if(bc==BLT||bc==BLE){
	    emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	    emit(f,"\tbne\t%s%d\n",labprefix,in?in:(in=++label));
	  }else if(bc==BGE||bc==BGT){
	    emit(f,"\tbcc\t%s%d\n",labprefix,in?in:(in=++label));
	    emit(f,"\tbne\t%s%d\n",labprefix,bout);
	  }else
	    ierror(0);
	  load_lobyte(f,&p->q1,t);
	  if(c==TEST)
	    emit(f,"\tcmp\t#0\n");
	  else
	    do_lobyte(f,"cmp",&p->q2,t);
	  if(bc==BEQ)
	    emit(f,"\tbeq\t%s%d\n",labprefix,bout);
	  else if(bc==BNE)
	    emit(f,"\tbne\t%s%d\n",labprefix,bout);
	  else if(bc==BLT)
	    emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	  else if(bc==BGE)
	    emit(f,"\tbcs\t%s%d\n",labprefix,bout);	
	  else if(bc==BLE){
	    emit(f,"\tbcc\t%s%d\n",labprefix,bout);
	    emit(f,"\tbeq\t%s%d\n",labprefix,bout);
	  }else if(bc==BGT){
	    emit(f,"\tbcc\t%s%d\n",labprefix,in);
	    emit(f,"\tbne\t%s%d\n",labprefix,bout);
	  }else
	    ierror(0);
	  if(in)
	    emit(f,"%s%d:\n",labprefix,in);
	  if(pacc){
	    reload_acc(f);
	    add_cmplist(bout,branch->typf,pacc);
	  }
	  continue;
	}else{
	  if(bc==BGT||bc==BLE){
	    obj o;
	    if(isacc(q1)){
	      int r;
	      if(pacc=cmp_get_acc(f,p,branch))
		bout=++label;
	      r=get_reg(f,p,CHAR);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      load_lobyte(f,&p->q2,t);
	      emit(f,"\tcmp\t%s\n",mregnames[r]);
	      emit(f,"\tstx\t%s\n",mregnames[r]);
	      load_hibyte(f,&p->q2,t);
	      emit(f,"\tsbc\t%s\n",mregnames[r]);
	      emit(f,"\tbvc\t%s%d\n",labprefix,++label);
	      emit(f,"\teor\t#128\n");
	      emit(f,"%s%d:\n",labprefix,label);
	      if(bc==BGT)
		emit(f,"\tbmi\t%s%d\n",labprefix,bout);
	      else
		emit(f,"\tbpl\t%s%d\n",labprefix,bout);
	      if(pacc){
		reload_acc(f);
		add_cmplist(bout,branch->typf,pacc);
	      }
	      continue;
	    }else{
	      o=p->q1;p->q1=p->q2;p->q2=o;
	      if(bc==BGT) bc=BLT; else bc=BGE;
	    }
	  }
	  if(pacc=cmp_get_acc(f,p,branch))
	    bout=++label;
	  if(ISLONG(t)){
	    load_lobyte(f,&p->q1,t);
	    do_lobyte(f,"cmp",&p->q2,t);
	    load_hibyte(f,&p->q1,t);
	    do_hibyte(f,"sbc",&p->q2,t);
	    do_byte3(f,"lda",&p->q1,t);
	    do_byte3(f,"sbc",&p->q2,t);
	    do_byte4(f,"lda",&p->q1,t);
	    do_byte4(f,"sbc",&p->q2,t);
	  }else{
	    load_lobyte(f,&p->q1,t);
	    do_lobyte(f,"cmp",&p->q2,t);
	    load_hibyte(f,&p->q1,t);
	    do_hibyte(f,"sbc",&p->q2,t);
	  }
	  emit(f,"\tbvc\t%s%d\n",labprefix,++label);
	  emit(f,"\teor\t#128\n");
	  emit(f,"%s%d:\n",labprefix,label);
	  if(bc==BLT)
	    emit(f,"\tbmi\t%s%d\n",labprefix,bout);
	  else if(bc==BGE)
	    emit(f,"\tbpl\t%s%d\n",labprefix,bout);
	  else
	    ierror(0);
	  if(pacc){
	    reload_acc(f);
	    add_cmplist(bout,branch->typf,pacc);
	  }
	  continue; 
	}
	ierror(0);
      }


      if((c==ADD||c==SUB)&&ISRIDX(z)&&isconst(q2)){
	eval_const(&p->q2.val,t);
	if(zmeqto(vmax,l2zm(-1L))){ 
	  vmax=Z1; vumax=ZU1; 
	  if(c==ADD) c=SUB; else c=ADD;
	}
	if(zmeqto(vmax,Z1)&&zumeqto(vumax,ZU1)){
	  if(indirect(&p->q1)){
	    get_acc(f,p,CHAR);
	    load_reg(f,p->z.reg,&p->q1,t);
	  }else
	    do_lobyte(f,"ldx",&p->q1,t);
	  emit(f,"\t%s%s\n",(c==ADD)?"in":"de",mregnames[p->z.reg]);
	  continue;
	}
      }

      if((c==LSHIFT||c==RSHIFT)&&isreg(q1)&&isreg(z)&&isconst(q2)&&p->q1.reg==p->z.reg&&p->z.reg!=ra&&p->z.reg!=rax){
	long l;
	eval_const(&p->q2.val,q2typ(p));
	l=zm2l(vmax);
	/*TODO: allow larger types */
	if(l<5&&(t&NQ)<=INT){
	  if(c==RSHIFT&&!(t&UNSIGNED))
	    get_acc(f,p,CHAR);
	  incmem(f,&p->z,t,c,l);
	  continue;
	}
      }

      if(c==LSHIFT||c==RSHIFT){
	long l=-1;int loop=0,r,r2,r3,outl=0;
	if(isconst(q2)){
	  eval_const(&p->q2.val,q2typ(p));
	  l=zm2l(vmax);
	  loop=0;
	}else
	  loop=1;
	if(l>=0&&optsize){
	  if(c==LSHIFT&&(l&7)>6) 
	    loop=1;
	  else if(c==RSHIFT&&(t&UNSIGNED)&&(l&7)>3)
	    loop=1;
	  else if(c==RSHIFT&&!(t&UNSIGNED)&&(l&7)>2)
	    loop=1;
	}

	if(!ISCHAR(t))
	  r=get_reg(f,p,CHAR);
	if(ISLONG(t)){
	  r2=get_reg(f,p,CHAR);
	  r3=get_reg(f,p,CHAR);
	}
	if(ISLONG(t)){
	  get_acc(f,p,CHAR);
	  if(l>=24){
	    if(c==LSHIFT){
	      load_lobyte(f,&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      emit(f,"\tlda\t#0\n");
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	    }else if(c==RSHIFT&&!(t&UNSIGNED)){
	      do_byte4(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      sety(f,0);
	      emit(f,"\tcmp\t#0\n");
	      emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	      emit(f,"\tdey\n");
	      emit(f,"%s%d:\n",labprefix,label);
	      emit(f,"\tsty\t%s\n",mregnames[r]);
	      emit(f,"\tsty\t%s\n",mregnames[r2]);
	      emit(f,"\ttya\n");
	      yval=NOVAL;
	    }else{
	      emit(f,"\tlda\t#0\n");
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      do_byte4(f,"lda",&p->q1,t);

	    }
	  }else if(l>=16){
	    if(c==LSHIFT){
	      load_lobyte(f,&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      load_hibyte(f,&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      emit(f,"\tlda\t#0\n");
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	    }else if(c==RSHIFT&&!(t&UNSIGNED)){
	      do_byte3(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      do_byte4(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      sety(f,0);
	      emit(f,"\tcmp\t#0\n");
	      emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	      emit(f,"\tdey\n");
	      emit(f,"%s%d:\n",labprefix,label);
	      emit(f,"\tsty\t%s\n",mregnames[r]);
	      emit(f,"\ttya\n");
	      yval=NOVAL;
	    }else{
	      emit(f,"\tlda\t#0\n");
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      do_byte4(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      do_byte3(f,"lda",&p->q1,t);

	    }
	  }else if(l>=8){
	    if(c==LSHIFT){
	      load_lobyte(f,&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      load_hibyte(f,&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      do_byte3(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      emit(f,"\tlda\t#0\n");
	    }else if(c==RSHIFT&&!(t&UNSIGNED)){
	      load_hibyte(f,&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      do_byte3(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      do_byte4(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      sety(f,0);
	      emit(f,"\tcmp\t#0\n");
	      emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	      emit(f,"\tdey\n");
	      emit(f,"%s%d:\n",labprefix,label);
	      emit(f,"\ttya\n");
	      yval=NOVAL;
	    }else{
	      emit(f,"\tlda\t#0\n");
	      emit(f,"\tsta\t%s\n",mregnames[r3]);
	      do_byte4(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r2]);
	      do_byte3(f,"lda",&p->q1,t);
	      emit(f,"\tsta\t%s\n",mregnames[r]);
	      load_hibyte(f,&p->q1,t);

	    }
	  }else if(c==RSHIFT&&!(t&UNSIGNED)){
	    load_lobyte(f,&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[r3]);
	    load_hibyte(f,&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[r2]);
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[r]);
	    do_byte4(f,"lda",&p->q1,t);
	  }else{
	    do_byte4(f,"lda",&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[r3]);
	    do_byte3(f,"lda",&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[r2]);
	    load_hibyte(f,&p->q1,t);
	    emit(f,"\tsta\t%s\n",mregnames[r]);
	    load_lobyte(f,&p->q1,t);
	  }
	}else
	  get_acc(f,p,t);
	if(!ISLONG(t)){
	  if(l>=8){
	    if(!ISSHORT(t)) ierror(0);
	    if(c==LSHIFT){
	      if(indirect(&p->q1)){
		load_lobyte(f,&p->q1,t);
		emit(f,"\ttax\n");
		emit(f,"\tlda\t#0\n");
	      }else{
		if(isacc(q1))
		  emit(f,"\ttax\n");
		else
		  do_lobyte(f,"ldx",&p->q1,t);
		emit(f,"\tlda\t#0\n");
	      }
	    }else{
	      load_hibyte(f,&p->q1,t);
	      emit(f,"\tldx\t#0\n");
	      if(!(t&UNSIGNED)){
		emit(f,"\tcmp\t#0\n");
		emit(f,"\tbpl\t%s%d\n",labprefix,++label);
		emit(f,"\tdex\n");
		emit(f,"%s%d:\n",labprefix,label);
	      }
	    }
	  }else
	    load_acc(f,&p->q1,t);
	}
	if(ISSHORT(t))
	  emit(f,"\tstx\t%s\n",mregnames[r]);
	if(l>=0) l&=7;
	if(loop){
	  if(l>=0)
	    sety(f,l);
	  else{
	    if(indirect(&p->q2)){
	      emit(f,"\tpha\n");
	      load_lobyte(f,&p->q2,q2typ(p));
	      emit(f,"\ttay\n");
	      emit(f,"\tpla\n");
	      emit(f,"\tcpy\t#0\n");
	    }else{
	      emit(f,"\tldy\t");
	      emit_lobyte(f,&p->q2,q2typ(p));
	      emit(f,"\n");
	    }
	    outl=++label;
	    emit(f,"\tbeq\t%s%d\n",labprefix,outl);
	  }
	  emit(f,"%s%d:\n",labprefix,++label);
	}else{
	  if(ISCHAR(t))
	    l&=7;
	  else if(ISSHORT(t))
	    l&=15;
	  else
	    l&=31;
	}
	while(l>0||loop){
	  if(c==LSHIFT){
	    emit(f,"\tasl\n");
	    if(!ISCHAR(t))
	      emit(f,"\trol\t%s\n",mregnames[r]);
	    if(ISLONG(t)){
	      emit(f,"\trol\t%s\n",mregnames[r2]);
	      emit(f,"\trol\t%s\n",mregnames[r3]);
	    }
	  }else if(t&UNSIGNED){
	    emit(f,"\tclc\n");
	    if(ISLONG(t)){
	      emit(f,"\tror\t%s\n",mregnames[r3]);
	      emit(f,"\tror\t%s\n",mregnames[r2]);
	    }
	    if(!ISCHAR(t))
	      emit(f,"\tror\t%s\n",mregnames[r]);
	    emit(f,"\tror\n");
	  }else{
	    if(ISLONG(t)){
	      if(ce02)
		emit(f,"\tasr\n");
	      else{
		emit(f,"\tcmp\t#128\n");
		emit(f,"\tror\n");
	      }
	      emit(f,"\tror\t%s\n",mregnames[r]);
	      emit(f,"\tror\t%s\n",mregnames[r2]);
	      emit(f,"\tror\t%s\n",mregnames[r3]);
	    }else{
	      if(!ISCHAR(t)){
		emit(f,"\tcpx\t#128\n");
		emit(f,"\tror\t%s\n",mregnames[r]);
	      }else
		emit(f,"\tcmp\t#128\n");
	      emit(f,"\tror\n");
	    }
	  }
	  if(loop){
	    emit(f,"\tdey\n");
	    emit(f,"\tbne\t%s%d\n",labprefix,label);
	    if(outl) emit(f,"%s%d:\n",labprefix,outl);
	    yval=0;
	    break;
	  }
	  l--;
	}
	if(ISLONG(t)){
	  if(c==RSHIFT&&!(t&UNSIGNED)){
	    do_byte4(f,"sta",&p->z,t);
	    emit(f,"\tlda\t%s\n",mregnames[r]);
	    do_byte3(f,"sta",&p->z,t);
	    emit(f,"\tlda\t%s\n",mregnames[r2]);
	    store_hibyte(f,&p->z,t);
	    emit(f,"\tlda\t%s\n",mregnames[r3]);
	    store_lobyte(f,&p->z,t);
	  }else{
	    store_lobyte(f,&p->z,t);
	    emit(f,"\tlda\t%s\n",mregnames[r]);
	    store_hibyte(f,&p->z,t);
	    emit(f,"\tlda\t%s\n",mregnames[r2]);
	    do_byte3(f,"sta",&p->z,t);
	    emit(f,"\tlda\t%s\n",mregnames[r3]);
	    do_byte4(f,"sta",&p->z,t);
	  }
	}else{
	  if(!ISCHAR(t))
	    emit(f,"\tldx\t%s\n",mregnames[r]);
	  if(ISCHAR(t)||indirect(&p->z))
	    store_acc(f,&p->z,t);
	  else{
	    store_lobyte(f,&p->z,t);
	    if(!isreg(z)||p->z.reg!=rax){
	      emit(f,"\tstx\t");
	      emit_hibyte(f,&p->z,t);
	      emit(f,"\n");
	    }
	  }
	}
	continue;
      }

      if((c>=OR&&c<=AND)||(c>=LSHIFT&&c<=MOD)||c==ADDI2P||c==SUBIFP){
	char *s;int t2=t,pt=p->typf2;
	if(!isacc(z)){
	  /* TODO: check other operations */
	  if((c==ADD||c==SUB||c==ADDI2P||c==SUBIFP)&&isacc(q1)&&scratch(p->next,ra,1))
	    ;
	  else{
	    if((c==ADDI2P||c==SUBIFP)&&(p->typf2&NU)==CHAR)
	      get_acc(f,p,INT);
	    else
	      get_acc(f,p,CHAR);
	  }
	}
	if(c==ADDI2P||c==SUBIFP){
	  if(c==ADDI2P) c=ADD; else c=SUB;
	  t=UNSIGNED|INT;
	  if((pt&NQ)==POINTER&&(p->q1.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p->q1.val,POINTER);
	    insert_const(&p->q1.val,UNSIGNED|INT);
	  }
	}
	if(c>=OR&&c<=AND)
	  s=logicals[c-OR];
	else
	  s=arithmetics[c-LSHIFT];
      
	if(ISFLOAT(t)){
	  if(!ieee) ierror(0);
	  t&=NQ;
	  if(regs[LAST_PAIR]) ierror(0);
	  get_acc(f,p,INT);
	  regs[LAST_PAIR]=1;
	  load_address(f,LAST_PAIR,&p->q1,t);
	  BSET(regused,t3);
	  BSET(regused,t4);
	  emit(f,"\tjsr\t%s__fload%c\n",idprefix,(t==FLOAT?'s':'d'));
	  yval=NOVAL;
	  load_address(f,LAST_PAIR,&p->q2,t);
	  emit(f,"\tjsr\t%s__f%s%c\n",idprefix,ename[c],(t==FLOAT?'s':'d'));
	  yval=NOVAL;
	  load_address(f,LAST_PAIR,&p->z,t);
	  emit(f,"\tjsr\t%s__fstore%c\n",idprefix,(t==FLOAT?'s':'d'));
	  yval=NOVAL;
	  regs[LAST_PAIR]=0;
	  continue;
	}else if(ISLONG(t)){
	  if(c==ADD) emit(f,"\tclc\n");
	  if(c==SUB) emit(f,"\tsec\n");
	  load_lobyte(f,&p->q1,t);
	  do_lobyte(f,s,&p->q2,t);
	  store_lobyte(f,&p->z,t);
	  load_hibyte(f,&p->q1,t);
	  do_hibyte(f,s,&p->q2,t);
	  store_hibyte(f,&p->z,t);
	  do_byte3(f,"lda",&p->q1,t);
	  do_byte3(f,s,&p->q2,t);
	  do_byte3(f,"sta",&p->z,t);
	  do_byte4(f,"lda",&p->q1,t);
	  do_byte4(f,s,&p->q2,t);
	  do_byte4(f,"sta",&p->z,t);
	  continue;
	}else if(ISCHAR(t)){
	  load_acc(f,&p->q1,t);
	  if(c02&&(c==ADD||c==SUB)&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p->q2.val,t);
	    if(zmeqto(vmax,Z1)||zmeqto(vmax,l2zm(2L))){
	      if(zmeqto(vmax,l2zm(2L)))
		emit(f,"\t%s\n",c==ADD?"ina":"dea");
	      emit(f,"\t%s\n",c==ADD?"ina":"dea");
	      store_acc(f,&p->z,t);
	      continue;
	    }
	  }
	  if(c==ADD) emit(f,"\tclc\n");
	  if(c==SUB) emit(f,"\tsec\n");
	  if(isreg(q2)&&ISIDX(p->q2.reg)){
	    int r=get_reg(f,p,CHAR);
	    emit(f,"\tst%s\t%s\n",mregnames[p->q2.reg],mregnames[r]);
	    p->q2.flags=REG;
	    p->q2.reg=r;
	  }
	  do_lobyte(f,s,&p->q2,t);
	  store_acc(f,&p->z,t);
	}else if(c==ADD||c==SUB){
	  int a,r;long l=1;
	  if(isconst(q2)){
	    eval_const(&p->q2.val,t2);
	    l=zm2l(vmax);
	    l&=0xff00;
	  }
	  if(isreg(z)&&p->z.reg==rax) a=1; else a=0;
	  if((t2&NU)==CHAR&&(p->q2.flags&(KONST|DREFOBJ))!=KONST){
	    get_acc(f,p,INT);
	    emit(f,"\tldx\t#0\n");
	    load_lobyte(f,&p->q2,t);
	    emit(f,"\tbpl\t%s%d\n",labprefix,++label);
	    emit(f,"\tdex\n");
	    emit(f,"%s%d:\n",labprefix,label);
	    if(c==ADD){
	      if(c==ADD) emit(f,"\tclc\n"); else emit(f,"\tsec\n");
	      do_lobyte(f,s,&p->q1,t);
	      if(isacc(z))
		emit(f,"\tpha\n");
	      else
		store_lobyte(f,&p->z,t);
	      emit(f,"\ttxa\n");
	      do_hibyte(f,s,&p->q1,t);
	      store_hibyte(f,&p->z,t);
	      if(ISFPOINTER(pt)&&!compare_objects(&p->q1,&p->z)){
		do_byte3(f,"lda",&p->q1,pt);
		do_byte3(f,"sta",&p->z,pt);
	      }
	      if(isacc(z))
		emit(f,"\tpla\n");
	      continue;
	    }
	    r=get_reg(f,p,CHAR);
	    emit(f,"\tstx\t%s\n",mregnames[r]);
	  }
	  load_lobyte(f,&p->q1,t);
	  if(c==ADD) emit(f,"\tclc\n"); else emit(f,"\tsec\n");
	  do_lobyte(f,s,&p->q2,t2);
	  store_lobyte(f,&p->z,t);
	  if(l==0&&isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg){
	    if(c==ADD){
	      emit(f,"\tbcc\t%s%d\n",labprefix,++label);
	      if(a)
		emit(f,"\tinx\n");
	      else{
		/*if(!reg_pair(p->z.reg,&rp)) ierror(0);*/
		emit(f,"\tinc\t%s+1\n",mregnames[p->z.reg]);
	      }
	    }else{
	      emit(f,"\tbcs\t%s%d\n",labprefix,++label);
	      if(a)
		emit(f,"\tdex\n");
	      else{
		if(!reg_pair(p->z.reg,&rp)) ierror(0);
		emit(f,"\tdec\t%s\n",mregnames[rp.r2]);
	      }
	    }
	    emit(f,"%s%d:\n",labprefix,label);
	  }else{
	    if(a==1) emit(f,"\tpha\n");
	    if((t2&NQ)==CHAR){
	      if(t2&UNSIGNED){
		load_hibyte(f,&p->q1,t);
		emit(f,"\t%s\t#0\n",s);
	      }else{
		load_hibyte(f,&p->q1,t);
		if((p->q2.flags&(KONST|DREFOBJ))==KONST){
		  eval_const(&p->q2.val,t2);
		  if(zmleq(Z0,vmax))
		    emit(f,"\t%s\t#0\n",s);
		  else
		    emit(f,"\t%s\t#255\n",s);
		}else{
		  emit(f,"\t%s\t%s\n",s,mregnames[r]);
		}
	      }
	    }else{
	      load_hibyte(f,&p->q1,t);
	      do_hibyte(f,s,&p->q2,t2);
	    }
	    store_hibyte(f,&p->z,t);
	    if(a==1) emit(f,"\tpla\n");
	    if(ISFPOINTER(pt)&&p->code!=SUBPFP){
	      if(!compare_objects(&p->q1,&p->z)){
		do_byte3(f,"lda",&p->q1,FPOINTER);
		do_byte3(f,"sta",&p->z,FPOINTER);
	      }
	    }
	  }
	}else{
	  if(isacc(q1))
	    emit(f,"\tpha\n");
	  load_hibyte(f,&p->q1,t);
	  do_hibyte(f,s,&p->q2,t);
	  store_hibyte(f,&p->z,t);
	  if(isacc(q1))
	    emit(f,"\tpla\n");
	  load_lobyte(f,&p->q1,t);
	  do_lobyte(f,s,&p->q2,t);
	  store_lobyte(f,&p->z,t);
	}
	continue;
      }
      pric2(stdout,p);
      ierror(0);
    }

    function_bottom(f,v,localsize);
    
    for(c=1;c<=MAXR;c++){
      if(regsa[c]||regused[c]){
	BSET(regs_modified,c);
      }
    }

    if(stack_valid){
      if(!v->fi) v->fi=new_fi();
      v->fi->flags|=ALL_STACK;
      v->fi->stack1=stack;
    }
  }

  free_IC(mi);

  emit(f,"; stacksize=%lu%s\n",zum2ul(stack),stack_valid?"":"+??");
}

int shortcut(int code,int typ)
{
  if(code==COMPARE/*||code==MULT*/||code==ADD||code==SUB||code==AND||code==OR||code==XOR||code==LSHIFT||code==RSHIFT||code==MINUS||code==KOMPLEMENT||code==NEGATION)
    return 1;

  return 0;
}

static int fattr(type *p,char *s)
{
  if(p->attr&&strstr(p->attr,s))
    return 1;
  if(p->next)
    return fattr(p->next,s);
  else
    return 0;
}

int reg_parm(struct reg_handle *m, struct Typ *t,int vararg,struct Typ *d)
{
  int f;

  if(vararg)
    return 0;
  f=t->flags&NQ;
  if(OLDFP&&ISFLOAT(f)) return 0;
  if(d&&fattr(d,"__stackparms__"))
    return 0;
  if(d&&fattr(d,"__cc65__")){
    m->regs++;
    printf("arg=%d cnt=%d\n",m->regs,d->exact->count);
    if(m->regs==d->exact->count-1){
      if(ISCHAR(t->flags))
	return ra;
      if(ISSHORT(t->flags))
	return rax;
    }
    return 0;
  }
  if(ISCHAR(f)){
    if(!t->exact){
      if(m->regs>=GPR_ARGS-1)
	return 0;
      f=FIRST_GPR+m->regs;
      m->regs+=2;
      return f;
    }else{
      if(m->regs>=GPR_ARGS)
	return 0;
      else
	return FIRST_GPR+m->regs++;
    }
  }
  if(ISSHORT(f)||f==POINTER){
    if(m->regs>=GPR_ARGS-1)
      return 0;
    else{
      if(m->regs&1) m->regs+=1;
      m->regs+=2;
      return FIRST_PAIR+m->regs/2-1;
    }
  }
  if(f==FPOINTER||f==LONG||f==FLOAT||(!ieee&&(f==DOUBLE||f==LDOUBLE))){
    if(m->bregs>=4)
      return 0;
    else
      return FIRST_BIG+m->bregs++;
  }
  if(f==LLONG||(ieee&&(f==DOUBLE||f==LDOUBLE))){
    if(m->bregs>=3)
      return 0;
    else{
      if(m->bregs&1)  m->bregs++;
      m->bregs+=2;
      return FIRST_BIGP+m->bregs/2-1;
    }
  }
  return 0;
}

int handle_pragma(const char *s)
{
  static char sec[SECLEN];
  int i;
  if(sscanf(s,"section %127s",sec)==1){
    if(!strcmp(sec,"default"))
      use_sec=0;
    else
      use_sec=sec;
    return 1;
  }
  if(sscanf(s,"bank %d",&i)==1){
    use_bank=i;
    return 1;
  }
}
void cleanup_cg(FILE *f)
{
  int i;
  struct fpconstlist *p=firstfpc;

  if(f&&p){
    emit(f,rodataname);emit(f,"\n");
    section=RODATA;
  }
  while(p=firstfpc){
    emit(f,"%s%d:\n\tword\t",labprefix,p->label);
    if(ieee)
      emit_ieee(f,&p->val,p->t);
    else{
      int words=zm2l(sizetab[p->t&NQ])/2;
      eval_const(&p->val,p->t);
      if(ISFLOAT(p->t)) cnv_fp();
      for(i=0;i<words;i++){
	emit(f,"%ld",zm2l(vmax)&0xffff);
	if(i<words-1){emit(f,",");vmax=zmrshift(vmax,l2zm(16L));}
      }
      emit(f,"\n");
      /*emit(f,"%ld,%ld\n",zm2l(vmax)&0xffff,zm2l(zmrshift(vmax,l2zm(16L)))&0xffff);*/
    }
    firstfpc=p->next;
    free(p);
  }

  emit(f,"\tzpage\t%s\n",mregnames[sp]);
  for(i=FIRST_GPR;i<=LAST_GPR;i++)
    emit(f,"\tzpage\t%s\n",mregnames[i]);
  for(i=FIRST_BIG;i<=LAST_BIG;i++)
    emit(f,"\tzpage\t%s\n",mregnames[i]);

}
void cleanup_db(FILE *f)
{
  if(f) section=-1;
}

static char *zops[]={
  "adc","and","asl","bit","eor","lda","ora",
  "tax","txa","tay","tya","sbc"};

static int setszflag(char *op)
{
  int i;
  for(i=0;i<sizeof(zops)/sizeof(zops[0]);i++)
    if(!strcmp(op,zops[i]))
      return 1;
  return 0;
}

static char *zxops[]={
  "tax","txa","ldx","inx","dex"};

static int setszxflag(char *op)
{
  int i;
  for(i=0;i<sizeof(zxops)/sizeof(zxops[0]);i++)
    if(!strcmp(op,zxops[i]))
      return 1;
  return 0;
}

enum peepf { NEEDSAME = 1, REMOVE1ST = 2};
struct peeps {char *s1,*s2,*r;enum peepf flags;};



int emit_peephole(void)
{
  int entries,i,j;
  char *asmline[EMIT_BUF_DEPTH];
  char buf1[1024],buf2[1024];
  char op1[8],op2[8];
  static char ca[1024],cx[1024],cy[1024],cz[1024];
  static int rm,disabled;

  static struct peeps elim[]={
    "lda","sta",0,NEEDSAME,
    "ldx","stx",0,NEEDSAME,
    "ldy","sty",0,NEEDSAME,
    "ldz","stz",0,NEEDSAME,
    "ldq","stq",0,NEEDSAME,
    "sta","sta",0,NEEDSAME,
    "stx","stx",0,NEEDSAME,
    "sty","sty",0,NEEDSAME,
    "stz","stz",0,NEEDSAME,
    "stq","stq",0,NEEDSAME,
    "sta","lda",0,NEEDSAME,
    "stx","ldx",0,NEEDSAME,
    "sty","ldy",0,NEEDSAME,
    "stz","ldz",0,NEEDSAME,
    "stq","ldq",0,NEEDSAME,
    "txa","tax",0,0,
    "tax","txa",0,0,
    "tay","tya",0,0,
    "tya","tay",0,0,
    "tza","taz",0,0,
    "taz","tza",0,0,
    "lda","lda",0,REMOVE1ST,
    "ldx","ldx",0,REMOVE1ST,
    "ldy","ldy",0,REMOVE1ST,
    "ldz","ldz",0,REMOVE1ST,
    "ldq","ldq",0,REMOVE1ST,
    "lda","pla",0,REMOVE1ST,
    "ldx","plx",0,REMOVE1ST,
    "ldy","ply",0,REMOVE1ST,
    "ldz","plz",0,REMOVE1ST,
    "lda","txa",0,REMOVE1ST,
    "lda","tya",0,REMOVE1ST,
    "lda","tza",0,REMOVE1ST,
    "ldx","tax",0,REMOVE1ST,
    "ldy","tay",0,REMOVE1ST,
    "ldz","taz",0,REMOVE1ST,
    "tay","ldy",0,REMOVE1ST,
    "tax","ldx",0,REMOVE1ST,
    "taz","ldz",0,REMOVE1ST,
    "txa","lda",0,REMOVE1ST,
    "tya","lda",0,REMOVE1ST,
    "tza","lda",0,REMOVE1ST,
    "lda","ldx","\ttax\n",NEEDSAME,
    "lda","ldy","\ttay\n",NEEDSAME,
    "lda","ldz","\ttaz\n",NEEDSAME,
    "ldx","lda","\ttxa\n",NEEDSAME,
    "ldy","lda","\ttya\n",NEEDSAME,
    "ldz","lda","\ttza\n",NEEDSAME,
    "sta","ldx","\ttax\n",NEEDSAME,
    "sta","ldy","\ttay\n",NEEDSAME,
    "sta","ldz","\ttaz\n",NEEDSAME,
    "stx","lda","\ttxa\n",NEEDSAME,
    "sty","lda","\ttya\n",NEEDSAME,
    "stz","lda","\ttza\n",NEEDSAME, /* must be last */
  };

  if(nopeep) return 0;

  /* activate optimization that is not valid for c02 */
  if(!ce02) elim[sizeof(elim)/sizeof(elim[0])-1].r="\tlda\t#0\n";

  i=emit_l;
  if(emit_f==0)
    entries=i-emit_f+1;
  else
    entries=EMIT_BUF_DEPTH;
  asmline[0]=emit_buffer[i];

  if(!strcmp(asmline[0],";startinline\n")) disabled=1;
  if(!strcmp(asmline[0],";endinline\n")) disabled=0;
  if(disabled) return 0;

  buf1[0]=0;op1[0]=0;
  if((j=sscanf(asmline[0]," %6s %999s",op1,buf1))>=1){
    /*printf("a=%s x=%s y=%s z=%s\n",ca,cx,cy,cz);
      printf("\t\t%s %s\n",op1,buf1);*/
    if(!strcmp(op1,"lda")){
      if(buf1[0]=='#'){
	if(!rm&&!strcmp(buf1,ca)){remove_asm();return rm=1;}
	if(!rm&&!strcmp(buf1,cx)){strcpy(asmline[0],"\ttxa\n");return rm=1;}
	if(!rm&&!strcmp(buf1,cy)){strcpy(asmline[0],"\ttya\n");return rm=1;}
	if(!rm&&!strcmp(buf1,cz)){strcpy(asmline[0],"\ttza\n");return rm=1;}
	strcpy(ca,buf1);
      }else ca[0]=0;
    }else if(!strcmp(op1,"ldx")){
      if(buf1[0]=='#'){
	if(!rm&&!strcmp(buf1,cx)){remove_asm();return rm=1;}
	if(!rm&&!strcmp(buf1,ca)){strcpy(asmline[0],"\ttax\n");return rm=1;}
	strcpy(cx,buf1);
      }else cx[0]=0;
    }else if(!strcmp(op1,"ldy")){
      if(buf1[0]=='#'){
	if(!rm&&!strcmp(buf1,cy)){remove_asm();return rm=1;}
	if(!rm&&!strcmp(buf1,ca)){strcpy(asmline[0],"\ttay\n");return rm=1;}
	strcpy(cy,buf1);
      }else cy[0]=0;
    }else if(!strcmp(op1,"ldz")){
      if(buf1[0]=='#'){
	if(!rm&&!strcmp(buf1,cz)){remove_asm();return rm=1;}
	if(!rm&&!strcmp(buf1,ca)){strcpy(asmline[0],"\ttaz\n");return rm=1;}
	strcpy(cz,buf1);
      }else cz[0]=0;
    }else{
      static char clobbernone[]="asw bit clc cld cli clv cmp cpx cpy dec deq dew inc inq inw nop pha php phw phz plp sec sed sei sta stq stx sty";
      static char clobbera[]="adc and asl asr eor lsr ora pla rol ror sbc txa tya tza";
      static char clobberx[]="dex inx tax tsx";
      static char clobbery[]="dey iny tay";
      static char clobberz[]="dez inz taz";
      if(strstr(clobbernone,op1)){
      }else if(strstr(clobbera,op1))
	ca[0]=0;
      else if(strstr(clobberx,op1))
	cx[0]=0;
      else if(strstr(clobbery,op1))
	cy[0]=0;
      else if(strstr(clobberz,op1))
	cz[0]=0;
      else
	ca[0]=cx[0]=cy[0]=cz[0]=0;
    }
  }else{
    ca[0]=cx[0]=cy[0]=cz[0]=0;
  }

  rm=0;

  if(sscanf(asmline[0]," %6s %999s",op1,buf1)==2&&!strcmp(op1,"and")&&!strcmp(buf1,"#0")){
    strcpy(asmline[0],"\tlda\t#0\n");
    return rm=1;
  }

  if(entries>=2){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[1]=emit_buffer[i];


    if(!strcmp(asmline[0],"; volatile barrier\n")&&!strcmp(asmline[0],asmline[1])){
      remove_asm();
      return rm=1;
    }

    if(sscanf(asmline[0]," %6s",op1)==1&&!strcmp(op1,"rts")&&
       sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&!strcmp(op2,"jsr")){
      sprintf(asmline[1],"\tjmp\t%s\n",buf2);
      remove_asm();
      return rm=1;
    }

    for(j=0;j<sizeof(elim)/sizeof(elim[0]);j++){
      if(elim[j].flags&NEEDSAME){
	if(sscanf(asmline[0]," %6s %999s",op2,buf2)==2&&
	   sscanf(asmline[1]," %6s %999s",op1,buf1)==2&&
	   !strcmp(op1,elim[j].s1)&&!strcmp(op2,elim[j].s2)&&
	   !strcmp(buf1,buf2)){
	  if(elim[j].r){
	    strcpy(asmline[0],elim[j].r);
	  }else{
	    if(elim[j].flags&REMOVE1ST)
	      strcpy(asmline[1],asmline[0]);
	    remove_asm();
	  }
	  return rm=1;
	}
      }else{
	if(sscanf(asmline[1]," %6s",op1)==1&&
	   sscanf(asmline[0]," %6s",op2)==1&&
	   !strcmp(op1,elim[j].s1)&&!strcmp(op2,elim[j].s2)){
	  if(elim[j].flags&REMOVE1ST)
	    strcpy(asmline[1],asmline[0]);
	  remove_asm();
	  return rm=1;
	}
      }
    }


  }

  if(entries>=3){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[2]=emit_buffer[i];
    if(sscanf(asmline[0]," %6s %999s",op1,buf1)==2){
#if 0
      if(!strcmp(op1,"lda")&&buf1[0]=='#'){
	if(sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&
	   !strcmp(op2,"sta")){
	  if(sscanf(asmline[2]," %6s %999s",op2,buf2)==2&&
	     !strcmp(op2,"lda")&&!strcmp(buf1,buf2)){
	    remove_asm();
	    return rm=1;
	  }
	}
      }
#endif
      if(!strcmp(op1,"beq")||!strcmp(op1,"bne")){
	if(sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&
	   !strcmp(op2,"cmp")&&!strcmp(buf2,"#0")){
	  if(sscanf(asmline[2]," %6s",op2)==1&&
	     setszflag(op2)){
	    strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return rm=1;
	  }
	}
      }
      if(!strcmp(op1,"beq")||!strcmp(op1,"bne")){
	if(sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&
	   !strcmp(op2,"cpx")&&!strcmp(buf2,"#0")){
	  if(sscanf(asmline[2]," %6s",op2)==1&&
	     setszxflag(op2)){
	    strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return rm=1;
	  }
	}
      }
    }
  }
  if(entries>=4){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[3]=emit_buffer[i];
  }
  if(entries>=5){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[4]=emit_buffer[i];
    if(sscanf(asmline[0]," %s %s",op1,buf1)>=1){
      if(!strcmp(op1,"lda")||!strcmp(op1,"pla")||!strcmp(op1,"txa")||!strcmp(op1,"tya")){
	if(sscanf(asmline[1]," %s %s",op1,buf1)>=1&&!strcmp(op1,"pla")&&
	   sscanf(asmline[2]," %s %s",op1,buf1)>=1&&!strcmp(op1,"tay")&&
	   sscanf(asmline[3]," %s %s",op1,buf1)>=1&&!strcmp(op1,"txa")&&
	   sscanf(asmline[4]," %s %s",op1,buf1)>=1&&!strcmp(op1,"pha")){
	  strcpy(asmline[4],asmline[3]);
	  strcpy(asmline[3],asmline[2]);
	  strcpy(asmline[2],asmline[0]);
	  remove_asm();
	  remove_asm();
	  return rm=1;
	}
      }
    }
  }
  return 0;
}

/* Return name of library function, if this node should be
   implemented via libcall. */
char *use_libcall(int c,int t,int t2)
{
  static char fname[16];
  char *ret=0;

  if(c==COMPARE){
    if((t&NQ)==LLONG||(ISFLOAT(t)&&!ieee)){
      sprintf(fname,"__cmp%s%s%ld",(t&UNSIGNED)?"u":"s",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8);
      ret=fname;
    }
  }else{
    t&=NU;
    t2&=NU;
    if(ISSHORT(t)&&c!=MULT&&c!=DIV&&c!=MOD&&!ISFLOAT(t2))
      return 0;
    if(ISLONG(t)&&c!=MULT&&c!=DIV&&c!=MOD&&!ISFLOAT(t2))
      return 0;

    if(!ieee&&ISFLOAT(t)) t=FLOAT;
    if(t==LDOUBLE) t=DOUBLE;
    if(t2==LDOUBLE) t2=DOUBLE;
    if(!ieee&&ISFLOAT(t2)) t2=FLOAT;
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
    if((t&NQ)==INT||(t&NQ)==LONG||(t&NQ)==LLONG||(!ieee&&ISFLOAT(t))){
      if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==KOMPLEMENT||c==MINUS){
	if(m65&&ISLONG(t)&&(c==MULT||(c==DIV&&(t&UNSIGNED)))) return 0;
	if(m65&&c==MULT&&ISSHORT(t)) return 0;
        if(t==(UNSIGNED|LLONG)&&(c==MULT||c==DIV||c==MOD||c==RSHIFT)){
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
        }else if(t==(UNSIGNED|INT)&&(c==DIV||c==MOD||c==RSHIFT)){
          sprintf(fname,"__%suint16",ename[c]);
          ret=fname;
        }else if((t&NQ)==INT){
          sprintf(fname,"__%sint16",ename[c]);
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


int pointer_varadr(Var *v)
{
  int b=bank(v);
  if(b>=0&&b!=bankcnum&&!NOBANKVARS){
    if(cur_funcv&&bank(cur_funcv)!=b)
      return FPOINTER;
  }
  return pointer_type(v->vtyp);
}

int pointer_type(struct Typ *p)
{
  struct Typ *merk=p;
  if(!p) ierror(0);
  if(LARGE) return FPOINTER;
  while(ISARRAY(p->flags)||ISFUNC(p->flags)) p=p->next;
  if(p->attr){
    char *s;
    if(strstr(p->attr,STR_HUGE)) return HPOINTER;
    if(strstr(p->attr,STR_FAR)) return FPOINTER;
    if(strstr(p->attr,STR_NEAR)) return POINTER;
  }
  return POINTER;
}

unsigned char cbmconv(unsigned char x)
{
  static unsigned char ctab[256]={
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x14,0x09,0x0D,0x11,0x93,0x0A,0x0E,0x0F,
    0x10,0x0B,0x12,0x13,0x08,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0x5B,0xBF,0x5D,0x5E,0xA4,
    0xAD,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xB3,0xDD,0xAB,0xB1,0xDF,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x0C,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0x5F,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0x7D,0xAC,0x60,0xAE,0xAF,
    0xB0,0x7E,0xB2,0x7B,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0x5C,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0xDC,0x7C,0xDE,0x7F,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
  };

  static unsigned char atab[]={0xfd,8,0x7f,0x9b,11,0x7d};

  if(cbmascii)
    return ctab[x&255];
  else if(atascii&&x>=7&&x<=12)
    return atab[x-7];
  else
    return x;
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
  if(t==(UNSIGNED|CHAR)) {p->vuchar=vuchar;return;}
  if(t==(UNSIGNED|SHORT)) {p->vushort=vushort;return;}
  if(t==(UNSIGNED|INT)) {p->vuint=vuint;return;}
  if(t==(UNSIGNED|LONG)) {p->vulong=vulong;return;}
  if(t==(UNSIGNED|LLONG)) {p->vullong=vullong;return;}
  if(t==(UNSIGNED|MAXINT)) {p->vumax=vumax;return;}
  if(t==FLOAT) {p->vfloat=vfloat;return;}
  if(t==DOUBLE) {p->vdouble=vdouble;return;}
  if(t==LDOUBLE) {p->vldouble=vldouble;return;}
  if(t==POINTER) {p->vuint=vuint;return;}
  if(t==FPOINTER||t==HPOINTER) {p->vulong=vulong;return;}
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
      if(f==POINTER)
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
  if(t==CHAR){fprintf(f,"C");vmax=zc2zm(p->vchar);printzm(f,vmax);}
  if(t==(UNSIGNED|CHAR)){fprintf(f,"UC");vumax=zuc2zum(p->vuchar);printzum(f,vumax);}
  if(t==SHORT){fprintf(f,"S");vmax=zs2zm(p->vshort);printzm(f,vmax);}
  if(t==(UNSIGNED|SHORT)){fprintf(f,"US");vumax=zus2zum(p->vushort);printzum(f,vumax);}
  if(t==FLOAT){fprintf(f,"F");vldouble=zf2zld(p->vfloat);printzld(f,vldouble);}
  if(t==DOUBLE){fprintf(f,"D");vldouble=zd2zld(p->vdouble);printzld(f,vldouble);}
  if(t==LDOUBLE){fprintf(f,"LD");printzld(f,p->vldouble);}
  if(t==INT){fprintf(f,"I");vmax=zi2zm(p->vint);printzm(f,vmax);}
  if(t==(UNSIGNED|INT)||t==POINTER){fprintf(f,"UI");vumax=zui2zum(p->vuint);printzum(f,vumax);}
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
  if(t==CHAR){vmax=zc2zm(p->vchar);emitzm(f,vmax);}
  if(t==(UNSIGNED|CHAR)){vumax=zuc2zum(p->vuchar);emitzum(f,vumax);}
  if(t==SHORT){vmax=zs2zm(p->vshort);emitzm(f,vmax);}
  if(t==(UNSIGNED|SHORT)){vumax=zus2zum(p->vushort);emitzum(f,vumax);}
  if(t==FLOAT){vldouble=zf2zld(p->vfloat);emitzld(f,vldouble);}
  if(t==DOUBLE){vldouble=zd2zld(p->vdouble);emitzld(f,vldouble);}
  if(t==LDOUBLE){emitzld(f,p->vldouble);}
  if(t==INT){vmax=zi2zm(p->vint);emitzm(f,vmax);}
  if(t==(UNSIGNED|INT)||t==POINTER){vumax=zui2zum(p->vuint);emitzum(f,vumax);}
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
          p->flags=((p->flags&~NU)|POINTER);
        if(strstr(attr,STR_FAR))
          p->flags=((p->flags&~NU)|FPOINTER);
        if(strstr(attr,STR_HUGE))
          p->flags=((p->flags&~NU)|HPOINTER);
      }
    }
    p=p->next;
  }
}

void add_var_hook_post(Var *v)
{
  if(use_sec&&(v->storage_class==STATIC||v->storage_class==EXTERN)){
    char buf[SECLEN+32];
    sprintf(buf,"section(\"%s\");",use_sec);
    add_attr(&v->vattr,buf);
  }
  if(use_bank>=0&&(v->storage_class==STATIC||v->storage_class==EXTERN)){
    char buf[64];
    /*sprintf(buf,"section(\"bank%d\");bank(%d)",use_bank,use_bank);*/
    sprintf(buf,"bank(%d)",use_bank);
    add_attr(&v->vattr,buf);
  }
}

int decide_reverse(zmax v)
{
  if(zmeqto(v,Z1)||zmeqto(v,l2zm(2L)))
    return 1;
  if(optspeed)
    if(zmeqto(v,l2zm(4L))||zmeqto(v,l2zm(8L))||zmeqto(v,l2zm(256L))||zmeqto(v,l2zm(512L)))
      return 1;
  
  return 0;
}

static int is_single_eff_ic(struct IC *p)
{
  struct Var *v,*idx;
  if(p->code!=ADDI2P||(p->typf2&NQ)!=POINTER)
    return 0;
  if(!(p->q2.flags&KONST)){
    if((p->typf&NU)!=(UNSIGNED|CHAR))
      return 0;
    if((p->q2.flags&(VAR|DREFOBJ))!=VAR)
      return 0;
    if(p->q2.v->storage_class!=AUTO&&p->q2.v->storage_class!=REGISTER)
      return 0;
    idx=p->q2.v;
  }else{
    idx=0;
    eval_const(&p->q2.val,p->typf);
    /* TODO: more precise check considering data type useful? */
    if(!zmleq(vumax,l2zm(255L)))
      return 0;
    return 1;
  }
  if(p->q1.flags&DREFOBJ)
    return 0;
  if((p->z.flags&(VAR|DREFOBJ))!=VAR)
    return 0;
  if(p->z.v->storage_class==STATIC||p->z.v->storage_class==EXTERN)
    return 0;
  v=p->z.v;
  for(p=p->next;p;p=p->next){
    int c=p->code;
    if(c==LABEL||(c>=BEQ&&c<=BRA))
      return 1; /* TODO: how elaborate should we test? */
    if((p->q1.flags&VAR)&&p->q1.v==v){
      if(p->q1.flags&DREFOBJ)
        return 1;
      else
        return 0;
    }
    if((p->q2.flags&VAR)&&p->q2.v==v){
      if(p->q2.flags&DREFOBJ)
        return 1;
      else
        return 0;
    }
    if((p->z.flags&VAR)&&p->z.v==v){
      if(p->z.flags&DREFOBJ)
        return 1;
      else
        return 0;
    }
    if((p->z.flags&VAR)&&p->z.v==idx)
      return 0;
  }
  return 0;
}

void mark_eff_ics(void)
{
  struct IC *p;
  for(p=first_ic;p;p=p->next){
    if(is_single_eff_ic(p))
      p->flags|=EFF_IC;
    else
      p->flags&=~EFF_IC;
  }
}

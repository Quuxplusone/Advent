/*  Code generator for Motorola 680x0 CPUs. Supports 68000-68060+68881/2    */
/*  and ColdFire.                                                           */
/*  Vasm, PhxAss and the GNU assembler is supported.                        */

#include "supp.h"

static char FILE_[]=__FILE__;

#include "dwarf2.c"

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for m68k/ColdFire V1.8 (c) in 1995-2010 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,VALFLAG,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char *g_flags_name[MAXGF]={
    "cpu","fpu","d2scratch","noa4","sc","sd","prof","const-in-data",
    "use-framepointer","no-peephole","no-delayed-popping",
    "gas","branch-opt","no-fp-return","no-mreg-return","hunkdebug",
    "no-intz","old-peephole","conservative-sr","elf","use-commons",
    "a2scratch"
};

union ppi g_flags_val[MAXGF];

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
char *regnames[MAXR+1]={"noreg","a0","a1","a2","a3","a4","a5","a6","a7",
                               "d0","d1","d2","d3","d4","d5","d6","d7",
                        "fp0","fp1","fp2","fp3","fp4","fp5","fp6","fp7",
                        "d0/d1","d2/d3","d4/d5","d6/d7"};

static char *elfregnames[MAXR+1]={"noreg","%a0","%a1","%a2","%a3","%a4","%a5","%a6","%a7",
			   "%d0","%d1","%d2","%d3","%d4","%d5","%d6","%d7",
			   "%fp0","%fp1","%fp2","%fp3","%fp4","%fp5","%fp6","%fp7",
			   "%d0/%d1","%d2/%d3","%d4/%d5","%d6/%d7"};

static char *mregnames[MAXR+1];

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1]={0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
			1,0,0,0};

int reg_prio[MAXR+1]={0,4,4,3,3,3,3,3,0,2,2,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__far","__near","__chip","__saveds",
                     "__interrupt","__amigainterrupt","__stdargs",0};
#define FAR 1
#define NEAR 2
#define CHIP 4
#define SAVEDS 8
#define INTERRUPT 16
#define AMIINTERRUPT 32
#define STDARGS 64 /* not used, just for compatibility */

int MINADDI2P=SHORT;

/****************************************/
/*  Some private data and functions.    */
/****************************************/
#define CPU             (g_flags_val[0].l)
#define FPU             (g_flags_val[1].l)
#define D2SCRATCH       (g_flags[2]&USEDFLAG)
#define NOA4            (g_flags[3]&USEDFLAG)
#define SMALLCODE       (g_flags[4]&USEDFLAG)
#define SMALLDATA       (g_flags[5]&USEDFLAG)
#define PROFILER        (g_flags[6]&USEDFLAG)
#define CONSTINDATA     (g_flags[7]&USEDFLAG)
#define USEFRAMEPOINTER (g_flags[8]&USEDFLAG)
#define NOPEEPHOLE      (g_flags[9]&USEDFLAG)
#define NODELAYEDPOP    (g_flags[10]&USEDFLAG)
#define GAS             (g_flags[11]&USEDFLAG)
#define BRANCHOPT       (g_flags[12]&USEDFLAG)
#define NOFPRETURN      (g_flags[13]&USEDFLAG)
#define NOMREGRETURN    (g_flags[14]&USEDFLAG)
#define HUNKDEBUG       (g_flags[15]&USEDFLAG)
#define NOINTZ          (g_flags[16]&USEDFLAG)
#define OLDPEEPHOLE     (g_flags[17]&USEDFLAG)
#define CONSERVATIVE_SR (g_flags[18]&USEDFLAG)
#define ELF             (g_flags[19]&USEDFLAG)
#define USE_COMMONS     (g_flags[20]&USEDFLAG)
#define A2SCRATCH       (g_flags[21]&USEDFLAG)


static int use_sd;

#ifdef M68K_16BIT_INT
static long malign[MAX_TYPE+1]=  {1,1,2,2,2,2,2,2,2,1,2,1,1,1,2,1};
static long msizetab[MAX_TYPE+1]={0,1,2,2,4,8,4,8,8,0,4,0,0,0,4,0};
#else
static long malign[MAX_TYPE+1]=  {1,1,2,2,2,2,2,2,2,1,2,1,1,1,2,1};
static long msizetab[MAX_TYPE+1]={0,1,2,4,4,8,4,8,8,0,4,0,0,0,4,0};
#endif

static struct Typ ltyp={LONG},larray={ARRAY,&ltyp},lltyp={LLONG};

static char cpu_macro[16],fpu_macro[16],*marray[6];

static char pushreglist[200],popreglist[200];

#define DATA 0
#define BSS 1
#define CODE 2
#define SPECIAL 3

static char *labprefix,*idprefix;

static int reglabel,freglabel,section=-1;
enum {
  a0=1,a1,a2,a3,a4,a5,a6,a7,
  d0,d1,d2,d3,d4,d5,d6,d7,
  fp0,fp1,fp2,fp3,fp4,fp5,fp6,fp7,
  d0d1,d2d3,d4d5,d6d7
};
static int sp=8,fbp=6,framesize;
static int stack_valid;
static char *codename,*bssname,*dataname;
static char *m_bssname,*m_dataname;
static char *rprefix;

static void emit_obj(FILE *,struct obj *,int);
static struct IC *do_refs(FILE *,struct IC *);
static void pr(FILE *,struct IC *);
static int get_reg(FILE *,int,struct IC *);
static long pof2(zumax);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

#define saveregs(x,y) saveregswfp(x,y,0)
static void saveregswfp(FILE *,struct IC *,int);
static void restoreregsa(FILE *,struct IC *);
static void restoreregsd(FILE *,struct IC *);

static void assign(FILE *,struct IC *,struct obj *,struct obj *,int,long,int);
static int compare_objects(struct obj *o1,struct obj *o2);

static char x_s[]={'0','b','w','3','l'};
#ifdef M68K_16BIT_INT
static char x_t[]={'?','b','w','w','l','L','s','d','d','v','l','a','s','u','e','f'};
#else
static char x_t[]={'?','b','w','l','l','L','s','d','d','v','l','a','s','u','e','f'};
#endif

static char *quick[2]={"","q"};
static char *strshort[2]={"l","w"};

static char *ubranch[]={"eq","ne","cs","cc","ls","hi"};

static int pushedreg,stored_cc; /* pushedreg&2: aregsaved; 4: dreg; 8: freg */
                                /* 16: durch RESTOREREGS gepushed           */
static int comptyp;
static int pushflag;
static int geta4;

#define D16OFF 1024

static int newobj=0;   /*  um zu erkennen, ob neue section erlaubt ist */

static int cf;

static int isquickkonst(union atyps *,int),isquickkonst2(union atyps *,int),regavailable(int);
static void move(FILE *,struct obj *,int,struct obj *,int,int);
static void loadext(FILE *,int,struct obj *,int);
static void add(FILE *,struct obj *,int,struct obj *,int,int);
static void sub(FILE *,struct obj *,int,struct obj *,int,int);
static void mult(FILE *,struct obj *,int,struct obj *,int,int,int,struct IC *);

extern int static_cse;

static void scratch_modified(void)
{
  BSET(regs_modified,a0);
  BSET(regs_modified,a1);
  BSET(regs_modified,d0);
  BSET(regs_modified,d1);
  if(FPU>68000){
    BSET(regs_modified,fp0);
    BSET(regs_modified,fp1);
  }
  if(D2SCRATCH) BSET(regs_modified,d2);
  if(A2SCRATCH) BSET(regs_modified,a2);
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

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

#ifdef M68K_16BIT_INT
#define ISHWORD(x) ((x&NQ)<=INT)
#else
#define ISHWORD(x) ((x&NQ)<INT)
#endif

#define PEA -1
#define LEA -2

static void emit_lword(FILE *,struct obj *);
static void emit_hword(FILE *,struct obj *);
static int addressing(struct IC *);
static long notpopped,dontpop,stackoffset,loff,maxpushed,stack;
static int offlabel,regoffset;
/*  For keeping track of condition codes.   */
static struct obj *cc_set,*cc_set_tst;
static int cc_typ,cc_typ_tst;
static int missing,savedemit,savedalloc;

static int special_section(FILE *f,struct Var *v)
{
  char *sec;
  if(!v->vattr) return 0;
  sec=strstr(v->vattr,"section(");
  if(!sec) return 0;
  sec+=strlen("section(");
  if(GAS)
    emit(f,"\t.section\t");
  else
    emit(f,"\tsection\t");
  while(*sec&&*sec!=')') emit_char(f,*sec++);
  emit(f,"\n");
  if(f) section=SPECIAL;
  return 1;
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
  if(stackoffset<maxpushed) 
    maxpushed=stackoffset;
  if(-maxpushed>stack)
    stack=-maxpushed;
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
    if(GAS)
      emit(f,"\t.file\t\"%s\"\n",inname);
    else
      emit(f,"\tidnt\t\"%s\"\n",inname);
  }
}      

static int is_arg_reg(struct IC *p,int r)
{
  int i,u;struct IC *a;
  for(i=0;i<p->arg_cnt;i++){
    a=p->arg_list[i];
    u=0;
    if((a->z.flags&(REG|DREFOBJ))==REG)
      u=a->z.reg;
    else if((a->z.flags&VAR)&&a->z.v->reg)
      u=a->z.v->reg;
    if(u){
      if(r==u||(reg_pair(u,&rp)&&r==rp.r1||r==rp.r2)){
	return 1;
      }
    }
  }
  return 0;
}

static int pget_reg(FILE *f,int flag,struct IC *p)
{
  int i;
  flag=1+flag*8;
  for(i=flag;i<flag+8;i++){
    if(regs[i]==1&&(!p||(i!=p->q1.reg&&i!=p->q2.reg&&i!=p->z.reg))){
      if(p){
	if((p->q1.am&&(p->q1.am->dreg==i||p->q1.am->basereg==i))
	   ||(p->q2.am&&(p->q2.am->dreg==i||p->q2.am->basereg==i))
	   ||(p->z.am&&(p->z.am->dreg==i||p->z.am->basereg==i))){
	  continue;
	}
	if(p->code==CALL&&is_arg_reg(p,i))
	  continue;
      }
      regs[i]|=8;
      pushflag=1;
      emit(f,"\tmove.l\t%s,%ld(%s)\n",mregnames[i],-stackoffset,mregnames[sp]);
      if(i<d0) 
	pushedreg|=2;
      else if (i<fp0)
	pushedreg|=4;
      else
	pushedreg|=8;
      BSET(regs_modified,i);
      return i;
    }
  }
  ierror(0);
}
static int get_reg(FILE *f,int flag,struct IC *p)
/*  Gets a register: flag=0=areg, 1=dreg, 2=fpreg           */
{
  int i;
  flag=1+flag*8;
  for(i=flag;i<flag+8;i++){
    if(regs[i]==0){
      if(p){
	if((p->q1.am&&(p->q1.am->dreg==i||p->q1.am->basereg==i))
	   ||(p->q2.am&&(p->q2.am->dreg==i||p->q2.am->basereg==i))
	   ||(p->z.am&&(p->z.am->dreg==i||p->z.am->basereg==i))){
	  continue;
	}
	if(p->code==CALL&&is_arg_reg(p,i))
	  continue;
      }
      regs[i]=2;pushedreg|=1;
      if(!regused[i]&&!regscratch[i]){regused[i]=1; }
      BSET(regs_modified,i);
      return i;
    }
  }
  for(i=flag;i<flag+8;i++){
    static struct rpair rp;
    if(regs[i]==1){
      if(p){
	if((p->q1.am&&(p->q1.am->dreg==i||p->q1.am->basereg==i))
	   ||(p->q2.am&&(p->q2.am->dreg==i||p->q2.am->basereg==i))
	   ||(p->z.am&&(p->z.am->dreg==i||p->z.am->basereg==i))){
	  continue;
	}
	if(p->code==CALL&&is_arg_reg(p,i))
	  continue;
	if(p->q1.flags&REG){
	  if(p->q1.reg==i) continue;
	  if(reg_pair(p->q1.reg,&rp)&&(rp.r1==i||rp.r2==i)) continue;
	}
	if(p->q2.flags&REG){
	  if(p->q2.reg==i) continue;
	  if(reg_pair(p->q2.reg,&rp)&&(rp.r1==i||rp.r2==i)) continue;
	}
	if(p->z.flags&REG){
	  if(p->z.reg==i) continue;
	  if(reg_pair(p->z.reg,&rp)&&(rp.r1==i||rp.r2==i)) continue;
	}
      }
      regs[i]+=4;
      if(i<fp0){
	emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[i],mregnames[sp]);
	push(4);
      }else{
	emit(f,"\tfmove.x\t%s,-(%s)\n",mregnames[i],mregnames[sp]);
	push(12);
      }
      if(i<d0)
	pushedreg|=2;
      else if(i<fp0)
	pushedreg|=4;
      else
	pushedreg|=8;
      BSET(regs_modified,i);
      return i;
    }
  }
  ierror(0);
}
static int isquickkonst(union atyps *p,int t)
/*  Returns 1 if constant is between -128 and 127.   */
{
    zmax zm;zumax zum;
    if(ISFLOAT(t)) return 0;
    eval_const(p,t);
    if(t&UNSIGNED){
        zum=ul2zum(127UL);
        return zumleq(vumax,zum);
    }else{
        zm=l2zm(-129L);
        if(zmleq(vmax,zm)) return 0;
        zm=l2zm(127L);
        return zmleq(vmax,zm);
    }
}
static int isquickkonst2(union atyps *p,int t)
/*  Returns 1 if constant is between 1 and 8.   */
{
    zmax zm;zumax zum;
    if(ISFLOAT(t)) return 0;
    eval_const(p,t);
    if(t&UNSIGNED){
        if(zumeqto(ul2zum(0UL),vumax)) return 0;
        zum=ul2zum(8UL);
        return zumleq(vumax,zum);
    }else{
        if(zmeqto(l2zm(0L),vmax)) return 0;
        zm=l2zm(-1L);
        if(zmleq(vmax,zm)) return 0;
        zm=l2zm(8L);
        return zmleq(vmax,zm);
    }
}

static int pregavailable(struct IC *p,int art)
/*  Returns true if matching register is available. Handles arglist*/
{
    int i;
    art=1+art*8;
    for(i=art+1;i<art+8;i++)
      if(regs[i]==0&&!is_arg_reg(p,i)) return(1);
    return 0;
}

static int regavailable(int art)
/*  Returns true if matching register is available.     */
{
    int i;
    art=1+art*8;
    for(i=art+1;i<art+8;i++)
        if(regs[i]==0) return(1);
    return 0;
}
static int compare_objects(struct obj *o1,struct obj *o2)
/*  Tests if two objects are equal.     */
{
  if((o1->flags&(REG|DREFOBJ))==REG&&(o2->flags&(REG|DREFOBJ))==REG&&o1->reg==o2->reg)
    return 1;
  if(o1->flags==o2->flags&&o1->am==o2->am){
    if(!(o1->flags&VAR)||(o1->v==o2->v&&zmeqto(o1->val.vmax,o2->val.vmax))){
      if(!(o1->flags&REG)||o1->reg==o2->reg){
	return 1;
      }
    }
  }
  return 0;
}
static struct IC *do_refs(FILE *f,struct IC *p)
/*  Loads DREFOBJs into address registers, if necessary.    */
/*  Small constants are loaded into data registers if this  */
/*  improves code.                                          */
{
    int reg,c=p->code,t=p->typf,equal;
    if((p->q1.flags&DREFOBJ)&&!(p->q1.flags&KONST)&&(!(p->q1.flags&REG)||p->q1.reg<1||p->q1.reg>8)){
        equal=0;
        if(compare_objects(&p->q1,&p->q2)) equal|=1;
        if(compare_objects(&p->q1,&p->z)) equal|=2;
	if((p->code==CALL||p->code==PUSH)&&!pregavailable(p,0))
	  reg=pget_reg(f,0,p);
	else
	  reg=get_reg(f,0,p);
        p->q1.flags&=~DREFOBJ;
        emit(f,"\tmove.l\t");emit_obj(f,&p->q1,POINTER);
        p->q1.flags=REG|DREFOBJ;
        p->q1.reg=reg;
        emit(f,",%s\n",mregnames[p->q1.reg]);
        if(equal&1) p->q2=p->q1;
        if(equal&2) p->z=p->q1;
	if(c==TEST) cc_set_tst=cc_set=0;
    }
    if((p->q2.flags&DREFOBJ)&&!(p->q2.flags&KONST)&&(!(p->q2.flags&REG)||p->q2.reg<1||p->q2.reg>8)){
        if(compare_objects(&p->q2,&p->z)) equal=1; else equal=0;
        reg=get_reg(f,0,p);
        p->q2.flags&=~DREFOBJ;
        emit(f,"\tmove.l\t");emit_obj(f,&p->q2,POINTER);
        p->q2.flags=REG|DREFOBJ;
        p->q2.reg=reg;
        emit(f,",%s\n",mregnames[p->q2.reg]);
        if(equal) p->z=p->q2;
    }
    if((p->z.flags&DREFOBJ)&&!(p->z.flags&KONST)&&(!(p->z.flags&REG)||p->z.reg<1||p->z.reg>8)){
        reg=get_reg(f,0,p);
        p->z.flags&=~DREFOBJ;
        emit(f,"\tmove.l\t");emit_obj(f,&p->z,POINTER);
        p->z.flags=REG|DREFOBJ;
        p->z.reg=reg;
        emit(f,",%s\n",mregnames[p->z.reg]);
    }
    if(CPU!=68040){
    /*  Don't do it on 040 because it's slower. */
        if(x_t[t&NQ]=='l'&&(t&NQ)!=FLOAT&&(c!=ASSIGN||!isreg(z))&&
           c!=MULT&&c!=DIV&&c!=MOD&&c!=LSHIFT&&c!=RSHIFT&&c!=SETRETURN&&c!=PUSH&&c!=ADDI2P&&c!=SUBIFP&&
           (!(p->z.flags&REG)||p->z.reg<d0||p->z.reg>d7)){
            /*  Constants into registers.    */
            if(isconst(q1)&&isquickkonst(&p->q1.val,t)&&((c!=ADD&&c!=SUB&&c!=ADDI2P&&c!=SUBIFP)||!isquickkonst2(&p->q1.val,t))){
                eval_const(&p->q1.val,t);
                if((!zldeqto(d2zld(0.0),vldouble)||!zmeqto(l2zm(0L),vmax)||!zumeqto(ul2zum(0UL),vumax))&&regavailable(1)){
                    reg=get_reg(f,1,p);
                    move(f,&p->q1,0,0,reg,t);
                    p->q1.flags=REG;p->q1.reg=reg;
                    p->q1.val.vmax=l2zm(0L);
                }
            }
            if(isconst(q2)&&isquickkonst(&p->q2.val,t)&&((c!=ADD&&c!=SUB&&c!=ADDI2P&&c!=SUBIFP)||!isquickkonst2(&p->q2.val,t))){
                eval_const(&p->q2.val,t);
                if((!zldeqto(d2zld(0.0),vldouble)||!zmeqto(l2zm(0L),vmax)||!zumeqto(ul2zum(0UL),vumax))&&regavailable(1)){
                    reg=get_reg(f,1,p);
                    move(f,&p->q2,0,0,reg,t);
                    p->q2.flags=REG;p->q2.reg=reg;
                    p->q2.val.vmax=l2zm(0L);
                }
            }
        }
    }
    return p;
}
static void pr(FILE *f,struct IC *p)
/*  Release registers and pop them from stack if necessary. */
{
  int i,size=0;char *s="";
    /*  To keep track of condition codes.   */
#if 0
  if((pushedreg&12)&&(p->code==TEST||p->code==COMPARE)){
    char *fp;struct IC *branch;
    if(FPU>68000&&ISFLOAT(p->typf)) fp="f"; else fp="";
    branch=p;
    while(branch->code<BEQ||branch->code>=BRA) branch=branch->next;
    /*FIXME*/
    if((p->typf&UNSIGNED)||ISPOINTER(p->typf)){
      emit(f,"\ts%s\t-2(%s)\n",ubranch[branch->code-BEQ],mregnames[sp]);
    }else{
      emit(f,"\t%ss%s\t-2(%s)\n",fp,ename[branch->code]+1,mregnames[sp]);
    }
    stored_cc=1;
  }
#endif
  if((pushedreg&12)&&(p->code==TEST||p->code==COMPARE||p->code==SETRETURN))
    s="m";
  for(i=MAXR;i>0;i--){
    if(regs[i]==2) regs[i]=0;
    if(regs[i]&8){
      regs[i]&=~8;
      
      emit(f,"\tmove%s.l\t%ld(%s),%s\n",s,-stackoffset,mregnames[sp],mregnames[i]);
      if(i>=d0) cc_set=0;
      if(cc_set&&(cc_set->flags&REG)&&cc_set->reg==i)
	cc_set=0;
      missing++;
    }
    if(regs[i]&4){
      regs[i]&=~4;
      if(i>=1&&i<=d7){
	if(cf&&*s=='m'){
	  emit(f,"\tmove%s.l\t(%s),%s\n\taddq.l\t#4,%s\n",s,mregnames[sp],mregnames[i],mregnames[sp]);
	}else{
	  emit(f,"\tmove%s.l\t(%s)+,%s\n",s,mregnames[sp],mregnames[i]);
	}
	pop(4);size+=4;
      }else if(i>=fp0&&i<=fp7){
	emit(f,"\tfmove%s.x\t(%s)+,%s\n",s,mregnames[sp],mregnames[i]);
	pop(12);size+=12;
      }else if(i>=25&&i<=28){
	if(cf)
	  emit(f,"\tmovem.l\t(%s),%s\n\taddq.l\t#8,%s\n",mregnames[sp],mregnames[i],mregnames[sp]);
	else
	  emit(f,"\tmovem.l\t(%s)+,%s\n",mregnames[sp],mregnames[i]);
	pop(8);size+=8;
      }else
	ierror(0);
      if(i>=d0) cc_set=0;
      if(cc_set&&(cc_set->flags&REG)&&cc_set->reg==i)
	cc_set=0;
      missing++;
    }
  }
#if 0
  if((pushedreg&12)&&(p->code==TEST||p->code==COMPARE))
    emit(f,"\ttst.b\t-%d(%s)\n",size+2,mregnames[sp]);
#endif
}
static void emit_obj(FILE *f,struct obj *p,int t)
/*  Write object.   */
{
  if(p->am){
    /*  Addressing modes.   */
    if(NOPEEPHOLE) {ierror(0);p->am=0;return;}
    if(p->am->skal>=0){
      long l=0;
      if(p->flags&D16OFF) l=zm2l(p->val.vmax);
      emit(f,"(%ld",p->am->dist+l);
      if(!GAS&&CPU>=68020&&((p->am->dist+l)>32767||(p->am->dist+l)<-32768))
	emit(f,".l");
      else if(!GAS&&CPU>=68020&&((p->am->dist+l)>127||(p->am->dist+l)<-128))
	emit(f,".w");
      emit(f,",%s",mregnames[p->am->basereg]);
      if(p->am->dreg){
	emit(f,",%s",mregnames[p->am->dreg&127]);
	if(p->am->dreg&128) emit(f,".w"); else emit(f,".l");
	if(p->am->skal) emit(f,"*%d",p->am->skal);
      }
      emit(f,")");
      return;
    }
    if((p->flags&D16OFF)&&p->am->skal<0&&!zmeqto(l2zm(0L),p->val.vmax)) ierror(0);
    if(p->am->skal==-1){
      emit(f,"(%s)+",mregnames[p->am->basereg]);
      return;
    }
    if(p->am->skal==-2){    /*  Noch nicht implementiert    */
      emit(f,"-(%s)",mregnames[p->am->basereg]);
      return;
    }
  }
  if(p->flags&DREFOBJ){
    if(p->flags&KONST){
      emitval(f,&p->val,p->dtyp&NU);
      return;
    }
    emit(f,"(");
    if((p->flags&D16OFF)&&!zmeqto(l2zm(0L),p->val.vmax)){
      emitval(f,&p->val,MAXINT);
      if(!zmleq(p->val.vmax,l2zm(32767L))||!zmleq(l2zm(-32768L),p->val.vmax))
	emit(f,".l");
      else if(!zmleq(p->val.vmax,l2zm(127L))||!zmleq(l2zm(-128L),p->val.vmax))
	emit(f,".w");
      emit(f,",");
    }
  }
  if(p->flags&VARADR) emit(f,"#");
  if(p->flags&VAR) {
    if(p->flags&REG){
      emit(f,"%s",mregnames[p->reg]);
    }else if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      long os;
      os=zm2l(p->val.vmax);
      if(!USEFRAMEPOINTER&&!vlas){
	if(!zmleq(l2zm(0L),p->v->offset)) 
	  os=os+loff-zm2l(p->v->offset)+(PROFILER?16:0);
	else
	  os=os+zm2l(p->v->offset);
	if(!GAS&&CPU>=68020&&(os-stackoffset)>0x7c00) /* +l%d max.1024? */
	  emit(f,"((%ld+%s%d).l,%s)",os-stackoffset,labprefix,offlabel,mregnames[sp]);
	else
	  emit(f,"(%ld+%s%d,%s)",os-stackoffset,labprefix,offlabel,mregnames[sp]); 
      }else{
	if(!zmleq(l2zm(0L),p->v->offset))
	  os=os-zm2l(p->v->offset)+4+(PROFILER?16:0);
	else
	  os=os-(zm2l(p->v->offset)+zm2l(szof(p->v->vtyp)));
	emit(f,"(%ld",os);
	if(!GAS&&CPU>=68020&&os>0x7fff)
	  emit(f,".l");
	emit(f,",%s)",mregnames[fbp]);
      }
    }else{
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,MAXINT);emit(f,"+");}
      if(p->v->storage_class==STATIC){
	emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
	emit(f,"%s%s",idprefix,p->v->identifier);
      }
      if(use_sd&&!(p->flags&VARADR)&&!ISFUNC(p->v->vtyp->flags)
	 &&!(p->v->tattr&(CHIP|FAR))&&(CONSTINDATA||!is_const(p->v->vtyp))
	 &&zmleq(l2zm(0L),p->val.vmax)&&!zmleq(szof(p->v->vtyp),p->val.vmax))
	emit(f,"(a4)");
    }
  }
  if((p->flags&REG)&&!(p->flags&VAR)) emit(f,"%s",mregnames[p->reg]);
  if(p->flags&KONST){
    /*  This requires IEEE floats/doubles on the host compiler. */
    if(ISFLOAT(t)){
      unsigned char *ip=(unsigned char *)&p->val.vfloat;
      char *s;
      if(GAS) s="0x"; else s="$";
      emit(f,"#%s%02x%02x%02x%02x",s,ip[0],ip[1],ip[2],ip[3]);
      if((t&NQ)!=FLOAT){
	if(DEBUG&1) printf("doubleconst=%f\n",zld2d(zd2zld(p->val.vdouble)));
	emit(f,"%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
      }
    }else{
      emit(f,"#");emitval(f,&p->val,t&NU);
    }
  }
  if(p->flags&DREFOBJ) emit(f,")");
}
static void dwarf2_print_frame_location(FILE *f,struct Var *v)
{
  /*FIXME: needs a location list and correct register trabslation */
  struct obj o;
  o.flags=REG;
  if(USEFRAMEPOINTER||vlas)
    o.reg=fbp;
  else
    o.reg=sp;
  o.val.vmax=l2zm(0L);
  o.v=0;
  dwarf2_print_location(f,&o);
}
static int dwarf2_regnumber(int r)
{
  if(r<=8)
    return r+7;
  else if(r<=d7)
    return r-8;
  else if(r<=fp7)
    return r+1;
  else
    ierror(0);
}
static zmax dwarf2_fboffset(struct Var *v)
{
  long os;
  if(!v||(v->storage_class!=AUTO&&v->storage_class!=REGISTER)) ierror(0);
  if(!USEFRAMEPOINTER&&!vlas){
    if(!zmleq(l2zm(0L),v->offset)) 
      os=loff-zm2l(v->offset);
    else
      os=zm2l(v->offset);
    return l2zm(os+framesize);
  }else{
    if(!zmleq(l2zm(0L),v->offset)) 
      return l2zm(-zm2l(v->offset)+4);
    else
      return l2zm(-(zm2l(v->offset)+zm2l(szof(v->vtyp))));
  }
}

static char tsh[]={'w','l'};
static int proflabel,stacksizelabel;
static void function_top(FILE *f,struct Var *v,long offset)
/*  Writes function header. */
{
    geta4=0;
    if(GAS){
    }else{
        if(debug_info&&HUNKDEBUG) emit(f,"\tsymdebug\n");
        if(CPU!=68000) emit(f,"\tmachine\t%ld\n",CPU);
        if(cf) strshort[1]="l";
        if(FPU>68000) emit(f,"\tfpu\t1\n");
        if(SMALLCODE) emit(f,"\tnear\tcode\n");
        if(use_sd) emit(f,"\tnear\ta4,-2\n");
        emit(f,"\topt\t0\n\topt\tNQLPSM");
        if(CPU!=68040) emit(f,"R");
        if(1/*BRANCHOPT||(optflags&2)*/) emit(f,"BT");
        emit(f,"\n");
    }
    if(!special_section(f,v)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
    if(PROFILER){
        proflabel=++label;
        if(GAS){
            emit(f,"%s%d:\n\t.byte\t\"%s\",0\n",labprefix,proflabel,v->identifier);
        }else{
            emit(f,"%s%d\n\tdc.b\t\"%s\",0\n",labprefix,proflabel,v->identifier);
        }
    }
    if(v->storage_class==EXTERN){
      if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC){
        if(GAS){
	  emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
        }else{
            emit(f,"\tpublic\t%s%s\n",idprefix,v->identifier);
        }
      }
    }
    if(v->storage_class==EXTERN){
      if(GAS){
        emit(f,"\t.align\t4\n%s%s:\n",idprefix,v->identifier);
      }else{
        emit(f,"\tcnop\t0,4\n%s%s\n",idprefix,v->identifier);
      }
    }else{
      if(GAS){
        emit(f,"\t.align\t4\n%s%ld:\n",labprefix,zm2l(v->offset));
      }else{
        emit(f,"\tcnop\t0,4\n%s%ld\n",labprefix,zm2l(v->offset));
      }
    }      
    if(stack_check&&!(v->tattr&AMIINTERRUPT)){
      stacksizelabel=++label;
      if(GAS){
	emit(f,"\tmove.l\t#%s%d,-(%s)\n\tjbsr\t___stack_check\n\taddq.l\t#4,%s\n",labprefix,stacksizelabel,mregnames[sp],mregnames[sp]);
      }else{
	emit(f,"\tmove.l\t#%s%d,-(%s)\n\tjsr\t___stack_check\n\taddq.l\t#4,%s\n",labprefix,stacksizelabel,mregnames[sp],mregnames[sp]);
      }
    }      
    if(PROFILER){
        if(GAS){
            emit(f,"\tsub.l\t#16,%s\n\tmove.l\t%s,-(%s)\n\tpea\t%s%d\n\t.global\t__startprof\n\tjbsr\t__startprof\n\taddq.%s\t#8,%s\n",mregnames[sp],mregnames[sp],mregnames[sp],labprefix,proflabel,strshort[1],mregnames[sp]);
        }else{
            emit(f,"\tsub.l\t#16,%s\n\tmove.l\t%s,-(%s)\n\tpea\t%s%d\n\tpublic\t__startprof\n\tjsr\t__startprof\n\taddq.%s\t#8,%s\n",mregnames[sp],mregnames[sp],mregnames[sp],labprefix,proflabel,strshort[1],mregnames[sp]);
        }
    }
    offset=-((offset+4-1)/4)*4;
    loff=-offset;offlabel=++label;
    if(!USEFRAMEPOINTER&&!vlas){
      if(offset<0) emit(f,"\tsub%s.%s\t#%ld,%s\n",quick[offset>=-8],strshort[offset>=-32768],-offset,mregnames[sp]);
    }else{
      if(offset>=-32768||CPU>=68020){
	emit(f,"\tlink.%c\t%s,#%ld\n",tsh[offset<-32768],mregnames[fbp],offset);
      }else{
	emit(f,"\tlink.w\t%s,#-32768\n",mregnames[fbp]);offset+=32768;
	  emit(f,"\tsub.%c\t#%ld,%s\n",tsh[offset<-32768],offset,mregnames[fbp]);
      }
    }
    if(FPU>68000&&float_used){
        if(GAS){
            emit(f,"\t.word\t0xf227,%s%d\n",labprefix,freglabel);
        }else{
            emit(f,"\tfmovem.x\t%s%d,-(%s)\n",labprefix,freglabel,mregnames[sp]);
        }
    }
    if(cf){
      emit(f,"\tsub.l\t#%s%d,%s\n",labprefix,offlabel,mregnames[sp]);
      if(GAS){
        emit(f,"\tmovem.l\t#%s%d,(%s)\n",labprefix,reglabel,mregnames[sp]);
      }else{
        emit(f,"\tmovem.l\t%s%d,(%s)\n",labprefix,reglabel,mregnames[sp]);
      }
    }else{
      if(GAS){
        emit(f,"\tmovem.l\t#%s%d,-(%s)\n",labprefix,reglabel,mregnames[sp]);
      }else{
        emit(f,"\tmovem.l\t%s%d,-(%s)\n",labprefix,reglabel,mregnames[sp]);
      }
    }
    if((v->tattr&SAVEDS)&&use_sd) emit(f,"\txref\t_LinkerDB\n\tlea\t_LinkerDB,a4\n");
    stack_valid=1;
    stack=0;
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  Writes function footer. */
{
    int i,size=0;unsigned int pushval,popval;
    *pushreglist=0;*popreglist=0;
    pushval=popval=0;
    if((v->tattr&SAVEDS)&&use_sd) geta4=1;
    for(i=1;i<=16;i++){
        if((((regused[i]&&!regscratch[i])||((v->tattr&INTERRUPT)&&BTST(regs_modified,i)))&&!regsa[i])||(i==5&&geta4)||(i==d0&&pushflag)){
            if(*pushreglist) strcat(pushreglist,"/");
            strcat(pushreglist,mregnames[i]);
	    if(i!=d0||(v->tattr&INTERRUPT)){
	      if(*popreglist) strcat(popreglist,"/");
	      strcat(popreglist,mregnames[i]);
	    }
            if(i<d0){
	      pushval|=(256>>i);popval|=(128<<i);
	    }else{
	      pushval|=(0x8000>>(i-9));
	      if(i!=d0) popval|=(1<<(i-9));
	    }
            size+=4;
        }
    }
    if(pushflag){
      emit(f,"\taddq.%c\t#4,%s\n",cf?'l':'w',mregnames[sp]);
      if(v->tattr&INTERRUPT) ierror(0);
    }
    if(cf){
      if(GAS){
        if(popval)
	  emit(f,"\t.equ\t%s%d,%u\n\tmovem.l\t(%s),#%u\n\tadd.l\t#%s%d,%s\n",labprefix,reglabel,pushval,mregnames[sp],popval,labprefix,offlabel,mregnames[sp]);
        else
	  emit(f,"\t.equ\t%s%d,0\n",labprefix,reglabel);
      }else{
        if(*pushreglist)
	  emit(f,"%s%d\treg\t%s\n",labprefix,reglabel,pushreglist);
	else
	  emit(f,"%s%d\treg\n",labprefix,reglabel);
	if(*popreglist)
	  emit(f,"\tmovem.l\t(%s),%s\n\tadd.l\t#%s%d,%s\n",mregnames[sp],popreglist,labprefix,offlabel,mregnames[sp]);
      }
    }else{
      if(GAS){
        if(popval)
	  emit(f,"\t.equ\t%s%d,%u\n\tmovem.l\t(%s)+,#%u\n",labprefix,reglabel,pushval,mregnames[sp],popval);
        else
	  emit(f,"\t.equ\t%s%d,0\n",labprefix,reglabel);
      }else{
        if(*pushreglist)
	  emit(f,"%s%d\treg\t%s\n",labprefix,reglabel,pushreglist);
        else
	  emit(f,"%s%d\treg\n",labprefix,reglabel);
	if(*popreglist)
	  emit(f,"\tmovem.l\t(%s)+,%s\n",mregnames[sp],popreglist);
      }
    }
    *pushreglist=0;*popreglist=0;pushval=0xe000;popval=0xd000;
    for(i=fp0;i<=fp7;i++){
      if((((regused[i]&&!regscratch[i])||((v->tattr&INTERRUPT)&&BTST(regs_modified,i)))&&!regsa[i])){
	if(*popreglist) strcat(popreglist,"/");
	strcat(popreglist,mregnames[i]);
	pushval|=(1<<(i-17));popval|=(0x80>>(i-17));
	size+=12;
      }
    }
    if(FPU>68000&&(float_used||(v->tattr&INTERRUPT))){
      if(GAS){
	if(popval!=0xd000)
	  emit(f,"\t.equ\t%s%d,0x%x\n\t.word\t0xf21f,0x%x\n",labprefix,freglabel,(int)pushval,(int)popval);
	else
	  emit(f,"\t.equ\t%s%d,0xe000\n",labprefix,freglabel);
      }else{
	if(*popreglist)
	  emit(f,"%s%d\tfreg\t%s\n\tfmovem.x\t(%s)+,%s%d\n",labprefix,freglabel,popreglist,mregnames[sp],labprefix,freglabel);
	else
	  emit(f,"%s%d\tfreg\n",labprefix,freglabel);
        }
    }
    if(cf||(!USEFRAMEPOINTER&&!vlas)){
      if(GAS){
	emit(f,"\t.equ\t%s%d,%d\n",labprefix,offlabel,size);
      }else{
	emit(f,"%s%d\tequ\t%d\n",labprefix,offlabel,size);
      }
    }
    if(!USEFRAMEPOINTER&&!vlas){
      if(loff) emit(f,"\tadd%s.%s\t#%ld,%s\n",quick[loff<=8],strshort[loff<32768],loff,mregnames[sp]);
      framesize=size;
    }else
      emit(f,"\tunlk\t%s\n",mregnames[fbp]);
    if(PROFILER){
        if(GAS){
            emit(f,"\tmove.l\t%s,-(%s)\n\t.global\t__endprof\n\tjbsr\t__endprof\n\tadd.%s\t#20,%s\n",mregnames[sp],mregnames[sp],strshort[1],mregnames[sp]);
        }else{
            emit(f,"\tmove.l\t%s,-(%s)\n\tpublic\t__endprof\n\tjsr\t__endprof\n\tadd.%s\t#20,%s\n",mregnames[sp],mregnames[sp],strshort[1],mregnames[sp]);
        }
    }
    if(v->tattr&INTERRUPT)
      emit(f,"\trte\n");
    else
      emit(f,"\trts\n");
    if(stack_check&&!(v->tattr&AMIINTERRUPT)){
      if(GAS)
	emit(f,"\t.equ\t%s%d,%ld\n",labprefix,stacksizelabel,size+loff-maxpushed);
      else
	emit(f,"%s%d\tequ\t%ld\n",labprefix,stacksizelabel,size+loff-maxpushed);
    }
    if(stack_valid){
      if(!v->fi) v->fi=new_fi();
      v->fi->flags|=ALL_STACK;
      v->fi->stack1=l2zm(size+loff+stack);
      emit(f,"%c stacksize=%ld\n",GAS?'#':';',size+loff+stack);
    }
}
static void move(FILE *f,struct obj *q,int qreg,struct obj *z,int zreg,int t)
/*  erzeugt eine move Anweisung...Da sollen mal Optimierungen rein  */
{
    if(!zreg&&(z->flags&(REG|DREFOBJ))==REG) zreg=z->reg;
    if(!qreg&&(q->flags&(REG|DREFOBJ))==REG) qreg=q->reg;
    if(zreg==qreg&&zreg) return;
    if(q&&(q->flags&VARADR)&&zreg>=1&&zreg<=8){
        emit(f,"\tlea\t");
        q->flags&=~VARADR;emit_obj(f,q,t);q->flags|=VARADR;
        emit(f,",%s\n",mregnames[zreg]);
	BSET(regs_modified,zreg);
        return;
    }
    if(zreg>=d0&&zreg<fp0&&q&&(q->flags&(KONST|DREFOBJ))==KONST&&isquickkonst(&q->val,t)){
        emit(f,"\tmoveq\t");
    }else{
        if((zreg>=fp0&&zreg<=fp7)||(qreg>=fp0&&qreg<=fp7)){
            if(qreg>=fp0&&qreg<=fp7&&zreg>=fp0&&zreg<=fp7) emit(f,"\tfmove.x\t");
             else emit(f,"\tfmove.%c\t",x_t[t&NQ]);
        }else{
	  if(!cf||qreg||zreg)
            emit(f,"\tmove.%c\t",x_s[msizetab[t&NQ]]);
        }
    }
    if(cf&&!qreg&&!zreg){
        static struct IC dummy;
        dummy.code=ASSIGN;
        dummy.typf=t;
        dummy.q1=*q;
        dummy.q2.flags=0;
        dummy.z=*z;
        qreg=get_reg(f,1,&dummy);
	emit(f,"\tmove.%c\t",x_s[msizetab[t&NQ]]);
        emit_obj(f,q,t);
        emit(f,",%s\n\tmove.%c\t%s,",mregnames[qreg],x_s[msizetab[t&NQ]],mregnames[qreg]);
        emit_obj(f,z,t);
        emit(f,"\n");
    }else{
      if(qreg) emit(f,"%s",mregnames[qreg]); else emit_obj(f,q,t);
      emit(f,",");
      if(zreg) emit(f,"%s",mregnames[zreg]); else emit_obj(f,z,t);
      emit(f,"\n");
      if(zreg) BSET(regs_modified,zreg);
    }
}
static void loadext(FILE *f,int r,struct obj *q,int t)
/*  laedt Objekt q vom Typ t in Register r und erweitert auf long   */
{
    if(t&UNSIGNED){
      if((q->flags&(REG|DREFOBJ))==REG&&q->reg==r)
        emit(f,"\tand.l\t#%u,%s\n",((t&NQ)==CHAR?0xffu:0xffffu),mregnames[r]);
      else
        emit(f,"\tmoveq\t#0,%s\n",mregnames[r]);
    }
    move(f,q,0,0,r,t);
#ifdef M68K_16BIT_INT
    if((t&NU)==SHORT||(t&NU)==INT) emit(f,"\text.l\t%s\n",mregnames[r]);
#else
    if((t&NU)==SHORT) emit(f,"\text.l\t%s\n",mregnames[r]);
#endif
    if((t&NU)==CHAR){
      if(cf||CPU>=68020)
        emit(f,"\textb.l\t%s\n",mregnames[r]);
      else
        emit(f,"\text.w\t%s\n\text.l\t%s\n",mregnames[r],mregnames[r]);
    }
}

static void add(FILE *f,struct obj *q,int qreg,struct obj *z,int zreg,int t)
/*  erzeugt eine add Anweisung...Da sollen mal Optimierungen rein   */
{
    if(!qreg&&!q) ierror(0);
    if(!zreg&&!z) ierror(0);
    if(!zreg&&(z->flags&(REG|DREFOBJ))==REG) zreg=z->reg;
    if(cf&&x_t[t&NQ]!='l'&&(qreg||(q->flags&(KONST|DREFOBJ))!=KONST)) ierror(0);
    if(!qreg&&(q->flags&(KONST|DREFOBJ))==KONST&&isquickkonst2(&q->val,t)){
      emit(f,"\taddq.%c\t",cf?'l':x_t[t&NQ]);
    }else{
        /*  hier noch Abfrage, ob #c.w,ax   */
      emit(f,"\tadd.%c\t",cf?'l':x_t[t&NQ]);
    }
    if(qreg) emit(f,"%s",mregnames[qreg]); else emit_obj(f,q,t);
    emit(f,",");
    if(zreg) emit(f,"%s",mregnames[zreg]); else emit_obj(f,z,t);
    emit(f,"\n");
}
static void sub(FILE *f,struct obj *q,int qreg,struct obj *z,int zreg,int t)
/*  erzeugt eine sub Anweisung...Da sollen mal Optimierungen rein   */
{
    if(cf&&x_t[t&NQ]!='l'&&(qreg||(q->flags&(KONST|DREFOBJ))!=KONST)) ierror(0);
    if(!zreg&&(z->flags&(REG|DREFOBJ))==REG) zreg=z->reg;
    if(q&&(q->flags&(KONST|DREFOBJ))==KONST&&isquickkonst2(&q->val,t)){
      emit(f,"\tsubq.%c\t",cf?'l':x_t[t&NQ]);
    }else{
        /*  hier noch Abfrage, ob #c.w,ax   */
      emit(f,"\tsub.%c\t",cf?'l':x_t[t&NQ]);
    }
    if(qreg) emit(f,"%s",mregnames[qreg]); else emit_obj(f,q,t);
    emit(f,",");
    if(zreg) emit(f,"%s",mregnames[zreg]); else emit_obj(f,z,t);
    emit(f,"\n");
}
static void mult(FILE *f,struct obj *q,int qreg,struct obj *z,int zreg, int t,int c,struct IC *p)
/*  erzeugt eine mult Anweisung...Da sollen mal Optimierungen rein  */
/*  erzeugt auch div/mod etc.                                       */
{
  int modreg;
  if(!qreg&&(q->flags&(REG|DREFOBJ))==REG) qreg=q->reg;
  if(!zreg&&(z->flags&(REG|DREFOBJ))==REG) zreg=z->reg;
  if(cf&&!qreg) {qreg=get_reg(f,1,p);move(f,q,0,0,qreg,t);}
  if((c==MULT||c==DIV||c==MOD)&&CPU<68020&&!cf&&msizetab[t&NQ]==4){
    if(c==MULT){
      /*  ist das mit get_reg(.,.,0) ok? nochmal ueberdenken...   */
      /*  ...die ganze Routine am besten...                       */
      /*  ...es war nicht, deshalb ist es jetzt geaendert         */
      int dx,dy,t1,t2;
      if(zreg>=d0&&zreg<fp0){
	dx=zreg;
      }else{
	dx=get_reg(f,1,p);
	move(f,z,0,0,dx,t);
      }
      if(qreg>d0&&qreg<fp0&&qreg!=dx){
	dy=qreg;
      }else{
	dy=get_reg(f,1,p);
	move(f,q,0,0,dy,t);
      }
      t1=get_reg(f,1,p);t2=get_reg(f,1,p);
      if(t1==dx||t2==dx||t1==dy||t2==dy) ierror(0);
      emit(f,"\tmove.l\t%s,%s\n",mregnames[dx],mregnames[t1]);
      emit(f,"\tmove.l\t%s,%s\n",mregnames[dy],mregnames[t2]);
      emit(f,"\tswap\t%s\n",mregnames[t1]);
      emit(f,"\tswap\t%s\n",mregnames[t2]);
      emit(f,"\tmulu.w\t%s,%s\n",mregnames[dy],mregnames[t1]);
      emit(f,"\tmulu.w\t%s,%s\n",mregnames[dx],mregnames[t2]);
      emit(f,"\tmulu.w\t%s,%s\n",mregnames[dy],mregnames[dx]);
      emit(f,"\tadd.w\t%s,%s\n",mregnames[t2],mregnames[t1]);
      emit(f,"\tswap\t%s\n",mregnames[t1]);
      emit(f,"\tclr.w\t%s\n",mregnames[t1]);
      emit(f,"\tadd.l\t%s,%s\n",mregnames[t1],mregnames[dx]);
      if(zreg!=dx) move(f,0,t1,z,0,t);
    }else ierror(0);
    return;
  }
  if(c==MULT){
    /*  das duerfte nur der Aesthetik dienen... */
    if(t&UNSIGNED) 
      emit(f,"\tmulu.%c\t",x_t[t&NQ]);
    else 
      emit(f,"\tmuls.%c\t",x_t[t&NQ]);
  }
  if(c==DIV||(c==MOD&&ISHWORD(t))){
    if(t&UNSIGNED){
      if(ISHWORD(t)) emit(f,"\tand.l\t#65535,%s\n",mregnames[zreg]);
      emit(f,"\tdivu.%c\t",x_t[t&NQ]);
    }else{
      if(ISHWORD(t)) emit(f,"\text.l\t%s\n",mregnames[zreg]);
      emit(f,"\tdivs.%c\t",x_t[t&NQ]);
    }
  }  
  if(qreg) 
    emit(f,"%s",mregnames[qreg]); 
  else 
    emit_obj(f,q,t);
  emit(f,",");
  /*  eigentlich muss zreg!=0 sein...     */
  if(zreg) 
    emit(f,"%s",mregnames[zreg]); 
  else 
    emit_obj(f,z,t);
  emit(f,"\n");
  if(c==MOD){
    emit(f,"\tswap\t%s\n",mregnames[zreg]);
    cc_set=0;
  }
}
static struct IC *am_freedreg[9],*am_shiftdreg[9];
static struct IC *am_dist_ic[9],*am_dreg_ic[9],*am_use[9];
/*  am_dist_ic und am_dreg_ic werden auch fuer (ax)+ benutzt    */
static long am_dist[9],am_dreg[9],am_base[9],am_inc[9],am_skal[9],am_dbase[9];
#define AMS sizeof(struct AddressingMode)

static int mopsize(struct IC *p,int reg)
/*  Liefert die Groesse in Bytes, mit der im IC auf (reg) zugegriffen wird. */
{
    int c=p->code;
    if(c==ADDI2P||c==SUBIFP){
        if((p->q2.flags&REG)&&p->q2.reg==reg)
            return zm2l(sizetab[p->typf&NQ]);
        return 4;
    }
    if(c==CONVERT){
        if((p->z.flags&REG)&&p->z.reg==reg)
          return zm2l(sizetab[p->typf&NQ]);
        else
          return zm2l(sizetab[p->typf2&NQ]);
    }
    return zm2l(sizetab[p->typf&NQ]);
}
static void clear_am(int reg)
/*  loescht Werte fuer erweiterte Adressierungsarten fuer Register reg  */
{
    if(reg<0||reg>d7) ierror(0);
    if(DEBUG&32) printf("clear_am(%s)\n",mregnames[reg]);
    if(reg<=8){
        am_dist_ic[reg]=am_dreg_ic[reg]=am_use[reg]=0;
        am_dist[reg]=am_dreg[reg]=am_base[reg]=am_inc[reg]=0;
    }else{
        reg-=8;
        am_freedreg[reg]=am_shiftdreg[reg]=0;
        am_skal[reg]=am_dbase[reg]=0;
    }
}
static void mod_reg(int r)
{
  int i;
  for(i=1;i<=8;i++){
    if((!am_use[i]&&am_base[i]==r)||(am_dreg[i]&127)==r)
      clear_am(i);
  }
#if 0
  if(r>=9&&r<=16){
    for(i=9;i<=16;i++){
      if(am_dbase[i-8]==r)
	clear_am(i);
    }
  }
#endif
}
static int new_peephole(struct IC *first)
{
  int localused=0,c,r,t,c2;long sz;
  struct IC *p,*p2;
  struct AddressingMode *am;
  for(p=first;p;p=p->next){
    int c=p->code;
    if(!localused){
      if((p->q1.flags&(VAR|REG))==VAR&&(p->q1.v->storage_class==AUTO||p->q1.v->storage_class==REGISTER)&&zmleq(l2zm(0L),p->q1.v->offset))
	localused=1;
      if((p->q2.flags&(VAR|REG))==VAR&&(p->q2.v->storage_class==AUTO||p->q2.v->storage_class==REGISTER)&&zmleq(l2zm(0L),p->q2.v->offset))
	localused=1;
      if((p->z.flags&(VAR|REG))==VAR&&(p->z.v->storage_class==AUTO||p->z.v->storage_class==REGISTER)&&zmleq(l2zm(0L),p->z.v->offset))
	localused=1;
      if(DEBUG&32&&localused==1) printf("localused=1\n");
    }
    /* -(ax) */
    if(c==SUBIFP&&isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg&&p->q1.reg<=8&&isconst(q2)){
      r=p->q1.reg;
      eval_const(&p->q2.val,q2typ(p));
      sz=zm2l(vmax);
      if(sz==1||sz==2||sz==4||sz==8){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r&&(!(p2->q2.flags&REG)||p2->q2.reg!=r)&&(!(p2->z.flags&REG)||p2->z.reg!=r)&&(c2!=CONVERT||(q1typ(p2)&NQ)<=(ztyp(p2)&NQ))){
	    t=(q1typ(p2)&NQ);
	    if((ISINT(t)||ISPOINTER(t))&&t!=LLONG&&zmeqto(sizetab[q1typ(p2)&NQ],l2zm(sz))){
	      p2->q1.am=am=mymalloc(sizeof(*am));
	      p2->q1.val.vmax=l2zm(0L);
	      am->basereg=r;
	      am->dist=0;
	      am->skal=-2;
	      am->dreg=0;
	      p->code=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	      break;
	    }
	  }
	  if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r&&(!(p2->q1.flags&REG)||p2->q1.reg!=r)&&(!(p2->z.flags&REG)||p2->z.reg!=r)){
	    t=(q2typ(p2)&NQ);
	    if((ISINT(t)||ISPOINTER(t))&&t!=LLONG&&zmeqto(sizetab[q2typ(p2)&NQ],l2zm(sz))){
	      p2->q2.am=am=mymalloc(sizeof(*am));
	      p2->q2.val.vmax=l2zm(0L);
	      am->basereg=r;
	      am->dist=0;
	      am->skal=-2;
	      am->dreg=0;
	      p->code=NOP;
	      p->q1.flags=p->q2.flags=p->z.flags=0;
	      break;
	    }
	  }
	  if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r&&(!(p2->q2.flags&REG)||p2->q2.reg!=r)&&(!(p2->q1.flags&REG)||p2->q1.reg!=r)){
	    t=(ztyp(p2)&NQ);
	    if((ISINT(t)||ISPOINTER(t))&&t!=LLONG&&zmeqto(sizetab[ztyp(p2)&NQ],l2zm(sz))){
	      p2->z.am=am=mymalloc(sizeof(*am));
	      p2->z.val.vmax=l2zm(0L);
	      am->basereg=r;
	      am->dist=0;
	      am->skal=-2;
	      am->dreg=0;
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
    /* (ax)+ in q1 */
    if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q1.reg<=8&&(c!=CONVERT||(q1typ(p)&NQ)<=(ztyp(p)&NQ))){
      t=(q1typ(p)&NQ);
      sz=zm2l(sizetab[t]);
      r=p->q1.reg;
      if((sz==1||sz==2||sz==4||sz==8)&&(ISINT(t)||ISPOINTER(t))&&t!=LLONG&&(!(p->q2.flags&REG)||p->q2.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,q2typ(p2));
	    if(zmeqto(vmax,l2zm(sz))){
	      p->q1.am=am=mymalloc(sizeof(*am));
	      p->q1.val.vmax=l2zm(0L);
	      am->basereg=r;
	      am->dist=0;
	      am->skal=-1;
	      am->dreg=0;
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
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
    /* (ax)+ in q2 */
    if(!p->q2.am&&(p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q2.reg<=8){
      t=(q2typ(p)&NQ);
      sz=zm2l(sizetab[t]);
      r=p->q2.reg;
      if((sz==1||sz==2||sz==4||sz==8)&&(ISINT(t)||ISPOINTER(t))&&t!=LLONG&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,q2typ(p2));
	    if(zmeqto(vmax,l2zm(sz))){
	      p->q2.am=am=mymalloc(sizeof(*am));
	      p->q2.val.vmax=l2zm(0L);
	      am->basereg=r;
	      am->dist=0;
	      am->skal=-1;
	      am->dreg=0;
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
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
    /* (ax)+ in z */
    if(!p->z.am&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->z.reg<=8){
      t=(ztyp(p)&NQ);
      sz=zm2l(sizetab[t]);
      r=p->z.reg;
      if((sz==1||sz==2||sz==4||sz==8)&&(ISINT(t)||ISPOINTER(t))&&t!=LLONG&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,q2typ(p2));
	    if(zmeqto(vmax,l2zm(sz))){
	      p->z.am=am=mymalloc(sizeof(*am));
	      p->z.val.vmax=l2zm(0L);
	      am->basereg=r;
	      am->dist=0;
	      am->skal=-1;
	      am->dreg=0;
	      p2->code=NOP;
	      p2->q1.flags=p2->q2.flags=p2->z.flags=0;
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
    /* d(ax) (+d(ax,dy)) */
    if((c==ADDI2P||c==SUBIFP)&&isreg(z)&&p->z.reg<=8&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
      int base,idx;zmax of;struct obj *o;
      struct IC *idx_ic=0,*free_idx=0,*free_base=0,*use=0;
      eval_const(&p->q2.val,p->typf);
      if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
      if(CPU>=68020||(zmleq(l2zm(-32768L),vmax)&&zmleq(vmax,l2zm(32767L)))){
	r=p->z.reg;
	if(isreg(q1)&&p->q1.reg<=8) base=p->q1.reg; else base=r;
	o=0;
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(!idx_ic&&c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg>=d0&&p2->q2.reg<fp0){
	    if(CPU>=68020||(zmleq(of,l2zm(127L))&&zmleq(l2zm(-128L),of))){
	      idx=p2->q2.reg;
	      idx_ic=p2;
	      continue;
	    }
	  }
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(!use&&idx_ic&&c2==FREEREG&&p2->q1.reg==idx){
	    free_idx=p2;
	    continue;
	  }
	  if(!use&&c2==FREEREG&&p2->q1.reg==base){
	    free_base=p2;
	    continue;
	  }
	  if(idx_ic){
	    if((p2->q1.flags&REG)&&p2->q1.reg==idx) break;
	    if((p2->q2.flags&REG)&&p2->q2.reg==idx) break;
	    if((p2->z.flags&REG)&&p2->z.reg==idx) break;
	    if(p2->q1.am&&p2->q1.am->dreg==idx) break;
	    if(p2->q2.am&&p2->q2.am->dreg==idx) break;
	    if(p2->z.am&&p2->z.am->dreg==idx) break;
	    if(c2==ALLOCREG&&p2->q1.reg==idx) break;
	  }

	  if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&c2==PUSH){
	    if(o) break;
	    o=&p2->q1;
	    use=p2;
	    continue;
	  }

	  if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
	  if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
	  if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
	    if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
	      if(o) break;
	      o=&p2->q1;
	      use=p2;
	    }
	    if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
	      if(o) break;
	      o=&p2->q2;
	      use=p2;
	    }
	    if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){

	      if(o) break;
	      if(p2->z.flags&&(idx==d0||idx==d1)&&FPU<=68000&&ISFLOAT(ztyp(p2))&&!(p2->q2.flags==0&&c2!=CONVERT)) break;
	      o=&p2->z;
	      use=p2;
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
		o->val.vmax=l2zm(0L);
		am->basereg=base;
		am->dist=zm2l(of);
		am->skal=0;
		if(free_base) move_IC(use,free_base);
		if(idx_ic){
		  am->dreg=idx;
		  if(free_idx) move_IC(use,free_idx);
		  idx_ic->code=NOP;
		  idx_ic->q1.flags=idx_ic->q2.flags=idx_ic->z.flags=0;
		}else
		  am->dreg=0;
		if(isreg(q1)&&p->q1.reg<=8){
		  p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
		}else{
		  p->code=c=ASSIGN;p->q2.flags=0;
		  p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
		}
		if(!(o->flags&DREFOBJ)){
		  o->flags|=DREFOBJ;
		  if(use->code==PUSH)
		    use->code=PEA;
		  else if(use->code==ASSIGN)
		    use->code=LEA;
		  else
		    ierror(0);
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
    /* (ax,dy) (+d(ax,dy)) */
    if(c==ADDI2P&&isreg(q2)&&p->q2.reg>=d0&&isreg(z)&&p->z.reg<=8&&(isreg(q1)||p->q2.reg!=p->z.reg)){
      int base,idx;struct obj *o;
      long dist=0;
      struct IC *free_idx=0,*free_base=0,*use=0,*off=0;
      r=p->z.reg;idx=p->q2.reg;
      if(isreg(q1)&&p->q1.reg<=8) base=p->q1.reg; else base=r;
      o=0;
      for(p2=p->next;p2;p2=p2->next){
        c2=p2->code;
	if(!off&&(c2==ADDI2P||c2==SUBIFP)&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	  eval_const(&p2->q2.val,p2->typf);
	  if(c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	  if(CPU>=68020||(zmleq(vmax,l2zm(127L))&&zmleq(l2zm(-128L),vmax))){
	    dist=zm2l(vmax);
	    off=p2;
	    continue;
	  }
	}
	
        if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;



	if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&c2==PUSH){
	  if(o) break;

	  o=&p2->q1;
	  use=p2;
	  continue;
	}

        if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
        if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
        if((p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==idx&&idx!=r) break;
	if(c2==ALLOCREG&&(p->q1.reg==idx||p->q1.reg==base)) break;
	if(!use&&c2==FREEREG&&p2->q1.reg==base) free_base=p2;
	if(!use&&c2==FREEREG&&p2->q1.reg==idx) free_idx=p2;


        if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
          if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            if(o||(q1typ(p2)&NQ)==LLONG) break;
            o=&p2->q1;use=p2;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            if(o||(q2typ(p2)&NQ)==LLONG) break;
            o=&p2->q2;use=p2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            if(o||(ztyp(p2)&NQ)==LLONG) break;
	    if(p2->z.flags&&(idx==d0||idx==d1)&&FPU<=68000&&ISFLOAT(ztyp(p2))&&!(p2->q2.flags==0&&c2!=CONVERT)) break;

            o=&p2->z;use=p2;
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
	      o->val.vmax=l2zm(0L);
              am->basereg=base;
              am->dreg=idx;
	      am->dist=dist;
	      am->skal=0;
	      if(ISHWORD(q2typ(p))) am->dreg|=128;
	      if(isreg(q1)&&p->q1.reg<=8){
		p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
	      }else{
		p->code=c=ASSIGN;p->q2.flags=0;
		p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
	      }
	      if(off){
		off->code=NOP;
		off->q1.flags=off->q2.flags=off->z.flags=0;
	      }
	      if(!use) ierror(0);
	      if(free_idx) move_IC(use,free_idx);
	      if(free_base) move_IC(use,free_base);
	      if(free_idx&&use->next!=free_idx&&use->next->next!=free_idx) ierror(0);
	      if(free_base&&use->next!=free_base&&use->next->next!=free_base) ierror(0);
	      if(!(o->flags&DREFOBJ)){
		o->flags|=DREFOBJ;
		if(use->code==PUSH)
		  use->code=PEA;
		else if(use->code==ASSIGN)
		  use->code=LEA;
		else
		  ierror(0);
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

  /* do an additional pass to search for scaled addressing-modes */
  if(CPU>=68020){
    for(p=first;p;p=p->next){
      c=p->code;t=p->typf;
      if(c==MULT&&isconst(q2)&&isreg(z)&&p->z.reg>=d0&&p->z.reg<=d7&&ISINT(t)&&(t&NQ)<=LONG){
	unsigned long ul;
	r=p->z.reg;
	eval_const(&p->q2.val,t);
	ul=zum2ul(vumax);
	if(ul==2||ul==4||ul==8){
	  struct AddressingMode *amuse=0;
	  struct IC *free_src=0,*free_rsrc=0,*use=0;
	  int src_mod=0;
	  for(p2=p->next;p2;p2=p2->next){
	    c2=p2->code;
	    if(!use&&p2->q1.am&&p2->q1.am->skal==0&&p2->q1.am->dreg==r&&(!p2->q2.am||p2->q2.am->dreg!=r)&&(!p2->z.am||p2->z.am->dreg!=r)&&(!(p2->q2.flags&REG)||p2->q2.flags!=r)&&(!(p2->z.flags&REG)||p2->z.flags!=r)){
	      amuse=p2->q1.am;
	      use=p2;
	      continue;
	    }
	    if(!use&&p2->q2.am&&p2->q2.am->skal==0&&p2->q2.am->dreg==r&&(!p2->q1.am||p2->q1.am->dreg!=r)&&(!p2->z.am||p2->z.am->dreg!=r)&&(!(p2->q1.flags&REG)||p2->q1.flags!=r)&&(!(p2->z.flags&REG)||p2->z.flags!=r)){
	      amuse=p2->q2.am;
	      use=p2;
	      continue;
	    }
	    if(!use&&p2->z.am&&p2->z.am->skal==0&&p2->z.am->dreg==r&&(!p2->q2.am||p2->q2.am->dreg!=r)&&(!p2->q1.am||p2->q1.am->dreg!=r)&&(!(p2->q2.flags&REG)||p2->q2.flags!=r)&&(!(p2->z.flags&REG)||p2->z.flags!=r)){
	      amuse=p2->z.am;
	      use=p2;
	      continue;
	    }
	    if(!use&&c2==FREEREG&&p2->q1.reg==r){
	      free_src=p2;
	      continue;
	    }
	    if(!use&&c2==FREEREG&&isreg(q1)&&p->q1.reg>=d0&&p->q1.reg<=d7&&p2->q1.reg==p->q1.reg){
	      free_rsrc=p2;
	      continue;
	    }
	    if(use&&((c2==FREEREG&&p2->q1.reg==r)||((p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r))){
	      amuse->skal=ul;
	      if(!src_mod&&isreg(q1)&&p->q1.reg>=d0&&p->q1.reg<=d7){
		amuse->dreg=p->q1.reg;
		p->code=NOP;
		p->q1.flags=p->q2.flags=p->z.flags=0;
	      }else{
		p->code=ASSIGN;
		p->q2.flags=0;
		p->q2.val.vmax=sizetab[p->typf&NQ];
	      }
	      if(free_src) move_IC(use,free_src);
	      if(free_rsrc) move_IC(use,free_rsrc);
	      break;
	    }
	    if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	    if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
	    if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
	    if((p2->z.flags&REG)&&p2->z.reg==r) break;
	    if(p2->q1.am&&p2->q1.am->dreg==r) break;
	    if(p2->q2.am&&p2->q2.am->dreg==r) break;
	    if(p2->z.am&&p2->z.am->dreg==r) break;
	    if((p2->z.flags&(REG|DREFOBJ))==REG&&(p->q1.flags&(REG|DREFOBJ))&&p2->z.reg==p->q1.reg)
	      src_mod=1;
	  }
	}
      }
    }	    
  }

  /* another pass to remove unnecessary ALLOCREGs */
  for(p=first;p;p=p->next){
    if(p->code==ALLOCREG){
      r=p->q1.reg;
      for(p2=p->next;p2;p2=p2->next){
	c2=p2->code;
	if(c2==ALLOCREG&&p2->q1.reg==r) ierror(0);
	if(c2==FREEREG&&p2->q1.reg==r){
	  p->code=NOP;
	  p->q1.flags=0;
	  p2->code=NOP;
	  p2->q1.flags=0;
	  savedalloc++;
	  break;
	}
	if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
	if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
	if((p2->z.flags&REG)&&p2->z.reg==r) break;
	if((am=p2->q1.am)&&(am->basereg==r||am->dreg==r)) break;
	if((am=p2->q2.am)&&(am->basereg==r||am->dreg==r)) break;
	if((am=p2->z.am)&&(am->basereg==r||am->dreg==r)) break;
      }
    }
  }

  return localused;
}
static int addressing(struct IC *p)
/*  Untersucht ICs auf erweiterte Addresierungsarten    */
{
    int count,localused=0;
    if(!OLDPEEPHOLE) return new_peephole(p);
    if(DEBUG&32) printf("addressing() started\n");
    for(count=1;count<=16;count++) clear_am(count);
    for(count=0;p;p=p->next){
        int c=p->code,q1reg,q2reg,zreg;
        if(p->q1.flags&REG) q1reg=p->q1.reg; else q1reg=0;
        if(p->q2.flags&REG) q2reg=p->q2.reg; else q2reg=0;
        if(p->z.flags&REG) zreg=p->z.reg; else zreg=0;
        if(c==ADDI2P) c=ADD;
        if(c==SUBIFP) c=SUB;
        if(DEBUG&32) pric2(stdout,p);
        if(!localused){
            if((p->q1.flags&(VAR|REG))==VAR&&(p->q1.v->storage_class==AUTO||p->q1.v->storage_class==REGISTER)&&zmleq(l2zm(0L),p->q1.v->offset))
                localused=1;
            if((p->q2.flags&(VAR|REG))==VAR&&(p->q2.v->storage_class==AUTO||p->q2.v->storage_class==REGISTER)&&zmleq(l2zm(0L),p->q2.v->offset))
                localused=1;
            if((p->z.flags&(VAR|REG))==VAR&&(p->z.v->storage_class==AUTO||p->z.v->storage_class==REGISTER)&&zmleq(l2zm(0L),p->z.v->offset))
                localused=1;
            if(DEBUG&32&&localused==1) printf("localused=1\n");
        }
        if(c==ASSIGN&&isreg(q1)&&isreg(z)&&q1reg>=1&&q1reg<=8&&zreg>=1&&zreg<=8){
        /*  fuer (ax)+  */
            int i;
            clear_am(q1reg);
            for(i=1;i<=8;i++)
                if(am_base[i]==zreg||am_base[i]==q1reg) clear_am(i);
	    mod_reg(zreg);
            clear_am(zreg);am_base[zreg]=q1reg;am_dreg_ic[zreg]=p;
            if(DEBUG&32) printf("move %s,%s found\n",mregnames[q1reg],mregnames[zreg]);
            continue;
        }
        if(c==MULT&&CPU>=68020&&isconst(q2)&&isreg(z)&&zreg>=d0&&zreg<=d7){
        /*  dx=a*const, fuer Skalierung    */
            int dreg=zreg-8;
            if(dreg<1||dreg>8) ierror(0);
            if(q1reg>=1&&q1reg<=d7){
                if(isreg(q1)&&(q1reg>8||am_use[q1reg]!=p)) clear_am(q1reg);
                if((p->q1.flags&DREFOBJ)&&q1reg<=8&&am_use[q1reg]) clear_am(q1reg);
            }
            if(DEBUG&32) printf("mult x,const->dreg found\n");
	    mod_reg(zreg);
            if(am_skal[dreg]) {clear_am(zreg);continue;}
            eval_const(&p->q2.val,p->typf);
            am_skal[dreg]=zm2l(vmax);
            if(am_skal[dreg]!=2&&am_skal[dreg]!=4&&am_skal[dreg]!=8)
                {clear_am(zreg);continue;}
            am_shiftdreg[dreg]=p;
            if(isreg(q1)&&q1reg>=d0&&q1reg<=d7) am_dbase[dreg]=q1reg; else am_dbase[dreg]=zreg;
	    if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)) clear_am(p->q1.reg);
            if(DEBUG&32) printf("is usable\n");
            continue;
        }
        if((c==ADD||c==SUB)&&isconst(q2)&&zreg>=1&&zreg<=8&&isreg(z)){
        /*  add ax,#const,ax->az Test auf d8/16 fehlt noch (nicht mehr) */
            long l;
            if(zreg<1||zreg>8) ierror(0);
            eval_const(&p->q2.val,p->typf);
            l=zm2l(vmax);
            if(c==SUB) l=-l;
	    mod_reg(zreg);
            if(q1reg==zreg&&isreg(q1)&&am_use[zreg]&&(l==1||l==2||l==4)){
                if(l==mopsize(am_use[zreg],zreg)&&(am_use[zreg]->code!=CONVERT||zmleq(sizetab[am_use[zreg]->typf2&NQ],sizetab[am_use[zreg]->typf&NQ]))){
                    struct IC *op=am_use[zreg];
                    struct obj *o=0;
                    if(DEBUG&32){ printf("found postincrement:\n");pric2(stdout,op);pric2(stdout,p);}
                    if((op->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&op->q1.reg==zreg){
                        if(DEBUG&32) printf("q1\n");
                        o=&op->q1;
                    }
                    if((op->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&op->q2.reg==zreg){
                        if(DEBUG&32) printf("q2\n");
                        if(o) continue; else o=&op->q2;
                    }
                    if((op->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&op->z.reg==zreg){
                        if(DEBUG&32) printf("z\n");
                        if(o) continue; else o=&op->z;
                    }
                    o->am=mymalloc(AMS);
                    o->am->basereg=zreg;
                    o->am->skal=-1;
                    o->am->dist=0;
                    o->am->dreg=0;
                    p=p->prev;
                    remove_IC(p->next);
                    clear_am(zreg);continue;
                }
            }
            clear_am(q1reg);
            if(am_dist[zreg]||am_inc[zreg]||am_use[zreg]) {clear_am(zreg);continue;} /* nur ein Offset */
            if(isreg(q1)&&q1reg==zreg&&(l==1||l==2||l==4)){
            /*  ax+=const, fuer (ax)+   */
                int i,f;
                for(f=0,i=1;i<=8;i++){
                    if(am_base[i]==zreg&&!am_dreg[i]&&!am_dist[i]){
                        if(f) ierror(0);
                        am_inc[i]=l;am_dist_ic[i]=p;f=i;
                        if(DEBUG&32) printf("inc %s found\n",mregnames[i]);
                    }
                }
                if(f) continue;
            }
            am_dist[zreg]=l;
            if(DEBUG&32) printf("dist=%ld\n",am_dist[zreg]);
            if(CPU<68020){
            /*  bei <68020 darf der Offset nur 16bit oder 8bit bei dreg sein */
                if((am_dreg[zreg]&&(am_dist[zreg]<-128||am_dist[zreg]>127))||am_dist[zreg]<-32768||am_dist[zreg]>32767)
                    {clear_am(zreg);continue;}
            }
            am_dist_ic[zreg]=p;
            if(am_base[zreg]){
                if(q1reg!=zreg||!isreg(q1)) {clear_am(zreg);continue;}
            }else{
                if(q1reg>=1&&q1reg<=8&&isreg(q1)) am_base[zreg]=q1reg; else am_base[zreg]=zreg;
                if(DEBUG&32) printf("%s potential base for %s\n",mregnames[am_base[zreg]],mregnames[zreg]);
            }
            if(DEBUG&32) printf("add #const,%s found\n",mregnames[zreg]);
            continue;
        }
        if(c==ADD&&q2reg>=d0&&q2reg<=d7&&isreg(q2)&&zreg>=1&&zreg<=8&&isreg(z)&&(p->q1.flags&(REG|DREFOBJ))!=(REG|DREFOBJ)){
        /*  add ax,dy->az   */
            int i;
            if(zreg<1||zreg>8) ierror(0);
            for(i=1;i<=8;i++)
                if(am_dreg[i]==q2reg){ clear_am(q2reg);clear_am(i);}
            clear_am(q1reg);
	    mod_reg(zreg);
            if(am_dreg[zreg]||am_inc[zreg]||am_use[zreg]) {clear_am(zreg);continue;} /* nur ein Regoffset */
            if(CPU<68020&&(am_dist[zreg]<-128||am_dist[zreg]>127))
                {clear_am(zreg);continue;} /* bei <68020 nur 8bit Offset */
            am_dreg[zreg]=q2reg;
            if(ISHWORD(p->typf)) am_dreg[zreg]|=128; /* dx.w statt dx.l */
            am_dreg_ic[zreg]=p;
            if(am_base[zreg]){
                if(q1reg!=zreg||!isreg(q1)) {clear_am(zreg);continue;}
            }else{
                if(q1reg>=1&&q1reg<=8&&isreg(q1)) am_base[zreg]=q1reg; else am_base[zreg]=zreg;
            }
            if(DEBUG&32) printf("add %s,%s found\n",mregnames[q2reg],mregnames[zreg]);
            continue;
        }
        if(c==FREEREG){
        /*  wir koennen den Modus tatsaechlich benutzen */
            struct AddressingMode *am;struct IC *p1,*p2;int dreg,i;
            if(DEBUG&32) printf("freereg %s found\n",mregnames[p->q1.reg]);
            if(q1reg>=d0&&q1reg<=d7) {am_freedreg[q1reg-8]=p;if(DEBUG&32) printf("freedreg[%d]=%lx\n",q1reg-8,(long)p);}
            if(q1reg>8) continue;
            if(DEBUG&32) printf("use=%p,base=%p,dist=%p,dreg=%p\n",(void*)am_use[q1reg],(void*)am_base[q1reg],(void*)am_dist[q1reg],(void*)am_dreg[q1reg]);
            for(i=1;i<=8;i++) if(am_base[i]==q1reg) clear_am(i);
            if(!am_use[q1reg]||!am_base[q1reg]) continue;
            if(am_inc[q1reg]&&am_inc[q1reg]!=mopsize(am_use[q1reg],q1reg))
                {clear_am(q1reg);continue;}
            if(!am_dist[q1reg]&&!am_dreg[q1reg]&&!am_inc[q1reg]) continue;
            p1=am_dist_ic[q1reg];p2=am_dreg_ic[q1reg];
            if(DEBUG&32){
                printf("could really use %s\n",mregnames[q1reg]);
                if(p1) pric2(stdout,p1);
                if(p2) pric2(stdout,p2);
            }
            if(am_base[q1reg]==q1reg){
                if(p1) {p1->q2.flags=0;p1->code=ASSIGN;p1->q2.val.vmax=l2zm(4L);p1->typf=POINTER;}
                if(p2) {p2->q2.flags=0;p2->code=ASSIGN;p2->q2.val.vmax=l2zm(4L);p2->typf=POINTER;}
            }else{
                if(p1) remove_IC(p1);
                if(p2) remove_IC(p2);
            }
            dreg=(am_dreg[q1reg]&127)-8;
            am=mymalloc(AMS);
            am->skal=0;
            am->basereg=am_base[q1reg];
            am->dist=am_dist[q1reg];
            am->dreg=am_dreg[q1reg];
            if(am_inc[q1reg]) am->skal=-1;
            if(dreg>0){
                /*  bei (d,ax,dy) das freereg dy nach hinten verschieben    */
                if(dreg<1||dreg>8) ierror(0);
                if(p1=am_freedreg[dreg]){
                    if(DEBUG&32){
                        printf("freereg %s moved from %p to %p\n",mregnames[dreg+8],(void*)p1,(void*)p);
                        pric2(stdout,p1);
                    }
                    if(p1->code!=FREEREG){ierror(0);printf("freereg[%d]=%p\n",dreg,(void*)p1);continue;}
                    if(!p1->next) {ierror(0);continue;}
                    if(!p1->prev) {ierror(0);continue;}
                    p1->prev->next=p1->next;
                    p1->next->prev=p1->prev;
                    p1->next=p->next;
                    p1->prev=p;
                    if(p->next) p->next->prev=p1;
                    p->next=p1;
                }
                if(am_skal[dreg]){
                /*  Skalierung bearbeiten   */
                    if(p1){
                        am->skal=am_skal[dreg];
                        am->dreg=am_dbase[dreg];
                        p1=am_shiftdreg[dreg];
                        if(DEBUG&32) pric2(stdout,p1);
                        if(am_dbase[dreg]==dreg+8){
                            p1->code=ASSIGN;p1->q2.flags=0;p1->q2.val.vmax=sizetab[p1->typf&NQ];
                        }else remove_IC(p1);
                    }
                    clear_am(dreg+8);
                }
            }
            /*  das hier duerfte unnoetig sein, da die Adressierungsart in  */
            /*  einem IC eigentlich hoechstens einmal vorkommen darf        */
            if(q1reg<0||q1reg>8) ierror(0);
            p1=am_use[q1reg];
            if(DEBUG&32) pric2(stdout,p1);
            if(p1->code==PUSH&&p1->q1.reg==q1reg&&((p1->q1.flags&(DREFOBJ|REG))==(REG))){
                p1->q1.am=mymalloc(AMS);
                memcpy(p1->q1.am,am,AMS);
                p->q1.val.vmax=l2zm(0L);
                p1->code=PEA;
                if(DEBUG&32) printf("q1 patched\n");
            }
            if(p1->q1.reg==q1reg&&((p1->q1.flags&(DREFOBJ|REG))==(DREFOBJ|REG))){
                p1->q1.am=mymalloc(AMS);
                memcpy(p1->q1.am,am,AMS);
                p1->q1.val.vmax=l2zm(0L);
                if(DEBUG&32) printf("q1 patched\n");
            }
            if(p1->q2.reg==q1reg&&((p1->q2.flags&(DREFOBJ|REG))==(DREFOBJ|REG))){
                p1->q2.am=mymalloc(AMS);
                memcpy(p1->q2.am,am,AMS);
                p1->q2.val.vmax=l2zm(0L);
                if(DEBUG&32) printf("q2 patched\n");
            }
            if(p1->z.reg==q1reg&&((p1->z.flags&(DREFOBJ|REG))==(DREFOBJ|REG))){
                p1->z.am=mymalloc(AMS);
                memcpy(p1->z.am,am,AMS);
                p1->z.val.vmax=l2zm(0L);
                if(DEBUG&32) printf("z patched\n");
            }
            free(am);count++;
            clear_am(q1reg);
            continue;
        }
        if(c>=LABEL&&c<=BRA){
            int i;      /*  ueber basic blocks hinweg unsicher  */
            for(i=1;i<=16;i++) clear_am(i);
            continue;
        }
        /*  Wenn Libraryaufrufe noetig sind (floating point ohne FPU oder   */
        /*  32bit mul/div/mod ohne 020+) keine Addressierungsarten nutzen   */
        if(FPU<=68000&&(ISFLOAT(p->typf)||(p->code==CONVERT&&ISFLOAT(p->typf2)))){
            int i;
            for(i=1;i<=16;i++) clear_am(i);
            continue;
        }
        if(CPU<68020&&(c==DIV||c==MOD)){
            int i;
            for(i=1;i<=16;i++) clear_am(i);
            continue;
        }
        if(c==PUSH&&((p->q1.flags&(DREFOBJ|REG))==REG&&q1reg<=8&&!am_use[q1reg]&&(am_inc[q1reg]||am_dist[q1reg]||am_dreg[q1reg]))){
            if(q1reg<1||q1reg>8) ierror(0);
            if(am_inc[q1reg]&&am_inc[q1reg]!=msizetab[p->typf&NQ]) clear_am(q1reg); else am_use[q1reg]=p;
            if(DEBUG&32) printf("use of %s found\n",mregnames[q1reg]);
            continue;
        }
        if(((p->q1.flags&(DREFOBJ|REG))==(DREFOBJ|REG)&&q1reg<=8)){
            if(q1reg<1||q1reg>8) ierror(0);
            if(am_use[q1reg]&&(am_use[q1reg]!=p||am_inc[q1reg])) clear_am(q1reg); else am_use[q1reg]=p;
            if(am_inc[q1reg]&&am_inc[q1reg]!=sizetab[p->typf&NQ]) clear_am(q1reg);
            if(DEBUG&32) printf("use of %s found\n",mregnames[q1reg]);
        }
        if(((p->q2.flags&(DREFOBJ|REG))==(DREFOBJ|REG)&&q2reg<=8)){
            if(q2reg<1||q2reg>8) ierror(0);
            if(am_use[q2reg]&&(am_use[q2reg]!=p||am_inc[q2reg])) clear_am(q2reg); else am_use[q2reg]=p;
            if(am_inc[q2reg]&&am_inc[q2reg]!=sizetab[p->typf&NQ]) clear_am(q2reg);
            if(DEBUG&32) printf("use of %s found\n",mregnames[q2reg]);
        }
        if(((p->z.flags&(DREFOBJ|REG))==(DREFOBJ|REG)&&zreg<=8)){
            if(zreg<1||zreg>8) ierror(0);
            if(am_use[zreg]&&(am_use[zreg]!=p||am_inc[zreg])) clear_am(zreg); else am_use[zreg]=p;
            if(am_inc[zreg]&&am_inc[zreg]!=sizetab[p->typf&NQ]) clear_am(zreg);
            if(DEBUG&32) printf("use of %s found\n",mregnames[zreg]);
        }
        if(c==ALLOCREG){
        /*  allocreg zaehlt als zerstoerung von reg */
            p->z.flags=REG;
            p->z.reg=zreg=q1reg=p->q1.reg;
        }
        if(q1reg>=1&&q1reg<=d7&&isreg(q1)&&(q1reg>8||am_use[q1reg]!=p)) clear_am(q1reg);
        if(q2reg>=1&&q2reg<=d7&&isreg(q2)&&(q2reg>8||am_use[q2reg]!=p)) clear_am(q2reg);
        if(zreg>=1&&zreg<=d7&&isreg(z)) clear_am(zreg);
        if(isreg(z)&&zreg<=d7){
        /*  schauen, ob eines der Register ueberschrieben wird  */
        /*  wohl noch sehr langsam                              */
	  mod_reg(zreg);
        }
        if(c==ALLOCREG) p->z.flags=0;
    }
    if(DEBUG&1) printf("%d addressingmodes used, localused=%d\n",count,localused);
    return localused;
}
static int alignment(struct obj *o)
/*  versucht rauszufinden, wie ein Objekt alignet ist   */
{
    /*  wenn es keine Variable ist, kann man nichts aussagen    */
    long os;
    if((o->flags&(DREFOBJ|VAR))!=VAR||o->am) return 0;
    if(!o->v) ierror(0);
    os=zm2l(o->val.vmax);
    if(o->v->storage_class==AUTO||o->v->storage_class==REGISTER){
        if(!USEFRAMEPOINTER&&!vlas){
            if(!zmleq(l2zm(0L),o->v->offset)) os=os+loff-zm2l(o->v->offset);
             else              os=os+zm2l(o->v->offset);
        }else{
            if(!zmleq(l2zm(0L),o->v->offset)) os=os-zm2l(o->v->offset);
             else              os=os-(zm2l(o->v->offset)+zm2l(szof(o->v->vtyp)));
        }
    }
    return os&3;
}
static void stored0d1(FILE *f,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    if(o->reg==25) return;
    emit(f,"\tmove.l\t%s,%s\n",mregnames[d0],mregnames[rp.r1]);
    emit(f,"\tmove.l\t%s,%s\n",mregnames[10],mregnames[rp.r2]);
  }else{
    if(cf){
      emit(f,"\tmove.l\t%s,",mregnames[d0]);
      emit_obj(f,o,t);
      emit(f,"\n");
      emit(f,"\tmove.l\t%s,",mregnames[d1]);
      o->val.vmax=zmadd(o->val.vmax,l2zm(4L));
      emit_obj(f,o,t);
      o->val.vmax=zmsub(o->val.vmax,l2zm(4L));
      emit(f,"\n");
    }else{
      emit(f,"\tmovem.l\t%s,",mregnames[d0d1]);
      emit_obj(f,o,t);
      emit(f,"\n");
    }
  }
}
static void loadd0d1(FILE *f,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(o->reg,&rp)) 
      ierror(0);
    if(o->reg==25) return;
    emit(f,"\tmove.l\t%s,%s\n",mregnames[rp.r1],mregnames[d0]);
    emit(f,"\tmove.l\t%s,%s\n",mregnames[rp.r2],mregnames[10]);
  }else{
    if(cf){
      emit(f,"\tmove.l\t");
      emit_obj(f,o,t);
      emit(f,",%s\n",mregnames[d0]);
      emit(f,"\tmove.l\t");
      o->val.vmax=zmadd(o->val.vmax,l2zm(4L));
      emit_obj(f,o,t);
      o->val.vmax=zmsub(o->val.vmax,l2zm(4L));
      emit(f,",%s\n",mregnames[d1]);
    }else{
      emit(f,"\tmovem.l\t");
      emit_obj(f,o,t);
      emit(f,",%s\n",mregnames[d0d1]);
    }
  }
}
static void assign(FILE *f,struct IC *p,struct obj *q,struct obj *z,int c,long size,int t)
/*  Generiert Code fuer Zuweisungen und PUSH.   */
{
    /*  auch noch sehr fpu-spezifisch   */
  if(FPU<=68000&&((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)&&!((q->flags&(KONST|DREFOBJ))==KONST))
    t=LLONG;
  if(ISFLOAT(t)){
    if(q&&(q->flags&(KONST|DREFOBJ))==KONST){
      if(z&&(z->flags&(DREFOBJ|REG))==REG&&z->reg>=fp0&&z->reg<=fp7){
	/*  FP-Konstante->Register (muss immer reinpassen)  */
	if(z->reg>=fp0) emit(f,"\tfmove"); else emit(f,"\tmove");
	emit(f,".%c\t",x_t[t&NQ]);emit_obj(f,q,t);
	emit(f,",%s\n",mregnames[z->reg]);
      }else{
	/*  FP-Konstante->Speicher (evtl. auf zweimal)  */
	int m,r;unsigned char *ip=(unsigned char *)&q->val.vfloat; /* nicht sehr schoen  */
	char *s;
	if(cf&&c==ASSIGN) r=get_reg(f,1,p);
	if(GAS) s="0x"; else s="$";
	if(c==PUSH&&(t&NQ)!=FLOAT){
	  emit(f,"\tmove.l\t#%s%02x%02x%02x%02x,-(%s)\n",s,ip[4],ip[5],ip[6],ip[7],mregnames[sp]);
	  push(4);
	}
	emit(f,"\tmove.l\t#%s%02x%02x%02x%02x,",s,ip[0],ip[1],ip[2],ip[3]);
	if(c==ASSIGN){
	  if(cf)
	    emit(f,"%s\n\tmove.l\t%s,",mregnames[r],mregnames[r]);
	  if(isreg(z)&&reg_pair(p->z.reg,&rp))
	    emit(f,"%s",mregnames[rp.r1]);
	  else
	    emit_obj(f,z,t);
	}else{
	  emit(f,"-(%s)",mregnames[sp]);
	  push(4);
	}
	emit(f,"\n");
	if((t&NQ)==FLOAT||c==PUSH) return;
	m=0;
	if(z&&(z->flags&REG)){
	  m=1;z->flags|=D16OFF;
	  z->val.vmax=l2zm(0L);
	}
	vmax=l2zm(4L);
	z->val.vmax=zmadd(z->val.vmax,vmax);
	emit(f,"\tmove.l\t#%s%02x%02x%02x%02x,",s,ip[4],ip[5],ip[6],ip[7]);
	if(cf)
	  emit(f,"%s\n\tmove.l\t%s,",mregnames[r],mregnames[r]);
	if(isreg(z)&&reg_pair(p->z.reg,&rp))
	  emit(f,"%s",mregnames[rp.r2]);
	else
	  emit_obj(f,z,t);
	emit(f,"\n");
	if(m){
	  z->flags&=~D16OFF;vmax=l2zm(4L);
	  z->val.vmax=zmsub(z->val.vmax,vmax);
	}
      }
      return;
    }
    if((q&&(q->flags&REG)&&q->reg>=fp0)||(z&&(z->flags&REG)&&z->reg>=fp0)){
      if(c==ASSIGN&&(q->flags&REG)&&(z->flags&REG)&&q->reg==z->reg) return;
      if(c==ASSIGN){ move(f,q,0,z,0,t);return;}
      emit(f,"\tfmove.%c\t",x_t[t&NQ]);
      emit_obj(f,q,t);
      emit(f,",");
      if(c==PUSH){
	emit(f,"-(%s)",mregnames[sp]);
	push(size);
      } else 
	emit_obj(f,z,t);
      emit(f,"\n");return;
    }
  }
  if((t&NQ)==LLONG){
    if(z&&compare_objects(q,z)) return;
    if(((cf&&c==ASSIGN)||(q->flags&(REG|DREFOBJ))==REG)&&(!z||(z->flags&(REG|DREFOBJ))!=REG)){
      if(cf){
	if(c==ASSIGN){
	  int r;
	  if((q->flags&(REG|DREFOBJ))==REG){
	    if(!reg_pair(q->reg,&rp)) ierror(0);
	    r=rp.r2;
	  }else{
	    r=get_reg(f,1,p);
	    emit(f,"\tmove.l\t");
	    emit_lword(f,q);
	    emit(f,",%s\n",mregnames[r]);
	  }
	  emit(f,"\tmove.l\t%s,",mregnames[r]);
	  emit_lword(f,z);
	  emit(f,"\n");
	  if((q->flags&(REG|DREFOBJ))==REG){
	    r=rp.r1;
	  }else{
	    emit(f,"\tmove.l\t");
	    emit_hword(f,q);
	    emit(f,",%s\n",mregnames[r]);
	  }
	  emit(f,"\tmove.l\t%s,",mregnames[r]);
	  emit_hword(f,z);
	  emit(f,"\n");
	}else{
	  emit(f,"\tsubq.l\t#8,%s\n",mregnames[sp]);
	  push(8);
	  emit(f,"\tmovem.l\t%s,(%s)\n",mregnames[q->reg],mregnames[sp]);
	}
      }else{
	emit(f,"\tmovem.l\t%s,",mregnames[q->reg]);
	if(c==ASSIGN){
	  emit_obj(f,z,t);
	}else{
	  emit(f,"-(%s)",mregnames[sp]);
	  push(8);
	}
	emit(f,"\n");
      }
      return;
    }
    if(!cf&&(q->flags&(REG|DREFOBJ))!=REG&&!((q->flags&(KONST|DREFOBJ))==KONST)&&z&&(z->flags&(REG|DREFOBJ))==REG){
      emit(f,"\tmovem.l\t");
      emit_obj(f,q,t);
      emit(f,",%s\n",mregnames[z->reg]);
      return;
    }
    emit(f,"\tmove.l\t");
    emit_lword(f,q);
    emit(f,",");
    if(c==ASSIGN){
      emit_lword(f,z);
    }else{
      emit(f,"-(%s)",mregnames[sp]);
      push(4);
    }
    emit(f,"\n");
    emit(f,"\tmove.l\t");
    emit_hword(f,q);
    emit(f,",");
    if(c==ASSIGN){
      emit_hword(f,z);
    }else{
      emit(f,"-(%s)",mregnames[sp]);
      push(4);
    }
    emit(f,"\n");    
    return;
  }
  if((size==1||size==2||size==4)&&!ISARRAY(t)&&(p->code!=PUSH||zm2l(p->z.val.vmax)!=3)&&((t&NQ)!=CHAR||size==1)){
    if(ISSTRUCT(t)||ISUNION(t)){
      if(p->code==PUSH&&!zmeqto(p->q2.val.vmax,p->z.val.vmax)){
	if(size!=4&&size!=2) ierror(size);
	emit(f,"\tsubq.%s\t#%d,%s\n",cf?"l":"w",size,mregnames[sp]);
	push(size);
	size=zm2l(p->z.val.vmax);
	if(size!=1&&size!=2) ierror(0);
	emit(f,"\tmove.%c\t",size==1?'b':'w');
	emit_obj(f,q,(size==1?CHAR:SHORT));
	emit(f,",(%s)\n",mregnames[sp]);
	return;
      }else{
	if(size==1)
	  t=CHAR;
	else if(size==2)
	  t=SHORT;
	else
	  t=LONG;
      }
    }
    if(c==ASSIGN){move(f,q,0,z,0,t);return;}
    /*  Sonderfall pea  */
    if((q->flags&VARADR)&&c==PUSH){
      emit(f,"\tpea\t");
      q->flags&=~VARADR; emit_obj(f,q,t); q->flags|=VARADR;
      emit(f,"\n"); 
      push(size);
      return;
    }
    emit(f,"\tmove.%c\t",x_s[size]);
    emit_obj(f,q,t);
    emit(f,",");
    if(c==PUSH){
      emit(f,"-(%s)",mregnames[sp]);
      push(size);
    } else 
      emit_obj(f,z,t);
    emit(f,"\n");return;
  }else{
    int a1,a2,qreg,zreg,dreg,s=size,loops,scratch=0;char *cpstr;
    struct IC *m;
    for(m=p->next;m&&m->code==FREEREG;m=m->next){
      if(q&&m->q1.reg==q->reg) scratch|=1;
      if(z&&m->q1.reg==z->reg) scratch|=2;
    }
    a1=alignment(q);
    if(c!=PUSH)  a2=alignment(z); else a2=0;
    if((c==PUSH||(scratch&1))&&(q->flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&q->reg>=1&&q->reg<=8&&!q->am){
      qreg=q->reg;
      if(c==PUSH&&(a1&1)==0&&(a2&1)==0)
	emit(f,"\tadd%s.%s\t#%ld,%s\n",quick[s<=8],strshort[s<=32767],(long)s,mregnames[q->reg]);
    }else{
      if(c!=ASSIGN&&!regavailable(0))
	qreg=pget_reg(f,0,p);
      else
	qreg=get_reg(f,0,p);
      if(c==PUSH&&(a1&1)==0&&(a2&1)==0){
	q->flags|=D16OFF; 
	q->val.vmax=zmadd(q->val.vmax,l2zm((long)s));
	emit(f,"\tlea\t");emit_obj(f,q,POINTER);
	q->val.vmax=zmsub(q->val.vmax,l2zm((long)s));
	emit(f,",%s\n",mregnames[qreg]);
      }else{
	emit(f,"\tlea\t");emit_obj(f,q,POINTER);
	emit(f,",%s\n",mregnames[qreg]);
      }	      
    }
    if(c==PUSH){
      if((a1&1)==0&&(a2&1)==0){
	zreg=8;
      }else{
	emit(f,"\tsub%s.%s\t#%ld,%s\n",quick[s<=8],strshort[s<=32767],(long)s,mregnames[sp]);
	if(!regavailable(0))
	  zreg=pget_reg(f,0,p);
	else
	  zreg=get_reg(f,0,p);
	emit(f,"\tmove.l\t%s,%s\n",mregnames[sp],mregnames[zreg]);
      }
    }else{
      if((scratch&2)&&(z->flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&z->reg>=1&&z->reg<=8&&!z->am){
	zreg=z->reg;
      }else{
	zreg=get_reg(f,0,p);
	emit(f,"\tlea\t");emit_obj(f,z,POINTER);
	emit(f,",%s\n",mregnames[zreg]);
      }
    }
    /*  wenn Typ==CHAR, dann ist das ein inline_memcpy und wir nehmen   */
    /*  das unguenstigste Alignment an                                  */
    if((t&NQ)==CHAR){ a1=1;a2=2;}
    
    if(c==PUSH&&(a1&1)==0&&(a2&1)==0)
      cpstr="\tmove.%c\t-(%s),-(%s)\n";
    else
      cpstr="\tmove.%c\t(%s)+,(%s)+\n";
    
    if((a1&1)&&(a2&1)){emit(f,cpstr,'b',mregnames[qreg],mregnames[zreg]);s--;a1&=~1;a2&=~1;}
    if((a1&2)&&(a2&2)){emit(f,cpstr,'w',mregnames[qreg],mregnames[zreg]);s-=2;a1&=~2;a2&=~2;}
    if(!(a1&1)&&!(a2&1)) loops=s/16-1; else loops=s/4-1;
    if(loops>0){
      if(c!=ASSIGN&&!regavailable(1)) dreg=pget_reg(f,1,p);
      else dreg=get_reg(f,1,p);
      emit(f,"\tmove%s.l\t#%d,%s\n%s%d:\n",quick[loops>=-128&&loops<=127],loops,mregnames[dreg],labprefix,++label);
    }
    if(loops>=0){
      int t;
      if(!(a1&1)&&!(a2&1)) t='l'; else t='b';
      emit(f,cpstr,t,mregnames[qreg],mregnames[zreg]);
      emit(f,cpstr,t,mregnames[qreg],mregnames[zreg]);
      emit(f,cpstr,t,mregnames[qreg],mregnames[zreg]);
      emit(f,cpstr,t,mregnames[qreg],mregnames[zreg]);
    }
    if(loops>0){
      if(!cf&&loops<=32767&&loops>=-32768){
	emit(f,"\tdbra\t%s,%s%d\n",mregnames[dreg],labprefix,label);
      }else{
	emit(f,"\tsubq.l\t#1,%s\n\tbge\t%s%d\n",mregnames[dreg],labprefix,label);
      }
    }
    if(!(a1&1)&&!(a2&1)){
      if(s&8){
	emit(f,cpstr,'l',mregnames[qreg],mregnames[zreg]);
	emit(f,cpstr,'l',mregnames[qreg],mregnames[zreg]);
      }
      if(s&4) emit(f,cpstr,'l',mregnames[qreg],mregnames[zreg]);
      if(s&2) emit(f,cpstr,'w',mregnames[qreg],mregnames[zreg]);
      if(s&1) emit(f,cpstr,'b',mregnames[qreg],mregnames[zreg]);
    }else{
      s&=3;
      while(s){emit(f,cpstr,'b',mregnames[qreg],mregnames[zreg]);s--;}
    }
    if(c==PUSH) push(size);
  }
  return;
}
static int muststore(struct IC *p,int r)
{
  if(!regs[r])
    return 0;
#if 0
  /* mal bei Gelegenheit testen */
  while(p){
    if(p->code==FREEREG&&p->q1.reg==r)
      return 0;
    if((p->q1.flags&REG)&&p->q1.reg==r)
      return 1;
    if(p->q1.am&&(p->q1.am->basereg==r||p->q1.am->dreg==r))
      return 1;
    if((p->q2.flags&REG)&&p->q2.reg==r)
      return 1;
    if(p->q2.am&&(p->q2.am->basereg==r||p->q2.am->dreg==r))
      return 1;
    if(p->z.am&&(p->z.am->basereg==r||p->z.am->dreg==r))
      return 1;
    if((p->z.flags&REG)&&p->z.reg==r)
      return (p->z.flags&DREFOBJ)!=0;
    if(p->code>=LABEL&&p->code<=BRA)
      return 1;
    p=p->next;
  }
  return 0;
#else
  return 1;
#endif
}

static int store_saveregs;
static void saveregswfp(FILE *f,struct IC *p,int storefp)
{
    int dontsave;
    store_saveregs=0;
    if((p->z.flags&(REG|DREFOBJ))==REG) dontsave=p->z.reg; else dontsave=0;
    if(dontsave!= d0&&dontsave!=25&&(muststore(p,d0)||muststore(p,d0d1))) {emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[d0],mregnames[sp]);push(4);store_saveregs|=1;}
    if(dontsave!=10&&dontsave!=25&&(muststore(p,d1)||muststore(p,d0d1))) {emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[d1],mregnames[sp]);push(4);store_saveregs|=2;}
    if(dontsave!= fp0&&muststore(p,fp0)) {emit(f,"\tfmove.x\t%s,-(%s)\n",mregnames[fp0],mregnames[sp]);push(12);store_saveregs|=16;}
    if(dontsave!= fp1&&muststore(p,fp1)) {emit(f,"\tfmove.x\t%s,-(%s)\n",mregnames[fp1],mregnames[sp]);push(12);store_saveregs|=32;}
    if(dontsave!= a0&&muststore(p,a0)) {emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[a0],mregnames[sp]);push(4);store_saveregs|=4;}
    if(dontsave!= a1&&muststore(p,a1)) {emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[a1],mregnames[sp]);push(4);store_saveregs|=8;}

}
static void restoreregsa(FILE *f,struct IC *p)
{
    if(store_saveregs&8) {emit(f,"\tmove.l\t(%s)+,%s\n",mregnames[sp],mregnames[a1]);pop(4);}
    if(store_saveregs&4) {emit(f,"\tmove.l\t(%s)+,%s\n",mregnames[sp],mregnames[a0]);pop(4);}
}
static void restoreregsd(FILE *f,struct IC *p)
{
    int dontsave;
    if((p->z.flags&(REG|DREFOBJ))==REG) dontsave=p->z.reg; else dontsave=0;
    if(dontsave!=fp1&&(store_saveregs&32)){
      emit(f,"\tfmove.x\t(%s)+,%s\n",mregnames[sp],mregnames[fp1]);
      pop(12);
    }
    if(dontsave!=fp0&&(store_saveregs&16)){
      emit(f,"\tfmove.x\t(%s)+,%s\n",mregnames[sp],mregnames[fp0]);
      pop(12);
    }
    if(dontsave!=10&&(store_saveregs&2)){
      if(cf)
        emit(f,"\tmovem.l\t(%s),%s\n\taddq.l\t#4,%s\n",mregnames[sp],mregnames[d1],mregnames[sp]);
      else
	emit(f,"\tmovem.l\t(%s)+,%s\n",mregnames[sp],mregnames[d1]);
      pop(4);
    }
    if(dontsave!=d0 &&(store_saveregs&1)){
      if(cf)
        emit(f,"\tmovem.l\t(%s),%s\n\taddq.l\t#4,%s\n",mregnames[sp],mregnames[d0],mregnames[sp]);
      else
	emit(f,"\tmovem.l\t(%s)+,%s\n",mregnames[sp],mregnames[d0]);
      pop(4);
    }
}

/* emits the low word of a long long object */
static void emit_lword(FILE *f,struct obj *o)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    emit(f,"%s",mregnames[rp.r2]);
  }else if((o->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&o->val,UNSIGNED|LLONG);
    vumax=zumand(vumax,ul2zum(0xffffffff));
    insert_const(&gval,UNSIGNED|MAXINT);
    emit(f,"#");
    emitval(f,&gval,UNSIGNED|MAXINT);
  }else{
    if(o->am||(o->flags&REG)){
      o->flags|=D16OFF;
      o->val.vmax=l2zm(0L);
    }
    o->val.vmax=zmadd(o->val.vmax,l2zm(4L));
    emit_obj(f,o,LLONG);
    o->flags&=~D16OFF;
    o->val.vmax=zmsub(o->val.vmax,l2zm(4L));
  }
}
/* emits the high word of a long long object */
static void emit_hword(FILE *f,struct obj *o)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    emit(f,"%s",mregnames[rp.r1]);
  }else if((o->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&o->val,UNSIGNED|MAXINT);
    vumax=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffffff));
    insert_const(&gval,UNSIGNED|MAXINT);
    emit(f,"#");
    emitval(f,&gval,UNSIGNED|MAXINT);
  }else{
    emit_obj(f,o,LONG);
  }
}
/* process ICs with long long; return 1 if IC has been handled */
static int handle_llong(FILE *f,struct IC *p)
{
  int c=p->code,r=0,t=p->typf&NU;
  char *libfuncname;

  if(c==ADDRESS) return 0;
  
  cc_set=0;
  if(c==GETRETURN){
    if(isreg(z)&&p->z.reg==25)
      return 1;
    emit(f,"\tmove.l\t%s,",mregnames[d1]);
    emit_lword(f,&p->z);
    emit(f,"\n");
    emit(f,"\tmove.l\t%s,",mregnames[d0]);
    emit_hword(f,&p->z);
    emit(f,"\n");
    return 1;
  }
  if(c==SETRETURN){
    if(isreg(q1)&&p->q1.reg==25)
      return 1;
    emit(f,"\tmove.l\t");
    emit_lword(f,&p->q1);
    emit(f,",%s\n",mregnames[d1]);
    emit(f,"\tmove.l\t");
    emit_hword(f,&p->q1);
    emit(f,",%s\n",mregnames[d0]);    
    return 1;
  }
  if(c==CONVERT){
    int told=q1typ(p),tnew=ztyp(p);
    if(ISPOINTER(told)) told=UNSIGNED|LONG;
    if(ISPOINTER(tnew)) tnew=UNSIGNED|LONG;
    if(ISFLOAT(told)){
      saveregs(f,p);
      assign(f,p,&p->q1,0,PUSH,msizetab[told&NQ],told);
      scratch_modified();
      if(GAS)
	emit(f,"\t.global\t__flt%ldto%cint64\n\tjbsr\t__flt%ldto%cint64\n\taddq.%s\t#%ld,%s\n",msizetab[told&NQ]*8,(tnew&UNSIGNED)?'u':'s',msizetab[told&NQ]*8,(tnew&UNSIGNED)?'u':'s',strshort[1],msizetab[told&NQ],mregnames[sp]);
      else
	emit(f,"\tpublic\t__flt%ldto%cint64\n\tjsr\t__flt%ldto%cint64\n\taddq.%s\t#%ld,%s\n",msizetab[told&NQ]*8,(tnew&UNSIGNED)?'u':'s',msizetab[told&NQ]*8,(tnew&UNSIGNED)?'u':'s',strshort[1],msizetab[told&NQ],mregnames[sp]);
      pop(msizetab[told]);
      restoreregsa(f,p);
      if(!isreg(z)||p->z.reg!=25){ /* d0/d1 */
	emit(f,"\tmove.l\t%s,",mregnames[d1]);
	emit_lword(f,&p->z);
	emit(f,"\n");
	emit(f,"\tmove.l\t%s,",mregnames[d0]);
	emit_hword(f,&p->z);
	emit(f,"\n");
      }
      restoreregsd(f,p);
      return 1;
    }
    if(ISFLOAT(tnew)){
      saveregswfp(f,p,1);
      assign(f,p,&p->q1,0,PUSH,msizetab[told&NQ],told);
      scratch_modified();
      if(GAS)
	emit(f,"\t.global\t__%cint64toflt%ld\n\tjbsr\t__%cint64toflt%ld\n\taddq.%s\t#%ld,%s\n",(told&UNSIGNED)?'u':'s',msizetab[tnew&NQ]*8,(told&UNSIGNED)?'u':'s',msizetab[tnew&NQ]*8,strshort[1],msizetab[told&NQ],mregnames[sp]);
      else
	emit(f,"\tpublic\t__%cint64toflt%ld\n\tjsr\t__%cint64toflt%ld\n\taddq.%s\t#%ld,%s\n",(told&UNSIGNED)?'u':'s',msizetab[tnew&NQ]*8,(told&UNSIGNED)?'u':'s',msizetab[tnew&NQ]*8,strshort[1],msizetab[told&NQ],mregnames[sp]);
      pop(msizetab[told&NQ]);
      restoreregsa(f,p);
      if(FPU>68000){
	emit(f,"\tfmove.x\tfp0,");
	emit_obj(f,&p->z,tnew);
	emit(f,"\n");
      }else if((tnew&NQ)==FLOAT){
	emit(f,"\tmove.l\t%s,",mregnames[d0]);
	emit_obj(f,&p->z,tnew);
	emit(f,"\n");
      }else{
	emit(f,"\tmove.l\td1,");
	emit_lword(f,&p->z);
	emit(f,"\n");
	emit(f,"\tmove.l\t%s,",mregnames[d0]);
	emit_hword(f,&p->z);
	emit(f,"\n");
      }
      restoreregsd(f,p);
      return 1;
    }
    if((told&NQ)<LLONG){
      int destreg=0;
      if(ISHWORD(told)||!(told&UNSIGNED)){
	if(isreg(z)){
	  if(!reg_pair(p->z.reg,&rp)) ierror(0);
	  r=rp.r2;
	  destreg=1;
	}else{
	  r=get_reg(f,1,p);
	}
	loadext(f,r,&p->q1,told);
	p->q1.flags=REG;
	p->q1.reg=r;
	p->q1.am=0;
      }
      if(!destreg){
	emit(f,"\tmove.l\t");
	emit_obj(f,&p->q1,told);
	emit(f,",");
	emit_lword(f,&p->z);
	emit(f,"\n");
      }
      if(told&UNSIGNED){
	emit(f,"\tmove.l\t#0,");
	emit_hword(f,&p->z);
	emit(f,"\n");
      }else{
	int tmp;
	if(r==0)
	  ierror(0);
	if(destreg){
	  emit(f,"\tmove.l\t%s,%s\n",mregnames[r],mregnames[rp.r1]);
	  r=rp.r1;
	}
	tmp=get_reg(f,1,p);
	emit(f,"\tmoveq\t#31,%s\n",mregnames[tmp]);
	emit(f,"\tasr.l\t%s,%s\n",mregnames[tmp],mregnames[r]);
	if(!destreg){
	  emit(f,"\tmove.l\t%s,",mregnames[r]);
	  emit_hword(f,&p->z);
	  emit(f,"\n");
	}
      }
      return 1;
    }
    if((tnew&NQ)<LLONG){
      if(ISHWORD(tnew)<INT&&!isreg(z)){
	r=get_reg(f,1,p);
	emit(f,"\tmove.l\t");
	emit_lword(f,&p->q1);
	emit(f,",%s\n",mregnames[r]);
	emit(f,"\tmove.%c\t%s,",x_t[tnew&NQ],mregnames[r]);
	emit_obj(f,&p->z,tnew);
	emit(f,"\n");
      }else{
	emit(f,"\tmove.%c\t",x_t[tnew&NQ]);
	p->q1.val.vmax=zmadd(p->q1.val.vmax,zmsub(sizetab[LONG],sizetab[tnew&NQ]));
	emit_lword(f,&p->q1);
	emit(f,",");
	emit_obj(f,&p->z,tnew);
	emit(f,"\n");
      }
      return 1;
    }
    assign(f,p,&p->q1,&p->z,ASSIGN,msizetab[t],t);
    return 1;
  }
  if(c==ASSIGN){
    assign(f,p,&p->q1,&p->z,ASSIGN,msizetab[t],t);
    return 1;
  }
  if(c==PUSH){
    dontpop+=zm2l(p->q2.val.vmax);
    assign(f,p,&p->q1,0,PUSH,msizetab[t],t);
    return 1;
  }  
  if(c==MINUS||c==KOMPLEMENT){
    char *sl,*sh;
    if(c==MINUS){
      sl="neg.l";
      sh="negx.l";
    }else
      sl=sh="not.l";
    if(!cf&&compare_objects(&p->q1,&p->z)){
      emit(f,"\t%s\t",sl);
      emit_lword(f,&p->z);
      emit(f,"\n");
      emit(f,"\t%s\t",sh);
      emit_hword(f,&p->z);
      emit(f,"\n");      
      return 1;
    }
    if(isreg(z)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      assign(f,p,&p->q1,&p->z,ASSIGN,msizetab[t],t);
      emit(f,"\t%s\t%s\n",sl,mregnames[rp.r2]);
      emit(f,"\t%s\t%s\n",sh,mregnames[rp.r1]);
      return 1;
    }
    r=get_reg(f,1,p);
    emit(f,"\tmove.l\t");
    emit_lword(f,&p->q1);
    emit(f,",%s\n",mregnames[r]);
    emit(f,"\t%s\t%s\n",sl,mregnames[r]);
    emit(f,"\tmove.l\t%s,",mregnames[r]);
    emit_lword(f,&p->z);
    emit(f,"\n");
    emit(f,"\tmove.l\t");
    emit_hword(f,&p->q1);
    emit(f,",%s\n",mregnames[r]);
    emit(f,"\t%s\t%s\n",sh,mregnames[r]);
    emit(f,"\tmove.l\t%s,",mregnames[r]);
    emit_hword(f,&p->z);    
    emit(f,"\n");
    return 1;
  }
  if((c>=OR&&c<=AND)||c==ADD||c==SUB){
    char *sl,*sh;int t2=0;
    switch_IC(p);
    if(c==ADD){sl="add.l";sh="addx.l";}
    else if(c==SUB){sl="sub.l";sh="subx.l";}
    else if(c==AND){sl=sh="and.l";}
    else if(c==OR){sl=sh="or.l";}
    else if(c==XOR){sl=sh="eor.l";}
    else
      ierror(0);
    if(isreg(q1)&&isreg(q2)&&isreg(z)&&p->q1.reg==p->q2.reg&&p->q1.reg==p->z.reg){
      if(!reg_pair(p->q1.reg,&rp)) ierror(0);
      emit(f,"\t%s\t%s,%s\n",sl,mregnames[rp.r2],mregnames[rp.r2]);
      emit(f,"\t%s\t%s,%s\n",sh,mregnames[rp.r1],mregnames[rp.r1]);
      return 1;
    }
    if(!isreg(z)&&compare_objects(&p->q1,&p->z)){
      if(isreg(q2)||(isconst(q2)&&(c==ADD||c==SUB))){
	if(!r) r=get_reg(f,1,p);
	emit(f,"\t%s\t",sl);
	emit_lword(f,&p->q2);
      }else{
	if(!r) r=get_reg(f,1,p);
	emit(f,"\tmove.l\t");
	emit_lword(f,&p->q2);
	emit(f,",%s\n",mregnames[r]);
	emit(f,"\t%s\t%s",sl,mregnames[r]);
      }
      emit(f,",");
      emit_lword(f,&p->z);
      emit(f,"\n");
      if(!isreg(q2)||c==ADD||c==SUB||c==XOR){
	emit(f,"\tmove.l\t");
	emit_hword(f,&p->q1);
	emit(f,",%s\n",mregnames[r]);
	if(isreg(q2)){
	  if(!reg_pair(p->q2.reg,&rp)) ierror(0);
	  t2=rp.r1;
	}else{
	  t2=get_reg(f,1,p);
	  emit(f,"\tmove.l\t");
	  emit_hword(f,&p->q2);
	  emit(f,",%s\n",mregnames[t2]);
	}
	emit(f,"\t%s\t%s,%s\n",sh,mregnames[t2],mregnames[r]);
	emit(f,"\tmove.l\t%s,",mregnames[r]);
	emit_hword(f,&p->z);
	emit(f,"\n");
      }else{
	if(isreg(q2)){
	  emit(f,"\t%s\t",sh);
	  emit_hword(f,&p->q2);
	}else{
	  if(!r) r=get_reg(f,1,p);
	  emit(f,"\tmove.l\t");
	  emit_hword(f,&p->q2);
	  emit(f,",%s\n",mregnames[r]);
	  emit(f,"\t%s\t%s",sh,mregnames[r]);
	}
	emit(f,",");
	emit_hword(f,&p->z);
	emit(f,"\n");      
      }
      return 1;
    }
    if(isreg(z)&&(!isreg(q2)||p->z.reg!=p->q2.reg)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      r=rp.r2;
    }else{
      r=get_reg(f,1,p);
    }
    if(!compare_objects(&p->q1,&p->z)){
      emit(f,"\tmove.l\t");
      emit_lword(f,&p->q1);
      emit(f,",%s\n",mregnames[r]);
    }
    if(c==XOR){
      t2=get_reg(f,1,p);
      emit(f,"\tmove.l\t");
      emit_lword(f,&p->q2);
      emit(f,",%s\n",mregnames[t2]);
      emit(f,"\t%s\t%s,%s\n",sl,mregnames[t2],mregnames[r]);
    }else{
      emit(f,"\t%s\t",sl);
      emit_lword(f,&p->q2);
      emit(f,",%s\n",mregnames[r]);
    }
    if(isreg(z)&&(!isreg(q2)||p->z.reg!=p->q2.reg)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      r=rp.r1;
    }else{
      emit(f,"\tmove.l\t%s,",mregnames[r]);
      emit_lword(f,&p->z);
      emit(f,"\n");
    }
    if(!compare_objects(&p->q1,&p->z)){
      emit(f,"\tmove.l\t");
      emit_hword(f,&p->q1);
      emit(f,",%s\n",mregnames[r]);
    }
    if(c==XOR||c==ADD||c==SUB){
      if(isreg(q2)){
	if(!reg_pair(p->q2.reg,&rp)) ierror(0);
	t2=rp.r1;
      }else{
	if(!t2) t2=get_reg(f,1,p);
	emit(f,"\tmove.l\t");
	emit_hword(f,&p->q2);
	emit(f,",%s\n",mregnames[t2]);
      }
      emit(f,"\t%s\t%s,%s\n",sh,mregnames[t2],mregnames[r]);
    }else{
      emit(f,"\t%s\t",sh);
      emit_hword(f,&p->q2);
      emit(f,",%s\n",mregnames[r]);
    }
    if(!isreg(z)||(isreg(q2)&&p->z.reg==p->q2.reg)){
      emit(f,"\tmove.l\t%s,",mregnames[r]);
      emit_hword(f,&p->z);
      emit(f,"\n");
    }      
    return 1;
  }
  if(c==TEST){
    p->code=c=COMPARE;
    p->q2.flags=KONST;
    if((t&NU)==LLONG)
      p->q2.val.vllong=zm2zll(l2zm(0L));
    else if((t&NU)==(UNSIGNED|LLONG))
      p->q2.val.vullong=zum2zull(ul2zum(0UL));
    else
      ierror(0);
  }
  if(c==COMPARE){
    int rl,rh,t2;
    comptyp=p->typf;
    rl=get_reg(f,1,p);
    rh=get_reg(f,1,p);
    emit(f,"\tmove.l\t");
    emit_lword(f,&p->q1);
    emit(f,",%s\n",mregnames[rl]);
    emit(f,"\tmove.l\t");
    emit_hword(f,&p->q1);
    emit(f,",%s\n",mregnames[rh]);
    if(isreg(q2)){
      if(!reg_pair(p->q2.reg,&rp)) ierror(0);
      t2=rp.r1;
    }else{
      t2=get_reg(f,1,p);
      emit(f,"\tmove.l\t");
      emit_hword(f,&p->q2);
      emit(f,",%s\n",mregnames[t2]);
    }
    emit(f,"\tsub.l\t");
    emit_lword(f,&p->q2);
    emit(f,",%s\n",mregnames[rl]);
    emit(f,"\tsubx.l\t%s,%s\n",mregnames[t2],mregnames[rh]);
    return 1;
  }
  saveregs(f,p);
  if((c==LSHIFT||c==RSHIFT)&&(q2typ(p)&NQ)==LLONG){
    emit(f,"\tmove.l\t");
    emit_lword(f,&p->q2);
    emit(f,",-(%s)\n",mregnames[sp]);
    push(4);
  }else
    assign(f,p,&p->q2,0,PUSH,msizetab[q2typ(p)&NQ],q2typ(p));
  assign(f,p,&p->q1,0,PUSH,msizetab[q1typ(p)&NQ],q1typ(p));
  scratch_modified();
  if(c==MULT){
    if(CPU==68060)
      libfuncname="__mulint64_060";
    else if(CPU>=68020)
      libfuncname="__mulint64_020";
    else
      libfuncname="__mulint64";
  }else if(c==DIV){
    if(t&UNSIGNED){
      if(CPU==68060)
	libfuncname="__divuint64_060";
      else if(CPU>=68020)
	libfuncname="__divuint64_020";
      else
	libfuncname="__divuint64";
    }else{
      if(CPU==68060)
	libfuncname="__divsint64_060";
      else if(CPU>=68020)
	libfuncname="__divsint64_020";
      else
	libfuncname="__divsint64";
    }
  }else if(c==MOD){
    if(t&UNSIGNED){
      if(CPU==68060)
	libfuncname="__moduint64_060";
      else if(CPU>=68020)
	libfuncname="__moduint64_020";
      else
	libfuncname="__moduint64";
    }else{
      if(CPU==68060)
	libfuncname="__modsint64_060";
      else if(CPU>=68020)
	libfuncname="__modsint64_020";
      else
	libfuncname="__modsint64";
    }
  }else if(c==RSHIFT){
    if(t&UNSIGNED)
      libfuncname="__rshuint64";
    else
      libfuncname="__rshsint64";
  }else if(c==LSHIFT)
    libfuncname="__lshint64";
  else{
    printf("c=%d\n",c);
    ierror(0);
  }
  if(GAS)
    emit(f,"\t.global\t%s\n\tjbsr\t%s\n",libfuncname,libfuncname);
  else
    emit(f,"\tpublic\t%s\n\tjsr\t%s\n",libfuncname,libfuncname);
  if(c==LSHIFT||c==RSHIFT){
#ifdef M68K_16BIT_INT
    emit(f,"\tadd.%s\t#10,%s\n",strshort[1],mregnames[sp]);
    pop(10);
#else
    emit(f,"\tadd.%s\t#12,%s\n",strshort[1],mregnames[sp]);
    pop(12);
#endif
  }else{
    emit(f,"\tadd.%s\t#16,%s\n",strshort[1],mregnames[sp]);
    pop(16);
  }
  restoreregsa(f,p);
  if(!isreg(z)||p->z.reg!=25){
    emit(f,"\tmove.l\t%s,",mregnames[d0]);
    emit_hword(f,&p->z);
    emit(f,"\n");
    emit(f,"\tmove.l\t%s,",mregnames[10]);
    emit_lword(f,&p->z);
    emit(f,"\n");
  }    
  restoreregsd(f,p);
  return 1;
}

/****************************************/
/*  End of private fata and functions.  */
/****************************************/



int init_cg(void)
/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
{
    int i;
    larray.size=l2zm(3L);
    /*  Initialize some values which cannot be statically initialized   */
    /*  because they are stored in the target's arithmetic.             */
    maxalign=l2zm(4L);
    char_bit=l2zm(8L);
    for(i=0;i<=MAX_TYPE;i++){
        sizetab[i]=l2zm(msizetab[i]);
        align[i]=l2zm(malign[i]);
    }
    for(i= 1;i<=16;i++) {regsize[i]=l2zm( 4L);regtype[i]=&ltyp;}
    for(i=fp0;i<=fp7;i++) {regsize[i]=l2zm(12L);regtype[i]=&larray;}
    for(i=25;i<=28;i++) {regsize[i]=l2zm( 8L);regtype[i]=&lltyp;}

    if(ELF){
      for(i=1;i<=MAXR;i++)
	mregnames[i]=elfregnames[i];
      idprefix="";
      labprefix=".l";
      rprefix="%";
    }else{
      for(i=1;i<=MAXR;i++)
	mregnames[i]=regnames[i];
      idprefix="_";
      labprefix="l";
      rprefix="";
    }

    if(SMALLDATA) use_sd=1;


    /*  default CPU is 68000    */
    if(!(g_flags[0]&USEDFLAG)) CPU=68000;
    if(CPU==68040) static_cse=0; else static_cse=1;
    if(CPU<68000) cf=1;
    if(cf) MINADDI2P=INT; /* requires 32bit int! */
    /*  no FPU by default       */
    if(!(g_flags[1]&USEDFLAG)) FPU=0;
    if(FPU<=68000) {x_t[FLOAT]='l';}
    if(D2SCRATCH) regscratch[11]=1;
    if(A2SCRATCH) regscratch[3]=1;
    if(NOA4) regsa[5]=1;
    if(GAS){
        codename="\t.text\n";
        bssname="";
        dataname="\t.data\n";
        if(use_sd) regsa[5]=1;
    }else{
        codename="\tsection\t\"CODE\",code\n";
        if(use_sd){
            /*  preparing small data    */
            regsa[5]=1;
            bssname= "\tsection\t\"__MERGED\",bss\n";
            dataname="\tsection\t\"__MERGED\",data\n";
        }else{
            bssname= "\tsection\t\"BSS\",bss\n";
            dataname="\tsection\t\"DATA\",data\n";
        }
    }
    m_dataname=dataname;
    m_bssname=bssname;
    /*  a5 can be used if no framepointer is used.  */
    if(!USEFRAMEPOINTER){ regsa[fbp]=0;/*fbp=sp;*/}
    if(DEBUG&1) printf("CPU=%ld FPU=%ld\n",CPU,FPU);
    /*  Initialize the min/max-settings. Note that the types of the     */
    /*  host system may be different from the target system and you may */
    /*  only use the smallest maximum values ANSI guarantees if you     */
    /*  want to be portable.                                            */
    /*  That's the reason for the subtraction in t_min[INT]. Long could */
    /*  be unable to represent -2147483648 on the host system.          */
    t_min[CHAR]=l2zm(-128L);
    t_min[SHORT]=l2zm(-32768L);
    t_min[LONG]=zmsub(l2zm(-2147483647L),l2zm(1L));
#ifdef M68K_16BIT_INT
    t_min[INT]=t_min(SHORT);
#else
    t_min[INT]=t_min(LONG);
#endif
    t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
    t_min[MAXINT]=t_min(LLONG);
    t_max[CHAR]=ul2zum(127L);
    t_max[SHORT]=ul2zum(32767UL);
    t_max[LONG]=ul2zum(2147483647UL);
#ifdef M68K_16BIT_INT
    t_max[INT]=t_max(SHORT);
#else
    t_max[INT]=t_max(LONG);
#endif
    t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
    t_max[MAXINT]=t_max(LLONG);
    tu_max[CHAR]=ul2zum(255UL);
    tu_max[SHORT]=ul2zum(65535UL);
    tu_max[LONG]=ul2zum(4294967295UL);
#ifdef M68K_16BIT_INT
    tu_max[INT]=t_max(UNSIGNED|SHORT);
#else
    tu_max[INT]=t_max(UNSIGNED|LONG);
#endif
    tu_max[LLONG]=zumkompl(ul2zum(0UL));
    tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

#ifdef M68K_16BIT_INT
    stackalign=2;
#endif

    marray[0]="__section(x)=__vattr(\"section(\"#x\")\")";
    marray[1]="__M68K__";

    if(CPU>=68000&&CPU<69000)
      sprintf(cpu_macro,"__M%ld=1",CPU);
    else
      sprintf(cpu_macro,"__COLDFIRE=1");
    marray[2]=cpu_macro;

#ifdef M68K_16BIT_INT
    marray[3]="__INTSIZE=16";
#else
    marray[3]="__INTSIZE=32";
#endif

    if(FPU==68881){
      sprintf(fpu_macro,"__M68881=1");
      marray[4]=fpu_macro;
    }else if(FPU>68000&&FPU<69000){
      sprintf(fpu_macro,"__M68882=1");
      marray[4]=fpu_macro;
    }else
      marray[4]=0;
    marray[5]=0;
    target_macros=marray;


    return 1;
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
{
    long l;int tu=t->flags&NQ;
    if(tu==FLOAT){
        if(FPU>68000&&!NOFPRETURN)
            return fp0;
        else
            return d0;
    }
    if(tu==DOUBLE){
        if(FPU>68000&&!NOFPRETURN){
            return fp0;
        }else{
            if(NOMREGRETURN) return 0;
            return d0;
        }
    }
    if(tu==STRUCT||tu==UNION){
        if(!NOMREGRETURN){
            l=zm2l(szof(t));
            if(l==4||l==8||l==12||l==16) return d0;
        }
        return 0;
    }
    if(tu==LLONG)
      return 25;
    if(zmleq(szof(t),l2zm(4L))) return d0; else return 0;
}
int cost_savings(struct IC *p,int r,struct obj *o)
{
  int c=p->code;
  if(o->flags&VKONST) return INT_MIN;
  if(o->flags&DREFOBJ){
    if(r>=1&&r<=8)
      return 4;
  }
  if((c==ADDI2P||c==SUBIFP||c==ADDRESS)&&(o==&p->q1||o==&p->z)&&r>=1&&r<=8) return 4;
  if(r>=1&&r<=8){
    if(o->flags&DREFOBJ) ierror(0);
    if(c!=GETRETURN&&c!=SETRETURN&&c!=ASSIGN&&c!=PUSH&&c!=TEST&&c!=COMPARE){
      if(c==ADDI2P||c==SUBIFP){
	if(o==&p->q2)
	  return INT_MIN;
      }else if(c==SUBPFP){
	if(o==&p->z)
	  return INT_MIN;
      }else{
	return INT_MIN;
      }
    }
  }
  if(c==SETRETURN&&r==p->z.reg&&!(o->flags&DREFOBJ)) return 3;
  if(c==GETRETURN&&r==p->q1.reg&&!(o->flags&DREFOBJ)) return 3;
  if(c==TEST&&r>=9&&r<=16) return 3;
  return 2;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if(r==0) return 0;
  t&=NQ;
  if(ISFLOAT(t)){
    if(FPU>68000){
      if(r>=fp0&&r<=fp7) return(1); else return 0;
    }else{
      if(t==FLOAT)
	return (r>=d0&&r<=d7);
      else
	return (r>=25&&r<=28);
    }
  }
  if(t==POINTER&&mode<=0&&r>=d0&&r<=d7) return 1;
  if(t==POINTER&&r>=1&&r<=8) return 1;
  if(t>=CHAR&&t<=LONG){
    if((r>=d0&&r<=d7)||(mode==-1&&t>=SHORT&&r>=1&&r<=8)) return 1;
  }
  if(t==LLONG&&r>=25&&r<=28) return 1;
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
{
    int op=o&NQ,tp=t&NQ;
    /*  All pointers have the same representation.  */
    if(tp==POINTER&&op==POINTER) return 0;
    /*  Pointer and int/long as well   */
    if(const_expr){
#ifdef M68K_16BIT_INT
      if(tp==POINTER&&(op==LONG)) return 0;
      if(op==POINTER&&(tp==LONG)) return 0;
#else
      if(tp==POINTER&&(op==INT||op==LONG)) return 0;
      if(op==POINTER&&(tp==INT||tp==LONG)) return 0;
#endif
    }
    /*  Signed und Unsigned integers with the same size, too.  */
    if(op==tp) return 0;
#ifdef M68K_16BIT_INT
    /*  int==short   */
    if((tp==INT&&op==SHORT)||(tp==SHORT&&op==INT)) return 0;
#else
    /*  int==long   */
    if((tp==INT&&op==LONG)||(tp==LONG&&op==INT)) return 0;
#endif
    /* long double==double */
    if((op==DOUBLE||op==LDOUBLE)&&(tp==DOUBLE||tp==LDOUBLE)) return 0;
    return 1;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  title(f);
  if(GAS){
    if(newobj&&section!=SPECIAL)
      emit(f,"%ld\n",zm2l(size));
    else
      emit(f,"\t.space\t%ld\n",zm2l(size));
    newobj=0;
  }else{
    /*if(section!=BSS&&section!=SPECIAL&&newobj){emit(f,bssname);if(f) section=BSS;}*/
    emit(f,"\tds.b\t%ld\n",zm2l(size));newobj=0;
  }
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  title(f);
  if(zm2l(align)>1){
    if(GAS){
      emit(f,"\t.align\t4\n");
    }else{
      emit(f,"\tcnop\t0,4\n");
    }
  }
}
void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;
  title(f);
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->tattr&(FAR|CHIP)){
    if(v->tattr&CHIP){
      dataname="\tsection\t\"CHIP_DATA\",data,chip\n";
      bssname="\tsection\t\"CHIP_BSS\",bss,chip\n";
    }else{
      dataname="\tsection\t\"DATA\",data\n";
      bssname="\tsection\t\"BSS\",bss\n";
    }
    if(f) section=-1;
  }else{
    dataname=m_dataname;
    bssname=m_bssname;
  }
  if(v->storage_class==STATIC){
    if(ISFUNC(v->vtyp->flags)) return;
    if(!special_section(f,v)){
      if(v->clist&&(!constflag||CONSTINDATA||use_sd)&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
      if(v->clist&&constflag&&!CONSTINDATA&&!use_sd&&section!=CODE){emit(f,codename);if(f) section=CODE;}
      if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
    }
    if(GAS){
      if(section!=BSS) emit(f,"\t.align\t4\n%s%ld:\n",labprefix,zm2l(v->offset));
      else emit(f,"\t.comm\t%s%ld,",labprefix,zm2l(v->offset));
    }else{
      emit(f,"\tcnop\t0,4\n%s%ld\n",labprefix,zm2l(v->offset));
    }
    newobj=1;
  }
  if(v->storage_class==EXTERN){
    if(GAS){
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    }else{
      emit(f,"\tpublic\t%s%s\n",idprefix,v->identifier);
    }
    if(v->flags&(DEFINED|TENTATIVE)){
      if(!special_section(f,v)){
	if(v->clist&&(!constflag||CONSTINDATA||use_sd)&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
	if(v->clist&&constflag&&!CONSTINDATA&&!use_sd&&section!=CODE){emit(f,codename);if(f) section=CODE;}
	if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
      }
      if(GAS){
	if(section!=BSS)
	  emit(f,"\t.align\t4\n%s%s:\n",idprefix,v->identifier);
	else
	  emit(f,"\t.global\t%s%s\n\t.%scomm\t%s%s,",idprefix,v->identifier,(USE_COMMONS?"":"l"),idprefix,v->identifier);
      }else{
	emit(f,"\tcnop\t0,4\n%s%s\n",idprefix,v->identifier);
      }
      newobj=1;
    }
  }
  if(v->tattr&(FAR|CHIP)) {if(f) section=-1;newobj=0;}
}
void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  char s;
  title(f);
  if(!p) ierror(0);
/*    if(section!=DATA){emit(f,dataname);if(f) section=DATA;}*/
  if(ISFLOAT(t)||(t&NQ)==LLONG) s='l'; else s=x_t[t&NQ];
  if(GAS){
    char *str;
    if(s=='b') str="\t.byte\t";
    else if(s=='w') str="\t.short\t";
    else if(s=='l') str="\t.long\t";
    else ierror(0);
    emit(f,"%s",str);
  }else{
    emit(f,"\tdc.%c\t",s);
  }
  if(!p->tree){
    if(ISFLOAT(t)){
      /*  auch wieder nicht sehr schoen und IEEE noetig   */
      unsigned char *ip;char *s;
      ip=(unsigned char *)&p->val.vdouble;
      if(GAS) s="0x"; else s="$";
      emit(f,"%s%02x%02x%02x%02x",s,ip[0],ip[1],ip[2],ip[3]);
      if((t&NQ)!=FLOAT){
	emit(f,",%s%02x%02x%02x%02x",s,ip[4],ip[5],ip[6],ip[7]);
      }
    }else if((t&NQ)==LLONG){
      zumax tmp;
      eval_const(&p->val,t);
      tmp=vumax;
      vumax=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffffff)); 
      gval.vulong=zum2zul(vumax);
      emitval(f,&gval,UNSIGNED|LONG);
      emit(f,",");
      vumax=zumand(tmp,ul2zum(0xffffffff)); 
      gval.vulong=zum2zul(vumax);
      emitval(f,&gval,UNSIGNED|LONG);
    }else{
      emitval(f,&p->val,t&NU);
    }
  }else{
    int m,m2,m3;
    p->tree->o.am=0;
    m=p->tree->o.flags;
    p->tree->o.flags&=~VARADR;
    m2=g_flags[5];
    g_flags[5]&=~USEDFLAG;
    m3=use_sd;
    use_sd=0;
    emit_obj(f,&p->tree->o,t&NU);
    p->tree->o.flags=m;
    g_flags[5]=m2;
    use_sd=m3;
  }
  emit(f,"\n");newobj=0;
}

static void allocreg(int r)
{
  if(reg_pair(r,&rp)){
    regs[rp.r1]=1;
    regs[rp.r2]=1;
  }
  regs[r]=1;
}

static void freereg(int r)
{
  if(reg_pair(r,&rp)){
    regs[rp.r1]=0;
    regs[rp.r2]=0;
  }
  regs[r]=0;
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
  int c,t;char fp[2]="\0\0";
  int act_line=0;char *act_file=0;
  if(DEBUG&1) printf("gen_code()\n");
  for(c=1;c<=MAXR;c++) regs[c]=regsa[c];
  if(!NOPEEPHOLE){
    /*  Adressierungsarten benutzen */
    if(!addressing(p)) offset=l2zm(0L);
  }
  title(f);
  if(debug_info){
    if(HUNKDEBUG){
      if(!GAS){
	act_line=1;
	emit(f,"\tdsource\t\"%s\"\n",filename);
	emit(f,"\tdebug\t%d\n",act_line);
      }
    }
  }
  reglabel=++label;freglabel=++label;
  function_top(f,v,zm2l(offset));
  cc_set=cc_set_tst=0;
  stackoffset=notpopped=dontpop=maxpushed=0;
  for(c=1;c<=MAXR;c++){
    if(regsa[c]||regused[c]){
      BSET(regs_modified,c);
    }
  }
  for(;p;pr(f,p),p=p->next){
    if(debug_info){
      if(HUNKDEBUG){
	if(p->file&&p->file!=act_file){
	  act_file=p->file;
	  if(!GAS) emit(f,"\tdsource\t\"%s\"\n",act_file);
	}
	if(p->line&&p->line!=act_line){
	  act_line=p->line;
	  if(!GAS) emit(f,"\tdebug\t%d\n",act_line);
	}
      }else{
	dwarf2_line_info(f,p);
      }
    }

    c=p->code;t=p->typf;
    if(c==NOP) continue;
    cc_set_tst=cc_set;
    cc_typ_tst=cc_typ;
    if(cc_set_tst&&(DEBUG&512)){emit(f,"; cc_set_tst=");emit_obj(f,cc_set_tst,t);emit(f,"\n");}
    if(cc_set&&(DEBUG&512)){emit(f,"; cc_set=");emit_obj(f,cc_set,t);emit(f,"\n");}
    pushedreg&=16;if(c==RESTOREREGS) pushedreg=0;
    if(DEBUG&256){emit(f,"; "); pric2(f,p);}
    if(DEBUG&512) emit(f,"; stackoffset=%ld, notpopped=%ld, pushedreg=%d, dontpop=%ld\n",stackoffset,notpopped,pushedreg,dontpop);
    /*  muessen wir Argumente poppen?   */
    if(notpopped&&!dontpop){
      int flag=0;
      if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
	emit(f,"\tadd%s.%s\t#%ld,%s\n",quick[notpopped<=8],strshort[notpopped<32768],notpopped,mregnames[sp]);
	pop(notpopped);notpopped=0;/*cc_set_tst=cc_set=0;*/
      }
    }
    /*  na, ob das hier ok ist..?   */
    if(c==SUBPFP) c=SUB;
    if(c==PMULT) c=MULT;
    if(c==ALLOCREG){
      allocreg(p->q1.reg);
      continue;
    }
    if(c==FREEREG){
      freereg(p->q1.reg);
      continue;
    }
    if(c==LABEL){
      if(debug_info&&HUNKDEBUG) act_line=0;
      if(GAS){
	emit(f,"%s%d:\n",labprefix,t);
      }else{
	emit(f,"%s%d\n",labprefix,t);
      }
      cc_set=0;continue;
    }
    if(c==BRA){emit(f,"\t%sbra\t%s%d\n",(GAS?"j":""),labprefix,t);continue;}
    if(c>=BEQ&&c<BRA){
      if(GAS){
	if(stored_cc){emit(f,"\tjne\t%s%d\n",labprefix,t);stored_cc=0;continue;}
	if((comptyp&UNSIGNED)||(comptyp&NQ)==POINTER){
	  emit(f,"\tj%s\t%s%d\n",ubranch[c-BEQ],labprefix,t);
	}else{
	  emit(f,"\t%sj%s\t%s%d\n",fp,ename[c]+1,labprefix,t);
	}
      }else{
	if(stored_cc){emit(f,"\tbne\t%s%d\n",labprefix,t);stored_cc=0;continue;}
	if((comptyp&UNSIGNED)||(comptyp&NQ)==POINTER){
	  emit(f,"\tb%s\t%s%d\n",ubranch[c-BEQ],labprefix,t);
	}else{
	  emit(f,"\t%s%s\t%s%d\n",fp,ename[c],labprefix,t);
	}
      }
      continue;
    }
    if(c==COMPARE&&isconst(q2)&&!cf&&(t&NQ)!=LLONG){
      struct case_table *ct=calc_case_table(p,JUMP_TABLE_DENSITY);
      struct IC *p2;
      if(ct&&(ct->num>=JUMP_TABLE_LENGTH||(!isreg(q1)&&ct->num>=JUMP_TABLE_LENGTH/2))){
	int r,defl,tabl=++label,rts=0,i,ar=0;
	if(ct->next_ic->code==BRA)
	  defl=ct->next_ic->typf;
	else
	  defl=++label;
	for(r=d0;r<=d7;r++)
	  if(!regs[r]) break;
	if(r>d7){
	  if((!(p->q1.flags&REG))||p->q1.reg!=d0)
	    r=d0;
	  else
	    r=d1;
	  emit(f,"\tsubq.%s\t#4,%s\n",cf?"l":"w",mregnames[sp]);
	  emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[r],mregnames[sp]);
	  push(8);
	}else{
	  regused[r]=1;
	  BSET(regs_modified,r);
	}
	if(!regs[r]||(p->q1.flags&(REG|DREFOBJ))==DREFOBJ){
	  for(ar=1;ar<sp;ar++)
	    if(!regs[ar]) break;
	  if(ar>=sp){
	    ar=a0;
	    if(!regs[r]){
	      emit(f,"\tsubq.%s\t#4,%s\n",cf?"l":"w",mregnames[sp]);
	      push(4);
	    }
	    emit(f,"\tmove.l\t%s,-(%s)\n",mregnames[ar],mregnames[sp]);
	    push(4);
	  }else{
	    regused[ar]=1;
	    BSET(regs_modified,ar);
	  }
	}
	if(regs[r]||(ar&&regs[ar])) defl=++label;
	if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==DREFOBJ){
	  p->q1.flags&=~DREFOBJ;
	  move(f,&p->q1,0,0,ar,POINTER);
	  p->q1.flags|=(REG|DREFOBJ);
	  p->q1.flags&=~VAR;
	  p->q1.reg=ar;
	}
	if((t&NU)==(UNSIGNED|CHAR))
	  emit(f,"\tmoveq\t#0,%s\n",mregnames[r]);
	move(f,&p->q1,0,0,r,t);
	if((t&NU)==CHAR){
	  emit(f,"\text.w\t%s\n",mregnames[r]);
	  t=SHORT;
	}
	if((t&NU)==(UNSIGNED|CHAR))
	  t=(UNSIGNED|SHORT);
	if(((t&UNSIGNED)&&!zumeqto(ct->min.vumax,ul2zum(0UL)))||
	   (!(t&UNSIGNED)&&!zmeqto(ct->min.vmax,l2zm(0L)))){
	  emit(f,"\tsub.%c\t#",x_t[t&NQ]);
	  if(t&UNSIGNED)
	    emitzum(f,ct->min.vumax);
	  else
	    emitzm(f,ct->min.vmax);
	  emit(f,",%s\n",mregnames[r]);
	}
	emit(f,"\tcmp.%c\t#",x_t[t&NQ]);
	emitzum(f,ct->diff);
	emit(f,",%s\n",mregnames[r]);
	if(regs[r]||regs[ar])
	  emit(f,"\tbhi\t%s%d\n",labprefix,++label);
	else
	  emit(f,"\tbhi\t%s%d\n",labprefix,defl);
	if(CPU<68020)
	  emit(f,"\tlsl.%c\t#2,%s\n",x_t[t&NQ],mregnames[r]);
	for(i=MAXR;i>=1;i--)
	  if(regs[i]&4) rts=1;
	if(regs[r]||regs[ar]){
	  int off;
	  if(regs[r]) off=4; else off=0;
	  if(ar&&regs[ar]) off+=4;
	  if(CPU>=68020)
	    emit(f,"\tmove.l\t%s%d(%spc,%s.%c*4),%d(%s)\n",labprefix,tabl,rprefix,mregnames[r],x_t[t&NQ],off,mregnames[sp]);
	  else
	    emit(f,"\tmove.l\t%s%d(%spc,%s.%c),%d(%s)\n",labprefix,tabl,rprefix,mregnames[r],x_t[t&NQ],off,mregnames[sp]);
	  if(ar&&regs[ar]){
	    emit(f,"\tmove.l\t(%s)+,%s\n",mregnames[sp],mregnames[ar]);
	    pop(4);
	  }	      
	  if(regs[r]){
	    emit(f,"\tmove.l\t(%s)+,%s\n",mregnames[sp],mregnames[r]);
	    pop(4);
	  }
	  emit(f,"\trts\n");
	  pop(4);
	}else{
	  if(CPU>=68020)
	    emit(f,"\tmove.l\t%s%d(%spc,%s.%c*4),%s\n",labprefix,tabl,rprefix,mregnames[r],x_t[t&NQ],mregnames[ar]);
	  else
	    emit(f,"\tmove.l\t%s%d(%spc,%s.%c),%s\n",labprefix,tabl,rprefix,mregnames[r],x_t[t&NQ],mregnames[ar]);
	  emit(f,"\tjmp\t(%s)\n",mregnames[ar]);
	}
	if(GAS){
	  emit(f,"\t.align\t2\n");
	  emit(f,"%s%d:\n",labprefix,tabl);
	  emit_jump_table(f,ct,"\t.long\t","l",defl);
	}else{
	  emit(f,"\tcnop\t0,4\n");
	  emit(f,"%s%d\n",labprefix,tabl);
	  emit_jump_table(f,ct,"\tdc.l\t","l",defl);
	}
	if(ct->next_ic->code!=BRA||regs[r]||(ar&&regs[ar])){
	  if(regs[r]||(ar&&regs[ar])){
	    emit(f,"%s%d%s\n",labprefix,label,GAS?":":"");
	    if(ar&&regs[ar])
	      emit(f,"\tmove.l\t(%s)+,%s\n",mregnames[sp],mregnames[ar]);
	    if(regs[r])
	      emit(f,"\tmove.l\t(%s)+,%s\n",mregnames[sp],mregnames[r]);
	    emit(f,"\taddq.%c\t#4,%s\n",cf?'l':'w',mregnames[sp]);
	  }
	  emit(f,"%s%d%s\n",labprefix,defl,GAS?":":"");
	  p2=ct->next_ic->prev;
	}else
	  p2=ct->next_ic;
	if(p->prev) p=p->prev;
	do{
	  p=p->next;
	  if(p->code==ALLOCREG)
	    allocreg(p->q1.reg);
	  if(p->code==FREEREG)
	    freereg(p->q1.reg);
	}while(p!=p2);
	continue;
      }
    }
    if(p->q1.am){
      if(!regs[p->q1.am->basereg]){
	pric2(stdout,p);printf("%s\n",mregnames[p->q1.am->basereg]); ierror(0);
      }
      if(p->q1.am->dreg&&!regs[p->q1.am->dreg&127]){
	printf("Register %s:\n",mregnames[p->q1.am->dreg&127]);
	ierror(0);
      }
    }
    if(p->q2.am){
      if(!regs[p->q2.am->basereg]) {pric2(stdout,p);ierror(0);}
      if(p->q2.am->dreg&&!regs[p->q2.am->dreg&127]) {printf("Register %s:\n",mregnames[p->q2.am->dreg&127]);ierror(0);}
    }
    if(p->z.am){
      if(!regs[p->z.am->basereg]) {pric2(stdout,p);printf("am=%p b=%s,i=%s,o=%ld,s=%d\n",(void*)p->z.am,mregnames[p->z.am->basereg],mregnames[p->z.am->dreg&127],p->z.am->dist,p->z.am->skal);ierror(0);}
      if(p->z.am->dreg&&!regs[p->z.am->dreg&127]) {printf("Register %s:\n",mregnames[p->z.am->dreg&127]);ierror(0);}
    }
    if((p->q1.flags&REG)&&!regs[p->q1.reg]&&p->code!=MOVEFROMREG){printf("Register %s:\n",mregnames[p->q1.reg]);pric2(stdout,p);terror("illegal use of register");}
    if((p->q2.flags&REG)&&!regs[p->q2.reg]){printf("Register %s:\n",mregnames[p->q2.reg]);pric2(stdout,p);terror("illegal use of register");}
    if((p->z.flags&REG)&&!regs[p->z.reg]&&p->code!=MOVETOREG){printf("Register %s:\n",mregnames[p->z.reg]);pric2(stdout,p);terror("illegal use of register");}
    /*        if((p->q2.flags&REG)&&(p->z.flags&REG)&&p->q2.reg==p->z.reg){pric2(stdout,p);ierror(0);}*/
    /*if((p->q2.flags&VAR)&&(p->z.flags&VAR)&&p->q2.v==p->z.v&&compare_objects(&p->q2,&p->z)){pric2(stdout,p);ierror(0);}*/
    /*  COMPARE #0 durch TEST ersetzen (erlaubt, da tst alle Flags setzt)   */
    if(c==COMPARE&&isconst(q2)){
      eval_const(&p->q2.val,t);
      if(zmeqto(l2zm(0L),vmax)&&zumeqto(ul2zum(0UL),vumax)&&zldeqto(d2zld(0.0),vldouble)){
	c=p->code=TEST;p->q2.flags=0;
      }
    }
    if(c==COMPARE&&isconst(q1)){
      eval_const(&p->q1.val,t);
      if(zmeqto(l2zm(0L),vmax)&&zumeqto(ul2zum(0UL),vumax)&&zldeqto(d2zld(0.0),vldouble)){
	struct IC *bp=p->next;int bc;
	c=p->code=TEST;p->q1=p->q2;p->q2.flags=0;p->q2.am=0;
	/*  Nachfolgenden Branch umdrehen   */
	while(bp&&bp->code==FREEREG) bp=bp->next;
	bc=bp->code;
	if(!bp||bc<BEQ||bc>BGT) ierror(0);
	if(bc==BLT) bp->code=BGT;
	if(bc==BGT) bp->code=BLT;
	if(bc==BLE) bp->code=BGE;
	if(bc==BGE) bp->code=BLE;
      }
    }
    /*  gesetzte ConditionCodes merken  */
    if(p->z.flags&&(!isreg(z)||p->z.reg>=d0)&&(c!=CONVERT||!ISFLOAT(p->typf2))&&((!ISFLOAT(t))||FPU>68000)){
      cc_set=&p->z;cc_typ=p->typf;
    }else{
      cc_set=0;
    }
    if(c==LEA){
      if(!isreg(z)||p->z.reg>8) ierror(0);
      emit(f,"\tlea\t");emit_obj(f,&p->q1,t);
      emit(f,",%s\n",mregnames[p->z.reg]);
      continue;
    }
    if(c==PEA){
      emit(f,"\tpea\t");emit_obj(f,&p->q1,t);emit(f,"\n");
      push(zm2l(p->q2.val.vmax));
      dontpop+=zm2l(p->q2.val.vmax);
      continue;
    }
    if(c==MOVEFROMREG){
      if(p->q1.reg<fp0)
	emit(f,"\tmove.l\t%s,",mregnames[p->q1.reg]);
      else if(p->q1.reg<25)
	emit(f,"\tfmove.x\t%s,",mregnames[p->q1.reg]);
      else
	emit(f,"\tmovem.l\t%s,",mregnames[p->q1.reg]);
      emit_obj(f,&p->z,t);emit(f,"\n");
      continue;
    }
    if(c==MOVETOREG){
      if(p->z.reg<fp0)
	emit(f,"\tmove.l\t");
      else if(p->z.reg<25)
	emit(f,"\tfmove.x\t");
      else
	emit(f,"\tmovem.l\t");
      emit_obj(f,&p->q1,t);emit(f,",%s\n",mregnames[p->z.reg]);
      continue;
    }
    if(NOPEEPHOLE){
      if(p->q1.am||p->q2.am||p->z.am){
	ierror(0);
	p->q1.am=p->q2.am=p->z.am=0;
      }
    }
    p=do_refs(f,p);
    if((p->q1.flags&&(q1typ(p)&NQ)==LLONG)||(p->q2.flags&&(q2typ(p)&NQ)==LLONG)||(p->z.flags&&(ztyp(p)&NQ)==LLONG)){
      if(handle_llong(f,p)){
	*fp=0;
	continue;
      }
    }
    if(NOPEEPHOLE){
      if(p->q1.am||p->q2.am||p->z.am){
	ierror(0);
	p->q1.am=p->q2.am=p->z.am=0;
      }
    }
#ifdef M68K_16BIT_INT
    if(c==CONVERT&&((t&NQ)==LONG||(t&NQ)==POINTER)&&((p->typf2&NQ)==LONG||(p->typf2&NQ)==POINTER)){
      p->code=c=ASSIGN;
      p->q2.val.vmax=sizetab[LONG];
    }                 
#else
    if(c==CONVERT&&((t&NQ)==LONG||(t&NQ)==INT||(t&NQ)==POINTER)&&((p->typf2&NQ)==LONG||(p->typf2&NQ)==INT||(p->typf2&NQ)==POINTER)){
      p->code=c=ASSIGN;
      p->q2.val.vmax=sizetab[LONG];
    }                 
#endif
    if(c==CONVERT){
      int to;
      if((t&NQ)==POINTER) t=(UNSIGNED|LONG);
      if((t&NQ)==LDOUBLE) t=DOUBLE;
      to=p->typf2&NU;
      if(to==POINTER) to=UNSIGNED|LONG;
      if(to==LDOUBLE) to=DOUBLE;
#ifdef M68K_16BIT_INT
      if((to==(UNSIGNED|CHAR)||to==(UNSIGNED|SHORT)||to==(UNSIGNED|INT))&&!(t&UNSIGNED)&&(t&NQ)>(to&NQ))
	cc_set=0;
#else
      if((to==(UNSIGNED|CHAR)||to==(UNSIGNED|SHORT))&&!(t&UNSIGNED)&&(t&NQ)>(to&NQ))
	cc_set=0;
#endif
      if(ISFLOAT(t)||ISFLOAT(to)){
	if(FPU>68000){
	  int zreg=0;
	  if(ISFLOAT(t)&&ISFLOAT(to)){
	    if(isreg(q1)&&isreg(z)){
	      if(p->q1.reg!=p->z.reg)
		emit(f,"\tfmove.x\t%s,%s\n",mregnames[p->q1.reg],mregnames[p->z.reg]);
	      continue;
	    }
	  }
	  if(isreg(z)&&p->z.reg>=fp0)
	    zreg=p->z.reg;
	  if(isreg(q1)&&p->q1.reg>=fp0){
	    if(!zreg&&(t&UNSIGNED)&&!ISHWORD(t))
	      zreg=p->q1.reg; 
	    else 
	      zreg=get_reg(f,2,p);}
	  if(!zreg) zreg=get_reg(f,2,p);
	  if((to&UNSIGNED)&&x_t[to&NQ]!='l'){
	    int dreg=get_reg(f,1,p);
	    emit(f,"\tmoveq\t#0,%s\n",mregnames[dreg]);
	    move(f,&p->q1,0,0,dreg,to);
	    move(f,0,dreg,0,zreg,LONG);
	  }else{
	    if(!isreg(q1)||p->q1.reg!=zreg)
	      move(f,&p->q1,0,0,zreg,to);
	  }
	  if(!ISFLOAT(t)){
	    if((t&UNSIGNED)&&!ISHWORD(t)){
	      char *s;
	      int dreg1,dreg2;
	      int l1=++label,l2=++label;
	      if(GAS) s="0x"; else s="$";
	      if(isreg(z)) 
		dreg1=p->z.reg;
	      else
		dreg1=get_reg(f,1,p);
	      if(FPU==68040)
		dreg2=get_reg(f,1,p);
	      emit(f,"\tfcmp.d\t#%s41e0000000000000,%s\n",s,mregnames[zreg]);
	      emit(f,"\tfbge\t%s%d\n",labprefix,l1);
	      if(FPU==68040){
		emit(f,"\tfmove.l\t%sfpcr,%s\n",rprefix,mregnames[dreg2]);
		emit(f,"\tmoveq\t#16,%s\n",mregnames[dreg1]);
		emit(f,"\tor.l\t%s,%s\n",mregnames[dreg2],mregnames[dreg1]);
		emit(f,"\tand.w\t#-33,%s\n",mregnames[dreg1]);
		emit(f,"\tfmove.l\t%s,%sfpcr\n",mregnames[dreg1],rprefix);
	      }else{
		emit(f,"\tfintrz\t%s\n",mregnames[zreg]);
	      }
	      emit(f,"\tfmove.l\t%s,%s\n",mregnames[zreg],mregnames[dreg1]);
	      emit(f,"\tbra\t%s%d\n",labprefix,l2);
	      emit(f,"%s%d:\n",labprefix,l1);
	      emit(f,"\tfsub.d\t#%s41e0000000000000,%s\n",s,mregnames[zreg]);
	      if(FPU==68040){
		emit(f,"\tfmove.l\t%sfpcr,%s\n",rprefix,mregnames[dreg2]);
		emit(f,"\tmoveq\t#16,%s\n",mregnames[dreg1]);
		emit(f,"\tor.l\t%s,%s\n",mregnames[dreg2],mregnames[dreg1]);
		emit(f,"\tand.w\t#-33,%s\n",mregnames[dreg1]);
		emit(f,"\tfmove.l\t%s,%sfpcr\n",mregnames[dreg1],rprefix);
	      }else{
		emit(f,"\tfintrz\t%s\n",mregnames[zreg]);
	      }
	      emit(f,"\tfmove.l\t%s,%s\n",mregnames[zreg],mregnames[dreg1]);
	      emit(f,"\tbchg\t#31,%s\n",mregnames[dreg1]);
	      emit(f,"%s%d:\n",labprefix,l2);
	      if(FPU==68040)
		emit(f,"\tfmove.l\t%s,%sfpcr\n",mregnames[dreg2],rprefix);
	      move(f,0,dreg1,&p->z,0,t);
	      continue;
	    }	      
	    /*  nach integer, d.h. Kommastellen abschneiden */
	    if(FPU==68040/*||FPU==68060*/){
	      /*  bei 040 emuliert    */
	      int dreg1,dreg2;
	      if(!NOINTZ){
		if(isreg(z))
		  dreg1=p->z.reg;
		else
		  dreg1=get_reg(f,1,p);
		dreg2=get_reg(f,1,p);
		emit(f,"\tfmove.l\t%sfpcr,%s\n",rprefix,mregnames[dreg2]);
		emit(f,"\tmoveq\t#16,%s\n",mregnames[dreg1]);
		emit(f,"\tor.l\t%s,%s\n",mregnames[dreg2],mregnames[dreg1]);
		emit(f,"\tand.w\t#-33,%s\n",mregnames[dreg1]);
		emit(f,"\tfmove.l\t%s,%sfpcr\n",mregnames[dreg1],rprefix);
	      }else{
		dreg1=get_reg(f,1,p);
	      }
	      if((t&UNSIGNED)&&ISHWORD(t)){
		emit(f,"\tfmove.l\t%s,%s\n",mregnames[zreg],mregnames[dreg1]);
		emit(f,"\tmove.%c\t%s,",x_t[t&NQ],mregnames[dreg1]);
	      }else{
		emit(f,"\tfmove.%c\t%s,",x_t[t&NQ],mregnames[zreg]);
	      }
	      emit_obj(f,&p->z,t);emit(f,"\n");
	      if(!NOINTZ)
		emit(f,"\tfmove.l\t%s,%sfpcr\n",mregnames[dreg2],rprefix);
	      continue;
	    }else{
	      if(!NOINTZ){
		if(!isreg(q1)||p->q1.reg!=zreg){
		  emit(f,"\tfintrz.x\t%s\n",mregnames[zreg]);
		}else{
		  int nreg=get_reg(f,2,p);
		  emit(f,"\tfintrz.x\t%s,%s\n",mregnames[zreg],mregnames[nreg]);
		  zreg=nreg;
		}
	      }
	      if((t&UNSIGNED)&&ISHWORD(t)){
		int r;
		if(isreg(z)) r=p->z.reg; else r=get_reg(f,1,p);
		move(f,0,zreg,0,r,LONG);
		move(f,0,r,&p->z,0,t);
	      }else{
		move(f,0,zreg,&p->z,0,t);
	      }
	      continue;
	    }
	  }
	  if((to&UNSIGNED)&&x_t[to&NQ]=='l'){
	    int nlabel;
	    emit(f,"\ttst.%c\t",x_t[to&NQ]);
	    emit_obj(f,&p->q1,to);emit(f,"\n");
	    nlabel=++label;
	    emit(f,"\tbge.s\t%s%d\n",labprefix,nlabel);
	    emit(f,"\tfadd.d\t#4294967296,%s\n",mregnames[zreg]);
	    emit(f,"%s%d:\n",labprefix,nlabel);
	  }
	  if(!(p->z.reg)||p->z.reg!=zreg){
	    move(f,0,zreg,&p->z,0,t);
	  }
	}else{
	  cc_set=0;
	  if((to&NQ)==(t&NQ)){
	    assign(f,p,&p->q1,&p->z,ASSIGN,sizetab[to&NQ],t);
	    continue;
	  }
	  if((to&NQ)==FLOAT&&((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)){
	    saveregs(f,p);
	    assign(f,p,&p->q1,0,PUSH,msizetab[FLOAT],FLOAT);
	    scratch_modified();
	    if(GAS){
	      emit(f,"\t.global\t__ieees2d\n\tjbsr\t__ieees2d\n\taddq.%s\t#4,%s\n",strshort[1],mregnames[sp]);
	    }else{
	      emit(f,"\tpublic\t__ieees2d\n\tjsr\t__ieees2d\n\taddq.%s\t#4,%s\n",strshort[1],mregnames[sp]);
	    }
	    pop(4);
	    restoreregsa(f,p);
	    stored0d1(f,&p->z,t);
	    restoreregsd(f,p);
	    continue;
	  }
	  if(((to&NQ)==DOUBLE||(to&NQ)==LDOUBLE)&&(t&NQ)==FLOAT){
	    saveregs(f,p);
	    assign(f,p,&p->q1,0,PUSH,msizetab[DOUBLE],DOUBLE);
	    scratch_modified();
	    if(GAS){
	      emit(f,"\t.global\t__ieeed2s\n\tjbsr\t__ieeed2s\n\taddq.%s\t#8,%s\n",strshort[1],mregnames[sp]);
	    }else{
	      emit(f,"\tpublic\t__ieeed2s\n\tjsr\t__ieeed2s\n\taddq.%s\t#8,%s\n",strshort[1],mregnames[sp]);
	    }
	    pop(8);
	    restoreregsa(f,p);
	    move(f,0,d0,&p->z,0,t);
	    restoreregsd(f,p);
	    continue;
	  }
	  if(ISFLOAT(to)){
	    int uns;
	    saveregs(f,p);
	    if(t&UNSIGNED) uns='u'; else uns='s';
	    assign(f,p,&p->q1,0,PUSH,sizetab[to&NQ],to);
	    scratch_modified();
	    if(GAS){
	      emit(f,"\t.global\t__ieeefix%c%c\n\tjbsr\t__ieeefix%c%c\n\taddq.%s\t#%ld,%s\n",x_t[to&NQ],uns,x_t[to&NQ],uns,strshort[1],zm2l(sizetab[to&NQ]),mregnames[sp]);
	    }else{
	      emit(f,"\tpublic\t__ieeefix%c%c\n\tjsr\t__ieeefix%c%c\n\taddq.%s\t#%ld,%s\n",x_t[to&NQ],uns,x_t[to&NQ],uns,strshort[1],zm2l(sizetab[to&NQ]),mregnames[sp]);
	    }
	    pop(sizetab[to&NQ]);
	    restoreregsa(f,p);
	    move(f,0,d0,&p->z,0,t);
	    restoreregsd(f,p);
	    continue;
	  }else{
	    int uns,xt=x_t[to&NQ];
	    saveregs(f,p);
	    if(to&UNSIGNED) uns='u'; else uns='s';
	    if(xt!='l'){
	      emit(f,"\tsubq.%s\t#4,%s\n",strshort[1],mregnames[sp]);
	      push(4);
	    }
	    emit(f,"\tmove.%c\t",xt);
	    emit_obj(f,&p->q1,to);
	    if(xt!='l')
	      emit(f,",(%s)\n",mregnames[sp]); 
	    else{
	      emit(f,",-(%s)\n",mregnames[sp]);
	      push(4);
	    }
	    scratch_modified();
	    if(GAS){
	      emit(f,"\t.global\t__ieeeflt%c%c%c\n\tjbsr\t__ieeeflt%c%c%c\n\taddq.%s\t#4,%s\n",uns,xt,x_t[t&NQ],uns,xt,x_t[t&NQ],strshort[1],mregnames[sp]);
	    }else{
	      emit(f,"\tpublic\t__ieeeflt%c%c%c\n\tjsr\t__ieeeflt%c%c%c\n\taddq.%s\t#4,%s\n",uns,xt,x_t[t&NQ],uns,xt,x_t[t&NQ],strshort[1],mregnames[sp]);
	    }
	    pop(4);
	    restoreregsa(f,p);
	    if((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)
	      stored0d1(f,&p->z,t);
	    else
	      move(f,0,d0,&p->z,0,t);
	    restoreregsd(f,p);
	    continue;
	  }
	}
	continue;
      }
      if((to&NQ)<(t&NQ)){
	int zreg;
	if(isreg(z)&&p->z.reg>=d0&&p->z.reg<=d7)
	  zreg=p->z.reg; else zreg=get_reg(f,1,p);
	if(sizetab[t&NQ]!=sizetab[LONG]||sizetab[to&NQ]!=sizetab[LONG]){
	  /*  aufpassen, falls unsigned und Quelle==Ziel  */
	  if((to&UNSIGNED)&&isreg(q1)&&zreg==p->q1.reg){
	    unsigned long l;
	    if((to&NQ)==CHAR) l=0xff; else l=0xffff;
	    emit(f,"\tand.%c\t#%lu,%s\n",cf?'l':x_t[t&NQ],l,mregnames[zreg]);
	    continue;
	  }
	  if(to&UNSIGNED) emit(f,"\tmoveq\t#0,%s\n",mregnames[zreg]);
	  move(f,&p->q1,0,0,zreg,to);
	  if(!(to&UNSIGNED)){
#ifdef M68K_16BIT_INT
	    if((to&NQ)==CHAR&&((to&NQ)==SHORT||(t&NQ)==INT)) emit(f,"\text.w\t%s\n",mregnames[zreg]);
	    if(((to&NQ)==SHORT||(to&NQ)==INT)&&msizetab[t&NQ]==4) emit(f,"\text.l\t%s\n",mregnames[zreg]);
#else
	    if((to&NQ)==CHAR&&(t&NQ)==SHORT) emit(f,"\text.w\t%s\n",mregnames[zreg]);
	    if((to&NQ)==SHORT&&msizetab[t&NQ]==4) emit(f,"\text.l\t%s\n",mregnames[zreg]);
#endif
	    if((to&NQ)==CHAR&&msizetab[t&NQ]==4){
	      if(cf||CPU>=68020)
		emit(f,"\textb.l\t%s\n",mregnames[zreg]);
	      else
		emit(f,"\text.w\t%s\n\text.l\t%s\n",mregnames[zreg],mregnames[zreg]);
	    }
	  }
	}
	if(!isreg(z)||p->z.reg!=zreg){
	  move(f,0,zreg,&p->z,0,t);
	}
      }else{
	long diff;int m;
	m=0;
	if(p->q1.flags&REG){
	  p->q1.val.vmax=l2zm(0L);
	  p->q1.flags|=D16OFF;m=1;
	}
	diff=msizetab[to&NQ]-msizetab[t&NQ];
	vmax=l2zm(diff);
	p->q1.val.vmax=zmadd(p->q1.val.vmax,vmax);
	move(f,&p->q1,0,&p->z,0,t);
	vmax=l2zm(diff);
	p->q1.val.vmax=zmsub(p->q1.val.vmax,vmax);
	if(m) p->q1.flags&=~D16OFF;
	if(compare_objects(&p->q1,&p->z))
	  cc_set=0;
      }
      continue;
    }
    if(ISFLOAT(t)&&FPU>68000) *fp='f'; else *fp=0;
    if(c==MINUS||c==KOMPLEMENT){
      int zreg;
      if(ISFLOAT(t)){
	if(FPU>68000){
	  if(isreg(z)) zreg=p->z.reg; else zreg=get_reg(f,2,p);
	  emit(f,"\tfneg.");
	  if(isreg(q1)) emit(f,"x\t%s",mregnames[p->q1.reg]);
	  else    {emit(f,"%c\t",x_t[t&NQ]);emit_obj(f,&p->q1,t);}
	  emit(f,",%s\n",mregnames[zreg]);
	  if(!isreg(z)||p->z.reg!=zreg){
	    move(f,0,zreg,&p->z,0,t);
	  }
	  continue;
	}else{
	  saveregs(f,p);
	  assign(f,p,&p->q1,0,PUSH,msizetab[t&NQ],t);
	  scratch_modified();
	  if(GAS){
	    emit(f,"\t.global\t__ieeeneg%c\n\tjbsr\t__ieeeneg%c\n\taddq.%s\t#%ld,%s\n",x_t[t&NQ],x_t[t&NQ],strshort[1],msizetab[t&NQ],mregnames[sp]);
	  }else{
	    emit(f,"\tpublic\t__ieeeneg%c\n\tjsr\t__ieeeneg%c\n\taddq.%s\t#%ld,%s\n",x_t[t&NQ],x_t[t&NQ],strshort[1],msizetab[t&NQ],mregnames[sp]);
	  }
	  pop(msizetab[t&NQ]);
	  restoreregsa(f,p);
	  if((t&NQ)!=FLOAT)
	    stored0d1(f,&p->z,t);
	  else
	    move(f,0,d0,&p->z,0,t);
	  restoreregsd(f,p);
	  continue;
	}
      }
      if(!cf&&compare_objects(&p->q1,&p->z)){
	emit(f,"\t%s.%c\t",ename[c],x_t[t&NQ]);
	emit_obj(f,&p->q1,t);emit(f,"\n");
	continue;
      }
      if(isreg(z)&&p->z.reg>=d0/*&&p->z.reg<=d7*/)
	zreg=p->z.reg; else zreg=get_reg(f,1,p);
      if(!isreg(q1)||p->q1.reg!=zreg){
	move(f,&p->q1,0,0,zreg,t);
      }
      emit(f,"\t%s.%c\t%s\n",ename[c],x_t[t&NQ],mregnames[zreg]);
      if(!isreg(z)||p->z.reg!=zreg){
	move(f,0,zreg,&p->z,0,t);
      }
      continue;
    }
    if(c==SETRETURN){
      /*  Returnwert setzen - q2.val.vmax==size, z.reg==Returnregister */
      if(((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)&&p->z.reg==d0){
	BSET(regs_modified,d0);BSET(regs_modified,10);
	if(isconst(q1)){
	  unsigned char *ip=(unsigned char *)&p->q1.val.vdouble;
	  char *s;
	  if(GAS) s="0x"; else s="$";
	  emit(f,"\tmove.l\t#%s%02x%02x%02x%02x,%s\n",s,ip[0],ip[1],ip[2],ip[3],mregnames[d0]);
	  emit(f,"\tmove.l\t#%s%02x%02x%02x%02x,%s\n",s,ip[4],ip[5],ip[6],ip[7],mregnames[d1]);
	  continue;
	}
	if(isreg(q1)&&p->q1.reg>=fp0&&p->q1.reg<=fp7){
	  emit(f,"\tfmove.d\t%s,-(%s)\n\tmovem.l\t(%s)+,%s\n",mregnames[p->q1.reg],mregnames[sp],mregnames[sp],mregnames[d0d1]);
	}else{
	  loadd0d1(f,&p->q1,t);
	}
	continue;
      }
      if((ISSTRUCT(t)||ISUNION(t))&&p->z.reg==d0){
	long l=zm2l(p->q2.val.vmax);
	emit(f,"\tmovem.l\t");emit_obj(f,&p->q1,t);
	emit(f,",%s",mregnames[d0]);BSET(regs_modified,d0);
	if(l>=8) {emit(f,"/%s",mregnames[d1]);BSET(regs_modified,d1);}
	if(l>=12) {emit(f,"/%s",mregnames[a0]);BSET(regs_modified,a0);}
	if(l>=16) {emit(f,"/%s",mregnames[a1]);BSET(regs_modified,a1);}
	emit(f,"\n");
	continue;
      }
      /*  Wenn Returnwert ueber Zeiger gesetzt wird, nichts noetig    */
      if(p->z.reg){
	move(f,&p->q1,0,0,p->z.reg,p->typf);
	BSET(regs_modified,p->z.reg);
	if(v->tattr&AMIINTERRUPT){
	  /* if necessary, set condition-codes */
	  if(p->z.reg!=d0) ierror(0);
	  if(isreg(q1)&&p->q1.reg==d0)
	    emit(f,"\ttst.%c\t%s\n",x_t[t&NQ],mregnames[d0]);
	}
      }
      continue;
    }
    if(c==GETRETURN){
      /*  Returnwert holen - q2.val.vmax==size, q1.reg==Returnregister     */
      if(((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)&&p->q1.reg==d0){
	if(isreg(z)&&p->z.reg>=fp0&&p->z.reg<=fp7){
	  emit(f,"\tmovem.l\t%s,-(%s)\n\tfmove.d\t(%s)+,%s\n",mregnames[d0d1],mregnames[sp],mregnames[sp],mregnames[p->z.reg]);
	}else{
	  stored0d1(f,&p->z,t);
	}
	continue;
      }
      if((ISSTRUCT(t)||ISUNION(t))&&p->q1.reg==d0){
	long l=zm2l(p->q2.val.vmax);
	emit(f,"\tmovem.l\t");
	emit(f,"%s",mregnames[d0]);
	if(l>=8) emit(f,"/%s",mregnames[d1]);
	if(l>=12) emit(f,"/%s",mregnames[a0]);
	if(l>=16) emit(f,"/%s",mregnames[a1]);
	emit(f,",");emit_obj(f,&p->z,t);emit(f,"\n");
	continue;
      }

      /*  Wenn Returnwert ueber Zeiger gesetzt wird, nichts noetig    */
      cc_set=0;
      if(p->q1.reg){
	move(f,0,p->q1.reg,&p->z,0,p->typf);
	if(!(p->z.flags&REG)||(p->z.reg!=p->q1.reg&&p->z.reg>=d0)){ cc_set=&p->z;cc_typ=p->typf;}
      }
      continue;
    }
    if(c==CALL){
      if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
	emit_inline_asm(f,p->q1.v->fi->inline_asm);
	if(stack_valid&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_STACK))
	  callee_push(zum2ul(p->q1.v->fi->stack1));
	else
	  stack_valid=0;
      }else{
	if(GAS){
	  emit(f,"\tjbsr\t");
	}else{
	  emit(f,"\tjsr\t");
	}
	/*  Wenn geta4() aufgerufen wurde, merken.  */
	if(use_sd&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp(p->q1.v->identifier,"geta4")&&p->q1.v->storage_class==EXTERN)
	  geta4=1;
	if((p->q1.flags&(DREFOBJ|REG|KONST))==DREFOBJ) ierror(0);
	emit_obj(f,&p->q1,t);
	emit(f,"\n");
	push(4);
	if(stack_valid&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_STACK))
	  callee_push(zum2ul(p->q1.v->fi->stack1));
	else
	  stack_valid=0;
	pop(4);
      }
      if(debug_info&&HUNKDEBUG) act_line=0;
      if(!zmeqto(p->q2.val.vmax,l2zm(0L))){
	notpopped+=zm2l(p->q2.val.vmax);
	dontpop-=zm2l(p->q2.val.vmax);
	if(!NODELAYEDPOP&&!(pushedreg&30)&&!vlas&&stackoffset==-notpopped){
	  /*  Entfernen der Parameter verzoegern  */
	}else{
	  if(debug_info&&HUNKDEBUG&&!GAS){ act_line=p->line; emit(f,"\tdebug\t%d\n",act_line);}
	  emit(f,"\tadd%s.%s\t#%ld,%s\n",quick[zm2l(p->q2.val.vmax)<=8],strshort[zm2l(p->q2.val.vmax)<32768],zm2l(p->q2.val.vmax),mregnames[sp]);
	  pop(zm2l(p->q2.val.vmax));
	  notpopped-=zm2l(p->q2.val.vmax);
	}
      }
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)){
	bvunite(regs_modified,p->q1.v->fi->regs_modified,RSIZE);
      }else{
	int i;
	for(i=1;i<=MAXR;i++){
	  if(regscratch[i])
	    if(i<fp0||i>fp7||FPU>68000) 
	      BSET(regs_modified,i);
	}
      }
      continue;
    }
    if(c==TEST){
      /*  ConditionCodes schon gesetzt?   */
      cc_set=&p->q1;cc_typ=t;
      comptyp=t;
      if(cc_set_tst&&t==cc_typ_tst){
	struct IC *branch;
	if(t&UNSIGNED){
	  branch=p->next;
	  while(branch&&(branch->code<BEQ||branch->code>BGT))
	    branch=branch->next;
	  if(!branch) continue;
	  if(branch->code==BLE) branch->code=BEQ;
	  if(branch->code==BGT) branch->code=BNE;
	  if(branch->code==BGE) {branch->code=BRA;continue;}
	  if(branch->code==BLT) {branch->code=NOP;continue;}
	}
	if(compare_objects(&p->q1,cc_set_tst)&&p->q1.am==cc_set_tst->am&&zmeqto(p->q1.val.vmax,cc_set_tst->val.vmax)){
	  if(DEBUG&512){emit(f,"; tst eliminated: cc=");emit_obj(f,cc_set_tst,t);
	  emit(f,", q1=");emit_obj(f,&p->q1,t);emit(f,"\n");}
	  continue;
	}
      }
      if(CPU<68020&&isreg(q1)&&p->q1.reg>=1&&p->q1.reg<=8){
	/*  tst ax gibt es nicht bei <68000 :-( */
	if(regavailable(1)){
	  emit(f,"\tmove.%c\t%s,%s\n",x_t[t&NQ],mregnames[p->q1.reg],mregnames[get_reg(f,1,p)]);
	}else{
	  emit(f,"\tcmp.%c\t#0,%s\n",cf?'l':'w',mregnames[p->q1.reg]);
	}
	continue;
      }
      if(ISFLOAT(t)&&FPU<=68000){
	/*  nicht sehr schoen   */
	int result=get_reg(f,1,p);
	saveregs(f,p);
	assign(f,p,&p->q1,0,PUSH,msizetab[t&NQ],t);
	scratch_modified();
	if(GAS){
	  emit(f,"\t.global\t__ieeetst%c\n\tjbsr\t__ieeetst%c\n\taddq.%s\t#%ld,%s\n",x_t[t&NQ],x_t[t&NQ],strshort[1],msizetab[t&NQ],mregnames[sp]);
	}else{
	  emit(f,"\tpublic\t__ieeetst%c\n\tjsr\t__ieeetst%c\n\taddq.%s\t#%ld,%s\n",x_t[t&NQ],x_t[t&NQ],strshort[1],msizetab[t&NQ],mregnames[sp]);
	}
	pop(msizetab[t&NQ]);
	restoreregsa(f,p);
	if(result!=d0) emit(f,"\tmove.l\t%s,%s\n",mregnames[d0],mregnames[result]);
	emit(f,"\ttst.l\t%s\n",mregnames[result]);
	restoreregsd(f,p);
	continue;
      }
      if(isreg(q1)&&p->q1.reg>=fp0&&p->q1.reg<=fp7){
	emit(f,"\tftst.x\t%s\n",mregnames[p->q1.reg]);
      }else if(p->q1.flags&(VARADR|KONST)){
	int r=get_reg(f,1,p);
	emit(f,"\tmove.%c\t",x_t[t&NQ]);
	emit_obj(f,&p->q1,t);
	emit(f,",%s\n",mregnames[r]);
      }else{
	emit(f,"\t%stst.%c\t",fp,x_t[t&NQ]);emit_obj(f,&p->q1,t);
	emit(f,"\n");
      }
      continue;
    }
    if(c==ASSIGN||c==PUSH){
      if(c==ASSIGN&&compare_objects(&p->q1,&p->z)) cc_set=0;
      if(c==PUSH) dontpop+=zm2l(p->q2.val.vmax);
      assign(f,p,&p->q1,&p->z,c,zm2l(p->q2.val.vmax),t);
      continue;
    }
    if(c==ADDRESS){
      int zreg;
      if(isreg(z)&&p->z.reg>=1&&p->z.reg<=8)
	zreg=p->z.reg; else zreg=get_reg(f,0,p);
      emit(f,"\tlea\t");emit_obj(f,&p->q1,t);
      emit(f,",%s\n",mregnames[zreg]);
      if(!isreg(z)||p->z.reg!=zreg){
	move(f,0,zreg,&p->z,0,POINTER);
      }
      continue;
    }
    if(c==COMPARE){
      int zreg;
      comptyp=t;
      if(isconst(q1)||isreg(q2)){
	/*  evtl. Argumente von cmp und nachfolgendes bcc umdrehen  */
	struct IC *n;struct obj m;
	n=p->next;
	while(n){
	  if(n->code>=BEQ&&n->code<BRA){
	    if(!p->z.flags){
	      if(DEBUG&1) printf("arguments of cmp exchanged\n");
	      m=p->q1;p->q1=p->q2;p->q2=m;
	      p->z.flags=1;
	    }
	    /*  nachfolgenden Branch umdrehen   */
	    switch(n->code){
	    case BGT: n->code=BLT;break;
	    case BLT: n->code=BGT;break;
	    case BGE: n->code=BLE;break;
	    case BLE: n->code=BGE;break;
	    }
	    break;
	  }
	  if(n->code==FREEREG) n=n->next; else break; /*  compare ohne branch => leerer Block o.ae.   */
	}
      }
      if(ISFLOAT(t)){
	if(FPU>68000){
	  if(isreg(q1)&&p->q1.reg>=fp0){
	    zreg=p->q1.reg;
	  }else{
	    zreg=get_reg(f,2,p);
	    move(f,&p->q1,0,0,zreg,t);
	  }
	  if(isreg(q2)){emit(f,"\tfcmp.x\t%s,%s\n",mregnames[p->q2.reg],mregnames[zreg]);continue;}
	  emit(f,"\tfcmp.%c\t",x_t[t&NQ]);emit_obj(f,&p->q2,t);
	  emit(f,",%s\n",mregnames[zreg]);
	  continue;
	}else{
	  /*  nicht sehr schoen   */
	  int result=get_reg(f,1,p);
	  saveregs(f,p);
	  assign(f,p,&p->q2,0,PUSH,msizetab[t&NQ],t);
	  assign(f,p,&p->q1,0,PUSH,msizetab[t&NQ],t);
	  scratch_modified();
	  if(GAS){
	    emit(f,"\t.global\t__ieeecmp%c\n\tjbsr\t__ieeecmp%c\n\tadd.%s\t#%ld,%s\n",x_t[t&NQ],x_t[t&NQ],strshort[1],2*msizetab[t&NQ],mregnames[sp]);
	  }else{
	    emit(f,"\tpublic\t__ieeecmp%c\n\tjsr\t__ieeecmp%c\n\tadd.%s\t#%ld,%s\n",x_t[t&NQ],x_t[t&NQ],strshort[1],2*msizetab[t&NQ],mregnames[sp]);
	  }
	  pop(2*msizetab[t&NQ]);
	  restoreregsa(f,p);
	  if(result!=d0) emit(f,"\tmove.l\t%s,%s\n",mregnames[d0],mregnames[result]);
	  emit(f,"\ttst.l\t%s\n",mregnames[result]);
	  restoreregsd(f,p);
	  continue;
	}
      }
      if(cf&&x_t[t&NQ]!='l'){
	if(isreg(q1)) zreg=p->q1.reg; else zreg=get_reg(f,1,p);
	loadext(f,zreg,&p->q1,t);
	if(isconst(q2)){
	  emit(f,"\tcmp.l\t");
	  emit_obj(f,&p->q2,t);
	}else{
	  int r;
	  if(isreg(q2)) r=p->q2.reg; else r=get_reg(f,1,p);
	  loadext(f,r,&p->q2,t);
	  emit(f,"\tcmp.l\t%s",mregnames[r]);
	}
	emit(f,",%s\n",mregnames[zreg]);
	continue;
      }
      if((p->q2.flags&(KONST|VARADR))&&!(p->q1.flags&(KONST|VARADR))&&(!cf||isreg(q1))){
	emit(f,"\tcmp.%c\t",x_t[t&NQ]);emit_obj(f,&p->q2,t);
	emit(f,",");emit_obj(f,&p->q1,t);emit(f,"\n");
	continue;
      }
      if(isreg(q1)){
	zreg=p->q1.reg;
      }else{
	zreg=get_reg(f,1,p);    /* hier evtl. auch Adressregister nehmen */
	move(f,&p->q1,0,0,zreg,t);
      }
      emit(f,"\tcmp.%c\t",x_t[t&NQ]);emit_obj(f,&p->q2,t);
      emit(f,",%s\n",mregnames[zreg]);
      continue;
    }
    if(c==ADDI2P||c==SUBIFP){
      int zreg=-1,r;

      /*  hier die zweite Alternative mit isreg() schreiben?  */
      if((((p->q2.flags&REG)&&(p->z.flags&REG)&&p->q2.reg==p->z.reg&&(!(p->q1.flags&REG)||p->q1.reg!=p->z.reg))||
	 (compare_objects(&p->q2,&p->z)&&!compare_objects(&p->q1,&p->z)))){
	struct obj m;
	if(c==ADDI2P&&x_t[t&NQ]=='l'){
	  m=p->q1;p->q1=p->q2;p->q2=m;
	}else{
	  if(!cf&&x_t[t&NQ]=='l'&&isreg(q2)&&p->q2.reg>=d0&&p->q2.reg<=d7){
	    m=p->q1;p->q1=p->q2;p->q2=m;
	    c=ADDI2P;
	    emit(f,"\tneg.%c\t",x_t[t&NQ]);
	    emit_obj(f,&p->q1,t);emit(f,"\n");
	  }else{
	    zreg=get_reg(f,0,p);
	  }
	}
      }

      if(isreg(q1)&&p->q1.reg<=8&&isreg(z)&&p->z.reg<=8&&p->q1.reg!=p->z.reg){
	/*  q1 und z Adressregister => lea nehmen   */
	if(isconst(q2)){
	  eval_const(&p->q2.val,t);
	  if(c==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	  if(CPU>=68020||(zmleq(vmax,l2zm(32767))&&zmleq(l2zm(-32768),vmax))){
	    emit(f,"\tlea\t(%ld",zm2l(vmax));
	    if(!GAS&&zm2l(vmax)>0x7fff)
	      emit(f,".l");
	    emit(f,",%s),%s\n",mregnames[p->q1.reg],mregnames[p->z.reg]);
	    continue;
	  }
	}else if(c==ADDI2P&&isreg(q2)){
	  emit(f,"\tlea\t(%s,%s.%c),%s\n",mregnames[p->q1.reg],mregnames[p->q2.reg],x_t[t&NQ],mregnames[p->z.reg]);
	  continue;
	}
      }
      if(compare_objects(&p->q1,&p->z)){
	if(isconst(q2)&&(!cf||isquickkonst2(&p->q2.val,t))){
	  if(c==ADDI2P)
	    emit(f,"\tadd%s.l\t",quick[isquickkonst2(&p->q2.val,t)]);
	  else
	    emit(f,"\tsub%s.l\t",quick[isquickkonst2(&p->q2.val,t)]);
	  emit_obj(f,&p->q2,t);emit(f,",");
	  emit_obj(f,&p->z,POINTER);emit(f,"\n");
	  continue;
	}
	if(isreg(q1)&&(x_t[t&NQ]=='l'||p->q1.reg<=8)){
	  if(c==ADDI2P)
	    emit(f,"\tadd.%c\t",x_t[t&NQ]);
	  else
	    emit(f,"\tsub.%c\t",x_t[t&NQ]);
	  emit_obj(f,&p->q2,t);emit(f,",%s\n",mregnames[p->z.reg]);
	  continue;
	}
	if(isreg(q2)&&p->q2.reg>=9&&p->q2.reg<=16){
	  r=p->q2.reg;
	}else{
	  r=get_reg(f,1,p);
	  move(f,&p->q2,0,0,r,t);
	}
	if(x_t[t&NQ]!='l'&&(!isreg(z)||p->z.reg<1||p->z.reg>8)){
	  /*  wenn Ziel kein Adressregister, muss short erst auf long */
	  /*  char darf hier nicht auftreteten und long passt schon   */
	  if(t&UNSIGNED){
	    if(CPU>=68040)
	      emit(f,"\tand.l\t#65535,%s\n",mregnames[r]);
	    else
	      emit(f,"\tswap\t%s\n\tclr.w\t%s\n\tswap\t%s\n",mregnames[r],mregnames[r],mregnames[r]);
	  }else 
	    emit(f,"\text.l\t%s\n",mregnames[r]);
	  t=POINTER;
	}
	/*                if(c==ADDI2P)
			  emit(f,"\tadd.%c\t%s,",x_t[t&NQ],mregnames[r]);
			  else
			  emit(f,"\tsub.%c\t%s,",x_t[t&NQ],mregnames[r]);
			  emit_obj(f,&p->z,t);emit(f,"\n");*/
	if(c==ADDI2P) 
	  add(f,0,r,&p->z,0,t);
	else
	  sub(f,0,r,&p->z,0,t);
	continue;
      }
      if(isreg(z)&&zreg==-1&&p->z.reg>=1&&p->z.reg<=d7)
	zreg=p->z.reg; 
      else 
	zreg=get_reg(f,0,p);
      /*  Spezialfall, falls Ziel Datenregister und short */
      /*  nicht schoen, aber auf die Schnelle...          */
      if(x_t[t&NQ]!='l'&&zreg>8){
	move(f,&p->q2,0,0,zreg,t);
	if(t&UNSIGNED){
	  if(CPU>=68040)
	    emit(f,"\tand.l\t#65535,%s\n",mregnames[zreg]);
	  else
	    emit(f,"\tswap\t%s\n\tclr.w\t%s\n\tswap\t%s\n",mregnames[zreg],mregnames[zreg],mregnames[zreg]);
	}else
	  emit(f,"\text.l\t%s\n",mregnames[zreg]);
	if(c==SUBIFP) emit(f,"\tneg.l\t%s\n",mregnames[zreg]);
	add(f,&p->q1,0,0,zreg,POINTER);
	if(!isreg(z)||p->z.reg!=zreg)
	  move(f,0,zreg,&p->z,0,POINTER);
	continue;
      }
      if(!isreg(q1)||p->q1.reg!=zreg){
	move(f,&p->q1,0,0,zreg,POINTER);
      }
      if(c==ADDI2P)
	add(f,&p->q2,0,0,zreg,t);
      else 
	sub(f,&p->q2,0,0,zreg,t);
      if(!isreg(z)||p->z.reg!=zreg){
	move(f,0,zreg,&p->z,0,POINTER);
      }
      continue;
    }
    switch_IC(p);
    if((c>=OR&&c<=AND)||(c>=LSHIFT&&c<=MOD)){
      int zreg,q1reg,q2reg;
      if(isconst(q2)&&
	 (!(p->q1.flags&REG)||!(p->z.flags&REG)||p->q1.reg!=p->z.reg)&&
	 (!(p->q1.flags&VAR)||!(p->z.flags&VAR)||p->q1.v!=p->z.v)&&
	 ((c>=OR&&c<=AND)||c==ADD||c==MULT)){
	struct obj o;
	if(c==MULT){
	  eval_const(&p->q2.val,t);
	  if(zmleq(l2zm(0L),vmax)&&zumleq(ul2zum(0UL),vumax)&&!pof2(vumax)){
	    o=p->q1;p->q1=p->q2;p->q2=o;
	  }
	}else{
	  if(!cf||!ISHWORD(t)){
	    o=p->q1;p->q1=p->q2;p->q2=o;
	  }
	}
      }
      if(ISFLOAT(t)){
	if(FPU>68000){
	  if(compare_objects(&p->q2,&p->z)&&!compare_objects(&p->q2,&p->q1)){
	    struct obj m;
	    if(c==ADD||c==MULT){
	      m=p->q1;p->q1=p->q2;p->q2=m;
	    }else{
	      if(isreg(q2)){
		int tmp=get_reg(f,2,p);
		move(f,&p->q2,0,0,tmp,t);
		p->q2.reg=tmp;
		p->q2.flags=REG;
	      }
	    }
	  }
	  if(isreg(z)&&p->z.reg>=fp0)
	    zreg=p->z.reg;
	  else 
	    zreg=get_reg(f,2,p);
	  if(!isreg(q1)||p->q1.reg!=p->z.reg)
	    move(f,&p->q1,0,0,zreg,t);
	  emit(f,"\tf%s.",ename[c]);
	  if(isreg(q2))
	    emit(f,"x\t");
	  else
	    emit(f,"%c\t",x_t[t&NQ]);
	  emit_obj(f,&p->q2,t);
	  emit(f,",%s\n",mregnames[zreg]);
	  if(!isreg(z)||p->z.reg!=zreg){
	    move(f,0,zreg,&p->z,0,t);
	  }
	  continue;
	}else{
	  saveregs(f,p);
	  assign(f,p,&p->q2,0,PUSH,msizetab[t&NQ],t);
	  assign(f,p,&p->q1,0,PUSH,msizetab[t&NQ],t);
	  scratch_modified();
	  if(GAS){
	    emit(f,"\t.global\t__ieee%s%c\n\tjbsr\t__ieee%s%c\n\tadd.%s\t#%ld,%s\n",ename[c],x_t[t&NQ],ename[c],x_t[t&NQ],strshort[1],2*msizetab[t&NQ],mregnames[sp]);
	  }else{
	    emit(f,"\tpublic\t__ieee%s%c\n\tjsr\t__ieee%s%c\n\tadd.%s\t#%ld,%s\n",ename[c],x_t[t&NQ],ename[c],x_t[t&NQ],strshort[1],2*msizetab[t&NQ],mregnames[sp]);
	  }
	  pop(2*msizetab[t&NQ]);
	  restoreregsa(f,p);
	  if((t&NQ)==DOUBLE||(t&NQ)==LDOUBLE)
	    stored0d1(f,&p->z,t);
	  else
	    move(f,0,d0,&p->z,0,t);
	  restoreregsd(f,p);
	  continue;
	}
      }
      if((c==MULT||((c==DIV||c==MOD)&&(p->typf&UNSIGNED)))&&isconst(q2)){
	/*  ersetzt mul etc. mit Zweierpotenzen     */
	/*  hier evtl. noch Fehler                  */
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
      if(c==DIV||c==MOD){
	if(x_t[t&NQ]=='l'&&CPU<68020){
	  /*  das hier ist auch nicht allzu schoen  */
	  char *fname;
	  cc_set=0;   /*  Library-Funktionen setzen cc nicht immer */
	  saveregs(f,p);
	  emit(f,"\tmove.l\t"); emit_obj(f,&p->q2,t);
	  emit(f,",-(%s)\n",mregnames[sp]);
	  push(4);
	  emit(f,"\tmove.l\t"); emit_obj(f,&p->q1,t);
	  emit(f,",-(%s)\n",mregnames[sp]);
	  push(4);
	  if(c==DIV){
	    if(t&UNSIGNED) fname="divu"; else fname="divs";
	  }else{
	    if(t&UNSIGNED) fname="modu"; else fname="mods";
	  }
	  scratch_modified();
	  if(GAS){
	    emit(f,"\t.global\t__l%s\n\tjbsr\t__l%s\n",fname,fname);
	  }else{
	    emit(f,"\tpublic\t__l%s\n\tjsr\t__l%s\n",fname,fname);
	  }
	  emit(f,"\taddq.%s\t#8,%s\n",strshort[1],mregnames[sp]);
	  pop(8);
	  restoreregsa(f,p);
	  move(f,0,d0,&p->z,0,t);
	  restoreregsd(f,p);
	  continue;
	}

      }
      /*  hier die zweite Alternative mit isreg() schreiben?  */
      if(compare_objects(&p->q2,&p->z)&&!compare_objects(&p->q2,&p->q1)){
	struct obj m;
	if((c>=OR&&c<=AND)||c==ADD||c==SUB){
	  if(c!=SUB){
	    m=p->q1;p->q1=p->q2;p->q2=m;
	  }else{
	    if(isreg(q2)&&p->q2.reg>=d0&&p->q2.reg<=d7){
	      m=p->q1;p->q1=p->q2;p->q2=m;
	      c=ADD;
	      emit(f,"\tneg.%c\t",x_t[t&NQ]);
	      emit_obj(f,&p->q1,t);emit(f,"\n");
	    }
	  }
	}
      }
      if(compare_objects(&p->q1,&p->z)){
	if((c>=OR&&c<=AND)||c==ADD||c==SUB){
	  int r;
	  if(isconst(q2)&&(!cf||isreg(z)||((c==ADD||c==SUB)&&isquickkonst2(&p->q2.val,t&NQ)))){
	    if(cf&&((t&NQ)==CHAR||ISHWORD(t))){
	      if(isreg(q1)) r=p->q1.reg; else r=get_reg(f,1,p);
	      loadext(f,r,&p->q1,t);
	      emit(f,"\t%s.l\t",ename[c]);
	      emit_obj(f,&p->q2,t);
	      emit(f,",%s\n",mregnames[r]);
	      move(f,0,r,&p->z,0,t);
	      continue;
	    }else{
	      if(c==ADD) {add(f,&p->q2,0,&p->z,0,t);continue;}
	      if(c==SUB) {sub(f,&p->q2,0,&p->z,0,t);continue;}
	      emit(f,"\t%s.%c\t",ename[c],x_t[t&NQ]);
	      emit_obj(f,&p->q2,t);emit(f,",");
	      emit_obj(f,&p->z,t);emit(f,"\n");
	    }
	    continue;
	  }
	  if(!isreg(z)&&(!cf||x_t[t&NQ]=='l')){
	    if(isreg(q2)&&p->q2.reg>=d0&&p->q2.reg<=d7)
	      r=p->q2.reg; else r=get_reg(f,1,p);
	    if(!isreg(q2)||p->q2.reg!=r){
	      move(f,&p->q2,0,0,r,t);
	    }
	    emit(f,"\t%s.%c\t%s,",ename[c],x_t[t&NQ],mregnames[r]);
	    emit_obj(f,&p->z,t);emit(f,"\n");
	    continue;
	  }
	}
      }
      /*  bei xor oder asl (ausser 0<=const<=8) muss q2 in Register   */
      if(isreg(q2)&&p->q2.reg>=d0&&p->q2.reg<=d7){
	q2reg=p->q2.reg;
      }else{
	if(c==LSHIFT||c==RSHIFT||c==XOR){
	  int t2=q2typ(p)&NU;
	  eval_const(&p->q2.val,t2);
	  if(c==XOR||!isconst(q2)||!isquickkonst2(&p->q2.val,t2)){
	    q2reg=get_reg(f,1,p);
	    move(f,&p->q2,0,0,q2reg,t2);
	  }else q2reg=0;
	}else{
	  q2reg=0;
	}
      }
      if(c==MOD&&!ISHWORD(t)){
	int modreg;
	modreg=get_reg(f,1,p);
	if(isreg(z)&&p->z.reg>=d0&&p->z.reg<=d7&&p->z.reg!=q2reg)
	  zreg=p->z.reg; else zreg=get_reg(f,1,p);
	move(f,&p->q1,0,0,modreg,t);
	if(0 /*CPU==68060*/){
	  /*  div?l.l wird da emuliert?   */
	  emit(f,"\tsmi\t%s\n\textb.l\t%s\n",mregnames[zreg],mregnames[zreg]);
	  if(t&UNSIGNED) emit(f,"\tdivu.%c\t",x_t[t&NQ]); else emit(f,"\tdivs.%c\t",x_t[t&NQ]);
	}else{
	  if(t&UNSIGNED) emit(f,"\tdivul.%c\t",x_t[t&NQ]); else emit(f,"\tdivsl.%c\t",x_t[t&NQ]);
	}
	emit_obj(f,&p->q2,t);
	emit(f,",%s:%s\n",mregnames[zreg],mregnames[modreg]);
	move(f,0,zreg,&p->z,0,t);
	cc_set=0;
	continue;
      }
      if(isreg(z)&&p->z.reg>=d0&&p->z.reg<=d7&&(p->z.reg!=q2reg||(isreg(q1)&&p->q1.reg==q2reg)))
	zreg=p->z.reg; else zreg=get_reg(f,1,p);
      if(isreg(q1)&&p->q1.reg>=d0&&p->q1.reg<=d7)
	q1reg=p->q1.reg; else q1reg=0;
      if(q1reg!=zreg){
	move(f,&p->q1,0,0,zreg,t);
      }
      if(c!=MULT&&c!=DIV&&c!=MOD&&c!=ADD&&c!=SUB){
	if(cf&&c==RSHIFT){
	  if(cf&&(t&NU)==CHAR)
	    emit(f,"\textb.l\t%s\n",mregnames[zreg]);
	  else if(cf&&(t&NU)==SHORT)
	    emit(f,"\text.w\t%s\n",mregnames[zreg]);
	  if(cf&&(t&NU)==(UNSIGNED|CHAR))
	    emit(f,"\tand.l\t#255,%s\n",mregnames[zreg]);
	  else if(cf&&(t&NU)==(UNSIGNED|SHORT))
	    emit(f,"\tand.l\t#65535,%s\n",mregnames[zreg]);
	}
	if(c==RSHIFT&&!(t&UNSIGNED))
	  emit(f,"\tasr.%c\t",cf?'l':x_t[t&NQ]);
	else
	  emit(f,"\t%s.%c\t",ename[c],cf?'l':x_t[t&NQ]);
	if(q2reg)
	  emit(f,"%s",mregnames[q2reg]);
	else 
	  emit_obj(f,&p->q2,q2typ(p));
	emit(f,",%s\n",mregnames[zreg]);
      }else{
	if(c==ADD) add(f,&p->q2,q2reg,0,zreg,t);
	if(c==SUB) sub(f,&p->q2,q2reg,0,zreg,t);
	if(c==MULT||c==DIV||c==MOD) mult(f,&p->q2,q2reg,0,zreg,t,c,p);
      }
      if((!isreg(z)||p->z.reg!=zreg)){
	move(f,0,zreg,&p->z,0,t);
      }
      continue;
    }
    ierror(0);
  }
  if(notpopped){
    emit(f,"\tadd%s.%s\t#%ld,%s\n",quick[notpopped<=8],strshort[notpopped<32768],notpopped,mregnames[sp]);
    pop(notpopped);notpopped=0;
  }
  function_bottom(f,v,zm2l(offset));
  if(debug_info&&!HUNKDEBUG){
    emit(f,"%s%d:\n",labprefix,++label);
    dwarf2_function(f,v,label);
    if(f) section=-1;
  }
  pushflag=0;
}

/*FIXME*/
int shortcut(int code,int typ)
{
  if(!cf&&(code==COMPARE||code==ADD||code==SUB)) return(1);
  return 0;
}
void init_db(FILE *f)
{
  if(!HUNKDEBUG){
    if(GAS)
      dwarf2_setup(sizetab[POINTER],".byte",".2byte",".4byte",".4byte","l","_",".section");
    else
      dwarf2_setup(sizetab[POINTER],"dc.b","dc.w","dc.l","dc.l","l","_","section");
    dwarf2_print_comp_unit_header(f);
  }
}
void cleanup_db(FILE *f)
{
  if(!HUNKDEBUG&&f){
    dwarf2_cleanup(f);
    if(f) section=-1;
  }
}
void cleanup_cg(FILE *f)
{
  title(f);
  if(f&&stack_check){
    if(GAS)
      emit(f,"\t.global\t___stack_check\n");
    else
      emit(f,"\tpublic\t___stack_check\n");
  }
  /*printf("pushed %d, saved %d, removed allocreg %d\n",missing,savedemit,savedalloc);*/
  return;
}

/* mark instructions which can (probably) be implemented with faster
   machine-code than the IC migh suggest, e.g. an addition which can
   be merged with a load bz use of target addressing-modes;
   the optimizer should hesitate to modifz such instructions if it's not
   a definite win */

static int is_single_eff_ic(struct IC *p)
{
  struct Var *v;
  if(p->code!=ADDI2P&&p->code!=SUBIFP)
    return 0;
  if(!isconst(q2)){
    if(CONSERVATIVE_SR){
      if((p->q2.flags&(VAR|DREFOBJ))!=VAR)
	return 0;
      if(p->q2.v->storage_class==STATIC||p->q2.v->storage_class==EXTERN)
	return 0;
    }else
      return 0;
  }
  if((p->q1.flags&(VAR|DREFOBJ))!=VAR)
    return 0;
  if(p->q1.v->storage_class==STATIC||p->q1.v->storage_class==EXTERN)
    return 0;
  if((p->z.flags&(VAR|DREFOBJ))!=VAR)
    return 0;
  if(p->z.v->storage_class==STATIC||p->z.v->storage_class==EXTERN)
    return 0;
  v=p->z.v;
  for(p=p->next;p;p=p->next){
    int c=p->code;
    if(c==LABEL||(c>=BEQ&&c<=BRA))
      return 0;
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
  }
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

int reg_pair(int r,struct rpair *p)
     /* Returns 0 if the register is no register pair. If r  */
     /* is a register pair non-zero will be returned and the */
     /* structure pointed to p will be filled with the two   */
     /* elements.                                            */
{
  if(r<=fp7) return 0;
  if(p){
    switch(r){
    case d0d1: p->r1=d0;p->r2=d1;break;
    case d2d3: p->r1=d2;p->r2=d3;break;
    case d4d5: p->r1=d4;p->r2=d5;break;
    case d6d7: p->r1=d6;p->r2=d7;break;
    default: ierror(0);
    }
  }
  return 1;
}

int emit_peephole(void)
{
  int entries,i,r1,r2,r3,r4;
  char *asmline[EMIT_BUF_DEPTH],c1,c2,c3,c4,e;

  return 0;
  
  if(OLDPEEPHOLE) return 0;
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
    if(sscanf(asmline[1],"\tmove.l\t(a7)+,%c%d\n%c",&c1,&r1,&e)==2&&
       sscanf(asmline[0],"\tmove.l\t%c%d,-(a7%c",&c2,&r2,&e)==3&&
       c1==c2&&r1==r2&&(c1=='a'||c1=='d')&&r1>=0&&r1<=7&&e==')'){
      remove_asm();
      remove_asm();
      savedemit+=2;
      return 1;
    }
    if(sscanf(asmline[1],"\tmovem.l\t(a7)+,%c%d\n%c",&c1,&r1,&e)==2&&
       sscanf(asmline[0],"\tmove.l\t%c%d,-(a7%c",&c2,&r2,&e)==3&&
       c1==c2&&r1==r2&&(c1=='a'||c1=='d')&&r1>=0&&r1<=7&&e==')'){
      remove_asm();
      remove_asm();
      savedemit+=2;
      return 1;
    }
    if(sscanf(asmline[1],"\tmove.l\t%c%d,%c%d\n%c",&c1,&r1,&c2,&r2,&e)==4&&
       sscanf(asmline[0],"\tmove.l\t%c%d,%c%d\n%c",&c3,&r3,&c4,&r4,&e)==4&&
       c1==c4&&r1==r4&&c2==c3&&r2==r3&&r1>=0&&r1<=7&&r2>=0&&r2<=7&&
       (c1=='a'||c1=='d')&&(c2=='a'||c2=='d')){
      remove_asm();
      savedemit++;
      return 1;
    }
  }
  return 0;
}

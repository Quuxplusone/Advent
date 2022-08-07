/*  Code generator for a PPC RISC cpu with 32 general purpose,  */
/*  32 floating point and 8 condition code registers.           */

#include "supp.h"

static char FILE_[]=__FILE__;

#include "dwarf2.c"

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for PPC V0.7 (c) in 1997-2022 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={STRINGFLAG,STRINGFLAG,0,0,0,0,
                    0,0,0,0,0,
                    0,0,0,0,0,0,
		    0,0,0};
char *g_flags_name[MAXGF]={"cpu","fpu","const-in-data","sd","merge-constants","fsub-zero",
                           "elf","amiga-align","no-regnames","no-peephole","setccs",
                           "use-lmw","poweropen","sc","madd","eabi","gas",
			   "no-align-args","conservative-sr","use-commons",
                           "baserel32os4","baserel32mos","oldlibcalls"};
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
                        "r0","r1","r2","r3","r4","r5","r6","r7",
                        "r8","r9","r10","r11","r12","r13","r14","r15",
                        "r16","r17","r18","r19","r20","r21","r22","r23",
                        "r24","r25","r26","r27","r28","r29","r30","r31",
                        "f0","f1","f2","f3","f4","f5","f6","f7",
                        "f8","f9","f10","f11","f12","f13","f14","f15",
                        "f16","f17","f18","f19","f20","f21","f22","f23",
                        "f24","f25","f26","f27","f28","f29","f30","f31",
                        "cr0","cr1","cr2","cr3","cr4","cr5","cr6","cr7",
                        "cnt",
			"r3/r4","r5/r6","r7/r8","r9/r10",
			"r14/r15","r16/r17","r18/r19","r20/r21",
			"r22/r23","r24/r25","r26/r27","r28/r29","r30/r31",
			"lr"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
		          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		          1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
			  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			  1,1,0,0,0,1,1,1,1,
			  1,1,1,1,
			  0,0,0,0,0,0,0,0,0};

int reg_prio[MAXR+1]={0,0,0,0,19,20,21,22,23,24,25,26,27,28,0,1,2,
		        3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
		        0,1,2,3,4,5,6,7,8,9,10,11,12,13,0,0,
		        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		        1,1,0,0,0,1,1,1,1,
		        19,21,23,25,
		        1,3,5,7,9,11,13,15,17};

struct reg_handle empty_reg_handle={0,0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__far","__near","__chip","__saveds","__rfi","__saveall",
		     "__syscall","__nosave","__brel",0};
#define FAR 1
#define NEAR 2
#define CHIP 4
#define SAVEDS 8
#define RFI 16
#define SAVEALL 32
#define SYSCALL 64
#define NOSAVE 128
#define BREL 256

/****************************************/
/*  Private data and functions.         */
/****************************************/

#define SMALLDATA (g_flags[3]&USEDFLAG)
#define POWEROPEN (g_flags[12]&USEDFLAG)
#define STORMC (g_flags[13]&USEDFLAG)
#define EABI (g_flags[15]&USEDFLAG)
#define GAS (g_flags[16]&USEDFLAG)
#define NOALIGNARGS (g_flags[17]&USEDFLAG)
#define CONSERVATIVE_SR (g_flags[18]&USEDFLAG)
#define USE_COMMONS (g_flags[19]&USEDFLAG)
#define BASERELOS4 (g_flags[20]&USEDFLAG)
#define BASERELMOS (g_flags[21]&USEDFLAG)
#define OLDLIBCALLS (g_flags[22]&USEDFLAG)


static char *mregnames[MAXR+1];

static char *ret="\tblr\n";

static long malign[MAX_TYPE+1]=  {1,1,2,4,4,8,4,8,8,1,4,1,1,1,4,1};
static long msizetab[MAX_TYPE+1]={1,1,2,4,4,8,4,8,8,0,4,0,0,0,4,0};

static struct Typ ltyp={LONG},lltyp={LLONG},ldbl={DOUBLE},lchar={CHAR};

static char *marray[]={"__section(x,y)=__vattr(\"section(\"#x\",\"#y\")\")",
		       "__PPC__",
		       "__aos4libcall=__attr(\"aos4libcall;\")",
		       "__linearvarargs=__attr(\"linearvarargs;\")",
		       "__interrupt=__rfi __saveall",
		       0};

const int r4=5,r5=6,r6=7,r3r4=74,r5r6=75;


static int r0=1;                   /*  special register                    */
static int r2=3;                   /*  reserved or toc                     */
static int r3=4;                   /*  return value                        */
static int sp=2;                   /*  Stackpointer                        */
static int fp=2;                   /*  Framepointer                        */
static int vlafp=32;               /*  Framepointer to use with vlas       */
static int sd=14;                  /*  SmallDataPointer                    */
static int sd2=3;                  /*  SmallData2 (eabi)                   */
static int t1=12,t2=13,t3=1;       /*  Temporaries used by code generator  */
static int f1=45,f2=46,f3=33;      /*  Temporaries used by code generator  */
static int cr0=65;                 /*  Default condition-code-register     */
static int ctr=73;                 /*  ctr                                 */
static int lr=87;                  /*  link register                       */
static int bp32os4=3;              /*  baserel32 pointer for Amiga OS4     */
static int bp32mos=14;             /*  baserel32 pointer for MorphOS       */

#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define TOC 4
#define SDATA 5
#define SDATA2 6
#define SBSS 7
#define SPECIAL 8

#if HAVE_OSEK
/* removed */
/* removed */
#endif

static long stack;
static int stack_valid;
static long tmpoff; /* offset for additional temporaries, upper end! */
static int lastlabel,exit_label;
static int sdp;
static int q1loaded,q2loaded;
static int section=-1,newobj,crsave;
static char *codename="\t.text\n\t.align\t2\n",
  *dataname="\t.data\n\t.align\t2\n",
  *bssname="",
  *rodataname="\t.section\t.rodata\n\t.align\t2\n",
  *tocname="\t.tocd\n\t.align\t2\n",
  *sdataname="\t.section\t\".sdata\",\"aw\"\n\t.align\t2\n",
  *sdata2name="\t.section\t\".sdata2\",\"a\"\n\t.align\t2\n",
  *sbssname="\t.section\t\".sbss\",\"auw\"\n\t.align\t2\n";

static char *labprefix="l",*idprefix="_",*tocprefix="@_";
static long frameoffset,pushed,maxpushed,framesize,localoffset,minframe=8;
static void emit_obj(FILE *f,struct obj *p,int t);

static int all_regs;

struct StatFPtrList {
  struct StatFPtrList *next;
  struct Var *vptr;
};
static struct StatFPtrList *firstfptr = NULL;

void title(FILE *f)
{
  static int done;
  extern char *inname; /*grmpf*/
  if(!done&&f){
    done=1;
    emit(f,"\t.file\t\"%s\"\n",inname);
  }
} 

static long real_offset(struct obj *o)
{
  long off;
  if(zm2l(o->v->offset)>=0){
     return zm2l(o->v->offset)+frameoffset+zm2l(o->val.vmax);
  }else{
    return framesize+minframe-zm2l(o->v->offset)-zm2l(maxalign)+zm2l(o->val.vmax);
  }
}
static long hi(long off)
{
  zmax zm=l2zm(off),r=zmrshift(zm,l2zm(16L));
  if(!zmeqto(zmand(zm,l2zm(32768L)),l2zm(0L))) r=zmadd(r,l2zm(1L));
  return zm2l(zs2zm(zm2zs(zmand(r,l2zm(65535L)))));
}
static long lo(long off)
{
  return zm2l(zs2zm(zm2zs(zmand(l2zm(off),l2zm(65535L)))));
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
  if(t==LDOUBLE) t=DOUBLE;
  if(g_flags[4]&USEDFLAG){
    for(p=firstfpc;p;p=p->next){
      if(t==p->typ){
        eval_const(&p->val,t);
        if(t==FLOAT&&zldeqto(vldouble,zf2zld(o->val.vfloat))) return p->label;
        if(t==DOUBLE&&zldeqto(vldouble,zd2zld(o->val.vdouble))) return p->label;
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
#define UPDATE  64

static struct obj *cam(int flags,int base,long offset)
/*  Initializes an addressing-mode structure and returns a pointer to   */
/*  that object. Will not survive a second call!                        */
{
  static struct obj obj;
  static struct AddressingMode am;
  obj.am=&am;
  am.flags=flags;
  am.base=base;
  am.offset=offset;
  return &obj;
}

static char *ldts[]={"","bz","ha","wz","wz","wz","fs","fd","fd","","wz"};
static char *ldtu[]={"","bz","hz","wz","wz","wz","??","??","??","??","??"};
static char *sdts[]={"","b","h","w","w","w","fs","fd","fd","","w"};
static char *sdtu[]={"","b","h","w","w","w","??","??","??","??","??"};

#define ldt(t) ((t&UNSIGNED)?ldtu[t&NQ]:ldts[t&NQ])
#define sdt(t) ((t&UNSIGNED)?sdtu[t&NQ]:sdts[t&NQ])

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

static int use_sd(int t)
/*  Shall the object of type t be addressed in small-data-mode?  */
{
  int r=0;
  if(ISFUNC(t)) return 0;
  if(g_flags[3]&USEDFLAG) 
    r=sd;
  else if(STORMC&&(t&NQ)<=POINTER) 
    r=sd;
  else if(EABI&&ISSCALAR(t)){
    if(t&CONST)
      r=sd2;
    else
      r=sd;
  }
  return r;
}

static void load_address(FILE *f,int r,struct obj *o,int typ)
/*  Generates code to load the address of a variable into register r.   */
{
  BSET(regs_modified,r);
  if(!(o->flags&VAR)) 
    ierror(0);
  if(o->v->storage_class==AUTO||o->v->storage_class==REGISTER){
    long off=real_offset(o);
    if(off<=32767){
      emit(f,"\taddi\t%s,%s,%ld\n",mregnames[r],mregnames[fp],off);
    }else{
      if(r==r0){
	BSET(regs_modified,t2);
	emit(f,"\taddis\t%s,%s,%ld\n",mregnames[t2],mregnames[fp],hi(off));
	emit(f,"\taddi\t%s,%s,%ld\n",mregnames[r],mregnames[t2],lo(off));
      }else{
	emit(f,"\taddis\t%s,%s,%ld\n",mregnames[r],mregnames[fp],hi(off));
	emit(f,"\taddi\t%s,%s,%ld\n",mregnames[r],mregnames[r],lo(off));
      }
    }
  }else{
    if((sdp=use_sd(o->v->vtyp->flags))&&!(o->v->tattr&FAR)
       &&zmleq(l2zm(0L),o->val.vmax)&&!zmleq(szof(o->v->vtyp),o->val.vmax)){
      emit(f,"\tla\t%s,",mregnames[r]);
      emit_obj(f,o,typ);
      if(GAS)
	emit(f,"@sda21(0)\n");
      else
	emit(f,"(%s)\n",mregnames[sdp]);
    }else{
      if(POWEROPEN){
        zmax offset=o->val.vmax;
	if(f){
	  if((o->v->vtyp->flags&NQ)==FUNKT&&o->v->storage_class==STATIC){
	    /* check if static function pointer was already created in TOC */
	    struct StatFPtrList **oldsfp=&firstfptr;
	    struct StatFPtrList *sfp=firstfptr;
	    while(sfp){
	      if(sfp->vptr==o->v) break;
	      oldsfp=&sfp->next;
	      sfp=sfp->next;
	    }
	    if(!sfp){
	      *oldsfp=sfp=mymalloc(sizeof(struct StatFPtrList));
	      sfp->next=NULL;
	      sfp->vptr=o->v;
	    }
	  }
	  emit(f,"\tl%s\t%s,%s",ldt(LONG),mregnames[r],tocprefix);
	  o->val.vmax=l2zm(0L);emit_obj(f,o,POINTER);o->val.vmax=offset;
	  emit(f,"(%s)\n",mregnames[r2]);
	  if(hi(zm2l(offset))) emit(f,"\taddis\t%s,%s,%ld\n",mregnames[r],mregnames[r],hi(zm2l(offset)));
	  if(lo(zm2l(offset))) emit(f,"\taddi\t%s,%s,%ld\n",mregnames[r],mregnames[r],lo(zm2l(offset)));
	}
      }else{
        if(BASERELOS4){
          emit(f,"\taddis\t%s,%s,",mregnames[r],mregnames[bp32os4]);
          emit_obj(f,o,typ);emit(f,"@brel@ha\n");
          emit(f,"\taddi\t%s,%s,",mregnames[r],mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@brel@l\n");
        }else if(BASERELMOS){
          emit(f,"\taddis\t%s,%s,",mregnames[r],mregnames[bp32mos]);
          emit_obj(f,o,typ);emit(f,"@drel@ha\n");
          emit(f,"\taddi\t%s,%s,",mregnames[r],mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@drel@l\n");
        }else{
          emit(f,"\tlis\t%s,",mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@ha\n");
          emit(f,"\taddi\t%s,%s,",mregnames[r],mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@l\n");
        }
      }
    }
  }
}
static void load_reg(FILE *f,int r,struct obj *o,int typ,int tmp)
/*  Generates code to load a memory object into register r. tmp is a    */
/*  general purpose register which may be used. tmp can be r.           */
{
  typ&=NU;
  BSET(regs_modified,r);
  if(o->flags&KONST){
    long l;
    eval_const(&o->val,typ);
    if(ISFLOAT(typ)){
      int lab;
      if((g_flags[5]&USEDFLAG)&&zldeqto(vldouble,d2zld(0.0))){
        emit(f,"\tfsub\t%s,%s,%s\n",mregnames[r],mregnames[r],mregnames[r]);
        return;
      }
      lab=addfpconst(o,typ);
      if(sdp=use_sd(typ)){
	if(GAS)
	  emit(f,"\tl%s\t%s,%s%d@sda21(0)\n",ldt(typ),mregnames[r],labprefix,lab);
	else
	  emit(f,"\tl%s\t%s,%s%d(%s)\n",ldt(typ),mregnames[r],labprefix,lab,mregnames[sdp]);
      }else{
	BSET(regs_modified,tmp);
        if(POWEROPEN){
          emit(f,"\tl%s\t%s,%s%s%ld(%s)\n",ldt(LONG),mregnames[tmp],tocprefix,labprefix,(long)lab,mregnames[r2]);
          emit(f,"\tl%s\t%s,0(%s)\n",ldt(typ),mregnames[r],mregnames[tmp]);
        }else{
          emit(f,"\tlis\t%s,%s%d@ha\n",mregnames[tmp],labprefix,lab);
          emit(f,"\tl%s\t%s,%s%d@l(%s)\n",ldt(typ),mregnames[r],labprefix,lab,mregnames[tmp]);
        }
      }
      return;
    }
    if(r==1) ierror(0);
    l=hi(zm2l(vmax));
    if(l){
      emit(f,"\tlis\t%s,%ld\n",mregnames[r],l);
      l=lo(zm2l(vmax));
      if(l)
	emit(f,"\taddi\t%s,%s,%ld\n",mregnames[r],mregnames[r],l);
    }else{
      emit(f,"\tli\t%s,%ld\n",mregnames[r],lo(vmax));
    }
    return;
  }
  if((o->flags&VAR)&&(o->v->storage_class==EXTERN||o->v->storage_class==STATIC)){
    if(o->flags&VARADR){
      load_address(f,r,o,POINTER);
    }else{
      if((sdp=use_sd(o->v->vtyp->flags))&&!(o->v->tattr&FAR)){
        emit(f,"\tl%s\t%s,",ldt(typ),mregnames[r]);
        emit_obj(f,o,typ);
	if(GAS)
	  emit(f,"@sda21(0)\n");
	else
	  emit(f,"(%s)\n",mregnames[sdp]);
      }else{
	BSET(regs_modified,tmp);
        if(POWEROPEN){
          zmax offset=o->val.vmax;
          emit(f,"\tl%s\t%s,%s",ldt(LONG),mregnames[tmp],tocprefix);
          o->val.vmax=l2zm(0L);emit_obj(f,o,POINTER);o->val.vmax=offset;
          emit(f,"(%s)\n",mregnames[r2]);
          if(hi(zm2l(offset))) emit(f,"\taddis\t%s,%s,%ld\n",mregnames[tmp],mregnames[tmp],hi(zm2l(offset)));
          emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(typ),mregnames[r],lo(zm2l(offset)),mregnames[tmp]);
        }else{
          if(BASERELOS4){
            emit(f,"\taddis\t%s,%s,",mregnames[tmp],mregnames[bp32os4]);
            emit_obj(f,o,typ);emit(f,"@brel@ha\n");
            emit(f,"\tl%s\t%s,",ldt(typ),mregnames[r]);
            emit_obj(f,o,typ);emit(f,"@brel@l(%s)\n",mregnames[tmp]);
          }else if(BASERELMOS){
            emit(f,"\taddis\t%s,%s,",mregnames[tmp],mregnames[bp32mos]);
            emit_obj(f,o,typ);emit(f,"@drel@ha\n");
            emit(f,"\tl%s\t%s,",ldt(typ),mregnames[r]);
            emit_obj(f,o,typ);emit(f,"@drel@l(%s)\n",mregnames[tmp]);
          }else{
            emit(f,"\tlis\t%s,",mregnames[tmp]);
            emit_obj(f,o,typ);emit(f,"@ha\n");
            emit(f,"\tl%s\t%s,",ldt(typ),mregnames[r]);
            emit_obj(f,o,typ);emit(f,"@l(%s)\n",mregnames[tmp]);
          }
        }
      }
    }
  }else{
    if((o->flags&(DREFOBJ|REG))==REG){
      if(r!=o->reg)
        emit(f,"\t%smr\t%s,%s\n",r>=33?"f":"",mregnames[r],mregnames[o->reg]);
    }else if(!o->am&&(o->flags&(DREFOBJ|REG))==(REG|DREFOBJ)){
      emit(f,"\tl%s\t%s,0(%s)\n",ldt(typ),mregnames[r],mregnames[o->reg]);
    }else if(!o->am){
      long off=real_offset(o);
      if(off<=32767){
        emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(typ),mregnames[r],off,mregnames[fp]);
      }else{
	BSET(regs_modified,tmp);
        emit(f,"\taddis\t%s,%s,%ld\n",mregnames[tmp],mregnames[fp],hi(off));
        emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(typ),mregnames[r],lo(zm2l(off)),mregnames[tmp]);
      }
    }else{
      emit(f,"\tl%s%s%s\t%s,",ldt(typ),(o->am->flags&UPDATE)?"u":"",(o->am->flags&REG_IND)?"x":"",mregnames[r]);
      emit_obj(f,o,typ);emit(f,"\n");
    }
  }
}

static void store_reg(FILE *f,int r,struct obj *o,int typ)
/*  Generates code to store register r into memory object o.            */
{
  int tmp;
  typ&=NQ;
  if((o->flags&VAR)&&(o->v->storage_class==EXTERN||o->v->storage_class==STATIC)){
    int tmp=t1;
    if(tmp==r) tmp=t2;
    if((sdp=use_sd(o->v->vtyp->flags))&&!(o->v->tattr&FAR)){
      emit(f,"\tst%s\t%s,",sdt(typ),mregnames[r]);
      emit_obj(f,o,typ);
      if(GAS)
	emit(f,"@sda21(0)\n");
      else
	emit(f,"(%s)\n",mregnames[sdp]);
      return;
    }else{
      BSET(regs_modified,tmp);
      if(POWEROPEN){
        zmax offset=o->val.vmax;
        emit(f,"\tl%s\t%s,%s",ldt(LONG),mregnames[tmp],tocprefix);
        o->val.vmax=l2zm(0L);emit_obj(f,o,POINTER);o->val.vmax=offset;
        emit(f,"(%s)\n",mregnames[r2]);
        if(hi(zm2l(offset))) emit(f,"\taddis\t%s,%s,%ld\n",mregnames[tmp],mregnames[tmp],hi(zm2l(offset)));
        emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(typ),mregnames[r],lo(zm2l(offset)),mregnames[tmp]);
        return;
      }else{
        if(BASERELOS4){
          emit(f,"\taddis\t%s,%s,",mregnames[tmp],mregnames[bp32os4]);
          emit_obj(f,o,typ);emit(f,"@brel@ha\n");
          emit(f,"\tst%s\t%s,",sdt(typ),mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@brel@l(%s)\n",mregnames[tmp]);
        }else if(BASERELMOS){
          emit(f,"\taddis\t%s,%s,",mregnames[tmp],mregnames[bp32mos]);
          emit_obj(f,o,typ);emit(f,"@drel@ha\n");
          emit(f,"\tst%s\t%s,",sdt(typ),mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@drel@l(%s)\n",mregnames[tmp]);
        }else{
          emit(f,"\tlis\t%s,",mregnames[tmp]);
          emit_obj(f,o,typ);emit(f,"@ha\n");
          emit(f,"\tst%s\t%s,",sdt(typ),mregnames[r]);
          emit_obj(f,o,typ);emit(f,"@l(%s)\n",mregnames[tmp]);
        }
        return;
      }
    }
  }
  if(!(o->flags&DREFOBJ)&&!o->am){
    long off=real_offset(o);
    if(r==t1) tmp=t2; else tmp=t1;
    if(off<=32767){
      emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(typ),mregnames[r],off,mregnames[fp]);
    }else{

      BSET(regs_modified,tmp);
      emit(f,"\taddis\t%s,%s,%ld\n",mregnames[tmp],mregnames[fp],hi(off));
      emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(typ),mregnames[r],lo(off),mregnames[tmp]);
    }
  }else{
    if(!o->am){
      emit(f,"\tst%s\t%s,",sdt(typ),mregnames[r]);
      emit_obj(f,o,typ);emit(f,"\n");
    }else{
      emit(f,"\tst%s%s%s\t%s,",sdt(typ),(o->am->flags&UPDATE)?"u":"",(o->am->flags&REG_IND)?"x":"",mregnames[r]);
      emit_obj(f,o,typ);emit(f,"\n");
    }
  }
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

#if 1
static char *dct[]={"","byte","2byte","4byte","4byte","4byte","4byte","4byte","4byte"};
#else
static char *dct[]={"","byte","short","long","long","long","long","long","long"};
#endif
static struct IC *do_refs(FILE *,struct IC *);
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)

static int q1reg,q2reg,zreg;

static char *ccs[]={"eq","ne","lt","ge","le","gt",""};
static char *logicals[]={"or","xor","and"};
static char *record[]={"","."};
static char *arithmetics[]={"slw","srw","add","sub","mullw","divw","mod"};
static char *isimm[]={"","i"};

static struct IC *do_refs(FILE *f,struct IC *p)
/*  Does some pre-processing like fetching operands from memory to      */
/*  registers etc.                                                      */
{
  int typ,typ1,reg,c=abs(p->code);

  q1loaded=q2loaded=0;

  /* cannot use q1typ, because p->code may have been negated */
  if(c==CONVERT) 
    typ=p->typf2;
  else
    typ=p->typf;

  if((typ&NQ)==POINTER) typ=UNSIGNED|LONG;

  if((c==SUB||c==SUBIFP)&&(p->q2.flags&(KONST|DREFOBJ))==KONST&&(typ&NQ)<=LLONG){
    eval_const(&p->q2.val,typ);
    if(zmleq(vmax,l2zm(32768L))&&zmleq(l2zm(-32767L),vmax)){
      union atyps val;
      if(c==SUB){
        if(p->code==SUB) p->code=c=ADD; else p->code=c=-ADD;
      }else{
        if(p->code==SUBIFP) p->code=c=ADDI2P; else p->code=c=-ADDI2P;
      }
      c=abs(c);
      val.vmax=zmsub(l2zm(0L),vmax);
      eval_const(&val,MAXINT);
      insert_const(&p->q2.val,typ);
      p->typf=typ=(typ&~UNSIGNED);
    }
  }

  q1reg=q2reg=zreg=0;
  if(p->q1.flags&REG) q1reg=p->q1.reg;
  if(p->q2.flags&REG) q2reg=p->q2.reg;
  if((p->z.flags&(REG|DREFOBJ))==REG) zreg=p->z.reg;

  if((p->q1.flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->q1.val,typ);
    if(ISFLOAT(typ)) reg=f1; else reg=t1;
    if(c==ASSIGN&&zreg) reg=zreg;
    if(c==SETRETURN&&p->z.reg) reg=p->z.reg;
    if(c!=SUB||(typ&NQ)>LONG||!zmleq(vmax,l2zm(32767L))||!zmleq(l2zm(-32768L),vmax)){
      load_reg(f,reg,&p->q1,typ,t1);
      q1reg=reg;
      q1loaded=1;
    }
  }else if(c!=ADDRESS){
    if(ISFLOAT(typ)) reg=f1; else reg=t1;
    if((c==ASSIGN||(c==CONVERT&&ISINT(p->typf2)))&&zreg>=1&&zreg<=32) reg=zreg;
    if((c==ASSIGN||(c==CONVERT&&ISFLOAT(p->typf2)))&&zreg>=33&&zreg<=64) reg=zreg;
    if(c==SETRETURN&&p->z.reg) reg=p->z.reg;
    if(p->q1.am){
      load_reg(f,reg,&p->q1,typ,t1);
      q1reg=reg;
      q1loaded=1;
    }else{
      if(p->q1.flags&&!q1reg){
        if(p->q1.flags&DREFOBJ) {typ1=POINTER;reg=t1;} else typ1=typ;
        if((typ1&NQ)<=POINTER){
          int m=p->q1.flags;
          p->q1.flags&=~DREFOBJ;
          load_reg(f,reg,&p->q1,typ1,t1);
          p->q1.flags=m;
          q1reg=reg;
	  q1loaded=1;
        }
      }
      if((p->q1.flags&DREFOBJ)&&(typ&NQ)<=POINTER){
        if(ISFLOAT(typ)) reg=f1; else reg=t1;
        if((c==ASSIGN||(c==CONVERT&&ISINT(p->typf2)))&&zreg>=1&&zreg<=32) reg=zreg;
	if((c==ASSIGN||(c==CONVERT&&ISFLOAT(p->typf2)))&&zreg>=33&&zreg<=64) reg=zreg;
        if(c==SETRETURN&&p->z.reg) reg=p->z.reg;
        if(p->q1.am)
          load_reg(f,reg,&p->q1,typ,t1);
        else
          load_reg(f,reg,cam(IMM_IND,q1reg,0),typ,t1);
        q1reg=reg;
	q1loaded=1;
      }
    }
  }
  /* cannot use q2typ (see above) */
  typ=p->typf;
  if((p->q2.flags&(KONST|DREFOBJ))==KONST){
    eval_const(&p->q2.val,typ);
    if(ISFLOAT(typ)) reg=f2; else reg=t2;
    if(ISFLOAT(typ)||c==DIV||c==SUB||c==MOD){
      load_reg(f,reg,&p->q2,typ,t2);
      q2reg=reg;
      q2loaded=1;
    }else{
      if((c>=OR&&c<=AND)||(c==COMPARE&&(typ&UNSIGNED))){
        if(!zumleq(vumax,ul2zum(65535UL))){
          load_reg(f,reg,&p->q2,typ,t2);
          q2reg=reg;
	  q2loaded=1;
        }
      }else{
        if(!zmleq(vmax,l2zm(32767L))||!zmleq(l2zm(-32768L),vmax)){
          load_reg(f,reg,&p->q2,typ,t2);
          q2reg=reg;
	  q2loaded=1;
        }
      }
    }
  }else{
    if(p->q2.am){
      if(ISFLOAT(typ)) reg=f2; else reg=t2;
      load_reg(f,reg,&p->q2,typ,t2);
      q2reg=reg;
      q2loaded=1;
    }else{
      if(p->q2.flags&&!q2reg){
        if((p->q2.flags&DREFOBJ)) typ1=POINTER; else typ1=typ;
        if(ISFLOAT(typ1)) reg=f2; else reg=t2;
        if((typ1&NQ)<=POINTER){
          int m=p->q2.flags;
          p->q2.flags&=~DREFOBJ;
          load_reg(f,reg,&p->q2,typ1,t2);
          p->q2.flags=m;
          q2reg=reg;
	  q2loaded=1;
        }
      }
      if((p->q2.flags&DREFOBJ)&&(typ&NQ)<=POINTER){
        if(ISFLOAT(typ)) reg=f2; else reg=t2;
        if(p->q2.am)
          load_reg(f,reg,&p->q2,typ,t2);
        else
          load_reg(f,reg,cam(IMM_IND,q2reg,0),typ,t2);
        q2reg=reg;
	q2loaded=1;
      }
    }
  }
  if(p->z.am||(p->z.flags&&!isreg(z))){
    typ=p->typf;
    if(c!=ADDRESS&&ISFLOAT(typ)) zreg=f3; else zreg=t3;
  }
  if(q1reg){p->q1.flags=REG;p->q1.reg=q1reg;p->q1.am=0;}
  if(q2reg){p->q2.flags=REG;p->q2.reg=q2reg;p->q2.am=0;}
  return p;
}
static void pr(FILE *f,struct IC *p)
     /*  Writes the destination register to the real destination if necessary.   */
{
  int typ=p->typf;
  if(p->code==ADDRESS) typ=POINTER;
  if(p->z.flags){
    if(zreg>=74&&zreg<=86){printf("c=%d\n",p->code); ierror(0);}
    if(p->z.am){
      store_reg(f,zreg,&p->z,typ);
    }else if(!isreg(z)){
      if(p->z.flags&DREFOBJ){
        if(p->z.flags&REG){
          if(p->z.am)
            store_reg(f,zreg,&p->z,typ);
          else
            store_reg(f,zreg,cam(IMM_IND,p->z.reg,0),typ);
        }else{
          int r;
          if(t1==zreg) r=t2; else r=t1;
          load_reg(f,r,&p->z,POINTER,r);
          store_reg(f,zreg,cam(IMM_IND,r,0),typ);
        }
      }else{
        store_reg(f,zreg,&p->z,typ);
      }
    }else{
      if(p->z.reg!=zreg)
        emit(f,"\t%smr\t%s,%s\n",(zreg>=33&&zreg<=64)?"f":"",mregnames[p->z.reg],mregnames[zreg]);
    }
  }
}
static int dwarf2_regnumber(int r)
{
  if(r==0) ierror(0);
  if(r<=32)
    return r-1;
  else if(r<=64)
    return r+6;
  else
    ierror(0);
}
static zmax dwarf2_fboffset(struct Var *v)
{
  struct obj o;
  if(!v||(v->storage_class!=AUTO&&v->storage_class!=REGISTER)) ierror(0);
  o.flags=VAR;
  o.v=v;
  o.val.vmax=l2zm(0L);
  return l2zm(real_offset(&o));
}
static void dwarf2_print_frame_location(FILE *f,struct Var *v)
{
  struct obj o;
  o.flags=REG;
  o.reg=sp;
  o.val.vmax=l2zm(0L);
  o.v=0;
  dwarf2_print_location(f,&o);
}
static void emit_obj(FILE *f,struct obj *p,int t)
/*  Prints an object.                               */
{
  if(p->am){
    if(p->am->flags&REG_IND) emit(f,"%s,%s",mregnames[p->am->offset],mregnames[p->am->base]);
    if(p->am->flags&IMM_IND) emit(f,"%ld(%s)",p->am->offset,mregnames[p->am->base]);
    return;
  }
  /*  if(p->flags&DREFOBJ) emit(f,"(");*/
  if(p->flags&VAR) {
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER){
      if(p->flags&REG){
        emit(f,"%s",mregnames[p->reg]);
      }else{
        emit(f,"%ld(%s)",real_offset(p),mregnames[fp]);
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
  if((p->flags&REG)&&!(p->flags&VAR)) emit(f,"%s",mregnames[p->reg]);
  if(p->flags&KONST){
    emitval(f,&p->val,t&NU);
  }
  /*  if(p->flags&DREFOBJ) emit(f,")");*/
}
static int exists_freereg(struct IC *p,int reg)
/*  Test if there is a sequence of FREEREGs containing FREEREG reg. */
{
  while(p&&(p->code==FREEREG||p->code==ALLOCREG)){
    if(p->code==FREEREG&&p->q1.reg==reg) return 1;
    p=p->next;
  }
  return 0;
}

static size_t lsize;
static bvtype *once,*twice;

static void peephole(struct IC *p)
/* Try to use addressing modes */
{
  int c,c2,r,cnt,maxlabel,uselr=0;struct IC *p2;struct AddressingMode *am;
  if(cross_module){
    lastlabel=0;
    for(p2=p;p2;p2=p2->next){
      if(p2->code>=LABEL&&p2->code<=BRA){
	if(!lastlabel||p2->typf<lastlabel) lastlabel=p2->typf;
	if(p->typf>maxlabel) maxlabel=p->typf;
      }
    }
  }else{
    maxlabel=label; /*FIXME*/
  }
  /*lsize=((label-lastlabel+1+7)/CHAR_BIT)*CHAR_BIT;*/
  lsize=BVSIZE(label-lastlabel+1);
  once=mymalloc(lsize);
  twice=mymalloc(lsize);
  memset(once,0,lsize);
  memset(twice,0,lsize);
  for(;p;p=p->next){
    c=p->code;
    if(c==CALL){
      if(p->call_list==0){
	uselr=1;
      }else{
	int i;
	for(i=0;i<p->call_cnt;i++)
	  if(!p->call_list[i].v->fi||
	     !p->call_list[i].v->fi->inline_asm||
	     !(p->call_list[i].v->fi->flags&ALL_REGS)||
	     BTST(p->call_list[i].v->fi->regs_modified,lr))
	    uselr=1;
      }
    }
    if((q1typ(p)&NQ)==LLONG&&(c==MULT||c==DIV||c==MOD||c==LSHIFT||c==RSHIFT)) uselr=1;
    if(c==CONVERT&&(q1typ(p)&NQ)==LLONG&&ISFLOAT(ztyp(p)&NQ)) uselr=1;
    if(c==CONVERT&&(ztyp(p)&NQ)==LLONG&&ISFLOAT(q1typ(p)&NQ)) uselr=1;
    if(c!=FREEREG&&c!=ALLOCREG&&(c!=SETRETURN||!isreg(q1)||p->q1.reg!=p->z.reg)) exit_label=0;
    if(c==LABEL) exit_label=p->typf;
    /* Test which labels are jumped to more than once. */
    if((c>=BEQ&&c<=BRA)&&p->typf-lastlabel>=lsize*CHAR_BIT){
      printf("lsize=%lu p->typf=%d lastlabel=%d\n",lsize,p->typf,lastlabel);
      ierror(0);
    }
    if(c>=BEQ&&c<=BRA){
      if(BTST(once,p->typf-lastlabel))
        BSET(twice,p->typf-lastlabel);
      else
        BSET(once,p->typf-lastlabel);
    }
    /* Try test-opt */
    if(c==TEST&&!(p->q1.flags&DREFOBJ)){
      for(p2=p->prev;p2;p2=p2->prev){
        c2=p2->code;
        if(c2==NOP||c2==ALLOCREG||c2==FREEREG) continue;
        if(c2==CALL||(c2>=LABEL&&c2<=BRA)) break;
        if((p2->z.flags&DREFOBJ)&&(p->q1.flags&(REG|DREFOBJ))!=REG) break;
        if(p->q1.flags==p2->z.flags&&p->q1.am==p2->z.am){
          if(!(p->q1.flags&VAR)||(p->q1.v==p2->z.v&&zmeqto(p->q1.val.vmax,p2->z.val.vmax))){
            if(!(p->q1.flags&REG)||p->q1.reg==p2->z.reg){
              if(p->z.flags==0||(isreg(z)&&p->z.reg==cr0)){
                if(p->typf==p2->typf&&(!(p->typf&UNSIGNED)||!multiple_ccs)){
		  p2->ext.setcc=1;
                }
                break;
              }
            }
          }
        }
      }
    }
    /* Try update */
    if((c==ADDI2P||c==SUBIFP)&&isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg){
      if((p->q2.flags&(KONST|DREFOBJ))==KONST){
        eval_const(&p->q2.val,p->typf);
        if(c==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
        if(zmleq(l2zm(-32768L),vmax)&&zmleq(vmax,l2zm(32767L))){
          struct obj *o;
          r=p->q1.reg;cnt=0;o=0;
          for(p2=p->next;p2;p2=p2->next){
            c2=p2->code;
            if(c2==NOP||c2==ALLOCREG||c2==FREEREG) continue;
            if((c2==CALL/*&&regscratch[r]*/)||(c2>=LABEL&&c2<=BRA)) break;
            if((p2->q1.flags&(DREFOBJ|REG))==REG&&p2->q1.reg==r) break;
            if((p2->q2.flags&(DREFOBJ|REG))==REG&&p2->q2.reg==r) break;
            if((p2->z.flags&(DREFOBJ|REG))==REG&&p2->z.reg==r) break;
            if((p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r) cnt|=1;
            if((p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r) cnt|=2;
            if((p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r) cnt|=4;
            if((cnt&3)==3) break;
            if(cnt){
	      if(cnt&1){
		if((q1typ(p2)&NQ)==LLONG) break;
		o=&p2->q1;
	      }else if(cnt&2){
		if((q2typ(p2)&NQ)==LLONG) break;
		o=&p2->q2;
	      }else{
		if((ztyp(p2)&NQ)==LLONG) break;
		o=&p2->z;
	      }
              if(p2->code==ASSIGN&&((p2->typf&NQ)>POINTER||!zmeqto(p2->q2.val.vmax,sizetab[p2->typf&NQ])))
                break;
              o->am=am=mymalloc(sizeof(*am));
              o->am->flags=(IMM_IND|UPDATE);
              o->am->base=r;
              o->am->offset=zm2l(vmax);
              p->code=c=NOP;
              break;
            }
          }
        }
      }
    }
    /* Try const(reg) */
#ifndef oldpeep
    if((c==ADDI2P||c==SUBIFP)&&isreg(z)&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
      int base;zmax of;struct obj *o;
      eval_const(&p->q2.val,p->typf);
      if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
      if(zmleq(l2zm(-32768L),vmax)&&zmleq(vmax,l2zm(32767L))){
	r=p->z.reg;
	if(isreg(q1)) base=p->q1.reg; else base=r;
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
        }
      }
    }
#else
    if((c==ADDI2P||c==SUBIFP)&&(p->q2.flags&(KONST|DREFOBJ))==KONST&&isreg(z)){
      int base;zmax of;
      p2=p->next;
      while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG)) p2=p2->next;
      if(p2) c2=p2->code; else c2=0;
      eval_const(&p->q2.val,p->typf);
      if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
      r=p->z.reg;
      if(isreg(q1)) base=p->q1.reg; else base=r;
      if(c2&&zmleq(l2zm(-32768L),of)&&zmleq(of,l2zm(32767L))&&c2!=CALL&&(c2<LABEL||c2>BRA)
         &&(c2!=ASSIGN||((p2->typf&NQ)<=POINTER&&zmeqto(p2->q2.val.vmax,sizetab[p2->typf&NQ])))
         &&c2!=ADDRESS&&(((p2->z.flags&(DREFOBJ|REG))==REG&&p2->z.reg==r&&p2->q2.flags==0)||exists_freereg(p2->next,r))){
        if(((p2->q1.flags&(REG|DREFOBJ))!=REG||p2->q1.reg!=r)
           &&((p2->q2.flags&(REG|DREFOBJ))!=REG||p2->q2.reg!=r)){
          cnt=0;
          if((p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            p2->q1.am=am=mymalloc(sizeof(*am));
            p2->q1.am->flags=IMM_IND;
            p2->q1.am->base=base;
            p2->q1.am->offset=zm2l(of);
            cnt++;
          }
          if((p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            p2->q2.am=am=mymalloc(sizeof(*am));
            p2->q2.am->flags=IMM_IND;
            p2->q2.am->base=base;
            p2->q2.am->offset=zm2l(of);
            cnt++;
          }
          if((p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            p2->z.am=am=mymalloc(sizeof(*am));
            p2->z.am->flags=IMM_IND;
            p2->z.am->base=base;
            p2->z.am->offset=zm2l(of);
            cnt++;
          }
          if(isreg(q1)){
            p->code=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
          }else{
            p->code=ASSIGN;p->q2.flags=0;
          }
          if(cnt==1&&((p2->q1.flags&(DREFOBJ|REG))!=REG||p2->q1.reg!=base)
             &&((p2->q2.flags&(DREFOBJ|REG))!=REG||p2->q2.reg!=base)
             &&((p2->z.flags&(DREFOBJ|REG))!=REG||p2->z.reg!=base) ){
            /* Can we use update? */
            p2=p2->next;
            while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG)) p2=p2->next;
            if(p2){
              c2=p2->code;
              if(c2==ADDI2P||c2==SUBIFP){
                if((p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==base
                   &&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==base
                   &&(p2->q2.flags&(KONST|DREFOBJ))==KONST ){
                  eval_const(&p2->q2.val,p2->typf);
                  if(c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
                  if(zmeqto(vmax,of)){
                    am->flags|=UPDATE;
                    p2->code=NOP;
                  }
                }
              }
            }
          }
          continue;
        }
      }
    }
#endif
    /* Try reg,reg */
#ifndef oldpeep
    if(c==ADDI2P&&isreg(q2)&&isreg(z)&&(isreg(q1)||p->q2.reg!=p->z.reg)){
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
            if(o||(q1typ(p2)&NQ)==LLONG) break;
            o=&p2->q1;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            if(o||(q2typ(p2)&NQ)==LLONG) break;
            o=&p2->q2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            if(o||(ztyp(p2)&NQ)==LLONG) break;
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
              am->flags=REG_IND;
              am->base=base;
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
      }
    }
#else
    if(c==ADDI2P&&isreg(q2)&&isreg(z)&&p->q2.reg!=p->z.reg){
      int base,idx;
      p2=p->next;
      while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG)) p2=p2->next;
      if(p2) c2=p2->code; else c2=0;
      r=p->z.reg;idx=p->q2.reg;
      if(isreg(q1)) base=p->q1.reg; else base=r;
      if(c2&&c2!=CALL&&(c2<LABEL||c2>BRA)
         &&(c2!=ASSIGN||((p2->typf&NQ)<=POINTER&&zmeqto(p2->q2.val.vmax,sizetab[p2->typf&NQ])))
         &&c2!=ADDRESS&&exists_freereg(p2->next,r)){
        if(((p2->q1.flags&(REG|DREFOBJ))!=REG||p2->q1.reg!=r)
           &&((p2->q2.flags&(REG|DREFOBJ))!=REG||p2->q2.reg!=r) ){
          cnt=0;
          if((p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            p2->q1.am=am=mymalloc(sizeof(*am));
            p2->q1.am->flags=REG_IND;
            p2->q1.am->base=base;
            p2->q1.am->offset=idx;
            cnt++;
          }
          if((p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            p2->q2.am=am=mymalloc(sizeof(*am));
            p2->q2.am->flags=REG_IND;
            p2->q2.am->base=base;
            p2->q2.am->offset=idx;
            cnt++;
          }
          if((p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            p2->z.am=am=mymalloc(sizeof(*am));
            p2->z.am->flags=REG_IND;
            p2->z.am->base=base;
            p2->z.am->offset=idx;
            cnt++;
          }
          if(isreg(q1)){
            p->code=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
          }else{
            p->code=ASSIGN;p->q2.flags=0;
          }
          if(cnt==1&&((p2->q1.flags&(DREFOBJ|REG))!=REG||p2->q1.reg!=base)
             &&((p2->q2.flags&(DREFOBJ|REG))!=REG||p2->q2.reg!=base)
             &&((p2->z.flags&(DREFOBJ|REG))!=REG||p2->z.reg!=base) ){
            /* Can we use update? */
            p2=p2->next;
            while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG)) p2=p2->next;
            if(p2){
              c2=p2->code;
              if(c2==ADDI2P){
                if((p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==base
                   &&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==base
                   &&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==idx){
                  am->flags|=UPDATE;
                  p2->code=NOP;
                }
              }
            }
          }
          continue;
        }
      }
    }
#endif
  }
  if(!uselr) function_calls=0;
}

static void toc_entry(FILE *f,struct Var *v)
/*  Create a toc-entry.  */
{
  if(!use_sd(v->vtyp->flags)||(v->tattr&FAR)){
    if(v->storage_class==STATIC&&!ISFUNC(v->vtyp->flags)){
      emit(f,tocname);
      emit(f,"%s%s%ld:\n",tocprefix,labprefix,zm2l(v->offset));
      emit(f,"\t.long\t%s%ld\n",labprefix,zm2l(v->offset));
    }else{
      if(!ISFUNC(v->vtyp->flags)){
        emit(f,tocname);
        emit(f,"%s%s%s:\n",tocprefix,idprefix,v->identifier);
        emit(f,"\t.long\t%s%s\n",idprefix,v->identifier);
      }
      if(v->storage_class==EXTERN)
        emit(f,"\t.global\t%s%s%s\n",tocprefix,idprefix,v->identifier);
    }
    if(f) section=TOC;
  }
}

static int savereg(struct Var *v,int i)
{
  if(v->tattr&(SYSCALL|NOSAVE)) return 0;
  if(vlas&&i==fp) return 1;
  if((i==sd&&(v->tattr&SAVEDS))||(regused[i]&&!regscratch[i]&&!regsa[i])
     ||((v->tattr&SAVEALL)&&i<=32&&(regscratch[i]||i==t1||i==t2||i==t3)&&((!v->fi)||(!(v->fi->flags&ALL_REGS))||BTST(v->fi->regs_modified,i)) ))
    return 1;
  return 0;
}

static int stmw,stme;

static void function_top(FILE *f,struct Var *v,long offset)
/*  Generates function top.                             */
{
  int i,preg;long of;
  if(POWEROPEN) toc_entry(f,v);
  if(mregnames[1]!=regnames[1]) emit(f,"#vsc elf\n");
  if(g_flags[0]&USEDFLAG) emit(f,"#vsc cpu %s\n",g_flags_val[0].p);
  if(g_flags[1]&USEDFLAG) emit(f,"#vsc fpu %s\n",g_flags_val[1].p);
  if(!special_section(f,v)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
  if(!GAS) emit(f,"\t.sdreg\t%s\n",mregnames[sd]);
  if(!optsize) emit(f,"\t.align\t4\n");
  if(v->storage_class==EXTERN){
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier);
  }else
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
  frameoffset=minframe+maxpushed;
  framesize=frameoffset+offset;
  stme=0;
  for(i=1;i<=64;i++){
    if(savereg(v,i)){
      if(i<=32) framesize+=4; else framesize+=8;
      if(i<=32&&stme==0) stme=i;
    }else{
      if(i<=32) stme=0;
    }
  }
  if(stme==32||!(g_flags[11]&USEDFLAG)) stme=0;
  for(crsave=0,i=65;i<=72;i++)
    if((regused[i]&&!regscratch[i]&&!regsa[i])) crsave=1;
  if(crsave&&!POWEROPEN) framesize+=4;
  if(framesize==minframe&&((v->tattr&(SYSCALL))||(function_calls==0&&!crsave))) framesize=frameoffset=0;
  if(EABI)
    framesize=(framesize+7)/8*8;
  else
    framesize=(framesize+15)/16*16;
  stack=framesize;
  stack_valid=1;
  if(v->tattr&SYSCALL){
    emit(f,"#barrier\n");
    emit(f,"\tmtspr\t81,0\n");
    emit(f,"#barrier\n");
    emit(f,"\tstwu\t%s,-4(%s)\n",mregnames[t1],mregnames[sp]);
    emit(f,"\tmflr\t%s\n",mregnames[t1]);
    emit(f,"\tstwu\t%s,-4(%s)\n",mregnames[t1],mregnames[sp]);
    emit(f,"\tbl\t%s__syscall_init\n",idprefix);
    /*FIXME: das koennte man evtl. noch sparen */
    emit(f,"\taddi\t%s,%s,-8\n",mregnames[sp],mregnames[sp]);
  }else if(v->tattr&NOSAVE){
    /* nothing */
  }else if(function_calls||(stack_check&&framesize)){
    emit(f,"\tmflr\t%s\n",mregnames[t1]);
    BSET(regs_modified,t1);
  }
  if(stack_check&&framesize){
    BSET(regs_modified,t2);
    if(framesize<=32767){
      emit(f,"\tli\t%s,%ld\n",mregnames[t2],framesize);
    }else{
      emit(f,"\tlis\t%s,%ld\n",mregnames[t2],hi(framesize));
      emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t2],mregnames[t2],lo(framesize));
    }     
    emit(f,"\tbl\t%s__stack_check\n",idprefix);
    if(!function_calls){
      emit(f,"\tmtlr\t%s\n",mregnames[t1]);
      BSET(regs_modified,lr);
    }
  }
  if(function_calls&&!(v->tattr&(SYSCALL|NOSAVE))&&framesize>=32760){
    if(POWEROPEN)
      emit(f,"\tst%s\t%s,8(%s)\n",sdt(LONG),mregnames[t1],mregnames[sp]);
    else
      emit(f,"\tst%s\t%s,4(%s)\n",sdt(LONG),mregnames[t1],mregnames[sp]);
  }
  of=minframe+maxpushed+offset;
  if(framesize!=0){
    if(framesize<=32767){
      emit(f,"\tstwu\t%s,-%ld(%s)\n",mregnames[sp],framesize,mregnames[sp]);
      preg=sp;
    }else{
      BSET(regs_modified,t1);
      BSET(regs_modified,t2);
      emit(f,"\tmr\t%s,%s\n",mregnames[t2],mregnames[sp]);
      emit(f,"\tlis\t%s,%ld\n",mregnames[t1],hi(-framesize));
      emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t1],mregnames[t1],lo(-framesize));
      emit(f,"\tstwux\t%s,%s,%s\n",mregnames[sp],mregnames[sp],mregnames[t1]);
      preg=t2;of-=framesize;
    }
  }
  if(crsave&&!(v->tattr&NOSAVE)){
    BSET(regs_modified,t3);
    if(POWEROPEN){
      emit(f,"\tmfcr\t%s\n\tst%s\t%s,4(%s)\n",mregnames[t3],sdt(LONG),mregnames[t3],mregnames[preg]);
    }else{
      emit(f,"\tmfcr\t%s\n\tst%s\t%s,%ld(%s)\n",mregnames[t3],sdt(LONG),mregnames[t3],of,mregnames[preg]);
      of+=4;
    }
  }
  for(i=1;i<=64;i++){
    if(savereg(v,i)){
      if(i<=32){
	if(i==stme){
          emit(f,"\tstmw\t%s,%ld(%s)\n",mregnames[stme],of,mregnames[preg]);
          of+=(32-stme+1)*4;
          i=32;
        }else{
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(LONG),mregnames[i],of,mregnames[preg]);
          of+=4;
        }
      }else{
        emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(DOUBLE),mregnames[i],of,mregnames[preg]);
        of+=8;
      }
    }
  }
  if(function_calls&&!(v->tattr&(SYSCALL|NOSAVE))&&framesize<32760){
    if(POWEROPEN)
      emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(LONG),mregnames[t1],framesize+8,mregnames[sp]);
    else
      emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(LONG),mregnames[t1],framesize+4,mregnames[sp]);
  }
  if((v->tattr&SAVEDS)&&(SMALLDATA||BASERELMOS||POWEROPEN)){
    if(POWEROPEN){
      emit(f,"\t.global\t%s__getr2\n",idprefix);
      if(!function_calls)
        emit(f,"\tmflr\t%s\n",mregnames[t2]);
      emit(f,"\tbl\t%s__getr2\n",idprefix);
      if(!function_calls){
        emit(f,"\tmtlr\t%s\n",mregnames[t2]);
	BSET(regs_modified,lr);
      }
    }else{
      if(BASERELMOS){
        emit(f,"\t.global\t%s__restore_r13\n",idprefix);
        emit(f,"\tbl\t%s__restore_r13\n",idprefix);
      }else{
        emit(f,"\tlis\t%s,%s_SDA_BASE_@ha\n",mregnames[sd],idprefix);
        emit(f,"\taddi\t%s,%s,%s_SDA_BASE_@l\n",mregnames[sd],mregnames[sd],idprefix);
      }
    }
  }
  if(v->tattr&BREL){
    if(BASERELOS4){
      emit(f,"\t.global\t%s__baserel_get_addr\n",idprefix);
      emit(f,"\tbl\t%s__baserel_get_addr\n",idprefix);
    }
  }
  if(vlas){
    emit(f,"\tmr\t%s,%s\n",mregnames[fp],mregnames[sp]);
    emit(f,"\t.set\t____fo,%ld\n",frameoffset);
  }
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  Generates function bottom.                          */
{
  int i,preg;long of;
  if(v->tattr&SYSCALL){
    emit(f,"\tb\t%s__dispatch\n",idprefix);
  }else{
    of=minframe+maxpushed+offset;
    if(framesize<=32767){
      preg=sp;
    }else{
      emit(f,"\tlwz\t%s,0(%s)\n",mregnames[t2],mregnames[sp]);
      preg=t2;of-=framesize;
    }
    if(crsave&&!(v->tattr&NOSAVE)){
      if(POWEROPEN){
	emit(f,"\tl%s\t%s,8(%s)\n\tmtcr\t%s\n",ldt(LONG),mregnames[t1],mregnames[preg],mregnames[t1]);
      }else{
	emit(f,"\tl%s\t%s,%ld(%s)\n\tmtcr\t%s\n",ldt(LONG),mregnames[t1],of,mregnames[preg],mregnames[t1]);
	of+=4;
      }
    }
    if(function_calls&&!(v->tattr&NOSAVE)&&framesize<32760){
      BSET(regs_modified,lr);
      if(POWEROPEN)
	emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(LONG),mregnames[t1],framesize+8,mregnames[sp]);
      else
	emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(LONG),mregnames[t1],framesize+4,mregnames[sp]);
    }
    for(i=1;i<=64;i++){
      if(savereg(v,i)){
	if(i<=32){
	  if(i==stme){
	    emit(f,"\tlmw\t%s,%ld(%s)\n",mregnames[stme],of,mregnames[preg]);
	    of+=(32-stme+1)*4;
	    i=32;
	  }else{
	    emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(LONG),mregnames[i],of,mregnames[preg]);
	    of+=4;
	  }
	}else{
	  emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(DOUBLE),mregnames[i],of,mregnames[preg]);
	  of+=8;
	}
      }
    }
    if(framesize){
      if(framesize<=32767)
	emit(f,"\taddi\t%s,%s,%ld\n",mregnames[sp],mregnames[sp],framesize);
      else
	emit(f,"\tmr\t%s,%s\n",mregnames[sp],mregnames[preg]);
    }
    if(function_calls&&!(v->tattr&NOSAVE)){
      if(framesize<32760){
	emit(f,"\tmtlr\t%s\n",mregnames[t1]);
      }else{
	BSET(regs_modified,lr);
	if(POWEROPEN)
	  emit(f,"\tl%s\t%s,8(%s)\n\tmtlr\t%s\n",ldt(LONG),mregnames[t1],mregnames[sp],mregnames[t1]);
	else
	  emit(f,"\tl%s\t%s,4(%s)\n\tmtlr\t%s\n",ldt(LONG),mregnames[t1],mregnames[sp],mregnames[t1]);
      }
    }
    emit(f,ret);
  }
  if(v->storage_class==EXTERN){
    emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
    emit(f,"\t.size\t%s%s,$-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
  }else{
    emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,$-%s%ld\n",labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
  }    
  if(all_regs&&v->fi) v->fi->flags|=ALL_REGS;
}

static int balign(struct obj *o)
/*  Liefert die unteren 2 Bits des Objekts. 1 wenn unklar. */
{
  int sc;
  if(o->flags&DREFOBJ) return 1;
  if(o->am) ierror(0);
  if(!(o->flags&VAR)) ierror(0);
  sc=o->v->storage_class;
  if(sc==EXTERN||sc==STATIC){
    /* Alle statischen Daten werden vom cg auf 32bit alignt. */
    return zm2l(zmand(o->val.vmax,l2zm(3L)));
  }
  if(sc==AUTO||sc==REGISTER){
    zmax of=o->v->offset;
    if(!zmleq(l2zm(0L),of))
      of=zmsub(l2zm(0L),zmadd(of,maxalign));
    return zm2l(zmand(zmadd(of,o->val.vmax),l2zm(3L)));
  }
  ierror(0);
}

/* load hiword of a long long object */
static void load_hword(FILE *f,int r,struct obj *o,int t,int tmp)
{
  struct rpair rp;
  BSET(regs_modified,r);
  if(o->flags&KONST){
    static struct obj cobj;
    cobj.flags=KONST;
    eval_const(&o->val,t);
    vumax=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffffff)); 
    cobj.val.vulong=zum2zul(vumax);
    load_reg(f,r,&cobj,UNSIGNED|LONG,tmp);
  }else if(!o->am&&(o->flags&DREFOBJ)){
    if(!(o->flags&REG)) ierror(0);
    emit(f,"\tlwz\t%s,0(%s)\n",mregnames[r],mregnames[o->reg]);
  }else if(!o->am&&(o->flags&REG)){
    struct rpair rp;
    if(!reg_pair(o->reg,&rp)) 
      ierror(0);
    if(rp.r1!=r)
      emit(f,"\tmr\t%s,%s\n",mregnames[r],mregnames[rp.r1]);
  }else{
    load_reg(f,r,o,UNSIGNED|LONG,tmp);
  }
}

/* load loword of a long long object */
static void load_lword(FILE *f,int r,struct obj *o,int t,int tmp)
{
  struct rpair rp;
  BSET(regs_modified,r);
  if(o->flags&KONST){
    static struct obj cobj;
    cobj.flags=KONST;
    eval_const(&o->val,t);
    vumax=zumand(vumax,ul2zum(0xffffffff)); 
    cobj.val.vulong=zum2zul(vumax);
    load_reg(f,r,&cobj,UNSIGNED|LONG,tmp);
  }else if(o->am){
    if(o->am->flags!=IMM_IND) ierror(0);
    o->am->offset+=4;
    load_reg(f,r,o,UNSIGNED|LONG,tmp);
    o->am->offset-=4;   
  }else if(o->flags&DREFOBJ){
    if(!(o->flags&REG)) ierror(0);
    emit(f,"\tlwz\t%s,4(%s)\n",mregnames[r],mregnames[o->reg]);
  }else if(o->flags&REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    if(rp.r2!=r)
      emit(f,"\tmr\t%s,%s\n",mregnames[r],mregnames[rp.r2]);
  }else{
    o->val.vmax=zmadd(o->val.vmax,l2zm(4L));
    load_reg(f,r,o,UNSIGNED|LONG,tmp);
    o->val.vmax=zmsub(o->val.vmax,l2zm(4L));
  }
}    
/* store hiword of a long long object */
static void store_hword(FILE *f,int r,struct obj *o)
{
  struct rpair rp;
  if(!o->am&&(o->flags&DREFOBJ)){
    if(!(o->flags&REG)) ierror(0);
    emit(f,"\tstw\t%s,0(%s)\n",mregnames[r],mregnames[o->reg]);
  }else if(!o->am&&(o->flags&REG)){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    if(rp.r1!=r)
      emit(f,"\tmr\t%s,%s\n",mregnames[rp.r1],mregnames[r]);
  }else{
    store_reg(f,r,o,UNSIGNED|LONG);
  }
}

/* store loword of a long long object */
static void store_lword(FILE *f,int r,struct obj *o)
{
  struct rpair rp;
  if(o->am){
    if(o->am->flags!=IMM_IND) ierror(0);
    o->am->offset+=4;
    store_reg(f,r,o,UNSIGNED|LONG);
    o->am->offset-=4;
  }else if(o->flags&DREFOBJ){
    if(!(o->flags&REG)) ierror(0);
    emit(f,"\tstw\t%s,4(%s)\n",mregnames[r],mregnames[o->reg]);
  }else if(o->flags&REG){
    if(!reg_pair(o->reg,&rp)) ierror(0);
    if(rp.r2!=r)
      emit(f,"\tmr\t%s,%s\n",mregnames[rp.r2],mregnames[r]);
  }else{
    o->val.vmax=zmadd(o->val.vmax,l2zm(4L));
    store_reg(f,r,o,UNSIGNED|LONG);
    o->val.vmax=zmsub(o->val.vmax,l2zm(4L));
  }
}
/* if object cannot be dereferenced with a single load, load its address
   in register r (!=r0) and modify the object */
static void create_loadable(FILE *f,struct obj *o,int r)
{
  struct rpair rp;
  if(o->am) return;
  if((o->flags&(REG|DREFOBJ))==DREFOBJ){
    o->flags&=~DREFOBJ;
    load_reg(f,r,o,POINTER,r);
    o->flags|=(REG|DREFOBJ);
    o->reg=r;
  }
  if((o->flags&(VAR|REG))==VAR){
    if((o->v->storage_class==STATIC||o->v->storage_class==EXTERN)&&(!use_sd(o->v->vtyp->flags)||(o->v->tattr&FAR))){
      load_address(f,r,o,POINTER);
      o->reg=r;
      o->flags=(REG|DREFOBJ);
    }
    if((o->v->storage_class==AUTO||o->v->storage_class==REGISTER)&&real_offset(o)>32760){
      load_address(f,r,o,POINTER);
      o->reg=r;
      o->flags=(REG|DREFOBJ);
    }
  }
}

static int get_reg()
{
  int i;
  for(i=2;i<=32;i++){
    if(!regs[i]&&(regscratch[i]||regused[i]))
      break;
  }
  if(i<=32){
    BSET(regs_modified,i);
    return i;
  }
  return 0;
}

static int handle_llong(FILE *f,struct IC *p)
{
  int c=p->code,t,savemask=0;char *libfuncname;
  int msp;long mtmpoff;

  t=(ztyp(p)&NU);

  if(c==ADDRESS) return 0;


  if(c==GETRETURN){
    create_loadable(f,&p->z,t1);
    if(!reg_pair(p->q1.reg,&rp)) ierror(0);
    store_hword(f,rp.r1,&p->z);
    store_lword(f,rp.r2,&p->z);
    p->z.flags=0;
    return 1;
  }

  if(c==SETRETURN){
    create_loadable(f,&p->q1,t1);
    if(!reg_pair(p->z.reg,&rp)) ierror(0);
    load_hword(f,rp.r1,&p->q1,q1typ(p),t2);
    load_lword(f,rp.r2,&p->q1,q1typ(p),t2);
    p->z.flags=0;
    return 1;
  }

  if(c==CONVERT&&(q1typ(p)&NQ)==LLONG&&(ztyp(p)&NQ)==LLONG){
    p->code=c=ASSIGN;
    p->q2.val.vmax=sizetab[LLONG];
  }

  if(c==ASSIGN||c==PUSH){
    int r;
    create_loadable(f,&p->q1,t1);
    if(c==ASSIGN){
      create_loadable(f,&p->z,t2);
    }else{
      pushed=(pushed+3)/4*4;
      if(align_arguments)
	pushed=(pushed+7)/8*8;
    }
    if(isreg(z)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      load_hword(f,rp.r1,&p->q1,q1typ(p),t1);
    }else{
      if(isreg(q1)){
	if(!reg_pair(p->q1.reg,&rp)) ierror(0);
	r=rp.r1;
      }else{
	if(p->q1.flags&KONST)
	  r=t1;
	else
	  r=t3;
	load_hword(f,r,&p->q1,q1typ(p),t1);
      }
      if(c==ASSIGN){
	store_hword(f,r,&p->z);
      }else{
	emit(f,"\tstw\t%s,%ld(%s)\n",mregnames[r],pushed+minframe,mregnames[sp]);
	pushed+=4;
      }
    }
    if(isreg(z)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      load_lword(f,rp.r2,&p->q1,q1typ(p),t1);
    }else{
      if(isreg(q1)){
	if(!reg_pair(p->q1.reg,&rp)) ierror(0);
	r=rp.r2;
      }else{
	if(p->q1.flags&KONST)
	  r=t1;
	else
	  r=t3;
	load_lword(f,r,&p->q1,q1typ(p),t1);
      }
      if(c==ASSIGN){
	store_lword(f,r,&p->z);
      }else{
	emit(f,"\tstw\t%s,%ld(%s)\n",mregnames[r],pushed+minframe,mregnames[sp]);
	pushed+=4;
      }    
    }
    p->z.flags=0;
    return 1;
  }

  if(c==CONVERT&&(t&NQ)<LLONG){
    if(isreg(q1)){
      if(!reg_pair(p->q1.reg,&rp)) ierror(0);
      zreg=rp.r2;
    }else{
      int r;
      if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==DREFOBJ)
        create_loadable(f,&p->q1,t1);
      if(isreg(z))
        r=p->z.reg;
      else
        r=t1;
      load_lword(f,r,&p->q1,q1typ(p),t2);
      zreg=r;
    }
    return 1;               
  }
  if(c==CONVERT&&(q1typ(p)&NQ)<LLONG){
    int zl,zh,told=q1typ(p);
    if(isreg(z)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      zh=rp.r1;
      zl=rp.r2;
    }else{
      zl=t2;
      zh=t3;
    }
    if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==DREFOBJ)
      create_loadable(f,&p->q1,t1);
    load_reg(f,zl,&p->q1,q1typ(p),t1);
    if(told&UNSIGNED){
      if((told&NQ)==CHAR)
	emit(f,"\tclrlwi\t%s,%s,24\n",mregnames[zl],mregnames[zl]);
      if((told&NQ)==SHORT)
	emit(f,"\tclrlwi\t%s,%s,16\n",mregnames[zl],mregnames[zl]);
      emit(f,"\tli\t%s,0\n",mregnames[zh]);
    }else{
      if((told&NQ)==CHAR)
	emit(f,"\textsb\t%s,%s\n",mregnames[zl],mregnames[zl]);
      if((told&NQ)==SHORT)
	emit(f,"\textsh\t%s,%s\n",mregnames[zl],mregnames[zl]);
      emit(f,"\tsrawi\t%s,%s,31\n",mregnames[zh],mregnames[zl]);
    }
    create_loadable(f,&p->z,t1);
    store_lword(f,zl,&p->z);
    store_hword(f,zh,&p->z);
    p->z.flags=0;
    return 1;
  }

  if(c==TEST){
    p->code=c=COMPARE;
    if(p->typf&UNSIGNED)
      p->q2.val.vullong=zum2zull(ul2zum(0UL));
    else
      p->q2.val.vllong=zm2zll(l2zm(0L));
    p->q2.flags=KONST;
  }

  if(c==KOMPLEMENT){
    p->code=c=XOR;
    p->q2.flags=KONST;
    if(p->typf&UNSIGNED)
      p->q2.val.vullong=tu_max[LLONG];
    else
      p->q2.val.vllong=l2zm(-1L);
  }
  if(c==MINUS){
    p->code=c=SUB;
    p->q2=p->q1;
    p->q1.flags=KONST;
    p->q1.am=0;
    if(p->typf&UNSIGNED)
      p->q1.val.vullong=ul2zum(0UL);
    else
      p->q1.val.vllong=l2zm(0L);
  }    

  switch_IC(p);

  if(c==COMPARE){
    int l1,l2,h1,h2,tmp,falselab=++label,c;
    zumax lo,uhi;zmax shi;
    char *sh;
    struct IC *b;
    if(multiple_ccs)
      ierror(0); /* still needed? */
    else
      p->z.reg=cr0;
    b=p->next;
    while(b->code==ALLOCREG||b->code==FREEREG) b=b->next;
    c=b->code;
    if(c<BEQ||c>BGT) ierror(0);
    if(p->typf&UNSIGNED)
      sh="cmplw";
    else
      sh="cmpw";
    if((c==BNE||c==BEQ)&&(p->q2.flags&KONST)&&p->z.reg==cr0){
      eval_const(&p->q2.val,q2typ(p));
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))){
	if(isreg(q1)){
	  if(!reg_pair(p->q1.reg,&rp)) ierror(0);
	  emit(f,"\tor.\t%s,%s,%s\n",mregnames[t1],mregnames[rp.r1],mregnames[rp.r2]);
	}else{
	  create_loadable(f,&p->q1,t1);
	  load_lword(f,t3,&p->q1,q1typ(p),t2);
	  load_hword(f,t1,&p->q1,q1typ(p),t2);
	  emit(f,"\tor.\t%s,%s,%s\n",mregnames[t3],mregnames[t3],mregnames[t1]);
	}
	return 1;
      }
    }
    h1=h2=l1=l2=0;
    if(isreg(q1)){
      if(!reg_pair(p->q1.reg,&rp)) ierror(0);
      h1=rp.r1;
      l1=rp.r2;
    }else{
      create_loadable(f,&p->q1,t1);
      h1=l1=t3;
    }
    if(isreg(q2)){
      if(!reg_pair(p->q2.reg,&rp)) ierror(0);
      h2=rp.r1;
      l2=rp.r2;      
    }else if(p->q2.flags&KONST){
      eval_const(&p->q2.val,q2typ(p));
      lo=zumrshift(zumlshift(vumax,ul2zum(32UL)),ul2zum(32UL));
      if(zumleq(lo,ul2zum(65535L)))
	l2=-1; 
      if(p->typf&UNSIGNED){
	uhi=zumrshift(vumax,ul2zum(32UL));
	if(zumleq(uhi,ul2zum(65535L)))
	  h2=-1; 
      }else{
	shi=zmrshift(vmax,l2zm(32L));
	if(zmleq(shi,l2zm(32767L))&&zmleq(l2zm(-32768L),shi))
	  h2=-1;
      }
    }
    if(h2==0||l2==0){
      if(!(p->q1.flags&REG)||p->q1.reg!=t1){
	create_loadable(f,&p->q2,t1);
      }else if(tmp=get_reg()){
	create_loadable(f,&p->q2,tmp);
      }
      if(h2==0) h2=t2;
      if(l2==0) l2=t2;
    }
    load_hword(f,h1,&p->q1,q1typ(p),0);
    if(h2==-1){
      emit(f,"\t%si\t%s,%s,",sh,mregnames[p->z.reg],mregnames[h1]);
      if(p->typf&UNSIGNED)
	emitzum(f,uhi);
      else
	emitzm(f,shi);
      emit(f,"\n");
    }else{
      load_hword(f,h2,&p->q2,q2typ(p),t2);
      emit(f,"\t%s\t%s,%s,%s\n",sh,mregnames[p->z.reg],mregnames[h1],mregnames[h2]);
    }
    if(c==BGT||c==BGE){
      emit(f,"\tbgt\t%s,%s%d\n",mregnames[p->z.reg],labprefix,b->typf);
      emit(f,"\tblt\t%s,%s%d\n",mregnames[p->z.reg],labprefix,falselab);
    }else if(c==BLT||c==BLE){
      emit(f,"\tblt\t%s,%s%d\n",mregnames[p->z.reg],labprefix,b->typf);
      emit(f,"\tbgt\t%s,%s%d\n",mregnames[p->z.reg],labprefix,falselab);
    }else if(c!=BNE){
      emit(f,"\tbne\t%s,%s%d\n",mregnames[p->z.reg],labprefix,falselab);
    }else{
      emit(f,"\tbne\t%s,%s%d\n",mregnames[p->z.reg],labprefix,b->typf);
    }

    load_lword(f,l1,&p->q1,q1typ(p),t1);
    if(l2==-1){
      emit(f,"\tcmplwi\t%s,%s,",mregnames[p->z.reg],mregnames[l1]);
      emitzm(f,lo);
      emit(f,"\n");
    }else{
      load_lword(f,l2,&p->q2,q2typ(p),t2);
      emit(f,"\tcmplw\t%s,%s,%s\n",mregnames[p->z.reg],mregnames[l1],mregnames[l2]);
    }   
    emit(f,"\tb%s\t%s,%s%d\n",ccs[c-BEQ],mregnames[p->z.reg],labprefix,b->typf);
    emit(f,"%s%d:\n",labprefix,falselab);
    b->code=NOP;
	   
    return 1;
  }

  if(c==ADD||c==SUB||c==AND||c==OR||c==XOR){
    /*FIXME: q2==z, q1reg+q2reg=zreg */
    int zl,zh,l1,l2,h1,h2,tmp;
    char *sl,*sh;
    if(c==ADD){
      sl="addc";
      sh="adde";
    }else if(c==SUB){
      /* there is no subc, therefore we always use reverse order */
      sl="subfc";
      sh="subfe";
    }else if(c==AND){
      sl=sh="and";
    }else if(c==OR){
      sl=sh="or";
    }else if(c==XOR){
      sl=sh="xor";
    }else
      ierror(0);
    l1=l2=h1=h2=zl=zh=0;
    create_loadable(f,&p->q1,t1);
    if(isreg(q1)){
      if(!reg_pair(p->q1.reg,&rp)) ierror(0);
      h1=rp.r1;
      l1=rp.r2;
    }else{
      create_loadable(f,&p->q1,t1);
      l1=t3;
      h1=t1;
    }
    if(isreg(q2)){
      if(!reg_pair(p->q2.reg,&rp)) ierror(0);
      h2=rp.r1;
      l2=rp.r2;
    }else if(p->q2.flags&KONST){
      /* check for immediates */
      if(c==OR||c==XOR){
	/* or and xor can always be done using (x)ori and (x)oris */
	h2=l2=-1;
      }
      if(c==ADD){
	zmax tmp;
	eval_const(&p->q2.val,q2typ(p));
	tmp=zmrshift(vmax,l2zm(32L));
	/* there are addze and addme instructions */
	if(zmeqto(tmp,l2zm(0L))||zmeqto(tmp,l2zm(-1L)))
	  h2=-1;	
	/* addic supports 16bit signed values */
	tmp=zmrshift(zmlshift(vmax,l2zm(32L)),l2zm(32L));	
	if(zmleq(tmp,l2zm(32767L))&&zmleq(l2zm(-32768L),tmp))
	  l2=-1;
      }
      if(c==SUB){
	zmax tmp;
	eval_const(&p->q2.val,q2typ(p));
	tmp=zmrshift(vmax,l2zm(32L));
	/* there are addze and addme instructions */
	if(zmeqto(tmp,l2zm(0L))||zmeqto(tmp,l2zm(1L)))
	  h2=-1;	
	/* addic supports 16bit signed values */
	tmp=zmrshift(zmlshift(vmax,l2zm(32L)),l2zm(32L));	
	if(zmleq(tmp,l2zm(32768L))&&zmleq(l2zm(-32767L),tmp))
	  l2=-1;
      }	
    }
    if(!l2||!h2){
      if(!(p->q1.flags&REG)||p->q1.reg!=t1){
	create_loadable(f,&p->q2,t1);
      }else if(tmp=get_reg()){
	create_loadable(f,&p->q2,tmp);
      }
      if(h2==0) h2=t2;
      if(l2==0) l2=t2;
    }
    if(isreg(z)&&(!isreg(q2)||p->z.reg!=p->q2.reg)){
      if(!reg_pair(p->z.reg,&rp)) ierror(0);
      zh=rp.r1;
      zl=rp.r2;
    }else{
      zl=t3;
      zh=t1;
    }
    if((p->q1.flags&KONST)&&l1==t3){
      load_lword(f,t2,&p->q1,q1typ(p),0);
      emit(f,"\tmr\t%s,%s\n",mregnames[t3],mregnames[t2]);
      BSET(regs_modified,t3);
    }else
      load_lword(f,l1,&p->q1,q1typ(p),0);
    if(l2!=-1){
      load_lword(f,l2,&p->q2,q2typ(p),t2);
      emit(f,"\t%s\t%s,%s,%s\n",sl,mregnames[zl],mregnames[l2],mregnames[l1]);
    }else{
      eval_const(&p->q2.val,q2typ(p));
      if(c==ADD||c==SUB){
	if(c==SUB) vmax=zmsub(l2zm(0L),vmax);
	vmax=zmrshift(zmlshift(vmax,l2zm(32L)),l2zm(32L));
	emit(f,"\taddic\t%s,%s,",mregnames[zl],mregnames[l1]);
	emitzm(f,vmax);
	emit(f,"\n");
      }else if(c==OR||c==XOR){
	zumax tmp;
	if(zumeqto(zumand(vumax,ul2zum(0xffffffffUL)),ul2zum(0xffffffffUL))){
	  if(c==XOR)
	    emit(f,"\tnor\t%s,%s,%s\n",mregnames[zl],mregnames[l1],mregnames[l1]);
	  else
	    emit(f,"\tli\t%s,-1\n",mregnames[zl]);
	}else{
	  tmp=zumand(vumax,ul2zum(0xffffUL));
	  if(!zumeqto(tmp,ul2zum(0UL))){
	    emit(f,"\t%si\t%s,%s,",sl,mregnames[zl],mregnames[l1]);
	    emitzum(f,tmp);
	    emit(f,"\n");
	    l1=zl;
	  }
	  tmp=zumand(zumrshift(vumax,ul2zum(16UL)),ul2zum(0xffff));
	  if(!zumeqto(tmp,ul2zum(0UL))){
	    emit(f,"\t%sis\t%s,%s,",sl,mregnames[zl],mregnames[l1]);
	    emitzum(f,tmp);
	    emit(f,"\n");
	    l1=zl;
	  }
	  if(l1!=zl)
	    emit(f,"\tmr\t%s,%s\n",mregnames[zl],mregnames[l1]);
	}
      }else
	ierror(0);
    }
    if(h2!=-1){
      load_hword(f,h2,&p->q2,q2typ(p),t2);    
      load_hword(f,h1,&p->q1,q1typ(p),0);
      emit(f,"\t%s\t%s,%s,%s\n",sh,mregnames[zh],mregnames[h2],mregnames[h1]);
    }else{
      load_hword(f,h1,&p->q1,q1typ(p),0);
      eval_const(&p->q2.val,q2typ(p));
      if(c==ADD||c==SUB){
	if(c==SUB) vmax=zmsub(l2zm(0L),vmax);
	vmax=zmrshift(vmax,l2zm(32L));
	if(zmeqto(vmax,l2zm(0L))){
	  emit(f,"\taddze\t%s,%s\n",mregnames[zh],mregnames[h1]);
	}else if(zmeqto(vmax,l2zm(-1L))){
	  emit(f,"\taddme\t%s,%s\n",mregnames[zh],mregnames[h1]);
	}else
	  ierror(0);
      }else if(c==OR||c==XOR){
	zumax tmp;
	if(zumeqto(zumand(zumrshift(vumax,ul2zum(32L)),ul2zum(0xffffffffUL)),ul2zum(0xffffffffUL))){
	  if(c==XOR)
	    emit(f,"\tnor\t%s,%s,%s\n",mregnames[zh],mregnames[h1],mregnames[h1]);
	  else
	    emit(f,"\tli\t%s,-1\n",mregnames[zh]);
	}else{
	  tmp=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffUL));
	  if(!zumeqto(tmp,ul2zum(0UL))){
	    emit(f,"\t%si\t%s,%s,",sl,mregnames[zh],mregnames[h1]);
	    emitzum(f,tmp);
	    emit(f,"\n");
	    h1=zh;
	  }
	  tmp=zumand(zumrshift(vumax,ul2zum(48UL)),ul2zum(0xffff));
	  if(!zumeqto(tmp,ul2zum(0UL))){
	    emit(f,"\t%sis\t%s,%s,",sl,mregnames[zh],mregnames[h1]);
	    emitzum(f,tmp);
	    emit(f,"\n");
	    h1=zh;
	  }	
	  if(h1!=zh)
	    emit(f,"\tmr\t%s,%s\n",mregnames[zh],mregnames[h1]);
	}
      }
    }
    if(p->z.flags){
      create_loadable(f,&p->z,t2);
      store_lword(f,zl,&p->z);
      store_hword(f,zh,&p->z);
      p->z.flags=0;
    }
    return 1;
  }

  create_loadable(f,&p->q1,t1);

  if(tmpoff>=32768&&(regs[r3]||regs[r4]||regs[r5]||regs[r6])){
    emit(f,"\taddis\t%s,%s,%ld\n",mregnames[t2],mregnames[sp],hi(tmpoff));
    if(lo(tmpoff)) emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t2],mregnames[t2],lo(tmpoff));
    msp=t2;
    mtmpoff=0;
  }else{
    msp=sp;
    mtmpoff=tmpoff;
  }

  if(regs[r3]){ emit(f,"\tstw\t%s,%ld(%s)\n",mregnames[r3],mtmpoff-4,mregnames[msp]);savemask|=1;}
  if(regs[r4]){ emit(f,"\tstw\t%s,%ld(%s)\n",mregnames[r4],mtmpoff-8,mregnames[msp]);savemask|=2;}
  if(regs[r5]) {emit(f,"\tstw\t%s,%ld(%s)\n",mregnames[r5],mtmpoff-12,mregnames[msp]);savemask|=4;}
  if(regs[r6]) {emit(f,"\tstw\t%s,%ld(%s)\n",mregnames[r6],mtmpoff-16,mregnames[msp]);savemask|=8;}

  if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q1.reg>=r3&&p->q1.reg<=r6){
    emit(f,"\tmr\t%s,%s\n",mregnames[t1],mregnames[p->q1.reg]);
    p->q1.reg=t1;
  }
  if((p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q2.reg>=r3&&p->q2.reg<=r6){
    emit(f,"\tmr\t%s,%s\n",mregnames[t2],mregnames[p->q2.reg]);
    p->q2.reg=t2;
  }
  
  if((q1typ(p)&NQ)==LLONG){
    if(p->q1.am&&p->q1.am->base==r3){
      if(p->q1.am->flags==REG_IND&&p->q1.am->offset==r4) ierror(0);
      load_lword(f,r4,&p->q1,q1typ(p),0);
      load_hword(f,r3,&p->q1,q1typ(p),0);
    }else{
      if(p->q1.am&&p->q1.am->flags==REG_IND&&p->q1.am->offset==r3) ierror(0);
      load_hword(f,r3,&p->q1,q1typ(p),0);
      load_lword(f,r4,&p->q1,q1typ(p),0);
    }
  }else{
    if(!ISFLOAT(q1typ(p))) ierror(0);
    load_reg(f,f1,&p->q1,q1typ(p),0); 
  }
  
  if(p->q2.flags){
    create_loadable(f,&p->q2,t2);
    if((q2typ(p)&NQ)==LLONG){
      if(isreg(q2)&&p->q2.reg==r3r4){
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r5],mtmpoff-4,mregnames[msp]);
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r6],mtmpoff-8,mregnames[msp]);
      }else{
	if(p->q2.am&&p->q2.am->base==r5){
	  if(p->q2.am->flags==REG_IND&&p->q2.am->offset==r6) ierror(0);
	  load_lword(f,r6,&p->q2,q2typ(p),0);
	  load_hword(f,r5,&p->q2,q2typ(p),0);
	}else{
	  if(p->q2.am&&p->q2.am->flags==REG_IND&&p->q2.am->offset==r5) ierror(0);
	  load_hword(f,r5,&p->q2,q2typ(p),0);
	  load_lword(f,r6,&p->q2,q2typ(p),0);
	}
      }
    }else{
      if((q2typ(p)&NQ)>=LLONG) ierror(0);
      if(isreg(q2)&&p->q2.reg==r3){
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r5],mtmpoff-4,mregnames[msp]);
      }else{
	load_reg(f,r5,&p->q2,q2typ(p),0);
      }      
    }
  }

  if(c==MULT) libfuncname="__mulint64";
  else if(c==DIV){
    if(t&UNSIGNED)
      libfuncname="__divuint64";
    else
      libfuncname="__divsint64";
  }else if(c==MOD){
    if(t&UNSIGNED)
      libfuncname="__moduint64";
    else
      libfuncname="__modsint64";
  }else if(c==CONVERT){
    static char s[32];
    if(ISFLOAT(q1typ(p)))
      sprintf(s,"__flt%dto%cint64",(q1typ(p)&NQ)==FLOAT?32:64,(ztyp(p)&UNSIGNED)?'u':'s');
    else
      sprintf(s,"__%cint64toflt%d",(q1typ(p)&UNSIGNED)?'u':'s',(ztyp(p)&NQ)==FLOAT?32:64);
    libfuncname=s;
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
  emit(f,"\t.global\t%s%s\n",idprefix,libfuncname);
  emit(f,"\tbl\t%s%s\n",idprefix,libfuncname);
  stack_valid=0; /*FIXME*/

  if((p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
    if((p->z.reg==r3&&(savemask&1))||
       (p->z.reg==r4&&(savemask&2))||
       (p->z.reg==r5&&(savemask&4))||
       (p->z.reg==r6&&(savemask&8))){
      if(p->z.reg==r3)
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[t1],mtmpoff-4,mregnames[msp]);
      else if(p->z.reg==r4)
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[t1],mtmpoff-8,mregnames[msp]);
      else if(p->z.reg==r5)
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[t1],mtmpoff-12,mregnames[msp]);
      else if(p->z.reg==r6)
	emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[t1],mtmpoff-16,mregnames[msp]);
      else
	ierror(0);
      p->z.reg=t1;
    }
  }

  if((p->z.flags&REG)&&p->z.reg==r3)
    savemask&=~1;
  if((p->z.flags&REG)&&p->z.reg==r4)
    savemask&=~2;
  if((p->z.flags&REG)&&p->z.reg==r5)
    savemask&=~4;
  if((p->z.flags&REG)&&p->z.reg==r6)
    savemask&=~8;
  if((p->z.flags&REG)&&p->z.reg==r3r4)
    savemask&=~3;
  if((p->z.flags&REG)&&p->z.reg==r5r6)
    savemask&=~12;


  if(ISFLOAT(ztyp(p))){
    zreg=f1;
  }else{
    create_loadable(f,&p->z,t1);
    store_hword(f,r3,&p->z);
    store_lword(f,r4,&p->z);
    p->z.flags=0;
  }

  if(savemask&1)
    emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r3],mtmpoff-4,mregnames[msp]);
  if(savemask&2)
    emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r4],mtmpoff-8,mregnames[msp]);
  if(savemask&4)
    emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r5],mtmpoff-12,mregnames[msp]);
  if(savemask&8)
    emit(f,"\tlwz\t%s,%ld(%s)\n",mregnames[r6],mtmpoff-16,mregnames[msp]);
  
  return 1;
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
  if(POWEROPEN){
    stackalign=l2zm(4L);
    align_arguments=0;
  }else{
    tocname=sdataname;
    stackalign=l2zm(4L);
    if(NOALIGNARGS)
      align_arguments=0;
    else
      align_arguments=1;
  }
  char_bit=l2zm(8L);
  if(g_flags[7]&USEDFLAG){
    malign[INT]=malign[LONG]=malign[LLONG]=malign[POINTER]=malign[FLOAT]=malign[DOUBLE]=malign[LDOUBLE]=2;
  }
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i=0;i<=MAXR;i++) mregnames[i]=regnames[i];
  for(i= 1;i<=32;i++){
    regsize[i]=l2zm(4L);
    regtype[i]=&ltyp;
    if(g_flags[8]&USEDFLAG) mregnames[i]++;
  }
  for(i=33;i<=64;i++){
    regsize[i]=l2zm(8L);
    regtype[i]=&ldbl;
    if(g_flags[8]&USEDFLAG) mregnames[i]++;
  }
  for(i=65;i<=72;i++){
    regsize[i]=l2zm(1L);
    regtype[i]=&lchar;
    if(g_flags[8]&USEDFLAG) mregnames[i]+=2;
  }
  for(i=74;i<=86;i++){
    regsize[i]=l2zm(8L);
    regtype[i]=&lltyp;
  }

  /*  Use multiple ccs.   */
  multiple_ccs=1;

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
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=ul2zum(4294967295UL);
  tu_max[LONG]=t_max(UNSIGNED|INT);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);

  /*  Reserve a few registers for use by the code-generator.      */
  /*  This is not optimal but simple.                             */
  if(POWEROPEN){
    sd=r2;
  }
  regsa[t1]=regsa[t2]=regsa[t3]=1;
  regsa[f1]=regsa[f2]=regsa[f3]=1;
  regsa[sp]=regsa[fp]=regsa[sd]=regsa[r2]=1;
  regscratch[t1]=regscratch[t2]=regscratch[t3]=0;
  regscratch[f1]=regscratch[f2]=regscratch[f3]=0;
  regscratch[sp]=regscratch[fp]=regscratch[sd]=regscratch[r2]=0;

  if(g_flags[6]&USEDFLAG) {minframe=8;labprefix=".l";idprefix="";}
  if(POWEROPEN) {minframe=24;labprefix="l";idprefix="_";}

  if(optsize){
    dataname="\t.data\n";
    rodataname="\t.section\t.rodata\n";
    sdataname="\t.section\t\".sdata\",\"aw\"\n";
    sdata2name="\t.section\t\".sdata2\",\"a\"\n";
    sbssname="\t.section\t\".sbss\",\"auw\"\n";
  }

  if(BASERELOS4) bssname="\t.bss\n";

  target_macros=marray;

  declare_builtin("__mulint64",LLONG,LLONG,r3r4,LLONG,r5r6,1,0);
  declare_builtin("__divsint64",LLONG,LLONG,r3r4,LLONG,r5r6,1,0);
  declare_builtin("__divuint64",UNSIGNED|LLONG,UNSIGNED|LLONG,r3r4,UNSIGNED|LLONG,r5r6,1,0);
  declare_builtin("__modsint64",LLONG,LLONG,r3r4,LLONG,r5r6,1,0);
  declare_builtin("__moduint64",UNSIGNED|LLONG,UNSIGNED|LLONG,r3r4,UNSIGNED|LLONG,r5r6,1,0);
  declare_builtin("__lshint64",LLONG,LLONG,r3r4,INT,r5,1,0);
  declare_builtin("__rshsint64",LLONG,LLONG,r3r4,INT,r5,1,0);
  declare_builtin("__rshuint64",LLONG,UNSIGNED|LLONG,r3r4,INT,r5,1,0);

  return 1;
}

void init_db(FILE *f)
{
  dwarf2_setup(sizetab[POINTER],".byte",".2byte",".4byte",".4byte",labprefix,idprefix,".section");
  dwarf2_print_comp_unit_header(f);
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
/*  A pointer MUST be returned in a register. The code-generator    */
/*  has to simulate a pseudo register if necessary.                 */
{
  if(ISFLOAT(t->flags)) return 34; /* f1 */
    if(ISSTRUCT(t->flags)||ISUNION(t->flags)) return 0;
    if(zmleq(szof(t),l2zm(4L))) return r3; 
    if((t->flags&NQ)==LLONG) return 74; /* r3/r4 */
    return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  if(r<74||r>86)
    return 0;
  switch(r){
  case 74: p->r1=4;p->r2=5;return 1;
  case 75: p->r1=6;p->r2=7;return 1;
  case 76: p->r1=8;p->r2=9;return 1;
  case 77: p->r1=10;p->r2=11;return 1;
  case 78: p->r1=15;p->r2=16;return 1;
  case 79: p->r1=17;p->r2=18;return 1;
  case 80: p->r1=19;p->r2=20;return 1;
  case 81: p->r1=21;p->r2=22;return 1;
  case 82: p->r1=23;p->r2=24;return 1;
  case 83: p->r1=25;p->r2=26;return 1;
  case 84: p->r1=27;p->r2=28;return 1;
  case 85: p->r1=29;p->r2=30;return 1;
  case 86: p->r1=31;p->r2=32;return 1;
  }
  ierror(0);
}


int cost_savings(struct IC *p,int r,struct obj *o)
{
  int c=p->code;
  if(o->flags&VKONST){
    struct obj *co=&o->v->cobj;
    int longload;
    if(o->flags&DREFOBJ)
      return 4;
    if(ISFLOAT(p->typf)) return 2;
    longload=0;
    if((co->flags&VARADR)&&!use_sd(o->v->vtyp->flags))
      longload=2;
    if(co->flags&KONST){
      eval_const(&co->val,p->typf);
      if(p->typf&UNSIGNED){
	if(!zumleq(vumax,ul2zum(65535UL)))
	  longload=2;
      }else{
	if(!zmleq(vmax,l2zm(32767L))||zmleq(vmax,l2zm(-32769L)))
	  longload=2;
      }
    }
    if(o==&p->q1&&p->code==ASSIGN&&((p->z.flags&DREFOBJ)||p->z.v->storage_class==STATIC||p->z.v->storage_class==EXTERN))
      return longload+2;
    else
      return longload;
  }
  if(o->flags&DREFOBJ)
    return 4;
  if(c==SETRETURN&&r==p->z.reg&&!(o->flags&DREFOBJ)) return 3;
  if(c==GETRETURN&&r==p->q1.reg&&!(o->flags&DREFOBJ)) return 3;
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
    if(t==0){
        if(r>=65&&r<=72) return 1; else return 0;
    }
    if(ISFLOAT(t)&&r>=33&&r<=64) return 1;
    if(t==POINTER&&r>=1&&r<=32) return 1;
    if(t>=CHAR&&t<=LONG&&r>=1&&r<=32) return 1;
    if(t==LLONG&&r>=74&&r<=86) return 1;
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
    if((op==INT||op==LONG||op==POINTER)&&(tp==INT||tp==LONG||tp==POINTER))
      return 0;
    if(op==DOUBLE&&tp==LDOUBLE) return 0;
    if(op==LDOUBLE&&tp==DOUBLE) return 0;
    if(op==tp) return 0;
    return 1;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  title(f);
  if(newobj&&section!=TOC&&section!=SBSS&&section!=SPECIAL&&!BASERELOS4)
    emit(f,"%ld\n",zm2l(size));
  else
    emit(f,"\t.space\t%ld\n",zm2l(size));
  newobj=0;
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  title(f);
  if(!optsize&&(zm2l(align)<4))
    emit(f,"\t.align\t2\n");
  else
    if(zm2l(align)>1)
      emit(f,"\t.align\t%ld\n",pof2(align)-1);
}

void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;char *sec;
  title(f);
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->storage_class==STATIC){
    if(ISFUNC(v->vtyp->flags)) return;
    if(!special_section(f,v)){
      if(use_sd(v->vtyp->flags)&&!(v->tattr&FAR)){
	if(EABI){
	  if(v->clist&&!constflag&&section!=SDATA){emit(f,sdataname);if(f) section=SDATA;}
	  if(v->clist&&constflag&&section!=SDATA2){emit(f,sdata2name);if(f) section=SDATA2;}
	  if(!v->clist&&section!=SBSS){emit(f,sbssname);if(f) section=SBSS;}
	}else if (POWEROPEN){
	  if(section!=TOC){emit(f,tocname);if(f) section=TOC;}
	}
        else {
	  if(v->clist&&section!=SDATA){emit(f,sdataname);if(f) section=SDATA;}
	  if(!v->clist&&section!=SBSS){emit(f,sbssname);if(f) section=SBSS;}
        }
      }else{
        if(BASERELOS4){
          if(v->clist&&section!=DATA){emit(f,dataname); if(f) section=DATA;}
        }else if(BASERELMOS){
          if(v->clist&&section!=SDATA){emit(f,sdataname); if(f) section=SDATA;}
          if(!v->clist&&section!=SBSS){emit(f,sbssname); if(f) section=SBSS;}
        }else{
          if(POWEROPEN) toc_entry(f,v);
          if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
          if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)&&section!=RODATA){emit(f,rodataname);if(f) section=RODATA;}
	  if(!USE_COMMONS&&!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
        }
      }
    }
    if(v->clist||section==TOC||section==SBSS||section==SPECIAL){
      emit(f,"\t.type\t%s%ld,@object\n",labprefix,zm2l(v->offset));
      emit(f,"\t.size\t%s%ld,%ld\n",labprefix,zm2l(v->offset),zm2l(szof(v->vtyp)));
      gen_align(f,falign(v->vtyp));
      emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    }else{
      emit(f,"\t.lcomm\t%s%ld,",labprefix,zm2l(v->offset));
      newobj=1;
    }
  }
  if(v->storage_class==EXTERN){
    emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    if(POWEROPEN&&(!use_sd(v->vtyp->flags)||(v->tattr&FAR)))
      emit(f,"\t.global\t%s%s%s\n",tocprefix,idprefix,v->identifier);
    if(v->flags&(DEFINED|TENTATIVE)){
      if(!special_section(f,v)){
	if(use_sd(v->vtyp->flags)&&!(v->tattr&FAR)){
	  if(EABI){
	    if(v->clist&&!constflag&&section!=SDATA){emit(f,sdataname);if(f) section=SDATA;}
	    if(v->clist&&constflag&&section!=SDATA2){emit(f,sdata2name);if(f) section=SDATA2;}
	    if(!v->clist&&section!=SBSS){emit(f,sbssname);if(f) section=SBSS;}
	  }else if (POWEROPEN){
	    if(section!=TOC){emit(f,tocname);if(f) section=TOC;}
	  }
          else {
	    if(v->clist&&section!=SDATA){emit(f,sdataname);if(f) section=SDATA;}
	    if(!USE_COMMONS&&!v->clist&&section!=SBSS){emit(f,sbssname);if(f) section=SBSS;}
          }
	}else{
          if(BASERELOS4){
            if(v->clist&&section!=DATA){emit(f,dataname); if(f) section=DATA;}
          }else if(BASERELMOS){
            if(v->clist&&section!=SDATA){emit(f,sdataname); if(f) section=SDATA;}
            if(!USE_COMMONS&&!v->clist&&section!=SBSS){emit(f,sbssname); if(f) section=SBSS;}
          }else{
            if(POWEROPEN) toc_entry(f,v);
            if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
            if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)&&section!=RODATA){emit(f,rodataname);if(f) section=RODATA;}
	    if(!USE_COMMONS&&!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
          }
	}
      }
      if(v->clist||section==TOC||section==SBSS||section==SPECIAL){
        emit(f,"\t.type\t%s%s,@object\n",idprefix,v->identifier);
        emit(f,"\t.size\t%s%s,%ld\n",idprefix,v->identifier,zm2l(szof(v->vtyp)));
	gen_align(f,falign(v->vtyp));
        emit(f,"%s%s:\n",idprefix,v->identifier);
      }else{
        emit(f,"\t.%scomm\t%s%s,",USE_COMMONS?"":"l",idprefix,v->identifier);
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
  if((t&NQ)==POINTER) t=UNSIGNED|LONG;
  emit(f,"\t.%s\t",dct[t&NQ]);
  if(!p->tree){
    if(ISFLOAT(t)){
      /*  auch wieder nicht sehr schoen und IEEE noetig   */
      unsigned char *ip;
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
      if((t&NQ)!=FLOAT){
	emit(f,",0x%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
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
    emit_obj(f,&p->tree->o,t&NU);
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
/*  The main code-generation.                                           */
{
  int c,t,i,addbuf,varargs=0,fixedgpr,fixedfpr,setcc,ccset;
  char *fpp;int fpf;struct IC *m;
  long of=(zm2l(offset)+3)/4*4,regbase,vparmos=0;
  if(DEBUG&1) printf("gen_code()\n");
  for(c=1;c<=MAXR;c++) regs[c]=regsa[c];
  if(vlas) fp=vlafp; else fp=sp;
  maxpushed=0;addbuf=0;
  if(v->tattr&RFI)
    ret="\trfi\n";
  else
    ret="\tblr\n";
  if(!v->fi) v->fi=new_fi();
  title(f);
  for(m=p;m;m=m->next){
    c=m->code;t=m->typf&NU;
    m->ext.setcc=0;
    if(c==ALLOCREG){
      allocreg(m->q1.reg);
      continue;
    }
    if(c==FREEREG){
      freereg(m->q1.reg);
      continue;
    }
    if(c==COMPARE&&(m->q2.flags&KONST)&&(t&NQ)!=CHAR&&(t&NQ)!=SHORT){
      eval_const(&m->q2.val,t);
      if(zmeqto(vmax,l2zm(0L))&&zldeqto(vldouble,d2zld(0.0))){
        m->q2.flags=0;m->code=c=TEST;
      }
    }
    if((t&NQ)<=LLONG&&(m->q2.flags&KONST)&&(t&NQ)<=LLONG&&(c==MULT||((c==DIV||c==MOD)&&(t&UNSIGNED)))){
      eval_const(&m->q2.val,t);
      i=pof2(vmax);
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
	  insert_const(&m->q2.val,t);
	}else{
	  insert_const(&m->q2.val,INT);
	  m->typf2=INT;
	}
      }
    }
    if(addbuf<16){
      if((q1typ(m)&NQ)==LLONG&&(c==MULT||c==DIV||c==MOD||c==LSHIFT||c==RSHIFT)) {addbuf=16;function_calls++;}
      if(c==CONVERT&&(q1typ(m)&NQ)==LLONG&&ISFLOAT(ztyp(m)&NQ)){addbuf=16;function_calls++;}
      if(c==CONVERT&&(ztyp(m)&NQ)==LLONG&&ISFLOAT(q1typ(m)&NQ)){addbuf=16;function_calls++;}
      if(addbuf<8){
	if(c==CONVERT&&ISFLOAT(m->typf2)&&!ISFLOAT(t)) addbuf=8;
	if(c==CONVERT&&!ISFLOAT(m->typf2)&&ISFLOAT(t)) addbuf=8;
      }
    }
    if(c==CALL&&maxpushed<zm2l(m->q2.val.vmax)) maxpushed=zm2l(m->q2.val.vmax);
    if(c==CALL&&(m->q1.flags&VAR)&&!strcmp(m->q1.v->identifier,"__va_start")) varargs=1;
  }
  once=twice=0;
  if(!(g_flags[9]&USEDFLAG)) peephole(p);
  if(varargs){
    fixedgpr=fixedfpr=0;
    if(!freturn(v->vtyp->next)) fixedgpr++;
    for(i=0;i<v->vtyp->exact->count;i++){
      c=(*v->vtyp->exact->sl)[i].styp->flags&NQ;
      if(fixedgpr<8&&(c==POINTER||c<=LONG))
	fixedgpr++;
      else if(fixedgpr<7&&c==LLONG)
	fixedgpr=(fixedgpr+3)/2*2;
      else if(fixedfpr<8&&ISFLOAT(c)) 
	fixedfpr++;
      else{
	
	vparmos+=zm2l(szof((*v->vtyp->exact->sl)[i].styp));
	vparmos=(vparmos+zm2l(stackalign)-1)/zm2l(stackalign)*zm2l(stackalign);
      }
    }
    regbase=of;
    addbuf+=96;
  }
  for(c=1;c<=MAXR;c++){
    if((!regsa[c])&&regused[c]){
      BSET(regs_modified,c);
    }
  }
  of+=addbuf;tmpoff=minframe+maxpushed+of;
  function_top(f,v,of);
  all_regs=1;
  if(varargs){
    regbase=frameoffset+regbase;
    fpp="";
    if(!(g_flags[8]&USEDFLAG)) fpp="r";
    for(i=fixedgpr;i<8;i++)
      emit(f,"\tstw\t%s%d,%ld(%s)\n",fpp,i+3,regbase+i*4,mregnames[sp]);
    if(!(g_flags[8]&USEDFLAG)) fpp="f";
    for(i=fixedfpr;i<8;i++)
      emit(f,"\tstfd\t%s%d,%ld(%s)\n",fpp,i+1,regbase+32+i*8,mregnames[sp]);
  }
  pushed=0;ccset=0;
  for(;p;pr(f,p),p=p->next){
    c=p->code;t=p->typf;
    setcc=p->ext.setcc;
    if(debug_info)
      dwarf2_line_info(f,p);
    if(c==NOP) {p->z.flags=0;continue;}
    if(c==ALLOCREG){
      allocreg(p->q1.reg);
      continue;
    }
    if(c==FREEREG){
      freereg(p->q1.reg);
      continue;
    }
    if(c==COMPARE&&(p->q2.flags&KONST)&&(t&NQ)!=LLONG){
      struct case_table *ct=calc_case_table(p,JUMP_TABLE_DENSITY);
      if(ct&&(ct->num>=JUMP_TABLE_LENGTH||(!isreg(q1)&&ct->num>=JUMP_TABLE_LENGTH/2))){
	int r,defl,tabl=++label;
	long l;unsigned long ul;
	struct IC *np;
	for(np=p;np!=ct->next_ic;np=np->next){
	  if(np->code==ALLOCREG) allocreg(np->q1.reg);
	  if(np->code==FREEREG) freereg(np->q1.reg);
	}
	if(ct->next_ic->code==BRA)
	  defl=ct->next_ic->typf;
	else
	  defl=++label;
	if(!isreg(q1)){
	  if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==DREFOBJ){
	    p->q1.flags&=~DREFOBJ;
	    load_reg(f,t1,&p->q1,POINTER,t1);
	    p->q1.flags|=(REG|DREFOBJ);
	    p->q1.flags&=~VAR;
	    p->q1.reg=t1;
	  }
	  load_reg(f,t2,&p->q1,p->typf,t1);
	  r=t2;
	}else
	  r=p->q1.reg;
	if(t&UNSIGNED)
	  l=-(long)zum2ul(ct->min.vumax);
	else
	  l=-zm2l(ct->min.vmax);
	if(l>=-32767&&l<=32767){
	  emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t2],mregnames[r],l);
	}else{
	  emit(f,"\taddis\t%s,%s,%ld\n",mregnames[t2],mregnames[r],hi(l));
	  if(lo(l))
	    emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t2],mregnames[t2],lo(l));
	}
	ul=zum2ul(ct->diff);
	if(regs[cr0]) ierror(0);
	if(ul<=65535){
	  emit(f,"\tcmplwi\t%s,%lu\n",mregnames[t2],ul);
	}else{
	  emit(f,"\tlis\t%s,%ld\n",mregnames[t1],hi(ul));
	  if(lo(ul))
	    emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t1],regnames[t1],lo(ul));
	}
	emit(f,"\tbgt\t%s,%s%d\n",mregnames[cr0],labprefix,defl);
	emit(f,"\tslwi\t%s,%s,2\n",mregnames[t2],mregnames[t2]);
	/*FIXME: small-data */
	if(POWEROPEN){
	  emit(f,"\tlwz\t%s,%s%s%d(%s)\n",mregnames[t1],tocprefix,labprefix,tabl,mregnames[r2]);
	}else{
	  emit(f,"\tlis\t%s,%s%d@ha\n",mregnames[t1],labprefix,tabl);
	  emit(f,"\taddi\t%s,%s,%s%d@l\n",mregnames[t1],mregnames[t1],labprefix,tabl);
	}
	emit(f,"\tlwzx\t%s,%s,%s\n",mregnames[t2],mregnames[t1],mregnames[t2]);
	emit(f,"\tmtctr\t%s\n",mregnames[t2]);
	emit(f,"\tbctr\n");
	BSET(regs_modified,ctr);
	if(POWEROPEN){
	  emit(f,tocname);
	  emit(f,"%s%s%d:\n",tocprefix,labprefix,tabl);
	  emit(f,"\t.long\t%s%d\n",labprefix,tabl);
	}
	emit(f,rodataname);
	emit(f,"\t.align\t2\n");
	emit(f,"%s%d:\n",labprefix,tabl);
	emit_jump_table(f,ct,"\t.long\t",labprefix,defl);
	if(!special_section(f,v)) emit(f,codename);
	if(ct->next_ic->code!=BRA){
	  emit(f,"%s%d:\n",labprefix,defl);
	  p=ct->next_ic->prev;
	}else
	  p=ct->next_ic;
	continue;
      }
    }
    if(c==LABEL) {ccset=0;emit(f,"%s%d:\n",labprefix,t);continue;}
    if(c==BRA){
      ccset=0;
      if(t==exit_label&&framesize==0)
	emit(f,ret);
      else
	emit(f,"\tb\t%s%d\n",labprefix,t);
      continue;
    }
    if(c>=BEQ&&c<BRA){
      ccset=0;
      if(!(p->q1.flags&REG)) p->q1.reg=cr0;
      if(!(g_flags[9]&USEDFLAG)&&!BTST(twice,p->typf-lastlabel)){
        struct IC *p2,*p3,*p4;int exit_label;
        p2=p->next;
        while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG)) p2=p2->next;
        if(p2&&p2->code==SETRETURN&&p2->z.reg){p2->code=ASSIGN;p2->z.flags=REG;}
        if(p2&&p2->code==ASSIGN&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg<=32){
          p3=p2->next;
          while(p3&&(p3->code==FREEREG||p3->code==ALLOCREG)) p3=p3->next;
          if(p3&&p3->code==BRA){
            exit_label=p3->typf;
            p3=p3->next;
            while(p3&&(p3->code==FREEREG||p3->code==ALLOCREG)) p3=p3->next;
            if(p3&&p3->code==LABEL&&p3->typf==p->typf){
              p3=p3->next;
              while(p3&&(p3->code==FREEREG||p3->code==ALLOCREG)) p3=p3->next;
              if(p3&&p3->code==SETRETURN&&p3->z.reg){p3->code=ASSIGN;p3->z.flags=REG;}
              if(p3&&p3->code==ASSIGN&&(p3->z.flags&(REG|DREFOBJ))==REG&&p3->z.reg==p2->z.reg){
                p4=p3->next;
                while(p4&&(p4->code==FREEREG||p4->code==ALLOCREG)) p4=p4->next;
                if(p4&&p4->code==LABEL&&p4->typf==exit_label){
                  int bit=(p->q1.reg-cr0)*4;
                  if((p2->q1.flags&KONST)&&(p3->q1.flags&KONST)){
                    eval_const(&p2->q1.val,p2->typf);
                    if(zmeqto(vmax,l2zm(0L))){
                      eval_const(&p3->q1.val,p3->typf);
                      if(zmeqto(vmax,l2zm(1L))||zmeqto(vmax,l2zm(-1L))){
                        if(c==BLE){emit(f,"\tcror\t%d,%d,%d\n",bit,bit,bit+2);}
                        if(c==BGE){bit++;emit(f,"\tcror\t%d,%d,%d\n",bit,bit,bit+1);}
                        if(c==BNE){bit+=2;emit(f,"\tcrnor\t%d,%d,%d\n",bit,bit,bit);}
                        if(c==BGT) bit++;
                        if(c==BEQ) bit+=2;
                        emit(f,"\tmfcr\t%s\n",mregnames[t1]);
                        emit(f,"\trlwinm\t%s,%s,%d,%d,%d\n",mregnames[p2->z.reg],mregnames[t1],bit+1,31,31);
                        if(zmeqto(vmax,l2zm(-1L))) emit(f,"\tneg\t%s,%s\n",mregnames[p2->z.reg],mregnames[p2->z.reg]);
                        if(BTST(twice,p4->typf-lastlabel)) emit(f,"%s%d:\n",labprefix,p4->typf);
                        p=p4;continue;
                      }
                    }else{
                      eval_const(&p3->q1.val,p3->typf);
                      if(zmeqto(vmax,l2zm(0L))){
                        eval_const(&p2->q1.val,p2->typf);
                        if(zmeqto(vmax,l2zm(1L))||zmeqto(vmax,l2zm(-1L))){
                          if(c==BLE){emit(f,"\tcrnor\t%d,%d,%d\n",bit,bit,bit+2);}
                          if(c==BGE){bit++;emit(f,"\tcrnor\t%d,%d,%d\n",bit,bit,bit+1);}
                          if(c==BNE){bit+=2;}
                          if(c==BGT){bit++;emit(f,"\tcrnor\t%d,%d,%d\n",bit,bit,bit);}
                          if(c==BEQ){bit+=2;emit(f,"\tcrnor\t%d,%d,%d\n",bit,bit,bit);}
                          if(c==BLT){emit(f,"\tcrnor\t%d,%d,%d\n",bit,bit,bit);}
                          emit(f,"\tmfcr\t%s\n",mregnames[t1]);
                          emit(f,"\trlwinm\t%s,%s,%d,%d,%d\n",mregnames[p2->z.reg],mregnames[t1],bit+1,31,31);
                          if(zmeqto(vmax,l2zm(-1L))) emit(f,"\tneg\t%s,%s\n",mregnames[p2->z.reg],mregnames[p2->z.reg]);
                          if(BTST(twice,p4->typf-lastlabel)) emit(f,"%s%d:\n",labprefix,p4->typf);
                          p=p4;continue;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      if(t==exit_label&&framesize==0&&!strcmp(ret,"\tblr\n"))
	emit(f,"\tb%slr\t%s\n",ccs[c-BEQ],mregnames[p->q1.reg]);
      else
	emit(f,"\tb%s\t%s,%s%d\n",ccs[c-BEQ],mregnames[p->q1.reg],labprefix,t);
      continue;
    }
    if(c==MOVETOREG){
      if(p->z.reg<=32){
        load_reg(f,p->z.reg,&p->q1,INT,t1);
      }else if(p->z.reg>=33&&p->z.reg<=64){
        load_reg(f,p->z.reg,&p->q1,DOUBLE,t1);
      }else if(reg_pair(p->z.reg,&rp)){
	create_loadable(f,&p->q1,t1);
	load_hword(f,rp.r1,&p->q1,LLONG,t2);
	load_lword(f,rp.r2,&p->q1,LLONG,t2);
      }else
	ierror(0);
      p->z.flags=0;
      continue;
    }
    if(c==MOVEFROMREG){
      if(p->q1.reg<=32){
        store_reg(f,p->q1.reg,&p->z,INT);
      }else if(p->q1.reg>=33&&p->q1.reg<=64){
        store_reg(f,p->q1.reg,&p->z,DOUBLE);
      }else if(reg_pair(p->q1.reg,&rp)){
	create_loadable(f,&p->z,t1);
	store_hword(f,rp.r1,&p->z);
	store_lword(f,rp.r2,&p->z);
      }else
	ierror(0);
      p->z.flags=0;
      continue;
    }
    if((p->q1.flags&&(q1typ(p)&NQ)==LLONG)||(p->q2.flags&&(q2typ(p)&NQ)==LLONG)||(p->z.flags&&(ztyp(p)&NQ)==LLONG))
      if(handle_llong(f,p))
	continue;
    if((c==ASSIGN||c==PUSH)&&((t&NQ)>POINTER||((t&NQ)==CHAR&&zm2l(p->q2.val.vmax)!=1))){
      unsigned long size,l;
      int a1,a2,b;char *ld,*st;
      size=zm2l(p->q2.val.vmax);
      a1=balign(&p->q1);
      if(c==ASSIGN) a2=balign(&p->z); else a2=0;
      b=1;ld=ldt(CHAR);st=sdt(CHAR);
      if(p->q1.flags&VAR){
	if(p->q1.flags&DREFOBJ){
	  if(p->q1.v->vtyp->next&&zmeqto(p->q2.val.vmax,szof(p->q1.v->vtyp->next))&&(a1&1)){
	    a1=zm2l(falign(p->q1.v->vtyp->next))&3;
	    a2&=a1;
	  }
	}else{
	  if(zmeqto(p->q2.val.vmax,szof(p->q1.v->vtyp))&&(a1&1)){
	    a1=zm2l(falign(p->q1.v->vtyp))&3;
	    a2&=a1;
	  }
	}
      }
      if(p->z.flags&VAR){
	if(p->z.flags&DREFOBJ){
	  if(p->z.v->vtyp->next&&zmeqto(p->q2.val.vmax,szof(p->z.v->vtyp->next))&&(a2&1)){
	    a2=zm2l(falign(p->z.v->vtyp->next))&3;
	    a1&=a2;
	  }
	}else{
	  if(zmeqto(p->q2.val.vmax,szof(p->z.v->vtyp))&&(a2&1)){
	    a2=zm2l(falign(p->z.v->vtyp))&3;
	    a1&=a2;
	  }
	}
      } 
      if(a1>=0&&a2>=0){
        if(a1==0&&a2==0){
          b=4;ld=ldt(INT);st=sdt(INT);
        }else if((a1&1)==0&&(a2&1)==0){
          b=2;ld=ldt(SHORT);st=sdt(SHORT);
        }
      }
      if(p->q1.flags&DREFOBJ){
        if(p->q1.am){
          if(p->q1.am->flags&REG_IND){
	    emit(f,"\tadd\t%s,%s,%s\n",mregnames[t1],mregnames[p->q1.am->offset],mregnames[p->q1.am->base]);
	    emit(f,"\taddi\t%s,%s,%d\n",mregnames[t1],mregnames[t1],-b);
	  }
          if(p->q1.am->flags&IMM_IND)
	    emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t1],mregnames[p->q1.am->base],p->q1.am->offset-b);
        }else{
          p->q1.flags&=~DREFOBJ;
	  if(isreg(q1)){
	    emit(f,"\taddi\t%s,%s,%d\n",mregnames[t1],mregnames[p->q1.reg],-b);
	  }else{
	    load_reg(f,t1,&p->q1,POINTER,t1);
	    emit(f,"\taddi\t%s,%s,%d\n",mregnames[t1],mregnames[t1],-b);
	  }    
          p->q1.flags|=DREFOBJ;
        }
      }else{
	p->q1.val.vmax=zmsub(p->q1.val.vmax,l2zm((long)b));
        load_address(f,t1,&p->q1,POINTER);
	p->q1.val.vmax=zmadd(p->q1.val.vmax,l2zm((long)b));
      }
      if(p->z.flags&DREFOBJ){
        if(p->z.am){
          if(p->z.am->flags&REG_IND){
	    emit(f,"\tadd\t%s,%s,%s\n",mregnames[t2],mregnames[p->z.am->offset],mregnames[p->z.am->base]);
	    emit(f,"\taddi\t%s,%s,%d\n",mregnames[t2],mregnames[t2],-b);
	  }
          if(p->z.am->flags&IMM_IND) 
	    emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t2],mregnames[p->z.am->base],p->z.am->offset-b);
        }else{
          p->z.flags&=~DREFOBJ;
	  if(isreg(z)){
	    emit(f,"\taddi\t%s,%s,%d\n",mregnames[t2],mregnames[p->z.reg],-b);
	  }else{
	    load_reg(f,t2,&p->z,POINTER,t2);
	    emit(f,"\taddi\t%s,%s,%d\n",mregnames[t2],mregnames[t2],-b);
	  }
          p->z.flags|=DREFOBJ;
        }
      }else{
        if(c==PUSH){
          pushed=(pushed+3)/4*4;
	  if(align_arguments&&p->ityp){
	    long al=falign(p->ityp);
	    pushed=(pushed+al-1)/al*al;
	  }
          emit(f,"\taddi\t%s,%s,%ld\n",mregnames[t2],mregnames[sp],pushed+minframe-b);
          pushed+=size;
	  size=zm2l(p->z.val.vmax);
        }else{
	  p->z.val.vmax=zmsub(p->z.val.vmax,l2zm((long)b));
          load_address(f,t2,&p->z,POINTER);
	  p->z.val.vmax=zmsub(p->z.val.vmax,l2zm((long)b));
        }
      }
      BSET(regs_modified,t1);
      BSET(regs_modified,t2);
      BSET(regs_modified,t3);
      if(optspeed)
	l=size/(8*b);
      else
	l=size/b;
      if(l>1){
        if(hi(l)){
	  emit(f,"\tlis\t%s,%lu\n",mregnames[t3],(l>>16)&65535);
	  emit(f,"\tori\t%s,%s,%lu\n",mregnames[t3],mregnames[t3],l&65535);
	}else{
	  emit(f,"\tli\t%s,%ld\n",mregnames[t3],lo(l));
	}
        emit(f,"\tmtctr\t%s\n",mregnames[t3]);
	BSET(regs_modified,ctr);
        emit(f,"%s%d:\n",labprefix,++label);
      }
      if(l>0){
        for(i=b;optspeed&&i<=7*b;i+=b){
          emit(f,"\tl%s\t%s,%d(%s)\n",ld,mregnames[t3],i,mregnames[t1]);
          emit(f,"\tst%s\t%s,%d(%s)\n",st,mregnames[t3],i,mregnames[t2]);
        }
        emit(f,"\tl%su\t%s,%d(%s)\n",ld,mregnames[t3],i,mregnames[t1]);
        emit(f,"\tst%su\t%s,%d(%s)\n",st,mregnames[t3],i,mregnames[t2]);
	
      }
      if(l>1){
        emit(f,"\tbdnz\t%s%d\n",labprefix,label);
      }
      if(optspeed)
	size=size%(8*b);
      else
	size=size%b;
      for(i=0;i<size/b;i++){
        emit(f,"\tl%su\t%s,%d(%s)\n",ld,mregnames[t3],b,mregnames[t1]);
        emit(f,"\tst%su\t%s,%d(%s)\n",st,mregnames[t3],b,mregnames[t2]);
      }
      size=size%b;i=b;
      if(size&2){
        emit(f,"\tl%su\t%s,%d(%s)\n",ldt(SHORT),mregnames[t3],b,mregnames[t1]);
        emit(f,"\tst%su\t%s,%d(%s)\n",sdt(SHORT),mregnames[t3],b,mregnames[t2]);
        i=2;
      }
      if(size&1){
        emit(f,"\tl%su\t%s,%d(%s)\n",ldt(CHAR),mregnames[t3],i,mregnames[t1]);
        emit(f,"\tst%su\t%s,%d(%s)\n",sdt(CHAR),mregnames[t3],i,mregnames[t2]);
      }
      p->z.flags=0;
      continue;
    }
    if(c==TEST&&ISFLOAT(t)){
      p->code=c=COMPARE;
      p->q2.flags=KONST;
      p->q2.val.vldouble=d2zld(0.0);
      if((t&NQ)==DOUBLE) p->q2.val.vdouble=zld2zd(p->q2.val.vldouble);
      if((t&NQ)==FLOAT) p->q2.val.vfloat=zld2zf(p->q2.val.vldouble);
    }
    p=do_refs(f,p);
    c=p->code;
    setcc=p->ext.setcc;
    if(c==SUBPFP) c=SUB;
    if(c==ADDI2P) c=ADD;
    if(c==SUBIFP) c=SUB;
    if(c==CONVERT){
      int to;
      static struct obj o;char *ip;
      long moff;int offreg;
      to=p->typf2;
      if(ISFLOAT(to)){
        if(ISFLOAT(t)){
          zreg=q1reg;
          continue;
        }
        if(tmpoff>32767){
          moff=0;offreg=t1;
          emit(f,"\taddis\t%s,%s,%ld\n",mregnames[offreg],mregnames[sp],hi(tmpoff));
          emit(f,"\taddi\t%s,%s,%ld\n",mregnames[offreg],mregnames[offreg],lo(tmpoff));
        }else{
          moff=tmpoff;
          offreg=sp;
        }
        if((t&NU)==(UNSIGNED|INT)||(t&NU)==(UNSIGNED|LONG)){
          o.flags=KONST;
          ip=(char *)&o.val.vdouble;
          ip[0]=0x41;
          ip[1]=0xe0;
          ip[2]=0x00;
          ip[3]=0x00;
          ip[4]=0x00;
          ip[5]=0x00;
          ip[6]=0x00;
          ip[7]=0x00;
          load_reg(f,f2,&o,DOUBLE,t2);
          emit(f,"\tfcmpu\t%s,%s,%s\n",mregnames[cr0],mregnames[q1reg],mregnames[f2]);
          emit(f,"\tcror\t3,2,1\n");
          emit(f,"\tbso\t%s,%s%d\n",mregnames[cr0],labprefix,++label);
          emit(f,"\tfctiwz\t%s,%s\n",mregnames[f2],mregnames[q1reg]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(DOUBLE),mregnames[f2],moff-8,mregnames[offreg]);
          emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(t&NQ),mregnames[zreg],moff-zm2l(sizetab[t&NQ]),mregnames[offreg]);
          emit(f,"\tb\t%s%d\n",labprefix,++label);
          emit(f,"%s%d:\n",labprefix,label-1);
          emit(f,"\tfsub\t%s,%s,%s\n",mregnames[f2],mregnames[q1reg],mregnames[f2]);
          emit(f,"\tfctiwz\t%s,%s\n",mregnames[f2],mregnames[f2]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(DOUBLE),mregnames[f2],moff-8,mregnames[offreg]);
          emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(INT),mregnames[zreg],moff-zm2l(sizetab[t&NQ]),mregnames[offreg]);
          emit(f,"\txoris\t%s,%s,32768\n",mregnames[zreg],mregnames[zreg]);
          emit(f,"%s%d:\n",labprefix,label);
	  BSET(regs_modified,f2);
	  BSET(regs_modified,zreg);
	  BSET(regs_modified,q1reg);
        }else{
          emit(f,"\tfctiwz\t%s,%s\n",mregnames[f3],mregnames[q1reg]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(DOUBLE),mregnames[f3],moff-8,mregnames[offreg]);
          emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(t&NQ),mregnames[zreg],moff-zm2l(sizetab[t&NQ]),mregnames[offreg]);
	  BSET(regs_modified,f3);
	  BSET(regs_modified,zreg);
	  BSET(regs_modified,q1reg);
        }
        if(t==CHAR) emit(f,"\textsb\t%s,%s\n",mregnames[zreg],mregnames[zreg]);
        continue;
      }
      if(ISFLOAT(t)){
        if(tmpoff>32767){
          moff=0;offreg=t1;
	  if(q1reg==t1){
	    emit(f,"\tmr\t%s,%s\n",mregnames[t3],mregnames[q1reg]);
	    q1reg=t3;
	    BSET(regs_modified,t3);
	  }
          emit(f,"\taddis\t%s,%s,%ld\n",mregnames[offreg],mregnames[sp],hi(tmpoff));
          emit(f,"\taddi\t%s,%s,%ld\n",mregnames[offreg],mregnames[offreg],lo(tmpoff));
        }else{
          moff=tmpoff;
          offreg=sp;
        }
        o.flags=KONST;
        ip=(char *)&o.val.vdouble;
        ip[0]=0x43;
        ip[1]=0x30;
        ip[2]=0x00;
        ip[3]=0x00;
        ip[4]=0x80;
        ip[5]=0x00;
        ip[6]=0x00;
        ip[7]=0x00;
        if((to&NU)==(UNSIGNED|INT)||(to&NU)==(UNSIGNED|LONG)){
          ip[4]=0x00;
          load_reg(f,f2,&o,DOUBLE,t2);
          emit(f,"\tlis\t%s,17200\n",mregnames[t2]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(INT),mregnames[q1reg],moff-4,mregnames[offreg]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(INT),mregnames[t2],moff-8,mregnames[offreg]);
          emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(DOUBLE),mregnames[zreg],moff-8,mregnames[offreg]);
          emit(f,"\tfsub\t%s,%s,%s\n",mregnames[zreg],mregnames[zreg],mregnames[f2]);
        }else{
          emit(f,"\tlis\t%s,17200\n",mregnames[t2]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(INT),mregnames[t2],moff-8,mregnames[offreg]);
          emit(f,"\txoris\t%s,%s,32768\n",mregnames[t2],mregnames[q1reg]);
          emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(INT),mregnames[t2],moff-4,mregnames[offreg]);
          emit(f,"\tl%s\t%s,%ld(%s)\n",ldt(DOUBLE),mregnames[zreg],moff-8,mregnames[offreg]);
          load_reg(f,f2,&o,DOUBLE,t2);
          emit(f,"\tfsub\t%s,%s,%s\n",mregnames[zreg],mregnames[zreg],mregnames[f2]);
        }
        continue;
      }
      if((t&NQ)>=(to&NQ)){
	if((to&NU)==CHAR){
	  emit(f,"\textsb%s\t%s,%s\n",record[setcc],mregnames[zreg],mregnames[q1reg]);
	  ccset|=setcc;
	}else if((to&NU)==SHORT&&!q1loaded){
	  emit(f,"\textsh%s\t%s,%s\n",record[setcc],mregnames[zreg],mregnames[q1reg]);
	  ccset|=setcc;
	}else if((to&NU)==(UNSIGNED|CHAR)&&!q1loaded){
	  emit(f,"\tclrlwi%s\t%s,%s,24\n",record[setcc],mregnames[zreg],mregnames[q1reg]);
	  ccset|=setcc;
	}else if((to&NU)==(UNSIGNED|SHORT)&&!q1loaded){
	  emit(f,"\tclrlwi%s\t%s,%s,16\n",record[setcc],mregnames[zreg],mregnames[q1reg]);
	  ccset|=setcc;
	}else{
	  if(setcc){
	    emit(f,"\tmr.\t%s,%s\n",mregnames[zreg],mregnames[q1reg]);
	    ccset=1;
	  }else{
	    zreg=q1reg;
	  }
	}
	continue;
      }else{
	zreg=q1reg;
        continue;
      }
    }
    if(c==KOMPLEMENT){
      emit(f,"\tnor%s\t%s,%s,%s\n",record[setcc],mregnames[zreg],mregnames[q1reg],mregnames[q1reg]);
      ccset|=setcc;
      continue;
    }
    if(ISFLOAT(t)) {fpp="f";fpf=1;} else {fpp="";fpf=0;}
    if(c==SETRETURN){
      if(p->z.reg){
	if((t&NU)==CHAR)
	  emit(f,"\textsb\t%s,%s\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if(!q1loaded&&(t&NU)==SHORT)
	  emit(f,"\textsh\t%s,%s\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if(!q1loaded&&(t&NU)==(UNSIGNED|CHAR))
	  emit(f,"\tclrlwi\t%s,%s,24\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if(!q1loaded&&(t&NU)==(UNSIGNED|SHORT))
	  emit(f,"\tclrlwi\t%s,%s,16\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if(p->z.reg!=q1reg)
	  emit(f,"\t%smr\t%s,%s\n",fpp,mregnames[p->z.reg],mregnames[q1reg]);
	BSET(regs_modified,p->z.reg);
      }else
        ierror(0);
      continue;
    }
    if(c==GETRETURN){
      if(p->q1.reg)
        zreg=p->q1.reg;
      else
        p->z.flags=0;
      continue;
    }
    if(c==CALL){
      ccset=0;
      if(stack_valid){
	int i;
	if(p->call_cnt<=0){
	  err_ic=p;error(320);
	  stack_valid=0;
	  v->fi->flags|=(WARNED_STACK|WARNED_REGS);
	}
	for(i=0;i<p->call_cnt;i++){
	  if(p->call_list[i].v->fi&&(p->call_list[i].v->fi->flags&ALL_STACK)){
	    if(framesize+zum2ul(p->call_list[i].v->fi->stack1)>stack)
	      stack=framesize+zum2ul(p->call_list[i].v->fi->stack1);
	  }else{
	    err_ic=p;
	    if(!p->call_list[i].v->fi) p->call_list[i].v->fi=new_fi();
	    if(!(p->call_list[i].v->fi->flags&WARNED_STACK)){
	      error(317,p->call_list[i].v->identifier);
	      p->call_list[i].v->fi->flags|=WARNED_STACK;
	    }
	    v->fi->flags|=WARNED_STACK;
	    stack_valid=0;
	  }
#if HAVE_OSEK
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
#endif
	}
      }
      if(!calc_regs(p,f!=0)&&v->fi) all_regs=0;
#if HAVE_OSEK
/* removed */
/* removed */
#endif
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
        emit_inline_asm(f,p->q1.v->fi->inline_asm);
      }else{
        if((p->q1.flags&(VAR|DREFOBJ))==VAR){
          if(!strcmp("__va_start",p->q1.v->identifier)){
            emit(f,"\taddi\t%s,%s,%lu\n",mregnames[r3],mregnames[sp],framesize+minframe+zm2l(va_offset(v))/*+vparmos*/);
            BSET(regs_modified,r3);continue;
          }
          if(!strcmp("__va_regbase",p->q1.v->identifier)){
            emit(f,"\taddi\t%s,%s,%ld\n",mregnames[r3],mregnames[sp],regbase);
            BSET(regs_modified,r3);continue;continue;
          }
          if(!strcmp("__va_fixedgpr",p->q1.v->identifier)){
            emit(f,"\tli\t%s,%d\n",mregnames[r3],fixedgpr);
            BSET(regs_modified,r3);continue;continue;
          }
          if(!strcmp("__va_fixedfpr",p->q1.v->identifier)){
            emit(f,"\tli\t%s,%d\n",mregnames[r3],fixedfpr);
            BSET(regs_modified,r3);continue;continue;
          }
        }
        if(g_flags[10]&USEDFLAG) emit(f,"\tcreqv\t6,6,6\n");
	/* poweropen-hack! look if some fp-args have been passed on the stack */
	if(POWEROPEN&&!zmeqto(p->q2.val.vmax,l2zm(0L))){
	  int r=45/*f12*/,off=24;
	  for(i=0;r<47&&i<p->arg_cnt;i++){
	    if(p->arg_list[i]->code==PUSH){
	      int typ=p->arg_list[i]->typf;
	      if(ISFLOAT(typ)){
		emit(f,"\tl%s\t%s,%d(%s)\n",ldt(typ&NQ),mregnames[r],off,mregnames[sp]);
		r++;
	      }
	    }
	    off+=(zm2l(p->arg_list[i]->q2.val.vmax)+3)/4*4;
	  }
	}
	if(q1reg){
          emit(f,"\tmtlr\t%s\n",mregnames[q1reg]);
          emit(f,"\tblrl\n");
        }else{
          emit(f,"\tbl\t");emit_obj(f,&p->q1,t);
          emit(f,"\n");
        }
	BSET(regs_modified,lr);
      }
      pushed-=zm2l(p->q2.val.vmax);
      continue;
    }
    if(c==ASSIGN||c==PUSH){
      if(t==0) ierror(0);
      if(q1reg||c==PUSH){
        if(c==PUSH){
	  pushed=(pushed+3)/4*4;
	  if(align_arguments&&malign[p->typf&NQ]==8)
	    pushed=(pushed+7)/8*8;
          if(q1reg)
            emit(f,"\tst%s\t%s,%ld(%s)\n",sdt(t&NQ),mregnames[q1reg],pushed+minframe,mregnames[sp]);
          pushed+=zm2l(p->q2.val.vmax);
          continue;
        }
        if(c==ASSIGN){
          if(setcc&&!fpf&&sizetab[t&NQ]==sizetab[INT]){
            emit(f,"\tmr.\t%s,%s\n",mregnames[zreg],mregnames[q1reg]);
            ccset=1;
          }else{
            zreg=q1reg;
          }
        }
        continue;
      }
    }
    if(c==ADDRESS){
      load_address(f,zreg,&p->q1,POINTER);
      continue;
    }
    if(c==MINUS){
      emit(f,"\t%sneg%s\t%s,%s\n",fpp,record[setcc&&!fpf],mregnames[zreg],mregnames[q1reg]);
      if(setcc&&!fpf) ccset=1;
      continue;
    }
    if(c==TEST){
      if(!(p->z.flags&REG))
        p->z.reg=cr0;
      if(!multiple_ccs&&(t&UNSIGNED)){
        struct IC *p2=p->next;
        while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG)) p2=p2->next;
        if(!p2) continue;
        if(p2->code==BGT) p2->code=BNE;
        else if(p2->code==BGE) p2->code=BRA;
        else if(p2->code==BLT) p2->code=NOP;
        else if(p2->code==BLE) p2->code=BEQ;
      }
      if(ccset&&p->z.reg==cr0) continue;
      if(ISFLOAT(t)){
        ierror(0);
      }else{
	if((t&NU)==CHAR)
	  emit(f,"\textsb.\t%s,%s\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if((t&NU)==SHORT)
	  emit(f,"\textsh.\t%s,%s\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if((t&NU)==(UNSIGNED|CHAR))
	  emit(f,"\tandi.\t%s,%s,255\n",mregnames[p->z.reg],mregnames[q1reg]);
	else if((t&NU)==(UNSIGNED|SHORT))
	  emit(f,"\tandi.\t%s,%s,65535\n",mregnames[p->z.reg],mregnames[q1reg]);
	else
	  emit(f,"\tcmp%swi\t%s,%s,0\n",(t&UNSIGNED)?"l":"",mregnames[p->z.reg],mregnames[q1reg]);
      }
      if(p->z.reg==cr0) ccset=0;
      continue;
    }
    if(c==COMPARE){
      if(!(p->z.flags&REG))
        p->z.reg=cr0;
      if(ISFLOAT(t))
        emit(f,"\tfcmpu\t%s,%s,",mregnames[p->z.reg],mregnames[q1reg]);
      else
        emit(f,"\tcmp%sw%s\t%s,%s,",((t&UNSIGNED)||((t&NQ)==POINTER))?"l":"",isimm[q2reg==0],mregnames[p->z.reg],mregnames[q1reg]);
      emit_obj(f,&p->q2,t);emit(f,"\n");
      if(p->z.reg==cr0) ccset=0;
      continue;
    }
    BSET(regs_modified,zreg);
    if(c==AND&&q2reg==0){
      ccset=setcc;
      emit(f,"\tandi.\t%s,%s,",mregnames[zreg],mregnames[q1reg]);
      emit_obj(f,&p->q2,t|UNSIGNED);emit(f,"\n");
      continue;
    }
    if(c>=OR&&c<=AND){
      emit(f,"\t%s%s%s\t%s,%s,",logicals[c-OR],isimm[q2reg==0],record[setcc&&q2reg],mregnames[zreg],mregnames[q1reg]);
      emit_obj(f,&p->q2,t|UNSIGNED);emit(f,"\n");
      if(setcc&&q2reg) ccset=1;
      continue;
    }
    if(c==SUB&&(p->q1.flags&KONST)){
      emit(f,"\tsubfic\t%s,%s,",mregnames[zreg],mregnames[q2reg]);
      emit_obj(f,&p->q1,t&NQ);emit(f,"\n");
      continue;
    }
    if(c>=LSHIFT&&c<=MOD){
      if(c==RSHIFT&&((t&NQ)<=SHORT)){
	/* special treatment for short shifts used in bitfields with
	   sub-int type; will not handle the general case (which, however,
	   should never occur) */
	int width,shift;
	width=sizetab[t&NQ]*8;
	if(!(p->q2.flags&KONST)) ierror(0);
	eval_const(&p->q2.val,p->typf2&NU);
	shift=zm2l(vmax);
	if(shift<0||shift>=width) ierror(0);
	if(shift==0) continue;
	if(!(t&UNSIGNED)){
	  emit(f,"\texts%c\t%s,%s\n",width==8?'b':'h',mregnames[zreg],mregnames[q1reg]);
	  q1reg=zreg;
	}
	emit(f,"\trlwinm\t%s,%s,%d,%d,%d\n",mregnames[zreg],mregnames[q1reg],32-shift,32-width+((t&UNSIGNED)?shift:0),31);
	continue;
      }
      if(c==RSHIFT&&!(t&UNSIGNED)){
        emit(f,"\tsraw%s%s\t%s,%s,",isimm[q2reg==0],record[setcc],mregnames[zreg],mregnames[q1reg]);
        emit_obj(f,&p->q2,q2typ(p));
	/* fix for illegal shift values (undefined behaviour) */
	if(!isreg(q2))
	  emit(f,"&31");
	emit(f,"\n");
        ccset|=setcc;
        continue;
      }
      if(c==MOD){
        i=0;
        if(zreg==q1reg||zreg==q2reg){
          if(t1!=q1reg&&t1!=q2reg) i=t1;
          if(t2!=q1reg&&t2!=q2reg) i=t2;
        }else i=zreg;
        if(i==0||i==q1reg||i==q2reg) ierror(0);
        emit(f,"\tdivw%s\t%s,%s,%s\n",(t&UNSIGNED)?"u":"",mregnames[i],mregnames[q1reg],mregnames[q2reg]);
        emit(f,"\tmullw\t%s,%s,%s\n",mregnames[i],mregnames[i],mregnames[q2reg]);
        emit(f,"\tsubf%s\t%s,%s,%s\n",record[setcc],mregnames[zreg],mregnames[i],mregnames[q1reg]);
        ccset|=setcc;
        continue;
      }
      if(c==DIV&&(t&UNSIGNED)){
        emit(f,"\tdivwu%s%s\t%s,%s,",isimm[q2reg==0],record[setcc&&q2reg],mregnames[zreg],mregnames[q1reg]);
        if(setcc&&q2reg) ccset=1;
      }else if(c==MULT&&ISFLOAT(t)){
	if(isreg(z)&&(g_flags[14]&USEDFLAG)){
	  struct IC *np=p->next,*add;int madd;
	  while(np&&(np->code==FREEREG||np->code==ALLOCREG)) np=np->next;
	  if(np&&(np->code==ADD||np->code==SUB)&&(np->q1.flags&(REG|DREFOBJ))==REG&&(np->q1.reg==p->z.reg||(np->code==ADD&&np->q2.reg==p->z.reg))&&(np->q2.flags&(REG|DREFOBJ))==REG&&np->q1.reg!=np->q2.reg){
	    add=np;
	    madd=0;
	    if((np->z.flags&(REG|DREFOBJ))==REG&&np->z.reg==p->z.reg) madd=1;
	    np=np->next;
	    while(np&&(np->code==FREEREG||np->code==ALLOCREG)){
	      if(np->code==FREEREG&&np->q1.reg==p->z.reg) madd=1;
	      np=np->next;
	    }
	    if(madd){
	      if((add->z.flags&(REG|DREFOBJ))==REG) zreg=add->z.reg;
	      if(add->code==ADD){
		if(add->q1.reg==p->z.reg) madd=add->q2.reg; else madd=add->q1.reg;
		emit(f,"\tfmadd%s\t%s,%s,%s,%s\n",(t&NQ)==FLOAT?"s":"",mregnames[zreg],mregnames[q1reg],mregnames[q2reg],mregnames[madd]);
	      }else
		emit(f,"\tfmsub%s\t%s,%s,%s,%s\n",(t&NQ)==FLOAT?"s":"",mregnames[zreg],mregnames[q1reg],mregnames[q2reg],mregnames[add->q2.reg]);
	      add->code=NOP;
	      p->z=add->z;
	      add->z.am=0;
	      continue;
	    }
	  }
	}
	emit(f,"\tfmul%s\t%s,%s,",(t&NQ)==FLOAT?"s":"",mregnames[zreg],mregnames[q1reg]);
      }else if(c==DIV&&ISFLOAT(t)){
        emit(f,"\tfdiv%s\t%s,%s,",(t&NQ)==FLOAT?"s":"",mregnames[zreg],mregnames[q1reg]);
      }else if(c==MULT&&q2reg==0){
        emit(f,"\tmulli\t%s,%s,",mregnames[zreg],mregnames[q1reg]);
      }else if(c==ADD&&setcc&&!q2reg){
        emit(f,"\taddic.\t%s,%s,",mregnames[zreg],mregnames[q1reg]);
        ccset=1;
      }else{
        emit(f,"\t%s%s%s%s%s\t%s,%s,",fpp,arithmetics[c-LSHIFT],(t&NQ)==FLOAT?"s":"",isimm[q2reg==0],record[setcc&&q2reg&&!fpf],mregnames[zreg],mregnames[q1reg]);
        if(setcc&&q2reg&&!fpf) ccset=1;
      }
      emit_obj(f,&p->q2,q2typ(p)&NQ);
      /* fix for illegal shift values (undefined behaviour) */
      if((c==LSHIFT||c==RSHIFT)&&q2reg==0) emit(f,"&31");
      emit(f,"\n");
      continue;
    }
    pric2(stdout,p);
    ierror(0);
  }
  if(!cross_module)  lastlabel=label;  /*FIXME*/
  free(once);free(twice);
  function_bottom(f,v,of);
#if HAVE_OSEK
/* removed */
#endif
  if(stack_valid){
    if(!v->fi) v->fi=new_fi();
    if(v->fi->flags&ALL_STACK){
      if(v->fi->stack1!=stack&&!(v->tattr&SAVEALL))
	if(f) error(319,"",stack,v->fi->stack1);
#if HAVE_OSEK
/* removed */
/* removed */
/* removed */
#endif
    }else{
      v->fi->flags|=ALL_STACK;
      v->fi->stack1=stack;
    }
  }
  emit(f,"# stacksize=%lu%s\n",(unsigned long)stack,stack_valid?"":"+??");
  if(stack_valid) emit(f,"\t.set\t%s__stack_%s,%lu\n",idprefix,v->identifier,(unsigned long)stack);
  if(debug_info){
    emit(f,"%s%d:\n",labprefix,++label);
    dwarf2_function(f,v,label);
    if(f) section=-1;
  }
}

int shortcut(int code,int typ)
{
  return 0;
}

int reg_parm(struct reg_handle *m, struct Typ *t,int vararg,struct Typ *ft)
{
#if HAVE_AOS4
  extern int aos4_attr(struct Typ *,char *);
#endif
  int f;
  if(!m) ierror(0);
  if(!t) ierror(0);
  if(vararg&&POWEROPEN) return 0;
#ifdef HAVE_AOS4
  if(vararg&&ft&&aos4_attr(ft,"linearvarargs")) return 0;
#endif
  f=t->flags&NQ;
  if(f<=LONG||f==POINTER){
    if(m->gregs>=8) return 0;
    if(POWEROPEN){
      if(!STORMC) m->fregs++;
      return -(r3+m->gregs++);
    }else{
      return r3+m->gregs++;
    }
  }
  if(f==LLONG){
    int r;
    if(m->gregs>=7) return 0;
    m->gregs=(m->gregs+3)/2*2;
    if(m->gregs==2) r=74;
    else if(m->gregs==4) r=75;
    else if(m->gregs==6) r=76;
    else if(m->gregs==8) r=77;
    else ierror(0);
    if(POWEROPEN) 
      return -r;
    else
      return r;
  }
  if(ISFLOAT(f)){
    if(POWEROPEN){
      if(m->fregs>=11) return 0; /* hack! we pretend fp-arg 12/13 is passed on the stack */
      if(!STORMC){
        if(f!=FLOAT) m->gregs+=2; else m->gregs++;
      }
      return -(34+m->fregs++);
    }else{
      if(m->fregs>=8) return 0;
      return 34+m->fregs++;
    }
  }
  return 0;
}

int handle_pragma(const char *s)
{
  if(!strncmp("amiga-align",s,11)){
    align[INT]=align[LONG]=align[LLONG]=align[POINTER]=align[FLOAT]=align[DOUBLE]=align[LDOUBLE]=l2zm(2L);
    return 1;
  }else if(!strncmp("natural-align",s,13)){
    align[INT]=align[LONG]=align[POINTER]=align[FLOAT]=l2zm(4L);
    align[LLONG]=align[DOUBLE]=align[LDOUBLE]=l2zm(8L);
    return 1;
  }else if(!strncmp("default-align",s,13)){
    if(g_flags[7]&USEDFLAG){
      align[INT]=align[LONG]=align[LLONG]=align[POINTER]=align[FLOAT]=align[DOUBLE]=align[LDOUBLE]=l2zm(2L);
    }else{
      align[INT]=align[LONG]=align[POINTER]=align[FLOAT]=l2zm(4L);
      align[LLONG]=align[DOUBLE]=align[LDOUBLE]=l2zm(8L);
    }
    return 1;
  }
  return 0;
}

void cleanup_cg(FILE *f)
{
  struct fpconstlist *p;
  unsigned char *ip;
  struct StatFPtrList *tfp,*fp=firstfptr;

  title(f);

  if(fp){
    emit(f,tocname);
    if(f) section=TOC;
  }
  while(tfp=fp){
    emit(f,"%s%s%ld:\n\t.long\t%s%ld\n",
	 tocprefix,labprefix,zm2l(tfp->vptr->offset),
	 labprefix,zm2l(tfp->vptr->offset));
    fp=fp->next;
    free(tfp);
  }
  if(f&&stack_check)
    emit(f,"\t.global\t%s__stack_check\n",idprefix);
  while(p=firstfpc){
    if(f){
      if(POWEROPEN&&!use_sd(p->typ)){
        emit(f,tocname);
        emit(f,"%s%s%ld:\n",tocprefix,labprefix,zm2l(p->label));
        emit(f,"\t.long\t%s%ld\n",labprefix,zm2l(p->label));
        if(f) section=TOC;
      }
      if(use_sd(p->typ)){
	if(EABI){
	  if(section!=SDATA2){emit(f,sdata2name);if(f) section=SDATA2;}
	}else{
	  if(section!=TOC){emit(f,tocname);if(f) section=TOC;}
	}
      }else{
        if(section!=RODATA){emit(f,rodataname);if(f) section=RODATA;}
      }
      if((p->typ&NQ)==DOUBLE)
        emit(f,"\t.align\t3\n");
      else
        emit(f,"\t.align\t2\n");
      emit(f,"%s%d:\n\t.long\t",labprefix,p->label);
      ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
      if((p->typ&NQ)==DOUBLE){
        emit(f,",0x%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
      }
      emit(f,"\n");
    }
    firstfpc=p->next;
    free(p);
  }
}
void cleanup_db(FILE *f)
{
  dwarf2_cleanup(f);
  if(f) section=-1;
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
  if(!(p->q2.flags&KONST)){
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

char *use_libcall(int c,int t,int t2)
{
  static char fname[32];
  char *ret=0;

  if(OLDLIBCALLS) return 0;

  if((t&NQ)==LLONG){
    ret=fname;
    if(c==MULT)
      sprintf(fname,"__mulint64");
    else if(c==DIV)
      sprintf(fname,"__div%cint64",(t&UNSIGNED)?'u':'s');
    else if(c==MOD)
      sprintf(fname,"__mod%cint64",(t&UNSIGNED)?'u':'s');
    else if(c==RSHIFT)
      sprintf(fname,"__rsh%cint64",(t&UNSIGNED)?'u':'s');
    else if(c==LSHIFT)
      sprintf(fname,"__lshint64");
    else
      ret=0;
  }
  return ret;
}



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
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
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

/*  Code generator for Motorola 68hc12 microcontrollers.               */

/*TODO:
  regs_modified bei struct-copy
  savings verfeinern
  4-Byte Copy
  [static] testen
  peephole-Pass um ALLOCREGs zu entfernen
  ACC_IND (Achtung?)
  struct-copy Problemfälle
  banked
  bit
  long long, float, double, long double

*/

#include "supp.h"
#include "vbc.h" /* nicht schoen, aber ... */

static char FILE_[]=__FILE__;

#include "dwarf2.c"

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc code-generator for 6809/6803/68hc12 V1.0 (c) in 2000-2021 by Volker Barthelmann";

/*  Commandline-flags the code-generator accepts                */
int g_flags[MAXGF]={VALFLAG,VALFLAG,0,0,
                    0,0,0,0,
		    0,0};
char *g_flags_name[MAXGF]={"cpu","fpu","no-delayed-popping","const-in-data",
                           "merge-constants","no-peephole","mem-cse","acc-glob",
			   "pcrel","drel","no-char-addi2p","nodx","nou","double64"};
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
char *regnames[]={"noreg","d","x","y","sp","u","d/x"};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  Type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1]={0,1,1,0,1,0,1};

int reg_prio[MAXR+1]={0,0,1,1,0,0,1};

struct reg_handle empty_reg_handle={0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__interrupt","__dpage",0};
#define INTERRUPT 1
#define DPAGE     2

int MINADDI2P=CHAR;

/****************************************/
/*  Some private data and functions.    */
/****************************************/

static long malign[MAX_TYPE+1]=  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static long msizetab[MAX_TYPE+1]={0,1,1,2,2,4,4,4,4,4,0,2,4,4,0,0,0,2,0};

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
#define KONSTINC 8

/* (user)stack-pointer, pointer-tmp, int-tmp; reserved for compiler */
static int acc=1,ix=2,iy=3,sp=4,iu=5,dx=6;
static void pr(FILE *,struct IC *);
static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

static char *marray[]={"__section(x,y)=__vattr(\"section(\"#x\",\"#y\")\")",
		       "__HC12__",
		       "__SIZE_T_INT=1",
		       "__direct=__vattr(\"section(\\\"dpage\\\")\")",
		       0};

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

static long loff,roff,stackoffset,notpopped,dontpop,maxpushed,stack;

static char *x_t[]={"?","","b","","","","","","","","","","","","","",""};
static char *ccs[]={"eq","ne","lt","ge","le","gt"};
static char *uccs[]={"eq","ne","lo","hs","ls","hi"};
static char *logicals[]={"ora","eor","and"};
static char *dct[]={"",".bit",".byte",".2byte",".2byte",".4byte",".8byte",".4byte",".4byte",".4byte",
		    "(void)",".2byte",".34byte",".34byte"};
static char *idprefix="",*labprefix=".l";
static int exit_label,have_frame;
static char *ret;
static int stackchecklabel;
static int frame_used,stack_valid;
static int CPU=6812;
static int pcrel,drel;
static int skip_rel;
static char *jsrinst="jsr";
static char *jmpinst="jmp";
static int nodx,nou;
int switchsubs;

static int cc_t;
static struct obj *cc;

static struct obj mobj;

#define STR_NEAR "near"
#define STR_FAR "far"
#define STR_HUGE "huge"
#define STR_BADDR "baddr"

#define ISNULL() (zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
#define ISLWORD(t) ((t&NQ)==LONG||(t&NQ)==FPOINTER||(t&NQ)==HPOINTER||ISFLOAT(t&NQ))
#define ISHWORD(t) ((t&NQ)==INT||(t&NQ)==SHORT||(t&NQ)==NPOINTER)
#define ISCHWORD(t) ((t&NQ)==CHAR||ISHWORD(t))
#define ISSTATIC(v) ((v)->storage_class==EXTERN||(v)->storage_class==STATIC)
#define ISBADDR(v) ((v)->vtyp->attr&&strstr(STR_BADDR,(v)->vtyp->attr))
/*FIXME*/
#define ISFAR(v) ((v)->vtyp->attr&&(strstr(STR_FAR,(v)->vtyp->attr)||strstr(STR_HUGE,(v)->vtyp->attr)))

#define ISACC(x) ((x)==acc)
#define ISX(x) ((x)==ix)
#define ISY(x) ((x)==iy)
#define ISU(x) ((x)==iu)
#define ISIDX(x) (ISX(x)||ISY(x)||(ISU(x)&&CPU!=6812))
#define ISRACC(x) (isreg(x)&&ISACC(p->x.reg))
#define ISRX(x) (isreg(x)&&ISX(p->x.reg))
#define ISRY(x) (isreg(x)&&ISY(p->x.reg))
#define ISRU(x) (isreg(x)&&ISU(p->x.reg))
#define ISRIDX(x) (isreg(x)&&ISIDX(p->x.reg))

#define CPUOPT ((g_flags[0]&USEDFLAG)?g_flags_val[0].l:6812)

#define SPUSH(x) (CPU==6812?"\tpsh" x "\n":"\tpshs\t" x "\n")
#define SPUSHD   (CPU==6812?"\tpshd\n":"\tpshs\tb,a\n")
#define SPULL(x) (CPU==6812?"\tpul" x "\n":"\tpuls\t" x "\n")
#define SPULLD   (CPU==6812?"\tpuld\n":"\tpuls\ta,b\n")
#define SCMP(x)  (CPU==6812?"\tcp" x "\t":"\tcmp" x "\t")
#define SEX      (CPU==6812?"\tsex\tb,d\n":"\tsex\n")

#define SGN16(x) (zm2l(zi2zm(zm2zi(l2zm((long)(x))))))

enum peepf { NEEDSAME = 1, REMOVE1ST = 2, ALLOWSFX = 4};
struct peeps {char *s1,*s2,*r;enum peepf flags;};

static int check_sfx(char *s)
{
  if(!*s) return 0;
  s+=strlen(s)-1;
  if(*s=='+'||*s=='-') return 1;
  if(*s!='s'&&*s!='x'&&*s!='y'&&*s!='u') return 0;
  s--;
  if(*s!=',') return 0;
  s--;
  if(*s=='+'||*s=='-') return 1;
  return 0;
}

static int setszflag(char *op,char r)
{
  static char *zb[]={"adcb","addb","andb","aslb","asrb","clrb","comb","decb","eorb","incb",
		"ldab","ldb","lslb","lsrb","negb","orb","orab","rolb","rorb","sbcb",
		"stb","stab","subb","tstb"};
  static char *zd[]={"addd","ldd","sex","std","subd"};

  int i;

  if(r=='b'){
    for(i=0;i<sizeof(zb)/sizeof(*zb);i++)
      if(!strcmp(op,zb[i]))
	return 1;
  }
  if(r=='d'){
    for(i=0;i<sizeof(zd)/sizeof(*zd);i++)
      if(!strcmp(op,zd[i]))
	return 1;
  }
  if(r=='x'&&(!strcmp(op,"leax")||!strcmp(op,"ldx"))) return 1;
  if(r=='y'&&(!strcmp(op,"leay")||!strcmp(op,"ldy"))) return 1;
  if(CPU==6812){
    if(r=='x'&&(!strcmp(op,"dex")||!strcmp(op,"inx"))) return 1;
    if(r=='y'&&(!strcmp(op,"dey")||!strcmp(op,"iny"))) return 1;
  }
  return 0;
}

int emit_peephole(void)
{
  int entries,i,j,v1,v2;
  char *asmline[EMIT_BUF_DEPTH];
  char buf1[1024],buf2[1024];
  char op1[8],op2[8];


  /* TODO: adapt better */
  static struct peeps elim[]={
    "lda","sta",0,NEEDSAME,
    "ldb","stb",0,NEEDSAME,
    "ldaa","staa",0,NEEDSAME,
    "ldab","stab",0,NEEDSAME,
    "ldd","std",0,NEEDSAME,
    "ldx","stx",0,NEEDSAME,
    "ldy","sty",0,NEEDSAME,
    "ldu","stu",0,NEEDSAME,
    "sta","sta",0,NEEDSAME,
    "stb","stb",0,NEEDSAME,
    "staa","staa",0,NEEDSAME,
    "stab","stab",0,NEEDSAME,
    "std","std",0,NEEDSAME,
    "stx","stx",0,NEEDSAME,
    "sty","sty",0,NEEDSAME,
    "stu","stu",0,NEEDSAME,
    "sta","lda",0,NEEDSAME,
    "stb","ldb",0,NEEDSAME,
    "staa","ldaa",0,NEEDSAME,
    "stab","ldab",0,NEEDSAME,
    "std","ldd",0,NEEDSAME,
    "stx","ldx",0,NEEDSAME,
    "sty","ldy",0,NEEDSAME,
    "stu","ldu",0,NEEDSAME,
#if 0
    "lda","lda",0,REMOVE1ST,
    "ldaa","ldaa",0,REMOVE1ST,
    "ldab","ldab",0,REMOVE1ST,
    "ldb","ldb",0,REMOVE1ST,
    "ldd","ldd",0,REMOVE1ST,
    "ldx","ldx",0,REMOVE1ST,
    "ldy","ldy",0,REMOVE1ST,
    "ldu","ldu",0,REMOVE1ST,
    "lda","pla",0,REMOVE1ST,
    "lda","txa",0,REMOVE1ST,
    "lda","tya",0,REMOVE1ST,
    "ldx","tax",0,REMOVE1ST,
    "ldy","tay",0,REMOVE1ST,
    "tay","ldy",0,REMOVE1ST,
    "tax","ldx",0,REMOVE1ST,
    "txa","lda",0,REMOVE1ST,
    "tya","lda",0,REMOVE1ST,
#endif
  };


  i=emit_l;
  if(emit_f==0)
    entries=i-emit_f+1;
  else
    entries=EMIT_BUF_DEPTH;
  asmline[0]=emit_buffer[i];
  if(sscanf(asmline[0]," %6s %999s",op1,buf1)==2&&!strcmp(op1,"cmpb")&&!strcmp(buf1,"#0"))
    strcpy(asmline[0],"\ttstb\n");
  if(sscanf(asmline[0]," %6s %999s",op1,buf1)==2&&!strcmp(op1,"cmpd")&&!strcmp(buf1,"#0"))
    strcpy(asmline[0],"\tsubd\t#0\n");

  if(entries>=2){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[1]=emit_buffer[i];

    for(j=0;j<sizeof(elim)/sizeof(elim[0]);j++){
      if(elim[j].flags&NEEDSAME){
	if(sscanf(asmline[0]," %6s %999s",op2,buf2)==2&&
	   sscanf(asmline[1]," %6s %999s",op1,buf1)==2&&
	   !strcmp(op1,elim[j].s1)&&!strcmp(op2,elim[j].s2)&&
	   !strcmp(buf1,buf2)){
	  if(!check_sfx(buf1)&&!check_sfx(buf2)){
	    if(elim[j].r){
	      strcpy(asmline[0],elim[j].r);
	    }else{
	      if(elim[j].flags&REMOVE1ST)
		strcpy(asmline[1],asmline[0]);
	      remove_asm();
	    }
	    return 1;
	  }
	}
      }else{
	*buf1=0;*buf2=0;
	if(sscanf(asmline[1]," %6s %999s",op1,buf1)>=1&&
	   sscanf(asmline[0]," %6s %999s",op2,buf2)>=1&&
	   !strcmp(op1,elim[j].s1)&&!strcmp(op2,elim[j].s2)){
	  if((elim[j].flags&ALLOWSFX)||(!check_sfx(buf1)&&!check_sfx(buf2))){
	    if(elim[j].flags&REMOVE1ST)
	      strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return 1;
	  }
	}
      }
    }

    if(!strcmp(asmline[0],"\trts\n")&&sscanf(asmline[1]," %6s %999s",op1,buf1)==2&&!strcmp(op1,"puls")){
      sprintf(asmline[1]+strlen(asmline[1])-1,",pc\n");
      remove_asm();
      return 1;
    }

    if(!strcmp(asmline[0],"\tstb\t0,s\n")&&!strcmp(asmline[1],"\tleas\t-1,s\n")){
      strcpy(asmline[1],"\tpshs\tb\n");
      remove_asm();
      return 1;
    }

    if(!strcmp(asmline[0],"\tstd\t0,s\n")&&!strcmp(asmline[1],"\tleas\t-2,s\n")){
      strcpy(asmline[1],"\tpshs\tb,a\n");
      remove_asm();
      return 1;
    }

    if(!strcmp(asmline[0],"\tldb\t0,s\n")&&!strcmp(asmline[1],"\tpshs\tb\n")){
      remove_asm();
      return 1;
    }

    if(!strcmp(asmline[0],"\tldd\t0,s\n")&&!strcmp(asmline[1],"\tpshs\tb,a\n")){
      remove_asm();
      return 1;
    }

    if(!strcmp(asmline[0],"\tpshs\tb,a\n")&&!strcmp(asmline[1],"\tpuls\ta,b\n")){
      strcpy(asmline[1],"\tldd\t0,s\n");
      remove_asm();
      return 1;
    }

    if(sscanf(asmline[1]," ldd %999s",op1)>=1&&sscanf(asmline[0]," ldd %999s",op2)>=1){
      if(!((op2[0]=='a'||op2[0]=='b'||op2[0]=='d')&&op2[1]==',')){
	strcpy(asmline[1],asmline[0]);
	remove_asm();
	return 1;
      }
    }

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

    if(sscanf(asmline[0]," leas %d,s",&v1)==1&&sscanf(asmline[1]," leas %d,s",&v2)==1){
      sprintf(asmline[1],"\tleas\t%ld,s\n",SGN16(v1+v2));
      remove_asm();
      return 1;
    }

    if(CPU!=6812&&sscanf(asmline[0]," tfr %c,%c",buf1,buf2)==2){
      if((*buf1=='x'||*buf1=='y'||*buf1=='u'||*buf1=='s')&&
	 (*buf2=='x'||*buf2=='y'||*buf2=='u'||*buf2=='s')){
	sprintf(asmline[0],"\tlea%c\t,%c\n",*buf2,*buf1);
      }
    }
    if(CPU==6812&&(!strcmp(asmline[1],"\tdex\n")||!strcmp(asmline[1],"\tdey\n")||!strcmp(asmline[1],"\tsubd\t#1\n"))&&
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
    if(CPU==6812&&(!strcmp(asmline[1],"\tinx\n")||!strcmp(asmline[1],"\tiny\n")||!strcmp(asmline[1],"\taddd\t#1\n"))&&
       (!strncmp(asmline[0],"\tbne\t",5)||!strncmp(asmline[0],"\tbeq\t",5))){
      char r=asmline[1][3];
      strcpy(asmline[1],"\ti");
      strncpy(asmline[1]+2,asmline[0]+1,4);
      asmline[1][6]=r;asmline[1][7]=',';
      strcpy(asmline[1]+8,asmline[0]+5);
      remove_asm();
      return 1;
    }
  }
  if(entries>=3){
    i--;
    if(i<0) i=EMIT_BUF_DEPTH-1;
    asmline[2]=emit_buffer[i];
    if(sscanf(asmline[0]," %6s %999s",op1,buf1)==2){
      if(!strcmp(op1,"beq")||!strcmp(op1,"bne")){
	if(!strcmp(asmline[1],"\ttstb\n")||!strcmp(asmline[1],"\tcpb\t#0\n")){
	  if(sscanf(asmline[2]," %6s %999s",op2,buf2)>=1&&
	     setszflag(op2,'b')){
	    strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return 1;
	  }
	}
	if(sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&
	   (!strcmp(op2,"subd")||!strcmp(op2,"cpd"))&&!strcmp(buf2,"#0")){
	  if(sscanf(asmline[2]," %6s %999s",op2,buf2)>=1&&
	     setszflag(op2,'d')){
	    strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return 1;
	  }
	}
	if(sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&
	   !strcmp(op2,(CPU==6812)?"cpx":"cmpx")&&!strcmp(buf2,"#0")){
	  if(sscanf(asmline[2]," %6s %999s",op2,buf2)>=1&&
	     setszflag(op2,'x')){
	    strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return 1;
	  }
	}
	if(sscanf(asmline[1]," %6s %999s",op2,buf2)==2&&
	   !strcmp(op2,(CPU==6812)?"cpy":"cmpy")&&!strcmp(buf2,"#0")){
	  if(sscanf(asmline[2]," %6s %999s",op2,buf2)>=1&&
	     setszflag(op2,'y')){
	    strcpy(asmline[1],asmline[0]);
	    remove_asm();
	    return 1;
	  }
	}
      }
    }
  }
  return 0;
}

static int special_section(FILE *f,struct Var *v)
{
  char *sec;
  if(v->tattr&DPAGE){
    emit(f,"\t.section\t.dpage\n");
  }else{
    if(!v->vattr) return 0;
    sec=strstr(v->vattr,"section(");
    if(!sec) return 0;
    sec+=strlen("section(");
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
    if(flags==KONSTINC){
      eval_const(&p->val,p->am->base);
      if(ISFLOAT(p->am->base)){
	unsigned char *m=(unsigned char *)&p->val.vfloat;
	if(zm2l(p->am->offset)==2)
	  emit(f,"#0x%02x%02x",m[2],m[3]);
	else
	  emit(f,"#0x%02x%02x",m[0],m[1]);
	return;
      }else if((t&NQ)==CHAR){
	vumax=zumrshift(vumax,24-8*p->am->offset);
	vumax=zumand(vumax,ul2zum(255UL));
      }else{
	vumax=zumrshift(vumax,16-8*p->am->offset);
	vumax=zumand(vumax,ul2zum(0xFFFFUL));
      }
      emit(f,"#%lu",zum2ul(vumax));
      return;
    }
    if(flags<POST_INC||flags>PRE_DEC||CPU==6812)
      emit(f,"%ld",p->am->offset&0xffff);
    if(p->am->v){
      if(p->am->v->storage_class==STATIC)
	emit(f,"+%s%ld",labprefix,zm2l(p->am->v->offset));
      else
	emit(f,"+(%s%s)",idprefix,p->am->v->identifier);
    }
    emit(f,",");
    if(flags==PRE_INC){
      emit(f,"+");
      if(p->am->offset==2&&CPU!=6812) emit(f,"+");
    }else if(flags==PRE_DEC){
      emit(f,"-");
      if(p->am->offset==2&&CPU!=6812) emit(f,"-");
    }
    emit(f,"%s",regnames[p->am->base]);
    if(flags==POST_INC){
      emit(f,"+");
      if(p->am->offset==2&&CPU!=6812) emit(f,"+");
    }else if(flags==POST_DEC){
      emit(f,"-");
      if(p->am->offset==2&&CPU!=6812) emit(f,"-");
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
      if(!zmeqto(l2zm(0L),p->val.vmax)){
	emit(f,"%ld",zm2l(zi2zm(zm2zi(p->val.vmax))));
	emit(f,"+");
      }
      if(p->v->storage_class==STATIC){
	emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
	emit(f,"(%s%s)",idprefix,p->v->identifier);
      }
      if(pcrel&&!(p->flags&VARADR)&&ISFUNC(p->v->vtyp->flags))
	emit(f,",pc");
      if(drel&&!(p->flags&VARADR)&&!ISFUNC(p->v->vtyp->flags)){
	if(CPU==6812) ierror(0);
	emit(f,",%s",regnames[iu]);
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
    if(p->v->storage_class==EXTERN||p->v->storage_class==STATIC){
      if(is_const(p->v->vtyp)){
	if(!pcrel&&CPU==6812) emit(f,",pc");
      }else{
	if(!drel&&CPU==6812) emit(f,",pc");
      }
    }
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
  static int dwarf_regs[MAXR+1]={-1,3,7,8,15};
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
  if(CPU!=6812) return 0;
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
static void inc_addr(struct obj *o,long val,int t)
{
  if(o->am){
    int f=o->am->flags;
    if(f==IMM_IND||f==KONSTINC)
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
  }else if((o->flags&(DREFOBJ|KONST))==DREFOBJ){
    struct AddressingMode *am;
    o->am=am=mymalloc(sizeof(*am));
    am->flags=IMM_IND;
    if(!o->reg) ierror(0);
    am->base=o->reg;
    am->offset=zm2l(val);
    am->v=0;
  }else if((o->flags&(DREFOBJ|KONST))==KONST){
    struct AddressingMode *am;
    if(o->am) ierror(0);
    o->am=am=mymalloc(sizeof(*am));
    am->flags=KONSTINC;
    am->offset=zm2l(val);
    am->base=t;
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
  if(l==1&&CPU==6812){
    emit(f,"\tins\n");
#if 0 /* might clobber return register */
  }else if(l==2&&!regs[acc]){
    emit(f,SPULLD);
    BSET(regs_modified,acc);
  }else if(l==2&&!regs[ix]){
    emit(f,SPULL("x"));
    BSET(regs_modified,ix);
  }else if(l==2&&!regs[iy]){
    emit(f,SPULL("y"));
    BSET(regs_modified,iy);
#endif
  }else{
    emit(f,"\tleas\t%u,%s\n",SGN16(l),regnames[sp]);
  }
  pop(l);
}
static void pr(FILE *f,struct IC *p)
{
  int r;
  if(pushed_acc){
    emit(f,SPULLD);
    pop(2);
    pushed_acc=0;
  }
  for(r=MAXR;r>=1;r--){
    if(regs[r]&8){
      emit(f,"\t%s%s\n",CPU==6812?"pul":"puls\t",regnames[r]);
      pop(2);
    }
    regs[r]&=~12;
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
  roff=0;
  for(i=MAXR;i>0;i--){
    if(regused[i]&&!regscratch[i]&&!regsa[i]){
      have_frame=1;
      loff+=2;
      roff+=2;
      if(i==iy) emit(f,SPUSH("y"));
      else if(i==iu){
	if(CPU!=6812&&regused[iy]){
	  emit(f,"\tpshs\tu,y\n");
	  loff+=2;roff+=2;i=iy;
	}else
	  emit(f,SPUSH("u"));
      }else
	ierror(0);
    }
  }
  if(stack_check){
    stackchecklabel=++label;
    emit(f,"\tldy\t#%s%d\n",labprefix,stackchecklabel);
    /* FIXME: banked */
    emit(f,"\t%s\t%s__stack_check\n",jsrinst,idprefix);
  }
  if(offset){
    if(CPU==6812&&offset==1)
      emit(f,SPUSH("b"));
    else if(CPU==6812&&offset==2)
      emit(f,SPUSHD);
    else
      emit(f,"\tleas\t%ld,%s\n",SGN16(-offset),regnames[sp]);
    have_frame=1;
  }
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionsende                       */
{
  int i;
  offset-=roff;
  if(offset){
    if(offset==1&&CPU==6812)
      emit(f,"\tins\n");
    else if(offset==2&&CPU==6812&&!zmeqto(szof(v->vtyp->next),l2zm(4L)))
      emit(f,SPULL("x"));
    else if(offset==2&&CPU==6812&&regused[iy])
      emit(f,SPULL("y"));
    else
      emit(f,"\tleas\t%ld,%s\n",SGN16(offset),regnames[sp]);
  }
  for(i=1;i<=MAXR;i++){
    if(regused[i]&&!regscratch[i]&&!regsa[i]){
      have_frame=1;
      if(i==iy){
	if(CPU!=6812&&regused[iu]&&!regscratch[iu]&&!regsa[iu]){
	  emit(f,"\tpuls\tu,y\n");
	  i=iu;
	}else
	  emit(f,SPULL("y"));
      }else if(i==iu) emit(f,SPULL("u"));
      else
	ierror(0);
    }
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
  zmax incmin,incmax;
  if(CPU==6812){
    incmin=l2zm(-8L);
    incmax=l2zm(8L);
  }else{
    incmin=l2zm(-2L);
    incmax=l2zm(2L);
  }
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
            if(o||!ISCHWORD(q1typ(p2))) break;
            o=&p2->q1;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
	    break; /*TODO: check what is possible */
            if(o||!ISCHWORD(q2typ(p2))) break;
            o=&p2->q2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
	    break; /*TODO: check what is possible */
            if(o||!ISCHWORD(ztyp(p2))) break;
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
      if(ISCHWORD(t)&&ISIDX(r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((c2==ADD||c2==ADDI2P||(CPU==6812&&(c2==SUB||c2==SUBIFP)))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(c2==SUB||c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	    if(zmleq(vmax,incmax)&&zmleq(incmin,vmax)){
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
      if(ISCHWORD(t)&&ISIDX(r)&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((c2==ADD||c2==ADDI2P||(CPU==6812&&(c2==SUB||c2==SUBIFP)))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(c2==SUB||c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	    if(zmleq(vmax,incmax)&&zmleq(incmin,vmax)){
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
      if(ISCHWORD(t)&&ISIDX(r)&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)){
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if((c2==ADD||c2==ADDI2P||(CPU==6812&&(c2==SUB||c2==SUBIFP)))&&(p2->q1.flags&(REG|DREFOBJ))==REG&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
	    eval_const(&p2->q2.val,p2->typf2);
	    if(c2==SUB||c2==SUBIFP) vmax=zmsub(l2zm(0L),vmax);
	    if(zmleq(vmax,incmax)&&zmleq(incmin,vmax)){
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
    if((c==ADDI2P||c==SUBIFP)&&ISHWORD(p->typf)&&((p->typf2&NQ)==NPOINTER||(p->typf2&NQ)==FPOINTER)&&isreg(z)&&((p->q2.flags&(KONST|DREFOBJ))==KONST||(!drel&&(p->q1.flags&VARADR)))){
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
      emit(f,SPUSHD);
      push(2);
      pushed_acc=1;
    }
  }
}
static int get_idx(FILE *f,IC *p)
{
  int r;
  for(r=1;r<=MAXR;r++){
    if(ISIDX(r)){
      if(!regs[r]){
	regs[r]|=4;
	return r;
      }
    }
  }
  for(r=1;r<=MAXR;r++){
    if(ISIDX(r)){
      if((!(p->q1.flags&REG)||p->q1.reg!=r)&&
	 (!(p->q2.flags&REG)||p->q2.reg!=r)&&
	 (!(p->z.flags&REG)||p->z.reg!=r)){
	emit(f,"\t%s%s\n",CPU==6812?"psh":"pshs\t",regnames[r]);
	regs[r]|=8;
	push(2);
	return r;
      }
    }
  }
  ierror(0);
}
static int get_reg(FILE *f,struct IC *p,int t)
{
  int reg;
  if(!regs[acc])
    reg=acc;
  else if(ISHWORD(t)&&!regs[ix])
    reg=ix;
#if 0
  else if(ISHWORD(t)&&!regs[iy])
    reg=iy;
#endif
  else{
    get_acc(f,p);
    reg=acc;
  }
  BSET(regs_modified,reg);
  return reg;
}
static void load_reg(FILE *f,int r,struct obj *o,int t)
{
  if(!o->am){
    if((o->flags&(REG|DREFOBJ))==REG){
      if(o->reg==r) return;
      emit(f,"\ttfr\t%s,%s\n",regnames[o->reg],regnames[r]);
      return;
    }
    if(r==acc&&(o->flags&(KONST|DREFOBJ))==KONST){
      eval_const(&o->val,t);
      if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))){
	if(CPU!=6812&&!optsize)
	  emit(f,"\tldd\t#0\n");
	else
	  emit(f,"\tclra\n\tclrb\n"); 
	cc=o;cc_t=t;
	return;
      }
    }
  }
  if(o->flags&VARADR){
    char *base=0;
    if(pcrel&&ISFUNC(o->v->vtyp->flags))
      base="pc";
    if(drel&&!ISFUNC(o->v->vtyp->flags))
      base=regnames[iu];
    if(base&&!skip_rel){
      if(ISACC(r))
	emit(f,"\ttfr\t%s,d\n",base);
      if(ISIDX(r))
	emit(f,"\tlea%s\t",regnames[r]);
      else{
	if(*base=='p') emit(f,"%s%d:\n",labprefix,++label);
	emit(f,"\taddd\t#");
      }
      emitzm(f,o->val.vmax);
      emit(f,"+");
      if(o->v->storage_class==EXTERN)
	emit(f,"%s%s",idprefix,o->v->identifier);
      else
	emit(f,"%s%ld",labprefix,zm2l(o->v->offset));
      if(ISIDX(r))
	emit(f,",%s",base);
      else if(*base=='p')
	emit(f,"-%s%d",labprefix,label);
      emit(f,"\n");
      cc=o;cc_t=t;
      return;
    }
    skip_rel=0;
  }
  emit(f,"\tld%s\t",(r==acc&&(t&NQ)==CHAR)?(CPU==6812?"ab":"b"):regnames[r]);
  emit_obj(f,o,t);emit(f,"\n");
  cc=o;cc_t=t;
}
static void store_reg(FILE *f,int r,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==r) return;
    emit(f,"\ttfr\t%s,%s\n",regnames[r],regnames[o->reg]);
  }else{
    if(r==acc&&(t&NQ)==CHAR)
      emit(f,"\tst%s\t",(CPU==6812)?"ab":"b");
    else
      emit(f,"\tst%s\t",regnames[r]);
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

  CPU=CPUOPT;

  if(g_flags[13]&USEDFLAG){
    msizetab[DOUBLE]=msizetab[LDOUBLE]=8;
    dct[DOUBLE]=dct[LDOUBLE]=".8byte";
  }
  

  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign=l2zm(1L);
  char_bit=l2zm(8L);
  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }
  for(i=1;i<=iu;i++){
    regsize[i]=l2zm(2L);regtype[i]=&ityp;
  }	
  regsize[dx]=l2zm(4L);regtype[i]=&ltyp;

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

  if(g_flags[9]&USEDFLAG) drel=1;
  if(g_flags[10]&USEDFLAG) MINADDI2P=SHORT;
  if(g_flags[11]&USEDFLAG) nodx=1;
  if(g_flags[12]&USEDFLAG) nou=1;


  if(CPU==6812) switchsubs=1;

  /*  Reserve a few registers for use by the code-generator.      */
  regsa[sp]=REGSA_NEVER;
  regscratch[sp]=0;

  if(CPU==6812||drel||nou){
    regsa[iu]=REGSA_NEVER;
    regscratch[iu]=0;
  }

  if(CPU!=6812){
    regnames[sp]="s";
    logicals[0]="or";
  }

  if(!(g_flags[6]&USEDFLAG)){
    extern int static_cse,dref_cse;
    static_cse=0;
    dref_cse=0;
  }

  if(!(g_flags[7]&USEDFLAG)){
    regsa[acc]=REGSA_TEMPS;
    regsa[dx]=REGSA_TEMPS;
  }

  if(g_flags[8]&USEDFLAG){
    pcrel=1;
    jsrinst="lbsr";
    jmpinst="lbra";
    rodataname="\t.data\n";
  }

  if(CPU==6809)
    marray[1]="__6809__";
  if(CPU==6309)
    marray[1]="__6309__";
  if(CPU==9)
    marray[1]="__TURBO9__";
  target_macros=marray;


  if(CPU==9||CPU==6812){
    declare_builtin("__mulint16",INT,INT,acc,INT,iy,1,mystrdup("\temul"));
    declare_builtin("__divint16",INT,INT,acc,INT,ix,1,mystrdup("\tidivs\n\ttfr\tx,d"));
    declare_builtin("__divuint16",UNSIGNED|INT,UNSIGNED|INT,acc,UNSIGNED|INT,ix,1,mystrdup("\tidiv\n\ttfr\tx,d"));
    declare_builtin("__modint16",INT,INT,acc,INT,ix,1,mystrdup("\tidivs"));
    declare_builtin("__moduint16",UNSIGNED|INT,UNSIGNED|INT,acc,UNSIGNED|INT,ix,1,mystrdup("\tidiv"));
  }else{
    declare_builtin("__mulint16",INT,INT,acc,INT,0,1,0);
    declare_builtin("__divint16",INT,INT,ix,INT,acc,1,0);
    declare_builtin("__divuint16",UNSIGNED|INT,UNSIGNED|INT,ix,UNSIGNED|INT,acc,1,0);
    declare_builtin("__modint16",INT,INT,ix,INT,acc,1,0);
    declare_builtin("__moduint16",UNSIGNED|INT,UNSIGNED|INT,ix,UNSIGNED|INT,acc,1,0);
  }

  /* TODO: set argument registers */
  declare_builtin("__mulint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__addint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__subint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__andint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__orint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__eorint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__negint32",LONG,LONG,0,0,0,1,0);
  declare_builtin("__lslint32",LONG,LONG,0,INT,0,1,0);

  declare_builtin("__divint32",LONG,LONG,0,LONG,0,1,0);
  declare_builtin("__divuint32",UNSIGNED|LONG,UNSIGNED|LONG,0,UNSIGNED|LONG,0,1,0);
  declare_builtin("__modint32",LONG,LONG,0,LONG,0,1,0);
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

  declare_builtin("__sint16toflt32",FLOAT,INT,0,0,0,1,0);
  declare_builtin("__uint16toflt32",FLOAT,UNSIGNED|INT,0,0,0,1,0);
  declare_builtin("__sint16toflt64",DOUBLE,INT,0,0,0,1,0);
  declare_builtin("__uint16toflt64",DOUBLE,UNSIGNED|INT,0,0,0,1,0);
  declare_builtin("__flt32tosint16",INT,FLOAT,0,0,0,1,0);
  declare_builtin("__flt32touint16",UNSIGNED|INT,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64tosint16",INT,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint16",UNSIGNED|INT,DOUBLE,0,0,0,1,0);

  declare_builtin("__sint8toflt32",FLOAT,CHAR,0,0,0,1,0);
  declare_builtin("__uint8toflt32",FLOAT,UNSIGNED|CHAR,0,0,0,1,0);
  declare_builtin("__sint8toflt64",DOUBLE,CHAR,0,0,0,1,0);
  declare_builtin("__uint8toflt64",DOUBLE,UNSIGNED|CHAR,0,0,0,1,0);
  declare_builtin("__flt32tosint8",CHAR,FLOAT,0,0,0,1,0);
  declare_builtin("__flt32touint8",UNSIGNED|CHAR,FLOAT,0,0,0,1,0);
  declare_builtin("__flt64tosint8",CHAR,DOUBLE,0,0,0,1,0);
  declare_builtin("__flt64touint8",UNSIGNED|CHAR,DOUBLE,0,0,0,1,0);



  declare_builtin("__mulint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__addint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__subint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__andint64",LLONG,LLONG,0,LLONG,0,1,0);
  declare_builtin("__orint64" ,LLONG,LLONG,0,LLONG,0,1,0);
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
  declare_builtin("__negflt32",FLOAT,FLOAT,0,0,0,1,0);
  declare_builtin("__cmpflt32",CHAR,FLOAT,0,FLOAT,0,1,0);

  declare_builtin("__addflt32 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__subflt32 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__mulflt32 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__divflt32 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__negflt32 ",DOUBLE,DOUBLE,0,0,0,1,0);
  declare_builtin("__cmpflt32 ",CHAR,DOUBLE,0,DOUBLE,0,1,0);

  declare_builtin("__addflt64 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__subflt64 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__mulflt64 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__divflt64 ",DOUBLE,DOUBLE,0,DOUBLE,0,1,0);
  declare_builtin("__negflt64 ",DOUBLE,DOUBLE,0,0,0,1,0);
  declare_builtin("__cmpflt64 ",CHAR,DOUBLE,0,DOUBLE,0,1,0);


  return 1;
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
{
  int f=t->flags&NQ;
  if(ISSCALAR(f)){
    if(ISHWORD(f)||f==CHAR)
      return acc;
    else if(ISLWORD(f))
      return dx;
  }
  return 0;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if(!ISSCALAR(t)) return 0;
  if(r==dx){
    if(ISLWORD(t)&&(optflags&2)&&!nodx) return 1;
    return 0;
  }
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
  if(r==dx){
    p->r1=acc;
    p->r2=ix;
    return 1;
  }
  return 0;
}

int cost_savings(struct IC *p,int r,struct obj *o)
{
  /*FIXME*/
  int c=p->code;
  if(r==dx){
    if(c==GETRETURN||c==SETRETURN||c==PUSH||c==ASSIGN) return 8;
    return INT_MIN;
  }
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
    if(p->q2.flags&&o!=&p->z)
      return 6;
    else
      return 6;
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
  if(c==COMPARE||c==TEST){
    if(r==ix) return 3;
    if(r==iy) return 2;
    if(r==iu) return 1;
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

/* Return name of library function, if this node should be
   implemented via libcall. */
char *use_libcall(int c,int t,int t2)
{
  static char fname[16];
  char *ret=0;

  if(c==COMPARE){
    if((t&NQ)==LLONG){
      sprintf(fname,"__cmp%sint32",(t&UNSIGNED)?"u":"s");
      ret=fname;
    }
    if(ISFLOAT(t)){
      return (t==FLOAT)?"__cmpflt32":"__cmpflt32 ";
      ret=fname;
    }

  }else{
    t&=NU;
    t2&=NU;
    if(t==LDOUBLE) t=DOUBLE;
    if(t2==LDOUBLE) t2=DOUBLE;
    if(c==CONVERT){
      if(t==t2) return 0;
      if(ISFLOAT(t)&&ISFLOAT(t2)) return 0;
#if 0
      /* currently support only 32bit flt */
      if(t==FLOAT&&t2==DOUBLE) return "__flt64toflt32";
      if(t==DOUBLE&&t2==FLOAT) return "__flt32toflt64";
#endif
      if(ISFLOAT(t)){
        sprintf(fname,"__%cint%ldtoflt%ld",(t2&UNSIGNED)?'u':'s',zm2l(sizetab[t2&NQ])*8,zm2l(sizetab[t&NQ])*8);
        ret=fname;
      }
      if(ISFLOAT(t2)){
        sprintf(fname,"__flt%ldto%cint%ld",zm2l(sizetab[t2&NQ])*8,(t&UNSIGNED)?'u':'s',zm2l(sizetab[t&NQ])*8);
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
	}else if(t==(UNSIGNED|LONG)&&(c==DIV||c==MOD||c==RSHIFT)){
	  sprintf(fname,"__%suint32",ename[c]);
	  ret=fname;
	}else if((t&NQ)==LONG){
          sprintf(fname,"__%sint32",ename[c]);
          ret=fname;
        }else{
	  sprintf(fname,"__%s%s%s%ld%s",ename[c],(c!=MULT&&(t&UNSIGNED))?"u":"",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8,(t&NQ)>=DOUBLE?" ":"");
          ret=fname;
	}
      }
    }
    if((c==MULT/*&&(CPU!=6812||(t&NQ)==LONG)*/)||c==DIV||c==MOD){
      sprintf(fname,"__%s%s%s%ld%s",ename[c],(c!=MULT&&(t&UNSIGNED))?"u":"",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8,(t&NQ)>=DOUBLE?" ":"");
      ret=fname;
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
  if(ISFLOAT(op)){
    if(ISFLOAT(tp)) return 0;
    return 1;
  }
  if(ISFLOAT(tp)) return 1;
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
    if(v->tattr&DPAGE)
      emit(f,"\t.direct\t%s%ld\n",labprefix,zm2l(v->offset));
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
      if(v->tattr&DPAGE)
	emit(f,"\t.direct\t%s%s\n",idprefix,v->identifier);
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
      if(zm2l(sizetab[t&NQ])==8){
	emit(f,",0x%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
      }
      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
    }else{
      emitval(f,&p->val,(t&NU)|UNSIGNED);
    }
  }else{
    int m=p->tree->o.flags,md=drel,mp=pcrel;
    p->tree->o.flags&=~VARADR;
    drel=0;pcrel=0;
    emit_obj(f,&p->tree->o,t&NU);
    p->tree->o.flags=m;
    drel=md;pcrel=mp;
  }
  emit(f,"\n");newobj=0;
}


static void preload(FILE *f,IC *p)
{
  int t,r;

  if((p->typf&VOLATILE)||(p->typf2&VOLATILE)||
     ((p->q1.flags&DREFOBJ)&&(p->q1.dtyp&(VOLATILE|PVOLATILE)))||
     ((p->q2.flags&DREFOBJ)&&(p->q2.dtyp&(VOLATILE|PVOLATILE)))||
     ((p->z.flags&DREFOBJ)&&(p->z.dtyp&(VOLATILE|PVOLATILE))))
    emit(f,"; volatile barrier\n");

  t=q1typ(p);
  if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ|KONST))==DREFOBJ&&ISLWORD(t)){
    r=get_idx(f,p);
    p->q1.flags&=~DREFOBJ;
    load_reg(f,r,&p->q1,INT);
    p->q1.flags|=(REG|DREFOBJ);
    p->q1.reg=r;
  }
  t=q2typ(p);
  if(!p->q2.am&&(p->q2.flags&(REG|DREFOBJ|KONST))==DREFOBJ&&ISLWORD(t)){
    r=get_idx(f,p);
    p->q2.flags&=~DREFOBJ;
    load_reg(f,r,&p->q2,INT);
    p->q2.flags|=(REG|DREFOBJ);
    p->q2.reg=r;
  }else if(isreg(z)&&(((p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->q2.reg==p->z.reg)||(p->q2.am&&p->q2.am->base==p->z.reg))){
    r=get_idx(f,p);
    p->q2.flags&=~DREFOBJ;
    load_reg(f,r,&p->q2,INT);
    p->q2.flags|=(REG|DREFOBJ);
    p->q2.reg=r;
  }
  t=ztyp(p);
  if(!p->z.am&&(p->z.flags&(REG|DREFOBJ|KONST))==DREFOBJ&&ISLWORD(t)){
    r=get_idx(f,p);
    p->z .flags&=~DREFOBJ;
    load_reg(f,r,&p->z ,INT);
    p->z .flags|=(REG|DREFOBJ);
    p->z .reg=r;
  }
}

/*  The main code-generation routine.                   */
/*  f is the stream the code should be written to.      */
/*  p is a pointer to a doubly linked list of ICs       */
/*  containing the function body to generate code for.  */
/*  v is a pointer to the function.                     */
/*  offset is the size of the stackframe the function   */
/*  needs for local variables.                          */
void gen_code(FILE *f,struct IC *fp,struct Var *v,zmax offset)
{
  int c,t,lastcomp=0,reg,short_add,bit_reverse,need_return=0;
  struct obj *bit_obj;char *bit_reg;
  static int idone;
  struct obj o;
  IC *p,*p2;
  if(v->tattr&INTERRUPT)
    ret="rti";
  else
    ret="rts"; /*FIXME: banked */
  if(DEBUG&1) printf("gen_code()\n");
  for(p=fp;p;p=p->next) clear_ext_ic(&p->ext);
  emit(f,"#off1=%ld\n",zm2l(offset));
  if(!(g_flags[5]&USEDFLAG)){
    peephole(fp);
    if(!frame_used) offset=l2zm(0L);
  }
  for(c=1;c<=MAXR;c++) regs[c]=(regsa[c]==REGSA_NEVER)?1:0;
  for(c=1;c<=MAXR;c++){
    if((regsa[c]==REGSA_NEVER||regused[c])){
      BSET(regs_modified,c);
    }
  }
  t=0;
  for(p=fp;p;p=p->next){
    c=p->code;
    if(c==ALLOCREG){ regs[p->q1.reg]=1; if(p->q1.reg==dx) regs[acc]=regs[ix]=1;}
    if(c==FREEREG){ regs[p->q1.reg]=0; if(p->q1.reg==dx) regs[acc]=regs[ix]=0;}
    if((c==LSHIFT||c==RSHIFT)&&(p->typf&NQ)>=LONG) regused[iy]=1;
    if(c==PUSH&&(p->q1.flags&(REG|DREFOBJ))!=REG){
      if(zmeqto(p->q2.val.vmax,Z1)){
	if(regs[acc]) t=(t>2)?t:2;
      }else if(zmeqto(p->q2.val.vmax,l2zm(2L))){
	if(regs[acc]&&regs[ix]&&regs[iy]&&(CPU==6812||regs[iu])) t=(t>2)?t:2;
      }else if(zmeqto(p->q2.val.vmax,l2zm(4L))){
	if(regs[acc]) t=(t>2)?t:2;
      }else{
	/* TODO: finer check */
	if(drel||!regsa[iu])
	  t=(t>8)?t:8;
	else
	  t=(t>6)?t:6;
      }
    }
  }
  emit(f,"#toff=%d\n",t);
  loff=zm2l(offset)+t;
  function_top(f,v,loff);
  stackoffset=notpopped=dontpop=maxpushed=0;
  if(!v->fi) v->fi=new_fi();
  v->fi->flags|=ALL_REGS;
  for(p=fp;p;pr(f,p),p=p->next){
    c=p->code;t=p->typf;
    if(debug_info)
      dwarf2_line_info(f,p); 
    short_add=0;
    if(c==NOP) continue;
    if(c==ALLOCREG){
      regs[p->q1.reg]=1;
      if(p->q1.reg==dx) regs[acc]=regs[ix]=1;
      continue;
    }
    if(c==FREEREG){
      regs[p->q1.reg]=0;
      if(p->q1.reg==dx) regs[acc]=regs[ix]=0;
      continue;
    }
    if(notpopped&&!dontpop){
      int flag=0;
      if(c==LABEL||c==COMPARE||c==TEST||(c>=BEQ&&c<=BRA)){
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
      if((lastcomp&UNSIGNED)||ISPOINTER(lastcomp))
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

    if(c==CONVERT&&ISLWORD(t)&&ISINT(t)&&ISLWORD(p->typf2)&&ISINT(p->typf2)){
      p->code=c=ASSIGN;
      p->q2.val.vmax=l2zm(4L);
    }

    if((p->q2.flags&REG)&&ISACC(p->q2.reg)&&(c==ADD||c==MULT||c==AND||c==OR||c==XOR)){
      obj o=p->q1;
      p->q1=p->q2;
      p->q2=o;
    }

    if(c==TEST){
      lastcomp=t;
      p->code=c=COMPARE;
      gval.vmax=l2zm(0L);
      p->q2.flags=KONST;
      eval_const(&gval,MAXINT);
      insert_const(&p->q2.val,t);
    }

    if(c==SUBPFP){
      p->code=c=SUB;
      p->typf=t=(UNSIGNED|INT);
    }



    if((c==ASSIGN||c==PUSH)&&zmeqto(p->q2.val.vmax,l2zm(4L))&&ISINT(t))
      p->typf=t=LONG;

    preload(f,p);

    if(c==ADDI2P||c==SUBIFP){
      if((p->typf2&NQ)!=HPOINTER){
	if(p->q2.flags&KONST){
	  eval_const(&p->q2.val,p->typf);
	  insert_const(&p->q2.val,p->typf2);
	  p->typf=t=(UNSIGNED|SHORT);
	}else{
	  if(ISLWORD(t)) inc_addr(&p->q2,2,t);
	  if((t&NQ)==CHAR) short_add=t;
	  p->typf=t=(UNSIGNED|SHORT);
	}
      }else if(ISHWORD(t)){
	if((t&NQ)==LLONG) 
	  inc_addr(&p->q2,4,t);
	else if((t&NQ)!=LONG) 
	  short_add=t;
	p->typf=t=(UNSIGNED|LONG);
      }
      p->code=c=(c==ADDI2P)?ADD:SUB;
    }

    if(c==COMPARE&&ISLWORD(t)){
      IC *branch=p->next;
      int r;
      while(branch&&branch->code==FREEREG) branch=branch->next;
      if(!branch) ierror(0);
      c=branch->code;
      if(c<BEQ||c>BGT) ierror(0);
      if(!regs[ix])
	r=ix;
      else
	r=get_reg(f,p,INT);
      
      if(c==BEQ||c==BNE){
	inc_addr(&p->q1,0,t);
	inc_addr(&p->q2,0,t);
	load_reg(f,r,&p->q1,INT);
	emit(f,"\t%s%s\t",CPU==6812?"cp":"cmp",regnames[r]);
	emit_obj(f,&p->q2,INT);
	emit(f,"\n");
	if(pushed_acc) emit(f,SPULLD);
	emit(f,"\tbne\t%s%d\n",labprefix,c==BEQ?++label:branch->typf);
	if(pushed_acc) emit(f,SPUSHD);
	inc_addr(&p->q1,2,t);
	inc_addr(&p->q2,2,t);
	load_reg(f,r,&p->q1,INT);
	emit(f,"\t%s%s\t",CPU==6812?"cp":"cmp",regnames[r]);
	emit_obj(f,&p->q2,INT);
	emit(f,"\n");
	pr(f,p);
	if(c==BEQ){
	  emit(f,"\tbeq\t%s%d\n",labprefix,branch->typf);
	  emit(f,"%s%d:\n",labprefix,label);
	}else
	  emit(f,"\tbne\t%s%d\n",labprefix,branch->typf);
      }else{
	inc_addr(&p->q1,0,t);
	inc_addr(&p->q2,0,t);
	load_reg(f,r,&p->q1,INT);
	emit(f,"\t%s%s\t",CPU==6812?"cp":"cmp",regnames[r]);
	emit_obj(f,&p->q2,INT);
	emit(f,"\n");
	label++;
	if(pushed_acc) emit(f,SPULLD);
	if(t&UNSIGNED){
	  if(c==BLT||c==BGT)
	    emit(f,"\tb%s\t%s%d\n",(c==BLT)?"lo":"hi",labprefix,branch->typf);
	  else
	    emit(f,"\tb%s\t%s%d\n",(c==BGE)?"lo":"hi",labprefix,label);
	}else{
	  if(c==BLT||c==BGT)
	    emit(f,"\tb%s\t%s%d\n",(c==BLT)?"lt":"gt",labprefix,branch->typf);
	  else
	    emit(f,"\tb%s\t%s%d\n",(c==BGE)?"lt":"gt",labprefix,label);
	}
	emit(f,"\tbne\t%s%d\n",labprefix,(c==BLT||c==BGT)?label:branch->typf);
	if(pushed_acc) emit(f,SPUSHD);
	inc_addr(&p->q1,2,t);
	inc_addr(&p->q2,2,t);
	load_reg(f,r,&p->q1,INT);
	emit(f,"\t%s%s\t",CPU==6812?"cp":"cmp",regnames[r]);
	emit_obj(f,&p->q2,INT);
	emit(f,"\n");
	pr(f,p);
	emit(f,"\tb%s\t%s%d\n",uccs[c-BEQ],labprefix,branch->typf);
	emit(f,"%s%d:\n",labprefix,label);
      }
      branch->code=NOP;
      continue;
    }

    if(ISLWORD(t)&&(c==LSHIFT||c==RSHIFT)){
      int cnt=-1000,i,r=0;
      int px=0,py=0,pa=0;
      if(isconst(q2)){
	eval_const(&p->q2.val,p->typf2);
	cnt=(int)zm2l(vmax);
	if(cnt==1&&compare_objects(&p->q1,&p->z)){
	  if(c==LSHIFT)
	    emit(f,"\tlsl\t");
	  else
	    emit(f,"\t%s\t",(t&UNSIGNED)?"lsr":"asr");
	  inc_addr(&p->z,c==LSHIFT?3:0,t);
	  emit_obj(f,&p->z,t);
	  emit(f,"\n");
	  emit(f,"\t%s\t",(c==LSHIFT)?"rol":"ror");
	  inc_addr(&p->z,c==LSHIFT?-1:1,t);
	  emit_obj(f,&p->z,t);
	  emit(f,"\n");
	  emit(f,"\t%s\t",(c==LSHIFT)?"rol":"ror");
	  inc_addr(&p->z,c==LSHIFT?-1:1,t);
	  emit_obj(f,&p->z,t);
	  emit(f,"\n");
	  emit(f,"\t%s\t",(c==LSHIFT)?"rol":"ror");
	  inc_addr(&p->z,c==LSHIFT?-1:1,t);
	  emit_obj(f,&p->z,t);
	  emit(f,"\n");
	  continue;
	}
      }
      inc_addr(&p->q1,2,t);
      inc_addr(&p->z,2,t);

      if(ISRACC(q2)||(regs[acc]&&!scratchreg(acc,p))){
	emit(f,SPUSHD);
	push(2);
	pa=1;
      }

      if(cnt<0||(optsize&&cnt>1)||(cnt>3&&!optspeed)){
	if(regs[ix]&&!scratchreg(ix,p)) {px=1;emit(f,SPUSH("x"));push(2);}
	if(regs[iy]&&!scratchreg(iy,p)) {py=1;emit(f,SPUSH("y"));push(2);}
      }

      if(!compare_objects(&p->q1,&p->z)){
	load_reg(f,acc,&p->q1,INT);
	store_reg(f,acc,&p->z,INT);
      }
      inc_addr(&p->q1,-2,t);
      inc_addr(&p->z,-2,t);
      load_reg(f,acc,&p->q1,INT);
      if(cnt<0||(optsize&&cnt>1)||(cnt>3&&!optspeed)){
	if((p->q2.flags&REG)&&p->q2.reg==ix){
	  if((p->z.flags&REG)&&p->z.reg==iy) ierror(0);
	}else
	  load_addr(f,ix,&p->z);
	if(ISRACC(q2)){
	  if(scratchreg(acc,p)&&(px+py==0)){
	    emit(f,SPULL("y"));
	    pop(2);pa=0;
	  }else
	    emit(f,"\tldy\t%d,%s\n",(px+py)*2,regnames[sp]);
	}else
	  load_reg(f,iy,&p->q2,p->typf2); /*TODO: types!=INT?? */
	if((p->q2.flags&REG)&&p->q2.reg==ix)
	  load_addr(f,ix,&p->z);
	if(c==LSHIFT)
	  emit(f,"\t%s\t%s__lsll\n",jsrinst,idprefix);
	else
	  emit(f,"\t%s\t%s__%s\n",jsrinst,idprefix,(t&UNSIGNED)?"lsrl":"asrl");
	if(py) {emit(f,SPULL("y"));pop(2);}
	if(px) {emit(f,SPULL("x"));pop(2);}
      }else{
	inc_addr(&p->z,c==LSHIFT?3:2,t);
	for(i=0;i<cnt;i++){
	  if(c==LSHIFT){
	    emit(f,"\tlsl\t");
	    emit_obj(f,&p->z,CHAR);
	    emit(f,"\n");
	    inc_addr(&p->z,-1,t);
	    emit(f,"\trol\t");
	    emit_obj(f,&p->z,CHAR);
	    emit(f,"\n");
	    inc_addr(&p->z,1,t);
	    emit(f,"\trolb\n");
	    emit(f,"\trola\n");
	  }else{
	    emit(f,"\t%s\n",(t&UNSIGNED)?"lsra":"asra");
	    emit(f,"\trorb\n");
	    emit(f,"\tror\t");
	    emit_obj(f,&p->z,CHAR);
	    emit(f,"\n");
	    inc_addr(&p->z,1,t);
	    emit(f,"\tror\t");
	    emit_obj(f,&p->z,CHAR);
	    emit(f,"\n");
	    inc_addr(&p->z,-1,t);
	  }
	}
	inc_addr(&p->z,c==LSHIFT?-3:-2,t);
	store_reg(f,acc,&p->z,INT);
      }
      if(pa) {emit(f,SPULLD);pop(2);}
      continue;
    }

    if(ISLWORD(t)&&c!=GETRETURN&&c!=SETRETURN&&c!=COMPARE&&c!=CONVERT&&c!=ADDRESS){
      if(c==PUSH&&isreg(q1)&&p->q1.reg==dx){
	if(CPU==6812){
	  emit(f,"\tpshd\n");
	  emit(f,"\tpshx\n");
	}else{
	  //emit(f,"\tpshs\ta,b,x\n");
	  emit(f,"\tpshs\tb,a\n");
	  emit(f,"\tpshs\tx\n");
	}
	dontpop += 4;
	push(4);
	continue;
      }
      if(c==ASSIGN&&isreg(q1)&&p->q1.reg==dx){
	inc_addr(&p->z,2,t);
	store_reg(f,ix,&p->z,INT);
	inc_addr(&p->z,-2,t);
	store_reg(f,acc,&p->z,INT);
	continue;
      }
      if(c==ASSIGN&&isreg(z)&&p->z.reg==dx){
	inc_addr(&p->q1,2,t);
	load_reg(f,ix,&p->q1,INT);
	inc_addr(&p->q1,-2,t);
	load_reg(f,acc,&p->q1,INT);
	continue;
      }
      if(c==PUSH){
	if(regs[acc]) emit(f,"\tstd\t%ld,%s\n",loff-roff-2-stackoffset,regnames[sp]);
      }else
	get_acc(f,p);
      /*TODO: acc in IC, constants */
      inc_addr(&p->q1,2,t);
      if(c==MINUS){
	if(CPU!=6812&&!optsize)
	  emit(f,"\tldd\t#0\n");
	else
	  emit(f,"\tclra\n\tclrb\n");
      }else
	load_reg(f,acc,&p->q1,INT);
      if(c==ADD||c==SUB){
	inc_addr(&p->q2,2,t);
	emit(f,"\t%s\t",c==ADD?"addd":"subd");
	emit_obj(f,&p->q2,INT);
	emit(f,"\n");
      }else if(c==ASSIGN||c==PUSH){
      }else if(c==MINUS){
	emit(f,"\tsubd\t");
	emit_obj(f,&p->q1,INT);
	emit(f,"\n");
      }else if(c==KOMPLEMENT){
	emit(f,"\tcoma\n");
	emit(f,"\tcomb\n");
      }else{
	if(c==AND)
	  emit(f,"\tandb\t");
	else if(c==OR)
	  emit(f,"\tor%sb\t",CPU==6812?"a":"");
	else if(c==XOR)
	  emit(f,"\teorb\t");
	inc_addr(&p->q2,3,t);
	emit_obj(f,&p->q2,CHAR);
	emit(f,"\n");
	if(c==AND)
	  emit(f,"\tanda\t");
	else if(c==OR)
	  emit(f,"\tor%sa\t",CPU==6812?"a":"");
	else if(c==XOR)
	  emit(f,"\teora\t");
	inc_addr(&p->q2,-1,t);
	emit_obj(f,&p->q2,CHAR);
	emit(f,"\n");
      }
      if(c==PUSH){
	if(CPU==6812)
	  emit(f,"\tpshd\n");
	else
	  emit(f,"\tpshs\tb,a\n");
	push(2);dontpop+=2;
      }else{
	inc_addr(&p->z,2,t);
	store_reg(f,acc,&p->z,INT);
      }
      inc_addr(&p->q1,-2,t);
      if(c==MINUS)
	emit(f,"\tldd\t#0\n");
      else
	load_reg(f,acc,&p->q1,INT);
      if(c==ADD)
	emit(f,"\tadcb\t");
      else if(c==SUB)
	emit(f,"\tsbcb\t");
      else if(c==AND)
	emit(f,"\tandb\t");
      else if(c==OR)
	emit(f,"\tor%sb\t",CPU==6812?"a":"");
      else if(c==XOR)
	emit(f,"\teorb\t");
      else if(c==KOMPLEMENT)
	emit(f,"\tcomb\n");
      else if(c==MINUS){
	inc_addr(&p->q1,1,t);
	emit(f,"\tsbcb\t");
	emit_obj(f,&p->q1,CHAR);
	emit(f,"\n");
      }
      if(p->q2.flags){
	inc_addr(&p->q2,-1,t);
	emit_obj(f,&p->q2,CHAR);
	emit(f,"\n");
      }
      if(c==ADD)
	emit(f,"\tadca\t");
      else if(c==SUB)
	emit(f,"\tsbca\t");
      else if(c==AND)
	emit(f,"\tanda\t");
      else if(c==OR)
	emit(f,"\tor%sa\t",CPU==6812?"a":"");
      else if(c==XOR)
	emit(f,"\teora\t");
      else if(c==KOMPLEMENT)
	emit(f,"\tcoma\n");
      else if(c==MINUS){
	inc_addr(&p->q1,-1,t);
	emit(f,"\tsbca\t");
	emit_obj(f,&p->q1,CHAR);
	emit(f,"\n");
      }
      if(p->q2.flags){
	inc_addr(&p->q2,-1,t);
	emit_obj(f,&p->q2,CHAR);
	emit(f,"\n");
      }
      if(c==PUSH){
	if(CPU==6812)
	  emit(f,"\tpshd\n");
	else
	  emit(f,"\tpshs\tb,a\n");
	push(2);dontpop+=2;
	if(regs[acc]) emit(f,"\tldd\t%ld,%s\n",loff-roff-2-stackoffset,regnames[sp]);
      }else{
	inc_addr(&p->z,-2,t);
	store_reg(f,acc,&p->z,INT);
      }
      continue;
    }

 
    if(c==COMPARE){
      int vadr;
      if(drel&&(p->q1.flags&VARADR)&&!ISFUNC(p->q1.v->vtyp->flags)) vadr=1;
      else if(drel&&(p->q2.flags&VARADR)&&!ISFUNC(p->q2.v->vtyp->flags)) vadr=2;
      else if(pcrel&&(p->q1.flags&VARADR)&&ISFUNC(p->q1.v->vtyp->flags)) vadr=1;
      else if(pcrel&&(p->q2.flags&VARADR)&&ISFUNC(p->q2.v->vtyp->flags)) vadr=2;
      else vadr=0;
      if(vadr!=1&&(vadr==2||isconst(q1)||ISRACC(q2))){
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
      /* case with two relative addresses */
      if(drel&&(p->q2.flags&VARADR)&&!ISFUNC(p->q2.v->vtyp->flags)) skip_rel=1;
      if(pcrel&&(p->q2.flags&VARADR)&&ISFUNC(p->q2.v->vtyp->flags)) skip_rel=1;
    }
#if 0
    /* TODO: fix cc */
    if(c==COMPARE&&isconst(q2)){
      eval_const(&p->q2.val,t);
      if(ISNULL()){
	if(cc&&(cc_t&NU)==(t&NU)&&compare_objects(cc,&p->q1)){
	  lastcomp=t;continue;
	}
      }
    }
#endif

    if(!short_add)
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
	  if(ISLWORD(t)){
	    get_acc(f,p);
	    load_reg(f,acc,&p->q1,to);
	    if((to&NU)==CHAR)
	      emit(f,SEX);
	    else if((to&NU)==(UNSIGNED|CHAR))
	      emit(f,"\tclra\n");
	    inc_addr(&p->z,2,t);
	    store_reg(f,acc,&p->z,INT);
	    inc_addr(&p->z,-2,t);
	    if(to&UNSIGNED){
	      emit(f,"\tclra\n\tclrb\n");
	    }else{
	      if(CPU==6812)
		emit(f,"\texg\ta,b\n");
	      else
		emit(f,"\ttfr\ta,b\n");
	      emit(f,SEX);
	      emit(f,"\ttfr\ta,b\n");
	    }
	    store_reg(f,acc,&p->z,INT);
	    continue;
	  }
	  /*emit(f,"#conv RACC=%d, regs=%d scratch=%d\n",(int)ISRACC(z),regs[acc],scratchreg(acc,p));*/
	  if(!ISRACC(z))
	    get_acc(f,p);
	  load_reg(f,acc,&p->q1,to);
	  if(to&UNSIGNED)
	    emit(f,"\tclra\n");
	  else
	    emit(f,SEX);
	  store_reg(f,acc,&p->z,t);
	  cc=&p->z;cc_t=t;
	  continue;
	}else if((to&NQ)>(t&NQ)){
	  if(!ISRACC(z)&&!ISRACC(q1))
	    get_acc(f,p);
	  if(ISLWORD(to))
	    inc_addr(&p->q1,2,to);
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
	  inc_addr(&mobj,1,t);
	  emit(f,"\tcom\t");
	  emit_obj(f,&mobj,INT);
	  emit(f,"\n");
	}
	continue;
      }
      if((!isreg(z)||p->z.reg!=acc)&&regs[acc]&&!scratchreg(acc,p))
	get_acc(f,p);
      load_reg(f,acc,&p->q1,t);
      emit(f,"\tcoma\n\tcomb\n");
      store_reg(f,acc,&p->z,t);
      continue;
    }
    if(c==MINUS){
      if((!isreg(z)||p->z.reg!=acc)&&regs[acc]&&!scratchreg(acc,p))
	get_acc(f,p);
      if(isreg(q1)){
	load_reg(f,acc,&p->q1,t);
	emit(f,"\tnega\n\tnegb\n\tsbca\t#0\n");
      }else{
	if(CPU!=6812&&!optsize)
	  emit(f,"\tldd\t#0\n");
	else
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
      if(isreg(q1)&&p->q1.reg==p->z.reg) continue;
      if(p->z.reg){
	if(ISLWORD(t)){
	  inc_addr(&p->q1,0,t);
	  load_reg(f,ix,&p->q1,INT);
	  BSET(regs_modified,ix);
	  inc_addr(&p->q1,2,t);
	}
	load_reg(f,acc,&p->q1,t);
	BSET(regs_modified,acc);

      }
      continue;
    }
    if(c==GETRETURN){
      if(isreg(z)&&p->z.reg==p->q1.reg) continue;
      if(p->q1.reg){
	if(ISLWORD(t)){
	  store_reg(f,ix,&p->z,INT);
	  BSET(regs_modified,ix);
	  inc_addr(&p->z,2,t);
	}
	store_reg(f,acc,&p->z,(t&NQ)==CHAR?t:INT);
      }
      continue;
    }
    if(c==CALL){
      int reg,jmp=0;
      cc=0;
      if(!calc_regs(p,f!=0)&&v->fi) v->fi->flags&=~ALL_REGS;
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp("__va_start",p->q1.v->identifier)){
	long of=va_offset(v)+loff+2;
	emit(f,"\ttfr\t%s,d\n",regnames[sp]);
	if(of) emit(f,"\taddd\t#%ld\n",of);
	continue;
      }
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
	    emit(f,"\t%s\t",jmpinst); /*emit(f,"\tbra\t");*/
	    /*if(!need_return) ret=0;*/ /*TODO: works with optimizer? */
	  }else{
	    emit(f,"\t%s\t",jsrinst); /*emit(f,"\tbsr\t");*/
	  }
	  if(pcrel){
	    pcrel=0;
	    emit_obj(f,&p->q1,t);
	    pcrel=1;
	  }else
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
	unsigned long size;int qr=0,zr=0,cr=0,px=0,py=0,pu=0,pd=0,lq=0,lz=0;
	size=zm2l(p->q2.val.vmax);
	if(c==ASSIGN){
	  if(!p->z.am&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&ISIDX(p->z.reg)){
	    zr=p->z.reg;lz=1;
	  }
	}
	if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&ISIDX(p->q1.reg)&&p->q1.reg!=zr){
	  qr=p->q1.reg;lq=1;
	}
	if(!qr){
	  if(zr==ix) qr=iy;
	  else if(zr==iy||zr==iu) qr=ix;
	  else{qr=ix;zr=iy;}
	}else if(!zr){
	  if(qr==ix) zr=iy; else zr=ix;
	}
	if(CPU!=6812){
	  if(qr!=iu&&zr!=iu) cr=iu;
	  if(qr!=ix&&zr!=ix) cr=ix;
	  if(qr!=iy&&zr!=iy) cr=iy;
	  if(!cr) ierror(0);
	}
	if(c==PUSH){
	  emit(f,"\tleas\t%ld,%s\n",SGN16(-size),regnames[sp]);
	  push(size);
	}
	if(CPU!=6812&&(drel||!regused[iu]||(regs[iu]&&!scratchreg(iu,p)))){
	  if(c==PUSH)
	    emit(f,"\tstu\t%ld,%s\n",loff-roff-8-stackoffset,regnames[sp]);
	  else{
	    emit(f,SPUSH("u"));
	    push(2);
	  }
	  pu=1;
	}
	if(!regused[iy]||(regs[iy]&&!scratchreg(iy,p))){
	  if(c==PUSH)
	    emit(f,"\tsty\t%ld,%s\n",loff-roff-4-stackoffset,regnames[sp]);
	  else{
	    emit(f,SPUSH("y"));
	    push(2);
	  }
	  py=1;
	}
	if(regs[ix]&&!scratchreg(ix,p)){
	  if(c==PUSH)
	    emit(f,"\tstx\t%ld,%s\n",loff-roff-2-stackoffset,regnames[sp]);
	  else{
	    emit(f,SPUSH("x"));
	    push(2);
	  }
	  px=1;
	}
	if(!lq) load_addr(f,qr,&p->q1);
	if(c==PUSH)
	  emit(f,"\ttfr\t%s,%s\n",regnames[sp],regnames[zr]);
	else
	  if(!lz) load_addr(f,zr,&p->z);
	if(size<=6||(size<=16&&!optsize)){
	  if(CPU!=6812){
	    if(!scratchreg(acc,p)){
	      if(c==PUSH)
		emit(f,"\tstd\t%ld,%s\n",loff-roff-6-stackoffset,regnames[sp]);
	      else{
		emit(f,SPUSHD);
		push(2);
	      }
	      pd=2;
	    }
	  }
	  while(size>2){
	    if(CPU==6812)
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    else
	      emit(f,"\tldd\t,%s++\n\tstd\t,%s++\n",regnames[qr],regnames[zr]);
	    size-=2;
	  }
	  if(CPU==6812)
	    emit(f,"\tmov%c\t0,%s,0,%s\n",size==2?'w':'b',regnames[qr],regnames[zr]);
	  else
	    emit(f,"\tld%c\t,%s\n\tst%c\t,%s\n",size==2?'d':'b',regnames[qr],size==2?'d':'b',regnames[zr]);
	}else{
	  int l=++label,cnt=(int)(optsize?(CPU==6812?size:(size/2)):size/8);
	  if(regs[acc]&&!scratchreg(acc,p)){
	    if(c==PUSH)
	      emit(f,"\tst%c\t%ld,%s\n",(CPU!=6812&&cnt<=255)?'b':'d',loff-roff-6-stackoffset,regnames[sp]);
	    else{
	      if(CPU!=6812&&cnt<=255){
		emit(f,SPUSH("b"));
		push(1);
	      }else{
		emit(f,SPUSHD);
		push(2);
	      }
	    }
	    pd=(CPU!=6812&&cnt<=255)?1:2;
	  }
	  if(CPU!=6812&&cnt<=255)
	    emit(f,"\tldb\t#%lu\n",cnt);
	  else
	    emit(f,"\tldd\t#%lu\n",cnt);
	  cc=0;
#if 0
	  if(CPU!=6812&&((!regsa[iu]&&regs[iu])||drel)){
	    if(c==PUSH){
	      emit(f,"\tstu\t%ld,%s\n",loff-roff-8-stackoffset,regnames[sp]); 
	    }else{
	      emit(f,SPUSH("u"));push(2);
	    }
	  }
#endif
	  emit(f,"%s%d:\n",labprefix,l);
	  if(CPU==6812){
	    if(optsize){
	      emit(f,"\tmovb\t1,%s+,1,%s+\n",regnames[qr],regnames[zr]);
	      size=0;
	    }else{
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	      size=size&7;
	    }
	    emit(f,"\tdbne\td,%s%d\n",labprefix,l);
	  }else{
	    if(optsize){
	      emit(f,"\tld%s\t,%s++\n\tst%s\t,%s++\n",regnames[cr],regnames[qr],regnames[cr],regnames[zr]);
	      size&=1;
	    }else{
	      emit(f,"\tld%s\t,%s++\n\tst%s\t,%s++\n",regnames[cr],regnames[qr],regnames[cr],regnames[zr]);
	      emit(f,"\tld%s\t,%s++\n\tst%s\t,%s++\n",regnames[cr],regnames[qr],regnames[cr],regnames[zr]);
	      emit(f,"\tld%s\t,%s++\n\tst%s\t,%s++\n",regnames[cr],regnames[qr],regnames[cr],regnames[zr]);
	      emit(f,"\tld%s\t,%s++\n\tst%s\t,%s++\n",regnames[cr],regnames[qr],regnames[cr],regnames[zr]);
	      size&=7;
	    }
	    if(cnt<=255)
	      emit(f,"\tdecb\n");
	    else
	      emit(f,"\tsubd\t#1\n");
	    emit(f,"\tbne\t%s%d\n",labprefix,l);
	  }
#if 0
	  if(CPU!=6812&&((!regsa[iu]&&regs[iu])||drel)){
	    if(c==PUSH){
	      emit(f,"\tldu\t%ld,%s\n",loff-roff-8-stackoffset,regnames[sp]);
	    }else{
	      emit(f,SPULL("u"));pop(2);
	    }
	  }
#endif
	  while(size>=2){
	    if(CPU==6812)
	      emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
	    else
	      emit(f,"\tldd\t,%s++\n\tstd\t,%s++\n",regnames[qr],regnames[zr]);
	    size-=2;
	  }
	  if(size){
	    if(CPU==6812)
	      emit(f,"\tmovb\t0,%s,0,%s\n",regnames[qr],regnames[zr]);
	    else
	      emit(f,"\tldb\t,%s\n\tstb\t,%s\n",regnames[qr],regnames[zr]);
	  }
	}
	if(pd){
	  if(c==PUSH) 
	    emit(f,"\tld%c\t%ld,%s\n",(pd==1)?'b':'d',loff-roff-6-stackoffset,regnames[sp]);
	  else{
	    if(pd==1){
	      emit(f,SPULL("b"));
	      pop(1);
	    }else{
	      emit(f,SPULLD);
	      pop(2);
	    }
	  }
	}
	if(px){
	  if(c==PUSH) 
	    emit(f,"\tldx\t%ld,%s\n",loff-roff-2-stackoffset,regnames[sp]);
	  else{
	    emit(f,SPULL("x"));
	    pop(2);
	  }
	}
	if(py){
	  if(c==PUSH) 
	    emit(f,"\tldy\t%ld,%s\n",loff-roff-4-stackoffset,regnames[sp]);
	  else{
	    emit(f,SPULL("y"));
	    pop(2);
	  }
	}
	if(pu){
	  if(c==PUSH) 
	    emit(f,"\tldu\t%ld,%s\n",loff-roff-8-stackoffset,regnames[sp]);
	  else{
	    emit(f,SPULL("u"));
	    pop(2);
	  }
	}
	continue;
      }
      if(!ISSCALAR(t)) t=zmeqto(p->q2.val.vmax,l2zm(1L))?CHAR:INT;
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
      if(((regs[acc]&&regs[ix])||(t&NQ)==CHAR)&&(p->q1.flags&KONST)&&!isreg(z)){
	eval_const(&p->q1.val,t);
	if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&((p->z.flags&(REG|DREFOBJ))!=DREFOBJ||(t&NQ)==CHAR)&&(!p->z.am||p->z.am->flags!=ACC_IND||(t&NQ)==CHAR)){
	  emit(f,"\tclr\t");
	  if(c==ASSIGN){
	    emit_obj(f,&p->z,t);emit(f,"\n");
	  }else{
	    emit(f,CPU==6812?"1,-sp\n":",-s\n");
	    push(1);
	  }
	  if(!ISHWORD(t)) continue;
	  emit(f,"\tclr\t");
	  if(c==ASSIGN){
	    mobj=p->z;
	    inc_addr(&mobj,1,t);
	    emit_obj(f,&mobj,t);emit(f,"\n");
	  }else{
	    emit(f,CPU==6812?"1,-sp\n":",-s\n");
	    push(1);
	  }
	  continue;
	}

      }
      if(c==PUSH){
	int st=0;
	if(isreg(q1)){
	  reg=p->q1.reg;
	}else{
	  if((t&NQ)==CHAR||!regs[acc]||scratchreg(acc,p)) reg=acc;
	  else if(!regs[ix]||scratchreg(ix,p)) reg=ix;
	  else if(regused[iy]&&(!regs[iy]||scratchreg(iy,p))) reg=iy;
	  else if(regused[iu]&&!drel&&CPU!=6812&&(!regs[iu]||scratchreg(iu,p))) reg=iu;
	  else reg=acc;
	  if(regs[reg]&&!scratchreg(reg,p)){
	    st=1;
	    emit(f,"\tst%s\t%ld,%s\n",regnames[reg],(long)loff-roff-2-stackoffset,regnames[sp]);
	  }
	  load_reg(f,reg,&p->q1,t);
	}
	if((t&NQ)==CHAR)
	  emit(f,SPUSH("b"));
	else if(reg==ix)
	  emit(f,SPUSH("x"));
	else if(reg==iy)
	  emit(f,SPUSH("y"));
	else if(reg==iu)
	  emit(f,SPUSH("u"));
	else
	  emit(f,SPUSHD);
	push(zm2l(p->q2.val.vmax));
	if(st)
	  emit(f,"\tld%s\t%ld,%s\n",regnames[reg],(long)loff-roff-2-stackoffset,regnames[sp]);
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
	emit(f,SPUSH("x"));
	reg=ix;
	push(2);
      }
      load_addr(f,reg,&p->q1);
      if(!(p->z.flags&REG)||p->z.reg!=reg)
	store_reg(f,reg,&p->z,p->typf2);
      if(px){
	emit(f,SPULL("x"));
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


    if((c==ADD||c==SUB)&&(p->q2.flags&(KONST|DREFOBJ))==KONST&&(p->q1.flags&(REG|DREFOBJ))!=REG&&(p->q1.flags&(REG|DREFOBJ))!=DREFOBJ&&!p->q1.am&&!p->z.am&&compare_objects(&p->q1,&p->z)){
      eval_const(&p->q2.val,t);
      if(c==SUB) vmax=zmsub(Z0,vmax);
      if((t&NQ)==CHAR&&zmeqto(vmax,Z1)){
	emit(f,"\tinc\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	continue;
      }else if((t&NQ)==CHAR&&zmeqto(vmax,l2zm(-1L))){
	emit(f,"\tdec\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	continue;
      }else if(((t&NQ)==SHORT||(t&NQ)==INT)&&zmeqto(vmax,l2zm(1L))){
	inc_addr(&p->z,1,t);
	emit(f,"\tinc\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	emit(f,"\tbne\t%s%d\n",labprefix,++label);
	inc_addr(&p->z,-1,t);
	emit(f,"\tinc\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	emit(f,"%s%d:\n",labprefix,label);
	continue;
      }else if(regs[acc]&&((t&NQ)==SHORT||(t&NQ)==INT)&&zmeqto(vmax,l2zm(-1L))){
	inc_addr(&p->z,1,t);
	emit(f,"\ttst\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	emit(f,"\tbne\t%s%d\n",labprefix,++label);
	inc_addr(&p->z,-1,t);
	emit(f,"\tdec\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	emit(f,"%s%d:\n",labprefix,label);
	inc_addr(&p->z,1,t);
	emit(f,"\tdec\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	continue;
      }
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
      if((c==ADD||c==ADDI2P||c==MULT||(c>=OR&&c<=AND))&&isreg(q2)&&!isreg(q1)&&!short_add){
	o=p->q1;p->q1=p->q2;p->q2=o;
      }
      if((c==ADD||c==MULT||(c>=OR&&c<=AND))&&isreg(q2)&&p->q2.reg==acc&&!short_add){
	o=p->q1;p->q1=p->q2;p->q2=o;
      }
      if(c==MULT||c==MOD){
	if((!isreg(z)||p->z.reg!=acc)&&regs[acc]&&!scratchreg(acc,p))
	  get_acc(f,p);
	reg=acc;
	/*FIXME: y bzw. x-Register*/
      }else if(c==LSHIFT||c==RSHIFT||c==AND||c==OR||c==XOR){
	if((!isreg(z)||p->z.reg!=acc)&&regs[acc]&&!scratchreg(acc,p))
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
	  if(ISRACC(q2))
	    reg=acc;
	  else
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
	    eval_const(&p->q2.val,short_add?short_add:q2typ(p));
	    if(CPU==6812&&p->q1.reg==reg&&zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))){
	      emit(f,"\t%s%s\n",c==SUB?"de":"in",regnames[reg]);
	      /*FIXME: condition-codes for bne/beq could be used */
	    }else{
	      emit(f,"\tlea%s\t%ld,%s\n",regnames[reg],c==SUB?SGN16(-zm2l(vmax)):SGN16(zm2l(vmax)),regnames[p->q1.reg]);
	    }
	    opdone=1;
	  }else	if((c==ADD||c==ADDI2P)&&ISIDX(reg)&&ISIDX(p->q1.reg)&&ISRACC(q2)){
	    emit(f,"\tlea%s\t%s,%s\n",regnames[reg],((t&NQ)==CHAR||(short_add&NQ)==CHAR)?"b":"d",regnames[p->q1.reg]);
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&ISIDX(reg)&&ISACC(p->q1.reg)&&ISRIDX(q2)){
	    emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[p->q2.reg]);
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&p->q1.reg==acc&&ISIDX(reg)&&!short_add){
	    load_reg(f,reg,&p->q2,t);
	    emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[reg]);
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&p->q1.reg==acc&&ISIDX(reg)&&(short_add&NU)==(UNSIGNED|CHAR)&&scratchreg(acc,p)){
	    emit(f,"\taddb\t");
	    emit_obj(f,&p->q2,short_add);
	    emit(f,"\n");
	    emit(f,"\tadca\t#0\n");
	    emit(f,"\ttfr\td,y\n");
	    opdone=1;
	  }else if((c==ADD||c==ADDI2P)&&ISACC(p->q1.reg)&&ISRACC(z)&&isreg(q2)&&ISIDX(p->q2.reg)){
	    if(!scratchreg(p->q2.reg,p)) emit(f,"\texg\t%s,%s\n",regnames[acc],regnames[p->q2.reg]);
	    emit(f,"\tlea%s\t%s,%s\n",regnames[p->q2.reg],regnames[acc],regnames[p->q2.reg]);
	    emit(f,"\texg\t%s,%s\n",regnames[acc],regnames[p->q2.reg]);
	    opdone=1;
	  }else	if(p->q1.reg!=reg){
	    if(c==ADD||c==ADDI2P||!ISRACC(q2))
	      emit(f,"\ttfr\t%s,%s\n",regnames[p->q1.reg],regnames[reg]);
	  }
	}else if(short_add&&c==SUB&&reg==acc&&!(short_add&UNSIGNED)){
	  load_reg(f,reg,&p->q2,short_add);
	  emit(f,"\tclra\n");
	  emit(f,"\tnegb\n");
	  emit(f,"\tsbca\t#0\n");
	  emit(f,"\taddd\t");
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	  store_reg(f,reg,&p->z,ztyp(p));
	  continue;
	}else if(short_add&&c==ADD&&reg==acc){
	  load_reg(f,reg,&p->q2,short_add);
	  if(short_add&UNSIGNED)
	    emit(f,"\tclra\n");
	  else
	    emit(f,SEX);
	  emit(f,"\taddd\t");
	  emit_obj(f,&p->q1,t);
	  emit(f,"\n");
	  store_reg(f,reg,&p->z,ztyp(p));
	  continue;
	}else{
	  if(!ISRACC(q2))
	    load_reg(f,reg,&p->q1,q1typ(p));
	}
	if(!opdone){
	  if(reg==acc){
	    if(ISRACC(q2)){
	      if(!ISRACC(z)) get_acc(f,p);
	      if(c==ADD||c==ADDI2P){
		if(short_add){
		  if(short_add&UNSIGNED)
		    emit(f,"\tclra\n");
		  else
		    emit(f,SEX);
		  emit(f,"\taddd\t");
		  emit_obj(f,&p->q1,t);
		  emit(f,"\n");
		}else{
		  if(CPU==6812)
		    emit(f,"\tasld\n"); /* only cases with q1=q2=acc should remain */
		  else{
		    emit(f,"\taslb\n");
		    if((t&NQ)!=CHAR)
		      emit(f,"\trola\n");
		  }
		}
	      }else{
		if(short_add){
		  if(short_add&UNSIGNED)
		    emit(f,"\tld%sa\t#255\n",(CPU==6812)?"a":"");
		  else{
		    emit(f,SEX);
		    emit(f,"\tnega\n");
		  }
		  emit(f,"\tnegb\n\tsbca\t#0\n");
		}else if((t&NQ)!=CHAR){
		  emit(f,"\tnega\n\tnegb\n\tsbca\t#0\n");
		}else{
		  emit(f,"\tnegb\n");
		}
		
		if(ISRIDX(q1)){
		  emit(f,"\t%s%s\n",CPU==6812?"psh":"pshs\t",regnames[p->q1.reg]);
		  emit(f,"\taddd\t%s\n",CPU==6812?"1,s+":",s++");
		}else{
		  emit(f,"\taddd\t");
		  emit_obj(f,&p->q1,t);
		  emit(f,"\n");
		}
	      }
	    }else{
	      if(ISRIDX(q2)){
		if(CPU==6812)
		  emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
		else
		  emit(f,"\tpshs\t%s\n",regnames[p->q2.reg]);
		push(2);pop(2);
		if(CPU==6812)
		  emit(f,"\t%sd\t2,%s+\n",(c==ADD||c==ADDI2P)?"add":"sub",regnames[sp]);
		else
		  emit(f,"\t%sd\t,%s++\n",(c==ADD||c==ADDI2P)?"add":"sub",regnames[sp]);
	      }else{
		emit(f,"\t%s%s\t",(c==ADD||c==ADDI2P)?"add":"sub",(short_add||(t&NQ)==CHAR)?"b":"d");
		emit_obj(f,&p->q2,short_add?short_add:t);emit(f,"\n");
		if(short_add){
		  if(short_add&UNSIGNED)
		    emit(f,"\t%s\t#0\n",c==ADD?"adca":"sbca");
		  else
		    ierror(0);
		}
		if(drel&&(p->q2.flags&VARADR)){
		  if(CPU==6812) ierror(0);
		  emit(f,"\tpshs\t%s\n",regnames[iu]);push(2);
		  emit(f,"\t%sd\t,%s++\n",(c==ADD||c==ADDI2P)?"add":"sub",regnames[sp]);
		  pop(2);
		}
	      }
	    }
	    cc=&p->z;cc_t=t;
	  }else{
	    if(isconst(q2)){
	      long l;
	      eval_const(&p->q2.val,short_add?short_add:t);
	      l=zm2l(vmax);
	      if(c==SUB) l=-l;
	      /*FIXME: condition-codes for bne/beq could be used */
	      if(l==1&&reg==ix&&CPU==6812){
		emit(f,"\tinx\n");
	      }else if(l==1&&reg==iy&&CPU==6812){
		emit(f,"\tiny\n");
	      }else if(l==-1&&reg==ix&&CPU==6812){
		emit(f,"\tdex\n");
	      }else if(l==-1&&reg==iy&&CPU==6812){
		emit(f,"\tdey\n");
	      }else{
		emit(f,"\tlea%s\t%ld,%s\n",regnames[reg],SGN16(l),regnames[reg]);
	      }
	    }else{
	      if(c!=ADD&&c!=ADDI2P){
		if(!ISRACC(q2)){
		  if(!scratchreg(acc,p))
		    get_acc(f,p);
		  load_reg(f,acc,&p->q2,t);
		  if((t&NQ)!=CHAR){
		    emit(f,"\tnega\n\tnegb\n\tsbca\t#0\n");
		  }else{
		    emit(f,"\tnegb\n");
		  }
		  /*load_reg(f,reg,&p->q1,t);*/
		}else{
		  get_acc(f,p);
		  load_reg(f,reg,&p->q1,t);
		  if((t&NQ)==CHAR)
		    emit(f,"\tnegb\n");
		  else
		    emit(f,"\tnega\n\tnegb\n\tsbca\t#0\n");
		}  
	      }else if(!ISRACC(q2)){
		get_acc(f,p);
		if(short_add){
		  load_reg(f,acc,&p->q2,short_add);
		  if(short_add&UNSIGNED){
		    if(reg==ix){
		      emit(f,"\tabx\n");
		      store_reg(f,reg,&p->z,ztyp(p));
		      continue;
		    }else{
		      emit(f,"\tclra\n");
		    }
		  }else
		    t=CHAR;
		}else
		  load_reg(f,acc,&p->q2,t);
	      }else{
		load_reg(f,reg,&p->q1,t);
		if(short_add&UNSIGNED)
		  emit(f,"\tclra\n");
		emit(f,"\tlea%s\t%s,%s\n",regnames[reg],((t&NU)==CHAR||(short_add&NU)==CHAR)?"b":"d",regnames[reg]);
		store_reg(f,reg,&p->z,ztyp(p));
		continue;
	      }
	      emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[reg]);
	    }
	  }
	}
	store_reg(f,reg,&p->z,ztyp(p));
	continue;
      }
      if(c!=LSHIFT&&c!=RSHIFT)
	load_reg(f,reg,&p->q1,t);
      if(c==MULT){
	if(CPU==6812){
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
	    emit(f,SPULL("y"));
	    pop(2);
	  }
	  store_reg(f,acc,&p->z,t);
	  continue;
	}else
	  ierror(0);
      }
      if(c==LSHIFT||c==RSHIFT){
	if(isconst(q2)){
	  int l,oldl;
	  load_reg(f,acc,&p->q1,t);
	  eval_const(&p->q2.val,t);
	  oldl=l=zm2l(vmax);
	  if(l>=16){
	    if(CPU!=6812&&!optsize)
	      emit(f,"\tldd\t#0\n");
	    else
	      emit(f,"\tclra\n\tclrb\n");
	    l=0;
	  }
	  if(l>=8){
	    if(c==LSHIFT)
	      emit(f,"\t%s\n\tclrb\n",(CPU==6812)?"tba":"tfr\tb,a");
	    else{
	      if(t&UNSIGNED)
		emit(f,"\ttfr\ta,b\n\tclra\n");
	      else{
		emit(f,"\ttfr\ta,b\n");
		emit(f,SEX);
	      }
	    }
	    l-=8;
	  }
	  while(l--){
	    if(c==RSHIFT){
	      if(t&UNSIGNED){
		if((t&NQ)==CHAR)
		  emit(f,"\tlsrb\n");
		else
		  emit(f,CPU!=6812?"\tlsra\n\trorb\n":"\tlsrd\n");
	      }else{
		if(oldl>8||(t&NQ)==CHAR)
		  emit(f,"\tasrb\n");
		else
		  emit(f,"\tasra\n\trorb\n");
	      }
	    }else{
	      if((t&NQ)==CHAR)
		emit(f,"\taslb\n");
	      else
		emit(f,CPU!=6812?"\taslb\n\trola\n":"\tasld\n");
	    }
	  }
	}else{
	  int px;char *s;
	  if(regs[ix]&&!scratchreg(ix,p)&&(!isreg(z)||p->z.reg!=ix)){
	    emit(f,SPUSH("x"));
	    push(2);px=1;
	  }else
	    px=0;
	  if(!isreg(q2)||p->q2.reg!=acc)
	    load_reg(f,acc,&p->q1,p->typf);
	  if((p->typf2&NQ)==CHAR){
	    load_reg(f,acc,&p->q2,p->typf2);
	    emit(f,(p->typf2&UNSIGNED)?"\tclra\n":SEX);
	    emit(f,"\ttfr\td,x\n");
	  }else
	    load_reg(f,ix,&p->q2,t);
	  if(isreg(q2)&&p->q2.reg==acc)
	    load_reg(f,acc,&p->q1,p->typf);
	  if((t&NQ)==CHAR)
	    emit(f,(p->typf2&UNSIGNED)?"\tclra\n":SEX);
	  if(c==LSHIFT) s="lsl";
	  else if(t&UNSIGNED) s="lsr";
	  else s="asr";
	  emit(f,"\t.global\t%s__%s\n",idprefix,s);
	  emit(f,"\t%s\t%s__%s\n",jsrinst,idprefix,s);
	  if(px){
	    emit(f,SPULL("x"));
	    /*emit(f,"\tpul%s\n",regnames[ix]);*/
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
	  if((t&NQ)!=CHAR){
	    h=(l>>8)&255;
	    if(c==AND&&h==0)
	      emit(f,"\tclra\n");
	    else if(c==XOR&&h==255)
	      emit(f,"\tcoma\n");
	    else if((c==AND&&h!=255)||(c==OR&&h!=0)||(c==XOR&&h!=0))
	      emit(f,"\t%sa\t#%lu\n",s,h);
	  }
	  h=l&255;
	  if(c==AND&&h==0)
	    emit(f,"\tclrb\n");
	  else if(c==XOR&&h==255)
	    emit(f,"\tcomb\n");
	  else if((c==AND&&h!=255)||(c==OR&&h!=0)||(c==XOR&&h!=0))
	    emit(f,"\t%sb\t#%lu\n",s,h);
	}else{
	  if(isreg(q2)){
	    if(p->q2.reg==acc){
	      if(c==XOR){
		emit(f,"\tclrb\n");
		if((t&NQ)!=CHAR) emit(f,"\tclra\n");
	      }
	    }else{
	      if((t&NQ)==CHAR){
		emit(f,SPUSH("a"));
		push(1);
		emit(f,"\t%sa\t%s,%s+\n",s,CPU==6812?"1":"",regnames[sp]);
		pop(1);
	      }else{
		emit(f,"\t%s%s\n",(CPU==6812)?"psh":"pshs\t",regnames[p->q2.reg]);
		push(2);
		emit(f,"\t%sa\t%s,%s+\n",s,CPU==6812?"1":"",regnames[sp]);
		emit(f,"\t%sb\t%s,%s+\n",s,CPU==6812?"1":"",regnames[sp]);
		pop(2);
	      }
	    }
	  }else if((p->q2.flags&(REG|DREFOBJ))==DREFOBJ&&(t&NQ)!=CHAR){
	    int xr=0;
	    if(!regs[ix]) xr=ix;
	    else if(!regs[iy]) xr=iy;
	    else{
	      xr=ix;
	      emit(f,"\t%s%s\n",(CPU==6812)?"psh":"pshs\t",regnames[xr]);
	      push(2);

	    }
	    BSET(regs_modified,xr);
	    load_addr(f,xr,&p->q2);
	    if((t&NQ)==CHAR)
	      emit(f,"\t%sb\t0,%s\n",s,regnames[xr]);
	    else{
	      emit(f,"\t%sa\t0,%s\n",s,regnames[xr]);
	      emit(f,"\t%sb\t1,%s\n",s,regnames[xr]);
	    }
	    if(regs[ix]&&xr==ix){
	      emit(f,SPULL("x"));
	      pop(2);
	    }
	  }else{
	    emit(f,"\t%sb\t",s);
	    if((t&NQ)!=CHAR) inc_addr(&p->q2,1,t);
	    emit_obj(f,&p->q2,t);
	    emit(f,"\n");
	    if((t&NQ)!=CHAR){
	      inc_addr(&p->q2,-1,t);
	      emit(f,"\t%sa\t",s);
	      emit_obj(f,&p->q2,t);
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
	  emit(f,"\t%s%s\n",(CPU==6812)?"psh":"pshs\t",regnames[p->q2.reg]);
	  push(2);
	}
	if(reg==acc){
	  if((t&NQ)==CHAR)
	    emit(f,"\tcmpb\t");
	  else
	    emit(f,SCMP("d"));
	}else if(reg==ix){
	  emit(f,SCMP("x"));
	}else if(reg==iy){
	  emit(f,SCMP("y"));
	}else if(reg==iu){
	  emit(f,SCMP("u"));
	}else
	  ierror(0);
	if(isreg(q2)){
	  if(CPU==6812)
	    emit(f,"2,%s+\n",regnames[sp]);
	  else
	    emit(f,",%s++\n",regnames[sp]);
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
  if((c==LSHIFT||c==RSHIFT)&&ISCHWORD(t&NQ)) return 1;
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
  if((t&NQ)==NPOINTER) t=(UNSIGNED|INT);
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

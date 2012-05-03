/*  $VER: vbcc (main.c) V0.8    */
#include "vbcc_cpp.h"
#include "vbc.h"
#include "opt.h"
static char FILE_[]=__FILE__;
void do_function(struct Var *);
static struct function_info *current_fi;
struct token *ctok;
struct lexer_state ls;
int endok=1;
int line,errors;
bvtype task_preempt_regs[RSIZE/sizeof(bvtype)];
bvtype task_schedule_regs[RSIZE/sizeof(bvtype)];
char *multname[]={"","s"};
void raus(void)
/*  Beendet das Programm                                            */
{
  static int inraus;
  if(inraus) return;
  inraus = 1;
  if(DEBUG) printf("raus()\n");
  if(!endok) fprintf(stderr,"unexpected end of file\n");
  if(errors) fprintf(stderr,"%d error%s found!\n",errors,multname[errors>1]);
  if(debug_info&&out)
    cleanup_db(out);
  while(nesting>=0) leave_block();
  /*FIXME: do I have to close input-file? */
  if(!wpo)
    cleanup_cg(out);
  emit_flush(out);
  if(cmdfile) fclose(cmdfile);
  if(out) fclose(out);
  if(ic1) fclose(ic1);
  if(ic2) fclose(ic2);
  /*FIXME: need to cleanup something for ucpp?*/;
  if(endok&&!errors) exit(EXIT_SUCCESS); else exit(EXIT_FAILURE);
}
int eof;
void translation_unit(void)
/*  bearbeitet translation_unit                                     */
/*  hier z.Z. nur provisorisch                                      */
{
  struct Var *p;
  if(cross_module){
    for(p=first_ext;p;p=p->next)
      p->flags|=NOTINTU;
  }
  while(1){
    killsp();
    if(eof||ctok->type!=NAME){
      if(!eof){
	error(0);
	raus();
      }else{
	if(cross_module){
	  int n=0;
	  if(last_tunit){
	    last_tunit->next=mymalloc(sizeof(*first_tunit));
	    last_tunit=last_tunit->next;
	  }else{
	    first_tunit=last_tunit=mymalloc(sizeof(*first_tunit));
	  }
	  last_tunit->next=0;
	  for(p=first_var[0];p;p=p->next){
	    if(p->storage_class==STATIC) n++;
	  }
	  last_tunit->statics=first_var[0];
	  return;
	}else{
	  raus();
	}
      }
    }
    endok=0;
    var_declaration();
    endok=1;
  }
}
void reserve_reg(char *p)
     /* reserviert ein Register */
{
  int i;
  if(*p!='=') error(4,"-reserve-reg");
  for(i=1;i<=MAXR;i++){
    if(!strcmp(p+1,regnames[i]))
      break;
  }
  if(i>MAXR){
    error(331,p+1);
  }else{
    regsa[i]=1;
  }
}

void dontwarn(char *p)
/*  schaltet flags fuer Meldung auf DONTWARN    */
{
  int i;
  if(*p!='=') error(4,"-dontwarn");
  i=atoi(p+1);
  if(i>=err_num) error(159,i);
  if(i<0){
    for(i=0;i<err_num;i++)
      if(!(err_out[i].flags&(ANSIV|FATAL)))
	err_out[i].flags|=DONTWARN;
    return;
  }
  if(err_out[i].flags&(ANSIV|FATAL)) error(160,i);
  err_out[i].flags|=DONTWARN;
}



#define MISRA_98_RULE_NUMBER	127
#define MISRA_04_CHAPTER			21
#define MISRA_04_MAX_RULE_IN_CHAPTER 17

static int misra_98_warn_flag[MISRA_98_RULE_NUMBER] = { 0 };
static int misra_04_warn_flag[MISRA_04_CHAPTER][MISRA_04_MAX_RULE_IN_CHAPTER] = { 0 };

void misrawarn(char *p) {
	int rule,subrule,misraoldrule;
	char* last;
	int not_found;
	struct misra_err_out* misr_err;
	rule = 0;
	subrule = 0;
	misraoldrule = 0;
  if(*p!='=') error(4,"-misrawarn");
	p++;
	if (!(strncmp("chapter",p,6))) {
		if (sscanf((p+7),"%d",&rule) != 1) {
			error(327,"-misrawarn");
		}
	} else if ( !(strncmp("misra98rule",p,11))) {
		if (sscanf((p+12),"%d",&misraoldrule) != 1) {
			error(327,"-misrawarn");
		}
	} else {
		if (last = strchr(p,'.')) {
			*last = 0;
			last++;
			if (sscanf(last,"%d",&subrule) != 1) error(327,"-misrawarn");
			if (sscanf(p,"%d",&rule) != 1) error(327,"-misrawarn");
			last--;
			*last='.';
		} else error(327,"-misrawarn");
	}

	p--;
	if (!misracheck) misracheck = 1;
	if (misraoldrule) {
		if (misraversion==2004) error(328,misraversion,"-misrawarn",p);
		if (!misraversion) misraversion=1998;
		if ((misraoldrule < 1) || (misraoldrule>MISRA_98_RULE_NUMBER)) error(329,misraoldrule,"-misrawarn",p);
		misra_98_warn_flag[misraoldrule-1] = 1;
	} else {
		if (misraversion==1998) error(328,misraversion,"-misrawarn",p);
		if (!misraversion) misraversion=2004;
		if (subrule) {
			misr_err = misra_err_out;
		  not_found = 1;
			while ( misr_err->text ) {
				if ((misr_err->chapter == rule) && (misr_err->rule == subrule)) {
					not_found = 0;
					break;
				}
				misr_err++;
			}
			if (not_found) error(330,rule,subrule,"-misrawarn",p);
			misra_04_warn_flag[rule-1][subrule-1] = 1;
		} else {
			misr_err = misra_err_out;
		  not_found = 1;
			while ( misr_err->text ) {
				if (misr_err->chapter == rule) {
					not_found = 0;
					misra_04_warn_flag[misr_err->chapter-1][misr_err->rule-1] = 1;
				}
				misr_err++;
			}
			if (not_found) error(330,rule,subrule,"-misrawarn",p);
		}
	}

}


void misradontwarn(char *p) {
	int rule,subrule,misraoldrule;
	char* last;
	int not_found;
	struct misra_err_out* misr_err;
	rule = 0;
	subrule = 0;
	misraoldrule = 0;
  if(*p!='=') error(4,"-misradontwarn");
	p++;
	if (!(strncmp("chapter",p,6))) {
		if (sscanf((p+7),"%d",&rule) != 1) {
			error(327,"-misradontwarn");
		}
	} else if ( !(strncmp("misra98rule",p,11))) {
		if (sscanf((p+12),"%d",&misraoldrule) != 1) {
			error(327,"-misradontwarn");
		}
	} else {
		if (last = strchr(p,'.')) {
			*last = 0;
			last++;
			if (sscanf(last,"%d",&subrule) != 1) error(327,"-misradontwarn");
			if (sscanf(p,"%d",&rule) != 1) error(327,"-misradontwarn");
			last--;
			*last='.';
		} else error(327,"-misradontwarn");
	}

	p--;
	if (misraoldrule) {
		if (misraversion==2004) error(328,misraversion,"-misradontwarn",p);
		if ((misraoldrule < 1) || (misraoldrule>MISRA_98_RULE_NUMBER)) error(329,misraoldrule,"-misradontwarn",p);
		misra_98_warn_flag[misraoldrule-1] = -1;
	} else {
		if (misraversion==1998) error(328,misraversion,"-misradontwarn",p);
		if (subrule) {
			misr_err = misra_err_out;
		  not_found = 1;
			while ( misr_err->text ) {
				if ((misr_err->chapter == rule) && (misr_err->rule == subrule)) {
					not_found = 0;
					break;
				}
				misr_err++;
			}
			if (not_found) error(330,rule,subrule,"-misradontwarn",p);
			misra_04_warn_flag[rule-1][subrule-1] = -1;
		} else {
			misr_err = misra_err_out;
		  not_found = 1;
			while ( misr_err->text ) {
				if (misr_err->chapter == rule) {
					not_found = 0;
					misra_04_warn_flag[misr_err->chapter-1][misr_err->rule-1] = -1;
				}
				misr_err++;
			}
			if (not_found) error(330,rule,subrule,"-misradontwarn",p);
		}
	}

}



void warn(char *p)
/*  schaltet Warnung fuer Meldung ein           */
/*  wenn Nummer<0 sind alle Warnungen ein       */
{
  int i;
  if(*p!='=') error(4,"-warn");
  i=atoi(p+1);
  if(i>=err_num) error(159,i);
  if(i<0){
    for(i=0;i<err_num;i++) err_out[i].flags&=~DONTWARN;
    return;
  }else err_out[i].flags&=~DONTWARN;
}
void gen_function(FILE *f,struct Var *v,int real_gen)
{
  struct IC *p,*new;int i,had_regs;
  if(DEBUG&1) printf("gen_function <%s>,f=%p,real_gen=%d\n",v->identifier,(void*)f,real_gen);
  if(!v->fi) ierror(0);
  if(errors!=0) return;
  first_ic=last_ic=0;
  for(i=1;i<=MAXR;i++) {regs[i]=regused[i]=regsa[i];regsbuf[i]=0;}
  function_calls=0;vlas=0;
  if(!real_gen){
    for(p=v->fi->first_ic;p;p=p->next){
      new=new_IC();
      *new=*p;
      p->copy=new;
      add_IC(new);
      new->file=p->file;
      new->line=p->line;
      if(p->code==CALL){
	int i;
	function_calls++;
	if((p->q1.flags&VAR)&&!strcmp(p->q1.v->identifier,"__allocvla")){
          vlas=1;
          v->fi->flags|=USES_VLA;
        }
	new->arg_list=mymalloc(sizeof(*new->arg_list)*new->arg_cnt);
	for(i=0;i<new->arg_cnt;i++) new->arg_list[i]=p->arg_list[i]->copy;      
      }
    }
  }else{
    for(i=1;i<=MAXR;i++) regused[i]=0;
    for(p=v->fi->opt_ic;p;p=p->next){
      if(p->code==ALLOCREG){
	regused[p->q1.reg]=1;
	if(reg_pair(p->q1.reg,&rp)){
	  regused[rp.r1]=1;
	  regused[rp.r2]=1;
	}
      }
      if(p->code==CALL){
	if((p->q1.flags&VAR)&&!strcmp(p->q1.v->identifier,"__allocvla")) vlas=1;
	function_calls++;
      }
    }
  }
  if(vlas&&FPVLA_REG) regs[FPVLA_REG]=regused[FPVLA_REG]=regsa[FPVLA_REG]=regscratch[FPVLA_REG]=1;
      
  if(!real_gen&&(c_flags[2]&USEDFLAG)&&ic1){
    fprintf(ic1,"function %s\n",v->identifier);
    pric(ic1,first_ic);
  }
  vl0=first_ext;
  vl1=v->fi->statics;
  vl2=0;
  vl3=v->fi->vars;
  nesting=1;
  first_var[nesting]=last_var[nesting]=0;
  cur_func=v->identifier;
  if(!real_gen){
    optimize(optflags,v);
    memset(regs_modified,0,RSIZE);
    /* pseudeo generator pass to get regs_modified */
    v->fi->opt_ic=clone_ic(first_ic);
    v->fi->max_offset=max_offset;
    if(v->fi&&(v->fi->flags&ALL_REGS))
      had_regs=1;
    else
      had_regs=0;
    gen_code(0,first_ic,v,max_offset);
#ifdef HAVE_REGS_MODIFIED
    if(!v->fi) v->fi=new_fi();
    {
      int i;struct IC *p;
      for(i=1;i<=MAXR;i++){
	if(BTST(regs_modified,i)&&reg_pair(i,&rp)){
	  BSET(regs_modified,rp.r1);
	  BSET(regs_modified,rp.r2);
	}
      }
#if 1
      for(i=1;i<=MAXR;i++){
	if(reg_pair(i,&rp)){
	  if(BTST(regs_modified,rp.r1)||BTST(regs_modified,rp.r2))
	    BSET(regs_modified,i);
	}
      }
#endif
      if(had_regs){
	if(memcmp(regs_modified,v->fi->regs_modified,RSIZE))
	  error(321,v->identifier);
      }else
	memcpy(v->fi->regs_modified,regs_modified,RSIZE);
#if 0
      printf("regs for %s (ALL_REGS=%d):\n",v->identifier,v->fi->flags&ALL_REGS);
      for(i=1;i<MAXR;i++) if(BTST(regs_modified,i)) printf("%s ",regnames[i]);
      printf("\n");
#endif
    }
#endif
    v->flags|=GENERATED;
    free_IC(first_ic);
    first_ic=last_ic=0;
    /*free_var(first_var[nesting]);*/
    nesting=0;
  }else{
    if((c_flags[3]&USEDFLAG)&&ic2){
      fprintf(ic2,"function %s\n",v->identifier);
      pric(ic2,v->fi->opt_ic);
    }
    gen_code(f,v->fi->opt_ic,v,v->fi->max_offset);
    static_stack_check(v);
  }
}
/* handle functions in a const list before caller */
static void do_clist_calls(struct const_list *cl)
{
  while(cl){
    if(cl->tree&&(cl->tree->o.flags&VARADR)){
      struct Var *v=cl->tree->o.v;
      if(ISFUNC(v->vtyp->flags)){
	printf(":: %s\n",v->identifier);
	do_function(v);
      }
    }
    if(cl->other)
      do_clist_calls(cl->other);
    cl=cl->next;
  }
}
void do_function(struct Var *v)
{
  int i;struct IC *p;
  if((v->flags&(GENERATED|DEFINED))!=DEFINED) return;
  v->flags|=GENERATED;
  if(!v->fi) v->fi=new_fi();
#if 0  
  for(i=0;i<v->fi->call_cnt;i++){
    if(v->fi->call_list[i].v->flags&DEFINED)
      do_function(v->fi->call_list[i].v);
  }
#endif
  /* handle callees before caller */
  for(p=v->fi->first_ic;p;p=p->next){
    /* direct call */
    if(p->code==CALL&&(p->q1.flags&(VAR|DREFOBJ))==VAR)
      do_function(p->q1.v);
    /* function address is also a candidate */
    if((p->q1.flags&(VAR|VARADR))&&ISFUNC(p->q1.v->vtyp->flags))
      do_function(p->q1.v);
    if((p->q2.flags&(VAR|VARADR))&&ISFUNC(p->q2.v->vtyp->flags))
      do_function(p->q2.v);
    if((p->z.flags&(VAR|VARADR))&&ISFUNC(p->z.v->vtyp->flags))
      do_function(p->z.v);
    /* indirect call, handle special case */
    if(p->code==CALL&&(p->q1.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ)){
      struct Var *v=p->q1.v;
      if(v->storage_class==AUTO||v->storage_class==REGISTER){
	struct IC *m=p->prev;struct Var *tmp=0;
	while(m&&(m->code<LABEL||m->code>=BRA)){
	  if(!tmp&&(m->z.flags&(VAR|DREFOBJ))==VAR&&m->z.v==v&&
	     (m->q1.flags&(VAR|DREFOBJ))==(VAR|DREFOBJ))
	    tmp=m->q1.v;
	  if(tmp&&(m->z.flags&(VAR|DREFOBJ))==VAR&&m->z.v==tmp&&
	     (m->q1.flags&(VAR|VARADR))==(VAR|VARADR)&&
	     m->q1.v->clist&&is_const(m->q1.v->vtyp)){
	    do_clist_calls(m->q1.v->clist);
	  }
	  m=m->prev;
	}
      }
    }
  }
  gen_function(0,v,0);
}



extern char *copyright;
int main(int argc,char *argv[])
{
  int i,j,*fname=malloc(argc*sizeof(int)),files=0;
  unsigned long ucpp_flags=LEXER|WARN_TRIGRAPHS|WARN_STANDARD|WARN_ANNOYING/*|CCHARSET*/|HANDLE_PRAGMA|COPY_LINE|WARN_TRIGRAPHS_MORE|HANDLE_TRIGRAPHS;
  if(!fname) ierror(0);
  memset(fname,0,argc*sizeof(int));
  c_flags_val[9].f=dontwarn;
  c_flags_val[10].f=warn;
  c_flags_val[42].f=misrawarn;
  c_flags_val[43].f=misradontwarn;
  c_flags_val[44].f=reserve_reg;
  for(i=1;i<argc;i++){
    if(*argv[i]!='-'){  /*  kein Flag   */
      fname[i]=1;
      files++;
      if(!inname) inname=argv[i];
    }else{
      int flag=0;
      if(argv[i][1]=='D'||argv[i][1]=='I') flag=1;
      for(j=0;j<MAXCF&&flag==0;j++){
	size_t l;
	if(!c_flags_name[j]) continue;
	l=strlen(c_flags_name[j]);
	if(l>0&&!strncmp(argv[i]+1,c_flags_name[j],l)&&(argv[i][1+l]==0||argv[i][1+l]=='=')){
	  flag=1;
	  if((c_flags[j]&(USEDFLAG|FUNCFLAG))==USEDFLAG){error(2,argv[i]);break;}
	  c_flags[j]|=USEDFLAG;
	  if(c_flags[j]&STRINGFLAG){
	    if(argv[i][l+1]!='='){error(3,argv[i]);}
	    if(argv[i][l+2]||i>=argc-1)
	      c_flags_val[j].p=&argv[i][l+2];
	    else
	      c_flags_val[j].p=&argv[++i][0];
	  }
	  if(c_flags[j]&VALFLAG){
	    if(argv[i][l+1]!='='){error(4,argv[i]);}
	    if(argv[i][l+2]||i>=argc-1)
	      c_flags_val[j].l=atol(&argv[i][l+2]);
	    else
	      c_flags_val[j].l=atol(&argv[++i][0]);
	  }
	  if(c_flags[j]&FUNCFLAG) c_flags_val[j].f(&argv[i][l+1]);
	}
      }
      for(j=0;j<MAXGF&&flag==0;j++){
	size_t l;
	if(!g_flags_name[j]) continue;
	l=strlen(g_flags_name[j]);
	if(l>0&&!strncmp(argv[i]+1,g_flags_name[j],l)){
	  flag=1;
	  if((g_flags[j]&(USEDFLAG|FUNCFLAG))==USEDFLAG){error(2,argv[i]);break;}
	  g_flags[j]|=USEDFLAG;
	  if(g_flags[j]&STRINGFLAG){
	    if(argv[i][l+1]!='='){error(3,argv[i]);}
	    if(argv[i][l+2]||i>=argc-1)
	      g_flags_val[j].p=&argv[i][l+2];
	    else
	      g_flags_val[j].p=&argv[++i][0];
	  }
	  if(g_flags[j]&VALFLAG){
	    if(argv[i][l+1]!='='){error(4,argv[i]);}
	    if(argv[i][l+2]||i>=argc-1)
	      g_flags_val[j].l=atol(&argv[i][l+2]);
	    else
	      g_flags_val[j].l=atol(&argv[++i][0]);
	  }
	  if(g_flags[j]&FUNCFLAG) g_flags_val[j].f(&argv[i][l+1]);
	}
      }
      if(!flag){error(5,argv[i]);}
    }
  }
  if(!(c_flags[6]&USEDFLAG)){
#ifdef SPECIAL_COPYRIGHT
    printf("%s\n",SPECIAL_COPYRIGHT);
#else
    printf("%s\n",copyright);
    printf("%s\n",cg_copyright);
#endif
  }
  if(c_flags[4]&USEDFLAG) DEBUG=c_flags_val[4].l; else DEBUG=0;
  if(c_flags[13]&USEDFLAG) ucpp_flags|=CPLUSPLUS_COMMENTS;
  if(c_flags[14]&USEDFLAG) ucpp_flags|=CPLUSPLUS_COMMENTS;
  if(c_flags[15]&USEDFLAG) ucpp_flags&=~HANDLE_TRIGRAPHS;
  if(c_flags[16]&USEDFLAG) no_inline_peephole=1;
  if(c_flags[17]&USEDFLAG) final=1;
  if(!(c_flags[8]&USEDFLAG)) c_flags_val[8].l=10; /* max. Fehlerzahl */
  if(c_flags[22]&USEDFLAG) c_flags[7]|=USEDFLAG;   /*  iso=ansi */
  if(c_flags[7]&USEDFLAG) error(209);
  if(c_flags[0]&USEDFLAG) optflags=c_flags_val[0].l;
  if(optflags&16384) cross_module=1;
  if(c_flags[11]&USEDFLAG) maxoptpasses=c_flags_val[11].l;
  if(c_flags[12]&USEDFLAG) inline_size=c_flags_val[12].l;
  if(c_flags[21]&USEDFLAG) fp_assoc=1;
  if(c_flags[25]&USEDFLAG) unroll_size=c_flags_val[25].l;
  if(c_flags[23]&USEDFLAG) noaliasopt=1;
  if(c_flags[27]&USEDFLAG) optspeed=1;
  if(c_flags[28]&USEDFLAG) optsize=1;
  if(c_flags[29]&USEDFLAG) unroll_all=1;
  if(c_flags[30]&USEDFLAG) stack_check=1;  
  if(c_flags[31]&USEDFLAG) inline_depth=c_flags_val[31].l;
  if(c_flags[32]&USEDFLAG) debug_info=1;
  if(c_flags[33]&USEDFLAG) c99=1;
  if(c_flags[34]&USEDFLAG) {wpo=1;no_emit=1;}
  if(c_flags[36]&USEDFLAG) {noitra=1;}
  if(c_flags[37]&USEDFLAG) {
		misracheck=1;
		if ((misraversion==1998) && (c_flags_val[37].l == 2004)) error(328,c_flags_val[37].l,"-misra","");
		if ((misraversion==2004) && (c_flags_val[37].l == 1998)) error(328,c_flags_val[37].l,"-misra","");
		misraversion=c_flags_val[37].l;

		if (!((misraversion==2004) || (misraversion==1998))) error(328,misraversion,"-misra","");
		if (misraversion==1998) {
			int misra_set_iterator;
			for (misra_set_iterator = 0; misra_set_iterator < MISRA_98_RULE_NUMBER; misra_set_iterator++ ) {
				if (misra_98_warn_flag[misra_set_iterator] != -1) misra_98_warn_flag[misra_set_iterator] = 1;
			}
		} else {
			int m1, m2;
			for (m1 = 0; m1 < MISRA_04_CHAPTER; m1++) {
				for (m2 = 0; m2 < MISRA_04_MAX_RULE_IN_CHAPTER; m2++) {
					if (misra_04_warn_flag[m1][m2] != -1) misra_04_warn_flag[m1][m2] = 1;
				}
			}
		}
	}
  if(c_flags[38]&USEDFLAG) {coloring=c_flags_val[38].l;}
  if(c_flags[39]&USEDFLAG) {dmalloc=1;}
  if(c_flags[40]&USEDFLAG) {disable=c_flags_val[40].l;}
  if(c_flags[41]&USEDFLAG) {softfloat=1;}
  if(c_flags[45]&USEDFLAG) {ecpp=1;}
  if(c_flags[46]&USEDFLAG) {short_push=1;}
  if(c_flags[47]&USEDFLAG) {default_unsigned=1;}

  if(wpo){
    cross_module=1;
    optflags=-1;
  }
  if(optsize){
    if(!(c_flags[25]&USEDFLAG)) unroll_size=0;
  }
  if(ecpp&&c99){
	  error(333, "c99", "ecpp");
  }
  if(c99){
    ucpp_flags|=CPLUSPLUS_COMMENTS|MACRO_VAARG;
    err_out[67].flags|=ANSIV;
    err_out[67].flags&=~DONTWARN;
    err_out[161].flags|=ANSIV;
    err_out[161].flags&=~DONTWARN;
    err_out[155].flags|=ANSIV;
    err_out[155].flags&=~DONTWARN;
    err_out[156].flags|=ANSIV;
    err_out[156].flags&=~DONTWARN;
  }
  if(ecpp){
#ifndef HAVE_ECPP
	error(334, "EC++");
#endif
    ucpp_flags|=CPLUSPLUS_COMMENTS|MACRO_VAARG;
  }
  if(!cross_module&&files>1) error(1);
  if(files<=0&&!(c_flags[35]&USEDFLAG)) error(6);
  stackalign=l2zm(0L);
  if(!init_cg()) exit(EXIT_FAILURE);
  if(zmeqto(stackalign,l2zm(0L)))
    stackalign=maxalign;
  for(i=0;i<EMIT_BUF_DEPTH;i++)
    emit_buffer[i]=mymalloc(EMIT_BUF_LEN);
  emit_p=emit_buffer[0];
  /*FIXME: multiple-ccs don't work */
  if(c_flags[24]&USEDFLAG) multiple_ccs=0;
  if(!(c_flags[5]&USEDFLAG)){
    if(c_flags[1]&USEDFLAG){
      out=open_out(c_flags_val[1].p,0);
    }else{
      if(wpo)
	out=open_out(inname,"o");
      else
	out=open_out(inname,"asm");
    }
    if(!out){
      exit(EXIT_FAILURE);
    }
  }
  if(wpo){
    wpo_key=MAGIC_WPO;
    fprintf(out,"%cVBCC",0);
  }
  if(debug_info) init_db(out);
  if(c_flags[2]&USEDFLAG) ic1=open_out(inname,"ic1");
  if(c_flags[3]&USEDFLAG) ic2=open_out(inname,"ic2");
  c99_compliant=0;
  init_cpp();
  if(c_flags[35]&USEDFLAG){
    /* we have a command file */
    cmdfile=fopen(c_flags_val[35].p,"r");
    if(!cmdfile) error(7,c_flags_val[35].p);
  }
  for(i=1;cmdfile||i<argc;i++){
    FILE *in;
    int first_byte;
    if(i<argc){
      if(!fname[i]) continue;
      inname=argv[i];
    }else{
      static char nbuf[1024];
      if(!fgets(nbuf,1023,cmdfile)) break;
      inname=nbuf;
      while(isspace((unsigned char)*inname)) inname++;
      if(*inname=='\"') inname++;
      if(inname[strlen(inname)-1]=='\n') inname[strlen(inname)-1]=0;
      if(inname[strlen(inname)-1]=='\"') inname[strlen(inname)-1]=0;
      if(!*inname) break;
    }
    if(DEBUG&1) printf("starting translation-unit <%s>\n",inname);
    in=fopen(inname,"r");
    if(!in) {error(7,inname);}
    misratok=0;
    first_byte=fgetc(in);
    if(first_byte==0){
      input_wpo=in;
      if(fgetc(in)!='V') error(300);
      if(fgetc(in)!='B') error(300);
      if(fgetc(in)!='C') error(300);
      if(fgetc(in)!='C') error(300);
      wpo_key=MAGIC_WPO;
    }else{
      ungetc(first_byte,in);
      input_wpo=0;
    }
    if(c_flags[18]&USEDFLAG) ppout=open_out(inname,"i");
    if(!input_wpo){
      int mcmerk=misracheck;
      misracheck=0;
      init_tables(0);
      init_include_path(0);
      set_init_filename(inname,1);
      init_lexer_state(&ls);
      init_lexer_mode(&ls);
      ls.flags=ucpp_flags;
      ls.input=in;
      for(j=1;j<argc;j++){
	if(argv[j][0]=='-'&&argv[j][1]=='I')
	  add_incpath(&argv[j][2]);
	if(argv[j][0]=='-'&&argv[j][1]=='D')
	  define_macro(&ls,&argv[j][2]);      
      }
      if(target_macros){
	char **m=target_macros;
	while(*m)
	  define_macro(&ls,*m++);
      }
      define_macro(&ls,"__VBCC__");
      define_macro(&ls,"__entry=__vattr(\"entry\")");
      define_macro(&ls,"__str(x)=#x");
      define_macro(&ls,"__asm(x)=do{static void inline_assembly()=x;inline_assembly();}while(0)");
      define_macro(&ls,"__regsused(x)=__vattr(\"regused(\"x\")\")");
      define_macro(&ls,"__varsused(x)=__vattr(\"varused(\"x\")\")");
      define_macro(&ls,"__varsmodified(x)=__vattr(\"varchanged(\"x\")\")");
      define_macro(&ls,"__noreturn=__vattr(\"noreturn()\")");
      define_macro(&ls,"__alwaysreturn=__vattr(\"alwaysreturn()\")");
      define_macro(&ls,"__nosidefx=__vattr(\"nosidefx()\")");
      define_macro(&ls,"__stack(x)=__vattr(__str(stack1(x)))");
      define_macro(&ls,"__stack2(x)=__vattr(__str(stack2(x)))");
      if(c99)
	define_macro(&ls,"__STDC_VERSION__=199901L");
      misracheck=mcmerk;
      enter_file(&ls,ls.flags);
    }
    filename=current_filename;
    switch_count=0;break_label=0;
    line=0;eof=0;
    next_token();
    killsp();
    nesting=-1;enter_block();
    translation_unit();
    fclose(in); /*FIXME: do I have to close??*/   
    if((c_flags[18]&USEDFLAG)&&ppout) fclose(ppout);
    if(!input_wpo)
      free_lexer_state(&ls);
  }
  if(wpo)
    raus();
  if(!cross_module){
    ierror(0);
  }else{
    struct tunit *t;
    struct Var *v,*sf;
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
#endif
    if(DEBUG&1) printf("first optimizing\n");
    for(v=first_ext;v;v=v->next){
      if(ISFUNC(v->vtyp->flags)&&(v->flags&DEFINED)&&(!v->vattr||!strstr(v->vattr,"taskprio("))){
	do_function(v);
      }      
    }
    for(t=first_tunit;t;t=t->next){
      for(v=t->statics;v;v=v->next){
	if(ISFUNC(v->vtyp->flags)&&(v->flags&DEFINED)){
	  do_function(v);
	}
      }
    }
    if(DEBUG&1) printf("determining used objects\n");
    for(v=first_ext;v;v=v->next){
      if((v->flags&(DEFINED|TENTATIVE))&&(v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC){
	if(!final||!strcmp(v->identifier,"main")||(v->vattr&&strstr(v->vattr,"entry"))){
	  used_objects(v);
	  if(ISFUNC(v->vtyp->flags)) do_function(v);
	}
      }
    }
    if(!(c_flags[5]&USEDFLAG)){
      if(DEBUG&1) printf("generating external functions:\n");
      for(v=first_ext;v;v=v->next){
	if(ISFUNC(v->vtyp->flags)&&(v->flags&(REFERENCED|DEFINED))==(REFERENCED|DEFINED)){
	  gen_function(out,v,1);
	}
      }
      if(DEBUG&1024) printf("generating static functions\n");
      i=0;
      for(t=first_tunit;t;t=t->next){
	if(DEBUG&1) printf("translation-unit %d:\n",++i);
	if(DEBUG&1) printf("generating statics:\n");
	for(v=t->statics;v;v=v->next){
	  if(DEBUG&1) printf(" %s\n",v->identifier);
	  if(ISFUNC(v->vtyp->flags)&&(v->flags&(REFERENCED|DEFINED))==(REFERENCED|DEFINED)){
	    gen_function(out,v,1);
	  }
	}
      }
      
      if(DEBUG&1) printf("generating vars:\n");
      gen_vars(first_ext);
      for(t=first_tunit;t;t=t->next)
	gen_vars(t->statics);
      for(v=first_ext;v;v=v->next){
	if(ISFUNC(v->vtyp->flags)&&(v->flags&DEFINED))
	  gen_vars(v->fi->vars);
      }
      for(t=first_tunit;t;t=t->next){
	for(v=t->statics;v;v=v->next){
	  if(ISFUNC(v->vtyp->flags)&&(v->flags&DEFINED))
	    gen_vars(v->fi->vars);
	}
      }
    }
  }
  raus();
}
int mcmp(const char *s1,const char *s2)
/*  Einfachere strcmp-Variante.     */
{
  char c;
  do{
    c=*s1++;
    if(c!=*s2++) return(1);
  }while(c);
  return 0;
}
int is_keyword(char *p)
{
  char *n=p+1;
  switch(*p){
  case 'a':
    if(!mcmp(n,"uto")) return 1;
    return 0;
  case 'b': 
    if(ecpp&&!mcmp(n,"ool")) return 1;
    if(!mcmp(n,"reak")) return 1;
    return 0;
  case 'c': 
    if(!mcmp(n,"ase")) return 1;
    if(ecpp&&!mcmp(n,"atch")) return 1;
    if(!mcmp(n,"har")) return 1;
    if(ecpp&&!mcmp(n,"lass")) return 1;
    if(!mcmp(n,"onst")) return 1;
    if(ecpp&&!mcmp(n,"ons_cast")) return 1;
    if(!mcmp(n,"ontinue")) return 1;
    return 0;
  case 'd': 
    if(!mcmp(n,"efault")) return 1;
    if(ecpp&&!mcmp(n,"elete")) return 1;
    if(!mcmp(n,"o")) return 1;
    if(!mcmp(n,"ouble")) return 1;
    if(ecpp&&!mcmp(n,"ynamic_cast")) return 1;
    return 0;
  case 'e': 
    if(!mcmp(n,"lse")) return 1;
    if(!mcmp(n,"num")) return 1;
    if(ecpp&&!mcmp(n,"xplicit")) return 1;
    if(ecpp&&!mcmp(n,"xport")) return 1;
    if(!mcmp(n,"xtern")) return 1;
    return 0;
  case 'f': 
    if(ecpp&&!mcmp(n,"alse")) return 1;
    if(!mcmp(n,"loat")) return 1;
    if(!mcmp(n,"or")) return 1;
    if(ecpp&&!mcmp(n,"riend")) return 1;
    return 0;
  case 'g': 
    if(!mcmp(n,"oto")) return 1;
    return 0;
  case 'i':
    if(!mcmp(n,"f")) return 1;
    if(c99&&!mcmp(n,"nline")) return 1;
    if(!mcmp(n,"nt")) return 1;
    return 0;
  case 'l':
    if(!mcmp(n,"ong")) return 1;
    return 0;
  case 'm': 
    if(ecpp&&!mcmp(n,"utable")) return 1;
    return 0;
  case 'n': 
    if(ecpp&&!mcmp(n,"amespace")) return 1;
    if(ecpp&&!mcmp(n,"ew")) return 1;
    return 0;
  case 'o': 
    if(ecpp&&!mcmp(n,"perator")) return 1;
    return 0;
  case 'p': 
    if(ecpp&&!mcmp(n,"rivate")) return 1;
    if(ecpp&&!mcmp(n,"rotected")) return 1;
    if(ecpp&&!mcmp(n,"ublic")) return 1;
    return 0;
  case 'r': 
    if(!mcmp(n,"egister")) return 1;
    if(ecpp&&!mcmp(n,"einterpret_cast")) return 1;
    if(c99&&!mcmp(n,"estrict")) return 1;
    if(!mcmp(n,"eturn")) return 1;
    return 0;
  case 's':
    if(!mcmp(n,"hort")) return 1;
    if(!mcmp(n,"igned")) return 1;
    if(!mcmp(n,"izeof")) return 1;
    if(!mcmp(n,"tatic")) return 1;
    if(ecpp&&!mcmp(n,"tatic_cast")) return 1;
    if(!mcmp(n,"truct")) return 1;
    if(!mcmp(n,"witch")) return 1;
    return 0;
  case 't':
    if(ecpp&&!mcmp(n,"emplate")) return 1;
   /* if(ecpp&&!mcmp(n,"his")) return 1;*/
    if(ecpp&&!mcmp(n,"hrow")) return 1;
    if(ecpp&&!mcmp(n,"rue")) return 1;
    if(ecpp&&!mcmp(n,"ry")) return 1;
    if(!mcmp(n,"ypedef")) return 1;
    if(ecpp&&!mcmp(n,"ypeid")) return 1;
    if(ecpp&&!mcmp(n,"ypename")) return 1;
    return 0;
  case 'u': 
    if(!mcmp(n,"nion")) return 1;
    if(!mcmp(n,"nsigned")) return 1;
    if(ecpp&&!mcmp(n,"sing")) return 1;
    return 0;
  case 'v': 
    if(ecpp&&!mcmp(n,"irtual")) return 1;
    if(!mcmp(n,"oid")) return 1;
    if(!mcmp(n,"olatile")) return 1;
    return 0;
  case 'w':
    if(ecpp&&!mcmp(n,"char_t")) return 1;
    if(!mcmp(n,"hile")) return 1;
    return 0;
  case '_': 
    if(c99&&!mcmp(n,"Bool")) return 1;
    if(c99&&!mcmp(n,"Complex")) return 1;
    if(c99&&!mcmp(n,"Imaginary")) return 1;
  default:
    return 0;
  }
}
void cpbez(char *m,int check_keyword)
/*  Kopiert den naechsten Bezeichner von s nach m. Wenn check_keyord!=0 */
/*  wird eine Fehlermeldung ausgegeben, falls das Ergebnis ein          */
/*  reserviertes Keyword von C ist.                                     */
{
  if(ctok->type!=NAME){
    *m=0;
    return;
  }
  if(strlen(ctok->name)>=MAXI){
    error(206,MAXI-1);
    strncpy(m,ctok->name,MAXI-1);
    m[MAXI-1]=0;
  }else{
    strcpy(m,ctok->name);
  }
  if(check_keyword&&is_keyword(m))
    error(216,m);
}
void cpnum(char *m)
/* kopiert die naechste int-Zahl von s nach m   */
/* muss noch erheblich erweiter werden          */
{
  if(ctok->type!=NUMBER){
    *m=0;
    return;
  }
  strcpy(m,ctok->name);
}
void copy_token(struct token *d,struct token *s)
{
  size_t l;
  *d=*s;
  if(S_TOKEN(s->type)){
    l=strlen(s->name)+1;
    d->name=mymalloc(l);
    memcpy(d->name,s->name,l);
  }else
    d->name=0;
}
static struct token back_token;
static int have_back_token;
void push_token(struct token *t)
{
  static char back_name[MAXI+1];
  if(have_back_token) ierror(0);
  back_token=*t;
  if(S_TOKEN(t->type)){
    strcpy(back_name,t->name);
    back_token.name=back_name;
  }else
    back_token.name=0;
  have_back_token=1;
  ctok=&back_token;
}
void next_token(void)
{
  if(eof){
    if(!endok)
      raus();
    else
      return;
  }
  if(input_wpo){
    int c;
    static struct token wpo_tok;
    static size_t sz;
    char *p;size_t cs;
  
    if(have_back_token){
      have_back_token=0;
      ctok=&wpo_tok;
      return;
    }
    ctok=&wpo_tok;
    c=fgetc(input_wpo);
    if(c==EOF){
      eof=1;
      return;
    }else
      c^=wpo_key++;
    wpo_tok.type=(unsigned char)c;
    /*printf("wpoget: %d (%s)\n",ctok->type,operators_name[ctok->type]);*/
    if(S_TOKEN(wpo_tok.type)){
      p=wpo_tok.name;
      cs=0;
      do{
	if(cs>=sz){
	  sz+=1000;
	  wpo_tok.name=myrealloc(wpo_tok.name,sz);
	  p=wpo_tok.name+cs;
	}
	c=fgetc(input_wpo);
	if(c!=EOF){
	  c^=wpo_key++;
	  *p++=c;
	  cs++;
	}else
	  eof=1;
      }while(((unsigned char)c)!=0&&c!=EOF);
      *p=0;
      /*printf("name=%s\n",ctok->name);*/
    }
    return;
  }
  if(have_back_token){
    have_back_token=0;
    if(S_TOKEN(ctok->type)&&!ctok->name)
      ierror(0);
  }else{
    static int last_line=1,last_token=NONE;
    static char *last_fname;static size_t last_size;
    eof=lex(&ls);
    if(ctok&&S_TOKEN(ls.ctok->type)&&!ls.ctok->name)
      ierror(0);
    if(wpo){
      fprintf(out,"%c",ls.ctok->type^wpo_key++);
      if(S_TOKEN(ls.ctok->type)){
	char *p=ls.ctok->name;
	while(*p){
	  fprintf(out,"%c",*p^wpo_key++);
	  p++;
	}
	fprintf(out,"%c",0^wpo_key++);
      }
    }
    if((c_flags[18]&USEDFLAG)&&ppout&&!input_wpo){
      if(!last_fname){
	last_fname=mymalloc(1);
	*last_fname=0;
      }
      if(strcmp(last_fname,current_filename)){
	fprintf(ppout,"\n#line %d \"%s\"\n",(int)ls.ctok->line,current_filename);
	last_line=ls.ctok->line;
	if(strlen(current_filename)>=last_size)
	  last_fname=myrealloc(last_fname,strlen(current_filename)+1);
	strcpy(last_fname,current_filename);
      }else{
	for(;last_line<ls.ctok->line;last_line++)
	  fprintf(ppout,"\n");
      }
      if(S_TOKEN(ls.ctok->type)){
	if(ls.ctok->type==PRAGMA)
	  fprintf(ppout,"#pragma");
	fprintf(ppout," %s",ls.ctok->name);
      }else
	fprintf(ppout," %s",operators_name[ls.ctok->type]);
      last_token=ls.ctok->type;
    }
  }
  ctok=ls.ctok;
  line=ctok->line;
  if(misracheck&&ctok->type!=PRAGMA&&ctok->type!=NONE&&ctok->type!=NEWLINE&&ctok->type!=COMMENT)
    misratok=1;
  /*FIXME: do not store multiple */
  if(filename!=current_filename){
    filename=mymalloc(strlen(current_filename)+1);
    strcpy(filename,current_filename);
  }
  /*filename=current_filename;*/
  if(DEBUG&2) printf("current token (type %d): %s\n",(int)ctok->type,ucpp_token_name(ctok));
}
char *pragma_cpbez(char *buff,char *s);
/* calculate fi entries (regs_modifed,uses,changes,flags etc.) from
   attributes */
void fi_from_attr(struct Var *v)
{
  char *attr;
  attr=v->vattr;
  if(!attr) return;
  while(attr=strstr(attr,"readmem(")){
    int f;
    attr+=8;
    if(sscanf(attr,"%d",&f)!=1) ierror(0);
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_USES;
    v->fi->use_cnt++;
    v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(struct varlist));
    v->fi->use_list[v->fi->use_cnt-1].v=0;
    v->fi->use_list[v->fi->use_cnt-1].flags=f;
  }
  attr=v->vattr;
  while(attr=strstr(attr,"writemem(")){
    int f;
    attr+=9;
    if(sscanf(attr,"%d",&f)!=1) ierror(0);
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_MODS;
    v->fi->change_cnt++;
    v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(struct varlist));
    v->fi->change_list[v->fi->change_cnt-1].v=0;
    v->fi->change_list[v->fi->change_cnt-1].flags=f;
  }
  attr=v->vattr;
  while(attr=strstr(attr,"varused(")){
    struct Var *n;
    attr+=8;
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_USES;
    while(1){
      while(isspace((unsigned char)*attr)) attr++;
      if(*attr==')') break;
      attr=pragma_cpbez(buff,attr);
      if(buff[0]==0){
	error(76);
	break;
      }
      n=find_ext_var(buff);
      if(!n){
	error(82,buff);
	break;
      }
      v->fi->use_cnt++;
      v->fi->use_list=myrealloc(v->fi->use_list,v->fi->use_cnt*sizeof(struct varlist));
      v->fi->use_list[v->fi->use_cnt-1].v=n;
      v->fi->use_list[v->fi->use_cnt-1].flags=n->vtyp->flags;
      while(isspace((unsigned char)*attr)) attr++;
      if(*attr==','||*attr=='/') attr++;
    }
  }
  attr=v->vattr;
  while(attr=strstr(attr,"varchanged(")){
    struct Var *n;
    attr+=11;
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_MODS;
    while(1){
      while(isspace((unsigned char)*attr)) attr++;
      if(*attr==')') break;
      attr=pragma_cpbez(buff,attr);
      if(buff[0]==0){
	error(76);
	break;
      }
      n=find_ext_var(buff);
      if(!n){
	error(82,buff);
	break;
      }
      v->fi->change_cnt++;
      v->fi->change_list=myrealloc(v->fi->change_list,v->fi->change_cnt*sizeof(struct varlist));
      v->fi->change_list[v->fi->change_cnt-1].v=n;
      v->fi->change_list[v->fi->change_cnt-1].flags=n->vtyp->flags;
      while(isspace((unsigned char)*attr)) attr++;
      if(*attr==','||*attr=='/') attr++;
    }
  }
  attr=v->vattr;
  while(attr=strstr(attr,"regused(")){
    int r,i;
    static char rname[MAXI];
    attr+=8;
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_REGS;
    while(1){
      while(isspace((unsigned char)*attr)) attr++;
      for(i=0;i<MAXI-1&&*attr&&!isspace((unsigned char)*attr)&&*attr!=','&&*attr!=')'&&*attr!='/';i++)
	rname[i]=*attr++;
      rname[i]=0;
      for(r=1;r<=MAXR;r++){
	if(!reg_pair(r,&rp)&&!strcmp(rname,regnames[r])){
	  BSET(v->fi->regs_modified,r);
	  while(isspace((unsigned char)*attr)) attr++;
	  if(*attr==','||*attr=='/') attr++;
	  break;
	}
      }
      if(r>MAXR){
	if(rname[0]) error(220,rname);
	break;
      }
    }
  }
  attr=v->vattr;
  while(attr=strstr(attr,"stack1(")){
    unsigned long sz;
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_STACK;
    attr+=7;
    if(sscanf(attr,"%lu",&sz)!=1) ierror(0); /*FIXME*/
    v->fi->stack1=ul2zum(sz);
  }
  attr=v->vattr;
  while(attr=strstr(attr,"stack2(")){
    unsigned long sz;
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_STACK;
    attr+=7;
    if(sscanf(attr,"%lu",&sz)!=1) ierror(0); /*FIXME*/
    v->fi->stack2=ul2zum(sz);
  }  
  if(strstr(v->vattr,"noreturn()")){
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=NEVER_RETURNS;
  }
  if(strstr(v->vattr,"alwaysreturn()")){
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALWAYS_RETURNS;
  }  
  if(strstr(v->vattr,"nosidefx()")){
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=NOSIDEFX;
  }  
}
#define pragma_killsp() while(isspace((unsigned char)*s)) s++;
char *pragma_cpbez(char *buff,char *s)
{
  int cnt=0;
  if(isalpha((unsigned char)*s)){
    *buff++=*s++;
    cnt++;
    while(cnt<MAXI-1&&isalnum((unsigned char)*s)){
      *buff++=*s++;
      cnt++;
    }
  }
  *buff++=0;
  return s;
}
#define WARNSTACKSIZE 128
static int widx,warn_num[WARNSTACKSIZE],warn_flags[WARNSTACKSIZE];
#define PACKSTACKSIZE 128
static int pidx,pack[PACKSTACKSIZE];
void do_pragma(char *s)
{
  error(163);
  pragma_killsp();
  if(!strncmp("opt",s,3)){
    s+=3;pragma_killsp();
    c_flags_val[0].l=optflags=atol(s);
    if(DEBUG&1) printf("#pragma opt %ld\n",c_flags_val[0].l);
  }else if(!strncmp("dontwarn",s,8)){
    int i;
    s+=8;pragma_killsp();
    if(widx>=WARNSTACKSIZE) ierror(0);
    if(sscanf(s,"%d",&i)==1){
      if(i>=err_num) error(159,i);
      warn_num[widx]=i;
      warn_flags[widx]=err_out[i].flags;
      widx++;
      err_out[i].flags|=DONTWARN;
    }
  }else if(!strncmp("warn",s,4)){
    int i;
    s+=4;pragma_killsp();
    if(widx>=WARNSTACKSIZE) ierror(0);
    if(sscanf(s,"%d",&i)==1){
      if(i>=err_num) error(159,i);
      warn_num[widx]=i;
      warn_flags[widx]=err_out[i].flags;
      widx++;
      err_out[i].flags&=~DONTWARN;
    }
  }else if(!strncmp("popwarn",s,7)){
    if(widx<=0) error(303);
    widx--;
    err_out[warn_num[widx]].flags=warn_flags[widx];
  }else if(!strncmp("begin_header",s,12)){
    header_cnt++;
  }else if(!strncmp("end_header",s,10)){
    header_cnt--;
  }else if(!strncmp("pfi",s,3)){
    struct Var *v;
    s+=3;pragma_killsp();
    pragma_cpbez(buff,s);
    if(DEBUG&1) printf("print function_info %s\n",buff);
    v=find_var(buff,0);
    if(v&&v->fi) print_fi(stdout,v->fi);
  }else if(!strncmp("finfo",s,5)){
    struct Var *v;
    s+=5;pragma_killsp();
    pragma_cpbez(buff,s);
    if(DEBUG&1) printf("new function_info %s\n",buff);
    v=find_var(buff,0);
    if(v){
      if(!v->fi) v->fi=new_fi();
      current_fi=v->fi;
    }
  }else if(!strncmp("fi_flags",s,8)){
    unsigned long flags;
    s+=8;pragma_killsp();
    sscanf(s,"%lu",&flags);
    if(DEBUG&1) printf("fi_flags %lu\n",flags);
    if(current_fi) current_fi->flags=flags;
  }else if(!strncmp("fi_uses",s,7)){
    int t;struct Var *v;
    s+=7;pragma_killsp();
    s=pragma_cpbez(buff,s);
    t=nesting;nesting=0;
    v=find_var(buff,0);
    nesting=t;
    sscanf(s,"%d",&t);
    if(DEBUG&1) printf("new fi_use %s,%d\n",buff,t);
    if(current_fi){
      current_fi->use_cnt++;
      current_fi->use_list=myrealloc(current_fi->use_list,current_fi->use_cnt*sizeof(struct varlist));
      current_fi->use_list[current_fi->use_cnt-1].v=v;
      current_fi->use_list[current_fi->use_cnt-1].flags=t;
    }
  }else if(!strncmp("fi_changes",s,10)){
    int t;struct Var *v;
    s+=10;pragma_killsp();
    s=pragma_cpbez(buff,s);
    t=nesting;nesting=0;
    v=find_var(buff,0);
    nesting=t;
    sscanf(s,"%d",&t);
    if(DEBUG&1) printf("new fi_change %s,%d\n",buff,t);
    if(current_fi){
      current_fi->change_cnt++;
      current_fi->change_list=myrealloc(current_fi->change_list,current_fi->change_cnt*sizeof(struct varlist));
      current_fi->change_list[current_fi->change_cnt-1].v=v;
      current_fi->change_list[current_fi->change_cnt-1].flags=t;
    }
  }else if(!strncmp("fi_calls",s,8)){
    int t;struct Var *v;
    s+=8;pragma_killsp();
    s=pragma_cpbez(buff,s);
    t=nesting;nesting=0;
    v=find_var(buff,0);
    nesting=t;
    sscanf(s,"%d",&t);
    if(DEBUG&1) printf("new fi_call %s,%d\n",buff,t);
    if(current_fi){
      current_fi->call_cnt++;
      current_fi->call_list=myrealloc(current_fi->call_list,current_fi->call_cnt*sizeof(struct varlist));
      current_fi->call_list[current_fi->call_cnt-1].v=v;
      current_fi->call_list[current_fi->call_cnt-1].flags=t;
    }
  }else if(!strncmp("fi_regs",s,7)){
    int r;
    s+=7;pragma_killsp();
    pragma_cpbez(buff,s);
    for(r=1;r<=MAXR;r++)
      if(!strcmp(buff,regnames[r])) break;
    if(r<=MAXR&&current_fi)
      BSET(current_fi->regs_modified,r);
  }else if(!strncmp("printflike",s,10)){
    struct Var *v;
    s+=10;pragma_killsp();
    pragma_cpbez(buff,s);
    if(DEBUG&1) printf("printflike %s\n",buff);
    v=find_var(buff,0);
    if(v){
      v->flags|=PRINTFLIKE;
      if(DEBUG&1) printf("succeeded\n");
    }
  }else if(!strncmp("scanflike",s,9)){
    struct Var *v;
    s+=9;pragma_killsp();
    pragma_cpbez(buff,s);
    if(DEBUG&1) printf("scanflike %s\n",buff);
    v=find_var(buff,0);
    if(v){
      v->flags|=SCANFLIKE;
      if(DEBUG&1) printf("succeeded\n");
    }
  }else if(!strncmp("only-inline",s,11)){
    s+=11;pragma_killsp();
    if(!strncmp("on",s,2)){
      if(DEBUG&1) printf("only-inline on\n");
      only_inline=1;
    }else{
      if(DEBUG&1) printf("only-inline off\n");
      only_inline=2;
    }
  }else if(!strncmp("pack",s,4)){
    /* packing of structures */
    s+=4;pragma_killsp();
    if(*s=='(') { s++;pragma_killsp();}
    if(*s==')'||!strncmp("pop",s,3)){
      if(pidx>0) pack_align=pack[--pidx];
    }else{
      int p=0;
      sscanf(s,"%i",&p);
      if(pidx==PACKSTACKSIZE){
        memmove(pack,pack+1,(PACKSTACKSIZE-1)*sizeof(pack[0]));
        pidx--;
      }
      pack[pidx++]=pack_align;
      pack_align=p;
    }
#if 0
  }else if(!strncmp("type",s,4)){
    /*  Typ eines Ausdrucks im Klartext ausgeben    */
    np tree;
    s+=4;strcat(s,";");
    tree=expression();
    if(tree&&type_expression(tree)){
      printf("type of %s is:\n",string+7);
      prd(stdout,tree->ntyp);printf("\n");
    }
    if(tree) free_expression(tree);
  }else if(!strncmp("tree",s,4)){
    /*  gibt eine expression aus    */
    np tree;
    s+=4;strcat(s,";");
    tree=expression();
    if(tree&&type_expression(tree)){
      printf("tree of %s is:\n",string+7);
      pre(stdout,tree);printf("\n");
    }
    if(tree) free_expression(tree);
#endif
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
#endif
  }else{
#ifdef HAVE_TARGET_PRAGMAS
    handle_pragma(s);
#endif
  }
}
void killsp(void)
/*  Ueberspringt Fuellzeichen                   */
/*  noch einige unschoene Dinge drin            */
{
  /*FIXME: #pragma etc. */
  while(!eof&&(ttWHI(ctok->type)||ctok->type==PRAGMA||(!input_wpo&&!ls.condcomp))){
    if(ctok->type==PRAGMA)
      do_pragma(ctok->name);
    next_token();
  }
}
void enter_block(void)
/*  Setzt Zeiger/Struckturen bei Eintritt in neuen Block    */
{
    if(nesting>=MAXN){error(9,nesting);return;}
    nesting++;
    if(DEBUG&1) printf("enter block %d\n",nesting);
    first_ilist[nesting]=last_ilist[nesting]=0;
    first_sd[nesting]=last_sd[nesting]=0;
    first_si[nesting]=last_si[nesting]=0;
    first_var[nesting]=last_var[nesting]=0;
    if(nesting==1){
        first_llist=last_llist=0;
        first_clist=last_clist=0;
        merk_varf=merk_varl=0;
        merk_ilistf=merk_ilistl=0;
        merk_sif=merk_sil=0;
/*  struct-declarations erst ganz am Schluss loeschen. Um zu vermeiden,     */
/*  dass struct-declarations in Prototypen frei werden und dann eine        */
/*  spaetere struct, dieselbe Adresse bekommt und dadurch gleich wird.      */
/*  Nicht sehr schoen - wenn moeglich noch mal aendern.                     */
/*        merk_sdf=merk_sdl=0;*/
        afterlabel=0;
    }
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
#endif
}
void leave_block(void)
/*  Setzt Zeiger/Struckturen bei Verlassen eines Blocks     */
{
  static int inleave;
  int i;
  if(inleave) return;
  inleave=1;
#ifdef HAVE_ECPP
/* removed */
/* removed */
/* removed */
#endif
  for(i=1;i<=MAXR;i++)
    if(regbnesting[i]==nesting) regsbuf[i]=0;
  if(nesting<0){error(10);inleave=0;return;}
  if(DEBUG&1) printf("leave block %d vla=%p\n",nesting,(void *)block_vla[nesting]);
  if(block_vla[nesting]) clearvl();
  if(nesting>0){
    if(merk_varl) merk_varl->next=first_var[nesting]; else merk_varf=first_var[nesting];
    if(last_var[nesting]) merk_varl=last_var[nesting];
    if(merk_sil) merk_sil->next=first_si[nesting]; else merk_sif=first_si[nesting];
    if(last_si[nesting]) merk_sil=last_si[nesting];
    if(merk_sdl) merk_sdl->next=first_sd[nesting]; else merk_sdf=first_sd[nesting];
    if(last_sd[nesting]) merk_sdl=last_sd[nesting];
    if(merk_ilistl) merk_ilistl->next=first_ilist[nesting]; else merk_ilistf=first_ilist[nesting];
    if(last_ilist[nesting]) merk_ilistl=last_ilist[nesting];
  }
  if(nesting==1){
    if(cross_module){
      /* anything to do? */
    }else{
      if(merk_varf) gen_vars(merk_varf);
      if(first_llist) free_llist(first_llist);
      first_llist=0;
      if(first_clist) free_clist(first_clist);
      first_clist=0;
      if(merk_varf) free_var(merk_varf);
      merk_varf=0;
      if(merk_sif) free_si(merk_sif);
      merk_sif=0;
      /*  struct-declarations erst ganz am Schluss loeschen. Um zu vermeiden,     */
      /*  dass struct-declarations in Prototypen frei werden und dann eine        */
      /*  spaetere struct, dieselbe Adresse bekommt und dadurch gleich wird.      */
      /*  Nicht sehr schoen - wenn moeglich noch mal aendern.                     */
      /*        if(merk_sdf) free_sd(merk_sdf);*/
      if(merk_ilistf) free_ilist(merk_ilistf);
      merk_ilistf=0;
    }
  }
  if(nesting==0){
    if(/*ecpp||*/cross_module){
      /* don't free struct_declarations in EC++ for now, since they can be */
      /* referenced even when they are not in scope */
      /* anything to do? */
    }else{
      /*  struct-declarations erst ganz am Schluss loeschen. Um zu vermeiden,     */
      /*  dass struct-declarations in Prototypen frei werden und dann eine        */
      /*  spaetere struct, dieselbe Adresse bekommt und dadurch gleich wird.      */
      /*  Nicht sehr schoen - wenn moeglich noch mal aendern.                     */
      if(first_si[0]) free_si(first_si[0]);
      if(merk_sdf) free_sd(merk_sdf);
      if(first_ext)
	gen_vars(first_ext);
      if(first_var[0])
	gen_vars(first_var[0]);
      if(first_ext)
	free_var(first_ext);
      if(first_var[0])
	free_var(first_var[0]);
      if(first_sd[0]) free_sd(first_sd[0]);
      if(first_ilist[0]) free_ilist(first_ilist[0]);
    }
  }
  nesting--;
  inleave=0;
}
void pra(FILE *f,struct argument_list *p)
/*  Gibt argument_list umgekehrt auf Bildschirm aus             */
{
    if(p->next){ pra(f,p->next);fprintf(f,",");}
    if(p->arg) pre(f,p->arg);
}
void pre(FILE *f,np p)
/*  Gibt expression auf Bildschirm aus                          */
{
    int c;
    c=p->flags;
    if(p->sidefx) fprintf(f,"/");
    if(p->lvalue) fprintf(f,"|");
    if(c==CALL){fprintf(f,"call-function(");pre(f,p->left);fprintf(f,")(");
                if(p->alist) pra(f,p->alist);
                fprintf(f,")");return;}
    if(c==CAST){fprintf(f,"cast(");pre(f,p->left);
                fprintf(f,"->");prd(f,p->ntyp);
                fprintf(f,")");return;}
    if(c==MEMBER){if(p->identifier) fprintf(f,".%s",p->identifier);return;}
    if(c==IDENTIFIER){if(p->identifier) fprintf(f,"%s",p->identifier);
        fprintf(f,"+");printval(f,&p->val,LONG); return;}
    fprintf(f,"%s(",ename[c]);
    if(p->left) pre(f,p->left);
    if(p->right){
        fprintf(f,",");
        pre(f,p->right);
    }
    fprintf(f,")");
    if(c==CEXPR||c==PCEXPR){fprintf(f,"(value="); printval(f,&p->val,p->ntyp->flags); fprintf(f,")");}
}
static int pp_line;
void do_error(int errn,va_list vl)
/*  Behandelt Ausgaben wie Fehler und Meldungen */
{
    int type;
    char *errstr="",*txt=filename;
    if(c_flags_val[8].l&&c_flags_val[8].l<=errors)
      return;
    if(errn==-1) errn=158;
    type=err_out[errn].flags;
#ifdef HAVE_MISRA
/* removed */
#endif
    if(type&DONTWARN) return;
    if(type&WARNING) errstr="warning";
    if(type&ERROR) errstr="error";
    if(input_wpo){
      fprintf(stderr,"%s %d: ",errstr,errn);
    }else if((type&NOLINE)/*||((type&PREPROC)&&pp_line<=0)*/){
      fprintf(stderr,"%s %d: ",errstr,errn);
    }else if(type&(INFUNC|INIC)){
      if((type&INIC)&&err_ic&&err_ic->line){
	fprintf(stderr,"%s %d in line %d of \"%s\": ",errstr,errn,err_ic->line,err_ic->file);
      }else{
	fprintf(stderr,"%s %d in function \"%s\": ",errstr,errn,cur_func);
      }
    }else if(!input_wpo){
      int n;
      if(eof){
	fprintf(stderr,">EOF\n");
      }else{
	if(ls.cli!=0) ls.copy_line[ls.cli]=0;
	fprintf(stderr,">%s\n",ls.copy_line);
      }
      if(type&PREPROC){
	      txt=current_filename;
	      n=pp_line;
      }else{
	if(ctok)
	  n=ctok->line;
	else
	  n=-1;
      }
      if(c_flags[20]&USEDFLAG){   /*  strip-path from filename */
	char *p=txt,c;
	while(c=*p++)
	  if(c==':'||c=='/'||c=='\\') txt=p;
      }
      fprintf(stderr,"%s %d in line %d of \"%s\": ",errstr,errn,n,txt);
    }
    vfprintf(stderr,err_out[errn].text,vl);
    fprintf(stderr,"\n");
    if(type&ERROR){
      errors++;
      if(c_flags_val[8].l&&c_flags_val[8].l<=errors&&!(type&NORAUS))
	{fprintf(stderr,"Maximum number of errors reached!\n");raus();}
    }
    if(type&FATAL){fprintf(stderr,"aborting...\n");raus();}
}
void error(int errn,...)
{
  va_list vl;
  va_start(vl,errn);
  do_error(errn,vl);
  va_end(vl);
}
void ucpp_ouch(char *fmt, ...)
{
  ierror(0);
}
void do_ucpp_error(char *fmt,va_list vl)
{
  int i;
  for(i=0;i<err_num;i++){
    if(!strcmp(fmt,err_out[i].text))
      break;
  }
  if(*fmt=='#'){
    /* #error/#warning-directive */
    i=*(fmt+1)=='w'?325:292;
  }
  if(i>=err_num){
    puts(fmt);
    ierror(0);
  }
  do_error(i,vl);
}
void ucpp_error(long line, char *fmt, ...)
{
  va_list ap;
  pp_line=line;
  va_start(ap, fmt);
  do_ucpp_error(fmt,ap);
  va_end(ap);
}
void ucpp_warning(long line, char *fmt, ...)
{
  va_list ap;
  pp_line=line;
  va_start(ap, fmt);
  do_ucpp_error(fmt,ap);
  va_end(ap);
}

void misra(int n,...)
{
  va_list vl;
  if(!misracheck) return;
  va_start(vl,n);
  fprintf(stderr,"MISRA error %d\n",n);
  va_end(vl);
}

void misra_error(int n, int rule, int subrule, int line, ...) {
	va_list vl;
	struct misra_err_out* misr_err;
	char* mis_vers_string;
	char mis_numb_string[100];
	char* rule_text;
	int not_found;
	if (!misracheck) return;
	mis_vers_string = 0;
	va_start(vl,line);

	if (line == 0) line = ctok->line;

	if ((n) && (misraversion==1998)) {
		if (misra_98_warn_flag[n-1] != 1) return; /* TODO: Zhler setzen wenn nur ein paar nicht ausgegeben werden sollen */
		mis_vers_string = mystrdup("1998");
		sprintf(mis_numb_string,"%d",n);
		misr_err = NULL;			
	} else if ((rule) && (misraversion==2004)) {
		if (misra_04_warn_flag[rule-1][subrule-1] != 1) return;/* TODO: Zhler setzen wenn nur ein paar nicht ausgegeben werden sollen */
		mis_vers_string = mystrdup("2004");
		sprintf(mis_numb_string,"Chapter %d, Rule %d",rule,subrule);
		if (subrule) {
  		misr_err = misra_err_out;
	  	not_found = 1;
		  while ( not_found ) {
				if ((misr_err->chapter == rule) && (misr_err->rule == subrule)) {
					not_found = 0;
					break;
				}
				misr_err++;
			}
		}
	}
	if (!mis_vers_string) return;
	if (misr_err) fprintf(stderr,"MISRA(%s) Rule violation (%s) in line <%d>\n%s\n",mis_vers_string,mis_numb_string,line,misr_err->text);
	else fprintf(stderr,"MISRA(%s) Rule violation (%s) in line <%d>: No description found\n",mis_vers_string,mis_numb_string,line);

	if (mis_vers_string) free(mis_vers_string);

	va_end(vl);
}

void misra_neu(int n,int rule,int subrule, int line, ...)
{
  va_list vl;
	int mis_warn = 0;
	misra_error(n,rule,subrule,line);
	return;
	va_start(vl,line);
  va_end(vl);

}

FILE *open_out(char *name,char *ext)
/*  Haengt ext an name an und versucht diese File als output zu oeffnen */
{
  char *s,*p;FILE *f;
  if(ext){
    s=mymalloc(strlen(name)+strlen(ext)+2);
    strcpy(s,name);
    p=s+strlen(s);
    while(p>=s){
      if(*p=='.'){*p=0;break;}
      p--;
    }
    strcat(s,".");
    strcat(s,ext);
  }else
    s=name;
  f=fopen(s,"w");
  if(!f) fprintf(stderr,"Couldn't open <%s> for output!\n",s);
  if(ext) free(s);
  return(f);
}

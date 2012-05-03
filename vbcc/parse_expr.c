/*  $VER: vbcc (parse_expr.c) V0.8  */

#include "vbcc_cpp.h"
#include "vbc.h"

static char FILE_[]=__FILE__;



np expression(void)
/*  Komma-Ausdruecke  */
{
  np left,right,new;
  left=assignment_expression();
  if(!left->flags) return 0;
  killsp();
  while(ctok->type==COMMA){
#ifdef HAVE_MISRA
/* removed */
#endif
    next_token();
    killsp();
    right=assignment_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->ntyp=0;
    new->flags=KOMMA;
    left=new;
    killsp();
  }
  return left;
}
np assignment_expression(void)
/*  Zuweisungsausdruecke  */
{
  np left,new;int c=ASSIGNOP,op;
  left=conditional_expression();
  killsp();
  switch(ctok->type){
  case ASGN: c=ASSIGN;break;
  case ASSTAR: op=MULT;break;
  case ASSLASH: op=DIV;break;
  case ASPCT: op=MOD;break;
  case ASPLUS: op=ADD;break;
  case ASMINUS: op=SUB;break;
  case ASAND: op=AND;break;
  case ASCIRC: op=XOR;break;
  case ASOR: op=OR;break;
  case ASLSH: op=LSHIFT;break;
  case ASRSH: op=RSHIFT;break;
  default:
    return left;
  }
  next_token();
  new=mymalloc(NODES);
  new->left=left;
  new->ntyp=0;
  if(c==ASSIGN){
    new->right=assignment_expression();
    new->flags=ASSIGN;
  }else{
    /*  ASSIGNOP(a,b)->ASSIGN(a,OP(a,b))    */
    new->flags=ASSIGNOP;    /* nur um zum Merken, dass nur einmal */
                            /* ausgewertet werden darf            */
    new->right=mymalloc(NODES);
    new->right->left=left;
    new->right->right=assignment_expression();
    new->right->ntyp=0;
    new->right->flags=op;
  }
  return new;
}
np conditional_expression(void)
/*  Erledigt ? :   */
{
  np left,new;
  left=logical_or_expression();
  killsp();
  if(ctok->type==QUEST){   
    next_token();killsp();
    new=mymalloc(NODES);
    new->flags=COND;
    new->ntyp=0;
    new->left=left;
    new->right=mymalloc(NODES);
    new->right->flags=COLON;
    new->right->ntyp=0;
    new->right->left=expression();
    killsp();
    if(ctok->type==T_COLON){next_token();killsp();} else error(70);
    new->right->right=conditional_expression();
    left=new;
    killsp();
  }
  return left;
}
np logical_or_expression(void)
/*  Erledigt ||  */
{
  np left,right,new;int bra;
  if(ctok->type==LPAR) bra=1; else bra=0;
  left=logical_and_expression();
  killsp();
  while(ctok->type==T_LOR){
#ifdef HAVE_MISRA
/* removed */
#endif
    next_token();
    killsp();
    if(ctok->type==LPAR) bra=1; else bra=0;
    right=logical_and_expression();
#ifdef HAVE_MISRA
/* removed */
#endif
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=LOR;
    new->ntyp=0;
    left=new;
    killsp();
    if(ctok->type==T_LAND) error(222);
  }
  return left;
}
np logical_and_expression(void)
/*  Erledigt &&  */
{
  np left,right,new;int bra;
  if(ctok->type==LPAR) bra=1; else bra=0;
  left=inclusive_or_expression();
  killsp();
  while(ctok->type==T_LAND){
#ifdef HAVE_MISRA
/* removed */
#endif
    next_token();
    killsp();
    if(ctok->type==LPAR) bra=1; else bra=0;
    right=inclusive_or_expression();
#ifdef HAVE_MISRA
/* removed */
#endif
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=LAND;
    new->ntyp=0;
    left=new;
    killsp();
    if(ctok->type==T_LOR) error(222);
  }
  return left;
}
np inclusive_or_expression(void)
/*  Erledigt |  */
{
  np left,right,new;
  left=exclusive_or_expression();
  killsp();
  while(ctok->type==T_OR){
    next_token();
    killsp();
    right=exclusive_or_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=OR;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np exclusive_or_expression(void)
/*  Erledigt ^  */
{
  np left,right,new;
  left=and_expression();
  killsp();
  while(ctok->type==CIRC){
    next_token();
    killsp();
    right=and_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=XOR;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np and_expression(void)

/*  Erledigt &  */
{
  np left,right,new;
  left=equality_expression();
  killsp();
  while(ctok->type==T_AND){
    next_token();
    killsp();
    right=equality_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=AND;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np equality_expression(void)
/*  Erledigt == und !=  */
{
  np left,right,new;int c;
  left=relational_expression();
  killsp();
  while(ctok->type==SAME||ctok->type==NEQ){
    if(ctok->type==NEQ) c=INEQUAL; else c=EQUAL;
    next_token();
    killsp();
    right=relational_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=c;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np relational_expression(void)
/*  Erledigt <,>,<=,>=  */
{
  np left,right,new;int c;
  left=shift_expression();
  killsp();
  while(ctok->type==LT||ctok->type==GT||ctok->type==LEQ||ctok->type==GEQ){
    if(ctok->type==LT) c=LESS;
    else if(ctok->type==GT) c=GREATER;
    else if(ctok->type==LEQ) c=LESSEQ;
    else c=GREATEREQ;
    next_token();
    killsp();
    right=shift_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=c;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np shift_expression(void)
/*  Erledigt <<,>>  */
{
  np left,right,new;int c;
  left=additive_expression();
  killsp();
  while(ctok->type==LSH||ctok->type==RSH){
    if(ctok->type==LSH)
      c=LSHIFT; 
    else
      c=RSHIFT;
    next_token();
    killsp();
    right=additive_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=c;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np additive_expression(void)
/*  Erledigt +,-  */
{
  np left,right,new;int c;
  left=multiplicative_expression();
  killsp();
  while(ctok->type==PLUS||ctok->type==T_MINUS){
    if(ctok->type==PLUS) c=ADD; else c=SUB;
    next_token();killsp();
    right=multiplicative_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=c;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np multiplicative_expression(void)
/*  Erledigt *,/,%  */
{
  np left,right,new;int c;
  left=cast_expression();
  killsp();
  while(ctok->type==STAR||ctok->type==SLASH||ctok->type==PCT){
    if(ctok->type==STAR) c=MULT; 
    else if(ctok->type==SLASH) c=DIV; 
    else c=MOD;
    next_token();killsp();
    right=cast_expression();
    new=mymalloc(NODES);
    new->left=left;
    new->right=right;
    new->flags=c;
    new->ntyp=0;
    left=new;
    killsp();
  }
  return left;
}
np cast_expression(void)
/*  Erledigt (typ)  */
{
  np new;char *imerk,buff[MAXI];
  killsp();
  if(ctok->type!=LPAR||!declaration(1)) return unary_expression();
  next_token();killsp();
  new=mymalloc(NODES);
  new->right=0;
  buff[0]=0;
  imerk=ident;ident=buff;
  new->ntyp=declarator(declaration_specifiers());
  ident=imerk;
  if(buff[0]) error(356);
  killsp();
  if(ctok->type!=RPAR) error(59); else next_token();
  killsp();
  if(c99&&ctok->type==LBRA){
    /* compund literals */
    init_dyn_cnt=0;
    new->cl=initialization(new->ntyp,nesting!=0,0,0,0,0);
    killsp();
    if(new->cl){
      if(ISARRAY(new->ntyp->flags)&&zmeqto(new->ntyp->size,l2zm(0L))){
	struct const_list *p=new->cl;
	while(p){new->ntyp->size=zmadd(p->idx,l2zm(1L));p=p->next;}
      }
    }
    new->flags=LITERAL;
    new->left=0;
    new->val.vmax=l2zm((long)init_dyn_cnt);
  }else{
    new->flags=CAST;
    new->left=cast_expression();
  }
  return new;
}
np unary_expression(void)
/*  Erledigt !,~,++,--,+,-,*,&,sizeof,__typeof  */
{
  np new;char *merk;
  killsp();
  if(ctok->type!=NAME&&ctok->type!=PLUS&&ctok->type!=PPLUS&&ctok->type!=T_MINUS&&ctok->type!=MMINUS&&ctok->type!=T_AND&&ctok->type!=STAR&&ctok->type!=NOT&&ctok->type!=LNOT)
    return postfix_expression();
  if(ctok->type==NAME){
    enum {SIZEOF,TYPEOF,ALIGNOF,OFFSETOF} op;
    if(strcmp("sizeof",ctok->name)&&strcmp("__typeof",ctok->name)&&strcmp("__alignof",ctok->name)&&strcmp("__offsetof",ctok->name)){
      return postfix_expression();
    }else{
      if(*ctok->name=='s') 
	op=SIZEOF; 
      else if(ctok->name[2]=='a') 
	op=ALIGNOF;
      else if(ctok->name[2]=='t')
	op=TYPEOF;
      else
	op=OFFSETOF;
      next_token();
      killsp();
      new=mymalloc(NODES);
      new->flags=CEXPR;
      new->ntyp=new_typ();
      if(op==SIZEOF||op==OFFSETOF){
	if(HAVE_INT_SIZET)
	  new->ntyp->flags=(UNSIGNED|INT);
	else
	  new->ntyp->flags=(UNSIGNED|LONG);
      }else
	new->ntyp->flags=INT;
      new->right=0;
      new->left=0;
      
      if(ctok->type==LPAR&&declaration(1)){
	struct Typ *t;
	next_token();killsp();
	buff[0]=0;
	merk=ident;ident=buff;
	t=declarator(declaration_specifiers());
	if(type_uncomplete(t)) error(176);
	ident=merk;
	if(buff[0]) error(356);
	if(op==OFFSETOF){
	  if(ctok->type==COMMA)
	    next_token();
	  else
	    error(57);
	  if(ctok->type!=NAME) error(76);
	  if(t->flags!=STRUCT){
	    error(310);
	    vumax=ul2zum(0UL);
	  }else
	    vumax=zm2zum(struct_offset(t->exact,ctok->name));
	  next_token();
	  if(HAVE_INT_SIZET)
	    new->val.vuint=zum2zui(vumax);
	  else
	    new->val.vulong=zum2zul(vumax);
	}else if(op==SIZEOF){
	  if(is_vlength(t)){
	    new->dsize=vlength_szof(t);
	    new->flags=IDENTIFIER;
	    new->val.vmax=l2zm(0L);
	    new->sidefx=0;
	    new->identifier=empty;
	  }else{
	    if(HAVE_INT_SIZET)
	      new->val.vuint=zum2zui(zm2zum(szof(t)));
	    else
	      new->val.vulong=zum2zul(zm2zum(szof(t)));
	  }
	}else if(op==ALIGNOF){
	  new->val.vint=zm2zi(falign(t));
	}else if(op==TYPEOF){
	  new->val.vint=zm2zi(l2zm(t->flags));
	}
	freetyp(t);
	killsp();
	if(ctok->type!=RPAR) error(59); else next_token();
      }else{
	np tree;
	if(op==OFFSETOF) error(0);
	killsp();
	tree=unary_expression();
	if(!tree||!type_expression(tree)){
	  if(op==SIZEOF){
	    if(HAVE_INT_SIZET)
	      new->val.vuint=zum2zui(ul2zum(0UL));
	    else
	      new->val.vulong=zum2zul(ul2zum(0UL));
	    error(73);
	  }else{
	    new->val.vint=zm2zi(l2zm(-1L));
	  }
	}else{
	  if(op==SIZEOF){
#ifdef HAVE_MISRA
/* removed */
#endif
		if(type_uncomplete(tree->ntyp)) error(176);
	    if(is_vlength(tree->ntyp)){
	      new->dsize=vlength_szof(tree->ntyp);
	      new->flags=IDENTIFIER;
	      new->val.vmax=l2zm(0L);
	      new->sidefx=0;
	      new->identifier=empty;
	    }else{
	      if(HAVE_INT_SIZET)
		new->val.vuint=zum2zui(zm2zum(szof(tree->ntyp)));
	      else
		new->val.vulong=zum2zul(zm2zum(szof(tree->ntyp)));
	    }
	  }else{
	    if(op==ALIGNOF)
	      new->val.vint=zm2zi(falign(tree->ntyp));
	    else
	      new->val.vint=zm2zi(l2zm(tree->ntyp->flags));
	  }
	}
	if(tree) free_expression(tree);killsp();
      }
      return new;
    }
  }
  new=mymalloc(NODES);
  new->right=0;
  new->ntyp=0;
  if(ctok->type==PLUS){
    next_token();
    free(new);
    return cast_expression();
  }else if(ctok->type==T_MINUS){
    next_token();
    new->left=cast_expression();
    new->flags=MINUS;    
  }else if(ctok->type==PPLUS){
    next_token();
    new->left=unary_expression();
    new->flags=PREINC;
  }else if(ctok->type==MMINUS){
    next_token();
    new->left=unary_expression();
    new->flags=PREDEC;
  }else if(ctok->type==T_AND){
    next_token();
    new->left=cast_expression();
    new->flags=ADDRESS;
  }else if(ctok->type==STAR){
    next_token();
    new->left=cast_expression();
    new->flags=CONTENT;
  }else if(ctok->type==NOT){
    next_token();
    new->left=cast_expression();
    new->flags=KOMPLEMENT;
  }else if(ctok->type==LNOT){
    next_token();
    new->left=cast_expression();
    new->flags=NEGATION;
  }
  new->right=0;
  new->ntyp=0;
  return new;
}
np postfix_expression(void)
/*  Erledigt [],(),.,->,++,--  */
{
  np new,left;
  left=primary_expression();
  killsp();
  while(ctok->type==LBRK||ctok->type==LPAR||ctok->type==DOT||ctok->type==ARROW||ctok->type==PPLUS||ctok->type==MMINUS){
    new=mymalloc(NODES);
    new->ntyp=0;
    new->right=0;
    new->left=left;
    if(ctok->type==MMINUS){
      next_token();
      new->flags=POSTDEC;
    }else if(ctok->type==ARROW){
      next_token(); killsp();
      new->flags=DSTRUCT;
      new->right=identifier_expression();
      new->right->flags=MEMBER;
      new->left=mymalloc(NODES);
      new->left->ntyp=0;
      new->left->left=left;
      new->left->right=0;
      new->left->flags=CONTENT;
    }else if(ctok->type==LBRK){
      next_token(); killsp();
      new->flags=CONTENT;
      new->left=mymalloc(NODES);
      new->left->flags=ADD;
      new->left->ntyp=0;
      new->left->left=left;
      new->left->right=expression();
      killsp();
      if(ctok->type!=RBRK) error(62); else next_token();
    }else if(ctok->type==PPLUS){
      next_token();
      new->flags=POSTINC;
    }else if(ctok->type==DOT){
      next_token();killsp();
      new->right=identifier_expression();
      new->flags=DSTRUCT;
      new->right->flags=MEMBER;
    }else if(ctok->type==LPAR){
      struct argument_list *al,*first_alist=0,*last_alist=0;np n;
#ifdef HAVE_MISRA
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
/* removed */
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
      new->left=left;
      new->flags=CALL;
      new->right=0;
      new->alist=argument_list_expression();
    }
    left=new;
    killsp();
  }
  return left;
}
struct argument_list *argument_list_expression(void)
/* returns CALL node with alist attached, but without identifier */
{
  struct argument_list *al,*first_alist=0,*last_alist=0;np n;
  /*new=mymalloc(NODES);memset(new,0,NODES);*/
  if(ctok->type!=LPAR)ierror(0);
  next_token();killsp();
  while(ctok->type!=RPAR){
    n=assignment_expression();
    al=mymalloc(sizeof(struct argument_list));
    al->arg=n;al->next=0;
    if(last_alist){
      last_alist->next=al;
      last_alist=al;
    }else{
      last_alist=first_alist=al;
    }
    killsp();
    if(ctok->type==COMMA){
      next_token();killsp();
      if(ctok->type==RPAR) error(59);
    }else if(ctok->type!=RPAR) error(57);
  }
  if(ctok->type!=RPAR) error(59); else next_token();
  return first_alist;
}
np primary_expression(void)
/*  primary-expressions (Konstanten,Strings,(expr),Identifier)  */
{
  np new;struct token mtok;
  if(ctok->type==NUMBER) return constant_expression();
  if(ctok->type==T_STRING||ctok->type==T_CHAR) return string_expression();
  if(ctok->type==LPAR){
    next_token();killsp();
    new=expression();
    killsp();
    if(ctok->type!=RPAR) error(59); else next_token();
    return new;
  }
  return identifier_expression();
}
np string_expression(void)
/*  Gibt Zeiger auf string oder Zeichenkonstante zurueck  */
{
  np new; char f,*s,*p;int flag,val;zmax zm;
  static char *string;
  static size_t slen;
  p=string;
  while(1){
    s=ctok->name;
    if(*s=='L') {
#ifdef HAVE_MISRA
/* removed */
#endif
		s++;}    /*  Noch keine erweiterten Zeichen  */
    if(ctok->type==T_STRING&&*s!='\"') ierror(0);
    if(ctok->type==T_CHAR&&*s!='\'') ierror(0);
    f=*s++;
    while(*s!=f&&*s!=0){
      size_t pos=p-string;
      if(slen<=pos){
	slen+=128;
	string=myrealloc(string,slen);
	p=string+pos;
      }
      if(*s=='\\'){
	s++;
	if(*s=='\\'){*p++='\\';s++;continue;}
	if(*s=='n'){*p++='\n';s++;continue;}
	if(*s=='t'){*p++='\t';s++;continue;}
	if(*s=='r'){*p++='\r';s++;continue;}
	if(*s=='v'){*p++='\v';s++;continue;}
	if(*s=='b'){*p++='\b';s++;continue;}
	if(*s=='f'){*p++='\f';s++;continue;}
	if(*s=='a'){*p++='\a';s++;continue;}
	if(*s=='\?'){*p++='\?';s++;continue;}
	if(*s=='\''){*p++='\'';s++;continue;}
	if(*s=='\"'){*p++='\"';s++;continue;}
	flag=val=0;
	while(*s>='0'&&*s<='7'&&flag<3){
	  val=val*8+*s-'0';
	  s++;flag++;
	}
	if(flag){*p++=val;continue;}
	if(*s=='x'){
	  s++;val=0;
	  while((*s>='0'&&*s<='9')||(*s>='a'&&*s<='f')||(*s>='A'&&*s<='F')){
	    val=val*16;
	    if(*s>='0'&&*s<='9') val+=*s-'0';
	    if(*s>='a'&&*s<='f') val+=*s-'a'+10;
	    if(*s>='A'&&*s<='F') val+=*s-'A'+10;
	    s++;
	  }
	  *p++=val;continue;
	}
	error(71);
#ifdef HAVE_MISRA
/* removed */
#endif
      }
      *p++=*s++;
    }
    if(*s!=f) error(74);
    next_token();
    killsp();
    if(f!='\"'||ctok->type!=T_STRING) break;
  }
  if(!string){
    slen+=128;
    string=myrealloc(string,slen);
    p=string;
  }
  *p=0;
  new=mymalloc(NODES);
  new->ntyp=new_typ();
  if(f=='\"'){
    struct const_list *cl,**prev;int i;
    new->ntyp->flags=ARRAY;
    new->ntyp->size=l2zm((long)(p-string)+1);
    new->ntyp->next=new_typ();
    new->ntyp->next->flags=STRINGCONST|CHAR;
    new->ntyp->next->next=0;
    new->flags=STRING;
    prev=&new->cl;
    for(i=0;i<p-string+1;i++){
      cl=mymalloc(CLS);
      cl->next=0;
      cl->tree=0;
      cl->idx=l2zm((long)i);
      cl->val.vmax=l2zm(0L);
      cl->other=mymalloc(CLS);
      cl->other->next=cl->other->other=0;
      cl->other->tree=0;
      cl->other->idx=l2zm(0L);
      cl->other->val.vchar=zm2zc(l2zm((long)string[i]));
      *prev=cl;
      prev=&cl->next;
    }
    /*        new->identifier=add_identifier(string,p-string);*/
    new->val.vmax=l2zm(0L);
  }else{
    char *l;
    if(ecpp)new->ntyp->flags=CONST|CHAR;
    else new->ntyp->flags=CONST|INT;
    new->ntyp->next=0;
    new->flags=CEXPR;
		/* TODO: Hier eventuell was mitspeichern das char */
    zm=l2zm(0L);
    p--;
    if(p>string){ error(72);
#ifdef HAVE_MISRA
/* removed */
#endif
	} 
    for(BIGENDIAN?(l=string):(l=p);BIGENDIAN?(l<=p):(l>=string);BIGENDIAN?(l++):(l--)){
      /*  zm=zm<<CHAR_BIT+*p  */
      zm=zmlshift(zm,char_bit);
      zm=zmadd(zm,l2zm((long)*l));
      new->val.vint=zm2zi(zm);
    }
  }
  new->left=new->right=0;
  return new;
}
np constant_expression(void)
/*  Gibt Zeiger auf erzeugt Struktur fuer Konstante zurueck */
{
  np new; zldouble db;
  zumax value,zbase,digit;unsigned long base=10,t;
  char *s,*merk;int warned=0,tm=0;
  merk=s=ctok->name;
  value=ul2zum(0L);
  new=mymalloc(NODES);
  new->ntyp=new_typ();
  new->ntyp->flags=0;
  new->flags=CEXPR;
  new->left=new->right=0;
  new->sidefx=0;
  if(*s=='0'){
    s++;
    if(*s=='x'||*s=='X'){s++;base=16;} else base=8;
  }
  zbase=ul2zum(base);
  if(*s>='0'&&*s<='9') t=*s-'0'; 
  else if(*s>='a'&&*s<='f') t=*s-'a'+10; 
  else if(*s>='A'&&*s<='F') t=*s-'A'+10;
  else t=20;
  while(t<base){
    digit=ul2zum(t);
    if(!warned){
      if(!c99&&!zumleq(value,zumdiv(zumsub(t_max(UNSIGNED|LONG),digit),zbase)))
	warned=1;
      if(c99&&!zumleq(value,zumdiv(zumsub(t_max(UNSIGNED|LLONG),digit),zbase)))
	warned=1;      
    }
    value=zumadd(zummult(value,zbase),digit);
    s++;
    if(*s>='0'&&*s<='9') t=*s-'0'; 
    else if(*s>='a'&&*s<='f') t=*s-'a'+10; 
    else if(*s>='A'&&*s<='F') t=*s-'A'+10; 
    else t=20;
  }
  while((!(tm&UNSIGNED)&&(*s=='u'||*s=='U'))||((tm&NQ)==0&&(*s=='l'||*s=='L'))){
#ifdef HAVE_MISRA
/* removed */
#endif
    if(!(tm&UNSIGNED)&&(*s=='u'||*s=='U')){
      if((tm&NQ)==LONG){
	if(!c99||zumleq(value,t_max(UNSIGNED|LONG)))
	  new->ntyp->flags=(UNSIGNED|LONG);
	else
	  new->ntyp->flags=(UNSIGNED|LLONG);
      }else if((tm&NQ)==LLONG)
	new->ntyp->flags=(UNSIGNED|LLONG);
      else if(zumleq(value,t_max(UNSIGNED|INT)))
	new->ntyp->flags=UNSIGNED|INT;
      else if(!c99||zumleq(value,t_max(UNSIGNED|LONG)))
	new->ntyp->flags=UNSIGNED|LONG;
      else
	new->ntyp->flags=UNSIGNED|LLONG;
      tm|=UNSIGNED;
    }else{
      if(c99&&s[1]==s[0]){
	s++;
	if(tm&UNSIGNED)
	  new->ntyp->flags=(UNSIGNED|LLONG);
	else if(base==10||zumleq(value,t_max(LLONG)))
	  new->ntyp->flags=LLONG;
	else
	  new->ntyp->flags=UNSIGNED|LLONG;
	tm|=LLONG;
      }else{
	if(tm&UNSIGNED){
	  if(!c99||zumleq(value,t_max(UNSIGNED|LONG)))
	    new->ntyp->flags=(UNSIGNED|LONG);
	  else
	    new->ntyp->flags=(UNSIGNED|LLONG);
	}else{
	  if(!c99){
	    if(base==10||zumleq(value,t_max(LONG)))
	      new->ntyp->flags=LONG;
	    else
	      new->ntyp->flags=UNSIGNED|LONG;
	  }else{ 
	    if(zumleq(value,t_max(LONG)))
	      new->ntyp->flags=LONG;
	    else if(base!=10&&zumleq(value,t_max(UNSIGNED|LONG)))
	      new->ntyp->flags=UNSIGNED|LONG;
	    else if(base==10||zumleq(value,t_max(LLONG)))
	      new->ntyp->flags=LLONG;
	    else
	      new->ntyp->flags=UNSIGNED|LLONG;
	  }
	}
	tm|=LONG;
      }	
    }
    s++;
  }
  if(*s=='.'||*s=='e'||*s=='E'){
    /*  Fliesskommakonstante, ignoriert vorher berechneten Wert, falls er   */
    /*  nicht dezimal und nicht 0 war (da er dann oktal war)                */
    if(*merk=='0'&&!zumeqto(value,ul2zum(0UL))){
      value=ul2zum(0UL);zbase=ul2zum(10UL);
      while(*merk>='0'&&*merk<='9'){
	digit=ul2zum((unsigned long)(*merk-'0'));
	value=zumadd(zummult(value,zbase),digit);
	merk++;
      }
      if(merk!=s) error(75);
    }
    db=zum2zld(value);
    if(*s=='.'){
      /*  Teil hinter Kommastellen    */
      zldouble zdiv,zbased,zsum,digit;
      int cnt=0;
      s++;
      zbased=d2zld(10.0);zdiv=d2zld(1.0);zsum=d2zld(0.0);
      while(*s>='0'&&*s<='9'){
        if (cnt<35){
	  cnt++;
          digit=d2zld((double)(*s-'0'));
          zsum=zldadd(zldmult(zsum,zbased),digit);
          zdiv=zldmult(zdiv,zbased);
        }
        s++;
      }
      db=zldadd(db,zlddiv(zsum,zdiv));
      
    }
    if(*s=='e'||*s=='E'){
      /*  Exponentialdarstellung  */
      int exp,vorz,i;zldouble zbased;
      zbased=d2zld((double)10);
      s++;
      if(*s=='-'){
	s++;vorz=-1;
      }else{
	vorz=1;if(*s=='+') s++;
      }
      exp=0;
      while(*s>='0'&&*s<='9') exp=exp*10+*s++-'0';
      for(i=0;i<exp;i++){
	if(vorz>0) db=zldmult(db,zbased);
	else   db=zlddiv(db,zbased);
      }
    }
    new->ntyp->flags=DOUBLE;
    if(*s=='f'||*s=='F'){
      new->ntyp->flags=FLOAT;s++;
    }else if(*s=='l'||*s=='L'){
      new->ntyp->flags=LDOUBLE;s++;
    }
  }else{
    if(warned) error(211);
    if(new->ntyp->flags==0){
      if(base==10){
	if(zumleq(value,t_max(INT))) 
	  new->ntyp->flags=INT;
	else if(!c99||zumleq(value,t_max(LONG)))
	  new->ntyp->flags=LONG; 
        else
	  new->ntyp->flags=LLONG;
      }else{
#ifdef HAVE_MISRA
/* removed */
#endif
	if(zumleq(value,t_max(INT)))
	  new->ntyp->flags=INT; 
	else 
	  if(zumleq(value,t_max(UNSIGNED|INT)))
	    new->ntyp->flags=UNSIGNED|INT;
	else if(zumleq(value,t_max(LONG)))
	  new->ntyp->flags=LONG;
	else if(!c99||zumleq(value,t_max(UNSIGNED|LONG))){
	  new->ntyp->flags=UNSIGNED|LONG;error(212);
	}else if(zumleq(value,t_max(LLONG)))
	  new->ntyp->flags=LLONG;
	else
	  new->ntyp->flags=UNSIGNED|LLONG;
      }
    }
  }

  if(*s) error(232);
  
  if(new->ntyp->flags==FLOAT) new->val.vfloat=zld2zf(db);
  else if(new->ntyp->flags==DOUBLE) new->val.vdouble=zld2zd(db);
  else if(new->ntyp->flags==LDOUBLE) new->val.vldouble=db;
  else if(new->ntyp->flags==INT) new->val.vint=zm2zi(zum2zm(value));
  else if(new->ntyp->flags==(UNSIGNED|INT)) new->val.vuint=zum2zui(value);
  else if(new->ntyp->flags==LONG) new->val.vlong=zm2zl(zum2zm(value));
  else if(new->ntyp->flags==(UNSIGNED|LONG)) new->val.vulong=zum2zul(value);
  else if(new->ntyp->flags==LLONG) new->val.vllong=zm2zll(zum2zm(value));
  else if(new->ntyp->flags==(UNSIGNED|LLONG)) new->val.vullong=zum2zull(value);
  else ierror(0);
  next_token();
  return new;
}
np identifier_expression(void)
/*  Erzeugt Identifier mit Knoten  */
{
  np new;char buff[MAXI];
  killsp();
  if(ctok->type==NAME){
    if(is_keyword(ctok->name)) error(216,ctok->name);
#ifdef HAVE_MISRA
/* removed */
#endif
    new=mymalloc(NODES);
    new->flags=IDENTIFIER;
    new->left=new->right=0;
    new->identifier=add_identifier(ctok->name,strlen(ctok->name));
    new->ntyp=0;
    new->sidefx=0;
    new->val.vmax=l2zm(0L);
    new->o.v=0;
    next_token();
  }else{
    error(76);
    new->flags=0;
  }
  return new;
}
void free_alist(struct argument_list *p)
/*  Gibt argument_list inkl. expressions frei  */
{
  struct argument_list *merk;
  while(p){
    merk=p->next;
    if(p->arg) free_expression(p->arg);
    free(p);
    p=merk;
  }
}
void free_expression(np p)
/*  Gibt expression mit allen Typen etc. frei  */
{
  if(!p) return;
  if(p->flags==ASSIGNOP){
    if(!p->right){ierror(0);return;}
    if(p->right->left==p->left)  p->left=0;
    if(p->right->right==p->left) p->left=0;
  }
  if(p->flags==CALL&&p->alist) free_alist(p->alist);
  if(p->ntyp) freetyp(p->ntyp);
  if(p->left) free_expression(p->left);
  if(p->right) free_expression(p->right);
  free(p);
}


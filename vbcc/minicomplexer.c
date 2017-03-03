
FILE *infile;
static int cur_char=' ';

char tkname[MAXLEN];

int yylex()
{
  char *p=tkname;
  int tmp;

  while(isspace(cur_char))
    cur_char=getc(infile);

  if(isdigit(cur_char)){
    do{
      *p++=cur_char;
      cur_char=getc(infile);
    }while(isdigit(cur_char));
    if(cur_char=='.'){
      *p++=cur_char;
      cur_char=getc(infile);
      while(isdigit(cur_char)){
	*p++=cur_char;
	cur_char=getc(infile);
      }
    }
    *p++=0;
    return TKNUMBER;
  }
  if(isalpha(cur_char)){
    do{
      *p++=cur_char;
      cur_char=getc(infile);
    }while(isalnum(cur_char));
    *p++=0;
    if(!strcmp(tkname,"int"))
      return TKINT;
    if(!strcmp(tkname,"real"))
      return TKREAL;
    if(!strcmp(tkname,"if"))
      return TKIF;
    if(!strcmp(tkname,"else"))
      return TKELSE;
    if(!strcmp(tkname,"while"))
      return TKWHILE;
    if(!strcmp(tkname,"return"))
      return TKRETURN;
    return TKIDENTIFIER;
  }
  tmp=cur_char;
  cur_char=getc(infile);
  if(tmp=='!'&&cur_char=='='){
    cur_char=getc(infile);
    return TKNEQ;
  }
  if(tmp=='<'&&cur_char=='='){
    cur_char=getc(infile);
    return TKLEQ;
  }
  if(tmp=='>'&&cur_char=='='){
    cur_char=getc(infile);
    return TKGEQ;
  }
  if(tmp=='&'&&cur_char=='&'){
    cur_char=getc(infile);
    return TKAND;
  }
  if(tmp=='|'&&cur_char=='|'){
    cur_char=getc(infile);
    return TKOR;
  }
  if(tmp==':'&&cur_char=='='){
    cur_char=getc(infile);
    return TKASSIGN;
  }
  return tmp;
}


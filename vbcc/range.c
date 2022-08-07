/*  $VER: vbcc (range.c) $Revision: 1.1 $    */
/*  range analysis and related optimizations */

#include "opt.h"

static char FILE_[]=__FILE__;


#define RANGESIZE (sizeof(range)*(vcount+1))

range *rangebuf;

static void clear_range(range *p,int i)
{
  Var *v;
  int t;

  v=vilist[i];
  t=v->vtyp->flags;
  if(ISINT(t)){
    if(t&UNSIGNED){
      p[i].min.vumax=Z0;
      p[i].max.vumax=tu_max[t&NQ];
    }else{
      p[i].min.vmax=t_min[t&NQ];
      p[i].max.vmax=t_max[t&NQ];
    }
  }
}

void print_ranges(FILE *f,range *p)
{
  int i,t;
  Var *v;
  for(i=0;i<vcount-rcount;i++){
    v=vilist[i];
    t=v->vtyp->flags;
    if(ISINT(t)){
      fprintf(f,"%03d %s (%p): ",i,v->identifier,v);
      if(t&UNSIGNED){
	printzum(f,p[i].min.vumax);
	fprintf(f,"...");
	printzum(f,p[i].max.vumax);
      }else{
	printzm(f,p[i].min.vmax);
	fprintf(f,"...");
	printzm(f,p[i].max.vmax);
      }
      fprintf(f,"\n");
    }
  }
}

static void combine_ranges(range *d,range *n)
{
  int i,t;
  Var *v;
  for(i=0;i<vcount-rcount;i++){
    v=vilist[i];
    t=v->vtyp->flags;
    if(ISINT(t)){
      if(t&UNSIGNED){
	if(zumleq(n[i].min.vumax,d[i].min.vumax))
	  d[i].min.vumax=n[i].min.vumax;
	if(zumleq(d[i].max.vumax,n[i].max.vumax))
	  d[i].max.vumax=n[i].max.vumax;
      }else{
	if(zmleq(n[i].min.vmax,d[i].min.vmax))
	  d[i].min.vmax=n[i].min.vmax;
	if(zmleq(d[i].max.vmax,n[i].max.vmax))
	  d[i].max.vmax=n[i].max.vmax;
      }
    }
  }
}

static int update_ranges(range *d,range *n)
{
  int i,t,change=0;
  Var *v;
  for(i=0;i<vcount-rcount;i++){
    v=vilist[i];
    t=v->vtyp->flags;
    if(ISINT(t)){
      if(t&UNSIGNED){
	if(!zumeqto(n[i].min.vumax,d[i].min.vumax)){
	  d[i].min.vumax=n[i].min.vumax;
	  change=1;
	}
	if(!zumeqto(d[i].max.vumax,n[i].max.vumax)){
	  d[i].max.vumax=n[i].max.vumax;
	  change=1;
	}
      }else{
	if(!zmeqto(n[i].min.vmax,d[i].min.vmax)){
	  d[i].min.vmax=n[i].min.vmax;
	  change=1;
	}
	if(!zmeqto(d[i].max.vmax,n[i].max.vmax)){
	  d[i].max.vmax=n[i].max.vmax;
	  change=1;
	}
      }
    }
  }
  return change;
}

void do_ranges(range *d,IC *p)
{
  int i,j,t,c=p->code;
  if((p->z.flags&(VAR|DREFOBJ))==VAR){
    i=p->z.v->index;
    t=p->z.v->vtyp->flags;
    if(c==ASSIGN){
      if((p->typf&NU)!=(t&NU)){
	clear_range(d,i);
      }else if((p->q1.flags&(KONST|DREFOBJ))==KONST){
	eval_const(&p->q1.val,t);
	if(t&UNSIGNED)
	  d[i].min.vumax=d[i].max.vumax=vumax; 
	else
	  d[i].min.vmax=d[i].max.vmax=vmax; 
      }else if((p->q1.flags&(VAR|DREFOBJ))==VAR&&(p->q1.v->vtyp->flags&NU)==(t&NU)){
	j=p->q1.v->index;
	d[i].min=d[j].min;
	d[i].max=d[j].max;
      }else
	clear_range(d,i);
    }else{
      clear_range(d,i);
    }
  }else{
    j=p->change_cnt;
    for(i=0;i<j;i++){
      if(!(p->change_list[i].flags&DREFOBJ))
	clear_range(d,p->change_list[i].v->index);
    }
  }
}

static void insert_max(range *p,int t)
{
  if(t&UNSIGNED){
    if(!zumleq(p->max.vumax,vumax))
      p->max.vumax=vumax;
    if(!zumleq(p->min.vumax,p->max.vumax))
      p->min.vumax=vumax;
  }else{
    if(!zmleq(p->max.vmax,vmax))
      p->max.vmax=vmax;
    if(!zmleq(p->min.vmax,p->max.vmax))
      p->min.vmax=vmax;
  }
}

static void insert_min(range *p,int t)
{
  if(t&UNSIGNED){
    if(!zumleq(vumax,p->min.vumax))
      p->min.vumax=vumax;
    if(!zumleq(p->min.vumax,p->max.vumax))
      p->max.vumax=vumax;
  }else{
    if(!zmleq(vmax,p->min.vmax))
      p->min.vmax=vmax;
    if(!zmleq(p->min.vmax,p->max.vmax))
      p->max.vmax=vmax;
  }
}

static void inc_val(int t)
{
  if(t&UNSIGNED){
    if(!zumeqto(vumax,tu_max[t&NU]))
      vumax=zumadd(vumax,ZU1);
  }else{
    if(!zmeqto(vmax,t_max[t&NU]))
      vmax=zmadd(vmax,Z1);
  }
}

static void dec_val(int t)
{
  if(t&UNSIGNED){
    if(!zumeqto(vumax,ZU0))
      vumax=zumsub(vumax,ZU1);
  }else{
    if(!zmeqto(vmax,t_min[t&NU]))
      vmax=zmsub(vmax,Z1);
  }
}

void range_analysis(flowgraph *fg)
{
  flowgraph *g;
  range *bp,*erange,*crange;
  Var *v;
  int i,t,done,pass;
  rangebuf=mymalloc(RANGESIZE*(2*basic_blocks+3));
  bp=rangebuf;
  for(i=0;i<vcount-rcount;i++)
    clear_range(bp,i);
  erange=bp;
  bp+=RANGESIZE;
  for(g=fg;g;g=g->normalout){
    g->ra_normalout=bp;
    memcpy(bp,erange,RANGESIZE);
    bp+=RANGESIZE;
    g->ra_branchout=bp;
    memcpy(bp,erange,RANGESIZE);
    bp+=RANGESIZE;
  }
  crange=bp;

  pass=1;
  do{
    if(DEBUG&1024) printf("range analysis pass %d\n",pass);
    done=1;
    for(g=fg;g;g=g->normalout){
      flowlist *lp;
      IC *p;
      int c,first=1;
      memcpy(crange,erange,RANGESIZE);
      for(lp=g->in;lp;lp=lp->next){
	if(lp->graph->normalout==g&&(!lp->graph->end||lp->graph->end->code!=BRA)){
	  if(first){
	    memcpy(crange,lp->graph->ra_normalout,RANGESIZE);
	    first=0;
	  }else
	    combine_ranges(crange,lp->graph->ra_normalout);
	}
	if(lp->graph->branchout==g){
	  if(first) {
	    memcpy(crange,lp->graph->ra_branchout,RANGESIZE);
	    first=0;
	  }else
	    combine_ranges(crange,lp->graph->ra_branchout);
	}
      }
      if(DEBUG&131072){
	printf("block %d, input:\n",g->index);
	print_ranges(stdout,crange);
      }
      for(p=g->start;p;p=p->next){
	do_ranges(crange,p);
	if(p==g->end||p->code==TEST||p->code==COMPARE) break;
      }
      if(p) c=p->code; else c=0;
      if(c==COMPARE||c==TEST){
	if((p->q1.flags&(VAR|DREFOBJ))==VAR&&(c==TEST||(p->q2.flags&(KONST|DREFOBJ))==KONST)){
	  IC *b=p->next;
	  range tmp;
	  while(b&&b->code<BEQ||b->code>BGT) b=b->next;
	  if(!b) ierror(0);
	  if(c==TEST){
	    vmax=Z0;
	    vumax=ZU0;
	  }else
	    eval_const(&p->q2.val,p->typf);
	  i=p->q1.v->index;
	  t=p->q1.v->vtyp->flags;
	  if(b->code==BEQ){
	    if(update_ranges(g->ra_normalout,crange)) done=0;
	    if(t&UNSIGNED)
	      crange[i].min.vumax=crange[i].max.vumax=vumax;
	    else
	      crange[i].min.vmax=crange[i].max.vmax=vmax;
	    if(update_ranges(g->ra_branchout,crange)) done=0;
	  }else if(b->code==BNE){
	    if(update_ranges(g->ra_branchout,crange)) done=0;
	    if(t&UNSIGNED)
	      crange[i].min.vumax=crange[i].min.vumax=vumax;
	    else
	      crange[i].max.vumax=crange[i].max.vumax=vumax;
	    if(update_ranges(g->ra_normalout,crange)) done=0;
	  }else if(b->code==BLT||b->code==BLE){
	    tmp=crange[i];
	    if(b->code==BLT) dec_val(t);
	    insert_max(&crange[i],t);
	    if(update_ranges(g->ra_branchout,crange)) done=0;
	    crange[i]=tmp;
	    inc_val(t);
	    insert_min(&crange[i],t);
	    if(update_ranges(g->ra_normalout,crange)) done=0;
	  }else if(b->code==BGE||b->code==BGT){
	    tmp=crange[i];
	    if(b->code==BGT) inc_val(t);
	    insert_min(&crange[i],t);
	    if(update_ranges(g->ra_branchout,crange)) done=0;
	    crange[i]=tmp;
	    dec_val(t);
	    insert_max(&crange[i],t);
	    if(update_ranges(g->ra_normalout,crange)) done=0;
	  }else
	    ierror(0);
	}else{
	  if(update_ranges(g->ra_branchout,crange)) done=0;
	  if(update_ranges(g->ra_normalout,crange)) done=0;
	}
      }else{
	if(g->branchout){
	  /* must be BRA as we checked for COMPARE/TEST above */
	  if(update_ranges(g->ra_branchout,crange))
	    done=0;
	}else{
	  if(update_ranges(g->ra_normalout,crange))
	    done=0;
	}
      }
      if(DEBUG&131072){
	printf("ra_branchout:\n");
	print_ranges(stdout,g->ra_branchout);
	printf("ra_normalout:\n");
	print_ranges(stdout,g->ra_normalout);
      }
    }
    pass++;
  }while(!done);
  
  
}

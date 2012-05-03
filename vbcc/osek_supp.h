/* used for special operating system support */
extern bvtype task_preempt_regs[],task_schedule_regs[];
typedef struct tasklist {
  struct Var *v;
  int prio;
  int taskid;
  enum {NON_PREEMPTIVE=1,DOES_BLOCK=2,CALLS_SCHED=4,ISR=8} flags;
  bvtype context[BVSIZE(MAXR+1)/sizeof(bvtype)];
  bvtype preempt_context[BVSIZE(MAXR+1)/sizeof(bvtype)];
  bvtype schedule_context[BVSIZE(MAXR+1)/sizeof(bvtype)];
  bvtype unsaved_context[BVSIZE(MAXR+1)/sizeof(bvtype)];
} tasklist;


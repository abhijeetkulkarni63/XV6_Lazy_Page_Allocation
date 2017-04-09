#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "traps.h"

extern int free_frame_cnt; // CS550: for mem proj
int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int globallazyindicator;

int
sys_sbrk(void)
{
  int addr;
  int n;
  uint sz;
  argint(0, &n);
  addr = proc->sz;
  if(globallazyindicator == 1) {
  	if((proc->sz + n) >= 0xfffffff) {
    	cprintf("Allocating pages failed!\n");
        return -1;
    }
  	proc->oldsz = proc->sz;
  	proc->sz = proc->sz + n;
    if(n < 0) {
    	if((sz = deallocuvm(proc->pgdir, proc->oldsz, proc->sz)) == 0)
    	{
      		cprintf("Deallocating pages failed!\n"); // CS550: for mem proj
      		return -1;
    	}
    }
  } else {
  	if(growproc(n) < 0)
    	return -1;
  }
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
  outw(0xB004, 0x0|0x2000);
  return 0;
}

int sys_set_page_allocator(void) {
	int lazyindicator;
	argint(0, &lazyindicator);
	globallazyindicator = lazyindicator;
	return 0;
}

int sys_print_free_frame_cnt(void)
{
    cprintf("free-frames %d\n", free_frame_cnt);
    return 0;
}
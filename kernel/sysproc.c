#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

extern pte_t *walk(pagetable_t, uint64, int);

#ifdef LAB_PGTBL
int sys_pgaccess(void)
{
  uint64 va, ua;
  int pnum; /* pnum-扫描页面数 */
  // get args
  if (argaddr(0, &va) < 0 ||
      argint(1, &pnum) < 0 ||
      argaddr(2, &ua) < 0)
    return -1;
  // 若扫描的页大于PGSIZE*8 返回-1
  if (pnum > 8*PGSIZE)
    return -1;
  // 开辟缓冲区
  char *buf = kalloc();
  // 初始化缓冲区
  memset(buf, 0, PGSIZE);
  //依次扫描页面
  for(int i=0;i<pnum;i++){
    pte_t *p = walk(myproc()->pagetable, va + i*PGSIZE, 0);
    if(*p & PTE_A){
      // 访问过,标记并重置
      buf[i/8] |= 1<<(i%8);
      *p &= ~PTE_A;
    }
  }
  //结果传递给用户空间
  copyout(myproc()->pagetable, ua, buf, pnum);
  //释放页面
  kfree(buf);
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

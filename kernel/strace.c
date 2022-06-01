#include "kernel/types.h"
#include "kernel/defs.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "kernel/mmu.h"
#include "kernel/proc.h"
#include "kernel/syscall.h"
#include "kernel/strace.h"
#include "kernel/format.c"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"

#define MAXTRACE 20
#define NUMSYSCALLS 25

struct event rbuf[MAXTRACE];
struct estats stats[NUMSYSCALLS];

void writetrace(struct event *e) {
  cprintf("TRACE: pid = %d | command name = %s | syscall = %s", e->pid, e->command, e->name);
  if (e->num != SYS_exec && e->num != SYS_exit)
    cprintf(" | return value = %d", e->ret);
  cprintf("\n");
}

char* getname(int num) {
  switch (num) {
    case SYS_fork: return "fork";
    case SYS_exit: return "exit";
    case SYS_wait: return "wait";
    case SYS_pipe: return "pipe";
    case SYS_read: return "read";
    case SYS_kill: return "kill";
    case SYS_exec: return "exec";
    case SYS_fstat: return "fstat";
    case SYS_chdir: return "chdir";
    case SYS_dup: return "dup";
    case SYS_getpid: return "getpid";
    case SYS_sbrk: return "sbrk";
    case SYS_sleep: return "sleep";
    case SYS_uptime: return "uptime";
    case SYS_open: return "open";
    case SYS_write: return "write";
    case SYS_mknod: return "mknod";
    case SYS_unlink: return "unlink";
    case SYS_link: return "link";
    case SYS_mkdir: return "mkdir";
    case SYS_close: return "close";
    case SYS_cstrace: return "cstrace";
    case SYS_pstrace: return "pstrace";
    case SYS_stracedump: return "stracedump";
    case SYS_cstflags: return "cstflags";
  }
  return "";
}

/*
* Ring Buffer Functions
*/

void rbprint() {
  int i;
  struct event *e;
   
  i = 0;
  for (e = rbuf; e < &rbuf[MAXTRACE]; e++) {
    cprintf("RBENTRY %d: pid = %d | command = %s | name = %s", i, e->pid, e->command, e->name);
    if (e->num != SYS_exec && e->num != SYS_exit)
      cprintf(" | return value = %d", e->ret);
    cprintf("\n");
    i++;
  }
}

void rbinit() {
  struct event *e;

  for (e = rbuf; e < &rbuf[MAXTRACE]; e++) {
    e->pid = -1;
    safestrcpy(e->command, """", sizeof(e->command));
    safestrcpy(e->name, """", sizeof(e->name));
    e->num = 0;
    e->ret = 0;
  }
}

void rbpop() {
  struct event *e, *next;
  int i = 0;

  for (e = rbuf; e < &rbuf[MAXTRACE]; e++) {
    next = e+1;
    e->pid = next->pid;
    safestrcpy(e->command, next->command, sizeof(e->command));
    safestrcpy(e->name, next->name, sizeof(e->name));
    e->num = next->num;
    e->ret = next->ret;
    i++;
    if (i == MAXTRACE-1) {
      next->pid = -1;
      safestrcpy(next->command, """", sizeof(next->command));
      safestrcpy(next->name, """", sizeof(next->name));
      next->num = 0;
      next->ret = 0;
      break;
    }
  }
}

struct event *rbappend(struct proc *p, int num) {
  struct event *e;

  // Look for an open spot
  for (e = rbuf; e < &rbuf[MAXTRACE]; e++)
    if (e->pid == -1)
      goto add;
  
  // Buffer is full
  rbpop();
  e = &rbuf[MAXTRACE-1];

add:
  e->pid = p->pid;
  safestrcpy(e->command, p->name, sizeof(e->command));
  safestrcpy(e->name, getname(num), sizeof(e->name));
  e->num = num;
  e->ret = 0;
  return e;
}

int rbdump(struct file *f) {
  struct event *e;
  char chrpid[32];
  char chrret[32];
  
  for (e = rbuf; e < &rbuf[MAXTRACE]; e++) {
    if (e->pid == -1)
      break;
    filewrite(f, "TRACE: pid = \0", strlen("TRACE: pid = \0"));
    integer_to_string(chrpid, 32, e->pid);
    filewrite(f, chrpid, strlen(chrpid));
    filewrite(f, " | command name = ", strlen(" | command name = "));
    filewrite(f, e->command, strlen(e->command));
    filewrite(f, " | syscall = ", strlen(" | syscall = "));
    filewrite(f, e->name, strlen(e->name));
    if (e->num != SYS_exec && e->num != SYS_exit) {
      filewrite(f, " | return value = ", strlen(" | return value = "));
      integer_to_string(chrret, 32, e->num);
      filewrite(f, chrret, strlen(chrret));
    }
    filewrite(f, "\n", strlen("\n"));
  }
  
  return 1;
}

void statsinit(void) {
  int i;
  struct estats *e;

  i = 0;
  for (e = stats; e < &stats[NUMSYSCALLS]; e++) {
    e->num = ++i;
    e->calls = 0;
    e->fails = 0;
  }
}

void updatestats(struct event *e) {
  struct estats *es;

  es = stats+(e->num-1);
  if (e->num == SYS_exec && e->ret == -1) {
    es->fails++;
  } else {
    es->calls++;
    if (e->ret == -1)
      es->fails++;
  }
}

static int padname(char *name) {
  return 13-strlen(name);
}

static int numdigits(int n) {
  int count = 0;  
  while(n!=0) {  
    n=n/10;  
    count++;  
  }
  return count;
}

static int padcalls(int calls) {
  return 7-numdigits(calls);
}

void dumpstats(void) {
  struct estats *es;
  int i;

  if (cst() == 0) {
    cprintf("strace is OFF\n");
    exit();
  }

  // Print Stats
  cprintf("\n syscall      calls  errors     \n");
  cprintf(" -----------  -----  -------\n");
  for (es = stats; es < &stats[NUMSYSCALLS]; es++) {
    if (es->calls > 0) {
      cprintf(" %s", getname(es->num));
      for (i = 0; i < padname(getname(es->num)); i++)
        cprintf(" ");
      cprintf("%d", es->calls);
      for (i = 0; i < padcalls(es->calls); i++)
        cprintf(" ");
      cprintf("%d", es->fails);
      cprintf("\n");
    }
  }
  cprintf("\n");
}

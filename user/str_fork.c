#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(void) {
  int pid, pid2;

  printf(1, "Strace Fork Program\n\n");

  // Turn on strace
  cstrace(1);
  pstrace(1);

  if ((pid = fork()) > 0) { // Parent
    wait();
    printf(1, "Parent\n");
    exit();
  }
  else if ((pid2 = fork()) > 0) { // Child
    wait();
    printf(1, "Child\n");
    exit();
  }
  // Grandchild
  printf(1, "Grandchild\n");

  exit();
}
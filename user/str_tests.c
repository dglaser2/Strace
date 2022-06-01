#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

int main(void) {
  

  //printf(1, "\nSTRACE USER TESTS:\n\n");

  /*
  * Test 1: cstrace and pstrace sys calls
  */
//   printf(1, "Test 1: cstrace and pstrace...\n");
//   printf(1, "  cstrace default value: %d\n  pstrace default value: %d\n", cstrace(-1), pstrace(-1));
//   printf(1, "Turning on cstrace\n");
//   cstrace(1);
//   printf(1, "  cstrace current value: %d\n  pstrace current value: %d\n", cstrace(-1), pstrace(-1));
//   printf(1, "Turning on pstrace\n");
//   pstrace(1);
//   printf(1, "  cstrace current value: %d\n  pstrace current value: %d\n", cstrace(-1), pstrace(-1));
//   printf(1, "Turning off cstrace\n");
//   cstrace(0);
//   printf(1, "  cstrace current value: %d\n  pstrace current value: %d\n", cstrace(-1), pstrace(-1));
//   printf(1, "Turning off pstrace\n");
//   pstrace(0);
//   printf(1, "  cstrace current value: %d\n  pstrace current value: %d\n", cstrace(-1), pstrace(-1));
//   printf(1, "\n");

  /*
  * Test 2: strace dump system call
  */
//   int fd;
//   printf(1, "Test 2: strace dump\n");
//   if ((fd = open("a.txt", O_CREATE)) < 0) {
//     printf(1, "failed to open a.txt");
//     exit();
//   }
//   stracedump(fd);
//   close(fd);
//   printf(1, "\n");

  /*
  * Test 3: flag combos
  */
  open("a.txt", O_RDONLY); // FAIL
  cstrace(-1); // SUCCESS
  cstrace(5); // FAIL
  printf(1, "a"); // SUCCESS

  exit();
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define MAXNODES 5

struct node {
  int key;
};

struct node rbuf[MAXNODES];

void rbprint() {
  int i;
  struct node *n;

  i = 0;
  for (n = rbuf; n < &rbuf[MAXNODES]; n++) {
    printf(1, "Node %d key -> %d\n", i, n->key);
    i++;
  }
}

void rbinit() {
  struct node *n;

  for (n = rbuf; n < &rbuf[MAXNODES]; n++)
    n->key = -1;
}

void rbpop() {
  struct node *n, *next;
  int i = 0;

  for (n = rbuf; n < &rbuf[MAXNODES]; n++) {
    next = n+1;
    n->key = next->key;
    i++;
    if (i == MAXNODES-1) {
      next->key = -1;
      break;
    }
  }
}

void rbappend(int key) {
  struct node *n;

  // Look for an empty spot
  for (n = rbuf; n < &rbuf[MAXNODES]; n++)
    if (n->key == -1)
      goto add;
  
  // Buffer is full
  rbpop();
  n = &rbuf[MAXNODES-1];

add:
  n->key = key;
  return;
}

void rbdump() {
  struct node *n;
  
  for (n = rbuf; n < &rbuf[MAXNODES]; n++) {
    printf(1, "DUMP: Node key -> %d\n", n->key);
    n->key = -1;
  }
}

int main(void) {
  printf(1, "\nRing Buffer Tests\n\n");

  // Init test
  printf(1, "Basic Init Test...\n");
  rbinit();
  rbprint();
  printf(1, "\n");

  // Basic append test
  printf(1, "Basic Append Test...\n");
  rbappend(5); rbappend(4); printf(1, " - Added 5 and 4\n");
  rbprint();
  rbappend(8); rbappend(2); rbappend(6); printf(1, " - Added 8, 6 and 2\n");
  rbprint();
  printf(1, "\n");

  // Pop test
  printf(1, "RB Pop Test...\n");
  rbpop();
  rbprint();
  rbappend(7); printf(1, " - Added 7\n");
  rbprint();
  printf(1, "\n");

  // Append to full buffer test
  printf(1, "Append to Full Buffer Test...\n");
  rbappend(11); printf(1, " - Added 11\n");
  rbprint();
  printf(1, "\n");

  // RB Dump test
  printf(1, "RB Dump Test...\n");
  rbdump();
  printf(1, " - Buffer after dump\n");
  rbprint();
  printf(1, "\n");
  
  exit();
}
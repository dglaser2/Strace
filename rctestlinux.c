#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pid1 = fork();
    if(pid1 < 0) {   // Child 1
        printf("Error forking child 1.\n"); 
    } else if (pid1 == 0) {
        printf("Child 1 Executing\n"); 
    } else {        
        int pid2 = fork();   // Child 2
        if(pid2 < 0) {
            printf("Error forking second child.\n"); 
        } else if(pid2 == 0) {
            printf("Child 2 Executing\n"); 
        } else {    // Parent
            printf("Parent Waiting\n"); 
            wait(&pid1);
            wait(&pid2);
        printf("Children Exited\n"); 
        printf("Parent Executing\n"); 
        printf("Parent Exiting\n");
        } 
    }
    return 0;
}
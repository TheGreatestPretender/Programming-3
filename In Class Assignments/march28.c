#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    pid_t pid;

    pid = fork();

    if (pid < 0){
        perror("fork failed");
        exit(-1);
    }
    else if (pid == 0){
        //first child process for ls -l
        printf("Child process for ls\n");
        execlp("/bin/ls", "ls", "-l", NULL);
    }
    else { //creates second child
        pid = fork();
        if(pid == 0){
            //second child process for cat fileName
            printf("Child process for cat\n");
            execlp("/bin/cat", "cat", "march28.c", NULL);
            exit(1);
        }
        else {
            wait(0);
        }
        wait(0);
        printf("Parent process found children process' completed\n");
    }

    return 0;


}

/*
   create a program (newshell.c) that will create a pipe, fork off a child and exec
   a new program that you will write called (childtype.c). That new program will read
   any data coming in through the pipe and print it to the screen. The parent program
   will allow the user to enter data until a blank line or EOF. Before the parent exits
   it should tell the child it is done so that child an exit as well.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(void){
    int fd[2];
    //int nbytes;
    pid_t childPID;
    char readbuffer[80];
    //char string[];

    //printf("Printed String: %s", readbuffer);
    childPID = fork();

    if (childPID == 0){
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
        //write to stdout from stdin (nbytes)
        dup2(fd[1], 0);
        printf("Received string: %s", readbuffer);
        close(fd[0]);


        printf("\nChild process finished\n");
    }



    return 0;
}

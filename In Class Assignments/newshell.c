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

int main(){
    int fd[2];
    int nbytes;
    pid_t childPID;
    char readbuffer[80];

    strcpy(stdin, readbuffer);

    //create pipe
    pipe(fd);

    //if something goes wrong with forking
    if ((childPID = fork()) == -1){
        perror("Error with fork");
        exit(1);
    }

    //forking child
    childPID = fork();
    if (childPID == 0){
        /*
            if childPID is 0 then we must redirect the output of newshell.c to
            be the input of childtype.c
        */
        nbytes = write(fd[1], readbuffer, sizeof(readbuffer));

        execlp("childtype.c", "childtype.c", NULL);

        close(fd[0]);
        printf("\nParent process finished\n");

    }



    return 0;
}

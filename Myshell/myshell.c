/*
 *  Assignment #3: Unix Shell with redirects and pipeCounters
 *  PID: 4012951
 *  Katherine Schmidt
 *  Program Description: The purpose of this assignment is to learn to
                         develop multi-process programs. You are expected to
                         extend the myshell.c program and add pipeCounterlines and I/O
                         redirections. In particular, your shell program should
                         recognize the following:

                            1. > Redirect stdout from a command to a file. If the
                                 file already exist, it will be erased and overwritten
                                 without warning
                            2. >> Append stdout from a command to a file if the
                                  file exists; if the file does not exist, create
                                  one
                            3. < Redirect the stdin to be from a file, rather
                                 than the keyboard
                            4. | Pass the stdout of one command to another for
                                 further processing
 */

 #include <fcntl.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/wait.h>
 #include <unistd.h>

 #define MAX_ARGS 20
 #define BUFFSIZE 1024

 char* args[MAX_ARGS];
 char* argstmp[MAX_ARGS];
 char* argsFinal[MAX_ARGS];
 int outFlag = 0, inFlag = 0, appendFlag = 0;
 char *inFile = NULL, *outFile = NULL;

 //method declaration
 int argsCpy(char **args, char **argstmp, char* symb, int mode);
 void execute(char* cmdline);
 int files(char **args);
 int get_args(char* cmdline, char* args[]);
 int process(int in, int out, char **inData, char **outData);
 int pipeCount();
 void reset();
 void swap(char ** argsSwap, char **argstmpSwap);


/*method implementation*/

 int argsCpy (char **args, char ** argstmp, char *symbol, int mode) {
    int i = 0;
    int j = 0;

    while (args[i] != NULL) {
       if (mode == 0) {
          if (!strcmp (args[i], symbol)) {
             args[i] = NULL;
             i++;
             while (args[i] != NULL) {
                argstmp[j] = args[i];
                args[i] = NULL;
                i++;
                j++;
             }
             i--;
          }
          i++;
       }
       //checking for any of the recognizable symbols
       if (mode == 1) {
          if (!strcmp(args[i], ">") || !strcmp(args[i], ">>") || !strcmp(args[i], "<")) {
             args[i] = NULL;
             i++;
             while (args[i] != NULL) {
                argstmp[j] = args[i];
                args[i] = NULL;
                i++;
                j++;
             }
             i--;
          }
          i++;
       }
    }
    while (j < MAX_ARGS) {
       argstmp[j] = NULL;
       j++;
    }
    return 0;
 }



 void execute (char* cmdline) {
    int numArgs = get_args (cmdline, args);
    int numPipes = pipeCount();
    //(1) = stdin
    int input = dup(1);
    //(0) = stdout
    int output = dup(0);

    if(numArgs <= 0) return;

    if (!strcmp(args[0], "quit") || !strcmp(args[0], "exit")) exit(0);

    if (numPipes == 0) {
       int pid;
       pid = process(input, output, args, argsFinal);
       waitpid(pid, NULL,0);
       reset();
    }

    if(numPipes > 0) {
       int pipeCounter = 0;
       int fd[2];
       int inCounter = 0;
       int pid;
       while (pipeCounter < numPipes) {
          argsCpy (args, argstmp, "|", 0);
          pipe(fd);
          pid = process (inCounter, fd[1], args, argsFinal);
          waitpid (pid, NULL, 0);
          close(fd[1]);
          swap(args, argstmp);
          inCounter = fd[0];
          pipeCounter++;
       }

       if(inCounter != 0)
          dup2(inCounter, 0);

       pid = process(inCounter, output, args, argsFinal);
       //block until all processes are finished
       waitpid(pid, NULL, 0);
       }
    reset();
    dup2(input, 0);
    dup2(output, 1);
 }

 int files(char ** args) {
    int i = 0;
    while (args[i] != NULL) {
        if (!strcmp(args[i], ">")) {
           outFile = args[i+1];
           outFlag = 1;
        }
        if (!strcmp(args[i], ">>")) {
           outFile = args[i + 1];
           appendFlag = 1;
        }
        if(!strcmp(args[i], "<")) {
           inFile = args[i + 1];
           inFlag = 1;
        }
        i++;
    }
    return 0;
 }

 int get_args (char* cmdline, char* args[]) {
    int i = 0;
    if ((args[0] = strtok(cmdline, "\n\t ")) == NULL) return 0;

    while ((args[++i] = strtok(NULL, "\n\t ")) != NULL) {
       if (i >= MAX_ARGS) {
          printf ("Too many arguments\n");
          exit(1);
       }
    }
    return i;
 }


 int process(int inPipe, int outPipe, char **inData, char **outData) {
    int pid;
    if ((pid = fork()) == 0) {
       //read if input is not stdin
       if (inPipe != 0){
          dup2 (inPipe, 0);
          close (inPipe);
       }
       //write if output is not stdout
       if (outPipe != 1){
          dup2 (outPipe, 1);
          close (outPipe);
       }
       int err = files(inData);
       //redirection of in and out
       int out;
       int in;

       if (err != 0) exit(1);

       if (inFlag == 1 && (outFlag == 1 || appendFlag == 1)) argsCpy (inData, outData , "", 1);

       else if (inFlag == 1 && outFlag == 0 && appendFlag == 0) argsCpy (inData, outData,"<", 0);

       else if ((outFlag == 1 || appendFlag == 1) && inFlag == 0) {
          if (outFlag == 1) argsCpy(inData, outData, ">", 0);

          if(appendFlag == 1) argsCpy (inData, outData,">>", 0);
       }
       if (outFlag == 1) {
          out = open (outFile, O_CREAT | O_RDWR | O_TRUNC, 0644);
          if (out < 0){
             perror ("opening file");
             exit(1);
          }
       }
       else if (appendFlag == 1) {
          out = open (outFile, O_CREAT | O_RDWR | O_APPEND, 0644);
          if (out < 0) {
             perror ("opening file");
             exit(1);
          }
       }
       if (inFlag == 1) {
          in = open (inFile, O_RDONLY, 0644);
          if (in < 0) {
             perror ("opening file");
             exit(1);
          }
       }
       if (outFlag == 1 || appendFlag == 1) {
          if (dup2(out, fileno(stdout)) < 0) {
             perror ("dup2");
             exit(1);
          }
          close (out);
       }
       if (inFlag == 1){
          if (dup2(in, fileno(stdin)) < 0){
              perror ("dup2");
              exit(1);
          }
          close (in);
       }
       execvp (inData[0], inData);
       perror ("exec failed");
       exit (-1);
    }
    if (pid < 0) perror("fork failed");

    close(inPipe);
    close(outPipe);
    return pid;
 }



 int pipeCount() {
    int i = 0;
    int pipeCounterNum = 0;
    while(args[i] != NULL) {
       if(!strcmp(args[i], "|"))
       pipeCounterNum++;
       i++;
    }
    return pipeCounterNum;
 }

 void reset() {
    *args = NULL;
    *argstmp = NULL;
    *argsFinal =  NULL;
    outFlag = 0;
    appendFlag = 0;
    inFlag = 0;
    inFile = NULL;
    outFile = NULL;
 }



 void swap(char **argsSwap, char **argstmpSwap) {
    char *temp[MAX_ARGS];
    int i = 0;
    while (i < MAX_ARGS) {
        temp[i] = argsSwap[i];
        i++;
    }

    i = 0;
    while (i < MAX_ARGS) {
        argsSwap[i] = argstmpSwap[i];
        i++;
    }

    i = 0;
    while (i < MAX_ARGS) {
         argstmpSwap[i] = temp[i];
         i++;
    }
 }

 int main(int argc, char* argv []) {
    char cmdline[BUFFSIZE];
    for(;;) {
       printf("COP4338$ ");
       if (fgets(cmdline, BUFFSIZE, stdin) == NULL) {
          perror("fgets failed");
          exit (1);
       }
       execute(cmdline);
    }
   return 0;
 }

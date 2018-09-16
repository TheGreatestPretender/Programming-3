

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include "bstsort.h"

int cflag = 0, oflag = 0;
FILE *ofile, *ifile;

int main(int argc, char **argv) {
   extern char *optarg;
   extern int optind;
   char *token;
   char *str;
   char buffer[100];
   int c;
   static char usage[] = "Usage: bstsort [-c] [-o output_file_name] [input_file_name] *\n";
   Node *root = NULL;

   //getopt code, given to us. Just modified to fit program description
   while((c = getopt(argc, argv, "co:")) != -1) {
      switch(c) {
         case 'c':   //user used casewise comparaison option
            cflag = 1;
            break;
         case 'o':   //user used write to output file option
            oflag = 1;
            ofile = fopen(optarg, "w");
            break;
         case '?':
            fprintf(stderr, usage, argv[0]);
            exit(1);
            break;
      }
   }

   if(argv[optind] != NULL) {   //if user gave input file only
      if(ifile = fopen(argv[optind], "r")) {
         while(fgets(buffer, sizeof(buffer), ifile)) {   //read input into buffer
            if(buffer[0] != '\n') {
                token = strtok(buffer, "\n");   //takes each line, and saves them as a token
                str = (char*)malloc(strlen(token));   //creating space in mem
                strcpy(str, token);
                insert(&root, str);
            }
         }
         inOrder(root);
      }
      else {
         printf("File not found!\n");
         exit(1);
      }
      //always make sure to close file so we don't have memory leak
      fclose(ifile);
   }

   //reading input from user
   else {
      printf("\n\nEnter different strings separated by the enter key\n");
      printf("Press the enter key twice to end execution\n");
      //reading from command line
      ifile = stdin;

      while(fgets(buffer, sizeof(buffer), ifile) && buffer[0] != '\n') {
         if(buffer[0] != EOF) {//if input isn't end of file
            token = strtok(buffer, "\n");//take each line, and save each as a token
            str = (char*)malloc(strlen(token) * sizeof(char)); //creating space in mem
            strcpy(str, token);
            insert(&root, str);
         }
      }
      inOrder(root);
      //if user used the output file option
      if(oflag) fclose(ofile);
   }
   reclaimMem(root);
   return 0;
}


//Function syntax: insert(**p, *data)
//If the **p position of the tree is empty assign a new node to it
//Else, recursively moves through and inserts into corresponding node
void insert(Node **p, char *data) {
   Node *q = NULL;
   int cmp;

   if(*p == NULL) {//if node is empty, we access members of the Node structure
      q = (Node *)malloc(sizeof(Node));
      q->leftChild = NULL;
      q->rightChild = NULL;
      q->data = data;
      (*p) = q;
      (*p)->count++;
      return;
   }

   //When user uses c option, we compare all the tokens in a casewise manner
   //if they don't, we just compare as they are
   if(cflag){
       cmp = compareCasewise(data, (*p)->data);
   }
   else{
       cmp = compare(data, (*p)->data);
   }
   //go left
   if(cmp < 0) insert(&(*p)-> leftChild, data);
   //go right
   else {
      if(cmp == 0)   //traverse right
         ((*p)->count)++;
      insert(&(*p)->rightChild, data);
   }
}


//Function Syntax: inOrder(*p)
//recursively prints out in order BST whether user has an input file or not
void inOrder(Node *p) {
   if(p == NULL) return;

   //go left
   inOrder(p->leftChild);

   //o option to print to file
   if(oflag) fprintf(ofile, "\n %s \n", p->data);
   //printing to command line
   else fprintf(stdout, "\n %s \n", p->data);


   inOrder(p->rightChild);   //go right
}


//Function syntax: cmp(*str1, *str2)
//Lexographically compares two strings in which it is NOT case sensitive
int compare(char *str1, char *str2) {
   int cmp;
   for(;; str1++, str2++) {
      cmp = tolower(*str1) - tolower(*str2);
      if(cmp != 0 || !*str1) return cmp;
   }
}


//Function syntax: cmpCW(*str1, *str2)
//Lexographically compares two strings in which it IS case sensitive
int compareCasewise(char *str1, char *str2) {
   int i;
   for(i = 0; str1[i] == str2[i]; i++)
      if(str1[i] == '\0') return 0;
   return str1[i] - str2[i];
}


//Function Syntax: reclaimMem(*p)
//recursively reclaims the malloc
void reclaimMem(Node *p) {
   if(p == NULL) return;

   reclaimMem(p->leftChild);
   reclaimMem(p->rightChild);

   //empty memory of nodes data
   free(p->data);
   //empty memory of the node
   free(p);
}

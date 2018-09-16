#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char **argv){
    static char usage[] = "writer [-i infile] [-f outfile]\n";
    static char name[] = "\nKatherine Schmidt\n";
    extern char *optarg;
    int c, err;
    int iflag = 0, fflag = 0;

    FILE* outFile=stdout;
    //FILE* oFile = stdout;

    while ((c = getopt(argc, argv, "i:f:")) != -1){
        switch(c){
            case 'i':
                iflag = 1;
		break;
            case 'f':
                fflag = 1;
                outFile = fopen(optarg, "w");//creates file
                break;
            case '?':
                err = 1;
                break;
        }
    }//end while for getopt parse

    //if (err) {
      //  fprintf(stderr, usage, argv[0]);
       // exit(1);
   // }//end if for incorrect usage
    
    if (fflag == 0){
        printf("\nKatherine Schmidt\n");
    }
    else {
        fprintf(outFile, "\nKatherine Schmidt\n");
        fclose(outFile);
    }

    return 0;

}



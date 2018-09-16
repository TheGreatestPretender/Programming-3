
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "bmplib.h"
#include <getopt.h>
#include <unistd.h>

/*
 * This method enlarges a 24-bit, uncompressed .bmp file
 * that has been read in using readFile()
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the original number of rows
 * cols     - the original number of columns
 *
 * scale    - the multiplier applied to EACH OF the rows and columns, e.g.
 *           if scale=2, then 2* rows and 2*cols
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows (scale*rows)
 * newcols  - the new number of cols (scale*cols)
 */


int enlarge(PIXEL* original, int rows, int cols, int scale,
	PIXEL** new, int* newrows, int* newcols){
		PIXEL* ogPixel;
		PIXEL* newPixel;
		*newrows = scale * rows;
		*newcols = scale * cols;

		//malloc for the new pixel array we will fill
		*new = (PIXEL*)malloc((*newcols) * (*newrows) * sizeof(PIXEL));

		//start to go through each pixel
		int countCol, countRow, countScale;

		for (countRow = 0; countRow < *newrows; countRow++){
			ogPixel = original + (countRow/scale) * cols;
			newPixel = (*new) + countRow * (*newcols);

			for (countCol = 0; countCol < cols; countCol++){
				for (countScale = 0; countScale < scale; countScale++){

					*newPixel++ = *ogPixel;
				}
				ogPixel++;
			}
		}
		return 0;
	}

/*
 * This method rotates a 24-bit, uncompressed .bmp file that has been read
 * in using readFile(). The rotation is expressed in degrees and can be
 * positive, negative, or 0 -- but it must be a multiple of 90 degrees
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 * rotation - a positive or negative rotation,
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows
 * newcols  - the new number of cols
 */
int rotate(PIXEL* original, int rows, int cols, int rotation,
	PIXEL** new, int* newrows, int* newcols){
		//malloc for the new pixel array we will fill
		*new = (PIXEL*)malloc((*newcols) * (*newrows) * sizeof(PIXEL));

	    PIXEL* newPixel;
		//think of rows as x axis and cols as y axis
		int quadNum = rotation / 90 % 4;

			/*
			    if we want to rotate either to: 90 degrees or 270 degrees
				1 quad = -3 quad
				3 quad = -1 quad
				since we can safely say that if we flip from 90 degrees to 270 degrees
				it is the same as doing 3 "backwards" rotations. Proof via trigonometry
			*/
			if (quadNum == 1 || quadNum == -1 || quadNum == 3 || quadNum == -3){
				*newcols = rows;
				*newrows = cols;
			}

			else if (quadNum == 2 || quadNum == 0 || quadNum == -2){
				*newcols = cols;
				*newrows = rows;
			}

			//start to go through each pixel
			int countCol, countRow;
			for(countRow = 0; countRow < rows; countRow++){
				for(countCol = 0; countCol < cols; countCol++){
					PIXEL* ogPixel = original + countRow*cols+ countCol;

					if(quadNum == 1 || quadNum == -3) newPixel = (*new) + (rows * (cols -1 -countCol))+ countRow;
					else if (quadNum == 2 || quadNum == -2) newPixel = (*new) + ((rows - 1 - countRow) * cols) + (cols - 1 - countCol);
					else if (quadNum == 3 || quadNum == -1) newPixel = (*new) + (rows * countCol) + (rows -1 - countRow);
					else if (quadNum == 0 ) newPixel = (*new)+ countRow*cols+ countCol;


					*newPixel = *ogPixel;
				}

			}

		return 0;
	}

/*
 * This method horizontally flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile().
 *
 * THIS IS GIVEN TO YOU SOLELY TO LOOK AT AS AN EXAMPLE
 * TRY TO UNDERSTAND HOW IT WORKS
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int flip (PIXEL *original, PIXEL **new, int rows, int cols)
{
  int row, col;

  if ((rows <= 0) || (cols <= 0)) return -1;

  *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  for (row=0; row < rows; row++)
    for (col=0; col < cols; col++) {
      PIXEL* o = original + row*cols + col;
      PIXEL* n = (*new) + row*cols + (cols-1-col);
      *n = *o;
    }

  return 0;
}


int main(int argc, char** argv) {
	extern char *optarg;
	extern int optind;
	int col, row, scale, rotation, c, sFlag = 0, rFlag = 0, fFlag = 0, oFlag = 0;
	static char usage[] = "Usage: %s [-s scale | -r degree | -f] [-o output_file] [input_file]\n";
	char * outFile = NULL;
	char * inFile = NULL;
	int* newCol = &col;
	int* newRow = &row;
	PIXEL* bitMap; //og bitmap
	PIXEL* newBitMap;

	//getopt code given in class, just modified to fit program options
	while ((c = getopt(argc, argv, "s:r:o:f")) != -1)
	{
		switch (c){
			case 's':
				sFlag = 1;
				//scale factor changing from ascii to integer
				scale = atoi(optarg);
				break;
			case 'r':
				rFlag = 1;
				//rotation factor changing from ascii to integer
				rotation = atoi(optarg);
				break;
			case 'f': //only one flip per f option
				fFlag = 1;
				break;
			case 'o':
				outFile = optarg;
				oFlag = 1;
				break;
			case '?':
				fprintf(stderr, usage, argv[0]);
				exit(1);
				break;
		}
		if (optind < argc) inFile = argv[optind];
	}

/*
Logic behind piping: since the pipe (|) command is an OS operation, we do not need
					to "create" it. Instead we must fork each option we have,
					(-s, -r, -f, -o) into separate child processes where the previous
					option output will be seen as input for the next option.
					Example: bmptool –f example.bmp | bmptool –f –o 9.bmp
						bmptool executes and flips example.bmp then sets the result
						as input for the next command. Such that, if we were to just
						execute with no pipe it would be ./bmptool -f -o 9.bmp example.bmp
						where example.bmp is the flipped version
*/

//variables for processing
int fd[2];
pid_t pidScale;
pid_t pidRotate;
pid_t pidFlip;
pid_t pidOutput;

/* DIDN'T GET TO FINISH PIPING*/

	//read file, enlarge it, then write the final product to outFile
	if (sFlag){
		readFile(inFile, &row, &col, &bitMap);

		enlarge(bitMap, row, col, scale, &newBitMap, newRow, newCol);
		writeFile(outFile, *newRow, *newCol, newBitMap);
	}


	if (rFlag){
		//if we had sFlag before read from outFile
		if (sFlag) readFile(outFile, &row, &col, &bitMap);

		//if no flag was present just read from inFile
		else readFile(inFile, &row, &col, &bitMap);

		//always rotate and write to outFile
		rotate(bitMap, row, col, rotation, &newBitMap, newRow, newCol);
		writeFile(outFile, *newRow, *newCol, newBitMap);
	}


	if (fFlag){
		//if we had sFlag or rFlag before read from outFile
		if (sFlag || rFlag) readFile(outFile, &row, &col, &bitMap);

		//if no flag was present just read from inFile
		else readFile(inFile, &row, &col, &bitMap);

		//always rotate and write to outFile
		flip(bitMap, &newBitMap, row, col);
		writeFile(outFile, *newRow, *newCol, newBitMap);
	}

  	//free(bitMap);
  	//free(newBitMap);
  	return 0;
}

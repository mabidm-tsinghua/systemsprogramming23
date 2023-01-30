// cpC.cpp : Defines the entry point for the console application.
// Sequential File Copy

#include "stdafx.h"


/* Chapter 1. Basic cp file copy program. C library Implementation. */
/* cpC file1 file2: Copy file1 to file2. */



#define BUF_SIZE 256
#define ctrl_z 26
int main (int argc, char *argv [])
{
	FILE *inFile, *outFile;
	char rec[BUF_SIZE]; //={'a','b','c','\n',ctrl_z,0,1,2,3};
	size_t bytesIn, bytesOut;
	if (argc != 3) {
		fprintf (stderr, "Usage: cp file1 file2\n");
		return 1;
	}

	/* In later chapters, we'll use the more secure functions, such as fopen_s
	 * See http://msdn.microsoft.com/en-us/library/8ef0s5kh%28VS.80%29.aspx 
	 * Note that this project defines the macro _CRT_SECURE_NO_WARNINGS to avoid a warning */

	inFile = fopen (argv[1], "rb");////binary mode; w/o b defaault is char mode
	if (inFile == NULL) {
		perror (argv[1]);//interprets the value of errno and print as argv[1]: string rep of errno
		return 2;
	}
	outFile = fopen (argv[2], "wb");//binary mode; w/o b defaault is char mode
	if (outFile == NULL) {
		perror (argv[2]);
		fclose(inFile);
		return 3;
	}
	fgets(rec,BUFSIZ,inFile);

    fwrite (rec, 1, strlen(rec), outFile);
	/*char r[5];
	fread (r, 1, 5, inFile);
    printf("%s",r);
	*/

	/* Process the input file a record at a time. */
	/*
	while ((bytesIn = fread (rec, 1, BUF_SIZE, inFile)) > 0) { //read BUF_SIZE elements, each with size of 1; ret total elements read
		bytesOut = fwrite (rec, 1, bytesIn, outFile);
		if (bytesOut != bytesIn) {
			perror ("Fatal write error.");
			fclose(inFile); 
			fclose(outFile);
			return 4;
		}
	}
	
	*/

	fclose (inFile);
	fclose (outFile);
	
	return 0;
}


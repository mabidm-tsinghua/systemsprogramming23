// cpCF.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BUF_SIZE 256
int _tmain (int argc, LPTSTR argv []) // Notice: _tmain
{
	
	if (argc != 3) {
		fprintf (stderr, "Usage: cp file1 file2\n");
		return 1;
	}
	if (!CopyFile (argv[1], argv[2], FALSE)) {
		fprintf (stderr, "CopyFile Error: %x\n", GetLastError ());
		return 2;
	}
	
	return 0;
	
	




}


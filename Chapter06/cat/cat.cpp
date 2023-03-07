/* Chapter 6. catHA. */
/* cat file */

/* This program illustrates:
	1.	ReportError
	2.	Win32 CreateFile, ReadFile, WriteFile, CloseHandle
	3.	Loop logic to process a sequential file
	4.	Use of handle passed as a command line argument */


#include "stdafx.h"


#define BUF_SIZE 0x200

static VOID CatFile (HANDLE, HANDLE);
int _tmain (int argc, LPTSTR argv [])
{
	HANDLE hInFile, hOut;

	_tprintf (_T("In cat. %s \n"), argv [1]);

	hInFile = CreateFile (argv [1], GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile == INVALID_HANDLE_VALUE)
			ReportError (_T("\nCat Error: File does not exist"), 0, TRUE);

	/* The output handle is obtained from the command line. */

	hOut = GetStdHandle (STD_OUTPUT_HANDLE);

	if (hInFile != INVALID_HANDLE_VALUE)
		CatFile (hInFile, hOut);
	
	CloseHandle (hInFile);
	
	return 0;
}

static VOID CatFile (HANDLE hInFile, HANDLE hOutFile)
{
	DWORD nIn, nOut;
	BYTE Buffer [BUF_SIZE];

	
	while (ReadFile (hInFile, Buffer, BUF_SIZE, &nIn, NULL)
			&& (nIn != 0)
			&& WriteFile (hOutFile, Buffer, nIn, &nOut, NULL)) ;
	
	 //testing code
	/*
	TCHAR c;
	_tprintf(_T("cat: Enter any key to exit"));
	_tscanf(_T("%c"),&c);
	*/
	

	return;
}

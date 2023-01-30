// cpW.cpp : Sequential file copy using windows API.
//
/* Chapter 1. Basic cp file copy program. Win32 Implementation. */
/* cpW file1 file2: Copy file1 to file2. */

#include "stdafx.h"

#define BUF_SIZE 16384  /* Optimal in several experiments. Small values such as 256 give very bad performance */

int _tmain (int argc, TCHAR *argv []) // Notice: int _tmain (int argc, LPTSTR argv [])
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer [BUF_SIZE];
	const int i =10;
	const int *const pi = &i;
	const int *const *ppi = &pi;
	//*pi = 123;

	if (argc != 3) {
		fprintf (stderr, "Usage: cp file1 file2\n");
		return 1;//
	}
	//FUTUR file objects cannot conflict with FILE_SHARE_READ. this means that future access modes cannot specify GENERIC_WRITE and FILE_SHARE_WRITE (ALONE)
	hIn = CreateFile (argv[1], GENERIC_READ,FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Cannot open input file. Error: %x\n", GetLastError ());
		return 2;
	}

	hOut = CreateFile (argv[2] , GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOut == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Cannot open output file. Error: 0X%x\n", GetLastError ());
		CloseHandle(hIn);
		return 3;
	}
	while (ReadFile (hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
		WriteFile (hOut, buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			fprintf (stderr, "Fatal write error: 0X%x\n", GetLastError ());
			CloseHandle(hIn); CloseHandle(hOut);
			return 4;
		}
	}
	
	/*
	HANDLE hIn2 = CreateFile (argv[1], GENERIC_READ,FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn2 == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Cannot open SECOND . Error: %x\n", GetLastError ());
		return 2;
	}
	//
	
		HANDLE hIn3 = CreateFile (argv[1], GENERIC_READ,FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn3 == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Cannot open THird . Error: %x\n", GetLastError ());
		return 2;
	}
	*/

	CloseHandle (hIn); //invalidates the hIn, decrement the object count, check if object can be destroyed
	CloseHandle (hOut);

	return 0;
}



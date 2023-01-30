// cci.cpp : Defines the entry point for the console application.
//
/* Chapter 2. cci Version 1. Modified Caesar cipher (http://en.wikipedia.org/wiki/Caesar_cipher). */
/* Main program, which can be linked to different implementations */
/* of the cci_f function. */

/* cci shift file1 file2
 *		shift is the integer added mod 256 to each byte.
 *	Otherwise, this program is like cp and cpCF but there is no direct UNIX equivalent. */

/* This program illustrates:
 *		1. File processing with converstion.
 *		2. Boilerplate code to process the command line.
 */
#include "stdafx.h"


BOOL cci_f (LPCTSTR, LPCTSTR, DWORD);
VOID ReportError (LPCTSTR userMessage, DWORD exitCode, BOOL printErrorMessage);

int _tmain (int argc, LPTSTR argv [])
{
	if (argc != 4)
		ReportError  (_T ("Usage: cci shift file1 file2"), 1, FALSE);
	
	if (!cci_f (argv [2], argv [3], _ttoi(argv[1]))) //atoi
		ReportError (_T ("Encryption failed."), 4, TRUE);

	return 0;
}

#define BUF_SIZE 65536	  /* Generally, you will get better performance with larger buffers (use powers of 2). 
					      /* 65536 worked well; larger numbers did not help in some simple tests. */

BOOL cci_f (LPCTSTR fIn, LPCTSTR fOut, DWORD shift)

/* Caesar cipher function  - Simple implementation
 *		fIn:		Source file pathname
 *		fOut:		Destination file pathname
 *		shift:		Numerical shift
 *	Behavior is modeled after CopyFile */
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut, iCopy;
	BYTE buffer [BUF_SIZE], bShift = (BYTE)shift;
	BOOL writeOK = TRUE;

	hIn = CreateFile (fIn, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) return FALSE;

	hOut = CreateFile (fOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOut == INVALID_HANDLE_VALUE) {
		CloseHandle(hIn);
		return FALSE;
	}

	while (writeOK && ReadFile (hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
		
		/* Mar 24, 2010. The book, and an earlier version of this program, used separate input and output buffers, which is not necessary
		   and which hurts performance slightly due to cache behavior (thanks to Melvin Smith for pointing this out. HOWEVER, some read-modify-write
		   applications would need two buffers. For example, you might be expanding the file in some way. Also, the App C results are still
		   useful as the compare different implementations and different platforms. */
		for (iCopy = 0; iCopy < nIn; iCopy++){
			
			buffer[iCopy] = (buffer[iCopy] + bShift) % 256; //to decrypt change + to -
		}
		writeOK = WriteFile (hOut, buffer, nIn, &nOut, NULL);
	}

	CloseHandle (hIn);
	CloseHandle (hOut);

	return writeOK;
}

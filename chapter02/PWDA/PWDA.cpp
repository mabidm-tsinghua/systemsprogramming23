// PWDA.cpp : Defines the entry point for the console application.
//

/* Chapter 2. pwda. */
/* pwd: Similar to the UNIX command. */
/* This program illustrates:
		1. Win32 GetCurrentDirectory
		2. Testing the length of a returned string
		3. A Version - allocate enough memory for the path */

#include "stdafx.h"
#include <stdlib.h>



VOID ReportError (LPCTSTR userMessage, DWORD exitCode, BOOL printErrorMessage);

int _tmain (int argc, LPTSTR argv [])
{
	/* Buffer to receive current directory allows
		for longest possible path. */

	//DWORD LenCurDir = GetCurrentDirectory (0, NULL);//size of buffer in characters including null char
	/* Return length allows for terminating null character
		 */
	TCHAR pwdBuffer[MAX_PATH]; // = (LPTSTR)malloc(LenCurDir * sizeof(TCHAR));

	//if (pwdBuffer == NULL)
	//	ReportError (_T ("Failure allocating pathname buffer."), 1, TRUE);

	//LenCurDir = GetCurrentDirectory (MAX_PATH, pwdBuffer);
	
	GetCurrentDirectory(MAX_PATH, pwdBuffer);//depends on how you run the program

	_ftprintf (stderr, _T("%s\n"), pwdBuffer);

	SetCurrentDirectory(TEXT("D:\\github_rep\\SystemsProgramming\\chapter02\\Debug"));
	/*When full pathnames are not supplied, the various Windows functions look for 
	files and directories in the current directory of the current drive. 
	For example, if a thread in a process calls CreateFile to open a file 
	(without specifying a full pathname), the system looks for the file in 
	the current drive and directory.
	The system keeps track of a process' current drive and directory internally. 
	Because this information is maintained on a per-process basis, a thread in the 
	process that changes the current drive or directory changes this information for 
	all the threads in the process.*/
	HANDLE hInFile = CreateFile(TEXT("d:abc.txt"), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile == INVALID_HANDLE_VALUE) {
		 ReportError(_T("\nCat Error1: File does not exist."), 0, FALSE);
	}
	/*
	The system keeps track of the process' current drive and directory, 
	but it does not keep track of the current directory for every drive. 
	However, there is some operating system support for handling current 
	directories for multiple drives. This support is offered via the process' 
	environment strings. 
	*/
	SetEnvironmentVariable(TEXT("=c:"),TEXT("c:\\working_directory"));//=c:=c:\\working_directory

	hInFile = CreateFile(TEXT("c:abc.txt"), GENERIC_READ, //search abc.txt in c:\\working_directory as env variable exist
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile == INVALID_HANDLE_VALUE) {
		ReportError(_T("\nCat Error2: File does not exist."), 0, TRUE);
	}
	GetCurrentDirectory(MAX_PATH, pwdBuffer);

	_ftprintf(stderr, _T("%s\n"), pwdBuffer);

	//_tchdir(TEXT("c:\\working_directory")); //change currrent directory and also set =c:=c:\\working_directory in process environemnt string

	return 0;
}



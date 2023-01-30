/* Chapter 3. tail command. */
/* tail file - Print the last 10 lines of the named file.
	All options are ignored. The file must be specified. */

	/* This program illustrates:
		1. Getting the file size.
		2. Setting the file pointer.
		3. LARGE_INTEGER arithmetic and using the 64-bit file positions.
		4. Getting the current file position by moving
			0 bytes from the current position. */

			/*  UPDATES. APRIL 12, 2010.
			 *  1. The program uses GetFileSizeEx and SetFilePointerEx, which are much easier to use than the non-Ex versions and
				   are supported in all the target Windows systems.
			 *  2. There are some corrections, suggested by Kwan Ting Chan (KTC) to account for the line size and UNICODE usage.
			 *
			 *  LIMITATIONS: Lines shouldn't be longer than 256 characters
			 *  .
			 */
#include "stdafx.h"
#include <iostream>

#define NUM_LINES 11 /* One more than number of lines in the tail. */

#define MAX_LINE_SIZE 256
#define MAX_CHAR NUM_LINES*(MAX_LINE_SIZE+1)

int _tmain(int argc, LPTSTR argv[])
{
	char str[] = "- This, a sample string.";
	char* pch;
	printf("Splitting string \"%s\" into tokens:\n", str);
	pch = strtok(str, " ,.-");
	while (pch != NULL)
	{
		printf("%s\n", pch);
		pch = strtok(NULL, " ,.-");
	}
	printf("%s\n", str);
	std::cout << argc;
	return 0;
	/*
	HANDLE hInFile;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	LARGE_INTEGER FileSize, CurPtr, FPos;
	LARGE_INTEGER LinePos[NUM_LINES];
	DWORD LastLine, FirstLine, LineCount, nRead;
	TCHAR buffer[MAX_CHAR + 1], c;
	TCHAR b[1024] = TEXT("This is Systems Programming Class.................");

	if (argc != 2)
		ReportError(_T("Usage: tail file"), 1, FALSE);
	hInFile = CreateFile(argv[1], GENERIC_READ,
		0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile) {
		_tprintf(TEXT("%u \n"),GetLastError() );
		ReportError(_T("tail error: Cannot openfile."), 2, TRUE);
	}

	// Get the current file size in bytes. 
	if (!GetFileSizeEx(hInFile, &FileSize))
		ReportError(_T("tail error: file size"), 3, TRUE);
	*/
	/* Set the file pointer on the assumption that 256
		is the maximum size and look for the line beginnings.
		If 10 lines are not located, then just put out the
		ones that are found. A more general solution would look
		farther back in the file if necessary. Alternatively,
		start the scan at the beginning of the file, but that
		would be time consuming for a large file. */
	/*
	CurPtr.QuadPart = (LONGLONG)FileSize.QuadPart
		- NUM_LINES * MAX_LINE_SIZE * sizeof(TCHAR);
	if (CurPtr.QuadPart < 0) CurPtr.QuadPart = 2; //possible if file is small 
	if (!SetFilePointerEx(hInFile, CurPtr, &FPos, FILE_END))
		ReportError(_T("tail Error: Set Pointer."), 4, TRUE);
	_tprintf(TEXT("%llx"),FPos.QuadPart);
	FPos.QuadPart = -0x1c;
	if (!SetFilePointerEx(hInFile, FPos, &CurPtr, FILE_CURRENT))
		ReportError(_T("tail Error: Set Pointer."), 4, TRUE);
	_tprintf(TEXT("\n%llx"), CurPtr.QuadPart);
	CloseHandle(hInFile);
	return 0;*/
}

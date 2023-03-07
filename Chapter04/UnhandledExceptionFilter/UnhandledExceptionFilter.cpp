/* Only works for unicode files.   */

/*
	Convert one or more files, changing all letters to upper case
	The output file will be the same name as the input file, except
	a "UC_" prefix will be attached to the file name.

/* This program illustrates:
	1.	Exception handling to recover from a run-time exceptions
		and errors by using RaiseException).
	2.	Completion hanlders.
	3.	Program logic simplification using handlers. */

	/* Technique:
		1.	Loop to process one file at a time.
		2.	Read the entire file into memory, allocating the memory and
			output file first. DO NOT WRITE OVER AN EXISTING FILE
		3. 	Convert the data in memory.
		4. 	Write the converted data to the output file.
		5.  Free memory and close file handles on each loop iteration. */

#include "stdafx.h"


LONG WINAPI Filter(LPEXCEPTION_POINTERS);

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
	DWORD nXfer, iFile, j;
	TCHAR outFileName[MAX_PATH] = _T(""), * pBuffer = NULL;
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };
	LARGE_INTEGER fSize;
	SetUnhandledExceptionFilter(Filter);
	

	if (argc <= 1)
		ReportError(_T("Usage: toupper.exe file1 [file2 file3 ...]"), 1, FALSE);

	/* Process all files on the command line. */
	for (iFile = 1; iFile < (unsigned int)argc; iFile++) __try { /* Exception block */
		/*	All file handles are invalid, pBuffer == NULL, and
			outFileName is empty. This is assured by the handlers */
		if (_tcslen(argv[iFile]) > MAX_PATH - 8) //8 for _UC.txt\0
			ReportException(_T("The file name is too long."), 1);

		_stprintf_s(outFileName, 256, _T("%s_UC.txt"), argv[iFile]);//filename: UC_
		__try { /* Inner try-finally block */
			/* Create the output file name, and create it (fail if file exists) */
			/* The file can be shared by other processes or threads */
			hIn = CreateFile(argv[iFile], GENERIC_READ,
				0, NULL, OPEN_EXISTING, 0, NULL);
			if (hIn == INVALID_HANDLE_VALUE) ReportException(argv[iFile], 1);
			if (!GetFileSizeEx(hIn, &fSize) || fSize.HighPart > 0) //maz size is 4GB
				ReportException(_T("This file is too large for this program."), 1);
			hOut = CreateFile(outFileName, GENERIC_READ | GENERIC_WRITE,
				0, NULL, CREATE_ALWAYS, 0, NULL);
			if (hOut == INVALID_HANDLE_VALUE) ReportException(outFileName, 1);;
			/* Allocate memory for the file contents */
			pBuffer = (TCHAR*)malloc(fSize.LowPart);
			if (pBuffer == NULL) ReportException(_T("Memory allocation error"), 1);;

			/* Read the data, convert it, and write to the output file */
			/* Free all resources upon completion, and then process the next file */
			/* Files are not locked, as the files are open with 0
			 *     for the share mode. However, in a more general situation, you
			 *     might have a process (Chapter 7) inherit the file handle or a
			 *     thread (Chapter 7) might access the handle. Or, you might
			 *     actually want to open the file in share mode                    */


			if (!ReadFile(hIn, pBuffer, fSize.LowPart, &nXfer, NULL) || (nXfer != fSize.LowPart))
				ReportException(_T("ReadFile error"), 1);

			for (j = 0; j < fSize.LowPart / TSIZE; j++) //TSIZE = sizeof(TCHAR)
				if (_istalpha(pBuffer[j])) pBuffer[j] = _totupper(pBuffer[j]);


			if (!WriteFile(hOut, pBuffer, fSize.LowPart, &nXfer, NULL) || (nXfer != fSize.LowPart))
				ReportException(_T("WriteFile error"), 1);


		}
		__finally { /* File handles are always closed */
		 /* memory freed, and handles and pointer reinitialized. */
			if (pBuffer != NULL) free(pBuffer); pBuffer = NULL;
			if (hIn != INVALID_HANDLE_VALUE) {
				CloseHandle(hIn);
				hIn = INVALID_HANDLE_VALUE;
			}
			if (hOut != INVALID_HANDLE_VALUE) {
				CloseHandle(hOut);
				hOut = INVALID_HANDLE_VALUE;
			}
			_tcscpy_s(outFileName, MAX_PATH, _T(""));
			_tprintf(_T("__finally 01 cleaning reseources for this file %s\n"), argv[iFile]);
			//return 0; //never put return in finally
			//ReportException (_T("__finally exception"), 1);
		}
	} /* End of main file processing loop and try block. */

	/* This exception handler applies to the loop body */
	//__except (EXCEPTION_EXECUTE_HANDLER) { //any unhandled exception from loop body no matter where it came from (__try or __finally or code not within __try/ __finally)
	__finally{
		_tprintf(_T("__finally 02 cleaning reseources for this file %s\n"), argv[iFile]);
		DeleteFile(outFileName); //delete the file
	}
	_tprintf(_T("All files converted, except as noted above\n"));
	return 0;
}

//if running under debugger then UnhandledExceptionFilter
//invokes debugger rather than calling this filter function
LONG WINAPI Filter(LPEXCEPTION_POINTERS pExp) {
	DWORD exCode;
	
	exCode = pExp->ExceptionRecord->ExceptionCode;

	_tprintf(_T("Filter. exCode: %x\n"), exCode);
	return EXCEPTION_EXECUTE_HANDLER;
	
}
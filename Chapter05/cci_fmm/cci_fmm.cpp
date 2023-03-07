// cci.cpp : Defines the entry point for the console application.
//
/* Chapter 2. cci Version 1. Modified Caesar cipher (http://en.wikipedia.org/wiki/Caesar_cipher). */
/* Main program, which can be linked to different implementations */
/* of the cci_f function. */

/* cci shift file1 file2
 *		shift is the shifting amount.
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
	
	if (!cci_f (argv [2], argv [3], _ttoi(argv[1])))
		ReportError (_T ("Encryption failed."), 4, TRUE);

	return 0;
}

#define BUF_SIZE 65536	  /* Generally, you will get better performance with larger buffers (use powers of 2). 
					      /* 65536 worked well; larger numbers did not help in some simple tests. */

/* Chapter 5.
cci_fMM.c function: Memory Mapped implementation of the
simple Caeser cipher function. */



BOOL cci_f (LPCTSTR fIn, LPCTSTR fOut, DWORD shift)

/* Caesar cipher function. 
*	fIn:		Source file pathname.
*	fOut:		Destination file pathname.
*	shift:		Numeric shift value */
{
	BOOL complete = FALSE;
	HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
	HANDLE hInMap = NULL, hOutMap = NULL;
	LPTSTR pIn = NULL, pInFile = NULL, pOut = NULL, pOutFile = NULL;

	__try {
		LARGE_INTEGER fileSize;

		/* Open the input file. */
		hIn = CreateFile (fIn, GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hIn == INVALID_HANDLE_VALUE) 
			ReportException (_T ("Failure opening input file."), 1);

		/* Get the input file size. */
		if (!GetFileSizeEx (hIn, &fileSize))
			ReportException (_T ("Failure getting file size."), 4);
		/* This is a necessar, but NOT sufficient, test for mappability on 32-bit systems S
		 * Also see the long comment a few lines below */
		if (fileSize.HighPart > 0 && sizeof(SIZE_T) == 4) //even this greater for 32-bit appl where max 3GB address space is allocated
			ReportException (_T ("This file is too large to map on a Win32 system."), 4);

		/* Only one of the followings can be passed as 3rd argument CreateFileMapping. 
		   Specifies the page protection of the file mapping object. All mapped views of the object must be compatible with this protection.

		  PAGE_READONLY: Allows views to be mapped for read-only access. An attempt to write to a specific region results
		  in an access violation. The file handle that the hFile parameter specifies must be created with the GENERIC_READ access right.
		  PAGE_READWRITE: Allows views to be mapped for read-only, copy-on-write,or read/write access. The file handle that the hFile 
		  parameter specifies must becreated with the GENERIC_READ and GENERIC_WRITE access rights.
		  PAGE_WRITECOPY: you can read/write to the file's data. Writing causes a private
		                  copy of the page to be created. You must have passed 
						  either GENERIC_READ or GENERIC_READ | GENERIC_WRITE to CreateFile.

		  Attributes for 3rd arg of CreateFileMapping
		  SEC_COMMIT, EC_IMAGE, SEC_IMAGE_NO_EXECUTE, SEC_RESERVE
		*/


		/* Create a file mapping object on the input file. Use the file size. */
		hInMap = CreateFileMapping (hIn, NULL, PAGE_READONLY, 0, 0, NULL);//file mapping object max size equals to file size
		if (hInMap == NULL)
			ReportException (_T ("Failure Creating input map."), 2);

		/* 2nd arg (The type of access to a file mapping object) of MapViewOfFile
		   FILE_MAP_ALL_ACCESS: A read/write view of the file is mapped. The file mappingobject must have been created with PAGE_READWRITE 
		                        or PAGE_EXECUTE_READWRITE protection.
		   FILE_MAP_WRITE: Equivalent to FILE_MAP_ALL_ACCESS or ( FILE_MAP_WRITE |FILE_MAP_READ
)		   FILE_MAP_READ: A read-only view of the file is mapped. An attempt towrite to the file view results in an access violation.
                          The file mapping object must have been created with PAGE_READONLY or PAGE_READWRITE 
		
		*/

		/* Map the input file */
		/* Comment: This may fail for large files, especially on 32-bit systems
		 * where you have, at most, 3 GB to work with (of course, you have much less
		 * in reality, and you need to map two files. 
		 * This program works by mapping the input and output files in their entirety.
		 * You could enhance this program by mapping one block at a time for each file,
		 * much as blocks are used in the ReadFile/WriteFile implementations. This would
		 * allow you to deal with very large files on 32-bit systems. I have not taken
		 * this step and leave it as an exercise. 
		 */
		//3rd and 4th args specify file offset where view begins. Must be multiple of allocation granularity (64kB)
		//this offset must lie in file mapping object.
		
		/* 5th arg:
		The number of bytes of a file mapping to map to the view. All bytes must be within the maximum size specified by 
		CreateFileMapping. If this parameter is 0 (zero), the mappingextends from the specified offset to the end of the file mapping.
		
		*/

		pInFile = (LPTSTR)MapViewOfFile (hInMap, FILE_MAP_READ, 0, 0, 0);// here view can access all bytes of file mapping object starting from offset 0
		if (pInFile == NULL)
			ReportException (_T ("Failure Mapping input file."), 3);

		/*  Create/Open the output file. */
		/* The output file MUST have Read/Write access for the mapping to succeed. */
		hOut = CreateFile (fOut, GENERIC_READ | GENERIC_WRITE, 
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hOut == INVALID_HANDLE_VALUE) {
			complete = TRUE; /* Do not delete an existing file. */
			ReportException (_T ("Failure Opening output file."), 5);
		}
		/*
		.
            After a file mapping object is created, the size of the file must not exceed the size of the filemapping object;
			if it does, not all of the file contents are available for sharing.
            If an application specifies a size for the file mapping object that is larger than the size ofthe actual named 
			file on disk and if the page protection allows write access (that is, the flProtect parameter specifies PAGE_READWRITE or 
			PAGE_EXECUTE_READWRITE), then thefile on disk is increased to match the specified size of the file mapping object. If the 
			file isextended, the contents of the file between the old end of the file and the new end of thefile are not guaranteed to be 
			zero; the behavior is defined by the file system. If the file ondisk cannot be increased, CreateFileMapping fails and
            GetLastError returns ERROR_DISK_FULL
		*/
		/* Map the output file. CreateFileMapping will expand
		the file if it is smaller than the mapping. */

		hOutMap = CreateFileMapping (hOut, NULL, PAGE_READWRITE, fileSize.HighPart, fileSize.LowPart, NULL);
		if (hOutMap == NULL)
			ReportException (_T ("Failure creating output map."), 7);
		pOutFile = (LPTSTR)MapViewOfFile (hOutMap, FILE_MAP_WRITE, 0, 0, (SIZE_T)fileSize.QuadPart );
		if (pOutFile == NULL)
			ReportException (_T ("Failure mapping output file."), 8);

		/* Now move the input file to the output file, doing all the work in memory. */
		__try
		{
			CHAR cShift = (CHAR)shift;
			pIn = pInFile;
			pOut = pOutFile;
			//SIZE_T s = 0;
			//_tprintf(_T("%lu "),fileSize.QuadPart);
			while (pIn < pInFile + fileSize.QuadPart/TSIZE) { 
				*pOut = (TSIZE == 1) ? (*pIn + cShift) % 256 : (*pIn + cShift) % 65536;
				//_tprintf(_T("%lu, %p "),s++,pIn);
				pIn++; pOut++;
				
				
			}
			complete = TRUE;
		}
		__except(GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			complete = FALSE;
			ReportException(_T("Fatal Error accessing mapped file."), 9);
		}

		/* Close all views and handles. */
		UnmapViewOfFile (pOutFile); UnmapViewOfFile (pInFile);
		CloseHandle (hOutMap); CloseHandle (hInMap);
		CloseHandle (hIn); CloseHandle (hOut);
		return complete;
	}

	__except (EXCEPTION_EXECUTE_HANDLER) {
		if (pOutFile != NULL) UnmapViewOfFile (pOutFile); if (pInFile != NULL) UnmapViewOfFile (pInFile);
		if (hOutMap != NULL) CloseHandle (hOutMap); if (hInMap != NULL) CloseHandle (hInMap);
		if (hIn != INVALID_HANDLE_VALUE) CloseHandle (hIn); if (hOut != INVALID_HANDLE_VALUE) CloseHandle (hOut);

		/* Delete the output file if the operation did not complete successfully. */
		if (!complete)
			DeleteFile (fOut);
		return FALSE;
	}
}

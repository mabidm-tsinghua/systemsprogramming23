/* Chapter 5, sortMM command. Memory Mapped version. */

/*  sortMM [options] [file]
Sort one file. The extension to multiple files is straightforward but is omitted here.
This creates an index file and does not sort the file itself

Options:
-r	Sort in reverse order.
-I	Use an existing index file to produce the sorted file.

This limited implementation sorts on the first field only.
The key field is 8 characters, starting at record position 0. */

/* This program illustrates:
1.	Mapping files to memory.
2.	Use of memory-based string processing functions in a mapped file.
3.	Use of a memory based function (qsort) with a memory-mapped file.
4.	__based pointers. */

/* Technique:
1.	Map the input file (the file to be sorted).
2.	Using standard character processing, scan the input file,
    placing each key (which is fixed size) into the "index" file record
    (the input file with a .idx suffix).
    The index file is created using file I/O; later it will be mapped.
3.	Also place the start position of the record in the index file record.
    This is a __based pointer to the mapped input file.
4.	Map the index file.
5.	Use the C library qsort to sort the index file
    (which has fixed length records consisting of key and __based pointer).
6.	Scan the sorted index file, putting input file records in sorted order.
7.	Skip steps 2, 3, and 5 if the -I option is specified.
    Notice that this requires __based pointers. */

#include "stdafx.h"

int KeyCompare (LPCVOID pK1, LPCVOID pK2);
VOID CreateIndexFile (LARGE_INTEGER, LPCTSTR, LPTSTR);
DWORD_PTR kStart = 0, kSize = 8; 	/* Key start position & size (TCHAR). */
BOOL reverse;

int _tmain (int argc, LPTSTR *argv )
{
	/* The file is the first argument. Sorting is done in place. */
	/* Sorting is done by file memory mapping. */

	HANDLE hInFile, hInMap;	/* Input file handles. */
	HANDLE hXFile, hXMap;	/* Index file handles. */
	HANDLE hStdOut = GetStdHandle (STD_OUTPUT_HANDLE);
	BOOL idxExists, noPrint;
	DWORD indexSize, rXSize, iKey, nWrite;
	LARGE_INTEGER inputSize;
	LPTSTR pInFile = NULL;
	LPBYTE pXFile = NULL, pX; 
	TCHAR __based (pInFile) *pIn, indexFileName[MAX_PATH+1], ChNewLine = _T('\n');
	int FlIdx;

	/* Determine the options. */
	FlIdx = Options (argc, (LPCTSTR*)argv, _T ("rIn"), &reverse, &idxExists, &noPrint, NULL);
	if (FlIdx >= argc)
		ReportError (_T ("No file name on command line."), 1, FALSE);

	/* Step 1: Open and Map the Input File. */
	hInFile = CreateFile (argv[FlIdx], GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	if (hInFile == INVALID_HANDLE_VALUE)
		ReportError (_T ("Failed to open input file."), 2, TRUE);

	/* Create a file mapping object. Use the file size. */
	hInMap = CreateFileMapping (hInFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hInMap == NULL)
		ReportError (_T ("Failed to create input file mapping."), 3, TRUE);
	pInFile = (LPTSTR)MapViewOfFile (hInMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);//whole file view
	if (pInFile == NULL)
		ReportError (_T ("Failed to map input file."), 4, TRUE);

	if (!GetFileSizeEx (hInFile, &inputSize))
		ReportError (_T("Failed to get input file size."), 5, TRUE);

	/* Create the index file name. */
	_stprintf_s (indexFileName, MAX_PATH, _T("%s.idx"), argv [FlIdx]);

	//discard unicode signature represented by BOM
	LPTSTR tmp_pInFile = pInFile;
	#if defined(UNICODE) || defined(_UNICODE)
	   tmp_pInFile = tmp_pInFile + 1; //ignore byte order mark char
	#endif

	/* Steps 2 and 3, if necessary. */
	if (!idxExists)
		CreateIndexFile (inputSize, indexFileName, tmp_pInFile);

	/* Step 4. Map the index file. */
	hXFile = CreateFile (indexFileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	if (hXFile == INVALID_HANDLE_VALUE)
		ReportError (_T ("Failed to open existing index file."), 6, TRUE);

	/* Create a file mapping object. Use the index file size. */
	hXMap = CreateFileMapping (hXFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hXMap == NULL)
		ReportError (_T ("Failed to create index file mapping."), 7, TRUE);
	pXFile = (LPBYTE)MapViewOfFile (hXMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pXFile == NULL)
		ReportError (_T ("Failed to map index file."), 8, TRUE);
	indexSize = GetFileSize (hXFile, NULL); /* Assume the index file is not huge */
	/* Individual index record size = Key plus a pointer. */
	rXSize = kSize * TSIZE + sizeof (LPTSTR);

	__try
	{
		/* Step 5. Sort the index file using qsort. */
		if (!idxExists)
			qsort (pXFile, indexSize / rXSize, rXSize, KeyCompare);

		/* Step 6. Output the sorted input file. */
		/* Point to the first pointer in the index file. */
		pX = pXFile + rXSize - sizeof (LPTSTR); //pX and pXFile are LPBYTE
		if (!noPrint) {
			for (iKey = 0; iKey < indexSize / rXSize; iKey++) {		
				WriteFile (hStdOut, &ChNewLine, TSIZE, &nWrite, NULL);//printing new line char

				/*	The cast on pX is important */
				pIn = (TCHAR __based (tmp_pInFile)*) *(DWORD_PTR*)pX;//*(DWORD_PTR*)pX is the pointer relative to pInFile
				while ((DWORD_PTR)pIn < inputSize.QuadPart - sizeof(TCHAR) && (*pIn != CR || *(pIn + 1) != LF)) {
					WriteFile (hStdOut, pIn, TSIZE, &nWrite, NULL);
					pIn++;
				}
				pX += rXSize; /* Advance to the next index file pointer */
			}
			WriteFile (hStdOut, &ChNewLine, TSIZE, &nWrite, NULL);
		}
	}
	__except(GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		ReportError(_T("Fatal Error accessing mapped file."), 9, TRUE);
	}

	/* Done. Free all the handles and maps. */
	UnmapViewOfFile (pInFile);
	CloseHandle (hInMap); CloseHandle (hInFile);
	UnmapViewOfFile (pXFile);
	CloseHandle (hXMap); CloseHandle (hXFile);
	return 0;
}

VOID CreateIndexFile (LARGE_INTEGER inputSize, LPCTSTR indexFileName, LPTSTR pInFile) //pInFile specifies the address of 1st record

/* Perform Steps 2-3 as defined in program description. */
/* This step will be skipped if the options specify use of an existing index file. */
{
	HANDLE hXFile;
	TCHAR __based (pInFile) *pInScan = 0; //0 is a special case
	DWORD nWrite;

	/* Step 2: Create an index file.
	Do not map it yet as its length is unknown. */
	hXFile = CreateFile (indexFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	if (hXFile == INVALID_HANDLE_VALUE)
		ReportError (_T ("Failure to create index file."), 10, TRUE);

	/* Step 3. Scan the complete file, writing keys
	   and record pointers to the key file. */
	//need cast coz cannot mix integer and pointer types

	while ((DWORD_PTR) pInScan < inputSize.QuadPart) { //DWORD_PTR can be 32- or 64-bit
		//each record in index file has two elements
		WriteFile (hXFile, pInScan + kStart, kSize * TSIZE, &nWrite, NULL);//writing key 
		WriteFile (hXFile, &pInScan, sizeof (LPTSTR), &nWrite, NULL);//writing pointer offset,relative to pInFile, oe each record
		//move pointer at the end of a record; record is of variable length
		while ( (DWORD_PTR)pInScan < inputSize.QuadPart - sizeof(TCHAR) && 
				((*pInScan != CR) || (*(pInScan + 1) != LF))) {
			pInScan++; /* Skip to end of line. */
		}
		pInScan += 2; /* next record begining*/
	}
	CloseHandle (hXFile);
	

	return;
}

int KeyCompare (LPCVOID pK1, LPCVOID pK2)

/* Compare two records of generic characters.
The key position and length are global variables. */
{
	DWORD i;
	TCHAR t1, t2;
	int Result = 0;
	LPCTSTR pKey1 = (LPCTSTR)pK1, pKey2=(LPCTSTR)pK2;
	for (i = kStart; i < kSize + kStart && Result == 0; i++) {
		t1 = *pKey1;
		t2 = *pKey2;
		if (t1 < t2)
			Result = -1;
		if (t1 > t2) 
			Result = +1;
		pKey1++;
		pKey2++;
	}
	return reverse ? -Result : Result;
}



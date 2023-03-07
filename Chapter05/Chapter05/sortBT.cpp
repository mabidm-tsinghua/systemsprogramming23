/* Chapter 5, sortBT command.   Binary Tree version. */

/* sort [-n] file1 file2
	Sort one or more files. File has many lines, we just use first 8 characters to sort lines
	First 8 characters make up a key
	This limited implementation sorts on the first field only.
	The key field length is assumed to be fixed length (8-characters).
	The data fields are varying length character strings. */

/* This program illustrates:
	1.	Multiple independent heaps; one for the sort tree nodes,
		the other for the records which is a string line here (key+rest of string line).
	2.	Using HeapDestroy to free an entire data structure in a single operation.
	3.	Structured exception handling to catch memory allocation errors. */

/* Technique:
	1.	Scan the input file, placing each key (which is fixed size)
		into the binary search tree and each record onto the data heap.
	2. 	Traverse the search tree and output the records in order.
	3. 	Destroy the heap and repeat for the next file. */

#include "stdafx.h"

#define KEY_SIZE 8

	/* Structure definition for a tree node. */

typedef struct _TREENODE {
	struct _TREENODE *Left, *Right;
	TCHAR key[KEY_SIZE];
	LPTSTR pData;
} TREENODE, *LPTNODE, **LPPTNODE;

#define NODE_SIZE sizeof (TREENODE)
#define NODE_HEAP_ISIZE 0x8000
#define DATA_HEAP_ISIZE 0x8000
#define MAX_DATA_LEN 0x1000
#define TKEY_SIZE KEY_SIZE * sizeof (TCHAR)
#define STATUS_FILE_ERROR 0xE0000001    // user exception

LPTNODE FillTree (HANDLE, HANDLE, HANDLE);
BOOL Scan (LPTNODE);
int KeyCompare (LPCTSTR, LPCTSTR), iFile; /* for access in exception handler */
BOOL InsertTree (LPPTNODE, LPTNODE);

int _tmain (int argc, LPTSTR argv[])
{
	HANDLE hIn = INVALID_HANDLE_VALUE, hNode = NULL, hData = NULL;
	LPTNODE pRoot;//pointer to 1st node
	BOOL noPrint;
	TCHAR errorMessage[256];
	int iFirstFile = Options (argc, (LPCTSTR*)argv, _T ("n"), &noPrint, NULL);

	if (argc == iFirstFile) //if argc == iFirstFile is true then user just ran the program using this sortBT.exe or sortBT.exe -n
		ReportError (_T ("Usage: sortBT [options] files"), 1, FALSE);
					/* Process all files on the command line. */
	for (iFile = iFirstFile; iFile < argc; iFile++) __try {
					/* Open the input file. */
		hIn = CreateFile (argv[iFile], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); //exclusive access
		if (hIn == INVALID_HANDLE_VALUE)
			RaiseException (STATUS_FILE_ERROR, 0, 0, NULL);
		
		__try {
			/* Allocate the two growable heaps. */
		
			hNode = HeapCreate ( 
				//safe t use HEAP_NO_SERIALIZE option as this is not multithreaded appl.
				HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, //raise an exception to indicate failure for calls to HeapAlloc and HeapReAlloc instead of returning NULL
				NODE_HEAP_ISIZE, //the initial amount of memory (rounded up to a multiple of the system page) that is committed for the heap size.
				0 //max size of heap (multiple of the system page size). 0 means the heap can grow
				); 
			
			hData = HeapCreate (
				HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, DATA_HEAP_ISIZE, 0);
		
			if (hNode == NULL || hData==NULL) 
		        ReportError (_T ("Cannot Allocate Heap"), 1, TRUE);
			
		
		
			/* Process the input file, creating the tree. */
			pRoot = FillTree (hIn, hNode, hData);

			/* Display the tree in key order. */
			if (!noPrint) {
				_tprintf (_T ("Sorted file: %s\n"), argv[iFile]);
				Scan (pRoot);
			} 
			
		} __finally { /* Heaps and file handle are always closed */
			/* Destroy the two heaps and data structures. */
			if (hNode != NULL) HeapDestroy (hNode);
			if (hData != NULL) HeapDestroy (hData);
			hNode = NULL; hData = NULL;
			if (hIn != INVALID_HANDLE_VALUE) CloseHandle (hIn);
			hIn = INVALID_HANDLE_VALUE;
			_tprintf(_T("Termination Handler for file %s\n"),argv[iFile]);
		}
		
	} /* End of main file processing loop and try block. */
	/* Handle the exceptions that we can expect - Namely, file open error or out of memory. */
	__except ( (GetExceptionCode() == STATUS_FILE_ERROR || GetExceptionCode() == STATUS_NO_MEMORY)
				? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		_stprintf (errorMessage, _T("\n%s %s"), _T("sortBT error on file:"), argv[iFile]);
		ReportError (errorMessage, 0, TRUE);
	}
	
	return 0;
}

LPTNODE FillTree (HANDLE hIn, HANDLE hNode, HANDLE hData)

/* Scan the input file, creating a binary search tree in the
	hNode heap with data pointers to the hData heap. */
/* Use the calling program's exception handler. */
{
	LPTNODE pRoot = NULL, pNode; //pRoot is pointer to first node in a tree
	DWORD nRead, i;
	BOOL atCR;
	TCHAR dataHold[MAX_DATA_LEN];
	LPTSTR pString;
	//this is first char in unicode text file
	//this is non-character with code (U+FEFF)
	//priting it will print ? on console
	
#if defined(UNICODE) || defined(_UNICODE)
	TCHAR bom;//byte order mark known as unicode signature; BOM determines utf encoding and text endianness; 
	ReadFile (hIn, &bom, TSIZE, &nRead, NULL);//read the first character of a file and discard it
#endif
	

					/* Open the input file. */
	while (TRUE) { // when no more data to read from a file while returns from a loop
		pNode = (LPTNODE) HeapAlloc (hNode, HEAP_ZERO_MEMORY, NODE_SIZE); //The allocated memory will be initialized to zero
		//pNode->pData = NULL;
		//(pNode->Left) = pNode->Right = NULL;
					/* Read the key. Return if done. */
		if (!ReadFile (hIn, pNode->key, TKEY_SIZE, &nRead, NULL) || nRead != TKEY_SIZE)
					/* Assume end of file on error. All records must be just the right size */
			return pRoot;	
		atCR = FALSE; 		/* Last character was not a CR. */
		/* Read the data until the end of line. */
		for (i = 0; i < MAX_DATA_LEN; i++) {
			ReadFile (hIn, &dataHold[i], TSIZE, &nRead, NULL); //reading a single character into a buffer
			if (atCR && dataHold[i] == LF) break;
			atCR = (dataHold[i] == CR);
		}
		dataHold[i - 1] = _T('\0'); //overwrite CR character

		/* dataHold contains the data without the key.
			Combine the key and the Data. */

		pString = (LPTSTR)HeapAlloc (hData, HEAP_ZERO_MEMORY,
				(SIZE_T)(KEY_SIZE + _tcslen (dataHold) + 1) * TSIZE);
		memcpy (pString, pNode->key, TKEY_SIZE); //intially copy key
		pString[KEY_SIZE] = _T('\0');//terminate key with NULL char
		_tcscat (pString, dataHold); //combine key and other part of the string
		pNode->pData = pString;
		/* Insert the new node into the search tree. */
		InsertTree (&pRoot, pNode);

	} /* End of while (TRUE) loop */
	return NULL; /* Failure */
}

BOOL InsertTree (LPPTNODE ppRoot, LPTNODE pNode)

/* Insert the new node, pNode, into the binary search tree, pRoot. */
{
	if (*ppRoot == NULL) {
		*ppRoot = pNode;
		return TRUE;
	}
	//if new node key is smaller 
	if (KeyCompare (pNode->key, (*ppRoot)->key) < 0) // 0 means equal , < 0 means first string character is < the corresponding character in the 2nd string
		InsertTree (&((*ppRoot)->Left), pNode);//left node for smaller string
	else
		InsertTree (&((*ppRoot)->Right), pNode);//right node 
	return TRUE;
}

int KeyCompare (LPCTSTR pKey1, LPCTSTR pKey2)

/* Compare two records of generic characters.
	The key position and length are global variables. */
{
	return _tcsncmp (pKey1, pKey2, KEY_SIZE);
}

/*
-start from left side of a node and reach to last node where left = NULL
-now print data of last node using _tprintf
-now process the right side of last node. now follow step 1 and 2

*/

static BOOL Scan (LPTNODE pNode)

/* Scan and print the contents of a binary tree. */
{
	if (pNode == NULL)
		return TRUE;
	Scan (pNode->Left);
	_tprintf (_T ("%s\n"), pNode->pData);
	//_tprintf (_T ("%x %s %x\n"), pNode->pData[0],pNode->pData,pNode->key[0]);
	Scan (pNode->Right);
	return TRUE;
}

/* Chapter 5. cci Explicit Link version.    Caesar cipher. */
/* cciEL shift file1 file2 DllName
	The DLL is loaded to provide the cci_f function. */

/* This program illustrates:
	1. Implicit DLL linking. */

#include "stdafx.h"

	//always good to use extern "C"
extern "C" __declspec (dllimport) BOOL cci_f(LPCTSTR, LPCTSTR, DWORD);

int _tmain (int argc, LPTSTR argv[])
{
	
	
	if (argc < 4)
		ReportError (_T("Usage: cci_f_IL shift file1 file2"), 1, FALSE);

	/*  Call the function. */
 
	if (!cci_f (argv[2], argv[3], _ttoi(argv[1]) ) ) {
		ReportError (_T ("cci failed."), 6, TRUE);
	}
	
	return 0;
}

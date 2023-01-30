/* Chapter 2. cat. */
/* console_redirect [-i|-o] [stdin_file|stdout_file]
	-redirect stdin or stdout 
	-if -i specified then stdin is redirected to stdin_file
	--if -o specified then stdout is redirected to stdout_file*/

	

#include "stdafx.h"

#define BUF_SIZE 512

static VOID CatFile(HANDLE hInFile, HANDLE hOutFile);
DWORD Options (int argc, LPCTSTR argv[], LPCTSTR OptStr, ...);
VOID ReportError(LPCTSTR userMessage, DWORD exitCode, BOOL printErrorMessage);
BOOL PrintMsg(HANDLE hOut, LPCTSTR pMsg);

int _tmain(int argc, LPCTSTR argv[])
{

	HANDLE hInFile , hOutFile ;
	
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL redirectStdin,redirectStdout;
	int iFirstFile;

	/*	 */
	/*	iFirstFile is the argv [] index of the input file. */
	iFirstFile = Options(argc, argv, _T("io"), &redirectStdin,&redirectStdout, NULL);
	
	//test if user has specified both -i and -o

	if (redirectStdin) { 
		hInFile = CreateFile(argv[iFirstFile], GENERIC_READ,
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hInFile == INVALID_HANDLE_VALUE) {
			ReportError(_T("\nSTDIN redirection failed."), 0, TRUE);
		}
		SetStdHandle(STD_INPUT_HANDLE,hInFile);
		CatFile(GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE));
		CloseHandle(hInFile);
		
	}else if (redirectStdout) { 
		hOutFile = CreateFile(argv[iFirstFile], GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hOutFile == INVALID_HANDLE_VALUE) {
			ReportError(_T("\nSTDOUT redirection failed."), 0, TRUE);
		}
		SetStdHandle(STD_OUTPUT_HANDLE, hOutFile);
		CatFile(GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE));
		CloseHandle(hOutFile);
	}
	else {
		PrintMsg(hStdOut,TEXT("\nNeither STDOUT nor STDIN redirected"));
	}

	/* // stdin still refers to console original input
	TCHAR buf[256];
	_fgetts(buf,256,stdin);
	_tprintf(TEXT("%s"),buf);
	*/

	//restore standard handles

	SetStdHandle(STD_INPUT_HANDLE, hStdIn);
	SetStdHandle(STD_OUTPUT_HANDLE, hStdOut);
	return 0;
}


static VOID CatFile(HANDLE hInFile, HANDLE hOutFile)
{
	DWORD nIn, nOut;
	BYTE buffer[BUF_SIZE];

	while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL) && (nIn != 0)
		&& WriteFile(hOutFile, buffer, nIn, &nOut, NULL));

	return;
}



#include "stdafx.h"

int _tmain(int argc, LPTSTR argv[]) {


	return 0;
}


BOOL Funcarama1() {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PVOID pvBuf = NULL;
	DWORD dwNumBytesRead;
	BOOL bOk;
	hFile = CreateFile(TEXT("SOMEDATA.DAT"), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return(FALSE);
	}
	pvBuf = VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
	if (pvBuf == NULL) {
		CloseHandle(hFile);
		return(FALSE);
	}
	bOk = ReadFile(hFile, pvBuf, 1024, &dwNumBytesRead, NULL);
	if (!bOk || (dwNumBytesRead == 0)) {
		VirtualFree(pvBuf, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return(FALSE);
	}
	// Do some calculation on the data....
		// Clean up all the resources.
	VirtualFree(pvBuf, 0,MEM_RELEASE );
	CloseHandle(hFile);
	return(TRUE);
}

////

BOOL Funcarama2() {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PVOID pvBuf = NULL;
	DWORD dwNumBytesRead;
	BOOL bOk, bSuccess = FALSE;
	hFile = CreateFile(TEXT("SOMEDATA.DAT"), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		pvBuf = VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
		if (pvBuf != NULL) {
			bOk = ReadFile(hFile, pvBuf, 1024, &dwNumBytesRead, NULL);
			if (bOk && (dwNumBytesRead != 0)) {
				// Do some calculation on the data....
					bSuccess = TRUE;
			}
			VirtualFree(pvBuf, 0, MEM_RELEASE);
		}
		CloseHandle(hFile);
	}
	return(bSuccess);
}
///

DWORD Funcarama3() {
	// IMPORTANT: Initialize all variables to assume failure.
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PVOID pvBuf = NULL;
	__try {
		DWORD dwNumBytesRead;
		BOOL bOk;
		hFile = CreateFile(TEXT("SOMEDATA.DAT"), GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return(FALSE);
		}
		pvBuf = VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
		if (pvBuf == NULL) {
			return(FALSE);
		}
		bOk = ReadFile(hFile, pvBuf, 1024, &dwNumBytesRead, NULL);
		if (!bOk || (dwNumBytesRead != 1024)) {
			return(FALSE);
		}
		// Do some calculation on the data....
	}
	__finally {
		// Clean up all the resources.
		if (pvBuf != NULL)
			VirtualFree(pvBuf, 0, MEM_RELEASE);
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}
	// Continue processing.
	return(TRUE);
}
///
DWORD Funcarama4() {
	// IMPORTANT: Initialize all variables to assume failure.
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PVOID pvBuf = NULL;
	// Assume that the function will not execute successfully.
	BOOL bFunctionOk = FALSE;
	__try {
		DWORD dwNumBytesRead;
		BOOL bOk;
		hFile = CreateFile(TEXT("SOMEDATA.DAT"), GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			__leave;//replaces return statement
		}
		pvBuf = VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
		if (pvBuf == NULL) {
			__leave;
		}
		bOk = ReadFile(hFile, pvBuf, 1024, &dwNumBytesRead, NULL);
		if (!bOk || (dwNumBytesRead == 0)) {
			__leave;
		}
		// Do some calculation on the data....
			// Indicate that the entire function executed successfully.
			bFunctionOk = TRUE;
	}
	__finally {
		// Clean up all the resources.
		if (pvBuf != NULL)
			VirtualFree(pvBuf, 0, MEM_RELEASE);
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}
	// Continue processing.
	return(bFunctionOk);
}
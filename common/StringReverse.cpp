#include "stdafx.h"
/*
wcsnlen_s()
These functions return the number of characters in the string, 
not including the terminating null character. If there's no null 
terminator within the first numberOfElements bytes of the string 
(or wide characters for wcsnlen), then numberOfElements is 
returned to indicate the error condition; null-terminated strings 
have lengths that are strictly less than numberOfElements.*/

BOOL StringReverseW(PWSTR pWideCharStr, DWORD cchLength) {
	// Get a pointer to the last character in the string.
	size_t size = wcsnlen_s(pWideCharStr, cchLength);
	//_tprintf(_T("%u, %u\n"), size, cchLength);
	//no NULL char in the string
	//if (size == cchLength) { ExitProcess(-1); }
	PWSTR pEndOfStr = pWideCharStr + size - 1;
	wchar_t cCharT;
	// Repeat until we reach the center character in the string.
	while (pWideCharStr < pEndOfStr) {
		// Save a character in a temporary variable.
		cCharT = *pWideCharStr;
		// Put the last character in the first character.
		*pWideCharStr = *pEndOfStr;
		// Put the temporary character in the last character.
		*pEndOfStr = cCharT;
		// Move in one character from the left.
		pWideCharStr++;
		// Move in one character from the right.
		pEndOfStr--;
	}
	// The string is reversed; return success.
	return(TRUE);
}

//
BOOL StringReverseA(PSTR pMultiByteStr, DWORD cchLength) {
	PWSTR pWideCharStr;
	int nLenOfWideCharStr;
	BOOL fOk = FALSE;
	// Calculate the number of characters needed to hold
// the wide-character version of the string.
	nLenOfWideCharStr = MultiByteToWideChar(CP_ACP, 0,
		pMultiByteStr, cchLength, NULL, 0);
	// Allocate memory from the process' default heap to
	// accommodate the size of the wide-character string.
	// Don't forget that MultiByteToWideChar returns the
	// number of characters, not the number of bytes, so
	// you must multiply by the size of a wide character.
	pWideCharStr = (PWSTR)HeapAlloc(GetProcessHeap(), 0,
		nLenOfWideCharStr * sizeof(wchar_t));
	if (pWideCharStr == NULL)
		return(fOk);
	// Convert the multibyte string to a wide-character string.
	MultiByteToWideChar(CP_ACP, 0, pMultiByteStr, cchLength,
		pWideCharStr, nLenOfWideCharStr);
	// Call the wide-character version of this
	// function to do the actual work.
	fOk = StringReverseW(pWideCharStr, cchLength);
	if (fOk) {
		// Convert the wide-character string back
		// to a multibyte string.
		WideCharToMultiByte(CP_ACP, 0, pWideCharStr, cchLength,
			pMultiByteStr, (int)strlen(pMultiByteStr), NULL, NULL);
	}
	// Free the memory containing the wide-character string.
	HeapFree(GetProcessHeap(), 0, pWideCharStr);
	return(fOk);
}
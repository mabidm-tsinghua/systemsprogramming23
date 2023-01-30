#include "stdafx.h"



/* General string routines. */

//LPCTSTR _memchr (LPCTSTR s, TCHAR c, DWORD n)

/* Locate the first occurrence of c in the
	initial n characters of the object indicated by s. 
	Returns a pointer to the located character or
	a NULL if the character is not found. */
/*
{
	DWORD in = 0;
	LPCTSTR su;

	for (su = s; in < n && *su != _T('\0'); su++,in++) 
		if (*su == c) return su;
	return NULL;
}
*/
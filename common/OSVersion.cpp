

#include "stdafx.h"


BOOL WindowsVersionOK (DWORD MajorVerRequired, DWORD MinorVerRequired)
{
	/* Can this Windows version run this application */
    OSVERSIONINFO OurVersion;

/*
typedef struct _OSVERSIONINFO{ 
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    TCHAR szCSDVersion[ 128 ]; 
} OSVERSIONINFO; 
*/ 

	/* Determine version information.  */
	/* The version number to test for is an NT version number */
	/* 9x systems must be ruled out explicitely */
	/* See the OSVERSIONINFO description in MSDN for Windows 9x */

	OurVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(suppress : 4996)
	if (!GetVersionEx (&OurVersion)) 
		ReportError (_T("Cannot get OS Version info."), 1, TRUE);

	//VER_PLATFORM_WIN32_NT value is for windows 2000, windows XP, windows server 2003, Windows vista, windows server 2008, windows 7
    
	return ( (OurVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
		       ((OurVersion.dwMajorVersion > MajorVerRequired) ||
                (OurVersion.dwMajorVersion >= MajorVerRequired &&  OurVersion.dwMinorVersion >= MinorVerRequired) 
			   )
			);
	
}


/* Chapter 6. timepp. Simplified version with no special 
	auxiliary functions or header files.  */

/* timeprint: Execute a command line and display
	the time (elapsed, kernel, user) required. */

/* This program illustrates:
	1. Creating processes.
	2. Obtaining the command line.
	3. Obtaining the elapsed times.
	4. Converting file times to system times.
	5. Displaying system times.
		Windows only. */

#include "stdafx.h"
BOOL WindowsVersionOK (DWORD MajorVerRequired, DWORD MinorVerRequired);

int _tmain (int argc, LPTSTR argv[])
{
	STARTUPINFO startUp;
	PROCESS_INFORMATION procInfo;
	union {		/* Structure required for file time arithmetic. */
		LONGLONG li;
		FILETIME ft;
	} createTime, exitTime, elapsedTime;

	FILETIME kernelTime, userTime;
	SYSTEMTIME elTiSys, keTiSys, usTiSys;
	LPTSTR targv, cLine = GetCommandLine ();
	
	HANDLE hProc;

	targv = SkipArg(cLine, 1, argc, argv);//targv points to first arg of CL 
	/*  Skip past the first blank-space delimited token on the command line */
	if (argc <= 1 || NULL == targv) //argc is atleast 2
		ReportError (_T("Usage: timep command ..."), 1, FALSE);

	/* windows xp is 5.1*/
		
	if (!WindowsVersionOK(5,1)) 
		ReportError (_T("This program requires atleast WindowsXP"), 2, FALSE);

	GetStartupInfo (&startUp); //windows XP req

	/* Execute the command line and wait for the process to complete. */
	/*Only name of module (with no args) can be passed as first arg*/

	if (!CreateProcess (NULL, targv, NULL, NULL, TRUE,  //windows XP req
		NORMAL_PRIORITY_CLASS, NULL, NULL, &startUp, &procInfo)) 
		ReportError (_T ("\nError starting process. %d"), 3, TRUE);
	hProc = procInfo.hProcess; //child process handle
	//WAIT_OBJECT_0: The state of the specified object is signaled
	//WAIT_TIMEOUT: The time-out interval elapsed, and the object's state isnonsignaled
	if (WaitForSingleObject (hProc, INFINITE) != WAIT_OBJECT_0) 
		ReportError (_T("Failed waiting for process termination. %d"), 5, TRUE);;

	if (!GetProcessTimes (hProc, &createTime.ft,
		&exitTime.ft, &kernelTime, &userTime)) //FILETIME
			ReportError (_T("Can not get process times. %d"), 6, TRUE);

	elapsedTime.li = exitTime.li - createTime.li;
	FileTimeToSystemTime (&elapsedTime.ft, &elTiSys);
	FileTimeToSystemTime (&kernelTime, &keTiSys);
	FileTimeToSystemTime (&userTime, &usTiSys);
	_tprintf (_T ("Real Time: %02d:%02d:%02d.%03d\n"),
		elTiSys.wHour, elTiSys.wMinute, elTiSys.wSecond,
		elTiSys.wMilliseconds);
	_tprintf (_T ("User Time: %02d:%02d:%02d.%03d\n"),
		usTiSys.wHour, usTiSys.wMinute, usTiSys.wSecond,
		usTiSys.wMilliseconds);
	_tprintf (_T ("Sys Time:  %02d:%02d:%02d.%03d\n"),
		keTiSys.wHour, keTiSys.wMinute, keTiSys.wSecond,
		keTiSys.wMilliseconds);

	CloseHandle (procInfo.hThread); CloseHandle (procInfo.hProcess);
	return 0;
}

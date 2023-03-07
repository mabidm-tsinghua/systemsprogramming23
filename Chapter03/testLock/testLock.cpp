// testLock.cpp : Defines the entry point for the console application.
/*
Locking a portion of a file for exclusive access denies all other processes both 
read and write access to the specified region of the file. Locking a region that 
goes beyond the current end-of-file position is not an error.
Locking a portion of a file for shared access denies all processes write access 
to the specified region of the file, including the process that first locks the 
region. All processes can read the locked region.

Important Points:
Exlcusive lock: We can have multiple exclusive locks in the same file but for different file 
regions. Regions of exclusive lock cannot overlap. Only process that holds the exclusive lock 
can perform read/write operation in that region.
Shared locks: We can have multiple shared locks in the same file with overlapping regions.
Shared and exclusive locks cannot overlap but we can have shared and overlapping locks with 
nonoverlapping regions.Any process can read from a shared lock region w/o acquiring shared lock


*/




/*  Test Locking. Chapter 3. */


/*  Usage: TestLock FileName mode numberBytestoLock wheretoStart 
* numberBytestoLock: used only for case 01 and 02; not being used for ReadFile and WriteFile
* wheretoStart: for all case statement; Even we can use it for ReadFile and WriteFile functions to start reading at specific file position
Mode: 1 - Get a share lock and hold it until prompted to release it 
      2 - Get an exclusive lock and hold it until prompted to release
      3 - Attempt to read the file, then retry when prompted
	  4 - Attempt to write to the file; retry when prompted  */

/*	This very simple program was developed to illustrate the
	file locking logic in Tables 7-1 and 7-2.
	You should have two console prompts and execute TestLock.exe
	in each using different "mode" values. First, create a small
	file to use in the test; assume its name is "lock.txt"
	For example, in conosle 1 execute:
		TestLock lock.txt 1
	to create a share lock. 
	In the console 2 try in succession:
		TestLock lock.txt 2     You can not create an exclusive lock over a share lock
		TestLock lock.txt 1     You can create a second share lock
		TestLock lock.txt 4     You can not write over a share lock
		TestLock lock.txt 3     You can read over a share lock even without a lock.
    Points:
	     -Shared and exclusive lock cannot overlap lock region
		 -We can have shared and exclusive lock in the same file but for different regions
*/

#include "stdafx.h"

int _tmain (int argc, LPTSTR argv[])

{
	HANDLE fh;
	TCHAR String[50] = _T("Hello, world."), Buffer[50] = _T("12345678"), c;
	int command;
	/*The Offset and OffsetHigh members together represent a 64-bit file position. 
	It is a byte offset from the start of the file*/
	OVERLAPPED ov = {0, 0, 0,0,  NULL};
	DWORD nRead = 0, nWrite = 0;
	//LockFileEx and UnLockFileEx functions require windows xp or greater
	if (!WindowsVersionOK (5, 1)) //return true if os is >= windows xp ; windows xp version is 5.1 or 5.2
		ReportError (_T("This program requires Windows XP or greater"), 1, FALSE);

	if (argc < 3) ReportError (_T("Require file name and mode"), 1, FALSE);

	command = _ttoi(argv[2]); 

	DWORD totBytes =32;//default
	DWORD start = 0; //from beg of a file
	if (argc == 5) { //for case 1 and case 2
		totBytes = _ttoi(argv[3]);
		start = _ttoi(argv[4]);
	}else if (argc == 4) { //for ReadFile and WriteFile
		start = _ttoi(argv[3]);
	}
	ov.Offset = start;

	fh = CreateFile (argv[1], GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		0, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		ReportError (_T("Cannot open file"), 2, TRUE);

	switch (command) {
		case 1:	/* Get a share lock and hold until prompted to release */
			//LOCKFILE_FAIL_IMMEDIATELY DETERMINE WHETHER LockFileEx OPERAATES synchronously or asynchronously
			//ov contains the file offset of the beginning of the lock range
				if (!LockFileEx (fh, LOCKFILE_FAIL_IMMEDIATELY, //LOCKFILE_EXCLUSIVE_LOCK NOT SPECIFIED SO SHARED LOCK
					0, //reserved
					totBytes, 0, //locking first 32 bytes  
					&ov //begining of lock region
				)) 
					ReportError (_T("Shared Lock Failed"), 3, TRUE);
				_tprintf (_T("\nType any character to release the shared lock"));
				_tscanf (_T("%c"), &c);
				if (!UnlockFileEx (fh, 0, totBytes, 0, &ov)) //
					ReportError (_T("Share Unlock Failed"), 3, TRUE);
				_tprintf (_T("\nShared Lock released"));

				break;
		
		case 2:	/* Get an exclusive lock and hold until prompted to release */
			/*ov.Offset = 30;
			  ov.OffsetHigh = 0;*/
			
				if (!LockFileEx (fh, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY,
					0, totBytes, 0, &ov))
					ReportError (_T("Exclusive Lock Failed"), 3, TRUE);
				_tprintf (_T("\nType any character to release the exclusive lock"));
				_tscanf (_T("%c"), &c);
				if (!UnlockFileEx (fh, 0, totBytes, 0, &ov))
					ReportError (_T("Exclusive Unlock Failed"), 3, TRUE);
				_tprintf (_T("\nLock released"));

				break;

		case 3:	/* Read a record */
			if (!ReadFile(fh, Buffer, sizeof(Buffer)-sizeof(_T('\0')), &nRead, &ov))
				ReportError(_T("File Read failed"), 0, TRUE);
			else{ 
				Buffer[nRead/ sizeof(TCHAR)] = _T('\0');
				_tprintf(_T("\nRead succeeded: %s"), Buffer+1);//use Buffer+1 for unicode file to skip unicode signature
				}
				break;

		case 4: /* Write a record */
			/*ov.Offset = 64;
			ov.OffsetHigh = 0;
			*/
			//if (!WriteFile (fh, String, sizeof(TCHAR) * (DWORD)(_tcslen(String) + 1), &nWrite, &ov))

				if (!WriteFile (fh, String, sizeof(TCHAR) * _countof(String), &nWrite, &ov))
						ReportError (_T("File Write failed"), 0, TRUE);
				else _tprintf (_T("\nWrite succeeded"));
				break;
				
		default:
				break;
	}
	_tprintf (_T("\nProgram End"));
	return 0;
}

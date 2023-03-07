	/*	Chapter 3. RecordAccess. */
	//
     //How to run:  
	 // 1. first use this command so that file is created: 
	 // RecordAccess.exe filename num_records
     //2. Second use this command to open a file for R/W records: 
//RecordAccess.exe filename or RecordAccess.exe filename 0
	/*	Demonstrates simple record management in a file.
		The file is specified as a collection of an arbitrary
		number of records (The number is specified on the command
		line). Each record has a character string (limited length),
		creation time and last update time, and an update count.
		The file has an 8-byte HEADER containing the total number
		of record slots and the total number of non-empty records.
		The user can interactively create, update, and modify
		records.

		Usage: RecordAccessMM FileName [nrec [No_prompt]]
		1. If nrec is omitted, FileName must already exist. 
		2. If nrec is supplied and > 0, FileName is recreated (destroying any existing file)
		   and the program exits, having created an empty file.
		3. All user interaction, i.e. printf(), is suppressed if prompt is specified - useful if input 
		   commands are redirected from a file, as in the performance tests  [as in RecordAccessTIME.bat]
        4. provide any arg for No_prompt such as -p, any letter or digit, or anything

		If the number of records is large, a sparse file is recommended.
		The techniques here could also be used if a hash function
		determined the record location (this would be a simple
		prorgam enhancment). */

	/* This program illustrates:
		1. Random file access.
		2. LARGE_INTEGER arithmetic and using the 64-bit file positions. 
		3. record update in place.
		4. File initialization to 0 (this will not work under Windows 9x
			or with a FAT file system. 
	*/

	#include "stdafx.h"

	#define STRING_SIZE 256
	typedef struct _RECORD { /* File record structure */
		DWORD			referenceCount;  /* 0 meands an empty record */
		SYSTEMTIME		recordCreationTime;
		SYSTEMTIME		recordLastRefernceTime;
		SYSTEMTIME		recordUpdateTime;
		TCHAR			dataString[STRING_SIZE];
	} RECORD;
	typedef struct _HEADER { /* File header descriptor */
		DWORD			numRecords;
		DWORD			numNonEmptyRecords;
	} HEADER;

	

	int _tmain (int argc, LPTSTR argv[])
	{
		HANDLE hFile;
		LARGE_INTEGER currentPtr;//64-bit file pointer
		DWORD OpenOption, nXfer, recNo;
		RECORD record;
		TCHAR string[STRING_SIZE], command, newline;
		OVERLAPPED ov = {0, 0, 0, 0, NULL}, ovZero = {0, 0, 0, 0, NULL};
		HEADER header = {0, 0}; 
		SYSTEMTIME currentTime;
		BOOLEAN headerChange, recordChange;
		int prompt = (argc <= 3) ? 1 : 0;//better to use -p arg with Options()

 		if (argc < 2)
			ReportError (_T("Usage: RecordAccess file [nrec [prompt]]"), 1, FALSE);

		OpenOption = ((argc > 2 && _ttoi(argv[2]) <= 0) || argc <= 2) ? OPEN_EXISTING : CREATE_ALWAYS;// open existing file if either is true RecordAccess filename or RecordAccess filename 0
		hFile = CreateFile (argv[1], GENERIC_READ | GENERIC_WRITE,
			0, NULL, OpenOption, FILE_FLAG_RANDOM_ACCESS, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			ReportError (_T("RecordAccess error: Cannot open existing file."), 2, TRUE);

		//only true for file with CREATE_ALWAYS (command RecordAccess filename num_records where num_records is > 0)
		//true when we want to create  new database file
		if (argc >= 3 && _ttoi(argv[2]) > 0)  { /* Write the header (all header and pre-size the new file) */
	
			header.numRecords = _ttoi(argv[2]);
			if (!WriteFile(hFile, &header, sizeof (header), &nXfer, &ovZero))
				ReportError (_T("RecordAccess Error: WriteFile header."), 4, TRUE);
			
			currentPtr.QuadPart = (LONGLONG)sizeof(RECORD) * _ttoi(argv[2]) + sizeof(HEADER);
			if (!SetFilePointerEx (hFile, currentPtr, NULL, FILE_BEGIN))
				ReportError (_T("RecordAccess Error: Set Pointer."), 4, TRUE);
			if (!SetEndOfFile(hFile)) // increasing the file size
				ReportError (_T("RecordAccess Error: Set End of File."), 5, TRUE);
			if (prompt) _tprintf (_T("Empty file with %d records created.\n"), header.numRecords);
			return 0;
		}

/////////////////// only when existing file is opened with commands: RecordAccess filename 0 or RecordAccess filename

		/* Read the file header to find the number of records and non-empty records */
		if (!ReadFile(hFile, &header, sizeof (HEADER), &nXfer, &ovZero))
			ReportError (_T("RecordAccess Error: ReadFile header."), 6, TRUE);
		if (prompt) _tprintf (_T("File %s contains %d non-empty records of size %d.\n Total capacity: %d\n"),
			argv[1], header.numNonEmptyRecords, sizeof(RECORD), header.numRecords);

		/* Prompt the user to read or write a numbered record */
		while (TRUE) {
			headerChange = FALSE; recordChange = FALSE;
			if (prompt) _tprintf (_T("Enter r(ead)/w(rite)/d(elete)/qu(it) record#\n"));
			recNo = 0xffff;
			//Space in string format of scanf can match against any white space char including none
			//newline is required for proper working of gets() when command is w.
			//check this _tscanf(_T(" %c %u%c"), &command, &recNo, &newline)
			_tscanf(_T("%c%u%c"), &command, &recNo, &newline);//newline remove new line
			/*_tprintf(TEXT("%x %x\n"),command,recNo);*/
			if (command == _T('q')) break;
			if (recNo >= header.numRecords) { 
				if (prompt) _tprintf (_T("record Number is too large. Try again.\n"));
				continue;
			}
			currentPtr.QuadPart = (LONGLONG)recNo * sizeof(RECORD) + sizeof(HEADER);
			ov.Offset = currentPtr.LowPart;
			ov.OffsetHigh = currentPtr.HighPart;
			if (!ReadFile (hFile, &record, sizeof (RECORD), &nXfer, &ov))
				ReportError (_T("RecordAccess: ReadFile failure."), 7, FALSE);
			GetSystemTime (&currentTime); /* Use to update record time fields */ //UTC
			record.recordLastRefernceTime = currentTime;
			if (command == _T('r') || command == _T('d')) { /* Report record contents, if any */
				if (record.referenceCount == 0) {
					if (prompt) _tprintf (_T("record Number %d is empty.\n"), recNo);
					continue;
				} else {
					if (prompt) _tprintf (_T("record Number %d. Reference Count: %d \n"), 
						recNo, record.referenceCount);
					if (prompt) _tprintf (_T("Data: %s\n"), record.dataString);
					/* Exercise: Display times. See ls.c for an example */
				}
				if (command == _T('d')) { /* Delete the record */
					record.referenceCount = 0;
					header.numNonEmptyRecords--;
					headerChange = TRUE;
					recordChange = TRUE;
				}
			} else if (command == _T('w')) { /* Write the record, even if for the first time */
				if (prompt) _tprintf (_T("Enter new data string for the record.\n"));
				_fgetts (string, sizeof(string), stdin); //also read new line; \0 also appended
				string[_tcslen(string)-1] = _T('\0'); // remove the newline character
				if (record.referenceCount == 0) {
					record.recordCreationTime = currentTime;
					header.numNonEmptyRecords++;
					headerChange = TRUE;
				}
				record.recordUpdateTime = currentTime;
				record.referenceCount++;
				//destination is padded with zeros until a total of num 
				//characters have been written to it
				_tcsncpy (record.dataString, string, STRING_SIZE-1);//null character also written to destination
				recordChange = TRUE;
			} else {
				if (prompt) _tprintf (_T("command must be r, w, or d. Try again.\n"));
			}
			/* Update the record in place if any record contents have changed. */
			if (recordChange && !WriteFile (hFile, &record, sizeof (RECORD), &nXfer, &ov))
				ReportError (_T("RecordAccess: WriteFile update failure."), 8, FALSE);
			/* Update the number of non-empty records if required */
			if (headerChange) {
				if (!WriteFile (hFile, &header, sizeof (header), &nXfer, &ovZero))
					ReportError (_T("RecordAccess: WriteFile update failure."), 9, FALSE);
			}
		}

		if (prompt) _tprintf (_T("Computed number of non-empty records is: %d\n"), header.numNonEmptyRecords);
		if (!ReadFile(hFile, &header, sizeof (HEADER), &nXfer, &ovZero))
			ReportError (_T("RecordAccess Error: ReadFile header."), 10, TRUE);
		if (prompt) _tprintf (_T("File %s NOW contains %d non-empty records.\nTotal capacity is: %d\n"),
			argv[1], header.numNonEmptyRecords, header.numRecords);

		CloseHandle (hFile);
		return 0;
	}

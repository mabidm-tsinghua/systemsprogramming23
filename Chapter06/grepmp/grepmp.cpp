/* Chapter 6. grepMP. */
/* Multiple process version of grep command. */
/* grep pattern files.
	Search one or more files for the pattern.
	List the complete line on which the pattern occurs.
	Include the file name if there is more than one
	file on the command line. No Options. */

/* This program illustrates:
	1. 	Creating processes.
	2. 	Setting a child process standard I/O using the process start-up structure.
	3. 	Specifying to the child process that the parent's file handles are inheritable.
	4. 	Synchronizing with process termination using WaitForMultipleObjects
		and WaitForSingleObject.
	5.	Generating and using temporary files to hold the output of each process. */

#include "stdafx.h"

int _tmain (int argc, LPTSTR argv[])

/*	Create a separate process to search each file on the
	command line. Each process is given a temporary file,
	in the current directory, to receive the results. */
{
	HANDLE hTempFile;
	SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
			{sizeof (SECURITY_ATTRIBUTES), NULL, TRUE};
	TCHAR commandLine[MAX_PATH + 100];//app name can have len of MAX_PATH
	
	STARTUPINFO startUpSearch, startUp;
	/*Contains information about a newly created process and its primary thread*/
	PROCESS_INFORMATION processInfo;
	DWORD exitCode, dwCreationFlags = 0;
	int iProc;
	HANDLE *hProc;  /* Pointer to an array of proc handles. */
	typedef struct {TCHAR tempFile[MAX_PATH];} PROCFILE;
	PROCFILE *procFile; /* Pointer to array of temp file names. */
#ifdef UNICODE
    dwCreationFlags = CREATE_UNICODE_ENVIRONMENT;
#endif


	if (argc < 3)
		ReportError (_T ("Usage: grepMP pattern files."), 1, FALSE);

	/* Startup info for each child search process as well as
		the child process that will display the results. */

	/*Retrieves the contents of the STARTUPINFO structure that was 
	specified when the calling process was created.*/

	GetStartupInfo (&startUpSearch);
	GetStartupInfo (&startUp);

	/* Allocate storage for an array of process data structures,
		each containing a process handle and a temporary file name. */

	procFile = (PROCFILE*)malloc ((argc - 2) * sizeof (PROCFILE));
	hProc = (HANDLE*)malloc ((argc - 2) * sizeof (HANDLE));

	/* Create a separate "grep" process for each file on the
		command line. Each process also gets a temporary file
		name for the results; the handle is communicated through
		the STARTUPINFO structure. argv[1] is the search pattern. */

	for (iProc = 0; iProc < argc - 2; iProc++) {

		/* Create a command line of the form: grep argv[1] argv[iProc + 2] */
		/* if prog and/or arg have space in their names then use " "*/
		/*grep app can be stored where this prog is stored or in the current dir of this
		grepmp prog or path variable*/
		/*here .exe ext appended. Be careful if extenion is not .exe*/
		/*If appname in lpCommandLine does not contain a directory path, the system searches for the executable file in the following sequence:
			1. The directory from which the application loaded.
			2. The current directory for the parent process.
			3. The 32-bit Windows system directory. Use the GetSystemDirectory function to get thepath of this directory.
			4. The 16-bit Windows system directory. There is no function that obtains the path ofthis directory, but it is searched. The name of this directory is System.
			5. The Windows directory. Use the GetWindowsDirectory function to get the path of thisdirectory.
			6. The directories that are listed in the PATH environment variable. Note that thisfunction does not search the per-application path specified by the
				App Paths registry key*/
		_stprintf (commandLine, _T ("grep  \"%s\" \"%s\""), argv[1], argv[iProc + 2]);

		/* Create the temp file name for std output. */
		/*Creates a name for a temporary file. If a unique 
		file name is generated, an empty file iscreated and 
		the handle to it is released; otherwise, only a file 
		name is generated.*/
		if (GetTempFileName (_T ("."), //directory path
			_T ("gtm"), //prefix string
			0, //ensure unique filename and file is created
			procFile[iProc].tempFile //buffer store unique filename
		) == 0)  //3rd arg 0 ensures unique filename
			ReportError (_T ("Temp file failure."), 2, TRUE);

		/* Set the std output for the search process. */

		hTempFile = /* This handle is inheritable */
			CreateFile (procFile[iProc].tempFile,
				 GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTempFile == INVALID_HANDLE_VALUE)
			ReportError (_T ("Failure opening temp file."), 3, TRUE);

		/* Specify that the new process takes its std output
			from the temporary file's handles. */

		startUpSearch.dwFlags = STARTF_USESTDHANDLES;
		startUpSearch.hStdOutput = hTempFile;
		startUpSearch.hStdError = hTempFile;
		startUpSearch.hStdInput = GetStdHandle (STD_INPUT_HANDLE);
		/* Create a process to execute the command line. */

		if (!CreateProcess (NULL, //app name
			commandLine, //appname with args
			NULL, //security attributes for this new process 
			NULL, // security attributes for this new thread 
				TRUE, /*TRUE means each inheritable handle in the calling process is inherited by thenew process. If the*/
			dwCreationFlags, //NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP
			NULL, //environment block; NULL mean env block of parent
			NULL, //current dir for the new process. NULL means current dir of parent process
			&startUpSearch, 
			&processInfo))
			ReportError (_T ("ProcCreate failed."), 4, TRUE);
		/* Close unwanted handles */
		CloseHandle (hTempFile); 
		CloseHandle (processInfo.hThread);

		/* Save the process handle. */

		hProc[iProc] = processInfo.hProcess;
	}

	/* Processes are all running. Wait for them to complete, then output
		the results - in the order of the command line file names. */
	for (iProc = 0; iProc < argc-2; iProc += MAXIMUM_WAIT_OBJECTS)
		WaitForMultipleObjects (
			min(MAXIMUM_WAIT_OBJECTS, argc - 2 - iProc), //The number of objects handles in the array pointed to by lpHandles
				&hProc[iProc], //array of handles; passing address of 1st element
			TRUE, //wait for all objects
			INFINITE //The time-out interval, in milliseconds
		);

	/* Result files sent to std output using "cat".
		Delete each temporary file upon completion. */

	for (iProc = 0; iProc < argc - 2; iProc++) { 
		//grep exitCode = 0 when pattern matched in a file
		if (GetExitCodeProcess (hProc[iProc], &exitCode) && exitCode == 0) {
			/* Pattern was detected - List results. */
			/* List the file name if there is more than one file to search */
			if (argc > 3) _tprintf (_T("%s:\n"), argv[iProc+2]);
			_stprintf (commandLine, _T ("cat \"%s\""), procFile[iProc].tempFile);
			if (!CreateProcess (NULL, commandLine, NULL, NULL,
					TRUE, dwCreationFlags, NULL, NULL, &startUp, &processInfo))
				ReportError (_T ("Failure executing cat."), 0, TRUE);
			else { 
				WaitForSingleObject (processInfo.hProcess, INFINITE);
				CloseHandle (processInfo.hProcess);
				CloseHandle (processInfo.hThread);
			}
		}

		CloseHandle (hProc[iProc]);
		if (!DeleteFile (procFile[iProc].tempFile))
			ReportError (_T ("Cannot delete temp file."), 6, TRUE);
	}//for
	free (procFile); free (hProc);
	
	return 0;
}


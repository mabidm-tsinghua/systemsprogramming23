// console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdarg.h>	

VOID ReportError (LPCTSTR userMessage, DWORD exitCode, BOOL printErrorMessage);
BOOL ConsolePrompt (LPCTSTR pPromptMsg, LPTSTR pResponse, DWORD maxChar, BOOL echo);

int _tmain(int argc, LPCTSTR argv[])
{
	TCHAR res[256];
	ConsolePrompt (_T("Enter name: "), res, 256, TRUE);
	
	_tprintf(res);
	_tprintf(_T("\nEnd of Program\n"));

	getchar();
	return 0;

}

/* PrintMsg.c:
	ConsolePrompt
	PrintStrings
	PrintMsg */

									  
	//use it to print variable number of strings to a file or console
BOOL PrintStrings (HANDLE hOut, ...) 

/* Write the messages to the output handle. Frequently hOut
	will be standard out or error, but this is not required.
	Use WriteConsole (to handle Unicode) first, as the
	output will normally be the console. If that fails, use WriteFile.

	hOut:	Handle for output file. 
	... :	Variable argument list containing TCHAR strings.
		The list must be terminated with NULL. */
{
	DWORD msgLen, count;
	LPCTSTR pMsg;
	va_list pMsgList;	/* Current message string. */
	va_start (pMsgList, hOut);	/* Start processing msgs. */
	while ((pMsg = va_arg (pMsgList, LPCTSTR)) != NULL) {
		msgLen = _tcslen (pMsg);
		//WriteConsole can only write to console output buffer; 
		//WriteConsole can be used either as wide-character functions or as ANSI functions
		if (!WriteConsole (hOut, pMsg, msgLen, &count, NULL) 
				&& !WriteFile (hOut, pMsg, msgLen * sizeof (TCHAR),	&count, NULL)) {
			va_end (pMsgList);
			return FALSE;
		}
	}
	va_end (pMsgList);
	return TRUE;
}


BOOL PrintMsg (HANDLE hOut, LPCTSTR pMsg)

/* For convenience only - Single message version of PrintStrings so that
	you do not have to remember the NULL arg list terminator.

	hOut:	Handle of output file
	pMsg:	Single message to output. */
{
	return PrintStrings (hOut, pMsg, NULL);
}


BOOL ConsolePrompt (LPCTSTR pPromptMsg, LPTSTR pResponse, DWORD maxChar, BOOL echo)

/* Prompt the user at the console and get a response
	which can be up to maxChar generic characters.

	pPromptMessage:	Message displayed to user.
	pResponse:	Programmer-supplied buffer that receives the response.
	maxChar:	Maximum size of the user buffer, measured as generic characters.
	echo:		Do not display the user's response if this flag is FALSE. */
{
	HANDLE hIn, hOut;
	DWORD charIn, echoFlag;
	BOOL success;
	
	/*The CreateFile function enables a process to get a handle to its console's input
	buffer and active screen buffer, even if STDIN and STDOUT have been redirected. 
	To open a handle to a console's input buffer, specify the CONIN$ value in a 
	call to CreateFile. Specify the CONOUT$ value in a call to CreateFile 
	to open a handle to a console's active screen buffer. CreateFile enables 
	you to specify the read/write access of the handle that it returns.*/

	hIn = CreateFile (_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, 0,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hOut = CreateFile (_T("CONOUT$"), GENERIC_WRITE, 0,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	/* Should the input be echoed? */
	echoFlag = echo ? ENABLE_ECHO_INPUT : 0;

	/* API "and" chain. If any test or system call fails, the
		rest of the expression is not evaluated, and the
		subsequent functions are not called. GetStdError ()
		will return the result of the failed call. */
	
	//ENABLE_PROCESSED_INPUT: control key ctrl-c is handled by the system and not stored in the input buffer. So if we enter ctrl-c the os terminate the process.
	//if we remove ENABLE_PROCESSED_INPUT option and then press ctrl-c SYSTEM won't kill the process
	//ENABLE_PROCESSED_INPUT does not affect ctrl-break. ctrl-break is processed by system and kills the process.
	//ENABLE_LINE_INPUT: read until Enter key pressed; otherwise just read a single char
	//ENABLE_PROCESSED_OUTPUT: Backspace, tab, bell, carriage return, and line feed characters are processed

	//| ENABLE_PROCESSED_INPUT: when disabled ctrl-c is placed in input buffer. When disabled other control chars still processed by system
	success =  SetConsoleMode (hIn, ENABLE_LINE_INPUT | echoFlag | ENABLE_PROCESSED_INPUT) //remove this ENABLE_PROCESSED_INPUT and see the effect of ctrl+C
			&& SetConsoleMode (hOut, ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_PROCESSED_OUTPUT)
			&& PrintStrings (hOut, pPromptMsg, NULL)
			&& ReadConsole (hIn, pResponse, maxChar - 2, &charIn, NULL); //this also read carriage return and linefeed
	
	/* Replace the CR-LF by the null character. */
	if (success) {
		
		pResponse[charIn - 2] = _T('\0');
		
	}
	else
		ReportError (_T("ConsolePrompt failure."), 0, TRUE);

	CloseHandle (hIn);
	CloseHandle (hOut);
	return success;
}


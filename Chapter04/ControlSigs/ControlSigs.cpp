/* Chapter 4. CNTRLC.C */
/* Catch Cntrl-C signals. */



/*Each console process has its own list of application-defined
HandlerRoutine functions that handle CTRL+C and CTRL+BREAK signals.
These last handler added by SetConsoleCtrlHandler() called first.
If this handler returns FALSE then next handler is called otherwise not called.*/
/*The signal handler can perform cleanup operations just as an exception or termination
handler would. The signal handler can return TRUE to indicate that the
function handled the signal. If the signal handler returns FALSE , the next handler
function in the list is executed. The signal handlers are executed in the reverse order
from the way they were set, so that the most recently set handler is executed
first and the system handler is executed last.*/

#include "stdafx.h"

static BOOL WINAPI Handler(DWORD cntrlEvent);

static BOOL exitFlag = FALSE;

int _tmain(int argc, LPTSTR argv[])

/* Beep periodically until signaled to stop. */
{
	
	//// since ENABLE_PROCESSED_INPUT is not present, ctrl-c is not processed by os/application 
	//and placed in input buffer. However, ctrl-break is still working. 
	//If we uncomment following code then there is no point of having  handler for ctrl+C
	/*
	HANDLE hIn;
	hIn = CreateFile (_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, 0,
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetConsoleMode (hIn, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT  );
	*/	

	 //If we want calling process to ingore ctrl-c;
	//ctrl+c will not work until we restore ctrl+c using SetConsoleCtrlHandler(NULL, FALSE)
	if (!SetConsoleCtrlHandler(NULL, TRUE)) 
		ReportError(_T("Error setting event handler"), 1, TRUE);
		
		
	/* Add an event handler. */
	//more than one handler can be added.
	//default OS handler kills the app
	if (!SetConsoleCtrlHandler(Handler, TRUE))//TRUE means add handler
		ReportError(_T("Error setting event handler"), 1, TRUE);

	while (!exitFlag) { /* This flag is detected right after a beep, before a handler exits */
		//Suspends the execution of the current thread until the time-out interval elapses.
		Sleep(4750); 
		/* Beep every 5 seconds (4750+250 = 5000ms=5s); allowing 250 ms of beep time. */
		Beep(1000 /* Frequency */, 250 /* Duration */);
		
		 //restores normal processing of CTRL + C input
		if (!SetConsoleCtrlHandler(NULL, FALSE))
			ReportError(_T("Error setting event handler"), 1, TRUE);
		_tprintf(_T("Inside\n"));
	
	}
	_tprintf(_T("Stopping the main program as requested.\n"));
	return 0;
}	

//control-c and control-break kills the application

//When the signal is received, the system creates a new thread in the process to execute the handler function

//default handlers for ctrl-c and ctrl-break call ExitProcess().

BOOL WINAPI Handler(DWORD cntrlEvent)
{
	switch (cntrlEvent) { 
		/* The signal timing will determine if you see the second handler message */
		case CTRL_C_EVENT:
			_tprintf(_T("Ctrl-C received by handler. Leaving in 5 seconds or less.\n"));
			exitFlag = TRUE;
			return TRUE; /* TRUE indicates that the signal was handled. */
		case CTRL_BREAK_EVENT:
			_tprintf(_T("Ctrl-BREAK received by handler. Leaving in 5 seconds or less.\n"));
			exitFlag = TRUE;
			return TRUE; /* TRUE indicates that the signal was handled. */
		case CTRL_CLOSE_EVENT:
			_tprintf(_T("Close event received by handler. Leaving the handler in 5 seconds or less.\n"));
			exitFlag = TRUE;
			return TRUE; /* Try returning FALSE. Any difference? */
		default:
			_tprintf(_T("Event: %d received by handler. Leaving in 5 seconds or less.\n"), cntrlEvent);
			exitFlag = TRUE;
			//Sleep(4000); /* Decrease this time to get a different effect */
			//_tprintf(_T("Leaving handler in 1 seconds or less.\n"));
			return FALSE; /* TRUE indicates that the signal was handled. */
	}
}

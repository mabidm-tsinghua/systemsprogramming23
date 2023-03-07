/* Chapter 4.  Excption.c
	Generate exceptions intentionally and respond to them. */

#include "stdafx.h"
#include <float.h>

DWORD Filter (LPEXCEPTION_POINTERS, LPDWORD);

int _tmain (int argc, LPTSTR argv [])
{
	DWORD eCategory, i = 0, ix = 14, iy = 0;
	LPDWORD pNull = NULL;//(LPDWORD) 12
	BOOL done = FALSE;

	while (!done) {
		_tprintf (_T("Enter exception type:\n"));
		_tprintf (_T("1: Mem, 2: Int, 3: User 4: return\n"));
		_tscanf_s (_T("%d"), &i);
		__try { /* Try-Except block. */
			switch (i) {
			case 1: /* Memory reference. */
				*pNull = ix  ;
				*pNull = 5;
				break;
			case 2: /* Integer arithmetic. */
				ix = ix / iy;
				break;
			case 3: /* User generated exception. */
				ReportException (_T("Raising user exception.\n"), 1);
				break;
			case 4: /* Use the return statement to terminate. */
				return 1;
			default: done = TRUE;
			}
		} /* End of inner __try. */
		  //GetExceptionInformation() can be called only from within the filter expression of an exception handler.
		__except (Filter (GetExceptionInformation (), &eCategory)){
			
			switch (eCategory) {
			case 0:	_tprintf (_T("Unknown exception.\n"));
				break;
			case 1:	_tprintf (_T("Memory ref exception.\n"));
				break;
			case 2:	_tprintf (_T("Integer arithmetic exception.\n"));
				break;
			case 10: _tprintf (_T("User generated exception.\n"));
				break; 
			default: _tprintf (_T("Unknown exception.\n"));
				break;
			}
			_tprintf (_T("End of handler.\n"));

		} /* End of inner __try __except block. */

	} //while

	return 0;
}

static DWORD Filter (LPEXCEPTION_POINTERS pExP, LPDWORD eCategory)

/*	Categorize the exception and decide whether to continue execution or
	execute the handler or to continue the search for a handler that
	can process this exception type. The exception category is only used
	by the exception handler. */
{
	DWORD exCode;
	DWORD_PTR readWrite, virtAddr;
	exCode = pExP->ExceptionRecord->ExceptionCode;

	_tprintf (_T("Filter. exCode: %x\n"), exCode);

	if ((0x20000000 & exCode) != 0) { //bit 29 is set for user generated exception
				/* User Exception. */
		*eCategory = 10;
		return EXCEPTION_EXECUTE_HANDLER;
	}

	switch (exCode) {
		case EXCEPTION_ACCESS_VIOLATION:
				/* Determine whether it was a read, write, or execute
					and give the virtual address. */
			//0 means read, 1 means write, 8 means user-mode dataexecution prevention (DEP) violation.
			readWrite =
				(DWORD)(pExP->ExceptionRecord->ExceptionInformation [0]);
			//virtual address that caused the exception
			virtAddr =
				(DWORD)(pExP->ExceptionRecord->ExceptionInformation [1]);
			_tprintf
				(_T("Access Violation. Read/Write/Execute: %d. Address: %x\n"),
				readWrite, virtAddr);
			*eCategory = 1;
			return EXCEPTION_EXECUTE_HANDLER;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			*eCategory = 1;
			return EXCEPTION_EXECUTE_HANDLER;
					/* Integer arithmetic exception. Halt execution. */
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_OVERFLOW:
			*eCategory = 2;
			return EXCEPTION_EXECUTE_HANDLER;
					
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			*eCategory = 4;
			return EXCEPTION_CONTINUE_SEARCH;
		case STATUS_NONCONTINUABLE_EXCEPTION:
			*eCategory = 5;
			return EXCEPTION_EXECUTE_HANDLER;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
		case EXCEPTION_PRIV_INSTRUCTION:
			*eCategory = 6;
			return EXCEPTION_EXECUTE_HANDLER;
		case STATUS_NO_MEMORY:
			*eCategory = 7;
			return EXCEPTION_EXECUTE_HANDLER;
		default:
			*eCategory = 0;
			return EXCEPTION_CONTINUE_SEARCH;
	}
}

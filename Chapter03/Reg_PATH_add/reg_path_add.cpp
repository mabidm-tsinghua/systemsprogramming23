			

#include "stdafx.h"



int _tmain(int argc, LPTSTR argv[])
{
	/*short a, b;
	scanf("%2x%2x", &a, &b);
	printf("%x:%d, %x:%d",a,a,b,b);*/
	HKEY hKey;
	LONG result;
	DWORD valueType, index;
	DWORD numSubKeys, maxSubKeyLen, numValues, maxValueNameLen, maxValueLen;
	DWORD valueNameLen, valueLen;
	FILETIME lastWriteTime;
	LPTSTR subKeyName, valueName;
	LPBYTE value;

	if (argc < 2) {
		_tprintf(_T("Usage: reg_path_add exe_path\n"));
		return 1;
	}

	/*samdesired: A mask that specifies the desired access rights to the key to be opened. 
	The function fails if the security descriptor of the key does not permit 
	the requested access for the calling process.
	*/
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), 0, 
		KEY_READ | KEY_WRITE, //samdesired
		&hKey) != ERROR_SUCCESS)
		ReportError(_T("Cannot open subkey properly"), 2, TRUE);
	
	//Retrieves information about the specified registry key.
	
	if (RegQueryInfoKey(hKey, NULL, NULL, NULL,   
		&numSubKeys, &maxSubKeyLen, NULL, //number of subkeys and max SubKey Length.  maxSubKeyLen is char count not including \0
		&numValues, &maxValueNameLen, &maxValueLen, //maxValueLen in bytes
		NULL, &lastWriteTime) != ERROR_SUCCESS)
		ReportError(_T("Cannot query subkey information"), 3, TRUE);
	//subKeyName = (LPTSTR)malloc(TSIZE * (maxSubKeyLen + 1));   /* +1 for null character */
	valueName = (LPTSTR)malloc(TSIZE * (maxValueNameLen + 1));//+1 for null character
	value = (LPBYTE)malloc(maxValueLen);      /* size in bytes */

	_tprintf(_T("\nSubKey Count: %d, Values Count: %d\n"), numSubKeys, numValues); //by me for info


	for (index = 0; index < numValues; index++) {
		valueNameLen = maxValueNameLen + 1; /* A very common bug is to forget to set */
		valueLen = maxValueLen;     /* these values; both are in/out params  */

		//enumerates values for the specified key
		//The function copies one indexedvalue name and data block for the key each 
		//time it is called.

		result = RegEnumValue(hKey, index,
			//receives name of the val
			//valueNameLen: When the function returns, the variable receives the number of 
			// characters stored in the buffer, not including the terminating null character.
			valueName, &valueNameLen, //value name buffer and its length in characters including \0
			NULL,  //reserved
			&valueType, //received val type
			//receives val
			//When the function returns, the variable receives the number of
			//bytes stored in the buffer.
			value, &valueLen);//value buffer and its length in bytes

		
		if (result == ERROR_SUCCESS && _tcscmp(_T("Path"), valueName) == 0) {
			//TCHAR exe[] = _T("C:\\XYZ\\bin");
			int path_size = _tcslen(argv[1]) * TSIZE + valueLen + TSIZE;
			LPTSTR new_path = (LPTSTR)malloc(path_size);
			_stprintf(new_path, _T("%s;%s"), (LPTSTR)value, argv[1]);
			if (RegSetValueEx(hKey, valueName, 0, valueType, (LPBYTE)new_path, path_size) != ERROR_SUCCESS)
				ReportException(_T("PATH update failed \n"), 1);
			_tprintf(_T("PATH updated successfully\n"));

		}
	}

	RegCloseKey(hKey);

	return 0;
}


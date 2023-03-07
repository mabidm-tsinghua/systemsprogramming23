// OpReg.cpp : Defines the entry point for the console application.
//




/* OpREG [options] SubKey   //Subkey starts with predefined key name
	
	Options:
		-a	to create a new subkey
		-d  to delete a subkey (first we have to delete nested subkeys of a key then delete a key)
		-v  to add values for subkey
			*/


/* OpReg.exe -a HKEY_CURRENT_USER\dcis\systemprogramming 
   OpReg.exe -d HKEY_CUURENT_USER\dcis\systemprogramming
   OpReg.exe -a -v HKEY_CUURENT_USER\dcis\systemprogramming
   
 */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "stdafx.h"


int _tmain(int argc, LPTSTR argv[])
{
	BOOL addFlag, delFlag, valFlag;
	TCHAR keyName[MAX_PATH+1];
	DWORD i, keyIndex,dwDisposition;
	HKEY hKey,hSubKey;
	LPTSTR pScan;
	
	if (argc < 3) ReportError (_T("OpReg -a|-d|-v keyname"), 1, FALSE);
	keyIndex = Options(argc, (LPCTSTR*)argv, _T("adv"), &addFlag, &delFlag, &valFlag, NULL);

	
	/*cannot specify both add and del simul*/
	if( addFlag && delFlag )
	  ReportError(_T("Cannot specify both add and del options simul"), 2, FALSE);
	/*specify both key and value options simul*/
	if( !addFlag && valFlag )
	  ReportError(_T("specify both key (-a) and val options(-v) simul"), 2, FALSE);


	

	LPTSTR PreDefKeyNames[] = {
		_T("HKEY_LOCAL_MACHINE"),
		_T("HKEY_CLASSES_ROOT"),
		_T("HKEY_CURRENT_USER"),
		_T("HKEY_CURRENT_CONFIG"),
		NULL };
	HKEY PreDefKeys[] = {
		HKEY_LOCAL_MACHINE,
		HKEY_CLASSES_ROOT,
		HKEY_CURRENT_USER,
		HKEY_CURRENT_CONFIG };


	/* "Parse" the search pattern into two parts: the "key"
		and the "subkey". The key is the first back-slash terminated
		string and must be one of HKEY_LOCAL_MACHINE, HKEY_CLASSES_ROOT,HKEY_CURRENT_CONFIG
		or HKEY_CURRENT_USER. The subkey is everything else.
		The key and subkey names will be copied from argv[keyIndex]. */
	/*  Build the Key */

	pScan = argv[keyIndex];
	for (i = 0; *pScan != _T('\\') && *pScan != _T('\0') && i < MAX_PATH; pScan++, i++)
		keyName[i] = *pScan; //copy string
	
	keyName[i] = _T('\0'); //terminate string
	if (*pScan == _T('\\')) pScan++;

	//make predefined keyname upper case
	for (unsigned int c = 0; c < _tcslen(keyName); c++) {
		keyName[c] = _totupper(keyName[c]);

	}

	/* Translate predefined key name to an HKEY */
	for (	i = 0;
			PreDefKeyNames[i] != NULL && _tcscmp(PreDefKeyNames[i], keyName) != 0; //0 means equal
			i++) ;
	if (PreDefKeyNames[i] == NULL) ReportError(_T("Use a Predefined Key"), 1, FALSE);
	hKey = PreDefKeys[i]; //Predefine key handle.

	if(addFlag){
		/*REG_OPTION_NON_VOLATILE: This key is not volatile; this is the default.
		The informationis stored in a file and is preserved when the system is restarted.
		*/
		/*KEY_ALL_ACCESS: specifies the access rights for the key to be created
		*/
		//Creates the specified registry key. If the key already exists, the function opens it. 
		//Note thatkey names are not case sensitive
		if (RegCreateKeyEx(hKey,
			pScan, //can be up to 32 levels deep
			0,NULL,
			REG_OPTION_NON_VOLATILE,
			//https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights
			KEY_ALL_ACCESS , //the access rights for the key to be created
			NULL,
			&hSubKey,//receives a handle to the opened or created key.
			&dwDisposition // key created or opened
		)!= ERROR_SUCCESS) //
		   ReportError(_T("Cannot create subkey"), 2, TRUE);

		

		if (dwDisposition == REG_OPENED_EXISTING_KEY) {
			_tprintf(_T("%s already exists, is now opened.\n"), pScan);
		}
		else {
			_tprintf(_T("%s created, is now opened.\n"), pScan);
		}
		/*Add val name and its data if -v is specified*/
		if(valFlag){
			TCHAR valName[MAX_PATH],valData[MAX_PATH];
			_tprintf(_T("Enter name of a val: "));
			_getts_s(valName,MAX_PATH);
			_tprintf(_T("Enter data for a value: "));
			_getts_s(valData,MAX_PATH);
			
				/**/
			//RegSetValueEx() can also modify exisiting values
			/*Registry keys do not have default values, but they can have one unnamed value, 
			which canbe of any type.*/

				if (RegSetValueEx (hSubKey,valName,
					0, //reserved
					REG_SZ, //data type
					(BYTE*)valData, //data buffer
					TSIZE * _countof(valData) //size of data in bytes
				) != ERROR_SUCCESS) //
		                     ReportError(_T("Cannot add data against a subkey"), 2, TRUE);
				_tprintf(_T("%s data added \n"), valData);
					
		}


		RegCloseKey(hSubKey);
		
	
	}else if(delFlag){
		if (RegDeleteKey(hKey,pScan)!= ERROR_SUCCESS) //Deletes a subkey and its values; pScan cannot have subkeys.
		   ReportError(_T("Cannot delete subkey"), 2, TRUE);
		   
		_tprintf(_T("'%s' subkey deleted.\n"), pScan); 
		
		
	}else{
	
		_tprintf(_T("Not a valid options"));
	}
	
	

	
	return 0;
}


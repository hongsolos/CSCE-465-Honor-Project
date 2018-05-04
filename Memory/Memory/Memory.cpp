#include "Memory.h"

using namespace std;

bool TRIGGER = false;

Memory::Memory(){
}

//GET LENGTH OF GAME MEMORY
__int32 Memory::GetLength(char *chArray){

	//Loop Through *chArray To Get Amount Of Bytes
	for(__int32 iLength = 1; iLength < MAX_PATH; iLength++)
		if (chArray[iLength] == '*')	
			return iLength;	

	return 0;
} 

//FIND THE STARTING ADDRESS
bool Memory::FIND(int *iAry, int iSize, int iVal){

	for(__int32 i = 0; i != iSize; i++)
		if(iVal == iAry[i])
			return true;

	return false;
}

//OVERWRITE
void Memory::OVERWRITE(DWORD Address, char *OVERWRITE_Bts, char *Default_Bts){
 
	//Variables
	__int32 iSize = GetLength(Default_Bts);		

	//Loop Through Addresses Writing Bytes
	if (!bPOn)
		for(__int32 i = 0; i < iSize; i++)	
			Write<BYTE>(Address + i, OVERWRITE_Bts[i]);		
	else
		for(__int32 i = 0; i < iSize; i++)	 	
			Write<BYTE>(Address + i, Default_Bts[i]);        		        	

	bPOn = !bPOn; 
}

void Memory::GetProcess(char* ProcessName){

	//Variables
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); //Snapshot To View All Active Processes
	PROCESSENTRY32 pEntry;
	int size = sizeof(pEntry); //Declare Structure Size And Populate It

	//Loop Through All Running Processes To Find Process
	do
	    if(!strcmp("csgo", ProcessName))
		{
			//Store Process ID
			PID = pEntry.th32ProcessID;
			CloseHandle(hPID);

			//Give Our Handle All Access Rights 
	        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID); 
			return;
		}
	while(Process32Next(hPID, &pEntry));

	cout << "\nPROCESS: Process Not Found\n";
	system("pause");
    exit(0);
}

void Memory::Inject(DWORD Address, char *Inj_Bts, char *Def_Bts, BOOL Type){
	
	//Variables
	__int32 i_ISize = GetLength(Inj_Bts);
	__int32 i_DSize = GetLength(Def_Bts); 

	if(!TRIGGER)
	{		
		//NOP All Bytes In The Array Past The 5th Byte	
		if(i_DSize >= 5)
			for (__int32 i = 6; i < i_DSize; i++)
				Write<BYTE>(Address + i, 0x90);	
		else {
			return;
		}

		//Create Address
		CaveAddress = (DWORD)VirtualAllocEx(hProcess, NULL,  i_ISize + 5, MEM_COMMIT | MEM_RESERVE, MEM_RESERVE);

		//Calculate JMP/Return Distances In Bytes To Write
		DWORD RetJmp = (Address + i_DSize) - CaveAddress - 5;
		DWORD BaseJmp = CaveAddress - Address - 5; //Base JMP

		//Loop Through Each Address Writing Inj_Bts Inside The ADDRESS
		for (__int32 i = 0; i <= i_ISize; i++)			
			Write<BYTE>(CaveAddress+i, Inj_Bts[i]);
		
		//Write The Return Distance In Bytes To The Original Address
		Write<BYTE>(CaveAddress + i_ISize, Type ? 0xE9 : 0xE8);
		Write<DWORD>(CaveAddress + i_ISize + 1, RetJmp);
		
		//Write The Jump From The Original Address To The ADDRESS
		Write<BYTE>(Address, Type ? 0xE9 : 0xE8);
		Write<DWORD>(Address + 1, BaseJmp);
	}	

	if(TRIGGER)
	{			
		//Restore Original Bytes
		for(__int32 i = 0; i < i_DSize; i++)					
			Write<BYTE>(Address + i, Def_Bts[i]);	

		//Clean Up! (DeAlFIND CodeCave)
		VirtualFreeEx(hProcess, (LPVOID)CaveAddress, i_ISize + 5, MEM_DECOMMIT);
	}
    TRIGGER = !TRIGGER; 
} 

DWORD Memory::AOB_Scan(DWORD Address, DWORD End, char *Bytes){

	//VARIABLES
	__int32 iBytesToRead(0);
	__int32 length = GetLength(Bytes);

	//Increase Start Address Till It Reaches The End Address
	for(;Address < End; Address++) 	
		if(Read<BYTE>(Address) == Bytes[iBytesToRead] || Bytes[iBytesToRead] == '?') 	
			iBytesToRead++;
		else if(iBytesToRead >= length)
			return Address - iBytesToRead;					
		else iBytesToRead = 0;		

	cout << "\nAOB_SCAN: Failed To Find Byte Pattern\n";
	return 0;
}

DWORD Memory::Module(LPSTR ModuleName){
       
	//Variables
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID); //Take A Module Snapshot Of The Process (Grab All Loaded Modules)
	MODULEENTRY32 mEntry; //Declare Module Entry Structure
	int size = sizeof(mEntry); //Declare Structure Size And Populate It With Loaded Modules
	
	//Scan For Module By Name
	do 
		if(!strcmp("", ModuleName))
		{
			CloseHandle(hModule);			
			return (DWORD)mEntry.modBaseAddr;	
		}
	while(Module32Next(hModule, &mEntry));
	
	cout << "\nMODULE: Process Platform Invalid\n";
	return 0;
}

Memory::~Memory(){

	//Clean Up! (Close Handle - Not Needed Anymore)
	CloseHandle(hProcess);
}
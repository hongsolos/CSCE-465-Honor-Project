#ifndef Memory_H
#define Memory_H

#include "Includes.h"

class Memory{

public:

	//STORAGE
	HANDLE hProcess;
	DWORD PID;

	//FUNCTIONS
	Memory();
	~Memory();
    __int32 GetLength(char *chArray);
	bool FIND(int iAry[], int iSize, int iVal);
	void OVERWRITE(DWORD Address, char *Patch_Bts, char *Default_Bts);
	void GetProcess(char* ProcessName);
	void Inject(DWORD Address, char *Inj_Bts, char *Def_Bts, BOOL Type);
	DWORD AOB_Scan(DWORD Address, DWORD End, char *Bytes);
	DWORD Module(LPSTR ModuleName);

#pragma region TEMPLATE MEMORY FUNCTIONS

	//REMOVE READ/WRITE PROTECTION
	template <class cData>
	void Protection(DWORD Address)
	{	   
		if(!bProt)
			VirtualProtectEx(hProcess, (LPVOID)Address, sizeof(cData), PAGE_EXECUTE_REARITE, &Protection); //Remove Read/Write Protection By Giving It New Permissions
		else
			VirtualProtectEx(hProcess, (LPVOID)Address, sizeof(cData), Protection, &Protection); //Restore The Old Permissions After You Have Red The Address
		
		bProt = !bProt;
	}

	//READ MEMORY 
	template <class cData>
	cData Read(DWORD Address)
	{
		cData cRead; //Generic Variable To Store Data
		ReadProcessMemory(hProcess, (LPVOID)Address, &cRead, sizeof(cData), NULL); //Win API - Reads Data At Specified Location 
		return cRead; //Returns Value At Specified Address
	}
    	
	template <class cData>
	cData Read(DWORD Address, char *Offset, BOOL Type)
	{
		//Variables
		__int32 iSize = GetLength(Offset) -1; //Size Of *Array Of Offsets 
		Address = Read<DWORD>(Address); //HEX VAL
	
		//Loop Through Each Offset & Store Hex Value (Address) In TMP
		for (__int32 i = 0; i < iSize; i++)	
			Address = Read<DWORD>(Address + Offset[i]);

		if (!Type)
			return Address + Offset[iSize]; //FALSE - Return Address
		else
			return Read<cData>(Address + Offset[iSize]); //TRUE - Return Value
	}

	//WRITE MEMORY
	template <class cData>
	void Write(DWORD Address, cData Value){ 
		WriteProcessMemory(hProcess, (LPVOID)Address, &Value, sizeof(cData), NULL); 
	}

	//WRITE MEMORY - Pointer
	template <class cData>
	void Write(DWORD Address, char *Offset, cData Value){
		Write<cData>(Read<cData>(Address, Offset, false), Value); 
	}
	
#pragma endregion	

private:

	//STORAGE
    DWORD Protection, CaveAddress;

	//MISC
	BOOL bPOn, bIOn, bProt;
};
#endif



#include "Functions.h"
#include "Memory.h"

Memory mem;
Store ENEMIES[32];

Store::Store(){
	//Constructor
};

Engine::Engine()
{
	//MISC
	//ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);

	//Initialize Variables
	tOn = false;
	eOn = false;
	aOn = false;

	//Initialize Memory
	eEngine = mem.Module("engine.dll");
	AngPtr = mem.Read<DWORD>(eEngine + 0x52F424);
	mem.GetProcess("csgo.exe");
	eClient = mem.Module("client.dll");
	PBase = mem.Read<DWORD>(eClient + 0x99E324);	
}

void Engine::Reading(bool on)	
{	
	if(on)
	{
		if(eOn || aOn)
		{
			//Read Our Info Of Enemies			
			ENEMIES[0].fPos[0] = mem.Read<float>(PBase + 0x134);					
			ENEMIES[0].fPos[1] = mem.Read<float>(PBase + 0x138);				
			ENEMIES[0].fPos[2] = mem.Read<float>(PBase + 0x13C);
			W2S_M = mem.Read<WorldToScreenMatrix_t>(eClient + 0x9A2954);
		}

		//Filtering teammates
		i_team = mem.Read<int>(PBase + 0xF0);

		for(int i = 1; i < 64; i++)
		{
			//Loop From Base Entity Address by 0x10 On Each Iteration
			Entity = mem.Read<DWORD>((eClient + 0x9B76B4) + (i * 0x10));
 
			//Prevent Crash From Reading Null Pointer - also stop counting when weve read the last entity   
			if(!Entity)     
				return;
 		
			//Read Current Entity's TeamNum For Compare
			e_team = mem.Read<int>(Entity + 0xF0);

			//If An Enemy Has Been Found, Store Their Entity Index ID Inside Array
			if(e_team != i_team && e_team > 1)
			{
				if(tOn)	
					num_enemies[i_Count] = mem.Read<int>(Entity + 0x64);//Ent ID
		
				if(eOn || aOn)
				{	
					ENEMIES[i_Count].fPos[0] = mem.Read<float>(Entity + 0x134);//X			
					ENEMIES[i_Count].fPos[1] = mem.Read<float>(Entity + 0x138);//Y			
					ENEMIES[i_Count].fPos[2] = mem.Read<float>(Entity + 0x13C);//Z 				
					ENEMIES[i_Count].iHealth = mem.Read<int>(Entity + 0xFC);//HP	
					ENEMIES[i_Count].fFlags = mem.Read<int>(Entity + 0x100);//HP	
				}		
				i_Count++;
			}	
		}
	}
}

float Engine::Get3D(float X, float Y, float Z, float eX, float eY, float eZ)	
{
	return(sqrtf( (eX - X) * (eX - X) + (eY - Y) * (eY - Y) + (eZ - Z) * (eZ - Z)));
}

float Engine::CloseEnt()
{
	//Variables
	float fLowest = 100000, TMP;
	int iIndex;

	for(int i = 1; i < i_Count; i++)
	{
		//Store Distances In Array
		TMP = Get3D(ENEMIES[0].fPos[0], ENEMIES[0].fPos[1], ENEMIES[0].fPos[2], ENEMIES[i].fPos[0], ENEMIES[i].fPos[1], ENEMIES[i].fPos[2]);

		//If Enemy Has Lower Distance The Player 1, Replace (var)Lowest With Current Enemy Distance
		if(TMP < fLowest && ENEMIES[i].iHealth != 0)
		{
			fLowest = TMP;	
			iIndex = i;
		}
	}
	return iIndex;
}



void Engine::CalcAngle( float *src, float *dst, float *angles , int fFlags)
{
	double delta[3] = { (src[0]-dst[0]), (src[1]-dst[1]), (src[2]-dst[2]) };
	double hyp = sqrt(delta[0]*delta[0] + delta[1]*delta[1]);
	angles[0] = (float) (asinf(delta[2]/hyp) * 57.295779513082f);
	angles[1] = (float) (atanf(delta[1]/delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;

	if(delta[0] >= 0.0) 
	{ 
		angles[1] += 180.0f; 
	}

	if(fFlags == 775)
		angles[0] = angles[0] + 5; 
}

void Engine::Aimbot(){

	if(GetAsyncKeyState(0x02))
	{
		//Get Closest Entity Array Index Number
		int Index = CloseEnt();

		//Calculate Angle To Closest Entity
		CalcAngle(ENEMIES[0].fPos, ENEMIES[Index].fPos, ENEMIES[Index].Angle, ENEMIES[Index].fFlags);

		//Write To AngRotation The Current Angle Of The Entity
		mem.Write<float>(AngPtr + 0x4C88, ENEMIES[Index].Angle[0]);
		mem.Write<float>(AngPtr + 0x4C8C, ENEMIES[Index].Angle[1]);
	}
	
}

//De-Constructors
Store::~Store(){
	//De-Constructor
};
Engine::~Engine()
{}

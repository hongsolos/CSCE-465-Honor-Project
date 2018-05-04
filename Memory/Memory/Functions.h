#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Includes.h"

struct Store{
	Store();
	~Store();
public:
	bool tOn, eOn, aOn;
	int i_team, e_team, iHealth,  i_Count, fFlags;
	float fPos[3], Angle[3];
};

class Engine: public Store{
public:

#pragma region FUNCTION PROTOTYPES

	//Misc & Reading
	Engine();
	float Get3D(float X, float Y, float Z, float eX, float eY, float eZ);
	void Reading(bool on);
	~Engine();
	//Aim-Bot Angles
	void CalcAngle( float *src, float *dst, float *angles, int fFlags);
	float CloseEnt();
	void Aimbot();

#pragma endregion

#pragma region Variables

	//Misc Variables
	DWORD eClient, eEngine, PBase, Entity, Base, AngPtr;
	int num_enemies[32];
	
	//World To Screen Info
	typedef struct{
		float flMatrix [4][4];
	}WorldToScreenMatrix_t;

	WorldToScreenMatrix_t W2S_M;

#pragma endregion
};

#endif
#include "Functions.h"

__int32 main()
{
	//Initialize
	Engine keyop;
	
	while(1)
	{
		//Options
		//Auto shoot activate
		if(GetAsyncKeyState(VK_F1)&1)
		{
			keyop.tOn = !keyop.tOn;
		}

		if(GetAsyncKeyState(VK_F2)&1)
		{
			keyop.aOn = !keyop.aOn;
		}
		
		if(GetAsyncKeyState(VK_F3)&1)
		{
			keyop.eOn = !keyop.eOn;		
		}

		if(GetAsyncKeyState(VK_F4)&1)
			return 0;


		//Read keys
		keyop.i_Count = 1;
		keyop.Reading(keyop.eOn || keyop.aOn || keyop.tOn);

		//Functions
		keyop.Aimbot();

	}
	system("pause");
	return 0;
}


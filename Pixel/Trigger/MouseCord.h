#pragma once

#ifndef _MouseCord_h_
#define _MouseCord_h_

#include <iostream>
#include <string>
#include <vector>
#include <process.h>
#include <Windows.h>


using namespace std;

class MouseCord {
	public:
		/*----------------------------------//
		//MEMBER VARIABLE                   //
		//----------------------------------*/
		int X;
		int Y;

		/*----------------------------------//
		//CONSTRUCTORS                      //
		//----------------------------------*/
		MouseCord(int x, int y) {
			X = x;
			Y = y;
		};

		~MouseCord() {};

		/*----------------------------------//
		//OPERATION FUNCTIONS               //
		//----------------------------------*/



};

class Thread {
	public:
		HWND AppWnd;
		string GameWindow;
		Thread(HWND appWnd, string gameWindow) {
			AppWnd = appWnd;
			GameWindow = gameWindow;
		}
};
#endif
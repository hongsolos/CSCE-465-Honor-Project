#pragma once

#ifndef _ScreenCapture_h_
#define _ScreenCapture_h_

#include <iostream>
#include <string>
#include <vector>
#include <process.h>
#include <Windows.h>


using namespace std;

class ScreenCapture
{
public:

	/*----------------------------------//
	//MEMBER VARIABLE                   //
	//----------------------------------*/
	vector<int> vecA;
	BITMAP screen;
	RECT screenWindows;
	RGBQUAD* pixels;

	/*----------------------------------//
	//CONSTRUCTORS                      //
	//----------------------------------*/
	ScreenCapture(BITMAP screen_input, RECT screenWindows_input, RGBQUAD* pixels_input) {
		screen = screen_input;
		screenWindows = screenWindows_input;
		pixels = pixels_input;
	};

	~ScreenCapture() {};

	/*----------------------------------//
	//OPERATION FUNCTIONS               //
	//----------------------------------*/



};
#endif
// Trigger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "ScreenCapture.h"
#include "MouseCord.h"
#include <ctime>
#include <iostream>
#include <Wingdi.h>
#include <string>

using namespace std;

MouseCord currentMouse(0, 0);

//SETUP BITMAP TO STORE SCREEN INFORMATIONS
void SetupBitmapInfo(BITMAPINFO &bmi, int w, int h, int bitperpixels) {
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bitperpixels;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
}

//CAPTURE THE CURRENT STATE OF THE SCREEN
bool ScreenShot(string current, BITMAP &bm, HBITMAP &hbmap, BITMAPINFO &bmi, HDC &hdcShot, HBITMAP &hbitmapOld, HWND &window) {
	RECT rc;
	GetWindowRect(window, &rc);

	hdcShot = CreateCompatibleDC(0);
	hbmap = CreateCompatibleBitmap(GetDC(0), rc.right - rc.left, rc.bottom - rc.top);
	SelectObject(hdcShot, hbmap);

	BitBlt(hdcShot, 0, 0, rc.right - rc.left, rc.bottom - rc.top, GetDC(0), rc.left, rc.top, SRCCOPY);

	if (!GetObject(hbmap, sizeof(BITMAP), (LPSTR)&bm))
		return false;

	int bitsPerPixel = bm.bmBitsPixel;

	if (bitsPerPixel != 32 || bm.bmPlanes != 1)
		return false;

	SetupBitmapInfo(bmi, bm.bmWidth, bm.bmHeight, bitsPerPixel);
	return true;
}

//COMPARITOR FOR PIXEL
bool CompareColor(RGBQUAD* ppixels, int h, int w, int x, int y) {
	int p = (h - y - 1)*w + x;

	//cout << (int)ppixels[p].rgbRed << " " << (int)ppixels[p].rgbRed << " " << (int)ppixels[p].rgbRed << endl;
	//Sleep(20);
	
	if (((int)ppixels[p].rgbRed > 79 && (int)ppixels[p].rgbGreen > 72 && (int)ppixels[p].rgbBlue > 45) ||
		((int)ppixels[p].rgbRed < 83 && (int)ppixels[p].rgbGreen < 77 && (int)ppixels[p].rgbBlue < 49))
	{
		//system("pause");
		return true;
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		exit(0);
	}
	return false;
}

//AUTO TRIGGER
int Shoot(int x, int y) {
	mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
	return 0;
}


//SCANNING FUNCTION
void ScanBMP(ScreenCapture *scan) {
	//SCANNING IN Y DIRECTION
	for (int y = (scan->screenWindows.bottom - scan->screenWindows.top) / 4;
		y < ((scan->screenWindows.bottom - scan->screenWindows.top) - (scan->screenWindows.bottom - scan->screenWindows.top)/3.5);
			y++)
	{
		//SCANNING IN X DIRECTION
		for (int x = (scan->screenWindows.right - scan->screenWindows.left) / 4;
			x < ((scan->screenWindows.bottom - scan->screenWindows.top) - (scan->screenWindows.bottom - scan->screenWindows.top) / 3.5);
			x++)
		{
			SetCursorPos(x + scan->screenWindows.left, (y + 4) + scan->screenWindows.top);
			
			//PIXEL COMPARISION THRESHOLD -- OPTIMIZED
			if (CompareColor(scan->pixels, scan->screen.bmHeight, scan->screen.bmWidth, x, y)) {
				int z = x;
				while (z < (int)((scan->screenWindows.right - scan->screenWindows.left) - (scan->screenWindows.right - scan->screenWindows.left) / 4)) {
					if (!CompareColor(scan->pixels, scan->screen.bmHeight, scan->screen.bmWidth, z, y))
						break;
					z++;
				}

				//MOVE MOUSE TO COORDINATE AND PREPARE TO FIRE (OR NOT, DEPEND)
				//SetCursorPos((z - (z - x) / 2) + scan->screenWindows.left, (y + 4) + scan->screenWindows.top);
				POINT currentPos;
				GetCursorPos(&currentPos);


				//A BIT OF DETOUR
				if (currentPos.x < currentMouse.X + 4 && currentPos.x > currentMouse.X - 4
					&& currentPos.y < currentMouse.Y + 4
					&& currentPos.y > currentMouse.Y - 4) 
				{
					Shoot(x + scan->screenWindows.left, scan->screenWindows.top);
				}

				currentMouse.X = currentPos.x;
				currentMouse.Y = currentPos.y;
				return;
			}
		}
	}
}


//THIS IS THE MAIN FUNCTION FOR AIMBOT. IT TRIGGERS WHEN CONDITION IS MET (OR PIXEL IN THIS CASE)
bool Trigger(HWND window, string current) {
	RECT scrWindow;
	GetWindowRect(window, &scrWindow);
	BITMAP bm;
	HBITMAP hbmap;
	HBITMAP hbmapOld;
	BITMAPINFO bmi;
	HDC hdcShot;
	HDC hdcScreen;

	RGBQUAD* ppixels;

	int interval;

	//CONTINOUS SCREEN CAPTURE
	while (true) {
		if (!GetAsyncKeyState('X')) {
			interval = clock();
			if (!ScreenShot(current, bm, hbmap, bmi, hdcShot, hbmapOld, window)) {
				break;
			}


			//STORE INFO IN BITMAP FOR COMPARISON
			HBITMAP hbmapNew = CreateCompatibleBitmap(hdcShot, scrWindow.right - scrWindow.left, scrWindow.bottom - scrWindow.top);
			HDC hdcShotNew = CreateCompatibleDC(hdcShot);
			HBITMAP OldBmp = (HBITMAP)SelectObject(hdcShotNew, hbmapNew);
			BitBlt(hdcShotNew, 0, 0, scrWindow.right - scrWindow.left, scrWindow.bottom - scrWindow.top, hdcShot, 0, 0, SRCCOPY);

			ppixels = new RGBQUAD[bm.bmWidth * bm.bmHeight];
			if (!ppixels) {
				return false;
			}

			SelectObject(hdcShotNew, OldBmp);

			if (!GetDIBits(hdcShotNew, hbmapNew, 0, bm.bmHeight, ppixels, &bmi,DIB_RGB_COLORS)){
				ReleaseDC(window, hdcShot);
				delete[] ppixels;
				return false;
			}

			ReleaseDC(window, hdcShot);
			ScreenCapture scan(bm, scrWindow, ppixels);
			ScanBMP(&scan);

			//MEMORY LEAK PREVENTION
			if (ppixels)
				free(ppixels);
			SelectObject(hdcShotNew, hbmapOld);
			DeleteObject(hbmap);
			DeleteDC(hdcShot);
			DeleteObject(hbmapNew);
			DeleteObject(OldBmp);
			DeleteDC(hdcShotNew);
			
			cout << "Scanning took " << clock() - interval << "ms" << endl;
		}
	}
}

int main()
{
	//WINDOW'S NAME. TESTING ON PAINT
	string GameWindow = "test.jpg - Paint";
	HWND curWindow = FindWindow(0, GameWindow.c_str());
    
	//ERROR NO WINDOWS FOUND
	while (!curWindow) {
		system("CLS");
		curWindow = FindWindow(0, GameWindow.c_str());
		cout << "Unable to find" << endl;
		Sleep(500);
	}
	
	//GET CURRENT MOUSE POSITION
	POINT currentPos;
	GetCursorPos(&currentPos);

	//STORE THE MOUSE POSITIONS
	currentMouse.X = currentPos.x;
	currentMouse.Y = currentPos.y;

	//AIMBOT
	Trigger(curWindow, GameWindow);


	return 0;
}


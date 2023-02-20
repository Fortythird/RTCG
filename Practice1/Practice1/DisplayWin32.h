#pragma once
class DisplayWin32
{
public:
	int screenWidth;
	int screenHeight;
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HWND hWnd;
	WNDCLASSEX wc;

	DisplayWin32(int _screenWidth, int _screenHeight)
	{
		screenHeight = _screenHeight;
		screenWidth = _screenWidth;
	}

	void CreateScreen();
};



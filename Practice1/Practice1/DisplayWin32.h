#pragma once
class DisplayWin32
{
public:
	int screenWidth;
	int screenHeight;
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HWND hWnd;
	WNDCLASSEX wc;

	void CreateScreen(int _screenWidth, int _screenHeight);
};



#include "export.h"
#include "InputDevice.h"

InputDevice::InputDevice() {
	keys = new std::unordered_set<Keys>();
}

InputDevice::~InputDevice()
{
	delete keys;
}

void InputDevice::Init(HWND hWndArg)
{
	hWnd = hWndArg;

	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0;  
	Rid[0].hwndTarget = hWnd;

	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0;   
	Rid[1].hwndTarget = hWnd;

	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
	{
		auto errorCode = GetLastError();
		std::cout << "ERROR: " << errorCode << std::endl;
	}
}

void InputDevice::OnKeyDown(KeyboardInputEventArgs args)
{
	bool Break = (args.Flags & 0x80) >> 7;
	auto key = (Keys)args.VKey;

	if (args.MakeCode == 42) key = Keys::LeftShift;
	if (args.MakeCode == 54) key = Keys::RightShift;

	if (Break)
	{
		RemovePressedKey(key);
	}
	else
	{
		AddPressedKey(key);
	}
}

void InputDevice::OnMouseMove(RawMouseEventArgs args)
{
	//if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonDown)) {
	//	AddPressedKey(Keys::LeftButton);
	//}
	//
	//if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonUp)) {
	//	RemovePressedKey(Keys::LeftButton);
	//}
	//
	//if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonDown)) {
	//	AddPressedKey(Keys::RightButton);
	//}
	//
	//if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonUp)) {
	//	RemovePressedKey(Keys::RightButton);
	//}
	//
	//if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonDown)) {
	//	AddPressedKey(Keys::MiddleButton);
	//}
	//
	//if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonUp)) {
	//	RemovePressedKey(Keys::MiddleButton);
	//}
	//
	//POINT p;
	//GetCursorPos(&p);
	//ScreenToClient(hWnd, &p);

	//MouseEventArgs.Offset = Vector2(static_cast<float>(args.X), static_cast<float>(args.Y));
	//MouseEventArgs.Position = Vector2(p.x, p.y);
	//MouseEventArgs.WheelDelta = args.WheelDelta;

	//printf(
	//	" Mouse: posX=%04.4f posY:%04.4f offsetX:%04.4f offsetY:%04.4f, wheelDelta=%04d \n",
	//	MouseEventArgs.Position.x,
	//	MouseEventArgs.Position.y,
	//	MouseEventArgs.Offset.x,
	//	MouseEventArgs.Offset.y,
	//	MouseEventArgs.WheelDelta
	//);

	//MouseMove.Broadcast(MouseEventArgs);
}

void InputDevice::OnChangeScreenSize(int width, int height) {
	ScreenParam.Width = width;
	ScreenParam.Height = height;
	ChangeScreenSize.Broadcast(ScreenParam);
};

void InputDevice::AddPressedKey(Keys key)
{
	if (!keys->count(key))
		keys->insert(key);
}

void InputDevice::RemovePressedKey(Keys key)
{
	if (keys->count(key))
		keys->erase(key);
}

bool InputDevice::IsKeyDown(Keys key)
{
	return keys->count(key);
}
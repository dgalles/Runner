#pragma once
#pragma comment(lib, "XInput.lib")

// include files
#include <windows.h>
#include <XInput.h>


class XInputManager
{
public:

	DWORD controller_OK[4];
	XINPUT_STATE state[4];   


	void update();


};
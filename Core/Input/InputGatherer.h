#include "Core/Input/rawinputconstants.h"
#include <bitset>

#include <windows.h>
#include <dinput.h>

class WindowsInputGatherer {
	public:
	WindowsInputGatherer(HINSTANCE, HWND);
	~WindowsInputGatherer();
	void updateKeyboard(std::bitset<KEYBOARD_KEYS::__SIZE__>& keyboardState);
	void updateMouse(float& dx, float& dy, float& dwheel, std::bitset<4>& buttons);

	private:
	LPDIRECTINPUT8 DirectInput;
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;

	HWND hwnd;
};

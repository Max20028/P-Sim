#include "Core/Input/InputGatherer.h"
#include <map>

WindowsInputGatherer::WindowsInputGatherer(HINSTANCE hInstance, HWND hwnd) {
    DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);

    DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);

    DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

    DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    DIMouse->SetDataFormat(&c_dfDIMouse);

    //Setup mouse
    //This version has mouse invisible
    DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
    //This version has mouse visible. We want it visible for picking
    // DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
    this->hwnd = hwnd;
}
WindowsInputGatherer::~WindowsInputGatherer() {
    DIKeyboard->Release();
    DIMouse->Release();
    DirectInput->Release();
}

void WindowsInputGatherer::updateKeyboard(std::bitset<KEYBOARD_KEYS::__SIZE__>& outputKeyboardState) {
    //Update Keyboard
    static std::map<KEYBOARD_KEYS::KEYBOARD_KEYS, int> mapKeyToDI {
        {KEYBOARD_KEYS::A, DIK_A},{KEYBOARD_KEYS::B, DIK_B},{KEYBOARD_KEYS::C, DIK_C},{KEYBOARD_KEYS::D, DIK_D},
        {KEYBOARD_KEYS::E, DIK_E},{KEYBOARD_KEYS::F, DIK_F},{KEYBOARD_KEYS::G, DIK_G},{KEYBOARD_KEYS::H, DIK_H},
        {KEYBOARD_KEYS::I, DIK_I},{KEYBOARD_KEYS::J, DIK_J},{KEYBOARD_KEYS::K, DIK_K},{KEYBOARD_KEYS::L, DIK_L},
        {KEYBOARD_KEYS::M, DIK_M},{KEYBOARD_KEYS::N, DIK_N},{KEYBOARD_KEYS::O, DIK_O},{KEYBOARD_KEYS::P, DIK_P},
        {KEYBOARD_KEYS::Q, DIK_Q},{KEYBOARD_KEYS::R, DIK_R},{KEYBOARD_KEYS::S, DIK_S},{KEYBOARD_KEYS::T, DIK_T},
        {KEYBOARD_KEYS::U, DIK_U},{KEYBOARD_KEYS::V, DIK_V},{KEYBOARD_KEYS::W, DIK_W},{KEYBOARD_KEYS::X, DIK_X},
        {KEYBOARD_KEYS::Y, DIK_Y},{KEYBOARD_KEYS::Z, DIK_Z}
    };

    BYTE keyboardState[256];

    DIKeyboard->Acquire();

    DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

    if(keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(hwnd, WM_DESTROY, 0, 0);

    outputKeyboardState.reset();
    for(auto const& in : mapKeyToDI) {
        KEYBOARD_KEYS::KEYBOARD_KEYS key = in.first;
        int DIindex = in.second;
        if(keyboardState[DIindex] & 0x80)
            outputKeyboardState.set(key);
    }

    return;
}

void WindowsInputGatherer::updateMouse(float &dx, float& dy, float& dwheel, std::bitset<4>& buttons) {
    DIMOUSESTATE mouseCurrState;
    DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
    buttons.reset();
    for(int i = 0; i < 4; i++){
        if(mouseCurrState.rgbButtons[i])
            buttons.set(i);
    }
    dx = mouseCurrState.lX;
    dy = mouseCurrState.lY;
    dwheel = mouseCurrState.lZ;
}

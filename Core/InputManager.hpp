#include "Core/Input/rawinputconstants.h"
#include "Core/RobotMap.h"
#include <bitset>
class InputManager {
    public:
    InputManager();
    ~InputManager();
    //----------------
    //Polling Section:
    //These functions are used for polling input. Should not be main method of accessing input but here in case it ever comes up
    bool getKeyboardKey(KEYBOARD_KEYS::KEYBOARD_KEYS key) {
        return keyboardState[key];
    }
    float getMouseChangeX(){
        return lastMouseState.dx;
    }
    float getMouseChangeY(){
        return lastMouseState.dy;
    }
    float getMouseChangeWheel(){
        return lastMouseState.dWheel;
    }
    bool getMouseButton(int button){
        if(button < 0 || button > MAX_MOUSE_BUTTONS)
            return 0;
        return lastMouseState.buttons[button];
    }
    //---------------
    void update() {
        //Update the States
    }
    private:
    std::bitset<KEYBOARD_KEYS::__SIZE__> keyboardState;
    struct MouseState {
        float dx;
        float dy;
        float dWheel;
        std::bitset<MAX_MOUSE_BUTTONS> buttons;
    };
    MouseState lastMouseState;
};

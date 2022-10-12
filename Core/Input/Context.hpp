#include <vector>
#include <string>
#include "Core/EventManager.hpp"
#include "Core/Input/rawinputconstants.h"
struct Action {
    std::string name;
    bool act;
    KEYBOARD_KEYS::KEYBOARD_KEYS key; //THIS WONT WORK: What if we want to bind mouse keys. Do both? use a super class to allow both?
    bool press; //Is it press or hold?
};
struct State {
    std::string name;
    bool enabled;
    bool hold;
};
struct Range {
    std::string name;
    double val;
};

class Context {
    private:
        std::vector<Action> actions;
        std::vector<State> states;
        std::vector<Range> ranges;
    public:
        Context();
        ~Context();
        void addAction() {
        }
        void addState() {
        }
};

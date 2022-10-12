//Keys to standardize accessing hardware values. Should not be modified by game unless wanting to modify the engine itself

namespace KEYBOARD_KEYS {
enum KEYBOARD_KEYS {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z, //Letters
										F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,              //Function Keys
										B1,B2,B3,B4,B5,B6,B7,B8,B9,B0,                       //Numbers at top of keyboard
										CTRL,SHIFT,CAPSLOCK,ALT,TAB,                				 //Mod Keys
										BACKSPACE,ESCAPE,LBRACK,RBRACK,SEMICOLON,COMMA,PERIOD,
										SLASH,BACKSLASH,MINUS,EQUALS,                        //MISC keys
										UPARROW,DOWNARROW,LEFTARROW,RIGHTARROW,							 //Arrow keys
                    __SIZE__};                                  //STORES SIZE OF ENUM. KEEP AS LAST ELEMENT!
}
namespace CONTROLLER_BUTTONS{
enum CONTROLLER_BUTTONS {A,B,X,Y,UPARROW,DOWNARROW,LEFTARROW,RIGHTARROW,
												 LB,RB,LT,RT,LSTICK,RSTICK,
												 START,BACK,HOME,
                         __SIZE__};
}
namespace CONTROLLER_AXES {
enum CONTROLLER_AXES {LX,LY,RX,RY,LT,RT,__SIZE__};
}


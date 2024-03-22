#include "gamecontroller.h"

GameController::GameController()
{
    gc=NULL;
    axis_deadzone=10000;

}

GameController::~GameController()
{
    close();
}
void GameController::close()
{
    if (gc) SDL_GameControllerClose(gc);
    gc=NULL;
}

void GameController::open(const Device& dev)
{
    open(dev.id);
}

void GameController::open(int sdl_id)
{
    close();
    gc=SDL_GameControllerOpen(sdl_id);
    if (!gc) {
        return;
    }
    has_axis=0;
    if (SDL_GameControllerHasAxis(gc, SDL_CONTROLLER_AXIS_LEFTX)) has_axis|=static_cast<uint32_t>(Axis::leftx);
    if (SDL_GameControllerHasAxis(gc, SDL_CONTROLLER_AXIS_LEFTY)) has_axis|=static_cast<uint32_t>(Axis::lefty);
    if (SDL_GameControllerHasAxis(gc, SDL_CONTROLLER_AXIS_RIGHTX)) has_axis|=static_cast<uint32_t>(Axis::rightx);
    if (SDL_GameControllerHasAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY)) has_axis|=static_cast<uint32_t>(Axis::righty);
    if (SDL_GameControllerHasAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) has_axis|=static_cast<uint32_t>(Axis::triggerx);
    if (SDL_GameControllerHasAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) has_axis|=static_cast<uint32_t>(Axis::triggery);

    has_button=0;
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_A)) has_button|=static_cast<uint32_t>(Button::a);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_B)) has_button|=static_cast<uint32_t>(Button::b);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_X)) has_button|=static_cast<uint32_t>(Button::x);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_Y)) has_button|=static_cast<uint32_t>(Button::y);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_BACK)) has_button|=static_cast<uint32_t>(Button::back);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_GUIDE)) has_button|=static_cast<uint32_t>(Button::guide);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_START)) has_button|=static_cast<uint32_t>(Button::start);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_LEFTSTICK)) has_button|=static_cast<uint32_t>(Button::leftstick);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSTICK)) has_button|=static_cast<uint32_t>(Button::rightstick);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) has_button|=static_cast<uint32_t>(Button::leftshoulder);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) has_button|=static_cast<uint32_t>(Button::rightshoulder);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP)) has_button|=static_cast<uint32_t>(Button::dpad_up);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) has_button|=static_cast<uint32_t>(Button::dpad_down);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) has_button|=static_cast<uint32_t>(Button::dpad_left);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) has_button|=static_cast<uint32_t>(Button::dpad_right);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_MISC1)) has_button|=static_cast<uint32_t>(Button::misc1);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_PADDLE1)) has_button|=static_cast<uint32_t>(Button::paddle1);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_PADDLE2)) has_button|=static_cast<uint32_t>(Button::paddle2);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_PADDLE3)) has_button|=static_cast<uint32_t>(Button::paddle3);
    if (SDL_GameControllerHasButton(gc, SDL_CONTROLLER_BUTTON_PADDLE4)) has_button|=static_cast<uint32_t>(Button::paddle4);


    has_led=SDL_GameControllerHasLED(gc);
    has_rumble=SDL_GameControllerHasRumble(gc);
    has_rumble_triggers=SDL_GameControllerHasRumbleTriggers(gc);

    SDL_GameControllerEventState(SDL_ENABLE);

    //printf("has_axis: %x, has_button: %x, has_led: %d, has_rumble: %d, has_rumble_triggers: %d\n", has_axis, has_button, has_led, has_rumble, has_rumble_triggers);

}



bool GameController::isOpen() const
{
    if (gc) return true;
    return false;
}

void GameController::setDeadzone(int value)
{
    int v=abs(value);
    if (v < 30000) axis_deadzone=v;
}

int GameController::deadzone() const
{
    return axis_deadzone;
}

ppl7::String GameController::name() const
{
    if (gc) return "";
    return SDL_GameControllerName(gc);
}


int GameController::getButtonState(int button) const
{
    if (gc) return SDL_GameControllerGetButton(gc, (SDL_GameControllerButton)button);
    return 0;
}

int GameController::getAxisState(int axis) const
{
    if (gc) {
        int value=SDL_GameControllerGetAxis(gc, (SDL_GameControllerAxis)axis);
        if (value > -axis_deadzone && value < axis_deadzone) value=0;
        return value;
    }
    return 0;
}



std::list<GameController::Device> GameController::enumerate()
{
    std::list<GameController::Device> device_list;
    int num=SDL_NumJoysticks();
    if (num > 0) {
        for (int i=0;i < num;i++) {
            SDL_GameController* gc=SDL_GameControllerOpen(i);
            if (gc) {
                GameController::Device d;
                d.id=i;
                d.name.set(SDL_GameControllerName(gc));
                device_list.push_back(d);
            }
        }
    }
    return device_list;
}

void GameController::rumbleTrigger(uint16_t left, uint16_t right, uint32_t duration_ms)
{
    if (has_rumble_triggers == true && gc != NULL)  SDL_GameControllerRumbleTriggers(gc, left, right, duration_ms);
}

void GameController::rumble(uint16_t low, uint16_t high, uint32_t duration_ms)
{
    if (has_rumble == true && gc != NULL)  SDL_GameControllerRumble(gc, low, high, duration_ms);
}


ppl7::String GameController::getAxisName(int axis)
{
    switch (axis) {
        case SDL_CONTROLLER_AXIS_LEFTX: return ppl7::String("Stick left horizontal");
            break;
        case SDL_CONTROLLER_AXIS_LEFTY: return ppl7::String("Stick left vertical");
            break;
        case SDL_CONTROLLER_AXIS_RIGHTX: return ppl7::String("Stick right horizontal");
            break;
        case SDL_CONTROLLER_AXIS_RIGHTY: return ppl7::String("Stick right vertical");
            break;
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return ppl7::String("Trigger left");
            break;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return ppl7::String("Trigger right");
            break;
    }
    return ppl7::String("unknown");

}

ppl7::String GameController::getButtonName(int button)
{
    switch (button) {
        case SDL_CONTROLLER_BUTTON_A: return ppl7::String("Button A");
            break;
        case SDL_CONTROLLER_BUTTON_B: return ppl7::String("Button B");
            break;
        case SDL_CONTROLLER_BUTTON_X: return ppl7::String("Button X");
            break;
        case SDL_CONTROLLER_BUTTON_Y: return ppl7::String("Button Y");
            break;
        case SDL_CONTROLLER_BUTTON_BACK: return ppl7::String("Button back");
            break;
        case SDL_CONTROLLER_BUTTON_GUIDE: return ppl7::String("Button guide");
            break;
        case SDL_CONTROLLER_BUTTON_START: return ppl7::String("Button start");
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return ppl7::String("Stick left");
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return ppl7::String("Stick right");
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return ppl7::String("Shoulder left");
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return ppl7::String("Shoulder right");
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return ppl7::String("Pad up");
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return ppl7::String("Pad down");
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return ppl7::String("Pad left");
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return ppl7::String("Pad right");
            break;
        case SDL_CONTROLLER_BUTTON_MISC1: return ppl7::String("Button misc");
            break;
        case SDL_CONTROLLER_BUTTON_PADDLE1: return ppl7::String("Paddle 1");
            break;
        case SDL_CONTROLLER_BUTTON_PADDLE2: return ppl7::String("Paddle 2");
            break;
        case SDL_CONTROLLER_BUTTON_PADDLE3: return ppl7::String("Paddle 3");
            break;
        case SDL_CONTROLLER_BUTTON_PADDLE4: return ppl7::String("Paddle 4");
            break;

    }
    return ppl7::String("unknown");
}



GameControllerMapping::GameControllerMapping()
{
    player_axis_x=SDL_CONTROLLER_AXIS_LEFTX;
    player_axis_y=SDL_CONTROLLER_AXIS_LEFTY;
    player_axis_crouch=SDL_CONTROLLER_AXIS_TRIGGERLEFT;

    menu_button_up=SDL_CONTROLLER_BUTTON_DPAD_UP;
    menu_button_down=SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    menu_button_left=SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    menu_button_right=SDL_CONTROLLER_BUTTON_DPAD_RIGHT;

    menu_button=SDL_CONTROLLER_BUTTON_START;
    action_button=SDL_CONTROLLER_BUTTON_A;
    back_button=SDL_CONTROLLER_BUTTON_B;
    jump_button=SDL_CONTROLLER_BUTTON_X;
    flashlight_button=SDL_CONTROLLER_BUTTON_Y;
    crouch_button=SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    updateMapping();
}

void GameControllerMapping::updateMapping()
{
    button_mapping.clear();
    axis_mapping.clear();
    button_mapping_rev.clear();
    axis_mapping_rev.clear();

    axis_mapping.insert(std::pair<int, Axis>(player_axis_x, Axis::Walk));
    axis_mapping.insert(std::pair<int, Axis>(player_axis_y, Axis::Jump));
    axis_mapping.insert(std::pair<int, Axis>(player_axis_crouch, Axis::Crouch));
    button_mapping.insert(std::pair<int, Button>(menu_button_up, Button::MenuUp));
    button_mapping.insert(std::pair<int, Button>(menu_button_down, Button::MenuDown));
    button_mapping.insert(std::pair<int, Button>(menu_button_left, Button::MenuLeft));
    button_mapping.insert(std::pair<int, Button>(menu_button_right, Button::MenuRight));
    button_mapping.insert(std::pair<int, Button>(menu_button, Button::Menu));
    button_mapping.insert(std::pair<int, Button>(action_button, Button::Action));
    button_mapping.insert(std::pair<int, Button>(jump_button, Button::Jump));
    button_mapping.insert(std::pair<int, Button>(back_button, Button::Back));
    button_mapping.insert(std::pair<int, Button>(flashlight_button, Button::Flashlight));
    button_mapping.insert(std::pair<int, Button>(crouch_button, Button::Crouch));

    axis_mapping_rev.insert(std::pair<Axis, int>(Axis::Walk, player_axis_x));
    axis_mapping_rev.insert(std::pair<Axis, int>(Axis::Jump, player_axis_y));
    axis_mapping_rev.insert(std::pair<Axis, int>(Axis::Crouch, player_axis_crouch));
    button_mapping_rev.insert(std::pair<Button, int>(Button::MenuUp, menu_button_up));
    button_mapping_rev.insert(std::pair<Button, int>(Button::MenuDown, menu_button_down));
    button_mapping_rev.insert(std::pair<Button, int>(Button::MenuLeft, menu_button_left));
    button_mapping_rev.insert(std::pair<Button, int>(Button::MenuRight, menu_button_right));
    button_mapping_rev.insert(std::pair<Button, int>(Button::Menu, menu_button));
    button_mapping_rev.insert(std::pair<Button, int>(Button::Action, action_button));
    button_mapping_rev.insert(std::pair<Button, int>(Button::Jump, back_button));
    button_mapping_rev.insert(std::pair<Button, int>(Button::Back, jump_button));
    button_mapping_rev.insert(std::pair<Button, int>(Button::Flashlight, flashlight_button));
    button_mapping_rev.insert(std::pair<Button, int>(Button::Crouch, crouch_button));

}

GameControllerMapping::Button GameControllerMapping::getButton(const ppl7::tk::GameControllerButtonEvent* event) const
{
    std::map<int, Button>::const_iterator it;
    it=button_mapping.find(event->button);
    if (it != button_mapping.end()) return it->second;
    return Button::Unknown;
}

GameControllerMapping::Axis GameControllerMapping::getAxis(const ppl7::tk::GameControllerAxisEvent* event) const
{
    std::map<int, Axis>::const_iterator it;
    it=axis_mapping.find(event->axis);
    if (it != axis_mapping.end()) return it->second;
    return Axis::Unknown;
}

int GameControllerMapping::getSDLAxis(const Axis a)
{
    std::map<Axis, int>::const_iterator it;
    it=axis_mapping_rev.find(a);
    if (it != axis_mapping_rev.end()) return it->second;
    return SDL_CONTROLLER_AXIS_INVALID;
}

int GameControllerMapping::getSDLButton(const Button b)
{
    std::map<Button, int>::const_iterator it;
    it=button_mapping_rev.find(b);
    if (it != button_mapping_rev.end()) return it->second;
    return SDL_CONTROLLER_BUTTON_INVALID;
}

void GameControllerMapping::setMappingAxis(const Axis a, int id)
{
    switch (a) {
        case Axis::Walk: player_axis_x=id; break;
        case Axis::Jump: player_axis_y=id; break;
        case Axis::Crouch: player_axis_crouch=id; break;
        default: break;
    }
}

void GameControllerMapping::setMappingButton(const Button a, int id)
{
    switch (a) {
        case Button::MenuUp: menu_button_up=id; break;
        case Button::MenuDown: menu_button_down=id; break;
        case Button::MenuLeft: menu_button_left=id; break;
        case Button::MenuRight: menu_button_right=id; break;
        case Button::Menu: menu_button=id; break;
        case Button::Action: action_button=id; break;
        case Button::Flashlight: flashlight_button=id; break;
        case Button::Jump: back_button=id; break;
        case Button::Back: jump_button=id; break;
        case Button::Crouch: crouch_button=id; break;
        default: break;
    }

}

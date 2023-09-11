#include "gamecontroller.h"

GameController::GameController()
{
    gc=NULL;


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
    close();
    gc=SDL_GameControllerOpen(dev.id);
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

    printf("has_axis: %x, has_button: %x\n", has_axis, has_button);

}

void GameController::processEvent(ppl7::tk::GameControllerAxisEvent* event)
{
    //if (event->axis==0)
}

void GameController::processEvent(ppl7::tk::GameControllerButtonEvent* event)
{

}


bool GameController::isOpen() const
{
    if (gc) return true;
    return false;
}

void GameController::update()
{

}

bool GameController::left() const
{
    return false;
}

bool GameController::right() const
{
    return false;
}

bool GameController::up() const
{
    return false;
}

bool GameController::down() const
{
    return false;
}

bool GameController::shift() const
{
    return false;
}

bool GameController::action() const
{
    return false;
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

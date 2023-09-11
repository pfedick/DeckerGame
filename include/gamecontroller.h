#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>

#include <SDL.h>

class GameController {
private:
    SDL_GameController* gc;
    bool has_axis_leftx=false;
    bool has_axis_lefty=false;
    bool has_axis_rightx=false;
    bool has_axis_righty=false;
    bool has_axis_triggerx=false;
    bool has_axis_triggery=false;

    uint32_t has_button=0;
    uint32_t has_axis=0;
    bool has_led=false;
    bool has_rumble=false;
    bool has_rumble_triggers=false;



public:
    class Device {
    public:
        int id;
        ppl7::String name;
    };
    enum class Axis {
        leftx			= 0x0001,
        lefty			= 0x0002,
        rightx			= 0x0004,
        righty			= 0x0008,
        triggerx		= 0x0010,
        triggery		= 0x0020
    };

    enum class Button {
        a				= 0x00000001,
        b				= 0x00000002,
        x				= 0x00000004,
        y				= 0x00000008,
        back			= 0x00000010,
        guide			= 0x00000020,
        start			= 0x00000040,
        leftstick		= 0x00000080,
        rightstick		= 0x00000100,
        leftshoulder	= 0x00000200,
        rightshoulder	= 0x00000400,
        dpad_up			= 0x00000800,
        dpad_down		= 0x00001000,
        dpad_left		= 0x00002000,
        dpad_right		= 0x00004000,
        misc1			= 0x00008000,
        paddle1			= 0x00010000,
        paddle2			= 0x00020000,
        paddle3			= 0x00040000,
        paddle4			= 0x00080000
    };
    GameController();
    GameController(const Device& dev);
    ~GameController();
    void open(const Device& dev);

    void close();

    bool isOpen() const;
    void processEvent(ppl7::tk::GameControllerAxisEvent* event);
    void processEvent(ppl7::tk::GameControllerButtonEvent* event);
    void update();
    bool left() const;
    bool right() const;
    bool up() const;
    bool down() const;
    bool shift() const;
    bool action() const;


    static std::list<Device> enumerate();
};

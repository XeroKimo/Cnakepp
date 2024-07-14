#include "bn_core.h"
#include "Display.hpp"
#include "Input.hpp"
#include "SnakeScene.hpp"
#include <bn_random.h>

bn::random defaultRandomGenerator;

int main()
{
    //For now use bncore to get access to the assert
    bn::core::init();
    //Clear DisplayControlStatus for now as bn::core::init enables some of the stuff
    //making SetBackgroundMode not work as intended, without bn::core::init, force blank flag is enabled by default for some reason
    cgba::Display::GetControlRegister().HideObjectWindow();
    cgba::Display::GetControlRegister().HideWindow0();
    cgba::Display::GetControlRegister().HideWindow1();

    while(1)
    {
        SnakeScene();
    }

    return 0;
}

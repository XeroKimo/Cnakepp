#include "bn_core.h"
#include "Display.hpp"

int main()
{
    //For now use bncore to get access to the assert
    bn::core::init();
    //Clear DisplayControlStatus for now as bn::core::init enables some of the stuff
    //making SetBackgroundMode not work as intended, without bn::core::init, force blank flag is enabled by default for some reason
    cgba::Display::GetControlRegister().HideObjectWindow();
    cgba::Display::GetControlRegister().HideWindow0();
    cgba::Display::GetControlRegister().HideWindow1();
    // cgba::Display::GetControlRegister() = cgba::DisplayControlRegister{};
    // cgba::Display::HideObjectWindow();
    // cgba::Display::HideWindow0();
    // cgba::Display::HideWindow1();
    
    
    // SetBackgroundMode<cgba::BackgroundMode0>();
    auto& backgroundMode = cgba::Display::SetBackgroundMode<cgba::BackgroundMode3>();

    auto background = backgroundMode.GetBackground2();
    background.Show();
    // BN_ASSERT((cgba::GetDisplayControlStatus() & cgba::DisplayControlStatus::Background_Mode_Mask) == cgba::BackgroundMode3::modeValue);
    
    background.PlotPixel({120, 80}, {255, 0, 0});
    background.PlotPixel({136, 80}, {0, 255, 0});
    background.PlotPixel({120, 96}, {0, 0, 255});
        
    while(1)
    {
    }

    return 0;
    // bn::core::init();

    // while(true)
    // {
    //     bn::core::update();
    // }
}

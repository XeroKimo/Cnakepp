#include "bn_core.h"
#include "DisplayControlRegister.hpp"

template<class Ty>
Ty SetBackgroundMode()
{
    cgba::DisplayControlRegister currentStatus = cgba::GetDisplayControlRegister();
    currentStatus &= ~cgba::DisplayControlRegister::Background_Mode_Mask;
    currentStatus |= Ty::modeValue | Ty::backgroundLayerSupport;
    cgba::GetDisplayControlRegister() = currentStatus;
    return Ty{};
}
// int main()
// {
//     cgba::BackgroundMode3::PlotPixel({120, 80}, {255, 0, 0});
// }

int main()
{
    //For now use bncore to get access to the assert
    bn::core::init();
    //Clear DisplayControlStatus for now as bn::core::init enables some of the stuff
    //making SetBackgroundMode not work as intended, without bn::core::init, force blank flag is enabled by default for some reason
    cgba::GetDisplayControlRegister() = {};
    

    // SetBackgroundMode<cgba::BackgroundMode0>();
    auto background = SetBackgroundMode<cgba::BackgroundMode3>();
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

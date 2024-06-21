#include "bn_core.h"
#include "types.hpp"

template<class Ty>
Ty SetBackgroundMode()
{
    cgba::DisplayControlStatus currentStatus = cgba::GetDisplayControlStatus();
    currentStatus &= ~cgba::DisplayControlStatus::Background_Mode_Mask;
    currentStatus |= Ty::modeValue | Ty::backgroundLayerSupport;
    cgba::GetDisplayControlStatus() = currentStatus;
    return Ty{};
}

int main()
{
    auto dc = cgba::GetDisplayControlStatus();
    //For now use bncore to get access to the assert
    bn::core::init();
    //Clear DisplayControlStatus for now as bncore init enables some of the stuff
    //making SetBackgroundMode not work as intended
    cgba::GetDisplayControlStatus() = dc;
    
    //BN_ASSERT(false, static_cast<cgba::u16>(dc));

    // SetBackgroundMode<cgba::BackgroundMode0>();
    auto background = SetBackgroundMode<cgba::BackgroundMode3>();
    // BN_ASSERT((cgba::GetDisplayControlStatus() & cgba::DisplayControlStatus::Background_Mode_Mask) == cgba::BackgroundMode3::modeValue);
    
    cgba::BackgroundMode3::PlotPixel({120, 80}, {255, 0, 0});
    // background.PlotPixel({120, 80}, {255, 0, 0});
    // background.PlotPixel({136, 80}, {0, 255, 0});
    // background.PlotPixel({120, 96}, {0, 0, 255});
        
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

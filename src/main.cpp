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
    auto& backgroundMode = cgba::Display::SetBackgroundMode<cgba::BackgroundMode0>();

    auto background = backgroundMode.GetBackground0();
    background.Show();
    // BN_ASSERT((cgba::GetDisplayControlStatus() & cgba::DisplayControlStatus::Background_Mode_Mask) == cgba::BackgroundMode3::modeValue);
    background.SetScreenBaseBlock(0);
    background.SetCharacterBaseBlock(1);
    background.SetScreenSize(cgba::TextScreenSizeMode::W256_H256);
    background.SetPalletteMode(cgba::PalletteMode::Color256_Palette1);
    cgba::TextBackgroundTileDescription* screenBase0 = &cgba::VRAM<cgba::TextBackgroundTileDescription>(0);
    cgba::TextBackgroundTileDescription* end =&cgba::VRAM<cgba::TextBackgroundTileDescription>(32 * 32);
    for(; screenBase0 != end; screenBase0++)
    {
        screenBase0->SetTileNumber(0);
        screenBase0->SetPalletteNumber(0);
    }


    
    cgba::Tile8& characterBase0 = reinterpret_cast<cgba::Tile8&>(cgba::VRAM<cgba::u8>(0x4000));
    characterBase0.data =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 0, 1, 1, 0,
        0, 0, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 1, 1,
        0, 0, 1, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
    cgba::Memory<cgba::RGB15>(0x0500'0002) = cgba::RGB15(31, 31, 31);


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

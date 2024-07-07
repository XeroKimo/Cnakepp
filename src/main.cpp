#include "bn_core.h"
#include "Display.hpp"
#include "Input.hpp"
void BadPresent()
{
    while(cgba::Display::GetVerticalCounter() >= 160)
    {

    }
    while(cgba::Display::GetVerticalCounter() < 160)
    {

    }
}

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
    background.SetScreenBaseBlock(1);
    background.SetCharacterBaseBlock(0);
    background.SetScreenSize(cgba::TextScreenSizeMode::W256_H256);
    background.SetPaletteMode(cgba::PaletteMode::Color256_Palette1);

    cgba::CharacterBlockView256 blockData = background.GetCharacterBlockData();
    blockData[1].data =
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
    
    cgba::PaletteView256 paletteBlockView = background.GetPalette256();
    paletteBlockView[1] = cgba::RGB15(31, 31, 31);

    static constexpr cgba::u32 lastTile = cgba::Display::hardwareScreenSizePixels.width / 8 * cgba::Display::hardwareScreenSizePixels.height / 8;
    cgba::u32 currentTile = lastTile - 1;

    cgba::TextScreenBlockView screenBlockView = background.GetScreenBlockData();
    screenBlockView[currentTile].SetTileNumber(1);
    screenBlockView[currentTile].SetPaletteNumber(0);

    cgba::KeyInput lastInput = cgba::PollInput();
    while(1)
    {
        cgba::KeyInput currentInput = cgba::PollInput();

        if(currentInput.data & cgba::Key::Right && (lastInput.data & cgba::Key::Right) == 0)
        {
            screenBlockView[currentTile].SetTileNumber(0);
            screenBlockView[currentTile].SetPaletteNumber(0);
            currentTile++;

            if(currentTile >= lastTile)
                currentTile = 0;
            
            screenBlockView[currentTile].SetTileNumber(1);
            screenBlockView[currentTile].SetPaletteNumber(0);
        }

        lastInput = currentInput;
        BadPresent();
    }

    return 0;
    // bn::core::init();

    // while(true)
    // {
    //     bn::core::update();
    // }
}

#include "bn_core.h"
#include "Display.hpp"
#include "Input.hpp"
void BadPresent()
{
    auto completeVBlank = []
    {
        while(cgba::Display::GetVerticalCounter() >= 160)
        {

        }
    };

    auto waitForVBlank = []
    {
        while(cgba::Display::GetVerticalCounter() < 160)
        {

        }
    };

    completeVBlank();
    waitForVBlank();
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

    static constexpr cgba::u16 lastVisibleX = cgba::Display::hardwareScreenSizePixels.width / 8;
    static constexpr cgba::u16 lastVisibleY = cgba::Display::hardwareScreenSizePixels.height / 8;
    
    cgba::Clamped<cgba::u16, 0, lastVisibleX> currentTileX{ 0 };
    cgba::Clamped<cgba::u16, 0, lastVisibleY> currentTileY{ 0 };
    cgba::u16 cachedCurrentTile = 0;
    auto updateCachedCurrentTile = [&cachedCurrentTile, &currentTileX, &currentTileY]
    {
        cachedCurrentTile = currentTileX.value + cgba::ScreenSizeConstants<cgba::TextScreenSizeMode::W256_H256>::screenSizeTiles.width * currentTileY;
    };

    cgba::TextScreenBlockView screenBlockView = background.GetScreenBlockData();
    screenBlockView[cachedCurrentTile].SetTileNumber(1);
    screenBlockView[cachedCurrentTile].SetPaletteNumber(0);

    cgba::KeyInput lastInput = cgba::PollInput();
    while(1)
    {
        cgba::KeyInput currentInput = cgba::PollInput();

        if(currentInput.data & cgba::Key::Right && (lastInput.data & cgba::Key::Right) == 0)
        {
            screenBlockView[cachedCurrentTile].SetTileNumber(0);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);

            currentTileX += 1;
            updateCachedCurrentTile();
            
            screenBlockView[cachedCurrentTile].SetTileNumber(1);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);
        }

        if(currentInput.data & cgba::Key::Left && (lastInput.data & cgba::Key::Left) == 0)
        {
            screenBlockView[cachedCurrentTile].SetTileNumber(0);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);

            currentTileX += -1;
            updateCachedCurrentTile();
            
            screenBlockView[cachedCurrentTile].SetTileNumber(1);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);
        }
        

        if(currentInput.data & cgba::Key::Down && (lastInput.data & cgba::Key::Down) == 0)
        {
            screenBlockView[cachedCurrentTile].SetTileNumber(0);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);

            currentTileY += 1;
            updateCachedCurrentTile();
            
            screenBlockView[cachedCurrentTile].SetTileNumber(1);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);
        }

        if(currentInput.data & cgba::Key::Up && (lastInput.data & cgba::Key::Up) == 0)
        {
            screenBlockView[cachedCurrentTile].SetTileNumber(0);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);

            currentTileY += -1;
            updateCachedCurrentTile();
            
            screenBlockView[cachedCurrentTile].SetTileNumber(1);
            screenBlockView[cachedCurrentTile].SetPaletteNumber(0);
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

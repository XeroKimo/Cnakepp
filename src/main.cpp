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

struct Snake
{
    static constexpr cgba::u32 moveDelay = 8; 

    cgba::u32 currentMaxSize = 20;
    cgba::u32 currentSize = 1;
    cgba::u32 moveDelayCounter = 0;
    cgba::Point<cgba::u16> tailPosition{};
    cgba::Point<cgba::u16> headPosition{};
    cgba::Point<cgba::u16> direction{ 1, 0 };
    cgba::Point<cgba::u16> lastInputDirection{ 0, 0 };

    bool playGame = true;

    std::array<cgba::Point<cgba::u16>, cgba::Display::hardwareScreenSizePixels.width / 8 * cgba::Display::hardwareScreenSizePixels.height / 8> directionCloud;

    Snake(cgba::TextScreenBlockView& screenBlock)
    {
        screenBlock[PointToScreenIndex(headPosition)].SetTileNumber(1);
        screenBlock[PointToScreenIndex(headPosition)].SetPaletteNumber(0);
    }

    cgba::u32 PointToCloudIndex(cgba::Point<cgba::u16> point)
    {
        return point.x + point.y * (cgba::Display::hardwareScreenSizePixels.width / 8);
    }

    cgba::u32 PointToScreenIndex(cgba::Point<cgba::u16> point)
    {
        return point.x + point.y * (cgba::ScreenSizeConstants<cgba::TextScreenSizeMode::W256_H256>::screenSizeTiles.width);
    }

    void Update(cgba::TextScreenBlockView& screenBlock)
    {
        if(!playGame)
            return;

        if(moveDelayCounter == moveDelay)
        {
            moveDelayCounter = 0;

            if((lastInputDirection.x != 0 && direction.y != 0) || (lastInputDirection.y != 0 && direction.x != 0))
            {
                direction = lastInputDirection;
            }
            lastInputDirection = {};
            
            directionCloud[PointToCloudIndex(headPosition)] = direction;

            //Keep a trail
            screenBlock[PointToScreenIndex(headPosition)].SetTileNumber(1);
            screenBlock[PointToScreenIndex(headPosition)].SetPaletteNumber(0);
            
            if(currentSize < currentMaxSize)
            {
                currentSize++;
            }
            else
            {
                auto oldTailPosition = tailPosition;
                screenBlock[PointToScreenIndex(tailPosition)].SetTileNumber(0);
                screenBlock[PointToScreenIndex(tailPosition)].SetPaletteNumber(0);

                tailPosition += directionCloud[PointToCloudIndex(tailPosition)];
                directionCloud[PointToCloudIndex(oldTailPosition)] = {};
            }

            headPosition += direction;
            screenBlock[PointToScreenIndex(headPosition)].SetTileNumber(1);
            screenBlock[PointToScreenIndex(headPosition)].SetPaletteNumber(0);
            if(directionCloud[PointToCloudIndex(headPosition)].x != 0 || directionCloud[PointToCloudIndex(headPosition)].y != 0)
            {
                playGame = false;
            }
        }
        moveDelayCounter++;
    }
};

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

    cgba::CharacterBlockView256 blockData = background.GetCharacterBlockData256();
    blockData[1].data =
    {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };
    
    cgba::PaletteView256 paletteBlockView = background.GetPalette256();
    paletteBlockView[1] = cgba::RGB15(31, 31, 31);
    
    cgba::TextScreenBlockView screenBlockView = background.GetScreenBlockData();
    Snake snake{ screenBlockView };
    

    cgba::KeyInput lastInput = cgba::PollInput();
    while(1)
    {
        cgba::KeyInput currentInput = cgba::PollInput();

        if(currentInput.data & cgba::Key::Right && (lastInput.data & cgba::Key::Right) == 0)
        {
            snake.lastInputDirection.x = 1;
        }

        else if(currentInput.data & cgba::Key::Left && (lastInput.data & cgba::Key::Left) == 0)
        {
            snake.lastInputDirection.x = -1;
        }
        

        else if(currentInput.data & cgba::Key::Down && (lastInput.data & cgba::Key::Down) == 0)
        {
            snake.lastInputDirection.y = 1;
        }

        else if(currentInput.data & cgba::Key::Up && (lastInput.data & cgba::Key::Up) == 0)
        {
            snake.lastInputDirection.y = -1;
        }
        snake.Update(screenBlockView);

        lastInput = currentInput;
        BadPresent();
    }

    return 0;
}

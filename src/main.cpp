#include "bn_core.h"
#include "Display.hpp"
#include "Input.hpp"
#include <bn_random.h>

bn::random defaultRandomGenerator;
//TODO: Refactor some of this stuff out as we confirm the game is working
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
    static constexpr cgba::u32 moveDelay = 5; 
    static constexpr cgba::u32 sizeIncrease = 2; 

    cgba::u32 currentMaxSize = 4;
    cgba::u32 currentSize = 1;
    cgba::u32 moveDelayCounter = 0;
    cgba::Point<cgba::i16> tailPosition;
    cgba::Point<cgba::i16> headPosition{ 7, 10 };
    cgba::Point<cgba::i16> direction{ 1, 0 };
    cgba::Point<cgba::i16> lastInputDirection{ 0, 0 };
    
    cgba::Point<cgba::i16> applePosition{ 15, 10 };

    bool playGame = true;

    std::array<cgba::Point<cgba::i16>, cgba::Display::hardwareScreenSizePixels.width / 8 * cgba::Display::hardwareScreenSizePixels.height / 8> directionCloud = {};

    Snake(cgba::TextScreenBlockView& screenBlock)
    {
        tailPosition = headPosition;
        screenBlock[PointToScreenIndex(headPosition)].SetTileNumber(1);
        screenBlock[PointToScreenIndex(headPosition)].SetPaletteNumber(0);
        
        screenBlock[PointToScreenIndex(applePosition)].SetTileNumber(2);
        screenBlock[PointToScreenIndex(applePosition)].SetPaletteNumber(0);
    }

    cgba::u32 PointToCloudIndex(cgba::Point<cgba::i16> point)
    {
        return point.x + point.y * (cgba::Display::hardwareScreenSizePixels.width / 8);
    }

    cgba::u32 PointToScreenIndex(cgba::Point<cgba::i16> point)
    {
        return point.x + point.y * (cgba::ScreenSizeConstants<cgba::TextScreenSizeMode::W256_H256>::screenSizeTiles.width);
    }

    void RandomizeApplePosition(cgba::TextScreenBlockView& screenBlock)
    {
        applePosition.x = defaultRandomGenerator.get_unbiased_int(cgba::Display::hardwareScreenSizePixels.width / 8);
        applePosition.y = defaultRandomGenerator.get_unbiased_int(cgba::Display::hardwareScreenSizePixels.height / 8);
        
        screenBlock[PointToScreenIndex(applePosition)].SetTileNumber(2);
        screenBlock[PointToScreenIndex(applePosition)].SetPaletteNumber(0);
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
            
            const auto nextHeadPosition = headPosition + direction;
         
            if((directionCloud[PointToCloudIndex(nextHeadPosition)].x != 0 || directionCloud[PointToCloudIndex(nextHeadPosition)].y != 0)
                || (nextHeadPosition.x < 0 || nextHeadPosition.x >=  cgba::Display::hardwareScreenSizePixels.width / 8)
                || (nextHeadPosition.y < 0 || nextHeadPosition.y >=  cgba::Display::hardwareScreenSizePixels.height / 8))
            {
                playGame = false;
                return;
            }

            
            directionCloud[PointToCloudIndex(headPosition)] = direction;

            //Keep a trail
            if(headPosition != applePosition)
            {
                screenBlock[PointToScreenIndex(headPosition)].SetTileNumber(1);
                screenBlock[PointToScreenIndex(headPosition)].SetPaletteNumber(0);
            }
            
            if(nextHeadPosition == applePosition)
            {
                currentMaxSize += sizeIncrease;
                
                screenBlock[PointToScreenIndex(applePosition)].SetTileNumber(0);
                screenBlock[PointToScreenIndex(applePosition)].SetPaletteNumber(0);

                RandomizeApplePosition(screenBlock);
            }
            
            if(currentSize < currentMaxSize)
            {
                currentSize++;
            }
            else
            {
                auto oldTailPosition = tailPosition;
                
                if(tailPosition != applePosition)
                {
                    screenBlock[PointToScreenIndex(tailPosition)].SetTileNumber(0);
                    screenBlock[PointToScreenIndex(tailPosition)].SetPaletteNumber(0);
                }

                tailPosition += directionCloud[PointToCloudIndex(tailPosition)];
                directionCloud[PointToCloudIndex(oldTailPosition)] = {};
            }

            headPosition = nextHeadPosition;
            if(headPosition != applePosition)
            {
                screenBlock[PointToScreenIndex(headPosition)].SetTileNumber(1);
                screenBlock[PointToScreenIndex(headPosition)].SetPaletteNumber(0);
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
    blockData[0].data =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };
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
    blockData[2].data =
    {
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2
    };
    
    cgba::PaletteView256 paletteBlockView = background.GetPalette256();
    paletteBlockView[1] = cgba::RGB15(31, 31, 31);
    paletteBlockView[2] = cgba::RGB15(31, 0, 0);
    
    cgba::TextScreenBlockView screenBlockView = background.GetScreenBlockData();
    
    auto ClearScreen = [](cgba::TextScreenBlockView view, cgba::u32 tileNumber)
    {
        for(cgba::u32 i = 0; i < cgba::ScreenSizeConstants<cgba::TextScreenSizeMode::W256_H256>::screenSizeTiles.width * cgba::ScreenSizeConstants<cgba::TextScreenSizeMode::W256_H256>::screenSizeTiles.height; i++)
        {
            view[i].SetTileNumber(tileNumber);
        }
    };

    cgba::KeyInput lastInput = cgba::PollInput();
    while(1)
    {
        ClearScreen(screenBlockView, 0);

        Snake snake{ screenBlockView };
        
        while(true)
        {
            cgba::KeyInput currentInput = cgba::PollInput();

            if(snake.playGame == false && currentInput.data & cgba::Key::A && (lastInput.data & cgba::Key::A) == 0)
            {
                break;
            }

            if(currentInput.data & cgba::Key::Right && (lastInput.data & cgba::Key::Right) == 0)
            {
                snake.lastInputDirection.x = 1;
                snake.lastInputDirection.y = 0;
            }

            else if(currentInput.data & cgba::Key::Left && (lastInput.data & cgba::Key::Left) == 0)
            {
                snake.lastInputDirection.x = -1;
                snake.lastInputDirection.y = 0;
            }
            

            else if(currentInput.data & cgba::Key::Down && (lastInput.data & cgba::Key::Down) == 0)
            {
                snake.lastInputDirection.x = 0;
                snake.lastInputDirection.y = 1;
            }

            else if(currentInput.data & cgba::Key::Up && (lastInput.data & cgba::Key::Up) == 0)
            {
                snake.lastInputDirection.x = 0;
                snake.lastInputDirection.y = -1;
            }
            snake.Update(screenBlockView);

            lastInput = currentInput;
            BadPresent();
        }
    }

    return 0;
}

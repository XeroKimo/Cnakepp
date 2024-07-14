#include "SnakeScene.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include <bn_random.h>
#include <utility>

extern bn::random defaultRandomGenerator;
namespace 
{
    using BackgroundView = cgba::StaticTileBackgroundView<cgba::TextScreenSizeMode::W256_H256, cgba::PaletteMode::Color256_Palette1>;
    
    constexpr cgba::u32 moveDelay = 5;
    constexpr cgba::u32 sizeIncrease = 4;

    void Initialize(SnakeGameState& state, BackgroundView snakeBuffer, BackgroundView appleBuffer);
    void RecordNewDirection(const cgba::BasicController& controller, cgba::Point<cgba::i16>& direction);
    void ConsumeDirection(cgba::Point<cgba::i16>& newDirection, SnakeGameState& state);
    void UpdateAndRender(SnakeGameState& state, BackgroundView snakeBuffer, BackgroundView appleBuffer);
    cgba::WordBool IsGameOver(const SnakeGameState& state);

    constexpr cgba::u32 emptyTile = 0;
    constexpr cgba::u32 snakeTile = 1;
    constexpr cgba::u32 appleTile = 2;
}

void SnakeScene()
{
    cgba::BackgroundMode0& backgroundMode = cgba::Display::SetBackgroundMode<cgba::BackgroundMode0>();
    BackgroundView background0 = backgroundMode.MakeStaticBackground0<cgba::TextScreenSizeMode::W256_H256, cgba::PaletteMode::Color256_Palette1>();
    BackgroundView background1 = backgroundMode.MakeStaticBackground1<cgba::TextScreenSizeMode::W256_H256, cgba::PaletteMode::Color256_Palette1>();
    background0.Show();
    background0.SetPriority(1);
    background1.Show();


    background0.SetScreenBaseBlock(1);
    background0.SetCharacterBaseBlock(0);

    background1.SetScreenBaseBlock(2);
    background1.SetCharacterBaseBlock(0);
    
    
    cgba::PaletteView256 paletteBlockView = background0.GetPalette();
    paletteBlockView[1] = cgba::RGB15(31, 31, 31);
    paletteBlockView[2] = cgba::RGB15(31, 0, 0);

    cgba::CharacterBlockView256 blockData = background0.GetCharacterBlockData();
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
    cgba::BasicController controller;

    while(true)
    {
        SnakeGameState state; 
        cgba::u32 moveTimer = moveDelay;
        cgba::WordBool playing = true;
        cgba::Point<cgba::i16> lastInputDirection{};

        Initialize(state, background0, background1);
        while(true)
        {
            controller.Poll();
            
            if(playing)
            {
                RecordNewDirection(controller, lastInputDirection);

                if(moveTimer == 0)
                {
                    moveTimer = moveDelay;
                    ConsumeDirection(lastInputDirection, state);
                    UpdateAndRender(state, background0, background1);
                    playing = !IsGameOver(state);
                }
                
                moveTimer--;
            }
            else
            {
                if(controller.Pressed(cgba::Key::A))
                    break;
            }

            cgba::BadPresent();
        }
    }
}

namespace
{
    void Initialize(SnakeGameState &state, BackgroundView snakeBuffer, BackgroundView appleBuffer)
    {
        auto ClearBuffer = [](BackgroundView buffer)
        {
            for(cgba::i32 i = 0; i < cgba::Area(buffer.GetTileScreenSize()); i++)
            {
                buffer.GetScreenBlockData()[i].SetTileNumber(emptyTile);
                buffer.GetScreenBlockData()[i].SetPaletteNumber(0);
            }
        };

        ClearBuffer(snakeBuffer);
        ClearBuffer(appleBuffer);

        state.snake.maxSize = 4;
        state.snake.headPosition = state.snake.tailPosition = {7, 10};
        state.applePosition = { 15, 10 };
        state.snakeMovementDirection = {1, 0};
        state.board.occupiedSpaceCount = 1;
        snakeBuffer.GetScreenBlockData()[state.snake.headPosition].SetTileNumber(snakeTile);
        appleBuffer.GetScreenBlockData()[state.applePosition].SetTileNumber(appleTile);
    }

    void RecordNewDirection(const cgba::BasicController &controller, cgba::Point<cgba::i16>& direction)
    {
        if(controller.Pressed(cgba::Key::Right))
            direction = {1, 0};
        else if(controller.Pressed(cgba::Key::Left))
            direction = {-1, 0};
        else if(controller.Pressed(cgba::Key::Up))
            direction = {0, -1};
        else if(controller.Pressed(cgba::Key::Down))
            direction = {0, 1};
    }

    void ConsumeDirection(cgba::Point<cgba::i16>& newDirection, SnakeGameState &state)
    {
        if((state.snakeMovementDirection.x != 0 && newDirection.y != 0)
            || (state.snakeMovementDirection.y != 0 && newDirection.x != 0))
            state.snakeMovementDirection = newDirection;

        newDirection = {};
    }

    void UpdateAndRender(SnakeGameState &state, BackgroundView snakeBuffer, BackgroundView appleBuffer)
    {
        state.board.DirectionAt(state.snake.headPosition) = state.snakeMovementDirection;
        state.snake.headPosition += state.snakeMovementDirection;

        if(state.snake.headPosition == state.applePosition)
        {
            state.snake.maxSize += sizeIncrease;
            appleBuffer.GetScreenBlockData()[state.applePosition].SetTileNumber(emptyTile);
            auto value = defaultRandomGenerator.get_unbiased_int(cgba::Area(SnakeDirectionBoard::boardSize));
            state.applePosition = { 
                static_cast<cgba::i16>(value % SnakeDirectionBoard::boardSize.width), 
                static_cast<cgba::i16>(value / SnakeDirectionBoard::boardSize.width)};
            appleBuffer.GetScreenBlockData()[state.applePosition].SetTileNumber(appleTile);
        }

        if(state.board.occupiedSpaceCount < state.snake.maxSize)
        {
            state.board.occupiedSpaceCount++;
        }
        else
        {
            auto tailDirection = std::exchange(state.board.DirectionAt(state.snake.tailPosition), cgba::Point<cgba::i16>{});
            snakeBuffer.GetScreenBlockData()[state.snake.tailPosition].SetTileNumber(emptyTile);
            state.snake.tailPosition += tailDirection;
        }

        snakeBuffer.GetScreenBlockData()[state.snake.headPosition].SetTileNumber(snakeTile);
    }

    cgba::WordBool IsGameOver(const SnakeGameState &state)
    {
        const cgba::WordBool selfCollided = state.board.DirectionAt(state.snake.headPosition).MagnitudeSquared() > 0;
        const cgba::WordBool gameWon = state.board.occupiedSpaceCount == state.board.board.size();
        const cgba::WordBool outOfBounds = state.snake.headPosition.x < 0 || state.snake.headPosition.x >= cgba::Display::hardwareScreenSizePixels.width / cgba::tileSizePixels.width
            ||  state.snake.headPosition.y < 0 || state.snake.headPosition.y >= cgba::Display::hardwareScreenSizePixels.height / cgba::tileSizePixels.height;
        return  selfCollided || outOfBounds || gameWon;
    }
}
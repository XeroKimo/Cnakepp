#pragma once
#include "Math.hpp"
#include "Display.hpp"
#include <array>

void SnakeScene();

struct Snake
{
    cgba::u32 maxSize;
    cgba::Point<cgba::i16> headPosition;
    cgba::Point<cgba::i16> tailPosition;
};

struct SnakeDirectionBoard
{
    static constexpr cgba::Rectangle boardSize = cgba::ElementWiseDiv(cgba::Display::hardwareScreenSizePixels, cgba::tileSizePixels);
    cgba::u32 occupiedSpaceCount;
    std::array<cgba::Point<cgba::i16>, cgba::Area(boardSize)> board = {};

    cgba::Point<cgba::i16>& DirectionAt(cgba::Point<cgba::i16> position)
    {
        return board[position.x + position.y * boardSize.width];
    }

    const cgba::Point<cgba::i16>& DirectionAt(cgba::Point<cgba::i16> position) const
    {
        return board[position.x + position.y * boardSize.width];
    }
};

struct SnakeGameState
{
    cgba::Point<cgba::i16> applePosition;
    cgba::Point<cgba::i16> snakeMovementDirection{ 1, 0 };
    Snake snake;
    SnakeDirectionBoard board;
};
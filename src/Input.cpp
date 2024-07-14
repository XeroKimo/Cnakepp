#include "Input.hpp"
#include <utility>

namespace cgba
{
    void BasicController::Poll()
    {
        previous = std::exchange(current, PollInput());
    }

    WordBool BasicController::Pressed(Key key) const
    {
        return WordBool{ (~previous.data & current.data) & key }; 
    }
    WordBool BasicController::Held(Key key) const
    {
        return WordBool{ (previous.data & current.data) & key }; 
    }
    WordBool BasicController::Released(Key key) const
    {
        return WordBool{ (previous.data & ~current.data) & key }; 
    }
}
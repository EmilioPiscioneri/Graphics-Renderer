#include "Tween.h"

Tween::State Tween::GetState()
{
    return _state;
}

void Tween::Resume()
{
    // set to active
    _state = State::Active;
}

void Tween::Stop()
{
    // set to inactive
    _state = State::InActive;
}

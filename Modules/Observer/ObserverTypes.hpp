#pragma once

////////////////////////////////////////////////////////////
/// EXOTIC MODULE TYPES
/// Module: Observer
////////////////////////////////////////////////////////////

namespace exotic
{

//==========================================================
// TYPES
//==========================================================

enum class ObserverState
{
    Created,
    Active,
    Paused,
    Error,
    Destroyed
};

}

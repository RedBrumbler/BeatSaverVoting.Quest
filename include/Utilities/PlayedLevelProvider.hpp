#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/DiContainer.hpp"

DECLARE_CLASS_CODEGEN(BeatSaverVoting::Utilities, PlayedLevelProvider, System::Object,
    DECLARE_CTOR(ctor, Zenject::DiContainer* container);
)

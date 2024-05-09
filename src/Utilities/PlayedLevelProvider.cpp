#include "Utilities/PlayedLevelProvider.hpp"
#include "UI/VotingUI.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"

DEFINE_TYPE(BeatSaverVoting::Utilities, PlayedLevelProvider);

namespace BeatSaverVoting::Utilities {
    void PlayedLevelProvider::ctor(Zenject::DiContainer* container) {
        auto votingUI = UI::VotingUI::get_instance();
        if (!votingUI) return;

        auto sceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
        if (!sceneSetupData) {
            votingUI->lastLevel = nullptr;
            return;
        }

        votingUI->lastLevel = sceneSetupData->beatmapLevel;
    }
}

#include "Utilities/PlayedLevelProvider.hpp"
#include "UI/VotingUI.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "logging.hpp"

DEFINE_TYPE(BeatSaverVoting::Utilities, PlayedLevelProvider);

namespace BeatSaverVoting::Utilities {
    void PlayedLevelProvider::ctor(Zenject::DiContainer* container) {
        auto votingUI = UI::VotingUI::get_instance();
        if (!votingUI) {
            WARNING("Voting UI instance is null! can't set last level");
            return;
        }

        auto sceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
        if (!sceneSetupData) {
            WARNING("Couldn't get scene setup data, setting level to null!");
            votingUI->lastLevel = nullptr;
            return;
        }

        votingUI->lastLevel = sceneSetupData->beatmapLevel;
    }
}

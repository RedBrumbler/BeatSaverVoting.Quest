#include "logging.hpp"
#include "hooking.hpp"
#include "config.hpp"
#include "_config.h"

#include "scotland2/shared/loader.hpp"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"
#include "lapiz/shared/zenject/Zenjector.hpp"

#include "UI/VotingUI.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ScopeConcreteIdArgConditionCopyNonLazyBinder.hpp"

BEATSAVER_VOTING_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;
}

BEATSAVER_VOTING_EXPORT_FUNC void load() {}

BEATSAVER_VOTING_EXPORT_FUNC void late_load() {
    if (!LoadConfig())
        SaveConfig();

    BSML::Init();
    custom_types::Register::AutoRegister();
    // BeatSaverVoting::Hooking::InstallHooks();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();

    zenjector->Install(Lapiz::Zenject::Location::Menu, [](auto container) {
        DEBUG("Menu installer");
        container->template BindInterfacesAndSelfTo<BeatSaverVoting::UI::VotingUI*>()->AsSingle();
    });
}

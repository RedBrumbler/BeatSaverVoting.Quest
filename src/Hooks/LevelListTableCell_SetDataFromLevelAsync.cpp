#include "hooking.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "GlobalNamespace/LevelListTableCell.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "HMUI/ImageView.hpp"

#include "VoteStatus.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/BSMLDataCache.hpp"

static std::string HashForLevelID(std::string levelId) {
    auto hashView = std::string_view(levelId);
    if (!hashView.starts_with("custom_level_")) return "";
    hashView = hashView.substr(13);
    if (hashView.ends_with(" WIP")) hashView = hashView.substr(0, hashView.size() - 4);
    return std::string(hashView);
}

MAKE_AUTO_HOOK_MATCH(LevelListTableCell_SetDataFromLevelAsync, &GlobalNamespace::LevelListTableCell::SetDataFromLevelAsync, void, GlobalNamespace::LevelListTableCell* self, ::GlobalNamespace::BeatmapLevel* level, bool isFavorite, bool isPromoted, bool isUpdated) {
    LevelListTableCell_SetDataFromLevelAsync(self, level, isFavorite, isPromoted, isUpdated);

    auto hash = HashForLevelID(level->levelID);
    auto voteStatus = BeatSaverVoting::VoteStatus::GetCurrentVoteStatus(hash);

    if (!voteStatus.has_value() && !isFavorite) return;

    auto badgeImg = self->_favoritesBadgeImage;

    static SafePtrUnity<UnityEngine::Sprite> defaultBadge;
    if (!defaultBadge) {
        defaultBadge = badgeImg->sprite;
    }

    if (voteStatus.has_value()) {
        switch (voteStatus.value()) {
            using enum BeatSaverVoting::VoteType;
            case Upvote: {
                BSML::Utilities::SetImage(badgeImg, isFavorite ? MOD_ID "_fav_uv" : MOD_ID "_uv");
            } break;
            case Downvote: {
                BSML::Utilities::SetImage(badgeImg, isFavorite ? MOD_ID "_fav_dv" : MOD_ID "_dv");
            } break;
        }
    } else {
        BSML::Utilities::SetImage(badgeImg, MOD_ID "_fav");
    }

    auto rt = badgeImg->rectTransform;
    if (rt->sizeDelta.x < 3.5f) {
        rt->sizeDelta = {3.5f, 7.0f};

        auto position = rt->localPosition;
        position.x += 1.0f;
        rt->localPosition = position;
    }
}

BSML_DATACACHE(fav) { return Assets::Favorite_png; }
BSML_DATACACHE(fav_uv) { return Assets::FavoriteUpvote_png; }
BSML_DATACACHE(fav_dv) { return Assets::FavoriteDownvote_png; }
BSML_DATACACHE(uv) { return Assets::Upvote_png; }
BSML_DATACACHE(dv) { return Assets::Downvote_png; }

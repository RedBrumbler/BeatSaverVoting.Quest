#include "UI/VotingUI.hpp"
#include "VoteStatus.hpp"
#include "logging.hpp"

#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/delegates.hpp"
#include "assets.hpp"

#include "HMUI/ButtonStaticAnimations.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/PlatformUserAuthTokenData.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"

#include "beatsaverplusplus/shared/BeatSaver.hpp"
#include <chrono>

DEFINE_TYPE(BeatSaverVoting::UI, VotingUI);

static std::string HashForLevelID(std::string levelId) {
    auto hashView = std::string_view(levelId);
    if (!hashView.starts_with("custom_level_")) return "";
    hashView = hashView.substr(0, 13);
    if (hashView.ends_with(" WIP")) hashView = hashView.substr(0, hashView.size() - 4);
    return std::string(hashView);
}

namespace BeatSaverVoting::UI {
    VotingUI* VotingUI::_instance = nullptr;

    void VotingUI::ctor(GlobalNamespace::ResultsViewController* resultsViewController, GlobalNamespace::PlatformLeaderboardsModel* leaderboardsModel, GlobalNamespace::LevelCollectionTableView* levelCollectionTable) {
        INVOKE_CTOR();
        _resultsViewController = resultsViewController;
        _levelCollectionTable = levelCollectionTable;
        _userModel = leaderboardsModel->_platformUserModel;

        _downloader.userAgent = MOD_ID "/" VERSION " (+https://github.com/RedBrumbler/BeatSaverVoting.Quest)";

        _didActivateDelegate = custom_types::MakeDelegate<HMUI::ViewController::DidActivateDelegate*>(
            std::function<void(bool, bool, bool)>(
                std::bind(&VotingUI::ResultsView_DidActivate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
            )
        );
    }

    void VotingUI::Initialize() {
        _instance = this;
        BSML::parse_and_construct(Assets::votingUI_bsml, _resultsViewController->transform, this);
        _resultsViewController->add_didActivateEvent(_didActivateDelegate);
        SetColors();
    }

    void VotingUI::Dispose() {
        if (_instance == this) _instance = nullptr;

        _resultsViewController->remove_didActivateEvent( _didActivateDelegate);

    }

    void VotingUI::UpvoteButtonPressed() {
        using namespace std::placeholders;
        VoteForSong(_lastBeatsaverSong, true, std::bind(&VotingUI::UpdateUIAfterVote, this, _1, _2, _3, _4));
    }

    void VotingUI::DownvoteButtonPressed() {
        using namespace std::placeholders;
        VoteForSong(_lastBeatsaverSong, false, std::bind(&VotingUI::UpdateUIAfterVote, this, _1, _2, _3, _4));
    }

    void VotingUI::ResultsView_DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        GetVotesForMap();
    }

    void VotingUI::SetColors() {
        auto upArrow = _upButton->GetComponentInChildren<HMUI::ImageView*>();
        auto downArrow = _downButton->GetComponentInChildren<HMUI::ImageView*>();

        if (!upArrow || !downArrow) return;

        SetupButtonAnimation(_upButton, {0.341f, 0.839f, 0.341f, 1});
        SetupButtonAnimation(_downButton, {0.984f, 0.282f, 0.305f, 1});
    }

    void VotingUI::SetupButtonAnimation(UnityEngine::Component* t, UnityEngine::Color c) {
        auto anim = t->GetComponent<HMUI::ButtonStaticAnimations*>();

        anim->_normalClip = GenerateButtonAnimation(c.r, c.g, c.b, 0.502f, 1, 1);
        anim->_highlightedClip = GenerateButtonAnimation(c.r, c.g, c.b, 1, 1.5f, 1.5f);
    }

    UnityEngine::AnimationCurve* ConstantCurve(float startTime, float endTime, float value) {
        return UnityEngine::AnimationCurve::Linear(startTime, value, endTime, value);
    }

    UnityEngine::AnimationClip* VotingUI::GenerateButtonAnimation(float r, float g, float b, float a, float x, float y) {
        return GenerateButtonAnimation(
            ConstantCurve(0, 1, r),
            ConstantCurve(0, 1, g),
            ConstantCurve(0, 1, b),
            ConstantCurve(0, 1, a),
            ConstantCurve(0, 1, x),
            ConstantCurve(0, 1, y)
        );
    }

    UnityEngine::AnimationClip* VotingUI::GenerateButtonAnimation(UnityEngine::AnimationCurve* r, UnityEngine::AnimationCurve* g, UnityEngine::AnimationCurve* b, UnityEngine::AnimationCurve* a, UnityEngine::AnimationCurve* x, UnityEngine::AnimationCurve* y) {
        auto animation = UnityEngine::AnimationClip::New_ctor();
        animation->legacy = true;

        animation->SetCurve("Icon", csTypeOf(UnityEngine::Transform*), "localScale.x", x);
        animation->SetCurve("Icon", csTypeOf(UnityEngine::Transform*), "localScale.y", y);
        animation->SetCurve("Icon", csTypeOf(UnityEngine::UI::Graphic*), "m_Color.r", r);
        animation->SetCurve("Icon", csTypeOf(UnityEngine::UI::Graphic*), "m_Color.g", g);
        animation->SetCurve("Icon", csTypeOf(UnityEngine::UI::Graphic*), "m_Color.b", b);
        animation->SetCurve("Icon", csTypeOf(UnityEngine::UI::Graphic*), "m_Color.a", a);

        return animation;
    }

    void VotingUI::GetVotesForMap() {
        // if we don't have a level dont do anything
        if (lastLevel == nullptr) return;

        bool isCustomLevel = lastLevel->levelID.starts_with(u"custom_level_");
        _upButton->gameObject->SetActive(isCustomLevel);
        _downButton->gameObject->SetActive(isCustomLevel);
        _voteTitle->gameObject->SetActive(isCustomLevel);

        static ConstString Loading("Loading...");
        static ConstString Empty("");
        _voteText->text = isCustomLevel ? Loading : Empty;

        if (isCustomLevel) {
            _voteTitle->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GetRatingForSong(lastLevel)));
        }
    }

    std::optional<Song> VotingUI::GetSongInfo(std::string hash) {
        if (hash.empty()) return std::nullopt;

        _lastBeatsaverSong = std::nullopt;

        // get the beatmap from beatsaver
        auto response = _downloader.Get<BeatSaver::API::BeatmapResponse>(BeatSaver::API::GetBeatmapByHashURLOptions(hash));
        if (!response.IsSuccessful() || !response.DataParsedSuccessful()) {
            DEBUG("Failed to fetch beatmap data from beatsaver");
            return std::nullopt;
        }

        // check whether the correct beatmap
        auto& beatmap = response.responseData.value();
        auto versions = beatmap.Versions;
        auto itr = std::find_if(versions.begin(), versions.end(), [&hash](auto& x){ return x.Hash == hash; });
        if (itr == versions.end()) return std::nullopt;

        return Song(beatmap);
    }

    std::future<std::optional<Song>> VotingUI::GetSongInfoAsync(std::string hash) {
        return std::async(std::launch::async, &VotingUI::GetSongInfo, this, hash);
    }

    void VotingUI::VoteForSong(std::string hash, bool upvote, VoteCallback callback) {
        _voteTitle->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(VoteForSongAsync(hash, upvote, callback)));
    }

    void VotingUI::VoteForSong(std::optional<Song> song, bool upvote, VoteCallback callback) {
        if (!song.has_value()) {
            if (callback) callback("", false, false, -1);
            return;
        }

        auto currentStatus = VoteStatus::GetCurrentVoteStatus(song->hash);
        int userTotal = 0;
        if (currentStatus.has_value()) {
            userTotal = currentStatus.value() == VoteType::Upvote ? 1 : -1;
        }
        auto oldValue = song->upVotes - song->downVotes - userTotal;
        VoteForSong(song->hash, upvote, oldValue, callback);
    }

    void VotingUI::VoteForSong(std::string hash, bool upvote, int currentVoteCount, VoteCallback callback) {
        _voteTitle->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(VoteWithUserInfo(hash, upvote, currentVoteCount, callback)));
    }

    custom_types::Helpers::Coroutine VotingUI::VoteForSongAsync(std::string hash, bool upvote, VoteCallback callback) {
        auto songFuture = GetSongInfoAsync(hash);
        while (songFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            co_yield nullptr;
        VoteForSong(songFuture.get(), upvote, callback);
        co_return;
    }

    custom_types::Helpers::Coroutine VotingUI::VoteWithUserInfo(std::string hash, bool upvote, int currentVoteCount, VoteCallback callback) {
        UpdateView("Voting...");

        auto userInfoTask = _userModel->GetUserInfo(::System::Threading::CancellationToken());
        auto userAuthTask = _userModel->GetUserAuthToken();
        while (!userInfoTask->IsCompleted && !userAuthTask->IsCompleted) co_yield nullptr;
        auto userInfo = userInfoTask->Result;
        auto authData = userAuthTask->Result;

        auto auth = BeatSaver::API::PlatformAuth {
            .platform = BeatSaver::API::UserPlatform::Oculus,
            .userId = userInfo->platformUserId,
            .proof = authData->token
        };
        auto [urlOptions, data] = BeatSaver::API::PostVoteURLOptionsAndData(auth, upvote, hash);
        urlOptions.headers["Content-Type"] = "application/json";

        _voteTitle->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(PerformVote(hash, upvote, urlOptions, data, currentVoteCount, callback)));
        co_return;
    }

    custom_types::Helpers::Coroutine VotingUI::PerformVote(std::string hash, bool upvote, WebUtils::URLOptions urlOptions, std::string data, int currentVoteCount, VoteCallback callback) {
        auto postFuture = _downloader.PostAsync<BeatSaver::API::VoteResponse>(urlOptions, std::span<uint8_t const>((uint8_t*)data.data(), data.size()));
        while (postFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            co_yield nullptr;

        auto response = postFuture.get();
        if (!response.IsSuccessful()) {
            if (response.curlStatus != 0) {
                ERROR("Curl status return for vote: {}", response.curlStatus);
                if (callback) callback(hash, false, false, currentVoteCount);
            } else if (response.httpCode < 200 || response.httpCode >= 300) {
                static std::map<int, std::string> errorMessages = {
                    {500, "Server \nerror"},
                    {401, "Invalid\nauth ticket"},
                    {404, "Beatmap not\nfound"},
                    {400, "Bad\nrequest"}
                };
                auto errorMessageItr = errorMessages.find(response.httpCode);
                auto errorMessage = errorMessageItr != errorMessages.end() ? errorMessageItr->second : fmt::format("Error\n{}", response.httpCode);
                UpdateView(errorMessage, errorMessageItr == errorMessages.end());

                ERROR("Error {}: {}", response.httpCode, errorMessage);

                if (callback) callback(hash, false, false, currentVoteCount);
            }
            co_return;
        }

        auto newTotal = currentVoteCount + (upvote ? 1 : -1);
        DEBUG("Current vote count: {}, new total: {}", currentVoteCount, newTotal);
        if (callback) callback(hash, true, upvote, newTotal);
    }

    static std::string GetScoreFromVotes(int upVotes, int downVotes) {
        double totalVotes = upVotes + downVotes;
        auto rawScore = upVotes / totalVotes;
        auto scoreWeighted = rawScore - (rawScore - 0.5) * std::pow(2.0, -std::log(totalVotes / 2 + 1) / std::log(3));

        return fmt::format("{:01.2f} ({})", scoreWeighted * 100, (int)totalVotes);
    }

    custom_types::Helpers::Coroutine VotingUI::GetRatingForSong(GlobalNamespace::BeatmapLevel* level) {
        auto levelHash = HashForLevelID(lastLevel->levelID);
        // not custom
        if (levelHash.empty()) co_return;
        _lastBeatsaverSong = std::nullopt;

        auto songFuture = GetSongInfoAsync(levelHash);
        while (songFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            co_yield nullptr;

        _lastBeatsaverSong = songFuture.get();
        // song not on beatsaver
        if (!_lastBeatsaverSong.has_value())
            co_return;

        _voteText->text = GetScoreFromVotes(_lastBeatsaverSong->upVotes, _lastBeatsaverSong->downVotes);

        upInteractable = true;
        downInteractable = true;

        auto status = VoteStatus::GetCurrentVoteStatus(levelHash);
        if (!status.has_value()) co_return;

        switch (status.value()) {
            using enum VoteType;
            case Upvote: { upInteractable = false; } break;
            case Downvote: { downInteractable = false; } break;
        }

        co_return;
    }

    void VotingUI::UpdateView(std::string text, bool up, std::optional<bool> down) {
        upInteractable = up;
        downInteractable = down.value_or(up);
        _voteText->text = text;
    }

    void VotingUI::UpdateUIAfterVote(std::string hash, bool success, bool upvote, int newTotal) {
        if (!success) return;

        bool hasPreviousVote = VoteStatus::GetCurrentVoteStatus(hash).has_value();
        auto expectedVoteStatus = upvote ? VoteType::Upvote : VoteType::Downvote;
        upInteractable = !upvote;
        downInteractable = upvote;

        if (hash == _lastBeatsaverSong->hash) {
            if (hasPreviousVote) {
                auto diff = upvote ? 1 : -1;
                _lastBeatsaverSong->upVotes += diff;
                _lastBeatsaverSong->downVotes -= diff;
            } else if (upvote) {
                _lastBeatsaverSong->upVotes++;
            } else {
                _lastBeatsaverSong->downVotes++;
            }

            _voteText->text = GetScoreFromVotes(_lastBeatsaverSong->upVotes, _lastBeatsaverSong->downVotes);
        } else {
            _voteText->text = std::to_string(newTotal);
        }

        auto status = VoteStatus::GetCurrentVoteStatus(hash);
        if (!status.has_value() || status.value() != expectedVoteStatus) {
            VoteStatus::SetVoteStatus(hash, expectedVoteStatus);
            _levelCollectionTable->_tableView->RefreshCellsContent();
        }
    }
}

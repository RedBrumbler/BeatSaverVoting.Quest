#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "Utilities/PlayedLevelProvider.hpp"

#include "UnityEngine/AnimationClip.hpp"
#include "UnityEngine/AnimationCurve.hpp"

#include "Song.hpp"
#include "web-utils/shared/DownloaderUtility.hpp"
#include "beatsaverplusplus/shared/BeatSaver.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(BeatSaverVoting::UI, VotingUI, System::Object, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, _upButton);
        DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, _downButton);
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _voteTitle);
        DECLARE_INSTANCE_FIELD_PRIVATE(TMPro::TextMeshProUGUI*, _voteText);

        DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ResultsViewController*, _resultsViewController);
        DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LevelCollectionTableView*, _levelCollectionTable);
        DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IPlatformUserModel*, _userModel);
        DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ViewController::DidActivateDelegate*, _didActivateDelegate);

        DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapLevel*, lastLevel);

        DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
        DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);

        DECLARE_INSTANCE_METHOD(void, UpvoteButtonPressed);
        DECLARE_INSTANCE_METHOD(void, DownvoteButtonPressed);

        DECLARE_CTOR(ctor, GlobalNamespace::ResultsViewController* resultsViewController, GlobalNamespace::PlatformLeaderboardsModel* leaderboardsModel, GlobalNamespace::LevelCollectionTableView* levelCollectionTable);
    public:
        static VotingUI* get_instance() { return _instance; }
        bool get_upInteractable() { return _upButton ? _upButton->interactable : false; }
        void set_upInteractable(bool interactable) {
            if (_upButton && _upButton->m_CachedPtr) _upButton->interactable = interactable;
        }

        __declspec(property(get=get_upInteractable, put=set_upInteractable)) bool upInteractable;

        bool get_downInteractable() { return _downButton ? _downButton->interactable : false; }
        void set_downInteractable(bool interactable) {
            if (_downButton && _downButton->m_CachedPtr) _downButton->interactable = interactable;
        }
        __declspec(property(get=get_downInteractable, put=set_downInteractable)) bool downInteractable;
    private:
        using VoteCallback = std::function<void(std::string hash, bool success, bool userDirection, int newtotal)>;

        friend class Utilities::PlayedLevelProvider;
        static VotingUI* _instance;
        WebUtils::DownloaderUtility _downloader;
        std::optional<Song> _lastBeatsaverSong;

        void ResultsView_DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
        void GetVotesForMap();

        void VoteForSong(std::string hash, bool upvote, VoteCallback callback);
        void VoteForSong(std::optional<Song> song, bool upvote, VoteCallback callback);
        void VoteForSong(std::string hash, bool upvote, int currentVoteCount, VoteCallback callback);
        custom_types::Helpers::Coroutine VoteForSongAsync(std::string hash, bool upvote, VoteCallback callback);

        custom_types::Helpers::Coroutine GetRatingForSong(GlobalNamespace::BeatmapLevel* level);

        custom_types::Helpers::Coroutine VoteWithUserInfo(std::string hash, bool upvote, int currentVoteCount, VoteCallback callback);

        custom_types::Helpers::Coroutine PerformVote(std::string hash, bool upvote, WebUtils::URLOptions urlOptions, std::string data, int currentVoteCount, VoteCallback callback);

        std::optional<Song> GetSongInfo(std::string hash);
        std::future<std::optional<Song>> GetSongInfoAsync(std::string hash);

        void UpdateView(std::string text, bool up = false, std::optional<bool> down = std::nullopt);
        void UpdateUIAfterVote(std::string hash, bool success, bool upvote, int newTotal);

        void SetColors();
        static void SetupButtonAnimation(UnityEngine::Component* t, UnityEngine::Color c);
        static UnityEngine::AnimationClip* GenerateButtonAnimation(float r, float g, float b, float a, float x, float y);
        static UnityEngine::AnimationClip* GenerateButtonAnimation(UnityEngine::AnimationCurve* r, UnityEngine::AnimationCurve* g, UnityEngine::AnimationCurve* b, UnityEngine::AnimationCurve* a, UnityEngine::AnimationCurve* x, UnityEngine::AnimationCurve* y);
)

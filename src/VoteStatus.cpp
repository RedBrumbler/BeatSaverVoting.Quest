#include "VoteStatus.hpp"
#include "config.hpp"

namespace BeatSaverVoting {
    VoteType Deserialize_VoteType(rapidjson::Value const& json) {
        auto str = json.Get<std::string>();
        std::transform(str.begin(), str.end(), str.begin(), tolower);

        if (str == "upvote") return VoteType::Upvote;
        if (str == "downvote") return VoteType::Downvote;
        return VoteType::Downvote;
    }

    rapidjson::Value& Serialize_VoteType(VoteType const& songVote, rapidjson::Value& json, rapidjson::Value::AllocatorType allocator) {
        switch(songVote) {
            using enum VoteType;
            case Upvote: json.SetString("Upvote", allocator); break;
            case Downvote: json.SetString("Downvote", allocator); break;
        }
        return json;
    }


    std::optional<VoteType> VoteStatus::GetCurrentVoteStatus(std::string hash) {
        std::transform(hash.begin(), hash.end(), hash.begin(), tolower);
        auto itr = config.votedSongs.find(hash);
        if (itr == config.votedSongs.end()) return std::nullopt;
        return itr->second;
    }

    void VoteStatus::SetVoteStatus(std::string hash, VoteType voteStatus) {
        std::transform(hash.begin(), hash.end(), hash.begin(), tolower);
        config.votedSongs[hash] = voteStatus;
        SaveConfig();
    }
}

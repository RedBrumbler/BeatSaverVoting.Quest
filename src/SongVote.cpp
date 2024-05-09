#include "SongVote.hpp"

namespace BeatSaverVoting {
    SongVote SongVote::Deserialize(rapidjson::Value const& json) {
        return {
            json["hash"].Get<std::string>(),
            json["voteType"].Get<VoteType>()
        };
    }

    rapidjson::Value& SongVote::Serialize(SongVote const& songVote, rapidjson::Value& json, rapidjson::Value::AllocatorType allocator) {
        json.SetObject();
        json.AddMember("hash", rapidjson::Value(songVote.hash, allocator), allocator);
        json.AddMember("voteType", songVote.voteType, allocator);

        return json;
    }
}

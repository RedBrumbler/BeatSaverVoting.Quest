#include "SongVote.hpp"
#include "VoteStatus.hpp"

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
        rapidjson::Value voteTypeValue;
        json.AddMember("voteType", Serialize_VoteType(songVote.voteType, voteTypeValue, allocator), allocator);

        return json;
    }
}

#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "VoteStatus.hpp"

namespace BeatSaverVoting {
    struct SongVote {
        std::string hash;
        VoteType voteType;

        static SongVote Deserialize(rapidjson::Value const& json);
        static rapidjson::Value& Serialize(SongVote const& songVote, rapidjson::Value& json, rapidjson::Value::AllocatorType allocator);
    };
}

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {
    template<typename ValueType>
    struct TypeHelper<ValueType, BeatSaverVoting::SongVote> {
        static bool Is(const ValueType& v) { return v.IsObject(); }
        static BeatSaverVoting::SongVote Get(const ValueType& v) { return BeatSaverVoting::SongVote::Deserialize(v); }                                                                                      \
        static ValueType& Set(ValueType& v, BeatSaverVoting::SongVote data, typename ValueType::AllocatorType& allocator) {
            return BeatSaverVoting::SongVote::Serialize(data, v, allocator);
        }
    };
}
RAPIDJSON_NAMESPACE_END

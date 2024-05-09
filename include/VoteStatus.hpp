#pragma once

#include "_config.h"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include <map>
#include <string>

namespace BeatSaverVoting {
    struct SongVote;

    enum class VoteType {
        Upvote,
        Downvote
    };

    BEATSAVER_VOTING_EXPORT VoteType Deserialize_VoteType(rapidjson::Value const& json);
    BEATSAVER_VOTING_EXPORT rapidjson::Value& Serialize_VoteType(VoteType const& songVote, rapidjson::Value& json, rapidjson::Value::AllocatorType allocator);

    struct VoteStatus {
        static std::optional<VoteType> GetCurrentVoteStatus(std::string hash);

        static void SetVoteStatus(std::string hash, VoteType voteStatus);
    };
}

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {
    template<typename ValueType>
    struct TypeHelper<ValueType, BeatSaverVoting::VoteType> {
        static bool Is(const ValueType& v) { return v.IsString(); }
        static BeatSaverVoting::VoteType Get(const ValueType& v) { return BeatSaverVoting::Deserialize_VoteType(v); }                                                                                      \
        static ValueType& Set(ValueType& v, BeatSaverVoting::VoteType data, typename ValueType::AllocatorType& allocator) {
            return BeatSaverVoting::Serialize_VoteType(data, v, allocator);
        }
    };
}
RAPIDJSON_NAMESPACE_END

#pragma once

#include "beatsaverplusplus/shared/Models/Beatmap.hpp"

namespace BeatSaverVoting {
    struct Song {
        int upVotes;
        int downVotes;
        std::string key;
        std::string hash;

        Song() {}

        Song(BeatSaver::Models::Beatmap const& beatmap) : upVotes(beatmap.Stats.UpVotes), downVotes(beatmap.Stats.DownVotes), hash(beatmap.Versions.front().Hash), key(beatmap.Id) {}

        Song(rapidjson::Value const& v) {
            upVotes = v["stats"]["upvotes"].GetInt();
            downVotes = v["stats"]["downvotes"].GetInt();

            hash = v["versions"].GetArray()[0]["hash"].Get<std::string>();
            key = v["id"].Get<std::string>();
        }

        bool operator==(Song const& other) { return hash == other.hash; }
    };
}

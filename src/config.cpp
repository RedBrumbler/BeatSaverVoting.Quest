#include "config.hpp"
#include "logging.hpp"
#include "SongVote.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

Config_t config;

Configuration& get_config() {
    static Configuration config({MOD_ID, VERSION, 0});
    config.Load();
    return config;
}

#define SET(name) doc.AddMember(#name, config.name, allocator)

void SaveConfig() {
    INFO("Saving Config...");
    auto& doc = get_config().config;
    doc.RemoveAllMembers();
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    rapidjson::Value votedSongs;
    votedSongs.SetArray();
    for (auto& [hash, vote] : config.votedSongs) {
        rapidjson::Value val;
        val.Set<BeatSaverVoting::SongVote>({hash, vote}, allocator);
        votedSongs.PushBack(val, allocator);
    }

    doc.AddMember("votedSongs", votedSongs, allocator);
    get_config().Write();

    INFO("Config Saved!");
}

#define GET(name) \
if (auto name##_itr = doc.FindMember(#name); name##_itr != doc.MemberEnd() && name##_itr.Is<decltype(config.name)>()) {   \
    config.name = name##_itr->value.Get<decltype(config.name)>();                \
} else {                                                                        \
    foundEverything = false;                                                    \
}

bool LoadConfig() {
    INFO("Loading Config...");
    bool foundEverything = true;
    auto& doc = get_config().config;

    auto votedSongsItr = doc.FindMember("votedSongs");
    if (votedSongsItr != doc.MemberEnd() && votedSongsItr->value.IsArray()) {
        config.votedSongs.clear();
        for (auto& song : votedSongsItr->value.GetArray()) {
            auto [hash, vote] = song.Get<BeatSaverVoting::SongVote>();
            config.votedSongs[hash] = vote;
        }
    } else {
        foundEverything = false;
    }

    if (foundEverything) INFO("Config Loaded!");
    return foundEverything;
}

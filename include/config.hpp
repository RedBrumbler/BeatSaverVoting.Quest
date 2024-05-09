#pragma once

#include "VoteStatus.hpp"

struct Config_t {
    std::map<std::string, BeatSaverVoting::VoteType> votedSongs;
};

extern Config_t config;
void SaveConfig();
bool LoadConfig();

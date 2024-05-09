#pragma once

#define BEATSAVER_VOTING_EXPORT __attribute__((visibility("default")))
#ifdef __cplusplus
#define BEATSAVER_VOTING_EXPORT_FUNC extern "C" BEATSAVER_VOTING_EXPORT
#else
#define BEATSAVER_VOTING_EXPORT_FUNC BEATSAVER_VOTING_EXPORT
#endif


/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <string>
#include <optional>
#include <functional>
#include <vector>

namespace RNTrackPlayerCodegen {

struct TrackPlayerSpec_Constants {
    double CAPABILITY_PLAY;
    double CAPABILITY_PLAY_FROM_ID;
    double CAPABILITY_PLAY_FROM_SEARCH;
    double CAPABILITY_PAUSE;
    double CAPABILITY_STOP;
    double CAPABILITY_SEEK_TO;
    double CAPABILITY_SKIP;
    double CAPABILITY_SKIP_TO_NEXT;
    double CAPABILITY_SKIP_TO_PREVIOUS;
    double CAPABILITY_SET_RATING;
    double CAPABILITY_JUMP_FORWARD;
    double CAPABILITY_JUMP_BACKWARD;
    std::string STATE_NONE;
    std::string STATE_READY;
    std::string STATE_PLAYING;
    std::string STATE_PAUSED;
    std::string STATE_STOPPED;
    std::string STATE_BUFFERING;
    std::string STATE_LOADING;
    double RATING_HEART;
    double RATING_THUMBS_UP_DOWN;
    double RATING_3_STARS;
    double RATING_4_STARS;
    double RATING_5_STARS;
    double RATING_PERCENTAGE;
    double REPEAT_OFF;
    double REPEAT_TRACK;
    double REPEAT_QUEUE;
    double PITCH_ALGORITHM_LINEAR;
    double PITCH_ALGORITHM_MUSIC;
    double PITCH_ALGORITHM_VOICE;
};

} // namespace RNTrackPlayerCodegen

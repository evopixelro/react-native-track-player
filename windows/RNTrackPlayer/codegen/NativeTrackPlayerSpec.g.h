
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

// #include "NativeTrackPlayerDataTypes.g.h" before this file to use the generated type definition
#include <NativeModules.h>
#include <tuple>

namespace RNTrackPlayerCodegen {

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(TrackPlayerSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"CAPABILITY_PLAY", &TrackPlayerSpec_Constants::CAPABILITY_PLAY},
        {L"CAPABILITY_PLAY_FROM_ID", &TrackPlayerSpec_Constants::CAPABILITY_PLAY_FROM_ID},
        {L"CAPABILITY_PLAY_FROM_SEARCH", &TrackPlayerSpec_Constants::CAPABILITY_PLAY_FROM_SEARCH},
        {L"CAPABILITY_PAUSE", &TrackPlayerSpec_Constants::CAPABILITY_PAUSE},
        {L"CAPABILITY_STOP", &TrackPlayerSpec_Constants::CAPABILITY_STOP},
        {L"CAPABILITY_SEEK_TO", &TrackPlayerSpec_Constants::CAPABILITY_SEEK_TO},
        {L"CAPABILITY_SKIP", &TrackPlayerSpec_Constants::CAPABILITY_SKIP},
        {L"CAPABILITY_SKIP_TO_NEXT", &TrackPlayerSpec_Constants::CAPABILITY_SKIP_TO_NEXT},
        {L"CAPABILITY_SKIP_TO_PREVIOUS", &TrackPlayerSpec_Constants::CAPABILITY_SKIP_TO_PREVIOUS},
        {L"CAPABILITY_SET_RATING", &TrackPlayerSpec_Constants::CAPABILITY_SET_RATING},
        {L"CAPABILITY_JUMP_FORWARD", &TrackPlayerSpec_Constants::CAPABILITY_JUMP_FORWARD},
        {L"CAPABILITY_JUMP_BACKWARD", &TrackPlayerSpec_Constants::CAPABILITY_JUMP_BACKWARD},
        {L"STATE_NONE", &TrackPlayerSpec_Constants::STATE_NONE},
        {L"STATE_READY", &TrackPlayerSpec_Constants::STATE_READY},
        {L"STATE_PLAYING", &TrackPlayerSpec_Constants::STATE_PLAYING},
        {L"STATE_PAUSED", &TrackPlayerSpec_Constants::STATE_PAUSED},
        {L"STATE_STOPPED", &TrackPlayerSpec_Constants::STATE_STOPPED},
        {L"STATE_BUFFERING", &TrackPlayerSpec_Constants::STATE_BUFFERING},
        {L"STATE_LOADING", &TrackPlayerSpec_Constants::STATE_LOADING},
        {L"RATING_HEART", &TrackPlayerSpec_Constants::RATING_HEART},
        {L"RATING_THUMBS_UP_DOWN", &TrackPlayerSpec_Constants::RATING_THUMBS_UP_DOWN},
        {L"RATING_3_STARS", &TrackPlayerSpec_Constants::RATING_3_STARS},
        {L"RATING_4_STARS", &TrackPlayerSpec_Constants::RATING_4_STARS},
        {L"RATING_5_STARS", &TrackPlayerSpec_Constants::RATING_5_STARS},
        {L"RATING_PERCENTAGE", &TrackPlayerSpec_Constants::RATING_PERCENTAGE},
        {L"REPEAT_OFF", &TrackPlayerSpec_Constants::REPEAT_OFF},
        {L"REPEAT_TRACK", &TrackPlayerSpec_Constants::REPEAT_TRACK},
        {L"REPEAT_QUEUE", &TrackPlayerSpec_Constants::REPEAT_QUEUE},
        {L"PITCH_ALGORITHM_LINEAR", &TrackPlayerSpec_Constants::PITCH_ALGORITHM_LINEAR},
        {L"PITCH_ALGORITHM_MUSIC", &TrackPlayerSpec_Constants::PITCH_ALGORITHM_MUSIC},
        {L"PITCH_ALGORITHM_VOICE", &TrackPlayerSpec_Constants::PITCH_ALGORITHM_VOICE},
    };
    return fieldMap;
}

struct TrackPlayerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<TrackPlayerSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(::React::JSValue, Promise<void>) noexcept>{0, L"setupPlayer"},
      Method<void(::React::JSValue, Promise<void>) noexcept>{1, L"updateOptions"},
      Method<void(::React::JSValue, Promise<std::optional<double>>) noexcept>{2, L"load"},
      Method<void(Promise<void>) noexcept>{3, L"reset"},
      Method<void(Promise<void>) noexcept>{4, L"play"},
      Method<void(Promise<void>) noexcept>{5, L"pause"},
      Method<void(Promise<void>) noexcept>{6, L"stop"},
      Method<void(bool, Promise<bool>) noexcept>{7, L"setPlayWhenReady"},
      Method<void(Promise<bool>) noexcept>{8, L"getPlayWhenReady"},
      Method<void(double, Promise<void>) noexcept>{9, L"seekTo"},
      Method<void(double, Promise<void>) noexcept>{10, L"seekBy"},
      Method<void(double, Promise<void>) noexcept>{11, L"setVolume"},
      Method<void(Promise<double>) noexcept>{12, L"getVolume"},
      Method<void(double, Promise<void>) noexcept>{13, L"setRate"},
      Method<void(Promise<double>) noexcept>{14, L"getRate"},
      Method<void(Promise<::React::JSValue>) noexcept>{15, L"getProgress"},
      Method<void(Promise<::React::JSValue>) noexcept>{16, L"getPlaybackState"},
      Method<void(Promise<void>) noexcept>{17, L"retry"},
      Method<void(std::vector<::React::JSValue>, double, Promise<std::optional<double>>) noexcept>{18, L"add"},
      Method<void(double, double, Promise<void>) noexcept>{19, L"move"},
      Method<void(std::vector<double>, Promise<void>) noexcept>{20, L"remove"},
      Method<void(Promise<void>) noexcept>{21, L"removeUpcomingTracks"},
      Method<void(double, double, Promise<void>) noexcept>{22, L"skip"},
      Method<void(double, Promise<void>) noexcept>{23, L"skipToNext"},
      Method<void(double, Promise<void>) noexcept>{24, L"skipToPrevious"},
      Method<void(double, ::React::JSValue, Promise<void>) noexcept>{25, L"updateMetadataForTrack"},
      Method<void(::React::JSValue, Promise<void>) noexcept>{26, L"updateNowPlayingMetadata"},
      Method<void(std::vector<::React::JSValue>, Promise<void>) noexcept>{27, L"setQueue"},
      Method<void(Promise<std::vector<::React::JSValue>>) noexcept>{28, L"getQueue"},
      Method<void(double, Promise<double>) noexcept>{29, L"setRepeatMode"},
      Method<void(Promise<double>) noexcept>{30, L"getRepeatMode"},
      Method<void(double, Promise<std::optional<::React::JSValue>>) noexcept>{31, L"getTrack"},
      Method<void(Promise<std::optional<double>>) noexcept>{32, L"getActiveTrackIndex"},
      Method<void(Promise<std::optional<::React::JSValue>>) noexcept>{33, L"getActiveTrack"},
      Method<void(std::string) noexcept>{34, L"addListener"},
      Method<void(double) noexcept>{35, L"removeListeners"},
      Method<void(Promise<void>) noexcept>{36, L"acquireWakeLock"},
      Method<void(Promise<void>) noexcept>{37, L"abandonWakeLock"},
      Method<void(Promise<bool>) noexcept>{38, L"validateOnStartCommandIntent"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, TrackPlayerSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, TrackPlayerSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "TrackPlayerSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) TrackPlayerSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static TrackPlayerSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setupPlayer",
          "    REACT_METHOD(setupPlayer) void setupPlayer(::React::JSValue && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setupPlayer) static void setupPlayer(::React::JSValue && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "updateOptions",
          "    REACT_METHOD(updateOptions) void updateOptions(::React::JSValue && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(updateOptions) static void updateOptions(::React::JSValue && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "load",
          "    REACT_METHOD(load) void load(::React::JSValue && track, ::React::ReactPromise<std::optional<double>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(load) static void load(::React::JSValue && track, ::React::ReactPromise<std::optional<double>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "reset",
          "    REACT_METHOD(reset) void reset(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(reset) static void reset(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "play",
          "    REACT_METHOD(play) void play(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(play) static void play(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "pause",
          "    REACT_METHOD(pause) void pause(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(pause) static void pause(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "stop",
          "    REACT_METHOD(stop) void stop(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stop) static void stop(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "setPlayWhenReady",
          "    REACT_METHOD(setPlayWhenReady) void setPlayWhenReady(bool playWhenReady, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setPlayWhenReady) static void setPlayWhenReady(bool playWhenReady, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getPlayWhenReady",
          "    REACT_METHOD(getPlayWhenReady) void getPlayWhenReady(::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getPlayWhenReady) static void getPlayWhenReady(::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "seekTo",
          "    REACT_METHOD(seekTo) void seekTo(double position, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(seekTo) static void seekTo(double position, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "seekBy",
          "    REACT_METHOD(seekBy) void seekBy(double offset, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(seekBy) static void seekBy(double offset, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "setVolume",
          "    REACT_METHOD(setVolume) void setVolume(double level, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setVolume) static void setVolume(double level, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "getVolume",
          "    REACT_METHOD(getVolume) void getVolume(::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getVolume) static void getVolume(::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "setRate",
          "    REACT_METHOD(setRate) void setRate(double rate, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setRate) static void setRate(double rate, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "getRate",
          "    REACT_METHOD(getRate) void getRate(::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getRate) static void getRate(::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "getProgress",
          "    REACT_METHOD(getProgress) void getProgress(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getProgress) static void getProgress(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "getPlaybackState",
          "    REACT_METHOD(getPlaybackState) void getPlaybackState(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getPlaybackState) static void getPlaybackState(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "retry",
          "    REACT_METHOD(retry) void retry(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(retry) static void retry(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "add",
          "    REACT_METHOD(add) void add(std::vector<::React::JSValue> const & tracks, double insertBeforeIndex, ::React::ReactPromise<std::optional<double>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(add) static void add(std::vector<::React::JSValue> const & tracks, double insertBeforeIndex, ::React::ReactPromise<std::optional<double>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "move",
          "    REACT_METHOD(move) void move(double fromIndex, double toIndex, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(move) static void move(double fromIndex, double toIndex, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "remove",
          "    REACT_METHOD(remove) void remove(std::vector<double> const & indexes, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(remove) static void remove(std::vector<double> const & indexes, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "removeUpcomingTracks",
          "    REACT_METHOD(removeUpcomingTracks) void removeUpcomingTracks(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeUpcomingTracks) static void removeUpcomingTracks(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "skip",
          "    REACT_METHOD(skip) void skip(double index, double initialPosition, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(skip) static void skip(double index, double initialPosition, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "skipToNext",
          "    REACT_METHOD(skipToNext) void skipToNext(double initialPosition, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(skipToNext) static void skipToNext(double initialPosition, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "skipToPrevious",
          "    REACT_METHOD(skipToPrevious) void skipToPrevious(double initialPosition, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(skipToPrevious) static void skipToPrevious(double initialPosition, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          25,
          "updateMetadataForTrack",
          "    REACT_METHOD(updateMetadataForTrack) void updateMetadataForTrack(double trackIndex, ::React::JSValue && metadata, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(updateMetadataForTrack) static void updateMetadataForTrack(double trackIndex, ::React::JSValue && metadata, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          26,
          "updateNowPlayingMetadata",
          "    REACT_METHOD(updateNowPlayingMetadata) void updateNowPlayingMetadata(::React::JSValue && metadata, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(updateNowPlayingMetadata) static void updateNowPlayingMetadata(::React::JSValue && metadata, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          27,
          "setQueue",
          "    REACT_METHOD(setQueue) void setQueue(std::vector<::React::JSValue> const & tracks, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setQueue) static void setQueue(std::vector<::React::JSValue> const & tracks, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          28,
          "getQueue",
          "    REACT_METHOD(getQueue) void getQueue(::React::ReactPromise<std::vector<::React::JSValue>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getQueue) static void getQueue(::React::ReactPromise<std::vector<::React::JSValue>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          29,
          "setRepeatMode",
          "    REACT_METHOD(setRepeatMode) void setRepeatMode(double mode, ::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setRepeatMode) static void setRepeatMode(double mode, ::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          30,
          "getRepeatMode",
          "    REACT_METHOD(getRepeatMode) void getRepeatMode(::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getRepeatMode) static void getRepeatMode(::React::ReactPromise<double> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          31,
          "getTrack",
          "    REACT_METHOD(getTrack) void getTrack(double index, ::React::ReactPromise<std::optional<::React::JSValue>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getTrack) static void getTrack(double index, ::React::ReactPromise<std::optional<::React::JSValue>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          32,
          "getActiveTrackIndex",
          "    REACT_METHOD(getActiveTrackIndex) void getActiveTrackIndex(::React::ReactPromise<std::optional<double>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getActiveTrackIndex) static void getActiveTrackIndex(::React::ReactPromise<std::optional<double>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          33,
          "getActiveTrack",
          "    REACT_METHOD(getActiveTrack) void getActiveTrack(::React::ReactPromise<std::optional<::React::JSValue>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getActiveTrack) static void getActiveTrack(::React::ReactPromise<std::optional<::React::JSValue>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          34,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          35,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          36,
          "acquireWakeLock",
          "    REACT_METHOD(acquireWakeLock) void acquireWakeLock(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(acquireWakeLock) static void acquireWakeLock(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          37,
          "abandonWakeLock",
          "    REACT_METHOD(abandonWakeLock) void abandonWakeLock(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(abandonWakeLock) static void abandonWakeLock(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          38,
          "validateOnStartCommandIntent",
          "    REACT_METHOD(validateOnStartCommandIntent) void validateOnStartCommandIntent(::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(validateOnStartCommandIntent) static void validateOnStartCommandIntent(::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace RNTrackPlayerCodegen

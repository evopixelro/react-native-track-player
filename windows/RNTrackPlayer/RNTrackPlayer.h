// Modified by EvoPixel to implement the shared TrackPlayer TurboModule contract on Windows.
#pragma once

#include "NativeModules.h"
#include "codegen/NativeTrackPlayerDataTypes.g.h"
#include "codegen/NativeTrackPlayerSpec.g.h"
#include "Logic/MediaManager.h"
#include "Players/LocalPlayback.h"
#include <type_traits>

namespace winrt::RNTrackPlayer {
    using namespace Microsoft::ReactNative;

    REACT_MODULE(TrackPlayer)
    struct TrackPlayer {
        using ModuleSpec = RNTrackPlayerCodegen::TrackPlayerSpec;
        std::shared_ptr<MediaManager> manager;

        REACT_INIT(Initialize)
        void Initialize(const ReactContext& context) noexcept {
            manager = std::make_shared<MediaManager>(context);
        }

        // Resolve every promise exactly once and translate native failures to JS errors.
        template <typename T, typename Callback>
        void Complete(const ReactPromise<T>& promise, Callback callback) noexcept {
            try {
                if (!manager) throw std::logic_error("player_not_initialized");
                std::lock_guard<std::recursive_mutex> lock(manager->mutex);
                if constexpr (std::is_void_v<T>) { callback(*manager); promise.Resolve(); }
                else promise.Resolve(callback(*manager));
            } catch (const std::logic_error& ex) {
                const std::string message = ex.what();
                const auto code = message == "player_not_initialized" || message == "player_already_initialized"
                    ? message : "windows-invalid-argument";
                promise.Reject(ReactError{code, message, {}});
            } catch (const winrt::hresult_error& ex) {
                promise.Reject(ReactError{"windows-playback", winrt::to_string(ex.message()), {}});
            } catch (const std::exception& ex) {
                promise.Reject(ReactError{"windows-playback", ex.what(), {}});
            } catch (...) { promise.Reject("Unexpected Windows playback failure"); }
        }

        REACT_GET_CONSTANTS(GetConstants)
        RNTrackPlayerCodegen::TrackPlayerSpec_Constants GetConstants() noexcept {
            return {1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 10, 11,
                "none", "ready", "playing", "paused", "stopped", "buffering", "loading",
                1, 2, 3, 4, 5, 6, 0, 1, 2, 1, 2, 3};
        }

        REACT_METHOD(setupPlayer)
        void setupPlayer(JSValue options, ReactPromise<void> promise) noexcept {
            Complete(promise, [&](auto& owner) { owner.Setup(options); });
        }

        REACT_METHOD(updateOptions)
        void updateOptions(JSValue options, ReactPromise<void> promise) noexcept {
            Complete(promise, [&](auto& owner) { owner.UpdateOptions(options); });
        }

        REACT_METHOD(load)
        void load(JSValue track, ReactPromise<std::optional<double>> promise) noexcept {
            Complete(promise, [&](auto& owner) { return owner.Load(track); });
        }

        REACT_METHOD(reset)
        void reset(ReactPromise<void> promise) noexcept { Complete(promise, [](auto& owner) { owner.Reset(); }); }

        REACT_METHOD(play)
        void play(ReactPromise<void> promise) noexcept { Complete(promise, [](auto& owner) { owner.Play(); }); }

        REACT_METHOD(pause)
        void pause(ReactPromise<void> promise) noexcept { Complete(promise, [](auto& owner) { owner.Pause(); }); }

        REACT_METHOD(stop)
        void stop(ReactPromise<void> promise) noexcept { Complete(promise, [](auto& owner) { owner.Stop(); }); }

        REACT_METHOD(setPlayWhenReady)
        void setPlayWhenReady(bool ready, ReactPromise<bool> promise) noexcept {
            Complete(promise, [ready](auto& owner) { if (ready) owner.Play(); else owner.Pause(); return owner.playWhenReady; });
        }

        REACT_METHOD(getPlayWhenReady)
        void getPlayWhenReady(ReactPromise<bool> promise) noexcept {
            Complete(promise, [](auto& owner) { owner.RequirePlayer(); return owner.playWhenReady; });
        }

        REACT_METHOD(seekTo)
        void seekTo(double position, ReactPromise<void> promise) noexcept {
            Complete(promise, [position](auto& owner) { owner.Seek(position); });
        }

        REACT_METHOD(seekBy)
        void seekBy(double offset, ReactPromise<void> promise) noexcept {
            Complete(promise, [offset](auto& owner) {
                if (!std::isfinite(offset)) throw std::invalid_argument("Seek offset must be finite");
                owner.Seek(std::max(0.0, owner.Position() + offset));
            });
        }

        REACT_METHOD(setVolume)
        void setVolume(double level, ReactPromise<void> promise) noexcept {
            Complete(promise, [level](auto& owner) {
                owner.RequirePlayer();
                if (!std::isfinite(level) || level < 0 || level > 1) throw std::invalid_argument("Volume must be between 0 and 1");
                owner.player->player.Volume(level);
            });
        }

        REACT_METHOD(getVolume)
        void getVolume(ReactPromise<double> promise) noexcept {
            Complete(promise, [](auto& owner) { owner.RequirePlayer(); return owner.player->player.Volume(); });
        }

        REACT_METHOD(setRate)
        void setRate(double rate, ReactPromise<void> promise) noexcept {
            Complete(promise, [rate](auto& owner) {
                owner.RequirePlayer();
                if (!std::isfinite(rate) || rate <= 0) throw std::invalid_argument("Rate must be positive");
                owner.player->player.PlaybackSession().PlaybackRate(rate);
            });
        }

        REACT_METHOD(getRate)
        void getRate(ReactPromise<double> promise) noexcept {
            Complete(promise, [](auto& owner) { owner.RequirePlayer(); return owner.player->player.PlaybackSession().PlaybackRate(); });
        }

        REACT_METHOD(getProgress)
        void getProgress(ReactPromise<JSValue> promise) noexcept {
            Complete(promise, [](auto& owner) { owner.RequirePlayer(); return owner.Progress(); });
        }

        REACT_METHOD(getPlaybackState)
        void getPlaybackState(ReactPromise<JSValue> promise) noexcept {
            Complete(promise, [](auto& owner) { return owner.PlaybackState(); });
        }

        REACT_METHOD(retry)
        void retry(ReactPromise<void> promise) noexcept { Complete(promise, [](auto& owner) { owner.Retry(); }); }

        REACT_METHOD(add)
        void add(std::vector<JSValue> tracks, double before, ReactPromise<std::optional<double>> promise) noexcept {
            Complete(promise, [&](auto& owner) { return owner.Add(std::move(tracks), before); });
        }

        REACT_METHOD(move)
        void move(double from, double to, ReactPromise<void> promise) noexcept {
            Complete(promise, [from, to](auto& owner) { owner.Move(from, to); });
        }

        REACT_METHOD(remove)
        void remove(std::vector<double> indexes, ReactPromise<void> promise) noexcept {
            Complete(promise, [&](auto& owner) { owner.Remove(indexes); });
        }

        REACT_METHOD(removeUpcomingTracks)
        void removeUpcomingTracks(ReactPromise<void> promise) noexcept {
            Complete(promise, [](auto& owner) { owner.RequirePlayer(); owner.playlist.RemoveUpcomingTracks(); });
        }

        REACT_METHOD(skip)
        void skip(double index, double position, ReactPromise<void> promise) noexcept {
            Complete(promise, [index, position](auto& owner) {
                owner.Select(Playback<Track>::Index(index, owner.playlist.queue.size()), position == -1 ? 0 : position);
            });
        }

        REACT_METHOD(skipToNext)
        void skipToNext(double position, ReactPromise<void> promise) noexcept {
            Complete(promise, [position](auto& owner) { owner.Next(position); });
        }

        REACT_METHOD(skipToPrevious)
        void skipToPrevious(double position, ReactPromise<void> promise) noexcept {
            Complete(promise, [position](auto& owner) { owner.Previous(position); });
        }

        REACT_METHOD(updateMetadataForTrack)
        void updateMetadataForTrack(double index, JSValue data, ReactPromise<void> promise) noexcept {
            Complete(promise, [&](auto& owner) { owner.UpdateTrack(index, data); });
        }

        REACT_METHOD(updateNowPlayingMetadata)
        void updateNowPlayingMetadata(JSValue data, ReactPromise<void> promise) noexcept {
            Complete(promise, [&](auto& owner) { owner.UpdateNowPlaying(data); });
        }

        REACT_METHOD(setQueue)
        void setQueue(std::vector<JSValue> tracks, ReactPromise<void> promise) noexcept {
            Complete(promise, [&](auto& owner) { owner.SetQueue(std::move(tracks)); });
        }

        REACT_METHOD(getQueue)
        void getQueue(ReactPromise<std::vector<JSValue>> promise) noexcept {
            Complete(promise, [](auto& owner) {
                owner.RequirePlayer();
                std::vector<JSValue> tracks;
                for (const auto& track : owner.playlist.queue) tracks.push_back(track.ToObject());
                return tracks;
            });
        }

        REACT_METHOD(setRepeatMode)
        void setRepeatMode(double mode, ReactPromise<double> promise) noexcept {
            Complete(promise, [mode](auto& owner) {
                owner.RequirePlayer();
                if (mode != 0 && mode != 1 && mode != 2) throw std::invalid_argument("Invalid repeat mode");
                owner.repeatMode = static_cast<int>(mode);
                return mode;
            });
        }

        REACT_METHOD(getRepeatMode)
        void getRepeatMode(ReactPromise<double> promise) noexcept {
            Complete(promise, [](auto& owner) { owner.RequirePlayer(); return static_cast<double>(owner.repeatMode); });
        }

        REACT_METHOD(getTrack)
        void getTrack(double index, ReactPromise<std::optional<JSValue>> promise) noexcept {
            Complete(promise, [index](auto& owner) -> std::optional<JSValue> {
                owner.RequirePlayer();
                if (!std::isfinite(index) || index < 0 || std::floor(index) != index || index >= owner.playlist.queue.size())
                    return std::nullopt;
                return owner.playlist.queue[static_cast<size_t>(index)].ToObject();
            });
        }

        REACT_METHOD(getActiveTrackIndex)
        void getActiveTrackIndex(ReactPromise<std::optional<double>> promise) noexcept {
            Complete(promise, [](auto& owner) -> std::optional<double> {
                owner.RequirePlayer();
                return owner.playlist.currentTrack ? std::optional<double>(*owner.playlist.currentTrack) : std::nullopt;
            });
        }

        REACT_METHOD(getActiveTrack)
        void getActiveTrack(ReactPromise<std::optional<JSValue>> promise) noexcept {
            Complete(promise, [](auto& owner) -> std::optional<JSValue> {
                owner.RequirePlayer();
                if (!owner.playlist.currentTrack) return std::nullopt;
                return owner.playlist.queue[*owner.playlist.currentTrack].ToObject();
            });
        }

        // NativeEventEmitter manages JS subscriptions; Windows events use the React context.
        REACT_METHOD(addListener)
        void addListener(std::string) noexcept {}

        REACT_METHOD(removeListeners)
        void removeListeners(double) noexcept {}

        // Android-only APIs remain present in the shared spec but are not emulated on Windows.
        REACT_METHOD(acquireWakeLock)
        void acquireWakeLock(ReactPromise<void> promise) noexcept { promise.Reject("Wake locks are Android-only"); }

        REACT_METHOD(abandonWakeLock)
        void abandonWakeLock(ReactPromise<void> promise) noexcept { promise.Reject("Wake locks are Android-only"); }

        REACT_METHOD(validateOnStartCommandIntent)
        void validateOnStartCommandIntent(ReactPromise<bool> promise) noexcept { promise.Resolve(false); }
    };
}

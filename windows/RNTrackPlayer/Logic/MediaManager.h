// Modified by EvoPixel for TurboModule playback, queue events and safe callback ownership.
#pragma once

#include "NativeModules.h"
#include "Track.h"
#include "Players/Playback.h"
#include <memory>
#include <mutex>
#include <winrt/Windows.System.Threading.h>

namespace winrt::RNTrackPlayer {
    struct LocalPlayback;
    struct Metadata;

    struct MediaManager : std::enable_shared_from_this<MediaManager> {
        Microsoft::ReactNative::ReactContext context;
        std::recursive_mutex mutex;
        std::unique_ptr<LocalPlayback> player;
        std::unique_ptr<Metadata> metadata;
        Playback<Track> playlist;
        std::string state = "none";
        Microsoft::ReactNative::JSValueObject error;
        bool playWhenReady = false;
        bool closed = false;
        int repeatMode = 0;
        double progressInterval = 0;
        uint64_t generation = 0;
        Windows::System::Threading::ThreadPoolTimer timer{nullptr};

        explicit MediaManager(const Microsoft::ReactNative::ReactContext& context);
        ~MediaManager();
        void Setup(const Microsoft::ReactNative::JSValue& options);
        void RequirePlayer() const;
        void UpdateOptions(const Microsoft::ReactNative::JSValue& options);
        void SendEvent(const std::string& name, const Microsoft::ReactNative::JSValueObject& data);
        void SetState(const std::string& value);
        void SetPlayWhenReady(bool value);
        void Play();
        void Pause();
        void Stop();
        void Reset();
        void Seek(double position);
        void Select(size_t index, double position = 0);
        void Activate(size_t index, double position);
        void Retry();
        void Next(double position = -1);
        void Previous(double position = -1);
        std::optional<double> Add(std::vector<Microsoft::ReactNative::JSValue> tracks, double before);
        std::optional<double> Load(const Microsoft::ReactNative::JSValue& track);
        void SetQueue(std::vector<Microsoft::ReactNative::JSValue> tracks);
        void Remove(const std::vector<double>& indexes);
        void Move(double from, double to);
        void UpdateTrack(double index, const Microsoft::ReactNative::JSValue& data);
        void UpdateNowPlaying(const Microsoft::ReactNative::JSValue& data);
        Microsoft::ReactNative::JSValue Progress() const;
        Microsoft::ReactNative::JSValue PlaybackState() const;
        double Position() const;
        double Duration() const;
        void OnOpened();
        void OnEnded();
        void OnStateChanged();
        void OnError(const std::string& code, const std::string& message) noexcept;
        void EmitTrackChange(std::optional<size_t> lastIndex,
            const Microsoft::ReactNative::JSValue& lastTrack, double lastPosition);

        template <typename Callback>
        static void Dispatch(const std::weak_ptr<MediaManager>& weak,
            std::optional<uint64_t> expected, Callback callback) noexcept {
            if (auto self = weak.lock()) {
                std::lock_guard<std::recursive_mutex> lock(self->mutex);
                if (self->closed || (expected && self->generation != *expected)) return;
                try { callback(*self); }
                catch (const winrt::hresult_error& ex) { self->OnError("windows-playback", winrt::to_string(ex.message())); }
                catch (const std::exception& ex) { self->OnError("windows-playback", ex.what()); }
                catch (...) { self->OnError("windows-playback", "Unexpected media callback failure"); }
            }
        }
    };
}

// Modified by EvoPixel for fractional seeking and lifetime-safe Windows playback.
#pragma once

#include "Logic/Track.h"
#include <winrt/Windows.Media.Playback.h>

namespace winrt::RNTrackPlayer {
    struct MediaManager;

    struct LocalPlayback {
        Windows::Media::Playback::MediaPlayer player{nullptr};
        double pendingPosition = 0;
        bool opened = false;
        Windows::Media::Playback::MediaPlayer::MediaOpened_revoker openedEvent;
        Windows::Media::Playback::MediaPlayer::MediaEnded_revoker endedEvent;
        Windows::Media::Playback::MediaPlayer::MediaFailed_revoker failedEvent;
        Windows::Media::Playback::MediaPlaybackSession::PlaybackStateChanged_revoker stateEvent;

        LocalPlayback();
        ~LocalPlayback();
        void Load(const Track& track, double position, const std::shared_ptr<MediaManager>& manager);
        void Clear();
        void Seek(double seconds);
        double Position() const;
        double Duration() const;
        double BufferedPosition() const;
        static Windows::Foundation::TimeSpan Time(double seconds);
    };
}

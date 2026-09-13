#include "pch.h"
#include "LocalPlayback.h"
#include "Logic/MediaManager.h"
#include <cmath>
#include <limits>

using namespace winrt::RNTrackPlayer;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Core;
using namespace winrt::Windows::Media::Playback;

LocalPlayback::LocalPlayback() {
    player = MediaPlayer();
    player.AutoPlay(false);
    player.AudioCategory(MediaPlayerAudioCategory::Media);
    player.CommandManager().IsEnabled(false);
}

LocalPlayback::~LocalPlayback() {
    openedEvent.revoke();
    endedEvent.revoke();
    failedEvent.revoke();
    stateEvent.revoke();
    try { if (player) { player.Pause(); player.Close(); } } catch (...) {}
}

TimeSpan LocalPlayback::Time(double seconds) {
    const double limit = static_cast<double>(std::numeric_limits<int64_t>::max()) / 10000000.0;
    if (!std::isfinite(seconds) || seconds < 0 || seconds >= limit)
        throw std::invalid_argument("Playback position must be a finite, nonnegative number");
    return TimeSpan(static_cast<int64_t>(std::llround(seconds * 10000000.0)));
}

void LocalPlayback::Load(const Track& track, double position, const std::shared_ptr<MediaManager>& manager) {
    Time(position);
    auto source = MediaSource::CreateFromUri(Uri(winrt::to_hstring(track.Url())));
    const auto generation = ++manager->generation;
    const std::weak_ptr<MediaManager> weak = manager;
    openedEvent.revoke();
    endedEvent.revoke();
    failedEvent.revoke();
    stateEvent.revoke();
    opened = false;
    pendingPosition = position;
    openedEvent = player.MediaOpened(winrt::auto_revoke, [weak, generation](const auto&, const auto&) {
        MediaManager::Dispatch(weak, generation, [](auto& owner) { owner.OnOpened(); });
    });
    endedEvent = player.MediaEnded(winrt::auto_revoke, [weak, generation](const auto&, const auto&) {
        MediaManager::Dispatch(weak, generation, [](auto& owner) { owner.OnEnded(); });
    });
    failedEvent = player.MediaFailed(winrt::auto_revoke, [weak, generation](const auto&, const auto& args) {
        const auto message = winrt::to_string(args.ErrorMessage());
        MediaManager::Dispatch(weak, generation, [&message](auto& owner) { owner.OnError("windows-source", message); });
    });
    stateEvent = player.PlaybackSession().PlaybackStateChanged(winrt::auto_revoke,
        [weak, generation](const auto&, const auto&) {
            MediaManager::Dispatch(weak, generation, [](auto& owner) { owner.OnStateChanged(); });
        });
    player.Source(source);
}

void LocalPlayback::Clear() {
    openedEvent.revoke();
    endedEvent.revoke();
    failedEvent.revoke();
    stateEvent.revoke();
    opened = false;
    pendingPosition = 0;
    player.Pause();
    player.Source(nullptr);
}

void LocalPlayback::Seek(double seconds) {
    const auto value = Time(seconds);
    pendingPosition = seconds;
    if (opened) {
        if (!player.PlaybackSession().CanSeek()) throw std::runtime_error("This stream is not seekable");
        const auto duration = player.PlaybackSession().NaturalDuration();
        player.PlaybackSession().Position(duration.count() > 0 && value > duration ? duration : value);
    }
}

double LocalPlayback::Position() const {
    return opened ? player.PlaybackSession().Position().count() / 10000000.0 : pendingPosition;
}

double LocalPlayback::Duration() const { return player.PlaybackSession().NaturalDuration().count() / 10000000.0; }

double LocalPlayback::BufferedPosition() const {
    const auto position = Position();
    for (const auto& range : player.PlaybackSession().GetBufferedRanges()) {
        const auto start = range.Start.count() / 10000000.0;
        const auto end = range.End.count() / 10000000.0;
        if (start <= position && position <= end) return end;
    }
    return position;
}

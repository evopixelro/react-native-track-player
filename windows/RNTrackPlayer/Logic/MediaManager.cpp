#include "pch.h"
#include "MediaManager.h"
#include "Metadata.h"
#include "Players/LocalPlayback.h"

using namespace winrt::RNTrackPlayer;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Media::Playback;
using namespace winrt::Windows::System::Threading;

namespace {
    struct Snapshot {
        std::optional<size_t> index;
        JSValue track;
        double position;
    };

    Snapshot Capture(const MediaManager& manager) {
        const auto index = manager.playlist.currentTrack;
        return {index, index ? manager.playlist.queue[*index].ToObject() : JSValue{}, manager.Position()};
    }

    std::vector<Track> Tracks(const std::vector<JSValue>& values) {
        std::vector<Track> result;
        result.reserve(values.size());
        for (const auto& value : values) result.emplace_back(value);
        return result;
    }
}

MediaManager::MediaManager(const ReactContext& value) : context(value) {}

MediaManager::~MediaManager() {
    closed = true;
    if (timer) timer.Cancel();
    metadata.reset();
    player.reset();
}

void MediaManager::Setup(const JSValue& options) {
    if (player) throw std::logic_error("player_already_initialized");
    if (options.Type() != JSValueType::Object) throw std::invalid_argument("Player options must be an object");
    auto playback = std::make_unique<LocalPlayback>();
    auto controls = std::make_unique<Metadata>(playback->player.SystemMediaTransportControls(), shared_from_this());
    player = std::move(playback);
    metadata = std::move(controls);
    UpdateOptions(JSValueObject{});
}

void MediaManager::RequirePlayer() const {
    if (!player || closed) throw std::logic_error("player_not_initialized");
}

void MediaManager::UpdateOptions(const JSValue& options) {
    RequirePlayer();
    if (options.Type() != JSValueType::Object) throw std::invalid_argument("Player options must be an object");
    const auto& intervalValue = options["progressUpdateEventInterval"];
    const double interval = intervalValue.Type() == JSValueType::Null ? progressInterval : intervalValue.AsDouble();
    if (!std::isfinite(interval) || interval < 0 || interval > 3600)
        throw std::invalid_argument("Progress interval must be between 0 and 3600 seconds");
    metadata->UpdateOptions(options);
    progressInterval = interval;
    if (timer) timer.Cancel();
    const std::weak_ptr<MediaManager> weak = shared_from_this();
    timer = ThreadPoolTimer::CreatePeriodicTimer([weak, interval](const auto&) {
        Dispatch(weak, std::nullopt, [interval](auto& owner) {
            if (!owner.player || !owner.playlist.currentTrack) return;
            owner.metadata->UpdateTimeline(owner.Position(), owner.Duration());
            if (interval > 0 && owner.state == "playing") {
                auto data = owner.Progress().AsObject().Copy();
                data["track"] = static_cast<double>(*owner.playlist.currentTrack);
                owner.SendEvent("playback-progress-updated", data);
            }
        });
    }, LocalPlayback::Time(interval > 0 ? std::max(0.1, interval) : 1.0));
}

void MediaManager::SendEvent(const std::string& name, const JSValueObject& data) {
    if (!closed) context.EmitJSEvent(L"RCTDeviceEventEmitter", winrt::to_hstring(name), data);
}

void MediaManager::SetState(const std::string& value) {
    if (state == value) return;
    state = value;
    if (metadata) metadata->SetState(value);
    SendEvent("playback-state", PlaybackState().AsObject());
}

void MediaManager::SetPlayWhenReady(bool value) {
    if (playWhenReady == value) return;
    playWhenReady = value;
    SendEvent("playback-play-when-ready-changed", JSValueObject{{"playWhenReady", value}});
}

void MediaManager::Play() {
    RequirePlayer();
    SetPlayWhenReady(true);
    if (!playlist.currentTrack) {
        if (!playlist.queue.empty()) Select(0);
        return;
    }
    if (state == "error" || state == "ended" || state == "stopped") {
        Select(*playlist.currentTrack, state == "error" ? Position() : 0);
    } else if (player->opened) player->player.Play();
}

void MediaManager::Pause() {
    RequirePlayer();
    SetPlayWhenReady(false);
    player->player.Pause();
    if (playlist.currentTrack && state != "error") SetState("paused");
}

void MediaManager::Stop() {
    RequirePlayer();
    SetPlayWhenReady(false);
    player->player.Pause();
    if (playlist.currentTrack) {
        if (player->opened && player->player.PlaybackSession().CanSeek()) player->Seek(0);
        SetState("stopped");
    }
}

void MediaManager::Reset() {
    RequirePlayer();
    const auto previous = Capture(*this);
    SetPlayWhenReady(false);
    ++generation;
    player->Clear();
    playlist.Reset();
    error.clear();
    metadata->Set(JSValueObject{});
    SetState("none");
    if (previous.index) EmitTrackChange(previous.index, previous.track, previous.position);
}

void MediaManager::Seek(double position) {
    RequirePlayer();
    if (!playlist.currentTrack) throw std::logic_error("No track is loaded");
    player->Seek(position);
    metadata->UpdateTimeline(Position(), Duration());
}

void MediaManager::Activate(size_t index, double position) {
    playlist.currentTrack = index;
    error.clear();
    try {
        player->Load(playlist.queue[index], position, shared_from_this());
        metadata->Set(playlist.queue[index].ToObject());
        SetState("loading");
    } catch (const std::exception& ex) {
        OnError("windows-source", ex.what());
        throw;
    } catch (const winrt::hresult_error& ex) {
        OnError("windows-source", winrt::to_string(ex.message()));
        throw;
    }
}

void MediaManager::Select(size_t index, double position) {
    RequirePlayer();
    Playback<Track>::Index(static_cast<double>(index), playlist.queue.size());
    LocalPlayback::Time(position);
    const auto previous = Capture(*this);
    Activate(index, position);
    EmitTrackChange(previous.index, previous.track, previous.position);
}

void MediaManager::Retry() {
    RequirePlayer();
    if (playlist.currentTrack) Select(*playlist.currentTrack, Position());
}

void MediaManager::Next(double position) {
    RequirePlayer();
    if (!playlist.currentTrack || *playlist.currentTrack + 1 >= playlist.queue.size())
        throw std::out_of_range("There is no next track");
    Select(*playlist.currentTrack + 1, position == -1 ? 0 : position);
}

void MediaManager::Previous(double position) {
    RequirePlayer();
    if (!playlist.currentTrack || *playlist.currentTrack == 0)
        throw std::out_of_range("There is no previous track");
    Select(*playlist.currentTrack - 1, position == -1 ? 0 : position);
}

std::optional<double> MediaManager::Add(std::vector<JSValue> values, double before) {
    RequirePlayer();
    const auto previous = Capture(*this);
    const auto index = playlist.Add(Tracks(values), before);
    if (index && !playlist.currentTrack) Select(0);
    else if (previous.index != playlist.currentTrack)
        EmitTrackChange(previous.index, previous.track, previous.position);
    return index ? std::optional<double>(static_cast<double>(*index)) : std::nullopt;
}

std::optional<double> MediaManager::Load(const JSValue& value) {
    RequirePlayer();
    Track track(value);
    const auto previous = Capture(*this);
    const auto index = playlist.currentTrack.value_or(0);
    if (playlist.queue.empty()) playlist.queue.push_back(std::move(track));
    else playlist.queue[index] = std::move(track);
    Activate(index, 0);
    EmitTrackChange(previous.index, previous.track, previous.position);
    return static_cast<double>(index);
}

void MediaManager::SetQueue(std::vector<JSValue> values) {
    RequirePlayer();
    auto tracks = Tracks(values);
    if (tracks.empty()) { Reset(); return; }
    const auto previous = Capture(*this);
    playlist.queue = std::move(tracks);
    Activate(0, 0);
    EmitTrackChange(previous.index, previous.track, previous.position);
}

void MediaManager::Remove(const std::vector<double>& indexes) {
    RequirePlayer();
    const auto previous = Capture(*this);
    const bool reload = playlist.Remove(indexes);
    if (playlist.queue.empty()) {
        ++generation;
        player->Clear();
        SetPlayWhenReady(false);
        error.clear();
        metadata->Set(JSValueObject{});
        SetState("none");
    } else if (reload) Activate(*playlist.currentTrack, 0);
    if (reload || previous.index != playlist.currentTrack)
        EmitTrackChange(previous.index, previous.track, previous.position);
}

void MediaManager::Move(double from, double to) {
    RequirePlayer();
    const auto previous = Capture(*this);
    playlist.Move(from, to);
    if (previous.index != playlist.currentTrack)
        EmitTrackChange(previous.index, previous.track, previous.position);
}

void MediaManager::UpdateTrack(double index, const JSValue& value) {
    RequirePlayer();
    const auto target = Playback<Track>::Index(index, playlist.queue.size());
    playlist.queue[target].SetMetadata(value);
    if (playlist.currentTrack == target) metadata->Set(playlist.queue[target].ToObject());
}

void MediaManager::UpdateNowPlaying(const JSValue& value) {
    RequirePlayer();
    if (value.Type() != JSValueType::Object) throw std::invalid_argument("Metadata must be an object");
    if (playlist.currentTrack) metadata->Set(value, true);
}

double MediaManager::Position() const { return player && playlist.currentTrack ? player->Position() : 0; }

double MediaManager::Duration() const {
    if (!player || !playlist.currentTrack) return 0;
    const auto natural = player->Duration();
    const auto declared = playlist.queue[*playlist.currentTrack].data["duration"].AsDouble();
    return natural > 0 ? natural : std::isfinite(declared) && declared > 0 ? declared : 0;
}

JSValue MediaManager::Progress() const {
    return JSValueObject{{"position", Position()}, {"duration", Duration()},
        {"buffered", player && playlist.currentTrack ? player->BufferedPosition() : 0}};
}

JSValue MediaManager::PlaybackState() const {
    JSValueObject result{{"state", state}};
    if (!error.empty()) result["error"] = error.Copy();
    return result;
}

void MediaManager::OnOpened() {
    if (!playlist.currentTrack || state == "error") return;
    player->opened = true;
    if (player->pendingPosition > 0) player->Seek(player->pendingPosition);
    metadata->UpdateTimeline(Position(), Duration());
    if (playWhenReady) player->player.Play();
    else if (state != "paused" && state != "stopped") SetState("ready");
}

void MediaManager::OnStateChanged() {
    if (!playlist.currentTrack || state == "error" || state == "ended" || state == "stopped") return;
    switch (player->player.PlaybackSession().PlaybackState()) {
        case MediaPlaybackState::Opening: SetState("loading"); break;
        case MediaPlaybackState::Buffering: SetState("buffering"); break;
        case MediaPlaybackState::Playing: SetState("playing"); break;
        case MediaPlaybackState::Paused:
            if (player->opened && state != "ready") SetState("paused");
            break;
        default: break;
    }
}

void MediaManager::OnEnded() {
    if (!playlist.currentTrack || !player->opened || state == "error" || state == "stopped") return;
    if (playWhenReady) {
        if (repeatMode == 1) { Select(*playlist.currentTrack); return; }
        if (*playlist.currentTrack + 1 < playlist.queue.size()) { Next(); return; }
        if (repeatMode == 2) { Select(0); return; }
    }
    SetState("ended");
    SendEvent("playback-queue-ended", JSValueObject{
        {"track", static_cast<double>(*playlist.currentTrack)}, {"position", Position()}});
}

void MediaManager::OnError(const std::string& code, const std::string& message) noexcept {
    try {
        if (player) player->player.Pause();
        error = JSValueObject{{"code", code}, {"message", message}};
        SetState("error");
        SendEvent("playback-error", error);
    } catch (...) {}
}

void MediaManager::EmitTrackChange(std::optional<size_t> lastIndex, const JSValue& lastTrack, double lastPosition) {
    JSValueObject active{{"lastPosition", lastPosition}};
    JSValueObject legacy{{"position", lastPosition}};
    if (lastIndex) {
        active["lastIndex"] = static_cast<double>(*lastIndex);
        active["lastTrack"] = lastTrack.Copy();
        legacy["track"] = static_cast<double>(*lastIndex);
    }
    if (playlist.currentTrack) {
        active["index"] = static_cast<double>(*playlist.currentTrack);
        active["track"] = playlist.queue[*playlist.currentTrack].ToObject();
        legacy["nextTrack"] = static_cast<double>(*playlist.currentTrack);
    }
    SendEvent("playback-active-track-changed", active);
    SendEvent("playback-track-changed", legacy);
}

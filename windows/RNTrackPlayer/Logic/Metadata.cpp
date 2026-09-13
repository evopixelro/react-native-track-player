#include "pch.h"
#include "Metadata.h"
#include "MediaManager.h"
#include "Players/LocalPlayback.h"
#include <cmath>

using namespace winrt::RNTrackPlayer;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Storage::Streams;

Metadata::Metadata(SystemMediaTransportControls transport, const std::shared_ptr<MediaManager>& owner)
    : controls(transport) {
    const std::weak_ptr<MediaManager> weak = owner;
    controls.IsEnabled(false);
    buttonEvent = controls.ButtonPressed(winrt::auto_revoke, [weak](const auto&, const auto& args) {
        const auto button = args.Button();
        MediaManager::Dispatch(weak, std::nullopt, [button](auto& manager) {
            const auto& metadata = *manager.metadata;
            const char* event = nullptr;
            int capability = 0;
            JSValueObject payload;
            switch (button) {
                case SystemMediaTransportControlsButton::Play: event = "remote-play"; capability = 1; break;
                case SystemMediaTransportControlsButton::Pause: event = "remote-pause"; capability = 4; break;
                case SystemMediaTransportControlsButton::Stop: event = "remote-stop"; capability = 5; break;
                case SystemMediaTransportControlsButton::Next: event = "remote-next"; capability = 8; break;
                case SystemMediaTransportControlsButton::Previous: event = "remote-previous"; capability = 9; break;
                case SystemMediaTransportControlsButton::FastForward:
                    event = "remote-jump-forward"; capability = 10; payload["interval"] = metadata.forwardInterval; break;
                case SystemMediaTransportControlsButton::Rewind:
                    event = "remote-jump-backward"; capability = 11; payload["interval"] = metadata.backwardInterval; break;
                default: return;
            }
            if (metadata.capabilities.count(capability)) manager.SendEvent(event, payload);
        });
    });
    seekEvent = controls.PlaybackPositionChangeRequested(winrt::auto_revoke, [weak](const auto&, const auto& args) {
        const auto position = args.RequestedPlaybackPosition().count() / 10000000.0;
        MediaManager::Dispatch(weak, std::nullopt, [position](auto& manager) {
            if (manager.metadata->capabilities.count(6))
                manager.SendEvent("remote-seek", JSValueObject{{"position", position}});
        });
    });
    UpdateOptions(JSValueObject{});
}

Metadata::~Metadata() {
    buttonEvent.revoke();
    seekEvent.revoke();
    try { if (controls) controls.IsEnabled(false); } catch (...) {}
}

void Metadata::UpdateOptions(const JSValue& options) {
    auto readInterval = [&](const char* name, double& destination) {
        if (options[name].Type() == JSValueType::Null) return;
        const auto value = options[name].AsDouble();
        if (!std::isfinite(value) || value <= 0) throw std::invalid_argument("Jump interval must be positive");
        destination = value;
    };
    readInterval("forwardJumpInterval", forwardInterval);
    readInterval("backwardJumpInterval", backwardInterval);
    if (options["capabilities"].Type() != JSValueType::Null) {
        if (options["capabilities"].Type() != JSValueType::Array)
            throw std::invalid_argument("Capabilities must be an array");
        std::set<int> requested;
        for (const auto& value : options["capabilities"].AsArray()) {
            const auto number = value.AsDouble();
            if (std::isfinite(number) && number >= 1 && number <= 12 && std::floor(number) == number)
                requested.insert(static_cast<int>(number));
        }
        capabilities = std::move(requested);
    }
    controls.IsPlayEnabled(capabilities.count(1) != 0);
    controls.IsPauseEnabled(capabilities.count(4) != 0);
    controls.IsStopEnabled(capabilities.count(5) != 0);
    controls.IsNextEnabled(capabilities.count(8) != 0);
    controls.IsPreviousEnabled(capabilities.count(9) != 0);
    controls.IsFastForwardEnabled(capabilities.count(10) != 0);
    controls.IsRewindEnabled(capabilities.count(11) != 0);
}

void Metadata::Set(const JSValue& value, bool merge) {
    if (merge) {
        for (const auto& item : value.AsObject()) data[item.first] = item.second.Copy();
    } else data = value.AsObject().Copy();
    const auto display = controls.DisplayUpdater();
    display.ClearAll();
    display.Type(MediaPlaybackType::Music);
    display.MusicProperties().Title(winrt::to_hstring(data["title"].AsString()));
    display.MusicProperties().Artist(winrt::to_hstring(data["artist"].AsString()));
    display.MusicProperties().AlbumTitle(winrt::to_hstring(data["album"].AsString()));
    const auto artwork = Track::Resource(data["artwork"]);
    if (!artwork.empty()) {
        try {
            display.Thumbnail(RandomAccessStreamReference::CreateFromUri(Windows::Foundation::Uri(winrt::to_hstring(artwork))));
        } catch (const winrt::hresult_error&) {
            // Invalid artwork must not interrupt otherwise playable audio.
        }
    }
    display.Update();
}

void Metadata::SetState(const std::string& state) {
    controls.IsEnabled(state != "none");
    const auto status = state == "playing" ? MediaPlaybackStatus::Playing
        : state == "paused" || state == "ready" ? MediaPlaybackStatus::Paused
        : state == "loading" || state == "buffering" ? MediaPlaybackStatus::Changing
        : state == "none" ? MediaPlaybackStatus::Closed : MediaPlaybackStatus::Stopped;
    controls.PlaybackStatus(status);
}

void Metadata::UpdateTimeline(double position, double duration) {
    if (!std::isfinite(duration) || duration <= 0) return;
    SystemMediaTransportControlsTimelineProperties timeline;
    timeline.StartTime(LocalPlayback::Time(0));
    timeline.MinSeekTime(LocalPlayback::Time(0));
    timeline.EndTime(LocalPlayback::Time(duration));
    timeline.MaxSeekTime(LocalPlayback::Time(duration));
    timeline.Position(LocalPlayback::Time(std::clamp(position, 0.0, duration)));
    controls.UpdateTimelineProperties(timeline);
}

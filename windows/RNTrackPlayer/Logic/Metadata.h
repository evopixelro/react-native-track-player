// Modified by EvoPixel for current capabilities, metadata updates and Windows media keys.
#pragma once

#include "NativeModules.h"
#include <winrt/Windows.Media.h>
#include <memory>
#include <set>

namespace winrt::RNTrackPlayer {
    struct MediaManager;

    struct Metadata {
        Windows::Media::SystemMediaTransportControls controls{nullptr};
        Microsoft::ReactNative::JSValueObject data;
        std::set<int> capabilities{1, 4, 5, 6, 8, 9};
        double forwardInterval = 15;
        double backwardInterval = 15;
        Windows::Media::SystemMediaTransportControls::ButtonPressed_revoker buttonEvent;
        Windows::Media::SystemMediaTransportControls::PlaybackPositionChangeRequested_revoker seekEvent;

        Metadata(Windows::Media::SystemMediaTransportControls controls, const std::shared_ptr<MediaManager>& owner);
        ~Metadata();
        void UpdateOptions(const Microsoft::ReactNative::JSValue& options);
        void Set(const Microsoft::ReactNative::JSValue& value, bool merge = false);
        void SetState(const std::string& state);
        void UpdateTimeline(double position, double duration);
    };
}

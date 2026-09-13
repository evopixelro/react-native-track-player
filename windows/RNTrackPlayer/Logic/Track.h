// Modified by EvoPixel to retain custom track fields and resolved asset URIs.
#pragma once

#include "NativeModules.h"

namespace winrt::RNTrackPlayer {
    struct Track {
        Microsoft::ReactNative::JSValueObject data;

        explicit Track(const Microsoft::ReactNative::JSValue& value);
        Track(const Track& other) : data(other.data.Copy()) {}
        Track& operator=(const Track& other) {
            if (this != &other) data = other.data.Copy();
            return *this;
        }
        Track(Track&&) noexcept = default;
        Track& operator=(Track&&) noexcept = default;

        void SetMetadata(const Microsoft::ReactNative::JSValue& metadata);
        std::string Url() const;
        static std::string Resource(const Microsoft::ReactNative::JSValue& value);
        Microsoft::ReactNative::JSValue ToObject() const { return data.Copy(); }
    };
}

#include "pch.h"
#include "Track.h"
#include <stdexcept>

using namespace winrt::Microsoft::ReactNative;
using namespace winrt::RNTrackPlayer;

Track::Track(const JSValue& value) {
    if (value.Type() != JSValueType::Object) throw std::invalid_argument("A track must be an object");
    data = value.AsObject().Copy();
    if (Url().empty()) throw std::invalid_argument("A track must have a URL");
    Windows::Foundation::Uri uri(winrt::to_hstring(Url()));
    if (!data["headers"].AsObject().empty())
        throw std::invalid_argument("Custom HTTP headers are not supported by the Windows media source");
}

std::string Track::Resource(const JSValue& value) {
    if (value.Type() == JSValueType::String) return value.AsString();
    if (value.Type() == JSValueType::Object) return value["uri"].AsString();
    return {};
}

std::string Track::Url() const { return Resource(data["url"]); }

void Track::SetMetadata(const JSValue& metadata) {
    if (metadata.Type() != JSValueType::Object) throw std::invalid_argument("Metadata must be an object");
    for (const auto& item : metadata.AsObject()) {
        // These updates describe a track, never replace its audio source or credentials.
        if (item.first != "url" && item.first != "type" && item.first != "headers")
            data[item.first] = item.second.Copy();
    }
}

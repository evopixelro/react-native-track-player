---
sidebar_position: 7
---

# macOS

The macOS module uses AVPlayer, AppKit and MediaPlayer with the same Track
Player TurboModule API. It shares the Objective-C++ bridge with iOS but does
not depend on UIKit, AVAudioSession or SwiftAudioEx.

## Installation

Use macOS 14 or later, Xcode and React Native macOS 0.81.9 with its matching
React Native 0.81.6 and React 19.1.4 dependencies. Enable the New Architecture
in the application Podfile before loading React Native's CocoaPods helpers:

```ruby
ENV['RCT_NEW_ARCH_ENABLED'] = '1'
```

Install `@evopixel/react-native-track-player` and run `bundle exec pod install`
in the application's macOS native project. Keep the scoped package as the
only Track Player dependency; its native module and pod names remain the
same as upstream.

Register a playback service and subscribe to remote events as on iOS. Remote
commands emit events; the service calls `play`, `pause`, `skipToNext` or the
other playback methods. This avoids executing each button twice.

## Playback

- Local files, bundled assets, HTTP/HTTPS sources and HLS use AVPlayer.
- Queue operations, Next/Previous, seeking, volume, rate and repeat modes
  use the same JavaScript API. Queue changes retain the native player and
  its media-command registrations.
- Volume accepts values from 0 to 1; playback rate accepts 0.1 to 4.
- Track title, artist, album and artwork appear in Now Playing. Updates via
  `updateMetadataForTrack` and `updateNowPlayingMetadata` also work while
  paused. `autoUpdateMetadata: false` leaves metadata updates to the app.
- Playback can continue while the application is in the background. The
  application must remain running; quitting it stops playback.
- System commands depend on the configured capabilities and the controls
  macOS chooses to display. Enable both Play and Pause for headset toggling.

Codecs and network access remain subject to AVPlayer, App Transport Security
and the application's sandbox entitlements. A sandboxed application needs
the outgoing network entitlement for remote streams. Opening arbitrary files
outside its container requires the corresponding user-granted access.

## Limitations

DASH, SmoothStreaming, DRM configuration, persistent media caching, Android
audio-focus options, iOS audio-session categories, rating/search commands and
embedded/timed/chapter metadata extraction are not implemented on macOS.
Supply display metadata through the Track Player metadata APIs. Native
macOS does not use the Web/Shaka implementation as a fallback.

## Example and Tests

The [example guide](https://github.com/evopixelro/react-native-track-player/blob/main/example/README.md) includes a native macOS host using
the existing player UI and playback service.

```sh
npm run test:macos
npm --prefix example run bundle:macos
npm --prefix example run build:macos
```

`test:macos` compiles the actual Swift queue and AVPlayer implementation and
checks queue mutations, input validation, remote-command capabilities,
metadata updates and command results. A generated local PCM file also checks
playback progress, autoplay, paused seeking, queue completion, repeat modes
and recovery after Stop. Only React's promise typedefs are
substituted in this standalone test; the separate example build compiles
the real React Native bridge and generated TurboModule code.

These checks do not verify audible playback, Control Center interactions,
Bluetooth devices, sleep/wake behavior or sandboxed distribution on a real
Mac. Those runtime checks are required before relying on macOS in production.

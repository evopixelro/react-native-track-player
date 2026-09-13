---
sidebar_position: 5
---

# Platform Support

These tables describe `@evopixel/react-native-track-player@5.0.4`.
✅ means implemented, ❌ means not implemented, and ◐ means partial or
platform-dependent support. An implemented feature is not a guarantee that
every codec, device or system control has been tested.

Android and iOS builds are checked with React Native 0.86.3 and 0.87.1.
[Windows](./windows.md) uses React Native Windows 0.84, and
[macOS](./macos.md) uses React Native macOS 0.81.9 with its matching framework.
Desktop build checks do not replace interactive playback and media-control
testing on the target operating system.

## Audio Sources

| Feature | Android | iOS | Web | Windows | macOS |
| ------- | :-----: | :-: | :-: | :-----: | :---: |
| App bundle¹ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Network | ✅ | ✅ | ✅ | ✅ | ✅ |
| File System² | ✅ | ✅ | ❌ | ✅ | ✅ |

¹: Use `require` or `import`. The application must bundle the asset for its target.

²: Use a `file:///` URI accessible to the application. Filesystem permissions,
sandbox entitlements and browser restrictions still apply.

## Stream Types

| Feature | Android | iOS | Web | Windows | macOS |
| ------- | :-----: | :-: | :-: | :-----: | :---: |
| Regular Streams | ✅ | ✅ | ✅ | ✅ | ✅ |
| DASH | ✅ | ❌ | ✅ | ◐ | ❌ |
| HLS | ✅ | ✅ | ✅ | ◐ | ✅ |
| SmoothStreaming | ✅ | ❌ | ❌ | ◐ | ❌ |

Codec, manifest and server compatibility depend on the playback backend.
Web uses Shaka Player and is subject to browser autoplay and CORS policies.
Windows delegates adaptive streams to
[MediaSource](https://learn.microsoft.com/en-us/windows/uwp/audio-video-camera/adaptive-streaming);
available formats depend on Windows and its installed codecs. These adaptive
stream combinations have not all been verified in the Windows example.
Windows and macOS do not expose custom DRM configuration. Neither Apple
implementation converts DASH into HLS or falls back to a WebView.

## Casting

| Feature | Android | iOS | Web | Windows | macOS |
| ------- | :-----: | :-: | :-: | :-----: | :---: |
| Google Cast | ❌ | ❌ | ❌ | ❌ | ❌ |
| Miracast/DLNA control | ❌ | ❌ | ❌ | ❌ | ❌ |
| AirPlay device selection | ❌ | ❌ | ❌ | ❌ | ❌ |

This package does not include a casting or route-picker API. Operating-system
audio routing is separate from managing cast devices in the application.
The original external casting add-on is not bundled or validated with this fork.

## Miscellaneous

| Feature | Android | iOS | Web | Windows | macOS |
| ------- | :-----: | :-: | :-: | :-----: | :---: |
| Media Controls | ✅ | ✅ | ❌ | ✅ | ✅ |
| Media Caching | ✅ | ❌ | ❌ | ❌ | ❌ |
| Background Mode¹ | ✅ | ✅ | ◐ | ◐ | ◐ |

¹: Read [Background Mode](./background-mode.md). Desktop playback requires the
application process to remain running. Browser playback depends on tab lifecycle
and is not a native background service. The OS chooses which configured media
controls to display; capabilities alone do not install playback event handlers.

## Functions

| Function | Android | iOS | Web | Windows | macOS |
| ------- | :-----: | :-: | :-: | :-----: | :---: |
| `setupPlayer` | ✅ | ✅ | ◐ | ◐ | ◐ |
| `updateOptions` | ✅ | ◐ | ◐ | ◐ | ◐ |
| `registerPlaybackService` | ✅ | ✅ | ◐ | ◐ | ◐ |
| `addEventListener` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `play` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `pause` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `stop` | ✅ | ✅ | ◐ | ✅ | ✅ |
| `retry` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `reset` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `setVolume` / `getVolume` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `setRate` / `getRate` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `seekTo` / `seekBy` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `getProgress` | ✅ | ✅ | ◐ | ✅ | ✅ |
| `getPlaybackState` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `setPlayWhenReady` / `getPlayWhenReady` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `getQueue` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `getActiveTrackIndex` / `getActiveTrack` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `getTrack` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `add` / `remove` | ✅ | ✅ | ◐ | ✅ | ✅ |
| `load` / `setQueue` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `move` | ✅ | ✅ | ◐ | ✅ | ✅ |
| `skip` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `skipToPrevious` / `skipToNext` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `removeUpcomingTracks` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `setRepeatMode` / `getRepeatMode` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `updateMetadataForTrack` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `updateNowPlayingMetadata` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `acquireWakeLock` / `abandonWakeLock` | ✅ | ❌ | ❌ | ❌ | ❌ |
| `validateOnStartCommandIntent` | ✅ | ❌ | ❌ | ❌ | ❌ |

- Setup and update options are platform-specific; see
  [PlayerOptions](../api/objects/player-options.md) and
  [UpdateOptions](../api/objects/update-options.md).
- On Web, `updateOptions` configures progress events, not system media controls.
  `getProgress().buffered` currently returns zero, and `move` cannot move
  the active track or move another track into its index.
- Web queue edits do not consistently preserve the active index when inserting
  or removing tracks before it. Avoid those edits during playback; rebuild the
  queue and explicitly select the desired track and position instead. Removing
  the active track also requires an explicit selection of its replacement.
- Web `stop()` clears the active index without resetting `playWhenReady`.
  See [Player](../api/functions/player.md#stop) before resuming a stopped queue.
- On Web and desktop, `registerPlaybackService` starts the JavaScript event
  handlers; it does not create an Android-style headless service.
- On non-Android platforms the wake-lock helpers do nothing, and
  `validateOnStartCommandIntent` returns `true` without checking an Android intent.
- Seeking and playback rate depend on the source. Live streams may not be
  seekable; macOS accepts rates from 0.1 to 4.

## Events

| Event | Android | iOS | Web | Windows | macOS |
| ------- | :-----: | :-: | :-: | :-----: | :---: |
| `remote-play` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `remote-play-id` / `remote-play-search` | ✅ | ❌ | ❌ | ❌ | ❌ |
| `remote-play-pause` | ✅ | ❌ | ❌ | ❌ | ✅ |
| `remote-pause` / `remote-stop` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `remote-skip` | ✅ | ❌ | ❌ | ❌ | ❌ |
| `remote-next` / `remote-previous` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `remote-seek` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `remote-set-rating` | ✅ | ❌ | ❌ | ❌ | ❌ |
| `remote-jump-forward` / `remote-jump-backward` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `remote-duck` | ✅ | ✅ | ❌ | ❌ | ❌ |
| `remote-like` / `remote-dislike` / `remote-bookmark` | ❌ | ◐ | ❌ | ❌ | ✅ |
| `playback-state` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `playback-active-track-changed` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `playback-queue-ended` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `playback-play-when-ready-changed` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `playback-progress-updated` | ✅ | ✅ | ✅ | ✅ | ✅ |
| `playback-error` | ✅ | ✅ | ❌ | ✅ | ✅ |
| `metadata-common-received` / `metadata-timed-received` | ✅ | ✅ | ❌ | ❌ | ❌ |
| `metadata-chapter-received` | ❌ | ✅ | ❌ | ❌ | ❌ |

Use the exported [Event](../api/events.md) names instead of hard-coded strings.
On Web, playback failures are reported through `PlaybackState` and its
`error` property, not a separate `PlaybackError` event. Stream metadata events
require metadata provided by the source; supplying a title manually is separate.

The iOS feedback event types remain exported, but its current capability mapping
does not enable Like, Dislike or Bookmark commands. Do not rely on those buttons
in 5.0.4. macOS enables them through the corresponding feedback options.

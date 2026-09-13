---
sidebar_position: 9
---

# Migrating from v4 to v5

This guide covers migration from `react-native-track-player@4.1.2` to
`@evopixel/react-native-track-player@5.0.4`. It describes this fork,
not a migration to a commercial release of the original package.

## Installation

Remove the original Track Player package:

```sh
npm uninstall react-native-track-player
```

Install this fork:

```sh
npm install @evopixel/react-native-track-player@5.0.4
```

Update imports in application code, playback services, test mocks and any
explicit native dependency paths:

```ts
import TrackPlayer, { Capability, Event, State } from '@evopixel/react-native-track-player';
```

Do not install two Track Player packages together. The native module and
CocoaPods names are shared. Commit the updated lockfile and rebuild the native
application; reloading JavaScript alone does not replace a native module.

## Native Setup

The fork requires React Native's New Architecture. Android and iOS builds are
checked with React Native 0.86.3 and 0.87.1. Use the native project template
and build tools for the React Native version in your application.

React Native 0.86.3 applications can use Node.js 20.19.4+. React Native 0.87.1
requires Node.js 22.13+ or 24.3+; the library's broader Node.js range does not
override React Native's own requirement. An upgrade from an older React Native
application is a separate native project migration, not just a package rename.

For iOS, reinstall CocoaPods dependencies after changing the package:

```sh
cd ios
bundle exec pod install
```

Keep the application's background audio configuration and playback service.
Use a native development or release build with Expo; Expo Go cannot load this
module. See [Installation](./basics/installation.mdx) for platform setup.

Windows and macOS use their own matching React Native versions. Follow the
[Windows](./basics/windows.md) and [macOS](./basics/macos.md) guides rather than
upgrading those targets to the Android/iOS framework version.

## Changes from 4.1.2

### Player Method Updates

Deprecated 4.x aliases are no longer exported:

| Previous API | Replacement |
| ------------ | ----------- |
| `getState()` | `(await getPlaybackState()).state` |
| `getDuration()` | `(await getProgress()).duration` |
| `getPosition()` | `(await getProgress()).position` |
| `getBufferedPosition()` | `(await getProgress()).buffered` |
| `getCurrentTrack()` | `getActiveTrackIndex()` |

The active track or its index can be `undefined` when the queue has no active
item. Compare the `state` field of `getPlaybackState()`, not the whole object:

```ts
const playback = await TrackPlayer.getPlaybackState();
const track = await TrackPlayer.getActiveTrack();
const { position, duration } = await TrackPlayer.getProgress();

if (playback.state === State.Playing && track) {
    console.log(track.title, position, duration);
}
```

`setQueue(tracks)` returns `Promise<void>`; use `getActiveTrackIndex()`
when you need the active index. Queue track resources are resolved by the
JavaScript wrapper before they reach the native module.

### Event Updates

| Previous API | Replacement |
| ------------ | ----------- |
| `Event.PlaybackTrackChanged` | `Event.PlaybackActiveTrackChanged` |
| `Event.PlaybackMetadataReceived` | `Event.MetadataCommonReceived`, `Event.MetadataTimedReceived` or `Event.MetadataChapterReceived`, according to the metadata type and platform |

The active-track event includes `index`, `track`, `lastIndex`,
`lastTrack` and `lastPosition`. Handle an absent active track when the queue
is cleared. Metadata payloads and platform availability are described in
[Events](./api/events.md#metadata).

### Options and Constants

Use the exported `Capability`, `PitchAlgorithm`, `State` and
`RepeatMode` values. Do not copy numeric values from another platform or
an older native bridge.

`waitForBuffer` is no longer part of `PlayerOptions`. Use only the fields
listed in [PlayerOptions](./api/objects/player-options.md) and
[UpdateOptions](./api/objects/update-options.md). Legacy custom notification
icon and compact-capability options are not in the current `UpdateOptions`
interface. Like, Dislike and Bookmark are not exported `Capability` members;
see the platform-specific feedback limitations in
[Platform Support](./basics/platform-support.md).

### Playback Service

Register the service once and keep remote handlers there. Play, Pause,
Stop, Next, Previous and Seek events must call the corresponding player
methods in your application. Do not reset or initialize the player again
just to skip a track.

The hook `usePlaybackState()` initially returns `{ state: undefined }`.
Wait for setup and handle this initial state in your UI.

## Verification

After rebuilding, check queue loading, Play/Pause/Stop, Next/Previous,
seeking, track changes, metadata and the controls your application exposes.
Repeat the relevant checks in foreground and background, including paused
playback and returning to the application.

If the application patches the original package, review those changes against
the fork before removing its patch. Keep patches and install hooks required
by unrelated dependencies. WebView adapters, navigation and application UI
are separate integrations and should retain their existing behavior.

The implementations do not have identical platform coverage. For example,
Apple playback uses HLS rather than DASH, Web metadata updates and native
media controls are not implemented, and desktop system controls still need
interactive validation. Review [Platform Support](./basics/platform-support.md)
before enabling a feature on a new target.

---
sidebar_position: 1
---

# Intro

A fully fledged audio module created for music apps. Provides audio playback, external media controls, background mode and more!

This documentation covers the EvoPixel fork, version 5.0.4. To replace the
original 4.1.2 package, follow the [v5 migration guide](./v5-migration.md).

## Features

* **Lightweight** - Optimized to use the least amount of resources according to your needs
* **Feels native** - As everything is built together, it follows the same design principles as real music apps do
* **Multi-platform** - Android, iOS, Web, Windows and macOS implementations; see [Platform Support](./basics/platform-support.md) for feature and validation limits
* **Media Controls support** - Provides events for controlling the app from a bluetooth device, the lockscreen, a notification, a smartwatch or even a car
* **Local or network, files or streams** - It doesn't matter where the media belongs, we've got you covered
* **Adaptive bitrate streaming support** - DASH, HLS or SmoothStreaming, depending on the platform
* **Caching support** - Android media caching; persistent downloads remain the application's responsibility
* **Background support** - Keep playing audio even after the app is in background
* **Configurable controls** - Choose the supported media commands and progress update interval
* **Supports React Hooks 🎣** - Includes React Hooks for common use-cases so you don't have to write them

## Example

If you want to get started with this module, check the [Installation](./basics/installation.mdx) & [Getting Started](./basics/getting-started.md) page.
If you want detailed information about the API, check the [API Reference](./api/functions/lifecycle.md).
You can also look at our example project [here](https://github.com/evopixelro/react-native-track-player/tree/main/example).

```javascript
import TrackPlayer, { RepeatMode } from '@evopixel/react-native-track-player';

// Creates the player
const setup = async () => {
  await TrackPlayer.setupPlayer({});

  await TrackPlayer.add({
    url: require('track.mp3'),
    title: 'Track Title',
    artist: 'Track Artist',
    artwork: require('track.png')
  });

  TrackPlayer.setRepeatMode(RepeatMode.Queue);
};
```

# React Native Track Player

This is a fork of [react-native-track-player](https://github.com/doublesymmetry/react-native-track-player) module

----

A fully-fledged audio module created for music apps. Provides audio playback, external media controls, background mode and more!

## Overview

- [Documentation](https://rntp.dev/)
  * [Installation](#installation)
  * [Getting Started](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/getting-started.md)
  * [API Docs](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/api/events.md)
  * [Platform Support](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/platform-support.md)
  * [Background Mode](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/background-mode.md)
  * [Playback Service and Remote Controls](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/playback-service.md)
  * [Player Options](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/api/objects/update-options.md)
  * [Expo and EAS Build](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/guides/with-expo.md)
  * [Saving Progress](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/guides/saving-progress.md)
  * [Troubleshooting](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/troubleshooting.md)
  * [v2 Migration Guide](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/v2-migration.md)
  * [v5 Migration Guide](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/v5-migration.md)
- [Features](#features)
- [Why another music module?](#why-another-music-module)
- [Example Setup](#example-setup)
- [Core Team ✨](#core-team-)
- [Special Thanks ✨](#special-thanks-)
- [I Have A Bug/Feature Request](#contributing)
- [Release](#release)
- [Community](#community)
- [License](#license)

Not sure where to start?

1. Install this fork, then try [Getting Started](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/getting-started.md).
2. Peruse the [API Docs](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/api/events.md).
3. Browse the [Example Project](https://github.com/evopixelro/react-native-track-player/tree/main/example) for integration examples; it uses the local EvoPixel fork with React Native 0.86.3 on Android/iOS, React Native Windows 0.84, React Native macOS 0.81 and React Native Web.

## Installation

```sh
npm install @evopixel/react-native-track-player
```

Import the package as `@evopixel/react-native-track-player`. Commit your application's lockfile and use `npm ci` for repeatable installations.

This fork requires React Native's New Architecture. Library development uses React Native 0.87.1 and Node.js 22.13+ or 24.3+. The Android/iOS example remains on 0.86.3, with separate 0.87.1 compatibility builds in CI. Applications using 0.86.3 can continue using Node.js 20.19.4+; React Native 0.87 requires a newer Node.js version.

React Native 0.87 Android builds use SDK/build tools 37, Kotlin 2.2 and Gradle 9.4.1. Follow its official template, including `android.builtInKotlin=false` and `android.newDsl=false` for AGP 9. iOS integration uses CocoaPods; the experimental Swift Package Manager integration is not included.

Desktop targets use their matching React Native releases, not the Android/iOS version. See [Windows support](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/windows.md) and [macOS support](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/macos.md) for setup, supported features and validation limits.

For iOS, install the application's CocoaPods dependencies:

```sh
cd ios
bundle exec pod install
```

Follow the [iOS and web setup guide](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/installation.mdx) and [background mode guide](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/background-mode.md). Do not install another Track Player package alongside this fork: the native module names are shared.

The current documentation covers this fork's 5.0.4 API; older versioned documentation is retained as a historical reference. When replacing the original 4.1.2 package, follow the [v5 migration guide](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/v5-migration.md) for imports, removed APIs and native rebuild steps.

## Commercial Usage

This fork is distributed under the [Apache License 2.0](LICENSE), including for commercial use. Preserve the license and attribution notices when redistributing it.

You can support the original project through [GitHub Sponsors](https://github.com/sponsors/doublesymmetry). The EvoPixel fork is maintained independently of the original authors.

## Features

* **Lightweight** - Optimized to use the least amount of resources according to your needs
* **Feels native** - As everything is built together, it follows the same design principles as real music apps do
* **Multi-platform** - Android, iOS, Web, Windows and macOS implementations; see [Platform Support](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/platform-support.md) for feature and validation limits
* **Media Controls support** - Provides events for controlling the app from a Bluetooth device, the lock screen, a notification, a smartwatch or even a car
* **Local or network, files or streams** - It doesn't matter where the media belongs, we've got you covered
* **Adaptive bitrate streaming support** - Support for DASH, HLS or SmoothStreaming, depending on the platform
* **Caching support** - Android media caching; persistent downloads remain the application's responsibility
* **Background support** - Keep playing audio even after the app is in background
* **Configurable controls** - Choose the supported media commands and progress update interval
* **Supports React Hooks 🎣** - Includes React Hooks for common use-cases so you don't have to write them

## Why another music module?

Music applications need playback, queue management, system media controls and background behavior to work together. Track Player brings those pieces into one module, with an Android playback service and native playback backends on Apple and Windows platforms.

The application still owns its user interface, remote-command handlers and persistent storage. Casting is a separate integration; this package does not provide a cast-device picker or a Google Cast API. See [Platform Support](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/platform-support.md) before relying on a feature across platforms.

## Example Setup

First please take a look at the [Getting Started](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/getting-started.md) guide, but a basic example of how to play a track:

```javascript
import TrackPlayer from '@evopixel/react-native-track-player';

const start = async () => {
    // Set up the player
    await TrackPlayer.setupPlayer();

    // Add a track to the queue
    await TrackPlayer.add({
        id: 'trackId',
        url: require('./track.mp3'),
        title: 'Track Title',
        artist: 'Track Artist',
        artwork: require('./track.png')
    });

    // Start playing it
    await TrackPlayer.play();
};
start();
```

For a complete integration, register a [Playback Service](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/basics/playback-service.md) once and handle the remote events for each enabled media control. Configure capabilities with [UpdateOptions](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/api/objects/update-options.md); showing a button does not implement its action.

The guides also cover [offline audio](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/guides/offline-playback.md), [per-track progress](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/guides/multitrack-progress.md) and [sleep timers](https://github.com/evopixelro/react-native-track-player/blob/main/docs/docs/guides/sleeptimers.md).

## Core Team ✨

Original project and fork contributors.

<table>
  <tr>
    <td align="center"><a href="https://github.com/dcvz"><img src="https://avatars.githubusercontent.com/u/2475932?v=4" width="100px;" alt=""/><br /><sub><b>David Chavez</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/evopixelro"><img src="https://avatars.githubusercontent.com/u/131179162?v=4" width="100px;" alt=""/><br /><sub><b>EvoPixel</b></sub></a><br /><sub>Fork</sub></td>
  </tr>
</table>

## External Core Contributors ✨

<table>
  <tr>
    <td align="center"><a href="https://github.com/jspizziri"><img src="https://avatars.githubusercontent.com/u/1452066?v=4" width="100px;" alt=""/><br /><sub><b>Jacob Spizziri</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/puckey"><img src="https://avatars.githubusercontent.com/u/271885?v=4" width="100px;" alt=""/><br /><sub><b>Jonathan Puckey</b></sub></a><br /></td>
  </tr>
</table>

## Special Thanks ✨

<table>
  <tr>
    <td align="center"><a href="https://github.com/Guichaguri"><img src="https://avatars.githubusercontent.com/u/1813032?v=4" width="100px;" alt=""/><br /><sub><b>Guilherme Chaguri</b></sub></a><br /></td>
    <td align="center"><a href="https://github.com/curiousdustin"><img src="https://avatars.githubusercontent.com/u/1706540?v=4" width="100px;" alt=""/><br /><sub><b>Dustin Bahr</b></sub></a><br /></td>
  </tr>
</table>

## Contributing

The guidelines below are retained from the original project. For issues or pull requests specific to this fork, use the [EvoPixel repository](https://github.com/evopixelro/react-native-track-player/issues) and run `npm run check` before submitting changes.

You want this package to be awesome and we want to deliver on that. As you know
already you can just [File A Ticket](#file-a-ticket), but thats not actually the
best way for you to get what you need (read on to see why). The best way is for
you to [Be A Champion](#be-a-champion) and [dive into the code](#where-do-you-start).

#### File A Ticket

The reality is that filing a ticket isn't always enough. **This is probably only
going to work if your issue aligns with both the interests _and_ the resources available** to the core team. Here are the things that align with our _interests_
in order of priority.

1. Fixing **_widespread, common, and critical Bugs_**.
2. Fixing **_uncommon but necessary Bugs_**.
3. Introducing new  **_Features that have broad value_**.

Now keep in mind available resources. Long story short, the thing you care about
needs to be cared about by either a lot of other people, or by us.

**BUT!** There's another and, arguably even **_better way_** that helps you get what
you need faster: [Be A Champion](#be-a-champion).

#### Be A Champion

Being a _champion_ makes it easy for us to help you. Which is what we all want!
So how can you be a champion? [Sponsor the Project](https://github.com/sponsors/DoubleSymmetry) or _be willing to write some code_.

**If _you're willing_** to write some code **_we're willing_** to:

- Open a design discussion, give feedback, and approve something that works.
- Provide guidance in the implementation journey.

So, in a nutshell, let us know you're willing to do the work and ask for a little
guidance, and watch the things you care about get done faster than anyone else.
The best help will be given to those who are willing to help themselves.

###### You don't have experience you say? It's OK!

You may be thinking that you can't help because you know nothing about native
iOS or Android or maybe even React code. But we're willing to help guide you.

If you're up for that task then we can help you understand native code and how
React Native works.

The only way you go from _not-knowing_ to _knowing_ is by learning. Learning isn't
something you should be ashamed of nor is it something you should be scared of.

#### Where Do You Start?

Our goal is to make it as easy as possible for you to make changes to the library.
All the documentation on how to work on the library and it's dependencies is
[located in this Guide](https://github.com/evopixelro/react-native-track-player/blob/main/example/README.md). For this fork, use the validation and build commands below; the example guide covers Android, iOS, Windows, macOS and Web.

## Release

Validate and build the library before preparing a package:

```sh
yarn install --frozen-lockfile
npm run check
npm pack
```

Development uses Yarn Classic 1.22.22 and `yarn.lock`, following the original v4 project layout.

The original `.husky/prepare-commit-msg` workflow is available for contributors.
Run `npm run hooks:install` once to enable it, or `npm run commit` to start the
commit-message prompt explicitly. Hooks are not installed by package builds;
non-interactive commits and supplied messages skip the prompt.

`npm run check` runs TypeScript checks, ESLint, regression tests and the library build. `npm pack` validates and builds the package, then creates a local archive; it does not publish to npm or push to GitHub. The original automatic release hooks are not used by this fork.

Native build and device testing are still required when changing platform code; the automated tests do not exercise playback on a device.

Run all tests or select a test group with npm:

```sh
npm test
npm run test:player
npm run test:native
npm run test:codegen
```

## Community

You can find the upstream community on [React Native Track Player Discord](https://discordapp.com/invite/ya2XDCR):

- `#introduce-yourself` - Come greet the newest members of this group!
- `#troubleshooting-forum` - Ask members of the community to trouble shoot issues with your app and make recommendations.
- `#show-and-tell` - Tell the community about the app you made with this project!
- `#news-and-releases` - Stay updated about the latest releases and dev efforts on the project.

## License

[Apache License 2.0](LICENSE). Original copyright and attribution notices are preserved. Modifications copyright 2026 EvoPixel, licensed under Apache-2.0.

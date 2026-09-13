---
sidebar_position: 98
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

# Developing with Expo

Expo is a popular development platform in the react-native ecosystem.

Install `@evopixel/react-native-track-player` in a native project. Keep the Expo
SDK and React Native versions compatible; the library does not select an Expo
SDK or replace the application's native configuration.

## Development Build

A [development build](https://docs.expo.dev/develop/development-builds/introduction/)
or a release build is required; Expo Go cannot load this native module.
Install the package before building, and rebuild after changing native dependencies.
[EAS Build](https://docs.expo.dev/build/introduction/) can compile Android and iOS
applications with the same native setup. Cloud compilation does not replace
device testing of background playback or system controls.


## Custom Entry Point

Register the playback service once in the application's entry file, not inside
a screen or React component. For a non-Router Expo application, point `main`
in `package.json` to your entry file and register the root component there:

```ts
import { registerRootComponent } from 'expo';
import TrackPlayer from '@evopixel/react-native-track-player';
import App from './App';
import { PlaybackService } from './service';

registerRootComponent(App);
TrackPlayer.registerPlaybackService(() => PlaybackService);
```

`PlaybackService` contains the remote event handlers described in
[Playback Service](../basics/playback-service.md). For Expo Router, follow its
[custom entry-point instructions](https://docs.expo.dev/router/installation/)
and keep Router's root registration; do not also register a second root component.


## Web

Install the optional Shaka Player peer from [Web Setup](../basics/installation.mdx#web-setup).
The Web implementation loads it dynamically in the browser. If your Metro web
configuration needs the async import runtime, add the following before loading
the player. Whether this is already included depends on the Expo entry point.

### 1. Install `@expo/metro-runtime`

<Tabs groupId="pkg">
  <TabItem value="npm" label="NPM" default>

    npm install --save @expo/metro-runtime

  </TabItem>
  <TabItem value="yarn" label="Yarn">

    yarn add @expo/metro-runtime

  </TabItem>
</Tabs>

### 2. Import `@expo/metro-runtime` in your App root

```tsx
// App.tsx
import '@expo/metro-runtime';
...
```

## Streaming HTTP

Prefer HTTPS streams. Cleartext HTTP access depends on the application's
Android network-security configuration, which can differ between debug and
release builds. Do not assume a debug stream also works in a release build.

To allow HTTP, you must first install the [`expo-build-properties` package](https://docs.expo.dev/versions/latest/sdk/build-properties).

```
npx expo install expo-build-properties
```

Then add this configuration in `app.json`:

```json
...
    "plugins": [
      [
        "expo-build-properties",
        {
          "android": {
            "usesCleartextTraffic": true
          }
        }
      ]
    ]
...
```

## Background Audio

For generated iOS projects, include `audio` in `ios.infoPlist.UIBackgroundModes`
in the Expo app configuration. For checked-in native projects, enable the
background audio capability in the native target. Do not expect an app-config
change to modify an existing native project without applying it there.

See [Background Mode](../basics/background-mode.md) for iOS setup, Android
service behavior and lifecycle limits. Changing these settings requires a new
native build, not only an over-the-air JavaScript update.

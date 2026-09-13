---
sidebar_position: 6
---

# Windows

## Module Installation

This implementation targets React Native Windows 0.84 with the New Architecture.
Use the matching React Native version required by `react-native-windows`; the
Android/iOS example uses React Native 0.86.3 independently.

The native module and Windows example compile in CI with Visual Studio 2026.
Native playback and system media controls still need a runtime check before
this platform is used in a production app.

## Automatic install with autolinking

RNTrackPlayer supports autolinking. Install `@evopixel/react-native-track-player`
and rebuild the Windows app:

```sh
npm install @evopixel/react-native-track-player
npx react-native run-windows
```

Register a playback service just as on iOS. The service handles the remote
Play, Pause, Stop, Previous, Next, seek and jump events; closing the app ends
playback. Do not register both this fork and the original package.

## Manual installation

1. `npm install @evopixel/react-native-track-player`
2. Open your solution in Visual Studio 2026 (eg. `windows\yourapp.sln`)
3. Right-click Solution icon in Solution Explorer > Add > Existing Project...
4. Add `node_modules\@evopixel\react-native-track-player\windows\RNTrackPlayer\RNTrackPlayer.vcxproj`
5. Right-click main application project > Add > Reference...
6. Select `RNTrackPlayer` in Solution Projects
7. In app `pch.h` add `#include "winrt/RNTrackPlayer.h"`
8. In `App.cpp` add `PackageProviders().Append(winrt::RNTrackPlayer::ReactPackageProvider());` before `InitializeComponent();`

## Module development

If you want to contribute to this module Windows implementation, first you must install the [Windows Development Dependencies](https://aka.ms/rnw-deps).

Use Visual Studio 2026 with the C++ development tools and Windows SDK required
by React Native Windows 0.84. Open `windows/RNTrackPlayer.sln` after installing
the matching Windows framework in a Windows app. When building the library
separately, set the MSBuild `ReactNativeWindowsDir` property to that app's
`node_modules/react-native-windows` directory, including the trailing slash.
The CI workflow builds the host in `example/windows` and its native module;
Android and iOS dependencies do not need to be downgraded.

The generated TurboModule headers are included in the package. After changing
`src/NativeTrackPlayer.ts`, run `npm run codegen:windows` from the repository
root and rebuild the native project.

Run `npm run test:windows` to compile and execute the queue regression tests.
This requires a C++17 compiler: use a Visual Studio Developer Command Prompt
on Windows, or set `CXX` to another compatible compiler. These tests exercise
queue edits and index validation, not audio playback or system media controls.

## Playback notes

Windows uses `Windows.Media.Playback.MediaPlayer` and the system media transport
controls. The implementation includes queue navigation, repeat modes, seeking,
volume, playback rate, progress events, metadata and artwork updates. Supported
file and stream formats depend on the Windows media codecs. Custom HTTP headers,
DRM configuration, caching, casting and Android-specific options are not supported.
An unseekable live stream cannot provide seeking controls.

For the separate AVPlayer implementation, see [macOS](./macos.md).

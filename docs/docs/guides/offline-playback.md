---
sidebar_position: 1
---

# Offline Playback

There are two general use-cases for offline playback:

1. An "Offline Only" case where all the audio is bundled with your App itself.
2. A "Hybrid Offline/Network" case where some of the time you're playing from a
  network and sometime you're playing offline.

Both of these can be achieved on the native platforms. The main
difference between the two is in the 2nd you'll need another package to
download your audio while your App is running instead of loading into the App's
source at build time.

After that, you simply send a `Track` object to the player with a **local file
path** to your audio.

## Offline Only

Include the audio files in your app's assets and pass a bundled resource such
as `url: require('./track.mp3')` when adding a track. Ensure that your target's
bundler includes the file in the release build.

⚠️ Please take into consideration that this approach will increase
the size of your App based on how much audio you want the user to be able to
play. If you're doing anything substantial, it's recommended that you use
the [Hybrid Offline/Network](#hybrid-offline-network) approach.

## Hybrid Offline/Network

Use a downloader and filesystem library compatible with your application's
React Native version, architecture and target platform. Applications configured
with Expo Modules can use [Expo FileSystem](https://docs.expo.dev/versions/latest/sdk/filesystem/).
Track Player itself does not download or manage persistent offline files.

Download the audio completely before adding its accessible `file:///` URI to
the queue. Store files intended for offline use in persistent application
storage, not a cache directory that the operating system can clear. Handle
failed downloads, storage limits and deletion in your app.

Android's playback cache is separate from a managed download collection. Web
does not accept native filesystem paths; browser offline storage and asset
delivery require a separate web implementation.

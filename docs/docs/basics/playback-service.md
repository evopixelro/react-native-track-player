---
sidebar_position: 3
---

# Playback Service

The playback service registers handlers tied to the player rather than the React UI. Put remote controls and progress listeners here so they do not depend on a mounted screen.

Android runs these handlers in a headless task. Other platforms use the application's existing JavaScript runtime; registration does not create a separate process. Configure [Background Mode](./background-mode.md) for your platform. Neither registration nor background audio guarantees execution after the operating system terminates the app or service.

## Remote Events

[Remote events](../api/events.md#media-controls) are sent from places outside of our user interface that we can react to. For example if the user presses the pause media control in the IOS lockscreen / Android notification or from their Bluetooth headset, we want to have TrackPlayer pause the audio.

If you create a listener to a remote event like `Event.RemotePause` in a React component, the listener may disappear when that component unmounts. Register remote listeners once in the playback service instead. Available events and system controls still depend on the [platform](./platform-support.md).

## Example
```js
import TrackPlayer from '@evopixel/react-native-track-player';
import { PlaybackService } from './src/services';

// This needs to go right after you register the main component of your app
// AppRegistry.registerComponent(...)
TrackPlayer.registerPlaybackService(() => PlaybackService);
```

```ts
// src/services/PlaybackService.ts
import TrackPlayer, { Event } from '@evopixel/react-native-track-player';

export const PlaybackService = async function() {

    TrackPlayer.addEventListener(Event.RemotePlay, () => TrackPlayer.play());

    TrackPlayer.addEventListener(Event.RemotePause, () => TrackPlayer.pause());

    // ...

};
```

## Custom Media Controls Notification id & name

On Android, react-native-track-player creates notification channels for its playback service.
 - More information read here: https://developer.android.com/media/implement/surfaces/mobile

To customize it, put the example below inside your project folder.

## Example
```xml
<!-- YOUR_PROJECT_DIR/android/app/src/main/res/values/strings.xml -->
<resources>
    <!-- rtnp channel id -->
    <string name="rntp_temporary_channel_id">temporary_channel</string>
    <!-- rtnp channel name -->
    <string name="rntp_temporary_channel_name">temporary_channel</string>
    <!-- playback_channel_name used by KotlinAudio in rntp -->
    <string name="playback_channel_name">Music Player</string>
</resources>
```

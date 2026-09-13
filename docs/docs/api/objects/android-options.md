# AndroidOptions

Options available for the android player. All options are optional.

| Param | Type  | Default | Description |
|-------|-------|---------|-------------|
| `appKilledPlaybackBehavior` | [`AppKilledPlaybackBehavior`](../constants/app-killed-playback-behavior.md) | [`ContinuePlayback`](../constants/app-killed-playback-behavior.md#continueplayback-default) | Define how the audio playback should behave after removing the app from recents (killing it). |
| `alwaysPauseOnInterruption` | `boolean` | `false` | Whether the `remote-duck` event will be triggered on every interruption |
| `audioOffload` | `boolean` | Player default | Requests Media3 audio offload; availability depends on the device and source. |
| `androidSkipSilence` | `boolean` | `false` | Enables the player's silence-skipping processing. |
| `shuffle` | `boolean` | `false` | Enables shuffle for the native Android queue. This does not discover or control a playlist in a web page. |
| `stopForegroundGracePeriod` | `number` | `5` | Time in seconds to wait once the player should transition to not considering the service as in the foreground. If playback resumes within this grace period, the service remains in the foreground state. |

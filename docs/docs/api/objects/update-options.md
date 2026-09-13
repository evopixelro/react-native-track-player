# UpdateOptions

All parameters are optional. Supply the complete capabilities and progress
configuration you want to keep when updating options: not every native backend
merges omitted fields with the previous call.

| Param | Type | Description | Android | iOS | Web | Windows | macOS |
| ----- | ---- | ----------- | :-----: | :-: | :-: | :-----: | :---: |
| `ratingType` | [RatingType](../constants/rating.md) | The rating type | ✅ | ❌ | ❌ | ❌ | ❌ |
| `forwardJumpInterval` | `number` | Jump-forward interval in seconds | ✅ | ✅ | ❌ | ✅ | ✅ |
| `backwardJumpInterval` | `number` | Jump-backward interval in seconds | ✅ | ✅ | ❌ | ✅ | ✅ |
| `android` | [AndroidOptions](./android-options.md) | Android service and playback options | ✅ | ❌ | ❌ | ❌ | ❌ |
| `likeOptions` | [FeedbackOptions](./feedback.md) | Like command configuration | ❌ | ◐ | ❌ | ❌ | ✅ |
| `dislikeOptions` | [FeedbackOptions](./feedback.md) | Dislike command configuration | ❌ | ◐ | ❌ | ❌ | ✅ |
| `bookmarkOptions` | [FeedbackOptions](./feedback.md) | Bookmark command configuration | ❌ | ◐ | ❌ | ❌ | ✅ |
| `capabilities` | [Capability[]](../constants/capability.md) | Enabled media commands | ✅ | ✅ | ❌ | ✅ | ✅ |
| `notificationCapabilities` | [Capability[]](../constants/capability.md) | Android notification commands; defaults to `capabilities` | ✅ | ❌ | ❌ | ❌ | ❌ |
| `color` | `number` | Android notification color in ARGB format | ✅ | ❌ | ❌ | ❌ | ❌ |
| `progressUpdateEventInterval` | `number` | Interval in seconds for [PlaybackProgressUpdated](../events.md#playbackprogressupdated); disabled unless positive | ✅ | ✅ | ✅ | ✅ | ✅ |

Set both jump intervals when you want consistent values across platforms.
Windows and macOS support Play, Pause, Stop, Next, Previous, Seek and jump
commands; Android-only capabilities are not implemented there.

The iOS feedback types remain in the interface, but its 5.0.4 capability
mapping does not enable Like, Dislike or Bookmark. Do not rely on those commands
on iOS. macOS enables them through the feedback options.

Custom icon fields and `compactCapabilities` from older releases are not part
of this interface. See [Platform Support](../../basics/platform-support.md)
for the implementation limits.

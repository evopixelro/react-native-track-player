---
sidebar_position: 2
---

# Capability

All Capability types are made available through the named export `Capability`:

```ts
import { Capability } from '@evopixel/react-native-track-player';
```

| Name | Description |
|------|-------------|
| `Play`           | Capability indicating the ability to play |
| `PlayFromId`     | Capability indicating the ability to play from a track id (Required for Android Auto) |
| `PlayFromSearch` | Capability indicating the ability to play from a text/voice search (Required for Android Auto) |
| `Pause`          | Capability indicating the ability to pause |
| `Stop`           | Capability indicating the ability to stop (on iOS available only for tracks where `.isLiveStream` is `true`) |
| `SeekTo`         | Capability indicating the ability to seek to a position in the timeline |
| `Skip`           | Capability indicating the ability to skip to any song in the queue |
| `SkipToNext`     | Capability indicating the ability to skip to the next track |
| `SkipToPrevious` | Capability indicating the ability to skip to the previous track |
| `SetRating`      | Capability indicating the ability to set the rating value based on the rating type |
| `JumpForward`    | Capability indicating the ability to jump forward by the amount of seconds specified in the options |
| `JumpBackward`   | Capability indicating the ability to jump backward by the amount of seconds specified in the options |

Use these exported values rather than hard-coded native numbers. Availability
depends on the [platform](../../basics/platform-support.md). Like, Dislike and
Bookmark are not exported capability members in 5.0.4; macOS configures its
feedback commands through the corresponding `UpdateOptions` fields.

---
sidebar_position: 1
---

# State

All State types are made available through the named export `State`:

```ts
import { State } from '@evopixel/react-native-track-player';
```

| Name | Description |
|------|-------------|
| `None`        | State indicating that no media is currently loaded |
| `Ready`       | State indicates that the player is paused, but ready to start playing |
| `Playing`     | State indicating that the player is currently playing |
| `Paused`      | State indicating that the player is currently paused |
| `Stopped`     | State indicating that the player is currently stopped |
| `Ended`       | State indicates playback stopped due to the end of the queue being reached |
| `Buffering`   | State indicating that the player is currently buffering (no matter whether playback is paused or not) |
| `Loading`     | State indicating the initial loading phase of a track |
| `Error`       | State indicating that a playback error stopped audio or prevented it from starting. Inspect the error and handle recovery; retrying or reloading depends on the platform and cause. Calling `play()` alone does not guarantee recovery on every backend. |

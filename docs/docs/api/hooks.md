---
sidebar_position: 3
---

# Hooks

These hooks subscribe to the player's state from React components. See
[Installation](../basics/installation.mdx) for the supported React Native setup.

## `useTrackPlayerEvents`

Register an event listener for one or more of the [events](./events.md) emitted by the TrackPlayer. The subscription is removed when the component unmounts.

Check out the [events section](./events.md) for a full list of supported events.

```tsx
import React, { useState } from 'react';
import { Text, View } from 'react-native';
import { useTrackPlayerEvents, Event, State } from '@evopixel/react-native-track-player';

// Subscribing to the following events inside MyComponent
const events = [
  Event.PlaybackState,
  Event.PlaybackError,
];

const MyComponent = () => {
  const [playerState, setPlayerState] = useState(null)

  useTrackPlayerEvents(events, (event) => {
    if (event.type === Event.PlaybackError) {
      console.warn('An error occured while playing the current track.');
    }
    if (event.type === Event.PlaybackState) {
      setPlayerState(event.state);
    }
  });

  const isPlaying = playerState === State.Playing;

  return (
    <View>
      <Text>The TrackPlayer is {isPlaying ? 'playing' : 'not playing'}</Text>
    </View>
  );
};
```

## `useProgress`

| State            | Type     | Description                      |
| ---------------- | -------- | -------------------------------- |
| position         | `number` | The current position in seconds  |
| buffered         | `number` | The buffered position in seconds |
| duration         | `number` | The duration in seconds          |

`useProgress` accepts an interval to set the rate (in miliseconds) to poll the track player's progress. The default value is `1000` or every second.

```tsx
import React from 'react';
import { Text, View } from 'react-native';
import { useProgress } from '@evopixel/react-native-track-player';

const MyComponent = () => {
  const { position, buffered, duration } = useProgress()

  return (
    <View>
      <Text>Track progress: {position} seconds out of {duration} total</Text>
      <Text>Buffered progress: {buffered} seconds buffered out of {duration} total</Text>
    </View>
  )
}
```

## `usePlaybackState`

A hook which returns the up to date state of [`getPlaybackState()`](./functions/player.md#getplaybackstate).
The hook will initially return `{ state: undefined }` while it is awaiting the
initial state of the player.

```tsx
import React, { useState } from 'react';
import { Text, View } from 'react-native';
import { usePlaybackState, State } from '@evopixel/react-native-track-player';

const MyComponent = () => {
  const playerState = usePlaybackState();
  const isPlaying = playerState.state === State.Playing;

  return (
    <View>
      <Text>The TrackPlayer is {isPlaying ? 'playing' : 'not playing'}</Text>
    </View>
  );
};
```

## `usePlayWhenReady`

A hook which returns the up to date state of `TrackPlayer.getPlayWhenReady()`.

## `useActiveTrack`

A hook which keeps track of the currently active track using
`TrackPlayer.getActiveTrack()` and `Event.PlaybackActiveTrackChanged`.

## `useIsPlaying`

Returns `{ playing, bufferingDuringPlay }`. Each field is `boolean | undefined`
until the player state and playback intent are known. This accounts for paused
buffering, errors and an ended queue. The named `isPlaying()` async function
provides the same result outside a React component.

See [Play Buttons](../guides/play-button.md) for how to use these values in the UI.

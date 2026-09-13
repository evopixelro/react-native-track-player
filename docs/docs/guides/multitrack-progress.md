---
sidebar_position: 4
---

# Multitrack Progress

If you're building an app that allows the playback of more than one Track you'll
probably also want to keep track of and display the users progress for each of
those tracks. **RNTP does not handle this for you**, but offers everything you
need in order to build it yourself.

## The Wrong Way

The most common misconception is that one could simply create a list of tracks
and then simply call `useProgress` in each of them to get their progress.
However, this doesn't work, as **`useProgress` is _only_ concerned with the
progress of the currently playing track!** If you attempt to do it this way
you'll quickly realize that all of your tracks are showing the exact same
progress, which given the understanding of `useProgress` above, should make
perfect sense.

UI hooks also cannot persist progress reliably when their components are
unmounted or the UI runtime is suspended during background playback.

## The Right Way

You're responsible for storing your progress on each track outside of RNTP, and
then using that progress when displaying things to your users. At a high-level,
what you need to do is store a record somewhere that associates a progress with
a unique track. Let's say we want to store a record that has a `track.id` and a
`track.progress`. Then what we want to do is _periodically_ update this record
while a given track is playing. Finally, when you want to display or otherwise
use your progress you should _read_ from the stored record (not from RNTP). See
the example below where we're going to use
[zustand](https://www.npmjs.com/package/zustand). Zustand will allow us to store
(and with some additional configuration, persist) our track progress AND it
gives us a nice way to dynamically update our progress displays in
realtime/reactively.

Please note, that the below solution assumes that you're adding an `id` property
to your `Track` object before you add it to RNTP, as RNTP does not add `id`'s
to your tracks by default, nor does it require them.

#### 1. Setup Zustand

First let's create a basic zustand store to store our progress in:

```ts
// src/store.ts
import { create } from 'zustand';

type ProgressStateStore = {
  map: Record<string, number>;
  setProgress: (id: string, progress: number) => void;
};

export const useProgressStateStore = create<ProgressStateStore>()(
  (set) => ({
    map: {},
    setProgress: (id: string, progress: number) => set((state) => ({
      map: { ...state.map, [id]: progress },
    })),
  })
);
```

Let's also set up a little helper hook to make it easier to read progress (we'll
use this later on):

```ts
// src/hooks/useTrackProgress.ts
import { useCallback } from 'react';
import { useProgressStateStore } from '../store';

export const useTrackProgress = (id: string | number): number => {
  return useProgressStateStore(useCallback(state => {
    return state.map[id.toString()] || 0;
  }, [id]));
};
```

#### 2. Listen To Progress Updates

Next we need to set up a listener for progress updates in our
[playback service](../basics/playback-service.md) and update our zustand store:

```ts
// src/services/PlaybackService.ts
import TrackPlayer, { Event } from '@evopixel/react-native-track-player';
import { useProgressStateStore } from '../store';

// create a local reference for the `setProgress` function
const setProgress = useProgressStateStore.getState().setProgress;

export const PlaybackService = async function() {
  TrackPlayer.addEventListener(Event.PlaybackProgressUpdated, async ({ position, track: trackIndex }) => {
    // get the track to fetch your unique ID property (if applicable)
    const track = await TrackPlayer.getTrack(trackIndex);
    if (!track || (typeof track.id !== 'string' && typeof track.id !== 'number')) return;
    // write progress to the zustand store
    setProgress(track.id.toString(), position);
  });
};
```

⚠️ make sure you've configured your `progressUpdateEventInterval`
in the `TrackPlayer.updateOptions` call.

This store is in memory. Add persistence if progress must survive an app restart;
see the [Zustand persistence guide](https://zustand.docs.pmnd.rs/integrations/persisting-store-data).
Progress events use queue indexes, so serialize queue edits with your own
progress handling if tracks can be removed while an asynchronous lookup runs.


#### 3. Reactively Update Progress

Finally, we just need to read from the store whenever we display our track list
item:

```ts
// src/components/TrackListItem.tsx
import React from 'react';
import { Text } from 'react-native';
import type { Track } from '@evopixel/react-native-track-player';
import { useTrackProgress } from '../hooks/useTrackProgress';

export interface TrackListItemProps extends Track {
  id: string | number;
}

export const TrackListItem: React.FC<TrackListItemProps> = (track) => {
  const progress = useTrackProgress(track.id);
  return (
    <Text>Progress: {progress}</Text>
  );
};
```

:confetti_ball: voilà

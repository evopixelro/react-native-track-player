---
sidebar_position: 2
---

# Saving Progress

A common use-case is to store the users progress on a particular `Track`
somewhere so that when they leave and come back, they can pick up right where
they left off. To do this you need to listen for progress updates and then
store the progress somewhere. There are two high level ways of getting this
done.

## Naive Approach

One approach could be to use the progress events/updates that the `useProgress`
hook provides. This isn't a very good idea and here's why:

Users can listen while the app is on screen or through background and system
media controls. During background playback, UI components may be unmounted or
their updates suspended. `useProgress` is useful for displaying the current
position, but persistence should not depend on a mounted React screen.

## Recommended Approach

The correct way to handle this is to track progress in the
[Playback Service](../basics/playback-service.md), based on the
`Event.PlaybackProgressUpdated` event. Set a positive
`progressUpdateEventInterval` in `TrackPlayer.updateOptions`; events are emitted
while playing, including supported background playback.

Persist the position together with your own stable track ID. Save a final
position when handling pause or a track change if losing the last interval is
unacceptable. Background execution still follows the
[platform lifecycle](../basics/background-mode.md); no JavaScript handler can
guarantee a final write after the operating system terminates the process.

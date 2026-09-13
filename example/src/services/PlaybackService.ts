import TrackPlayer, { Event } from '@evopixel/react-native-track-player';

// Report rejected remote commands without leaving an unhandled promise in the service.
const performCommand = async (command: () => Promise<unknown>) => {
  try {
    await command();
  } catch (error) {
    console.warn('Playback command failed:', error);
  }
};

export async function PlaybackService() {
  TrackPlayer.addEventListener(Event.RemotePlayPause, () => {
    console.log('Event.RemotePlayPause');
    return performCommand(async () => {
      if (await TrackPlayer.getPlayWhenReady()) {
        await TrackPlayer.pause();
      } else {
        await TrackPlayer.play();
      }
    });
  });

  TrackPlayer.addEventListener(Event.RemotePause, () => {
    console.log('Event.RemotePause');
    return performCommand(() => TrackPlayer.pause());
  });

  TrackPlayer.addEventListener(Event.RemotePlay, () => {
    console.log('Event.RemotePlay');
    return performCommand(() => TrackPlayer.play());
  });

  TrackPlayer.addEventListener(Event.RemoteNext, () => {
    console.log('Event.RemoteNext');
    return performCommand(() => TrackPlayer.skipToNext());
  });

  TrackPlayer.addEventListener(Event.RemotePrevious, () => {
    console.log('Event.RemotePrevious');
    return performCommand(() => TrackPlayer.skipToPrevious());
  });

  TrackPlayer.addEventListener(Event.RemoteJumpForward, async (event) => {
    console.log('Event.RemoteJumpForward', event);
    return performCommand(() => TrackPlayer.seekBy(event.interval));
  });

  TrackPlayer.addEventListener(Event.RemoteJumpBackward, async (event) => {
    console.log('Event.RemoteJumpBackward', event);
    return performCommand(() => TrackPlayer.seekBy(-event.interval));
  });

  TrackPlayer.addEventListener(Event.RemoteSeek, (event) => {
    console.log('Event.RemoteSeek', event);
    return performCommand(() => TrackPlayer.seekTo(event.position));
  });

  TrackPlayer.addEventListener(Event.RemoteStop, () => {
    return performCommand(() => TrackPlayer.stop());
  });

  TrackPlayer.addEventListener(Event.RemoteDuck, async (event) => {
    console.log('Event.RemoteDuck', event);
  });

  TrackPlayer.addEventListener(Event.PlaybackQueueEnded, (event) => {
    console.log('Event.PlaybackQueueEnded', event);
  });

  TrackPlayer.addEventListener(Event.PlaybackActiveTrackChanged, (event) => {
    console.log('Event.PlaybackActiveTrackChanged', event);
  });

  TrackPlayer.addEventListener(Event.PlaybackProgressUpdated, (event) => {
    console.log('Event.PlaybackProgressUpdated', event);
  });

  TrackPlayer.addEventListener(Event.PlaybackPlayWhenReadyChanged, (event) => {
    console.log('Event.PlaybackPlayWhenReadyChanged', event);
  });

  TrackPlayer.addEventListener(Event.PlaybackState, (event) => {
    console.log('Event.PlaybackState', event);
  });

  TrackPlayer.addEventListener(Event.MetadataChapterReceived, (event) => {
    console.log('Event.MetadataChapterReceived', event);
  });

  TrackPlayer.addEventListener(Event.MetadataTimedReceived, (event) => {
    console.log('Event.MetadataTimedReceived', event);
  });

  TrackPlayer.addEventListener(Event.MetadataCommonReceived, (event) => {
    console.log('Event.MetadataCommonReceived', event);
  });
}

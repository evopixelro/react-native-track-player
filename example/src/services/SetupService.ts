import TrackPlayer, {
  AppKilledPlaybackBehavior,
  Capability,
  RepeatMode,
} from '@evopixel/react-native-track-player';

export const DefaultRepeatMode = RepeatMode.Queue;
export const DefaultAudioServiceBehaviour =
  AppKilledPlaybackBehavior.StopPlaybackAndRemoveNotification;

let setupPromise: Promise<void> | undefined;

// Share setup across concurrent mounts and allow retry after a failed initialization.
export const SetupService = (): Promise<void> => {
  if (!setupPromise) {
    setupPromise = configurePlayer().catch((error: unknown) => {
      setupPromise = undefined;
      throw error;
    });
  }
  return setupPromise;
};

const configurePlayer = async () => {
  try {
    await TrackPlayer.setupPlayer({
      autoHandleInterruptions: true,
    });
  } catch (error) {
    if ((error as { code?: string }).code !== 'player_already_initialized') {
      throw error;
    }
  }
  try {
    await TrackPlayer.updateOptions({
      android: {
        appKilledPlaybackBehavior: DefaultAudioServiceBehaviour,
      },
      capabilities: [
        Capability.Play,
        Capability.Pause,
        Capability.Stop,
        Capability.SkipToNext,
        Capability.SkipToPrevious,
        Capability.SeekTo,
        Capability.JumpBackward,
        Capability.JumpForward,
      ],
      notificationCapabilities: [
        Capability.Play,
        Capability.Pause,
        Capability.SeekTo,
        Capability.SkipToNext,
        Capability.SkipToPrevious,
      ],
      progressUpdateEventInterval: 2,
    });
    await TrackPlayer.setRepeatMode(DefaultRepeatMode);
  } catch (error) {
    console.error('Error setting up player:', error);
    throw error;
  }
};

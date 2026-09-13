import React from 'react';
import { StyleSheet, TouchableWithoutFeedback, View } from 'react-native';
import TrackPlayer, {
  usePlaybackState,
} from '@evopixel/react-native-track-player';
import { Icon } from './PlatformControls';

import { PlaybackError } from './PlaybackError';
import { PlayPauseButton } from './PlayPauseButton';

// Keep rejected queue commands recoverable, including unavailable stream URLs.
const performSkipToNext = () =>
  TrackPlayer.skipToNext().catch((error) => console.warn('Next failed', error));
const performSkipToPrevious = () =>
  TrackPlayer.skipToPrevious().catch((error) =>
    console.warn('Previous failed', error)
  );

export const PlayerControls: React.FC = () => {
  const playback = usePlaybackState();
  return (
    <View style={styles.container}>
      <View style={styles.row}>
        <TouchableWithoutFeedback
          accessibilityRole="button"
          accessibilityLabel="Previous track"
          onPress={performSkipToPrevious}
        >
          <Icon name="backward" size={30} color="white" iconStyle="solid" />
        </TouchableWithoutFeedback>
        <PlayPauseButton />
        <TouchableWithoutFeedback
          accessibilityRole="button"
          accessibilityLabel="Next track"
          onPress={performSkipToNext}
        >
          <Icon name="forward" size={30} color="white" iconStyle="solid" />
        </TouchableWithoutFeedback>
      </View>
      <PlaybackError
        error={'error' in playback ? playback.error.message : undefined}
      />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    width: '100%',
  },
  row: {
    flexDirection: 'row',
    justifyContent: 'space-evenly',
    alignItems: 'center',
  },
});

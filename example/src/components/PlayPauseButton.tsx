import React from 'react';
import {
  ActivityIndicator,
  StyleSheet,
  TouchableWithoutFeedback,
  View,
} from 'react-native';
import TrackPlayer, { useIsPlaying } from '@evopixel/react-native-track-player';
import { Icon } from './PlatformControls';

export const PlayPauseButton: React.FC = () => {
  const { playing, bufferingDuringPlay } = useIsPlaying();

  return (
    <View style={styles.container}>
      {bufferingDuringPlay ? (
        <ActivityIndicator />
      ) : (
        <TouchableWithoutFeedback
          accessibilityRole="button"
          accessibilityLabel={playing ? 'Pause' : 'Play'}
          onPress={() =>
            (playing ? TrackPlayer.pause() : TrackPlayer.play()).catch(
              (error) => console.warn('Playback command failed', error)
            )
          }
        >
          <Icon
            name={playing ? 'pause' : 'play'}
            size={48}
            color="white"
            iconStyle="solid"
          />
        </TouchableWithoutFeedback>
      )}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    height: 50,
    width: 120,
    alignItems: 'center',
    justifyContent: 'center',
  },
});

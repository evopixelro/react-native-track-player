import React from 'react';
import { Image, StyleSheet, Text, View } from 'react-native';
import type { Track } from '@evopixel/react-native-track-player';

export const TrackInfo: React.FC<{
  track?: Track;
}> = ({ track }) => {
  const artwork = track?.artwork;
  const source = typeof artwork === 'string' ? { uri: artwork } : artwork;

  return (
    <View style={styles.container}>
      <Image style={styles.artwork} source={source} />
      <Text style={styles.titleText}>{track?.title}</Text>
      <Text style={styles.artistText}>{track?.artist}</Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    alignItems: 'center',
  },
  artwork: {
    width: '60%',
    aspectRatio: 1,
    marginTop: '2%',
    backgroundColor: 'grey',
  },
  titleText: {
    fontSize: 18,
    fontWeight: '600',
    color: 'white',
    marginTop: 30,
  },
  artistText: {
    fontSize: 16,
    fontWeight: '200',
    color: 'white',
  },
});

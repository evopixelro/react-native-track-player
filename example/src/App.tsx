import React, {
  useCallback,
  useEffect,
  useMemo,
  useRef,
  useState,
} from 'react';
import {
  ActivityIndicator,
  Linking,
  StatusBar,
  StyleSheet,
  View,
  Platform,
  Dimensions,
  Text,
} from 'react-native';
import TrackPlayer, {
  useActiveTrack,
} from '@evopixel/react-native-track-player';
import {
  BottomSheet,
  type BottomSheetHandle,
  GestureHandlerRootView,
  SafeAreaView,
  SafeAreaProvider,
} from './components/PlatformControls';

import {
  Button,
  OptionSheet,
  ActionSheet,
  PlayerControls,
  Progress,
  Spacer,
  TrackInfo,
} from './components';
import { QueueInitialTracksService, SetupService } from './services';
import { SponsorCard } from './components/SponsorCard';

export default function App() {
  return (
    <SafeAreaProvider>
      <GestureHandlerRootView style={styles.gestureContainer}>
        <Inner />
      </GestureHandlerRootView>
    </SafeAreaProvider>
  );
}

const Inner: React.FC = () => {
  const track = useActiveTrack();
  const { playerReady: isPlayerReady, setupError, retry } = useSetupPlayer();

  // options bottom sheet
  const optionsSheetRef = useRef<BottomSheetHandle>(null);
  const optionsSheetSnapPoints = useMemo(() => ['40%'], []);
  const handleOptionsPress = useCallback(() => {
    optionsSheetRef.current?.snapToIndex(0);
  }, [optionsSheetRef]);

  // actions bottom sheet
  const actionsSheetRef = useRef<BottomSheetHandle>(null);
  const actionsSheetSnapPoints = useMemo(() => ['40%'], []);
  const handleActionsPress = useCallback(() => {
    actionsSheetRef.current?.snapToIndex(0);
  }, [actionsSheetRef]);

  useEffect(() => {
    function deepLinkHandler(data: { url: string }) {
      console.log('deepLinkHandler', data.url);
    }

    // This event will be fired when the app is already open and the notification is clicked
    const subscription = Linking.addEventListener('url', deepLinkHandler);

    // When you launch the closed app from the notification or any other link
    Linking.getInitialURL().then((url) => console.log('getInitialURL', url));

    return () => {
      subscription.remove();
    };
  }, []);

  if (!isPlayerReady) {
    return (
      <SafeAreaView style={styles.screenContainer}>
        {setupError ? (
          <>
            <Text accessibilityRole="alert" style={styles.setupError}>
              {setupError}
            </Text>
            <Button title="Retry" onPress={retry} type="primary" />
          </>
        ) : (
          <ActivityIndicator />
        )}
      </SafeAreaView>
    );
  }

  return (
    <SafeAreaView style={styles.screenContainer}>
      <StatusBar barStyle={'light-content'} />
      <View style={styles.contentContainer}>
        <View style={styles.topBarContainer}>
          <Button title="Options" onPress={handleOptionsPress} type="primary" />
          <Button title="Actions" onPress={handleActionsPress} type="primary" />
        </View>
        <TrackInfo track={track} />
        <Progress live={track?.isLiveStream} />
        <Spacer />
        <PlayerControls />
        <Spacer mode={'expand'} />
        <SponsorCard />
      </View>
      <BottomSheet
        index={-1}
        ref={optionsSheetRef}
        enablePanDownToClose={true}
        snapPoints={optionsSheetSnapPoints}
        handleIndicatorStyle={styles.sheetHandle}
        backgroundStyle={styles.sheetBackgroundContainer}
      >
        <OptionSheet />
      </BottomSheet>
      <BottomSheet
        index={-1}
        ref={actionsSheetRef}
        enablePanDownToClose={true}
        snapPoints={actionsSheetSnapPoints}
        handleIndicatorStyle={styles.sheetHandle}
        backgroundStyle={styles.sheetBackgroundContainer}
      >
        <ActionSheet />
      </BottomSheet>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  gestureContainer: { flex: 1 },
  screenContainer: {
    flex: 1,
    backgroundColor: '#212121',
    alignItems: 'center',
    justifyContent: 'center',
    minHeight: Platform.OS === 'web' ? Dimensions.get('window').height : '100%',
  },
  contentContainer: {
    flex: 1,
    alignItems: 'center',
  },
  topBarContainer: {
    width: '100%',
    flexDirection: 'row',
    justifyContent: 'flex-end',
  },
  sheetBackgroundContainer: {
    backgroundColor: '#181818',
  },
  sheetHandle: {
    backgroundColor: 'white',
  },
  setupError: {
    color: 'white',
    margin: 16,
  },
});

function useSetupPlayer() {
  const [playerReady, setPlayerReady] = useState<boolean>(false);
  const [setupError, setSetupError] = useState<string>();
  const [attempt, setAttempt] = useState(0);
  const retry = useCallback(() => setAttempt((value) => value + 1), []);

  useEffect(() => {
    let unmounted = false;
    setSetupError(undefined);
    (async () => {
      await SetupService();
      if (unmounted) return;
      const queue = await TrackPlayer.getQueue();
      if (unmounted) return;
      if (queue.length <= 0) {
        await QueueInitialTracksService();
      }
      if (!unmounted) setPlayerReady(true);
    })().catch((error: unknown) => {
      if (!unmounted) {
        setSetupError(
          error instanceof Error
            ? error.message
            : 'Unable to initialize playback.'
        );
      }
    });
    return () => {
      unmounted = true;
    };
  }, [attempt]);
  return { playerReady, setupError, retry };
}

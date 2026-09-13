/**
 * @format
 */

import React from 'react';
import 'react-native';
import App from '../src/App';
import TrackPlayer, { useIsPlaying } from '@evopixel/react-native-track-player';
import { SetupService, QueueInitialTracksService } from '../src/services';

// Note: test renderer must be required after react-native.
import renderer, { act } from 'react-test-renderer';

jest.mock('@evopixel/react-native-track-player', () => ({
  __esModule: true,
  default: {
    getQueue: jest.fn().mockResolvedValue([]),
    play: jest.fn().mockResolvedValue(undefined),
    pause: jest.fn().mockResolvedValue(undefined),
    skipToNext: jest.fn().mockResolvedValue(undefined),
    skipToPrevious: jest.fn().mockResolvedValue(undefined),
    getRepeatMode: jest.fn().mockResolvedValue(2),
  },
  useActiveTrack: () => ({ title: 'Example track', artist: 'Example artist' }),
  usePlaybackState: () => ({ state: 'paused' }),
  useIsPlaying: jest.fn().mockReturnValue({ playing: false }),
  useProgress: () => ({ position: 12, duration: 60, buffered: 30 }),
  RepeatMode: { Off: 0, Track: 1, Queue: 2 },
  AppKilledPlaybackBehavior: {},
}));

jest.mock('../src/services', () => ({
  SetupService: jest.fn().mockResolvedValue(undefined),
  QueueInitialTracksService: jest.fn().mockResolvedValue(undefined),
  DefaultRepeatMode: 2,
}));

jest.mock('react-native-gesture-handler', () => ({
  GestureHandlerRootView: require('react-native').View,
}));

jest.mock('react-native-safe-area-context', () => ({
  SafeAreaProvider: require('react-native').View,
  SafeAreaView: require('react-native').View,
}));

jest.mock('@gorhom/bottom-sheet', () => ({
  __esModule: true,
  default: require('react-native').View,
  BottomSheetScrollView: require('react-native').ScrollView,
}));

jest.mock('@react-native-community/slider', () => require('react-native').View);
jest.mock(
  '@react-native-segmented-control/segmented-control',
  () => require('react-native').View
);
jest.mock(
  '@react-native-vector-icons/fontawesome6',
  () => require('react-native').Text
);

describe('Example app', () => {
  let tree: renderer.ReactTestRenderer;

  beforeEach(() => {
    jest.mocked(TrackPlayer.getQueue).mockResolvedValue([]);
    jest.mocked(SetupService).mockResolvedValue(undefined);
    jest
      .mocked(useIsPlaying)
      .mockReturnValue({ playing: false, bufferingDuringPlay: false });
  });

  afterEach(async () => {
    if (tree) await act(async () => tree.unmount());
  });

  // Preserve the upstream smoke test while waiting for asynchronous player setup.
  it('renders correctly and queues the example tracks', async () => {
    await act(async () => {
      tree = renderer.create(<App />);
    });
    expect(tree!.toJSON()).not.toBeNull();
    expect(SetupService).toHaveBeenCalledTimes(1);
    expect(QueueInitialTracksService).toHaveBeenCalledTimes(1);
    expect(tree!.root.findByProps({ children: 'Example track' })).toBeTruthy();
  });

  // Remounting must not append the sample playlist over an existing native queue.
  it('keeps an existing playback queue', async () => {
    jest
      .mocked(TrackPlayer.getQueue)
      .mockResolvedValue([{ url: 'file:///audio.mp3' }]);
    await act(async () => {
      tree = renderer.create(<App />);
    });
    expect(QueueInitialTracksService).not.toHaveBeenCalled();
  });

  // Exercise the rendered controls rather than replacing the player UI with a mock.
  it('connects Play and Previous/Next to the fork API', async () => {
    await act(async () => {
      tree = renderer.create(<App />);
    });
    for (const label of ['Play', 'Previous track', 'Next track']) {
      await act(async () => {
        await tree.root
          .findAllByProps({ accessibilityLabel: label })[0]!
          .props.onPress();
      });
    }
    expect(TrackPlayer.play).toHaveBeenCalledTimes(1);
    expect(TrackPlayer.skipToPrevious).toHaveBeenCalledTimes(1);
    expect(TrackPlayer.skipToNext).toHaveBeenCalledTimes(1);
  });

  // A playing track must expose Pause without changing the control's appearance.
  it('connects Pause when playback is active', async () => {
    jest
      .mocked(useIsPlaying)
      .mockReturnValue({ playing: true, bufferingDuringPlay: false });
    await act(async () => {
      tree = renderer.create(<App />);
    });
    await act(async () => {
      await tree.root
        .findAllByProps({ accessibilityLabel: 'Pause' })[0]!
        .props.onPress();
    });
    expect(TrackPlayer.pause).toHaveBeenCalledTimes(1);
  });

  // Unavailable sources and queue boundaries must not produce unhandled rejections.
  it.each([
    ['Play', 'play'],
    ['Pause', 'pause'],
    ['Next track', 'skipToNext'],
    ['Previous track', 'skipToPrevious'],
  ] as const)('handles a rejected %s command', async (label, method) => {
    const failure = new Error('Unavailable media');
    jest.mocked(TrackPlayer[method]).mockRejectedValueOnce(failure);
    jest.mocked(useIsPlaying).mockReturnValue({
      playing: label === 'Pause',
      bufferingDuringPlay: false,
    });
    const warning = jest.spyOn(console, 'warn').mockImplementation(() => {});
    try {
      await act(async () => {
        tree = renderer.create(<App />);
      });
      await act(async () => {
        await expect(
          tree.root
            .findAllByProps({ accessibilityLabel: label })[0]!
            .props.onPress()
        ).resolves.toBeUndefined();
      });
      expect(warning).toHaveBeenCalledWith(expect.any(String), failure);
      expect(tree.toJSON()).not.toBeNull();
    } finally {
      warning.mockRestore();
    }
  });

  // Reopening a desktop sheet must reflect native state rather than its initial default.
  it('restores the current repeat mode when mounting the options', async () => {
    jest.mocked(TrackPlayer.getRepeatMode).mockResolvedValueOnce(1);
    await act(async () => {
      tree = renderer.create(<App />);
    });
    const repeat = tree.root.findAll(
      (node) =>
        Array.isArray(node.props.values) && node.props.values[0] === 'Off'
    )[0]!;
    expect(repeat.props.selectedIndex).toBe(1);
  });

  // Setup failure must be visible and recoverable, not an endless loading spinner.
  it('allows retry after setup fails', async () => {
    jest
      .mocked(SetupService)
      .mockRejectedValueOnce(new Error('Audio initialization failed'));
    await act(async () => {
      tree = renderer.create(<App />);
    });
    expect(
      tree!.root.findAllByProps({ accessibilityRole: 'alert' }).length
    ).toBeGreaterThan(0);
    await act(async () => {
      tree.root.findByProps({ title: 'Retry' }).props.onPress();
    });
    expect(SetupService).toHaveBeenCalledTimes(2);
    expect(
      tree!.root.findAllByProps({ accessibilityRole: 'alert' })
    ).toHaveLength(0);
  });
});

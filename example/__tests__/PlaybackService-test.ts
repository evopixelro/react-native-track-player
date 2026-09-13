import TrackPlayer, { Event } from '@evopixel/react-native-track-player';
import { PlaybackService } from '../src/services/PlaybackService';

jest.mock('@evopixel/react-native-track-player', () => ({
  __esModule: true,
  Event: jest.requireActual('../../src/constants/Event').Event,
  default: {
    addEventListener: jest.fn(),
    getPlayWhenReady: jest.fn().mockResolvedValue(false),
    play: jest.fn().mockResolvedValue(undefined),
    pause: jest.fn().mockResolvedValue(undefined),
    stop: jest.fn().mockResolvedValue(undefined),
    skipToNext: jest.fn().mockResolvedValue(undefined),
    skipToPrevious: jest.fn().mockResolvedValue(undefined),
    seekBy: jest.fn().mockResolvedValue(undefined),
    seekTo: jest.fn().mockResolvedValue(undefined),
  },
}));

describe('Example playback service', () => {
  let listeners: Map<string, (event: object) => unknown>;

  beforeEach(async () => {
    listeners = new Map();
    jest
      .mocked(TrackPlayer.addEventListener)
      .mockImplementation((event, listener) => {
        listeners.set(event, listener as (event: object) => unknown);
        return { remove: jest.fn() } as unknown as ReturnType<
          typeof TrackPlayer.addEventListener
        >;
      });
    jest.spyOn(console, 'log').mockImplementation(() => {});
    await PlaybackService();
  });

  afterEach(() => jest.restoreAllMocks());

  // Both directions of a headset toggle must use the current native playback intent.
  it.each([false, true])(
    'toggles play/pause when playWhenReady is %s',
    async (ready) => {
      jest.mocked(TrackPlayer.getPlayWhenReady).mockResolvedValue(ready);
      await listeners.get(Event.RemotePlayPause)!({});
      expect(
        ready ? TrackPlayer.pause : TrackPlayer.play
      ).toHaveBeenCalledTimes(1);
      expect(
        ready ? TrackPlayer.play : TrackPlayer.pause
      ).not.toHaveBeenCalled();
    }
  );

  // Each media-session button must call its corresponding native command once.
  it.each([
    [Event.RemotePlay, 'play'],
    [Event.RemotePause, 'pause'],
    [Event.RemoteStop, 'stop'],
    [Event.RemoteNext, 'skipToNext'],
    [Event.RemotePrevious, 'skipToPrevious'],
  ] as const)('handles %s', async (event, command) => {
    await listeners.get(event)!({});
    expect(TrackPlayer[command]).toHaveBeenCalledTimes(1);
  });

  // Jump backward uses a negative offset; seeking keeps fractional seconds intact.
  it('forwards seek and jump positions', async () => {
    await listeners.get(Event.RemoteJumpForward)!({ interval: 15 });
    await listeners.get(Event.RemoteJumpBackward)!({ interval: 10 });
    await listeners.get(Event.RemoteSeek)!({ position: 12.75 });
    expect(TrackPlayer.seekBy).toHaveBeenNthCalledWith(1, 15);
    expect(TrackPlayer.seekBy).toHaveBeenNthCalledWith(2, -10);
    expect(TrackPlayer.seekTo).toHaveBeenCalledWith(12.75);
  });

  // An unavailable next track must not cause an unhandled service rejection.
  it('handles a rejected remote command', async () => {
    const error = new Error('No next track');
    const warning = jest.spyOn(console, 'warn').mockImplementation(() => {});
    jest.mocked(TrackPlayer.skipToNext).mockRejectedValueOnce(error);
    await expect(listeners.get(Event.RemoteNext)!({})).resolves.toBeUndefined();
    expect(warning).toHaveBeenCalledWith('Playback command failed:', error);
  });
});

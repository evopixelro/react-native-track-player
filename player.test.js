const fs = require('node:fs');
const path = require('node:path');
const { execFileSync } = require('node:child_process');

// Load the public API with an isolated native bridge
function createPlayer(platform = 'android') {
  jest.resetModules();
  const methods = [
    'setupPlayer',
    'updateOptions',
    'add',
    'load',
    'setQueue',
    'play',
    'pause',
    'stop',
    'skipToNext',
    'skipToPrevious',
    'seekTo',
    'updateMetadataForTrack',
    'acquireWakeLock',
    'abandonWakeLock',
  ];
  const native = Object.fromEntries(
    methods.map((name) => [name, jest.fn().mockResolvedValue(undefined)])
  );
  const subscription = { remove: jest.fn() };
  const emitter = { addListener: jest.fn(() => subscription) };
  const registry = { registerHeadlessTask: jest.fn() };
  jest.doMock('react-native', () => ({
    AppRegistry: registry,
    Platform: { OS: platform },
    NativeEventEmitter: jest.fn(() => emitter),
  }));
  jest.doMock('./src/NativeTrackPlayer', () => ({
    __esModule: true,
    default: native,
    Constants: {},
  }));
  jest.doMock('./src/resolveAssetSource', () => ({
    __esModule: true,
    default: (id) => ({ uri: `asset://${id}` }),
  }));
  return {
    player: require('./src/trackPlayer'),
    native,
    emitter,
    registry,
    subscription,
  };
}

describe('Playback API', () => {
  test('passes setup options to the native player', async () => {
    const { player, native } = createPlayer();
    const options = { minBuffer: 10, maxBuffer: 30 };
    await player.setupPlayer(options);
    expect(native.setupPlayer).toHaveBeenCalledWith(options);
  });

  test.each(['play', 'pause', 'stop'])('forwards %s', async (command) => {
    const { player, native } = createPlayer();
    await player[command]();
    expect(native[command]).toHaveBeenCalledTimes(1);
  });

  test('propagates native failures', async () => {
    const { player, native } = createPlayer();
    native.play.mockRejectedValueOnce(new Error('Source unavailable'));
    await expect(player.play()).rejects.toThrow('Source unavailable');
  });

  test.each(['skipToNext', 'skipToPrevious'])(
    'forwards %s without stopping playback',
    async (command) => {
      const { player, native } = createPlayer();
      await player[command](12);
      expect(native[command]).toHaveBeenCalledWith(12);
      expect(native.stop).not.toHaveBeenCalled();
    }
  );

  test('preserves Unicode metadata and source headers', async () => {
    const { player, native } = createPlayer();
    const track = {
      url: 'https://example.com/live',
      title: 'Și музыка',
      headers: { Authorization: 'test' },
      isLiveStream: true,
    };
    await player.load(track);
    expect(native.load).toHaveBeenCalledWith(expect.objectContaining(track));
  });

  test('resolves bundled artwork in a replacement playlist', async () => {
    const { player, native } = createPlayer();
    await player.setQueue([
      { url: 'https://example.com/track.mp3', artwork: 8 },
    ]);
    expect(native.setQueue).toHaveBeenCalledWith([
      expect.objectContaining({ artwork: { uri: 'asset://8' } }),
    ]);
  });

  test('keeps Next and Previous as distinct capabilities', async () => {
    const { player, native } = createPlayer();
    const { Capability } = require('./src/constants');
    await player.updateOptions({
      capabilities: [
        Capability.SkipToPrevious,
        Capability.Play,
        Capability.SkipToNext,
      ],
    });
    expect(native.updateOptions).toHaveBeenCalledWith(
      expect.objectContaining({ capabilities: [9, 1, 8] })
    );
  });

  test('updates metadata without restarting playback', async () => {
    const { player, native } = createPlayer();
    await player.updateMetadataForTrack(0, { title: 'Altă melodie' });
    expect(native.updateMetadataForTrack).toHaveBeenCalledWith(
      0,
      expect.objectContaining({ title: 'Altă melodie' })
    );
    expect(native.stop).not.toHaveBeenCalled();
    expect(native.load).not.toHaveBeenCalled();
  });

  test('registers the Android headless service', () => {
    const { player, registry } = createPlayer();
    const factory = jest.fn(() => jest.fn());
    player.registerPlaybackService(factory);
    expect(registry.registerHeadlessTask).toHaveBeenCalledWith(
      'TrackPlayer',
      factory
    );
    expect(factory).not.toHaveBeenCalled();
  });

  test.each(['ios', 'macos', 'windows'])(
    'starts the %s service on the next tick',
    (platform) => {
      jest.useFakeTimers();
      try {
        const { player, registry } = createPlayer(platform);
        const service = jest.fn();
        player.registerPlaybackService(() => service);
        expect(service).not.toHaveBeenCalled();
        jest.runAllTimers();
        expect(service).toHaveBeenCalledTimes(1);
        expect(registry.registerHeadlessTask).not.toHaveBeenCalled();
      } finally {
        jest.useRealTimers();
      }
    }
  );

  test('returns removable event subscriptions', () => {
    const { player, emitter, subscription } = createPlayer();
    const handler = jest.fn();
    expect(player.addEventListener('remote-next', handler)).toBe(subscription);
    expect(emitter.addListener).toHaveBeenCalledWith('remote-next', handler);
  });
});

// Check native patch contracts without claiming to exercise device playback
function source(relativePath) {
  return fs.readFileSync(path.join(__dirname, relativePath), 'utf8');
}

describe('Native patch contracts', () => {
  test.each([
    ['PREVIOUS', 'SKIP_TO_PREVIOUS'],
    ['NEXT', 'SKIP_TO_NEXT'],
  ])('%s uses its Android capability', (button, capability) => {
    const code = source(
      'android/src/main/java/com/doublesymmetry/kotlinaudio/models/CustomCommandButton.kt'
    );
    expect(code).toMatch(
      new RegExp(
        `${button}\\(\\s*customAction = "${button}",\\s*capability = Capability\\.${capability}`
      )
    );
  });

  test('exports the iOS constants required by codegen', () => {
    const spec = source('src/NativeTrackPlayer.ts');
    const implementation = source('ios/TrackPlayer.mm');
    const constants = [
      ...spec.matchAll(/^\s+([A-Z][A-Z_]+): (number|string);/gm),
    ].map((entry) => entry[1]);
    expect(constants.length).toBeGreaterThan(20);
    for (const name of constants) {
      expect(implementation).toMatch(new RegExp(`\\.${name}\\s*=`));
    }
  });

  test('maps numeric iOS navigation and pitch constants', () => {
    const code = source('ios/TrackPlayer.swift');
    expect(code).toMatch(/8:\s*"next"/);
    expect(code).toMatch(/9:\s*"previous"/);
    expect(code).toContain('options["capabilities"] as? [Int]');
    expect(source('ios/Models/Track.swift')).toMatch(
      /1:\s*"linear",\s*2:\s*"music",\s*3:\s*"voice"/
    );
  });

  test('completes setQueue without an undefined index', () => {
    const code = source('ios/TrackPlayer.swift');
    const start = code.indexOf('public func setQueue(');
    const method = code.slice(
      start,
      code.indexOf('public func getActiveTrack(', start)
    );
    expect(method).toContain('resolve(NSNull())');
    expect(method).not.toContain('resolve(index)');
  });

  test('uses the iOS main queue and expected event selector', () => {
    expect(source('ios/TrackPlayer.mm')).toContain(
      'return dispatch_get_main_queue();'
    );
    expect(source('ios/TrackPlayer.swift')).toContain('@objc(sendEvent:body:)');
  });

  test('keeps scoped-package native names stable', () => {
    const manifest = JSON.parse(source('package.json'));
    expect(manifest.name).toBe('@evopixel/react-native-track-player');
    expect(manifest.version).toBe('5.0.4');
    expect(manifest.codegenConfig.ios.modulesProvider).toEqual({
      TrackPlayer: 'TrackPlayer',
    });
    expect(source('react-native-track-player.podspec')).toContain(
      's.module_name  = "react_native_track_player"'
    );
  });

  test('points the source export at the library entry point', () => {
    const manifest = JSON.parse(source('package.json'));
    expect(source(manifest.exports['.'].source)).toContain('./trackPlayer');
  });

  test('keeps npm repository URLs compatible with CocoaPods', () => {
    const manifest = JSON.parse(source('package.json'));
    const podspec = source('react-native-track-player.podspec');
    expect(manifest.repository.url).toBe(
      'git+https://github.com/evopixelro/react-native-track-player.git'
    );
    expect(podspec).toContain(
      String.raw`repository_url = package["repository"]["url"].sub(/\Agit\+/, '')`
    );
    expect(podspec).toContain('s.homepage     = package["homepage"]');
    expect(podspec).toContain(':git => repository_url, :branch => "main"');
    expect(new URL(manifest.homepage).protocol).toBe('https:');
  });

  test('retains the Apache license and upstream attribution', () => {
    expect(source('LICENSE')).toContain('Version 2.0, January 2004');
    const manifest = JSON.parse(source('package.json'));
    expect(manifest.contributors).toEqual(
      expect.arrayContaining([expect.objectContaining({ name: 'Kiryl-Sadko' })])
    );
    expect(source('README.md')).toContain(
      'doublesymmetry/react-native-track-player'
    );
  });

  test('keeps macOS implementation isolated from iOS-only dependencies', () => {
    const podspec = source('react-native-track-player.podspec');
    expect(podspec).toContain('s.ios.dependency "SwiftAudioEx", "1.1.0"');
    expect(podspec).toContain('s.osx.source_files = "ios/TrackPlayer.{h,mm}"');
    expect(podspec).toContain(
      's.osx.exclude_files = "macos/player.test.swift"'
    );
    const macos = source('macos/TrackPlayer.swift');
    expect(macos).not.toMatch(/import (UIKit|SwiftAudioEx)/);
    expect(macos).toContain('private let player = AVPlayer()');
    expect(macos).toContain('@objc(sendEvent:body:)');
  });

  test('implements every selector forwarded by the shared Apple bridge', () => {
    const bridge = source('ios/TrackPlayer.mm');
    const macos = source('macos/TrackPlayer.swift');
    const calls = [...bridge.matchAll(/\[nativeTrackPlayer ([^\]]+)\]/g)];
    expect(calls.length).toBeGreaterThan(30);
    for (const [, call] of calls) {
      const selector = [...call.matchAll(/(\w+):/g)]
        .map((match) => `${match[1]}:`)
        .join('');
      expect(macos).toContain(`@objc(${selector})`);
    }
  });
});

// Bound parser execution so a recursive type alias fails instead of hanging CI
test('the native specification generates a schema without recursive aliases', () => {
  const script = `
    const { TypeScriptParser } = require('@react-native/codegen/lib/parsers/typescript/parser');
    const schema = new TypeScriptParser().parseFile('src/NativeTrackPlayer.ts');
    process.stdout.write(JSON.stringify(schema));
  `;
  const result = execFileSync(process.execPath, ['-e', script], {
    cwd: __dirname,
    timeout: 10000,
    encoding: 'utf8',
    windowsHide: true,
  });
  const schema = JSON.parse(result);
  expect(Object.keys(schema.modules)).toHaveLength(1);
  expect(
    Object.values(schema.modules).map((module) => module.moduleName)
  ).toEqual(['TrackPlayer']);
});

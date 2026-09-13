const { execFileSync } = require('node:child_process');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

if (process.platform !== 'darwin') {
  throw new Error(
    'Native macOS tests require macOS and the Xcode command-line tools.'
  );
}

const temporary = fs.mkdtempSync(path.join(os.tmpdir(), 'rntp-macos-test-'));
const run = (args) =>
  execFileSync('xcrun', ['swiftc', ...args], {
    stdio: 'inherit',
    timeout: 120000,
  });

try {
  // Only the React promise typedefs are substituted; AVPlayer and MediaPlayer are real.
  fs.writeFileSync(
    path.join(temporary, 'React.swift'),
    [
      'import Foundation',
      'public typealias RCTPromiseResolveBlock = @convention(block) (Any?) -> Void',
      'public typealias RCTPromiseRejectBlock = @convention(block) (String?, String?, Error?) -> Void',
      '',
    ].join('\n')
  );
  run([
    '-emit-module',
    '-emit-library',
    '-module-name',
    'React',
    path.join(temporary, 'React.swift'),
    '-emit-module-path',
    path.join(temporary, 'React.swiftmodule'),
    '-o',
    path.join(temporary, 'libReact.dylib'),
  ]);
  const binary = path.join(temporary, 'player-test');
  run([
    '-swift-version',
    '5',
    '-I',
    temporary,
    '-L',
    temporary,
    '-lReact',
    '-Xlinker',
    '-rpath',
    '-Xlinker',
    temporary,
    path.join(__dirname, 'Models/TrackQueue.swift'),
    path.join(__dirname, 'TrackPlayer.swift'),
    path.join(__dirname, 'player.test.swift'),
    '-o',
    binary,
  ]);
  execFileSync(binary, [], { stdio: 'inherit', timeout: 60000 });
} finally {
  // Remove only the fresh directory created by this test invocation.
  fs.rmSync(temporary, { recursive: true, force: true });
}

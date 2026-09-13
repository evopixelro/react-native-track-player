const { spawnSync } = require('node:child_process');
const { mkdtempSync, rmSync } = require('node:fs');
const os = require('node:os');
const path = require('node:path');

// Compile the actual queue implementation and run it without a media device.
const directory = mkdtempSync(path.join(os.tmpdir(), 'rntp-windows-queue-'));
const executable = path.join(
  directory,
  process.platform === 'win32' ? 'queue.exe' : 'queue'
);
const source = path.join(__dirname, 'player.test.cpp');
const compiler =
  process.env.CXX || (process.platform === 'win32' ? 'cl' : 'c++');
const microsoft = /(^|[\\/])cl(?:\.exe)?$/i.test(compiler);

try {
  const args = microsoft
    ? [
        '/nologo',
        '/std:c++17',
        '/EHsc',
        source,
        `/Fe:${executable}`,
        `/Fo:${path.join(directory, 'queue.obj')}`,
      ]
    : ['-std=c++17', '-Wall', '-Wextra', '-Werror', source, '-o', executable];
  const build = spawnSync(compiler, args, { cwd: directory, stdio: 'inherit' });
  if (build.error) throw build.error;
  if (build.status !== 0)
    throw new Error(`Queue test compilation failed (${build.status}).`);
  const test = spawnSync(executable, [], { cwd: directory, stdio: 'inherit' });
  if (test.error) throw test.error;
  if (test.status !== 0)
    throw new Error(`Queue regression tests failed (${test.status}).`);
} finally {
  rmSync(directory, { recursive: true, force: true });
}

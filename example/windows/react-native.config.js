const path = require('node:path');

module.exports = {
  project: {
    windows: {
      sourceDir: 'native',
      solutionFile: 'TrackPlayerExample.sln',
      project: { projectFile: 'TrackPlayerExample/TrackPlayerExample.vcxproj' },
    },
  },
  dependencies: {
    '@evopixel/react-native-track-player': {
      root: path.resolve(__dirname, '../..'),
    },
  },
};

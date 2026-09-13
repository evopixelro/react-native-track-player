const path = require('node:path');

module.exports = {
  reactNativePath: path.join(__dirname, 'node_modules/react-native-macos'),
  project: {
    ios: { sourceDir: 'macos' },
    macos: { sourceDir: 'macos' },
  },
  dependencies: {
    '@evopixel/react-native-track-player': {
      root: path.resolve(__dirname, '../..'),
    },
  },
};

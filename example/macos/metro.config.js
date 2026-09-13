const path = require('node:path');
const { getDefaultConfig, mergeConfig } = require('@react-native/metro-config');

const library = path.resolve(__dirname, '../..');
const modules = path.join(__dirname, 'node_modules');
const macos = path.join(modules, 'react-native-macos');

module.exports = mergeConfig(getDefaultConfig(__dirname), {
  maxWorkers: 2,
  watchFolders: [
    path.join(library, 'src'),
    path.join(library, 'web'),
    path.resolve(__dirname, '../src'),
  ],
  resolver: {
    disableHierarchicalLookup: true,
    nodeModulesPaths: [modules],
    blockList: [/.*[\\/]macos[\\/]macos[\\/](Pods|build)[\\/].*/],
    resolveRequest: (context, name, platform) => {
      if (name === 'react-native' || name.startsWith('react-native/')) {
        return context.resolveRequest(
          context,
          path.join(macos, name.slice('react-native'.length)),
          platform
        );
      }
      if (name === '@evopixel/react-native-track-player') {
        return context.resolveRequest(
          context,
          path.join(library, 'src/index.tsx'),
          platform
        );
      }
      return context.resolveRequest(context, name, platform);
    },
  },
});

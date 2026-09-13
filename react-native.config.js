module.exports = {
  dependency: {
    platforms: {
      windows: {
        sourceDir: 'windows',
        solutionFile: 'RNTrackPlayer.sln',
        projects: [
          {
            projectFile: 'RNTrackPlayer\\RNTrackPlayer.vcxproj',
            directDependency: true,
          },
        ],
      },
    },
  },
};

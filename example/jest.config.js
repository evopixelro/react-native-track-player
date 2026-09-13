module.exports = {
  preset: '@react-native/jest-preset',
  transform: {
    '^.+\\.[jt]sx?$': [
      'babel-jest',
      {
        babelrc: false,
        configFile: false,
        presets: ['module:@react-native/babel-preset'],
      },
    ],
  },
  testMatch: [
    '<rootDir>/__tests__/**/*-test.tsx',
    '<rootDir>/__tests__/**/*-test.ts',
  ],
  cacheDirectory: '<rootDir>/node_modules/.cache/jest',
  clearMocks: true,
};

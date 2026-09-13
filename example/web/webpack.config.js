const path = require('node:path');
const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const example = path.resolve(__dirname, '..');
const library = path.resolve(example, '..');

module.exports = (_, argv) => ({
  context: example,
  entry: './web/index.js',
  output: {
    path: path.join(__dirname, 'build'),
    filename: '[name].[contenthash].js',
    clean: true,
  },
  resolve: {
    alias: {
      'react-native$': require.resolve('react-native-web'),
      'react$': path.join(example, 'node_modules/react'),
      'react-dom$': path.join(example, 'node_modules/react-dom'),
      '@evopixel/react-native-track-player$': path.join(
        library,
        'src/index.tsx'
      ),
    },
    extensions: [
      '.web.tsx',
      '.web.ts',
      '.web.js',
      '.tsx',
      '.ts',
      '.js',
      '.jsx',
      '.mjs',
    ],
    mainFields: ['react-native', 'browser', 'module', 'main'],
    modules: [path.join(example, 'node_modules'), 'node_modules'],
  },
  module: {
    rules: [
      {
        test: /\.[jt]sx?$/,
        exclude:
          /node_modules[\\/](?:react|react-dom|shaka-player|react-native-web)[\\/]/,
        resolve: { fullySpecified: false },
        use: {
          loader: 'babel-loader',
          options: {
            babelrc: false,
            configFile: false,
            presets: [
              [
                'module:@react-native/babel-preset',
                { disableImportExportTransform: true },
              ],
            ],
            plugins: ['react-native-worklets/plugin'],
          },
        },
      },
      {
        test: /\.(png|jpe?g|gif|webp|ttf|otf|mp3|mp4|m4a|wav)$/,
        type: 'asset/resource',
      },
    ],
  },
  plugins: [
    new HtmlWebpackPlugin({ template: path.join(__dirname, 'index.html') }),
    // This optional native image renderer is not used by browser text icons.
    new webpack.IgnorePlugin({
      resourceRegExp: /^@react-native-vector-icons\/get-image$/,
    }),
    new webpack.DefinePlugin({
      __DEV__: JSON.stringify(argv.mode !== 'production'),
    }),
  ],
  devServer: {
    host: '127.0.0.1',
    port: 5173,
    historyApiFallback: true,
    hot: true,
  },
});

import { AppRegistry } from 'react-native';
import TrackPlayer from '@evopixel/react-native-track-player';
import App from '../src/App';
import { PlaybackService } from '../src/services';
import app from './app.json';

AppRegistry.registerComponent(app.name, () => App);
TrackPlayer.registerPlaybackService(() => PlaybackService);

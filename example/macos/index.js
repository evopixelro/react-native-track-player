import { AppRegistry } from 'react-native';
import TrackPlayer from '@evopixel/react-native-track-player';
import App from '../src/App';
import { PlaybackService } from '../src/services';

AppRegistry.registerComponent('TrackPlayerExample', () => App);
TrackPlayer.registerPlaybackService(() => PlaybackService);

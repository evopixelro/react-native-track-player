import { AppRegistry } from 'react-native';
import '../index';
import app from '../app.json';
import solidFont from '../node_modules/@react-native-vector-icons/fontawesome6/fonts/FontAwesome6_Solid.ttf';

// Load the same icon font used by the native example before rendering controls.
const font = new FontFace('FontAwesome6Free-Solid', `url(${solidFont})`);
document.fonts.add(font);
font
  .load()
  .catch((error) => console.warn('Unable to load the icon font:', error));

AppRegistry.runApplication(app.name, {
  rootTag: document.getElementById('root'),
});

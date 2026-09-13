import BottomSheet from '@gorhom/bottom-sheet';
import type { ComponentType } from 'react';
import NativeSegmentedControl, {
  type SegmentedControlProps,
} from '@react-native-segmented-control/segmented-control';

// The upstream class declaration extends NativeMethods, removed in RN 0.87.
// Keep its supported props and unchanged native component without that legacy mixin.
export const SegmentedControl =
  NativeSegmentedControl as unknown as ComponentType<SegmentedControlProps>;

export { BottomSheet };
export type BottomSheetHandle = BottomSheet;
export { BottomSheetScrollView } from '@gorhom/bottom-sheet';
export { GestureHandlerRootView } from 'react-native-gesture-handler';
export { SafeAreaView, SafeAreaProvider } from 'react-native-safe-area-context';
export { default as Slider } from '@react-native-community/slider';
export { default as Icon } from '@react-native-vector-icons/fontawesome6';

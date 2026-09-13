import React, {
  forwardRef,
  useImperativeHandle,
  useRef,
  useState,
} from 'react';
import {
  Modal,
  Platform,
  Pressable,
  ScrollView,
  StyleSheet,
  Text,
  View,
  type GestureResponderEvent,
  type StyleProp,
  type TextProps,
  type ViewProps,
  type ViewStyle,
} from 'react-native';
const glyphs: Record<string, number> = {
  backward: 0xf04a,
  forward: 0xf04e,
  play: 0xf04b,
  pause: 0xf04c,
};

export const GestureHandlerRootView = View;
export const SafeAreaView = View;
export const BottomSheetScrollView = ScrollView;
export const SafeAreaProvider = ({ children }: React.PropsWithChildren) => (
  <>{children}</>
);

export interface BottomSheetHandle {
  snapToIndex: (index: number) => void;
}

interface SheetProps extends React.PropsWithChildren {
  index?: number;
  snapPoints?: string[];
  enablePanDownToClose?: boolean;
  backgroundStyle?: StyleProp<ViewStyle>;
  handleIndicatorStyle?: StyleProp<ViewStyle>;
}

// Keep the example sheets on desktop without mobile-only gesture native modules.
export const BottomSheet = forwardRef<BottomSheetHandle, SheetProps>(
  ({ children, index = -1, backgroundStyle, handleIndicatorStyle }, ref) => {
    const [visible, setVisible] = useState(index >= 0);
    useImperativeHandle(
      ref,
      () => ({ snapToIndex: (value) => setVisible(value >= 0) }),
      []
    );
    return (
      <Modal
        transparent
        visible={visible}
        onRequestClose={() => setVisible(false)}
      >
        <View style={styles.overlay}>
          <Pressable
            accessibilityRole="button"
            accessibilityLabel="Close sheet"
            style={styles.backdrop}
            onPress={() => setVisible(false)}
          />
          <View style={[styles.sheet, backgroundStyle]}>
            <Pressable
              accessibilityLabel="Close sheet"
              onPress={() => setVisible(false)}
            >
              <View style={[styles.handle, handleIndicatorStyle]} />
            </Pressable>
            {children}
          </View>
        </View>
      </Modal>
    );
  }
);

interface SliderProps extends ViewProps {
  value: number;
  minimumValue: number;
  maximumValue: number;
  onSlidingComplete: (value: number) => void;
  tapToSeek?: boolean;
  thumbTintColor?: string;
  minimumTrackTintColor?: string;
  maximumTrackTintColor?: string;
}

// Use the same seek bar with pointer dragging and accessible step commands.
export const Slider = ({
  value,
  minimumValue,
  maximumValue,
  onSlidingComplete,
  style,
  thumbTintColor,
  minimumTrackTintColor,
  maximumTrackTintColor,
}: SliderProps) => {
  const width = useRef(1);
  const [dragged, setDragged] = useState<number>();
  const range = maximumValue - minimumValue;
  const displayed = dragged ?? value;
  const percent =
    range > 0
      ? Math.max(0, Math.min(1, (displayed - minimumValue) / range))
      : 0;
  const position = (event: GestureResponderEvent) =>
    minimumValue +
    Math.max(0, Math.min(1, event.nativeEvent.locationX / width.current)) *
      Math.max(0, range);
  const keyboardProps = {
    onKeyDown: (event: {
      nativeEvent: { key: string };
      preventDefault?: () => void;
    }) => {
      const key = event.nativeEvent.key;
      if (
        ![
          'ArrowLeft',
          'ArrowDown',
          'ArrowRight',
          'ArrowUp',
          'Home',
          'End',
        ].includes(key) ||
        range <= 0
      )
        return;
      event.preventDefault?.();
      const target =
        key === 'Home'
          ? minimumValue
          : key === 'End'
            ? maximumValue
            : value + (key === 'ArrowRight' || key === 'ArrowUp' ? 5 : -5);
      onSlidingComplete(Math.max(minimumValue, Math.min(maximumValue, target)));
    },
  };
  return (
    <View
      {...keyboardProps}
      focusable={range > 0}
      style={[styles.slider, style]}
      onLayout={(event) => {
        width.current = Math.max(1, event.nativeEvent.layout.width);
      }}
      accessibilityRole="adjustable"
      accessibilityLabel="Playback position"
      aria-valuemin={minimumValue}
      aria-valuemax={maximumValue}
      aria-valuenow={displayed}
      accessibilityValue={{
        min: minimumValue,
        max: maximumValue,
        now: displayed,
      }}
      accessibilityActions={[{ name: 'increment' }, { name: 'decrement' }]}
      onAccessibilityAction={(event) => {
        const offset = event.nativeEvent.actionName === 'increment' ? 5 : -5;
        onSlidingComplete(
          Math.max(minimumValue, Math.min(maximumValue, value + offset))
        );
      }}
      onStartShouldSetResponder={() => range > 0}
      onMoveShouldSetResponder={() => range > 0}
      onResponderGrant={(event) => setDragged(position(event))}
      onResponderMove={(event) => setDragged(position(event))}
      onResponderRelease={(event) => {
        onSlidingComplete(position(event));
        setDragged(undefined);
      }}
      onResponderTerminate={() => setDragged(undefined)}
    >
      <View
        pointerEvents="none"
        style={[styles.track, { backgroundColor: maximumTrackTintColor }]}
      >
        <View
          style={[
            styles.fill,
            {
              width: `${percent * 100}%`,
              backgroundColor: minimumTrackTintColor,
            },
          ]}
        />
        <View
          style={[
            styles.thumb,
            { left: `${percent * 100}%`, backgroundColor: thumbTintColor },
          ]}
        />
      </View>
    </View>
  );
};

interface SegmentedProps {
  appearance?: string;
  values: string[];
  selectedIndex: number;
  onChange: (event: { nativeEvent: { selectedSegmentIndex: number } }) => void;
}

export const SegmentedControl = ({
  values,
  selectedIndex,
  onChange,
}: SegmentedProps) => (
  <View style={styles.segments}>
    {values.map((value, index) => (
      <Pressable
        key={value}
        accessibilityRole="button"
        accessibilityLabel={value}
        accessibilityState={{ selected: index === selectedIndex }}
        style={[styles.segment, index === selectedIndex && styles.selected]}
        onPress={() =>
          onChange({ nativeEvent: { selectedSegmentIndex: index } })
        }
      >
        <Text style={styles.segmentText}>{value}</Text>
      </Pressable>
    ))}
  </View>
);

interface IconProps extends TextProps {
  name: string;
  size: number;
  color: string;
  iconStyle?: 'solid';
}

// The native project bundles the original Font Awesome font used on mobile.
export const Icon = ({
  name,
  size,
  color,
  iconStyle: _iconStyle,
  style,
  ...props
}: IconProps) => (
  <Text
    {...props}
    style={[
      {
        fontFamily: Platform.select({
          windows: '/Assets/FontAwesome6_Solid.ttf#FontAwesome6Free-Solid',
          default: 'FontAwesome6Free-Solid',
        }),
        fontSize: size,
        color,
      },
      style,
    ]}
  >
    {String.fromCodePoint((glyphs as Record<string, number>)[name] ?? 63)}
  </Text>
);

const styles = StyleSheet.create({
  overlay: { flex: 1, justifyContent: 'flex-end' },
  backdrop: {
    position: 'absolute',
    top: 0,
    bottom: 0,
    left: 0,
    right: 0,
    backgroundColor: '#00000066',
  },
  sheet: { height: '40%', borderTopLeftRadius: 16, borderTopRightRadius: 16 },
  handle: {
    width: 32,
    height: 4,
    borderRadius: 2,
    alignSelf: 'center',
    margin: 12,
  },
  slider: { justifyContent: 'center', alignItems: 'center' },
  track: { width: '100%', height: 4 },
  fill: { height: 4 },
  thumb: {
    position: 'absolute',
    top: -6,
    width: 16,
    height: 16,
    borderRadius: 8,
    marginLeft: -8,
  },
  segments: {
    flexDirection: 'row',
    backgroundColor: '#333333',
    borderRadius: 6,
    padding: 2,
  },
  segment: {
    flex: 1,
    paddingVertical: 6,
    paddingHorizontal: 12,
    alignItems: 'center',
    borderRadius: 4,
  },
  selected: { backgroundColor: '#666666' },
  segmentText: { color: 'white', fontWeight: '500' },
});

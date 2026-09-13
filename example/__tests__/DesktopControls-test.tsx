import React, { createRef } from 'react';
import { Modal, View } from 'react-native';
import { act, create, type ReactTestRenderer } from 'react-test-renderer';
import {
  BottomSheet,
  type BottomSheetHandle,
  SegmentedControl,
  Slider,
} from '../src/components/PlatformControls.desktop';

// Exercise the real desktop adapters without starting a native audio device.
describe('Desktop controls', () => {
  let tree: ReactTestRenderer;

  afterEach(async () => {
    if (tree) await act(async () => tree.unmount());
  });

  test('opens and dismisses the options sheet through its public handle', async () => {
    const ref = createRef<BottomSheetHandle>();
    await act(async () => {
      tree = create(
        <BottomSheet ref={ref}>
          <View />
        </BottomSheet>
      );
    });
    expect(tree.root.findByType(Modal).props.visible).toBe(false);
    await act(async () => ref.current?.snapToIndex(0));
    expect(tree.root.findByType(Modal).props.visible).toBe(true);
    await act(async () =>
      tree.root
        .findAllByProps({ accessibilityLabel: 'Close sheet' })[0]
        .props.onPress()
    );
    expect(tree.root.findByType(Modal).props.visible).toBe(false);
  });

  test('emits the same repeat-mode selection event as the native control', async () => {
    const onChange = jest.fn();
    await act(async () => {
      tree = create(
        <SegmentedControl
          values={['Off', 'Track', 'Queue']}
          selectedIndex={0}
          onChange={onChange}
        />
      );
    });
    await act(async () =>
      tree.root.findByProps({ accessibilityLabel: 'Queue' }).props.onPress()
    );
    expect(onChange).toHaveBeenCalledWith({
      nativeEvent: { selectedSegmentIndex: 2 },
    });
  });

  test('seeks using pointer position and clamps out-of-range drags', async () => {
    const seek = jest.fn();
    await act(async () => {
      tree = create(
        <Slider
          value={40}
          minimumValue={0}
          maximumValue={200}
          onSlidingComplete={seek}
        />
      );
    });
    const slider = tree.root.findByProps({ accessibilityRole: 'adjustable' });
    slider.props.onLayout({ nativeEvent: { layout: { width: 200 } } });
    await act(async () =>
      slider.props.onResponderRelease({ nativeEvent: { locationX: 150 } })
    );
    expect(seek).toHaveBeenLastCalledWith(150);
    await act(async () =>
      slider.props.onResponderRelease({ nativeEvent: { locationX: 500 } })
    );
    expect(seek).toHaveBeenLastCalledWith(200);
  });

  test('supports keyboard and accessibility seeking without exceeding duration', async () => {
    const seek = jest.fn();
    await act(async () => {
      tree = create(
        <Slider
          value={198}
          minimumValue={0}
          maximumValue={200}
          onSlidingComplete={seek}
        />
      );
    });
    const slider = tree.root.findByProps({ accessibilityRole: 'adjustable' });
    slider.props.onKeyDown({ nativeEvent: { key: 'ArrowRight' } });
    expect(seek).toHaveBeenLastCalledWith(200);
    slider.props.onKeyDown({ nativeEvent: { key: 'Home' } });
    expect(seek).toHaveBeenLastCalledWith(0);
    slider.props.onAccessibilityAction({
      nativeEvent: { actionName: 'decrement' },
    });
    expect(seek).toHaveBeenLastCalledWith(193);
  });
});

---
sidebar_position: 99
---

# Amazon FireOS Support

:::caution Device validation required
FireOS-specific builds have not been validated with this fork. A successful
Android build does not establish compatibility with every Fire tablet or TV.
:::

## Setup

The Android implementation uses AndroidX Media3. Keep the native dependencies
required by this fork and the application's React Native version. Do not
replace them with the old Amazon ExoPlayer 2.9 port.

Check the device's Android API level against the application's minimum SDK,
and test playback, codecs, background execution and remote controls on the
actual target. Fire TV navigation and a television user interface remain the
application's responsibility.

The original flavor-based ExoPlayer replacement instructions are retained in
the [v4.1 guide](../../versioned_docs/version-4.1/guides/amazon-fire-support.md)
for historical reference, not as setup instructions for this fork.

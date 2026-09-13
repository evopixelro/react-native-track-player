---
sidebar_position: 2
---

# Feedback

Controls the rendering of the control center item.

In this fork, macOS enables these commands through `UpdateOptions`. The iOS
types remain available, but its current capability mapping does not enable
feedback commands; see [Platform Support](../../basics/platform-support.md).

| Param          | Type                        | Description  |
| -------------- | --------------------------- | ------------ |
| isActive       | `boolean`                    | Marks wether the option should be marked as active or "done" |
| title          | `string`                     | The title to give the action |

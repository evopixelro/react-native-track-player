# PlaybackErrorEvent

An object denoting a playback error encountered during loading or playback of a
track.

| Property | Type     | Description |
|----------|----------|-------------|
| code     | `string` | A platform-specific error code. Do not assume codes or prefixes are shared across backends. |
| message  | `string` | The error message emitted by the native player. |

On Web, errors are exposed on `PlaybackState.error`, not through a separate
`PlaybackError` event. See [Events](../events.md).

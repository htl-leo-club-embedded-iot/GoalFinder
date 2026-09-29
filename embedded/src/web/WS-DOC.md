# WebSocket Messaging Reference

The GoalFinder device hosts a WebSocket server on port `81` (see `embedded/src/web/WebSocket.cpp`).

All messages are JSON objects carrying a `type` field. Every response and every outbound message also carries a `sourceType` field in the payload root with one of `"wa-no-auth"`, `"wa-auth"`, or `"hub"`.

## Status Legend

- Current: message is part of the active protocol.
- Outdated: leftover from the previous architecture in which the web app owned the game logic. The device now owns the game (see `GamePreset`, `PlayerSet`, `GameSession`, `GameManager`). Outdated messages are planned for removal as part of the legacy cleanup.
- Removed: legacy message that has been deleted from the device. Sending it now yields an `error` response with `permission_denied`.

## Access Model

| Source type | Applies when | May send writes |
|---|---|---|
| `wa-no-auth` | Connection is open, not authenticated | No |
| `wa-auth` | A successful `auth` was performed | Yes |
| `hub` | `identify` with `role: "hub"` after authentication | Yes |

Public (no authentication required): `ping`, `is_auth`, `auth`, `get_settings`, `game_get`.

All other actions require `wa-auth` or `hub`.

## Client to Device Actions

### auth

Authenticate the client using the SHA-256 hash of the device password. Responses are rate limited to 5 attempts per 60 seconds.

Request:

```json
{
  "type": "auth",
  "passwordHash": "5e2bf57d3f40c9b6f1e4c3a65d8c2b7a"
}
```

Response: `auth_result` with `success`, optional `error`, and optional `timeout`.

Example response (success):

```json
{
  "type": "auth_result",
  "success": true,
  "sourceType": "wa-auth"
}
```

Example response (invalid password):

```json
{
  "type": "auth_result",
  "success": false,
  "error": "Invalid password",
  "sourceType": "wa-no-auth"
}
```

Example response (rate limited):

```json
{
  "type": "auth_result",
  "success": false,
  "error": "Too many attempts. Please wait.",
  "timeout": true,
  "sourceType": "wa-no-auth"
}
```

Source: `WebSocket.cpp:790`. Status: current.

### is_auth

Query whether the device is password protected.

Request:

```json
{
  "type": "is_auth"
}
```

Response: `is_auth_result` with `isPasswordProtected`.

Example response:

```json
{
  "type": "is_auth_result",
  "isPasswordProtected": true,
  "sourceType": "wa-no-auth"
}
```

Source: `WebSocket.cpp:859`. Status: current.

### ping

Liveness check.

Request:

```json
{
  "type": "ping"
}
```

Response: `pong`.

Example response:

```json
{
  "type": "pong",
  "sourceType": "wa-no-auth"
}
```

Source: `WebSocket.cpp:867`. Status: current.

### get_settings

Request the full device settings document. The `data` object contains device name and password flags, detection, audio, LED, advanced, external network, DNS, and firmware version fields.

Request:

```json
{
  "type": "get_settings"
}
```

Response: `settings` with a `data` object.

Example response:

```json
{
  "type": "settings",
  "sourceType": "wa-auth",
  "data": {
    "deviceName": "GoalFinder",
    "devicePasswordSet": true,
    "wifiPasswordSet": false,
    "vibrationSensorSensitivity": 35,
    "ballHitDetectionDistance": 50,
    "distanceOnlyHitDetection": false,
    "volume": 70,
    "metronomeSound": 0,
    "hitSound": 0,
    "missSound": 0,
    "waitingSound": 0,
    "metSoundDelay": 200,
    "ledMode": 0,
    "ledBrightness": 50,
    "macAddress": "A4:CF:12:F8:2A:3C",
    "isSoundEnabled": false,
    "version": "0.6.0a",
    "afterHitTimeout": 2000,
    "advancedSettingsEnabled": false,
    "DNSEnabled": true,
    "extNW": false,
    "extNWSSID": "",
    "extNWPasswordSet": false,
    "extNWUseDHCP": true,
    "extNWIP": "",
    "extNWSNM": "",
    "extNWDFG": "",
    "extNWDNSIP": "",
    "extNWAuthMode": "none",
    "extNWEnterpriseIdentity": "",
    "extNWEnterpriseUsername": "",
    "extNWEnterpriseAnonymousIdentity": "",
    "extNWEnterprisePasswordSet": false,
    "extNWEnterprisePhase2Method": "",
    "extNWEnterpriseCaCertificate": "",
    "extNWEnterpriseClientCertificate": "",
    "extNWEnterpriseClientPrivateKey": ""
  }
}
```

Source: `WebSocket.cpp:278`. Status: current.

### set_settings

Write a single setting. Password-like fields accept an RSA-encrypted value.

Request:

```json
{
  "type": "set_settings",
  "key": "volume",
  "value": 70
}
```

Request with password field:

```json
{
  "type": "set_settings",
  "key": "devicePassword",
  "value": "<encrypted or plain value>"
}
```

Response: `setting_ack` with `key` and the applied `value`. Password fields do not echo the value back. An unknown `key` produces no response at all.

Example response:

```json
{
  "type": "setting_ack",
  "key": "volume",
  "value": 70,
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:349`. Status: current.

### restart

Reboot the device. No further messages are sent.

Request:

```json
{
  "type": "restart"
}
```

Response: `restarting`.

Example response:

```json
{
  "type": "restarting",
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:775`. Status: current.

### factory_reset

Reset all settings to their defaults.

Request:

```json
{
  "type": "factory_reset"
}
```

Response: `factory_resetting`.

Example response:

```json
{
  "type": "factory_resetting",
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:783`. Status: current.

### set_web_logging

Enable or disable relaying of device log messages over the WebSocket.

Request:

```json
{
  "type": "set_web_logging",
  "value": true
}
```

Response: `set_web_logging_ack` with `value`.

Example response:

```json
{
  "type": "set_web_logging_ack",
  "value": true,
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:880`. Status: current.

### identify

Declare the client role. Only the role `"hub"` is currently recognised; identifying as a hub requires prior authentication and sets the client source type to `"hub"`.

Request:

```json
{
  "type": "identify",
  "role": "hub"
}
```

Response: `identify_ack` with `role` and an optional `error`. On success the response `sourceType` is `"hub"` because the client role is applied before the reply is sent.

Example response (success):

```json
{
  "type": "identify_ack",
  "role": "hub",
  "sourceType": "hub"
}
```

Example response (unrecognised role):

```json
{
  "type": "identify_ack",
  "role": "unknown",
  "error": "Unrecognized role",
  "sourceType": "wa-auth"
}
```

Example response (not authenticated):

```json
{
  "type": "identify_ack",
  "error": "Not authenticated",
  "sourceType": "wa-no-auth"
}
```

Source: `WebSocket.cpp:968`. Status: current (hub groundwork).

### game_get

Request a full snapshot of the device-owned game. The `data` object contains the session state (`isRunning`, `mode`, `presetIndex`, `playerSetIndex`, `currentPlayer`, `playerCount`, `timer`, `timePerTurn`, `currentRound`, `maxRounds`), per-player scores (`players`), and the complete configuration (`presets`, `playerSets`).

Request:

```json
{
  "type": "game_get"
}
```

Response: `game_state` with a `data` object.

Example response:

```json
{
  "type": "game_state",
  "sourceType": "wa-auth",
  "data": {
    "isRunning": true,
    "mode": "timed_shots",
    "presetIndex": 1,
    "playerSetIndex": 0,
    "currentPlayer": 0,
    "playerCount": 3,
    "timer": 58,
    "timePerTurn": 60,
    "currentRound": 1,
    "maxRounds": 3,
    "players": [
      { "name": "Anna", "hits": 2, "misses": 0 },
      { "name": "Ben", "hits": 1, "misses": 1 },
      { "name": "Cleo", "hits": 0, "misses": 0 }
    ],
    "presets": {
      "free_play": [
        { "name": "Free Play 1", "rounds": 0, "timePerTurn": 0 },
        { "name": "Free Play 2", "rounds": 0, "timePerTurn": 0 },
        { "name": "Free Play 3", "rounds": 0, "timePerTurn": 0 },
        { "name": "Free Play 4", "rounds": 0, "timePerTurn": 0 }
      ],
      "timed_shots": [
        { "name": "Timed 1", "rounds": 0, "timePerTurn": 60 },
        { "name": "Timed 2", "rounds": 0, "timePerTurn": 90 },
        { "name": "Timed 3", "rounds": 0, "timePerTurn": 120 },
        { "name": "Timed 4", "rounds": 0, "timePerTurn": 180 }
      ],
      "board_hits": [
        { "name": "Board 1", "rounds": 3, "timePerTurn": 60 },
        { "name": "Board 2", "rounds": 5, "timePerTurn": 60 },
        { "name": "Board 3", "rounds": 10, "timePerTurn": 60 },
        { "name": "Board 4", "rounds": 15, "timePerTurn": 60 }
      ]
    },
    "playerSets": [
      {
        "name": "Set A",
        "players": ["Anna", "Ben", "Cleo"]
      }
    ]
  }
}
```

Source: `WebSocket.cpp:687`. Status: current (device-hosted game).

### game_set

Drive the device-owned game session. The action is one of `start`, `stop`, `save_presets`, or `save_player_sets`.

- Starting requires a `mode` key (`free_play`, `timed_shots`, or `board_hits`) plus preset and player set indices; indices are validated against the device configuration.
- `save_presets` replaces the presets for one or more modes. The `presets` object uses the same shape as the `game_state` document; only modes whose array is present are written. Names are truncated to 16 characters and arrays to `Settings::PRESETS_PER_MODE` entries.
- `save_player_sets` replaces the player sets. The `playerSets` array is clamped to `Settings::PLAYER_SET_COUNT` sets of up to `Settings::PLAYERS_PER_SET` players; names are truncated to 16 characters.

Request (start):

```json
{
  "type": "game_set",
  "data": {
    "action": "start",
    "mode": "timed_shots",
    "presetIndex": 1,
    "playerSetIndex": 0
  }
}
```

Request (stop):

```json
{
  "type": "game_set",
  "data": {
    "action": "stop"
  }
}
```

Request (save presets):

```json
{
  "type": "game_set",
  "data": {
    "action": "save_presets",
    "presets": {
      "free_play": [
        { "name": "Free Play 1", "rounds": 0, "timePerTurn": 0 }
      ],
      "timed_shots": [
        { "name": "Timed 1", "rounds": 3, "timePerTurn": 60 }
      ],
      "board_hits": [
        { "name": "Board 1", "rounds": 3, "timePerTurn": 60 }
      ]
    }
  }
}
```

Request (save player sets):

```json
{
  "type": "game_set",
  "data": {
    "action": "save_player_sets",
    "playerSets": [
      {
        "name": "Set A",
        "players": ["Anna", "Ben", "Cleo"]
      }
    ]
  }
}
```

Response: `game_ack` with `success` and optional `error`. On success a `game_state` broadcast follows.

Example response (success):

```json
{
  "type": "game_ack",
  "success": true,
  "sourceType": "wa-auth"
}
```

Example response (invalid parameters):

```json
{
  "type": "game_ack",
  "success": false,
  "error": "invalid_parameters",
  "sourceType": "wa-auth"
}
```

Example response (out of range):

```json
{
  "type": "game_ack",
  "success": false,
  "error": "out_of_range",
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:520`. Status: current (device-hosted game).

### get_game (Removed)

Removed legacy read of game data from the era when the web app simulated the game locally. Superseded by `game_get`.

Request:

```json
{
  "type": "get_game"
}
```

Response: `error` with `permission_denied`; the message type is no longer recognised.

Source: removed (was `WebSocket.cpp:357`). Status: removed.

### set_game (Removed)

Removed legacy write of `presets`, `playerSets`, and `isSoundEnabled` from the era when the web app owned the game configuration. Presets and player sets are now written through `game_set` (`save_presets` / `save_player_sets`); `isSoundEnabled` is written through `set_settings`.

Request:

```json
{
  "type": "set_game",
  "data": {
    "isSoundEnabled": true,
    "presets": {
      "free_play": [
        { "name": "Free Play 1", "rounds": 0, "timePerTurn": 0 }
      ],
      "timed_shots": [
        { "name": "Timed 1", "rounds": 3, "timePerTurn": 60 }
      ],
      "board_hits": [
        { "name": "Board 1", "rounds": 3, "timePerTurn": 60 }
      ]
    },
    "playerSets": [
      {
        "name": "Set A",
        "players": ["Anna", "Ben", "Cleo"]
      }
    ]
  }
}
```

Response: `error` with `permission_denied`; the message type is no longer recognised.

Source: removed (was `WebSocket.cpp:362`). Status: removed.

### start (Removed)

Removed legacy begin-detection action. It enabled sound and detection without creating a game session, from the era when the web app ran the entire game in the browser and only required the device for sensing and audio. Sessions now begin via `game_set` (`action: start`).

Request:

```json
{
  "type": "start"
}
```

Response: `error` with `permission_denied`; the message type is no longer recognised.

Source: removed (was `WebSocket.cpp:641`). Status: removed.

### stop (Removed)

Removed legacy end-detection action. It disabled sound and detection without ending a game session, same era as `start`. Sessions now end via `game_set` (`action: stop`).

Request:

```json
{
  "type": "stop"
}
```

Response: `error` with `permission_denied`; the message type is no longer recognised.

Source: removed (was `WebSocket.cpp:650`). Status: removed.

## Device to Client Pushes

These messages are not triggered by a request; the device sends them on its own. Each push carries the `sourceType` of the receiving client.

### event

Sent on every sensor detection. `event` is either `"hit"` or `"miss"`. This is transitional: it previously served as the web app's scoring input, but scores are now authoritative in `game_state`, so the event is informational only.

```json
{
  "type": "event",
  "event": "hit",
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:268`.

### game_state

Broadcast after a session change. The document is identical in shape to the `game_get` response.

```json
{
  "type": "game_state",
  "sourceType": "wa-auth",
  "data": {
    "isRunning": true,
    "mode": "timed_shots",
    "presetIndex": 1,
    "playerSetIndex": 0,
    "currentPlayer": 0,
    "playerCount": 3,
    "timer": 58,
    "timePerTurn": 60,
    "currentRound": 1,
    "maxRounds": 3,
    "players": [
      { "name": "Anna", "hits": 2, "misses": 0 },
      { "name": "Ben", "hits": 1, "misses": 1 },
      { "name": "Cleo", "hits": 0, "misses": 0 }
    ],
    "presets": {
      "free_play": [
        { "name": "Free Play 1", "rounds": 0, "timePerTurn": 0 },
        { "name": "Free Play 2", "rounds": 0, "timePerTurn": 0 },
        { "name": "Free Play 3", "rounds": 0, "timePerTurn": 0 },
        { "name": "Free Play 4", "rounds": 0, "timePerTurn": 0 }
      ],
      "timed_shots": [
        { "name": "Timed 1", "rounds": 0, "timePerTurn": 60 },
        { "name": "Timed 2", "rounds": 0, "timePerTurn": 90 },
        { "name": "Timed 3", "rounds": 0, "timePerTurn": 120 },
        { "name": "Timed 4", "rounds": 0, "timePerTurn": 180 }
      ],
      "board_hits": [
        { "name": "Board 1", "rounds": 3, "timePerTurn": 60 },
        { "name": "Board 2", "rounds": 5, "timePerTurn": 60 },
        { "name": "Board 3", "rounds": 10, "timePerTurn": 60 },
        { "name": "Board 4", "rounds": 15, "timePerTurn": 60 }
      ]
    },
    "playerSets": [
      {
        "name": "Set A",
        "players": ["Anna", "Ben", "Cleo"]
      }
    ]
  }
}
```

Source: `WebSocket.cpp:744`.

### log

A relayed device log line, sent only while web logging is enabled.

```json
{
  "type": "log",
  "message": "Setting 'volume' updated",
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:511`.

### error

Sent for failed requests, such as a permission denial. The offending message type is echoed.

```json
{
  "type": "error",
  "error": "permission_denied",
  "messageType": "set_settings",
  "sourceType": "wa-auth"
}
```

Source: `WebSocket.cpp:959`.

## Summary of Removed Actions

The following legacy messages were removed from the device; sending them yields `permission_denied`. The web app must stop using them (see the web-side migration issue).

| Removed message | Replaced by |
|---|---|
| `get_game` | `game_get` |
| `set_game` | `game_set` (`save_presets` / `save_player_sets`) |
| `start` | `game_set` (`action: start`) |
| `stop` | `game_set` (`action: stop`) |

The `event` hit/miss push remains current, but its role changes from scoring input to notification.
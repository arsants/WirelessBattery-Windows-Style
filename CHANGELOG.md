# Changelog

## 1.0.4.4 — Windows Style · 2026-11-20

First public release of this independent fork, based on [upstream v1.0.4](https://github.com/krisgfx9/Wireless-Device-Battery-Monitor/releases/tag/v1.0.4).

### Added

- Lofree Flow Lite84 2.4 GHz identification, online and battery protocol, including charging and voltage.
- AJAZZ AK820 Pro receiver handshake/battery queries, USB battery-marker handling and Bluetooth identity normalization.
- A stable keyboard identity across transports, preferring a live Bluetooth battery reading over an unknown receiver entry.
- A custom vector gamepad icon with a four-bar D-pad and one large ring.
- Keyboard and display-state regression tests, hardware diagnostics and a production widget preview renderer.
- Russian/English fork documentation, hardware verification table, upstream attribution and release packaging.

### Changed

- Softer foreground colors in both taskbar themes.
- Supersampled grayscale font masks for smoother small icons and text.
- A missed battery response no longer forces an `asleep` state or dims an otherwise known value.

### Known limitations

- The tested AJAZZ AK820 Pro receiver does not supply a usable numeric battery reading. Use Bluetooth for a percentage on that unit.
- The special AJAZZ USB `FF` marker is shown as `USB`; it is not treated as a measured 100%.
- Sleeping Lofree devices may require a keypress and refresh; cached readings are explicitly marked `last known`.
- Other device protocols and the limitations documented in the archived upstream README remain applicable.

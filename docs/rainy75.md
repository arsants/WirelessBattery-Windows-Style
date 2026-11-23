# Rainy 75 Pro investigation

Branch `feat/rainy-keyboard` starts at `feat/wlmouse-beast-x` (`de1c2cc`).

The user's Rainy 75 Pro receiver enumerates as `320F:5088`, product
`Wireless Gaming Keyboard`. Its vendor collection is interface 2,
usage `FF1C:0092`, input/output report lengths 64. These IDs and the generic
product string are shared with other Evision keyboards; they do not identify
Rainy 75 Pro on their own. The expected cable PID is `320F:5055`.

The existing application has no driver for this protocol.

## Protocol reference (active queries disabled)

[Rainy75Desktop battery implementation](https://github.com/zzixxxx/Rainy75Desktop/blob/main/battery.py)
documents output report `04 20 00 1A 06`, zero-padded to 64 bytes.
Bytes 1–2 are a payload checksum, not a sequence number. A response to command
`1A` may contain a battery percentage at byte 8 (including report ID).
The interpretation of byte 9 as charging is not hardware-verified.

That project reports responses delayed by 5–12 seconds and warns that sending
requests during typing can interfere with key release reports. Input-idle gating did not prevent an input-repeat incident on this user's device.
Do not treat a battery query as harmless merely because it does not change settings.

## Results on this device

- Receiver discovered and opened successfully.
- No battery reply during a 15-second window with the documented request,
  including a check after the user woke the keyboard.
- The user confirmed unintended repeated keyboard input following these probes
  and had to reboot the PC. No clipboard or input-injection APIs were used by
  the diagnostic. The exact firmware failure mechanism is not established.
  Active Rainy queries have been removed; do not run more hardware probes.
- Cable-mode testing is cancelled following the input incident. Rainy battery
  support was left unimplemented. Live probing was disabled after the incident;
  see the later, explicitly authorized limited retry below.
- The installed WLMouse build is unchanged.

## Diagnostic

`tests/evision_status.cpp` only enumerates device descriptors. It contains no
HID battery writes, reads, or polling loops. All command-line arguments,
including the former `--battery` and `--receiver`, are rejected before enumeration.
The previous diagnostic executable has been quarantined locally and must not
be used. This utility is not included in the installed application or autostart.

## Offline review after the incident

The user confirms no drivers were installed and the keyboard works out of the
box. Exact firmware version is unknown; stock VIA behavior is a working
hypothesis, not a firmware identification.

Official files were downloaded for static inspection only. No scripts or firmware
were executed and no HID queries were made during this review.

- [WOBKEY firmware/configuration page](https://wiki.wobkey.com/en/products/rainy-75/driver-firmware)
  links the RGB Standard/Pro VIA JSONs for USB and receiver mode.
- [USB JSON](https://drive.wobkey.com/f/d/XNltG/Rainy-75-RGB-USB-3.0.JSON)
  identifies `320F:5055`;
  [receiver JSON](https://drive.wobkey.com/f/d/o29f2/Rainy-75-RGB-2.4G-3.0.JSON)
  identifies `320F:5088`. Both expose lighting controls in `menus` and a
  `BATTERY STATUS` custom keycode. Neither defines an application battery
  percentage field. A custom keycode is not a documented host battery-read API.
- [WOB legacy driver](https://www.wobwxe.com/legacy/index.html), module 126
  (`Rainy75.js`), contains `GetBatteryInformationCom` with payload `[32,0,26,6]`.
  Its device entry is `320F:5055`, usage `FF1C:0092`, report 4. The receiver
  `320F:5088` is absent from this driver's device filter.
- This module's `handleAllData` does not return a battery percentage or charging
  property. The presence of a request constant does not establish that the
  user's stock receiver implements it, or establish a valid reply format.

Review of our initial probe also found a concrete error: two packets used
`20 00 1A 00` and `26 00 1A 06` after report ID 4. The first two bytes were
incorrectly treated as a sequence, despite representing a checksum. Their
expected checksum values would have been `1A 00` and `20 00`, respectively.
Later probes used the referenced `20 00 1A 06` payload but still got no reply.
Malformed requests are a possible contributor to the input incident; neither
they nor the precise firmware failure mechanism have been established as the
sole cause. All live Rainy queries remain disabled.

### Evidence needed before implementing a battery driver

A verified response/capture or firmware implementation for the stock Rainy 75 Pro
receiver is still required. Do not infer percentages from echoed requests,
lighting values, shared device IDs, or another model's protocol. Continue with
existing public captures/source analysis; do not restore live probing on this PC.

### Download hashes

These identify the exact public files inspected; the downloaded assets are kept
under ignored `research/rainy-offline/`, not shipped with the application.

- `usb.json`: `9c7dbf389a295254f140a6da5dfdd2dd507f11b2be30fa9e163dd0246b7ca3ac`
- `receiver.json`: `5aa6a9581aab568ec037b63a360e2038851804693572050d407c8d2e17ceadcc`
- `legacy-app~918758ea.7850b4b2.js`: `0d6b75a7bfd70f82f14f8257b27c47590546a85931995f438be7e877d7db237c`
- `legacy-app~5c551db8.a4721159.js`: `d6e20b08b08f4d189b4646cbd8c299978b5a78c0f79a106388eb08199d8c2003`
- `legacy-app~648002f7.429d224b.js`: `6d16e54344b03d0eb6719d64a10bddddb510d5fb9d74c65c80ede69325eb1b7a`

### Independent stock-firmware research

The [Rainy 75 Pro reverse-engineering report](https://github.com/scholzri/rainy75-zmk/blob/cb4b7edeb9f8a6d4d81b48ddfb3b145a0a97cd5d/docs/protocols.md#hid-command-probe-results)
reports a silent `FF1C` channel on its stock unit and distinguishes this firmware
from the protocols exposed in WOB's driver. This is evidence from another unit,
not an identification of the user's firmware. It supports avoiding an assumption
that shared VID/PID and a request constant imply a functioning battery API.
No existing verified battery response for the user's receiver has been obtained.

## Bluetooth implementation

Windows now reports the user's connected `Rainy 75-1` device and battery 91%
through its existing Bluetooth PnP properties. No vendor HID query is involved.
The generic Bluetooth driver normalizes supported Rainy 75 slot names to
`WOBKEY Rainy 75` and sets the keyboard type. The model name does not claim Pro,
since Rainy 75 Standard/Pro share these Bluetooth names.

The existing Windows container key is preserved, including per-device visibility
settings. Distinct containers are not merged. Existing connection filtering
removes disconnected devices; no receiver grace period is applied to Bluetooth.
Name/identity tests and a display-state disconnect/reconnect test cover this
path. `tests/bluetooth_status.cpp` reads Windows properties only.

The 2.4 GHz battery protocol remains unresolved and active probes remain disabled.

## Explicitly authorized limited receiver retry

The user subsequently authorized a careful retry, with Bluetooth as fallback.
This was a separate, isolated experiment, not a restoration of application polling.

- Windows reported the Rainy Bluetooth link disconnected before the receiver test.
- A 15-second listen opened only `320F:5088`, interface 2, vendor collection
  `FF1C:0092`, using `GENERIC_READ`. It sent no commands and received no vendor
  report 4 messages. No keyboard report payloads were recorded.
- Descriptor inspection verified input/output report 4, both 64 bytes. Windows
  describes both as button arrays on vendor page `FF1C`, usage range `00..FF`
  (`BitField=0`); they are not `HIDP_VALUE_CAPS` entries.
- Chromium's current `HidConnectionWin::PlatformWrite` uses `WriteFile`, matching
  the application's transport. It does not use `HidD_SetOutputReport`.
  [Chromium source](https://chromium.googlesource.com/chromium/src/+/main/services/device/hid/hid_connection_win.cc)
- The isolated sender checked idle input, no held keys, unambiguous receiver
  collection and report capabilities, waited five seconds, then transmitted
  exactly one 64-byte output report: `04 20 00 1A 06`, zero padded.
- It listened for 15 seconds. No valid battery reply arrived. No retransmits,
  handshakes, settings writes, or alternative commands were attempted.
- The sender closed and its executable was quarantined after the run. The main
  application and its autostart were not modified.

Keyboard input health still needs user confirmation; no lack-of-failure claim
is inferred from a successful host write. Cable-mode descriptor inspection is
the next requested user-assisted step. A USB battery response would help assess
compatibility with WOB's declared wired protocol, but has not been obtained.

## Cable enumeration confirmed

After the user switched modes, the cable enumerated as `320F:5055`, product
`Rainy 75`. Its `FF1C:0092` vendor collection is on interface 3 and declares
8-byte input reports and 64-byte output reports. The receiver remains present
separately as `320F:5088`, interface 2, with 64-byte input/output reports.

The referenced Evision parser expects at least 10 bytes (percentage at offset 8
and state at offset 9, including report ID). It cannot decode that layout from
the cable's declared 8-byte input report. This rules out a direct reuse of that
parser on this cable interface; it does not prove that all other commands or
fragmented formats are impossible. No battery packet was sent to the cable.

A descriptor-only inspector confirms report ID 4 on the cable's vendor input
and output arrays. No input-report capture or output-report writes were used
for this step. The installed application remains the working Bluetooth build.

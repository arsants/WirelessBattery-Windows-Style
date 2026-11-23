# Rainy 75 Pro investigation

Branch `feat/rainy-keyboard` starts at `feat/wlmouse-beast-x` (`de1c2cc`).

The user's Rainy 75 Pro receiver enumerates as `320F:5088`, product
`Wireless Gaming Keyboard`. Its vendor collection is interface 2,
usage `FF1C:0092`, input/output report lengths 64. These IDs and the generic
product string are shared with other Evision keyboards; they do not identify
Rainy 75 Pro on their own. The expected cable PID is `320F:5055`.

The existing application has no driver for this protocol.

## Read-only protocol reference

[Rainy75Desktop battery implementation](https://github.com/zzixxxx/Rainy75Desktop/blob/main/battery.py)
documents output report `04 20 00 1A 06`, zero-padded to 64 bytes.
Bytes 1–2 are a payload checksum, not a sequence number. A response to command
`1A` may contain a battery percentage at byte 8 (including report ID).
The interpretation of byte 9 as charging is not hardware-verified.

That project reports responses delayed by 5–12 seconds and warns that sending
requests during typing can interfere with key release reports. Input-idle gating
reduces that risk but does not prove polling is safe for every firmware.

## Results on this device

- Receiver discovered and opened successfully.
- No battery reply during a 15-second window with the documented request,
  including a check after the user woke the keyboard.
- A later user message contained repeated text. Whether this was deliberate
  typing or an input problem has not yet been confirmed. All queries stopped.
- Cable-mode verification is pending. Do not advertise working Rainy battery
  support or integrate background receiver polling without a valid response
  and input-behavior verification.
- The installed WLMouse build is unchanged.

## Diagnostic

`tests/evision_status.cpp` enumerates only by default. Compile with the existing
MinGW-compatible toolchain and `src/hid.cpp`, linking `hid` and `setupapi`.
`--battery` sends one documented battery query on the cable, after checking
input idle time. It listens for at most 15 seconds and closes the handle.
Receiver queries additionally require `--receiver`; there are no retransmits.
No firmware, keymap, lighting, or pairing commands are sent.

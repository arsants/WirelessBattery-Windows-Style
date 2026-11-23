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
  support remains unimplemented. Future research must be offline, using existing
  captures or source analysis; no further live keyboard queries are authorized.
- The installed WLMouse build is unchanged.

## Diagnostic

`tests/evision_status.cpp` only enumerates device descriptors. It contains no
HID battery writes, reads, or polling loops. All command-line arguments,
including the former `--battery` and `--receiver`, are rejected before enumeration.
The previous diagnostic executable has been quarantined locally and must not
be used. This utility is not included in the installed application or autostart.

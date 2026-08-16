# Loopy My Seal Emulator

## Prerequisites

LoopyMSE requires a Loopy BIOS and, for sound emulation, the Loopy sound BIOS is required. It expects the files to be named bios.bin and soundbios.bin, and looks for them:

- Where the .app or .exe lives
- `~/Library/Application Support/LoopyMSE/LoopyMSE/` on MacOS
- Next to the game you're launching

Or you can provide a different name or an absolute path in your `loopymse.ini` configuration or on the command line.

## Configuration

Find the configuration `loopymse.ini` either:

- Next to the .exe on Windows
- In `~/Library/Application Support/LoopyMSE/LoopyMSE/` on MacOS (after first run)

You can use the settings here to change your keyboard or controller bindings as well as certain options.

## Running

You can run LoopyMSE several ways:

- Opening the .exe or .app. If the BIOS can be found, you'll get an empty window, into which you can drag a Loopy ROM to play. If nothing happens, it may have failed to find the BIOS.
- Dragging a Loopy ROM onto the .exe or .app. You can pin the app to your Dock / Taskbar.
- On MacOS, the .loopy file extension will be associated with LoopyMSE and you can open Loopy ROMs in LoopyMSE by renaming them .loopy and simply opening them.
- On Windows, you can associate the .loopy file extension with LoopyMSE.
- It can be launched via the command line. On MacOS and Linux, you can get logging this way.

Launching via commandline:
- On MacOS, the binary is in `LoopyMSE.app/Contents/MacOS/LoopyMSE`.
- Use `LoopyMSE --help` for information on commandline parameters.

## Playing Loopy My Seal Emulator

You can use the keyboard or a gamepad / controller to emulate controller input.

The Loopy Mouse is also emulated. Press F2 (or set `mouse=true` in `loopymse.ini`) to plug it in; as on real hardware, the gamepad is disconnected while the mouse is plugged in. Click the window to capture your mouse and start playing, and press F2 again to release it. Press F1 to plug the gamepad back in. Games that only check for the mouse at startup may need a reboot (F12) after plugging it in.

Default controls (remap in `loopymse.ini`):

| Loopy | Keyboard |
| ----- | -------- |
| A     | Z        |
| B     | X        |
| C     | C        |
| D     | V        |
| L     | Q        |
| R     | W        |
| Start | Enter    |

Additionally, these special functions are available.

| Function        | Key          |
| --------------- | ------------ |
| Plug in gamepad | F1           |
| Plug in mouse   | F2           |
| Capture mouse   | Click window |
| Release mouse   | F2           |
| Screenshot      | F10          |
| Fullscreen      | F11          |
| Reboot          | F12          |
| Exit            | Esc          |

Screenshots are saved in the same directory as the loaded ROM, or in the same directory as `loopymse.ini` if the ROM directory is not available for some reason. By default, both raw unscaled and aspect ratio corrected images are exported.

## Printing

LoopyMSE has basic printer emulation for the most common types of seals. When a game tries to print a supported type, it will be saved as an image.
The location and file name of the saved image are similar to screenshots, but prefixed with `print_`.
On supported systems, the image is automatically opened with the associated application after printing.

If an image file can't be created, LoopyMSE reports a general printing failure to the game, and the game should handle it appropriately.
A general failure is also reported if a game tries to print an unsupported seal type.
Printing is implemented at a high level by interpreting data sent to the BIOS, so the supported types depend on currently understood data formats.

## MacOS Security

LoopyMSE is not signed or notarized, so you will only be able to run it if you "Allow Applications From App Store & Known Developers" in System Preferences > Privacy & Security.

If the system says "'LoopyMSE' is damaged and can't be opened. You should move it to Trash." it's being flagged as an untrusted, un-signed download. To override this you must clear the quarantine by running `xattr -rd com.apple.quarantine LoopyMSE.app`.

If the system says "Apple could not verify 'LoopyMSE.app' is free of malware that may harm your Mac or compromise your privacy.", click "Done", open System Preferences > Privacy & Security and click "Open Anyway" and then "Open Anyway" again.

**Please, only do the above steps if you know what you are doing, and you trust this executable.**

## Wanwan Expansion Audio

Currently, to emulate expansion PCM audio on Wanwan Aijou Monogatari, place numbered .wav files in a `pcm/` directory next to the Wanwan ROM.
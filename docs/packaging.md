# Linux AppImage and Android APK

Build outputs are development packages. They use LunarNX's own Xbox and
PlayStation engines and one native Borealis interface. No separate clients
need to be installed.

## Download

Open the successful GitHub Actions run and download its artifact:

- Linux: **LunarNX-Linux-AppImage-x86_64**
- Android: **LunarNX-Android-ARM64-APK**

Each artifact includes the matching application and prepared dependency source archive.
The binary and sources identify the same workflow commit. Artifacts have the
retention period shown by GitHub Actions; these are not published releases.

## Linux

Extract the artifact, mark LunarNX-Linux-x86_64.AppImage executable, and launch:

```sh
chmod +x LunarNX-Linux-x86_64.AppImage
./LunarNX-Linux-x86_64.AppImage
```

The build uses Ubuntu 24.04's glibc baseline (2.39). The AppImage bundles
application libraries and resources, but uses the host kernel and graphics
driver. If FUSE is unavailable, use --appimage-extract-and-run.

Build with the instructions in linux-development.md, then run:

```sh
python3 scripts/package_linux_appimage.py
python3 scripts/archive_port_sources.py build/packages/LunarNX-Linux-corresponding-source.tar.gz
```

The launcher preserves TLS certificate verification and selects the host CA
bundle, with a bundled CA snapshot as fallback. Configuration remains in the
user's XDG directories.

## Android

Install LunarNX-Android-arm64-debug.apk on an ARM64 device running Android 10
(API 29) or newer with OpenGL ES 3.0. This is a debug-signed development APK.
CI signs development APKs with a stable key kept in the repository secrets
`ANDROID_KEYSTORE_BASE64` and `ANDROID_KEYSTORE_PASSWORD` (PKCS12, alias
`lunarnx`, `android/app/build.gradle` signing config `lunarDevelopment`, enabled
by `LUNARNX_KEYSTORE_PATH`). Builds signed with the same key update in place and
keep accounts/configuration. The first APK signed with this key still requires
one uninstall of any APK signed with an earlier random debug key. Builds
without the secrets (forks, local Gradle) use Gradle's debug key and may
require an uninstall. Uninstalling removes the app's private account and
configuration files. The key is for development only: never commit it; keep a
private backup (losing it forces one more uninstall) and rotate it by
replacing both secrets. The CI log prints the certificate digest
(`apksigner --print-certs`) so a build can be compared across runs.

The app packages its interface, resources, SDL, FFmpeg, Opus, legacy libpeer
and Chiaki. Configuration, accounts and cached files stay in Android's private
application storage. Android's locale is used on first launch. Phone helper
login flows and physical controller mappings are shared with Linux.

To build, use Linux, JDK 17, Gradle 8.11.1, Android SDK 35,
build-tools 35.0.0, CMake 3.22.1 and NDK 28.2.13676358. Install the host tools
listed in .github/workflows/android-port.yml, then:

```sh
python3 scripts/prepare_linux_dependencies.py
python3 scripts/prepare_linux_ui.py
python3 scripts/prepare_linux_chiaki.py
python3 scripts/prepare_android_dependencies.py
export ANDROID_NDK_ROOT=/path/to/android/sdk/ndk/28.2.13676358
bash scripts/build_android_dependencies.sh
export PKG_CONFIG_LIBDIR="$PWD/build/android-prefix/lib/pkgconfig"
export PKG_CONFIG_PATH=""
gradle -p android --no-daemon assembleDebug
```

Output: android/app/build/outputs/apk/debug/app-debug.apk. The GitHub workflow
also verifies its signature, ARM64 native libraries, SDL entry point, resources
and 16 KiB zip alignment.

## Validation limits

Linux CI exercises settings, controller mapping/rumble, local pairing/PSN
callback services, Xbox mock streaming and PlayStation H.264/HEVC replay in one
window. The packaged AppImage repeats navigation and PlayStation replay tests.

The Android checks establish compilation, linking and APK packaging. They do
not establish successful startup or console streaming on a physical tablet.
Both builds attempt native acceleration with software recovery as described
below. Real account/console sessions, physical controls, suspend/resume and
Android performance still need device testing. On-screen controls are implemented
as described below; actual hardware acceleration/performance validation remains pending.

## Native video acceleration

Linux attempts VA-API and Android attempts FFmpeg's NDK MediaCodec decoders
when the first random-access H.264/HEVC frame arrives. Android dependencies
include both MediaCodec decoders and request the optional low-latency mode.
Decoding uses the platform codec; presentation still copies to CPU RGBA and
uploads through the existing unified interface. This is not zero-copy rendering.
The Switch NVDEC renderer is unchanged.

The performance overlay shows VA-API, MediaCodec, or SW only after that decoder
produces an image. MediaCodec identifies the Android decoding service; whether
a vendor hardware codec is selected must be confirmed on the device.
Private application logs record accelerator opening and fallback events.
On Linux the log is `~/.cache/lunarnx/lunarnx.log` (or under `$XDG_CACHE_HOME`);
search for `[video-decode]`. `accelerator-attempt` followed by
`accelerator-opened` means VA-API was opened; `selected=software reason=...`
states why it was not (including the FFmpeg error text when opening fails).

If opening fails, software continues with the original random-access frame.
A runtime codec/transfer error or 120 input access units without an output
switches to software and lets the normal keyframe recovery path resume.
Acceleration is attempted once per session. Linux can explicitly use CPU mode
with `LUNARNX_VIDEO_DECODE=software` or select a render device with
`LUNARNX_VAAPI_DEVICE=/dev/dri/renderD128`. VA-API requires compatible host
drivers and permission to access the render device. CI without a GPU verifies
fallback and CPU replay, not actual hardware performance.

## PlayStation touchscreen touchpad (Linux and Android)

With the touch gamepad hidden during a PlayStation stream, the touchscreen sends up to two independent
contacts to the remote PS4/PS5 touchpad, including stationary contacts and small
movements. Coordinates use the entire application window and are mapped to the
console touchpad dimensions. Swipe to send a touchpad gesture; briefly tap to
click, or hold one stationary finger for 500 ms to hold the touchpad button.
Lifting the finger releases the contact, including during the short click pulse.
Two-finger gestures do not synthesize a click.

The rightmost 96 pixels of the 1280-wide logical UI are reserved for the
leftward swipe that opens the stream menu. Start game gestures outside that
strip. Menu/pause input is released and held fingers must lift before game
input resumes. A connected controller's physical touchpad and its click button
remain supported. Linux requires a touchscreen driver exposing SDL touch
contacts; mouse input is not interpreted as a touchpad contact.

The portable native_touchpad regression covers gesture and input lifecycle
behavior shared by both builds. Real touchscreen-to-console operation still
requires testing on Linux hardware and an Android device.

## Selecting decoding and showing its status

On Linux and Android, open the Xbox or PlayStation settings and select
**Video decoder** independently for that platform:

- **Automatic (hardware with CPU fallback)**: try VA-API on Linux or MediaCodec
  on Android, retaining CPU recovery if acceleration cannot open or fails.
- **Software**: decode on the CPU without trying acceleration.

Save the settings and start a new session. Changing the preference during a
stream takes effect on the next session. Xbox Remote Play and Cloud share the
Xbox preference; PlayStation keeps its own. Older configuration files default
to Automatic. `LUNARNX_VIDEO_DECODE=software` remains a CPU-only diagnostic override.

Enable **Show active decoder** in general settings or in the in-game menu to
show a small indicator without the full performance panel. This shared setting
is saved for Xbox and PlayStation and defaults to hidden. It reads `...` before
the first decoded frame, then VA-API, MediaCodec or SW according to actual frame
output; a fallback updates the indicator to SW. MediaCodec can select a software
codec on some devices, so its label alone does not prove physical acceleration.

For a useful comparison, keep resolution, bitrate, console, network and game
scene unchanged, and compare Automatic with Software across separate sessions.

## Touch gamepad (Linux and Android)

Open Xbox or PlayStation settings, then the controller section. **Touch gamepad**
shows/hides the on-screen controls. **Touch gamepad opacity** offers 10%, 25%,
50%, 75% and 100% (higher means more visible). Each platform saves its own
visibility and opacity; defaults are hidden and 50%.

While streaming, tap **...** at the upper right, open Stream settings, change
these preferences, save and return to the game. Visibility and opacity take
effect immediately on returning; no stream restart is required. The app menu
remains accessible when the gamepad is hidden. The existing edge swipe also works.

Controls include both analog sticks, their L3/R3 (LS/RS) clicks, D-pad,
face buttons, L1/R1/L2/R2 (LB/RB/LT/RT), Share/Create + Options (View + Menu),
and PS/Xbox Home. PlayStation also includes a touchpad-click button and a
central touchpad rectangle with up to two contacts, tap-click and hold-click.
The dedicated Xbox capture button and DualSense microphone/mute function are
not available through the current input protocol/app audio path.

With the gamepad visible, only contacts beginning inside the PS touchpad
rectangle become touchpad gestures. Movement is scaled to the entire console
pad and clamped at its boundary. A finger starting on a stick, button or video
cannot enter the pad until it lifts. Multiple fingers can use sticks, buttons
and the pad simultaneously. Hiding the gamepad restores fullscreen PS touchpad
behavior, including the existing menu-edge reservation. The physical controller
and its touchpad continue to work alongside on-screen controls.

Opening menus, changing mode, losing window focus or ending a session releases
virtual input; fingers held across the transition must lift before they can
control the game again. A touchscreen exposing SDL touch events is required;
mouse clicks are not synthesized as gamepad contacts. This feature is native
Linux/Android only; Switch retains its existing HID/touchpad path.

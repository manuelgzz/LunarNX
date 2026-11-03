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
CI uses a fresh debug signing key; future development builds may require an
uninstall before installing a differently signed APK. Uninstalling removes
the app's private account/configuration files.

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
Android performance still need device testing. Configurable on-screen gamepad
buttons and actual hardware acceleration/performance validation remain pending.

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

During a PlayStation stream, the touchscreen sends up to two independent
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

# Native Linux and Android direction

## Target

One application with its own UI and linked Xbox/PlayStation engines. Preserve
the LunarNX platform selector and independent per-platform configuration.
Do not launch Chiaki-ng or XStreamingDesktop as external applications and do not
embed their windows. Use the existing LunarNX controllers and media pipeline.

The user's existing fork is manuelgzz/LunarNX. The Python Dualstream launcher
remains a separate implementation until an explicit migration decision.

## Verified starting point

- Makefile.desktop is a macOS test build using xcrun, Homebrew and Apple frameworks.
- CMake's desktop target uses src/platform/desktop_main.cpp, a console-oriented
  Xbox test runner; it does not include the full Borealis UI.
- The production UI entry point in src/main.cpp is guarded by __SWITCH__.
- The repository has no Android Gradle project or APK packaging pipeline.
- The upstream README describes real-console validation on Switch, not Linux
  or Android. Hardware decoding claims do not carry over automatically.

## This first change

Makefile.linux reuses the maintained desktop source list but overrides the Apple
toolchain flags, library discovery, interface-name filter and output directory.
It does not fetch dependencies or change the active legacy libpeer provider.
It requires the prepared, pinned legacy dependency and native Linux libraries.

The isolated tests/portable CMake project compiles existing shared policy tests
without libnx, SDL, credentials or network services. GitHub Actions runs those
tests and checks that Linux commands contain no Apple toolchain flags.

These tests are a foundation, not evidence that the full desktop runner builds,
that streaming works, or that a release package is ready.

## Linux milestones

1. Build the complete native runner with reproducible legacy libpeer/mbedTLS/
   libsrtp dependencies. Keep all tracked packet-buffer and transport fixes.
2. Port Borealis startup, UI and resources; replace Switch-specific storage,
   browser login, controller, networking and media presentation hooks.
3. Enable and validate Xbox Remote Play, Cloud Gaming and PS local/PSN paths
   in that single UI, with separate settings.
4. Validate software decode first, then Linux hardware decoding and frame pacing;
   test audio, input, reconnect, sleep/resume and Steam Gaming Mode.
5. Package the validated executable and resources as an AppImage with dependency
   provenance, notices and corresponding sources for the combined PS build.

## Android milestones

After defining the shared platform boundary, add an ARM64 Android NDK/Gradle
target, lifecycle and resource/storage integration, authentication callbacks,
controller/touch input and video/audio presentation. Evaluate MediaCodec and
the audio backend against the existing pipeline before selecting JNI/SDL or
another integration. Do not claim that an empty APK is a streaming port.

## Validation limits

No Switch source or build files are changed by this foundation.
Full streaming, authentication, Borealis Linux UI, AppImage, APK and real-device
testing remain outstanding. In particular, tests do not prove that every Chiaki
or XStreaming setting exists in LunarNX; settings need an explicit parity audit.

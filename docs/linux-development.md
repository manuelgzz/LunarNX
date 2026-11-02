# Native Linux development

Linux has a native Borealis interface and a separate diagnostic CLI target.
The interface renders Xbox video inside its own window and retains separate
Xbox and PlayStation settings and controller mappings. It does not launch
Chiaki-ng or XStreamingDesktop applications. The PlayStation tile now opens the integrated console, pairing and PSN flows.
Native hardware decoding is still pending. Real console/account sessions need validation.

## Build

Requires Linux, Python 3.12+, Git (for applying patches, not cloning), CMake,
GNU Make, a C/C++ compiler and pkg-config. On Ubuntu 24.04 the native libraries are:

```sh
sudo apt-get install cmake g++ make pkg-config libsdl2-dev libavcodec-dev \
  libavutil-dev libswresample-dev libopus-dev libcurl4-openssl-dev libssl-dev
python3 scripts/prepare_linux_dependencies.py
bash scripts/build_linux_dependencies.sh
make -f Makefile.linux -j2
```

The preparation script fetches GitHub source archives at the revisions in
linux/dependencies.lock.json. No repositories are cloned. It applies the
existing four legacy libpeer patches and the tracked mbedTLS DTLS-SRTP/ECDSA
patch. Native mbedTLS timing and sockets stay enabled; Linux uses usrsctp's
native headers and the pinned libsrtp OpenSSL backend.

An existing lib/libpeer directory is deliberately left untouched and causes
preparation to stop. Use a fresh build workspace to avoid mixing dependency
trees. LINUX_PROVENANCE.json records the source revisions, archive hashes and
applied patch names in the generated dependency directory.

Output: build/linux/LunarNX_Desktop. It is dynamically linked to system SDL2,
FFmpeg, Opus, curl and OpenSSL. This CLI artifact is a diagnostic build. The native UI is also packaged separately
as a development AppImage; see [packaging.md](packaging.md).

## Offline media smoke

Install libavformat-dev, ffmpeg, xvfb, xauth and libgl1-mesa-dri, then build:

```sh
make -f Makefile.linux -j2 linux-native-smoke
```

The CI workflow creates short 320x180 Annex-B fixtures with libx264/libx265 and
runs the actual Xbox H.264, PlayStation H.264 and PlayStation HEVC decoder
strategies through the SDL renderer under Xvfb/Mesa. It also encodes an Opus
packet, decodes stereo PCM, tests packet-loss concealment and writes it to SDL's
dummy audio output. These tests use no accounts or real consoles.

This validates the shared PlayStation media strategy, independently of Chiaki
networking or PSN authentication.

## Local Xbox streaming smoke

```sh
make -f Makefile.linux -j2 linux-mock-stream
python3 -m venv build/mock-venv
build/mock-venv/bin/pip install -r linux/mock-requirements.txt
build/mock-venv/bin/python scripts/linux_mock_smoke.py
```

The harness generates a synthetic audio/video file and starts the existing
mock Xbox server. It drives the real StreamController/session/WebRTC/media
stack with dummy credentials, checks that video and audio frames arrive, then
stops the session. Generated media, logs and any runtime state stay in a
temporary working directory. The test does not authenticate to Microsoft.

## Native interface

Add libgl1-mesa-dev, libswscale-dev, libjson-c-dev, libminiupnpc-dev,
libevent-dev, protobuf-compiler and python3-protobuf to the packages above,
then run:

```sh
python3 scripts/prepare_linux_ui.py
python3 scripts/prepare_linux_chiaki.py
cmake -S linux/ui -B build/linux-ui -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux-ui --parallel 2
ctest --test-dir build/linux-ui --output-on-failure
build/linux-ui/LunarNX_Linux
```

Resources are copied beside the executable. Settings and account state use
XDG_CONFIG_HOME/lunarnx (default ~/.config/lunarnx); cached images and diagnostics
use XDG_CACHE_HOME/lunarnx (default ~/.cache/lunarnx). Keep account files private.
The native software video path uses FFmpeg and uploads RGBA frames to Borealis;
it does not create an additional SDL video window.

CI checks navigation through home, global settings, Xbox settings, PlayStation
settings and controller mapping in a single window. It also streams synthetic
video/audio through the real Xbox WebRTC stack into that same window:

```sh
SDL_AUDIODRIVER=dummy xvfb-run -a build/linux-ui/LunarNX_Linux --ui-smoke
build/mock-venv/bin/python scripts/linux_mock_smoke.py --native-ui
```

The isolated settings test also exercises SDL virtual controller mapping and
rumble strength. Real account authentication, console streaming, physical
controller behavior and handheld suspend/resume still require device testing.

## Native PlayStation SDK

The matching patched Chiaki library now builds independently on Linux:

```sh
sudo apt-get install libjson-c-dev libminiupnpc-dev libevent-dev protobuf-compiler python3-protobuf
python3 scripts/prepare_linux_chiaki.py
bash scripts/build_linux_chiaki.sh
```

This downloads pinned source archives into lib/chiaki-native, including curl
with WebSocket support, nanopb, Jerasure and gf-complete. It applies the same
transport patches as the Switch SDK and uses native OpenSSL instead of libnx
cryptography. Existing dependency sources are never overwritten.
The UI builds and links the same SDK in its CMake graph, including a single
pinned curl implementation shared with Xbox HTTP requests. The integrated
PlayStation page supports LAN discovery, registration, wake, PSN phone sign-in
and remote connection setup. These account/console flows require real device
validation.

Native controller input uses SDL. Touchscreen and physical controller touchpad
snapshots feed the existing PlayStation gesture classifier. Supported controller
gyro/accelerometer sensors feed Chiaki orientation tracking; devices without
those sensors report no motion data. Physical sensor behavior is unverified.

CI tests the local phone services with real sockets, including invalid pairing
input and a mismatched PSN callback state. It also replays synthetic H.264/HEVC
and stereo Opus through PsStreamController, PsMediaBridge, Chiaki's Opus decoder
and the in-window renderer:

```sh
SDL_AUDIODRIVER=dummy xvfb-run -a build/linux-ui/LunarNX_Linux --ps-replay /absolute/fixture.mp4
# Use --ps-hevc for a HEVC fixture.
```

Fixtures must contain 1280x720 video at 30 fps and 48 kHz stereo Opus with
10 ms packets. Replay is diagnostic only and does not exercise console
networking, encryption or Sony authentication.

## Next platform work

Validate local pairing, console streaming and PSN flows on real devices, then
address any platform differences. Development AppImage and Android ARM64 APK builds are now available from CI.
See [packaging.md](packaging.md) for downloads, build commands and validation limits.

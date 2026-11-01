# Native Linux development

Linux has a native Borealis interface and a separate diagnostic CLI target.
The interface renders Xbox video inside its own window and retains separate
Xbox and PlayStation settings and controller mappings. It does not launch
Chiaki-ng or XStreamingDesktop applications. PlayStation connection and native
hardware decoding are still pending; the PlayStation tile currently opens settings.

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
FFmpeg, Opus, curl and OpenSSL. The CI artifact is a diagnostic build, not an
AppImage or a portable release.

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

This validates the shared PlayStation media strategy, not Chiaki networking or
PSN authentication. PlayStation controllers are still guarded by __SWITCH__.

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

Add libgl1-mesa-dev and libswscale-dev to the packages above, then run:

```sh
python3 scripts/prepare_linux_ui.py
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

## Next platform work

Link a native Chiaki build and adapt the PlayStation discovery, pairing,
PSN callback and controller platform hooks. Validate local pairing and PSN
flows before calling the unified app ready. AppImage packaging and Android
platform work remain pending.

# Native Linux development

The current Linux target is the desktop test runner, not the finished Borealis
application. It links the Xbox session/WebRTC/media stack in one executable.
It does not launch Chiaki-ng or XStreamingDesktop applications.

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

## Next platform work

Port Borealis startup and resources using its existing desktop/SDL support.
Separate the outer UI and PlayStation feature guards from libnx-specific code
rather than defining __SWITCH__ on Linux. Adapt storage, locale, controller and
browser callbacks; preserve per-platform settings. Link a native Chiaki build
and validate local pairing and PSN flows before calling the unified app ready.
Android and AppImage packaging follow the working native UI and platform layer.

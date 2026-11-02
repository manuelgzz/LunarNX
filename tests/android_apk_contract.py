#!/usr/bin/env python3
"""Check the installable APK's native architecture, entry point and assets."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import zipfile

apk = Path(sys.argv[1])
readelf = Path(os.environ["ANDROID_NDK_ROOT"]) / "toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-readelf"
with zipfile.ZipFile(apk) as package, tempfile.TemporaryDirectory() as folder:
    names = set(package.namelist())
    libraries = {name for name in names if name.startswith("lib/") and name.endswith(".so")}
    required = {f"lib/arm64-v8a/{name}" for name in ("libmain.so", "libSDL2.so", "libc++_shared.so")}
    assert required <= libraries, required - libraries
    assert all(name.startswith("lib/arm64-v8a/") for name in libraries), libraries
    for name in libraries:
        target = Path(folder) / Path(name).name
        target.write_bytes(package.read(name))
        header = subprocess.check_output([readelf, "-h", target], text=True)
        assert "AArch64" in header, name
        if name.endswith("/libmain.so"):
            symbols = subprocess.check_output([readelf, "--dyn-syms", target], text=True)
            assert "SDL_main" in symbols
            native = target.read_bytes()
            for codec in (b"h264_mediacodec\x00", b"hevc_mediacodec\x00"):
                assert codec in native, f"Missing packaged decoder: {codec!r}"
    for name in ("i18n/en-US/lunarnx.json", "font/switch_font.ttf", "ca-certificates.crt"):
        assert "assets/resources/" + name in names, name
print("PASS Android APK: ARM64 native engines, SDL entry point and packaged resources")

#!/usr/bin/env python3
"""Package the native application using a checksum-pinned linuxdeploy build."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parent.parent


def main():
    output = ROOT / "build/packages"
    output.mkdir(parents=True, exist_ok=True)
    lock = json.loads((ROOT / "linux/appimage-tools.lock.json").read_text())
    tool = output / "linuxdeploy.AppImage"
    if not tool.exists():
        with urllib.request.urlopen(lock["url"], timeout=120) as response:
            tool.write_bytes(response.read())
    if hashlib.sha256(tool.read_bytes()).hexdigest() != lock["sha256"]:
        raise SystemExit("linuxdeploy checksum mismatch")
    tool.chmod(0o755)
    native = ROOT / "build/linux-ui"
    with tempfile.TemporaryDirectory(prefix="lunarnx-appdir-", dir=output) as folder:
        appdir = Path(folder) / "AppDir"
        binary = appdir / "usr/bin"
        docs = appdir / "usr/share/doc/lunarnx"
        binary.mkdir(parents=True)
        docs.mkdir(parents=True)
        shutil.copy2(native / "LunarNX_Linux", binary)
        shutil.copytree(native / "resources", binary / "resources")
        for name in ("LICENSE", "THIRD_PARTY_NOTICES.md"):
            shutil.copy2(ROOT / name, docs)
        for dependency in ("libpeer", "borealis", "chiaki-native"):
            source = ROOT / "lib" / dependency
            for path in source.rglob("*"):
                if path.is_file() and path.name.startswith(("COPYING", "LICENSE", "LINUX_PROVENANCE")) and ".git" not in path.parts:
                    target = docs / dependency / path.relative_to(source)
                    target.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copy2(path, target)
        trust = appdir / "usr/share/lunarnx"
        trust.mkdir(parents=True)
        shutil.copy2("/etc/ssl/certs/ca-certificates.crt", trust / "ca-certificates.crt")
        icon = Path(folder) / "lunarnx.png"
        subprocess.run(["ffmpeg", "-hide_banner", "-loglevel", "error", "-i",
                        str(ROOT / "res/icon.jpg"), "-vf", "scale=256:256",
                        "-frames:v", "1", str(icon)], check=True)
        environment = dict(os.environ, APPIMAGE_EXTRACT_AND_RUN="1", ARCH="x86_64",
                           LDAI_OUTPUT=str(output / "LunarNX-Linux-x86_64.AppImage"),
                           LDAI_NO_APPSTREAM="1",
                           LDAI_VERSION=(ROOT / "version.txt").read_text().strip())
        subprocess.run([str(tool), "--appdir", str(appdir), "--executable",
                        str(binary / "LunarNX_Linux"), "--desktop-file",
                        str(ROOT / "linux/lunarnx.desktop"), "--icon-file", str(icon),
                        "--custom-apprun", str(ROOT / "linux/AppRun"),
                        "--output", "appimage"], env=environment, cwd=output, check=True)


if __name__ == "__main__":
    main()

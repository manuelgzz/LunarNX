#!/usr/bin/env python3
"""Prepare pinned native dependencies from GitHub archives, without cloning.

The generated source tree is ignored by the application repository. Existing
trees are never overwritten. Requires Python 3.12+ for tarfile's data filter.
"""
from __future__ import annotations

import hashlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parent.parent
LOCK = ROOT / "linux/dependencies.lock.json"
PATCHES = [
    "0001-switch-adapt-libpeer-WebRTC-path.patch",
    "0002-fix-H264-access-unit-flush-and-quiet-SCTP-logs.patch",
    "legacy-libpeer-switch.patch",
    "0003-enable-usrsctp-datachannel-policy.patch",
]


def run(*args: str, cwd: Path | None = None) -> None:
    subprocess.run(args, cwd=cwd, check=True)


def unpack(repository: str, revision: str, destination: Path) -> str:
    url = f"https://codeload.github.com/{repository}/tar.gz/{revision}"
    request = urllib.request.Request(url, headers={"User-Agent": "LunarNX-Linux-build"})
    with urllib.request.urlopen(request, timeout=120) as response:
        archive = response.read()
    destination.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=destination.parent) as folder:
        with tarfile.open(fileobj=io.BytesIO(archive), mode="r:gz") as source:
            source.extractall(folder, filter="data")
        entries = list(Path(folder).iterdir())
        if len(entries) != 1 or not entries[0].is_dir():
            raise RuntimeError(f"Unexpected archive layout: {repository}")
        shutil.copytree(entries[0], destination, dirs_exist_ok=True)
    return hashlib.sha256(archive).hexdigest()


def main() -> None:
    lock = json.loads(LOCK.read_text())
    destination = ROOT / "lib/libpeer"
    if destination.exists():
        raise SystemExit("lib/libpeer already exists; use a fresh CI workspace. No existing sources were modified.")
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix=".linux-deps-", dir=destination.parent) as folder:
        stage = Path(folder) / "libpeer"
        provenance = []
        for item in lock["dependencies"]:
            print(f"Fetching {item['repository']} at {item['revision']}", flush=True)
            digest = unpack(item["repository"], item["revision"], stage / item["directory"])
            provenance.append({**item, "archive_sha256": digest})
        # git apply works on an initialized tree without cloning any repository.
        run("git", "init", "--quiet", str(stage))
        for name in PATCHES:
            patch = ROOT / "tools/libpeer_legacy" / name
            run("git", "apply", "--check", str(patch), cwd=stage)
            run("git", "apply", str(patch), cwd=stage)
        # Retain DTLS-SRTP and the ECDSA certificate fix. Restore native timing
        # and sockets, which the Switch-specific mbedTLS patch disables.
        mbed = stage / "third_party/mbedtls"
        patch = ROOT / "tools/libpeer_legacy/nested/0001-switch-configure-mbedtls-for-DTLS-SRTP.patch"
        run("git", "apply", "--directory=third_party/mbedtls", "--check", str(patch), cwd=stage)
        run("git", "apply", "--directory=third_party/mbedtls", str(patch), cwd=stage)
        config = mbed / "include/mbedtls/mbedtls_config.h"
        text = config.read_text()
        text = text.replace("//#define MBEDTLS_TIMING_C", "#define MBEDTLS_TIMING_C")
        text = text.replace("#define MBEDTLS_NO_TIMING_C", "/* Native platform provides timing. */")
        text = text.replace("//#define MBEDTLS_NET_C", "#define MBEDTLS_NET_C")
        config.write_text(text)
        # Linux uses the native usrsctp headers, never the libnx compatibility stubs.
        (stage / "LINUX_PROVENANCE.json").write_text(
            json.dumps({"dependencies": provenance, "patches": PATCHES,
                        "mbedtls_patch": patch.name,
                        "native_mbedtls_timing": True}, indent=2) + "\n"
        )
        os.replace(stage, destination)
    print("Pinned Linux dependency sources are ready.", flush=True)


if __name__ == "__main__":
    main()

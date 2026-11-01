#!/usr/bin/env python3
"""Prepare the matching Chiaki SDK and its submodules from pinned archives."""
import json
from pathlib import Path
import tempfile
from prepare_linux_dependencies import unpack, run

ROOT = Path(__file__).resolve().parent.parent
PATCHES = [
    "stun-order", "stream-switch", "video-reorder-capacity", "recv-allocation",
    "holepunch-reliability", "http-status", "stream-rtt", "recvbuf",
    "packetstats-wrap", "transport-diagnostics", "key-position-diagnostics",
    "route-preference",
]


def main():
    destination = ROOT / "lib/chiaki-native"
    if destination.exists():
        raise SystemExit("lib/chiaki-native already exists; existing sources were not modified")
    lock = json.loads((ROOT / "linux/chiaki-dependencies.lock.json").read_text())
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix=".native-chiaki-", dir=destination.parent) as folder:
        stage = Path(folder) / "chiaki"
        provenance = []
        for item in lock["dependencies"]:
            digest = unpack(item["repository"], item["revision"], stage / item["directory"])
            provenance.append({**item, "archive_sha256": digest})
        run("git", "init", "--quiet", str(stage))
        for name in PATCHES:
            patch = ROOT / "tools/chiaki_switch" / f"lunarnx-chiaki-{name}.patch"
            options = ["--recount"] if name in ("transport-diagnostics", "key-position-diagnostics") else []
            run("git", "apply", *options, "--check", str(patch), cwd=stage)
            run("git", "apply", *options, str(patch), cwd=stage)
        (stage / "LINUX_PROVENANCE.json").write_text(json.dumps(
            {"dependencies": provenance, "patches": PATCHES,
             "crypto_backend": "OpenSSL", "curl_websockets": True}, indent=2) + "\n")
        stage.rename(destination)


if __name__ == "__main__":
    main()

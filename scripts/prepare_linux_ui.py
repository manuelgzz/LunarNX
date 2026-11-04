#!/usr/bin/env python3
"""Prepare the pinned Borealis native UI source archive without cloning."""
import json
from pathlib import Path
import tempfile
from prepare_linux_dependencies import unpack

ROOT = Path(__file__).resolve().parent.parent
REVISION = "240223f372731949e04cba943c453dc45b69faa1"


def main():
    destination = ROOT / "lib/borealis"
    if destination.exists():
        raise SystemExit("lib/borealis already exists; existing sources were not modified")
    with tempfile.TemporaryDirectory(prefix=".native-ui-", dir=destination.parent) as folder:
        stage = Path(folder) / "borealis"
        digest = unpack("XITRIX/borealis", REVISION, stage)
        (stage / "LINUX_PROVENANCE.json").write_text(json.dumps(
            {"repository": "XITRIX/borealis", "revision": REVISION,
             "archive_sha256": digest, "system_sdl2": True}, indent=2) + "\n")
        stage.rename(destination)


if __name__ == "__main__":
    main()

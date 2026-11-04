#!/usr/bin/env python3
"""Archive the application and exact prepared dependency sources for the binary."""
from pathlib import Path
import subprocess
import sys
import tarfile

ROOT = Path(__file__).resolve().parent.parent


def source_filter(info):
    name = Path(info.name).name
    if info.isdir() and (name in (".git", "build", "build_host", "build_android", "CMakeFiles", "__pycache__") or name.startswith("build-")):
        return None
    if info.isfile() and name.endswith((".o", ".a", ".so", ".pyc")):
        return None
    return info


def main():
    output = Path(sys.argv[1]).resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    tracked = subprocess.check_output(["git", "ls-files", "-z"], cwd=ROOT).decode().split("\0")
    with tarfile.open(output, "w:gz") as archive:
        for name in tracked:
            if name and (ROOT / name).is_file():
                archive.add(ROOT / name, arcname="LunarNX/" + name, recursive=False)
        for name in ("libpeer", "borealis", "chiaki-native", "android-sources"):
            path = ROOT / "lib" / name
            if path.exists():
                archive.add(path, arcname="LunarNX/lib/" + name, filter=source_filter)


if __name__ == "__main__":
    main()

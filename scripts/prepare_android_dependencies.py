#!/usr/bin/env python3
"""Prepare Android dependency archives, Java bindings and packaged resources."""
import json
from pathlib import Path
import shutil
import tempfile
from prepare_linux_dependencies import ROOT, unpack

def main():
    destination = ROOT / "lib/android-sources"
    if destination.exists():
        raise SystemExit("lib/android-sources already exists; existing sources were not modified")
    provenance = []
    lock = json.loads((ROOT / "android/dependencies.lock.json").read_text())
    with tempfile.TemporaryDirectory(prefix=".android-deps-", dir=destination.parent) as folder:
        stage = Path(folder) / "sources"
        stage.mkdir()
        for item in lock["dependencies"]:
            digest = unpack(item["repository"], item["revision"], stage / item["directory"])
            provenance.append({**item, "archive_sha256": digest})
        (stage / "ANDROID_PROVENANCE.json").write_text(json.dumps(provenance, indent=2) + "\n")
        stage.rename(destination)
    generated = ROOT / "android/app/generated"
    java = generated / "java/org/libsdl/app"
    java.mkdir(parents=True, exist_ok=True)
    shutil.copytree(destination / "SDL/android-project/app/src/main/java/org/libsdl/app", java, dirs_exist_ok=True)
    borealis = ROOT / "lib/borealis"
    for name in ("PlatformUtils.java", "BorealisHandler.java"):
        shutil.copy2(borealis / "android-project/app/src/main/java/org/libsdl/app" / name, java)
    assets = generated / "assets/resources"
    shutil.copytree(borealis / "resources", assets, dirs_exist_ok=True)
    shutil.copytree(ROOT / "romfs", assets, dirs_exist_ok=True)
    shutil.copy2("/etc/ssl/certs/ca-certificates.crt", assets / "ca-certificates.crt")
    import subprocess
    subprocess.run(["python3", str(ROOT / "scripts/generate_changelog_resource.py"),
                    str(ROOT / "CHANGELOG.md"), str(assets / "changelog.json")], check=True)
    licenses = assets / "licenses"
    licenses.mkdir()
    for name in ("LICENSE", "THIRD_PARTY_NOTICES.md"):
        shutil.copy2(ROOT / name, licenses)
    for dependency in (ROOT / "lib/libpeer", borealis, ROOT / "lib/chiaki-native", destination):
        for path in dependency.rglob("*"):
            if path.is_file() and path.name.startswith(("COPYING", "LICENSE", "LINUX_PROVENANCE", "ANDROID_PROVENANCE")) and ".git" not in path.parts:
                target = licenses / dependency.name / path.relative_to(dependency)
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(path, target)
    # Android only needs the Chiaki SDK, not its separate JNI application.
    cmake = ROOT / "lib/chiaki-native/CMakeLists.txt"
    text = cmake.read_text().replace("if(CHIAKI_ENABLE_ANDROID)\n\tadd_subdirectory(android/app)",
        "if(CHIAKI_ENABLE_ANDROID AND NOT LUNARNX_CHIAKI_SDK_ONLY)\n\tadd_subdirectory(android/app)")
    cmake.write_text(text)

if __name__ == "__main__":
    main()

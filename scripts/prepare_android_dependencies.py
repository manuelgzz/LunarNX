#!/usr/bin/env python3
"""Prepare Android dependency archives, Java bindings and packaged resources."""
import json
from pathlib import Path
import shutil
import tempfile
from prepare_linux_dependencies import ROOT, unpack

def main():
    destination = ROOT / "lib/android-sources"
    provenance = []
    lock = json.loads((ROOT / "android/dependencies.lock.json").read_text())
    if destination.exists():
        import sys
        if "--reuse-prepared" not in sys.argv:
            raise SystemExit("lib/android-sources already exists; use --reuse-prepared for a verified CI cache")
        existing = json.loads((destination / "ANDROID_PROVENANCE.json").read_text())
        if [{key: item[key] for key in ("repository", "revision", "tag", "directory")} for item in existing] != lock["dependencies"]:
            raise SystemExit("Prepared Android dependency revisions do not match the lock")
    else:
        with tempfile.TemporaryDirectory(prefix=".android-deps-", dir=destination.parent) as folder:
            stage = Path(folder) / "sources"
            stage.mkdir()
            for item in lock["dependencies"]:
                digest = unpack(item["repository"], item["revision"], stage / item["directory"])
                provenance.append({**item, "archive_sha256": digest})
            (stage / "ANDROID_PROVENANCE.json").write_text(json.dumps(provenance, indent=2) + "\n")
            stage.rename(destination)
    # Request Android's optional low-latency decoder mode before configure.
    # Unsupported vendors ignore this MediaFormat key; CPU fallback stays available.
    mc_source = destination / "ffmpeg/libavcodec/mediacodecdec_common.c"
    mc = mc_source.read_text()
    configure_call = "    status = ff_AMediaCodec_configure(s->codec, format, s->surface, NULL, 0);"
    low_latency = '    ff_AMediaFormat_setInt32(format, "low-latency", 1);'
    if low_latency not in mc:
        if mc.count(configure_call) != 1:
            raise SystemExit("Pinned MediaCodec configure site changed")
        mc_source.write_text(mc.replace(configure_call, low_latency + "\n" + configure_call))
        (ROOT / "build/android-prefix/.lunarnx-complete").unlink(missing_ok=True)
    # mbedTLS 3.4's AESCE intrinsics are incompatible with NDK 28's Clang.
    # Use its portable AES implementation; SRTP still uses native OpenSSL.
    config = ROOT / "lib/libpeer/third_party/mbedtls/include/mbedtls/mbedtls_config.h"
    original = config.read_text()
    portable = "\n".join(
        "/* Android NDK: use portable mbedTLS AES. */"
        if line.strip() == "#define MBEDTLS_AESCE_C" else line
        for line in original.splitlines()) + "\n"
    if portable != original:
        config.write_text(portable)
        (ROOT / "build/android-prefix/.lunarnx-complete").unlink(missing_ok=True)
    generated = ROOT / "android/app/generated"
    java = generated / "java/org/libsdl/app"
    java.mkdir(parents=True, exist_ok=True)
    shutil.copytree(destination / "SDL/android-project/app/src/main/java/org/libsdl/app", java, dirs_exist_ok=True)
    borealis = ROOT / "lib/borealis"
    for name in ("BorealisHandler.java",):
        shutil.copy2(borealis / "android-project/app/src/main/java/org/libsdl/app" / name, java)
    # PlatformUtils is maintained with the application so its JNI ABI matches
    # the native Borealis backend, including both device-rumble methods.
    (java / "PlatformUtils.java").unlink(missing_ok=True)
    assets = generated / "assets/resources"
    shutil.copytree(borealis / "resources", assets, dirs_exist_ok=True)
    shutil.copytree(ROOT / "romfs", assets, dirs_exist_ok=True)
    shutil.copy2("/etc/ssl/certs/ca-certificates.crt", assets / "ca-certificates.crt")
    import subprocess
    subprocess.run(["python3", str(ROOT / "scripts/generate_changelog_resource.py"),
                    str(ROOT / "CHANGELOG.md"), str(assets / "changelog.json")], check=True)
    licenses = assets / "licenses"
    licenses.mkdir(exist_ok=True)
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

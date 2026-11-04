#!/usr/bin/env python3
"""Validate the Java methods the native Android platform looks up through JNI.

Inspect declarations in the packaged DEX, including exact descriptors and
public/static flags. A string or an unresolved method reference is insufficient.
"""
from pathlib import Path
import io
import struct
import sys
import zipfile

CLASS = "Lorg/libsdl/app/PlatformUtils;"
REQUIRED = {
    ("initDeviceRumble", "()V"),
    ("deviceRumble", "(SS)V"),
    ("isBatterySupported", "()Z"),
    ("getBatteryLevel", "()I"),
    ("isBatteryCharging", "()Z"),
    ("isWifiSupported", "()Z"),
    ("isWifiConnected", "()Z"),
    ("getWifiSignalStrength", "()I"),
    ("isEthernetConnected", "()Z"),
    ("openBrowser", "(Ljava/lang/String;)V"),
    ("getAppScreenBrightness", "(Landroid/app/Activity;)F"),
    ("setAppScreenBrightness", "(Landroid/app/Activity;F)V"),
}


def declarations(data):
    if not data.startswith(b"dex\n"):
        raise ValueError("Expected a standard DEX file")

    def uint(offset):
        return struct.unpack_from("<I", data, offset)[0]

    def uleb(offset):
        value = 0
        for index in range(5):
            byte = data[offset]
            offset += 1
            value |= (byte & 0x7f) << (7 * index)
            if byte < 0x80:
                return value, offset
        raise ValueError("Invalid ULEB128")

    string_count, string_offset = uint(56), uint(60)
    strings = []
    for index in range(string_count):
        _, start = uleb(uint(string_offset + 4 * index))
        end = data.index(0, start)
        # JNI identifiers/descriptors are ASCII; tolerate unrelated MUTF-8 text.
        strings.append(data[start:end].decode("utf-8", errors="replace"))
    types = [strings[uint(uint(68) + 4 * i)] for i in range(uint(64))]

    def descriptor(index):
        offset = uint(76) + 12 * index
        return_type, parameters = uint(offset + 4), uint(offset + 8)
        arguments = ""
        if parameters:
            count = uint(parameters)
            arguments = "".join(types[struct.unpack_from("<H", data, parameters + 4 + 2 * i)[0]]
                                for i in range(count))
        return "(" + arguments + ")" + types[return_type]

    result = set()
    for index in range(uint(96)):
        offset = uint(100) + 32 * index
        if types[uint(offset)] != CLASS:
            continue
        cursor = uint(offset + 24)
        if not cursor:
            continue
        counts = []
        for _ in range(4):
            count, cursor = uleb(cursor)
            counts.append(count)
        for _ in range(counts[0] + counts[1]):
            _, cursor = uleb(cursor)
            _, cursor = uleb(cursor)
        for method_count in counts[2:]:
            method_index = 0
            for _ in range(method_count):
                delta, cursor = uleb(cursor)
                flags, cursor = uleb(cursor)
                _, cursor = uleb(cursor)
                method_index += delta
                if flags & 0x9 != 0x9:
                    continue
                owner, proto, name = struct.unpack_from("<HHI", data, uint(92) + 8 * method_index)
                if types[owner] == CLASS:
                    result.add((strings[name], descriptor(proto)))
    return result


def main():
    found = set()
    with zipfile.ZipFile(Path(sys.argv[1])) as source:
        apks = [name for name in source.namelist() if name.endswith(".apk")]
        if apks:
            if len(apks) != 1:
                raise SystemExit("Expected one APK in the artifact ZIP")
            with zipfile.ZipFile(io.BytesIO(source.read(apks[0]))) as package:
                for name in package.namelist():
                    if name.startswith("classes") and name.endswith(".dex"):
                        found.update(declarations(package.read(name)))
        else:
            for name in source.namelist():
                if name.startswith("classes") and name.endswith(".dex"):
                    found.update(declarations(source.read(name)))
    missing = REQUIRED - found
    if missing:
        raise SystemExit("FAIL Android JNI bridge: missing public static methods: " +
                         ", ".join(name + signature for name, signature in sorted(missing)))
    print(f"PASS Android JNI bridge: {len(REQUIRED)} native-call signatures declared in packaged DEX")


if __name__ == "__main__":
    main()

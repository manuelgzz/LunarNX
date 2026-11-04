#pragma once
#include <cstring>

namespace lunar::stream {
// Native platforms always present CPU-accessible frames, independently of decoding.
enum class NativeDecodeMode { Automatic, Software };
inline const char* nativeDecodeModeName(NativeDecodeMode mode) {
    return mode == NativeDecodeMode::Software ? "software" : "auto";
}
inline NativeDecodeMode parseNativeDecodeMode(const char* value) {
    return value && std::strcmp(value, "software") == 0
        ? NativeDecodeMode::Software : NativeDecodeMode::Automatic;
}
inline bool nativeSoftwareRequested(NativeDecodeMode mode, const char* override_value) {
    return mode == NativeDecodeMode::Software ||
        (override_value && std::strcmp(override_value, "software") == 0);
}
} // namespace lunar::stream

#pragma once
#include <atomic>
#include <cstdint>
namespace lunar::platform {
inline std::atomic<uint64_t> native_presented_frames{0};
inline std::atomic<uint64_t> native_presented_generation{0};
inline void noteNativePresentation(uint64_t generation) {
    if (native_presented_generation.exchange(generation) != generation)
        native_presented_frames.fetch_add(1);
}
} // namespace lunar::platform

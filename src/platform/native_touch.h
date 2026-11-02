#pragma once
#include <cstdint>

namespace lunar::platform {
// SDL's normalized coordinates expressed in the shared 1280 x 720 UI space.
struct NativeTouch {
    uint64_t finger_id;
    uint32_t x, y;
    bool touchscreen = false;
};
} // namespace lunar::platform

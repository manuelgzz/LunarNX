#pragma once
#include <cstdint>

namespace lunar::platform {
// SDL's normalized coordinates expressed in the shared 1280 x 720 UI space.
struct NativeTouch {
    uint64_t finger_id;
    uint32_t x, y;
    bool touchscreen = false;
    // Dedicated pads scale coordinates for the console, but gesture thresholds
    // should still measure movement in original screen pixels.
    float gesture_scale_x = 1.0f, gesture_scale_y = 1.0f;
};
} // namespace lunar::platform

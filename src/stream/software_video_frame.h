#pragma once

#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>

namespace lunar::stream {

struct SoftwareVideoFrameSnapshot {
    std::vector<uint8_t> rgba;
    int width = 0;
    int height = 0;
    uint64_t timestamp = 0;
    uint64_t generation = 0;

    bool empty() const { return rgba.empty() || width <= 0 || height <= 0; }
};

class SoftwareVideoFrameSink {
public:
    static SoftwareVideoFrameSink& instance();

    void clear();
    bool publishRgba(const uint8_t* rgba,
                     int width,
                     int height,
                     uint64_t timestamp);
    // Immutable ownership keeps pixels alive during upload without holding the
    // producer lock or copying a full image on each UI repaint.
    std::shared_ptr<const SoftwareVideoFrameSnapshot> acquire() const;
    SoftwareVideoFrameSnapshot snapshot() const;

private:
    mutable std::mutex mutex_;
    std::shared_ptr<const SoftwareVideoFrameSnapshot> frame_;
    uint64_t generation_ = 0;
};

} // namespace lunar::stream

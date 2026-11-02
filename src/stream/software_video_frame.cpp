#include "software_video_frame.h"

#include <limits>

namespace lunar::stream {

SoftwareVideoFrameSink& SoftwareVideoFrameSink::instance() {
    static SoftwareVideoFrameSink sink;
    return sink;
}

void SoftwareVideoFrameSink::clear() {
    std::shared_ptr<const SoftwareVideoFrameSnapshot> retired;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        retired.swap(frame_);
        ++generation_;
    }
}

bool SoftwareVideoFrameSink::publishRgba(const uint8_t* rgba,
                                         int width,
                                         int height,
                                         uint64_t timestamp) {
    if (!rgba || width <= 0 || height <= 0) return false;
    const size_t w = static_cast<size_t>(width);
    const size_t h = static_cast<size_t>(height);
    if (w > std::numeric_limits<size_t>::max() / 4 / h) return false;

    // Copy before taking the publication lock. Readers retain an immutable
    // frame, so decoding the next image cannot overwrite an in-flight upload.
    auto next = std::make_shared<SoftwareVideoFrameSnapshot>();
    next->rgba.assign(rgba, rgba + w * h * 4);
    next->width = width;
    next->height = height;
    next->timestamp = timestamp;
    std::shared_ptr<const SoftwareVideoFrameSnapshot> retired;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        next->generation = ++generation_;
        retired.swap(frame_);
        frame_ = std::move(next);
    }
    return true;
}

std::shared_ptr<const SoftwareVideoFrameSnapshot>
SoftwareVideoFrameSink::acquire() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frame_;
}

SoftwareVideoFrameSnapshot SoftwareVideoFrameSink::snapshot() const {
    const auto frame = acquire();
    return frame ? *frame : SoftwareVideoFrameSnapshot{};
}

} // namespace lunar::stream

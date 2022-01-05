#pragma once

namespace unity {
namespace webrtc {

using namespace ::webrtc;

class Size {
public:
    constexpr Size() : width_(0), height_(0) {}
    constexpr Size(int width, int height)
        : width_(std::max(0, width)), height_(std::max(0, height)) {}

    void set_width(int width) { width_ = std::max(0, width); }
    void set_height(int height) { height_ = std::max(0, height); }

    constexpr int width() const { return width_; }
    constexpr int height() const { return height_; }
private:
    int width_;
    int height_;
};

}
}

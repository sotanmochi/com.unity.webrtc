#include "pch.h"
#include "VideoFrameUtil.h"

namespace unity {
namespace webrtc {

rtc::scoped_refptr<VideoFrame> CreateTestFrame(
    int width, int height) {

    std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer = std::make_unique<GpuMemoryBuffer>();

    const int64_t timestamp_us =
        webrtc::Clock::GetRealTimeClock()->TimeInMicroseconds();

    return VideoFrame::WrapExternalGpuMemoryBuffer(
        width, height, std::move(gpu_memory_buffer), webrtc::TimeDelta::Micros(timestamp_us));
}

} // end namespace webrtc
} // end namespace unity

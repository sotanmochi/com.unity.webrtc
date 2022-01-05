#include "pch.h"
#include "VideoFrameUtil.h"

namespace unity {
namespace webrtc {

rtc::scoped_refptr<VideoFrame> CreateTestFrame(
    const Size& size) {

    std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer =
        std::make_unique<FakeGpuMemoryBuffer>();

    const int64_t timestamp_us =
        webrtc::Clock::GetRealTimeClock()->TimeInMicroseconds();

    return VideoFrame::WrapExternalGpuMemoryBuffer(
        size, std::move(gpu_memory_buffer), webrtc::TimeDelta::Micros(timestamp_us));
}

} // end namespace webrtc
} // end namespace unity

#include "pch.h"

#include "VideoFrameUtil.h"

namespace unity
{
namespace webrtc
{

    rtc::scoped_refptr<VideoFrame> CreateTestFrame(const Size& size)
    {
        rtc::scoped_refptr<GpuMemoryBuffer> gmb = new rtc::RefCountedObject<FakeGpuMemoryBuffer>();

        const int64_t timestamp_us = webrtc::Clock::GetRealTimeClock()->TimeInMicroseconds();

        return VideoFrame::WrapExternalGpuMemoryBuffer(
            size, std::move(gmb), nullptr, webrtc::TimeDelta::Micros(timestamp_us));
    }

} // end namespace webrtc
} // end namespace unity

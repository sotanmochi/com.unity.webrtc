#pragma once

#include "GpuMemoryBuffer.h"
#include "Size.h"
#include "api/units/time_delta.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/timestamp_aligner.h"

namespace unity
{
namespace webrtc
{

    using namespace ::webrtc;

    class VideoFrame : public rtc::RefCountInterface
    {
    public:
        using ReturnBufferToPoolCallback = std::function<void(std::unique_ptr<GpuMemoryBuffer>)>;

        VideoFrame() = delete;
        VideoFrame(const VideoFrame&) = delete;
        VideoFrame& operator=(const VideoFrame&) = delete;

        Size size() const { return size_; }
        TimeDelta timestamp() const { return timestamp_; }
        void set_timestamp(TimeDelta timestamp) { timestamp_ = timestamp; }

        GpuMemoryBuffer* GetGpuMemoryBuffer() const;
        bool HasGpuMemoryBuffer() const;

        static rtc::scoped_refptr<VideoFrame> WrapExternalGpuMemoryBuffer(
            const Size& size,
            std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
            ReturnBufferToPoolCallback returnBufferToPoolCallback,
            TimeDelta timestamp);
        static rtc::scoped_refptr<VideoFrame>
        ConvertToMemoryMappedFrame(rtc::scoped_refptr<VideoFrame> video_frame);

    protected:
        VideoFrame(
            const Size& size,
            std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
            ReturnBufferToPoolCallback returnBufferToPoolCallback,
            TimeDelta timestamp);
        virtual ~VideoFrame();

    private:
        Size size_;
        std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer_;
        ReturnBufferToPoolCallback returnBufferToPoolCallback_;
        TimeDelta timestamp_;
    };

} // end namespace webrtc
} // end namespace unity

#pragma once

#include "rtc_base/timestamp_aligner.h"
#include "rtc_base/ref_counted_object.h"
#include "api/units/time_delta.h"
#include "GpuMemoryBuffer.h"
#include "Size.h"

namespace unity {
namespace webrtc {

using namespace ::webrtc;

class VideoFrame : public rtc::RefCountInterface {
public:
    //enum {
    //    kMaxPlanes = 4,

    //    kYPlane = 0,
    //    kARGBPlane = kYPlane,
    //    kUPlane = 1,
    //    kUVPlane = kUPlane,
    //    kVPlane = 2,
    //    kAPlane = 3,
    //};

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
        //const gfx::Rect& visible_rect,
        //const gfx::Size& natural_size,
        std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
        //const gpu::MailboxHolder(&mailbox_holders)[kMaxPlanes],
        //ReleaseMailboxAndGpuMemoryBufferCB mailbox_holder_and_gmb_release_cb,
        TimeDelta timestamp);
    static rtc::scoped_refptr<VideoFrame> ConvertToMemoryMappedFrame(
        rtc::scoped_refptr<VideoFrame> video_frame);
protected:
    VideoFrame(
        const Size& size,
        std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
        TimeDelta timestamp);
    virtual ~VideoFrame() {}
private:
    Size size_;
    std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer_;
    TimeDelta timestamp_;
};

} // end namespace webrtc
} // end namespace unity

#pragma once

#include "rtc_base/timestamp_aligner.h"
#include "rtc_base/ref_counted_object.h"
#include "GpuMemoryBuffer.h"

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

    constexpr int width() const { return width_; }
    constexpr int height() const { return height_; }

    void set_width(int width) { width_ = std::max(0, width); }
    void set_height(int height) { height_ = std::max(0, height); }

    TimeDelta timestamp() const { return timestamp_; }
    void set_timestamp(TimeDelta timestamp) { timestamp_ = timestamp; }

    rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer() { return frame_buffer_; }

    GpuMemoryBuffer* GetGpuMemoryBuffer() const;
    bool VideoFrame::HasGpuMemoryBuffer() const;

    static rtc::scoped_refptr<VideoFrame> WrapExternalGpuMemoryBuffer(
        int width, int height,
        //const gfx::Rect& visible_rect,
        //const gfx::Size& natural_size,
        std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
        //const gpu::MailboxHolder(&mailbox_holders)[kMaxPlanes],
        //ReleaseMailboxAndGpuMemoryBufferCB mailbox_holder_and_gmb_release_cb,
        TimeDelta timestamp);
    static rtc::scoped_refptr<VideoFrame> ConvertToMemoryMappedFrame(
        rtc::scoped_refptr<VideoFrame> video_frame);
    virtual ~VideoFrame() {}
protected:
    VideoFrame::VideoFrame(
        int width, int height,
        std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
        TimeDelta timestamp);
private:
    TimeDelta timestamp_;
    int width_;
    int height_;
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer_;
    std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer_;
};

} // end namespace webrtc
} // end namespace unity

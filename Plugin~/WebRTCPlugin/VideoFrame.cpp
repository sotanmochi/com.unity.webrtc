#include "pch.h"
#include "VideoFrame.h"

namespace unity {
namespace webrtc {

VideoFrame::VideoFrame(const Size& size,
    std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
    TimeDelta timestamp)
    : size_(size)
    , gpu_memory_buffer_(std::move(gpu_memory_buffer))
    , timestamp_(timestamp) {}

rtc::scoped_refptr<VideoFrame> VideoFrame::WrapExternalGpuMemoryBuffer(
    const Size& size,
    std::unique_ptr<GpuMemoryBuffer> gpu_memory_buffer,
//    const gpu::MailboxHolder(&mailbox_holders)[kMaxPlanes],
//    ReleaseMailboxAndGpuMemoryBufferCB mailbox_holder_and_gmb_release_cb,
    TimeDelta timestamp) {
    return new rtc::RefCountedObject<VideoFrame>(size, std::move(gpu_memory_buffer), timestamp);
}

bool VideoFrame::HasGpuMemoryBuffer() const {
    return gpu_memory_buffer_ != nullptr;
}

GpuMemoryBuffer* VideoFrame::GetGpuMemoryBuffer() const {
    return gpu_memory_buffer_.get();
}

rtc::scoped_refptr<VideoFrame> VideoFrame::ConvertToMemoryMappedFrame(
    rtc::scoped_refptr<VideoFrame> video_frame) {
    RTC_DCHECK(video_frame);
    RTC_DCHECK(video_frame->HasGpuMemoryBuffer());

    //auto* gmb = video_frame->GetGpuMemoryBuffer();
    //if (!gmb->Map())
    //    return nullptr;

    //const size_t num_planes = VideoFrame::NumPlanes(video_frame->format());
    //uint8_t* plane_addrs[VideoFrame::kMaxPlanes] = {};
    //for (size_t i = 0; i < num_planes; i++)
    //    plane_addrs[i] = static_cast<uint8_t*>(gmb->memory(i));

    //auto mapped_frame = VideoFrame::WrapExternalYuvDataWithLayout(
    //    video_frame->layout(), video_frame->visible_rect(),
    //    video_frame->natural_size(), plane_addrs[0], plane_addrs[1],
    //    plane_addrs[2], video_frame->timestamp());

    //if (!mapped_frame) {
    //    gmb->Unmap();
    //    return nullptr;
    //}

    //mapped_frame->set_color_space(video_frame->ColorSpace());
    //mapped_frame->metadata().MergeMetadataFrom(video_frame->metadata());

    //// Pass |video_frame| so that it outlives |mapped_frame| and the mapped buffer
    //// is unmapped on destruction.
    //mapped_frame->AddDestructionObserver(base::BindOnce(
    //    [](rtc::scoped_refptr<VideoFrame> frame) {
    //        DCHECK(frame->HasGpuMemoryBuffer());
    //        frame->GetGpuMemoryBuffer()->Unmap();
    //    },
    //    std::move(video_frame)));
    //return mapped_frame;
    return nullptr;
}

} // end namespace webrtc
} // end namespace unity

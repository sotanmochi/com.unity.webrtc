#include "pch.h"
#include "UnityVideoTrackSource.h"
#include "VideoFrameBufferCreatorInterface.h"

namespace unity
{
namespace webrtc
{

//class I420FrameAdapter : public webrtc::I420BufferInterface {
//public:
//    explicit I420FrameAdapter(rtc::scoped_refptr<VideoFrame> frame)
//        : frame_(std::move(frame)) {
//        //RTC_DCHECK_EQ(frame_->format(), media::PIXEL_FORMAT_I420);
//        //RTC_DCHECK_EQ(frame_->visible_rect().size(), frame_->natural_size());
//    }
//
//    int width() const override { return frame_->width(); }
//    int height() const override { return frame_->height(); }
//
//    const uint8_t* DataY() const override {
//        return frame_->visible_data(VideoFrame::kYPlane);
//    }
//
//    const uint8_t* DataU() const override {
//        return frame_->visible_data(VideoFrame::kUPlane);
//    }
//
//    const uint8_t* DataV() const override {
//        return frame_->visible_data(VideoFrame::kVPlane);
//    }
//
//    int StrideY() const override {
//        return frame_->stride(VideoFrame::kYPlane);
//    }
//
//    int StrideU() const override {
//        return frame_->stride(VideoFrame::kUPlane);
//    }
//
//    int StrideV() const override {
//        return frame_->stride(VideoFrame::kVPlane);
//    }
//
//protected:
//    rtc::scoped_refptr<VideoFrame> frame_;
//};

rtc::scoped_refptr<I420BufferInterface> VideoFrameAdapter::ToI420() {
    return ConvertToVideoFrameBuffer(frame_)->ToI420();
}

rtc::scoped_refptr<webrtc::VideoFrameBuffer>
VideoFrameAdapter::ConvertToVideoFrameBuffer(rtc::scoped_refptr<VideoFrame> video_frame) {
    // https://source.chromium.org/chromium/chromium/src/+/main:third_party/blink/renderer/platform/webrtc/convert_to_webrtc_video_frame_buffer.cc

    auto converted_frame = ConstructVideoFrameFromGpu(video_frame);
    return nullptr; // use GraphicsUtility

    //return new rtc::RefCountedObject<I420FrameAdapter>(
    //    std::move(converted_frame));
}

rtc::scoped_refptr<VideoFrame>
VideoFrameAdapter::ConstructVideoFrameFromGpu(rtc::scoped_refptr<VideoFrame> video_frame) {
    // todo(kazuki):: use GraphicsUtility
    return nullptr;
}


UnityVideoTrackSource::UnityVideoTrackSource(
    bool is_screencast,
    absl::optional<bool> needs_denoising)
    : AdaptedVideoTrackSource(/*required_alignment=*/1)
    , is_screencast_(is_screencast)
    , needs_denoising_(needs_denoising)
{
//  DETACH_FROM_THREAD(thread_checker_);
}

UnityVideoTrackSource::~UnityVideoTrackSource()
{
    {
        std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
    }
}

UnityVideoTrackSource::SourceState UnityVideoTrackSource::state() const
{
    // TODO(nisse): What's supposed to change this state?
    return MediaSourceInterface::SourceState::kLive;
}

bool UnityVideoTrackSource::remote() const
{
    return false;
}

bool UnityVideoTrackSource::is_screencast() const
{
    return is_screencast_;
}

absl::optional<bool> UnityVideoTrackSource::needs_denoising() const
{
    return needs_denoising_;
}

void UnityVideoTrackSource::OnFrameCaptured(
    rtc::scoped_refptr<VideoFrame> frame)
{
    // todo::(kazuki) change compiler vc to clang
#if defined(__clang__)
    // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
#endif
    const std::unique_lock<std::shared_timed_mutex> lock(m_mutex, std::try_to_lock);
    if (!lock)
    {
        // currently encoding
        return;
    }

    const int64_t now_us = rtc::TimeMicros();
    const int64_t translated_camera_time_us =
        timestamp_aligner_.TranslateTimestamp(frame->timestamp().us(),
            now_us);

    rtc::scoped_refptr<VideoFrameAdapter> frame_adapter(
        new rtc::RefCountedObject<VideoFrameAdapter>(std::move(frame)));

    ::webrtc::VideoFrame::Builder builder =
        ::webrtc::VideoFrame::Builder()
        .set_video_frame_buffer(frame_adapter)
        .set_timestamp_us(translated_camera_time_us);

    OnFrame(builder.build());
}

} // end namespace webrtc
} // end namespace unity

#pragma once

#include <shared_mutex>
#include "GraphicsDevice/IGraphicsDevice.h"
#include "rtc_base/timestamp_aligner.h"


// todo::(kazuki) change compiler vc to clang
#if defined(__clang__)
// todo::(kazuki) fix error: 'build/chromeos_buildflags.h' file not found
// #include "base/threading/thread_checker.h"
#endif

namespace unity {
namespace webrtc {

class IGraphicsDevice;
class VideoFrameBufferCreatorInterface;

enum VIDEO_SOURCE_DEST_MEMORY_TYPE_FLAG
{
    GPU_MEMORY = 0x1L,
    CPU_MEMORY = 0x2L,
};

// This class implements webrtc's VideoTrackSourceInterface. To pass frames down
// the webrtc video pipeline, each received a media::VideoFrame is converted to
// a webrtc::VideoFrame, taking any adaptation requested by downstream classes
// into account.
class UnityVideoTrackSource :
    public rtc::AdaptedVideoTrackSource
{
    public:
        //struct FrameAdaptationParams
        //{
        //    bool should_drop_frame;
        //    int crop_x;
        //    int crop_y;
        //    int crop_width;
        //    int crop_height;
        //    int scale_to_width;
        //    int scale_to_height;
        //};

    UnityVideoTrackSource(
        IGraphicsDevice* device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t memoryType,
        bool is_screencast, absl::optional<bool> needs_denoising);
    ~UnityVideoTrackSource() override;


    void Init();

    SourceState state() const override;

    bool remote() const override;
    bool is_screencast() const override;
    absl::optional<bool> needs_denoising() const override;

    void OnFrameCaptured(int64_t timestamp_us);

    using ::webrtc::VideoTrackSourceInterface::AddOrUpdateSink;
    using ::webrtc::VideoTrackSourceInterface::RemoveSink;

private:
    //FrameAdaptationParams ComputeAdaptationParams(int width,
    //                                            int height,
    //                                            int64_t time_us);

    // Delivers |frame| to base class method
    // rtc::AdaptedVideoTrackSource::OnFrame(). If the cropping (given via
    // |frame->visible_rect()|) has changed since the last delivered frame, the
    // whole frame is marked as updated.
    // void DeliverFrame(rtc::scoped_refptr<::webrtc::VideoFrame> frame,
    //                  gfx::Rect* update_rect,
    //                  int64_t timestamp_us);

    // |thread_checker_| is bound to the libjingle worker thread.
    // todo::(kazuki) change compiler vc to clang
#if defined(__clang__)
    // THREAD_CHECKER(thread_checker_);
#endif
    // media::VideoFramePool scaled_frame_pool_;

    // State for the timestamp translation.
    rtc::TimestampAligner timestamp_aligner_;

    const bool is_screencast_;
    const absl::optional<bool> needs_denoising_;
    std::timed_mutex m_mutex;
    std::unique_ptr<VideoFrameBufferCreatorInterface> m_bufferCreator;
};

} // end namespace webrtc
} // end namespace unity

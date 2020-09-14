#include "pch.h"

#include "GpuResourceBuffer.h"

namespace unity
{
namespace webrtc
{

//TEST(GpuResourceBufferTest, WidthAndHeight) {
    //const gfx::Size kCodedSize(1280, 960);
    //const gfx::Rect kVisibleRect(0, 120, 1280, 720);
    //const gfx::Size kNaturalSize(640, 360);

    // The adapter should report width and height from the visible rectangle for
    // VideoFrame backed by owned memory.
    //auto owned_memory_frame =
    //    CreateTestFrame(kCodedSize, kVisibleRect, kNaturalSize,
    //        media::VideoFrame::STORAGE_OWNED_MEMORY);

    //rtc::scoped_refptr<GpuResourceBuffer> buffer(
    //    new rtc::RefCountedObject<GpuResourceBuffer>();
    //EXPECT_EQ(buffer->width(), kVisibleRect.width());
    //EXPECT_EQ(buffer->height(), kVisibleRect.height());

    // The adapter should report width and height from the natural size for
    // VideoFrame backed by GpuMemoryBuffer.
    //auto gmb_frame =
    //    CreateTestFrame(kCodedSize, kVisibleRect, kNaturalSize,
    //        media::VideoFrame::STORAGE_GPU_MEMORY_BUFFER);
    //rtc::scoped_refptr<webrtc::VideoFrameBuffer> gmb_frame_adapter(
    //    new rtc::RefCountedObject<WebRtcVideoFrameAdapter>(
    //        std::move(gmb_frame),
    //        WebRtcVideoFrameAdapter::LogStatus::kNoLogging));
    //EXPECT_EQ(gmb_frame_adapter->width(), kNaturalSize.width());
    //EXPECT_EQ(gmb_frame_adapter->height(), kNaturalSize.height());
//}

} // end namespace webrtc
} // end namespace unity

#include "pch.h"
#include "Size.h"
#include "GpuMemoryBuffer.h"
#include "VideoFrameUtil.h"
#include "UnityVideoTrackSource.h"

namespace unity
{
namespace webrtc
{

TEST(GpuMemoryBufferTest, WidthAndHeight) {
    const Size kCodedSize(1280, 960);

    auto testFrame = CreateTestFrame(kCodedSize);
    auto frame = VideoFrameAdapter::CreateVideoFrame(testFrame);
    auto buffer = frame.video_frame_buffer();
    
     EXPECT_EQ(buffer->width(), kCodedSize.width());
     EXPECT_EQ(buffer->height(), kCodedSize.height());
}

} // end namespace webrtc
} // end namespace unity

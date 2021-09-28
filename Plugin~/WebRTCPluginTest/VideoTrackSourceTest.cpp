#include "pch.h"

#include "GpuResourceBuffer.h"
#include "GraphicsDeviceTestBase.h"
#include "Context.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"
#include "UnityVideoTrackSource.h"

using testing::_;
using testing::Invoke;
using testing::Mock;

namespace unity
{
namespace webrtc
{
class MockVideoSink : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    MOCK_METHOD1(OnFrame, void(const webrtc::VideoFrame&));
};

const int width = 256;
const int height = 256;

class VideoTrackSourceTest : public GraphicsDeviceTestBase
{
public:
    VideoTrackSourceTest() :
        m_texture(m_device->CreateDefaultTextureV(width, height, m_textureFormat))
    {
        m_trackSource = new rtc::RefCountedObject<UnityVideoTrackSource>(
            m_device,
            m_texture->GetNativeTexturePtrV(),
            m_textureFormat,
            GPU_MEMORY | CPU_MEMORY,
            /*is_screencast=*/ false,
            /*needs_denoising=*/ absl::nullopt);
        m_trackSource->AddOrUpdateSink(&mock_sink_, rtc::VideoSinkWants());
        m_trackSource->Init();

        EXPECT_NE(nullptr, m_device);

        context = std::make_unique<Context>();
    }
    ~VideoTrackSourceTest() override
    {
        m_trackSource->RemoveSink(&mock_sink_);
    }
protected:
    std::unique_ptr<Context> context;
    std::unique_ptr<ITexture2D> m_texture;

    MockVideoSink mock_sink_;
    rtc::scoped_refptr<UnityVideoTrackSource> m_trackSource;

    webrtc::VideoFrame::Builder CreateBlackFrameBuilder(int width, int height)
    {
        rtc::scoped_refptr<webrtc::I420Buffer> buffer =
            webrtc::I420Buffer::Create(width, height);

        webrtc::I420Buffer::SetBlack(buffer);
        return webrtc::VideoFrame::Builder().set_video_frame_buffer(buffer);
    }

    void SendTestFrame(int width, int height)
    {
        m_trackSource->OnFrameCaptured(0);
    }
};

TEST_P(VideoTrackSourceTest, Init)
{
    m_trackSource->Init();
}

TEST_P(VideoTrackSourceTest, CreateVideoSourceProxy)
{
    std::unique_ptr<rtc::Thread> workerThread = rtc::Thread::Create();
    workerThread->Start();
    std::unique_ptr<rtc::Thread> signalingThread = rtc::Thread::Create();
    signalingThread->Start();

    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSourceProxy =
        webrtc::VideoTrackSourceProxy::Create(
            signalingThread.get(),
            workerThread.get(), m_trackSource);
}

// todo::(kazuki) fix MetalGraphicsDevice.mm
#if !defined(SUPPORT_METAL)
TEST_P(VideoTrackSourceTest, SendTestFrame)
{
    EXPECT_CALL(mock_sink_, OnFrame(_))
        .WillOnce(Invoke([](const webrtc::VideoFrame& frame) {
            EXPECT_EQ(width, frame.width());
            EXPECT_EQ(height, frame.height());

            GpuResourceBuffer* buffer
                = static_cast<GpuResourceBuffer*>(frame.video_frame_buffer().get());
            EXPECT_NE(buffer, nullptr);
            rtc::scoped_refptr<I420BufferInterface> i420Buffer = buffer->ToI420();
            EXPECT_NE(i420Buffer, nullptr);
            CUarray array = buffer->ToArray();
            EXPECT_NE(array, nullptr);
    }));
    m_trackSource->Init();
    const int64_t timestamp_us =
        webrtc::Clock::GetRealTimeClock()->TimeInMicroseconds();
    m_trackSource->OnFrameCaptured(timestamp_us);
}
#endif

INSTANTIATE_TEST_CASE_P(
    GraphicsDeviceParameters,
    VideoTrackSourceTest,
    testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity

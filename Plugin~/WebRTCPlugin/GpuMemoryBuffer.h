#pragma once
#include "GraphicsDevice/GraphicsDevice.h"
#include "IUnityRenderingExtensions.h"
#include "Size.h"
#include "common_video/include/video_frame_buffer.h"
#include <shared_mutex>

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    class GpuMemoryBuffer
    {
    public:
        virtual ~GpuMemoryBuffer() { }
        virtual Size GetSize() const = 0;
        virtual rtc::scoped_refptr<I420BufferInterface> ToI420() = 0;
    };

    class GpuMemoryBufferFromUnity : public GpuMemoryBuffer
    {
    public:
        GpuMemoryBufferFromUnity(
            IGraphicsDevice* device,
            NativeTexPtr ptr,
            Size& size,
            UnityRenderingExtTextureFormat format);
        GpuMemoryBufferFromUnity(const GpuMemoryBufferFromUnity&) = delete;
        GpuMemoryBufferFromUnity& operator=(const GpuMemoryBufferFromUnity&) = delete;
        ~GpuMemoryBufferFromUnity() override;

        Size GetSize() const override;
        rtc::scoped_refptr<I420BufferInterface> ToI420() override;

    private:
        IGraphicsDevice* device_;
        std::unique_ptr<ITexture2D> texture_;
        Size size_;
    };

    class FakeGpuMemoryBuffer : public GpuMemoryBuffer
    {
    public:
        Size GetSize() const override { return Size(0, 0); }
        rtc::scoped_refptr<I420BufferInterface> ToI420() override { return nullptr; }
    };

}
}

#pragma once
#include "GraphicsDevice/GraphicsDevice.h"
#include "IUnityRenderingExtensions.h"
#include "Size.h"
#include "common_video/include/video_frame_buffer.h"
#include "rtc_base/ref_counted_object.h"
#include <shared_mutex>

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    class GpuMemoryBuffer : public rtc::RefCountInterface
    {
    public:
        virtual ~GpuMemoryBuffer() { }
        virtual Size GetSize() const = 0;
        virtual UnityRenderingExtTextureFormat GetFormat() const = 0;
        virtual rtc::scoped_refptr<I420BufferInterface> ToI420() = 0;
    };

    class GpuMemoryBufferFromUnity : public GpuMemoryBuffer
    {
    public:
        GpuMemoryBufferFromUnity(
            IGraphicsDevice* device,
            NativeTexPtr ptr,
            const Size& size,
            UnityRenderingExtTextureFormat format);
        GpuMemoryBufferFromUnity(const GpuMemoryBufferFromUnity&) = delete;
        GpuMemoryBufferFromUnity& operator=(const GpuMemoryBufferFromUnity&) = delete;
        ~GpuMemoryBufferFromUnity() override;

        void CopyBuffer(NativeTexPtr ptr);
        UnityRenderingExtTextureFormat GetFormat() const override;
        Size GetSize() const override;
        rtc::scoped_refptr<I420BufferInterface> ToI420() override;

    private:
        IGraphicsDevice* device_;
        UnityRenderingExtTextureFormat format_;
        Size size_;
        std::unique_ptr<ITexture2D> texture_;
    };

    class FakeGpuMemoryBuffer : public GpuMemoryBuffer
    {
    public:
        Size GetSize() const override { return Size(0, 0); }
        UnityRenderingExtTextureFormat GetFormat() const override
        {
            return kUnityRenderingExtFormatR8G8B8A8_SRGB;
        }
        rtc::scoped_refptr<I420BufferInterface> ToI420() override { return nullptr; }
    };

}
}

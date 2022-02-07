#pragma once
#include "GpuMemoryBuffer.h"
#include "Size.h"
#include "VideoFrame.h"

namespace unity
{
namespace webrtc
{
    class GpuMemoryBufferPool
    {
    public:
        GpuMemoryBufferPool(IGraphicsDevice* device);
        GpuMemoryBufferPool(const GpuMemoryBufferPool&) = delete;
        GpuMemoryBufferPool& operator=(const GpuMemoryBufferPool&) = delete;

        virtual ~GpuMemoryBufferPool();

        rtc::scoped_refptr<VideoFrame> CreateFrame(
            NativeTexPtr ptr,
            const Size& size,
            UnityRenderingExtTextureFormat format,
            int64_t timestamp);

        uint32_t bufferCount() { return resourcesPool_.size(); }

    private:
        struct FrameReources
        {
            FrameReources(rtc::scoped_refptr<GpuMemoryBuffer> buffer)
                : buffer_(std::move(buffer))
            {
            }
            rtc::scoped_refptr<GpuMemoryBuffer> buffer_;
            bool IsUsed() { return isUsed_; }
            void MarkUsed() { isUsed_ = true; }
            void MarkUnused() { isUsed_ = false; }
            bool isUsed_;
        };
        rtc::scoped_refptr<GpuMemoryBuffer> GetOrCreateFrameResources(
            NativeTexPtr ptr, const Size& size, UnityRenderingExtTextureFormat format);
        void OnReturnBuffer(rtc::scoped_refptr<GpuMemoryBuffer> buffer);
        void CopyBuffer();

        static bool AreFrameResourcesCompatible(
            const FrameReources* resources,
            const Size& size,
            UnityRenderingExtTextureFormat format);

        IGraphicsDevice* device_;
        std::list<std::unique_ptr<FrameReources>> resourcesPool_;
    };
}
}

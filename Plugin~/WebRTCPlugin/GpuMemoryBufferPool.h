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
            NativeTexPtr ptr, Size& size, UnityRenderingExtTextureFormat format, int64_t timestamp);

    private:
        struct FrameReources
        {
            FrameReources(std::unique_ptr<GpuMemoryBuffer> buffer)
                : buffer_(std::move(buffer))
            {
            }
            std::unique_ptr<GpuMemoryBuffer> buffer_;
            bool IsUsed() { return isUsed_; }
            void MarkUsed() { isUsed_ = true; }
            void MarkUnused() { isUsed_ = false; }
            bool isUsed_;
        };
        GpuMemoryBuffer* GetOrCreateFrameResources(
            NativeTexPtr ptr, Size& size, UnityRenderingExtTextureFormat format);
        void OnReturnBuffer(std::unique_ptr<GpuMemoryBuffer> buffer);
        void CopyBuffer();

        static bool AreFrameResourcesCompatible(const FrameReources* resources, const Size& size);

        IGraphicsDevice* device_;
        std::list<std::unique_ptr<FrameReources>> resourcesPool_;
    };
}
}

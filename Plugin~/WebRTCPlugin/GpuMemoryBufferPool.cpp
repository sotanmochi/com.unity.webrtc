#include "pch.h"

#include "GpuMemoryBufferPool.h"

namespace unity
{
namespace webrtc
{
    GpuMemoryBufferPool::GpuMemoryBufferPool(IGraphicsDevice* device)
        : device_(device)
    {
    }

    GpuMemoryBufferPool::~GpuMemoryBufferPool() {}

    rtc::scoped_refptr<VideoFrame> GpuMemoryBufferPool::CreateFrame(
        NativeTexPtr ptr,
        const Size& size,
        UnityRenderingExtTextureFormat format,
        int64_t timestamp)
    {
        GpuMemoryBuffer* buffer = GetOrCreateFrameResources(ptr, size, format);
        VideoFrame::ReturnBufferToPoolCallback callback =
            std::bind(&GpuMemoryBufferPool::OnReturnBuffer, this, std::placeholders::_1);

        return VideoFrame::WrapExternalGpuMemoryBuffer(
            size,
            std::unique_ptr<GpuMemoryBuffer>(buffer),
            callback,
            webrtc::TimeDelta::Micros(timestamp));
    }

    GpuMemoryBuffer* GpuMemoryBufferPool::GetOrCreateFrameResources(
        NativeTexPtr ptr, const Size& size, UnityRenderingExtTextureFormat format)
    {
        auto it = resourcesPool_.begin();
        while (it != resourcesPool_.end())
        {
            FrameReources* resources = it->get();
            if (!resources->IsUsed() && AreFrameResourcesCompatible(resources, size))
            {
                resources->MarkUsed();
                return resources->buffer_.get();
            }
            else
            {
                it++;
            }
        }

        auto buffer = std::make_unique<GpuMemoryBufferFromUnity>(device_, ptr, size, format);
        GpuMemoryBuffer* ret = buffer.get();
        std::unique_ptr<FrameReources> resources =
            std::make_unique<FrameReources>(std::move(buffer));
        resourcesPool_.push_back(std::move(resources));
        return ret;
    }

    bool GpuMemoryBufferPool::AreFrameResourcesCompatible(
        const FrameReources* resources, const Size& size)
    {
        return resources->buffer_->GetSize() == size;
    }

    void GpuMemoryBufferPool::OnReturnBuffer(std::unique_ptr<GpuMemoryBuffer> buffer)
    {
        GpuMemoryBuffer* ptr = buffer.release();
        auto result = std::find_if(
            resourcesPool_.begin(),
            resourcesPool_.end(),
            [ptr](std::unique_ptr<FrameReources>& x) { return x->buffer_.get() == ptr; });
        RTC_DCHECK(result != resourcesPool_.end());

        (*result)->MarkUnused();
    }

}
}

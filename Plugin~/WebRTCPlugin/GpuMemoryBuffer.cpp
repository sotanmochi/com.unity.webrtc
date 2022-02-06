#include "pch.h"

#include "GpuMemoryBuffer.h"
#include "GraphicsDevice/ITexture2D.h"

namespace unity
{
namespace webrtc
{

    GpuMemoryBufferFromUnity::GpuMemoryBufferFromUnity(
        IGraphicsDevice* device,
        NativeTexPtr ptr,
        Size& size,
        UnityRenderingExtTextureFormat format)
        : device_(device)
        , size_(size)
        , texture_(nullptr)
    {
        texture_.reset(device_->CreateCPUReadTextureV(size.width(), size.height(), format));
        device_->CopyResourceFromNativeV(texture_.get(), ptr);
    }

    Size GpuMemoryBufferFromUnity::GetSize() const { return size_; }

    rtc::scoped_refptr<I420BufferInterface> GpuMemoryBufferFromUnity::ToI420()
    {
        return device_->ConvertRGBToI420(texture_.get());
    }

    GpuMemoryBufferFromUnity::~GpuMemoryBufferFromUnity() { }

}
}

#include "pch.h"
#include "GpuMemoryBuffer.h"
#include <shared_mutex>
#include "GraphicsDevice/ITexture2D.h"

namespace unity
{
namespace webrtc
{

GpuMemoryBufferFromUnity::GpuMemoryBufferFromUnity(
    IGraphicsDevice* device, NativeTexPtr ptr,
    Size& size, UnityRenderingExtTextureFormat format)
    : device_(device)
    , ptr_(ptr)
    , texture_(nullptr)
{
    texture_.reset(device_->CreateCPUReadTextureV(size.width(), size.height(), format));

    // todo
    device_->CopyResourceFromNativeV(texture_.get(), ptr_);
    i420buffer_ = device_->ConvertRGBToI420(texture_.get());
}

rtc::scoped_refptr<I420BufferInterface> GpuMemoryBufferFromUnity::ToI420()
{
    //device_->CopyResourceFromNativeV(texture_, ptr_);
    //return device_->ConvertRGBToI420(texture_.get());
    return i420buffer_;
}

GpuMemoryBufferFromUnity::~GpuMemoryBufferFromUnity()
{}

}
}

#include "pch.h"
#include "GpuMemoryBuffer.h"
#include <shared_mutex>

namespace unity
{
namespace webrtc
{


GpuMemoryBuffer::~GpuMemoryBuffer()
{
}

GpuMemoryBufferFromUnity::GpuMemoryBufferFromUnity(
    IGraphicsDevice* device, NativeTexPtr ptr,
    Size& size, UnityRenderingExtTextureFormat format)
    : device_(device)
    , ptr_(ptr)
    , texture_(nullptr)
{
    texture_ = device_->CreateCPUReadTextureV(
        size.width(), size.height(), format);
}

rtc::scoped_refptr<I420BufferInterface> GpuMemoryBufferFromUnity::ToI420()
{
    device_->CopyResourceFromNativeV(texture_, ptr_);
    return device_->ConvertRGBToI420(texture_);
}

GpuMemoryBufferFromUnity::~GpuMemoryBufferFromUnity()
{}

//::webrtc::VideoFrameBuffer::Type GpuMemoryBuffer::type() const
//{
//    // todo::(kazuki)
//    // should return Type::kNative
//    // but memory in this class is broken when return Type::kNative
//    return m_buffer->type();
//}
//
//int GpuMemoryBuffer::width() const
//{
//    return m_buffer->width();
//}
//int GpuMemoryBuffer::height() const
//{
//    return m_buffer->height();
//}

std::shared_timed_mutex* GpuMemoryBuffer::mutex() const
{
    return m_mutex;
}


//rtc::scoped_refptr<I420BufferInterface> GpuMemoryBuffer::ToI420()
//{
//    return m_buffer;
//}

}
}

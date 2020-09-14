#include "pch.h"
#include "GpuResourceBuffer.h"
#include <shared_mutex>

namespace unity
{
namespace webrtc
{

GpuResourceBuffer::GpuResourceBuffer(
    const rtc::scoped_refptr<I420BufferInterface>& buffer,
    CUdeviceptr devicePtr, std::shared_mutex& mutex)
    : m_buffer(buffer)
    , m_mutex(&mutex)
    , m_devicePtr(devicePtr)
    , m_memoryType(CU_MEMORYTYPE_DEVICE)
{
}

GpuResourceBuffer::GpuResourceBuffer(
    const rtc::scoped_refptr<I420BufferInterface>& buffer,
    CUarray array, std::shared_mutex& mutex)
    : m_buffer(buffer)
    , m_mutex(&mutex)
    , m_array(array)
    , m_memoryType(CU_MEMORYTYPE_ARRAY)
{
}

GpuResourceBuffer::~GpuResourceBuffer()
{
}

::webrtc::VideoFrameBuffer::Type GpuResourceBuffer::type() const
{
    // todo::(kazuki)
    // should return Type::kNative
    // but memory in this class is broken when return Type::kNative
    return m_buffer->type();
}

int GpuResourceBuffer::width() const
{
    return m_buffer->width();
}
int GpuResourceBuffer::height() const
{
    return m_buffer->height();
}

std::shared_mutex* GpuResourceBuffer::mutex() const
{
    return m_mutex;
}

CUstream GpuResourceBuffer::ToStream() const
{
    return nullptr;
}

CUdeviceptr GpuResourceBuffer::ToDevicePtr() const
{
    return m_devicePtr;
}

CUarray GpuResourceBuffer::ToArray() const
{
    return m_array;
}

CUmemorytype GpuResourceBuffer::memoryType() const
{
    return m_memoryType;
}

rtc::scoped_refptr<I420BufferInterface> GpuResourceBuffer::ToI420()
{
    return m_buffer;
}

}
}

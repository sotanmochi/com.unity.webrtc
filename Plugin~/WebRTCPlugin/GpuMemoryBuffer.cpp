#include "pch.h"
#include "GpuMemoryBuffer.h"
#include <shared_mutex>

namespace unity
{
namespace webrtc
{

GpuMemoryBuffer::GpuMemoryBuffer(
    CUdeviceptr devicePtr, std::shared_timed_mutex& mutex)
    : m_mutex(&mutex)
    , m_devicePtr(devicePtr)
    , m_memoryType(CU_MEMORYTYPE_DEVICE)
{
}

GpuMemoryBuffer::GpuMemoryBuffer(
    CUarray array, std::shared_timed_mutex& mutex)
    : m_mutex(&mutex)
    , m_array(array)
    , m_memoryType(CU_MEMORYTYPE_ARRAY)
{
}

GpuMemoryBuffer::~GpuMemoryBuffer()
{
}

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

CUstream GpuMemoryBuffer::ToStream() const
{
    return nullptr;
}

CUdeviceptr GpuMemoryBuffer::ToDevicePtr() const
{
    return m_devicePtr;
}

CUarray GpuMemoryBuffer::ToArray() const
{
    return m_array;
}

CUmemorytype GpuMemoryBuffer::memoryType() const
{
    return m_memoryType;
}

//rtc::scoped_refptr<I420BufferInterface> GpuMemoryBuffer::ToI420()
//{
//    return m_buffer;
//}

}
}

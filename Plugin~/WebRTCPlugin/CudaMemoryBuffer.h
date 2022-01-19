#pragma once
#include <cuda.h>
#include <shared_mutex>

#include "GpuMemoryBuffer.h"

namespace unity
{
namespace webrtc
{
using namespace ::webrtc;

class CudaMemoryBuffer : public GpuMemoryBuffer
{
public:
    CudaMemoryBuffer(CUarray array, std::shared_timed_mutex& mutex)
        : m_array(array)
        , m_mutex(&mutex) {}
    std::shared_timed_mutex* mutex() const { return m_mutex; }
//    CUstream ToStream() const;
    CUdeviceptr ToDevicePtr() const { return m_devicePtr; }
    CUarray ToArray() const { return m_array; }
    rtc::scoped_refptr<I420BufferInterface> ToI420() override;
private:
    CUdeviceptr m_devicePtr;
    CUarray m_array;
    CUmemorytype m_memoryType;
    std::shared_timed_mutex* m_mutex;
};

}
}

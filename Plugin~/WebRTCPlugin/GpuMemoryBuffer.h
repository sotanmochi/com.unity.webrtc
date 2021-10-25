#pragma once
#include <cuda.h>
#include <shared_mutex>
#include "common_video/include/video_frame_buffer.h"

namespace unity
{
namespace webrtc
{
using namespace ::webrtc;

class GpuMemoryBuffer
{
public:
    explicit GpuMemoryBuffer() {};
    GpuMemoryBuffer(CUdeviceptr devicePtr, std::shared_timed_mutex& mutex);
    GpuMemoryBuffer(CUarray array, std::shared_timed_mutex& mutex);
    virtual ~GpuMemoryBuffer();

    std::shared_timed_mutex* mutex() const;
    CUmemorytype memoryType() const;
    int index() const;

    CUstream ToStream() const;
    CUdeviceptr ToDevicePtr() const;
    CUarray ToArray() const;

    //
    // todo::(kazuki)
    // type member always return Type::kI420 but not support I420
    // should return Type::kNative
    // but memory in this class is broken when return Type::kNative
    //Type type() const final;
    //int width() const final;
    //int height() const final;
    //rtc::scoped_refptr<I420BufferInterface> ToI420() final;
private:
    rtc::scoped_refptr<I420BufferInterface> m_buffer;
    std::shared_timed_mutex* m_mutex;
    CUdeviceptr m_devicePtr;
    CUarray m_array;
    CUmemorytype m_memoryType;
};

class FakeGpuMemoryBuffer : public GpuMemoryBuffer {
public:
    explicit FakeGpuMemoryBuffer() {};
    ~FakeGpuMemoryBuffer() override {}
};

}
}

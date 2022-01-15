#pragma once
#include <cuda.h>
#include <shared_mutex>
#include "Size.h"
#include "GraphicsDevice/GraphicsDevice.h"
#include "common_video/include/video_frame_buffer.h"

namespace unity
{
namespace webrtc
{
using namespace ::webrtc;

class GpuMemoryBuffer
{
public:
    virtual ~GpuMemoryBuffer() {}

    //std::shared_timed_mutex* mutex() const;
    int index() const;


    //
    // todo::(kazuki)
    // type member always return Type::kI420 but not support I420
    // should return Type::kNative
    // but memory in this class is broken when return Type::kNative
    //Type type() const final;
    //int width() const final;
    //int height() const final;
    virtual rtc::scoped_refptr<I420BufferInterface> ToI420() = 0;
private:
    //std::shared_timed_mutex* m_mutex;
};

class GpuMemoryBufferFromUnity : public GpuMemoryBuffer
{
public:
    GpuMemoryBufferFromUnity(
        IGraphicsDevice* device, NativeTexPtr ptr,
        Size& size, UnityRenderingExtTextureFormat format);
    rtc::scoped_refptr<I420BufferInterface> ToI420() override;
    ~GpuMemoryBufferFromUnity() override;
private:
    IGraphicsDevice* device_;
    NativeTexPtr ptr_;
    std::unique_ptr<ITexture2D> texture_;

    rtc::scoped_refptr<I420BufferInterface> i420buffer_;
};

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

class FakeGpuMemoryBuffer : public GpuMemoryBuffer
{
public:
    rtc::scoped_refptr<I420BufferInterface> ToI420() override { return nullptr; }
};

}
}

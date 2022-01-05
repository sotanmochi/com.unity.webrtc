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
    explicit GpuMemoryBuffer() {};
    virtual ~GpuMemoryBuffer();

    std::shared_timed_mutex* mutex() const;
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
    //rtc::scoped_refptr<I420BufferInterface> m_buffer;
    std::shared_timed_mutex* m_mutex;
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
    ITexture2D* texture_;
};

class CudaMemoryBuffer : public GpuMemoryBuffer
{
public:
    CUstream ToStream() const;
    CUdeviceptr ToDevicePtr() const;
    CUarray ToArray() const;
    rtc::scoped_refptr<I420BufferInterface> ToI420() override;
private:
    CUdeviceptr m_devicePtr;
    CUarray m_array;
    CUmemorytype m_memoryType;
};

class FakeGpuMemoryBuffer : public GpuMemoryBuffer
{
public:
    explicit FakeGpuMemoryBuffer() {};
    ~FakeGpuMemoryBuffer() override {}
};

}
}

#pragma once
#if UNITY_WIN || UNITY_LINUX
#include <cuda.h>
#endif 

#if UNITY_WIN || UNITY_LINUX || UNITY_ANDROID
#include "GraphicsDevice/Vulkan/VulkanGraphicsDevice.h"
#endif

#include <shared_mutex>
#include "GraphicsDevice/IGraphicsDevice.h"

using namespace webrtc;

namespace unity {
namespace webrtc {

class VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorInterface(
        IGraphicsDevice* device, void* frame, UnityRenderingExtTextureFormat format, uint32_t memoryType);
    virtual ~VideoFrameBufferCreatorInterface();
    virtual void Init() = 0;
    virtual rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_timed_mutex& mutex) = 0;
public:
    static VideoFrameBufferCreatorInterface* Create(
        IGraphicsDevice* device, NativeTexPtr ptr, UnityGfxRenderer renderer, UnityRenderingExtTextureFormat format, uint32_t memoryType);
protected:
    int m_width;
    int m_height;
    IGraphicsDevice* m_device;
    UnityRenderingExtTextureFormat m_format;
    void* m_frame;
    bool m_useCpu;
    bool m_useGpu;
};

#if UNITY_WIN
class VideoFrameBufferCreatorD3D11 : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorD3D11(
        IGraphicsDevice* m_device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t memoryType);
    virtual ~VideoFrameBufferCreatorD3D11();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_timed_mutex& mutex) override;
private:
    rtc::scoped_refptr<I420Buffer> CreateI420Buffer(ITexture2D* tex);

    CUgraphicsResource m_resource;
    CUarray m_mappedArray;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
    std::unique_ptr<ITexture2D> m_cpuReadTexture;
    rtc::scoped_refptr<I420Buffer> m_dummyBuffer;
};

class VideoFrameBufferCreatorD3D12 : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorD3D12(
        IGraphicsDevice* m_device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t memoryType);
    virtual ~VideoFrameBufferCreatorD3D12();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_timed_mutex& mutex) override;
private:
    rtc::scoped_refptr<I420Buffer> CreateI420Buffer(ITexture2D* tex);

    CUgraphicsResource m_resource;
    CUarray m_mappedArray;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
    std::unique_ptr<ITexture2D> m_cpuReadTexture;
    rtc::scoped_refptr<I420Buffer> m_dummyBuffer;
};
#endif

#if UNITY_LINUX || UNITY_ANDROID || UNITY_WIN
class VideoFrameBufferCreatorVulkan : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorVulkan(
        IGraphicsDevice* m_device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t memoryType);
    virtual ~VideoFrameBufferCreatorVulkan();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_timed_mutex& mutex) override;
private:
    rtc::scoped_refptr<I420Buffer> CreateI420Buffer(ITexture2D* tex);

    UnityVulkanImage* m_image;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
    std::unique_ptr<ITexture2D> m_cpuReadTexture;
    VulkanGraphicsDevice* m_vulkanDevice;
    rtc::scoped_refptr<I420Buffer> m_dummyBuffer;
};
#endif

#if UNITY_OSX || UNITY_IOS
class VideoFrameBufferCreatorMetal : VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorMetal(
        IGraphicsDevice* m_device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t memoryType);
    virtual ~VideoFrameBufferCreatorMetal();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_timed_mutex& mutex) override;
};
#endif

#if UNITY_LINUX || UNITY_ANDROID
class VideoFrameBufferCreatorOpenGL : public VideoFrameBufferCreatorInterface
{
public:
    VideoFrameBufferCreatorOpenGL(
        IGraphicsDevice* m_device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t memoryType);
    virtual ~VideoFrameBufferCreatorOpenGL();

    void Init() override;
    rtc::scoped_refptr<VideoFrameBuffer> CreateBuffer(
        std::shared_timed_mutex& mutex) override;
private:
    CUgraphicsResource m_resource;
    CUarray m_mappedArray;
    std::unique_ptr<ITexture2D> m_gpuReadTexture;
};
#endif
} // end namespace webrtc
} // end namespace unity

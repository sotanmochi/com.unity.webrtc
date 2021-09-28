#include "pch.h"
#include <cuda.h>
#include "GpuResourceBuffer.h"
#include "../NvCodec/Utils/NvCodecUtils.h"
#include "VideoFrameBufferCreatorInterface.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"
#include "GraphicsDevice/Vulkan/VulkanGraphicsDevice.h"

namespace unity
{
namespace webrtc
{

VideoFrameBufferCreatorVulkan::VideoFrameBufferCreatorVulkan(
    IGraphicsDevice* device, void* frame, UnityRenderingExtTextureFormat format, uint32_t destMemoryType)
    : VideoFrameBufferCreatorInterface(device, frame, format, destMemoryType)
    , m_image(static_cast<UnityVulkanImage*>(frame))
    , m_gpuReadTexture(nullptr)
    , m_vulkanDevice(static_cast<VulkanGraphicsDevice*>(device))
    , m_dummyBuffer(nullptr)
{
    m_width = m_image->extent.width;
    m_height = m_image->extent.height;

    std::unique_ptr<ITexture2D> tex(device->CreateDefaultTextureV(m_width, m_height, m_format));
    m_gpuReadTexture = std::move(tex);
    m_dummyBuffer = I420Buffer::Create(m_width, m_height);
}

void VideoFrameBufferCreatorVulkan::Init()
{
    if (m_useGpu)
    {
        if (m_device->GetCuContext() == nullptr)
        {
            throw;
        }
    }

    // todo::(kazuki)
    //CUcontext context = m_device->GetCudaContext().GetContext();

    //CUcontext current;
    //if (!ck(cuCtxGetCurrent(&current)))
    //{
    //    throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    //}
    //if (current != context)
    //{
    //    if (!ck(cuCtxSetCurrent(context)))
    //    {
    //        throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    //    }
    //}
}

VideoFrameBufferCreatorVulkan::~VideoFrameBufferCreatorVulkan()
{

}

rtc::scoped_refptr<I420Buffer> VideoFrameBufferCreatorVulkan::CreateI420Buffer(
    ITexture2D* tex)
{
    return m_dummyBuffer;
    //if (!m_useCpu)
    //{
    //    return m_dummyBuffer;
    //}
    //return m_device->ConvertRGBToI420(tex);
}

rtc::scoped_refptr<VideoFrameBuffer> VideoFrameBufferCreatorVulkan::CreateBuffer(
    std::shared_mutex& mutex)
{
    // texture copy for hardware encoder
    if (m_useGpu)
    {
        if (!m_device->CopyResourceFromNativeV(
            m_gpuReadTexture.get(), m_frame))
        {
            throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }
    }

    // texture copy for software encoder 
    //if (m_useGpu)
    //{
    //    if (!m_device->CopyResourceFromNativeV(
    //        m_cpuReadTexture.get(), m_frame))
    //    {
    //        throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    //    }
    //}

    // i420 buffer for software encoder
    rtc::scoped_refptr<I420Buffer> buffer = CreateI420Buffer(
        m_cpuReadTexture.get());

    CUarray array = static_cast<CUarray>(m_gpuReadTexture->GetEncodeTexturePtrV());
    return new rtc::RefCountedObject<GpuResourceBuffer>(buffer, array, mutex);
}

} // end namespace webrtc
} // end namespace unity

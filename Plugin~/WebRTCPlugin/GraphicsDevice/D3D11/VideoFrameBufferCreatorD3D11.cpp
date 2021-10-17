#include "pch.h"
#include <cuda.h>
#include <cudaD3D11.h>

#include "NvCodecUtils.h"
#include "GpuResourceBuffer.h"
#include "VideoFrameBufferCreatorInterface.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"

namespace unity
{
namespace webrtc
{

VideoFrameBufferCreatorD3D11::VideoFrameBufferCreatorD3D11(
    IGraphicsDevice* device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t destMemoryType)
    : VideoFrameBufferCreatorInterface(device, ptr, format, destMemoryType)
    , m_resource(nullptr)
    , m_mappedArray(nullptr)
    , m_dummyBuffer(nullptr)
{
    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D* tex = static_cast<ID3D11Texture2D*>(ptr);
    tex->GetDesc(&desc);
    m_width = desc.Width;
    m_height = desc.Height;
    m_dummyBuffer = I420Buffer::Create(m_width, m_height);
}

void VideoFrameBufferCreatorD3D11::Init()
{
    if(m_useCpu)
    {
        std::unique_ptr<ITexture2D> tex2(m_device->CreateCPUReadTextureV(m_width, m_height, m_format));
        m_cpuReadTexture = std::move(tex2);
    }

    if(m_useGpu)
    {
        if (m_device->GetCuContext() == nullptr)
        {
            throw;
        }

        std::unique_ptr<ITexture2D> tex(m_device->CreateDefaultTextureV(m_width, m_height, m_format));
        m_gpuReadTexture = std::move(tex);

        ID3D11Texture2D* pD3DResource =
            static_cast<ID3D11Texture2D*>(m_gpuReadTexture->GetNativeTexturePtrV());

        D3D11_TEXTURE2D_DESC desc;
        pD3DResource->GetDesc(&desc);

        CUresult result = cuGraphicsD3D11RegisterResource(&m_resource,
            pD3DResource, CU_GRAPHICS_REGISTER_FLAGS_SURFACE_LDST);
        if (!ck(result))
        {
            throw;
        }

        result = cuGraphicsMapResources(1, &m_resource, 0);
        if (!ck(result))
        {
            throw;
        }

        result = cuGraphicsSubResourceGetMappedArray(&m_mappedArray, m_resource, 0, 0);
        if (!ck(result))
        {
            throw;
        }
    }
}

VideoFrameBufferCreatorD3D11::~VideoFrameBufferCreatorD3D11()
{
    cuGraphicsUnmapResources(1, &m_resource, 0);

    if (m_resource != nullptr)
    {
        cuGraphicsUnregisterResource(m_resource);
        m_resource = nullptr;
    }
};

rtc::scoped_refptr<I420Buffer> VideoFrameBufferCreatorD3D11::CreateI420Buffer(
    ITexture2D* tex)
{
    if(!m_useCpu)
    {
        return m_dummyBuffer;
    }
    return m_device->ConvertRGBToI420(tex);
}

rtc::scoped_refptr<VideoFrameBuffer>
VideoFrameBufferCreatorD3D11::CreateBuffer(std::shared_timed_mutex& mutex)
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
    if (m_useGpu)
    {
        if (!m_device->CopyResourceFromNativeV(
            m_cpuReadTexture.get(), m_frame))
        {
            throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }
    }

    // i420 buffer for software encoder
    rtc::scoped_refptr<I420Buffer> buffer = CreateI420Buffer(
        m_cpuReadTexture.get());

    return new rtc::RefCountedObject<GpuResourceBuffer>(
        buffer, m_mappedArray, mutex);
}

} // end namespace webrtc
} // end namespace unity

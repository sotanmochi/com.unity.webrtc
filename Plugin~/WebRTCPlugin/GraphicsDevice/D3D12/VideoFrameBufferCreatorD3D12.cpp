#include "pch.h"
#include <cuda.h>
#include <cudaD3D11.h>

#include "D3D12GraphicsDevice.h"
#include "GpuResourceBuffer.h"
#include "VideoFrameBufferCreatorInterface.h"
#include "../NvCodec/Utils/NvCodecUtils.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"

namespace unity
{
namespace webrtc
{

VideoFrameBufferCreatorD3D12::VideoFrameBufferCreatorD3D12(
    IGraphicsDevice* device, NativeTexPtr ptr, UnityRenderingExtTextureFormat format, uint32_t destMemoryType)
    : VideoFrameBufferCreatorInterface(device, ptr, format, destMemoryType)
    , m_resource(nullptr)
    , m_mappedArray(nullptr)
    , m_dummyBuffer(nullptr)
{
    ID3D12Resource* tex = static_cast<ID3D12Resource*>(ptr);
    D3D12_RESOURCE_DESC desc = tex->GetDesc();
    m_width = desc.Width;
    m_height = desc.Height;
    m_dummyBuffer = I420Buffer::Create(m_width, m_height);
}

void VideoFrameBufferCreatorD3D12::Init()
{
    if (m_useCpu)
    {
        std::unique_ptr<ITexture2D> tex(m_device->CreateCPUReadTextureV(m_width, m_height, m_format));
        m_cpuReadTexture = std::move(tex

        );
    }

    if(m_useGpu)
    {
        if (m_device->GetCuContext() == nullptr)
        {
            throw;
        }

        std::unique_ptr<ITexture2D> tex(m_device->CreateDefaultTextureV(m_width, m_height, m_format));
        m_gpuReadTexture = std::move(tex);

        //ID3D11Texture2D* pD3DResource =
        //    static_cast<ID3D11Texture2D*>(m_gpuReadTexture->GetEncodeTexturePtrV());

        //D3D11_TEXTURE2D_DESC desc;
        //pD3DResource->GetDesc(&desc);
        //
        D3D12GraphicsDevice* pD3D12Device = static_cast<D3D12GraphicsDevice*>(m_device);
        ID3D11Texture2D* pD3DResource = pD3D12Device->GetTempTexture(m_width, m_height);

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

VideoFrameBufferCreatorD3D12::~VideoFrameBufferCreatorD3D12()
{
    if(m_useGpu)
    {
        cuGraphicsUnmapResources(1, &m_resource, 0);

        if (m_resource != nullptr)
        {
            cuGraphicsUnregisterResource(m_resource);
            m_resource = nullptr;
        }
    }
};

rtc::scoped_refptr<I420Buffer> VideoFrameBufferCreatorD3D12::CreateI420Buffer(
    ITexture2D* tex)
{
    if (!m_useCpu)
    {
        return m_dummyBuffer;
    }
    return m_device->ConvertRGBToI420(tex);
}

rtc::scoped_refptr<VideoFrameBuffer>
VideoFrameBufferCreatorD3D12::CreateBuffer(std::shared_mutex& mutex)
{
    // texture copy for hardware encoder
    if(m_useGpu)
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
    rtc::scoped_refptr<I420Buffer> buffer = CreateI420Buffer(
        m_cpuReadTexture.get());

    return new rtc::RefCountedObject<GpuResourceBuffer>(buffer, m_mappedArray, mutex);
}

} // end namespace webrtc
} // end namespace unity

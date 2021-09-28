#include "pch.h"
#include "VideoFrameBufferCreatorInterface.h"
#include "UnityVideoTrackSource.h"

namespace unity
{
namespace webrtc
{

VideoFrameBufferCreatorInterface::VideoFrameBufferCreatorInterface(
	IGraphicsDevice* device, void* frame, UnityRenderingExtTextureFormat format, uint32_t memoryType)
    : m_width(0)
    , m_height(0)
    , m_device(device)
    , m_format(format)
    , m_frame(frame)
    , m_useCpu(memoryType & CPU_MEMORY)
    , m_useGpu(memoryType & GPU_MEMORY)
{
    RTC_DCHECK(m_device);
    RTC_DCHECK(frame);
}

VideoFrameBufferCreatorInterface::~VideoFrameBufferCreatorInterface() = default;

VideoFrameBufferCreatorInterface* VideoFrameBufferCreatorInterface::Create(
    IGraphicsDevice* device, NativeTexPtr ptr, UnityGfxRenderer renderer,
    UnityRenderingExtTextureFormat format, uint32_t memoryType)
{
    switch (renderer)
    {
#if defined(UNITY_WIN)
    case kUnityGfxRendererD3D11:
        return new VideoFrameBufferCreatorD3D11(device, ptr, format, memoryType);
    case kUnityGfxRendererD3D12:
        return new VideoFrameBufferCreatorD3D12(device, ptr, format, memoryType);
#endif
#if defined(UNITY_LINUX) || defined(UNITY_WIN)
    case kUnityGfxRendererVulkan:
        return new VideoFrameBufferCreatorVulkan(device, ptr, format, memoryType);
#endif
#if defined(UNITY_LINUX)
    case kUnityGfxRendererOpenGLCore:
        return new VideoFrameBufferCreatorOpenGL(device, format, frame);
#endif
    }
}
}
}

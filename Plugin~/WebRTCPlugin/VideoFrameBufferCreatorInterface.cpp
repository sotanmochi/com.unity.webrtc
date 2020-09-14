#include "pch.h"
#include "VideoFrameBufferCreatorInterface.h"
#include "UnityVideoTrackSource.h"

namespace unity
{
namespace webrtc
{

VideoFrameBufferCreatorInterface::VideoFrameBufferCreatorInterface(
	IGraphicsDevice* device, void* frame, uint32_t destMemoryType)
    : m_width(0)
    , m_height(0)
    , m_device(device)
    , m_frame(frame)
    , m_useCpu(destMemoryType & CPU_MEMORY)
    , m_useGpu(destMemoryType & GPU_MEMORY)
{
    RTC_DCHECK(m_device);
    RTC_DCHECK(frame);
}

VideoFrameBufferCreatorInterface::~VideoFrameBufferCreatorInterface() = default;

VideoFrameBufferCreatorInterface* VideoFrameBufferCreatorInterface::Create(
    IGraphicsDevice* device, NativeTexPtr ptr, UnityGfxRenderer renderer, uint32_t destMemoryType)
{
    switch (renderer)
    {
#if defined(UNITY_WIN)
    case kUnityGfxRendererD3D11:
        return new VideoFrameBufferCreatorD3D11(device, ptr, destMemoryType);
    case kUnityGfxRendererD3D12:
        return new VideoFrameBufferCreatorD3D12(device, ptr, destMemoryType);
#endif
#if defined(UNITY_LINUX) || defined(UNITY_WIN)
    case kUnityGfxRendererVulkan:
        return new VideoFrameBufferCreatorVulkan(device, ptr, destMemoryType);
#endif
#if defined(UNITY_LINUX)
    case kUnityGfxRendererOpenGLCore:
        return new VideoFrameBufferCreatorOpenGL(device, frame);
#endif
    }
}
}
}

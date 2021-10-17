#include "pch.h"
#include "VideoFrameBufferCreatorInterface.h"
#include "GraphicsDevice/IGraphicsDevice.h"
#include "GraphicsDevice/ITexture2D.h"

#if CUDA_PLATFORM
#include <cuda.h>
#include <cudaGL.h>
#include "GpuResourceBuffer.h"
#include "../NvCodec/Utils/NvCodecUtils.h"
#endif

namespace unity
{
namespace webrtc
{

VideoFrameBufferCreatorOpenGL::VideoFrameBufferCreatorOpenGL(
    IGraphicsDevice* device, void* frame)
    : VideoFrameBufferCreatorInterface(device, frame)
    , m_resource(nullptr)
    , m_mappedArray(nullptr)
    , m_texture(nullptr)
{

}

void VideoFrameBufferCreatorOpenGL::Init()
{
    // Note.
    // OpenGL API must be called from render thread.

    //GLuint texture = reinterpret_cast<GLuint>(m_frame);

    //glBindTexture(GL_TEXTURE_2D, texture);

    //GLint width;
    //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    //GLint height;
    //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    //m_width = width;
    //m_height = height;

    //glBindTexture(GL_TEXTURE_2D, 0);



    //CUcontext context = m_device->GetCuContext();

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

    //std::unique_ptr<ITexture2D> tex(m_device->CreateDefaultTextureV(m_width, m_height));
    //m_texture = std::move(tex);

    //GLuint image = reinterpret_cast<GLuint>(m_texture->GetNativeTexturePtrV());

    //CUresult result = cuGraphicsGLRegisterImage(&m_resource,
    //    image, GL_TEXTURE_2D, CU_GRAPHICS_REGISTER_FLAGS_SURFACE_LDST);
    //if (!ck(result))
    //{
    //    throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    //}

    //result = cuGraphicsMapResources(1, &m_resource, 0);
    //if (!ck(result))
    //{
    //    throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    //}

    //result = cuGraphicsSubResourceGetMappedArray(&m_mappedArray, m_resource, 0, 0);
    //if (!ck(result))
    //{
    //    throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    //}
}

VideoFrameBufferCreatorOpenGL::~VideoFrameBufferCreatorOpenGL()
{

};

rtc::scoped_refptr<VideoFrameBuffer> VideoFrameBufferCreatorOpenGL::CreateBuffer(
    std::shared_timed_mutex& mutex)
{
    if(!m_device->CopyResourceFromNativeV(m_texture.get(), m_frame))
    {
        throw WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    }
    return new rtc::RefCountedObject<GpuResourceBuffer>(nullptr, m_mappedArray, mutex);
}

} // end namespace webrtc
} // end namespace unity

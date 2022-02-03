#include "pch.h"
#include "NvDecoderImpl.h"
#include "NvDecoder/NvDecoder.h"
#include "../Utils/NvCodecUtils.h"
#include "api/video/video_codec_type.h"

using namespace webrtc;

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    NvDecoderImpl::NvDecoderImpl(CUcontext context)
    : m_context(context)
    , m_decoder(nullptr)
    {
    }

    NvDecoderImpl::~NvDecoderImpl()
    {
        Release();
    }

    int NvDecoderImpl::InitDecode(const VideoCodec* codec_settings, int32_t number_of_cores)
    {
        if (codec_settings == nullptr)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }

        if (codec_settings->codecType != kVideoCodecH264)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }
        if (codec_settings->maxFramerate == 0)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }
        if (codec_settings->width < 1 || codec_settings->height < 1)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }

        m_codec = *codec_settings;

        const CUresult result = cuCtxSetCurrent(m_context);
        if(!ck(result))
        {
            return WEBRTC_VIDEO_CODEC_ENCODER_FAILURE;
        }

        // todo implement init decoder
        return WEBRTC_VIDEO_CODEC_OK;
    }

    int32_t NvDecoderImpl::RegisterDecodeCompleteCallback(DecodedImageCallback* callback)
    {
        this->m_decodedCompleteCallback = callback;
        return WEBRTC_VIDEO_CODEC_OK;
    }

    int32_t NvDecoderImpl::Release()
    {
        this->m_decodedCompleteCallback = nullptr;
        return WEBRTC_VIDEO_CODEC_OK;
    }

    int32_t NvDecoderImpl::Decode(const EncodedImage& input_image, bool missing_frames, int64_t render_time_ms)
    {
        CUcontext current;
        if(!ck(cuCtxGetCurrent(&current)))
        {
            return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
        }
        if(current != m_context)
        {
            return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
        }
        if (m_decodedCompleteCallback == nullptr)
        {
            return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
        }

        // todo implement decode process
        return WEBRTC_VIDEO_CODEC_OK;
    }

} // end namespace webrtc
} // end namespace unity

#pragma once
#include <cuda.h>
#include <NvDecoder/NvDecoder.h>
#include "NvCodec.h"

using namespace webrtc;

namespace unity
{
namespace webrtc
{

using NvDecoderInternal = ::NvDecoder;

class NvDecoderImpl : public unity::webrtc::NvDecoder
{
public:
    NvDecoderImpl();
    NvDecoderImpl(CUcontext context);
    NvDecoderImpl(const NvDecoderImpl&) = delete;
    NvDecoderImpl& operator=(const NvDecoderImpl&) = delete;
    ~NvDecoderImpl() override;

    virtual int32_t InitDecode(const VideoCodec* codec_settings, int32_t number_of_cores) override;
    virtual int32_t Decode(const EncodedImage& input_image, bool missing_frames, int64_t render_time_ms) override;
    virtual int32_t RegisterDecodeCompleteCallback(DecodedImageCallback* callback) override;
    virtual int32_t Release() override;

private:
    CUcontext m_context;
    std::unique_ptr<NvDecoderInternal> m_decoder;

    VideoCodec m_codec;

    DecodedImageCallback* m_decodedCompleteCallback = nullptr;
    
};

} // end namespace webrtc
} // end namespace unity

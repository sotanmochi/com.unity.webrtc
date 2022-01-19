#pragma once
#include <vector>
#include <cuda.h>
#include "api/video_codecs/sdp_video_format.h"
#include "nvEncodeAPI.h"

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    std::vector<SdpVideoFormat> SupportedNvEncoderCodecs(CUcontext context);
    std::vector<SdpVideoFormat> SupportedNvDecoderCodecs(CUcontext context);

    class NvEncoder : public VideoEncoder {
    public:
        static std::unique_ptr<NvEncoder> Create(
            const cricket::VideoCodec& codec,
            CUcontext context, CUmemorytype memoryType, NV_ENC_BUFFER_FORMAT format);
        // If H.264 is supported (any implementation).
        static bool IsSupported();
        static bool SupportsScalabilityMode(absl::string_view scalability_mode);

        ~NvEncoder() override {}
    };

    class NvDecoder : public VideoDecoder {
    public:
        static std::unique_ptr<H264Decoder> Create();
        static bool IsSupported();

        ~NvDecoder() override {}
    };
}
}


#pragma once
#include "api/video_codecs/sdp_video_format.h"
#include "nvEncodeAPI.h"
#include <cuda.h>
#include <vector>
#include "api/video_codecs/h264_profile_level_id.h"

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    absl::optional<H264Profile> GuidToProfile(GUID& guid);
    absl::optional<GUID> ProfileToGuid(H264Profile profile);

    std::vector<SdpVideoFormat> SupportedNvEncoderCodecs(CUcontext context);
    std::vector<SdpVideoFormat> SupportedNvDecoderCodecs(CUcontext context);

    class NvEncoder : public VideoEncoder
    {
    public:
        static std::unique_ptr<NvEncoder> Create(
            const cricket::VideoCodec& codec,
            CUcontext context,
            CUmemorytype memoryType,
            NV_ENC_BUFFER_FORMAT format);
        // If H.264 is supported (any implementation).
        static bool IsSupported();
        static bool SupportsScalabilityMode(absl::string_view scalability_mode);

        ~NvEncoder() override { }
    };

    class NvDecoder : public VideoDecoder
    {
    public:
        static std::unique_ptr<H264Decoder> Create();
        static bool IsSupported();

        ~NvDecoder() override { }
    };
}
}

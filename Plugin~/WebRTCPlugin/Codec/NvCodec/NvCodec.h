#pragma once
#include "api/video_codecs/h264_profile_level_id.h"
#include "api/video_codecs/sdp_video_format.h"
#include "api/video_codecs/video_decoder.h"
#include "api/video_codecs/video_encoder.h"
#include "media/base/codec.h"
#include "nvEncodeAPI.h"
#include <cuda.h>
#include <vector>

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    constexpr absl::optional<H264Profile> GuidToProfile(GUID& guid);
    constexpr absl::optional<GUID> ProfileToGuid(H264Profile profile);

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
        static std::unique_ptr<NvDecoder> Create();
        static bool IsSupported();

        ~NvDecoder() override { }
    };
}
}

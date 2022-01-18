#pragma once
#include <vector>
#include <cuda.h>
#include "api/video_codecs/sdp_video_format.h"

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    std::vector<SdpVideoFormat> SupportedNvEncoderCodecs(CUcontext context);
    std::vector<SdpVideoFormat> SupportedNvDecoderCodecs(CUcontext context);
}
}


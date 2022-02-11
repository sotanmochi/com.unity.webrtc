#include "pch.h"

#include "VideoCodecTest.h"

namespace unity
{
namespace webrtc
{
    void VideoCodecTest::SetUp()
    {
        SetDefaultSettings(&codecSettings_);

        encoder_ = CreateEncoder();
        decoder_ = CreateDecoder();
        encoder_->RegisterEncodeCompleteCallback(_encodedImageCallback);
        // decoder_->RegisterDecodeCompleteCallback(_decodedImageCallback);
    }
}
}

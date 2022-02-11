#pragma once

#include "api/video_codecs/video_codec.h"
#include "api/video_codecs/video_decoder.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/h264_profile_level_id.h"
#include "media/base/codec.h"
#include "gtest/gtest.h"

using namespace ::webrtc;

namespace
{
constexpr int kNumCores = 1;
constexpr size_t kMaxPayloadSize = 1440;
const H264ProfileLevelId
    kProfileLevelId(H264Profile::kProfileConstrainedHigh, H264Level::kLevel5_2);
static const std::string kProfileLevelIdString()
{
    return *H264ProfileLevelIdToString(kProfileLevelId);
}
static const VideoEncoder::Capabilities kCapabilities()
{
    return VideoEncoder::Capabilities(false);
}
static const VideoEncoder::Settings kSettings()
{
    return VideoEncoder::Settings(kCapabilities(), kNumCores, kMaxPayloadSize);
}
static void SetDefaultSettings(VideoCodec* codec_settings)
{
    codec_settings->codecType = kVideoCodecH264;
    codec_settings->maxFramerate = 60;
    codec_settings->width = 640;
    codec_settings->height = 480;
    // If frame dropping is false, we get a warning that bitrate can't
    // be controlled for RC_QUALITY_MODE; RC_BITRATE_MODE and RC_TIMESTAMP_MODE
    codec_settings->H264()->frameDroppingOn = true;
    codec_settings->startBitrate = 2000;
    codec_settings->maxBitrate = 4000;
}
}

namespace unity
{
namespace webrtc
{
    class VideoCodecTest : public testing::Test
    {
    public:
        VideoCodecTest() { }
        virtual ~VideoCodecTest() override { }

        virtual std::unique_ptr<VideoEncoder> CreateEncoder() = 0;
        virtual std::unique_ptr<VideoDecoder> CreateDecoder() = 0;
        void SetUp() override;

    protected:
        VideoCodec codecSettings_;
        std::unique_ptr<VideoEncoder> encoder_;
        std::unique_ptr<VideoDecoder> decoder_;

    private:
        EncodedImageCallback* _encodedImageCallback;
        DecodedImageCallback* _decodedImageCallback;
    };
}
}

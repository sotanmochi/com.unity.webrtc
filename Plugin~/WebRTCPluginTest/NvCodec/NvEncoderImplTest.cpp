#include "pch.h"

#include "../NvCodec/Utils/NvCodecUtils.h"
#include "Codec/NvCodec/NvEncoderImpl.h"

using namespace webrtc;

using testing::Values;

namespace unity
{
namespace webrtc
{

    class NvEncoderImplTest : public ::testing::Test
    {
    public:
        NvEncoderImplTest()
        {
            EXPECT_TRUE(ck(cuInit(0)));
            EXPECT_TRUE(ck(cuDeviceGet(&device_, 0)));
            EXPECT_TRUE(ck(cuCtxCreate(&context_, 0, device_)));
        }
        ~NvEncoderImplTest()
        {
            EXPECT_TRUE(ck(cuCtxDestroy(context_)));
        }
    protected:
        CUdevice device_;
        CUcontext context_;
    };

    const int kMaxPayloadSize = 1024;
    const int kNumCores = 1;

    const VideoEncoder::Capabilities kCapabilities(false);
    const VideoEncoder::Settings kSettings(kCapabilities,
        kNumCores,
        kMaxPayloadSize);

    void SetDefaultSettings(VideoCodec* codec_settings) {
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

    TEST_F(NvEncoderImplTest, CanInitializeWithDefaultParameters)
    {
        NvEncoderImpl encoder(
            cricket::VideoCodec("H264"), context_,
            CU_MEMORYTYPE_ARRAY, NV_ENC_BUFFER_FORMAT_ARGB);

        VideoCodec codec_settings;
        SetDefaultSettings(&codec_settings);
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK,
            encoder.InitEncode(&codec_settings, kSettings));
    }

} // end namespace webrtc
} // end namespace unity

#include "pch.h"

#include "../NvCodec/Utils/NvCodecUtils.h"
#include "Codec/NvCodec/NvEncoderImpl.h"
#include "VideoCodecTest.h"

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
        ~NvEncoderImplTest() override { EXPECT_TRUE(ck(cuCtxDestroy(context_))); }

    protected:
        CUdevice device_;
        CUcontext context_;
    };

    TEST_F(NvEncoderImplTest, CanInitializeWithDefaultParameters)
    {
        cricket::VideoCodec codec = cricket::VideoCodec(cricket::kH264CodecName);
        codec.SetParam(cricket::kH264FmtpProfileLevelId, kProfileLevelIdString());
        NvEncoderImpl encoder(codec, context_, CU_MEMORYTYPE_ARRAY, NV_ENC_BUFFER_FORMAT_ARGB);

        VideoCodec codec_settings;
        SetDefaultSettings(&codec_settings);
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder.InitEncode(&codec_settings, kSettings()));
    }

} // end namespace webrtc
} // end namespace unity

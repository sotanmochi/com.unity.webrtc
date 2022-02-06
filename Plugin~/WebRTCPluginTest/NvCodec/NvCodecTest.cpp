#include "pch.h"

#include "Codec/NvCodec/NvCodec.h"
#include "../NvCodec/Utils/NvCodecUtils.h"
//#include "modules/video_coding/codecs/test/video_codec_unittest.h"
//#include "test/video_codec_settings.h"

using testing::Values;

namespace unity
{
namespace webrtc
{

    class NvCodecTest : public testing::Test
    {
    public:
        NvCodecTest()
        {
            EXPECT_TRUE(ck(cuInit(0)));
            EXPECT_TRUE(ck(cuDeviceGet(&device_, 0)));
            EXPECT_TRUE(ck(cuCtxCreate(&context_, 0, device_)));
        }
        ~NvCodecTest()
        {
            EXPECT_TRUE(ck(cuCtxDestroy(context_)));
        }
    protected:
        //std::unique_ptr<VideoEncoder> CreateEncoder() override
        //{
        //    return NvEncoder::Create(
        //        cricket::VideoCodec(cricket::kH264CodecName), context_,
        //        CU_MEMORYTYPE_ARRAY, NV_ENC_BUFFER_FORMAT_ARGB);
        //}

        //std::unique_ptr<VideoDecoder> CreateDecoder() override
        //{
        //    return NvDecoder::Create();
        //}

        //void ModifyCodecSettings(VideoCodec * codec_settings) override
        //{
        //    webrtc::test::CodecSettings(kVideoCodecH264, codec_settings);
        //}
        CUdevice device_;
        CUcontext context_;
    };

    TEST_F(NvCodecTest, SupportedNvEncoderCodecs)
    {
        //std::vector<SdpVideoFormat> formats =
        //    SupportedNvEncoderCodecs(context_);
        //EXPECT_GT(formats.size(), 0);
    }

}
}

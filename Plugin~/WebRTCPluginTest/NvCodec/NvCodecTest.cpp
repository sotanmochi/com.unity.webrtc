#include "pch.h"

#include "../NvCodec/Utils/NvCodecUtils.h"
#include "Codec/NvCodec/NvCodec.h"
#include "Codec/NvCodec/NvEncoderImpl.h"
#include "VideoCodecTest.h"

using testing::Values;

namespace unity
{
namespace webrtc
{
    class NvCodecTest : public VideoCodecTest
    {
    public:
        NvCodecTest()
        {
            EXPECT_TRUE(ck(cuInit(0)));
            EXPECT_TRUE(ck(cuDeviceGet(&device_, 0)));
            EXPECT_TRUE(ck(cuCtxCreate(&context_, 0, device_)));
        }
        ~NvCodecTest() override
        {
            if(encoder_)
                encoder_ = nullptr;
            EXPECT_TRUE(ck(cuCtxDestroy(context_)));
        }

    protected:
        std::unique_ptr<VideoEncoder> CreateEncoder() override
        {
            cricket::VideoCodec codec = cricket::VideoCodec(cricket::kH264CodecName);
            codec.SetParam(cricket::kH264FmtpProfileLevelId, kProfileLevelIdString());
            return NvEncoder::Create(
                codec, context_, CU_MEMORYTYPE_ARRAY, NV_ENC_BUFFER_FORMAT_ARGB);
        }

        std::unique_ptr<VideoDecoder> CreateDecoder() override { return NvDecoder::Create(); }

        void EncodeAndWaitForFrame(
            const VideoFrame& input_frame,
            EncodedImage* encoded_frame,
            CodecSpecificInfo* codec_specific_info,
            bool keyframe = false)
        {
            std::vector<VideoFrameType> frame_types;
            if (keyframe)
            {
                frame_types.emplace_back(VideoFrameType::kVideoFrameKey);
            }
            else
            {
                frame_types.emplace_back(VideoFrameType::kVideoFrameDelta);
            }
            EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->Encode(input_frame, &frame_types));
            ASSERT_TRUE(WaitForEncodedFrame(encoded_frame, codec_specific_info));
            //VerifyQpParser(*encoded_frame);
            EXPECT_EQ(kVideoCodecH264, codec_specific_info->codecType);
            EXPECT_EQ(0, encoded_frame->SpatialIndex());
        }

        CUdevice device_;
        CUcontext context_;
    };

    TEST_F(NvCodecTest, SupportedNvEncoderCodecs)
    {
        std::vector<SdpVideoFormat> formats = SupportedNvEncoderCodecs(context_);
        EXPECT_GT(formats.size(), 0);
    }

    TEST_F(NvCodecTest, SetRates)
    {
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->InitEncode(&codecSettings_, kSettings()));

        const uint32_t kBitrateBps = 300000;
        VideoBitrateAllocation bitrateAllocation;
        bitrateAllocation.SetBitrate(0, 0, kBitrateBps);

        encoder_->SetRates(VideoEncoder::RateControlParameters(
            bitrateAllocation, static_cast<double>(codecSettings_.maxFramerate)));
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->Release());
    }

    TEST_F(NvCodecTest, EncodeFrameAndRelease)
    {
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->Release());
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->InitEncode(&codecSettings_, kSettings()));

        EncodedImage encoded_frame;
        CodecSpecificInfo codec_specific_info;
        //EncodeAndWaitForFrame(NextInputFrame(), &encoded_frame, &codec_specific_info);

        //EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->Release());
        // EXPECT_EQ(WEBRTC_VIDEO_CODEC_UNINITIALIZED, encoder_->Encode(NextInputFrame(), nullptr));
    }

}
}

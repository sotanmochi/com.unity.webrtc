#include "pch.h"

#include "base/callback.h"
#include "GpuMemoryBuffer.h"
#include "../NvCodec/Utils/NvCodecUtils.h"
#include "../WebRTCPlugin/H264HardwareEncoder.h"
#include "modules/video_coding/codecs/test/video_codec_unittest.h"
#include "test/video_codec_settings.h"
//#include "api/test/create_frame_generator.h"
#include "api/test/create_videocodec_test_fixture.h"
//#include "modules/video_coding/codecs/vp8/include/vp8.h"
#include "modules/video_coding/codecs/test/videocodec_test_fixture_impl.h"
#include "UnityVideoTrackSource.h"
#include "VideoFrameUtil.h"

//using testing::_;
using namespace webrtc;
using namespace webrtc::test;


//static const int kEncodeTimeoutMs = 100;
//static const int kDecodeTimeoutMs = 25;
//// Set bitrate to get higher quality.
//static const int kStartBitrate = 300;
//static const int kMaxBitrate = 4000;
//static const int kWidth = 176;        // Width of the input image.
//static const int kHeight = 144;       // Height of the input image.
//static const int kMaxFramerate = 30;  // Arbitrary value.

namespace unity
{
namespace webrtc
{

const int kInputFrameFillY = 12;
const int kInputFrameFillU = 23;
const int kInputFrameFillV = 34;
const uint16_t kInputFrameHeight = 234;
const uint16_t kInputFrameWidth = 345;
const uint16_t kStartBitrate = 100;

const webrtc::VideoEncoder::Capabilities kVideoEncoderCapabilities(
    /* loss_notification= */ false);
const webrtc::VideoEncoder::Settings
    kVideoEncoderSettings(kVideoEncoderCapabilities, 1, 12345);

class EncodedImageCallbackWrapper : public webrtc::EncodedImageCallback {
public:
    using EncodedCallback = ::base::OnceCallback<void(
        const webrtc::EncodedImage& encoded_image,
        const webrtc::CodecSpecificInfo* codec_specific_info)>;

    EncodedImageCallbackWrapper(EncodedCallback encoded_callback)
        : encoded_callback_(std::move(encoded_callback)) {}

    Result OnEncodedImage(
        const webrtc::EncodedImage& encoded_image,
        const webrtc::CodecSpecificInfo* codec_specific_info) override {
        std::move(encoded_callback_).Run(encoded_image, codec_specific_info);
        return Result(Result::OK);
    }

private:
    EncodedCallback encoded_callback_;
};

class H264HardwareEncoderTest : public ::testing::TestWithParam<webrtc::VideoCodecType>
{
public:
    H264HardwareEncoderTest()
        : encoder_thread_(rtc::Thread::Create())
//        decode_complete_callback_(this)
//        wait_for_encoded_frames_threshold_(1),
//        last_input_frame_timestamp_(0)
        {}
protected:

    std::unique_ptr<VideoEncoder> CreateEncoder()
    {
        return H264HardwareEncoder::Create(
            m_context, CU_MEMORYTYPE_DEVICE, NV_ENC_BUFFER_FORMAT_ARGB);
    }

    std::unique_ptr<VideoDecoder> CreateDecoder()
    {
        // todo::(kazuki) this is for test
        return VP8Decoder::Create();
    }
    void ModifyCodecSettings(VideoCodec* codec_settings)
    {
        test::CodecSettings(kVideoCodecH264, codec_settings);
    }

    void SetUp() override
    {
        ASSERT_TRUE(encoder_thread_->Start());

        //EXPECT_CALL(*mock_gpu_factories_.get(), GetTaskRunner())
        //    .WillRepeatedly(Return(encoder_thread_.task_runner()));

        //EXPECT_TRUE(
        //    ck(cuInit(0)));

        //EXPECT_TRUE(
        //    ck(cuDeviceGet(&m_device, 0)));

        //EXPECT_TRUE(
        //    ck(cuCtxCreate(&m_context, 0, m_device)));

        //VideoCodec codec_settings_;
        //// webrtc::test::CodecSettings(kVideoCodecVP8, &codec_settings_);
        //codec_settings_.startBitrate = kStartBitrate;
        //codec_settings_.maxBitrate = kMaxBitrate;
        //codec_settings_.maxFramerate = kMaxFramerate;
        //codec_settings_.width = kWidth;
        //codec_settings_.height = kHeight;
        //codec_settings_.codecType = kVideoCodecH264;

        //ModifyCodecSettings(&codec_settings_);

        //input_frame_generator_ = test::CreateSquareFrameGenerator(
        //    codec_settings_.width, codec_settings_.height,
        //    test::FrameGeneratorInterface::OutputType::kI420, absl::optional<int>());

        //encoder_ = CreateEncoder();
        //decoder_ = CreateDecoder();
        //encoder_->RegisterEncodeCompleteCallback(&encode_complete_callback_);
        //decoder_->RegisterDecodeCompleteCallback(&decode_complete_callback_);

        //EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK,
        //    encoder_->InitEncode(
        //        &codec_settings_,
        //        VideoEncoder::Settings(kCapabilities, 1 /* number of cores */,
        //            0 /* max payload size (unused) */)));
        //EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK,
        //    decoder_->InitDecode(&codec_settings_, 1 /* number of cores */));

        //const int size = kWidth * kHeight * 4;
        //EXPECT_TRUE(
        //    ck(cuMemAlloc(&devicePtr_, size)));

        //CUDA_ARRAY3D_DESCRIPTOR arrayDesc = {};
        //arrayDesc.Width = kWidth;
        //arrayDesc.Height = kHeight;
        //arrayDesc.Depth = 0; /* CUDA 2D arrays are defined to have depth 0 */
        //arrayDesc.Format = CU_AD_FORMAT_UNSIGNED_INT32;
        //arrayDesc.NumChannels = 1;
        //arrayDesc.Flags = CUDA_ARRAY3D_SURFACE_LDST;

        //EXPECT_TRUE(
        //    ck(cuArray3DCreate(&array_, &arrayDesc)));
    }

    void TearDown() override
    {
        //EXPECT_TRUE(encoder_thread_->IsRunning());
        //RunUntilIdle();
        if (encoder_)
            encoder_->Release();
        //RunUntilIdle();
        encoder_thread_->Quit();

        //EXPECT_TRUE(
        //    ck(cuMemFree(devicePtr_)));
        //EXPECT_TRUE(
        //    ck(cuArrayDestroy(array_)));
    }

    void RunUntilIdle() {
        //encoder_thread_->PostTask(
        //    RTC_FROM_HERE, base::Bind(&base::WaitableEvent::Signal,
        //        base::Unretained(&idle_waiter_)));
        //idle_waiter_.Wait();
    }

    void CreateEncoder(webrtc::VideoCodecType codec_type) {
        //media::VideoCodecProfile media_profile;
        switch (codec_type) {
        case webrtc::kVideoCodecVP8:
            media_profile = media::VP8PROFILE_ANY;
            break;
        case webrtc::kVideoCodecH264:
            media_profile = media::H264PROFILE_BASELINE;
            break;
        case webrtc::kVideoCodecVP9:
            media_profile = media::VP9PROFILE_PROFILE0;
            break;
        default:
            ADD_FAILURE() << "Unexpected codec type: " << codec_type;
            media_profile = media::VIDEO_CODEC_PROFILE_UNKNOWN;
        }

        mock_vea_ = ExpectCreateInitAndDestroyVEA();
        encoder_ = std::make_unique<H264HardwareEncoder>(
            media_profile, false,
            mock_gpu_factories_.get());
    }

    ::webrtc::VideoFrame NextInputFrame()
    {
        test::FrameGeneratorInterface::VideoFrameData frame_data =
            input_frame_generator_->NextFrame();
        ::webrtc::VideoFrame input_frame = ::webrtc::VideoFrame::Builder()
            .set_video_frame_buffer(frame_data.buffer)
            .set_update_rect(frame_data.update_rect)
            .build();

        auto frame = CreateTestFrame(kWidth, kHeight);

        //rtc::scoped_refptr<VideoFrameAdapter> frame_adapter(
        //    new rtc::RefCountedObject<VideoFrameAdapter>(std::move(frame)));

        //::webrtc::VideoFrame input_frame = ::webrtc::VideoFrame::Builder()
        //    .set_video_frame_buffer(frame_adapter)
        //    .build();

        const uint32_t timestamp = 0;
            lastTimesamp_ +
            kVideoPayloadTypeFrequency / 30.0f; // 30 frame per second
        input_frame.set_timestamp(timestamp);

        lastTimesamp_ = timestamp;
        return input_frame;
    }

    std::unique_ptr<VideoEncoder> encoder_;
    std::unique_ptr<VideoDecoder> decoder_;
    std::unique_ptr<test::FrameGeneratorInterface> input_frame_generator_;
private:
    CUdevice m_device;
    CUcontext m_context;
    CUdeviceptr devicePtr_;
    CUarray array_;
    std::shared_timed_mutex mutex_;
    uint32_t lastTimesamp_;
    std::unique_ptr<rtc::Thread> encoder_thread_;
    //FakeEncodeCompleteCallback encode_complete_callback_;
    //FakeDecodeCompleteCallback decode_complete_callback_;

    Mutex encoded_frame_section_;
    std::vector<EncodedImage> encoded_frames_
        RTC_GUARDED_BY(encoded_frame_section_);
    std::vector<CodecSpecificInfo> codec_specific_infos_
        RTC_GUARDED_BY(encoded_frame_section_);
};

//VideoCodecTestFixture::Config CreateConfig() {
//    VideoCodecTestFixture::Config config;
//    config.filename = "foreman_cif";
//    config.filepath = ResourcePath(config.filename, "yuv");
//    config.num_frames = kNumFrames;
//    // Only allow encoder/decoder to use single core, for predictability.
//    config.use_single_core = true;
//    return config;
//}

TEST_F(H264HardwareEncoderTest, CreateAndInitSucceeds)
{
    const webrtc::VideoCodecType codec_type = GetParam();
    CreateEncoder(codec_type);
    webrtc::VideoCodec codec = GetDefaultCodec();
    codec.codecType = codec_type;
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK,
        rtc_encoder_->InitEncode(&codec, kVideoEncoderSettings));
}

INSTANTIATE_TEST_SUITE_P(CodecProfiles,
    H264HardwareEncoderTest,
    Values(webrtc::kVideoCodecH264,
        webrtc::kVideoCodecVP8,
        webrtc::kVideoCodecVP9));

//{
    //::webrtc::VideoFrame input_frame = NextInputFrame();
    //EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->Encode(input_frame, nullptr));
    //EncodedImage encoded_frame;
    //CodecSpecificInfo codec_specific_info;
    //ASSERT_TRUE(WaitForEncodedFrame(&encoded_frame, &codec_specific_info));

    // First frame should be a key frame.
    //encoded_frame._frameType = VideoFrameType::kVideoFrameKey;
    //EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, decoder_->Decode(encoded_frame, false, 0));
    //std::unique_ptr<VideoFrame> decoded_frame;
    //absl::optional<uint8_t> decoded_qp;
    //ASSERT_TRUE(WaitForDecodedFrame(&decoded_frame, &decoded_qp));
    //ASSERT_TRUE(decoded_frame);
    //EXPECT_GT(I420PSNR(&input_frame, decoded_frame.get()), 36);

    //const ColorSpace color_space = *decoded_frame->color_space();
    //EXPECT_EQ(ColorSpace::PrimaryID::kUnspecified, color_space.primaries());
    //EXPECT_EQ(ColorSpace::TransferID::kUnspecified, color_space.transfer());
    //EXPECT_EQ(ColorSpace::MatrixID::kUnspecified, color_space.matrix());
    //EXPECT_EQ(ColorSpace::RangeID::kInvalid, color_space.range());
    //EXPECT_EQ(ColorSpace::ChromaSiting::kUnspecified,
    //    color_space.chroma_siting_horizontal());
    //EXPECT_EQ(ColorSpace::ChromaSiting::kUnspecified,
    //    color_space.chroma_siting_vertical());
//}

} // end namespace webrtc
} // end namespace unity

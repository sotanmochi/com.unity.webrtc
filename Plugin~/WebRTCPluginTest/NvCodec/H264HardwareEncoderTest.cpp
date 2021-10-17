#include "pch.h"


#include "GpuResourceBuffer.h"
#include "../NvCodec/Utils/NvCodecUtils.h"
#include "../WebRTCPlugin/H264HardwareEncoder.h"
#include "modules/video_coding/codecs/test/video_codec_unittest.h"
#include "test/video_codec_settings.h"
#include "modules/video_coding/codecs/vp8/include/vp8.h"

//using testing::_;
using namespace webrtc;

static const int kEncodeTimeoutMs = 100;
static const int kDecodeTimeoutMs = 25;
// Set bitrate to get higher quality.
static const int kStartBitrate = 300;
static const int kMaxBitrate = 4000;
static const int kWidth = 176;        // Width of the input image.
static const int kHeight = 144;       // Height of the input image.
static const int kMaxFramerate = 30;  // Arbitrary value.

namespace unity
{
namespace webrtc
{

const VideoEncoder::Capabilities kCapabilities(false);

class H264HardwareEncoderTest : public ::testing::Test
{
public:
    H264HardwareEncoderTest()
        : encode_complete_callback_(this),
        decode_complete_callback_(this)
//        wait_for_encoded_frames_threshold_(1),
//        last_input_frame_timestamp_(0)
        {}
protected:
    class FakeEncodeCompleteCallback : public EncodedImageCallback {
    public:
        explicit FakeEncodeCompleteCallback(H264HardwareEncoderTest* test)
            : test_(test) {}

        Result OnEncodedImage(const EncodedImage& frame,
            const CodecSpecificInfo* codec_specific_info)
        {
            return Result(Result::OK);
        }

    private:
        H264HardwareEncoderTest* const test_;
    };

    class FakeDecodeCompleteCallback : public DecodedImageCallback {
    public:
        explicit FakeDecodeCompleteCallback(H264HardwareEncoderTest* test)
            : test_(test) {}

        int32_t Decoded(VideoFrame& frame) override {
            RTC_NOTREACHED();
            return -1;
        }
        int32_t Decoded(VideoFrame& frame, int64_t decode_time_ms) override {
            RTC_NOTREACHED();
            return -1;
        }
        void Decoded(VideoFrame& frame,
            absl::optional<int32_t> decode_time_ms,
            absl::optional<uint8_t> qp) override
        {
        }

    private:
        H264HardwareEncoderTest* const test_;
    };

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
        EXPECT_TRUE(
            ck(cuInit(0)));

        EXPECT_TRUE(
            ck(cuDeviceGet(&m_device, 0)));

        EXPECT_TRUE(
            ck(cuCtxCreate(&m_context, 0, m_device)));

        VideoCodec codec_settings_;
        // webrtc::test::CodecSettings(kVideoCodecVP8, &codec_settings_);
        codec_settings_.startBitrate = kStartBitrate;
        codec_settings_.maxBitrate = kMaxBitrate;
        codec_settings_.maxFramerate = kMaxFramerate;
        codec_settings_.width = kWidth;
        codec_settings_.height = kHeight;
        codec_settings_.codecType = kVideoCodecH264;

        // ModifyCodecSettings(&codec_settings_);

        //input_frame_generator_ = test::CreateSquareFrameGenerator(
        //    codec_settings_.width, codec_settings_.height,
        //    test::FrameGeneratorInterface::OutputType::kI420, absl::optional<int>());
        encoder_ = CreateEncoder();
        decoder_ = CreateDecoder();
        encoder_->RegisterEncodeCompleteCallback(&encode_complete_callback_);
        decoder_->RegisterDecodeCompleteCallback(&decode_complete_callback_);

        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK,
            encoder_->InitEncode(
                &codec_settings_,
                VideoEncoder::Settings(kCapabilities, 1 /* number of cores */,
                    0 /* max payload size (unused) */)));
        EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK,
            decoder_->InitDecode(&codec_settings_, 1 /* number of cores */));

        const int size = kWidth * kHeight * 4;
        EXPECT_TRUE(
            ck(cuMemAlloc(&devicePtr_, size)));

        CUDA_ARRAY3D_DESCRIPTOR arrayDesc = {};
        arrayDesc.Width = kWidth;
        arrayDesc.Height = kHeight;
        arrayDesc.Depth = 0; /* CUDA 2D arrays are defined to have depth 0 */
        arrayDesc.Format = CU_AD_FORMAT_UNSIGNED_INT32;
        arrayDesc.NumChannels = 1;
        arrayDesc.Flags = CUDA_ARRAY3D_SURFACE_LDST;

        EXPECT_TRUE(
            ck(cuArray3DCreate(&array_, &arrayDesc)));
    }

    void TearDown() override
    {
        EXPECT_TRUE(
            ck(cuMemFree(devicePtr_)));
        EXPECT_TRUE(
            ck(cuArrayDestroy(array_)));
    }

    VideoFrame NextInputFrame()
    {
        rtc::scoped_refptr<I420Buffer> i420Buffer = I420Buffer::Create(kWidth, kHeight);
        rtc::scoped_refptr<GpuResourceBuffer> buffer =
            new rtc::RefCountedObject<GpuResourceBuffer>(i420Buffer, devicePtr_, mutex_);

        VideoFrame input_frame = VideoFrame::Builder()
            .set_video_frame_buffer(buffer)
            .build();

        const uint32_t timestamp = 0;
            lastTimesamp_ +
            kVideoPayloadTypeFrequency / 30.0f; // 30 frame per second
        input_frame.set_timestamp(timestamp);

        lastTimesamp_ = timestamp;
        return input_frame;
    }

    std::unique_ptr<VideoEncoder> encoder_;
    std::unique_ptr<VideoDecoder> decoder_;
private:
    CUdevice m_device;
    CUcontext m_context;
    CUdeviceptr devicePtr_;
    CUarray array_;
    std::shared_timed_mutex mutex_;
    uint32_t lastTimesamp_;
    FakeEncodeCompleteCallback encode_complete_callback_;
    FakeDecodeCompleteCallback decode_complete_callback_;
};

TEST_F(H264HardwareEncoderTest, MAYBE_EncodeDecode)
{
    VideoFrame input_frame = NextInputFrame();
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->Encode(input_frame, nullptr));
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
}

} // end namespace webrtc
} // end namespace unity

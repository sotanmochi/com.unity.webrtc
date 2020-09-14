#pragma once
#include <cuda.h>
#include <NvEncoder/NvEncoderCuda.h>

using namespace webrtc;

namespace unity
{
namespace webrtc
{

class H264HardwareEncoder : public ::webrtc::VideoEncoder
{
public:
    struct LayerConfig {
        int simulcast_idx = 0;
        int width = -1;
        int height = -1;
        bool sending = true;
        bool key_frame_request = false;
        float max_frame_rate = 0;
        uint32_t target_bps = 0;
        uint32_t max_bps = 0;
        bool frame_dropping_on = false;
        int key_frame_interval = 0;
        int num_temporal_layers = 1;

        void SetStreamState(bool send_stream) {}
    };
    H264HardwareEncoder(CUcontext context, CUmemorytype memoryType, NV_ENC_BUFFER_FORMAT format);

    // webrtc::VideoEncoder
    // Initialize the encoder with the information from the codecSettings
    virtual int InitEncode(const VideoCodec* codec_settings,
        const VideoEncoder::Settings& settings) override;
    // Register an encode complete m_encodedCompleteCallback object.
    virtual int32_t RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
    // Free encoder memory.
    virtual int32_t Release() override;
    // Encode an I420 image (as a part of a video stream). The encoded image
    // will be returned to the user through the encode complete m_encodedCompleteCallback.
    virtual int32_t Encode(const VideoFrame& frame,
        const std::vector<VideoFrameType>* frame_types) override;
    // Default fallback: Just use the sum of bitrates as the single target rate.
    virtual void SetRates(const RateControlParameters& parameters) override;

    static std::unique_ptr<VideoEncoder> Create(
        CUcontext context, CUmemorytype memoryType, NV_ENC_BUFFER_FORMAT format);
protected:
    int32_t ProcessEncodedFrame(std::vector<uint8_t>& packet, const VideoFrame& inputFrame);
    void SetStreamState(bool sendStream);
private:

    CUcontext m_context;
    CUmemorytype m_memoryType;
    std::unique_ptr<NvEncoder> m_encoder;

    VideoCodec m_codec;

    NV_ENC_BUFFER_FORMAT m_format;
    NV_ENC_INITIALIZE_PARAMS m_nvEncInitializeParams;
    NV_ENC_CONFIG m_nvEncConfig;

    EncodedImageCallback* m_encodedCompleteCallback = nullptr;
    EncodedImage m_encodedImage;
    RTPFragmentationHeader m_fragHeader;
    H264BitstreamParser m_h264BitstreamParser;
    std::unique_ptr<BitrateAdjuster> m_bitrateAdjuster;
    RateStatistics m_encode_fps;
    Clock* const m_clock;
    bool m_keyframeRequest;

    int64_t prev;
};

} // end namespace webrtc
} // end namespace unity

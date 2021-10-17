#include "pch.h"
#include "H264HardwareEncoder.h"
#include "GpuResourceBuffer.h"
#include "NvEncoder/NvEncoder.h"
#include "NvEncoder/NvEncoderCuda.h"
#include "Codec/NvCodec/NvEncoderCudaWithCUarray.h"
#include "../Utils/NvCodecUtils.h"

using namespace webrtc;

namespace unity
{
namespace webrtc
{

    std::unique_ptr<VideoEncoder> H264HardwareEncoder::Create(
        CUcontext context, CUmemorytype memoryType, NV_ENC_BUFFER_FORMAT format)
    {
        return std::make_unique<H264HardwareEncoder>(context, memoryType, format);
    }

    H264HardwareEncoder::H264HardwareEncoder(
        CUcontext context, CUmemorytype memoryType, NV_ENC_BUFFER_FORMAT format)
    : m_context(context)
    , m_memoryType(memoryType)
    , m_format(format)
    , m_encoder(nullptr)
    , m_encode_fps(1000, 1000)
    , m_clock(Clock::GetRealTimeClock())
    {
        // not implemented for host memory
        RTC_CHECK_NE(memoryType, CU_MEMORYTYPE_HOST);
    }

    int H264HardwareEncoder::InitEncode(const VideoCodec* codec,
        const VideoEncoder::Settings& settings)
    {
        if (codec == nullptr)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }

        if (codec->codecType != kVideoCodecH264)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }
        if (codec->maxFramerate == 0)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }
        if (codec->width < 1 || codec->height < 1)
        {
            return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
        }

        m_codec = *codec;

        const CUresult result = cuCtxSetCurrent(m_context);
        if(!ck(result))
        {
            return WEBRTC_VIDEO_CODEC_ENCODER_FAILURE;
        }

        if(m_memoryType == CU_MEMORYTYPE_DEVICE)
        {
            m_encoder = std::make_unique<NvEncoderCuda>(
                m_context, codec->width, codec->height,
                m_format, 0);
        }
        else if(m_memoryType == CU_MEMORYTYPE_ARRAY)
        {
            m_encoder = std::make_unique<NvEncoderCudaWithCUarray>(
                m_context, codec->width, codec->height,
                m_format, 0);
        }

        m_bitrateAdjuster = std::make_unique<BitrateAdjuster>(0.5f, 0.95f);

        m_nvEncInitializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
        m_nvEncConfig = { NV_ENC_CONFIG_VER };
        m_nvEncInitializeParams.encodeConfig = &m_nvEncConfig;
        m_nvEncInitializeParams.frameRateNum = m_codec.maxFramerate;
        m_encoder->CreateDefaultEncoderParams(&m_nvEncInitializeParams,
            NV_ENC_CODEC_H264_GUID,
            NV_ENC_PRESET_LOW_LATENCY_DEFAULT_GUID);
        m_nvEncConfig.profileGUID = NV_ENC_H264_PROFILE_BASELINE_GUID;
        m_nvEncConfig.gopLength = NVENC_INFINITE_GOPLENGTH;
        m_nvEncConfig.frameIntervalP = 1;
        m_nvEncConfig.encodeCodecConfig.h264Config.idrPeriod = NVENC_INFINITE_GOPLENGTH;
        m_nvEncConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CBR_LOWDELAY_HQ;
        //m_nvEncConfig.rcParams.averageBitRate = (static_cast<unsigned int>(5.0f *
        //        m_nvEncInitializeParams.encodeWidth *
        //        m_nvEncInitializeParams.encodeHeight) / (1280 * 720)) * 100000;

        m_nvEncConfig.rcParams.averageBitRate = m_bitrateAdjuster->GetAdjustedBitrateBps();
        m_nvEncConfig.rcParams.vbvBufferSize = (m_nvEncConfig.rcParams.averageBitRate *
                m_nvEncInitializeParams.frameRateDen / m_nvEncInitializeParams.frameRateNum) * 5;
        m_nvEncConfig.rcParams.maxBitRate = m_nvEncConfig.rcParams.averageBitRate;
        m_nvEncConfig.rcParams.vbvInitialDelay = m_nvEncConfig.rcParams.vbvBufferSize;

        m_encoder->CreateEncoder(&m_nvEncInitializeParams);

        return WEBRTC_VIDEO_CODEC_OK;
    }

    int32_t H264HardwareEncoder::RegisterEncodeCompleteCallback(EncodedImageCallback* callback)
    {
        this->m_encodedCompleteCallback = callback;
        return WEBRTC_VIDEO_CODEC_OK;
    }

    int32_t H264HardwareEncoder::Release()
    {
        this->m_encodedCompleteCallback = nullptr;

        return WEBRTC_VIDEO_CODEC_OK;
    }

    void CopyResource (
        const NvEncInputFrame* dst, const GpuResourceBuffer* buffer,
        CUcontext context, CUmemorytype memoryType)
    {
        std::shared_timed_mutex* mutex = buffer->mutex();
        std::shared_lock<std::shared_timed_mutex> lock(*mutex);

        int width = buffer->width();
        int height = buffer->height();

        if(memoryType == CU_MEMORYTYPE_DEVICE)
        {
            const CUdeviceptr devicePtr = buffer->ToDevicePtr();

            NvEncoderCuda::CopyToDeviceFrame(
                context, reinterpret_cast<void*>(devicePtr), 0, reinterpret_cast<CUdeviceptr>(dst->inputPtr),
                dst->pitch, width, height,
                CU_MEMORYTYPE_DEVICE, dst->bufferFormat, dst->chromaOffsets,
                dst->numChromaPlanes);
        }
        else if(memoryType == CU_MEMORYTYPE_ARRAY)
        {
            const CUarray array = buffer->ToArray();

            NvEncoderCudaWithCUarray::CopyToDeviceFrame(
                context, static_cast<void*>(array), 0, static_cast<CUarray>(dst->inputPtr),
                dst->pitch, width, height,
                CU_MEMORYTYPE_ARRAY, dst->bufferFormat, dst->chromaOffsets,
                dst->numChromaPlanes);
        }
    }

    int32_t H264HardwareEncoder::Encode(
        const VideoFrame& frame, const std::vector<VideoFrameType>* frameTypes)
    {
        RTC_DCHECK_EQ(frame.width(), m_codec.width);
        RTC_DCHECK_EQ(frame.height(), m_codec.height);

        CUcontext current;
        if(!ck(cuCtxGetCurrent(&current)))
        {
            return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
        }
        if(current != m_context)
        {
            return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
        }
        if (m_encodedCompleteCallback == nullptr)
        {
            return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
        }

        bool send_key_frame = false;
        //if (m_configuration.key_frame_request && m_configuration.sending)
        //{
        //    send_key_frame = true;
        //}
        //if (!send_key_frame && frameTypes)
        //{
        //    for (size_t i = 0; i < frameTypes->size(); ++i)
        //    {
        //        if ((*frameTypes)[i] == VideoFrameType::kVideoFrameKey) {
        //            send_key_frame = true;
        //            break;
        //        }
        //    }
        //}
        if(m_keyframeRequest)
        {
            send_key_frame = true;
        }
        if (!send_key_frame && frameTypes)
        {
            if ((*frameTypes)[0] == VideoFrameType::kVideoFrameKey) {
                send_key_frame = true;
            }
        }

        GpuResourceBuffer* buffer = static_cast<GpuResourceBuffer*>(
            frame.video_frame_buffer().get());

        RTC_DCHECK_EQ(m_encoder->GetEncodeWidth(), buffer->width());
        RTC_DCHECK_EQ(m_encoder->GetEncodeHeight(), buffer->height());

        const NvEncInputFrame* dst = m_encoder->GetNextInputFrame();

        CopyResource(dst, buffer, m_context, m_memoryType);

        NV_ENC_PIC_PARAMS picParams = { NV_ENC_PIC_PARAMS_VER };
        picParams.encodePicFlags = 0;
        if (send_key_frame)
        {
            picParams.encodePicFlags =
                NV_ENC_PIC_FLAG_FORCEINTRA | NV_ENC_PIC_FLAG_FORCEIDR;
            m_keyframeRequest = false;
        }

        std::vector<std::vector<uint8_t>> vPacket;
        m_encoder->EncodeFrame(vPacket, &picParams);

        for (std::vector<uint8_t>& packet : vPacket)
        {
            int32_t result = ProcessEncodedFrame(packet, frame);
            if (result != WEBRTC_VIDEO_CODEC_OK)
            {
                return result;
            }
            m_bitrateAdjuster->Update(packet.size());

            int64_t now_ms = m_clock->TimeInMilliseconds();
            m_encode_fps.Update(1, now_ms);
        }
        return WEBRTC_VIDEO_CODEC_OK;
    }

    int32_t H264HardwareEncoder::ProcessEncodedFrame(
        std::vector<uint8_t>& packet, const VideoFrame& inputFrame)
    {
        m_encodedImage.SetTimestamp(inputFrame.timestamp());
        m_encodedImage._encodedWidth = inputFrame.video_frame_buffer()->width();
        m_encodedImage._encodedHeight = inputFrame.video_frame_buffer()->height();
        m_encodedImage.ntp_time_ms_ = inputFrame.ntp_time_ms();
        m_encodedImage.capture_time_ms_ = inputFrame.render_time_ms();
        m_encodedImage.rotation_ = inputFrame.rotation();
        m_encodedImage.content_type_ = VideoContentType::UNSPECIFIED;
        m_encodedImage.timing_.flags = VideoSendTiming::kInvalid;
        m_encodedImage._frameType = VideoFrameType::kVideoFrameDelta;
        m_encodedImage.SetColorSpace(inputFrame.color_space());
        std::vector<H264::NaluIndex> naluIndices =
            H264::FindNaluIndices(packet.data(), packet.size());
        for (uint32_t i = 0; i < naluIndices.size(); i++)
        {
            const H264::NaluType naluType = H264::ParseNaluType(packet[naluIndices[i].payload_start_offset]);
            if (naluType == H264::kIdr)
            {
                m_encodedImage._frameType = VideoFrameType::kVideoFrameKey;
                break;
            }
        }

        m_encodedImage.SetEncodedData(EncodedImageBuffer::Create(packet.data(), packet.size()));
        m_encodedImage.set_size(packet.size());

        m_h264BitstreamParser.ParseBitstream(m_encodedImage);
        m_encodedImage.qp_ = m_h264BitstreamParser.GetLastSliceQp().value_or(-1);

        CodecSpecificInfo codecInfo;
        codecInfo.codecType = kVideoCodecH264;
        codecInfo.codecSpecific.H264.packetization_mode = H264PacketizationMode::NonInterleaved;

        const auto result = m_encodedCompleteCallback->OnEncodedImage(m_encodedImage, &codecInfo);
        if (result.error != EncodedImageCallback::Result::OK)
        {
            RTC_LOG(LS_ERROR) << "Encode m_encodedCompleteCallback failed " <<  result.error;
            return WEBRTC_VIDEO_CODEC_ERROR;
        }
        return WEBRTC_VIDEO_CODEC_OK;
    }


    void H264HardwareEncoder::SetRates(const RateControlParameters& parameters)
    {
        if(m_encoder == nullptr)
        {
            RTC_LOG(LS_WARNING) << "while uninitialized.";
            return;
        }

        if (parameters.framerate_fps < 1.0)
        {
            RTC_LOG(LS_WARNING) << "Invalid frame rate: " << parameters.framerate_fps;
            return;
        }

        if (parameters.bitrate.get_sum_bps() == 0)
        {
            RTC_LOG(LS_WARNING) << "Encoder paused, turn off all encoding";
            SetStreamState(false);
            return;
        }

        m_codec.maxFramerate = static_cast<uint32_t>(parameters.framerate_fps + 0.5);
        const uint32_t frameRate = m_codec.maxFramerate;

        m_bitrateAdjuster->SetTargetBitrateBps(parameters.bitrate.get_sum_bps());
        const uint32_t bitRate = m_bitrateAdjuster->GetAdjustedBitrateBps();

        NV_ENC_RECONFIGURE_PARAMS reconfigureParams = { NV_ENC_RECONFIGURE_PARAMS_VER };
        memcpy(&reconfigureParams.reInitEncodeParams, &m_nvEncInitializeParams, sizeof(m_nvEncInitializeParams));
        NV_ENC_CONFIG reInitCodecConfig = { NV_ENC_CONFIG_VER };
        memcpy(&reInitCodecConfig, m_nvEncInitializeParams.encodeConfig, sizeof(reInitCodecConfig));
        reconfigureParams.reInitEncodeParams.encodeConfig = &reInitCodecConfig;

        // change framerate
        reconfigureParams.reInitEncodeParams.frameRateNum = frameRate;
        reconfigureParams.reInitEncodeParams.frameRateDen = 1;
        reconfigureParams.resetEncoder = 1;
        reconfigureParams.forceIDR = 1;

        // change bitrate
        reconfigureParams.reInitEncodeParams.encodeConfig->rcParams.averageBitRate = bitRate;
        m_encoder->Reconfigure(&reconfigureParams);

        // Force send Keyframe
        SetStreamState(true);
    }

    void H264HardwareEncoder::SetStreamState(bool sendStream)
    {
        m_keyframeRequest = sendStream;
    }
} // end namespace webrtc
} // end namespace unity

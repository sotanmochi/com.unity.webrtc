#include "pch.h"
#include "NvCodec.h"
#include "NvEncoder/NvEncoderCuda.h"
#include "NvEncoderImpl.h"

namespace unity
{
namespace webrtc
{
    using namespace ::webrtc;

    class NvEncoderCudaCapability : public NvEncoderCuda
    {
    public:
        NvEncoderCudaCapability(
            CUcontext cuContext)
            : NvEncoderCuda(cuContext, 0, 0, NV_ENC_BUFFER_FORMAT_UNDEFINED)
        {}

        std::vector<GUID> GetEncodeProfileGUIDs(GUID encodeGUID)
        {
            uint32_t count = 0;
            m_nvenc.nvEncGetEncodeProfileGUIDCount(
                m_hEncoder, encodeGUID, &count);
            uint32_t validCount = 0;
            std::vector<GUID> guids(count);
            m_nvenc.nvEncGetEncodeProfileGUIDs(
                m_hEncoder, encodeGUID, guids.data(), count, &validCount);
            return guids;
        }

        int GetLevelMax(GUID encodeGUID)
        {
            return GetCapabilityValue(encodeGUID, NV_ENC_CAPS_LEVEL_MAX);
        }
        int GetLevelMin(GUID encodeGUID)
        {
            return GetCapabilityValue(encodeGUID, NV_ENC_CAPS_LEVEL_MIN);
        }
        int GetMaxWidth(GUID encodeGUID)
        {
            return GetCapabilityValue(encodeGUID, NV_ENC_CAPS_WIDTH_MAX);
        }
        int GetMaxHeight(GUID encodeGUID)
        {
            return GetCapabilityValue(encodeGUID, NV_ENC_CAPS_HEIGHT_MAX);
        }
    };

    absl::optional<H264Profile> GuidToProfile(GUID& guid)
    {
        if (guid == NV_ENC_H264_PROFILE_BASELINE_GUID)
            return H264Profile::kProfileBaseline;
        if (guid == NV_ENC_H264_PROFILE_MAIN_GUID)
            return H264Profile::kProfileMain;
        if (guid == NV_ENC_H264_PROFILE_HIGH_GUID)
            return H264Profile::kProfileHigh;
        if (guid == NV_ENC_H264_PROFILE_CONSTRAINED_HIGH_GUID)
            return H264Profile::kProfileConstrainedHigh;
        return absl::nullopt;
    }

    std::vector<SdpVideoFormat> SupportedNvEncoderCodecs(CUcontext context)
    {
        const H264Level levels[] = {
            H264Level::kLevel1_b, H264Level::kLevel1,   H264Level::kLevel1_1, H264Level::kLevel1_2,
            H264Level::kLevel1_3, H264Level::kLevel2,   H264Level::kLevel2_1, H264Level::kLevel2_2,
            H264Level::kLevel3,   H264Level::kLevel3_1, H264Level::kLevel3_2, H264Level::kLevel4,
            H264Level::kLevel4_1, H264Level::kLevel4_2, H264Level::kLevel5,   H264Level::kLevel5_1,
            H264Level::kLevel5_2
        };

        auto encoder = std::make_unique<NvEncoderCudaCapability>(context);

        int maxLevel = encoder->GetLevelMax(NV_ENC_CODEC_H264_GUID);
        H264Level supportedMaxLevel = static_cast<H264Level>(maxLevel);

        std::vector<GUID> profileGUIDs =
            encoder->GetEncodeProfileGUIDs(NV_ENC_CODEC_H264_GUID);

        std::vector<H264Profile> supportedProfiles;
        for (auto& guid : profileGUIDs) {
            absl::optional <H264Profile> profile = GuidToProfile(guid);
            if (profile.has_value())
                supportedProfiles.push_back(profile.value());
        }

        std::vector<SdpVideoFormat> supportedFormats;
        for (auto& profile : supportedProfiles) {
            supportedFormats.push_back(
                CreateH264Format(profile, supportedMaxLevel, "1"));
        }
        return supportedFormats;
    }

    std::vector<SdpVideoFormat> SupportedNvDecoderCodecs(CUcontext context)
    {
        std::vector<SdpVideoFormat> supportedFormats;
        // todo
        return supportedFormats;
    }

    std::unique_ptr<NvEncoder> NvEncoder::Create(
        const cricket::VideoCodec& codec,
        CUcontext context, CUmemorytype memoryType, NV_ENC_BUFFER_FORMAT format)
    {
        return std::make_unique<NvEncoderImpl>(
            codec, context, memoryType, format);
    }
}
}

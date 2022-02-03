#include "pch.h"
#include "UnityVideoDecoderFactory.h"
#include "absl/strings/match.h"

#if CUDA_PLATFORM
#include <cuda.h>
#include "Codec/NvCodec/NvCodec.h"
#endif

#if UNITY_OSX || UNITY_IOS
#import "sdk/objc/components/video_codec/RTCDefaultVideoDecoderFactory.h"
#import "sdk/objc/native/api/video_decoder_factory.h"
#elif UNITY_ANDROID
#include "Codec/AndroidCodec/android_codec_factory_helper.h"
#endif

using namespace ::webrtc::H264;

namespace unity
{
namespace webrtc
{
    bool UnityVideoDecoderFactory::IsFormatSupported(
        const std::vector<webrtc::SdpVideoFormat>& supported_formats,
        const webrtc::SdpVideoFormat& format) const
    {
        for (const webrtc::SdpVideoFormat& supported_format : supported_formats)
        {
            if (cricket::IsSameCodec(format.name, format.parameters,
                supported_format.name,
                supported_format.parameters))
            {
                return true;
            }
        }
        return false;
    }

    webrtc::VideoDecoderFactory* CreateDecoderFactory()
    {
#if UNITY_OSX || UNITY_IOS
        return webrtc::ObjCToNativeVideoDecoderFactory(
            [[RTCDefaultVideoDecoderFactory alloc] init]).release();
#elif UNITY_ANDROID
        return CreateAndroidDecoderFactory().release();
#else
        return new webrtc::InternalDecoderFactory();
#endif
    }

    UnityVideoDecoderFactory::UnityVideoDecoderFactory(
        IGraphicsDevice* gfxDevice, bool forTest)
    : gfxDevice_(gfxDevice)
    , internal_decoder_factory_(CreateDecoderFactory())
    , forTest_(forTest)
    {
    }

    UnityVideoDecoderFactory::~UnityVideoDecoderFactory() = default;

    std::vector<webrtc::SdpVideoFormat> UnityVideoDecoderFactory::GetHardwareDecoderFormats() const
    {
#if CUDA_PLATFORM
        CUcontext context = gfxDevice_->GetCuContext();
        return SupportedNvDecoderCodecs(context);
#else
        auto formats = internal_decoder_factory_->GetSupportedFormats();
        std::vector<webrtc::SdpVideoFormat> filtered;
        std::copy_if(formats.begin(), formats.end(), std::back_inserter(filtered),
            [](webrtc::SdpVideoFormat format) {
                if(format.name.find("H264") == std::string::npos)
                    return false;
                return true;
            });
        return filtered;
#endif
    }

    std::vector<webrtc::SdpVideoFormat> UnityVideoDecoderFactory::GetSupportedFormats() const
    {
        std::vector<SdpVideoFormat> supported_codecs;

        for (const webrtc::SdpVideoFormat& format : internal_decoder_factory_->GetSupportedFormats())
            supported_codecs.push_back(format);
        for (const webrtc::SdpVideoFormat& format : GetHardwareDecoderFormats())
            supported_codecs.push_back(format);
        return supported_codecs;
    }

    std::unique_ptr<webrtc::VideoDecoder> UnityVideoDecoderFactory::CreateVideoDecoder(const webrtc::SdpVideoFormat & format)
    {
#if CUDA_PLATFORM
        if (IsFormatSupported(GetHardwareDecoderFormats(), format))
        {
            CUcontext context = gfxDevice_->GetCuContext();
            if (absl::EqualsIgnoreCase(format.name, cricket::kH264CodecName))
                return NvDecoder::Create(context);
        }
#endif

        if (absl::EqualsIgnoreCase(format.name, cricket::kAv1CodecName))
        {
            RTC_LOG(LS_INFO) << "AV1 codec is not supported";
            return nullptr;
        }
        return internal_decoder_factory_->CreateVideoDecoder(format);
    }

}  // namespace webrtc
}  // namespace unity

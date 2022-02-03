#pragma once
#include "GraphicsDevice/IGraphicsDevice.h"

namespace unity
{
namespace webrtc
{
    class IGraphicsDevice;
    using namespace ::webrtc;

    // This class is only used for status testing.
    class UnityVideoDecoderFactory : public webrtc::VideoDecoderFactory
    {
    public:
        virtual std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
        virtual std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(const webrtc::SdpVideoFormat& format) override;
        virtual std::vector<SdpVideoFormat> GetHardwareDecoderFormats() const;
        virtual bool IsFormatSupported(const std::vector<webrtc::SdpVideoFormat>& supported_formats,const webrtc::SdpVideoFormat& format) const;

        UnityVideoDecoderFactory(IGraphicsDevice* gfxDevice, bool forTest);
        ~UnityVideoDecoderFactory();
    private:
        IGraphicsDevice* gfxDevice_;
        const std::unique_ptr<VideoDecoderFactory> internal_decoder_factory_;
        bool forTest_;
    };
}
}

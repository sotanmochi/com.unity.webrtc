#pragma once
#include "GraphicsDevice/IGraphicsDevice.h"

namespace unity
{
namespace webrtc
{
    class IGraphicsDevice;
    using namespace ::webrtc;

    class UnityVideoEncoderFactory : public VideoEncoderFactory
    {
    public:
        //VideoEncoderFactory
        // Returns a list of supported video formats in order of preference, to use
        // for signaling etc.
        virtual std::vector<SdpVideoFormat> GetSupportedFormats() const override;
        // Returns information about how this format will be encoded. The specified
        // format must be one of the supported formats by this factory.
        virtual CodecInfo QueryVideoEncoder(const SdpVideoFormat& format) const override;
        // Creates a VideoEncoder for the specified format.
        virtual std::unique_ptr<VideoEncoder> CreateVideoEncoder(const SdpVideoFormat& format) override;

        virtual std::vector<SdpVideoFormat> GetHardwareEncoderFormats() const;
        virtual bool IsFormatSupported(const std::vector<webrtc::SdpVideoFormat>& supported_formats,const webrtc::SdpVideoFormat& format) const;

        UnityVideoEncoderFactory(IGraphicsDevice* gfxDevice);
        ~UnityVideoEncoderFactory();
    private:
        IGraphicsDevice* gfxDevice_;
        const std::unique_ptr<VideoEncoderFactory> internal_encoder_factory_;
    };
}
}

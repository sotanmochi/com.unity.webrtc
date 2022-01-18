#include "pch.h"
#include "VideoFrame.h"
#include "GpuMemoryBuffer.h"
#include "Codec/NvCodec/NvCodec.h"
#include "../NvCodec/Utils/NvCodecUtils.h"

namespace unity {
namespace webrtc {

class NvCodecTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        EXPECT_TRUE(
            ck(cuInit(0)));

        EXPECT_TRUE(
            ck(cuDeviceGet(&device_, 0)));

        EXPECT_TRUE(
            ck(cuCtxCreate(&context_, 0, device_)));
    }

    void TearDown() override
    {
        cuCtxDestroy(context_);
    }
protected:
    CUdevice device_;
    CUcontext context_;
};

TEST_F(NvCodecTest, SupportedNvEncoderCodecs)
{
    std::vector<SdpVideoFormat> formats =
        SupportedNvEncoderCodecs(context_);
    EXPECT_GT(formats.size(), 0);
}

}
}

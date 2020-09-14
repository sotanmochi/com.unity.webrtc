#include "pch.h"
#include "GraphicsDeviceTestBase.h"

namespace unity
{
namespace webrtc
{

class CudaDeviceTest : public GraphicsDeviceTestBase {};

TEST_P(CudaDeviceTest, GetCuContext) {
    EXPECT_NE(m_device->GetCuContext(), nullptr);
}

TEST_P(CudaDeviceTest, IsNvSupported) {
    EXPECT_TRUE(m_device->IsNvCodecSupport());
}

INSTANTIATE_TEST_CASE_P(GraphicsDeviceParameters,
    CudaDeviceTest, testing::ValuesIn(VALUES_TEST_ENV));

} // end namespace webrtc
} // end namespace unity

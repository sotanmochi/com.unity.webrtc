#pragma once

namespace unity
{
namespace webrtc
{
    PFN_vkGetInstanceProcAddr InterceptVulkanInitialization(
        PFN_vkGetInstanceProcAddr getInstanceProcAddr, void* userdata);
} // end namespace webrtc
} // end namespace unity

#include "pch.h"

#include "CudaContext.h"

#include <array>

#if defined(_WIN32)
#include <cudaD3D11.h>
#include <wrl/client.h>
#endif


#include "NvCodecUtils.h"
#include "GraphicsDevice/Vulkan/VulkanUtility.h"

#if defined(_WIN32)
using namespace Microsoft::WRL;
#endif

namespace unity
{
namespace webrtc
{

static void* s_hModule = nullptr;

CudaContext::CudaContext() : m_context(nullptr) {
}
//---------------------------------------------------------------------------------------------------------------------
CUresult LoadModule() {
    // dll check
    if (s_hModule == nullptr)
    {
        // dll delay load
#if defined(_WIN32)
        HMODULE module = LoadLibrary(TEXT("nvcuda.dll"));
        if (module == nullptr)
        {
            LogPrint("nvcuda.dll is not found. Please be sure the environment supports CUDA API.");
            return CUDA_ERROR_NOT_FOUND;
        }
        s_hModule = module;
#else
#endif
    }
    return CUDA_SUCCESS;
}
//---------------------------------------------------------------------------------------------------------------------

CUresult CudaContext::Init(const VkInstance instance, VkPhysicalDevice physicalDevice) {

    // dll check
    CUresult result = LoadModule();
    if (result != CUDA_SUCCESS) {
        return result;
    }

    CUdevice dev;
    bool foundDevice = true;

    result = cuInit(0);
    if (result != CUDA_SUCCESS) {
        return result;
    }

    int numDevices = 0;
    result = cuDeviceGetCount(&numDevices);
    if (result != CUDA_SUCCESS) {
        return result;
    }

    CUuuid id = {};
    std::array<uint8_t, VK_UUID_SIZE> deviceUUID;
    if (!VulkanUtility::GetPhysicalDeviceUUIDInto(instance, physicalDevice, &deviceUUID)) {
        return CUDA_ERROR_INVALID_DEVICE;
    }

    //Loop over the available devices and identify the CUdevice  corresponding to the physical device in use by
    //this Vulkan instance. This is required because there is no other way to match GPUs across API boundaries.
    for (int i = 0; i < numDevices; i++) {
        cuDeviceGet(&dev, i);

        cuDeviceGetUuid(&id, dev);

        if (!std::memcmp(static_cast<const void *>(&id),
                static_cast<const void *>(deviceUUID.data()),
                sizeof(CUuuid))) {
            foundDevice = true;
            break;
        }
    }

    if (!foundDevice) {
        return CUDA_ERROR_NO_DEVICE;
 
    }

    result = cuCtxCreate(&m_context, 0, dev);
    return result;
}
//---------------------------------------------------------------------------------------------------------------------

#if defined(_WIN32)
CUresult CudaContext::Init(ID3D11Device* device) {

    // dll check
    CUresult result = LoadModule();
    if (result != CUDA_SUCCESS) {
        return result;
    }

    result = cuInit(0);
    if (result != CUDA_SUCCESS) {
        return result;
    }
    int numDevices = 0;
    result = cuDeviceGetCount(&numDevices);
    if (result != CUDA_SUCCESS) {
        return result;
    }

    ComPtr<IDXGIDevice> pDxgiDevice = nullptr;
    if (!ck(device->QueryInterface(IID_PPV_ARGS(&pDxgiDevice))))
    {
        return CUDA_ERROR_NO_DEVICE;
    }
    ComPtr<IDXGIAdapter> pDxgiAdapter = nullptr;
    if (!ck(pDxgiDevice->GetAdapter(&pDxgiAdapter)))
    {
        return CUDA_ERROR_NO_DEVICE;
    }
    CUdevice dev;
    if (!ck(cuD3D11GetDevice(&dev, pDxgiAdapter.Get())))
    {
        return CUDA_ERROR_NO_DEVICE;
    }

    result = cuCtxCreate(&m_context, 0, dev);
    return result;
}
#endif
//---------------------------------------------------------------------------------------------------------------------

CUcontext CudaContext::GetContextOnThread() const
{
    RTC_DCHECK(m_context);

    CUcontext current;
    if (!ck(cuCtxGetCurrent(&current)))
    {
        throw;
    }
    if(m_context == current)
    {
        return m_context;
    }
    if (!ck(cuCtxSetCurrent(m_context)))
    {
        throw;
    }
    return m_context;
}

//---------------------------------------------------------------------------------------------------------------------

void CudaContext::Shutdown() {
    if (nullptr != m_context) {
        cuCtxDestroy(m_context);
        m_context = nullptr;
    }
    if (s_hModule)
    {
#if _WIN32
        FreeLibrary((HMODULE)s_hModule);
#else
#endif
        s_hModule = nullptr;
    }
}

} // end namespace webrtc
} // end namespace unity

target_sources(WebRTCLib
  PRIVATE
#    NvEncoderCuda.cpp
#    NvEncoderCuda.h
    NvEncoderCudaWithCUarray.cpp
    NvEncoderCudaWithCUarray.h
)

# if(Windows)
#   target_sources(WebRTCLib
#     PRIVATE
#       NvEncoderD3D11.cpp
#       NvEncoderD3D11.h
#       NvEncoderD3D12.cpp
#       NvEncoderD3D12.h
#       NvEncoderGL.cpp
#       NvEncoderGL.h
#   )
# endif()
# 
# if(Linux)
#   target_sources(WebRTCLib
#     PRIVATE
#       NvEncoderGL.cpp
#       NvEncoderGL.h
#   )
# endif()

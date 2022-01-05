#pragma once
#include "VideoFrame.h"

namespace unity {
namespace webrtc {

rtc::scoped_refptr<VideoFrame> CreateTestFrame(
    const Size& size);

} // end namespace webrtc
} // end namespace unity

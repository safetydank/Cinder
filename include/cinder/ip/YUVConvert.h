#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"

namespace cinder { namespace ip {

enum YUVFormat
{
    YUV_NV21=1,
    YUV_NV12=2,
    YUV_YV12=3,
    YUV_IYUV=4,
    YUV_Unknown=11
};

void YUVConvert(const uint8_t *yuv_data, YUVFormat format, int32_t width, int32_t height, Surface8u *surface);

} } // namespace cinder::ip
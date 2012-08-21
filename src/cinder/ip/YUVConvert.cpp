
#include "cinder/ip/YUVConvert.h"
#include <limits>
#include <math.h>

namespace cinder { namespace ip {

template<typename _Tp> static inline _Tp saturate_cast(uint8_t v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(signed char v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(unsigned short v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(short v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(unsigned v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(int v) { return _Tp(v); }

template<> inline uint8_t saturate_cast<uint8_t>(int8_t v)
{ return (uint8_t)std::max((int)v, 0); }
template<> inline uint8_t saturate_cast<uint8_t>(unsigned short v)
{ return (uint8_t)std::min((unsigned)v, (unsigned)UCHAR_MAX); }
template<> inline uint8_t saturate_cast<uint8_t>(int v)
{ return (uint8_t)((unsigned)v <= UCHAR_MAX ? v : v > 0 ? UCHAR_MAX : 0); }
template<> inline uint8_t saturate_cast<uint8_t>(short v)
{ return saturate_cast<uint8_t>((int)v); }
template<> inline uint8_t saturate_cast<uint8_t>(unsigned v)
{ return (uint8_t)std::min(v, (unsigned)UCHAR_MAX); }

const int ITUR_BT_601_CY = 1220542;
const int ITUR_BT_601_CUB = 2116026;
const int ITUR_BT_601_CUG = -409993;
const int ITUR_BT_601_CVG = -852492;
const int ITUR_BT_601_CVR = 1673527;
const int ITUR_BT_601_SHIFT = 20;

template<int bIdx, int uIdx>
struct YUV420sp2RGB888Invoker
{
    uint8_t* dst;
    const uint8_t* my1, *muv;
    int width, stride;

    YUV420sp2RGB888Invoker(uint8_t* _dst, int _width, int _stride, const uint8_t* _y1, const uint8_t* _uv)
        : dst(_dst), my1(_y1), muv(_uv), width(_width), stride(_stride) {}

    void operator()(int begin, int end) const
    {
        int rangeBegin = begin * 2;
        int rangeEnd = end * 2;

        //R = 1.164(Y - 16) + 1.596(V - 128)
        //G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
        //B = 1.164(Y - 16)                  + 2.018(U - 128)

        //R = (1220542(Y - 16) + 1673527(V - 128)                  + (1 << 19)) >> 20
        //G = (1220542(Y - 16) - 852492(V - 128) - 409993(U - 128) + (1 << 19)) >> 20
        //B = (1220542(Y - 16)                  + 2116026(U - 128) + (1 << 19)) >> 20

        const uint8_t* y1 = my1 + rangeBegin * stride, *uv = muv + rangeBegin * stride / 2;

        for (int j = rangeBegin; j < rangeEnd; j += 2, y1 += stride * 2, uv += stride)
        {
            uint8_t* row1 = dst + j * (width*3);
            uint8_t* row2 = row1 + (width*3);
            const uint8_t* y2 = y1 + stride;

            for (int i = 0; i < width; i += 2, row1 += 6, row2 += 6)
            {
                int u = int(uv[i + 0 + uIdx]) - 128;
                int v = int(uv[i + 1 - uIdx]) - 128;

                int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
                int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
                int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

                int y00 = std::max(0, int(y1[i]) - 16) * ITUR_BT_601_CY;
                row1[2-bIdx] = saturate_cast<uint8_t>((y00 + ruv) >> ITUR_BT_601_SHIFT);
                row1[1]      = saturate_cast<uint8_t>((y00 + guv) >> ITUR_BT_601_SHIFT);
                row1[bIdx]   = saturate_cast<uint8_t>((y00 + buv) >> ITUR_BT_601_SHIFT);

                int y01 = std::max(0, int(y1[i + 1]) - 16) * ITUR_BT_601_CY;
                row1[5-bIdx] = saturate_cast<uint8_t>((y01 + ruv) >> ITUR_BT_601_SHIFT);
                row1[4]      = saturate_cast<uint8_t>((y01 + guv) >> ITUR_BT_601_SHIFT);
                row1[3+bIdx] = saturate_cast<uint8_t>((y01 + buv) >> ITUR_BT_601_SHIFT);

                int y10 = std::max(0, int(y2[i]) - 16) * ITUR_BT_601_CY;
                row2[2-bIdx] = saturate_cast<uint8_t>((y10 + ruv) >> ITUR_BT_601_SHIFT);
                row2[1]      = saturate_cast<uint8_t>((y10 + guv) >> ITUR_BT_601_SHIFT);
                row2[bIdx]   = saturate_cast<uint8_t>((y10 + buv) >> ITUR_BT_601_SHIFT);

                int y11 = std::max(0, int(y2[i + 1]) - 16) * ITUR_BT_601_CY;
                row2[5-bIdx] = saturate_cast<uint8_t>((y11 + ruv) >> ITUR_BT_601_SHIFT);
                row2[4]      = saturate_cast<uint8_t>((y11 + guv) >> ITUR_BT_601_SHIFT);
                row2[3+bIdx] = saturate_cast<uint8_t>((y11 + buv) >> ITUR_BT_601_SHIFT);
            }
        }
    }
};

template<int bIdx, int uIdx>
struct YUV420sp2RGBA8888Invoker
{
    uint8_t* dst;
    const uint8_t* my1, *muv;
    int width, stride;

    YUV420sp2RGBA8888Invoker(uint8_t* _dst, int _width, int _stride, const uint8_t* _y1, const uint8_t* _uv)
        : dst(_dst), my1(_y1), muv(_uv), width(_width), stride(_stride) {}

    void operator()(int begin, int end) const
    {
        int rangeBegin = begin * 2;
        int rangeEnd = end * 2;

        //R = 1.164(Y - 16) + 1.596(V - 128)
        //G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
        //B = 1.164(Y - 16)                  + 2.018(U - 128)

        //R = (1220542(Y - 16) + 1673527(V - 128)                  + (1 << 19)) >> 20
        //G = (1220542(Y - 16) - 852492(V - 128) - 409993(U - 128) + (1 << 19)) >> 20
        //B = (1220542(Y - 16)                  + 2116026(U - 128) + (1 << 19)) >> 20

        const uint8_t* y1 = my1 + rangeBegin * stride, *uv = muv + rangeBegin * stride / 2;

        for (int j = rangeBegin; j < rangeEnd; j += 2, y1 += stride * 2, uv += stride)
        {
            uint8_t* row1 = dst + j * (width*4);
            uint8_t* row2 = row1 + (width*4);
            const uint8_t* y2 = y1 + stride;

            for (int i = 0; i < width; i += 2, row1 += 8, row2 += 8)
            {
                int u = int(uv[i + 0 + uIdx]) - 128;
                int v = int(uv[i + 1 - uIdx]) - 128;

                int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
                int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
                int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

                int y00 = std::max(0, int(y1[i]) - 16) * ITUR_BT_601_CY;
                row1[2-bIdx] = saturate_cast<uint8_t>((y00 + ruv) >> ITUR_BT_601_SHIFT);
                row1[1]      = saturate_cast<uint8_t>((y00 + guv) >> ITUR_BT_601_SHIFT);
                row1[bIdx]   = saturate_cast<uint8_t>((y00 + buv) >> ITUR_BT_601_SHIFT);
                row1[3]      = uint8_t(0xff);

                int y01 = std::max(0, int(y1[i + 1]) - 16) * ITUR_BT_601_CY;
                row1[6-bIdx] = saturate_cast<uint8_t>((y01 + ruv) >> ITUR_BT_601_SHIFT);
                row1[5]      = saturate_cast<uint8_t>((y01 + guv) >> ITUR_BT_601_SHIFT);
                row1[4+bIdx] = saturate_cast<uint8_t>((y01 + buv) >> ITUR_BT_601_SHIFT);
                row1[7]      = uint8_t(0xff);

                int y10 = std::max(0, int(y2[i]) - 16) * ITUR_BT_601_CY;
                row2[2-bIdx] = saturate_cast<uint8_t>((y10 + ruv) >> ITUR_BT_601_SHIFT);
                row2[1]      = saturate_cast<uint8_t>((y10 + guv) >> ITUR_BT_601_SHIFT);
                row2[bIdx]   = saturate_cast<uint8_t>((y10 + buv) >> ITUR_BT_601_SHIFT);
                row2[3]      = uint8_t(0xff);

                int y11 = std::max(0, int(y2[i + 1]) - 16) * ITUR_BT_601_CY;
                row2[6-bIdx] = saturate_cast<uint8_t>((y11 + ruv) >> ITUR_BT_601_SHIFT);
                row2[5]      = saturate_cast<uint8_t>((y11 + guv) >> ITUR_BT_601_SHIFT);
                row2[4+bIdx] = saturate_cast<uint8_t>((y11 + buv) >> ITUR_BT_601_SHIFT);
                row2[7]      = uint8_t(0xff);
            }
        }
    }
};

template<int bIdx>
struct YUV420p2RGB888Invoker
{
    uint8_t* dst;
    const uint8_t* my1, *mu, *mv;
    int width, stride;
    int ustepIdx, vstepIdx;

    YUV420p2RGB888Invoker(uint8_t* _dst, int _width, int _stride, const uint8_t* _y1, const uint8_t* _u, const uint8_t* _v, int _ustepIdx, int _vstepIdx)
        : dst(_dst), my1(_y1), mu(_u), mv(_v), width(_width), stride(_stride), ustepIdx(_ustepIdx), vstepIdx(_vstepIdx) {}

    void operator()(int begin, int end) const
    {
        const int rangeBegin = begin * 2;
        const int rangeEnd = end * 2;
        
        size_t uvsteps[2] = {width/2, stride - width/2};
        int usIdx = ustepIdx, vsIdx = vstepIdx;

        const uint8_t* y1 = my1 + rangeBegin * stride;
        const uint8_t* u1 = mu + (begin / 2) * stride;
        const uint8_t* v1 = mv + (begin / 2) * stride;
        
        if(begin % 2 == 1)
        {
            u1 += uvsteps[(usIdx++) & 1];
            v1 += uvsteps[(vsIdx++) & 1];
        }

        for (int j = rangeBegin; j < rangeEnd; j += 2, y1 += stride * 2, u1 += uvsteps[(usIdx++) & 1], v1 += uvsteps[(vsIdx++) & 1])
        {
            uint8_t* row1 = dst + j * (width*3);
            uint8_t* row2 = row1 + (width*3);
            const uint8_t* y2 = y1 + stride;

            for (int i = 0; i < width / 2; i += 1, row1 += 6, row2 += 6)
            {
                int u = int(u1[i]) - 128;
                int v = int(v1[i]) - 128;

                int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
                int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
                int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

                int y00 = std::max(0, int(y1[2 * i]) - 16) * ITUR_BT_601_CY;
                row1[2-bIdx] = saturate_cast<uint8_t>((y00 + ruv) >> ITUR_BT_601_SHIFT);
                row1[1]      = saturate_cast<uint8_t>((y00 + guv) >> ITUR_BT_601_SHIFT);
                row1[bIdx]   = saturate_cast<uint8_t>((y00 + buv) >> ITUR_BT_601_SHIFT);

                int y01 = std::max(0, int(y1[2 * i + 1]) - 16) * ITUR_BT_601_CY;
                row1[5-bIdx] = saturate_cast<uint8_t>((y01 + ruv) >> ITUR_BT_601_SHIFT);
                row1[4]      = saturate_cast<uint8_t>((y01 + guv) >> ITUR_BT_601_SHIFT);
                row1[3+bIdx] = saturate_cast<uint8_t>((y01 + buv) >> ITUR_BT_601_SHIFT);

                int y10 = std::max(0, int(y2[2 * i]) - 16) * ITUR_BT_601_CY;
                row2[2-bIdx] = saturate_cast<uint8_t>((y10 + ruv) >> ITUR_BT_601_SHIFT);
                row2[1]      = saturate_cast<uint8_t>((y10 + guv) >> ITUR_BT_601_SHIFT);
                row2[bIdx]   = saturate_cast<uint8_t>((y10 + buv) >> ITUR_BT_601_SHIFT);

                int y11 = std::max(0, int(y2[2 * i + 1]) - 16) * ITUR_BT_601_CY;
                row2[5-bIdx] = saturate_cast<uint8_t>((y11 + ruv) >> ITUR_BT_601_SHIFT);
                row2[4]      = saturate_cast<uint8_t>((y11 + guv) >> ITUR_BT_601_SHIFT);
                row2[3+bIdx] = saturate_cast<uint8_t>((y11 + buv) >> ITUR_BT_601_SHIFT);
            }
        }
    }
};

template<int bIdx>
struct YUV420p2RGBA8888Invoker
{
    uint8_t* dst;
    const uint8_t* my1, *mu, *mv;
    int width, stride;
    int ustepIdx, vstepIdx;

    YUV420p2RGBA8888Invoker(uint8_t* _dst, int _width, int _stride, const uint8_t* _y1, const uint8_t* _u, const uint8_t* _v, int _ustepIdx, int _vstepIdx)
        : dst(_dst), my1(_y1), mu(_u), mv(_v), width(_width), stride(_stride), ustepIdx(_ustepIdx), vstepIdx(_vstepIdx) {}

    void operator()(int begin, int end) const
    {
        int rangeBegin = begin * 2;
        int rangeEnd = end * 2;

        size_t uvsteps[2] = {width/2, stride - width/2};
        int usIdx = ustepIdx, vsIdx = vstepIdx;

        const uint8_t* y1 = my1 + rangeBegin * stride;
        const uint8_t* u1 = mu + (begin / 2) * stride;
        const uint8_t* v1 = mv + (begin / 2) * stride;
        
        if(begin % 2 == 1)
        {
            u1 += uvsteps[(usIdx++) & 1];
            v1 += uvsteps[(vsIdx++) & 1];
        }

        for (int j = rangeBegin; j < rangeEnd; j += 2, y1 += stride * 2, u1 += uvsteps[(usIdx++) & 1], v1 += uvsteps[(vsIdx++) & 1])
        {
            uint8_t* row1 = dst + j * (width*4);
            uint8_t* row2 = row1 + (width*4);
            const uint8_t* y2 = y1 + stride;

            for (int i = 0; i < width / 2; i += 1, row1 += 8, row2 += 8)
            {
                int u = int(u1[i]) - 128;
                int v = int(v1[i]) - 128;

                int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
                int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
                int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

                int y00 = std::max(0, int(y1[2 * i]) - 16) * ITUR_BT_601_CY;
                row1[2-bIdx] = saturate_cast<uint8_t>((y00 + ruv) >> ITUR_BT_601_SHIFT);
                row1[1]      = saturate_cast<uint8_t>((y00 + guv) >> ITUR_BT_601_SHIFT);
                row1[bIdx]   = saturate_cast<uint8_t>((y00 + buv) >> ITUR_BT_601_SHIFT);
                row1[3]      = uint8_t(0xff);

                int y01 = std::max(0, int(y1[2 * i + 1]) - 16) * ITUR_BT_601_CY;
                row1[6-bIdx] = saturate_cast<uint8_t>((y01 + ruv) >> ITUR_BT_601_SHIFT);
                row1[5]      = saturate_cast<uint8_t>((y01 + guv) >> ITUR_BT_601_SHIFT);
                row1[4+bIdx] = saturate_cast<uint8_t>((y01 + buv) >> ITUR_BT_601_SHIFT);
                row1[7]      = uint8_t(0xff);

                int y10 = std::max(0, int(y2[2 * i]) - 16) * ITUR_BT_601_CY;
                row2[2-bIdx] = saturate_cast<uint8_t>((y10 + ruv) >> ITUR_BT_601_SHIFT);
                row2[1]      = saturate_cast<uint8_t>((y10 + guv) >> ITUR_BT_601_SHIFT);
                row2[bIdx]   = saturate_cast<uint8_t>((y10 + buv) >> ITUR_BT_601_SHIFT);
                row2[3]      = uint8_t(0xff);

                int y11 = std::max(0, int(y2[2 * i + 1]) - 16) * ITUR_BT_601_CY;
                row2[6-bIdx] = saturate_cast<uint8_t>((y11 + ruv) >> ITUR_BT_601_SHIFT);
                row2[5]      = saturate_cast<uint8_t>((y11 + guv) >> ITUR_BT_601_SHIFT);
                row2[4+bIdx] = saturate_cast<uint8_t>((y11 + buv) >> ITUR_BT_601_SHIFT);
                row2[7]      = uint8_t(0xff);
            }
        }
    }
};
/*
template<int bIdx, int uIdx>
inline void cvtYUV420sp2RGB(uint8_t* _dst, int _width, int _height, int _stride, const uint8_t* _y1, const uint8_t* _uv)
{
    YUV420sp2RGB888Invoker<bIdx, uIdx> converter(_dst, _width, _stride, _y1,  _uv);
    converter(0, _height/2);
}

template<int bIdx, int uIdx>
inline void cvtYUV420sp2RGBA(uint8_t* _dst, int _width, int _height, int _stride, const uint8_t* _y1, const uint8_t* _uv)
{
    YUV420sp2RGBA8888Invoker<bIdx, uIdx> converter(_dst, _width, _stride, _y1,  _uv);
    converter(0, _height/2);
}

template<int bIdx>
inline void cvtYUV420p2RGB(uint8_t* _dst, int _width, int _height, int _stride, const uint8_t* _y1, const uint8_t* _u, const uint8_t* _v, int ustepIdx, int vstepIdx)
{
    YUV420p2RGB888Invoker<bIdx> converter(_dst, _width, _stride, _y1,  _u, _v, ustepIdx, vstepIdx);
    converter(0, _height/2);
}

template<int bIdx>
inline void cvtYUV420p2RGBA(uint8_t* _dst, int _width, int _height, int _stride, const uint8_t* _y1, const uint8_t* _u, const uint8_t* _v, int ustepIdx, int vstepIdx)
{
    YUV420p2RGBA8888Invoker<bIdx> converter(_dst, _width, _stride, _y1,  _u, _v, ustepIdx, vstepIdx);
    converter(0, _height/2);
}
*/
void YUVConvert(const uint8_t *yuv_data, YUVFormat format, int32_t width, int32_t height, Surface8u *surface)
{

    const int dst_ch = surface->getPixelInc();
    const int ch_order = surface->getChannelOrder().getCode();
    switch( format )
    {
        case YUV_NV21:  case YUV_NV12:
            {
                const int bidx = (ch_order==SurfaceChannelOrder::BGRA || ch_order==SurfaceChannelOrder::BGRX || ch_order==SurfaceChannelOrder::BGR) ? 0 : 2;
                const int uidx = (format==YUV_NV21) ? 1 : 0;

                const int dst_w = width;
                const int dst_h = height * 2 / 3;
                const uint8_t* y = yuv_data;
                const uint8_t* uv = y + width * dst_h;

                switch(dst_ch*100 + bidx * 10 + uidx)
                {
                    case 300: YUV420sp2RGB888Invoker<0, 0> (surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 301: YUV420sp2RGB888Invoker<0, 1> (surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 320: YUV420sp2RGB888Invoker<2, 0> (surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 321: YUV420sp2RGB888Invoker<2, 1> (surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 400: YUV420sp2RGBA8888Invoker<0, 0>(surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 401: YUV420sp2RGBA8888Invoker<0, 1>(surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 420: YUV420sp2RGBA8888Invoker<2, 0>(surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                    case 421: YUV420sp2RGBA8888Invoker<2, 1>(surface->getData(), dst_w, width, y, uv)(0, dst_h/2); break;
                }
            }
            break;
        case YUV_YV12: case YUV_IYUV:
            {
                //TODO
            }
            break;
    }
}

} } //namespace
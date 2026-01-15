#pragma once
extern "C"
{
#pragma warning(push)
#pragma warning(disable:4819)
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#pragma warning(pop)
}

#include <memory>
class CVideo
{
    friend class CMedia;
public:
    CVideo();
    ~CVideo();

private:
    std::unique_ptr<AVCodecContext> m_pCodecCtx;
    std::unique_ptr<AVStream> m_pStream;
};
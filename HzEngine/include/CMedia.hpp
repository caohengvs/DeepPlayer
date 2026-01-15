#pragma once
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavutil/avutil.h>
#pragma warning(pop)
}

#include <string>
#include <thread>
class CVideo;
class CAudio;
class AVFormatContext;
class AVCodecContext;
class AVPacket;

class CMedia
{
public:
    CMedia();
    ~CMedia();

public:
    int Open(std::string_view path);

private:
    int open_codec_context(int* stream_idx, AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, AVMediaType type);
    void decode(); 

private:
    CVideo* m_pVideo{nullptr};
    CAudio* m_pAudio{nullptr};
    AVFormatContext* m_pFmtCtx{nullptr};
    AVPacket* m_pPkt{nullptr};
    std::thread m_thdDecode; 
};
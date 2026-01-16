#include "CMedia.hpp"
#include "CAudio.hpp"
#include "CVideo.hpp"
#include "HzLogger.hpp"
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#pragma warning(pop)
}

CMedia::CMedia()
{
    m_pVideo = new CVideo;
    m_pAudio = new CAudio;
    m_pPkt = av_packet_alloc();
}

CMedia::~CMedia()
{
    m_bRun = false;
    if (m_thdDecode.joinable())
        m_thdDecode.join();

    if (m_pVideo)
        delete m_pVideo;

    if (m_pAudio)
        delete m_pAudio;

    avformat_close_input(&m_pFmtCtx);
    av_packet_free(&m_pPkt);
}

int CMedia::Open(std::string_view path)
{
    int ret = 0;
    if ((ret = avformat_open_input(&m_pFmtCtx, path.data(), nullptr, nullptr)) < 0)
    {
        LOG_ERROR << "avformat_open_input failed:" << ret;
        return ret;
    }
    if (avformat_find_stream_info(m_pFmtCtx, NULL) < 0)
    {
        LOG_ERROR << "Could not find stream information:" << ret;
        return ret;
    }

    // find video stream
    if ((ret = open_codec_context(&m_pVideo->m_nStreamIndex, &(m_pVideo->m_pCodecCtx), m_pFmtCtx,
                                  AVMEDIA_TYPE_VIDEO)) >= 0)
    {
        m_pVideo->m_pStream = m_pFmtCtx->streams[m_pVideo->m_nStreamIndex];

        AVRational fps_rational = m_pVideo->m_pStream->avg_frame_rate;

        double fps = 0;
        if (fps_rational.den > 0)
        {
            fps = av_q2d(fps_rational);
        }
        LOG_INFO << "fps:" << fps;
    }

    // find audio stream
    if (open_codec_context(&m_pAudio->m_nStreamIndex, &m_pAudio->m_pCodecCtx, m_pFmtCtx, AVMEDIA_TYPE_AUDIO) >= 0)
    {
        m_pAudio->m_pStream = m_pFmtCtx->streams[m_pAudio->m_nStreamIndex];
    }

    if (!m_pVideo->m_pCodecCtx && !m_pAudio->m_pCodecCtx)
    {
        LOG_ERROR << "Could not find audio or video stream in the input, aborting";
        m_pVideo->FreeCtx();
        m_pAudio->FreeCtx();
        return -1;
    }

    m_bRun = true;
    m_thdDecode = std::thread(&CMedia::decode, this);

    return ret;
}

int CMedia::open_codec_context(int* stream_idx, AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, AVMediaType type)
{
    int ret, stream_index;
    AVStream* st;
    const AVCodec* dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec)
        {
            return AVERROR(EINVAL);
        }

        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx)
        {
            return AVERROR(ENOMEM);
        }

        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
        {
            return ret;
        }

        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
        {
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

void CMedia::decode()
{
    while (m_bRun)
    {
        while (m_bRun)
        {
            if (m_bSeeking)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }

            std::lock_guard<std::mutex> lock(m_mtxSeek);
            if (av_read_frame(m_pFmtCtx, m_pPkt) >= 0)
            {
                if (m_pPkt->stream_index == m_pVideo->m_nStreamIndex)
                {
                    m_pVideo->Decode(m_pPkt, m_nTargetPts);
                }
                av_packet_unref(m_pPkt);
            }
        }
    }
}

void CMedia::Seek(double seconds)
{
    m_bSeeking = true;
    {
        std::lock_guard<std::mutex> lock(m_mtxSeek);

        m_nTargetPts = seconds / av_q2d(m_pFmtCtx->streams[m_pVideo->m_nStreamIndex]->time_base);

        avformat_seek_file(m_pFmtCtx, m_pVideo->m_nStreamIndex, INT64_MIN, m_nTargetPts, m_nTargetPts,
                           AVSEEK_FLAG_BACKWARD);

        m_pVideo->ClearCache();
        avcodec_flush_buffers(m_pVideo->m_pCodecCtx);
    }
    m_bSeeking = false;
}

double CMedia::GetCurrentTime()
{
    return m_pVideo->GetCurrentPts() * av_q2d(m_pFmtCtx->streams[m_pVideo->m_nStreamIndex]->time_base);
}

AVFrame* CMedia::GetFrame()
{
    return m_pVideo->GetFrame();
}

#include "CVideo.hpp"
#include "HzLogger.hpp"
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#pragma warning(pop)
}
#include <opencv4/opencv2/opencv.hpp>
#include <sstream>
CVideo::CVideo()
{
    m_pFrame = av_frame_alloc();
    m_pFrameRGB = av_frame_alloc();
}

CVideo::~CVideo()
{
    while (!m_queFrame.empty())
    {
        AVFrame* frame = m_queFrame.front();
        m_queFrame.pop();

        if (frame != nullptr)
        {
            av_frame_free(&frame);
        }
    }
    m_bRun = false;

    m_cvFrame.notify_all();
    av_frame_free(&m_pFrame);
    av_frame_free(&m_pFrameRGB);
}

void CVideo::FreeCtx()
{
    avcodec_free_context(&m_pCodecCtx);
}

void CVideo::Flush()
{
    int ret = 0;
    ret = avcodec_send_packet(m_pCodecCtx, nullptr);
    while (ret >= 0)
    {
        ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0)
        {
            break;
        }
        SwsContext* sws_ctx =
            sws_getContext(m_pFrame->width, m_pFrame->height, (AVPixelFormat)m_pFrame->format, m_pFrame->width,
                           m_pFrame->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
        m_pFrameRGB->format = AV_PIX_FMT_RGB24;
        m_pFrameRGB->width = m_pFrame->width;
        m_pFrameRGB->height = m_pFrame->height;
        av_frame_get_buffer(m_pFrameRGB, 0);
        sws_scale(sws_ctx, m_pFrame->data, m_pFrame->linesize, 0, m_pFrame->height, m_pFrameRGB->data,
                  m_pFrameRGB->linesize);

        av_frame_unref(m_pFrameRGB);
        av_frame_unref(m_pFrame);
    }
}

void CVideo::Decode(AVPacket* packet, int64_t targetPts)
{
    int ret = 0;

    auto* pkt = packet;
    ret = avcodec_send_packet(m_pCodecCtx, pkt);

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0)
        {
            break;
        }
        if (m_pFrame->pts < targetPts)
        {
            continue;
        }

        SwsContext* sws_ctx =
            sws_getContext(m_pFrame->width, m_pFrame->height, (AVPixelFormat)m_pFrame->format, m_pFrame->width,
                           m_pFrame->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

        m_pFrameRGB->format = AV_PIX_FMT_RGB24;
        m_pFrameRGB->width = m_pFrame->width;
        m_pFrameRGB->height = m_pFrame->height;
        av_frame_get_buffer(m_pFrameRGB, 0);
        sws_scale(sws_ctx, m_pFrame->data, m_pFrame->linesize, 0, m_pFrame->height, m_pFrameRGB->data,
                  m_pFrameRGB->linesize);

        AVFrame* pFrameRGB = av_frame_alloc();
        pFrameRGB->format = m_pFrameRGB->format;
        pFrameRGB->width = m_pFrameRGB->width;
        pFrameRGB->height = m_pFrameRGB->height;

        av_frame_get_buffer(pFrameRGB, 0);

        av_frame_copy(pFrameRGB, m_pFrameRGB);

        {
            std::lock_guard<std::mutex> lock(m_mtxPkt);
            m_queFrame.push(pFrameRGB);
        }
        m_cvFrame.notify_one();

        int64_t pts = (m_pFrame->pts != AV_NOPTS_VALUE) ? m_pFrame->pts : m_pFrame->pkt_dts;

        if (pts < 0)
            pts = 0;

        m_nCurrentPts = pts;

        av_frame_unref(m_pFrameRGB);

        av_frame_unref(m_pFrame);
    }
}

AVFrame* CVideo::GetFrame()
{
    std::unique_lock<std::mutex> lock(m_mtxPkt);
    m_cvFrame.wait(lock, [this]() { return !m_queFrame.empty() || !m_bRun; });
    if (!m_bRun)
        return nullptr;

    auto* frame = m_queFrame.front();
    m_queFrame.pop();

    return frame;
}

void CVideo::ClearCache()
{
    std::lock_guard<std::mutex> lock(m_mtxPkt);
    while (!m_queFrame.empty())
    {
        AVFrame* frame = m_queFrame.front();
        m_queFrame.pop();

        if (frame != nullptr)
        {
            av_frame_free(&frame);
        }
    }
}

double CVideo::GetCurrentPts()
{
    return m_nCurrentPts;
}

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
    for (auto& it : m_arrPkt)
    {
        auto* pkt = av_packet_alloc();
        it = pkt;
    }
    m_pFrame = av_frame_alloc();
    m_pFrameRGB = av_frame_alloc();
}

CVideo::~CVideo()
{
    for (auto it : m_arrPkt)
    {
        av_packet_free(&it);
    }

    av_frame_free(&m_pFrame);
    av_frame_free(&m_pFrameRGB);
}

void CVideo::Clear()
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
        SwsContext* sws_ctx = sws_getContext(m_pFrame->width, m_pFrame->height, (AVPixelFormat)m_pFrame->format,  // 源
                                             m_pFrame->width, m_pFrame->height, AV_PIX_FMT_RGB24,  // 目标
                                             SWS_BILINEAR, NULL, NULL, NULL);
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

void CVideo::Decode(AVPacket* packet)
{
    int ret = 0;

    auto* pkt = packet;
    ret = avcodec_send_packet(m_pCodecCtx, pkt);

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0)
        {
            // 数据不足
            break;
        }

        SwsContext* sws_ctx = sws_getContext(m_pFrame->width, m_pFrame->height, (AVPixelFormat)m_pFrame->format,  // 源
                                             m_pFrame->width, m_pFrame->height, AV_PIX_FMT_RGB24,  // 目标
                                             SWS_BILINEAR, NULL, NULL, NULL);

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

        // 分配新内存
        av_frame_get_buffer(pFrameRGB, 0);

        // 物理拷贝像素数据
        av_frame_copy(pFrameRGB, m_pFrameRGB);

        // 现在 pFrameRGB 是一个完全独立的副本
        {
            std::lock_guard<std::mutex> lock(m_mtxPkt);
            m_queFrame.push(pFrameRGB);
        }

        av_frame_unref(m_pFrameRGB);

        av_frame_unref(m_pFrame);
    }
    av_packet_unref(pkt);
}

AVFrame* CVideo::GetFrame()
{
    if (m_queFrame.empty())
    {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(m_mtxPkt);
    auto* frame = m_queFrame.front();
    m_queFrame.pop();
    return frame;
}

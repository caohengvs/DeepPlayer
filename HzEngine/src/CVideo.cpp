#include "CVideo.hpp"
#include "HzLogger.hpp"
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavutil/pixdesc.h>
#pragma warning(pop)
}
CVideo::CVideo()
{
    for (auto& it : m_arrPkt)
    {
        auto* pkt = av_packet_alloc();
        it = pkt;
    }
    m_pFrame = av_frame_alloc();
    m_bRun = true;
    m_thdDecode = std::thread(&CVideo::decode, this);
}

CVideo::~CVideo()
{
    m_bRun = false;
    m_cvPkt.notify_all();

    for (auto it : m_arrPkt)
    {
        av_packet_free(&it);
    }

    av_frame_free(&m_pFrame);
}

void CVideo::Append(AVPacket* packet)
{
    if (m_nWriteIndex >= m_arrPkt.size())
        m_nWriteIndex = 0;

    if (av_packet_ref(m_arrPkt[m_nWriteIndex], packet) < 0)
    {
        return;
    }
    m_nWriteIndex++;

    m_cvPkt.notify_one();
}

void CVideo::Clear()
{
    avcodec_free_context(&m_pCodecCtx);
}

void CVideo::decode()
{
    int ret = 0;
    while (m_bRun)
    {
        std::unique_lock<std::mutex> lk(m_mtxPkt);
        m_cvPkt.wait(lk, [this] { return m_nReadIndex < m_nWriteIndex || !m_bRun; });
        if (m_nReadIndex > m_arrPkt.size())
        {
            m_nReadIndex = 0;
        }

        if (!m_bRun)
        {
            break;
        }

        auto* pkt = m_arrPkt[m_nReadIndex++];
        ret = avcodec_send_packet(m_pCodecCtx, pkt);

        while (ret >= 0)
        {
            ret = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF || ret < 0)
            {
                // 数据不足
                break;
            }
            const char* format_name = av_get_pix_fmt_name((AVPixelFormat)m_pFrame->format);

            LOG_INFO << "image formate" << format_name;
            // LOG_DEBUG << "图像格式：" << format_name << "图像大小：" << m_pFrame->width << "x" << m_pFrame->height;
            av_frame_unref(m_pFrame);
        }
        av_packet_unref(pkt);
    }
}

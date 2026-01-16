#pragma once
#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>

class AVCodecContext;
class AVStream;
class AVPacket;
class AVFrame;
class CVideo
{
    friend class CMedia;

public:
    CVideo();
    ~CVideo();

public:
    void Clear();
    void Flush();
    void Decode(AVPacket* pPacket);

    AVFrame* GetFrame();

private:
    AVCodecContext* m_pCodecCtx{nullptr};
    AVStream* m_pStream{nullptr};
    int m_nStreamIndex{-1};

    std::array<AVPacket*, 1024*1024> m_arrPkt;
    uint8_t m_nWriteIndex{0};
    uint8_t m_nReadIndex{0};
    std::thread m_thdDecode;  // 解码线程
    std::atomic<bool> m_bRun{false};
    std::mutex m_mtxPkt;
    std::condition_variable m_cvPkt;

    AVFrame* m_pFrame{nullptr};
    AVFrame* m_pFrameRGB{nullptr};
    std::queue<AVFrame*> m_queFrame;
    
};
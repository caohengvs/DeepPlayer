#pragma once
#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

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
    void FreeCtx();
    void Flush();
    void Decode(AVPacket* pPacket, int64_t targetPts);

    AVFrame* GetFrame();
    void ClearCache();
    double GetCurrentPts();

private:
    AVCodecContext* m_pCodecCtx{nullptr};
    AVStream* m_pStream{nullptr};
    int m_nStreamIndex{-1};

    std::thread m_thdDecode;  // 解码线程

    std::mutex m_mtxPkt;
    std::condition_variable m_cvFrame;

    AVFrame* m_pFrame{nullptr};
    AVFrame* m_pFrameRGB{nullptr};
    std::queue<AVFrame*> m_queFrame;
    std::atomic<bool> m_bRun{true};
    std::atomic<int64_t> m_nCurrentPts{0};
};
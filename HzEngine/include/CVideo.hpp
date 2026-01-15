#pragma once
#include <array>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
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
    void Append(AVPacket* pPacket);
    void Clear();

    private:
    void decode();

private:
    AVCodecContext* m_pCodecCtx{nullptr};
    AVStream* m_pStream{nullptr};
    int m_nStreamIndex{-1};

    std::array<AVPacket*, 30> m_arrPkt;
    uint8_t m_nWriteIndex{0};
    uint8_t m_nReadIndex{0};
    std::thread m_thdDecode; // 解码线程
    std::atomic<bool> m_bRun{false};
    std::mutex m_mtxPkt;
    std::condition_variable m_cvPkt;

    AVFrame* m_pFrame{nullptr};
};
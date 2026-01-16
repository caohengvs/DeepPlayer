#pragma once
#include <queue>

class AVCodecContext;
class AVStream;
class AVPacket;
class AVFrame;
class CAudio
{
    friend class CMedia;

public:
    CAudio();
    ~CAudio();

public:
    void FreeCtx();

private:
    AVCodecContext* m_pCodecCtx{nullptr};
    AVStream* m_pStream{nullptr};
    int m_nStreamIndex{-1};

    std::queue<AVPacket*> m_qPackets;
    std::queue<AVFrame*> m_qFrames;
};
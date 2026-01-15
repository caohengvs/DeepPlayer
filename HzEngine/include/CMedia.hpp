#pragma once
#include <string>
class CVideo;
class AVFormatContext;

class CMedia
{
public:
    CMedia();
    ~CMedia();

public:
    int Open(std::string_view path);

private:
    CVideo* m_pVideo{nullptr};
    AVFormatContext *m_pFmtCtx{nullptr};
};
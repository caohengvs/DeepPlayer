#include "CMedia.hpp"
#include "HzLogger.hpp"
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavformat/avformat.h>
#pragma warning(pop)
}

CMedia::CMedia()
{
}

CMedia::~CMedia()
{
}

int CMedia::Open(std::string_view path)
{
    // bool bRet = false;
    int ret = 0;
    if ((ret = avformat_open_input(&m_pFmtCtx, path.data(), nullptr, nullptr)) < 0)
    {
        LOG_ERROR << "avformat_open_input failed:" << ret;
        return ret;
    }
    return ret;
}
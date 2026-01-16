#include "CAudio.hpp"
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavcodec/avcodec.h>

#pragma warning(pop)
}

CAudio::CAudio()
{
}

CAudio::~CAudio()
{
}

void CAudio::FreeCtx()
{
    avcodec_free_context(&m_pCodecCtx);
}

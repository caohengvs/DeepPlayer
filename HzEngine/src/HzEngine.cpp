#include "HzEngine.hpp"
#include "CMedia.hpp"
extern "C"
{
#pragma warning(push)
#pragma warning(disable : 4819)
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#pragma warning(pop)
}

HzEngine::HzEngine()
{
    m_pMedia = new CMedia;
}

HzEngine::~HzEngine()
{
    if(m_pMedia)
    delete m_pMedia;
}

bool HzEngine::Init(std::string_view filename)
{
    if (m_pMedia == nullptr)
        return false;

    m_pMedia->Open(filename);
    return true;
}

void HzEngine::Pause()
{
}

void HzEngine::Stop()
{
     delete m_pMedia;
     m_pMedia = nullptr;
}

void HzEngine::SeekPos(float position)
{
}

void HzEngine::SetVolume(float volume)
{
}

void HzEngine::SetSpeed(float speed)
{
}

void HzEngine::SetLoop(bool loop)
{
}

HzFrame HzEngine::GetFrame()
{
    auto* pFrame = m_pMedia->GetFrame();
    if(pFrame == nullptr)
    {
        return HzFrame();
    }
    m_Frame.clear();
    m_Frame.width = pFrame->width;
    m_Frame.height = pFrame->height;
    m_Frame.linesize = pFrame->linesize[0];
    int size = av_image_get_buffer_size((AVPixelFormat)pFrame->format, pFrame->width, pFrame->height, 1);

    m_Frame.data = (uint8_t*)malloc(size);
    av_image_copy_to_buffer(m_Frame.data, size, pFrame->data, pFrame->linesize, (AVPixelFormat)pFrame->format,
                            pFrame->width, pFrame->height, 1);
   
    av_frame_free(&pFrame);
    return m_Frame;
}

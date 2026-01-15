#include "HzEngine.hpp"
#include "CMedia.hpp"

HzEngine::HzEngine()
{
    m_pMedia = new CMedia;
}

HzEngine::~HzEngine()
{
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

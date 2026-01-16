#pragma once
/**
 * @file: HzEngine.hpp
 * @brief:
 * @author: Curtis Cao
 * @date: 2026-01-15 11:15:25
 * @version: 1.0
 * @email: caohengvs888@gmail.com
 */
#include <memory>
#include <string_view>
#include "ExportDef.h"

class CMedia;
struct HzFrame
{
    uint8_t* data{nullptr};
    int width{0};
    int height{0};
    int linesize{0};

    void clear()
    {
        if (data)
        {
            // delete[] data;
            free(data);
            data = nullptr;
        }
        width = 0;
        height = 0;
        linesize = 0;
    }
};
using HzFramePtr = std::shared_ptr<HzFrame>;
class HZ_LIB_API HzEngine
{
public:
public:
    HzEngine();
    ~HzEngine();

public:
    bool Init(std::string_view filename);
    void Pause();
    void Stop();
    void SeekPos(float position);
    void SetVolume(float volume);
    void SetSpeed(float speed);
    void SetLoop(bool loop);
    HzFrame GetFrame();

private:
    CMedia* m_pMedia{nullptr};
    HzFrame m_Frame;
};
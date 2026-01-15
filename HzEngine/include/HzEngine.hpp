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

class  HZ_LIB_API HzEngine
{
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

private:
    CMedia* m_pMedia{nullptr};
};
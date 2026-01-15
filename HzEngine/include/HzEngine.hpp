#pragma once
/**
 * @file: HzEngine.hpp
 * @brief:
 * @author: Curtis Cao
 * @date: 2026-01-15 11:15:25
 * @version: 1.0
 * @email: caohengvs888@gmail.com
 */
#include <string_view>

class HzEngine
{
public:
    HzEngine();
    ~HzEngine();

public: 
    bool Play(const std::string_view& filename);
    void Pause();
    void Stop();
    void SeekPos(float position);
    void SetVolume(float volume);
    void SetSpeed(float speed);
    void SetLoop(bool loop);

};
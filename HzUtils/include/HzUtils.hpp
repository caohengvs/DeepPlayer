#pragma once

#include "ExportDef.h"
#include <string>
class HZ_LIB_API HzUtils
{
public:
    HzUtils() = default;
    ~HzUtils() = default;

    static std::string GetTime();
};

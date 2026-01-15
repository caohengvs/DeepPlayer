#pragma once
#ifdef _WIN32
#ifdef HZ_LIB_EXPORT
#define HZ_LIB_API __declspec(dllexport)
#else
#define HZ_LIB_API __declspec(dllimport)
#endif
#else
#define HZ_LIB_EXPORT
#endif
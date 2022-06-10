#pragma once
#include <cstdint>
#include <windows.h>
#include <cstring>

#define EXPORT __declspec(dllexport)


EXPORT int Init();
EXPORT uint16_t * ChangeText(uint16_t * src);
size_t my_strlen(uint16_t * s);
size_t my_strlen16(uint16_t * s);

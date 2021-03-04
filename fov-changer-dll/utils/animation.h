#pragma once

#include "../pch.h"

#define PI (float)(2 * acos(0.0))

namespace animation
{
    float Smooth(float x, float diff, float strength);
    float SmoothInverse(float x, float width, float from, float to);
    void LerpFov(float* fov_p, float from, float to, float strength);
}
#include "animation.h"

/// <summary>
/// Lerp with a sin curve
/// </summary>
/// <param name="x"></param>
/// <param name="diff"> = height</param>
/// <param name="strength"> = frequency</param>
/// <returns></returns>
float animation::Smooth(float x, float diff, float strength)
{
    return ( sin((x * (PI / 2) * (2 / strength)) + ((3 * PI) / 2)) * diff + diff ) / 2.0f;
}

/// <summary>
/// Lerp with a asin curve
/// </summary>
/// <param name="x"></param>
/// <param name="height"> = height</param>
/// <param name="width"> = frequency</param>
/// <param name="base_height"> = frequency</param>
/// <returns></returns>
float animation::SmoothInverse(float x, float width, float from, float to)
{
    return asin(x * (1 / width)) * (2 / PI) * (to - from) + from;
}

//float animation::SmoothEx(float x, float width, float from, float to)
//{
//    return -log(-x * (1/width) + 1) * smooth + to
//}

/// <summary>
/// Lerps the fov of given pointer
/// </summary>
/// <param name="fov_p">the pointer to a fov value</param>
/// <param name="from">initial fov value</param>
/// <param name="to">fov value to get there</param>
/// <param name="strength">gets multiplied with the steps (differnce of to and from) -> over 1: more time, under 1: less time</param>
void animation::LerpFov(float* fov_p, float from, float to, float strength)
{
    float diff = to - from;

    // std::cout << "Diff: " << diff << " N: " << n << std::endl;

    for (float i = 0; i <= abs(diff * strength); i++)
    {
        //std::cout << i << " => " << Smooth(i, diff / 2.0f, abs(diff * strength)) << std::endl;
        *fov_p = from + Smooth(i, diff, abs(diff * strength));

        Sleep(10);
    }
}
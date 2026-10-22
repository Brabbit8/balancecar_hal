#include "qmath.h"
#include <stdint.h>

#define QMATH_ATAN_TABLE_INTERVALS 64U
#define QMATH_ATAN_TABLE_SIZE     (QMATH_ATAN_TABLE_INTERVALS + 1U)

/*
 * atan(i / 64) 的角度查找表，覆盖 ratio=[0, 1]。
 * 查表点之间使用线性插值，避免每次调用软件 atan2。
 */
static const float atan_table_deg[QMATH_ATAN_TABLE_SIZE] =
{
    0.000000f, 0.895174f, 1.789911f, 2.683775f, 3.576334f,
    4.467159f, 5.355825f, 6.241914f, 7.125016f, 8.004729f,
    8.880659f, 9.752425f, 10.619655f, 11.481991f, 12.339087f,
    13.190611f, 14.036243f, 14.875682f, 15.708638f, 16.534838f,
    17.354025f, 18.165957f, 18.970408f, 19.767169f, 20.556045f,
    21.336859f, 22.109448f, 22.873665f, 23.629378f, 24.376469f,
    25.114835f, 25.844388f, 26.565051f, 27.276763f, 27.979474f,
    28.673146f, 29.357754f, 30.033280f, 30.699723f, 31.357085f,
    32.005383f, 32.644640f, 33.274888f, 33.896167f, 34.508523f,
    35.112011f, 35.706691f, 36.292630f, 36.869898f, 37.438572f,
    37.998732f, 38.550465f, 39.093859f, 39.629005f, 40.156000f,
    40.674940f, 41.185925f, 41.689058f, 42.184443f, 42.672185f,
    43.152390f, 43.625165f, 44.090620f, 44.548861f, 45.000000f
};

static float atan_lookup_deg(float ratio)
{
    float position = ratio * (float)QMATH_ATAN_TABLE_INTERVALS;
    uint16_t index = (uint16_t)position;
    float fraction;

    if (index >= QMATH_ATAN_TABLE_INTERVALS)
    {
        return atan_table_deg[QMATH_ATAN_TABLE_INTERVALS];
    }

    fraction = position - (float)index;
    return atan_table_deg[index]
         + (atan_table_deg[index + 1U] - atan_table_deg[index]) * fraction;
}

float QMath_Atan2Deg(float y, float x)
{
    float abs_x;
    float abs_y;
    float angle;

    if ((x == 0.0f) && (y == 0.0f))
    {
        return 0.0f;
    }

    abs_x = (x < 0.0f) ? -x : x;
    abs_y = (y < 0.0f) ? -y : y;

    if (abs_x >= abs_y)
    {
        angle = atan_lookup_deg(abs_y / abs_x);
    }
    else
    {
        angle = 90.0f - atan_lookup_deg(abs_x / abs_y);
    }

    if (x >= 0.0f)
    {
        return (y >= 0.0f) ? angle : -angle;
    }

    return (y >= 0.0f) ? (180.0f - angle) : (angle - 180.0f);
}


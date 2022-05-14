#ifndef AP_MATH_H

#include <math.h>

static inline float ap_absf(float f)
{
        return (f > 0) ? f : -f;
}

static inline int ap_absi(int i)
{
        return (i > 0) ? i : -i;
}

static inline float ap_powf(float base, int n)
{
        float result = 1.0f;
        for (int i = 0; i < n; ++i) {
                result *= base;
        }
        return result;
}

static inline float ap_distance(float pos1[3], float pos2[3])
{
        return (float) sqrt(
                ap_powf(pos1[0] - pos2[0], 2)
                + ap_powf(pos1[1] - pos2[1], 2)
                + ap_powf(pos1[2] - pos2[2], 2)
        );
}

#endif  // AP_MATH_H
#ifndef AP_MATH_H

#include <string.h>
#include <math.h>
#include <stdbool.h>

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

static inline int ap_v2_set(float v[2], float x, float y)
{
        v[0] = x;
        v[1] = y;
        return 0;
}

static inline int ap_v3_set(float v[3], float x, float y, float z)
{
        v[0] = x;
        v[1] = y;
        v[2] = z;
        return 0;
}

static inline int ap_v4_set(float v[4], float x, float y, float z, float w)
{
        v[0] = x;
        v[1] = y;
        v[2] = z;
        v[3] = w;
        return 0;
}

static inline bool ap_is_num(char c)
{
        return (c >= '0' && c <= '9');
}

/**
 * @brief string to integer
 */
static inline int ap_c2i(const char *num)
{
        if (!num) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        int len = strlen(num);
        int ret = 0;
        bool negative = false;
        for (int i = 0; i < len; ++i) {
                if (ap_is_num(num[i])) {
                        ret = ret * 10 + num[i] - '0';
                } else if (i == 0 && num[i] == '-') {
                        negative = true;
                } else if (num[i] == '.') {
                        break;
                }
        }

        return negative ? -ret : ret;
}

/**
 * @brief string to double
 */
static inline double ap_c2d(const char *num)
{
        if (!num) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        int len = strlen(num);
        double ret = 0.0;
        bool negative = false;
        bool is_decimal = false;
        int decimal_num = 0;
        for (int i = 0; i < len; ++i) {
                if (ap_is_num(num[i])) {
                        if (is_decimal) {
                                decimal_num++;
                                double tmp = (double) (num[i] - '0');
                                for (int j = 0; j < decimal_num; ++j) {
                                        tmp *= 0.1;
                                }
                                ret = ret + tmp;
                        } else {
                                ret = ret * 10 + num[i] - '0';
                        }
                } else if (i == 0 && num[i] == '-') {
                        negative = true;
                } else if (num[i] == '.') {
                        is_decimal = true;
                }
        }

        return negative ? -ret : ret;
}

#endif  // AP_MATH_H
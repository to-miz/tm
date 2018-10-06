/*
tm_bezier.h v1.0.0.1 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2016

no warranty; use at your own risk

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
            #define TM_BEZIER_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

NOTES
    Library for evaluation of bezier curves under usage of floating point additions.
    Based on this article: http://www.drdobbs.com/forward-difference-calculation-of-bezier/184403417

    You need to call computeBezierForwardDifferencer once to turn your bezier curve points into
    forward difference data. From then on use the data to evaluate your bezier curve with
    evaluateBezierForwardDifferencer.

    evaluateBezierForwardDifferencerFromX returns the y value of a given bezier curve from an x
    value. This only works for special bezier curves, where no loops are present. Use this function
    if you want to use bezier curves for easing. Your bezier curve points should be like this:
            x0 = 0, y0 = 0
            x1 in interval [0, 1], y1 in interval (-inf, inf)
            x2 in interval [0, 1], y2 in interval (-inf, inf)
            x3 = 1, y3 = 1
    For most easing curves a steps count of 10 should be totally fine.

    For better usability #define TMB_VECTOR with your own vector2 struct before #including this
    header. It has to have x and y members, or change the functions to use your vector2 struct
    natively.

HISTORY
    v1.0.0.1  06.10.18 changed formatting
    v1.0b     25.08.18 added repository link, changed some wording in the description
    v1.0a     01.07.16 improved C99 conformity
    v1.0      24.06.16 initial commit
*/

/* clang-format off */

#ifdef TM_BEZIER_IMPLEMENTATION
    /* define these to avoid crt */
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif
#endif /* defined(TM_BEZIER_IMPLEMENTATION) */

#ifndef _TM_BEZIER_H_INCLUDED_
#define _TM_BEZIER_H_INCLUDED_

#ifdef TMB_STATIC
    #define TMB_DEF static
#else
    #define TMB_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TMB_VECTOR
    typedef struct {
        float x, y;
    } tmb_vector;
    #define TMB_VECTOR tmb_vector
#endif

/* clang-format on */

typedef struct {
    float h;
    float x, y;
    float dx, dy;
    float ddx, ddy;
    float dddx, dddy;
} BezierForwardDifferencerData;

TMB_DEF BezierForwardDifferencerData computeBezierForwardDifferencer(int steps, float x0, float y0, float x1, float y1,
                                                                     float x2, float y2, float x3, float y3);
/* Get point on bezier from t. */
TMB_DEF TMB_VECTOR evaluateBezierForwardDifferencer(BezierForwardDifferencerData* data, float t);
/* Get y from x, only works for special kinds of bezier curves, like those used for easing, where
   x0 = 0, y0 = 0, x3 = 1, y3 = 1 and x1, x2 are bounded in [0, 1]. */
TMB_DEF float evaluateBezierForwardDifferencerFromX(BezierForwardDifferencerData* data, float x);

#ifdef __cplusplus
}
#endif

#endif /* defined(_TM_BEZIER_H_INCLUDED_) */

#ifdef TM_BEZIER_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

TMB_DEF BezierForwardDifferencerData computeBezierForwardDifferencer(int steps, float x0, float y0, float x1, float y1,
                                                                     float x2, float y2, float x3, float y3) {
    BezierForwardDifferencerData result = {0};
    float ax, ay, bx, by, cx, cy;
    float h, h2, h3;
    float x0_3, y0_3, x1_3, y1_3, x2_3, y2_3;
    float h3_6;
    float h2_2;

    x0_3 = x0 * 3;
    y0_3 = y0 * 3;
    x1_3 = x1 * 3;
    y1_3 = y1 * 3;
    x2_3 = x2 * 3;
    y2_3 = y2 * 3;

    ax = -x0 + x1_3 + -x2_3 + x3;
    ay = -y0 + y1_3 + -y2_3 + y3;

    bx = x0_3 + -6 * x1 + x2_3;
    by = y0_3 + -6 * y1 + y2_3;

    cx = -x0_3 + x1_3;
    cy = -y0_3 + y1_3;

    h = 1.0f / (float)steps;
    h2 = h * h;
    h3 = h2 * h;
    h3_6 = h3 * 6;
    h2_2 = h2 * 2;

    result.h = h;

    result.x = x0;
    result.y = y0;

    result.dx = ax * h3 + bx * h2 + cx * h;
    result.dy = ay * h3 + by * h2 + cy * h;

    result.dddx = ax * h3_6;
    result.dddy = ay * h3_6;

    result.ddx = result.dddx + bx * h2_2;
    result.ddy = result.dddy + by * h2_2;

    return result;
}
TMB_DEF TMB_VECTOR evaluateBezierForwardDifferencer(BezierForwardDifferencerData* data, float t) {
    float current, h;
    float dx, dy;
    float ddx, ddy;
    float dddx, dddy;
    float lastx, lasty;
    float diff;
    TMB_VECTOR result;

    TM_ASSERT(data->h > 0);

    current = 0;
    h = data->h;
    dx = data->dx;
    dy = data->dy;
    result.x = data->x + dx;
    result.y = data->y + dy;
    ddx = data->ddx;
    ddy = data->ddy;
    dddx = data->dddx;
    dddy = data->dddy;

    for (;;) {
        if (current >= t) {
            lastx = result.x - dx;
            lasty = result.y - dy;
            diff = (current - t) / h;
            result.x = lastx + (result.x - lastx) * diff;
            result.y = lasty + (result.y - lasty) * diff;
            break;
        }

        if (current >= 1.0f) {
            break;
        }

        current += h;
        dx += ddx;
        dy += ddy;
        ddx += dddx;
        ddy += dddy;
        result.x += dx;
        result.y += dy;
    }

    return result;
}
TMB_DEF float evaluateBezierForwardDifferencerFromX(BezierForwardDifferencerData* data, float x) {
    float current, h;
    float dx, dy;
    float ddx, ddy;
    float dddx, dddy;
    float lastx, lasty;
    float diff;
    float px;
    float py;

    TM_ASSERT(data->h > 0);

    current = 0;
    h = data->h;
    dx = data->dx;
    dy = data->dy;
    px = data->x + dx;
    py = data->y + dy;
    ddx = data->ddx;
    ddy = data->ddy;
    dddx = data->dddx;
    dddy = data->dddy;

    for (;;) {
        if (px >= x) {
            lastx = px - dx;
            lasty = py - dy;
            diff = (x - lastx) / (px - lastx);
            return lasty + (py - lasty) * diff;
        }

        if (current >= 1.0f) {
            break;
        }

        current += h;
        dx += ddx;
        dy += ddy;
        ddx += dddx;
        ddy += dddy;
        px += dx;
        py += dy;
    }

    return py - dy;
}

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_BEZIER_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2016 Tolga Mizrak

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---------------------------------------------------------------------------

Public Domain (www.unlicense.org):
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

---------------------------------------------------------------------------
*/

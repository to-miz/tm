/*
tm_polygon.h v1.0.0.1 - public domain - https://github.com/to-miz/tm
author: Tolga Mizrak 2016

no warranty; use at your own risk

LICENSE
    see license notes at end of file

USAGE
    This file works as both the header and implementation.
    To implement the interfaces in this header,
        #define TM_POLYGON_IMPLEMENTATION
    in ONE C or C++ source file before #including this header.

NOTES
    This library implements algorithms to triangulate polygons and clip polygons against one
    another.
    Note that the algorithms do not allocate any memory on their own. When calling the functions,
    you have to pass in the memory the algorithms operate on, so be sure to be generous with memory,
    or the algorithms can miss intersections or generate too few polygons in case of the clipping
    functions.

    The triangulation is implemented using ear clipping and does not handle self intersecting
    polygons or polygons with holes. It takes a list of vertices and emits triangles in the form of
    indices. These indices can be used to populate a gpu index buffer directly.

    Clipping polygons is implemented using the Greiner–Hormann clipping algorithm.
    Clipping is done in 3 phases, with one additional initialization step.
    Call these functions in this order:
    First you need to convert your polygon into a format that can be used by the algorithm.
    This is done using the function clipPolyTransformData, it returns the polygon in a format that
    the algorithm can work with. The function expects you to pass in the memory for the intermediate
    polygon format. Be sure to pass in enough memory (in form of ClipVertex entries) so that the
    algorithm can store intersections in the same structures. Transform both your polygons like
    this.

    The first phase is finding intersections. Call into clipPolyFindIntersections with both
    polygons. The intersections will be stored in the polygons themselves, this is why you need to
    pass in enough memory into clipPolyTransformData.

    The second phase is clipPolyMarkEntryExitPoints. This marks whether the intersections are entry
    vertices into the other polygon or exit vertices. At the same time this function expects two
    ClipFollowDirection arguments, one for each polygon. What follow direction you want depends on
    the operation you want to do. Refer to the following table:
        Polygon a | Polygon b | Result
        Forward   | Forward   | a AND b (difference of a and b)
        Backward  | Forward   | a \ b
        Forward   | Backward  | b \ a
        Backward  | Backward  | a OR b (union of a and b)

    The third phase is clipPolyEmitClippedPolygons. Now we are ready to emit polygons. The emitted
    polygons depend on the ClipFollowDirection arguments to clipPolyMarkEntryExitPoints. Note that
    again you have to supply the memory for the polygons. All polygons share the same pool of
    vertices (arguments 5 and 6).

ISSUES
    Clipping:
        - in case one polygon is completely inside another, only in case of clipping (logical
          AND of both polygons) the right result is being returned.

HISTORY
    v1.0.0.1  06.10.18 changed formatting
                       added TMPO_VERSION
                       fixed some C compilation warnings
    v1.0d     25.08.18 added repository link
    v1.0c     07.10.16 changed tmp prefix to tmpo prefix, since tm_print.h already uses tmp
                       removed using unsigned arithmetic when tm_size_t is signed
    v1.0b     02.07.16 changed #include <memory.h> into string.h
    v1.0a     01.07.16 improved C99 conformity
    v1.0      26.06.16 initial commit

LICENSE
    This software is dual-licensed to the public domain and under the following
    license: you are granted a perpetual, irrevocable license to copy, modify,
    publish, and distribute this file as you see fit.
*/

/* clang-format off */

#ifdef TM_POLYGON_IMPLEMENTATION
    /* Define these to avoid crt. */

    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif

    #ifndef TM_MEMMOVE
        #include <string.h>
        #define TM_MEMMOVE memmove
    #endif

    #ifndef TM_MEMSET
        #include <string.h>
        #define TM_MEMSET memset
    #endif
#endif

#ifndef _TM_POLYGON_H_INCLUDED_
#define _TM_POLYGON_H_INCLUDED_

#define TMPO_VERSION 0x01000001u

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h>           /* include C version so identifiers are in global namespace */
    typedef size_t tm_size_t;
#endif /* !defined(TM_SIZE_T_DEFINED) */

/* Native bools, override by defining TM_BOOL_DEFINED yourself before including this file. */
#ifndef TM_BOOL_DEFINED
    #define TM_BOOL_DEFINED
    #ifdef __cplusplus
        typedef bool tm_bool;
        #define TM_TRUE true
        #define TM_FALSE false
    #else
        typedef _Bool tm_bool;
        #define TM_TRUE 1
        #define TM_FALSE 0
    #endif
#endif /* !defined(TM_BOOL_DEFINED) */

#ifndef TMPO_OWN_TYPES
    typedef unsigned short tmpo_index;
    typedef unsigned short tmpo_uint16;
#endif

#ifdef __cplusplus
    #define TMPO_UNDERLYING : tmpo_uint16
#else
    #define TMPO_UNDERLYING
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TMPO_VECTOR
    typedef struct {
        float x;
        float y;
    } tmpo_vector;
    #define TMPO_VECTOR tmpo_vector
    /* If your vector struct is too big for value copies, #define this with const your_vector&. */
    #define TMPO_VECTOR_CONST_REF tmpo_vector
#endif

#ifndef TMPO_CLOCKWISE_TRIANGLES
    /* Define this to 0 if you want anti clockwise triangles to be generated. */
    #define TMPO_CLOCKWISE_TRIANGLES 1
#endif

#ifndef TMPO_STATIC
    #define TMPO_DEF extern
#else
    #define TMPO_DEF static
#endif

/* clang-format on */

TMPO_DEF tm_bool isPolygonClockwise(const TMPO_VECTOR* vertices, tm_size_t count);

/*
Takes a polygon and outputs a stream of indices that define triangles.
Params:
    vertices:    Vertices that the polygon is made out of.
    count:       How many vertices there are.
    clockwise:   Whether the polygon is oriented clockwise.
    queryList:   Internal list the algorithm needs to triangulate. Make sure it is as big as count.
    queryCount:  The size of the queryList. Make sure it is as big as count.
    begin:       Offset to the indices.
    out:         The resulting indices will be placed here. Make sure that it is at least 3 * count.
    maxIndices:  Size of the array in out.
Returns the number of indices generated.
*/
TMPO_DEF tm_size_t triangulatePolygonEarClipping(const TMPO_VECTOR* vertices, tm_size_t count, tm_bool clockwise,
                                                 tmpo_index* queryList, tm_size_t queryCount, tmpo_index begin,
                                                 tmpo_index* out, tm_size_t maxIndices);

/* Clipping using Greiner–Hormann clipping algorithm. */
typedef struct {
    TMPO_VECTOR pos;
    tmpo_uint16 next;
    tmpo_uint16 prev;
    tmpo_uint16 nextPoly;
    tmpo_uint16 flags;
    tmpo_uint16 neighbor;
    float alpha;
} ClipVertex;

enum ClipVertexFlags TMPO_UNDERLYING {
    CVF_INTERSECT = (1 << 0),
    CVF_EXIT = (1 << 1),
    CVF_PROCESSED = (1 << 2),

    CVF_FORCE_16 = 0x7FFF
};

typedef enum { CFD_FORWARD, CFD_BACKWARD } ClipFollowDirection;

typedef struct {
    ClipVertex* data;
    tm_size_t originalSize;
    tm_size_t size;
    tm_size_t capacity;
} ClipVertices;

typedef struct {
    TMPO_VECTOR* vertices;
    tm_size_t size;
} ClipPolygonEntry;

typedef struct {
    tm_size_t polygons; /* How many polygons we emitted. */
    tm_size_t vertices; /* How many vertices were used up for the polygons in total. */
} ClipPolyResult;

/*
Converts an array of vertices into a format usable by the clipping algorithm.
Note that you supply the memory for the algorithm to use in the 3rd and 4th argument.
Make sure that you supply enough clipVertices so that intersections can be stored.
*/
TMPO_DEF ClipVertices clipPolyTransformData(const TMPO_VECTOR* vertices, tm_size_t count, ClipVertex* clipVertices,
                                            tm_size_t maxClipVertices);

/* Phase 1 of the clipping algorithm. */
TMPO_DEF void clipPolyFindIntersections(ClipVertices* a, ClipVertices* b);

/*
Phase 2 of the clipping algorithm.
The ClipFollowDirection values depend on what operation you want to do:
     Polygon a    | Polygon b    | Result
     CFD_FORWARD  | CFD_FORWARD  | a AND b (difference of a and b)
     CFD_BACKWARD | CFD_FORWARD  | a \ b
     CFD_FORWARD  | CFD_BACKWARD | b \ a
     CFD_BACKWARD | CFD_BACKWARD | a OR b (union of a and b)
*/
TMPO_DEF void clipPolyMarkEntryExitPoints(ClipVertices* a, ClipVertices* b, ClipFollowDirection aDir,
                                          ClipFollowDirection bDir);

/*
Phase 3 of the clipping algorithm.
The clipped polygons will be emitted. The memory for the polygons are passed in through the
arguments 3 through 6. All polygons share the same vertices pool (arguments 5 and 6).
Returns a structure containing the number of polygons emitted and the total number of vertices
consumed. The number of vertices that each polygon takes up is stored directly in the third
argument (polygons).
*/
TMPO_DEF ClipPolyResult clipPolyEmitClippedPolygons(ClipVertices* a, ClipVertices* b, ClipPolygonEntry* polygons,
                                                    tm_size_t maxPolygons, TMPO_VECTOR* vertices, tm_size_t maxCount);

/* Convenience function in case you only expect one polygon to be emitted. */
TMPO_DEF tm_size_t clipPolyEmitClippedPolygon(ClipVertices* a, ClipVertices* b, TMPO_VECTOR* vertices,
                                              tm_size_t maxCount);

#ifdef __cplusplus
}
#endif

#endif /* _TM_POLYGON_H_INCLUDED_ */

/* implementation */

#ifdef TM_POLYGON_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

TMPO_DEF tm_bool isPolygonClockwise(const TMPO_VECTOR* vertices, tm_size_t count) {
    float sum = 0;
    tm_size_t last = count - 1;
    for (tm_size_t i = 0; i < count; last = i, ++i) {
        sum += vertices[last].x * vertices[i].y - vertices[last].y * vertices[i].x;
    }
    return sum >= 0;
}

static int isTriangleClockwise(TMPO_VECTOR_CONST_REF a, TMPO_VECTOR_CONST_REF b, TMPO_VECTOR_CONST_REF c) {
    float bx = b.x - a.x;
    float by = b.y - a.y;
    float cx = c.x - a.x;
    float cy = c.y - a.y;

    float cross = bx * cy - by * cx;
    return cross >= 0;
}
static int pointInsideTriangle(TMPO_VECTOR_CONST_REF a, TMPO_VECTOR_CONST_REF b, TMPO_VECTOR_CONST_REF c,
                               TMPO_VECTOR_CONST_REF v) {
    float bx = b.x - a.x;
    float by = b.y - a.y;
    float cx = c.x - a.x;
    float cy = c.y - a.y;
    float vx = v.x - a.x;
    float vy = v.y - a.y;

    float bc = bx * cx + by * cy;
    float vc = vx * cx + vy * cy;
    float vb = vx * bx + vy * by;
    float cc = cx * cx + cy * cy;
    float bb = bx * bx + by * by;

    float invDenom = 1.0f / (bb * cc - bc * bc);
    float r = (cc * vb - bc * vc) * invDenom;
    float s = (bb * vc - bc * vb) * invDenom;

    return (r >= 0) && (s >= 0) && (r + s <= 1);
}
static int isTriangleEar(tm_size_t a, tm_size_t b, tm_size_t c, const TMPO_VECTOR* vertices, tm_size_t count, int cw) {
    TM_ASSERT(a < count);
    TM_ASSERT(b < count);
    TM_ASSERT(c < count);

    TMPO_VECTOR va = vertices[a];
    TMPO_VECTOR vb = vertices[b];
    TMPO_VECTOR vc = vertices[c];
    if (isTriangleClockwise(va, vb, vc) != cw) {
        return 0;
    }

    for (tm_size_t i = 0; i < count; ++i) {
        if (i == a || i == b || i == c) {
            continue;
        }
        if (pointInsideTriangle(va, vb, vc, vertices[i])) {
            return 0;
        }
    }

    return 1;
}

TMPO_DEF tm_size_t triangulatePolygonEarClipping(const TMPO_VECTOR* vertices, tm_size_t count, tm_bool clockwise,
                                                 tmpo_index* queryList, tm_size_t queryCount, tmpo_index begin,
                                                 tmpo_index* out, tm_size_t maxIndices) {
    TM_ASSERT(vertices);
    TM_ASSERT(count >= 0);
    TM_ASSERT(queryList);
    TM_ASSERT(queryCount >= count);
    TM_ASSERT(out);
    TM_ASSERT(maxIndices);

    if (count < 3) {
        return 0;
    }

    /* int clockwise = isTriangleClockwise( vertices[0], vertices[1], vertices[2] ); */
    tm_size_t verticesCount = count;

    tm_size_t size = (count < queryCount) ? (count) : queryCount;
    for (tm_size_t i = 0; i < size; ++i) {
        queryList[i] = (tmpo_index)i;
    }

    tm_size_t indicesCount = 0;
    tm_size_t a = 0, b = 1, c = 2;
    tm_size_t current = 2;
    tm_size_t iterationCount = 0;
    while (size > 2) {
        if (isTriangleEar(queryList[a], queryList[b], queryList[c], vertices, verticesCount, clockwise)) {
            if (indicesCount + 3 > maxIndices) {
                TM_ASSERT(0 && "out of memory");
                break;
            }
            if (clockwise == TMPO_CLOCKWISE_TRIANGLES) {
                *(out++) = queryList[a] + begin;
                *(out++) = queryList[b] + begin;
                *(out++) = queryList[c] + begin;
            } else {
                *(out++) = queryList[a] + begin;
                *(out++) = queryList[c] + begin;
                *(out++) = queryList[b] + begin;
            }
            indicesCount += 3;

            --size;
            iterationCount = 0;
            TM_MEMMOVE(queryList + b, queryList + b + 1, (size - b) * sizeof(tmpo_index));
            current = a;
            if (current >= size) {
                current = current - size;
            }
            if (current >= 2) {
                a = current - 2;
                b = current - 1;
            } else {
                a = size - (2 - current);

                if (current >= 1) {
                    b = current - 1;
                } else {
                    b = size - (1 - current);
                }
            }
            c = current;
        } else {
            a = b;
            b = current;
            ++current;
            c = current;
            if (current >= size) {
                current = 0;
                a = size - 2;
                b = size - 1;
                c = current;
            }
            if (iterationCount > 2 * size) {
                break;
            }
            ++iterationCount;
        }
    }

    return (tm_size_t)indicesCount;
}

/* clipPoly implementation */

TMPO_DEF ClipVertices clipPolyTransformData(const TMPO_VECTOR* vertices, tm_size_t count, ClipVertex* clipVertices,
                                            tm_size_t maxClipVertices) {
    TM_ASSERT(maxClipVertices >= count);

    ClipVertices result;
    result.data = clipVertices;
    result.originalSize = count;
    result.size = count;
    result.capacity = maxClipVertices;

    tm_size_t verticesCount = count;
    TM_MEMSET(clipVertices, 0, verticesCount * sizeof(ClipVertex));
    tmpo_index prev = (tmpo_index)verticesCount - 1;
    for (tm_size_t i = 0; i < verticesCount; ++i) {
        ClipVertex* entry = &clipVertices[i];
        entry->pos = vertices[i];
        entry->next = (tmpo_index)(i + 1);
        entry->prev = prev;
        prev = (tmpo_index)i;
    }
    if (verticesCount) {
        clipVertices[verticesCount - 1].next = 0;
    }
    return result;
}

/* Helpers */
static ClipVertex* clipPolyCreateVertex(ClipVertices* vertices, tm_size_t at) {
    /* Contrary to how insertion works in linked lists usually, we insert AFTER at, not before.
       This safes us some checks because of the way we iterate over vertices. */

    TM_ASSERT(vertices->size + 1 <= vertices->capacity);
    ClipVertex* added = &vertices->data[vertices->size];
    ClipVertex* ref = &vertices->data[at];
    ClipVertex* oldNext = &vertices->data[ref->next];
    added->prev = (tmpo_index)at;
    added->next = ref->next;
    added->flags = 0;
    oldNext->prev = ref->next = (tmpo_index)vertices->size;
    ++vertices->size;
    return added;
}
static int clipPolyLineIntersectionFactor(TMPO_VECTOR_CONST_REF a, TMPO_VECTOR_CONST_REF aDir, TMPO_VECTOR_CONST_REF b,
                                          TMPO_VECTOR_CONST_REF bDir, float* t) {
    TM_ASSERT(t);

    int hit = 0;
    float cross = aDir.x * bDir.y - aDir.y * bDir.x;
    if (cross < 0.000001f || cross > 0.000001f) {
        float relx = a.x - b.x;
        float rely = a.y - b.y;
        *t = (bDir.x * rely - bDir.y * relx) / cross;
        hit = 1;
    }
    return hit;
}

static tm_size_t clipPolyFindIntersectionIndex(ClipVertices* vertices, tm_size_t at, float alpha) {
    ClipVertex* ref = &vertices->data[at];
    while ((ref->flags & CVF_INTERSECT) && ref->alpha > alpha) {
        at = ref->prev;
        ref = &vertices->data[at];
    }
    return at;
}
static void clipPolyCreateIntersection(ClipVertices* vertices, tm_size_t at, TMPO_VECTOR_CONST_REF intersection,
                                       tm_size_t neighbor, float alpha) {
    ClipVertex* added = clipPolyCreateVertex(vertices, at);
    added->pos = intersection;
    added->flags |= CVF_INTERSECT;
    added->neighbor = (tmpo_index)neighbor;
    added->alpha = alpha;
}

TMPO_DEF void clipPolyFindIntersections(ClipVertices* a, ClipVertices* b) {
    tm_size_t aCount = a->originalSize;
    tm_size_t bCount = b->originalSize;
    tm_size_t aPrevIndex = aCount - 1;
    tm_size_t bPrevIndex = bCount - 1;
    for (tm_size_t i = 0; i < aCount; aPrevIndex = i, ++i) {
        for (tm_size_t j = 0; j < bCount;) {
            ClipVertex* aVertex = &a->data[i];
            TMPO_VECTOR aCurrent = aVertex->pos;
            TMPO_VECTOR aPrev = a->data[aPrevIndex].pos;

            ClipVertex* bVertex = &b->data[j];
            TMPO_VECTOR bCurrent = bVertex->pos;
            TMPO_VECTOR bPrev = b->data[bPrevIndex].pos;

            TMPO_VECTOR aDir = {0};
            aDir.x = aCurrent.x - aPrev.x;
            aDir.y = aCurrent.y - aPrev.y;
            TMPO_VECTOR bDir = {0};
            bDir.x = bCurrent.x - bPrev.x;
            bDir.y = bCurrent.y - bPrev.y;

            float aAlpha, bAlpha;
            if (clipPolyLineIntersectionFactor(aPrev, aDir, bPrev, bDir, &aAlpha) &&
                clipPolyLineIntersectionFactor(bPrev, bDir, aPrev, aDir, &bAlpha) && aAlpha >= 0.0f && aAlpha <= 1.0f &&
                bAlpha >= 0.0f && bAlpha <= 1.0f) {
                /* Check for degenerates, ie intersection lies directly on an edge. */
                /* NOTE: To be exact, we should normalize the push instead of multiplying with
                   0.0001 but in most cases this should be fine. */
                if (aAlpha <= 0.00001f) {
                    /* aPrev is degenerate */
                    TMPO_VECTOR* v = &a->data[aPrevIndex].pos;
                    v->x -= bDir.y * 0.0001f;
                    v->y += bDir.x * 0.0001f;
                    continue;
                }
                if (aAlpha >= 0.99999f) {
                    /* aCurrent is degenerate */
                    TMPO_VECTOR* v = &aVertex->pos;
                    v->x -= bDir.y * 0.0001f;
                    v->y += bDir.x * 0.0001f;
                    continue;
                }
                if (bAlpha <= 0.00001f) {
                    /* bPrev is degenerate */
                    TMPO_VECTOR* v = &b->data[bPrevIndex].pos;
                    v->x -= aDir.y * 0.0001f;
                    v->y += aDir.x * 0.0001f;
                    continue;
                }
                if (bAlpha >= 0.99999f) {
                    /* bCurrent is degenerate */
                    TMPO_VECTOR* v = &bVertex->pos;
                    v->x -= aDir.y * 0.0001f;
                    v->y += aDir.x * 0.0001f;
                    continue;
                }

                TMPO_VECTOR intersection = {0};
                intersection.x = aPrev.x + aAlpha * aDir.x;
                intersection.y = aPrev.y + aAlpha * aDir.y;
                tm_size_t aIndex = clipPolyFindIntersectionIndex(a, aVertex->prev, aAlpha);
                tm_size_t bIndex = clipPolyFindIntersectionIndex(b, bVertex->prev, bAlpha);
                tm_size_t aNeighbor = b->size;
                tm_size_t bNeighbor = a->size;
                clipPolyCreateIntersection(a, aIndex, intersection, aNeighbor, aAlpha);
                clipPolyCreateIntersection(b, bIndex, intersection, bNeighbor, bAlpha);
            }
            bPrevIndex = j;
            ++j;
        }
    }
}

static int clipPolyPointInsidePoly(ClipVertices* poly, TMPO_VECTOR_CONST_REF p) {
    unsigned cn = 0;
    tm_size_t count = poly->originalSize;
    tm_size_t prevIndex = count - 1;
    for (tm_size_t i = 0; i < count; prevIndex = i, ++i) {
        TMPO_VECTOR* cur = &poly->data[i].pos;
        TMPO_VECTOR* prev = &poly->data[prevIndex].pos;

        /* Simple bounds check whether the lines can even intersect. */
        if ((p.y <= prev->y && p.y > cur->y) || (p.y > prev->y && p.y <= cur->y)) {
            /* Calculate intersection with horizontal ray. */
            float alpha = (prev->y - p.y) / (prev->y - cur->y);
            float xIntersection = prev->x + alpha * (cur->x - prev->x);
            if (p.x < xIntersection) {
                ++cn;
            }
        }
    }

    /* If the counter is even, the point is outside, otherwise the point is inside. */
    return (cn % 2);
}
static void clipPolyMarkEntryExitPointsSingle(ClipVertices* current, ClipVertices* other, ClipFollowDirection dir) {
    tm_size_t count = current->size;
    if (count > 0) {
        int inside = clipPolyPointInsidePoly(other, current->data[0].pos);
        if (dir != CFD_FORWARD) {
            inside = !inside;
        }
        tm_size_t i = current->data[0].next;
        do {
            ClipVertex* entry = &current->data[i];
            if (entry->flags & CVF_INTERSECT) {
                if (inside) {
                    entry->flags |= CVF_EXIT;
                }
                inside = !inside;
            }
            i = entry->next;
        } while (i != 0);
    }
}

TMPO_DEF void clipPolyMarkEntryExitPoints(ClipVertices* a, ClipVertices* b, ClipFollowDirection aDir,
                                          ClipFollowDirection bDir) {
    TM_ASSERT(a);
    TM_ASSERT(b);

    clipPolyMarkEntryExitPointsSingle(a, b, aDir);
    clipPolyMarkEntryExitPointsSingle(b, a, bDir);
}

TMPO_DEF ClipPolyResult clipPolyEmitClippedPolygons(ClipVertices* a, ClipVertices* b, ClipPolygonEntry* polygons,
                                                    tm_size_t maxPolygons, TMPO_VECTOR* vertices, tm_size_t maxCount) {
    TM_ASSERT(a);
    TM_ASSERT(b);
    TM_ASSERT(vertices);

    ClipPolyResult result = {0};

    if (a->size < 1) {
        return result;
    }

    ClipVertices* currentVertices = a;
    ClipVertices* otherVertices = b;
    /* First vertex is never an intersection. */
    tm_size_t i = currentVertices->data[0].next;

    tm_size_t currentPoly = 1;
    tm_size_t polyCount = 0;

    tm_size_t put = 0;
    tm_size_t size = maxCount;
    ClipVertex* current = &currentVertices->data[i];
    int hasIntersections = 0;
    while (i != 0) {
        /* Current is intersection but not processed. */
        if ((current->flags & (CVF_INTERSECT | CVF_PROCESSED)) == CVF_INTERSECT) {
            current->flags |= CVF_PROCESSED;
            hasIntersections = 1;

            /* New polygon */
            if (currentPoly < polyCount) {
                polygons[currentPoly].size = (tm_size_t)(&vertices[put] - polygons[currentPoly].vertices);
            }
            if (polyCount + 1 > maxPolygons) {
                TM_ASSERT(0 && "out of memory");
                result.polygons = (tm_size_t)polyCount;
                result.vertices = (tmpo_index)put;
                return result;
            }
            currentPoly = polyCount++;
            polygons[currentPoly].vertices = &vertices[put];
            tm_size_t start = i;
            ClipVertices* startVertices = currentVertices;
            do {
                if (current->flags & CVF_EXIT) {
                    do {
                        i = current->prev;
                        current = &currentVertices->data[i];
                        current->flags |= CVF_PROCESSED;
                        if (put + 1 > size) {
                            TM_ASSERT(0 && "out of memory");
                            result.polygons = (tm_size_t)polyCount;
                            result.vertices = (tmpo_index)put;
                            return result;
                        }
                        vertices[put++] = current->pos;
                    } while (!(current->flags & CVF_INTERSECT));
                } else {
                    do {
                        i = current->next;
                        current = &currentVertices->data[i];
                        current->flags |= CVF_PROCESSED;
                        if (put + 1 > size) {
                            TM_ASSERT(0 && "out of memory");
                            result.polygons = (tm_size_t)polyCount;
                            result.vertices = (tmpo_index)put;
                            return result;
                        }
                        vertices[put++] = current->pos;
                    } while (!(current->flags & CVF_INTERSECT));
                }
                /* ClipVertex* prev = current; */
                i = current->neighbor;
                ClipVertices* temp = currentVertices;
                currentVertices = otherVertices;
                otherVertices = temp;
                current = &currentVertices->data[i];
                current->flags |= CVF_PROCESSED;
                TM_ASSERT(current->flags & CVF_INTERSECT);
                /* TM_ASSERT( ( current->flags & CVF_EXIT ) == ( prev->flags & CVF_EXIT ) ); */
            } while (i != start || currentVertices != startVertices);
        }
        i = current->next;
        current = &currentVertices->data[i];
    }

    if (!hasIntersections) {
        /* Is a completely inside b? */
        if (clipPolyPointInsidePoly(b, a->data[0].pos)) {
            if (polyCount + 1 > maxPolygons) {
                TM_ASSERT(0 && "out of memory");
                result.polygons = (tm_size_t)polyCount;
                result.vertices = (tmpo_index)put;
                return result;
            }
            currentPoly = polyCount++;
            polygons[currentPoly].vertices = &vertices[put];
            if (size > a->originalSize) {
                size = a->originalSize;
            }
            for (tm_size_t j = 0; j < size; ++j) {
                vertices[j] = a->data[j].pos;
            }
            put = size;
        } else if (b->size && clipPolyPointInsidePoly(a, b->data[0].pos)) {
            if (polyCount + 1 > maxPolygons) {
                TM_ASSERT(0 && "out of memory");
                result.polygons = (tm_size_t)polyCount;
                result.vertices = (tmpo_index)put;
                return result;
            }
            currentPoly = polyCount++;
            polygons[currentPoly].vertices = &vertices[put];
            if (size > b->originalSize) {
                size = b->originalSize;
            }
            for (tm_size_t j = 0; j < size; ++j) {
                vertices[j] = b->data[j].pos;
            }
            put = size;
        }
    }
    if (currentPoly < polyCount) {
        polygons[currentPoly].size = (tm_size_t)(&vertices[put] - polygons[currentPoly].vertices);
    }
    result.polygons = (tm_size_t)polyCount;
    result.vertices = (tmpo_index)put;
    return result;
}

TMPO_DEF tm_size_t clipPolyEmitClippedPolygon(ClipVertices* a, ClipVertices* b, TMPO_VECTOR* vertices,
                                              tm_size_t maxCount) {
    ClipPolygonEntry entry = {0};
    clipPolyEmitClippedPolygons(a, b, &entry, 1, vertices, maxCount);
    return entry.size;
}

#ifdef __cplusplus
}
#endif

#endif /* TM_POLYGON_IMPLEMENTATION */

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

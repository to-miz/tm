/*
tm_polygon.h v1.0 - public domain
author: Tolga Mizrak 2016

no warranty; use at your own risk

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
	v1.0 26.06.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

#ifdef TM_POLYGON_IMPLEMENTATION
// define these to avoid crt

	#ifndef TMP_ASSERT
		#include <assert.h>
		#define TMP_ASSERT assert
	#endif

	#ifndef TMP_MEMMOVE
		#include <memory.h>
		#define TMP_MEMMOVE memmove
	#endif

	#ifndef TMP_MEMSET
		#include <memory.h>
		#define TMP_MEMSET memset
	#endif
#endif

#ifndef _TM_POLYGON_H_INCLUDED_
#define _TM_POLYGON_H_INCLUDED_

#ifndef TMP_OWN_TYPES
	typedef unsigned short tmp_index;
	typedef size_t tmp_size_t;
	typedef unsigned short tmp_uint16;
	#ifdef __cplusplus
		typedef bool tmp_bool;
	#else
		typedef int tmp_bool;
	#endif
#endif

#ifdef __cplusplus
	#define TMP_UNDERLYING : tmp_uint16
#else
	#define TMP_UNDERLYING
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TMP_VECTOR
	typedef struct {
		float x;
		float y;
	} tmp_vector;
	#define TMP_VECTOR tmp_vector
	// if your vector struct is too big for value copies, #define this with const your_vector&
	#define TMP_VECTOR_CONST_REF tmp_vector
#endif


#ifndef TMP_CLOCKWISE_TRIANGLES
	// define this to 0 if you want anti clockwise triangles to be generated
	#define TMP_CLOCKWISE_TRIANGLES 1
#endif

#ifndef TMP_STATIC
	#define TMP_DEF extern
#else
	#define TMP_DEF static
#endif

TMP_DEF tmp_bool isPolygonClockwise( const TMP_VECTOR* vertices, tmp_size_t count );

// takes a polygon and outputs a stream of indices that define triangles
// Args:
//		vertices: vertices that the polygon is made out of
//		count: how many vertices there are
//		clockwise: whether the polygon is oriented clockwise
//		queryList: internal list the algorithm needs to triangulate. Make sure it is as big as count
//		queryCount: the size of the queryList. Make sure it is as big as count
//		begin: offset to the indices
//		out: the resulting indices will be placed here. Make sure that it is at least 3 * count.
//		maxIndices: size of the array in out
// returns the number of indices generated
TMP_DEF tmp_size_t triangulatePolygonEarClipping( const TMP_VECTOR* vertices, tmp_size_t count,
												  tmp_bool clockwise, tmp_index* queryList,
												  tmp_size_t queryCount, tmp_index begin,
												  tmp_index* out, tmp_size_t maxIndices );

// clipping using Greiner–Hormann clipping algorithm
typedef struct {
	TMP_VECTOR pos;
	tmp_uint16 next;
	tmp_uint16 prev;
	tmp_uint16 nextPoly;
	tmp_uint16 flags;
	tmp_uint16 neighbor;
	float alpha;
} ClipVertex;

enum ClipVertexFlags TMP_UNDERLYING {
	CVF_INTERSECT = ( 1 << 0 ),
	CVF_EXIT = ( 1 << 1 ),
	CVF_PROCESSED = ( 1 << 2 ),

	CVF_FORCE_16 = 0x7FFF
};

typedef enum { CFD_FORWARD, CFD_BACKWARD } ClipFollowDirection;

typedef struct {
	ClipVertex* data;
	tmp_size_t originalSize;
	tmp_size_t size;
	tmp_size_t capacity;
} ClipVertices;

typedef struct {
	TMP_VECTOR* vertices;
	tmp_size_t size;
} ClipPolygonEntry;

typedef struct {
	tmp_size_t polygons;  // how many polygons we emitted
	tmp_size_t vertices;  // how many vertices were used up for the polygons in total
} ClipPolyResult;

// converts an array of vertices into a format usable by the clipping algorithm
// note that you supply the memory for the algorithm to use in the 3rd and 4th argument
// make sure that you supply enough clipVertices so that intersections can be stored
TMP_DEF ClipVertices clipPolyTransformData( const TMP_VECTOR* vertices, tmp_size_t count,
											ClipVertex* clipVertices, tmp_size_t maxClipVertices );

// phase 1 of the clipping algorithm
TMP_DEF void clipPolyFindIntersections( ClipVertices* a, ClipVertices* b );

// phase 2 of the clipping algorithm
// the ClipFollowDirection values depend on what operation you want to do:
//		Polygon a    | Polygon b    | Result
//		CFD_FORWARD  | CFD_FORWARD  | a AND b (difference of a and b)
//		CFD_BACKWARD | CFD_FORWARD  | a \ b
//		CFD_FORWARD  | CFD_BACKWARD | b \ a
//		CFD_BACKWARD | CFD_BACKWARD | a OR b (union of a and b)
TMP_DEF void clipPolyMarkEntryExitPoints( ClipVertices* a, ClipVertices* b,
										  ClipFollowDirection aDir, ClipFollowDirection bDir );

// phase 3 of the clipping algorithm
// the clipped polygons will be emitted. The memory for the polygons are passed in through the
// arguments 3 through 6. All polygons share the same vertices pool (arguments 5 and 6).
// returns a structure containing the number of polygons emitted and the total number of vertices
// consumed. The number of vertices that each polygon takes up is stored directly in the third
// argument (polygons)
TMP_DEF ClipPolyResult clipPolyEmitClippedPolygons( ClipVertices* a, ClipVertices* b,
                                                    ClipPolygonEntry* polygons,
                                                    tmp_size_t maxPolygons,
                                                    TMP_VECTOR* vertices, tmp_size_t maxCount );

// convenience function in case you only expect one polygon to be emitted
TMP_DEF tmp_size_t clipPolyEmitClippedPolygon( ClipVertices* a, ClipVertices* b,
											   TMP_VECTOR* vertices, tmp_size_t maxCount );

#ifdef __cplusplus
}
#endif

#endif  // _TM_POLYGON_H_INCLUDED_

// implementation

#ifdef TM_POLYGON_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

TMP_DEF tmp_bool isPolygonClockwise( const TMP_VECTOR* vertices, tmp_size_t count )
{
	float sum = 0;
	size_t size = count;
	size_t last = size - 1;
	for( size_t i = 0; i < size; last = i, ++i ) {
		sum += vertices[last].x * vertices[i].y - vertices[last].y * vertices[i].x;
	}
	return sum >= 0;
}

static int isTriangleClockwise( TMP_VECTOR_CONST_REF a, TMP_VECTOR_CONST_REF b,
								TMP_VECTOR_CONST_REF c )
{
	float bx = b.x - a.x;
	float by = b.y - a.y;
	float cx = c.x - a.x;
	float cy = c.y - a.y;

	float cross = bx * cy - by * cx;
	return cross >= 0;
}
static int pointInsideTriangle( TMP_VECTOR_CONST_REF a, TMP_VECTOR_CONST_REF b,
								TMP_VECTOR_CONST_REF c, TMP_VECTOR_CONST_REF v )
{
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

	float invDenom = 1.0f / ( bb * cc - bc * bc );
	float r = ( cc * vb - bc * vc ) * invDenom;
	float s = ( bb * vc - bc * vb ) * invDenom;

	return ( r >= 0 ) && ( s >= 0 ) && ( r + s <= 1 );
}
static int isTriangleEar( size_t a, size_t b, size_t c, const TMP_VECTOR* vertices, size_t count,
						  int cw )
{
	TMP_ASSERT( a < count );
	TMP_ASSERT( b < count );
	TMP_ASSERT( c < count );

	TMP_VECTOR va = vertices[a];
	TMP_VECTOR vb = vertices[b];
	TMP_VECTOR vc = vertices[c];
	if( isTriangleClockwise( va, vb, vc ) != cw ) {
		return 0;
	}

	for( size_t i = 0; i < count; ++i ) {
		if( i == a || i == b || i == c ) {
			continue;
		}
		if( pointInsideTriangle( va, vb, vc, vertices[i] ) ) {
			return 0;
		}
	}

	return 1;
}

TMP_DEF tmp_size_t triangulatePolygonEarClipping( const TMP_VECTOR* vertices, tmp_size_t count,
												  tmp_bool clockwise, tmp_index* queryList,
												  tmp_size_t queryCount, tmp_index begin,
												  tmp_index* out, tmp_size_t maxIndices )
{
	TMP_ASSERT( vertices );
	TMP_ASSERT( count >= 0 );
	TMP_ASSERT( queryList );
	TMP_ASSERT( queryCount >= count );
	TMP_ASSERT( out );
	TMP_ASSERT( maxIndices );

	if( count < 3 ) {
		return 0;
	}

	// int clockwise = isTriangleClockwise( vertices[0], vertices[1], vertices[2] );
	size_t verticesCount = (size_t)count;

	size_t size = ( count < queryCount ) ? ( count ) : queryCount;
	for( size_t i = 0; i < size; ++i ) {
		queryList[i] = (tmp_index)i;
	}

	size_t indicesCount = 0;
	size_t a = 0, b = 1, c = 2;
	size_t current = 2;
	size_t iterationCount = 0;
	while( size > 2 ) {
		if( isTriangleEar( queryList[a], queryList[b], queryList[c], vertices, verticesCount,
						   clockwise ) ) {
			if( indicesCount + 3 > maxIndices ) {
				TMP_ASSERT( 0 && "out of memory" );
				break;
			}
			if( clockwise == TMP_CLOCKWISE_TRIANGLES ) {
				*( out++ ) = queryList[a] + begin;
				*( out++ ) = queryList[b] + begin;
				*( out++ ) = queryList[c] + begin;
			} else {
				*( out++ ) = queryList[a] + begin;
				*( out++ ) = queryList[c] + begin;
				*( out++ ) = queryList[b] + begin;
			}
			indicesCount += 3;

			--size;
			iterationCount = 0;
			TMP_MEMMOVE( queryList + b, queryList + b + 1, ( size - b ) * sizeof( tmp_index ) );
			current = a;
			if( current >= size ) {
				current = current - size;
			}
			if( current >= 2 ) {
				a = current - 2;
				b = current - 1;
			} else {
				a = size - ( 2 - current );

				if( current >= 1 ) {
					b = current - 1;
				} else {
					b = size - ( 1 - current );
				}
			}
			c = current;
		} else {
			a = b;
			b = current;
			++current;
			c = current;
			if( current >= size ) {
				current = 0;
				a = size - 2;
				b = size - 1;
				c = current;
			}
			if( iterationCount > 2 * size ) {
				break;
			}
			++iterationCount;
		}
	}

	return (tmp_size_t)indicesCount;
}

// clipPoly implementation

TMP_DEF ClipVertices clipPolyTransformData( const TMP_VECTOR* vertices, tmp_size_t count,
											ClipVertex* clipVertices, tmp_size_t maxClipVertices )
{
	TMP_ASSERT( maxClipVertices >= count );

	ClipVertices result = {clipVertices, count, count, maxClipVertices};

	size_t verticesCount = (size_t)count;
	TMP_MEMSET( clipVertices, 0, verticesCount * sizeof( ClipVertex ) );
	tmp_index prev = (tmp_index)verticesCount - 1;
	for( size_t i = 0; i < verticesCount; ++i ) {
		ClipVertex* entry = &clipVertices[i];
		entry->pos = vertices[i];
		entry->next = ( tmp_index )( i + 1 );
		entry->prev = prev;
		prev = (tmp_index)i;
	}
	if( verticesCount ) {
		clipVertices[verticesCount - 1].next = 0;
	}
	return result;
}

// helpers
static ClipVertex* clipPolyCreateVertex( ClipVertices* vertices, size_t at )
{
	// contrary to how insertion works in linked lists usually, we insert AFTER at, not before
	// this safes us some checks because of the way we iterate over vertices

	TMP_ASSERT( vertices->size + 1 <= vertices->capacity );
	ClipVertex* added = &vertices->data[(size_t)vertices->size];
	ClipVertex* ref = &vertices->data[at];
	ClipVertex* oldNext = &vertices->data[ref->next];
	added->prev = (tmp_index)at;
	added->next = ref->next;
	added->flags = 0;
	oldNext->prev = ref->next = (tmp_index)vertices->size;
	++vertices->size;
	return added;
}
static int clipPolyLineIntersectionFactor( TMP_VECTOR_CONST_REF a, TMP_VECTOR_CONST_REF aDir,
										   TMP_VECTOR_CONST_REF b, TMP_VECTOR_CONST_REF bDir,
										   float* t )
{
	TMP_ASSERT( t );

	int hit = 0;
	float cross = aDir.x * bDir.y - aDir.y * bDir.x;
	if( cross < 0.000001f || cross > 0.000001f ) {
		float relx = a.x - b.x;
		float rely = a.y - b.y;
		*t = ( bDir.x * rely - bDir.y * relx ) / cross;
		hit = 1;
	}
	return hit;
}

static size_t clipPolyFindIntersectionIndex( ClipVertices* vertices, size_t at, float alpha )
{
	ClipVertex* ref = &vertices->data[at];
	while( ( ref->flags & CVF_INTERSECT ) && ref->alpha > alpha ) {
		at = ref->prev;
		ref = &vertices->data[at];
	}
	return at;
}
static void clipPolyCreateIntersection( ClipVertices* vertices, size_t at,
										TMP_VECTOR_CONST_REF intersection, size_t neighbor,
										float alpha )
{
	ClipVertex* added = clipPolyCreateVertex( vertices, at );
	added->pos = intersection;
	added->flags |= CVF_INTERSECT;
	added->neighbor = (tmp_index)neighbor;
	added->alpha = alpha;
}

TMP_DEF void clipPolyFindIntersections( ClipVertices* a, ClipVertices* b )
{
	size_t aCount = a->originalSize;
	size_t bCount = b->originalSize;
	size_t aPrevIndex = aCount - 1;
	size_t bPrevIndex = bCount - 1;
	for( size_t i = 0; i < aCount; aPrevIndex = i, ++i ) {
		for( size_t j = 0; j < bCount; ) {
			ClipVertex* aVertex = &a->data[i];
			TMP_VECTOR aCurrent = aVertex->pos;
			TMP_VECTOR aPrev = a->data[aPrevIndex].pos;

			ClipVertex* bVertex = &b->data[j];
			TMP_VECTOR bCurrent = bVertex->pos;
			TMP_VECTOR bPrev = b->data[bPrevIndex].pos;

			TMP_VECTOR aDir = {aCurrent.x - aPrev.x, aCurrent.y - aPrev.y};
			TMP_VECTOR bDir = {bCurrent.x - bPrev.x, bCurrent.y - bPrev.y};

			float aAlpha, bAlpha;
			if( clipPolyLineIntersectionFactor( aPrev, aDir, bPrev, bDir, &aAlpha )
				&& clipPolyLineIntersectionFactor( bPrev, bDir, aPrev, aDir, &bAlpha )
				&& aAlpha >= 0.0f && aAlpha <= 1.0f && bAlpha >= 0.0f && bAlpha <= 1.0f ) {

				// check for degenerates, ie intersection lies directly on an edge
				// NOTE: to be exact, we should normalize the push instead of multiplying with
				// 0.0001 but in most cases this should be fine
				if( aAlpha <= 0.00001f ) {
					// aPrev is degenerate
					TMP_VECTOR* v = &a->data[aPrevIndex].pos;
					v->x -= bDir.y * 0.0001f;
					v->y += bDir.x * 0.0001f;
					continue;
				}
				if( aAlpha >= 0.99999f ) {
					// aCurrent is degenerate
					TMP_VECTOR* v = &aVertex->pos;
					v->x -= bDir.y * 0.0001f;
					v->y += bDir.x * 0.0001f;
					continue;
				}
				if( bAlpha <= 0.00001f ) {
					// bPrev is degenerate
					TMP_VECTOR* v = &b->data[bPrevIndex].pos;
					v->x -= aDir.y * 0.0001f;
					v->y += aDir.x * 0.0001f;
					continue;
				}
				if( bAlpha >= 0.99999f ) {
					// bCurrent is degenerate
					TMP_VECTOR* v = &bVertex->pos;
					v->x -= aDir.y * 0.0001f;
					v->y += aDir.x * 0.0001f;
					continue;
				}

				TMP_VECTOR intersection = {aPrev.x + aAlpha * aDir.x, aPrev.y + aAlpha * aDir.y};
				size_t aIndex = clipPolyFindIntersectionIndex( a, aVertex->prev, aAlpha );
				size_t bIndex = clipPolyFindIntersectionIndex( b, bVertex->prev, bAlpha );
				size_t aNeighbor = b->size;
				size_t bNeighbor = a->size;
				clipPolyCreateIntersection( a, aIndex, intersection, aNeighbor, aAlpha );
				clipPolyCreateIntersection( b, bIndex, intersection, bNeighbor, bAlpha );
			}
			bPrevIndex = j;
			++j;
		}
	}
}

static int clipPolyPointInsidePoly( ClipVertices* poly, TMP_VECTOR_CONST_REF p )
{
	unsigned cn = 0;
	size_t count = poly->originalSize;
	size_t prevIndex = count - 1;
	for( size_t i = 0; i < count; prevIndex = i, ++i ) {
		TMP_VECTOR* cur = &poly->data[i].pos;
		TMP_VECTOR* prev = &poly->data[prevIndex].pos;

		// simple bounds check whether the lines can even intersect
		if( ( p.y <= prev->y && p.y > cur->y ) || ( p.y > prev->y && p.y <= cur->y ) ) {
			// calculate intersection with horizontal ray
			float alpha = ( prev->y - p.y ) / ( prev->y - cur->y );
			float xIntersection = prev->x + alpha * ( cur->x - prev->x );
			if( p.x < xIntersection ) {
				++cn;
			}
		}
	}

	// if the counter is even, the point is outside, otherwise the point is inside
	return ( cn % 2 );
}
static void clipPolyMarkEntryExitPointsSingle( ClipVertices* current, ClipVertices* other,
											   ClipFollowDirection dir )
{
	size_t count = current->size;
	if( count > 0 ) {
		int inside = clipPolyPointInsidePoly( other, current->data[0].pos );
		if( dir != CFD_FORWARD ) {
			inside = !inside;
		}
		size_t i = current->data[0].next;
		do {
			ClipVertex* entry = &current->data[i];
			if( entry->flags & CVF_INTERSECT ) {
				if( inside ) {
					entry->flags |= CVF_EXIT;
				}
				inside = !inside;
			}
			i = entry->next;
		} while( i != 0 );
	}
}

TMP_DEF void clipPolyMarkEntryExitPoints( ClipVertices* a, ClipVertices* b,
										  ClipFollowDirection aDir, ClipFollowDirection bDir )
{
	TMP_ASSERT( a );
	TMP_ASSERT( b );

	clipPolyMarkEntryExitPointsSingle( a, b, aDir );
	clipPolyMarkEntryExitPointsSingle( b, a, bDir );
}

TMP_DEF ClipPolyResult
clipPolyEmitClippedPolygons( ClipVertices* a, ClipVertices* b, ClipPolygonEntry* polygons,
							 tmp_size_t maxPolygons, TMP_VECTOR* vertices, tmp_size_t maxCount )
{
	TMP_ASSERT( a );
	TMP_ASSERT( b );
	TMP_ASSERT( vertices );

	ClipPolyResult result = {};

	if( a->size < 1 ) {
		return result;
	}

	ClipVertices* currentVertices = a;
	ClipVertices* otherVertices = b;
	// first vertex is never an intersection
	size_t i = currentVertices->data[0].next;

	size_t currentPoly = 1;
	size_t polyCount = 0;

	size_t put = 0;
	size_t size = (size_t)maxCount;
	ClipVertex* current = &currentVertices->data[i];
	int hasIntersections = 0;
	while( i != 0 ) {
		// current is intersection but not processed
		if( ( current->flags & ( CVF_INTERSECT | CVF_PROCESSED ) ) == CVF_INTERSECT ) {
			current->flags |= CVF_PROCESSED;
			hasIntersections = 1;

			// newPolygon
			if( currentPoly < polyCount ) {
				polygons[currentPoly].size =
					( tmp_size_t )( &vertices[put] - polygons[currentPoly].vertices );
			}
			if( polyCount + 1 > maxPolygons ) {
				TMP_ASSERT( 0 && "out of memory" );
				result.polygons = (tmp_size_t)polyCount;
				result.vertices = (tmp_index)put;
				return result;
			}
			currentPoly = polyCount++;
			polygons[currentPoly].vertices = &vertices[put];
			size_t start = i;
			ClipVertices* startVertices = currentVertices;
			do {
				if( current->flags & CVF_EXIT ) {
					do {
						i = current->prev;
						current = &currentVertices->data[i];
						current->flags |= CVF_PROCESSED;
						if( put + 1 > size ) {
							TMP_ASSERT( 0 && "out of memory" );
							result.polygons = (tmp_size_t)polyCount;
							result.vertices = (tmp_index)put;
							return result;
						}
						vertices[put++] = current->pos;
					} while( !( current->flags & CVF_INTERSECT ) );
				} else {
					do {
						i = current->next;
						current = &currentVertices->data[i];
						current->flags |= CVF_PROCESSED;
						if( put + 1 > size ) {
							TMP_ASSERT( 0 && "out of memory" );
							result.polygons = (tmp_size_t)polyCount;
							result.vertices = (tmp_index)put;
							return result;
						}
						vertices[put++] = current->pos;
					} while( !( current->flags & CVF_INTERSECT ) );
				}
				ClipVertex* prev = current;
				i = current->neighbor;
				ClipVertices* temp = currentVertices;
				currentVertices = otherVertices;
				otherVertices = temp;
				current = &currentVertices->data[i];
				current->flags |= CVF_PROCESSED;
				TMP_ASSERT( current->flags & CVF_INTERSECT );
				// TMP_ASSERT( ( current->flags & CVF_EXIT ) == ( prev->flags & CVF_EXIT ) );
			} while( i != start || currentVertices != startVertices );
		}
		i = current->next;
		current = &currentVertices->data[i];
	}

	if( !hasIntersections ) {
		// is a completely inside b?
		if( clipPolyPointInsidePoly( b, a->data[0].pos ) ) {
			if( polyCount + 1 > maxPolygons ) {
				TMP_ASSERT( 0 && "out of memory" );
				result.polygons = (tmp_size_t)polyCount;
				result.vertices = (tmp_index)put;
				return result;
			}
			currentPoly = polyCount++;
			polygons[currentPoly].vertices = &vertices[put];
			if( size > a->originalSize ) {
				size = a->originalSize;
			}
			for( size_t i = 0; i < size; ++i ) {
				vertices[i] = a->data[i].pos;
			}
			put = size;
		} else if( b->size && clipPolyPointInsidePoly( a, b->data[0].pos ) ) {
			if( polyCount + 1 > maxPolygons ) {
				TMP_ASSERT( 0 && "out of memory" );
				result.polygons = (tmp_size_t)polyCount;
				result.vertices = (tmp_index)put;
				return result;
			}
			currentPoly = polyCount++;
			polygons[currentPoly].vertices = &vertices[put];
			if( size > b->originalSize ) {
				size = b->originalSize;
			}
			for( size_t i = 0; i < size; ++i ) {
				vertices[i] = b->data[i].pos;
			}
			put = size;
		}
	}
	if( currentPoly < polyCount ) {
		polygons[currentPoly].size =
			( tmp_size_t )( &vertices[put] - polygons[currentPoly].vertices );
	}
	result.polygons = (tmp_size_t)polyCount;
	result.vertices = (tmp_index)put;
	return result;
}

TMP_DEF tmp_size_t clipPolyEmitClippedPolygon( ClipVertices* a, ClipVertices* b,
											   TMP_VECTOR* vertices, tmp_size_t maxCount )
{
	ClipPolygonEntry entry = {};
	clipPolyEmitClippedPolygons( a, b, &entry, 1, vertices, maxCount );
	return entry.size;
}

#ifdef __cplusplus
}
#endif

#endif // TM_POLYGON_IMPLEMENTATION

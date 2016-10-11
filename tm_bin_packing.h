/*
tm_bin_packing.h v1.0.1 - public domain
written by Tolga Mizrak 2016

no warranty; use at your own risk

C99 port of algorithms by Jukka Jylänki, see https://github.com/juj/RectangleBinPack (also public
domain)

USAGE
	This file works as both the header and implementation.
	To implement the interfaces in this header,
		#define TM_BIN_PACKING_IMPLEMENTATION
	in ONE C or C++ source file before #including this header.

NOTES
	Included in this library are multiple variants of the Guillotine and MaxRects algorithms.
	This library is intended as a utility to implement your own BinPacker. It is recommended that
	you write your own init/insert/free functions and use the library functions to implement them,
	instead of calling into these functions directly from your usage code, since the generality of
	the library makes it very verbose otherwise. See SAMPLES for reference.

	All algorithms work on a BinPack structure where packing information are stored.
	BinPack structures can be created in multiple ways:
		- Call binPackCreate and supply an allocator with pointers to allocation functions.
		  This will create a dynamically resizing BinPack structure. Call binPackDestroy to free all
		  allocated memory.
		- Call binPackCreateStatic and supply the memory to be used by the BinPack structure.
		  This will create a static BinPack structure that will not resize dynamically. Use this if
		  you want to control how much memory the bin packing algorithms are allowed to use or if
		  you know how much memory you will need to pack all bins.
		- Fill in a BinPack structure yourself and call binPackInit.

	Once you have a BinPack structure, you can start inserting bins.
	Inserting bins can be done in multiple ways, depending on your use case:
		- Calling one of the GuillotineHeuristicResult/MaxRectsHeuristicResult returning functions
		  directly. These functions use specific heuristics to look for the optimal destination to
		  place a bin. Once the destination is found, a call to guillotineInsert/maxRectsInsert does
		  the actual insertion into the bin pack structure.
		- Calling guillotineInsertChoice/maxRectsInsertChoice. These functions take enum values as
		  arguments to determine what heuristic to use when inserting. They are intended to be used
		  when writing an offline bin packer tool, where you try out different heuristics on the
		  same input to determine the best outcome/packing.
		- Calling guillotineInsertBatch/maxRectsInsertBatch. These functions insert multiple bins in
		  a single call, while searching for the best order of insertion based on the heuristics
		  supplied. This means they offer different results than inserting bins one by one and also
		  can be quite slower than inserting bins in a loop yourself. These functions are also
		  intended to be used when writing an offline bin packer tool.

	Notes on guillotineInsertBatch/maxRectsInsertBatch:
		The order of insertions are very important in the quality of the resulting bin packing.
		These functions will always insert bins in the order of the their heuristic score.
		If you want to use a different insertion order, do not call these functions, as they do not
		insert bins in the order they are supplied.
		To use a different insertion order, sort your inputs yourself in the way you want and then
		call one of the insertion functions yourself in a loop for each bin.

	Notes on what heuristic to use:
		This depends on what your goal is.
		If you need fast runtime bin packing where not all bins are inserted at the same time or not
		known beforehand (like loading textures in a game and placing them on a texture atlas on
		runtime) you should use guillotineRectBestShortSideFit and GuillotineSplitMinimizeArea
		(BSSF-MINAS). The guillotine algorithm has low complexity for inserting bins, but also
		doesn't result in the best quality of packing.

		If you need good quality bin packing or can afford using a slower algorithm, use MaxRects.
		MaxRects is slower when inserting and uses more memory to store free rects (areas with
		no bins), but has better bin packing quality. What heuristics to use also depends on your
		use case, but maxRectsBestShortSideFit (BSSF) is good enough in most cases.

		If you have a list of bins and want to find the best packing the algorithms can result in,
		just try out every combination of heuristics and select the best packing out of those.
		This is very time consuming, but in case of offline packing like when writing a texture
		atlas tool, you can afford the cost of several seconds to come up with a good packing.

		Consult the bin packing survey at https://github.com/juj/RectangleBinPack for more
		information.

	Notes on memory:
		When working with static memory BinPacks, you should know how much memory you will need to
		be able to hold X number of bins. For the Guillotine algorithm, you can use
		guillotineFreeRectsSize to get the size of the freeRects array you need for X number of bins
		( freeRectsSize = 2 * X in this case) and maxRectsFreeRectsSize for the MaxRects algorithm.
		Note that MaxRects uses a lot of memory in the worst case (up to 5 times the number of bins)
		to store free rects, while it uses 2 times the number of bins on average, so it is
		recommended to use dynamic bins for MaxRects instead of static storage. For the static
		storage case you would need to supply enough static memory for the worst case, otherwise
		your program will crash by running out of memory sporadically.

	Do not mix Guillotine and MaxRects algorithms on the same BinPack structure. Stick to one
	heuristic/algorithm combination per BinPack.

	The library is written in a way where no assumptions are made on what data types are used in the
	destination codebase. You can override what datatypes the algorithms use by #defining
	TMBP_OWN_TYPES and supplying your own types by typedefing the same names.
	If you are using a different integer type for tmbp_int, make sure to define TMBP_INT_MAX
	and TMBP_INT_MIN with the right min/max values for that type.
	If you are using signed integers when typedefing tmbp_size_t, note that you need to typedef
	tmbp_usize_t with an unsigned version of the same size. This is because the algorithms cast
	sizes to size_t internally by first casting to tmbp_usize_t, so that sizes are promoted to
	register size integers without sign extension.

SAMPLES
	Writing your own insertion function for specific heuristics depending on your use case:
		BinPackResult myBinPackInsert( BinPack* pack, int width, int height )
		{
			// inserts using GUILLOTINE-BSSF-MINAS with flipping allowed
			BinPackResult ret = {{0}};
			GuillotineHeuristicResult result = guillotineBestShortSideFit( pack, width, height );
			if( guillotineHeuristicIsValidResult( result.score ) ) {
				ret = guillotineInsert( pack, width, height, &result, GuillotineSplitMinimizeArea );
				// optional: merge free rects that share an edge (slow) to improve packing quality
				// guillotineMergeFreeRects( pack );
			}
			return ret;
		}

		BinPackResult myOtherBinPackInsert( BinPack* pack, int width, int height )
		{
			// inserts using MAXRECTS-BSSF with flipping allowed
			BinPackResult ret = {{0}};
			MaxRectsHeuristicResult result = maxRectsBestShortSideFit( pack, width, height );
			if( maxRectsHeuristicIsValidResult( result.scores ) ) {
				ret = maxRectsInsert( pack, width, height, &result );
			}
			return ret;
		}

		BinPackResult myStaticInsert( BinPack* pack, int width, int height )
		{
			// pack is a non resizable bin pack such that it has an upper limit on how many bins can be inserted
			BinPackResult ret = {{0}};
			if( binPackHasSpace( pack ) ) {
				GuillotineHeuristicResult result = guillotineBestShortSideFit( pack, width, height );
				if( guillotineHeuristicIsValidResult( result.score ) ) {
					ret = guillotineInsert( pack, width, height, &result, GuillotineSplitMinimizeArea );
				}
			}
			return ret;
		}

	Resizable bin packing using malloc:
		tmbp_rect* allocateRectArray( void* state, size_t size )
		{
			// we do not need argument state with malloc
			return (tmbp_rect*)malloc( size * sizeof( tmbp_rect ) );
		}
		tmbp_rect* reallocateRectArray( void* state, tmbp_rect* ptr, size_t oldSize, size_t newSize )
		{
			// we do not need arguments state and oldSize with realloc
			return (tmbp_rect*)realloc( ptr, newSize * sizeof( tmbp_rect ) );
		}
		void freeRectArray( void* state, tmbp_rect* ptr, size_t size )
		{
			// we do not need arguments state and size with free
			free( ptr );
		}

		BinPack myBinPackCreate( int width, int height )
		{
			return binPackCreate( width, height, NULL, allocateRectArray, reallocateRectArray );
		}
		void myBinPackDestroy( BinPack* pack )
		{
			binPackDestroy( pack, NULL, freeRectArray );
		}

	Resizable bin packing using new/delete:
		tmbp_rect* allocateRectArray( void* state, size_t size )
		{
			// we do not need argument state with new
			return new tmbp_rect[size];
		}
		tmbp_rect* reallocateRectArray( void* state, tmbp_rect* ptr, size_t oldSize, size_t newSize )
		{
			// we do not need arguments state and oldSize with new/delete
			// there is no native realloc with new/delete in c++
			tmbp_rect* newArray = new tmbp_rect[newSize];
			memcpy( newArray, ptr, oldSize * sizeof( tmbp_rect ) );
			delete[] ptr;
			return newArray;
		}
		void freeRectArray( void* state, tmbp_rect* ptr, size_t size )
		{
			// we do not need arguments state and size with delete[]
			delete[] ptr;
		}

		BinPack myBinPackCreate( int width, int height )
		{
			return binPackCreate( width, height, nullptr, allocateRectArray, reallocateRectArray );
		}
		void myBinPackDestroy( BinPack* pack )
		{
			binPackDestroy( pack, nullptr, freeRectArray );
		}

	Static bin packing (no resizing) using the stack:
		void doBinPackingOnStack()
		{
			tmbp_rect freeRectsMemory[200];
			tmbp_rect usedRectsMemory[200];
			int width = 256;
			int height = 256;
			BinPack pack = binPackCreateStatic( width, height, freeRectsMemory, 200, usedRectsMemory, 200 );
			// do NOT use static arrays freeRectsMemory or usedRectsMemory from this point on directly

			// pack bins here ...
		}

HISTORY
	v1.0.1	11.10.16 fixed a bug in batch insertion functions using the wrong index
	v1.0b   09.10.16 fixed a typo
	v1.0a   07.10.16 removed using forced unsigned arithmetic when tmbp_size_t is signed
	v1.0	20.07.16 initial commit

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.
*/

// define these to avoid crt
#ifdef TM_BIN_PACKING_IMPLEMENTATION
	#ifndef TMBP_ASSERT
		#include <assert.h>
		#define TMBP_ASSERT assert
	#endif
#endif // defined( TM_BIN_PACKING_IMPLEMENTATION )

#ifndef _TM_BIN_PACKING_H_INCLUDED_
#define _TM_BIN_PACKING_H_INCLUDED_

#ifndef TMBP_OWN_TYPES
	#include <limits.h>
	#include <stddef.h> // size_t

	typedef struct {
		int left, top, width, height;
	} tmbp_rect_;
	typedef tmbp_rect_ tmbp_rect;
	typedef size_t tmbp_size_t;
	typedef struct {
		int width, height;
	} tmbp_dim_;
	typedef tmbp_dim_ tmbp_dim;

	typedef int tmbp_int;
	// max value for tmbp_int
	#define TMBP_INT_MAX INT_MAX
	// min value for tmbp_int
	#define TMBP_INT_MIN INT_MIN
#endif

#ifdef __cplusplus
	typedef bool tmbp_bool;
#else
	typedef int tmbp_bool;
#endif

#ifndef TMBP_STATIC
	#define TMBP_DEF extern
#else
	#define TMBP_DEF static
#endif

#define TMBP_INVALID_SCORE TMBP_INT_MAX

#ifdef __cplusplus
extern "C" {
#endif

// function types for taking function pointers to these signatures
// params
//	state: same as what was passed into binPackCreate, useful if you are using a stateful allocator
typedef tmbp_rect* BinPackReallocate( void* state, tmbp_rect* ptr, size_t oldSize, size_t newSize );
typedef tmbp_rect* BinPackAllocate( void* state, size_t size );
typedef void BinPackFree( void* state, tmbp_rect* ptr, size_t size );

typedef struct {
	void* state;
	BinPackReallocate* reallocate;
} BinPackReallocator;

typedef struct {
	tmbp_rect* data;
	tmbp_size_t size;
	tmbp_size_t capacity;
} tmbp_rect_array;

typedef struct {
	tmbp_int width;
	tmbp_int height;
	tmbp_int usedArea;

	tmbp_rect_array freeRects;
	tmbp_rect_array usedRects;

	BinPackReallocator reallocator;

	tmbp_size_t maxFreeRectsSize;
} BinPack;

// Guillotine bin packing algorithms
typedef enum {
	GuillotineRectBestAreaFit,		 // BAF
	GuillotineRectBestShortSideFit,  // BSSF
	GuillotineRectBestLongSideFit	 // BLSF
} GuillotineFreeRectChoiceHeuristic;
typedef enum {
	GuillotineSplitShorterLeftoverAxis,  // SLAS
	GuillotineSplitLongerLeftoverAxis,   // LLAS
	GuillotineSplitMinimizeArea,		 // MINAS
	GuillotineSplitMaximizeArea,		 // MAXAS
	GuillotineSplitShorterAxis,			 // SAS
	GuillotineSplitLongerAxis			 // LAS
} GuillotineSplitHeuristic;

typedef struct {
	tmbp_rect rect;
	tmbp_bool flipped;
	tmbp_bool placed;
} BinPackResult;

typedef struct {
	tmbp_size_t freeRectIndex;
	tmbp_int score;
	tmbp_bool flipped;
} GuillotineHeuristicResult;

typedef struct {
	tmbp_dim dim;
	void* userData;
} BinPackBatchDim;
typedef struct {
	BinPackResult result;
	void* userData;
} BinPackBatchResult;

// creates a BinPack by allocating memory using allocateFunc and storing allocatorState and reallocateFunc into the resulting BinPack for dynamic resizing
// params
//	allocatorState: if you are using a stateful allocator, pass in your allocator state. The state will be passed back into the allocation functions supplied
//	allocateFunc: function pointer to an allocation function. See BinPackAllocate for the function signature
//	reallocateFunc: function pointer to a reallocation function. This is used when dynamically resizing the internal arrays. See BinPackReallocate for the function signature
TMBP_DEF BinPack binPackCreate( tmbp_int width, tmbp_int height, void* allocatorState, BinPackAllocate* allocateFunc, BinPackReallocate* reallocateFunc );

// destroys a dynamic BinPack that was created using binPackCreate using the supplied allocatorState and freeFunc to free memory
// params
//	allocatorState: if you are using a stateful allocator, pass in your allocator state. The state will be passed back into freeFunc
//	freeFunc: function pointer to a free function. See BinPackFree for the function signature
TMBP_DEF void binPackDestroy( BinPack* pack, void* allocatorState, BinPackFree* freeFunc );

// frees unused memory of a dynamic BinPack that was created using binPackCreate
TMBP_DEF void binPackFitToSize( BinPack* pack );

// creates a static (non resizable) BinPack using the memory supplied. Do NOT call binPackDestroy on the returned BinPack
// note that not supplying enough memory to hold the number of bins will result in undefined behavior when inserting
// refer to guillotineFreeRectsSize and maxRectsFreeRectsSize for the minimum size of freeRects to hold X number of bins
// params
//	freeRects: pointer to an array of tmbp_rect to be used as the internal storage for free rects. Must not overlap with usedRects
//	freeRectsCount: number of entries in freeRects
//	usedRects: pointer to an array of tmbp_rect to be used as the internal storage for used rects. Must not overlap with freeRects
//	usedRectsCount: number of entries in usedRects
TMBP_DEF BinPack binPackCreateStatic( tmbp_int width, tmbp_int height, tmbp_rect* freeRects, tmbp_size_t freeRectsCount, tmbp_rect* usedRects, tmbp_size_t usedRectsCount );

// initializes pack for usage. Does not need to be called if binPackCreate or binPackCreateStatic is used to create the BinPack
TMBP_DEF void binPackInit( BinPack* pack, tmbp_int width, tmbp_int height );

// returns 1 if pack has enough space for one more bin, 0 otherwise
tmbp_bool binPackHasSpace( BinPack* pack );

// guillotine heuristics with flipping (swapping width with height when inserting)
TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFit( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFit( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFit( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF GuillotineHeuristicResult guillotineChoice( BinPack* pack, tmbp_int width, tmbp_int height, GuillotineFreeRectChoiceHeuristic freeChoice );

// guillotine heuristics without flipping (without swapping width with height when inserting)
TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFitNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFitNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFitNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF GuillotineHeuristicResult guillotineChoiceNoFlip( BinPack* pack, tmbp_int width, tmbp_int height, GuillotineFreeRectChoiceHeuristic freeChoice );

// returns 1 if score returned by heuristic is valid (bin insertion position was found), 0 otherwise
tmbp_bool guillotineHeuristicIsValidResult( tmbp_int score );

// inserts bin into pack using the result from a heuristic
TMBP_DEF BinPackResult guillotineInsert( BinPack* pack, tmbp_int width, tmbp_int height, const GuillotineHeuristicResult* choice, GuillotineSplitHeuristic splitChoice );

// inserts bin into pack using a heuristic
TMBP_DEF BinPackResult guillotineInsertChoice( BinPack* pack, tmbp_int width, tmbp_int height, GuillotineFreeRectChoiceHeuristic freeChoice, GuillotineSplitHeuristic splitChoice, tmbp_bool canFlip );

// inserts multiple rects into pack at once while consuming rect dimensions from dims
// note that the order of elements in dims will be altered
// this is different than inserting bins into pack in a loop, the bins will be inserted in the order of best to lowest heuristic score
// see NOTES for more information
TMBP_DEF tmbp_size_t guillotineInsertBatch( BinPack* pack, BinPackBatchDim* dims, BinPackBatchResult* results, tmbp_size_t count, GuillotineFreeRectChoiceHeuristic freeChoice, GuillotineSplitHeuristic splitChoice, tmbp_bool canFlip );

// returns a value between 0 and 1 for how much area is occupied in the pack
TMBP_DEF float guillotineOccupancy( const BinPack* pack );

// merges pairs of free rects that share an edge, slow but improves packing quality
// misses groups of rectangles that share an edge, but those cases are very rare
TMBP_DEF void guillotineMergeFreeRects( BinPack* pack );

// returns how big the freeRects array needs to be to hold binCount number of bins
tmbp_size_t guillotineFreeRectsSize( tmbp_size_t binCount );

// MaxRects bin packing algorithms

typedef enum {
	MaxRectsBestShortSideFit,  // BSSF
	MaxRectsBestLongSideFit,   // BLSF
	MaxRectsBestAreaFit,	   // BAF
	MaxRectsBottomLeftRule,	   // BL
	MaxRectsContactPointRule   // CP
} MaxRectsFreeRectChoiceHeuristic;

typedef struct {
	tmbp_size_t freeRectIndex;
	tmbp_int scores[2];
	tmbp_bool flipped;
} MaxRectsHeuristicResult;

// max rects heuristics with flipping allowed (swapping width with height when inserting)
TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFit( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRule( BinPack* pack, tmbp_int width,	tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFit( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFit( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRule( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsChoice( BinPack* pack, tmbp_int width, tmbp_int height, MaxRectsFreeRectChoiceHeuristic freeChoice );

// max rects heuristics without flipping (without swapping width with height when inserting)
TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFitNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRuleNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFitNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFitNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRuleNoFlip( BinPack* pack, tmbp_int width, tmbp_int height );
TMBP_DEF MaxRectsHeuristicResult maxRectsChoiceNoFlip( BinPack* pack, tmbp_int width, tmbp_int height, MaxRectsFreeRectChoiceHeuristic freeChoice );

// returns 1 if score returned by heuristic is valid (bin insertion position was found), 0 otherwise
tmbp_bool maxRectsHeuristicIsValidResult( const tmbp_int score[2] );

// inserts bin into pack using the result from a heuristic
TMBP_DEF BinPackResult maxRectsInsert( BinPack* pack, tmbp_int width, tmbp_int height, const MaxRectsHeuristicResult* choice );

// inserts bin into pack using a heuristic
TMBP_DEF BinPackResult maxRectsInsertChoice( BinPack* pack, tmbp_int width, tmbp_int height, MaxRectsFreeRectChoiceHeuristic freeChoice, tmbp_bool canFlip );

// inserts multiple rects into pack at once while consuming rect dimensions from dims
// note that the order of elements in dims will be altered
// this is different than inserting bins into pack in a loop, the bins will be inserted in the order of best to lowest heuristic score
// see NOTES for more information
TMBP_DEF tmbp_size_t maxRectsInsertBatch( BinPack* pack, BinPackBatchDim* dims, BinPackBatchResult* results, tmbp_size_t count, MaxRectsFreeRectChoiceHeuristic freeChoice, tmbp_bool canFlip );

// returns a value between 0 and 1 for how much area is occupied in the pack
TMBP_DEF float maxRectsOccupancy( const BinPack* pack );

// returns how big the freeRects array needs to be to hold binCount number of bins
tmbp_size_t maxRectsFreeRectsSize( tmbp_size_t binCount );

// inline implementation

inline tmbp_bool binPackHasSpace( BinPack* pack )
{
	return pack->usedRects.size < pack->usedRects.capacity;
}
inline tmbp_bool guillotineHeuristicIsValidResult( tmbp_int score )
{
	return score != TMBP_INVALID_SCORE;
}
inline tmbp_bool maxRectsHeuristicIsValidResult( const tmbp_int scores[2] )
{
	return scores[0] != TMBP_INVALID_SCORE || scores[1] != TMBP_INVALID_SCORE;
}
inline tmbp_size_t guillotineFreeRectsSize( tmbp_size_t binCount ) { return binCount * 2; }
inline tmbp_size_t maxRectsFreeRectsSize( tmbp_size_t binCount ) { return binCount * 5; }

#ifdef __cplusplus
}
#endif

#endif  // _TM_BIN_PACKING_H_INCLUDED_

#ifdef TM_BIN_PACKING_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

static tmbp_rect* tmbp_push( tmbp_rect_array* array, BinPackReallocator* reallocator )
{
	TMBP_ASSERT( array );
	TMBP_ASSERT( array->data );
	TMBP_ASSERT( array->size >= 0 );
	TMBP_ASSERT( array->capacity >= 0 );
	if( array->size >= array->capacity ) {
		if( reallocator->reallocate ) {
			array->data = reallocator->reallocate( reallocator->state, array->data, array->capacity,
			                                       array->capacity * 2 );
			array->capacity *= 2;
		} else {
			TMBP_ASSERT( 0 && "OutOfMemory" );
		}
	}

	return &array->data[array->size++];
}
/*static void tmbp_pop( tmbp_rect_array* array )
{
    TMBP_ASSERT( array );
    TMBP_ASSERT( array->data );
    TMBP_ASSERT( array->size > 0 );
    TMBP_ASSERT( array->size <= array->capacity );
    --array->size;
}*/
static void tmbp_erase( tmbp_rect_array* array, tmbp_size_t index )
{
	TMBP_ASSERT( array );
	TMBP_ASSERT( array->data );
	TMBP_ASSERT( array->size > 0 );
	TMBP_ASSERT( array->size <= array->capacity );
	TMBP_ASSERT( index >= 0 && index < array->size );
	// we assign entry at index to the last element and decrement size
	--array->size;
	if( index != array->size ) {
		array->data[index] = array->data[array->size];
	}
}
static void tmbp_clear( tmbp_rect_array* array )
{
	TMBP_ASSERT( array );
	TMBP_ASSERT( array->data );
	array->size = 0;
}

TMBP_DEF BinPack binPackCreate( tmbp_int width, tmbp_int height, void* allocatorState,
                                BinPackAllocate* allocateFunc, BinPackReallocate* reallocateFunc )
{
	BinPack result;
	tmbp_size_t rectsCount    = 16;
	result.freeRects.data     = allocateFunc( allocatorState, rectsCount );
	result.freeRects.size     = 0;
	result.freeRects.capacity = rectsCount;

	result.usedRects.data     = allocateFunc( allocatorState, rectsCount );
	result.usedRects.size     = 0;
	result.usedRects.capacity = rectsCount;

	result.reallocator.state      = allocatorState;
	result.reallocator.reallocate = reallocateFunc;

	binPackInit( &result, width, height );
	return result;
}
TMBP_DEF void binPackDestroy( BinPack* pack, void* allocatorState, BinPackFree* freeFunc )
{
	freeFunc( allocatorState, pack->freeRects.data, pack->freeRects.size );
	pack->freeRects.data     = NULL;
	pack->freeRects.size     = 0;
	pack->freeRects.capacity = 0;
	freeFunc( allocatorState, pack->usedRects.data, pack->usedRects.size );
	pack->usedRects.data     = NULL;
	pack->usedRects.size     = 0;
	pack->usedRects.capacity = 0;
}
TMBP_DEF void binPackFitToSize( BinPack* pack )
{
	pack->freeRects.data =
	    pack->reallocator.reallocate( pack->reallocator.state, pack->freeRects.data,
	                                  pack->freeRects.capacity, pack->freeRects.size );
	pack->freeRects.capacity = pack->freeRects.size;
	pack->usedRects.data =
	    pack->reallocator.reallocate( pack->reallocator.state, pack->usedRects.data,
	                                  pack->usedRects.capacity, pack->usedRects.size );
	pack->usedRects.capacity = pack->usedRects.size;
}
TMBP_DEF BinPack binPackCreateStatic( tmbp_int width, tmbp_int height, tmbp_rect* freeRects,
                                      tmbp_size_t freeRectsCount, tmbp_rect* usedRects,
                                      tmbp_size_t usedRectsCount )
{
	// assert that arrays do not overlap
	TMBP_ASSERT( freeRects >= usedRects + usedRectsCount
	             || freeRects + freeRectsCount <= usedRects );

	BinPack result;
	result.freeRects.data     = freeRects;
	result.freeRects.size     = 0;
	result.freeRects.capacity = freeRectsCount;

	result.usedRects.data     = usedRects;
	result.usedRects.size     = 0;
	result.usedRects.capacity = usedRectsCount;

	result.reallocator.state      = NULL;
	result.reallocator.reallocate = NULL;
	binPackInit( &result, width, height );
	return result;
}
TMBP_DEF void binPackInit( BinPack* pack, tmbp_int width, tmbp_int height )
{
	TMBP_ASSERT( pack );

	pack->width    = width;
	pack->height   = height;
	pack->usedArea = 0;

	tmbp_clear( &pack->usedRects );

	tmbp_clear( &pack->freeRects );
	tmbp_rect* added = tmbp_push( &pack->freeRects, &pack->reallocator );
	added->left      = 0;
	added->top       = 0;
	added->width     = width;
	added->height    = height;
}

inline static tmbp_int tmbp_abs( tmbp_int val ) { return ( val < 0 ) ? ( -val ) : ( val ); }
#define tmbp_min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define tmbp_max( a, b ) ( ( ( a ) < ( b ) ) ? ( b ) : ( a ) )

static tmbp_int guillotineScoreBestAreaFit( tmbp_int width, tmbp_int height,
                                            const tmbp_rect* freeRect )
{
	return freeRect->width * freeRect->height - width * height;
}
static tmbp_int guillotineScoreBestShortSideFit( tmbp_int width, tmbp_int height,
                                                 const tmbp_rect* freeRect )
{
	tmbp_int leftoverHoriz = tmbp_abs( freeRect->width - width );
	tmbp_int leftoverVert  = tmbp_abs( freeRect->height - height );
	tmbp_int leftover      = tmbp_min( leftoverHoriz, leftoverVert );
	return leftover;
}
static tmbp_int guillotineScoreBestLongSideFit( tmbp_int width, tmbp_int height,
                                                const tmbp_rect* freeRect )
{
	tmbp_int leftoverHoriz = tmbp_abs( freeRect->width - width );
	tmbp_int leftoverVert  = tmbp_abs( freeRect->height - height );
	tmbp_int leftover      = tmbp_max( leftoverHoriz, leftoverVert );
	return leftover;
}

TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFit( BinPack* pack, tmbp_int width,
                                                          tmbp_int height )
{
	GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

	tmbp_size_t count = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < count; ++i ) {
		tmbp_rect* current = &pack->freeRects.data[i];
		if( width == current->width && height == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 0;
			break;
		} else if( height == current->width && width == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 1;
			break;
		} else if( width <= current->width && height <= current->height ) {
			tmbp_int score = guillotineScoreBestAreaFit( width, height, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 0;
			}
		} else if( height <= current->width && width <= current->height ) {
			tmbp_int score = guillotineScoreBestAreaFit( height, width, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 1;
			}
		}
	}
	return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFit( BinPack* pack, tmbp_int width,
                                                               tmbp_int height )
{
	GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

	tmbp_size_t count = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < count; ++i ) {
		tmbp_rect* current = &pack->freeRects.data[i];
		if( width == current->width && height == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 0;
			break;
		} else if( height == current->width && width == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 1;
			break;
		} else if( width <= current->width && height <= current->height ) {
			tmbp_int score = guillotineScoreBestShortSideFit( width, height, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 0;
			}
		} else if( height <= current->width && width <= current->height ) {
			tmbp_int score = guillotineScoreBestShortSideFit( height, width, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 1;
			}
		}
	}
	return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFit( BinPack* pack, tmbp_int width,
                                                              tmbp_int height )
{
	GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

	tmbp_size_t count = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < count; ++i ) {
		tmbp_rect* current = &pack->freeRects.data[i];
		if( width == current->width && height == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 0;
			break;
		} else if( height == current->width && width == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 1;
			break;
		} else if( width <= current->width && height <= current->height ) {
			tmbp_int score = guillotineScoreBestLongSideFit( width, height, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 0;
			}
		} else if( height <= current->width && width <= current->height ) {
			tmbp_int score = guillotineScoreBestLongSideFit( height, width, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 1;
			}
		}
	}
	return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineChoice( BinPack* pack, tmbp_int width, tmbp_int height,
                                                     GuillotineFreeRectChoiceHeuristic freeChoice )
{
	GuillotineHeuristicResult result;
	switch( freeChoice ) {
		case GuillotineRectBestAreaFit: {
			result = guillotineBestAreaFit( pack, width, height );
			break;
		}
		case GuillotineRectBestShortSideFit: {
			result = guillotineBestShortSideFit( pack, width, height );
			break;
		}
		case GuillotineRectBestLongSideFit: {
			result = guillotineBestLongSideFit( pack, width, height );
			break;
		}
		default: {
			TMBP_ASSERT( 0 );
			result.freeRectIndex = 0;
			result.score         = TMBP_INVALID_SCORE;
			break;
		}
	}
	return result;
}

TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFitNoFlip( BinPack* pack, tmbp_int width,
                                                                tmbp_int height )
{
	GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

	tmbp_size_t count = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < count; ++i ) {
		tmbp_rect* current = &pack->freeRects.data[i];
		if( width == current->width && height == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 0;
			break;
		} else if( width <= current->width && height <= current->height ) {
			tmbp_int score = guillotineScoreBestAreaFit( width, height, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 0;
			}
		}
	}
	return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFitNoFlip( BinPack* pack, tmbp_int width,
                                                                     tmbp_int height )
{
	GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

	tmbp_size_t count = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < count; ++i ) {
		tmbp_rect* current = &pack->freeRects.data[i];
		if( width == current->width && height == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 0;
			break;
		} else if( width <= current->width && height <= current->height ) {
			tmbp_int score = guillotineScoreBestShortSideFit( width, height, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 0;
			}
		}
	}
	return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFitNoFlip( BinPack* pack, tmbp_int width,
                                                                    tmbp_int height )
{
	GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

	tmbp_size_t count = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < count; ++i ) {
		tmbp_rect* current = &pack->freeRects.data[i];
		if( width == current->width && height == current->height ) {
			result.freeRectIndex = i;
			result.score         = TMBP_INT_MIN;
			result.flipped       = 0;
			break;
		} else if( width <= current->width && height <= current->height ) {
			tmbp_int score = guillotineScoreBestLongSideFit( width, height, current );
			if( score < result.score ) {
				result.freeRectIndex = i;
				result.score         = score;
				result.flipped       = 0;
			}
		}
	}
	return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineChoiceNoFlip(
    BinPack* pack, tmbp_int width, tmbp_int height, GuillotineFreeRectChoiceHeuristic freeChoice )
{
	GuillotineHeuristicResult result;
	switch( freeChoice ) {
		case GuillotineRectBestAreaFit: {
			result = guillotineBestAreaFitNoFlip( pack, width, height );
			break;
		}
		case GuillotineRectBestShortSideFit: {
			result = guillotineBestShortSideFitNoFlip( pack, width, height );
			break;
		}
		case GuillotineRectBestLongSideFit: {
			result = guillotineBestLongSideFitNoFlip( pack, width, height );
			break;
		}
		default: {
			TMBP_ASSERT( 0 );
			result.freeRectIndex = 0;
			result.score         = TMBP_INVALID_SCORE;
			break;
		}
	}
	return result;
}

static tmbp_rect binPackRectFromPosition( BinPack* pack, tmbp_int width, tmbp_int height,
                                          tmbp_size_t index, tmbp_bool flipped )
{
	tmbp_rect rect     = {0};
	tmbp_rect* current = &pack->freeRects.data[index];
	rect.left          = current->left;
	rect.top           = current->top;
	if( flipped ) {
		rect.width  = height;
		rect.height = width;
	} else {
		rect.width  = width;
		rect.height = height;
	}
	return rect;
}

static void guillotineSplitFreeRectByHeuristic( tmbp_rect_array* freeRects,
                                                BinPackReallocator* reallocator,
                                                const tmbp_rect* freeRect,
                                                const tmbp_rect* placedRect,
                                                GuillotineSplitHeuristic heuristic )
{
	tmbp_int w = freeRect->width - placedRect->width;
	tmbp_int h = freeRect->height - placedRect->height;

	int splitHorizontal = 1;
	switch( heuristic ) {
		case GuillotineSplitShorterLeftoverAxis: {
			splitHorizontal = ( w <= h );
			break;
		}
		case GuillotineSplitLongerLeftoverAxis: {
			splitHorizontal = ( w > h );
			break;
		}
		case GuillotineSplitMinimizeArea: {
			splitHorizontal = ( placedRect->width * h > w * placedRect->height );
			break;
		}
		case GuillotineSplitMaximizeArea: {
			splitHorizontal = ( placedRect->width * h <= w * placedRect->height );
			break;
		}
		case GuillotineSplitShorterAxis: {
			splitHorizontal = ( freeRect->width <= freeRect->height );
			break;
		}
		case GuillotineSplitLongerAxis: {
			splitHorizontal = ( freeRect->width > freeRect->height );
			break;
		}
		default: {
			TMBP_ASSERT( 0 );
			break;
		}
	}

	tmbp_rect bottom;
	bottom.left   = freeRect->left;
	bottom.top    = freeRect->top + placedRect->height;
	bottom.height = h;
	tmbp_rect right;
	right.left  = freeRect->left + placedRect->width;
	right.top   = freeRect->top;
	right.width = w;
	if( splitHorizontal ) {
		bottom.width = freeRect->width;
		right.height = placedRect->height;
	} else {
		bottom.width = placedRect->width;
		right.height = freeRect->height;
	}
	if( bottom.width > 0 && bottom.height > 0 ) {
		*tmbp_push( freeRects, reallocator ) = bottom;
	}
	if( right.width > 0 && right.height > 0 ) {
		*tmbp_push( freeRects, reallocator ) = right;
	}
}

TMBP_DEF BinPackResult guillotineInsert( BinPack* pack, tmbp_int width, tmbp_int height,
                                         const GuillotineHeuristicResult* choice,
                                         GuillotineSplitHeuristic splitChoice )
{
	TMBP_ASSERT( choice );
	TMBP_ASSERT( choice->score != TMBP_INVALID_SCORE );

	BinPackResult ret;
	ret.rect =
	    binPackRectFromPosition( pack, width, height, choice->freeRectIndex, choice->flipped );
	ret.flipped        = choice->flipped;
	ret.placed         = 1;
	tmbp_rect freeRect = pack->freeRects.data[choice->freeRectIndex];
	tmbp_erase( &pack->freeRects, choice->freeRectIndex );
	guillotineSplitFreeRectByHeuristic( &pack->freeRects, &pack->reallocator, &freeRect, &ret.rect,
	                                    splitChoice );
	*tmbp_push( &pack->usedRects, &pack->reallocator ) = ret.rect;
	pack->usedArea += ret.rect.width * ret.rect.height;
	return ret;
}
TMBP_DEF BinPackResult guillotineInsertChoice( BinPack* pack, tmbp_int width, tmbp_int height,
                                               GuillotineFreeRectChoiceHeuristic freeChoice,
                                               GuillotineSplitHeuristic splitChoice,
                                               tmbp_bool canFlip )
{
	BinPackResult ret = {{0}};
	GuillotineHeuristicResult result;
	if( canFlip ) {
		result = guillotineChoice( pack, width, height, freeChoice );
	} else {
		result = guillotineChoiceNoFlip( pack, width, height, freeChoice );
	}
	if( guillotineHeuristicIsValidResult( result.score ) ) {
		ret = guillotineInsert( pack, width, height, &result, splitChoice );
	}
	return ret;
}
TMBP_DEF tmbp_size_t guillotineInsertBatch( BinPack* pack, BinPackBatchDim* dims,
                                            BinPackBatchResult* results, tmbp_size_t count,
                                            GuillotineFreeRectChoiceHeuristic freeChoice,
                                            GuillotineSplitHeuristic splitChoice,
                                            tmbp_bool canFlip )
{
	tmbp_size_t dimsCount          = count;
	GuillotineHeuristicResult best = {0};
	while( dimsCount > 0 ) {
		best.score         = TMBP_INVALID_SCORE;
		tmbp_int bestIndex = -1;
		for( tmbp_size_t i = 0; i < dimsCount; ++i ) {
			tmbp_int w = dims[i].dim.width;
			tmbp_int h = dims[i].dim.height;
			GuillotineHeuristicResult result;
			if( canFlip ) {
				result = guillotineChoice( pack, w, h, freeChoice );
			} else {
				result = guillotineChoiceNoFlip( pack, w, h, freeChoice );
			}
			if( result.score < best.score ) {
				best      = result;
				bestIndex = i;
				if( result.score == TMBP_INT_MIN ) {
					break;
				}
			}
		}
		if( best.score == TMBP_INVALID_SCORE ) {
			// we didn't find any matches
			break;
		}

		BinPackBatchResult* result = results++;
		result->result             = guillotineInsert( pack, dims[bestIndex].dim.width,
		                                   dims[bestIndex].dim.height, &best, splitChoice );
		result->userData = dims[bestIndex].userData;

		// we processed bestIndex already, move it to the back of the array and decrease dimsCount
		--dimsCount;
		if( bestIndex != dimsCount ) {
			BinPackBatchDim tmp = dims[bestIndex];
			dims[bestIndex]     = dims[dimsCount];
			dims[dimsCount]     = tmp;
		}
	}
	return ( tmbp_size_t )( count - dimsCount );
}
TMBP_DEF float guillotineOccupancy( const BinPack* pack )
{
	return (float)pack->usedArea / ( pack->width * pack->height );
}
TMBP_DEF void guillotineMergeFreeRects( BinPack* pack )
{
	// merge pairs of free rects into one if able
	// since we are only looking at pairs, we will miss chances to merge three rectangles into one
	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* a = &freeRects[i];
		for( tmbp_size_t j = i + 1; j < freeRectsCount; ) {
			tmbp_rect* b = &freeRects[j];
			if( a->left == b->left && a->width == b->width ) {
				if( a->top == b->top + b->height ) {
					a->top -= b->height;
					a->height += b->height;
					tmbp_erase( &pack->freeRects, j );
					--freeRectsCount;
					continue;
				} else if( a->top + a->height == b->top ) {
					a->height += b->height;
					tmbp_erase( &pack->freeRects, j );
					--freeRectsCount;
					continue;
				}
			} else if( a->top == b->top && a->height == b->height ) {
				if( a->left == b->left + b->width ) {
					a->left -= b->width;
					a->width += b->width;
					tmbp_erase( &pack->freeRects, j );
					--freeRectsCount;
					continue;
				} else if( a->left + a->width == b->left ) {
					a->width += b->width;
					tmbp_erase( &pack->freeRects, j );
					--freeRectsCount;
					continue;
				}
			}
			++j;
		}
	}
}

// MaxRects
static tmbp_int maxRectsCommonIntervalLength( tmbp_int aStart, tmbp_int aEnd, tmbp_int bStart,
                                              tmbp_int bEnd )
{
	if( aEnd < bStart || bEnd < aStart ) {
		return 0;
	}
	return tmbp_min( aEnd, bEnd ) - tmbp_max( aStart, bStart );
}
static tmbp_int maxRectsContactPointScore( BinPack* pack, tmbp_int left, tmbp_int top,
                                           tmbp_int right, tmbp_int bottom )
{
	tmbp_int score = 0;
	if( left == 0 || right == pack->width ) {
		score += bottom - top;
	}
	if( top == 0 || bottom == pack->height ) {
		score += right - left;
	}

	tmbp_rect* usedRects       = pack->usedRects.data;
	tmbp_size_t usedRectsCount = pack->usedRects.size;
	for( tmbp_size_t i = 0; i < usedRectsCount; ++i ) {
		tmbp_rect* current = &usedRects[i];
		if( current->left == right || current->left + current->width == left ) {
			score += maxRectsCommonIntervalLength( current->top, current->top + current->height,
			                                       top, bottom );
		}
		if( current->top == bottom || current->top + current->height == top ) {
			score += maxRectsCommonIntervalLength( current->left, current->left + current->width,
			                                       left, right );
		}
	}
	return score;
}

TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFit( BinPack* pack, tmbp_int width,
                                                           tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int leftoverHoriz = current->width - width;
			tmbp_int leftoverVert  = current->height - height;
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );
			tmbp_int longSideFit   = tmbp_max( leftoverHoriz, leftoverVert );

			if( shortSideFit < result.scores[0]
			    || ( shortSideFit == result.scores[0] && longSideFit < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = shortSideFit;
				result.scores[1]     = longSideFit;
				result.flipped       = 0;
			}
		}

		if( height <= current->width && width <= current->height ) {
			tmbp_int leftoverHoriz = current->width - height;
			tmbp_int leftoverVert  = current->height - width;
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );
			tmbp_int longSideFit   = tmbp_max( leftoverHoriz, leftoverVert );

			if( shortSideFit < result.scores[0]
			    || ( shortSideFit == result.scores[0] && longSideFit < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = shortSideFit;
				result.scores[1]     = longSideFit;
				result.flipped       = 1;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRule( BinPack* pack, tmbp_int width,
                                                         tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int topSideY = current->top + height;
			if( topSideY < result.scores[0]
			    || ( topSideY == result.scores[0] && current->left < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = topSideY;
				result.scores[1]     = current->left;
				result.flipped       = 0;
			}
		}

		if( height <= current->width && width <= current->height ) {
			tmbp_int topSideY = current->top + width;
			if( topSideY < result.scores[0]
			    || ( topSideY == result.scores[0] && current->left < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = topSideY;
				result.scores[1]     = current->left;
				result.flipped       = 1;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFit( BinPack* pack, tmbp_int width,
                                                          tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - width );
			tmbp_int leftoverVert  = tmbp_abs( current->height - height );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );
			tmbp_int longSideFit   = tmbp_max( leftoverHoriz, leftoverVert );

			if( longSideFit < result.scores[1]
			    || ( longSideFit == result.scores[1] && shortSideFit < result.scores[0] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = shortSideFit;
				result.scores[1]     = longSideFit;
				result.flipped       = 0;
			}
		}

		if( height <= current->width && width <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - height );
			tmbp_int leftoverVert  = tmbp_abs( current->height - width );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );
			tmbp_int longSideFit   = tmbp_max( leftoverHoriz, leftoverVert );

			if( longSideFit < result.scores[1]
			    || ( longSideFit == result.scores[1] && shortSideFit < result.scores[0] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = shortSideFit;
				result.scores[1]     = longSideFit;
				result.flipped       = 1;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFit( BinPack* pack, tmbp_int width,
                                                      tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		tmbp_int areaFit   = current->width * current->height - width * height;

		if( width <= current->width && height <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - width );
			tmbp_int leftoverVert  = tmbp_abs( current->height - height );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );

			if( areaFit < result.scores[0]
			    || ( areaFit == result.scores[0] && shortSideFit < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = areaFit;
				result.scores[1]     = shortSideFit;
				result.flipped       = 0;
			}
		}

		if( height <= current->width && width <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - height );
			tmbp_int leftoverVert  = tmbp_abs( current->height - width );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );

			if( areaFit < result.scores[0]
			    || ( areaFit == result.scores[0] && shortSideFit < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = areaFit;
				result.scores[1]     = shortSideFit;
				result.flipped       = 1;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRule( BinPack* pack, tmbp_int width,
                                                           tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int score = -maxRectsContactPointScore(
			    pack, current->left, current->top, current->left + width, current->top + height );
			if( score < result.scores[0] ) {
				result.freeRectIndex = i;
				result.scores[0]     = score;
				result.flipped       = 0;
			}
		}

		if( height <= current->width && width <= current->height ) {
			tmbp_int score = -maxRectsContactPointScore(
			    pack, current->left, current->top, current->left + height, current->top + width );
			if( score < result.scores[0] ) {
				result.freeRectIndex = i;
				result.scores[0]     = score;
				result.flipped       = 1;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsChoice( BinPack* pack, tmbp_int width, tmbp_int height,
                                                 MaxRectsFreeRectChoiceHeuristic freeChoice )
{
	MaxRectsHeuristicResult result;
	switch( freeChoice ) {
		case MaxRectsBestShortSideFit: {
			result = maxRectsBestShortSideFit( pack, width, height );
			break;
		}
		case MaxRectsBottomLeftRule: {
			result = maxRectsBottomLeftRule( pack, width, height );
			break;
		}
		case MaxRectsContactPointRule: {
			result = maxRectsContactPointRule( pack, width, height );
			break;
		}
		case MaxRectsBestLongSideFit: {
			result = maxRectsBestLongSideFit( pack, width, height );
			break;
		}
		case MaxRectsBestAreaFit: {
			result = maxRectsBestAreaFit( pack, width, height );
			break;
		}
		default: {
			TMBP_ASSERT( 0 );
			result.scores[0] = TMBP_INVALID_SCORE;
			result.scores[1] = TMBP_INVALID_SCORE;
			break;
		}
	}
	return result;
}

// no flip variants
TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFitNoFlip( BinPack* pack, tmbp_int width,
                                                                 tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - width );
			tmbp_int leftoverVert  = tmbp_abs( current->height - height );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );
			tmbp_int longSideFit   = tmbp_max( leftoverHoriz, leftoverVert );

			if( shortSideFit < result.scores[0]
			    || ( shortSideFit == result.scores[0] && longSideFit < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = shortSideFit;
				result.scores[1]     = longSideFit;
				result.flipped       = 0;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRuleNoFlip( BinPack* pack, tmbp_int width,
                                                               tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int topSideY = current->top + height;
			if( topSideY < result.scores[0]
			    || ( topSideY == result.scores[0] && current->left < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = topSideY;
				result.scores[1]     = current->left;
				result.flipped       = 0;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFitNoFlip( BinPack* pack, tmbp_int width,
                                                                tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - width );
			tmbp_int leftoverVert  = tmbp_abs( current->height - height );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );
			tmbp_int longSideFit   = tmbp_max( leftoverHoriz, leftoverVert );

			if( longSideFit < result.scores[1]
			    || ( longSideFit == result.scores[1] && shortSideFit < result.scores[0] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = shortSideFit;
				result.scores[1]     = longSideFit;
				result.flipped       = 0;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFitNoFlip( BinPack* pack, tmbp_int width,
                                                            tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		tmbp_int areaFit   = current->width * current->height - width * height;

		if( width <= current->width && height <= current->height ) {
			tmbp_int leftoverHoriz = tmbp_abs( current->width - width );
			tmbp_int leftoverVert  = tmbp_abs( current->height - height );
			tmbp_int shortSideFit  = tmbp_min( leftoverHoriz, leftoverVert );

			if( areaFit < result.scores[0]
			    || ( areaFit == result.scores[0] && shortSideFit < result.scores[1] ) ) {
				result.freeRectIndex = i;
				result.scores[0]     = areaFit;
				result.scores[1]     = shortSideFit;
				result.flipped       = 0;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRuleNoFlip( BinPack* pack, tmbp_int width,
                                                                 tmbp_int height )
{
	MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* current = &freeRects[i];
		if( width <= current->width && height <= current->height ) {
			tmbp_int score = -maxRectsContactPointScore(
			    pack, current->left, current->top, current->left + width, current->top + height );
			if( score < result.scores[0] ) {
				result.freeRectIndex = i;
				result.scores[0]     = score;
				result.flipped       = 0;
			}
		}
	}

	return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsChoiceNoFlip( BinPack* pack, tmbp_int width,
                                                       tmbp_int height,
                                                       MaxRectsFreeRectChoiceHeuristic freeChoice )
{
	MaxRectsHeuristicResult result;
	switch( freeChoice ) {
		case MaxRectsBestShortSideFit: {
			result = maxRectsBestShortSideFitNoFlip( pack, width, height );
			break;
		}
		case MaxRectsBottomLeftRule: {
			result = maxRectsBottomLeftRuleNoFlip( pack, width, height );
			break;
		}
		case MaxRectsContactPointRule: {
			result = maxRectsContactPointRuleNoFlip( pack, width, height );
			break;
		}
		case MaxRectsBestLongSideFit: {
			result = maxRectsBestLongSideFitNoFlip( pack, width, height );
			break;
		}
		case MaxRectsBestAreaFit: {
			result = maxRectsBestAreaFitNoFlip( pack, width, height );
			break;
		}
		default: {
			TMBP_ASSERT( 0 );
			result.scores[0] = TMBP_INVALID_SCORE;
			result.scores[1] = TMBP_INVALID_SCORE;
			break;
		}
	}
	return result;
}

static tmbp_bool maxRectsSplitFreeNode( BinPack* pack, tmbp_rect freeNode,
                                        const tmbp_rect* usedNode )
{
	tmbp_int freeNodeRight  = freeNode.left + freeNode.width;
	tmbp_int freeNodeBottom = freeNode.top + freeNode.height;
	tmbp_int usedNodeRight  = usedNode->left + usedNode->width;
	tmbp_int usedNodeBottom = usedNode->top + usedNode->height;
	if( usedNode->left >= freeNodeRight || usedNodeRight <= freeNode.left
	    || usedNode->top >= freeNodeBottom || usedNodeBottom <= freeNode.top ) {
		return 0;
	}

	if( usedNode->left < freeNodeRight && usedNodeRight > freeNode.left ) {
		if( usedNode->top > freeNode.top && usedNode->top < freeNodeBottom ) {
			tmbp_rect* added = tmbp_push( &pack->freeRects, &pack->reallocator );
			*added           = freeNode;
			added->height    = usedNode->top - freeNode.top;
		}

		if( usedNodeBottom < freeNodeBottom ) {
			tmbp_rect* added = tmbp_push( &pack->freeRects, &pack->reallocator );
			*added           = freeNode;
			added->top       = usedNodeBottom;
			added->height    = freeNodeBottom - usedNodeBottom;
		}
	}

	if( usedNode->top < freeNodeBottom && usedNodeBottom > freeNode.top ) {
		if( usedNode->left > freeNode.left && usedNode->left < freeNodeRight ) {
			tmbp_rect* added = tmbp_push( &pack->freeRects, &pack->reallocator );
			*added           = freeNode;
			added->width     = usedNode->left - freeNode.left;
		}

		if( usedNodeRight < freeNodeRight ) {
			tmbp_rect* added = tmbp_push( &pack->freeRects, &pack->reallocator );
			*added           = freeNode;
			added->left      = usedNodeRight;
			added->width     = freeNodeRight - usedNodeRight;
		}
	}
	return 1;
}

static tmbp_bool maxRectsIsContainedIn( const tmbp_rect* a, const tmbp_rect* b )
{
	return a->left >= b->left && a->top >= b->top && a->left + a->width <= b->left + b->width
	       && a->top + a->height <= b->top + b->height;
}
static void maxRectsPruneFreeRects( BinPack* pack )
{
	TMBP_ASSERT( pack );

	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	for( tmbp_size_t i = 0; i < freeRectsCount; ++i ) {
		tmbp_rect* a = &freeRects[i];
		for( tmbp_size_t j = i + 1; j < freeRectsCount; ) {
			tmbp_rect* b = &freeRects[j];
			if( maxRectsIsContainedIn( a, b ) ) {
				tmbp_erase( &pack->freeRects, i );
				--i;
				--freeRectsCount;
				break;
			}
			if( maxRectsIsContainedIn( b, a ) ) {
				tmbp_erase( &pack->freeRects, j );
				--freeRectsCount;
				continue;
			}
			++j;
		}
	}
}

TMBP_DEF BinPackResult maxRectsInsert( BinPack* pack, tmbp_int width, tmbp_int height,
                                       const MaxRectsHeuristicResult* choice )
{
	TMBP_ASSERT( choice );
	TMBP_ASSERT( maxRectsHeuristicIsValidResult( choice->scores ) );
	BinPackResult ret;
	ret.rect =
	    binPackRectFromPosition( pack, width, height, choice->freeRectIndex, choice->flipped );
	ret.flipped                = choice->flipped;
	ret.placed                 = 1;
	tmbp_rect* freeRects       = pack->freeRects.data;
	tmbp_size_t freeRectsCount = pack->freeRects.size;
	pack->maxFreeRectsSize     = 0;
	for( tmbp_size_t i = 0; i < freeRectsCount; ) {
		if( maxRectsSplitFreeNode( pack, freeRects[i], &ret.rect ) ) {
			if( pack->freeRects.size > pack->maxFreeRectsSize ) {
				pack->maxFreeRectsSize = pack->freeRects.size;
			}
			tmbp_erase( &pack->freeRects, i );
			freeRectsCount = pack->freeRects.size;
			continue;
		}
		++i;
	}
	maxRectsPruneFreeRects( pack );
	*tmbp_push( &pack->usedRects, &pack->reallocator ) = ret.rect;
	pack->usedArea += ret.rect.width * ret.rect.height;
	return ret;
}

TMBP_DEF BinPackResult maxRectsInsertChoice( BinPack* pack, tmbp_int width, tmbp_int height,
                                             MaxRectsFreeRectChoiceHeuristic freeChoice,
                                             tmbp_bool canFlip )
{
	BinPackResult ret = {{0}};
	MaxRectsHeuristicResult result;
	if( canFlip ) {
		result = maxRectsChoice( pack, width, height, freeChoice );
	} else {
		result = maxRectsChoiceNoFlip( pack, width, height, freeChoice );
	}

	if( maxRectsHeuristicIsValidResult( result.scores ) ) {
		ret = maxRectsInsert( pack, width, height, &result );
	}

	return ret;
}
TMBP_DEF tmbp_size_t maxRectsInsertBatch( BinPack* pack, BinPackBatchDim* dims,
                                          BinPackBatchResult* results, tmbp_size_t count,
                                          MaxRectsFreeRectChoiceHeuristic freeChoice,
                                          tmbp_bool canFlip )
{
	tmbp_size_t dimsCount = count;
	while( dimsCount > 0 ) {
		MaxRectsHeuristicResult best = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}};
		tmbp_int bestIndex           = -1;
		for( tmbp_size_t i = 0; i < dimsCount; ++i ) {
			MaxRectsHeuristicResult result;
			tmbp_int w = dims[i].dim.width;
			tmbp_int h = dims[i].dim.height;
			if( canFlip ) {
				result = maxRectsChoice( pack, w, h, freeChoice );
			} else {
				result = maxRectsChoiceNoFlip( pack, w, h, freeChoice );
			}
			if( result.scores[0] < best.scores[0]
			    || ( result.scores[0] == best.scores[0] && result.scores[1] < best.scores[1] ) ) {
				best      = result;
				bestIndex = i;
			}
		}

		if( !maxRectsHeuristicIsValidResult( best.scores ) ) {
			break;
		}
		BinPackBatchResult* result = results++;
		result->result =
		    maxRectsInsert( pack, dims[bestIndex].dim.width, dims[bestIndex].dim.height, &best );
		result->userData = dims[bestIndex].userData;

		// we processed bestIndex already, move it to the back of the array and decrease dimsCount
		--dimsCount;
		if( bestIndex != dimsCount ) {
			BinPackBatchDim tmp = dims[bestIndex];
			dims[bestIndex]     = dims[dimsCount];
			dims[dimsCount]     = tmp;
		}
	}
	return ( tmbp_size_t )( count - dimsCount );
}
TMBP_DEF float maxRectsOccupancy( const BinPack* pack ) { return guillotineOccupancy( pack ); }

#ifdef __cplusplus
}
#endif

#endif  // defined( TM_BIN_PACKING_IMPLEMENTATION )

/*
tm_bin_packing.h v1.0.7 - public domain - https://github.com/to-miz/tm
written by Tolga Mizrak 2016

no warranty; use at your own risk

C99 port of algorithms by Jukka Jylänki, see https://github.com/juj/RectangleBinPack (also public
domain)

LICENSE
    see license notes at end of file

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
        runtime) you should use guillotineBestShortSideFit and GuillotineSplitMinimizeArea
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

SAMPLES
    Writing your own insertion function for specific heuristics depending on your use case:
        BinPackResult myBinPackInsert(BinPack* pack, int width, int height) {
            // Inserts using GUILLOTINE-BSSF-MINAS with flipping allowed.
            BinPackResult ret = {0};
            GuillotineHeuristicResult result = guillotineBestShortSideFit(pack, width, height);
            if (guillotineHeuristicIsValidResult(result.score)) {
                ret = guillotineInsert(pack, width, height, &result, GuillotineSplitMinimizeArea);
                // Optional: merge free rects that share an edge (slow) to improve packing quality.
                // guillotineMergeFreeRects(pack);
            }
            return ret;
        }

        BinPackResult myOtherBinPackInsert(BinPack* pack, int width, int height) {
            // inserts using MAXRECTS-BSSF with flipping allowed
            BinPackResult ret = {0};
            MaxRectsHeuristicResult result = maxRectsBestShortSideFit(pack, width, height);
            if (maxRectsHeuristicIsValidResult(result.scores)) {
                ret = maxRectsInsert(pack, width, height, &result);
            }
            return ret;
        }

        BinPackResult myStaticInsert(BinPack* pack, int width, int height) {
            // Pack is a non resizable bin pack such that it has an upper limit on how many bins can be inserted.
            BinPackResult ret = {0};
            if (binPackHasSpace(pack)) {
                GuillotineHeuristicResult result = guillotineBestShortSideFit(pack, width, height);
                if (guillotineHeuristicIsValidResult(result.score)) {
                    ret = guillotineInsert(pack, width, height, &result, GuillotineSplitMinimizeArea);
                }
            }
            return ret;
        }

    Resizable bin packing using malloc:
        tmbp_rect* allocateRectArray(void* state, size_t size) {
            // We do not need argument state with malloc.
            return (tmbp_rect*)malloc(size * sizeof(tmbp_rect));
        }
        tmbp_rect* reallocateRectArray(void* state, tmbp_rect* ptr, size_t oldSize, size_t newSize) {
            // We do not need arguments state and oldSize with realloc.
            return (tmbp_rect*)realloc(ptr, newSize * sizeof(tmbp_rect));
        }
        void freeRectArray(void* state, tmbp_rect* ptr, size_t size) {
            // We do not need arguments state and size with free.
            free(ptr);
        }

        BinPack myBinPackCreate(int width, int height) {
            return binPackCreate(width, height, NULL, allocateRectArray, reallocateRectArray);
        }
        void myBinPackDestroy(BinPack* pack) {
            binPackDestroy(pack, NULL, freeRectArray);
        }

    Resizable bin packing using new/delete:
        tmbp_rect* allocateRectArray(void* state, size_t size) {
            // We do not need argument state with new.
            return new tmbp_rect[size];
        }
        tmbp_rect* reallocateRectArray(void* state, tmbp_rect* ptr, size_t oldSize, size_t newSize) {
            // We do not need arguments state and oldSize with new/delete.
            // There is no native realloc with new/delete in C++.
            tmbp_rect* newArray = new tmbp_rect[newSize];
            memcpy(newArray, ptr, oldSize * sizeof(tmbp_rect) );
            delete[] ptr;
            return newArray;
        }
        void freeRectArray(void* state, tmbp_rect* ptr, size_t size) {
            // We do not need arguments state and size with delete[].
            delete[] ptr;
        }

        BinPack myBinPackCreate(int width, int height) {
            return binPackCreate(width, height, nullptr, allocateRectArray, reallocateRectArray);
        }
        void myBinPackDestroy(BinPack* pack) {
            binPackDestroy(pack, nullptr, freeRectArray);
        }

    Static bin packing (no resizing) using the stack:
        void doBinPackingOnStack() {
            tmbp_rect freeRectsMemory[200];
            tmbp_rect usedRectsMemory[200];
            int width = 256;
            int height = 256;
            BinPack pack = binPackCreateStatic(width, height, freeRectsMemory, 200, usedRectsMemory, 200);
            // Do NOT use static arrays freeRectsMemory or usedRectsMemory from this point on directly.

            // Pack bins here ...
        }

HISTORY
    v1.0.7  15.01.19 Fixed some gcc and clang warnings.
    v1.0.6  06.10.18 Formatting change.
                     Added TMBP_VERSION.
                     Changed some definitions to use common macros instead.
                     Fixed some C99 compilation warnings.
    v1.0.5  28.05.17 Added binPackReset.
    v1.0.4a 28.08.18 Added repository link.
    v1.0.4  28.05.17 Changed how the allocator works.
                     Added binPackCreateEx.
    v1.0.3a 27.05.17 Fixed a minor signed/unsigned mismatch.
    v1.0.3  16.05.17 Changed formatting and growing factor of the allocator (1.5 from 2).
                     Added binPackClear.
    v1.0.2  31.03.17 Fixed a bug in maxRectsInsert resulting in too many splits.
    v1.0.1b 29.03.17 Changed documentation, fixed a typo, changed allocator functions signature.
    v1.0.1a 07.11.16 Added TM_NULL.
    v1.0.1  11.10.16 Fixed a bug in batch insertion functions using the wrong index.
    v1.0b   09.10.16 Fixed a typo.
    v1.0a   07.10.16 Removed using forced unsigned arithmetic when tm_size_t is signed.
    v1.0    20.07.16 Initial commit.
*/

/* clang-format off */

/* define these to avoid crt */
#ifdef TM_BIN_PACKING_IMPLEMENTATION
    #ifndef TM_ASSERT
        #include <assert.h>
        #define TM_ASSERT assert
    #endif
#endif  /* defined(TM_BIN_PACKING_IMPLEMENTATION) */

#ifndef _TM_BIN_PACKING_H_INCLUDED_
#define _TM_BIN_PACKING_H_INCLUDED_

#define TMBP_VERSION 0x01000700u

#include <stdint.h>

/* size_t is unsigned by default, but we also allow for signed and/or 32bit size_t.
   You can override this block by defining TM_SIZE_T_DEFINED and the typedefs before including this file. */
#ifndef TM_SIZE_T_DEFINED
    #define TM_SIZE_T_DEFINED
    #define TM_SIZE_T_IS_SIGNED 0 /* define to 1 if tm_size_t is signed */
    #include <stddef.h> /* include C version so identifiers are in global namespace */
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

/* Use null of the underlying language. */
#ifndef TM_NULL
    #ifdef __cplusplus
        #define TM_NULL nullptr
    #else
        #define TM_NULL NULL
    #endif
#endif

#ifndef TMBP_OWN_TYPES
    typedef struct { int32_t left, top, width, height; } tmbp_rect_;
    typedef tmbp_rect_ tmbp_rect;
    typedef struct { int32_t width, height; } tmbp_dim_;
    typedef tmbp_dim_ tmbp_dim;

    typedef int32_t tmbp_int;
    /* max value for tmbp_int */
    #define TMBP_INT_MAX INT32_MAX
    /* min value for tmbp_int */
    #define TMBP_INT_MIN INT32_MIN
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

/* clang-format on */

/*
Function types for taking function pointers to these signatures.
Params:
    state: Same as what was passed into binPackCreate, useful if you are using a stateful allocator.
*/
typedef tmbp_rect* BinPackReallocate(void* state, tmbp_rect* ptr, tm_size_t oldSize, tm_size_t newSize);
typedef tmbp_rect* BinPackAllocate(void* state, tm_size_t size);
typedef void BinPackFree(void* state, tmbp_rect* ptr, tm_size_t size);

typedef struct {
    void* state;
    BinPackReallocate* reallocateFunc;
    BinPackFree* freeFunc;
} BinPackAllocator;

typedef struct {
    tmbp_rect* data;
    tm_size_t size;
    tm_size_t capacity;
} tmbp_rect_array;

typedef struct {
    tmbp_int width;
    tmbp_int height;
    tmbp_int usedArea;

    tmbp_rect_array freeRects;
    tmbp_rect_array usedRects;

    BinPackAllocator allocator;
} BinPack;

/* Guillotine bin packing algorithms */
typedef enum {
    GuillotineRectBestAreaFit,      /* BAF */
    GuillotineRectBestShortSideFit, /* BSSF */
    GuillotineRectBestLongSideFit   /* BLSF */
} GuillotineFreeRectChoiceHeuristic;
typedef enum {
    GuillotineSplitShorterLeftoverAxis, /* SLAS */
    GuillotineSplitLongerLeftoverAxis,  /* LLAS */
    GuillotineSplitMinimizeArea,        /* MINAS */
    GuillotineSplitMaximizeArea,        /* MAXAS */
    GuillotineSplitShorterAxis,         /* SAS */
    GuillotineSplitLongerAxis           /* LAS */
} GuillotineSplitHeuristic;

typedef struct {
    tmbp_rect rect;
    tm_bool flipped;
    tm_bool placed;
} BinPackResult;

typedef struct {
    tm_size_t freeRectIndex;
    tmbp_int score;
    tm_bool flipped;
} GuillotineHeuristicResult;

typedef struct {
    tmbp_dim dim;
    void* userData;
} BinPackBatchDim;
typedef struct {
    BinPackResult result;
    void* userData;
} BinPackBatchResult;

/*
Creates a BinPack by allocating memory using allocateFunc and storing allocatorState and reallocateFunc into the
resulting BinPack for dynamic resizing params.
Params:
    allocatorState:   If you are using a stateful allocator, pass in your allocator state. The state will be passed back
                      into the allocation functions supplied.
    allocateFunc:     Function pointer to an allocation function. See BinPackAllocate for the function signature.
    reallocateFunc:   Function pointer to a reallocation function. This is used when dynamically resizing the internal
                      arrays. See BinPackReallocate for the function signature.
    freeFunc:         Function pointer to a free function. See BinPackFree for the function signature.
    initialFreeRects: Initial counts for free rects.
    initialUsedRects: Initial counts for used rects.
*/
TMBP_DEF BinPack binPackCreate(tmbp_int width, tmbp_int height, void* allocatorState, BinPackAllocate* allocateFunc,
                               BinPackReallocate* reallocateFunc, BinPackFree* freeFunc);
TMBP_DEF BinPack binPackCreateEx(tmbp_int width, tmbp_int height, void* allocatorState, BinPackAllocate* allocateFunc,
                                 BinPackReallocate* reallocateFunc, BinPackFree* freeFunc, tm_size_t initialFreeRects,
                                 tm_size_t initialUsedRects);

/* destroys a dynamic BinPack that was created using binPackCreate */
TMBP_DEF void binPackDestroy(BinPack* pack);

/* clears given BinPack without freeing memory */
TMBP_DEF void binPackClear(BinPack* pack);
/* clears given BinPack and resets to new size without freeing memory */
TMBP_DEF void binPackReset(BinPack* pack, tmbp_int width, tmbp_int height);

/* frees unused memory of a dynamic BinPack that was created using binPackCreate */
TMBP_DEF void binPackFitToSize(BinPack* pack);

/*
Creates a static (non resizable) BinPack using the memory supplied. Do NOT call binPackDestroy on the returned
BinPack note that not supplying enough memory to hold the number of bins will result in undefined behavior when
inserting refer to guillotineFreeRectsSize and maxRectsFreeRectsSize for the minimum size of freeRects to hold X
number of bins.
Params:
    freeRects:      Pointer to an array of tmbp_rect to be used as the internal storage for free rects.
                    Must not overlap with usedRects.
    freeRectsCount: Number of entries in freeRects.
    usedRects:      Pointer to an array of tmbp_rect to be used as the internal storage for used rects.
                    Must not overlap with freeRects.
    usedRectsCount: Number of entries in usedRects.
*/
TMBP_DEF BinPack binPackCreateStatic(tmbp_int width, tmbp_int height, tmbp_rect* freeRects, tm_size_t freeRectsCount,
                                     tmbp_rect* usedRects, tm_size_t usedRectsCount);

/*
Initializes pack for usage. Does not need to be called if binPackCreate or binPackCreateStatic is used to create the
BinPack.
*/
TMBP_DEF void binPackInit(BinPack* pack, tmbp_int width, tmbp_int height);

/* Returns 1 if pack has enough space for one more bin, 0 otherwise. */
static tm_bool binPackHasSpace(BinPack* pack);

/* Guillotine heuristics with flipping (swapping width with height when inserting). */
TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFit(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFit(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFit(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF GuillotineHeuristicResult guillotineChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                                    GuillotineFreeRectChoiceHeuristic freeChoice);

/* Guillotine heuristics without flipping (without swapping width with height when inserting). */
TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF GuillotineHeuristicResult guillotineChoiceNoFlip(BinPack* pack, tmbp_int width, tmbp_int height,
                                                          GuillotineFreeRectChoiceHeuristic freeChoice);

/* Returns 1 if score returned by heuristic is valid (bin insertion position was found), 0 otherwise. */
static tm_bool guillotineHeuristicIsValidResult(tmbp_int score);

/* Inserts bin into pack using the result from a heuristic. */
TMBP_DEF BinPackResult guillotineInsert(BinPack* pack, tmbp_int width, tmbp_int height,
                                        const GuillotineHeuristicResult* choice, GuillotineSplitHeuristic splitChoice);

/* Inserts bin into pack using a heuristic. */
TMBP_DEF BinPackResult guillotineInsertChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                              GuillotineFreeRectChoiceHeuristic freeChoice,
                                              GuillotineSplitHeuristic splitChoice, tm_bool canFlip);

/*
Inserts multiple rects into pack at once while consuming rect dimensions from dims.
Note that the order of elements in dims will be altered.
This is different than inserting bins into pack in a loop, the bins will be inserted in the order of best to lowest
heuristic score see NOTES for more information.
*/
TMBP_DEF tm_size_t guillotineInsertBatch(BinPack* pack, BinPackBatchDim* dims, BinPackBatchResult* results,
                                         tm_size_t count, GuillotineFreeRectChoiceHeuristic freeChoice,
                                         GuillotineSplitHeuristic splitChoice, tm_bool canFlip, tm_bool merge);

/* Returns a value between 0 and 1 for how much area is occupied in the pack. */
TMBP_DEF float guillotineOccupancy(const BinPack* pack);

/*
Merges pairs of free rects that share an edge, slow but improves packing quality.
Misses groups of rectangles that share an edge, but those cases are very rare.
*/
TMBP_DEF void guillotineMergeFreeRects(BinPack* pack);

/* Returns how big the freeRects array needs to be to hold binCount number of bins. */
static tm_size_t guillotineFreeRectsSize(tm_size_t binCount);

/* MaxRects bin packing algorithms. */

typedef enum {
    MaxRectsBestShortSideFit, /* BSSF */
    MaxRectsBestLongSideFit,  /* BLSF */
    MaxRectsBestAreaFit,      /* BAF */
    MaxRectsBottomLeftRule,   /* BL */
    MaxRectsContactPointRule  /* CP */
} MaxRectsFreeRectChoiceHeuristic;

typedef struct {
    tm_size_t freeRectIndex;
    tmbp_int scores[2];
    tm_bool flipped;
} MaxRectsHeuristicResult;

/* Max rects heuristics with flipping allowed (swapping width with height when inserting). */
TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFit(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRule(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFit(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFit(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRule(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                                MaxRectsFreeRectChoiceHeuristic freeChoice);

/* Max rects heuristics without flipping (without swapping width with height when inserting). */
TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRuleNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRuleNoFlip(BinPack* pack, tmbp_int width, tmbp_int height);
TMBP_DEF MaxRectsHeuristicResult maxRectsChoiceNoFlip(BinPack* pack, tmbp_int width, tmbp_int height,
                                                      MaxRectsFreeRectChoiceHeuristic freeChoice);

/* Returns 1 if score returned by heuristic is valid (bin insertion position was found), 0 otherwise. */
static tm_bool maxRectsHeuristicIsValidResult(const tmbp_int score[2]);

/* Inserts bin into pack using the result from a heuristic. */
TMBP_DEF BinPackResult maxRectsInsert(BinPack* pack, tmbp_int width, tmbp_int height,
                                      const MaxRectsHeuristicResult* choice);

/* Inserts bin into pack using a heuristic. */
TMBP_DEF BinPackResult maxRectsInsertChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                            MaxRectsFreeRectChoiceHeuristic freeChoice, tm_bool canFlip);

/*
Inserts multiple rects into pack at once while consuming rect dimensions from dims.
Note that the order of elements in dims will be altered.
This is different than inserting bins into pack in a loop, the bins will be inserted in the order of best to lowest
heuristic score see NOTES for more information.
*/
TMBP_DEF tm_size_t maxRectsInsertBatch(BinPack* pack, BinPackBatchDim* dims, BinPackBatchResult* results,
                                       tm_size_t count, MaxRectsFreeRectChoiceHeuristic freeChoice, tm_bool canFlip);

/* Returns a value between 0 and 1 for how much area is occupied in the pack. */
TMBP_DEF float maxRectsOccupancy(const BinPack* pack);

/* Returns how big the freeRects array needs to be to hold binCount number of bins. */
static tm_size_t maxRectsFreeRectsSize(tm_size_t binCount);

/* inline implementation */

inline tm_bool binPackHasSpace(BinPack* pack) { return pack->usedRects.size < pack->usedRects.capacity; }
inline tm_bool guillotineHeuristicIsValidResult(tmbp_int score) { return score != TMBP_INVALID_SCORE; }
inline tm_bool maxRectsHeuristicIsValidResult(const tmbp_int scores[2]) {
    return scores[0] != TMBP_INVALID_SCORE || scores[1] != TMBP_INVALID_SCORE;
}
inline tm_size_t guillotineFreeRectsSize(tm_size_t binCount) { return binCount * 2; }
inline tm_size_t maxRectsFreeRectsSize(tm_size_t binCount) { return binCount * 5; }

#ifdef __cplusplus
}
#endif

#endif /* _TM_BIN_PACKING_H_INCLUDED_ */

#ifdef TM_BIN_PACKING_IMPLEMENTATION

#ifndef TM_ASSERT_VALID_SIZE
    #if defined(TM_SIZE_T_IS_SIGNED) && TM_SIZE_T_IS_SIGNED
        #define TM_ASSERT_VALID_SIZE(x) TM_ASSERT((x) >= 0)
    #else
        /* always true if size_t is unsigned */
        #define TM_ASSERT_VALID_SIZE(x) ((void)0)
    #endif
#endif /* !defined(TM_ASSERT_VALID_SIZE) */

#ifdef __cplusplus
extern "C" {
#endif

static tmbp_rect* tmbp_push(tmbp_rect_array* array, BinPackAllocator* allocator) {
    TM_ASSERT(array);
    TM_ASSERT(array->data);
    TM_ASSERT_VALID_SIZE(array->size);
    TM_ASSERT_VALID_SIZE(array->capacity);
    if (array->size >= array->capacity) {
        if (allocator->reallocateFunc) {
            tm_size_t newSize = (tm_size_t)((array->capacity + 1) * 1.5);
            array->data = allocator->reallocateFunc(allocator->state, array->data, array->capacity, newSize);
            array->capacity = newSize;
        } else {
            TM_ASSERT(0 && "OutOfMemory");
        }
    }

    return &array->data[array->size++];
}
/*static void tmbp_pop( tmbp_rect_array* array )
{
    TM_ASSERT( array );
    TM_ASSERT( array->data );
    TM_ASSERT( array->size > 0 );
    TM_ASSERT( array->size <= array->capacity );
    --array->size;
}*/
static void tmbp_unstable_erase(tmbp_rect_array* array, tm_size_t index) {
    TM_ASSERT(array);
    TM_ASSERT(array->data);
    TM_ASSERT(array->size > 0);
    TM_ASSERT(array->size <= array->capacity);
    TM_ASSERT_VALID_SIZE(index);
    TM_ASSERT(index < array->size);
    /* we assign entry at index to the last element and decrement size
      relative order of elements is not preserved, hence unstable */
    --array->size;
    if (index != array->size) {
        array->data[index] = array->data[array->size];
    }
}
static void tmbp_clear(tmbp_rect_array* array) {
    TM_ASSERT(array);
    TM_ASSERT(array->data);
    array->size = 0;
}

TMBP_DEF BinPack binPackCreate(tmbp_int width, tmbp_int height, void* allocatorState, BinPackAllocate* allocateFunc,
                               BinPackReallocate* reallocateFunc, BinPackFree* freeFunc) {
    return binPackCreateEx(width, height, allocatorState, allocateFunc, reallocateFunc, freeFunc, 16, 16);
}
TMBP_DEF BinPack binPackCreateEx(tmbp_int width, tmbp_int height, void* allocatorState, BinPackAllocate* allocateFunc,
                                 BinPackReallocate* reallocateFunc, BinPackFree* freeFunc, tm_size_t initialFreeRects,
                                 tm_size_t initialUsedRects) {
    BinPack result;
    result.usedRects.data = allocateFunc(allocatorState, initialUsedRects);
    result.usedRects.size = 0;
    result.usedRects.capacity = (result.usedRects.data) ? initialUsedRects : 0;

    result.freeRects.data = allocateFunc(allocatorState, initialFreeRects);
    result.freeRects.size = 0;
    result.freeRects.capacity = (result.freeRects.data) ? initialFreeRects : 0;

    result.allocator.state = allocatorState;
    result.allocator.reallocateFunc = reallocateFunc;
    result.allocator.freeFunc = freeFunc;

    binPackInit(&result, width, height);
    return result;
}
TMBP_DEF void binPackDestroy(BinPack* pack) {
    if (pack->allocator.freeFunc) {
        pack->allocator.freeFunc(pack->allocator.state, pack->freeRects.data, pack->freeRects.capacity);
        pack->allocator.freeFunc(pack->allocator.state, pack->usedRects.data, pack->usedRects.capacity);
    }
    pack->freeRects.data = TM_NULL;
    pack->freeRects.size = 0;
    pack->freeRects.capacity = 0;
    pack->usedRects.data = TM_NULL;
    pack->usedRects.size = 0;
    pack->usedRects.capacity = 0;
}
TMBP_DEF void binPackReset(BinPack* pack, tmbp_int width, tmbp_int height) {
    TM_ASSERT(pack);
    if (pack->freeRects.capacity) {
        pack->freeRects.size = 1;
        tmbp_rect* free_rect = &pack->freeRects.data[0];
        free_rect->left = 0;
        free_rect->top = 0;
        free_rect->width = width;
        free_rect->height = height;
    } else {
        pack->freeRects.size = 0;
    }
    pack->usedRects.size = 0;
    pack->usedArea = 0;
}
TMBP_DEF void binPackClear(BinPack* pack) { binPackReset(pack, pack->width, pack->height); }
TMBP_DEF void binPackFitToSize(BinPack* pack) {
    pack->freeRects.data = pack->allocator.reallocateFunc(pack->allocator.state, pack->freeRects.data,
                                                          pack->freeRects.capacity, pack->freeRects.size);
    pack->freeRects.capacity = pack->freeRects.size;
    pack->usedRects.data = pack->allocator.reallocateFunc(pack->allocator.state, pack->usedRects.data,
                                                          pack->usedRects.capacity, pack->usedRects.size);
    pack->usedRects.capacity = pack->usedRects.size;
}
TMBP_DEF BinPack binPackCreateStatic(tmbp_int width, tmbp_int height, tmbp_rect* freeRects, tm_size_t freeRectsCount,
                                     tmbp_rect* usedRects, tm_size_t usedRectsCount) {
    /* Assert that arrays do not overlap. */
    TM_ASSERT(freeRects >= usedRects + usedRectsCount || freeRects + freeRectsCount <= usedRects);

    BinPack result;
    result.freeRects.data = freeRects;
    result.freeRects.size = 0;
    result.freeRects.capacity = freeRectsCount;

    result.usedRects.data = usedRects;
    result.usedRects.size = 0;
    result.usedRects.capacity = usedRectsCount;

    result.allocator.state = TM_NULL;
    result.allocator.reallocateFunc = TM_NULL;
    binPackInit(&result, width, height);
    return result;
}
TMBP_DEF void binPackInit(BinPack* pack, tmbp_int width, tmbp_int height) {
    TM_ASSERT(pack);

    pack->width = width;
    pack->height = height;
    pack->usedArea = 0;

    tmbp_clear(&pack->usedRects);

    tmbp_clear(&pack->freeRects);
    tmbp_rect* added = tmbp_push(&pack->freeRects, &pack->allocator);
    added->left = 0;
    added->top = 0;
    added->width = width;
    added->height = height;
}

inline static tmbp_int tmbp_abs(tmbp_int val) { return (val < 0) ? (-val) : (val); }
#ifndef TM_MIN
#define TM_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef TM_MAX
#define TM_MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif

static tmbp_int guillotineScoreBestAreaFit(tmbp_int width, tmbp_int height, const tmbp_rect* freeRect) {
    return freeRect->width * freeRect->height - width * height;
}
static tmbp_int guillotineScoreBestShortSideFit(tmbp_int width, tmbp_int height, const tmbp_rect* freeRect) {
    tmbp_int leftoverHoriz = tmbp_abs(freeRect->width - width);
    tmbp_int leftoverVert = tmbp_abs(freeRect->height - height);
    tmbp_int leftover = TM_MIN(leftoverHoriz, leftoverVert);
    return leftover;
}
static tmbp_int guillotineScoreBestLongSideFit(tmbp_int width, tmbp_int height, const tmbp_rect* freeRect) {
    tmbp_int leftoverHoriz = tmbp_abs(freeRect->width - width);
    tmbp_int leftoverVert = tmbp_abs(freeRect->height - height);
    tmbp_int leftover = TM_MAX(leftoverHoriz, leftoverVert);
    return leftover;
}

TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFit(BinPack* pack, tmbp_int width, tmbp_int height) {
    GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

    tm_size_t count = pack->freeRects.size;
    for (tm_size_t i = 0; i < count; ++i) {
        tmbp_rect* current = &pack->freeRects.data[i];
        if (width == current->width && height == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 0;
            break;
        } else if (height == current->width && width == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 1;
            break;
        } else if (width <= current->width && height <= current->height) {
            tmbp_int score = guillotineScoreBestAreaFit(width, height, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 0;
            }
        } else if (height <= current->width && width <= current->height) {
            tmbp_int score = guillotineScoreBestAreaFit(height, width, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 1;
            }
        }
    }
    return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFit(BinPack* pack, tmbp_int width, tmbp_int height) {
    GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

    tm_size_t count = pack->freeRects.size;
    for (tm_size_t i = 0; i < count; ++i) {
        tmbp_rect* current = &pack->freeRects.data[i];
        if (width == current->width && height == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 0;
            break;
        } else if (height == current->width && width == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 1;
            break;
        } else if (width <= current->width && height <= current->height) {
            tmbp_int score = guillotineScoreBestShortSideFit(width, height, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 0;
            }
        } else if (height <= current->width && width <= current->height) {
            tmbp_int score = guillotineScoreBestShortSideFit(height, width, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 1;
            }
        }
    }
    return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFit(BinPack* pack, tmbp_int width, tmbp_int height) {
    GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

    tm_size_t count = pack->freeRects.size;
    for (tm_size_t i = 0; i < count; ++i) {
        tmbp_rect* current = &pack->freeRects.data[i];
        if (width == current->width && height == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 0;
            break;
        } else if (height == current->width && width == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 1;
            break;
        } else if (width <= current->width && height <= current->height) {
            tmbp_int score = guillotineScoreBestLongSideFit(width, height, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 0;
            }
        } else if (height <= current->width && width <= current->height) {
            tmbp_int score = guillotineScoreBestLongSideFit(height, width, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 1;
            }
        }
    }
    return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                                    GuillotineFreeRectChoiceHeuristic freeChoice) {
    GuillotineHeuristicResult result;
    switch (freeChoice) {
        case GuillotineRectBestAreaFit: {
            result = guillotineBestAreaFit(pack, width, height);
            break;
        }
        case GuillotineRectBestShortSideFit: {
            result = guillotineBestShortSideFit(pack, width, height);
            break;
        }
        case GuillotineRectBestLongSideFit: {
            result = guillotineBestLongSideFit(pack, width, height);
            break;
        }
        default: {
            TM_ASSERT(0);
            result.freeRectIndex = 0;
            result.score = TMBP_INVALID_SCORE;
            break;
        }
    }
    return result;
}

TMBP_DEF GuillotineHeuristicResult guillotineBestAreaFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

    tm_size_t count = pack->freeRects.size;
    for (tm_size_t i = 0; i < count; ++i) {
        tmbp_rect* current = &pack->freeRects.data[i];
        if (width == current->width && height == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 0;
            break;
        } else if (width <= current->width && height <= current->height) {
            tmbp_int score = guillotineScoreBestAreaFit(width, height, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 0;
            }
        }
    }
    return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestShortSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

    tm_size_t count = pack->freeRects.size;
    for (tm_size_t i = 0; i < count; ++i) {
        tmbp_rect* current = &pack->freeRects.data[i];
        if (width == current->width && height == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 0;
            break;
        } else if (width <= current->width && height <= current->height) {
            tmbp_int score = guillotineScoreBestShortSideFit(width, height, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 0;
            }
        }
    }
    return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineBestLongSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    GuillotineHeuristicResult result = {0, TMBP_INVALID_SCORE, 0};

    tm_size_t count = pack->freeRects.size;
    for (tm_size_t i = 0; i < count; ++i) {
        tmbp_rect* current = &pack->freeRects.data[i];
        if (width == current->width && height == current->height) {
            result.freeRectIndex = i;
            result.score = TMBP_INT_MIN;
            result.flipped = 0;
            break;
        } else if (width <= current->width && height <= current->height) {
            tmbp_int score = guillotineScoreBestLongSideFit(width, height, current);
            if (score < result.score) {
                result.freeRectIndex = i;
                result.score = score;
                result.flipped = 0;
            }
        }
    }
    return result;
}
TMBP_DEF GuillotineHeuristicResult guillotineChoiceNoFlip(BinPack* pack, tmbp_int width, tmbp_int height,
                                                          GuillotineFreeRectChoiceHeuristic freeChoice) {
    GuillotineHeuristicResult result;
    switch (freeChoice) {
        case GuillotineRectBestAreaFit: {
            result = guillotineBestAreaFitNoFlip(pack, width, height);
            break;
        }
        case GuillotineRectBestShortSideFit: {
            result = guillotineBestShortSideFitNoFlip(pack, width, height);
            break;
        }
        case GuillotineRectBestLongSideFit: {
            result = guillotineBestLongSideFitNoFlip(pack, width, height);
            break;
        }
        default: {
            TM_ASSERT(0);
            result.freeRectIndex = 0;
            result.score = TMBP_INVALID_SCORE;
            break;
        }
    }
    return result;
}

static tmbp_rect binPackRectFromPosition(BinPack* pack, tmbp_int width, tmbp_int height, tm_size_t index,
                                         tm_bool flipped) {
    tmbp_rect rect = {0, 0, 0, 0};
    tmbp_rect* current = &pack->freeRects.data[index];
    rect.left = current->left;
    rect.top = current->top;
    if (flipped) {
        rect.width = height;
        rect.height = width;
    } else {
        rect.width = width;
        rect.height = height;
    }
    return rect;
}

static void guillotineSplitFreeRectByHeuristic(tmbp_rect_array* freeRects, BinPackAllocator* allocator,
                                               const tmbp_rect* freeRect, const tmbp_rect* placedRect,
                                               GuillotineSplitHeuristic heuristic) {
    tmbp_int w = freeRect->width - placedRect->width;
    tmbp_int h = freeRect->height - placedRect->height;

    int splitHorizontal = 1;
    switch (heuristic) {
        case GuillotineSplitShorterLeftoverAxis: {
            splitHorizontal = (w <= h);
            break;
        }
        case GuillotineSplitLongerLeftoverAxis: {
            splitHorizontal = (w > h);
            break;
        }
        case GuillotineSplitMinimizeArea: {
            splitHorizontal = (placedRect->width * h > w * placedRect->height);
            break;
        }
        case GuillotineSplitMaximizeArea: {
            splitHorizontal = (placedRect->width * h <= w * placedRect->height);
            break;
        }
        case GuillotineSplitShorterAxis: {
            splitHorizontal = (freeRect->width <= freeRect->height);
            break;
        }
        case GuillotineSplitLongerAxis: {
            splitHorizontal = (freeRect->width > freeRect->height);
            break;
        }
        default: {
            TM_ASSERT(0);
            break;
        }
    }

    tmbp_rect bottom;
    bottom.left = freeRect->left;
    bottom.top = freeRect->top + placedRect->height;
    bottom.height = h;
    tmbp_rect right;
    right.left = freeRect->left + placedRect->width;
    right.top = freeRect->top;
    right.width = w;
    if (splitHorizontal) {
        bottom.width = freeRect->width;
        right.height = placedRect->height;
    } else {
        bottom.width = placedRect->width;
        right.height = freeRect->height;
    }
    if (bottom.width > 0 && bottom.height > 0) {
        *tmbp_push(freeRects, allocator) = bottom;
    }
    if (right.width > 0 && right.height > 0) {
        *tmbp_push(freeRects, allocator) = right;
    }
}

TMBP_DEF BinPackResult guillotineInsert(BinPack* pack, tmbp_int width, tmbp_int height,
                                        const GuillotineHeuristicResult* choice, GuillotineSplitHeuristic splitChoice) {
    TM_ASSERT(choice);
    TM_ASSERT(choice->score != TMBP_INVALID_SCORE);

    BinPackResult ret = {{0, 0, 0, 0}, TM_FALSE, TM_FALSE};
    ret.rect = binPackRectFromPosition(pack, width, height, choice->freeRectIndex, choice->flipped);
    ret.flipped = choice->flipped;
    ret.placed = 1;
    tmbp_rect freeRect = pack->freeRects.data[choice->freeRectIndex];
    tmbp_unstable_erase(&pack->freeRects, choice->freeRectIndex);
    guillotineSplitFreeRectByHeuristic(&pack->freeRects, &pack->allocator, &freeRect, &ret.rect, splitChoice);
    *tmbp_push(&pack->usedRects, &pack->allocator) = ret.rect;
    pack->usedArea += ret.rect.width * ret.rect.height;
    return ret;
}
TMBP_DEF BinPackResult guillotineInsertChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                              GuillotineFreeRectChoiceHeuristic freeChoice,
                                              GuillotineSplitHeuristic splitChoice, tm_bool canFlip) {
    BinPackResult ret = {{0, 0, 0, 0}, TM_FALSE, TM_FALSE};
    GuillotineHeuristicResult result;
    if (canFlip) {
        result = guillotineChoice(pack, width, height, freeChoice);
    } else {
        result = guillotineChoiceNoFlip(pack, width, height, freeChoice);
    }
    if (guillotineHeuristicIsValidResult(result.score)) {
        ret = guillotineInsert(pack, width, height, &result, splitChoice);
    }
    return ret;
}
TMBP_DEF tm_size_t guillotineInsertBatch(BinPack* pack, BinPackBatchDim* dims, BinPackBatchResult* results,
                                         tm_size_t count, GuillotineFreeRectChoiceHeuristic freeChoice,
                                         GuillotineSplitHeuristic splitChoice, tm_bool canFlip, tm_bool merge) {
    tm_size_t dimsCount = count;
    GuillotineHeuristicResult best = {0, 0, TM_FALSE};
    while (dimsCount > 0) {
        best.score = TMBP_INVALID_SCORE;
        tm_size_t bestIndex = 0;
        for (tm_size_t i = 0; i < dimsCount; ++i) {
            tmbp_int w = dims[i].dim.width;
            tmbp_int h = dims[i].dim.height;
            GuillotineHeuristicResult result;
            if (canFlip) {
                result = guillotineChoice(pack, w, h, freeChoice);
            } else {
                result = guillotineChoiceNoFlip(pack, w, h, freeChoice);
            }
            if (result.score < best.score) {
                best = result;
                bestIndex = i;
                if (result.score == TMBP_INT_MIN) {
                    break;
                }
            }
        }
        if (best.score == TMBP_INVALID_SCORE) {
            /* We didn't find any matches. */
            break;
        }

        BinPackBatchResult* result = results++;
        result->result =
            guillotineInsert(pack, dims[bestIndex].dim.width, dims[bestIndex].dim.height, &best, splitChoice);
        result->userData = dims[bestIndex].userData;

        if (result->result.placed && merge) {
            guillotineMergeFreeRects(pack);
        }

        /* We processed bestIndex already, move it to the back of the array and decrease dimsCount. */
        --dimsCount;
        if (bestIndex != dimsCount) {
            BinPackBatchDim tmp = dims[bestIndex];
            dims[bestIndex] = dims[dimsCount];
            dims[dimsCount] = tmp;
        }
    }
    return (tm_size_t)(count - dimsCount);
}
TMBP_DEF float guillotineOccupancy(const BinPack* pack) { return (float)pack->usedArea / (pack->width * pack->height); }
TMBP_DEF void guillotineMergeFreeRects(BinPack* pack) {
    /* Merge pairs of free rects into one if able.
       Since we are only looking at pairs, we will miss chances to merge three rectangles into one. */
    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* a = &freeRects[i];
        for (tm_size_t j = i + 1; j < freeRectsCount;) {
            tmbp_rect* b = &freeRects[j];
            if (a->left == b->left && a->width == b->width) {
                if (a->top == b->top + b->height) {
                    a->top -= b->height;
                    a->height += b->height;
                    tmbp_unstable_erase(&pack->freeRects, j);
                    --freeRectsCount;
                    continue;
                } else if (a->top + a->height == b->top) {
                    a->height += b->height;
                    tmbp_unstable_erase(&pack->freeRects, j);
                    --freeRectsCount;
                    continue;
                }
            } else if (a->top == b->top && a->height == b->height) {
                if (a->left == b->left + b->width) {
                    a->left -= b->width;
                    a->width += b->width;
                    tmbp_unstable_erase(&pack->freeRects, j);
                    --freeRectsCount;
                    continue;
                } else if (a->left + a->width == b->left) {
                    a->width += b->width;
                    tmbp_unstable_erase(&pack->freeRects, j);
                    --freeRectsCount;
                    continue;
                }
            }
            ++j;
        }
    }
}

/* MaxRects */
static tmbp_int maxRectsCommonIntervalLength(tmbp_int aStart, tmbp_int aEnd, tmbp_int bStart, tmbp_int bEnd) {
    if (aEnd < bStart || bEnd < aStart) {
        return 0;
    }
    return TM_MIN(aEnd, bEnd) - TM_MAX(aStart, bStart);
}
static tmbp_int maxRectsContactPointScore(BinPack* pack, tmbp_int left, tmbp_int top, tmbp_int right, tmbp_int bottom) {
    tmbp_int score = 0;
    if (left == 0 || right == pack->width) {
        score += bottom - top;
    }
    if (top == 0 || bottom == pack->height) {
        score += right - left;
    }

    tmbp_rect* usedRects = pack->usedRects.data;
    tm_size_t usedRectsCount = pack->usedRects.size;
    for (tm_size_t i = 0; i < usedRectsCount; ++i) {
        tmbp_rect* current = &usedRects[i];
        if (current->left == right || current->left + current->width == left) {
            score += maxRectsCommonIntervalLength(current->top, current->top + current->height, top, bottom);
        }
        if (current->top == bottom || current->top + current->height == top) {
            score += maxRectsCommonIntervalLength(current->left, current->left + current->width, left, right);
        }
    }
    return score;
}

TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFit(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int leftoverHoriz = current->width - width;
            tmbp_int leftoverVert = current->height - height;
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);
            tmbp_int longSideFit = TM_MAX(leftoverHoriz, leftoverVert);

            if (shortSideFit < result.scores[0] ||
                (shortSideFit == result.scores[0] && longSideFit < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = shortSideFit;
                result.scores[1] = longSideFit;
                result.flipped = 0;
            }
        }

        if (height <= current->width && width <= current->height) {
            tmbp_int leftoverHoriz = current->width - height;
            tmbp_int leftoverVert = current->height - width;
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);
            tmbp_int longSideFit = TM_MAX(leftoverHoriz, leftoverVert);

            if (shortSideFit < result.scores[0] ||
                (shortSideFit == result.scores[0] && longSideFit < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = shortSideFit;
                result.scores[1] = longSideFit;
                result.flipped = 1;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRule(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int topSideY = current->top + height;
            if (topSideY < result.scores[0] || (topSideY == result.scores[0] && current->left < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = topSideY;
                result.scores[1] = current->left;
                result.flipped = 0;
            }
        }

        if (height <= current->width && width <= current->height) {
            tmbp_int topSideY = current->top + width;
            if (topSideY < result.scores[0] || (topSideY == result.scores[0] && current->left < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = topSideY;
                result.scores[1] = current->left;
                result.flipped = 1;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFit(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - width);
            tmbp_int leftoverVert = tmbp_abs(current->height - height);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);
            tmbp_int longSideFit = TM_MAX(leftoverHoriz, leftoverVert);

            if (longSideFit < result.scores[1] ||
                (longSideFit == result.scores[1] && shortSideFit < result.scores[0])) {
                result.freeRectIndex = i;
                result.scores[0] = shortSideFit;
                result.scores[1] = longSideFit;
                result.flipped = 0;
            }
        }

        if (height <= current->width && width <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - height);
            tmbp_int leftoverVert = tmbp_abs(current->height - width);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);
            tmbp_int longSideFit = TM_MAX(leftoverHoriz, leftoverVert);

            if (longSideFit < result.scores[1] ||
                (longSideFit == result.scores[1] && shortSideFit < result.scores[0])) {
                result.freeRectIndex = i;
                result.scores[0] = shortSideFit;
                result.scores[1] = longSideFit;
                result.flipped = 1;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFit(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        tmbp_int areaFit = current->width * current->height - width * height;

        if (width <= current->width && height <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - width);
            tmbp_int leftoverVert = tmbp_abs(current->height - height);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);

            if (areaFit < result.scores[0] || (areaFit == result.scores[0] && shortSideFit < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = areaFit;
                result.scores[1] = shortSideFit;
                result.flipped = 0;
            }
        }

        if (height <= current->width && width <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - height);
            tmbp_int leftoverVert = tmbp_abs(current->height - width);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);

            if (areaFit < result.scores[0] || (areaFit == result.scores[0] && shortSideFit < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = areaFit;
                result.scores[1] = shortSideFit;
                result.flipped = 1;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRule(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int score = -maxRectsContactPointScore(pack, current->left, current->top, current->left + width,
                                                        current->top + height);
            if (score < result.scores[0]) {
                result.freeRectIndex = i;
                result.scores[0] = score;
                result.flipped = 0;
            }
        }

        if (height <= current->width && width <= current->height) {
            tmbp_int score = -maxRectsContactPointScore(pack, current->left, current->top, current->left + height,
                                                        current->top + width);
            if (score < result.scores[0]) {
                result.freeRectIndex = i;
                result.scores[0] = score;
                result.flipped = 1;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                                MaxRectsFreeRectChoiceHeuristic freeChoice) {
    MaxRectsHeuristicResult result;
    switch (freeChoice) {
        case MaxRectsBestShortSideFit: {
            result = maxRectsBestShortSideFit(pack, width, height);
            break;
        }
        case MaxRectsBottomLeftRule: {
            result = maxRectsBottomLeftRule(pack, width, height);
            break;
        }
        case MaxRectsContactPointRule: {
            result = maxRectsContactPointRule(pack, width, height);
            break;
        }
        case MaxRectsBestLongSideFit: {
            result = maxRectsBestLongSideFit(pack, width, height);
            break;
        }
        case MaxRectsBestAreaFit: {
            result = maxRectsBestAreaFit(pack, width, height);
            break;
        }
        default: {
            TM_ASSERT(0 && "Invalid enum value.");
            result.scores[0] = TMBP_INVALID_SCORE;
            result.scores[1] = TMBP_INVALID_SCORE;
            break;
        }
    }
    return result;
}

/* No flip variants. */
TMBP_DEF MaxRectsHeuristicResult maxRectsBestShortSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - width);
            tmbp_int leftoverVert = tmbp_abs(current->height - height);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);
            tmbp_int longSideFit = TM_MAX(leftoverHoriz, leftoverVert);

            if (shortSideFit < result.scores[0] ||
                (shortSideFit == result.scores[0] && longSideFit < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = shortSideFit;
                result.scores[1] = longSideFit;
                result.flipped = 0;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBottomLeftRuleNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int topSideY = current->top + height;
            if (topSideY < result.scores[0] || (topSideY == result.scores[0] && current->left < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = topSideY;
                result.scores[1] = current->left;
                result.flipped = 0;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestLongSideFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - width);
            tmbp_int leftoverVert = tmbp_abs(current->height - height);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);
            tmbp_int longSideFit = TM_MAX(leftoverHoriz, leftoverVert);

            if (longSideFit < result.scores[1] ||
                (longSideFit == result.scores[1] && shortSideFit < result.scores[0])) {
                result.freeRectIndex = i;
                result.scores[0] = shortSideFit;
                result.scores[1] = longSideFit;
                result.flipped = 0;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsBestAreaFitNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        tmbp_int areaFit = current->width * current->height - width * height;

        if (width <= current->width && height <= current->height) {
            tmbp_int leftoverHoriz = tmbp_abs(current->width - width);
            tmbp_int leftoverVert = tmbp_abs(current->height - height);
            tmbp_int shortSideFit = TM_MIN(leftoverHoriz, leftoverVert);

            if (areaFit < result.scores[0] || (areaFit == result.scores[0] && shortSideFit < result.scores[1])) {
                result.freeRectIndex = i;
                result.scores[0] = areaFit;
                result.scores[1] = shortSideFit;
                result.flipped = 0;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsContactPointRuleNoFlip(BinPack* pack, tmbp_int width, tmbp_int height) {
    MaxRectsHeuristicResult result = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* current = &freeRects[i];
        if (width <= current->width && height <= current->height) {
            tmbp_int score = -maxRectsContactPointScore(pack, current->left, current->top, current->left + width,
                                                        current->top + height);
            if (score < result.scores[0]) {
                result.freeRectIndex = i;
                result.scores[0] = score;
                result.flipped = 0;
            }
        }
    }

    return result;
}
TMBP_DEF MaxRectsHeuristicResult maxRectsChoiceNoFlip(BinPack* pack, tmbp_int width, tmbp_int height,
                                                      MaxRectsFreeRectChoiceHeuristic freeChoice) {
    MaxRectsHeuristicResult result;
    switch (freeChoice) {
        case MaxRectsBestShortSideFit: {
            result = maxRectsBestShortSideFitNoFlip(pack, width, height);
            break;
        }
        case MaxRectsBottomLeftRule: {
            result = maxRectsBottomLeftRuleNoFlip(pack, width, height);
            break;
        }
        case MaxRectsContactPointRule: {
            result = maxRectsContactPointRuleNoFlip(pack, width, height);
            break;
        }
        case MaxRectsBestLongSideFit: {
            result = maxRectsBestLongSideFitNoFlip(pack, width, height);
            break;
        }
        case MaxRectsBestAreaFit: {
            result = maxRectsBestAreaFitNoFlip(pack, width, height);
            break;
        }
        default: {
            TM_ASSERT(0);
            result.scores[0] = TMBP_INVALID_SCORE;
            result.scores[1] = TMBP_INVALID_SCORE;
            break;
        }
    }
    return result;
}

static tm_bool maxRectsSplitFreeNode(BinPack* pack, tmbp_rect freeNode, const tmbp_rect* usedNode) {
    tmbp_int freeNodeRight = freeNode.left + freeNode.width;
    tmbp_int freeNodeBottom = freeNode.top + freeNode.height;
    tmbp_int usedNodeRight = usedNode->left + usedNode->width;
    tmbp_int usedNodeBottom = usedNode->top + usedNode->height;
    if (usedNode->left >= freeNodeRight || usedNodeRight <= freeNode.left || usedNode->top >= freeNodeBottom ||
        usedNodeBottom <= freeNode.top) {
        return 0;
    }

    if (usedNode->left < freeNodeRight && usedNodeRight > freeNode.left) {
        if (usedNode->top > freeNode.top && usedNode->top < freeNodeBottom) {
            tmbp_rect* added = tmbp_push(&pack->freeRects, &pack->allocator);
            *added = freeNode;
            added->height = usedNode->top - freeNode.top;
        }

        if (usedNodeBottom < freeNodeBottom) {
            tmbp_rect* added = tmbp_push(&pack->freeRects, &pack->allocator);
            *added = freeNode;
            added->top = usedNodeBottom;
            added->height = freeNodeBottom - usedNodeBottom;
        }
    }

    if (usedNode->top < freeNodeBottom && usedNodeBottom > freeNode.top) {
        if (usedNode->left > freeNode.left && usedNode->left < freeNodeRight) {
            tmbp_rect* added = tmbp_push(&pack->freeRects, &pack->allocator);
            *added = freeNode;
            added->width = usedNode->left - freeNode.left;
        }

        if (usedNodeRight < freeNodeRight) {
            tmbp_rect* added = tmbp_push(&pack->freeRects, &pack->allocator);
            *added = freeNode;
            added->left = usedNodeRight;
            added->width = freeNodeRight - usedNodeRight;
        }
    }
    return 1;
}

static tm_bool maxRectsIsContainedIn(const tmbp_rect* a, const tmbp_rect* b) {
    return a->left >= b->left && a->top >= b->top && a->left + a->width <= b->left + b->width &&
           a->top + a->height <= b->top + b->height;
}
static void maxRectsPruneFreeRects(BinPack* pack) {
    TM_ASSERT(pack);

    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; ++i) {
        tmbp_rect* a = &freeRects[i];
        for (tm_size_t j = i + 1; j < freeRectsCount;) {
            tmbp_rect* b = &freeRects[j];
            if (maxRectsIsContainedIn(a, b)) {
                tmbp_unstable_erase(&pack->freeRects, i);
                --i;
                --freeRectsCount;
                break;
            }
            if (maxRectsIsContainedIn(b, a)) {
                tmbp_unstable_erase(&pack->freeRects, j);
                --freeRectsCount;
                continue;
            }
            ++j;
        }
    }
}

TMBP_DEF BinPackResult maxRectsInsert(BinPack* pack, tmbp_int width, tmbp_int height,
                                      const MaxRectsHeuristicResult* choice) {
    TM_ASSERT(choice);
    TM_ASSERT(maxRectsHeuristicIsValidResult(choice->scores));
    BinPackResult ret;
    ret.rect = binPackRectFromPosition(pack, width, height, choice->freeRectIndex, choice->flipped);
    ret.flipped = choice->flipped;
    ret.placed = 1;
    tmbp_rect* freeRects = pack->freeRects.data;
    tm_size_t freeRectsCount = pack->freeRects.size;
    for (tm_size_t i = 0; i < freeRectsCount; i++) {
        if (maxRectsSplitFreeNode(pack, freeRects[i], &ret.rect)) {
            tmbp_unstable_erase(&pack->freeRects, i);
        }
    }
    maxRectsPruneFreeRects(pack);
    *tmbp_push(&pack->usedRects, &pack->allocator) = ret.rect;
    pack->usedArea += ret.rect.width * ret.rect.height;
    return ret;
}

TMBP_DEF BinPackResult maxRectsInsertChoice(BinPack* pack, tmbp_int width, tmbp_int height,
                                            MaxRectsFreeRectChoiceHeuristic freeChoice, tm_bool canFlip) {
    BinPackResult ret = {{0, 0, 0, 0}, TM_FALSE, TM_FALSE};
    MaxRectsHeuristicResult result;
    if (canFlip) {
        result = maxRectsChoice(pack, width, height, freeChoice);
    } else {
        result = maxRectsChoiceNoFlip(pack, width, height, freeChoice);
    }

    if (maxRectsHeuristicIsValidResult(result.scores)) {
        ret = maxRectsInsert(pack, width, height, &result);
    }

    return ret;
}
TMBP_DEF tm_size_t maxRectsInsertBatch(BinPack* pack, BinPackBatchDim* dims, BinPackBatchResult* results,
                                       tm_size_t count, MaxRectsFreeRectChoiceHeuristic freeChoice, tm_bool canFlip) {
    tm_size_t dimsCount = count;
    while (dimsCount > 0) {
        MaxRectsHeuristicResult best = {0, {TMBP_INVALID_SCORE, TMBP_INVALID_SCORE}, TM_FALSE};
        tm_size_t bestIndex = 0;
        for (tm_size_t i = 0; i < dimsCount; ++i) {
            MaxRectsHeuristicResult result;
            tmbp_int w = dims[i].dim.width;
            tmbp_int h = dims[i].dim.height;
            if (canFlip) {
                result = maxRectsChoice(pack, w, h, freeChoice);
            } else {
                result = maxRectsChoiceNoFlip(pack, w, h, freeChoice);
            }
            if (result.scores[0] < best.scores[0] ||
                (result.scores[0] == best.scores[0] && result.scores[1] < best.scores[1])) {
                best = result;
                bestIndex = i;
            }
        }

        if (!maxRectsHeuristicIsValidResult(best.scores)) {
            break;
        }
        BinPackBatchResult* result = results++;
        result->result = maxRectsInsert(pack, dims[bestIndex].dim.width, dims[bestIndex].dim.height, &best);
        result->userData = dims[bestIndex].userData;

        /* We processed bestIndex already, move it to the back of the array and decrease dimsCount. */
        --dimsCount;
        if (bestIndex != dimsCount) {
            BinPackBatchDim tmp = dims[bestIndex];
            dims[bestIndex] = dims[dimsCount];
            dims[dimsCount] = tmp;
        }
    }
    return (tm_size_t)(count - dimsCount);
}
TMBP_DEF float maxRectsOccupancy(const BinPack* pack) { return guillotineOccupancy(pack); }

#ifdef __cplusplus
}
#endif

#endif /* defined(TM_BIN_PACKING_IMPLEMENTATION) */

/*
There are two licenses you can freely choose from - MIT or Public Domain
---------------------------------------------------------------------------

MIT License:
Copyright (c) 2017 Tolga Mizrak

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

/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "krafton jungle",
    /* First member's full name */
    "Sijun Yang",
    /* First member's email address */
    "yangsijun5528@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

// fundamental alignment:
// 일반적인 C 객체를 안전하게 저장하기 위해 필요한 기본 정렬 수준.
// (특별히 더 큰 정렬을 요구하는 over-aligned 타입은 제외)
//
// malloc이 반환하는 포인터는 적어도 이런 기본 정렬 요구를 만족해야 한다.
// 이유는 CPU의 정렬된 메모리 접근, 컴파일러 최적화, ABI/struct layout이 alignment를 전제로 하기 때문이다.

// malloc lab에서는 8-byte alignment면 충분하다고 가정한다.
#define ALIGNMENT 8

// ALIGNMENT == 8 이라는 가정 아래, size를 가장 가까운 8의 배수로 올림
// size에 7을 더한 뒤 비트 연산으로 하위 3비트를 0으로 만들어 8의 배수로 맞춘다.
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// sizeof(size_t)를 정렬 단위에 맞게 올린 값. 보통 블록 헤더 크기나 payload 시작 위치 계산에 사용한다.
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define META_SIZE 4 // header 또는 footer 크기
#define OVERHEAD (2 * META_SIZE) // 블록의 고정 메타데이터 크기(header + footer)
#define CHUNK_SIZE (1 << 12) // 힙을 한 번 늘릴 때 사용하는 바이트 수 | (1 << 12) 4096
#define MIN_BLOCK_SIZE (OVERHEAD + (2 * sizeof(size_t))) // prev/next를 담을 수 있어야 함

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

// header/footer에 저장할 값을 만든다.
// size는 header 크기가 아니라 header와 footer를 포함한 "블록 전체 크기"다.
// is_alloc은 하위 1비트에 저장되며, size는 ALIGNMENT 때문에 하위 3비트가 0이라고 가정한다.
#define PACK(size, is_alloc) ((size) | (is_alloc))

#define GET_META(p) (*(unsigned int *)(p)) // 4-byte metadata 읽기
#define PUT_META(p, val) (*(unsigned int *)(p) = (val)) // 4-byte metadata 쓰기

// header/footer word에서 블록 전체 크기와 할당 비트를 분리한다.
// 하위 3비트는 상태 bit 용도로 비워두고, 크기는 8-byte 단위로 정렬되어 있다.
#define GET_SIZE(p) (GET_META(p) & ~0x7) // 블록 크기 추출
#define GET_ALLOC(p) (GET_META(p) & 0x1) // 할당 비트 추출

#define HDRP(bp) ((char *)(bp) - META_SIZE) // payload 포인터로부터 header 주소를 계산
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - OVERHEAD) // payload 포인터로부터 footer 주소를 계산

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp))) // 현재 블록 다음 블록의 payload 주소를 계산
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - OVERHEAD)) // 현재 블록 이전 블록의 payload 주소를 계산

static char *heap_listp = NULL;
static void *free_listp = NULL;

#define PREV_FREEP(bp) (*(void **)(bp))
#define NEXT_FREEP(bp) (*(void **)((char *)(bp) + sizeof(void *)))
#define SET_PREV_FREE(bp, prev) (PREV_FREEP(bp) = (prev))
#define SET_NEXT_FREE(bp, next) (NEXT_FREEP(bp) = (next))

//FIFO
void push_list(void *bp) {
    SET_PREV_FREE(bp, NULL);
    SET_NEXT_FREE(bp, free_listp);

    if (free_listp != NULL) {
        SET_PREV_FREE(free_listp, bp);
    }

    free_listp = bp;
}

void *pop_list(size_t asize) {
    void *bp = free_listp;

    while (bp != NULL) {
        if (asize <= GET_SIZE(HDRP(bp))) {
            void *prev = PREV_FREEP(bp);
            void *next = NEXT_FREEP(bp);

            if (prev != NULL) {
                SET_NEXT_FREE(prev, next);
            } else {
                free_listp = next;
            }

            if (next != NULL) {
                SET_PREV_FREE(next, prev);
            }

            SET_PREV_FREE(bp, NULL);
            SET_NEXT_FREE(bp, NULL);
            return bp;
        }

        bp = NEXT_FREEP(bp);
    }

    return NULL;
}

// 메모리 추가 공간이 생기면 인접한 free 상태의 블록과 합치는(coalesce) 역할
// 호출되는 케이스
// 1. heap 공간 확장
// 2. 사용자가 할당된 메모리 공간 헤제(`mm_free()`) 요청
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        return bp;
    }

    if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT_META(HDRP(bp), PACK(size, 0));
        PUT_META(FTRP(bp), PACK(size, 0));
    } else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT_META(FTRP(bp), PACK(size, 0));
        PUT_META(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    } else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT_META(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT_META(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    return bp;
}

static void *extend_heap(size_t bytes) {
    // 어차피 CHUCK_SIZE이상이라 굳이 예외처리할 정도는 아닌데, 인식하곤 있어야 함.
    assert (bytes >= MIN_BLOCK_SIZE);
    size_t size = ALIGN(bytes);
    char *bp = mem_sbrk(size); // 새 시작 주소
    if (bp == (void *) -1) {
        return NULL;
    }

    assert(size % ALIGNMENT == 0); // 정렬된 상태


    PUT_META(HDRP(bp), PACK(size, 0)); // new hdr
    PUT_META(FTRP(bp), PACK(size, 0)); // new ftr
    PUT_META(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // epi hdr 설정
    SET_PREV_FREE(bp, NULL);
    SET_NEXT_FREE(bp, NULL);

    /*
     * mm_init에서의 최초 호출에서 메모리 뷰 찍으면 이렇게 나옴
     * pad           pro hdr       pro ftr       new hdr
     * 00 00 00 00   09 00 00 00   09 00 00 00   00 10 00 00
     * ...
     * new pld       new pld       new ftr       epi hdr
     * 00 00 00 00   00 00 00 00   00 10 00 00   01 00 00 00
     *
     * 00 10 00 00 = 10^12 = 4092
     * 즉, new hdr은 아직 할당되지 않았으며 4092 byte의 공간을 가진다는 의미.
     */

    return coalesce(bp);
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    char *heap_initp = mem_sbrk(4 * META_SIZE);
    if (heap_initp == (void *) -1) {
        return -1;
    }

    // heap_listp가 8-byte alignment를 가지기 위해서 패딩 추가
    PUT_META(heap_initp, 0);                                    // alignment padding
    PUT_META(heap_initp + (1 * META_SIZE), PACK(OVERHEAD, 1));  // prologue header
    PUT_META(heap_initp + (2 * META_SIZE), PACK(OVERHEAD, 1));  // prologue footer
    PUT_META(heap_initp + (3 * META_SIZE), PACK(0, 1));         // epilogue header
    // heap_listp는 prologue payload 포인터
    heap_listp = heap_initp + 2 * META_SIZE;
    free_listp = NULL;

    /*
     * 메모리 뷰 찍으면 이렇게 나옴
     * pad           pro hdr       pro ftr       epi hdr
     * 00 00 00 00   09 00 00 00   09 00 00 00   01 00 00 00
     *
     * 리틀 엔디언이라 순서가 반대임.
     *
     * 09 00 00 00: size:8 + alloc:1
     * 01 00 00 00: size:0 + alloc:1
     */

    if (extend_heap(CHUNK_SIZE) == NULL) {
        return -1;
    }

    return 0;
}


static void *first_fit(size_t asize) {
    char *bp = heap_listp;

    while (GET_SIZE(HDRP(bp)) != 0) {
        if (!GET_ALLOC(HDRP(bp)) && asize <= GET_SIZE(HDRP(bp))) {
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }

    return NULL;
}

// 할당하는데, 필요하면 분할
static void place(void *bp, size_t asize)
{
    size_t free_size      = GET_SIZE(HDRP(bp));
    size_t remain_size    = free_size - asize;
    int split_required    = remain_size >= MIN_BLOCK_SIZE;
    size_t allocated_size = split_required ? asize : free_size;

    PUT_META(HDRP(bp), PACK(allocated_size, 1));
    PUT_META(FTRP(bp), PACK(allocated_size, 1));

    if (split_required) {
        void *next_bp = NEXT_BLKP(bp); // 방금 만들었던 곳으로 이동

        PUT_META(HDRP(next_bp), PACK(remain_size, 0));
        PUT_META(FTRP(next_bp), PACK(remain_size, 0));
    }
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t asize; // adjusted size - 조정된 크기
    size_t extendsize;
    char *bp;

    if (size == 0) { return NULL; }

    asize = MAX(ALIGN(size + OVERHEAD), MIN_BLOCK_SIZE);

    bp = first_fit(asize);
    if (bp == NULL) {
        extendsize = MAX(asize, CHUNK_SIZE);

        bp = extend_heap(extendsize);
        if (bp == NULL) { return NULL; }
    }

    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
    if (ptr == NULL) return;

    size_t size = GET_SIZE(HDRP(ptr));
    PUT_META(HDRP(ptr), PACK(size, 0));
    PUT_META(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) return mm_malloc(size);

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    size_t old_size = GET_SIZE(HDRP(ptr));
    size_t asize = MAX(ALIGN(size + OVERHEAD), MIN_BLOCK_SIZE);

    /* 1) 이미 충분히 크면 shrink */
    if (asize <= old_size) {
        size_t remain = old_size - asize;

        if (remain >= MIN_BLOCK_SIZE) {
            PUT_META(HDRP(ptr), PACK(old_size, 0));
            PUT_META(FTRP(ptr), PACK(old_size, 0));

            place(ptr, asize);

            void *split_bp = NEXT_BLKP(ptr);
            coalesce(split_bp);
        }

        return ptr;
    }

    /* 2) 다음 블록이 free면 흡수해서 in-place expand 시도 */
    void *next_bp = NEXT_BLKP(ptr);
    if (!GET_ALLOC(HDRP(next_bp))) {
        size_t next_size = GET_SIZE(HDRP(next_bp));
        size_t total_size = old_size + next_size;

        if (total_size >= asize) {
            /* ptr + next_bp를 하나의 큰 free block으로 만든다 */
            PUT_META(HDRP(ptr), PACK(total_size, 0));
            PUT_META(FTRP(ptr), PACK(total_size, 0));

            /* 필요한 만큼 다시 alloc */
            place(ptr, asize);

            /* 남은 free block이 있으면 뒤와 coalesce */
            if (total_size - asize >= MIN_BLOCK_SIZE) {
                void *split_bp = NEXT_BLKP(ptr);
                coalesce(split_bp);
            }

            return ptr;
        }
    }

    /* 3) 불가능하면 새로 할당 후 복사 */
    void *newptr = mm_malloc(size);
    if (newptr == NULL) return NULL;

    size_t old_payload = old_size - OVERHEAD;
    size_t copy_size = MIN(size, old_payload);
    memcpy(newptr, ptr, copy_size);
    mm_free(ptr);

    return newptr;
}

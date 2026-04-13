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

#define MAX_VAL(x, y) ((x) > (y) ? (x) : (y))

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
    size_t size = ALIGN(bytes);
    char *bp = mem_sbrk(size); // 새 시작 주소
    if (bp == (void *) -1) {
        return NULL;
    }

    assert(size % ALIGNMENT == 0); // 정렬된 상태

    PUT_META(HDRP(bp), PACK(size, 0));
    PUT_META(FTRP(bp), PACK(size, 0));
    PUT_META(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    /*
     * mm_init에서의 최초 호출에서 메모리 뷰 찍으면 이렇게 나옴
     * pad           pro hdr       pro ftr       new hdr
     * 00 00 00 00   09 00 00 00   09 00 00 00   00 10 00 00
     * ...
     * new pld       new pld       new ftr       epi hdr
     * 00 00 00 00   00 00 00 00   00 10 00 00   01 00 00 00
     *
     * 00 10 00 00 = 10^12 = 4092
     * 즉, new hdr은 할당되지 않았으며 4092 byte의 공간을 가진다는 의미.
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

    // heap_listp는 prologue payload 포인터
    heap_listp = heap_initp + 2 * META_SIZE;

    if (extend_heap(CHUNK_SIZE) == NULL) {
        return -1;
    }

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    // do nothing
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
    // do nothing
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *) ((char *) oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

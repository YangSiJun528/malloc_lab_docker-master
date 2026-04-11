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
#define CHUNK_SIZE (1 << 12) // 힙을 한 번 늘릴 때 사용하는 크기

#define MAX_VAL(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc)) // 블록 크기와 할당 비트를 하나의 워드에 저장

#define GET(p) (*(unsigned int *)(p)) // 워드 읽기
#define PUT(p, val) (*(unsigned int *)(p) = (val)) // 워드 쓰기

#define GET_SIZE(p) (GET(p) & ~0x7) // 블록 크기 추출
#define GET_ALLOC(p) (GET(p) & 0x1) // 할당 비트 추출

#define HDRP(bp) ((char *)(bp) - META_SIZE) // payload 포인터로부터 header 주소를 계산
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - OVERHEAD) // payload 포인터로부터 footer 주소를 계산

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp))) // 현재 블록 다음 블록의 payload 주소를 계산
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - OVERHEAD)) // 현재 블록 이전 블록의 payload 주소를 계산

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *) -1)
        return NULL;
    else {
        *(size_t *) p = size;
        return (void *) ((char *) p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
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

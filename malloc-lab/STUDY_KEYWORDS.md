# Malloc Lab Study Keywords

설명 없이 키워드만 정리했습니다.

## 가상 메모리 / 페이징

- 가상 메모리
- 물리 메모리
- 가상 주소
- 물리 주소
- 주소 변환
- 페이지
- 페이지 프레임
- 페이지 테이블
- 다단계 페이지 테이블
- TLB
- 페이지 히트
- 페이지 폴트
- locality
- Linux virtual memory system
- demand-zero memory
- memory mapping
- `mmap`
- `fork`
- `execve`

## 동적 메모리 할당 기본

- dynamic memory allocation
- heap
- dynamic storage allocator
- `malloc`
- `free`
- `realloc`
- `sbrk`
- `mem_sbrk`
- system call
- libc `malloc`
- 8-byte alignment
- aligned pointer
- payload
- block
- heap region

## 할당기 목표 / 성능

- allocator requirements and goals
- correctness
- space utilization
- throughput
- performance index
- perf index
- `UTIL_WEIGHT`
- `AVG_LIBC_THRUPUT`
- peak utilization
- optimization
- balanced optimization

## 단편화 / 배치 정책

- fragmentation
- internal fragmentation
- external fragmentation
- first fit
- next fit
- best fit
- placing allocated blocks
- splitting free blocks
- getting additional heap memory
- extend heap

## 블록 구조 / Free List

- implicit free list
- explicit free list
- segregated free list
- free block
- allocated block
- header
- footer
- boundary tag
- coalescing
- coalescing free blocks
- coalescing with boundary tags
- free list pointer
- contiguous free blocks

## Malloc Lab 구현 식별자

- `mm_init`
- `mm_malloc`
- `mm_free`
- `mm_realloc`
- `mm_check`
- `extend_heap`
- `find_fit`
- `place`
- `coalesce`
- 가용리스트 조작 매크로
- pointer arithmetic
- C preprocessor macros
- `ALIGNMENT`
- `ALIGN`
- `SIZE_T_SIZE`
- `mem_heap_lo`
- `mem_heap_hi`
- `mem_heapsize`
- `mem_pagesize`

## 트레이스 / 검증

- trace-driven driver
- trace file
- balanced trace
- `mdriver`
- `valid`
- `-f`
- `-t`
- `-v`
- `-V`
- `-l`
- `short1-bal.rep`
- `short2-bal.rep`
- `amptjp-bal.rep`
- `cccp-bal.rep`
- `cp-decl-bal.rep`
- `expr-bal.rep`
- `coalescing-bal.rep`
- `random-bal.rep`
- `random2-bal.rep`
- `binary-bal.rep`
- `binary2-bal.rep`
- `realloc-bal.rep`
- `realloc2-bal.rep`

## CS:APP 9장 기준 추가 키워드

- VM as a tool for caching
- VM as a tool for memory management
- VM as a tool for memory protection
- DRAM cache organization
- page tables
- page faults
- allocating pages
- integrating caches and VM
- Intel Core i7/Linux memory system
- why dynamic memory allocation?
- implementation issues
- implementing a simple allocator
- garbage collection
- mark-and-sweep
- common memory-related bugs

## 참고 자료

- GitHub Project Week7
- SNU Systems Programming Malloc Lab
- CS:APP Chapter 9
- `malloc-lab/mm.c`
- `malloc-lab/mdriver.c`
- `malloc-lab/config.h`

# Why WSIZE=4 and DSIZE=8 Are OK in This 64-bit Malloc Lab

## 결론

64-bit 컴퓨터에서도 이 과제에서는 `WSIZE = 4`, `DSIZE = 8`로 구현해도 괜찮다.

이유는 `WSIZE`가 CPU의 64-bit word 크기를 뜻한다기보다, allocator가 header/footer 메타데이터를 저장하는 단위로 쓰이기 때문이다.

## 핵심 구분

- `WSIZE = 4`: header/footer 하나의 크기
- `DSIZE = 8`: payload alignment와 최소 정렬 단위
- `sizeof(void *) = 8`: 64-bit 환경의 포인터 크기

이 셋은 같은 값일 필요가 없다.

## Header/Footer는 주소가 아니다

CS:APP 스타일 header/footer에는 보통 다음 정보만 저장한다.

- block size
- allocated/free bit

즉 header/footer는 포인터 주소를 저장하는 공간이 아니다.

따라서 64-bit 포인터 크기인 8바이트와 header/footer 크기 4바이트가 반드시 같을 필요는 없다.

## 왜 4바이트 Header/Footer로 충분한가

이 과제의 heap 크기는 작게 제한되어 있다.

```c
#define MAX_HEAP (20*(1<<20))  /* 20 MB */
```

4바이트 unsigned integer는 약 4GB까지의 값을 표현할 수 있다.

이 과제에서는 전체 heap이 20MB라서 block size가 4GB를 넘을 수 없다. 그래서 block size를 4바이트 header/footer에 저장해도 충분하다.

## Payload는 8-byte aligned 된다

Header가 4바이트여도 사용자가 받는 payload pointer는 8-byte aligned 되도록 배치한다.

예시:

```text
0x1008 ~ 0x100b : previous footer 4B
0x100c ~ 0x100f : current header 4B
0x1010 ~        : current payload start
```

`0x1010`은 8의 배수이므로 payload alignment가 만족된다.

즉 64-bit word 단위로 보면 보통 다음처럼 보인다.

```text
+-----------------------------+
| previous footer | header    |
| 4B              | 4B        |
+-----------------------------+
| payload 8B                  |
+-----------------------------+
| footer          | next hdr  |
| 4B              | 4B        |
+-----------------------------+
| next payload 8B             |
+-----------------------------+
```

버려지는 4바이트가 있는 것이 아니다. 이전 블록의 footer와 다음 블록의 header가 각각 4바이트씩 차지하면서 8바이트 word 하나를 이루는 경우가 많다.

## Explicit Free List에서는 주의

Explicit free list를 구현하면 free block의 payload 안에 `prev`, `next` 포인터를 저장한다.

64-bit 환경에서 포인터는 8바이트이므로, 이 포인터들은 반드시 8바이트 크기로 저장해야 한다.

따라서 최소 free block 크기는 header/footer만 보고 정하면 안 된다.

```c
#define WSIZE 4
#define DSIZE 8
#define OVERHEAD (2 * WSIZE)
#define MIN_BLOCK_SIZE ALIGN(OVERHEAD + 2 * sizeof(void *))
```

64-bit 환경에서는:

```text
header 4B + prev pointer 8B + next pointer 8B + footer 4B = 24B
```

따라서 explicit free list의 최소 블록 크기는 최소 24바이트 이상이어야 한다.

## 과제 밖에서는 그대로 쓰기 어렵다

`WSIZE = 4`, `DSIZE = 8` 구조는 이 과제의 제한 조건 때문에 괜찮다.

일반적인 64-bit allocator에서는 다음 문제가 생길 수 있다.

- 4GB 이상 block size를 표현할 수 없다.
- 모든 블록에 footer를 두면 메모리 오버헤드가 커진다.
- 실제 allocator는 arena, tcache, size class, slab, mmap 같은 구조를 추가로 사용한다.

그래서 실전 allocator에서는 보통 `size_t` 기반 metadata나 별도 allocator-specific metadata 구조를 사용한다.

## 요약

- 이 과제에서는 `WSIZE = 4`, `DSIZE = 8`이 괜찮다.
- `WSIZE`는 CPU word size가 아니라 header/footer metadata 크기로 보면 된다.
- header/footer에는 주소가 아니라 block size와 상태 bit만 저장한다.
- heap이 20MB라서 4-byte size field로 충분하다.
- payload pointer는 여전히 8-byte aligned 되어야 한다.
- explicit free list의 `prev`, `next`는 64-bit 포인터이므로 `sizeof(void *)` 기준으로 다뤄야 한다.
- 과제 밖의 실전 allocator에서는 이 구조를 그대로 쓰기 어렵다.

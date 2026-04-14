# 📘 Docker + VSCode DevContainer 기반 C 개발 환경 구축 가이드 (MallocLab)

이 문서는 **Windows**와 **macOS** 사용자가 Docker와 VSCode DevContainer 기능을 활용하여 C 개발 및 디버깅 환경을 빠르게 구축할 수 있도록 도와줍니다.

[**주의**] 기존 차수와 다른 점만 확인하시면 4장부터 6장만 확인하시면 됩니다.

---

## 1. Docker란 무엇인가요?

**Docker**는 애플리케이션을 어떤 컴퓨터에서든 **동일한 환경에서 실행**할 수 있게 도와주는 **가상화 플랫폼**입니다.  

Docker는 다음 구성요소로 이루어져 있습니다:

- **Docker Engine**: 컨테이너를 실행하는 핵심 서비스
- **Docker Image**: 컨테이너 생성에 사용되는 템플릿 (레시피 📃)
- **Docker Container**: 이미지를 기반으로 생성된 실제 실행 환경 (요리 🍜)

### ✅ AWS EC2와의 차이점

| 구분 | EC2 같은 VM | Docker 컨테이너 |
|------|-------------|-----------------|
| 실행 단위 | OS 포함 전체 | 애플리케이션 단위 |
| 실행 속도 | 느림 (수십 초 이상) | 매우 빠름 (거의 즉시) |
| 리소스 사용 | 무거움 | 가벼움 |

---

## 2. VSCode DevContainer란 무엇인가요?

**DevContainer**는 VSCode에서 Docker 컨테이너를 **개발 환경**처럼 사용할 수 있게 해주는 기능입니다.

- 코드를 실행하거나 디버깅할 때 **컨테이너 내부 환경에서 동작**
- 팀원 간 **환경 차이 없이 동일한 개발 환경 구성** 가능
- `.devcontainer` 폴더에 정의된 설정을 VSCode가 읽어 자동 구성

---

## 3. Docker Desktop 설치하기

1. Docker 공식 사이트에서 설치 파일 다운로드:  
   👉 [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)

2. 설치 후 Docker Desktop 실행  
   - Windows: Docker 아이콘이 트레이에 떠야 함  
   - macOS: 상단 메뉴바에 Docker 아이콘 확인

---

## 4. 프로젝트 파일 다운로드 (히스토리 없이)

터미널(CMD, PowerShell, zsh 등)에서 아래 명령어로 프로젝트 폴더만 내려받습니다:

```bash
git clone --depth=1 https://github.com/krafton-jungle/malloc_lab_docker.git
```

- `--depth=1` 옵션은 git commit 히스토리를 생략하고 **최신 파일만 가져옵니다.**

### 📂 다운로드 후 폴더 구조 설명

```
malloc_lab_docker/
├── .devcontainer/
│   ├── devcontainer.json      # VSCode에서 컨테이너 환경 설정
│   └── Dockerfile             # C 개발 환경 이미지 정의
│
├── .vscode/
│   ├── launch.json            # 디버깅 설정 (F5 실행용)
│   └── tasks.json             # 컴파일 자동화 설정
│
├── malloc-lab
│   ├── short1-bal.rep          # 테스트 케이스
│   ├── Makefile                # 과제를 컴파일하고 테스트하기 위한 파일
│   └── README.md               # malloc-lab 과제 설명
│
└── README.md  # 설치 및 사용법 설명 문서
```

---

## 5. VSCode에서 해당 프로젝트 폴더 열기

1. VSCode를 실행
2. `파일 → 폴더 열기`로 방금 클론한 `malloc_lab_docker` 폴더를 선택

---

## 6. 개발 컨테이너: 컨테이너에서 열기

1. VSCode에서 `Ctrl+Shift+P` (Windows/Linux) 또는 `Cmd+Shift+P` (macOS)를 누릅니다.
2. 명령어 팔레트에서 `Dev Containers: Reopen in Container`를 선택합니다.
3. 이후 컨테이너가 자동으로 실행되고 빌드됩니다. 처음 컨테이너를 열면 빌드하는 시간이 오래걸릴 수 있습니다. 빌드 후, 프로젝트가 **컨테이너 안에서 실행됨**.

---

## 7. C 파일에 브레이크포인트 설정 후 디버깅 (F5)

이제 본격적으로 문제를 풀 시간입니다. `malloc-lab/README.md` 파일을 참조하셔서 rbtree 문제를 풀어보세요.

C 언어로 문제를 풀다가 디버깅이 필요하시면 소스코드에 BreakPoint를 설정한 뒤에 키보드에서 `F5`를 눌러 디버깅을 시작할 수 있습니다.`F5`를 누르면 `malloc-lab`폴더에서 `mdriver -V -f short1-bal.rep` 를 실행하여 테스트 코드를 디버깅 모드로 실행합니다.
- 참고로 변수, 메모리, 스택, 출력 등을 VSCode에서 확인할 수도 있습니다.

---

## 8. 새로운 Git 리포지토리에 Commit & Push 하기

금주 프로젝트를 개인 Git 리포와 같은 다른 리포지토리에 업로드하려면, 기존 Git 연결을 제거하고 새롭게 초기화해야 합니다.

### ✅ 완전히 새로운 Git 리포로 업로드하는 방법

아래 명령어를 순서대로 실행하세요:

```bash
rm -rf .git
git init
git remote add origin https://github.com/myusername/my-new-repo.git
git add .
git commit -m "Clean start"
git push -u origin main
```

### 📌 설명

- `rm -rf .git`: 기존 Git 기록과 연결을 완전히 삭제합니다.
- `git init`: 현재 폴더를 새로운 Git 리포지토리로 초기화합니다.
- `git remote add origin ...`: 새로운 리포지토리 주소를 origin으로 등록합니다.
- `git add .` 및 `git commit`: 모든 파일을 커밋합니다.
- `git push`: 새로운 리포에 최초 업로드(Push)합니다.

이 과정을 거치면 기존 리포와의 연결은 완전히 제거되고, **새로운 독립적인 프로젝트로 관리**할 수 있습니다.

## 🎉 끝

이제 Docker와 DevContainer를 활용한 C 개발 환경이 완성되었습니다.

- (주의) 위 내용은 처음 설치하는 사람을 기준으로 작성된 내용입니다. malloc-lab 폴더에서 있는 프로젝트를 반복적으로 개발할 경우 5에서 7장의 내용만 반복하시면 됩니다.
- 어떤 운영체제에서든 동일한 환경으로 개발 가능  
- VSCode 내에서 코드 작성, 컴파일, 디버깅까지 한 번에 가능

---

# 내 메모

## 테스크 실행하는 법

작업 폴더로 이동

```bash
cd malloc-lab
```

Release로 빌드 후 실행 (현재 Clion 설정은 Debug 빌드만 있음)

```bash
make clean
make 
./mdriver -V
```

## 실행 결과 읽는 법

### 항목 분석

`./mdriver -V`를 실행하면 각 trace별 결과와 전체 점수가 출력됩니다.

```text
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.005317  1071
```

- `trace`: 실행한 trace 파일 번호입니다. 기본 순서는 `malloc-lab/config.h`의 `DEFAULT_TRACEFILES`에 정의되어 있습니다.
- `valid`: 정확성 검사 결과입니다. `yes`면 해당 trace에서 `malloc/free/realloc`이 정상 동작했다는 뜻입니다. 하나라도 `no`면 먼저 correctness 버그를 고쳐야 합니다.
- `util`: 공간 효율입니다. 실행 중 실제 payload 사용량의 최대치 대비 allocator가 사용한 heap 크기의 비율입니다. 높을수록 메모리 낭비가 적습니다.
- `ops`: trace에 들어 있는 요청 수입니다. `malloc`, `free`, `realloc` 호출 횟수의 합입니다.
- `secs`: 해당 trace를 처리하는 데 걸린 시간입니다.
- `Kops`: 초당 몇 천 개의 요청을 처리했는지 나타냅니다. `ops / 1000 / secs`로 계산됩니다.

마지막의 `Perf index`는 최종 성능 점수입니다.

```text
Perf index = 44 (util) + 12 (thru) = 57/100
```

이 설정에서는 공간 효율이 최대 60점, 처리량이 최대 40점입니다. 따라서 `valid`가 전부 `yes`라면 기능적으로는 통과한 것이고, `Perf index`는 그 allocator가 얼마나 효율적인지를 나타내는 성능 점수입니다.

### trace 정리

기본 실행(`./mdriver -V`)에서 사용하는 trace 순서는 `malloc-lab/config.h`의 `DEFAULT_TRACEFILES`와 같습니다.

| 번호 | trace 파일 | 주로 보는 것 |
|------|------------|--------------|
| 0 | `amptjp-bal.rep` | 실제 프로그램에서 나온 할당/해제 패턴입니다. 일반적인 allocator 동작을 봅니다. |
| 1 | `cccp-bal.rep` | 실제 프로그램에서 나온 할당/해제 패턴입니다. 일반적인 allocator 동작을 봅니다. |
| 2 | `cp-decl-bal.rep` | 실제 프로그램에서 나온 할당/해제 패턴입니다. 일반적인 allocator 동작을 봅니다. |
| 3 | `expr-bal.rep` | 실제 프로그램에서 나온 할당/해제 패턴입니다. 일반적인 allocator 동작을 봅니다. |
| 4 | `coalescing-bal.rep` | 인접한 free block을 잘 합치는지 봅니다. 병합이 안 되면 큰 요청을 처리하기 어렵습니다. |
| 5 | `random-bal.rep` | 임의의 malloc/free 패턴입니다. 전반적인 정확성과 안정성을 봅니다. |
| 6 | `random2-bal.rep` | 다른 랜덤 패턴입니다. 특정 순서에만 우연히 맞는 구현인지 확인하는 데 도움이 됩니다. |
| 7 | `binary-bal.rep` | 작은 블록과 큰 블록을 번갈아 할당한 뒤 특정 크기 블록을 다시 요청합니다. first fit 탐색 비용과 외부 단편화 영향을 보기 좋습니다. |
| 8 | `binary2-bal.rep` | `binary-bal.rep`와 비슷하지만 블록 크기와 요청 수가 다릅니다. 선형 탐색 allocator는 여기서 느려지기 쉽습니다. |
| 9 | `realloc-bal.rep` | 같은 블록을 계속 키우는 realloc 패턴입니다. 매번 새로 malloc-copy-free 하면 느리고 공간 효율도 낮아집니다. |
| 10 | `realloc2-bal.rep` | realloc이 섞인 다른 패턴입니다. 기존 데이터 보존과 realloc 처리 방식을 확인합니다. |

파일 이름의 `-bal`은 balanced trace라는 뜻입니다. 할당된 블록마다 대응되는 `free` 요청이 있어서 trace가 끝날 때 정리되는 형태입니다.

## 버전 별 성능

### v1

```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.005891   966
 1       yes   99%    5848  0.004463  1310
 2       yes   99%    6648  0.007786   854
 3       yes  100%    5380  0.005391   998
 4       yes   66%   14400  0.000058249567
 5       yes   92%    4800  0.003847  1248
 6       yes   92%    4800  0.003536  1358
 7       yes   55%   12000  0.084239   142
 8       yes   51%   24000  0.154604   155
 9       yes   27%   14401  0.302199    48
10       yes   34%   14401  0.015306   941
Total          74%  112372  0.587319   191

Perf index = 44 (util) + 13 (thru) = 57/100
```

### v2

```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.003899  1460
 1       yes   99%    5848  0.003256  1796
 2       yes   99%    6648  0.005932  1121
 3       yes  100%    5380  0.004128  1303
 4       yes   66%   14400  0.000060240401
 5       yes   92%    4800  0.003589  1337
 6       yes   92%    4800  0.003302  1454
 7       yes   55%   12000  0.084749   142
 8       yes   51%   24000  0.150730   159
 9       yes   80%   14401  0.000402 35806
10       yes   46%   14401  0.000067214940
Total          80%  112372  0.260114   432

Perf index = 48 (util) + 29 (thru) = 77/100
```

#### AI 피드백

v2에서는 `realloc-bal.rep` 성능이 크게 좋아졌습니다. 기존에는 `realloc`마다 새 블록을 할당하고 복사한 뒤 이전 블록을 해제했지만, v2에서는 다음 free block을 흡수해 제자리 확장을 시도하기 때문입니다.

아직 낮게 나오는 부분은 7번, 8번 trace입니다. `binary-bal.rep`, `binary2-bal.rep`는 작은 블록과 큰 블록을 번갈아 할당한 뒤 일부 큰 블록을 free하고 더 큰 블록을 다시 요청하는 패턴입니다. 현재 구현은 implicit free list + first fit이라 매번 heap 처음부터 선형 탐색합니다. 그래서 앞쪽에 "free이지만 요청보다 작아서 못 쓰는 블록"이 많이 쌓이면 탐색 시간이 길어지고 `Kops`가 낮아집니다.

또한 7번, 8번의 `util`이 낮은 이유는 외부 단편화 때문입니다. 외부 단편화는 free 공간의 총량은 충분하지만, 큰 요청을 담을 만큼 연속된 free block이 없는 상태를 말합니다. 예를 들어 448 byte free block이 여러 개 있어도 사이에 64 byte allocated block이 끼어 있으면 서로 합칠 수 없습니다. 이 상태에서 512 byte 요청이 들어오면 기존 free block들을 재사용하지 못하고 heap을 새로 늘리게 됩니다. 그래서 payload 대비 heap 크기가 커지고 `util`이 낮아집니다.

4번 `coalescing-bal.rep`의 `util`이 낮은 것은 조금 다른 이유입니다. 이 trace는 4095 byte 블록 두 개를 할당했다가 해제한 뒤 8190 byte 블록을 다시 요청하는 패턴입니다. payload 기준으로는 `4095 + 4095 = 8190`이라 딱 맞아 보이지만, allocator 내부에서는 header/footer와 8 byte alignment가 추가됩니다. 4095 byte 요청 하나는 실제로 `ALIGN(4095 + 8) = 4104` byte 블록이 필요합니다. 그런데 현재 기본 heap 확장 단위는 `CHUNK_SIZE = 4096`이므로 4095 byte 요청 하나가 기본 chunk 하나에 딱 들어가지 않습니다. 이 때문에 실제 heap 사용량이 payload보다 커지고 `util`이 낮게 나옵니다.

정리하면 v2의 남은 병목은 `realloc`보다 `first_fit` 탐색 방식입니다. 다음 개선 방향은 `next fit`, explicit free list, segregated free list 순서로 생각할 수 있습니다.

### v3 - 여기서부턴 AI 많이 씀

f2e0eb9185a8d2a0e21b5617c99948a82656eed3 이거 v3 태그 

```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   93%    5694  0.000100 56826
 1       yes   92%    5848  0.000098 59857
 2       yes   97%    6648  0.000124 53441
 3       yes   98%    5380  0.000101 53110
 4       yes   66%   14400  0.000095151899
 5       yes   89%    4800  0.000426 11257
 6       yes   87%    4800  0.000512  9373
 7       yes   55%   12000  0.030521   393
 8       yes   51%   24000  0.044023   545
 9       yes   39%   14401  0.001071 13453
10       yes   31%   14401  0.000133108035
Total          73%  112372  0.077204  1456

Perf index = 44 (util) + 40 (thru) = 84/100
```

### v4

```
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.000083 68192
 1       yes   99%    5848  0.000096 60790
 2       yes   99%    6648  0.000098 67561
 3       yes   99%    5380  0.000079 67929
 4       yes   66%   14400  0.000083174123
 5       yes   96%    4800  0.001545  3106
 6       yes   95%    4800  0.001640  2927
 7       yes   55%   12000  0.027307   439
 8       yes   51%   24000  0.031545   761
 9       yes   28%   14401  0.001498  9616
10       yes   35%   14401  0.000124116137
Total          75%  112372  0.064098  1753

Perf index = 45 (util) + 40 (thru) = 85/100
```
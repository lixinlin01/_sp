# 程式說明

本專案包含五個 C 語言範例，循序漸進學習 POSIX Thread (pthread) 的核心概念。

## 檔案結構

```
program/
├── thread_basic.c       # 基本 thread 操作（建立、等待、傳參數、回傳值）
├── race_condition.c     # 競爭條件（Race Condition）示範
├── mutex_solution.c     # 用 Mutex 解決 Race Condition
├── deadlock.c           # Deadlock 示範（兩個 thread 互相等待）
├── deadlock_fixed.c     # Deadlock 的兩種解法
└── Makefile             # 編譯腳本（`make` 或 `make test`）
```

## 各程式說明

### 1. thread_basic.c — 基本 Thread 操作

- `pthread_create()` — 建立新執行緒，指定要執行的函數與參數
- `pthread_join()` — 等待指定執行緒結束，並可接收回傳值
- 執行緒函數簽名固定為 `void* func(void* arg)`
- 傳遞與回傳皆透過 `void*` 指標，可指向任意型別

### 2. race_condition.c — 競爭條件

- 多個執行緒同時讀寫共享變數 `counter`
- `counter++` 在機器碼層級為 LOAD → ADD → STORE 三個步驟
- 執行緒排程交錯時，可能發生「讀到舊值、覆蓋結果」，導致最終值小於預期
- **重點**：每次執行結果可能不同，即使這次正確也不代表每次都正確

### 3. mutex_solution.c — Mutex 互斥鎖

- `pthread_mutex_lock()` — 取得鎖（若被持有則阻塞等待）
- `pthread_mutex_unlock()` — 釋放鎖
- 臨界區段（critical section）同一時間只有一個執行緒能進入
- 解決 race condition，但過度使用會降低並行效能

### 4. deadlock.c — 死結

- 執行緒 1：lock A → lock B
- 執行緒 2：lock B → lock A
- 雙方各持一鎖，互相等待對方釋放，永遠卡住
- 滿足死結四條件：互斥、持有並等待、不可搶佔、循環等待

### 5. deadlock_fixed.c — 死結解法

- **方法一：固定鎖順序** — 所有人都先 lock A 再 lock B，破壞「循環等待」
- **方法二：trylock + 退讓** — `pthread_mutex_trylock()` 嘗試鎖，失敗則釋放已持有的鎖並重試，破壞「持有並等待」

## 重點整理

| 概念 | 關鍵函數 / 機制 | 一句話說明 |
|------|----------------|-----------|
| Thread | `pthread_create`, `pthread_join` | 建立並行執行的輕量級任務 |
| Race Condition | 無保護的共享變數 | 多執行緒同時讀寫導致結果不確定 |
| Critical Section | `lock` / `unlock` 之間 | 一次只能一個執行緒執行的程式碼區段 |
| Mutex | `pthread_mutex_lock`, `unlock` | 確保互斥進入 critical section |
| Deadlock | 多把鎖順序不一致 | 兩個執行緒互等對方持有的鎖，永久阻塞 |
| trylock | `pthread_mutex_trylock` | 嘗試鎖，失敗立即回傳而非阻塞 |

## 編譯與執行

```bash
cd program
make          # 編譯所有程式
make test     # 編譯並依序執行（deadlock 除外，因其會卡住）
```

單獨執行：

```bash
./thread_basic      # 基本 thread
./race_condition    # race condition（可執行多次觀察變化）
./mutex_solution    # mutex 解決方案
./deadlock_fixed    # 死結解法
```

> **注意**：`deadlock.exe` 會永久卡住，需按 Ctrl+C 中斷。
> **注意**：race condition 在執行緒數量少時不易觸發，可加大 `LOOP_COUNT` 或 `THREAD_COUNT` 觀察。

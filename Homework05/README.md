# Thread、Race Condition、Mutex、Deadlock 學習筆記

## 什麼是 Thread？

Thread（執行緒）是作業系統排程的最小單位，同一行程（process）內的多個 thread 共享記憶體空間，可以並行（多核心）或交錯（單核心）執行。

**重點**：
- 同行程內的 thread 共享**全域變數**、heap 記憶體
- 每個 thread 有自己的**堆疊 (stack)**（區域變數互不干擾）
- 建立 thread 使用 `pthread_create()`，等待結束用 `pthread_join()`

```c
pthread_t t;
pthread_create(&t, NULL, my_function, &arg);  // 建立
pthread_join(t, NULL);                         // 等待
```

---

## Race Condition（競爭條件）

當多個 thread 同時讀寫同一塊共享資料，且執行順序不確定時，結果可能與預期不符，稱為 **race condition**。

**為什麼會發生？**
高階語言的一行程式碼（例如 `balance = balance + 10`）在 CPU 層級是**多個指令**：
```
LOAD  balance → 暫存器
ADD   10       → 暫存器
STORE 暫存器   → balance
```
如果 thread A 做完 LOAD 後，thread B 插入執行完整的 LOAD→ADD→STORE，A 再寫回，就會**覆蓋** B 的結果，導致少加一次。

---

## Critical Section & Mutex

**Critical Section（臨界區段）**：存取共享資源的程式碼區段，同一時間只能有一個 thread 執行。

**Mutex（互斥鎖）**：用來保護 critical section 的工具，確保一次只有一個 thread 進入。

```c
pthread_mutex_lock(&lock);      // 進入臨界區段
balance = balance + 10;          // 受保護的操作
pthread_mutex_unlock(&lock);    // 離開臨界區段
```

- `lock()`：若鎖已被別人持有，則**阻塞等待**
- `unlock()`：釋放鎖，讓等待中的 thread 可以取得

> **對比**：`pthread_mutex_trylock()` 不會阻塞，失敗立刻回傳非零值，可用來避免死結。

---

## Deadlock（死結）

當兩個以上的 thread 互相等待對方持有的資源，導致全部永久阻塞，就是 deadlock。

### 死結四條件（缺一不可）

| 條件 | 說明 | 破壞方式 |
|------|------|---------|
| **互斥** (Mutual Exclusion) | 資源一次只能一個 thread 使用 | 無法避免（鎖的本質） |
| **持有並等待** (Hold and Wait) | thread 拿著一個鎖，同時等待另一個鎖 | 一次拿所有鎖，或拿不到就釋放 |
| **不可搶佔** (No Preemption) | 鎖不能被強制拿走 | 用 trylock 可「假裝搶佔」 |
| **循環等待** (Circular Wait) | A 等 B，B 等 C，C 等 A 形成環狀 | **固定鎖順序**（最常見解法） |

### 避免 Deadlock 的常見方法

1. **固定鎖取得順序**：所有 thread 都依照相同的順序拿鎖（最簡單有效）
2. **trylock 策略**：拿不到就釋放已持有的鎖，等一會再重試
3. **鎖階層 (Lock Hierarchy)**：為鎖編號，只能由小往大拿

---

## 本專案程式一覽

| 程式 | 主題 | 核心機制 | 說明 |
|------|------|---------|------|
| `bank.c` | Race Condition + Mutex | `pthread_mutex_lock/unlock` | 銀行存提款 10 萬次，比較無鎖 vs 有鎖的差異 |
| `producer_consumer.c` | 生產者消費者 | Mutex + Condition Variable | 有限 buffer，滿時生產者等待，空時消費者等待 |
| `dining_philosophers.c` | 哲學家用餐 / Deadlock | 不同拿叉順序 / trylock | 展示死結及兩種修正方案 |

## 編譯與執行

```bash
cd program
make          # 編譯所有程式
make test     # 編譯並依序執行
```

單獨執行：
```bash
./bank                  # 銀行存提款
./producer_consumer     # 生產者消費者
./dining_philosophers   # 哲學家用餐
```

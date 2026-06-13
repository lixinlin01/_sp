/**
 * race_condition.c — 競爭條件 (Race Condition) 示範
 *
 * 學習目標：
 *   1. 理解什麼是 race condition
 *   2. 觀察多個執行緒同時存取共享變數造成的問題
 *   3. 了解「執行順序不確定」導致的非預期結果
 *
 * 編譯方式：
 *   gcc race_condition.c -o race_condition -lpthread
 *
 * 執行多次觀察結果：
 *   ./race_condition
 */

#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 4     // 執行緒數量
#define LOOP_COUNT   10000 // 每個執行緒累加次數

/**
 * 共享全域變數 —— 所有執行緒都會同時讀寫它
 * 這正是 race condition 的根源
 */
int counter = 0;

/**
 * 執行緒執行的函數
 * 沒有使用任何同步機制，單純對 counter 做多次累加
 *
 * 問題所在：
 *   counter++ 在機器碼層級其實是三個步驟：
 *     1. 從記憶體讀取 counter 到暫存器 (LOAD)
 *     2. 將暫存器 +1         (ADD)
 *     3. 寫回記憶體          (STORE)
 *
 *   當兩個執行緒同時執行時，可能發生：
 *     執行緒 A 讀到 counter=5
 *     執行緒 B 讀到 counter=5  (尚未被 A 寫回！)
 *     執行緒 A 寫回 counter=6
 *     執行緒 B 寫回 counter=6  (覆蓋了 A 的結果，少加一次！)
 *
 *   這就是典型的「競爭條件」——結果取決於執行緒的執行時序
 */
void* increment(void* arg) {
    int id = *(int*)arg;
    printf("[執行緒 %d] 開始，將 counter 累加 %d 次\n", id, LOOP_COUNT);

    for (int i = 0; i < LOOP_COUNT; i++) {
        counter = counter + 1;  // 臨界區段 (critical section)
    }

    printf("[執行緒 %d] 結束\n", id);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    printf("=== Race Condition 示範 ===\n");
    printf("執行緒數量: %d，每個累加 %d 次\n", THREAD_COUNT, LOOP_COUNT);
    printf("預期結果: %d\n", THREAD_COUNT * LOOP_COUNT);
    printf("==========================\n\n");

    // 建立所有執行緒
    for (int i = 0; i < THREAD_COUNT; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, increment, &ids[i]);
    }

    // 等待所有執行緒結束
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n=== 結果 ===\n");
    printf("實際 counter 值: %d\n", counter);
    printf("預期 counter 值: %d\n", THREAD_COUNT * LOOP_COUNT);

    if (counter != THREAD_COUNT * LOOP_COUNT) {
        printf("╔══════════════════════════════════════╗\n");
        printf("║  競爭條件發生了！結果不正確！          ║\n");
        printf("╚══════════════════════════════════════╝\n");
    } else {
        printf("（這次碰巧正確，但無法保證每次如此）\n");
    }

    return 0;
}

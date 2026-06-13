/**
 * mutex_solution.c — 使用 Mutex 解決 Race Condition
 *
 * 學習目標：
 *   1. 使用 pthread_mutex_t 保護臨界區段 (critical section)
 *   2. 理解 lock / unlock 的意義
 *   3. 比較與 race_condition.c 的差異
 *
 * 編譯方式：
 *   gcc mutex_solution.c -o mutex_solution -lpthread
 *
 * Mutex (Mutual Exclusion，互斥鎖)：
 *   一次只允許一個執行緒進入臨界區段
 *   其他執行緒若想進入，必須等待直到鎖被釋放
 *
 * 流程：
 *   執行緒 A: ... lock() → [臨界區段] → unlock() ...
 *   執行緒 B: ... lock() → (等待中...) → [臨界區段] → unlock() ...
 */

#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 4
#define LOOP_COUNT   10000

int counter = 0;

/**
 * 全域互斥鎖
 * 所有執行緒共用同一把鎖，才能達到互斥效果
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// 等同於: pthread_mutex_t mutex; pthread_mutex_init(&mutex, NULL);

void* increment(void* arg) {
    int id = *(int*)arg;
    printf("[執行緒 %d] 開始\n", id);

    for (int i = 0; i < LOOP_COUNT; i++) {
        /**
         * lock() — 嘗試取得鎖
         * - 如果鎖是釋放狀態，執行緒立即取得鎖並繼續
         * - 如果鎖已被其他執行緒持有，此執行緒會「阻塞」(block)
         *   直到鎖被釋放
         */
        pthread_mutex_lock(&mutex);

        /**
         * 臨界區段 — 同一時間只有一個執行緒可以執行這裡
         * 對 counter 的操作現在是安全的了
         */
        counter = counter + 1;

        /**
         * unlock() — 釋放鎖
         * 讓其他等待中的執行緒可以取得鎖
         */
        pthread_mutex_unlock(&mutex);

        /**
         * 如果忘了 unlock():
         * - 其他所有執行緒將永遠卡在 lock() 等待
         * - 這就是死結 (deadlock) 的一種形式
         */
    }

    printf("[執行緒 %d] 結束\n", id);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    printf("=== Mutex 解決方案 ===\n");
    printf("用 mutex 保護 counter，確保結果正確\n\n");

    for (int i = 0; i < THREAD_COUNT; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, increment, &ids[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n=== 結果 ===\n");
    printf("實際 counter 值: %d\n", counter);
    printf("預期 counter 值: %d\n", THREAD_COUNT * LOOP_COUNT);

    if (counter == THREAD_COUNT * LOOP_COUNT) {
        printf("✓ 正確！Mutex 成功避免了競爭條件\n");
    } else {
        printf("✗ 錯誤（這不應該發生）\n");
    }

    // 清理 mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}

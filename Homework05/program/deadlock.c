/**
 * deadlock.c — 死結 (Deadlock) 示範
 *
 * 學習目標：
 *   1. 理解死結發生的四個必要條件（均需滿足才會死結）
 *      - 互斥 (Mutual Exclusion)：資源一次只能被一個執行緒使用
 *      - 持有並等待 (Hold and Wait)：執行緒拿著一個鎖，同時等待另一個鎖
 *      - 不可搶佔 (No Preemption)：鎖不能被強制從執行緒手中拿走
 *      - 循環等待 (Circular Wait)：A 等 B，B 等 A，形成環狀
 *   2. 觀察兩個執行緒互相等待的死結現象
 *
 * 編譯方式：
 *   gcc deadlock.c -o deadlock -lpthread
 *
 * 情境說明：
 *   執行緒 1 拿了 lock A，接著想拿 lock B
 *   執行緒 2 拿了 lock B，接著想拿 lock A
 *   → 雙方都在等對方釋放，永遠卡住
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

/**
 * 執行緒 1：先鎖 A，再鎖 B
 */
void* thread_1(void* arg) {
    (void)arg; // 未使用參數
    printf("[執行緒 1] 嘗試取得 lock A...\n");
    pthread_mutex_lock(&lock_a);
    printf("[執行緒 1] 取得 lock A\n");

    // 故意暫停，確保執行緒 2 有機會取得 lock B
    sleep(1);

    printf("[執行緒 1] 嘗試取得 lock B...\n");
    pthread_mutex_lock(&lock_b);
    printf("[執行緒 1] 取得 lock B（這行不會被印出 → 死結）\n");

    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);

    printf("[執行緒 1] 結束\n");
    return NULL;
}

/**
 * 執行緒 2：先鎖 B，再鎖 A
 * 與執行緒 1 的鎖順序相反 → 死結！
 */
void* thread_2(void* arg) {
    (void)arg; // 未使用參數
    printf("[執行緒 2] 嘗試取得 lock B...\n");
    pthread_mutex_lock(&lock_b);
    printf("[執行緒 2] 取得 lock B\n");

    sleep(1);

    printf("[執行緒 2] 嘗試取得 lock A...\n");
    pthread_mutex_lock(&lock_a);
    printf("[執行緒 2] 取得 lock A（這行不會被印出 → 死結）\n");

    pthread_mutex_unlock(&lock_a);
    pthread_mutex_unlock(&lock_b);

    printf("[執行緒 2] 結束\n");
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("=== Deadlock 示範 ===\n");
    printf("執行緒 1: lock A → lock B\n");
    printf("執行緒 2: lock B → lock A\n");
    printf("鎖的順序相反 → 死結！\n\n");

    pthread_create(&t1, NULL, thread_1, NULL);
    pthread_create(&t2, NULL, thread_2, NULL);

    /**
     * 主執行緒會卡在這裡 forever
     * 因為 t1 和 t2 永遠不會結束
     *
     * 如果使用 gdb 或 strace 觀察：
     *   gdb ./deadlock
     *   (gdb) run
     *   Ctrl+C → (gdb) info threads  # 查看所有執行緒狀態
     *   (gdb) thread apply all bt    # 查看所有執行緒的 call stack
     */
    printf("[主程式] 等待執行緒結束（會永遠卡住）...\n");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    /** 以下不會被執行到 */
    printf("[主程式] 結束\n");
    return 0;
}

/**
 * 如何避免 Deadlock？
 *
 * 方法 1：固定鎖的取得順序
 *    所有人都先鎖 A 再鎖 B，就不會發生循環等待
 *
 * 方法 2：使用 pthread_mutex_trylock()
 *    嘗試鎖，如果失敗就釋放已持有的鎖
 *
 * 方法 3：使用單一鎖保護所有資源（降低並行度換取安全性）
 *
 * 方法 4：使用 lock hierarchy（鎖階層）
 *    為每個鎖指定層級，只能由低往高取得
 *
 * 參考：deadlock_fixed.c
 */

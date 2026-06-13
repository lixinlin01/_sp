/**
 * deadlock_fixed.c — 修復 Deadlock 的幾種方法
 *
 * 學習目標：
 *   1. 固定鎖順序法
 *   2. trylock + 退讓法
 *
 * 編譯方式：
 *   gcc deadlock_fixed.c -o deadlock_fixed -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

/* ============================================================
 * 方法 1：固定鎖順序
 *   所有執行緒都用相同順序取得鎖，就不會形成循環等待
 *   這裡統一：先 lock A，再 lock B
 * ============================================================ */

void* thread_1_fixed_order(void* arg) {
    (void)arg;
    printf("[方法1-執行緒1] lock A → lock B\n");
    pthread_mutex_lock(&lock_a);
    sleep(1);
    pthread_mutex_lock(&lock_b);

    printf("[方法1-執行緒1] 成功取得兩個鎖\n");
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
    return NULL;
}

void* thread_2_fixed_order(void* arg) {
    (void)arg;
    printf("[方法1-執行緒2] lock A → lock B (與執行緒1相同順序)\n");
    pthread_mutex_lock(&lock_a);
    sleep(1);
    pthread_mutex_lock(&lock_b);

    printf("[方法1-執行緒2] 成功取得兩個鎖\n");
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
    return NULL;
}

void demo_fixed_order() {
    pthread_t t1, t2;
    printf("\n=== 方法 1：固定鎖順序 ===\n");
    pthread_create(&t1, NULL, thread_1_fixed_order, NULL);
    pthread_create(&t2, NULL, thread_2_fixed_order, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("✓ 成功完成，沒有死結！\n");
}

/* ============================================================
 * 方法 2：trylock + 退讓
 *   用 pthread_mutex_trylock() 嘗試鎖
 *   如果失敗就釋放已持有的鎖，再做一次
 * ============================================================ */

void* thread_trylock_a_then_b(void* arg) {
    int id = *(int*)arg;

    while (1) {
        pthread_mutex_lock(&lock_a);
        printf("[方法2-執行緒%d] 取得 lock A\n", id);

        sleep(1);

        // trylock: 立即回傳，不會阻塞
        if (pthread_mutex_trylock(&lock_b) == 0) {
            printf("[方法2-執行緒%d] 取得 lock B，任務完成\n", id);
            pthread_mutex_unlock(&lock_b);
            pthread_mutex_unlock(&lock_a);
            return NULL;
        }

        // 拿不到 lock B → 釋放 lock A，退讓給其他人
        printf("[方法2-執行緒%d] 拿不到 lock B，釋放 lock A 重試\n", id);
        pthread_mutex_unlock(&lock_a);

        // 短暫等待，避免 busy waiting
        usleep(100000); // 100ms
    }
}

void demo_trylock() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    printf("\n=== 方法 2：trylock + 退讓 ===\n");
    pthread_create(&t1, NULL, thread_trylock_a_then_b, &id1);
    pthread_create(&t2, NULL, thread_trylock_a_then_b, &id2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("✓ 成功完成，沒有死結！\n");
}

int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║   Deadlock 解決方案示範               ║\n");
    printf("╚══════════════════════════════════════╝\n");

    demo_fixed_order();
    demo_trylock();

    pthread_mutex_destroy(&lock_a);
    pthread_mutex_destroy(&lock_b);
    return 0;
}

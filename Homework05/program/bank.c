/**
 * bank.c — 銀行存提款模擬（Race Condition + Mutex 解法）
 *
 * 情境：
 *   一個銀行帳戶，由兩個執行緒同時進行多次存/提款
 *   版本一：無同步 → 競爭條件導致餘額錯誤
 *   版本二：加 Mutex → 餘額正確
 *
 * 學習目標：
 *   1. Race Condition 如何造成金融系統錯誤
 *   2. Mutex 如何保證資料一致性
 *   3. 同一個問題的「錯誤版」與「正確版」對比
 *
 * 編譯：
 *   gcc bank.c -o bank -lpthread
 */

#include <stdio.h>
#include <pthread.h>

#define OPERATIONS 100000  // 每人操作次數

/* ============================================================
 * 共用資料
 * ============================================================ */
int balance = 1000;        // 帳戶初始餘額
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* ============================================================
 * 版本一：沒有 Mutex 保護（會發生 Race Condition）
 * ============================================================ */

void* deposit_unsafe(void* arg) {
    (void)arg;
    for (int i = 0; i < OPERATIONS; i++) {
        balance = balance + 10;  // 每次存入 10 元
        // 這三行機器碼不是 atomic 的：LOAD → ADD → STORE
    }
    return NULL;
}

void* withdraw_unsafe(void* arg) {
    (void)arg;
    for (int i = 0; i < OPERATIONS; i++) {
        balance = balance - 10;  // 每次提領 10 元
    }
    return NULL;
}

void run_unsafe() {
    balance = 1000;  // 重設餘額
    pthread_t t1, t2;

    pthread_create(&t1, NULL, deposit_unsafe, NULL);
    pthread_create(&t2, NULL, withdraw_unsafe, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("  存款 %d 次 (+10)，提款 %d 次 (-10)\n", OPERATIONS, OPERATIONS);
    printf("  期望餘額: 1000\n");
    printf("  實際餘額: %d  %s\n", balance,
           balance == 1000 ? "✓ 正確" : "✗ 錯誤 (Race Condition 發生)");
}

/* ============================================================
 * 版本二：加上 Mutex 保護
 * ============================================================ */

void* deposit_safe(void* arg) {
    (void)arg;
    for (int i = 0; i < OPERATIONS; i++) {
        pthread_mutex_lock(&lock);
        balance = balance + 10;   // ─┐ 臨界區段
        pthread_mutex_unlock(&lock); // ─┘
    }
    return NULL;
}

void* withdraw_safe(void* arg) {
    (void)arg;
    for (int i = 0; i < OPERATIONS; i++) {
        pthread_mutex_lock(&lock);
        balance = balance - 10;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void run_safe() {
    balance = 1000;
    pthread_t t1, t2;

    pthread_create(&t1, NULL, deposit_safe, NULL);
    pthread_create(&t2, NULL, withdraw_safe, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("  存款 %d 次 (+10)，提款 %d 次 (-10)\n", OPERATIONS, OPERATIONS);
    printf("  期望餘額: 1000\n");
    printf("  實際餘額: %d  %s\n", balance,
           balance == 1000 ? "✓ 正確 (Mutex 保護成功)" : "✗ 錯誤");
}

int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║       銀行存提款模擬 (Bank)           ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    printf("▶ 版本一：無同步保護\n");
    run_unsafe();

    printf("\n▶ 版本二：Mutex 保護\n");
    run_safe();

    pthread_mutex_destroy(&lock);
    return 0;
}

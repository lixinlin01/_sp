/**
 * producer_consumer.c — 生產者消費者問題
 *
 * 經典同步問題：
 *   生產者（Producer）將資料放入有限的緩衝區（buffer）
 *   消費者（Consumer）從緩衝區取出資料處理
 *   當 buffer 滿時，生產者必須等待
 *   當 buffer 空時，消費者必須等待
 *
 * 使用工具：
 *   - Mutex: 保護 buffer 的存取
 *   - Condition Variable: 讓生產者/消費者在條件不滿足時等待
 *     (pthread_cond_wait / pthread_cond_signal)
 *
 * 編譯：
 *   gcc producer_consumer.c -o producer_consumer -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE  5   // 緩衝區大小
#define NUM_ITEMS    20  // 總共要生產的項目數

/* 環形佇列 (circular buffer) */
int buffer[BUFFER_SIZE];
int count = 0;      // 當前 buffer 中的項目數
int in_index = 0;   // 下一個生產者要放的位置
int out_index = 0;  // 下一個消費者要拿的位置

/* 同步工具 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;  // 生產者等待「不滿」
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;  // 消費者等待「不空」

/* ============================================================
 * 生產者執行緒
 * ============================================================ */
void* producer(void* arg) {
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        /* 如果 buffer 滿了，等待消費者拿走資料 */
        while (count == BUFFER_SIZE) {
            printf("[P] buffer 滿，等待中...\n");
            pthread_cond_wait(&cond_producer, &mutex);
            // pthread_cond_wait 會自動釋放 mutex，醒來時重新取得
        }

        /* 生產一項資料 */
        int item = rand() % 100;          // 隨機產生資料
        buffer[in_index] = item;
        in_index = (in_index + 1) % BUFFER_SIZE;
        count++;

        printf("[P] 生產: %2d  (buffer: %d/%d)\n", item, count, BUFFER_SIZE);

        /* 通知消費者 buffer 有資料了 */
        pthread_cond_signal(&cond_consumer);
        pthread_mutex_unlock(&mutex);

        usleep(100000);  // 模擬生產時間
    }

    return NULL;
}

/* ============================================================
 * 消費者執行緒
 * ============================================================ */
void* consumer(void* arg) {
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        /* 如果 buffer 空的，等待生產者放入資料 */
        while (count == 0) {
            printf("[C] buffer 空，等待中...\n");
            pthread_cond_wait(&cond_consumer, &mutex);
        }

        /* 消費一項資料 */
        int item = buffer[out_index];
        out_index = (out_index + 1) % BUFFER_SIZE;
        count--;

        printf("[C] 消費: %2d  (buffer: %d/%d)\n", item, count, BUFFER_SIZE);

        /* 通知生產者 buffer 有空位了 */
        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex);

        usleep(200000);  // 模擬消費時間（比生產慢，觀察 buffer 堆積）
    }

    return NULL;
}

int main() {
    pthread_t p, c;

    printf("╔══════════════════════════════════════╗\n");
    printf("║    生產者消費者問題 (Producer-Consumer) ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
    printf("Buffer 大小: %d\n", BUFFER_SIZE);
    printf("生產者將產生 %d 項資料\n", NUM_ITEMS);
    printf("消費者將消費 %d 項資料\n", NUM_ITEMS);
    printf("========================================\n\n");

    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    printf("\n✓ 所有資料生產與消費完成！\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_producer);
    pthread_cond_destroy(&cond_consumer);

    return 0;
}

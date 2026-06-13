/**
 * thread_basic.c — 基本執行緒 (Thread) 操作
 *
 * 學習目標：
 *   1. 使用 pthread_create() 建立執行緒
 *   2. 使用 pthread_join() 等待執行緒結束
 *   3. 傳遞參數給執行緒函數
 *   4. 從執行緒回傳值
 *
 * 編譯方式：
 *   gcc thread_basic.c -o thread_basic -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/**
 * 執行緒要執行的函數
 * 參數: arg — 由 pthread_create 傳入的參數
 * 回傳: void* — 可以由 pthread_join 接收
 */
void* print_message(void* arg) {
    int id = *(int*)arg;                        // 將 void* 轉回 int*
    printf("   [執行緒 %d] 開始執行\n", id);

    for (int i = 0; i < 3; i++) {
        printf("   [執行緒 %d] 迴圈第 %d 次\n", id, i + 1);
        sleep(1);                               // 模擬工作（暫停 1 秒）
    }

    printf("   [執行緒 %d] 結束\n", id);

    // 回傳結果給 pthread_join
    int* result = malloc(sizeof(int));
    *result = id * 100;
    return result;                              // 等同 pthread_exit(result);
}

int main() {
    pthread_t t1, t2;                           // 執行緒 ID（本質是 unsigned long）
    int id1 = 1, id2 = 2;

    // 建立執行緒（man pthread_create）
    // 原型: pthread_create(&thread, attr, start_routine, arg)
    printf("[主程式] 建立執行緒 1\n");
    pthread_create(&t1, NULL, print_message, &id1);

    printf("[主程式] 建立執行緒 2\n");
    pthread_create(&t2, NULL, print_message, &id2);

    // 等待執行緒結束，並接收回傳值
    void* ret1;
    void* ret2;
    pthread_join(t1, &ret1);
    pthread_join(t2, &ret2);

    printf("[主程式] 執行緒 1 回傳: %d\n", *(int*)ret1);
    printf("[主程式] 執行緒 2 回傳: %d\n", *(int*)ret2);

    free(ret1);
    free(ret2);

    printf("[主程式] 結束\n");
    return 0;
}

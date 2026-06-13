/**
 * dining_philosophers.c — 哲學家用餐問題（Dining Philosophers）
 *
 * 經典死結問題：
 *   五位哲學家坐在圓桌前，每人面前一盤義大利麵，相鄰兩人之間有一根叉子
 *   哲學家要「思考」或「用餐」
 *   用餐需要拿起左右兩根叉子
 *   如果每人先拿左邊叉子，再拿右邊 → 五人同時拿左叉 → 死結！
 *
 * 本程式展示兩種版本：
 *   版本一：錯誤方式 — 每位哲學家先拿左叉再拿右叉 → 可能死結
 *   版本二：修正方式 — 奇數哲學家先拿左再拿右，偶數先拿右再拿左 → 破壞循環等待
 *
 * 編譯：
 *   gcc dining_philosophers.c -o dining_philosophers -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N 5                     // 哲學家數量（也是叉子數量）

pthread_mutex_t forks[N];       // 每根叉子一把鎖
int deadlock_occurred = 0;

/* ============================================================
 * 版本一：錯誤方式（可能死結）
 *   每位哲學家：拿起左叉 → 拿起右叉 → 用餐 → 放下
 *   如果五人同時拿起左叉，就沒有人能拿起右叉，形成死結
 * ============================================================ */
void* philosopher_bad(void* arg) {
    int id = *(int*)arg;
    int left  = id;               // 左邊叉子索引
    int right = (id + 1) % N;    // 右邊叉子索引

    for (int i = 0; i < 5; i++) {   // 每人用餐 5 次
        printf("  [哲學家 %d] 思考中...\n", id);
        usleep(100000);             // 思考

        printf("  [哲學家 %d] 餓了，拿起左叉 (%d)\n", id, left);
        pthread_mutex_lock(&forks[left]);

        /**
         * 故意延遲，讓所有哲學家都有機會拿到左叉
         * 這樣就能觀察到死結
         */
        usleep(100000);

        printf("  [哲學家 %d] 嘗試拿起右叉 (%d)...\n", id, right);
        if (pthread_mutex_trylock(&forks[right]) != 0) {
            // trylock 失敗 → 避免死結，放開左叉重試
            printf("  [哲學家 %d] 拿不到右叉，放下左叉重試\n", id);
            pthread_mutex_unlock(&forks[left]);
            i--;    // 這次不算，重新嘗試
            usleep(50000);
            continue;
        }

        printf("  [哲學家 %d] 拿到兩根叉子，開始用餐\n", id);
        usleep(100000);             // 用餐

        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
        printf("  [哲學家 %d] 放下叉子\n", id);
    }

    printf("  [哲學家 %d] 吃飽了！\n", id);
    return NULL;
}

/**
 * 模擬死結的版本 — 每個人都用 lock（不 trylock）
 * 運行後會卡住，用 Ctrl+C 中斷
 */
void* philosopher_deadlock(void* arg) {
    int id = *(int*)arg;
    int left  = id;
    int right = (id + 1) % N;

    printf("  [哲學家 %d] 思考中...\n", id);
    usleep(100000);

    printf("  [哲學家 %d] 拿起左叉 (%d)\n", id, left);
    pthread_mutex_lock(&forks[left]);

    usleep(200000);  // 確保每人都有機會拿到左叉

    printf("  [哲學家 %d] 嘗試拿起右叉 (%d) — 如果卡住就是死結！\n", id, right);
    pthread_mutex_lock(&forks[right]);  // 會卡在這裡

    // 以下不會執行到（如果死結發生）
    printf("  [哲學家 %d] 用餐中\n", id);
    usleep(100000);
    pthread_mutex_unlock(&forks[right]);
    pthread_mutex_unlock(&forks[left]);
    printf("  [哲學家 %d] 吃飽了\n", id);
    return NULL;
}

/* ============================================================
 * 版本二：修正方式
 *   策略：奇數哲學家先拿左再拿右，偶數先拿右再拿左
 *   破壞了「循環等待」條件，死結不會發生
 * ============================================================ */
void* philosopher_good(void* arg) {
    int id = *(int*)arg;
    int left  = id;
    int right = (id + 1) % N;

    for (int i = 0; i < 5; i++) {
        printf("  [哲學家 %d] 思考中...\n", id);
        usleep(100000);

        printf("  [哲學家 %d] 餓了\n", id);

        /* 奇偶不同順序，破壞循環等待 */
        if (id % 2 == 1) {
            /* 奇數：左 → 右 */
            pthread_mutex_lock(&forks[left]);
            pthread_mutex_lock(&forks[right]);
        } else {
            /* 偶數：右 → 左 */
            pthread_mutex_lock(&forks[right]);
            pthread_mutex_lock(&forks[left]);
        }

        printf("  [哲學家 %d] 拿到叉子，用餐中 🍝\n", id);
        usleep(100000);

        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
        printf("  [哲學家 %d] 放下叉子\n", id);
    }

    printf("  [哲學家 %d] 吃飽了！\n", id);
    return NULL;
}

/* ============================================================
 * 主程式
 * ============================================================ */
void run_deadlock_demo() {
    printf("\n▶ 版本一：死結示範（會卡住，請按 Ctrl+C 中斷）\n");
    printf("   按下 Enter 繼續...\n");
    getchar();  // 等使用者確認

    pthread_t philosophers[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher_deadlock, &ids[i]);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }
}

void run_good_demo() {
    printf("\n▶ 版本二：修正版（奇偶不同順序，避免死結）\n");

    pthread_t philosophers[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher_good, &ids[i]);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }
    printf("\n✓ 所有哲學家都吃飽了！沒有死結！\n");
}

int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║    哲學家用餐問題 (Dining Philosophers) ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    /* 初始化叉子鎖 */
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    /* 用 trylock 模擬不死結的版本 */
    printf("▶ 版本零：trylock 避險版（不會死結）\n");
    {
        pthread_t philosophers[N];
        int ids[N];
        for (int i = 0; i < N; i++) {
            ids[i] = i;
            pthread_create(&philosophers[i], NULL, philosopher_bad, &ids[i]);
        }
        for (int i = 0; i < N; i++) {
            pthread_join(philosophers[i], NULL);
        }
        printf("✓ 使用 trylock，沒有死結！\n");
    }

    // run_deadlock_demo();  // 預設註解，避免卡住

    run_good_demo();

    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}

/*
 * 01_fork_basic.c — fork 基本範例
 *
 * 學習重點：
 *   - fork() 建立子行程，回傳值區分父/子
 *   - getpid() 取得行程 ID
 *   - wait() 讓父行程等待子行程結束
 *   - 直接使用 write() 系統呼叫輸出 (STDOUT_FILENO=1, STDERR_FILENO=2)
 */
#include <stdio.h>      // snprintf
#include <unistd.h>     // fork, getpid, write, STDOUT_FILENO, STDERR_FILENO
#include <sys/wait.h>   // wait

int main() {
    /*
     * fork() 回傳值：
     *   < 0 → 錯誤
     *   == 0 → 在子行程中
     *   > 0 → 在父行程中，回傳值為子行程 PID
     */
    pid_t pid = fork();

    if (pid < 0) {                          // fork 失敗
        write(STDERR_FILENO, "fork failed\n", 12);
        return 1;
    }

    if (pid == 0) {                         // 子行程
        write(STDOUT_FILENO, "Child process: Hello from child!\n", 33);
        write(STDOUT_FILENO, "Child PID = ", 12);

        char buf[16];
        int len = snprintf(buf, sizeof(buf), "%d\n", getpid()); // 整數轉字串
        write(STDOUT_FILENO, buf, len);                         // 顯示子行程 PID
    } else {                                // 父行程
        write(STDOUT_FILENO, "Parent process: Hello from parent!\n", 35);
        write(STDOUT_FILENO, "Parent PID = ", 13);

        char buf[16];
        int len = snprintf(buf, sizeof(buf), "%d\n", getpid()); // 整數轉字串
        write(STDOUT_FILENO, buf, len);                         // 顯示父行程 PID

        int status;
        wait(&status);                      // 父行程阻塞，直到子行程結束
        write(STDOUT_FILENO, "Parent: Child finished.\n", 24);
    }

    return 0;
}

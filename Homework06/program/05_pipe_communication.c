/*
 * 05_pipe_communication.c — pipe 行程間通訊
 *
 * 學習重點：
 *   - pipe(int fd[2]) 建立單向通訊管道
 *     fd[0] = 讀端, fd[1] = 寫端
 *   - fork 後父子行程共享 fd，需關閉不用的一端
 *   - 子寫入 pipefd[1]，父從 pipefd[0] 讀取
 *   - 單向 (half-duplex)：資料從寫端流向讀端
 */
#include <stdio.h>
#include <string.h>     // strlen
#include <unistd.h>     // pipe, fork, close, read, write, STDOUT/STDERR_FILENO
#include <sys/wait.h>   // wait

int main() {
    int pipefd[2];                          // pipefd[0]=讀, pipefd[1]=寫

    // pipe() 建立管道，成功回傳 0，失敗回傳 -1
    if (pipe(pipefd) < 0) {
        write(STDERR_FILENO, "pipe failed\n", 12);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        write(STDERR_FILENO, "fork failed\n", 12);
        return 1;
    }

    if (pid == 0) {                         // 子行程 — 負責寫入
        close(pipefd[0]);                   // 子行程關閉讀端

        const char *msg = "Hello from child via pipe!";
        write(pipefd[1], msg, strlen(msg)); // 寫入管道

        close(pipefd[1]);                   // 關閉寫端
    } else {                                // 父行程 — 負責讀取
        close(pipefd[1]);                   // 父行程關閉寫端

        char buffer[64];
        ssize_t bytes = read(pipefd[0], buffer, sizeof(buffer) - 1);

        if (bytes > 0) {
            buffer[bytes] = '\0';
            write(STDOUT_FILENO, "Parent received: ", 17);
            write(STDOUT_FILENO, buffer, bytes);
            write(STDOUT_FILENO, "\n", 1);
        }

        close(pipefd[0]);                   // 關閉讀端

        int status;
        wait(&status);                      // 等待子行程結束
    }

    return 0;
}

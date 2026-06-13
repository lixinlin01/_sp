/*
 * 02_fork_exec.c — fork + exec 組合
 *
 * 學習重點：
 *   - fork() + execvp() 建立新行程並載入新程式
 *   - 子行程用 execvp 執行外部指令 (/bin/ls)
 *   - wait() + WIFEXITED / WEXITSTATUS 檢查子行程結束狀態
 *   - exec 成功後不會返回原本程式碼
 */
#include <stdio.h>      // snprintf
#include <unistd.h>     // fork, execvp, write, STDOUT/STDERR_FILENO
#include <sys/wait.h>   // wait, WIFEXITED, WEXITSTATUS

int main() {
    pid_t pid = fork();

    if (pid < 0) {                          // fork 失敗
        write(STDERR_FILENO, "fork failed\n", 12);
        return 1;
    }

    if (pid == 0) {                         // 子行程
        write(STDOUT_FILENO, "Child: about to exec ls\n", 24);

        /*
         * execvp 會把當前行程的程式替換成 /bin/ls
         * args[0] 必須是執行檔名稱（可帶路徑），以 NULL 結尾
         * 若成功，底下程式碼「不會被執行」
         * 若失敗（如指令不存在），才會執行錯誤處理
         */
        char *args[] = {"/bin/ls", "-l", "-a", NULL};
        execvp(args[0], args);

        // 能跑到這裡代表 exec 失敗
        write(STDERR_FILENO, "Child: exec failed\n", 19);
        return 1;
    } else {                                // 父行程
        write(STDOUT_FILENO, "Parent: waiting for child...\n", 29);

        int status;
        wait(&status);                      // 等待子行程結束

        // WIFEXITED: 是否正常結束（exit/return）
        // WEXITSTATUS: 取得子行程回傳值
        if (WIFEXITED(status)) {
            write(STDOUT_FILENO, "Parent: child exited with status ", 33);

            char buf[16];
            int len = snprintf(buf, sizeof(buf), "%d\n", WEXITSTATUS(status));
            write(STDOUT_FILENO, buf, len);
        }
    }

    return 0;
}

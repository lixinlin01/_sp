/*
 * 06_redirect_stdout_to_file.c — 綜合應用：fork + dup2 + exec
 *
 * 學習重點：
 *   - fork + dup2: 子行程先重新導向 stdout 再 exec
 *   - 外部指令的輸出（原本會到終端機）被導向檔案
 *   - 相當於 shell 的 "ls -la > program_output.txt"
 *   - 父行程不受影響，仍可正常輸出到終端機
 */
#include <stdio.h>
#include <unistd.h>     // fork, dup2, execvp, close, write
#include <fcntl.h>      // open, O_*
#include <sys/stat.h>   // S_*
#include <sys/wait.h>   // wait

int main() {
    // 建立輸出檔案
    int fd = open("program_output.txt",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        write(STDERR_FILENO, "open failed\n", 12);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        write(STDERR_FILENO, "fork failed\n", 12);
        return 1;
    }

    if (pid == 0) {                         // 子行程
        dup2(fd, STDOUT_FILENO);            // stdout → 檔案
        close(fd);                          // 可關閉原 fd

        // exec 後行程變成 /bin/ls，而它的 stdout 指向檔案
        char *args[] = {"/bin/ls", "-la", NULL};
        execvp(args[0], args);

        write(STDERR_FILENO, "exec failed\n", 12);
        return 1;
    } else {                                // 父行程
        close(fd);                          // 父行程不用 fd

        int status;
        wait(&status);                      // 等待子結束

        write(STDOUT_FILENO, "Parent: child output saved to program_output.txt\n", 49);
    }

    return 0;
}

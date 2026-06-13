/*
 * 04_dup_redirection.c — dup2 重新導向
 *
 * 學習重點：
 *   - dup() 複製一個 fd，回傳新的 fd，兩者指向同一核心檔案表
 *   - dup2(fd, target_fd) 將 target_fd 重新指向 fd 的檔案表
 *   - 先 dup 備份原 stdout，再 dup2 重新導向，最後還原
 *   - 重新導向後 write(stdout) 實際上會寫入檔案
 *   - 標準檔案代號：stdin=0, stdout=1, stderr=2
 */
#include <stdio.h>
#include <unistd.h>     // dup, dup2, close, write, STDOUT_FILENO, STDERR_FILENO
#include <fcntl.h>      // open, O_*
#include <sys/stat.h>   // S_*

int main() {
    // dup() 複製一份 stdout 的 fd，備份原來的終端機輸出
    int saved_stdout = dup(STDOUT_FILENO);

    int fd = open("redirected_output.txt",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        write(STDERR_FILENO, "open failed\n", 12);
        return 1;
    }

    /*
     * dup2(fd, STDOUT_FILENO) → 把 stdout (1) 重新指向 fd 的檔案
     * 從此以後 write(1, ...) 全部寫入檔案而非終端機
     */
    dup2(fd, STDOUT_FILENO);
    close(fd);                              // 原本的 fd 可關閉，因 stdout 已有指向

    write(STDOUT_FILENO, "This goes to the file, not the terminal!\n", 41);
    write(STDOUT_FILENO, "stdout is now redirected via dup2.\n", 35);

    // 還原 stdout 回終端機：將 stdout (1) 指向備份的 saved_stdout
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    write(STDOUT_FILENO, "This goes back to the terminal.\n", 32);

    unlink("redirected_output.txt");        // 清理暫存檔

    return 0;
}

/*
 * 07_redirect_stdin_from_file.c — 綜合應用：stdin 重新導向
 *
 * 學習重點：
 *   - dup2 也可以重新導向 stdin (STDIN_FILENO=0)
 *   - 子行程從檔案讀取輸入，而非鍵盤
 *   - 相當於 shell 的 "wc -l < input_data.txt"
 *   - 父行程建立輸入檔，子行程消耗它
 */
#include <stdio.h>
#include <unistd.h>     // fork, dup2, execvp, close, read, write
#include <fcntl.h>      // open, O_*
#include <sys/stat.h>   // S_*
#include <sys/wait.h>   // wait

int main() {
    const char *input_file = "input_data.txt";

    // 父行程先建立一個輸入檔案
    int fd_in = open(input_file, O_WRONLY | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR);
    write(fd_in, "hello world\n", 12);
    write(fd_in, "line two\n", 9);
    close(fd_in);

    // 唯讀開啟作為子行程的輸入
    fd_in = open(input_file, O_RDONLY);
    if (fd_in < 0) {
        write(STDERR_FILENO, "open failed\n", 12);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        write(STDERR_FILENO, "fork failed\n", 12);
        return 1;
    }

    if (pid == 0) {                         // 子行程
        dup2(fd_in, STDIN_FILENO);          // stdin → 檔案
        close(fd_in);

        // wc -l 會從 stdin 讀取，而 stdin 已指向檔案
        char *args[] = {"/usr/bin/wc", "-l", NULL};
        execvp(args[0], args);

        write(STDERR_FILENO, "exec failed\n", 12);
        return 1;
    } else {                                // 父行程
        close(fd_in);

        int status;
        wait(&status);

        write(STDOUT_FILENO, "Parent: child counted lines from input file.\n", 45);
    }

    unlink(input_file);                     // 清理暫存檔

    return 0;
}

/*
 * 03_file_io.c — 檔案 I/O (open / read / write / close)
 *
 * 學習重點：
 *   - open() 的三種模式：唯讀、唯寫、建立、截斷
 *   - read() / write() 回傳實際讀寫的 bytes 數
 *   - 用完 fd 必須 close() 釋放
 *   - unlink() 刪除檔案
 *   - fd (file descriptor) 本質上是整數，指向核心的開啟檔案表
 */
#include <stdio.h>      // snprintf
#include <unistd.h>     // read, write, close, unlink, STDOUT/STDERR_FILENO
#include <fcntl.h>      // open, O_* 旗標
#include <sys/stat.h>   // S_* 權限常數

int main() {
    const char *filename = "test_output.txt";
    const char *message = "Hello, this is written using open/write/close!\n";
    char buffer[128];

    /*
     * O_WRONLY | O_CREAT | O_TRUNC：唯寫、不存在則建立、存在則清空
     * 第三參數為建立時的檔案權限 (rw-r--r--)
     */
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {                           // open 失敗回傳 -1
        write(STDERR_FILENO, "open for write failed\n", 22);
        return 1;
    }

    write(STDOUT_FILENO, "Writing to file...\n", 19);
    write(fd, message, 48);                 // 寫入檔案 (48 bytes)

    close(fd);                              // 關閉寫入端 fd

    // 重新以唯讀方式開啟同一檔案
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        write(STDERR_FILENO, "open for read failed\n", 21);
        return 1;
    }

    write(STDOUT_FILENO, "Reading from file:\n", 19);

    // read 回傳實際讀取到的 bytes；-1 代表錯誤，0 代表 EOF
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';               // 加上字串終止符
        write(STDOUT_FILENO, buffer, bytes);// 輸出到終端機
    }

    close(fd);                              // 關閉讀取端 fd

    unlink(filename);                       // 刪除暫存檔案

    return 0;
}

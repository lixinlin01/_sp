# 行程與檔案操作

本目錄包含 7 個 C 語言範例程式，用於學習 UNIX 系統程式設計中的行程管理與檔案 I/O 相關系統呼叫。

---

## 系統呼叫總覽

| 系統呼叫 | 功能 | 常用情境 |
|----------|------|----------|
| `fork()` | 建立子行程 (複製當前行程) | 行程分裂，父子並行執行 |
| `execvp()` | 載入新程式取代當前行程 | 執行外部指令 (ls, wc 等) |
| `wait()` | 等待子行程結束 | 父行程同步 / 避免殭屍行程 |
| `open()` | 開啟或建立檔案，回傳 fd | 檔案存取起點 |
| `close()` | 關閉 fd | 釋放系統資源 |
| `read()` | 從 fd 讀取資料 | 從檔案 / 管道 / 網路讀入 |
| `write()` | 寫入資料到 fd | 寫入檔案 / 終端機 / 管道 |
| `dup2()` | 複製 fd 到指定編號 | 重新導向 stdin/stdout/stderr |
| `pipe()` | 建立單向通訊管道 | 父子行程間傳遞資料 |
| `unlink()` | 刪除檔案 | 清理暫存檔 |

---

## 標準檔案代號

| 代號 | 名稱 | 常數 | 預設指向 |
|------|------|------|----------|
| 0 | stdin | `STDIN_FILENO` | 鍵盤 |
| 1 | stdout | `STDOUT_FILENO` | 終端機 (螢幕) |
| 2 | stderr | `STDERR_FILENO` | 終端機 (螢幕) |

---

## 程式一覽

| # | 檔案 | 使用系統呼叫 | 核心概念 |
|---|------|-------------|----------|
| 01 | `01_fork_basic.c` | `fork`, `wait`, `write`, `getpid` | fork 回傳值區分父子、wait 同步 |
| 02 | `02_fork_exec.c` | `fork`, `execvp`, `wait`, `WEXITSTATUS` | 子行程執行外部指令、結束狀態檢查 |
| 03 | `03_file_io.c` | `open`, `read`, `write`, `close`, `unlink` | fd 基本操作流程 |
| 04 | `04_dup_redirection.c` | `dup`, `dup2`, `close` | dup2 重新導向 stdout 並還原 |
| 05 | `05_pipe_communication.c` | `pipe`, `fork`, `close`, `read`, `write` | 管道單向通訊、fd 關閉時機 |
| 06 | `06_redirect_stdout_to_file.c` | `fork`, `dup2`, `execvp`, `wait` | 模擬 `ls -la > file` |
| 07 | `07_redirect_stdin_from_file.c` | `fork`, `dup2`, `execvp`, `wait` | 模擬 `wc -l < file` |

---

## 編譯與執行

### 編譯

```bash
# 在 Linux / WSL 環境下
cd program
make          # 編譯所有程式
make clean    # 清除編譯產物
```

或手動編譯單一檔案：

```bash
gcc -Wall -Wextra -std=c99 -o 01_fork_basic 01_fork_basic.c
```

### 執行

```bash
./01_fork_basic
./02_fork_exec
./03_file_io
# ... 以此類推
```

> **注意**：這些程式使用 POSIX API (`fork`, `execvp`, `pipe` 等)，僅能在 Linux、WSL、macOS 或 Cygwin 等 UNIX-like 環境下編譯執行。Windows 原生 (MinGW) 不支援。

---

## 學習重點整理

### 1. fork() 行程分裂

- `fork()` 一次呼叫、兩次返回。
- 父行程收到子行程的 PID (正整數)，子行程收到 0。
- 父子行程共享 **fork 前的 fd**，但各自擁有獨立的位址空間。
- 子行程結束後若父行程未 `wait()`，會變成**殭屍行程 (zombie)**。

### 2. exec() 系列 — 載入新程式

- `execvp(file, argv)` 從 `PATH` 環境變數搜尋 `file`。
- `argv[0]` 必須是程式名稱 (可帶路徑)，陣列最後必須是 `NULL`。
- exec 成功後，原行程的程式碼被完全取代，**不會返回**。
- 若 exec 失敗 (如指令不存在)，才會執行後續錯誤處理。

### 3. 檔案描述子 (fd)

- fd 是非負整數，核心用來識別開啟的檔案 / 管道 / socket。
- 0=stdin, 1=stdout, 2=stderr 在程式啟動時自動開啟。
- `open()` 回傳的 fd 通常從 3 開始。

### 4. dup2() 與重新導向

- `dup2(old_fd, new_fd)`：關閉 `new_fd`，再將它指向與 `old_fd` 相同的核心檔案表。
- 可將 stdout (1) 或 stdin (0) 重新指向任意檔案或管道。
- 結合 `fork + dup2 + exec` 即可實作 shell 的 `>` 與 `<` 重新導向。

### 5. pipe() 行程間通訊

- `int fd[2]`：`fd[0]` 讀端、`fd[1]` 寫端。
- fork 後父子行程共享 pipe fd，**必須關閉不需要的一端**。
- pipe 是單向的 (half-duplex)，資料從寫端流向讀端。
- 當所有寫端關閉時，讀端的 `read()` 回傳 0 (EOF)。

### 6. 關閉 fd 的重要性

- pipe 範例中若未關閉不用的 fd，可能導致：
  - 讀端 read 永遠阻塞 (還有其他寫端未關)
  - 行程用盡 fd 上限
  - 資源洩漏 (resource leak)

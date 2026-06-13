# 第六章：進階 I/O 模型與多工處理

## 6.1 阻塞 I/O

預設情況下，Socket 操作是阻塞的（Blocking）：
- `read()` / `recv()`：若接收緩衝區無資料，行程進入睡眠直到資料到達
- `write()` / `send()`：若發送緩衝區已滿，行程進入睡眠直到空間釋放
- `accept()`：若無新連線，行程進入睡眠直到有連線請求
- `connect()`：等待 TCP 三方交握完成

## 6.2 非阻塞 I/O

將 Socket 設為非阻塞模式：
```c
int flags = fcntl(sockfd, F_GETFL, 0);
fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
```

非阻塞模式下，若操作無法立即完成，會傳回 -1 並設定 `errno` 為 `EAGAIN` 或 `EWOULDBLOCK`。

## 6.3 I/O 多工模型

### 6.3.1 `select()`
```c
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(server_fd, &read_fds);

struct timeval tv = { .tv_sec = 5, .tv_usec = 0 };

int ret = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
if (ret > 0 && FD_ISSET(server_fd, &read_fds)) {
    client_fd = accept(server_fd, ...);
}
```
**缺點**：
- 最大檔案描述子限制（通常 1024）
- 每次呼叫需重新設定 `fd_set`
- O(n) 掃描所有描述子

### 6.3.2 `poll()`
```c
struct pollfd fds[MAX_CLIENTS];
fds[0].fd = server_fd;
fds[0].events = POLLIN;

int ret = poll(fds, nfds, timeout);
if (fds[0].revents & POLLIN) {
    client_fd = accept(server_fd, ...);
}
```
**優點**：無 1024 上限，不需重新設定事件。
**缺點**：仍為 O(n) 掃描。

### 6.3.3 `epoll`（Linux）
```c
int epfd = epoll_create1(0);

struct epoll_event ev;
ev.events = EPOLLIN;
ev.data.fd = server_fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

struct epoll_event events[MAX_EVENTS];
while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
        if (events[i].data.fd == server_fd) {
            client_fd = accept(server_fd, ...);
        } else {
            // 處理客戶端資料
        }
    }
}
```
**優點**：
- O(1) 複雜度，只傳回就緒的描述子
- 支援邊緣觸發（Edge-Triggered, ET）與水平觸發（Level-Triggered, LT）
- 可擴展至數萬個連線

### 6.3.4 IOCP（Windows）
Windows 平台使用 I/O 完成埠（I/O Completion Ports）實現高效非同步 I/O。應用程式將 I/O 操作提交後可繼續執行其他工作，作業系統在操作完成時將結果放入完成佇列。

## 6.4 非同步 I/O

### 6.4.1 POSIX AIO
```c
struct aiocb cb;
cb.aio_fildes = fd;
cb.aio_buf = buffer;
cb.aio_nbytes = BUFFER_SIZE;
cb.aio_offset = 0;
aio_read(&cb);

// 檢查完成狀態
while (aio_error(&cb) == EINPROGRESS) {
    // 做其他事情
}
ssize_t ret = aio_return(&cb);
```

## 6.5 反應器模式（Reactor Pattern）

I/O 多工搭配事件處理的經典設計模式：
1. 註冊檔案描述子與對應的事件處理器
2. 事件迴圈等待 I/O 事件
3. 事件發生時呼叫對應的處理器

## 6.6 本章小結

從阻塞 I/O 到 epoll / IOCP，不同的 I/O 模型適用於不同的場景與平台的。對於需要支援大量並行連線的高效能伺服器，epoll（Linux）與 IOCP（Windows）是業界標準選擇。選擇正確的 I/O 模型是建構高效能網路服務的關鍵決策。

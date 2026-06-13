# 第四章：TCP 伺服器與客戶端實作

## 4.1 反覆式 TCP 伺服器

最基本的伺服器模型：一次只服務一個客戶端。

```c
// 虛擬碼
server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, ...);
listen(server_fd, BACKLOG);

while (1) {
    client_fd = accept(server_fd, ...);
    // 處理客戶端請求
    handle_client(client_fd);
    close(client_fd);
}
```

**缺點**：若某個客戶端長時間佔用，其他客戶端必須排隊等待。

## 4.2 並行 TCP 伺服器

### 4.2.1 多行程模型（fork）
```c
while (1) {
    client_fd = accept(server_fd, ...);
    if (fork() == 0) {
        // 子行程
        close(server_fd);
        handle_client(client_fd);
        close(client_fd);
        exit(0);
    }
    close(client_fd);  // 父行程關閉客戶端描述子
}
```
**注意**：需處理 SIGCHLD 訊號避免殭屍行程。

### 4.2.2 多執行緒模型
```c
void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    // 處理客戶端
    close(client_fd);
    return NULL;
}

while (1) {
    client_fd = accept(server_fd, ...);
    pthread_create(&tid, NULL, handle_client, &client_fd);
    pthread_detach(tid);  // 自動回收資源
}
```

### 4.2.3 I/O 多工模型
使用 `select()`、`poll()` 或 `epoll` 在單一行程中同時監控多個檔案描述子。

## 4.3 TCP 客戶端實作範例

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

// 收送資料
send(sockfd, request, strlen(request), 0);
recv(sockfd, buffer, sizeof(buffer) - 1, 0);

close(sockfd);
```

## 4.4 邊界條件處理

### 4.4.1 部分讀寫
TCP 是串流協定，`send()` 和 `recv()` 不一定一次收送完整資料。應使用迴圈確保所有資料都已處理：

```c
ssize_t send_all(int fd, const void *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = send(fd, buf + total, len - total, 0);
        if (n < 0) return -1;
        total += n;
    }
    return total;
}
```

### 4.4.2 訊息邊界界定
由於 TCP 不保留訊息邊界，應用層需自行定義協定：
- **固定長度**：每個訊息長度固定
- **長度前綴**：先發送長度，再發送內容
- **分隔符號**：使用特定字元（如換行）標示訊息結束

## 4.5 Socket 選項

| 選項 | 層級 | 功能 |
|------|------|------|
| SO_REUSEADDR | SOL_SOCKET | 允許重用 TIME_WAIT 狀態的位址 |
| SO_KEEPALIVE | SOL_SOCKET | 定時發送保活探測封包 |
| TCP_NODELAY | IPPROTO_TCP | 停用 Nagle 演算法，降低延遲 |
| SO_LINGER | SOL_SOCKET | 控制 close() 時未送出資料的處理方式 |
| SO_RCVBUF | SOL_SOCKET | 設定接收緩衝區大小 |
| SO_SNDBUF | SOL_SOCKET | 設定傳送緩衝區大小 |

## 4.6 本章小結

本章實作了從反覆式到並行式的 TCP 伺服器，涵蓋多行程、多執行緒與 I/O 多工等多種模型，並探討了部分讀寫、訊息邊界定義等實務要點。這些模式是開發高可靠網路服務的基礎。

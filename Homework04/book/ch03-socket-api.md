# 第三章：Socket API 程式設計

## 3.1 Socket 類型

| 類型 | 協定 | 特性 |
|------|------|------|
| SOCK_STREAM | TCP | 可靠、有序、雙向位元組串流 |
| SOCK_DGRAM | UDP | 無連線、不可靠、保留訊息邊界 |
| SOCK_RAW | IP | 直接存取 IP 層，需 root 權限 |

## 3.2 位址結構

### 3.2.1 `sockaddr_in`（IPv4）
```c
struct sockaddr_in {
    sa_family_t    sin_family;  // AF_INET
    in_port_t      sin_port;    // 網路位元組序的埠號
    struct in_addr sin_addr;    // IPv4 位址
    char           sin_zero[8]; // 填充
};
```

### 3.2.2 `sockaddr_in6`（IPv6）
```c
struct sockaddr_in6 {
    sa_family_t     sin6_family;   // AF_INET6
    in_port_t       sin6_port;     // 網路位元組序的埠號
    uint32_t        sin6_flowinfo; // 流量標籤
    struct in6_addr sin6_addr;     // IPv6 位址
    uint32_t        sin6_scope_id; // 範圍 ID
};
```

## 3.3 基本 Socket 系統呼叫

### 3.3.1 `socket()`
建立新的通訊端點，傳回檔案描述子。
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

### 3.3.2 `bind()`
將 Socket 綁定到特定 IP 位址與埠號。
```c
bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
```

### 3.3.3 `listen()`
將 Socket 設為被動監聽模式，僅用於 TCP 伺服器。
```c
listen(sockfd, BACKLOG);  // BACKLOG 為連線佇列長度
```

### 3.3.4 `connect()`
客戶端發起連線請求，指定伺服器位址。
```c
connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

### 3.3.5 `accept()`
伺服器接受連線，產生新的 Socket 描述子用於後續通訊。
```c
int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
```

## 3.4 資料傳輸

### 3.4.1 `send()` / `recv()`
```c
ssize_t n = send(sockfd, buf, len, flags);
ssize_t n = recv(sockfd, buf, len, flags);
```

### 3.4.2 `write()` / `read()`
```c
ssize_t n = write(sockfd, buf, len);
ssize_t n = read(sockfd, buf, len);
```

### 3.4.3 `sendto()` / `recvfrom()`（UDP）
```c
sendto(sockfd, buf, len, flags, (struct sockaddr *)&dest, addrlen);
recvfrom(sockfd, buf, len, flags, (struct sockaddr *)&src, &addrlen);
```

## 3.5 位址解析與轉換

### 3.5.1 `getaddrinfo()`
現代化位址解析函式，同時支援 IPv4 與 IPv6。
```c
struct addrinfo hints, *res;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;   // IPv4 或 IPv6
hints.ai_socktype = SOCK_STREAM;
getaddrinfo("example.com", "http", &hints, &res);
```

### 3.5.2 `inet_pton()` / `inet_ntop()`
將點分十進制字串與二進制位址互相轉換，同時支援 IPv4 與 IPv6。

## 3.6 錯誤處理

常見 Socket 錯誤碼及其意義：
- `EAGAIN` / `EWOULDBLOCK`：非阻塞模式下無資料可用
- `ECONNREFUSED`：連線被拒絕
- `ECONNRESET`：對端強行關閉連線
- `ETIMEDOUT`：連線逾時
- `EADDRINUSE`：位址已被佔用（可使用 SO_REUSEADDR 解決）

## 3.7 本章小結

Socket API 是網路程式設計的基石。本章涵蓋了 Socket 的建立、綁定、監聽、連線、收送資料等核心操作，以及位址結構與轉換函式的使用方式。下一章將以這些 API 為基礎，實作完整的 TCP 伺服器與客戶端。

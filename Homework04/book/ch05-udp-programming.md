# 第五章：UDP 程式設計

## 5.1 UDP 特性回顧

UDP（User Datagram Protocol）提供無連線的傳輸服務，其特性決定了適用的場景：

| 特性 | 說明 | 適用場景 |
|------|------|----------|
| 無連線 | 不需交握即可傳送 | DNS、DHCP、SNMP |
| 不可靠 | 不保證送達、不重傳 | 語音通話、串流影音 |
| 保留邊界 | 一次 `sendto` 對應一次 `recvfrom` | 即時遊戲、物聯網感測器 |
| 低開銷 | 標頭僅 8 bytes | 高效能傳輸 |

## 5.2 UDP 伺服器範例

```c
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(12345);
addr.sin_addr.s_addr = INADDR_ANY;
bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

char buffer[BUFFER_SIZE];
struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);

while (1) {
    ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
    buffer[n] = '\0';
    printf("收到：%s\n", buffer);

    // 回送資料（若需要）
    sendto(sockfd, response, strlen(response), 0,
           (struct sockaddr *)&client_addr, addr_len);
}
```

## 5.3 UDP 客戶端範例

```c
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(12345);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

sendto(sockfd, message, strlen(message), 0,
       (struct sockaddr *)&server_addr, sizeof(server_addr));

char buffer[BUFFER_SIZE];
socklen_t addr_len = sizeof(server_addr);
recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
         (struct sockaddr *)&server_addr, &addr_len);
```

## 5.3 可靠 UDP 實作

雖然 UDP 本身不可靠，但可透過應用層機制提升可靠性：

### 5.3.1 超時重傳
```c
struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, ...);
if (n < 0 && errno == EAGAIN) {
    // 超時未收到回覆，進行重傳
    sendto(sockfd, message, msg_len, 0, ...);
}
```

### 5.3.2 序號與確認
在應用層資料中加入序號（Sequence Number）與確認號（ACK），接收端回傳 ACK 告知已收到哪些封包。

### 5.3.3 最佳化策略
- **選擇性確認（Selective ACK）**：只重傳丟失的封包而非全部
- **前向糾錯（FEC）**：傳送冗餘資料，允許部分遺失時仍可還原

## 5.4 UDP vs TCP 效能比較

| 項目 | TCP | UDP |
|------|-----|-----|
| 連線建立延遲 | 需 1 RTT 交握 | 無 |
| 標頭開銷 | 20+ bytes | 8 bytes |
| 吞吐量瓶頸 | 擁塞控制可能限制 | 無內建限制 |
| 適合場景 | Web、Email、檔案傳輸 | DNS、VoIP、串流、遊戲 |

## 5.5 廣播與多播

### 5.5.1 廣播（Broadcast）
向子網路中所有主機發送封包。
```c
int broadcast = 1;
setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
```

### 5.5.2 多播（Multicast）
加入特定多播群組，只接收該群組的封包。
```c
struct ip_mreq mreq;
mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
mreq.imr_interface.s_addr = INADDR_ANY;
setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
```

## 5.6 本章小結

UDP 以其低延遲與簡單性在即時應用中佔有重要地位。本章說明了 UDP 伺服器與客戶端的實作方式，並探討了可靠性增強、廣播/多播等進階主題。選擇 TCP 或 UDP 應根據應用場景的需求權衡。

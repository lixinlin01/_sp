# 第八章：網路安全與效能最佳化

## 8.1 傳輸層安全（TLS/SSL）

### 8.1.1 TLS 交握流程
1. **ClientHello**：客戶端發送支援的 TLS 版本、加密套件清單
2. **ServerHello**：伺服器選擇版本與加密套件，發送憑證
3. **金鑰交換**：客戶端驗證憑證，產生預主金鑰（Pre-Master Secret）
4. **ChangeCipherSpec**：雙方切換至加密通訊
5. **Finished**：確認交握完成

### 8.1.2 OpenSSL 基本用法
```c
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);

SSL *ssl = SSL_new(ctx);
SSL_set_fd(ssl, client_fd);
SSL_accept(ssl);  // TLS 交握

// 使用 SSL_read / SSL_write 取代 read / write
char buf[4096];
SSL_read(ssl, buf, sizeof(buf));
SSL_write(ssl, response, strlen(response));
```

## 8.2 常見網路攻擊與防禦

| 攻擊類型 | 原理 | 防禦措施 |
|----------|------|----------|
| SYN Flood | 大量 SYN 但不完成交握，耗盡連線佇列 | SYN Cookie、縮短超時 |
| DDoS | 分散式大量請求耗盡資源 | 速率限制、CDN、防火牆 |
| Man-in-the-Middle | 攔截通訊並偽造身分 | TLS 憑證驗證、HSTS |
| DNS Spoofing | 偽造 DNS 回應導向惡意站點 | DNSSEC、DoH/DoT |
| Port Scanning | 掃描開放埠尋找弱點 | 關閉不必要的服務、fail2ban |

### 8.2.1 SYN Cookie 實作原理
伺服器不為 SYN 請求分配資源，而是利用序號欄位編碼 TCP 選項資訊。收到 ACK 時驗證 Cookie 合法性後才分配資源。

## 8.3 高效能設計策略

### 8.3.1 事件驅動架構
使用 epoll / kqueue / IOCP 搭配非阻塞 I/O，避免每連線一個執行緒的開銷。

### 8.3.2 零拷貝（Zero-Copy）
```c
// sendfile() 直接從檔案描述子傳送資料，不需經過用戶空間緩衝區
sendfile(out_fd, in_fd, &offset, count);
```

### 8.3.3 記憶體池
預先分配固定大小的記憶體區塊，減少頻繁的 malloc/free 開銷與記憶體碎片。

### 8.3.4 緩衝區管理
- **環形緩衝區（Ring Buffer）**：支援高效的生產者-消費者模型
- **鏈結串列緩衝區**：適合不固定長度的資料塊

## 8.4 速率限制與流量控制

```c
// 簡單的記數器實作
typedef struct {
    time_t start;
    int count;
    int limit;
} RateLimiter;

int rate_limit(RateLimiter *rl, time_t now) {
    if (now - rl->start >= 1) {
        rl->start = now;
        rl->count = 0;
    }
    if (rl->count >= rl->limit) return -1;  // 超過限制
    rl->count++;
    return 0;
}
```

## 8.5 常見最佳化參數調校

### 8.5.1 核心參數
```bash
# /etc/sysctl.conf
net.ipv4.tcp_tw_reuse = 1       # 重用 TIME_WAIT 連線
net.ipv4.tcp_fin_timeout = 30   # 縮短 FIN 超時
net.core.somaxconn = 128        # listen backlog 上限
net.ipv4.tcp_rmem = 4096 87380 6291456  # TCP 接收緩衝區
```

### 8.5.2 Socket 參數
```c
int bufsize = 1024 * 1024;  // 1MB
setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
```

## 8.6 本章小結

網路安全與效能是系統程式中的一體兩面。TLS 加密保護通訊安全，而 SYN Cookie、零拷貝、記憶體池與適當的核心參數調校則能顯著提升伺服器效能。在設計網路應用時，應從系統層面同時考量資安防護與效能最佳化。

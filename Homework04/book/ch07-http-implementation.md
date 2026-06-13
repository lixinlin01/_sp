# 第七章：HTTP 協定與伺服器實作

## 7.1 HTTP 協定基礎

### 7.1.1 HTTP 請求格式
```
GET /index.html HTTP/1.1\r\n
Host: www.example.com\r\n
User-Agent: Mozilla/5.0\r\n
Accept: text/html\r\n
\r\n
```

- **請求行**：方法（GET/POST/PUT/DELETE）+ URI + HTTP 版本
- **標頭（Headers）**：一或多個 `Key: Value` 行
- **空行**：標頭結束標記
- **訊息主體（Body）**：可選，POST/PUT 使用

### 7.1.2 HTTP 回應格式
```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 128\r\n
\r\n
<!DOCTYPE html><html>...
```

- **狀態行**：HTTP 版本 + 狀態碼 + 狀態訊息
- **常見狀態碼**：
  - 1xx：資訊（100 Continue）
  - 2xx：成功（200 OK, 201 Created）
  - 3xx：重定向（301 Moved, 304 Not Modified）
  - 4xx：客戶端錯誤（400 Bad Request, 404 Not Found）
  - 5xx：伺服器錯誤（500 Internal Server Error, 503 Service Unavailable）

## 7.2 簡易 HTTP 伺服器實作

```c
#define RESPONSE "HTTP/1.1 200 OK\r\n" \
                 "Content-Type: text/plain\r\n" \
                 "Content-Length: 13\r\n" \
                 "Connection: close\r\n" \
                 "\r\n" \
                 "Hello, World!"

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET,
                                .sin_port = htons(8080),
                                .sin_addr = INADDR_ANY };
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 10);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        char buf[4096];
        recv(client_fd, buf, sizeof(buf), 0);
        send(client_fd, RESPONSE, strlen(RESPONSE), 0);
        close(client_fd);
    }
}
```

## 7.3 HTTP/1.1 Keep-Alive

HTTP/1.1 預設啟用持久連線（Persistent Connection），允許在同一個 TCP 連線上發送多個請求。

```c
// 回應中加上
"Connection: keep-alive\r\n"
```

伺服器需在迴圈中持續讀取請求，直到收到 `Connection: close` 或連線逾時。

## 7.4 HTTP/2 與 HTTP/3

### 7.4.1 HTTP/2 特性
- **二進位分幀**：取代 HTTP/1.1 的文字格式
- **多路複用**：單一連線上同時傳輸多個資料流
- **伺服器推送**：伺服器可主動發送資源
- **標頭壓縮**：使用 HPACK 壓縮演算法

### 7.4.2 HTTP/3 特性
- 基於 QUIC 協定（使用 UDP 而非 TCP）
- 內建 TLS 1.3 加密
- 0-RTT 連線建立
- 更好的遺失復原機制（無序隊頭阻塞問題）

## 7.5 WebSocket 協定

WebSocket 提供伺服器與客戶端之間的全雙工通訊通道：

1. 客戶端發送 HTTP Upgrade 請求
2. 伺服器回覆 101 Switching Protocols
3. 連線升級為 WebSocket，後續透過幀（Frame）進行雙向通訊

```c
// WebSocket 幀格式
// 1 byte: FIN + Opcode
// 1 byte: Mask + Payload Length
// 0-8 bytes: Extended Length
// 0-4 bytes: Masking Key
// Payload Data
```

## 7.6 本章小結

HTTP 是當代網路應用最廣泛的應用層協定。本章從 HTTP/1.1 的文字協定出發，逐步延伸到 HTTP/2 的二進位多工與 HTTP/3 的 QUIC 架構，並涵蓋了 WebSocket 即時通訊機制。實作 HTTP 伺服器是理解應用層協定的最佳練習。

# 系統程式 — 網路通訊實務

## 書籍摘要

本書以系統程式的角度深入探討網路通訊的理論與實作，共分八章：

| 章節 | 檔案 | 內容簡介 |
|------|------|----------|
| 第一章 | `ch01-network-introduction.md` | 網路通訊概論與系統程式基礎 — OSI/TCP/IP 分層模型、封裝概念、Socket 抽象、主機位元組序 |
| 第二章 | `ch02-tcp-ip-protocol.md` | TCP/IP 協定棧核心機制 — IPv4/IPv6 標頭、TCP 三方交握與四次揮手、可靠傳輸原理、UDP、ICMP 與 ARP |
| 第三章 | `ch03-socket-api.md` | Socket API 程式設計 — socket/bind/listen/connect/accept 系統呼叫、位址結構、資料傳輸函式、getaddrinfo、錯誤處理 |
| 第四章 | `ch04-tcp-server-client.md` | TCP 伺服器與客戶端實作 — 反覆式、多行程、多執行緒伺服器模型、部分讀寫處理、訊息邊界定義、Socket 選項 |
| 第五章 | `ch05-udp-programming.md` | UDP 程式設計 — UDP 伺服器/客戶端範例、可靠性增強（超時重傳、序號確認）、廣播與多播 |
| 第六章 | `ch06-advanced-io-models.md` | 進階 I/O 模型與多工處理 — 阻塞/非阻塞 I/O、select/poll/epoll/IOCP、非同步 I/O、Reactor 模式 |
| 第七章 | `ch07-http-implementation.md` | HTTP 協定與伺服器實作 — HTTP 請求/回應格式、簡易伺服器實作、Keep-Alive、HTTP/2、HTTP/3、WebSocket |
| 第八章 | `ch08-security-optimization.md` | 網路安全與效能最佳化 — TLS/SSL 交握、OpenSSL 實作、常見攻擊防禦、零拷貝、記憶體池、核心參數調校 |

## 學習目標

- 理解 TCP/IP 協定棧底層運作原理
- 熟練使用 Socket API 開發網路應用程式
- 掌握多種 I/O 模型與伺服器架構設計
- 實作 HTTP 伺服器並理解 Web 通訊協定
- 具備網路安全意識與效能調校能力

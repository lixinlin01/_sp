# 第二章：TCP/IP 協定棧核心機制

## 2.1 IP 協定（網際網路協定）

### 2.1.1 IPv4 標頭結構
IPv4 標頭長度通常為 20 bytes，主要欄位包含：
- **版本（Version, 4 bits）**：標示為 IPv4（值為 4）
- **IHL（Internet Header Length, 4 bits）**：標頭長度（以 32-bit 字組為單位）
- **服務類型（Type of Service, 8 bits）**：QoS 優先級
- **總長度（Total Length, 16 bits）**：整個封包長度
- **TTL（Time to Live, 8 bits）**：每經一個路由器減 1，歸零則丟棄
- **協定（Protocol, 8 bits）**：上層協定代號（6=TCP, 17=UDP）
- **來源 IP 與目的 IP（各 32 bits）**

### 2.1.2 IPv6 主要改進
- 位址長度由 32 bits 擴充為 128 bits
- 固定標頭長度 40 bytes，簡化路由器處理
- 內建 IPsec 安全支援
- 取消校驗和（Checksum），減少每跳計算開銷

## 2.2 TCP 協定（傳輸控制協定）

### 2.2.1 TCP 標頭結構
- **來源埠與目的埠（各 16 bits）**
- **序號（Sequence Number, 32 bits）**：標示資料流中的位元組位置
- **確認序號（Acknowledgment Number, 32 bits）**
- **控制旗標（Control Flags, 9 bits）**：SYN、ACK、FIN、RST、PSH、URG 等
- **窗口大小（Window Size, 16 bits）**：流量控制
- **緊急指標（Urgent Pointer, 16 bits）**

### 2.2.2 連線建立：三方交握（Three-Way Handshake）
1. 客戶端發送 SYN（序號 = x）
2. 伺服器回覆 SYN+ACK（序號 = y，確認序號 = x+1）
3. 客戶端發送 ACK（確認序號 = y+1）

### 2.2.3 連線終止：四次揮手（Four-Way Wavehand）
1. 主動方發送 FIN
2. 被動方回覆 ACK
3. 被動方發送 FIN
4. 主動方回覆 ACK，進入 TIME_WAIT 狀態

### 2.2.4 可靠傳輸機制
- **累積確認（Cumulative ACK）**：確認收到某序號之前的所有資料
- **超時重傳（Retransmission Timeout, RTO）**：未收到 ACK 則重送
- **快速重傳（Fast Retransmit）**：收到三個重複 ACK 立即重送
- **滑動窗口（Sliding Window）**：控制傳輸速率與流量

## 2.3 UDP 協定（用戶資料報協定）

UDP 提供無連線、不可靠但低延遲的傳輸服務。標頭僅 8 bytes，包含：
- 來源埠、目的埠（各 16 bits）
- 長度（16 bits）
- 校驗和（16 bits，可選）

## 2.4 ICMP 與 ARP

### 2.4.1 ICMP（網際網路控制訊息協定）
用於傳遞錯誤報告與診斷資訊，如 `ping` 使用 ICMP Echo Request/Reply。

### 2.4.2 ARP（位址解析協定）
將 IP 位址轉換為實體 MAC 位址，運作於區域網路中。

## 2.5 本章小結

本章詳細說明了 TCP/IP 協定棧中 IP、TCP、UDP 等核心協定的封包結構與運作原理。理解這些底層機制是編寫高效網路程式的必要前提。

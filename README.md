# 課程：系統程式 -- 筆記、習題與報告

## 作業總覽

| 作業 | 主題 | 技術棧 | 說明 |
|------|------|--------|------|
| [Homework01](Homework01/) | C 語言編譯器 + 虛擬機 | C | 自訂 mini-language 的編譯器（Lexer + Recursive Descent Parser），產生四元組中介碼（Quadruple IR），並以堆疊框架 VM 執行。支援函式、while 迴圈、if 條件、遞迴呼叫。 |
| [Homework02](Homework02/) | NumScript 迷你語言 + 堆疊機 | Python | 專注數值計算的 DSL，具備 REPL 與檔案執行雙模式。包含正規表達式 Lexer、遞迴下降編譯器（產生 bytecode）、堆疊式虛擬機。支援函式呼叫（sqrt, sin, cos, abs）、冪次運算。 |
| [Homework03](Homework03/) | 天氣查詢 + Todo List 單頁應用 | HTML / CSS / JavaScript | 單檔 PWA，提供 22 個台灣城市按鈕、搜尋框、地理定位功能。動態主題（依天氣/溫度/晝夜變換 10 種配色），整合 Open-Meteo API（免 API Key）。 |
| [Homework04](Homework04/) | 網路通訊教材（8 章） | Markdown | 從 OSI/TCP-IP 模型到 HTTP/3 與 QUIC 的系統性教材。涵蓋 Socket API、TCP/UDP 程式設計、I/O 多工（select/poll/epoll）、HTTP 伺服器實作、TLS 與安全優化。 |
| [Homework05](Homework05/) | 執行緒同步與死結 | C / POSIX pthreads | 三支程式：銀行交易（Race condition vs Mutex）、生產者消費者（Condition Variable）、哲學家聚餐（Deadlock 演示與修復）。 |
| [Homework06](Homework06/) | 行程管理與檔案 I/O | C / POSIX system calls | 七支程式循序演示 UNIX 系統程式設計：fork、exec、wait、open/read/write、dup2 重導向、pipe IPC、Shell 風格的 `>` 與 `<` 重導向。 |
| [mini_WIreshark](mini_WIreshark/) | 原始封包嗅探器（期末專題） | Python / Raw Socket | 使用 AF_PACKET 原始 socket 繞過核心網路堆疊，直接擷取並解析 Ethernet 訊框、IPv4 封包、TCP/UDP 協定。 |

## 各作業詳細內容

### Homework01 — C 語言編譯器 + 虛擬機

- **關鍵檔案：** `compiler.c`（374 行，含 Lexer + Parser + VM）
- **語言：** C
- **核心機制：**
  - 詞法分析：支援識別字、數字、運算子、註解（`//` 與 `/* */`）
  - 語法分析：遞迴下降法，支援 while 迴圈（Backpatching 技術）、if 條件、函式定義與呼叫
  - 中介碼：四元組（op, arg1, arg2, result），共 20 種指令（IMM, ADD, SUB, JMP_F, CALL, RET_VAL 等）
  - 虛擬機：堆疊框架（Frame）管理區域變數、返回位址、參數傳遞，支援遞迴
- **文件：** `while處理與呼叫機制.md` — 說明 while 編譯策略與函式呼叫的執行時期堆疊變化

### Homework02 — NumScript 迷你語言

- **關鍵檔案：** `lexer.py` / `compiler.py` / `vm.py` / `main.py`
- **語言：** Python
- **核心機制：**
  - 雙模式：`python main.py example.ns`（檔案執行）或 `python main.py`（REPL 互動）
  - EBNG 文法：`Expr → Term (+/- Term)*`，`Term → Factor (*/ /% Factor)*`，`Factor → Power (^ Power)*`
  - Bytecode 指令：PUSH, LOAD, STORE, ADD, SUB, MUL, DIV, MOD, POW, CALL, PRINT_EXPR, HALT
  - 內建函式：`sqrt`, `sin`, `cos`, `abs`（對應 Python math 模組）
- **範例：** `example.ns` — 畢氏定理與圓面積計算

### Homework03 — 天氣 + Todo List 網頁應用

- **關鍵檔案：** `weather.html`（673 行單頁應用）
- **語言：** HTML5 / CSS3 / JavaScript (ES2020)
- **功能特色：**
  - 22 個台灣城市快捷按鈕（含 GPS 座標）
  - 城市搜尋（支援中英文，非台灣城市自動查詢 Open-Meteo Geocoding API）
  - 瀏覽器地理定位（Geolocation API）
  - 動態主題：依溫度（>30°C 紅、<-5°C 冰藍）、降水（雨/雪/雷暴）、晝夜切換 10 種漸層
  - 生活建議：隨機產生天氣對應的提示語
  - Todo List：新增/完成/刪除待辦事項，localStorage 持久化儲存
  - 響應式設計：桌面三欄（>860px）↔ 行動版垂直堆疊
  - 毛玻璃視覺效果（backdrop-filter）、浮動動畫、自訂滾動條

### Homework04 — 網路通訊教材

- **路徑：** `book/`，共 8 章
- **語言：** Markdown（含 C 程式碼範例）
- **章節內容：**
  | 章節 | 主題 |
  |------|------|
  | ch01 | 網路通訊概論 — OSI 七層、TCP/IP 四層、封裝/解封裝、Byte Order |
  | ch02 | TCP/IP 協定 — IPv4/IPv6 標頭、TCP 三向交握/UDP、ICMP、ARP |
  | ch03 | Socket API — socket 類型、位址結構、系統呼叫完整列表 |
  | ch04 | TCP 伺服器/客戶端 — 迭代伺服器、fork/thread/epoll 三種並行模型、Partial Read/Write |
  | ch05 | UDP 程式設計 — 可靠度增強、廣播/多播（SO_BROADCAST, IP_ADD_MEMBERSHIP） |
  | ch06 | 進階 I/O 模型 — blocking/non-blocking、select/poll/epoll、IOCP、Reactor Pattern |
  | ch07 | HTTP 實作 — HTTP/1.1 Keep-Alive、HTTP/2 多工、HTTP/3 QUIC、WebSocket |
  | ch08 | 安全與優化 — TLS/SSL、攻擊類型、sendfile 零複製、Ring Buffer、核心參數調校 |

### Homework05 — 執行緒同步

- **關鍵檔案：** `program/bank.c` / `producer_consumer.c` / `dining_philosophers.c`
- **語言：** C（POSIX pthreads）
- **程式列表：**
  | 程式 | 主題 | 關鍵技術 |
  |------|------|----------|
  | `bank.c` | 銀行交易模擬 | mutex 保護共享變數，對比有/無同步的執行結果 |
  | `producer_consumer.c` | 生產者消費者 | mutex + 2 個 condition variable，環形緩衝區（size=5） |
  | `dining_philosophers.c` | 哲學家聚餐 | 三版本：trylock（安全）、locked 死結演示、奇偶順序（修正） |
- **概念涵蓋：** Race condition（含組合語言層級分析）、Critical Section、Mutex（lock/unlock/trylock）、Deadlock 四條件與預防

### Homework06 — 行程管理與檔案 I/O

- **關鍵檔案：** `program/` 下 7 支 C 程式
- **語言：** C（POSIX system calls）
- **程式列表：**
  | 程式 | 主題 | 系統呼叫 |
  |------|------|----------|
  | `01_fork_basic.c` | 基本 fork | `fork()`, `wait()`, `write()`, `getpid()` |
  | `02_fork_exec.c` | fork + exec | `execvp()`, `WIFEXITED/WEXITSTATUS` |
  | `03_file_io.c` | 檔案 I/O | `open()`, `write()`, `read()`, `close()`, `unlink()` |
  | `04_dup_redirection.c` | stdout 重導向 | `dup()`, `dup2()` |
  | `05_pipe_communication.c` | pipe IPC | `pipe()`, 父子行程通訊 |
  | `06_redirect_stdout_to_file.c` | Shell `>` 重導向 | fork + dup2 + execvp（ls -la 輸出至檔案） |
  | `07_redirect_stdin_from_file.c` | Shell `<` 重導向 | fork + dup2 + execvp（wc -l 讀取檔案） |

### mini_WIreshark — 原始封包嗅探器（期末專題）

- **關鍵檔案：** `sniffer.py` / `packet_parser.py`
- **語言：** Python（AF_PACKET Raw Socket）
- **核心機制：**
  - 建立 Raw Socket：`socket(AF_PACKET, SOCK_RAW, ntohs(0x0003))`（需 root）
  - 擷取 Ethernet 訊框 → 檢查 EtherType (0x800) → 解析 IPv4 標頭（version, IHL, TTL, protocol, src/dst IP）
  - 根據 Protocol 解析 TCP（port）或 UDP（port）
  - 使用 `struct.unpack` 處理網路順序（Big-Endian）與位元運算
- **關鍵發現：** Loopback 封包的 MAC 全為零，驗證核心內部回環機制

---

## 編譯與執行

```bash
# Homework01 (C)
cd Homework01 && gcc -o compiler compiler.c && ./compiler

# Homework02 (Python)
cd Homework02 && python main.py example.ns   # 檔案模式
cd Homework02 && python main.py               # REPL 模式

# Homework03 (Web)
# 直接用瀏覽器開啟 Homework03/weather.html

# Homework04 (文件)
# 使用 Markdown 閱讀器開啟 book/*.md

# Homework05 (C, pthread)
cd Homework05/program && make && ./bank

# Homework06 (C)
cd Homework06/program && make && ./01_fork_basic

# mini_WIreshark (Python, 需 root)
cd mini_WIreshark && sudo python sniffer.py
```


欄位 | 內容
-----|--------
學期 | 114 學年下學期
學生 | 李鑫林
學號末兩碼 | 02
教師 | [陳鍾誠](https://www.nqu.edu.tw/educsie/index.php?act=blog&code=list&ids=4)
學校科系 | [金門大學資訊工程系](https://www.nqu.edu.tw/educsie/index.php)
課程教材 | https://github.com/ccc114b/cpu2os

---

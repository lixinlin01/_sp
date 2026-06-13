import struct

# 工具函式：將二進位的 MAC 地址轉換為人類可讀的格式 (例如: AA:BB:CC:DD:EE:FF)
def format_mac_addr(bytes_addr):
    # 將每個 byte 轉成兩位數的十六進位字串，並用冒號連接
    mac_str = map('{:02x}'.format, bytes_addr)
    return ':'.join(mac_str).upper()

# 核心功能：解析乙太網框架
def parse_ethernet_frame(data):
    # 乙太網表頭固定為 14 bytes
    # struct.unpack('! 6s 6s H') 的系統程式魔法：
    # ! : 網路位元組順序 (Network Byte Order, Big-Endian)
    # 6s: 讀取 6 bytes 當作字串 (目標 MAC)
    # 6s: 讀取 6 bytes 當作字串 (來源 MAC)
    # H : 讀取 2 bytes 當作無號整數 (協定類型 EtherType)
    dest_mac, src_mac, proto = struct.unpack('! 6s 6s H', data[:14])
    
    # 回傳：目標MAC, 來源MAC, 協定類型(轉為 Hex 方便辨識), 以及「剩下的封包資料(Payload)」
    return format_mac_addr(dest_mac), format_mac_addr(src_mac), hex(proto), data[14:]

# 新增工具函式：將 4 bytes 的二進位 IP 轉換為人類可讀字串 (例如 192.168.1.1)
def ipv4_to_str(bytes_addr):
    return '.'.join(map(str, bytes_addr))

# 核心功能：解析 IPv4 封包
def parse_ipv4_packet(data):
    # 第一個 byte 包含了「版本 (Version)」與「表頭長度 (Header Length)」
    version_header_length = data[0]
    
    # 系統程式常見技巧：位元運算 (Bitwise Operation)
    # 右移 4 個 bits 取得前 4 碼 (IPv4 的版本號固定為 4)
    version = version_header_length >> 4
    # 和二進位的 00001111 (也就是十進位的 15) 做 AND 運算，取得後 4 碼
    # 表頭長度是以 4 bytes 為單位，所以要乘以 4
    header_length = (version_header_length & 15) * 4
    
    # 利用 struct.unpack 快速跳過我們不需要的欄位，直接精準抓取：
    # ! : 網路位元組順序 (Big-Endian)
    # 8x: 跳過前 8 個 bytes (TOS, 總長度, 識別碼等)
    # B : 讀取 TTL (存活時間, 1 byte)
    # B : 讀取 Protocol (傳輸層協定編號, 1 byte)
    # 2x: 跳過 Checksum (2 bytes)
    # 4s: 讀取來源 IP (4 bytes)
    # 4s: 讀取目的 IP (4 bytes)
    ttl, proto, src, target = struct.unpack('! 8x B B 2x 4s 4s', data[:20])
    
    # 回傳：版本號, 表頭長度, TTL, 協定編號(判斷是 TCP/UDP/ICMP), 來源IP, 目的IP, 剩下的 Payload
    return version, header_length, ttl, proto, ipv4_to_str(src), ipv4_to_str(target), data[header_length:]

# 解析 TCP 區段 (TCP Segment)
def parse_tcp_segment(data):
    # TCP 表頭的前 4 個 bytes 分別是來源 Port 和目的 Port
    # H: 讀取 2 bytes 當作無號整數 (Unsigned Short)
    src_port, dest_port = struct.unpack('! H H', data[:4])
    return src_port, dest_port, data[20:] # 回傳 Port 號與剩下的實際資料

# 解析 UDP 資料報 (UDP Datagram)
def parse_udp_segment(data):
    # UDP 的結構比 TCP 簡單很多
    # 2x: 跳過我們不需要的長度欄位
    src_port, dest_port = struct.unpack('! H H 2x', data[:6])
    return src_port, dest_port, data[8:]
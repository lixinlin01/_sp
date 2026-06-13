# 檔案：sniffer.py
from packet_parser import parse_ethernet_frame, parse_ipv4_packet, parse_tcp_segment, parse_udp_segment
import socket
import os
import sys

def main():
    if os.name == 'nt':
        print("[!] 請確認您已透過 VS Code 左下角連接到 WSL。")
        sys.exit(1)

    print("[*] 啟動微型 Wireshark，正在監聽網路流量 (按 Ctrl+C 停止)...\n")

    try:
        sniffer = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.ntohs(0x0003))

        while True:
            raw_data, addr = sniffer.recvfrom(65535)
            dest_mac, src_mac, eth_proto, eth_payload = parse_ethernet_frame(raw_data)
            
            # 只處理 IPv4 的封包 (EtherType 為 0x800)
            if eth_proto == '0x800':
                # 呼叫我們剛寫好的 IPv4 解析器，把乙太網剩下的 payload 丟進去解剖
                (version, header_length, ttl, proto, src_ip, target_ip, ipv4_payload) = parse_ipv4_packet(eth_payload)
                
                print('\n[+] 乙太網框架 (Ethernet Frame):')
                print(f'    [-] 目標 MAC: {dest_mac}, 來源 MAC: {src_mac}')
                
                print(f'    [+] IPv4 封包 (IPv4 Packet):')
                print(f'        [-] 版本: {version}, 表頭長度: {header_length} bytes, TTL: {ttl}')
                
                # 判斷裡面包的是什麼協定 (1: ICMP, 6: TCP, 17: UDP)
                if proto == 1:
                    proto_name = "ICMP"
                elif proto == 6:
                    proto_name = "TCP"
                elif proto == 17:
                    proto_name = "UDP"
                else:
                    proto_name = str(proto)
                    
                print(f'        [-] 傳輸層協定: {proto_name}')
                print(f'        [-] 來源 IP: {src_ip}')
                print(f'        [-] 目的 IP: {target_ip}')
                

                # --- 新增的傳輸層解析 ---
                if proto == 6: # 如果是 TCP
                    src_port, dest_port, tcp_payload = parse_tcp_segment(ipv4_payload)
                    print(f'        [+] TCP 區段:')
                    print(f'            [-] 來源 Port: {src_port}, 目的 Port: {dest_port}')
                    
                elif proto == 17: # 如果是 UDP
                    src_port, dest_port, udp_payload = parse_udp_segment(ipv4_payload)
                    print(f'        [+] UDP 區段:')
                    print(f'            [-] 來源 Port: {src_port}, 目的 Port: {dest_port}')
                
                # 為了版面乾淨，印完一個完整封包後加一條分隔線
                print('    ' + '-'*50)
                # 為了避免畫面洗太快，一樣先暫停迴圈測試
                # break

    except KeyboardInterrupt:
        print("\n[*] 停止監聽，微型 Wireshark 已關閉。")
        sys.exit(0)
    except Exception as e:
        print(f"[!] 發生未預期的錯誤: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
import socket

def send_udp_message(message, target_ip, target_port, bind_ip=None):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    sock.setsockopt(socket.IPPROTO_IP, socket.IP_TOS, 0x10)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 4096)


    if bind_ip:
        # Bind to a specific interface
        sock.bind((bind_ip, 0))  # Use any available port for sending

    try:
        print(f"Sending message to {target_ip}:{target_port}")
        sock.sendto(message.encode('utf-8'), (target_ip, target_port))
        print("Message sent successfully!")
    finally:
        sock.close()

# Example usage
message = "Hello from Python!"
target_ip = "192.168.64.110"
target_port = 42069
bind_ip = "192.168.64.107"  # Replace with your local IP

send_udp_message(message, target_ip, target_port, bind_ip)

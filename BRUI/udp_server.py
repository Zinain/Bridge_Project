import socket
import json

def udp_server(host='localhost', port=12345):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((host, port))
    print(f"Listening on {host}:{port}...")

    while True:
        data, addr = sock.recvfrom(4096)  # buffer size
        try:
            message = json.loads(data.decode())
            print(f"Received from {addr}: {message}")
        except json.JSONDecodeError:
            print("Invalid JSON received.")

# Run the server
if __name__ == "__main__":
    udp_server()

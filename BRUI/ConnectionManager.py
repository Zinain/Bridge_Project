__package__ = "BRUI"

from datetime import datetime
import socket
import json

import socket
import json
from datetime import datetime

HOST = "0.0.0.0"   # listen on all interfaces
PORT = 5000

def handle_client(conn, addr):
    print(f"New connection from {addr}")
    with conn:
        while True:
            try:
                data = conn.recv(4096)
                if not data:
                    print(f"Client {addr} disconnected")
                    break

                message = json.loads(data.decode())
                print(f"Received from {addr}: {message}")

                # Example response: echo with timestamp
                response = {
                    "status": "OK",
                    "received": message,
                    "timestamp": datetime.now().isoformat()
                }

                conn.sendall(json.dumps(response).encode())

            except json.JSONDecodeError:
                print("Invalid JSON received")
                break
            except Exception as e:
                print(f"Error handling client {addr}: {e}")
                break


def run_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((HOST, PORT))
        server.listen()
        print(f"TCP server running on {HOST}:{PORT}")

        while True:
            conn, addr = server.accept()
            handle_client(conn, addr)


if __name__ == "__main__":
    run_server()


#Setup init
def udp_init():
    data_to_send = {
        "type": "GREET",
        "command": "AKC",
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

#Close bridge command
def udp_close():
    data_to_send = {
        "type": "COMMAND",
        "command": "CLOSEB",
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

#Open bridge command
def udp_open():
    data_to_send = {
        "type": "COMMAND",
        "command": "OPENB",
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

#Reset Command
def udp_reset():
    data_to_send = {
        "type": "COMMAND",
        "command": "RESET",
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

#Switch override
def udp_swtich(toggle):
    data_to_send = {
        "type": "SWITCH",
        "command": toggle,
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

#Lights on
def udp_lighton():
    data_to_send = {
        "type": "COMMAND",
        "command": "LIGHTON",
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

#Lights off
def udp_lightoff():
    data_to_send = {
        "type": "COMMAND",
        "command": "LIGHTOFF",
        "timestamp": datetime.now().isoformat()
    }
    return udp_client(data_to_send)

# Run the client
if __name__ == "__main__":
    data_to_send = {
        "type": "status",
        "content": "Hello, UDP server!",
        "timestamp": "2025-08-05T10:00:00"
    }
    udp_client(data_to_send)

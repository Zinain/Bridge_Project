__package__ = "BRUI"

from datetime import datetime
import socket
import json

def udp_client(message, host='192.168.1.100', port=12345):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    json_data = json.dumps(message)
    sock.sendto(json_data.encode(), (host, port))
    print("Message sent.")

    try:
        data, addr = sock.recvfrom(4096)
        response = json.loads(data.decode())
        print(f"Received response from {addr}: {response}")
        return response
    except socket.timeout:
        print("No response received (timeout).")

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

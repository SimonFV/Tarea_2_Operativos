import socket


class Client:
    def __init__(self, server_ip, server_port):
        self.server_ip = server_ip  # Cambia esta dirección IP por la del servidor
        self.server_port = server_port  # Cambia este puerto por el del servidor
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self):
        try:
            # Conectar al servidor
            self.client_socket.connect((self.server_ip, self.server_port))
            print("Conexión establecida con el servidor.")

        except Exception as e:
            print("Error:", e)

    def disconnect(self):
        self.client_socket.close()

    def send_msg(self, message):
        self.client_socket.send(message.encode())

    def set_IP(self, ip):
        self.server_ip = ip

    def set_PORT(self, port):
        self.server_port = port

    def wait_server_response(self):
        data = ""

        while True:
            data_bytes = self.client_socket.recv(1024)
            if data_bytes:
                data += data_bytes.decode()
                break

        return data

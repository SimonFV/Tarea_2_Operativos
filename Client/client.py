import socket


class Client:
    """
    _summary_ Client class
    """
    def __init__(self, server_ip, server_port):
        """
        _summary_ Constructor of the Client class

        Args:
            server_ip (_type_): IP direction (localhost) 
            server_port (_type_): integer with the port value
        """
        self.server_ip = server_ip  
        self.server_port = server_port  
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self):
        """
        _summary_ Establishes the connection to the server
        """
        try:
            # Connect to the server
            self.client_socket.connect((self.server_ip, self.server_port))
            print("Conexión establecida con el servidor.")

        except Exception as e:
            print("Error:", e)

    def disconnect(self):
        """
        _summary_ Closes the connection to the server
        """
        self.client_socket.close()

    def send_msg(self, message):
        """
        _summary_ Send a message to the server

        Args:
            message (_type_): message text
        """
        self.client_socket.send(message.encode())

    def set_IP(self, ip):
        """
        _summary_ Sets the IP value

        Args:
            ip (_type_): IP value
        """
        self.server_ip = ip

    def set_PORT(self, port):
        """
        _summary_ Sets the port value

        Args:
            port (_type_): PORT value
        """
        self.server_port = port

    def wait_server_response(self):
        """
        _summary_ Wait until the server sends a message

        Returns:
            _type_: the incoming message
        """
        data = ""

        while True:
            data_bytes = self.client_socket.recv(1024)
            if data_bytes:
                data += data_bytes.decode()
                break

        return data

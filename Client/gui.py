from PIL import Image
import client
import time
import tkinter as tk
from tkinter import filedialog


root = tk.Tk()
entry_font = ("Helvetica", 20)
entry1 = tk.Entry(root, font=entry_font)
entry1.insert(0, "1717")
entry2 = tk.Entry(root, font=entry_font)
entry2.insert(0, "127.0.0.1")
_client = client.Client("", "")


def on_button_click():
    """
    _summary_ Action indicating that the connection to the server can be established.
    """
    port = entry1.get()
    ip = entry2.get()
    port = int(port)
    _client.set_IP(ip)
    _client.set_PORT(port)
    _client.connect()


def on_file_button_click():
    """
    _summary_ Used to select images from the file explorer
    """
    file_path = filedialog.askopenfilename()
    if file_path:
        send_image(file_path)


# Connect button
button = tk.Button(root, text="Conectar", command=on_button_click, state=tk.NORMAL)


def validate_entries():
    if entry1.get() and entry2.get():
        button.config(state=tk.NORMAL)  # Activate the button
    else:
        button.config(state=tk.NORMAL)  # Deactivate the button


def init_GUI():
    """
    _summary_ Initialize all the GUI
    """
    root.title("Interfaz")
    root.configure(bg="black")

    # Set window size
    window_width = 500
    window_height = 500
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    x_coordinate = (screen_width - window_width) // 2
    y_coordinate = (screen_height - window_height) // 2
    root.geometry(f"{window_width}x{window_height}+{x_coordinate}+{y_coordinate}")


    label_font = ("Helvetica", 20, "bold")
    label_port = tk.Label(root, text="PORT", fg="white", bg="black", font=label_font)
    label_ip = tk.Label(root, text="IP", fg="white", bg="black", font=label_font)

    entry1.bind("<KeyRelease>", lambda event: validate_entries())
    entry2.bind("<KeyRelease>", lambda event: validate_entries())

    # File selector and button
    file_button = tk.Button(
        root, text="Seleccionar archivo", command=on_file_button_click
    )

    # Organization of widgets in the window
    label_port.pack()
    entry1.pack()
    label_ip.pack()
    entry2.pack()
    button.pack()
    file_button.pack()

    # Start the main loop of the interface
    root.mainloop()


def send_image(path):
    """_summary_ Sends the image to the server

    Args:
        path (_type_): path of the selected image
    """
    
    image = Image.open(path)
    
    pixeles = image.load()
    
    width, height = image.size

    # Send image size
    message = "start," + str(width) + "," + str(height) + "," + path.split("/")[-1]
    _client.send_msg(message)
    print(_client.wait_server_response())

    pixel_string = "pixels,"

    # Access the pixel values and send the values to the server
    for y in range(height):
        for x in range(width):
            pixel = pixeles[x, y]
            if type(pixel) in [list, tuple]:
                pixel_string += (
                    str(pixel[0]) + "," + str(pixel[1]) + "," + str(pixel[2]) + ","
                )
            else:
                pixel_string += str(pixel) + "," + str(pixel) + "," + str(pixel) + ","

            if len(pixel_string) > 4060 or (y == height - 1 and x == width - 1):
                pixel_string = pixel_string[:-1]
                _client.send_msg(pixel_string)
                pixel_string = "pixels,"
                _client.wait_server_response()

    _client.send_msg("end")
    print(_client.wait_server_response())


def main():
    init_GUI()


if __name__ == "__main__":
    main()

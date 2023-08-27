from PIL import Image
import client
import time
import tkinter as tk
from tkinter import filedialog


root = tk.Tk()
entry_font = ("Helvetica", 20)  
entry1 = tk.Entry(root, font=entry_font)
entry2 = tk.Entry(root, font=entry_font)
_client = client.Client("", "")

def on_button_click():
    port = entry1.get()
    ip = entry2.get()
    port = int(port)
    _client.set_IP(ip)
    _client.set_PORT(port)
    _client.connect()

def on_file_button_click():
    file_path = filedialog.askopenfilename()
    if file_path:
        send_image(file_path)
        time.sleep(2)
        print(_client.server_response())
        _client.disconnect()

# Botón de conectar
button = tk.Button(root, text="Conectar", command=on_button_click, state=tk.DISABLED)
def validate_entries():
    if entry1.get() and entry2.get():
        button.config(state=tk.NORMAL)  # Activar el botón
    else:
        button.config(state=tk.DISABLED)  # Desactivar el botón

def init_GUI():
    
    root.title("Interfaz")
    root.configure(bg="black")

    # Establecer el tamaño de la ventana
    window_width = 500
    window_height = 500
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    x_coordinate = (screen_width - window_width) // 2
    y_coordinate = (screen_height - window_height) // 2
    root.geometry(f"{window_width}x{window_height}+{x_coordinate}+{y_coordinate}")

    # Etiquetas con fuentes más grandes
    label_font = ("Helvetica", 20, "bold")  
    label_port = tk.Label(root, text="PORT", fg="white", bg="black", font=label_font)
    label_ip = tk.Label(root, text="IP", fg="white", bg="black", font=label_font)

    entry1.bind("<KeyRelease>", lambda event: validate_entries())
    entry2.bind("<KeyRelease>", lambda event: validate_entries())
    

    # Selector de archivos y botón
    file_button = tk.Button(root, text="Seleccionar archivo", command=on_file_button_click)

    # Organización de widgets en la ventana
    label_port.pack()
    entry1.pack()
    label_ip.pack()
    entry2.pack()
    button.pack()
    file_button.pack()

    # Iniciar el bucle principal de la interfaz
    root.mainloop()

def send_image(path):
    # Abre una imagen
    image = Image.open(path)
    # Obtiene los pixeles de la imagen
    pixeles = image.load()
    # Obtiene las dimensiones de la imagen
    width, height = image.size

    message = "start," + str(width) + "," + str(height)
    print(message)
    _client.send_msg(message)
    # Accede a los valores de los pixeles
    for y in range(height):
        for x in range(width):
            pixel = pixeles[x, y]
            # Aquí puedes hacer lo que quieras con el valor del pixel, por ejemplo, imprimirlo
            # print(f"Pixel en ({x}, {y}): {pixel}")


def main():
    init_GUI()
    


if __name__ == "__main__":
    main()

import tkinter as tk
from tkinter import filedialog

def on_button_click():
    input_text1 = entry1.get()
    input_text2 = entry2.get()
    print("Input 1 (PORT):", input_text1)
    print("Input 2 (IP):", input_text2)

def on_file_button_click():
    file_path = filedialog.askopenfilename()
    if file_path:
        print("Selected File Path:", file_path)
        file_name = file_path.split("/")[-1]  # Obtener solo el nombre del archivo
        print("Selected File Name:", file_name)

# Crear la ventana principal
root = tk.Tk()
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

# Campos de entrada de texto con fuentes más grandes
entry_font = ("Helvetica", 20)  
entry1 = tk.Entry(root, font=entry_font)
entry2 = tk.Entry(root, font=entry_font)

# Botón de acción
button = tk.Button(root, text="Conectar", command=on_button_click)

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

from PIL import Image
import client
import time

_client = client.Client("127.0.0.1", 1717)


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
    _client.connect()
    send_image("images/Resize.jpg")

    time.sleep(2)
    print(_client.server_response())
    _client.disconnect()


if __name__ == "__main__":
    main()

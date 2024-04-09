import socket
import threading
import random

HOST = '127.0.0.1'
PORT = 24925

responses = [
    'Oh, hello there!',
    'Yes, I agree.',
    'Haha, too true!',
    'Come Nerevar, friend or traitor, come. Come and look upon the Heart and Akulakahn, and bring Wraithguard, I have need of it.',
    'To be fair, you have to have a very high IQ to understand Rick and Morty.'
    ]

def handle_connection(sock):
    while 1:
        try:
            msg = sock.recv(4096).decode('utf-8')
            if msg is None:
                sock.close()
                return
        
            send_msg = random.choice(responses)
            sock.sendall(send_msg.encode('utf-8'))
        except UnicodeDecodeError:
            print('Failed to decode received string.')

if __name__ == '__main__':
    listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listen_sock.bind((HOST, PORT))
    listen_sock.listen()

    while 1:
        conn, _ = listen_sock.accept()
        threading.Thread(target=handle_connection, args=(conn,)).start()

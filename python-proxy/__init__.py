# -*- coding: utf-8 -*-
import socket

from proxy import Proxy

# Settings
LOCAL_PORT = 8080       # Port d'écoute
LOCAL_HOST = ""         # Hôte d'écoute
VNC_PORT = 8008         # Port du serveur VNC
VNC_HOST = "127.0.0.1"  # Hôte du serveur VNC

if __name__ == "__main__":
    # Socket d'écoute
    listening_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # On autorise la réutilisation d'addresse
    listening_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # On écoute
    listening_socket.bind((LOCAL_HOST, LOCAL_PORT))
    listening_socket.listen()

    i = 0
    while True:
        # On attend une nouvelle connexion
        # m2p_conn et m2p_addr sont le socket et l'addresse de la connexion "Master to Proxy (m2p)"
        m2p_conn, m2p_addr = listening_socket.accept()
        # On démare un nouveau proxy pour cette connetion et on lui attribue un ID
        Proxy(m2p_conn, m2p_addr, i, VNC_HOST, VNC_PORT)
        i += 1

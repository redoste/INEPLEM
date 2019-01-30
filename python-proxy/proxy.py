# -*- coding: utf-8 -*-
import socket

from pipe import Pipe
from italcpipe import ItalcPipe

class Proxy():
	# Proxy, objet qui s'occupe d'une connection et établie la fermeture de manière propre
	def __init__(self, m2p_conn, m2p_addr, proxy_id, VNC_HOST, VNC_PORT):
		self.proxy_id = proxy_id
		self.m2p_conn = m2p_conn # Connection du master vers le proxy

		# On initialise la connection du proxy vers le vnc
		self.p2v_conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.p2v_conn.connect((VNC_HOST, VNC_PORT))

		# On crée un pipe dans chaque sens
		# On utilise un ItalcPipe pour la connection venant d'Italc
		self.m2v_pipe = ItalcPipe(self.m2p_conn, self.p2v_conn, self)
		self.v2m_pipe = Pipe(self.p2v_conn, self.m2p_conn, self)
		self.m2v_pipe.start()
		self.v2m_pipe.start()

		# Et on informe
		print("New connection from {}:{} ID:{}".format(m2p_addr[0], m2p_addr[1], proxy_id))

	# Applé par l'un des pipe en cas de fermeture
	def close_connection(self):
		# On envois l'ordre de fermeture des thread
		self.m2v_pipe.stop = 1
		self.v2m_pipe.stop = 1

		# Et on ferme les socket
		try:
			self.m2p_conn.close()
			self.p2v_conn.close()
		except socket.error or BrokenPipeError:
			# Peut arriver si l'un des socket est deja fermé
			pass
		# Et on informe
		print("Connection close ID:{}".format(self.proxy_id))

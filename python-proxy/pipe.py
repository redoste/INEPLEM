# -*- coding: utf-8 -*-
import threading
import socket

class Pipe(threading.Thread):
	# Pipe, thread qui établie la liaison d'un socket vers un autre
	def __init__(self, source, dest, proxy):
		threading.Thread.__init__(self)
		self.source = source # Socket ou l'on va lire
		self.dest = dest # Socket ou l'on va écrire
		self.proxy = proxy # Objet pour informer la fermeture d'un socket
		self.stop = 0

	def run(self):
		while self.stop == 0: # Tant que le signal d'arret n'est pas émis
			try:
				buf = self.source.recv(1024) # On lis
				if self.parse_packet(buf): # On teste si le packet doit être parsé
					self.dest.send(buf) # Et on écrit
			except socket.error or BrokenPipeError:
				self.proxy.close_connection() # En cas de fermeture on l'informe
				break

	def parse_packet(self, buf):
		# Retourne toujours True mais peut être réécris par une classe enfant
		return True

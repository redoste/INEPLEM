# -*- coding: utf-8 -*-
import struct

from pipe import Pipe

CHARSET = "utf-16-be"

class ItalcPipe(Pipe):
	# Identique a Pipe mais parse les packets Italc
	def __init__(self, source, dest, proxy):
		Pipe.__init__(self, source, dest, proxy)

	def parse_packet(self, buf):
		if buf != b"\x28":
			# Packet Normal
			return True
		italc_command = {}
		command_length_bytes = self.source.recv(1024)	# Inutilisé

		command_bytes = self.source.recv(1024)
		command = command_bytes.decode(CHARSET)
		italc_command["command"] = command

		args_length_bytes = self.source.recv(1024)
		try:
			args_length = struct.unpack(">I", args_length_bytes)[0]
		except struct.error:
			print("{}".format(args_length_bytes))
			args_length = 3

		args = {}

		for i in range(args_length):
			key_length_bytes = self.source.recv(1024)	# Inutilisé

			key_bytes = self.source.recv(1024)
			try:
				key = key_bytes.decode(CHARSET)
			except UnicodeDecodeError:
				print("Key UTF-16 decode error {}".format(key_bytes))
				key = ""

			value_type_bytes = self.source.recv(1024)
			if value_type_bytes == b"\x00\x00\x00\x0a":
				value_unk = self.source.recv(1024)
				value_len = self.source.recv(1024)
				if value_len == b"\xff\xff\xff\xff": # String vide
					value_len = 0
					value = ""
				else:
					value_bytes = self.source.recv(1024)
					value = value_bytes.decode(CHARSET)
				args[key] = value
			else:
				print("Unknown type {}".format(value_type_bytes))
				args[key] = self.source.recv(1024)
		italc_command["args"] = args

		print("{}".format(italc_command))

		if italc_command["command"] == "GetUserInformation":
			self.send_userinformation()

		return False

	def send_userinformation(self):
		command = "UserInformation".encode(CHARSET)
		command_length = len(command)

		args_len = 2

		args_username = "username".encode(CHARSET)
		args_username_len = len(args_username)

		username = "Hackerman".encode(CHARSET)
		username_len = len(username)

		args_homedir = "homedir".encode(CHARSET)
		args_homedir_len = len(args_homedir)

		homedir = "/dev/null".encode(CHARSET)
		homedir_len = len(homedir)

		packets = [
			b"\x28", # Information, c'est Italc
			struct.pack(">I", command_length),
			command,
			struct.pack(">I", args_len),
			struct.pack(">I", args_username_len),
			args_username,
			b"\x00\x00\x00\x0a", # Information c'est une string
			b"\x00", # Unknown byte
			struct.pack(">I", username_len),
			username,
			struct.pack(">I", args_homedir_len),
			args_homedir,
			b"\x00\x00\x00\x0a", # Information c'est une string
			b"\x00", # Unknown byte
			struct.pack(">I", homedir_len),
			homedir
		]

		for p in packets:
			self.source.send(p)

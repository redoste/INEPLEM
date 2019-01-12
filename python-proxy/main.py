import socket
import threading
import struct

LOCAL_PORT  = 8080
VNC_PORT    = 8008
VNC_HOST    = "127.0.0.1"
LISTEN_HOST = ""

class Pipe(threading.Thread):
    def __init__(self, source_socket, dest_socket, is_from_master):
        threading.Thread.__init__(self)
        self.source_socket = source_socket
        self.dest_socket = dest_socket
        self.is_from_master = is_from_master

        self.processing_italccommand = False
        self.italccommand = {}
    def run(self):
        while True:
            try:
                buf = self.source_socket.recv(4096)
                if not buf: break
                if self.processing_italccommand:
                    self.process_italc(buf)
                if buf == b"\x28" and self.is_from_master:
                    print("ITALC MESSAGE")
                    self.processing_italccommand = True
                    self.italccommand = {}
                if not self.processing_italccommand:
                    self.dest_socket.send(buf)
            except e:
                print("{}".format(e))
                break
            if(self.source_socket.fileno() == -1):
                break
            elif(self.dest_socket.fileno() == -1):
                break
        print("Pipe Broken is_from_master {}".format(self.is_from_master))
        try:
            self.dest_socket.close()
        except:
            pass

        try:
            self.source_socket.close()
        except:
            pass
    def process_italc(self, buf):
        if "command_length" not in self.italccommand:
            self.italccommand["command_length"] = struct.unpack(">I", buf)[0]
            return True
        if "command" not in self.italccommand:
            self.italccommand["command"] = buf.decode("utf-16-be")
            return True
        if "args_length" not in self.italccommand:
            self.italccommand["args_length"] = struct.unpack(">I", buf)[0]
            self.italccommand["args_parsed"] = 0
            self.italccommand["args"] = []
            if self.italccommand["args_length"] == 0:
                self.processing_italccommand = False
                print("{}".format(self.italccommand))
            return True
        if len(self.italccommand["args"]) == self.italccommand["args_parsed"]:
            kl = struct.unpack(">I", buf)[0]
            self.italccommand["args"].append({"key_length": kl})
            return True
        else:
            i = self.italccommand["args_parsed"]
            if "key" not in self.italccommand["args"][i]:
                self.italccommand["args"][i]["key"] = buf.decode("utf-16-be")
                return True
            if "value_type" not in self.italccommand["args"][i]:
                self.italccommand["args"][i]["value_type"] = struct.unpack(">I", buf)[0]
                return True
            if self.italccommand["args"][i]["value_type"] == 10:
                if "value_unk" not in self.italccommand["args"][i]:
                    self.italccommand["args"][i]["value_unk"] = buf
                    return True
                if "value_length" not in self.italccommand["args"][i]:
                    self.italccommand["args"][i]["value_length"] = struct.unpack(">I", buf)[0]
                    return True
                if "value_data" not in self.italccommand["args"][i]:
                    self.italccommand["args"][i]["value_data"] = buf.decode("utf-16-be")
                    self.italccommand["args_parsed"] += 1
                    if self.italccommand["args_parsed"] >= self.italccommand["args_length"]:
                        self.processing_italccommand = False
                        print("{}".format(self.italccommand))
                    return True
            else:
                pass


def main():
    m2p_listening = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    m2p_listening.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    m2p_listening.bind((LISTEN_HOST, LOCAL_PORT))
    m2p_listening.listen(32)
    while True:
        m2p_conn, m2p_addr = m2p_listening.accept()
        print("New Session From {}".format(m2p_addr))
        p2e_conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        p2e_conn.connect((VNC_HOST, VNC_PORT))
        Pipe(m2p_conn, p2e_conn, True).start()
        Pipe(p2e_conn, m2p_conn, False).start()

if __name__ == "__main__":
    main()

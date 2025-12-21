
import threading

class Base:
    def __init__(self, server_main):
        self._name = "Base"
        self._server_main = server_main

    def start_listen(self):
        self._listen_thread = threading.Thread(target=self.listen_main, name = "base")
        self._listen_thread.start()

    def listen_main(self):
        print("listen main")

class DerivedA(Base):
    def __init__(self, server_main):
        self._name = "DerivedA"
        self._server_main = server_main

    def start_listen(self):
        self._listen_thread = threading.Thread(target=self.listen_main, name = "DerivedA")
        self._listen_thread.start()

    def listen_main(self):
        print("listen DerivedA " + self._server_main._name)

class DerivedB(Base):
    def __init__(self, server_main):
        self._name = "DerivedB"
        self._server_main = server_main

    def start_listen(self):
        self._listen_thread = threading.Thread(target=self.listen_main, name = "DerivedB")
        self._listen_thread.start()

    def listen_main(self):
        print("listen DerivedB " + self._server_main._name)                

class ServerMain:
    def __init__(self, is_a:bool = True):
        self._name = "ServerMain"
        if is_a:
            self._test_obj = DerivedA(self)
        else:
            self._test_obj = DerivedB(self)
    
    def start(self):
        self._test_obj.start_listen()

def test_main():
    server_obj = ServerMain()
    server_obj.start()
    

if __name__ == "__main__":
    test_main()
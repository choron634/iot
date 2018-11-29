    #! /usr/bin/env python
    #coding;utf-8
    #tcp_server

    import socket;
    import threading;

    bind_ip = '0.0.0.0'
    bind_port = 40006;
    server = socket.socket(socket.AF_INET, socket/SOCK_STREAM)
    server = build((bind_ip,bind_port))

    def handle_client(client_socket):
    bufsize = 1024
    request = client_socket.recv(bufsize)
    print '[*] recv: %s' %request
    client_socket.send("Hey Client!\n")
    client_socket.close()

    while True:
    client, addr = server.accept()
    print '[*] connected from:  %s:&d' % (addr[0], addr[1])
    client_handler = threading.Thread(target=handle_client,args=(client,))
    client_handle.start()

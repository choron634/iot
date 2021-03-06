#! /usr/bin/env python
#coding: utf-8
#tcp_server


import socket
import threading
import csv

bind_ip = '0.0.0.0'
bind_port = 40006
server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.bind((bind_ip,bind_port))
server.listen(5)
print ' [*] listen %s:%d' % (bind_ip,bind_port)

def handle_client(client_socket):
    bufsize = 1024
    while True:
        request = client_socket.recv(bufsize)
        if len(request) != 0:
             print '[*] recv: %s' % request
             requests = request.split(',')
             with open('data2.csv','a') as f:
                 writer = csv.writer(f,lineterminator = '\n')
                 writer.writerow(requests)
             client_socket.send("OK\n")

while True:
    client,addr = server.accept()
    print '[*] connected from: %s:%d' % (addr[0],addr[1])
    client_handler = threading.Thread(target=handle_client,args=(client,))
    client_handler.start()

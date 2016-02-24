############################
#slave
#Script to run optimization tests for variables TX, A and REC of Sense and Avoid prototype
#Server side - receives variables from Client and sends to arduino
#January, 2016
#Miguel Fonseca
###########################

import socket
import serial
import time
from available_coms import serial_ports

BAUD_RATE = 115200

# select port
ports = serial_ports()
print('available ports are:')
for i, p in enumerate(ports):
    print('{} - {}'.format(i, p))
ans = input('please choose a port:')
ans = int(ans)
portname = ports[ans]

#create server
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.bind(('0.0.0.0', 11311))
serversocket.listen(5)  # become a server socket, maximum 5 connections
print('listening...')
connection, address = serversocket.accept()
buf = connection.recv(64)
print("data received")
if buf.decode() == "hi":
    connection.send('ho'.encode()) #syncronization with client
    print("answer sent!")
    while True:
        buf = connection.recv(64) #receive variables
        buf = buf.decode()
        print("Starting Arduino")
        time.sleep(5)
        while True: #connect with arduino
            value = True
            try:
                ser = serial.Serial(portname, BAUD_RATE, timeout=31)
            except serial.SerialException:
                print("Port occupied")
                value = False
            if value: break
        ser.flush()
        time.sleep(3) #wait for arduino to initialize
        print(buf[0])
        ser.write(buf[0].encode()) #send TX to arduino
        time.sleep(0.1)
        print(buf[2])
        ser.write(buf[2].encode()) #send A to arduino
        time.sleep(0.1)
        print(buf[4])
        ser.write(buf[4].encode()) #send REC to arduino
        time.sleep(0.1)
        data_raw = ser.readline() #wait for confirmation from arduino
        data = (data_raw.decode())
        print(data)
        connection.send('done'.encode()) #syncronization with client
        buf = connection.recv(64)
        if buf.decode() == "close": #terminate communication with arduino when client says
            print("closing")
            ser.close()

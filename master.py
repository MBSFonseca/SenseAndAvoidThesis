############################
#master
#Script to run optimization tests for variables TX, A and REC of Sense and Avoid prototype
#Client side - sends variables to server and to the arduino
#manages duration of each test
#January, 2016
#Miguel Fonseca
###########################

import socket
import serial
import time
from available_coms import serial_ports

BAUD_RATE = 115200

TX = ['1', '2', '3', '4', '5']


def sync_with_slave():
    answer = clientsocket.recv(64)
    answer = answer.decode()
    if answer == "ho" or answer == "done":
        print("success")
        return True
    else:
        return False


if __name__ == '__main__':
	#select port
    ports = serial_ports()
    print('available ports are:')
    for i, p in enumerate(ports):
        print('{} - {}'.format(i, p))
    ans = input('please choose a port:')
    ans = int(ans)
    portname = ports[ans]
	#select filename to save information
    ans = input('filename: ')
    filename = str(ans)
    f = open(filename, 'w')
	#initiate communication with server
    clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        print("Creating socket")
        clientsocket.connect(('192.168.1.103', 11311)) #change IP and port for Socket connection here
        clientsocket.send('hi'.encode())
    except clientsocket.error as msg:
        print("Failed to create and connect. Error code: " + str(msg[0]) + " , Error message : " + msg[1])
    success = False
    while not success:
        success = sync_with_slave() #syncronize with server
    for i in range(0, 4): #run tests for all the combinations
        for u in range(0, 2):
            for y in range(0, 3):
                clientsocket.send((TX[i]+','+TX[u]+','+TX[y]).encode()) #send variables to server
                success = sync_with_slave() #syncronize with server
                time.sleep(5)
                while True: #initiate communication with arduino
                    value = True
                    try:
                        ser = serial.Serial(portname, BAUD_RATE, timeout=31)
                    except serial.SerialException:
                        print("Port occupied")
                        value = False
                    if value: break
                ser.flush()
                time.sleep(3)
                ser.write(TX[i].encode()) #send TX to arduino
                time.sleep(0.1)
                ser.write(TX[u].encode()) #send A to arduino
                time.sleep(0.1)
                ser.write(TX[y].encode()) #send REC to arduino
                time.sleep(0.1)
                data_raw = ser.readline() #receive confirmation from arduino
                data = (data_raw.decode())
                print(data, end='')
                print("starting test")
                a = 0
                t0 = time.clock()
                while (time.clock() - t0) < 60: #let arduino work for 60 seconds
                    data_raw = ser.readline()
                    data = (data_raw.decode())
                    if data != '':
                        print(data)
                        a += 1 #save number of data received from arduino
                    # print(a)
                print(time.clock()-t0)
                ser.close()
                clientsocket.send("close".encode()) #tell client to stop communication with arduino
                save = TX[i]+','+TX[u]+','+TX[y]+','+str(a)+'\n'
                print(save, end='') #save information to file
                f.write(save)
    try:
        f.close()
    except Exception:
        print('Could not close file')
        pass
    print(time.clock())

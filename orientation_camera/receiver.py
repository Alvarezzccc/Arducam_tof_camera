import socket
import pyaudio 
import numpy as np 
import wave 

localIP     = "127.0.0.1"
localPort   = 20001

bufferSize  = 1600
chunk = 800 
FORMAT = pyaudio.paInt16 
CHANNELS = 2 
RATE = 8000 


# Para grabar a disco, file=True
# Para sacar por audio, file=False
file=False

p = pyaudio.PyAudio() 
stream = p.open(format = FORMAT, 
                channels = CHANNELS, 
                rate = RATE, 
                input = False, 
                output = True, 
                frames_per_buffer = chunk) 

 

#msgFromServer       = "Hello UDP Client"
#bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP escuchando")

if file:
    outputFileName = 'output.wav'
    wv = wave.open(outputFileName, 'w')
    wv.setparams((2, 1, RATE, 0, 'NONE', 'not compressed'))

# Listen for incoming datagrams

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0]
    data = np.fromstring(message, dtype=np.int16)
    data[:]=data[:]
    signal = wave.struct.pack("%dh"%(len(data)), *list(data))
    
    if not file:
        stream.write(signal) 
    
    if file:
        wv.writeframes(signal)
    #address = bytesAddressPair[1]

    #clientMsg = "Message from Client:{}".format(message)
    #clientIP  = "Client IP Address:{}".format(address)
    
    #print(clientMsg)
    #print(clientIP)
    print(f"{len(message)} datos recibidos")


    # Sending a reply to client

    #UDPServerSocket.sendto(bytesToSend, address)

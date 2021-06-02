import hid, time #hidapi
h=hid.device()
h.open(1003,8211)
h.get_manufacturer_string()
data=[0]*9 # data[0] is report ID (always zero)
h.write(data) # write 8 bytes

for i in range(256):
    data[-1]=i
    h.write(data)
    time.sleep(0.001)



# sample code for official hidio demo
#data[1]=1 # LED turn-on mode
#data[2]=1 # target LED number (1~4)
#h.write(data) # LED1 turns on
#
#data[2]=2
#h.write(data) # LED2 turns on
#
#data[1]=0 # LED turn-off mode
#h.write(data) # LED2 turns off
#
#data[2]=1
#h.write(data) # LED1 turns off
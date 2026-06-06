# Motor Control Client in python
import matplotlib.pyplot as plt 
from statistics import mean
import serial

DATA_LENGTH = 400
ref = []
data = []
data_received = 0

ser = serial.Serial('COM15', 115200)
print('Opening port: ')
print(ser.name)

# Send the letter 'a' to STM32
ser.write(('a'+'\n').encode())

# Collect data

while data_received < DATA_LENGTH:
    data_str = ser.read_until(b'\n')
    data_f = list(map(float, data_str.split()))

    ref.append(data_f[1])
    data.append(data_f[2])
    data_received += 1

meanzip = zip(ref,data)
meanlist = []
for i,j in meanzip:
    meanlist.append(abs(i-j))
score = mean(meanlist)
t = range(len(ref)) # index array
plt.plot(t,ref,'r*-',t,data,'b*-')
plt.title('Current Control Test: Score = ' + str(score))
plt.ylabel('Value (mA)')
plt.xlabel('Index')
plt.tight_layout()
plt.show()


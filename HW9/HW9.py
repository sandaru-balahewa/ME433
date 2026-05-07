import csv

# List to store the paths of 4 files
file_list = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]

t = []
sig_data = []

for file in file_list:
    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            sig_data.append(float(row[1])) # second column

    sample_rate = (len(t) - 1) / t[-1]
    print(sample_rate)


# FFT

import matplotlib.pyplot as plt
import numpy as np

dt = 1.0/10000.0 # 10kHz
t = np.arange(0.0, 1.0, dt) # 10s
# a constant plus 100Hz and 1000Hz
s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25

Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = s # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()


# for i in range(len(t)):
#     # print the data to verify it was read
#     print(str(t[i]) + ", " + str(data1[i]))

# import matplotlib.pyplot as plt # for plotting
# import numpy as np # for sine function

# dt = 1.0/100.0 # 100Hz
# t = np.arange(0.0, 5.0, dt) # for 5s

# s = 2.0 * np.sin(2 * np.pi * 2.3 * t) + 2.5 # 2.3Hz

# plt.plot(t,s,'b-*')
# plt.xlabel('Time [s]')
# plt.ylabel('Signal')
# plt.title('Signal vs Time')
# plt.show()
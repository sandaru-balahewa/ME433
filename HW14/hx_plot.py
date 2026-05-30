import sys
import serial
import time
import matplotlib.pyplot as plt
import numpy as np

if len(sys.argv) < 2:
    print("Usage: hx_plot.py <serial_port> [baud_rate]")
    sys.exit(1)

port = sys.argv[1]
baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200

NUM_LINES = 5*80

def read_lines(ser, n):
    lines = []
    start = time.time()
    while len(lines) < n and (time.time() - start) < 5.0: # 5 second timeout
        line = ser.readline()
        if not line:
            continue
        try:
            s = line.decode('utf-8', errors='replace').strip()
        except Exception:
            continue
        if s:
            lines.append(s)

    return lines

with serial.Serial(port, baud, timeout=10) as ser:
    # flush input/output
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    print("Collection requested...")

    # send a number to request data
    ser.write(f"{NUM_LINES}\n".encode('utf-8'))
    ser.flush()

    raw_lines = read_lines(ser, NUM_LINES)
    print(f"Collected {len(raw_lines)} lines")

    if len(raw_lines) < NUM_LINES:
        print(f"Warning: expected {NUM_LINES} lines, got {len(raw_lines)}")

# parse lines expecting: index time value
indices = []
times = []
values = []

for line in raw_lines:
    parts = line.split()
    if len(parts) < 3:
        continue
    try:
        idx = int(parts[0])
        t = float(parts[1])
        v = float(parts[2])
    except ValueError:
        continue

    indices.append(idx)
    times.append(t)
    values.append(v)

if not times:
    print("No valid data parsed.")
    sys.exit(1)

base = times[0]
for i in range(len(times)):
    times[i] -= base
    times[i] = times[i]/1000

# Plot value vs time
plt.figure(figsize=(10,4))
plt.plot(times, values, marker='o', linestyle='-')
plt.xlabel("Time (s)")
plt.ylabel("Value")
plt.title("HX711 Values vs. Time")
plt.grid(True)
plt.tight_layout()
plt.show()

# FFT code from HW9

Fs = len(times)/(times[-1]-times[0]) # sample rate
Ts = 1.0/Fs; # sampling interval
# ts = np.arange(0,t[-1],Ts) # time vector

y = values # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]


# Plotting
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12,8))
fig.suptitle(f"Raw Signal FFT")
ax1.plot(t,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title("Time Series Plot")

ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
ax2.set_title("Frequency Domain Signal Using FFT")
plt.tight_layout()
plt.show()
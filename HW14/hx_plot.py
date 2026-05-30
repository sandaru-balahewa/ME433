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
iir_values = []
raw_values = []

for line in raw_lines:
    parts = line.split()
    if len(parts) < 4:
        continue
    try:
        idx = int(parts[0])
        t = float(parts[1])
        raw_val = float(parts[2])
        v = float(parts[3])
    except ValueError:
        continue

    indices.append(idx)
    times.append(t)
    raw_values.append(raw_val)
    iir_values.append(v)

if not times:
    print("No valid data parsed.")
    sys.exit(1)

base = times[0]
for i in range(len(times)):
    times[i] -= base
    times[i] = times[i]/1000

# Plot Raw value and IIR Filtered values vs time
fig, (ax1,ax2) = plt.subplots(2,1, figsize=(10,6))
fig.suptitle("Raw vs IIR Filtered Signal Comparison")

ax1.plot(times, raw_values, marker='o', linestyle='-', color='k')
ax1.set_xlabel("Time (s)")
ax1.set_ylabel("Value")
ax1.set_title("Raw Signal")
ax1.grid(True)


ax2.plot(times, iir_values, marker='o', linestyle='-', color='b')
ax2.set_xlabel("Time (s)")
ax2.set_ylabel("Value")
ax2.set_title("IIR Filtered Signal")
ax2.grid(True)

plt.tight_layout()
plt.show()

# FFT code from HW9

Fs = len(times)/(times[-1]-times[0]) # sample rate

Ts = 1.0/Fs; # sampling interval
# ts = np.arange(0,t[-1],Ts) # time vector

y_iir = iir_values # the data to make the fft from
n = len(y_iir) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range

# Raw FFT
Y_raw = np.fft.fft(raw_values)/n
Y_raw = Y_raw[:n//2]

# IIR FFT
Y_iir = np.fft.fft(y_iir)/n # fft computing and normalization
Y_iir = Y_iir[range(int(n/2))]


# Plotting
fig, axs = plt.subplots(2, 2, figsize=(16, 8))
fig.suptitle("HX711 Raw vs IIR Filtered Signal FFT Comparison")

# Raw signal
axs[0,0].plot(times, raw_values, color='k')
axs[0,0].set_title("Raw Signal")
axs[0,0].set_xlabel("Time (s)")
axs[0,0].set_ylabel("Amplitude")
axs[0,0].grid(True)

axs[0,1].loglog(frq[1:], np.abs(Y_raw[1:]), color='k')
axs[0,1].set_title("Raw Signal FFT")
axs[0,1].set_xlabel("Frequency (Hz)")
axs[0,1].set_ylabel("|Y(f)|")
axs[0,1].grid(True)

# IIR signal
axs[1,0].plot(times, iir_values)
axs[1,0].set_title("IIR Filtered Signal")
axs[1,0].set_xlabel("Time (s)")
axs[1,0].set_ylabel("Amplitude")
axs[1,0].grid(True)

axs[1,1].loglog(frq[1:], np.abs(Y_iir[1:]))
axs[1,1].set_title("IIR Filtered FFT")
axs[1,1].set_xlabel("Frequency (Hz)")
axs[1,1].set_ylabel("|Y(f)|")
axs[1,1].grid(True)

plt.tight_layout()
plt.show()

# fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12,8))
# fig.suptitle(f"Raw Signal FFT")
# ax1.plot(times,y_iir,'b')
# ax1.set_xlabel('Time')
# ax1.set_ylabel('Amplitude')
# ax1.set_title("Time Series Plot")

# ax2.loglog(frq,abs(Y_iir),'b') # plotting the fft
# ax2.set_xlabel('Freq (Hz)')
# ax2.set_ylabel('|Y(freq)|')
# ax2.set_title("Frequency Domain Signal Using FFT")
# plt.tight_layout()
# plt.show()
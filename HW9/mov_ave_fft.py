import csv
import matplotlib.pyplot as plt
import numpy as np

X = 75

# List to store the paths of 4 files
file_list = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]

for file in file_list:

    file_name = file.replace(".csv", "")

    # Lists to store time and signal data
    t = []
    sig_data = []
    mov_ave_data = []

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            sig_data.append(float(row[1])) # second column

    # Loop to get the smoothed signal using a moving average filter
    for i in range(X-1, len(sig_data)):
        total = sum(sig_data[i-X+1:i+1])
        mov_ave_data.append(total/X)

    # FFT original

    # Calculate sample rate
    sample_rate = (len(t) - 1) / (t[-1] - t[0])

    Fs = sample_rate # sample rate
    y = sig_data # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y_original = np.fft.fft(y)/n # fft computing and normalization
    Y_original = Y_original[range(int(n/2))]

    # FFT moving average
    y_mov = mov_ave_data # the data to make the fft from
    n_mov = len(y_mov) # length of the signal
    k_mov = np.arange(n_mov)
    T_mov = n_mov/Fs
    frq_mov = k_mov/T_mov # two sides frequency range
    frq_mov = frq_mov[range(int(n_mov/2))] # one side frequency range
    Y_mov = np.fft.fft(y_mov)/n_mov # fft computing and normalization
    Y_mov = Y_mov[range(int(n_mov/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12,8))
    ax1.plot(t,sig_data,'k', label="Unfiltered")
    ax1.plot(t[X-1:], mov_ave_data, 'r', label="Filtered")
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f"{file_name} Time Series Signal Comparison for a Moving Average Filter with X = {X}")
    ax1.legend()
    ax1.grid(True)

    ax2.loglog(frq,abs(Y_original),'k', label="Unfiltered")
    ax2.loglog(frq_mov, abs(Y_mov), 'r', label="Filtered")
    ax2.set_title(f"{file_name} FFT Comparison for a Moving Average Filter with X = {X}")
    ax2.legend()
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    ax2.grid(True)
    plt.tight_layout()
    output_name = f"Figures/{file_name}_MAF_X_{X}.png"
    plt.savefig(output_name, dpi=300)
    plt.close()
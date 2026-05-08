import csv
import matplotlib.pyplot as plt
import numpy as np

A = 0.95
B = 0.05

# List to store the paths of 4 files
file_list = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]
# file_list = ["sigA.csv", "sigB.csv", "sigC.csv"]
weights_file_list = ["sigA_fir_weights.txt", "sigB_fir_weights.txt", "sigC_fir_weights.txt", "sigD_fir_weights.txt"]

cut_off_freq = [100, 33, 25, 20]
bandwidths = [200, 66, 100, 36]
X_list = [231, 231, 115, 53]

for index, file in enumerate(file_list):

    # Lists to store time and signal data
    t = []
    sig_data = []
    fir_data = []

    weights = np.loadtxt(weights_file_list[index])

    file_name = file.replace(".csv", "")

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            sig_data.append(float(row[1])) # second column
    
    X = X_list[index]

    # Loop to get the smoothed signal using a moving average filter
    for i in range(X-1, len(sig_data)):
        total = 0
        for k, val in enumerate(sig_data[i-X+1:i+1]):
            total += weights[k] * val
        
        fir_data.append(total)

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
    y_fir = fir_data # the data to make the fft from
    n_fir = len(y_fir) # length of the signal
    k_fir = np.arange(n_fir)
    T_fir = n_fir/Fs
    frq_fir = k_fir/T_fir # two sides frequency range
    frq_fir = frq_fir[range(int(n_fir/2))] # one side frequency range
    Y_fir = np.fft.fft(y_fir)/n_fir # fft computing and normalization
    Y_fir = Y_fir[range(int(n_fir/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12,8))
    fig.suptitle(f"{file_name} FIR Filter Comparison - Cutoff Frequency = {cut_off_freq[index]}, Bandwidth = {bandwidths[index]} Weights = {X_list[index]}")
    ax1.plot(t,sig_data,'k', label="Unfiltered")
    ax1.plot(t[X-1:], fir_data, 'r', label="Filtered")
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f"Time Series Signal Comparison")
    ax1.legend()
    ax1.grid(True)

    ax2.loglog(frq,abs(Y_original),'k', label="Unfiltered")
    ax2.loglog(frq_fir, abs(Y_fir), 'r', label="Filtered")
    ax2.set_title(f"FFT Comparison")
    ax2.legend()
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    ax2.grid(True)
    plt.tight_layout()
    plt.show()
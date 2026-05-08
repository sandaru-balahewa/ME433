import csv
import matplotlib.pyplot as plt
import numpy as np

A = 0.95
B = 0.05

# List to store the paths of 4 files
file_list = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]

for file in file_list:

    # Lists to store time and signal data
    t = []
    sig_data = []
    iir_data = []

    file_name = file.replace(".csv", "")

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            sig_data.append(float(row[1])) # second column

    iir_data.append(sig_data[0])
    # Loop to get the smoothed signal using an IIR filter
    for i in range(1, len(sig_data)):
        new_val = A * iir_data[-1] + B * sig_data[i]
        iir_data.append(new_val)

    # FFT original

    # Calculate sample rate
    sample_rate = (len(t) - 1) / (t[-1] - t[0])
    print(f"Sample Rate: {sample_rate}")

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
    y_fil = iir_data # the data to make the fft from
    n_fil = len(y_fil) # length of the signal
    k_fil = np.arange(n_fil)
    T_fil = n_fil/Fs
    frq_fil = k_fil/T_fil # two sides frequency range
    frq_fil = frq_fil[range(int(n_fil/2))] # one side frequency range
    Y_fil = np.fft.fft(y_fil)/n_fil # fft computing and normalization
    Y_fil = Y_fil[range(int(n_fil/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12,8))
    fig.suptitle(f"{file_name} IIR Filter with A = {A}, B = {B}")
    ax1.plot(t,sig_data,'k', label="Unfiltered")
    ax1.plot(t, iir_data, 'r', label="Filtered")
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title("Time Series Signal Comparison")
    ax1.legend()
    ax1.grid(True)

    ax2.loglog(frq,abs(Y_original),'k', label="Unfiltered")
    ax2.loglog(frq_fil, abs(Y_fil), 'r', label="Filtered")
    ax2.set_title("FFT Comparison")
    ax2.legend()
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    ax2.grid(True)
    plt.tight_layout()
    output_name = f"Figures/{file_name}_IIR.png"
    plt.savefig(output_name, dpi=300)
    plt.close()
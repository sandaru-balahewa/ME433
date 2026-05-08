import csv
import matplotlib.pyplot as plt
import numpy as np

# List to store the paths of 4 files
file_list = ["sigA.csv", "sigB.csv", "sigC.csv", "sigD.csv"]

for file in file_list:

    # Lists to store time and signal data
    t = []
    sig_data = []

    file_name = file.replace(".csv", "")

    with open(file) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            sig_data.append(float(row[1])) # second column

    # convert to numpy arrays
    t = np.array(t)
    sig_data = np.array(sig_data)

    # Calculate sample rate
    sample_rate = (len(t) - 1) / (t[-1] - t[0])

    print(sample_rate)

    # FFT

    Fs = sample_rate # sample rate
    # Ts = 1.0/Fs; # sampling interval
    # ts = np.arange(0,t[-1],Ts) # time vector
    y = sig_data # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]


    # Plotting
    fig, (ax1, ax2) = plt.subplots(2, 1)
    fig.suptitle(f"Raw Signal FFT")
    ax1.plot(t,y,'b')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f"{file} Time Series Plot")
    ax2.loglog(frq,abs(Y),'b') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    ax2.set_title(f"{file} Frequency Domain Signal Using FFT")
    plt.tight_layout()
    output_name = f"Figures/{file_name}_Raw_FFT.png"
    plt.savefig(output_name, dpi=300)
    plt.close()

import sys
import serial
import time
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Usage: hx_plot.py <serial_port> [baud_rate]")
    sys.exit(1)

port = sys.argv[1]
baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200

NUM_LINES = 400


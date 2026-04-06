import board
import pwmio
import time

servo = pwmio.PWMOut(board.GP28, variable_frequency=True)
servo.frequency = 50 # hz
duty_lower = int(65535*0.5/20)
duty_higher = int(65535*2.5/20)

while True:
    # pulse 0.5 ms to 2.5 ms out of a possible 20 ms (50Hz)
    # for 0 degrees to 180 degrees
    # so duty_cycle can be 65535*0.5/20 to 65535*2.5/20
    # but check this, some servo brands might only want 1-2 ms
    
    # 0 to 180 degrees
    for i in range(duty_lower, duty_higher, 20):
        servo.duty_cycle = i
        time.sleep(0.01)
    # 180 to 0 degrees
    for i in range(duty_higher, duty_lower, -20):
        servo.duty_cycle = i
        time.sleep(0.01)
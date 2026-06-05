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
plt.title('Score = ' + str(score))
plt.ylabel('value')
plt.xlabel('index')
plt.show()


# def read_plot_matrix():
#     ref = []
#     data = []
#     data_received = 0
#     while data_received < n_int:
#         dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
#         dat_f = list(map(float,dat_str.split())) # now the data is a list
#         ref.append(dat_f[0])
#         data.append(dat_f[1])
#         data_received = data_received + 1
#     meanzip = zip(ref,data)
#     meanlist = []
#     for i,j in meanzip:
#         meanlist.append(abs(i-j))
#     score = mean(meanlist)
#     t = range(len(ref)) # index array
#     plt.plot(t,ref,'r*-',t,data,'b*-')
#     plt.title('Score = ' + str(score))
#     plt.ylabel('value')
#     plt.xlabel('index')
#     plt.show()


# ser = serial.Serial('COM13')
# print('Opening port: ')
# print(ser.name)


# has_quit = False
# # menu loop
# while not has_quit:
#     print('PIC32 MOTOR DRIVER INTERFACE')
#     # display the menu options; this list will grow
#     print('\td: test case \te: test case 2\ta: read current\n\tr: get state\tf: set PWM\tp: unpower motor') # '\t' is a tab
#     print('\tg: set CC gains\t\t\th: get CC gains\t')
#     print('\tt: get encoder count\t\ty: get encoder angle')
#     print('\tk: ITEST\ti: set PC gains\tj: get PC gains')
#     print('\tm: step\t\tn: cubic\to: TRACK')
#     print('\tz: encoder reset\t\tq: Quit')
#     # read the user's choice
#     selection = input('\nENTER COMMAND: ')
#     selection_endline = selection+'\n'
     
#     # send the command to the PIC32
#     ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
#     # take the appropriate action
#     # there is no switch() in python, using if elif instead
#     if (selection == 'd'):
#         input_str = input('Enter an integer: ') 
#         input_int = int(input_str)
#         print('The number = ' + str(input_int)) 
#         ser.write((str(input_int)+'\n').encode())
#         n_str = ser.read_until(b'\n'); 
#         n_int = int(n_str)
#         print('Returned number = '+str(n_int)+'\n')

#     elif (selection == 'q'):
#         print('Exiting client')
#         has_quit = True; # exit client
#         # be sure to close the port
#         ser.close()

#     elif (selection == 'e'):
#         input_str = input("Enter two integers: ")
#         ser.write((input_str+'\n').encode())
#         n_str = ser.read_until(b'\n')
#         integer_list = list(map(int, n_str.split()))
#         print(f"Returned numbers: {integer_list[0]} {integer_list[1]}\n")

#     elif (selection == 'a'):
#         current_str = ser.read_until(b'\n')
#         current = float(current_str)
#         print(f"Current = {current} mA\n")

#     elif (selection == 'r'):
#         state_num_str = ser.read_until(b'\n')
#         state = state_names[int(state_num_str)]
#         print(f"State = {state}\n")

#     elif (selection == 'f'):
#         duty_cycle_str = input("Enter a duty cycle between -100 and 100: ")
#         ser.write((duty_cycle_str + '\n').encode())
#         print(f"PWM = {float(duty_cycle_str):.1f}\n")

#     elif (selection == 'p'):
#         print("Requested IDLE")

#     elif (selection == 'g'):
#         gain_str = input("Enter gains Kp and Ki: ")
#         ser.write((gain_str + '\n').encode())
#         gain_list = list(map(float, gain_str.split()))
#         print(f"The gains are set to Kp = {gain_list[0]} and Ki = {gain_list[1]}\n")

#     elif (selection == 'h'):
#         gain_str = ser.read_until(b'\n')
#         gain_list = list(map(float, gain_str.split()))
#         print(f"CC gains: Kp = {gain_list[0]}, Ki = {gain_list[1]}\n")

#     elif (selection == 'k'):
#         read_plot_matrix()
    
#     elif (selection == 't'):
#         encoder_count_str = ser.read_until(b'\n')
#         print(f"Encoder Count: {int(encoder_count_str)}\n")

#     elif (selection == 'y'):
#         encoder_angle_str = ser.read_until(b'\n')
#         print(f"Encoder Angle: {float(encoder_angle_str)} degrees\n")

#     elif (selection == 'z'):
#         print("Encoder reset\n")

#     elif (selection == 'i'):
#         gain_str = input("Enter PC gains Kp, Kd, and Ki: ")
#         ser.write((gain_str + '\n').encode())
#         gain_list = list(map(float, gain_str.split()))
#         print(f"The PC gains are set to Kp = {gain_list[0]}, Kd = {gain_list[1]}, and Ki = {gain_list[2]}\n")

#     elif (selection == 'j'):
#         gain_str = ser.read_until(b'\n')
#         gain_list = list(map(float, gain_str.split()))
#         print(f"PC gains: Kp = {gain_list[0]}, Kd = {gain_list[1]}, and Ki = {gain_list[2]}\n")

#     elif (selection == 'm'):
#         print("Load a step trajectory")
#         angles = genRef('step')
#         ser.write((str(len(angles)) + '\n').encode())
#         for i in range(len(angles)):
#             ser.write((str(angles[i]) + '\n').encode())

#     elif (selection == 'n'):
#         print("Load a cubic trajectory")
#         angles = genRef('cubic')
#         ser.write((str(len(angles)) + '\n').encode())
#         for i in range(len(angles)):
#             ser.write((str(angles[i]) + '\n').encode())

#     elif (selection == 'o'):
#         read_plot_matrix()
#     else:
#         print('Invalid Selection ' + selection_endline)

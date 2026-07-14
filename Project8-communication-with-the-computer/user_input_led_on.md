Based on what happens in ButtonLedMsTimer2.ino file under Project6 folder, create a new arduino code under Project8 folder that tirns on the led based on a user input:
1. Develop an Arduino sketch that interacts with a device over a serial port.
2. The sketch should continuously monitor the serial port for incoming data.
3. When data is available, the sketch should read a number of unknown length from the serial port. This number is the time in milliseconds that the LED on the device should light up when a button is pressed.
4. After reading the number, the sketch should print a message to the serial port stating "I received: " followed by the number.
5. The sketch should then use the received number to set a timer using the MsTimer2 library. The timer should be set to the received number plus one ms (if this bug was seen in project6).
5. use interrupt to catch the state change of the button, and use the MsTimer2 library for the timer functions.
6. When the timer expires, a function named `turn_off` should be executed.
7. The sketch should handle any exceptions or errors that might occur during the communication with the device.
8. The sketch should include comments explaining the functionality of each part of the code.
9. The sketch should be written in a clean code following good programming practices.
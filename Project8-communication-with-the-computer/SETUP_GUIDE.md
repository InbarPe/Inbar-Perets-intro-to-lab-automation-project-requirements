# Project 8: Two-Way Arduino Communication Setup Guide

## Overview
This project implements a complete two-way communication system between a Python GUI application and an Arduino board. The Arduino controls an LED based on user input from the GUI, and sends state updates back to the computer in real-time.

---

## Files Included

1. **user_interface.py** - Python GUI application using PySimpleGUI
2. **UserInputLedOn/UserInputLedOn.ino** - Updated Arduino sketch with state code feedback

---

## Hardware Requirements

- Arduino board (e.g., Arduino Uno, Mega)
- LED connected to pin 4
- Push button connected to pin 2 (with pull-up resistor)
- USB cable for Arduino connection

### Wiring Diagram (Arduino Uno)
```
Button (with pull-up):  Pin 2 ---> Ground (when pressed)
LED with resistor:      Pin 4 ---> +5V (LED anode through ~220Ω resistor to ground)
```

---

## Arduino Setup

### 1. Install Required Library
The sketch uses the **MsTimer2** library. Install it via Arduino IDE:
1. Go to **Sketch → Include Library → Manage Libraries**
2. Search for "MsTimer2"
3. Install the version by Juri Baradei

### 2. Upload the Sketch
1. Open `UserInputLedOn/UserInputLedOn.ino` in Arduino IDE
2. Select your Arduino board and COM port
3. Click **Upload**

### 3. Verify Arduino Connection
- Connect Arduino to your computer via USB
- Open Arduino Serial Monitor (9600 baud)
- You should see the Arduino ready for input

---

## Python Environment Setup

### Prerequisites
- Python 3.11+ installed
- Conda or venv virtual environment manager

### Setup Using Conda (Recommended)

Open **Anaconda Prompt** and run:

```bash
# Configure SSL (for Weizmann network)
conda config --set ssl_verify false

# Create environment
conda create --name intro python=3.11

# Activate environment
conda activate intro
```

### Install Required Packages

```bash
pip install PySimpleGUI
pip install pyserial
```

### Verify Installation

```python
# In Python/IDLE
import PySimpleGUI as sg
import serial
print("All packages installed successfully!")
```

### Configure VS Code Python Interpreter

1. Press **F1** to open Command Palette
2. Search for **"Python: Select Interpreter"**
3. Choose the `intro` environment created above

---

## Running the Application

### Start the Python GUI

```bash
# Make sure the conda environment is activated
conda activate intro

# Navigate to Project8 folder
cd "Project8-communication-with-the-computer"

# Run the script
python user_interface.py
```

### Using the GUI

1. **Connect to Arduino**
   - Select the correct COM port (e.g., COM3, COM4)
   - Click **Connect**
   - Status should change to "Connected" (green)

2. **Send LED Duration**
   - Enter a duration in milliseconds (e.g., 5000 for 5 seconds)
   - Click **Send**
   - Arduino will confirm: "I received: 5000"

3. **Press the Physical Button**
   - Press the button on the Arduino
   - GUI will show: "✓ Button Pressed - LED ON"
   - LED stays on for the specified duration

4. **LED Turns Off Automatically**
   - After the duration expires, GUI shows: "✓ Button Released - LED OFF"
   - LED turns off

5. **Disconnect**
   - Click **Disconnect** when done
   - Status returns to "Disconnected" (red)

---

## Communication Protocol

### Python → Arduino
- **Format**: Duration value followed by newline (e.g., "5000\n")
- **Example**: Send 5000 to make LED stay on for 5 seconds

### Arduino → Python
The Arduino sends status codes after each event:

| Code | Event | GUI Message |
|------|-------|-------------|
| `I received: XXXX` | Duration received | Displayed as confirmation |
| `1` | Button pressed, LED on | ✓ Button Pressed - LED ON |
| `2` | LED turned off | ✓ Button Released - LED OFF |

---

## Features of the Python Application

✓ **GUI Interface**
- Clean, user-friendly layout
- Real-time status updates
- Message history with timestamps

✓ **Serial Communication**
- Automatic port detection
- Parallel reading using threading
- Non-blocking event processing

✓ **Error Handling**
- Input validation (duration must be 1-60000 ms)
- Serial port error handling
- Graceful disconnection

✓ **Code Quality**
- Comprehensive comments and docstrings
- Modular design with SerialHandler class
- Thread-safe message passing

---

## Troubleshooting

### "Failed to connect" error
- Check that Arduino is connected via USB
- Verify correct COM port selected
- Try unplugging and replugging Arduino
- Check Device Manager for Arduino COM port

### No response from Arduino
- Verify sketch was uploaded correctly
- Check that baud rate matches (9600)
- Press reset button on Arduino
- Check serial monitor output in Arduino IDE

### GUI freezes
- This shouldn't happen due to threading
- If it does, check that PySimpleGUI is updated: `pip install --upgrade PySimpleGUI`

### Invalid input error
- Duration must be a positive integer (1-60000)
- No letters or special characters allowed
- Clear the field and try again

---

## Advanced Features

### Customization Options

Modify these constants in `user_interface.py`:
```python
BAUD_RATE = 9600              # Change if Arduino configured differently
RESPONSE_DISPLAY_LINES = 12   # Increase for more message history
MAX_MESSAGES = 100            # Maximum messages to keep
```

### Extending the Project

You can extend this project by:
- Adding multiple LED control
- Reading analog sensors from Arduino
- Logging data to CSV file
- Adding graphical display of timing

---

## Testing Checklist

- [ ] Arduino sketch uploads without errors
- [ ] Python GUI launches without errors
- [ ] Can connect to Arduino via GUI
- [ ] Can send duration value (e.g., 2000)
- [ ] Arduino echoes received value
- [ ] Button press shows correct status
- [ ] LED turns on and off at correct times
- [ ] GUI status messages appear correctly
- [ ] Can disconnect and reconnect

---

## References

- [Arduino MsTimer2 Library](https://github.com/PaulStoffregen/MsTimer2)
- [PySimpleGUI Documentation](https://pysimplegui.readthedocs.io/)
- [Python Serial Library](https://pyserial.readthedocs.io/)
- [Arduino Serial Communication](https://www.arduino.cc/reference/en/language/functions/communication/serial/)

---

## Support

If you encounter issues:
1. Check that all files are in the correct locations
2. Verify Arduino sketch compiles without errors
3. Test serial communication using Arduino Serial Monitor
4. Check Python console for error messages
5. Review the comments in the code for implementation details

---

**Created for: Intro to Lab Automation Course**  
**Date: 2026**

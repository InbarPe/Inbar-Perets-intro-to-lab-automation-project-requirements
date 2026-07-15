import serial
import csv
import time

# --- CONFIGURATION ---
# Replace 'COM4' with your Arduino's port (e.g., '/dev/ttyUSB0' on Linux/Mac or 'COM4' on Windows)
SERIAL_PORT = 'COM4' 
BAUD_RATE = 9600
CSV_FILE_NAME = 'arduino_log.csv'
# ---------------------

print(f"Connecting to Arduino on {SERIAL_PORT}...")

try:
    # Initialize serial port connection
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) # Give Arduino 2 seconds to reset/initialize
    print("Connected successfully!")
    
    # Open (or create) the CSV file and write the header row
    with open(CSV_FILE_NAME, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Time_ms", "Angle_deg", "Buzzer_Active"])
        print(f"Logging started. Saving data to '{CSV_FILE_NAME}'... Press Ctrl+C to stop.")
        
        while True:
            if ser.in_waiting > 0:
                # Read a line from the Arduino
                raw_data = ser.readline().decode('utf-8').strip()
                
                # Split the incoming string by commas
                data_fields = raw_data.split(',')
                
                # Make sure we got exactly 3 values (Time, Angle, Buzzer) to avoid corruption
                if len(data_fields) == 3:
                    writer.writerow(data_fields)
                    file.flush() # Forces Python to physically write the data to the hard drive immediately
                    print(f"Logged: Time={data_fields[0]}ms, Angle={data_fields[1]}°, Buzzer={data_fields[2]}")
                    
except KeyboardInterrupt:
    print("\nLogging stopped by user. File saved.")
except Exception as e:
    print(f"\nAn error occurred: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial port closed.")
"""
Two-Way Arduino Communication GUI
==================================
A PySimpleGUI application that communicates with an Arduino board via serial port.

Features:
- Send LED timing values to Arduino
- Display real-time status updates from Arduino
- Parallel serial reading using threading
- Full error handling and logging

Author: Lab Automation Project
Date: 2024
"""

import PySimpleGUI as sg
import serial
from serial.tools import list_ports
import threading
import time
from collections import deque
from typing import Optional, List, Tuple


# ============================================================================
# Configuration Constants
# ============================================================================

# GUI Theme and layout parameters
sg.theme('LightBlue2')
WINDOW_WIDTH = 500
WINDOW_HEIGHT = 400
RESPONSE_DISPLAY_LINES = 12

# Serial communication parameters
BAUD_RATE = 9600
SERIAL_TIMEOUT = 0.5  # seconds

# Message buffer configuration
MAX_MESSAGES = 100  # Maximum messages to keep in display

# State code mappings from Arduino
STATE_MESSAGES = {
    '0': '✓ LED OFF',
    '1': '✓ Button Pressed - LED ON',
    '2': '✓ Button Released and LED on',
}


# ============================================================================
# Serial Communication Handler
# ============================================================================

class SerialHandler:
    """
    Manages serial communication with Arduino in a separate thread.
    
    Handles:
    - Opening/closing serial connections
    - Reading incoming data
    - Writing outgoing data
    - Thread-safe message passing
    """
    
    def __init__(self, port: str, baudrate: int = 9600):
        """
        Initialize the serial handler.
        
        Args:
            port: COM port name (e.g., 'COM3')
            baudrate: Serial communication speed
        """
        self.port = port
        self.baudrate = baudrate
        self.serial_connection: Optional[serial.Serial] = None
        self.is_running = False
        self.message_queue: deque = deque()
        self.error_queue: deque = deque()
        self.read_thread: Optional[threading.Thread] = None
        
    def connect(self) -> Tuple[bool, str]:
        """
        Establish serial connection to Arduino.
        
        Returns:
            Tuple of (success: bool, message: str)
        """
        try:
            self.serial_connection = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=SERIAL_TIMEOUT
            )
            time.sleep(2)  # Wait for Arduino to reset after connection
            self.is_running = True
            
            # Start the reading thread
            self.read_thread = threading.Thread(
                target=self._read_from_serial,
                daemon=True
            )
            self.read_thread.start()
            
            return True, f"Connected to {self.port} at {self.baudrate} baud"
        
        except serial.SerialException as e:
            return False, f"Failed to connect: {str(e)}"
        except Exception as e:
            return False, f"Unexpected error during connection: {str(e)}"
    
    def disconnect(self):
        """Close the serial connection and stop the reading thread."""
        self.is_running = False
        if self.read_thread:
            self.read_thread.join(timeout=1)
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
    
    def send_data(self, data: str) -> Tuple[bool, str]:
        """
        Send data to Arduino with newline terminator.
        
        Args:
            data: String data to send
            
        Returns:
            Tuple of (success: bool, message: str)
        """
        try:
            if not self.serial_connection or not self.serial_connection.is_open:
                return False, "Serial connection is not open"
            
            # Add newline and send
            self.serial_connection.write((data + '\n').encode('utf-8'))
            return True, f"Sent: {data}"
        
        except serial.SerialException as e:
            return False, f"Send error: {str(e)}"
        except Exception as e:
            return False, f"Unexpected error during send: {str(e)}"
    
    def _read_from_serial(self):
        """
        Continuously read from serial port (runs in separate thread).
        
        Reads incoming lines and queues them for GUI processing.
        Handles errors gracefully and continues reading.
        """
        while self.is_running:
            try:
                if self.serial_connection and self.serial_connection.is_open:
                    if self.serial_connection.in_waiting:
                        line = self.serial_connection.readline().decode('utf-8').strip()
                        if line:
                            self.message_queue.append(line)
            
            except serial.SerialException as e:
                self.error_queue.append(f"Serial read error: {str(e)}")
            except UnicodeDecodeError as e:
                self.error_queue.append(f"Decode error: {str(e)}")
            except Exception as e:
                self.error_queue.append(f"Unexpected read error: {str(e)}")
            
            time.sleep(0.05)  # Small delay to prevent CPU spinning
    
    def get_message(self) -> Optional[str]:
        """Get next message from queue (thread-safe)."""
        try:
            return self.message_queue.popleft()
        except IndexError:
            return None
    
    def get_error(self) -> Optional[str]:
        """Get next error from queue (thread-safe)."""
        try:
            return self.error_queue.popleft()
        except IndexError:
            return None


# ============================================================================
# GUI Functions
# ============================================================================

def get_available_ports() -> List[str]:
    """
    Get list of available serial ports.
    
    Returns:
        List of available COM port names
    """
    ports = []
    try:
        for port_info in list_ports.comports():
            ports.append(port_info.device)
    except Exception as e:
        print(f"Error listing ports: {e}")
    return ports if ports else ['COM3', 'COM4', 'COM5']


def create_layout() -> List:
    """
    Create the GUI layout.
    
    Returns:
        PySimpleGUI layout list
    """
    layout = [
        # Title
        [sg.Text('Arduino LED Control Interface', 
                 font=('Arial', 14, 'bold'))],
        
        # Serial Port Selection
        [sg.Text('Serial Port:', size=(12, 1)),
         sg.Combo(values=get_available_ports(), 
                  default_value='COM3',
                  key='-PORT-',
                  size=(15, 1)),
         sg.Button('Connect', size=(10, 1)),
         sg.Button('Disconnect', size=(10, 1)),
         sg.Text('Disconnected', key='-STATUS-', 
                 text_color='red', size=(15, 1))],
        
        # Separator
        [sg.Text('_' * 60)],
        
        # Duration Input
        [sg.Text('LED Duration (ms):', size=(18, 1)),
         sg.InputText(default_text='5000',
                     key='-DURATION-',
                     size=(15, 1)),
         sg.Button('Send', size=(10, 1))],
        
        # Separator
        [sg.Text('_' * 60)],
        
        # Response Display
        [sg.Text('Device Status:', font=('Arial', 10, 'bold'))],
        [sg.Multiline(size=(60, RESPONSE_DISPLAY_LINES),
                      key='-RESPONSE-',
                      disabled=True,
                      autoscroll=True,
                      background_color='white')],
        
        # Control Buttons
        [sg.Button('Clear', size=(10, 1)),
         sg.Button('Exit', size=(10, 1))],
    ]
    return layout


def validate_duration_input(duration_str: str) -> Tuple[bool, str, Optional[int]]:
    """
    Validate and parse duration input.
    
    Args:
        duration_str: User input string
        
    Returns:
        Tuple of (is_valid: bool, error_message: str, value: Optional[int])
    """
    try:
        duration = int(duration_str)
        if duration <= 0:
            return False, "Duration must be positive", None
        if duration > 60000:
            return False, "Duration must be <= 60000 ms", None
        return True, "", duration
    except ValueError:
        return False, "Duration must be a whole number", None


def update_response_display(window, message: str, is_error: bool = False):
    """
    Update the response display area with new message.
    
    Args:
        window: PySimpleGUI window object
        message: Message to display
        is_error: If True, display as error (red text)
    """
    response_element = window['-RESPONSE-']
    current_text = response_element.get()
    
    # Format timestamp
    timestamp = time.strftime("%H:%M:%S")
    
    # Format message with emoji
    if is_error:
        formatted_message = f"[{timestamp}] ✗ {message}"
    else:
        formatted_message = f"[{timestamp}] {message}"
    
    # Keep only last MAX_MESSAGES
    lines = current_text.split('\n') if current_text else []
    lines.append(formatted_message)
    lines = lines[-MAX_MESSAGES:]
    
    response_element.update('\n'.join(lines))


# ============================================================================
# Main Application
# ============================================================================

def main():
    """
    Main application loop.
    
    Handles:
    - Window creation and event processing
    - Serial communication
    - GUI updates
    - Error handling
    """
    # Initialize variables
    serial_handler: Optional[SerialHandler] = None
    is_connected = False
    
    # Create window
    layout = create_layout()
    window = sg.Window('Arduino Communication',
                      layout,
                      size=(WINDOW_WIDTH, WINDOW_HEIGHT),
                      finalize=True)
    
    # Initial message
    update_response_display(window, "Ready. Select port and click Connect.")
    
    try:
        # Main event loop
        while True:
            # Process window events (non-blocking)
            event, values = window.read(timeout=100)
            
            # Check for errors from serial handler
            if serial_handler:
                while True:
                    error = serial_handler.get_error()
                    if error is None:
                        break
                    update_response_display(window, error, is_error=True)
                
                # Process incoming messages from Arduino
                while True:
                    message = serial_handler.get_message()
                    if message is None:
                        break
                    
                    # Handle different message types
                    if message in STATE_MESSAGES:
                        # State code from Arduino
                        display_message = STATE_MESSAGES[message]
                        update_response_display(window, display_message)
                    elif message.startswith("I received:"):
                        # Arduino confirmation
                        update_response_display(window, message)
                    else:
                        # Other messages from Arduino
                        update_response_display(window, f"Arduino: {message}")
            
            # Process button events
            if event == sg.WINDOW_CLOSED or event == 'Exit':
                break
            
            elif event == 'Connect':
                if is_connected:
                    update_response_display(window, 
                                          "Already connected. Disconnect first.",
                                          is_error=True)
                else:
                    port = values['-PORT-']
                    serial_handler = SerialHandler(port, BAUD_RATE)
                    success, message = serial_handler.connect()
                    
                    if success:
                        is_connected = True
                        window['-STATUS-'].update('Connected', text_color='green')
                        update_response_display(window, message)
                    else:
                        is_connected = False
                        update_response_display(window, message, is_error=True)
            
            elif event == 'Disconnect':
                if not is_connected:
                    update_response_display(window,
                                          "Not connected.",
                                          is_error=True)
                else:
                    if serial_handler:
                        serial_handler.disconnect()
                    is_connected = False
                    window['-STATUS-'].update('Disconnected', text_color='red')
                    update_response_display(window, "Disconnected from device.")
            
            elif event == 'Send':
                if not is_connected:
                    update_response_display(window,
                                          "Not connected. Please connect first.",
                                          is_error=True)
                else:
                    duration_str = values['-DURATION-'].strip()
                    
                    # Validate input
                    is_valid, error_msg, duration = validate_duration_input(duration_str)
                    
                    if not is_valid:
                        update_response_display(window, error_msg, is_error=True)
                    else:
                        # Send to Arduino
                        success, message = serial_handler.send_data(duration_str)
                        update_response_display(window, message)
                        if not success:
                            update_response_display(window, 
                                                  "Failed to send data",
                                                  is_error=True)
            
            elif event == 'Clear':
                window['-RESPONSE-'].update('')
                update_response_display(window, "Display cleared.")
    
    except Exception as e:
        # Handle unexpected errors
        sg.popup_error(f"Unexpected error: {str(e)}")
    
    finally:
        # Cleanup
        if serial_handler and is_connected:
            serial_handler.disconnect()
        window.close()


# ============================================================================
# Entry Point
# ============================================================================

if __name__ == '__main__':
    main()

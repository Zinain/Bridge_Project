__package__ = "BRUI"

# Import required PyQt5 modules for building the GUI
import ConnectionManager
import sys
import json
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout, QCheckBox
)
from PyQt5.QtCore import Qt

class BridgeControlUI(QWidget):
    def __init__(self):
        super().__init__()  # Call parent class constructor
        self.setWindowTitle("Bridge Remote Control Interface")

        # Status labels to display system state
        self.bridge_status_label = QLabel("Bridge Status: CLOSED")
        self.ship_detected_label = QLabel("Ship Detected: NO")
        self.lights_status_label = QLabel("Lights: OFF")
        self.override_checkbox = QCheckBox("Override Sensor Inputs")

        # Buttons for manual control
        self.open_button = QPushButton("Open Bridge")
        self.close_button = QPushButton("Close Bridge")
        self.reset_button = QPushButton("Reset System")
        self.lighton_button = QPushButton("Turn Lights On")
        self.lightoff_button = QPushButton("Turn Lights Off")

        # Setup initial states
        self.bridge_open = False
        self.ship_detected = False
        self.lights = False

        # Connect button clicks to handler methods
        self.open_button.clicked.connect(self.open_bridge)
        self.close_button.clicked.connect(self.close_bridge)
        self.reset_button.clicked.connect(self.reset_system)
        self.lighton_button.clicked.connect(self.lights_on)
        self.lightoff_button.clicked.connect(self.lights_off)
        self.override_checkbox.stateChanged.connect(self.toggle_override)

        # Layout organization
        layout = QVBoxLayout()

        # Add status labels to layout
        layout.addWidget(self.bridge_status_label)
        layout.addWidget(self.ship_detected_label)
        layout.addWidget(self.lights_status_label)

        # Add override checkbox
        layout.addWidget(self.override_checkbox)

        # Horizontal layout for buttons
        button_layout = QHBoxLayout()
        button_layout.addWidget(self.open_button)
        button_layout.addWidget(self.close_button)
        button_layout.addWidget(self.reset_button)
        button_layout.addWidget(self.lighton_button)
        button_layout.addWidget(self.lightoff_button)

        # Add button layout to main layout
        layout.addLayout(button_layout)

        # Set the layout to the main window
        self.setLayout(layout)

    def open_bridge(self):
        """Simulate opening the bridge"""
        if not self.bridge_open:
            ConnectionManager.udp_open()
            self.bridge_status_label.setText("Bridge Status: OPENING...")
            # Simulate some processing delay here if needed
            self.bridge_status_label.setText("Bridge Status: OPEN")
            self.bridge_open = True

    def close_bridge(self):
        """Simulate closing the bridge"""
        if self.bridge_open:
            ConnectionManager.udp_close()
            self.bridge_status_label.setText("Bridge Status: CLOSING...")
            # Simulate some processing delay here if needed
            self.bridge_status_label.setText("Bridge Status: CLOSED")
            self.bridge_open = False

    def reset_system(self):
        """Reset system state (useful for simulation or re-sync)"""
        ConnectionManager.udp_reset()
        self.bridge_open = False
        self.ship_detected = False
        self.lights = False
        self.override_checkbox.setChecked(False)
        self.bridge_status_label.setText("Bridge Status: CLOSED")
        self.ship_detected_label.setText("Ship Detected: NO")
        self.lights_status_label.setText("Lights: OFF")
        
    def lights_on(self):
        ConnectionManager.udp_lighton()
        "Lights simulation"
        self.lights_status_label.setText("Lights: ON")
        self.lights = True

    def lights_off(self):
        ConnectionManager.udp_lightoff()
        "Lights simulation"
        self.lights_status_label.setText("Lights: OFF")
        self.lights = False

    def toggle_override(self, state):
        """Enable or disable override mode"""
        if state == Qt.Checked:
            ConnectionManager.udp_swtich("TRUE")
            print("Sensor override enabled")
        else:
            ConnectionManager.udp_swtich("FALSE")
            print("Sensor override disabled")


# Standard way to start a PyQt5 application
if __name__ == "__main__":
    app = QApplication(sys.argv)  # Create application instance
    window = BridgeControlUI()    # Create main window
    window.show()                 # Display window
    sys.exit(app.exec_())         # Start event loop

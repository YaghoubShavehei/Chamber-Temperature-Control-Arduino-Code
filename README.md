# Chamber-Temperature-Control-Arduino-Code
This document outlines the design and implementation of a system to control the temperature in a chamber using quartz heating elements and convection fans. An Arduino Uno microcontroller was used to manage time intervals and control the system, while an OLED display and a rotary encoder were employed for the user interface. Solid-state relay (SSR) modules was used to control the AC heater and fans. The design incorporates measures to protect the convection fans from overheating during prolonged use by alternating between dual fans at preset intervals. Image shows sensors and connections.

![image](https://github.com/user-attachments/assets/cdeb0a45-adb7-45c3-b5f4-621cd9cfe642)

## System Overview
Components and Their Roles:
1.	**Arduino Uno**:
The core of the system, responsible for running the firmware and controlling the entire design.
2.	**OLED Display (1.3”)**:
Displays the user interface, including settings such as heater temperature, fan switching intervals, and default configurations.
  Note: Initially, the <ins> Adafruit_SSD1306.h </ins> library caused display issues. Switching to the <ins> Adafruit_SH110X.h </ins> library resolved these problems. Test both libraries     to determine the best fit for your setup.
3.	**Rotary Encoder**:
  A 5-pin rotary encoder is used for adjusting settings.
  **Pins**: 
    **Power**: 2 pins for input.
    **CLK and DT**: For detecting rotation.
    **SW**: For detecting button presses.
  **Functions**: 
    Set heater temperature.
    Configure fan switch intervals.
    Define heater temperature boundaries (upper and lower limits).
    Reset to default settings.
4.	**Buzzer**:
Provides audible alerts for connection issues or sensor malfunctions.
5.	**SSR Module**:
Controls the AC-powered heater and fans. SSRs are preferred over relay modules due to their silent operation and durability for prolonged use.
6.	**NTC 100K** Thermistor:
Monitors the chamber temperature in realtime.
### Design Highlights
  Dual-Fan Setup:
    To prevent overheating, two fans alternate at specific intervals, allowing one to cool while the other operates.
  User Interface:
    The OLED display and rotary encoder provide an intuitive interface for configuring the system.



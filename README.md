## Project Overview

[中文](README_CN.md)

This project develops a **Multimodal Detection System** that integrates **Capacitive Sensing** and **Triboelectric Nanogenerator (TENG)** technologies for a range of applications. The system includes both embedded hardware and software components, as well as a host software interface for data visualization and analysis.

![](.\Hardware\HaredwarImage\main.jpg)

### System Components:

1. **Lower-Level System: STM32 Microcontroller (STM32F103C8T6)**

   - **TENG Detection**: The TENG sensor is interfaced with the STM32 using operational amplifiers and the on-board ADC for analog-to-digital conversion.
   - **Capacitive Sensing**: The system uses the **FDC2214** capacitive sensor IC to measure changes in capacitance for detection purposes.
   - **FreeRTOS**: A real-time operating system (RTOS) running on the STM32 provides task scheduling and synchronization for efficient sensor data processing.
   - **Data Processing**: The embedded system applies **FIR filtering** to smooth and process ADC readings before they are transmitted to the host computer.
   - [mind_mapping](.\Software\STM32\mind_mapping.md)

2. **Host System: C++/QT-based Interface**

   - **Visualization**: A graphical user interface (GUI) is developed using C++ and QT to display real-time data from the sensors.
   - **Data Logging and Analysis**: The system logs sensor data and allows for advanced analysis, including Fourier Transform (FFT) processing for frequency domain analysis.

   ![](.\Software\Oscilloscope\img\main.png)

3. **Hardware Design**:

   - The project includes various schematics and PCB designs for interfacing the STM32 with the FDC2214 and TENG sensor. These files are organized in the `Hardware` directory.

4. **Software Design**:

   - The **STM32 software** consists of multiple layers including core libraries, middleware, and peripheral drivers, along with the FreeRTOS configuration.
   - The **Data Processing** software in Python helps to visualize and process the sensor data, including implementing FIR filters and FFT transformations.

5. **Documentation and References**:

   - The project includes references to datasheets, design notes, and application notes for key components such as the **FDC2214**, **TENG**, and **STM32F103C8T6** microcontroller.

   

------

### Project Directory Structure:

- **Hardware**: Contains all hardware design files, including schematics and PCB layouts for capacitive sensors, TENG interface, and the STM32-based board.

- Software

  :

  - **DataProcess**: Python scripts for data analysis, including FIR filtering and FFT processing.
  - **Oscilloscope**: QT-based software for real-time data visualization and serial communication with the STM32.
  - **STM32**: Firmware source code for STM32, including drivers, middleware, and FreeRTOS configuration.

- **REF**: Documentation, datasheets, and application notes for hardware components, including FDC2214 and STM32 microcontroller.

------

### Key Features:

- **Multimodal Detection**: Combines capacitive sensing and TENG detection in a single system.
- **Real-Time Processing**: Utilizes FreeRTOS for concurrent processing of sensor data.
- **Data Visualization**: A user-friendly interface for displaying sensor data and performing analysis.
- **Extensibility**: The system can be expanded with additional sensors or functionality for future applications.


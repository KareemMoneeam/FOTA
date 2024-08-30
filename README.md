# FOTA
# Firmware Over-The-Air (FOTA) System for Smart Vehicles

![Project Banner](Create_a_banner_for_a_GitHub_README_file_for_a_Fir.png)

### Cairo University, Faculty of Computers and Artificial Intelligence  
**Department of Information Technology**  
**Graduation Project 2023-2024**  

**Team Members:**  
- Kareem Abd El-Moneam Fawzy Yassin  
- Mahmoud Alaa Eldeen Fathy Sadek  
- Mohammed Gabr Ahmed Khamis  
- Salma Sherif Ibrahim Moustafa  
- Ahmed Adel Emad Eldeen Farag  
- Zyad Mahmoud Abbady Amin  

**Supervisor:**  
PhD. Haitham Safwat Hamza, Vice-Dean for Postgrad and Research, Cairo University

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Key Features](#key-features)
4. [Technologies Used](#technologies-used)
5. [Hardware Components](#hardware-components)
6. [Software Components](#software-components)
7. [Installation](#installation)
8. [Usage](#usage)
9. [Contributing](#contributing)
10. [License](#license)

---

## Project Overview

This project involves the development of a Firmware Over-The-Air (FOTA) system for smart vehicles, enabling secure, efficient, and remote software updates. The project aims to address the complexities of embedded systems in modern automobiles, focusing on enhancing user experience, maintaining vehicle safety, and ensuring cybersecurity.

Modern vehicles rely heavily on embedded systems and electronic control units (ECUs) to manage various functionalities, from engine performance to advanced driver assistance systems. The evolution of smart cars has introduced the need for real-time adaptability through FOTA systems, which allow for seamless and secure software updates.

---

## System Architecture

The project employs a layered architecture, featuring two main views:
- **Main ECU:** Manages the overall system and controls various peripherals.
- **Application ECU:** Executes application logic, including obstacle detection and avoidance using sensors and actuators.

### Components:
- **Microcontroller Abstraction Layer (MCAL):** Manages hardware specifics.
- **Hardware Abstraction Layer (HAL):** Interfaces with physical hardware components.
- **Application Layer:** Implements vehicle control and safety features.

![System Architecture](https://upload.wikimedia.org/wikipedia/commons/thumb/e/e2/System_Architecture.jpg/1024px-System_Architecture.jpg)

---

## Key Features

- **Real-time Firmware Updates:** Ensures vehicles are equipped with the latest software enhancements.
- **Automotive Cybersecurity:** Includes secure boot and encryption protocols to protect against unauthorized access.
- **User-Friendly Mobile Application:** Allows users to manage updates, report problems, and receive notifications directly from the vehicle's dashboard.

---

## Technologies Used

The project utilizes various technologies to create a robust FOTA system:

### 1. Django - FOTA Server
Django is used to develop the backend server that manages firmware updates and user interactions.

![Django Logo](https://upload.wikimedia.org/wikipedia/commons/thumb/7/75/Django_logo.svg/600px-Django_logo.svg.png)

### 2. NodeMCU ESP32 - Gateway Communication
NodeMCU ESP32 serves as the gateway between the vehicle's main ECU and the FOTA server, enabling Wi-Fi connectivity for updates.

![NodeMCU ESP32](https://upload.wikimedia.org/wikipedia/commons/thumb/c/c2/NodeMCU.jpg/1024px-NodeMCU.jpg)

### 3. STM32F401RCT6 - Main and Application ECUs
The STM32F401RCT6 microcontroller is the core component for managing the vehicle's embedded systems.

![STM32F401RCT6](https://upload.wikimedia.org/wikipedia/commons/thumb/7/7e/STM32F401_Discovery_Board.jpg/1024px-STM32F401_Discovery_Board.jpg)

### 4. HC-05 Bluetooth Module
Used for wireless communication between the vehicle and mobile applications.

![HC-05 Bluetooth Module](https://upload.wikimedia.org/wikipedia/commons/thumb/5/59/HC-05_Bluetooth_Module.jpg/800px-HC-05_Bluetooth_Module.jpg)

### 5. Mobile Application
Developed to provide a user-friendly interface for managing updates and system notifications.

---

## Hardware Components

- **STM32F401RCT6 Microcontroller:** Serves as the core processing unit, managing both the main and application ECUs.
- **NodeMCU ESP32:** Acts as a gateway for receiving updates from the server.
- **HC-05 Bluetooth Module:** Facilitates communication between the vehicle and mobile devices.
- **Ultrasonic Sensor:** Detects obstacles for navigation and safety.
- **H-Bridge Motor Driver:** Controls the vehicle's DC motors for movement.

---

## Software Components

- **FOTA Server (Django):** Manages the distribution and deployment of firmware updates.
- **Secure Bootloader:** Validates and loads authenticated firmware onto the ECUs.
- **Mobile Application:** Provides an interface for vehicle owners to interact with the FOTA system.

---

## Installation

To set up the FOTA system, follow these steps:

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/fota-system.git
2. **Set up the hardware** according to the pinout configuration in the documentation.
3. **Flash the microcontroller** with the bootloader firmware using the provided scripts.
4. **Deploy the FOTA server** on your preferred hosting environment.

## Usage
1. **Connect the vehicle's ECU** to the FOTA server using the NodeMCU ESP32.
2. **Use the mobile application** to check for updates and manage notifications.
3. **Accept or snooze** updates directly from the vehicle's dashboard interface.

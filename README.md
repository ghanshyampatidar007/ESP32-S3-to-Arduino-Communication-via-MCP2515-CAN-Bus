# ESP32-S3 to Arduino Communication via MCP2515 (CAN Bus)

## Overview

This project demonstrates how to implement reliable 8-byte data communication between an **ESP32-S3 microcontroller** and an **Arduino UNO** using the **MCP2515 CAN Bus controller** on both ends. It uses the **CAN protocol** to transmit data efficiently and robustly in embedded systems.

## Hardware Used

- ESP32-S3 DevKitC
- Arduino UNO
- 2x MCP2515 CAN Bus Modules with TJA1050 transceivers
- Jumper wires
- Common GND connection

## Features

- ESP32-S3 acts as the **CAN transmitter**.
- Arduino acts as the **CAN receiver**.
- 8 bytes of data are sent cyclically from ESP32-S3 to Arduino.
- Communication is handled using MCP2515 over the **SPI interface**.
- Baud rate: **500 kbps**
- MCP2515 oscillator frequency: **8 MHz**

## Wiring (Both ESP32-S3 and Arduino to MCP2515)

| MCP2515 Pin | ESP32-S3        | Arduino UNO |
|-------------|------------------|-------------|
| VCC         | 5V               | 5V          |
| GND         | GND              | GND         |
| CS          | GPIO10           | D10         |
| SO (MISO)   | GPIO13           | D12         |
| SI (MOSI)   | GPIO11           | D11         |
| SCK         | GPIO12           | D13         |
| INT         | GPIO9            | D2 (optional) |

> Note: Ensure **common ground** between ESP32-S3, Arduino, and both MCP2515 modules.

## Software Stack

### On ESP32-S3 (Transmitter)
- **Zephyr RTOS** (v4.1.0)
- MCP2515 CAN driver
- Sends 8 bytes of predefined data repeatedly over CAN

### On Arduino UNO (Receiver)
- **Arduino IDE**
- **mcp2515** library (`https://github.com/autowp/arduino-mcp2515`)
- Receives and prints 8 bytes via Serial Monitor

## Transmitted CAN Data Example
```c
uint8_t data[8] = {0x23, 0xAB, 0xCD, 0x98, 0x23, 0xAB, 0xCD, 0x98};

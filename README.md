# 🌱 Soil Analysis System - IoT Based Agriculture Monitoring

[![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Research](https://img.shields.io/badge/Research-Paper-blue?style=for-the-badge)](./research-paper.pdf)


## 📋 Overview

This project presents an **IoT-based Soil Analysis System** that measures crucial soil parameters for agricultural applications. The system provides real-time monitoring of:

- 💧 **Soil Moisture Content** (0-100%)
- 🌡️ **Temperature** (Celsius)
- 🧪 **pH Value** (0-14 scale)
- 💊 **Mineral/Nutrient Levels** (N, P, K, Ca, Mg)

## 🎯 Features

| Feature | Description |
|---------|-------------|
| Real-time Monitoring | Continuous soil parameter measurement |
| LCD Display | Local visualization of all readings |
| Crop Recommendation | Suggests suitable crops based on soil conditions |
| Fertilizer Advisory | Recommends fertilizer based on nutrient levels |
| Soil Health Score | Overall soil quality assessment (0-100) |
| Serial Data Output | Easy integration with external systems |

## 🏗️ Hardware Components
 Soil Analysis System │
├─────────────────────────────────────────────────────────┤
│ • Arduino UNO / Nano (Microcontroller) │
│ • SSD1306 OLED Display (128x64) │
│ • Soil Moisture Sensor (Analog) │
│ • pH Sensor Module │
│ • Mineral Detection Sensor │
│ • Modbus RS485 Soil Sensor (Optional) │
│ • 5V Power Supply │

## 🔧 Pin Connections

| Component | Arduino Pin |
|-----------|-------------|
| OLED SDA | A4 |
| OLED SCL | A5 |
| pH Sensor | A0 |
| Moisture Sensor | A1 |
| Mineral Sensor | A2 |
| Modbus RE | D8 |
| Modbus DE | D7 |
| Modbus RX | D2 |
| Modbus TX | D3 |

## 📊 System Architecture

│ Sensors │────▶│ Arduino │────▶│ OLED │
│ │ │ UNO │ │ Display │

│
▼

│ Serial │
│ Output │

## 🚀 Installation & Setup

### 1. Install Arduino IDE
Download from: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

### 2. Install Required Libraries

Open Arduino IDE → Sketch → Include Library → Manage Libraries

Install these libraries:
- `Adafruit SSD1306` - OLED Display Driver
- `Adafruit GFX` - Graphics Library
- `SoftwareSerial` - Modbus Communication



# ✈️ SMART PITOT
### Embedded Air Data Acquisition & Processing System

<p align="center">
  <strong>A custom STM32-based air data computer for experimental aerodynamic measurements.</strong>
</p>

<p align="center">
  ![Platform](https://img.shields.io/badge/Platform-STM32L476RG-03234B?style=for-the-badge&logo=stmicroelectronics)
  ![Language](https://img.shields.io/badge/Language-Embedded%20C-00599C?style=for-the-badge&logo=c)
  ![Status](https://img.shields.io/badge/Status-In%20Development-orange?style=for-the-badge)
</p>

---

## 🚀 Overview

**SMART PITOT** is an embedded air data measurement system designed to acquire, process and display atmospheric pressure measurements for experimental aeronautical applications.

Originally developed using Arduino, the project is being migrated to the **STM32L476RG**, with custom low-level peripheral drivers and a modular embedded software architecture.

The system uses two independent BME280 sensors to acquire total and static pressure measurements, which are processed to estimate several aerodynamic parameters.

<p align="center">
  <em>From raw sensor measurements to meaningful air data.</em>
</p>

---

## 🧩 System Architecture

```mermaid
flowchart TB
    MCU["STM32L476RG"]

    MCU --> I2C1["I2C1"]
    MCU --> I2C2["I2C2"]
    MCU --> I2C3["I2C3"]

    I2C1 --> LCD["LCD1602"]
    I2C2 --> BME1["BME280 #1<br/>Total Pressure"]
    I2C3 --> BME2["BME280 #2<br/>Static Pressure"]

    BME1 --> ACQ["Data Acquisition"]
    BME2 --> ACQ

    ACQ --> FILTER["Kalman Filtering"]
    FILTER --> CALC["Air Data Computation"]
    CALC --> DISPLAY["LCD Display"]
```

### Hardware configuration

| Component | Interface | Role |
|:---|:---:|:---|
| STM32L476RG | — | Main controller |
| BME280 #1 | I2C2 | Total pressure measurement |
| BME280 #2 | I2C3 | Static pressure measurement |
| LCD1602 | I2C1 | Data visualization |

---

## ⚙️ Features & Objectives

| Feature | Description |
|:---|:---|
| 🛰️ Pressure acquisition | Independent total and static pressure measurements |
| 🌡️ Environmental sensing | Temperature and relative humidity |
| 📉 Digital filtering | Kalman filtering for sensor measurements |
| 💨 Airspeed computation | IAS, CAS, EAS and TAS estimation |
| 📏 Altitude estimation | Barometric altitude calculation |
| 🖥️ Real-time display | Measurement visualization on LCD1602 |
| 🔧 Custom drivers | Low-level I2C, BME280 and LCD drivers |

---

## 🏗️ Software Architecture

The software is organized into independent layers to separate hardware communication, data acquisition and aerodynamic calculations.

```text
SMART_PITOT/
│
├── Core/
│   ├── Inc/
│   └── Src/
│       └── main.c
│
├── Drivers/
│   ├── I2C/
│   │   ├── Inc/
│   │   └── Src/
│   │
│   ├── BME280/
│   │   ├── Inc/
│   │   └── Src/
│   │
│   └── LCD/
│       ├── Inc/
│       └── Src/
│
├── Application/
│   ├── Inc/
│   └── Src/
│       ├── acquisition.c
│       ├── filtering.c
│       ├── air_data.c
│       └── display.c
│
├── Algorithms/
│   ├── Inc/
│   └── Src/
│       ├── kalman.c
│       └── atmosphere.c
│
└── README.md
```

<sub>Target architecture — directory organization may evolve during development.</sub>

### Processing pipeline

```text
  SENSOR DATA
       │
       ▼
  ACQUISITION
       │
       ▼
  KALMAN FILTERING
       │
       ▼
  AERODYNAMIC COMPUTATION
       │
       ├── IAS
       ├── CAS
       ├── EAS
       ├── TAS
       └── ALTITUDE
       │
       ▼
  LCD1602 DISPLAY
```

---

## 📐 Air Data Parameters

The system aims to calculate the following parameters:

| Symbol | Parameter | Unit |
|:---:|:---|:---:|
| IAS | Indicated Airspeed | m/s |
| CAS | Calibrated Airspeed | m/s |
| EAS | Equivalent Airspeed | m/s |
| TAS | True Airspeed | m/s |
| \(P_t\) | Total Pressure | Pa |
| \(P_s\) | Static Pressure | Pa |
| \(\rho\) | Air Density | kg/m³ |
| \(h\) | Barometric Altitude | m |

---

## 🛠️ Development Environment

<p align="center">
  <img src="https://img.shields.io/badge/C-Embedded%20Programming-00599C?style=flat-square&logo=c" />
  <img src="https://img.shields.io/badge/STM32-Microcontroller-03234B?style=flat-square&logo=stmicroelectronics" />
  <img src="https://img.shields.io/badge/CMake-Build%20System-064F8C?style=flat-square&logo=cmake" />
  <img src="https://img.shields.io/badge/VS%20Code-IDE-007ACC?style=flat-square&logo=visualstudiocode" />
  <img src="https://img.shields.io/badge/GNU%20ARM-Toolchain-A42E2B?style=flat-square&logo=gnu" />
</p>

| Tool / Technology | Purpose |
|:---|:---|
| Embedded C | Firmware development |
| STM32L476RG | Target microcontroller |
| CMake | Build configuration |
| ARM GNU Toolchain | Cross-compilation |
| Visual Studio Code | Development environment |
| ST-LINK | Hardware debugging and programming |
| STM32CubeProgrammer | Flash programming |

---

## 📍 Development Roadmap

- [x] Custom I2C1 driver
- [x] Custom I2C2 driver
- [x] Custom I2C3 driver
- [x] BME280 driver
- [x] LCD1602 driver
- [ ] Hardware integration on STM32L476RG
- [ ] Dual-sensor acquisition module
- [ ] Kalman filtering implementation
- [ ] Atmospheric model implementation
- [ ] Air data computation
- [ ] LCD display integration
- [ ] Experimental validation

---

## ⚠️ Experimental Project

SMART PITOT is an experimental engineering project intended for educational, development and laboratory applications.

The use of two absolute-pressure BME280 sensors to estimate small pressure differences requires careful consideration of sensor accuracy, resolution, noise and calibration.

**This project is not a certified aviation instrument and must not be used as the sole source of flight-critical airspeed or altitude information.**

---

<p align="center">
  <strong>SMART PITOT</strong><br/>
  <sub>Embedded Systems · Aeronautics · Sensor Fusion · Air Data</sub>
</p>

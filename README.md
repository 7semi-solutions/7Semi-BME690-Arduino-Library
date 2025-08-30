# 7Semi-BME690-Arduino-Library

This Arduino library provides support for the **7Semi BME690 Sensor Module**, a powerful 4-in-1 environmental sensor capable of measuring **temperature**, **humidity**, **barometric pressure**, and **gas resistance (VOC sensing)** via I2C. It is ideal for air quality monitoring, weather stations, and smart environmental IoT systems.

![Arduino](https://img.shields.io/badge/platform-arduino-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Status](https://img.shields.io/badge/status-active-brightgreen.svg)

---

## Hardware Required

- 7Semi BME690 Sensor Module  
- Arduino-compatible board  
- **I2C (SDA, SCL)** or **SPI (MOSI, MISO, SCK, CS)** connection  

---

## Getting Started

### 1. Library Installation

The recommended way to install this library is via the **Arduino Library Manager**:

1. Open the **Arduino IDE** (v1.8.13+ or Arduino IDE 2.x)  
2. Navigate to  
   - `Sketch` > `Include Library` > `Manage Libraries…` (Arduino IDE 1.x), or  
   - `Tools` > `Manage Libraries…` or click 📚 (IDE 2.x sidebar)
3. Search for:
   " 7Semi BME690 "
4. Click **Install**

Once installed, include it in your sketch:
```cpp
" #include <7semi_bme690.h> "

🔎 Tip: You can find all 7Semi libraries by just searching for 7Semi in the Library Manager.


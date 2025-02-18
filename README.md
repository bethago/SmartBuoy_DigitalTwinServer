# Digital Twin Server (C++)

**C++-based Digital Twin system** for processing and analyzing Smart Buoy sensor data in real time.

This project is designed to interface with:
- **Smart Buoy (Client)** sending sensor data via oneM2M
- **Unreal Engine 5-based Visualization Server** for 3D representation

---

## 📄 Overview
- Fetches and manages real-time data from multiple Smart Buoy devices.
- Creates and maintains **TwinDevice** objects, each representing a physical buoy in the virtual environment.
- Performs wave period calculations, evaluates the danger level, and sends updates to the Visualization Server.
- Utilizes [cpprestsdk](https://github.com/microsoft/cpprestsdk), [Boost](https://www.boost.org/), and [nlohmann/json](https://github.com/nlohmann/json) libraries.

---

## 🚀 How to Build & Run

### 1. Requirements
- C++17-compatible compiler (Visual Studio 2019+, or g++ 9+)
- CMake 3.30+
- Dependencies: `cpprestsdk`, `Boost`, `nlohmann_json`
    - If using vcpkg, make sure `VCPKG_ROOT` is defined in your environment.

### 2. Build Steps
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 3. Run
- Once compiled, run the generated `main` executable:
  ```bash
  ./main
  ```
- You will be prompted to select Real-time Communication (currently supported) or other modes.

---

## 📂 File Structure

```
.
├── CMakeLists.txt           # Build configuration
├── config.h                 # Compile-time configs for server IP, port, etc.
├── main.cpp/.h              # Entry point
├── oneM2M.cpp/.h            # OneM2M client functionalities (verify CSE, fetch GRP, discovery CIN)
├── TwinDevice.cpp/.h        # Digital Twin model (wave period calc, danger evaluation)
├── UnrealEngine.cpp/.h      # Sends device data to UE5 visualization
└── README.md
```

---

## ✨ Key Features
- **Real-time Data Retrieval**: Uses oneM2M discovery to fetch latest sensor values (CIN).
- **Wave Danger Assessment**: Basic algorithm to classify wave conditions into SAFE/CAUTION/ALERT/DANGER.
- **3rd-Party Communication**: Sends structured JSON data to UE5 visualization server.

---

## 📌 Notes
- Adjust `config.h` for your oneM2M server URL, port, and device naming.
- Ensure the oneM2M CSE server is running and the buoy devices are active.
- Make sure the Visualization Server is also running to receive and display device updates.


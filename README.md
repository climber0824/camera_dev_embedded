# Surveillance System

A C++ surveillance system supporting USB and IP cameras with motion detection.

## Features

- 📹 **Multi-camera Support**: USB and IP camera integration
- 🎯 **Motion Detection**: Real-time motion detection with configurable sensitivity

- 🔒 **Thread-safe**: Concurrent camera access with mutex protection
- 🎨 **Extensible Design**: Easy to add new camera types and features
- 🚀 **Modern C++**: Uses C++11 features and smart pointers

## Project Structure
```
surveillance_system/
│
├── README.md
├── CMakeLists.txt
├── LICENSE
│
├── include/                    # Header files
│   ├── Camera.h               # Abstract base class for all cameras
│   ├── USBCamera.h            # USB camera implementation
│   ├── IPCamera.h             # IP/Network camera implementation
│   ├── CameraManager.h        # Camera lifecycle management
│   ├── MotionDetector.h       # Motion detection algorithms
│   ├── VideoRecorder.h        # Video recording functionality
│   └── SurveillanceSystem.h   # Main system coordinator
│
├── src/                        # Implementation files
│   ├── Camera.cpp
│   ├── USBCamera.cpp
│   ├── IPCamera.cpp
│   ├── CameraManager.cpp
│   ├── MotionDetector.cpp
│   ├── VideoRecorder.cpp
│   └── SurveillanceSystem.cpp
│
├── examples/                   # Example applications
│   ├── main.cpp               # Basic usage example
│   ├── multi_camera.cpp       # Multi-camera monitoring
│   └── motion_recording.cpp   # Motion-triggered recording
│
├── tests/                      # Unit tests
│   ├── test_camera.cpp
│   ├── test_manager.cpp
│   └── test_motion.cpp
│
├── docs/                       # Documentation
│   ├── API.md
│   ├── ARCHITECTURE.md
│   └── USAGE.md
│
└── build/                      # Build output (generated)
```

## Dependencies

- **OpenCV 4.x**: Computer vision and camera handling
- **C++11** or later
- **CMake 3.10+**: Build system
- **pthread**: Threading support (usually included)

## Installation

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libopencv-dev

# Clone repository
git clone https://github.com/yourusername/surveillance_system.git
cd surveillance_system

# Build
mkdir build && cd build
cmake ..
make

# Run
./surveillance_system
```

### macOS
```bash
# Install dependencies
brew install opencv cmake

# Clone and build
git clone https://github.com/yourusername/surveillance_system.git
cd surveillance_system
mkdir build && cd build
cmake ..
make
./surveillance_system
```

## Quick Start
```cpp
#include "SurveillanceSystem.h"
#include "USBCamera.h"
#include "IPCamera.h"

int main() {
    SurveillanceSystem system;

    // Add USB camera
    auto usbCam = std::make_shared<USBCamera>("cam1", "Front Door", 0);
    system.addCamera(usbCam);

    // Add IP camera
    auto ipCam = std::make_shared<IPCamera>("cam2", "Backyard", "192.168.1.100");
    ipCam->setCredentials("admin", "password");
    system.addCamera(ipCam);

    // Enable motion detection
    system.enableMotionDetection("cam1", 30);

    // Start recording
    system.startRecording("cam2", "output.avi");

    // Start surveillance
    system.start();

    return 0;
}
```

## API Overview

### Camera Classes
```cpp
// USB Camera
USBCamera cam("id", "name", deviceIndex);
cam.connect();
cam.captureFrame();

// IP Camera
IPCamera cam("id", "name", "192.168.1.100", 554);
cam.setCredentials("user", "pass");
cam.connect();
```

### Camera Manager
```cpp
CameraManager manager;
manager.addCamera(camera);
manager.connectAll();
manager.getAllCameras();
```

### Surveillance System
```cpp
SurveillanceSystem system;
system.addCamera(camera);
system.enableMotionDetection(cameraId, threshold);
system.startRecording(cameraId, filename);
system.start();
```

## Configuration

Create a `config.json` file:
```json
{
  "cameras": [
    {
      "id": "front_door",
      "type": "usb",
      "device": 0,
      "resolution": [1280, 720],
      "fps": 30
    },
    {
      "id": "backyard",
      "type": "ip",
      "address": "192.168.1.100",
      "port": 554,
      "username": "admin",
      "password": "password",
      "stream_url": "rtsp://192.168.1.100:554/stream"
    }
  ],
  "motion_detection": {
    "enabled": true,
    "threshold": 25,
    "min_area": 500
  },
  "recording": {
    "codec": "XVID",
    "output_dir": "./recordings/"
  }
}
```

## Architecture
```
┌─────────────────────────────────────┐
│     SurveillanceSystem              │
│  (Main Coordinator/Facade)          │
└──────────────┬──────────────────────┘
               │
       ┌───────┴────────┐
       │                │
       ▼                ▼
┌──────────────┐  ┌─────────────┐
│CameraManager │  │MotionDetector│
└──────┬───────┘  └─────────────┘
       │
       ├─────────┬─────────┐
       ▼         ▼         ▼
   ┌────────┐ ┌──────┐ ┌──────┐
   │ Camera │ │USBCam│ │IPCam │
   │ (Base) │ └──────┘ └──────┘
   └────────┘
```

## Testing
```bash
cd build
ctest --verbose

# Or run individual tests
./tests/test_camera
./tests/test_manager
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Roadmap
- [ ]  📼 **Video Recording**: Automatic recording with motion triggers
- [ ] Cloud storage integration
- [ ] Email/SMS alerts on motion detection
- [ ] Web interface for remote monitoring
- [ ] AI-based object detection
- [ ] H.265/HEVC codec support
- [ ] Multi-zone motion detection
- [ ] Face recognition
- [ ] Mobile app integration

## License

This project is licensed under the MIT License - see the [MIT](https://github.com/climber0824/camera_dev_embedded/blob/main/LICENSE.md) file for details.

## Authors

- **Robert Chang** - *Initial work* - [climber0824](https://github.com/climber0824)

## Acknowledgments

- OpenCV community for computer vision library
- Contributors and testers

## Support

For support, email meteorx900824@gmail.com or open an issue in the GitHub repository.

---

**⭐ Star this repository if you find it helpful!**

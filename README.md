# Console monitoring application for Mini-Box DCDC NUC

![alt text](http://www.mini-box.com/Mini-Box-DCDC-NUC-b.jpg "DCDC NUC")


## Installation

Before compiling the application make sure libusb dependency is installed. In Ubuntu it can be installed using the following command:

```
sudo apt-get install libusb-dev
```

To compile:

```
cmake CMakeLists.txt
make
sudo make install
```

To run:
```
sudo /usr/local/dcdc_nuc_console
```

## Output

```DCDC NUC PSU found in Automotive mode
Firmware Verion 1.0
Input Voltage: 0.121
Input Current: 0.000
Output Voltage: 0.000
Output Current: 0.000
Output Power: 0.000
Ignition Voltage: 0.000
Temperature: 1000.000
Thump Voltage: 0.000
State: Low Power
Mode: Automotive
Output Enabled: 0
Input Voltage Good: 0
Ignition Voltage Good: 0
Mobo Alive (USB): 1
Mobo Alive (Power): 0
Ignition Raised: 0
Thump Ouput Enbaled: 0
Ignition Cancel timer reached
Auto-shutdown enabled
```

This application is based on source code from McGill Robotics: https://github.com/mcgill-robotics/ros-dcdc-nuc

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
Input Voltage: 14.371
Input Current: 0.040
Output Voltage: 12.216
Output Current: 0.040
Output Power: 0.488
Ignition Voltage: 14.462
Temperature: 27.667
Thump Voltage: 14.325
State: Normal
Mode: Automotive
Output Enabled: 1
Input Voltage Good: 1
Ignition Voltage Good: 1
Mobo Alive (USB): 1
Mobo Alive (Power): 0
Ignition Raised: 1
Thump Ouput Enbaled: 1
Ignition Cancel timer reached
Auto-shutdown enabled

```

This application is based on source code from McGill Robotics: https://github.com/mcgill-robotics/ros-dcdc-nuc

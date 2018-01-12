# Console monitoring application for Mini-Box DCDC NUC

[alt text] !(http://www.mini-box.com/Mini-Box-DCDC-NUC-b.jpg "DCDC NUC")


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


This application is based on source code from McGill Robotics: https://github.com/mcgill-robotics/ros-dcdc-nuc

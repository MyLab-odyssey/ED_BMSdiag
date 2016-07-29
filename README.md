# ED BMSdiag
Retrieve battery diagnostic data from your smart electric drive EV.

```  
-----------------------------------------
Time [hh:mm]: 22:24,   ODO : 19440 km
-----------------------------------------
Battery-Production [Y/M/D]: 2013/9/30
Rev.[Y/WK/PL] HW:2013/29/0, SW:2013/29/1
-----------------------------------------
SOC : 80.5 %, realSOC: 79.0 %
HV  : 366.5 V, 0.00 A, 0.00 kW
LV  : 12.3 V
-----------------------------------------
CV mean : 3939 mV, dV = 46 mV
CV min  : 3915 mV
CV max  : 3961 mV
OCVtimer: 1338 s
-----------------------------------------
Last measurement      : 0 day(s)
Measurement estimation: 0.824
Actual estimation     : 0.808
CAP mean: 19098 As/10, 53.0 Ah
CAP min : 17873 As/10, 49.6 Ah
CAP max : 18928 As/10, 52.6 Ah
-----------------------------------------
HV contactor state OFF, for: 18333 s
cycles left   : 295861
of max. cycles: 300000
DC isolation  : 4216 kOhm, NO FAULT
-----------------------------------------
Temperatures Battery-Unit /degC:
module 1: 26.7, 28.8, 27.2
module 2: 27.5, 29.3, 27.2
module 3: 27.0, 28.6, 26.4
   mean : 27.6, min : 26.4, max : 29.4
coolant : 25.4```

### You need
An Arduino with CAN bus shield to connect to the diagnostics port (OBDII-connector) of your car. Get the hardware and use an appropriate cable for the physical connection. See the schematics for making your own cable.

<p align="center">
<img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/Arduino%26CANbusShield.jpg" width="640"/>
<p/>

### Get started
* Download the repo-ZIP, then copy the files to your Arduino folder. The folder structure should look like this:
<p align="left">
<img  src="https://github.com/MyLab-odyssey/ED_BMSdiag/raw/master/pictures/Arduino_folder_structure.png" /><p/>

* Open the ED_BMSdiag.ino file and compile / upload it to the Arduino board.

Please see the [wiki](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki) for further information.

### Usage [>on your own risk<]
Connect the CAN shield to the OBDII-connector and power up the car.

This simple tool will display the diagnostics via a serial USB connection. The readout will be started by entering commands after the prompt. See the [wiki for further details of the CLI](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Command-Line-Interface).


>**You need to open the serial monitor of the Arduino-IDE.  Verify that it is set to CR = Carriage Return and the baud rate is 115200.**

<p align="center">
<img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/Arduino%20-IDE_serial_monitor.png" />
<p/>

### Version history
version  | comment
-------- | --------
v0.5.0   | Added a CLI and new readouts:
         | ... NLG6 fast charger
         | ... Cooling- and other subsystems
         | ... Logging function
v0.4.2   | **please reload all files !**
         | Bugfix for Average-Template memory bug > rewritten  to AvgNew-Class
         | ... This version uses dynamic memory allocation, so monitor free memory (between heap and stack) if you make modifications!
v0.4.0   | New canDiag class structure and BMS_dfs.h for BMS specific code
v0.3.9b  | **now using semantic versioning** (old files unchanged)
v0.39b   | fixed type cast bug HVcontactor-timer
v0.39    | Optimized memory usage to gain ~ 560 Bytes of SRAM for more features.
(For more entries see VERSIONS.md)

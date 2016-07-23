# ED BMSdiag
Retrieve battery diagnostic data from your smart electric drive EV.

 <p align="center">
 <img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/ED_BMSdiag_example_data.png" />
 <p/>

## You need
An Arduino with CAN bus shield to connect to the diagnostics port (OBDII-connector) of your car. Get the hardware and use an appropriate cable for the physical connection. See the schematics for making your own cable.

<p align="center">
<img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/Arduino%26CANbusShield.jpg" width="640"/>
<p/>

## Get started
* Download the repo-ZIP, then copy the files to your Arduino folder. The folder structure should look like this:
<p align="left">
<img  src="https://github.com/MyLab-odyssey/ED_BMSdiag/raw/master/pictures/Arduino_folder_structure.png" /><p/>

* Open the ED_BMSdiag.ino file and compile / upload it to the Arduino board.

Please see the [wiki](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki) for further information.

## Usage (on your own risk!)
Connect the CAN shield to the OBDII-connector and power up the car.

This simple version will display the diagnostics via a serial USB connection. You need to open the serial monitor of the Arduino-IDE. Verify that the baud rate is set to 115200. **The readout will be started by entering a serial command / keystroke in the top input line.** You will get an text output of the current battery status.
<p align="center">
<img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/Arduino%20-IDE_serial_monitor.png" />
<p/>

## Version history
version  | comment
-------- | --------
v0.4.2   | **please reload all files !**
         | Bugfix for Average-Template memory bug > rewritten  to AvgNew-Class
         | ... This version uses dynamic memory allocation, so monitor free memory (between heap and stack) if you make modifications!
v0.4.0   | New canDiag class structure and BMS_dfs.h for BMS specific code
v0.3.9b  | **now using semantic versioning** (old files unchanged)
v0.39b   | fixed type cast bug HVcontactor-timer
v0.39    | Optimized memory usage to gain ~ 560 Bytes of SRAM for more features.
(For more entries see VERSIONS.md)

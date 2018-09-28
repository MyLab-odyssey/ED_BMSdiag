# ED BMSdiag

[![version](https://img.shields.io/badge/version-v1.0.5-blue.svg)](https://github.com/MyLab-odyssey/ED_BMSdiag/archive/master.zip)
[![version](https://img.shields.io/badge/issues-none-brightgreen.svg)](https://github.com/MyLab-odyssey/ED_BMSdiag/issues)
[![release](https://img.shields.io/badge/release-v1.0.5-brightgreen.svg)](https://github.com/MyLab-odyssey/ED_BMSdiag/releases)
[![license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/MyLab-odyssey/ED_BMSdiag/blob/master/LICENSE.txt)

Retrieve battery diagnostic data from your smart electric drive EV. Get a Status Report to rate the health of the battery or dig into more detailed measurements.  

>**Further documentation in the [Wiki](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki).**

>**The development is now basically finished with the final release of v1.0.5 .**  

>**The software will only work on the third generation Smart electric drive vehicle build from late 2012 to mid 2015.**

>**You can buy a pre-assembled version of this tool. More information [here](http://www.sokoloff.com/smart451ED-BMS/).**

---

## You need
An Arduino with CAN bus shield to connect to the diagnostics port (OBDII-connector) of your car. Find information about usable [CAN-Shields here](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/CAN-Bus-Basics). If you have trouble getting a CAN connection please see the [Troubleshooting and Tips](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Troubleshooting-and-Tips) section.

Get the hardware and use an appropriate cable for the physical connection. See the schematics for making your own cable.

<p align="center">
<img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/Arduino%26CANbusShield.jpg" width="640"/>
<p/>

## Get started
* Download the [repo-ZIP](https://github.com/MyLab-odyssey/ED_BMSdiag/archive/master.zip), then copy the files to your **Arduino folder** (location depends on OS). The folder structure should look like this:
<p align="left">
<img  src="https://github.com/MyLab-odyssey/ED_BMSdiag/raw/master/pictures/Arduino_folder_structure.png" /><p/>

* Open the ED_BMSdiag.ino file and compile / upload it to the Arduino board.  

* **Find detailed installation instructions in: [english](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Installation) | [german](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Installation_DE).**

> **Verified to work with Arduino IDE 1.8.4 (on OS X 10.12 and WIN-Systems)**

> **Verified to work with Arduino IDE 1.8.5 (on OS X 10.13)**

## Usage >on your own risk<
Connect the CAN shield to the OBDII-connector and power up the car.

This simple tool will display the diagnostics via a serial USB connection. The readout will be started by entering commands after the prompt. See the [Wiki for further details of the CLI](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Command-Line-Interface).


>**You need to open the serial monitor of the Arduino-IDE.  Verify that it is set to CR = Carriage Return and the baud rate is 115200.**

<p align="center">
<img  src="https://raw.githubusercontent.com/MyLab-odyssey/ED_BMSdiag/master/pictures/Arduino%20-IDE_serial_monitor.png" />
<p/>

## Version history
|version  | comment|
|-------- | --------|
|v1.0.5   | Internal:|
|         | ... Eliminate compiler warnings throughout the code. Two warnings remain, but are in the EEPROM library|
|v1.0.4   | Feature:|
|         | ... When battery SOH flags show "DEGRADED", output individual flags|
|v1.0.3   | Small improvement:|
|         | ... Experimental/unverified data now configurable at use rather than at compile time|
|v1.0.2   | Small improvements:|
|         | ... Now supports automatic dumping of all diagnostic data upon connection|
|         | ... Configuration is now stored to onboard EEPROM, so device comes up in the same state as it was last used (for logging settings and intial dump on/off)|
|v1.0.1   | **Final Release - Project finished**. Small bugfixes and improvements:|
|         | ... Battery Status Flag shows health status of the pack [OK, FAULT].|
|         | ... Outliners count in box plot now excludes min- / max-values.|
|         | ... Battery Status Report optimized with hints for better charging / capacity measurement.|
|v0.9.2   | **Final Release Canidate** with bugfixes and new features. **Please reload all files (including library files) !!!**|
|         | `all` command in MAIN menue will run all test.|
|         | `rpt` command will get a Battery Status Report. This completed feature [#12](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/12).|
|v0.7.1   | Now showing outliners count in the box plot. Feature [#13](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/13) completed. See [Wiki for description](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Data-Overview).|
|**v0.7.0**   |**please reload all files (including library files) !!!**|
|         | Now the cell voltage distribution is shown as box plot in the `bms all` query and some new features are implemented:|
|         | ... in the CLI the input is no more case sensitive|
|         | ... the local echo can be disabled with the `ECHO`-switch in the `ED_BMSdiag.h`|
|v0.6.2   | Standard charger now implemented as **OBL** command in CLI (feature [#11](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/11)):|
|         | ... use from `CMD >>` prompt with the command `obl all` to get all readouts from the LEAR **O**n**B**oard**L**oader. See `?` for help in all menu levels and take a look at the [Command Reference of the Wiki](https://github.com/MyLab-odyssey/ED_BMSdiag/wiki/Command-Line-Interface#obl-submenu-prompt-obl--).|
|         | ... THX to *erich* & *AlterAmi* for their support|
|v0.6.0   | Some new features and minor bug fixes:|
|         | ... make runtime memory more efficient by skipping unused data (>200 bytes saved during cell readout). See feature [#9](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/9)|
|         | ... show **F**actory **A**cceptance **T**esting date of the completed battery |
|         | ... tested / corrected display of negative cooling-temperatures (done in wintertime)|
|v0.5.6   | Fixed memory leak issue [#8](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/8).|
|v0.5.5   | NLG6 fast charger SW revisions and HW PN now reported (see issue [#7](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/7)).|
|         | ... power calculation now updated in BMS submenu ([#5](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/5) fixed)|
|         | ... canDiag-Library updated to real time data of drivetrain ([#6](https://github.com/MyLab-odyssey/ED_BMSdiag/issues/6), not used by now in this project)|
|v0.5.2   | NLG6 fast charger is now detected by hardware part number.|
|         | ... power calculation now corrected|
|v0.5.1   | Small bugfix for logging min, max HV-Voltage not updating|
|         | ... added HV-Ri to logging values|
|         | ... new readout for cooling fan (RPM / OTR)|
|v0.5.0   | Added a CLI and new readouts:|
|         | ... NLG6 fast charger|
|         | ... Cooling- and other subsystems|
|         | ... Logging function|
|v0.4.2   | **Please reload all files !**|
|         | Bugfix for Average-Template memory bug > rewritten  to AvgNew-Class|
|         | ... This version uses dynamic memory allocation, so monitor free memory (between heap and stack) if you make modifications!|
|v0.4.0   | New canDiag class structure and BMS_dfs.h for BMS specific code.|
|v0.3.9b  | **Now using semantic versioning** (old files unchanged)|
|v0.39b   | Fixed type cast bug HVcontactor-timer.|
|v0.39    | Optimized memory usage to gain ~ 560 Bytes of SRAM for more features.|

(For more entries see VERSIONS.md)

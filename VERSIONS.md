## ED BMSdiag version history
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
|v0.9.2   | **Final Release Candidate** with bugfixes and new features. **Please reload all files (including library files) !!!**|
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
|v0.39    | Optimized memory usage to gain ~ 560 Bytes of SRAM for more features:|
|         | ... usage of min. data type|
|         | ... new structure typedef and templates|
|         | ... small bugfixes|
|v0.38    | New readings and stability improvements:|
|         | ... read HV-Battery voltage even with contactors open|
|         | ... get HV-DC-Isolation in kOhm (reliable with contactors closed)|
|         | ... get info about hardware- / software-revision of battery|
|v0.35a   | Bugfix to continue reading messages if one was skipped.|
|         | ... show 0.00kW power instead of -300.00kW|
|v0.35    | Added new readouts.|
|         | ... realSOC, current, power, time from BC|
|         | ... EXPERIMENTAL: initial capacity & loss|
|         | Changed data formatting and code cleanup|
|v0.31    | Fixed overflow in ODO calc.|
|         | ... no special ASCII-characters for int. support|
|v0.30    | Added more readout data and reformatted the output.|
|         | ... ODO, 12V battery, HV-unit production date|
|         | ... temperatures of battery modules|
|         | ... # of cell at min, max (in individual cell data)|
|v0.24    | Overall improvements|
|         | ... added Doxygen compatible comments|
|         | ... added standard deviation calc.|
|         | ... changed formatting|
|         | Average.h (**please reload library**)|
|         | ... corrected vice versa cell mapping|
|         | ... overflow corrected|
|v0.21    | Waiting for serial init|
|         | ... Showing status of OBD port (CAN-Bus)|
|v0.2     | Initial Commit|
|         | ... Tested with Arduino R3 and Sparkfun CAN-Bus shield|

## ED BMSdiag version history
version  | comment
-------- | --------
v0.4.2   | **please reload all files !**
         | Bugfix for Average-Template memory bug > rewritten  to AvgNew-Class
         | ... This version uses dynamic memory allocation, so monitor free memory (between heap and stack) if you make modifications!
v0.4.0   | New canDiag class structure and BMS_dfs.h for BMS specific code
v0.3.9b  | **now using semantic versioning** (old files unchanged)
v0.39b   | fixed type cast bug HVcontactor-timer
v0.39    | Optimized memory usage to gain ~ 560 Bytes of SRAM for more features.
         | ... usage of min. data type
         | ... new structure typedef and templates
         | ... small bugfixes
v0.38    | New readings and stability improvements
         | ... read HV-Battery voltage even with contactors open
         | ... get HV-DC-Isolation in kOhm (reliable with contactors closed)
         | ... get info about hardware- / software-revision of battery
v0.35a   | Bugfix to continue reading messages if one was skipped
         | ... show 0.00kW power instead of -300.00kW
v0.35    | Added new readouts
         | ... realSOC, current, power, time from BC
         | ... EXPERIMENTAL: initial capacity & loss
         | Changed data formatting and code cleanup
v0.31    | Fixed overflow in ODO calc.
         | ... no special ASCII-characters for int. support
v0.30    | Added more readout data and reformatted the output
         | ... ODO, 12V battery, HV-unit production date
         | ... temperatures of battery modules
         | ... # of cell at min, max (in individual cell data)
v0.24    | Overall improvements
         | ... added Doxygen compatible comments
         | ... added standard deviation calc.
         | ... changed formatting
         | Average.h (**please reload library**)
         | ... corrected vice versa cell mapping
         | ... overflow corrected
v0.21    | Waiting for serial init
         | Showing status of OBD port (CAN-Bus)
v0.2     | Initial Commit
         | Tested with Arduino R3 and Sparkfun CAN-Bus shield

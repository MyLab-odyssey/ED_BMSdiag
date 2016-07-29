//--------------------------------------------------------------------------------
// (c) 2016 by MyLab-odyssey
//
// Licensed under "MIT License (MIT)", see license file for more information.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER OR CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//--------------------------------------------------------------------------------
//! \file    ED_BMSdiag_PRN.ino
//! \brief   Functions for serial printing the datasets
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.5.0
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//! \brief   Output a space-line for separation of datasets
//--------------------------------------------------------------------------------
void PrintSPACER() {
  for (byte i = 0; i < 41; i++) Serial.print(F("-"));
  Serial.println();
}

//--------------------------------------------------------------------------------
//! \brief   Output header data as welcome screen - wait for CAN-Bus to be ready
//--------------------------------------------------------------------------------
void printWelcomeScreen() {
  byte vLength = strlen(version);
  PrintSPACER();
  Serial.println(F("--- ED Battery Management Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(version);
  for (byte i = 0; i < (41 - 5 - vLength - 3); i++) {
    Serial.print(" ");
  }
  Serial.println(F("---"));
  PrintSPACER();

  Serial.println(F("Connect to OBD port - Waiting for CAN-Bus "));
  do {
    Serial.print(F("."));
    delay(1000);
  } while (digitalRead(2));
  Serial.println(F("CONNECTED"));
  PrintSPACER();
}

//--------------------------------------------------------------------------------
//! \brief   Output header data
//--------------------------------------------------------------------------------
void printHeaderData() {
  Serial.print(F("Time [hh:mm]: "));
  if (BMS.hour <= 9) Serial.print(F("0"));
  Serial.print(BMS.hour); Serial.print(F(":"));
  if (BMS.minutes <= 9) Serial.print(F("0"));
  Serial.print(BMS.minutes);
  Serial.print(F(",   ODO : ")); Serial.print(BMS.ODO); Serial.println(F(" km"));
}

//--------------------------------------------------------------------------------
//! \brief   Output battery production data
//--------------------------------------------------------------------------------
void printBatteryProductionData() {
  Serial.print(F("Battery-Production [Y/M/D]: ")); Serial.print(2000 + BMS.Year); Serial.print(F("/"));
  Serial.print(BMS.Month); Serial.print(F("/")); Serial.println(BMS.Day);
  Serial.print(F("Rev.[Y/WK/PL] HW:")); Serial.print(2000 + BMS.hw.rev[0]); Serial.print(F("/"));
  Serial.print(BMS.hw.rev[1]); Serial.print(F("/")); Serial.print(BMS.hw.rev[2]);
  Serial.print(F(", SW:")); Serial.print(2000 + BMS.sw.rev[0]); Serial.print(F("/"));
  Serial.print(BMS.sw.rev[1]); Serial.print(F("/")); Serial.println(BMS.sw.rev[2]);
}

//--------------------------------------------------------------------------------
//! \brief   Output experimental data
//--------------------------------------------------------------------------------
void printExperimentalData() {
  Serial.println(F("*** Experimental Data - NOT VERIFIED ***"));
  Serial.print(F("Maximum Capacity @45C: ")); Serial.print(BMS.CapInit / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("Aging-Loss: ")); Serial.print((float) BMS.CapLoss / 1000, 3); Serial.println(F(" %"));
  Serial.print(F("Unknown Counter: 0x")); 
  if (BMS.UnknownCounter[0] <= 9) Serial.print(F("0")); Serial.print(BMS.UnknownCounter[0], HEX);
  if (BMS.UnknownCounter[1] <= 9) Serial.print(F("0")); Serial.print(BMS.UnknownCounter[1], HEX);
  if (BMS.UnknownCounter[2] <= 9) Serial.print(F("0")); Serial.println(BMS.UnknownCounter[2], HEX);
}

//--------------------------------------------------------------------------------
//! \brief   Output standard dataset
//--------------------------------------------------------------------------------
void printStandardDataset() {
  Serial.print(F("SOC : ")); Serial.print(BMS.SOC,1); Serial.print(F(" %"));
  Serial.print(F(", realSOC: ")); Serial.print((float) BMS.realSOC / 10.0, 1); Serial.println(F(" %"));
  Serial.print(F("HV  : ")); Serial.print(BMS.HV,1); Serial.print(F(" V, "));
  Serial.print((float) BMS.Amps/32.0, 2); Serial.print(F(" A, "));
  if (BMS.Power != 0) {
    Serial.print(((float) (BMS.Power / 8192.0) -1) * 300, 2); Serial.println(F(" kW"));
  } else {
    Serial.println(F("0.00 kW"));
  }
  Serial.print(F("LV  : ")); Serial.print(BMS.LV,1); Serial.println(F(" V"));
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS cell voltages
//--------------------------------------------------------------------------------
void printBMS_CellVoltages() {
  Serial.print(F("CV mean : ")); Serial.print(BMS.ADCCvolts.mean); Serial.print(F(" mV"));
  Serial.print(F(", dV = ")); Serial.print(BMS.ADCCvolts.max - BMS.ADCCvolts.min); Serial.println(F(" mV"));
  Serial.print(F("CV min  : ")); Serial.print(BMS.ADCCvolts.min); Serial.println(F(" mV"));
  Serial.print(F("CV max  : ")); Serial.print(BMS.ADCCvolts.max); Serial.println(F(" mV"));
  Serial.print(F("OCVtimer: ")); Serial.print(BMS.OCVtimer); Serial.println(F(" s"));
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS capacity estimation
//--------------------------------------------------------------------------------
void printBMS_CapacityEstimate() {
  Serial.print(F("Last measurement      : ")); Serial.print(BMS.LastMeas_days); Serial.println(F(" day(s)"));
  Serial.print(F("Measurement estimation: ")); Serial.println(BMS.Cap_meas_quality,3);
  Serial.print(F("Actual estimation     : ")); Serial.println(BMS.Cap_combined_quality,3);
  Serial.print(F("CAP mean: ")); Serial.print(BMS.Cap_As.mean); Serial.print(F(" As/10, ")); Serial.print(BMS.Cap_As.mean / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("CAP min : ")); Serial.print(BMS.Cap_As.min); Serial.print(F(" As/10, ")); Serial.print(BMS.Cap_As.min / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("CAP max : ")); Serial.print(BMS.Cap_As.max); Serial.print(F(" As/10, ")); Serial.print(BMS.Cap_As.max / 360.0,1); Serial.println(F(" Ah"));
}

//--------------------------------------------------------------------------------
//! \brief   Output HV contactor state and DC isolation
//--------------------------------------------------------------------------------
void printHVcontactorState() {
  Serial.print(F("HV contactor "));
  if (BMS.HVcontactState == 0x02) {
    Serial.print(F("state ON"));
    Serial.print(F(", low current: ")); Serial.print(BMS.HV_lowcurrent); Serial.println(F(" s"));
  } else if (BMS.HVcontactState == 0x00) {
    Serial.print(F("state OFF"));
    Serial.print(F(", for: ")); Serial.print(BMS.HVoff_time); Serial.println(F(" s"));
  }
  Serial.print(F("cycles left   : ")); Serial.println(BMS.HVcontactCyclesLeft);
  Serial.print(F("of max. cycles: ")); Serial.println(BMS.HVcontactCyclesMax);
  Serial.print(F("DC isolation  : ")); Serial.print(BMS.Isolation); Serial.print(F(" kOhm, "));
  if (BMS.DCfault == 0) {
    Serial.println(F("NO FAULT"));
  } else {
    Serial.println(F("DC FAULT"));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS temperatures
//--------------------------------------------------------------------------------
void printBMStemperatures() {
  Serial.println(F("Temperatures Battery-Unit /degC: "));
  for (byte n = 0; n < 9; n = n + 3) {
    Serial.print(F("module ")); Serial.print((n / 3) + 1); Serial.print(F(": "));
    for (byte i = 0; i < 3; i++) {
      Serial.print((float) BMS.Temps[n + i] / 64, 1);
      if ( i < 2) {
        Serial.print(F(", "));
      } else {
        Serial.println();
      }
    }
  }
  Serial.print(F("   mean : ")); Serial.print((float) BMS.Temps[11] / 64, 1);
  Serial.print(F(", min : ")); Serial.print((float) BMS.Temps[10] / 64, 1);
  Serial.print(F(", max : ")); Serial.println((float) BMS.Temps[9] / 64, 1);
  Serial.print(F("coolant : ")); Serial.println((float) BMS.Temps[12] / 64, 1);
}

//--------------------------------------------------------------------------------
//! \brief   Output individual cell data and statistics
//--------------------------------------------------------------------------------
void printIndividualCellData() {
  Serial.println(F("# ;mV  ;As/10"));
  for(int n = 0; n < CELLCOUNT; n++){
    if (n < 9) Serial.print(F("0"));
    Serial.print(n+1); Serial.print(F(";")); Serial.print(DiagCAN.getCellVoltage(n) - BMS.ADCvoltsOffset); Serial.print(F(";")); Serial.println(DiagCAN.getCellCapacity(n));
  }
  PrintSPACER();
  Serial.println(F("Individual Cell Statistics:"));
  PrintSPACER();
  Serial.print(F("CV mean : ")); Serial.print(BMS.Cvolts.mean - BMS.ADCvoltsOffset,0); Serial.print(F(" mV"));
  Serial.print(F(", dV= ")); Serial.print(BMS.Cvolts.max - BMS.Cvolts.min); Serial.print(F(" mV"));
  Serial.print(F(", s= ")); Serial.print(BMS.Cvolts_stdev); Serial.println(F(" mV"));
  Serial.print(F("CV min  : ")); Serial.print(BMS.Cvolts.min - BMS.ADCvoltsOffset); Serial.print(F(" mV, # ")); Serial.println(BMS.CV_min_at + 1);
  Serial.print(F("CV max  : ")); Serial.print(BMS.Cvolts.max - BMS.ADCvoltsOffset); Serial.print(F(" mV, # ")); Serial.println(BMS.CV_max_at + 1);
  PrintSPACER();
  Serial.print(F("CAP mean: ")); Serial.print(BMS.Ccap_As.mean, 0); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.mean / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("CAP min : ")); Serial.print(BMS.Ccap_As.min); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.min / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BMS.CAP_min_at + 1);
  Serial.print(F("CAP max : ")); Serial.print(BMS.Ccap_As.max); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.max / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BMS.CAP_max_at + 1);
}

//--------------------------------------------------------------------------------
//! \brief   Output NLG6 charger temperatures
//--------------------------------------------------------------------------------
void printNLG6temperatures() {
  Serial.println(F("Temperatures Charger-Unit /degC: "));
  Serial.print(F("Reported       : ")); Serial.println(NLG6.ReportedTemp - TEMP_OFFSET, DEC);
  Serial.print(F("Cooling plate  : ")); Serial.println(NLG6.CoolingPlateTemp - TEMP_OFFSET, DEC);
  Serial.print(F("Inlet socket   : ")); Serial.println(NLG6.SocketTemp - TEMP_OFFSET, DEC);
  Serial.println(F("Internal values: "));
          
  for (byte n = 0; n < 8; n++) {
      Serial.print(NLG6.Temps[n] - TEMP_OFFSET, DEC);
      if (n < 7) Serial.print(F(", "));
  }
  Serial.println();
}

//--------------------------------------------------------------------------------
//! \brief   Output NLG6 charger voltages and currents AC and DC
//--------------------------------------------------------------------------------
void printNLG6_Status() {
  Serial.println(F("Status NLG6 Charger-Unit: "));
  Serial.print(F("User selected : ")); Serial.print(NLG6.Amps_setpoint); Serial.println(F(" A"));
  Serial.print(F("Cable maximum : ")); Serial.print(NLG6.AmpsCableCode / 10); Serial.println(F(" A"));
  Serial.print(F("Chargingpoint : ")); Serial.print(NLG6.AmpsChargingpoint / 10); Serial.println(F(" A"));
  Serial.print(F("AC L1: ")); Serial.print(NLG6.MainsVoltage[0] / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(NLG6.MainsAmps[0] / 10.0, 1); Serial.println(F(" A"));
  Serial.print(F("AC L2: ")); Serial.print(NLG6.MainsVoltage[1] / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(NLG6.MainsAmps[1] / 10.0, 1); Serial.println(F(" A"));
  Serial.print(F("AC L3: ")); Serial.print(NLG6.MainsVoltage[2] / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(NLG6.MainsAmps[2] / 10.0, 1); Serial.println(F(" A"));
  Serial.print(F("DC HV: ")); Serial.print(NLG6.DC_HV / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(NLG6.DC_Current / 10.0, 1); Serial.println(F(" A"));
  Serial.print(F("DC LV: ")); Serial.print(NLG6.LV / 10.0, 1); Serial.println(F(" V"));
}

//--------------------------------------------------------------------------------
//! \brief   Output Cooling Subsystem temperatures
//--------------------------------------------------------------------------------
void printCLS_Status() {
  Serial.println(F("Status Cooling- and Subsystems: "));
  Serial.print(F("Temperature   : ")); Serial.print(CLS.CoolingTemp / 8.0,1); Serial.println(F(" degC"));
  Serial.print(F("Cooling Pump  : "));
  Serial.print(CLS.CoolingPumpRPM / 255.0 * 100.0, 1); Serial.print(F(" %, "));
  Serial.print(CLS.CoolingPumpTemp - 50); Serial.println(F(" degC"));
  Serial.print(F("              : "));
  Serial.print(CLS.CoolingPumpLV / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(CLS.CoolingPumpAmps / 5.0, 1); Serial.println(F(" A"));
  Serial.println(F("OTR:")); 
  Serial.print(F("Cooling Pump  : ")); Serial.print(CLS.CoolingPumpOTR); Serial.println(F(" h"));
  Serial.print(F("Battery heater: ")); Serial.print(CLS.BatteryHeaterOTR); Serial.print(F(" h, "));
  if (CLS.BatteryHeaterON == 0) {
    Serial.println(F("OFF"));
  } else {
    Serial.println(F("ON"));
  }
  Serial.print(F("Vaccum Pump   : ")); Serial.print(CLS.VaccumPumpOTR / 36000.0, 3); Serial.println(F(" h"));
  Serial.print(F("Pressure 1, 2 : ")); Serial.print(CLS.VaccumPumpPress1); Serial.print(F(" mbar, "));
  Serial.print(CLS.VaccumPumpPress2); Serial.println(F(" mbar"));
}

//--------------------------------------------------------------------------------
//! \brief   Output status data as splash screen
//--------------------------------------------------------------------------------
void printSplashScreen() {
  Serial.println(); PrintSPACER();
  printHeaderData();
  PrintSPACER();
  printStandardDataset();
  PrintSPACER();
  Serial.println(F("ENTER command... (? for help)"));
}

//--------------------------------------------------------------------------------
//! \brief   Output BMS dataset
//--------------------------------------------------------------------------------
void printBMSdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER();
  printHeaderData();
  PrintSPACER();
  printBatteryProductionData();
  PrintSPACER();
  printStandardDataset();
  PrintSPACER();
  printBMS_CellVoltages();
  PrintSPACER();
  printBMS_CapacityEstimate();
  PrintSPACER();
  printHVcontactorState();
  PrintSPACER();
  printBMStemperatures();
  PrintSPACER();
  if (VERBOSE) {
    printIndividualCellData();
    PrintSPACER();
  }
  if (EXPDATA) {
    printExperimentalData();
    PrintSPACER();
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output NLG6 dataset
//--------------------------------------------------------------------------------
void printNLG6data() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER();
  printNLG6_Status();
  PrintSPACER();
  printNLG6temperatures();
  PrintSPACER();
}


//--------------------------------------------------------------------------------
//! \brief   Output Cooling- and Subsystem dataset
//--------------------------------------------------------------------------------
void printCLSdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER();
  printCLS_Status();
  PrintSPACER();
}

//--------------------------------------------------------------------------------
//! \brief   Get all BMS data and output them
//! \brief   Dynamic memory allocation for CellVoltages and -Capacities
//! \brief   The allocated memory will be released after the data output
//--------------------------------------------------------------------------------
void printBMSall() {
  byte selected[12];                   //hold list for selected tasks
  
  //Read CAN-Bus IDs related to BMS (sniff traffic)
  for (byte i = 0; i < 8; i++) {
    selected[i] = i;
  }
  Serial.print(F("Reading data"));
  ReadCANtraffic_BMS(selected, 8);
  
  //Reserve memory
  DiagCAN.reserveMem_CellVoltage();
  DiagCAN.reserveMem_CellCapacity();
  //Serial.println(getFreeRam());
  
  //Get all diagnostics data of BMS
  for (byte i = 0; i < 12; i++) {
    selected[i] = i;
  }
  if (getBMSdata(selected, 12)) {
    printBMSdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
    
  //Free allocated memory
  DiagCAN.freeMem_CellVoltage();
  DiagCAN.freeMem_CellCapacity();
}

//--------------------------------------------------------------------------------
//! \brief   Get all NLG6 data and output them
//--------------------------------------------------------------------------------
void printNLG6all() {
  Serial.print(F("Reading data"));
  if (getNLG6data()) {
    printNLG6data();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
}

//--------------------------------------------------------------------------------
//! \brief   Get all Cooling- and Subsystem data and output them
//--------------------------------------------------------------------------------
void printCLSall() {
  Serial.print(F("Reading data"));
  if (getCLSdata()) {
    printCLSdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
}

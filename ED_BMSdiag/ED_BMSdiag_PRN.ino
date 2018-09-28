//--------------------------------------------------------------------------------
// (c) 2015-2017 by MyLab-odyssey
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
//! \date    2017-December
//! \author  MyLab-odyssey
//! \version 1.0.4
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
  Serial.println(); PrintSPACER();
  Serial.println(F("--- ED Battery Management Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(version);
  for (byte i = 0; i < (41 - 5 - vLength - 3); i++) {
    Serial.print(" ");
  }
  Serial.println(F("---"));
  PrintSPACER();

  int count = 0;
  do {
    if (count == 0) {
        Serial.println();
        Serial.println(F("Connect to OBD port - Waiting for CAN-Bus "));
    }
    count++;
    if (count >= 41) {
      count = 0;
    }
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
  
  Serial.print(F("Battery VIN: "));
  Serial.println(BMS.BattVIN);
  Serial.print(F("Time [hh:mm]: "));
  if (BMS.hour <= 9) Serial.print(F("0"));
  Serial.print(BMS.hour); Serial.print(F(":"));
  if (BMS.minutes <= 9) Serial.print(F("0"));
  Serial.print(BMS.minutes);
  Serial.print(F(",   ODO : ")); Serial.print(BMS.ODO); Serial.println(F(" km"));
}

//--------------------------------------------------------------------------------
//! \brief   Output battery production data and battery status SOH flag
//--------------------------------------------------------------------------------
void printBatteryProductionData(boolean fRPT) {
  Serial.print(F("Battery Status   : "));
  if (BMS.SOH == 0xFF) {
    Serial.println(MSG_OK);
  } else if (BMS.SOH == 0) {
    Serial.println(F("FAULT"));
  } else {
    Serial.print(F("DEGRADED: "));
    for(byte mask = 0x80; mask; mask >>= 1) {
      if(mask & BMS.SOH) {
        Serial.print(F("1"));
      } else {
        Serial.print(F("0"));
      }
    }
    Serial.println("");
  }
  Serial.println();
  Serial.print(F("Battery Production [Y/M/D]: ")); Serial.print(2000 + BMS.ProdYear); Serial.print(F("/"));
  Serial.print(BMS.ProdMonth); Serial.print(F("/")); Serial.println(BMS.ProdDay);
  if (fRPT) {
    Serial.print(F("Battery verified   [Y/M/D]: "));
    Serial.print(2000 + BMS.Year); Serial.print(F("/"));
    Serial.print(BMS.Month); Serial.print(F("/")); Serial.println(BMS.Day);
  } else {
    Serial.print(F("Battery-FAT date   [Y/M/D]: ")); 
    Serial.print(2000 + BMS.Year); Serial.print(F("/")); 
    Serial.print(BMS.Month); Serial.print(F("/")); Serial.println(BMS.Day);
    Serial.print(F("Rev.[Y/WK/PL] HW:")); Serial.print(2000 + BMS.hw.rev[0]); Serial.print(F("/"));
    Serial.print(BMS.hw.rev[1]); Serial.print(F("/")); Serial.print(BMS.hw.rev[2]);
    Serial.print(F(", SW:")); Serial.print(2000 + BMS.sw.rev[0]); Serial.print(F("/"));
    Serial.print(BMS.sw.rev[1]); Serial.print(F("/")); Serial.println(BMS.sw.rev[2]);
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output experimental data
//--------------------------------------------------------------------------------
void printExperimentalData() {
  Serial.println(F("*** Experimental Data - NOT VERIFIED ***"));
  Serial.print(F("Maximum capacity @45C: ")); Serial.print(BMS.CapInit / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("Aging-Loss: ")); Serial.print((float) BMS.CapLoss / 1000, 3); Serial.println(F(" %"));
  Serial.print(F("Unknown counter: 0x")); 
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
  Serial.print((float) BMS.Amps2, 2); Serial.print(F(" A, "));
  if (BMS.Power != 0) {
    Serial.print((float) BMS.Power, 2); Serial.println(F(" kW"));
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
  Serial.print(F("Cycles left   : ")); Serial.println(BMS.HVcontactCyclesLeft);
  Serial.print(F("of max. cycles: ")); Serial.println(BMS.HVcontactCyclesMax);
  Serial.print(F("DC isolation  : ")); Serial.print(BMS.Isolation); Serial.print(F(" kOhm, "));
  if (BMS.DCfault == 0) {
    Serial.println(MSG_OK);
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
  for(int16_t n = 0; n < CELLCOUNT; n++){
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
//! \brief   Visualize voltage distribution of cell data and statistics
//--------------------------------------------------------------------------------
void printVoltageDistribution() {
  uint16_t CVmin = BMS.Cvolts.min - BMS.ADCvoltsOffset;
  uint16_t CVmax = BMS.Cvolts.max - BMS.ADCvoltsOffset;
  uint16_t CVp25 = BMS.Cvolts.p25 - BMS.ADCvoltsOffset;
  uint16_t CVp50 = BMS.Cvolts.median - BMS.ADCvoltsOffset;
  uint16_t CVp75 = BMS.Cvolts.p75 - BMS.ADCvoltsOffset;
  uint16_t CVp3IQR = (BMS.Cvolts.p75 - BMS.Cvolts.p25) * IQR_FACTOR;
  
  byte bp_p25 = map(CVp25, CVmin, CVmax, 0, 40);
  byte bp_p50 = map(CVp50, CVmin, CVmax, 0, 40);
  byte bp_p75 = map(CVp75, CVmin, CVmax, 0, 40);
  byte bp_p3IQR_low = map(CVp25 - CVp3IQR, CVmin, CVmax, 0, 40);
  byte bp_p3IQR_high = map(CVp75 + CVp3IQR, CVmin, CVmax, 0, 40);
 
  Serial.print(F("Voltage Distribution (dV= ")); Serial.print(CVmax - CVmin); Serial.println(F(" mV):"));

  Serial.print(F("*"));
  for (byte n = 1; n < 40; n++) {
    if (n < bp_p25) {
      if (n == bp_p3IQR_low) {
        Serial.print(F(">"));
      } else {
        Serial.print(F("-"));
      }
    } else if (n == bp_p25) {
      Serial.print(F("["));
    } else if (n < bp_p50) {
      Serial.print(F("="));
    } else if (n == bp_p50) {
      Serial.print(F("|"));
    } else if (n > bp_p50 && n < bp_p75) {
      Serial.print(F("="));
    } else if (n == bp_p75) {
      Serial.print(F("]"));
    } else {
      if (n == bp_p3IQR_high) {
        Serial.print(F("<"));
      } else {
        Serial.print(F("-"));
      }
    }
  }
  Serial.println(F("*"));

  Serial.print(CVmin);
  Serial.print(F("   "));
  if (BMS.Cvolts.p25_out_count < 10) Serial.print('0');
  Serial.print(BMS.Cvolts.p25_out_count); Serial.print(F(" > "));
  Serial.print(F("["));
  Serial.print(CVp25); Serial.print(F("; "));
  Serial.print(CVp50); Serial.print(F("; "));
  Serial.print(CVp75); Serial.print(F("]"));
  Serial.print(F(" < "));
  if (BMS.Cvolts.p75_out_count < 10) Serial.print('0');
  Serial.print(BMS.Cvolts.p75_out_count); Serial.print(F(" "));
  Serial.print(F(" ")); Serial.print(CVmax);

  Serial.println();
  Serial.print(F("min"));
  for (byte n = 0; n <= 8; n++) Serial.print(F(" "));
  Serial.print(F("[p25; median; p75]"));
  for (byte n = 0; n <= 7; n++) Serial.print(F(" "));
  Serial.println(F("max"));
}

//--------------------------------------------------------------------------------
//! \brief   Output NLG6 charger voltages and currents AC and DC
//--------------------------------------------------------------------------------
void printNLG6_Status() {
  if (NLG6.NLG6present) {
    Serial.println(F("Status NLG6 Charger-Unit: "));
  } else {
    Serial.println(F("Status OBL Charger-Unit: "));
  }
  Serial.print(F("User selected : ")); Serial.print(NLG6.Amps_setpoint); Serial.println(F(" A"));
  Serial.print(F("Cable maximum : ")); Serial.print(NLG6.AmpsCableCode / 10); Serial.println(F(" A"));
  if (NLG6.NLG6present) {
    Serial.print(F("Chargingpoint : ")); Serial.print(NLG6.AmpsChargingpoint / 10); Serial.println(F(" A"));
  }
  Serial.print(F("AC L1: ")); Serial.print(NLG6.MainsVoltage[0] / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(NLG6.MainsAmps[0] / 10.0, 1); Serial.println(F(" A"));
  if (NLG6.NLG6present) {
    Serial.print(F("AC L2: ")); Serial.print(NLG6.MainsVoltage[1] / 10.0, 1); Serial.print(F(" V, "));
    Serial.print(NLG6.MainsAmps[1] / 10.0, 1); Serial.println(F(" A"));
    Serial.print(F("AC L3: ")); Serial.print(NLG6.MainsVoltage[2] / 10.0, 1); Serial.print(F(" V, "));
    Serial.print(NLG6.MainsAmps[2] / 10.0, 1); Serial.println(F(" A"));
  }
  Serial.print(F("DC HV: ")); Serial.print(NLG6.DC_HV / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(NLG6.DC_Current / 10.0, 1); Serial.println(F(" A"));
  Serial.print(F("DC LV: ")); Serial.print(NLG6.LV / 10.0, 1); Serial.println(F(" V"));
}

//--------------------------------------------------------------------------------
//! \brief   Output NLG6 charger temperatures
//--------------------------------------------------------------------------------
void printNLG6temperatures() {
  Serial.println(F("Temperatures Charger-Unit /degC: "));
  Serial.print(F("Reported       : ")); 
  if (NLG6.ReportedTemp < 0xFF) {
    Serial.println(NLG6.ReportedTemp - TEMP_OFFSET, DEC);
  } else {
    Serial.println("NA");
  }
  Serial.print(F("Cooling plate  : ")); 
  if (NLG6.CoolingPlateTemp < 0xFF) {
    Serial.println(NLG6.CoolingPlateTemp - TEMP_OFFSET, DEC);
  } else {
    Serial.println("NA");
  }
  Serial.print(F("Inlet socket   : ")); 
  if (NLG6.SocketTemp < 0xFF) {
    Serial.println(NLG6.SocketTemp - TEMP_OFFSET, DEC);
  } else {
    Serial.println("NA");
  }
  if (NLG6.NLG6present) {
    Serial.println(F("Internal values: "));       
    for (byte n = 0; n < 8; n++) {
        Serial.print(NLG6.Temps[n] - TEMP_OFFSET, DEC);
        if (n < 7) Serial.print(F(", "));
    }
    Serial.println();
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output NLG6 charger HW/SW revisions
//--------------------------------------------------------------------------------
void printNLG6revision() {
  if (NLG6.NLG6present) {
    Serial.print(F("HW PN : ")); Serial.println(NLG6.PN_HW);
    Serial.print(F("SW rev: "));
    DiagCAN.printNLG6ChargerSWrev(&NLG6, false); //get SW revisons and send to serial
  }
}

//--------------------------------------------------------------------------------
//! \brief   Output Cooling Subsystem temperatures
//--------------------------------------------------------------------------------
void printCLS_Status() {
  Serial.println(F("Status Cooling- and Subsystems: "));
  Serial.print(F("Temperature   : ")); Serial.print(CLS.CoolingTemp / 8.0,1); Serial.println(F(" degC"));
  Serial.print(F("Cooling fan   : "));
  Serial.print(CLS.CoolingFanRPM / 255.0 * 100.0, 1); Serial.println(F(" %"));
  Serial.print(F("Cooling pump  : "));
  Serial.print(CLS.CoolingPumpRPM / 255.0 * 100.0, 1); Serial.print(F(" %, "));
  Serial.print(CLS.CoolingPumpTemp - 50); Serial.println(F(" degC"));
  Serial.print(F("              : "));
  Serial.print(CLS.CoolingPumpLV / 10.0, 1); Serial.print(F(" V, "));
  Serial.print(CLS.CoolingPumpAmps / 5.0, 1); Serial.println(F(" A"));
  Serial.println(F("OTR:")); 
  Serial.print(F("Cooling fan   : ")); Serial.print(CLS.CoolingFanOTR); Serial.println(F(" h"));
  Serial.print(F("Cooling pump  : ")); Serial.print(CLS.CoolingPumpOTR); Serial.println(F(" h"));
  Serial.print(F("Battery heater: ")); Serial.print(CLS.BatteryHeaterOTR); Serial.print(F(" h, "));
  if (CLS.BatteryHeaterON == 0) {
    Serial.println(F("OFF"));
  } else {
    Serial.println(F("ON"));
  }
  Serial.print(F("Vaccum pump   : ")); Serial.print(CLS.VaccumPumpOTR / 36000.0, 3); Serial.println(F(" h"));
  Serial.print(F("Pressure 1, 2 : ")); Serial.print((int) CLS.VaccumPumpPress1); Serial.print(F(" mbar, "));
  Serial.print(CLS.VaccumPumpPress2); Serial.println(F(" mbar"));
}

//--------------------------------------------------------------------------------
//! \brief   Output status data as splash screen
//--------------------------------------------------------------------------------
void printSplashScreen() {
  Serial.println(); Serial.println(); PrintSPACER();
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
  printBatteryProductionData(false);
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
  if (BOXPLOT) {
    DiagCAN.getBatteryVoltageDist(&BMS);  //Sort cell voltages rising up and calc. quartiles
                                          //!!! after sorting track of individual cells is lost -> redo ".getBatteryVoltages" !!!
    printVoltageDistribution();           //Print statistic data as boxplot
    PrintSPACER();
  }
  if (myDevice.experimental) {
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
  if (NLG6.NLG6present) {
    printNLG6revision();
    PrintSPACER();
  }
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
//! \brief   Output BMS dataset
//--------------------------------------------------------------------------------
void printRPTdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  PrintSPACER();
  Serial.println(F("---       Battery Status Report       ---"));
  PrintSPACER();
  printHeaderData();
  Serial.println();
  printBatteryProductionData(true);
  Serial.println();
  Serial.print(F("realSOC          : ")); Serial.print((float) BMS.realSOC / 10.0, 1); Serial.print(F(" %, "));
  Serial.print(F("SOC: ")); Serial.print(BMS.SOC,1); Serial.println(F(" %"));
  Serial.print(F("Charged capacity : ")); Serial.print(BMS.Ccap_As.min / 360.0,1); Serial.print(F(" Ah, min. Cell# ")); Serial.println(BMS.CAP_min_at + 1);
  Serial.print(F("BMS estimate     : ")); Serial.print(BMS.Cap_As.min / 360.0,1); Serial.println(F(" Ah, value @25degC "));
  Serial.print(F("Measurement done : ")); Serial.print(BMS.LastMeas_days); Serial.println(F(" day(s) ago"));
  if (BMS.LastMeas_days > 21) Serial.println(F("* Watch timer for OCV state, redo test! *"));
  Serial.print(F("Estimation factor: ")); Serial.print(BMS.Cap_meas_quality,3);
  if (BMS.Cap_meas_quality >= 0.85) {
    Serial.println(F(", excellent"));
  } else if (BMS.Cap_meas_quality >= 0.82) {
    Serial.println(F(", very good"));
  } else if (BMS.Cap_meas_quality >= 0.80) {
    Serial.println(F(", good"));
  } else if (BMS.Cap_meas_quality >= 0.78) {
    Serial.println(F(", fair"));
    Serial.println(F("* need >>dSOC, start test below 20% SOC *"));
  } else {
    Serial.println(F(", poor!"));
    Serial.println(F("*** Don't trust and redo measurement! ***"));
  }
  Serial.println();
  Serial.print(F("OCV timer        : ")); Serial.print(BMS.OCVtimer); Serial.print(F(" s, "));
  Serial.print(F("@")); Serial.print((float) BMS.Temps[11] / 64, 1); Serial.println(F("degC"));
  Serial.print(F("OCV state        : "));
  if (BMS.HVcontactState == 0x02) {
    Serial.println(F("HV ON, can't meas. OCV"));
  } else if (BMS.HVcontactState == 0x00) {  
    if (BMS.HVoff_time < BMS.OCVtimer) {
      Serial.print(F("wait for "));
      uint16_t dTime = BMS.OCVtimer - BMS.HVoff_time;
      if (dTime > 3600) {
        if ((dTime / 3600) < 10) Serial.print(F("0"));
        Serial.print(dTime / 3600); Serial.print(F(":")); 
        if (((dTime % 3600) / 60) < 10) Serial.print(F("0"));
        Serial.print((dTime % 3600) / 60); Serial.println(F(" [hh:mm]"));
      } else if (dTime > 60) {
        if ((dTime / 60) < 10) Serial.print(F("0"));
        Serial.print(dTime / 60); Serial.print(F(":")); 
        if ((dTime % 60) < 10) Serial.print(F("0"));
        Serial.print(dTime % 60); Serial.println(F(" [mm:ss]"));
      } else {
        Serial.print(dTime); Serial.println(F(" s"));
      }
    } else {
      if (BMS.SOC <= 20) {
        Serial.println(F("Ready for cap. meas. !"));
      } else {
        Serial.println(MSG_OK);
      }
    }
  }
  Serial.println();
  Serial.print(F("DC isolation     : ")); Serial.print(BMS.Isolation); Serial.print(F(" kOhm, "));
  if (BMS.DCfault == 0) {
    Serial.println(MSG_OK);
  } else {
    Serial.println(F("DC FAULT"));
  }
  Serial.println();
  DiagCAN.getBatteryVoltageDist(&BMS);  //Sort cell voltages rising up and calc. quartiles
                                        //!!! after sorting track of individual cells is lost -> redo ".getBatteryVoltages" !!!
  printVoltageDistribution();           //Print statistic data as boxplot
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

//--------------------------------------------------------------------------------
//! \brief   Get all BMS data and output them as status report
//! \brief   Dynamic memory allocation for CellVoltages and -Capacities
//! \brief   The allocated memory will be released after the data output
//--------------------------------------------------------------------------------
void printRPT() {
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
  
  //Get all diagnostics data of BMS
  for (byte i = 0; i < 12; i++) {
    selected[i] = i;
  }
  if (getBMSdata(selected, 12)) {
    printRPTdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
    
  //Free allocated memory
  DiagCAN.freeMem_CellVoltage();
  DiagCAN.freeMem_CellCapacity();
}


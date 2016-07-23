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
//! \version 0.4.2
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//! \brief   Output header data as welcome screen - wait for CAN-Bus to be ready
//--------------------------------------------------------------------------------
void printWelcomeScreen() {
  byte vLength = strlen(version);
  Serial.println(SPACER);
  Serial.println(F("--- ED Battery Management Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(version);
  for (byte i = 0; i < (41 - 5 - vLength - 3); i++) {
    Serial.print(" ");
  }
  Serial.println(F("---"));
  Serial.println(SPACER);

  Serial.println(F("Connect to OBD port - Waiting for CAN-Bus "));
  do {
    Serial.print(F("."));
    delay(1000);
  } while (digitalRead(2));
  Serial.println(F("CONNECTED"));
  Serial.println(SPACER);
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
  Serial.println(SPACER);
  Serial.println(F("Individual Cell Statistics:"));
  Serial.println(SPACER);
  Serial.print(F("CV mean : ")); Serial.print(BMS.Cvolts.mean - BMS.ADCvoltsOffset,0); Serial.print(F(" mV"));
  Serial.print(F(", dV= ")); Serial.print(BMS.Cvolts.max - BMS.Cvolts.min); Serial.print(F(" mV"));
  Serial.print(F(", s= ")); Serial.print(BMS.Cvolts_stdev); Serial.println(F(" mV"));
  Serial.print(F("CV min  : ")); Serial.print(BMS.Cvolts.min - BMS.ADCvoltsOffset); Serial.print(F(" mV, # ")); Serial.println(BMS.CV_min_at + 1);
  Serial.print(F("CV max  : ")); Serial.print(BMS.Cvolts.max - BMS.ADCvoltsOffset); Serial.print(F(" mV, # ")); Serial.println(BMS.CV_max_at + 1);
  Serial.println(SPACER);
  Serial.print(F("CAP mean: ")); Serial.print(BMS.Ccap_As.mean, 0); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.mean / 360.0,1); Serial.println(F(" Ah"));
  Serial.print(F("CAP min : ")); Serial.print(BMS.Ccap_As.min); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.min / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BMS.CAP_min_at + 1);
  Serial.print(F("CAP max : ")); Serial.print(BMS.Ccap_As.max); Serial.print(F(" As/10, ")); Serial.print(BMS.Ccap_As.max / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BMS.CAP_max_at + 1);
}

//--------------------------------------------------------------------------------
//! \brief   Output status data as splash screen
//--------------------------------------------------------------------------------
void printSplashScreen() {
  Serial.println(); Serial.println(SPACER);
  printHeaderData();
  Serial.println(SPACER);
  printStandardDataset();
  Serial.println(SPACER);
  Serial.println(F("ENTER command... (? for help)"));
}

//--------------------------------------------------------------------------------
//! \brief   Output all BMS data
//--------------------------------------------------------------------------------
void printBMSdata() {
  Serial.println(MSG_OK);
  digitalWrite(CS, HIGH);
  Serial.println(SPACER);
  printHeaderData();
  Serial.println(SPACER);
  printBatteryProductionData();
  Serial.println(SPACER);
  printStandardDataset();
  Serial.println(SPACER);
  printBMS_CellVoltages();
  Serial.println(SPACER);
  printBMS_CapacityEstimate();
  Serial.println(SPACER);
  printHVcontactorState();
  Serial.println(SPACER);
  printBMStemperatures();
  Serial.println(SPACER);
  if (VERBOSE) {
    printIndividualCellData();
    Serial.println(SPACER);
  }
  if (EXPDATA) {
    printExperimentalData();
    Serial.println(SPACER);
  }
}

//--------------------------------------------------------------------------------
//! \brief   Get and output all BMS data
//--------------------------------------------------------------------------------
void printBMSall() {
  //Read CAN-Bus IDs related to BMS (sniff traffic)
  ReadCANtraffic_BMS();
  
  //Reserve memory
  DiagCAN.reserveMem_CellVoltage();
  DiagCAN.reserveMem_CellCapacity();

  //Get all diagnostics data of BMS
  if (getBMSdata()) {
    printBMSdata();
  } else {
    Serial.println();
    Serial.println(FAILURE);
  }
    
  //Free allocated memory
  DiagCAN.freeMem_CellVoltage();
  DiagCAN.freeMem_CellCapacity();
}


//--------------------------------------------------------------------------------
<<<<<<< HEAD
// ED BMSdiag, v0.4.0-b3
=======
// ED BMSdiag, v0.39b
>>>>>>> master
// Retrieve battery diagnostic data from your smart electric drive EV.
//
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
//! \file    ED_BMSdiag.ino
//! \brief   Retrieve battery diagnostic data from your smart electric drive EV.
//! \brief   Build a diagnostic tool with the MCP2515 CAN controller and Arduino
//! \brief   compatible hardware.
//! \date    2016-July
//! \author  My-Lab-odyssey
<<<<<<< HEAD
//! \version 0.4.0-b3
=======
//! \version 0.39b
>>>>>>> master
//--------------------------------------------------------------------------------

#define VERBOSE 1                //!< VERBOSE mode will output individual cell data
#define EXPDATA 1                //!< EXPDATA mode will output experimental / NOT VERIFIED data

#include <mcp_can.h>
//#include <SPI.h>
#include <Timeout.h>
#include <Average.h>
#include "canDiag.h"

//Global definitions
<<<<<<< HEAD
#define VERSION F("0.4.0-b3")
=======
#define VERSION F("0.39b")
#define DATALENGTH 440
#define CELLCOUNT 93
>>>>>>> master
#define SPACER F("-----------------------------------------")
#define MSG_OK F("OK")
#define MSG_FAIL F("F")
#define MSG_DOT F(".")

#define CS     10                //!< chip select pin of MCP2515 CAN-Controller
#define CS_SD  8                 //!< CS for SD card, if you plan to use a logger...
MCP_CAN CAN0(CS);                //!< Set CS pin

canDiag DiagCAN;

BatteryDiag_t BMS;
CTimeout CAN_Timeout(5000);     //!< Timeout value for CAN response in millis

//--------------------------------------------------------------------------------
//! \brief   SETUP()
//--------------------------------------------------------------------------------
void setup()
{  
  Serial.begin(115200);
  while (!Serial); // while the serial stream is not open, do nothing
  
  pinMode(CS, OUTPUT);
  pinMode(CS_SD, OUTPUT);
  digitalWrite(CS_SD, HIGH);
  
  // Initialize MCP2515 and clear filters
  DiagCAN.begin(&CAN0, &CAN_Timeout);
  DiagCAN.clearCAN_Filter();
    
  digitalWrite(CS, HIGH);
  
  // MCP2515 read buffer: setting pin 2 for input, LOW if CAN messages are received
  pinMode(2, INPUT); 
  
<<<<<<< HEAD
  printWelcomeScreen();
=======
  Serial.println(SPACER); 
  Serial.println(F("--- ED Battery Management Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(VERSION);
  Serial.println(F("                            ---"));
  Serial.println(SPACER);
  
  //Serial.print(F("SRAM: ")); Serial.println(freeRam());
  
  Serial.println(F("Connect to OBD port - Waiting for CAN-Bus "));
  do {
    Serial.print(F("."));
    delay(1000);
  } while (digitalRead(2));
  Serial.println(F("CONNECTED"));
  Serial.println(SPACER);
>>>>>>> master
}

//--------------------------------------------------------------------------------
//! \brief   Memory available between Heap and Stack
//--------------------------------------------------------------------------------
int getFreeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

//--------------------------------------------------------------------------------
//! \brief   Wait for serial data to be avaiable.
//--------------------------------------------------------------------------------
void WaitforSerial() {
  Serial.println(F("Press ENTER to start query:"));
  while (!Serial.available()) {}                  // Wait for serial input to start
}

//--------------------------------------------------------------------------------
//! \brief   Read all queued charachers to clear input buffer.
//--------------------------------------------------------------------------------
void clearSerialBuffer() {
  do {                                            // Clear serial input buffer
      delay(10);
  } while (Serial.read() >= 0);
}

//--------------------------------------------------------------------------------
//! \brief   Output header data
//--------------------------------------------------------------------------------
void printWelcomeScreen() {
  Serial.println(SPACER); 
  Serial.println(F("--- ED Battery Management Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(VERSION);
  Serial.println(F("                         ---"));
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
}

//--------------------------------------------------------------------------------
//! \brief   Output standard dataset
//--------------------------------------------------------------------------------
<<<<<<< HEAD
void printStandardDataset() {
  Serial.print(F("SOC : ")); Serial.print(BMS.SOC,1); Serial.print(F(" %"));
  Serial.print(F(", realSOC: ")); Serial.print((float) BMS.realSOC / 10.0, 1); Serial.println(F(" %"));
  Serial.print(F("HV  : ")); Serial.print(BMS.HV,1); Serial.print(F(" V, "));
  Serial.print((float) BMS.Amps/32.0, 2); Serial.print(F(" A, "));
  if (BMS.Power != 0) {
    Serial.print(((float) (BMS.Power / 8192.0) -1) * 300, 2); Serial.println(F(" kW"));
=======
boolean getBatteryCapacity(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattCapacity);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    CellCapacity.clear();
    ReadCellCapacity(data,25,CELLCOUNT);
    BMS.Ccap_As.min = CellCapacity.minimum(&BMS.CAP_min_at);
    BMS.Ccap_As.max = CellCapacity.maximum(&BMS.CAP_max_at);
    BMS.Ccap_As.mean = CellCapacity.mean();

    BMS.HVoff_time = (unsigned long) data[5] * 65535 + (unsigned int) data[6] * 256 + data[7];
    BMS.HV_lowcurrent = (unsigned long) data[9] * 65535 + (unsigned int) data[10] * 256 + data[11];
    BMS.OCVtimer = (unsigned int) data[12] * 256 + data[13];
    ReadDiagWord(&BMS.Cap_As.min,data,21,1);
    ReadDiagWord(&BMS.Cap_As.mean,data,23,1);
    ReadDiagWord(&BMS.Cap_As.max,data,17,1);
    ReadDiagWord(&BMS.LastMeas_days,data,427,1);
    //BMS.LastMeas_days = data[428];
    unsigned int value;
    ReadDiagWord(&value,data,429,1);
    BMS.Cap_meas_quality = value / 65535.0;
    ReadDiagWord(&value,data,425,1);
    BMS.Cap_combined_quality = value / 65535.0;
    return true;
>>>>>>> master
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
//! \brief   Read CAN-Bus traffic for BMS relevant data
//--------------------------------------------------------------------------------
void Read_CANtraffic_BMS() {
  boolean fOK = false;
  
  //Read CAN-messages 
  byte testStep = 0;
  do {
    switch (testStep) {
      case 0:
         Serial.print(F("Reading data"));
         fOK = DiagCAN.ReadSOC(&BMS);
         break;
      case 1:
         fOK = DiagCAN.ReadSOCinternal(&BMS);
         break;
      case 2:
         fOK = DiagCAN.ReadPower(&BMS);
         break;
      case 3:
         fOK = DiagCAN.ReadHV(&BMS);
         break;
      case 4:
         fOK = DiagCAN.ReadLV(&BMS);
         break;
      case 5:
         fOK = DiagCAN.ReadODO(&BMS);
         break;
      case 6:
         fOK = DiagCAN.ReadTime(&BMS);
         break;
    }
    if (testStep < 7) {
      if (fOK) {
        Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(testStep);
      }
    }
    testStep++;
  } while (testStep < 7);
}

//--------------------------------------------------------------------------------
//! \brief   LOOP()
//--------------------------------------------------------------------------------
void loop()
{         
   //Wait for start via serial terminal
   WaitforSerial();
   clearSerialBuffer();
   delay(500);
   
   //Read CAN-Bus IDs related to BMS (sniff traffic)
   Read_CANtraffic_BMS();
   
   //Get diagnostics data
   DiagCAN.setCAN_ID(0x7E7, 0x7EF);

   boolean fOK = false;
   byte testStep = 0;
   do {
      switch (testStep) {
        case 0:
           fOK = DiagCAN.getBatteryVoltage(&BMS, false);
           break;
        case 1:
           fOK = DiagCAN.getBatteryCapacity(&BMS, false);
           break;
        case 2:
           fOK = DiagCAN.getBatteryAmps(&BMS, false);
           break;
        case 3:
           fOK = DiagCAN.getBatteryCapInit(&BMS, false);
           break;
        case 4:
           fOK = DiagCAN.getBatteryCapLoss(&BMS, false);
           break;
        case 5:
           fOK = DiagCAN.getBatteryADCref(&BMS, false);
           break;
        case 6:
           fOK = DiagCAN.getBatteryDate(&BMS, false);
           break;
        case 7:
           fOK = DiagCAN.getBatteryRevision(&BMS, false);
           break;
        case 8:
           fOK = DiagCAN.getBatteryTemperature(&BMS, false);
           break;
        case 9:
           fOK = DiagCAN.getHVcontactorState(&BMS, false); 
           break;
        case 10:
           fOK = DiagCAN.getHVstatus(&BMS, false); 
           break;
        case 11:
           fOK = DiagCAN.getIsolationValue(&BMS, false); 
           break;
      }
      if (testStep < 12) {
        if (fOK) {
          Serial.print(MSG_DOT);
        } else {
          Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(testStep);
        }
      }
      testStep++;
   } while (fOK && testStep < 12);
    
   if (fOK) {
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
   } else {
      Serial.println();
      Serial.println(F("---------- Measurement failed !----------"));
      fOK = false;
   } 
   
}

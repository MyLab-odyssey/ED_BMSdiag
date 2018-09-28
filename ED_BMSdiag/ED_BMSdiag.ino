//--------------------------------------------------------------------------------
// ED BMSdiag, v1.0.5
// Retrieve battery diagnostic data from your smart electric drive EV.
//
// (c) 2015-2018 by MyLab-odyssey
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
//! \brief   Only usable for third gen. model build from late 2012 to mid 2015!!!
//! \brief   Build a diagnostic tool with the MCP2515 CAN controller and Arduino
//! \brief   compatible hardware.
//! \date    2018-February
//! \author  MyLab-odyssey
//! \version 1.0.5
//--------------------------------------------------------------------------------
#include "ED_BMSdiag.h"

#include <EEPROM.h>

//--------------------------------------------------------------------------------
//! \brief   SETUP()
//--------------------------------------------------------------------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    // while the serial stream is not open,  blink LED
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a 1/2 second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                       // wait for a 1/2 second
  }

  // Read configuration from EEPROM
  ReadGlobalConfig(&myDevice);

  pinMode(CS, OUTPUT);
  pinMode(CS_SD, OUTPUT);
  digitalWrite(CS_SD, HIGH);

  // Initialize MCP2515 and clear filters
  DiagCAN.begin(&CAN0, &CAN_Timeout);
  DiagCAN.clearCAN_Filter();

  digitalWrite(CS, HIGH);

  // MCP2515 read buffer: setting pin 2 for input, LOW if CAN messages are received
  pinMode(2, INPUT);

  //Serial.println(getFreeRam());

  //Print Welcome Screen and wait for CAN-Bus
  printWelcomeScreen();
  delay(1000);

  //Look if a NLG6 fastcharger is installed
  if (NLG6TEST) nlg6_installed();

  //test valid VIN stored in battery
  test_BattVIN();
  
  //Read CAN-Bus IDs related to BMS (sniff traffic)
  byte selected[] = {0,1,2,3,4,5,6,7};
  ReadCANtraffic_BMS(selected, sizeof(selected));

  //Setup CLI, display prompt and local echo
  setupMenu();
  init_cmd_prompt();

  // For convenience of users, dump all data first (same as "all" from top-level menu)
  // Do this after the setupMenu() call.
  if (myDevice.initialDump)
    get_all(0, (char**) 0L);

  //Print standard data set as overview
  printSplashScreen();
  
  // Prompt user for what to do next
  cmd_display();
  set_local_echo(ECHO);
  
}

//--------------------------------------------------------------------------------
//! \brief   LOOP()
//--------------------------------------------------------------------------------
void loop() {
   if (CLI_Timeout.Expired(true)) {
      cmdPoll();                                   //Poll CLI status
   }
   if (myDevice.logging && LOG_Timeout.Expired(true)){
      logdata();
   }
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
//! \brief   Read CAN-Bus traffic for BMS relevant data
//! \param   selected items for task (byte array), length of array
//--------------------------------------------------------------------------------
void ReadCANtraffic_BMS(byte *selected, byte len) {
  boolean fOK = false;

  //Read CAN-messages
  byte testStep = 0;
  do {
    switch (selected[testStep]) {
      case 0:
         fOK = DiagCAN.ReadSOC(&BMS);
         break;
      case 1:
         fOK = DiagCAN.ReadSOCinternal(&BMS);
         break;
      /*case 2:
         //fOK = DiagCAN.getBatteryAmps(&BMS, false);
         fOK = DiagCAN.ReadAmps(&BMS);
         break;
      case 3:
         fOK = DiagCAN.ReadHV(&BMS);
         break;*/
      case 4: 
         fOK = DiagCAN.ReadPower(&BMS);
         break;
      case 5:
         fOK = DiagCAN.ReadLV(&BMS);
         break;
      case 6:
         fOK = DiagCAN.ReadODO(&BMS);
         break;
      case 7:
         fOK = DiagCAN.ReadTime(&BMS);
         break;
    }
    if (!myDevice.logging && testStep < 8) {
      if (fOK) {
        Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(selected[testStep]);
      }
    }
    testStep++;
  } while (testStep < len);
}

//--------------------------------------------------------------------------------
//! \brief   Get BMS datasets
//! \param   selected items for task (byte array), length of array
//--------------------------------------------------------------------------------
boolean getBMSdata(byte *selected, byte len) {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(0x7E7, 0x7EF);

  byte testStep = 0;
  do {
    switch (selected[testStep]) {
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
         fOK = DiagCAN.getBatteryExperimentalData(&BMS, false);
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
    if (!myDevice.logging && testStep < 12) {
      if (fOK) {
        Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(selected[testStep]);
      }
    }
    testStep++;
  } while (fOK && testStep < len);
  
  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Get NLG6 datasets
//--------------------------------------------------------------------------------
boolean getNLG6data() {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(0x61A, 0x483);

  byte testStep = 0;
  do {
    switch (testStep) {
      case 0:
         fOK = DiagCAN.getChargerVoltages(&NLG6, false);
         break;
      case 1:
         fOK = DiagCAN.getChargerAmps(&NLG6, false);
         break;
      case 2:
         fOK = DiagCAN.getChargerSelCurrent(&NLG6, false);
         break;
      case 3:
         fOK = DiagCAN.getChargerTemperature(&NLG6, false);
         break;
    }
    if (!myDevice.logging && testStep < 4) {
      if (fOK) {
        Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(testStep);
      }
    }
    testStep++;
  } while (fOK && testStep < 4);
  
  return fOK;
}

//--------------------------------------------------------------------------------
//! \brief   Get cooling- & subsystem datasets
//--------------------------------------------------------------------------------
boolean getCLSdata() {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(0x7E5, 0x7ED);

  fOK = DiagCAN.getCoolingAndSubsystems(&CLS, false);   

  if (!myDevice.logging) {
    if (fOK) {
      Serial.print(MSG_DOT);
    } else {
      Serial.print(MSG_FAIL);Serial.print(F("#0"));
    }
  }

  return fOK;
}


void ReadGlobalConfig(deviceStatus_t *config, bool force_write)
{
  // If the EEPROM hasn't been programmed yet, program it
  if (force_write || EEPROM.read(EE_Signature) != kMagicSignature) {
    Serial.println("Setting factory defaults");
    EEPROM.update(EE_InitialDumpAll, 1); 
    EEPROM.update(EE_logging, 0);
    EEPROM.update(EE_logInterval, 30);
    EEPROM.update(EE_Experimental, 0);
    EEPROM.update(EE_Signature, kMagicSignature);
  }
  config->initialDump = (EEPROM.read(EE_InitialDumpAll) > 0);
  config->logging = (EEPROM.read(EE_logging) > 0);
  config->timer = EEPROM.read(EE_logInterval);
  config->experimental = (EEPROM.read(EE_Experimental) > 0);
}


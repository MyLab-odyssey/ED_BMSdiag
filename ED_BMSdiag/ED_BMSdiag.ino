//--------------------------------------------------------------------------------
// ED BMSdiag, v0.4.2
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
//! \version 0.4.2
//--------------------------------------------------------------------------------
#include "ED_BMSdiag.h"

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

  printWelcomeScreen();
  delay(500);
}

//--------------------------------------------------------------------------------
//! \brief   LOOP()
//--------------------------------------------------------------------------------
void loop()
{
   //Wait for start via serial terminal
   WaitforSerial();
   clearSerialBuffer();

   //Get BMS data and output all datasets
   printBMSall();
   
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
//--------------------------------------------------------------------------------
void ReadCANtraffic_BMS() {
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
         fOK = DiagCAN.getBatteryAmps(&BMS, false);
         break;
      case 3:
         fOK = DiagCAN.ReadPower(&BMS);
         break;
      case 4:
         fOK = DiagCAN.ReadHV(&BMS);
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
    if (testStep < 8) {
      if (fOK) {
        Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(testStep);
      }
    }
    testStep++;
  } while (testStep < 8);
}

//--------------------------------------------------------------------------------
//! \brief   Get BMS datasets
//--------------------------------------------------------------------------------
boolean getBMSdata() {
  boolean fOK = false;
  
  //Get diagnostics data
  DiagCAN.setCAN_ID(0x7E7, 0x7EF);

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
    if (testStep < 12) {
      if (fOK) {
        Serial.print(MSG_DOT);
      } else {
        Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.print(testStep);
      }
    }
    testStep++;
  } while (fOK && testStep < 12);
  
  return fOK;
}

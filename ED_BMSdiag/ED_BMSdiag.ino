//--------------------------------------------------------------------------------
// ED BMSdiag, v0.39
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
//! \date    2016-June
//! \author  My-Lab-odyssey
//! \version 0.39
//--------------------------------------------------------------------------------

//#define DO_DEBUG_UPDATE        //!< Uncomment to show DEBUG output
#define RAW_VOLTAGES 0           //!< Use RAW values or calc ADC offset voltage
#define VERBOSE 1                //!< VERBOSE mode will output individual cell data

#ifndef DO_DEBUG_UPDATE
#define DEBUG_UPDATE(...)
#else
#define DEBUG_UPDATE(...) Serial.print(__VA_ARGS__)
#endif

#include <mcp_can.h>
#include <SPI.h>
#include <Timeout.h>
#include <Average.h>

//Global definitions
#define VERSION F("0.39")
#define DATALENGTH 440
#define CELLCOUNT 93
#define SPACER F("-----------------------------------------")
#define MSG_OK F("OK")
#define MSG_FAIL F("FAIL")
#define MSG_DOT F(".")

//Data arrays
byte data[DATALENGTH]; 
Average <unsigned int> CellVoltage(CELLCOUNT);
Average <unsigned int> CellCapacity(CELLCOUNT);

//Data structure soft-/hardware-revision
typedef struct {
  byte rev[3];                   //!< year, week, patchlevel
} Revision_t;

//BMS data structure
typedef struct {
  unsigned int ADCCvolts_min;    //!< minimum cell voltage in mV, add offset +1500
  unsigned int ADCCvolts_mean;   //!< average cell voltage in mV, no offset
  unsigned int ADCCvolts_max;    //!< maxiimum cell voltage in mV, add offset +1500
  int ADCvoltsOffset;            //!< calculated offset between RAW cell voltages and ADCref, about 90mV
  unsigned int Cap_min_As;       //!< mininum cell capacity from measurement cycle
  unsigned int Cap_mean_As;      //!< average cell capacity from measurement cycle
  unsigned int Cap_max_As;       //!< maximum cell capacity from measurement cycle
  
  float Cvolts_mean;             //!< calculated average from individual cell voltage query  
  unsigned int Cvolts_min;       //!< voltage mininum in pack
  int CV_min_at;                 //!< cell number with voltage mininum in pack
  unsigned int Cvolts_max;       //!< voltage maximum in pack
  int CV_max_at;                 //!< cell number with voltage maximum in pack
  float Cvolts_stdev;            //!< calculated standard deviation (populated)
  unsigned int Ccap_mean_As;     //!< cell capacity calculated average
  unsigned int Ccap_min_As;      //!< cell capacity mininum in pack
  int CAP_min_at;                //!< cell number with capacity mininum in pack
  unsigned int Ccap_max_As;      //!< cell capacity maximum in pack
  int CAP_max_at;                //!< cell number with capacity maximum in pack
  
  int CapInit;                   //!< battery initial capacity (As/10)
  int CapLoss;                   //!< battery capacity loss (x/1000) in %  
  
  unsigned long HVoff_time;      //!< HighVoltage contactor off time in seconds
  unsigned long HV_lowcurrent;   //!< counter time of no current, reset e.g. with PLC heater or driving
  unsigned int OCVtimer;         //!< counter time in seconds to reach OCV state
  float Cap_meas_quality;        //!< some sort of estimation factor??? after measurement cycle
  float Cap_combined_quality;    //!< some sort of estimation factor??? constantly updated
  unsigned int LastMeas_days;    //!< days elapsed since last successful measurement
  
  byte Day;                      //!< day of battery production
  byte Month;                    //!< month of battery production
  byte Year;                     //!< year of battery production

  Revision_t sw;
  Revision_t hw;
  
  byte hour;                     //!< time in car: hour
  byte minutes;                  //!< time in car: minutes
  
  float SOC;                     //!< State of Charge, as reported by vehicle dash
  unsigned int realSOC;          //!< is this the internal SOC value in % (x/10)
    
  int Amps;                      //!< battery current in ampere (x/32)  
  int Power;                     //!< power in kW, drivetrain and charge ((x/8192)-1)*300
  
  float HV;                      //!< total voltage of HV system in V
  float LV;                      //!< 12V onboard voltage / LV system
  
  unsigned long ODO;             //!< Odometer count
  
  int Temps[13];                 //!< three temperatures per battery unit (1 to 3)
                                 //!< + max, min, mean and coolant-in temperatures
  unsigned int Isolation;        //!< Isolation in DC path, resistance in kOhm
  unsigned int DCfault;          //!< Flag to show DC-isolation fault
  
  byte HVcontactState;           //!< contactor state: 0 := OFF, 2 := ON
  long HVcontactCyclesLeft;      //!< counter related to ON/OFF cyles of the car
  long HVcontactCyclesMax;       //!< static, seems to be maxiumum of contactor cycles 
} BatteryDiag_t; 

//CAN-Bus declarations
long unsigned int rxID;
byte len = 0;
byte rxLength = 0;
byte rxBuf[8];
byte rqFC_length = 8;            //!< Interval to send flow control messages (rqFC) 
byte rqFlowControl[8]                             = {0x30, 0x08, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const PROGMEM byte rqBattHWrev[4]                 = {0x03, 0x22, 0xF1, 0x50};
const PROGMEM byte rqBattSWrev[4]                 = {0x03, 0x22, 0xF1, 0x51};
const PROGMEM byte rqBattTemperatures[4]          = {0x03, 0x22, 0x02, 0x01}; 
const PROGMEM byte rqBattModuleTemperatures[4]    = {0x03, 0x22, 0x02, 0x02};
const PROGMEM byte rqBattHVstatus[4]              = {0x03, 0x22, 0x02, 0x04};
const PROGMEM byte rqBattADCref[4]                = {0x03, 0x22, 0x02, 0x07};
const PROGMEM byte rqBattVolts[4]                 = {0x03, 0x22, 0x02, 0x08};
const PROGMEM byte rqBattIsolation[4]             = {0x03, 0x22, 0x02, 0x09};
const PROGMEM byte rqBattAmps[4]                  = {0x03, 0x22, 0x02, 0x03};
const PROGMEM byte rqBattDate[4]                  = {0x03, 0x22, 0x03, 0x04};
const PROGMEM byte rqBattCapInit[4]               = {0x03, 0x22, 0x03, 0x05};
const PROGMEM byte rqBattCapLoss[4]               = {0x03, 0x22, 0x03, 0x09};
const PROGMEM byte rqBattCapacity[4]              = {0x03, 0x22, 0x03, 0x10};
const PROGMEM byte rqBattHVContactorCyclesLeft[4] = {0x03, 0x22, 0x03, 0x0B};
const PROGMEM byte rqBattHVContactorMax[4]        = {0x03, 0x22, 0x03, 0x0C};
const PROGMEM byte rqBattHVContactorState[4]      = {0x03, 0x22, 0xD0, 0x00};

#define CS     10                //!< chip select pin of MCP2515 CAN-Controller
#define CS_SD  8                 //!< CS for SD card, if you plan to use a logger...
MCP_CAN CAN0(CS);                //!< Set CS pin

BatteryDiag_t BattDiag;
CTimeout CAN_Timeout(5000);     //!< Timeout value for CAN response in millis

//--------------------------------------------------------------------------------
//! \brief   SETUP()
//--------------------------------------------------------------------------------
void setup()
{  
  Serial.begin(115200);
  while (!Serial); // while the serial stream is not open, do nothing:
  
  pinMode(CS, OUTPUT);
  pinMode(CS_SD, OUTPUT);
  digitalWrite(CS_SD, HIGH);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters enabled.
  if(CAN0.begin(MCP_STD, CAN_500KBPS, MCP_16MHZ) == CAN_OK) DEBUG_UPDATE(F("MCP2515 Init Okay!!\r\n"));
  else DEBUG_UPDATE(F("MCP2515 Init Failed!!\r\n"));
  
  clearCAN_Filter();
    
  digitalWrite(CS, HIGH);
  
  // MCP2515 read buffer: setting pin 2 for input, LOW if CAN messages are received
  pinMode(2, INPUT); 
  
  Serial.println(SPACER); 
  Serial.println(F("--- ED Battery Management Diagnostics ---"));
  Serial.print(F("--- v")); Serial.print(VERSION);
  Serial.println(F("                             ---"));
  Serial.println(SPACER);
  
  //Serial.print(F("SRAM: ")); Serial.println(freeRam());
  
  Serial.println(F("Connect to OBD port - Waiting for CAN-Bus "));
  do {
    Serial.print(F("."));
    delay(1000);
  } while (digitalRead(2));
  Serial.println(F("CONNECTED"));
  Serial.println(SPACER);
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
//! \brief   Clear CAN ID filters.
//--------------------------------------------------------------------------------
void clearCAN_Filter(){
  CAN0.init_Mask(0, 0, 0x00000000);
  CAN0.init_Mask(1, 0, 0x00000000);
  delay(100);
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}

//--------------------------------------------------------------------------------
//! \brief   Set all filters to one CAN ID.
//--------------------------------------------------------------------------------
void setCAN_Filter(unsigned long filter){
  filter = filter << 16;
  CAN0.init_Mask(0, 0, 0x07FF0000);
  CAN0.init_Mask(1, 0, 0x07FF0000);
  CAN0.init_Filt(0, 0, filter);
  CAN0.init_Filt(1, 0, filter);
  CAN0.init_Filt(2, 0, filter);
  CAN0.init_Filt(3, 0, filter);
  CAN0.init_Filt(4, 0, filter);
  CAN0.init_Filt(5, 0, filter);
  delay(100);
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}

//--------------------------------------------------------------------------------
//! \brief   Send diagnostic request to ECU.
//! \param   byte* rqQuery
//! \see     rqBattADCref ... rqBattVolts
//! \return  received lines count (unsigned int) of function #Get_RequestResponse
//--------------------------------------------------------------------------------
unsigned int Request_Diagnostics(const byte* rqQuery){  
  byte rqMsg[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy_P(rqMsg, rqQuery, 4 * sizeof(byte)); // Fill byte 01 to 04 of rqMsg with rqQuery content (from PROGMEM)
  
  CAN_Timeout.Reset();                          // Reset Timeout-Timer
  
  digitalWrite(CS_SD, HIGH);                    // Disable SD card, or other SPI devices if nessesary
  
  //--- Diag Request Message ---
  DEBUG_UPDATE(F("Send Diag Request\n\r"));
  CAN0.sendMsgBuf(0x7E7, 0, 8, rqMsg);        // send data: Request diagnostics data
  
  return Get_RequestResponse();                 // wait for response of first frame
}

//--------------------------------------------------------------------------------
//! \brief   Wait and read initial diagnostic response
//! \return  lines count (unsigned int) of received lines á 7 bytes
//--------------------------------------------------------------------------------
unsigned int Get_RequestResponse(){ 
    
    byte i;
    unsigned int items = 0;
    byte FC_length = rqFlowControl[1];    
    boolean fDataOK = false;
    
    do{
      //--- Read Frames ---
      if(!digitalRead(2))                         // If pin 2 is LOW, read receive buffer
      {
        do{
          CAN0.readMsgBuf(&rxID, &len, rxBuf);    // Read data: len = data length, buf = data byte(s)       
          
          if (rxID == 0x7EF) { 
            if(rxBuf[0] < 0x10) {
              if((rxBuf[1] != 0x7F)) {  
                for (i = 0; i<len; i++) {         // read data bytes: offset +1, 1 to 7
                    data[i] = rxBuf[i+1];       
                }
                DEBUG_UPDATE(F("SF reponse: "));
                DEBUG_UPDATE(rxBuf[0] & 0x0F); DEBUG_UPDATE("\n\r");
                items = 0;
                fDataOK = true;
              } else if (rxBuf[3] == 0x78) {
                DEBUG_UPDATE(F("pending reponse...\n\r"));
              } else {
                DEBUG_UPDATE(F("ERROR\n\r"));
              }
            }
            if ((rxBuf[0] & 0xF0) == 0x10){
              items = (rxBuf[0] & 0x0F)*256 + rxBuf[1]; // six data bytes already read (+ two type and length)
              for (i = 0; i<len; i++) {                 // read data bytes: offset +1, 1 to 7
                  data[i] = rxBuf[i+1];       
              }
              //--- send rqFC: Request for more data ---
              CAN0.sendMsgBuf(0x7E7, 0, 8, rqFlowControl);
              DEBUG_UPDATE(F("Resp, i:"));
              DEBUG_UPDATE(items - 6); DEBUG_UPDATE("\n\r");
              fDataOK = Read_FC_Response(items - 6);
            } 
          }     
        } while(!digitalRead(2) && !CAN_Timeout.Expired(false) && !fDataOK);
      }
    } while (!CAN_Timeout.Expired(false) && !fDataOK);

    if (fDataOK) {
      return (items + 7) / 7;
      DEBUG_UPDATE(F("success!\n\r"));
    } else {
      DEBUG_UPDATE(F("Event Timeout!\n\r")); 
      ClearReadBuffer(); 
      return 0; 
    } 
}

//--------------------------------------------------------------------------------
//! \brief   Read remaining data and sent corresponding Flow Control frames
//! \param   items still to read (int)
//! \return  fDiagOK (boolean)
//--------------------------------------------------------------------------------
boolean Read_FC_Response(int items){   
    CAN_Timeout.Reset();
    
    byte i;
    int n = 7;
    int FC_count = 0;
    byte FC_length = rqFlowControl[1];
    boolean fDiagOK = false;
    
    do{
      //--- Read Frames ---
      if(!digitalRead(2))                         // If pin 2 is LOW, read receive buffer
      {
        do{
          CAN0.readMsgBuf(&rxID, &len, rxBuf);    // Read data: len = data length, buf = data byte(s)       
          if((rxBuf[0] & 0xF0) == 0x20){
            FC_count++;
            items = items - len + 1;
            for(i = 0; i<len; i++) {              // copy each byte of the rxBuffer to data-field
              if ((n < (DATALENGTH - 6)) && (i < 7)){
                data[n+i] = rxBuf[i+1];
              }       
            }
            //--- FC counter -> then send Flow Control Message ---
            if (FC_count % FC_length == 0 && items > 0) {
              // send rqFC: Request for more data
              CAN0.sendMsgBuf(0x7E7, 0, 8, rqFlowControl);
              DEBUG_UPDATE(F("FCrq\n\r"));
            }
            n = n + 7;
          }      
        } while(!digitalRead(2) && !CAN_Timeout.Expired(false) && items > 0);
      }
    } while (!CAN_Timeout.Expired(false) && items > 0);
    if (!CAN_Timeout.Expired(false)) {
      fDiagOK = true;
      DEBUG_UPDATE(F("Items left: ")); DEBUG_UPDATE(items); DEBUG_UPDATE("\n\r");
      DEBUG_UPDATE(F("FC count: ")); DEBUG_UPDATE(FC_count); DEBUG_UPDATE("\n\r");
    } else {
      fDiagOK = false;
      DEBUG_UPDATE(F("Event Timeout!\n\r"));
    } 
    ClearReadBuffer();    
    return fDiagOK;
}

//--------------------------------------------------------------------------------
//! \brief   Output read buffer
//! \param   lines count (unsigned int)
//--------------------------------------------------------------------------------
void PrintReadBuffer(unsigned int lines) {
  Serial.println(lines);
  for(int i = 0; i < lines; i++) {
      Serial.print(F("Data: "));
      for(byte n = 0; n < 7; n++)               // Print each byte of the data.
      {
        if(data[n + 7 * i] < 0x10)             // If data byte is less than 0x10, add a leading zero.
        {
          Serial.print(F("0"));
        }
        Serial.print(data[n + 7 * i], HEX);
        Serial.print(" ");
      }
      Serial.println();
  }
}

//--------------------------------------------------------------------------------
//! \brief   Cleanup after switching filters
//--------------------------------------------------------------------------------
void ClearReadBuffer(){
  if(!digitalRead(2)) {                        // still messages? pin 2 is LOW, clear the two rxBuffers by reading
    for (byte i = 1; i <= 2; i++) {
      CAN0.readMsgBuf(&rxID, &len, rxBuf);
    }
    DEBUG_UPDATE(F("Buffer cleared!\n\r"));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in temperature array
//--------------------------------------------------------------------------------
void ReadBatteryTemperatures(byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    BattDiag.Temps[n/2] = ((data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in CellCapacity obj
//--------------------------------------------------------------------------------
void ReadCellCapacity(byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    CellCapacity.push((data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in CellVoltage obj
//--------------------------------------------------------------------------------
void ReadCellVoltage(byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    CellVoltage.push((data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data
//! \param   address to output data array (unsigned int)
//! \param   address to input data array (unsigned int)
//! \param   start of first high byte in data array (unsigned int)
//! \param   length of data submitted (unsigned int)
//--------------------------------------------------------------------------------
void ReadDiagWord(unsigned int data_out[], byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    data_out[n/2] = data_in[n + highOffset] * 256 + data_in[n + highOffset + 1];
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery temperatures (values / 64 in deg C)
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryTemperature(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattTemperatures);
  boolean fOK = false;
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    ReadBatteryTemperatures(data,4,7);
    //Copy max, min, mean and coolant-in temp to end of array
    for(byte n = 0; n < 4; n++) {
      BattDiag.Temps[n + 9] = BattDiag.Temps[n];
    }
    fOK = true;
  }
  //Read three temperatures per module (á 31 cells)
  items = Request_Diagnostics(rqBattModuleTemperatures);
  if(items && fOK){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    ReadBatteryTemperatures(data,4,9);
    fOK = true;
  }
  if(fOK){
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery production date
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryDate(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattDate);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    BattDiag.Year = data[4];
    BattDiag.Month = data[5];
    BattDiag.Day = data[6];
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery soft-/hardware-revision
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryRevision(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattHWrev);
  byte n;
  boolean fOK = false;
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    for(n = 0; n < 3; n++) {
      BattDiag.hw.rev[n] =  data[n + 3];
    }
    fOK = true;
  }
  items = Request_Diagnostics(rqBattSWrev);
  if(items && fOK){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    byte offset = (data[0] - 3) + 1;
    for(n = 0; n < 3; n++) {
      BattDiag.sw.rev[n] =  data[n + offset];
    }
    fOK = true;
  }
  if(fOK){
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery high voltage status
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getHVstatus(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattHVstatus);
  unsigned int value;
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    if(BattDiag.HVcontactState != 0x02) {
      ReadDiagWord(&value,data,12,1);
      BattDiag.HV = (float) value/64.0;
    }
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery isolation resistance
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getIsolationValue(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattIsolation);
  unsigned int value;
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    } 
    ReadDiagWord(&value,data,4,1);
    BattDiag.Isolation = (signed) value;
    BattDiag.DCfault = data[6];
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate capacity data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryCapacity(boolean debug_verbose) {
  unsigned int items = Request_Diagnostics(rqBattCapacity);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    CellCapacity.clear();
    ReadCellCapacity(data,25,CELLCOUNT);
    BattDiag.Ccap_min_As = CellCapacity.minimum(&BattDiag.CAP_min_at);
    BattDiag.Ccap_max_As = CellCapacity.maximum(&BattDiag.CAP_max_at);
    BattDiag.Ccap_mean_As = CellCapacity.mean();

    BattDiag.HVoff_time = (unsigned long) data[5] * 65535 + data[6] * 256 + data[7];
    BattDiag.HV_lowcurrent = (unsigned long) data[9] * 65535 + data[10] * 256 + data[11];
    BattDiag.OCVtimer = (unsigned int) data[12] * 256 + data[13];
    ReadDiagWord(&BattDiag.Cap_min_As,data,21,1);
    ReadDiagWord(&BattDiag.Cap_mean_As,data,23,1);
    ReadDiagWord(&BattDiag.Cap_max_As,data,17,1);
    ReadDiagWord(&BattDiag.LastMeas_days,data,427,1);
    //BattDiag.LastMeas_days = data[428];
    unsigned int value;
    ReadDiagWord(&value,data,429,1);
    BattDiag.Cap_meas_quality = value / 65535.0;
    ReadDiagWord(&value,data,425,1);
    BattDiag.Cap_combined_quality = value / 65535.0;
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate initial battery capacity
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryCapInit(boolean debug_verbose) {
  unsigned int items;
  unsigned int value;
  items = Request_Diagnostics(rqBattCapInit);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    ReadDiagWord(&value,data,3,1);
    BattDiag.CapInit = (signed) value;
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery capacity loss
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryCapLoss(boolean debug_verbose) {
  unsigned int items;
  unsigned int value;
  items = Request_Diagnostics(rqBattCapLoss);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    ReadDiagWord(&value,data,3,1);
    BattDiag.CapLoss = (signed) value;
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate voltage data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryVoltage(boolean debug_verbose) {
  unsigned int items;
  items = Request_Diagnostics(rqBattVolts);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    CellVoltage.clear();
    ReadCellVoltage(data,4,CELLCOUNT);
    BattDiag.Cvolts_min = CellVoltage.minimum(&BattDiag.CV_min_at);
    BattDiag.Cvolts_max = CellVoltage.maximum(&BattDiag.CV_max_at);
    BattDiag.Cvolts_mean = CellVoltage.mean();
    BattDiag.Cvolts_stdev = CellVoltage.stddev();
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate current data / ampere
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryAmps(boolean debug_verbose) {
  unsigned int items;
  unsigned int value;
  items = Request_Diagnostics(rqBattAmps);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    ReadDiagWord(&value,data,3,1);
    BattDiag.Amps = (signed) value;
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate ADC reference data
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getBatteryADCref(boolean debug_verbose) {
  unsigned int items;
  items = Request_Diagnostics(rqBattADCref);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    ReadDiagWord(&BattDiag.ADCCvolts_mean,data,8,1);
    
    ReadDiagWord(&BattDiag.ADCCvolts_min,data,6,1);
    BattDiag.ADCCvolts_min += 1500;
    ReadDiagWord(&BattDiag.ADCCvolts_max,data,4,1);
    BattDiag.ADCCvolts_max += 1500;
    
    if (RAW_VOLTAGES) {
      BattDiag.ADCvoltsOffset = 0;
    } else {
      BattDiag.ADCvoltsOffset = (int) BattDiag.Cvolts_mean - BattDiag.ADCCvolts_mean;
    }
    
    return true;
  } else {
    return false;
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate High Voltage contractor state
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean getHVcontactorState(boolean debug_verbose) {
  unsigned int items;
  boolean fValid = false;
  items = Request_Diagnostics(rqBattHVContactorCyclesLeft);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    BattDiag.HVcontactCyclesLeft = (long) data[4] * 65536 + data[5] * 256 + data[6]; 
    fValid = true;
  } else {
    fValid = false;
  }
  items = Request_Diagnostics(rqBattHVContactorMax);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    BattDiag.HVcontactCyclesMax = (long) data[4] * 65536 + data[5] * 256 + data[6]; 
    fValid = true;
  } else {
    fValid = false;
  }
  items = Request_Diagnostics(rqBattHVContactorState);
  if(items){
    if (debug_verbose) {
      PrintReadBuffer(items);
    }   
    BattDiag.HVcontactState = (unsigned int) data[3]; 
    fValid = true;
  } else {
    fValid = false;
  }
  return fValid;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate SOC
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadSOC() {
  setCAN_Filter(0x518);
  CAN_Timeout.Reset();
     
  do {    
    if(!digitalRead(2)) { 
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x518) {
          BattDiag.SOC = (float) rxBuf[7] / 2;
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate internal SOC
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadSOCinternal() {
  setCAN_Filter(0x2D5);
  CAN_Timeout.Reset(); 
      
  do {    
    if(!digitalRead(2)) {
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x2D5) {
          BattDiag.realSOC =  (unsigned int) (rxBuf[4] & 0x03) * 256 + (unsigned int) rxBuf[5];
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate power reading
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadPower() {
  setCAN_Filter(0x508);
  CAN_Timeout.Reset();
      
  do {    
    if(!digitalRead(2)) {
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x508) {
          BattDiag.Power =  (unsigned int) (rxBuf[2] & 0x3F) * 256 + (unsigned int) rxBuf[3];
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate system High Voltage
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadHV() {
  setCAN_Filter(0x448);
  CAN_Timeout.Reset();
  
  float HV;   
  do {   
    if(!digitalRead(2)) {  
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x448) {
          HV = ((float)rxBuf[6]*256 + (float)rxBuf[7]);
          HV = HV / 10.0;
          BattDiag.HV = HV;
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate system Low Voltage (12V)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadLV() {
  setCAN_Filter(0x3D5);
  CAN_Timeout.Reset();
  
  float LV;
      
  do {    
    if(!digitalRead(2)) {
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x3D5) {
          LV = ((float)rxBuf[3]);
          LV = LV / 10.0;
          BattDiag.LV = LV;
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate odometer
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadODO() {
  setCAN_Filter(0x412);
  CAN_Timeout.Reset();
      
  do {    
    if(!digitalRead(2)) {
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x412) {
          BattDiag.ODO = (unsigned long) rxBuf[2] * 65535 + (unsigned int) rxBuf[3] * 256 + (unsigned int) rxBuf[4];
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate odometer
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean ReadTime() {
  setCAN_Filter(0x512);
  CAN_Timeout.Reset();
      
  do {    
    if(!digitalRead(2)) {
      CAN0.readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x512) {
          BattDiag.hour = rxBuf[0];
          BattDiag.minutes = rxBuf[1];
          return true;
        }
    }
  } while (!CAN_Timeout.Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   LOOP()
//--------------------------------------------------------------------------------
void loop()
{       
   boolean fOK = false;
   
   //Wait for start via serial terminal
   WaitforSerial();
   clearSerialBuffer();
   //Serial.println(SPACER);
   delay(500);
   
   //Read CAN-messages 
   byte testStep = 0;
   do {
      switch (testStep) {
        case 0:
           Serial.print(F("Reading data"));
           fOK = ReadSOC();
           break;
        case 1:
           fOK = ReadSOCinternal();
           break;
        case 2:
           fOK = ReadPower();
           break;
        case 3:
           fOK = ReadHV();
           break;
        case 4:
           fOK = ReadLV();
           break;
        case 5:
           fOK = ReadODO();
           break;
        case 6:
           fOK = ReadTime();
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
   
   
   //Get diagnostics data
   setCAN_Filter(0x7EF);
  
   testStep = 0;
   do {
      switch (testStep) {
        case 0:
           fOK = getBatteryVoltage(false);
           break;
        case 1:
           fOK = getBatteryCapacity(false);
           break;
        case 2:
           fOK = getBatteryAmps(false);
           break;
        case 3:
           fOK = getBatteryCapInit(false);
           break;
        case 4:
           fOK = getBatteryCapLoss(false);
           break;
        case 5:
           fOK = getBatteryADCref(false);
           break;
        case 6:
           fOK = getBatteryDate(false);
           break;
        case 7:
           fOK = getBatteryRevision(false);
           break;
        case 8:
           fOK = getBatteryTemperature(false);
           break;
        case 9:
           fOK = getHVcontactorState(false); 
           break;
        case 10:
           fOK = getHVstatus(false); 
           break;
        case 11:
           fOK = getIsolationValue(false); 
           break;
      }
      if (testStep < 12) {
        if (fOK) {
          Serial.print(MSG_DOT);
        } else {
          Serial.print(MSG_FAIL);Serial.print(F("#")); Serial.println(testStep);
        }
      }
      testStep++;
   } while (fOK && testStep < 12);
    
   if (fOK) {
      Serial.println(MSG_OK);
      digitalWrite(CS, HIGH);
      Serial.println(SPACER);
      Serial.print(F("Time [hh:mm]: ")); 
      if (BattDiag.hour <= 9) Serial.print(F("0"));
      Serial.print(BattDiag.hour); Serial.print(F(":"));
      if (BattDiag.minutes <= 9) Serial.print(F("0"));
      Serial.print(BattDiag.minutes);
      Serial.print(F(",   ODO : ")); Serial.print(BattDiag.ODO); Serial.println(F(" km"));
      Serial.println(SPACER);
      Serial.print(F("Battery-Production [Y/M/D]: ")); Serial.print(2000 + BattDiag.Year); Serial.print(F("/"));
      Serial.print(BattDiag.Month); Serial.print(F("/")); Serial.println(BattDiag.Day);
      Serial.print(F("Rev.[Y/WK/PL] HW:")); Serial.print(2000 + BattDiag.hw.rev[0]); Serial.print(F("/"));
      Serial.print(BattDiag.hw.rev[1]); Serial.print(F("/")); Serial.print(BattDiag.hw.rev[2]);
      Serial.print(F(", SW:")); Serial.print(2000 + BattDiag.sw.rev[0]); Serial.print(F("/"));
      Serial.print(BattDiag.sw.rev[1]); Serial.print(F("/")); Serial.println(BattDiag.sw.rev[2]);
      Serial.print(F("Initial Capacity : ")); Serial.print(BattDiag.CapInit / 360.0,1); Serial.print(F(" Ah"));
      Serial.print(F(", Loss: ")); Serial.print((float) BattDiag.CapLoss / 1000, 3); Serial.println(F(" %"));
      Serial.println(SPACER);
      Serial.print(F("SOC : ")); Serial.print(BattDiag.SOC,1); Serial.print(F(" %"));
      Serial.print(F(", realSOC: ")); Serial.print((float) BattDiag.realSOC / 10.0, 1); Serial.println(F(" %"));
      Serial.print(F("HV  : ")); Serial.print(BattDiag.HV,1); Serial.print(F(" V, "));
      Serial.print((float) BattDiag.Amps/32.0, 2); Serial.print(F(" A, "));
      if (BattDiag.Power != 0) {
        Serial.print(((float) (BattDiag.Power / 8192.0) -1) * 300, 2); Serial.println(F(" kW"));
      } else {
        Serial.println(F("0.00 kW"));
      }
      Serial.print(F("LV  : ")); Serial.print(BattDiag.LV,1); Serial.println(F(" V"));
        
      Serial.println(SPACER);
      Serial.print(F("CV mean : ")); Serial.print(BattDiag.ADCCvolts_mean); Serial.print(F(" mV"));
      Serial.print(F(", dV = ")); Serial.print(BattDiag.ADCCvolts_max - BattDiag.ADCCvolts_min); Serial.println(F(" mV"));
      Serial.print(F("CV min  : ")); Serial.print(BattDiag.ADCCvolts_min); Serial.println(F(" mV"));
      Serial.print(F("CV max  : ")); Serial.print(BattDiag.ADCCvolts_max); Serial.println(F(" mV"));
      Serial.print(F("OCVtimer: ")); Serial.print(BattDiag.OCVtimer); Serial.println(F(" s"));
      Serial.println(SPACER);
      Serial.print(F("Last measurement      : ")); Serial.print(BattDiag.LastMeas_days); Serial.println(F(" day(s)"));
      Serial.print(F("Measurement estimation: ")); Serial.println(BattDiag.Cap_meas_quality,3);
      Serial.print(F("Actual estimation     : ")); Serial.println(BattDiag.Cap_combined_quality,3);
      Serial.print(F("CAP mean: ")); Serial.print(BattDiag.Cap_mean_As); Serial.print(F(" As/10, ")); Serial.print(BattDiag.Cap_mean_As / 360.0,1); Serial.println(F(" Ah"));
      Serial.print(F("CAP min : ")); Serial.print(BattDiag.Cap_min_As); Serial.print(F(" As/10, ")); Serial.print(BattDiag.Cap_min_As / 360.0,1); Serial.println(F(" Ah"));
      Serial.print(F("CAP max : ")); Serial.print(BattDiag.Cap_max_As); Serial.print(F(" As/10, ")); Serial.print(BattDiag.Cap_max_As / 360.0,1); Serial.println(F(" Ah"));
      Serial.println(SPACER);
      Serial.print(F("HV contactor "));
      if (BattDiag.HVcontactState == 0x02) {
        Serial.print(F("state ON"));
        Serial.print(F(", low current: ")); Serial.print(BattDiag.HV_lowcurrent); Serial.println(F(" s"));
      } else if (BattDiag.HVcontactState == 0x00) {
        Serial.print(F("state OFF"));
        Serial.print(F(", for: ")); Serial.print(BattDiag.HVoff_time); Serial.println(F(" s"));
      }
      Serial.print(F("cycles left   : ")); Serial.println(BattDiag.HVcontactCyclesLeft);
      Serial.print(F("of max. cycles: ")); Serial.println(BattDiag.HVcontactCyclesMax);
      Serial.print(F("DC isolation  : ")); Serial.print(BattDiag.Isolation); Serial.print(F(" kOhm, "));
      if (BattDiag.DCfault == 0) {
        Serial.println(F("NO FAULT"));
      } else {
        Serial.println(F("DC FAULT !!!"));
      }
      Serial.println(SPACER);
      Serial.println(F("Temperatures Battery-Unit /degC: "));
      for (byte n = 0; n < 9; n = n + 3) {
        Serial.print(F("module ")); Serial.print((n / 3) + 1); Serial.print(F(": "));
        for (byte i = 0; i < 3; i++) {          
          Serial.print((float) BattDiag.Temps[n + i] / 64, 1);
          if ( i < 2) {
            Serial.print(F(", "));
          } else {
            Serial.println();
          }
        }
      }
      Serial.print(F("   mean : ")); Serial.print((float) BattDiag.Temps[11] / 64, 1);
      Serial.print(F(", min : ")); Serial.print((float) BattDiag.Temps[10] / 64, 1);
      Serial.print(F(", max : ")); Serial.println((float) BattDiag.Temps[9] / 64, 1);
      Serial.print(F("coolant : ")); Serial.println((float) BattDiag.Temps[12] / 64, 1);
      Serial.println(SPACER);
 
      if (VERBOSE) {
        Serial.println(F("# ;mV  ;As/10"));
        for(int n = 0; n < CELLCOUNT; n++){
          if (n < 9) Serial.print(F("0"));
          Serial.print(n+1); Serial.print(F(";")); Serial.print(CellVoltage.get(n) - BattDiag.ADCvoltsOffset); Serial.print(F(";")); Serial.println(CellCapacity.get(n));
        }
        Serial.println(SPACER);
        Serial.println(F("Individual Cell Statistics:"));
        Serial.println(SPACER);
        Serial.print(F("CV mean : ")); Serial.print(BattDiag.Cvolts_mean - BattDiag.ADCvoltsOffset,0); Serial.print(F(" mV"));
        Serial.print(F(", dV = ")); Serial.print(BattDiag.Cvolts_max - BattDiag.Cvolts_min); Serial.print(F(" mV"));
        Serial.print(F(", s = ")); Serial.print(BattDiag.Cvolts_stdev); Serial.println(F(" mV"));
        Serial.print(F("CV min  : ")); Serial.print(BattDiag.Cvolts_min - BattDiag.ADCvoltsOffset); Serial.print(F(" mV, # ")); Serial.println(BattDiag.CV_min_at + 1);
        Serial.print(F("CV max  : ")); Serial.print(BattDiag.Cvolts_max - BattDiag.ADCvoltsOffset); Serial.print(F(" mV, # ")); Serial.println(BattDiag.CV_max_at + 1);
        Serial.println(SPACER);
        Serial.print(F("CAP mean: ")); Serial.print(BattDiag.Ccap_mean_As); Serial.print(F(" As/10, ")); Serial.print(BattDiag.Ccap_mean_As / 360.0,1); Serial.println(F(" Ah"));
        Serial.print(F("CAP min : ")); Serial.print(BattDiag.Ccap_min_As); Serial.print(F(" As/10, ")); Serial.print(BattDiag.Ccap_min_As / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BattDiag.CAP_min_at + 1);
        Serial.print(F("CAP max : ")); Serial.print(BattDiag.Ccap_max_As); Serial.print(F(" As/10, ")); Serial.print(BattDiag.Ccap_max_As / 360.0,1); Serial.print(F(" Ah, # ")); Serial.println(BattDiag.CAP_max_at + 1);
        Serial.println(SPACER);
      }   
   } else {
      Serial.println(F("---------- Measurement failed !----------"));
      fOK = false;
   } 
   
}

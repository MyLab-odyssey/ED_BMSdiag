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
//! \file    canDiag.cpp
//! \brief   Library module for retrieving diagnostic data.
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.1.0
//--------------------------------------------------------------------------------
#include "canDiag.h"

//--------------------------------------------------------------------------------
//! \brief   Standard constructor / destructor
//--------------------------------------------------------------------------------
canDiag::canDiag() {
  //CellVoltages = new Average<unsigned int>(93);
  //CellCapacities = new Average<unsigned int>(93);
}

canDiag::~canDiag() {  
  //delete CellVoltages;
  //delete CellCapacities;
}


//--------------------------------------------------------------------------------
//! \brief   Get method for CellVoltages
//--------------------------------------------------------------------------------
unsigned int canDiag::getCellVoltage(byte n) {
  return CellVoltage.get(n);
}

//--------------------------------------------------------------------------------
//! \brief   Get method for CellCapacities
//--------------------------------------------------------------------------------
unsigned int canDiag::getCellCapacity(byte n) {
  return CellCapacity.get(n);
}

//--------------------------------------------------------------------------------
//! \brief   Initialize CAN-Object and MCP2515 Controller
//--------------------------------------------------------------------------------
void canDiag::begin(MCP_CAN *_myCAN, CTimeout *_myCAN_Timeout) {
  //Set Pointer to MCP_CANobj
  myCAN0 = _myCAN;
  myCAN_Timeout = _myCAN_Timeout;
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters enabled.
  if(myCAN0->begin(MCP_STD, CAN_500KBPS, MCP_16MHZ) == CAN_OK) DEBUG_UPDATE(F("MCP2515 Init Okay!!\r\n"));
  else DEBUG_UPDATE(F("MCP2515 Init Failed!!\r\n"));
}

//--------------------------------------------------------------------------------
//! \brief   Clear CAN ID filters.
//--------------------------------------------------------------------------------
void canDiag::clearCAN_Filter(){
  myCAN0->init_Mask(0, 0, 0x00000000);
  myCAN0->init_Mask(1, 0, 0x00000000);
  delay(100);
  myCAN0->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}

//--------------------------------------------------------------------------------
//! \brief   Set all filters to one CAN ID.
//--------------------------------------------------------------------------------
void canDiag::setCAN_Filter(unsigned long filter){
  filter = filter << 16;
  myCAN0->init_Mask(0, 0, 0x07FF0000);
  myCAN0->init_Mask(1, 0, 0x07FF0000);
  myCAN0->init_Filt(0, 0, filter);
  myCAN0->init_Filt(1, 0, filter);
  myCAN0->init_Filt(2, 0, filter);
  myCAN0->init_Filt(3, 0, filter);
  myCAN0->init_Filt(4, 0, filter);
  myCAN0->init_Filt(5, 0, filter);
  delay(100);
  myCAN0->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}


//--------------------------------------------------------------------------------
//! \brief   Set request CAN ID and response CAN ID for get functions
//--------------------------------------------------------------------------------
void canDiag::setCAN_ID(unsigned long _respID) {
  respID = _respID;
}
void canDiag::setCAN_ID(unsigned long _rqID, unsigned long _respID) {
  rqID = _rqID;
  respID = _respID;
  this->setCAN_Filter(respID);
}

//--------------------------------------------------------------------------------
//! \brief   Send diagnostic request to ECU.
//! \param   byte* rqQuery
//! \see     rqBattADCref ... rqBattVolts
//! \return  received lines count (unsigned int) of function #Get_RequestResponse
//--------------------------------------------------------------------------------
unsigned int canDiag::Request_Diagnostics(const byte* rqQuery){  
  byte rqMsg[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy_P(rqMsg, rqQuery, 4 * sizeof(byte)); // Fill byte 01 to 04 of rqMsg with rqQuery content (from PROGMEM)
  
  myCAN_Timeout->Reset();                     // Reset Timeout-Timer
  
  //digitalWrite(CS_SD, HIGH);                // Disable SD card, or other SPI devices if nessesary
  
  //--- Diag Request Message ---
  DEBUG_UPDATE(F("Send Diag Request\n\r"));
  myCAN0->sendMsgBuf(rqID, 0, 8, rqMsg);      // send data: Request diagnostics data
  
  return this->Get_RequestResponse();         // wait for response of first frame
}

//--------------------------------------------------------------------------------
//! \brief   Wait and read initial diagnostic response
//! \return  lines count (unsigned int) of received lines á 7 bytes
//--------------------------------------------------------------------------------
unsigned int canDiag::Get_RequestResponse(){ 
    
    byte i;
    unsigned int items = 0;
    byte FC_length = rqFlowControl[1];    
    boolean fDataOK = false;
    
    do{
      //--- Read Frames ---
      if(!digitalRead(2))                         // If pin 2 is LOW, read receive buffer
      {
        do{
          myCAN0->readMsgBuf(&rxID, &len, rxBuf);    // Read data: len = data length, buf = data byte(s)       
          
          if (rxID == respID) { 
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
              myCAN0->sendMsgBuf(rqID, 0, 8, rqFlowControl);
              DEBUG_UPDATE(F("Resp, i:"));
              DEBUG_UPDATE(items - 6); DEBUG_UPDATE("\n\r");
              fDataOK = Read_FC_Response(items - 6);
            } 
          }     
        } while(!digitalRead(2) && !myCAN_Timeout->Expired(false) && !fDataOK);
      }
    } while (!myCAN_Timeout->Expired(false) && !fDataOK);

    if (fDataOK) {
      return (items + 7) / 7;
      DEBUG_UPDATE(F("success!\n\r"));
    } else {
      DEBUG_UPDATE(F("Event Timeout!\n\r")); 
      this->ClearReadBuffer(); 
      return 0; 
    } 
}

//--------------------------------------------------------------------------------
//! \brief   Read remaining data and sent corresponding Flow Control frames
//! \param   items still to read (int)
//! \return  fDiagOK (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::Read_FC_Response(int items){   
    myCAN_Timeout->Reset();
    
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
          myCAN0->readMsgBuf(&rxID, &len, rxBuf);    // Read data: len = data length, buf = data byte(s)       
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
              myCAN0->sendMsgBuf(rqID, 0, 8, rqFlowControl);
              DEBUG_UPDATE(F("FCrq\n\r"));
            }
            n = n + 7;
          }      
        } while(!digitalRead(2) && !myCAN_Timeout->Expired(false) && items > 0);
      }
    } while (!myCAN_Timeout->Expired(false) && items > 0);
    if (!myCAN_Timeout->Expired(false)) {
      fDiagOK = true;
      DEBUG_UPDATE(F("Items left: ")); DEBUG_UPDATE(items); DEBUG_UPDATE("\n\r");
      DEBUG_UPDATE(F("FC count: ")); DEBUG_UPDATE(FC_count); DEBUG_UPDATE("\n\r");
    } else {
      fDiagOK = false;
      DEBUG_UPDATE(F("Event Timeout!\n\r"));
    } 
    this->ClearReadBuffer();    
    return fDiagOK;
}

//--------------------------------------------------------------------------------
//! \brief   Output read buffer
//! \param   lines count (unsigned int)
//--------------------------------------------------------------------------------
void canDiag::PrintReadBuffer(unsigned int lines) {
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
void canDiag::ClearReadBuffer(){
  if(!digitalRead(2)) {                        // still messages? pin 2 is LOW, clear the two rxBuffers by reading
    for (byte i = 1; i <= 2; i++) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf);
    }
    DEBUG_UPDATE(F("Buffer cleared!\n\r"));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in temperature array
//--------------------------------------------------------------------------------
void canDiag::ReadBatteryTemperatures(BatteryDiag_t *myBMS, byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    myBMS->Temps[n/2] = ((data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in CellCapacity obj
//--------------------------------------------------------------------------------
void canDiag::ReadCellCapacity(byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    CellCapacity.push((data_in[n + highOffset] * 256 + data_in[n + highOffset + 1]));
  }
}

//--------------------------------------------------------------------------------
//! \brief   Store two byte data in CellVoltage obj
//--------------------------------------------------------------------------------
void canDiag::ReadCellVoltage(byte data_in[], unsigned int highOffset, unsigned int length){
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
void canDiag::ReadDiagWord(unsigned int data_out[], byte data_in[], unsigned int highOffset, unsigned int length){
  for(int n = 0; n < (length * 2); n = n + 2){
    data_out[n/2] = data_in[n + highOffset] * 256 + data_in[n + highOffset + 1];
  }
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate battery temperatures (values / 64 in deg C)
//! \param   enable verbose / debug output (boolean)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::getBatteryTemperature(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items = this->Request_Diagnostics(rqBattTemperatures);
  boolean fOK = false;
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    this->ReadBatteryTemperatures(myBMS,data,4,7);
    //Copy max, min, mean and coolant-in temp to end of array
    for(byte n = 0; n < 4; n++) {
      myBMS->Temps[n + 9] = myBMS->Temps[n];
    }
    fOK = true;
  }
  //Read three temperatures per module (á 31 cells)
  items = this->Request_Diagnostics(rqBattModuleTemperatures);
  if(items && fOK){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    this->ReadBatteryTemperatures(myBMS,data,4,9);
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
boolean canDiag::getBatteryDate(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items = this->Request_Diagnostics(rqBattDate);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    myBMS->Year = data[4];
    myBMS->Month = data[5];
    myBMS->Day = data[6];
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
boolean canDiag::getBatteryRevision(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items = this->Request_Diagnostics(rqBattHWrev);
  byte n;
  boolean fOK = false;
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    for(n = 0; n < 3; n++) {
      myBMS->hw.rev[n] =  data[n + 3];
    }
    fOK = true;
  }
  items = this->Request_Diagnostics(rqBattSWrev);
  if(items && fOK){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    byte offset = (data[0] - 3) + 1;
    for(n = 0; n < 3; n++) {
      myBMS->sw.rev[n] =  data[n + offset];
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
boolean canDiag::getHVstatus(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items = this->Request_Diagnostics(rqBattHVstatus);
  unsigned int value;
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    if(myBMS->HVcontactState != 0x02) {
      this->ReadDiagWord(&value,data,12,1);
      myBMS->HV = (float) value/64.0;
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
boolean canDiag::getIsolationValue(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items = this->Request_Diagnostics(rqBattIsolation);
  unsigned int value;
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    } 
    this->ReadDiagWord(&value,data,4,1);
    myBMS->Isolation = (signed) value;
    myBMS->DCfault = data[6];
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
boolean canDiag::getBatteryCapacity(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items = this->Request_Diagnostics(rqBattCapacity);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    CellCapacity.clear();
    this->ReadCellCapacity(data,25,CELLCOUNT);
    myBMS->Ccap_As.min = CellCapacity.minimum(&myBMS->CAP_min_at);
    myBMS->Ccap_As.max = CellCapacity.maximum(&myBMS->CAP_max_at);
    myBMS->Ccap_As.mean = CellCapacity.mean();

    myBMS->HVoff_time = (unsigned long) data[5] * 65535 + (unsigned int) data[6] * 256 + data[7];
    myBMS->HV_lowcurrent = (unsigned long) data[9] * 65535 + (unsigned int) data[10] * 256 + data[11];
    myBMS->OCVtimer = (unsigned int) data[12] * 256 + data[13];
    this->ReadDiagWord(&myBMS->Cap_As.min,data,21,1);
    this->ReadDiagWord(&myBMS->Cap_As.mean,data,23,1);
    this->ReadDiagWord(&myBMS->Cap_As.max,data,17,1);
    this->ReadDiagWord(&myBMS->LastMeas_days,data,427,1);
    unsigned int value;
    this->ReadDiagWord(&value,data,429,1);
    myBMS->Cap_meas_quality = value / 65535.0;
    this->ReadDiagWord(&value,data,425,1);
    myBMS->Cap_combined_quality = value / 65535.0;
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
boolean canDiag::getBatteryCapInit(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items;
  unsigned int value;
  items = this->Request_Diagnostics(rqBattCapInit);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    this->ReadDiagWord(&value,data,3,1);
    myBMS->CapInit = (signed) value;
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
boolean canDiag::getBatteryCapLoss(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items;
  unsigned int value;
  items = this->Request_Diagnostics(rqBattCapLoss);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    this->ReadDiagWord(&value,data,3,1);
    myBMS->CapLoss = (signed) value;
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
boolean canDiag::getBatteryVoltage(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items;
  items = this->Request_Diagnostics(rqBattVolts);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    CellVoltage.clear();
    this->ReadCellVoltage(data,4,CELLCOUNT);
    myBMS->Cvolts.min = CellVoltage.minimum(&myBMS->CV_min_at);
    myBMS->Cvolts.max = CellVoltage.maximum(&myBMS->CV_max_at);
    myBMS->Cvolts.mean = CellVoltage.mean();
    myBMS->Cvolts_stdev = CellVoltage.stddev();
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
boolean canDiag::getBatteryAmps(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items;
  unsigned int value;
  items = this->Request_Diagnostics(rqBattAmps);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    this->ReadDiagWord(&value,data,3,1);
    myBMS->Amps = (signed) value;
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
boolean canDiag::getBatteryADCref(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items;
  items = this->Request_Diagnostics(rqBattADCref);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    this->ReadDiagWord(&myBMS->ADCCvolts.mean,data,8,1);
    
    this->ReadDiagWord(&myBMS->ADCCvolts.min,data,6,1);
    myBMS->ADCCvolts.min += 1500;
    this->ReadDiagWord(&myBMS->ADCCvolts.max,data,4,1);
    myBMS->ADCCvolts.max += 1500;
    
    if (RAW_VOLTAGES) {
      myBMS->ADCvoltsOffset = 0;
    } else {
      myBMS->ADCvoltsOffset = (int) myBMS->Cvolts.mean - myBMS->ADCCvolts.mean;
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
boolean canDiag::getHVcontactorState(BatteryDiag_t *myBMS, boolean debug_verbose) {
  unsigned int items;
  boolean fValid = false;
  items = this->Request_Diagnostics(rqBattHVContactorCyclesLeft);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    myBMS->HVcontactCyclesLeft = (unsigned long) data[4] * 65536 + (unsigned int) data[5] * 256 + data[6]; 
    fValid = true;
  } else {
    fValid = false;
  }
  items = this->Request_Diagnostics(rqBattHVContactorMax);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    myBMS->HVcontactCyclesMax = (unsigned long) data[4] * 65536 + (unsigned int) data[5] * 256 + data[6]; 
    fValid = true;
  } else {
    fValid = false;
  }
  items = this->Request_Diagnostics(rqBattHVContactorState);
  if(items){
    if (debug_verbose) {
      this->PrintReadBuffer(items);
    }   
    myBMS->HVcontactState = (unsigned int) data[3]; 
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
boolean canDiag::ReadSOC(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x518);
  myCAN_Timeout->Reset();
     
  do {    
    if(!digitalRead(2)) { 
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x518) {
          myBMS->SOC = (float) rxBuf[7] / 2;
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate internal SOC
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::ReadSOCinternal(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x2D5);
  myCAN_Timeout->Reset(); 
      
  do {    
    if(!digitalRead(2)) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x2D5) {
          myBMS->realSOC =  (unsigned int) (rxBuf[4] & 0x03) * 256 + (unsigned int) rxBuf[5];
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate power reading
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::ReadPower(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x508);
  myCAN_Timeout->Reset();
      
  do {    
    if(!digitalRead(2)) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x508) {
          myBMS->Power =  (unsigned int) (rxBuf[2] & 0x3F) * 256 + (unsigned int) rxBuf[3];
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate system High Voltage
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::ReadHV(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x448);
  myCAN_Timeout->Reset();
  
  float HV;   
  do {   
    if(!digitalRead(2)) {  
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x448) {
          HV = ((float)rxBuf[6]*256 + (float)rxBuf[7]);
          HV = HV / 10.0;
          myBMS->HV = HV;
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate system Low Voltage (12V)
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::ReadLV(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x3D5);
  myCAN_Timeout->Reset();
  
  float LV;
      
  do {    
    if(!digitalRead(2)) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x3D5) {
          LV = ((float)rxBuf[3]);
          LV = LV / 10.0;
          myBMS->LV = LV;
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate odometer
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::ReadODO(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x412);
  myCAN_Timeout->Reset();
      
  do {    
    if(!digitalRead(2)) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x412) {
          myBMS->ODO = (unsigned long) rxBuf[2] * 65535 + (unsigned int) rxBuf[3] * 256 + (unsigned int) rxBuf[4];
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  return false;
}

//--------------------------------------------------------------------------------
//! \brief   Read and evaluate odometer
//! \return  report success (boolean)
//--------------------------------------------------------------------------------
boolean canDiag::ReadTime(BatteryDiag_t *myBMS) {
  this->setCAN_Filter(0x512);
  myCAN_Timeout->Reset();
      
  do {    
    if(!digitalRead(2)) {
      myCAN0->readMsgBuf(&rxID, &len, rxBuf); 
        if (rxID == 0x512) {
          myBMS->hour = rxBuf[0];
          myBMS->minutes = rxBuf[1];
          return true;
        }
    }
  } while (!myCAN_Timeout->Expired(false));
  return false;
}

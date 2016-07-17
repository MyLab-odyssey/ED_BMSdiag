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
//! \file    canDiag.h
//! \brief   Library module for retrieving diagnostic data.
//! \date    2016-July
//! \author  My-Lab-odyssey
//! \version 0.1.0
//--------------------------------------------------------------------------------
#ifndef CANDIAG_H
#define CANDIAG_H

#define DO_DEBUG_UPATE    //!< Uncomment to show DEBUG output

#ifndef DO_DEBUG_UPDATE
#define DEBUG_UPDATE(...)
#else
#define DEBUG_UPDATE(...) Serial.print(__VA_ARGS__)
#endif

//#include <Arduino.h>
#include <mcp_can.h>
//#include <SPI.h>
#include <Timeout.h>
#include <Average.h>
#include "BMS_dfs.h"

//Data arrays and structures
static byte data[DATALENGTH]; 

typedef Average <unsigned int> myAverage;
static myAverage CellVoltage(CELLCOUNT);
static myAverage CellCapacity(CELLCOUNT);

//CAN-Bus declarations
static unsigned long rxID;
static byte len = 0;
static byte rxLength = 0;
static byte rxBuf[8];
static byte rqFC_length = 8;   //!< Interval to send flow control messages (rqFC) 
static byte rqFlowControl[8] = {0x30, 0x08, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

class canDiag { 
 
private:
    MCP_CAN *myCAN0;
    CTimeout *myCAN_Timeout;

    unsigned long rqID;
    unsigned long respID;
        
    unsigned int Request_Diagnostics(const byte* rqQuery);
    unsigned int Get_RequestResponse();
    boolean Read_FC_Response(int items);
    void PrintReadBuffer(unsigned int lines);
    void ClearReadBuffer();

    void ReadBatteryTemperatures(BatteryDiag_t *myBMS, byte data_in[], unsigned int highOffset, unsigned int length);
    void ReadCellCapacity(byte data_in[], unsigned int highOffset, unsigned int length);
    void ReadCellVoltage(byte data_in[], unsigned int highOffset, unsigned int length);
    void ReadDiagWord(unsigned int data_out[], byte data_in[], unsigned int highOffset, unsigned int length);
  
public:  
    //Average<unsigned int> *CellVoltages;
    //Average<unsigned int> *CellCapacities;

    canDiag();
    ~canDiag(); 
      

//--------------------------------------------------------------------------------
//! \brief   General functions MCP2515 controller
//--------------------------------------------------------------------------------
    void begin(MCP_CAN *myCAN0, CTimeout *myCAN_TimeoutObj);  
    void clearCAN_Filter();
    void setCAN_Filter(unsigned long filter);
    void setCAN_ID(unsigned long _respID);
    void setCAN_ID(unsigned long _rqID, unsigned long _respID);

//--------------------------------------------------------------------------------
//! \brief   Get methods for BMS data
//--------------------------------------------------------------------------------
    boolean getBatteryTemperature(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryDate(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryRevision(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getHVstatus(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getIsolationValue(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryCapacity(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryCapInit(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryCapLoss(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryVoltage(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryAmps(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryADCref(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getHVcontactorState(BatteryDiag_t *myBMS, boolean debug_verbose);

    unsigned int getCellVoltage(byte n);
    unsigned int getCellCapacity(byte n);

//--------------------------------------------------------------------------------
//! \brief   Read BMS values from CAN-Bus traffic
//--------------------------------------------------------------------------------
    boolean ReadSOC(BatteryDiag_t *myBMS);
    boolean ReadSOCinternal(BatteryDiag_t *myBMS);
    boolean ReadPower(BatteryDiag_t *myBMS);
    boolean ReadHV(BatteryDiag_t *myBMS);
    boolean ReadLV(BatteryDiag_t *myBMS);
    boolean ReadODO(BatteryDiag_t *myBMS);
    boolean ReadTime(BatteryDiag_t *myBMS);

};

#endif // #ifndef CANDIAG_H

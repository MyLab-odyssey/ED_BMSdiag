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
//! \file    canDiag.h
//! \brief   Library module for retrieving diagnostic data.
//! \date    2017-December
//! \author  MyLab-odyssey
//! \version 1.0.2
//--------------------------------------------------------------------------------
#ifndef CANDIAG_H
#define CANDIAG_H

//#define DO_DEBUG_UPATE           //!< Uncomment to show DEBUG output

#ifndef DO_DEBUG_UPDATE
#define DEBUG_UPDATE(...)
#else
#define DEBUG_UPDATE(...) Serial.print(__VA_ARGS__)
#endif

#define VERBOSE_ENABLE 1         //!< Local verbose mode enable to allow output of CAN messages

#include <mcp_can.h>
#include <Timeout.h>
#include <AvgNew.h>
#include "_BMS_dfs.h"
#include "_NLG6_dfs.h"
#include "_CS_dfs.h"
#include "_DRV_dfs.h"

class canDiag { 
 
private:
    MCP_CAN *myCAN0;
    CTimeout *myCAN_Timeout;

    byte *data;
    Average CellVoltage;
    Average CellCapacity;

    int _getFreeRam();

    //CAN-Bus declarations
    unsigned long rxID;
    byte len = 0;
    byte rxLength = 0;
    byte rxBuf[8];
    byte rqFlowControl[8] = {0x30, 0x08, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte rqFC_length = 8;   //!< Interval to send flow control messages (rqFC) 
    byte rqWakeUp[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //byte rqWakeUp[1] = {0x00};

    unsigned long rqID;
    unsigned long respID;
    uint16_t SkipStart;
    uint16_t SkipEnd;
    boolean SkipEnable = false;
        
    uint16_t Request_Diagnostics(const byte* rqQuery);
    uint16_t Get_RequestResponse();
    boolean Read_FC_Response(int16_t items);
    void PrintReadBuffer(uint16_t lines);

    void ReadBatteryTemperatures(BatteryDiag_t *myBMS, byte data_in[], uint16_t highOffset, uint16_t length);
    void ReadCellCapacity(byte data_in[], uint16_t highOffset, uint16_t length);
    void ReadCellVoltage(byte data_in[], uint16_t highOffset, uint16_t length);
    void ReadDiagWord(uint16_t data_out[], byte data_in[], uint16_t highOffset, uint16_t length);
  
public:  
    canDiag();
    ~canDiag(); 

    void reserveMem_CellVoltage();
    void reserveMem_CellCapacity();
    void freeMem_CellVoltage();
    void freeMem_CellCapacity();

    boolean WakeUp();

    boolean ReadCAN(BatteryDiag_t *myBMS, unsigned long _rxID);
    boolean ReadCAN(DriveStats_t *myDRV, unsigned long _rxID);
    
//--------------------------------------------------------------------------------
//! \brief   General functions MCP2515 controller
//--------------------------------------------------------------------------------
    void begin(MCP_CAN *myCAN0, CTimeout *myCAN_TimeoutObj);  
    void clearCAN_Filter();
    boolean ClearReadBuffer();
    void setCAN_Filter(unsigned long filter);
    void setCAN_ID(unsigned long _respID);
    void setCAN_ID(unsigned long _rqID, unsigned long _respID);
    void setCAN_Filter_DRV();

//--------------------------------------------------------------------------------
//! \brief   Get methods for BMS data
//--------------------------------------------------------------------------------
    boolean getBatteryTemperature(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryDate(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryRevision(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryVIN(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getHVstatus(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getIsolationValue(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryCapacity(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryVoltage(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryVoltageDist(BatteryDiag_t *myBMS);
    boolean getBatteryAmps(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryADCref(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getHVcontactorState(BatteryDiag_t *myBMS, boolean debug_verbose);
    boolean getBatteryExperimentalData(BatteryDiag_t *myBMS, boolean debug_verbose);

    boolean getCarVIN(BatteryDiag_t *myBMS, boolean debug_verbose);

    uint16_t getCellVoltage(byte n);
    uint16_t getCellCapacity(byte n);

//--------------------------------------------------------------------------------
//! \brief   Get methods for NLG6 charger data
//--------------------------------------------------------------------------------
    boolean NLG6ChargerInstalled(ChargerDiag_t *myNLG6, boolean debug_verbose);
    boolean printNLG6ChargerSWrev(ChargerDiag_t *myNLG6, boolean debug_verbose);
    boolean getChargerTemperature(ChargerDiag_t *myNLG6, boolean debug_verbose);
    boolean getChargerSelCurrent(ChargerDiag_t *myNLG6, boolean debug_verbose);
    boolean getChargerVoltages(ChargerDiag_t *myNLG6, boolean debug_verbose);
    boolean getChargerAmps(ChargerDiag_t *myNLG6, boolean debug_verbose);

//--------------------------------------------------------------------------------
//! \brief   Get methods for cooling- and other subsystems
//--------------------------------------------------------------------------------
    boolean getCoolingAndSubsystems(CoolingSub_t *myCLS, boolean debug_verbose);

//--------------------------------------------------------------------------------
//! \brief   Read BMS values from CAN-Bus traffic
//--------------------------------------------------------------------------------
    boolean ReadSOC(BatteryDiag_t *myBMS);
    boolean ReadSOCinternal(BatteryDiag_t *myBMS);
    boolean ReadPower(BatteryDiag_t *myBMS);
    boolean ReadAmps(BatteryDiag_t *myBMS);
    boolean ReadHV(BatteryDiag_t *myBMS);
    boolean ReadLV(BatteryDiag_t *myBMS);
    boolean ReadODO(BatteryDiag_t *myBMS);
    boolean ReadTime(BatteryDiag_t *myBMS);
    
//--------------------------------------------------------------------------------
//! \brief   Evaluate values
//--------------------------------------------------------------------------------
    boolean CalcPower(BatteryDiag_t *myBMS);

//--------------------------------------------------------------------------------
//! \brief   Read DRV values from CAN-Bus traffic
//--------------------------------------------------------------------------------
    boolean ReadECO(DriveStats_t *myDRV);
    boolean ReadVelocity(DriveStats_t *myDRV);
    boolean ReadRange(DriveStats_t *myDRV);
    boolean ReadEnergyConsumption(DriveStats_t *myDRV);
    boolean ReadUserCounter(DriveStats_t *myDRV);
};

#endif // of #ifndef CANDIAG_H

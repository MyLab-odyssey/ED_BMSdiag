#ifndef CANDIAG_H
#define CANDIAG_H

//#define DO_DEBUG_UPATE

#ifndef DO_DEBUG_UPDATE
#define DEBUG_UPDATE(...)
#else
#define DEBUG_UPDATE(...) Serial.print(__VA_ARGS__)
#endif

#include <Arduino.h> 
#include <mcp_can.h>
#include <SPI.h>
#include <Timeout.h>
#include <Average.h>
#include "BMS_dfs.h"

typedef Average <unsigned int> myAverage;

static myAverage CellVoltage(93);
static myAverage CellCapacity(93);

//Data arrays
static byte data[DATALENGTH]; 

//CAN-Bus declarations
static long unsigned int rxID;
static byte len = 0;
static byte rxLength = 0;
static byte rxBuf[8];
static byte rqFC_length = 8;            //!< Interval to send flow control messages (rqFC) 
static byte rqFlowControl[8] = {0x30, 0x08, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

class canDiag { 

    
private:
    MCP_CAN *myCAN0;
    CTimeout *myCAN_Timeout;
  
public:  
//Average<unsigned int> *CellVoltages;
//Average<unsigned int> *CellCapacities;

    canDiag();
    ~canDiag(); 
      
    void begin(MCP_CAN *myCAN0, CTimeout *myCAN_TimeoutObj);
    
    void clearCAN_Filter();
    void setCAN_Filter(unsigned long filter);

    unsigned int Request_Diagnostics(const byte* rqQuery);
    unsigned int Get_RequestResponse();
    boolean Read_FC_Response(int items);
    void PrintReadBuffer(unsigned int lines);
    void ClearReadBuffer();

    void ReadBatteryTemperatures(BatteryDiag_t *myBMS, byte data_in[], unsigned int highOffset, unsigned int length);
    void ReadCellCapacity(byte data_in[], unsigned int highOffset, unsigned int length);
    void ReadCellVoltage(byte data_in[], unsigned int highOffset, unsigned int length);
    void ReadDiagWord(unsigned int data_out[], byte data_in[], unsigned int highOffset, unsigned int length);

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
    
    void getBMSdata(BatteryDiag_t *myBMS);

    
    boolean ReadSOC(BatteryDiag_t *myBMS);
    boolean ReadSOCinternal(BatteryDiag_t *myBMS);
    boolean ReadPower(BatteryDiag_t *myBMS);
    boolean ReadHV(BatteryDiag_t *myBMS);
    boolean ReadLV(BatteryDiag_t *myBMS);
    boolean ReadODO(BatteryDiag_t *myBMS);
    boolean ReadTime(BatteryDiag_t *myBMS);
 



};

#endif // #ifndef CANDIAG_H
